/*******************************************************************************
  TCPIP network packet manager implementation

  Summary:
    
  Description:
*******************************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
    #define TCPIP_MAC_DATA_SEGMENT_GAP      34   
#else
    #define TCPIP_MAC_DATA_SEGMENT_GAP      4   
#endif

// MAC driver data offset required by the TCP/IP stack
#define TCPIP_MAC_PAYLOAD_OFFSET            2 

// the TCPIP_MAC_DATA_SEGMENT.segLoadOffset value
// Allocation test
// Note: enabling this in a real app will lead to run time exceptions!
// The segment data gap cannot change at run time!
// Enable for testing purposes only!
#define TCPIP_MAC_SEGMENT_GAP_TEST      0

// Segment payload gap:
// sizeof the TCPIP_MAC_SEGMENT_PAYLOAD::segmentDataGap
#if defined(TCPIP_IF_PIC32WK) || defined(TCPIP_IF_PIC32MZW1)
    #define TCPIP_MAC_DATA_SEGMENT_GAP      34   
#else
    #define TCPIP_MAC_DATA_SEGMENT_GAP      4   
#endif

// should be uintptr_t aligned, for storing the segmentPktPtr
#define _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE  ((((sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT) + TCPIP_MAC_DATA_SEGMENT_GAP) + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)) * sizeof(uintptr_t))

// for TX/RX we place the segment gap in front of the packet:
#define _TCPIP_MAC_GAP_OFFSET    (int16_t)(-_TCPIP_MAC_DATA_SEGMENT_GAP_SIZE)




static TCPIP_STACK_HEAP_HANDLE    pktMemH = 0;

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static TCPIP_PKT_TRACE_ENTRY    _pktTraceTbl[TCPIP_PKT_TRACE_SIZE];


static TCPIP_PKT_TRACE_INFO _pktTraceInfo;  // global counters  


static TCPIP_PKT_TRACE_ENTRY*   _TCPIP_PKT_TraceFindEntry(int moduleId, bool addNewSlot);
static void                     _TCPIP_PKT_TraceAddToEntry(int moduleId, TCPIP_MAC_PACKET* pPkt);
static void                     _TCPIP_PKT_TraceFreeEntry(int moduleId, TCPIP_MAC_PACKET* pPkt);
static void                     _TCPIP_PKT_TraceAckEntry(int moduleId, TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
static uint32_t                 _TCPIP_PKT_TracePktSize(TCPIP_MAC_PACKET* pPkt);


static /*__inline__*/ void /*__attribute__((always_inline))*/ _TCPIP_PKT_TraceFail(void)
{
    _pktTraceInfo.traceFails++;
}

static TCPIP_MAC_PACKET* _TCPIP_PKT_PacketAllocInt(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId);
static void _TCPIP_PKT_PacketFreeInt(TCPIP_MAC_PACKET* pPkt, int moduleId);
static TCPIP_MAC_PACKET*  _TCPIP_PKT_SocketAllocInt(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId);
static TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAllocInt(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags, int moduleId);
static void _TCPIP_PKT_SegmentFreeInt(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if (TCPIP_PACKET_LOG_ENABLE)

// size of the packet log
// The log entries will be overwritten when no available free slot is found!

static TCPIP_PKT_LOG_ENTRY  _pktLogTbl[TCPIP_PKT_LOG_SIZE];

static TCPIP_PKT_LOG_INFO   _pktLogInfo;  // global log counters  

static int                  _pktOverwriteIx;    // simple LRU displacement pointer

static void                 _TCPIP_PKT_LogInit(bool resetAll);

#endif  // (TCPIP_PACKET_LOG_ENABLE)



// API

bool TCPIP_PKT_Initialize(TCPIP_STACK_HEAP_HANDLE heapH, const TCPIP_NETWORK_CONFIG* pNetConf, int nNets)
{
    pktMemH = 0;

    while(heapH != 0)
    {
        TCPIP_MAC_PACKET* allocPtr;

        allocPtr = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(heapH, sizeof(TCPIP_MAC_PACKET));

        if(allocPtr == 0)
        {
            break;
        }

        TCPIP_HEAP_Free(heapH, allocPtr);
        // success
        pktMemH = heapH;

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
        memset(_pktTraceTbl, 0, sizeof(_pktTraceTbl));
        memset(&_pktTraceInfo, 0, sizeof(_pktTraceInfo));
        _pktTraceInfo.nEntries = sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if (TCPIP_PACKET_LOG_ENABLE)
        _TCPIP_PKT_LogInit(true);
        // construct the start netLogMask
        int ix;
        for(ix = 0; ix < nNets; ix++, pNetConf++)
        {
            if((pNetConf->startFlags & TCPIP_NETWORK_CONFIG_PKT_LOG_ON) != 0) 
            {
                _pktLogInfo.netLogMask |= 1 << ix;
            }
        }

#endif  // (TCPIP_PACKET_LOG_ENABLE)

        break;
    }


    return pktMemH != 0;
    
}

void TCPIP_PKT_Deinitialize(void)
{
    pktMemH = 0;
}


// acknowledges a packet
void _TCPIP_PKT_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId)
{
    if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
    {
        pPkt->ackRes = ackRes;
    }

    if(pPkt->ackFunc)
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

    if((pN = pPkt->pDSeg) == 0)
    {   // insert as root
        pPkt->pDSeg = pSeg;
    }
    else
    {   // traverse the list
        for(prev = 0; pN != 0; prev = pN, pN = pN->next);
        prev->next = pSeg;
    }

}

