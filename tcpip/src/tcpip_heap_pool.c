/*******************************************************************************
  TCPIP Pool Allocation Manager

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides the implementation for pool heap

*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

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


#if defined(__mips__)
#include <sys/kmem.h>
#endif

#include "tcpip/src/tcpip_private.h"

// POOL debug levels
#define TCPIP_POOL_DEBUG_MASK_BASIC             (0x0001)
#define TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL      (0x0002)

// enable POOL debugging levels
#define TCPIP_POOL_DEBUG_LEVEL  0

#define _TCPIP_POOL_DCPT_MAGIC_NO        0x706f6f6c     // "pool" descriptor signature

// min heap alignment
// always power of 2
#if (CACHE_LINE_SIZE >= 8u)
typedef struct __attribute__((aligned(CACHE_LINE_SIZE)))
{
    uint64_t     pad[CACHE_LINE_SIZE / 8];
}_TCPIP_POOL_ALIGN;
#elif (CACHE_LINE_SIZE >= 4u)
typedef uint32_t _TCPIP_POOL_ALIGN;
#else
#error "TCP/IP Heap: incorrect CACHE_LINE_SIZE!"
#endif // (CACHE_LINE_SIZE >= 8u)

typedef union _tag_TCPIP_POOL_DATA_BLK
{
    _TCPIP_POOL_ALIGN x;
    struct  
    {
        union _tag_TCPIP_POOL_DATA_BLK* next;
        void*                           data[];                // pool data, user storage
    };
}TCPIP_POOL_DATA_BLK;   // a pool data chunk consists of linked blocks
                        // all of the same size
                        //

struct _tag_TCPIP_POOL_DCPT;      // forward definition

typedef struct 
{
    struct _tag_TCPIP_POOL_DCPT*    poolH;          // handle of the pool it belongs to
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


typedef struct _tag_TCPIP_POOL_DCPT
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
    uint8_t                 entryExpBlks[];         // nEntries array of # of blocks to expand when an entry runs out of mem
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

static TCPIP_STACK_HEAP_RES   _TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);
static void*            _TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);
static void*            _TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
static size_t           _TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff);


// allows a more efficient allocation based on the size that needs to be allocated (for pool type heaps)
//#define TCPIP_STACK_HEAP_SIZE_ALLOCATION

#if defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)
typedef const void* TCPIP_STACK_HEAP_SIZE_HANDLE;
static TCPIP_STACK_HEAP_SIZE_HANDLE _TCPIP_HEAP_HandleBySize(TCPIP_STACK_HEAP_HANDLE heapH, int blkSize);
static void*            _TCPIP_HEAP_MallocBySize(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_STACK_HEAP_SIZE_HANDLE entryH, size_t nBytes);
static TCPIP_POOL_ENTRY* _PoolEntryHandle(TCPIP_POOL_DCPT* pPool, TCPIP_STACK_HEAP_SIZE_HANDLE entryH);
#endif  // defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)

static size_t           _TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH);
static TCPIP_STACK_HEAP_RES   _TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH);
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t           _TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

// maps a buffer to non cached memory
const void* _TCPIP_HEAP_BufferMapNonCached(const void* buffer, size_t buffSize);
const void* _TCPIP_HEAP_PointerMapCached(const void* ptr);


// the heap object
static const TCPIP_HEAP_OBJECT      _tcpip_heap_object = 
{
    .TCPIP_HEAP_Delete = _TCPIP_HEAP_Delete,
    .TCPIP_HEAP_Malloc = _TCPIP_HEAP_Malloc,
    .TCPIP_HEAP_Calloc = _TCPIP_HEAP_Calloc,
    .TCPIP_HEAP_Free = _TCPIP_HEAP_Free,
    .TCPIP_HEAP_Size = _TCPIP_HEAP_Size,
    .TCPIP_HEAP_MaxSize = _TCPIP_HEAP_MaxSize,
    .TCPIP_HEAP_FreeSize = _TCPIP_HEAP_FreeSize,
    .TCPIP_HEAP_HighWatermark = _TCPIP_HEAP_HighWatermark,
    .TCPIP_HEAP_LastError = _TCPIP_HEAP_LastError,
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    .TCPIP_HEAP_AllocSize = _TCPIP_HEAP_AllocSize,
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
#if defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)
    _TCPIP_HEAP_HandleBySize,
    _TCPIP_HEAP_MallocBySize,
#endif  // defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)
};

typedef struct
{
    TCPIP_HEAP_OBJECT   heapObj;    // heap object API
    TCPIP_POOL_DCPT     poolDcpt;   // private pool heap object data
}TCPIP_HEAP_OBJ_INSTANCE;


static int                  _PoolEntryCompare(const void* p1, const void* p2);
static TCPIP_POOL_ENTRY*    _PoolEntryBySize(TCPIP_POOL_DCPT* pPool, int blkSize);
static size_t               _PoolEntryAddBlocks(TCPIP_POOL_ENTRY* pEntry, uint8_t* blockBuffer, size_t buffSize, int newBlocks);
static void*                _PoolEntryAlloc(TCPIP_POOL_ENTRY* pEntry);



#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
volatile int _PoolExitAssertLoop = 0;
static bool _PoolAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("TCPIP_POOL Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_PoolExitAssertLoop == 0);
        return false;
    }

    return true;
}
// a debug condition, not really assertion
volatile int _PoolExitCondLoop = 0;
static bool _PoolDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("TCPIP_POOL Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_PoolExitCondLoop == 0);
        return false;
    }
    return true;
}

#else
static __inline__ bool __attribute__((always_inline)) _PoolAssertCond(bool cond, const char* message, int lineNo)
{
    return true;
}
static __inline__ bool __attribute__((always_inline)) _PoolDbgCond(bool cond, const char* message, int lineNo)
{
    return true;
}
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)

#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL) != 0)
TCPIP_HEAP_OBJ_INSTANCE* _poolTraceInst = 0;
TCPIP_POOL_ENTRY*        _poolTraceEntries[6] = {0};

static void _PoolDbgTraceClear(void)
{
    _poolTraceInst = 0;
    memset(_poolTraceEntries, 0, sizeof(_poolTraceEntries));
}

static void _PoolDbgTraceSet(TCPIP_HEAP_OBJ_INSTANCE* hInst)
{
    TCPIP_POOL_ENTRY*   pEntry;
    TCPIP_POOL_DCPT*    pPool;
    int                 poolIx;

    if((_poolTraceInst = hInst) != 0)
    {
        pPool = &_poolTraceInst->poolDcpt;
        for(pEntry = pPool->poolStart, poolIx = 0; pEntry < pPool->poolEnd && poolIx < sizeof(_poolTraceEntries) / sizeof(*_poolTraceEntries); pEntry++, poolIx++)
        {
            _poolTraceEntries[poolIx] = pEntry;
        }
    }
}

void _PoolDbgTraceDisplay(void)
{
    TCPIP_POOL_DCPT*    pPool;
    TCPIP_POOL_ENTRY*   pEntry;
    int                 ix;
    uint32_t            totSize = 0;
    uint32_t            totAvlblSize = 0;

    pPool = &_poolTraceInst->poolDcpt;
    
    if(pPool)
    {
        for(ix = 0, pEntry = pPool->poolStart; pEntry < pPool->poolEnd; pEntry++, ix++)
        {
            SYS_CONSOLE_PRINT("Pool Entry: %d, BlkSize: %4d, nBlks: %3d, FreeBlks: %d\r\n", ix, pEntry->blockSize, pEntry->nBlocks, pEntry->freeBlocks); 
            totSize += pEntry->blockSize * pEntry->nBlocks;
            totAvlblSize += pEntry->blockSize * pEntry->freeBlocks;
        }
    }
    SYS_CONSOLE_PRINT("Pool Tot Size: %d, Avlbl Size: %d\r\n", totSize, totAvlblSize); 

}
#else
static __inline__ void __attribute__((always_inline)) _PoolDbgTraceClear(void)
{
}

static __inline__ void __attribute__((always_inline)) _PoolDbgTraceSet(TCPIP_HEAP_OBJ_INSTANCE* hInst)
{
}

void _PoolDbgTraceDisplay(void)
{
}

#endif  // ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_TRACE_GLOBAL) != 0)


// gets a TCPIP_HEAP_OBJ_INSTANCE from a heap handle
// does minimum sanity checking 
static __inline__ TCPIP_HEAP_OBJ_INSTANCE* __attribute__((always_inline)) _PoolInstance(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_OBJ_INSTANCE* pInst = (TCPIP_HEAP_OBJ_INSTANCE*)heapH;
    if(pInst)
    {
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        if(pInst->heapObj.TCPIP_HEAP_Delete == _TCPIP_HEAP_Delete)
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        {
            return pInst;
        }
    }

    return 0;
}


// converts a heap handle to a valid pool descriptor pointer
static  TCPIP_POOL_DCPT* _PoolDcptHandle(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT* pPool = 0;
    TCPIP_HEAP_OBJ_INSTANCE* pInst = _PoolInstance(heapH);
    if(pInst)
    {
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        if(pInst->poolDcpt.poolMagic == _TCPIP_POOL_DCPT_MAGIC_NO)
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        {
            pPool = &pInst->poolDcpt;
        }
    }

    _PoolAssertCond(pPool != 0, __func__, __LINE__);
    return pPool;
}

// delete a pool entry
// mark it as invalid
static __inline__ void _PoolEntryDelete(TCPIP_POOL_ENTRY* pEntry)
{
    memset(pEntry, 0, sizeof(*pEntry));
}

// delete a pool descriptor
// mark it as invalid
static __inline__ void _PoolDcptDelete(TCPIP_HEAP_OBJ_INSTANCE* pInst)
{
    TCPIP_POOL_DCPT* pPDcpt = &pInst->poolDcpt;

    void* allocatedBuffer = pPDcpt->allocatedBuffer;
    void (*free_fnc)(void* ptr) = pPDcpt->free_fnc;
    OSAL_SEM_HANDLE_TYPE poolSem = pPDcpt->poolSemaphore;

    memset(pInst, 0, sizeof(*pInst));
    (*free_fnc)(allocatedBuffer);
    OSAL_SEM_Delete(&poolSem);
    _PoolDbgTraceClear();
}

// get a buffer from a specified pool entry
static __inline__ void* __attribute__((always_inline)) _PoolEntryGetBuff(TCPIP_POOL_ENTRY* pEntry)
{
	TCPIP_POOL_DATA_BLK* pN;
    
	if(pEntry->freeList)
	{
		pN = pEntry->freeList;
		pEntry->freeList = pN->next;
		pEntry->freeBlocks--;
        pN->next = (TCPIP_POOL_DATA_BLK*)pEntry;  // store the entry we belong to!
        return pN + 1;
	}

    return 0;
}

// return a previously allocated pool entry buffer
static __inline__ void __attribute__((always_inline)) _PoolEntryRetBuff(TCPIP_POOL_ENTRY* pEntry, TCPIP_POOL_DATA_BLK* pN)
{
    pN->next = pEntry->freeList;
    pEntry->freeList = pN;       // reinsert
    pEntry->freeBlocks++;
}

// aligns a buffer on a alignSize boundary
// alignSize must be multiple of 2!
// returns the adjusted size
static size_t   _PoolAlignBuffer(void** pBuffer, size_t buffSize, size_t alignSize)
{
    uint8_t*    blockBuffer = (uint8_t*)*pBuffer;
    uintptr_t   alignBuffer;

    // align properly
    alignBuffer = ((uintptr_t)blockBuffer + alignSize - 1 ) & ~(alignSize - 1);
    *pBuffer = (uint8_t*)alignBuffer;

    buffSize -= (uint8_t*)alignBuffer - blockBuffer ;

    return (buffSize & (~(alignSize - 1)));
}

// local data
// 
/********************************
 * Interface Functions
*******************************************/ 

