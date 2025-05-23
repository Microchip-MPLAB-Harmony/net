/*******************************************************************************
  TCPIP Heap Allocation Manager

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_heap_alloc.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef H_TCPIP_HEAP_ALLOC_H_
#define H_TCPIP_HEAP_ALLOC_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


// heap trace
// only if TCPIP_STACK_DRAM_DEBUG_ENABLE and TCPIP_STACK_DRAM_TRACE_ENABLE are enabled
// the moduleId is the one from tcpip.h::TCPIP_STACK_MODULE
typedef struct
{
    int         moduleId;           // info belonging to this module; <0 means slot free
    size_t      nAllocs;            // total number of alloc operations
    size_t      nFrees;             // total number of free operations
    size_t      totAllocated;       // total number of bytes allocated successfully by this module
    ssize_t     currAllocated;      // number of bytes still allocated by this module
    size_t      totFailed;          // total number of bytes that failed for this module
    size_t      maxFailed;          // maximum number of bytes that could not be allocated
}TCPIP_HEAP_TRACE_ENTRY;

// heap distribution entry
// useful for checking the distribution of allocated memory blocks in the heap
// only if TCPIP_STACK_DRAM_DEBUG_ENABLE and TCPIP_STACK_DRAM_DIST_ENABLE are enabled
//
typedef struct
{
    size_t          lowLimit;           // lower limit of the memory block
    size_t          highLimit;          // upper limit of the memory block
    struct moduleDist
    {
        int         modId;              // module id
        size_t      modHits;            // number of module hits
    }modDist[4];
    size_t          gHits;              // number of global allocations hits, no module info
    size_t          currHits;           // current number of allocations hits
}TCPIP_HEAP_DIST_ENTRY;

/********************************
 * Interface Functions
*******************************************/ 

// Creates a TCPIP memory heap
//
/*********************************************************************
 * Function:
    TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_Create(const TCPIP_STACK_HEAP_CONFIG* initData, TCPIP_STACK_HEAP_RES* pRes);
 *
 * PreCondition:    initData    - valid initialization data
 *
 * Input:           initData    - pointer to initialization data for the corresponding heap type
 *                  pRes        - Address to store the operation result.
 *                                Can be NULL if result not needed         
 *
 * Output:          A valid handle if the creation succeeded,
 *                  NULL otherwise.         
 *
 * Side Effects:    None
 *
 * Overview:        The function creates a heap of the provided type and using the supplied initialization data.
 *                  The size of the heap cannot be changed afterwards.
 *
 * Note:            The current heap alignment unit is an unsigned long, i.e. 32 bits.
 *
 ********************************************************************/
TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_Create(const TCPIP_STACK_HEAP_CONFIG* initData, TCPIP_STACK_HEAP_RES* pRes);

/*********************************************************************
 * Function:        TCPIP_STACK_HEAP_RES TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap that was created with TCPIP_HEAP_Create()
 *
 * Output:          TCPIP_STACK_HEAP_RES_OK if deletion succeeded
 *                  TCPIP_STACK_HEAP_RES_IN_USE if the heap is still used
 *                  TCPIP_STACK_HEAP_RES_NO_HEAP if no such heap exists
 *                  
 *
 * Side Effects:    None
 *
 * Overview:        The function deletes a previously created heap.
 *
 * Note:            The call will fail if blocks are still in use.
 ********************************************************************/
TCPIP_STACK_HEAP_RES      TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);

// simple out of memory notification mechanism
// only one client/handler supported per heap
typedef void    (*TCPIP_HEAP_NO_MEM_HANDLER)(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes,  int moduleId, int lineNo);


// *****************************************************************************
/*
  Structure:
    TCPIP_HEAP_OBJECT

  Summary:
    A TCPIP_HEAP Object definition

  Description:
    Definition of a TCPIP_HEAP Object
    A TCPIP_HEAP_OBJECT is an object exposing heap allocation functions
*/

