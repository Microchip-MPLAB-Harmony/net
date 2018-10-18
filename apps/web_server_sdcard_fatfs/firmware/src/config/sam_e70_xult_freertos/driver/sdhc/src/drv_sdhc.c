/*******************************************************************************
  SD Host Controller Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdhc.c

  Summary:
    SD Host Controller Device Driver Dynamic Implementation

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
#include "driver/sdhc/drv_sdhc.h"
#include "driver/sdhc/src/drv_sdhc_local.h"
#include "driver/sdhc/src/drv_sdhc_host.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

uint8_t gDrvSDHCInitCount = 0;
OSAL_MUTEX_DECLARE(gDrvSDHCClientMutex);

/************************************************
 * This token is incremented for every request
 * added to the queue and is used to generate
 * a different buffer handle for every request.
 ***********************************************/
uint16_t gDrvSDHCBufferToken = 0;


/*************************************
 * SDHC driver geometry object
 ************************************/
SYS_MEDIA_GEOMETRY gDrvSDHCMediaGeometry;

/*****************************************
 * Media Geomtery Table.
 ****************************************/
SYS_MEDIA_REGION_GEOMETRY gDrvSDHCGeometryTable[3];

DRV_SDHC_BUFFER_OBJ gDrvSDHCBufferObj[DRV_SDHC_BUFFER_OBJ_NUMBER];

// *****************************************************************************
/* Driver Hardware instance objects.

  Summary:
    Defines the hardware instances objects that are available on the part

  Description:
    This data type defines the hardware instance objects that are available on
    the part, so as to capture the hardware state of the instance.

  Remarks:
    Not all modes are available on all micro-controllers.
*/

DRV_SDHC_OBJ gDrvSDHCObj[DRV_SDHC_INSTANCES_NUMBER];

// *****************************************************************************
/* Driver Client instance objects.

  Summary:
    Defines the Client instances objects that are available on the part

  Description:
    This data type defines the Client instance objects that are available on
    the part, so as to capture the Client state of the instance.

  Remarks:
    None
*/

DRV_SDHC_CLIENT_OBJ gDrvSDHCClientObj[DRV_SDHC_CLIENTS_NUMBER * DRV_SDHC_INSTANCES_NUMBER];

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static DRV_SDHC_CLIENT_OBJ * DRV_SDHC_ValidateDriverHandle
(
    DRV_HANDLE handle
)
{
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;
    DRV_SDHC_OBJ *dObj = NULL;

    if ((handle == DRV_HANDLE_INVALID) || (handle == 0))
    {
        return NULL;
    }

    clientObj = (DRV_SDHC_CLIENT_OBJ *)handle;
    if (clientObj->inUse == false)
    {
        return NULL;
    }

    /* Check if the driver is ready for operation */
    dObj = (DRV_SDHC_OBJ *)clientObj->driverObj;
    if (dObj->status != SYS_STATUS_READY)
    {
        return NULL;
    }

    return clientObj;
}

static void DRV_SDHC_UpdateGeometry
(
    DRV_SDHC_OBJ *dObj
)
{
    uint8_t i = 0;

    /* Update the Media Geometry Table */
    for (i = 0; i <= GEOMETRY_TABLE_ERASE_ENTRY; i++) 
    {
        gDrvSDHCGeometryTable[i].blockSize = 512;
        gDrvSDHCGeometryTable[i].numBlocks = dObj->cardCtxt->discCapacity;
    }

    /* Update the Media Geometry Main Structure */
    /* Number of read, write and erase entries in the table */
    gDrvSDHCMediaGeometry.numReadRegions = 1,
    gDrvSDHCMediaGeometry.numWriteRegions = 1,
    gDrvSDHCMediaGeometry.numEraseRegions = 1,
    gDrvSDHCMediaGeometry.geometryTable = (SYS_MEDIA_REGION_GEOMETRY *)&gDrvSDHCGeometryTable;
}

/* This function finds a free buffer object and populates it with the transfer
 * parameters. It also generates a new command handle for the request. */
static DRV_SDHC_BUFFER_OBJ* DRV_SDHC_AllocateBufferObject
(
    DRV_SDHC_CLIENT_OBJ *clientObj,
    void *buffer,
    uint32_t blockStart,
    uint32_t nBlocks,
    DRV_SDHC_OPERATION_TYPE opType
)
{
    uint8_t iEntry = 0;
    DRV_SDHC_BUFFER_OBJ *bufferObj = NULL;

    for (iEntry = 0; iEntry < DRV_SDHC_BUFFER_OBJ_NUMBER; iEntry++)
    {
        /* Search for a free buffer object to use */
        if (gDrvSDHCBufferObj[iEntry].inUse == false)
        {
            /* Found a free buffer object. */
            bufferObj = &gDrvSDHCBufferObj[iEntry];

            bufferObj->inUse         = true;
            bufferObj->commandHandle = DRV_SDHC_MAKE_HANDLE(gDrvSDHCBufferToken, iEntry);
            bufferObj->hClient       = clientObj;
            bufferObj->buffer        = buffer;
            bufferObj->blockStart    = blockStart;
            bufferObj->nBlocks       = nBlocks;
            bufferObj->opType        = opType;
            bufferObj->status        = DRV_SDHC_COMMAND_QUEUED;
            bufferObj->next          = NULL;
            bufferObj->previous      = NULL;

            /* Update the token number. */
            DRV_SDHC_UPDATE_BUF_TOKEN(gDrvSDHCBufferToken);
            break;
        }
    }

    return bufferObj;
}


/* This function finds and allocates a client object. */
static DRV_SDHC_CLIENT_OBJ* DRV_SDHC_AllocateClientObject
(
    void
)
{
    uint8_t count = 0;
    DRV_SDHC_CLIENT_OBJ *clientObj = &gDrvSDHCClientObj[0];

    for (count = 0; count < DRV_SDHC_CLIENTS_NUMBER; count++)
    {
        if (!clientObj->inUse)
        {
            return clientObj;
        }

        clientObj++;
    }

    return NULL;
}

/* This function adds the buffer object to the tail of the queue. */
static void DRV_SDHC_AddToQueue
(
    DRV_SDHC_OBJ *dObj,
    DRV_SDHC_BUFFER_OBJ *bufferObj
)
{
    if (dObj->queue == NULL)
    {
        dObj->queue = bufferObj;    
    }
    else
    {
        if (dObj->queue->previous != NULL)
        {
            dObj->queue->previous->next = bufferObj;
            bufferObj->previous = dObj->queue->previous;
            dObj->queue->previous = bufferObj;
        }
        else
        {
            dObj->queue->previous = bufferObj;
            dObj->queue->next = bufferObj;
            bufferObj->previous = dObj->queue;
        }
    }
}

/* This function updates the head of the queue. */
static void DRV_SDHC_UpdateQueue
(
    DRV_SDHC_OBJ *dObj
)
{
    DRV_SDHC_BUFFER_OBJ * bufferObj = dObj->queue;

    if (dObj->queue != NULL)
    {
        bufferObj->inUse = false;
        if (dObj->queue->next != NULL)
        {
            dObj->queue = dObj->queue->next;
            if (dObj->queue == bufferObj->previous)
            {
                dObj->queue->previous = NULL;
            }
            else
            {
                dObj->queue->previous = bufferObj->previous;
            }
            bufferObj->previous = NULL;
        }
        else
        {
            dObj->queue->previous = NULL;
            dObj->queue = NULL;
        }
    }

    return;
}

static void DRV_SDHC_RemoveBufferObjects
(
    DRV_SDHC_OBJ *dObj
)
{
    DRV_SDHC_BUFFER_OBJ *bufferObject = NULL;
    DRV_SDHC_BUFFER_OBJ *temp = NULL;
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;

    bufferObject = dObj->queue;
    while (bufferObject != NULL)
    {
        temp = bufferObject;
        bufferObject = bufferObject->next;

        clientObj = (DRV_SDHC_CLIENT_OBJ *)bufferObject->hClient;
        if (clientObj->eventHandler != NULL)
        {
            /* Call the event handler */
            clientObj->eventHandler(DRV_SDHC_EVENT_COMMAND_ERROR, temp->commandHandle, clientObj->context);
        }

        temp->inUse = false;
        temp->previous = NULL;
        temp->next = NULL;
    }
}

static void DRV_SDHC_RemoveClientBufferObjects
(
    DRV_SDHC_CLIENT_OBJ *clientObj,
    DRV_SDHC_OBJ *dObj
)
{
    DRV_SDHC_BUFFER_OBJ *bufferObject = NULL;
    DRV_SDHC_BUFFER_OBJ *lastObject = NULL;
    DRV_SDHC_BUFFER_OBJ *head = NULL;
    DRV_SDHC_BUFFER_OBJ *temp = NULL;

    bufferObject = dObj->queue;

    if (dObj->queue != NULL)
    {
        dObj->queue->previous = NULL;
    }

    while (bufferObject != NULL)
    {
        temp = bufferObject->next;
        if (bufferObject->hClient == clientObj)
        {
            bufferObject->inUse = false;

            if (clientObj->eventHandler != NULL)
            {
                /* Call the event handler */
                clientObj->eventHandler(DRV_SDHC_EVENT_COMMAND_ERROR, bufferObject->commandHandle, clientObj->context);
            }

            if(bufferObject->previous != NULL)
            {
                bufferObject->previous->next = bufferObject->next;
            }

            if(bufferObject->next != NULL)
            {
                bufferObject->next->previous = bufferObject->previous;
            }

            bufferObject->previous = NULL;
            bufferObject->next = NULL;
        }
        else
        {
            if (head == NULL)
            {
                head = bufferObject;
            }

            lastObject = bufferObject;
        }

        bufferObject = temp;
    }

    dObj->queue = head;

    if ((head != NULL) && (head != lastObject))
    {
        dObj->queue->previous = lastObject;
    }
}

