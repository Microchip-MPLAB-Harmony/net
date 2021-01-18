/*******************************************************************************
  User Datagram Protocol (UDP) Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides unreliable, minimum latency transport of application 
     datagram (packet) oriented data
    -Reference: RFC 768
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2020 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_UDP

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_UDP)


#include "udp_private.h"


/****************************************************************************
  Section:
	UDP Global Variables
  ***************************************************************************/
 

// Store an array of information pertaining to each UDP socket
static UDP_SOCKET_DCPT** UDPSocketDcpt = 0; 

static int          nUdpSockets = 0;    // number of sockets in the current UDP configuration
static const void*  udpMemH = 0;        // memory handle
static int          udpInitCount = 0;   // initialization counter

static uint16_t     udpDefTxSize;               // default size of the TX buffer

#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
static SINGLE_LIST  udpPacketPool = { 0 };  // private pool of UDP packets

static uint16_t     udpPacketsInPool = 0;       // number of packets in the pool

static uint16_t     udpPoolPacketSize = 0;      // size of packets
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)


static tcpipSignalHandle    signalHandle = 0;


// user threads protection semaphore
static OSAL_SEM_HANDLE_TYPE userSem;

#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
static TCPIP_UDP_PACKET_HANDLER udpPktHandler = 0;
static const void* udpPktHandlerParam;
#endif  // (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)

/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/

// The User threads protection
// For efficiency reasons, there is NO PROTECTION for each API call except Open and Close sockets
// What it means is that:
//  - the user application has to close all its sockets before issuing
//    a stack/if down command
//    The stack manager takes care of the internally used sockets
//  - A socket cannot be used concurrently from multiple threads!
//    It's ok to pass a socket from one thread to another as long as
//    there's is no access from more than one thread at a time
//
static __inline__ OSAL_RESULT  __attribute__((always_inline))   _UserGblLockCreate(void)
{
    // create the shared Data Lock
    return OSAL_SEM_Create(&userSem, OSAL_SEM_TYPE_BINARY, 1, 1);
}    

static __inline__ void  __attribute__((always_inline))          _UserGblLockDelete(void)
{
    OSAL_SEM_Delete(&userSem);
}    

// locks access to shared resources
static __inline__ void  __attribute__((always_inline))          _UserGblLock(void)
{
    // Shared Data Lock
    OSAL_SEM_Pend(&userSem, OSAL_WAIT_FOREVER);
}    

// unlocks access to shared resources
static __inline__ void  __attribute__((always_inline))          _UserGblUnlock(void)
{
    // Shared Data unlock
    OSAL_SEM_Post(&userSem);
}

// following is the implementation for the RX thread lock
// This a per socket lock (not efficient to have a global lock
// to disable the whole reception process)

// locks/disables the socket RX process
// to protect between user threads and RX thread
// It protects:
//  - socket resources used for matching RX packets:
//      1. localPort
//      2. addType
//      3. remotePort, flags.looseRemPort
//      4. pSktNet, flags.looseNetIf
//      5. pktSrcAddress, flags.looseRemAddress
//  - socket RX queue (which is r/w accessed by both user threads and the RX thread)
//    Note that this approach doesn't implement a real lock (semaphore/mutex) on the
//    socket RX queue (pretty expensive) but uses the socket RX disable feature

static __inline__ void  __attribute__((always_inline))      _RxSktLock(UDP_SOCKET_DCPT* pSkt)
{
    pSkt->extFlags.rxEnable = 0;
}

// unlocks/enables the socket RX process
static __inline__ void  __attribute__((always_inline))      _RxSktUnlock(UDP_SOCKET_DCPT* pSkt)
{
    pSkt->extFlags.rxEnable = 1;
}

static __inline__ bool  __attribute__((always_inline))      _RxSktIsLocked(UDP_SOCKET_DCPT* pSkt)
{
    return (pSkt->extFlags.rxEnable == 0);
}

// protected access to the socket RX queue
static TCPIP_UDP_SIGNAL_FUNCTION _RxSktQueueAddLocked(UDP_SOCKET_DCPT* pSkt, void* pNode, const void** pSigParam)
{
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler = 0;
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_Helper_SingleListTailAdd(&pSkt->rxQueue, (SGL_LIST_NODE*)pNode);
    if((pSkt->sigMask & TCPIP_UDP_SIGNAL_RX_DATA) != 0)
    {
        sigHandler = pSkt->sigHandler;
        *pSigParam = pSkt->sigParam;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return sigHandler;
}

static void*        _RxSktQueueRemoveLocked(UDP_SOCKET_DCPT* pSkt)
{
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    SGL_LIST_NODE* pNode = TCPIP_Helper_SingleListHeadRemove(&pSkt->rxQueue);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);


    return pNode;
}


#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
// protected access to the global udpPacketPool
static void     _PoolAddNodeLocked(void* pPkt)
{
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_Helper_SingleListTailAdd(&udpPacketPool, (SGL_LIST_NODE*)pPkt);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
}

static SGL_LIST_NODE* _PoolRemoveNodeLocked(void)
{
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    SGL_LIST_NODE* pNode = TCPIP_Helper_SingleListHeadRemove(&udpPacketPool);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    return pNode;
}
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)


#if (_TCPIP_IPV4_FRAGMENTATION != 0)
static void _UDP_RxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
#else
static __inline__ void __attribute__((always_inline)) _UDP_RxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);
}
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

static void     _UDPClose(UDP_SOCKET_DCPT* pSkt);
static void     _UDPFreeTxResources(UDP_SOCKET_DCPT* pSkt);
static void     _UDPFreeRxQueue(UDP_SOCKET_DCPT* pSkt);

// returns the source port of a UDP RX packet
// it assumes that the packet was properly received and processed
// (i.e. endianess already converted)
static __inline__ uint16_t __attribute__((always_inline)) _UDPRxPktSourcePort(TCPIP_MAC_PACKET* pRxPkt)
{
    return ((UDP_HEADER*)pRxPkt->pTransportLayer)->SourcePort;
}

static void _UDPResetRxPacket(UDP_SOCKET_DCPT* pSkt, TCPIP_MAC_PACKET* pRxPkt)
{
    if((pSkt->pCurrRxPkt = pRxPkt) != 0)
    {
        UDP_HEADER* pUDPHdr = (UDP_HEADER*)pRxPkt->pTransportLayer;
        pSkt->pCurrRxSeg = pRxPkt->pDSeg;
        pSkt->rxSegLen = pRxPkt->pDSeg->segLen - sizeof(UDP_HEADER);
        pSkt->rxTotLen = pUDPHdr->Length;  
        pSkt->rxCurr = pRxPkt->pTransportLayer + sizeof(UDP_HEADER);
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
        pSkt->pCurrFrag = pRxPkt;
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)
    }
    else
    {
        pSkt->pCurrRxSeg = 0;
        pSkt->rxSegLen = 0;
        pSkt->rxTotLen = 0;
        pSkt->rxCurr = 0;
    }
}

static void _UDPsetPacketInfo(UDP_SOCKET_DCPT* pSkt, TCPIP_MAC_PACKET* pRxPkt)
{

#if defined (TCPIP_STACK_USE_IPV4)
    if(pRxPkt != 0 && pSkt->addType == IP_ADDRESS_TYPE_IPV4)
    {
        uint32_t pktSrcAdd = TCPIP_IPV4_PacketGetSourceAddress(pRxPkt)->Val;
        pSkt->pktSrcAddress.Val = pktSrcAdd;
        if(pSkt->extFlags.ignoreSrcAdd == 0 && pSkt->flags.destSet == 0)
        {
            pSkt->destAddress.Val = pktSrcAdd;
        }
        uint32_t pktDestAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt)->Val;
        pSkt->pktDestAddress.Val = pktDestAdd;
        if(pSkt->flags.srcSet == 0)
        {   // try to reply to the sender; change the source address, if not forced
            bool isMcastAdd = TCPIP_Helper_IsMcastAddress((IPV4_ADDR*)&pktDestAdd);
            bool isBcastAdd = TCPIP_STACK_IsBcastAddress((TCPIP_NET_IF*)pRxPkt->pktIf, (IPV4_ADDR*)&pktDestAdd);
            if(!isBcastAdd && !isMcastAdd)
            {   // don't reply to a bcast/mcast address; user has to take action
                if(pSkt->srcAddress.Val != pktDestAdd)
                {
                    pSkt->flags.srcSolved = 0;
                    pSkt->srcAddress.Val = pktDestAdd;
                }
                pSkt->flags.srcValid = 1;
            }
        }
        pSkt->pSktNet = (TCPIP_NET_IF*)pRxPkt->pktIf;    // bind it
        if(pSkt->extFlags.ignoreSrcPort == 0 )
        {
            pSkt->remotePort = _UDPRxPktSourcePort(pRxPkt); 
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
    if(pRxPkt != 0 && pSkt->addType == IP_ADDRESS_TYPE_IPV6)
    {
        if(pSkt->flags.destSet == 0)
        {
            TCPIP_IPV6_DestAddressSet(pSkt->pV6Pkt, TCPIP_IPV6_PacketGetSourceAddress(pRxPkt));
        }
        if(pSkt->flags.srcSet == 0)
        {
            pSkt->flags.srcSolved = 0;
            pSkt->flags.srcValid = 1;
            TCPIP_IPV6_SourceAddressSet(pSkt->pV6Pkt, TCPIP_IPV6_PacketGetDestAddress(pRxPkt));
        }
        pSkt->pSktNet = (TCPIP_NET_IF*)pRxPkt->pktIf;    // bind it
        pSkt->remotePort = _UDPRxPktSourcePort(pRxPkt); 
        pSkt->pV6Pkt->netIfH = (TCPIP_NET_IF*)pRxPkt->pktIf;
        TCPIP_IPV6_PacketIPProtocolSet (pSkt->pV6Pkt);
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    // else leave the old source/dest values in place

}

static void _UDPSetNewRxPacket(UDP_SOCKET_DCPT* pSkt, TCPIP_MAC_PACKET* pRxPkt)
{

    if(pSkt->pCurrRxPkt != 0)
    {   // acknowledge the old one
        _UDP_RxPktAcknowledge(pSkt->pCurrRxPkt, TCPIP_MAC_PKT_ACK_RX_OK);
    }

    _UDPResetRxPacket(pSkt, pRxPkt);
    _UDPsetPacketInfo(pSkt, pRxPkt);
}


// lock protects access to RX queue
// and makes sure the RX thread doesn't kick in
static void _UDPUpdatePacketLock(UDP_SOCKET_DCPT* pSkt)
{
    // extract RX packet
    TCPIP_MAC_PACKET* pNextPkt = (TCPIP_MAC_PACKET*)_RxSktQueueRemoveLocked(pSkt);
    _UDPSetNewRxPacket(pSkt, pNextPkt);
}

static void _UDPUpdatePacket(UDP_SOCKET_DCPT* pSkt)
{
    // extract RX packet
    TCPIP_MAC_PACKET* pNextPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&pSkt->rxQueue);
    _UDPSetNewRxPacket(pSkt, pNextPkt);
}

static UDP_SOCKET_DCPT*  _UDPFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, UDP_HEADER *h, IP_ADDRESS_TYPE addressType);

static bool             _UDPTxPktValid(UDP_SOCKET_DCPT * pSkt);

#if defined (TCPIP_STACK_USE_IPV4)
static void*            _UDPv4AllocateSktTxBuffer(UDP_SOCKET_DCPT* pSkt, IP_ADDRESS_TYPE addType, bool update);
static bool             _UDPv4TxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param);
static uint16_t         _UDPv4IsTxPutReady(UDP_SOCKET_DCPT* pSkt);
static uint16_t         _UDPv4Flush(UDP_SOCKET_DCPT* pSkt);
static void*            _TxSktGetLockedV4Pkt(UDP_SOCKET_DCPT* pSkt, bool clrSktPkt);
static void             _UDPv4TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV4_PACKET* pPkt);
static TCPIP_MAC_PKT_ACK_RES TCPIP_UDP_ProcessIPv4(TCPIP_MAC_PACKET* pRxPkt);
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)
static void*             _UDPAllocateTxPacket(uint16_t pktSize, uint16_t txBuffSize, TCPIP_MAC_PACKET_FLAGS allocFlags);
#endif  // defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)


#if defined (TCPIP_STACK_USE_IPV6)
static IPV6_PACKET*     _UDPv6AllocateTxPacketStruct (TCPIP_NET_IF * pNetIf, UDP_SOCKET_DCPT * socket, bool update);
static bool             _UDPv6TxAckFnc (void* pkt, bool success, const void * param);
static bool             _UDPv6TxMacAckFnc (TCPIP_MAC_PACKET* pkt, const void * param);
static uint16_t         _UDPv6IsTxPutReady(UDP_SOCKET_DCPT* pSkt, unsigned short count);
static uint16_t         _UDPv6Flush(UDP_SOCKET_DCPT* pSkt);
static void             _UDPv6FreePacket(IPV6_PACKET* pPkt);
static void             _UDPResetHeader(UDP_HEADER * h);
static void*            _TxSktGetLockedV6Pkt(UDP_SOCKET_DCPT* pSkt, bool clear);
static void             _UDPv6TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV6_PACKET* pPkt, void* pUpperLayer);
static TCPIP_MAC_PKT_ACK_RES TCPIP_UDP_ProcessIPv6(TCPIP_MAC_PACKET* pRxPkt);
#endif  // defined (TCPIP_STACK_USE_IPV6)

static UDP_PORT         _UDPAllocateEphemeralPort(void);
static bool             _UDPIsAvailablePort(UDP_PORT port);

static void             TCPIP_UDP_Process(void);

static UDP_SOCKET       _UDPOpen(IP_ADDRESS_TYPE addType, UDP_OPEN_TYPE opType, UDP_PORT port, IP_MULTI_ADDRESS* address);

