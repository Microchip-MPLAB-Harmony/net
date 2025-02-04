/*******************************************************************************
  TCPIP network packet manager implementation

  Summary:
    
  Description:
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


#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MANAGER

#include "tcpip_private.h"
#include "tcpip_packet.h"
#include "tcpip/tcpip_mac.h"

#define TCPIP_SEGMENT_CACHE_ALIGN_SIZE (CACHE_LINE_SIZE)

// Segment payload gap:
// sizeof the TCPIP_MAC_SEGMENT_PAYLOAD::segmentDataGap
#if defined(TCPIP_IF_PIC32WK) || defined(TCPIP_IF_PIC32MZW1)
    #define TCPIP_MAC_DATA_SEGMENT_GAP      34U   
#else
    #define TCPIP_MAC_DATA_SEGMENT_GAP      4U 
#endif

// MAC driver data offset required by the TCP/IP stack
#define TCPIP_MAC_PAYLOAD_OFFSET            2U 

// the TCPIP_MAC_DATA_SEGMENT.segLoadOffset value
// Allocation test
// Note: enabling this in a real app will lead to run time exceptions!
// The segment data gap cannot change at run time!
// Enable for testing purposes only!
#define TCPIP_MAC_SEGMENT_GAP_TEST      0

// Segment payload gap:
// sizeof the TCPIP_MAC_SEGMENT_PAYLOAD::segmentDataGap
#if defined(TCPIP_IF_PIC32WK) || defined(TCPIP_IF_PIC32MZW1)
    #define TCPIP_MAC_DATA_SEGMENT_GAP      34U   
#else
    #define TCPIP_MAC_DATA_SEGMENT_GAP      4U   
#endif

// should be uintptr_t aligned, for storing the segmentPktPtr
#define TCPIP_MAC_DATA_SEGMENT_GAP_SIZE  ((((sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT) + TCPIP_MAC_DATA_SEGMENT_GAP) + sizeof(uintptr_t) - 1U) / sizeof(uintptr_t)) * sizeof(uintptr_t))

// for TX/RX we place the segment gap in front of the packet:
#define TCPIP_MAC_GAP_OFFSET    (-(int16_t)TCPIP_MAC_DATA_SEGMENT_GAP_SIZE)




static TCPIP_STACK_HEAP_HANDLE    pktMemH = NULL;

static void F_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId);

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static TCPIP_PKT_TRACE_ENTRY    pktTraceTbl[TCPIP_PKT_TRACE_SIZE];


static TCPIP_PKT_TRACE_INFO pktTraceInfo;  // global counters  


static TCPIP_PKT_TRACE_ENTRY*   F_TCPIP_PKT_TraceFindEntry(TCPIP_STACK_MODULE moduleId, bool addNewSlot);
static void                     F_TCPIP_PKT_TraceAddToEntry(TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PACKET* pPkt);
static void                     F_TCPIP_PKT_TraceFreeEntry(TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PACKET* pPkt);
static void                     F_TCPIP_PKT_TraceAckEntry(TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
static uint32_t                 F_TCPIP_PKT_TracePktSize(TCPIP_MAC_PACKET* pPkt);


static /*__inline__*/ void /*__attribute__((always_inline))*/ F_TCPIP_PKT_TraceFail(void)
{
    pktTraceInfo.traceFails++;
}

static TCPIP_MAC_PACKET* F_TCPIP_PKT_PacketAllocInt(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId);
static void F_TCPIP_PKT_PacketFreeInt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId);
static TCPIP_MAC_PACKET*  F_TCPIP_PKT_SocketAllocInt(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId);
static TCPIP_MAC_DATA_SEGMENT* F_TCPIP_PKT_SegmentAllocInt(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags, TCPIP_STACK_MODULE moduleId);
static void F_TCPIP_PKT_SegmentFreeInt(TCPIP_MAC_DATA_SEGMENT* pSeg, TCPIP_STACK_MODULE moduleId);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if (TCPIP_PACKET_LOG_ENABLE)

// size of the packet log
// The log entries will be overwritten when no available free slot is found!

static TCPIP_PKT_LOG_ENTRY  pktLogTbl[TCPIP_PKT_LOG_SIZE];

static TCPIP_PKT_LOG_INFO   pktLogInfo;  // global log counters  

static int32_t              pktOverwriteIx;    // simple LRU displacement pointer

static void                 F_TCPIP_PKT_LogInit(bool resetAll);

#endif  // (TCPIP_PACKET_LOG_ENABLE)


// API

bool TCPIP_PKT_Initialize(TCPIP_STACK_HEAP_HANDLE heapH, const TCPIP_NETWORK_CONFIG* pNetConf, size_t nNets)
{
    pktMemH = NULL;

    while(heapH != NULL)
    {
        TCPIP_MAC_PACKET* allocPtr;

        allocPtr = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(heapH, sizeof(TCPIP_MAC_PACKET));

        if(allocPtr == NULL)
        {
            break;
        }

        (void) TCPIP_HEAP_Free(heapH, allocPtr);
        // success
        pktMemH = heapH;

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
        (void) memset(pktTraceTbl, 0, sizeof(pktTraceTbl));
        (void) memset(&pktTraceInfo, 0, sizeof(pktTraceInfo));
        pktTraceInfo.nEntries = sizeof(pktTraceTbl)/sizeof(*pktTraceTbl);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if (TCPIP_PACKET_LOG_ENABLE)
        F_TCPIP_PKT_LogInit(true);
        // construct the start netLogMask
        size_t ix;
        for(ix = 0; ix < nNets; ix++)
        {
            if(((uint32_t)pNetConf->startFlags & (uint32_t)TCPIP_NETWORK_CONFIG_PKT_LOG_ON) != 0U) 
            {
                pktLogInfo.netLogMask |= 1UL << ix;
            }
            pNetConf++;
        }

#endif  // (TCPIP_PACKET_LOG_ENABLE)

        break;
    }


    return pktMemH != NULL;
    
}

