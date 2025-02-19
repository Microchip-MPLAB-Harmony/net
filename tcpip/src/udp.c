/*******************************************************************************
  User Datagram Protocol (UDP) Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides unreliable, minimum latency transport of application 
     datagram (packet) oriented data
    -Reference: RFC 768
*******************************************************************************/

/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_UDP

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_UDP)


#include "udp_private.h"


/****************************************************************************
  Section:
    UDP Global Variables
  ***************************************************************************/
 

// Store an array of information pertaining to each UDP socket
static UDP_SOCKET_DCPT** udpSocketDcpt = NULL; 

static int32_t      nUdpSockets = 0;    // number of sockets in the current UDP configuration; >= 0
static const void*  udpMemH = NULL;     // memory handle
static int          udpInitCount = 0;   // initialization counter

static uint16_t     udpDefTxSize;               // default size of the TX buffer

#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
static SINGLE_LIST  udpPacketPool = { 0 };  // private pool of UDP packets

static uint16_t     udpPacketsInPool = 0;       // number of packets in the pool

static uint16_t     udpPoolPacketSize = 0;      // size of packets
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)


static TCPIP_SIGNAL_HANDLE    signalHandle = NULL;


// user threads protection semaphore
static OSAL_SEM_HANDLE_TYPE userSem;

#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
static TCPIP_UDP_PACKET_HANDLER udpPktHandler = NULL;
static const void* udpPktHandlerParam;
#endif  // (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)

/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

#if ((TCPIP_UDP_DEBUG_LEVEL & TCPIP_UDP_DEBUG_MASK_RX_CHECK) != 0)
// check ports: 0 - irrelevant; otherwise it's considered in match
static uint16_t checkUdpSrcPort = 0; 
static uint16_t checkUdpDstPort = 80;

static bool checkStrict = false;    // if 0, then any match, src or dest will do
                                    // else both source and dest must match
static uint32_t checkUdpBkptCnt = 0;

static bool TCPIP_UDP_CheckRxPkt(UDP_HEADER* pHdr)
{
    UDP_PORT srcPort = pHdr->SourcePort;
    UDP_PORT destPort = pHdr->DestinationPort;

    bool srcMatch = (srcPort == 0 || srcPort == checkUdpSrcPort);
    bool destMatch = (destPort == 0 || destPort == checkUdpDstPort);

    bool match = 0;

    if(checkStrict)
    {
        match = srcMatch && destMatch;
    }
    else
    {
        match = srcMatch || destMatch;
    }

    if(match)
    {
        checkUdpBkptCnt++;
        return true;
    }

    return false;
}
#else
#define TCPIP_UDP_CheckRxPkt(pHdr)
#endif // ((TCPIP_UDP_DEBUG_LEVEL & TCPIP_UDP_DEBUG_MASK_RX_CHECK) != 0)


// conversion functions/helpers
//
static __inline__ IPV4_ADDR*  __attribute__((always_inline)) FC_Uptr2IPv4Add(uint32_t* uptr)
{
    union
    {
        uint32_t*   uptr;
        IPV4_ADDR*  pV4Add;
    }U_UPTR_IPV4_ADD;

    U_UPTR_IPV4_ADD.uptr = uptr;
    return U_UPTR_IPV4_ADD.pV4Add;
}

static __inline__ UDP_SOCKET_DCPT*  __attribute__((always_inline)) FC_Vptr2UdpSkt(const void* vptr)
{
    union
    {
        const void*      vptr;
        UDP_SOCKET_DCPT* pSkt;
    }U_VPTR_UDP_SKT;

    U_VPTR_UDP_SKT.vptr = vptr;
    return U_VPTR_UDP_SKT.pSkt;
}

static __inline__ TCPIP_UDP_SIGNAL_HANDLE  __attribute__((always_inline)) FC_SigF2SigH(TCPIP_UDP_SIGNAL_FUNCTION sigF)
{
    union
    {
        TCPIP_UDP_SIGNAL_FUNCTION sigF;
        TCPIP_UDP_SIGNAL_HANDLE sigH;
    }U_SIGF_SIG_HANDLE;

    U_SIGF_SIG_HANDLE.sigF = sigF;
    return U_SIGF_SIG_HANDLE.sigH;
}

static __inline__ TCPIP_UDP_SIGNAL_FUNCTION  __attribute__((always_inline)) FC_SigH2SigF(TCPIP_UDP_SIGNAL_HANDLE sigH)
{
    union
    {
        TCPIP_UDP_SIGNAL_HANDLE sigH;
        TCPIP_UDP_SIGNAL_FUNCTION sigF;
    }U_SIG_HNDL_SIG_FUNC;

    U_SIG_HNDL_SIG_FUNC.sigH = sigH;
    return U_SIG_HNDL_SIG_FUNC.sigF;
}

static __inline__ TCPIP_UDP_PROCESS_HANDLE  __attribute__((always_inline)) FC_PktHndl2ProcHndl(TCPIP_UDP_PACKET_HANDLER pktHandler)
{
    union
    {
        TCPIP_UDP_PACKET_HANDLER pktHandler;
        TCPIP_UDP_PROCESS_HANDLE procHandle;
    }U_PKT_HNDL_PROC_HNDL;

    U_PKT_HNDL_PROC_HNDL.pktHandler = pktHandler;
    return U_PKT_HNDL_PROC_HNDL.procHandle;
}

static __inline__ TCPIP_UDP_PACKET_HANDLER  __attribute__((always_inline)) FC_ProcHndl2PktHndl(TCPIP_UDP_PROCESS_HANDLE procHandle)
{
    union
    {
        TCPIP_UDP_PROCESS_HANDLE procHandle;
        TCPIP_UDP_PACKET_HANDLER pktHandler;
    }U_PROC_HNDL_PKT_HNDL;

    U_PROC_HNDL_PKT_HNDL.procHandle = procHandle;
    return U_PROC_HNDL_PKT_HNDL.pktHandler;
}

static __inline__ UDP_V4_ZC_PACKET*  __attribute__((always_inline)) FC_MacPkt2UdpZcPkt(TCPIP_MAC_PACKET* pMacPkt)
{
    union
    {
        TCPIP_MAC_PACKET*   pMacPkt;
        UDP_V4_PACKET*      pU4Pkt;
        UDP_V4_ZC_PACKET*   pZcPkt;
        IPV4_PACKET*        pIp4Pkt;
    }U_MAC_PKT_UDP_PKT;

    U_MAC_PKT_UDP_PKT.pMacPkt = pMacPkt;
    return U_MAC_PKT_UDP_PKT.pZcPkt;
}

static __inline__ IPV4_PACKET*  __attribute__((always_inline)) FC_MacPkt2IpV4Pkt(TCPIP_MAC_PACKET* pMacPkt)
{
    union
    {
        TCPIP_MAC_PACKET*   pMacPkt;
        UDP_V4_PACKET*      pU4Pkt;
        UDP_V4_ZC_PACKET*   pZcPkt;
        IPV4_PACKET*        pIp4Pkt;
    }U_MAC_PKT_IPV4_PKT;

    U_MAC_PKT_IPV4_PKT.pMacPkt = pMacPkt;
    return U_MAC_PKT_IPV4_PKT.pIp4Pkt;
}

static __inline__ UDP_V4_ZC_PACKET*  __attribute__((always_inline)) FC_IpV4Pkt2UdpZcPkt(IPV4_PACKET* pIp4Pkt)
{
    union
    {
        IPV4_PACKET*        pIp4Pkt;
        TCPIP_MAC_PACKET*   pMacPkt;
        UDP_V4_PACKET*      pU4Pkt;
        UDP_V4_ZC_PACKET*   pZcPkt;
    }U_IP_PKT_UDP_ZC_PKT;

    U_IP_PKT_UDP_ZC_PKT.pIp4Pkt = pIp4Pkt;
    return U_IP_PKT_UDP_ZC_PKT.pZcPkt;
}

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
static __inline__ OSAL_RESULT  __attribute__((always_inline))   UserGblLockCreate(void)
{
    // create the shared Data Lock
    return OSAL_SEM_Create(&userSem, OSAL_SEM_TYPE_BINARY, 1, 1);
}    

static __inline__ void  __attribute__((always_inline))          UserGblLockDelete(void)
{
    (void) OSAL_SEM_Delete(&userSem);
}    

// locks access to shared resources
static __inline__ void  __attribute__((always_inline))          UserGblLock(void)
{
    // Shared Data Lock
    (void)OSAL_SEM_Pend(&userSem, OSAL_WAIT_FOREVER);
}    

// unlocks access to shared resources
static __inline__ void  __attribute__((always_inline))          UserGblUnlock(void)
{
    // Shared Data unlock
    (void)OSAL_SEM_Post(&userSem);
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

static __inline__ void  __attribute__((always_inline))      RxSktLock(UDP_SOCKET_DCPT* pSkt)
{
    pSkt->flags.rxEnable = 0;
}

// unlocks/enables the socket RX process
static __inline__ void  __attribute__((always_inline))      RxSktUnlock(UDP_SOCKET_DCPT* pSkt)
{
    pSkt->flags.rxEnable = 1;
}

static __inline__ bool  __attribute__((always_inline))      RxSktIsLocked(UDP_SOCKET_DCPT* pSkt)
{
    return (pSkt->flags.rxEnable == 0U);
}

// protected access to the socket RX queue
static TCPIP_UDP_SIGNAL_FUNCTION RxSktQueueAddLocked(UDP_SOCKET_DCPT* pSkt, void* pNode, const void** pSigParam)
{
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler = NULL;
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_Helper_SingleListTailAdd(&pSkt->rxQueue, (SGL_LIST_NODE*)pNode);
    if((pSkt->sigMask & (uint16_t)TCPIP_UDP_SIGNAL_RX_DATA) != 0U)
    {
        sigHandler = pSkt->sigHandler;
        *pSigParam = pSkt->sigParam;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return sigHandler;
}

static SGL_LIST_NODE* RxSktQueueRemoveLocked(UDP_SOCKET_DCPT* pSkt)
{
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    SGL_LIST_NODE* pNode = TCPIP_Helper_SingleListHeadRemove(&pSkt->rxQueue);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);


    return pNode;
}


#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
// protected access to the global udpPacketPool
static void     PoolAddPktLocked(TCPIP_MAC_PACKET* pPkt)
{
    SGL_LIST_NODE* node = FC_MacPkt2SglNode(pPkt);

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_Helper_SingleListTailAdd(&udpPacketPool, node);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
}

static TCPIP_MAC_PACKET* PoolRemovePktLocked(void)
{
    SGL_LIST_NODE*  node;

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    node = TCPIP_Helper_SingleListHeadRemove(&udpPacketPool);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    return FC_SglNode2MacPkt(node);
}
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)


#if (TCPIP_IPV4_UDP_FRAGMENTATION != 0)
static void UDP_RxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
#else
static __inline__ void __attribute__((always_inline)) UDP_RxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);
}
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

static void     UDPClose(UDP_SOCKET_DCPT* pSkt);
static void     UDPFreeTxResources(UDP_SOCKET_DCPT* pSkt);
static void     UDPFreeRxQueue(UDP_SOCKET_DCPT* pSkt);

// returns the source port of a UDP RX packet
// it assumes that the packet was properly received and processed
// (i.e. endianess already converted)
static __inline__ uint16_t __attribute__((always_inline)) UDPRxPktSourcePort(TCPIP_MAC_PACKET* pRxPkt)
{
    UDP_HEADER* pHdr = FC_U8Ptr2UdpHdr(pRxPkt->pTransportLayer);
    return pHdr->SourcePort;
}

static void UDPResetRxPacket(UDP_SOCKET_DCPT* pSkt, TCPIP_MAC_PACKET* pRxPkt)
{
    if((pSkt->pCurrRxPkt = pRxPkt) != NULL)
    {
        UDP_HEADER* pHdr = FC_U8Ptr2UdpHdr(pRxPkt->pTransportLayer);
        pSkt->pCurrRxSeg = pRxPkt->pDSeg;
        pSkt->rxSegLen = pRxPkt->pDSeg->segLen - (uint16_t)sizeof(UDP_HEADER);
        pSkt->rxTotLen = pHdr->Length;  
        pSkt->rxCurr = pRxPkt->pTransportLayer + sizeof(UDP_HEADER);
#if (TCPIP_IPV4_UDP_FRAGMENTATION != 0)
        pSkt->pCurrFrag = pRxPkt;
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)
    }
    else
    {
        pSkt->pCurrRxSeg = NULL;
        pSkt->rxSegLen = 0;
        pSkt->rxTotLen = 0;
        pSkt->rxCurr = NULL;
    }
}

