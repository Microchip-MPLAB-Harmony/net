/*******************************************************************************
  TCPIP Pool Allocation Manager

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides the implementation for pool heap

*******************************************************************************/

/*
Copyright (C) 2016-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


#if defined(__mips__)
#include <sys/kmem.h>
#endif

#include "tcpip/src/tcpip_private.h"

// POOL debug levels
#define TCPIP_POOL_DEBUG_MASK_BASIC             (0x0001)
#define TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL      (0x0002)

// enable POOL debugging levels
#define TCPIP_POOL_DEBUG_LEVEL  (0x01)

#define M_TCPIP_POOL_DCPT_MAGIC_NO        0x706f6f6cU     // "pool" descriptor signature

// min heap alignment
// always power of 2
#if (CACHE_LINE_SIZE >= 8u)
typedef struct __attribute__((aligned(CACHE_LINE_SIZE)))
{
    uint64_t     pad[CACHE_LINE_SIZE / 8];
}S_TCPIP_POOL_ALIGN;
#elif (CACHE_LINE_SIZE >= 4u)
typedef uint32_t S_TCPIP_POOL_ALIGN;
#else
#error "TCP/IP Heap: incorrect CACHE_LINE_SIZE!"
#endif // (CACHE_LINE_SIZE >= 8u)

typedef union U_tag_TCPIP_POOL_DATA_BLK
{
    S_TCPIP_POOL_ALIGN x;
    struct  
    {
        union U_tag_TCPIP_POOL_DATA_BLK* next;
        // void*                         data[];                // pool data, user storage
    };
}TCPIP_POOL_DATA_BLK;   // a pool data chunk consists of linked blocks
                        // all of the same size
                        //

struct S_tag_TCPIP_POOL_DCPT;      // forward definition

typedef struct 
{
    struct S_tag_TCPIP_POOL_DCPT*    poolH;          // handle of the pool it belongs to
    TCPIP_POOL_DATA_BLK*            freeList;        // list of data blocks
    // pool entry specifics
    uint16_t                        blockSize;       // size of the individual block (multiple of unit size)
    uint16_t                        allocSize;       // allocated size of the block
    // delete pool helpers
    uint16_t                        nBlocks;         // how many blocks this entry contains
    uint16_t                        freeBlocks;      // know if some blocks in use; 
}TCPIP_POOL_ENTRY;          // pool entry descriptor
                            // Note: Must be 32 bit aligned!
                            // a pool entry consists of multiple blocks (TCPIP_POOL_DATA_BLK)
                            // each block shares its data into the freeList
                            // this is where the data is returned to the user from


typedef struct S_tag_TCPIP_POOL_DCPT
{
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
    uint32_t                poolMagic;              // valid pool identifier
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
    TCPIP_POOL_ENTRY*       poolStart;              // start of pool indicator
    TCPIP_POOL_ENTRY*       poolEnd;                // end of pool indicator
    void*                   allocatedBuffer;        // buffer initially allocated for this heap
    size_t                  allocatedSize;          // inital allocated buffer size
    size_t                  currAllocatedSize;      // current total allocated size
    size_t                  highWatermark;          // highest amount of allocated memory
    uint8_t*                expansionBuffer;        // run time expansion buffer 
    void                    (*free_fnc)(void* ptr); // free function needed to delete the heap
    uint16_t                nEntries;               // number of entries
    uint16_t                expansionSize;          // size of the expansion buffer
    int16_t                 lastPoolErr;            // TCPIP_STACK_HEAP_RES: last error encountered
    uint16_t                flags;                  // TCPIP_STACK_HEAP_FLAGS value
    OSAL_SEM_HANDLE_TYPE    poolSemaphore;          // synchronization object
                                                    // A semaphore per entry might be more efficient at run time
                                                    // However it's expensive.
    uint8_t                 entryExpBlks[1];        // nEntries array of # of blocks to expand when an entry runs out of mem
                                                    // 0 means no expansion
    //TCPIP_POOL_ENTRY      poolEntry[];            // pools themselves, different sizes
}TCPIP_POOL_DCPT;   // pool descriptor: collection of pool entries
                    // Note: Must be 32 bit aligned!

// Synchronization notes: there is no protection against multiple threads using and deleting this heap object!
// Special lock need to be put in place for something like this.
// Since this heap is under the use of the TCP/IP manager, a global lock mechanism at that level is more efficient.
// For now the user should call delete when all the threads have completed their job!


// the general picture
// to optimize the memory buffer usage because of fragmentation,
// the TCPIP_POOL_ENTRY entries are grouped together, followed by the data blocks themselves.
// This allows only one truncation, aligning just the data blocks to 16 bytes
//
// TCPIP_POOL_DCPT:
//        - poolMagic
//        - poolStart
//        - poolEnd
//        - allocatedBuffer
//        - allocatedSize
//        - expansionBuffer
//        - free_fnc
//        - nEntries
//        - expansionSize
//        - lastPoolErr
//        - flags
//        - poolSemaphore
//        - entryExpBlks[]
//        - poolEntry; starting the pool entries
//          
//          - TCPIP_POOL_ENTRY for blocks of specific size 1:
//                      - poolH
//                      - blockSize
//                      - freeList:  pointer to the data blocks for this entry
//                                   (data_blk1 -> data_blk2 -> ... ->data_blkm -> 0;)
//                      - nBlocks
//                      - freeBlocks
//                          
//          - TCPIP_POOL_ENTRY for blocks of specific size 2:
//                      - poolH
//                      - blockSize
//                      - freeList:  pointer to the data blocks for this entry
//                                   (data_blk1 -> data_blk2 -> ... ->data_blkm -> 0;)
//                      - nBlocks
//                      - freeBlocks
//                          
//          - ....
//
//                          
//          - TCPIP_POOL_ENTRY for blocks of specific size n:
//
//          - 16 bytes align gap
//              - TCPIP_POOL_DATA_BLK Data blocks for entry 1: data_blk1 -> data_blk2 -> ... ->data_blki -> 0;
//              - TCPIP_POOL_DATA_BLK Data blocks for entry 2: data_blk1 -> data_blk2 -> ... ->data_blkj -> 0;
//          - ....
//              - TCPIP_POOL_DATA_BLK Data blocks for entry n: data_blk1 -> data_blk2 -> ... ->data_blkm -> 0;

// prototypes
// 


// local data
//

static TCPIP_STACK_HEAP_RES   FS_TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);
static void*            FS_TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);
static void*            FS_TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
static size_t           FS_TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff);


// allows a more efficient allocation based on the size that needs to be allocated (for pool type heaps)
//#define TCPIP_STACK_HEAP_SIZE_ALLOCATION

#if defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)
typedef const void* TCPIP_STACK_HEAP_SIZE_HANDLE;
static TCPIP_STACK_HEAP_SIZE_HANDLE FS_TCPIP_HEAP_HandleBySize(TCPIP_STACK_HEAP_HANDLE heapH, size_t blkSize);
static void*            FS_TCPIP_HEAP_MallocBySize(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_STACK_HEAP_SIZE_HANDLE entryH, size_t nBytes);
static TCPIP_POOL_ENTRY* FS_PoolEntryHandle(TCPIP_POOL_DCPT* pPool, TCPIP_STACK_HEAP_SIZE_HANDLE entryH);
#endif  // defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)

static size_t           FS_TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           FS_TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           FS_TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           FS_TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH);
static TCPIP_STACK_HEAP_RES   FS_TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH);
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t           FS_TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff);
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
#if defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)
    &FS_TCPIP_HEAP_HandleBySize,
    &FS_TCPIP_HEAP_MallocBySize,
#endif  // defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)
};

typedef struct
{
    TCPIP_HEAP_OBJECT   heapObj;    // heap object API
    TCPIP_POOL_DCPT     poolDcpt;   // private pool heap object data
}TCPIP_POOL_OBJ_INSTANCE;


static int                  FS_PoolEntryCompare(const void* p1, const void* p2);
static TCPIP_POOL_ENTRY*    FS_PoolEntryBySize(TCPIP_POOL_DCPT* pPool, size_t blkSize);
static size_t               FS_PoolEntryAddBlocks(TCPIP_POOL_ENTRY* pEntry, uint8_t* blockBuffer, size_t buffSize, size_t newBlocks);
static void*                FS_PoolEntryAlloc(TCPIP_POOL_ENTRY* pEntry);



#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
static volatile int g_PoolExitAssertLoop = 0;
static void FS_PoolAssert(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("TCPIP_POOL Assert: %s, in line: %d, \r\n", message, lineNo);
        while(g_PoolExitAssertLoop == 0)
        {
            // do nothing
        }
    }
}
// a debug condition, not really assertion
static volatile int g_PoolExitCondLoop = 0;
static bool FS_PoolDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("TCPIP_POOL Cond: %s, in line: %d, \r\n", message, lineNo);
        while(g_PoolExitCondLoop == 0)
        {
            // do nothing
        }
        return false;
    }
    return true;
}

#else
static __inline__ bool __attribute__((always_inline)) FS_PoolAssert(bool cond, const char* message, int lineNo)
{
}
static __inline__ bool __attribute__((always_inline)) FS_PoolDbgCond(bool cond, const char* message, int lineNo)
{
    return true;
}
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)

#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL) != 0)
static TCPIP_POOL_OBJ_INSTANCE* g_poolTraceInst = NULL;
static TCPIP_POOL_ENTRY*        g_poolTraceEntries[6] = {0};

static void FS_PoolDbgTraceDisplay(void);

static void FS_PoolDbgTraceClear(void)
{
    g_poolTraceInst = NULL;
    (void)memset(g_poolTraceEntries, 0, sizeof(g_poolTraceEntries));
}

static void FS_PoolDbgTraceSet(TCPIP_POOL_OBJ_INSTANCE* hInst)
{
    TCPIP_POOL_ENTRY*   pEntry;
    TCPIP_POOL_DCPT*    pPool;
    size_t              poolIx;

    if((g_poolTraceInst = hInst) != NULL)
    {
        pPool = &g_poolTraceInst->poolDcpt;
        pEntry = pPool->poolStart;
        for(poolIx = 0; poolIx < sizeof(g_poolTraceEntries) / sizeof(*g_poolTraceEntries); poolIx++)
        {
            g_poolTraceEntries[poolIx] = pEntry;
            if((uint8_t*)pEntry >= (uint8_t*)pPool->poolEnd)
            {
                break;
            }
            pEntry++;
        }
    }
}

static void FS_PoolDbgTraceDisplay(void)
{
    TCPIP_POOL_DCPT*    pPool;
    TCPIP_POOL_ENTRY*   pEntry;
    int                 ix;
    uint32_t            totSize = 0;
    uint32_t            totAvlblSize = 0;

    pPool = &g_poolTraceInst->poolDcpt;
    
    if(pPool != NULL)
    {
        pEntry = pPool->poolStart;
        ix = 0;
        for(pEntry = pPool->poolStart; (uint8_t*)pEntry < (uint8_t*)pPool->poolEnd; pEntry++)
        {
            SYS_CONSOLE_PRINT("Pool Entry: %d, BlkSize: %4d, nBlks: %3d, FreeBlks: %d\r\n", ix, pEntry->blockSize, pEntry->nBlocks, pEntry->freeBlocks); 
            totSize += (uint32_t)pEntry->blockSize * (uint32_t)pEntry->nBlocks;
            totAvlblSize += (uint32_t)pEntry->blockSize * (uint32_t)pEntry->freeBlocks;
            ix++;
        }
    }
    SYS_CONSOLE_PRINT("Pool Tot Size: %d, Avlbl Size: %d\r\n", totSize, totAvlblSize); 

}
#else
static __inline__ void __attribute__((always_inline)) FS_PoolDbgTraceClear(void)
{
}

static __inline__ void __attribute__((always_inline)) FS_PoolDbgTraceSet(TCPIP_POOL_OBJ_INSTANCE* hInst)
{
}

static __inline__ void __attribute__((always_inline)) FS_PoolDbgTraceDisplay(void)
{
}

#endif  // ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL) != 0)

// conversion functions/helpers
//
static __inline__ TCPIP_POOL_DATA_BLK* __attribute__((always_inline)) FC_PoolEntry2PoolBlk(TCPIP_POOL_ENTRY* pEntry)
{
    union
    {
        TCPIP_POOL_ENTRY* pEntry;
        TCPIP_POOL_DATA_BLK* pBlk;
    }U_POOL_ENTRY_DATA_BLK;

    U_POOL_ENTRY_DATA_BLK.pEntry = pEntry;
    return U_POOL_ENTRY_DATA_BLK.pBlk;
}

static __inline__ TCPIP_POOL_ENTRY* __attribute__((always_inline)) FC_PoolBlk2PoolEntry(TCPIP_POOL_DATA_BLK* pBlk)
{
    union
    {
        TCPIP_POOL_DATA_BLK* pBlk;
        TCPIP_POOL_ENTRY* pEntry;
    }U_POOL_BLK_POOL_ENTRY;

    U_POOL_BLK_POOL_ENTRY.pBlk = pBlk;
    return U_POOL_BLK_POOL_ENTRY.pEntry;
}


static __inline__ TCPIP_POOL_OBJ_INSTANCE* __attribute__((always_inline)) FC_Uptr2PoolInst(uint8_t* uptr)
{
    union
    {
        uint8_t* uptr;
        TCPIP_POOL_OBJ_INSTANCE* hInst;
    }U_UPTR_POOL_INST;

    U_UPTR_POOL_INST.uptr = uptr;
    return U_UPTR_POOL_INST.hInst;
}

static __inline__ TCPIP_POOL_ENTRY* __attribute__((always_inline)) FC_Uptr2PoolEntry(uint8_t* uptr)
{
    union
    {
        uint8_t* uptr;
        TCPIP_POOL_ENTRY* pEntry;
    }U_UPTR_POOL_ENTRY;

    U_UPTR_POOL_ENTRY.uptr = uptr;
    return U_UPTR_POOL_ENTRY.pEntry;
}

static __inline__ TCPIP_POOL_DATA_BLK* __attribute__((always_inline)) FC_Cvptr2PoolBlk(const void* cvptr)
{
    union
    {
        const void* cvptr;
        TCPIP_POOL_DATA_BLK* pBlk;
    }U_CVPTR_POOL_BLK;

    U_CVPTR_POOL_BLK.cvptr = cvptr;
    return U_CVPTR_POOL_BLK.pBlk;
}

static __inline__ TCPIP_POOL_DATA_BLK* __attribute__((always_inline)) FC_Uptr2PoolBlk(uint8_t* uptr)
{
    union
    {
        uint8_t* uptr;
        TCPIP_POOL_DATA_BLK* pBlk;
    }U_UPTR_POOL_BLK;

    U_UPTR_POOL_BLK.uptr = uptr;
    return U_UPTR_POOL_BLK.pBlk;
}

static __inline__ ssize_t __attribute__((always_inline)) FC_Size_t2SSize_t(size_t uval)
{
    union
    {
        size_t uval;
        ssize_t sval;
    }U_SIZE_T_SSIZE_T;

    U_SIZE_T_SSIZE_T.uval = uval;
    return U_SIZE_T_SSIZE_T.sval;
}


// other helpers    

// gets a TCPIP_POOL_OBJ_INSTANCE from a heap handle
// does minimum sanity checking 
static __inline__ TCPIP_POOL_OBJ_INSTANCE* __attribute__((always_inline)) FS_PoolInstance(TCPIP_STACK_HEAP_HANDLE heapH)
{
    union
    {
        TCPIP_STACK_HEAP_HANDLE heapH;
        TCPIP_POOL_OBJ_INSTANCE* pInst;
    }U_HANDLE_INST;

    U_HANDLE_INST.heapH = heapH;
    TCPIP_POOL_OBJ_INSTANCE* pInst = U_HANDLE_INST.pInst;
    if(pInst != NULL)
    {
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        if(pInst->heapObj.F_TCPIP_HEAP_Delete == &FS_TCPIP_HEAP_Delete)
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        {
            return pInst;
        }
    }

    return NULL;
}


// converts a heap handle to a valid pool descriptor pointer
static  TCPIP_POOL_DCPT* FS_PoolDcptHandle(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT* pPool = NULL;
    TCPIP_POOL_OBJ_INSTANCE* pInst = FS_PoolInstance(heapH);
    if(pInst != NULL)
    {
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        if(pInst->poolDcpt.poolMagic == M_TCPIP_POOL_DCPT_MAGIC_NO)
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        {
            pPool = &pInst->poolDcpt;
        }
    }

    FS_PoolAssert(pPool != NULL, __func__, __LINE__);
    return pPool;
}

// delete a pool entry
// mark it as invalid
static __inline__ void FS_PoolEntryDelete(TCPIP_POOL_ENTRY* pEntry)
{
    (void)memset(pEntry, 0, sizeof(*pEntry));
}

// delete a pool descriptor
// mark it as invalid
static __inline__ void FS_PoolDcptDelete(TCPIP_POOL_OBJ_INSTANCE* pInst)
{
    TCPIP_POOL_DCPT* pPDcpt = &pInst->poolDcpt;

    void* allocatedBuffer = pPDcpt->allocatedBuffer;
    void (*free_fnc)(void* ptr) = pPDcpt->free_fnc;
    OSAL_SEM_HANDLE_TYPE poolSem = pPDcpt->poolSemaphore;

    (void)memset(pInst, 0, sizeof(*pInst));
    (*free_fnc)(allocatedBuffer);
    (void)OSAL_SEM_Delete(&poolSem);
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL) != 0)
    FS_PoolDbgTraceClear();
#endif  // ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL) != 0)
}

// get a buffer from a specified pool entry
static __inline__ void* __attribute__((always_inline)) FS_PoolEntryGetBuff(TCPIP_POOL_ENTRY* pEntry)
{
    TCPIP_POOL_DATA_BLK* pN;
    
    if(pEntry->freeList != NULL)
    {
        pN = pEntry->freeList;
        pEntry->freeList = pN->next;
        pEntry->freeBlocks--;

        pN->next = FC_PoolEntry2PoolBlk(pEntry); // store the entry we belong to!
        return pN + 1;
    }

    return NULL;
}

// return a previously allocated pool entry buffer
static __inline__ void __attribute__((always_inline)) FS_PoolEntryRetBuff(TCPIP_POOL_ENTRY* pEntry, TCPIP_POOL_DATA_BLK* pN)
{
    pN->next = pEntry->freeList;
    pEntry->freeList = pN;       // reinsert
    pEntry->freeBlocks++;
}

// aligns a buffer on a alignSize boundary
// alignSize must be multiple of 2!
// returns the adjusted size
static size_t   FS_PoolAlignBuffer(uint8_t** pBuffer, size_t buffSize, size_t alignSize)
{
    uint8_t* blockBuffer = *pBuffer;

    union
    {
        uint8_t*  u8Ptr;  
        uintptr_t alignBuffer;
    }U_INT_PTR;

    // align properly
    U_INT_PTR.u8Ptr = blockBuffer;
    U_INT_PTR.alignBuffer = (U_INT_PTR.alignBuffer + alignSize - 1U) & ~(alignSize - 1U);
    *pBuffer = U_INT_PTR.u8Ptr;

    buffSize -= U_INT_PTR.alignBuffer - (uintptr_t)blockBuffer; 

    return (buffSize & (~(alignSize - 1U)));
}

// local data
// 
/********************************
 * Interface Functions
*******************************************/ 