// creates a TCPIP memory pool
TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateInternalPool(const TCPIP_STACK_HEAP_POOL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_HEAP_OBJ_INSTANCE* hInst;
    uint8_t            *heapBuffer, *expansionBuffer;
    size_t              entriesBuffSize, entriesSize, allocatedSize, headerSize, expansionSize;
    int                 nEntries, nExpEntries;
    const TCPIP_STACK_HEAP_POOL_ENTRY   *pCreateEntries, *pPoolCreate;
    TCPIP_STACK_HEAP_RES      poolRes;
    void*               allocatedBuffer;
    TCPIP_POOL_DCPT*    pPool; // pool is an array of pool entries
    TCPIP_POOL_ENTRY*   pEntry;
    size_t              usedBlksSize;
    int                 nEntriesBytes;
    int                 nUnitsPerBlk;
    int                 newBlocks;
    int                 poolIx, poolJx;
    OSAL_SEM_HANDLE_TYPE poolSem;
    OSAL_RESULT         osalRes;
    bool                checkFail;
    
    while(true)
    {
        hInst = 0;
        poolRes = TCPIP_STACK_HEAP_RES_OK;

        if( pHeapConfig == 0 || pHeapConfig->pEntries == 0)
        {
            poolRes = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }
        
        nEntries = pHeapConfig->nPoolEntries;
        pCreateEntries = pHeapConfig->pEntries; 

        // check for 0 or duplicate entries
        pPoolCreate = pCreateEntries;
        checkFail =  false;
        for(poolIx = 0; poolIx < nEntries && checkFail == false; poolIx++, pPoolCreate++)
        {
            if(pPoolCreate->entrySize == 0)
            {   // invalid entry size; abort
                checkFail =  true;
                break;
            }
            // check no other entry with the same size
            for(poolJx = poolIx + 1; poolJx < nEntries; poolJx++)
            {
                if(pPoolCreate->entrySize == (pHeapConfig->pEntries + poolJx)->entrySize)
                {   // duplicate
                    checkFail =  true;
                    break;
                }
            }
        }

        if(checkFail == true)
        {
            poolRes = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }



        entriesSize = 0;
        pPoolCreate = pCreateEntries;
        for(poolIx = 0; poolIx < nEntries; poolIx++, pPoolCreate++)
        {
            entriesSize += pPoolCreate->entrySize * pPoolCreate->nBlocks + sizeof(TCPIP_POOL_ENTRY) + pPoolCreate->nBlocks * sizeof(TCPIP_POOL_DATA_BLK);
        }

        // check how much space is needed for the entries increment sizes: entryExpBlks
        expansionSize = pHeapConfig->expansionHeapSize;
        nExpEntries = expansionSize == 0 ? 0 : nEntries;

        // align properly
        entriesSize = ((entriesSize + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)) * sizeof(uintptr_t);
        headerSize = ((sizeof(*hInst) + sizeof(*hInst->poolDcpt.entryExpBlks) * nExpEntries + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)) * sizeof(uintptr_t);
        expansionSize = ((expansionSize + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)) * sizeof(uintptr_t);
        
        allocatedSize = entriesSize + headerSize + expansionSize;  // extra room for header

        // allocate the buffer to create the heap into
        if((allocatedBuffer = (*pHeapConfig->malloc_fnc)(allocatedSize)) == 0)
        {
            poolRes = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }

        heapBuffer = (uint8_t*)allocatedBuffer;
        entriesBuffSize = allocatedSize - expansionSize;    // remove what we don't need now
        // align properly on a 32 bit boundary
        entriesBuffSize = _PoolAlignBuffer((void**)&heapBuffer, entriesBuffSize, sizeof(uintptr_t));

        // check for enough room
        entriesBuffSize -= headerSize;
        nEntriesBytes = nEntries * sizeof(TCPIP_POOL_ENTRY);

        osalRes = OSAL_SEM_Create(&poolSem, OSAL_SEM_TYPE_BINARY, 1, 1);
        if(osalRes != OSAL_RESULT_TRUE || entriesBuffSize < nEntriesBytes)
        {   // semaphore creation failed or not enough memory; abort
            (*pHeapConfig->free_fnc)(allocatedBuffer);
            poolRes = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }

        // create and initialize a proper heap object
        hInst = (TCPIP_HEAP_OBJ_INSTANCE*)heapBuffer;
        hInst->heapObj = _tcpip_heap_object;

        heapBuffer += headerSize;
        // this op should render a properly 32 bit aligned buffer!
        _PoolAssertCond(((uintptr_t)heapBuffer & (sizeof(uintptr_t) - 1)) == 0, __func__, __LINE__);

        // init each entry
        pPool = &hInst->poolDcpt;
        pPoolCreate = pCreateEntries;
        pEntry = (TCPIP_POOL_ENTRY*)heapBuffer;
        pPool->poolStart = pEntry;
        for(poolIx = 0; poolIx < nEntries; poolIx++, pEntry++, pPoolCreate++)
        {
            memset(pEntry, 0, sizeof(*pEntry));  // clear the entry
            // round up: room for extra next pointer in the TCPIP_POOL_DATA_BLK
            nUnitsPerBlk = (pPoolCreate->entrySize + sizeof(TCPIP_POOL_DATA_BLK) - 1) / sizeof(TCPIP_POOL_DATA_BLK) + 1;
            pEntry->allocSize = nUnitsPerBlk * sizeof(TCPIP_POOL_DATA_BLK);
            pEntry->blockSize = pEntry->allocSize - sizeof(TCPIP_POOL_DATA_BLK); // room for the next pointer
            pEntry->nBlocks = pPoolCreate->nBlocks;
            pEntry->poolH = pPool;
        }

        pPool->nEntries = nEntries;
        // always alloc uncached!
        pPool->flags = pHeapConfig->heapFlags | TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED;
        pPool->lastPoolErr = TCPIP_STACK_HEAP_RES_OK;
        pPool->poolEnd = pEntry;
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        pPool->poolMagic = _TCPIP_POOL_DCPT_MAGIC_NO;        
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        pPool->allocatedBuffer = allocatedBuffer;
        pPool->allocatedSize = allocatedSize;
        pPool->currAllocatedSize = 0;
        pPool->highWatermark = 0;
        pPool->free_fnc = pHeapConfig->free_fnc;
        pPool->poolSemaphore = poolSem; 

        if(expansionSize != 0)
        {
            expansionBuffer = (uint8_t*)allocatedBuffer + allocatedSize - expansionSize; 
            // align properly on a 32 bit boundary
            expansionSize = _PoolAlignBuffer((void**)&expansionBuffer, expansionSize, sizeof(uintptr_t));
        }
        else
        {
            expansionBuffer = 0;
        }
        pPool->expansionBuffer = expansionBuffer;
        pPool->expansionSize = (uint16_t)expansionSize;


        // and sort it ascending
        qsort(pPool->poolStart, nEntries, sizeof(TCPIP_POOL_ENTRY), _PoolEntryCompare);

        // store the expansion sizes
        // Note: because of qsort, the order has changed!
        if(pPool->expansionSize != 0)
        { 
            for(pEntry = pPool->poolStart; pEntry != pPool->poolEnd; pEntry++)
            {
                pPoolCreate = pCreateEntries;
                for(poolIx = 0; poolIx < nEntries; poolIx++, pPoolCreate++)
                {
                    nUnitsPerBlk = (pPoolCreate->entrySize + sizeof(TCPIP_POOL_DATA_BLK) - 1) / sizeof(TCPIP_POOL_DATA_BLK) + 1;
                    if(pEntry->allocSize ==  nUnitsPerBlk * sizeof(TCPIP_POOL_DATA_BLK))
                    {   // found entry
                        pPool->entryExpBlks[pEntry - pPool->poolStart] = pPoolCreate->nExpBlks;
                        break;
                    }
                }
            }
        }

        // create the data blocks for each pool entry 
        entriesBuffSize -= nEntriesBytes;
        heapBuffer = (uint8_t*)pPool->poolEnd;

        // allocate the largest blocks first
        for(pEntry = pPool->poolEnd - 1; pEntry >= pPool->poolStart; pEntry--)
        {
            if(pEntry == pPool->poolStart && pPool->expansionSize == 0 && pEntry->nBlocks != 0)
            {   // for the last allocation - the smallest - we take all the available space 
                // but only if run-time expansion is not enabled
                pEntry->nBlocks = 0xffff;
            }

            if(pEntry->nBlocks)
            {   // add these blocks 
                newBlocks = pEntry->nBlocks;
                pEntry->nBlocks = 0;
                if((usedBlksSize = _PoolEntryAddBlocks(pEntry, heapBuffer, entriesBuffSize, newBlocks)) == 0)
                {   // no more room
                    break;
                }
                heapBuffer += usedBlksSize;
                entriesBuffSize -= usedBlksSize;
            }
        }

        poolRes = TCPIP_STACK_HEAP_RES_OK;
        _PoolDbgTraceSet(hInst);
        break;
    }

    if(pRes)
    {
        *pRes = poolRes;
    }

    return hInst;
}

