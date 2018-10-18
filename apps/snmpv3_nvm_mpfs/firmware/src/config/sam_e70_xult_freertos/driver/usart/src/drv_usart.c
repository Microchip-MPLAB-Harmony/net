/*******************************************************************************
  USART Driver Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart.c

  Summary:
    Source code for the USART driver dynamic implementation.

  Description:
    This file contains the source code for the dynamic implementation of the
    USART driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute Software
only when embedded on a Microchip microcontroller or digital  signal  controller
that is integrated into your product or third party  product  (pursuant  to  the
sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "driver/usart/drv_usart.h"
#include "drv_usart_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
DRV_USART_OBJ gDrvUSARTObj[DRV_USART_INSTANCES_NUMBER] ;

/* This is the array of USART Driver Buffet object. */
DRV_USART_BUFFER_OBJ gDrvUSARTBufferObj[DRV_USART_QUEUE_DEPTH_COMBINED];

/* This a global token counter used to generate unique buffer handles */
static uint16_t gDrvUSARTTokenCount = 0;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static bool _DRV_USART_ValidateClientHandle(DRV_USART_OBJ * object, DRV_HANDLE handle)
{
    if(DRV_HANDLE_INVALID == handle)
    {
        return false;
    }

    object = &gDrvUSARTObj[handle];

    if(false == object->clientInUse)
    {
        return false;
    }

    return true;
}

static bool _DRV_USART_ResourceLock(DRV_USART_OBJ * object)
{
    DRV_USART_OBJ * dObj = object;

    /* Grab a mutex to avoid other threads to modify driver resource
     * asynchronously. */
    if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        return false;
    }

    /* We will disable USART and/or DMA interrupt so that the driver resource
     * is not updated asynchronously. */
    if( (DMA_CHANNEL_NONE != dObj->txDMAChannel) || (DMA_CHANNEL_NONE != dObj->rxDMAChannel))
    {
        SYS_INT_SourceDisable(dObj->interruptDMA);
    }

    SYS_INT_SourceDisable(dObj->interruptUSART);

    return true;
}

static bool _DRV_USART_ResourceUnlock(DRV_USART_OBJ * object)
{
    DRV_USART_OBJ * dObj = object;

    /* Restore the interrupt and release mutex. */
    if( (DMA_CHANNEL_NONE != dObj->txDMAChannel) || (DMA_CHANNEL_NONE != dObj->rxDMAChannel))
    {
        SYS_INT_SourceEnable(dObj->interruptDMA);
    }

    SYS_INT_SourceEnable(dObj->interruptUSART);

    OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));

    return true;
}

static DRV_USART_BUFFER_OBJ * _DRV_USART_BufferObjectGet( void )
{
    unsigned int i = 0;

    /* Search the buffer pool for a free buffer object */
    while(i < DRV_USART_QUEUE_DEPTH_COMBINED)
    {
        if(false == gDrvUSARTBufferObj[i].inUse)
        {
            /* This means this object is free. */
            /* Assign a handle to this buffer. The buffer handle must be unique.
             * To do this, we construct the buffer handle out of the
             * gDrvUSARTTokenCount and allocated buffer index. Note that
             * gDrvUSARTTokenCount is incremented and wrapped around when the
             * value reaches OxFFFF. We do avoid a case where the token value
             * becomes 0xFFFF and the buffer index also becomes 0xFFFF */
            gDrvUSARTBufferObj[i].bufferHandle = _DRV_USART_MAKE_HANDLE(gDrvUSARTTokenCount, i);

            /* Update the token number. */
            _DRV_USART_UPDATE_BUFFER_TOKEN(gDrvUSARTTokenCount);

            return &gDrvUSARTBufferObj[i];
        }

        i++;
    }

    /* This means we could not find a buffer. This will happen if the the
     * DRV_USART_QUEUE_DEPTH_COMBINED parameter is configured to be less */
    return NULL;
}

static void _DRV_USART_BufferObjectRelease( DRV_USART_BUFFER_OBJ * object )
{
    DRV_USART_BUFFER_OBJ *bufferObj = object;

    /* Reset the buffer object */
    bufferObj->inUse = false;
    bufferObj->next = NULL;
    bufferObj->currentState = DRV_USART_BUFFER_IS_FREE;
}