static void UDPsetPacketInfo(UDP_SOCKET_DCPT* pSkt, TCPIP_MAC_PACKET* pRxPkt)
{

#if defined (TCPIP_STACK_USE_IPV4)
    if(pRxPkt != NULL && pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {
        uint32_t pktSrcAdd = TCPIP_IPV4_PacketGetSourceAddress(pRxPkt)->Val;
        pSkt->pktSrcAddress.Val = pktSrcAdd;
        if(pSkt->flags.destSet == 0U || pSkt->flags.fixedDestAddress == 0U)
        {
            pSkt->destAddress.Val = pktSrcAdd;
        }
        uint32_t pktDestAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt)->Val;
        pSkt->pktDestAddress.Val = pktDestAdd;
        if(pSkt->flags.srcSet == 0U)
        {   // try to reply to the sender; change the source address, if not forced
            IPV4_ADDR* destAdd = FC_Uptr2IPv4Add(&pktDestAdd);
            bool isMcastAdd = TCPIP_Helper_IsMcastAddress(destAdd);
            bool isBcastAdd = TCPIP_STACK_IsBcastAddress((const TCPIP_NET_IF*)pRxPkt->pktIf, destAdd);
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
        pSkt->pSktNet = (const TCPIP_NET_IF*)pRxPkt->pktIf;    // bind it
        if(pSkt->remotePort == 0U || pSkt->flags.fixedDestPort == 0U )
        {
            pSkt->remotePort = UDPRxPktSourcePort(pRxPkt); 
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
    if(pRxPkt != NULL && pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        if(pSkt->flags.destSet == 0U || pSkt->flags.fixedDestAddress == 0U)
        {
            TCPIP_IPV6_DestAddressSet(pSkt->pV6Pkt, TCPIP_IPV6_PacketGetSourceAddress(pRxPkt));
        }
        if(pSkt->flags.srcSet == 0U)
        {
            pSkt->flags.srcSolved = 0;
            pSkt->flags.srcValid = 1;
            TCPIP_IPV6_SourceAddressSet(pSkt->pV6Pkt, TCPIP_IPV6_PacketGetDestAddress(pRxPkt));
        }
        pSkt->pSktNet = (const TCPIP_NET_IF*)pRxPkt->pktIf;    // bind it
        if(pSkt->remotePort == 0U || pSkt->flags.fixedDestPort == 0U )
        {
            pSkt->remotePort = UDPRxPktSourcePort(pRxPkt); 
        }
        pSkt->pV6Pkt->netIfH = (const TCPIP_NET_IF*)pRxPkt->pktIf;
        TCPIP_IPV6_PacketIPProtocolSet (pSkt->pV6Pkt);
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    // else leave the old source/dest values in place

}

static void UDPSetNewRxPacket(UDP_SOCKET_DCPT* pSkt, TCPIP_MAC_PACKET* pRxPkt)
{

    if(pSkt->pCurrRxPkt != NULL)
    {   // acknowledge the old one
        UDP_RxPktAcknowledge(pSkt->pCurrRxPkt, TCPIP_MAC_PKT_ACK_RX_OK);
    }

    UDPResetRxPacket(pSkt, pRxPkt);
    UDPsetPacketInfo(pSkt, pRxPkt);
}


// lock protects access to RX queue
// and makes sure the RX thread doesn't kick in
static void UDPUpdatePacketLock(UDP_SOCKET_DCPT* pSkt)
{
    // extract RX packet
    SGL_LIST_NODE* node = RxSktQueueRemoveLocked(pSkt);
    TCPIP_MAC_PACKET* pNextPkt = FC_SglNode2MacPkt(node); 
    UDPSetNewRxPacket(pSkt, pNextPkt);
}

static void UDPUpdatePacket(UDP_SOCKET_DCPT* pSkt)
{
    // extract RX packet
    SGL_LIST_NODE* node = TCPIP_Helper_SingleListHeadRemove(&pSkt->rxQueue);
    TCPIP_MAC_PACKET* pMacPkt = FC_SglNode2MacPkt(node); 
    UDPSetNewRxPacket(pSkt, pMacPkt);
}

static UDP_SOCKET_DCPT*  UDPFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, UDP_HEADER *h, IP_ADDRESS_TYPE addressType);

static bool             UDPTxPktValid(UDP_SOCKET_DCPT * pSkt);

#if defined (TCPIP_STACK_USE_IPV4)
static void*            UDPv4AllocateSktTxBuffer(UDP_SOCKET_DCPT* pSkt, IP_ADDRESS_TYPE addType, bool update);
static void             UDPv4TxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param);
static uint16_t         UDPv4IsTxPutReady(UDP_SOCKET_DCPT* pSkt);
static uint16_t         UDPv4Flush(UDP_SOCKET_DCPT* pSkt);
static void*            TxSktGetLockedV4Pkt(UDP_SOCKET_DCPT* pSkt, bool clrSktPkt);
static void             UDPv4TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV4_PACKET* pPkt);
static TCPIP_MAC_PKT_ACK_RES TCPIP_UDP_ProcessIPv4(TCPIP_MAC_PACKET* pRxPkt);
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)
static void*             UDPAllocateTxPacket(uint16_t pktSize, uint16_t txBuffSize, uint32_t allocFlags);
#endif  // defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)


#if defined (TCPIP_STACK_USE_IPV6)
static IPV6_PACKET*     UDPv6AllocateTxPacketStruct (const TCPIP_NET_IF * pNetIf, UDP_SOCKET_DCPT * pSkt, bool update);
static void             UDPv6TxAckFnc (void* pkt, bool success, const void * param);
static void             UDPv6TxMacAckFnc (TCPIP_MAC_PACKET* pPkt, const void * param);
static uint16_t         UDPv6IsTxPutReady(UDP_SOCKET_DCPT* pSkt, uint16_t count);
static uint16_t         UDPv6Flush(UDP_SOCKET_DCPT* pSkt);
static void             UDPv6FreePacket(IPV6_PACKET* pkt);
static void             UDPv6ResetHeader(UDP_HEADER * h);
static void*            TxSktGetLockedV6Pkt(UDP_SOCKET_DCPT* pSkt, bool clear);
static void             UDPv6TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV6_PACKET* pV6Pkt, void* pUpperLayer);
static TCPIP_MAC_PKT_ACK_RES TCPIP_UDP_ProcessIPv6(TCPIP_MAC_PACKET* pRxPkt);
#endif  // defined (TCPIP_STACK_USE_IPV6)

static UDP_PORT         UDPAllocateEphemeralPort(void);
static bool             UDPIsAvailablePort(UDP_PORT port);

static void             TCPIP_UDP_Process(void);

static UDP_SOCKET       UDP_SktOpen(IP_ADDRESS_TYPE addType, UDP_OPEN_TYPE opType, UDP_PORT port, IP_MULTI_ADDRESS* hostAddress);

#if (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void UDPAbortSockets(uint32_t netMask, TCPIP_UDP_SIGNAL_TYPE sigType); 
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

static bool UDPSetSourceAddress(UDP_SOCKET_DCPT* pSkt, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* localAddress)
{

    if(localAddress == NULL)
    {   // nothing to set
        return false;
    }

    while(pSkt->addType == (uint8_t)addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != NULL)
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

static  bool UDPSocketBind(UDP_SOCKET_DCPT* pSkt, const TCPIP_NET_IF* pNet, IP_MULTI_ADDRESS* srcAddress)
{
    if((pSkt->pSktNet = pNet) != NULL && pSkt->flags.noNetStrict == 0U)
    {   // specific bind requested
        if((pSkt->extFlags & (uint8_t)UDP_EFLAG_STICKY_LOOSE_NET_IF) == 0U)
        {
            pSkt->flags.looseNetIf = 0;
        }
    }
#if defined (TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        if(pSkt->pV6Pkt != NULL)
        {
            pSkt->pV6Pkt->netIfH = pNet;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    return UDPSetSourceAddress(pSkt, (IP_ADDRESS_TYPE)pSkt->addType, srcAddress);
}



/*static __inline__*/static  void /*__attribute__((always_inline))*/ UDPSocketTxSet(UDP_SOCKET_DCPT* pSkt,  void* pTxPkt, uint8_t* txBuff, IP_ADDRESS_TYPE addType)
{
    pSkt->txStart = txBuff;
    pSkt->txEnd = txBuff + pSkt->txSize;
    pSkt->txWrite = txBuff;
    pSkt->addType =  (uint8_t)addType;
    pSkt->pPkt = pTxPkt;
}

// returns the associated socket descriptor, if such a socket is valid
/*static __inline__*/static  UDP_SOCKET_DCPT* /*__attribute__((always_inline))*/ UDPSocketDcpt(UDP_SOCKET s)
{
    if(s >= 0 && s < nUdpSockets)
    {
       return udpSocketDcpt[s];
    }

    return NULL;
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

bool TCPIP_UDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    UDP_SOCKET_DCPT** newSktDcpt; 
    
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface start up
        return true;    // do not store per interface data
    }
    
    // stack start up
    if(udpInitCount != 0)
    {   // initialize just once
        udpInitCount++;
        return true;
    }
    
    if(stackCtrl->memH == NULL)
    {
        SYS_ERROR(SYS_ERROR_ERROR, "UDP NULL dynamic allocation handle");
        return false;
    }

    // check configuration data not missing
    if(initData == NULL)
    {
        return false;
    }
    const TCPIP_UDP_MODULE_CONFIG* pUdpInit = (const TCPIP_UDP_MODULE_CONFIG*)initData;

    // create the locks
    if(UserGblLockCreate() != OSAL_RESULT_SUCCESS)
    {
        return false;
    }

    // register the task function
    signalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_UDP_Task, 0);
    if(signalHandle == NULL)
    {
        UserGblLockDelete();
        return false;
    }

    newSktDcpt = (UDP_SOCKET_DCPT**)TCPIP_HEAP_Calloc(stackCtrl->memH, pUdpInit->nSockets, sizeof(UDP_SOCKET_DCPT*));
    if(newSktDcpt == NULL)
    {
        SYS_ERROR(SYS_ERROR_ERROR, "UDP Dynamic allocation failed");
        UserGblLockDelete();
        TCPIPStackSignalHandlerDeregister(signalHandle);
        return false;
    }
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
    TCPIP_Helper_SingleListInitialize (&udpPacketPool);
    udpPacketsInPool = pUdpInit->poolBuffers;
    udpPoolPacketSize = (pUdpInit->poolBufferSize < (uint16_t)UDP_SOCKET_POOL_BUFFER_MIN_SIZE) ? (uint16_t)UDP_SOCKET_POOL_BUFFER_MIN_SIZE: pUdpInit->poolBufferSize;
    // allocate the pool; used for IPv4 only for now!!!
    if(udpPacketsInPool != 0U)
    {
        uint16_t ix;
        TCPIP_MAC_PACKET* pPkt;
        SGL_LIST_NODE*  node;

        for(ix = 0; ix < udpPacketsInPool; ix++)
        {
            pPkt = UDPAllocateTxPacket((uint16_t)sizeof(UDP_V4_PACKET), udpPoolPacketSize, (uint32_t)UDP_SOCKET_POOL_BUFFER_FLAG);
            if(pPkt != NULL)
            {
                // Note: no protection for this access
                node = FC_MacPkt2SglNode(pPkt); 
                TCPIP_Helper_SingleListTailAdd(&udpPacketPool, node);
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
    nUdpSockets = (int32_t)pUdpInit->nSockets;
    udpDefTxSize = pUdpInit->sktTxBuffSize;
    udpSocketDcpt = newSktDcpt;
#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
    udpPktHandler = NULL;
#endif  // (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
    udpInitCount++;

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    // allow user access
    UserGblUnlock();
    
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

    UserGblLock();     // make sure no one is opening/closing sockets now

    // interface is going down
    UDPAbortSockets(1UL << (uint32_t)stackCtrl->netIx, TCPIP_UDP_SIGNAL_IF_DOWN); 

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
    {   // stack shut down
        if(--udpInitCount == 0)
        {   // all closed
            // release resources
            // just in case there are any not bound sockets
            for(ix = 0; ix < nUdpSockets; ix++)
            {
                pSkt = udpSocketDcpt[ix];
                if(pSkt != NULL) 
                {
                    UDPClose(pSkt);
                }
            }

            (void) TCPIP_HEAP_Free(udpMemH, udpSocketDcpt);

            udpSocketDcpt = NULL;

#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
            // Note: no protection for this access
            TCPIP_MAC_PACKET* pPkt;
            SGL_LIST_NODE*  node;

            while(true)
            {
                node = TCPIP_Helper_SingleListHeadRemove(&udpPacketPool);
                if(node == NULL)
                {   // done
                    break;
                }
                // free the packet
                pPkt = FC_SglNode2MacPkt(node);
                TCPIP_PKT_PacketFree(pPkt);
            }

            udpPacketsInPool = 0;
            udpPoolPacketSize = 0;
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)

            if(signalHandle != NULL)
            {
                TCPIPStackSignalHandlerDeregister(signalHandle);
                signalHandle = NULL;
            }

            udpMemH = NULL;
            nUdpSockets = 0;
            killSem = true;
        }
    }
    
    if(killSem)
    {
        UserGblLockDelete();
    }
    else
    {
        UserGblUnlock();
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void UDPAbortSockets(uint32_t netMask, TCPIP_UDP_SIGNAL_TYPE sigType)
{
    int ix, sktIfIx;
    UDP_SOCKET_DCPT* pSkt;
    const TCPIP_NET_IF* sktIf;

    TCPIP_UDP_SIGNAL_FUNCTION sigHandler;
    const void*     sigParam;
    uint16_t        sigMask;


    for(ix = 0; ix < nUdpSockets; ix++)
    {
        if((pSkt = udpSocketDcpt[ix]) != NULL)  
        {
            sktIf = pSkt->pSktNet;
            sktIfIx = TCPIP_STACK_NetIxGet(sktIf); 
            if(sktIfIx >= 0)
            {
                uint32_t sktIfMask = 1UL << (uint32_t)sktIfIx;
                if((sktIfMask & netMask) != 0U)
                {   // match
                    // just disconnect, don't kill sockets
                    (void) TCPIP_UDP_Disconnect(pSkt->sktIx, true);
                    // get a consistent reading
                    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                    sigHandler = pSkt->sigHandler;
                    sigParam = pSkt->sigParam;
                    sigMask = pSkt->sigMask;
                    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

                    if(sigHandler != NULL && (sigMask & (uint16_t)sigType) != 0U)
                    {
                        (*sigHandler)(pSkt->sktIx, (TCPIP_NET_HANDLE)sktIf, sigType, sigParam);
                    }
                }
            }
        }
    }
} 

#endif  // (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

UDP_SOCKET TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE addType, UDP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)
{
    return TCPIP_UDP_OpenServerSkt(addType, localPort, localAddress, UDP_OPEN_SERVER);
}

UDP_SOCKET TCPIP_UDP_OpenServerSkt(IP_ADDRESS_TYPE addType, UDP_PORT localPort,  IP_MULTI_ADDRESS* localAddress, UDP_OPEN_TYPE opType)
{
    UDP_SOCKET  skt;
    TCPIP_NET_IF* pDefIf = NULL;   // default: unbound
   
#if !defined (TCPIP_STACK_USE_IPV6)
   if(addType == IP_ADDRESS_TYPE_IPV6)
   {
       return INVALID_SOCKET;
   } 
#endif  // defined (TCPIP_STACK_USE_IPV6)
    
#if !defined (TCPIP_STACK_USE_IPV4)
   if(addType == IP_ADDRESS_TYPE_IPV4)
   {
       return INVALID_SOCKET;
   }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV4)
   if(addType == IP_ADDRESS_TYPE_IPV4 && localAddress != NULL)
   {
       if(localAddress->v4Add.Val == 0U)
       { // ignore
           localAddress = NULL;
       }
       else
       {
           pDefIf = TCPIP_STACK_IPAddToNet(&localAddress->v4Add, false);
           if(pDefIf == NULL)
           {    // no such interface
               return INVALID_UDP_SOCKET;
           }
       }
   }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
   if(addType == IP_ADDRESS_TYPE_IPV6 && localAddress != NULL)
   {
       pDefIf = TCPIPStackIPv6AddToNet(&localAddress->v6Add, IPV6_ADDR_TYPE_UNICAST, false);
       if(pDefIf == NULL)
       {    // no such interface
           return INVALID_UDP_SOCKET;
       }
   }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    
    skt = UDP_SktOpen(addType, opType, localPort, NULL);
    if(skt != INVALID_UDP_SOCKET)
    {
        UDP_SOCKET_DCPT* pSkt = udpSocketDcpt[skt];
        (void) UDPSocketBind(pSkt, pDefIf, localAddress);
        if(pDefIf != NULL)
        {
            pSkt->flags.openBindIf = 1;
        }
        if(localAddress != NULL)
        {
            pSkt->flags.openBindAdd = 1;
        }
        // safely enable RX
        RxSktUnlock(pSkt);
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

    skt = UDP_SktOpen(addType, opType, remotePort, remoteAddress);
    if(skt != INVALID_UDP_SOCKET)
    {
        UDP_SOCKET_DCPT* pSkt = udpSocketDcpt[skt];
#if defined (TCPIP_STACK_USE_IPV6)
        if(addType == IP_ADDRESS_TYPE_IPV6)
        {   // IPv6 clients require explicit binding; use the default interface; user can change the binding
            (void) UDPSocketBind(pSkt, (const TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet(), NULL);
            pSkt->flags.openBindIf = 1;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV6)
        // safely enable RX
        RxSktUnlock(pSkt);
    }

    return skt;
}



static UDP_SOCKET UDP_SktOpen(IP_ADDRESS_TYPE addType, UDP_OPEN_TYPE opType, UDP_PORT port, IP_MULTI_ADDRESS* hostAddress)
{
    int        sktIx = -1;
    UDP_SOCKET_DCPT *pSkt = NULL;
    UDP_PORT   localPort, remotePort;
    bool       newSktValid; 

    if(udpSocketDcpt == NULL)
    {
        return (UDP_SOCKET)INVALID_UDP_SOCKET;
    }

    if(((uint16_t)opType & (uint16_t)UDP_OPEN_CLIENT) != 0U)
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
    UserGblLock();
    while(true)
    {
        if(localPort == 0U )
        {
            localPort = UDPAllocateEphemeralPort();
            if(localPort == 0U)
            {   // couldn't allocate a new port
                break;
            }
        }

        // search for an empty socket slot
        pSkt = (UDP_SOCKET_DCPT*)-1;
        for ( sktIx = 0; sktIx < nUdpSockets; sktIx++ )
        {
            pSkt = udpSocketDcpt[sktIx];
            if(pSkt == NULL)
            {   // found an empty slot; sktIx is the valid slot index
                break;
            }
        }

        if(pSkt != NULL)
        {   // all slots taken;
            break;
        }

        // success; following expensive/blocking allocation operation done with
        // all sockets locked!
        // Otherwise the socket would not be properly constructed and
        // a de-init/close operation now would be a disaster! 
        pSkt = (UDP_SOCKET_DCPT*)TCPIP_HEAP_Calloc(udpMemH, 1, sizeof(*pSkt));
        newSktValid = pSkt != NULL;

        break;
    }

    if(newSktValid)
    {   // mark slot as valid; continue initialization
        pSkt->sktIx = (int16_t)sktIx;
        udpSocketDcpt[sktIx] = pSkt;
    }

    // end of critical
    UserGblUnlock();

    if(!newSktValid)
    {
        return (UDP_SOCKET)INVALID_UDP_SOCKET;
    }

    // fill in all the socket parameters
    // so that the RX thread can see all the right data
    pSkt->localPort = localPort;    
    pSkt->remotePort = remotePort;
    pSkt->addType = (uint8_t)addType;
    pSkt->txAllocLimit = (uint16_t)TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT; 
    pSkt->rxQueueLimit = (uint16_t)TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT;
    pSkt->flags.openAddType = (uint8_t)addType;

    if(((uint16_t)opType & (uint16_t)UDP_OPEN_TX_SPLIT) != 0U)
    {
        pSkt->txSize = 0;
        pSkt->flags.txSplitAlloc = 1;
    }
    else
    {
        pSkt->txSize = udpDefTxSize;
    }

    if(((uint16_t)opType & (uint16_t)UDP_OPEN_CONFIG_SERVICE) != 0U)
    {
        pSkt->flags.stackConfig = 1;
    }

    // For IPv4 we postpone the allocation until the user wants to write something.
    // This allows RX only server sockets, that don't take extra memory.
    // Not possible for IPv6. It will have to rely on TCPIP_UDP_OptionsSet!

#if defined (TCPIP_STACK_USE_IPV6)
    if (addType == IP_ADDRESS_TYPE_IPV6)
    {
        (void)UDPv6AllocateTxPacketStruct (NULL, pSkt, true);
        if(pSkt->pPkt == NULL)
        {   // out of memory
            UserGblLock(); 
            UDPClose(pSkt);
            UserGblUnlock();
            return (UDP_SOCKET)INVALID_UDP_SOCKET;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)


    if(((uint16_t)opType & (uint16_t)UDP_OPEN_SERVER) != 0U)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            TCPIP_IPV6_DestAddressSet(pSkt->pV6Pkt, &IPV6_FIXED_ADDR_ALL_NODES_MULTICAST);
            TCPIP_IPV6_SetRemoteMacAddress(pSkt->pV6Pkt, &IPV6_MULTICAST_MAC_ADDRESS);
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        // default non strict connections for server
        pSkt->flags.looseRemPort = 1U; 
        pSkt->flags.looseNetIf = 1U; 
        pSkt->flags.looseRemAddress = 1U; 
        pSkt->flags.serverSkt = 1;
    }
    else
    {   // UDP_OPEN_CLIENT
        switch(addType)
        {
#if defined (TCPIP_STACK_USE_IPV4)
            case IP_ADDRESS_TYPE_IPV4:
                // hostAddress is a literal IP address.
                if(hostAddress != NULL)
                {
                    pSkt->destAddress.Val = hostAddress->v4Add.Val;
                    pSkt->flags.destSet = 1;
                }
                break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
            case IP_ADDRESS_TYPE_IPV6:
                if(hostAddress != NULL)
                {
                    TCPIP_IPV6_DestAddressSet (pSkt->pV6Pkt, &hostAddress->v6Add);
                }
                break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

            default:
                // do nothing
                break;
        }
    }

    return (UDP_SOCKET)sktIx;
}



// allocates a packet from the packet heap
#if defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)
static void* UDPAllocateTxPacket(uint16_t pktSize, uint16_t txBuffSize, uint32_t allocFlags)
{
    allocFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_TX | (uint32_t)TCPIP_MAC_PKT_FLAG_UDP;
    
    return TCPIP_PKT_SocketAlloc(pktSize, (uint16_t)sizeof(UDP_HEADER), txBuffSize, (TCPIP_MAC_PACKET_FLAGS)allocFlags);
}
#endif // defined (TCPIP_STACK_USE_IPV4) || (TCPIP_UDP_USE_POOL_BUFFERS != 0)

void TCPIP_UDP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
    uint32_t  netMask;

    sigPend = TCPIPStackModuleSignalParamGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL, &netMask);

    if(((uint32_t)sigPend & (uint32_t)(TCPIP_MODULE_SIGNAL_IF_CHANGE)) != 0U)
    { // interface address change occurred
        UDPAbortSockets(netMask, TCPIP_UDP_SIGNAL_IF_CHANGE); 
    }
#else
    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_UDP_Process();
    }

}

static void TCPIP_UDP_Process(void)
{
    TCPIP_MAC_PACKET*   pRxPkt;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    // extract queued UDP packets
    while((pRxPkt = TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != NULL)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
        if(udpPktHandler != NULL)
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
        else if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_NET_TYPE) == (uint32_t)TCPIP_MAC_PKT_FLAG_IPV4) 
        {
            ackRes = TCPIP_UDP_ProcessIPv4(pRxPkt);
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
        else if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_NET_TYPE) == (uint32_t)TCPIP_MAC_PKT_FLAG_IPV6) 
        {
            ackRes = TCPIP_UDP_ProcessIPv6(pRxPkt);
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)
        else
        {
            /* Do Nothing */
        }
        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {   // unknown/error; discard it.
            UDP_RxPktAcknowledge(pRxPkt, ackRes);
        }
    }
}



