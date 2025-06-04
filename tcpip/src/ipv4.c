/*******************************************************************************
  Internet Protocol (IP) Version 4 Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides a transport for TCP, UDP, and ICMP messages
    -Reference: RFC 791
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





#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_IPV4

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/ipv4_private.h"

#if defined(TCPIP_STACK_USE_IPV4)


// This is left shifted by 4.  Actual value is 0x04.
#define IPv4_VERSION        (0x04U)

#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
  #define IPV4_DEFAULT_TTL           (255U)  // Time-To-Live in hops 
  // IP TTL is set to 255 for Multicast DNS compatibility. See mDNS-draft-08, section 4.
#else
  #define IPV4_DEFAULT_TTL           (100U)  // Time-To-Live in hops
#endif



static uint16_t             ipv4Identifier = 0;           // Static identifier value for IPv4 headers
static const void*          ipv4MemH = NULL;                 // memory handle

static TCPIP_SIGNAL_HANDLE    signalHandle = NULL;

static PROTECTED_SINGLE_LIST ipv4ArpQueue = { {0} };    // queue of packets waiting for ARP resolution
static SINGLE_LIST          ipv4ArpPool = {0};          // pool of ARP entries
                                                        // access protected by ipv4ArpQueue!
static IPV4_ARP_ENTRY*      ipv4ArpEntries = NULL;         // allocated nodes for ipv4ArpPool 

static TCPIP_ARP_HANDLE     ipv4ArpHandle = NULL;       // ARP registration handle

static int                  ipv4InitCount = 0;

static PROTECTED_SINGLE_LIST ipv4PacketFilters = { {0} };
static volatile uint32_t    ipv4ActFilterCount = 0;    // counter of active filters
                                                        // access protected by ipv4PacketFilters 

static TCPIP_IPV4_FILTER_TYPE ipv4FilterType = TCPIP_IPV4_FILTER_NONE;       // IPv4 current filter

#if defined(TCPIP_IPV4_FRAGMENTATION) && (TCPIP_IPV4_FRAGMENTATION != 0)
#define M_TCPIP_IPV4_FRAGMENTATION    1
#else
#define M_TCPIP_IPV4_FRAGMENTATION    0
#endif

#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
static SINGLE_LIST          ipv4FragmentQueue = {0};  // IPv4 fragments to be processed
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

typedef enum
{
    TCPIP_IPV4_DEST_FAIL    = -1,   // cannot find a destination
    TCPIP_IPV4_DEST_SELF    = 0,    // route it internally
    TCPIP_IPV4_DEST_NETWORK = 1,    // send on the net segment
    TCPIP_IPV4_DEST_GW      = 2,    // pass it to the router

}TCPIP_IPV4_DEST_TYPE;

static TCPIP_IPV4_TX_PRI_HANDLER ipv4PriPktHandler = NULL;

#if (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)
static TCPIP_IPV4_PACKET_HANDLER ipv4PktHandler = NULL;
static const void* ipv4PktHandlerParam;
#endif  // (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)

#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
static IPV4_FORWARD_DESCRIPTOR* ipv4ForwardDcpt = NULL;    // the forward descriptor, for all interfaces
static size_t                   ipv4ForwardIfs = 0;    // allocated forward interfaces
static IPV4_FORWARD_NODE*       ipv4ForwardNodes;
static DOUBLE_LIST              ipv4ForwardPool;        // pool of Forward Node entries
static DOUBLE_LIST              ipv4ForwardQueue;       // list of forward nodes waiting to be transmitted
#if (M_TCPIP_IPV4_FWD_DYN_API != 0)
static OSAL_MUTEX_DECLARE(ipv4ForwardMux);
#endif  // (M_TCPIP_IPV4_FWD_DYN_API != 0)

#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

// conversion functions/helpers
//
static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_ArpEntry2SglNode(IPV4_ARP_ENTRY* pEntry)
{
    union
    {
        IPV4_ARP_ENTRY* pEntry;
        SGL_LIST_NODE*  node;
    }U_ARP_ENTRY_SGL_NODE;

    U_ARP_ENTRY_SGL_NODE.pEntry = pEntry;
    return U_ARP_ENTRY_SGL_NODE.node;
}

static __inline__ IPV4_ARP_ENTRY* __attribute__((always_inline)) FC_SglNode2ArpEntry(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        IPV4_ARP_ENTRY* pEntry;
    }U_SGL_NODE_ARP_ENTRY;

    U_SGL_NODE_ARP_ENTRY.node = node;
    return U_SGL_NODE_ARP_ENTRY.pEntry;
}


static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_FiltHndl2SglNode(IPV4_FILTER_HANDLE fH)
{
    union
    {
        IPV4_FILTER_HANDLE fH;
        SGL_LIST_NODE*  node;
    }U_FILT_HNDL_SGL_NODE;

    U_FILT_HNDL_SGL_NODE.fH = fH;
    return U_FILT_HNDL_SGL_NODE.node;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_FragNode2SglNode(IPV4_FRAGMENT_NODE* pFNode)
{
    union
    {
        IPV4_FRAGMENT_NODE* pFNode;
        SGL_LIST_NODE*  node;
    }U_FRAG_NODE_SGL_NODE;

    U_FRAG_NODE_SGL_NODE.pFNode = pFNode;
    return U_FRAG_NODE_SGL_NODE.node;
}

static __inline__ IPV4_FRAGMENT_NODE* __attribute__((always_inline)) FC_SglNode2FragNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        IPV4_FRAGMENT_NODE* pFNode;
    }U_SGL_NODE_FRAG_NODE;

    U_SGL_NODE_FRAG_NODE.node = node;
    return U_SGL_NODE_FRAG_NODE.pFNode;
}

static __inline__ IPV4_FILTER_LIST_NODE* __attribute__((always_inline)) FC_FiltHndl2FiltNode(IPV4_FILTER_HANDLE hFilter)
{
    union
    {
        IPV4_FILTER_HANDLE  hFilter;
        IPV4_FILTER_LIST_NODE* pFNode;
    }U_FILT_HNDL_FILT_NODE;

    U_FILT_HNDL_FILT_NODE.hFilter = hFilter;
    return U_FILT_HNDL_FILT_NODE.pFNode;
}

static __inline__ IPV4_FILTER_LIST_NODE* __attribute__((always_inline)) FC_SglNode2FiltNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        IPV4_FILTER_LIST_NODE* pFNode;
    }U_SGL_NODE_FILT_NODE;

    U_SGL_NODE_FILT_NODE.node = node;
    return U_SGL_NODE_FILT_NODE.pFNode;
}

static __inline__ IPV4_HEADER_BARE* __attribute__((always_inline)) FC_U8Ptr2HdrBare(uint8_t* u8Ptr)
{
    union
    {
        uint8_t*  u8Ptr;
        IPV4_HEADER_BARE* pHdr;
    }U_U8_PTR_HDR_BARE;

    U_U8_PTR_HDR_BARE.u8Ptr = u8Ptr;
    return U_U8_PTR_HDR_BARE.pHdr;
}

static __inline__ IPV4_HEADER_OPT* __attribute__((always_inline)) FC_U8Ptr2HdrOpt(uint8_t* u8Ptr)
{
    union
    {
        uint8_t*  u8Ptr;
        IPV4_HEADER_OPT* pHdrOpt;
    }U_U8_PTR_HDR_OPT;

    U_U8_PTR_HDR_OPT.u8Ptr = u8Ptr;
    return U_U8_PTR_HDR_OPT.pHdrOpt;
}

static __inline__ IPV4_HEADER_OPT* __attribute__((always_inline)) FC_HdrBare2HdrOpt(IPV4_HEADER_BARE* pBHdr)
{
    union
    {
        IPV4_HEADER_BARE* pBHdr;
        IPV4_HEADER_OPT*  pOptHdr;
    }U_HDR_BARE_OPT_HDR;

    U_HDR_BARE_OPT_HDR.pBHdr = pBHdr;
    return U_HDR_BARE_OPT_HDR.pOptHdr;
}

static __inline__ IPV4_PACKET* __attribute__((always_inline)) FC_MacPkt2Ip4Pkt(TCPIP_MAC_PACKET* pPkt)
{
    union
    {
        TCPIP_MAC_PACKET*  pPkt;
        IPV4_PACKET* pIp4Pkt;
    }U_MAC_PKT_IPV4_PKT;

    U_MAC_PKT_IPV4_PKT.pPkt = pPkt;
    return U_MAC_PKT_IPV4_PKT.pIp4Pkt;
}

static __inline__ IPV4_FRAG_TX_PKT* __attribute__((always_inline)) FC_MacPkt2FragPkt(TCPIP_MAC_PACKET* pPkt)
{
    union
    {
        TCPIP_MAC_PACKET*  pPkt;
        IPV4_FRAG_TX_PKT* pFragPkt;
    }U_MAC_PKT_FRAG_PKT;

    U_MAC_PKT_FRAG_PKT.pPkt = pPkt;
    return U_MAC_PKT_FRAG_PKT.pFragPkt;
}

static __inline__ IPV4_ROUTE_TABLE_ENTRY* __attribute__((always_inline)) FC_FwdDcpt2RteEntry(IPV4_FORWARD_DESCRIPTOR* pFDcpt)
{
    union
    {
        IPV4_FORWARD_DESCRIPTOR*  pFDcpt;
        IPV4_ROUTE_TABLE_ENTRY* pREntry;
    }U_FWD_DCPT_RTE_ENTRY;

    U_FWD_DCPT_RTE_ENTRY.pFDcpt = pFDcpt;
    return U_FWD_DCPT_RTE_ENTRY.pREntry;
}

static __inline__ IPV4_ROUTE_TABLE_ENTRY* __attribute__((always_inline)) FC_CvPtr2RteEntry(const void* cvPtr)
{
    union
    {
        const void* cvPtr;
        IPV4_ROUTE_TABLE_ENTRY* pREntry;
    }U_CV_PTR_RTE_ENTRY;

    U_CV_PTR_RTE_ENTRY.cvPtr = cvPtr;
    return U_CV_PTR_RTE_ENTRY.pREntry;
}

static __inline__ DBL_LIST_NODE* __attribute__((always_inline)) FC_FwdNode2DblNode(IPV4_FORWARD_NODE* pFwdNode)
{
    union
    {
        IPV4_FORWARD_NODE* pFwdNode;
        DBL_LIST_NODE*  node;
    }U_FWD_NODE_DBL_NODE;

    U_FWD_NODE_DBL_NODE.pFwdNode = pFwdNode;
    return U_FWD_NODE_DBL_NODE.node;
}

static __inline__ IPV4_FORWARD_NODE* __attribute__((always_inline)) FC_DblNode2FwdNode(DBL_LIST_NODE* node)
{
    union
    {
        DBL_LIST_NODE*  node;
        IPV4_FORWARD_NODE* pFwdNode;
    }U_FWD_NODE_DBL_NODE;

    U_FWD_NODE_DBL_NODE.node = node;
    return U_FWD_NODE_DBL_NODE.pFwdNode;
}

static __inline__ IPV4_FORWARD_NODE* __attribute__((always_inline)) FC_CvPtr2FwdNode(const void* cvPtr)
{
    union
    {
        const void* cvPtr;
        IPV4_FORWARD_NODE* pFwdNode;
    }U_CV_PTR_FWD_NODE;

    U_CV_PTR_FWD_NODE.cvPtr = cvPtr;
    return U_CV_PTR_FWD_NODE.pFwdNode;
}

static __inline__ IPV4_OPTION_FIELD* __attribute__((always_inline)) FC_U32Ptr2OptField(uint32_t* u32Ptr)
{
    union
    {
        uint32_t* u32Ptr;
        IPV4_OPTION_FIELD* pOptField;
    }U_U32_PTR_OPT_FIELD;

    U_U32_PTR_OPT_FIELD.u32Ptr = u32Ptr;
    return U_U32_PTR_OPT_FIELD.pOptField;
}

static __inline__ IPV4_OPTION_FIELD* __attribute__((always_inline)) FC_OptFieldInc(IPV4_OPTION_FIELD* pOptField, size_t incSize)
{
    union
    {
        uint8_t* u8Ptr;
        IPV4_OPTION_FIELD* pOptField;
    }U_U8_PTR_OPT_FIELD;

    U_U8_PTR_OPT_FIELD.pOptField = pOptField;
    U_U8_PTR_OPT_FIELD.u8Ptr += incSize;

    return U_U8_PTR_OPT_FIELD.pOptField;
}


static __inline__ IPV4_ROUTER_ALERT_OPTION* __attribute__((always_inline)) FC_U32Ptr2RtrAlert(uint32_t* u32Ptr)
{
    union
    {
        uint32_t* u32Ptr;
        IPV4_ROUTER_ALERT_OPTION* pRAlert;
    }U_U32_PTR_RTR_ALERT;

    U_U32_PTR_RTR_ALERT.u32Ptr = u32Ptr;
    return U_U32_PTR_RTR_ALERT.pRAlert;
}

static __inline__ TCPIP_IPV4_PROCESS_HANDLE __attribute__((always_inline)) FC_PktHndl2ProcHndl(TCPIP_IPV4_PACKET_HANDLER pktHndl)
{
    union
    {
        TCPIP_IPV4_PACKET_HANDLER pktHndl;
        TCPIP_IPV4_PROCESS_HANDLE procHndl;
    }U_PKT_HNDL_PROC_HNDL;

    U_PKT_HNDL_PROC_HNDL.pktHndl = pktHndl;
    return U_PKT_HNDL_PROC_HNDL.procHndl;
}



/************************************************************************/
/****************               Prototypes               ****************/
/************************************************************************/
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_BASIC) != 0)
static volatile int V_IPv4StayAssertLoop = 0;
static void F_IPv4AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("IPv4 Assert: %s, in line: %d, \r\n", message, lineNo);
        while (true)
        {
            if(V_IPv4StayAssertLoop == 0)
            {
                break;
            }
        }
    }
}

#else
#define F_IPv4AssertCond(cond, message, lineNo)
#endif  // (TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_BASIC)

#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
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

static void F_IPv4FragmentDbg(IPV4_FRAGMENT_NODE* pFragNode, TCPIP_MAC_PACKET* pFragPkt, TCPIP_IPV4_FRAG_EVENT_TYPE evType)
{
    IPV4_HEADER_BARE *pFHdr;
    uint16_t    fragId;
    uint16_t    segMin, segMax;
    uint32_t    fragTmo;

    if(pFragPkt != NULL)
    {
        pFHdr = FC_U8Ptr2HdrBare(pFragPkt->pNetLayer);
        segMin = pFHdr->FragmentInfo.fragOffset * 8U;
        segMax = segMin + pFragPkt->totTransportLen;
    }
    else
    {
        segMin = segMax = 0;
        pFHdr = FC_U8Ptr2HdrBare(pFragNode->fragHead->pNetLayer);
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
#define F_IPv4FragmentDbg(pFrag, pFragPkt, evType)
#endif  // (TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FRAGMENT)
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_RX_CHECK) != 0)
static uint16_t checkRxUdpSrcPort = 67U; 
static uint16_t checkRxUdpDstPort = 68U; 

static uint16_t checkRxTcpSrcPort = 9760U; 
static uint16_t checkRxTcpDstPort = 9760U; 

static uint32_t checkRxUdpBkptCnt = 0U;
static uint32_t checkRxIcmpBkptCnt = 0U;
static uint32_t checkRxTcpBkptCnt = 0U;
static void TCPIP_IPV4_CheckRxPkt(TCPIP_MAC_PACKET* pRxPkt)
{
    IPV4_HEADER_BARE* pHeader = FC_U8Ptr2HdrBare(pRxPkt->pNetLayer);
    if(pHeader->Protocol == (uint8_t)IP_PROT_ICMP)
    {
        checkRxIcmpBkptCnt++;
    }
    else if(pHeader->Protocol == (uint8_t)IP_PROT_UDP)
    {   // UDP packet
        uint8_t headerLen = pHeader->IHL << 2U;
        UDP_HEADER* pUDPHdr = FC_U8Ptr2UdpHdr(pRxPkt->pNetLayer + headerLen);
        UDP_PORT destPort = TCPIP_Helper_ntohs(pUDPHdr->DestinationPort);
        UDP_PORT srcPort = TCPIP_Helper_ntohs(pUDPHdr->SourcePort);
        if(srcPort == checkRxUdpSrcPort || destPort == checkRxUdpDstPort)
        {
            checkRxUdpBkptCnt++;
        }
    }
    else if(pHeader->Protocol == (uint8_t)IP_PROT_TCP)
    {   
        TCP_HEADER* pTcpHdr = FC_U8Ptr2TcpHdr(pRxPkt->pNetLayer + sizeof(TCP_HEADER));
        TCP_PORT destPort = TCPIP_Helper_ntohs(pTcpHdr->DestPort);
        TCP_PORT srcPort = TCPIP_Helper_ntohs(pTcpHdr->SourcePort);
        if(srcPort == checkRxTcpSrcPort || destPort == checkRxTcpDstPort)
        {
            checkRxTcpBkptCnt++;
        }
    }
    else
    {
        // do nothing
    }

}
#else
#define TCPIP_IPV4_CheckRxPkt(pRxPkt)
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_RX_CHECK) != 0)


#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD_MAC_DEST) != 0)
static uint32_t t_ipv4DbgDestTbl[] = {0xf502a8c0U, 0x9301a8c0U};   // table with destinations to be matched against
static uint32_t* p_ipv4DbgDest = t_ipv4DbgDestTbl;

static void F_IPv4FwdMacDestDebug(const IPV4_ADDR* pDestAdd, const IPV4_ADDR* arpTarget, const TCPIP_NET_IF* pNetIf, TCPIP_IPV4_DEST_TYPE destType, bool solved)
{
    size_t ix;
    char destBuff[20] = {'\0'};
    char targetBuff[20] = {'\0'};

    (void)TCPIP_Helper_IPAddressToString(pDestAdd, destBuff, sizeof(destBuff));
    (void)TCPIP_Helper_IPAddressToString(arpTarget, targetBuff, sizeof(targetBuff));

    bool doPrint = false;

    if(p_ipv4DbgDest != NULL)
    {   // search for a matching destination address
        for(ix = 0; ix < sizeof(t_ipv4DbgDestTbl) / sizeof(*t_ipv4DbgDestTbl); ix++)
        {
            if(pDestAdd->Val == t_ipv4DbgDestTbl[ix])
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
#define F_IPv4FwdMacDestDebug(pDestAdd, arpTarget, pNetIf, destType, solved)
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FWD_MAC_DEST) != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)

static TCPIP_IPV4_ARP_QUEUE_STAT s_ipv4_arp_stat = {0};

bool TCPIP_IPv4_ArpStatGet(TCPIP_IPV4_ARP_QUEUE_STAT* pStat, bool clear)
{
    if(pStat != NULL)
    {
        s_ipv4_arp_stat.nPool = TCPIP_Helper_SingleListCount(&ipv4ArpPool); 
        s_ipv4_arp_stat.nPend = TCPIP_Helper_SingleListCount(&ipv4ArpQueue.list); 

        *pStat = s_ipv4_arp_stat;
    }
    if(clear)
    {
        (void)memset(&s_ipv4_arp_stat, 0, sizeof(s_ipv4_arp_stat));
    }
    return true;
}

#else
bool TCPIP_IPv4_ArpStatGet(TCPIP_IPV4_ARP_QUEUE_STAT* pStat, bool clear)
{
    return false;
}
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)