// creates a TCPIP memory pool
TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateInternalPool(const TCPIP_STACK_HEAP_POOL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_POOL_OBJ_INSTANCE* hInst;
    uint8_t            *heapBuffer, *expansionBuffer;
    size_t              entriesBuffSize, entriesSize, allocatedSize, headerSize, expansionSize;
    size_t              nEntries, nExpEntries;
    const TCPIP_STACK_HEAP_POOL_ENTRY   *pCreateEntries, *pPoolCreate;
    TCPIP_STACK_HEAP_RES      poolRes;
    void*               allocatedBuffer;
    TCPIP_POOL_DCPT*    pPool; // pool is an array of pool entries
    TCPIP_POOL_ENTRY*   pEntry;
    size_t              usedBlksSize;
    size_t              nEntriesBytes;
    size_t              nUnitsPerBlk;
    size_t              newBlocks;
    size_t              poolIx, poolJx;
    OSAL_SEM_HANDLE_TYPE poolSem;
    OSAL_RESULT         osalRes;
    bool                checkFail;
    
    while(true)
    {
        hInst = NULL;
        poolRes = TCPIP_STACK_HEAP_RES_OK;

        if( pHeapConfig == NULL || pHeapConfig->malloc_fnc == NULL || pHeapConfig->free_fnc == NULL || pHeapConfig->pEntries == NULL)
        {
            poolRes = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }
        
        nEntries = pHeapConfig->nPoolEntries;
        pCreateEntries = pHeapConfig->pEntries; 

        // check for 0 or duplicate entries
        pPoolCreate = pCreateEntries;
        checkFail =  false;
        for(poolIx = 0; poolIx < nEntries && checkFail == false; poolIx++)
        {
            if(pPoolCreate->entrySize == 0U)
            {   // invalid entry size; abort
                checkFail =  true;
                break;
            }
            // check no other entry with the same size
            for(poolJx = poolIx + 1U; poolJx < nEntries; poolJx++)
            {
                if(pPoolCreate->entrySize == (pHeapConfig->pEntries + poolJx)->entrySize)
                {   // duplicate
                    checkFail =  true;
                    break;
                }
            }
            pPoolCreate++;
        }

        if(checkFail == true)
        {
            poolRes = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }



        entriesSize = 0;
        pPoolCreate = pCreateEntries;
        for(poolIx = 0; poolIx < nEntries; poolIx++)
        {
            entriesSize += (size_t)pPoolCreate->entrySize * (size_t)pPoolCreate->nBlocks + sizeof(TCPIP_POOL_ENTRY) + (size_t)pPoolCreate->nBlocks * sizeof(TCPIP_POOL_DATA_BLK);
            pPoolCreate++;
        }

        // check how much space is needed for the entries increment sizes: entryExpBlks
        expansionSize = pHeapConfig->expansionHeapSize;
        nExpEntries = expansionSize == 0U ? 0U : nEntries;

        // align properly
        entriesSize = ((entriesSize + sizeof(uintptr_t) - 1U) / sizeof(uintptr_t)) * sizeof(uintptr_t);
        headerSize = ((sizeof(*hInst) + sizeof(*hInst->poolDcpt.entryExpBlks) * nExpEntries + sizeof(uintptr_t) - 1U) / sizeof(uintptr_t)) * sizeof(uintptr_t);
        expansionSize = ((expansionSize + sizeof(uintptr_t) - 1U) / sizeof(uintptr_t)) * sizeof(uintptr_t);
        
        allocatedSize = entriesSize + headerSize + expansionSize;  // extra room for header

        // allocate the buffer to create the heap into
        if((allocatedBuffer = (*pHeapConfig->malloc_fnc)(allocatedSize)) == NULL)
        {
            poolRes = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }

        heapBuffer = (uint8_t*)allocatedBuffer;
        entriesBuffSize = allocatedSize - expansionSize;    // remove what we don't need now
        // align properly on a 32 bit boundary
        entriesBuffSize = FS_PoolAlignBuffer(&heapBuffer, entriesBuffSize, sizeof(uintptr_t));

        // check for enough room
        entriesBuffSize -= headerSize;
        nEntriesBytes = nEntries * sizeof(TCPIP_POOL_ENTRY);

        osalRes = OSAL_SEM_Create(&poolSem, OSAL_SEM_TYPE_BINARY, 1, 1);
        if(osalRes != OSAL_RESULT_SUCCESS || entriesBuffSize < nEntriesBytes)
        {   // semaphore creation failed or not enough memory; abort
            (*pHeapConfig->free_fnc)(allocatedBuffer);
            poolRes = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }

        // create and initialize a proper heap object
        hInst = FC_Uptr2PoolInst(heapBuffer);
        hInst->heapObj = tcpip_heap_object;

        heapBuffer += headerSize;
        // this op should render a properly 32 bit aligned buffer!
        FS_PoolAssert(((uintptr_t)heapBuffer & (sizeof(uintptr_t) - 1U)) == 0U, __func__, __LINE__);

        // init each entry
        pPool = &hInst->poolDcpt;
        pPoolCreate = pCreateEntries;

        pEntry = FC_Uptr2PoolEntry(heapBuffer);
        pPool->poolStart = pEntry;
        for(poolIx = 0; poolIx < nEntries; poolIx++)
        {
            (void)memset(pEntry, 0, sizeof(*pEntry));  // clear the entry
            // round up: room for extra next pointer in the TCPIP_POOL_DATA_BLK
            nUnitsPerBlk = (pPoolCreate->entrySize + sizeof(TCPIP_POOL_DATA_BLK) - 1U) / sizeof(TCPIP_POOL_DATA_BLK) + 1U;
            pEntry->allocSize = (uint16_t)(nUnitsPerBlk * sizeof(TCPIP_POOL_DATA_BLK));
            pEntry->blockSize = pEntry->allocSize - (uint16_t)sizeof(TCPIP_POOL_DATA_BLK); // room for the next pointer
            pEntry->nBlocks = pPoolCreate->nBlocks;
            pEntry->poolH = pPool;
            pPoolCreate++;
            pEntry++;
        }

        pPool->nEntries = (uint16_t)nEntries;
        // always alloc uncached!
        pPool->flags = (uint16_t)pHeapConfig->heapFlags | (uint16_t)TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED;
        pPool->lastPoolErr = (int16_t)TCPIP_STACK_HEAP_RES_OK;
        pPool->poolEnd = pEntry;
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        pPool->poolMagic = M_TCPIP_POOL_DCPT_MAGIC_NO;        
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        pPool->allocatedBuffer = allocatedBuffer;
        pPool->allocatedSize = allocatedSize;
        pPool->currAllocatedSize = 0;
        pPool->highWatermark = 0;
        pPool->free_fnc = pHeapConfig->free_fnc;
        pPool->poolSemaphore = poolSem; 

        if(expansionSize != 0U)
        {
            expansionBuffer = (uint8_t*)allocatedBuffer + allocatedSize - expansionSize; 
            // align properly on a 32 bit boundary
            expansionSize = FS_PoolAlignBuffer(&expansionBuffer, expansionSize, sizeof(uintptr_t));
        }
        else
        {
            expansionBuffer = NULL;
        }
        pPool->expansionBuffer = expansionBuffer;
        pPool->expansionSize = (uint16_t)expansionSize;


        // and sort it ascending
        FC_Sort(pPool->poolStart, nEntries, sizeof(TCPIP_POOL_ENTRY), &FS_PoolEntryCompare);

        // store the expansion sizes
        // Note: because of FC_Sort, the order has changed!
        if(pPool->expansionSize != 0U)
        { 
            for(pEntry = pPool->poolStart; pEntry != pPool->poolEnd; pEntry++)
            {
                pPoolCreate = pCreateEntries;
                for(poolIx = 0; poolIx < nEntries; poolIx++)
                {
                    nUnitsPerBlk = (pPoolCreate->entrySize + sizeof(TCPIP_POOL_DATA_BLK) - 1U) / sizeof(TCPIP_POOL_DATA_BLK) + 1U;
                    if(pEntry->allocSize ==  nUnitsPerBlk * sizeof(TCPIP_POOL_DATA_BLK))
                    {   // found entry
                        pPool->entryExpBlks[pEntry - pPool->poolStart] = pPoolCreate->nExpBlks;
                        break;
                    }
                    pPoolCreate++;
                }
            }
        }

        // create the data blocks for each pool entry 
        entriesBuffSize -= nEntriesBytes;
        heapBuffer = (uint8_t*)pPool->poolEnd;

        // allocate the largest blocks first
        for(pEntry = pPool->poolEnd - 1; pEntry >= pPool->poolStart; pEntry--)
        {
            if(pEntry == pPool->poolStart && pPool->expansionSize == 0U && pEntry->nBlocks != 0U)
            {   // for the last allocation - the smallest - we take all the available space 
                // but only if run-time expansion is not enabled
                pEntry->nBlocks = 0xffff;
            }

            if(pEntry->nBlocks != 0U)
            {   // add these blocks 
                newBlocks = pEntry->nBlocks;
                pEntry->nBlocks = 0U;
                if((usedBlksSize = FS_PoolEntryAddBlocks(pEntry, heapBuffer, entriesBuffSize, newBlocks)) == 0U)
                {   // no more room
                    break;
                }
                heapBuffer += usedBlksSize;
                entriesBuffSize -= usedBlksSize;
            }
        }

        poolRes = TCPIP_STACK_HEAP_RES_OK;
        FS_PoolDbgTraceSet(hInst);
        break;
    }

    if(pRes != NULL)
    {
        *pRes = poolRes;
    }

    return hInst;
}