#if (TCPIP_STACK_DOWN_OPERATION != 0) || (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void _UDPAbortSockets(uint32_t netMask, TCPIP_UDP_SIGNAL_TYPE sigType); 
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0) || (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

static bool             _UDPSetSourceAddress(UDP_SOCKET_DCPT* pSkt, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* localAddress)
{

    if(localAddress == 0)
    {   // nothing to set
        return false;
    }

    while(pSkt->addType == addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != 0)
            {
               TCPIP_IPV6_SourceAddressSet(pSkt->pV6Pkt, &localAddress->v6Add);
               return true;
            }
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (addType == IP_ADDRESS_TYPE_IPV4)
        {
            pSkt->srcAddress.Val = localAddress->v4Add.Val;
            pSkt->flags.srcSet = 1;
            pSkt->flags.srcValid = 1;
            pSkt->flags.srcSolved = 0;
            return true;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        break;
    }

    return false;

}

static  bool _UDPSocketBind(UDP_SOCKET_DCPT* pSkt, TCPIP_NET_IF* pNet, IP_MULTI_ADDRESS* srcAddress)
{
    if((pSkt->pSktNet = pNet) != 0)
    {   // specific bind requested
        pSkt->flags.looseNetIf = 0;
    }
#if defined (TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == IP_ADDRESS_TYPE_IPV6)
    {
        if(pSkt->pV6Pkt != 0)
        {
            pSkt->pV6Pkt->netIfH = pNet;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    return _UDPSetSourceAddress(pSkt, pSkt->addType, srcAddress);
}



/*static __inline__*/static  void /*__attribute__((always_inline))*/ _UDPSocketTxSet(UDP_SOCKET_DCPT* pSkt,  void* pTxPkt, uint8_t* txBuff, IP_ADDRESS_TYPE addType)
{
    pSkt->txStart = txBuff;
    pSkt->txEnd = txBuff + pSkt->txSize;
    pSkt->txWrite = txBuff;
    pSkt->addType =  addType;
    pSkt->pPkt = pTxPkt;
}

// returns the associated socket descriptor, if such a socket is valid
/*static __inline__*/static  UDP_SOCKET_DCPT* /*__attribute__((always_inline))*/ _UDPSocketDcpt(UDP_SOCKET s)
{
    if(s >= 0 && s < nUdpSockets)
    {
       return UDPSocketDcpt[s];
    }

    return 0;
}

/****************************************************************************
  Section:
	Connection Management Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	void TCPIP_UDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_UDP_MODULE_CONFIG* pUdpInit)

  Summary:
	Initializes the UDP module.

  Description:
	Initializes the UDP module.  This function initializes all the UDP 
	sockets to the closed state.

  Precondition:
	If passed as a parameter, the used stack heap should be initialized

  Parameters:
    stackCtrl  - pointer to a Stack initialization structure

    pUdpInit    - pointer to a UDP initialization structure containing:
                    - nSockets:         number of sockets to be created
                    - sktTxBuffSize:    size of the TX buffer
                    - poolBuffers:      number of buffers in the pool; 0 if none
                    - poolBufferSize:   size of the buffers in the pool; all equal    

  Returns:
  	true if success,
    false otherwise
  	
  Remarks:
	This function is called only once per interface
    but it actually performs initialization just once

    At the time of the call the stack manager makes sure that:
    - the call occurs from within one thread only (protection against user threads)
    - the RX (dispatch packets ) and the TX MAC threads
      are locked and not running (this avoids deadlock if the init/deinit
      function uses RX and/or TX process resources)

   The users should not call the stack/interface initialization
   from multiple threads simultaneously.
      
  ***************************************************************************/

bool TCPIP_UDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_UDP_MODULE_CONFIG* pUdpInit)
{
    UDP_SOCKET_DCPT** newSktDcpt; 
    
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface start up
        return true;    // do not store per interface data
    }
    
    // stack start up
    if(udpInitCount != 0)
    {   // initialize just once
        udpInitCount++;
        return true;
    }
    
    if(stackCtrl->memH == 0)
    {
        SYS_ERROR(SYS_ERROR_ERROR, "UDP NULL dynamic allocation handle");
        return false;
    }

    // check configuration data not missing
    if(pUdpInit == 0)
    {
        return false;
    }

    // create the locks
    if(_UserGblLockCreate() != OSAL_RESULT_TRUE)
    {
        return false;
    }

    // register the task function
    signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_UDP_Task, 0);
    if(signalHandle == 0)
    {
        _UserGblLockDelete();
        return false;
    }

    newSktDcpt = (UDP_SOCKET_DCPT**)TCPIP_HEAP_Calloc(stackCtrl->memH, pUdpInit->nSockets, sizeof(UDP_SOCKET_DCPT*));
    if(newSktDcpt == 0)
    {
        SYS_ERROR(SYS_ERROR_ERROR, "UDP Dynamic allocation failed");
        _UserGblLockDelete();
        _TCPIPStackSignalHandlerDeregister(signalHandle);
        return false;
    }
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
    TCPIP_Helper_SingleListInitialize (&udpPacketPool);
    udpPacketsInPool = pUdpInit->poolBuffers;
    udpPoolPacketSize = (pUdpInit->poolBufferSize < UDP_SOCKET_POOL_BUFFER_MIN_SIZE) ? UDP_SOCKET_POOL_BUFFER_MIN_SIZE: pUdpInit->poolBufferSize;
    // allocate the pool; used for IPv4 only for now!!!
    if(udpPacketsInPool)
    {
        TCPIP_MAC_PACKET*   pPkt;
        int ix;

        for(ix = 0; ix < udpPacketsInPool; ix++)
        {
            pPkt = _UDPAllocateTxPacket(sizeof(UDP_V4_PACKET), udpPoolPacketSize, UDP_SOCKET_POOL_BUFFER_FLAG);
            if(pPkt)
            {
                // Note: no protection for this access
                TCPIP_Helper_SingleListTailAdd(&udpPacketPool, (SGL_LIST_NODE*)pPkt);
            }
            else
            {
                break;
            }

        }

        if(ix != udpPacketsInPool)
        {
            udpPacketsInPool = ix;
            SYS_ERROR(SYS_ERROR_WARNING, "UDP Pool allocation failed");
        }
    }
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)

    // have a consistent state for the UDP module
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    udpMemH = stackCtrl->memH;
    nUdpSockets = pUdpInit->nSockets;
    udpDefTxSize = pUdpInit->sktTxBuffSize;
    UDPSocketDcpt = newSktDcpt;
#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
    udpPktHandler = 0;
#endif  // (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
    udpInitCount++;

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    // allow user access
    _UserGblUnlock();
    
    return true;
}

