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
// definitions


// min heap alignment
// always power of 2
#if (CACHE_LINE_SIZE >= 8u)
typedef struct __attribute__((aligned(CACHE_LINE_SIZE)))
{
    uint64_t     pad[CACHE_LINE_SIZE / 8];
}S_eHeap_Align;
#elif (CACHE_LINE_SIZE >= 4u)
typedef uint32_t S_eHeap_Align;
#else
#error "TCP/IP Heap: incorrect CACHE_LINE_SIZE!"
#endif // (CACHE_LINE_SIZE >= 8u) 

typedef union __attribute__((aligned(CACHE_LINE_SIZE))) U_tag_eHeadNode
{
    S_eHeap_Align x;
    struct
    {
        union U_tag_eHeadNode*    allocPtr;   // pointer as returned by the external allocation function
    };
}u_eHeadNode;



typedef struct
{
    TCPIP_STACK_HEAP_EXTERNAL_CONFIG    heapConfig;         // configuration data save
    size_t                              allocatedBytes;     // how many bytes allocated out there

    OSAL_SEM_HANDLE_TYPE                heapSemaphore;     // protection semaphore
    TCPIP_STACK_HEAP_RES                lastHeapErr;       // last error encountered

    // run time flags - faster access
    uint8_t                             heapDoProtect;      // protect external memory access
    uint8_t                             heapDoMap;          // map buffers to non-cached
    uint8_t                             heapDoAlign;        // align buffers internally
    uint8_t                             heapPad[1];         // padding, not used
}TCPIP_HEAP_EXT_DCPT; // descriptor of a heap


// local data
//

static TCPIP_STACK_HEAP_RES   FS_TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);
static void*            FS_TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);
static void*            FS_TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
static size_t           FS_TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* cptr);

static size_t           FS_TCPIP_HEAP_Unavailable(TCPIP_STACK_HEAP_HANDLE heapH);
static TCPIP_STACK_HEAP_RES   FS_TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH);
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t           FS_TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


// the heap object
static const TCPIP_HEAP_OBJECT      tcpip_heap_object = 
{
    .F_TCPIP_HEAP_Delete = &FS_TCPIP_HEAP_Delete,
    .F_TCPIP_HEAP_Malloc = &FS_TCPIP_HEAP_Malloc,
    .F_TCPIP_HEAP_Calloc = &FS_TCPIP_HEAP_Calloc,
    .F_TCPIP_HEAP_Free = &FS_TCPIP_HEAP_Free,
    .F_TCPIP_HEAP_Size = &FS_TCPIP_HEAP_Unavailable,
    .F_TCPIP_HEAP_MaxSize = &FS_TCPIP_HEAP_Unavailable,
    .F_TCPIP_HEAP_FreeSize = &FS_TCPIP_HEAP_Unavailable,
    .F_TCPIP_HEAP_HighWatermark = &FS_TCPIP_HEAP_Unavailable,
    .F_TCPIP_HEAP_LastError = &FS_TCPIP_HEAP_LastError,
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    .F_TCPIP_HEAP_AllocSize = &FS_TCPIP_HEAP_AllocSize,
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
};

typedef struct
{
    TCPIP_HEAP_OBJECT   heapObj;    // heap object API
    TCPIP_HEAP_EXT_DCPT heapDcpt;   // private heap object data
}TCPIP_HEAP_EXT_OBJ_INSTANCE;



// local prototypes
//
static __inline__ bool __attribute__((always_inline)) FS_TCPIP_HEAP_IsPtrAligned(const void* ptr)
{
    union
    {
        const void* ptr;
        uintptr_t   uptr;
    }U_UINT_PTR;

    U_UINT_PTR.ptr = ptr;
    return U_UINT_PTR.uptr == (U_UINT_PTR.uptr & (~(sizeof(u_eHeadNode) - 1U)));
}

// returns the TCPIP_HEAP_EXT_OBJ_INSTANCE associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_EXT_OBJ_INSTANCE* __attribute__((always_inline)) FS_TCPIP_HEAP_ObjInstance(TCPIP_STACK_HEAP_HANDLE heapH)
{
    union
    {
        TCPIP_STACK_HEAP_HANDLE heapH;
        TCPIP_HEAP_EXT_OBJ_INSTANCE* hInst;
    }U_HANDLE_INST;

    U_HANDLE_INST.heapH = heapH;
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    if(heapH != NULL)
    {
        if(U_HANDLE_INST.hInst->heapObj.F_TCPIP_HEAP_Delete == &FS_TCPIP_HEAP_Delete)
        {
            return U_HANDLE_INST.hInst;
        }
    }
    return NULL;
#else
    return U_HANDLE_INST.hInst;
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

}