static void DRV_SDHC_SetClock
(
    DRV_SDHC_OBJ *dObj,
    uint32_t clock
)
{
    switch (dObj->clockState)
    {
        case DRV_SDHC_CLOCK_SET_COMPLETE:
        case DRV_SDHC_CLOCK_SET_DIVIDER:
        default:
            {
                sdhostSetClock(clock);
				dObj->tmrHandle = SYS_TIME_HANDLE_INVALID;
				if (SYS_TIME_DelayMS(5, &(dObj->tmrHandle)) != SYS_TIME_SUCCESS)
       			{
           			dObj->clockState = DRV_SDHC_CLOCK_SET_DIVIDER;
       			}
				else
				{
					dObj->clockState = DRV_SDHC_CLOCK_PRE_ENABLE_DELAY;
				}				
                break;
            }

        case DRV_SDHC_CLOCK_PRE_ENABLE_DELAY:
            {
                if (SYS_TIME_DelayIsComplete(dObj->tmrHandle))
                {
                    /* Delay is elapsed. */
                    dObj->clockState = DRV_SDHC_CLOCK_ENABLE;
                }
                break;
            }

        case DRV_SDHC_CLOCK_ENABLE:
            {
                sdhostClockEnable ();   
				dObj->tmrHandle = SYS_TIME_HANDLE_INVALID;
				if ( SYS_TIME_DelayMS(5, &(dObj->tmrHandle)) != SYS_TIME_SUCCESS)
       			{
           			dObj->clockState = DRV_SDHC_CLOCK_ENABLE;
       			}
				else
				{
					dObj->clockState = DRV_SDHC_CLOCK_POST_ENABLE_DELAY;
				}
                break;
            }

        case DRV_SDHC_CLOCK_POST_ENABLE_DELAY:
            {
                if (SYS_TIME_DelayIsComplete(dObj->tmrHandle))
                {
                    /* Delay is elapsed. */
                    dObj->clockState = DRV_SDHC_CLOCK_SET_COMPLETE;
                }
                break;
            }
    }
}

static void DRV_SDHC_TimerCallback
(
    uintptr_t context
)
{
    volatile bool *flag = (bool *) context;
    *flag = true;
}

static void DRV_SDHC_CommandSend
(
    DRV_SDHC_OBJ *dObj,
    uint8_t opCode,
    uint32_t argument,
    uint8_t respType,
    uint8_t flags
)
{
    switch (dObj->cmdState)
    {
        case DRV_SDHC_CMD_EXEC_IS_COMPLETE:
        default:
            {
                dObj->cmdTimerState = false;
                dObj->cmdTimerHandle = SYS_TIME_CallbackRegisterMS (DRV_SDHC_TimerCallback, (uintptr_t)&dObj->cmdTimerState, 10, SYS_TIME_SINGLE);
				if(dObj->cmdTimerHandle == SYS_TIME_HANDLE_INVALID)
				{
					break;
				}
				dObj->cmdState = DRV_SDHC_CMD_LINE_STATE_CHECK;
                /* Fall through to the next state. */

            }

        case DRV_SDHC_CMD_LINE_STATE_CHECK:
            {
                if (sdhostIsCmdLineBusy () == true)
                {
                    /* Command line is busy. Wait for the line to become free. */
                    sdhostResetError (SDHOST_RESET_CMD);

                    if (dObj->cmdTimerState == true)
                    {
                        /* Timer has expired. */
                        dObj->cardCtxt->commandError = true;
                        dObj->cmdState = DRV_SDHC_CMD_CHECK_TRANSFER_COMPLETE;
                    }
                    break;
                }
                
                if (sdhostIsDat0LineBusy() == true)
                {
                    /* This command requires the use of the DAT line, but the
                     * DAT lines are busy. Wait for the lines to become free. */
                    if (dObj->cmdTimerState == true)
                    {
                        /* Timer has expired. */
                        sdhostResetError (SDHOST_RESET_DAT);
                        dObj->cardCtxt->commandError = true;
                        dObj->cmdState = DRV_SDHC_CMD_CHECK_TRANSFER_COMPLETE;
                    }
                    break;
                }

                dObj->cmdState = DRV_SDHC_CMD_FRAME_AND_SEND_CMD;
                /* Fall through to the next case. */
            }

        case DRV_SDHC_CMD_FRAME_AND_SEND_CMD:
            {
                dObj->cardCtxt->commandCompleted = false;
                dObj->cardCtxt->commandError = false;
                dObj->cardCtxt->waitForCmdResp = false;
                dObj->cardCtxt->dataCompleted = false;
                dObj->cardCtxt->isDataPresent = false;
                dObj->cardCtxt->errorFlag = 0;
                sdhostSendCommand (opCode, respType, flags, argument);
                dObj->cmdState = DRV_SDHC_CMD_CHECK_TRANSFER_COMPLETE;
                break;
            }

        case DRV_SDHC_CMD_CHECK_TRANSFER_COMPLETE:
            {
                if ((dObj->cardCtxt->commandCompleted == false) && (dObj->cardCtxt->commandError == false))
                {
                    if (dObj->cmdTimerState == true)
                    {
                        /* Timer has expired. */
                        dObj->cardCtxt->commandError = true;
                    }
                    else
                    {
                        /* Wait for the command to be completed. */
                        break;
                    }
                }

                /* Stop the timer. */
                SYS_TIME_TimerDestroy(dObj->cmdTimerHandle);
                dObj->cmdTimerState = false;

                if (dObj->cardCtxt->commandError)
                {
                    if (dObj->cardCtxt->errorFlag & DRV_SDHC_COMMAND_TIMEOUT_ERROR)
                    {
                        dObj->commandStatus = DRV_SDHC_COMMAND_STATUS_TIMEOUT_ERROR;
                    }
                    else  if (dObj->cardCtxt->errorFlag & DRV_SDHC_COMMAND_CRC_ERROR)
                    {
                        dObj->commandStatus = DRV_SDHC_COMMAND_STATUS_CRC_ERROR;
                    }
                    else 
                    {
                        dObj->commandStatus = DRV_SDHC_COMMAND_STATUS_ERROR;
                    }
                }
                else
                {
                    dObj->commandStatus = DRV_SDHC_COMMAND_STATUS_SUCCESS;
                }

                /* Reset the variables. */
                dObj->cmdState = DRV_SDHC_CMD_EXEC_IS_COMPLETE;
                break;
            }
    }
}

static void DRV_SDHC_ParseCSD
(
    DRV_SDHC_OBJ *dObj
)
{
    sdhostParseCsd(dObj->cardCtxt);
}

