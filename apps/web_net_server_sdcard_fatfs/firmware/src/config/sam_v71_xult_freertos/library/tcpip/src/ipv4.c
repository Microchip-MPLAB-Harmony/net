/*******************************************************************************
  Internet Protocol (IP) Version 4 Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides a transport for TCP, UDP, and ICMP messages
    -Reference: RFC 791
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_IPV4

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/ipv4_private.h"

#if defined(TCPIP_STACK_USE_IPV4)


// This is left shifted by 4.  Actual value is 0x04.
#define IPv4_VERSION        (0x04)

#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
  #define IPV4_DEFAULT_TTL           (255)  // Time-To-Live in hops 
  // IP TTL is set to 255 for Multicast DNS compatibility. See mDNS-draft-08, section 4.
#else
  #define IPV4_DEFAULT_TTL           (100)  // Time-To-Live in hops
#endif



static uint16_t             ipv4Identifier = 0;           // Static identifier value for IPv4 headers
static const void*          ipv4MemH = 0;                 // memory handle

static tcpipSignalHandle    signalHandle = 0;

static PROTECTED_SINGLE_LIST ipv4ArpQueue = { {0} };       // queue of packets waiting for ARP resolution

static TCPIP_ARP_HANDLE     ipv4ArpHandle = 0;          // ARP registration handle

static uint16_t             ipv4InitCount = 0;

static PROTECTED_SINGLE_LIST ipv4PacketFilters = { {0} };

static TCPIP_IPV4_FILTER_TYPE ipv4FilterType = 0;       // IPv4 current filter

#if (TCPIP_IPV4_FRAGMENTATION != 0)
static SINGLE_LIST          ipv4FragmentQueue = {0};  // IPv4 fragments to be processed
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

typedef enum
{
    TCPIP_IPV4_DEST_FAIL    = 0,    // cannot find a destination
    TCPIP_IPV4_DEST_SELF    = 1,    // route it internally
    TCPIP_IPV4_DEST_NETWORK = 2,    // route it externally

}TCPIP_IPV4_DEST_TYPE;



/************************************************************************/
/****************               Prototypes               ****************/
/************************************************************************/
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_BASIC) != 0)
volatile int _IPv4StayAssertLoop = 0;
static void _IPv4AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("IPv4 Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_IPv4StayAssertLoop != 0);
    }
}

#else
#define _IPv4AssertCond(cond, message, lineNo)
#endif  // (TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_BASIC)

#if (TCPIP_IPV4_FRAGMENTATION != 0)
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FRAGMENT) != 0)

typedef enum
{
    TCPIP_IPV4_FRAG_NONE,
    TCPIP_IPV4_FRAG_CREATED,                // new fragment created
    TCPIP_IPV4_FRAG_INSERTED,               // fragment inserted into existing chain
    TCPIP_IPV4_FRAG_INSERT_OVERLAP,         // overlap fragment inserted into existing chain
    TCPIP_IPV4_FRAG_COMPLETE,               // fragment assembled
    TCPIP_IPV4_FRAG_DISCARD_EXCEEDED,       // too many segments; discarded
    TCPIP_IPV4_FRAG_DISCARD_OVERLAP,        // overlapped; discarded
    TCPIP_IPV4_FRAG_DISCARD_TMO,            // tmo in reassembly; discarded
}TCPIP_IPV4_FRAG_EVENT_TYPE;

static void _IPv4FragmentDbg(IPV4_FRAGMENT_NODE* pFragNode, TCPIP_MAC_PACKET* pFragPkt, TCPIP_IPV4_FRAG_EVENT_TYPE evType)
{
    IPV4_HEADER *pFHdr;
    uint16_t    fragId;
    uint16_t    segMin, segMax;
    uint32_t    fragTmo;

    if(pFragPkt)
    {
        pFHdr = (IPV4_HEADER*)pFragPkt->pNetLayer;
        segMin = pFHdr->FragmentInfo.fragOffset * 8;
        segMax = segMin + pFragPkt->totTransportLen;
    }
    else
    {
        segMin = segMax = 0;
        pFHdr = (IPV4_HEADER*)pFragNode->fragHead->pNetLayer;
    }

    fragId = pFHdr->Identification;
    fragTmo = pFragNode->fragTmo;

    switch(evType)
    {
        case TCPIP_IPV4_FRAG_CREATED:
            SYS_CONSOLE_PRINT("IPFrag - created; id: %d, tmo: %d, seg: %d - %d\r\n", fragId, fragTmo, segMin, segMax);
            break;

        case TCPIP_IPV4_FRAG_INSERTED:
            SYS_CONSOLE_PRINT("IPFrag - inserted; id: %d, tmo: %d, seg: %d - %d\r\n", fragId, fragTmo, segMin, segMax);
            break;

        case TCPIP_IPV4_FRAG_INSERT_OVERLAP:
            SYS_CONSOLE_PRINT("IPFrag - insert overlap; id: %d, tmo: %d, seg: %d - %d\r\n", fragId, fragTmo, segMin, segMax);
            break;

        case TCPIP_IPV4_FRAG_COMPLETE:
            SYS_CONSOLE_PRINT("IPFrag - completed; id: %d, tmo: %d\r\n", fragId, fragTmo);
            break;

        case TCPIP_IPV4_FRAG_DISCARD_EXCEEDED:
            SYS_CONSOLE_PRINT("IPFrag - discarded-exceed; id: %d, tmo: %d, seg: %d - %d\r\n", fragId, fragTmo, segMin, segMax);
            break;

        case TCPIP_IPV4_FRAG_DISCARD_OVERLAP:
            SYS_CONSOLE_PRINT("IPFrag - discarded-overlap; id: %d, tmo: %d, seg: %d - %d\r\n", fragId, fragTmo, segMin, segMax);
            break;

        case TCPIP_IPV4_FRAG_DISCARD_TMO:
            SYS_CONSOLE_PRINT("IPFrag - discarded-tmo; id: %d, tmo: %d\r\n", fragId, fragTmo);
            break;

        default:
            SYS_CONSOLE_PRINT("IPFrag - unknown event: %d. Retry?\r\n", evType);
            break;
    }

}

#else
#define _IPv4FragmentDbg(pFrag, pFragPkt, evType)
#endif  // (TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FRAGMENT)
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_RX_CHECK) != 0)
static void TCPIP_IPV4_CheckRxPkt(TCPIP_MAC_PACKET* pRxPkt)
{
    IPV4_HEADER* pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;
    if(pHeader->Protocol == IP_PROT_UDP)
    {   // UDP packet
        UDP_HEADER* pUDPHdr = (UDP_HEADER*)pRxPkt->pTransportLayer;
        UDP_PORT destPort = TCPIP_Helper_ntohs(pUDPHdr->DestinationPort);
        UDP_PORT srcPort = TCPIP_Helper_ntohs(pUDPHdr->SourcePort);
        if(srcPort == destPort)
        {
        }
    }
    else if(pHeader->Protocol == IP_PROT_TCP)
    {   
    }

}
#else
#define TCPIP_IPV4_CheckRxPkt(pRxPkt)
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_RX_CHECK) != 0)



static void TCPIP_IPV4_ArpHandler(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType, const void* param);

static bool TCPIP_IPV4_VerifyPkt(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt);

static TCPIP_NET_IF* TCPIP_IPV4_CheckPktTx(TCPIP_NET_HANDLE hNet, TCPIP_MAC_PACKET* pPkt);

static bool TCPIP_IPV4_VerifyPktFilters(TCPIP_MAC_PACKET* pRxPkt);

#if (TCPIP_IPV4_FRAGMENTATION != 0)
static void TCPIP_IPV4_Timeout(void);

// RX fragmentation
static TCPIP_MAC_PKT_ACK_RES    TCPIP_IPV4_RxFragmentInsert(TCPIP_MAC_PACKET* pRxPkt, IPV4_FRAGMENT_NODE **ppFrag);
static void                     TCPIP_IPV4_RxFragmentDiscard(IPV4_FRAGMENT_NODE* pFrag, TCPIP_MAC_PKT_ACK_RES ackRes);
static void                     TCPIP_IPV4_RxFragmentListPurge(SINGLE_LIST* pL);


// TX fragmentation
static bool TCPIP_IPV4_FragmentTxPkt(IPV4_PACKET* pPkt, uint16_t linkMtu, uint16_t pktPayload);
static bool TCPIP_IPV4_FragmentTxAckFnc(TCPIP_MAC_PACKET* pkt,  const void* param);
static void TCPIP_IPV4_FragmentTxAcknowledge(TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PKT_ACK_RES ackRes, IPV4_FRAG_TX_ACK txAck);
static void TCPIP_IPV4_FragmentTxInsertToRx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PACKET_FLAGS flags, bool signal);
static bool TCPIP_IPV4_TxMacPkt(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pPkt);
#else
// if no fragment support, transmit just the head
static __inline__ bool __attribute__((always_inline)) TCPIP_IPV4_TxMacPkt(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pPkt)
{
    return _TCPIPStackPacketTx(pNetIf, pPkt) == TCPIP_MAC_RES_OK;
}

