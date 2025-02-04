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
}S_iHeap_Align;
#elif (CACHE_LINE_SIZE >= 4u)
typedef uint32_t S_iHeap_Align;
#else
#error "TCP/IP Heap: incorrect CACHE_LINE_SIZE!"
#endif // (CACHE_LINE_SIZE >= 8u) 

typedef union __attribute__((aligned(CACHE_LINE_SIZE))) U_tag_iheadNode
{
    S_iHeap_Align x;
    struct
    {
        union U_tag_iheadNode*  next;
        size_t                  units;
    };
}u_iHeadNode;


#define M_TCPIP_HEAP_MIN_BLK_USIZE_      2U     // avoid tiny blocks having units <=  value. 

#define M_TCPIP_HEAP_MIN_BLKS_           64U    // efficiency reasons, the minimum heap size that can be handled. 



typedef struct
{
    u_iHeadNode*     heapHead;
    u_iHeadNode*     heapTail;                  // head and tail pointers
    size_t          heapUnits;                 // size of the heap, units
    size_t          heapAllocatedUnits;        // how many units allocated out there
    size_t          heapWatermark;             // max allocated units
    TCPIP_STACK_HEAP_RES  lastHeapErr;         // last error encountered
    TCPIP_STACK_HEAP_FLAGS heapFlags;          // heap flags
    void*           allocatedBuffer;            // buffer initially allocated for this heap
    void            (*free_fnc)(void* ptr);     // free function needed to delete the heap
    
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    u_iHeadNode*   heapStart;                    // debug checkpoint     
    u_iHeadNode*   heapEnd;                      // debug checkpoint     
#endif

    OSAL_SEM_HANDLE_TYPE heapSemaphore;
    
    // alignment padding
    // u_iHeadNode heap[1];           // the heap itself, dynamically allocated

}TCPIP_HEAP_DCPT; // descriptor of a heap


// local data
//

static TCPIP_STACK_HEAP_RES   FS_TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);
static void*            FS_TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);
static void*            FS_TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
static size_t           FS_TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff);

static size_t           FS_TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           FS_TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           FS_TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           FS_TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH);
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
    .F_TCPIP_HEAP_Size = &FS_TCPIP_HEAP_Size,
    .F_TCPIP_HEAP_MaxSize = &FS_TCPIP_HEAP_MaxSize,
    .F_TCPIP_HEAP_FreeSize = &FS_TCPIP_HEAP_FreeSize,
    .F_TCPIP_HEAP_HighWatermark = &FS_TCPIP_HEAP_HighWatermark,
    .F_TCPIP_HEAP_LastError = &FS_TCPIP_HEAP_LastError,
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    .F_TCPIP_HEAP_AllocSize = &FS_TCPIP_HEAP_AllocSize,
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
};

typedef struct
{
    TCPIP_HEAP_OBJECT   heapObj;    // heap object API
    TCPIP_HEAP_DCPT     heapDcpt;   // private heap object data
}TCPIP_HEAP_OBJ_INSTANCE;



// local prototypes
//

// conversion functions/helpers
//
static __inline__ u_iHeadNode* __attribute__((always_inline)) FC_Cvptr2HeadNode(const void* cvptr)
{
    union
    {
        const void* cvptr;
        u_iHeadNode* node;
    }U_CVPTR_NODE;

    U_CVPTR_NODE.cvptr = cvptr;
    return U_CVPTR_NODE.node;
}

static __inline__ u_iHeadNode* __attribute__((always_inline)) FC_Uptr2HeadNode(uint8_t* uptr)
{
    union
    {
        uint8_t* uptr;
        u_iHeadNode* node;
    }U_UPTR_NODE;

    U_UPTR_NODE.uptr = uptr;
    return U_UPTR_NODE.node;
}