static size_t FS_PoolEntryAddBlocks(TCPIP_POOL_ENTRY* pEntry, uint8_t* blockBuffer, size_t buffSize, size_t newBlocks)
{
    size_t      ix, nBlocks;
    size_t      takenSize;
    uint8_t*    alignBuffer;
    TCPIP_POOL_DCPT* pPool = pEntry->poolH;
    TCPIP_POOL_DATA_BLK* pDataBlk, *pBlk, *pNextBlk = NULL;

    // make sure it's 16 bits
    FS_PoolAssert(newBlocks == (size_t)((uint16_t)newBlocks), __func__, __LINE__);

    // align the data blocks
    alignBuffer = blockBuffer;
    buffSize = FS_PoolAlignBuffer(&alignBuffer, buffSize, sizeof(TCPIP_POOL_DATA_BLK));

    // configure the data blocks
    nBlocks = buffSize / pEntry->allocSize;
    if(nBlocks > newBlocks)
    {
        nBlocks = newBlocks;
    }

    if(nBlocks != 0U)
    {
        takenSize = nBlocks * pEntry->allocSize;
        if((pPool->flags & (uint16_t)TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED) != 0U) 
        {

            const void* cvptr = F_TCPIP_HEAP_BufferMapNonCached(alignBuffer, takenSize);
            pDataBlk = FC_Cvptr2PoolBlk(cvptr);
        }
        else
        {
            pDataBlk = FC_Uptr2PoolBlk(alignBuffer);
        }

        // link the data blocks
        pBlk = pDataBlk;
        for(ix = 0; ix < nBlocks - 1U; ix++)
        {
            pNextBlk = FC_Uptr2PoolBlk((uint8_t*)pBlk + pEntry->allocSize); 
            pBlk->next = pNextBlk;
            pBlk = pNextBlk;
        }

        // link the data blocks
        pBlk->next = pEntry->freeList;
        pEntry->freeList = pDataBlk;

        // adjust for buffer alignment
        takenSize += (uintptr_t)alignBuffer - (uintptr_t)blockBuffer;
    }
    else
    {
        takenSize = 0U;
    }

    // should not exceed uint16_t representation
    FS_PoolAssert((size_t)pEntry->nBlocks + nBlocks < 0x10000U, __func__, __LINE__);
    FS_PoolAssert((size_t)pEntry->freeBlocks + nBlocks < 0x10000U, __func__, __LINE__);
    pEntry->nBlocks += (uint16_t)nBlocks;    
    pEntry->freeBlocks += (uint16_t)nBlocks;    

    return takenSize;
}

