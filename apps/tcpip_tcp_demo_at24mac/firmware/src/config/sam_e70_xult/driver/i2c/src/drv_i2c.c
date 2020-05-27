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
#include "driver/i2c/drv_i2c.h"
#include "system/debug/sys_debug.h"

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

static inline uint32_t _DRV_I2C_MAKE_HANDLE(uint16_t token, uint8_t drvIndex, uint8_t index)
{
    return ((token << 16) | (drvIndex << 8) | index);
}

static inline uint16_t _DRV_I2C_UPDATE_TOKEN(uint16_t token)
{
    token++;
    if (token >= DRV_I2C_TOKEN_MAX)
    {
        token = 1;
    }

    return token;
}

static DRV_I2C_CLIENT_OBJ* _DRV_I2C_DriverHandleValidate(DRV_HANDLE handle)
{
    /* This function returns the pointer to the client object that is
       associated with this handle if the handle is valid. Returns NULL
       otherwise. */

    uint32_t drvInstance = 0;
    DRV_I2C_CLIENT_OBJ* clientObj = NULL;

    if((handle != DRV_HANDLE_INVALID) && (handle != 0))
    {
        /* Extract the drvInstance value from the handle */
        drvInstance = ((handle & DRV_I2C_INSTANCE_MASK) >> 8);

        if (drvInstance >= DRV_I2C_INSTANCES_NUMBER)
        {
            return (NULL);
        }

        if ((handle & DRV_I2C_INDEX_MASK) >= gDrvI2CObj[drvInstance].nClientsMax)
        {
            return (NULL);
        }

        /* Extract the client index and obtain the client object */
        clientObj = &((DRV_I2C_CLIENT_OBJ *)gDrvI2CObj[drvInstance].clientObjPool)[handle & DRV_I2C_INDEX_MASK];

        if ((clientObj->clientHandle != handle) || (clientObj->inUse == false))
        {
            return (NULL);
        }
    }

    return(clientObj);
}

static bool _DRV_I2C_ResourceLock(DRV_I2C_OBJ * dObj)
{
    bool interruptStatus;
    const DRV_I2C_INTERRUPT_SOURCES* intInfo = dObj->interruptSources;
    const DRV_I2C_MULTI_INT_SRC* multiVector = &dObj->interruptSources->intSources.multi;

    /* We will allow buffers to be added in the interrupt
       context of this I2C driver. But we must make
       sure that if we are in interrupt, then we should
       not modify mutexes. */
    if(dObj->interruptNestingCount == 0)
    {
        /* Grab a mutex. This is okay because we are not in an interrupt context */
        if(OSAL_MUTEX_Lock(&(dObj->mutexTransferObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
        {
            return false;
        }
    }

    if (intInfo->isSingleIntSrc == true)
    {
        /* Disable I2C interrupt */
         dObj->i2cInterruptStatus = SYS_INT_SourceDisable((INT_SOURCE)intInfo->intSources.i2cInterrupt);
    }
    else
    {
         interruptStatus = SYS_INT_Disable();

        /* Disable I2C interrupt sources */
         if(multiVector->i2cInt0 != -1)
         {
            dObj->i2cInt0Status = SYS_INT_SourceDisable((INT_SOURCE)multiVector->i2cInt0);
         }
         if(multiVector->i2cInt1 != -1)
         {
            dObj->i2cInt1Status = SYS_INT_SourceDisable((INT_SOURCE)multiVector->i2cInt1);
         }
         if(multiVector->i2cInt2 != -1)
         {
            dObj->i2cInt2Status = SYS_INT_SourceDisable((INT_SOURCE)multiVector->i2cInt2);
         }
         if(multiVector->i2cInt3 != -1)
         {
            dObj->i2cInt3Status = SYS_INT_SourceDisable((INT_SOURCE)multiVector->i2cInt3);
         }

         SYS_INT_Restore(interruptStatus);

    }

    return true;
}

static void _DRV_I2C_ResourceUnlock(DRV_I2C_OBJ * dObj)
{
    bool interruptStatus;
    const DRV_I2C_INTERRUPT_SOURCES* intInfo = dObj->interruptSources;
    const DRV_I2C_MULTI_INT_SRC* multiVector = &dObj->interruptSources->intSources.multi;

    /* Restore the interrupts back */
    if (intInfo->isSingleIntSrc == true)
    {
        /* Enable I2C interrupt */
         SYS_INT_SourceRestore((INT_SOURCE)intInfo->intSources.i2cInterrupt, dObj->i2cInterruptStatus);
    }
    else
    {
        interruptStatus = SYS_INT_Disable();

        /* Enable I2C interrupt sources */
        if(multiVector->i2cInt0 != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->i2cInt0,dObj->i2cInt0Status );
        }
        if(multiVector->i2cInt1 != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->i2cInt1,dObj->i2cInt1Status );
        }
        if(multiVector->i2cInt2 != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->i2cInt2,dObj->i2cInt2Status );
        }
        if(multiVector->i2cInt3 != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->i2cInt3,dObj->i2cInt3Status );
        }

        SYS_INT_Restore(interruptStatus);
    }

    if(dObj->interruptNestingCount == 0)
    {
        /* Release mutex */
        OSAL_MUTEX_Unlock(&(dObj->mutexTransferObjects));
    }
}

