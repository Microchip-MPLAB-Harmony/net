/*******************************************************************************
  SD Host Controller Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdmmc.c

  Summary:
    SD Host Controller Device Driver Implementation

  Description:
    The SD Host Controller device driver provides a simple interface to manage the
    SD Host Controller modules on Microchip microcontrollers.  This file Implements
    the core interface routines for the SD Host Controller driver.

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
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "driver/sdmmc/drv_sdmmc.h"
#include "driver/sdmmc/src/drv_sdmmc_local.h"
#include "system/cache/sys_cache.h"
#include <string.h>

static DRV_SDMMC_OBJ gDrvSDMMCObj[DRV_SDMMC_INSTANCES_NUMBER];


static inline uint32_t  _DRV_SDMMC_MAKE_HANDLE(uint16_t token, uint8_t drvIndex, uint8_t index)
{
    return ((token << 16) | (drvIndex << 8) | index);
}

static inline uint16_t _DRV_SDMMC_UPDATE_TOKEN(uint16_t token)
{
    token++;

    if (token >= DRV_SDMMC_TOKEN_MAX)
    {
        token = 1;
    }

    return token;
}

static DRV_SDMMC_CLIENT_OBJ* _DRV_SDMMC_DriverHandleValidate(DRV_HANDLE handle)
{
    /* This function returns the pointer to the client object that is
       associated with this handle if the handle is valid. Returns NULL
       otherwise. */

    uint32_t drvInstance = 0;
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_OBJ* dObj = NULL;

    if((handle != DRV_HANDLE_INVALID) && (handle != 0))
    {
        /* Extract the drvInstance value from the handle */
        drvInstance = ((handle & DRV_SDMMC_INSTANCE_MASK) >> 8);

        if (drvInstance >= DRV_SDMMC_INSTANCES_NUMBER)
        {
            return NULL;
        }

        if ((handle & DRV_SDMMC_INDEX_MASK) >= gDrvSDMMCObj[drvInstance].nClientsMax)
        {
            return NULL;
        }

        /* Extract the client index and obtain the client object */
        clientObj = &((DRV_SDMMC_CLIENT_OBJ *)gDrvSDMMCObj[drvInstance].clientObjPool)[handle & DRV_SDMMC_INDEX_MASK];

        if ((clientObj->clientHandle != handle) || (clientObj->inUse == false))
        {
            return NULL;
        }

        /* Check if the driver is ready for operation */
        dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];
        if (dObj->status != SYS_STATUS_READY)
        {
            return NULL;
        }
    }

    return(clientObj);
}

static DRV_SDMMC_BUFFER_OBJ* _DRV_SDMMC_FreeBufferObjectGet(DRV_SDMMC_CLIENT_OBJ* clientObj)
{
    uint32_t index;
    DRV_SDMMC_OBJ* dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];
    DRV_SDMMC_BUFFER_OBJ* pBufferObj = (DRV_SDMMC_BUFFER_OBJ*)dObj->bufferObjPool;

    for (index = 0; index < dObj->bufferObjPoolSize; index++)
    {
        if (pBufferObj[index].inUse == false)
        {
            pBufferObj[index].inUse = true;
            pBufferObj[index].next = NULL;

            /* Generate a unique buffer handle consisting of an incrementing
             * token counter, driver index and the buffer index.
             */
            pBufferObj[index].commandHandle = (DRV_SDMMC_COMMAND_HANDLE)_DRV_SDMMC_MAKE_HANDLE(
                dObj->sdmmcTokenCount, (uint8_t)clientObj->drvIndex, index);

            /* Update the token for next time */
            dObj->sdmmcTokenCount = _DRV_SDMMC_UPDATE_TOKEN(dObj->sdmmcTokenCount);

            return &pBufferObj[index];
        }
    }
    return NULL;
}

static bool _DRV_SDMMC_BufferObjectAddToList(
    DRV_SDMMC_OBJ* dObj,
    DRV_SDMMC_BUFFER_OBJ* bufferObj
)
{
    DRV_SDMMC_BUFFER_OBJ** pBufferObjList;
    bool isFirstBufferInList = false;

    pBufferObjList = (DRV_SDMMC_BUFFER_OBJ**)&(dObj->bufferObjList);

    // Is the buffer object list empty?
    if (*pBufferObjList == NULL)
    {
        *pBufferObjList = bufferObj;
        isFirstBufferInList = true;
    }
    else
    {
        // List is not empty. Iterate to the end of the buffer object list.
        while (*pBufferObjList != NULL)
        {
            if ((*pBufferObjList)->next == NULL)
            {
                // End of the list reached, add the buffer here.
                (*pBufferObjList)->next = bufferObj;
                break;
            }
            else
            {
                pBufferObjList = (DRV_SDMMC_BUFFER_OBJ**)&((*pBufferObjList)->next);
            }
        }
    }

    return isFirstBufferInList;
}

static DRV_SDMMC_BUFFER_OBJ* _DRV_SDMMC_BufferListGet(
    DRV_SDMMC_OBJ* dObj
)
{
    DRV_SDMMC_BUFFER_OBJ* pBufferObj = NULL;

    // Return the element at the head of the linked list
    pBufferObj = (DRV_SDMMC_BUFFER_OBJ*)dObj->bufferObjList;

    return pBufferObj;
}

static void _DRV_SDMMC_RemoveBufferObjFromList(
    DRV_SDMMC_OBJ* dObj
)
{
    DRV_SDMMC_BUFFER_OBJ** pBufferObjList;

    pBufferObjList = (DRV_SDMMC_BUFFER_OBJ**)&(dObj->bufferObjList);

    // Remove the element at the head of the linked list
    if (*pBufferObjList != NULL)
    {
        /* Save the buffer object to be removed. Set the next buffer object as
         * the new head of the linked list. Reset the removed buffer object. */

        DRV_SDMMC_BUFFER_OBJ* temp = *pBufferObjList;
        *pBufferObjList = (*pBufferObjList)->next;
        temp->next = NULL;
        temp->inUse = false;
    }
}

static void _DRV_SDMMC_RemoveClientBuffersFromList(
    DRV_SDMMC_OBJ* dObj,
    DRV_SDMMC_CLIENT_OBJ* clientObj
)
{
    DRV_SDMMC_BUFFER_OBJ** pBufferObjList;
    DRV_SDMMC_BUFFER_OBJ* delBufferObj = NULL;

    pBufferObjList = (DRV_SDMMC_BUFFER_OBJ**)&(dObj->bufferObjList);

    while (*pBufferObjList != NULL)
    {
        // Do not remove the buffer object that is already in process

        if (((*pBufferObjList)->clientHandle == clientObj->clientHandle) &&
                ((*pBufferObjList)->status == DRV_SDMMC_COMMAND_QUEUED))
        {
            // Save the node to be deleted off the list
            delBufferObj = *pBufferObjList;

            // Update the current node to point to the deleted node's next
            *pBufferObjList = (DRV_SDMMC_BUFFER_OBJ*)(*pBufferObjList)->next;

            if (clientObj->eventHandler != NULL)
            {
                /* Call the event handler */
                clientObj->eventHandler((SYS_MEDIA_BLOCK_EVENT)DRV_SDMMC_EVENT_COMMAND_ERROR, delBufferObj->commandHandle, clientObj->context);
            }

            // Reset the deleted node
            delBufferObj->status = DRV_SDMMC_COMMAND_COMPLETED;
            delBufferObj->next = NULL;
            delBufferObj->inUse = false;
        }
        else
        {
            // Move to the next node
            pBufferObjList = (DRV_SDMMC_BUFFER_OBJ**)&((*pBufferObjList)->next);
        }
    }
}

static void _DRV_SDMMC_RemoveBufferObjects (
    DRV_SDMMC_OBJ* dObj
)
{
    DRV_SDMMC_BUFFER_OBJ** pBufferObjList;
    DRV_SDMMC_BUFFER_OBJ* delBufferObj = NULL;
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;

    pBufferObjList = (DRV_SDMMC_BUFFER_OBJ**)&(dObj->bufferObjList);

    while (*pBufferObjList != NULL)
    {
        // Save the node to be deleted off the list
        delBufferObj = *pBufferObjList;

        // Update the current node to point to the deleted node's next
        *pBufferObjList = (DRV_SDMMC_BUFFER_OBJ*)(*pBufferObjList)->next;

        /* Get the client object that owns this buffer */
        clientObj = &((DRV_SDMMC_CLIENT_OBJ *)dObj->clientObjPool)[delBufferObj->clientHandle & DRV_SDMMC_INDEX_MASK];

        if (clientObj->eventHandler != NULL)
        {
            /* Call the event handler */
            clientObj->eventHandler((SYS_MEDIA_BLOCK_EVENT)DRV_SDMMC_EVENT_COMMAND_ERROR, delBufferObj->commandHandle, clientObj->context);
        }

        // Reset the deleted node
        delBufferObj->next = NULL;
        delBufferObj->inUse = false;
    }
}