static size_t _PoolEntryAddBlocks(TCPIP_POOL_ENTRY* pEntry, uint8_t* blockBuffer, size_t buffSize, int newBlocks)
{
    int         ix, nBlocks;
    size_t      takenSize;
    void*       alignBuffer;
    TCPIP_POOL_DCPT* pPool = pEntry->poolH;
    TCPIP_POOL_DATA_BLK* pDataBlk, *pBlk, *pNextBlk = 0;

    // align the data blocks
    alignBuffer = blockBuffer;
    buffSize = _PoolAlignBuffer(&alignBuffer, buffSize, sizeof(TCPIP_POOL_DATA_BLK));

    // configure the data blocks
    nBlocks = buffSize / pEntry->allocSize;
    if(nBlocks > newBlocks)
    {
        nBlocks = newBlocks;
    }

    if(nBlocks)
    {
        takenSize = nBlocks * pEntry->allocSize;
        if((pPool->flags & TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED) != 0) 
        {
            pDataBlk = (TCPIP_POOL_DATA_BLK*)_TCPIP_HEAP_BufferMapNonCached(alignBuffer, takenSize);
        }
        else
        {
            pDataBlk = (TCPIP_POOL_DATA_BLK*)alignBuffer;
        }

        // link the data blocks
        for(ix = 0, pBlk = pDataBlk; ix < nBlocks - 1; ix++, pBlk = pNextBlk)
        {
            pNextBlk = (TCPIP_POOL_DATA_BLK*)((uint8_t*)pBlk + pEntry->allocSize);
            pBlk->next = pNextBlk;
        }

        // link the data blocks
        pBlk->next = pEntry->freeList;
        pEntry->freeList = pDataBlk;

        // adjust for buffer alignment
        takenSize += (uint8_t*)alignBuffer - blockBuffer;
    }
    else
    {
        takenSize = 0;
    }

    pEntry->nBlocks += nBlocks;    
    pEntry->freeBlocks += nBlocks;    

    return takenSize;
}