// helpers

bool TCPIP_PKT_PacketMACFormat(TCPIP_MAC_PACKET* pPkt, const TCPIP_MAC_ADDR* dstAddr, const TCPIP_MAC_ADDR* srcAddr, uint16_t pktType)
{
    if(srcAddr)
    {
        TCPIP_MAC_ETHERNET_HEADER* macHdr;
        TCPIP_MAC_ADDR    *destHdrAdd, *srcHdrAdd;

        macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pPkt->pMacLayer;
        srcHdrAdd = &macHdr->SourceMACAddr;

        if(dstAddr)
        {
            destHdrAdd = &macHdr->DestMACAddr;
            memcpy(destHdrAdd, dstAddr, sizeof(*destHdrAdd));
        }

        memcpy(srcHdrAdd, srcAddr, sizeof(*srcHdrAdd));
        // set the MAC frame type
        macHdr->Type = TCPIP_Helper_htons(pktType);

        // update the frame length
        pPkt->pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);
        return true;
    }

    return false;
}

// returns the segment to which dataAddress belongs
// 0 if not in this packet
TCPIP_MAC_DATA_SEGMENT* TCPIP_PKT_DataSegmentGet(TCPIP_MAC_PACKET* pPkt, const uint8_t* dataAddress, bool srchTransport)
{
    TCPIP_MAC_DATA_SEGMENT  *pStartSeg, *pSeg;

    pStartSeg = 0;

    if(srchTransport)
    {   // search the segment containing the transport data
        for(pSeg = pPkt->pDSeg; pSeg != 0; pSeg = pSeg->next)
        {
            if(pSeg->segLoad <= pPkt->pTransportLayer && pPkt->pTransportLayer <= pSeg->segLoad + pSeg->segSize)
            {   // found segment containing the beg of the transport
                if(pPkt->pTransportLayer <= dataAddress && dataAddress <= pSeg->segLoad + pSeg->segSize)
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


    for(pSeg = pStartSeg; pSeg != 0; pSeg = pSeg->next)
    {
        if(pSeg->segLoad <= dataAddress && dataAddress <= pSeg->segLoad + pSeg->segSize)
        {
            return pSeg;
        }
    }

    return 0;
}

uint16_t TCPIP_PKT_PayloadLen(TCPIP_MAC_PACKET* pPkt)
{
    uint32_t payloadSize = 0;

    if(pPkt)
    {
        TCPIP_MAC_DATA_SEGMENT* pSeg = pPkt->pDSeg;

        while(pSeg != 0)
        {
            payloadSize += pSeg->segLen;
            pSeg = pSeg->next;
        }
    }

    return payloadSize;
}

int16_t TCPIP_PKT_GapDcptOffset(void)
{
    return _TCPIP_MAC_GAP_OFFSET; 
}

uint16_t TCPIP_PKT_GapDcptSize(void)
{
    return _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE; 
}


// repeated debug versions; they store the original moduleId
#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) _TCPIP_PKT_PacketAllocInt(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint16_t        pktUpLen, allocLen, segAlignSize, segAllocSize;

    if(pktLen < sizeof(TCPIP_MAC_PACKET))
    {
        pktLen = sizeof(TCPIP_MAC_PACKET);
    }

    pktUpLen = (((pktLen + 3) >> 2) << 2);     // 32 bits round up
    // segment size, multiple of cache line size
    segAlignSize = ((segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER) + TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE;
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = segAlignSize + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_MAC_PAYLOAD_OFFSET + TCPIP_SEGMENT_CACHE_ALIGN_SIZE; 
    // total allocation size
    allocLen = pktUpLen + sizeof(*pSeg) + segAllocSize;

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_MallocDebug(pktMemH, allocLen, moduleId, __LINE__);
#else
    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(pktMemH, allocLen);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

    if(pPkt)
    {   
        // clear the TCPIP_MAC_PACKET and 1st segment fields
        // populate the 1st segment
        memset(pPkt, 0, pktUpLen + sizeof(*pSeg));
        pSeg = (TCPIP_MAC_DATA_SEGMENT*)((uint8_t*)pPkt + pktUpLen);

        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1) + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        // set the pointer to the packet that segment belongs to
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pSeg->segBuffer + _TCPIP_MAC_GAP_OFFSET);
        pGap->segmentPktPtr = pPkt;

        pSeg->segFlags = TCPIP_MAC_SEG_FLAG_STATIC; // embedded in TCPIP_MAC_PACKET itself
        pPkt->pDSeg = pSeg;

        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
        pPkt->pMacLayer = pSeg->segLoad;
        pPkt->pktFlags = flags & (~TCPIP_MAC_PKT_FLAG_STATIC);  // this packet is dynamically allocated
        if(segLoadLen)
        {
            pPkt->pNetLayer = pPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        }

    }

    return pPkt;
}

static __inline__ void __attribute__((always_inline)) _TCPIP_PKT_PacketFreeInt(TCPIP_MAC_PACKET* pPkt, int moduleId)
{
    if((pPkt->pktFlags & TCPIP_MAC_PKT_FLAG_STATIC) == 0)
    {   // we don't deallocate static packets
        TCPIP_MAC_DATA_SEGMENT  *pSeg, *pNSeg;

        for(pSeg = pPkt->pDSeg; pSeg != 0; pSeg = pNSeg)
        {
            pNSeg = pSeg->next;
            if((pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
            {
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
                TCPIP_HEAP_FreeDebug(pktMemH, pSeg, moduleId);
#else
                TCPIP_HEAP_Free(pktMemH, pSeg);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
            }
        }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        TCPIP_HEAP_FreeDebug(pktMemH, pPkt, moduleId);
#else
        TCPIP_HEAP_Free(pktMemH, pPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    }
}

static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) _TCPIP_PKT_SocketAllocInt(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    uint16_t    netHdrLen, totHdrLen;
    TCPIP_MAC_PACKET* pPkt;

    if((flags & TCPIP_MAC_PKT_FLAG_IPV6) != 0)
    {
        netHdrLen = sizeof(IPV6_HEADER);
    }
    else
    {
        netHdrLen = sizeof(IPV4_HEADER);
    }


    totHdrLen = netHdrLen + transpHdrLen;

    pPkt = _TCPIP_PKT_PacketAllocInt(pktLen, totHdrLen +  payloadLen, flags, moduleId);

    if(pPkt)
    {   // set the layer pointers in place
        if(transpHdrLen)
        {
            pPkt->pTransportLayer = pPkt->pNetLayer + netHdrLen;
        }
    }

    return pPkt;
}

// the segment size is allocated following the rules:
//  - payload size is multiple of cache line size
//  - load starts at a cache aligned address
static __inline__ TCPIP_MAC_DATA_SEGMENT* __attribute__((always_inline)) _TCPIP_PKT_SegmentAllocInt(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags, int moduleId)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t allocLen, segAlignSize, segAllocSize;

    segAlignSize = ((loadLen + TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE;
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = segAlignSize + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_SEGMENT_CACHE_ALIGN_SIZE; 

    // total allocation size
    allocLen = sizeof(*pSeg) + segAllocSize;


#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_MallocDebug(pktMemH, allocLen, moduleId, __LINE__);
#else
    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_Malloc(pktMemH, allocLen);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


    if(pSeg)
    {
        memset(pSeg, 0, sizeof(*pSeg));

        pSeg->segFlags = flags & (~TCPIP_MAC_SEG_FLAG_STATIC);
        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1) + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
    }

    return pSeg;
}

static __inline__ void __attribute__((always_inline)) _TCPIP_PKT_SegmentFreeInt(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId)
{
    if( (pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
    {
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        TCPIP_HEAP_FreeDebug(pktMemH, pSeg, moduleId);
#else
        TCPIP_HEAP_Free(pktMemH, pSeg);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    }

}

TCPIP_MAC_PACKET* _TCPIP_PKT_SocketAllocDebug(uint16_t pktLen, uint16_t tHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    TCPIP_MAC_PACKET* pPkt = _TCPIP_PKT_SocketAllocInt(pktLen, tHdrLen, payloadLen, flags, moduleId);
    _TCPIP_PKT_TraceAddToEntry(moduleId, pPkt);
    return pPkt;

}

TCPIP_MAC_PACKET* _TCPIP_PKT_PacketAllocDebug(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    TCPIP_MAC_PACKET* pPkt = _TCPIP_PKT_PacketAllocInt(pktLen, segLoadLen, flags, moduleId);
    _TCPIP_PKT_TraceAddToEntry(moduleId, pPkt);
    return pPkt;

}


void _TCPIP_PKT_PacketFreeDebug(TCPIP_MAC_PACKET* pPkt, int moduleId)
{
    _TCPIP_PKT_TraceFreeEntry(moduleId, pPkt);
    _TCPIP_PKT_PacketFreeInt(pPkt, moduleId);
}

TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAllocDebug(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags, int moduleId)
{
    return _TCPIP_PKT_SegmentAllocInt(loadLen, flags, moduleId); 
}

void _TCPIP_PKT_SegmentFreeDebug(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId)
{
    _TCPIP_PKT_SegmentFreeInt(pSeg, moduleId);
}


void _TCPIP_PKT_PacketAcknowledgeDebug(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId)
{
    _TCPIP_PKT_PacketAcknowledge(pPkt, ackRes, moduleId);
    _TCPIP_PKT_TraceAckEntry(moduleId, pPkt, ackRes);
}

int TCPIP_PKT_TraceGetEntriesNo(TCPIP_PKT_TRACE_INFO* pTraceInfo)
{
    TCPIP_PKT_TRACE_ENTRY *pEntry;
    int ix;
    int nUsed = 0;


    for(ix = 0, pEntry = _pktTraceTbl; ix < sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl); ix++, pEntry++)
    {
        if(pEntry->moduleId > 0)
        {
            nUsed++;
        }
    }

    _pktTraceInfo.nUsed = nUsed;
    if(pTraceInfo)
    {
        *pTraceInfo = _pktTraceInfo;
    }


    return nUsed;
}


// populates a trace entry with data for a index
bool TCPIP_PKT_TraceGetEntry(int entryIx, TCPIP_PKT_TRACE_ENTRY* tEntry)
{
    TCPIP_PKT_TRACE_ENTRY *pEntry;

    if(entryIx < sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl))
    {   // valid index
        pEntry = _pktTraceTbl + entryIx;
        if(pEntry->moduleId > 0)
        {
            *tEntry = *pEntry;
            return true;
        }
    }

    return false;
}

static TCPIP_PKT_TRACE_ENTRY* _TCPIP_PKT_TraceFindEntry(int moduleId, bool addNewSlot)
{
    int ix;
    TCPIP_PKT_TRACE_ENTRY    *freeEntry,*pEntry;

    freeEntry = 0;
    for(ix = 0, pEntry = _pktTraceTbl; ix < sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl); ix++, pEntry++)
    {
        if(pEntry->moduleId == moduleId)
        {
            return pEntry;
        }
        else if(addNewSlot && freeEntry == 0 && pEntry->moduleId == 0)
        {
            freeEntry = pEntry;
        }
    }

    if(freeEntry)
    {
        memset(freeEntry, 0x0, sizeof(*freeEntry));
        freeEntry->moduleId = moduleId;
    }

    return freeEntry;
}

static uint32_t _TCPIP_PKT_TracePktSize(TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg = pPkt->pDSeg;
    uint32_t pktSize = ((uint8_t*)pSeg - (uint8_t*)pPkt) + sizeof(*pSeg) + pSeg->segAllocSize;

    while((pSeg = pSeg->next) != 0)
    {
        if((pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
        {
            pktSize += sizeof(*pSeg) + pSeg->segAllocSize;
        }
    }

    return pktSize;

}
    
static void _TCPIP_PKT_TraceAddToEntry(int moduleId, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = _TCPIP_PKT_TraceFindEntry(moduleId, true);

    if(pEntry)
    {
        if(pPkt)
        {
            pEntry->totAllocated++;
            pEntry->currAllocated++;
            pEntry->currSize += _TCPIP_PKT_TracePktSize(pPkt);
        }
        else
        {
            pEntry->totFailed++;
        }
    }
    else
    {
        _TCPIP_PKT_TraceFail();
    }

}



static void _TCPIP_PKT_TraceFreeEntry(int moduleId, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = _TCPIP_PKT_TraceFindEntry(moduleId, false);

    if(pEntry)
    {
        pEntry->currAllocated--;
        pEntry->currSize -= _TCPIP_PKT_TracePktSize(pPkt);
    }
    else
    {
        _TCPIP_PKT_TraceFail();
    }

}

static void _TCPIP_PKT_TraceAckEntry(int moduleId, TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = _TCPIP_PKT_TraceFindEntry(moduleId, false);

    if(pEntry)
    {
        pEntry->nAcks++;
        if(ackRes < 0)
        {
            _pktTraceInfo.traceAckErrors++;
        }
    }
    else
    {
        _pktTraceInfo.traceAckOwnerFails++;
    }

}

#else
// regular allocation functions, non-debug
// they don't take a module Id parameter; otherwise exactly the same functions
TCPIP_MAC_PACKET* _TCPIP_PKT_PacketAlloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint16_t        pktUpLen, allocLen, segAlignSize, segAllocSize;

    if(pktLen < sizeof(TCPIP_MAC_PACKET))
    {
        pktLen = sizeof(TCPIP_MAC_PACKET);
    }

    pktUpLen = (((pktLen + 3) >> 2) << 2);     // 32 bits round up
    // segment size, multiple of cache line size
    segAlignSize = ((segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER) + TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE;
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = segAlignSize + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_MAC_PAYLOAD_OFFSET + TCPIP_SEGMENT_CACHE_ALIGN_SIZE; 
    // total allocation size
    allocLen = pktUpLen + sizeof(*pSeg) + segAllocSize;

    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(pktMemH, allocLen);

    if(pPkt)
    {   
        // clear the TCPIP_MAC_PACKET and 1st segment fields
        // populate the 1st segment
        memset(pPkt, 0, pktUpLen + sizeof(*pSeg));
        pSeg = (TCPIP_MAC_DATA_SEGMENT*)((uint8_t*)pPkt + pktUpLen);

        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1) + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        // set the pointer to the packet that segment belongs to
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pSeg->segBuffer + _TCPIP_MAC_GAP_OFFSET);
        pGap->segmentPktPtr = pPkt;

        pSeg->segFlags = TCPIP_MAC_SEG_FLAG_STATIC; // embedded in TCPIP_MAC_PACKET itself
        pPkt->pDSeg = pSeg;

        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
        pPkt->pMacLayer = pSeg->segLoad;
        pPkt->pktFlags = flags & (~TCPIP_MAC_PKT_FLAG_STATIC);  // this packet is dynamically allocated
        if(segLoadLen)
        {
            pPkt->pNetLayer = pPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        }
        pPkt->pktPriority = 0; // set the default priority

    }

    return pPkt;
}

// allocates a socket packet
TCPIP_MAC_PACKET*  _TCPIP_PKT_SocketAlloc(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    uint16_t    netHdrLen, totHdrLen;
    TCPIP_MAC_PACKET* pPkt;

    if((flags & TCPIP_MAC_PKT_FLAG_IPV6) != 0)
    {
        netHdrLen = sizeof(IPV6_HEADER);
    }
    else
    {
        netHdrLen = sizeof(IPV4_HEADER);
    }

    totHdrLen = netHdrLen + transpHdrLen;

    pPkt = _TCPIP_PKT_PacketAlloc(pktLen, totHdrLen +  payloadLen, flags );

    if(pPkt)
    {   // set the layer pointers in place
        if(transpHdrLen)
        {
            pPkt->pTransportLayer = pPkt->pNetLayer + netHdrLen;
        }
    }

    return pPkt;
}

// frees a previously allocated packet
void _TCPIP_PKT_PacketFree(TCPIP_MAC_PACKET* pPkt)
{
    if((pPkt->pktFlags & TCPIP_MAC_PKT_FLAG_STATIC) == 0)
    {   // we don't deallocate static packets
        TCPIP_MAC_DATA_SEGMENT  *pSeg, *pNSeg;

        for( pSeg = pPkt->pDSeg; pSeg != 0; pSeg = pNSeg )
        {
            pNSeg = pSeg->next;
            if((pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
            {
                TCPIP_HEAP_Free(pktMemH, pSeg);
            }
        }

        TCPIP_HEAP_Free(pktMemH, pPkt);
    }
}

// the segment size is allocated following the rules:
//  - payload size is multiple of cache line size
//  - load starts at a cache aligned address
TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAlloc(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t allocLen, segAlignSize, segAllocSize;

    segAlignSize = ((loadLen + TCPIP_SEGMENT_CACHE_ALIGN_SIZE  - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE;
    // segment allocation size, extra cache line so that the segBuffer can start on a cache line boundary
    segAllocSize = segAlignSize + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + TCPIP_SEGMENT_CACHE_ALIGN_SIZE; 

    // total allocation size
    allocLen = sizeof(*pSeg) + segAllocSize;

    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_Malloc(pktMemH, allocLen);

    if(pSeg)
    {
        memset(pSeg, 0, sizeof(*pSeg));

        pSeg->segFlags = flags & (~TCPIP_MAC_SEG_FLAG_STATIC);
        pSeg->segSize = segAlignSize;
        pSeg->segAllocSize = segAllocSize;
        pSeg->segBuffer = (uint8_t*)(pSeg + 1) + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
        // cache-align the data segment
        pSeg->segBuffer = (uint8_t*)((((uint32_t)pSeg->segBuffer + TCPIP_SEGMENT_CACHE_ALIGN_SIZE - 1) / TCPIP_SEGMENT_CACHE_ALIGN_SIZE) * TCPIP_SEGMENT_CACHE_ALIGN_SIZE);
        pSeg->segLoad = pSeg->segBuffer + TCPIP_MAC_PAYLOAD_OFFSET;
    }

    return pSeg;
}

void _TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg)
{
    if( (pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
    {
        TCPIP_HEAP_Free(pktMemH, pSeg);
    }
}
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)


#if (TCPIP_PACKET_LOG_ENABLE)


// discards an used log entry
static __inline__ void __attribute__((always_inline)) _TCPIP_PKT_LogDiscardEntry(TCPIP_PKT_LOG_ENTRY* pEntry)
{
    if(pEntry->pPkt != 0)
    {
        _pktLogInfo.nUsed--;
        if((pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) != 0)
        {
            _pktLogInfo.nPersistent--;
        }
    }
    pEntry->pPkt = 0;

}

// initializes the log
// if resetAll is specified then all the info is cleared
// otherwise the current masks are retained
static void _TCPIP_PKT_LogInit(bool resetAll)
{
    memset(_pktLogTbl, 0, sizeof(_pktLogTbl));
    _pktOverwriteIx = 0;

    if(resetAll)
    {
        memset(&_pktLogInfo, 0, sizeof(_pktLogInfo));
        _pktLogInfo.nEntries = sizeof(_pktLogTbl) / sizeof(*_pktLogTbl);
    }
    else
    {
        _pktLogInfo.nUsed = 0;
        _pktLogInfo.nPersistent = 0;
        _pktLogInfo.nFailed = 0;
    }
}

// finds a log entry that matches the pPkt
// if itExists == true, then only an existing packet is searched for
static TCPIP_PKT_LOG_ENTRY* _TCPIP_PKT_LogFindEntry(TCPIP_MAC_PACKET* pPkt, int moduleId, bool itExists)
{
    int ix, lowOvrIx, hiOvrIx;
    TCPIP_PKT_LOG_ENTRY *freeEntry, *pEntry;
    TCPIP_PKT_LOG_ENTRY *pLowOvr, *pHiOvr;
    TCPIP_PKT_LOG_FLAGS logFlags;

    freeEntry = pLowOvr = pHiOvr = 0;
    lowOvrIx = hiOvrIx = 0;
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt == 0)
        {   // empty slot
            if(freeEntry == 0)
            {
                freeEntry = pEntry;
            }
            continue;
        }

        if((pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
        {   // can overwrite
            if(ix <= _pktOverwriteIx)
            {
                pLowOvr = pEntry;
                lowOvrIx = ix;
            }
            else if(ix > _pktOverwriteIx && pHiOvr == 0)
            {
                pHiOvr = pEntry;
                hiOvrIx = ix;
            }
        }


        if(pEntry->pPkt == pPkt && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) == 0)
        {   // packet match. this is it
            pEntry->logFlags &= ~(TCPIP_PKT_LOG_FLAG_NEW | TCPIP_PKT_LOG_FLAG_REPLACE);
            return pEntry;
        }

    }

    // check if we have a freeEntry or overwrite to setup
    if(itExists)
    {   
        return 0;
    }

    if(freeEntry)
    {
        pEntry = freeEntry;
        logFlags = TCPIP_PKT_LOG_FLAG_NEW;
    }
    else if(pHiOvr != 0)
    {
        pEntry = pHiOvr;
        logFlags = TCPIP_PKT_LOG_FLAG_REPLACE;
        _pktOverwriteIx = hiOvrIx;
    }
    else if(pLowOvr != 0)
    {
        pEntry = pLowOvr;
        logFlags = TCPIP_PKT_LOG_FLAG_REPLACE;
        _pktOverwriteIx = lowOvrIx;
    }
    else
    {
        pEntry = 0;
        logFlags = TCPIP_PKT_LOG_FLAG_NONE;
    }

    if(pEntry)
    {
        memset(pEntry, 0x0, sizeof(*pEntry));
        pEntry->pPkt = pPkt;
        pEntry->logFlags = logFlags;
    }
    // else we're out of slots...

    return pEntry;
}


static void _TCPIP_PKT_LogCallHandler(TCPIP_PKT_LOG_ENTRY* pLogEntry, TCPIP_STACK_MODULE moduleId)
{
    // check if we need to report this log
    while(_pktLogInfo.logHandler != 0)
    {
        if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_HANDLER_ALL) == 0)
        {
            if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_RX) == 0)
                {   // don't report
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_TX) == 0)
                {   // don't report
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0)
                {   // don't report
                    break;
                }
            }

            if((pLogEntry->moduleLog & _pktLogInfo.logModuleMask) == 0)
            {   // don't report
                break;
            }
        }

        // report it
        (*_pktLogInfo.logHandler)(moduleId, pLogEntry);
        break;
    }
}