#if (M_TCPIP_IPV4_FWD_ENABLE != 0) && ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_PROC_EXT) != 0)

static uint32_t t_ipv4DbgExtTbl[] = {0xf002a8c0U};   // table with addresses (src, dest) to be matched against
static uint32_t* p_ipv4DbgExt = t_ipv4DbgExtTbl;

static void F_IPv4ProcessExtPktDbg(TCPIP_MAC_PACKET* pRxPkt)
{
    size_t ix;
    char destBuff[20] = {'\0'};
    char srcBuff[20] = {'\0'};

    const TCPIP_NET_IF* pInIf = (const TCPIP_NET_IF*)pRxPkt->pktIf;
    const IPV4_ADDR* dstAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);
    const IPV4_ADDR* srcAdd = TCPIP_IPV4_PacketGetSourceAddress(pRxPkt);
    (void)TCPIP_Helper_IPAddressToString(dstAdd, destBuff, sizeof(destBuff));
    (void)TCPIP_Helper_IPAddressToString(srcAdd, srcBuff, sizeof(srcBuff));

    bool doPrint = false;
    if(p_ipv4DbgExt != NULL)
    {
        for(ix = 0; ix < sizeof(t_ipv4DbgExtTbl) / sizeof(*t_ipv4DbgExtTbl); ix++)
        {
            if(t_ipv4DbgExtTbl[ix] == dstAdd->Val || t_ipv4DbgExtTbl[ix] == srcAdd->Val)
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
#define F_IPv4ProcessExtPktDbg(pRxPkt)
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0) && ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_PROC_EXT) != 0)

#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
static TCPIP_IPV4_FORWARD_STAT s_ipv4_fwd_stat[3] = {{0}};  // FWD stats on if 0, 1, all else

bool TCPIP_IPv4_ForwardStatGet(size_t index, TCPIP_IPV4_FORWARD_STAT* pStat, bool clear)
{
    if(index < sizeof(s_ipv4_fwd_stat) / sizeof*(s_ipv4_fwd_stat))
    {
        TCPIP_IPV4_FORWARD_STAT* pFwdStat = s_ipv4_fwd_stat + index;

        if(pStat != NULL)
        {
            *pStat = *pFwdStat;
        }

        if(clear)
        {
            (void)memset(pFwdStat, 0, sizeof(*pFwdStat));
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
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FILT_COUNT) != 0)
static uint32_t ipv4PrevFCount = 0xffffffffU;

static void F_IPv4FiltCountDbg(const char* msg)
{
    if(ipv4ActFilterCount != ipv4PrevFCount)
    {
        ipv4PrevFCount = ipv4ActFilterCount;
        SYS_CONSOLE_PRINT("IPv4 Filt: %s - count: %d\r\n", msg, ipv4PrevFCount);
    }
}
#else
#define F_IPv4FiltCountDbg(msg)
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FILT_COUNT) != 0)

static bool TCPIP_IPV4_QueueArpPacket(void* pPkt, uint16_t arpIfIx, IPV4_ARP_PKT_TYPE type, IPV4_ADDR* arpTarget);

static void TCPIP_IPV4_ArpHandler(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE arpEvType, const void* param);

static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktHost(const TCPIP_NET_IF* pNetIf, IPV4_HEADER_BARE* pHeader, TCPIP_MAC_PACKET* pRxPkt);

static TCPIP_NET_IF* TCPIP_IPV4_CheckPktTx(TCPIP_NET_HANDLE hNet, TCPIP_MAC_PACKET* pPkt);

static bool TCPIP_IPV4_VerifyPktFilters(TCPIP_MAC_PACKET* pRxPkt, uint8_t hdrlen);

static TCPIP_STACK_MODULE TCPIP_IPV4_FrameDestination(IPV4_HEADER_BARE* pHeader);

#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
static void TCPIP_IPV4_Timeout(void);

// RX fragmentation
static TCPIP_MAC_PKT_ACK_RES    TCPIP_IPV4_RxFragmentInsert(TCPIP_MAC_PACKET* pRxPkt, IPV4_FRAGMENT_NODE **ppFrag);
static void                     TCPIP_IPV4_RxFragmentDiscard(IPV4_FRAGMENT_NODE* pFrag, TCPIP_MAC_PKT_ACK_RES ackRes);
static void                     TCPIP_IPV4_RxFragmentListPurge(SINGLE_LIST* pL);


// TX fragmentation
static bool TCPIP_IPV4_FragmentTxPkt(TCPIP_MAC_PACKET* pMacPkt, uint16_t linkMtu, uint16_t pktPayload);
static void TCPIP_IPV4_FragmentTxAckFnc(TCPIP_MAC_PACKET* pkt,  const void* param);
static void TCPIP_IPV4_FragmentTxAcknowledge(TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PKT_ACK_RES ackRes, uint8_t txAck);
static void TCPIP_IPV4_FragmentTxInsertToRx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PACKET_FLAGS flags, bool doSignal);
static bool TCPIP_IPV4_TxMacPkt(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pPkt);
#else
// if no fragment support, transmit just the head
static __inline__ bool __attribute__((always_inline)) TCPIP_IPV4_TxMacPkt(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pPkt)
{
    return (int)TCPIPStackPacketTx(pNetIf, pPkt) >= 0;
}

// if no fragment support, acknowledge just the head
static __inline__ void __attribute__((always_inline)) TCPIP_IPV4_FragmentTxAcknowledge(TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PKT_ACK_RES ackRes, uint8_t txAck)
{
    if((txAck & (uint8_t)IPV4_FRAG_TX_ACK_HEAD) != 0U)
    {
        TCPIP_PKT_PacketAcknowledge(pTxPkt, ackRes);
    }
}
// if no fragment support, insert just the head
static __inline__ void __attribute__((always_inline)) TCPIP_IPV4_FragmentTxInsertToRx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PACKET_FLAGS flags, bool doSignal)
{
    pTxPkt->pktFlags |= (uint32_t)flags;
    TCPIPStackInsertRxPacket(pNetIf, pTxPkt, doSignal);
}
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_PktMacDestination(IPV4_PACKET* pPkt, const IPV4_ADDR* pIpAdd, TCPIP_MAC_ADDR** ppMacAdd, IPV4_ADDR* arpTarget);

static void TCPIP_IPV4_Process(void);

static TCPIP_MAC_PKT_ACK_RES TCPIP_IPV4_DispatchPacket(TCPIP_MAC_PACKET* pRxPkt);

static IPV4_OPTION_FIELD* F_IPv4CheckPacketOption(TCPIP_MAC_PACKET* pRxPkt, size_t* pOptLen);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_IPV4_Cleanup(void);
static void TCPIP_IPV4_ArpListPurge(TCPIP_NET_IF* pNetIf);
#else
#define TCPIP_IPV4_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
static TCPIP_IPV4_RES IPV4_BuildForwardTables(const TCPIP_IPV4_MODULE_CONFIG* pIpInit, const void* memH, size_t nIfs);
static TCPIP_IPV4_DEST_TYPE TCPIP_IPV4_FwdPktMacDestination(TCPIP_MAC_PACKET* pFwdPkt, const IPV4_ROUTE_TABLE_ENTRY* pEntry, TCPIP_MAC_ADDR** ppMacAdd, IPV4_ADDR* arpTarget);
static bool TCPIP_IPV4_ForwardPkt(TCPIP_MAC_PACKET* pFwdPkt, const IPV4_ROUTE_TABLE_ENTRY* pEntry, IPV4_PKT_PROC_TYPE procType);
static bool TCPIP_IPV4_ProcessExtPkt(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV4_PKT_PROC_TYPE procType);
static const IPV4_ROUTE_TABLE_ENTRY* TCPIP_IPV4_FindFwdRoute(IPV4_FORWARD_DESCRIPTOR* pFDcpt, TCPIP_MAC_PACKET* pRxPkt);
static uint32_t IPV4_32TrailZeros(uint32_t v);
static uint32_t IPV4_32LeadingZeros(uint32_t v);

#if (M_TCPIP_IPV4_FWD_TABLE_ASCII != 0)
static TCPIP_IPV4_RES IPv4_BuildAsciiTable(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY* fwdTbl, size_t nEntries);
static TCPIP_IPV4_RES IPv4_AsciiToBinEntry(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pAEntry, TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry, size_t nEntries);

#if (M_TCPIP_IPV4_FWD_DYN_API != 0)
static TCPIP_IPV4_RES IPv4_ProcessAsciiEntries(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pArrEntry, size_t nEntries, bool isRemove);
#endif  // (M_TCPIP_IPV4_FWD_DYN_API != 0)
#endif  // (M_TCPIP_IPV4_FWD_TABLE_ASCII != 0)

#if (M_TCPIP_IPV4_FWD_DYN_API != 0)
static TCPIP_IPV4_RES IPv4_DynFwdTblOper(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pArrEntry, size_t nEntries, bool isRemove);
static TCPIP_IPV4_RES IPv4_RemoveBinaryTableEntry(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry);
#endif  // (M_TCPIP_IPV4_FWD_DYN_API != 0)    

static TCPIP_IPV4_RES IPv4_BuildBinaryTable(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY* fwdTbl, size_t nEntries);
static TCPIP_IPV4_RES IPv4_AddBinaryTableEntry(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry);

static void IPv4_SortFwdTable(IPV4_ROUTE_TABLE_ENTRY* pTable, size_t tableEntries);
static IPV4_FORWARD_NODE* TCPIP_IPV4_Forward_QueuePacket(TCPIP_MAC_PACKET* pFwdPkt, IPV4_PKT_PROC_TYPE procType);
static bool TCPIP_IPV4_Forward_DequeuePacket(IPV4_FORWARD_NODE* pFwdNode, bool aliveCheck);
static void TCPIP_IPV4_ForwardAckFunc(TCPIP_MAC_PACKET* pkt,  const void* param);
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktFwd(const TCPIP_NET_IF* pNetIf, IPV4_HEADER_BARE* pHeader, TCPIP_MAC_PACKET* pRxPkt);
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPkt(const TCPIP_NET_IF* pNetIf, IPV4_HEADER_BARE* pHeader, TCPIP_MAC_PACKET* pRxPkt);
#else

#define TCPIP_IPV4_VerifyPkt(pNetIf, pHeader, pRxPkt)   TCPIP_IPV4_VerifyPktHost(pNetIf, pHeader, pRxPkt)  

#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

static uint8_t TCPIP_IPV4_TxPriQueue(const TCPIP_NET_IF* pNetIf, uint8_t precedence);

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
bool TCPIP_IPV4_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const void* initData)
{
    size_t ix;
    TCPIP_IPV4_RES iniRes;
    IPV4_ARP_ENTRY* pEntry;

    if(stackInit->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    if(ipv4InitCount == 0)
    {
        while(true)
        {
            iniRes = TCPIP_IPV4_RES_OK;
            ipv4MemH = stackInit->memH;
            ipv4ArpHandle = NULL;
            signalHandle = NULL;
            (void)memset(&ipv4ArpQueue, 0, sizeof(ipv4ArpQueue));
            (void)memset(&ipv4ArpPool, 0, sizeof(ipv4ArpPool));
            ipv4ArpEntries = NULL;
            (void)memset(&ipv4PacketFilters, 0, sizeof(ipv4PacketFilters));
            ipv4ActFilterCount = 0;
#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
            ipv4ForwardDcpt = NULL;
            ipv4ForwardIfs = 0;
            ipv4ForwardNodes = NULL;
            TCPIP_Helper_DoubleListInitialize(&ipv4ForwardPool); 
            TCPIP_Helper_DoubleListInitialize(&ipv4ForwardQueue); 
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

            // check initialization data is provided and minimal sanity check
            const TCPIP_IPV4_MODULE_CONFIG* pIpInit = (const TCPIP_IPV4_MODULE_CONFIG*)initData;
            if(pIpInit == NULL || pIpInit->arpEntries == 0U)
            {
                iniRes = TCPIP_IPV4_RES_INIT_VAL_ERR;
                break;
            }
#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
            TCPIP_Helper_SingleListInitialize(&ipv4FragmentQueue);
            signalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_IPV4_Task, TCPIP_IPV4_TASK_TICK_RATE);
#else
            signalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_IPV4_Task, 0);
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)
            if(signalHandle == NULL)
            {
                iniRes = TCPIP_IPV4_RES_SIGNAL_ERR;
                break;
            }

            if(!TCPIP_Helper_ProtSglListInitialize (&ipv4ArpQueue))
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
            if(ipv4ArpEntries == NULL)
            {   // allocation failed
                iniRes = TCPIP_IPV4_RES_MEM_ERR;
                break;
            }
            // build the ARP pool
            TCPIP_Helper_SingleListInitialize(&ipv4ArpPool);
            pEntry = ipv4ArpEntries;
            for(ix = 0; ix < pIpInit->arpEntries; ix++)
            {
                TCPIP_Helper_SingleListTailAdd(&ipv4ArpPool, FC_ArpEntry2SglNode(pEntry)); 
                pEntry++;
            }

            ipv4PriPktHandler = NULL;

#if (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)
            ipv4PktHandler = NULL;
#endif  // (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)

#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
            if(stackInit->nIfs > 1U)
            {   // forwarding enabled only for multiple interfaces
                if(pIpInit->forwardTableMaxEntries == 0U)
                {
                    iniRes = TCPIP_IPV4_RES_INIT_VAL_ERR;
                    break;
                }

                // build the forwarding table
                iniRes = IPV4_BuildForwardTables(pIpInit, ipv4MemH, stackInit->nIfs);
                if((int)iniRes < 0)
                {   // failed
                    break;
                }

#if (M_TCPIP_IPV4_FWD_DYN_API != 0)
                if(OSAL_MUTEX_Create(&ipv4ForwardMux) != OSAL_RESULT_SUCCESS)
                {
                    iniRes = TCPIP_IPV4_RES_FWD_LOCK_ERR;
                    break;
                }
#endif  // (M_TCPIP_IPV4_FWD_DYN_API != 0)
            }
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

            break;
        }
        
        if((int)iniRes < 0)
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
    void TCPIP_IPV4_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

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
void TCPIP_IPV4_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(ipv4InitCount > 0)
    {   // up and running
        // one way or another this interface is going down
#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
        TCPIP_IPV4_RxFragmentListPurge(&ipv4FragmentQueue);
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

        TCPIP_IPV4_ArpListPurge(stackCtrl->pNetIf);

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
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
    if(ipv4ArpHandle != NULL)
    {
        (void)TCPIP_ARP_HandlerDeRegister(ipv4ArpHandle);
        ipv4ArpHandle = NULL;
    }

    TCPIP_Notification_Deinitialize(&ipv4PacketFilters, ipv4MemH);
    ipv4ActFilterCount = 0;

    TCPIP_Helper_ProtSglListDeinitialize(&ipv4ArpQueue);

    if(signalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(signalHandle);
        signalHandle = NULL;
    }

    if(ipv4ArpEntries != NULL)
    {
        (void)TCPIP_HEAP_Free(ipv4MemH, ipv4ArpEntries);
        ipv4ArpEntries = NULL;
    }

#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
    if(ipv4ForwardDcpt != NULL)
    {
        (void)TCPIP_HEAP_Free(ipv4MemH, ipv4ForwardDcpt);
        ipv4ForwardDcpt = NULL;
    }

    if(ipv4ForwardNodes != NULL)
    {
        (void)TCPIP_HEAP_Free(ipv4MemH, ipv4ForwardNodes);
        ipv4ForwardNodes = NULL;
    }
    ipv4ForwardIfs = 0;
#if (M_TCPIP_IPV4_FWD_DYN_API != 0)
    (void)OSAL_MUTEX_Delete(&ipv4ForwardMux);
#endif  // (M_TCPIP_IPV4_FWD_DYN_API != 0)
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

    ipv4MemH = NULL;
}

// purges the ARP pending list of IPV4_PACKET packets
// searches for pNetIf match
// if pNetIf == 0 everything matches
static void TCPIP_IPV4_ArpListPurge(TCPIP_NET_IF* pNetIf)
{
    SINGLE_LIST         newList;
    IPV4_ARP_ENTRY*     pEntry;
    TCPIP_MAC_PACKET*   pMacPkt;
    const TCPIP_NET_IF* pPktIf;
    

    TCPIP_Helper_SingleListInitialize (&newList);

    PROTECTED_SINGLE_LIST* pList = &ipv4ArpQueue;
    (void)TCPIP_Helper_ProtSglListLock(pList);
    // traverse the list
    // and find all the packets matching the pNetIf

    while((pEntry = FC_SglNode2ArpEntry(TCPIP_Helper_SingleListHeadRemove(&pList->list))) != NULL)
    {
        if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_TX)
        {   // IPV4_PACKET*
            pMacPkt = &pEntry->pTxPkt->macPkt;
        }
        else if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_MAC)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pMacPkt; 
        }
#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
        else if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_FWD)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pFwdPkt; 
        }
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)
        else
        {   // should not happen!
            F_IPv4AssertCond(false, __func__, __LINE__);
            return;
        }

        pPktIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet((size_t)pEntry->arpIfIx);

        if(pNetIf == NULL || pNetIf == pPktIf)
        {   // match
            if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_TX || pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_MAC)
            {   // IPV4_PACKET TX packet 
                TCPIP_IPV4_FragmentTxAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_ARP_NET_ERR, (uint8_t)IPV4_FRAG_TX_ACK_HEAD | (uint8_t)IPV4_FRAG_TX_ACK_FRAGS);
            }