static void DRV_SDHC_MediaInitialize
(
    DRV_SDHC_OBJ *dObj
)
{
    uint32_t response = 0;

    switch (dObj->initState)
    {
        case DRV_SDHC_INIT_RESET_CARD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_GO_IDLE_STATE, 0x0, SDHOST_CMD_RESP_NONE, 0x0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    dObj->initState = DRV_SDHC_INIT_RESET_DELAY;
	                    /* Wait for approx. 2 ms after issuing the reset command. */
					dObj->tmrHandle = SYS_TIME_HANDLE_INVALID;
					if (SYS_TIME_DelayMS(2, &(dObj->tmrHandle)) != SYS_TIME_SUCCESS)
	       			{
	           			dObj->initState = DRV_SDHC_INIT_RESET_CARD;
	       			}
					else
					{
						dObj->initState = DRV_SDHC_INIT_RESET_DELAY;
					}
                }
                break;
            }

        case DRV_SDHC_INIT_RESET_DELAY:
            {
                if (SYS_TIME_DelayIsComplete(dObj->tmrHandle))
                {
                    /* Delay is elapsed. */
                    dObj->initState = DRV_SDHC_INIT_CHK_IFACE_CONDITION;
                }
                
                break;
            }

        case DRV_SDHC_INIT_CHK_IFACE_CONDITION:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SEND_IF_COND, 0x1AA, SDHOST_CMD_RESP_R7, 0x0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    dObj->initState = DRV_SDHC_INIT_SEND_APP_CMD;
                    dObj->nextInitState = DRV_SDHC_INIT_SEND_ACMD41;
                    dObj->cardCtxt->voltWindow = 0x00;
                    dObj->trials = 2000;
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        sdhostReadResponse (SDHOST_READ_RESP_REG_0, &response);
                        if ((response & 0x1FF) == 0x1AA)
                        {
                            /* Card version is 2.0 or higher. */
                            dObj->cardCtxt->cardVer = 1;
                        }
                        else
                        {
                            /* Cannot initialize this card. */
                            dObj->initState = DRV_SDHC_INIT_ERROR;
                        }
                    }
                    else
                    {
                        if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_TIMEOUT_ERROR)
                        {
                            /* The card doesn't support CMD8 */
                            dObj->cardCtxt->cardVer = 0;
                        }
                        else
                        {
                            /* Cannot initialize this card. */
                            dObj->initState = DRV_SDHC_INIT_ERROR;
                        }
                    }
                }

                break;
            }

        case DRV_SDHC_INIT_SEND_APP_CMD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_APP_CMD, (dObj->cardCtxt->rca << 16), SDHOST_CMD_RESP_R1, 0x0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        dObj->initState = dObj->nextInitState;
                    }
                    else
                    {
                        /* Cannot initialize the SD Card. */
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }

                break;
            }

        case DRV_SDHC_INIT_SEND_ACMD41:
            {
                DRV_SDHC_CommandSend(dObj, DRV_SDHC_CMD_SD_SEND_OP_COND, (dObj->cardCtxt->cardVer << 30) | (dObj->cardCtxt->voltWindow << 16), SDHOST_CMD_RESP_R3, 0x00);

                /* Change from this state only on completion of command execution */
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->trials == 0)
                    {
                        /* Timed out while waiting for the card's internal state
                         * machine to get initialized. */

                        dObj->initState = DRV_SDHC_INIT_ERROR;
                        break;
                    }

                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        sdhostReadResponse (SDHOST_READ_RESP_REG_0, &response);
                        /* Check if the card has set any one of the supported voltage range bits. */
                        if ((response & 0x3C0000) == 0)
                        {
                            /* Cannot initialize the card. */
                            dObj->initState = DRV_SDHC_INIT_ERROR;
                        }
                        else
                        {
                            if (response & (1 << 31))
                            {
                                /* Cards internal initialization is complete. */
                                if (response & (1 << 30))
                                {
                                    dObj->cardCtxt->cardType = DRV_SDHC_MODE_HC;
                                }
                                else
                                {
                                    /* Byte addressing */
                                    dObj->cardCtxt->cardType = DRV_SDHC_MODE_NORMAL;
                                }

                                dObj->initState = DRV_SDHC_INIT_ALL_SEND_CID;
                            }
                            else
                            {
                                /* Voltage window - 3.4V to 3.0V */
                                dObj->cardCtxt->voltWindow = 0x3C;

                                /* Decrement the trial count. */
                                dObj->trials --;
                                /* Repeat the APPCMD and the ACMD41 until the
                                 * card sets the ready bit or until timeout. */
                                dObj->initState = DRV_SDHC_INIT_SEND_APP_CMD;
                            }
                        }
                    }
                    else
                    {
                        /* Cannot initialize the card. */
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }

                break;
            }

        case DRV_SDHC_INIT_ALL_SEND_CID:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_ALL_SEND_CID, 0x00, SDHOST_CMD_RESP_R2, 0x0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        sdhostReadResponse (SDHOST_READ_RESP_REG_ALL, (uint32_t *)&dObj->cardCtxt->cid[0]);
                        dObj->initState = DRV_SDHC_INIT_PUBLISH_RCA;
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_INIT_PUBLISH_RCA:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SEND_RCA, 0x00, SDHOST_CMD_RESP_R6, 0x0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        sdhostReadResponse(SDHOST_READ_RESP_REG_0, &response);
                        if (response & 0xE000)
                        {
                            /* Status bits 23, 22 and 19. Command CRC error,
                             * illegal command and error respectively.  */
                            dObj->initState = DRV_SDHC_INIT_ERROR;
                        }
                        else
                        {
                            dObj->cardCtxt->rca = ((response >> 16) & 0xFFFF);

                            if (dObj->cardCtxt->rca == 0)
                            {
                                /* Repeat this command until the card publishes
                                 * a non-zero RCA. */
                                dObj->initState = DRV_SDHC_INIT_PUBLISH_RCA;
                            }
                            else
                            {
                                dObj->initState = DRV_SDHC_INIT_READ_CSD;
                            }
                        }
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_INIT_READ_CSD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SEND_CSD, (dObj->cardCtxt->rca << 16), SDHOST_CMD_RESP_R2, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        sdhostReadResponse(SDHOST_READ_RESP_REG_ALL, (uint32_t *)&dObj->cardCtxt->csd[0]);
                        DRV_SDHC_ParseCSD (dObj);
                        dObj->initState = DRV_SDHC_INIT_CHANGE_CLK_FREQ;
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_INIT_CHANGE_CLK_FREQ:
            {
                //sdhostSetClock (25000000);
                DRV_SDHC_SetClock (dObj, SDHOST_CLOCK_FREQ_DS_25_MHZ);
                if (dObj->clockState == DRV_SDHC_CLOCK_SET_COMPLETE)
                {
                    dObj->initState = DRV_SDHC_INIT_SELECT_CARD;
                }
                break;
            }

        case DRV_SDHC_INIT_SELECT_CARD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SELECT_DESELECT_CARD, (dObj->cardCtxt->rca << 16), SDHOST_CMD_RESP_R1B, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        dObj->initState = DRV_SDHC_INIT_SEND_APP_CMD;
                        dObj->nextInitState = DRV_SDHC_INIT_CHK_CARD_STATE;
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }

                break;
            }

        case DRV_SDHC_INIT_CHK_CARD_STATE:
            {
                sdhostReadResponse(SDHOST_READ_RESP_REG_0, &response);

                /* Check if the card is in the locked state. */
                if (response & (1 << 25))
                {
                    dObj->cardCtxt->locked = true;

                    /* Mark the initialization as complete. */
                    dObj->initState = DRV_SDHC_INIT_DONE;
                    break;
                }

                /* Check if the card is in the transfer state. If not
                 * then fail the card init. */
                if (response & 0x800)
                {
                    dObj->initState = DRV_SDHC_INIT_PRE_SCR_STATE;
                }
                else
                {
                    /* Cannot initialize the SD Card. */
                    dObj->initState = DRV_SDHC_INIT_ERROR;
                }
                break;
            }

        case DRV_SDHC_INIT_PRE_SCR_STATE:
            {
                /* Set the block length to 8 bytes. */
                sdhostSetBlockSize (8);

                /* Set up the DMA for the data transfer. */
                sdhostSetupDma (&dObj->cardCtxt->scr[0], 8, DRV_SDHC_OPERATION_TYPE_READ);
                dObj->initState = DRV_SDHC_INIT_SEND_SCR;
                break;
            }

        case DRV_SDHC_INIT_SEND_SCR:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_READ_SCR, 0x00, SDHOST_CMD_RESP_R1, 0x1);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        dObj->initState = DRV_SDHC_INIT_PARSE_SCR_DATA;
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_INIT_PARSE_SCR_DATA:
            {
                /* Wait for the data phase to be completed. */
                if (dObj->cardCtxt->dataCompleted == true)
                {
                    if ((dObj->cardCtxt->scr[1] & 0x04) && (dObj->busWidth == DRV_SDHC_BUS_WIDTH_4_BIT))
                    {
                        /* The card supports 4-bit bus width mode. */
                        dObj->initState = DRV_SDHC_INIT_SEND_APP_CMD;
                        dObj->nextInitState = DRV_SDHC_INIT_SET_BUS_WIDTH;
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_CARD_VER_CHECK;
                    }

                    dObj->cardCtxt->dataCompleted = false;
                }
                break;
            }

        case DRV_SDHC_INIT_SET_BUS_WIDTH:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SET_BUS_WIDTH, 0x02, SDHOST_CMD_RESP_R1, 0x0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        /* Configure the controller to use 4-bit wide bus from now on. */
                        sdhostSetBusWidth (SDHOST_BUS_WIDTH_4_BIT);
                        dObj->cardCtxt->busWidth = DRV_SDHC_BUS_WIDTH_4_BIT;
                        dObj->initState = DRV_SDHC_INIT_CARD_VER_CHECK;
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_INIT_CARD_VER_CHECK:
            {
                if ((dObj->cardCtxt->scr[0] & 0x0F) && (dObj->speedMode == DRV_SDHC_SPEED_MODE_HIGH))
                {
                    /* Card follows SD Spec version 1.10 or higher */
                    dObj->cardCtxt->cmd6Mode = 0;
                    dObj->initState = DRV_SDHC_INIT_PRE_SWITCH_CMD;
                }
                else
                {
                    dObj->initState = DRV_SDHC_INIT_SET_BLOCK_LENGTH;
                }

                break;
            }

        case DRV_SDHC_INIT_PRE_SWITCH_CMD:
            {
                /* Set the block length to 64 bytes. */
                sdhostSetBlockSize (64);

                /* Set up the DMA for the data transfer. */
                sdhostSetupDma (&dObj->cardCtxt->switchStatus[0], 64, DRV_SDHC_OPERATION_TYPE_READ);
                dObj->initState = DRV_SDHC_INIT_SWITCH_CMD;

                /* Fall through to the next case. */
            }

        case DRV_SDHC_INIT_SWITCH_CMD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SWITCH_FUNCTION, (0x00FFFF01) | (dObj->cardCtxt->cmd6Mode << 31), SDHOST_CMD_RESP_R1, 0x1);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        /* Now wait for the data from the card. */
                        dObj->initState = DRV_SDHC_INIT_SWITCH_STATUS;
                    }
                    else
                    {
                        /* Command execution failed. */
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }

                break;
            }

        case DRV_SDHC_INIT_SWITCH_STATUS:
            {
                if (dObj->cardCtxt->dataCompleted == true)
                {
                    /* Wait for the data transfer to complete. */
                    if (dObj->cardCtxt->cmd6Mode == 1)
                    {
                        /* Bits 379:376 should be set to 1 if the switch was successful. */
                        if ((dObj->cardCtxt->switchStatus[16] & 0x0F) == 0x01)
                        {
                            dObj->initState = DRV_SDHC_INIT_SET_HS_FREQ;
                        }
                        else
                        {
                            dObj->initState = DRV_SDHC_INIT_DONE;
                        }
                    }
                    else
                    {
                        /* Bit 401 should be set indicating HS support. */
                        if (dObj->cardCtxt->switchStatus[13] & 0x02)
                        {
                            /* The card supports switching to HS mode of operation. */
                            dObj->cardCtxt->cmd6Mode = 1;
                            dObj->initState = DRV_SDHC_INIT_PRE_SWITCH_CMD;
                        }
                        else
                        {
                            dObj->initState = DRV_SDHC_INIT_DONE;
                        }
                    }

                    dObj->cardCtxt->dataCompleted = false;
                }

                break;
            }

        case DRV_SDHC_INIT_SET_HS_FREQ:
            {
                DRV_SDHC_SetClock (dObj, SDHOST_CLOCK_FREQ_HS_50_MHZ);
                if (dObj->clockState == DRV_SDHC_CLOCK_SET_COMPLETE)
                {
                    sdhostSetSpeedMode (SDHOST_SPEED_MODE_HIGH);
                    dObj->initState = DRV_SDHC_INIT_SET_BLOCK_LENGTH;
                }
                break;
            }

        case DRV_SDHC_INIT_SET_BLOCK_LENGTH:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SET_BLOCKLEN, 512, SDHOST_CMD_RESP_R1, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        dObj->initState = DRV_SDHC_INIT_DESELECT_CARD;
                    }
                    else
                    {
                        dObj->initState = DRV_SDHC_INIT_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_INIT_DESELECT_CARD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SELECT_DESELECT_CARD, 0, SDHOST_CMD_RESP_NONE, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    dObj->initState = DRV_SDHC_INIT_DONE;
                }
                break;
            }

        case DRV_SDHC_INIT_DONE:
        case DRV_SDHC_INIT_ERROR:
            {
                dObj->initState = DRV_SDHC_INIT_RESET_CARD;
                break;
            }
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

