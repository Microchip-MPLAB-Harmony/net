/*******************************************************************************
  Internet Protocol (IP) Version 4 Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides a transport for TCP, UDP, and ICMP messages
    -Reference: RFC 791
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

static PROTECTED_SINGLE_LIST ipv4ArpQueue = { {0} };    // queue of packets waiting for ARP resolution
static SINGLE_LIST          ipv4ArpPool = {0};          // pool of ARP entries
                                                        // access protected by ipv4ArpQueue!
static IPV4_ARP_ENTRY*      ipv4ArpEntries = 0;         // allocated nodes for ipv4ArpPool 

static TCPIP_ARP_HANDLE     ipv4ArpHandle = 0;          // ARP registration handle

static uint16_t             ipv4InitCount = 0;

static PROTECTED_SINGLE_LIST ipv4PacketFilters = { {0} };
static volatile uint32_t    ipv4ActFilterCount = 0;    // counter of active filters
                                                        // access protected by ipv4PacketFilters 

static TCPIP_IPV4_FILTER_TYPE ipv4FilterType = 0;       // IPv4 current filter

#if defined(TCPIP_IPV4_FRAGMENTATION) && (TCPIP_IPV4_FRAGMENTATION != 0)
#define _TCPIP_IPV4_FRAGMENTATION    1
#else
#define _TCPIP_IPV4_FRAGMENTATION    0
#endif

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
static SINGLE_LIST          ipv4FragmentQueue = {0};  // IPv4 fragments to be processed
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

typedef enum
{
    TCPIP_IPV4_DEST_FAIL    = -1,   // cannot find a destination
    TCPIP_IPV4_DEST_SELF    = 0,    // route it internally
    TCPIP_IPV4_DEST_NETWORK = 1,    // send on the net segment
    TCPIP_IPV4_DEST_GW      = 2,    // pass it to the router

}TCPIP_IPV4_DEST_TYPE;


#if (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)
static TCPIP_IPV4_PACKET_HANDLER ipv4PktHandler = 0;
static const void* ipv4PktHandlerParam;
#endif  // (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
static IPV4_FORWARD_DESCRIPTOR* ipv4ForwardDcpt = 0;    // the forward descriptor, for all interfaces
static IPV4_FORWARD_NODE*       ipv4ForwardNodes;
static DOUBLE_LIST              ipv4ForwardPool;        // pool of Forward Node entries
static DOUBLE_LIST              ipv4ForwardQueue;       // list of forward nodes waiting to be transmitted
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

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

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
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
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_RX_CHECK) != 0)
static uint16_t checkRxUdpSrcPort = 67; 
static uint16_t checkRxUdpDstPort = 68; 

static uint16_t checkRxTcpSrcPort = 9760; 
static uint16_t checkRxTcpDstPort = 9760; 

static uint32_t checkRxUdpBkptCnt = 0;
static uint32_t checkRxIcmpBkptCnt = 0;
static uint32_t checkRxTcpBkptCnt = 0;
static void TCPIP_IPV4_CheckRxPkt(TCPIP_MAC_PACKET* pRxPkt)
{
    IPV4_HEADER* pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;
    if(pHeader->Protocol == IP_PROT_ICMP)
    {
        checkRxIcmpBkptCnt++;
    }
    else if(pHeader->Protocol == IP_PROT_UDP)
    {   // UDP packet
        uint8_t headerLen = pHeader->IHL << 2;
        UDP_HEADER* pUDPHdr = (UDP_HEADER*)(pRxPkt->pNetLayer + headerLen);
        UDP_PORT destPort = TCPIP_Helper_ntohs(pUDPHdr->DestinationPort);
        UDP_PORT srcPort = TCPIP_Helper_ntohs(pUDPHdr->SourcePort);
        if(srcPort == checkRxUdpSrcPort || destPort == checkRxUdpDstPort)
        {
            checkRxUdpBkptCnt++;
        }
    }
    else if(pHeader->Protocol == IP_PROT_TCP)
    {   
        TCP_HEADER* pTcpHdr = (TCP_HEADER*)(pRxPkt->pNetLayer + sizeof(TCP_HEADER));
        TCP_PORT destPort = TCPIP_Helper_ntohs(pTcpHdr->DestPort);
        TCP_PORT srcPort = TCPIP_Helper_ntohs(pTcpHdr->SourcePort);
        if(srcPort == checkRxTcpSrcPort || destPort == checkRxTcpDstPort)
        {
            checkRxTcpBkptCnt++;
        }
    }

}
#else
#define TCPIP_IPV4_CheckRxPkt(pRxPkt)
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_RX_CHECK) != 0)


#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD_MAC_DEST) != 0)
static uint32_t _ipv4DbgDestTbl[] = {0xf502a8c0, 0x9301a8c0};   // table with destinations to be matched against
static uint32_t* _ipv4DbgDest = _ipv4DbgDestTbl;

static void _IPv4FwdMacDestDebug(const IPV4_ADDR* pDestAdd, const IPV4_ADDR* arpTarget, TCPIP_NET_IF* pNetIf, TCPIP_IPV4_DEST_TYPE destType, bool solved)
{
    int ix;
    char destBuff[20];
    char targetBuff[20];

    TCPIP_Helper_IPAddressToString(pDestAdd, destBuff, sizeof(destBuff));
    TCPIP_Helper_IPAddressToString(arpTarget, targetBuff, sizeof(targetBuff));

    bool doPrint = false;

    if(_ipv4DbgDest != 0)
    {   // search for a matching destination address
        for(ix = 0; ix < sizeof(_ipv4DbgDestTbl) / sizeof(*_ipv4DbgDestTbl); ix++)
        {
            if(pDestAdd->Val == _ipv4DbgDestTbl[ix])
            {
                doPrint = true;
                break;
            }
        }
    }
    else
    {   // print everything 
        doPrint = true;
    }

    if(doPrint)
    {
        SYS_CONSOLE_PRINT("IPv4 Fwd MAC Dest - fwd if: %d, dest: %s, target: %s, res: %d, found: %d\r\n", pNetIf->netIfIx, destBuff, targetBuff, destType, solved);
    }
} 
#else
#define _IPv4FwdMacDestDebug(pDestAdd, arpTarget, pNetIf, destType, solved)
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD_MAC_DEST) != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)

static TCPIP_IPV4_ARP_QUEUE_STAT _ipv4_arp_stat = {0};

bool TCPIP_IPv4_ArpStatGet(TCPIP_IPV4_ARP_QUEUE_STAT* pStat, bool clear)
{
    if(pStat)
    {
        _ipv4_arp_stat.nPool = TCPIP_Helper_SingleListCount(&ipv4ArpPool); 
        _ipv4_arp_stat.nPend = TCPIP_Helper_SingleListCount(&ipv4ArpQueue.list); 

        *pStat = _ipv4_arp_stat;
    }
    if(clear)
    {
        memset(&_ipv4_arp_stat, 0, sizeof(_ipv4_arp_stat));
    }
    return true;
}

#else
bool TCPIP_IPv4_ArpStatGet(TCPIP_IPV4_ARP_QUEUE_STAT* pStat, bool clear)
{
    return false;
}
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0) && ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_PROC_EXT) != 0)

static uint32_t _ipv4DbgExtTbl[] = {0xf002a8c0};   // table with addresses (src, dest) to be matched against
static uint32_t* _ipv4DbgExt = _ipv4DbgExtTbl;

static void _IPv4ProcessExtPktDbg(TCPIP_MAC_PACKET* pRxPkt)
{
    int ix;
    char destBuff[20];
    char srcBuff[20];

    TCPIP_NET_IF* pInIf = (TCPIP_NET_IF*)pRxPkt->pktIf;
    const IPV4_ADDR* dstAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);
    const IPV4_ADDR* srcAdd = TCPIP_IPV4_PacketGetSourceAddress(pRxPkt);
    TCPIP_Helper_IPAddressToString(dstAdd, destBuff, sizeof(destBuff));
    TCPIP_Helper_IPAddressToString(srcAdd, srcBuff, sizeof(srcBuff));

    bool doPrint = false;
    if(_ipv4DbgExt != 0)
    {
        for(ix = 0; ix < sizeof(_ipv4DbgExtTbl) / sizeof(*_ipv4DbgExtTbl); ix++)
        {
            if(_ipv4DbgExtTbl[ix] == dstAdd->Val || _ipv4DbgExtTbl[ix] == srcAdd->Val)
            {   // match
                doPrint = true;
                break;
            }
        } 
    }
    else
    {
        doPrint = true;
    }

    if(doPrint)
    {
        SYS_CONSOLE_PRINT("IPv4 Proc Ext - in if: %d, dest: %s, source: %s\r\n", pInIf->netIfIx, destBuff, srcBuff);
    }
}
#else
#define _IPv4ProcessExtPktDbg(pRxPkt)
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0) && ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_PROC_EXT) != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)

static TCPIP_IPV4_FORWARD_STAT _ipv4_fwd_stat[3] = {{0}};  // FWD stats on if 0, 1, all else

bool TCPIP_IPv4_ForwardStatGet(size_t index, TCPIP_IPV4_FORWARD_STAT* pStat, bool clear)
{
    if(index < sizeof(_ipv4_fwd_stat) / sizeof*(_ipv4_fwd_stat))
    {
        TCPIP_IPV4_FORWARD_STAT* pFwdStat = _ipv4_fwd_stat + index;

        if(pStat)
        {
            *pStat = *pFwdStat;
        }

        if(clear)
        {
            memset(pFwdStat, 0, sizeof(*pFwdStat));
        }
        return true;
    }

    return false;
}

#else
bool TCPIP_IPv4_ForwardStatGet(size_t index, TCPIP_IPV4_FORWARD_STAT* pStat, bool clear)
{
    return false;
}
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FILT_COUNT) != 0)
static uint32_t ipv4PrevFCount = -1;

static void _IPv4FiltCountDbg(const char* msg)
{
    if(ipv4ActFilterCount != ipv4PrevFCount)
    {
        ipv4PrevFCount = ipv4ActFilterCount;
        SYS_CONSOLE_PRINT("IPv4 Filt: %s - count: %d\r\n", msg, ipv4PrevFCount);
    }
}
#else
#define _IPv4FiltCountDbg(msg)
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FILT_COUNT) != 0)

static bool TCPIP_IPV4_QueueArpPacket(void* pPkt, int arpIfIx, IPV4_ARP_PKT_TYPE type, IPV4_ADDR* arpTarget);

static void TCPIP_IPV4_ArpHandler(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType, const void* param);

static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktHost(TCPIP_NET_IF* pNetIf, IPV4_HEADER* pHeader, TCPIP_MAC_PACKET* pRxPkt);

static TCPIP_NET_IF* TCPIP_IPV4_CheckPktTx(TCPIP_NET_HANDLE hNet, TCPIP_MAC_PACKET* pPkt);

static bool TCPIP_IPV4_VerifyPktFilters(TCPIP_MAC_PACKET* pRxPkt, uint8_t hdrlen);

static TCPIP_STACK_MODULE TCPIP_IPV4_FrameDestination(IPV4_HEADER* pHeader);

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
static void TCPIP_IPV4_Timeout(void);

// RX fragmentation
static TCPIP_MAC_PKT_ACK_RES    TCPIP_IPV4_RxFragmentInsert(TCPIP_MAC_PACKET* pRxPkt, IPV4_FRAGMENT_NODE **ppFrag);
static void                     TCPIP_IPV4_RxFragmentDiscard(IPV4_FRAGMENT_NODE* pFrag, TCPIP_MAC_PKT_ACK_RES ackRes);
static void                     TCPIP_IPV4_RxFragmentListPurge(SINGLE_LIST* pL);


// TX fragmentation
static bool TCPIP_IPV4_FragmentTxPkt(TCPIP_MAC_PACKET* pMacPkt, uint16_t linkMtu, uint16_t pktPayload);
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
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_PktMacDestination(IPV4_PACKET* pPkt, const IPV4_ADDR* pIpAdd, TCPIP_MAC_ADDR** ppMacAdd, IPV4_ADDR* arpTarget);

static void TCPIP_IPV4_Process(void);

static TCPIP_MAC_PKT_ACK_RES TCPIP_IPV4_DispatchPacket(TCPIP_MAC_PACKET* pRxPkt);

static IPV4_OPTION_FIELD* _IPv4CheckPacketOption(TCPIP_MAC_PACKET* pRxPkt, int* pOptLen);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_IPV4_Cleanup(void);
static void TCPIP_IPV4_ArpListPurge(TCPIP_NET_IF* pNetIf);
#else
#define TCPIP_IPV4_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
static TCPIP_IPV4_RES IPV4_BuildForwardTables(const TCPIP_IPV4_MODULE_CONFIG* pIpInit, const void* memH, int nIfs);
static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_FwdPktMacDestination(TCPIP_MAC_PACKET* pFwdPkt, const IPV4_ROUTE_TABLE_ENTRY* pEntry, TCPIP_MAC_ADDR** ppMacAdd, IPV4_ADDR* arpTarget);
static bool TCPIP_IPV4_ForwardPkt(TCPIP_MAC_PACKET* pFwdPkt, const IPV4_ROUTE_TABLE_ENTRY* pEntry, IPV4_PKT_PROC_TYPE procType);
static bool TCPIP_IPV4_ProcessExtPkt(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV4_PKT_PROC_TYPE procType);
static const IPV4_ROUTE_TABLE_ENTRY* TCPIP_IPV4_FindFwdRoute(IPV4_FORWARD_DESCRIPTOR* pFDcpt, TCPIP_MAC_PACKET* pRxPkt);
static TCPIP_UINT32_VAL IPV4_32LeadingOnes(uint32_t value);
#if (TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0)
static TCPIP_IPV4_RES IPv4_BuildAsciiTable(IPV4_FORWARD_DESCRIPTOR* pFDcpt, const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pAEntry, size_t nEntries);
#endif  // (TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0)
#if (TCPIP_IPV4_FORWARDING_TABLE_BIN != 0)
static TCPIP_IPV4_RES IPv4_BuildBinaryTable(IPV4_FORWARD_DESCRIPTOR* pFDcpt, const TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry, size_t nEntries);
#endif  // (TCPIP_IPV4_FORWARDING_TABLE_BIN != 0)
static void IPv4_SortFwdTable(IPV4_ROUTE_TABLE_ENTRY* pTable, size_t tableEntries);
static IPV4_FORWARD_NODE* TCPIP_IPV4_Forward_QueuePacket(TCPIP_MAC_PACKET* pFwdPkt, IPV4_PKT_PROC_TYPE procType);
static bool TCPIP_IPV4_Forward_DequeuePacket(IPV4_FORWARD_NODE* pFwdNode, bool aliveCheck);
static bool TCPIP_IPV4_ForwardAckFunc(TCPIP_MAC_PACKET* pkt,  const void* param);
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktFwd(TCPIP_NET_IF* pNetIf, IPV4_HEADER* pHeader, TCPIP_MAC_PACKET* pRxPkt);
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPkt(TCPIP_NET_IF* pNetIf, IPV4_HEADER* pHeader, TCPIP_MAC_PACKET* pRxPkt);
#else

#define TCPIP_IPV4_VerifyPkt(pNetIf, pHeader, pRxPkt)   TCPIP_IPV4_VerifyPktHost(pNetIf, pHeader, pRxPkt)  

#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

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
    int ix;
    TCPIP_IPV4_RES iniRes;
    IPV4_ARP_ENTRY* pEntry;

    if(stackInit->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    if(ipv4InitCount == 0)
    {
        _IPv4AssertCond(true, 0, 0);        // hush compiler warning - unused

        while(true)
        {
            iniRes = TCPIP_IPV4_RES_OK;
            ipv4MemH = stackInit->memH;
            ipv4ArpHandle = 0;
            signalHandle = 0;
            memset(&ipv4ArpQueue, 0, sizeof(ipv4ArpQueue));
            memset(&ipv4ArpPool, 0, sizeof(ipv4ArpPool));
            ipv4ArpEntries = 0;
            memset(&ipv4PacketFilters, 0, sizeof(ipv4PacketFilters));
            ipv4ActFilterCount = 0;
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
            ipv4ForwardDcpt = 0;
            ipv4ForwardNodes = 0;
            TCPIP_Helper_DoubleListInitialize(&ipv4ForwardPool); 
            TCPIP_Helper_DoubleListInitialize(&ipv4ForwardQueue); 
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

            // check initialization data is provided and minimal sanity check
            if(pIpInit == 0 || pIpInit->arpEntries == 0)
            {
                iniRes = TCPIP_IPV4_RES_INIT_VAL_ERR;
                break;
            }
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
            TCPIP_Helper_SingleListInitialize(&ipv4FragmentQueue);
            signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_IPV4_Task, TCPIP_IPV4_TASK_TICK_RATE);
#else
            signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_IPV4_Task, 0);
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)
            if(signalHandle == 0)
            {
                iniRes = TCPIP_IPV4_RES_SIGNAL_ERR;
                break;
            }

            if(!TCPIP_Helper_ProtectedSingleListInitialize (&ipv4ArpQueue))
            {
                iniRes = TCPIP_IPV4_RES_ARP_ERR;
                break;
            }

            if(!TCPIP_Notification_Initialize(&ipv4PacketFilters))
            {
                iniRes = TCPIP_IPV4_RES_NOTIFY_ERR;
                break;
            }

            ipv4ArpEntries = (IPV4_ARP_ENTRY*)TCPIP_HEAP_Calloc(ipv4MemH, pIpInit->arpEntries , sizeof(*ipv4ArpEntries));
            if(ipv4ArpEntries == 0)
            {   // allocation failed
                iniRes = TCPIP_IPV4_RES_MEM_ERR;
                break;
            }
            // build the ARP pool
            TCPIP_Helper_SingleListInitialize(&ipv4ArpPool);
            pEntry = ipv4ArpEntries;
            for(ix = 0; ix < pIpInit->arpEntries; ix++, pEntry++)
            {
                TCPIP_Helper_SingleListTailAdd(&ipv4ArpPool, (SGL_LIST_NODE*)pEntry); 
            }


#if (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)
            ipv4PktHandler = 0;
#endif  // (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
            if(stackInit->nIfs > 1)
            {   // forwarding enabled only for multiple interfaces
                if(pIpInit->forwardTableMaxEntries == 0)
                {
                    iniRes = TCPIP_IPV4_RES_INIT_VAL_ERR;
                    break;
                }

                // build the forwarding table
                if((iniRes = IPV4_BuildForwardTables(pIpInit, ipv4MemH, stackInit->nIfs)) < 0)
                {   // failed
                    break;
                }
            }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

            break;
        }
        
        if(iniRes < 0)
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, "IPv4 Init: Failed to initalize: %d! \r\n", iniRes);
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
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
        TCPIP_IPV4_RxFragmentListPurge(&ipv4FragmentQueue);
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

        TCPIP_IPV4_ArpListPurge(stackCtrl->pNetIf);

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
    ipv4ActFilterCount = 0;

    TCPIP_Helper_ProtectedSingleListDeinitialize(&ipv4ArpQueue);

    if(signalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(signalHandle);
        signalHandle = 0;
    }

    if(ipv4ArpEntries != 0)
    {
        TCPIP_HEAP_Free(ipv4MemH, ipv4ArpEntries);
        ipv4ArpEntries = 0;
    }

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
    if(ipv4ForwardDcpt != 0)
    {
        TCPIP_HEAP_Free(ipv4MemH, ipv4ForwardDcpt);
        ipv4ForwardDcpt = 0;
    }

    if(ipv4ForwardNodes)
    {
        TCPIP_HEAP_Free(ipv4MemH, ipv4ForwardNodes);
        ipv4ForwardNodes = 0;
    }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

    ipv4MemH = 0;
}

// purges the ARP pending list of IPV4_PACKET packets
// searches for pNetIf match
// if pNetIf == 0 everything matches
static void TCPIP_IPV4_ArpListPurge(TCPIP_NET_IF* pNetIf)
{
    SINGLE_LIST         newList;
    IPV4_ARP_ENTRY*     pEntry;
    TCPIP_MAC_PACKET*   pMacPkt;
    TCPIP_NET_IF*       pPktIf;
    

    TCPIP_Helper_SingleListInitialize (&newList);

    PROTECTED_SINGLE_LIST* pList = &ipv4ArpQueue;
    TCPIP_Helper_ProtectedSingleListLock(pList);
    // traverse the list
    // and find all the packets matching the pNetIf

    while((pEntry = (IPV4_ARP_ENTRY*)TCPIP_Helper_SingleListHeadRemove(&pList->list)) != 0)
    {
        if(pEntry->type == IPV4_ARP_PKT_TYPE_TX)
        {   // IPV4_PACKET*
            pMacPkt = &pEntry->pTxPkt->macPkt;
        }
        else if(pEntry->type == IPV4_ARP_PKT_TYPE_MAC)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pMacPkt; 
        }
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        else if(pEntry->type == IPV4_ARP_PKT_TYPE_FWD)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pFwdPkt; 
        }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        else
        {   // should not happen!
            _IPv4AssertCond(false, __func__, __LINE__);
            return;
        }

        pPktIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(pEntry->arpIfIx);

        if(pNetIf == 0 || pNetIf == pPktIf)
        {   // match
            if(pEntry->type == IPV4_ARP_PKT_TYPE_TX || pEntry->type == IPV4_ARP_PKT_TYPE_MAC)
            {   // IPV4_PACKET TX packet 
                TCPIP_IPV4_FragmentTxAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_ARP_NET_ERR, IPV4_FRAG_TX_ACK_HEAD | IPV4_FRAG_TX_ACK_FRAGS);
            }
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
            else if(pEntry->type == IPV4_ARP_PKT_TYPE_FWD)
            {   // FWD Packet
                TCPIP_PKT_PacketAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_ARP_NET_ERR);
            }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)
            // back to pool
            TCPIP_Helper_SingleListTailAdd(&ipv4ArpPool, (SGL_LIST_NODE*)pEntry); 
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newList, (SGL_LIST_NODE*)pEntry);
        }
    }

    // update the arp queue
    pList->list = newList;
    TCPIP_Helper_ProtectedSingleListUnlock(pList);
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)

// checks if this packet needs forwarding
// returns true if forwarded,
// false if forwarding failed
static bool TCPIP_IPV4_ProcessExtPkt(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV4_PKT_PROC_TYPE procType)
{
    if(ipv4ForwardDcpt == 0)
    {   // no forwarding tables
        return false;
    }

    int netIx = _TCPIPStackNetIxGet(pNetIf);

    IPV4_FORWARD_DESCRIPTOR* pFDcpt = ipv4ForwardDcpt + netIx;

    while((pFDcpt->runFlags & IPV4_FWD_FLAG_FWD_ENABLE) != 0)
    {   // forwarding enabled
        IPV4_HEADER* pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;
        if(pHeader->TimeToLive == 0)
        {   // TTL expired
            return false;
        }

        _IPv4ProcessExtPktDbg(pRxPkt);

        // find route
        const IPV4_ROUTE_TABLE_ENTRY* pEntry = TCPIP_IPV4_FindFwdRoute(pFDcpt, pRxPkt);
        if(pEntry)
        {   // found it
            return TCPIP_IPV4_ForwardPkt(pRxPkt, pEntry, procType);
        }

        // no route
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        TCPIP_IPV4_FORWARD_STAT* pFwdDbg = _ipv4_fwd_stat + (netIx < 2 ? netIx : 2);
        pFwdDbg->failNoRoute++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        break;
    }

    return false;
}

// Finds the entry in the routing table that routes this packet
// Select the entry from the table, based on the number of leading ones
// For now, simple linear search...
static const IPV4_ROUTE_TABLE_ENTRY* TCPIP_IPV4_FindFwdRoute(IPV4_FORWARD_DESCRIPTOR* pFDcpt, TCPIP_MAC_PACKET* pRxPkt)
{
    int ix;
    const IPV4_ROUTE_TABLE_ENTRY* pEntry = pFDcpt->fwdTable;
    // packet destination
    const IPV4_ADDR* dstAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);

    for(ix = 0; ix < pFDcpt->usedEntries; ix++, pEntry++)
    {
        if((dstAdd->Val & pEntry->mask) == pEntry->netAdd)
        {   // found it
            return pEntry;
        }
    }

    // no route
    return 0;
} 

// select destination MAC address
// for an externally forwarded packet
// ppMacAdd is set to 0 if the MAC address is not available yet (ARP)
static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_FwdPktMacDestination(TCPIP_MAC_PACKET* pFwdPkt, const IPV4_ROUTE_TABLE_ENTRY* pEntry, TCPIP_MAC_ADDR** ppMacAdd, IPV4_ADDR* arpTarget)
{
    TCPIP_ARP_RESULT    arpRes;
    TCPIP_MAC_ADDR*     pMacDst = *ppMacAdd;
    TCPIP_IPV4_DEST_TYPE destType;

    // destination interface
    TCPIP_NET_IF* pDestIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(pEntry->outIfIx);

    // packet destination
    const IPV4_ADDR* pDestAdd = TCPIP_IPV4_PacketGetDestAddress(pFwdPkt);

    while(true)
    {
        // check dest is not host 0
        if(_TCPIPStackHostPartAddress(pDestIf, pDestAdd) == 0)
        {   // invalid, discard
            destType = TCPIP_IPV4_DEST_FAIL;
            break;
        }

        // check if it's a net-directed broadcasts
        if(_TCPIPStack_IsDirectedBcast(pDestIf, pDestAdd))
        {   
            memset(pMacDst, 0xff, sizeof(*pMacDst));
            destType = TCPIP_IPV4_DEST_NETWORK; 
            break;
        }

        // check IP multicast address range from 224.0.0.0 to 239.255.255.255
        // can be done locally; No need for an ARP request.
        else if ((pDestAdd->v[0] >= 224) && (pDestAdd->v[0] <= 239))
        {
            pMacDst->v[0] = 0x01;
            pMacDst->v[1] = 0x00;
            pMacDst->v[2] = 0x5E;
            pMacDst->v[3] = 0x7f & pDestAdd->v[1];
            pMacDst->v[4] = pDestAdd->v[2];
            pMacDst->v[5] = pDestAdd->v[3];
            destType = TCPIP_IPV4_DEST_NETWORK; 
            break;
        }
        else
        {   
            if(pEntry->gwAdd != 0)
            {   
                arpTarget->Val  = pEntry->gwAdd;
                destType = TCPIP_IPV4_DEST_GW;
            }
            else if(_TCPIPStackIpAddFromLAN(pDestIf, pDestAdd))
            {   // this LAN
                arpTarget->Val = pDestAdd->Val;
                destType = TCPIP_IPV4_DEST_NETWORK;
            }
            else
            {   // cannot send
                destType = TCPIP_IPV4_DEST_FAIL;
                break;
            }

            arpRes = TCPIP_ARP_EntryGet(pDestIf, arpTarget, pMacDst, true);
            if(arpRes == ARP_RES_ENTRY_SOLVED)
            {   // good to transmit
            }
            else if(arpRes == ARP_RES_ENTRY_QUEUED || arpRes == ARP_RES_ENTRY_NEW)
            {   // will have to be queued
                *ppMacAdd = 0;    // not known yet
            }
            else
            {   // discard, cannot send
                destType = TCPIP_IPV4_DEST_FAIL;
            }
        }
        break;
    }

    _IPv4FwdMacDestDebug(pDestAdd, arpTarget, pDestIf, destType, ppMacAdd != 0);

    return destType;
}
// forwards a packet over a network
// returns true if success
// false otherwise
static bool TCPIP_IPV4_ForwardPkt(TCPIP_MAC_PACKET* pFwdPkt, const IPV4_ROUTE_TABLE_ENTRY* pEntry, IPV4_PKT_PROC_TYPE procType)
{
    TCPIP_MAC_ADDR   destMacAdd, *pMacDst;
    TCPIP_IPV4_DEST_TYPE destType;
    TCPIP_NET_IF*   pFwdIf;
    TCPIP_MAC_ETHERNET_HEADER* macHdr;
    IPV4_FORWARD_NODE* pFwdNode;
    IPV4_ADDR       arpTarget;
    bool            macRes;
    uint16_t        pktPayload, linkMtu;
    uint8_t         headerLen;

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        TCPIP_IPV4_FORWARD_STAT* pFwdDbg = _ipv4_fwd_stat + (pEntry->outIfIx < 2 ? pEntry->outIfIx : 2);
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
    
    // the forward interface
    // do NOT set the packet interface, as this could be redirected internally too...
    pFwdIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(pEntry->outIfIx);

    // check for proper packet source address:
    const IPV4_ADDR* pSrcAdd = TCPIP_IPV4_PacketGetSourceAddress(pFwdPkt);
    if(_TCPIPStackNetPartAddress(pFwdIf, pSrcAdd) == 0)
    {   // RFC 1812: source address {0, host} is not forwarded
        return false;
    }

    // select packet's destination MAC address
    pMacDst = &destMacAdd;
    // select packet's external destination MAC address
    destType = TCPIP_IPV4_FwdPktMacDestination(pFwdPkt, pEntry, &pMacDst, &arpTarget);
    if(destType == TCPIP_IPV4_DEST_FAIL) 
    {   // discard, cannot send
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        pFwdDbg->failMacDest++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        return false;
    }

    if(!TCPIP_STACK_NetworkIsUp(pFwdIf))
    {   // don't send over dead interface
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        pFwdDbg->failNetDown++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        return false;
    }

    if(destType != TCPIP_IPV4_DEST_SELF) 
    {   // check the MTU
        // get the payload w/o the MAC frame
        pktPayload = TCPIP_PKT_PayloadLen(pFwdPkt);
        linkMtu = _TCPIPStackNetLinkMtu(pFwdIf);
        if(pktPayload > linkMtu)
        {
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            pFwdDbg->failMtu++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            return false;
        }
    }

    if((procType & IPV4_PKT_DEST_HOST) != 0)
    {   // after forwarding need to be processed locally  
        // save packet MAC address before changing anything
        pFwdNode = TCPIP_IPV4_Forward_QueuePacket(pFwdPkt, procType);
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        if(pFwdNode == 0)
        {   // no more packets could be queued up
            pFwdDbg->failFwdQueue++;
        }
        else
        {
            pFwdDbg->fwdQueuedPackets++;
        }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        if(pFwdNode == 0)
        {   // no more packets could be queued up
            return false;
        }
    }
    else
    {
        pFwdNode = 0;
    }

    // set the proper source and destination MAC addresses
    macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pFwdPkt->pMacLayer;
    if(pMacDst != 0)
    {
        memcpy(&macHdr->DestMACAddr, pMacDst, sizeof(macHdr->DestMACAddr));
    }
    memcpy(&macHdr->SourceMACAddr, (const TCPIP_MAC_ADDR*)_TCPIPStackNetMACAddress(pFwdIf), sizeof(macHdr->SourceMACAddr));
    pFwdPkt->pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);
    pFwdPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_TX; 

    // adjust the TTL and recalculate the IP checksum
    IPV4_HEADER* pHeader = (IPV4_HEADER*)pFwdPkt->pNetLayer;
    pHeader->TimeToLive -= 1;
    pHeader->HeaderChecksum = 0;
    headerLen = pHeader->IHL << 2;
    pHeader->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHeader, headerLen, 0);

    if(pMacDst == 0)
    {   // ARP target not known yet; queue it
        // cannot have both IPV4_PKT_DEST_FWD and IPV4_PKT_DEST_HOST and not know the ARP!
        _IPv4AssertCond((procType & IPV4_PKT_DEST_HOST) == 0, __func__, __LINE__);

        if(ipv4ArpHandle == 0)
        {
            if((ipv4ArpHandle = TCPIP_ARP_HandlerRegister(0, TCPIP_IPV4_ArpHandler, 0)) == 0)
            {
                SYS_ERROR(SYS_ERROR_WARNING, "IPv4 Fw: Failed to register ARP notification! \r\n");
                return false;
            }
        }

        if(!TCPIP_IPV4_QueueArpPacket(pFwdPkt, pEntry->outIfIx, IPV4_ARP_PKT_TYPE_FWD, &arpTarget))
        {
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            pFwdDbg->failArpQueue++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            return false;
        }

        // ARP notification will be received: either TMO or resolved
        // mark packet as queued 
        pFwdPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        pFwdDbg->arpQueued++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        return true;
    }

    TCPIP_PKT_FlightLogTx(pFwdPkt, TCPIP_THIS_MODULE_ID);


    // TCPIP_IPV4_DEST_NETWORK/TCPIP_IPV4_DEST_GW and pMacDst != 0
    if(pFwdNode != 0)
    {   // after forwarding need to be processed locally  
        macRes = TCPIP_IPV4_TxMacPkt(pFwdIf, pFwdPkt);
        if(macRes == false)
        {   // failed and need to dequeue...
            pFwdPkt->ipv4PktData = 0;   // clear the 'forward first process later' flag
            TCPIP_IPV4_Forward_DequeuePacket(pFwdNode, false);
        }
    }
    else
    {   // normal - unicast probably - forwarding
        macRes = TCPIP_IPV4_TxMacPkt(pFwdIf, pFwdPkt);
    }


#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
    if(macRes)
    {
        pFwdDbg->macPackets++;
    }
    else
    {
        pFwdDbg->failMac++;
    }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)

    return macRes;
}

// simplistic implementation of count leading ones in a network order 32 bit value
// It returns the number of contiguous leading ones - in the hi part
// followed by contiguous zeroes - in the low part
// Counting stops if after detecting zeroes, a one is found again
static TCPIP_UINT32_VAL IPV4_32LeadingOnes(uint32_t value)
{
    int ix;
    TCPIP_UINT32_VAL count;
    uint32_t mask;
    uint16_t nOnes, nZeroes;
    bool countZeroes;      // true: counting zeroes

    nOnes = nZeroes = 0;
    countZeroes = false;
    mask = 1 << 0;  // network order; start from LSB

    for(ix = 0; ix < 32; ix++)
    {
        if((value & mask) != 0)
        {   // 1 detected
            if(countZeroes)
            {   // already counting zeroes
                break;
            }
            nOnes++;
        }
        else
        {   // 0 detected
            countZeroes = true;
            nZeroes++;
        }
        mask <<= 1;
    }

    count.word.HW = nOnes;
    count.word.LW = nZeroes;

    return count;
}

static TCPIP_IPV4_RES IPV4_BuildForwardTables(const TCPIP_IPV4_MODULE_CONFIG* pIpInit, const void* memH, int nIfs)
{
    TCPIP_IPV4_RES buildRes;
    bool    knownFmt;
    int     netIx;
    size_t  usedEntries;
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;
    IPV4_ROUTE_TABLE_ENTRY* pFwdTbl;
    IPV4_FORWARD_NODE*      pFwdNode;

    // allocate the descriptors
    ipv4ForwardDcpt = (IPV4_FORWARD_DESCRIPTOR*)TCPIP_HEAP_Calloc(memH, nIfs, sizeof(*pFDcpt) + pIpInit->forwardTableMaxEntries * sizeof(*pFwdTbl));
    if(ipv4ForwardDcpt == 0)
    {   // out of memory
        return TCPIP_IPV4_RES_MEM_ERR;
    }

    if(pIpInit->forwardTxQueueSize != 0)
    {
        ipv4ForwardNodes = (IPV4_FORWARD_NODE*)TCPIP_HEAP_Calloc(memH, pIpInit->forwardTxQueueSize, sizeof(*ipv4ForwardNodes));
        if(ipv4ForwardNodes == 0)
        {   // out of memory
            return TCPIP_IPV4_RES_MEM_ERR;
        }
    }

    // set the forward tables
    pFDcpt = ipv4ForwardDcpt;
    // keep the forwarding tables at the end of allocated descriptor
    pFwdTbl = (IPV4_ROUTE_TABLE_ENTRY*)(ipv4ForwardDcpt + nIfs);
    for(netIx = 0; netIx < nIfs; netIx++, pFDcpt++)
    {
        pFDcpt->totEntries = pIpInit->forwardTableMaxEntries;
        pFDcpt->iniFlags = pIpInit->forwardFlags;
        pFDcpt->fwdTable = pFwdTbl; 
        if((pFDcpt->iniFlags & TCPIP_IPV4_FWD_FLAG_ENABLED) != 0)
        {
            pFDcpt->runFlags = IPV4_FWD_FLAG_FWD_ENABLE;
        }
        if((pFDcpt->iniFlags & TCPIP_IPV4_FWD_FLAG_BCAST_ENABLED) != 0)
        {
            pFDcpt->runFlags |= IPV4_FWD_FLAG_BCAST_ENABLE;
        }
        if((pFDcpt->iniFlags & TCPIP_IPV4_FWD_FLAG_MCAST_ENABLED) != 0)
        {
            pFDcpt->runFlags |= IPV4_FWD_FLAG_MCAST_ENABLE;
        }
        pFwdTbl = pFwdTbl + pIpInit->forwardTableMaxEntries;  
    }


    if(pIpInit->forwardTable == 0)
    {
        usedEntries = 0;
    }
    else
    {
        usedEntries = pIpInit->forwardTableSize;
    }

    buildRes = TCPIP_IPV4_RES_OK;
    while(usedEntries != 0)
    {
        knownFmt = false;
#if (TCPIP_IPV4_FORWARDING_TABLE_BIN != 0)
        if((pIpInit->forwardFlags & TCPIP_IPV4_FWD_FLAG_ASCII_TABLE) == 0) 
        {
            buildRes = IPv4_BuildBinaryTable(ipv4ForwardDcpt, &pIpInit->forwardTable->entryBin, usedEntries);
            knownFmt = true;
        }
#endif

#if (TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0)
        if((pIpInit->forwardFlags & TCPIP_IPV4_FWD_FLAG_ASCII_TABLE) != 0) 
        {
            buildRes = IPv4_BuildAsciiTable(ipv4ForwardDcpt, &pIpInit->forwardTable->entryAscii, usedEntries);
            knownFmt = true;
        }
#endif
        if(knownFmt ==  false)
        {   // cannot do
            buildRes = TCPIP_IPV4_RES_FORMAT_ERR;
            break;
        }

        if(buildRes < 0)
        {
            break;
        }

        // sort the tables for proper operation
        pFDcpt = ipv4ForwardDcpt;
        for(netIx = 0; netIx < nIfs; netIx++, pFDcpt++)
        {
            if(pFDcpt->usedEntries)
            {
                IPv4_SortFwdTable(pFDcpt->fwdTable, pFDcpt->usedEntries);
            }
        }

        // build the tx pool
        if(ipv4ForwardNodes != 0)
        {   // if no bcast/mcast packets need to be forwarded AND processed internally
            // it's ok to have ipv4ForwardNodes == 0, so process only internally!
            pFwdNode = ipv4ForwardNodes;
            for(netIx = 0; netIx < pIpInit->forwardTxQueueSize; netIx++, pFwdNode++)
            {
                TCPIP_Helper_DoubleListTailAdd(&ipv4ForwardPool, (DBL_LIST_NODE*)pFwdNode); 
            } 
        }

        break;
    }

    return buildRes;
}

#if (TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0)
// builds the forward table from ASCII input
// table size needs to be > than nEntries!
static TCPIP_IPV4_RES IPv4_BuildAsciiTable(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pAEntry, size_t nEntries)
{
    int ix;
    TCPIP_NET_HANDLE inNetH, outNetH;
    IPV4_ADDR netAdd, netMask, gwAdd;
    TCPIP_UINT32_VAL onesCount;
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;
    IPV4_ROUTE_TABLE_ENTRY* pTable;

    for(ix = 0; ix < nEntries; ix++, pAEntry++)
    {
        if(pAEntry->inIfName == 0 || (inNetH = TCPIP_STACK_NetHandleGet(pAEntry->inIfName)) == 0)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }
        pFDcpt = pFwdDcpt + TCPIP_STACK_NetIxGet(_TCPIPStackHandleToNet(inNetH));

        if(pFDcpt->usedEntries == pFDcpt->totEntries)
        {   // cannot have this
            return TCPIP_IPV4_RES_ENTRIES_ERR;
        }

        if(pAEntry->netAddress == 0 || strlen(pAEntry->netAddress) == 0)
        {
            netAdd.Val = 0;
        }
        else if(!TCPIP_Helper_StringToIPAddress(pAEntry->netAddress, &netAdd))
        {
            return TCPIP_IPV4_RES_ADDRESS_ERR; 
        }

        if(pAEntry->netMask == 0 || strlen(pAEntry->netMask) == 0)
        {
            netMask.Val = 0;
        }
        else if(!TCPIP_Helper_StringToIPAddress(pAEntry->netMask, &netMask))
        {
            return TCPIP_IPV4_RES_ADDRESS_ERR; 
        }

        if(pAEntry->gwAddress == 0 || strlen(pAEntry->gwAddress) == 0)
        {
            gwAdd.Val = 0;
        }
        else if(!TCPIP_Helper_StringToIPAddress(pAEntry->gwAddress, &gwAdd))
        {
            return TCPIP_IPV4_RES_ADDRESS_ERR; 
        }

        if(pAEntry->outIfName == 0 || (outNetH = TCPIP_STACK_NetHandleGet(pAEntry->outIfName)) == 0)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }

        // check for the proper mask format
        onesCount = IPV4_32LeadingOnes(netMask.Val);
        if(onesCount.word.HW + onesCount.word.LW != 32)
        {   // ill formatted mask
            return TCPIP_IPV4_RES_MASK_ERR;
        }

        // OK
        pTable = pFDcpt->fwdTable + pFDcpt->usedEntries;
        pTable->mask = netMask.Val;
        pTable->netAdd = netAdd.Val;
        pTable->gwAdd = gwAdd.Val;
        pTable->outIfIx = TCPIP_STACK_NetIxGet(_TCPIPStackHandleToNet(outNetH));
        pTable->inIfIx = TCPIP_STACK_NetIxGet(_TCPIPStackHandleToNet(inNetH));
        pTable->metric = pAEntry->metric;
        pTable->nOnes = (uint8_t)onesCount.word.HW;

        pFDcpt->usedEntries++;
    }

    return TCPIP_IPV4_RES_OK;
}
#endif  // (TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0)

#if (TCPIP_IPV4_FORWARDING_TABLE_BIN != 0)
// builds the forward table from binary input
// table size needs to be > than nEntries!
static TCPIP_IPV4_RES IPv4_BuildBinaryTable(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry, size_t nEntries)
{
    int ix;
    TCPIP_NET_HANDLE netH;
    TCPIP_UINT32_VAL onesCount;
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;
    IPV4_ROUTE_TABLE_ENTRY* pTable;

    for(ix = 0; ix < nEntries; ix++, pBEntry++)
    {
        netH = TCPIP_STACK_IndexToNet(pBEntry->inIfIx);
        if(netH == 0)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }

        pFDcpt = pFwdDcpt + pBEntry->inIfIx;

        if(pFDcpt->usedEntries == pFDcpt->totEntries)
        {   // cannot have this
            return TCPIP_IPV4_RES_ENTRIES_ERR;
        }

        netH = TCPIP_STACK_IndexToNet(pBEntry->outIfIx);
        if(netH == 0)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }

        // check for the proper mask format
        onesCount = IPV4_32LeadingOnes(pBEntry->netMask);
        if(onesCount.word.HW + onesCount.word.LW != 32)
        {   // ill formatted mask
            return TCPIP_IPV4_RES_MASK_ERR;
        }

        // OK
        pTable = pFDcpt->fwdTable + pFDcpt->usedEntries;
        // TCPIP_IPV4_FORWARD_ENTRY_BIN == IPV4_ROUTE_TABLE_ENTRY 
        memcpy(pTable, pBEntry, sizeof(*pBEntry));
        pTable->nOnes = (uint8_t)onesCount.word.HW;

        pFDcpt->usedEntries++;
    }

    return TCPIP_IPV4_RES_OK;
}
#endif  // (TCPIP_IPV4_FORWARDING_TABLE_BIN != 0)

// sort the forwarding table by the largest number of leading ones in the network mask
// sorting function
static int _RouteEntryCompare(const void* p1, const void* p2)
{
    IPV4_ROUTE_TABLE_ENTRY* pE1 = (IPV4_ROUTE_TABLE_ENTRY*)p1;
    IPV4_ROUTE_TABLE_ENTRY* pE2 = (IPV4_ROUTE_TABLE_ENTRY*)p2;
   

    if(pE1->nOnes < pE2->nOnes)
    {
        return 1;
    }
    else if(pE1->nOnes > pE2->nOnes)
    {
        return -1;
    }

    // match; use the metric
    if(pE1->metric < pE2->metric)
    {
        return -1;
    }
    else if(pE1->metric > pE2->metric)
    {
        return 1;
    }

    // equal
    return 0;    
}


static void IPv4_SortFwdTable(IPV4_ROUTE_TABLE_ENTRY* pTable, size_t tableEntries)
{
    // sort the table ascending
    qsort(pTable, tableEntries, sizeof(*pTable), _RouteEntryCompare);
}

size_t TCPIP_IPV4_ForwadTableSizeGet(TCPIP_NET_HANDLE netH)
{
    if(ipv4ForwardDcpt != 0)
    {   
        TCPIP_NET_IF* pIf = _TCPIPStackHandleToNetUp(netH);
        if(pIf)
        {
            IPV4_FORWARD_DESCRIPTOR* pFDcpt = ipv4ForwardDcpt + _TCPIPStackNetIxGet(pIf);
            return pFDcpt->usedEntries; 
        }
    }

    // no forwarding tables or invalid interface
    return 0;
}

bool TCPIP_IPV4_ForwadTableEntryGet(TCPIP_NET_HANDLE netH, size_t index, TCPIP_IPV4_FORWARD_ENTRY_BIN* pFwdEntry)
{
    if(ipv4ForwardDcpt != 0)
    {   
        TCPIP_NET_IF* pIf = _TCPIPStackHandleToNetUp(netH);
        if(pIf)
        {
            IPV4_FORWARD_DESCRIPTOR* pFDcpt = ipv4ForwardDcpt + _TCPIPStackNetIxGet(pIf);
            if(index < pFDcpt->usedEntries)
            {
                if(pFwdEntry)
                {
                    IPV4_ROUTE_TABLE_ENTRY* pEntry = pFDcpt->fwdTable + index;
                    memcpy(pFwdEntry, pEntry, sizeof(*pFwdEntry));
                }

                return true;
            }
        }
    }

    // no forwarding tables, invalid interface or index
    return false; 
}

// function that adds a new packet to the forwarding queue
// protection needed because the IPv4 thread could be different from the 
// stack or MAC driver (the packet ack function can be called asynchronously)
static IPV4_FORWARD_NODE* TCPIP_IPV4_Forward_QueuePacket(TCPIP_MAC_PACKET* pFwdPkt, IPV4_PKT_PROC_TYPE procType)
{
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    IPV4_FORWARD_NODE* pFwdNode = (IPV4_FORWARD_NODE*)TCPIP_Helper_DoubleListHeadRemove(&ipv4ForwardPool);
    if(pFwdNode != 0)
    {  
        pFwdNode->pFwdPkt = pFwdPkt;
        pFwdNode->ownerAckFunc = pFwdPkt->ackFunc;
        pFwdNode->ownerAckParam = pFwdPkt->ackParam;
        pFwdPkt->ackFunc = TCPIP_IPV4_ForwardAckFunc;
        pFwdPkt->ackParam = pFwdNode;

        TCPIP_MAC_ETHERNET_HEADER* fwdMacHdr = (TCPIP_MAC_ETHERNET_HEADER*)pFwdPkt->pMacLayer;
        memcpy(pFwdNode->destMacAdd.v, fwdMacHdr->DestMACAddr.v, sizeof(pFwdNode->destMacAdd));
        memcpy(pFwdNode->sourceMacAdd.v, fwdMacHdr->SourceMACAddr.v, sizeof(pFwdNode->sourceMacAdd));

        pFwdPkt->ipv4PktData = (uint16_t)procType;  // set the 'forward first process later' flag
        TCPIP_Helper_DoubleListTailAdd(&ipv4ForwardQueue, (DBL_LIST_NODE*)pFwdNode);
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    return pFwdNode;
}

// de-queues a previously packet queued for forwarding
// checks that IPv4 forwarding is still alive
// returns true if alive and packet de-queued
static bool TCPIP_IPV4_Forward_DequeuePacket(IPV4_FORWARD_NODE* pFwdNode, bool aliveCheck)
{
    bool dequeueRes = false;
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv4ForwardNodes != 0 || aliveCheck == false)
    {
        TCPIP_Helper_DoubleListNodeRemove(&ipv4ForwardQueue, (DBL_LIST_NODE*)pFwdNode);
        // restore the packet
        TCPIP_MAC_PACKET* pFwdPkt = pFwdNode->pFwdPkt;
        pFwdPkt->ackFunc = pFwdNode->ownerAckFunc;
        pFwdPkt->ackParam = pFwdNode->ownerAckParam;

        TCPIP_MAC_ETHERNET_HEADER* fwdMacHdr = (TCPIP_MAC_ETHERNET_HEADER*)pFwdPkt->pMacLayer;
        memcpy(fwdMacHdr->DestMACAddr.v, pFwdNode->destMacAdd.v, sizeof(fwdMacHdr->DestMACAddr));
        memcpy(fwdMacHdr->SourceMACAddr.v, pFwdNode->sourceMacAdd.v, sizeof(fwdMacHdr->SourceMACAddr));

        TCPIP_Helper_DoubleListTailAdd(&ipv4ForwardPool, (DBL_LIST_NODE*)pFwdNode);
        dequeueRes = true;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    return dequeueRes;
}

// packet acknowledge function for a packet that's been forwarded
// and needs to be sent for internal processing
static bool TCPIP_IPV4_ForwardAckFunc(TCPIP_MAC_PACKET* pkt,  const void* param)
{
    bool isReinsert;

    IPV4_FORWARD_NODE* pFwdNode = (IPV4_FORWARD_NODE*)param;
    // simple sanity check
    _IPv4AssertCond(pFwdNode->pFwdPkt == pkt, __func__, __LINE__);
    // the 'forward first process later' flag should be set
    _IPv4AssertCond((pkt->ipv4PktData & (IPV4_PKT_DEST_FWD | IPV4_PKT_DEST_HOST)) == (IPV4_PKT_DEST_FWD | IPV4_PKT_DEST_HOST) , __func__, __LINE__);

    // check we're still up and running
    isReinsert = TCPIP_IPV4_Forward_DequeuePacket(pFwdNode, true);

    if(isReinsert)
    {   // re-insert for process...
        _TCPIPStackModuleRxInsert(TCPIP_MODULE_IPV4, pkt, true);
    }
    else
    {   // gone; simply acknowledge the packet
        TCPIP_PKT_PacketAcknowledge(pkt, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
    }

    return false;
}
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

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
TCPIP_NET_HANDLE TCPIP_IPV4_SelectSourceInterface(TCPIP_NET_HANDLE netH, const IPV4_ADDR* pDestAddress, IPV4_ADDR* pSrcAddress, bool srcSet)
{
    TCPIP_NET_IF  *pIf = _TCPIPStackHandleToNetUp(netH);

    if(srcSet)
    {   // address in pSrcAddress is valid, no matter what
        if(pIf == 0)
        {   // try to see if it belongs to some interface
            pIf = _TCPIPStackIpAddFromAnyNet(0, pSrcAddress);
            if(pIf == 0)
            {   // pSrcAddress is not the address of an interface!
                // use the destination address
                pIf = (TCPIP_NET_IF*)TCPIP_IPV4_SelectDestInterface(pDestAddress);
            }
        }
        return pIf;
    }

    if(pIf == 0)
    {   // use the destination address to decide which interface is this packet going out on
        pIf = (TCPIP_NET_IF*)TCPIP_IPV4_SelectDestInterface(pDestAddress);
    }
    // else interface is forced

    if(pIf)
    {
        pSrcAddress->Val = TCPIP_STACK_NetAddressGet(pIf);
    }
    return pIf;
}

TCPIP_NET_HANDLE TCPIP_IPV4_SelectDestInterface(const IPV4_ADDR* pDestAddress)
{
    int netIx;
    int avlblInterfaces;
    uint32_t    ifAdd, ifMask;
    TCPIP_NET_IF  *pIf;

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
                ifAdd = _TCPIPStackNetAddress(pIf);
                ifMask = _TCPIPStackNetMask(pIf);

                if((ifAdd & ifMask) == (pDestAddress->Val & ifMask))
                {   // destination address is on this interface
                    return pIf;
                }
            }
        }
    }

    // use the default/any interface
    return _TCPIPStackAnyNetLinked(true);
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
    return TCPIP_IPV4_PktTx(pPkt, &pPkt->macPkt, true);
}

bool TCPIP_IPV4_PktTx(IPV4_PACKET* pPkt, TCPIP_MAC_PACKET* pMacPkt, bool isPersistent)
{
    TCPIP_MAC_ADDR   destMacAdd, *pMacDst;
    TCPIP_NET_IF*    pNetIf, *pHostIf;
    TCPIP_IPV4_DEST_TYPE destType;
    IPV4_ADDR       arpTarget;
    void*           arpPkt;
    IPV4_ARP_PKT_TYPE arpType;
    uint16_t        pktPayload, linkMtu;
    bool            txRes;
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
    bool            isFragmented = false;
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)
    
    if(isPersistent)
    {
        arpType = IPV4_ARP_PKT_TYPE_TX;
        arpPkt = pPkt;
    }
    else 
    {
        arpType = IPV4_ARP_PKT_TYPE_MAC;
        arpPkt = pMacPkt; 
    }

    pNetIf = _TCPIPStackHandleToNet(pPkt->netIfH);
    if(pNetIf == 0)
    {
        return false;
    }

    // if the destination is internal, and need to change the network, solve it here
    pMacDst = &destMacAdd;
    if((pHostIf = TCPIP_STACK_MatchNetAddress(pNetIf, &pPkt->destAddress)))
    {   // localhost address
        memcpy(pMacDst, _TCPIPStackNetMACAddress(pHostIf), sizeof(*pMacDst));
        pPkt->netIfH = pHostIf;
        destType = TCPIP_IPV4_DEST_SELF; 
    }
    else
    {   // select packet's external destination MAC address
        destType = TCPIP_IPV4_PktMacDestination(pPkt, &pPkt->destAddress, &pMacDst, &arpTarget);
        if(destType == TCPIP_IPV4_DEST_FAIL) 
        {   // discard, cannot send
            return false;
        }
    }

    // check valid interface
    pNetIf = TCPIP_IPV4_CheckPktTx(pPkt->netIfH, pMacPkt);
    if(pNetIf == 0)
    {   // cannot transmit over invalid interface
        return false;
    }


    pMacPkt->pktIf = pNetIf;

    // properly format the packet
    TCPIP_PKT_PacketMACFormat(pMacPkt, pMacDst, (const TCPIP_MAC_ADDR*)_TCPIPStackNetMACAddress(pNetIf), TCPIP_ETHER_TYPE_IPV4);
    if(destType != TCPIP_IPV4_DEST_SELF)
    {   // get the payload w/o the MAC frame
        pktPayload = TCPIP_PKT_PayloadLen(pMacPkt) - sizeof(TCPIP_MAC_ETHERNET_HEADER);
        linkMtu = _TCPIPStackNetLinkMtu(pNetIf);
        if(pktPayload > linkMtu)
        {
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
            IPV4_HEADER* pHdr = (IPV4_HEADER*)pMacPkt->pNetLayer;
            if(pHdr->FragmentInfo.DF != 0 || !TCPIP_IPV4_FragmentTxPkt(pMacPkt, linkMtu, pktPayload))
            {   // no fragments or failed to build the fragments; out of memory
                return false;
            }
            else
            {
                isFragmented = true;
            }
#else
            // MAC transmit will fail anyway
            return false;
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)
        }
    }

    TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);

    if(destType == TCPIP_IPV4_DEST_SELF)
    {
        TCPIP_IPV4_FragmentTxInsertToRx(pNetIf, pMacPkt, TCPIP_MAC_PKT_FLAG_UNICAST, true);
        return true;
    }


    // TCPIP_IPV4_DEST_NETWORK/TCPIP_IPV4_DEST_GW 
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

        if(!TCPIP_IPV4_QueueArpPacket(arpPkt, _TCPIPStackNetIxGet(pNetIf), arpType, &arpTarget))
        {
            return false;
        }

        // ARP notification will be received: either TMO or resolved
        // mark packet as queued 
       pMacPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        return true;
    }

  
    // MAC sets itself the TCPIP_MAC_PKT_FLAG_QUEUED
    txRes = TCPIP_IPV4_TxMacPkt(pNetIf, pMacPkt);

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
    if(txRes == false && isFragmented)
    {   // ack only fragments created here
        TCPIP_IPV4_FragmentTxAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR, IPV4_FRAG_TX_ACK_FRAGS);
    }
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

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

// queues a packet waiting for ARP resolution
static bool TCPIP_IPV4_QueueArpPacket(void* pPkt, int arpIfIx, IPV4_ARP_PKT_TYPE type, IPV4_ADDR* arpTarget)
{
    PROTECTED_SINGLE_LIST* pList = &ipv4ArpQueue;
    TCPIP_Helper_ProtectedSingleListLock(pList);
    IPV4_ARP_ENTRY* pEntry = (IPV4_ARP_ENTRY*)TCPIP_Helper_SingleListHeadRemove(&ipv4ArpPool);
    if(pEntry == 0)
    {   // out of ARP entries in the pool
        SYS_ERROR(SYS_ERROR_WARNING, "IPv4: ARP entries pool empty!\r\n");
        TCPIP_Helper_ProtectedSingleListUnlock(pList);
        return false;
    }

    pEntry->type = (uint8_t)type;
    pEntry->arpIfIx = (uint8_t)arpIfIx;
    pEntry->pPkt = pPkt;     
    pEntry->arpTarget.Val = arpTarget->Val;
    TCPIP_Helper_SingleListTailAdd(&pList->list, (SGL_LIST_NODE*)pEntry);
    TCPIP_Helper_ProtectedSingleListUnlock(pList);
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    if(type == IPV4_ARP_PKT_TYPE_TX || type == IPV4_ARP_PKT_TYPE_MAC)
    {
        _ipv4_arp_stat.txSubmit++;
    }
    else if(type == IPV4_ARP_PKT_TYPE_FWD)
    {
        _ipv4_arp_stat.fwdSubmit++;
    }
    else
    {
        _IPv4AssertCond(false, __func__, __LINE__);
    }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    return true;
}



// ARP resolution done
static void TCPIP_IPV4_ArpHandler(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType, const void* param)
{
    TCPIP_NET_IF* pPktIf;
    SINGLE_LIST newList, *arpList;
    IPV4_ARP_ENTRY *pEntry;
    TCPIP_MAC_PACKET*   pMacPkt = 0;
    TCPIP_MAC_PKT_ACK_RES   pktAckFail;
    TCPIP_MAC_ETHERNET_HEADER* macHdr;
    

    TCPIP_Helper_SingleListInitialize (&newList);
    
    TCPIP_Helper_ProtectedSingleListLock(&ipv4ArpQueue);
    // traverse the ipv4ArpQueue list
    // and find all the packets waiting for the solved address

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    if(evType >= 0)
    {
        _ipv4_arp_stat.totSolved++;
    }
    else
    {
        _ipv4_arp_stat.totFailed++;
    }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    arpList = &ipv4ArpQueue.list;

    while((pEntry = (IPV4_ARP_ENTRY*)TCPIP_Helper_SingleListHeadRemove(arpList)) != 0)
    {
        if(pEntry->type == IPV4_ARP_PKT_TYPE_TX)
        {   // IPV4_PACKET*
            pMacPkt = &pEntry->pTxPkt->macPkt;
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
            if(evType >= 0)
            {
                _ipv4_arp_stat.txSolved++;
            }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
        }
        else if(pEntry->type == IPV4_ARP_PKT_TYPE_MAC)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pMacPkt;
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
            if(evType >= 0)
            {
                _ipv4_arp_stat.txSolved++;
            }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
        }
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        else if(pEntry->type == IPV4_ARP_PKT_TYPE_FWD)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pFwdPkt; 
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
            if(evType >= 0)
            {
                _ipv4_arp_stat.fwdSolved++;
            }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
        }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        else
        {   // should not happen!
            _IPv4AssertCond(false, __func__, __LINE__);
            return;
        }

        pPktIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(pEntry->arpIfIx);
            
        pktAckFail = TCPIP_MAC_PKT_ACK_NONE; 
        if(pEntry->arpTarget.Val == ipAdd->Val)
        {   // match
            if(evType >= 0)
            {   // successfully resolved the ARP; update the packet destination
                macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pMacPkt->pMacLayer;
                memcpy(&macHdr->DestMACAddr, MACAddr, sizeof(*MACAddr));
                pMacPkt->next = 0;  // send single packet
                if(pPktIf == 0 || !TCPIP_IPV4_TxMacPkt(pPktIf, pMacPkt))
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
                TCPIP_IPV4_FragmentTxAcknowledge(pMacPkt, pktAckFail, IPV4_FRAG_TX_ACK_HEAD | IPV4_FRAG_TX_ACK_FRAGS);
            }

            // back to pool
            TCPIP_Helper_SingleListTailAdd(&ipv4ArpPool, (SGL_LIST_NODE*)pEntry); 
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newList, (SGL_LIST_NODE*)pEntry);
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

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_IPV4_Timeout();
    }
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

}

// processes an incoming IPV4 packet
static void TCPIP_IPV4_Process(void)
{
    TCPIP_NET_IF* pNetIf;
    TCPIP_MAC_PACKET* pRxPkt;
    uint8_t      headerLen;
    uint16_t     headerChecksum, totalLength, payloadLen;
    IPV4_HEADER  *pHeader;
    IPV4_HEADER  cIpv4Hdr, *pCHeader;
    IPV4_PKT_PROC_TYPE procType;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    // extract queued IPv4 packets
    while((pRxPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        if(pRxPkt->ipv4PktData != 0)
        {   // re-visited packet; forwarded first; now processed
            pRxPkt->ipv4PktData = 0;
            ackRes = TCPIP_IPV4_DispatchPacket(pRxPkt);
            _IPv4AssertCond(ackRes == TCPIP_MAC_PKT_ACK_NONE, __func__, __LINE__);
            continue;
        }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

        // processing 1st time. log the packet
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
#if (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)
        if(ipv4PktHandler != 0)
        {
            bool was_processed = (*ipv4PktHandler)(pRxPkt->pktIf, pRxPkt, ipv4PktHandlerParam);
            if(was_processed)
            {
                TCPIP_PKT_FlightLogAcknowledge(pRxPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_EXTERN);
                continue;
            }
        }
#endif  // (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)

        while(true)
        {
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
            if(headerLen < sizeof(IPV4_HEADER) || (uint16_t)headerLen > pRxPkt->pDSeg->segLen)
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }
            totalLength = TCPIP_Helper_ntohs(pHeader->TotalLength);
            if(totalLength < (uint16_t)headerLen)
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }
            payloadLen = TCPIP_PKT_PayloadLen(pRxPkt);
            if(totalLength > payloadLen)
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // detect the proper alias interface
            pNetIf = _TCPIPStackMapAliasInterface((TCPIP_NET_IF*)pRxPkt->pktIf, &pHeader->DestAddress);
            pRxPkt->pktIf = pNetIf;

            if(!TCPIP_STACK_NetworkIsUp(pNetIf))
            {   // discard the packet
                ackRes = TCPIP_MAC_PKT_ACK_IP_REJECT_ERR;
                break;
            }

            // discard wrong source address
            if(_TCPIPStack_IsBcastAddress(pNetIf, &pHeader->SourceAddress))
            {   // net or limited bcast
                ackRes = TCPIP_MAC_PKT_ACK_SOURCE_ERR;
                break;
            }

            // discard wrong destination address
            if(pHeader->DestAddress.Val == 0)
            {   // invalid destination
                ackRes = TCPIP_MAC_PKT_ACK_DEST_ERR;
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

            // Make a copy of the header for the network to host conversion
            cIpv4Hdr = *pHeader;
            pCHeader = &cIpv4Hdr;
            pCHeader->TotalLength = totalLength;
            pCHeader->FragmentInfo.val = TCPIP_Helper_ntohs(pCHeader->FragmentInfo.val);

#if (_TCPIP_IPV4_FRAGMENTATION == 0)
            // Throw this packet away if it is a fragment.  
            // We don't support IPv4 fragment reconstruction.
            if(pCHeader->FragmentInfo.MF != 0 || pCHeader->FragmentInfo.fragOffset != 0)
            {   // discard the fragment
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }
#endif  // (_TCPIP_IPV4_FRAGMENTATION == 0)

            TCPIP_IPV4_CheckRxPkt(pRxPkt);

            // Check the packet arrived on the proper interface and passes the filters
            procType = TCPIP_IPV4_VerifyPkt(pNetIf, pCHeader, pRxPkt);

            if((procType & IPV4_PKT_DEST_HOST) == 0)
            {   // not processed internally; but some oter module may still need it; check the filters
                if(TCPIP_IPV4_VerifyPktFilters(pRxPkt, headerLen))
                {
                    procType = IPV4_PKT_DEST_HOST;
                }
            }

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
            if((procType & IPV4_PKT_DEST_FWD) != 0)
            {   // packet to be forwarded
                if(TCPIP_IPV4_ProcessExtPkt(pNetIf, pRxPkt, procType))
                {   // we're done
                    break;
                }
            }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

            if((procType & (IPV4_PKT_DEST_HOST)) == 0)
            {   // discard
                ackRes = TCPIP_MAC_PKT_ACK_IP_REJECT_ERR;
                break;
            }

            // valid IPv4 packet
            ackRes = TCPIP_IPV4_DispatchPacket(pRxPkt);
            break;
        }

        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {   // something wrong; discard
            TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes); 
        }
    }

}

// dispatch an IPv4 packet to its module
// packet is assumed to be valid!
// returns TCPIP_MAC_PKT_ACK_NONE if packet dispatched OK
// an error code  (< 0) otherwise 
static TCPIP_MAC_PKT_ACK_RES TCPIP_IPV4_DispatchPacket(TCPIP_MAC_PACKET* pRxPkt)
{
    IPV4_HEADER  *pHeader;
    bool        isFragment;
    TCPIP_STACK_MODULE destId;
    uint8_t      headerLen;

    pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;
    headerLen = pHeader->IHL << 2;

    // for internal processed packets, change to host order
    pRxPkt->pTransportLayer = pRxPkt->pNetLayer + headerLen;
    pRxPkt->pDSeg->segLen -= headerLen;
    pHeader->TotalLength = TCPIP_Helper_ntohs(pHeader->TotalLength);
    pRxPkt->totTransportLen = pHeader->TotalLength - headerLen;
    pHeader->FragmentInfo.val = TCPIP_Helper_ntohs(pHeader->FragmentInfo.val);

    isFragment = pHeader->FragmentInfo.MF != 0 || pHeader->FragmentInfo.fragOffset != 0;

    // check where it needs to go
    destId = TCPIP_IPV4_FrameDestination(pHeader);
    if(destId == TCPIP_MODULE_NONE)
    {
        return TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
    }

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
    pRxPkt->pkt_next = 0;       // make sure it's not linked
    if(isFragment)
    {
        IPV4_FRAGMENT_NODE *fragNode;
        TCPIP_MAC_PKT_ACK_RES ackRes = TCPIP_IPV4_RxFragmentInsert(pRxPkt, &fragNode);

        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {   // failed; discard
            return ackRes;
        }

        if(fragNode != 0)
        {
            pRxPkt = fragNode->fragHead; // this list is already ordered by pkt_next!
            isFragment = 0; // let it through
        }
    }
#else
    _IPv4AssertCond(isFragment == false, __func__, __LINE__);
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

    if(!isFragment)
    {   // forward this packet and signal
        _TCPIPStackModuleRxInsert(destId, pRxPkt, true);
    }

    return TCPIP_MAC_PKT_ACK_NONE;
} 

/*
 Checks packets that are received on the wrong interface
 (have an IP address that doesn't match the interface we
 received the packet on) or are rejected by the current IPv4 packet filters.
 Returns: IPV4_PKT_PROC_TYPE value containing:
    - type of packet
    - destination of packet: local or forwarded
    - if packet processing is enabled
*/                  
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
// packet verification when the forwarding is enabled
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPkt(TCPIP_NET_IF* pNetIf, IPV4_HEADER* pHeader, TCPIP_MAC_PACKET* pRxPkt)
{
    if(ipv4ForwardDcpt == 0)
    {   // no forwarding tables
        return TCPIP_IPV4_VerifyPktHost(pNetIf, pHeader, pRxPkt);
    }

    return TCPIP_IPV4_VerifyPktFwd(pNetIf, pHeader, pRxPkt);
}

