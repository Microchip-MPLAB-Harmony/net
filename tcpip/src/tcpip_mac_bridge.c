/*******************************************************************************
  TCP/IP MAC Layer 2

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    Implementation of a simple transparent Layer 2 bridge 
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





#include "tcpip/src/tcpip_private.h"


#if defined(TCPIP_STACK_USE_MAC_BRIDGE)

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_BRIDGE

#include "tcpip/tcpip_mac_bridge.h"
#include "tcpip/src/tcpip_mac_bridge_private.h"

// local prototypes
static void     F_MAC_Bridge_Cleanup(void);
static int      F_MAC_Bridge_SetPorts(MAC_BRIDGE_DCPT* pBDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pConfig);

static TCPIP_MAC_BRIDGE_RESULT      F_MAC_Bridge_SetPermEntries(MAC_BRIDGE_DCPT* pBDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pBConfig);
static TCPIP_MAC_BRIDGE_RESULT      F_MAC_Bridge_SetHostEntries(MAC_BRIDGE_DCPT* pDcpt);

static TCPIP_MAC_BRIDGE_RESULT      F_MAC_Bridge_SetFDBFromPerm(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* hE,  const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPerm);

static void     F_MAC_Bridge_ForwardPacket(TCPIP_MAC_PACKET* pFwdPkt, MAC_BRIDGE_HASH_ENTRY* hEntry);

static void     F_MAC_Bridge_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, TCPIP_MAC_PACKET* pBridgePkt, uint16_t pktLen, MAC_BRIDGE_HASH_ENTRY* hEntry);
static void     F_MAC_Bridge_PacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);

static void     F_MAC_Bridge_SetHashDynamicEntry(MAC_BRIDGE_HASH_ENTRY* hE, uint8_t port, uint16_t clrFlags, uint16_t setFlags);
static void     F_MAC_Bridge_SetHashStaticEntry(MAC_BRIDGE_HASH_ENTRY* hE, uint16_t stFlags, bool clearOutMap);

static size_t   F_MAC_BridgeHashKeyHash(OA_HASH_DCPT* pOH, const void* key);
static OA_HASH_ENTRY* F_MAC_BridgeHashEntryDelete(OA_HASH_DCPT* pOH);
static int      F_MAC_BridgeHashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void     F_MAC_BridgeHashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);

static void     F_MAC_Bridge_SetPacketForward(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* hDest, uint8_t inPort, MAC_BRIDGE_FWD_DCPT* pFDcpt);

static size_t   F_MAC_Bridge_AllocatePackets(size_t nPkts, SINGLE_LIST* addList, uint16_t pktSize, uint8_t allocFlags);

static size_t   F_MAC_Bridge_AllocateDescriptors(MAC_BRIDGE_DCPT* pBDcpt, size_t nDcpts, SINGLE_LIST* addList);

static TCPIP_MAC_PACKET* F_MAC_Bridge_GetFwdPkt(MAC_BRIDGE_DCPT* pBDcpt, uint16_t pktLen);

static MAC_BRIDGE_FWD_DCPT* F_MAC_Bridge_GetFwdDcpt(MAC_BRIDGE_DCPT* pBDcpt);


static int      F_MAC_Bridge_IfBridged(MAC_BRIDGE_DCPT* pDcpt, size_t netIx);

#if (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)
static void     F_MAC_Bridge_StatUpdate(MAC_BRIDGE_DCPT* bDcpt, MAC_BRIDGE_STAT_TYPE statType, uint32_t incUpdate);
static void     F_MAC_Bridge_StatPortUpdate(MAC_BRIDGE_DCPT* bDcpt, uint8_t port, BRIDGE_PORT_STAT_TYPE statType, uint32_t incUpdate);
#else
#define         F_MAC_Bridge_StatUpdate(bDcpt, statType, incUpdate)
#define         F_MAC_Bridge_StatPortUpdate(bDcpt, port, statType, incUpdate)
#endif  // (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)

#if (M_TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
static void     F_MAC_Bridge_NotifyEvent(MAC_BRIDGE_DCPT* pBDcpt, TCPIP_MAC_BRIDGE_EVENT event, const void* evParam);
#else
#define         F_MAC_Bridge_NotifyEvent(pBDcpt, event, evParam)
#endif  // (M_TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0)


#if defined (__PIC32MX__) || defined(__PIC32MZ__)
#define F_MAC_BridgeLeadingZeroes(value) __builtin_clz(value)
#else
static unsigned int F_MAC_BridgeLeadingZeroes(uint32_t value);
#endif

// definitions and local data
static int32_t         bridgeInitCount = 0;

static MAC_BRIDGE_DCPT  bridgeDcpt = { 0, 0 };
static MAC_BRIDGE_DCPT* gBridgeDcpt = NULL;

static const uint8_t bridge_reserved_address[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};

// implementation
//

// conversion functions/helpers
//
static __inline__ MAC_BRIDGE_HASH_ENTRY* __attribute__((always_inline)) FC_CVPtr2BridgeEntry(const void* cvPtr)
{
    union
    {
        const void* cvPtr;
        MAC_BRIDGE_HASH_ENTRY*  brHE;
    }U_CV_PTR_BRIDGE_E;

    U_CV_PTR_BRIDGE_E.cvPtr = cvPtr;
    return U_CV_PTR_BRIDGE_E.brHE;
}

static __inline__ MAC_BRIDGE_HASH_ENTRY* __attribute__((always_inline)) FC_HEntry2BridgeEntry(OA_HASH_ENTRY* hE)
{
    union
    {
        OA_HASH_ENTRY* hE;
        MAC_BRIDGE_HASH_ENTRY*  brHE;
    }U_HASH_E_BRIDGE_E;

    U_HASH_E_BRIDGE_E.hE = hE;
    return U_HASH_E_BRIDGE_E.brHE;
}

static __inline__ const uint8_t* __attribute__((always_inline)) FC_BridgeEntry2U8Ptr(const MAC_BRIDGE_HASH_ENTRY* brHE)
{
    union
    {
        const MAC_BRIDGE_HASH_ENTRY* brHE;
        const uint8_t*  pU8;
    }U_BRIDGE_E_U8_PTR;

    U_BRIDGE_E_U8_PTR.brHE = brHE;
    return U_BRIDGE_E_U8_PTR.pU8;
}

static __inline__ MAC_BRIDGE_FWD_DCPT* __attribute__((always_inline)) FC_SglNode2FwdDcpt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        MAC_BRIDGE_FWD_DCPT* pFwdDcpt;
    }U_SGL_NODE_FWD_DCPT;

    U_SGL_NODE_FWD_DCPT.node = node;
    return U_SGL_NODE_FWD_DCPT.pFwdDcpt;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_FwdDcpt2SglNode(MAC_BRIDGE_FWD_DCPT* pFwdDcpt)
{
    union
    {
        MAC_BRIDGE_FWD_DCPT* pFwdDcpt;
        SGL_LIST_NODE*  node;
    }U_FWD_DCPT_SGL_NODE;

    U_FWD_DCPT_SGL_NODE.pFwdDcpt = pFwdDcpt;
    return U_FWD_DCPT_SGL_NODE.node;
}

static __inline__ MAC_BRIDGE_FWD_DCPT** __attribute__((always_inline)) FC_U32Ptr2FwdDcptPP(uint32_t* pUi32)
{
    union
    {
        uint32_t*  pUi32;
        MAC_BRIDGE_FWD_DCPT** ppFwdDcpt;
    }U_UI32_FWD_DCPT_PP;

    U_UI32_FWD_DCPT_PP.pUi32 = pUi32;
    return U_UI32_FWD_DCPT_PP.ppFwdDcpt;
}

static __inline__ MAC_BRIDGE_DCPT* __attribute__((always_inline)) FC_BrHndl2BrDcpt(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    union
    {
        TCPIP_MAC_BRIDGE_HANDLE  brHandle;
        MAC_BRIDGE_DCPT* pDcpt;
    }U_BR_HNDL_BR_DCPT;

    U_BR_HNDL_BR_DCPT.brHandle = brHandle;
    return U_BR_HNDL_BR_DCPT.pDcpt;
}

static __inline__ const TCPIP_MAC_BRIDGE_ENTRY_ASCII* __attribute__((always_inline)) FC_BridgeTbl2AscEntry(const TCPIP_MAC_BRIDGE_ENTRY* bridgeTable)
{
    return &bridgeTable->entryAscii;
}

static __inline__ const TCPIP_MAC_BRIDGE_ENTRY_BIN* __attribute__((always_inline)) FC_BridgeTbl2BinEntry(const TCPIP_MAC_BRIDGE_ENTRY* bridgeTable)
{
    return &bridgeTable->entryBin;
}


#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_BASIC) != 0)
static volatile int v_MacBridgeStayAssertLoop = 0;
static void F_Mac_Bridge_AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("Bridge Assert: %s, in line: %d, \r\n", message, lineNo);
        while(v_MacBridgeStayAssertLoop != 0)
        {
        }
    }
}

#else
#define F_Mac_Bridge_AssertCond(cond, message, lineNo)
#endif  // (MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_BASIC)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_INIT) != 0)
static const char* c_init_default_message = "MAC Bridge initialization failed: ";
static void F_Mac_Bridge_InitCond(const char* message, TCPIP_MAC_BRIDGE_RESULT resCode)
{
    if (message == NULL)
    {
        message = c_init_default_message; 
    }

    SYS_ERROR_PRINT(SYS_ERROR_ERROR, "%s, %d\r\n", message, resCode);
}
#else
#define F_Mac_Bridge_InitCond(message, resCode)
#endif  // (MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_INIT)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_FDB_INTEGRITY) != 0)
static TCPIP_MAC_ADDR hashTrace[TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES];

static void F_MAC_Bridge_CheckFDB(MAC_BRIDGE_DCPT* pDcpt)
{
    static uint32_t hashDupl = 0;
    size_t ix, jx;
    MAC_BRIDGE_HASH_ENTRY* hE;

    (void)memset(hashTrace, 0, sizeof(hashTrace));
    size_t traceIx = 0;
    TCPIP_MAC_ADDR* pTrace = hashTrace;

    size_t nEntries = pDcpt->hashDcpt->hEntries;
    for(ix = 0; ix < nEntries; ix++)
    {
        hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryGet(pDcpt->hashDcpt, ix));
        if(hE->hEntry.flags.busy != 0U)
        {   // in use entry
            (void)memcpy(pTrace->v, hE->destAdd.v, sizeof(*pTrace));
            pTrace++;
            traceIx++;
        }
    }

    // now check integrity
    if(traceIx != 0U)
    {
        pTrace = hashTrace;
        for(ix = 0; ix < traceIx - 1U; ix++)
        {
            TCPIP_MAC_ADDR* pNext = pTrace + 1;
            for(jx = ix + 1U; jx < traceIx; jx++)
            {
                if(memcmp(pTrace->v, pNext->v, 6) == 0)
                {   // ooops! duplication
                    hashDupl++;
                    F_Mac_Bridge_AssertCond(false, __func__, __LINE__);
                    return;
                }
                pNext++;
            } 
            pTrace++;
        }
    }

}

#else
#define F_MAC_Bridge_CheckFDB(pDcpt)
#endif // ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_FDB_INTEGRITY) != 0)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_RX_IP_CHECK) != 0)
static uint16_t checkRxUdpSrcPort = 67U; 
static uint16_t checkRxUdpDstPort = 68U; 

static uint16_t checkRxTcpSrcPort = 9760U; 
static uint16_t checkRxTcpDstPort = 9760U; 

static uint32_t checkRxArpTarget = 0xc701a8c0U; 

static uint32_t checkRxUdpBkptCnt = 0U;
static uint32_t checkRxIcmpBkptCnt = 0U;
static uint32_t checkRxTcpBkptCnt = 0U;
static uint32_t checkRxArpBkptCnt = 0U;

typedef struct __attribute__((aligned(2), packed))
{
    uint16_t    HardwareType; 
    uint16_t    Protocol;
    uint8_t     MACAddrLen;
    uint8_t     ProtocolLen;
    uint16_t    Operation;
    TCPIP_MAC_ADDR    SenderMACAddr;
    uint32_t    SenderIPAddr; 
    TCPIP_MAC_ADDR    TargetMACAddr; 
    uint32_t    TargetIPAddr; 
} S_MAC_BRIDGE_CHECK_ARP_PACKET;

static __inline__ S_MAC_BRIDGE_CHECK_ARP_PACKET* __attribute__((always_inline)) FC_Uptr2BridgeArpPkt(uint8_t* uptr)
{
    union
    {
        uint8_t*    uptr;
        S_MAC_BRIDGE_CHECK_ARP_PACKET* pArpPkt;
    }U_UPTR_BRIDGE_ARP_PKT;

    U_UPTR_BRIDGE_ARP_PKT.uptr = uptr;
    return U_UPTR_BRIDGE_ARP_PKT.pArpPkt;
}

static void F_MAC_Bridge_CheckRxIpPkt(TCPIP_MAC_PACKET* pRxPkt)
{
    TCPIP_MAC_ETHERNET_HEADER*  pMacHdr = FC_Uptr2MacEthHdr(pRxPkt->pMacLayer);
    uint16_t frameType = TCPIP_Helper_ntohs(pMacHdr->Type);
    if(frameType ==  0x0806U)
    {
        S_MAC_BRIDGE_CHECK_ARP_PACKET* pArpPkt = FC_Uptr2BridgeArpPkt(pRxPkt->pNetLayer);
        if(pArpPkt->TargetIPAddr == checkRxArpTarget)
        { 
            checkRxArpBkptCnt++;
            return;
        }
    }

    IPV4_HEADER* pHeader = FC_Uptr2IpHdr(pRxPkt->pNetLayer);
    if(pHeader->Protocol == (uint8_t)IP_PROT_ICMP)
    {
        checkRxIcmpBkptCnt++;
    }
    else if(pHeader->Protocol == (uint8_t)IP_PROT_UDP)
    {   // UDP packet
        uint8_t headerLen = pHeader->IHL << 2;
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
#define F_MAC_Bridge_CheckRxIpPkt(pRxPkt)
#endif  // ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_RX_IP_CHECK) != 0)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_TRACE_PKT) != 0)
static TCPIP_MAC_ADDR traceAddArray[] =
{
    {{0x00, 0x04, 0x25, 0x1c, 0xa0, 0x02}},
    {{0xc8, 0x5b, 0x76, 0x1d, 0xa9, 0x2d}},
};

static void F_MAC_Bridge_TracePkt(TCPIP_MAC_PACKET* pPkt, uint8_t brPort, bool isTx)
{
    static uint16_t breakType = 0x0608U;
    size_t ix;

    TCPIP_MAC_ETHERNET_HEADER*  pMacHdr = FC_Uptr2MacEthHdr(pPkt->pMacLayer);
    const TCPIP_MAC_ADDR* pSrcAdd = &pMacHdr->SourceMACAddr;
    const TCPIP_MAC_ADDR* pDestAdd = &pMacHdr->DestMACAddr;

    const TCPIP_MAC_ADDR* pTraceAdd = traceAddArray;
    for(ix = 0; ix < sizeof(traceAddArray) / sizeof(*traceAddArray); ix++)
    {
        if(memcmp(pDestAdd->v, pTraceAdd->v, sizeof(*pDestAdd)) == 0 || 
           memcmp(pSrcAdd->v, pTraceAdd->v, sizeof(*pSrcAdd)) == 0)
        {
            char srcAddBuff[20], dstAddBuff[20];

            (void)TCPIP_Helper_MACAddressToString(pDestAdd, dstAddBuff, sizeof(dstAddBuff));
            (void)TCPIP_Helper_MACAddressToString(pSrcAdd, srcAddBuff, sizeof(srcAddBuff));

            uint16_t pktType = pMacHdr->Type;
            uint16_t pktLen = pPkt->pDSeg->segLen;
            SYS_CONSOLE_PRINT("Trace- %s pkt from: %s to %s, pktType: 0x%04x, pktLen: %d, brPort: %d\r\n", isTx ? "out" : "in", srcAddBuff, dstAddBuff, pktType, pktLen, brPort); 
            if(breakType == pktType)
            {
                breakType = pktType;
            }
            break;
        }
        pTraceAdd++;
    }
}
#else
#define F_MAC_Bridge_TracePkt(pPkt, brPort, isRx)
#endif  // ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_TRACE_PKT) != 0)

// rough bridge time keeping
static uint32_t  bridgeSecCount = 0U; 
static uint32_t F_MAC_Bridge_UpdateSecond(void)
{
    static uint32_t sysFreq = 0U;

    if(sysFreq == 0U)
    {
        sysFreq = SYS_TIME_FrequencyGet();
    }

    if(sysFreq != 0U)
    {
        bridgeSecCount = (uint32_t)(SYS_TIME_Counter64Get() / sysFreq); 
    }

    return bridgeSecCount;
}

static __inline__ uint32_t __attribute__((always_inline))  F_MAC_Bridge_GetSecond(void)
{
    return bridgeSecCount;
}

// clear/empty the forwarding map
static __inline__ void __attribute__((always_inline))  F_MAC_Bridge_ClearMap(MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    pFDcpt->fwdMap32 = 0U;
}

// check if the forwarding map is not empty
static __inline__ bool __attribute__((always_inline))  F_MAC_Bridge_IsMapEmpty(const MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    return pFDcpt->fwdMap32 == 0U;
}

// check if port is part of the forwarding map
static __inline__ bool __attribute__((always_inline))  F_MAC_Bridge_IsPortMapped(uint8_t inPort, const MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    return ((1UL << inPort) & pFDcpt->fwdMap32) != 0U;
}

// remove port from forwarding map
static __inline__ void __attribute__((always_inline))  F_MAC_Bridge_DelPortMap(uint8_t inPort, MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    pFDcpt->fwdMap32 &= ~(1UL << inPort);
}

// add port to the forwarding map
static __inline__ void __attribute__((always_inline))  F_MAC_Bridge_AddPortMap(uint8_t portIx, MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    pFDcpt->fwdMap32 |= 1UL << portIx;
}

// return a processing port number
// < 0 if none
static __inline__ int __attribute__((always_inline))  F_MAC_Bridge_PortFromMap(const MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    if(pFDcpt->fwdMap32 != 0U)
    {
        return 31 - (int)F_MAC_BridgeLeadingZeroes(pFDcpt->fwdMap32);
    }
    return -1;
}

static __inline__ MAC_BRIDGE_DCPT* __attribute__((always_inline)) F_MAC_Bridge_ValidateHandle(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    MAC_BRIDGE_DCPT* pDcpt = FC_BrHndl2BrDcpt(brHandle);
    return (pDcpt == gBridgeDcpt) ? pDcpt : NULL;
}

// sets the flags of the packet
// since this is either the bridge copy or the host does not process it
// we use the pktClientData16[0] for flags
static __inline__ void __attribute__((always_inline)) F_MAC_Bridge_SetPktFlags(TCPIP_MAC_PACKET* pPkt, uint16_t flags)
{
    pPkt->pktClientData16[0] = flags;
}

// returns the flags of the packet
static __inline__ uint16_t __attribute__((always_inline)) F_MAC_Bridge_GetPktFlags(TCPIP_MAC_PACKET* pPkt)
{
    return pPkt->pktClientData16[0];
}

// stores the forward descriptor in the packet
// since this is either the bridge copy or the host does not process it
// we use the pktClientData32[1] for storing the descriptor
static __inline__ void __attribute__((always_inline)) F_MAC_Bridge_StorePktFwdDcpt(TCPIP_MAC_PACKET* pPkt, MAC_BRIDGE_FWD_DCPT* pFwdDcpt)
{
    MAC_BRIDGE_FWD_DCPT** pStoreDcpt = FC_U32Ptr2FwdDcptPP(pPkt->pktClientData32 + 1);
    // should be properly aligned
    F_Mac_Bridge_AssertCond( ((uintptr_t)pStoreDcpt & (sizeof(uintptr_t) - 1U)) == 0U, __func__, __LINE__);
    
    *pStoreDcpt = pFwdDcpt;
}

// returns the forward descriptor of the packet
static __inline__ MAC_BRIDGE_FWD_DCPT* __attribute__((always_inline)) F_MAC_Bridge_GetPktFwdDcpt(TCPIP_MAC_PACKET* pPkt)
{
    MAC_BRIDGE_FWD_DCPT** pStoreDcpt = FC_U32Ptr2FwdDcptPP(pPkt->pktClientData32 + 1);
    // should be properly aligned
    F_Mac_Bridge_AssertCond( ((uintptr_t)pStoreDcpt & (sizeof(uintptr_t) - 1U)) == 0U, __func__, __LINE__);
    
    return *pStoreDcpt;
}


#if (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0) 
static __inline__ MAC_BRIDGE_DCPT* __attribute__((always_inline)) F_MAC_Bridge_DcptFromHandle(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    return FC_BrHndl2BrDcpt(brHandle);
}
static TCPIP_MAC_BRIDGE_RESULT F_MAC_Bridge_FDBLock(TCPIP_MAC_BRIDGE_HANDLE brHandle, bool validate);
static void F_MAC_Bridge_FDBUnlock(MAC_BRIDGE_DCPT* pDcpt);
#endif  // (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0) 



bool TCPIP_MAC_Bridge_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart; reset the interface as being bridge if needed
        if(gBridgeDcpt != NULL)
        {
            int brPort = F_MAC_Bridge_IfBridged(gBridgeDcpt, stackCtrl->netIx);
            if(brPort >= 0)
            {
                TCPIPStack_BridgeSetIf(stackCtrl->pNetIf, (uint8_t)brPort);
            }
        }
        return true;
    }

    // stack init
    while(bridgeInitCount == 0)
    {   // first time we run
        // basic sanity check
        
        if(initData == NULL)
        {   // missing initialization data
            F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_NO_INIT_DATA);
            return false;
        }

        const TCPIP_MAC_BRIDGE_CONFIG* pBConfig = (const TCPIP_MAC_BRIDGE_CONFIG*)initData;
        if(pBConfig->fdbEntries == 0U || pBConfig->bridgeTable == NULL || pBConfig->bridgeTableSize == 0U || pBConfig->bridgeTableSize > (size_t)TCPIP_MAC_BRIDGE_MAX_PORTS_NO
                            || pBConfig->bridgeTableSize > (size_t)MAC_BRIDGE_MAX_SUPPORTED_PORTS || (pBConfig->pktPoolSize != 0U && pBConfig->pktSize == 0U) || pBConfig->dcptPoolSize == 0U)
        {   // wrong initialization data
            F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_INIT_DATA_ERROR);
            return false;
        }

        // unbridge all the network interfaces. just in case the stack is restarting
        size_t ix;
        size_t nIfs = TCPIP_STACK_NumberOfNetworksGet();
        TCPIP_NET_IF*   pNetIf;
        for(ix = 0; ix < nIfs; ix++)
        {
            pNetIf = TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(ix));
            TCPIPStack_BridgeClearIf(pNetIf);
        }


        gBridgeDcpt = NULL;
        (void)memset(&bridgeDcpt, 0, sizeof(bridgeDcpt));
        int nPorts = F_MAC_Bridge_SetPorts(&bridgeDcpt, pBConfig);
        if(nPorts < 0)
        {   // failed, wrong initialization data
            F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_INIT_PORT_ERROR);
            return false;
        }

        bridgeDcpt.nPorts = (uint8_t)nPorts;

        // create the FDB
        bridgeDcpt.memH = stackCtrl->memH;
        size_t hashMemSize = sizeof(OA_HASH_DCPT) + pBConfig->fdbEntries * sizeof(MAC_BRIDGE_HASH_ENTRY);
        OA_HASH_DCPT* hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(bridgeDcpt.memH, hashMemSize);

        if(hashDcpt == NULL)
        {   // failed
            F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_FDB_ALLOC_ERROR);
            return false;
        }

        // populate the entries
        hashDcpt->memBlk = hashDcpt + 1;
        hashDcpt->hParam = NULL;
        hashDcpt->hEntrySize = sizeof(MAC_BRIDGE_HASH_ENTRY);
        hashDcpt->hEntries = pBConfig->fdbEntries;
        hashDcpt->probeStep = MAC_BRIDGE_HASH_PROBE_STEP;

        hashDcpt->hashF = &F_MAC_BridgeHashKeyHash;
        hashDcpt->delF = &F_MAC_BridgeHashEntryDelete;
        hashDcpt->cmpF = &F_MAC_BridgeHashKeyCompare;
        hashDcpt->cpyF = &F_MAC_BridgeHashKeyCopy; 
        TCPIP_OAHASH_Initialize(hashDcpt);

        bridgeDcpt.hashDcpt = hashDcpt;
        TCPIP_MAC_BRIDGE_RESULT resPerm = F_MAC_Bridge_SetPermEntries(&bridgeDcpt, pBConfig);
        if(resPerm != TCPIP_MAC_BRIDGE_RES_OK) 
        {
            F_Mac_Bridge_InitCond(NULL, resPerm);
            F_MAC_Bridge_Cleanup();
            return false;
        }
        
        uint32_t purgeTmo = pBConfig->purgeTimeout != 0U ? pBConfig->purgeTimeout : (uint32_t)TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT; 
        if(purgeTmo > 0xffffffffU / 2U)
        {   // requested timeout too great
            F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_TIMEOUT_ERROR);
            F_MAC_Bridge_Cleanup();
            return false;
        }
        bridgeDcpt.purgeTmo = purgeTmo;
        bridgeDcpt.transitDelay = (uint32_t)pBConfig->transitDelay;

        bridgeDcpt.pktPoolSize = pBConfig->pktPoolSize;
        bridgeDcpt.pktSize = pBConfig->pktSize;
        bridgeDcpt.pktReplenish = pBConfig->pktReplenish;
        bridgeDcpt.dcptReplenish = pBConfig->dcptReplenish;
        bridgeDcpt.bridgeFlags = pBConfig->bridgeFlags;

        TCPIP_Helper_SingleListInitialize(&bridgeDcpt.pktPool);
        TCPIP_Helper_SingleListInitialize(&bridgeDcpt.dcptPool);

        if(bridgeDcpt.pktPoolSize != 0U)
        {   // allocate packets
            size_t nAllocPkts = F_MAC_Bridge_AllocatePackets(bridgeDcpt.pktPoolSize, &bridgeDcpt.pktPool, bridgeDcpt.pktSize, 
                    (uint8_t)MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN | (uint8_t)MAC_BRIDGE_ALLOC_FLAG_STICKY);

            if(nAllocPkts != bridgeDcpt.pktPoolSize)
            {
                F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_PKT_ALLOC_ERROR);
                F_MAC_Bridge_Cleanup();
                return false;
            }
        }
#if (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)
        bridgeDcpt.stat.pktPoolLowSize = bridgeDcpt.pktPoolSize; 
#endif    
        // allocate descriptors
        size_t nAllocDcpts = F_MAC_Bridge_AllocateDescriptors(&bridgeDcpt, pBConfig->dcptPoolSize, &bridgeDcpt.dcptPool);

        if(nAllocDcpts != pBConfig->dcptPoolSize)
        {
            F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_DCPT_ALLOC_ERROR);
            F_MAC_Bridge_Cleanup();
            return false;
        }
#if (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)
        bridgeDcpt.stat.dcptPoolLowSize = pBConfig->dcptPoolSize; 
#endif   
        bridgeDcpt.tmrSigHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_MAC_Bridge_Task, TCPIP_MAC_BRIDGE_TASK_RATE);
        if(bridgeDcpt.tmrSigHandle == NULL)
        {
            F_Mac_Bridge_InitCond(NULL, TCPIP_MAC_BRIDGE_RES_SIGNAL_ERROR);
            F_MAC_Bridge_Cleanup();
            return false;
        }
        
        bridgeDcpt.status = (int8_t)SYS_STATUS_BUSY;    // waiting to complete the initialization
        gBridgeDcpt = &bridgeDcpt;

        break;
    }


    bridgeInitCount++;
    return true;

}

TCPIP_MAC_BRIDGE_HANDLE TCPIP_MAC_Bridge_Open(const SYS_MODULE_INDEX bridgeIndex)
{
    if(bridgeIndex == 0U)
    {
        return gBridgeDcpt;
    }

    return NULL;
}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_MAC_Bridge_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN)
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down

    if(bridgeInitCount > 0)
    {   // we're up and running
        // interface is going down one way or another
        // we do NOT remove the interface from the bridge:
        //  - for an input interface, a dead interface should not receive packets anyway
        //  - for an output interface, the bridge checks it to be up and running when sending packets 

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--bridgeInitCount == 0)
            {   // all closed
                // release resources
                F_MAC_Bridge_Cleanup();
            }
        }
    }

}

static void F_MAC_Bridge_Cleanup(void)
{
    if(bridgeDcpt.tmrSigHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(bridgeDcpt.tmrSigHandle);
        bridgeDcpt.tmrSigHandle = NULL;
    }

    if(bridgeDcpt.hashDcpt != NULL)
    {
        TCPIP_OAHASH_EntriesRemoveAll(bridgeDcpt.hashDcpt);

        (void)TCPIP_HEAP_Free(bridgeDcpt.memH, bridgeDcpt.hashDcpt);
        bridgeDcpt.hashDcpt = NULL;
    }

    TCPIP_MAC_PACKET* pBPkt;
    while( (pBPkt = FC_SglNode2MacPkt(TCPIP_Helper_SingleListHeadRemove(&bridgeDcpt.pktPool))) != NULL)
    {
        TCPIP_PKT_PacketFree(pBPkt);
    }

    MAC_BRIDGE_FWD_DCPT* pFwdDcpt;
    while( (pFwdDcpt = FC_SglNode2FwdDcpt(TCPIP_Helper_SingleListHeadRemove(&bridgeDcpt.dcptPool))) != NULL)
    {
        (void)TCPIP_HEAP_Free(bridgeDcpt.memH, pFwdDcpt);
    }

    gBridgeDcpt = NULL;
    bridgeInitCount = 0;

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// bridge packet processing
// pkt arriving on port_i
// no forward on port_i !
//
//  - source is not multicast:
//      - learning:
//          - source learn only if NO static entry or the static entry does NOT contain mandatory filter/forward for this learnPort == outPort
//            however, we update anyway because the static entry takes precedence
//
// - destination multicast:
//      there should not be an dynamic entry for mcast addresses!
//
//      - dst exists in FDB static:
//          - outPortMap[port_i][port1, 2, ...n-1] give the behavior on the other port_x:
//              - default/forward: forward on port_x
//              - filter: do not forward on port_x
//          - outPortMap[port_i][port_i] gives behavior for this host:
//              - default/forward: process on this host
//              - filter: discard processing
//
//      - dist NOT in FDB static:              
//          - forward on all other ports (flood)
//          - process on this host
//
// - destination unicast:
//      there could be a dynamic entry for this ucast address
//
//      - dst exists in FDB static:
//
//          - dst is NOT one of the host MAC addresses:
//              - outPortMap[port_i][port1, 2, ...n-1] give the behavior on the other port_x:
//                  - default:
//                              if there is a learnPort (valid) then default is:
//                                  - forward on port_x == learnPort, filter on other ports
//                              else default is forward on port_x
//                  - forward: forward on port_x
//                  - filter: do not forward on port_x
//              - outPortMap[port_i][port_i] gives behavior for this host:
//                  - forward: process on this host
//                  - default/filter: discard processing
//                  - 
//          - dst is one of the host MAC addresses:
//            (and the entry exists always!)
//              - outPortMap[port_i][port1, 2, ...n-1] give the behavior on the other port_x:
//                  - default:  filter
//                  - forward: forward on port_x
//                  - filter: do not forward on port_x
//              - outPortMap[port_i][port_i] gives behavior for this host:
//                  - default/forward: process on this host
//                  - filter: discard processing
//              
//      - dst NOT in FDB static:
//          - dst exists in FDB dynamic:
//              - forward to learnPort only
//              - host: filter
//          - dst NOT in FDB dynamic:
//              - flood to all ports
//              - host: filter
//              
TCPIP_MAC_BRIDGE_PKT_RES TCPIP_MAC_Bridge_ProcessPacket(TCPIP_MAC_PACKET* pRxPkt)
{
    MAC_BRIDGE_HASH_ENTRY* heSrc, *heDest;
    TCPIP_MAC_PACKET* pFwdPkt;
    TCPIP_MAC_BRIDGE_EVENT brEvent;
    MAC_BRIDGE_FWD_DCPT *pFwdDcpt;
    MAC_BRIDGE_FWD_DCPT fwdDcpt = {0};

    const TCPIP_NET_IF* pInIf = (const TCPIP_NET_IF*)pRxPkt->pktIf;

    if(gBridgeDcpt == NULL || TCPIPStack_BridgeCheckIf(pInIf) == false)
    {   // not for us
        return TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS;
    }

#if (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    TCPIP_MAC_BRIDGE_RESULT res = F_MAC_Bridge_FDBLock(gBridgeDcpt, false);
    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {   // couldn't get a lock
        F_MAC_Bridge_NotifyEvent(gBridgeDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK, NULL);
        F_MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_FAIL_LOCK, 1);
        return TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS;
    }
#endif  // (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)



    uint8_t inPort = TCPIPStack_BridgeGetIfPort(pInIf);
    F_MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, BRIDGE_PORT_STAT_RX_PKTS, 1);
    F_MAC_Bridge_TracePkt(pRxPkt, inPort, false);

    // learn
    TCPIP_MAC_ETHERNET_HEADER*  pMacHdr = FC_Uptr2MacEthHdr(pRxPkt->pMacLayer);
    heSrc = NULL;
    if(!TCPIP_Helper_IsMcastMACAddress(&pMacHdr->SourceMACAddr))
    {   // we shouldn't learn multicast addresses
        // update destination in the FDB
        F_MAC_Bridge_CheckFDB(gBridgeDcpt);
        brEvent = TCPIP_MAC_BRIDGE_EVENT_NONE;
        if((gBridgeDcpt->bridgeFlags & (uint8_t)TCPIP_MAC_BRIDGE_FLAG_NO_DYNAMIC_LEARN) != 0U)
        {
            heSrc = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryLookup(gBridgeDcpt->hashDcpt, pMacHdr->SourceMACAddr.v));
        }
        else
        {
            heSrc = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryLookupOrInsert(gBridgeDcpt->hashDcpt, pMacHdr->SourceMACAddr.v));
            if(heSrc == NULL)
            {
                brEvent = TCPIP_MAC_BRIDGE_EVENT_FDB_FULL;
                F_MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_FDB_FULL, 1);
            }
        }

        if(heSrc != NULL)
        {   // (new) dynamic/learnt entry needs to be updated
            F_MAC_Bridge_SetHashDynamicEntry(heSrc, inPort, (uint16_t)MAC_BRIDGE_HFLAG_NONE, (uint16_t)MAC_BRIDGE_HFLAG_PORT_VALID); 
            if(heSrc->hEntry.flags.newEntry != 0U)
            {
                brEvent = TCPIP_MAC_BRIDGE_EVENT_ENTRY_ADDED;
            }
        }

        if(brEvent != TCPIP_MAC_BRIDGE_EVENT_NONE)
        {
            F_MAC_Bridge_NotifyEvent(gBridgeDcpt, brEvent, pMacHdr->SourceMACAddr.v);
        }
        F_MAC_Bridge_CheckFDB(gBridgeDcpt);
    }

    // check if destination is a reserved address
    // IEEE 802.1D-2004 - 7.12.6:
    //      Frames containing any of the group MAC Addresses specified in Table 7-10
    //      in their destination address field shall not be relayed by the Bridge.
    //      They are configured in the Permanent Database:
    //      01-80-C2-00-00-00 - 01-80-C2-00-00-0F
    //      We test directly for these addresses without taking slots in the FDB

    if(memcmp(pMacHdr->DestMACAddr.v, bridge_reserved_address, sizeof(bridge_reserved_address) - 1U) == 0)
    {
        if((pMacHdr->DestMACAddr.v[5] & 0xf0U) == 0x00U)
        {   // reserved address
            F_MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, BRIDGE_PORT_STAT_RESERVED_PKTS, 1);
#if (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
            F_MAC_Bridge_FDBUnlock(gBridgeDcpt);
#endif // (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
            TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_BRIDGE_DISCARD); 
            return TCPIP_MAC_BRIDGE_PKT_RES_BR_DISCARD;
        }
    } 
            
    // debug check point
    F_MAC_Bridge_CheckRxIpPkt(pRxPkt);

    // forward
    fwdDcpt.tReceive = F_MAC_Bridge_GetSecond();
    fwdDcpt.pktLen = TCPIP_PKT_PayloadLen(pRxPkt);
    

    // check destination is in the FDB
    F_MAC_Bridge_CheckFDB(gBridgeDcpt);
    heDest = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryLookup(gBridgeDcpt->hashDcpt, pMacHdr->DestMACAddr.v));

    if(TCPIP_Helper_IsMcastMACAddress(&pMacHdr->DestMACAddr))
    {
        F_Mac_Bridge_AssertCond(heDest == NULL || (heDest->hEntry.flags.value & ((uint16_t)MAC_BRIDGE_HFLAG_STATIC | (uint16_t)MAC_BRIDGE_HFLAG_HOST)) == (uint16_t)MAC_BRIDGE_HFLAG_STATIC, __func__, __LINE__);
        fwdDcpt.fwdFlags = (uint16_t)MAC_BRIDGE_FWD_FLAG_MCAST;
        F_MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, BRIDGE_PORT_STAT_RX_DEST_MCAST, 1);
    }
    else
    {
        fwdDcpt.fwdFlags = (uint16_t)MAC_BRIDGE_FWD_FLAG_NONE;
        F_MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, (heDest == NULL || (heDest->hEntry.flags.value & MAC_BRIDGE_HFLAG_HOST) == 0U) ? BRIDGE_PORT_STAT_RX_DEST_NOT_ME_UCAST : BRIDGE_PORT_STAT_RX_DEST_ME_UCAST, 1);
    } 

    TCPIP_MAC_BRIDGE_PKT_RES pktRes = TCPIP_MAC_BRIDGE_PKT_RES_BR_PROCESS; 
    F_MAC_Bridge_ClearMap(&fwdDcpt);

    if((gBridgeDcpt->bridgeFlags & (uint8_t)TCPIP_MAC_BRIDGE_FLAG_FDB_ENTRY_EXISTS) == 0U || (heDest != NULL || heSrc != NULL))
    {   // regular forwarding or only if we have the src/dest in FDB
        F_MAC_Bridge_SetPacketForward(gBridgeDcpt, heDest, inPort, &fwdDcpt);
    }

    if(!F_MAC_Bridge_IsMapEmpty(&fwdDcpt))
    {   // check if the host processes this packet
        if(F_MAC_Bridge_IsPortMapped(inPort, &fwdDcpt))
        {
            pktRes = TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS;
            F_MAC_Bridge_DelPortMap(inPort, &fwdDcpt); // remove incoming port from forwarding
        }
    }

    if(!F_MAC_Bridge_IsMapEmpty(&fwdDcpt))
    {   // packet must be forwarded on some ports 

        pFwdDcpt = F_MAC_Bridge_GetFwdDcpt(gBridgeDcpt);
        if(pFwdDcpt != NULL)
        {   // have valid descriptor
            if(pktRes == TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS)
            {   // host needs to process this packet; we need a copy
                pFwdPkt = F_MAC_Bridge_GetFwdPkt(gBridgeDcpt, fwdDcpt.pktLen);
                if(pFwdPkt != NULL)
                {   // ok, we can flood/forward
                    F_MAC_Bridge_PacketCopy(pRxPkt, pFwdPkt, fwdDcpt.pktLen, heDest);
                }
            }
            else
            {   // use directly the incoming packet
                pFwdPkt = pRxPkt;
                F_MAC_Bridge_SetPktFlags(pFwdPkt, (uint16_t)MAC_BRIDGE_ALLOC_FLAG_NONE);
            }

            if(pFwdPkt != NULL)
            {   // start forwarding away...
                // set the descriptor
                *pFwdDcpt = fwdDcpt;
                pFwdDcpt->ownAckFunc = pRxPkt->ackFunc;
                pFwdDcpt->ownAckParam = pRxPkt->ackParam;
                F_MAC_Bridge_StorePktFwdDcpt(pFwdPkt, pFwdDcpt);

                pFwdPkt->ackFunc = &F_MAC_Bridge_PacketAck; 
                pFwdPkt->ackParam = heDest; 
                // adjust: the source packet comes from the MAC driver with segLen adjusted
                pFwdPkt->pDSeg->segLen += (uint16_t)sizeof(TCPIP_MAC_ETHERNET_HEADER);

                F_MAC_Bridge_ForwardPacket(pFwdPkt, heDest);
            }
        }
    }
    else
    {
        if (pktRes != TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS)
        {
            // If forwarding rejected (too large for example) and host isn't going to process
            TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_BRIDGE_DISCARD); 
            pktRes = TCPIP_MAC_BRIDGE_PKT_RES_BR_DISCARD;
        }
    }

#if (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    F_MAC_Bridge_FDBUnlock(gBridgeDcpt);
#endif // (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)

    return pktRes;
}

// periodic task
// purge the dynamic entries in the FDB
void TCPIP_MAC_Bridge_Task(void)
{
    if(gBridgeDcpt == NULL || gBridgeDcpt->status < 0)
    {   // not up or error
        return;
    }

    if(gBridgeDcpt->status == (int8_t)SYS_STATUS_BUSY)
    {   // try to complete the initialization
        // get the stack handle; it should be running
        SYS_MODULE_OBJ stackObj = TCPIP_STACK_Initialize(0, NULL);
        SYS_STATUS stackStat = TCPIP_STACK_Status(stackObj);
        if((int)stackStat < 0)
        {   // some error has occurred
            gBridgeDcpt->status = (int8_t)SYS_STATUS_ERROR;
            return;
        }
        else if(stackStat != SYS_STATUS_READY)
        {   // wait some more
           return;
        }
        else
        {
            // proceed
        }

        // proceed with the initialization
        TCPIP_MAC_BRIDGE_RESULT resHost = F_MAC_Bridge_SetHostEntries(gBridgeDcpt);
        if(resHost != TCPIP_MAC_BRIDGE_RES_OK)
        {   // some error has occurred
            gBridgeDcpt->status = (int8_t)SYS_STATUS_ERROR;
            return;
        }

        // all good
        gBridgeDcpt->status = (int8_t)SYS_STATUS_READY;
    }

    uint32_t currSec = F_MAC_Bridge_UpdateSecond();

#if (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    TCPIP_MAC_BRIDGE_RESULT res = F_MAC_Bridge_FDBLock(gBridgeDcpt, false);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {   // couldn't get a lock
        F_MAC_Bridge_NotifyEvent(gBridgeDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK, NULL);
        F_MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_FAIL_LOCK, 1);
        return;
    }
#endif  // (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)

    // traverse the FDB for periodic maintenance
    size_t ix;
    MAC_BRIDGE_HASH_ENTRY* hE;
    size_t nEntries = gBridgeDcpt->hashDcpt->hEntries;

    F_MAC_Bridge_CheckFDB(gBridgeDcpt);
    for(ix = 0; ix < nEntries; ix++)
    {
        hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryGet(gBridgeDcpt->hashDcpt, ix));
        if(hE->hEntry.flags.busy != 0U && (hE->hEntry.flags.value & (uint16_t)MAC_BRIDGE_HFLAG_STATIC ) == 0U)
        {   // in use dynamic entry
            if(FC_Ui322I32(currSec - hE->tExpire) > 0)
            {   // expired entry; remove
                F_MAC_Bridge_NotifyEvent(gBridgeDcpt, TCPIP_MAC_BRIDGE_EVENT_ENTRY_EXPIRED, hE->destAdd.v);
                TCPIP_OAHASH_EntryRemove(gBridgeDcpt->hashDcpt, &hE->hEntry);
            }
        }
    }
    F_MAC_Bridge_CheckFDB(gBridgeDcpt);

#if (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    F_MAC_Bridge_FDBUnlock(gBridgeDcpt);
#endif // (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
}

SYS_STATUS TCPIP_MAC_Bridge_Status(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    MAC_BRIDGE_DCPT* bDcpt = F_MAC_Bridge_ValidateHandle(brHandle);

    return bDcpt == NULL ? SYS_STATUS_UNINITIALIZED : (SYS_STATUS)bDcpt->status;
}

// low level and local functionality implementation

// allocate n bridge packets of a required size
static size_t F_MAC_Bridge_AllocatePackets(size_t nPkts, SINGLE_LIST* addList, uint16_t pktSize, uint8_t allocFlags)
{
    size_t ix;
    TCPIP_MAC_PACKET* pBPkt;

    for(ix = 0; ix < nPkts; ix++)
    { 
        pBPkt = (TCPIP_MAC_PACKET*)TCPIP_PKT_PacketAlloc( (uint16_t)sizeof(*pBPkt), pktSize, TCPIP_MAC_PKT_FLAG_NONE);
        if(pBPkt == NULL)
        {   // out of mem
            break;
        }

        F_MAC_Bridge_SetPktFlags(pBPkt, (uint16_t)allocFlags);
        TCPIP_Helper_SingleListTailAdd(addList, FC_MacPkt2SglNode(pBPkt)); 
    }

    return ix;
}

static size_t F_MAC_Bridge_AllocateDescriptors(MAC_BRIDGE_DCPT* pBDcpt, size_t nDcpts, SINGLE_LIST* addList)
{
    size_t ix;
    MAC_BRIDGE_FWD_DCPT *dcptArray, *pFwdDcpt;

    dcptArray = (MAC_BRIDGE_FWD_DCPT*)TCPIP_HEAP_Calloc(pBDcpt->memH, nDcpts, sizeof(MAC_BRIDGE_FWD_DCPT));
    if(dcptArray == NULL)
    {   // out of mem
        return 0U;
    }

    // append to the list
    pFwdDcpt = dcptArray;
    for(ix = 0; ix < nDcpts; ix++)
    {
        TCPIP_Helper_SingleListTailAdd(addList, FC_FwdDcpt2SglNode(pFwdDcpt)); 
        pFwdDcpt++;
    }

    return nDcpts;
}

// gets a packet for forwarding
// either from the pool or a new allocation is made
static TCPIP_MAC_PACKET* F_MAC_Bridge_GetFwdPkt(MAC_BRIDGE_DCPT* pBDcpt, uint16_t pktLen)
{
    TCPIP_MAC_PACKET* pPkt;
    size_t nAllocPkts;

    // get a bridge packet
    while(true)
    {
        if(pBDcpt->pktPoolSize != 0U)
        {   // grab a packet from the pool

            // however, need to check the size of the packets in the pool 
            if(pktLen > pBDcpt->pktSize - sizeof(TCPIP_MAC_ETHERNET_HEADER))
            {   // if this packet is too big for the bridge, just drop it
                F_MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_SIZE, FC_Uint162VPtr(pktLen));
                F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FAIL_SIZE, 1);
                return NULL;
            }

            pPkt = FC_SglNode2MacPkt(TCPIP_Helper_SingleListHeadRemove(&pBDcpt->pktPool));
#if (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)
            uint32_t lowSize = TCPIP_Helper_SingleListCount(&pBDcpt->pktPool);
            if(lowSize < pBDcpt->stat.pktPoolLowSize)
            {
                pBDcpt->stat.pktPoolLowSize = lowSize;
            }
#endif  // (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)

            if(pPkt != NULL)
            {   // done
                break;
            }

            // pPkt == 0; pool was empty
            F_MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_PKT_POOL_EMPTY, NULL);
            F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_PKT_POOL_EMPTY, 1);

            size_t replPkts = pBDcpt->pktReplenish;
            if(replPkts != 0U)
            {   // try to replenish the packet pool
                nAllocPkts = F_MAC_Bridge_AllocatePackets(replPkts, &pBDcpt->pktPool, pBDcpt->pktSize, (uint8_t)MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN);
                if(nAllocPkts != replPkts)
                {
                    F_MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC, FC_Uint2VPtr((uint32_t)replPkts - (uint32_t)nAllocPkts));
                    F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FAIL_PKT_ALLOC, replPkts - nAllocPkts);
                }

                F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_ALLOC_PKTS, nAllocPkts);

                if(nAllocPkts != 0U)
                {
                    pPkt = FC_SglNode2MacPkt(TCPIP_Helper_SingleListHeadRemove(&pBDcpt->pktPool));
                    F_Mac_Bridge_AssertCond(pPkt != NULL, __func__, __LINE__);
                }
            }
            break;
        }

        // there is no pool
        // simply allocate a new packet
        SINGLE_LIST pktList = {0};
        nAllocPkts = F_MAC_Bridge_AllocatePackets(1, &pktList, pktLen, (uint8_t)MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN);
        if(nAllocPkts == 0U)
        {

            F_MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC, FC_Uint2VPtr(1UL));
            F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FAIL_PKT_ALLOC, 1);
            pPkt = NULL;
        }
        else
        {
            F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_ALLOC_PKTS, 1);
            pPkt = FC_SglNode2MacPkt(TCPIP_Helper_SingleListHeadRemove(&pktList));
            F_Mac_Bridge_AssertCond(pPkt != NULL, __func__, __LINE__);
        }

        break;
    }

    return pPkt;
}

// gets a descriptor for forwarding
// either from the pool or a new allocation is made
static MAC_BRIDGE_FWD_DCPT* F_MAC_Bridge_GetFwdDcpt(MAC_BRIDGE_DCPT* pBDcpt)
{
    MAC_BRIDGE_FWD_DCPT* pFwdDcpt;
    size_t nAllocDcpts;

    // grab a descriptor from the pool
    pFwdDcpt = FC_SglNode2FwdDcpt(TCPIP_Helper_SingleListHeadRemove(&pBDcpt->dcptPool));
#if (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)
    uint32_t lowSize = TCPIP_Helper_SingleListCount(&pBDcpt->dcptPool);
    if(lowSize < pBDcpt->stat.dcptPoolLowSize)
    {
        pBDcpt->stat.dcptPoolLowSize = lowSize;
    }
#endif  // (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)

    if(pFwdDcpt == NULL)
    {   // pFwdDcpt == 0; descriptor pool was empty
        F_MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_DCPT_POOL_EMPTY, NULL);
        F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_DCPT_POOL_EMPTY, 1);

        size_t replDcpts = pBDcpt->dcptReplenish;
        if(replDcpts != 0U)
        {
            nAllocDcpts = F_MAC_Bridge_AllocateDescriptors(pBDcpt, replDcpts, &pBDcpt->dcptPool);
            if(nAllocDcpts != replDcpts)
            {
                F_MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_DCPT_ALLOC, FC_Uint2VPtr((uint32_t)replDcpts - (uint32_t)nAllocDcpts));
                F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FAIL_DCPT_ALLOC, replDcpts - nAllocDcpts);
            }

            F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_ALLOC_DCPTS, nAllocDcpts);

            if(nAllocDcpts != 0U)
            {
                pFwdDcpt = FC_SglNode2FwdDcpt(TCPIP_Helper_SingleListHeadRemove(&pBDcpt->dcptPool));
                F_Mac_Bridge_AssertCond(pFwdDcpt != NULL, __func__, __LINE__);
            }
        }
    }

    return pFwdDcpt;
}

// populate a descriptor with the flood ports for a unicast/multicast destination address
// based on the hash entry and the input port
// on entry, pFDcpt holds the valid packet length and multicast/unicast flags 
// Note: the pFDcpt bit for the inPort shows the forward/filter for the internal host! 
static void F_MAC_Bridge_SetPacketForward(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* hDest, uint8_t inPort, MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    uint8_t portIx, startPort, endPort;
    uint8_t outIfIx;
    uint8_t* pOutMap;
    TCPIP_NET_IF* pOutIf;
    uint8_t portControl, defOutControl, defHostControl;   // TCPIP_MAC_BRIDGE_CONTROL_TYPE
    uint16_t linkMtu;
    uint8_t learnPort = 0U;

    if(hDest == NULL || ((hDest->hEntry.flags.value & ((uint16_t)MAC_BRIDGE_HFLAG_STATIC | (uint16_t)MAC_BRIDGE_HFLAG_PORT_VALID)) == 0U) )
    {   // no such entry in our FDB or invalid dynamic entry; forward an all other ports;
        // default action for internal host is filter
        pOutMap = NULL;
        startPort = 0U;
        endPort = pBDcpt->nPorts;
        defOutControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD; 
        defHostControl = (pFDcpt->fwdFlags & (uint16_t)MAC_BRIDGE_FWD_FLAG_MCAST) != 0U ? (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD: (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FILTER; 
    }
    else if((hDest->hEntry.flags.value & (uint16_t)MAC_BRIDGE_HFLAG_STATIC ) == 0U)
    {   // valid dynamic entry; forward to learnPort only
        // default action for internal host is filter
        pOutMap = NULL;
        startPort = hDest->learnPort;
        endPort = startPort + 1U;
        defOutControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD; 
        defHostControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FILTER; 
    }
    else if((hDest->hEntry.flags.value & (uint16_t)MAC_BRIDGE_HFLAG_HOST ) != 0U)
    {   // static entry for our own host
        // get the forward/filter behavior from the outPortMap,
        // including the outPortMap[inPort][inPort] for the host processing
        pOutMap = hDest->outPortMap[inPort];
        startPort = 0U;
        endPort = pBDcpt->nPorts;
        defOutControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FILTER; 
        defHostControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD; 
    }
    else
    {   // static entry but not for our host 
        // get the forward/filter behavior from the outPortMap,
        // including the outPortMap[inPort][inPort] for the host processing
        pOutMap = hDest->outPortMap[inPort];
        startPort = 0U;
        endPort = pBDcpt->nPorts;
        if((pFDcpt->fwdFlags & (uint16_t)MAC_BRIDGE_FWD_FLAG_MCAST) != 0U)
        {
            defHostControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD;
            defOutControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD; 
        }
        else
        {
            defHostControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FILTER; 
            if((hDest->hEntry.flags.value & (uint16_t)MAC_BRIDGE_HFLAG_PORT_VALID) != 0U)
            {   // we have a learnt port for this dest address
                learnPort = hDest->learnPort;
                defOutControl = TCPIP_MAC_BRIDGE_CONTROL_DYN_LEARN; 
            }
            else
            {   // no info for this dest address
                defOutControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD; 
            }
        }
    }
    

    // traverse all ports and construct the map
    // remove ports for which this packet is too big
    for(portIx = startPort; portIx < endPort; portIx++)
    {
        if(pOutMap != NULL)
        {
            portControl = *pOutMap++;
        }
        else
        {
            portControl = (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_DEFAULT;
        }

        if(portIx == inPort)
        {   // this host control
            if(portControl == (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_DEFAULT)
            {
                portControl = defHostControl;
            }

            if(portControl == (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD)
            {   // port needs flooding
                F_MAC_Bridge_AddPortMap(portIx, pFDcpt);
            }
            continue;
        }

        // output port control
        if(portControl == (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_DEFAULT)
        {
            portControl = defOutControl; 
        }

        if(portControl == TCPIP_MAC_BRIDGE_CONTROL_DYN_LEARN)
        {   // special case, need to use the learnPort 
            portControl = (portIx == learnPort) ? (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD : (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FILTER; 
        }

        if(portControl == (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD)
        {   // port needs forwarding
            // check it's not too large to send on the output interface
            outIfIx = pBDcpt->port2IfIx[portIx];
            pOutIf = TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet((size_t)outIfIx));
            linkMtu = TCPIPStackNetLinkMtu(pOutIf);
            if(pFDcpt->pktLen <= linkMtu)
            {   
                F_MAC_Bridge_AddPortMap(portIx, pFDcpt);
            }
            else
            {   // discard packets too large for interface
                F_MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_MTU, FC_Uint162VPtr(pFDcpt->pktLen));
                F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FAIL_MTU, 1);
            }
        }
        // else filter
    }
        
}

#if (M_TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
static void F_MAC_Bridge_NotifyEvent(MAC_BRIDGE_DCPT* pBDcpt, TCPIP_MAC_BRIDGE_EVENT event, const void* evParam)
{
    if(pBDcpt->evHandler != NULL)
    {
        pBDcpt->evHandler(event, evParam);
    }
}

TCPIP_MAC_BRIDGE_EVENT_HANDLE TCPIP_MAC_Bridge_EventHandlerRegister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLER evHandler)
{
    TCPIP_MAC_BRIDGE_EVENT_HANDLE evHandle = NULL;
    MAC_BRIDGE_DCPT* bDcpt = F_MAC_Bridge_ValidateHandle(brHandle);

    if(bDcpt != NULL && bDcpt->evHandler == NULL)
    {
        bDcpt->evHandler = evHandler;
        evHandle = bDcpt->evHandler;
    }

    return evHandle;
}

bool TCPIP_MAC_Bridge_EventHandlerDeregister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLE evHandle)
{
    MAC_BRIDGE_DCPT* bDcpt = F_MAC_Bridge_ValidateHandle(brHandle);
    if(bDcpt != NULL)
    {
        if(bDcpt->evHandler == evHandle)
        {
            bDcpt->evHandler = NULL;
            return true;
        }
    }

    return false;
}

#else
TCPIP_MAC_BRIDGE_EVENT_HANDLE TCPIP_MAC_Bridge_EventHandlerRegister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLER evHandler)
{
    return NULL;
}
bool TCPIP_MAC_Bridge_EventHandlerDeregister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLE evHandle)
{
    return false;
}

#endif  // (M_TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 

size_t TCPIP_MAC_Bridge_FDBEntries(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    MAC_BRIDGE_DCPT* bDcpt = F_MAC_Bridge_ValidateHandle(brHandle);
    if(bDcpt != NULL)
    {
        return bDcpt->hashDcpt->hEntries;
    }

    return 0;
}

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBIndexRead(TCPIP_MAC_BRIDGE_HANDLE brHandle, size_t ix, TCPIP_MAC_FDB_ENTRY* pEntry)
{
    MAC_BRIDGE_HASH_ENTRY h1, h2;
    const uint8_t* uPtr1, *uPtr2;
    MAC_BRIDGE_DCPT* bDcpt = F_MAC_Bridge_ValidateHandle(brHandle);

    if(bDcpt == NULL)
    {
        return TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR;
    }

    MAC_BRIDGE_HASH_ENTRY* hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryGet(bDcpt->hashDcpt, ix));

    if(hE == NULL)
    {
        return TCPIP_MAC_BRIDGE_RES_INDEX_ERROR; 
    }


    // obtain a consistent reading
    while(true)
    {
        (void)memcpy(&h1, hE, sizeof(h1));
        (void)memcpy(&h2, hE, sizeof(h2));
        uPtr1 = FC_BridgeEntry2U8Ptr(&h1);
        uPtr2 = FC_BridgeEntry2U8Ptr(&h2);
        if(memcmp(uPtr1, uPtr2, sizeof(h1)) == 0)
        {   // match
            break;
        }
    }

    if(h1.hEntry.flags.busy != 0U)
    {
        if(pEntry != NULL)
        {
            (void)memcpy(pEntry->destAdd.v, h1.destAdd.v, sizeof(h1.destAdd));
            pEntry->flags = (uint8_t)(h1.hEntry.flags.value >> 8U);
            pEntry->learnPort = h1.learnPort;
            pEntry->tExpire = h1.tExpire;
            pEntry->fwdPackets = h1.fwdPackets;
            (void)memcpy(pEntry->outPortMap, h1.outPortMap, sizeof(h1.outPortMap));
        }

        return TCPIP_MAC_BRIDGE_RES_OK;
    }

    return TCPIP_MAC_BRIDGE_RES_INDEX_NO_ENTRY;
}

#if (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
static TCPIP_MAC_BRIDGE_RESULT F_MAC_Bridge_FDBLock(TCPIP_MAC_BRIDGE_HANDLE brHandle, bool validate)
{
    MAC_BRIDGE_DCPT* pDcpt;

    if(validate)
    {
        pDcpt = F_MAC_Bridge_ValidateHandle(brHandle);
        if(pDcpt == NULL)
        {
            return TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR;
        }
    }
    else
    {
        pDcpt = F_MAC_Bridge_DcptFromHandle(brHandle); 
    }

    TCPIP_MAC_BRIDGE_RESULT res;
    // make sure the user threads don't mess with the access queue
    OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(pDcpt->bridgeLock != 0U)
    {
        res = TCPIP_MAC_BRIDGE_RES_LOCK_ERROR;
    }
    else
    {
        pDcpt->bridgeLock = 1U;
        res = TCPIP_MAC_BRIDGE_RES_OK;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);

    return res;
}

static void F_MAC_Bridge_FDBUnlock(MAC_BRIDGE_DCPT* pDcpt)
{
    OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pDcpt->bridgeLock = 0U;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);
}


TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBReset(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    // delete all dynamic data from the FDB
    size_t ix;
    MAC_BRIDGE_HASH_ENTRY* hE;
    size_t nEntries;

    // lock access: make sure the user threads don't mess with the FDB
    TCPIP_MAC_BRIDGE_RESULT res = F_MAC_Bridge_FDBLock(brHandle, true);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {
        return res;
    }

    MAC_BRIDGE_DCPT* pDcpt = F_MAC_Bridge_DcptFromHandle(brHandle);

    nEntries = pDcpt->hashDcpt->hEntries;

    for(ix = 0; ix < nEntries; ix++)
    {
        hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryGet(pDcpt->hashDcpt, ix));
        if(hE->hEntry.flags.busy != 0U)
        {   // in use
            if((hE->hEntry.flags.value & (uint16_t)MAC_BRIDGE_HFLAG_STATIC ) != 0U)
            {   // static entry
                hE->hEntry.flags.value &= ~(uint16_t)MAC_BRIDGE_HFLAG_PORT_VALID;
                hE->fwdPackets = 0U;
            }
            else
            {   // dynamic entry
                TCPIP_OAHASH_EntryRemove(pDcpt->hashDcpt, &hE->hEntry);
            }
        }
    }

    // done
    F_MAC_Bridge_FDBUnlock(pDcpt);
    return TCPIP_MAC_BRIDGE_RES_OK;
}

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBDeleteEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_ADDR* pDestAddr)
{

    // lock access: make sure the user threads don't mess with the FDB
    TCPIP_MAC_BRIDGE_RESULT res = F_MAC_Bridge_FDBLock(brHandle, true);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {
        return res;
    }

    MAC_BRIDGE_DCPT* pDcpt = F_MAC_Bridge_DcptFromHandle(brHandle);

    MAC_BRIDGE_HASH_ENTRY* hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryLookup(pDcpt->hashDcpt, pDestAddr->v));

    if(hE != NULL && hE->hEntry.flags.busy != 0U)
    {   // valid entry
        TCPIP_OAHASH_EntryRemove(pDcpt->hashDcpt, &hE->hEntry);
    }

    // done
    F_MAC_Bridge_FDBUnlock(pDcpt);
    return TCPIP_MAC_BRIDGE_RES_OK;
}

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBAddEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPermEntry)
{
    if(pPermEntry == NULL)
    {
        return TCPIP_MAC_BRIDGE_RES_PARAM_ERROR; 
    }
    // lock access: make sure the user threads don't mess with the FDB
    TCPIP_MAC_BRIDGE_RESULT res = F_MAC_Bridge_FDBLock(brHandle, true);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {
        return res;
    }

    MAC_BRIDGE_DCPT* pDcpt = F_MAC_Bridge_DcptFromHandle(brHandle);

    MAC_BRIDGE_HASH_ENTRY* hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryLookupOrInsert(pDcpt->hashDcpt, pPermEntry->destAdd.v));

    if(hE != NULL)
    {   // clear the outPortMap for a new entry, keep it for existent one
        F_MAC_Bridge_SetHashStaticEntry(hE, 0U, hE->hEntry.flags.newEntry != 0U);
        // update the outPortMap
        res = F_MAC_Bridge_SetFDBFromPerm(pDcpt, hE, pPermEntry);
    }
    else
    {
        res = TCPIP_MAC_BRIDGE_RES_FDB_FULL; 
    }

    // done
    F_MAC_Bridge_FDBUnlock(pDcpt);
    return res;
}
#else
TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBReset(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    return TCPIP_MAC_BRIDGE_RES_NO_OP; 
}
TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBDeleteEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_ADDR* pDestAddr)
{
    return TCPIP_MAC_BRIDGE_RES_NO_OP; 
}
TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBAddEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPermEntry)
{
    return TCPIP_MAC_BRIDGE_RES_NO_OP; 
}

#endif  // (M_TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)

static void F_MAC_Bridge_SetHashDynamicEntry(MAC_BRIDGE_HASH_ENTRY* hE, uint8_t port, uint16_t clrFlags, uint16_t setFlags)
{
    hE->hEntry.flags.value &= ~(clrFlags);
    hE->hEntry.flags.value |= setFlags;
    hE->learnPort = port;
    hE->tExpire = F_MAC_Bridge_GetSecond() + gBridgeDcpt->purgeTmo;
}

static void F_MAC_Bridge_SetHashStaticEntry(MAC_BRIDGE_HASH_ENTRY* hE, uint16_t stFlags, bool clearOutMap)
{
    hE->hEntry.flags.value &= ~(uint16_t)(MAC_BRIDGE_HFLAG_MASK);
    hE->hEntry.flags.value |= ((uint16_t)MAC_BRIDGE_HFLAG_STATIC | stFlags);

    if(clearOutMap)
    {
        (void)memset(hE->outPortMap, 0, sizeof(hE->outPortMap));
    }
}


// forward a packet on an output interface
// if all done, it frees the allocated packet
static void F_MAC_Bridge_ForwardPacket(TCPIP_MAC_PACKET* pFwdPkt, MAC_BRIDGE_HASH_ENTRY* hEntry)
{
    MAC_BRIDGE_FWD_DCPT* pFDcpt = F_MAC_Bridge_GetPktFwdDcpt(pFwdPkt);
    uint16_t pktFlags = F_MAC_Bridge_GetPktFlags(pFwdPkt);
    MAC_BRIDGE_DCPT* pBDcpt = gBridgeDcpt; 

    // check we've not exceeded the time
    bool pktDrop;
    if(F_MAC_Bridge_GetSecond() - pFDcpt->tReceive > pBDcpt->transitDelay)
    {
        F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_DELAY_PKTS, 1);
        pktDrop = true;
    }
    else
    {
        pktDrop = false;
    }

    while(!pktDrop)
    {
        // get the bridge port to process on
        int mapBrPort = F_MAC_Bridge_PortFromMap(pFDcpt);
        if(mapBrPort < 0)
        {   // done, nothing else
            break;
        }

        // there is something to transmit
        uint8_t brPort = (uint8_t)mapBrPort;
        if((size_t)brPort >= sizeof(pBDcpt->port2IfIx))
        {   // should not happen!
            F_Mac_Bridge_AssertCond(false, __func__, __LINE__);
            // make sure the faulty packet is discarded/freed
            pktFlags |= (uint16_t)MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN;
            pktFlags &= ~(uint16_t)MAC_BRIDGE_ALLOC_FLAG_STICKY;
            F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FAIL_PORT, 1);
            break;
        }

        // done with this port
        F_MAC_Bridge_DelPortMap(brPort, pFDcpt);
        uint8_t outIf = pBDcpt->port2IfIx[brPort];
        TCPIP_NET_IF* pOutIf = TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet((size_t)outIf));

        // test that the netowrk IF hasn't been killed; there's no other test for this
        if(TCPIP_STACK_NetworkIsUp(pOutIf))
        {
            pFwdPkt->next = NULL;   // send single packet
            if((int)TCPIPStackPacketTx(pOutIf, pFwdPkt) >= 0)
            {   // successfully transmitted
                if((pFDcpt->fwdFlags & (uint16_t)MAC_BRIDGE_FWD_FLAG_MCAST) != 0U)
                {
                    F_MAC_Bridge_StatPortUpdate(gBridgeDcpt, brPort, BRIDGE_PORT_STAT_FWD_MCAST_PKTS, 1);
                }
                else
                {
                    F_MAC_Bridge_StatPortUpdate(gBridgeDcpt, brPort, BRIDGE_PORT_STAT_FWD_UCAST_PKTS, 1);
                }
                if(hEntry != NULL)
                {
                    hEntry->fwdPackets++;
                }
                if((pktFlags & (uint16_t)MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN) == 0U)
                {
                    F_MAC_Bridge_StatPortUpdate(gBridgeDcpt, brPort, BRIDGE_PORT_STAT_FWD_DIRECT_PKTS, 1);
                }

                F_MAC_Bridge_TracePkt(pFwdPkt, brPort, true);
                return;
            }

            // failed sending the packet 
            F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FAIL_MAC, 1);
        }

        // try on another interface
    }

    // at this point the forward map is empty and we're done

    if((pktFlags & (uint16_t)MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN) != 0U)
    {   // bridge owned
        if((pktFlags & (uint16_t)MAC_BRIDGE_ALLOC_FLAG_STICKY) != 0U)
        {   // return to the pool
            TCPIP_Helper_SingleListTailAdd(&pBDcpt->pktPool, FC_MacPkt2SglNode(pFwdPkt)); 
        }
        else
        {   // free the allocated packet
            TCPIP_PKT_PacketFree(pFwdPkt);
            F_MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_FREE_PKTS, 1);
        }
    }
    else
    {   // restore the packet owners and acknowledge it
        pFwdPkt->ackParam = pFDcpt->ownAckParam;
        pFwdPkt->ackFunc = pFDcpt->ownAckFunc;
        TCPIP_PKT_PacketAcknowledge(pFwdPkt, TCPIP_MAC_PKT_ACK_BRIDGE_DONE); 
    }

    // re-add the fowrward descriptor to the pool
    TCPIP_Helper_SingleListTailAdd(&pBDcpt->dcptPool, FC_FwdDcpt2SglNode(pFDcpt)); 
}


// creates a copy of the source packet
// make a copy of the packet and then this copy has its own ack fnc
// no need to re-insert into the stack
static void F_MAC_Bridge_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, TCPIP_MAC_PACKET* pBridgePkt, uint16_t pktLen, MAC_BRIDGE_HASH_ENTRY* hEntry)
{
    // copy the payload
    uint8_t* srcStartAdd = pSrcPkt->pNetLayer;
    uint16_t copiedLen = TCPIP_Helper_PacketCopy(pSrcPkt, pBridgePkt->pNetLayer, &srcStartAdd, pktLen, false);

    if(copiedLen != pktLen)
    {   // should NOT happen!
        F_Mac_Bridge_AssertCond(false, __func__, __LINE__);
    }
    // copy the Ethernet header
    (void)memcpy(pBridgePkt->pMacLayer, pSrcPkt->pMacLayer, sizeof(TCPIP_MAC_ETHERNET_HEADER));

    pBridgePkt->pDSeg->segLen = copiedLen;
}

// acknowledges a bridge packet once it's been transmitted
static void F_MAC_Bridge_PacketAck(TCPIP_MAC_PACKET* pkt,  const void* param)
{
    F_MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_ACK_PKTS, 1);

    // keep forwarding to the other interfaces
    F_MAC_Bridge_ForwardPacket(pkt, FC_CVPtr2BridgeEntry(param));
}

// returns the number of the ports that are part of the bridge and their port map
// -1 if failure
static int  F_MAC_Bridge_SetPorts(MAC_BRIDGE_DCPT* pBDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pConfig)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_NET_IF*  pNetIf;
    size_t ix;
    uint8_t nPorts = 0U;
    const TCPIP_MAC_BRIDGE_ENTRY_ASCII* pAscEntry = FC_BridgeTbl2AscEntry(pConfig->bridgeTable);
    const TCPIP_MAC_BRIDGE_ENTRY_BIN* pBinEntry = FC_BridgeTbl2BinEntry(pConfig->bridgeTable);

    uint8_t portInUse[TCPIP_MAC_BRIDGE_MAX_PORTS_NO] = {0U};

    for(ix = 0; ix < pConfig->bridgeTableSize; ix++)
    {
        if((pConfig->bridgeFlags & (uint8_t)TCPIP_MAC_BRIDGE_FLAG_IF_NAME_TABLE) != 0U) 
        {
            if(pAscEntry->ifName == NULL)
            {
                return -1;
            }

            netH = TCPIP_STACK_NetHandleGet(pAscEntry->ifName);
        }
        else
        {
            netH = TCPIP_STACK_IndexToNet((size_t)pBinEntry->ifIx);
        }

        if(netH == NULL)
        {
            return -1;
        }

        pNetIf = TCPIPStackHandleToNet(netH);

        int ifIx = TCPIP_STACK_NetIxGet(pNetIf);
        if(ifIx < 0)
        {
            return -1;
        }

        if(portInUse[nPorts] == 0U)
        {
            pBDcpt->port2IfIx[nPorts] = (uint8_t)ifIx;
            portInUse[nPorts] = 1U;
            TCPIPStack_BridgeSetIf(pNetIf, nPorts);
            nPorts++;
        }
        pAscEntry++; pBinEntry++;
    }

    return (int)nPorts;
}

static TCPIP_MAC_BRIDGE_RESULT F_MAC_Bridge_SetPermEntries(MAC_BRIDGE_DCPT* pBDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pBConfig)
{
    const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPerm;
    MAC_BRIDGE_HASH_ENTRY* hE;
    size_t ix;

    if(pBConfig->bridgePermTableSize == 0U || pBConfig->bridgePermTable == NULL)
    {   // nothing to do
        return TCPIP_MAC_BRIDGE_RES_OK;
    }

    // start adding static entries
    // 
    TCPIP_MAC_BRIDGE_RESULT res = TCPIP_MAC_BRIDGE_RES_OK;
    pPerm = pBConfig->bridgePermTable;
    for(ix = 0; ix < pBConfig->bridgePermTableSize; ix++)
    {
        hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryLookupOrInsert(pBDcpt->hashDcpt, pPerm->destAdd.v));
        if(hE == NULL)
        {   // out of entries!
            return TCPIP_MAC_BRIDGE_RES_FDB_FULL;
        }

        // set the permanent entry!
        F_MAC_Bridge_SetHashStaticEntry(hE, 0U, true);

        res = F_MAC_Bridge_SetFDBFromPerm(pBDcpt, hE,  pPerm);
        if(res != TCPIP_MAC_BRIDGE_RES_OK)
        {
            break;
        }
        pPerm++;
    }

    return res;
}


static TCPIP_MAC_BRIDGE_RESULT F_MAC_Bridge_SetFDBFromPerm(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* hE,  const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPerm)
{
    size_t jx, kx;
    TCPIP_NET_IF    *pInIf, *pOutIf;
    const TCPIP_MAC_BRIDGE_CONTROL_ENTRY* pCEntry;
    const TCPIP_MAC_BRIDGE_CONTROL_DCPT* pCtrl;
    uint8_t*  pOutMap;
    uint8_t bridgePort;


    pCEntry = pPerm->pControlEntry;
    for(jx = 0; jx < pPerm->controlEntries; jx++)
    {
        pInIf = TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(pCEntry->inIx));
        if(pInIf == NULL || !TCPIPStack_BridgeCheckIf(pInIf))
        {   // input interface not part of the bridge!
            return TCPIP_MAC_BRIDGE_RES_IF_NOT_BRIDGED;
        }

        if(pCEntry->dcptMapEntries != 0U)
        {   // there is static info we need to set
            bridgePort = TCPIPStack_BridgeGetIfPort(pInIf);
            pOutMap = hE->outPortMap[bridgePort];

            pCtrl = pCEntry->pDcptMap;
            for(kx = 0; kx < pCEntry->dcptMapEntries; kx++)
            {
                pOutIf = TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(pCtrl->outIx));
                if(pOutIf == NULL || !TCPIPStack_BridgeCheckIf(pOutIf))
                {   // output interface not part of the bridge!
                    return TCPIP_MAC_BRIDGE_RES_IF_NOT_BRIDGED;
                }
                pOutMap[pCtrl->outIx] = pCtrl->portControl;
                pCtrl++;
            }
        }
        pCEntry++;
    }

    return TCPIP_MAC_BRIDGE_RES_OK;
} 


// populates the FDB with the host internal entries
// these configure the host packets behavior
static TCPIP_MAC_BRIDGE_RESULT F_MAC_Bridge_SetHostEntries(MAC_BRIDGE_DCPT* pDcpt)
{
    MAC_BRIDGE_HASH_ENTRY* hE;
    TCPIP_NET_IF*   pNetIf;
    const TCPIP_MAC_ADDR* pMacAddress;
    size_t ifIx;
    size_t portIx;
    bool clearOutMap;

    for(portIx = 0; portIx < pDcpt->nPorts; portIx++) 
    {
        ifIx = pDcpt->port2IfIx[portIx];
        pNetIf = TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(ifIx));

        F_Mac_Bridge_AssertCond(pNetIf != NULL, __func__, __LINE__);
        F_Mac_Bridge_AssertCond(TCPIPStack_BridgeCheckIf(pNetIf) == true, __func__, __LINE__);
        F_Mac_Bridge_AssertCond(TCPIPStack_BridgeGetIfPort(pNetIf) == portIx, __func__, __LINE__);


        pMacAddress = FC_CUptr2CMacAdd(TCPIPStack_NetMACAddressGet(pNetIf));

        if(TCPIP_Helper_IsMcastMACAddress(pMacAddress))
        {
            return TCPIP_MAC_BRIDGE_RES_IF_ADDRESS_ERROR; 
        }

        // add this interface as a static entry
        hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryLookupOrInsert(pDcpt->hashDcpt, TCPIPStack_NetMACAddressGet(pNetIf)));
        if(hE == NULL)
        {   // out of entries!
            return TCPIP_MAC_BRIDGE_RES_FDB_FULL;
        }

        if(hE->hEntry.flags.newEntry == 0U)
        {   // overriden by initialization 
            F_Mac_Bridge_AssertCond((hE->hEntry.flags.value & (uint16_t)MAC_BRIDGE_HFLAG_STATIC) != 0U, __func__, __LINE__);
            clearOutMap = false;
        }
        else
        {   // just added
            clearOutMap = true;
        }

        F_MAC_Bridge_SetHashStaticEntry(hE, (uint16_t)MAC_BRIDGE_HFLAG_HOST, clearOutMap);
    }

    return TCPIP_MAC_BRIDGE_RES_OK;
}

// helper to check if an iterface going up
// is part of the bridge
// returns the bridge port number if true, < 0 if false
static int F_MAC_Bridge_IfBridged(MAC_BRIDGE_DCPT* pDcpt, size_t netIx)
{
    size_t ix;
    uint8_t* pIf = pDcpt->port2IfIx;
    for(ix = 0; ix < pDcpt->nPorts; ix++)
    {
        if((size_t)*pIf == netIx)
        {
            return (int)ix;
        }
        pIf++;
    }

    return -1;
}

// simplistic implementation of count leading zeros in a 32 bit value
// It returns the number of contiguous leading zeroes
#if !defined (__PIC32MX__) && !defined(__PIC32MZ__)
static unsigned int F_MAC_BridgeLeadingZeroes(uint32_t value)
{
    size_t ix;
    uint32_t mask;
    uint16_t nZeroes;

    nZeroes = 0U;
    mask = 1UL << 31;  // start from MSB

    for(ix = 0; ix < 32U; ix++)
    {
        if((value & mask) != 0U)
        {   // 1 detected
            return nZeroes;
        }

        // 0 detected
        nZeroes++;
        mask >>= 1;
    }

    return nZeroes;
}
#endif  // !defined (__PIC32MX__) && !defined(__PIC32MZ__)



static size_t F_MAC_BridgeHashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, MAC_BRIDGE_HASH_KEY_SIZE) % (pOH->hEntries);
}

// Deletes an entry to make room in the hash table.
// This should be taken care of the MAC_BRIDGE_Task()
// However, there are static entries and the purgeTmo could be fairly long
// So it's possible to be unable to make room in the cache
static OA_HASH_ENTRY* F_MAC_BridgeHashEntryDelete(OA_HASH_DCPT* pOH)
{
    F_Mac_Bridge_AssertCond(pOH == gBridgeDcpt->hashDcpt, __func__, __LINE__);

    size_t ix;
    MAC_BRIDGE_HASH_ENTRY* hE;
    size_t nEntries = gBridgeDcpt->hashDcpt->hEntries;

    for(ix = 0; ix < nEntries; ix++)
    {
        hE = FC_HEntry2BridgeEntry(TCPIP_OAHASH_EntryGet(gBridgeDcpt->hashDcpt, ix));
        if(hE->hEntry.flags.busy != 0U && (hE->hEntry.flags.value & (uint16_t)MAC_BRIDGE_HFLAG_STATIC ) == 0U)
        {   // in use dynamic entry
            if(FC_Ui322I32(F_MAC_Bridge_GetSecond() - hE->tExpire) > 0)
            {   // expired entry; remove
                return &hE->hEntry;
            }
        }
    }
    
    return NULL;
}


static int F_MAC_BridgeHashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    const TCPIP_MAC_ADDR* destAddKey = FC_CVptr2MacAdd(key); 
    return memcmp(FC_HEntry2BridgeEntry(hEntry)->destAdd.v, destAddKey->v, MAC_BRIDGE_HASH_KEY_SIZE);
}

static void F_MAC_BridgeHashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    const TCPIP_MAC_ADDR* destAddKey = FC_CVptr2MacAdd(key); 
    (void)memcpy(FC_HEntry2BridgeEntry(dstEntry)->destAdd.v, destAddKey->v, MAC_BRIDGE_HASH_KEY_SIZE);
}

#if (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)
static void F_MAC_Bridge_StatUpdate(MAC_BRIDGE_DCPT* bDcpt, MAC_BRIDGE_STAT_TYPE statType, uint32_t incUpdate)
{
    bDcpt->stat_array[statType] += incUpdate;
}

static void F_MAC_Bridge_StatPortUpdate(MAC_BRIDGE_DCPT* bDcpt, uint8_t port, BRIDGE_PORT_STAT_TYPE statType, uint32_t incUpdate)
{
    TCPIP_MAC_BRIDGE_PORT_STAT* pPortStat = bDcpt->stat.portStat + port;
    uint32_t* pStat = &pPortStat->rxPackets + statType;
    *pStat += incUpdate;
}

bool TCPIP_MAC_Bridge_StatisticsGet(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_STAT* pStat, bool clear)
{
    MAC_BRIDGE_DCPT* bDcpt = F_MAC_Bridge_ValidateHandle(brHandle);

    if(bDcpt == NULL)
    {
        return false;
    }

    uint32_t pktPoolSize = TCPIP_Helper_SingleListCount(&bDcpt->pktPool);
    uint32_t dcptPoolSize = TCPIP_Helper_SingleListCount(&bDcpt->dcptPool);
    if(pStat)
    {
        *pStat = bDcpt->stat;
        pStat->pktPoolSize = pktPoolSize;
        pStat->dcptPoolSize = dcptPoolSize;
    }

    if(clear)
    {
        (void)memset(bDcpt->stat_array, 0, sizeof(bDcpt->stat_array));
        bDcpt->stat.pktPoolLowSize = pktPoolSize;
        bDcpt->stat.dcptPoolLowSize = dcptPoolSize;
    }

    return true;
}
#else
bool TCPIP_MAC_Bridge_StatisticsGet(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_STAT* pStat, bool clear)
{
    return false;
}
#endif  // (M_TCPIP_MAC_BRIDGE_STATISTICS != 0)

#endif  //if defined(TCPIP_STACK_USE_MAC_BRIDGE)