/*****************************************************************************
  Function:
	void TCPIP_UDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  Summary:
	De-Initializes the UDP module.

  Description:
	De-Initializes the UDP module.
    This function initializes each socket to the CLOSED state.
    If dynamic memory was allocated for the UDP sockets, the function
	will deallocate it.

  Precondition:
	TCPIP_UDP_Initialize() should have been called

  Parameters:
	stackCtrl   - pointer to Stack data showing which interface is closing

  Returns:
    None
    
  Remarks:
    At the time of the call the stack manager makes sure that:
    - the call occurs from within one thread only (protection against user threads)
    - the RX (dispatch packets ) and the TX MAC threads
      are locked and not running (this avoids deadlock if the init/deinit
      function uses RX and/or TX process resources)

   The users should not call the stack/interface de-initialization
   from multiple threads simultaneously.
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_UDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    UDP_SOCKET_DCPT*    pSkt;
    int ix;

    if(udpInitCount == 0)
    {   // not properly initialized
        return;
    }

    // we're up and running
    bool    killSem = false;

    _UserGblLock();     // make sure no one is opening/closing sockets now

    // interface is going down
    _UDPAbortSockets(1 << stackCtrl->netIx, TCPIP_UDP_SIGNAL_IF_DOWN); 

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
    {   // stack shut down
        if(--udpInitCount == 0)
        {   // all closed
            // release resources
            // just in case there are any not bound sockets
            for(ix = 0; ix < nUdpSockets; ix++)
            {
                pSkt = UDPSocketDcpt[ix];
                if(pSkt) 
                {
                    _UDPClose(pSkt);
                }
            }

            TCPIP_HEAP_Free(udpMemH, UDPSocketDcpt);

            UDPSocketDcpt = 0;

#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
            // Note: no protection for this access
            TCPIP_MAC_PACKET*   pPkt;
            while((pPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&udpPacketPool)) != 0)
            {
                TCPIP_PKT_PacketFree(pPkt);
            }
            udpPacketsInPool = 0;
            udpPoolPacketSize = 0;
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)

            if(signalHandle)
            {
                _TCPIPStackSignalHandlerDeregister(signalHandle);
                signalHandle = 0;
            }

            udpMemH = 0;
            nUdpSockets = 0;
            killSem = true;
        }
    }
    
    if(killSem)
    {
        _UserGblLockDelete();
    }
    else
    {
        _UserGblUnlock();
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (TCPIP_STACK_DOWN_OPERATION != 0) || (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void _UDPAbortSockets(uint32_t netMask, TCPIP_UDP_SIGNAL_TYPE sigType)
{
    int ix;
    TCPIP_NET_HANDLE sktNet;
    UDP_SOCKET_DCPT* pSkt;

    TCPIP_UDP_SIGNAL_FUNCTION sigHandler;
    const void*     sigParam;
    uint16_t        sigMask;


    for(ix = 0; ix < nUdpSockets; ix++)
    {
        if((pSkt = UDPSocketDcpt[ix]) != 0)  
        {
            uint32_t sktIfMask = 1 << TCPIP_STACK_NetIxGet(pSkt->pSktNet);
            if((sktIfMask & netMask) != 0)
            {   // match
                sktNet = pSkt->pSktNet; 
                // just disconnect, don't kill sockets
                TCPIP_UDP_Disconnect(pSkt->sktIx, true);
                // get a consistent reading
                OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                sigHandler = pSkt->sigHandler;
                sigParam = pSkt->sigParam;
                sigMask = pSkt->sigMask;
                OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

                if(sigHandler != 0 && (sigMask & sigType) != 0)
                {
                    (*sigHandler)(pSkt->sktIx, sktNet, sigType, sigParam);
                }
            }
        }
    }
} 

#endif  // (TCPIP_STACK_DOWN_OPERATION != 0) || (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

UDP_SOCKET TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE addType, UDP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)
{
    return TCPIP_UDP_OpenServerSkt(addType, localPort, localAddress, UDP_OPEN_SERVER);
}

UDP_SOCKET TCPIP_UDP_OpenServerSkt(IP_ADDRESS_TYPE addType, UDP_PORT localPort,  IP_MULTI_ADDRESS* localAddress, UDP_OPEN_TYPE opType)
{
    UDP_SOCKET  skt;
    TCPIP_NET_IF* pDefIf = 0;   // default: unbound
   
#if !defined (TCPIP_STACK_USE_IPV6)
   if(addType == IP_ADDRESS_TYPE_IPV6)
   {
       return INVALID_SOCKET;
   } 
   else
   {
       addType = IP_ADDRESS_TYPE_IPV4;
   }
#endif  // defined (TCPIP_STACK_USE_IPV6)
    
#if !defined (TCPIP_STACK_USE_IPV4)
   if(addType == IP_ADDRESS_TYPE_IPV4)
   {
       return INVALID_SOCKET;
   }
   else
   {
       addType = IP_ADDRESS_TYPE_IPV6;
   }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV4)
   if(addType == IP_ADDRESS_TYPE_IPV4 && localAddress != 0)
   {
       if(localAddress->v4Add.Val == 0)
       { // ignore
           localAddress = 0;
       }
       else
       {
           pDefIf = TCPIP_STACK_IPAddToNet(&localAddress->v4Add, false);
           if(pDefIf == 0)
           {    // no such interface
               return INVALID_UDP_SOCKET;
           }
       }
   }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
   if(addType == IP_ADDRESS_TYPE_IPV6 && localAddress != 0)
   {
       pDefIf = _TCPIPStackIPv6AddToNet(&localAddress->v6Add, IPV6_ADDR_TYPE_UNICAST, false);
       if(pDefIf == 0)
       {    // no such interface
           return INVALID_UDP_SOCKET;
       }
   }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    
    skt = _UDPOpen(addType, opType, localPort, 0);
    if(skt != INVALID_UDP_SOCKET)
    {
        UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt[skt];
        _UDPSocketBind(pSkt, pDefIf, localAddress);
        if(pDefIf != 0)
        {
            pSkt->flags.openBindIf = 1;
        }
        if(localAddress != 0)
        {
            pSkt->flags.openBindAdd = 1;
        }
        // safely enable RX
        _RxSktUnlock(pSkt);
    }

    return skt;
}

UDP_SOCKET TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE addType, UDP_PORT remotePort, IP_MULTI_ADDRESS* remoteAddress)
{
    return TCPIP_UDP_OpenClientSkt(addType, remotePort, remoteAddress, UDP_OPEN_CLIENT);
}

UDP_SOCKET TCPIP_UDP_OpenClientSkt(IP_ADDRESS_TYPE addType, UDP_PORT remotePort, IP_MULTI_ADDRESS* remoteAddress, UDP_OPEN_TYPE opType)
{
    UDP_SOCKET  skt;

#if !defined (TCPIP_STACK_USE_IPV6)
    if(addType == IP_ADDRESS_TYPE_IPV6)
    {
        return INVALID_SOCKET; 
    }
    else
    {
        addType = IP_ADDRESS_TYPE_IPV4;
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if !defined (TCPIP_STACK_USE_IPV4)
    if(addType == IP_ADDRESS_TYPE_IPV4)
    {
        return INVALID_SOCKET; 
    }
    else
    {
        addType = IP_ADDRESS_TYPE_IPV6;
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

    skt = _UDPOpen(addType, opType, remotePort, remoteAddress);
    if(skt != INVALID_UDP_SOCKET)
    {
        UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt[skt];
#if defined (TCPIP_STACK_USE_IPV6)
        if(addType == IP_ADDRESS_TYPE_IPV6)
        {   // IPv6 clients require explicit binding; use the default interface; user can change the binding
            _UDPSocketBind(pSkt, (TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet(), 0);
            pSkt->flags.openBindIf = 1;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV6)
        // safely enable RX
        _RxSktUnlock(pSkt);
    }

    return skt;
}



static UDP_SOCKET _UDPOpen(IP_ADDRESS_TYPE addType, UDP_OPEN_TYPE opType, UDP_PORT port, IP_MULTI_ADDRESS* hostAddress)
{
    int        sktIx = -1;
    UDP_SOCKET_DCPT *pSkt = NULL;
    UDP_PORT   localPort, remotePort;
    bool       newSktValid; 

    if(UDPSocketDcpt == 0)
    {
        return (UDP_SOCKET)INVALID_UDP_SOCKET;
    }

    if((opType & UDP_OPEN_CLIENT) != 0)
    {
        localPort = 0;
        remotePort = port;
    }
    else
    {
        localPort = port;
        remotePort = 0;
    }

    // critical section: allocation of new port and of new socket 
    // needs locked access
    newSktValid = false;
    _UserGblLock();
    while(true)
    {
        if(localPort == 0)
        {
            localPort = _UDPAllocateEphemeralPort();
            if(localPort == 0)
            {   // couldn't allocate a new port
                break;
            }
        }

        // search for an empty socket slot
        pSkt = (UDP_SOCKET_DCPT*)-1;
        for ( sktIx = 0; sktIx < nUdpSockets; sktIx++ )
        {
            pSkt = UDPSocketDcpt[sktIx];
            if(pSkt == 0)
            {   // found an empty slot; sktIx is the valid slot index
                break;
            }
        }

        if(pSkt != 0)
        {   // all slots taken;
            break;
        }

        // success; following expensive/blocking allocation operation done with
        // all sockets locked!
        // Otherwise the socket would not be properly constructed and
        // a de-init/close operation now would be a disaster! 
        pSkt = (UDP_SOCKET_DCPT*)TCPIP_HEAP_Calloc(udpMemH, 1, sizeof(*pSkt));
        newSktValid = pSkt != 0;

        break;
    }

    if(newSktValid)
    {   // mark slot as valid; continue initialization
        pSkt->sktIx = sktIx;
        UDPSocketDcpt[sktIx] = pSkt;
    }

    // end of critical
    _UserGblUnlock();

    if(!newSktValid)
    {
        return (UDP_SOCKET)INVALID_UDP_SOCKET;
    }

    // fill in all the socket parameters
    // so that the RX thread can see all the right data
    pSkt->localPort = localPort;	
    pSkt->remotePort = remotePort;
    pSkt->addType = addType;
    pSkt->txAllocLimit = TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT; 
    pSkt->rxQueueLimit = TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT;
    pSkt->flags.openAddType = addType;

    if((opType & UDP_OPEN_TX_SPLIT) != 0)
    {
        pSkt->txSize = 0;
        pSkt->flags.txSplitAlloc = 1;
    }
    else
    {
        pSkt->txSize = udpDefTxSize;
    }

    if((opType & UDP_OPEN_CONFIG_SERVICE) != 0)
    {
        pSkt->flags.stackConfig = 1;
    }

    // For IPv4 we postpone the allocation until the user wants to write something.
    // This allows RX only server sockets, that don't take extra memory.
    // Not possible for IPv6. It will have to rely on TCPIP_UDP_OptionsSet!

#if defined (TCPIP_STACK_USE_IPV6)
    if (addType == IP_ADDRESS_TYPE_IPV6)
    {
        _UDPv6AllocateTxPacketStruct (0, pSkt, true);
        if(pSkt->pPkt == 0)
        {   // out of memory
            _UserGblLock(); 
            _UDPClose(pSkt);
            _UserGblUnlock();
            return INVALID_UDP_SOCKET;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)


    if((opType & UDP_OPEN_SERVER) != 0)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            TCPIP_IPV6_DestAddressSet(pSkt->pV6Pkt,(IPV6_ADDR *)&IPV6_FIXED_ADDR_ALL_NODES_MULTICAST);
            TCPIP_IPV6_SetRemoteMacAddress(pSkt->pV6Pkt, &IPV6_MULTICAST_MAC_ADDRESS);
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        // default non strict connections for server
        pSkt->flags.looseRemPort = pSkt->flags.looseNetIf = pSkt->flags.looseRemAddress = 1; 
        pSkt->extFlags.serverSkt = 1;
    }
    else
    {   // UDP_OPEN_CLIENT
        switch(addType)
        {
#if defined (TCPIP_STACK_USE_IPV4)
            case IP_ADDRESS_TYPE_IPV4:
                // hostAddress is a literal IP address.
                if(hostAddress)
                {
                    pSkt->destAddress.Val = hostAddress->v4Add.Val;
                    pSkt->flags.destSet = 1;
                }
                break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
            case IP_ADDRESS_TYPE_IPV6:
                if(hostAddress)
                {
                    TCPIP_IPV6_DestAddressSet (pSkt->pV6Pkt, &hostAddress->v6Add);
                }
                break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

            default:
                break;
        }
    }

    return (UDP_SOCKET)sktIx;
}



// allocates a packet from the packet heap
#if defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)
static void* _UDPAllocateTxPacket(uint16_t pktSize, uint16_t txBuffSize, TCPIP_MAC_PACKET_FLAGS allocFlags)
{
    allocFlags |= TCPIP_MAC_PKT_FLAG_TX | TCPIP_MAC_PKT_FLAG_UDP;
    
    return TCPIP_PKT_SocketAlloc(pktSize, sizeof(UDP_HEADER), txBuffSize, allocFlags);
}
#endif // defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)

void TCPIP_UDP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

#if (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
    uint32_t  netMask;

    sigPend = _TCPIPStackModuleSignalParamGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL, &netMask);

    if((sigPend & (TCPIP_MODULE_SIGNAL_INTERFACE_CHANGE)) != 0)
    { // interface address change occurred
        _UDPAbortSockets(netMask, TCPIP_UDP_SIGNAL_IF_CHANGE); 
    }
#else
    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);
#endif  // (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_UDP_Process();
    }

}

static void TCPIP_UDP_Process(void)
{
    TCPIP_MAC_PACKET*   pRxPkt;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    // extract queued UDP packets
    while((pRxPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
        if(udpPktHandler != 0)
        {
            bool was_processed = (*udpPktHandler)(pRxPkt->pktIf, pRxPkt, udpPktHandlerParam);
            if(was_processed)
            {
                TCPIP_PKT_FlightLogAcknowledge(pRxPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_EXTERN);
                continue;
            }
        }
#endif  // (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)

        ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
        if(pRxPkt->totTransportLen < sizeof(UDP_HEADER))
        {
            ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
        }

#if defined (TCPIP_STACK_USE_IPV4)
        else if((pRxPkt->pktFlags & TCPIP_MAC_PKT_FLAG_NET_TYPE) == TCPIP_MAC_PKT_FLAG_IPV4) 
        {
            ackRes = TCPIP_UDP_ProcessIPv4(pRxPkt);
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
        else if((pRxPkt->pktFlags & TCPIP_MAC_PKT_FLAG_NET_TYPE) == TCPIP_MAC_PKT_FLAG_IPV6) 
        {
            ackRes = TCPIP_UDP_ProcessIPv6(pRxPkt);
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {   // unknown/error; discard it.
            _UDP_RxPktAcknowledge(pRxPkt, ackRes);
        }
    }
}



#if defined (TCPIP_STACK_USE_IPV4)
static void* _UDPv4AllocateSktTxBuffer(UDP_SOCKET_DCPT* pSkt, IP_ADDRESS_TYPE addType, bool update)
{
    TCPIP_MAC_PACKET_FLAGS allocFlags;
    TCPIP_MAC_PACKET*   pPkt;
    uint8_t*            pTxBuff;
    uint16_t            pktSize;

    if(addType != IP_ADDRESS_TYPE_IPV4)
    {   // IPv4 allocation only
        return 0;
    }

    if(pSkt->txAllocCnt >= pSkt->txAllocLimit)
    {   // cannot try to allocate any more packets
        return 0;
    }

    // allocate IPv4 packet
    pPkt = 0;
    allocFlags = TCPIP_MAC_PKT_FLAG_IPV4;
    if(pSkt->flags.stackConfig != 0)
    {
        allocFlags |= TCPIP_MAC_PKT_FLAG_CONFIG;
    }

    if(pSkt->flags.txSplitAlloc == 0)
    {
        pktSize = sizeof(UDP_V4_PACKET);
        // allocate from pool, if possible
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
        if(pSkt->flags.usePool != 0 && pSkt->txSize <= udpPoolPacketSize)
        {
            pPkt = (TCPIP_MAC_PACKET*)_PoolRemoveNodeLocked();
            if(pPkt != 0)
            {
                pPkt->pktFlags = allocFlags | UDP_SOCKET_POOL_BUFFER_FLAG;
                pPkt->next = 0;
                pPkt->pDSeg->segLen = 0;
            }
        }
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)
    }
    else
    {
        allocFlags |= TCPIP_MAC_PKT_FLAG_SPLIT;
        pktSize = sizeof(UDP_V4_PACKET) + sizeof(*((UDP_V4_PACKET*)0)->zcSeg);
    }

    if(pPkt == 0)
    {   // allocate from main packet pool
        pPkt = _UDPAllocateTxPacket(pktSize, pSkt->txSize, allocFlags);
    }

    if(pPkt)
    {
        TCPIP_PKT_PacketAcknowledgeSet(pPkt, _UDPv4TxAckFnc, pSkt);

        if(pSkt->flags.txSplitAlloc)
        {   // link the 2 segments
            TCPIP_MAC_DATA_SEGMENT* pZSeg = ((UDP_V4_PACKET*)pPkt)->zcSeg;
            pPkt->pDSeg->next = pZSeg;
            pZSeg->segFlags = TCPIP_MAC_SEG_FLAG_STATIC;   // embedded in packet itself
            pTxBuff = 0;    // will be set as external payload
        }
        else
        {
            pTxBuff = pPkt->pTransportLayer + sizeof(UDP_HEADER);
        }

        if(update)
        {
            _UDPSocketTxSet(pSkt, pPkt, pTxBuff, addType);
        }
        pSkt->txAllocCnt++;
    }


    return pPkt;
}

static bool _UDPv4TxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param)
{
    TCPIP_NET_HANDLE pktIf = 0;
    UDP_SOCKET   sktIx = 0;
    UDP_SOCKET_DCPT* pSkt = (UDP_SOCKET_DCPT*)param;
    bool critLock = false;
    bool freePkt = true;
    bool    loopPkt = false;
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler = 0;
    const void* sigParam = 0;

    OSAL_CRITSECT_DATA_TYPE status = 0;

	while(pSkt != 0)
    {  
        // make sure the user threads don't mess with the socket right now
        status =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        sktIx = pSkt->sktIx;
        if(UDPSocketDcpt == 0 || sktIx < 0 || sktIx >= nUdpSockets || pSkt != UDPSocketDcpt[sktIx])
        {   // no longer this socket?
            pSkt = 0;
            break;
        }

        if(pPkt->modPktData != 0)
        {   // redirect internally. once!
            pPkt->modPktData = 0;
            loopPkt = true;
            freePkt = false;
            break;
        }

        if((pSkt->sigMask & TCPIP_UDP_SIGNAL_TX_DONE) != 0)
        {
            sigHandler = pSkt->sigHandler;
            sigParam = pSkt->sigParam;
            pktIf = pPkt->pktIf;
        }
        if(pSkt->pV4Pkt != (IPV4_PACKET*)pPkt)
        {   // no longer using this packet;
            break;
        }

        // OK, keep the current packet
        _UDPv4TxPktReset(pSkt, (IPV4_PACKET*)pPkt);
        freePkt = false;

        break;
    }

    if(freePkt && pSkt)
    {
        pSkt->txAllocCnt--;
    }

    if(critLock)
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }


    if(loopPkt)
    {
        pPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
        TCPIP_PKT_FlightLogTxSkt(pPkt, TCPIP_THIS_MODULE_ID, ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort, pSkt->sktIx);
        _TCPIPStackInsertRxPacket((TCPIP_NET_IF*)pPkt->pktIf, pPkt, true);
        return false;
    }

    if(freePkt)
    {   // discard packet
        // either closed socket or another packet already allocated
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
        if((pPkt->pktFlags & UDP_SOCKET_POOL_BUFFER_FLAG) != 0 && udpPacketsInPool != 0)
        {   // re-insert in the pool
            _PoolAddNodeLocked(pPkt);
        }
        else
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)
        {
            TCPIP_PKT_PacketFree(pPkt);
        }
    }

    if(sigHandler)
    {   // notify socket user
        (*sigHandler)(sktIx, pktIf, TCPIP_UDP_SIGNAL_TX_DONE, sigParam);

    }

    return false;
}

static void _UDPv4TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV4_PACKET* pPkt)
{
    pPkt->macPkt.pDSeg->segLen = 0;
    if(pSkt->flags.txSplitAlloc)
    {
        ((UDP_V4_PACKET*)pPkt)->zcSeg->segLen = 0;
    }
    else
    {
        pSkt->txWrite = pSkt->txStart;
    }
    pPkt->macPkt.pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;

}

// following is the implementation for the TX thread lock
// This a global lock that needs to be very fast

// Disables the pre-emption during the socket changes in the TX process
// to protect between user threads and the TX thread
// It protects:
//  - the socket TX buffer resources which are updated by the user threads
//    and the TX thread via the _UDPvxTxAckFnc
//
// protects user vs TX threads when the user needs to allocate a new TX packet for a socket
// returns:
//  - valid pointer: the packet is no longer in use and can be safely modified
//  - 0: a new packet needs to be allocated and safely updated
//  Note: there's no corresponding unlock function!
static void*  _TxSktGetLockedV4Pkt(UDP_SOCKET_DCPT* pSkt, bool clrSktPkt)
{
    IPV4_PACKET* pPkt;
    
    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    
    if((pPkt = pSkt->pV4Pkt) != 0)
    {   // we have a current TX packet
        if((pPkt->macPkt.pktFlags & TCPIP_MAC_PKT_FLAG_QUEUED) != 0)
        {   // queued, cannot use it
            pPkt = 0;
            if(clrSktPkt)
            {   // clear it so the TX thread deletes it
                pSkt->pV4Pkt = 0;
            }
        }
        // else not queued, could safely use it
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return pPkt;
}


bool TCPIP_UDP_BcastIPV4AddressSet(UDP_SOCKET s, UDP_SOCKET_BCAST_TYPE bcastType, TCPIP_NET_HANDLE hNet)
{
    UDP_SOCKET_DCPT *pSkt;
    IPV4_ADDR       bcastAddress;

    pSkt = _UDPSocketDcpt(s);
    if(pSkt == 0 || pSkt->addType != IP_ADDRESS_TYPE_IPV4)
    {
        return false;
    }

    if(pSkt->flags.bcastForceType != UDP_BCAST_NONE)
    {   // BCAST is already set and cannot be overridden!
        return false;
    }


    switch (bcastType)
    {
        case UDP_BCAST_NETWORK_LIMITED:
            bcastAddress.Val = 0xffffffff;
            break;


        case UDP_BCAST_NETWORK_DIRECTED:
            if((bcastAddress.Val = TCPIP_STACK_NetAddressBcast(hNet)) == 0)
            {   // interface down/unknown?
                return false;
            }
            break;


        default:
            return false;
    }


    // set broadcast address
    pSkt->destAddress.Val = bcastAddress.Val;
    pSkt->flags.destSet = 1;

    return true;
}

static uint16_t _UDPv4IsTxPutReady(UDP_SOCKET_DCPT* pSkt)
{
    void* pPkt = _TxSktGetLockedV4Pkt(pSkt, true);

    if(pPkt == 0)
    {   // packet is in one of the queues; allocate another tx space
        if(_UDPv4AllocateSktTxBuffer(pSkt, IP_ADDRESS_TYPE_IPV4, true) == 0)
        {   // allocation failed; caller will have to retry later
            return 0;
        }
        // else the new packet data is updated in place
    }

    if(pSkt->flags.txSplitAlloc == 0)
    {
        return pSkt->txEnd - pSkt->txWrite;
    }

    // return max segment possible
    return 1514 - sizeof(TCPIP_MAC_ETHERNET_HEADER) - sizeof(IPV4_HEADER) - sizeof(UDP_HEADER);
}

// Handles an incoming UDP v4 packet.
static TCPIP_MAC_PKT_ACK_RES TCPIP_UDP_ProcessIPv4(TCPIP_MAC_PACKET* pRxPkt)
{
    UDP_HEADER*		 pUDPHdr;
    UDP_SOCKET_DCPT* pSkt;
    uint16_t         udpTotLength;
    const IPV4_ADDR* pPktSrcAdd;
    const IPV4_ADDR* pPktDstAdd;
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler;
    const void*      sigParam;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    pUDPHdr = (UDP_HEADER*)pRxPkt->pTransportLayer;
    udpTotLength = TCPIP_Helper_ntohs(pUDPHdr->Length);

#if (_TCPIP_IPV4_FRAGMENTATION == 0)
    if(udpTotLength != pRxPkt->totTransportLen)
    {   // discard suspect packet
        return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
    }
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

    pPktSrcAdd = TCPIP_IPV4_PacketGetSourceAddress(pRxPkt);
    pPktDstAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);
	// See if we need to validate the checksum field (0x0000 is disabled)
#ifdef TCPIP_UDP_USE_RX_CHECKSUM
	if((pUDPHdr->Checksum != 0))
	{
        IPV4_PSEUDO_HEADER  pseudoHdr;
        uint16_t            calcChkSum;
	    // Calculate IP pseudoheader checksum.
	    pseudoHdr.SourceAddress.Val = pPktSrcAdd->Val;
	    pseudoHdr.DestAddress.Val = pPktDstAdd->Val;
	    pseudoHdr.Zero	= 0;
	    pseudoHdr.Protocol = IP_PROT_UDP;
	    pseudoHdr.Length = pUDPHdr->Length;

	    calcChkSum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHdr, sizeof(pseudoHdr), 0);
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
        TCPIP_MAC_PACKET* pFragPkt;
        uint16_t totCalcUdpLen = 0;
        for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
        {
            calcChkSum = ~TCPIP_Helper_PacketChecksum(pFragPkt, pFragPkt->pTransportLayer, pFragPkt->totTransportLen, calcChkSum);
            totCalcUdpLen += pFragPkt->totTransportLen;
        }
        calcChkSum = ~calcChkSum;

        if(udpTotLength != totCalcUdpLen)
        {   // discard suspect packet
            return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
        }
#else
        if((pRxPkt->pktFlags & TCPIP_MAC_PKT_FLAG_SPLIT) != 0)
        {
            calcChkSum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pUDPHdr, udpTotLength, calcChkSum);
        }
        else
        {
            calcChkSum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pUDPHdr, udpTotLength, calcChkSum);
        }
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

        if(calcChkSum != 0)
        {   // discard packet
            return TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        }
	}
#endif // TCPIP_UDP_USE_RX_CHECKSUM

    pUDPHdr->SourcePort = TCPIP_Helper_ntohs(pUDPHdr->SourcePort);
    pUDPHdr->DestinationPort = TCPIP_Helper_ntohs(pUDPHdr->DestinationPort);
    pUDPHdr->Length = udpTotLength - sizeof(UDP_HEADER);    

    while(true)
    {
        pSkt = _UDPFindMatchingSocket(pRxPkt, pUDPHdr, IP_ADDRESS_TYPE_IPV4);
        if(pSkt == 0)
        {
            // If there is no matching socket, There is no one to handle
            // this data.  Discard it.
            ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
            break;
        }

#if defined(TCPIP_STACK_USE_IGMP)    
        if(pSkt->extFlags.mcastSkipCheck == 0)
        {   // don't skip check multicast traffic
            if(TCPIP_Helper_IsMcastAddress(pPktDstAdd))
            {   // need to check
                if(!TCPIP_IGMP_IsMcastEnabled(pSkt->sktIx, pRxPkt->pktIf, *pPktDstAdd, *pPktSrcAdd))
                {   // don't let it through
                    ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
                    break;
                }
            }
        }
#endif  // defined(TCPIP_STACK_USE_IGMP)    
    
        if(pSkt->extFlags.mcastOnly != 0)
        {   // let through multicast traffic only
            if(!TCPIP_Helper_IsMcastAddress(pPktDstAdd))
            {   // don't let it through
                ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
                break;
            }
        }

        // insert valid packet in the RX queue
        sigHandler = _RxSktQueueAddLocked(pSkt, pRxPkt, &sigParam);
        if(sigHandler)
        {   // notify socket user
            (*sigHandler)(pSkt->sktIx, pRxPkt->pktIf, TCPIP_UDP_SIGNAL_RX_DATA, sigParam);
        }

        // everything OK, pass to user
        ackRes = TCPIP_MAC_PKT_ACK_NONE;
        break;
    }


    // log 
#if (TCPIP_PACKET_LOG_ENABLE)
    uint32_t logPort = (pSkt != 0) ? ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort : ((uint32_t)pUDPHdr->DestinationPort << 16) | pUDPHdr->SourcePort;
    TCPIP_PKT_FlightLogRxSkt(pRxPkt, TCPIP_MODULE_LAYER3, logPort, pSkt != 0 ? pSkt->sktIx: 0xffff);
#endif  // (TCPIP_PACKET_LOG_ENABLE)

    return ackRes;

}

static uint16_t _UDPv4Flush(UDP_SOCKET_DCPT* pSkt)
{
    IPV4_PACKET*        pv4Pkt;
    uint16_t            udpLoadLen, udpTotLen, rootLen;
    UDP_HEADER*         pUDPHdr;
    IPV4_PSEUDO_HEADER  pseudoHdr;
    uint16_t            checksum;
    TCPIP_MAC_DATA_SEGMENT* pZSeg;
    TCPIP_IPV4_PACKET_PARAMS pktParams;
    bool                isMcastDest;

    if(pSkt->destAddress.Val == 0)
    {   // don't even bother
        return 0;
    }

    if(pSkt->flags.srcSolved == 0 || pSkt->pSktNet == 0)
    {
        pSkt->pSktNet = (TCPIP_NET_IF*)TCPIP_IPV4_SelectSourceInterface(pSkt->pSktNet, &pSkt->destAddress, &pSkt->srcAddress, pSkt->flags.srcValid != 0);
        if(pSkt->pSktNet == 0)
        {   // cannot find an route?
            return 0;
        }
        pSkt->flags.srcSolved = 1;
        pSkt->flags.srcValid = 1;
    }

    if(pSkt->flags.bcastForceType == UDP_BCAST_NETWORK_DIRECTED)
    {   // have to adjust for this interface
        pSkt->destAddress.Val = TCPIP_STACK_NetAddressBcast(pSkt->pSktNet);
    }

    pv4Pkt = pSkt->pV4Pkt;
    pv4Pkt->srcAddress.Val = pSkt->srcAddress.Val;
    pv4Pkt->destAddress.Val = pSkt->destAddress.Val;
    pv4Pkt->netIfH = pSkt->pSktNet;

    // start preparing the UDP header and packet
    pUDPHdr = (UDP_HEADER*)pv4Pkt->macPkt.pTransportLayer;

    // update the current load length
    if(pSkt->flags.txSplitAlloc != 0)
    {
        pZSeg = ((UDP_V4_PACKET*)pv4Pkt)->zcSeg;
        udpLoadLen = pZSeg->segLen;
        rootLen = sizeof(UDP_HEADER);
        // size of the payload should already be set
    }
    else
    {
        udpLoadLen = pSkt->txWrite - pSkt->txStart;
        rootLen = udpLoadLen + sizeof(UDP_HEADER); 
        pZSeg = 0;
    }
    pv4Pkt->macPkt.pDSeg->segLen += rootLen;
    udpTotLen = udpLoadLen + sizeof(UDP_HEADER);

    pUDPHdr->SourcePort = TCPIP_Helper_htons(pSkt->localPort);
    pUDPHdr->DestinationPort = TCPIP_Helper_htons(pSkt->remotePort);
    pUDPHdr->Length = TCPIP_Helper_htons(udpTotLen);
    pUDPHdr->Checksum = 0;

    // add the pseudo header
    pseudoHdr.SourceAddress.Val = pv4Pkt->srcAddress.Val;
    pseudoHdr.DestAddress.Val = pv4Pkt->destAddress.Val;
    pseudoHdr.Zero = 0;
    pseudoHdr.Protocol = IP_PROT_UDP;
    pseudoHdr.Length = pUDPHdr->Length;
    checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHdr, sizeof(pseudoHdr), 0);

    if(pSkt->flags.txSplitAlloc != 0)
    {
        checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)pUDPHdr, sizeof(UDP_HEADER), checksum);
        checksum = ~TCPIP_Helper_CalcIPChecksum(pZSeg->segLoad, udpLoadLen, checksum);
    }
    else
    {   // one contiguous buffer
        checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)pUDPHdr, udpTotLen, checksum);
    }

    pUDPHdr->Checksum = ~checksum;

    isMcastDest = TCPIP_Helper_IsMcastAddress(&pv4Pkt->destAddress);
    if(isMcastDest)
    {
        pktParams.ttl = pSkt->ttl == 0 ? UDP_MULTICAST_DEFAULT_TTL : pSkt->ttl;
    }
    else
    {
        pktParams.ttl = pSkt->ttl;
    }

    pktParams.tosFlags = pSkt->extFlags.tos;
    pktParams.df = pSkt->extFlags.df;

    // and we're done
    TCPIP_IPV4_PacketFormatTx(pv4Pkt, IP_PROT_UDP, udpTotLen, &pktParams);
    pv4Pkt->macPkt.next = 0;    // single packet

    if(isMcastDest && pSkt->extFlags.mcastLoop)
    {
        pv4Pkt->macPkt.modPktData = 1;
    }
    else
    {
        pv4Pkt->macPkt.modPktData = 0;
    }

    TCPIP_PKT_FlightLogTxSkt(&pv4Pkt->macPkt, TCPIP_THIS_MODULE_ID,  ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort, pSkt->sktIx);
    if(TCPIP_IPV4_PacketTransmit(pv4Pkt))
    {
        return udpLoadLen; 
    }

    // packet reuse
    TCPIP_PKT_FlightLogAcknowledge(&pv4Pkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
    _UDPv4TxPktReset(pSkt, pv4Pkt);
    
    return 0;
}

#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
static IPV6_PACKET * _UDPv6AllocateTxPacketStruct (TCPIP_NET_IF * pNetIf, UDP_SOCKET_DCPT * pSkt, bool update)
{
    IPV6_PACKET * pkt;
    uint8_t*     txBuffer;

    if(pSkt->txAllocCnt >= pSkt->txAllocLimit)
    {   // cannot try to allocate any more packets
        return 0;
    }

    pkt = TCPIP_IPV6_TxPacketAllocate (pNetIf, _UDPv6TxAckFnc, pSkt);

    if (pkt == 0)
    {
        return 0;
    }

    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, NULL, sizeof (UDP_HEADER), IP_PROT_UDP, UDP_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree (pkt);
        return 0;
    }

    // initialize the packet
    TCPIP_IPV6_SourceAddressSet(pkt, 0);
    TCPIP_IPV6_DestAddressSet(pkt, 0);
    TCPIP_IPV6_SetPacketMacAcknowledge(pkt, _UDPv6TxMacAckFnc);

    txBuffer = (uint8_t*)TCPIP_HEAP_Malloc(udpMemH, pSkt->txSize);
    if(txBuffer == 0)
    {
        TCPIP_IPV6_PacketFree (pkt);
        return 0;
    }

    if(update)
    {
        _UDPSocketTxSet(pSkt, pkt, txBuffer, IP_ADDRESS_TYPE_IPV6);
    }

    pSkt->txAllocCnt++;
    pkt->clientData = txBuffer;
    return pkt;
}

static void _UDPv6FreePacket(IPV6_PACKET* pkt)
{
    if(pkt->clientData != 0)
    {
        TCPIP_HEAP_Free(udpMemH, pkt->clientData);
        pkt->clientData = 0;
    }
    TCPIP_IPV6_PacketFree ((IPV6_PACKET *)pkt);
}

static bool _UDPv6TxAckFnc (void* pkt, bool success, const void * param)
{
    UDP_SOCKET   sktIx;
    IPV6_PACKET*    pV6Pkt = (IPV6_PACKET*)pkt;
    UDP_SOCKET_DCPT* pSkt = (UDP_SOCKET_DCPT*)param;
    bool critLock = false;
    bool freePkt = true;

    OSAL_CRITSECT_DATA_TYPE status = 0;

	while(pSkt != 0)
    {  
        // make sure the user threads don't mess with the socket right now
        status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        sktIx = pSkt->sktIx;
        if(UDPSocketDcpt == 0 || sktIx < 0 || sktIx >= nUdpSockets || pSkt != UDPSocketDcpt[sktIx])
        {   // no longer this socket?
            pSkt = 0;
            break;
        }

        if(pSkt->pV6Pkt != pV6Pkt)
        {   // no longer using this packet;
            break;
        }

        // OK, keep the current packet
        _UDPv6TxPktReset(pSkt, pV6Pkt, 0);
        freePkt = false;

        break;
    }

    if(freePkt && pSkt)
    {
        pSkt->txAllocCnt--;
    }

    if(critLock)
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }

    if(freePkt)
    {   // discard packet
        // either closed socket or another packet already allocated
        _UDPv6FreePacket (pV6Pkt);
    }
    
    return freePkt ? false: true;
    
}

static void _UDPv6TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV6_PACKET* pV6Pkt, void* pUpperLayer)
{
    TCPIP_IPV6_TransmitPacketStateReset (pV6Pkt);
    if(pUpperLayer == 0)
    {
        pUpperLayer = TCPIP_IPV6_UpperLayerHeaderPtrGet(pV6Pkt);
    }

    _UDPResetHeader(pUpperLayer);
    pSkt->txWrite = pSkt->txStart;
}

static bool _UDPv6TxMacAckFnc (TCPIP_MAC_PACKET* pPkt, const void * param)
{
    TCPIP_NET_HANDLE pktIf = 0;
    UDP_SOCKET   sktIx = 0;
    UDP_SOCKET_DCPT* pSkt = (UDP_SOCKET_DCPT*)param;
    bool critLock = false;
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler = 0;
    const void* sigParam = 0;

    OSAL_CRITSECT_DATA_TYPE status = 0;

	while(pSkt != 0)
    {  
        // make sure the user threads don't mess with the socket right now
        status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        if(pSkt != _UDPSocketDcpt(pSkt->sktIx))
        {   // no longer this socket?
            break;
        }

        if((pSkt->sigMask & TCPIP_UDP_SIGNAL_TX_DONE) != 0)
        {
            sigHandler = pSkt->sigHandler;
            sigParam = pSkt->sigParam;
            pktIf = pPkt->pktIf;
            sktIx = pSkt->sktIx;
        }

        break;
    }


    if(critLock)
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }

    TCPIP_PKT_PacketFree(pPkt);
    
    if(sigHandler)
    {   // notify socket user
        (*sigHandler)(sktIx, pktIf, TCPIP_UDP_SIGNAL_TX_DONE, sigParam);

    }

    return false;
}

// same TX protection
// IPv6 variant
// protects user vs TX threads when the user needs to allocate a new TX packet for a socket
// returns:
//  - valid pointer: the packet is no longer in use and can be safely modified
//  - 0: a new packet needs to be allocated and safely updated
//  Note: there's no corresponding unlock function!
static void*  _TxSktGetLockedV6Pkt(UDP_SOCKET_DCPT* pSkt, bool clear)
{
    IPV6_PACKET* pPkt;
    
    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    
    if((pPkt = pSkt->pV6Pkt) != 0)
    {   // we have a current TX packet
        if(pPkt->flags.queued != 0)
        {   // queued, cannot use it
            pPkt = 0;
            if(clear)
            {   // clear it so the TX thread deletes it
                pSkt->pV6Pkt = 0;
            }
        }
        // else not queued, could safely use it
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return pPkt;
}



static uint16_t _UDPv6IsTxPutReady(UDP_SOCKET_DCPT* pSkt, unsigned short count)
{
    bool    newPkt;
    IPV6_PACKET * pkt;

    bool queued = false;
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if((pkt = pSkt->pV6Pkt) != 0)
    {
        queued = pkt->flags.queued != 0;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    if (pkt && !queued)
    {   // packet available
        return pSkt->txEnd - pSkt->txWrite;
    }

    // either no packet or already queued
    // Try to allocate a new transmit packet
    IPV6_PACKET * tempPtr = _UDPv6AllocateTxPacketStruct(pSkt->pSktNet, pSkt, false);
    if (tempPtr == 0)
    {
        // We couldn't allocate a new packet.  Return 0 until we can 
        // or until a queued packet can be returned to this node.
        return 0;
    }

    // copy the old packet info
    if (pkt != 0 && !TCPIP_IPV6_TxPacketStructCopy (tempPtr, pkt))
    {   // failed; leave the old one in place
        _UDPv6FreePacket(tempPtr);
        return 0;
    }

    // now store changes if original packet not yet available
    status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if(pkt != 0 && pkt->flags.queued == 0)
    {   // TX thread just cleared it
        newPkt = false;
    }
    else
    {   // still queued
        _UDPSocketTxSet(pSkt, tempPtr, tempPtr->clientData, IP_ADDRESS_TYPE_IPV6);
        newPkt = true;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    if(newPkt)
    {
        _UDPResetHeader(TCPIP_IPV6_UpperLayerHeaderPtrGet(tempPtr));
    }
    else
    {   // leave the old one in place
        _UDPv6FreePacket(tempPtr);
    }

    return pSkt->txEnd - pSkt->txWrite;
}

static TCPIP_MAC_PKT_ACK_RES TCPIP_UDP_ProcessIPv6(TCPIP_MAC_PACKET* pRxPkt)
{
    UDP_HEADER*         h;
    uint16_t            udpTotLength;
    UDP_SOCKET_DCPT*    pSkt;
    const IPV6_ADDR*    localIP;
    const IPV6_ADDR*    remoteIP;
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler;
    const void*         sigParam;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    if(!TCPIP_IPV6_AddressesGet(pRxPkt, &localIP, &remoteIP))
    {
        return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
    }
    
    // Retrieve UDP header.
    h = (UDP_HEADER*)pRxPkt->pTransportLayer;
    udpTotLength = TCPIP_Helper_ntohs(h->Length);

    if(udpTotLength != pRxPkt->totTransportLen)
    {   // discard suspect packet
        return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
    }


#ifdef TCPIP_UDP_USE_RX_CHECKSUM
    if(h->Checksum != 0)
    {
        IPV6_PSEUDO_HEADER  pseudoHeader;
        uint16_t            calcChkSum;

        // Calculate IP pseudoheader checksum.
        memcpy (&pseudoHeader.SourceAddress, remoteIP, sizeof (IPV6_ADDR));
        memcpy (&pseudoHeader.DestAddress, localIP, sizeof (IPV6_ADDR));
        // Total payload length is the length of data + extension headers
        pseudoHeader.PacketLength = h->Length;
        pseudoHeader.zero1 = 0;
        pseudoHeader.zero2 = 0;
        pseudoHeader.NextHeader = IP_PROT_UDP;

	    calcChkSum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHeader, sizeof(pseudoHeader), 0);
        if((pRxPkt->pktFlags & TCPIP_MAC_PKT_FLAG_SPLIT) != 0)
        {
            calcChkSum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)h, udpTotLength, calcChkSum);
        }
        else
        {
            calcChkSum = TCPIP_Helper_CalcIPChecksum((uint8_t*)h, udpTotLength, calcChkSum);
        }

        if(calcChkSum != 0)
        {   // discard packet
            return TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        }
	}
#endif // TCPIP_UDP_USE_RX_CHECKSUM



    h->SourcePort        = TCPIP_Helper_ntohs(h->SourcePort);
    h->DestinationPort   = TCPIP_Helper_ntohs(h->DestinationPort);
    h->Length            = udpTotLength - sizeof(UDP_HEADER);

    while(true)
    {
        pSkt = _UDPFindMatchingSocket(pRxPkt, h, IP_ADDRESS_TYPE_IPV6);
        if(pSkt == 0)
        {   // Send ICMP Destination Unreachable Code 4 (Port unreachable) and discard packet
            uint16_t headerLen = pRxPkt->ipv6PktData;
            TCPIP_IPV6_ErrorSend ((TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt, localIP, remoteIP, ICMPV6_ERR_DU_PORT_UNREACHABLE, ICMPV6_ERROR_DEST_UNREACHABLE, 0x00000000, udpTotLength + headerLen + sizeof (IPV6_HEADER));

            // If there is no matching socket, There is no one to handle
            // this data.  Discard it.
            ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
            break;
        }

        // insert valid packet in the RX queue
        sigHandler = _RxSktQueueAddLocked(pSkt, pRxPkt, &sigParam);
        if(sigHandler)
        {   // notify socket user
            (*sigHandler)(pSkt->sktIx, pRxPkt->pktIf, TCPIP_UDP_SIGNAL_RX_DATA, sigParam);
        }

        // let it through
        ackRes = TCPIP_MAC_PKT_ACK_NONE;
        break;
    }

#if (TCPIP_PACKET_LOG_ENABLE)
    uint32_t logPort = (pSkt != 0) ? ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort : ((uint32_t)h->DestinationPort << 16) | h->SourcePort;
    TCPIP_PKT_FlightLogRxSkt(pRxPkt, TCPIP_MODULE_LAYER3, logPort, pSkt != 0 ? pSkt->sktIx: 0xffff);
#endif  // (TCPIP_PACKET_LOG_ENABLE)

    
    return ackRes;

}

static uint16_t _UDPv6Flush(UDP_SOCKET_DCPT* pSkt)
{
    TCPIP_NET_IF*   pSktNet;
    uint16_t        wUDPLength;
    UDP_HEADER*     pUDPHeader;

    pSktNet = (TCPIP_NET_IF*)pSkt->pV6Pkt->netIfH;
	if(pSktNet == 0 || !TCPIP_IPV6_InterfaceIsReady(pSktNet))
    {   // IPv6 client socket requires explicit binding
        return 0;
    }

    pSkt->pSktNet = pSktNet;    // make sure they are in sync
    wUDPLength = pSkt->txWrite - pSkt->txStart;
    if(!TCPIP_IPV6_PayloadSet (pSkt->pV6Pkt, pSkt->txStart, wUDPLength))
    {
        return 0;
    }


    TCPIP_IPV6_HeaderPut(pSkt->pV6Pkt, IP_PROT_UDP);

    pUDPHeader = (UDP_HEADER *)TCPIP_IPV6_UpperLayerHeaderPtrGet(pSkt->pV6Pkt);

    pUDPHeader->SourcePort = TCPIP_Helper_htons(pSkt->localPort);
    pUDPHeader->DestinationPort = TCPIP_Helper_htons(pSkt->remotePort);
    pUDPHeader->Length = TCPIP_Helper_htons(wUDPLength + sizeof(UDP_HEADER));

    TCPIP_IPV6_Flush (pSkt->pV6Pkt);

    if (!TCPIP_IPV6_IsPacketQueued(pSkt->pV6Pkt))
    {
        _UDPv6TxPktReset(pSkt, pSkt->pV6Pkt, pUDPHeader);
    }
    return wUDPLength;
}


static void _UDPResetHeader(UDP_HEADER * h)
{
    if (h)
    {
        h->Checksum = 0x0000;
        h->Length = 0x0000;
    }
}

#endif  // defined (TCPIP_STACK_USE_IPV6)


static bool _UDPTxPktValid(UDP_SOCKET_DCPT * pSkt)
{
    switch(pSkt->addType)
    {
#if defined(TCPIP_STACK_USE_IPV6)
        case IP_ADDRESS_TYPE_IPV6:
            return _TxSktGetLockedV6Pkt(pSkt, false) != 0;
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
        case IP_ADDRESS_TYPE_IPV4:
            return _TxSktGetLockedV4Pkt(pSkt, false) != 0;
#endif  // defined (TCPIP_STACK_USE_IPV4)

        default:
            return false;
    }

}

bool TCPIP_UDP_IsConnected(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);
    while(pSkt != 0)
    {
        // if there's a pending packet, update info
        if(pSkt->pCurrRxSeg == 0)
        {   // no more data in this packet 
            _UDPUpdatePacketLock(pSkt);
        }

#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != 0 && pSkt->pV6Pkt->netIfH != 0)
            {
                IPV6_ADDR* pAdd = TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt);
                if(pAdd != 0 && memcmp(pAdd, (IPV6_ADDR *)&IPV6_FIXED_ADDR_UNSPECIFIED, sizeof(IPV6_ADDR)) != 0)
                {
                    return true;
                }
            }

            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == IP_ADDRESS_TYPE_IPV4)
        {
            return pSkt->destAddress.Val != 0;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        break;
        // cannot send data with address ANY
    }

    return false;
}




bool TCPIP_UDP_Close(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt)
    {
        _UserGblLock(); 
        _UDPClose(pSkt);
        _UserGblUnlock();
        return true;
    }

    return false;
}

bool TCPIP_UDP_Disconnect(UDP_SOCKET s, bool flushRxQueue)
{

    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt)
    {
        if(pSkt->flags.openAddType == IP_ADDRESS_TYPE_ANY)
        {   // free the TX resources because next traffic could be different
            _UDPFreeTxResources(pSkt);
        }

        _RxSktLock(pSkt);
        // restore initial settings
        pSkt->addType = (IP_ADDRESS_TYPE)pSkt->flags.openAddType;
        if(pSkt->flags.openBindIf == 0)
        {
            pSkt->pSktNet = 0;
        }

        if(pSkt->flags.openBindAdd == 0)
        {
            pSkt->srcAddress.Val = 0;
            pSkt->flags.srcValid = pSkt->flags.srcSolved = 0;
        }

        if(pSkt->extFlags.serverSkt != 0 && pSkt->flags.looseRemPort != 0)
        {   // stop server connection on the remote port if looseRemPort (rbind was not performed);
            pSkt->remotePort = 0;
        }

        if(flushRxQueue)
        {
            _UDPFreeRxQueue(pSkt);
            _UDPUpdatePacket(pSkt);
        }
        else
        {   // re-read the current packet
            _UDPsetPacketInfo(pSkt, pSkt->pCurrRxPkt);
        }

        _RxSktUnlock(pSkt);
        return true;
    }

    return false;

}


static void _UDPClose(UDP_SOCKET_DCPT* pSkt)
{
    _UDPFreeTxResources(pSkt);
    _RxSktLock(pSkt);
    _UDPFreeRxQueue(pSkt);
    if(pSkt->pCurrRxPkt != 0)
    {   // acknowledge the old one
        _UDP_RxPktAcknowledge(pSkt->pCurrRxPkt, TCPIP_MAC_PKT_ACK_PROTO_DEST_CLOSE);
    }
    UDPSocketDcpt[pSkt->sktIx] = 0;
    TCPIP_HEAP_Free(udpMemH, pSkt);
}

static void _UDPFreeTxResources(UDP_SOCKET_DCPT* pSkt)
{
    void* pCurrPkt = 0;

    switch(pSkt->addType)
    {
#if defined(TCPIP_STACK_USE_IPV6)
        case IP_ADDRESS_TYPE_IPV6:
            
            pCurrPkt = _TxSktGetLockedV6Pkt(pSkt, true);
            if(pCurrPkt != 0)
            {
                _UDPv6FreePacket(pSkt->pV6Pkt);
                pSkt->txStart = 0;
            }
            break;
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
        case IP_ADDRESS_TYPE_IPV4:
            pCurrPkt = _TxSktGetLockedV4Pkt(pSkt, true);
            if(pCurrPkt)
            {
                TCPIP_MAC_PACKET* pPkt = &pSkt->pV4Pkt->macPkt;
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
                if((pPkt->pktFlags & UDP_SOCKET_POOL_BUFFER_FLAG) != 0)
                {   // re-insert in the pool
                    _PoolAddNodeLocked(pPkt);
                }
                else
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)
                {
                    TCPIP_PKT_PacketFree(pPkt);
                }
            }
            break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

        default:
            break;
    }


    if(pCurrPkt != 0)
    {   // count only the not in flight packet
        pSkt->txAllocCnt--;
    }
    pSkt->pPkt = 0;
}

// frees the associated socket RX packet queue
static void _UDPFreeRxQueue(UDP_SOCKET_DCPT* pSkt)
{
    TCPIP_MAC_PACKET*   pRxPkt;

    while((pRxPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&pSkt->rxQueue)) != 0)
    {
        _UDP_RxPktAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_PROTO_DEST_CLOSE);
    }

}

bool TCPIP_UDP_SocketInfoGet(UDP_SOCKET s, UDP_SOCKET_INFO* pInfo)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt == 0 || pInfo == 0)
    {
        return false;
    }

    memset(pInfo, 0, sizeof(*pInfo));

    while(true)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != 0)
            {
                memcpy(&pInfo->remoteIPaddress.v6Add.v, (void*)TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt), sizeof(IPV6_ADDR));
                memcpy(&pInfo->localIPaddress.v6Add.v, (void*)TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt), sizeof(IPV6_ADDR));
                memcpy(&pInfo->sourceIPaddress.v6Add.v, (void*)TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt), sizeof(IPV6_ADDR));
                memcpy(&pInfo->destIPaddress.v6Add.v, (void*)TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt), sizeof(IPV6_ADDR));
            }
            pInfo->addressType = IP_ADDRESS_TYPE_IPV6;
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)


#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == IP_ADDRESS_TYPE_IPV4)
        {
            pInfo->remoteIPaddress.v4Add.Val = pSkt->destAddress.Val;
            pInfo->localIPaddress.v4Add.Val = pSkt->srcAddress.Val;
            pInfo->sourceIPaddress.v4Add.Val = pSkt->pktSrcAddress.Val;
            pInfo->destIPaddress.v4Add.Val = pSkt->pktDestAddress.Val;
            pInfo->addressType = IP_ADDRESS_TYPE_IPV4;
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        pInfo->addressType = IP_ADDRESS_TYPE_ANY;
        break;
    }

	pInfo->remotePort = pSkt->remotePort;
	pInfo->localPort = pSkt->localPort;
    pInfo->hNet = pSkt->pSktNet;
    pInfo->rxQueueSize = TCPIP_Helper_SingleListCount(&pSkt->rxQueue);
    pInfo->txSize = pSkt->txEnd - pSkt->txStart;

	return true;

}

bool TCPIP_UDP_TxOffsetSet(UDP_SOCKET s, uint16_t wOffset, bool relative)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt && _UDPTxPktValid(pSkt))
    {
        uint8_t* pNewWrite = relative ? pSkt->txWrite : pSkt->txStart;
        pNewWrite += wOffset;

        if(pSkt->txStart <= pNewWrite && pNewWrite <= pSkt->txEnd)
        {
            pSkt->txWrite = pNewWrite;
            return true;
        }        
    }

    return false;
}

uint8_t* TCPIP_UDP_TxPointerGet(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt && _UDPTxPktValid(pSkt))
    {
        return pSkt->txWrite;
    }

    return 0;
}

void TCPIP_UDP_RxOffsetSet(UDP_SOCKET s, uint16_t wOffset)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt && pSkt->pCurrRxPkt != 0)
    {
        // set the packet segments from the beginning and discard
        _UDPResetRxPacket(pSkt, pSkt->pCurrRxPkt);
        TCPIP_UDP_ArrayGet(s, 0, wOffset);
    }
}




uint16_t TCPIP_UDP_PutIsReady(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);
    if(pSkt == 0)
    { 
        return 0;
    }

#if defined (TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == IP_ADDRESS_TYPE_IPV6)
    {
        return _UDPv6IsTxPutReady(pSkt, pSkt->txSize);
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
    if(pSkt->addType == IP_ADDRESS_TYPE_IPV4)
    {
        return _UDPv4IsTxPutReady(pSkt);
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

    return 0;   // can happen if it is a server socket and opened with IP_ADDRESS_TYPE_ANY
                // and no client connected to it

}

uint16_t TCPIP_UDP_TxPutIsReady(UDP_SOCKET s, unsigned short count)
{
    return TCPIP_UDP_PutIsReady(s);
}



uint16_t TCPIP_UDP_Put(UDP_SOCKET s, uint8_t v)
{
    return TCPIP_UDP_ArrayPut(s, &v, 1);
}

uint16_t TCPIP_UDP_ArrayPut(UDP_SOCKET s, const uint8_t *cData, uint16_t wDataLen)
{
    if(cData != 0 && wDataLen != 0)
    {
        UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

        if(pSkt != 0 && _UDPTxPktValid(pSkt))
        {
            uint16_t wrSpace = pSkt->txEnd - pSkt->txWrite;
            if(wDataLen > wrSpace)
            {
                wDataLen = wrSpace;
            }

            if(wDataLen)
            {
                memcpy(pSkt->txWrite, cData, wDataLen);
                pSkt->txWrite += wDataLen;
            }

            return wDataLen;
        }
    }

    return 0;
}

const uint8_t* TCPIP_UDP_StringPut(UDP_SOCKET s, const uint8_t *strData)
{
    if(strData)
    {
        UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);
        if(pSkt)
        {
            return strData + TCPIP_UDP_ArrayPut(s, strData, strlen((char*)strData));
        }
    }

    return 0;
}



uint16_t TCPIP_UDP_Flush(UDP_SOCKET s)
{
    uint16_t payload;
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt && _UDPTxPktValid(pSkt))
    {
        if(pSkt->flags.txSplitAlloc == 0)
        {
            payload = pSkt->txWrite - pSkt->txStart;
        }
        else
        {
            payload = ((UDP_V4_PACKET*)pSkt->pPkt)->zcSeg->segLen;
        }

        if(payload)
        {
            switch(pSkt->addType)
            {
#if defined(TCPIP_STACK_USE_IPV6)
                case IP_ADDRESS_TYPE_IPV6:
                    return _UDPv6Flush(pSkt);
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
                case IP_ADDRESS_TYPE_IPV4:
                    return _UDPv4Flush(pSkt);
#endif  // defined (TCPIP_STACK_USE_IPV4)

                default:
                    break;
            }
        }
    }

    return 0;
}


uint16_t TCPIP_UDP_TxCountGet(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt != 0 && _UDPTxPktValid(pSkt))
    {
        return pSkt->txWrite - pSkt->txStart;
    }

    return 0;
}


/****************************************************************************
  Section:
	Receive Functions
  ***************************************************************************/