// simple binary search for the right slot
static TCPIP_POOL_ENTRY* _PoolEntryBySize(TCPIP_POOL_DCPT* pPool, int blkSize)
{
// binary search

    int len, half;
    TCPIP_POOL_ENTRY  *pFirstEntry, *pMidEntry;

    len = pPool->nEntries - 1;
    pFirstEntry = pPool->poolStart;
    
    while(len > 0)
    {
        half = len >> 1;
        pMidEntry = pFirstEntry + half;

        if(pMidEntry->blockSize < blkSize)
        {
            pFirstEntry = pMidEntry + 1;
            len -= half + 1;
        }
        else
        {
            len = half;
        }
    }
    
    return pFirstEntry->blockSize >= blkSize ? pFirstEntry : 0;

}

////////////////////////////
//  Low level routines
//  
// sorting function
static int _PoolEntryCompare(const void* p1, const void* p2)
{
    TCPIP_POOL_ENTRY* pE1 = (TCPIP_POOL_ENTRY*)p1;
    TCPIP_POOL_ENTRY* pE2 = (TCPIP_POOL_ENTRY*)p2;
   

    if(pE1->blockSize < pE2->blockSize)
    {
        return -1;
    }
    else if(pE1->blockSize > pE2->blockSize)
    {
        return 1;
    }

    return 0;    
}