#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
            else if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_FWD)
            {   // FWD Packet
                TCPIP_PKT_PacketAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_ARP_NET_ERR);
            }
            else
            {
                // do nothing
            }
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)
            // back to pool
            TCPIP_Helper_SingleListTailAdd(&ipv4ArpPool, FC_ArpEntry2SglNode(pEntry)); 
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newList, FC_ArpEntry2SglNode(pEntry));
        }
    }

    // update the arp queue
    pList->list = newList;
    (void)TCPIP_Helper_ProtSglListUnlock(pList);
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (M_TCPIP_IPV4_FWD_ENABLE != 0)

// checks if this packet needs forwarding
// returns true if forwarded,
// false if forwarding failed
static bool TCPIP_IPV4_ProcessExtPkt(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV4_PKT_PROC_TYPE procType)
{
    if(ipv4ForwardDcpt == NULL)
    {   // no forwarding tables
        return false;
    }

    uint16_t netIx = TCPIPStackNetIxGet(pNetIf);

    IPV4_FORWARD_DESCRIPTOR* pFDcpt = ipv4ForwardDcpt + netIx;

    while((pFDcpt->runFlags & (uint8_t)IPV4_FWD_FLAG_FWD_ENABLE) != 0U)
    {   // forwarding enabled
        IPV4_HEADER_BARE* pHeader = FC_U8Ptr2HdrBare(pRxPkt->pNetLayer);
        if(pHeader->TimeToLive == 0U)
        {   // TTL expired
            return false;
        }

        F_IPv4ProcessExtPktDbg(pRxPkt);

        // find route
        const IPV4_ROUTE_TABLE_ENTRY* pEntry = TCPIP_IPV4_FindFwdRoute(pFDcpt, pRxPkt);
        if(pEntry != NULL)
        {   // found it
            return TCPIP_IPV4_ForwardPkt(pRxPkt, pEntry, procType);
        }

        // no route
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        TCPIP_IPV4_FORWARD_STAT* pFwdDbg = s_ipv4_fwd_stat + (netIx < 2U ? netIx : 2U);
        pFwdDbg->failNoRoute++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        break;
    }

    return false;
}

// Finds the entry in the routing table that routes this packet
// Select the entry from the table, based on the number of leading ones
// For now, simple linear search...
static const IPV4_ROUTE_TABLE_ENTRY* TCPIP_IPV4_FindFwdRoute(IPV4_FORWARD_DESCRIPTOR* pFDcpt, TCPIP_MAC_PACKET* pRxPkt)
{
    size_t ix;
    const IPV4_ROUTE_TABLE_ENTRY* pEntry = pFDcpt->fwdTable;
    // packet destination
    const IPV4_ADDR* dstAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);

    for(ix = 0; ix < pFDcpt->usedEntries; ix++)
    {
        if((dstAdd->Val & pEntry->netMask) == pEntry->netAddress)
        {   // found it
            return pEntry;
        }
        pEntry++;
    }

    // no route
    return NULL;
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
    const TCPIP_NET_IF* pDestIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet((size_t)pEntry->outIfIx);

    // packet destination
    const IPV4_ADDR* pDestAdd = TCPIP_IPV4_PacketGetDestAddress(pFwdPkt);

    while(true)
    {
        // check dest is not host 0
        if(TCPIPStackHostPartAddress(pDestIf, pDestAdd) == 0U)
        {   // invalid, discard
            destType = TCPIP_IPV4_DEST_FAIL;
            break;
        }

        // check if it's a net-directed broadcasts
        if(TCPIPStack_IsDirectedBcast(pDestIf, pDestAdd))
        {   
            (void)memset(pMacDst, 0xff, sizeof(*pMacDst));
            destType = TCPIP_IPV4_DEST_NETWORK; 
            break;
        }
        else if(TCPIPStack_NetMacType(pDestIf) == TCPIP_MAC_TYPE_PPP)
        {   // no MAC address or ARP resolution needed for a serial link
            (void)memset(pMacDst, 0x0, sizeof(*pMacDst));
            destType = TCPIP_IPV4_DEST_NETWORK; 
            break;
        }

        // check IP multicast address range from 224.0.0.0 to 239.255.255.255
        // can be done locally; No need for an ARP request.
        else if ((pDestAdd->v[0] >= 224U) && (pDestAdd->v[0] <= 239U))
        {
            pMacDst->v[0] = 0x01U;
            pMacDst->v[1] = 0x00U;
            pMacDst->v[2] = 0x5EU;
            pMacDst->v[3] = 0x7fU & pDestAdd->v[1];
            pMacDst->v[4] = pDestAdd->v[2];
            pMacDst->v[5] = pDestAdd->v[3];
            destType = TCPIP_IPV4_DEST_NETWORK; 
            break;
        }
        else
        {   
            if(pEntry->gwAddress != 0U)
            {   
                arpTarget->Val  = pEntry->gwAddress;
                destType = TCPIP_IPV4_DEST_GW;
            }
            else if(TCPIPStackIpAddFromLAN(pDestIf, pDestAdd))
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
                *ppMacAdd = NULL;    // not known yet
            }
            else
            {   // discard, cannot send
                destType = TCPIP_IPV4_DEST_FAIL;
            }
        }
        break;
    }

    F_IPv4FwdMacDestDebug(pDestAdd, arpTarget, pDestIf, destType, ppMacAdd != NULL);

    return destType;
}
// forwards a packet over a network
// returns true if success
// false otherwise
static bool TCPIP_IPV4_ForwardPkt(TCPIP_MAC_PACKET* pFwdPkt, const IPV4_ROUTE_TABLE_ENTRY* pEntry, IPV4_PKT_PROC_TYPE procType)
{
    TCPIP_MAC_ADDR   destMacAdd;
    TCPIP_MAC_ADDR* pMacDst = NULL;
    TCPIP_IPV4_DEST_TYPE destType;
    const TCPIP_NET_IF* pFwdIf;
    TCPIP_MAC_ETHERNET_HEADER* macHdr;
    IPV4_FORWARD_NODE* pFwdNode;
    IPV4_ADDR       arpTarget;
    bool            macRes;
    uint16_t        pktPayload, linkMtu;
    uint8_t         headerLen;

#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        TCPIP_IPV4_FORWARD_STAT* pFwdDbg = s_ipv4_fwd_stat + (pEntry->outIfIx < 2U ? pEntry->outIfIx : 2U);
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
    
    // the forward interface
    // do NOT set the packet interface, as this could be redirected internally too...
    pFwdIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet((size_t)pEntry->outIfIx);

    // check for proper packet source address:
    const IPV4_ADDR* pSrcAdd = TCPIP_IPV4_PacketGetSourceAddress(pFwdPkt);
    if(TCPIPStackNetPartAddress(pFwdIf, pSrcAdd) == 0U)
    {   // RFC 1812: source address {0, host} is not forwarded
        return false;
    }

    // select packet's destination MAC address
    pMacDst = &destMacAdd;
    // select packet's external destination MAC address
    destType = TCPIP_IPV4_FwdPktMacDestination(pFwdPkt, pEntry, &pMacDst, &arpTarget);
    if(destType == TCPIP_IPV4_DEST_FAIL) 
    {   // discard, cannot send
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        pFwdDbg->failMacDest++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        return false;
    }

    if(!TCPIP_STACK_NetworkIsUp(pFwdIf))
    {   // don't send over dead interface
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        pFwdDbg->failNetDown++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        return false;
    }

    if(destType != TCPIP_IPV4_DEST_SELF) 
    {   // check the MTU
        // get the payload w/o the MAC frame
        pktPayload = TCPIP_PKT_PayloadLen(pFwdPkt);
        linkMtu = TCPIPStackNetLinkMtu(pFwdIf);
        if(pktPayload > linkMtu)
        {
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            pFwdDbg->failMtu++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            return false;
        }
    }

    if(((uint16_t)procType & (uint16_t)IPV4_PKT_DEST_HOST) != 0U)
    {   // after forwarding need to be processed locally  
        // save packet MAC address before changing anything
        pFwdNode = TCPIP_IPV4_Forward_QueuePacket(pFwdPkt, procType);
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        if(pFwdNode == NULL)
        {   // no more packets could be queued up
            pFwdDbg->failFwdQueue++;
        }
        else
        {
            pFwdDbg->fwdQueuedPackets++;
        }
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        if(pFwdNode == NULL)
        {   // no more packets could be queued up
            return false;
        }
    }
    else
    {
        pFwdNode = NULL;
    }

    // set the proper source and destination MAC addresses
    macHdr = FC_Uptr2MacEthHdr(pFwdPkt->pMacLayer);
    if(pMacDst != NULL)
    {
        (void)memcpy(&macHdr->DestMACAddr, pMacDst, sizeof(macHdr->DestMACAddr));
    }
    (void)memcpy(&macHdr->SourceMACAddr, FC_CUptr2CMacAdd(TCPIPStack_NetMACAddressGet(pFwdIf)), sizeof(macHdr->SourceMACAddr));
    pFwdPkt->pDSeg->segLen += (uint16_t)sizeof(TCPIP_MAC_ETHERNET_HEADER);
    pFwdPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_TX; 

    // adjust the TTL and recalculate the IP checksum
    IPV4_HEADER_BARE* pHeader = FC_U8Ptr2HdrBare(pFwdPkt->pNetLayer);
    pHeader->TimeToLive -= 1U;
    pHeader->HeaderChecksum = 0U;
    headerLen = pHeader->IHL << 2U;
    if((pFwdIf->txOffload & (uint8_t)TCPIP_MAC_CHECKSUM_IPV4) == 0U)
    {   // not handled by hardware
        pHeader->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHeader, headerLen, 0);
    }

    if(pMacDst == NULL)
    {   // ARP target not known yet; queue it
        // cannot have both IPV4_PKT_DEST_FWD and IPV4_PKT_DEST_HOST and not know the ARP!
        F_IPv4AssertCond(((uint16_t)procType & (uint16_t)IPV4_PKT_DEST_HOST) == 0U, __func__, __LINE__);

        if(ipv4ArpHandle == NULL)
        {
            if((ipv4ArpHandle = TCPIP_ARP_HandlerRegister(NULL, &TCPIP_IPV4_ArpHandler, NULL)) == NULL)
            {
                SYS_ERROR(SYS_ERROR_WARNING, "IPv4 Fw: Failed to register ARP notification! \r\n");
                return false;
            }
        }

        if(!TCPIP_IPV4_QueueArpPacket(pFwdPkt, (uint16_t)pEntry->outIfIx, IPV4_ARP_PKT_TYPE_FWD, &arpTarget))
        {
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            pFwdDbg->failArpQueue++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            return false;
        }

        // ARP notification will be received: either TMO or resolved
        // mark packet as queued 
        pFwdPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        pFwdDbg->arpQueued++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        return true;
    }

    TCPIP_PKT_FlightLogTx(pFwdPkt, TCPIP_THIS_MODULE_ID);


    // TCPIP_IPV4_DEST_NETWORK/TCPIP_IPV4_DEST_GW and pMacDst != 0
    if(pFwdNode != NULL)
    {   // after forwarding need to be processed locally  
        macRes = TCPIP_IPV4_TxMacPkt(pFwdIf, pFwdPkt);
        if(macRes == false)
        {   // failed and need to dequeue...
            pFwdPkt->ipv4PktData = 0U;   // clear the 'forward first process later' flag
            (void)TCPIP_IPV4_Forward_DequeuePacket(pFwdNode, false);
        }
    }
    else
    {   // normal - unicast probably - forwarding
        macRes = TCPIP_IPV4_TxMacPkt(pFwdIf, pFwdPkt);
    }


#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
    if(macRes)
    {
        pFwdDbg->macPackets++;
    }
    else
    {
        pFwdDbg->failMac++;
    }
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)

    return macRes;
}

// Returns the Trailing zeroes count in an uint32_t
// NOTE: if v == 0, then it returns 31!
// routines from http://graphics.stanford.edu/~seander/bithacks.html
// public domain
static uint32_t IPV4_32TrailZeros(uint32_t v)
{
    uint32_t c;

    if ((v & 0x1U) != 0U) 
    {   // special case for odd v (assumed to happen half of the time)
        c = 0U;
    }
    else
    {
        c = 1U;
        if ((v & 0xffffU) == 0U) 
        {  
            v >>= 16U;  
            c += 16U;
        }
        if ((v & 0xffU) == 0U) 
        {  
            v >>= 8U;  
            c += 8U;
        }
        if ((v & 0xfU) == 0U) 
        {  
            v >>= 4U;
            c += 4U;
        }
        if ((v & 0x3U) == 0U) 
        {  
            v >>= 2U;
            c += 2U;
        }
        c -= v & 0x1U;
    }   

    return c;
}

// Returns the Leading zeroes count in an uint32_t
static uint32_t IPV4_32LeadingZeros(uint32_t v)
{
    if(v == 0U)
    {
        return 32U;
    }

    uint32_t c = 0U;

    if((v & 0xFFFF0000U) == 0U)
    {
        c += 16U;
        v <<= 16U;
    }

    if((v & 0xFF000000U) == 0U)
    {
        c +=  8U;
        v <<=  8U;
    }

    if((v & 0xF0000000U) == 0U)
    {
        c +=  4U;
        v <<=  4U;
    }

    if((v & 0xC0000000U) == 0U)
    {
       c += 2U;
       v <<= 2U;
    }

    if((v & 0x80000000U) == 0U)
    {
        c += 1U;
    }

    return c;
}


static TCPIP_IPV4_RES IPV4_BuildForwardTables(const TCPIP_IPV4_MODULE_CONFIG* pIpInit, const void* memH, size_t nIfs)
{
    TCPIP_IPV4_RES buildRes;
    bool    knownFmt;
    size_t  netIx, tblIx;
    size_t  usedEntries;
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;
    IPV4_ROUTE_TABLE_ENTRY* pTblEntry;
    IPV4_FORWARD_NODE*      pFwdNode;

    // allocate the descriptors
    ipv4ForwardDcpt = (IPV4_FORWARD_DESCRIPTOR*)TCPIP_HEAP_Calloc(memH, nIfs, sizeof(*pFDcpt) + pIpInit->forwardTableMaxEntries * sizeof(*pTblEntry));
    if(ipv4ForwardDcpt == NULL)
    {   // out of memory
        return TCPIP_IPV4_RES_MEM_ERR;
    }

    if(pIpInit->forwardTxQueueSize != 0U)
    {
        ipv4ForwardNodes = (IPV4_FORWARD_NODE*)TCPIP_HEAP_Calloc(memH, pIpInit->forwardTxQueueSize, sizeof(*ipv4ForwardNodes));
        if(ipv4ForwardNodes == NULL)
        {   // out of memory
            return TCPIP_IPV4_RES_MEM_ERR;
        }
    }

    ipv4ForwardIfs = nIfs;
    // set the forward tables
    pFDcpt = ipv4ForwardDcpt;
    // keep the forwarding tables at the end of allocated descriptor
    pTblEntry = FC_FwdDcpt2RteEntry(ipv4ForwardDcpt + nIfs);
    for(netIx = 0; netIx < nIfs; netIx++)
    {
        pFDcpt->totEntries = pIpInit->forwardTableMaxEntries;
        pFDcpt->iniFlags = pIpInit->forwardFlags;
        pFDcpt->fwdTable = pTblEntry; 
        if((pFDcpt->iniFlags & (uint16_t)TCPIP_IPV4_FWD_FLAG_ENABLED) != 0U)
        {
            pFDcpt->runFlags = (uint8_t)IPV4_FWD_FLAG_FWD_ENABLE; 
        }
        if((pFDcpt->iniFlags & (uint16_t)TCPIP_IPV4_FWD_FLAG_BCAST_ENABLED) != 0U)
        {
            pFDcpt->runFlags |= (uint8_t)IPV4_FWD_FLAG_BCAST_ENABLE;
        }
        if((pFDcpt->iniFlags & (uint16_t)TCPIP_IPV4_FWD_FLAG_MCAST_ENABLED) != 0U)
        {
            pFDcpt->runFlags |= (uint8_t)IPV4_FWD_FLAG_MCAST_ENABLE;
        }

        IPV4_ROUTE_TABLE_ENTRY* pRtEntry = pFDcpt->fwdTable;
        for(tblIx = 0; tblIx < pFDcpt->totEntries; tblIx++)
        {
            pRtEntry->nOnes = -1;   // mark entry as invalid
            pRtEntry++;
        }

        pTblEntry = pTblEntry + pIpInit->forwardTableMaxEntries;  
        pFDcpt++;
    }


    if(pIpInit->forwardTable == NULL)
    {
        usedEntries = 0;
    }
    else
    {
        usedEntries = pIpInit->forwardTableSize;
    }

    buildRes = TCPIP_IPV4_RES_OK;
    while(usedEntries != 0U)
    {
        knownFmt = false;
        if((pIpInit->forwardFlags & (uint16_t)TCPIP_IPV4_FWD_FLAG_ASCII_TABLE) == 0U) 
        {
            buildRes = IPv4_BuildBinaryTable(ipv4ForwardDcpt, pIpInit->forwardTable, usedEntries);
            knownFmt = true;
        }

#if (M_TCPIP_IPV4_FWD_TABLE_ASCII != 0)
        if((pIpInit->forwardFlags & (uint16_t)TCPIP_IPV4_FWD_FLAG_ASCII_TABLE) != 0U) 
        {
            buildRes = IPv4_BuildAsciiTable(ipv4ForwardDcpt, pIpInit->forwardTable, usedEntries);
            knownFmt = true;
        }
#endif
        if(knownFmt ==  false)
        {   // cannot do
            buildRes = TCPIP_IPV4_RES_FORMAT_ERR;
            break;
        }

        if((int)buildRes < 0)
        {
            break;
        }

        // sort the tables for proper operation
        pFDcpt = ipv4ForwardDcpt;
        for(netIx = 0; netIx < nIfs; netIx++)
        {
            if(pFDcpt->usedEntries != 0U)
            {
                IPv4_SortFwdTable(pFDcpt->fwdTable, pFDcpt->totEntries);
            }
            pFDcpt++;
        }

        // build the tx pool
        if(ipv4ForwardNodes != NULL)
        {   // if no bcast/mcast packets need to be forwarded AND processed internally
            // it's ok to have ipv4ForwardNodes == 0, so process only internally!
            pFwdNode = ipv4ForwardNodes;
            for(netIx = 0; netIx < pIpInit->forwardTxQueueSize; netIx++)
            {
                TCPIP_Helper_DoubleListTailAdd(&ipv4ForwardPool, FC_FwdNode2DblNode(pFwdNode)); 
                pFwdNode++;
            } 
        }

        break;
    }

    return buildRes;
}