static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktFwd(TCPIP_NET_IF* pNetIf, IPV4_HEADER* pHeader, TCPIP_MAC_PACKET* pRxPkt)
{
    // process first the registered filters
    IPV4_ADDR* pktDestIP, *pktSrcIP;
    TCPIP_IPV4_FILTER_TYPE currFilter; // current filter
    IPV4_PKT_PROC_TYPE procType;
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;
    bool    isMcast, isBcast;
    int ifIx = _TCPIPStackNetIxGet(pNetIf);
    TCPIP_MAC_ETHERNET_HEADER* macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pRxPkt->pMacLayer;

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
    TCPIP_IPV4_FORWARD_STAT* pFwdDbg = _ipv4_fwd_stat + (ifIx < 2 ? ifIx : 2);
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)

    while(true)
    {
        procType = IPV4_PKT_TYPE_UNK;
        isMcast = isBcast = false;
        pktDestIP = &pHeader->DestAddress;
        // standard IPv4 filtering
        currFilter = ipv4FilterType;

        // first few sanity checks
        if(_TCPIPStackIpAddFromLAN(pNetIf, pktDestIP))
        {   // check dest is not host 0
            if(_TCPIPStackHostPartAddress(pNetIf, pktDestIP) == 0)
            {   // invalid, discard
                break;
            }
        }

        pktSrcIP = &pHeader->SourceAddress;
        if(pktSrcIP->Val == 0 || pktSrcIP->v[0] == 0x7f)
        {   // RFC 1812: discard if the source address is: {0, 0} or { 127, <any> }
            // {0, host} not known yet!
            break;
        }

        // start the Local Delivery Decision (5.2.3)

        // check is for any of our interfaces
        if(TCPIP_STACK_MatchNetAddress(pNetIf, pktDestIP) != 0)
        {   // unicast to one of my networks
            procType = ((currFilter & TCPIP_IPV4_FILTER_UNICAST) == 0) ? (IPV4_PKT_DEST_HOST | IPV4_PKT_TYPE_UNICAST) : (IPV4_PKT_TYPE_UNICAST);
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            pFwdDbg->ucastPackets++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            break;
        }

        // check limited broadcast
        if(_TCPIPStack_IsLimitedBcast(pktDestIP))
        {   // limited bcast, never forwarded
            procType = ((currFilter & TCPIP_IPV4_FILTER_BROADCAST) == 0) ? (IPV4_PKT_DEST_HOST | IPV4_PKT_TYPE_BCAST) : (IPV4_PKT_TYPE_BCAST);
            break;
        }

        if((isMcast = TCPIP_Helper_IsMcastAddress(pktDestIP)))
        {   // multicast
            if(_TCPIPStack_IsLocalMcast(pktDestIP))
            {   // local, never forwarded
                procType = ((currFilter & TCPIP_IPV4_FILTER_MULTICAST) == 0) ? (IPV4_PKT_DEST_HOST | IPV4_PKT_TYPE_MCAST) : (IPV4_PKT_TYPE_MCAST);
            }
            else
            {   // can be forwarded
                procType = IPV4_PKT_TYPE_MCAST | IPV4_PKT_DEST_FWD;
                if((currFilter & TCPIP_IPV4_FILTER_MULTICAST) == 0)
                {
                    procType |= IPV4_PKT_DEST_HOST;
                }
            }
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            pFwdDbg->mcastPackets++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)

            break;
        }

        // check net-directed IP address
        TCPIP_NET_IF* pRIf = _TCPIPStackAnyNetDirected(pktDestIP);

        if(pRIf != 0)
        {   // net-directed for one of the router's interfaces
            isBcast = true;
            if(_TCPIPStackNetGetPrimary(pRIf) == _TCPIPStackNetGetPrimary(pNetIf))
            {   // net-directed to one of the receiving interfaces
                // packet delivered locally
                procType = ((currFilter & TCPIP_IPV4_FILTER_BROADCAST) == 0) ? (IPV4_PKT_DEST_HOST | IPV4_PKT_TYPE_BCAST) : (IPV4_PKT_TYPE_BCAST);
                if((macHdr->DestMACAddr.v[0] & 0x01) == 0)
                {   // link layer unicast; the packet should also be forwarded
                    procType |= IPV4_PKT_DEST_FWD;
                }
            }
            else
            {   // net-directed to another router interface, not the receiving one
                // packet delivered locally and forwarded
                procType = IPV4_PKT_TYPE_BCAST | IPV4_PKT_DEST_FWD;
                if((currFilter & TCPIP_IPV4_FILTER_BROADCAST) == 0)
                {
                    procType |= IPV4_PKT_DEST_HOST;
                }
            }

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            pFwdDbg->bcastPackets++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            break;
        }

        // some other type of packet
        // should be forwarded
        procType = IPV4_PKT_DEST_FWD;
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
            pFwdDbg->fwdPackets++;
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD) != 0)
        
        break;
    }

    // for local packets check that we actually handle it
    if((procType & IPV4_PKT_DEST_HOST) != 0 && (TCPIP_IPV4_FrameDestination(pHeader) == TCPIP_MODULE_NONE))
    {   // unknown; discard
        procType &= ~IPV4_PKT_DEST_HOST;
    }

    // few more rules to check for forwarding
    // 5.3.4
    while((procType & IPV4_PKT_DEST_FWD) != 0)
    {
        pFDcpt = ipv4ForwardDcpt + ifIx;
        if(isBcast)
        {   // check if we forward broadcast traffic
            if((pFDcpt->runFlags & IPV4_FWD_FLAG_BCAST_ENABLE) == 0)
            {
                procType &= ~IPV4_PKT_DEST_FWD;
                break;
            }
        }
        if(isMcast)
        {   // check if we forward multicast traffic
            if((pFDcpt->runFlags & IPV4_FWD_FLAG_MCAST_ENABLE) == 0)
            {
                procType &= ~IPV4_PKT_DEST_FWD;
                break;
            }
        }

        TCPIP_MAC_ADDR bcastAdd = { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };

        if(memcmp(macHdr->DestMACAddr.v, bcastAdd.v, sizeof(bcastAdd)) == 0)
        {   // link layer broadcast 
            if(!isMcast && !isBcast)
            {   // forward only if mcast or bcast
                procType &= ~IPV4_PKT_DEST_FWD;
            }
            break;
        }

        if((macHdr->DestMACAddr.v[0] & 0x01) != 0)
        {   // link layer multicast
            if(!isMcast)
            {   // no forward if it's not mcast
                procType &= ~IPV4_PKT_DEST_FWD;
            }
            break;
        }

        break;
    }

    return procType;
}
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE == 0)