// returns the TCPIP_HEAP_EXT_DCPT associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_EXT_DCPT* __attribute__((always_inline)) FS_TCPIP_HEAP_ObjDcpt(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_OBJ_INSTANCE* hInst = FS_TCPIP_HEAP_ObjInstance(heapH);

    return (hInst == NULL) ? NULL : &hInst->heapDcpt;
}

// conversion functions/helpers
//

static __inline__ void* __attribute__((always_inline)) FC_Cvptr2Vptr(const void* cvptr)
{
    union
    {
        const void* cvptr;
        void* vptr;
    }U_CVPTR_VPTR;

    U_CVPTR_VPTR.cvptr = cvptr;
    return U_CVPTR_VPTR.vptr;
}

static __inline__ void* __attribute__((always_inline)) FC_HeadNode2AlgnVptr(u_eHeadNode* node)
{
    union
    {
        u_eHeadNode* node;
        uintptr_t   uptr;
        void*       vptr;
    }U_HEAD_NODE_ALGN_VPTR;

    U_HEAD_NODE_ALGN_VPTR.node = node;
    // align
    U_HEAD_NODE_ALGN_VPTR.uptr = ((U_HEAD_NODE_ALGN_VPTR.uptr + sizeof(u_eHeadNode) - 1U) & ~(sizeof(u_eHeadNode) - 1U));
    return U_HEAD_NODE_ALGN_VPTR.vptr;
}

// API

TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateExternal(const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_HEAP_EXT_DCPT* hDcpt;
    TCPIP_HEAP_EXT_OBJ_INSTANCE* hInst;
    TCPIP_STACK_HEAP_RES  res;
    

    while(true)
    {
        hDcpt = NULL;
        hInst = NULL;

        if( pHeapConfig == NULL || pHeapConfig->malloc_fnc == NULL || pHeapConfig->free_fnc == NULL || pHeapConfig->calloc_fnc == NULL)
        {
            res = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }

        hInst = (TCPIP_HEAP_EXT_OBJ_INSTANCE*)(*pHeapConfig->calloc_fnc)(1, sizeof(*hInst));

        if(hInst == NULL)
        {
            res = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }

        hDcpt = &hInst->heapDcpt;
        if(((uint32_t)pHeapConfig->heapFlags & (uint32_t)TCPIP_STACK_HEAP_FLAG_NO_MTHREAD_SYNC) == 0U)
        {
            if(OSAL_SEM_Create(&hDcpt->heapSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_SUCCESS)
            {
                (*pHeapConfig->free_fnc)(hInst);
                hInst = NULL;
                res = TCPIP_STACK_HEAP_RES_SYNCH_ERR;
                break;
            }

            hDcpt->heapDoProtect = 1;
        }

        // save the configuration parameters
        hDcpt->heapConfig = *pHeapConfig;
        // create the object
        hInst->heapObj = tcpip_heap_object;

        // check if mapping needed; always alloc uncached!
        // if(((uint32_t)hDcpt->heapConfig.heapFlags & (uint32_t)TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED) != 0)
        {
            const void* testPtr = F_TCPIP_HEAP_BufferMapNonCached(hInst, sizeof(*hInst));
            if(hInst != testPtr)
            {
                hDcpt->heapDoMap = 1;
            }
        }
        // check if alignment needed
        if(((uint32_t)hDcpt->heapConfig.heapFlags & (uint32_t)TCPIP_STACK_HEAP_FLAG_ALLOC_UNALIGN) == 0U)
        {
            hDcpt->heapDoAlign = 1;
        }

        res = TCPIP_STACK_HEAP_RES_OK;
        break;
    }

    if(pRes != NULL)
    {
        *pRes = res;
    }

    return hInst;
    
}

// internal functions
//
// deallocates the heap
// NOTE: no check is done if some blocks are still in use!
static TCPIP_STACK_HEAP_RES FS_TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_OBJ_INSTANCE*   hInst = FS_TCPIP_HEAP_ObjInstance(heapH);
    TCPIP_HEAP_EXT_DCPT*   hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt != NULL)
    {
        if (hDcpt->heapDoProtect != 0U)
        {
            (void)OSAL_SEM_Delete(&hDcpt->heapSemaphore);
        }
        // invalidate it
        (void)memset(&hInst->heapObj, 0, sizeof(hInst->heapObj));
        (*hInst->heapDcpt.heapConfig.free_fnc)(hInst);
        return TCPIP_STACK_HEAP_RES_OK;
    }

    return TCPIP_STACK_HEAP_RES_NO_HEAP; 
}


