/*******************************************************************************
  I2C Driver Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c.c

  Summary:
    Source code for the I2C driver dynamic implementation.

  Description:
    This file contains the source code for the dynamic implementation of the
    I2C driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
//#include "system/debug/sys_debug.h"
#include "driver/i2c/drv_i2c.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
static DRV_I2C_OBJ gDrvI2CObj[DRV_I2C_INSTANCES_NUMBER] ;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static inline uint32_t  DRV_I2C_MAKE_HANDLE(uint16_t token, uint8_t drvIndex, uint8_t index)
{
    return ((token << 16) | (drvIndex << 8) | index);
}

static inline uint16_t DRV_I2C_UPDATE_TOKEN(uint16_t token)
{
    token++;
    if (token >= DRV_I2C_TOKEN_MAX)
    {
        token = 1;
    }

    return token;
}

static bool DRV_I2C_ResourceLock(DRV_I2C_OBJ * dObj)
{
    /* We will allow buffers to be added in the interrupt
       context of this I2C driver. But we must make
       sure that if we are in interrupt, then we should
       not modify mutexes. */
    if(dObj->interruptNestingCount == 0)
    {
        /* Grab a mutex. This is okay because we are not in an
           interrupt context */
        if(OSAL_MUTEX_Lock(&(dObj->mutexTransferObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* We will disable interrupts so that the queue
               status does not get updated asynchronously.
               This code will always execute. */
            SYS_INT_SourceDisable(dObj->interruptI2C);

            return true;
        }
        else
        {
            /* The mutex acquisition timed out. Return with an
               invalid handle. This code will not execute
               if there is no RTOS. */
            return false;
        }
    }

    return true;
}

static void DRV_I2C_ResourceUnlock(DRV_I2C_OBJ * dObj)
{
    /* Restore the interrupt if it was enabled */
    SYS_INT_SourceEnable(dObj->interruptI2C);

    if(dObj->interruptNestingCount == 0)
    {
        /* Release mutex */
        OSAL_MUTEX_Unlock(&(dObj->mutexTransferObjects));
    }
}

static void DRV_I2C_TransferObjectsInit( DRV_I2C_OBJ * dObj )
{
    uint32_t i;

    dObj->trObjFree = dObj->trObjArr;
    dObj->trObjFree[0].index = 0;

    if(dObj->trQueueSize == 1)
    {
        return;
    }

    for(i = 1; i < dObj->trQueueSize; i++ )
    {
        dObj->trObjFree[i-1].next = &dObj->trObjFree[i];
        dObj->trObjFree[i].index = i;
    }
}

static DRV_I2C_CLIENT_OBJ * DRV_I2C_DriverHandleValidate(DRV_HANDLE handle)
{
    /* This function returns the pointer to the client object that is
       associated with this handle if the handle is valid. Returns NULL
       otherwise. */

    DRV_I2C_CLIENT_OBJ * client;

    if((handle == DRV_HANDLE_INVALID) || (handle == 0))
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Handle");
        return(NULL);
    }

    client = (DRV_I2C_CLIENT_OBJ *)handle;

    if(!client->inUse)
    {
        return(NULL);
    }

    return(client);
}

