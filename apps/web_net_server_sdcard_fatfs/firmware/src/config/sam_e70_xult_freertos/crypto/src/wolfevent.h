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
/*****************************************************************************
 Copyright (C) 2013-2019 Microchip Technology Inc. and its subsidiaries.

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









//DOM-IGNORE-END
#ifndef _WOLF_EVENT_H_
#define _WOLF_EVENT_H_

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef SINGLE_THREADED
    #include "crypto/src/wc_port.h"
#endif

typedef struct WOLF_EVENT WOLF_EVENT;
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

typedef enum WOLF_EVENT_STATE {
    WOLF_EVENT_STATE_READY,
    WOLF_EVENT_STATE_PENDING,
    WOLF_EVENT_STATE_DONE,
} WOLF_EVENT_STATE;

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
    word64              reqId;
    #ifdef WOLFSSL_NITROX_DEBUG
    word32              pendCount;
    #endif
#endif
#ifndef WC_NO_ASYNC_THREADING
    pthread_t           threadId;
#endif
    int                 ret;    /* Async return code */
    unsigned int        flags;
    WOLF_EVENT_TYPE     type;
    WOLF_EVENT_STATE    state;
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