// heap object
typedef struct
{
    TCPIP_STACK_HEAP_RES (*F_TCPIP_HEAP_Delete)(TCPIP_STACK_HEAP_HANDLE h);
    void*               (*F_TCPIP_HEAP_Malloc)(TCPIP_STACK_HEAP_HANDLE h, size_t nBytes);
    void*               (*F_TCPIP_HEAP_Calloc)(TCPIP_STACK_HEAP_HANDLE h, size_t nElems, size_t elemSize);
    size_t              (*F_TCPIP_HEAP_Free)(TCPIP_STACK_HEAP_HANDLE h, const void* pBuff);
    size_t              (*F_TCPIP_HEAP_Size)(TCPIP_STACK_HEAP_HANDLE h);
    size_t              (*F_TCPIP_HEAP_MaxSize)(TCPIP_STACK_HEAP_HANDLE h);
    size_t              (*F_TCPIP_HEAP_FreeSize)(TCPIP_STACK_HEAP_HANDLE h);
    size_t              (*F_TCPIP_HEAP_HighWatermark)(TCPIP_STACK_HEAP_HANDLE h);
    TCPIP_STACK_HEAP_RES      (*F_TCPIP_HEAP_LastError)(TCPIP_STACK_HEAP_HANDLE h);
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    // returns the actual allocated size for a successfully allocated block
    size_t              (*F_TCPIP_HEAP_AllocSize)(TCPIP_STACK_HEAP_HANDLE h, const void* ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
}TCPIP_HEAP_OBJECT;




// heap object offerring debugging facilities
#if !defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

/*********************************************************************
 * Function:        void* TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap
 *                  nBytes      - size of the block to be allocated
 *
 * Output:          a valid pointer to an allocated block if allocation succeeded,
 *                  NULL otherwise.         
 *
 * Side Effects:    None
 *
 * Overview:        The function checks the heap for finding a block large enough to accommodate the request.
 *                  A first fit algorithm is used.
 *
 * Note:            None
 ********************************************************************/
static __inline__ void* __attribute__((always_inline)) TCPIP_HEAP_MallocInline(TCPIP_STACK_HEAP_HANDLE h, size_t nBytes)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_Malloc)(h, nBytes);
}
#define  TCPIP_HEAP_Malloc(h, nBytes)   TCPIP_HEAP_MallocInline(h, nBytes)
// out of line version
void*   TCPIP_HEAP_MallocOutline(TCPIP_STACK_HEAP_HANDLE h, size_t nBytes);

/*********************************************************************
 * Function:        void* TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap
 *                  nElems      - number of elements to be allocated
 *                  elemSize    - size of each element, in bytes
 *
 * Output:          a valid pointer to an allocated block if allocation succeeded,
 *                  NULL otherwise.         
 *
 * Side Effects:    None
 *
 * Overview:        The function checks the heap for finding a block large enough to accommodate
 *                  nElems*elemSize request.
 *                  If the block is found, it is zero initialized and returned to user.
 *                  A first fit algorithm is used.
 *
 * Note:            None
 ********************************************************************/
static __inline__ void* __attribute__((always_inline)) TCPIP_HEAP_CallocInline(TCPIP_STACK_HEAP_HANDLE h, size_t nElems, size_t elemSize)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_Calloc)(h, nElems, elemSize);
}

#define  TCPIP_HEAP_Calloc(h, nElems, elemSize)   TCPIP_HEAP_CallocInline(h, nElems, elemSize)
// out of line version
void* TCPIP_HEAP_CallocOutline(TCPIP_STACK_HEAP_HANDLE h, size_t nElems, size_t elemSize);

static __inline__ size_t __attribute__((always_inline)) TCPIP_HEAP_FreeInline(TCPIP_STACK_HEAP_HANDLE h, const void* ptr)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_Free)(h, ptr);
}

/*********************************************************************
 * Function:        size_t TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, void* pBuff);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap
 *                  pBuff       - pointer to a buffer previously allocated from the heap 
 *
 * Output:          number of freed bytes the operation succeeded,
 *                  0 if invalid pointer
 *                  or the number of freed bytes is not available.
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the buffer to the heap.
 *                  Left and/or right defragment operations are performed if possible.
 *
 * Note:            Not all heap types support returning the number of freed bytes.
 *                  These heaps will return 0 even for a successful operation.
 *
 ********************************************************************/