static bool DRV_I2C_TransferQueueFlush( DRV_I2C_CLIENT_OBJ * clientObj )
{
    DRV_I2C_OBJ * dObj = &gDrvI2CObj[clientObj->drvIndex];
    DRV_I2C_TRANSFER_OBJ * current = NULL;
    DRV_I2C_TRANSFER_OBJ * previous = NULL;
    DRV_I2C_TRANSFER_OBJ * dirty = NULL;
    bool interruptWasEnabled = false;

    /* Disable the transmit interrupt */
    interruptWasEnabled = SYS_INT_SourceDisable(dObj->interruptI2C);

    current = dObj->trQueueHead;
    while(current != NULL)
    {
        if(clientObj == (DRV_I2C_CLIENT_OBJ *)current->hClient)
        {
            /* That means this transfer object is owned
               by this client. This transfer object should
               be removed. The following code removes
               the object from a linked list queue. */
            dirty = current;

            if(previous == NULL)
            {
                dObj->trQueueHead = current->next;
                previous = current;
            }
            else
            {
                previous->next = current->next;
            }

            current = current->next;

            /* return the dirty object to the free list */
            dirty->next = dObj->trObjFree;
            dObj->trObjFree = dirty;
        }
        else
        {
            previous = current;
            current = current->next;
        }
    }

    if( (DRV_I2C_TRANSFER_OBJ *)NULL != dObj->trQueueHead )
    {
        dObj->trQueueTail = (DRV_I2C_TRANSFER_OBJ *)NULL;
    }
    else
    {
        dObj->trQueueTail = previous;
    }

    /* Re-enable the interrupt if it was enabled */
    if(interruptWasEnabled)
    {
        SYS_INT_SourceEnable(dObj->interruptI2C);
    }

    return true;
}