#if defined (TCPIP_STACK_USE_IPV4)
static void* UDPv4AllocateSktTxBuffer(UDP_SOCKET_DCPT* pSkt, IP_ADDRESS_TYPE addType, bool update)
{
    TCPIP_MAC_PACKET*   pPkt;
    uint8_t*            pTxBuff;
    uint32_t            allocFlags;
    uint16_t            pktSize;

    if(addType != IP_ADDRESS_TYPE_IPV4)
    {   // IPv4 allocation only
        return NULL;
    }

    if(pSkt->txAllocCnt >= pSkt->txAllocLimit)
    {   // cannot try to allocate any more packets
        return NULL;
    }

    // allocate IPv4 packet
    pPkt = NULL;
    allocFlags = (uint32_t)TCPIP_MAC_PKT_FLAG_IPV4;
    if(pSkt->flags.stackConfig != 0U)
    {
        allocFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_CONFIG;
    }

    if(pSkt->flags.txSplitAlloc == 0U)
    {
        pktSize = (uint16_t)sizeof(UDP_V4_PACKET);
        // allocate from pool, if possible
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
        if(pSkt->flags.usePool != 0U && pSkt->txSize <= udpPoolPacketSize)
        {
            pPkt = PoolRemovePktLocked();
            if(pPkt != NULL)
            {
                pPkt->pktFlags = allocFlags | (uint32_t)UDP_SOCKET_POOL_BUFFER_FLAG;
                pPkt->next = NULL;
                pPkt->pDSeg->segLen = 0;
            }
        }
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)
    }
    else
    {
        allocFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT;
        pktSize = (uint16_t)sizeof(UDP_V4_ZC_PACKET);
    }

#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
    if(pPkt == NULL)
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)
    {   // allocate from main packet pool
        pPkt = UDPAllocateTxPacket(pktSize, pSkt->txSize, allocFlags);
    }

    if(pPkt != NULL)
    {
        TCPIP_PKT_PacketAcknowledgeSet(pPkt, &UDPv4TxAckFnc, pSkt);

        if(pSkt->flags.txSplitAlloc != 0U)
        {   // link the 2 segments
            UDP_V4_ZC_PACKET* pZcPkt = FC_MacPkt2UdpZcPkt(pPkt);
            TCPIP_MAC_DATA_SEGMENT* pZSeg = pZcPkt->zcSeg;
            pPkt->pDSeg->next = pZSeg;
            pZSeg->segFlags = (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC;   // embedded in packet itself
            pTxBuff = NULL;    // will be set as external payload
        }
        else
        {
            pTxBuff = pPkt->pTransportLayer + sizeof(UDP_HEADER);
        }

        if(update)
        {
            UDPSocketTxSet(pSkt, pPkt, pTxBuff, addType);
        }
        pSkt->txAllocCnt++;
    }


    return pPkt;
}

static void UDPv4TxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param)
{
    TCPIP_NET_HANDLE pktIf = NULL;
    UDP_SOCKET   sktIx = 0;
    bool critLock = false;
    bool freePkt = true;
    bool    loopPkt = false;
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler = NULL;
    const void* sigParam = NULL;

    UDP_SOCKET_DCPT* pSkt = FC_Vptr2UdpSkt(param);
    OSAL_CRITSECT_DATA_TYPE status = 0;

    while(pSkt != NULL)
    {  
        // make sure the user threads don't mess with the socket right now
        status =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        sktIx = pSkt->sktIx;
        if(udpSocketDcpt == NULL || sktIx < 0 || sktIx >= nUdpSockets || pSkt != udpSocketDcpt[sktIx])
        {   // no longer this socket?
            pSkt = NULL;
            break;
        }

        if(pPkt->modPktData != 0U)
        {   // redirect internally. once!
            pPkt->modPktData = 0;
            loopPkt = true;
            freePkt = false;
            break;
        }

        if((pSkt->sigMask & (uint16_t)TCPIP_UDP_SIGNAL_TX_DONE) != 0U)
        {
            sigHandler = pSkt->sigHandler;
            sigParam = pSkt->sigParam;
            pktIf = pPkt->pktIf;
        }

        IPV4_PACKET* pIp4Pkt = FC_MacPkt2IpV4Pkt(pPkt);
        if(pSkt->pV4Pkt != pIp4Pkt)
        {   // no longer using this packet;
            break;
        }

        // OK, keep the current packet
        UDPv4TxPktReset(pSkt, pIp4Pkt);
        freePkt = false;

        break;
    }

    if(freePkt && (pSkt != NULL))
    {
        pSkt->txAllocCnt--;
    }

    if(critLock)
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }


    if(loopPkt)
    {
        pPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_MCAST;
        TCPIP_PKT_FlightLogTxSkt(pPkt, TCPIP_THIS_MODULE_ID, ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort, (uint16_t)pSkt->sktIx);
        TCPIPStackInsertRxPacket((const TCPIP_NET_IF*)pPkt->pktIf, pPkt, true);
        return;
    }

    if(freePkt)
    {   // discard packet
        // either closed socket or another packet already allocated
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
        if((pPkt->pktFlags & (uint32_t)UDP_SOCKET_POOL_BUFFER_FLAG) != 0U && udpPacketsInPool != 0U)
        {   // re-insert in the pool
            PoolAddPktLocked(pPkt);
        }
        else
#endif  // (TCPIP_UDP_USE_POOL_BUFFERS != 0)
        {
            TCPIP_PKT_PacketFree(pPkt);
        }
    }

    if(sigHandler != NULL)
    {   // notify socket user
        (*sigHandler)(sktIx, pktIf, TCPIP_UDP_SIGNAL_TX_DONE, sigParam);

    }
}