void TCPIP_PKT_Deinitialize(void)
{
    pktMemH = NULL;
}

// acknowledges a packet
static void F_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId)
{
    if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
    {
        pPkt->ackRes = (int8_t)ackRes;
    }

    if(pPkt->ackFunc != NULL)
    {
       TCPIP_PKT_FlightLogAcknowledge(pPkt, moduleId, ackRes);
       (*pPkt->ackFunc)(pPkt, pPkt->ackParam);
    }
    else
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Packet Ack: orphan packet! \r\n");
    }
}

void TCPIP_PKT_SegmentAppend(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_DATA_SEGMENT* pSeg)
{
    TCPIP_MAC_DATA_SEGMENT  *pN, *prev;

    pN = pPkt->pDSeg;

    if(pN == NULL)
    {   // insert as root
        pPkt->pDSeg = pSeg;
    }
    else
    {   // traverse the list
        prev = NULL; 
        while(pN != NULL)            
        {
            prev = pN;
            pN = pN->next;
        }
        prev->next = pSeg;
    }

}

// helpers

bool TCPIP_PKT_PacketMACFormat(TCPIP_MAC_PACKET* pPkt, const TCPIP_MAC_ADDR* dstAddr, const TCPIP_MAC_ADDR* srcAddr, uint16_t pktType)
{
    if(srcAddr != NULL)
    {
        TCPIP_MAC_ETHERNET_HEADER* macHdr;
        TCPIP_MAC_ADDR    *destHdrAdd, *srcHdrAdd;

        macHdr = FC_Uptr2MacEthHdr(pPkt->pMacLayer);
        srcHdrAdd = &macHdr->SourceMACAddr;

        if(dstAddr != NULL)
        {
            destHdrAdd = &macHdr->DestMACAddr;
            (void) memcpy(destHdrAdd, dstAddr, sizeof(*destHdrAdd));
        }

        (void) memcpy(srcHdrAdd, srcAddr, sizeof(*srcHdrAdd));
        // set the MAC frame type
        macHdr->Type = TCPIP_Helper_htons(pktType);

        // update the frame length
        pPkt->pDSeg->segLen += (uint16_t)sizeof(TCPIP_MAC_ETHERNET_HEADER);
        return true;
    }

    return false;
}

// returns the segment to which dataAddress belongs
// 0 if not in this packet
TCPIP_MAC_DATA_SEGMENT* TCPIP_PKT_DataSegmentGet(TCPIP_MAC_PACKET* pPkt, const uint8_t* dataAddress, bool srchTransport)
{
    TCPIP_MAC_DATA_SEGMENT  *pStartSeg, *pSeg;

    pStartSeg = NULL;

    if(srchTransport)
    {   // search the segment containing the transport data
        for(pSeg = pPkt->pDSeg; pSeg != NULL; pSeg = pSeg->next)
        {
            if((pSeg->segLoad <= pPkt->pTransportLayer) && (pPkt->pTransportLayer <= (pSeg->segLoad + pSeg->segSize)))
            {   // found segment containing the beg of the transport
                if((pPkt->pTransportLayer <= dataAddress) && (dataAddress <= (pSeg->segLoad + pSeg->segSize)))
                {
                    return pSeg;
                }

                pStartSeg = pSeg->next;
                break;
            }
        }
    }
    else
    {
        pStartSeg = pPkt->pDSeg;
    }


    for(pSeg = pStartSeg; pSeg != NULL; pSeg = pSeg->next)
    {
        if((pSeg->segLoad <= dataAddress) && (dataAddress <= (pSeg->segLoad + pSeg->segSize)))
        {
            return pSeg;
        }
    }

    return NULL;
}

uint16_t TCPIP_PKT_PayloadLen(TCPIP_MAC_PACKET* pPkt)
{
    uint32_t payloadSize = 0;

    if(pPkt != NULL)
    {
        TCPIP_MAC_DATA_SEGMENT* pSeg = pPkt->pDSeg;

        while(pSeg != NULL)
        {
            payloadSize += pSeg->segLen;
            pSeg = pSeg->next;
        }
    }

    return (uint16_t)payloadSize;
}

int16_t TCPIP_PKT_GapDcptOffset(void)
{
    return (int16_t)TCPIP_MAC_GAP_OFFSET; 
}

uint16_t TCPIP_PKT_GapDcptSize(void)
{
    return TCPIP_MAC_DATA_SEGMENT_GAP_SIZE; 
}


// repeated debug versions; they store the original moduleId
#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) F_TCPIP_PKT_PacketAllocInt(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint16_t        pktUpLen, allocLen, segAlignSize, segAllocSize;

    if(pktLen < (uint16_t)sizeof(TCPIP_MAC_PACKET))
    {
        pktLen = (uint16_t)sizeof(TCPIP_MAC_PACKET);
    }

    pktUpLen = (((pktLen + 3U) >> 2U) << 2U);     // 32 bits round up
    // segment size, multiple of cache line size
    segAlignSize = (uint16_t)(((segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER) + TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1U) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = (uint16_t)(segAlignSize + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_MAC_PAYLOAD_OFFSET + TCPIP_SEGMENT_CACHE_ALIGN_SIZE); 
    // total allocation size
    allocLen = pktUpLen + (uint16_t)sizeof(*pSeg) + segAllocSize;

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_MallocDebug(pktMemH, allocLen, (int)moduleId, __LINE__);
#else
    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(pktMemH, allocLen);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

    if(pPkt != NULL)
    {   
        // clear the TCPIP_MAC_PACKET and 1st segment fields
        // populate the 1st segment
        (void) memset(pPkt, 0, pktUpLen + sizeof(*pSeg));
        pSeg = FC_Uptr2MacDataSeg(((uint8_t*)pPkt + pktUpLen));

        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1) + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1U) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        // set the pointer to the packet that segment belongs to
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_Uptr2MacGapDcpt(pSeg->segBuffer + TCPIP_MAC_GAP_OFFSET);
        pGap->segmentPktPtr = pPkt;

        pSeg->segFlags = (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC; // embedded in TCPIP_MAC_PACKET itself
        pPkt->pDSeg = pSeg;

        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
        pPkt->pMacLayer = pSeg->segLoad;
        pPkt->pktFlags = (uint32_t)flags & (~(uint32_t)TCPIP_MAC_PKT_FLAG_STATIC);  // this packet is dynamically allocated
        if(segLoadLen != 0U)
        {
            pPkt->pNetLayer = pPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        }

    }

    return pPkt;
}