//////////////////////////////////////////
//  Heap object routines
//

// destroys a pool
static TCPIP_STACK_HEAP_RES   _TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;
    TCPIP_POOL_ENTRY*    pEntry;
    TCPIP_STACK_HEAP_RES res = TCPIP_STACK_HEAP_RES_NO_HEAP;

    pPool = _PoolDcptHandle(heapH);
    
    while(pPool)
    {
        if(OSAL_SEM_Pend(&pPool->poolSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
        {
            res = TCPIP_STACK_HEAP_RES_SYNCH_ERR;
            break;
        }
        res = TCPIP_STACK_HEAP_RES_OK;
        for(pEntry = pPool->poolStart; pEntry < pPool->poolEnd; pEntry++)
        {   // check all entries are released
            if(pEntry->freeBlocks != pEntry->nBlocks)
            {
                res = (pPool->lastPoolErr = TCPIP_STACK_HEAP_RES_IN_USE);
                break;
            }
        }
        if(res != TCPIP_STACK_HEAP_RES_OK)
        {
            OSAL_SEM_Post(&pPool->poolSemaphore);
            break;
        }

        // OK
        for(pEntry = pPool->poolStart; pEntry < pPool->poolEnd; pEntry++)
        {
            _PoolEntryDelete(pEntry);
        }
        
        
        _PoolDcptDelete(_PoolInstance(heapH));

        break;
    }

    return res;
}

// get a pool buffer
static void* _TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
    TCPIP_POOL_DCPT*    pPool;
    TCPIP_POOL_ENTRY*   pEntry;
    void*               pBuff = 0;
    
    while(nBytes != 0 && (pPool = _PoolDcptHandle(heapH)) != 0)
    {
        if((pEntry = _PoolEntryBySize(pPool, nBytes)) == 0)
        {
            pPool->lastPoolErr = TCPIP_STACK_HEAP_RES_SIZE_ERR; // cannot satisfy the request
            break;
        }

        pBuff = _PoolEntryAlloc(pEntry);

        break;
    }

    return pBuff;
}