// if no fragment support, acknowledge just the head
static __inline__ void __attribute__((always_inline)) TCPIP_IPV4_FragmentTxAcknowledge(TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PKT_ACK_RES ackRes, IPV4_FRAG_TX_ACK txAck)
{
    if((txAck & IPV4_FRAG_TX_ACK_HEAD) != 0)
    {
        TCPIP_PKT_PacketAcknowledge(pTxPkt, ackRes);
    }
}
// if no fragment support, insert just the head
static __inline__ void __attribute__((always_inline)) TCPIP_IPV4_FragmentTxInsertToRx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PACKET_FLAGS flags, bool signal)
{
    pTxPkt->pktFlags |= flags;
    _TCPIPStackInsertRxPacket(pNetIf, pTxPkt, signal);
}
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_PktMacDestination(IPV4_PACKET* pPkt, const IPV4_ADDR* pIpAdd, TCPIP_MAC_ADDR** ppMacAdd);

static void TCPIP_IPV4_Process(void);

static IPV4_OPTION_FIELD* _IPv4CheckPacketOption(TCPIP_MAC_PACKET* pRxPkt, int* pOptLen);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_IPV4_Cleanup(void);
static void TCPIP_IPV4_ListPurge(PROTECTED_SINGLE_LIST* pList, TCPIP_NET_IF* pNetIf);
#else
#define TCPIP_IPV4_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

/*****************************************************************************
  Function:
	bool TCPIP_IPV4_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, 
        const TCPIP_IPV4_MODULE_CONFIG* pIpInit)

  Summary:
	Initializes the IP module.

  Description:
	Initializes the IP module.  Sets the dynamic heap used by this module.	

  Precondition:
	None

  Parameters:
	stackInit - Stack initialization parameters
    pIpInit - Unused supplementary data.

  Returns:
  	true
  	
  Remarks:
	None
  ***************************************************************************/
bool TCPIP_IPV4_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_IPV4_MODULE_CONFIG* pIpInit)
{
    bool iniRes;

    if(stackInit->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    if(ipv4InitCount == 0)
    {
        _IPv4AssertCond(true, 0, 0);        // hush compiler warning - unused
        while(true)
        {
            ipv4MemH = stackInit->memH;
            ipv4ArpHandle = 0;

#if (TCPIP_IPV4_FRAGMENTATION != 0)
            TCPIP_Helper_SingleListInitialize(&ipv4FragmentQueue);
            iniRes = (signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_IPV4_Task, TCPIP_IPV4_TASK_TICK_RATE)) != 0;
#else
            iniRes = (signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_IPV4_Task, 0)) != 0;
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)
            if(iniRes == false)
            {
                break;
            }

            if((iniRes = TCPIP_Helper_ProtectedSingleListInitialize (&ipv4ArpQueue)) == false)
            {
                break;
            }

            iniRes = TCPIP_Notification_Initialize(&ipv4PacketFilters);

            break;
        }
        
        if(iniRes == false)
        {
            TCPIP_IPV4_Cleanup();
            return false;
        }
    }

    ipv4InitCount++;

    return true;
}