static bool _DRV_USART_WriteBufferQueuePurge( DRV_USART_OBJ * object )
{
    DRV_USART_OBJ * dObj = object;
    DRV_USART_BUFFER_OBJ * iterator = NULL;
    DRV_USART_BUFFER_OBJ * nextBufferObj = NULL;

    _DRV_USART_ResourceLock(dObj);

    iterator = dObj->queueWrite;

    while(iterator != NULL)
    {
        nextBufferObj = iterator->next;
        _DRV_USART_BufferObjectRelease(iterator);
        iterator = nextBufferObj;
    }

    /* Make the head pointer to NULL */
    dObj->queueSizeCurrentWrite = 0;
    dObj->queueWrite = NULL;

    _DRV_USART_ResourceUnlock(dObj);

    return true;
}

static bool _DRV_USART_ReadBufferQueuePurge( DRV_USART_OBJ * object )
{
    DRV_USART_OBJ * dObj = object;
    DRV_USART_BUFFER_OBJ * iterator = NULL;
    DRV_USART_BUFFER_OBJ * nextBufferObj = NULL;

    _DRV_USART_ResourceLock(dObj);

    iterator = dObj->queueRead;

    while(iterator != NULL)
    {
        nextBufferObj = iterator->next;
        _DRV_USART_BufferObjectRelease(iterator);
        iterator = nextBufferObj;
    }

    /* Make the head pointer to NULL */
    dObj->queueSizeCurrentRead = 0;
    dObj->queueRead = NULL;

    _DRV_USART_ResourceUnlock(dObj);

    return true;
}

static void _DRV_USART_BufferQueueTask( DRV_USART_OBJ *object, DRV_USART_DIRECTION direction, DRV_USART_BUFFER_EVENT event)
{
    DRV_USART_OBJ * dObj = object;
    DRV_USART_BUFFER_OBJ *currentObj = NULL;
    DRV_USART_BUFFER_OBJ *newObj = NULL;

    if((false == dObj->inUse) || (SYS_STATUS_READY != dObj->status))
    {
        return;
    }

    /* Get the buffer object at queue head */
    if(DRV_USART_DIRECTION_RX == direction)
    {
        currentObj = dObj->queueRead;
    }
    else if(DRV_USART_DIRECTION_TX == direction)
    {
        currentObj = dObj->queueWrite;
    }

    if(currentObj != NULL)
    {

        currentObj->status = event;

        if(DRV_USART_BUFFER_EVENT_ERROR == currentObj->status)
        {
            if( (DMA_CHANNEL_NONE != dObj->rxDMAChannel))
            {
                /* DMA mode doesn't return number of bytes completed in case of
                 * an error. */
            }
            else
            {
                currentObj->nCount = dObj->usartPlib->readCountGet();
            }
        }
        else
        {
            /* Buffer transfer was successful, hence set completed bytes to
             * requested buffer size */
            currentObj->nCount = currentObj->size;
        }

        if((dObj->eventHandler != NULL))
        {
            dObj->eventHandler(currentObj->status, currentObj->bufferHandle, dObj->context);
        }

        /* Get the next buffer object in the queue and deallocate the current
         * buffer */
        newObj = currentObj->next;
        _DRV_USART_BufferObjectRelease(currentObj);

        /* Update the new buffer object head and submit it to the PLIB */
        if(DRV_USART_DIRECTION_RX == direction)
        {
            dObj->queueRead = newObj;
            dObj->queueSizeCurrentRead --;
            if (newObj != NULL)
            {
                newObj->currentState = DRV_USART_BUFFER_IS_PROCESSING;

                if( (DMA_CHANNEL_NONE != dObj->rxDMAChannel))
                {
                    SYS_DMA_ChannelTransfer(dObj->rxDMAChannel, (const void *)dObj->rxAddress, (const void *)newObj->buffer, newObj->size);
                }
                else
                {
                    dObj->usartPlib->read(newObj->buffer, newObj->size);
                }
            }
        }
        else if(DRV_USART_DIRECTION_TX == direction)
        {
            dObj->queueWrite = newObj;
            dObj->queueSizeCurrentWrite --;
            if (newObj != NULL)
            {
                newObj->currentState = DRV_USART_BUFFER_IS_PROCESSING;

                if( (DMA_CHANNEL_NONE != dObj->txDMAChannel))
                {
                    SYS_DMA_ChannelTransfer(dObj->txDMAChannel, (const void *)newObj->buffer, (const void *)dObj->txAddress, newObj->size);
                }
                else
                {
                    dObj->usartPlib->write(newObj->buffer, newObj->size);
                }
            }
        }
    }
    else
    {
        /* Queue purge has been called. Do nothing. */
    }

    return;
}