// packet verification when the forwarding is disabled
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktHost(TCPIP_NET_IF* pNetIf, IPV4_HEADER* pHeader, TCPIP_MAC_PACKET* pRxPkt)
{
    // process first the registered filters
    IPV4_ADDR* pktDestIP;
    TCPIP_IPV4_FILTER_TYPE currFilter; // current filter
    IPV4_PKT_PROC_TYPE procType;

    while(true)
    {
        procType = IPV4_PKT_TYPE_UNK;
        pktDestIP = &pHeader->DestAddress;
        // standard IPv4 filtering
        currFilter = ipv4FilterType;

        if(_TCPIPStackIpAddFromLAN(pNetIf, pktDestIP))
        {   // check dest is not host 0
            if(_TCPIPStackHostPartAddress(pNetIf, pktDestIP) == 0)
            {   // invalid, discard
                break;
            }
        }

        // check is for the arriving interface
        if(TCPIP_STACK_AddressIsOfNet(pNetIf, pktDestIP))
        {   // unicast to me
            procType = ((currFilter & TCPIP_IPV4_FILTER_UNICAST) == 0) ? (IPV4_PKT_DEST_HOST | IPV4_PKT_TYPE_UNICAST) : (IPV4_PKT_TYPE_UNICAST);
            break;
        }

        if(_TCPIPStack_IsBcastAddress(pNetIf, pktDestIP))
        {   // net or limited bcast
            procType = ((currFilter & TCPIP_IPV4_FILTER_BROADCAST) == 0) ? (IPV4_PKT_DEST_HOST | IPV4_PKT_TYPE_BCAST) : (IPV4_PKT_TYPE_BCAST);
            break;
        }

        if(TCPIP_Helper_IsMcastAddress(pktDestIP))
        {   // multicast
            procType = ((currFilter & TCPIP_IPV4_FILTER_MULTICAST) == 0) ? (IPV4_PKT_DEST_HOST | IPV4_PKT_TYPE_MCAST) : (IPV4_PKT_TYPE_MCAST);
            break;
        }

        // some other type of packet
        
        break;
    }

    return procType;
}