// simple binary search for the right slot
static TCPIP_POOL_ENTRY* FS_PoolEntryBySize(TCPIP_POOL_DCPT* pPool, size_t blkSize)
{
// binary search

    ssize_t len, half;
    TCPIP_POOL_ENTRY  *pFirstEntry, *pMidEntry;

    len = FC_Size_t2SSize_t((size_t)pPool->nEntries - 1U);

    pFirstEntry = pPool->poolStart;
    
    while(len > 0)
    {
        half = len / 2;
        pMidEntry = pFirstEntry + half;

        if((size_t)pMidEntry->blockSize < blkSize)
        {
            pFirstEntry = pMidEntry + 1;
            len -= half + 1;
        }
        else
        {
            len = half;
        }
    }
    
    return (size_t)pFirstEntry->blockSize >= blkSize ? pFirstEntry : NULL;

}

////////////////////////////
//  Low level routines
//  
// sorting function
static int FS_PoolEntryCompare(const void* p1, const void* p2)
{
    const TCPIP_POOL_ENTRY* pE1 = (const TCPIP_POOL_ENTRY*)p1;
    const TCPIP_POOL_ENTRY* pE2 = (const TCPIP_POOL_ENTRY*)p2;
   

    if(pE1->blockSize < pE2->blockSize)
    {
        return -1;
    }
    else if(pE1->blockSize > pE2->blockSize)
    {
        return 1;
    }
    else
    {
        return 0;    
    }
}