static void UDPv4TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV4_PACKET* pPkt)
{

    pPkt->macPkt.pDSeg->segLen = 0;
    if(pSkt->flags.txSplitAlloc != 0U)
    {
        UDP_V4_ZC_PACKET* pZcPkt = FC_IpV4Pkt2UdpZcPkt(pPkt); 
        pZcPkt->zcSeg->segLen = 0;
    }
    else
    {
        pSkt->txWrite = pSkt->txStart;
    }
    pPkt->macPkt.pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
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
static void*  TxSktGetLockedV4Pkt(UDP_SOCKET_DCPT* pSkt, bool clrSktPkt)
{
    IPV4_PACKET* pPkt;
    
    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    
    if((pPkt = pSkt->pV4Pkt) != NULL)
    {   // we have a current TX packet
        if((pPkt->macPkt.pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED) != 0U)
        {   // queued, cannot use it
            pPkt = NULL;
            if(clrSktPkt)
            {   // clear it so the TX thread deletes it
                pSkt->pV4Pkt = NULL;
            }
        }
        // else not queued, could safely use it
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return pPkt;
}


bool TCPIP_UDP_BcastIPV4AddressSet(UDP_SOCKET hUDP, UDP_SOCKET_BCAST_TYPE bcastType, TCPIP_NET_HANDLE hNet)
{
    UDP_SOCKET_DCPT *pSkt;
    IPV4_ADDR       bcastAddress;

    pSkt = UDPSocketDcpt(hUDP);
    if(pSkt == NULL || pSkt->addType != (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {
        return false;
    }

    if(pSkt->flags.bcastForceType != (unsigned int)UDP_BCAST_NONE)
    {   // BCAST is already set and cannot be overridden!
        return false;
    }

    if(bcastType == UDP_BCAST_NETWORK_LIMITED)
    {
        bcastAddress.Val = 0xffffffffU;
    } 

    else if(bcastType == UDP_BCAST_NETWORK_DIRECTED)
    {
        if((bcastAddress.Val = TCPIP_STACK_NetAddressBcast(hNet)) == 0U)
            {   // interface down/unknown?
                return false;
            }
    }
    else
    {
            return false;
    }


    // set broadcast address
    pSkt->destAddress.Val = bcastAddress.Val;
    pSkt->flags.destSet = 1;

    return true;
}

static uint16_t UDPv4IsTxPutReady(UDP_SOCKET_DCPT* pSkt)
{
    void* pPkt = TxSktGetLockedV4Pkt(pSkt, true);

    if(pPkt == NULL)
    {   // packet is in one of the queues; allocate another tx space
        if(UDPv4AllocateSktTxBuffer(pSkt, IP_ADDRESS_TYPE_IPV4, true) == NULL)
        {   // allocation failed; caller will have to retry later
            return 0;
        }
        // else the new packet data is updated in place
    }

    if(pSkt->flags.txSplitAlloc == 0U)
    {
        return (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txWrite);
    }

    // return max segment possible
    return 1514U - sizeof(TCPIP_MAC_ETHERNET_HEADER) - sizeof(IPV4_HEADER) - sizeof(UDP_HEADER);
}

// Handles an incoming UDP v4 packet.
static TCPIP_MAC_PKT_ACK_RES TCPIP_UDP_ProcessIPv4(TCPIP_MAC_PACKET* pRxPkt)
{
    UDP_HEADER*      pUDPHdr;
    UDP_SOCKET_DCPT* pSkt;
    uint16_t         udpTotLength;
    const IPV4_ADDR* pPktSrcAdd;
    const IPV4_ADDR* pPktDstAdd;
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler;
    const void*      sigParam;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    pUDPHdr = FC_U8Ptr2UdpHdr(pRxPkt->pTransportLayer);
    udpTotLength = TCPIP_Helper_ntohs(pUDPHdr->Length);


#if (TCPIP_IPV4_UDP_FRAGMENTATION == 0)
    if(udpTotLength != pRxPkt->totTransportLen)
    {   // discard suspect packet
        return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
    }
#else
    bool isFragmented = pRxPkt->pkt_next != NULL;
#endif  // (TCPIP_IPV4_UDP_FRAGMENTATION != 0)

    pPktSrcAdd = TCPIP_IPV4_PacketGetSourceAddress(pRxPkt);
    pPktDstAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);
    // See if we need to validate the checksum field (0x0000 is disabled)
#ifdef TCPIP_UDP_USE_RX_CHECKSUM
#if (TCPIP_IPV4_UDP_FRAGMENTATION == 0)
    if(pUDPHdr->Checksum != 0U && (pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_UDP) == 0U)
#else
    if(pUDPHdr->Checksum != 0U && (isFragmented || (pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_UDP) == 0U))
#endif
    {   // no hardware checksum offload 
        IPV4_PSEUDO_HEADER  pseudoHdr;
        uint16_t            calcChkSum;
        // Calculate IP pseudoheader checksum.
        pseudoHdr.SourceAddress.Val = pPktSrcAdd->Val;
        pseudoHdr.DestAddress.Val = pPktDstAdd->Val;
        pseudoHdr.Zero  = 0;
        pseudoHdr.Protocol = (uint8_t)IP_PROT_UDP;
        pseudoHdr.Length = pUDPHdr->Length;

        calcChkSum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHdr, (uint16_t)sizeof(pseudoHdr), 0);
#if (TCPIP_IPV4_UDP_FRAGMENTATION != 0)
        TCPIP_MAC_PACKET* pFragPkt;
        uint16_t totCalcUdpLen = 0;
        for(pFragPkt = pRxPkt; pFragPkt != NULL; pFragPkt = pFragPkt->pkt_next)
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
        if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT) != 0U)
        {
            calcChkSum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pUDPHdr, udpTotLength, calcChkSum);
        }
        else
        {
            calcChkSum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pUDPHdr, udpTotLength, calcChkSum);
        }
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

        if(calcChkSum != 0U)
        {   // discard packet
            return TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        }
    }
#endif // TCPIP_UDP_USE_RX_CHECKSUM

    pUDPHdr->SourcePort = TCPIP_Helper_ntohs(pUDPHdr->SourcePort);
    pUDPHdr->DestinationPort = TCPIP_Helper_ntohs(pUDPHdr->DestinationPort);
    pUDPHdr->Length = udpTotLength - (uint16_t)sizeof(UDP_HEADER);    

    TCPIP_UDP_CheckRxPkt(pUDPHdr);

    while(true)
    {
        pSkt = UDPFindMatchingSocket(pRxPkt, pUDPHdr, IP_ADDRESS_TYPE_IPV4);
        if(pSkt == NULL)
        {
            // If there is no matching socket, There is no one to handle
            // this data.  Discard it.
            ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
            break;
        }

#if defined(TCPIP_STACK_USE_IGMP)    
        if(pSkt->flags.mcastSkipCheck == 0U)
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
    
        if(pSkt->flags.mcastOnly != 0U)
        {   // let through multicast traffic only
            if(!TCPIP_Helper_IsMcastAddress(pPktDstAdd))
            {   // don't let it through
                ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
                break;
            }
        }

        // insert valid packet in the RX queue
        sigHandler = RxSktQueueAddLocked(pSkt, pRxPkt, &sigParam);
        if(sigHandler != NULL)
        {   // notify socket user
            (*sigHandler)(pSkt->sktIx, pRxPkt->pktIf, TCPIP_UDP_SIGNAL_RX_DATA, sigParam);
        }

        // everything OK, pass to user
        ackRes = TCPIP_MAC_PKT_ACK_NONE;
        break;
    }


    // log 
#if (TCPIP_PACKET_LOG_ENABLE)
    uint32_t logPort = ((uint32_t)pUDPHdr->DestinationPort << 16) | pUDPHdr->SourcePort;
    TCPIP_PKT_FlightLogRxSkt(pRxPkt, TCPIP_MODULE_LAYER3, logPort, pSkt != NULL ? (uint16_t)pSkt->sktIx: (uint16_t)0xffffU);
#endif  // (TCPIP_PACKET_LOG_ENABLE)

    return ackRes;

}

static uint16_t UDPv4Flush(UDP_SOCKET_DCPT* pSkt)
{
    IPV4_PACKET*        pv4Pkt;
    uint16_t            udpLoadLen, udpTotLen, rootLen;
    UDP_HEADER*         pUDPHdr;
    IPV4_PSEUDO_HEADER  pseudoHdr;
    uint16_t            checksum;
    TCPIP_MAC_DATA_SEGMENT* pZSeg;
    TCPIP_IPV4_PACKET_PARAMS pktParams;
    bool                isMcastDest;

    if(pSkt->destAddress.Val == 0U)
    {   // don't even bother
        return 0;
    }

    if(pSkt->flags.srcSolved == 0U || pSkt->pSktNet == NULL)
    {
        pSkt->pSktNet = (const TCPIP_NET_IF*)TCPIP_IPV4_SelectSourceInterface(pSkt->pSktNet, &pSkt->destAddress, &pSkt->srcAddress, pSkt->flags.srcValid != 0U);
        if(pSkt->pSktNet == NULL)
        {   // cannot find an route?
            return 0;
        }
        pSkt->flags.srcSolved = 1;
        pSkt->flags.srcValid = 1;
    }

    if(pSkt->flags.bcastForceType == (unsigned int)UDP_BCAST_NETWORK_DIRECTED)
    {   // have to adjust for this interface
        pSkt->destAddress.Val = TCPIP_STACK_NetAddressBcast(pSkt->pSktNet);
    }

    pv4Pkt = pSkt->pV4Pkt;
    pv4Pkt->srcAddress.Val = pSkt->srcAddress.Val;
    pv4Pkt->destAddress.Val = pSkt->destAddress.Val;
    pv4Pkt->netIfH = pSkt->pSktNet;

    // start preparing the UDP header and packet
    pUDPHdr = FC_U8Ptr2UdpHdr(pv4Pkt->macPkt.pTransportLayer);
    // update the current load length
    if(pSkt->flags.txSplitAlloc != 0U)
    {
        UDP_V4_ZC_PACKET* pZcPkt = FC_IpV4Pkt2UdpZcPkt(pv4Pkt);
        pZSeg = pZcPkt->zcSeg;
        udpLoadLen = pZSeg->segLen;
        rootLen = (uint16_t)sizeof(UDP_HEADER);
        // size of the payload should already be set
    }
    else
    {
        udpLoadLen = (uint16_t)((uintptr_t)pSkt->txWrite - (uintptr_t)pSkt->txStart);
        rootLen = udpLoadLen + (uint16_t)sizeof(UDP_HEADER); 
        pZSeg = NULL;
    }
    pv4Pkt->macPkt.pDSeg->segLen += rootLen;
    udpTotLen = udpLoadLen + (uint16_t)sizeof(UDP_HEADER);

    pUDPHdr->SourcePort = TCPIP_Helper_htons(pSkt->localPort);
    pUDPHdr->DestinationPort = TCPIP_Helper_htons(pSkt->remotePort);
    pUDPHdr->Length = TCPIP_Helper_htons(udpTotLen);
    pUDPHdr->Checksum = 0;

    if((pSkt->pSktNet->txOffload & (uint8_t)TCPIP_MAC_CHECKSUM_UDP) == 0U)
    {   // not handled by hardware; add the pseudo header
        pseudoHdr.SourceAddress.Val = pv4Pkt->srcAddress.Val;
        pseudoHdr.DestAddress.Val = pv4Pkt->destAddress.Val;
        pseudoHdr.Zero = 0;
        pseudoHdr.Protocol = (uint8_t)IP_PROT_UDP;
        pseudoHdr.Length = pUDPHdr->Length;

        checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHdr, (uint16_t)sizeof(pseudoHdr), 0);

        if(pSkt->flags.txSplitAlloc != 0U)
        {
            checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)pUDPHdr, (uint16_t)sizeof(UDP_HEADER), checksum);
            checksum = ~TCPIP_Helper_CalcIPChecksum(pZSeg->segLoad, udpLoadLen, checksum);
        }
        else
        {   // one contiguous buffer
            checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)pUDPHdr, udpTotLen, checksum);
        }

        pUDPHdr->Checksum = ~checksum;
    }

    isMcastDest = TCPIP_Helper_IsMcastAddress(&pv4Pkt->destAddress);
    if(isMcastDest)
    {
        pktParams.ttl = pSkt->ttl == 0U ? UDP_MULTICAST_DEFAULT_TTL : pSkt->ttl;
    }
    else
    {
        pktParams.ttl = pSkt->ttl;
    }

    pktParams.tosFlags = (TCPIP_IPV4_TOS_FLAGS)pSkt->flags.tos;
    pktParams.df = (uint8_t)pSkt->flags.df;

    // and we're done
    TCPIP_IPV4_PacketFormatTx(pv4Pkt, (uint8_t)IP_PROT_UDP, udpTotLen, &pktParams);
    pv4Pkt->macPkt.next = NULL;    // single packet

    if(isMcastDest && (pSkt->flags.mcastLoop != 0U))
    {
        pv4Pkt->macPkt.modPktData = 1;
    }
    else
    {
        pv4Pkt->macPkt.modPktData = 0;
    }

    TCPIP_PKT_FlightLogTxSkt(&pv4Pkt->macPkt, TCPIP_THIS_MODULE_ID,  ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort, (uint16_t)pSkt->sktIx);
    if(TCPIP_IPV4_PacketTransmit(pv4Pkt))
    {
        return udpLoadLen; 
    }

    // packet reuse
    TCPIP_PKT_FlightLogAcknowledge(&pv4Pkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
    UDPv4TxPktReset(pSkt, pv4Pkt);
    
    return 0;
}

#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
static IPV6_PACKET * UDPv6AllocateTxPacketStruct (const TCPIP_NET_IF * pNetIf, UDP_SOCKET_DCPT * pSkt, bool update)
{
    IPV6_PACKET * pkt;
    uint8_t*     txBuffer;

    if(pSkt->txAllocCnt >= pSkt->txAllocLimit)
    {   // cannot try to allocate any more packets
        return NULL;
    }

    pkt = TCPIP_IPV6_TxPacketAllocate (pNetIf, &UDPv6TxAckFnc, pSkt);

    if (pkt == NULL)
    {
        return NULL;
    }

    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, NULL, (uint16_t)sizeof (UDP_HEADER), (uint8_t)IP_PROT_UDP, UDP_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree (pkt);
        return NULL;
    }

    // initialize the packet
    TCPIP_IPV6_SourceAddressSet(pkt, NULL);
    TCPIP_IPV6_DestAddressSet(pkt, NULL);
    TCPIP_IPV6_SetPacketMacAcknowledge(pkt, &UDPv6TxMacAckFnc);

    txBuffer = (uint8_t*)TCPIP_HEAP_Malloc(udpMemH, pSkt->txSize);
    if(txBuffer == NULL)
    {
        TCPIP_IPV6_PacketFree (pkt);
        return NULL;
    }

    if(update)
    {
        UDPSocketTxSet(pSkt, pkt, txBuffer, IP_ADDRESS_TYPE_IPV6);
    }

    pSkt->txAllocCnt++;
    pkt->clientData = txBuffer;
    return pkt;
}