static void* _PoolEntryAlloc(TCPIP_POOL_ENTRY* pEntry)
{
    void*               pBuff;
    int                 newBlocks;
    size_t              usedBlksSize;
    TCPIP_POOL_DCPT*    pPool = pEntry->poolH;

    if(OSAL_SEM_Pend(&pPool->poolSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        return 0;
    }

    pBuff = 0;

    while(true)
    {
        if((pBuff = _PoolEntryGetBuff(pEntry)) != 0)
        {   // got it
            break;
        }

        if(pPool->expansionSize != 0)
        {   // there is an expansion buffer
            if((newBlocks = pPool->entryExpBlks[pEntry - pPool->poolStart]) != 0)
            {   // we could expand this entry
                if((usedBlksSize = _PoolEntryAddBlocks(pEntry, pPool->expansionBuffer, pPool->expansionSize, newBlocks)) != 0)
                {   // managed to get room to grow
                    pPool->expansionBuffer += usedBlksSize;
                    pPool->expansionSize -= usedBlksSize;
                    pBuff = _PoolEntryGetBuff(pEntry);  // this should not fail now
                    break;
                }
            }
        }

        // no luck so far; try moving up
        if((pPool->flags & TCPIP_STACK_HEAP_FLAG_POOL_STRICT) == 0)
        {   // we can try other entries
            for(pEntry = pEntry + 1; pEntry < pPool->poolEnd; pEntry++)
            {
                if((pBuff = _PoolEntryGetBuff(pEntry)) != 0)
                {
                    break;
                }
            }
        }

        break;
    }

    if(pBuff == 0)
    {
        pPool->lastPoolErr = TCPIP_STACK_HEAP_RES_NO_MEM; 
    }
    else
    {
        if((pPool->currAllocatedSize += pEntry->blockSize) > pPool->highWatermark)
        {
            pPool->highWatermark = pPool->currAllocatedSize;
        }
    }

    OSAL_SEM_Post(&pPool->poolSemaphore);
    return pBuff;
}

static void* _TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize)
{
    void* pBuff = _TCPIP_HEAP_Malloc(heapH, nElems * elemSize);
    if(pBuff)
    {
        memset(pBuff, 0, nElems * elemSize);
    }

    return pBuff;
}