/*****************************************************************************
  Function:
	void TCPIP_IPV4_DeInitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  Summary:
	Deinitializes the IP module.

  Description:
	Deinitializes the IP module.	

  Precondition:
	None

  Parameters:
	stackCtrl - Stack control data

  Returns:
  	None
  	
  Remarks:
	None
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_IPV4_DeInitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(ipv4InitCount > 0)
    {   // up and running
        // one way or another this interface is going down
#if (TCPIP_IPV4_FRAGMENTATION != 0)
        TCPIP_IPV4_RxFragmentListPurge(&ipv4FragmentQueue);
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

        TCPIP_IPV4_ListPurge(&ipv4ArpQueue, stackCtrl->pNetIf);

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down

            if (--ipv4InitCount == 0)
            {   // free the ARP resources
                TCPIP_IPV4_Cleanup();
            }
        }
    }
}

static void TCPIP_IPV4_Cleanup(void)
{
    if(ipv4ArpHandle)
    {
        TCPIP_ARP_HandlerDeRegister(ipv4ArpHandle);
        ipv4ArpHandle = 0;
    }

    TCPIP_Notification_Deinitialize(&ipv4PacketFilters, ipv4MemH);
    ipv4MemH = 0;

    TCPIP_Helper_ProtectedSingleListDeinitialize(&ipv4ArpQueue);
    if(signalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(signalHandle);
        signalHandle = 0;
    }

}

// purges a list of IPV4_PACKET packets
// searches for pNetIf match
// if pNetIf == 0 everything matches
static void TCPIP_IPV4_ListPurge(PROTECTED_SINGLE_LIST* pList, TCPIP_NET_IF* pNetIf)
{
    SINGLE_LIST newList;
    IPV4_PACKET *pPkt, *pNext;

    TCPIP_Helper_SingleListInitialize (&newList);

    TCPIP_Helper_ProtectedSingleListLock(pList);
    // traverse the list
    // and find all the packets matching the pNetIf

    pNext = (IPV4_PACKET*)pList->list.head;

    while((pPkt = pNext) != 0)
    {
        pNext = (IPV4_PACKET*)pPkt->macPkt.next;
            
        if(pNetIf == 0 || pNetIf == (TCPIP_NET_IF*)pPkt->netIfH)
        {   // match
            TCPIP_IPV4_FragmentTxAcknowledge(&pPkt->macPkt, TCPIP_MAC_PKT_ACK_ARP_NET_ERR, IPV4_FRAG_TX_ACK_HEAD | IPV4_FRAG_TX_ACK_FRAGS);
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newList, (SGL_LIST_NODE*)pPkt);
        }
    }

    // update the arp queue
    pList->list = newList;
    TCPIP_Helper_ProtectedSingleListUnlock(pList);
}

#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// selects a source address and an interface based on the IPv4 destination address
// updates the pSrcAddress and returns the needed interface, if successful
// returns 0 if failed
// if srcSet == 1 and netH != 0 the function won't change anything 
// if srcSet == 1 and netH == 0 then the call will never fail 
//      it will use whatever value in pSrcAddress (even 0)
//      and will try to come up with an appropriate interface.
// if srcSet == 0 and netH == 0 it will use the destination address
// if srcSet == 0 and netH != 0 it will use the address of that interface
// returns a valid interface if it succeeds, 0 otherwise
// Condition: netH has to be valid (if non 0)
// The interface is checked to be valid, up and linked
TCPIP_NET_HANDLE TCPIP_IPV4_SelectSourceInterface(TCPIP_NET_HANDLE netH, IPV4_ADDR* pDestAddress, IPV4_ADDR* pSrcAddress, bool srcSet)
{
    int netIx;
    int avlblInterfaces;
    uint32_t    ifAdd, ifMask;
    TCPIP_NET_IF  *pIf;

    pIf = _TCPIPStackHandleToNetUp(netH);

    if(srcSet)
    {   // address in pSrcAddress is valid, no matter what
        if(pIf == 0)
        {   // try to see if it belongs to some interface
            pIf = _TCPIPStackIpAddFromAnyNet(0, pSrcAddress);
            if(pIf == 0)
            {
                pIf = _TCPIPStackAnyNetLinked(true);
            }
        }
        return pIf;
    }

    if(pIf != 0)
    {   // interface is forced
        pSrcAddress->Val = TCPIP_STACK_NetAddressGet(pIf);
        return pIf;
    }

    // use the destination address to decide which interface is this packet going out on

    if(pDestAddress == 0 || pDestAddress->Val == 0)
    {   // unroutable
        return 0;
    }

    if((avlblInterfaces = TCPIP_STACK_NumberOfNetworksGet()) > 1)
    {   // we have a choice
        for(netIx = 0; netIx < avlblInterfaces; netIx++)
        {
            pIf = _TCPIPStackHandleToNetLinked(TCPIP_STACK_IndexToNet(netIx));
            if(pIf)
            {
                ifAdd = TCPIP_STACK_NetAddressGet(pIf);
                ifMask = TCPIP_STACK_NetMaskGet(pIf);

                if((ifAdd & ifMask) == (pDestAddress->Val & ifMask))
                {   // destination address is on this interface
                    pSrcAddress->Val = ifAdd;
                    return pIf;
                }
            }
        }
    }

    // use the default/any interface
    pIf = _TCPIPStackAnyNetLinked(true);
    if(pIf)
    {
        pSrcAddress->Val = TCPIP_STACK_NetAddressGet(pIf);
    }
    return pIf;
}


static TCPIP_NET_IF* TCPIP_IPV4_CheckPktTx(TCPIP_NET_HANDLE hNet, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_NET_IF* pNetIf = 0;

    if(pPkt->next == 0)
    {   // no support for chained packets!
        // make sure the interface is valid
        if((pNetIf = _TCPIPStackHandleToNetLinked(hNet)) != 0)
        {   // cannot transmit over dead interface
            if(_TCPIPStackIsConfig(pNetIf) && (pPkt->pktFlags & TCPIP_MAC_PKT_FLAG_CONFIG) == 0)
            {   // no packets go out in stack configuration
                pNetIf = 0;
            } 
        }
    }

    return pNetIf;
}


// transmits a packet over the network
bool TCPIP_IPV4_PacketTransmit(IPV4_PACKET* pPkt)
{
    TCPIP_MAC_ADDR   destMacAdd, *pMacDst;
    TCPIP_NET_IF*    pNetIf;
    TCPIP_IPV4_DEST_TYPE destType;
    uint16_t        pktPayload, linkMtu;
    bool            txRes;
    
    pNetIf = TCPIP_IPV4_CheckPktTx(pPkt->netIfH, &pPkt->macPkt);

    if(pNetIf == 0)
    {   // cannot transmit over invalid interface
        return false;
    }

    // now select packet's destination MAC address
    pMacDst = &destMacAdd;
    destType = TCPIP_IPV4_PktMacDestination(pPkt, &pPkt->destAddress, &pMacDst);
    if(destType == TCPIP_IPV4_DEST_FAIL) 
    {   // discard, cannot send
        return false;
    }

    // get the payload w/o the MAC frame
    pktPayload = TCPIP_PKT_PayloadLen(&pPkt->macPkt);
    pNetIf = (TCPIP_NET_IF*)pPkt->netIfH;   // re-read; could have changed
    if(!TCPIP_PKT_PacketMACFormat(&pPkt->macPkt, pMacDst, (const TCPIP_MAC_ADDR*)TCPIP_STACK_NetMACAddressGet(pNetIf), TCPIP_ETHER_TYPE_IPV4))
    {   // discard, cannot send
        return false;
    }

    linkMtu = _TCPIPStackNetLinkMtu(pNetIf);
    if(pktPayload > linkMtu)
    {
#if (TCPIP_IPV4_FRAGMENTATION != 0)
        IPV4_HEADER* pHdr = (IPV4_HEADER*)pPkt->macPkt.pNetLayer;
        if(pHdr->FragmentInfo.DF != 0 || !TCPIP_IPV4_FragmentTxPkt(pPkt, linkMtu, pktPayload))
        {   // no fragments or failed to build the fragments; out of memory
            return false;
        }
#else
        // MAC transmit will fail anyway
        return false;
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

    }

    TCPIP_PKT_FlightLogTx(&pPkt->macPkt, TCPIP_THIS_MODULE_ID);

    if(destType == TCPIP_IPV4_DEST_SELF)
    {
        TCPIP_IPV4_FragmentTxInsertToRx(pNetIf, &pPkt->macPkt, TCPIP_MAC_PKT_FLAG_UNICAST, true);
        return true;
    }


    // TCPIP_IPV4_DEST_NETWORK 
    if(pMacDst == 0)
    {   // queue it
        if(ipv4ArpHandle == 0)
        {
            if((ipv4ArpHandle = TCPIP_ARP_HandlerRegister(0, TCPIP_IPV4_ArpHandler, 0)) == 0)
            {
                SYS_ERROR(SYS_ERROR_WARNING, "IPv4: Failed to register ARP notification! \r\n");
                return false;
            }
        }

        TCPIP_Helper_ProtectedSingleListTailAdd(&ipv4ArpQueue, (SGL_LIST_NODE*)pPkt);
        // ARP notification will be received: either TMO or resolved
        // mark packet as queued 
        pPkt->macPkt.pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        return true;
    }

  
    // MAC sets itself the TCPIP_MAC_PKT_FLAG_QUEUED
    txRes = TCPIP_IPV4_TxMacPkt(pNetIf, &pPkt->macPkt);
    if(txRes == false)
    {
        TCPIP_IPV4_FragmentTxAcknowledge(&pPkt->macPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR, IPV4_FRAG_TX_ACK_FRAGS);
    }

    return txRes;
}



// IPv4 formats a IPV4_PACKET and calculates the header checksum
// the source and destination addresses should be updated in the packet 
void TCPIP_IPV4_PacketFormatTx(IPV4_PACKET* pPkt, uint8_t protocol, uint16_t ipLoadLen, TCPIP_IPV4_PACKET_PARAMS* pParams)
{
    uint8_t *pEndOpt, *pEndHdr;
    IPV4_ROUTER_ALERT_OPTION* pOpt;
    IPV4_HEADER*    pHdr = (IPV4_HEADER*)pPkt->macPkt.pNetLayer;
    uint16_t hdrLen = sizeof(IPV4_HEADER) + pPkt->optionLen;
    _IPv4AssertCond(hdrLen <= IPV4_HEADER_MAXIMUM_SIZE, __func__, __LINE__);

    pHdr->Version = IPv4_VERSION;
    pHdr->IHL = hdrLen >> 2;
    // set the standard supported option, if any
    if((pPkt->optionMask & IPV4_OPTION_MASK_ROUTER_ALERT) != 0)
    {
        pOpt = (IPV4_ROUTER_ALERT_OPTION*)pHdr->options + pPkt->optionOffset;
        pOpt->optCopied = IPV4_ROUTER_ALERT_OPTION_COPIED;
        pOpt->optClass = IPV4_ROUTER_ALERT_OPTION_CLASS;
        pOpt->optNumber = TCPIP_IPV4_OPTION_ROUTER_ALERT;
        pOpt->optLength = IPV4_ROUTER_ALERT_OPTION_SIZE;
        pOpt->optValue = IPV4_ROUTER_ALERT_OPTION_DATA;

        // check if padding is needed
        pEndOpt = (uint8_t*)(pOpt + 1);
        pEndHdr = (uint8_t*)pHdr + hdrLen;
        _IPv4AssertCond(pEndOpt <= pEndHdr, __func__, __LINE__);
        if(pEndOpt < pEndHdr)
        {
            *pEndOpt = 0;
        }
    }

    pHdr->TypeOfService.val = 0;
    if(pParams)
    {
        if((pParams->tosFlags & TCPIP_IPV4_RELIABILITY_HIGH) != 0)
        {
            pHdr->TypeOfService.reliability = 1;
        }
        if((pParams->tosFlags & TCPIP_IPV4_THROUGHPUT_HIGH) != 0)
        {
            pHdr->TypeOfService.throughput = 1;
        }
        if((pParams->tosFlags & TCPIP_IPV4_DELAY_LOW) != 0)
        {
            pHdr->TypeOfService.delay = 1;
        }
        if((pParams->tosFlags & TCPIP_IPV4_PRECEDENCE_MASK) != 0)
        {
            pHdr->TypeOfService.precedence = pParams->tosFlags & TCPIP_IPV4_PRECEDENCE_MASK;
        }
    }

    pHdr->TotalLength = TCPIP_Helper_htons(hdrLen + ipLoadLen);
    pHdr->Identification = TCPIP_Helper_htons(++ipv4Identifier);
    pHdr->FragmentInfo.val = TCPIP_Helper_htons(0);
    if(pParams !=0 && pParams->df != 0)
    {
        pHdr->FragmentInfo.DF = 1;
    }
    pHdr->TimeToLive = (pParams != 0 && pParams->ttl != 0) ? pParams->ttl : IPV4_DEFAULT_TTL;
    pHdr->Protocol = protocol;
    pHdr->HeaderChecksum = 0;
    pHdr->SourceAddress.Val = pPkt->srcAddress.Val;
    pHdr->DestAddress.Val = pPkt->destAddress.Val;
    // update the checksum
    pHdr->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHdr, hdrLen, 0);

    pPkt->macPkt.pDSeg->segLen += hdrLen;
    pPkt->macPkt.pTransportLayer = pPkt->macPkt.pNetLayer + hdrLen;
    pPkt->macPkt.pktIf = pPkt->netIfH;
}
	
// ARP resolution done
static void TCPIP_IPV4_ArpHandler(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType, const void* param)
{
    TCPIP_NET_IF* pPktIf;
    SINGLE_LIST newList;
    IPV4_PACKET *pPkt, *pNext;
    TCPIP_MAC_PKT_ACK_RES   pktAckFail;
    TCPIP_MAC_ETHERNET_HEADER* macHdr;
    

    TCPIP_Helper_SingleListInitialize (&newList);
    
    TCPIP_Helper_ProtectedSingleListLock(&ipv4ArpQueue);
    // traverse the ipv4ArpQueue list
    // and find all the packets waiting for the solved address

    pNext = (IPV4_PACKET*)ipv4ArpQueue.list.head;

    while((pPkt = pNext) != 0)
    {
        pNext = (IPV4_PACKET*)pPkt->macPkt.next;
        pPkt->macPkt.next = 0;  // send single packet
            
        pktAckFail = TCPIP_MAC_PKT_ACK_NONE; 
        if(pPkt->arpTarget.Val == ipAdd->Val)
        {   // match
            if(evType >= 0)
            {   // successfully resolved the ARP; update the packet destination
                macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pPkt->macPkt.pMacLayer;
                memcpy(&macHdr->DestMACAddr, MACAddr, sizeof(*MACAddr));
                pPktIf = _TCPIPStackHandleToNet(pPkt->netIfH);
                if(pPktIf == 0 || !TCPIP_IPV4_TxMacPkt(pPktIf, &pPkt->macPkt))
                {
                    pktAckFail = TCPIP_MAC_PKT_ACK_ARP_NET_ERR; 
                }
            }
            else
            {   // some error;
                pktAckFail = TCPIP_MAC_PKT_ACK_ARP_TMO; 
            }

            if(pktAckFail != TCPIP_MAC_PKT_ACK_NONE)
            {   // some error; discard the packet
                TCPIP_IPV4_FragmentTxAcknowledge(&pPkt->macPkt, pktAckFail, IPV4_FRAG_TX_ACK_HEAD | IPV4_FRAG_TX_ACK_FRAGS);
            }
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newList, (SGL_LIST_NODE*)pPkt);
        }
    }

    // update the arp queue
    ipv4ArpQueue.list = newList;
    TCPIP_Helper_ProtectedSingleListUnlock(&ipv4ArpQueue);

}

void  TCPIP_IPV4_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { // RX signal occurred
        TCPIP_IPV4_Process();
    }

#if (TCPIP_IPV4_FRAGMENTATION != 0)
    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_IPV4_Timeout();
    }
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

}


// processes an incoming IPV4 packet
static void TCPIP_IPV4_Process(void)
{
    TCPIP_NET_IF* pNetIf;
    TCPIP_MAC_PACKET* pRxPkt;
    uint8_t      headerLen;
    uint16_t     headerChecksum;
    IPV4_HEADER* pHeader;
    uint8_t      ipFrameType;
    bool         pktAccepted;
    TCPIP_MAC_PKT_ACK_RES ackRes;
    TCPIP_STACK_MODULE destId;
    bool        isFragment = 0;

    // extract queued IPv4 packets
    while((pRxPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
        // log the packet
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        while(true)
        {
            destId = TCPIP_MODULE_NONE;
            ackRes = TCPIP_MAC_PKT_ACK_NONE;
            pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;

            // Make sure that this is an IPv4 packet.
            if((pHeader->Version) != IPv4_VERSION)
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // make sure the header length is within packet limits
            headerLen = pHeader->IHL << 2;
            if(headerLen < sizeof(IPV4_HEADER) || headerLen > pRxPkt->pDSeg->segLen)
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // detect the proper alias interface
            pNetIf = _TCPIPStackMapAliasInterface((TCPIP_NET_IF*)pRxPkt->pktIf, &pHeader->DestAddress);
            pRxPkt->pktIf = pNetIf;

            // discard wrong source address
            if(TCPIP_STACK_IsBcastAddress(pNetIf, &pHeader->SourceAddress))
            {   // net or limited bcast
                ackRes = TCPIP_MAC_PKT_ACK_SOURCE_ERR;
                break;
            }

            // Validate the IP header.  If it is correct, the checksum 
            // will come out to 0x0000 (because the header contains a 
            // precomputed checksum).  A corrupt header will have a 
            // nonzero checksum.
            headerChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHeader, headerLen, 0);

            if(headerChecksum)
            {
                // Bad packet. The function caller will be notified by means of the false 
                // return value and it should discard the packet.
                ackRes = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
                break;
            }

            // Network to host conversion.
            pHeader->TotalLength = TCPIP_Helper_ntohs(pHeader->TotalLength);
            pHeader->FragmentInfo.val = TCPIP_Helper_ntohs(pHeader->FragmentInfo.val);

            isFragment = pHeader->FragmentInfo.MF != 0 || pHeader->FragmentInfo.fragOffset != 0;
#if (TCPIP_IPV4_FRAGMENTATION == 0)
            // Throw this packet away if it is a fragment.  
            // We don't support IPv4 fragment reconstruction.

            if(isFragment)
            {   // discard the fragment
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }
#endif  // (TCPIP_IPV4_FRAGMENTATION == 0)

            pRxPkt->pTransportLayer = pRxPkt->pNetLayer + headerLen;
            pRxPkt->pDSeg->segLen -= headerLen;

            // Check the packet arrived on the proper interface and passes the filters
            pktAccepted = TCPIP_IPV4_VerifyPkt(pNetIf, pRxPkt);
            if(!pktAccepted)
            {
                pktAccepted = TCPIP_IPV4_VerifyPktFilters(pRxPkt);
            }

            TCPIP_IPV4_CheckRxPkt(pRxPkt);

            if(!pktAccepted)
            {   // discard
                ackRes = TCPIP_MAC_PKT_ACK_SOURCE_ERR;
                break;
            }

            // valid IPv4 packet
            // check where it needs to go
            pRxPkt->totTransportLen = pHeader->TotalLength - headerLen;
            ipFrameType = pHeader->Protocol;

            switch(ipFrameType)
            {
#if defined(TCPIP_STACK_USE_ICMP_SERVER) || defined(TCPIP_STACK_USE_ICMP_CLIENT)
                case IP_PROT_ICMP:
                    destId = TCPIP_MODULE_ICMP;
                    break;
#endif

#if defined(TCPIP_STACK_USE_IGMP)
                case IP_PROT_IGMP:
                    destId = TCPIP_MODULE_IGMP;
                    break;
#endif
                    
#if defined(TCPIP_STACK_USE_TCP)
                case IP_PROT_TCP:
                    destId = TCPIP_MODULE_TCP;
                    break;
#endif

#if defined(TCPIP_STACK_USE_UDP)
                case IP_PROT_UDP:
                    destId = TCPIP_MODULE_UDP;
                    break;
#endif

                default:
                    // unknown/unsupported; discard
                    ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR; 
                    break;
            }

            break;
        }

#if (TCPIP_IPV4_FRAGMENTATION != 0)
        pRxPkt->pkt_next = 0;       // make sure it's not linked
        if(ackRes == TCPIP_MAC_PKT_ACK_NONE)
        {   // valid so far
            if(isFragment)
            {
                IPV4_FRAGMENT_NODE *fragNode;
                ackRes = TCPIP_IPV4_RxFragmentInsert(pRxPkt, &fragNode);

                if(ackRes == TCPIP_MAC_PKT_ACK_NONE && fragNode != 0)
                {
                    pRxPkt = fragNode->fragHead; // this list is already ordered by pkt_next!
                    isFragment = 0; // let it through
                }
            }
        }
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {   // unknown/unsupported; discard
            TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes); 
        }
        else if(!isFragment) 
        {   // forward this packet and signal
            _TCPIPStackModuleRxInsert(destId, pRxPkt, true);
        }
    }

}

/*
 Discard packets that are received on the wrong interface
 (have an IP address that doesn't match the interface we
 received the packet on) or are rejected by the current IPv4 packet filters.
 Returns: true if valid packet
          false otherwise.
*/                  
static bool TCPIP_IPV4_VerifyPkt(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt)
{
    // process first the registered filters
    IPV4_HEADER* pHeader;
    IPV4_ADDR* pktDestIP;
    TCPIP_IPV4_FILTER_TYPE currFilter; // current filter

    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {   // standard IPv4 filtering
        pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;
        pktDestIP = &pHeader->DestAddress;
        currFilter = ipv4FilterType;

        if(TCPIP_STACK_AddressIsOfNet(pNetIf, pktDestIP))
        {   // unicast to me
            return (currFilter & TCPIP_IPV4_FILTER_UNICAST) == 0 ? true : false;
        }

        if(TCPIP_STACK_IsBcastAddress(pNetIf, pktDestIP))
        {   // net or limited bcast
            return (currFilter & TCPIP_IPV4_FILTER_BROADCAST) == 0 ? true : false;
        }

        if(TCPIP_Helper_IsMcastAddress(pktDestIP))
        {   // multicast
            return (currFilter & TCPIP_IPV4_FILTER_MULTICAST) == 0 ? true : false;
        }

        // else: unknown packet type
    }

    return false;

}