//////////////////////////////////////////
//  Heap object routines
//

// destroys a pool
static TCPIP_STACK_HEAP_RES   FS_TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;
    TCPIP_POOL_ENTRY*    pEntry;
    TCPIP_STACK_HEAP_RES res = TCPIP_STACK_HEAP_RES_NO_HEAP;

    pPool = FS_PoolDcptHandle(heapH);
    
    while(pPool != NULL)
    {
        if(OSAL_SEM_Pend(&pPool->poolSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            res = TCPIP_STACK_HEAP_RES_SYNCH_ERR;
            break;
        }
        res = TCPIP_STACK_HEAP_RES_OK;
        for(pEntry = pPool->poolStart; pEntry < pPool->poolEnd; pEntry++)
        {   // check all entries are released
            if(pEntry->freeBlocks != pEntry->nBlocks)
            {
                pPool->lastPoolErr = (int16_t)TCPIP_STACK_HEAP_RES_IN_USE;
                res = TCPIP_STACK_HEAP_RES_IN_USE;
                break;
            }
        }
        if(res != TCPIP_STACK_HEAP_RES_OK)
        {
            (void)OSAL_SEM_Post(&pPool->poolSemaphore);
            break;
        }

        // OK
        for(pEntry = pPool->poolStart; pEntry < pPool->poolEnd; pEntry++)
        {
            FS_PoolEntryDelete(pEntry);
        }
        
        
        FS_PoolDcptDelete(FS_PoolInstance(heapH));

        break;
    }

    return res;
}