static __inline__ void __attribute__((always_inline)) F_TCPIP_PKT_PacketFreeInt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    if((pPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_STATIC) == 0U)
    {   // we don't deallocate static packets
        TCPIP_MAC_DATA_SEGMENT  *pSeg, *pNSeg;

        pSeg = pPkt->pDSeg;
        while(pSeg != NULL)
        {
            pNSeg = pSeg->next;
            if((pSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC) == 0U)
            {
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
                (void)TCPIP_HEAP_FreeDebug(pktMemH, pSeg, (int)moduleId, __LINE__);
#else
                (void) TCPIP_HEAP_Free(pktMemH, pSeg);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
            }
            pSeg = pNSeg;
        }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        (void)TCPIP_HEAP_FreeDebug(pktMemH, pPkt, (int)moduleId, __LINE__);
#else
        (void) TCPIP_HEAP_Free(pktMemH, pPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    }
}

static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) F_TCPIP_PKT_SocketAllocInt(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId)
{
    uint16_t    netHdrLen, totHdrLen;
    TCPIP_MAC_PACKET* pPkt;

    if(((uint32_t)flags & TCPIP_MAC_PKT_FLAG_IPV6) != 0U)
    {
        netHdrLen = (uint16_t)sizeof(IPV6_HEADER);
    }
    else
    {
        netHdrLen = (uint16_t)sizeof(IPV4_HEADER);
    }


    totHdrLen = netHdrLen + transpHdrLen;

    pPkt = F_TCPIP_PKT_PacketAllocInt(pktLen, totHdrLen +  payloadLen, flags, moduleId);

    if(pPkt != NULL)
    {   // set the layer pointers in place
        if(transpHdrLen != 0U)
        {
            pPkt->pTransportLayer = pPkt->pNetLayer + netHdrLen;
        }
    }

    return pPkt;
}

// the segment size is allocated following the rules:
//  - payload size is multiple of cache line size
//  - load starts at a cache aligned address
static __inline__ TCPIP_MAC_DATA_SEGMENT* __attribute__((always_inline)) F_TCPIP_PKT_SegmentAllocInt(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t allocLen, segAlignSize, segAllocSize;

    segAlignSize = ((loadLen + TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1U) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE;
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = segAlignSize + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_SEGMENT_CACHE_ALIGN_SIZE; 

    // total allocation size
    allocLen = (uint16_t)sizeof(*pSeg) + segAllocSize;


#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_MallocDebug(pktMemH, allocLen, (int)moduleId, __LINE__);
#else
    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_Malloc(pktMemH, allocLen);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


    if(pSeg != NULL)
    {
        (void) memset(pSeg, 0, sizeof(*pSeg));

        pSeg->segFlags = (uint16_t)flags & (~(uint16_t)TCPIP_MAC_SEG_FLAG_STATIC);
        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1) + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1U) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
    }

    return pSeg;
}

static __inline__ void __attribute__((always_inline)) F_TCPIP_PKT_SegmentFreeInt(TCPIP_MAC_DATA_SEGMENT* pSeg, TCPIP_STACK_MODULE moduleId)
{
    if( (pSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC) == 0U)
    {
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        (void)TCPIP_HEAP_FreeDebug(pktMemH, pSeg, (int)moduleId, __LINE__);
#else
        (void) TCPIP_HEAP_Free(pktMemH, pSeg);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    }

}

TCPIP_MAC_PACKET* F_TCPIP_PKT_SocketAllocDebug(uint16_t pktLen, uint16_t tHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_MAC_PACKET* pPkt = F_TCPIP_PKT_SocketAllocInt(pktLen, tHdrLen, payloadLen, flags, moduleId);
    F_TCPIP_PKT_TraceAddToEntry(moduleId, pPkt);
    return pPkt;

}

TCPIP_MAC_PACKET* F_TCPIP_PKT_PacketAllocDebug(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_MAC_PACKET* pPkt = F_TCPIP_PKT_PacketAllocInt(pktLen, segLoadLen, flags, moduleId);
    F_TCPIP_PKT_TraceAddToEntry(moduleId, pPkt);
    return pPkt;

}


void F_TCPIP_PKT_PacketFreeDebug(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    F_TCPIP_PKT_TraceFreeEntry(moduleId, pPkt);
    F_TCPIP_PKT_PacketFreeInt(pPkt, moduleId);
}

TCPIP_MAC_DATA_SEGMENT* F_TCPIP_PKT_SegmentAllocDebug(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags, TCPIP_STACK_MODULE moduleId)
{
    return F_TCPIP_PKT_SegmentAllocInt(loadLen, flags, moduleId); 
}

void F_TCPIP_PKT_SegmentFreeDebug(TCPIP_MAC_DATA_SEGMENT* pSeg, TCPIP_STACK_MODULE moduleId)
{
    F_TCPIP_PKT_SegmentFreeInt(pSeg, moduleId);
}


void F_TCPIP_PKT_PacketAcknowledgeDebug(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId)
{
    F_PacketAcknowledge(pPkt, ackRes, moduleId);
    F_TCPIP_PKT_TraceAckEntry(moduleId, pPkt, ackRes);
}