#if (M_TCPIP_IPV4_FWD_TABLE_ASCII != 0)
// builds the forward table from ASCII input
// table size needs to be > than nEntries!
static TCPIP_IPV4_RES IPv4_BuildAsciiTable(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY* fwdTbl, size_t nEntries)
{
    size_t ix;
    TCPIP_IPV4_FORWARD_ENTRY_BIN bEntry = {0};

    TCPIP_IPV4_RES buildRes = TCPIP_IPV4_RES_OK;
    for(ix = 0; ix < nEntries; ix++)
    {
        const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pAEntry = &fwdTbl->entryAscii;
        buildRes = IPv4_AsciiToBinEntry(pAEntry, &bEntry, 1);
        if(buildRes != TCPIP_IPV4_RES_OK)
        {   // failed
            break;
        }
        // add entry
        buildRes = IPv4_AddBinaryTableEntry(pFwdDcpt, &bEntry);
        if(buildRes != TCPIP_IPV4_RES_OK)
        {   // failed
           break;
        }
        fwdTbl++;
    }

    return buildRes;
}

static TCPIP_IPV4_RES IPv4_AsciiToBinEntry(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pAEntry, TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry, size_t nEntries)
{
    size_t ix;
    TCPIP_NET_HANDLE inNetH, outNetH;
    IPV4_ADDR netAddress, netMask, gwAddress;

    if(pAEntry == NULL)
    {
        return TCPIP_IPV4_RES_FWD_ENTRY_ERR; 
    }
    
    for(ix = 0; ix < nEntries; ix++)
    {
        if(pAEntry->netAddress == NULL || strlen(pAEntry->netAddress) == 0U)
        {
            netAddress.Val = 0;
        }
        else if(!TCPIP_Helper_StringToIPAddress(pAEntry->netAddress, &netAddress))
        {
            return TCPIP_IPV4_RES_ADDRESS_ERR; 
        }
        else
        {
            // OK
        }

        if(pAEntry->netMask == NULL || strlen(pAEntry->netMask) == 0U)
        {
            netMask.Val = 0;
        }
        else if(!TCPIP_Helper_StringToIPAddress(pAEntry->netMask, &netMask))
        {
            return TCPIP_IPV4_RES_ADDRESS_ERR; 
        }
        else
        {
            // OK
        }

        if(pAEntry->gwAddress == NULL || strlen(pAEntry->gwAddress) == 0U)
        {
            gwAddress.Val = 0U;
        }
        else if(!TCPIP_Helper_StringToIPAddress(pAEntry->gwAddress, &gwAddress))
        {
            return TCPIP_IPV4_RES_ADDRESS_ERR; 
        }
        else
        {
            // OK
        }

        if(pAEntry->inIfName == NULL)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }
        inNetH = TCPIP_STACK_NetHandleGet(pAEntry->inIfName);
        if(inNetH == NULL)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }

        if(pAEntry->outIfName == NULL)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }
        outNetH = TCPIP_STACK_NetHandleGet(pAEntry->outIfName);
        if(outNetH == NULL)
        {
            return TCPIP_IPV4_RES_IF_ERR;
        }

        // OK
        pBEntry->netAddress = netAddress.Val;
        pBEntry->netMask = netMask.Val;
        pBEntry->gwAddress = gwAddress.Val;
        pBEntry->inIfIx = (uint8_t)TCPIP_STACK_NetIxGet(TCPIPStackHandleToNet(inNetH));
        pBEntry->outIfIx = (uint8_t)TCPIP_STACK_NetIxGet(TCPIPStackHandleToNet(outNetH));
        pBEntry->metric = pAEntry->metric;
    
        pAEntry++;
        pBEntry++;
    }

    return TCPIP_IPV4_RES_OK;
}

#if (M_TCPIP_IPV4_FWD_DYN_API != 0)
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pArrEntry, size_t nEntries)
{
    return IPv4_ProcessAsciiEntries(pArrEntry, nEntries, false);
}

TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pArrEntry, size_t nEntries)
{
    return IPv4_ProcessAsciiEntries(pArrEntry, nEntries, true);
}

static TCPIP_IPV4_RES IPv4_ProcessAsciiEntries(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pArrEntry, size_t nEntries, bool isRemove)
{
#define BIN_PROCESS_CHUNK_ENTRIES 16U
    TCPIP_IPV4_FORWARD_ENTRY_BIN bArrEntry[BIN_PROCESS_CHUNK_ENTRIES];

    (void)memset(bArrEntry, 0, sizeof(bArrEntry));

    size_t nChunks = nEntries / BIN_PROCESS_CHUNK_ENTRIES;
    size_t nRem = nEntries - nChunks * BIN_PROCESS_CHUNK_ENTRIES;

    TCPIP_IPV4_RES procRes = TCPIP_IPV4_RES_OK;
    const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pEntry = pArrEntry;
    while(nChunks-- != 0U)
    {
        procRes = IPv4_AsciiToBinEntry(pEntry, bArrEntry, BIN_PROCESS_CHUNK_ENTRIES);
        if(procRes == TCPIP_IPV4_RES_OK)
        {
            procRes = IPv4_DynFwdTblOper(bArrEntry, BIN_PROCESS_CHUNK_ENTRIES, isRemove);
        } 
        if(procRes != TCPIP_IPV4_RES_OK)
        {
            break;
        }
        pEntry += BIN_PROCESS_CHUNK_ENTRIES;
    }

    if(nRem != 0U && procRes == TCPIP_IPV4_RES_OK)
    {
        procRes = IPv4_AsciiToBinEntry(pEntry, bArrEntry, nRem);
        if(procRes == TCPIP_IPV4_RES_OK)
        {
            procRes = IPv4_DynFwdTblOper(bArrEntry, nRem, isRemove);
        } 
    }

    return procRes;
}

#endif  // (M_TCPIP_IPV4_FWD_DYN_API != 0)

#endif  // (M_TCPIP_IPV4_FWD_TABLE_ASCII != 0)

// builds the forward table from binary input
// table size needs to be > than nEntries!
static TCPIP_IPV4_RES IPv4_BuildBinaryTable(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY* fwdTbl, size_t nEntries)
{
    size_t ix;

    TCPIP_IPV4_RES buildRes = TCPIP_IPV4_RES_OK;
    for(ix = 0; ix < nEntries; ix++)
    {
        const TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry = &fwdTbl->entryBin; 
        buildRes = IPv4_AddBinaryTableEntry(pFwdDcpt, pBEntry);
        if(buildRes != TCPIP_IPV4_RES_OK)
        {   // failed
           break;
        }
        fwdTbl++;
    }

    return buildRes;
}

// Note: access should be locked for run time access!
static TCPIP_IPV4_RES IPv4_AddBinaryTableEntry(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry)
{
    TCPIP_NET_HANDLE netH;
    uint32_t onesCount, zerosCount;
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;
    IPV4_ROUTE_TABLE_ENTRY* pTblEntry;

    // get the corresponding interface
    netH = TCPIP_STACK_IndexToNet((size_t)pBEntry->inIfIx);
    if(netH == NULL)
    {
        return TCPIP_IPV4_RES_IF_ERR;
    }

    pFDcpt = pFwdDcpt + pBEntry->inIfIx;

    if(pFDcpt->usedEntries >= pFDcpt->totEntries)
    {   // cannot have this
        return TCPIP_IPV4_RES_ENTRIES_ERR;
    }

    netH = TCPIP_STACK_IndexToNet((size_t)pBEntry->outIfIx);
    if(netH == NULL)
    {
        return TCPIP_IPV4_RES_IF_ERR;
    }

    // check for the proper mask format
    zerosCount = IPV4_32LeadingZeros(pBEntry->netMask);
    onesCount = IPV4_32TrailZeros(~pBEntry->netMask);

    if(onesCount + zerosCount != 32U)
    {   // ill formatted mask
        return TCPIP_IPV4_RES_MASK_ERR;
    }

    // OK - add to end as the table is sorted
    pTblEntry = pFDcpt->fwdTable + pFDcpt->usedEntries;
    F_IPv4AssertCond(pTblEntry->nOnes < 0, __func__, __LINE__);
    // TCPIP_IPV4_FORWARD_ENTRY_BIN == IPV4_ROUTE_TABLE_ENTRY 
    (void)memcpy(pTblEntry, pBEntry, sizeof(*pBEntry));
    pTblEntry->nOnes = (int8_t)onesCount;

    pFDcpt->usedEntries++;

    return TCPIP_IPV4_RES_OK;
}

#if (M_TCPIP_IPV4_FWD_DYN_API != 0)
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries)
{
    return IPv4_DynFwdTblOper(pEntry, nEntries, false);
}

TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries)
{
    return IPv4_DynFwdTblOper(pEntry, nEntries, true);
}