static TCPIP_PKT_LOG_ENTRY* _TCPIP_PKT_FlightLog(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_PKT_LOG_FLAGS logFlags)
{

    uint16_t netMask = 1 << TCPIP_STACK_NetIxGet((TCPIP_NET_IF*)pPkt->pktIf);

    if((_pktLogInfo.netLogMask & netMask) == 0)
    {   // not logging this
        return 0;
    }

    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_LogFindEntry(pPkt, moduleId, false);

    if(pLogEntry == 0)
    {
        _pktLogInfo.nFailed++;
        return 0;
    }


    if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_NEW) != 0)
    {
        _pktLogInfo.nUsed++;
    }
    // else probably a replacement

    if((pLogEntry->logFlags & (TCPIP_PKT_LOG_FLAG_NEW | TCPIP_PKT_LOG_FLAG_REPLACE)) != 0)
    {
        pLogEntry->pktOwner = moduleId;
        pLogEntry->netMask = netMask;
    }

    uint32_t tStamp = SYS_TIME_CounterGet();
    if(moduleId >= TCPIP_MODULE_MAC_START)
    {
        pLogEntry->macId = moduleId;
        pLogEntry->macStamp = tStamp;
    }
    else
    {   // some module can log the same packet multiple times if routed internally
        if(moduleId > TCPIP_MODULE_LAYER3)
        {
            moduleId = TCPIP_MODULE_LAYER3;
        }

        pLogEntry->moduleLog |= 1 << moduleId;
        pLogEntry->moduleStamp[moduleId - 1] = tStamp;
    }

    pLogEntry->logFlags |= logFlags;


    return pLogEntry;


}