uint16_t TCPIP_UDP_GetIsReady(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);
    
    if(pSkt == 0)
    { 
        return 0;
    }

    if(pSkt->pCurrRxSeg == 0 || pSkt->rxTotLen == 0)
    {   // no more data in this packet 
        _UDPUpdatePacketLock(pSkt);
    }

    return pSkt->pCurrRxSeg == 0 ? 0 : pSkt->rxTotLen;

}

uint16_t TCPIP_UDP_Get(UDP_SOCKET s, uint8_t *v)
{
    return TCPIP_UDP_ArrayGet(s, v, 1);
}

uint16_t TCPIP_UDP_ArrayGet(UDP_SOCKET s, uint8_t *cData, uint16_t reqBytes)
{
    TCPIP_MAC_DATA_SEGMENT *pSeg;
    uint16_t    xtractBytes;
    uint16_t    avlblBytes;

    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(reqBytes == 0 || pSkt == 0)
    {   // do not advance the current RX packet if no data is requested
        return 0;
    }
    
    if(pSkt->pCurrRxSeg == 0 && pSkt->extFlags.rxAutoAdvance != 0)
    {   // no more data in this packet 
        _UDPUpdatePacketLock(pSkt);
    }

    avlblBytes = 0;
    while(reqBytes != 0 && (pSeg = pSkt->pCurrRxSeg) != 0 && pSkt->rxTotLen != 0)
    {
        xtractBytes = reqBytes <= pSkt->rxSegLen ? reqBytes : pSkt->rxSegLen;
        if(xtractBytes > pSkt->rxTotLen)
        {
            xtractBytes = pSkt->rxTotLen;
        }

        if(xtractBytes)
        {
            if(cData != 0)
            {
                memcpy(cData, pSkt->rxCurr, xtractBytes);
                cData += xtractBytes;
            }
            // adjust
            reqBytes -= xtractBytes;
            avlblBytes += xtractBytes;

            pSkt->rxTotLen -= xtractBytes;
            pSkt->rxSegLen -= xtractBytes;
            pSkt->rxCurr += xtractBytes;
        }

        if(pSkt->rxSegLen == 0)
        {   // go to the next segment in the packet
            pSeg = pSeg->next;
            if((pSkt->pCurrRxSeg = pSeg) != 0)
            {
                pSkt->rxSegLen = pSeg->segLen;
                pSkt->rxCurr = pSeg->segLoad;
            }
            else
            {
                pSkt->rxSegLen = 0;
                pSkt->rxCurr = 0;

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
                TCPIP_MAC_PACKET* pFrag;
                pFrag = pSkt->pCurrFrag = pSkt->pCurrFrag->pkt_next;
                if(pFrag != 0)
                {   // there is another fragment
                    pSkt->pCurrRxSeg = pFrag->pDSeg;
                    pSkt->rxSegLen = pFrag->totTransportLen;
                    pSkt->rxCurr = pFrag->pTransportLayer;
                } 
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)
            }
        }
        // else more data in this segment
    }

    if(pSkt->rxTotLen == 0 && pSkt->extFlags.rxAutoAdvance != 0)
    {   // done with this packet
        _UDPUpdatePacketLock(pSkt);
    }

    return avlblBytes;
}