// return a previously allocated pool entry buffer
static size_t _TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff)
{

    size_t blockSize = 0;

    while(pBuff)
    {
        TCPIP_POOL_DCPT* pPool = _PoolDcptHandle(heapH);
        if(pPool)
        {
            TCPIP_POOL_DATA_BLK*  pN = (TCPIP_POOL_DATA_BLK*)pBuff - 1;
            TCPIP_POOL_ENTRY* pEntry = (TCPIP_POOL_ENTRY*)pN->next; // get the pool entry pointer
            if(OSAL_SEM_Pend(&pPool->poolSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
            {
                break;
            }
            if(_PoolDbgCond(pEntry != 0 && pEntry->poolH == pPool, __func__, __LINE__))
            {
                _PoolEntryRetBuff(pEntry, pN);
                blockSize = pEntry->blockSize;
                pPool->currAllocatedSize -= blockSize;
            }
            else
            {
                pPool->lastPoolErr = TCPIP_STACK_HEAP_RES_PTR_ERR; 
            }
            OSAL_SEM_Post(&pPool->poolSemaphore);
        }
        break;
    }

    return blockSize;
}


static size_t _TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;

    pPool = _PoolDcptHandle(heapH);
    
    if(pPool)
    {
        return pPool->allocatedSize;
    }

    return 0;
}

static size_t _TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;
    TCPIP_POOL_ENTRY*    pEntry;

    pPool = _PoolDcptHandle(heapH);
    
    if(pPool)
    {
        for(pEntry = pPool->poolEnd - 1; pEntry >= pPool->poolStart; pEntry--)
        {
            if(pEntry->freeBlocks)
            {   // return the largest available block
                return pEntry->blockSize;
            }
        }
    }

    return 0;
}

static size_t _TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT*     pPool;
    TCPIP_POOL_ENTRY*    pEntry;
    size_t              totPoolSize = 0;

    pPool = _PoolDcptHandle(heapH);
    
    if(pPool)
    {
        for(pEntry = pPool->poolStart; pEntry < pPool->poolEnd; pEntry++)
        {
            if(pEntry->freeBlocks)
            {
                totPoolSize += pEntry->freeBlocks * pEntry->blockSize;
            }
        }
        totPoolSize += pPool->expansionSize;
    }

    return totPoolSize;
}

static size_t _TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT* pPool = _PoolDcptHandle(heapH);
    
    return pPool ? pPool->highWatermark : 0;
}