void TCPIP_PKT_FlightLogTx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_TX);

    if(pLogEntry)
    {
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogRx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry =  _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_RX);

    if(pLogEntry)
    {
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogTxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo )
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_TX);

    if(pLogEntry)
    {
        pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_SKT_PARAM;
        pLogEntry->sktNo =  sktNo;
        pLogEntry->lclPort =  (uint16_t)(lclRemPort >> 16);
        pLogEntry->remPort =  (uint16_t)(lclRemPort);
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogRxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo )
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_RX);

    if(pLogEntry)
    {
        pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_SKT_PARAM;
        pLogEntry->sktNo =  sktNo;
        pLogEntry->lclPort =  (uint16_t)(lclRemPort >> 16);
        pLogEntry->remPort =  (uint16_t)(lclRemPort);
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}


void TCPIP_PKT_FlightLogAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PKT_ACK_RES ackRes)
{

    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_LogFindEntry(pPkt, moduleId, true);

    if(pLogEntry != 0)
    {
        pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_DONE;
        pLogEntry->ackRes = ackRes;
        pLogEntry->pktAcker = moduleId;
        // store the module to log it
        pLogEntry->moduleLog |= 1 << moduleId; 
        pLogEntry->ackStamp = SYS_TIME_CounterGet();

        bool discardPkt = false;

        // check if we keep this log
        while(true)
        {
            if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_RX) == 0)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_TX) == 0)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0 && (pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0)
            {   // discard it
                discardPkt = true;
                break;
            }

            if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0)
            {   // a socket entry; check against skt discard mask
                if(((1 << pLogEntry->sktNo) & _pktLogInfo.sktLogMask) == 0)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }

            // see if this is to be made persistent
            if((pLogEntry->moduleLog & _pktLogInfo.persistMask) != 0)
            {
                pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_PERSISTENT;
                _pktLogInfo.nPersistent++;
            }
            else if((pLogEntry->moduleLog & _pktLogInfo.logModuleMask) == 0)
            {   // discard it
                discardPkt = true;
            }
            // else keep it

            break;
        }

        if(discardPkt)
        {
            _TCPIP_PKT_LogDiscardEntry(pLogEntry);
        }
    }
    // else it can happen if the service was stopped!
}