static void _DRV_USART_TX_PLIB_CallbackHandler( uintptr_t context )
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;

    _DRV_USART_BufferQueueTask(dObj, DRV_USART_DIRECTION_TX, DRV_USART_BUFFER_EVENT_COMPLETE);

    return;
}

static void _DRV_USART_RX_PLIB_CallbackHandler( uintptr_t context )
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;

    dObj->errors = dObj->usartPlib->errorGet();

    if(DRV_USART_ERROR_NONE == dObj->errors)
    {
        _DRV_USART_BufferQueueTask(dObj, DRV_USART_DIRECTION_RX, DRV_USART_BUFFER_EVENT_COMPLETE);
    }
    else
    {
        _DRV_USART_BufferQueueTask(dObj, DRV_USART_DIRECTION_RX, DRV_USART_BUFFER_EVENT_ERROR);
    }

    return;
}

static void _DRV_USART_TX_DMA_CallbackHandler(SYS_DMA_TRANSFER_EVENT event, uintptr_t context)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;

    if(SYS_DMA_TRANSFER_COMPLETE == event)
    {
        _DRV_USART_BufferQueueTask(dObj, DRV_USART_DIRECTION_TX, DRV_USART_BUFFER_EVENT_COMPLETE);
    }
    else if(SYS_DMA_TRANSFER_ERROR == event)
    {
        _DRV_USART_BufferQueueTask(dObj, DRV_USART_DIRECTION_TX, DRV_USART_BUFFER_EVENT_ERROR);
    }

    return;
}