static void UDPv6FreePacket(IPV6_PACKET* pkt)
{
    if(pkt->clientData != NULL)
    {
        (void) TCPIP_HEAP_Free(udpMemH, pkt->clientData);
        pkt->clientData = NULL;
    }
    TCPIP_IPV6_PacketFree ((IPV6_PACKET *)pkt);
}

static void UDPv6TxAckFnc (void* pkt, bool success, const void * param)
{
    UDP_SOCKET   sktIx;
    IPV6_PACKET*    pV6Pkt = (IPV6_PACKET*)pkt;
    bool critLock = false;
    bool freePkt = true;

    UDP_SOCKET_DCPT* pSkt = FC_Vptr2UdpSkt(param);
    
    OSAL_CRITSECT_DATA_TYPE status = 0;

    while(pSkt != NULL)
    {  
        // make sure the user threads don't mess with the socket right now
        status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        sktIx = pSkt->sktIx;
        if(udpSocketDcpt == NULL || sktIx < 0 || sktIx >= nUdpSockets || pSkt != udpSocketDcpt[sktIx])
        {   // no longer this socket?
            pSkt = NULL;
            break;
        }

        if(pSkt->pV6Pkt != pV6Pkt)
        {   // no longer using this packet;
            break;
        }

        // OK, keep the current packet
        UDPv6TxPktReset(pSkt, pV6Pkt, NULL);
        freePkt = false;

        break;
    }

    if(freePkt && (pSkt != NULL))
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
        UDPv6FreePacket (pV6Pkt);
    }
    
}

static void UDPv6TxPktReset(UDP_SOCKET_DCPT* pSkt, IPV6_PACKET* pV6Pkt, void* pUpperLayer)
{
    TCPIP_IPV6_TransmitPacketStateReset (pV6Pkt);
    if(pUpperLayer == NULL)
    {
        pUpperLayer = TCPIP_IPV6_UpperLayerHeaderPtrGet(pV6Pkt);
    }

    UDPv6ResetHeader(pUpperLayer);
    pSkt->txWrite = pSkt->txStart;
}

static void UDPv6TxMacAckFnc (TCPIP_MAC_PACKET* pPkt, const void * param)
{
    TCPIP_NET_HANDLE pktIf = NULL;
    UDP_SOCKET   sktIx = 0;
    bool critLock = false;
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler = NULL;
    const void* sigParam = NULL;

    UDP_SOCKET_DCPT* pSkt = FC_Vptr2UdpSkt(param);
    OSAL_CRITSECT_DATA_TYPE status = 0;

    while(pSkt != NULL)
    {  
        // make sure the user threads don't mess with the socket right now
        status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        if(pSkt != UDPSocketDcpt(pSkt->sktIx))
        {   // no longer this socket?
            break;
        }

        if((pSkt->sigMask & (uint16_t)TCPIP_UDP_SIGNAL_TX_DONE) != 0U)
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
#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
    // this packet was actually allocated by IPv6
    // trace will fail if UDP wants to free it
    F_TCPIP_PKT_PacketFreeDebug(pPkt, TCPIP_MODULE_IPV6);
#else
    TCPIP_PKT_PacketFree(pPkt);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

    
    if(sigHandler != NULL)
    {   // notify socket user
        (*sigHandler)(sktIx, pktIf, TCPIP_UDP_SIGNAL_TX_DONE, sigParam);

    }

}

// same TX protection
// IPv6 variant
// protects user vs TX threads when the user needs to allocate a new TX packet for a socket
// returns:
//  - valid pointer: the packet is no longer in use and can be safely modified
//  - 0: a new packet needs to be allocated and safely updated
//  Note: there's no corresponding unlock function!
static void*  TxSktGetLockedV6Pkt(UDP_SOCKET_DCPT* pSkt, bool clear)
{
    IPV6_PACKET* pPkt;
    
    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    
    if((pPkt = pSkt->pV6Pkt) != NULL)
    {   // we have a current TX packet
        if(pPkt->flags.queued != 0U)
        {   // queued, cannot use it
            pPkt = NULL;
            if(clear)
            {   // clear it so the TX thread deletes it
                pSkt->pV6Pkt = NULL;
            }
        }
        // else not queued, could safely use it
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return pPkt;
}



static uint16_t UDPv6IsTxPutReady(UDP_SOCKET_DCPT* pSkt, uint16_t count)
{
    bool    newPkt;
    IPV6_PACKET * pkt;

    bool queued = false;
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if((pkt = pSkt->pV6Pkt) != NULL)
    {
        queued = pkt->flags.queued != 0U;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    if ((pkt != NULL) && !queued)
    {   // packet available
        return (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txWrite);
    }

    // either no packet or already queued
    // Try to allocate a new transmit packet
    IPV6_PACKET * tempPtr = UDPv6AllocateTxPacketStruct(pSkt->pSktNet, pSkt, false);
    if (tempPtr == NULL)
    {
        // We couldn't allocate a new packet.  Return 0 until we can 
        // or until a queued packet can be returned to this node.
        return 0;
    }

    // copy the old packet info
    if (pkt != NULL)
    {
        if (!TCPIP_IPV6_TxPacketStructCopy (tempPtr, pkt))
        {   // failed; leave the old one in place
            UDPv6FreePacket(tempPtr);
            return 0;
        }
    }

    // now store changes if original packet not yet available
    status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if(pkt != NULL && pkt->flags.queued == 0U)
    {   // TX thread just cleared it
        newPkt = false;
    }
    else
    {   // still queued
        UDPSocketTxSet(pSkt, tempPtr, tempPtr->clientData, IP_ADDRESS_TYPE_IPV6);
        newPkt = true;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    if(newPkt)
    {
        UDPv6ResetHeader(TCPIP_IPV6_UpperLayerHeaderPtrGet(tempPtr));
    }
    else
    {   // leave the old one in place
        UDPv6FreePacket(tempPtr);
    }

    return (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txWrite);
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
    h = FC_U8Ptr2UdpHdr(pRxPkt->pTransportLayer);
    udpTotLength = TCPIP_Helper_ntohs(h->Length);

    if(udpTotLength != pRxPkt->totTransportLen)
    {   // discard suspect packet
        return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
    }


#ifdef TCPIP_UDP_USE_RX_CHECKSUM
    if(h->Checksum != 0U && (pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_UDP) == 0U)
    {
        IPV6_PSEUDO_HEADER  pseudoHeader;
        uint16_t            calcChkSum;

        // Calculate IP pseudoheader checksum.
        (void) memcpy (&pseudoHeader.SourceAddress, remoteIP, sizeof (IPV6_ADDR));
        (void) memcpy (&pseudoHeader.DestAddress, localIP, sizeof (IPV6_ADDR));
        // Total payload length is the length of data + extension headers
        pseudoHeader.PacketLength = h->Length;
        pseudoHeader.zero1 = 0;
        pseudoHeader.zero2 = 0;
        pseudoHeader.NextHeader = (uint8_t)IP_PROT_UDP;

        calcChkSum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHeader, (uint16_t)sizeof(pseudoHeader), 0);
        if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT) != 0U)
        {
            calcChkSum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)h, udpTotLength, calcChkSum);
        }
        else
        {
            calcChkSum = TCPIP_Helper_CalcIPChecksum((uint8_t*)h, udpTotLength, calcChkSum);
        }

        if(calcChkSum != 0U)
        {   // discard packet
            return TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        }
    }
#endif // TCPIP_UDP_USE_RX_CHECKSUM



    h->SourcePort        = TCPIP_Helper_ntohs(h->SourcePort);
    h->DestinationPort   = TCPIP_Helper_ntohs(h->DestinationPort);
    h->Length            = udpTotLength - (uint16_t)sizeof(UDP_HEADER);

    while(true)
    {
        pSkt = UDPFindMatchingSocket(pRxPkt, h, IP_ADDRESS_TYPE_IPV6);
        if(pSkt == NULL)
        {   // Send ICMP Destination Unreachable Code 4 (Port unreachable) and discard packet
            uint16_t headerLen = pRxPkt->ipv6PktData;
            TCPIP_IPV6_ErrorSend ((const TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt, localIP, remoteIP, (uint8_t)ICMPV6_ERR_DU_PORT_UNREACHABLE, (uint8_t)ICMPV6_ERROR_DEST_UNREACHABLE, 0, udpTotLength + headerLen + (uint16_t)sizeof (IPV6_HEADER));

            // If there is no matching socket, There is no one to handle
            // this data.  Discard it.
            ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
            break;
        }

        // insert valid packet in the RX queue
        sigHandler = RxSktQueueAddLocked(pSkt, pRxPkt, &sigParam);
        if(sigHandler != NULL)
        {   // notify socket user
            (*sigHandler)(pSkt->sktIx, pRxPkt->pktIf, TCPIP_UDP_SIGNAL_RX_DATA, sigParam);
        }

        // let it through
        ackRes = TCPIP_MAC_PKT_ACK_NONE;
        break;
    }

#if (TCPIP_PACKET_LOG_ENABLE)
    uint32_t logPort = ((uint32_t)h->DestinationPort << 16) | h->SourcePort;
    TCPIP_PKT_FlightLogRxSkt(pRxPkt, TCPIP_MODULE_LAYER3, logPort, pSkt != NULL ? (uint16_t)pSkt->sktIx: (uint16_t)0xffffU);
#endif  // (TCPIP_PACKET_LOG_ENABLE)

    
    return ackRes;

}

static uint16_t UDPv6Flush(UDP_SOCKET_DCPT* pSkt)
{
    const TCPIP_NET_IF* pSktNet;
    uint16_t        wUDPLength;
    UDP_HEADER*     pUDPHeader;

    pSktNet = (const TCPIP_NET_IF*)pSkt->pV6Pkt->netIfH;
    if(pSktNet == NULL || !TCPIP_IPV6_InterfaceIsReady(pSktNet))
    {   // IPv6 client socket requires explicit binding
        return 0;
    }

    pSkt->pSktNet = pSktNet;    // make sure they are in sync
    wUDPLength = (uint16_t)((uintptr_t)pSkt->txWrite - (uintptr_t)pSkt->txStart);
    if(TCPIP_IPV6_PayloadSet (pSkt->pV6Pkt, pSkt->txStart, wUDPLength) == 0U)
    {
        return 0;
    }


    TCPIP_IPV6_HeaderPut(pSkt->pV6Pkt, (uint8_t)IP_PROT_UDP);

    pUDPHeader = (UDP_HEADER *)TCPIP_IPV6_UpperLayerHeaderPtrGet(pSkt->pV6Pkt);

    pUDPHeader->SourcePort = TCPIP_Helper_htons(pSkt->localPort);
    pUDPHeader->DestinationPort = TCPIP_Helper_htons(pSkt->remotePort);
    pUDPHeader->Length = TCPIP_Helper_htons(wUDPLength + (uint16_t)sizeof(UDP_HEADER));

    (void) TCPIP_IPV6_Flush (pSkt->pV6Pkt);

    if (TCPIP_IPV6_IsPacketQueued(pSkt->pV6Pkt) == 0U)
    {
        UDPv6TxPktReset(pSkt, pSkt->pV6Pkt, pUDPHeader);
    }
    return wUDPLength;
}


static void UDPv6ResetHeader(UDP_HEADER * h)
{
    if (h != NULL)
    {
        h->Checksum = 0x0000;
        h->Length = 0x0000;
    }
}

#endif  // defined (TCPIP_STACK_USE_IPV6)


static bool UDPTxPktValid(UDP_SOCKET_DCPT * pSkt)
{
#if defined(TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        return TxSktGetLockedV6Pkt(pSkt, false) != NULL;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {
        return TxSktGetLockedV4Pkt(pSkt, false) != NULL;
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)


    return false;
}

bool TCPIP_UDP_IsConnected(UDP_SOCKET hUDP)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);
    while(pSkt != NULL)
    {
        // if there's a pending packet, update info
        if(pSkt->pCurrRxSeg == NULL)
        {   // no more data in this packet 
            UDPUpdatePacketLock(pSkt);
        }

#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != NULL && pSkt->pV6Pkt->netIfH != NULL)
            {
                IPV6_ADDR* pAdd = TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt);
                if(pAdd != NULL && memcmp(pAdd->v, IPV6_FIXED_ADDR_UNSPECIFIED.v, sizeof(IPV6_ADDR)) != 0)
                {
                    return true;
                }
            }

            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            return pSkt->destAddress.Val != 0U;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        break;
        // cannot send data with address ANY
    }

    return false;
}




bool TCPIP_UDP_Close(UDP_SOCKET hUDP)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if(pSkt != NULL)
    {
        UserGblLock(); 
        UDPClose(pSkt);
        UserGblUnlock();
        return true;
    }

    return false;
}

bool TCPIP_UDP_Disconnect(UDP_SOCKET hUDP, bool flushRxQueue)
{

    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if(pSkt != NULL)
    {
        if(pSkt->flags.openAddType == (unsigned int)IP_ADDRESS_TYPE_ANY)
        {   // free the TX resources because next traffic could be different
            UDPFreeTxResources(pSkt);
        }

        RxSktLock(pSkt);
        // restore initial settings
        pSkt->addType = (uint8_t)(IP_ADDRESS_TYPE)pSkt->flags.openAddType;
        if(pSkt->flags.openBindIf == 0U)
        {
            pSkt->pSktNet = NULL;
        }

        if(pSkt->flags.openBindAdd == 0U)
        {
            pSkt->srcAddress.Val = 0;
            pSkt->flags.srcValid = 0U;
            pSkt->flags.srcSolved = 0U;
        }

        if(pSkt->flags.serverSkt != 0U && pSkt->flags.looseRemPort != 0U)
        {   // stop server connection on the remote port if looseRemPort (rbind was not performed);
            pSkt->remotePort = 0;
        }

        if(flushRxQueue)
        {
            UDPFreeRxQueue(pSkt);
            UDPUpdatePacket(pSkt);
        }
        else
        {   // re-read the current packet
            UDPsetPacketInfo(pSkt, pSkt->pCurrRxPkt);
        }

        RxSktUnlock(pSkt);
        return true;
    }

    return false;

}


static void UDPClose(UDP_SOCKET_DCPT* pSkt)
{
    UDPFreeTxResources(pSkt);
    RxSktLock(pSkt);
    UDPFreeRxQueue(pSkt);
    if(pSkt->pCurrRxPkt != NULL)
    {   // acknowledge the old one
        UDP_RxPktAcknowledge(pSkt->pCurrRxPkt, TCPIP_MAC_PKT_ACK_PROTO_DEST_CLOSE);
    }
    udpSocketDcpt[pSkt->sktIx] = NULL;
    (void) TCPIP_HEAP_Free(udpMemH, pSkt);
}