bool  TCPIP_PKT_FlightLogGetInfo(TCPIP_PKT_LOG_INFO* pLogInfo)
{
    if(pLogInfo)
    {
        *pLogInfo = _pktLogInfo;
    }

    return true;
}

// populates a log entry with data for a index
bool TCPIP_PKT_FlightLogGetEntry(int entryIx, TCPIP_PKT_LOG_ENTRY* pLEntry)
{
    TCPIP_PKT_LOG_ENTRY* pEntry;

    if(entryIx < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl))
    {
        pEntry = _pktLogTbl + entryIx;
        if(pEntry->pPkt != 0)
        {
            if(pLEntry != 0)
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
    _pktLogInfo.logHandler = logHandler;
    if(logAll)
    {
        _pktLogInfo.logType |= TCPIP_PKT_LOG_TYPE_HANDLER_ALL; 
    }
    else
    {
        _pktLogInfo.logType &= ~TCPIP_PKT_LOG_TYPE_HANDLER_ALL; 
    }
    return true;
}

bool TCPIP_PKT_FlightLogTypeSet(TCPIP_PKT_LOG_TYPE logType, bool clrPersist)
{
    TCPIP_PKT_LOG_FLAGS searchRxTx;
    bool                sktsOnly = false;
    TCPIP_PKT_LOG_TYPE oldType = _pktLogInfo.logType & (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY | TCPIP_PKT_LOG_TYPE_SKT_ONLY);

    logType &= (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY | TCPIP_PKT_LOG_TYPE_SKT_ONLY);
    // don't allow unknown types/filters
    if((logType & (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY)) == (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY)) 
    {   // these 2 are exclusive
       return false;
    } 

    if((logType & TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0) 
    {   
        searchRxTx = TCPIP_PKT_LOG_FLAG_RX;
    }
    else if((logType & TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0) 
    {   
        searchRxTx = TCPIP_PKT_LOG_FLAG_TX;
    }
    else
    {
        logType = TCPIP_PKT_LOG_TYPE_RX_TX;
        searchRxTx = 0;
        sktsOnly = (logType & TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0;
    }

    if(oldType != logType)
    {
        _pktLogInfo.logType = logType;

        if(logType != TCPIP_PKT_LOG_TYPE_RX_TX)
        {   // only RX or TX needed; clean up the log
            int ix;
            TCPIP_PKT_LOG_ENTRY *pEntry;

            for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
            {
                if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
                {   // non empty completed slot
                    if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
                    {   // can check if needs to be discarded 
                        if((pEntry->logFlags & searchRxTx) == 0 || (sktsOnly && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0))
                        {
                            _TCPIP_PKT_LogDiscardEntry(pEntry);
                        }
                    }
                }
            }
        }
    }

    return true;
}

void TCPIP_PKT_FlightLogUpdateModuleMask(uint32_t andModuleMask, uint32_t orModuleMask, bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask only up to and including TCPIP_MODULE_LAYER3 modules
    _pktLogInfo.logModuleMask = (( _pktLogInfo.logModuleMask & andModuleMask) | orModuleMask) & ((1 << (TCPIP_MODULE_LAYER3 + 1)) - 1);

    // apply discard mask it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                if((pEntry->moduleLog & _pktLogInfo.logModuleMask) == 0)
                {
                    _TCPIP_PKT_LogDiscardEntry(pEntry);
                }
            }
        }
    }
}

void TCPIP_PKT_FlightLogUpdatePersistMask(uint32_t andModuleMask, uint32_t orModuleMask, bool clrNonPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask only up to and including TCPIP_MODULE_LAYER3 modules
    _pktLogInfo.persistMask =  ((_pktLogInfo.persistMask & andModuleMask) | orModuleMask) & ((1 << (TCPIP_MODULE_LAYER3 + 1)) - 1);

    // apply it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if((pEntry->moduleLog & _pktLogInfo.persistMask) != 0)
            {
                pEntry->logFlags |= TCPIP_PKT_LOG_FLAG_PERSISTENT;
            }
            else if(clrNonPersist)
            {
                _TCPIP_PKT_LogDiscardEntry(pEntry);
            }
            else
            {   // keep it but mark it non persistent
                pEntry->logFlags &= ~TCPIP_PKT_LOG_FLAG_PERSISTENT;
            }
        }
    }
}