size_t TCPIP_PKT_TraceGetEntriesNo(TCPIP_PKT_TRACE_INFO* pTraceInfo)
{
    TCPIP_PKT_TRACE_ENTRY *pEntry;
    size_t ix;
    size_t nUsed = 0;

    pEntry = pktTraceTbl;
    for(ix = 0; ix < sizeof(pktTraceTbl) / sizeof(*pktTraceTbl); ix++)
    {
        if((int)pEntry->moduleId > 0)
        {
            nUsed++;
        }
        pEntry++;
    }

    pktTraceInfo.nUsed = nUsed;
    if(pTraceInfo != NULL)
    {
        *pTraceInfo = pktTraceInfo;
    }


    return nUsed;
}


// populates a trace entry with data for a index
bool TCPIP_PKT_TraceGetEntry(size_t entryIx, TCPIP_PKT_TRACE_ENTRY* tEntry)
{
    TCPIP_PKT_TRACE_ENTRY *pEntry;

    if(entryIx < sizeof(pktTraceTbl) / sizeof(*pktTraceTbl))
    {   // valid index
        pEntry = pktTraceTbl + entryIx;
        if((int)pEntry->moduleId > 0)
        {
            *tEntry = *pEntry;
            return true;
        }
    }

    return false;
}

static TCPIP_PKT_TRACE_ENTRY* F_TCPIP_PKT_TraceFindEntry(TCPIP_STACK_MODULE moduleId, bool addNewSlot)
{
    size_t ix;
    TCPIP_PKT_TRACE_ENTRY    *freeEntry,*pEntry;

    freeEntry = NULL;
    pEntry = pktTraceTbl;
    for(ix = 0; ix < sizeof(pktTraceTbl) / sizeof(*pktTraceTbl); ix++)
    {
        if(pEntry->moduleId == moduleId)
        {
            return pEntry;
        }
        else if(addNewSlot && freeEntry == NULL && (int)pEntry->moduleId == 0)
        {
            freeEntry = pEntry;
        }
        else
        {
            // do nothing
        }
        pEntry++;
    }

    if(freeEntry != NULL)
    {
        (void) memset(freeEntry, 0x0, sizeof(*freeEntry));
        freeEntry->moduleId = moduleId;
    }

    return freeEntry;
}

static uint32_t F_TCPIP_PKT_TracePktSize(TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg = pPkt->pDSeg;
    ptrdiff_t segDiff = FC_CU8PtrDiff((uint8_t*)pSeg, (uint8_t*)pPkt);
    uint32_t pktSize = (uint32_t)segDiff + sizeof(*pSeg) + (uint32_t)pSeg->segAllocSize;

    while((pSeg = pSeg->next) != NULL)
    {
        if((pSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC) == 0U)
        {
            pktSize += sizeof(*pSeg) + pSeg->segAllocSize;
        }
    }

    return pktSize;

}
    
static void F_TCPIP_PKT_TraceAddToEntry(TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = F_TCPIP_PKT_TraceFindEntry(moduleId, true);

    if(pEntry != NULL)
    {
        if(pPkt != NULL)
        {
            pEntry->totAllocated++;
            pEntry->currAllocated++;
            pEntry->currSize += F_TCPIP_PKT_TracePktSize(pPkt);
        }
        else
        {
            pEntry->totFailed++;
        }
    }
    else
    {
        F_TCPIP_PKT_TraceFail();
    }

}



static void F_TCPIP_PKT_TraceFreeEntry(TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = F_TCPIP_PKT_TraceFindEntry(moduleId, false);

    if(pEntry != NULL)
    {
        pEntry->currAllocated--;
        pEntry->currSize -= F_TCPIP_PKT_TracePktSize(pPkt);
    }
    else
    {
        F_TCPIP_PKT_TraceFail();
    }

}

static void F_TCPIP_PKT_TraceAckEntry(TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = F_TCPIP_PKT_TraceFindEntry(moduleId, false);

    if(pEntry != NULL)
    {
        pEntry->nAcks++;
        if((int)ackRes < 0)
        {
            pktTraceInfo.traceAckErrors++;
        }
    }
    else
    {
        pktTraceInfo.traceAckOwnerFails++;
    }

}

#else
// regular allocation functions, non-debug
// they don't take a module Id parameter; otherwise exactly the same functions
TCPIP_MAC_PACKET* F_TCPIP_PKT_PacketAlloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint16_t        pktUpLen, allocLen, segAlignSize, segAllocSize;

    if(pktLen < (uint16_t)sizeof(TCPIP_MAC_PACKET))
    {
        pktLen = (uint16_t)sizeof(TCPIP_MAC_PACKET);
    }

    pktUpLen = (((pktLen + 3U) >> 2U) << 2U);     // 32 bits round up
    // segment size, multiple of cache line size
    segAlignSize = (uint16_t)(((segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER) + TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1U) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = (uint16_t)(segAlignSize + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_MAC_PAYLOAD_OFFSET + TCPIP_SEGMENT_CACHE_ALIGN_SIZE); 
    // total allocation size
    allocLen = pktUpLen + (uint16_t)sizeof(*pSeg) + segAllocSize;

    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(pktMemH, allocLen);

    if(pPkt != NULL)
    {   
        // clear the TCPIP_MAC_PACKET and 1st segment fields
        // populate the 1st segment
        (void) memset(pPkt, 0, pktUpLen + sizeof(*pSeg));
        pSeg = FC_Uptr2MacDataSeg(((uint8_t*)pPkt + pktUpLen));

        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1) + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1U) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        // set the pointer to the packet that segment belongs to
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_Uptr2MacGapDcpt(pSeg->segBuffer + TCPIP_MAC_GAP_OFFSET);
        pGap->segmentPktPtr = pPkt;

        pSeg->segFlags = (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC; // embedded in TCPIP_MAC_PACKET itself
        pPkt->pDSeg = pSeg;

        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
        pPkt->pMacLayer = pSeg->segLoad;
        pPkt->pktFlags = (uint32_t)flags & (~(uint32_t)TCPIP_MAC_PKT_FLAG_STATIC);  // this packet is dynamically allocated
        if(segLoadLen != 0U)
        {
            pPkt->pNetLayer = pPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        }
        pPkt->pktPriority = 0; // set the default priority

    }

    return pPkt;
}