static bool TCPIP_IPV4_VerifyPktFilters(TCPIP_MAC_PACKET* pRxPkt)
{
    // process the registered filters
    IPV4_FILTER_LIST_NODE* fNode;
    bool pktOk = false;

    TCPIP_Notification_Lock(&ipv4PacketFilters);
    for(fNode = (IPV4_FILTER_LIST_NODE*)ipv4PacketFilters.list.head; fNode != 0; fNode = fNode->next)
    {
        if((*fNode->handler)(pRxPkt, fNode->hParam))
        {   // packet accepted
            pktOk = true;
            break;
        }
    }
    TCPIP_Notification_Unlock(&ipv4PacketFilters);
        
    return pktOk;
}

IPV4_FILTER_HANDLE IPv4RegisterFilter(IPV4_FILTER_FUNC handler, const void* hParam)
{
    if(ipv4MemH && handler)
    {
        IPV4_FILTER_LIST_NODE* newNode = (IPV4_FILTER_LIST_NODE*)TCPIP_Notification_Add(&ipv4PacketFilters, ipv4MemH, sizeof(*newNode));
        if(newNode)
        {
            newNode->handler = handler;
            newNode->hParam = hParam;
            return newNode;
        }
    }

    return 0;

}

// deregister the filter handler
// returns true or false if no such handler registered
bool Ipv4DeRegisterFilter(IPV4_FILTER_HANDLE hFilter)
{
    if(hFilter && ipv4MemH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hFilter, &ipv4PacketFilters, ipv4MemH))
        {
            return true;
        }
    }

    return false;
}