static DRV_I2C_TRANSFER_OBJ* _DRV_I2C_FreeTransferObjGet(DRV_I2C_CLIENT_OBJ* clientObj)
{
    uint32_t index;
    DRV_I2C_OBJ* dObj = (DRV_I2C_OBJ* )&gDrvI2CObj[clientObj->drvIndex];
    DRV_I2C_TRANSFER_OBJ* pTransferObj = dObj->transferObjPool;

    for (index = 0; index < dObj->transferObjPoolSize; index++)
    {
        if (pTransferObj[index].inUse == false)
        {
            pTransferObj[index].inUse = true;
            pTransferObj[index].next = NULL;

            /* Generate a unique buffer handle consisting of an incrementing
             * token counter, driver index and the buffer index.
             */
            pTransferObj[index].transferHandle = (DRV_I2C_TRANSFER_HANDLE)_DRV_I2C_MAKE_HANDLE(
                dObj->i2cTokenCount, (uint8_t)clientObj->drvIndex, index);

            /* Update the token for next time */
            dObj->i2cTokenCount = _DRV_I2C_UPDATE_TOKEN(dObj->i2cTokenCount);

            return &pTransferObj[index];
        }
    }
    return NULL;
}

static bool _DRV_I2C_TransferObjAddToList(
    DRV_I2C_OBJ* dObj,
    DRV_I2C_TRANSFER_OBJ* transferObj
)
{
    DRV_I2C_TRANSFER_OBJ** pTransferObjList;
    bool isFirstTransferInList = false;

    pTransferObjList = (DRV_I2C_TRANSFER_OBJ**)&(dObj->transferObjList);

    // Is the buffer object list empty?
    if (*pTransferObjList == NULL)
    {
        *pTransferObjList = transferObj;
        isFirstTransferInList = true;
    }
    else
    {
        // List is not empty. Iterate to the end of the buffer object list.
        while (*pTransferObjList != NULL)
        {
            if ((*pTransferObjList)->next == NULL)
            {
                // End of the list reached, add the buffer here.
                (*pTransferObjList)->next = transferObj;
                break;
            }
            else
            {
                pTransferObjList = (DRV_I2C_TRANSFER_OBJ**)&((*pTransferObjList)->next);
            }
        }
    }

    return isFirstTransferInList;
}

static DRV_I2C_TRANSFER_OBJ* _DRV_I2C_TransferObjListGet( DRV_I2C_OBJ* dObj )
{
    DRV_I2C_TRANSFER_OBJ* pTransferObj = NULL;

    // Return the element at the head of the linked list
    pTransferObj = (DRV_I2C_TRANSFER_OBJ*)dObj->transferObjList;

    return pTransferObj;
}

static void _DRV_I2C_RemoveTransferObjFromList( DRV_I2C_OBJ* dObj )
{
    DRV_I2C_TRANSFER_OBJ** pTransferObjList;

    pTransferObjList = (DRV_I2C_TRANSFER_OBJ**)&(dObj->transferObjList);

    // Remove the element at the head of the linked list
    if (*pTransferObjList != NULL)
    {
        /* Save the buffer object to be removed. Set the next buffer object as
         * the new head of the linked list. Reset the removed buffer object. */

        DRV_I2C_TRANSFER_OBJ* temp = *pTransferObjList;
        *pTransferObjList = (*pTransferObjList)->next;
        temp->currentState = DRV_I2C_TRANSFER_OBJ_IS_FREE;
        temp->next = NULL;
        temp->inUse = false;
    }
}