// allocates a socket packet
TCPIP_MAC_PACKET*  F_TCPIP_PKT_SocketAlloc(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    uint16_t    netHdrLen, totHdrLen;
    TCPIP_MAC_PACKET* pPkt;

    if(((uint32_t)flags & TCPIP_MAC_PKT_FLAG_IPV6) != 0U)
    {
        netHdrLen = (uint16_t)sizeof(IPV6_HEADER);
    }
    else
    {
        netHdrLen = (uint16_t)sizeof(IPV4_HEADER);
    }

    totHdrLen = netHdrLen + transpHdrLen;

    pPkt = F_TCPIP_PKT_PacketAlloc(pktLen, totHdrLen +  payloadLen, flags );

    if(pPkt != NULL)
    {   // set the layer pointers in place
        if(transpHdrLen != 0U)
        {
            pPkt->pTransportLayer = pPkt->pNetLayer + netHdrLen;
        }
    }

    return pPkt;
}

// frees a previously allocated packet
void F_TCPIP_PKT_PacketFree(TCPIP_MAC_PACKET* pPkt)
{
    if((pPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_STATIC) == 0U)
    {   // we don't deallocate static packets
        TCPIP_MAC_DATA_SEGMENT  *pSeg, *pNSeg;

        pSeg = pPkt->pDSeg;
        while(pSeg != NULL)
        {
            pNSeg = pSeg->next;
            if((pSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC) == 0U)
            {
                (void) TCPIP_HEAP_Free(pktMemH, pSeg);
            }
            pSeg = pNSeg;
        }

        (void) TCPIP_HEAP_Free(pktMemH, pPkt);
    }
}

// the segment size is allocated following the rules:
//  - payload size is multiple of cache line size
//  - load starts at a cache aligned address
TCPIP_MAC_DATA_SEGMENT* F_TCPIP_PKT_SegmentAlloc(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t allocLen, segAlignSize, segAllocSize;

    segAlignSize = ((loadLen + (uint16_t)TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1U) / (uint16_t)TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * (uint16_t)TCPIP_SEGMENT_CACHE_ALIGN_SIZE;
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = segAlignSize + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_SEGMENT_CACHE_ALIGN_SIZE; 

    // total allocation size
    allocLen = (uint16_t)sizeof(*pSeg) + segAllocSize;

    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_Malloc(pktMemH, allocLen);

    if(pSeg != NULL)
    {
        (void) memset(pSeg, 0, sizeof(*pSeg));

        pSeg->segFlags = (uint16_t)flags & (~(uint16_t)TCPIP_MAC_SEG_FLAG_STATIC);
        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1U) + TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1U) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
    }

    return pSeg;
}

void F_TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg)
{
    if( (pSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC) == 0U)
    {
        (void) TCPIP_HEAP_Free(pktMemH, pSeg);
    }
}

void F_TCPIP_PKT_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId)
{
    F_PacketAcknowledge(pPkt, ackRes, (TCPIP_STACK_MODULE)moduleId);
}

#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)


#if (TCPIP_PACKET_LOG_ENABLE)


// discards an used log entry
static __inline__ void __attribute__((always_inline)) F_TCPIP_PKT_LogDiscardEntry(TCPIP_PKT_LOG_ENTRY* pEntry)
{
    if(pEntry->pPkt != NULL)
    {
        pktLogInfo.nUsed--;
        if((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) != 0U)
        {
            pktLogInfo.nPersistent--;
        }
    }
    pEntry->pPkt = NULL;

}

// initializes the log
// if resetAll is specified then all the info is cleared
// otherwise the current masks are retained
static void F_TCPIP_PKT_LogInit(bool resetAll)
{
    (void) memset(pktLogTbl, 0, sizeof(pktLogTbl));
    pktOverwriteIx = 0;

    if(resetAll)
    {
        (void) memset(&pktLogInfo, 0, sizeof(pktLogInfo));
        pktLogInfo.nEntries = (int16_t)sizeof(pktLogTbl) / (int16_t)sizeof(*pktLogTbl);
    }
    else
    {
        pktLogInfo.nUsed = 0;
        pktLogInfo.nPersistent = 0;
        pktLogInfo.nFailed = 0;
    }
}