static void UDPFreeTxResources(UDP_SOCKET_DCPT* pSkt)
{
    void* pCurrPkt = NULL;

    switch(pSkt->addType)
    {
#if defined(TCPIP_STACK_USE_IPV6)
        case (uint8_t)IP_ADDRESS_TYPE_IPV6:
            
            pCurrPkt = TxSktGetLockedV6Pkt(pSkt, true);
            if(pCurrPkt != NULL)
            {
                UDPv6FreePacket(pSkt->pV6Pkt);
                pSkt->txStart = NULL;
            }
            break;
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
        case (uint8_t)IP_ADDRESS_TYPE_IPV4:
            pCurrPkt = TxSktGetLockedV4Pkt(pSkt, true);
            if(pCurrPkt != NULL)
            {
                TCPIP_MAC_PACKET* pPkt = &pSkt->pV4Pkt->macPkt;
#if (TCPIP_UDP_USE_POOL_BUFFERS != 0)
                if((pPkt->pktFlags & (uint32_t)UDP_SOCKET_POOL_BUFFER_FLAG) != 0U)
                {   // re-insert in the pool
                    PoolAddPktLocked(pPkt);
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
            // do nothing
            break;
    }


    if(pCurrPkt != NULL)
    {   // count only the not in flight packet
        pSkt->txAllocCnt--;
    }
    pSkt->pPkt = NULL;
}

// frees the associated socket RX packet queue
static void UDPFreeRxQueue(UDP_SOCKET_DCPT* pSkt)
{
    SGL_LIST_NODE* node;
    TCPIP_MAC_PACKET*   pMacPkt;

    while(true)
    {
        node = TCPIP_Helper_SingleListHeadRemove(&pSkt->rxQueue);
        if(node == NULL)
        {
            break;
        }

        pMacPkt = FC_SglNode2MacPkt(node); 
        UDP_RxPktAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_PROTO_DEST_CLOSE);
    }
}

bool TCPIP_UDP_SocketInfoGet(UDP_SOCKET hUDP, UDP_SOCKET_INFO* pInfo)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);
    uint8_t sktFlags;

    if(pSkt == NULL || pInfo == NULL)
    {
        return false;
    }

    (void) memset(pInfo, 0, sizeof(*pInfo));

    while(true)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != NULL)
            {
                (void) memcpy(pInfo->remoteIPaddress.v6Add.v, TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt)->v, sizeof(IPV6_ADDR));
                (void) memcpy(pInfo->localIPaddress.v6Add.v, TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt)->v, sizeof(IPV6_ADDR));
                (void) memcpy(pInfo->sourceIPaddress.v6Add.v, TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt)->v, sizeof(IPV6_ADDR));
                (void) memcpy(pInfo->destIPaddress.v6Add.v, TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt)->v, sizeof(IPV6_ADDR));
            }
            pInfo->addressType = IP_ADDRESS_TYPE_IPV6;
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)


#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
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
    pInfo->rxQueueSize = (uint16_t)TCPIP_Helper_SingleListCount(&pSkt->rxQueue);
    pInfo->txSize = (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);

    sktFlags = 0U;
    if(pSkt->flags.looseRemPort == 0U)
    {
        sktFlags |= (uint8_t)UDP_SOCKET_FLAG_STRICT_PORT;
    }
    if((pSkt->extFlags & (uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_PORT) != 0U)
    {
        sktFlags |= (uint8_t)UDP_SOCKET_FLAG_STICKY_PORT;
    }
    if(pSkt->flags.looseNetIf == 0U)
    {
        sktFlags |= (uint8_t)UDP_SOCKET_FLAG_STRICT_NET;
    }
    if((pSkt->extFlags & (uint8_t)UDP_EFLAG_STICKY_LOOSE_NET_IF) != 0U)
    {
        sktFlags |= (uint8_t)UDP_SOCKET_FLAG_STICKY_NET;
    }
    if(pSkt->flags.looseRemAddress == 0U)
    {
        sktFlags |= (uint8_t)UDP_SOCKET_FLAG_STRICT_ADD;
    }
    if((pSkt->extFlags & (uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_ADDRESS) != 0U)
    {
        sktFlags |= (uint8_t)UDP_SOCKET_FLAG_STICKY_ADD;
    }

    pInfo->flags = (UDP_SOCKET_FLAGS)sktFlags;
    return true;

}

bool TCPIP_UDP_TxOffsetSet(UDP_SOCKET hUDP, uint16_t wOffset, bool relative)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if(pSkt != NULL)
    {
        if(UDPTxPktValid(pSkt))
        {
            uint8_t* pNewWrite = relative ? pSkt->txWrite : pSkt->txStart;
            pNewWrite += wOffset;

            if(pSkt->txStart <= pNewWrite && pNewWrite <= pSkt->txEnd)
            {
                pSkt->txWrite = pNewWrite;
                return true;
            }        
        }
    }

    return false;
}

uint8_t* TCPIP_UDP_TxPointerGet(UDP_SOCKET s)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(s);

    if(pSkt != NULL)
    {
        if(UDPTxPktValid(pSkt))
        {
            return pSkt->txWrite;
        }
    }

    return NULL;
}

void TCPIP_UDP_RxOffsetSet(UDP_SOCKET hUDP, uint16_t rOffset)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if((pSkt != NULL) && pSkt->pCurrRxPkt != NULL)
    {
        // set the packet segments from the beginning and discard
        UDPResetRxPacket(pSkt, pSkt->pCurrRxPkt);
        (void) TCPIP_UDP_ArrayGet(hUDP, NULL, rOffset);
    }
}




uint16_t TCPIP_UDP_PutIsReady(UDP_SOCKET hUDP)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);
    if(pSkt == NULL)
    { 
        return 0;
    }

#if defined (TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        return UDPv6IsTxPutReady(pSkt, pSkt->txSize);
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {
        return UDPv4IsTxPutReady(pSkt);
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

    return 0;   // can happen if it is a server socket and opened with IP_ADDRESS_TYPE_ANY
                // and no client connected to it

}

uint16_t TCPIP_UDP_TxPutIsReady(UDP_SOCKET hUDP, unsigned short count)
{
    return TCPIP_UDP_PutIsReady(hUDP);
}



uint16_t TCPIP_UDP_Put(UDP_SOCKET hUDP, uint8_t v)
{
    return TCPIP_UDP_ArrayPut(hUDP, &v, 1);
}

uint16_t TCPIP_UDP_ArrayPut(UDP_SOCKET hUDP, const uint8_t *cData, uint16_t wDataLen)
{
    if(cData != NULL && wDataLen != 0U)
    {
        UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

        if(pSkt != NULL)
        {
            if(UDPTxPktValid(pSkt))
            {
                uint16_t wrSpace = (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txWrite);
                if(wDataLen > wrSpace)
                {
                    wDataLen = wrSpace;
                }

                if(wDataLen != 0U)
                {
                    (void)TCPIP_Helper_Memcpy(pSkt->txWrite, cData, wDataLen);
                    pSkt->txWrite += wDataLen;
                }

                return wDataLen;
            }
        }
    }

    return 0;
}

const uint8_t* TCPIP_UDP_StringPut(UDP_SOCKET hUDP, const uint8_t *strData)
{
    if(strData != NULL)
    {
        UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);
        if(pSkt != NULL)
        {
            return strData + TCPIP_UDP_ArrayPut(hUDP, strData, (uint16_t)strlen((const char*)strData));
        }
    }

    return NULL;
}



uint16_t TCPIP_UDP_Flush(UDP_SOCKET hUDP)
{
    uint16_t payload;
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if(pSkt != NULL)
    {
        if(UDPTxPktValid(pSkt))
        {
            if(pSkt->flags.txSplitAlloc == 0U)
            {
                payload = (uint16_t)((uintptr_t)pSkt->txWrite - (uintptr_t)pSkt->txStart);
            }
            else
            {
                payload = ((UDP_V4_ZC_PACKET*)pSkt->pPkt)->zcSeg->segLen;
            }

            if(payload != 0U)
            {
#if defined(TCPIP_STACK_USE_IPV6)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
                {
                    return UDPv6Flush(pSkt);
                }
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
                {
                    return UDPv4Flush(pSkt);
                }
#endif  // defined (TCPIP_STACK_USE_IPV4)

            }
        }
    }

    return 0;
}


uint16_t TCPIP_UDP_TxCountGet(UDP_SOCKET hUDP)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if(pSkt != NULL)
    {
        if(UDPTxPktValid(pSkt))
        {
            return (uint16_t)((uintptr_t)pSkt->txWrite - (uintptr_t)pSkt->txStart);
        }
    }

    return 0;
}


/****************************************************************************
  Section:
    Receive Functions
  ***************************************************************************/

uint16_t TCPIP_UDP_GetIsReady(UDP_SOCKET hUDP)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);
    
    if(pSkt == NULL)
    { 
        return 0;
    }

    if(pSkt->pCurrRxSeg == NULL || pSkt->rxTotLen == 0U)
    {   // no more data in this packet 
        UDPUpdatePacketLock(pSkt);
    }

    return pSkt->pCurrRxSeg == NULL ? 0U : pSkt->rxTotLen;

}

uint16_t TCPIP_UDP_Get(UDP_SOCKET hUDP, uint8_t *v)
{
    return TCPIP_UDP_ArrayGet(hUDP, v, 1);
}