static void _DRV_SDMMC_UpdateGeometry( DRV_SDMMC_OBJ* dObj )
{
    uint8_t i = 0;

    /* Update the Media Geometry Table */
    for (i = 0; i <= GEOMETRY_TABLE_ERASE_ENTRY; i++)
    {
        dObj->mediaGeometryTable[i].blockSize = 512;
        dObj->mediaGeometryTable[i].numBlocks = dObj->cardCtxt.discCapacity;
    }

    /* Update the Media Geometry Main Structure */
    /* Number of read, write and erase entries in the table */
    dObj->mediaGeometryObj.numReadRegions = 1,
    dObj->mediaGeometryObj.numWriteRegions = 1,
    dObj->mediaGeometryObj.numEraseRegions = 1,
    dObj->mediaGeometryObj.geometryTable = (SYS_MEDIA_REGION_GEOMETRY *)dObj->mediaGeometryTable;
}


static void _DRV_SDMMC_PlibCallbackHandler(
    DRV_SDMMC_XFER_STATUS xferStatus,
    uintptr_t contextHandle
)
{
    DRV_SDMMC_OBJ* dObj = (DRV_SDMMC_OBJ*)contextHandle;

    if (xferStatus & DRV_SDMMC_XFER_STATUS_CARD_REMOVED)
    {
        dObj->cardCtxt.isCommandCompleted = true;
        dObj->cardCtxt.isDataCompleted = true;
        dObj->cardCtxt.errorFlag |= DRV_SDMMC_CARD_REMOVAL_ERROR;
    }
    if (xferStatus & DRV_SDMMC_XFER_STATUS_COMMAND_COMPLETED)
    {
        dObj->cardCtxt.isCommandCompleted = true;
        dObj->cardCtxt.errorFlag |= dObj->sdmmcPlib->sdhostGetCommandError();
    }
    if (xferStatus & DRV_SDMMC_XFER_STATUS_DATA_COMPLETED)
    {
        dObj->cardCtxt.isDataCompleted = true;
        dObj->cardCtxt.errorFlag |= dObj->sdmmcPlib->sdhostGetDataError();
    }
}

static void _DRV_SDMMC_TimerCallback (
    uintptr_t context
)
{
    volatile bool* timeoutFlag = (bool* ) context;
    *timeoutFlag = true;
}

static void _DRV_SDMMC_InitCardContext ( uint32_t drvIndex, DRV_SDHOST_CARD_CTXT* cardCtxt )
{
    cardCtxt->isAttached            = false;
    cardCtxt->busWidth              = DRV_SDMMC_BUS_WIDTH_1_BIT;
    cardCtxt->cmd6Mode              = false;
    cardCtxt->voltWindow            = 0;
    cardCtxt->rca                   = 0;
    cardCtxt->cardVer               = 0;
    /* HC or Standard card. */
    cardCtxt->cardType              = DRV_SDMMC_CARD_TYPE_STANDARD;
    /* Capacity of the card in number of blocks. */
    cardCtxt->discCapacity          = 0;
    /* Variables to track the command/data status. */
    cardCtxt->isDataCompleted       = false;
    cardCtxt->isCommandCompleted    = false;
    cardCtxt->isWriteProtected      = false;
    cardCtxt->isLocked              = false;
    cardCtxt->errorFlag             = 0;


    memset (cardCtxt->cidBuffer, 0, DRV_SDMMC_CID_BUFFER_LEN);
    memset (cardCtxt->csdBuffer, 0, DRV_SDMMC_CSD_BUFFER_LEN);
    memset (cardCtxt->scrBuffer, 0, DRV_SDMMC_SCR_BUFFER_LEN);
    memset (cardCtxt->switchStatusBuffer, 0, DRV_SDMMC_SWITCH_STATUS_BUFFER_LEN);
}

static void _DRV_SDMMC_ParseCSD (
    uint8_t* csdPtr,
    DRV_SDHOST_CARD_CTXT* cardCtxt
)
{
    uint8_t cSizeMultiplier = 0;
    uint16_t blockLength = 0;
    uint32_t cSize = 0;
    uint32_t mult = 0;

    /* Note: The structure format depends on if it is a CSD V1 or V2 device.
       Therefore, need to first determine version of the specs that the card
       is designed for, before interpreting the individual fields.
    */

    /* Bits 127:126 */
    if (((csdPtr[14] >> 6) & 0x03) == 0x01)
    {
        /* CSD Version 2.0 */

        /* C_SIZE Bits 69:48 (22-bit) */
        cSize = (csdPtr[7] & 0x3F) << 16;
        cSize |= csdPtr[6] << 8;
        cSize |= csdPtr[5];

        cardCtxt->discCapacity = ((uint32_t)(cSize + 1) * (uint32_t)(1024));
    }
    else
    {
        /* CSD Version 1.0 */

        /* Memory capacity = BLOCKNR * BLOCK_LEN
         * BLOCKNR = (C_SIZE + 1) * MULT
         * MULT = 2 POW(C_SIZE_MULT + 2)
         * BLOCK_LEN = 2 POW(READ_BL_LEN)
         */

        /* READ_BL_LEN Bits 83:80 (4-bit) */
        blockLength = csdPtr[9] & 0x0F;
        blockLength = 1 << (blockLength - 9);

        /* CSIZE Bits 73:62 (12-bit) */
        cSize = (csdPtr[8] & 0x03) << 10;
        cSize |= csdPtr[7] << 2;
        cSize |= csdPtr[6] >> 6;

        /* C_SIZE_MULT Bits 49:47 (3-bit) */
        cSizeMultiplier = (csdPtr[4] & 0x03) << 1;
        cSizeMultiplier |= csdPtr[3] >> 7;

        mult = 1 << (cSizeMultiplier + 2);
        cardCtxt->discCapacity = (((uint32_t)(cSize + 1) * mult) * blockLength);
    }
}