// get a pool buffer
static void* FS_TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
    TCPIP_POOL_DCPT*    pPool;
    TCPIP_POOL_ENTRY*   pEntry;
    void*               pBuff = NULL;
    
    while(nBytes != 0U)
    {
        pPool = FS_PoolDcptHandle(heapH);
        if(pPool == NULL)
        {
            break;
        }

        pEntry = FS_PoolEntryBySize(pPool, nBytes);
        if(pEntry == NULL)
        {
            pPool->lastPoolErr = (int16_t)TCPIP_STACK_HEAP_RES_SIZE_ERR; // cannot satisfy the request
            break;
        }

        pBuff = FS_PoolEntryAlloc(pEntry);

        break;
    }

    return pBuff;
}

static void* FS_PoolEntryAlloc(TCPIP_POOL_ENTRY* pEntry)
{
    void*               pBuff;
    size_t              newBlocks, usedBlksSize;
    TCPIP_POOL_DCPT*    pPool = pEntry->poolH;

    if(OSAL_SEM_Pend(&pPool->poolSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
    {
        return NULL;
    }

    pBuff = NULL;

    while(true)
    {
        if((pBuff = FS_PoolEntryGetBuff(pEntry)) != NULL)
        {   // got it
            break;
        }

        if(pPool->expansionSize != 0U)
        {   // there is an expansion buffer
            if((newBlocks = (size_t)pPool->entryExpBlks[pEntry - pPool->poolStart]) != 0U)
            {   // we could expand this entry
                if((usedBlksSize = FS_PoolEntryAddBlocks(pEntry, pPool->expansionBuffer, pPool->expansionSize, newBlocks)) != 0U)
                {   // managed to get room to grow
                    pPool->expansionBuffer += usedBlksSize;
                    pPool->expansionSize = (uint16_t)((size_t)pPool->expansionSize - usedBlksSize);
                    pBuff = FS_PoolEntryGetBuff(pEntry);  // this should not fail now
                    break;
                }
            }
        }

        // no luck so far; try moving up
        if((pPool->flags & (uint16_t)TCPIP_STACK_HEAP_FLAG_POOL_STRICT) == 0U)
        {   // we can try other entries
            for(pEntry = pEntry + 1; pEntry < pPool->poolEnd; pEntry++)
            {
                if((pBuff = FS_PoolEntryGetBuff(pEntry)) != NULL)
                {
                    break;
                }
            }
        }

        break;
    }

    if(pBuff == NULL)
    {
        pPool->lastPoolErr = (int16_t)TCPIP_STACK_HEAP_RES_NO_MEM; 
    }
    else
    {
        if((pPool->currAllocatedSize += pEntry->blockSize) > pPool->highWatermark)
        {
            pPool->highWatermark = pPool->currAllocatedSize;
        }
    }

    (void)OSAL_SEM_Post(&pPool->poolSemaphore);
    return pBuff;
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



// return a previously allocated pool entry buffer
static size_t FS_TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff)
{

    size_t blockSize = 0;

    while(pBuff != NULL)
    {
        TCPIP_POOL_DCPT* pPool = FS_PoolDcptHandle(heapH);
        if(pPool != NULL)
        {
            TCPIP_POOL_DATA_BLK*  pN = FC_Cvptr2PoolBlk(pBuff) - 1;
            TCPIP_POOL_ENTRY* pEntry = FC_PoolBlk2PoolEntry(pN->next); // get the pool entry pointer 

            if(OSAL_SEM_Pend(&pPool->poolSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
            {
                break;
            }
            if(FS_PoolDbgCond(pEntry != NULL && pEntry->poolH == pPool, __func__, __LINE__))
            {
                FS_PoolEntryRetBuff(pEntry, pN);
                blockSize = pEntry->blockSize;
                pPool->currAllocatedSize -= blockSize;
            }
            else
            {
                pPool->lastPoolErr = (int16_t)TCPIP_STACK_HEAP_RES_PTR_ERR; 
            }
            (void)OSAL_SEM_Post(&pPool->poolSemaphore);
        }
        break;
    }

    return blockSize;
}


static size_t FS_TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;

    pPool = FS_PoolDcptHandle(heapH);
    
    if(pPool != NULL)
    {
        return pPool->allocatedSize;
    }

    return 0;
}

static size_t FS_TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;
    TCPIP_POOL_ENTRY*    pEntry;

    pPool = FS_PoolDcptHandle(heapH);
    
    if(pPool != NULL)
    {
        for(pEntry = pPool->poolEnd - 1; pEntry >= pPool->poolStart; pEntry--)
        {
            if(pEntry->freeBlocks != 0U)
            {   // return the largest available block
                return pEntry->blockSize;
            }
        }
    }

    return 0;
}

static size_t FS_TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;
    TCPIP_POOL_ENTRY*    pEntry;
    size_t              totPoolSize = 0;

    pPool = FS_PoolDcptHandle(heapH);
    
    if(pPool != NULL)
    {
        for(pEntry = pPool->poolStart; pEntry < pPool->poolEnd; pEntry++)
        {
            if(pEntry->freeBlocks != 0U)
            {
                totPoolSize += (size_t)pEntry->freeBlocks * (size_t)pEntry->blockSize;
            }
        }
        totPoolSize += (size_t)pPool->expansionSize;
    }

    return totPoolSize;
}

static size_t FS_TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT* pPool = FS_PoolDcptHandle(heapH);
    
    return pPool != NULL ? pPool->highWatermark : 0U;
}