static void* FS_TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
    size_t      mapBytes;
    u_eHeadNode*  ptr;
    TCPIP_HEAP_EXT_DCPT*   hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);
    void* alignPtr = NULL;
    
    if(hDcpt == NULL || nBytes == 0U)
    {
        return NULL;
    }

    if(hDcpt->heapDoAlign != 0U)
    {
        nBytes = ((nBytes + sizeof(u_eHeadNode) - 1U) / sizeof(u_eHeadNode) + 1U) * sizeof(u_eHeadNode);    // allocate multiple of units + 1 entry to store the pointer  
    }

    if (hDcpt->heapDoProtect != 0U)
    {
        (void)OSAL_SEM_Pend(&hDcpt->heapSemaphore, OSAL_WAIT_FOREVER);
    }

    while(true)
    {
        ptr = (u_eHeadNode*)(*hDcpt->heapConfig.malloc_fnc)(nBytes);

        if(ptr == NULL)
        {   // failed
            hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_NO_MEM;
            break;
        }

        if(hDcpt->heapDoAlign != 0U)
        {   
            if(FS_TCPIP_HEAP_IsPtrAligned(ptr))
            {   // 1st unit is for storing the allocated pointer
                alignPtr = ptr + 1;
            }
            else
            {   // always returned an aligned pointer; advance to next aligned address 
                alignPtr = FC_HeadNode2AlgnVptr(ptr);
            }

           // store the modified pointer
            *((uintptr_t*)alignPtr - 1) = (uintptr_t)ptr;
            mapBytes = nBytes - sizeof(u_eHeadNode);
        }
        else
        {
            if(!FS_TCPIP_HEAP_IsPtrAligned(ptr))
            {
                hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_BUFF_ALIGN_ERR;
                break;
            }
            alignPtr = ptr;
            mapBytes = nBytes;
        }

        if(hDcpt->heapDoMap != 0U)
        {   // map to non-cached
            const void* cvptr = F_TCPIP_HEAP_BufferMapNonCached(alignPtr, mapBytes);
            alignPtr = FC_Cvptr2Vptr(cvptr);
        }

        // update stats
        hDcpt->allocatedBytes += nBytes;

        break;
    }

    if (hDcpt->heapDoProtect != 0U)
    {
        (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
    }

    return alignPtr;
}

static void* FS_TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize)
{
    size_t nBytes = nElems * elemSize;

    // redirect to malloc

    void* ptr = FS_TCPIP_HEAP_Malloc(heapH, nBytes);
    if(ptr != NULL)
    {
        (void)memset(ptr, 0, nBytes);
    }

    return ptr;
}

static size_t FS_TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* cptr)
{
    TCPIP_HEAP_EXT_DCPT*   hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);
  
    if(hDcpt != NULL && cptr != NULL)
    {
        union
        {
            const void* cptr;
            void*       vptr;
            uintptr_t*  uptr;
            uintptr_t   uval;
        }U_UINT_PTR;

        U_UINT_PTR.cptr = cptr;

        if (hDcpt->heapDoProtect != 0U)
        {
            (void)OSAL_SEM_Pend(&hDcpt->heapSemaphore, OSAL_WAIT_FOREVER);
        }

        if(hDcpt->heapDoMap != 0U)
        {   // map back to cached
            U_UINT_PTR.cptr = F_TCPIP_HEAP_PointerMapCached(U_UINT_PTR.cptr);
        }

        if(hDcpt->heapDoAlign != 0U)
        {   // restore the allocated pointer
            U_UINT_PTR.uval = *(U_UINT_PTR.uptr - 1);
        }

        (*hDcpt->heapConfig.free_fnc)(U_UINT_PTR.vptr);

        // update stats
        // no info on how many de-allocated bytes here!

        if (hDcpt->heapDoProtect != 0U)
        {
            (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
        }
    }

    return 0;
}


static size_t FS_TCPIP_HEAP_Unavailable(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return 0;   
}

static TCPIP_STACK_HEAP_RES FS_TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_DCPT*      hDcpt;
    TCPIP_STACK_HEAP_RES  res;

    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt != NULL)
    {
        res = hDcpt->lastHeapErr;
        hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_OK;
    }
    else
    {
        res = TCPIP_STACK_HEAP_RES_NO_HEAP; 
    }

    return res;
}

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t FS_TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr)
{
    return 0;
}
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