#define  TCPIP_HEAP_Free(h, ptr)   TCPIP_HEAP_FreeInline(h, ptr)
// out of line version
size_t TCPIP_HEAP_FreeOutline(TCPIP_STACK_HEAP_HANDLE h, const void* ptr);


#else   // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


void* TCPIP_HEAP_MallocDebug(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes, int moduleId, int lineNo);
#define  TCPIP_HEAP_Malloc(heapH, nBytes)   TCPIP_HEAP_MallocDebug(heapH, nBytes, (int)TCPIP_THIS_MODULE_ID, __LINE__)


void* TCPIP_HEAP_CallocDebug(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize, int moduleId, int lineNo);
#define  TCPIP_HEAP_Calloc(heapH, nElems, elemSize)   TCPIP_HEAP_CallocDebug(heapH, nElems, elemSize, (int)TCPIP_THIS_MODULE_ID, __LINE__)

size_t TCPIP_HEAP_FreeDebug(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr, int moduleId, int lineNo);
#define  TCPIP_HEAP_Free(h, ptr)   TCPIP_HEAP_FreeDebug(h, ptr, (int)TCPIP_THIS_MODULE_ID, __LINE__)

/*********************************************************************
 * Function:      TCPIP_STACK_HEAP_RES TCPIP_HEAP_SetNoMemHandler(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_HEAP_NO_MEM_HANDLER handler)
 *
 * PreCondition:    heapH       - valid heap handle
 *
 * Input:           heapH       - handle of a heap
 *                  handler     - notification handler to be called when a memory allocation failed
 *                                use 0 to clear
 *
 * Output:          TCPIP_STACK_HEAP_RES_OK if no error occurred
 *
 * Side Effects:    None
 *
 * Overview:        The function sets the out of memory notification handler.
 *                  It is a very simple notification and only one client is supported,
 *                  basically it is meant for the creator of the heap.
 *                  When the debug is enabled and some allocation fails
 *                  some system warning message will be sent to the debug port.
 *                  Also if the handler is not null it will be called.
 *
 * Note:            The handler could be a signaling function, short
 *                  and just setting an event of some sort.
 *                  However since it is not called from ISR context this is not really a requirement and
 *                  some more extensive processing can be done.
 *
 *                  This function is implemented only when the system debug and
 *                  heap debug (TCPIP_STACK_DRAM_DEBUG_ENABLE) are enabled 
 ********************************************************************/
TCPIP_STACK_HEAP_RES TCPIP_HEAP_SetNoMemHandler(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_HEAP_NO_MEM_HANDLER handler);

/*********************************************************************
 * Function:      bool  TCPIP_HEAP_TraceGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_TRACE_ENTRY* tEntry)
 *
 * PreCondition:    None
 *
 * Input:           heapH       - handle of a heap
 *                  entryIx     - index of the requested heap trace entry
 *                  tEntry      - address of a TCPIP_HEAP_TRACE_ENTRY that will be updated with corresponding info
 *
 * Output:          true if tEntry was populated with the info
 *                  false if entryIx record does not exist or if tracing is not enabled
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the heap trace info for the requested entry.
 *
 * Note:            
 *                  Trace info is recorded only when
 *                  TCPIP_STACK_DRAM_DEBUG_ENABLE and TCPIP_STACK_DRAM_TRACE_ENABLE are enabled
 *                  Also, the number of defined slots TCPIP_STACK_DRAM_TRACE_SLOTS has to be 
 *                  big enough to accommodate all the modules.
 *
 *                  The moduleId is the one from tcpip.h::TCPIP_STACK_MODULE
 ********************************************************************/
bool  TCPIP_HEAP_TraceGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_TRACE_ENTRY* tEntry);