static void _DRV_SDMMC_CommandSend (
    DRV_SDMMC_OBJ* dObj,
    uint8_t opCode,
    uint32_t argument,
    uint8_t respType,
    DRV_SDMMC_DataTransferFlags* dataTransferFlags
)
{
    switch (dObj->cmdState)
    {
        case DRV_SDMMC_CMD_EXEC_IS_COMPLETE:
        default:
            dObj->isCmdTimerExpired = false;
            dObj->cmdTimerHandle = SYS_TIME_CallbackRegisterMS (_DRV_SDMMC_TimerCallback, (uintptr_t)&dObj->isCmdTimerExpired, 2000, SYS_TIME_SINGLE);
            if(dObj->cmdTimerHandle == SYS_TIME_HANDLE_INVALID)
            {
                break;
            }
            dObj->cmdState = DRV_SDMMC_CMD_LINE_STATE_CHECK;
            /* Fall through to the next state. */

        case DRV_SDMMC_CMD_LINE_STATE_CHECK:
            if (dObj->sdmmcPlib->sdhostIsCmdLineBusy () == true)
            {
                /* Command line is busy. Wait for the line to become free. */
                if (dObj->sdmmcPlib->sdhostResetError != NULL)
                {
                    dObj->sdmmcPlib->sdhostResetError (DRV_SDMMC_RESET_CMD);
                }

                if (dObj->isCmdTimerExpired == true)
                {
                    /* Timer has expired. */
                    dObj->commandStatus = DRV_SDMMC_COMMAND_STATUS_TIMEOUT_ERROR;
                    dObj->cmdState = DRV_SDMMC_CMD_EXEC_IS_COMPLETE;
                }
                break;
            }

            if (dObj->sdmmcPlib->sdhostIsDatLineBusy() == true)
            {
                /* This command requires the use of the DAT line, but the
                 * DAT lines are busy. Wait for the lines to become free. */
                if (dObj->isCmdTimerExpired == true)
                {
                    /* Timer has expired. */
                    if (dObj->sdmmcPlib->sdhostResetError != NULL)
                    {
                        dObj->sdmmcPlib->sdhostResetError (DRV_SDMMC_RESET_DAT);
                    }
                    dObj->commandStatus = DRV_SDMMC_COMMAND_STATUS_TIMEOUT_ERROR;
                    dObj->cmdState = DRV_SDMMC_CMD_EXEC_IS_COMPLETE;
                }
                break;
            }

            //Command and data lines are available, now send the command
            dObj->cmdState = DRV_SDMMC_CMD_FRAME_AND_SEND_CMD;
            /* Fall through to the next case. */

        case DRV_SDMMC_CMD_FRAME_AND_SEND_CMD:

            dObj->cardCtxt.isCommandCompleted = false;
            dObj->cardCtxt.isDataCompleted = false;
            dObj->cardCtxt.errorFlag = 0;
            dObj->sdmmcPlib->sdhostSendCommand (opCode, argument, respType, *dataTransferFlags);
            dObj->cmdState = DRV_SDMMC_CMD_CHECK_TRANSFER_COMPLETE;
            break;

        case DRV_SDMMC_CMD_CHECK_TRANSFER_COMPLETE:

            if (dObj->cardCtxt.isCommandCompleted == true)
            {
                if (dObj->cardCtxt.errorFlag & DRV_SDMMC_ANY_COMMAND_ERRORS)
                {
                    if (dObj->cardCtxt.errorFlag & DRV_SDMMC_COMMAND_TIMEOUT_ERROR)
                    {
                        dObj->commandStatus = DRV_SDMMC_COMMAND_STATUS_TIMEOUT_ERROR;
                    }
                    else if (dObj->cardCtxt.errorFlag & DRV_SDMMC_COMMAND_CRC_ERROR)
                    {
                        dObj->commandStatus = DRV_SDMMC_COMMAND_STATUS_CRC_ERROR;
                    }
                    else
                    {
                        dObj->commandStatus = DRV_SDMMC_COMMAND_STATUS_ERROR;
                    }
                }
                else
                {
                    dObj->commandStatus = DRV_SDMMC_COMMAND_STATUS_SUCCESS;
                }

                /* Stop the timer. */
                SYS_TIME_TimerDestroy (dObj->cmdTimerHandle);

                /* Reset the state */
                dObj->cmdState = DRV_SDMMC_CMD_EXEC_IS_COMPLETE;
            }
            else
            {
                if (dObj->isCmdTimerExpired == true)
                {
                    dObj->commandStatus = DRV_SDMMC_COMMAND_STATUS_TIMEOUT_ERROR;
                    /* Reset the state */
                    dObj->cmdState = DRV_SDMMC_CMD_EXEC_IS_COMPLETE;
                    break;
                }
            }
            break;
    }
}