static void _DRV_I2C_RemoveClientTransfersFromList(
    DRV_I2C_OBJ* dObj,
    DRV_I2C_CLIENT_OBJ* clientObj
)
{
    DRV_I2C_TRANSFER_OBJ** pTransferObjList;
    DRV_I2C_TRANSFER_OBJ* delTransferObj = NULL;

    pTransferObjList = (DRV_I2C_TRANSFER_OBJ**)&(dObj->transferObjList);

    while (*pTransferObjList != NULL)
    {
        // Do not remove the buffer object that is already in process
        if (((*pTransferObjList)->clientHandle == clientObj->clientHandle) &&
            ((*pTransferObjList)->currentState == DRV_I2C_TRANSFER_OBJ_IS_IN_QUEUE))
        {
            // Save the node to be deleted off the list
            delTransferObj = *pTransferObjList;

            // Update the current node to point to the deleted node's next
            *pTransferObjList = (DRV_I2C_TRANSFER_OBJ*)(*pTransferObjList)->next;

            // Reset the deleted node
            delTransferObj->currentState = DRV_I2C_TRANSFER_OBJ_IS_FREE;
            delTransferObj->event = DRV_I2C_TRANSFER_EVENT_COMPLETE;
            delTransferObj->next = NULL;
            delTransferObj->inUse = false;
        }
        else
        {
            // Move to the next node
            pTransferObjList = (DRV_I2C_TRANSFER_OBJ**)&((*pTransferObjList)->next);
        }
    }
}