// returns the TCPIP_HEAP_OBJ_INSTANCE associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_OBJ_INSTANCE* __attribute__((always_inline)) FC_TCPIP_HEAP_ObjInstance(TCPIP_STACK_HEAP_HANDLE heapH)
{
    union
    {
        TCPIP_STACK_HEAP_HANDLE heapH;
        TCPIP_HEAP_OBJ_INSTANCE* hInst;
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

// returns the TCPIP_HEAP_DCPT associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_DCPT* __attribute__((always_inline)) FS_TCPIP_HEAP_ObjDcpt(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_OBJ_INSTANCE* hInst = FC_TCPIP_HEAP_ObjInstance(heapH);

    return (hInst == NULL) ? NULL : &hInst->heapDcpt;
}

// API

// do the CacheInvalidate before calling specific pool/heap creation routines
// actually, for the pool, only the pool itself needs to be k1, not the pool entries!
TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateInternal(const TCPIP_STACK_HEAP_INTERNAL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_HEAP_DCPT* hDcpt;
    size_t          heapSize, heapUnits, headerSize;
    uint8_t*        allocatedHeapBuffer;
    size_t          heapBufferSize;
    uintptr_t       alignBuffer;
    uint8_t*        heapStart;
    TCPIP_STACK_HEAP_RES  res;
    
    union
    {
        uint8_t* alignHeapBuffer;
        const uint8_t* cBuffer;
        TCPIP_HEAP_OBJ_INSTANCE* hInst;
        TCPIP_STACK_HEAP_HANDLE h;
    }U_BUFF_INST;

    while(true)
    {
        hDcpt = NULL;
        U_BUFF_INST.hInst =  NULL;
        
        if( pHeapConfig == NULL || pHeapConfig->malloc_fnc == NULL || pHeapConfig->free_fnc == NULL || pHeapConfig->heapSize == 0U)
        {
            res = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }
        

        heapBufferSize = pHeapConfig->heapSize;
        allocatedHeapBuffer = (uint8_t*)(*pHeapConfig->malloc_fnc)(heapBufferSize);

        if(allocatedHeapBuffer == NULL)
        {
            res = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }


        // align properly: round up and truncate
        alignBuffer = ((uintptr_t)allocatedHeapBuffer + sizeof(S_iHeap_Align)-1U) & ~(sizeof(S_iHeap_Align)-1U);
        heapBufferSize -= (size_t)(alignBuffer - (uintptr_t)allocatedHeapBuffer);
        heapBufferSize &= ~(sizeof(S_iHeap_Align)-1U) ;
        U_BUFF_INST.alignHeapBuffer = (uint8_t*)alignBuffer;

        headerSize = ((sizeof(TCPIP_HEAP_OBJ_INSTANCE) + sizeof(u_iHeadNode) - 1U) / sizeof(u_iHeadNode)) * sizeof(u_iHeadNode);

        heapSize = heapBufferSize - headerSize;

        heapUnits = heapSize / sizeof(u_iHeadNode);           // adjust to multiple of heads

        if(heapUnits < M_TCPIP_HEAP_MIN_BLKS_)
        {
            res = TCPIP_STACK_HEAP_RES_BUFF_SIZE_ERR;
            break;
        }


        // check if mapping needed; always alloc uncached!
        // if((pHeapConfig->heapFlags & TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED) != 0) 
        {
            U_BUFF_INST.cBuffer = (const uint8_t*)F_TCPIP_HEAP_BufferMapNonCached(U_BUFF_INST.alignHeapBuffer, heapBufferSize);
        }
        heapStart = U_BUFF_INST.alignHeapBuffer + headerSize; 
        U_BUFF_INST.hInst->heapObj = tcpip_heap_object;
        hDcpt = &U_BUFF_INST.hInst->heapDcpt;
        hDcpt->heapHead = FC_Uptr2HeadNode(heapStart);
        hDcpt->heapHead->units = heapUnits;
        hDcpt->heapHead->next = NULL;
        hDcpt->heapTail = hDcpt->heapHead;
        hDcpt->heapUnits = heapUnits;
        hDcpt->heapAllocatedUnits = 0;
        hDcpt->heapWatermark = 0;
        hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_OK;
        hDcpt->heapFlags = pHeapConfig->heapFlags;
        hDcpt->allocatedBuffer = allocatedHeapBuffer;
        hDcpt->free_fnc = pHeapConfig->free_fnc;
#ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
        hDcpt->heapStart = hDcpt->heapHead;
        hDcpt->heapEnd = hDcpt->heapHead + heapUnits;
#endif
        if(OSAL_SEM_Create(&hDcpt->heapSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_SUCCESS)
        {
            (*pHeapConfig->free_fnc)(allocatedHeapBuffer);
            U_BUFF_INST.hInst = NULL;
            res = TCPIP_STACK_HEAP_RES_SYNCH_ERR;
            break;
        }

        res = TCPIP_STACK_HEAP_RES_OK;
        break;
    }

    if(pRes != NULL)
    {
        *pRes = res;
    }

    return U_BUFF_INST.h;
    
}

// internal functions
//
// deallocates the heap
// NOTE: check is done if some blocks are still in use!
static TCPIP_STACK_HEAP_RES FS_TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_OBJ_INSTANCE* hInst;
    TCPIP_HEAP_DCPT*   hDcpt;

    hInst = FC_TCPIP_HEAP_ObjInstance(heapH);

    if(hInst == NULL)
    {
        return TCPIP_STACK_HEAP_RES_NO_HEAP; 
    }

    hDcpt = &hInst->heapDcpt;
    
    if(hDcpt->heapHead != NULL)
    {
    #ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
        if(hDcpt->heapAllocatedUnits != 0U || hDcpt->heapHead != hDcpt->heapStart || hDcpt->heapHead->units != hDcpt->heapUnits)
    #else
        if(hDcpt->heapAllocatedUnits != 0U)
    #endif
        {
            //  deallocating a heap not completely de-allocated or corrupted
            return (hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_IN_USE); 
        }

        (void)OSAL_SEM_Delete(&hDcpt->heapSemaphore);
        // invalidate it
        (void)memset(&hInst->heapObj, 0, sizeof(hInst->heapObj));
        (*hDcpt->free_fnc)(hDcpt->allocatedBuffer);

        return TCPIP_STACK_HEAP_RES_OK;
    }
    
    return (hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_IN_USE); 
}


static void* FS_TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
    u_iHeadNode   *ptr,*prev;
    size_t      nunits;
    TCPIP_HEAP_DCPT*  hDcpt;


    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt == NULL || nBytes == 0U)
    {
        return NULL;
    }
    
    nunits=(nBytes + sizeof(u_iHeadNode) - 1U) / sizeof(u_iHeadNode) + 1U;    // allocate units   
    prev = NULL;

    (void)OSAL_SEM_Pend(&hDcpt->heapSemaphore, OSAL_WAIT_FOREVER);

    ptr = hDcpt->heapHead;
    while(ptr != NULL)
    {
        if(ptr->units >= nunits)
        {   // found block
            if(ptr->units-nunits <= M_TCPIP_HEAP_MIN_BLK_USIZE_)
            {
                nunits=ptr->units;  // get the whole block
            }

            if(ptr->units == nunits)
            {   // exact match
                if(prev != NULL)
                {
                    prev->next = ptr->next;
                }
                else
                {
                    hDcpt->heapHead = ptr->next;
                    prev = hDcpt->heapHead;
                }

                if(hDcpt->heapTail == ptr)
                {
                    hDcpt->heapTail = prev;
                }
            }
            else
            {   // larger than we need
                ptr->units -= nunits;
                ptr += ptr->units;
                ptr->units = nunits;
            }

            if((hDcpt->heapAllocatedUnits += nunits) > hDcpt->heapWatermark)
            {
                hDcpt->heapWatermark = hDcpt->heapAllocatedUnits;
            }
            (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
            return ptr + 1;
        }
        prev = ptr;
        ptr = ptr->next;
    }

    hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_NO_MEM;
    (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
    return NULL;
}

static void* FS_TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize)
{
    void* pBuff = FS_TCPIP_HEAP_Malloc(heapH, nElems * elemSize);
    if(pBuff != NULL)
    {
        (void)memset(pBuff, 0, nElems * elemSize);
    }

    return pBuff;

}