static void _DRV_SDMMC_MediaInitialize (
    DRV_SDMMC_OBJ* dObj
)
{
    uint32_t response = 0;

    switch (dObj->initState)
    {
        case DRV_SDMMC_INIT_SET_INIT_SPPED:
            if (dObj->cardCtxt.currentSpeed != DRV_SDMMC_CLOCK_FREQ_400_KHZ)
            {
                if (dObj->sdmmcPlib->sdhostSetClock(DRV_SDMMC_CLOCK_FREQ_400_KHZ) == true)
                {
                    dObj->cardCtxt.currentSpeed = DRV_SDMMC_CLOCK_FREQ_400_KHZ;
                    if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
                    {
                        dObj->initState = DRV_SDMMC_INIT_START_POLLING_TIMEOUT;
                    }
                }
            }
            else
            {
                if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
                {
                    dObj->initState = DRV_SDMMC_INIT_START_POLLING_TIMEOUT;
                }
            }
            break;

        case DRV_SDMMC_INIT_START_POLLING_TIMEOUT:
            if (SYS_TIME_DelayMS(dObj->cardDetectionPollingIntervalMs, &(dObj->tmrHandle)) == SYS_TIME_SUCCESS)
            {
                dObj->initState = DRV_SDMMC_INIT_WAIT_POLLING_TIMEOUT;
            }
            else
            {
                dObj->initState = DRV_SDMMC_INIT_ERROR;
            }
            break;

        case DRV_SDMMC_INIT_WAIT_POLLING_TIMEOUT:
            if (SYS_TIME_DelayIsComplete(dObj->tmrHandle) == true)
            {
                /* Delay has elapsed. */
                dObj->initState = DRV_SDMMC_INIT_RESET_CARD;
            }
            break;

        case DRV_SDMMC_INIT_RESET_CARD:
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_GO_IDLE_STATE, 0x0, DRV_SDMMC_CMD_RESP_NONE, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                /* Wait for approx. 2 ms after issuing the reset command. */
                if (SYS_TIME_DelayMS(2, &(dObj->tmrHandle)) == SYS_TIME_SUCCESS)
                {
                    dObj->initState = DRV_SDMMC_INIT_RESET_DELAY;
                }
            }
            break;

        case DRV_SDMMC_INIT_RESET_DELAY:
            if (SYS_TIME_DelayIsComplete(dObj->tmrHandle) == true)
            {
                /* Delay has elapsed. */
                dObj->initState = DRV_SDMMC_INIT_CHK_IFACE_CONDITION;
            }
            break;

        case DRV_SDMMC_INIT_CHK_IFACE_CONDITION:
            //VHS = 2.7-3.3V
            //Check Pattern = 0xAA
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SEND_IF_COND, 0x1AA, DRV_SDMMC_CMD_RESP_R7, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                dObj->initState = DRV_SDMMC_INIT_SEND_APP_CMD;
                dObj->nextInitState = DRV_SDMMC_INIT_SEND_ACMD41;
                dObj->cardCtxt.voltWindow = 0x00;
                if (SYS_TIME_DelayMS (2000, &dObj->generalTimerHandle) == SYS_TIME_ERROR)
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                    break;
                }
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse (DRV_SDMMC_READ_RESP_REG_0, &response);
                    if ((response & 0x1FF) == 0x1AA)
                    {
                        /* Card version is 2.0 or higher. */
                        dObj->cardCtxt.cardVer = 1;
                    }
                    else
                    {
                        /* Cannot initialize this card. */
                        dObj->initState = DRV_SDMMC_INIT_ERROR;
                    }
                }
                else
                {
                    if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_TIMEOUT_ERROR)
                    {
                        /* The card doesn't support CMD8.
                         * Either voltage mismatch Or Ver1.X SD Memory card
                         * Or Not a SD Card. */
                        dObj->cardCtxt.cardVer = 0;
                    }
                    else
                    {
                        /* Cannot initialize this card. */
                        dObj->initState = DRV_SDMMC_INIT_ERROR;
                    }
                }
            }
            break;

        case DRV_SDMMC_INIT_SEND_APP_CMD:

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_APP_CMD, (dObj->cardCtxt.rca << 16), DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->initState = dObj->nextInitState;
                }
                else
                {
                    /* Cannot initialize the SD Card. */
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_SEND_ACMD41:
            //ACMD41
            /* BIT30=> HCS (Host Capacity Support): 0 = SDSC, 1 = SDMMC/SDXC
             * For cards that respond to CMD8, host specifies the HCS bit as
             * either 0 or 1. For cards that do not respond to CMD8, the HCS bit
             * is ignored by the SD cards. Specify voltage range of 3.0 to 3.4V.
             */
            _DRV_SDMMC_CommandSend(dObj, DRV_SDMMC_CMD_SD_SEND_OP_COND, (dObj->cardCtxt.cardVer << 30) | (dObj->cardCtxt.voltWindow << 16), DRV_SDMMC_CMD_RESP_R3, &dObj->dataTransferFlags);

            /* Change from this state only on completion of command execution */
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (SYS_TIME_DelayIsComplete(dObj->generalTimerHandle) == true)
                {
                    /* Timed out while waiting for the card's internal state
                     * machine to get initialized. */
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                    break;
                }

                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse (DRV_SDMMC_READ_RESP_REG_0, &response);
                    /* Check if the card has set any one of the supported voltage range bits. */
                    if ((response & 0x3C0000) == 0)
                    {
                        /* Cannot initialize the card. */
                        dObj->initState = DRV_SDMMC_INIT_ERROR;
                    }
                    else
                    {
                        /* Check if cards internal initialization is complete. */
                        if (response & (1U << 31))
                        {
                            /* Check card capacity - CCS bit */
                            if (response & (1U << 30))
                            {
                                dObj->cardCtxt.cardType = DRV_SDMMC_CARD_TYPE_HC;
                            }
                            else
                            {
                                /* Byte addressing */
                                dObj->cardCtxt.cardType = DRV_SDMMC_CARD_TYPE_STANDARD;
                            }

                            /* Destroy the timer */
                            SYS_TIME_TimerDestroy(dObj->generalTimerHandle);

                            dObj->initState = DRV_SDMMC_INIT_ALL_SEND_CID;
                        }
                        else
                        {
                            /* Voltage window - 3.0V to 3.4V */
                            dObj->cardCtxt.voltWindow = 0x3C;

                            /* Repeat the APPCMD and the ACMD41 until the
                             * card sets the ready bit or until timeout. */
                            dObj->initState = DRV_SDMMC_INIT_SEND_APP_CMD;
                        }
                    }
                }
                else
                {
                    /* Cannot initialize the card. */
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_ALL_SEND_CID:
            //CMD2
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_ALL_SEND_CID, 0x00, DRV_SDMMC_CMD_RESP_R2, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse (DRV_SDMMC_READ_RESP_REG_ALL, (uint32_t *)&dObj->cardCtxt.cidBuffer[0]);
                    dObj->initState = DRV_SDMMC_INIT_PUBLISH_RCA;
                }
                else
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_PUBLISH_RCA:
            //CMD3 - Ask the SD Card to publish a relative card address (RCA)
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SEND_RCA, 0x00, DRV_SDMMC_CMD_RESP_R6, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse(DRV_SDMMC_READ_RESP_REG_0, &response);
                    if (response & 0xE000)
                    {
                        /* Status bits 23, 22 and 19. Command CRC error,
                         * illegal command and error respectively.  */
                        dObj->initState = DRV_SDMMC_INIT_ERROR;
                    }
                    else
                    {
                        dObj->cardCtxt.rca = ((response >> 16) & 0xFFFF);

                        if (dObj->cardCtxt.rca == 0)
                        {
                            /* Repeat this command until the card publishes
                             * a non-zero RCA. */
                            dObj->initState = DRV_SDMMC_INIT_PUBLISH_RCA;
                        }
                        else
                        {
                            dObj->initState = DRV_SDMMC_INIT_READ_CSD;
                        }
                    }
                }
                else
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_READ_CSD:
            //CMD9 - Read Card Specific Data (CSD) from SD Card
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SEND_CSD, (dObj->cardCtxt.rca << 16), DRV_SDMMC_CMD_RESP_R2, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse(DRV_SDMMC_READ_RESP_REG_ALL, (uint32_t *)&dObj->cardCtxt.csdBuffer[0]);
                    _DRV_SDMMC_ParseCSD (&dObj->cardCtxt.csdBuffer[0], &dObj->cardCtxt);
                    dObj->initState = DRV_SDMMC_INIT_CHANGE_CLK_FREQ;
                }
                else
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_CHANGE_CLK_FREQ:
            if (dObj->sdmmcPlib->sdhostSetClock(DRV_SDMMC_CLOCK_FREQ_DS_25_MHZ) == true)
            {
                dObj->cardCtxt.currentSpeed = DRV_SDMMC_CLOCK_FREQ_DS_25_MHZ;
                dObj->initState = DRV_SDMMC_INIT_SELECT_CARD;
            }
            else
            {
                dObj->initState = DRV_SDMMC_INIT_ERROR;
            }
            break;

        case DRV_SDMMC_INIT_SELECT_CARD:
            //CMD7 - Enter Transfer State
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SELECT_DESELECT_CARD, (dObj->cardCtxt.rca << 16), DRV_SDMMC_CMD_RESP_R1B, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->initState = DRV_SDMMC_INIT_SEND_APP_CMD;
                    dObj->nextInitState = DRV_SDMMC_INIT_CHK_CARD_STATE;
                }
                else
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_CHK_CARD_STATE:

            dObj->sdmmcPlib->sdhostReadResponse(DRV_SDMMC_READ_RESP_REG_0, &response);

            /* Check if the card is in the locked state. */
            if (response & (1 << 25))
            {
                dObj->cardCtxt.isLocked = true;

                /* Mark the initialization as complete. */
                dObj->initState = DRV_SDMMC_INIT_DONE;
                break;
            }

            /* Check if the card is in the transfer state. If not
             * then fail the card init. */
            if (response & 0x800)
            {
                dObj->initState = DRV_SDMMC_INIT_PRE_SCR_STATE;
            }
            else
            {
                /* Cannot initialize the SD Card. */
                dObj->initState = DRV_SDMMC_INIT_ERROR;
            }
            break;

        case DRV_SDMMC_INIT_PRE_SCR_STATE:
            // SCR response is 64-bits (or 8 bytes)
            /* Set the block length to 8 bytes. */
            dObj->sdmmcPlib->sdhostSetBlockSize (8);

            dObj->dataTransferFlags.isDataPresent = true;
            dObj->dataTransferFlags.transferDir = DRV_SDMMC_DATA_TRANSFER_DIR_READ;
            dObj->dataTransferFlags.transferType = DRV_SDMMC_DATA_TRANSFER_TYPE_SINGLE;

            /* Invalidate the cache to force the CPU to read the latest data
             * from the main memory. */
            SYS_CACHE_InvalidateDCache_by_Addr((uint32_t *)dObj->cardCtxt.scrBuffer, DRV_SDMMC_SCR_BUFFER_LEN);

            /* Set up the DMA for the data transfer. */
            dObj->sdmmcPlib->sdhostSetupDma (&dObj->cardCtxt.scrBuffer[0], 8, DRV_SDMMC_OPERATION_TYPE_READ);
            dObj->initState = DRV_SDMMC_INIT_SEND_SCR;
            break;

        case DRV_SDMMC_INIT_SEND_SCR:
            //ACMD51
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_READ_SCR, 0x00, DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                dObj->dataTransferFlags.isDataPresent = false;
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->initState = DRV_SDMMC_INIT_PARSE_SCR_DATA;
                }
                else
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_PARSE_SCR_DATA:

            /* Wait for the data phase to be completed. */
            if (dObj->cardCtxt.isDataCompleted == true)
            {
                /* Check if there are any data errors. */
                if (dObj->cardCtxt.errorFlag & DRV_SDMMC_ANY_DATA_ERRORS)
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
                else
                {

                    if ((dObj->cardCtxt.scrBuffer[1] & 0x04) && (dObj->busWidth == DRV_SDMMC_BUS_WIDTH_4_BIT))
                    {
                        /* The card supports 4-bit bus width mode. */
                        dObj->initState = DRV_SDMMC_INIT_SEND_APP_CMD;
                        dObj->nextInitState = DRV_SDMMC_INIT_SET_BUS_WIDTH;
                    }
                    else
                    {
                        dObj->initState = DRV_SDMMC_INIT_CARD_VER_CHECK;
                    }
                }
            }
            break;

        case DRV_SDMMC_INIT_SET_BUS_WIDTH:
            //ACMD6 - Set bus width (For this command, card must be in transfer state and not locked)
            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SET_BUS_WIDTH, 0x02, DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    /* Configure the controller to use 4-bit wide bus from now on. */
                    dObj->sdmmcPlib->sdhostSetBusWidth (DRV_SDMMC_BUS_WIDTH_4_BIT);
                    dObj->cardCtxt.busWidth = DRV_SDMMC_BUS_WIDTH_4_BIT;
                    dObj->initState = DRV_SDMMC_INIT_CARD_VER_CHECK;
                }
                else
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_CARD_VER_CHECK:

            if ((dObj->cardCtxt.scrBuffer[0] & 0x0F) && (dObj->speedMode == DRV_SDMMC_SPEED_MODE_HIGH))
            {
                /* Card follows SD Spec version 1.10 or higher */
                dObj->cardCtxt.cmd6Mode = 0;
                dObj->initState = DRV_SDMMC_INIT_PRE_SWITCH_CMD;
            }
            else
            {
                dObj->initState = DRV_SDMMC_INIT_SET_BLOCK_LENGTH;
            }
            break;

        case DRV_SDMMC_INIT_PRE_SWITCH_CMD:
            //Response to CMD6 is 512 bits (64 bytes)
            //Mode 0 - To query if the card supports a specific function
            //Mode 1 - To switch to the specific function
            /* Set the block length to 64 bytes. */
            dObj->sdmmcPlib->sdhostSetBlockSize (64);

            dObj->dataTransferFlags.isDataPresent = true;
            dObj->dataTransferFlags.transferDir = DRV_SDMMC_DATA_TRANSFER_DIR_READ;
            dObj->dataTransferFlags.transferType = DRV_SDMMC_DATA_TRANSFER_TYPE_SINGLE;

            /* Invalidate the cache to force the CPU to read the latest data
             * from the main memory. */
            SYS_CACHE_InvalidateDCache_by_Addr((uint32_t *)dObj->cardCtxt.switchStatusBuffer, DRV_SDMMC_SWITCH_STATUS_BUFFER_LEN);

            /* Set up the DMA for the data transfer. */
            dObj->sdmmcPlib->sdhostSetupDma (&dObj->cardCtxt.switchStatusBuffer[0], 64, DRV_SDMMC_OPERATION_TYPE_READ);
            dObj->initState = DRV_SDMMC_INIT_SWITCH_CMD;
            /* Fall through to the next case. */

        case DRV_SDMMC_INIT_SWITCH_CMD:
            //Mode[31]=> 0 = Check, 1 = Switch
            //[3:0] => Function Group 1 for Access Mode - Select function (0-0xF) in Function Group 1

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SWITCH_FUNCTION, (0x00FFFF01) | (dObj->cardCtxt.cmd6Mode << 31), DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                dObj->dataTransferFlags.isDataPresent = false;
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    /* Now wait for the data from the card. */
                    dObj->initState = DRV_SDMMC_INIT_SWITCH_STATUS;
                }
                else
                {
                    /* Command execution failed. */
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_SWITCH_STATUS:

            if (dObj->cardCtxt.isDataCompleted == true)
            {
                /* Check if there are any data errors. */
                if (dObj->cardCtxt.errorFlag & DRV_SDMMC_ANY_DATA_ERRORS)
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
                else
                {

                    /* Wait for the data transfer to complete. */
                    if (dObj->cardCtxt.cmd6Mode == 1)
                    {
                        /* Bits 379:376 should be set to 1 if the switch was successful. */
                        if ((dObj->cardCtxt.switchStatusBuffer[16] & 0x0F) == 0x01)
                        {
                            dObj->initState = DRV_SDMMC_INIT_SET_HS_FREQ;
                        }
                        else
                        {
                            dObj->initState = DRV_SDMMC_INIT_DONE;
                        }
                    }
                    else
                    {
                        /* Bit 401 should be set indicating HS support. */
                        if (dObj->cardCtxt.switchStatusBuffer[13] & 0x02)
                        {
                            /* The card supports switching to HS mode of operation. */
                            dObj->cardCtxt.cmd6Mode = 1;
                            dObj->initState = DRV_SDMMC_INIT_PRE_SWITCH_CMD;
                        }
                        else
                        {
                            dObj->initState = DRV_SDMMC_INIT_DONE;
                        }
                    }
                }
            }
            break;

        case DRV_SDMMC_INIT_SET_HS_FREQ:
            if (dObj->sdmmcPlib->sdhostSetClock(DRV_SDMMC_CLOCK_FREQ_HS_50_MHZ) == true)
            {
                dObj->cardCtxt.currentSpeed = DRV_SDMMC_CLOCK_FREQ_HS_50_MHZ;
                dObj->sdmmcPlib->sdhostSetSpeedMode (DRV_SDMMC_SPEED_MODE_HIGH);
                dObj->initState = DRV_SDMMC_INIT_SET_BLOCK_LENGTH;
            }
            else
            {
                dObj->initState = DRV_SDMMC_INIT_ERROR;
            }
            break;

        case DRV_SDMMC_INIT_SET_BLOCK_LENGTH:

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SET_BLOCKLEN, 512, DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->initState = DRV_SDMMC_INIT_DESELECT_CARD;
                }
                else
                {
                    dObj->initState = DRV_SDMMC_INIT_ERROR;
                }
            }
            break;

        case DRV_SDMMC_INIT_DESELECT_CARD:

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SELECT_DESELECT_CARD, 0, DRV_SDMMC_CMD_RESP_NONE, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                dObj->initState = DRV_SDMMC_INIT_DONE;
            }
            break;

        case DRV_SDMMC_INIT_DONE:
        case DRV_SDMMC_INIT_ERROR:

            SYS_TIME_TimerDestroy(dObj->generalTimerHandle);
            dObj->initState = DRV_SDMMC_INIT_SET_INIT_SPPED;
            break;

        default:

            dObj->initState = DRV_SDMMC_INIT_SET_INIT_SPPED;
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