// select destination MAC address
// ppMacAdd is set to 0 if the MAC address is not available yet
static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_PktMacDestination(IPV4_PACKET* pPkt, const IPV4_ADDR* pIpAdd, TCPIP_MAC_ADDR** ppMacAdd)
{
    TCPIP_ARP_RESULT  arpRes;
    TCPIP_NET_IF*     pHostIf;
    TCPIP_MAC_ADDR*   pMacDst = *ppMacAdd;
    TCPIP_NET_IF* pNetIf = (TCPIP_NET_IF*)pPkt->netIfH;

    if(TCPIP_STACK_AddressIsOfNet(pNetIf, pIpAdd))
    {   // localhost address
        memcpy(pMacDst, TCPIP_STACK_NetMACAddressGet(pNetIf), sizeof(*pMacDst));
        return TCPIP_IPV4_DEST_SELF; 
    }
    else if((pHostIf = TCPIP_STACK_NetByAddress(pIpAdd)))
    {   // localhost address
        memcpy(pMacDst, TCPIP_STACK_NetMACAddressGet(pHostIf), sizeof(*pMacDst));
        pPkt->netIfH = pHostIf;
        return TCPIP_IPV4_DEST_SELF; 
    }

    if(pIpAdd->Val == 0xffffffff || pIpAdd->Val == TCPIP_STACK_NetAddressBcast(pNetIf))
    {
        memset(pMacDst, 0xff, sizeof(*pMacDst));
    }
    // check IP multicast address range from 224.0.0.0 to 239.255.255.255
    // can be done locally; No need for an ARP request.
    else if ((pIpAdd->v[0] >= 224) && (pIpAdd->v[0] <= 239))
    {
        pMacDst->v[0] = 0x01;
        pMacDst->v[1] = 0x00;
        pMacDst->v[2] = 0x5E;
        pMacDst->v[3] = 0x7f & pIpAdd->v[1];
        pMacDst->v[4] = pIpAdd->v[2];
        pMacDst->v[5] = pIpAdd->v[3];
    }
    else
    {   
        if(_TCPIPStackIpAddFromLAN(pNetIf, pIpAdd))
        {
            pPkt->arpTarget.Val  = pIpAdd->Val;
        }
        else
        {   // not this LAN
            pPkt->arpTarget.Val  = pNetIf->netGateway.Val;
        }

        arpRes = TCPIP_ARP_EntryGet(pNetIf, &pPkt->arpTarget, pMacDst, true);
        if(arpRes == ARP_RES_ENTRY_SOLVED)
        {   // good to transmit
        }
        else if(arpRes == ARP_RES_ENTRY_QUEUED || arpRes == ARP_RES_ENTRY_NEW)
        {   // will have to be queued
            *ppMacAdd = 0;    // not known yet
        }
        else
        {   // discard, cannot send
            return TCPIP_IPV4_DEST_FAIL;
        }
    }

    return TCPIP_IPV4_DEST_NETWORK;
}

TCPIP_IPV4_FILTER_TYPE TCPIP_IPV4_PacketFilterSet(TCPIP_IPV4_FILTER_TYPE filtType)
{
    TCPIP_IPV4_FILTER_TYPE currFilt;

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    currFilt = (ipv4FilterType |= filtType);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return currFilt;
}

TCPIP_IPV4_FILTER_TYPE TCPIP_IPV4_PacketFilterClear(TCPIP_IPV4_FILTER_TYPE filtType)
{
    TCPIP_IPV4_FILTER_TYPE currFilt;

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    currFilt = (ipv4FilterType &= ~filtType);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return currFilt;
}

// helper to transform a RX packet to a TX packet
// IPv4 header:
//          - the destination addresses is set as the packet source address
//          - the source address is the IP address of the coresponding packet interface (which should be set!) 
//          - total length and fragment info are converted to network order
//          - data segment is re-adjusted with the IPv4 header length
// MAC header:
//          - the destination addresses is set as the MAC packet source address
//          - the source address is the MAC address of the coresponding packet interface (which should be set!) 
//          - data segment is re-adjusted with the MAC header length
//
// setChecksum:
//          - if true, the IPv4 header checksum is updated for the IPv4 header
// TCPIP_MAC_PKT_FLAG_TX flag is set
void TCPIP_IPV4_MacPacketSwitchTxToRx(TCPIP_MAC_PACKET* pRxPkt, bool setChecksum)
{
    IPV4_HEADER* pIpv4Hdr;

    pIpv4Hdr = (IPV4_HEADER*)pRxPkt->pNetLayer;
    pIpv4Hdr->DestAddress.Val = pIpv4Hdr->SourceAddress.Val;
    pIpv4Hdr->SourceAddress.Val = _TCPIPStackNetAddress((TCPIP_NET_IF*)pRxPkt->pktIf);

    pIpv4Hdr->TotalLength = TCPIP_Helper_htons(pIpv4Hdr->TotalLength);
    pIpv4Hdr->FragmentInfo.val = TCPIP_Helper_htons(pIpv4Hdr->FragmentInfo.val);
    if(pIpv4Hdr->TimeToLive == 0)
    {
        pIpv4Hdr->TimeToLive = IPV4_DEFAULT_TTL;
    }
    uint8_t headerLen = pIpv4Hdr->IHL << 2;  
    pRxPkt->pDSeg->segLen += headerLen;

    if(setChecksum)
    {
        pIpv4Hdr->HeaderChecksum = 0;
        pIpv4Hdr->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pIpv4Hdr, headerLen, 0);
    }

    // set macHdr source and destination
    TCPIP_MAC_ETHERNET_HEADER* macHdr;
    macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pRxPkt->pMacLayer;

    memcpy(&macHdr->DestMACAddr, &macHdr->SourceMACAddr, sizeof(TCPIP_MAC_ADDR));
    memcpy(&macHdr->SourceMACAddr, _TCPIPStack_NetMACAddressGet((TCPIP_NET_IF*)pRxPkt->pktIf), sizeof(TCPIP_MAC_ADDR));

    pRxPkt->pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);
    pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_TX; 
}

bool  TCPIP_IPV4_MacPacketTransmit(TCPIP_MAC_PACKET* pPkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR* pDestAddress)
{
    TCPIP_NET_IF* pHostIf;
    TCPIP_IPV4_DEST_TYPE destType;
    TCPIP_NET_IF* pNetIf = TCPIP_IPV4_CheckPktTx(hNet, pPkt);

    if(pNetIf == 0)
    {   // cannot transmit over invalid interface
        return false;
    }

    if(TCPIP_STACK_AddressIsOfNet(pNetIf, pDestAddress))
    {   // localhost address
        destType = TCPIP_IPV4_DEST_SELF; 
    }
    else if((pHostIf = TCPIP_STACK_NetByAddress(pDestAddress)) != 0)
    {   // localhost address
        pNetIf = pHostIf;
        destType = TCPIP_IPV4_DEST_SELF; 
    }
    else
    {
        destType = TCPIP_IPV4_DEST_NETWORK;
    }

    pPkt->pktIf = pNetIf;
    TCPIP_PKT_FlightLogTx(pPkt, TCPIP_THIS_MODULE_ID);
    if(destType == TCPIP_IPV4_DEST_SELF)
    {
        TCPIP_IPV4_FragmentTxInsertToRx(pNetIf, pPkt, TCPIP_MAC_PKT_FLAG_UNICAST, true);
        return true;
    }

    // MAC sets itself the TCPIP_MAC_PKT_FLAG_QUEUED
    // this is an externally passed packet, don't acknowledge anything
    return TCPIP_IPV4_TxMacPkt(pNetIf, pPkt);
}

