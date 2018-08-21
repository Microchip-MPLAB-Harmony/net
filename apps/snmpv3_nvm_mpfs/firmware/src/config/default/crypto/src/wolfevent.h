/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    wolfevent.h
  
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
#ifndef _WOLF_EVENT_H_
#define _WOLF_EVENT_H_

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef SINGLE_THREADED
    #include "crypto/src/wc_port.h"
#endif

#ifndef WOLFSSL_WOLFSSL_TYPE_DEFINED
#define WOLFSSL_WOLFSSL_TYPE_DEFINED
typedef struct WOLFSSL WOLFSSL;
#endif
typedef struct WOLF_EVENT WOLF_EVENT;
#ifndef WOLFSSL_WOLFSSL_CTX_TYPE_DEFINED
#define WOLFSSL_WOLFSSL_CTX_TYPE_DEFINED
typedef struct WOLFSSL_CTX WOLFSSL_CTX;
#endif

typedef unsigned short WOLF_EVENT_FLAG;

typedef enum WOLF_EVENT_TYPE {
    WOLF_EVENT_TYPE_NONE,
#ifdef WOLFSSL_ASYNC_CRYPT
    WOLF_EVENT_TYPE_ASYNC_WOLFSSL,    /* context is WOLFSSL* */
    WOLF_EVENT_TYPE_ASYNC_WOLFCRYPT,  /* context is WC_ASYNC_DEV */
    WOLF_EVENT_TYPE_ASYNC_FIRST = WOLF_EVENT_TYPE_ASYNC_WOLFSSL,
    WOLF_EVENT_TYPE_ASYNC_LAST = WOLF_EVENT_TYPE_ASYNC_WOLFCRYPT,
#endif /* WOLFSSL_ASYNC_CRYPT */
} WOLF_EVENT_TYPE;

struct WOLF_EVENT {
    /* double linked list */
    WOLF_EVENT*         next;
    WOLF_EVENT*         prev;

    void*               context;
    union {
        void* ptr;
#ifdef WOLFSSL_ASYNC_CRYPT
        struct WC_ASYNC_DEV* async;
#endif
    } dev;
#ifdef HAVE_CAVIUM
    CavReqId            reqId;
#endif
    int                 ret;    /* Async return code */
    unsigned int        flags;
    WOLF_EVENT_TYPE     type;

    /* event flags */
    WOLF_EVENT_FLAG     pending:1;
    WOLF_EVENT_FLAG     done:1;
    /* Future event flags can go here */
};

enum WOLF_POLL_FLAGS {
    WOLF_POLL_FLAG_CHECK_HW = 0x01,
};

typedef struct {
    WOLF_EVENT*         head;     /* head of queue */
    WOLF_EVENT*         tail;     /* tail of queue */
#ifndef SINGLE_THREADED
    wolfSSL_Mutex       lock;     /* queue lock */
#endif
    int                 count;
} WOLF_EVENT_QUEUE;


#ifdef HAVE_WOLF_EVENT

/* Event */
WOLFSSL_API int wolfEvent_Init(WOLF_EVENT* event, WOLF_EVENT_TYPE type, void* context);
WOLFSSL_API int wolfEvent_Poll(WOLF_EVENT* event, WOLF_EVENT_FLAG flags);

/* Event Queue */
WOLFSSL_API int wolfEventQueue_Init(WOLF_EVENT_QUEUE* queue);
WOLFSSL_API int wolfEventQueue_Push(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event);
WOLFSSL_API int wolfEventQueue_Pop(WOLF_EVENT_QUEUE* queue, WOLF_EVENT** event);
WOLFSSL_API int wolfEventQueue_Poll(WOLF_EVENT_QUEUE* queue, void* context_filter,
    WOLF_EVENT** events, int maxEvents, WOLF_EVENT_FLAG flags, int* eventCount);
WOLFSSL_API int wolfEventQueue_Count(WOLF_EVENT_QUEUE* queue);
WOLFSSL_API void wolfEventQueue_Free(WOLF_EVENT_QUEUE* queue);

/* the queue mutex must be locked prior to calling these */
WOLFSSL_API int wolfEventQueue_Add(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event);
WOLFSSL_API int wolfEventQueue_Remove(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event);


#endif /* HAVE_WOLF_EVENT */


#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif /* _WOLF_EVENT_H_ */