static void DRV_I2C_PLibCallbackHandler( uintptr_t contextHandle )
{
    DRV_I2C_OBJ            * dObj             = (DRV_I2C_OBJ *)            contextHandle;
    DRV_I2C_CLIENT_OBJ     * client           = (DRV_I2C_CLIENT_OBJ *)     NULL;
    DRV_I2C_TRANSFER_OBJ   * transferObj      = (DRV_I2C_TRANSFER_OBJ *)   NULL;

    if((!dObj->inUse) || (dObj->status != SYS_STATUS_READY))
    {
        /* This instance of the driver is not initialized. Don't
         * do anything */
        return;
    }

    transferObj = dObj->trQueueHead;
    client = (DRV_I2C_CLIENT_OBJ *)transferObj->hClient;

    client->errors = dObj->i2cPlib->errorGet();

    if(client->errors == DRV_I2C_ERROR_NONE)
    {
        transferObj->event = DRV_I2C_TRANSFER_EVENT_COMPLETE;
    }
    else
    {
        transferObj->event = DRV_I2C_TRANSFER_EVENT_ERROR;
    }

    if(client->eventHandler != NULL)
    {
        /* Before calling the event handler, the interrupt nesting
           counter is incremented. This will allow driver routine that
           are called from the event handler to know the interrupt
           nesting level. Events are only generated for buffers that
           were submitted using the buffer add routine */

        dObj->interruptNestingCount ++;

        client->eventHandler( transferObj->event,
                              transferObj->transferHandle,
                              client->context );

        /* Decrement the nesting count */
        dObj->interruptNestingCount -- ;
    }

    /* Get the next buffer in the queue */
    dObj->trQueueHead = dObj->trQueueHead->next;

    /* return the processed buffer to free list */
    transferObj->next = dObj->trObjFree;
    dObj->trObjFree = transferObj;

    /* Process the next transfer in queue */
    if(dObj->trQueueHead != (DRV_I2C_TRANSFER_OBJ *)NULL)
    {
        transferObj = dObj->trQueueHead;

        switch(transferObj->flag)
        {
            case DRV_I2C_TRANSFER_OBJ_FLAG_READ:
            {
                dObj->i2cPlib->read(transferObj->slaveAddress, transferObj->readBuffer, transferObj->readSize);
                break;
            }

            case DRV_I2C_TRANSFER_OBJ_FLAG_WRITE:
            {
                dObj->i2cPlib->write(transferObj->slaveAddress, transferObj->writeBuffer, transferObj->writeSize);
                break;
            }

            case DRV_I2C_TRANSFER_OBJ_FLAG_WRITE_READ:
            {
                dObj->i2cPlib->writeRead(transferObj->slaveAddress, transferObj->writeBuffer, transferObj->writeSize, transferObj->readBuffer, transferObj->readSize);
                break;
            }
        }
    }
    else
    {
        dObj->trQueueTail = NULL;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_I2C_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Dynamic implementation of DRV_I2C_Initialize system interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_Initialize system interface
    function.

  Remarks:
    See drv_i2c.h for usage information.
*/

SYS_MODULE_OBJ DRV_I2C_Initialize( const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT * const init )
{
    DRV_I2C_OBJ *dObj     = (DRV_I2C_OBJ *)NULL;
    DRV_I2C_INIT *i2cInit = (DRV_I2C_INIT *)init;

    /* Validate the request */
    if(drvIndex >= DRV_I2C_INSTANCES_NUMBER)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid driver instance");
        return SYS_MODULE_OBJ_INVALID;
    }

    if(gDrvI2CObj[drvIndex].inUse == true)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Instance already in use");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Allocate the driver object */
    dObj = &gDrvI2CObj[drvIndex];

    /* create mutexes */
    if(OSAL_MUTEX_Create(&(dObj->mutexClientObjects)) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    if(OSAL_MUTEX_Create(&(dObj->mutexTransferObjects)) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    dObj->inUse = true;

    /* Update the driver parameters */
    dObj->i2cPlib                     = i2cInit->i2cPlib;
    dObj->interruptI2C                = i2cInit->interruptI2C;
    dObj->clientObjPool               = i2cInit->clientObjPool;
    dObj->nClientsMax                 = i2cInit->numClients;
    dObj->trObjArr                    = (DRV_I2C_TRANSFER_OBJ *)i2cInit->transferObj;
    dObj->trQueueSize                 = i2cInit->queueSize;
    dObj->trObjFree                   = (DRV_I2C_TRANSFER_OBJ *)NULL;
    dObj->trQueueHead                 = (DRV_I2C_TRANSFER_OBJ *)NULL;
    dObj->trQueueTail                 = (DRV_I2C_TRANSFER_OBJ *)NULL;
    dObj->nClients                    = 0;
    dObj->isExclusive                 = false;
    dObj->interruptNestingCount       = 0;
    dObj->tokenCount                  = 1;

    DRV_I2C_TransferObjectsInit(dObj);

    /* Register a callback with PLIB.
     * dObj as a context parameter will be used to distinguish the events
     * from different instances. */
    dObj->i2cPlib->callbackRegister(DRV_I2C_PLibCallbackHandler, (uintptr_t)dObj);

    /* Enable the system interrupt flag */
    SYS_INT_SourceEnable(dObj->interruptI2C);

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_I2C_Status( SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_I2C_Status system interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_Status system interface
    function.

  Remarks:
    See drv_i2c.h for usage information.
*/

SYS_STATUS DRV_I2C_Status( SYS_MODULE_OBJ object)
{
    /* Validate the request */
    if( (object == SYS_MODULE_OBJ_INVALID) || (object >= DRV_I2C_INSTANCES_NUMBER) )
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid system object handle");
        return SYS_STATUS_UNINITIALIZED;
    }

    return (gDrvI2CObj[object].status);
}

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_I2C_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT    ioIntent )

  Summary:
    Dynamic implementation of DRV_I2C_Open client interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_Open client interface
    function.

  Remarks:
    See drv_i2c.h for usage information.
*/

DRV_HANDLE DRV_I2C_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    DRV_I2C_CLIENT_OBJ *clientObj;
    DRV_I2C_OBJ *dObj = NULL;
    unsigned int iClient;

    /* Validate the request */
    if (drvIndex >= DRV_I2C_INSTANCES_NUMBER)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Instance");
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvI2CObj[drvIndex];

    if(OSAL_MUTEX_Lock(&(dObj->mutexClientObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
    {
        return DRV_HANDLE_INVALID;
    }

    if((dObj->status != SYS_STATUS_READY) || (dObj->inUse == false))
    {
        OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));
        SYS_DEBUG(SYS_ERROR_ERROR, "Was the driver initialized?");
        return DRV_HANDLE_INVALID;
    }

    if(dObj->isExclusive)
    {
        OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));
        /* This means the another client has opened the driver in exclusive
           mode. The driver cannot be opened again */
        return DRV_HANDLE_INVALID;
    }

    if((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE))
    {
        OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));
        /* This means the driver was already opened and another driver was
           trying to open it exclusively.  We cannot give exclusive access in
           this case */
        return(DRV_HANDLE_INVALID);
    }

    for(iClient = 0; iClient != dObj->nClientsMax; iClient++)
    {
        clientObj = &((DRV_I2C_CLIENT_OBJ *)dObj->clientObjPool)[iClient];

        if(!clientObj->inUse)
        {
            /* This means we have a free client object to use */
            clientObj->inUse        = true;

            if(ioIntent & DRV_IO_INTENT_EXCLUSIVE)
            {
                /* Set the driver exclusive flag */
                dObj->isExclusive = true;
            }

            dObj->nClients ++;

            /* We have found a client object. Release the mutex */
            OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

            clientObj->drvIndex     = drvIndex;

            /* In a case where the driver is configured for polled
               and bare metal operation, it will not support blocking operation */

            clientObj->ioIntent     = (ioIntent | DRV_IO_INTENT_NONBLOCKING);
            clientObj->eventHandler = NULL;
            clientObj->context      = (uintptr_t)NULL;
            clientObj->errors        = DRV_I2C_ERROR_NONE;

            return ((DRV_HANDLE) clientObj );
        }
    }

    /* Could not find a client object. Release the mutex and
       return with an invalid handle. */
    OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

    /* If we have reached here, it means either we could not find a spare
       client object or the mutex timed out in a RTOS environment. */

    return DRV_HANDLE_INVALID;
}