void TCPIP_PKT_FlightLogUpdateNetMask(uint32_t andNetMask, uint32_t orNetMask, bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask
    _pktLogInfo.netLogMask = (_pktLogInfo.netLogMask & andNetMask) | orNetMask;

    // apply it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                if((pEntry->netMask & _pktLogInfo.netLogMask) == 0)
                {
                    _TCPIP_PKT_LogDiscardEntry(pEntry);
                }
            }
        }
    }

}  

void TCPIP_PKT_FlightLogUpdateSocketMask(uint32_t andSktMask, uint32_t orSktMask, bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // update mask
    _pktLogInfo.sktLogMask = (_pktLogInfo.sktLogMask & andSktMask) | orSktMask;

    // apply it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                if((pEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0)
                {
                    if(((1 << pEntry->sktNo) & _pktLogInfo.sktLogMask) == 0)
                    {
                        _TCPIP_PKT_LogDiscardEntry(pEntry);
                    }
                }
            }
        }
    }

}  
void TCPIP_PKT_FlightLogClear(bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // discard all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                _TCPIP_PKT_LogDiscardEntry(pEntry);
            }
        }
    }
}

void TCPIP_PKT_FlightLogReset(bool resetMasks)
{
    _TCPIP_PKT_LogInit(resetMasks);
}

#endif  //  (TCPIP_PACKET_LOG_ENABLE)



