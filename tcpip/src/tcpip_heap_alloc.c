/*******************************************************************************
  TCPIP Heap Allocation Manager

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









#include <string.h>
#include <stdlib.h>

#if defined(__mips__)
#include <sys/kmem.h>
#endif


#include "tcpip/src/tcpip_private.h"



// cache helpers

#if defined(__PIC32MZ__)
// flushes a data cache line/address
static __inline__ void __attribute__((always_inline)) F_TCPIP_HEAP_DataLineFlush(const void* address)
{   // issue a hit-writeback invalidate order
    __asm__ __volatile__ ("cache 0x15, 0(%0)" ::"r"(address));
}

static __inline__ void __attribute__((always_inline)) F_TCPIP_HEAP_DataSync(void)
{   // issue a 'sync' instruction
    __asm__ __volatile__ ("sync");
}


// invalidates a range of addresses from the cache
static void F_TCPIP_HEAP_DataCacheInvalidate(const void* address, size_t nBytes)
{
    size_t ix;

    uint32_t u32Addr = FC_CvPtr2U32(address);
    u32Addr &= 0xfffffff0U; // start on an even cache line
    const uint8_t*    pLine = (const uint8_t*)FC_U322CvPtr(u32Addr); // start on an even cache line
    nBytes += (uint16_t)FC_U8PtrDiff2UI16((const uint8_t*)address, pLine);
    size_t nLines = (nBytes + 15U) / 16U;   // round up the number of taken cache lines 

    for(ix = 0; ix < nLines; ix ++)
    {
        F_TCPIP_HEAP_DataLineFlush(pLine);
        pLine += 16;
    }

    F_TCPIP_HEAP_DataSync();
}

/* MISRA C-2012 Rule 11.6 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_11_6_NET_DR_9 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 11.6" "H3_MISRAC_2012_R_11_6_NET_DR_9" 
const void* F_TCPIP_HEAP_BufferMapNonCached(const void* dataBuff, size_t buffSize)
{
    if(IS_KVA0(dataBuff))
    {   // map in K1
        F_TCPIP_HEAP_DataCacheInvalidate(dataBuff, buffSize);
        dataBuff = (const void*)KVA0_TO_KVA1(dataBuff);
    }

    return dataBuff;
}

const void* F_TCPIP_HEAP_PointerMapCached(const void* ptr)
{
    if(IS_KVA1(ptr))
    {   // map in K1
        ptr = (const void*)KVA1_TO_KVA0(ptr);
    }

    return ptr;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 11.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

#else
const void* F_TCPIP_HEAP_BufferMapNonCached(const void* dataBuff, size_t buffSize)
{
    return dataBuff;
}
const void* F_TCPIP_HEAP_PointerMapCached(const void* ptr)
{
    return ptr;
}

#endif  // defined(__PIC32MZ__)

static __inline__ const TCPIP_STACK_HEAP_INTERNAL_CONFIG* __attribute__((always_inline)) FC_HeapCfg2IntCfg(const TCPIP_STACK_HEAP_CONFIG* cfg)
{
    union
    {
        const TCPIP_STACK_HEAP_CONFIG* cfg;
        const TCPIP_STACK_HEAP_INTERNAL_CONFIG* intCfg;
        const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* extCfg;
        const TCPIP_STACK_HEAP_POOL_CONFIG* poolCfg;

    }U_HEAP_CFG;

    U_HEAP_CFG.cfg = cfg;
    return U_HEAP_CFG.intCfg;
}

static __inline__ const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* __attribute__((always_inline)) FC_HeapCfg2ExtCfg(const TCPIP_STACK_HEAP_CONFIG* cfg)
{
    union
    {
        const TCPIP_STACK_HEAP_CONFIG* cfg;
        const TCPIP_STACK_HEAP_INTERNAL_CONFIG* intCfg;
        const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* extCfg;
        const TCPIP_STACK_HEAP_POOL_CONFIG* poolCfg;

    }U_HEAP_CFG;

    U_HEAP_CFG.cfg = cfg;
    return U_HEAP_CFG.extCfg;
}

static __inline__ const TCPIP_STACK_HEAP_POOL_CONFIG* __attribute__((always_inline)) FC_HeapCfg2PoolCfg(const TCPIP_STACK_HEAP_CONFIG* cfg)
{
    union
    {
        const TCPIP_STACK_HEAP_CONFIG* cfg;
        const TCPIP_STACK_HEAP_INTERNAL_CONFIG* intCfg;
        const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* extCfg;
        const TCPIP_STACK_HEAP_POOL_CONFIG* poolCfg;

    }U_HEAP_CFG;

    U_HEAP_CFG.cfg = cfg;
    return U_HEAP_CFG.poolCfg;
}


#if !defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

// functions needed when debugging is not enabled

TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_Create(const TCPIP_STACK_HEAP_CONFIG* initData, TCPIP_STACK_HEAP_RES* pRes)
{
    if(initData != NULL)
    {
#if defined (TCPIP_STACK_USE_INTERNAL_HEAP)
        if(initData->heapType == TCPIP_STACK_HEAP_TYPE_INTERNAL)
        {
            const TCPIP_STACK_HEAP_INTERNAL_CONFIG* intCfg = FC_HeapCfg2IntCfg(initData); 
            return TCPIP_HEAP_CreateInternal(intCfg, pRes);
        }
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP)


#if defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
        if(initData->heapType == TCPIP_STACK_HEAP_TYPE_EXTERNAL)
        {
            const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* extCfg = FC_HeapCfg2ExtCfg(initData); 
            return TCPIP_HEAP_CreateExternal(extCfg, pRes);
        }
#endif  // defined (TCPIP_STACK_USE_EXTERNAL_HEAP)


#if defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
        if(initData->heapType == TCPIP_STACK_HEAP_TYPE_POOL)
        {
            const TCPIP_STACK_HEAP_POOL_CONFIG* poolCfg = FC_HeapCfg2PoolCfg(initData); 
            return TCPIP_HEAP_CreateInternalPool(poolCfg, pRes);
        }
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

    }

    return NULL;
}

TCPIP_STACK_HEAP_RES TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return (*((const TCPIP_HEAP_OBJECT*)heapH)->F_TCPIP_HEAP_Delete)(heapH);
}

// functions needed when not inlined
void* TCPIP_HEAP_MallocOutline(TCPIP_STACK_HEAP_HANDLE h, size_t nBytes)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_Malloc)(h, nBytes);
}

void* TCPIP_HEAP_CallocOutline(TCPIP_STACK_HEAP_HANDLE h, size_t nElems, size_t elemSize)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_Calloc)(h, nElems, elemSize);
}

size_t TCPIP_HEAP_FreeOutline(TCPIP_STACK_HEAP_HANDLE h, const void* ptr)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_Free)(h, ptr);
}

#else   // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
// debug functionality

#if defined(TCPIP_STACK_DRAM_TRACE_ENABLE) 
    #define M_TCPIP_STACK_DRAM_TRACE_ENABLE
    #define M_TCPIP_STACK_DRAM_TRACE_NDP_COLLAPSE   1       // collapse NDP info into IPv6 since NDP allocated packets are freed by IPv6
#else
    #undef  M_TCPIP_STACK_DRAM_TRACE_ENABLE
#endif

#if defined(TCPIP_STACK_DRAM_DIST_ENABLE) 
    #define M_TCPIP_STACK_DRAM_DIST_ENABLE

    // distribution block sizes that are monitored
    // Note: there's no way (yet) of allowing the user to specify the requested distribution
    static const int t_tcpip_heap_dist_sizes[] = 
    {
        0,      32,     64,     128,    256,    512,    640,    768,
        896,    1024,   1152,   1280,   1408,   1536,   1792,   2000,
        10000
    };

#else
    #undef  M_TCPIP_STACK_DRAM_DIST_ENABLE
#endif



typedef struct
{
    TCPIP_STACK_HEAP_HANDLE     heapH;          // heap handle; 0 means slot is free
    TCPIP_STACK_HEAP_TYPE       heapType;       // type of heap
    TCPIP_STACK_HEAP_FLAGS      heapFlags;      // creation flags
    TCPIP_HEAP_NO_MEM_HANDLER   noMemHandler;   // out of memory handler for this heap
#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
    TCPIP_HEAP_TRACE_ENTRY      t_heapTraceTbl[TCPIP_STACK_DRAM_TRACE_SLOTS];
#endif
#if defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)
    TCPIP_HEAP_DIST_ENTRY t_tcpip_heap_dist_array[sizeof(t_tcpip_heap_dist_sizes)/sizeof(*t_tcpip_heap_dist_sizes) - 1];
#endif
}TCPIP_HEAP_DBG_DCPT;

// the heap debug descriptor
static TCPIP_HEAP_DBG_DCPT      t_tcpipHeapDbgDcpt[TCPIP_STACK_SUPPORTED_HEAPS] = { {0} };


#define M_heapFailMessage "Heap allocation of %d bytes failed in module %d line %d\r\n"

#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
    static TCPIP_HEAP_TRACE_ENTRY* TCPIP_HEAP_FindEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, bool addNewSlot);
    static void TCPIP_HEAP_AddToEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes, void* ptr);
    static void TCPIP_HEAP_RemoveFromEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes);
#endif  // defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)

#if defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)
static int TCPIP_HEAP_DistCompare(const void *a, const void *b);
static void TCPIP_HEAP_DistAdd(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes);
static void TCPIP_HEAP_DistRem(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes);
#endif  // defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)


// API

TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_Create(const TCPIP_STACK_HEAP_CONFIG* initData, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_STACK_HEAP_HANDLE newH = NULL;
    TCPIP_STACK_HEAP_FLAGS  flags;

    while(initData != NULL)
    {
#if defined (TCPIP_STACK_USE_INTERNAL_HEAP)
        if(initData->heapType == TCPIP_STACK_HEAP_TYPE_INTERNAL)
        {
            const TCPIP_STACK_HEAP_INTERNAL_CONFIG* intCfg = FC_HeapCfg2IntCfg(initData); 
            newH = TCPIP_HEAP_CreateInternal(intCfg, pRes);
            flags = initData->heapFlags;
            break;
        }
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP)

#if defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
        if(initData->heapType == TCPIP_STACK_HEAP_TYPE_EXTERNAL)
        {
            const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* extCfg = FC_HeapCfg2ExtCfg(initData); 
            newH = TCPIP_HEAP_CreateExternal(extCfg, pRes);
            flags = initData->heapFlags;
            break;
        }
#endif  // defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
            
#if defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
        if(initData->heapType == TCPIP_STACK_HEAP_TYPE_POOL)
        {
            const TCPIP_STACK_HEAP_POOL_CONFIG* poolCfg = FC_HeapCfg2PoolCfg(initData); 
            newH = TCPIP_HEAP_CreateInternalPool(poolCfg, pRes);
            flags = initData->heapFlags;
            break;
        }
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

        break;
    }

    if(newH != NULL)
    {
        size_t hIx;
        TCPIP_HEAP_DBG_DCPT* pDcpt = t_tcpipHeapDbgDcpt;
        for(hIx = 0; hIx < sizeof(t_tcpipHeapDbgDcpt) / sizeof(*t_tcpipHeapDbgDcpt); hIx++)
        {
            if(pDcpt->heapH == NULL)
            {   // found slot
                pDcpt->heapH = newH;
                pDcpt->heapType = initData->heapType;
                pDcpt->heapFlags = flags;
#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
                (void)memset(pDcpt->t_heapTraceTbl, 0, sizeof(pDcpt->t_heapTraceTbl)); // clear entries
#endif
#if defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)
                // initialize the distribution sizes array
                int ix;
                TCPIP_HEAP_DIST_ENTRY* pEntry;
                const int* pSize;

                pSize = t_tcpip_heap_dist_sizes;
                pEntry = pDcpt->t_tcpip_heap_dist_array;
                for(ix = 0; ix < sizeof(t_tcpip_heap_dist_sizes) / sizeof(*t_tcpip_heap_dist_sizes) - 1; ix++, pSize++, pEntry++)
                {
                    (void)memset(pEntry, 0, sizeof(*pEntry)); 
                    pEntry->lowLimit = *pSize;
                    pEntry->highLimit = *(pSize + 1);
                }
#endif
                break;
            }
            pDcpt++;
        }
    }

    return newH;
}


static TCPIP_HEAP_DBG_DCPT* F_TCPIP_HEAP_FindDcpt(TCPIP_STACK_HEAP_HANDLE heapH)
{
    size_t hIx;
    TCPIP_HEAP_DBG_DCPT* pDcpt = t_tcpipHeapDbgDcpt;
    for(hIx = 0; hIx < sizeof(t_tcpipHeapDbgDcpt) / sizeof(*t_tcpipHeapDbgDcpt); hIx++)
    {
        if(pDcpt->heapH == heapH)
        {   // found slot
            return pDcpt;
        }
        pDcpt++;
    }

    return NULL;
}

TCPIP_STACK_HEAP_RES TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_STACK_HEAP_RES res;
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt != NULL)
    {
        res =  ((const TCPIP_HEAP_OBJECT*)heapH)->F_TCPIP_HEAP_Delete(heapH);
        if((int)res >= 0)
        {   // success, deleted
            pDcpt->heapH = NULL;
        }
    }
    else
    {
        res = TCPIP_STACK_HEAP_RES_NO_HEAP;
    }

    return res; 

}

void* TCPIP_HEAP_MallocDebug(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes, int moduleId, int lineNo)
{
    const TCPIP_HEAP_OBJECT* hObj = (const TCPIP_HEAP_OBJECT*)heapH;
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    void* ptr = (*hObj->F_TCPIP_HEAP_Malloc)(heapH, nBytes);

    if(ptr == NULL)
    {
        if(pDcpt != NULL)
        {
            if(((uint32_t)pDcpt->heapFlags & (uint32_t)TCPIP_STACK_HEAP_FLAG_NO_WARN_MESSAGE) == 0U)
            {
                SYS_ERROR_PRINT(SYS_ERROR_WARNING, M_heapFailMessage, nBytes, moduleId, lineNo);
            }

            if(pDcpt->noMemHandler != NULL)
            {
                (*pDcpt->noMemHandler)(heapH, nBytes, moduleId, lineNo);
            }
        }
    }
#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
    if(pDcpt != NULL)
    {
        if(ptr != NULL)
        {
            nBytes = (*hObj->F_TCPIP_HEAP_AllocSize)(heapH, ptr);
        }
        TCPIP_HEAP_AddToEntry(pDcpt, moduleId, nBytes, ptr);
    }
#endif  // defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)

#if defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)
    if(pDcpt != NULL)
    {
        TCPIP_HEAP_DistAdd(pDcpt, moduleId, nBytes);
    }
#endif  // defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)

    return ptr;
}

void* TCPIP_HEAP_CallocDebug(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize, int moduleId, int lineNo)
{
    const TCPIP_HEAP_OBJECT* hObj = (const TCPIP_HEAP_OBJECT*)heapH;
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    size_t nBytes = nElems * elemSize;
    void* ptr = (*hObj->F_TCPIP_HEAP_Calloc)(heapH, nElems, elemSize);

    if(ptr == NULL)
    {
        if(pDcpt != NULL)
        {
            if(((uint32_t)pDcpt->heapFlags & (uint32_t)TCPIP_STACK_HEAP_FLAG_NO_WARN_MESSAGE) == 0U)
            {
                SYS_ERROR_PRINT(SYS_ERROR_WARNING, M_heapFailMessage, nBytes, moduleId, lineNo);
            }

            if(pDcpt->noMemHandler != NULL)
            {
                (*pDcpt->noMemHandler)(heapH, nBytes, moduleId, lineNo);
            }
        }
    }

#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
    if(pDcpt != NULL)
    {
        if(ptr != NULL)
        {
            nBytes = (*hObj->F_TCPIP_HEAP_AllocSize)(heapH, ptr);
        }
        TCPIP_HEAP_AddToEntry(pDcpt, moduleId, nBytes, ptr);
    }
#endif  // defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)

#if defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)
    if(pDcpt != NULL)
    {
        TCPIP_HEAP_DistAdd(pDcpt, moduleId, nBytes);
    }
#endif  // defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)

    return ptr;
}

size_t TCPIP_HEAP_FreeDebug(TCPIP_STACK_HEAP_HANDLE heapH,  const void* ptr, int moduleId, int lineNo)
{
    const TCPIP_HEAP_OBJECT* hObj = (const TCPIP_HEAP_OBJECT*)heapH;
#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE) || defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);
#endif

    size_t nBytes = (*hObj->F_TCPIP_HEAP_Free)(heapH, ptr);

#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
    if(pDcpt != NULL && nBytes != 0U)
    {
        TCPIP_HEAP_RemoveFromEntry(pDcpt, moduleId, nBytes);
    }
#endif  // defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
#if defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)
    if(pDcpt != NULL && nBytes != 0)
    {
        TCPIP_HEAP_DistRem(pDcpt, moduleId, nBytes);
    }
#endif  // defined(M_TCPIP_STACK_DRAM_DIST_ENABLE)

    return nBytes;
}

TCPIP_STACK_HEAP_RES TCPIP_HEAP_SetNoMemHandler(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_HEAP_NO_MEM_HANDLER handler)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt != NULL)
    {
        pDcpt->noMemHandler = handler;
        return TCPIP_STACK_HEAP_RES_OK;
    }

    return TCPIP_STACK_HEAP_RES_NO_HEAP;
}

#if defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)
static TCPIP_HEAP_TRACE_ENTRY* TCPIP_HEAP_FindEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, bool addNewSlot)
{
    size_t ix;
    TCPIP_HEAP_TRACE_ENTRY    *freeEntry,*pEntry;
#if M_TCPIP_STACK_DRAM_TRACE_NDP_COLLAPSE
    if(moduleId == (int)TCPIP_MODULE_NDP)
    {
        moduleId = (int)TCPIP_MODULE_IPV6;
    }
#endif  // M_TCPIP_STACK_DRAM_TRACE_NDP_COLLAPSE

    freeEntry = NULL;
    pEntry = hDcpt->t_heapTraceTbl;
    for(ix = 0; ix < sizeof(hDcpt->t_heapTraceTbl)/sizeof(*hDcpt->t_heapTraceTbl); ix++)
    {
        if(pEntry->moduleId == moduleId)
        {
            return pEntry;
        }
        else if(addNewSlot && freeEntry == NULL && pEntry->moduleId == 0)
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
        (void)memset(freeEntry, 0x0, sizeof(*freeEntry));
        freeEntry->moduleId = moduleId;
    }

    return freeEntry;
}

static void TCPIP_HEAP_AddToEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes, void* ptr)
{
    TCPIP_HEAP_TRACE_ENTRY *pEntry = TCPIP_HEAP_FindEntry(hDcpt, moduleId, true);

    if(pEntry != NULL)
    {
        pEntry->nAllocs++;
        if(ptr != NULL)
        {   // successful
            pEntry->totAllocated += nBytes;
            pEntry->currAllocated += (ssize_t)nBytes;
        }
        else
        {
            pEntry->totFailed += nBytes;
            if(nBytes > pEntry->maxFailed)
            {
                pEntry->maxFailed = nBytes;
            }
        }
    }
}

static void TCPIP_HEAP_RemoveFromEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes)
{
    TCPIP_HEAP_TRACE_ENTRY *pEntry = TCPIP_HEAP_FindEntry(hDcpt, moduleId, false);

    if(pEntry != NULL)
    {
        pEntry->nFrees++;
        pEntry->currAllocated -= (ssize_t)nBytes;
        TCPIPStack_Assert(pEntry->currAllocated >= 0 && pEntry->nFrees <= pEntry->nAllocs, __FILE__, __func__, __LINE__);
    }
}

bool TCPIP_HEAP_TraceGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_TRACE_ENTRY* tEntry)
{
    TCPIP_HEAP_TRACE_ENTRY *pEntry;
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt != NULL)
    {
        if(entryIx < sizeof(pDcpt->t_heapTraceTbl)/sizeof(*pDcpt->t_heapTraceTbl))
        {   // valid index
            pEntry = pDcpt->t_heapTraceTbl + entryIx;
            if(pEntry->moduleId > 0)
            {
                *tEntry = *pEntry;
                return true;
            }
        }
    }

    return false;
}

size_t TCPIP_HEAP_TraceGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH, bool getUsed)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt == NULL)
    {
        return 0;
    }

    if(getUsed)
    {
        TCPIP_HEAP_TRACE_ENTRY *pEntry;
        size_t ix, nUsed;

        nUsed = 0;
        pEntry = pDcpt->t_heapTraceTbl;
        for(ix = 0; ix < sizeof(pDcpt->t_heapTraceTbl)/sizeof(*pDcpt->t_heapTraceTbl); ix++)
        {
            if(pEntry->moduleId > 0)
            {
                nUsed++;
            }
            pEntry++;
        }

        return nUsed;
    }

    return sizeof(pDcpt->t_heapTraceTbl)/sizeof(*pDcpt->t_heapTraceTbl);
}


#else

bool TCPIP_HEAP_TraceGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_TRACE_ENTRY* tEntry)
{
    return false;
}

size_t TCPIP_HEAP_TraceGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH, bool getUsed)
{
    return 0;
}

#endif  // defined(M_TCPIP_STACK_DRAM_TRACE_ENABLE)


#if defined (M_TCPIP_STACK_DRAM_DIST_ENABLE)
bool TCPIP_HEAP_DistGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_DIST_ENTRY* pEntry)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt && pEntry)
    {
        if(entryIx < sizeof(pDcpt->t_tcpip_heap_dist_array)/sizeof(*pDcpt->t_tcpip_heap_dist_array))
        {   // valid index
           *pEntry =  *(pDcpt->t_tcpip_heap_dist_array + entryIx);
           return true;
        }
    }

    return false;
}

size_t TCPIP_HEAP_DistGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = F_TCPIP_HEAP_FindDcpt(heapH);

    return pDcpt == NULL ? 0 : sizeof(pDcpt->t_tcpip_heap_dist_array)/sizeof(*pDcpt->t_tcpip_heap_dist_array);
}


static int TCPIP_HEAP_DistCompare(const void *a, const void *b)
{
    TCPIP_HEAP_DIST_ENTRY* pA = (TCPIP_HEAP_DIST_ENTRY*)a;
    TCPIP_HEAP_DIST_ENTRY* pB = (TCPIP_HEAP_DIST_ENTRY*)b;

    if(pA->lowLimit > pB->highLimit)
    {
        return 1;
    }
    else if(pA->highLimit < pB->lowLimit)
    {
        return -1;
    }

    return 0;
}

static void TCPIP_HEAP_DistAdd(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes)
{
    int     modIx;
    TCPIP_HEAP_DIST_ENTRY* pEntry;
    TCPIP_HEAP_DIST_ENTRY nEntry =
    {
        .lowLimit = nBytes,
        .highLimit = nBytes,
        { {0} },
        0
    };
    
    pEntry = bsearch(&nEntry, hDcpt->t_tcpip_heap_dist_array, sizeof(hDcpt->t_tcpip_heap_dist_array)/sizeof(*hDcpt->t_tcpip_heap_dist_array), sizeof(*hDcpt->t_tcpip_heap_dist_array), TCPIP_HEAP_DistCompare);

    pEntry->currHits++;
    // search for module
    struct moduleDist* pFreeMod = 0;
    struct moduleDist* pmDist = pEntry->modDist;
    for(modIx = 0; modIx < sizeof(pEntry->modDist)/sizeof(*pEntry->modDist); modIx++, pmDist++)
    {
        if(pmDist->modId == moduleId)
        {   // found module slot
            pmDist->modHits++;
            return;
        }
        else if(pmDist->modId == 0 && pFreeMod == NULL)
        {
            pFreeMod = pmDist;
        }
    }

    // module slot not found
    if(pFreeMod)
    {
        pFreeMod->modId = moduleId;
        pFreeMod->modHits = 1;
    }
    else
    {   // all slots occupied, use the generic entry
        pEntry->gHits++;
    }
}


static void TCPIP_HEAP_DistRem(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes)
{
    TCPIP_HEAP_DIST_ENTRY* pEntry;
    TCPIP_HEAP_DIST_ENTRY nEntry =
    {
        .lowLimit = nBytes,
        .highLimit = nBytes,
        { {0} },
        0
    };
    
    pEntry = bsearch(&nEntry, hDcpt->t_tcpip_heap_dist_array, sizeof(hDcpt->t_tcpip_heap_dist_array)/sizeof(*hDcpt->t_tcpip_heap_dist_array), sizeof(*hDcpt->t_tcpip_heap_dist_array), TCPIP_HEAP_DistCompare);

    pEntry->currHits--;
}

#else

bool TCPIP_HEAP_DistGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_DIST_ENTRY* pEntry)
{
    return false;
}

size_t TCPIP_HEAP_DistGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return 0;
}

#endif  // defined (M_TCPIP_STACK_DRAM_DIST_ENABLE)


#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