static TCPIP_STACK_MODULE TCPIP_IPV4_FrameDestination(IPV4_HEADER* pHeader)
{
    TCPIP_STACK_MODULE destId;

    switch(pHeader->Protocol)
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
            // unknown/unsupported; 
            destId = TCPIP_MODULE_NONE; 
            break;
    }

    return destId;
}

static bool TCPIP_IPV4_VerifyPktFilters(TCPIP_MAC_PACKET* pRxPkt, uint8_t hdrlen)
{
    // process the registered filters
    IPV4_FILTER_LIST_NODE* fNode;
    bool pktOk = false;
    
    uint32_t cnt1, cnt2;

    // get a consistent reading
    do
    {
        cnt1 = ipv4ActFilterCount;
        cnt2 = ipv4ActFilterCount;
    }while(cnt1 != cnt2);


    _IPv4FiltCountDbg("verify");
    if(cnt1 != 0)
    {   // active filters
        TCPIP_Notification_Lock(&ipv4PacketFilters);
        for(fNode = (IPV4_FILTER_LIST_NODE*)ipv4PacketFilters.list.head; fNode != 0; fNode = fNode->next)
        {
            if(fNode->active != 0)
            {
                if((*fNode->handler)(pRxPkt, hdrlen))
                {   // packet accepted
                    pktOk = true;
                    break;
                }
            }
        }
        TCPIP_Notification_Unlock(&ipv4PacketFilters);
    }
        
    return pktOk;
}