__WEAK void DRV_SDMMC_RegisterWithSysFs(
    const SYS_MODULE_INDEX drvIndex
)
{

}

SYS_MODULE_OBJ DRV_SDMMC_Initialize (
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT* const init
)
{
    DRV_SDMMC_OBJ* dObj = NULL;
    const DRV_SDMMC_INIT* const sdmmcInit = (DRV_SDMMC_INIT *)init;

    /* Validate the driver index */
    if ((drvIndex >= DRV_SDMMC_INSTANCES_NUMBER) || (sdmmcInit == NULL))
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Is the driver instance already initialized? */
    if(gDrvSDMMCObj[drvIndex].inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    dObj = &gDrvSDMMCObj[drvIndex];

    /* Initialize the driver object's structure members */
    memset (dObj, 0, sizeof(DRV_SDMMC_OBJ));

    if(OSAL_MUTEX_Create(&dObj->mutex) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if(OSAL_MUTEX_Create(&dObj->mutexClientObjects) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    dObj->inUse                 = true;

    /* Attach to the peripheral SDMMC PLIB driver */
    dObj->sdmmcPlib                         = sdmmcInit->sdmmcPlib;
    dObj->bufferObjPool                     = sdmmcInit->bufferObjPool;
    dObj->bufferObjPoolSize                 = sdmmcInit->bufferObjPoolSize;
    dObj->clientObjPool                     = sdmmcInit->clientObjPool;
    dObj->nClientsMax                       = sdmmcInit->numClients;
    dObj->speedMode                         = sdmmcInit->speedMode;
    dObj->busWidth                          = sdmmcInit->busWidth;
    dObj->cardDetectionMethod               = sdmmcInit->cardDetectionMethod;
    dObj->cardDetectionPollingIntervalMs    = sdmmcInit->cardDetectionPollingIntervalMs;
    dObj->isWriteProtectCheckEnabled        = sdmmcInit->isWriteProtectCheckEnabled;
    dObj->sdmmcTokenCount                   = 1;
    dObj->initState                         = DRV_SDMMC_INIT_SET_INIT_SPPED;
    dObj->taskState                         = DRV_SDMMC_TASK_WAIT_FOR_DEVICE_ATTACH;
    dObj->cmdState                          = DRV_SDMMC_CMD_EXEC_IS_COMPLETE;
    dObj->mediaState                        = SYS_MEDIA_DETACHED;
    dObj->clockState                        = DRV_SDMMC_CLOCK_SET_DIVIDER;
    dObj->bufferObjList                     = (uintptr_t)NULL;
    dObj->isExclusive                       = false;
    dObj->isCmdTimerExpired                 = false;

    /* Register a callback with the underlying SDMMC PLIB */
    dObj->sdmmcPlib->sdhostCallbackRegister(_DRV_SDMMC_PlibCallbackHandler, (uintptr_t)dObj);

    if (sdmmcInit->isFsEnabled == true)
    {
        DRV_SDMMC_RegisterWithSysFs(drvIndex);
    }

    _DRV_SDMMC_InitCardContext((uint32_t)drvIndex, &dObj->cardCtxt);
    dObj->cardCtxt.currentSpeed = DRV_SDMMC_CLOCK_FREQ_400_KHZ;

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ((SYS_MODULE_OBJ)drvIndex);
}

SYS_STATUS DRV_SDMMC_Status (
    SYS_MODULE_OBJ object
)
{
    /* Validate the request */
    if( (object == SYS_MODULE_OBJ_INVALID) || (object >= DRV_SDMMC_INSTANCES_NUMBER) )
    {
        return SYS_STATUS_UNINITIALIZED;
    }

    return (gDrvSDMMCObj[object].status);
}

DRV_HANDLE DRV_SDMMC_Open (
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_OBJ* dObj = NULL;
    uint8_t iClient;

    /* Validate the driver index */
    if (drvIndex >= DRV_SDMMC_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvSDMMCObj[drvIndex];

    /* Guard against multiple threads trying to open the driver */
    if (OSAL_MUTEX_Lock(&dObj->mutexClientObjects , OSAL_WAIT_FOREVER ) == OSAL_RESULT_FALSE)
    {
        return DRV_HANDLE_INVALID;
    }

    if ((dObj->status != SYS_STATUS_READY) || (dObj->inUse == false))
    {
        OSAL_MUTEX_Unlock(&dObj->mutexClientObjects);
        return DRV_HANDLE_INVALID;
    }
    /* Flag error if:
     * Driver is already opened by maximum configured clients
     * Driver is already opened with exclusive access
     * Exclusive access is requested while the driver is already opened by other client*/

    if ((dObj->nClients == dObj->nClientsMax) ||
        (dObj->isExclusive == true) ||
        ((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE))
    )
    {
        OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));
        return DRV_HANDLE_INVALID;
    }

    for(iClient = 0; iClient != dObj->nClientsMax; iClient++)
    {
        clientObj = &((DRV_SDMMC_CLIENT_OBJ *)dObj->clientObjPool)[iClient];

        if(clientObj->inUse == false)
        {
            /* This means we have a free client object to use */
            clientObj->inUse = true;
            clientObj->intent = ioIntent;

            if(ioIntent & DRV_IO_INTENT_EXCLUSIVE)
            {
                /* Set the driver exclusive flag */
                dObj->isExclusive = true;
            }

            dObj->nClients++;

            /* Generate the client handle */
            clientObj->clientHandle = (DRV_HANDLE)_DRV_SDMMC_MAKE_HANDLE(dObj->sdmmcTokenCount, (uint8_t)drvIndex, iClient);

            /* Increment the instance specific token counter */
            dObj->sdmmcTokenCount = _DRV_SDMMC_UPDATE_TOKEN(dObj->sdmmcTokenCount);

            /* We have found a client object, now release the mutex */
            OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

            clientObj->eventHandler  = NULL;
            clientObj->context       = (uintptr_t)NULL;
            clientObj->drvIndex      = drvIndex;

            return clientObj->clientHandle;
        }
    }

    OSAL_MUTEX_Unlock(&(dObj->mutexClientObjects));

    return DRV_HANDLE_INVALID;
}

void DRV_SDMMC_Close (
    DRV_HANDLE handle
)
{
    DRV_SDMMC_OBJ* dObj = NULL;
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;

    clientObj = _DRV_SDMMC_DriverHandleValidate(handle);
    if (clientObj == NULL)
    {
        return;
    }

    dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];

    /* Guard against multiple threads trying to open/close the driver */
    if (OSAL_MUTEX_Lock(&dObj->mutexClientObjects , OSAL_WAIT_FOREVER ) == OSAL_RESULT_FALSE)
    {
        return;
    }

    _DRV_SDMMC_RemoveClientBuffersFromList (dObj, clientObj);

    /* Reduce the number of clients */
    dObj->nClients --;

    /* Reset the exclusive flag */
    dObj->isExclusive = false;

    /* Free the client object */
    clientObj->inUse = false;

    OSAL_MUTEX_Unlock(&dObj->mutexClientObjects);

}

void DRV_SDMMC_SetupXfer(
    const DRV_HANDLE handle,
    DRV_SDMMC_COMMAND_HANDLE* commandHandle,
    void* buffer,
    uint32_t blockStart,
    uint32_t nBlocks,
    DRV_SDMMC_OPERATION_TYPE opType
)
{
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_BUFFER_OBJ* bufferObj = NULL;
    DRV_SDMMC_OBJ* dObj = NULL;

    if (commandHandle)
    {
        *commandHandle = DRV_SDMMC_COMMAND_HANDLE_INVALID;
    }
    if ((buffer == NULL) || (nBlocks == 0))
    {
        return;
    }

    clientObj = _DRV_SDMMC_DriverHandleValidate(handle);

    if (clientObj == NULL)
    {
        return;
    }

    dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];

    if (dObj->mediaState != SYS_MEDIA_ATTACHED)
    {
        return;
    }

    if (opType == DRV_SDMMC_OPERATION_TYPE_READ)
    {
        if (!(clientObj->intent & DRV_IO_INTENT_READ))
        {
            return;
        }
        if (((blockStart + nBlocks) > dObj->mediaGeometryTable[GEOMETRY_TABLE_READ_ENTRY].numBlocks))
        {
            return;
        }
    }
    else
    {
        if (!(clientObj->intent & DRV_IO_INTENT_WRITE))
        {
            return;
        }
        if (((blockStart + nBlocks) > dObj->mediaGeometryTable[GEOMETRY_TABLE_WRITE_ENTRY].numBlocks))
        {
            return;
        }
    }

    if (OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        return;
    }

    bufferObj = _DRV_SDMMC_FreeBufferObjectGet(clientObj);

    if (bufferObj != NULL)
    {
        bufferObj->clientHandle  = handle;
        bufferObj->buffer        = buffer;
        bufferObj->blockStart    = blockStart;
        bufferObj->nBlocks       = nBlocks;
        bufferObj->opType        = opType;
        bufferObj->status        = DRV_SDMMC_COMMAND_QUEUED;

        if (commandHandle)
        {
            *commandHandle = bufferObj->commandHandle;
        }
        /* Add the buffer object to the linked list */
        _DRV_SDMMC_BufferObjectAddToList (dObj, bufferObj);
    }

    OSAL_MUTEX_Unlock(&dObj->mutex);
}