// finds a log entry that matches the pPkt
// if itExists == true, then only an existing packet is searched for
static TCPIP_PKT_LOG_ENTRY* F_TCPIP_PKT_LogFindEntry(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, bool itExists)
{
    int32_t ix, lowOvrIx, hiOvrIx;
    TCPIP_PKT_LOG_ENTRY *freeEntry, *pEntry;
    TCPIP_PKT_LOG_ENTRY *pLowOvr, *pHiOvr;
    TCPIP_PKT_LOG_FLAGS logFlags;

    freeEntry = NULL;
    pLowOvr = NULL;
    pHiOvr = NULL;
    lowOvrIx = 0;
    hiOvrIx = 0;
    pEntry = pktLogTbl;
    for(ix = 0; ix < ((int32_t)sizeof(pktLogTbl) / (int32_t)sizeof(*pktLogTbl)); ix++)
    {
        if(pEntry->pPkt == NULL)
        {   // empty slot
            if(freeEntry == NULL)
            {
                freeEntry = pEntry;
            }
            continue;
        }

        if(((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U) && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0U))
        {   // can overwrite
            if(ix <= pktOverwriteIx)
            {
                pLowOvr = pEntry;
                lowOvrIx = ix;
            }
            else if((ix > pktOverwriteIx) && (pHiOvr == NULL))
            {
                pHiOvr = pEntry;
                hiOvrIx = ix;
            }
            else
            {
                /* Do Nothing */
            }
        }


        if((pEntry->pPkt == pPkt) && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) == 0U))
        {   // packet match. this is it
            pEntry->logFlags &= ~((uint16_t)TCPIP_PKT_LOG_FLAG_NEW | (uint16_t)TCPIP_PKT_LOG_FLAG_REPLACE);
            return pEntry;
        }
        pEntry++;

    }

    // check if we have a freeEntry or overwrite to setup
    if(itExists)
    {   
        return NULL;
    }

    if(freeEntry != NULL)
    {
        pEntry = freeEntry;
        logFlags = TCPIP_PKT_LOG_FLAG_NEW;
    }
    else if(pHiOvr != NULL)
    {
        pEntry = pHiOvr;
        logFlags = TCPIP_PKT_LOG_FLAG_REPLACE;
        pktOverwriteIx = hiOvrIx;
    }
    else if(pLowOvr != NULL)
    {
        pEntry = pLowOvr;
        logFlags = TCPIP_PKT_LOG_FLAG_REPLACE;
        pktOverwriteIx = lowOvrIx;
    }
    else
    {
        pEntry = NULL;
        logFlags = TCPIP_PKT_LOG_FLAG_NONE;
    }

    if(pEntry != NULL)
    {
        (void) memset(pEntry, 0x0, sizeof(*pEntry));
        pEntry->pPkt = pPkt;
        pEntry->logFlags = (uint16_t)logFlags;
    }
    // else we're out of slots...

    return pEntry;
}