/*********************************************************************
 * Function:      size_t TCPIP_HEAP_TraceGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH, bool getUsed)
 *
 * PreCondition:    None
 *
 * Input:           heapH       - handle of a heap
 *                  getUsed     - if true, only the currently used record slots will be returned
 *                                otherwise the whole number of slots will be returned  
 *
 * Output:          number of entries the heap has trace slots for
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the number of slots that hold trace info.
 *                  By using the "getUsed" parameter, only the number of slots
 *                  that hold recorded info will be returned.
 *
 * Note:            
 *                  Trace info is recorded only when
 *                  TCPIP_STACK_DRAM_DEBUG_ENABLE and TCPIP_STACK_DRAM_TRACE_ENABLE are enabled
 *                  Also, the number of defined slots TCPIP_STACK_DRAM_TRACE_SLOTS has to be 
 *                  big enough to accommodate all the modules.
 *
 *                  The moduleId is the one from tcpip.h::TCPIP_STACK_MODULE
 *
 *                  If trace is enabled the function will return the value of TCPIP_STACK_DRAM_TRACE_SLOTS
 ********************************************************************/
size_t TCPIP_HEAP_TraceGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH, bool getUsed);


/*********************************************************************
 * Function:      bool  TCPIP_HEAP_DistGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_DIST_ENTRY* pEntry)
 *
 * PreCondition:    None
 *
 * Input:           heapH       - handle of a heap
 *                  entryIx     - index of the requested heap distribution entry
 *                  pEntry      - address of a TCPIP_HEAP_DIST_ENTRY that will be updated with corresponding info
 *
 * Output:          true if pEntry was populated with the info
 *                  false if entryIx record does not exist or if heap distribution is not enabled
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the heap distribution info for the requested entry.
 *
 * Note:            
 *                  Heap distribution info is recorded only when
 *                  TCPIP_STACK_DRAM_DEBUG_ENABLE and TCPIP_STACK_DRAM_DIST_ENABLE are enabled
 *
 ********************************************************************/
bool  TCPIP_HEAP_DistGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_DIST_ENTRY* pEntry);

/*********************************************************************
 * Function:      size_t TCPIP_HEAP_DistGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH)
 *
 * PreCondition:    None
 *
 * Input:           heapH       - handle of a heap
 *
 * Output:          number of entries the heap has distribution slots for
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the number of slots that hold distribution info.
 *
 * Note:            
 *                  Heap distribution info is recorded only when
 *                  TCPIP_STACK_DRAM_DEBUG_ENABLE and TCPIP_STACK_DRAM_DIST_ENABLE are enabled
 *
 ********************************************************************/
size_t TCPIP_HEAP_DistGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH);


#endif  // !defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

// general mappings
//

/*********************************************************************
 * Function:        size_t TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap
 *
 * Output:          the size of the heap as it was created
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the size of the heap.
 *                  This is the size that was specified when the heap was created.
 *
 * Note:            None
 ********************************************************************/
static __inline__ size_t __attribute__((always_inline)) TCPIP_HEAP_SizeInline(TCPIP_STACK_HEAP_HANDLE h)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_Size)(h);
}
#define TCPIP_HEAP_Size(h) TCPIP_HEAP_SizeInline(h)

/*********************************************************************
 * Function:        size_t TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap
 *
 * Output:          the max size of a block that can be allocated from the heap
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the maximum size of a block that can be 
 *                  currently allocated from this heap.
 *
 * Note:            This is info only.
 *                  It can change is the heap has multiple clients.
 *
 *                  The call is expensive.
 *                  The whole heap has to be traversed to find the maximum.
 *                  If the heap is really fragmented this might take some time.
 *
 ********************************************************************/
static __inline__ size_t __attribute__((always_inline)) TCPIP_HEAP_MaxSizeInline(TCPIP_STACK_HEAP_HANDLE h)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_MaxSize)(h);
}
#define TCPIP_HEAP_MaxSize(h) TCPIP_HEAP_MaxSizeInline(h)