void DRV_SDMMC_AsyncRead (
    const DRV_HANDLE handle,
    DRV_SDMMC_COMMAND_HANDLE* commandHandle,
    void* targetBuffer,
    uint32_t blockStart,
    uint32_t nBlocks
)
{
    DRV_SDMMC_SetupXfer(
        handle,
        commandHandle,
        targetBuffer,
        blockStart,
        nBlocks,
        DRV_SDMMC_OPERATION_TYPE_READ
    );
}

void DRV_SDMMC_AsyncWrite
(
    const DRV_HANDLE handle,
    DRV_SDMMC_COMMAND_HANDLE* commandHandle,
    void* sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlocks
)
{
    DRV_SDMMC_SetupXfer(
        handle,
        commandHandle,
        sourceBuffer,
        blockStart,
        nBlocks,
        DRV_SDMMC_OPERATION_TYPE_WRITE
    );
}

DRV_SDMMC_COMMAND_STATUS DRV_SDMMC_CommandStatus (
    const DRV_HANDLE handle,
    const DRV_SDMMC_COMMAND_HANDLE commandHandle
)
{
    DRV_SDMMC_CLIENT_OBJ* clientObj;
    DRV_SDMMC_BUFFER_OBJ* bufferPool;
    DRV_SDMMC_OBJ* dObj;
    uint32_t bufferIndex;

    clientObj = _DRV_SDMMC_DriverHandleValidate (handle);

    if ((clientObj == NULL) || (commandHandle == DRV_SDMMC_COMMAND_HANDLE_INVALID))
    {
        return DRV_SDMMC_COMMAND_ERROR_UNKNOWN;
    }

    dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];

    bufferPool = (DRV_SDMMC_BUFFER_OBJ*)dObj->bufferObjPool;
    bufferIndex = commandHandle & DRV_SDMMC_INDEX_MASK;

    if (bufferIndex >= dObj->bufferObjPoolSize)
    {
        return DRV_SDMMC_COMMAND_ERROR_UNKNOWN;
    }

    /* Check if the buffer object is still valid */
    if (bufferPool[bufferIndex].commandHandle == commandHandle)
    {
        /* Return the last known buffer object status */
        return bufferPool[bufferIndex].status;
    }
    else
    {
        /* This means that object has been re-used by another request. Indicate
         * that the operation is completed.  */
        return (DRV_SDMMC_COMMAND_COMPLETED);
    }
}

SYS_MEDIA_GEOMETRY* DRV_SDMMC_GeometryGet (
    const DRV_HANDLE handle
)
{
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_OBJ* dObj = NULL;
    SYS_MEDIA_GEOMETRY* mediaGeometryObj = NULL;

    clientObj = _DRV_SDMMC_DriverHandleValidate (handle);

    if (clientObj != NULL)
    {
        dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];
        mediaGeometryObj = &dObj->mediaGeometryObj;
    }

    return mediaGeometryObj;
}

void DRV_SDMMC_EventHandlerSet (
    const DRV_HANDLE handle,
    const void* eventHandler,
    const uintptr_t context
)
{
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_OBJ* dObj = NULL;

    clientObj = _DRV_SDMMC_DriverHandleValidate (handle);

    if (clientObj != NULL)
    {
        dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];

        if (OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* Set the event handler */
            clientObj->eventHandler = (DRV_SDMMC_EVENT_HANDLER)eventHandler;
            clientObj->context = context;
            OSAL_MUTEX_Unlock(&dObj->mutex);
        }
    }
}

bool DRV_SDMMC_IsAttached (
    const DRV_HANDLE handle
)
{
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_OBJ* dObj = NULL;
    bool isAttached = false;

    clientObj = _DRV_SDMMC_DriverHandleValidate (handle);

    if (clientObj != NULL)
    {
        dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];

        if (dObj->mediaState == SYS_MEDIA_ATTACHED)
        {
            isAttached = true;
        }
    }

    return isAttached;
}

