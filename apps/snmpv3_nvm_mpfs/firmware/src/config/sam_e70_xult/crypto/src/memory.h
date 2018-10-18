/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    memory.h
  
  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright © 2013-2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END



/* submitted by eof */


#ifndef WOLFSSL_MEMORY_H
#define WOLFSSL_MEMORY_H

#include <stdlib.h>
#include "crypto/src/types.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_STATIC_MEMORY
    #ifdef WOLFSSL_DEBUG_MEMORY
        typedef void *(*wolfSSL_Malloc_cb)(size_t size, void* heap, int type, const char* func, unsigned int line);
        typedef void (*wolfSSL_Free_cb)(void *ptr, void* heap, int type, const char* func, unsigned int line);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size, void* heap, int type, const char* func, unsigned int line);
        WOLFSSL_API void* wolfSSL_Malloc(size_t size, void* heap, int type, const char* func, unsigned int line);
        WOLFSSL_API void  wolfSSL_Free(void *ptr, void* heap, int type, const char* func, unsigned int line);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size, void* heap, int type, const char* func, unsigned int line);
    #else
        typedef void *(*wolfSSL_Malloc_cb)(size_t size, void* heap, int type);
        typedef void (*wolfSSL_Free_cb)(void *ptr, void* heap, int type);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size, void* heap, int type);
        WOLFSSL_API void* wolfSSL_Malloc(size_t size, void* heap, int type);
        WOLFSSL_API void  wolfSSL_Free(void *ptr, void* heap, int type);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size, void* heap, int type);
    #endif /* WOLFSSL_DEBUG_MEMORY */
#else
    #ifdef WOLFSSL_DEBUG_MEMORY
        typedef void *(*wolfSSL_Malloc_cb)(size_t size, const char* func, unsigned int line);
        typedef void (*wolfSSL_Free_cb)(void *ptr, const char* func, unsigned int line);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size, const char* func, unsigned int line);

        /* Public in case user app wants to use XMALLOC/XFREE */
        WOLFSSL_API void* wolfSSL_Malloc(size_t size, const char* func, unsigned int line);
        WOLFSSL_API void  wolfSSL_Free(void *ptr, const char* func, unsigned int line);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size, const char* func, unsigned int line);
    #else
        typedef void *(*wolfSSL_Malloc_cb)(size_t size);
        typedef void (*wolfSSL_Free_cb)(void *ptr);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size);
        /* Public in case user app wants to use XMALLOC/XFREE */
        WOLFSSL_API void* wolfSSL_Malloc(size_t size);
        WOLFSSL_API void  wolfSSL_Free(void *ptr);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size);
    #endif /* WOLFSSL_DEBUG_MEMORY */
#endif /* WOLFSSL_STATIC_MEMORY */

/* Public get/set functions */
WOLFSSL_API int wolfSSL_SetAllocators(wolfSSL_Malloc_cb,
                                      wolfSSL_Free_cb,
                                      wolfSSL_Realloc_cb);

WOLFSSL_API int wolfSSL_GetAllocators(wolfSSL_Malloc_cb*,
                                      wolfSSL_Free_cb*,
                                      wolfSSL_Realloc_cb*);