void __attribute ((weak)) DRV_SDHC_RegisterWithSysFs(
    const SYS_MODULE_INDEX drvIndex
)
{

}

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_SDHC_Initialize 
     (
         const SYS_MODULE_INDEX index,
         const SYS_MODULE_INIT  * const init
     );

  Summary:
    Initializes the SD Host Controller driver.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This routine initializes the SD Host Controller driver, making it ready for clients to
    open and use the driver.

  Precondition:
    None.

  Parameters:
    drvIndex        - Index for the driver instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the driver. This pointer may be null if no
                      data is required because static overrides have been
                      provided.

  Returns:
    If successful, returns a valid handle to a driver object. Otherwise, it
    returns SYS_MODULE_OBJ_INVALID.

  Remarks:
    This routine must be called before any other SD Host Controller routine is called.

    This routine should only be called once during system initialization
    unless DRV_SDHC_Deinitialize is called to deinitialize the driver instance.

    This routine will NEVER block for hardware access. If the operation requires
    time to allow the hardware to reinitialize, it will be reported by the
    DRV_SDHC_Status operation. The system must use DRV_SDHC_Status to find out
    when the driver is in the ready state.

    Build configuration options may be used to statically override options in the
    "init" structure and will take precedence over initialization data passed
    using this routine.
*/