static size_t FS_TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff)
{  
    TCPIP_HEAP_DCPT*  hDcpt;
    u_iHeadNode   *hdr,*ptr;
    int         fail;
    size_t      freedUnits;

    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt == NULL || pBuff == NULL)
    {
        return 0;
    }

    ptr = FC_Cvptr2HeadNode(pBuff) - 1;

    (void)OSAL_SEM_Pend(&hDcpt->heapSemaphore, OSAL_WAIT_FOREVER);

#ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
    if((uint8_t*)ptr < (uint8_t*)hDcpt->heapStart || (uint8_t*)(ptr + ptr->units) > (uint8_t*)hDcpt->heapEnd)
    {
        hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_PTR_ERR;   // not one of our pointers!!!
        (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
        return 0;
    }
#endif
    
    freedUnits = ptr->units;
    
    fail = 0;
    
    if(hDcpt->heapHead == NULL)
    {
        ptr->next = NULL;
        hDcpt->heapHead = hDcpt->heapTail = ptr;
    }
    else if((uint8_t*)ptr < (uint8_t*)hDcpt->heapHead)
    {   // put it in front
        if(ptr+ptr->units == hDcpt->heapHead)
        {   // compact
            ptr->units += hDcpt->heapHead->units;
            ptr->next = hDcpt->heapHead->next;
            if(hDcpt->heapTail == hDcpt->heapHead)
            {
                hDcpt->heapTail = ptr;
            }
        }
        else
        {
            ptr->next = hDcpt->heapHead;
        }
        hDcpt->heapHead = ptr;    // new head
    }
    else if((uint8_t*)ptr > (uint8_t*)hDcpt->heapTail)
    {   // append tail
        if(hDcpt->heapTail + hDcpt->heapTail->units == ptr)
        {   // compact
            hDcpt->heapTail->units += ptr->units;
        }
        else
        {
            hDcpt->heapTail->next = ptr;
            ptr->next = NULL;
            hDcpt->heapTail = ptr;
        }
    }
    else
    {   // somewhere in the middle
        fail = 1;
        hdr = hDcpt->heapHead;
        while(hdr != NULL)
        {
            if((uint8_t*)hdr < (uint8_t*)ptr && (uint8_t*)ptr < (uint8_t*)hdr->next)
            {   // found a place
                if(ptr+ptr->units == hdr->next)
                {   // compact right
                    ptr->units += hdr->next->units;
                    ptr->next = hdr->next->next;
                    if(hDcpt->heapTail == hdr->next)
                    {
                        hDcpt->heapTail = ptr;
                    }
                }
                else
                {
                #ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
                    if((uint8_t*)(ptr + ptr->units) > (uint8_t*)hdr->next)
                    {
                        break;  // corrupted pointer!!!
                    }
                #endif
                    ptr->next = hdr->next;
                }
                
                // compact left
                if(hdr+hdr->units == ptr)
                {
                    hdr->units += ptr->units;
                    hdr->next = ptr->next;
                    if(hDcpt->heapTail == ptr)
                    {
                        hDcpt->heapTail = hdr;
                    }
                }
                else
                {
                #ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
                    if((uint8_t*)(hdr + hdr->units) > (uint8_t*)ptr)
                    {
                        break;      // corrupted pointer!!!
                    }
                #endif
                    hdr->next = ptr;
                }

                fail = 0;   // everything OK
                break;                
            }
            hdr = hdr->next;
        }
    }

    if(fail != 0)
    {
        hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_PTR_ERR;   // corrupted pointer!!!
        (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
        return 0;
    }
    
    hDcpt->heapAllocatedUnits -= freedUnits;
    (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
    return freedUnits * sizeof(u_iHeadNode);
}


static size_t FS_TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;

    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt != NULL)
    {
        return hDcpt->heapUnits * sizeof(u_iHeadNode);   
    }

    return 0;
}