// *****************************************************************************
/* Function:
    void DRV_I2C_Close ( DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_I2C_Close client interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_Close client interface
    function.

  Remarks:
    See drv_i2c.h for usage information.
*/

void DRV_I2C_Close( DRV_HANDLE handle )
{
    /* This function closes the client, The client
       object is deallocated and returned to the
       pool. */

    DRV_I2C_CLIENT_OBJ * clientObj;
    DRV_I2C_OBJ * dObj;

    /* Validate the handle */
    clientObj = DRV_I2C_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        return;
    }

    dObj = &gDrvI2CObj[clientObj->drvIndex];

    if(OSAL_MUTEX_Lock(&(dObj->mutexClientObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
    {
        return;
    }

    /* Remove all buffers that this client owns from the driver queue. This
       function will map to _DRV_I2C_ClientBufferQueueObjectsRemove() if the
       driver was built for buffer queue support. Else this condition always
       maps to true. */

    if(DRV_I2C_TransferQueueFlush(clientObj) == true)
    {
        /* Reduce the number of clients */
        dObj->nClients --;

        /* Reset the exclusive flag */
        dObj->isExclusive = false;

        clientObj->inUse = false;
    }

    OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

    return;
}

// *****************************************************************************
/* Function:
    void DRV_I2C_TransferEventHandlerSet
    (
        const SYS_MODULE_INDEX index,
        const DRV_I2C_TRANSFER_EVENT_HANDLER eventHandler
    )

  Summary:
    Registers transfer callback function.

  Description:
    This function is used to register the callback function to be invoked
    upon transmission of a buffer.

  Remarks:
    See drv_i2c.h for usage information.
*/

void DRV_I2C_TransferEventHandlerSet( const DRV_HANDLE handle, const DRV_I2C_TRANSFER_EVENT_HANDLER eventHandler, const uintptr_t context )
{
    DRV_I2C_CLIENT_OBJ * client = NULL;
    DRV_I2C_OBJ* dObj = (DRV_I2C_OBJ *)NULL;

    /* Validate the Request */
    if(handle == DRV_HANDLE_INVALID)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Handle");
        return;
    }

    client = (DRV_I2C_CLIENT_OBJ *)handle;

    if(client->inUse == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Handle");
        return;
    }

    dObj = &gDrvI2CObj[client->drvIndex];

    if(DRV_I2C_ResourceLock(dObj) == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Failed to get resource lock");
        return;
    }

    client->eventHandler = eventHandler;
    client->context = context;

    DRV_I2C_ResourceUnlock(dObj);
}

// *****************************************************************************
/* Function:
    DRV_I2C_ERROR DRV_I2C_ErrorGet( const DRV_HANDLE handle )

   Summary:
    Gets the I2C hardware errors associated with the client.

   Description:
    This function returns the errors associated with the given client.
    The call to this function also clears all the associated error flags.

   Remarks:
    See drv_i2c.h for usage information.
*/

DRV_I2C_ERROR DRV_I2C_ErrorGet( const DRV_HANDLE handle )
{
    DRV_I2C_CLIENT_OBJ* clientObj = NULL;
    DRV_I2C_OBJ* dObj = (DRV_I2C_OBJ *)NULL;
    DRV_I2C_ERROR errors = DRV_I2C_ERROR_NONE;

    /* Validate the driver handle */
    clientObj = DRV_I2C_DriverHandleValidate(handle);
    if(clientObj == NULL)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Transfer Handle");
        return DRV_I2C_ERROR_NONE;
    }

    dObj = &gDrvI2CObj[clientObj->drvIndex];

    if(DRV_I2C_ResourceLock(dObj) == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Failed to get resource lock");
        return DRV_I2C_ERROR_NONE;
    }

    errors = clientObj->errors;
    clientObj->errors = DRV_I2C_ERROR_NONE;

    DRV_I2C_ResourceUnlock(dObj);

    return errors;
}

// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver Transfer Queue Interface Implementation
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_I2C_ReadTransferAdd
    (
        const DRV_HANDLE handle,
        void * buffer,
        const size_t size,
        DRV_I2C_TRANSFER_HANDLE * const transferHandle
    )

  Summary:
    Dynamic implementation of DRV_I2C_ReadTransferAdd system interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_ReadTransferAdd system interface
    function.

  Remarks:
    See drv_i2c.h for usage information.
*/

void DRV_I2C_ReadTransferAdd( const DRV_HANDLE handle, const uint16_t address, void * buffer, const size_t size, DRV_I2C_TRANSFER_HANDLE * const transferHandle )
{
    DRV_I2C_CLIENT_OBJ     * clientObj        = (DRV_I2C_CLIENT_OBJ *)     NULL;
    DRV_I2C_OBJ            * hDriver          = (DRV_I2C_OBJ *)            NULL;
    DRV_I2C_TRANSFER_OBJ   * transferObj      = (DRV_I2C_TRANSFER_OBJ *)   NULL;

    *transferHandle = DRV_I2C_TRANSFER_HANDLE_INVALID;

        /* Validate the driver handle */
    clientObj = DRV_I2C_DriverHandleValidate(handle);
    if(clientObj == NULL)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Transfer Handle");
        return;
    }

    /* Validate the Request */
    if (transferHandle == NULL)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "NULL Transfer Handle Pointer");
        return;
    }

    if((size == 0) || (buffer == NULL))
    {
        /* We either got an invalid source pointer or 0 bytes to transfer */
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Transfer Handle");

        return;
    }

    if(clientObj->inUse == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Handle");
        return;
    }

    hDriver = &gDrvI2CObj[clientObj->drvIndex];

    if(DRV_I2C_ResourceLock(hDriver) == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Failed to get resource lock");
        return;
    }

    if(hDriver->trObjFree == (DRV_I2C_TRANSFER_OBJ *) NULL)
    {
        /* This means we could not find a buffer. This
           will happen if the the transfer queue size
           parameter is configured to be less */

        SYS_ASSERT(false, "Insufficient Combined Queue Depth");

        DRV_I2C_ResourceUnlock(hDriver);

        return;
    }

    /* get transfer object from the free list */
    transferObj = hDriver->trObjFree;
    hDriver->trObjFree = hDriver->trObjFree->next;

    /* Configure the object */
    transferObj->slaveAddress = address;
    transferObj->readBuffer   = ( uint8_t *)buffer;
    transferObj->readSize     = size;
    transferObj->writeBuffer  = ( uint8_t *)NULL;
    transferObj->writeSize    = 0;
    transferObj->hClient      = clientObj;
    transferObj->next         = ( DRV_I2C_TRANSFER_OBJ * ) NULL;
    transferObj->event        = DRV_I2C_TRANSFER_EVENT_PENDING;
    transferObj->flag         = DRV_I2C_TRANSFER_OBJ_FLAG_READ;

    /* update transferHandle object with unique Id */
    transferObj->transferHandle = DRV_I2C_MAKE_HANDLE(hDriver->tokenCount, clientObj->drvIndex, transferObj->index);
    *transferHandle = transferObj->transferHandle;

    hDriver->tokenCount = DRV_I2C_UPDATE_TOKEN(hDriver->tokenCount);

    if(hDriver->trQueueHead == NULL)
    {
        /* This is the first buffer in the queue */
        hDriver->trQueueHead = transferObj;
        hDriver->trQueueTail = transferObj;

        /* Because this is the first transfer in the queue, we need to submit the
         * transfer to the PLIB to start processing. */
        hDriver->i2cPlib->read(transferObj->slaveAddress, transferObj->readBuffer, transferObj->readSize);
    }
    else
    {
        /* This means the write queue is not empty. We must add
         * the buffer object to the end of the queue */
        hDriver->trQueueTail->next = transferObj;
        hDriver->trQueueTail = transferObj;
    }

    DRV_I2C_ResourceUnlock(hDriver);

    return;
}