uint16_t TCPIP_UDP_ArrayGet(UDP_SOCKET hUDP, uint8_t *cData, uint16_t wDataLen)
{
    TCPIP_MAC_DATA_SEGMENT *pSeg;
    uint16_t    xtractBytes;
    uint16_t    avlblBytes;

    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if(wDataLen == 0U || pSkt == NULL)
    {   // do not advance the current RX packet if no data is requested
        return 0;
    }
    
    if(pSkt->pCurrRxSeg == NULL && pSkt->flags.rxAutoAdvance != 0U)
    {   // no more data in this packet 
        UDPUpdatePacketLock(pSkt);
    }

    avlblBytes = 0;
    while(wDataLen != 0U && pSkt->rxTotLen != 0U)
    {
        pSeg = pSkt->pCurrRxSeg;
        if(pSeg == NULL)
        {
            break;
        }

        xtractBytes = wDataLen <= pSkt->rxSegLen ? wDataLen : pSkt->rxSegLen;
        if(xtractBytes > pSkt->rxTotLen)
        {
            xtractBytes = pSkt->rxTotLen;
        }

        if(xtractBytes != 0U)
        {
            if(cData != NULL)
            {
                (void)TCPIP_Helper_Memcpy(cData, pSkt->rxCurr, xtractBytes);     
                cData += xtractBytes;
            }
            // adjust
            wDataLen -= xtractBytes;
            avlblBytes += xtractBytes;

            pSkt->rxTotLen -= xtractBytes;
            pSkt->rxSegLen -= xtractBytes;
            pSkt->rxCurr += xtractBytes;
        }

        if(pSkt->rxSegLen == 0U)
        {   // go to the next segment in the packet
            pSeg = pSeg->next;
            if((pSkt->pCurrRxSeg = pSeg) != NULL)
            {
                pSkt->rxSegLen = pSeg->segLen;
                pSkt->rxCurr = pSeg->segLoad;
            }
            else
            {
                pSkt->rxSegLen = 0;
                pSkt->rxCurr = NULL;

#if (TCPIP_IPV4_UDP_FRAGMENTATION != 0)
                TCPIP_MAC_PACKET* pFrag;
                pFrag = pSkt->pCurrFrag = pSkt->pCurrFrag->pkt_next;
                if(pFrag != NULL)
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

    if(pSkt->rxTotLen == 0U && pSkt->flags.rxAutoAdvance != 0U)
    {   // done with this packet
        UDPUpdatePacketLock(pSkt);
    }

    return avlblBytes;
}

uint16_t TCPIP_UDP_Discard(UDP_SOCKET hUDP)
{
    uint16_t nBytes = 0;
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    if(pSkt != NULL)
    {
        if(pSkt->pCurrRxSeg != NULL)
        {
            nBytes = pSkt->rxTotLen;
        }
        UDPUpdatePacketLock(pSkt);
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
static UDP_SOCKET_DCPT* UDPFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, UDP_HEADER *h, IP_ADDRESS_TYPE addressType)
{
    int sktIx;
    UDP_SOCKET_DCPT *pSkt;
    const TCPIP_NET_IF* pPktIf;
    uint16_t exactMatch, looseMatch;
    OSAL_CRITSECT_DATA_TYPE critStatus;
    // snapshot of socket settings
    UDP_PORT u_localPort = 0, u_remotePort = 0;
    uint16_t u_addType = 0;
    const TCPIP_NET_IF* u_pSktNet = NULL;
#if defined (TCPIP_STACK_USE_IPV4)
    IPV4_ADDR u_pktSrcAddress;
    u_pktSrcAddress.Val = 0;
#endif // defined (TCPIP_STACK_USE_IPV4)
    TCPIP_UDP_SKT_FLAGS u_flags; 
    u_flags.Val = 0;


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
    

    pPktIf = (const TCPIP_NET_IF*)pRxPkt->pktIf;
    for(sktIx = 0; sktIx < nUdpSockets; sktIx++)
    {
        bool processSkt = false;
        critStatus = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        while(true)
        {
            pSkt = udpSocketDcpt[sktIx];
            if(pSkt == NULL) 
            {   // not valid socket
                break;
            }
            if(RxSktIsLocked(pSkt)) 
            {   // socket disabled
                break;
            }

            if((uint16_t)TCPIP_Helper_SingleListCount(&pSkt->rxQueue) >= pSkt->rxQueueLimit)
            {   // RX limit exceeded
                break;
            }

            // take a snapshot of socket settings
            u_localPort = pSkt->localPort;
            u_addType = pSkt->addType;
            u_remotePort = pSkt->remotePort;
            u_flags.Val = pSkt->flags.Val;
            u_pSktNet = pSkt->pSktNet;
#if defined (TCPIP_STACK_USE_IPV4)
            u_pktSrcAddress.Val = pSkt->pktSrcAddress.Val;
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
        if(u_localPort != h->DestinationPort)
        {   // cannot handle this port
            continue;
        }
       
        exactMatch = looseMatch = 0U;

        // 2. packet address type
        if(u_addType == (uint16_t)addressType)
        {
            exactMatch = (uint16_t)TCPIP_UDP_PKT_MATCH_IP_TYPE;
        }
        else if(u_addType == (uint16_t)IP_ADDRESS_TYPE_ANY)
        {
            looseMatch = (uint16_t)TCPIP_UDP_PKT_MATCH_IP_TYPE;
        }
        else
        {   // cannot handle this address type
            continue;
        }

        // 3. packet source port
        if(u_remotePort == h->SourcePort)
        {
            exactMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_SRC_PORT;
        }
        else if(u_flags.looseRemPort != 0U)
        {
            looseMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_SRC_PORT;
        }
        else
        {
            /* Do Nothing */
        }

        // 4. packet incoming interface
#if defined (TCPIP_STACK_USE_IPV4)
        if(addressType == IP_ADDRESS_TYPE_IPV4)
        {
            if(u_pSktNet == pPktIf)
            {
                exactMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_NET;
            }
            else if(u_pSktNet == NULL || u_flags.looseNetIf != 0U)
            {
                looseMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_NET;
            }
            else
            {
                /* Do Nothing */
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
        if(addressType == IP_ADDRESS_TYPE_IPV6)
        {
            if(u_pSktNet == pPktIf)
            {
                const IPV6_ADDR* pPktDstAddress = TCPIP_IPV6_PacketGetDestAddress(pRxPkt);
                IPV6_ADDRESS_TYPE pktAddressType = (IPV6_ADDRESS_TYPE)TCPIP_IPV6_AddressTypeGet (pPktIf, pPktDstAddress);
                
                if(TCPIP_IPV6_AddressFind(pPktIf, pPktDstAddress, (IPV6_ADDR_TYPE)pktAddressType.bits.type) != NULL)
                {   // interface match
                    exactMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_NET;
                }
            }
            else if(u_pSktNet == NULL || u_flags.looseNetIf != 0U)
            {
                looseMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_NET;
            }
            else
            {
                /* Do Nothing */
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        // 5. packet source address
#if defined (TCPIP_STACK_USE_IPV4)
        if(addressType == IP_ADDRESS_TYPE_IPV4)
        {
            if(u_pktSrcAddress.Val == 0U || u_flags.looseRemAddress != 0U)
            {
                looseMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_SRC_ADD;
            }
            else if(u_pktSrcAddress.Val == TCPIP_IPV4_PacketGetSourceAddress(pRxPkt)->Val)
            {
                exactMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_SRC_ADD;
            }
            else
            {
                /* Do Nothing */
            }
        }
#endif // defined (TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
        if(addressType == IP_ADDRESS_TYPE_IPV6)
        {
            // no IPv6 check done
            exactMatch |= (uint16_t)TCPIP_UDP_PKT_MATCH_SRC_ADD;
        }
#endif // defined(TCPIP_STACK_USE_IPV6)

        // finally check the match we got
        if(exactMatch == (uint16_t)TCPIP_UDP_PKT_MATCH_MASK)
        {   // perfect match
            return pSkt;
        }
        else if( (looseMatch | exactMatch) == (uint16_t)TCPIP_UDP_PKT_MATCH_MASK )
        {   // overall match; adjust and return
#if defined (TCPIP_STACK_USE_IPV6)
            if (addressType == IP_ADDRESS_TYPE_IPV6)
            {   // lazy allocation does not work for IPv6
                // This is expensive and IPv6 should be able to delay the allocation, like IPv4 does!
                // avoid user threads mess with this
                if(pSkt->pV6Pkt == NULL)
                {   // could be a server socket opened with IP_ADDRESS_TYPE_ANY
                    IPV6_PACKET* pNewPkt = UDPv6AllocateTxPacketStruct(pPktIf, pSkt, false);
                    if(pNewPkt == NULL)
                    {   // failed to allocate memory; not much we can do
                        return NULL;
                    }

                    // stop the user threads from messing with this socket TX buffer
                    bool useOldPkt = false;
                    critStatus = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                    if(pSkt->pV6Pkt == NULL)
                    {   // we can use the new packet
                        UDPSocketTxSet(pSkt, pNewPkt, pNewPkt->clientData, IP_ADDRESS_TYPE_IPV6);
                    }
                    else
                    {
                        useOldPkt = true;
                    }
                    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStatus);

                    if(useOldPkt)
                    {
                        UDPv6FreePacket(pNewPkt);
                    }
                }
            }
#endif  // defined (TCPIP_STACK_USE_IPV6)
            
            pSkt->addType = (uint8_t)addressType;
            return pSkt;
        }
        else
        {
            /* Do Nothing */
        }

        // no match, continue
    }

    // not found
    return NULL;
}

bool TCPIP_UDP_SocketNetSet(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);
    if(pSkt != NULL)
    {
        TCPIP_NET_IF* pIf = TCPIPStackHandleToNetUp(hNet);
        // don't do any check here;
        // user can clear the assigned interface

        RxSktLock(pSkt);
        if((pSkt->pSktNet = pIf) != NULL && pSkt->flags.noNetStrict == 0U)
        {   // specific bind requested
            if((pSkt->extFlags & (uint8_t)UDP_EFLAG_STICKY_LOOSE_NET_IF) == 0U)
            {
                pSkt->flags.looseNetIf = 0;
            }
        }


#if defined (TCPIP_STACK_USE_IPV6)
        if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt == NULL)
            {
                RxSktUnlock(pSkt);
                return false;
            }

            pSkt->pV6Pkt->netIfH = pIf;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        RxSktUnlock(pSkt);
        return true;
    }
    return false;
}

TCPIP_NET_HANDLE TCPIP_UDP_SocketNetGet(UDP_SOCKET hUDP)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);
    
    return (pSkt != NULL) ? pSkt->pSktNet : NULL;
}

// sets the source IP address of a packet
bool TCPIP_UDP_SourceIPAddressSet(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* localAddress)
{
    UDP_SOCKET_DCPT* pSkt = UDPSocketDcpt(hUDP);

    return (pSkt != NULL) ? UDPSetSourceAddress(pSkt, addType, localAddress) : false;
}

bool TCPIP_UDP_Bind(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, UDP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)
{
    IP_ADDRESS_TYPE oldAddType;
    TCPIP_NET_IF* pSktIf;
    UDP_SOCKET_DCPT* pSkt;
    bool        portFail, bindSuccess;

    pSkt = UDPSocketDcpt(hUDP);
    if(pSkt == NULL)
    {
        return false;
    }

    oldAddType = (IP_ADDRESS_TYPE)pSkt->addType;
    if(addType == IP_ADDRESS_TYPE_ANY)
    {
        if(oldAddType != IP_ADDRESS_TYPE_ANY)
        {   // cannot change the type w/o a disconnect...
            return false;
        }
        
        // don't use the remote address for ANY
        localAddress = NULL;
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
    pSktIf = NULL;
    if(localAddress != NULL)
    {
#if defined (TCPIP_STACK_USE_IPV4)
        if (addType == IP_ADDRESS_TYPE_IPV4 && localAddress->v4Add.Val != 0U)
        {
            if((pSktIf = TCPIP_STACK_IPAddToNet(&localAddress->v4Add, false)) == NULL)
            {    // no such interface
                return false;
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            if((pSktIf = TCPIPStackIPv6AddToNet(&localAddress->v6Add, IPV6_ADDR_TYPE_UNICAST, false)) == NULL)
            {    // no such interface
                return false;
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)
    }

    portFail = false;
    UserGblLock();
    if(localPort == 0U)
    {
        if((localPort = pSkt->localPort) == 0U)
        {
            localPort = UDPAllocateEphemeralPort();
            if(localPort == 0U)
            {
                portFail = true;
            }
        }
        // else let the old local port be
    }
    else if(localPort != pSkt->localPort && UDPIsAvailablePort(localPort) == false)
    {
        portFail = true;
    }
    else
    {
        /* Do Nothing */
    }
    UserGblUnlock();

    if(portFail)
    {
        return false;
    }

    // success; bind
    RxSktLock(pSkt);
    pSkt->addType = (uint8_t)addType;
    bindSuccess = UDPSocketBind(pSkt, pSktIf, localAddress);
    if(localAddress == NULL)
    {   // if no localAddress, ignore the failure result
        bindSuccess = true;
    }
    if(bindSuccess)
    {
        pSkt->localPort = localPort;
    }
    else
    {   // restore old add type
        pSkt->addType = (uint8_t)oldAddType;
    }

    RxSktUnlock(pSkt);

    return bindSuccess;
}

bool TCPIP_UDP_RemoteBind(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, UDP_PORT remotePort,  IP_MULTI_ADDRESS* remoteAddress)
{
    IP_ADDRESS_TYPE oldAddType;
    UDP_SOCKET_DCPT *pSkt = UDPSocketDcpt(hUDP);

    if(pSkt == NULL)
    {   // no such socket
        return false;
    }

    oldAddType = (IP_ADDRESS_TYPE)pSkt->addType;
    if(addType == IP_ADDRESS_TYPE_ANY)
    {
        if(oldAddType != IP_ADDRESS_TYPE_ANY)
        {   // cannot change the type w/o a disconnect...
            return false;
        }
        
        // don't use the remote address for ANY
        remoteAddress = NULL;
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

    RxSktLock(pSkt);
#if defined (TCPIP_STACK_USE_IPV6)
    if(addType == IP_ADDRESS_TYPE_IPV6 && oldAddType == IP_ADDRESS_TYPE_ANY)
    {   // change of type; special IPv6 case: IPv6 cannot handle lazy allocation!
        const TCPIP_NET_IF* pBindIf;
        if((pBindIf = pSkt->pSktNet) == NULL)
        {   // use the default; IPv6 cannot handle no interface!
            pBindIf = (const TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet();
        } 
        (void) UDPv6AllocateTxPacketStruct (pBindIf, pSkt, true);
        if(pSkt->pPkt == NULL)
        {   // out of memory
            RxSktUnlock(pSkt);
            return false;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    // set the new parameters
    pSkt->addType = (uint8_t)addType;    // bind it
    if(remoteAddress != NULL)
    {
        if(!TCPIP_UDP_DestinationIPAddressSet(hUDP, addType, remoteAddress))
        {   // failed
            pSkt->addType = (uint8_t)oldAddType; // restore
            RxSktUnlock(pSkt);
            return false;
        }

#if defined (TCPIP_STACK_USE_IPV4)
        if(addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            pSkt->pktSrcAddress.Val = remoteAddress->v4Add.Val;
        } 
#endif  // defined (TCPIP_STACK_USE_IPV4)

        if((pSkt->extFlags & (uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_ADDRESS) == 0U)
        {
            pSkt->flags.looseRemAddress = 0;
        }
    }

    pSkt->remotePort = remotePort;
    if((pSkt->extFlags & (uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_PORT) == 0U)
    {
        pSkt->flags.looseRemPort = 0;
    }
    RxSktUnlock(pSkt);
    return true;
}

// Allows setting options to a socket like enable broadcast, Rx/Tx buffer size, etc
bool TCPIP_UDP_OptionsSet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam)
{
    UDP_SOCKET_DCPT*  pSkt = UDPSocketDcpt(hUDP);
    bool res = false;

    if(pSkt != NULL)
    {
        uint32_t wOption = (uint32_t)option;
        int stickyOp = 0;   // 0 = no op; 1 = set; 2 = clr  
        if((wOption & (uint32_t)UDP_OPTION_STRICT_SET_STICKY) != 0U)
        {
            stickyOp = 1;
        }
        else if((wOption & (uint32_t)UDP_OPTION_STRICT_CLR_STICKY) != 0U)
        {
            stickyOp = 2;
        }
        else
        {
            /* Do Nothing */
        }       

        union
        {
            void*       optParam;
            uintptr_t   paramPtr;
            uint32_t    param32;
            unsigned int iparam;
            uint16_t    param16;
            uint8_t     param8;
        }U_OPT_PARAM;
        U_OPT_PARAM.optParam = optParam;
        wOption &= ~((uint32_t)UDP_OPTION_STRICT_SET_STICKY | (uint32_t)UDP_OPTION_STRICT_CLR_STICKY);
        res = true;
        switch(wOption)
        {
            case (uint32_t)UDP_OPTION_STRICT_PORT:
                pSkt->flags.looseRemPort = (optParam == NULL) ? 1U : 0U;
                if(stickyOp == 1)
                {
                    pSkt->extFlags |=  (uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_PORT;
                }
                else if(stickyOp == 2)
                {   // clear
                    pSkt->extFlags &=  ~(uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_PORT;
                }
                else
                {   // no change
                }
                break;

            case (uint32_t)UDP_OPTION_STRICT_NET:
                pSkt->flags.looseNetIf = (optParam == NULL) ? 1U : 0U;
                if(stickyOp == 1)
                {
                    pSkt->extFlags |=  (uint8_t)UDP_EFLAG_STICKY_LOOSE_NET_IF;
                }
                else if(stickyOp == 2)
                {   // clear
                    pSkt->extFlags &=  ~(uint8_t)UDP_EFLAG_STICKY_LOOSE_NET_IF;
                }
                else
                {   // no change
                }
                break;

            case (uint32_t)UDP_OPTION_STRICT_ADDRESS:
                pSkt->flags.looseRemAddress = (optParam == NULL) ? 1U : 0U;
                if(stickyOp == 1)
                {
                    pSkt->extFlags |=  (uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_ADDRESS;
                }
                else if(stickyOp == 2)
                {   // clear
                    pSkt->extFlags &=  ~(uint8_t)UDP_EFLAG_STICKY_LOOSE_REM_ADDRESS;
                }
                else
                {   // no change
                }
                break;

            case (uint32_t)UDP_OPTION_BROADCAST:
                pSkt->flags.bcastForceType = U_OPT_PARAM.param8;
                if(pSkt->flags.bcastForceType != (unsigned int)UDP_BCAST_NONE)
                {   // set limited broadcast address (for now)
                    pSkt->destAddress.Val = 0xffffffffU;
                    pSkt->flags.destSet = 1;
                } 
                else
                {   // the discrete address will have to be set and take effect
                    pSkt->destAddress.Val = 0;
                    pSkt->flags.destSet = 0;
                }

                break;

            case (uint32_t)UDP_OPTION_BUFFER_POOL:
#if defined (TCPIP_STACK_USE_IPV6)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
                {   
                    res = false;
                    break;
                }
#endif  // defined (TCPIP_STACK_USE_IPV6)
                if(pSkt->flags.txSplitAlloc != 0U)
                {   // no support for external payload sockets
                    res = false;
                    break;
                }

                if(pSkt->flags.usePool != (unsigned int)(optParam != NULL))
                {   // changed the buffer type; just release the packet
                    UDPFreeTxResources(pSkt);
                    pSkt->flags.usePool = (optParam != NULL) ? 1U : 0U;
                }
                break;

            case (uint32_t)UDP_OPTION_TX_BUFF:
                // just release the packet
                UDPFreeTxResources(pSkt);
                pSkt->txSize = U_OPT_PARAM.param16;
                break;

            case (uint32_t)UDP_OPTION_TX_QUEUE_LIMIT:
                pSkt->txAllocLimit = U_OPT_PARAM.param16;
                break;

            case (uint32_t)UDP_OPTION_RX_QUEUE_LIMIT:
                pSkt->rxQueueLimit = U_OPT_PARAM.param16;
                break;

            case (uint32_t)UDP_OPTION_RX_AUTO_ADVANCE:
                pSkt->flags.rxAutoAdvance = (optParam != NULL) ? 1U : 0U;
                break;

            case (uint32_t)UDP_OPTION_TX_TTL:
                pSkt->ttl = U_OPT_PARAM.param8;
                break;

            case (uint32_t)UDP_OPTION_MULTICAST:
                {
                    UDP_OPTION_MULTICAST_DATA* pMcOpt = (UDP_OPTION_MULTICAST_DATA*)optParam;
                    if(pMcOpt!= NULL)
                    {
                        uint16_t flagsMask = (uint16_t)pMcOpt->flagsMask;
                        uint16_t flagsValue = (uint16_t)pMcOpt->flagsValue;
                        if((flagsMask & (uint16_t)UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0U)
                        {   // change the source/net/source/port value
                            pSkt->flags.looseNetIf = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0U) ? 1U : 0U;
                            pSkt->flags.looseRemAddress = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0U) ? 1U : 0U;
                            pSkt->flags.looseRemPort = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT) != 0U) ? 1U : 0U;
                        }
                        if((flagsMask & (uint16_t)UDP_MCAST_FLAG_IGNORE_SOURCE_ADD) != 0U)
                        {
                            pSkt->flags.fixedDestAddress = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_IGNORE_SOURCE_ADD) != 0U) ? 1U : 0U;
                        }
                        if((flagsMask & (uint16_t)UDP_MCAST_FLAG_IGNORE_SOURCE_PORT) != 0U)
                        {
                            pSkt->flags.fixedDestPort = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_IGNORE_SOURCE_PORT) != 0U) ? 1U : 0U;
                        }
                        if((flagsMask & (uint16_t)UDP_MCAST_FLAG_IGNORE_UNICAST) != 0U)
                        {
                            pSkt->flags.mcastOnly = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_IGNORE_UNICAST) != 0U) ? 1U : 0U;
                        }
                        if((flagsMask & (uint16_t)UDP_MCAST_FLAG_LOOP) != 0U)
                        {
                            pSkt->flags.mcastLoop = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_LOOP) != 0U) ? 1U : 0U;
                        }
                        if((flagsMask & (uint16_t)UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK) != 0U)
                        {
                            pSkt->flags.mcastSkipCheck = ((flagsValue & (uint16_t)UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK) != 0U) ? 1U : 0U;
                        }
                    }
                    else
                    {
                        res = false;
                    }

                    break;
                }

            case (uint32_t)UDP_OPTION_TOS:
                pSkt->flags.tos = U_OPT_PARAM.param8;
                break;
                
            case (uint32_t)UDP_OPTION_DF:
                pSkt->flags.df = (optParam != NULL) ? 1U : 0U;
                break;
                
            case (uint32_t)UDP_OPTION_FIXED_DEST_ADDRESS:
                pSkt->flags.fixedDestAddress = (optParam != NULL) ? 1U : 0U;
                break;
                
            case (uint32_t)UDP_OPTION_FIXED_DEST_PORT:
                pSkt->flags.fixedDestPort = (optParam != NULL) ? 1U : 0U;
                break;
                
            case (uint32_t)UDP_OPTION_ENFORCE_STRICT_NET:
                pSkt->flags.noNetStrict = (optParam == NULL) ? 1U : 0U;
                break;
                
            default:
                res = false;
                break;
        }
    }    

    return res;
}

// Allows getting options to a socket like enable broadcast, Rx/Tx buffer size, etc
bool TCPIP_UDP_OptionsGet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam)
{
    UDP_SOCKET_DCPT *pSkt = UDPSocketDcpt(hUDP);
    bool res =  false;

    if((pSkt != NULL) && (optParam != NULL))
    {
        res = true;
        switch(option)
        {
            case UDP_OPTION_STRICT_PORT:
                *(bool*)optParam = pSkt->flags.looseRemPort == 0U;
                break;

            case UDP_OPTION_STRICT_NET:
                *(bool*)optParam = pSkt->flags.looseNetIf == 0U;
                break;

            case UDP_OPTION_STRICT_ADDRESS:
                *(bool*)optParam = pSkt->flags.looseRemAddress == 0U;
                break;

            case UDP_OPTION_BROADCAST:
                *(UDP_SOCKET_BCAST_TYPE*)optParam = (UDP_SOCKET_BCAST_TYPE)pSkt->flags.bcastForceType;
                break;

            case UDP_OPTION_BUFFER_POOL:
                *(bool*)optParam = pSkt->flags.usePool != 0U;
                break;

            case UDP_OPTION_TX_BUFF:
                *(uint16_t*)optParam = pSkt->txSize;
                break;
                
            case UDP_OPTION_TX_QUEUE_LIMIT:
                *(uint16_t*)optParam = pSkt->txAllocLimit;
                break;

            case UDP_OPTION_RX_QUEUE_LIMIT:
                *(uint16_t*)optParam = pSkt->rxQueueLimit;
                break;

            case UDP_OPTION_RX_AUTO_ADVANCE:
                *(bool*)optParam = pSkt->flags.rxAutoAdvance != 0U;
                break;

            case UDP_OPTION_TX_TTL:
                *(uint8_t*)optParam = pSkt->ttl;
                break;

            case UDP_OPTION_MULTICAST:
                {
                    UDP_MULTICAST_FLAGS* pMcFlags = (UDP_MULTICAST_FLAGS*)optParam;
                    uint16_t mcFlags = 0;

                    if(pSkt->flags.looseNetIf == 1U && pSkt->flags.looseRemAddress == 1U && pSkt->flags.looseRemPort == 1U)
                    {
                        mcFlags |= (uint16_t)UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT;
                    }
                    if(pSkt->flags.fixedDestAddress != 0U)
                    {
                        mcFlags |= (uint16_t)UDP_MCAST_FLAG_IGNORE_SOURCE_ADD;
                    }
                    if(pSkt->flags.fixedDestPort != 0U)
                    {
                        mcFlags |= (uint16_t)UDP_MCAST_FLAG_IGNORE_SOURCE_PORT;
                    }
                    if(pSkt->flags.mcastOnly != 0U)
                    {
                        mcFlags |= (uint16_t)UDP_MCAST_FLAG_IGNORE_UNICAST;
                    }
                    if(pSkt->flags.mcastLoop != 0U)
                    {
                        mcFlags |= (uint16_t)UDP_MCAST_FLAG_LOOP;
                    }
                    if(pSkt->flags.mcastSkipCheck != 0U)
                    {
                        mcFlags |= (uint16_t)UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK;
                    }

                    *pMcFlags = (UDP_MULTICAST_FLAGS)mcFlags;

                    break;
                }


             case UDP_OPTION_TOS:
                *(uint8_t*)optParam = (uint8_t)pSkt->flags.tos;
                break;
                
             case UDP_OPTION_DF:
                *(bool*)optParam = pSkt->flags.df != 0U;
                break;

            case UDP_OPTION_FIXED_DEST_ADDRESS:
                *(bool*)optParam = pSkt->flags.fixedDestAddress != 0U;
                break;
                
            case UDP_OPTION_FIXED_DEST_PORT:
                *(bool*)optParam = pSkt->flags.fixedDestPort != 0U;
                break;
                
             case UDP_OPTION_ENFORCE_STRICT_NET:
                *(bool*)optParam = pSkt->flags.noNetStrict == 0U;
                break;
                
           default:
                /* Do Nothing */
                res = false;
                break;
        }
    }    

    return res;
}


bool TCPIP_UDP_DestinationIPAddressSet(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, const IP_MULTI_ADDRESS* remoteAddress)
{
    UDP_SOCKET_DCPT *pSkt;

    if(remoteAddress == NULL)
    {
        return false;
    }

    pSkt = UDPSocketDcpt(hUDP);

    while(pSkt != NULL && pSkt->addType == (uint8_t)addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != NULL)
            {
                TCPIP_IPV6_DestAddressSet (pSkt->pV6Pkt, &remoteAddress->v6Add);
                return true;
            }
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            if(pSkt->flags.bcastForceType != (unsigned int)UDP_BCAST_NONE)
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
    UDP_SOCKET_DCPT *pSkt = UDPSocketDcpt(s);

    if(pSkt != NULL)
    {
        RxSktLock(pSkt);
        pSkt->remotePort = remotePort;
        RxSktUnlock(pSkt);
        return true;
    }

    return false;
}

#if defined (TCPIP_STACK_USE_IPV4)
bool TCPIP_UDP_SetSplitPayload(UDP_SOCKET s, void* pLoad, uint16_t loadSize)
{
    bool    success = false;
    UDP_SOCKET_DCPT *pSkt = UDPSocketDcpt(s);

    if((pSkt != NULL) && (pSkt->flags.txSplitAlloc != 0U) && pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {
        void* pPkt = TxSktGetLockedV4Pkt(pSkt, true);
        if(pPkt == NULL)
        {   // no packet or queued
            (void) UDPv4AllocateSktTxBuffer(pSkt, (IP_ADDRESS_TYPE)pSkt->addType, true);
        }

        if(pSkt->pPkt != NULL)
        {
            TCPIP_MAC_DATA_SEGMENT* pZSeg = ((UDP_V4_ZC_PACKET*)pSkt->pPkt)->zcSeg;
            pZSeg->segLen = pZSeg->segSize = loadSize;
            pZSeg->segLoad = pLoad;

            success = true;
        }
    }

    return success;
}
#endif // defined (TCPIP_STACK_USE_IPV4)



static UDP_PORT UDPAllocateEphemeralPort(void)
{
    uint32_t    num_ephemeral;
    uint32_t    count;
    UDP_PORT next_ephemeral;


    count = num_ephemeral = (uint32_t)TCPIP_UDP_LOCAL_PORT_END_NUMBER - (uint32_t)TCPIP_UDP_LOCAL_PORT_START_NUMBER + 1U;

    next_ephemeral = (uint16_t)((uint32_t)TCPIP_UDP_LOCAL_PORT_START_NUMBER + (SYS_RANDOM_PseudoGet() % num_ephemeral));

    while(count-- != 0U)
    {
        if(UDPIsAvailablePort(next_ephemeral))
        {
            return next_ephemeral;
        }

        if (next_ephemeral == (uint16_t)TCPIP_UDP_LOCAL_PORT_END_NUMBER)
        {
            next_ephemeral = (uint16_t)TCPIP_UDP_LOCAL_PORT_START_NUMBER;
        }
        else
        {
            next_ephemeral++;
        }
    }

    return 0;   // not found
}

static bool UDPIsAvailablePort(UDP_PORT port)
{
    int skt;
    UDP_SOCKET_DCPT *pSkt;

    // Find an available socket that matches the specified socket type
    for(skt = 0; skt < nUdpSockets; skt++)
    {
        pSkt = udpSocketDcpt[skt]; 
        if((pSkt != NULL) && pSkt->localPort == port)
        {
            return false;
        }
    }

    return true;
}

TCPIP_UDP_SIGNAL_HANDLE TCPIP_UDP_SignalHandlerRegister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_TYPE sigMask, TCPIP_UDP_SIGNAL_FUNCTION handler, const void* hParam)
{
    TCPIP_UDP_SIGNAL_FUNCTION sigF = NULL;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if(handler != NULL)
    {
        UDP_SOCKET_DCPT *pSkt = UDPSocketDcpt(s);
        if(pSkt != NULL && pSkt->sigHandler == NULL)
        {
            pSkt->sigHandler = handler;
            pSkt->sigParam = hParam;
            pSkt->sigMask = (uint16_t)sigMask;
            sigF = handler;
            // Note: this may change if multiple notfication handlers required 
        }
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return FC_SigF2SigH(sigF);
}

bool TCPIP_UDP_SignalHandlerDeregister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_HANDLE hSig)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    UDP_SOCKET_DCPT *pSkt = UDPSocketDcpt(s);

    if(pSkt != NULL)
    {  
        if(pSkt->sigHandler == FC_SigH2SigF(hSig))
        {
            pSkt->sigHandler = NULL;
            pSkt->sigMask = 0;
            res = true;
        }
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

size_t TCPIP_UDP_SocketsNumberGet(void)
{
    return (size_t)nUdpSockets;
}

// fragmentation support
#if (TCPIP_IPV4_UDP_FRAGMENTATION != 0)
static void UDP_RxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET *pFragPkt, *pFragNext;

    pFragPkt = pRxPkt;
    while(pFragPkt != NULL)
    {
        pFragNext = pFragPkt->pkt_next;
        TCPIP_PKT_PacketAcknowledge(pFragPkt, ackRes); 
        pFragPkt = pFragNext;
    }
}

#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

// external packet processing
#if (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)
TCPIP_UDP_PROCESS_HANDLE TCPIP_UDP_PacketHandlerRegister(TCPIP_UDP_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_UDP_PACKET_HANDLER pktH = NULL;

    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(udpPktHandler ==NULL)
    {
        udpPktHandlerParam = handlerParam;
        udpPktHandler = pktHandler;
        pktH = pktHandler;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return FC_PktHndl2ProcHndl(pktH);
}

bool TCPIP_UDP_PacketHandlerDeregister(TCPIP_UDP_PROCESS_HANDLE procHandle)
{
    bool res = false;

    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(udpPktHandler == FC_ProcHndl2PktHndl(procHandle))
    {
        udpPktHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}
#else
TCPIP_UDP_PROCESS_HANDLE TCPIP_UDP_PacketHandlerRegister(TCPIP_UDP_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    return NULL;
}
bool TCPIP_UDP_PacketHandlerDeregister(TCPIP_UDP_PROCESS_HANDLE procHandle)
{
    return false;
}
#endif  // (TCPIP_UDP_EXTERN_PACKET_PROCESS != 0)


#endif //#if defined(TCPIP_STACK_USE_UDP)

