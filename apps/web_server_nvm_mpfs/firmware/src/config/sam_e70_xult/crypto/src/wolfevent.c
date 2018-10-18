/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    wolfevent.c
  
  Summary:
    Crypto Framework Library source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic 
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
File Name:  wolfevent.c
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
#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "crypto/src/settings.h"


#ifdef HAVE_WOLF_EVENT

#include "crypto/src/internal.h"
#include "crypto/src/error-ssl.h"
#include "crypto/src/error-crypt.h"

#include "crypto/src/wolfevent.h"


int wolfEvent_Init(WOLF_EVENT* event, WOLF_EVENT_TYPE type, void* context)
{
    if (event == NULL) {
        return BAD_FUNC_ARG;
    }

    if (event->pending) {
        WOLFSSL_MSG("event already pending!");
        return BAD_COND_E;
    }

    XMEMSET(event, 0, sizeof(WOLF_EVENT));
    event->type = type;
    event->context = context;

    return 0;
}

int wolfEvent_Poll(WOLF_EVENT* event, WOLF_EVENT_FLAG flags)
{
    int ret = BAD_COND_E;

    /* Check hardware */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (event->type >= WOLF_EVENT_TYPE_ASYNC_FIRST &&
        event->type <= WOLF_EVENT_TYPE_ASYNC_LAST)
    {
        ret = wolfAsync_EventPoll(event, flags);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}

int wolfEventQueue_Init(WOLF_EVENT_QUEUE* queue)
{
    int ret = 0;

    if (queue == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(queue, 0, sizeof(WOLF_EVENT_QUEUE));
#ifndef SINGLE_THREADED
    ret = wc_InitMutex(&queue->lock);
#endif
    return ret;
}


int wolfEventQueue_Push(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event)
{
    int ret;

    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    /* Setup event */
    event->next = NULL;
    event->pending = 1;

    ret = wolfEventQueue_Add(queue, event);

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    return ret;
}

int wolfEventQueue_Pop(WOLF_EVENT_QUEUE* queue, WOLF_EVENT** event)
{
    int ret = 0;

    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    /* In single threaded mode "event_queue.lock" doesn't exist */
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    /* Pop first item off queue */
    *event = queue->head;
    ret = wolfEventQueue_Remove(queue, *event);

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    return ret;
}

/* assumes queue is locked by caller */
int wolfEventQueue_Add(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event)
{
    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

    if (queue->tail == NULL)  {
        queue->head = event;
    }
    else {
        queue->tail->next = event;
        event->prev = queue->tail;
    }
    queue->tail = event;      /* add to the end either way */
    queue->count++;

    return 0;
}

/* assumes queue is locked by caller */
int wolfEventQueue_Remove(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event)
{
    int ret = 0;

    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

    if (event == queue->head && event == queue->tail) {
        queue->head = NULL;
        queue->tail = NULL;
    }
    else if (event == queue->head) {
        queue->head = event->next;
        queue->head->prev = NULL;
    }
    else if (event == queue->tail) {
        queue->tail = event->prev;
        queue->tail->next = NULL;
    }
    else {
        WOLF_EVENT* next = event->next;
        WOLF_EVENT* prev = event->prev;
        next->prev = prev;
        prev->next = next;
    }
    queue->count--;

    return ret;
}

int wolfEventQueue_Poll(WOLF_EVENT_QUEUE* queue, void* context_filter,
    WOLF_EVENT** events, int maxEvents, WOLF_EVENT_FLAG flags, int* eventCount)
{
    WOLF_EVENT* event;
    int ret = 0, count = 0;

    if (queue == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    /* In single threaded mode "event_queue.lock" doesn't exist */
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    /* itterate event queue */
    for (event = queue->head; event != NULL; event = event->next)
    {
        /* optional filter based on context */
        if (context_filter == NULL || event->context == context_filter) {

            /* poll event */
            ret = wolfEvent_Poll(event, flags);
            if (ret < 0) break; /* exit for */

            /* If event is done then process */
            if (event->done) {
                /* remove from queue */
                ret = wolfEventQueue_Remove(queue, event);
                if (ret < 0) break; /* exit for */

                /* return pointer in 'events' arg */
                if (events) {
                    events[count] = event; /* return pointer */
                }
                count++;

                /* check to make sure our event list isn't full */
                if (events && count >= maxEvents) {
                    break; /* exit for */
                }
            }
        }
    }

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    /* return number of properly populated events */
    if (eventCount) {
        *eventCount = count;
    }

    return ret;
}

int wolfEventQueue_Count(WOLF_EVENT_QUEUE* queue)
{
    int ret;

    if (queue == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    /* In single threaded mode "event_queue.lock" doesn't exist */
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    ret = queue->count;

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    return ret;
}

void wolfEventQueue_Free(WOLF_EVENT_QUEUE* queue)
{
    if (queue) {
    #ifndef SINGLE_THREADED
        wc_FreeMutex(&queue->lock);
    #endif
    }
}

#endif /* HAVE_WOLF_EVENT */