// *****************************************************************************
/* Function:
    void DRV_I2C_WriteTransferAdd
    (
        const DRV_HANDLE handle,
        void * buffer,
        const size_t size,
        DRV_I2C_TRANSFER_HANDLE * const transferHandle
    )

  Summary:
    Dynamic implementation of DRV_I2C_WriteTransferAdd system interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_WriteTransferAdd system interface
    function.

  Remarks:
    See drv_i2c.h for usage information.
*/

void DRV_I2C_WriteTransferAdd( const DRV_HANDLE handle, const uint16_t address, void * buffer, const size_t size, DRV_I2C_TRANSFER_HANDLE * transferHandle )
{
    DRV_I2C_CLIENT_OBJ     * clientObj        = (DRV_I2C_CLIENT_OBJ *)     NULL;
    DRV_I2C_OBJ            * hDriver          = (DRV_I2C_OBJ *)            NULL;
    DRV_I2C_TRANSFER_OBJ   * transferObj      = (DRV_I2C_TRANSFER_OBJ *)   NULL;

    *transferHandle = DRV_I2C_TRANSFER_HANDLE_INVALID;

        /* Validate the driver handle */
    clientObj = DRV_I2C_DriverHandleValidate(handle);
    if(clientObj == NULL)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Transfer Handle");
        return;
    }

    /* Validate the Request */
    if (transferHandle == NULL)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "NULL Transfer Handle Pointer");
        return;
    }

    if((size == 0) || (buffer == NULL))
    {
        /* We either got an invalid source pointer or 0 bytes to transfer */
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Transfer Handle");

        return;
    }

    if(clientObj->inUse == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Handle");
        return;
    }

    hDriver = &gDrvI2CObj[clientObj->drvIndex];

    if(DRV_I2C_ResourceLock(hDriver) == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Failed to get resource lock");
        return;
    }

    if(hDriver->trObjFree == (DRV_I2C_TRANSFER_OBJ*)NULL )
    {
        /* This means we could not find a buffer. This
           will happen if the the transfer queue size
           parameter is configured to be less */

        SYS_ASSERT(false, "Insufficient Combined Queue Depth");

        DRV_I2C_ResourceUnlock(hDriver);

        return;
    }

    /* get transfer object from the free list */
    transferObj = hDriver->trObjFree;
    hDriver->trObjFree = hDriver->trObjFree->next;

    /* Configure the object */
    transferObj->slaveAddress = address;
    transferObj->readBuffer   = ( uint8_t *)NULL;
    transferObj->readSize     = 0;
    transferObj->writeBuffer  = ( uint8_t *)buffer;
    transferObj->writeSize    = size;
    transferObj->hClient      = clientObj;
    transferObj->next         = ( DRV_I2C_TRANSFER_OBJ * ) NULL;
    transferObj->event        = DRV_I2C_TRANSFER_EVENT_PENDING;
    transferObj->flag         = DRV_I2C_TRANSFER_OBJ_FLAG_WRITE;

    /* update transferHandle object with unique Id */
    transferObj->transferHandle = DRV_I2C_MAKE_HANDLE(hDriver->tokenCount, clientObj->drvIndex, transferObj->index);
    *transferHandle = transferObj->transferHandle;

    hDriver->tokenCount = DRV_I2C_UPDATE_TOKEN(hDriver->tokenCount);

    if(hDriver->trQueueHead == NULL)
    {
        /* This is the first buffer in the queue */
        hDriver->trQueueHead = transferObj;
        hDriver->trQueueTail = transferObj;

        /* Because this is the first transfer in the queue, we need to submit the
         * transfer to the PLIB to start processing. */
        hDriver->i2cPlib->write(transferObj->slaveAddress, transferObj->writeBuffer, transferObj->writeSize);
    }
    else
    {
        /* This means the write queue is not empty. We must add
         * the buffer object to the end of the queue */
        hDriver->trQueueTail->next = transferObj;
        hDriver->trQueueTail = transferObj;
    }

    DRV_I2C_ResourceUnlock(hDriver);

    return;
}

