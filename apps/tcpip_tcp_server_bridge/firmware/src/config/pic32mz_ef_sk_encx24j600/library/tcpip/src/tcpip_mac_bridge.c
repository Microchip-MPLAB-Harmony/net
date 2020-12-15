/*******************************************************************************
  TCP/IP MAC Layer 2

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    Implementation of a simple transparent Layer 2 bridge 
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





#include "tcpip/src/tcpip_private.h"


#if defined(TCPIP_STACK_USE_MAC_BRIDGE)

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_BRIDGE

#include "tcpip/tcpip_mac_bridge.h"
#include "tcpip/src/tcpip_mac_bridge_private.h"

// local prototypes
static void     _MAC_Bridge_Cleanup(void);
static int      _MAC_Bridge_SetPorts(MAC_BRIDGE_DCPT* pBDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pConfig);

static TCPIP_MAC_BRIDGE_RESULT      _MAC_Bridge_SetPermEntries(MAC_BRIDGE_DCPT* pDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pBConfig);
static TCPIP_MAC_BRIDGE_RESULT      _MAC_Bridge_SetHostEntries(MAC_BRIDGE_DCPT* pDcpt);

static TCPIP_MAC_BRIDGE_RESULT      _MAC_Bridge_SetFDBFromPerm(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* hE,  const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPerm);

static void     _MAC_Bridge_ForwardPacket(TCPIP_MAC_PACKET* pFwdPkt, MAC_BRIDGE_HASH_ENTRY* hEntry);

static void     _MAC_Bridge_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, TCPIP_MAC_PACKET* pFwdPkt, uint16_t pktLen, MAC_BRIDGE_HASH_ENTRY* hEntry);
static bool     _MAC_Bridge_PacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);

static void     _MAC_Bridge_SetHashDynamicEntry(MAC_BRIDGE_HASH_ENTRY* hE, uint8_t port, MAC_BRIDGE_HASH_FLAGS clrFlags, MAC_BRIDGE_HASH_FLAGS setFlags);
static void     _MAC_Bridge_SetHashStaticEntry(MAC_BRIDGE_HASH_ENTRY* hE, MAC_BRIDGE_HASH_FLAGS flags, bool clearOutMap);

static size_t   _MAC_BridgeHashKeyHash(OA_HASH_DCPT* pOH, const void* key);
static OA_HASH_ENTRY* _MAC_BridgeHashEntryDelete(OA_HASH_DCPT* pOH);
static int      _MAC_BridgeHashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void     _MAC_BridgeHashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);

static void     _MAC_Bridge_SetPacketForward(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* heDest, uint8_t inPort, MAC_BRIDGE_FWD_DCPT* pFDcpt);

static int      _MAC_Bridge_AllocatePackets(int nPkts, SINGLE_LIST* addList, uint16_t pktSize, uint8_t allocFlags);

static int      _MAC_Bridge_AllocateDescriptors(MAC_BRIDGE_DCPT* pBDcpt, int nDcpts, SINGLE_LIST* addList);

static TCPIP_MAC_PACKET* _MAC_Bridge_GetFwdPkt(MAC_BRIDGE_DCPT* pBDcpt, uint16_t pktLen);

static MAC_BRIDGE_FWD_DCPT* _MAC_Bridge_GetFwdDcpt(MAC_BRIDGE_DCPT* pBDcpt);


static int      _MAC_Bridge_IfBridged(MAC_BRIDGE_DCPT* pDcpt, int netIx);

#if (TCPIP_MAC_BRIDGE_STATISTICS != 0)
static void     _MAC_Bridge_StatUpdate(MAC_BRIDGE_DCPT* bDcpt, MAC_BRIDGE_STAT_TYPE statType, uint32_t incUpdate);
static void     _MAC_Bridge_StatPortUpdate(MAC_BRIDGE_DCPT* bDcpt, int port, MAC_BRIDGE_STAT_TYPE statType, uint32_t incUpdate);
#else
#define         _MAC_Bridge_StatUpdate(bDcpt, statType, incUpdate)
#define         _MAC_Bridge_StatPortUpdate(bDcpt, port, statType, incUpdate)
#endif  // (TCPIP_MAC_BRIDGE_STATISTICS != 0)

#if (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
static void     _MAC_Bridge_NotifyEvent(MAC_BRIDGE_DCPT* pBDcpt, TCPIP_MAC_BRIDGE_EVENT event, const void* evParam);
#else
#define         _MAC_Bridge_NotifyEvent(pBDcpt, event, evParam)
#endif  // (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0)


#if defined (__PIC32MX__) || defined(__PIC32MZ__)
#define _MAC_BridgeLeadingZeroes(value) __builtin_clz(value)
#else
static unsigned int _MAC_BridgeLeadingZeroes(uint32_t value);
#endif

// definitions and local data
static uint32_t         bridgeInitCount = 0;

static MAC_BRIDGE_DCPT  bridgeDcpt = { 0, 0 };
static MAC_BRIDGE_DCPT* gBridgeDcpt = 0;

static const uint8_t bridge_reserved_address[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};

// implementation
#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_BASIC) != 0)
volatile int _MacBridgeStayAssertLoop = 0;
static void _Mac_Bridge_AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("Bridge Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_MacBridgeStayAssertLoop != 0);
    }
}

#else
#define _Mac_Bridge_AssertCond(cond, message, lineNo)
#endif  // (MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_BASIC)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_INIT) != 0)
static const char* _init_default_message = "MAC Bridge initialization failed: ";
static void _Mac_Bridge_InitCond(const char* message, int code)
{
    if (message == 0)
    {
        message = _init_default_message; 
    }

    SYS_ERROR_PRINT(SYS_ERROR_ERROR, "%s, %d\r\n", message, code);
}
#else
#define _Mac_Bridge_InitCond(message, code)
#endif  // (MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_INIT)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_FDB_INTEGRITY) != 0)
TCPIP_MAC_ADDR hashTrace[TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES];

static uint32_t hashDupl = 0;
bool _MAC_Bridge_CheckFDB(MAC_BRIDGE_DCPT* pDcpt)
{
    int ix, jx;
    MAC_BRIDGE_HASH_ENTRY* hE;

    memset(hashTrace, 0, sizeof(hashTrace));
    int traceIx = 0;
    TCPIP_MAC_ADDR* pTrace = hashTrace;

    int nEntries = pDcpt->hashDcpt->hEntries;
    for(ix = 0; ix < nEntries; ix++)
    {
        hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pDcpt->hashDcpt, ix);
        if(hE->hEntry.flags.busy != 0)
        {   // in use entry
            memcpy(pTrace, hE->destAdd.v, sizeof(*pTrace));
            pTrace++;
            traceIx++;
        }
    }

    // now check integrity
    if(traceIx != 0)
    {
        pTrace = hashTrace;
        for(ix = 0; ix < traceIx - 1; ix++, pTrace++)
        {
            TCPIP_MAC_ADDR* pNext = pTrace + 1;
            for(jx = ix + 1; jx < traceIx; jx++, pNext++)
            {
                if(memcmp(pTrace->v, pNext->v, 6) == 0)
                {   // ooops! duplication
                    hashDupl++;
                    _Mac_Bridge_AssertCond(false, __func__, __LINE__);
                    return false;
                }
            } 
        }
    }

    return true;
}

#else
#define _MAC_Bridge_CheckFDB(pDcpt)
#endif // ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_FDB_INTEGRITY) != 0)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_RX_IP_CHECK) != 0)
static uint16_t checkRxUdpSrcPort = 67; 
static uint16_t checkRxUdpDstPort = 68; 

static uint16_t checkRxTcpSrcPort = 9760; 
static uint16_t checkRxTcpDstPort = 9760; 

static uint32_t checkRxUdpBkptCnt = 0;
static uint32_t checkRxIcmpBkptCnt = 0;
static uint32_t checkRxTcpBkptCnt = 0;
static void _MAC_Bridge_CheckRxIpPkt(TCPIP_MAC_PACKET* pRxPkt)
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
#define _MAC_Bridge_CheckRxIpPkt(pRxPkt)
#endif  // ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_RX_IP_CHECK) != 0)

#if ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_TRACE_PKT) != 0)
static TCPIP_MAC_ADDR traceAddArray[] =
{
    {{0x00, 0x04, 0x25, 0x1c, 0xa0, 0x02}},
    {{0xc8, 0x5b, 0x76, 0x1d, 0xa9, 0x2d}},
};

static void _MAC_Bridge_TracePkt(TCPIP_MAC_PACKET* pPkt, int brPort, bool isTx)
{
    static uint16_t breakType = 0x0608;
    int ix;

    TCPIP_MAC_ETHERNET_HEADER*  pMacHdr = (TCPIP_MAC_ETHERNET_HEADER*)pPkt->pMacLayer;
    const TCPIP_MAC_ADDR* pSrcAdd = &pMacHdr->SourceMACAddr;
    const TCPIP_MAC_ADDR* pDestAdd = &pMacHdr->DestMACAddr;

    const TCPIP_MAC_ADDR* pTraceAdd = traceAddArray;
    for(ix = 0; ix < sizeof(traceAddArray) / sizeof(*traceAddArray); ix++, pTraceAdd++)
    {
        if(memcmp(pDestAdd->v, pTraceAdd->v, sizeof(*pDestAdd)) == 0 || 
           memcmp(pSrcAdd->v, pTraceAdd->v, sizeof(*pSrcAdd)) == 0)
        {
            char srcAddBuff[20], dstAddBuff[20];

            TCPIP_Helper_MACAddressToString(pDestAdd, dstAddBuff, sizeof(dstAddBuff));
            TCPIP_Helper_MACAddressToString(pSrcAdd, srcAddBuff, sizeof(srcAddBuff));

            uint16_t pktType = pMacHdr->Type;
            uint16_t pktLen = pPkt->pDSeg->segLen;
            SYS_CONSOLE_PRINT("Trace- %s pkt from: %s to %s, pktType: 0x%04x, pktLen: %d, brPort: %d\r\n", isTx ? "out" : "in", srcAddBuff, dstAddBuff, pktType, pktLen, brPort); 
            if(breakType == pktType)
            {
                breakType = pktType;
            }
            break;
        }
    }
}
#else
#define _MAC_Bridge_TracePkt(pPkt, brPort, isRx)
#endif  // ((MAC_BRIDGE_DEBUG_MASK & MAC_BRIDGE_DEBUG_MASK_TRACE_PKT) != 0)

// clear/empty the forwarding map
static __inline__ void __attribute__((always_inline))  _MAC_Bridge_ClearMap(MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    pFDcpt->fwdMap32 = 0;
}

// check if the forwarding map is not empty
static __inline__ bool __attribute__((always_inline))  _MAC_Bridge_IsMapEmpty(const MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    return pFDcpt->fwdMap32 == 0;
}

// check if port is part of the forwarding map
static __inline__ bool __attribute__((always_inline))  _MAC_Bridge_IsPortMapped(int inPort, const MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    return ((1ul << inPort) & pFDcpt->fwdMap32) != 0;
}

// remove port from forwarding map
static __inline__ void __attribute__((always_inline))  _MAC_Bridge_DelPortMap(int inPort, MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    pFDcpt->fwdMap32 &= ~(1ul << inPort);
}

// add port to the forwarding map
static __inline__ void __attribute__((always_inline))  _MAC_Bridge_AddPortMap(int portIx, MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    pFDcpt->fwdMap32 |= 1ul << portIx;
}

// return a processing port number
// < 0 if none
static __inline__ int __attribute__((always_inline))  _MAC_Bridge_PortFromMap(const MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    if(pFDcpt->fwdMap32 != 0)
    {
        return 31 - (int)_MAC_BridgeLeadingZeroes(pFDcpt->fwdMap32);
    }
    return -1;
}

static __inline__ MAC_BRIDGE_DCPT* __attribute__((always_inline)) _MAC_Bridge_ValidateHandle(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    return ((MAC_BRIDGE_DCPT*)brHandle ==  gBridgeDcpt) ? (MAC_BRIDGE_DCPT*)brHandle : 0;
}

// sets the flags of the packet
// since this is either the bridge copy or the host does not process it
// we use the pktClientData[0] for flags
static __inline__ void __attribute__((always_inline)) _MAC_Bridge_SetPktFlags(TCPIP_MAC_PACKET* pPkt, uint16_t flags)
{
    pPkt->pktClientData[0] = flags;
}

// returns the flags of the packet
static __inline__ uint16_t __attribute__((always_inline)) _MAC_Bridge_GetPktFlags(TCPIP_MAC_PACKET* pPkt)
{
    return pPkt->pktClientData[0];
}

// stores the forward descriptor in the packet
// since this is either the bridge copy or the host does not process it
// we use the pktClientData[1:2] for storing the descriptor
static __inline__ void __attribute__((always_inline)) _MAC_Bridge_StorePktFwdDcpt(TCPIP_MAC_PACKET* pPkt, MAC_BRIDGE_FWD_DCPT* pFwdDcpt)
{
    MAC_BRIDGE_FWD_DCPT** pStoreDcpt = (MAC_BRIDGE_FWD_DCPT**)(pPkt->pktClientData + 1);
    // should be properly aligned
    _Mac_Bridge_AssertCond( ((uintptr_t)pStoreDcpt & (sizeof(uintptr_t) - 1)) == 0, __func__, __LINE__);
    
    *pStoreDcpt = pFwdDcpt;
}

// returns the forward descriptor of the packet
static __inline__ MAC_BRIDGE_FWD_DCPT* __attribute__((always_inline)) _MAC_Bridge_GetPktFwdDcpt(TCPIP_MAC_PACKET* pPkt)
{
    MAC_BRIDGE_FWD_DCPT** pStoreDcpt = (MAC_BRIDGE_FWD_DCPT**)(pPkt->pktClientData + 1);
    // should be properly aligned
    _Mac_Bridge_AssertCond( ((uintptr_t)pStoreDcpt & (sizeof(uintptr_t) - 1)) == 0, __func__, __LINE__);
    
    return *pStoreDcpt;
}


#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0) 
static __inline__ MAC_BRIDGE_DCPT* __attribute__((always_inline)) _MAC_Bridge_DcptFromHandle(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    return (MAC_BRIDGE_DCPT*)brHandle;
}
static TCPIP_MAC_BRIDGE_RESULT _MAC_Bridge_FDBLock(TCPIP_MAC_BRIDGE_HANDLE brHandle, bool validate);
static void _MAC_Bridge_FDBUnlock(MAC_BRIDGE_DCPT* pDcpt);
#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0) 



bool TCPIP_MAC_Bridge_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_MAC_BRIDGE_CONFIG* pBConfig)
{
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart; reset the interface as being bridge if needed
        if(gBridgeDcpt != 0)
        {
            int brPort = _MAC_Bridge_IfBridged(gBridgeDcpt, stackCtrl->netIx);
            if(brPort >= 0)
            {
                _TCPIPStack_BridgeSetIf(stackCtrl->pNetIf, brPort);
            }
        }
        return true;
    }

    // stack init
    while(bridgeInitCount == 0)
    {   // first time we run
        // basic sanity check
        
        if(pBConfig == 0)
        {   // missing initialization data
            _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_NO_INIT_DATA);
            return false;
        }

        if(pBConfig->fdbEntries == 0 || pBConfig->bridgeTable == 0 || pBConfig->bridgeTableSize == 0 || pBConfig->bridgeTableSize > TCPIP_MAC_BRIDGE_MAX_PORTS_NO
                            || pBConfig->bridgeTableSize > MAC_BRIDGE_MAX_SUPPORTED_PORTS || (pBConfig->pktPoolSize != 0 && pBConfig->pktSize == 0) || pBConfig->dcptPoolSize == 0)
        {   // wrong initialization data
            _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_INIT_DATA_ERROR);
            return false;
        }

        // unbridge all the network interfaces. just in case the stack is restarting
        int ix;
        int nIfs = TCPIP_STACK_NumberOfNetworksGet();
        TCPIP_NET_IF*   pNetIf;
        for(ix = 0; ix < nIfs; ix++)
        {
            pNetIf = _TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(ix));
            _TCPIPStack_BridgeClearIf(pNetIf);
        }


        gBridgeDcpt = 0;
        memset(&bridgeDcpt, 0, sizeof(bridgeDcpt));
        int nPorts = _MAC_Bridge_SetPorts(&bridgeDcpt, pBConfig);
        if(nPorts < 0)
        {   // failed, wrong initialization data
            _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_INIT_PORT_ERROR);
            return false;
        }

        bridgeDcpt.nPorts = nPorts;

        // create the FDB
        bridgeDcpt.memH = stackCtrl->memH;
        size_t hashMemSize = sizeof(OA_HASH_DCPT) + pBConfig->fdbEntries * sizeof(MAC_BRIDGE_HASH_ENTRY);
        OA_HASH_DCPT* hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(bridgeDcpt.memH, hashMemSize);

        if(hashDcpt == 0)
        {   // failed
            _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_FDB_ALLOC_ERROR);
            return false;
        }

        // populate the entries
        hashDcpt->memBlk = hashDcpt + 1;
        hashDcpt->hParam = 0;
        hashDcpt->hEntrySize = sizeof(MAC_BRIDGE_HASH_ENTRY);
        hashDcpt->hEntries = pBConfig->fdbEntries;
        hashDcpt->probeStep = MAC_BRIDGE_HASH_PROBE_STEP;

        hashDcpt->hashF = _MAC_BridgeHashKeyHash;
        hashDcpt->delF = _MAC_BridgeHashEntryDelete;
        hashDcpt->cmpF = _MAC_BridgeHashKeyCompare;
        hashDcpt->cpyF = _MAC_BridgeHashKeyCopy; 
        TCPIP_OAHASH_Initialize(hashDcpt);

        bridgeDcpt.hashDcpt = hashDcpt;
        TCPIP_MAC_BRIDGE_RESULT resPerm = _MAC_Bridge_SetPermEntries(&bridgeDcpt, pBConfig);
        if(resPerm != TCPIP_MAC_BRIDGE_RES_OK) 
        {
            _Mac_Bridge_InitCond(0, resPerm);
            _MAC_Bridge_Cleanup();
            return false;
        }
        
        uint64_t tmoTicks = pBConfig->purgeTimeout != 0 ? pBConfig->purgeTimeout : TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT; 
        tmoTicks *= SYS_TMR_TickCounterFrequencyGet();
        if(tmoTicks > 0xffffffff / 2)
        {   // requested timeout too great
            _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_TIMEOUT_ERROR);
            _MAC_Bridge_Cleanup();
            return false;
        }
        bridgeDcpt.purgeTmo = (uint32_t)tmoTicks;
        bridgeDcpt.transitDelay = (uint32_t)pBConfig->transitDelay * SYS_TMR_TickCounterFrequencyGet();

        bridgeDcpt.pktPoolSize = pBConfig->pktPoolSize;
        bridgeDcpt.pktSize = pBConfig->pktSize;
        bridgeDcpt.pktReplenish = pBConfig->pktReplenish;
        bridgeDcpt.dcptReplenish = pBConfig->dcptReplenish;
        bridgeDcpt.bridgeFlags = pBConfig->bridgeFlags;

        TCPIP_Helper_SingleListInitialize(&bridgeDcpt.pktPool);
        TCPIP_Helper_SingleListInitialize(&bridgeDcpt.dcptPool);

        if(bridgeDcpt.pktPoolSize != 0)
        {   // allocate packets
            int nAllocPkts = _MAC_Bridge_AllocatePackets(bridgeDcpt.pktPoolSize, &bridgeDcpt.pktPool, bridgeDcpt.pktSize, 
                    MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN | MAC_BRIDGE_ALLOC_FLAG_STICKY);

            if(nAllocPkts != bridgeDcpt.pktPoolSize)
            {
                _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_PKT_ALLOC_ERROR);
                _MAC_Bridge_Cleanup();
                return false;
            }
        }
        bridgeDcpt.stat.pktPoolLowSize = bridgeDcpt.pktPoolSize; 
        
        // allocate descriptors
        int nAllocDcpts = _MAC_Bridge_AllocateDescriptors(&bridgeDcpt, pBConfig->dcptPoolSize, &bridgeDcpt.dcptPool);

        if(nAllocDcpts != pBConfig->dcptPoolSize)
        {
            _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_DCPT_ALLOC_ERROR);
            _MAC_Bridge_Cleanup();
            return false;
        }

        bridgeDcpt.stat.dcptPoolLowSize = pBConfig->dcptPoolSize; 

        bridgeDcpt.tmrSigHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_MAC_Bridge_Task, TCPIP_MAC_BRIDGE_TASK_RATE);
        if(bridgeDcpt.tmrSigHandle == 0)
        {
            _Mac_Bridge_InitCond(0, TCPIP_MAC_BRIDGE_RES_SIGNAL_ERROR);
            _MAC_Bridge_Cleanup();
            return false;
        }
        
        bridgeDcpt.status = SYS_STATUS_BUSY;    // waiting to complete the initialization
        gBridgeDcpt = &bridgeDcpt;

        break;
    }


    bridgeInitCount++;
    return true;

}

TCPIP_MAC_BRIDGE_HANDLE TCPIP_MAC_Bridge_Open(const SYS_MODULE_INDEX bridgeIndex)
{
    if(bridgeIndex == 0)
    {
        return gBridgeDcpt;
    }

    return 0;
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

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--bridgeInitCount == 0)
            {   // all closed
                // release resources
                _MAC_Bridge_Cleanup();
            }
        }
    }

}

static void _MAC_Bridge_Cleanup(void)
{
    if(bridgeDcpt.tmrSigHandle != 0)
    {
        _TCPIPStackSignalHandlerDeregister(bridgeDcpt.tmrSigHandle);
        bridgeDcpt.tmrSigHandle = 0;
    }

    if(bridgeDcpt.hashDcpt != 0)
    {
        TCPIP_OAHASH_EntriesRemoveAll(bridgeDcpt.hashDcpt);

        TCPIP_HEAP_Free(bridgeDcpt.memH, bridgeDcpt.hashDcpt);
        bridgeDcpt.hashDcpt = 0;
    }

    TCPIP_MAC_PACKET* pBPkt;
    while( (pBPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&bridgeDcpt.pktPool)) != 0)
    {
        TCPIP_PKT_PacketFree((TCPIP_MAC_PACKET*)pBPkt);
    }

    MAC_BRIDGE_FWD_DCPT* pFwdDcpt;
    while( (pFwdDcpt = (MAC_BRIDGE_FWD_DCPT*)TCPIP_Helper_SingleListHeadRemove(&bridgeDcpt.dcptPool)) != 0)
    {
        TCPIP_HEAP_Free(bridgeDcpt.memH, pFwdDcpt);
    }

    gBridgeDcpt = 0;
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
//      there could a dynamic entry for this ucast address
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
    MAC_BRIDGE_FWD_DCPT fwdDcpt, *pFwdDcpt;
    TCPIP_MAC_PACKET* pFwdPkt;
    TCPIP_MAC_BRIDGE_EVENT brEvent;

    TCPIP_NET_IF* pInIf = (TCPIP_NET_IF*)pRxPkt->pktIf;

    if(gBridgeDcpt == 0 || _TCPIPStack_BridgeCheckIf(pInIf) == false)
    {   // not for us
        return TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS;
    }

#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    TCPIP_MAC_BRIDGE_RESULT res = _MAC_Bridge_FDBLock(gBridgeDcpt, false);
    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {   // couldn't get a lock
        _MAC_Bridge_NotifyEvent(gBridgeDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK, 0);
        _MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_LOCK, 1);
        return TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS;
    }
#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)



    uint8_t inPort = _TCPIPStack_BridgeGetIfPort(pInIf);
    _MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, MAC_BRIDGE_STAT_TYPE_RX_PKTS, 1);
    _MAC_Bridge_TracePkt(pRxPkt, inPort, false);

    // learn
    TCPIP_MAC_ETHERNET_HEADER*  pMacHdr = (TCPIP_MAC_ETHERNET_HEADER*)pRxPkt->pMacLayer;
    heSrc = 0;
    if(!TCPIP_Helper_IsMcastMACAddress(&pMacHdr->SourceMACAddr))
    {   // we shouldn't learn multicast addresses
        // update destination in the FDB
        _MAC_Bridge_CheckFDB(gBridgeDcpt);
        brEvent = TCPIP_MAC_BRIDGE_EVENT_NONE;
        if((gBridgeDcpt->bridgeFlags & TCPIP_MAC_BRIDGE_FLAG_NO_DYNAMIC_LEARN) != 0)
        {
            heSrc = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(gBridgeDcpt->hashDcpt, pMacHdr->SourceMACAddr.v);
        }
        else
        {
            heSrc = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(gBridgeDcpt->hashDcpt, pMacHdr->SourceMACAddr.v);
            if(heSrc == 0)
            {
                brEvent = TCPIP_MAC_BRIDGE_EVENT_FDB_FULL;
                _MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_TYPE_FDB_FULL, 1);
            }
        }

        if(heSrc != 0)
        {   // (new) dynamic/learnt entry needs to be updated
            _MAC_Bridge_SetHashDynamicEntry(heSrc, inPort, MAC_BRIDGE_HFLAG_NONE, MAC_BRIDGE_HFLAG_PORT_VALID); 
            if(heSrc->hEntry.flags.newEntry != 0)
            {
                brEvent = TCPIP_MAC_BRIDGE_EVENT_ENTRY_ADDED;
            }
        }

        if(brEvent != TCPIP_MAC_BRIDGE_EVENT_NONE)
        {
            _MAC_Bridge_NotifyEvent(gBridgeDcpt, brEvent, pMacHdr->SourceMACAddr.v);
        }
        _MAC_Bridge_CheckFDB(gBridgeDcpt);
    }

    // check if destination is a reserved address
    // IEEE 802.1D-2004 - 7.12.6:
    //      Frames containing any of the group MAC Addresses specified in Table 7-10
    //      in their destination address field shall not be relayed by the Bridge.
    //      They are configured in the Permanent Database:
    //      01-80-C2-00-00-00 - 01-80-C2-00-00-0F
    //      We test directly for these addresses without taking slots in the FDB

    if(memcmp(pMacHdr->DestMACAddr.v, bridge_reserved_address, sizeof(bridge_reserved_address) - 1) == 0)
    {
        if((pMacHdr->DestMACAddr.v[5] & 0xf0) == 0x00)
        {   // reserved address
            _MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, MAC_BRIDGE_STAT_TYPE_RESERVED_PKTS, 1);
#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
            _MAC_Bridge_FDBUnlock(gBridgeDcpt);
#endif // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
            TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_BRIDGE_DISCARD); 
            return TCPIP_MAC_BRIDGE_PKT_RES_BRIDGE_DISCARD;
        }
    } 
            
    // debug check point
    _MAC_Bridge_CheckRxIpPkt(pRxPkt);

    // forward
    fwdDcpt.tReceive = SYS_TMR_TickCountGet();
    fwdDcpt.pktLen = TCPIP_PKT_PayloadLen(pRxPkt);
    

    // check destination is in the FDB
    _MAC_Bridge_CheckFDB(gBridgeDcpt);
    heDest = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(gBridgeDcpt->hashDcpt, pMacHdr->DestMACAddr.v);

    if(TCPIP_Helper_IsMcastMACAddress(&pMacHdr->DestMACAddr))
    {
        _Mac_Bridge_AssertCond(heDest == 0 || (heDest->hEntry.flags.value & (MAC_BRIDGE_HFLAG_STATIC | MAC_BRIDGE_HFLAG_HOST)) == MAC_BRIDGE_HFLAG_STATIC, __func__, __LINE__);
        fwdDcpt.fwdFlags = MAC_BRIDGE_FWD_FLAG_MCAST;
        _MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, MAC_BRIDGE_STAT_TYPE_RX_DEST_MCAST, 1);
    }
    else
    {
        fwdDcpt.fwdFlags = MAC_BRIDGE_FWD_FLAG_NONE;
        _MAC_Bridge_StatPortUpdate(gBridgeDcpt, inPort, (heDest == 0 || (heDest->hEntry.flags.value & MAC_BRIDGE_HFLAG_HOST) == 0) ? MAC_BRIDGE_STAT_TYPE_RX_DEST_NOT_ME_UCAST : MAC_BRIDGE_STAT_TYPE_RX_DEST_ME_UCAST, 1);
    } 

    TCPIP_MAC_BRIDGE_PKT_RES pktRes = TCPIP_MAC_BRIDGE_PKT_RES_BRIDGE_PROCESS; 
    _MAC_Bridge_ClearMap(&fwdDcpt);

    if((gBridgeDcpt->bridgeFlags & TCPIP_MAC_BRIDGE_FLAG_FDB_ENTRY_EXISTS) == 0 || (heDest != 0 || heSrc != 0))
    {   // regular forwarding or only if we have the src/dest in FDB
        _MAC_Bridge_SetPacketForward(gBridgeDcpt, heDest, inPort, &fwdDcpt);
    }

    if(!_MAC_Bridge_IsMapEmpty(&fwdDcpt))
    {   // check if the host processes this packet
        if(_MAC_Bridge_IsPortMapped(inPort, &fwdDcpt))
        {
            pktRes = TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS;
            _MAC_Bridge_DelPortMap(inPort, &fwdDcpt); // remove incoming port from forwarding
        }
    }

    if(!_MAC_Bridge_IsMapEmpty(&fwdDcpt))
    {   // packet must be forwarded on some ports 

        pFwdDcpt = _MAC_Bridge_GetFwdDcpt(gBridgeDcpt);
        if(pFwdDcpt != 0)
        {   // have valid descriptor
            if(pktRes == TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS)
            {   // host needs to process this packet; we need a copy
                pFwdPkt = _MAC_Bridge_GetFwdPkt(gBridgeDcpt, fwdDcpt.pktLen);
                if(pFwdPkt != 0)
                {   // ok, we can flood/forward
                    _MAC_Bridge_PacketCopy(pRxPkt, pFwdPkt, fwdDcpt.pktLen, heDest);
                }
            }
            else
            {   // use directly the incoming packet
                pFwdPkt = pRxPkt;
                _MAC_Bridge_SetPktFlags(pFwdPkt, MAC_BRIDGE_ALLOC_FLAG_NONE);
            }

            if(pFwdPkt != 0)
            {   // start forwarding away...
                // set the descriptor
                *pFwdDcpt = fwdDcpt;
                pFwdDcpt->ownAckFunc = pRxPkt->ackFunc;
                pFwdDcpt->ownAckParam = pRxPkt->ackParam;
                _MAC_Bridge_StorePktFwdDcpt(pFwdPkt, pFwdDcpt);

                pFwdPkt->ackFunc = _MAC_Bridge_PacketAck; 
                pFwdPkt->ackParam = heDest; 
                // adjust: the source packet comes from the MAC driver with segLen adjusted
                pFwdPkt->pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);

                _MAC_Bridge_ForwardPacket(pFwdPkt, heDest);
            }
        }
    }

#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    _MAC_Bridge_FDBUnlock(gBridgeDcpt);
#endif // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)

    return pktRes;
}

// periodic task
// purge the dynamic entries in the FDB
void TCPIP_MAC_Bridge_Task(void)
{
    if(gBridgeDcpt == 0 || gBridgeDcpt->status < 0)
    {   // not up or error
        return;
    }

    if(gBridgeDcpt->status == SYS_STATUS_BUSY)
    {   // try to complete the initialization
        // get the stack handle; it should be running
        SYS_MODULE_OBJ stackObj = TCPIP_STACK_Initialize(0, 0);
        SYS_STATUS stackStat = TCPIP_STACK_Status(stackObj);
        if(stackStat < 0)
        {   // some error has occurred
            gBridgeDcpt->status = SYS_STATUS_ERROR;
            return;
        }
        else if(stackStat != SYS_STATUS_READY)
        {   // wait some more
           return;
        }

        // proceed with the initialization
        TCPIP_MAC_BRIDGE_RESULT resHost = _MAC_Bridge_SetHostEntries(gBridgeDcpt);
        if(resHost != TCPIP_MAC_BRIDGE_RES_OK)
        {   // some error has occurred
            gBridgeDcpt->status = SYS_STATUS_ERROR;
            return;
        }

        // all good
        gBridgeDcpt->status = SYS_STATUS_READY;
    }

#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    TCPIP_MAC_BRIDGE_RESULT res = _MAC_Bridge_FDBLock(gBridgeDcpt, false);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {   // couldn't get a lock
        _MAC_Bridge_NotifyEvent(gBridgeDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK, 0);
        _MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_LOCK, 1);
        return;
    }
#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)

    // traverse the FDB for periodic maintenance
    int ix;
    MAC_BRIDGE_HASH_ENTRY* hE;
    int nEntries = gBridgeDcpt->hashDcpt->hEntries;

    _MAC_Bridge_CheckFDB(gBridgeDcpt);
    uint32_t currTime = SYS_TMR_TickCountGet();
    for(ix = 0; ix < nEntries; ix++)
    {
        hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(gBridgeDcpt->hashDcpt, ix);
        if(hE->hEntry.flags.busy != 0 && (hE->hEntry.flags.value & MAC_BRIDGE_HFLAG_STATIC ) == 0)
        {   // in use dynamic entry
            if((int32_t)(currTime - hE->tExpire) > 0)
            {   // expired entry; remove
                _MAC_Bridge_NotifyEvent(gBridgeDcpt, TCPIP_MAC_BRIDGE_EVENT_ENTRY_EXPIRED, hE->destAdd.v);
                TCPIP_OAHASH_EntryRemove(gBridgeDcpt->hashDcpt, &hE->hEntry);
            }
        }
    }
    _MAC_Bridge_CheckFDB(gBridgeDcpt);

#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    _MAC_Bridge_FDBUnlock(gBridgeDcpt);
#endif // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
}

SYS_STATUS TCPIP_MAC_Bridge_Status(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    MAC_BRIDGE_DCPT* bDcpt = _MAC_Bridge_ValidateHandle(brHandle);

    return bDcpt == 0 ? SYS_STATUS_UNINITIALIZED : bDcpt->status;
}

// low level and local functionality implementation

// allocate n bridge packets of a required size
static int _MAC_Bridge_AllocatePackets(int nPkts, SINGLE_LIST* addList, uint16_t pktSize, uint8_t allocFlags)
{
    int ix;
    TCPIP_MAC_PACKET* pBPkt;

    for(ix = 0; ix < nPkts; ix++)
    { 
        pBPkt = (TCPIP_MAC_PACKET*)TCPIP_PKT_PacketAlloc( sizeof(*pBPkt), pktSize, 0);
        if(pBPkt == 0)
        {   // out of mem
            break;
        }

        _MAC_Bridge_SetPktFlags(pBPkt, allocFlags);
        TCPIP_Helper_SingleListTailAdd(addList, (SGL_LIST_NODE*)pBPkt); 
    }

    return ix;
}

static int _MAC_Bridge_AllocateDescriptors(MAC_BRIDGE_DCPT* pBDcpt, int nDcpts, SINGLE_LIST* addList)
{
    int ix;
    MAC_BRIDGE_FWD_DCPT *dcptArray, *pFwdDcpt;

    dcptArray = (MAC_BRIDGE_FWD_DCPT*)TCPIP_HEAP_Calloc(pBDcpt->memH, nDcpts, sizeof(MAC_BRIDGE_FWD_DCPT));
    if(dcptArray == 0)
    {   // out of mem
        return 0;
    }

    // append to the list
    pFwdDcpt = dcptArray;
    for(ix = 0; ix < nDcpts; ix++, pFwdDcpt++)
    {
        TCPIP_Helper_SingleListTailAdd(addList, (SGL_LIST_NODE*)pFwdDcpt); 
    }

    return nDcpts;
}

// gets a packet for forwarding
// either from the pool or a new allocation is made
static TCPIP_MAC_PACKET* _MAC_Bridge_GetFwdPkt(MAC_BRIDGE_DCPT* pBDcpt, uint16_t pktLen)
{
    TCPIP_MAC_PACKET* pPkt;
    int nAllocPkts;

    // get a bridge packet
    while(true)
    {
        if(pBDcpt->pktPoolSize != 0)
        {   // grab a packet from the pool

            // however, need to check the size of the packets in the pool 
            if(pktLen > pBDcpt->pktSize - sizeof(TCPIP_MAC_ETHERNET_HEADER))
            {   // if this packet is too big for the bridge, just drop it
                _MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_SIZE, (const void*)(uintptr_t)pktLen);
                _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_SIZE, 1);
                return 0;
            }

            pPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&pBDcpt->pktPool);
#if (TCPIP_MAC_BRIDGE_STATISTICS != 0)
            uint32_t lowSize = TCPIP_Helper_SingleListCount(&pBDcpt->pktPool);
            if(lowSize < pBDcpt->stat.pktPoolLowSize)
            {
                pBDcpt->stat.pktPoolLowSize = lowSize;
            }
#endif  // (TCPIP_MAC_BRIDGE_STATISTICS != 0)

            if(pPkt != 0)
            {   // done
                break;
            }

            // pPkt == 0; pool was empty
            _MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_PKT_POOL_EMPTY, 0);
            _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_PKT_POOL_EMPTY, 1);

            int replPkts = pBDcpt->pktReplenish;
            if(replPkts != 0)
            {   // try to replenish the packet pool
                nAllocPkts = _MAC_Bridge_AllocatePackets(replPkts, &pBDcpt->pktPool, pBDcpt->pktSize, MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN);
                if(nAllocPkts != replPkts)
                {
                    _MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC, (const void*)(uintptr_t)(replPkts - nAllocPkts));
                    _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_PKT_ALLOC, replPkts - nAllocPkts);
                }

                _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_ALLOC_PKTS, nAllocPkts);

                if(nAllocPkts != 0)
                {
                    pPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&pBDcpt->pktPool);
                    _Mac_Bridge_AssertCond(pPkt != 0, __func__, __LINE__);
                }
            }
            break;
        }

        // there is no pool
        // simply allocate a new packet
        SINGLE_LIST pktList = {0};
        nAllocPkts = _MAC_Bridge_AllocatePackets(1, &pktList, pktLen, MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN);
        if(nAllocPkts == 0)
        {

            _MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC, (const void*)1);
            _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_PKT_ALLOC, 1);
            pPkt = 0;
        }
        else
        {
            _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_ALLOC_PKTS, 1);
            pPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&pktList);
            _Mac_Bridge_AssertCond(pPkt != 0, __func__, __LINE__);
        }

        break;
    }

    return pPkt;
}

// gets a descriptor for forwarding
// either from the pool or a new allocation is made
static MAC_BRIDGE_FWD_DCPT* _MAC_Bridge_GetFwdDcpt(MAC_BRIDGE_DCPT* pBDcpt)
{
    MAC_BRIDGE_FWD_DCPT* pFwdDcpt;
    int nAllocDcpts;

    // grab a descriptor from the pool
    pFwdDcpt = (MAC_BRIDGE_FWD_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pBDcpt->dcptPool);
#if (TCPIP_MAC_BRIDGE_STATISTICS != 0)
    uint32_t lowSize = TCPIP_Helper_SingleListCount(&pBDcpt->dcptPool);
    if(lowSize < pBDcpt->stat.dcptPoolLowSize)
    {
        pBDcpt->stat.dcptPoolLowSize = lowSize;
    }
#endif  // (TCPIP_MAC_BRIDGE_STATISTICS != 0)

    if(pFwdDcpt == 0)
    {   // pFwdDcpt == 0; descriptor pool was empty
        _MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_DCPT_POOL_EMPTY, 0);
        _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_DCPT_POOL_EMPTY, 1);

        int replDcpts = pBDcpt->dcptReplenish;
        if(replDcpts != 0)
        {
            nAllocDcpts = _MAC_Bridge_AllocateDescriptors(pBDcpt, replDcpts, &pBDcpt->dcptPool);
            if(nAllocDcpts != replDcpts)
            {
                _MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_DCPT_ALLOC, (const void*)(uintptr_t)(replDcpts - nAllocDcpts));
                _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_DCPT_ALLOC, replDcpts - nAllocDcpts);
            }

            _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_ALLOC_DCPTS, nAllocDcpts);

            if(nAllocDcpts != 0)
            {
                pFwdDcpt = (MAC_BRIDGE_FWD_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pBDcpt->dcptPool);
                _Mac_Bridge_AssertCond(pFwdDcpt != 0, __func__, __LINE__);
            }
        }
    }

    return pFwdDcpt;
}

// populate a descriptor with the flood ports for a unicast/multicast destination address
// based on the hash entry and the input port
// on entry, pFDcpt holds the valid packet length and multicast/unicast flags 
// Note: the pFDcpt bit for the inPort shows the forward/filter for the internal host! 
static void _MAC_Bridge_SetPacketForward(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* hDest, uint8_t inPort, MAC_BRIDGE_FWD_DCPT* pFDcpt)
{
    int portIx, startPort, endPort, learnPort;
    int outIfIx;
    uint8_t* pOutMap;
    TCPIP_NET_IF* pOutIf;
    TCPIP_MAC_BRIDGE_CONTROL_TYPE portControl, defOutControl, defHostControl;
    uint16_t linkMtu;

    if(hDest == 0 || ((hDest->hEntry.flags.value & (MAC_BRIDGE_HFLAG_STATIC | MAC_BRIDGE_HFLAG_PORT_VALID)) == 0) )
    {   // no such entry in our FDB or invalid dynamic entry; forward an all other ports;
        // default action for internal host is filter
        pOutMap = 0;
        startPort = 0;
        endPort = pBDcpt->nPorts;
        defOutControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD; 
        defHostControl = (pFDcpt->fwdFlags & MAC_BRIDGE_FWD_FLAG_MCAST) != 0 ? TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD: TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER; 
    }
    else if((hDest->hEntry.flags.value & MAC_BRIDGE_HFLAG_STATIC ) == 0)
    {   // valid dynamic entry; forward to learnPort only
        // default action for internal host is filter
        pOutMap = 0;
        startPort = hDest->learnPort;
        endPort = startPort + 1;
        defOutControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD; 
        defHostControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER; 
    }
    else if((hDest->hEntry.flags.value & MAC_BRIDGE_HFLAG_HOST ) != 0)
    {   // static entry for our own host
        // get the forward/filter behavior from the outPortMap,
        // including the outPortMap[inPort][inPort] for the host processing
        pOutMap = hDest->outPortMap[inPort];
        startPort = 0;
        endPort = pBDcpt->nPorts;
        defOutControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER; 
        defHostControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD; 
    }
    else
    {   // static entry but not for our host 
        // get the forward/filter behavior from the outPortMap,
        // including the outPortMap[inPort][inPort] for the host processing
        pOutMap = hDest->outPortMap[inPort];
        startPort = 0;
        endPort = pBDcpt->nPorts;
        if((pFDcpt->fwdFlags & MAC_BRIDGE_FWD_FLAG_MCAST) != 0)
        {
            defHostControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD;
            defOutControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD; 
        }
        else
        {
            defHostControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER; 
            if((hDest->hEntry.flags.value & MAC_BRIDGE_HFLAG_PORT_VALID) != 0)
            {   // we have a learnt port for this dest address
                learnPort = (int)hDest->learnPort;
                defOutControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_DYN_LEARN; 
            }
            else
            {   // no info for this dest address
                defOutControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD; 
            }
        }
    }
    

    // traverse all ports and construct the map
    // remove ports for which this packet is too big
    for(portIx = startPort; portIx < endPort; portIx++)
    {
        if(pOutMap != 0)
        {
            portControl = (TCPIP_MAC_BRIDGE_CONTROL_TYPE)*pOutMap++;
        }
        else
        {
            portControl = TCPIP_MAC_BRIDGE_CONTROL_TYPE_DEFAULT;
        }

        if(portIx == inPort)
        {   // this host control
            if(portControl == TCPIP_MAC_BRIDGE_CONTROL_TYPE_DEFAULT)
            {
                portControl = defHostControl;
            }

            if(portControl == TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD)
            {   // port needs flooding
                _MAC_Bridge_AddPortMap(portIx, pFDcpt);
            }
            continue;
        }

        // output port control
        if(portControl == TCPIP_MAC_BRIDGE_CONTROL_TYPE_DEFAULT)
        {
            portControl = defOutControl; 
        }

        if(portControl == TCPIP_MAC_BRIDGE_CONTROL_TYPE_DYN_LEARN)
        {   // special case, need to use the learnPort 
            portControl = (portIx == learnPort) ? TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD : TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER; 
        }

        if(portControl == TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD)
        {   // port needs forwarding
            // check it's not too large to send on the output interface
            outIfIx = pBDcpt->port2IfIx[portIx];
            pOutIf = _TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(outIfIx));
            linkMtu = _TCPIPStackNetLinkMtu(pOutIf);
            if(pFDcpt->pktLen <= linkMtu)
            {   
                _MAC_Bridge_AddPortMap(portIx, pFDcpt);
            }
            else
            {   // discard packets too large for interface
                _MAC_Bridge_NotifyEvent(pBDcpt, TCPIP_MAC_BRIDGE_EVENT_FAIL_MTU, (const void*)(uintptr_t)pFDcpt->pktLen);
                _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_MTU, 1);
            }
        }
        // else filter
    }
        
}

#if (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
static void _MAC_Bridge_NotifyEvent(MAC_BRIDGE_DCPT* pBDcpt, TCPIP_MAC_BRIDGE_EVENT event, const void* evParam)
{
    if(pBDcpt->evHandler != 0)
    {
        pBDcpt->evHandler(event, evParam);
    }
}

TCPIP_MAC_BRIDGE_EVENT_HANDLE TCPIP_MAC_Bridge_EventHandlerRegister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLER evHandler)
{
    TCPIP_MAC_BRIDGE_EVENT_HANDLE evHandle = 0;
    MAC_BRIDGE_DCPT* bDcpt = _MAC_Bridge_ValidateHandle(brHandle);

    if(bDcpt != 0)
    {
        if(bDcpt != 0 && bDcpt->evHandler == 0)
        {
            bDcpt->evHandler = evHandler;
            evHandle = bDcpt->evHandler;
        }
    }

    return evHandle;
}

bool TCPIP_MAC_Bridge_EventHandlerDeregister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLE evHandle)
{
    MAC_BRIDGE_DCPT* bDcpt = _MAC_Bridge_ValidateHandle(brHandle);
    if(bDcpt != 0)
    {
        if(bDcpt->evHandler == evHandle)
        {
            bDcpt->evHandler = 0;
            return true;
        }
    }

    return false;
}

#endif  // (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 

size_t TCPIP_MAC_Bridge_FDBEntries(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    MAC_BRIDGE_DCPT* bDcpt = _MAC_Bridge_ValidateHandle(brHandle);
    if(bDcpt != 0)
    {
        return bDcpt->hashDcpt->hEntries;
    }

    return 0;
}

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBIndexRead(TCPIP_MAC_BRIDGE_HANDLE brHandle, size_t ix, TCPIP_MAC_FDB_ENTRY* pEntry)
{
    MAC_BRIDGE_HASH_ENTRY h1, h2;
    MAC_BRIDGE_DCPT* bDcpt = _MAC_Bridge_ValidateHandle(brHandle);

    if(bDcpt == 0)
    {
        return TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR;
    }

    MAC_BRIDGE_HASH_ENTRY* hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(bDcpt->hashDcpt, ix);

    if(hE == 0)
    {
        return TCPIP_MAC_BRIDGE_RES_INDEX_ERROR; 
    }


    // obtain a consistent reading
    while(true)
    {
        memcpy(&h1, hE, sizeof(h1));
        memcpy(&h2, hE, sizeof(h2));
        if(memcmp(&h1, &h2, sizeof(h1)) == 0)
        {   // match
            break;
        }
    }

    if(h1.hEntry.flags.busy != 0)
    {
        if(pEntry)
        {
            memcpy(pEntry->destAdd.v, h1.destAdd.v, sizeof(h1.destAdd));
            pEntry->flags = (uint8_t)(h1.hEntry.flags.value >> 8);
            pEntry->learnPort = h1.learnPort;
            pEntry->tExpire = h1.tExpire;
            pEntry->fwdPackets = h1.fwdPackets;
            memcpy(pEntry->outPortMap, h1.outPortMap, sizeof(h1.outPortMap));
        }

        return TCPIP_MAC_BRIDGE_RES_OK;
    }

    return TCPIP_MAC_BRIDGE_RES_INDEX_NO_ENTRY;
}

#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
static TCPIP_MAC_BRIDGE_RESULT _MAC_Bridge_FDBLock(TCPIP_MAC_BRIDGE_HANDLE brHandle, bool validate)
{
    MAC_BRIDGE_DCPT* pDcpt;

    if(validate)
    {
        pDcpt = _MAC_Bridge_ValidateHandle(brHandle);
        if(pDcpt == 0)
        {
            return TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR;
        }
    }
    else
    {
        pDcpt = _MAC_Bridge_DcptFromHandle(brHandle); 
    }

    TCPIP_MAC_BRIDGE_RESULT res;
    // make sure the user threads don't mess with the access queue
    OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(pDcpt->bridgeLock)
    {
        res = TCPIP_MAC_BRIDGE_RES_LOCK_ERROR;
    }
    else
    {
        pDcpt->bridgeLock = true;
        res = TCPIP_MAC_BRIDGE_RES_OK;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);

    return res;
}

static void _MAC_Bridge_FDBUnlock(MAC_BRIDGE_DCPT* pDcpt)
{
    OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pDcpt->bridgeLock = false;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);
}


TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBReset(TCPIP_MAC_BRIDGE_HANDLE brHandle)
{
    // delete all dynamic data from the FDB
    int ix;
    MAC_BRIDGE_HASH_ENTRY* hE;
    int nEntries;

    // lock access: make sure the user threads don't mess with the FDB
    TCPIP_MAC_BRIDGE_RESULT res = _MAC_Bridge_FDBLock(brHandle, true);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {
        return res;
    }

    MAC_BRIDGE_DCPT* pDcpt = _MAC_Bridge_DcptFromHandle(brHandle);

    nEntries = pDcpt->hashDcpt->hEntries;

    for(ix = 0; ix < nEntries; ix++)
    {
        hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pDcpt->hashDcpt, ix);
        if(hE->hEntry.flags.busy != 0)
        {   // in use
            if((hE->hEntry.flags.value & MAC_BRIDGE_HFLAG_STATIC ) != 0)
            {   // static entry
                hE->hEntry.flags.value &= ~MAC_BRIDGE_HFLAG_PORT_VALID;
                hE->fwdPackets = 0;
            }
            else
            {   // dynamic entry
                TCPIP_OAHASH_EntryRemove(pDcpt->hashDcpt, &hE->hEntry);
            }
        }
    }

    // done
    _MAC_Bridge_FDBUnlock(pDcpt);
    return TCPIP_MAC_BRIDGE_RES_OK;
}

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBDeleteEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_ADDR* pDestAddr)
{

    // lock access: make sure the user threads don't mess with the FDB
    TCPIP_MAC_BRIDGE_RESULT res = _MAC_Bridge_FDBLock(brHandle, true);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {
        return res;
    }

    MAC_BRIDGE_DCPT* pDcpt = _MAC_Bridge_DcptFromHandle(brHandle);

    MAC_BRIDGE_HASH_ENTRY* hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pDcpt->hashDcpt, pDestAddr->v);

    if(hE != 0 && hE->hEntry.flags.busy != 0)
    {   // valid entry
        TCPIP_OAHASH_EntryRemove(pDcpt->hashDcpt, &hE->hEntry);
    }

    // done
    _MAC_Bridge_FDBUnlock(pDcpt);
    return TCPIP_MAC_BRIDGE_RES_OK;
}

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBAddEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPermEntry)
{
    if(pPermEntry == 0)
    {
        return TCPIP_MAC_BRIDGE_RES_PARAM_ERROR; 
    }
    // lock access: make sure the user threads don't mess with the FDB
    TCPIP_MAC_BRIDGE_RESULT res = _MAC_Bridge_FDBLock(brHandle, true);

    if(res != TCPIP_MAC_BRIDGE_RES_OK)
    {
        return res;
    }

    MAC_BRIDGE_DCPT* pDcpt = _MAC_Bridge_DcptFromHandle(brHandle);

    MAC_BRIDGE_HASH_ENTRY* hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(pDcpt->hashDcpt, pPermEntry->destAdd.v);

    if(hE != 0)
    {   // clear the outPortMap for a new entry, keep it for existent one
        _MAC_Bridge_SetHashStaticEntry(hE, 0, hE->hEntry.flags.newEntry != 0);
        // update the outPortMap
        res = _MAC_Bridge_SetFDBFromPerm(pDcpt, hE, pPermEntry);
    }
    else
    {
        res = TCPIP_MAC_BRIDGE_RES_FDB_FULL; 
    }

    // done
    _MAC_Bridge_FDBUnlock(pDcpt);
    return res;
}

#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)

static void _MAC_Bridge_SetHashDynamicEntry(MAC_BRIDGE_HASH_ENTRY* hE, uint8_t port, MAC_BRIDGE_HASH_FLAGS clrFlags, MAC_BRIDGE_HASH_FLAGS setFlags) 
{
    hE->hEntry.flags.value &= ~(clrFlags);
    hE->hEntry.flags.value |= setFlags;
    hE->learnPort = port;
    hE->tExpire = SYS_TMR_TickCountGet() + gBridgeDcpt->purgeTmo;
}

static void _MAC_Bridge_SetHashStaticEntry(MAC_BRIDGE_HASH_ENTRY* hE, MAC_BRIDGE_HASH_FLAGS flags, bool clearOutMap)
{
    hE->hEntry.flags.value &= ~(MAC_BRIDGE_HFLAG_MASK);
    hE->hEntry.flags.value |= (MAC_BRIDGE_HFLAG_STATIC | flags);

    if(clearOutMap)
    {
        memset(hE->outPortMap, 0, sizeof(hE->outPortMap));
    }
}


// forward a packet on an output interface
// if all done, it frees the allocated packet
static void _MAC_Bridge_ForwardPacket(TCPIP_MAC_PACKET* pFwdPkt, MAC_BRIDGE_HASH_ENTRY* hEntry)
{
    MAC_BRIDGE_FWD_DCPT* pFDcpt = _MAC_Bridge_GetPktFwdDcpt(pFwdPkt);
    uint16_t pktFlags = _MAC_Bridge_GetPktFlags(pFwdPkt);
    MAC_BRIDGE_DCPT* pBDcpt = gBridgeDcpt; 

    // check we've not exceeded the time
    bool pktDrop;
    if(SYS_TMR_TickCountGet() - pFDcpt->tReceive > pBDcpt->transitDelay)
    {
        _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_DELAY_PKTS, 1);
        pktDrop = true;
    }
    else
    {
        pktDrop = false;
    }

    while(!pktDrop)
    {
        // get the bridge port to process on
        int brPort = _MAC_Bridge_PortFromMap(pFDcpt);
        if(brPort < 0)
        {   // done, nothing else
            break;
        }

        // there is something to transmit

        // done with this port
        _MAC_Bridge_DelPortMap(brPort, pFDcpt);
        int outIf = pBDcpt->port2IfIx[brPort];
        TCPIP_NET_IF* pOutIf = _TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(outIf));

        // test that the netowrk IF hasn't been killed; there's no other test for this
        if(TCPIP_STACK_NetworkIsUp(pOutIf))
        {
            pFwdPkt->next = 0;   // send single packet
            if(_TCPIPStackPacketTx(pOutIf, pFwdPkt) >= 0)
            {   // successfully transmitted
                if((pFDcpt->fwdFlags & MAC_BRIDGE_FWD_FLAG_MCAST) != 0)
                {
                    _MAC_Bridge_StatPortUpdate(gBridgeDcpt, brPort, MAC_BRIDGE_STAT_TYPE_FWD_MCAST_PKTS, 1);
                }
                else
                {
                    _MAC_Bridge_StatPortUpdate(gBridgeDcpt, brPort, MAC_BRIDGE_STAT_TYPE_FWD_UCAST_PKTS, 1);
                }
                if(hEntry != 0)
                {
                    hEntry->fwdPackets++;
                }
                if((pktFlags & MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN) == 0)
                {
                    _MAC_Bridge_StatPortUpdate(gBridgeDcpt, brPort, MAC_BRIDGE_STAT_TYPE_FWD_DIRECT_PKTS, 1);
                }

                _MAC_Bridge_TracePkt(pFwdPkt, brPort, true);
                return;
            }

            // failed sending the packet 
            _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_FAIL_MAC, 1);
        }

        // try on another interface
    }

    // at this point the forward map is empty and we're done

    if((pktFlags & MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN) != 0)
    {   // bridge owned
        if((pktFlags & MAC_BRIDGE_ALLOC_FLAG_STICKY) != 0)
        {   // return to the pool
            TCPIP_Helper_SingleListTailAdd(&pBDcpt->pktPool, (SGL_LIST_NODE*)pFwdPkt); 
        }
        else
        {   // free the allocated packet
            TCPIP_PKT_PacketFree((TCPIP_MAC_PACKET*)pFwdPkt);
            _MAC_Bridge_StatUpdate(pBDcpt, MAC_BRIDGE_STAT_TYPE_FREE_PKTS, 1);
        }
    }
    else
    {   // restore the packet owners and acknowledge it
        pFwdPkt->ackParam = pFDcpt->ownAckParam;
        pFwdPkt->ackFunc = pFDcpt->ownAckFunc;
        TCPIP_PKT_PacketAcknowledge(pFwdPkt, TCPIP_MAC_PKT_ACK_BRIDGE_DONE); 
    }

    // re-add the fowrward descriptor to the pool
    TCPIP_Helper_SingleListTailAdd(&pBDcpt->dcptPool, (SGL_LIST_NODE*)pFDcpt); 
}


// creates a copy of the source packet
// make a copy of the packet and then this copy has its own ack fnc
// no need to re-insert into the stack
static void _MAC_Bridge_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, TCPIP_MAC_PACKET* pBridgePkt, uint16_t pktLen, MAC_BRIDGE_HASH_ENTRY* hEntry)
{
    // copy the payload
    uint8_t* srcStartAdd = pSrcPkt->pNetLayer;
    uint16_t copiedLen = TCPIP_Helper_PacketCopy(pSrcPkt, pBridgePkt->pNetLayer, &srcStartAdd, pktLen, false);

    if(copiedLen != pktLen)
    {   // should NOT happen!
        _Mac_Bridge_AssertCond(false, __func__, __LINE__);
    }
    // copy the Ethernet header
    memcpy(pBridgePkt->pMacLayer, pSrcPkt->pMacLayer, sizeof(TCPIP_MAC_ETHERNET_HEADER));

    pBridgePkt->pDSeg->segLen = copiedLen;
}

// acknowledges a bridge packet once it's been transmitted
static bool _MAC_Bridge_PacketAck(TCPIP_MAC_PACKET* pkt,  const void* param)
{
    _MAC_Bridge_StatUpdate(gBridgeDcpt, MAC_BRIDGE_STAT_TYPE_ACK_PKTS, 1);

    // keep forwarding to the other interfaces
    _MAC_Bridge_ForwardPacket(pkt, (MAC_BRIDGE_HASH_ENTRY*)param);
    return false;
}

// returns the number of the ports that are part of the bridge and their port map
// -1 if failure
static int  _MAC_Bridge_SetPorts(MAC_BRIDGE_DCPT* pBDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pConfig)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_NET_IF*  pNetIf;
    size_t ix;
    uint8_t nPorts = 0;
    const TCPIP_MAC_BRIDGE_ENTRY_ASCII* pAscEntry = &pConfig->bridgeTable->entryAscii;
    const TCPIP_MAC_BRIDGE_ENTRY_BIN* pBinEntry = &pConfig->bridgeTable->entryBin;

    uint8_t portInUse[TCPIP_MAC_BRIDGE_MAX_PORTS_NO] = {0};

    for(ix = 0; ix < pConfig->bridgeTableSize; ix++, pAscEntry++, pBinEntry++)
    {
        if((pConfig->bridgeFlags & TCPIP_MAC_BRIDGE_FLAG_ASCII_TABLE) != 0) 
        {
            if(pAscEntry->ifName == 0)
            {
                return -1;
            }

            netH = TCPIP_STACK_NetHandleGet(pAscEntry->ifName);
        }
        else
        {
            netH = TCPIP_STACK_IndexToNet((int)pBinEntry->ifIx);
        }

        if(netH == 0)
        {
            return -1;
        }

        pNetIf = _TCPIPStackHandleToNet(netH);

        int ifIx = TCPIP_STACK_NetIxGet(pNetIf);
        if(ifIx < 0)
        {
            return -1;
        }

        if(portInUse[nPorts] == 0)
        {
            pBDcpt->port2IfIx[nPorts] = (uint8_t)ifIx;
            portInUse[nPorts] = 1;
            _TCPIPStack_BridgeSetIf(pNetIf, nPorts);
            nPorts++;
        }
    }

    return nPorts;
}

static TCPIP_MAC_BRIDGE_RESULT _MAC_Bridge_SetPermEntries(MAC_BRIDGE_DCPT* pBDcpt, const TCPIP_MAC_BRIDGE_CONFIG* pBConfig)
{
    const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPerm;
    MAC_BRIDGE_HASH_ENTRY* hE;
    size_t ix;

    if(pBConfig->bridgePermTableSize == 0 || pBConfig->bridgePermTable == 0)
    {   // nothing to do
        return TCPIP_MAC_BRIDGE_RES_OK;
    }

    // start adding static entries
    // 
    TCPIP_MAC_BRIDGE_RESULT res = TCPIP_MAC_BRIDGE_RES_OK;
    for(ix = 0, pPerm = pBConfig->bridgePermTable; ix < pBConfig->bridgePermTableSize; ix++, pPerm++)
    {
        hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(pBDcpt->hashDcpt, pPerm->destAdd.v);
        if(hE == 0)
        {   // out of entries!
            return TCPIP_MAC_BRIDGE_RES_FDB_FULL;
        }

        // set the permanent entry!
        _MAC_Bridge_SetHashStaticEntry(hE, 0, true);

        res = _MAC_Bridge_SetFDBFromPerm(pBDcpt, hE,  pPerm);
        if(res != TCPIP_MAC_BRIDGE_RES_OK)
        {
            break;
        }
    }

    return res;
}


static TCPIP_MAC_BRIDGE_RESULT _MAC_Bridge_SetFDBFromPerm(MAC_BRIDGE_DCPT* pBDcpt, MAC_BRIDGE_HASH_ENTRY* hE,  const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPerm)
{
    int jx, kx;
    TCPIP_NET_IF    *pInIf, *pOutIf;
    const TCPIP_MAC_BRIDGE_CONTROL_ENTRY* pCEntry;
    const TCPIP_MAC_BRIDGE_CONTROL_DCPT* pCtrl;
    uint8_t*  pOutMap;
    uint8_t bridgePort;


    pCEntry = pPerm->pControlEntry;
    for(jx = 0; jx < pPerm->controlEntries; jx++, pCEntry++)
    {
        pInIf = _TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(pCEntry->inIx));
        if(pInIf == 0 || !_TCPIPStack_BridgeCheckIf(pInIf))
        {   // input interface not part of the bridge!
            return TCPIP_MAC_BRIDGE_RES_IF_NOT_BRIDGED;
        }

        if(pCEntry->dcptMapEntries != 0)
        {   // there is static info we need to set
            bridgePort = _TCPIPStack_BridgeGetIfPort(pInIf);
            pOutMap = hE->outPortMap[bridgePort];

            pCtrl = pCEntry->pDcptMap;
            for(kx = 0; kx < pCEntry->dcptMapEntries; kx++, pCtrl++)
            {
                pOutIf = _TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(pCtrl->outIx));
                if(pOutIf == 0 || !_TCPIPStack_BridgeCheckIf(pOutIf))
                {   // output interface not part of the bridge!
                    return TCPIP_MAC_BRIDGE_RES_IF_NOT_BRIDGED;
                }
                pOutMap[pCtrl->outIx] = pCtrl->portControl;
            }
        }
    }

    return TCPIP_MAC_BRIDGE_RES_OK;
} 


// populates the FDB with the host internal entries
// these configure the host packets behavior
static TCPIP_MAC_BRIDGE_RESULT _MAC_Bridge_SetHostEntries(MAC_BRIDGE_DCPT* pDcpt)
{
    MAC_BRIDGE_HASH_ENTRY* hE;
    TCPIP_NET_IF*   pNetIf;
    const TCPIP_MAC_ADDR* pMacAddress;
    int ifIx;
    int portIx;
    bool clearOutMap;

    for(portIx = 0; portIx < pDcpt->nPorts; portIx++) 
    {
        ifIx = pDcpt->port2IfIx[portIx];
        pNetIf = _TCPIPStackHandleToNet(TCPIP_STACK_IndexToNet(ifIx));

        _Mac_Bridge_AssertCond(pNetIf != 0, __func__, __LINE__);
        _Mac_Bridge_AssertCond(_TCPIPStack_BridgeCheckIf(pNetIf) == true, __func__, __LINE__);
        _Mac_Bridge_AssertCond(_TCPIPStack_BridgeGetIfPort(pNetIf) == portIx, __func__, __LINE__);


        pMacAddress = (const TCPIP_MAC_ADDR*)_TCPIPStackNetMACAddress(pNetIf);

        if(TCPIP_Helper_IsMcastMACAddress(pMacAddress))
        {
            return TCPIP_MAC_BRIDGE_RES_IF_ADDRESS_ERROR; 
        }

        // add this interface as a static entry
        hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(pDcpt->hashDcpt, _TCPIPStackNetMACAddress(pNetIf));
        if(hE == 0)
        {   // out of entries!
            return TCPIP_MAC_BRIDGE_RES_FDB_FULL;
        }

        if(hE->hEntry.flags.newEntry == 0)
        {   // overriden by initialization 
            _Mac_Bridge_AssertCond((hE->hEntry.flags.value & MAC_BRIDGE_HFLAG_STATIC) != 0, __func__, __LINE__);
            clearOutMap = false;
        }
        else
        {   // just added
            clearOutMap = true;
        }

        _MAC_Bridge_SetHashStaticEntry(hE, MAC_BRIDGE_HFLAG_HOST, clearOutMap);
    }

    return TCPIP_MAC_BRIDGE_RES_OK;
}

// helper to check if an iterface going up
// is part of the bridge
// returns the bridge port number if true, < 0 if false
static int _MAC_Bridge_IfBridged(MAC_BRIDGE_DCPT* pDcpt, int netIx)
{
    int ix;
    uint8_t* pIf = pDcpt->port2IfIx;
    for(ix = 0; ix < pDcpt->nPorts; ix++, pIf++)
    {
        if((int)*pIf == netIx)
        {
            return ix;
        }
    }

    return -1;
}

// simplistic implementation of count leading zeros in a 32 bit value
// It returns the number of contiguous leading zeroes
#if !defined (__PIC32MX__) && !defined(__PIC32MZ__)
static unsigned int _MAC_BridgeLeadingZeroes(uint32_t value)
{
    int ix;
    uint32_t mask;
    uint16_t nZeroes;

    nZeroes = 0;
    mask = 1 << 31;  // start from MSB

    for(ix = 0; ix < 32; ix++)
    {
        if((value & mask) != 0)
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



size_t _MAC_BridgeHashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, MAC_BRIDGE_HASH_KEY_SIZE) % (pOH->hEntries);
}

// Deletes an entry to make room in the hash table.
// This should be taken care of the MAC_BRIDGE_Task()
// However, there are static entries and the purgeTmo could be fairly long
// So it's possible to be unable to make room in the cache
OA_HASH_ENTRY* _MAC_BridgeHashEntryDelete(OA_HASH_DCPT* pOH)
{
    _Mac_Bridge_AssertCond(pOH == gBridgeDcpt->hashDcpt, __func__, __LINE__);

    int ix;
    MAC_BRIDGE_HASH_ENTRY* hE;
    int nEntries = gBridgeDcpt->hashDcpt->hEntries;

    uint32_t currTime = SYS_TMR_TickCountGet();
    for(ix = 0; ix < nEntries; ix++)
    {
        hE = (MAC_BRIDGE_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(gBridgeDcpt->hashDcpt, ix);
        if(hE->hEntry.flags.busy != 0 && (hE->hEntry.flags.value & MAC_BRIDGE_HFLAG_STATIC ) == 0)
        {   // in use dynamic entry
            if((int32_t)(currTime - hE->tExpire) > 0)
            {   // expired entry; remove
                return &hE->hEntry;
            }
        }
    }
    
    return 0;
}


int _MAC_BridgeHashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    return memcmp(((MAC_BRIDGE_HASH_ENTRY*)hEntry)->destAdd.v, key, MAC_BRIDGE_HASH_KEY_SIZE);
}

void _MAC_BridgeHashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    memcpy(((MAC_BRIDGE_HASH_ENTRY*)dstEntry)->destAdd.v, key, MAC_BRIDGE_HASH_KEY_SIZE);
}

#if (TCPIP_MAC_BRIDGE_STATISTICS != 0)
static void _MAC_Bridge_StatUpdate(MAC_BRIDGE_DCPT* bDcpt, MAC_BRIDGE_STAT_TYPE statType, uint32_t incUpdate)
{
    bDcpt->stat_array[statType] += incUpdate;
}

static void _MAC_Bridge_StatPortUpdate(MAC_BRIDGE_DCPT* bDcpt, int port, MAC_BRIDGE_STAT_TYPE statType, uint32_t incUpdate)
{
    TCPIP_MAC_BRIDGE_PORT_STAT* pPortStat = bDcpt->stat.portStat + port;
    uint32_t* pStat = &pPortStat->rxPackets + statType;
    *pStat += incUpdate;
}

bool TCPIP_MAC_Bridge_StatisticsGet(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_STAT* pStat, bool clear)
{
    MAC_BRIDGE_DCPT* bDcpt = _MAC_Bridge_ValidateHandle(brHandle);

    if(bDcpt == 0)
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
        memset(bDcpt->stat_array, 0, sizeof(bDcpt->stat_array));
        bDcpt->stat.pktPoolLowSize = pktPoolSize;
        bDcpt->stat.dcptPoolLowSize = dcptPoolSize;
    }

    return true;
}


#endif  // (TCPIP_MAC_BRIDGE_STATISTICS != 0)

#endif  //if defined(TCPIP_STACK_USE_MAC_BRIDGE)