IPV4_PACKET* TCPIP_IPV4_PacketAlloc(uint16_t pktPayload, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions,  TCPIP_MAC_PACKET_FLAGS flags)
{
    IPV4_PACKET * pPacket;
    TCPIP_IPV4_OPTION_TYPE optType;
    int ix;
    int optLen;
    IPV4_OPTION_TYPE_MASK totOptMask = 0;
    int totOptionsLen = 0;

    if(nOptions != 0 && pOptions != 0)
    {
        for(ix = 0; ix < nOptions; ix++, pOptions++)
        {
            optType = pOptions->optionType;
            optLen = pOptions->optionSize;
            // check for well known options
            if(optType == TCPIP_IPV4_OPTION_ROUTER_ALERT)
            {   // override
                optLen = IPV4_ROUTER_ALERT_OPTION_SIZE;
                totOptMask |= IPV4_OPTION_MASK_ROUTER_ALERT;
            }
            else if(optType == TCPIP_IPV4_OPTION_NONE)
            {   // standard case
                optLen = 0;
            }

            totOptionsLen += optLen;
        }
    }

    // make sure is 4 bytes multiple
    if(totOptionsLen)
    {
        totOptionsLen = ((totOptionsLen + 3) >> 2) << 2; 

        if(totOptionsLen > IPV4_OPTIONS_MAXIMUM_SIZE)
        {
            totOptionsLen = IPV4_OPTIONS_MAXIMUM_SIZE;
        }
    }

    uint16_t transpHdrLen = sizeof(IPV4_HEADER) + totOptionsLen; 


    pPacket = (IPV4_PACKET*)TCPIP_PKT_PacketAlloc(sizeof(IPV4_PACKET), transpHdrLen + pktPayload, flags | TCPIP_MAC_PKT_FLAG_IPV4 | TCPIP_MAC_PKT_FLAG_TX);
    if(pPacket)
    {
        pPacket->optionLen = totOptionsLen;
        pPacket->optionMask = (uint16_t)totOptMask;
    }

    return pPacket;
}


IPV4_OPTION_FIELD* TCPIP_IPV4_PacketOptionFieldSet(IPV4_PACKET* pPkt, uint16_t optionLen, bool firstOption)
{
    int optSpace;
    IPV4_OPTION_FIELD* pOpt = 0;
    IPV4_HEADER*    pHdr = (IPV4_HEADER*)pPkt->macPkt.pNetLayer;
    
    if(firstOption)
    {
        pPkt->optionOffset = 0;
    }
    
    optSpace = (int)pPkt->optionLen - (int)pPkt->optionOffset - (int)optionLen;
    if(optSpace >= 0)
    {   // OK
        pOpt = (IPV4_OPTION_FIELD*)pHdr->options + pPkt->optionOffset;
        pPkt->optionOffset += optionLen;
    }


    return pOpt;
}

int TCPIP_IPV4_PacketOptionListGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions )
{
    int          optLen, totOptLen;
    int          pktOptions;
    IPV4_OPTION_FIELD* pOpt;
    
    if(nOptions != 0 && pOptions == 0)
    {
        nOptions = 0;
    }

    pktOptions = 0;

    pOpt = _IPv4CheckPacketOption(pRxPkt, &totOptLen);
    if(pOpt)
    {
        while(totOptLen != 0)
        {
            optLen = pOpt->optLength;
            if(nOptions)
            {
                pOptions->optionType = pOpt->optNumber;
                pOptions->optionSize = optLen;
                pOptions++;
                nOptions--;
            }
            totOptLen -= optLen;
            pktOptions++;
            pOpt = (IPV4_OPTION_FIELD*)((uint8_t*)pOpt + optLen);
        }
    }

    return pktOptions;
}

int TCPIP_IPV4_PacketOptionGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_TYPE optType, IPV4_OPTION_FIELD* pOptField, int optSize)
{
    IPV4_OPTION_FIELD* pOpt;
    int          optLen, totOptLen;

    if(optSize != 0 && pOptField == 0)
    {
        optSize = 0;
    }

    pOpt = _IPv4CheckPacketOption(pRxPkt, &totOptLen);
    if(pOpt)
    {   // traverse all options
        while(totOptLen != 0)
        {
            optLen = pOpt->optLength;
            if(pOpt->optNumber == optType)
            {   // found it
                if(optLen <= optSize)
                {
                    memcpy(pOpt, pOptField, optLen);
                }
                return optLen;
            }
            totOptLen -= optLen;
            pOpt = (IPV4_OPTION_FIELD*)((uint8_t*)pOpt + optLen);
        }
    }


    return 0;
}

// checks an IPv4 packet to have valid options and returns a pointer to the beginning of the option field
// it also updates the options length
static IPV4_OPTION_FIELD* _IPv4CheckPacketOption(TCPIP_MAC_PACKET* pRxPkt, int* pOptLen)
{
    IPV4_HEADER* pHeader;
    int          totOptLen;

    // minimal packet check
    pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;

    // Make sure that this is an IPv4 packet.
    if((pHeader->Version) == IPv4_VERSION)
    {
        // make sure the header length is within packet limits
        totOptLen = (int)(pHeader->IHL << 2) - sizeof(IPV4_HEADER);
        if(totOptLen > 0)
        {   // neither invalid nor no options
            *pOptLen = totOptLen;
            return (IPV4_OPTION_FIELD*)pHeader->options;
        }
    }

    return 0;

}

int TCPIP_IPV4_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pIf = _TCPIPStackHandleToNetUp(netH);

    return pIf ? _TCPIPStackNetLinkMtu(pIf) - sizeof(IPV4_HEADER) : 0;
}

bool TCPIP_IPV4_IsFragmentationEnabled(void)
{
#if (TCPIP_IPV4_FRAGMENTATION != 0)
    return true;
#else
    return false;
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)
}

// fragmentation functionality
//
#if (TCPIP_IPV4_FRAGMENTATION != 0)

// fragments reassembly

// IPv4 task timeout
// checks fragments timeouts
static void TCPIP_IPV4_Timeout(void)
{
    uint32_t tickFreq, currTick;
    IPV4_FRAGMENT_NODE *pF;
    SINGLE_LIST     newList;
    
    TCPIP_Helper_SingleListInitialize(&newList);
    tickFreq = SYS_TMR_TickCounterFrequencyGet();
    currTick = SYS_TMR_TickCountGet();

    while((pF = (IPV4_FRAGMENT_NODE*)TCPIP_Helper_SingleListHeadRemove(&ipv4FragmentQueue)) != 0)
    {
        if(currTick - pF->fragTStart > pF->fragTmo * tickFreq)
        {   // expired node; remove
            _IPv4FragmentDbg(pF, 0, TCPIP_IPV4_FRAG_DISCARD_TMO);
            TCPIP_IPV4_RxFragmentDiscard(pF, TCPIP_MAC_PKT_ACK_FRAGMENT_ERR);
        }
        else
        {
            TCPIP_Helper_SingleListHeadAdd(&newList, (SGL_LIST_NODE*)pF);
        }
    }

    // replace lists
    ipv4FragmentQueue = newList;
}