static void F_TCPIP_PKT_LogCallHandler(TCPIP_PKT_LOG_ENTRY* pLogEntry, TCPIP_STACK_MODULE moduleId)
{
    // check if we need to report this log
    while(pktLogInfo.logHandler != NULL)
    {
        if(((uint32_t)pktLogInfo.logType & (uint32_t)TCPIP_PKT_LOG_TYPE_HANDLER_ALL) == 0U)
        {
            if(((uint32_t)pktLogInfo.logType & (uint32_t)TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0U)
            {
                if((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_RX) == 0U)
                {   // don't report
                    break;
                }
            }
            else if(((uint32_t)pktLogInfo.logType & (uint32_t)TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0U)
            {
                if((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_TX) == 0U)
                {   // don't report
                    break;
                }
            }
            else if(((uint32_t)pktLogInfo.logType & (uint32_t)TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0U)
            {
                if((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0U)
                {   // don't report
                    break;
                }
            }
            else
            {
                /* Do Nothing */
            }

            if((pLogEntry->moduleLog & pktLogInfo.logModuleMask) == 0U)
            {   // don't report
                break;
            }
        }

        // report it
        (*pktLogInfo.logHandler)(moduleId, pLogEntry);
        break;
    }
}

static TCPIP_PKT_LOG_ENTRY* F_TCPIP_PKT_FlightLog(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_PKT_LOG_FLAGS logFlags)
{
    TCPIP_NET_IF* pNetIf = FC_Cvptr2NetIf(pPkt->pktIf);
    int netIx = TCPIP_STACK_NetIxGet(pNetIf);
    if(netIx < 0)
    {   // invalid interface
        return NULL;
    }

    uint16_t netMask = (uint16_t)((uint32_t)1U << (uint32_t)netIx);

    if((pktLogInfo.netLogMask & netMask) == 0U)
    {   // not logging this
        return NULL;
    }

    TCPIP_PKT_LOG_ENTRY* pLogEntry = F_TCPIP_PKT_LogFindEntry(pPkt, moduleId, false);

    if(pLogEntry == NULL)
    {
        pktLogInfo.nFailed++;
        return NULL;
    }


    if((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_NEW) != 0U)
    {
        pktLogInfo.nUsed++;
    }
    // else probably a replacement

    if((pLogEntry->logFlags & ((uint16_t)TCPIP_PKT_LOG_FLAG_NEW | (uint16_t)TCPIP_PKT_LOG_FLAG_REPLACE)) != 0U)
    {
        pLogEntry->pktOwner = (uint16_t)moduleId;
        pLogEntry->netMask = netMask;
    }

    uint32_t tStamp = SYS_TIME_CounterGet();
    if(moduleId >= TCPIP_MODULE_MAC_START)
    {
        pLogEntry->macId = (uint16_t)moduleId;
        pLogEntry->macStamp = tStamp;
    }
    else
    {   // some module can log the same packet multiple times if routed internally
        if(moduleId > TCPIP_MODULE_LAYER3)
        {
            moduleId = TCPIP_MODULE_LAYER3;
        }

        pLogEntry->moduleLog |= (uint16_t)1U << (uint16_t)moduleId;
        pLogEntry->moduleStamp[(uint32_t)moduleId - 1U] = tStamp;
    }

    pLogEntry->logFlags |= (uint16_t)logFlags;


    return pLogEntry;


}

void TCPIP_PKT_FlightLogTx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = F_TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_TX);

    if(pLogEntry != NULL)
    {
        F_TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogRx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry =  F_TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_RX);

    if(pLogEntry != NULL)
    {
        F_TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogTxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo )
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = F_TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_TX);

    if(pLogEntry != NULL)
    {
        pLogEntry->logFlags |= (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM;
        pLogEntry->sktNo =  sktNo;
        pLogEntry->lclPort =  (uint16_t)(lclRemPort >> 16);
        pLogEntry->remPort =  (uint16_t)(lclRemPort);
        F_TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogRxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo )
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = F_TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_RX);

    if(pLogEntry != NULL)
    {
        pLogEntry->logFlags |= (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM;
        pLogEntry->sktNo =  sktNo;
        pLogEntry->lclPort =  (uint16_t)(lclRemPort >> 16);
        pLogEntry->remPort =  (uint16_t)(lclRemPort);
        F_TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}


void TCPIP_PKT_FlightLogAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PKT_ACK_RES ackRes)
{

    TCPIP_PKT_LOG_ENTRY* pLogEntry = F_TCPIP_PKT_LogFindEntry(pPkt, moduleId, true);

    if(pLogEntry != NULL)
    {
        pLogEntry->logFlags |= (uint16_t)TCPIP_PKT_LOG_FLAG_DONE;
        pLogEntry->ackRes = (int16_t)ackRes;
        pLogEntry->pktAcker = (uint16_t)moduleId;
        // store the module to log it
        pLogEntry->moduleLog |= (uint16_t)1U << (uint16_t)moduleId; 
        pLogEntry->ackStamp = SYS_TIME_CounterGet();

        bool discardPkt = false;

        // check if we keep this log
        while(true)
        {
            if(((uint32_t)pktLogInfo.logType & (uint32_t)TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0U)
            {
                if((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_RX) == 0U)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }
            else if(((uint32_t)pktLogInfo.logType & (uint32_t)TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0U)
            {
                if((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_TX) == 0U)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }
            else if((((uint32_t)pktLogInfo.logType & (uint32_t)TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0U) && ((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0U))
            {   // discard it
                discardPkt = true;
                break;
            }
            else
            {
                // do nothing
            }

            if((pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0U)
            {   // a socket entry; check against skt discard mask
                if(((1UL << pLogEntry->sktNo) & pktLogInfo.sktLogMask) == 0U)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }
            else
            {
                /* Do Nothing */
            }

            // see if this is to be made persistent
            if((pLogEntry->moduleLog & pktLogInfo.persistMask) != 0U)
            {
                pLogEntry->logFlags |= (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT;
                pktLogInfo.nPersistent++;
            }
            else if((pLogEntry->moduleLog & pktLogInfo.logModuleMask) == 0U)
            {   // discard it
                discardPkt = true;
            }
            else
            {
                /* Do Nothing */
            }
            // else keep it

            break;
        }

        if(discardPkt)
        {
            F_TCPIP_PKT_LogDiscardEntry(pLogEntry);
        }
    }
    // else it can happen if the service was stopped!
}

bool  TCPIP_PKT_FlightLogGetInfo(TCPIP_PKT_LOG_INFO* pLogInfo)
{
    if(pLogInfo != NULL)
    {
        *pLogInfo = pktLogInfo;
    }

    return true;
}

// populates a log entry with data for a index
bool TCPIP_PKT_FlightLogGetEntry(size_t entryIx, TCPIP_PKT_LOG_ENTRY* pLEntry)
{
    TCPIP_PKT_LOG_ENTRY* pEntry;

    if(entryIx < (sizeof(pktLogTbl) / sizeof(*pktLogTbl)))
    {
        pEntry = pktLogTbl + entryIx;
        if(pEntry->pPkt != NULL)
        {
            if(pLEntry != NULL)
            {
                *pLEntry = *pEntry;
            }
            return true;
        }
    }

    return false;
}

bool TCPIP_PKT_FlightLogRegister(TCPIP_PKT_LOG_HANDLER logHandler, bool logAll)
{
    uint32_t logType;

    pktLogInfo.logHandler = logHandler;
    if(logAll)
    {
        logType = (uint32_t)pktLogInfo.logType | (uint32_t)TCPIP_PKT_LOG_TYPE_HANDLER_ALL; 
        pktLogInfo.logType = (TCPIP_PKT_LOG_TYPE)logType; 
    }
    else
    {
        logType = (uint32_t)pktLogInfo.logType & ~((uint32_t)TCPIP_PKT_LOG_TYPE_HANDLER_ALL);
        pktLogInfo.logType = (TCPIP_PKT_LOG_TYPE)logType;
    }
    return true;
}

bool TCPIP_PKT_FlightLogTypeSet(TCPIP_PKT_LOG_TYPE logType, bool clrPersist)
{
    TCPIP_PKT_LOG_FLAGS searchRxTx;
    uint32_t uLogType;

    uLogType = (uint32_t)pktLogInfo.logType & ((uint32_t)TCPIP_PKT_LOG_TYPE_RX_ONLY | (uint32_t)TCPIP_PKT_LOG_TYPE_TX_ONLY | (uint32_t)TCPIP_PKT_LOG_TYPE_SKT_ONLY);
    TCPIP_PKT_LOG_TYPE oldType = (TCPIP_PKT_LOG_TYPE)uLogType;

    uLogType = (uint32_t)logType & ((uint32_t)TCPIP_PKT_LOG_TYPE_RX_ONLY | (uint32_t)TCPIP_PKT_LOG_TYPE_TX_ONLY | (uint32_t)TCPIP_PKT_LOG_TYPE_SKT_ONLY);
    logType = (TCPIP_PKT_LOG_TYPE)uLogType;

    // don't allow unknown types/filters
    if(((uint32_t)logType & ((uint32_t)TCPIP_PKT_LOG_TYPE_RX_ONLY | (uint32_t)TCPIP_PKT_LOG_TYPE_TX_ONLY)) == ((uint32_t)TCPIP_PKT_LOG_TYPE_RX_ONLY | (uint32_t)TCPIP_PKT_LOG_TYPE_TX_ONLY)) 
    {   // these 2 are exclusive
       return false;
    } 

    if(((uint32_t)logType & (uint32_t)TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0U) 
    {   
        searchRxTx = TCPIP_PKT_LOG_FLAG_RX;
    }
    else if(((uint32_t)logType & (uint32_t)TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0U) 
    {   
        searchRxTx = TCPIP_PKT_LOG_FLAG_TX;
    }
    else
    {
        logType = TCPIP_PKT_LOG_TYPE_RX_TX;
        searchRxTx = (TCPIP_PKT_LOG_FLAGS)0;
    }

    if(oldType != logType)
    {
        pktLogInfo.logType = logType;

        if(logType != TCPIP_PKT_LOG_TYPE_RX_TX)
        {   // only RX or TX needed; clean up the log
            size_t ix;
            TCPIP_PKT_LOG_ENTRY *pEntry;

            pEntry = pktLogTbl;
            for(ix = 0; ix < (sizeof(pktLogTbl) / sizeof(*pktLogTbl)); ix++)
            {
                if(pEntry->pPkt != NULL && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U))
                {   // non empty completed slot
                    if(clrPersist || ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0U))
                    {   // can check if needs to be discarded 
                        if(((pEntry->logFlags & (uint16_t)searchRxTx) == 0U) || ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0U))
                        {
                            F_TCPIP_PKT_LogDiscardEntry(pEntry);
                        }
                    }
                }
                pEntry++;
            }
        }
    }

    return true;
}

void TCPIP_PKT_FlightLogUpdateModuleMask(uint32_t andModuleMask, uint32_t orModuleMask, bool clrPersist)
{
    size_t ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask only up to and including TCPIP_MODULE_LAYER3 modules
    pktLogInfo.logModuleMask = (( pktLogInfo.logModuleMask & andModuleMask) | orModuleMask) & ((1UL << ((uint32_t)TCPIP_MODULE_LAYER3 + 1U)) - 1U);

    // apply discard mask it to all completed logs...
    pEntry = pktLogTbl;
    for(ix = 0; ix < (sizeof(pktLogTbl) / sizeof(*pktLogTbl)); ix++)
    {
        if((pEntry->pPkt != NULL) && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U))
        {   // non empty completed slot
            if(clrPersist || ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0U))
            {
                if((pEntry->moduleLog & pktLogInfo.logModuleMask) == 0U)
                {
                    F_TCPIP_PKT_LogDiscardEntry(pEntry);
                }
            }
        }
        pEntry++;
    }
}

void TCPIP_PKT_FlightLogUpdatePersistMask(uint32_t andModuleMask, uint32_t orModuleMask, bool clrNonPersist)
{
    size_t ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask only up to and including TCPIP_MODULE_LAYER3 modules
    pktLogInfo.persistMask =  ((pktLogInfo.persistMask & andModuleMask) | orModuleMask) & ((1UL << ((uint32_t)TCPIP_MODULE_LAYER3 + 1U)) - 1U);

    // apply it to all completed logs...
    pEntry = pktLogTbl;
    for(ix = 0; ix < (sizeof(pktLogTbl) / sizeof(*pktLogTbl)); ix++)
    {
        if((pEntry->pPkt != NULL) && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U))
        {   // non empty completed slot
            if((pEntry->moduleLog & pktLogInfo.persistMask) != 0U)
            {
                pEntry->logFlags |= (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT;
            }
            else if(clrNonPersist)
            {
                F_TCPIP_PKT_LogDiscardEntry(pEntry);
            }
            else
            {   // keep it but mark it non persistent
                pEntry->logFlags &= ~(uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT;
            }
        }
        pEntry++;
    }
}

void TCPIP_PKT_FlightLogUpdateNetMask(uint32_t andNetMask, uint32_t orNetMask, bool clrPersist)
{
    size_t ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask
    pktLogInfo.netLogMask = (pktLogInfo.netLogMask & andNetMask) | orNetMask;

    // apply it to all completed logs...
    pEntry = pktLogTbl;
    for(ix = 0; ix < (sizeof(pktLogTbl) / sizeof(*pktLogTbl)); ix++)
    {
        if((pEntry->pPkt != NULL) && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U))
        {   // non empty completed slot
            if(clrPersist || ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0U))
            {
                if((pEntry->netMask & pktLogInfo.netLogMask) == 0U)
                {
                    F_TCPIP_PKT_LogDiscardEntry(pEntry);
                }
            }
        }
        pEntry++;
    }

}  

void TCPIP_PKT_FlightLogUpdateSocketMask(uint32_t andSktMask, uint32_t orSktMask, bool clrPersist)
{
    size_t ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask
    pktLogInfo.sktLogMask = (pktLogInfo.sktLogMask & andSktMask) | orSktMask;

    // apply it to all completed logs...
    pEntry = pktLogTbl;
    for(ix = 0; ix < (sizeof(pktLogTbl) / sizeof(*pktLogTbl)); ix++)
    {
        if((pEntry->pPkt != NULL) && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U))
        {   // non empty completed slot
            if(clrPersist || ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0U))
            {
                if((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0U)
                {
                    if(((1UL << pEntry->sktNo) & pktLogInfo.sktLogMask) == 0U)
                    {
                        F_TCPIP_PKT_LogDiscardEntry(pEntry);
                    }
                }
            }
        }
        pEntry++;
    }

}  
void TCPIP_PKT_FlightLogClear(bool clrPersist)
{
    size_t ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // discard all completed logs...
    pEntry = pktLogTbl;
    for(ix = 0; ix < (sizeof(pktLogTbl) / sizeof(*pktLogTbl)); ix++)
    {
        if((pEntry->pPkt != NULL) && ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U))
        {   // non empty completed slot
            if(clrPersist || ((pEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0U))
            {
                F_TCPIP_PKT_LogDiscardEntry(pEntry);
            }
        }
        pEntry++;
    }
}

void TCPIP_PKT_FlightLogReset(bool resetMasks)
{
    F_TCPIP_PKT_LogInit(resetMasks);
}

#endif  //  (TCPIP_PACKET_LOG_ENABLE)