// dynamically adds/removes a binary entry in the FIB
// Note: a sequence of operations MUST be of the same type: add or remove!
static TCPIP_IPV4_RES IPv4_DynFwdTblOper(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pArrEntry, size_t nEntries, bool isRemove)
{
    if(pArrEntry == NULL || nEntries == 0U)
    {
        return TCPIP_IPV4_RES_FWD_ENTRY_ERR; 
    }

    if(ipv4ForwardDcpt == NULL)
    {
        return TCPIP_IPV4_RES_FWD_TABLE_ERR; 
    }

    if(OSAL_MUTEX_Lock(&ipv4ForwardMux, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
    {
        return TCPIP_IPV4_RES_FWD_LOCK_ERR; 
    }

    size_t ix;
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;

    // lock and disable routing on all descriptors
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pFDcpt = ipv4ForwardDcpt;
    for(ix = 0; ix < ipv4ForwardIfs; ix++)
    {
        pFDcpt->saveFlags = pFDcpt->runFlags;   // store current value
        // disable routing on all interfaces when changing the FWD Table!
        pFDcpt->runFlags &= ~((uint8_t)IPV4_FWD_FLAG_FWD_ENABLE | (uint8_t)IPV4_FWD_FLAG_DYN_PROC);
        pFDcpt++;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    TCPIP_IPV4_RES opRes = TCPIP_IPV4_RES_OK; 
    while(true)
    {   
        // traverse the entries searching processing one interface at a time
        int setDcpt = 0;    // count descriptor updated
        int needProc = 0;   // count descriptors need processing
        IPV4_FORWARD_DESCRIPTOR* pCurrDcpt = NULL; // currently processed descriptor per pass

        const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry = pArrEntry;
        for(ix = 0; ix < nEntries; ix++)
        {
            // check we're dealing with a valid interface
            TCPIP_NET_HANDLE netH = TCPIP_STACK_IndexToNet((size_t)pEntry->inIfIx);
            if(netH == NULL)
            {
                opRes = TCPIP_IPV4_RES_IF_ERR;
                break;
            }

            pFDcpt = ipv4ForwardDcpt + pEntry->inIfIx;
            if((pFDcpt->runFlags & (uint8_t)IPV4_FWD_FLAG_DYN_PROC) != 0U) 
            {   // already processed
                continue;
            }

            if(pCurrDcpt == NULL)
            {   // select this as current descriptor
                pCurrDcpt = pFDcpt;
            }
            else if (pFDcpt != pCurrDcpt)
            {   // work on an descriptor/interface at a time
                needProc++; // descriptor needs processing
                continue;
            }
            else
            {
                // do nothing
            }

            // OK to add/remove to the fwd table for the current descriptor
            if(isRemove)
            {
                opRes = IPv4_RemoveBinaryTableEntry(ipv4ForwardDcpt, pEntry);
            }
            else
            {
                opRes = IPv4_AddBinaryTableEntry(ipv4ForwardDcpt, pEntry);
            }

            if(opRes == TCPIP_IPV4_RES_OK)
            {   // successful op on this descriptor
                setDcpt++;
            }
            else
            {   // failed
                break;
            }

            pEntry++;
        }

        if(pCurrDcpt != NULL)
        {
            if(setDcpt != 0)
            {   // there's been changes: sort the table
                IPv4_SortFwdTable(pCurrDcpt->fwdTable, pFDcpt->totEntries);
            }
            pCurrDcpt->runFlags |= (uint8_t)IPV4_FWD_FLAG_DYN_PROC;
        }

        if(opRes != TCPIP_IPV4_RES_OK) 
        {   // failed in some entry
            break;
        }

        if(needProc == 0)
        {   // done
            break;
        }
    }
    
    status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pFDcpt = ipv4ForwardDcpt;
    for(ix = 0; ix < ipv4ForwardIfs; ix++)
    {
        pFDcpt->runFlags = pFDcpt->saveFlags;   // restore flags
        pFDcpt++;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    (void)OSAL_MUTEX_Unlock(&ipv4ForwardMux);
    return opRes;
}

static TCPIP_IPV4_RES IPv4_RemoveBinaryTableEntry(IPV4_FORWARD_DESCRIPTOR* pFwdDcpt, const TCPIP_IPV4_FORWARD_ENTRY_BIN* pBEntry)
{
    size_t ix;
    TCPIP_NET_HANDLE netH;
    
    netH = TCPIP_STACK_IndexToNet((size_t)pBEntry->inIfIx);
    if(netH == NULL)
    {
        return TCPIP_IPV4_RES_IF_ERR;
    }

    IPV4_FORWARD_DESCRIPTOR* pFDcpt = pFwdDcpt + pBEntry->inIfIx;

    IPV4_ROUTE_TABLE_ENTRY* pTblEntry = pFDcpt->fwdTable;
    for(ix = 0; ix < pFDcpt->totEntries; ix++)
    {
        if(pTblEntry->nOnes >= 0)
        {   // search only valid entries
            // match without the nOnes field
            if(memcmp(pTblEntry->entryBytes, pBEntry->entryBytes, sizeof(*pBEntry) - 1U) == 0)
            {   // match found!
                pTblEntry->nOnes = -1;  // mark invalid; 
                pTblEntry->metric = 0U;
                pFDcpt->usedEntries--;
                return TCPIP_IPV4_RES_OK; 
            } 
        }
        pTblEntry++;
    }

    return TCPIP_IPV4_RES_FWD_NO_ENTRY_ERR; 
}

TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAll(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIP_Stack_UserHandleToNet(netH);
    
    if(pNetIf == NULL)
    {
        return TCPIP_IPV4_RES_IF_ERR;
    }

    if(ipv4ForwardDcpt == NULL)
    {
        return TCPIP_IPV4_RES_FWD_TABLE_ERR; 
    }

    uint16_t netIx = TCPIPStackNetIxGet(pNetIf);


    if(OSAL_MUTEX_Lock(&ipv4ForwardMux, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
    {
        return TCPIP_IPV4_RES_FWD_LOCK_ERR; 
    }

    IPV4_FORWARD_DESCRIPTOR* pFDcpt = ipv4ForwardDcpt + netIx;

    // lock and disable routing on all descriptors
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pFDcpt->saveFlags = pFDcpt->runFlags;   // store current value
    // disable routing on all interfaces when changing the FWD Table!
    pFDcpt->runFlags &= ~(uint8_t)IPV4_FWD_FLAG_FWD_ENABLE;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);


    size_t tblIx;
    IPV4_ROUTE_TABLE_ENTRY* pRtEntry = pFDcpt->fwdTable;
    for(tblIx = 0; tblIx < pFDcpt->totEntries; tblIx++)
    {
        pRtEntry->nOnes = -1;   // mark entry as invalid
        pRtEntry++;
    }
    pFDcpt->usedEntries = 0;

    
    status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pFDcpt->runFlags = pFDcpt->saveFlags;   // restore flags
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    (void)OSAL_MUTEX_Unlock(&ipv4ForwardMux);
    return TCPIP_IPV4_RES_OK;
}

#endif  // (M_TCPIP_IPV4_FWD_DYN_API != 0)


// sort the forwarding table by the largest number of leading ones in the network mask
// sorting function
static int F_RouteEntryCompare(const void* p1, const void* p2)
{
    IPV4_ROUTE_TABLE_ENTRY* pE1 = FC_CvPtr2RteEntry(p1);
    IPV4_ROUTE_TABLE_ENTRY* pE2 = FC_CvPtr2RteEntry(p2);
   

    if(pE1->nOnes < pE2->nOnes)
    {
        return 1;
    }
    else if(pE1->nOnes > pE2->nOnes)
    {
        return -1;
    }
    else
    {
        // do nothing
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
    else
    {
        // do nothing
    }

    // equal
    return 0;    
}


static void IPv4_SortFwdTable(IPV4_ROUTE_TABLE_ENTRY* pTable, size_t tableEntries)
{
    // sort the table ascending
    FC_Sort(pTable, tableEntries, sizeof(*pTable), &F_RouteEntryCompare);
}

size_t TCPIP_IPV4_ForwadTableSizeGet(TCPIP_NET_HANDLE netH, size_t* pValid)
{
    if(ipv4ForwardDcpt != NULL)
    {   
        TCPIP_NET_IF* pIf = TCPIPStackHandleToNetUp(netH);
        if(pIf != NULL)
        {
            IPV4_FORWARD_DESCRIPTOR* pFDcpt = ipv4ForwardDcpt + TCPIPStackNetIxGet(pIf);
            if(pValid != NULL)
            {
                *pValid = pFDcpt->usedEntries; 
            }
            return pFDcpt->totEntries; 
        }
    }

    // no forwarding tables or invalid interface
    if(pValid != NULL)
    {
        *pValid = 0U;
    }
    return 0U;
}

bool TCPIP_IPV4_ForwadTableEntryGet(TCPIP_NET_HANDLE netH, size_t index, TCPIP_IPV4_FORWARD_ENTRY_BIN* pFwdEntry)
{
    if(ipv4ForwardDcpt != NULL)
    {   
        TCPIP_NET_IF* pIf = TCPIPStackHandleToNetUp(netH);
        if(pIf != NULL)
        {
            IPV4_FORWARD_DESCRIPTOR* pFDcpt = ipv4ForwardDcpt + TCPIPStackNetIxGet(pIf);
            if(index < pFDcpt->totEntries)
            {
                if(pFwdEntry != NULL)
                {
                    IPV4_ROUTE_TABLE_ENTRY* pEntry = pFDcpt->fwdTable + index;
                    (void)memcpy(pFwdEntry, pEntry, sizeof(*pFwdEntry));
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

    IPV4_FORWARD_NODE* pFwdNode = FC_DblNode2FwdNode(TCPIP_Helper_DoubleListHeadRemove(&ipv4ForwardPool));
    if(pFwdNode != NULL)
    {  
        pFwdNode->pFwdPkt = pFwdPkt;
        pFwdNode->ownerAckFunc = pFwdPkt->ackFunc;
        pFwdNode->ownerAckParam = pFwdPkt->ackParam;
        pFwdPkt->ackFunc = &TCPIP_IPV4_ForwardAckFunc;
        pFwdPkt->ackParam = pFwdNode;

        TCPIP_MAC_ETHERNET_HEADER* fwdMacHdr = FC_Uptr2MacEthHdr(pFwdPkt->pMacLayer);
        (void)memcpy(pFwdNode->destMacAdd.v, fwdMacHdr->DestMACAddr.v, sizeof(pFwdNode->destMacAdd));
        (void)memcpy(pFwdNode->sourceMacAdd.v, fwdMacHdr->SourceMACAddr.v, sizeof(pFwdNode->sourceMacAdd));

        pFwdPkt->ipv4PktData = (uint16_t)procType;  // set the 'forward first process later' flag
        TCPIP_Helper_DoubleListTailAdd(&ipv4ForwardQueue, FC_FwdNode2DblNode(pFwdNode));
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

    if(ipv4ForwardNodes != NULL || aliveCheck == false)
    {
        TCPIP_Helper_DoubleListNodeRemove(&ipv4ForwardQueue, FC_FwdNode2DblNode(pFwdNode));
        // restore the packet
        TCPIP_MAC_PACKET* pFwdPkt = pFwdNode->pFwdPkt;
        pFwdPkt->ackFunc = pFwdNode->ownerAckFunc;
        pFwdPkt->ackParam = pFwdNode->ownerAckParam;

        TCPIP_MAC_ETHERNET_HEADER* fwdMacHdr = FC_Uptr2MacEthHdr(pFwdPkt->pMacLayer);
        (void)memcpy(fwdMacHdr->DestMACAddr.v, pFwdNode->destMacAdd.v, sizeof(fwdMacHdr->DestMACAddr));
        (void)memcpy(fwdMacHdr->SourceMACAddr.v, pFwdNode->sourceMacAdd.v, sizeof(fwdMacHdr->SourceMACAddr));

        TCPIP_Helper_DoubleListTailAdd(&ipv4ForwardPool, FC_FwdNode2DblNode(pFwdNode));
        dequeueRes = true;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    return dequeueRes;
}

// packet acknowledge function for a packet that's been forwarded
// and needs to be sent for internal processing
static void TCPIP_IPV4_ForwardAckFunc(TCPIP_MAC_PACKET* pkt,  const void* param)
{
    bool isReinsert;

    IPV4_FORWARD_NODE* pFwdNode = FC_CvPtr2FwdNode(param);
    // simple sanity check
    F_IPv4AssertCond(pFwdNode->pFwdPkt == pkt, __func__, __LINE__);
    // the 'forward first process later' flag should be set
    F_IPv4AssertCond((pkt->ipv4PktData & ((uint16_t)IPV4_PKT_DEST_FWD | (uint16_t)IPV4_PKT_DEST_HOST)) == ((uint16_t)IPV4_PKT_DEST_FWD | (uint16_t)IPV4_PKT_DEST_HOST) , __func__, __LINE__);

    // check we're still up and running
    isReinsert = TCPIP_IPV4_Forward_DequeuePacket(pFwdNode, true);

    if(isReinsert)
    {   // re-insert for process...re-insert to ourselves should succeed, since we're running!
        (void)TCPIPStackModuleRxInsert(TCPIP_MODULE_IPV4, pkt, true);
    }
    else
    {   // gone; simply acknowledge the packet
        TCPIP_PKT_PacketAcknowledge(pkt, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
    }
}
#else
size_t TCPIP_IPV4_ForwadTableSizeGet(TCPIP_NET_HANDLE netH, size_t* pValid)
{
    return 0;
}
bool TCPIP_IPV4_ForwadTableEntryGet(TCPIP_NET_HANDLE netH, size_t index, TCPIP_IPV4_FORWARD_ENTRY_BIN* pFwdEntry)
{
    return false;
}
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries)
{
    return TCPIP_IPV4_RES_FWD_TABLE_ERR;
}

TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries)
{
    return TCPIP_IPV4_RES_FWD_TABLE_ERR;
}
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAll(TCPIP_NET_HANDLE netH)
{
    return TCPIP_IPV4_RES_FWD_TABLE_ERR;
}
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pArrEntry, size_t nEntries)
{
    return TCPIP_IPV4_RES_FWD_TABLE_ERR; 
}

TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pArrEntry, size_t nEntries)
{
    return TCPIP_IPV4_RES_FWD_TABLE_ERR; 
}
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

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
    const TCPIP_NET_IF* pIf = TCPIPStackHandleToNetUp(netH);

    if(srcSet)
    {   // address in pSrcAddress is valid, no matter what
        if(pIf == NULL)
        {   // try to see if it belongs to some interface
            pIf = TCPIPStackIpAddFromAnyNet(NULL, pSrcAddress);
            if(pIf == NULL)
            {   // pSrcAddress is not the address of an interface!
                // use the destination address
                pIf = (const TCPIP_NET_IF*)TCPIP_IPV4_SelectDestInterface(pDestAddress);
            }
        }
        return pIf;
    }

    if(pIf == NULL)
    {   // use the destination address to decide which interface is this packet going out on
        pIf = (const TCPIP_NET_IF*)TCPIP_IPV4_SelectDestInterface(pDestAddress);
    }
    // else interface is forced

    if(pIf != NULL)
    {
        pSrcAddress->Val = TCPIP_STACK_NetAddressGet(pIf);
    }
    return pIf;
}

TCPIP_NET_HANDLE TCPIP_IPV4_SelectDestInterface(const IPV4_ADDR* pDestAddress)
{
    size_t netIx;
    size_t avlblInterfaces;
    uint32_t ifAdd, ifMask;
    TCPIP_NET_IF  *pIf;

    // use the destination address to decide which interface is this packet going out on

    if(pDestAddress == NULL || pDestAddress->Val == 0U)
    {   // unroutable
        return NULL;
    }

    if((avlblInterfaces = TCPIP_STACK_NumberOfNetworksGet()) > 1U)
    {   // we have a choice
        for(netIx = 0; netIx < avlblInterfaces; netIx++)
        {
            pIf = TCPIPStackHandleToNetLinked(TCPIP_STACK_IndexToNet(netIx));
            if(pIf != NULL)
            {
                ifAdd = TCPIPStackNetAddress(pIf);
                ifMask = TCPIPStackNetMask(pIf);

                if((ifAdd & ifMask) == (pDestAddress->Val & ifMask))
                {   // destination address is on this interface
                    return pIf;
                }
            }
        }
    }

    // use the default/any interface
    return TCPIPStackAnyNetLinked(true);
}


static TCPIP_NET_IF* TCPIP_IPV4_CheckPktTx(TCPIP_NET_HANDLE hNet, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_NET_IF* pNetIf = NULL;

    if(pPkt->next == NULL)
    {   // no support for chained packets!
        // make sure the interface is valid
        if((pNetIf = TCPIPStackHandleToNetLinked(hNet)) != NULL)
        {   // cannot transmit over dead interface
            if(TCPIPStackIsConfig(pNetIf) && (pPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_CONFIG) == 0U)
            {   // no packets go out in stack configuration
                pNetIf = NULL;
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
#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
    bool            isFragmented = false;
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)
    
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

    pNetIf = TCPIPStackHandleToNet(pPkt->netIfH);
    if(pNetIf == NULL)
    {
        return false;
    }

    // if the destination is internal, and need to change the network, solve it here
    pMacDst = &destMacAdd;
    if((pHostIf = TCPIP_STACK_MatchNetAddress(pNetIf, &pPkt->destAddress)) != NULL)
    {   // localhost address
        (void)memcpy(pMacDst->v, TCPIPStack_NetMACAddressGet(pHostIf), sizeof(*pMacDst));
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
    if(pNetIf == NULL)
    {   // cannot transmit over invalid interface
        return false;
    }


    pMacPkt->pktIf = pNetIf;
    IPV4_HEADER_BARE* pHdr = FC_U8Ptr2HdrBare(pMacPkt->pNetLayer);
    uint8_t precedence = pHdr->TypeOfService.precedence;
    pMacPkt->pktPriority = TCPIP_IPV4_TxPriQueue(pNetIf, precedence);

    // properly format the packet
    (void)TCPIP_PKT_PacketMACFormat(pMacPkt, pMacDst, FC_CUptr2CMacAdd(TCPIPStack_NetMACAddressGet(pNetIf)), TCPIP_ETHER_TYPE_IPV4);

    if(destType != TCPIP_IPV4_DEST_SELF)
    {   // get the payload w/o the MAC frame
        pktPayload = TCPIP_PKT_PayloadLen(pMacPkt) - (uint16_t)sizeof(TCPIP_MAC_ETHERNET_HEADER);
        linkMtu = TCPIPStackNetLinkMtu(pNetIf);
        if(pktPayload > linkMtu)
        {
#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
            if(pHdr->FragmentInfo.DF != 0U)
            {   // no fragments
                return false;
            }
            else if(!TCPIP_IPV4_FragmentTxPkt(pMacPkt, linkMtu, pktPayload))
            {   // failed to build the fragments; out of memory
                return false;
            }
            else
            {
                isFragmented = true;
            }
#else
            // MAC transmit will fail anyway
            return false;
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)
        }
    }

    TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);

    if(destType == TCPIP_IPV4_DEST_SELF)
    {
        TCPIP_IPV4_FragmentTxInsertToRx(pNetIf, pMacPkt, TCPIP_MAC_PKT_FLAG_UNICAST, true);
        return true;
    }


    // TCPIP_IPV4_DEST_NETWORK/TCPIP_IPV4_DEST_GW 
    if(pMacDst == NULL)
    {   // queue it
        if(ipv4ArpHandle == NULL)
        {
            if((ipv4ArpHandle = TCPIP_ARP_HandlerRegister(NULL, &TCPIP_IPV4_ArpHandler, NULL)) == NULL)
            {
                SYS_ERROR(SYS_ERROR_WARNING, "IPv4: Failed to register ARP notification! \r\n");
                return false;
            }
        }

        if(!TCPIP_IPV4_QueueArpPacket(arpPkt, TCPIPStackNetIxGet(pNetIf), arpType, &arpTarget))
        {
            return false;
        }

        // ARP notification will be received: either TMO or resolved
        // mark packet as queued 
        pMacPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
        return true;
    }

  
    // MAC sets itself the TCPIP_MAC_PKT_FLAG_QUEUED
    txRes = TCPIP_IPV4_TxMacPkt(pNetIf, pMacPkt);

#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
    if(txRes == false && isFragmented)
    {   // ack only fragments created here
        TCPIP_IPV4_FragmentTxAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR, (uint8_t)IPV4_FRAG_TX_ACK_FRAGS);
    }
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

    return txRes;
}


// IPv4 formats a IPV4_PACKET and calculates the header checksum
// the source and destination addresses should be updated in the packet 
void TCPIP_IPV4_PacketFormatTx(IPV4_PACKET* pPkt, uint8_t protocol, uint16_t ipLoadLen, TCPIP_IPV4_PACKET_PARAMS* pParams)
{
    uint8_t *pEndOpt, *pEndHdr;
    IPV4_ROUTER_ALERT_OPTION* pOpt;
    IPV4_HEADER_OPT* pHdr = FC_U8Ptr2HdrOpt(pPkt->macPkt.pNetLayer);
    uint16_t hdrLen = (uint16_t)sizeof(IPV4_HEADER_BARE) + (uint16_t)pPkt->optionLen;
    F_IPv4AssertCond(hdrLen <= (uint16_t)IPV4_HEADER_MAXIMUM_SIZE, __func__, __LINE__);

    pHdr->Version = IPv4_VERSION;
    pHdr->IHL = (uint8_t)(hdrLen >> 2U);
    // set the standard supported option, if any
    if((pPkt->optionMask & (uint16_t)IPV4_OPTION_MASK_ROUTER_ALERT) != 0U)
    {
        pOpt = FC_U32Ptr2RtrAlert(pHdr->options) + pPkt->optionOffset;
        pOpt->optCopied = IPV4_ROUTER_ALERT_OPTION_COPIED;
        pOpt->optClass = IPV4_ROUTER_ALERT_OPTION_CLASS;
        pOpt->optNumber = (uint8_t)TCPIP_IPV4_OPTION_ROUTER_ALERT;
        pOpt->optLength = IPV4_ROUTER_ALERT_OPTION_SIZE;
        pOpt->optValue = IPV4_ROUTER_ALERT_OPTION_DATA;

        // check if padding is needed
        pEndOpt = (uint8_t*)(pOpt + 1);
        pEndHdr = (uint8_t*)pHdr + hdrLen;
        int16_t iDiff = FC_PtrDiff2I16(pEndOpt, pEndHdr);
        F_IPv4AssertCond(iDiff <= 0, __func__, __LINE__);
        if(iDiff < 0)
        {
            *pEndOpt = 0;
        }
    }

    pHdr->TypeOfService.val = 0;
    if(pParams != NULL)
    {
        if(((uint8_t)pParams->tosFlags & (uint8_t)TCPIP_IPV4_RELIABILITY_HIGH) != 0U)
        {
            pHdr->TypeOfService.reliability = 1U;
        }
        if(((uint8_t)pParams->tosFlags & (uint8_t)TCPIP_IPV4_THROUGHPUT_HIGH) != 0U)
        {
            pHdr->TypeOfService.throughput = 1U;
        }
        if(((uint8_t)pParams->tosFlags & (uint8_t)TCPIP_IPV4_DELAY_LOW) != 0U)
        {
            pHdr->TypeOfService.delay = 1U;
        }
        if(((uint8_t)pParams->tosFlags & (uint8_t)TCPIP_IPV4_PRECEDENCE_MASK) != 0U)
        {
            pHdr->TypeOfService.precedence = (uint8_t)((uint8_t)pParams->tosFlags & (uint8_t)TCPIP_IPV4_PRECEDENCE_MASK);
        }
    }

    pHdr->TotalLength = TCPIP_Helper_htons(hdrLen + ipLoadLen);
    pHdr->Identification = TCPIP_Helper_htons(++ipv4Identifier);
    pHdr->FragmentInfo.val = TCPIP_Helper_htons(0);
    if(pParams != NULL && pParams->df != 0U)
    {
        pHdr->FragmentInfo.DF = 1U;
    }
    pHdr->TimeToLive = (pParams != NULL && pParams->ttl != 0U) ? pParams->ttl : IPV4_DEFAULT_TTL;
    pHdr->Protocol = protocol;
    pHdr->HeaderChecksum = 0U;
    pHdr->SourceAddress.Val = pPkt->srcAddress.Val;
    pHdr->DestAddress.Val = pPkt->destAddress.Val;
    if((((const TCPIP_NET_IF*)pPkt->netIfH)->txOffload & (uint8_t)TCPIP_MAC_CHECKSUM_IPV4) == 0U)
    {   // not handled by hardware; update the checksum
        pHdr->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHdr, hdrLen, 0);
    }

    pPkt->macPkt.pDSeg->segLen += hdrLen;
    pPkt->macPkt.pTransportLayer = pPkt->macPkt.pNetLayer + hdrLen;
    pPkt->macPkt.pktIf = pPkt->netIfH;
}

// queues a packet waiting for ARP resolution
static bool TCPIP_IPV4_QueueArpPacket(void* pPkt, uint16_t arpIfIx, IPV4_ARP_PKT_TYPE type, IPV4_ADDR* arpTarget)
{
    PROTECTED_SINGLE_LIST* pList = &ipv4ArpQueue;
    (void)TCPIP_Helper_ProtSglListLock(pList);
    IPV4_ARP_ENTRY* pEntry = FC_SglNode2ArpEntry(TCPIP_Helper_SingleListHeadRemove(&ipv4ArpPool));
    if(pEntry == NULL)
    {   // out of ARP entries in the pool
        SYS_ERROR(SYS_ERROR_WARNING, "IPv4: ARP entries pool empty!\r\n");
        (void)TCPIP_Helper_ProtSglListUnlock(pList);
        return false;
    }

    pEntry->type = (uint8_t)type;
    pEntry->arpIfIx = (uint8_t)arpIfIx;
    pEntry->pPkt = pPkt;     
    pEntry->arpTarget.Val = arpTarget->Val;
    TCPIP_Helper_SingleListTailAdd(&pList->list, FC_ArpEntry2SglNode(pEntry));
    (void)TCPIP_Helper_ProtSglListUnlock(pList);
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    if(type == IPV4_ARP_PKT_TYPE_TX || type == IPV4_ARP_PKT_TYPE_MAC)
    {
        s_ipv4_arp_stat.txSubmit++;
    }
    else if(type == IPV4_ARP_PKT_TYPE_FWD)
    {
        s_ipv4_arp_stat.fwdSubmit++;
    }
    else
    {
        F_IPv4AssertCond(false, __func__, __LINE__);
    }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    return true;
}



// ARP resolution done
static void TCPIP_IPV4_ArpHandler(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE arpEvType, const void* param)
{
    const TCPIP_NET_IF* pPktIf;
    SINGLE_LIST newList, *arpList;
    IPV4_ARP_ENTRY *pEntry;
    TCPIP_MAC_PACKET*   pMacPkt = NULL;
    TCPIP_MAC_PKT_ACK_RES   pktAckFail;
    TCPIP_MAC_ETHERNET_HEADER* macHdr;
    
    int evType = (int)arpEvType;

    TCPIP_Helper_SingleListInitialize (&newList);
    
    (void)TCPIP_Helper_ProtSglListLock(&ipv4ArpQueue);
    // traverse the ipv4ArpQueue list
    // and find all the packets waiting for the solved address

#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    if(evType >= 0)
    {
        s_ipv4_arp_stat.totSolved++;
    }
    else
    {
        s_ipv4_arp_stat.totFailed++;
    }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
    arpList = &ipv4ArpQueue.list;

    while((pEntry = FC_SglNode2ArpEntry(TCPIP_Helper_SingleListHeadRemove(arpList))) != NULL)
    {
        if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_TX)
        {   // IPV4_PACKET*
            pMacPkt = &pEntry->pTxPkt->macPkt;
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
            if(evType >= 0)
            {
                s_ipv4_arp_stat.txSolved++;
            }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
        }
        else if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_MAC)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pMacPkt;
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
            if(evType >= 0)
            {
                s_ipv4_arp_stat.txSolved++;
            }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
        }
#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
        else if(pEntry->type == (uint8_t)IPV4_ARP_PKT_TYPE_FWD)
        {   // TCPIP_MAC_PACKET*
            pMacPkt = pEntry->pFwdPkt; 
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
            if(evType >= 0)
            {
                s_ipv4_arp_stat.fwdSolved++;
            }
#endif  // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE) != 0)
        }
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)
        else
        {   // should not happen!
            F_IPv4AssertCond(false, __func__, __LINE__);
            return;
        }

        pPktIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet((size_t)pEntry->arpIfIx);
            
        pktAckFail = TCPIP_MAC_PKT_ACK_NONE; 
        if(pEntry->arpTarget.Val == ipAdd->Val)
        {   // match
            if(evType >= 0)
            {   // successfully resolved the ARP; update the packet destination
                macHdr = FC_Uptr2MacEthHdr(pMacPkt->pMacLayer);
                (void)memcpy(&macHdr->DestMACAddr, MACAddr, sizeof(*MACAddr));
                pMacPkt->next = NULL;  // send single packet
                if(pPktIf == NULL)
                {
                    pktAckFail = TCPIP_MAC_PKT_ACK_ARP_NET_ERR; 
                }
                else if(!TCPIP_IPV4_TxMacPkt(pPktIf, pMacPkt))
                {
                    pktAckFail = TCPIP_MAC_PKT_ACK_ARP_NET_ERR; 
                }
                else
                {
                    // ok
                }
            }
            else
            {   // some error;
                pktAckFail = TCPIP_MAC_PKT_ACK_ARP_TMO; 
            }

            if(pktAckFail != TCPIP_MAC_PKT_ACK_NONE)
            {   // some error; discard the packet
                TCPIP_IPV4_FragmentTxAcknowledge(pMacPkt, pktAckFail, (uint8_t)IPV4_FRAG_TX_ACK_HEAD | (uint8_t)IPV4_FRAG_TX_ACK_FRAGS);
            }

            // back to pool
            TCPIP_Helper_SingleListTailAdd(&ipv4ArpPool, FC_ArpEntry2SglNode(pEntry)); 
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newList, FC_ArpEntry2SglNode(pEntry));
        }
    }

    // update the arp queue
    ipv4ArpQueue.list = newList;
    (void)TCPIP_Helper_ProtSglListUnlock(&ipv4ArpQueue);

}

void  TCPIP_IPV4_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { // RX signal occurred
        TCPIP_IPV4_Process();
    }

#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_IPV4_Timeout();
    }
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

}