static TCPIP_STACK_HEAP_RES FS_TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_STACK_HEAP_RES  res = TCPIP_STACK_HEAP_RES_NO_HEAP;
    TCPIP_POOL_DCPT* pPool = FS_PoolDcptHandle(heapH);

    if(pPool != NULL)
    {
        res = (TCPIP_STACK_HEAP_RES)pPool->lastPoolErr;
        pPool->lastPoolErr = (int16_t)TCPIP_STACK_HEAP_RES_OK;
    }

    return res;
}

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t FS_TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff)
{

    if(pBuff != NULL)
    {
        TCPIP_POOL_DCPT* pPool = FS_PoolDcptHandle(heapH);
        if(pPool != NULL)
        {
            TCPIP_POOL_DATA_BLK*  pN = FC_Cvptr2PoolBlk(pBuff) - 1;

            TCPIP_POOL_ENTRY* pEntry = FC_PoolBlk2PoolEntry(pN->next); // get the pool entry pointer 

            if(FS_PoolDbgCond(pEntry != NULL && pEntry->poolH == pPool, __func__, __LINE__))
            {
                return pEntry->blockSize;
            }
        }
    }

    return 0;
}
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

#if defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)
// gets a handle for a pool entry using size
static TCPIP_STACK_HEAP_SIZE_HANDLE FS_TCPIP_HEAP_HandleBySize(TCPIP_STACK_HEAP_HANDLE heapH, size_t blkSize)
{
    TCPIP_POOL_DCPT* pPool;
    TCPIP_POOL_ENTRY* pEntry = NULL;

    if(blkSize != 0)
    {
        pPool = FS_PoolDcptHandle(heapH);
        if(pPool != NULL)
        {
            pEntry = FS_PoolEntryBySize(pPool, blkSize);

            if(pEntry == NULL)
            {
                pPool->lastPoolErr = (int16_t)TCPIP_STACK_HEAP_RES_SIZE_ERR;
            }
        }
    }

    return pEntry;
}