IPV4_FILTER_HANDLE IPv4RegisterFilter(IPV4_FILTER_FUNC handler, bool active)
{
    IPV4_FILTER_LIST_NODE* newNode = 0;

    if(ipv4MemH && handler)
    {
        IPV4_FILTER_LIST_NODE filtNode;
        filtNode.handler = handler;
        filtNode.active = active;

        newNode = (IPV4_FILTER_LIST_NODE*)TCPIP_Notification_Add(&ipv4PacketFilters, ipv4MemH, &filtNode, sizeof(*newNode));
        if(newNode && active)
        {
            TCPIP_Notification_Lock(&ipv4PacketFilters);
            ipv4ActFilterCount++;
            TCPIP_Notification_Unlock(&ipv4PacketFilters);
        }
    }

    return newNode;
}

// deregister the filter handler
// returns true or false if no such handler registered
static void Ipv4DeRegisterCallback(SGL_LIST_NODE* node)
{
    IPV4_FILTER_LIST_NODE* filtNode = (IPV4_FILTER_LIST_NODE*)node;
    if(filtNode->active)
    {
        ipv4ActFilterCount--;
    }
}
    
// deregister the filter handler
// returns true or false if no such handler registered
bool Ipv4DeRegisterFilter(IPV4_FILTER_HANDLE hFilter)
{
    if(hFilter && ipv4MemH)
    {
        if(TCPIP_Notification_CbackRemove((SGL_LIST_NODE*)hFilter, &ipv4PacketFilters, ipv4MemH, Ipv4DeRegisterCallback))
        {   // was valid
            return true;
        }
    }

    return false;
}