// processes an incoming IPV4 packet
static void TCPIP_IPV4_Process(void)
{
    const TCPIP_NET_IF* pNetIf;
    TCPIP_MAC_PACKET* pRxPkt;
    uint8_t      headerLen;
    uint16_t     headerChecksum, totalLength, payloadLen;
    IPV4_HEADER_BARE  *pHeader;
    IPV4_HEADER_BARE  cIpv4Hdr, *pCHeader;
    IPV4_PKT_PROC_TYPE procType;
    TCPIP_MAC_PKT_ACK_RES ackRes = TCPIP_MAC_PKT_ACK_NONE;

    // extract queued IPv4 packets
    while((pRxPkt = TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != NULL)
    {
#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
        if(pRxPkt->ipv4PktData != 0U)
        {   // re-visited packet; forwarded first; now processed
            pRxPkt->ipv4PktData = 0U;
            ackRes = TCPIP_IPV4_DispatchPacket(pRxPkt);
            F_IPv4AssertCond(ackRes == TCPIP_MAC_PKT_ACK_NONE, __func__, __LINE__);
            continue;
        }
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

        // processing 1st time. log the packet
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
#if (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)
        if(ipv4PktHandler != NULL)
        {
            bool was_processed = (*ipv4PktHandler)(pRxPkt->pktIf, pRxPkt, ipv4PktHandlerParam);
            if(was_processed)
            {
                TCPIP_PKT_FlightLogAcknowledge(pRxPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_EXTERN);
                continue;
            }
        }
#endif  // (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)

        while(true)
        {
            pHeader = FC_U8Ptr2HdrBare(pRxPkt->pNetLayer);
            // Make sure that this is an IPv4 packet.
            if((pHeader->Version) != IPv4_VERSION)
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // make sure the header length is within packet limits
            headerLen = pHeader->IHL << 2U;
            if(headerLen < sizeof(IPV4_HEADER_BARE) || (uint16_t)headerLen > pRxPkt->pDSeg->segLen)
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
            pNetIf = TCPIPStackMapAliasInterface((const TCPIP_NET_IF*)pRxPkt->pktIf, &pHeader->DestAddress);
            pRxPkt->pktIf = pNetIf;

            if(!TCPIP_STACK_NetworkIsUp(pNetIf))
            {   // discard the packet
                ackRes = TCPIP_MAC_PKT_ACK_IP_REJECT_ERR;
                break;
            }

            // discard wrong source address
            if(TCPIPStack_IsBcastAddress(pNetIf, &pHeader->SourceAddress))
            {   // net or limited bcast
                ackRes = TCPIP_MAC_PKT_ACK_SOURCE_ERR;
                break;
            }

            // discard wrong destination address
            if(pHeader->DestAddress.Val == 0U)
            {   // invalid destination
                ackRes = TCPIP_MAC_PKT_ACK_DEST_ERR;
                break;
            }

            // Make a copy of the header for the network to host conversion
            cIpv4Hdr = *pHeader;
            pCHeader = &cIpv4Hdr;
            pCHeader->TotalLength = totalLength;
            pCHeader->FragmentInfo.val = TCPIP_Helper_ntohs(pCHeader->FragmentInfo.val);

#if (M_TCPIP_IPV4_FRAGMENTATION == 0)
            bool isFragment =  (pCHeader->FragmentInfo.MF != 0U || pCHeader->FragmentInfo.fragOffset != 0U);
            // Throw this packet away if it is a fragment.  
            // We don't support IPv4 fragment reconstruction.
            if(isFragment)
            {   // discard the fragment
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }
#endif  // (M_TCPIP_IPV4_FRAGMENTATION == 0)

            // Validate the IP header.  If it is correct, the checksum 
            // will come out to 0x0000 (because the header contains a 
            // precomputed checksum).  A corrupt header will have a 
            // nonzero checksum.
            if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_IP) == 0U)
            {   // cannot skip checksum calculation if not handled by MAC!
                headerChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHeader, headerLen, 0);

                if(headerChecksum != 0U)
                {
                    // Bad packet. The function caller will be notified by means of the false 
                    // return value and it should discard the packet.
                    ackRes = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
                    break;
                }
            }


            TCPIP_IPV4_CheckRxPkt(pRxPkt);

            // Check the packet arrived on the proper interface and passes the filters
            procType = TCPIP_IPV4_VerifyPkt(pNetIf, pCHeader, pRxPkt);

            if(((uint16_t)procType & (uint16_t)IPV4_PKT_DEST_HOST) == 0U)
            {   // not processed internally; but some oter module may still need it; check the filters
                if(TCPIP_IPV4_VerifyPktFilters(pRxPkt, headerLen))
                {
                    procType = IPV4_PKT_DEST_HOST;
                }
            }

#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
            if(((uint16_t)procType & (uint16_t)IPV4_PKT_DEST_FWD) != 0U)
            {   // packet to be forwarded
                if(TCPIP_IPV4_ProcessExtPkt(pNetIf, pRxPkt, procType))
                {   // we're done
                    break;
                }
            }
#endif  // (M_TCPIP_IPV4_FWD_ENABLE != 0)

            if(((uint16_t)procType & ((uint16_t)IPV4_PKT_DEST_HOST)) == 0U)
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
    IPV4_HEADER_BARE  *pHeader;
    bool        isFragment;
    TCPIP_STACK_MODULE destId;
    uint8_t      headerLen;

    pHeader = FC_U8Ptr2HdrBare(pRxPkt->pNetLayer);
    headerLen = pHeader->IHL << 2U;

    // for internal processed packets, change to host order
    pRxPkt->pTransportLayer = pRxPkt->pNetLayer + headerLen;
    pRxPkt->pDSeg->segLen -= headerLen;
    pHeader->TotalLength = TCPIP_Helper_ntohs(pHeader->TotalLength);
    pRxPkt->totTransportLen = pHeader->TotalLength - headerLen;
    pHeader->FragmentInfo.val = TCPIP_Helper_ntohs(pHeader->FragmentInfo.val);

    isFragment = pHeader->FragmentInfo.MF != 0U || pHeader->FragmentInfo.fragOffset != 0U;

    // check where it needs to go
    destId = TCPIP_IPV4_FrameDestination(pHeader);
    if(destId == TCPIP_MODULE_NONE)
    {
        return TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
    }

#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
    pRxPkt->pkt_next = NULL;       // make sure it's not linked
    if(isFragment)
    {
        IPV4_FRAGMENT_NODE *fragNode;
        TCPIP_MAC_PKT_ACK_RES ackRes = TCPIP_IPV4_RxFragmentInsert(pRxPkt, &fragNode);

        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {   // failed; discard
            return ackRes;
        }

        if(fragNode != NULL)
        {
            pRxPkt = fragNode->fragHead; // this list is already ordered by pkt_next!
            isFragment = false; // let it through
        }
    }
#else
    F_IPv4AssertCond(isFragment == false, __func__, __LINE__);
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

    if(!isFragment)
    {   // forward this packet and signal
        if(!TCPIPStackModuleRxInsert(destId, pRxPkt, true))
        {
            return TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
        }
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
#if (M_TCPIP_IPV4_FWD_ENABLE != 0)
// packet verification when the forwarding is enabled
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPkt(const TCPIP_NET_IF* pNetIf, IPV4_HEADER_BARE* pHeader, TCPIP_MAC_PACKET* pRxPkt)
{
    if(ipv4ForwardDcpt == NULL)
    {   // no forwarding tables
        return TCPIP_IPV4_VerifyPktHost(pNetIf, pHeader, pRxPkt);
    }

    return TCPIP_IPV4_VerifyPktFwd(pNetIf, pHeader, pRxPkt);
}

static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktFwd(const TCPIP_NET_IF* pNetIf, IPV4_HEADER_BARE* pHeader, TCPIP_MAC_PACKET* pRxPkt)
{
    // process first the registered filters
    IPV4_ADDR* pktDestIP, *pktSrcIP;
    uint16_t currFilter; // current filter: TCPIP_IPV4_FILTER_TYPE value
    uint16_t procType;      // IPV4_PKT_PROC_TYPE value
    IPV4_FORWARD_DESCRIPTOR* pFDcpt;
    bool    isMcast, isBcast;
    uint16_t ifIx = TCPIPStackNetIxGet(pNetIf);
    TCPIP_MAC_ETHERNET_HEADER* macHdr = FC_Uptr2MacEthHdr(pRxPkt->pMacLayer);

#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
    TCPIP_IPV4_FORWARD_STAT* pFwdDbg = s_ipv4_fwd_stat + (ifIx < 2U ? ifIx : 2U);
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)

    while(true)
    {
        procType = (uint16_t)IPV4_PKT_TYPE_UNK;
        isMcast = isBcast = false;
        pktDestIP = &pHeader->DestAddress;
        // standard IPv4 filtering
        currFilter = (uint16_t)ipv4FilterType;

        // first few sanity checks
        if(TCPIPStackIpAddFromLAN(pNetIf, pktDestIP))
        {   // check dest is not host 0
            if(TCPIPStackHostPartAddress(pNetIf, pktDestIP) == 0U)
            {   // invalid, discard
                break;
            }
        }

        pktSrcIP = &pHeader->SourceAddress;
        if(pktSrcIP->v[0] == 0x7fU)
        {   // RFC 1812: discard if the source address is: { 127, <any> }
            break;
        }

        if(pktSrcIP->Val == 0U && !TCPIPStack_IsLimitedBcast(pktDestIP))
        {   // RFC 1812: discard if the source address is: {0, 0}; {0, host} not known yet!
            // however broadcast packets could be processed (DHCPs)
            break;
        }

        // start the Local Delivery Decision (5.2.3)

        // check is for any of our interfaces
        if(TCPIP_STACK_MatchNetAddress(pNetIf, pktDestIP) != NULL)
        {   // unicast to one of my networks
            procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_UNICAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_UNICAST) : ((uint16_t)IPV4_PKT_TYPE_UNICAST);
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            pFwdDbg->ucastPackets++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            break;
        }

        // check limited broadcast
        if(TCPIPStack_IsLimitedBcast(pktDestIP))
        {   // limited bcast, never forwarded
            procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_BROADCAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_BCAST) : ((uint16_t)IPV4_PKT_TYPE_BCAST);
            break;
        }

        if((isMcast = TCPIP_Helper_IsMcastAddress(pktDestIP)))
        {   // multicast
            if(TCPIPStack_IsLocalMcast(pktDestIP))
            {   // local, never forwarded
                procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_MULTICAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_MCAST) : ((uint16_t)IPV4_PKT_TYPE_MCAST);
            }
            else
            {   // can be forwarded
                procType = (uint16_t)IPV4_PKT_TYPE_MCAST | (uint16_t)IPV4_PKT_DEST_FWD;
                if((currFilter & (uint16_t)TCPIP_IPV4_FILTER_MULTICAST) == 0U)
                {
                    procType |= (uint16_t)IPV4_PKT_DEST_HOST;
                }
            }
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            pFwdDbg->mcastPackets++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)

            break;
        }

        // check net-directed IP address
        TCPIP_NET_IF* pRIf = TCPIPStackAnyNetDirected(pktDestIP);

        if(pRIf != NULL)
        {   // net-directed for one of the router's interfaces
            isBcast = true;
            if(TCPIPStackNetGetPrimary(pRIf) == TCPIPStackNetGetPrimary(pNetIf))
            {   // net-directed to one of the receiving interfaces
                // packet delivered locally
                procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_BROADCAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_BCAST) : ((uint16_t)IPV4_PKT_TYPE_BCAST);
                if((macHdr->DestMACAddr.v[0] & 0x01U) == 0U)
                {   // link layer unicast; the packet should also be forwarded
                    procType |= (uint16_t)IPV4_PKT_DEST_FWD;
                }
            }
            else
            {   // net-directed to another router interface, not the receiving one
                // packet delivered locally and forwarded
                procType = (uint16_t)IPV4_PKT_TYPE_BCAST | (uint16_t)IPV4_PKT_DEST_FWD;
                if((currFilter & (uint16_t)TCPIP_IPV4_FILTER_BROADCAST) == 0U)
                {
                    procType |= (uint16_t)IPV4_PKT_DEST_HOST;
                }
            }

#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            pFwdDbg->bcastPackets++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            break;
        }

        // some other type of packet
        // should be forwarded
        procType = (uint16_t)IPV4_PKT_DEST_FWD;
#if (M_TCPIP_IPV4_FORWARDING_STATS != 0)
            pFwdDbg->fwdPackets++;
#endif  // (M_TCPIP_IPV4_FORWARDING_STATS != 0)
        
        break;
    }

    // for local packets check that we actually handle it
    if((procType & (uint16_t)IPV4_PKT_DEST_HOST) != 0U && (TCPIP_IPV4_FrameDestination(pHeader) == TCPIP_MODULE_NONE))
    {   // unknown; discard
        procType &= ~(uint16_t)IPV4_PKT_DEST_HOST;
    }

    // few more rules to check for forwarding
    // 5.3.4
    while((procType & (uint16_t)IPV4_PKT_DEST_FWD) != 0U)
    {
        pFDcpt = ipv4ForwardDcpt + ifIx;
        if(isBcast)
        {   // check if we forward broadcast traffic
            if((pFDcpt->runFlags & (uint8_t)IPV4_FWD_FLAG_BCAST_ENABLE) == 0U)
            {
                procType &= ~(uint16_t)IPV4_PKT_DEST_FWD;
                break;
            }
        }
        if(isMcast)
        {   // check if we forward multicast traffic
            if((pFDcpt->runFlags & (uint8_t)IPV4_FWD_FLAG_MCAST_ENABLE) == 0U)
            {
                procType &= ~(uint16_t)IPV4_PKT_DEST_FWD;
                break;
            }
        }

        TCPIP_MAC_ADDR bcastAdd = { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };

        if(memcmp(macHdr->DestMACAddr.v, bcastAdd.v, sizeof(bcastAdd)) == 0)
        {   // link layer broadcast 
            if(!isMcast && !isBcast)
            {   // forward only if mcast or bcast
                procType &= ~(uint16_t)IPV4_PKT_DEST_FWD;
            }
            break;
        }

        if((macHdr->DestMACAddr.v[0] & 0x01U) != 0U)
        {   // link layer multicast
            if(!isMcast)
            {   // no forward if it's not mcast
                procType &= ~(uint16_t)IPV4_PKT_DEST_FWD;
            }
            break;
        }

        break;
    }

    return (IPV4_PKT_PROC_TYPE)procType;
}
#endif  // (M_TCPIP_IPV4_FWD_ENABLE == 0)

// packet verification when the forwarding is disabled
static IPV4_PKT_PROC_TYPE TCPIP_IPV4_VerifyPktHost(const TCPIP_NET_IF* pNetIf, IPV4_HEADER_BARE* pHeader, TCPIP_MAC_PACKET* pRxPkt)
{
    // process first the registered filters
    IPV4_ADDR* pktDestIP;
    uint16_t currFilter; // current filter: TCPIP_IPV4_FILTER_TYPE value
    uint16_t procType;    // IPV4_PKT_PROC_TYPE value

    while(true)
    {
        procType = (uint16_t)IPV4_PKT_TYPE_UNK;
        pktDestIP = &pHeader->DestAddress;
        // standard IPv4 filtering
        currFilter = (uint16_t)ipv4FilterType;

        if(TCPIPStackIpAddFromLAN(pNetIf, pktDestIP))
        {   // check dest is not host 0
            if(TCPIPStackHostPartAddress(pNetIf, pktDestIP) == 0U)
            {   // invalid, discard
                break;
            }
        }

        if(TCPIPStack_IsBcastAddress(pNetIf, pktDestIP))
        {   // net or limited bcast
            procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_BROADCAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_BCAST) : ((uint16_t)IPV4_PKT_TYPE_BCAST);
            break;
        }

        if(TCPIP_Helper_IsMcastAddress(pktDestIP))
        {   // multicast
            procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_MULTICAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_MCAST) : ((uint16_t)IPV4_PKT_TYPE_MCAST);
            break;
        }

        // unicast; check is for the arriving interface
        if(TCPIP_STACK_AddressIsOfNet(pNetIf, pktDestIP))
        {   // unicast to me
            procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_UNICAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_UNICAST) : ((uint16_t)IPV4_PKT_TYPE_UNICAST);
            break;
        }