static size_t FS_TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;

    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);


    if(hDcpt != NULL)
    {
        return (hDcpt->heapUnits - hDcpt->heapAllocatedUnits) * sizeof(u_iHeadNode);   
    }
    return 0;
}

static size_t FS_TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;

    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt != NULL)
    {
        return hDcpt->heapWatermark * sizeof(u_iHeadNode);
    }
    return 0;
}

static size_t FS_TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT   *hDcpt;
    u_iHeadNode   *ptr;
    size_t      max_nunits;

    max_nunits = 0;

    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);
    if(hDcpt != NULL)
    {
        (void)OSAL_SEM_Pend(&hDcpt->heapSemaphore, OSAL_WAIT_FOREVER);

        for(ptr = hDcpt->heapHead; ptr != NULL; ptr = ptr->next)
        {
            if(ptr->units >= max_nunits)
            {   // found block
                max_nunits = ptr->units;
            }
        }
        (void)OSAL_SEM_Post(&hDcpt->heapSemaphore);
    }

    return max_nunits * sizeof(u_iHeadNode);   

}


static TCPIP_STACK_HEAP_RES FS_TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;
    TCPIP_STACK_HEAP_RES  res;

    hDcpt = FS_TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt != NULL)
    {
        res = hDcpt->lastHeapErr;
        hDcpt->lastHeapErr = TCPIP_STACK_HEAP_RES_OK;
        return res;
    }

    return TCPIP_STACK_HEAP_RES_NO_HEAP;

}

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t FS_TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr)
{
    if(ptr != NULL)
    {
        const u_iHeadNode* hPtr = (const u_iHeadNode*)ptr -1;
        return hPtr->units * sizeof(u_iHeadNode);
    }

    return 0;
}
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