// activates/de-activates an IPv4 filter
bool Ipv4FilterSetActive(IPV4_FILTER_HANDLE hFilter, bool active)
{
    IPV4_FILTER_LIST_NODE* fNode;
    bool activeOk = false;

    TCPIP_Notification_Lock(&ipv4PacketFilters);
    for(fNode = (IPV4_FILTER_LIST_NODE*)ipv4PacketFilters.list.head; fNode != 0; fNode = fNode->next)
    {
        if(fNode == (IPV4_FILTER_LIST_NODE*)hFilter)
        {
            if(fNode->active != active)
            {   // change
                if(active)
                {
                    ipv4ActFilterCount++;
                }
                else
                {
                    ipv4ActFilterCount--;
                }

                _IPv4FiltCountDbg("activate");
                fNode->active = active;
                activeOk = true;
            }
            break;
        }
    }
    TCPIP_Notification_Unlock(&ipv4PacketFilters);
        
    return activeOk;
}

// select destination MAC address
// for an external destination packet
// ppMacAdd is set to 0 if the MAC address is not available yet
static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_PktMacDestination(IPV4_PACKET* pPkt, const IPV4_ADDR* pIpAdd, TCPIP_MAC_ADDR** ppMacAdd, IPV4_ADDR* arpTarget)
{
    TCPIP_ARP_RESULT  arpRes;
    TCPIP_MAC_ADDR*   pMacDst = *ppMacAdd;
    TCPIP_NET_IF* pNetIf = (TCPIP_NET_IF*)pPkt->netIfH;

    if(pIpAdd->Val == 0xffffffff || pIpAdd->Val == TCPIP_STACK_NetAddressBcast(pNetIf))
    {
        memset(pMacDst, 0xff, sizeof(*pMacDst));
        return TCPIP_IPV4_DEST_NETWORK;
    }

    // check IP multicast address range from 224.0.0.0 to 239.255.255.255
    // can be done locally; No need for an ARP request.
    if ((pIpAdd->v[0] >= 224) && (pIpAdd->v[0] <= 239))
    {
        pMacDst->v[0] = 0x01;
        pMacDst->v[1] = 0x00;
        pMacDst->v[2] = 0x5E;
        pMacDst->v[3] = 0x7f & pIpAdd->v[1];
        pMacDst->v[4] = pIpAdd->v[2];
        pMacDst->v[5] = pIpAdd->v[3];
        return TCPIP_IPV4_DEST_NETWORK;
    }

    TCPIP_IPV4_DEST_TYPE destType;

    if(_TCPIPStackIpAddFromLAN(pNetIf, pIpAdd))
    {
        arpTarget->Val  = pIpAdd->Val;
        destType = TCPIP_IPV4_DEST_NETWORK;
    }
    else
    {   // not this LAN
        arpTarget->Val  = pNetIf->netGateway.Val;
        destType = TCPIP_IPV4_DEST_GW;
    }

    arpRes = TCPIP_ARP_EntryGet(pNetIf, arpTarget, pMacDst, true);
    if(arpRes == ARP_RES_ENTRY_SOLVED)
    {   // good to transmit
    }
    else if(arpRes == ARP_RES_ENTRY_QUEUED || arpRes == ARP_RES_ENTRY_NEW)
    {   // will have to be queued
        *ppMacAdd = 0;    // not known yet
    }
    else
    {   // discard, cannot send
        destType = TCPIP_IPV4_DEST_FAIL;
    }

    return destType;
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
// setChecksum:
//          - if true, the IPv4 header checksum is updated for the IPv4 header
// MAC header: if setMac == true, then:
//          - the destination addresses is set as the MAC packet source address
//          - the source address is the MAC address of the coresponding packet interface (which should be set!) 
//          - data segment is re-adjusted with the MAC header length
// TCPIP_MAC_PKT_FLAG_TX flag is set
void TCPIP_IPV4_MacPacketSwitchTxToRx(TCPIP_MAC_PACKET* pRxPkt, bool setChecksum, bool setMac)
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

    if(setMac)
    {   // set macHdr source and destination
        TCPIP_MAC_ETHERNET_HEADER* macHdr;
        macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pRxPkt->pMacLayer;

        memcpy(&macHdr->DestMACAddr, &macHdr->SourceMACAddr, sizeof(TCPIP_MAC_ADDR));
        memcpy(&macHdr->SourceMACAddr, _TCPIPStack_NetMACAddressGet((TCPIP_NET_IF*)pRxPkt->pktIf), sizeof(TCPIP_MAC_ADDR));

        pRxPkt->pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);
    }

    pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_TX; 
}