static TCPIP_STACK_HEAP_RES _TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_STACK_HEAP_RES  res = TCPIP_STACK_HEAP_RES_NO_HEAP;
    TCPIP_POOL_DCPT* pPool = _PoolDcptHandle(heapH);

    if(pPool)
    {
        res = pPool->lastPoolErr;
        pPool->lastPoolErr = TCPIP_STACK_HEAP_RES_OK;
    }

    return res;
}

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t _TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff)
{

    if(pBuff)
    {
        TCPIP_POOL_DCPT* pPool = _PoolDcptHandle(heapH);
        if(pPool)
        {
            TCPIP_POOL_DATA_BLK*  pN = (TCPIP_POOL_DATA_BLK*)pBuff - 1;
            TCPIP_POOL_ENTRY* pEntry = (TCPIP_POOL_ENTRY*)pN->next; // get the pool entry pointer
            if(_PoolDbgCond(pEntry != 0 && pEntry->poolH == pPool, __func__, __LINE__))
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
static TCPIP_STACK_HEAP_SIZE_HANDLE _TCPIP_HEAP_HandleBySize(TCPIP_STACK_HEAP_HANDLE heapH, int blkSize)
{
    TCPIP_POOL_DCPT* pPool;
    TCPIP_POOL_ENTRY* pEntry = 0;

    if(blkSize != 0)
    {
        pPool = _PoolDcptHandle(heapH);
        if(pPool != 0)
        {
            pEntry = _PoolEntryBySize(pPool, blkSize);

            if(pEntry == 0)
            {
                pPool->lastPoolErr = TCPIP_STACK_HEAP_RES_SIZE_ERR;
            }
        }
    }

    return pEntry;
}

// get a buffer from a specified pool entry
// nBytes is not really needed...but needed for the other heap types that don't really support entries....!
static void* _TCPIP_HEAP_MallocBySize(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_STACK_HEAP_SIZE_HANDLE entryH, size_t nBytes)
{
    TCPIP_POOL_DCPT* pPool;
    TCPIP_POOL_ENTRY* pEntry;
    void*       pBuff = 0;

    if(nBytes != 0)
    {
        if((pPool = _PoolDcptHandle(heapH)) != 0)
        {
            if((pEntry = _PoolEntryHandle(pPool, entryH)) != 0) 
            {
                if(nBytes <= pEntry->blockSize)
                {
                    pBuff = _PoolEntryAlloc(pEntry);
                }
            }
        }
    }

    return pBuff;
}
// converts a entry handle to a valid pool Entry pointer
static TCPIP_POOL_ENTRY* _PoolEntryHandle(TCPIP_POOL_DCPT* pPool, TCPIP_STACK_HEAP_SIZE_HANDLE entryH)
{
    TCPIP_POOL_ENTRY* pEntry = (TCPIP_POOL_ENTRY*)entryH;
    if(pEntry)
    {
#if ((TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC) != 0)
        if(pPool->poolStart <= pEntry && pEntry < pPool->poolEnd)
#endif  // (TCPIP_POOL_DEBUG_LEVEL & TCPIP_POOL_DEBUG_MASK_BASIC)
        {   // check that the pool it belongs to is valid
            return pEntry;
        }
    }

    _PoolAssertCond(false, __func__, __LINE__);
    return 0;
}



#endif  // defined(TCPIP_STACK_HEAP_SIZE_ALLOCATION)


int TCPIP_HEAP_POOL_Entries(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_POOL_DCPT* pPool = _PoolDcptHandle(heapH);
    if(pPool)
    {
        return pPool->nEntries;
    }

    return 0;
}


bool TCPIP_HEAP_POOL_EntryList(TCPIP_STACK_HEAP_HANDLE heapH, int entryIx, TCPIP_HEAP_POOL_ENTRY_LIST* pList)
{
    TCPIP_POOL_DCPT* pPool = _PoolDcptHandle(heapH);
    if(pPool && entryIx < pPool->nEntries)
    {
        if(pList)
        {
            TCPIP_POOL_ENTRY* pEntry = pPool->poolStart + entryIx;
            pList->blockSize = pEntry->blockSize;
            pList->nBlocks = pEntry->nBlocks;
            pList->freeBlocks = pEntry->freeBlocks;
            pList->totEntrySize = pEntry->blockSize * pEntry->nBlocks;
            pList->totFreeSize = pEntry->blockSize * pEntry->freeBlocks;
            pList->expansionSize = pPool->expansionSize;
        }
        return true;
    }

    return false;
}