bool DRV_SDMMC_IsWriteProtected (
    const DRV_HANDLE handle
)
{
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_OBJ* dObj = NULL;
    bool isWriteProtected = false;

    clientObj = _DRV_SDMMC_DriverHandleValidate (handle);
    if (clientObj != NULL)
    {
        dObj = (DRV_SDMMC_OBJ* )&gDrvSDMMCObj[clientObj->drvIndex];
        if (dObj->mediaState == SYS_MEDIA_ATTACHED)
        {
            isWriteProtected = dObj->cardCtxt.isWriteProtected;
        }
    }

    return isWriteProtected;
}

void DRV_SDMMC_Tasks( SYS_MODULE_OBJ object )
{
    DRV_SDMMC_OBJ* dObj = NULL;
    DRV_SDMMC_CLIENT_OBJ* clientObj = NULL;
    DRV_SDMMC_BUFFER_OBJ* currentBufObj = NULL;
    DRV_SDMMC_EVENT evtStatus = DRV_SDMMC_EVENT_COMMAND_COMPLETE;
    uint32_t response = 0;
    static bool cardAttached = true;

    dObj = &gDrvSDMMCObj[object];

    if (OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "SDMMC Driver: OSAL_MUTEX_Lock failed");
    }

    currentBufObj = _DRV_SDMMC_BufferListGet(dObj);

    switch (dObj->taskState)
    {
        case DRV_SDMMC_TASK_WAIT_FOR_DEVICE_ATTACH:
            if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
            {
                /* Polling method available on SDHC and HSMCI PLIBs */
                _DRV_SDMMC_InitCardContext((uint32_t)object, &dObj->cardCtxt);
                dObj->sdmmcPlib->sdhostInitModule();
                dObj->cardCtxt.currentSpeed = DRV_SDMMC_CLOCK_FREQ_400_KHZ;

                /* PLIB does not provide with card attach/detach status.
                 * Attempt media initialization assuming that the media is present */
                dObj->taskState = DRV_SDMMC_TASK_MEDIA_INIT;
            }
            break;

        case DRV_SDMMC_TASK_MEDIA_INIT:

            _DRV_SDMMC_MediaInitialize (dObj);
            if (dObj->initState == DRV_SDMMC_INIT_DONE)
            {
                /* Check and update the card's write protected status */
                if (dObj->isWriteProtectCheckEnabled == true)
                {
                    if (dObj->sdmmcPlib->sdhostIsWriteProtected != NULL)
                    {
                        dObj->cardCtxt.isWriteProtected = dObj->sdmmcPlib->sdhostIsWriteProtected();
                    }
                    else
                    {
                        /* For HSMCI - Read from the configured GPIO pin */
                    }
                }
                else
                {
                    dObj->cardCtxt.isWriteProtected = false;
                }

                /* Update the Media Geometry structure */
                _DRV_SDMMC_UpdateGeometry (dObj);

                if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
                {
                    dObj->generalTimerHandle = SYS_TIME_HANDLE_INVALID;
                }

                dObj->mediaState = SYS_MEDIA_ATTACHED;
                dObj->taskState = DRV_SDMMC_TASK_PROCESS_QUEUE;
            }
            else if (dObj->initState == DRV_SDMMC_INIT_ERROR)
            {
                if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
                {
                    /* Polling method available on SDHC and HSMCI PLIBs */
                    _DRV_SDMMC_InitCardContext((uint32_t)object, &dObj->cardCtxt);
                }
            }
            break;

        case DRV_SDMMC_TASK_PROCESS_QUEUE:

            currentBufObj = _DRV_SDMMC_BufferListGet(dObj);
            if ((currentBufObj != NULL) && (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE))
            {
                if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
                {
                    SYS_TIME_TimerDestroy(dObj->generalTimerHandle);
                    dObj->generalTimerHandle = SYS_TIME_HANDLE_INVALID;
                }

                if (dObj->cardCtxt.isLocked == true)
                {
                    /* Card is locked. Fail the transaction. */
                    currentBufObj->status = DRV_SDMMC_COMMAND_ERROR_UNKNOWN;
                    dObj->taskState = DRV_SDMMC_TASK_ERROR;
                    break;
                }

                if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
                {
                    /* Assume card is attached */
                }
                else
                {
                    if (dObj->sdmmcPlib->sdhostIsCardAttached () == false)
                    {
                        /* Card has been removed. */
                        dObj->taskState = DRV_SDMMC_TASK_HANDLE_CARD_DETACH;
                        break;
                    }
                }

                currentBufObj->status = DRV_SDMMC_COMMAND_IN_PROGRESS;

                if (dObj->cardCtxt.cardType == DRV_SDMMC_CARD_TYPE_STANDARD)
                {
                    /* Card supports byte addressing. Convert the start
                     * address to byte address. */
                    currentBufObj->blockStart <<= 9;
                }

                if (currentBufObj->opType == DRV_SDMMC_OPERATION_TYPE_READ)
                {
                    dObj->dataTransferFlags.transferDir = DRV_SDMMC_DATA_TRANSFER_DIR_READ;
                    if (currentBufObj->nBlocks == 1)
                    {
                        currentBufObj->opCode = DRV_SDMMC_CMD_READ_SINGLE_BLOCK;
                        dObj->dataTransferFlags.transferType = DRV_SDMMC_DATA_TRANSFER_TYPE_SINGLE;
                    }
                    else
                    {
                        currentBufObj->opCode = DRV_SDMMC_CMD_READ_MULTI_BLOCK;
                        dObj->dataTransferFlags.transferType = DRV_SDMMC_DATA_TRANSFER_TYPE_MULTI;
                    }
                }
                else
                {
                    /* Fail the transfer if the card is write protected. */
                    if (dObj->cardCtxt.isWriteProtected == true)
                    {
                        currentBufObj->status = DRV_SDMMC_COMMAND_ERROR_UNKNOWN;
                        dObj->taskState = DRV_SDMMC_TASK_ERROR;
                        break;
                    }
                    else
                    {
                        dObj->dataTransferFlags.transferDir = DRV_SDMMC_DATA_TRANSFER_DIR_WRITE;
                        if (currentBufObj->nBlocks == 1)
                        {
                            currentBufObj->opCode = DRV_SDMMC_CMD_WRITE_SINGLE_BLOCK;
                            dObj->dataTransferFlags.transferType = DRV_SDMMC_DATA_TRANSFER_TYPE_SINGLE;
                        }
                        else
                        {
                            currentBufObj->opCode = DRV_SDMMC_CMD_WRITE_MULTI_BLOCK;
                            dObj->dataTransferFlags.transferType = DRV_SDMMC_DATA_TRANSFER_TYPE_MULTI;
                        }
                    }
                }

                dObj->taskState = DRV_SDMMC_TASK_SELECT_CARD;
            }
            else
            {
                if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
                {
                    /* Periodically check if the card is still attached. Start a timer first. */
                    if (dObj->generalTimerHandle == SYS_TIME_HANDLE_INVALID)
                    {
                        if (SYS_TIME_DelayMS (dObj->cardDetectionPollingIntervalMs, &dObj->generalTimerHandle) == SYS_TIME_ERROR)
                        {
                            dObj->taskState = DRV_SDMMC_TASK_ERROR;
                            break;
                        }
                    }
                    else
                    {
                        if (SYS_TIME_DelayIsComplete(dObj->generalTimerHandle) == true)
                        {
                            dObj->generalTimerHandle = SYS_TIME_HANDLE_INVALID;
                            /* Polling timeout has expired. Now check if the card is still attached. */
                            dObj->taskState = DRV_SDMMC_TASK_CHECK_CARD_DETACH;
                        }
                    }
                }

            }
            break;

        case DRV_SDMMC_TASK_CHECK_CARD_DETACH:
            dObj->dataTransferFlags.isDataPresent = false;

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SEND_STATUS, (dObj->cardCtxt.rca << 16), DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse (DRV_SDMMC_READ_RESP_REG_0, &response);
                    if (((response & 0x00001E00) >> 9) != 0x03)
                    {
                        /* Card is not in the expected state (standby state) */
                        dObj->taskState = DRV_SDMMC_TASK_HANDLE_CARD_DETACH;
                    }
                    else
                    {
                        /* Card is present and is in the standby state. Go back and check if any request is queued. */
                        dObj->taskState = DRV_SDMMC_TASK_PROCESS_QUEUE;
                    }
                }
                else
                {
                    /* The card is probably removed. Handle detach. */
                    dObj->taskState = DRV_SDMMC_TASK_HANDLE_CARD_DETACH;
                }
            }
            break;

        case DRV_SDMMC_TASK_SELECT_CARD:

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SELECT_DESELECT_CARD, (dObj->cardCtxt.rca << 16), DRV_SDMMC_CMD_RESP_R1B, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse (DRV_SDMMC_READ_RESP_REG_0, &response);
                    dObj->taskState = DRV_SDMMC_TASK_SETUP_XFER;
                }
                else
                {
                    dObj->taskState = DRV_SDMMC_TASK_ERROR;
                }
            }
            break;

        case DRV_SDMMC_TASK_SETUP_XFER:

            if (currentBufObj == NULL)
            {
                dObj->taskState = DRV_SDMMC_TASK_ERROR;
                break;
            }

            if (currentBufObj->nBlocks == 1)
            {
                /* For transfers involving only a single block of data the
                 * block count field needs to be set to zero. */
                dObj->sdmmcPlib->sdhostSetBlockCount (0);
            }
            else
            {
                /* Configure the Block Count register with the number of
                 * blocks to be transferred. */
                dObj->sdmmcPlib->sdhostSetBlockCount (currentBufObj->nBlocks);
            }

            /* Block count has already been set. */
            dObj->sdmmcPlib->sdhostSetBlockSize(512);

            if (currentBufObj->opType == DRV_SDMMC_OPERATION_TYPE_WRITE)
            {
                /* Clean the cache to push the data to be written, from the cache
                 * memory to the main memory for the DMA */
                SYS_CACHE_CleanDCache_by_Addr((uint32_t *)currentBufObj->buffer, (currentBufObj->nBlocks << 9));
            }
            else if (currentBufObj->opType == DRV_SDMMC_OPERATION_TYPE_READ)
            {
                /* Invalidate the cache to force the CPU to read the latest data
                 * from the main memory. */
                SYS_CACHE_InvalidateDCache_by_Addr((uint32_t *)currentBufObj->buffer, (currentBufObj->nBlocks << 9));
            }

            dObj->dataTransferFlags.isDataPresent = true;
            dObj->sdmmcPlib->sdhostSetupDma (currentBufObj->buffer, (currentBufObj->nBlocks << 9), currentBufObj->opType);
            dObj->taskState = DRV_SDMMC_TASK_XFER_COMMAND;

            /* Fall through to the next state */

        case DRV_SDMMC_TASK_XFER_COMMAND:

            _DRV_SDMMC_CommandSend (dObj, currentBufObj->opCode, currentBufObj->blockStart, DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                dObj->dataTransferFlags.isDataPresent = false;
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->taskState = DRV_SDMMC_TASK_XFER_STATUS;
                }
                else
                {
                    dObj->taskState = DRV_SDMMC_TASK_ERROR;
                }
            }
            break;

        case DRV_SDMMC_TASK_XFER_STATUS:

            /* Wait for the data transfer to get completed. */
            if (dObj->cardCtxt.isDataCompleted == true)
            {
                /* Check if there are any data errors. */
                if (dObj->cardCtxt.errorFlag & DRV_SDMMC_ANY_DATA_ERRORS)
                {
                    dObj->taskState = DRV_SDMMC_TASK_ERROR;
                }
                else
                {
                    if (currentBufObj != NULL)
                    {
                        /* Stop Transfer if multiple blocks are being
                         * transferred. CMD13 status check to ensure that
                         * there were no issues while performing the data
                         * transfer. */
                        if (currentBufObj->nBlocks > 1)
                        {
                            /* Send stop transmission command. */
                            dObj->taskState = DRV_SDMMC_TASK_SEND_STOP_TRANS_CMD;
                        }
                        else
                        {
                            dObj->taskState = DRV_SDMMC_TASK_CHECK_CARD_STATUS;
                        }
                    }
                    else
                    {
                        dObj->taskState = DRV_SDMMC_TASK_ERROR;
                    }
                }
            }
            break;

        case DRV_SDMMC_TASK_SEND_STOP_TRANS_CMD:

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_STOP_TRANSMISSION, 0, DRV_SDMMC_CMD_RESP_R1B, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->taskState = DRV_SDMMC_TASK_CHECK_CARD_STATUS;
                }
                else
                {
                    dObj->taskState = DRV_SDMMC_TASK_ERROR;
                }
            }
            break;

        case DRV_SDMMC_TASK_CHECK_CARD_STATUS:

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SEND_STATUS, (dObj->cardCtxt.rca << 16), DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                if (dObj->commandStatus == DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                {
                    dObj->sdmmcPlib->sdhostReadResponse (DRV_SDMMC_READ_RESP_REG_0, &response);
                    if (response & 0x100)
                    {
                        //Card is ready for new data. Corresponds to buffer empty signaling on the bus.
                        dObj->taskState = DRV_SDMMC_TASK_DESELECT_CARD;
                    }
                }
                else
                {
                    dObj->taskState = DRV_SDMMC_TASK_ERROR;
                }
            }
            break;

        case DRV_SDMMC_TASK_DESELECT_CARD:

            _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SELECT_DESELECT_CARD, 0, DRV_SDMMC_CMD_RESP_NONE, &dObj->dataTransferFlags);
            if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
            {
                currentBufObj->status = DRV_SDMMC_COMMAND_COMPLETED;
                dObj->taskState = DRV_SDMMC_TASK_TRANSFER_COMPLETE;
            }
            break;

        case DRV_SDMMC_TASK_ERROR:
            if (dObj->cardDetectionMethod == DRV_SDMMC_CD_METHOD_POLLING)
            {
                /* PLIB does not provide card attach/detach status (HSMCI), poll the status
                 * of the card by reading the card status from the card */

                dObj->dataTransferFlags.isDataPresent = false;

                _DRV_SDMMC_CommandSend (dObj, DRV_SDMMC_CMD_SEND_STATUS, (dObj->cardCtxt.rca << 16), DRV_SDMMC_CMD_RESP_R1, &dObj->dataTransferFlags);
                if (dObj->cmdState == DRV_SDMMC_CMD_EXEC_IS_COMPLETE)
                {
                    currentBufObj->status = DRV_SDMMC_COMMAND_ERROR_UNKNOWN;
                    dObj->taskState = DRV_SDMMC_TASK_TRANSFER_COMPLETE;

                    if (dObj->commandStatus != DRV_SDMMC_COMMAND_STATUS_SUCCESS)
                    {
                        /* The card is probably removed */
                        cardAttached = false;
                    }
                }
            }
            break;

        case DRV_SDMMC_TASK_TRANSFER_COMPLETE:

            if (currentBufObj != NULL)
            {
                /* Get the client object that owns this buffer */
                clientObj = &((DRV_SDMMC_CLIENT_OBJ *)dObj->clientObjPool)[currentBufObj->clientHandle & DRV_SDMMC_INDEX_MASK];

                /* Check if the client that submitted the request is active? */
                if (clientObj->clientHandle == currentBufObj->clientHandle)
                {
                    if (currentBufObj->status == DRV_SDMMC_COMMAND_COMPLETED)
                    {
                        evtStatus = DRV_SDMMC_EVENT_COMMAND_COMPLETE;
                    }
                    else
                    {
                        evtStatus = DRV_SDMMC_EVENT_COMMAND_ERROR;
                    }
                    if(clientObj->eventHandler != NULL)
                    {
                        /* Call the event handler */
                        clientObj->eventHandler((SYS_MEDIA_BLOCK_EVENT)evtStatus, currentBufObj->commandHandle, clientObj->context);
                    }
                }
                /* Free the completed buffer */
                _DRV_SDMMC_RemoveBufferObjFromList(dObj);
            }

            if (cardAttached)
            {
                dObj->taskState = DRV_SDMMC_TASK_PROCESS_QUEUE;
            }
            else
            {
                /* Reset the cardAttached back to true */
                cardAttached = true;
                dObj->taskState = DRV_SDMMC_TASK_HANDLE_CARD_DETACH;
            }
            break;

        case DRV_SDMMC_TASK_HANDLE_CARD_DETACH:

            // Remove the buffer objects queued by all clients on this driver instance
            _DRV_SDMMC_RemoveBufferObjects (dObj);

            dObj->mediaState = SYS_MEDIA_DETACHED;
            dObj->taskState = DRV_SDMMC_TASK_WAIT_FOR_DEVICE_ATTACH;
            break;

        default:
            break;
    }

    if (OSAL_MUTEX_Unlock(&dObj->mutex) != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "SDMMC Driver: OSAL_MUTEX_Unlock failed");
    }
}