uint16_t TCPIP_UDP_Discard(UDP_SOCKET s)
{
    uint16_t nBytes = 0;
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    if(pSkt)
    {
        if(pSkt->pCurrRxSeg)
        {
            nBytes = pSkt->rxTotLen;
        }
        _UDPUpdatePacketLock(pSkt);
    }

    return nBytes;
}

/*****************************************************************************
  Function:
	static UDP_SOCKET_DCPT* _UDPFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, UDP_HEADER *h, IP_ADDRESS_TYPE addressType)

  Summary:
	Matches an incoming UDP segment to a currently active socket.
	
  Description:
	This function attempts to match an incoming UDP segment to a currently
	active socket for processing.

  Precondition:
	UDP segment header and IP header have both been retrieved.

  Parameters:
    pRxPkt - packet received containing UDP datagram
	h - The UDP header that was received.
    addressType - IPv4/IPv6
	
  Returns:
  	A UDP_SOCKET_DCPT handle of a matching socket, or 0 when no
  	match could be made.
  ***************************************************************************/
static UDP_SOCKET_DCPT* _UDPFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, UDP_HEADER *h, IP_ADDRESS_TYPE addressType)
{
    int sktIx;
    UDP_SOCKET_DCPT *pSkt;
    TCPIP_NET_IF* pPktIf;
    TCPIP_UDP_PKT_MATCH exactMatch, looseMatch;
    OSAL_CRITSECT_DATA_TYPE critStatus;
    // snapshot of socket settings
    UDP_PORT _localPort = 0, _remotePort = 0;
    uint16_t _addType = 0;
    TCPIP_NET_IF* _pSktNet = 0;
#if defined (TCPIP_STACK_USE_IPV4)
    IPV4_ADDR _pktSrcAddress;
    _pktSrcAddress.Val = 0;
#endif // defined (TCPIP_STACK_USE_IPV4)
    TCPIP_UDP_SKT_FLAGS _flags; 
    _flags.Val = 0;


    // This packet is said to be matching with current socket:
    // 1. Packet destination port matches the socket local port
    // and
    // 2. Packet address type matches the socket address type (IPv4/IPv6) or socket type is IP_ADDRESS_TYPE_ANY 
    // and
    // 3. Packet source port number matches the socket remote port number or the looseRemPort flag is set
    // and
    // 4. Packet incoming network interface matches the socket network interface or looseNetIf flag is set
    // and (IPv4 only for now)
    // 5. packet source address matches the socket expected source address or looseRemAddress flag is set
    

    pPktIf = (TCPIP_NET_IF*)pRxPkt->pktIf;
    for(sktIx = 0; sktIx < nUdpSockets; sktIx++)
    {
        bool processSkt = false;
        critStatus = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        while(true)
        {
            pSkt = UDPSocketDcpt[sktIx];
            if(pSkt == 0) 
            {   // not valid socket
                break;
            }
            if(_RxSktIsLocked(pSkt)) 
            {   // socket disabled
                break;
            }

            if(TCPIP_Helper_SingleListCount(&pSkt->rxQueue) >= pSkt->rxQueueLimit)
            {   // RX limit exceeded
                break;
            }

            // take a snapshot of socket settings
            _localPort = pSkt->localPort;
            _addType = pSkt->addType;
            _remotePort = pSkt->remotePort;
            _flags.Val = pSkt->flags.Val;
            _pSktNet = pSkt->pSktNet;
#if defined (TCPIP_STACK_USE_IPV4)
            _pktSrcAddress.Val = pSkt->pktSrcAddress.Val;
#endif // defined (TCPIP_STACK_USE_IPV4)

            processSkt = true;
            break;
        }
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStatus);

        if(processSkt == false)
        {
            continue;
        }

        // 1. packet destination port
        if(_localPort != h->DestinationPort)
        {   // cannot handle this port
            continue;
        }
       
        exactMatch = looseMatch = 0;

        // 2. packet address type
        if(_addType == addressType)
        {
            exactMatch = TCPIP_UDP_PKT_MATCH_IP_TYPE;
        }
        else if(_addType == IP_ADDRESS_TYPE_ANY)
        {
            looseMatch = TCPIP_UDP_PKT_MATCH_IP_TYPE;
        }
        else
        {   // cannot handle this address type
            continue;
        }

        // 3. packet source port
        if(_remotePort == h->SourcePort)
        {
            exactMatch |= TCPIP_UDP_PKT_MATCH_SRC_PORT;
        }
        else if(_flags.looseRemPort != 0)
        {
            looseMatch |= TCPIP_UDP_PKT_MATCH_SRC_PORT;
        }

        // 4. packet incoming interface
#if defined (TCPIP_STACK_USE_IPV4)
        if(addressType == IP_ADDRESS_TYPE_IPV4)
        {
            if(_pSktNet == pPktIf)
            {
                exactMatch |= TCPIP_UDP_PKT_MATCH_NET;
            }
            else if(_pSktNet == 0 || _flags.looseNetIf != 0)
            {
                looseMatch |= TCPIP_UDP_PKT_MATCH_NET;
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
        if(addressType == IP_ADDRESS_TYPE_IPV6)
        {
            if(_pSktNet == pPktIf)
            {
                if(TCPIP_IPV6_AddressFind(pPktIf, TCPIP_IPV6_PacketGetDestAddress(pRxPkt), IPV6_ADDR_TYPE_UNICAST) != 0)
                {   // interface match
                    exactMatch |= TCPIP_UDP_PKT_MATCH_NET;
                }
            }
            else if(_pSktNet == 0 || _flags.looseNetIf != 0)
            {
                looseMatch |= TCPIP_UDP_PKT_MATCH_NET;
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        // 5. packet source address
#if defined (TCPIP_STACK_USE_IPV4)
        if(addressType == IP_ADDRESS_TYPE_IPV4)
        {
            if(_pktSrcAddress.Val == 0 || _flags.looseRemAddress != 0)
            {
                looseMatch |= TCPIP_UDP_PKT_MACTH_SRC_ADD;
            }
            else if(_pktSrcAddress.Val == TCPIP_IPV4_PacketGetSourceAddress(pRxPkt)->Val)
            {
                exactMatch |= TCPIP_UDP_PKT_MACTH_SRC_ADD;
            }
        }
#endif // defined (TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
        if(addressType == IP_ADDRESS_TYPE_IPV6)
        {
            // no IPv6 check done
            exactMatch |= TCPIP_UDP_PKT_MACTH_SRC_ADD;
        }
#endif // defined(TCPIP_STACK_USE_IPV6)

        // finally check the match we got
        if(exactMatch == TCPIP_UDP_PKT_MACTH_MASK)
        {   // perfect match
            return pSkt;
        }
        else if( (looseMatch | exactMatch) == TCPIP_UDP_PKT_MACTH_MASK )
        {   // overall match; adjust and return
#if defined (TCPIP_STACK_USE_IPV6)
            if (addressType == IP_ADDRESS_TYPE_IPV6)
            {   // lazy allocation does not work for IPv6
                // This is expensive and IPv6 should be able to delay the allocation, like IPv4 does!
                // avoid user threads mess with this
                if(pSkt->pV6Pkt == 0)
                {   // could be a server socket opened with IP_ADDRESS_TYPE_ANY
                    IPV6_PACKET* pNewPkt = _UDPv6AllocateTxPacketStruct(pPktIf, pSkt, false);
                    if(pNewPkt == 0)
                    {   // failed to allocate memory; not much we can do
                        return 0;
                    }

                    // stop the user threads from messing with this socket TX buffer
                    bool useOldPkt = false;
                    critStatus = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                    if(pSkt->pV6Pkt == 0)
                    {   // we can use the new packet
                        _UDPSocketTxSet(pSkt, pNewPkt, pNewPkt->clientData, IP_ADDRESS_TYPE_IPV6);
                    }
                    else
                    {
                        useOldPkt = true;
                    }
                    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStatus);

                    if(useOldPkt)
                    {
                        _UDPv6FreePacket(pNewPkt);
                    }
                }
            }
#endif  // defined (TCPIP_STACK_USE_IPV6)
            
            pSkt->addType = addressType;
            return pSkt;
        }

        // no match, continue
    }

    // not found
    return 0;
}

bool TCPIP_UDP_SocketNetSet(UDP_SOCKET s, TCPIP_NET_HANDLE hNet)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);
    if(pSkt)
    {
        TCPIP_NET_IF* pIf = _TCPIPStackHandleToNetUp(hNet);
        // don't do any check here;
        // user can clear the assigned interface

        _RxSktLock(pSkt);
        if((pSkt->pSktNet = pIf) != 0)
        {   // specific bind requested
            pSkt->flags.looseNetIf = 0;
        }


#if defined (TCPIP_STACK_USE_IPV6)
        if(pSkt->addType == IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt == 0)
            {
                _RxSktUnlock(pSkt);
                return false;
            }

            pSkt->pV6Pkt->netIfH = pIf;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        _RxSktUnlock(pSkt);
        return true;
    }
    return false;
}