static void _DRV_USART_RX_DMA_CallbackHandler(SYS_DMA_TRANSFER_EVENT event, uintptr_t context)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;

    if(SYS_DMA_TRANSFER_COMPLETE == event)
    {
        _DRV_USART_BufferQueueTask(dObj, DRV_USART_DIRECTION_RX, DRV_USART_BUFFER_EVENT_COMPLETE);
    }
    else if(SYS_DMA_TRANSFER_ERROR == event)
    {
        _DRV_USART_BufferQueueTask(dObj, DRV_USART_DIRECTION_RX, DRV_USART_BUFFER_EVENT_ERROR);
    }

    return;
}

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
SYS_MODULE_OBJ DRV_USART_Initialize( const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT * const init )
{
    DRV_USART_OBJ *dObj = NULL;
    DRV_USART_INIT *usartInit = (DRV_USART_INIT *)init ;

    /* Validate the request */
    if(DRV_USART_INSTANCES_NUMBER <= drvIndex)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if(false != gDrvUSARTObj[drvIndex].inUse)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Allocate the driver object */
    dObj = &gDrvUSARTObj[drvIndex];
    dObj->inUse = true;
    dObj->clientInUse           = false;
    dObj->usartPlib             = usartInit->usartPlib;
    dObj->queueSizeRead         = usartInit->queueSizeReceive;
    dObj->queueSizeWrite        = usartInit->queueSizeTransmit;
    dObj->interruptUSART        = usartInit->interruptUSART;
    dObj->queueSizeCurrentRead  = 0;
    dObj->queueSizeCurrentWrite = 0;
    dObj->queueRead             = NULL;
    dObj->queueWrite            = NULL;
    dObj->txDMAChannel          = usartInit->dmaChannelTransmit;
    dObj->rxDMAChannel          = usartInit->dmaChannelReceive;
    dObj->txAddress             = usartInit->usartTransmitAddress;
    dObj->rxAddress             = usartInit->usartReceiveAddress;
    dObj->interruptDMA          = usartInit->interruptDMA;

    /* Create the Mutexes needed for RTOS mode. These calls always passes in the
     * non-RTOS mode */
    if(OSAL_RESULT_TRUE != OSAL_MUTEX_Create(&(dObj->mutexDriverInstance)))
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Register a callback with either DMA or USART PLIB based on configuration.
     * dObj is used as a context parameter, that will be used to distinguish the
     * events for different driver instances. */
    if(DMA_CHANNEL_NONE != dObj->txDMAChannel)
    {
        SYS_DMA_ChannelCallbackRegister(dObj->txDMAChannel, _DRV_USART_TX_DMA_CallbackHandler, (uintptr_t)dObj);
    }
    else
    {
        dObj->usartPlib->writeCallbackRegister(_DRV_USART_TX_PLIB_CallbackHandler, (uintptr_t)dObj);
        (void)_DRV_USART_TX_DMA_CallbackHandler;
    }

    if(DMA_CHANNEL_NONE != dObj->rxDMAChannel)
    {
        SYS_DMA_ChannelCallbackRegister(dObj->rxDMAChannel, _DRV_USART_RX_DMA_CallbackHandler, (uintptr_t)dObj);
    }
    else
    {
        dObj->usartPlib->readCallbackRegister(_DRV_USART_RX_PLIB_CallbackHandler, (uintptr_t)dObj);
        (void)_DRV_USART_RX_DMA_CallbackHandler;
    }

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

// *****************************************************************************
SYS_STATUS DRV_USART_Status( SYS_MODULE_OBJ object)
{
    /* Validate the request */
    if( (SYS_MODULE_OBJ_INVALID == object) || (DRV_USART_INSTANCES_NUMBER <= object) )
    {
        return SYS_STATUS_UNINITIALIZED;
    }

    return (gDrvUSARTObj[object].status);
}

// *****************************************************************************
DRV_HANDLE DRV_USART_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    DRV_USART_OBJ *dObj = NULL;

    /* Validate the request */
    if (DRV_USART_INSTANCES_NUMBER <= drvIndex)
    {
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvUSARTObj[drvIndex];

    if((SYS_STATUS_READY != dObj->status) || (false == dObj->inUse))
    {
        return DRV_HANDLE_INVALID;
    }

    if(true == dObj->clientInUse)
    {
        /* This driver supports only one client per instance */
        return DRV_HANDLE_INVALID;
    }

    /* Grab client object here */
    dObj->clientInUse  = true;
    dObj->eventHandler = NULL;
    dObj->context      = (uintptr_t)NULL;

    /* Driver index is the handle */
    return (DRV_HANDLE)drvIndex;
}

// *****************************************************************************
void DRV_USART_Close( DRV_HANDLE handle )
{
    DRV_USART_OBJ * dObj = NULL;

    /* Validate the request */
    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return;
    }

    dObj = &gDrvUSARTObj[handle];

    if(false == _DRV_USART_WriteBufferQueuePurge(dObj))
    {
        return;
    }

    if(false == _DRV_USART_ReadBufferQueuePurge(dObj))
    {
        return;
    }

    dObj->clientInUse = false;

    return;
}

DRV_USART_ERROR DRV_USART_ErrorGet( const DRV_HANDLE handle )
{
    DRV_USART_OBJ * dObj = NULL;
    DRV_USART_ERROR errors = DRV_USART_ERROR_NONE;

    /* Validate the request */
    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return DRV_USART_ERROR_NONE;
    }

    dObj = &gDrvUSARTObj[handle];
    errors = dObj->errors;
    dObj->errors = DRV_USART_ERROR_NONE;

    return errors;
}

bool DRV_USART_SerialSetup( const DRV_HANDLE handle, DRV_USART_SERIAL_SETUP* setup )
{
    DRV_USART_OBJ * dObj = NULL;

    /* Validate the request */
    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return DRV_USART_ERROR_NONE;
    }

    /* Clock source cannot be modified dynamically, so passing the '0' to pick
     * the configured clock source value */
    return dObj->usartPlib->serialSetup(setup, 0);
}

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Buffer Queue Interface Implementation
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
void DRV_USART_BufferEventHandlerSet( const DRV_HANDLE handle, const DRV_USART_BUFFER_EVENT_HANDLER eventHandler, const uintptr_t context )
{
    DRV_USART_OBJ * dObj = NULL;

    /* Validate the Request */
    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return;
    }

    dObj = &gDrvUSARTObj[handle];

    dObj->eventHandler = eventHandler;
    dObj->context = context;

    return;
}