bool  TCPIP_IPV4_MacPacketTransmit(TCPIP_MAC_PACKET* pPkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR* pDestAddress)
{
    TCPIP_NET_IF* pNetIf, *pHostIf;
    TCPIP_IPV4_DEST_TYPE destType;

    pNetIf = _TCPIPStackHandleToNet(hNet);
    if(pNetIf == 0)
    {
        return false;
    }

    // if the destination is internal, and need to change the network, solve it here
    if((pHostIf = TCPIP_STACK_MatchNetAddress(pNetIf, pDestAddress)) != 0)
    {   // localhost address
        pNetIf = pHostIf;
        destType = TCPIP_IPV4_DEST_SELF; 
    }
    else
    {
        destType = TCPIP_IPV4_DEST_NETWORK;
    }

    // check valid interface
    pNetIf = TCPIP_IPV4_CheckPktTx(pNetIf, pPkt);
    if(pNetIf == 0)
    {   // cannot transmit over invalid interface
        return false;
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
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
    return true;
#else
    return false;
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)
}

// fragmentation functionality
//
#if (_TCPIP_IPV4_FRAGMENTATION != 0)

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
    uint16_t rxMin, rxMax, nextMin;   

    // minimal check 
    pRxHdr = (IPV4_HEADER*)pRxPkt->pNetLayer;
    rxMin = pRxHdr->FragmentInfo.fragOffset * 8;
    rxMax = rxMin + pRxPkt->totTransportLen;
    if(rxMax < rxMin)
    {   // overflow, fragOffset is too big?
        return TCPIP_MAC_PKT_ACK_FRAGMENT_ERR;
    } 

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
    uint16_t currMin, currMax;   
    bool fragOverlap = false;

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
                if(copyBytes != le)
                {
                    _IPv4AssertCond(false, __func__, __LINE__);
                }
                pDestSeg->segLen += le;
                pRxPkt->totTransportLen += le;
            }
            pNextPkt = pCurrPkt;    // reevaluate
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
static bool TCPIP_IPV4_FragmentTxPkt(TCPIP_MAC_PACKET* pMacPkt, uint16_t linkMtu, uint16_t pktPayload)
{
    int ix;
    IPV4_FRAG_TX_PKT    *pFragTx;
    TCPIP_MAC_PACKET    *pFragPkt, *pMacNext, *tail;
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    IPV4_HEADER*        pHdr;
    uint8_t*            pIpv4Load;
    IPV4_FRAGMENT_INFO  fragInfo;

    uint16_t pktHeaderSize = pMacPkt->pTransportLayer - pMacPkt->pMacLayer; 

    uint16_t ipv4HeaderSize = pMacPkt->pTransportLayer - pMacPkt->pNetLayer;
    uint16_t ipv4Payload = pktPayload - ipv4HeaderSize;

    uint16_t fragSize = (linkMtu - ipv4HeaderSize) & 0xfff8;  // fragments need to be multiple of 8
    int nFrags = (ipv4Payload + fragSize - 1) / fragSize;   // round up
    uint16_t lastFragSize = ipv4Payload - (nFrags - 1) * fragSize;

    if(nFrags > TCPIP_IPV4_FRAGMENT_MAX_NUMBER)
    {
        return false;
    }

    // allocate nFrags -1 packets; we already have the 1st
    pMacPkt->pkt_next = 0;
    tail = pMacPkt;
    for(ix = 1; ix < nFrags; ix++)
    {
        pFragTx = (IPV4_FRAG_TX_PKT*)TCPIP_PKT_PacketAlloc(sizeof(IPV4_FRAG_TX_PKT), ipv4HeaderSize, TCPIP_MAC_PKT_FLAG_IPV4 | TCPIP_MAC_PKT_FLAG_SPLIT | TCPIP_MAC_PKT_FLAG_TX);
        if(pFragTx != 0)
        {   
            TCPIP_PKT_PacketAcknowledgeSet(&pFragTx->macPkt, TCPIP_IPV4_FragmentTxAckFnc, 0);
            pFragTx->fragSeg.segFlags = TCPIP_MAC_SEG_FLAG_STATIC; // embedded in packet itself
            // copy the header part: MAC + IPv4
            memcpy(pFragTx->macPkt.pMacLayer, pMacPkt->pMacLayer, pktHeaderSize);
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
        for(pFragPkt = pMacPkt->pkt_next; pFragPkt != 0; pFragPkt = pMacNext)
        {   // deallocate
            pMacNext = pFragPkt->pkt_next;
            TCPIP_PKT_PacketFree(pFragPkt);
        } 

        return false;
    }


    // assemble the fragments
    pIpv4Load = pMacPkt->pTransportLayer; 
    for(ix = 0, pFragPkt = pMacPkt; ix < nFrags; ix++, pFragPkt = pFragPkt->pkt_next)
    {
        uint16_t currFragSize = ix == nFrags - 1 ? lastFragSize : fragSize;

        if(ix == 0)
        {   // this is the master/parent packet
            pSeg = pFragPkt->pDSeg;
            pSeg->segLen = pktHeaderSize + currFragSize;    // truncate size
        }
        else
        {
            pFragTx = (IPV4_FRAG_TX_PKT*)pFragPkt;
            pSeg = &pFragTx->fragSeg;
            pSeg->segLen = pSeg->segSize = currFragSize;
            pSeg->segLoad = pIpv4Load;
            pFragPkt->pDSeg->next = pSeg;  // link to the 1st data seg to be transmitted
        }

        // adjust the IPv4 header
        pHdr = (IPV4_HEADER*)pFragPkt->pNetLayer;
        pHdr->TotalLength = TCPIP_Helper_htons(ipv4HeaderSize + currFragSize);
        fragInfo.val = 0;
        fragInfo.fragOffset = (pIpv4Load - pMacPkt->pTransportLayer) / 8; 
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


#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

// external packet processing
#if (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)
TCPIP_IPV4_PROCESS_HANDLE TCPIP_IPV4_PacketHandlerRegister(TCPIP_IPV4_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_IPV4_PROCESS_HANDLE pHandle = 0;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv4PktHandler == 0)
    {
        ipv4PktHandlerParam = handlerParam;
        ipv4PktHandler = pktHandler;
        pHandle = pktHandler;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return pHandle;
}

bool TCPIP_IPV4_PacketHandlerDeregister(TCPIP_IPV4_PROCESS_HANDLE pktHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv4PktHandler == pktHandle)
    {
        ipv4PktHandler = 0;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // (TCPIP_IPV4_EXTERN_PACKET_PROCESS != 0)

#endif  // defined(TCPIP_STACK_USE_IPV4)