static void _DRV_I2C_PLibCallbackHandler( uintptr_t contextHandle )
{
    DRV_I2C_OBJ* dObj = (DRV_I2C_OBJ *)contextHandle;
    DRV_I2C_CLIENT_OBJ* clientObj = NULL;
    DRV_I2C_TRANSFER_OBJ* transferObj = NULL;
    DRV_I2C_TRANSFER_EVENT event;
    DRV_I2C_TRANSFER_HANDLE transferHandle;

    if((dObj->inUse == false) || (dObj->status != SYS_STATUS_READY))
    {
        /* This instance of the driver is not initialized. Don't
         * do anything */
        return;
    }

    // Get the transfer object at the head of the list
    transferObj = _DRV_I2C_TransferObjListGet(dObj);

    // Get the client object that owns this buffer
    clientObj = &((DRV_I2C_CLIENT_OBJ *)gDrvI2CObj[((transferObj->clientHandle & DRV_I2C_INSTANCE_MASK) >> 8)].clientObjPool)
                [transferObj->clientHandle & DRV_I2C_INDEX_MASK];

    /* Check if the client that submitted the request is active? */
    if (clientObj->clientHandle == transferObj->clientHandle)
    {
        transferObj->errors = dObj->i2cPlib->errorGet();

        if(transferObj->errors == DRV_I2C_ERROR_NONE)
        {
            transferObj->event = DRV_I2C_TRANSFER_EVENT_COMPLETE;
        }
        else
        {
            transferObj->event = DRV_I2C_TRANSFER_EVENT_ERROR;
        }

        /* Save the transfer handle and event locally before freeing the transfer object*/
        event = transferObj->event;
        transferHandle = transferObj->transferHandle;

        /* Free the completed buffer.
         * This is done before giving callback to allow application to use the freed
         * buffer and queue in a new request from within the callback */

        _DRV_I2C_RemoveTransferObjFromList(dObj);

        if(clientObj->eventHandler != NULL)
        {
            dObj->interruptNestingCount ++;

            clientObj->eventHandler(event, transferHandle, clientObj->context);

            dObj->interruptNestingCount -- ;
        }
    }
    else
    {
        /* The client has probably closed the driver. Free the completed buffer */
        _DRV_I2C_RemoveTransferObjFromList(dObj);
    }

    /* Get the transfer object at the head of the list */
    transferObj = _DRV_I2C_TransferObjListGet(dObj);

    /* Process the next transfer buffer */
    if((transferObj != NULL) && (transferObj->currentState == DRV_I2C_TRANSFER_OBJ_IS_IN_QUEUE))
    {
        // Get the client object that owns this buffer
        clientObj = &((DRV_I2C_CLIENT_OBJ *)gDrvI2CObj[((transferObj->clientHandle & DRV_I2C_INSTANCE_MASK) >> 8)].clientObjPool)
                    [transferObj->clientHandle & DRV_I2C_INDEX_MASK];

        /* Check if the transfer setup for this client is different than the current transfer setup */
        if (dObj->currentTransferSetup.clockSpeed != clientObj->transferSetup.clockSpeed)
        {
            /* Set the new transfer setup */
            dObj->i2cPlib->transferSetup(&clientObj->transferSetup, 0);

            dObj->currentTransferSetup.clockSpeed = clientObj->transferSetup.clockSpeed;
        }

        switch(transferObj->flag)
        {
            case DRV_I2C_TRANSFER_OBJ_FLAG_READ:
                transferObj->currentState = DRV_I2C_TRANSFER_OBJ_IS_PROCESSING;
                dObj->i2cPlib->read(transferObj->slaveAddress, transferObj->readBuffer, transferObj->readSize);
                break;

            case DRV_I2C_TRANSFER_OBJ_FLAG_WRITE:
                transferObj->currentState = DRV_I2C_TRANSFER_OBJ_IS_PROCESSING;
                dObj->i2cPlib->write(transferObj->slaveAddress, transferObj->writeBuffer, transferObj->writeSize);
                break;
            case DRV_I2C_TRANSFER_OBJ_FLAG_WRITE_READ:
                transferObj->currentState = DRV_I2C_TRANSFER_OBJ_IS_PROCESSING;
                dObj->i2cPlib->writeRead(transferObj->slaveAddress, transferObj->writeBuffer, transferObj->writeSize, transferObj->readBuffer, transferObj->readSize);
                break;
            default:
                /* Execution should never enter the default case */
                break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_I2C_Initialize(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT* const init
)
{
    DRV_I2C_OBJ* dObj     = NULL;
    DRV_I2C_INIT* i2cInit = (DRV_I2C_INIT*)init;

    /* Validate the request */
    if(drvIndex >= DRV_I2C_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Is the driver instance already initialized? */
    if(gDrvI2CObj[drvIndex].inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Allocate the driver object */
    dObj = &gDrvI2CObj[drvIndex];

    /* Create mutex */
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
    dObj->i2cPlib                           = i2cInit->i2cPlib;
    dObj->interruptSources                  = i2cInit->interruptSources;
    dObj->clientObjPool                     = i2cInit->clientObjPool;
    dObj->nClientsMax                       = i2cInit->numClients;
    dObj->transferObjPool                   = (DRV_I2C_TRANSFER_OBJ*)i2cInit->transferObjPool;
    dObj->transferObjPoolSize               = i2cInit->transferObjPoolSize;
    dObj->transferObjList                   = (DRV_I2C_TRANSFER_OBJ*)NULL;
    dObj->nClients                          = 0;
    dObj->isExclusive                       = false;
    dObj->interruptNestingCount             = 0;
    dObj->i2cTokenCount                     = 1;
    dObj->initI2CClockSpeed                 = i2cInit->clockSpeed;
    dObj->currentTransferSetup.clockSpeed   = i2cInit->clockSpeed;

    /* Register a callback with the underlying PLIB.
     * dObj as a context parameter will be used to distinguish the events
     * from different instances. */
    dObj->i2cPlib->callbackRegister(_DRV_I2C_PLibCallbackHandler, (uintptr_t)dObj);

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

SYS_STATUS DRV_I2C_Status( const SYS_MODULE_OBJ object)
{
    /* Validate the request */
    if( (object == SYS_MODULE_OBJ_INVALID) || (object >= DRV_I2C_INSTANCES_NUMBER) )
    {
        return SYS_STATUS_UNINITIALIZED;
    }

    return (gDrvI2CObj[object].status);
}

DRV_HANDLE DRV_I2C_Open(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    DRV_I2C_CLIENT_OBJ* clientObj;
    DRV_I2C_OBJ* dObj = NULL;
    uint32_t iClient;

    /* Validate the request */
    if (drvIndex >= DRV_I2C_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvI2CObj[drvIndex];

    /* Guard against multiple threads trying to open the driver */
    if(OSAL_MUTEX_Lock(&(dObj->mutexClientObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
    {
        return DRV_HANDLE_INVALID;
    }

    if((dObj->status != SYS_STATUS_READY) || (dObj->inUse == false))
    {
        OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));
        return DRV_HANDLE_INVALID;
    }

    if(dObj->isExclusive == true)
    {
        /* Driver is already opened with exclusive access. Hence, cannot be opened again*/
        OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));
        return DRV_HANDLE_INVALID;
    }

    if((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE))
    {
        /* Exclusive access is requested while the driver is already opened by other client */
        OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));
        return DRV_HANDLE_INVALID;
    }

    for(iClient = 0; iClient != dObj->nClientsMax; iClient++)
    {
        clientObj = &((DRV_I2C_CLIENT_OBJ *)dObj->clientObjPool)[iClient];

        if(clientObj->inUse == false)
        {
            /* This means we have a free client object to use */
            clientObj->inUse = true;

            if(ioIntent & DRV_IO_INTENT_EXCLUSIVE)
            {
                /* Set the driver exclusive flag */
                dObj->isExclusive = true;
            }

            dObj->nClients ++;

            /* Generate the client handle */
            clientObj->clientHandle = (DRV_HANDLE)_DRV_I2C_MAKE_HANDLE(dObj->i2cTokenCount, (uint8_t)drvIndex, iClient);

            /* Increment the instance specific token counter */
            dObj->i2cTokenCount = _DRV_I2C_UPDATE_TOKEN(dObj->i2cTokenCount);

            /* We have found a client object, now release the mutex */
            OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

            clientObj->drvIndex                 = drvIndex;
            clientObj->ioIntent                 = (DRV_IO_INTENT)(ioIntent | DRV_IO_INTENT_NONBLOCKING);
            clientObj->transferSetup.clockSpeed = dObj->initI2CClockSpeed;
            clientObj->eventHandler             = NULL;
            clientObj->context                  = (uintptr_t)NULL;

            return ((DRV_HANDLE) clientObj->clientHandle );
        }
    }

    /* Could not find a client object. Release the mutex and return with an invalid handle. */
    OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

    return DRV_HANDLE_INVALID;
}

void DRV_I2C_Close( const DRV_HANDLE handle )
{
    /* This function closes the client, The client object is deallocated and
     * returned to the pool. */

    DRV_I2C_OBJ* dObj;
    DRV_I2C_CLIENT_OBJ* clientObj;

    /* Validate the handle */
    clientObj = _DRV_I2C_DriverHandleValidate(handle);
    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        return;
    }

    dObj = &gDrvI2CObj[clientObj->drvIndex];

    /* Guard against multiple threads trying to open/close the driver */
    if(OSAL_MUTEX_Lock(&(dObj->mutexClientObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
    {
        return;
    }

    /* We will be removing the transfers queued by the client. Guard the linked list
     * against interrupts and/or other threads trying to modify the linked list.
     */
    if (_DRV_I2C_ResourceLock(dObj) == false)
    {
        return;
    }

    /* Remove all buffers that this client owns from the driver queue */

    _DRV_I2C_RemoveClientTransfersFromList(dObj, clientObj);

    _DRV_I2C_ResourceUnlock(dObj);

    /* Reduce the number of clients */
    dObj->nClients --;

    /* Reset the exclusive flag */
    dObj->isExclusive = false;

    /* Invalidate the client handle */
    clientObj->clientHandle = DRV_HANDLE_INVALID;

    /* Free the client object */
    clientObj->inUse = false;

    OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

    return;
}

void DRV_I2C_TransferEventHandlerSet(
    const DRV_HANDLE handle,
    const DRV_I2C_TRANSFER_EVENT_HANDLER eventHandler,
    const uintptr_t context
)
{
    DRV_I2C_CLIENT_OBJ* clientObj = NULL;
    DRV_I2C_OBJ* dObj = NULL;

    /* Validate the handle */
    clientObj = _DRV_I2C_DriverHandleValidate(handle);

    if (clientObj == NULL)
    {
        return;
    }

    dObj = &gDrvI2CObj[clientObj->drvIndex];

    if(_DRV_I2C_ResourceLock(dObj) == false)
    {
        return;
    }

    clientObj->eventHandler = eventHandler;
    clientObj->context = context;

    _DRV_I2C_ResourceUnlock(dObj);
}

bool DRV_I2C_TransferSetup( const DRV_HANDLE handle, DRV_I2C_TRANSFER_SETUP* setup )
{
    DRV_I2C_CLIENT_OBJ* clientObj = NULL;

    if(setup == NULL)
    {
        return false;
    }

    /* Validate the driver handle */
    clientObj = _DRV_I2C_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        return false;
    }

    /* Save the client specific transfer setup */
    clientObj->transferSetup = *setup;

    return true;
}

DRV_I2C_ERROR DRV_I2C_ErrorGet( const DRV_I2C_TRANSFER_HANDLE transferHandle )
{
    DRV_I2C_OBJ* dObj = NULL;
    uint32_t drvInstance = 0;
    uint8_t transferIndex;
    DRV_I2C_ERROR errors = DRV_I2C_ERROR_NONE;

    /* Extract driver instance value from the transfer handle */
    drvInstance = ((transferHandle & DRV_I2C_INSTANCE_MASK) >> 8);

    if(drvInstance >= DRV_I2C_INSTANCES_NUMBER)
    {
        return errors;
    }

    dObj = (DRV_I2C_OBJ*)&gDrvI2CObj[drvInstance];

    if(_DRV_I2C_ResourceLock(dObj) == false)
    {
        return errors;
    }

    /* Extract transfer buffer index value from the transfer handle */
    transferIndex = transferHandle & DRV_I2C_INDEX_MASK;

    /* Validate the transferIndex and corresponding request */
    if(transferIndex < dObj->transferObjPoolSize)
    {
        if(transferHandle == dObj->transferObjPool[transferIndex].transferHandle)
        {
            errors = dObj->transferObjPool[transferIndex].errors;
            dObj->transferObjPool[transferIndex].errors = DRV_I2C_ERROR_NONE;
        }
    }

    _DRV_I2C_ResourceUnlock(dObj);

    return errors;
}

static void _DRV_I2C_WriteReadTransferAdd (
    const DRV_HANDLE handle,
    const uint16_t address,
    void* const writeBuffer,
    const size_t writeSize,
    void* const readBuffer,
    const size_t readSize,
    DRV_I2C_TRANSFER_HANDLE* const transferHandle,
    DRV_I2C_TRANSFER_OBJ_FLAGS transferFlags
)
{
    DRV_I2C_CLIENT_OBJ* clientObj = NULL;
    DRV_I2C_OBJ* dObj = NULL;
    DRV_I2C_TRANSFER_OBJ* transferObj = NULL;

    /* Validate the transfer handle */
    if (transferHandle == NULL)
    {
        return;
    }

    *transferHandle = DRV_I2C_TRANSFER_HANDLE_INVALID;

    /* Validate the driver handle */
    clientObj = _DRV_I2C_DriverHandleValidate(handle);
    if(clientObj == NULL)
    {
        return;
    }

    if (transferFlags == DRV_I2C_TRANSFER_OBJ_FLAG_READ)
    {
        if((readSize == 0) || (readBuffer == NULL))
        {
            return;
        }
    }
    else if ((transferFlags == DRV_I2C_TRANSFER_OBJ_FLAG_WRITE) || (transferFlags == DRV_I2C_TRANSFER_OBJ_FLAG_WRITE_FORCED))
    {
        if((writeSize == 0) || (writeBuffer == NULL))
        {
            return;
        }
    }
    else
    {
        if((writeSize == 0) || (writeBuffer == NULL) || (readSize == 0) || (readBuffer == NULL))
        {
            return;
        }
    }

    /* Get the driver object from the client handle */
    dObj = &gDrvI2CObj[clientObj->drvIndex];


    if(_DRV_I2C_ResourceLock(dObj) == false)
    {
        return;
    }

    /* Get a free transfer object */
    transferObj = _DRV_I2C_FreeTransferObjGet(clientObj);

    if(transferObj == NULL)
    {
        _DRV_I2C_ResourceUnlock(dObj);
        return;
    }

    /* Configure the transfer object */
    transferObj->slaveAddress = address;
    transferObj->readBuffer   = ( uint8_t *)readBuffer;
    transferObj->readSize     = readSize;
    transferObj->writeBuffer  = ( uint8_t *)writeBuffer;
    transferObj->writeSize    = writeSize;
    transferObj->clientHandle = handle;
    transferObj->errors       = DRV_I2C_ERROR_NONE;
    transferObj->currentState = DRV_I2C_TRANSFER_OBJ_IS_IN_QUEUE;
    transferObj->event        = DRV_I2C_TRANSFER_EVENT_PENDING;
    transferObj->flag         = transferFlags;

    *transferHandle = transferObj->transferHandle;

    /* Add the buffer object to the transfer buffer list */
    if (_DRV_I2C_TransferObjAddToList(dObj, transferObj) == true)
    {
        /* This is the first request in the queue, hence initiate a PLIB transfer */

        /* Check if the transfer setup for this client is different than the current transfer setup */
        if (dObj->currentTransferSetup.clockSpeed != clientObj->transferSetup.clockSpeed)
        {
            /* Set the new transfer setup */
            dObj->i2cPlib->transferSetup(&clientObj->transferSetup, 0);

            dObj->currentTransferSetup.clockSpeed = clientObj->transferSetup.clockSpeed;
        }

        transferObj->currentState = DRV_I2C_TRANSFER_OBJ_IS_PROCESSING;

        if (transferFlags == DRV_I2C_TRANSFER_OBJ_FLAG_READ)
        {
            dObj->i2cPlib->read(
                transferObj->slaveAddress,
                transferObj->readBuffer,
                transferObj->readSize
            );
        }
        else if (transferFlags == DRV_I2C_TRANSFER_OBJ_FLAG_WRITE)
        {
            dObj->i2cPlib->write(
                transferObj->slaveAddress,
                transferObj->writeBuffer,
                transferObj->writeSize
            );
        }
        else
        {
            dObj->i2cPlib->writeRead(
                transferObj->slaveAddress,
                transferObj->writeBuffer,
                transferObj->writeSize,
                transferObj->readBuffer,
                transferObj->readSize
            );
        }
    }

    _DRV_I2C_ResourceUnlock(dObj);
}

void DRV_I2C_ReadTransferAdd(
    const DRV_HANDLE handle,
    const uint16_t address,
    void* const buffer,
    const size_t size,
    DRV_I2C_TRANSFER_HANDLE* const transferHandle
)
{
    _DRV_I2C_WriteReadTransferAdd(handle, address, NULL, 0,
        buffer, size, transferHandle, DRV_I2C_TRANSFER_OBJ_FLAG_READ);
}

void DRV_I2C_WriteTransferAdd(
    const DRV_HANDLE handle,
    const uint16_t address,
    void* const buffer,
    const size_t size,
    DRV_I2C_TRANSFER_HANDLE* const transferHandle
)
{
    _DRV_I2C_WriteReadTransferAdd(handle, address, buffer, size,
        NULL, 0, transferHandle, DRV_I2C_TRANSFER_OBJ_FLAG_WRITE);
}

void DRV_I2C_WriteReadTransferAdd (
    const DRV_HANDLE handle,
    const uint16_t address,
    void* const writeBuffer,
    const size_t writeSize,
    void* const readBuffer,
    const size_t readSize,
    DRV_I2C_TRANSFER_HANDLE* const transferHandle
)
{
    _DRV_I2C_WriteReadTransferAdd(handle, address, writeBuffer, writeSize,
        readBuffer, readSize, transferHandle, DRV_I2C_TRANSFER_OBJ_FLAG_WRITE_READ);
}

DRV_I2C_TRANSFER_EVENT DRV_I2C_TransferStatusGet(
    const DRV_I2C_TRANSFER_HANDLE transferHandle
)
{
    DRV_I2C_OBJ* dObj = NULL;
    uint32_t drvInstance = 0;
    uint8_t transferIndex;
    DRV_I2C_TRANSFER_EVENT  event = DRV_I2C_TRANSFER_EVENT_HANDLE_INVALID;

    /* Extract driver instance value from the transfer handle */
    drvInstance = ((transferHandle & DRV_I2C_INSTANCE_MASK) >> 8);

    if(drvInstance >= DRV_I2C_INSTANCES_NUMBER)
    {
        return event;
    }

    dObj = (DRV_I2C_OBJ*)&gDrvI2CObj[drvInstance];

    if(_DRV_I2C_ResourceLock(dObj) == false)
    {
        return event;
    }

    /* Extract transfer buffer index value from the transfer handle */
    transferIndex = transferHandle & DRV_I2C_INDEX_MASK;

    /* Validate the transferIndex and corresponding request */
    if(transferIndex < dObj->transferObjPoolSize)
    {
        if(transferHandle == dObj->transferObjPool[transferIndex].transferHandle)
        {
            event = dObj->transferObjPool[transferIndex].event;
        }
        else
        {
            event = DRV_I2C_TRANSFER_EVENT_HANDLE_EXPIRED;
        }
    }

    _DRV_I2C_ResourceUnlock(dObj);

    return event;
}