// *****************************************************************************
void DRV_USART_WriteBufferAdd( DRV_HANDLE handle, void * buffer, const size_t size, DRV_USART_BUFFER_HANDLE * bufferHandle)
{
    DRV_USART_OBJ * dObj = NULL;
    DRV_USART_BUFFER_OBJ * bufferObj = NULL;
    DRV_USART_BUFFER_OBJ * iterator = NULL;

    /* Validate the Request */
    if (bufferHandle == NULL)
    {
        return;
    }

    *bufferHandle = DRV_USART_BUFFER_HANDLE_INVALID;

    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return;
    }

    dObj = &gDrvUSARTObj[handle];

    if((size == 0) || (NULL == buffer))
    {
        return;
    }

    if(dObj->queueSizeCurrentWrite >= dObj->queueSizeWrite)
    {
        return;
    }

    _DRV_USART_ResourceLock(dObj);

    /* Search the buffer pool for a free buffer object */
    bufferObj = _DRV_USART_BufferObjectGet();

    if(NULL == bufferObj)
    {
        /* Couldn't able to get the buffer object */
        _DRV_USART_ResourceUnlock(dObj);
        return;
    }

    /* Configure the buffer object */
    bufferObj->size         = size;
    bufferObj->nCount       = 0;
    bufferObj->inUse        = true;
    bufferObj->buffer       = buffer;
    bufferObj->dObj         = dObj;
    bufferObj->next         = NULL;
    bufferObj->currentState = DRV_USART_BUFFER_IS_IN_QUEUE;

    *bufferHandle = bufferObj->bufferHandle;

    dObj->queueSizeCurrentWrite ++;

    if(dObj->queueWrite == NULL)
    {
        /* This is the first buffer in the queue */
        dObj->queueWrite = bufferObj;

        /* Because this is the first buffer in the queue, we need to submit the
         * buffer to the PLIB to start processing. */
        bufferObj->currentState = DRV_USART_BUFFER_IS_PROCESSING;

        if( (DMA_CHANNEL_NONE != dObj->txDMAChannel))
        {
            SYS_DMA_ChannelTransfer(dObj->txDMAChannel, (const void *)bufferObj->buffer, (const void *)dObj->txAddress, bufferObj->size);
        }
        else
        {
            dObj->usartPlib->write(bufferObj->buffer, bufferObj->size);
        }
    }
    else
    {
        /* This means the write queue is not empty. We must add
         * the buffer object to the end of the queue */
        iterator = dObj->queueWrite;
        while(iterator->next != NULL)
        {
            /* Get the next buffer object */
            iterator = iterator->next;
        }

        iterator->next = bufferObj;
    }

    _DRV_USART_ResourceUnlock(dObj);

    return;
}

// *****************************************************************************
void DRV_USART_ReadBufferAdd( DRV_HANDLE handle, void * buffer, const size_t size, DRV_USART_BUFFER_HANDLE * bufferHandle)
{
    DRV_USART_OBJ * dObj = NULL;
    DRV_USART_BUFFER_OBJ * bufferObj = NULL;
    DRV_USART_BUFFER_OBJ * iterator = NULL;

    /* Validate the Request */
    if (bufferHandle == NULL)
    {
        return;
    }

    *bufferHandle = DRV_USART_BUFFER_HANDLE_INVALID;

    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return;
    }

    dObj = &gDrvUSARTObj[handle];

    if((size == 0) || (NULL == buffer))
    {
        return;
    }

    if(dObj->queueSizeCurrentRead >= dObj->queueSizeRead)
    {
        return;
    }

    _DRV_USART_ResourceLock(dObj);

    /* Search the buffer pool for a free buffer object */
    bufferObj = _DRV_USART_BufferObjectGet();

    if(NULL == bufferObj)
    {
        /* Couldn't able to get the buffer object */
        _DRV_USART_ResourceUnlock(dObj);
        return;
    }

    /* Configure the buffer object */
    bufferObj->size            = size;
    bufferObj->nCount          = 0;
    bufferObj->inUse           = true;
    bufferObj->buffer          = buffer;
    bufferObj->dObj            = dObj;
    bufferObj->next            = NULL;
    bufferObj->currentState    = DRV_USART_BUFFER_IS_IN_QUEUE;

    *bufferHandle = bufferObj->bufferHandle;

    dObj->queueSizeCurrentRead ++;

    if(dObj->queueRead == NULL)
    {
        /* This is the first buffer in the queue */
        dObj->queueRead = bufferObj;

        /* Because this is the first buffer in the queue, we need to submit the
         * buffer to the PLIB to start processing. */
        bufferObj->currentState    = DRV_USART_BUFFER_IS_PROCESSING;

        if( (DMA_CHANNEL_NONE != dObj->rxDMAChannel))
        {
            SYS_DMA_ChannelTransfer(dObj->rxDMAChannel, (const void *)dObj->rxAddress, (const void *)bufferObj->buffer, bufferObj->size);
        }
        else
        {
            dObj->usartPlib->read(bufferObj->buffer, bufferObj->size);
        }
    }
    else
    {
        /* This means the write queue is not empty. We must add
         * the buffer object to the end of the queue */
        iterator = dObj->queueRead;
        while(iterator->next != NULL)
        {
            /* Get the next buffer object */
            iterator = iterator->next;
        }
        iterator->next = bufferObj;
    }

    _DRV_USART_ResourceUnlock(dObj);

    return;
}