/*********************************************************************
 * Function:        size_t TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap
 *
 * Output:          the size of the available space in the heap
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the size of the space currently
 *                  available in the heap.
 *
 * Note:            This is a cumulative number, counting all the existent free space.
 *                  It is not the maximum blocks size that could be allocated from the heap.
 ********************************************************************/
static __inline__ size_t __attribute__((always_inline)) TCPIP_HEAP_FreeSizeInline(TCPIP_STACK_HEAP_HANDLE h)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_FreeSize)(h);
}
#define TCPIP_HEAP_FreeSize(h) TCPIP_HEAP_FreeSizeInline(h)

/*********************************************************************
 * Function:        size_t TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH);
 *
 * PreCondition:    heapH       - valid heap handle 
 *
 * Input:           heapH       - handle to a heap
 *
 * Output:          the maximum amount of allocated memory from this heap
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the maximum amount of memory that has been
 *                  allocated from this heap
 *
 * Note:            This is a cumulative number, maintained over the life time of the heap.
 ********************************************************************/
static __inline__ size_t __attribute__((always_inline)) TCPIP_HEAP_HighWatermarkInline(TCPIP_STACK_HEAP_HANDLE h)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_HighWatermark)(h);
}
#define TCPIP_HEAP_HighWatermark(h) TCPIP_HEAP_HighWatermarkInline(h)

/*********************************************************************
 * Function:      TCPIP_STACK_HEAP_RES TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
 *
 * PreCondition:    heapH       - valid heap handle
 *
 * Input:           heapH       - handle of a heap
 *
 * Output:          The last error encountered in an operation
 *                  or TCPIP_STACK_HEAP_RES_OK if no error occurred
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the last error encountered in a heap operation.
 *                  It clears the value of the last error variable.
 *
 * Note:            The heap holds an error variable storing the last error
 *                  encountered in an operation.
 *                  This should be consulted by the caller after each operation
 *                  that returns an invalid result for checking what the error condition
 *                  was.
 ********************************************************************/
static __inline__ TCPIP_STACK_HEAP_RES __attribute__((always_inline)) TCPIP_HEAP_LastErrorInline(TCPIP_STACK_HEAP_HANDLE h)
{
    return (*((const TCPIP_HEAP_OBJECT*)h)->F_TCPIP_HEAP_LastError)(h);
}
#define TCPIP_HEAP_LastError(h) TCPIP_HEAP_LastErrorInline(h)


#if defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
// pool heap specific functionality for managing entries, sizes, etc.
// these functions should be called with a valid pool heap handle!

// pool entry list describing the entry parameters
typedef struct
{
    uint16_t blockSize;
    uint16_t nBlocks;
    uint16_t freeBlocks;
    // the expansion size at the moment of call
    // Note that this is a global pool number, not per entry  
    uint16_t expansionSize;
    size_t totEntrySize;
    size_t totFreeSize;
}TCPIP_HEAP_POOL_ENTRY_LIST;

// returns the number of entries in the pool heap
uint16_t     TCPIP_HEAP_POOL_Entries(TCPIP_STACK_HEAP_HANDLE heapH);


// lists a pool entry identified by its index
bool TCPIP_HEAP_POOL_EntryList(TCPIP_STACK_HEAP_HANDLE heapH, size_t entryIx, TCPIP_HEAP_POOL_ENTRY_LIST* pList);

// create internal pool heap
TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateInternalPool(const TCPIP_STACK_HEAP_POOL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes);
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

// create external heap
#if defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
extern TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateExternal(const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes);
#endif


// *****************************************************************************
// supported heap creation functions
//
// create internal heap
#if defined (TCPIP_STACK_USE_INTERNAL_HEAP)
extern TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateInternal(const TCPIP_STACK_HEAP_INTERNAL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes);
#endif



// ****************************************************************
// buffer mapping functions
const void* F_TCPIP_HEAP_BufferMapNonCached(const void* dataBuff, size_t buffSize);
const void* F_TCPIP_HEAP_PointerMapCached(const void* ptr);



#endif  // H_TCPIP_HEAP_ALLOC_H_