// *****************************************************************************
/* Function:
    void DRV_I2C_WriteReadTransferAdd
    (
        const DRV_HANDLE handle,
        void * writeBuffer,
        const size_t writeSize,
        void * readBuffer,
        const size_t readSize,
        DRV_I2C_TRANSFER_HANDLE * const transferHandle
    )

  Summary:
    Dynamic implementation of DRV_I2C_WriteReadTransferAdd system interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_WriteReadTransferAdd system interface
    function.

  Remarks:
    See drv_i2c.h for usage information.
*/

void DRV_I2C_WriteReadTransferAdd ( const DRV_HANDLE handle, const uint16_t address, void *writeBuffer, size_t writeSize, void *readBuffer, size_t readSize, DRV_I2C_TRANSFER_HANDLE * transferHandle )
{
    DRV_I2C_CLIENT_OBJ     * clientObj        = (DRV_I2C_CLIENT_OBJ *)     NULL;
    DRV_I2C_OBJ            * hDriver          = (DRV_I2C_OBJ *)            NULL;
    DRV_I2C_TRANSFER_OBJ   * transferObj      = (DRV_I2C_TRANSFER_OBJ *)   NULL;


    *transferHandle = DRV_I2C_TRANSFER_HANDLE_INVALID;

        /* Validate the driver handle */
    clientObj = DRV_I2C_DriverHandleValidate(handle);
    if(clientObj == NULL)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Transfer Handle");
        return;
    }

    /* Validate the Request */
    if (transferHandle == NULL)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "NULL Transfer Handle Pointer");
        return;
    }

    if((writeSize == 0) || (writeBuffer == NULL) || (readSize == 0) || (readBuffer == NULL))
    {
        /* We either got an invalid source pointer or 0 bytes to transfer */
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Transfer Handle");

        return;
    }

    if(clientObj->inUse == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Handle");
        return;
    }

    hDriver = &gDrvI2CObj[clientObj->drvIndex];

    if(DRV_I2C_ResourceLock(hDriver) == false)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Failed to get resource lock");
        return;
    }

    if(hDriver->trObjFree == (DRV_I2C_TRANSFER_OBJ*)NULL )
    {
        /* This means we could not find a buffer. This
           will happen if the the transfer queue size
           parameter is configured to be less */

        SYS_ASSERT(false, "Insufficient Combined Queue Depth");

        DRV_I2C_ResourceUnlock(hDriver);

        return;
    }

    /* get transfer object from the free list */
    transferObj = hDriver->trObjFree;
    hDriver->trObjFree = hDriver->trObjFree->next;

    /* Configure the object */
    transferObj->slaveAddress = address;
    transferObj->readBuffer   = ( uint8_t *)readBuffer;
    transferObj->readSize     = readSize;
    transferObj->writeBuffer  = ( uint8_t *)writeBuffer;
    transferObj->writeSize    = writeSize;
    transferObj->hClient      = clientObj;
    transferObj->next         = ( DRV_I2C_TRANSFER_OBJ * ) NULL;
    transferObj->event        = DRV_I2C_TRANSFER_EVENT_PENDING;
    transferObj->flag         = DRV_I2C_TRANSFER_OBJ_FLAG_WRITE_READ;

    /* update transferHandle object with unique Id */
    transferObj->transferHandle = DRV_I2C_MAKE_HANDLE(hDriver->tokenCount, clientObj->drvIndex, transferObj->index);
    *transferHandle = transferObj->transferHandle;

    hDriver->tokenCount = DRV_I2C_UPDATE_TOKEN(hDriver->tokenCount);

    if(hDriver->trQueueHead == NULL)
    {
        /* This is the first buffer in the queue */
        hDriver->trQueueHead = transferObj;
        hDriver->trQueueTail = transferObj;

        /* Because this is the first transfer in the queue, we need to submit the
         * transfer to the PLIB to start processing. */
        hDriver->i2cPlib->writeRead(transferObj->slaveAddress, transferObj->writeBuffer, transferObj->writeSize, transferObj->readBuffer, transferObj->readSize);
    }
    else
    {
        /* This means the write queue is not empty. We must add
         * the buffer object to the end of the queue */
        hDriver->trQueueTail->next = transferObj;
        hDriver->trQueueTail = transferObj;
    }

    DRV_I2C_ResourceUnlock(hDriver);

    return;
}