// inserts a new fragment to the ipv4FragmentQueue 
// returns TCPIP_MAC_PKT_ACK_NONE if successful insertion/processing
//      ppFrag points to 0 if nothing else is required (intermediary fragment)
//      ppFrag points to a valid complete fragment that needs to be reassembled and passed to the user
//
// a TCPIP_MAC_PKT_ACK_RES error code otherwise
//
static TCPIP_MAC_PKT_ACK_RES TCPIP_IPV4_RxFragmentInsert(TCPIP_MAC_PACKET* pRxPkt, IPV4_FRAGMENT_NODE **ppFrag)
{
    IPV4_FRAGMENT_NODE *pF, *pParent, *pPrevParent;
    IPV4_HEADER *pFHdr, *pRxHdr;

    pRxHdr = (IPV4_HEADER*)pRxPkt->pNetLayer;

    *ppFrag = 0;
    pParent = pPrevParent = 0;
    for(pF = (IPV4_FRAGMENT_NODE*)ipv4FragmentQueue.head; pF != 0; pF = pF->next)
    {
        pFHdr = (IPV4_HEADER*)pF->fragHead->pNetLayer;
        if(pFHdr->Identification == pRxHdr->Identification && pFHdr->SourceAddress.Val == pRxHdr->SourceAddress.Val &&
                pFHdr->DestAddress.Val == pRxHdr->DestAddress.Val && pFHdr->Protocol == pRxHdr->Protocol)
        {   // found parent fragment
            pParent = pF;
            break;
        }   
        pPrevParent = pF;
    }

    if(pParent == 0)
    {   // brand new fragment packet
        if(TCPIP_Helper_SingleListCount(&ipv4FragmentQueue) >= TCPIP_IPV4_FRAGMENT_MAX_STREAMS)
        {   // don't start another fragmented stream
            return TCPIP_MAC_PKT_ACK_FRAGMENT_ERR;
        }

        IPV4_FRAGMENT_NODE*  newNode = (IPV4_FRAGMENT_NODE*)TCPIP_HEAP_Calloc(ipv4MemH, 1, sizeof(*newNode));

        if(newNode == 0)
        {   // failed memory allocation
            return TCPIP_MAC_PKT_ACK_FRAGMENT_ERR;
        }

        newNode->fragHead = pRxPkt;
        newNode->fragTStart = SYS_TMR_TickCountGet();  
        newNode->fragTmo =  TCPIP_IPV4_FRAGMENT_TIMEOUT;
        newNode->nFrags =  1;

        _IPv4FragmentDbg(newNode, pRxPkt, TCPIP_IPV4_FRAG_CREATED);
        TCPIP_Helper_SingleListTailAdd(&ipv4FragmentQueue, (SGL_LIST_NODE*)newNode);  
        return TCPIP_MAC_PKT_ACK_NONE;
    }

    // this is just a new fragment;
    if(pParent->nFrags >= TCPIP_IPV4_FRAGMENT_MAX_NUMBER)
    {   // more fragments than allowed
        TCPIP_Helper_SingleListNextRemove(&ipv4FragmentQueue, (SGL_LIST_NODE*)pPrevParent);
        _IPv4FragmentDbg(pParent, pRxPkt, TCPIP_IPV4_FRAG_DISCARD_EXCEEDED);
        TCPIP_IPV4_RxFragmentDiscard(pParent, TCPIP_MAC_PKT_ACK_FRAGMENT_ERR);
        return TCPIP_MAC_PKT_ACK_FRAGMENT_ERR;
    }

    // adjust the time
    if(pRxHdr->TimeToLive > pParent->fragTmo)
    {
        pParent->fragTmo = pRxHdr->TimeToLive;
    }

    // a new fragment needs to be inserted in the proper place
    // old overlapping fragments need to be discarded/adjusted
    TCPIP_MAC_PACKET* pPrevPkt, *pCurrPkt, *pNextPkt;
    IPV4_HEADER *pCurrHdr;
    uint16_t rxMin, rxMax, nextMin;   
    uint16_t currMin, currMax;   
    bool fragOverlap = false;

    rxMin = pRxHdr->FragmentInfo.fragOffset * 8;
    rxMax = rxMin + pRxPkt->totTransportLen;

    // insert in proper place
    pPrevPkt = 0;
    for(pCurrPkt = pParent->fragHead; pCurrPkt != 0; pCurrPkt = pNextPkt)
    {
        pNextPkt = pCurrPkt->pkt_next;
        pCurrHdr = (IPV4_HEADER*)pCurrPkt->pNetLayer;
        currMin = pCurrHdr->FragmentInfo.fragOffset * 8;
        currMax = currMin + pCurrPkt->totTransportLen;
        fragOverlap = false;
        if(rxMax <= currMin)
        {   // insert after pPrevPkt
            break;
        }

        // check for overlap
        if(rxMax > currMin && rxMin < currMax)
        {   // overlap; adjust current packet
            // 3 segments are created: a begin, an end and a discard segment, with sizes lb, le, ld
            int lb, le, ld;

            fragOverlap = true;

            lb = rxMin - currMin;
            if(lb < 0)
            {
                lb = 0;
            }
            le = currMax - rxMax;
            if(le < 0)
            {
                le = 0;
            }
            ld = pCurrPkt->totTransportLen - (lb + le);

            if(le == 0 && lb == 0)
            {   // total overlap; rx > current; simply discard current
                if(pPrevPkt)
                {   
                    pPrevPkt->pkt_next = pNextPkt;
                }
                else
                {   // head is removed
                    pParent->fragHead = pNextPkt;
                }
                _IPv4FragmentDbg(pParent, pCurrPkt, TCPIP_IPV4_FRAG_DISCARD_OVERLAP);
                pParent->nFrags--;
                TCPIP_PKT_PacketAcknowledge(pCurrPkt, TCPIP_MAC_PKT_ACK_FRAGMENT_ERR); 
                continue;
            }
            else if(lb == 0)
            {   // partial overlap; discard at the beginning of current 
                pCurrHdr->FragmentInfo.fragOffset += ld / 8;
                pCurrPkt->totTransportLen -= ld;
                pCurrPkt->pTransportLayer += ld;
            }
            else if(le == 0)
            {   // partial overlap; discard at the end of current
                pCurrPkt->totTransportLen -= ld;
            }
            else
            {   // lb!= 0 && le != 0; total overlap; rx < current; keep begin + discard + keep end
                pCurrPkt->totTransportLen -= ld + le;
                // copy the end part to rx; consider packets spanning multiple segments!
                TCPIP_MAC_DATA_SEGMENT* pDestSeg = TCPIP_PKT_DataSegmentGet(pRxPkt, pRxPkt->pTransportLayer +  pRxPkt->totTransportLen, true);
                _IPv4AssertCond(pDestSeg != 0, __func__, __LINE__);
                // check that there's enough room; it should, since the current packet is larger!
                if(le > pDestSeg->segSize - pDestSeg->segLen)
                {   // copy less; probably the packet will tmo!
                    le = pDestSeg->segSize - pDestSeg->segLen;
                    _IPv4AssertCond(false, __func__, __LINE__); // shouldn't happen though since all packets could hold this size...
                }

                // ok, we can copy
                uint8_t* startAdd = pCurrPkt->pTransportLayer + (lb + ld);
                int copyBytes = TCPIP_Helper_PacketCopy(pCurrPkt, pDestSeg->segLoad + pDestSeg->segLen, &startAdd, le, true);
                _IPv4AssertCond(copyBytes == le, __func__, __LINE__);
                pDestSeg->segLen += le;
                pRxPkt->totTransportLen += le;
            }
            pNextPkt = pCurrPkt;    // reeveluate
            continue;
        }
        
        // advance;
        pPrevPkt = pCurrPkt;
    }

    // found a spot for the rx packet;insert after pPrevPkt
    if(pPrevPkt != 0)
    {
        pRxPkt->pkt_next = pPrevPkt->pkt_next;
        pPrevPkt->pkt_next = pRxPkt;
    }
    else
    {    // insert head
        pRxPkt->pkt_next = pParent->fragHead;
        pParent->fragHead = pRxPkt;
    }
    // packet has been inserted
    fragOverlap = fragOverlap;  // quiet unused variable warning
    _IPv4FragmentDbg(pParent, pRxPkt, fragOverlap ? TCPIP_IPV4_FRAG_INSERT_OVERLAP:  TCPIP_IPV4_FRAG_INSERTED);
    pRxPkt->next = 0; 
    pParent->nFrags++;

    // check for packet completion
    bool pktDone = false;
    pCurrPkt = pParent->fragHead;
    pCurrHdr = (IPV4_HEADER*)pCurrPkt->pNetLayer;
    if(pCurrHdr->FragmentInfo.MF != 0 && pCurrHdr->FragmentInfo.fragOffset == 0)
    {   // at least 2 frags and 1st fragment is in place
        for( ; pCurrPkt != 0; pCurrPkt = pNextPkt)
        {
            pNextPkt = pCurrPkt->pkt_next;
            pCurrHdr = (IPV4_HEADER*)pCurrPkt->pNetLayer;
            // check if gap

            if(pNextPkt)
            {
                currMax = pCurrHdr->FragmentInfo.fragOffset * 8 + pCurrPkt->totTransportLen;
                nextMin = ((IPV4_HEADER*)pNextPkt->pNetLayer)->FragmentInfo.fragOffset * 8;
                if(currMax != nextMin)
                {   // gap
                    break;
                }
            }

            // no gap
            if(pCurrHdr->FragmentInfo.MF == 0)
            {   // last packet is here; we're done; 
                _IPv4AssertCond(pNextPkt == 0, __func__, __LINE__);
                pktDone = true;
                break;
            }
        }
    }
    // else no way, missing the 1st fragment

    if(pktDone)
    {   // completed; remove the packet from the list
        *ppFrag = pParent;
        TCPIP_Helper_SingleListNextRemove(&ipv4FragmentQueue, (SGL_LIST_NODE*)pPrevParent);
        TCPIP_IPV4_RxFragmentDiscard(pParent, TCPIP_MAC_PKT_ACK_NONE);    // segments are still valid but the node itself is deleted
        _IPv4FragmentDbg(pParent, 0, TCPIP_IPV4_FRAG_COMPLETE);
    }

    return TCPIP_MAC_PKT_ACK_NONE;
}