SYS_MODULE_OBJ DRV_SDHC_Initialize 
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT *const init
)
{
    DRV_SDHC_OBJ *dObj = NULL;
    const DRV_SDHC_INIT * const sdhcInit = (const DRV_SDHC_INIT * const)init;

    /* Validate the driver index */
    if (drvIndex >= DRV_SDHC_INDEX_COUNT)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Allocate the driver object and set the operation flag to be in use */
    dObj = &gDrvSDHCObj[drvIndex];
    if (dObj->inUse)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if (init == NULL)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if (gDrvSDHCInitCount == 0)
    {
        /* Create the Client Object mutex */
        if (OSAL_MUTEX_Create(&gDrvSDHCClientMutex) != OSAL_RESULT_TRUE)
        {
            return SYS_MODULE_OBJ_INVALID;
        }

    }
    gDrvSDHCInitCount++;

    /* Initialize the driver object's structure members */
    memset (dObj, 0, sizeof(DRV_SDHC_OBJ));

    if (OSAL_MUTEX_Create(&dObj->mutex) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Set that this instance of the driver is in use */
    dObj->inUse = true;
    dObj->taskState = DRV_SDHC_TASK_WAIT_FOR_DEVICE_ATTACH;
    dObj->cmdState = DRV_SDHC_CMD_LINE_STATE_CHECK;
    dObj->drvIndex = drvIndex;
    dObj->queue = NULL;
    dObj->speedMode = sdhcInit->speedMode;
    dObj->busWidth = sdhcInit->busWidth;
    dObj->cmdTimerState = false;

    /* Configure the SDHC RX and TX thresholds. */
    sdhostSetThreshold();
    
    if (sdhcInit->sdCardDetectEnable)
    {
        sdhostCardDetectEnable();
    }
    else
    {
        sdhostCardDetectDisable();
    }

    if (sdhcInit->sdWriteProtectEnable)
    {
        sdhostWriteProtectEnable();
    }
    else
    {
        sdhostWriteProtectDisable();
    }
            
    dObj->cardDetectEnable = sdhcInit->sdCardDetectEnable;
    dObj->writeProtectEnable = sdhcInit->sdWriteProtectEnable;

    if (sdhcInit->isFsEnabled)
    {
        DRV_SDHC_RegisterWithSysFs(drvIndex);
    }
    
    if (sdhostInit(&dObj->cardCtxt) == true)
    {        
        dObj->cardCtxt->busWidth = DRV_SDHC_BUS_WIDTH_1_BIT;
        /* Update the status */
        dObj->status = SYS_STATUS_READY;
        
        /* Return the object structure */
        return ((SYS_MODULE_OBJ)drvIndex);
    }

    return SYS_MODULE_OBJ_INVALID;
} /* DRV_SDHC_Initialize */

// *****************************************************************************
/* Function:
    void DRV_SDHC_Reinitialize 
     (
         SYS_MODULE_OBJ          object,
         const SYS_MODULE_INIT * const init
     );

  Summary:
    Reinitializes the driver and refreshes any associated hardware settings.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This routine reinitializes the driver and refreshes any associated hardware
    settings using the given initialization data, but it will not interrupt any
    ongoing operations.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - Driver object handle, returned from the DRV_SDHC_Initialize
                      routine

    init            - Pointer to the initialization data structure

  Returns:
    None

  Remarks:
    This function can be called multiple times to reinitialize the module.

    This operation can be used to refresh any supported hardware registers as
    specified by the initialization data or to change the power state of the
    module.

    This routine will NEVER block for hardware access. If the operation requires
    time to allow the hardware to reinitialize, it will be reported by the
    DRV_SDHC_Status operation. The system must use DRV_SDHC_Status to find out
    when the driver is in the ready state.

    Build configuration options may be used to statically override options in the
    "init" structure and will take precedence over initialization data passed
    using this routine.
*/

void DRV_SDHC_Reinitialize( SYS_MODULE_OBJ        object ,
                              const SYS_MODULE_INIT * const init )
{
    DRV_SDHC_OBJ           	*dObj 		= ( DRV_SDHC_OBJ* ) NULL;

    /* Validate the driver object */
    SYS_ASSERT ( object != SYS_MODULE_OBJ_INVALID, "Invalid system object handle" );

    /* get the driver object */
    dObj = &gDrvSDHCObj[object];

    if (OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE) {SYS_ASSERT(false, "SDHC Driver: OSAL_MUTEX_Lock failed");}

	/* Set the current driver state */
    ( dObj->status ) = SYS_STATUS_UNINITIALIZED;

    /* Update the status */
    ( dObj->status ) = SYS_STATUS_READY;
    if (OSAL_MUTEX_Unlock(&dObj->mutex) != OSAL_RESULT_TRUE) {SYS_ASSERT(false, "SDHC Driver: OSAL_MUTEX_Unlock failed");}

} /* DRV_SDHC_Reinitialize */


// *****************************************************************************
/* Function:
    void DRV_SDHC_Deinitialize 
     ( 
         SYS_MODULE_OBJ object 
     );

  Summary:
    Deinitializes the specified instance of the SD Host Controller driver module.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    Deinitializes the specified instance of the SD Host Controller driver module, disabling
    its operation (and any hardware). Invalidates all the internal data.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - Driver object handle, returned from the
					  DRV_SDHC_Initialize routine.

  Returns:
    None.

  Remarks:
    Once the Initialize operation has been called, the Deinitialize operation
    must be called before the Initialize operation can be called again.

    This routine will NEVER block waiting for hardware. If the operation
    requires time to allow the hardware to complete, this will be reported by
    the DRV_SDHC_Status operation.  The system has to use DRV_SDHC_Status to 
	check if the de-initialization is complete.
*/

void DRV_SDHC_Deinitialize
(
    SYS_MODULE_OBJ object
)
{
    DRV_SDHC_OBJ *dObj = NULL;

    dObj = &gDrvSDHCObj[object];

    dObj->inUse  = false;
    dObj->status = SYS_STATUS_UNINITIALIZED;

    if (OSAL_MUTEX_Delete(&dObj->mutex) != OSAL_RESULT_TRUE) {SYS_ASSERT(false, "SDHC Driver: OSAL_MUTEX_Delete failed");}
    gDrvSDHCInitCount--;
    if (gDrvSDHCInitCount == 0)
    {
        if (OSAL_MUTEX_Delete(&gDrvSDHCClientMutex) != OSAL_RESULT_TRUE) {SYS_ASSERT(false, "SDHC Driver: OSAL_MUTEX_Delete failed");}
    }

} /* DRV_SDHC_Deinitialize */


// *****************************************************************************
/* Function:
    void DRV_SDHC_InterruptServiceRoutine 
     ( 
         SYS_MODULE_OBJ object 
     );

  Summary:
    Services interrupts from the SDHC Module
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    Services interrupts from the SDHC Module

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - Driver object handle, returned from the
					  DRV_SDHC_Initialize routine.

  Returns:
    None.

  Remarks:
    None.
*/

void DRV_SDHC_InterruptServiceRoutine
(
    SYS_MODULE_OBJ object
)
{
    DRV_SDHC_OBJ *dObj = NULL;

    dObj = &gDrvSDHCObj[object];

    if (dObj->status != SYS_STATUS_READY)
        return;
    
    sdhostInterruptHandler (dObj->cardCtxt);

} /* DRV_SDHC_InterruptServiceRoutine */

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_SDHC_Status 
     ( 
         SYS_MODULE_OBJ object
     );

  Summary:
    Provides the current status of the SD Host Controller driver module.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This routine provides the current status of the SD Host Controller driver module.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    function

  Parameters:
    object                    - Driver object handle, returned from the
                                DRV_SDHC_Initialize routine

  Returns:
    SYS_STATUS_READY          - Indicates that the driver is busy with a
                                previous system level operation and cannot start
                                another

                                Note Any value greater than SYS_STATUS_READY is
                                also a normal running state in which the driver
                                is ready to accept new operations.

    SYS_STATUS_BUSY           - Indicates that the driver is busy with a
                                previous system level operation and cannot start
                                another

    SYS_STATUS_ERROR          - Indicates that the driver is in an error state

                                Note:  Any value less than SYS_STATUS_ERROR is
                                also an error state.

    SYS_MODULE_DEINITIALIZED  - Indicates that the driver has been deinitialized

                                Note:  This value is less than SYS_STATUS_ERROR

  Remarks:
    This operation can be used to determine when any of the driver's module
    level operations has completed.

    If the status operation returns SYS_STATUS_BUSY, then a previous operation
    has not yet completed. If the status operation returns SYS_STATUS_READY,
    then it indicates that all previous operations have completed.

    The value of SYS_STATUS_ERROR is negative (-1).  Any value less than that is
    also an error state.

    This routine will NEVER block waiting for hardware.

    If the Status operation returns an error value, the error may be cleared by
    calling the reinitialize operation. If that fails, the deinitialize
    operation will need to be called, followed by the initialize operation to
    return to normal operations.
*/

SYS_STATUS DRV_SDHC_Status
(
    SYS_MODULE_OBJ object
)
{
    DRV_SDHC_OBJ *dObj = NULL;

    dObj = &gDrvSDHCObj[object];

    /* Return the driver status */
    return dObj->status;
} /* DRV_SDHC_Status */


// *****************************************************************************
/* Function:
    void DRV_SDHC_Tasks 
     ( 
         SYS_MODULE_OBJ object
     );

  Summary:
    Maintains the driver's state machine.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This routine is used to maintain the driver's internal state machine.

  Precondition:
    The DRV_SDHC_Initialize routine must have been called for the specified
    SDHC driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_SDHC_Initialize)

  Returns:
    None

  Remarks:
    This routine is normally not called directly by an application. It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.

    This routine may execute in an ISR context and will never block or access any
    resources that may cause it to block.
*/

void DRV_SDHC_Tasks
( 
    SYS_MODULE_OBJ object 
)
{
    DRV_SDHC_OBJ *dObj = NULL;
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;
    DRV_SDHC_BUFFER_OBJ *currentBufObj = NULL;
    OSAL_RESULT retVal = OSAL_RESULT_FALSE;
    DRV_SDHC_EVENT evtStatus = DRV_SDHC_EVENT_COMMAND_COMPLETE;
    bool cardAttached = true;
    uint32_t response = 0;

    dObj = &gDrvSDHCObj[object];
    retVal = OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER);
    if (retVal != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "SDHC Driver: OSAL_MUTEX_Lock failed");
    }

    currentBufObj = dObj->queue;
    switch (dObj->taskState)
    {
        case DRV_SDHC_TASK_WAIT_FOR_DEVICE_ATTACH:
            {
                /* Check the Present state register to see if the card inserted
                 * bit is set. */
                if (sdhostIsCardAttached ())
                {
                    /* Kick start the card initialization. */
                    dObj->initState = DRV_SDHC_INIT_RESET_CARD;
                    dObj->taskState = DRV_SDHC_TASK_MEDIA_INIT;
                }
                else
                {
                    /* Continue to remain in the same state till a card is
                     * inserted. */
                }
                break;
            }

        case DRV_SDHC_TASK_MEDIA_INIT:
            {
                DRV_SDHC_MediaInitialize (dObj);
                if (dObj->initState == DRV_SDHC_INIT_DONE)
                {
                    /* Check and update the card's write protected status */
                    if (dObj->writeProtectEnable)
                    {
                        dObj->cardCtxt->writeProtected = sdhostIsWriteProtected();
                    }
                    else
                    {
                        dObj->cardCtxt->writeProtected = false;
                    }

                    /* Update the Media Geometry structure */
                    DRV_SDHC_UpdateGeometry (dObj);

                    dObj->mediaState = SYS_MEDIA_ATTACHED;
                    dObj->taskState = DRV_SDHC_TASK_PROCESS_QUEUE;
                }
                else if (dObj->initState == DRV_SDHC_INIT_ERROR)
                {
                    /* Cannot initialize this card. */
                    dObj->taskState = DRV_SDHC_TASK_WAIT_FOR_DEVICE_ATTACH;
                }
                break;
            }

        case DRV_SDHC_TASK_PROCESS_QUEUE:
            {
                currentBufObj = dObj->queue;
                if (currentBufObj != NULL)
                {
                    if (dObj->cardCtxt->locked == true)
                    {
                        /* Card is locked. Fail the transaction. */
                        currentBufObj->status = DRV_SDHC_COMMAND_ERROR_UNKNOWN;
                        dObj->taskState = DRV_SDHC_TASK_ERROR;
                        break;
                    }

                    if (sdhostIsCardAttached () == false)
                    {
                        /* Card has been removed. */
                        dObj->taskState = DRV_SDHC_TASK_HANDLE_CARD_DETACH;
                        break;
                    }

                    currentBufObj->status = DRV_SDHC_COMMAND_IN_PROGRESS;
                    if (dObj->cardCtxt->cardType == DRV_SDHC_MODE_NORMAL)
                    {
                        /* Card supports byte addressing. Convert the start
                         * address to byte address. */
                        currentBufObj->blockStart <<= 9;
                    }

                    if (currentBufObj->opType == DRV_SDHC_OPERATION_TYPE_READ)
                    {
                        if (currentBufObj->nBlocks == 1)
                        {
                            currentBufObj->opCode = DRV_SDHC_CMD_READ_SINGLE_BLOCK;
                        }
                        else
                        {
                            currentBufObj->opCode = DRV_SDHC_CMD_READ_MULTI_BLOCK;
                        }
                    }
                    else
                    {
                        /* Fail the transfer if the card is write protected. */
                        if (dObj->cardCtxt->writeProtected == true)
                        {
                            currentBufObj->status = DRV_SDHC_COMMAND_ERROR_UNKNOWN;
                            dObj->taskState = DRV_SDHC_TASK_ERROR;
                            break;
                        }
                        else
                        {
                            if (currentBufObj->nBlocks == 1)
                            {
                                currentBufObj->opCode = DRV_SDHC_CMD_WRITE_SINGLE_BLOCK;
                            }
                            else
                            {
                                currentBufObj->opCode = DRV_SDHC_CMD_WRITE_MULTI_BLOCK;
                            }
                        }
                    }

                    dObj->taskState = DRV_SDHC_TASK_SELECT_CARD;
                }
                else
                {
                    /* Queue is empty. Check if the card is still present. */
                    if (sdhostIsCardAttached () == false)
                    {
                        /* Card has been removed. Handle the event. */
                        dObj->taskState = DRV_SDHC_TASK_HANDLE_CARD_DETACH;
                    }
                }
                break;
            }

        case DRV_SDHC_TASK_SELECT_CARD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SELECT_DESELECT_CARD, (dObj->cardCtxt->rca << 16), SDHOST_CMD_RESP_R1B, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        sdhostReadResponse (SDHOST_READ_RESP_REG_0, &response);
                        dObj->taskState = DRV_SDHC_TASK_SETUP_XFER;
                    }
                    else
                    {
                        dObj->taskState = DRV_SDHC_TASK_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_TASK_SETUP_XFER:
            {
                if (currentBufObj == NULL)
                {
                    dObj->taskState = DRV_SDHC_TASK_ERROR;
                    break;
                }

                if (currentBufObj->nBlocks == 1)
                {
                    /* For transfers involving only a single block of data the
                     * block count field needs to be set to zero. */
                    sdhostSetBlockCount (0);
                }
                else
                {
                    /* Configure the Block Count register with the number of
                     * blocks to be transferred. */
                    sdhostSetBlockCount (currentBufObj->nBlocks);
                }
                /* Block count has already been set. */
                sdhostSetBlockSize(512);
                sdhostSetupDma (&currentBufObj->buffer[0], currentBufObj->nBlocks << 9,currentBufObj->opType );
                dObj->taskState = DRV_SDHC_TASK_XFER_COMMAND;

                /* Fall through to the next state */
            }

        case DRV_SDHC_TASK_XFER_COMMAND:
            {
                if (currentBufObj == NULL)
                {
                    dObj->taskState = DRV_SDHC_TASK_ERROR;
                    break;
                }

                DRV_SDHC_CommandSend (dObj, currentBufObj->opCode, currentBufObj->blockStart, SDHOST_CMD_RESP_R1, 1);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        dObj->taskState = DRV_SDHC_TASK_XFER_STATUS;
                    }
                    else
                    {
                        dObj->taskState = DRV_SDHC_TASK_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_TASK_XFER_STATUS:
            {
                /* Wait for the data transfer to get completed. */
                if (dObj->cardCtxt->dataCompleted)
                {
                    /* Check if there have been any errors. */
                    if (dObj->cardCtxt->errorFlag)
                    {
                        dObj->taskState = DRV_SDHC_TASK_ERROR;
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
                                dObj->taskState = DRV_SDHC_TASK_SEND_STOP_TRANS_CMD;
                            }
                            else
                            {
                                dObj->taskState = DRV_SDHC_TASK_CHECK_CARD_STATUS;
                            }
                        }
                        else
                        {
                            dObj->taskState = DRV_SDHC_TASK_ERROR;
                        }
                    }

                    dObj->cardCtxt->dataCompleted = false;
                }

                break;
            }

        case DRV_SDHC_TASK_SEND_STOP_TRANS_CMD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_STOP_TRANSMISSION, 0, SDHOST_CMD_RESP_R1B, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        dObj->taskState = DRV_SDHC_TASK_CHECK_CARD_STATUS;
                    }
                    else
                    {
                        dObj->taskState = DRV_SDHC_TASK_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_TASK_CHECK_CARD_STATUS:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SEND_STATUS, (dObj->cardCtxt->rca << 16), SDHOST_CMD_RESP_R1, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    if (dObj->commandStatus == DRV_SDHC_COMMAND_STATUS_SUCCESS)
                    {
                        sdhostReadResponse (SDHOST_READ_RESP_REG_0, &response);
                        if (response & 0x100)
                        {
                            dObj->taskState = DRV_SDHC_TASK_DESELECT_CARD;
                        }
                    }
                    else
                    {
                        dObj->taskState = DRV_SDHC_TASK_ERROR;
                    }
                }
                break;
            }

        case DRV_SDHC_TASK_DESELECT_CARD:
            {
                DRV_SDHC_CommandSend (dObj, DRV_SDHC_CMD_SELECT_DESELECT_CARD, 0, SDHOST_CMD_RESP_NONE, 0);
                if (dObj->cmdState == DRV_SDHC_CMD_EXEC_IS_COMPLETE)
                {
                    dObj->taskState = DRV_SDHC_TASK_TRANSFER_COMPLETE;
                }
                break;
            }

        case DRV_SDHC_TASK_ERROR:
            {
                /* Fall through to the next state. */
                cardAttached = sdhostIsCardAttached ();
            }

        case DRV_SDHC_TASK_TRANSFER_COMPLETE:
            {
                if (currentBufObj != NULL)
                {
                    if (dObj->taskState == DRV_SDHC_TASK_TRANSFER_COMPLETE)
                    {
                        currentBufObj->status = DRV_SDHC_COMMAND_COMPLETED;
                        evtStatus = DRV_SDHC_EVENT_COMMAND_COMPLETE;
                    }
                    else
                    {
                        currentBufObj->status = DRV_SDHC_COMMAND_ERROR_UNKNOWN;
                        evtStatus = DRV_SDHC_EVENT_COMMAND_ERROR;
                    }

                    DRV_SDHC_UpdateQueue(dObj);

                    clientObj = (DRV_SDHC_CLIENT_OBJ *)currentBufObj->hClient;
                    if(clientObj->eventHandler != NULL)
                    {
                        /* Call the event handler */
                        clientObj->eventHandler(evtStatus, currentBufObj->commandHandle, clientObj->context);
                    }
                }

                if (cardAttached)
                {
                    dObj->taskState = DRV_SDHC_TASK_PROCESS_QUEUE;
                }
                else
                {
                    dObj->taskState = DRV_SDHC_TASK_HANDLE_CARD_DETACH;
                }
                break;
            }

        case DRV_SDHC_TASK_HANDLE_CARD_DETACH:
            {
                DRV_SDHC_RemoveBufferObjects (dObj);
                sdhostInit(&dObj->cardCtxt);
                dObj->mediaState = SYS_MEDIA_DETACHED;
                dObj->taskState = DRV_SDHC_TASK_WAIT_FOR_DEVICE_ATTACH;
                break;
            }

        default:
            {
                break;
            }
    }

    retVal = OSAL_MUTEX_Unlock(&dObj->mutex);
    if (retVal != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "SDHC Driver: OSAL_MUTEX_Unlock failed");
    }

} /* DRV_SDHC_Tasks */


// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_SDHC_Open 
     (
         const SYS_MODULE_INDEX drvIndex,
         const DRV_IO_INTENT    intent
     );

  Summary:
    Opens the specified SD Host Controller driver instance and returns a handle to it.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This routine opens the specified SD Card driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    function.

  Parameters:
    drvIndex    - Identifier for the object instance to be opened

    intent      - Zero or more of the values from the enumeration
                  DRV_IO_INTENT "ORed" together to indicate the intended use
                  of the driver

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID.

  Remarks:
    The handle returned is valid until the DRV_SDHC_Close routine is called.

    This routine will NEVER block waiting for hardware.

    If the DRV_IO_INTENT_BLOCKING is requested and the driver was built
    appropriately to support blocking behavior, then other client-level
    operations may block waiting on hardware until they are complete.

    If the requested intent flags are not supported, the routine will return
    DRV_HANDLE_INVALID.
*/

DRV_HANDLE DRV_SDHC_Open 
(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;
    DRV_SDHC_OBJ *dObj = NULL;
    OSAL_RESULT retVal = OSAL_RESULT_FALSE;
   
    /* Validate the driver index */
    if (drvIndex >= DRV_SDHC_INDEX_COUNT)
    {
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvSDHCObj[drvIndex];

    /* Acquire the driver object mutex */
    retVal = OSAL_MUTEX_Lock(&gDrvSDHCClientMutex, OSAL_WAIT_FOREVER);
    if (retVal != OSAL_RESULT_TRUE)
    {
        return DRV_HANDLE_INVALID;
    }

    if ((dObj->status != SYS_STATUS_READY) || (dObj->inUse == false)) 
    {
        OSAL_MUTEX_Unlock(&gDrvSDHCClientMutex);
        return DRV_HANDLE_INVALID;
    }

    /* The SDHC driver doesn't support blocking intent. Flag error. */
    /* Flag error if max number of clients are already open.
     * Flag error if driver was already opened exclusively.
     * Flag error if the client is to trying to open the driver exclusively
     * when it is already by other clients in non-exclusive mode.
     * */
    if ((DRV_IO_ISBLOCKING(ioIntent)) ||
        (dObj->numClients == DRV_SDHC_CLIENTS_NUMBER) ||
        (dObj->isExclusive) || 
        ((dObj->numClients > 0) && DRV_IO_ISEXCLUSIVE(ioIntent)))
    {
        OSAL_MUTEX_Unlock(&gDrvSDHCClientMutex);
        return DRV_HANDLE_INVALID;
    }

    clientObj = DRV_SDHC_AllocateClientObject ();
    if (clientObj != NULL)
    {
        /* Found a client object that can be used */
        clientObj->inUse = true;
        clientObj->driverObj = dObj;
        clientObj->intent = ioIntent;
        clientObj->eventHandler = NULL;

        clientObj->context = (uintptr_t) NULL;

        if (ioIntent & DRV_IO_INTENT_EXCLUSIVE)
        {
            /* Driver was opened in exclusive mode */
            dObj->isExclusive = true;
        }

        dObj->numClients ++;
    }

    OSAL_MUTEX_Unlock(&gDrvSDHCClientMutex);

    return clientObj ? ((DRV_HANDLE)clientObj) : DRV_HANDLE_INVALID;

} /* DRV_SDHC_Open */


// *****************************************************************************
/* Function:
    void DRV_SDHC_Close
     (
         DRV_HANDLE handle
     );

  Summary:
    Closes an opened-instance of the SD Card driver.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This routine closes an opened-instance of the SD Card driver, invalidating
    the handle.

  Precondition:
    The DRV_SDHC_Initialize routine must have been called for the specified
    SD Card driver instance.

    DRV_SDHC_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining driver routines.  A new handle must be obtained by
    calling DRV_SDHC_Open before the caller may use the driver again.

    If DRV_IO_INTENT_BLOCKING was requested and the driver was built
    appropriately to support blocking behavior call may block until the
    operation is complete.

    If DRV_IO_INTENT_NON_BLOCKING request the driver client can call the
    DRV_SDHC_Status operation to find out when the module is in
    the ready state (the handle is no longer valid).

    Note:
    Usually there is no need for the driver client to verify that the Close
    operation has completed.
*/

void DRV_SDHC_Close
(
    DRV_HANDLE handle
)
{
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;
    DRV_SDHC_OBJ* dObj = NULL;
    clientObj = DRV_SDHC_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {

        return;
    }

    dObj = clientObj->driverObj;

    if (OSAL_MUTEX_Lock(&gDrvSDHCClientMutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "SDHC Driver: OSAL_MUTEX_Lock failed");
    }

    DRV_SDHC_RemoveClientBufferObjects (clientObj, dObj);
    /* Update the client count */
    dObj->numClients --;
    dObj->isExclusive = false;

    /* Free the Client Instance */
    clientObj->inUse = false;

    /* Update the Client Status */
    clientObj->inUse = false;

    dObj->numClients--;

    OSAL_MUTEX_Unlock(&gDrvSDHCClientMutex);

} /* DRV_SDHC_Close */


// *****************************************************************************
/* Function:
    void DRV_SDHC_Read
    (
        const DRV_HANDLE handle,
        DRV_SDHC_COMMAND_HANDLE * commandHandle,
        void * targetBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Reads blocks of data from the specified block address of the SD Card.

  Description:
    This function schedules a non-blocking read operation for reading blocks
    of data from the SD Card. The function returns with a valid buffer handle
    in the commandHandle argument if the read request was scheduled successfully.
    The function adds the request to the hardware instance queue and returns
    immediately. While the request is in the queue, the application buffer is
    owned by the driver and should not be modified. The function returns 
    DRV_SDHC_COMMAND_HANDLE_INVALID in the commandHandle argument under the 
    following circumstances:
    - if the driver handle is invalid
    - if the target buffer pointer is NULL
    - if the number of blocks to be read is zero or more than the actual number
      of blocks available
    - if a buffer object could not be allocated to the request
    - if the client opened the driver in write only mode

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_SDHC_EVENT_COMMAND_COMPLETE event if the
    buffer was processed successfully or DRV_SDHC_EVENT_COMMAND_ERROR
    event if the buffer was not processed successfully.

  Precondition:
    The DRV_SDHC_Initialize routine must have been called for the specified SDHC 
    driver instance.

    DRV_SDHC_Open must have been called with DRV_IO_INTENT_READ or 
    DRV_IO_INTENT_READWRITE as the ioIntent to obtain a valid opened device handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the return buffer
                    handle
                   
    targetBuffer  - Buffer into which the data read from the SD Card will be placed

    blockStart    - Start block address of the SD Card from where the read should begin.

    nBlock        - Total number of blocks to be read.

  Returns:
    The buffer handle is returned in the commandHandle argument. It will be
    DRV_SDHC_COMMAND_HANDLE_INVALID if the request was not successful.

  Remarks:
    None.
*/

void DRV_SDHC_Read 
(
    const DRV_HANDLE handle,
    DRV_SDHC_COMMAND_HANDLE *commandHandle,
    void *targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    DRV_SDHC_COMMAND_HANDLE *tempHandle1 = NULL, tempHandle2;
    DRV_SDHC_CLIENT_OBJ     *clientObj = NULL;
    DRV_SDHC_BUFFER_OBJ     *bufferObj = NULL;
    DRV_SDHC_OBJ            *dObj = NULL;

    tempHandle1 = (commandHandle == NULL) ? &tempHandle2 : commandHandle;
    *tempHandle1 = DRV_SDHC_COMMAND_HANDLE_INVALID;

    clientObj = DRV_SDHC_ValidateDriverHandle (handle);
    if (clientObj == NULL)
        return;

    if (!(clientObj->intent & DRV_IO_INTENT_READ))
        return;

    if ((targetBuffer == NULL) || (nBlock == 0))
        return;
    
    if (((blockStart + nBlock) > gDrvSDHCGeometryTable[GEOMETRY_TABLE_READ_ENTRY].numBlocks))
        return;

    dObj = (DRV_SDHC_OBJ*)clientObj->driverObj;

    if (OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE) 
        return;

    bufferObj = DRV_SDHC_AllocateBufferObject (clientObj, targetBuffer, blockStart, nBlock, DRV_SDHC_OPERATION_TYPE_READ);
    if (bufferObj != NULL)
    {
        *tempHandle1 = bufferObj->commandHandle;

        /* Add the request to the queue. */
        DRV_SDHC_AddToQueue (dObj, bufferObj);
    }

    OSAL_MUTEX_Unlock(&dObj->mutex);
}


// *****************************************************************************
/* Function:
    void DRV_SDHC_Write
    (
        const DRV_HANDLE handle,
        DRV_SDHC_COMMAND_HANDLE * commandHandle,
        void * sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Writes blocks of data starting at the specified address of the SD Card.

  Description:
    This function schedules a non-blocking write operation for writing blocks
    of data to the SD Card. The function returns with a valid buffer handle
    in the commandHandle argument if the write request was scheduled successfully.
    The function adds the request to the hardware instance queue and returns
    immediately. While the request is in the queue, the application buffer is
    owned by the driver and should not be modified. The function returns 
    DRV_SDHC_COMMAND_HANDLE_INVALID in the commandHandle argument under the 
    following circumstances:
    - if a buffer object could not be allocated to the request
    - if the source buffer pointer is NULL
    - if the client opened the driver for read only
    - if the number of blocks to be written is either zero or more than the number
      of blocks actually available
    - if the write queue size is full or queue depth is insufficient
    - if the driver handle is invalid 

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_SDHC_EVENT_COMMAND_COMPLETE event if the
    buffer was processed successfully or DRV_SDHC_EVENT_COMMAND_ERROR
    event if the buffer was not processed successfully.

  Precondition:
    The DRV_SDHC_Initialize() routine must have been called for the specified
    SDHC driver instance.

    DRV_SDHC_Open() routine must have been called to obtain a valid opened device
    handle. DRV_IO_INTENT_WRITE or DRV_IO_INTENT_READWRITE must have been specified
    as a parameter to this routine.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the return buffer
                    handle
                   
    sourceBuffer  - The source buffer containing data to be programmed to the SD Card.

    blockStart    - Start block address of SD Card where the writes should begin. 

    nBlock        - Total number of blocks to be written. 

  Returns:
    The buffer handle is returned in the commandHandle argument. It will be
    DRV_SDHC_COMMAND_HANDLE_INVALID if the request was not successful.

  Remarks:
    None.
*/

void DRV_SDHC_Write
(
    const DRV_HANDLE handle,
    DRV_SDHC_COMMAND_HANDLE *commandHandle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    DRV_SDHC_COMMAND_HANDLE *tempHandle1 = NULL, tempHandle2;
    DRV_SDHC_CLIENT_OBJ     *clientObj = NULL;
    DRV_SDHC_BUFFER_OBJ     *bufferObj = NULL;
    DRV_SDHC_OBJ            *dObj = NULL;

    tempHandle1 = (commandHandle == NULL) ? &tempHandle2 : commandHandle;
    *tempHandle1 = DRV_SDHC_COMMAND_HANDLE_INVALID;

    clientObj = DRV_SDHC_ValidateDriverHandle (handle);
    if (clientObj == NULL)
        return;

    if (!(clientObj->intent & DRV_IO_INTENT_WRITE))
        return;

    if ((sourceBuffer == NULL) || (nBlock == 0))
        return;

    if (((blockStart + nBlock) > gDrvSDHCGeometryTable[GEOMETRY_TABLE_WRITE_ENTRY].numBlocks))
        return;

    dObj = (DRV_SDHC_OBJ*)clientObj->driverObj;

    /* Return error if the card is write protected */
    if (dObj->cardCtxt->writeProtected)
        return;

    if (OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
        return;

    bufferObj = DRV_SDHC_AllocateBufferObject (clientObj, sourceBuffer, blockStart, nBlock, DRV_SDHC_OPERATION_TYPE_WRITE);
    if (bufferObj != NULL)
    {
        *tempHandle1 = bufferObj->commandHandle;

        /* Add the request to the queue. */
        DRV_SDHC_AddToQueue (dObj, bufferObj);
    }

    OSAL_MUTEX_Unlock(&dObj->mutex);
}


// *****************************************************************************
/* Function:
    DRV_SDHC_COMMAND_STATUS DRV_SDHC_CommandStatus
    (
        const DRV_HANDLE handle, 
        const DRV_SDHC_COMMAND_HANDLE commandHandle
    );

  Summary:
    Gets the current status of the command.

  Description:
    This routine gets the current status of the command. The application must use
    this routine where the status of a scheduled command needs to be polled on. The
    function may return DRV_SDHC_COMMAND_HANDLE_INVALID in a case where the command
    handle has expired. A command handle expires when the internal buffer object
    is re-assigned to another read or write request. It is recommended that this
    function be called regularly in order to track the command status correctly.

    The application can alternatively register an event handler to receive read or
    write operation completion events.

  Preconditions:
    The DRV_SDHC_Initialize() routine must have been called.

    The DRV_SDHC_Open() must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    A DRV_SDHC_COMMAND_STATUS value describing the current status of the command.
    Returns DRV_SDHC_COMMAND_HANDLE_INVALID if the client handle or the command
    handle is not valid.

  Remarks:
    This routine will not block for hardware access and will immediately return
    the current status.
*/

DRV_SDHC_COMMAND_STATUS DRV_SDHC_CommandStatus
(
    const DRV_HANDLE handle, 
    const DRV_SDHC_COMMAND_HANDLE commandHandle
)
{
    uint16_t iEntry = 0;
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;

    clientObj = DRV_SDHC_ValidateDriverHandle (handle);
    if (clientObj == NULL)
    {
        return DRV_SDHC_COMMAND_ERROR_UNKNOWN;
    }

    if (commandHandle == DRV_SDHC_COMMAND_HANDLE_INVALID)
    {
        return DRV_SDHC_COMMAND_ERROR_UNKNOWN;
    }

    /* The upper 16 bits of the buffer handle are the token and the lower 16
     * bits of the are buffer index into the gDrvSDHCBufferObject array */
    iEntry = commandHandle & 0xFFFF;

    /* Compare the buffer handle with buffer handle in the object */
    if(gDrvSDHCBufferObj[iEntry].commandHandle != commandHandle)
    {
        /* This means that object has been re-used by another request. Indicate
         * that the operation is completed.  */
        return (DRV_SDHC_COMMAND_COMPLETED);
    }

    /* Return the last known buffer object status */
    return (gDrvSDHCBufferObj[iEntry].status);
}

// *****************************************************************************
/* Function:
    SYS_MEDIA_GEOMETRY * DRV_SDHC_GeometryGet
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the geometry of the device.

  Description:
    This API gives the following geometrical details of the SD Card.
    - Media Property
    - Number of Read/Write/Erase regions in the SD Card
    - Number of Blocks and their size in each region of the device

  Precondition:
    The DRV_SDHC_Initialize() routine must have been called for the
    specified SDHC driver instance.

    The DRV_SDHC_Open() routine must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    SYS_MEDIA_GEOMETRY - Pointer to structure which holds the media geometry information.

  Remarks:
    None.
*/

SYS_MEDIA_GEOMETRY * DRV_SDHC_GeometryGet
(
    const DRV_HANDLE handle
)
{
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;

    clientObj = DRV_SDHC_ValidateDriverHandle (handle);
    if (clientObj != NULL)
    {
        return (&gDrvSDHCMediaGeometry);
    }

    return NULL;
}

// *****************************************************************************
/* Function:
    void DRV_SDHC_EventHandlerSet
    (
        const DRV_HANDLE handle,
        const void * eventHandler,
        const uintptr_t context
    );

  Summary:
    Allows a client to identify an event handling function for the driver to
    call back when queued operation has completed.

  Description:
    This function allows a client to identify an event handling function for
    the driver to call back when queued operation has completed.  
    When a client queues a request for a read or a write operation, it is provided 
    with a handle identifying the buffer that was added to the driver's buffer queue.
    The driver will pass this handle back to the client by calling "eventHandler"
    function when the queued operation has completed.
    
    The event handler should be set before the client performs any read or write
    operations that could generate events. The event handler once set, persists
    until the client closes the driver or sets another event handler (which could
    be a "NULL" pointer to indicate no callback).

  Precondition:
    The DRV_SDHC_Initialize() routine must have been called for the specified
    SDHC driver instance.

    The DRV_SDHC_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

    eventHandler - Pointer to the event handler function implemented by the user
    
    context      - The value of parameter will be passed back to the client 
                   unchanged, when the eventHandler function is called. It can
                   be used to identify any client specific data object that 
                   identifies the instance of the client module (for example, 
                   it may be a pointer to the client module's state structure).

  Returns:
    None.

  Remarks:
    If the client does not want to be notified when the queued operation
    has completed, it does not need to register a callback.
*/

void DRV_SDHC_EventHandlerSet
(
    const DRV_HANDLE handle,
    const void * eventHandler,
    const uintptr_t context
)
{
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;
    DRV_SDHC_OBJ            *dObj = NULL;

    clientObj = DRV_SDHC_ValidateDriverHandle (handle);

    if (clientObj != NULL)
    {
        dObj = (DRV_SDHC_OBJ*)clientObj->driverObj;

        if (OSAL_MUTEX_Lock(&dObj->mutex, OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* Set the event handler */
            clientObj->eventHandler = eventHandler;
            clientObj->context = context;
            OSAL_MUTEX_Unlock(&dObj->mutex);
        }

    }
}


// *****************************************************************************
/* Function:
    bool DRV_SDHC_IsAttached
    ( 
        const DRV_HANDLE handle 
    );

  Summary:
    Returns the physical attach status of the SD Card.

  Description:
    This function returns the physical attach status of the SD Card.

  Precondition:
    The DRV_SDHC_Initialize() routine must have been called for the specified 
    SDHC driver instance.

    The DRV_SDHC_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Returns false if the handle is invalid otherwise returns the attach status
    of the SD Card. Returns true if the SD Card is attached and initialized by the
    SDHC driver otherwise returns false.

  Remarks:
    None.
*/

bool DRV_SDHC_IsAttached
(
    const DRV_HANDLE handle
)
{
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;
    DRV_SDHC_OBJ *dObj = NULL;
    bool isAttached = false;

    clientObj = DRV_SDHC_ValidateDriverHandle (handle);
    if (clientObj == NULL)
        return false;

    dObj = (DRV_SDHC_OBJ*)clientObj->driverObj;

    if (dObj->mediaState == SYS_MEDIA_ATTACHED)
        isAttached = true;

    return isAttached;
}
    
// *****************************************************************************
/* Function:
    bool DRV_SDHC_IsWriteProtected
    ( 
        const DRV_HANDLE handle 
    );

  Summary:
    Returns the write protect status of the SDHC.

  Description:
    This function returns the physical attach status of the SDHC. This function 
    returns true if the SD Card is write protected otherwise it returns false.

  Precondition:
    The DRV_SDHC_Initialize() routine must have been called for the specified 
    SDHC driver instance.

    The DRV_SDHC_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Returns true if the attached SD Card is write protected.
    Returns false if the handle is not valid, or if the SD Card is not write protected.

  Remarks:
    None.
*/

bool DRV_SDHC_IsWriteProtected
(
    const DRV_HANDLE handle
)
{
    DRV_SDHC_CLIENT_OBJ *clientObj = NULL;
    DRV_SDHC_OBJ *dObj = NULL;

    clientObj = DRV_SDHC_ValidateDriverHandle (handle);
    if (clientObj == NULL)
        return false;

    dObj = (DRV_SDHC_OBJ*)clientObj->driverObj;
    if (dObj->mediaState == SYS_MEDIA_DETACHED)
        return false;

    return dObj->cardCtxt->writeProtected;
}

/* Function:
	SD Host controller Interrupt service routine
*/
void SDHC_InterruptHandler(void)
{
	SYS_INT_SourceStatusClear(HSMCI_IRQn);
    DRV_SDHC_InterruptServiceRoutine((SYS_MODULE_OBJ)DRV_SDHC_INDEX_0);
}
/*******************************************************************************
End of File
*/