// *****************************************************************************
size_t DRV_USART_BufferCompletedBytesGet( DRV_USART_BUFFER_HANDLE bufferHandle )
{
    DRV_USART_OBJ *dObj = NULL;
    DRV_USART_BUFFER_OBJ * bufferObj = NULL;
    size_t processedBytes = DRV_USART_BUFFER_HANDLE_INVALID;

    /* Validate the Request */
    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        return DRV_USART_BUFFER_HANDLE_INVALID;
    }

    /* The buffer index is the contained in the lower 16 bits of the buffer
     * handle */
    bufferObj = &gDrvUSARTBufferObj[bufferHandle & _DRV_USART_BUFFER_TOKEN_MAX];

    dObj = bufferObj->dObj;

    /* Check if the buffer is currently submitted to PLIB */
    if( bufferObj->currentState == DRV_USART_BUFFER_IS_PROCESSING )
    {
        /* Get the number of bytes processed by PLIB. */
        if(dObj->queueWrite == bufferObj)
        {
            processedBytes = dObj->usartPlib->writeCountGet();
        }
        else if(dObj->queueRead == bufferObj)
        {
            processedBytes = dObj->usartPlib->readCountGet();
        }
    }
    else
    {
        /* Buffer is not with PLIB, so get the nCount of buffer object */
        processedBytes = bufferObj->nCount;
    }

    /* Return the processed number of bytes..
     * If the buffer handle is invalid or bufferObj is expired
     * then return DRV_USART_BUFFER_HANDLE_INVALID */
    return processedBytes;
}

// *****************************************************************************
DRV_USART_BUFFER_EVENT DRV_USART_BufferStatusGet( const DRV_USART_BUFFER_HANDLE bufferHandle )
{
    DRV_USART_BUFFER_OBJ * bufferObj = NULL;

    /* Validate the Request */
    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        return DRV_USART_BUFFER_EVENT_ERROR;
    }

    /* The buffer index is the contained in the lower 16 bits of the buffer
     * handle */
    bufferObj = &gDrvUSARTBufferObj[bufferHandle & _DRV_USART_BUFFER_TOKEN_MAX];

    return bufferObj->status;
}

// *****************************************************************************
bool DRV_USART_WriteQueuePurge( const DRV_HANDLE handle )
{
    DRV_USART_OBJ * dObj = NULL;

    /* Validate the Request */
    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return false;
    }

    dObj = &gDrvUSARTObj[handle];

    return _DRV_USART_WriteBufferQueuePurge(dObj);
}

// *****************************************************************************
bool DRV_USART_ReadQueuePurge( const DRV_HANDLE handle )
{
    DRV_USART_OBJ * dObj = NULL;

    /* Validate the Request */
    if( false == _DRV_USART_ValidateClientHandle(dObj, handle))
    {
        return false;
    }

    dObj = &gDrvUSARTObj[handle];

    return _DRV_USART_ReadBufferQueuePurge(dObj);
}