// if ackRes != TCPIP_MAC_PKT_ACK_NONE, it acknowledges all the packets in the fragment node
// then deallocates the node itself
// node should have been removed from the ipv4FragmentQueue!
static void TCPIP_IPV4_RxFragmentDiscard(IPV4_FRAGMENT_NODE* pFrag, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET *pPkt, *pPktNext;

    if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
    {   // acknowledge the segments too
        for(pPkt = pFrag->fragHead; pPkt != 0; pPkt = pPktNext)
        {
            pPktNext = pPkt->pkt_next;
            TCPIP_PKT_PacketAcknowledge(pPkt, ackRes); 
        }
    }

    TCPIP_HEAP_Free(ipv4MemH, pFrag);
}

// purges the ipv4FragmentQueue 
// if pRxPkt == 0, then it purges everything
// otherwise it looks for a specific packet 
static void TCPIP_IPV4_RxFragmentListPurge(SINGLE_LIST* pL)
{
    IPV4_FRAGMENT_NODE* pF;

    while((pF = (IPV4_FRAGMENT_NODE*)TCPIP_Helper_SingleListHeadRemove(pL)) != 0)
    {
        TCPIP_IPV4_RxFragmentDiscard(pF, TCPIP_MAC_PKT_ACK_PROTO_DEST_CLOSE);
    }
}


// fragment transmit functionality


// TX packet needs fragmentation; each fragment will contain the IPv4 header and a fragment of data
static bool TCPIP_IPV4_FragmentTxPkt(IPV4_PACKET* pPkt, uint16_t linkMtu, uint16_t pktPayload)
{
    int ix;
    IPV4_FRAG_TX_PKT    *pFragTx;
    TCPIP_MAC_PACKET    *pMacPkt, *pMacNext, *tail;
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    IPV4_HEADER*        pHdr;
    uint8_t*            pIpv4Load;
    IPV4_FRAGMENT_INFO  fragInfo;

    uint16_t pktHeaderSize = pPkt->macPkt.pTransportLayer - pPkt->macPkt.pMacLayer; 

    uint16_t ipv4HeaderSize = pPkt->macPkt.pTransportLayer - pPkt->macPkt.pNetLayer;
    uint16_t ipv4Payload = pktPayload - ipv4HeaderSize;

    uint16_t fragSize = (linkMtu - ipv4HeaderSize) & 0xfff8;  // fragments need to be multiple of 8
    int nFrags = (ipv4Payload + fragSize - 1) / fragSize;   // round up
    uint16_t lastFragSize = ipv4Payload - (nFrags - 1) * fragSize;

    if(nFrags > TCPIP_IPV4_FRAGMENT_MAX_NUMBER)
    {
        return false;
    }

    // allocate nFrags -1 packets; we already have the 1st
    pPkt->macPkt.pkt_next = 0;
    tail = &pPkt->macPkt;
    for(ix = 1; ix < nFrags; ix++)
    {
        pFragTx = (IPV4_FRAG_TX_PKT*)TCPIP_PKT_PacketAlloc(sizeof(IPV4_FRAG_TX_PKT), ipv4HeaderSize, TCPIP_MAC_PKT_FLAG_IPV4 | TCPIP_MAC_PKT_FLAG_SPLIT | TCPIP_MAC_PKT_FLAG_TX);
        if(pFragTx != 0)
        {   
            TCPIP_PKT_PacketAcknowledgeSet(&pFragTx->macPkt, TCPIP_IPV4_FragmentTxAckFnc, 0);
            pFragTx->fragSeg.segFlags = TCPIP_MAC_SEG_FLAG_STATIC; // embedded in packet itself
            // copy the header part: MAC + IPv4
            memcpy(pFragTx->macPkt.pMacLayer, pPkt->macPkt.pMacLayer, pktHeaderSize);
            pFragTx->macPkt.pDSeg->segLen = pktHeaderSize;
            
            // link
            tail->pkt_next = &pFragTx->macPkt;
            tail = &pFragTx->macPkt;
        }
        else
        {   // allocation failed
            break;
        }
    }

    if(ix != nFrags)
    {   // couldn't allocate all packet fragments
        for(pMacPkt = pPkt->macPkt.pkt_next; pMacPkt != 0; pMacPkt = pMacNext)
        {   // deallocate
            pMacNext = pMacPkt->pkt_next;
            TCPIP_PKT_PacketFree(pMacPkt);
        } 

        return false;
    }


    // assemble the fragments
    pIpv4Load = pPkt->macPkt.pTransportLayer; 
    for(ix = 0, pMacPkt = &pPkt->macPkt; ix < nFrags; ix++, pMacPkt = pMacPkt->pkt_next)
    {
        uint16_t currFragSize = ix == nFrags - 1 ? lastFragSize : fragSize;

        if(ix == 0)
        {   // this is the master/parent packet
            pSeg = pMacPkt->pDSeg;
            pSeg->segLen = pktHeaderSize + currFragSize;    // truncate size
        }
        else
        {
            pFragTx = (IPV4_FRAG_TX_PKT*)pMacPkt;
            pSeg = &pFragTx->fragSeg;
            pSeg->segLen = pSeg->segSize = currFragSize;
            pSeg->segLoad = pIpv4Load;
            pMacPkt->pDSeg->next = pSeg;  // link to the 1st data seg to be transmitted
        }

        // adjust the IPv4 header
        pHdr = (IPV4_HEADER*)pMacPkt->pNetLayer;
        pHdr->TotalLength = TCPIP_Helper_htons(ipv4HeaderSize + currFragSize);
        fragInfo.val = 0;
        fragInfo.fragOffset = (pIpv4Load - pPkt->macPkt.pTransportLayer) / 8; 
        if(ix != nFrags - 1)
        {
            fragInfo.MF = 1;
        }
        pHdr->FragmentInfo.val = TCPIP_Helper_htons(fragInfo.val);
        // update the checksum
        pHdr->HeaderChecksum = 0;
        pHdr->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHdr, ipv4HeaderSize, 0);

        pIpv4Load += currFragSize;
    }

    // success
    return true;
}

static bool TCPIP_IPV4_FragmentTxAckFnc(TCPIP_MAC_PACKET* pkt,  const void* param)
{
    // this is temporary fragment packet;
    // if MAC is done with it, just delete
    TCPIP_PKT_PacketFree(pkt);
    return false;
}

// transmits the packet as multiple fragments
static bool TCPIP_IPV4_TxMacPkt(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_MAC_PACKET *pFragPkt, *pFragNext;

    for(pFragPkt = pPkt; pFragPkt != 0; pFragPkt = pFragNext)
    {
        pFragNext = pFragPkt->pkt_next;
        if(_TCPIPStackPacketTx(pNetIf, pFragPkt) != TCPIP_MAC_RES_OK)
        {
            return false;
        }
    }

    return true;
}


static void TCPIP_IPV4_FragmentTxAcknowledge(TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PKT_ACK_RES ackRes, IPV4_FRAG_TX_ACK txAck)
{
    TCPIP_MAC_PACKET *pFragHead, *pFragPkt, *pFragNext;

    pFragHead = ((txAck & IPV4_FRAG_TX_ACK_HEAD) == 0) ? pTxPkt->pkt_next : pTxPkt;

    for(pFragPkt = pFragHead; pFragPkt != 0; pFragPkt = pFragNext)
    {
        pFragNext = pFragPkt->pkt_next;
        TCPIP_PKT_PacketAcknowledge(pFragPkt, ackRes);
    }
}

static void TCPIP_IPV4_FragmentTxInsertToRx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PACKET_FLAGS flags, bool signal)
{
    TCPIP_MAC_PACKET *pFragPkt;

    for(pFragPkt = pTxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
    {
        pTxPkt->pktFlags |= flags;
        _TCPIPStackInsertRxPacket(pNetIf, pFragPkt, signal);
    }
}


#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)


#endif  // defined(TCPIP_STACK_USE_IPV4)