#ifdef WOLFSSL_STATIC_MEMORY
    #define WOLFSSL_STATIC_TIMEOUT 1
    #ifndef WOLFSSL_STATIC_ALIGN
        #define WOLFSSL_STATIC_ALIGN 16
    #endif
    #ifndef WOLFMEM_MAX_BUCKETS
        #define WOLFMEM_MAX_BUCKETS  9
    #endif
    #define WOLFMEM_DEF_BUCKETS  9     /* number of default memory blocks */
    #define WOLFMEM_IO_SZ        16992 /* 16 byte aligned */
    #ifndef WOLFMEM_BUCKETS
        /* default size of chunks of memory to seperate into
         * having session certs enabled makes a 21k SSL struct */
        #ifndef SESSION_CERTS
            #define WOLFMEM_BUCKETS 64,128,256,512,1024,2432,3456,4544,16128
        #else
            #define WOLFMEM_BUCKETS 64,128,256,512,1024,2432,3456,4544,21056
        #endif
    #endif
    #ifndef WOLFMEM_DIST
        #define WOLFMEM_DIST    8,4,4,12,4,5,8,1,1
    #endif

    /* flags for loading static memory (one hot bit) */
    #define WOLFMEM_GENERAL       0x01
    #define WOLFMEM_IO_POOL       0x02
    #define WOLFMEM_IO_POOL_FIXED 0x04
    #define WOLFMEM_TRACK_STATS   0x08

    #ifndef WOLFSSL_MEM_GUARD
    #define WOLFSSL_MEM_GUARD
        typedef struct WOLFSSL_MEM_STATS      WOLFSSL_MEM_STATS;
        typedef struct WOLFSSL_MEM_CONN_STATS WOLFSSL_MEM_CONN_STATS;
    #endif

    struct WOLFSSL_MEM_CONN_STATS {
        word32 peakMem;   /* peak memory usage    */
        word32 curMem;    /* current memory usage */
        word32 peakAlloc; /* peak memory allocations */
        word32 curAlloc;  /* current memory allocations */
        word32 totalAlloc;/* total memory allocations for lifetime */
        word32 totalFr;   /* total frees for lifetime */
    };

    struct WOLFSSL_MEM_STATS {
        word32 curAlloc;  /* current memory allocations */
        word32 totalAlloc;/* total memory allocations for lifetime */
        word32 totalFr;   /* total frees for lifetime */
        word32 totalUse;  /* total amount of memory used in blocks */
        word32 avaIO;     /* available IO specific pools */
        word32 maxHa;     /* max number of concurent handshakes allowed */
        word32 maxIO;     /* max number of concurent IO connections allowed */
        word32 blockSz[WOLFMEM_MAX_BUCKETS]; /* block sizes in stacks */
        word32 avaBlock[WOLFMEM_MAX_BUCKETS];/* ava block sizes */
        word32 usedBlock[WOLFMEM_MAX_BUCKETS];
        int    flag; /* flag used */
    };

    typedef struct wc_Memory wc_Memory; /* internal structure for mem bucket */
    typedef struct WOLFSSL_HEAP {
        wc_Memory* ava[WOLFMEM_MAX_BUCKETS];
        wc_Memory* io;                  /* list of buffers to use for IO */
        word32     maxHa;               /* max concurent handshakes */
        word32     curHa;
        word32     maxIO;               /* max concurrent IO connections */
        word32     curIO;
        word32     sizeList[WOLFMEM_MAX_BUCKETS];/* memory sizes in ava list */
        word32     distList[WOLFMEM_MAX_BUCKETS];/* general distribution */
        word32     inUse; /* amount of memory currently in use */
        word32     ioUse;
        word32     alloc; /* total number of allocs */
        word32     frAlc; /* total number of frees  */
        int        flag;
        wolfSSL_Mutex memory_mutex;
    } WOLFSSL_HEAP;

    /* structure passed into XMALLOC as heap hint
     * having this abstraction allows tracking statistics of individual ssl's
     */
    typedef struct WOLFSSL_HEAP_HINT {
        WOLFSSL_HEAP*           memory;
        WOLFSSL_MEM_CONN_STATS* stats;  /* hold individual connection stats */
        wc_Memory*  outBuf; /* set if using fixed io buffers */
        wc_Memory*  inBuf;
        byte        haFlag; /* flag used for checking handshake count */
    } WOLFSSL_HEAP_HINT;

    WOLFSSL_API int wc_LoadStaticMemory(WOLFSSL_HEAP_HINT** pHint,
            unsigned char* buf, unsigned int sz, int flag, int max);

    WOLFSSL_LOCAL int wolfSSL_init_memory_heap(WOLFSSL_HEAP* heap);
    WOLFSSL_LOCAL int wolfSSL_load_static_memory(byte* buffer, word32 sz,
                                                  int flag, WOLFSSL_HEAP* heap);
    WOLFSSL_LOCAL int wolfSSL_GetMemStats(WOLFSSL_HEAP* heap,
                                                      WOLFSSL_MEM_STATS* stats);
    WOLFSSL_LOCAL int SetFixedIO(WOLFSSL_HEAP* heap, wc_Memory** io);
    WOLFSSL_LOCAL int FreeFixedIO(WOLFSSL_HEAP* heap, wc_Memory** io);

    WOLFSSL_API int wolfSSL_StaticBufferSz(byte* buffer, word32 sz, int flag);
    WOLFSSL_API int wolfSSL_MemoryPaddingSz(void);
#endif /* WOLFSSL_STATIC_MEMORY */

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_MEMORY_H */