#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (!defined(TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS) || (TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS == 0))
        // check if there is another interface that has this IP address, bridged to pNetIf
        else if(TCPIPStack_BridgeCheckIf(pNetIf))
        {   // pNetIf is bridged
            TCPIP_NET_IF* pTgtIf = TCPIP_STACK_NetByAddress(pktDestIP);
            if(pTgtIf != NULL && TCPIPStack_BridgeCheckIf(pTgtIf))
            {   // pNetIf and target IF are bridged; however we check that they are in the same network
                if(TCPIPStackNetNetwork(pTgtIf) == TCPIPStackNetNetwork(pNetIf))
                {
                    procType = ((currFilter & (uint16_t)TCPIP_IPV4_FILTER_UNICAST) == 0U) ? ((uint16_t)IPV4_PKT_DEST_HOST | (uint16_t)IPV4_PKT_TYPE_UNICAST) : ((uint16_t)IPV4_PKT_TYPE_UNICAST);
                    break;
                }
            }
        }
#endif  // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (!defined(TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS) || (TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS == 0))

        else
        {
            // some other type of packet
        }
        
        break;
    }

    return (IPV4_PKT_PROC_TYPE)procType;
}

static TCPIP_STACK_MODULE TCPIP_IPV4_FrameDestination(IPV4_HEADER_BARE* pHeader)
{
    TCPIP_STACK_MODULE destId;

    switch(pHeader->Protocol)
    {
#if defined(TCPIP_STACK_USE_ICMP_SERVER) || defined(TCPIP_STACK_USE_ICMP_CLIENT)
        case (uint8_t)IP_PROT_ICMP:
            destId = TCPIP_MODULE_ICMP;
            break;
#endif

#if defined(TCPIP_STACK_USE_IGMP)
        case (uint8_t)IP_PROT_IGMP:
            destId = TCPIP_MODULE_IGMP;
            break;
#endif

#if defined(TCPIP_STACK_USE_TCP)
        case (uint8_t)IP_PROT_TCP:
            destId = TCPIP_MODULE_TCP;
            break;
#endif

#if defined(TCPIP_STACK_USE_UDP)
        case (uint8_t)IP_PROT_UDP:
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


    F_IPv4FiltCountDbg("verify");
    if(cnt1 != 0U)
    {   // active filters
        TCPIP_Notification_Lock(&ipv4PacketFilters);
        for(fNode = FC_SglNode2FiltNode(ipv4PacketFilters.list.head); fNode != NULL; fNode = fNode->next)
        {
            if(fNode->active != 0U)
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
    IPV4_FILTER_LIST_NODE* newNode = NULL;

    if(ipv4MemH != NULL && handler != NULL )
    {
        IPV4_FILTER_LIST_NODE filtNode;
        (void)memset(&filtNode, 0, sizeof(filtNode));
        filtNode.handler = handler;
        filtNode.active = (active == false) ? 0U : 1U;

        newNode = FC_SglNode2FiltNode(TCPIP_Notification_Add(&ipv4PacketFilters, ipv4MemH, &filtNode, sizeof(*newNode)));
        if(newNode != NULL && active == true)
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
    IPV4_FILTER_LIST_NODE* filtNode = FC_SglNode2FiltNode(node);
    if(filtNode->active != 0U)
    {
        ipv4ActFilterCount--;
    }
}
    
// deregister the filter handler
// returns true or false if no such handler registered
bool Ipv4DeRegisterFilter(IPV4_FILTER_HANDLE hFilter)
{
    if(hFilter != NULL && ipv4MemH != NULL)
    {
        if(TCPIP_Notification_CbackRemove(FC_FiltHndl2SglNode(hFilter), &ipv4PacketFilters, ipv4MemH, &Ipv4DeRegisterCallback))
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
    for(fNode = FC_SglNode2FiltNode(ipv4PacketFilters.list.head); fNode != NULL; fNode = fNode->next)
    {
        if(fNode == FC_FiltHndl2FiltNode(hFilter))
        {
            if((bool)fNode->active != active)
            {   // change
                if(active != false)
                {
                    ipv4ActFilterCount++;
                }
                else
                {
                    ipv4ActFilterCount--;
                }

                F_IPv4FiltCountDbg("activate");
                fNode->active = (uint8_t)active;
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
    const TCPIP_NET_IF* pNetIf = (const TCPIP_NET_IF*)pPkt->netIfH;

    if(pIpAdd->Val == 0xffffffffU || pIpAdd->Val == TCPIP_STACK_NetAddressBcast(pNetIf))
    {
        (void)memset(pMacDst, 0xff, sizeof(*pMacDst));
        return TCPIP_IPV4_DEST_NETWORK;
    }
    else if(TCPIPStack_NetMacType(pNetIf) == TCPIP_MAC_TYPE_PPP)
    {   // no MAC address or ARP resolution needed for a serial link
        (void)memset(pMacDst, 0x0, sizeof(*pMacDst));
        return TCPIP_IPV4_DEST_NETWORK;
    }
    else
    {
        // do nothing
    }

    // check IP multicast address range from 224.0.0.0 to 239.255.255.255
    // can be done locally; No need for an ARP request.
    if ((pIpAdd->v[0] >= 224U) && (pIpAdd->v[0] <= 239U))
    {
        pMacDst->v[0] = 0x01U;
        pMacDst->v[1] = 0x00U;
        pMacDst->v[2] = 0x5EU;
        pMacDst->v[3] = 0x7fU & pIpAdd->v[1];
        pMacDst->v[4] = pIpAdd->v[2];
        pMacDst->v[5] = pIpAdd->v[3];
        return TCPIP_IPV4_DEST_NETWORK;
    }

    TCPIP_IPV4_DEST_TYPE destType;

    if(TCPIPStackIpAddFromLAN(pNetIf, pIpAdd))
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
        *ppMacAdd = NULL;    // not known yet
    }
    else
    {   // discard, cannot send
        destType = TCPIP_IPV4_DEST_FAIL;
    }

    return destType;
}

const IPV4_ADDR* TCPIP_IPV4_PacketGetDestAddress(TCPIP_MAC_PACKET* pPkt)
{
    IPV4_HEADER_BARE* pHdr = FC_U8Ptr2HdrBare(pPkt->pNetLayer);
    return &pHdr->DestAddress;
}

const IPV4_ADDR* TCPIP_IPV4_PacketGetSourceAddress(TCPIP_MAC_PACKET* pPkt)
{
    IPV4_HEADER_BARE* pHdr = FC_U8Ptr2HdrBare(pPkt->pNetLayer);
    return &pHdr->SourceAddress;
}

TCPIP_IPV4_FILTER_TYPE TCPIP_IPV4_PacketFilterSet(TCPIP_IPV4_FILTER_TYPE filtType)
{
    uint16_t currFilt;

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    currFilt = (uint16_t)ipv4FilterType;
    currFilt |= (uint16_t)filtType;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return (TCPIP_IPV4_FILTER_TYPE)currFilt;
}

TCPIP_IPV4_FILTER_TYPE TCPIP_IPV4_PacketFilterClear(TCPIP_IPV4_FILTER_TYPE filtType)
{
    uint16_t currFilt;

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    currFilt = (uint16_t)ipv4FilterType;
    currFilt &= ~(uint16_t)filtType;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return (TCPIP_IPV4_FILTER_TYPE)currFilt;
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
    IPV4_HEADER_BARE* pIpv4Hdr;
    const TCPIP_NET_IF* netIf = (const TCPIP_NET_IF*)pRxPkt->pktIf;

    pIpv4Hdr = FC_U8Ptr2HdrBare(pRxPkt->pNetLayer);
    pIpv4Hdr->DestAddress.Val = pIpv4Hdr->SourceAddress.Val;
    pIpv4Hdr->SourceAddress.Val = TCPIPStackNetAddress(netIf);

    pIpv4Hdr->TotalLength = TCPIP_Helper_htons(pIpv4Hdr->TotalLength);
    pIpv4Hdr->FragmentInfo.val = TCPIP_Helper_htons(pIpv4Hdr->FragmentInfo.val);
    if(pIpv4Hdr->TimeToLive == 0U)
    {
        pIpv4Hdr->TimeToLive = IPV4_DEFAULT_TTL;
    }
    uint8_t headerLen = pIpv4Hdr->IHL << 2U;
    pRxPkt->pDSeg->segLen += headerLen;

    if(setChecksum)
    {
        pIpv4Hdr->HeaderChecksum = 0U;
        if((netIf->txOffload & (uint8_t)TCPIP_MAC_CHECKSUM_IPV4) == 0U)
        {   // not handled by hardware
            pIpv4Hdr->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pIpv4Hdr, headerLen, 0);
        }
    }

    if(setMac)
    {   // set macHdr source and destination
        TCPIP_MAC_ETHERNET_HEADER* macHdr;
        macHdr = FC_Uptr2MacEthHdr(pRxPkt->pMacLayer);

        (void)memcpy(macHdr->DestMACAddr.v, macHdr->SourceMACAddr.v, sizeof(TCPIP_MAC_ADDR));
        (void)memcpy(macHdr->SourceMACAddr.v, TCPIPStack_NetMACAddressGet((const TCPIP_NET_IF*)pRxPkt->pktIf), sizeof(TCPIP_MAC_ADDR));

        pRxPkt->pDSeg->segLen += (uint16_t)sizeof(TCPIP_MAC_ETHERNET_HEADER);
    }

    pRxPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_TX; 
}

bool  TCPIP_IPV4_MacPacketTransmit(TCPIP_MAC_PACKET* pPkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR* pDestAddress)
{
    TCPIP_NET_IF* pNetIf, *pHostIf;
    TCPIP_IPV4_DEST_TYPE destType;

    pNetIf = TCPIPStackHandleToNet(hNet);
    if(pNetIf == NULL)
    {
        return false;
    }

    // if the destination is internal, and need to change the network, solve it here
    if((pHostIf = TCPIP_STACK_MatchNetAddress(pNetIf, pDestAddress)) != NULL)
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
    if(pNetIf == NULL)
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

IPV4_PACKET* TCPIP_IPV4_PacketAlloc(uint16_t pktPayload, TCPIP_IPV4_OPTION_DCPT* pOptions, size_t nOptions,  TCPIP_MAC_PACKET_FLAGS flags)
{
    IPV4_PACKET * pPacket;
    TCPIP_IPV4_OPTION_TYPE optType;
    size_t ix;
    uint16_t optLen;
    uint16_t totOptMask = 0U;   // IPV4_OPTION_TYPE_MASK value
    uint16_t totOptionsLen = 0U;

    if(nOptions != 0U && pOptions != NULL)
    {
        for(ix = 0; ix < nOptions; ix++)
        {
            optType = pOptions->optionType;
            optLen = pOptions->optionSize;
            // check for well known options
            if(optType == TCPIP_IPV4_OPTION_ROUTER_ALERT)
            {   // override
                optLen = (uint16_t)IPV4_ROUTER_ALERT_OPTION_SIZE;
                totOptMask |= (uint16_t)IPV4_OPTION_MASK_ROUTER_ALERT;
            }
            else if(optType == TCPIP_IPV4_OPTION_NONE)
            {   // standard case
                optLen = 0U;
            }
            else
            {
                // do nothing
            }

            totOptionsLen += optLen;
            pOptions++;
        }
    }

    // make sure is 4 bytes multiple
    if(totOptionsLen != 0U)
    {
        totOptionsLen = ((totOptionsLen + 3U) >> 2U) << 2U; 

        if(totOptionsLen > (uint16_t)IPV4_OPTIONS_MAXIMUM_SIZE)
        {
            totOptionsLen = (uint16_t)IPV4_OPTIONS_MAXIMUM_SIZE;
        }
    }

    uint16_t transpHdrLen = (uint16_t)sizeof(IPV4_HEADER_BARE) + totOptionsLen; 

    uint32_t uFlags = (uint32_t)flags | (uint32_t)TCPIP_MAC_PKT_FLAG_IPV4 | (uint32_t)TCPIP_MAC_PKT_FLAG_TX; 
    TCPIP_MAC_PACKET* pMacPkt = TCPIP_PKT_PacketAlloc((uint16_t)sizeof(IPV4_PACKET), transpHdrLen + pktPayload, (TCPIP_MAC_PACKET_FLAGS)uFlags);
    if(pMacPkt != NULL)
    {
        pPacket = FC_MacPkt2Ip4Pkt(pMacPkt);
        pPacket->optionLen = (uint8_t)totOptionsLen;
        pPacket->optionMask = totOptMask;
        return pPacket;
    }

    return NULL;
}


IPV4_OPTION_FIELD* TCPIP_IPV4_PacketOptionFieldSet(IPV4_PACKET* pPkt, uint16_t optionLen, bool firstOption)
{
    int optSpace;
    IPV4_OPTION_FIELD* pOpt = NULL;
    IPV4_HEADER_OPT* pHdr = FC_U8Ptr2HdrOpt(pPkt->macPkt.pNetLayer);
    
    if(firstOption)
    {
        pPkt->optionOffset = 0U;
    }
    
    optSpace = (int)pPkt->optionLen - (int)pPkt->optionOffset - (int)optionLen;
    if(optSpace >= 0)
    {   // OK
        pOpt = FC_U32Ptr2OptField(pHdr->options) + pPkt->optionOffset;
        pPkt->optionOffset += (uint8_t)optionLen;
    }


    return pOpt;
}

size_t TCPIP_IPV4_PacketOptionListGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_DCPT* pOptions, size_t nOptions )
{
    size_t  optLen, totOptLen;
    size_t  pktOptions;
    IPV4_OPTION_FIELD* pOpt;
    
    if(nOptions != 0U && pOptions == NULL)
    {
        nOptions = 0U;
    }

    pktOptions = 0;

    pOpt = F_IPv4CheckPacketOption(pRxPkt, &totOptLen);
    if(pOpt != NULL)
    {
        while(totOptLen != 0U)
        {
            optLen = pOpt->optLength;
            if(nOptions != 0U)
            {
                pOptions->optionType = (TCPIP_IPV4_OPTION_TYPE)pOpt->optNumber;
                pOptions->optionSize = (uint16_t)optLen;
                pOptions++;
                nOptions--;
            }
            totOptLen -= optLen;
            pktOptions++;
            pOpt = FC_OptFieldInc(pOpt, optLen);
        }
    }

    return pktOptions;
}

size_t TCPIP_IPV4_PacketOptionGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_TYPE optType, IPV4_OPTION_FIELD* pOptField, size_t optSize)
{
    IPV4_OPTION_FIELD* pOpt;
    size_t optLen;
    size_t  totOptLen;

    if(optSize != 0U && pOptField == NULL)
    {
        optSize = 0U;
    }

    pOpt = F_IPv4CheckPacketOption(pRxPkt, &totOptLen);
    if(pOpt != NULL)
    {   // traverse all options
        while(totOptLen != 0U)
        {
            optLen = pOpt->optLength;
            if(pOpt->optNumber == (unsigned)optType)
            {   // found it
                if(optLen <= optSize && optLen != 0U)
                {
                    uint8_t* pOptDest = (uint8_t*)pOpt;
                    uint8_t* pOptSrc = (uint8_t*)pOptField;
                    (void)memcpy(pOptDest, pOptSrc, optLen);
                }
                return (size_t)optLen;
            }
            totOptLen -= optLen;
            pOpt = FC_OptFieldInc(pOpt, optLen);
        }
    }


    return 0U;
}

// checks an IPv4 packet to have valid options and returns a pointer to the beginning of the option field
// it also updates the options length
static IPV4_OPTION_FIELD* F_IPv4CheckPacketOption(TCPIP_MAC_PACKET* pRxPkt, size_t* pOptLen)
{
    IPV4_HEADER_OPT* pHeader;
    size_t totOptLen;

    // minimal packet check
    pHeader = FC_U8Ptr2HdrOpt(pRxPkt->pNetLayer);

    // Make sure that this is an IPv4 packet.
    if((pHeader->Version) == IPv4_VERSION)
    {
        // make sure the header length is within packet limits
        uint8_t headerLen = pHeader->IHL << 2U;

        if(headerLen > (uint8_t)sizeof(IPV4_HEADER_BARE))
        {   // valid, existing options
            totOptLen = (size_t)headerLen - sizeof(IPV4_HEADER_BARE);
            *pOptLen = totOptLen;
            return FC_U32Ptr2OptField(pHeader->options);
        }
    }

    return NULL;

}

uint16_t TCPIP_IPV4_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pIf = TCPIPStackHandleToNetUp(netH);

    uint16_t dgramSize = pIf != NULL ? TCPIPStackNetLinkMtu(pIf) - (uint16_t)sizeof(IPV4_HEADER_BARE) : 0U;
    return dgramSize;
}

bool TCPIP_IPV4_IsFragmentationEnabled(void)
{
#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
    return true;
#else
    return false;
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)
}

// fragmentation functionality
//
#if (M_TCPIP_IPV4_FRAGMENTATION != 0)

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

    while((pF = FC_SglNode2FragNode(TCPIP_Helper_SingleListHeadRemove(&ipv4FragmentQueue))) != NULL)
    {
        if(currTick - pF->fragTStart > pF->fragTmo * tickFreq)
        {   // expired node; remove
            F_IPv4FragmentDbg(pF, NULL, TCPIP_IPV4_FRAG_DISCARD_TMO);
            TCPIP_IPV4_RxFragmentDiscard(pF, TCPIP_MAC_PKT_ACK_FRAGMENT_ERR);
        }
        else
        {
            TCPIP_Helper_SingleListHeadAdd(&newList, FC_FragNode2SglNode(pF));
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
    IPV4_HEADER_BARE *pFHdr, *pRxHdr;
    uint16_t rxMin, rxMax, nextMin;   

    // minimal check 
    pRxHdr = FC_U8Ptr2HdrBare(pRxPkt->pNetLayer);
    rxMin = pRxHdr->FragmentInfo.fragOffset * 8U;
    rxMax = rxMin + pRxPkt->totTransportLen;
    if(rxMax < rxMin)
    {   // overflow, fragOffset is too big?
        return TCPIP_MAC_PKT_ACK_FRAGMENT_ERR;
    } 

    *ppFrag = NULL;
    pParent = pPrevParent = NULL;
    for(pF = FC_SglNode2FragNode(ipv4FragmentQueue.head); pF != NULL; pF = pF->next)
    {
        pFHdr = FC_U8Ptr2HdrBare(pF->fragHead->pNetLayer);
        if(pFHdr->Identification == pRxHdr->Identification && pFHdr->SourceAddress.Val == pRxHdr->SourceAddress.Val &&
                pFHdr->DestAddress.Val == pRxHdr->DestAddress.Val && pFHdr->Protocol == pRxHdr->Protocol)
        {   // found parent fragment
            pParent = pF;
            break;
        }   
        pPrevParent = pF;
    }

    if(pParent == NULL)
    {   // brand new fragment packet
        if(TCPIP_Helper_SingleListCount(&ipv4FragmentQueue) >= (size_t)TCPIP_IPV4_FRAGMENT_MAX_STREAMS)
        {   // don't start another fragmented stream
            return TCPIP_MAC_PKT_ACK_FRAGMENT_ERR;
        }

        IPV4_FRAGMENT_NODE*  newNode = (IPV4_FRAGMENT_NODE*)TCPIP_HEAP_Calloc(ipv4MemH, 1, sizeof(*newNode));

        if(newNode == NULL)
        {   // failed memory allocation
            return TCPIP_MAC_PKT_ACK_FRAGMENT_ERR;
        }

        newNode->fragHead = pRxPkt;
        newNode->fragTStart = SYS_TMR_TickCountGet();  
        newNode->fragTmo =  TCPIP_IPV4_FRAGMENT_TIMEOUT;
        newNode->nFrags =  1;

        F_IPv4FragmentDbg(newNode, pRxPkt, TCPIP_IPV4_FRAG_CREATED);
        TCPIP_Helper_SingleListTailAdd(&ipv4FragmentQueue, FC_FragNode2SglNode(newNode));  
        return TCPIP_MAC_PKT_ACK_NONE;
    }

    // this is just a new fragment;
    if(pParent->nFrags >= (uint16_t)TCPIP_IPV4_FRAGMENT_MAX_NUMBER)
    {   // more fragments than allowed
        (void)TCPIP_Helper_SingleListNextRemove(&ipv4FragmentQueue, FC_FragNode2SglNode(pPrevParent));
        F_IPv4FragmentDbg(pParent, pRxPkt, TCPIP_IPV4_FRAG_DISCARD_EXCEEDED);
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
    IPV4_HEADER_BARE *pCurrHdr;
    uint16_t currMin, currMax;   
    bool fragOverlap = false;

    // insert in proper place
    pPrevPkt = NULL;
    pCurrPkt = pParent->fragHead;
    while(pCurrPkt != NULL)
    {
        pNextPkt = pCurrPkt->pkt_next;
        pCurrHdr = FC_U8Ptr2HdrBare(pCurrPkt->pNetLayer);
        currMin = pCurrHdr->FragmentInfo.fragOffset * 8U;
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
            uint16_t ulb, uld, ule;

            fragOverlap = true;

            ulb = (rxMin < currMin) ? 0U : rxMin - currMin;

            ule = (currMax < rxMax) ? 0U : currMax - rxMax;

            uld = pCurrPkt->totTransportLen - (ulb + ule);

            if(ule == 0U && ulb == 0U)
            {   // total overlap; rx > current; simply discard current
                if(pPrevPkt != NULL)
                {   
                    pPrevPkt->pkt_next = pNextPkt;
                }
                else
                {   // head is removed
                    pParent->fragHead = pNextPkt;
                }
                F_IPv4FragmentDbg(pParent, pCurrPkt, TCPIP_IPV4_FRAG_DISCARD_OVERLAP);
                pParent->nFrags--;
                TCPIP_PKT_PacketAcknowledge(pCurrPkt, TCPIP_MAC_PKT_ACK_FRAGMENT_ERR); 
                pCurrPkt = pNextPkt;
                continue;
            }
            else if(ulb == 0U)
            {   // partial overlap; discard at the beginning of current 
                uint16_t fragOffs = pCurrHdr->FragmentInfo.fragOffset;
                fragOffs += (uint16_t)(uld / 8U); 
                pCurrHdr->FragmentInfo.fragOffset = fragOffs;
                pCurrPkt->totTransportLen -= uld;
                pCurrPkt->pTransportLayer += uld;
            }
            else if(ule == 0U)
            {   // partial overlap; discard at the end of current
                pCurrPkt->totTransportLen -= uld;
            }
            else
            {   // ulb!= 0 && ule != 0; total overlap; rx < current; keep begin + discard + keep end
                pCurrPkt->totTransportLen -= uld + ule;
                // copy the end part to rx; consider packets spanning multiple segments!
                TCPIP_MAC_DATA_SEGMENT* pDestSeg = TCPIP_PKT_DataSegmentGet(pRxPkt, pRxPkt->pTransportLayer +  pRxPkt->totTransportLen, true);
                F_IPv4AssertCond(pDestSeg != NULL, __func__, __LINE__);
                // check that there's enough room; it should, since the current packet is larger!
                if(ule > pDestSeg->segSize - pDestSeg->segLen)
                {   // copy less; probably the packet will tmo!
                    ule = pDestSeg->segSize - pDestSeg->segLen;
                    F_IPv4AssertCond(false, __func__, __LINE__); // shouldn't happen though since all packets could hold this size...
                }

                // ok, we can copy
                uint8_t* startAdd = pCurrPkt->pTransportLayer + (ulb + uld);
                uint16_t copyBytes = TCPIP_Helper_PacketCopy(pCurrPkt, pDestSeg->segLoad + pDestSeg->segLen, &startAdd, ule, true);
                if(copyBytes != ule)
                {
                    F_IPv4AssertCond(false, __func__, __LINE__);
                }
                pDestSeg->segLen += ule;
                pRxPkt->totTransportLen += ule;
            }
            pNextPkt = pCurrPkt;    // reevaluate
            pCurrPkt = pNextPkt;
            continue;
        }
        
        // advance;
        pPrevPkt = pCurrPkt;
        pCurrPkt = pNextPkt;
    }

    // found a spot for the rx packet;insert after pPrevPkt
    if(pPrevPkt != NULL)
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
#if ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FRAGMENT) != 0)
    TCPIP_IPV4_FRAG_EVENT_TYPE fragEvent = fragOverlap ? TCPIP_IPV4_FRAG_INSERT_OVERLAP : TCPIP_IPV4_FRAG_INSERTED; 
    F_IPv4FragmentDbg(pParent, pRxPkt, fragEvent);
#else
    (void)fragOverlap;  // quiet unused variable warning
#endif // ((TCPIP_IPV4_DEBUG_LEVEL & TCPIP_IPV4_DEBUG_MASK_FRAGMENT) != 0)

    pRxPkt->next = NULL; 
    pParent->nFrags++;

    // check for packet completion
    bool pktDone = false;
    pCurrPkt = pParent->fragHead;
    pCurrHdr = FC_U8Ptr2HdrBare(pCurrPkt->pNetLayer);
    if(pCurrHdr->FragmentInfo.MF != 0U && pCurrHdr->FragmentInfo.fragOffset == 0U)
    {   // at least 2 frags and 1st fragment is in place
        while(pCurrPkt != NULL)
        {
            pNextPkt = pCurrPkt->pkt_next;
            pCurrHdr = FC_U8Ptr2HdrBare(pCurrPkt->pNetLayer);
            // check if gap

            if(pNextPkt != NULL)
            {
                currMax = pCurrHdr->FragmentInfo.fragOffset * 8U + pCurrPkt->totTransportLen;
                nextMin = FC_U8Ptr2HdrBare(pNextPkt->pNetLayer)->FragmentInfo.fragOffset * 8U;
                if(currMax != nextMin)
                {   // gap
                    break;
                }
            }

            // no gap
            if(pCurrHdr->FragmentInfo.MF == 0U)
            {   // last packet is here; we're done; 
                F_IPv4AssertCond(pNextPkt == NULL, __func__, __LINE__);
                pktDone = true;
                break;
            }
            pCurrPkt = pNextPkt;
        }
    }
    // else no way, missing the 1st fragment

    if(pktDone)
    {   // completed; remove the packet from the list
        *ppFrag = pParent;
        (void)TCPIP_Helper_SingleListNextRemove(&ipv4FragmentQueue, FC_FragNode2SglNode(pPrevParent));
        TCPIP_IPV4_RxFragmentDiscard(pParent, TCPIP_MAC_PKT_ACK_NONE);    // segments are still valid but the node itself is deleted
        F_IPv4FragmentDbg(pParent, NULL, TCPIP_IPV4_FRAG_COMPLETE);
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
        pPkt = pFrag->fragHead;
        while(pPkt != NULL)
        {
            pPktNext = pPkt->pkt_next;
            TCPIP_PKT_PacketAcknowledge(pPkt, ackRes); 
            pPkt = pPktNext;
        }
    }

    (void)TCPIP_HEAP_Free(ipv4MemH, pFrag);
}

// purges the ipv4FragmentQueue 
// if pRxPkt == 0, then it purges everything
// otherwise it looks for a specific packet 
static void TCPIP_IPV4_RxFragmentListPurge(SINGLE_LIST* pL)
{
    IPV4_FRAGMENT_NODE* pF;

    while((pF = FC_SglNode2FragNode(TCPIP_Helper_SingleListHeadRemove(pL))) != NULL)
    {
        TCPIP_IPV4_RxFragmentDiscard(pF, TCPIP_MAC_PKT_ACK_PROTO_DEST_CLOSE);
    }
}


// fragment transmit functionality


// TX packet needs fragmentation; each fragment will contain the IPv4 header and a fragment of data
static bool TCPIP_IPV4_FragmentTxPkt(TCPIP_MAC_PACKET* pMacPkt, uint16_t linkMtu, uint16_t pktPayload)
{
    uint16_t ix;
    IPV4_FRAG_TX_PKT    *pFragTx;
    TCPIP_MAC_PACKET    *pFragPkt, *pMacNext, *tail;
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    IPV4_HEADER_BARE    *pHdr;
    uint8_t*            pIpv4Load;
    IPV4_FRAGMENT_INFO  fragInfo;

    uint16_t pktHeaderSize = FC_PtrDiff2UI16(pMacPkt->pTransportLayer - pMacPkt->pMacLayer); 

    uint16_t ipv4HeaderSize = FC_PtrDiff2UI16(pMacPkt->pTransportLayer - pMacPkt->pNetLayer);
    uint16_t ipv4Payload = pktPayload - ipv4HeaderSize;

    uint16_t fragSize = (linkMtu - ipv4HeaderSize) & 0xfff8U;  // fragments need to be multiple of 8
    uint16_t nFrags = (ipv4Payload + fragSize - 1U) / fragSize;   // round up
    uint16_t lastFragSize = ipv4Payload - (nFrags - 1U) * fragSize;

    if(nFrags > (uint16_t)TCPIP_IPV4_FRAGMENT_MAX_NUMBER)
    {
        return false;
    }

    // allocate nFrags -1 packets; we already have the 1st
    pMacPkt->pkt_next = NULL;
    tail = pMacPkt;
    uint32_t uFlags = (uint32_t)TCPIP_MAC_PKT_FLAG_IPV4 | (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT | (uint32_t)TCPIP_MAC_PKT_FLAG_TX; 
    for(ix = 1U; ix < nFrags; ix++)
    {
        pFragTx = FC_MacPkt2FragPkt(TCPIP_PKT_PacketAlloc((uint16_t)sizeof(IPV4_FRAG_TX_PKT), ipv4HeaderSize, (TCPIP_MAC_PACKET_FLAGS)uFlags));
        if(pFragTx != NULL)
        {   
            TCPIP_PKT_PacketAcknowledgeSet(&pFragTx->macPkt, &TCPIP_IPV4_FragmentTxAckFnc, NULL);
            pFragTx->fragSeg.segFlags = (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC; // embedded in packet itself
            // copy the header part: MAC + IPv4
            (void)memcpy(pFragTx->macPkt.pMacLayer, pMacPkt->pMacLayer, pktHeaderSize);
            pFragTx->macPkt.pDSeg->segLen = pktHeaderSize;
            pFragTx->macPkt.pktPriority = pMacPkt->pktPriority;
            
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
        pFragPkt = pMacPkt->pkt_next;
        while(pFragPkt != NULL)
        {   // deallocate
            pMacNext = pFragPkt->pkt_next;
            TCPIP_PKT_PacketFree(pFragPkt);
            pFragPkt = pMacNext;
        } 

        return false;
    }


    // assemble the fragments
    pIpv4Load = pMacPkt->pTransportLayer; 
    ix = 0U;
    pFragPkt = pMacPkt;
    while(ix < nFrags)
    {
        uint16_t currFragSize = (ix == nFrags - 1U) ? lastFragSize : fragSize;

        if(ix == 0U)
        {   // this is the master/parent packet
            pSeg = pFragPkt->pDSeg;
            pSeg->segLen = pktHeaderSize + currFragSize;    // truncate size
        }
        else
        {
            pFragTx = FC_MacPkt2FragPkt(pFragPkt);
            pSeg = &pFragTx->fragSeg;
            pSeg->segLen = pSeg->segSize = currFragSize;
            pSeg->segLoad = pIpv4Load;
            pFragPkt->pDSeg->next = pSeg;  // link to the 1st data seg to be transmitted
        }

        // adjust the IPv4 header
        pHdr = FC_U8Ptr2HdrBare(pFragPkt->pNetLayer);
        pHdr->TotalLength = TCPIP_Helper_htons(ipv4HeaderSize + currFragSize);
        fragInfo.val = 0;
        fragInfo.fragOffset = (uint8_t)(FC_PtrDiff2UI16(pIpv4Load - pMacPkt->pTransportLayer) / 8U); 
        if(ix != nFrags - 1U)
        {
            fragInfo.MF = 1U;
        }
        pHdr->FragmentInfo.val = TCPIP_Helper_htons(fragInfo.val);
        // update the checksum
        pHdr->HeaderChecksum = 0U;
        // no checksum offload for fragmented packets
        pHdr->HeaderChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pHdr, ipv4HeaderSize, 0U);

        pIpv4Load += currFragSize;

        ix++;
        pFragPkt = pFragPkt->pkt_next;
    }

    // success
    return true;
}

static void TCPIP_IPV4_FragmentTxAckFnc(TCPIP_MAC_PACKET* pkt,  const void* param)
{
    // this is temporary fragment packet;
    // if MAC is done with it, just delete
    TCPIP_PKT_PacketFree(pkt);
}

// transmits the packet as multiple fragments
static bool TCPIP_IPV4_TxMacPkt(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_MAC_PACKET *pFragPkt, *pFragNext;
    int resTx;

    pFragPkt = pPkt;
    while(pFragPkt != NULL)
    {
        pFragNext = pFragPkt->pkt_next;
        resTx = (int)TCPIPStackPacketTx(pNetIf, pFragPkt);
        if(resTx < 0)
        {
            return false;
        }
        pFragPkt = pFragNext;
    }

    return true;
}


static void TCPIP_IPV4_FragmentTxAcknowledge(TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PKT_ACK_RES ackRes, uint8_t txAck)
{
    TCPIP_MAC_PACKET *pFragHead, *pFragPkt, *pFragNext;

    pFragHead = ((txAck & (uint8_t)IPV4_FRAG_TX_ACK_HEAD) == 0U) ? pTxPkt->pkt_next : pTxPkt;

    pFragPkt = pFragHead;
    while(pFragPkt != NULL)
    {
        pFragNext = pFragPkt->pkt_next;
        TCPIP_PKT_PacketAcknowledge(pFragPkt, ackRes);
        pFragPkt = pFragNext;
    }
}

static void TCPIP_IPV4_FragmentTxInsertToRx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pTxPkt, TCPIP_MAC_PACKET_FLAGS flags, bool doSignal)
{
    TCPIP_MAC_PACKET *pFragPkt;

    for(pFragPkt = pTxPkt; pFragPkt != NULL; pFragPkt = pFragPkt->pkt_next)
    {
        pTxPkt->pktFlags |= (uint32_t)flags;
        TCPIPStackInsertRxPacket(pNetIf, pFragPkt, doSignal);
    }
}


#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

// external packet processing
#if (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)
TCPIP_IPV4_PROCESS_HANDLE TCPIP_IPV4_PacketHandlerRegister(TCPIP_IPV4_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_IPV4_PROCESS_HANDLE pHandle = NULL;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv4PktHandler == NULL)
    {
        ipv4PktHandlerParam = handlerParam;
        ipv4PktHandler = pktHandler;
        pHandle = FC_PktHndl2ProcHndl(pktHandler);
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return pHandle;
}

bool TCPIP_IPV4_PacketHandlerDeregister(TCPIP_IPV4_PROCESS_HANDLE pktHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    TCPIP_IPV4_PROCESS_HANDLE procHandle = FC_PktHndl2ProcHndl(ipv4PktHandler);
    if(procHandle == pktHandle)
    {
        ipv4PktHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}
#else
TCPIP_IPV4_PROCESS_HANDLE TCPIP_IPV4_PacketHandlerRegister(TCPIP_IPV4_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    return NULL;
}
bool TCPIP_IPV4_PacketHandlerDeregister(TCPIP_IPV4_PROCESS_HANDLE pktHandle)
{
    return false;
}
#endif  // (M_TCPIP_IPV4_EXT_PKT_PROCESS != 0)


// calculates a TX packet priority queue based on the network interface
// precedence value should be the IPv4 header IPV4_TYPE_OF_SERVICE::precedence
static uint8_t TCPIP_IPV4_TxPriQueue(const TCPIP_NET_IF* pNetIf, uint8_t precedence)
{
    if(ipv4PriPktHandler != NULL)
    {
        return ipv4PriPktHandler((TCPIP_NET_HANDLE)pNetIf, precedence);
    }
    // default calculation
    uint16_t qNo = (uint16_t)TCPIPStack_TxPriNum(pNetIf);   // number of the MAC supported queues
    if(precedence > (uint8_t)TCPIP_IPV4_PRECEDENCE_MAX)
    {   // avoid overflow
        precedence = (uint8_t)TCPIP_IPV4_PRECEDENCE_MAX;
    }

    uint16_t pri16 = ((uint16_t)precedence * qNo) / (uint16_t)TCPIP_IPV4_PRECEDENCE_MAX; 
    return pri16 == 0U ? 0U : (uint8_t)pri16 - 1U;
}

bool TCPIP_IPV4_TxPriHandlerRegister(TCPIP_IPV4_TX_PRI_HANDLER priHandler)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv4PriPktHandler == NULL)
    {
        ipv4PriPktHandler = priHandler;
        res = true;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

bool TCPIP_IPV4_TxPriHandlerDeregister(TCPIP_IPV4_TX_PRI_HANDLER priHandler)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv4PriPktHandler == priHandler)
    {
        ipv4PriPktHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // defined(TCPIP_STACK_USE_IPV4)