// get a buffer from a specified pool entry
// nBytes is not really needed...but needed for the other heap types that don't really support entries....!
static void* FS_TCPIP_HEAP_MallocBySize(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_STACK_HEAP_SIZE_HANDLE entryH, size_t nBytes)
{
    TCPIP_POOL_DCPT* pPool;
    TCPIP_POOL_ENTRY* pEntry;
    void*       pBuff = NULL;

    if(nBytes != 0)
    {
        if((pPool = FS_PoolDcptHandle(heapH)) != NULL)
        {
            if((pEntry = FS_PoolEntryHandle(pPool, entryH)) != NULL) 
            {
                if(nBytes <= pEntry->blockSize)
                {
                    pBuff = FS_PoolEntryAlloc(pEntry);
                }
            }
        }
    }

    return pBuff;
}
// converts a entry handle to a valid pool Entry pointer
static TCPIP_POOL_ENTRY* FS_PoolEntryHandle(TCPIP_POOL_DCPT* pPool, TCPIP_STACK_HEAP_SIZE_HANDLE entryH)
{
    TCPIP_POOL_ENTRY* pEntry = (TCPIP_POOL_ENTRY*)entryH;
    if(pEntry != NULL)
    {
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        if(pPool->poolStart <= pEntry && pEntry < pPool->poolEnd)
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        {   // check that the pool it belongs to is valid
            return pEntry;
        }
    }

    FS_PoolAssert(false, __func__, __LINE__);
    return 0;
}



#endif  // defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)


uint16_t TCPIP_HEAP_POOL_Entries(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT* pPool = FS_PoolDcptHandle(heapH);
    if(pPool != NULL)
    {
        return pPool->nEntries;
    }

    return 0U;
}


bool TCPIP_HEAP_POOL_EntryList(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_POOL_ENTRY_LIST* pList)
{
    TCPIP_POOL_DCPT* pPool = FS_PoolDcptHandle(heapH);
    if(pPool != NULL && entryIx < (size_t)pPool->nEntries)
    {
        if(pList != NULL)
        {
            TCPIP_POOL_ENTRY* pEntry = pPool->poolStart + entryIx;
            pList->blockSize = pEntry->blockSize;
            pList->nBlocks = pEntry->nBlocks;
            pList->freeBlocks = pEntry->freeBlocks;
            pList->totEntrySize = (size_t)pEntry->blockSize * (size_t)pEntry->nBlocks;
            pList->totFreeSize = (size_t)pEntry->blockSize * (size_t)pEntry->freeBlocks;
            pList->expansionSize = pPool->expansionSize;
        }
        return true;
    }

    return false;
}