// *****************************************************************************
/* Function:
    DRV_I2C_TRANSFER_EVENT DRV_I2C_TransferStatusGet( const DRV_I2C_TRANSFER_HANDLE transferHandle )

  Summary:
    Dynamic implementation of DRV_I2C_TransferStatusGet client interface function.

  Description:
    This is the dynamic implementation of DRV_I2C_TransferStatusGet client
    interface function.

  Remarks:
    See drv_i2c.h for usage information.
*/

DRV_I2C_TRANSFER_EVENT DRV_I2C_TransferStatusGet( const DRV_I2C_TRANSFER_HANDLE transferHandle )
{
    DRV_I2C_OBJ * dObj        = NULL;
    uint32_t      drvInstance;
    uint32_t      transferIndex;

    /* Extract drvInstance value from the transfer handle */
    drvInstance = ((transferHandle & DRV_I2C_INSTANCE_MASK) >> 8);
    if(drvInstance >= DRV_I2C_INSTANCES_NUMBER)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Transfer Handle Invalid");
        return (DRV_I2C_TRANSFER_EVENT) DRV_I2C_TRANSFER_HANDLE_INVALID;
    }

    dObj = (DRV_I2C_OBJ*)&gDrvI2CObj[drvInstance];

    /* Extract transfer index value from the transfer handle */
    transferIndex = transferHandle & DRV_I2C_INDEX_MASK;

    /* Validate the transferIndex and corresponding request */
    if(transferIndex >= dObj->trQueueSize)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Transfer Handle Invalid");
        return (DRV_I2C_TRANSFER_EVENT) DRV_I2C_TRANSFER_HANDLE_INVALID;
    }
    else if(transferHandle != dObj->trObjArr[transferIndex].transferHandle)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Transfer Handle Expired");
        return (DRV_I2C_TRANSFER_EVENT) DRV_I2C_TRANSFER_HANDLE_INVALID;
    }
    else
    {
        return dObj->trObjArr[transferIndex].event;
    }
}

/*******************************************************************************
 End of File
*/