TCPIP_NET_HANDLE TCPIP_UDP_SocketNetGet(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);
    
    return pSkt?pSkt->pSktNet:0;
}

// sets the source IP address of a packet
bool TCPIP_UDP_SourceIPAddressSet(UDP_SOCKET s, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* localAddress)
{
    UDP_SOCKET_DCPT* pSkt = _UDPSocketDcpt(s);

    return pSkt ? _UDPSetSourceAddress(pSkt, addType, localAddress) : false;
}

bool TCPIP_UDP_Bind(UDP_SOCKET s, IP_ADDRESS_TYPE addType, UDP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)
{
    IP_ADDRESS_TYPE oldAddType;
    TCPIP_NET_IF* pSktIf;
    UDP_SOCKET_DCPT* pSkt;
    bool        portFail, bindSuccess;

    pSkt = _UDPSocketDcpt(s);
    if(pSkt == 0)
    {
        return false;
    }

    oldAddType = pSkt->addType;
    if(addType == IP_ADDRESS_TYPE_ANY)
    {
        if(oldAddType != IP_ADDRESS_TYPE_ANY)
        {   // cannot change the type w/o a disconnect...
            return false;
        }
        
        // don't use the remote address for ANY
        localAddress = 0;
    }
    else
    {   // specific address type wanted
        if(addType != IP_ADDRESS_TYPE_IPV4 && addType != IP_ADDRESS_TYPE_IPV6) 
        {   // sanity check
            return false;
        }
        if(oldAddType != IP_ADDRESS_TYPE_ANY && oldAddType != addType)
        {   // address type mismatch
            return false;
        }
    }

    // check for valid address
    pSktIf = 0;
    if(localAddress != 0)
    {
#if defined (TCPIP_STACK_USE_IPV4)
        if (addType == IP_ADDRESS_TYPE_IPV4 && localAddress->v4Add.Val != 0)
        {
            if((pSktIf = TCPIP_STACK_IPAddToNet(&localAddress->v4Add, false)) == 0)
            {    // no such interface
                return false;
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            if((pSktIf = _TCPIPStackIPv6AddToNet(&localAddress->v6Add, IPV6_ADDR_TYPE_UNICAST, false)) == 0)
            {    // no such interface
                return false;
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)
    }

    portFail = false;
    _UserGblLock();
    if(localPort == 0)
    {
        if((localPort = pSkt->localPort) == 0)
        {
            localPort = _UDPAllocateEphemeralPort();
            if(localPort == 0)
            {
                portFail = true;
            }
        }
        // else let the old local port be
    }
    else if(localPort != pSkt->localPort && _UDPIsAvailablePort(localPort) == false)
    {
        portFail = true;
    }
    _UserGblUnlock();

    if(portFail)
    {
        return false;
    }

    // success; bind
    _RxSktLock(pSkt);
    pSkt->addType = addType;
    bindSuccess = _UDPSocketBind(pSkt, pSktIf, localAddress);
    if(localAddress == 0)
    {   // if no localAddress, ignore the failure result
        bindSuccess = true;
    }
    if(bindSuccess)
    {
        pSkt->localPort = localPort;
    }
    else
    {   // restore old add type
        pSkt->addType = oldAddType;
    }

    _RxSktUnlock(pSkt);

    return bindSuccess;
}

bool TCPIP_UDP_RemoteBind(UDP_SOCKET s, IP_ADDRESS_TYPE addType, UDP_PORT remotePort,  IP_MULTI_ADDRESS* remoteAddress)
{
    IP_ADDRESS_TYPE oldAddType;
    UDP_SOCKET_DCPT *pSkt = _UDPSocketDcpt(s);

    if(pSkt == 0)
    {   // no such socket
        return false;
    }

    oldAddType = pSkt->addType;
    if(addType == IP_ADDRESS_TYPE_ANY)
    {
        if(oldAddType != IP_ADDRESS_TYPE_ANY)
        {   // cannot change the type w/o a disconnect...
            return false;
        }
        
        // don't use the remote address for ANY
        remoteAddress = 0;
    }
    else
    {   // specific address type wanted
        if(addType != IP_ADDRESS_TYPE_IPV4 && addType != IP_ADDRESS_TYPE_IPV6) 
        {   // sanity check
            return false;
        }
        if(oldAddType != IP_ADDRESS_TYPE_ANY && oldAddType != addType)
        {   // address type mismatch
            return false;
        }
    }

    _RxSktLock(pSkt);
#if defined (TCPIP_STACK_USE_IPV6)
    if(addType == IP_ADDRESS_TYPE_IPV6 && oldAddType == IP_ADDRESS_TYPE_ANY)
    {   // change of type; special IPv6 case: IPv6 cannot handle lazy allocation!
        TCPIP_NET_IF* pBindIf;
        if((pBindIf = pSkt->pSktNet) == 0)
        {   // use the default; IPv6 cannot handle no interface!
            pBindIf = (TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet();
        } 
        _UDPv6AllocateTxPacketStruct (pBindIf, pSkt, true);
        if(pSkt->pPkt == 0)
        {   // out of memory
            _RxSktUnlock(pSkt);
            return false;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    // set the new parameters
    pSkt->addType = addType;    // bind it
    if(remoteAddress != 0)
    {
        if(!TCPIP_UDP_DestinationIPAddressSet(s, addType, remoteAddress))
        {   // failed
            pSkt->addType = oldAddType; // restore
            _RxSktUnlock(pSkt);
            return false;
        }
        pSkt->flags.looseRemAddress = 0;
    }

    pSkt->remotePort = remotePort;
    pSkt->flags.looseRemPort = 0;
    _RxSktUnlock(pSkt);
    return true;
}

// Allows setting options to a socket like enable broadcast, Rx/Tx buffer size, etc
bool TCPIP_UDP_OptionsSet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam)
{
    UDP_SOCKET_DCPT*  pSkt = _UDPSocketDcpt(hUDP);

    if(pSkt)
    {
        switch(option)
        {
            case UDP_OPTION_STRICT_PORT:
                pSkt->flags.looseRemPort = (optParam == 0);
                return true;


            case UDP_OPTION_STRICT_NET:
                pSkt->flags.looseNetIf = (optParam == 0);
                return true;

            case UDP_OPTION_STRICT_ADDRESS:
                pSkt->flags.looseRemAddress = (optParam == 0);
                return true;

            case UDP_OPTION_BROADCAST:
                if((pSkt->flags.bcastForceType = (int)optParam) != UDP_BCAST_NONE)
                {   // set limited broadcast address (for now)
                    pSkt->destAddress.Val = 0xffffffff;
                    pSkt->flags.destSet = 1;
                } 
                else
                {   // the discrete address will have to be set and take effect
                    pSkt->destAddress.Val = 0;
                    pSkt->flags.destSet = 0;
                }

                return true;

            case UDP_OPTION_BUFFER_POOL:
#if defined (TCPIP_STACK_USE_IPV6)
                if(pSkt->addType == IP_ADDRESS_TYPE_IPV6)
                {   
                    return false;
                }
#endif  // defined (TCPIP_STACK_USE_IPV6)
                if(pSkt->flags.txSplitAlloc != 0)
                {   // no support for external payload sockets
                    return false;
                }

                if(pSkt->flags.usePool != (optParam != 0))
                {   // changed the buffer type; just release the packet
                    _UDPFreeTxResources(pSkt);
                    pSkt->flags.usePool = (optParam != 0);
                }
                return true;


            case UDP_OPTION_TX_BUFF:
                // just release the packet
                _UDPFreeTxResources(pSkt);
                pSkt->txSize = (uint16_t)(unsigned int)optParam;
                return true;

            case UDP_OPTION_TX_QUEUE_LIMIT:
                pSkt->txAllocLimit = (uint8_t)(unsigned int)optParam;
                return true;

            case UDP_OPTION_RX_QUEUE_LIMIT:
                pSkt->rxQueueLimit = (uint8_t)(unsigned int)optParam;
                return true;

            case UDP_OPTION_RX_AUTO_ADVANCE:
                pSkt->extFlags.rxAutoAdvance = (optParam != 0);
                return true;

            case UDP_OPTION_TX_TTL:
                pSkt->ttl = (uint8_t)(unsigned int)optParam;
                return true;

            case UDP_OPTION_MULTICAST:
                {
                    UDP_OPTION_MULTICAST_DATA* pMcOpt = (UDP_OPTION_MULTICAST_DATA*)optParam;
                    if(pMcOpt!= 0)
                    {
                        if((pMcOpt->flagsMask & UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0)
                        {   // change the source/net/source/port value
                            pSkt->flags.looseNetIf = (pMcOpt->flagsValue & UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0;
                            pSkt->flags.looseRemAddress = (pMcOpt->flagsValue & UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0;
                            pSkt->flags.looseRemPort = (pMcOpt->flagsValue & UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0;
                        }
                        if((pMcOpt->flagsMask & UDP_MCAST_FLAG_IGNORE_SOURCE_ADD) != 0)
                        {
                            pSkt->extFlags.ignoreSrcAdd = (pMcOpt->flagsValue & UDP_MCAST_FLAG_IGNORE_SOURCE_ADD) != 0;
                        }
                        if((pMcOpt->flagsMask & UDP_MCAST_FLAG_IGNORE_SOURCE_PORT) != 0)
                        {
                            pSkt->extFlags.ignoreSrcPort = (pMcOpt->flagsValue & UDP_MCAST_FLAG_IGNORE_SOURCE_PORT) != 0;
                        }
                        if((pMcOpt->flagsMask & UDP_MCAST_FLAG_IGNORE_UNICAST) != 0)
                        {
                            pSkt->extFlags.mcastOnly = (pMcOpt->flagsValue & UDP_MCAST_FLAG_IGNORE_UNICAST) != 0;
                        }
                        if((pMcOpt->flagsMask & UDP_MCAST_FLAG_LOOP) != 0)
                        {
                            pSkt->extFlags.mcastLoop = (pMcOpt->flagsValue & UDP_MCAST_FLAG_LOOP) != 0;
                        }
                        if((pMcOpt->flagsMask & UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK) != 0)
                        {
                            pSkt->extFlags.mcastSkipCheck = (pMcOpt->flagsValue & UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK) != 0;
                        }

                        return true;
                    }
                }
                return false;

            case UDP_OPTION_TOS:
                pSkt->extFlags.tos = (uint8_t)(unsigned int)optParam;
                return true;
                
            case UDP_OPTION_DF:
                pSkt->extFlags.df = (optParam != 0);
                return true;
                
            default:
                break;
        }
    }    

    return false;
}

// Allows getting options to a socket like enable broadcast, Rx/Tx buffer size, etc
bool TCPIP_UDP_OptionsGet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam)
{
    UDP_SOCKET_DCPT *pSkt = _UDPSocketDcpt(hUDP);

    if(pSkt && optParam)
    {
        switch(option)
        {
            case UDP_OPTION_STRICT_PORT:
                *(bool*)optParam = pSkt->flags.looseRemPort == 0;
                return true;

            case UDP_OPTION_STRICT_NET:
                *(bool*)optParam = pSkt->flags.looseNetIf == 0;
                return true;

            case UDP_OPTION_STRICT_ADDRESS:
                *(bool*)optParam = pSkt->flags.looseRemAddress == 0;
                return true;

            case UDP_OPTION_BROADCAST:
                *(UDP_SOCKET_BCAST_TYPE*)optParam = (UDP_SOCKET_BCAST_TYPE)pSkt->flags.bcastForceType;
                return true;

            case UDP_OPTION_BUFFER_POOL:
                *(bool*)optParam = pSkt->flags.usePool != 0;
                return true;

            case UDP_OPTION_TX_BUFF:
                *(uint16_t*)optParam = pSkt->txSize;
                return true;
                
            case UDP_OPTION_TX_QUEUE_LIMIT:
                *(uint8_t*)optParam = pSkt->txAllocLimit;
                return true;

            case UDP_OPTION_RX_QUEUE_LIMIT:
                *(uint8_t*)optParam = pSkt->rxQueueLimit;
                return true;

            case UDP_OPTION_RX_AUTO_ADVANCE:
                *(bool*)optParam = pSkt->extFlags.rxAutoAdvance != 0;
                return true;

            case UDP_OPTION_TX_TTL:
                *(uint8_t*)optParam = pSkt->ttl;
                return true;

            case UDP_OPTION_MULTICAST:
                {
                    UDP_MULTICAST_FLAGS* pMcFlags = (UDP_MULTICAST_FLAGS*)optParam;
                    if(pMcFlags!= 0)
                    {
                        UDP_MULTICAST_FLAGS mcFlags = 0;

                        if(pSkt->flags.looseNetIf == 1 && pSkt->flags.looseRemAddress == 1 && pSkt->flags.looseRemPort == 1)
                        {
                            mcFlags |= UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT;
                        }
                        if(pSkt->extFlags.ignoreSrcAdd != 0)
                        {
                            mcFlags |= UDP_MCAST_FLAG_IGNORE_SOURCE_ADD;
                        }
                        if(pSkt->extFlags.ignoreSrcPort != 0)
                        {
                            mcFlags |= UDP_MCAST_FLAG_IGNORE_SOURCE_PORT;
                        }
                        if(pSkt->extFlags.mcastOnly != 0)
                        {
                            mcFlags |= UDP_MCAST_FLAG_IGNORE_UNICAST;
                        }
                        if(pSkt->extFlags.mcastLoop != 0)
                        {
                            mcFlags |= UDP_MCAST_FLAG_LOOP;
                        }
                        if(pSkt->extFlags.mcastSkipCheck != 0)
                        {
                            mcFlags |= UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK;
                        }

                        *pMcFlags = mcFlags;
                        return true;
                    }
                }

                return false;

             case UDP_OPTION_TOS:
                *(uint8_t*)optParam = pSkt->extFlags.tos;
                return true;
                
             case UDP_OPTION_DF:
                *(bool*)optParam = pSkt->extFlags.df != 0;
                return true;
                
           default:
                break;
        }
    }    

    return false;
}


bool TCPIP_UDP_DestinationIPAddressSet(UDP_SOCKET s, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* remoteAddress)
{
    UDP_SOCKET_DCPT *pSkt;

    if(remoteAddress == 0)
    {
        return false;
    }

    pSkt = _UDPSocketDcpt(s);

    while(pSkt != 0 && pSkt->addType == addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != 0)
            {
                TCPIP_IPV6_DestAddressSet (pSkt->pV6Pkt, &remoteAddress->v6Add);
                return true;
            }
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == IP_ADDRESS_TYPE_IPV4)
        {
            if(pSkt->flags.bcastForceType != UDP_BCAST_NONE)
            {   // BCAST is already set and cannot be overridden!
                return false;
            }
            pSkt->destAddress.Val = remoteAddress->v4Add.Val;
            pSkt->flags.destSet = 1;
            return true;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        break;
    }

    return false;
}

bool TCPIP_UDP_DestinationPortSet(UDP_SOCKET s, UDP_PORT remotePort)
{
    UDP_SOCKET_DCPT *pSkt = _UDPSocketDcpt(s);

    if(pSkt != 0)
    {
        _RxSktLock(pSkt);
        pSkt->remotePort = remotePort;
        _RxSktUnlock(pSkt);
        return true;
    }

    return false;
}

#if defined (TCPIP_STACK_USE_IPV4)
bool TCPIP_UDP_SetSplitPayload(UDP_SOCKET s, void* pLoad, uint16_t loadSize)
{
    bool    success = false;
    UDP_SOCKET_DCPT *pSkt = _UDPSocketDcpt(s);

    if(pSkt && pSkt->flags.txSplitAlloc && pSkt->addType == IP_ADDRESS_TYPE_IPV4)
    {
        void* pPkt = _TxSktGetLockedV4Pkt(pSkt, true);
        if(pPkt == 0)
        {   // no packet or queued
            _UDPv4AllocateSktTxBuffer(pSkt, pSkt->addType, true);
        }

        if(pSkt->pPkt != 0)
        {
            TCPIP_MAC_DATA_SEGMENT* pZSeg = ((UDP_V4_PACKET*)pSkt->pPkt)->zcSeg;
            pZSeg->segLen = pZSeg->segSize = loadSize;
            pZSeg->segLoad = pLoad;

            success = true;
        }
    }

    return success;
}
#endif // defined (TCPIP_STACK_USE_IPV4)



static UDP_PORT _UDPAllocateEphemeralPort(void)
{
    int      num_ephemeral;
    int      count;
    UDP_PORT next_ephemeral;


    count = num_ephemeral = TCPIP_UDP_LOCAL_PORT_END_NUMBER - TCPIP_UDP_LOCAL_PORT_START_NUMBER + 1;

    next_ephemeral = TCPIP_UDP_LOCAL_PORT_START_NUMBER + (SYS_RANDOM_PseudoGet() % num_ephemeral);

    while(count--)
    {
        if(_UDPIsAvailablePort(next_ephemeral))
        {
            return next_ephemeral;
        }

        if (next_ephemeral == TCPIP_UDP_LOCAL_PORT_END_NUMBER)
        {
            next_ephemeral = TCPIP_UDP_LOCAL_PORT_START_NUMBER;
        }
        else
        {
            next_ephemeral++;
        }
    }

    return 0;   // not found
}

static bool _UDPIsAvailablePort(UDP_PORT port)
{
    int skt;
    UDP_SOCKET_DCPT *pSkt;

    // Find an available socket that matches the specified socket type
    for(skt = 0; skt < nUdpSockets; skt++)
    {
        pSkt = UDPSocketDcpt[skt]; 
        if(pSkt && pSkt->localPort == port)
        {
            return false;
        }
    }

    return true;
}

TCPIP_UDP_SIGNAL_HANDLE TCPIP_UDP_SignalHandlerRegister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_TYPE sigMask, TCPIP_UDP_SIGNAL_FUNCTION handler, const void* hParam)
{
    TCPIP_UDP_SIGNAL_HANDLE sHandle = 0;

    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if(handler != 0)
    {
        UDP_SOCKET_DCPT *pSkt = _UDPSocketDcpt(s);
        if(pSkt != 0 && pSkt->sigHandler == 0)
        {
            pSkt->sigHandler = handler;
            pSkt->sigParam = hParam;
            pSkt->sigMask = sigMask;
            sHandle = (TCPIP_UDP_SIGNAL_HANDLE)handler;
            // Note: this may change if multiple notfication handlers required 
        }
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return sHandle;
}

bool TCPIP_UDP_SignalHandlerDeregister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_HANDLE hSig)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    UDP_SOCKET_DCPT *pSkt = _UDPSocketDcpt(s);

    if(pSkt != 0)
    {  
        if(pSkt->sigHandler == (TCPIP_UDP_SIGNAL_FUNCTION)hSig)
        {
            pSkt->sigHandler = 0;
            pSkt->sigMask = 0;
            res = true;
        }
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

int TCPIP_UDP_SocketsNumberGet(void)
{
    return nUdpSockets;
}

// fragmentation support
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
static void _UDP_RxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET *pFragPkt, *pFragNext;

    for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragNext)
    {
        pFragNext = pFragPkt->pkt_next;
        TCPIP_PKT_PacketAcknowledge(pFragPkt, ackRes); 
    }
}

#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

// external packet processing
#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
TCPIP_UDP_PROCESS_HANDLE TCPIP_UDP_PacketHandlerRegister(TCPIP_UDP_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_UDP_PROCESS_HANDLE pHandle = 0;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(udpPktHandler == 0)
    {
        udpPktHandlerParam = handlerParam;
        udpPktHandler = pktHandler;
        pHandle = pktHandler;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return pHandle;
}

bool TCPIP_UDP_PacketHandlerDeregister(TCPIP_UDP_PROCESS_HANDLE pktHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(udpPktHandler == pktHandle)
    {
        udpPktHandler = 0;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)


#endif //#if defined(TCPIP_STACK_USE_UDP)

