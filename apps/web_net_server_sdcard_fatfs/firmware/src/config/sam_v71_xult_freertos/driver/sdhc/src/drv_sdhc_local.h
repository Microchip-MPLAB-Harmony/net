/*******************************************************************************
  SD Host Controller Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdhc_local.h

  Summary:
    SD Host Controller driver local declarations, structures and function prototypes.

  Description:
    This file contains the SD Host Controller driver's local declarations and definitions.
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

#ifndef _DRV_SDHC_LOCAL_H
#define _DRV_SDHC_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "driver/sdhc/drv_sdhc.h"
#include "driver/sdhc/src/drv_sdhc_host.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros
// *****************************************************************************
// *****************************************************************************
/* Helper macros for the driver */

// *****************************************************************************
/* SDHC Driver Buffer Handle Macros

  Summary:
    SDHC driver Buffer Handle Macros

  Description:
    Buffer handle related utility macros. SDHC driver buffer handle is a
    combination of buffer token and the buffer object index. The buffertoken
    is a 16 bit number that is incremented for every new write or erase request
    and is used along with the buffer object index to generate a new buffer
    handle for every request.

  Remarks:
    None
*/

#define DRV_SDHC_BUF_TOKEN_MAX         (0xFFFF)
#define DRV_SDHC_MAKE_HANDLE(token, index) ((token) << 16 | (index))
#define DRV_SDHC_UPDATE_BUF_TOKEN(token) \
{ \
    (token)++; \
    (token) = ((token) == DRV_SDHC_BUF_TOKEN_MAX) ? 0: (token); \
}

#define DRV_SDHC_COMMAND_STATUS_SUCCESS         0x00
#define DRV_SDHC_COMMAND_STATUS_ERROR           0x01
#define DRV_SDHC_COMMAND_STATUS_TIMEOUT_ERROR   0x02
#define DRV_SDHC_COMMAND_STATUS_CRC_ERROR       0x03

#define DRV_SDHC_COMMAND_TIMEOUT_ERROR          0x01
#define DRV_SDHC_COMMAND_CRC_ERROR              0x02
#define DRV_SDHC_COMMAND_END_BIT_ERROR          0x04
#define DRV_SDHC_COMMAND_INDEX_ERROR            0x08
#define DRV_SDHC_XFER_ERROR                     0x10

// *****************************************************************************
// *****************************************************************************
// Section: SD Host Controller constants
// *****************************************************************************
// *****************************************************************************
/* Constants used by SD card driver */

/* SD Host Controller connection states. */
typedef enum
{
    /* SD Host Controller is attached from the system */
    DRV_SDHC_IS_DETACHED,

    /* SD Host Controller is attached to the system */
    DRV_SDHC_IS_ATTACHED

}DRV_SDHC_ATTACH;

/* SD Host Controller type. */
typedef enum
{
    /* Normal SD Host Controller */
    DRV_SDHC_MODE_NORMAL,

    /* SDHC type Card */
    DRV_SDHC_MODE_HC
}DRV_SDHC_TYPE;

/* SD Host Controller initialization stages. */
typedef enum
{
    DRV_SDHC_INIT_RESET_CARD = 0,
    DRV_SDHC_INIT_RESET_DELAY,
    DRV_SDHC_INIT_CHK_IFACE_CONDITION,
    DRV_SDHC_INIT_SEND_APP_CMD,
    DRV_SDHC_INIT_SEND_ACMD41,
    DRV_SDHC_INIT_ALL_SEND_CID,
    DRV_SDHC_INIT_PUBLISH_RCA,
    DRV_SDHC_INIT_READ_CSD,
    DRV_SDHC_INIT_CHANGE_CLK_FREQ,
    DRV_SDHC_INIT_SELECT_CARD,
    DRV_SDHC_INIT_CHK_CARD_STATE,
    DRV_SDHC_INIT_PRE_SCR_STATE,
    DRV_SDHC_INIT_SEND_SCR,
    DRV_SDHC_INIT_PARSE_SCR_DATA,
    DRV_SDHC_INIT_SET_BUS_WIDTH,
    DRV_SDHC_INIT_CARD_VER_CHECK,
    DRV_SDHC_INIT_PRE_SWITCH_CMD,
    DRV_SDHC_INIT_SWITCH_CMD,
    DRV_SDHC_INIT_SWITCH_STATUS,
    DRV_SDHC_INIT_SET_HS_FREQ,
    DRV_SDHC_INIT_SET_BLOCK_LENGTH,
    DRV_SDHC_INIT_DESELECT_CARD,
    DRV_SDHC_INIT_DONE,
    DRV_SDHC_INIT_ERROR
} DRV_SDHC_INIT_STATES;

typedef enum
{
    DRV_SDHC_TASK_WAIT_FOR_DEVICE_ATTACH = 0,
    DRV_SDHC_TASK_MEDIA_INIT,
    DRV_SDHC_TASK_PROCESS_QUEUE,
    DRV_SDHC_TASK_SELECT_CARD,
    DRV_SDHC_TASK_SETUP_XFER,
    DRV_SDHC_TASK_XFER_COMMAND,
    DRV_SDHC_TASK_XFER_STATUS,
    DRV_SDHC_TASK_SEND_STOP_TRANS_CMD,
    DRV_SDHC_TASK_CHECK_CARD_STATUS,
    DRV_SDHC_TASK_DESELECT_CARD,
    DRV_SDHC_TASK_ERROR,
    DRV_SDHC_TASK_TRANSFER_COMPLETE,
    DRV_SDHC_TASK_HANDLE_CARD_DETACH

} DRV_SDHC_TASK_STATES;

typedef enum
{
    DRV_SDHC_CMD_LINE_STATE_CHECK = 0,
    DRV_SDHC_CMD_FRAME_AND_SEND_CMD,
    DRV_SDHC_CMD_CHECK_TRANSFER_COMPLETE,
    DRV_SDHC_CMD_EXEC_IS_COMPLETE
} DRV_SDHC_COMMAND_STATES;

typedef enum
{
    /* Set the clock divider value. */
    DRV_SDHC_CLOCK_SET_DIVIDER = 0,

    /* Wait for the internal clock stable bit to be set. */
    DRV_SDHC_CLOCK_PRE_ENABLE_DELAY,

    /* Enable the clock. */
    DRV_SDHC_CLOCK_ENABLE,

    /* Delay post clock enable. */
    DRV_SDHC_CLOCK_POST_ENABLE_DELAY,

    /* Clock set is complete. */
    DRV_SDHC_CLOCK_SET_COMPLETE

} DRV_SDHC_CLOCK_STATES;

/* SD Host Controller Driver Client Object. */
typedef struct _DRV_SDHC_CLIENT_OBJ_STRUCT
{
    /* Hardware instance index associated with the client */
    void *driverObj;

    /* Flag to indicate in use  */
    bool inUse;

    /* The intent with which the client was opened */
    DRV_IO_INTENT intent;

    /* Client specific event handler */
    DRV_SDHC_EVENT_HANDLER eventHandler;

    /* Client specific context */
    uintptr_t context;

} DRV_SDHC_CLIENT_OBJ;

typedef struct DRV_SDHC_BUFFER_OBJ
{
    /* True if object is allocated */
    bool inUse;

    /* Client that owns this buffer */
    DRV_SDHC_CLIENT_OBJ *hClient;

    /* Present status of this command */
    DRV_SDHC_COMMAND_STATUS status;

    /* Current command handle of this buffer object */
    DRV_SDHC_COMMAND_HANDLE commandHandle;

    /* Pointer to the source/destination buffer */
    uint8_t *buffer;

    /* Start address of the operation. */
    uint32_t blockStart;

    /* Number of blocks */
    uint32_t nBlocks;

    /* Op code associated with the buffer object. */
    uint8_t opCode;

    /* Operation type - read/write */
    DRV_SDHC_OPERATION_TYPE opType;

    /* Pointer to the next buffer in the queue */
    struct DRV_SDHC_BUFFER_OBJ* next;

    /* Pointer to the previous buffer in the queue */
    struct DRV_SDHC_BUFFER_OBJ* previous;

} DRV_SDHC_BUFFER_OBJ;

/* SD Host Controller Driver Hardware Instance Object. */
typedef struct _DRV_SDHC_OBJ_STRUCT
{
    /* Index of the driver */
    SYS_MODULE_INDEX drvIndex;

    /* The status of the driver */
    SYS_STATUS status;

    /* Flag to indicate in use  */
    bool inUse;

    /* Flag to indicate if the driver is used in exclusive mode of operation */
    bool isExclusive;

    /* Whether card detect should be used for detecting the presence/absence of
     * the card. */
    bool cardDetectEnable;

    /* Whether the write protect pin should be used to check if the card is
     * write protected. */
    bool writeProtectEnable;

    /* Number of clients possible with the hardware instance */
    uint8_t numClients;

    /* Sent command status. */
    uint8_t commandStatus;

    /* Variable used to track the number of trials of a particular operation.
     * */
    uint16_t trials;

    /* Status of the device */
    SYS_MEDIA_STATUS mediaState;

    /* System Timer Handle */
    SYS_TIME_HANDLE tmrHandle;

    /* Pointer to the card context. */
    SDHOST_CARD_CTXT *cardCtxt;

    /* Speed mode - Default Speed or High Speed mode of operation. */
    DRV_SDHC_SPEED_MODE speedMode;

    /* Bus width to be used for the card. */
    DRV_SDHC_BUS_WIDTH busWidth;

    /* This variable holds the current state of the DRV_SDHC_Tasks */
    DRV_SDHC_TASK_STATES taskState;

    /* Different stages in media initialization */
    DRV_SDHC_INIT_STATES initState;
    DRV_SDHC_INIT_STATES nextInitState;

    /* Different states in sending a command */
    DRV_SDHC_COMMAND_STATES cmdState;

    /* Different states in setting up the clock */
    DRV_SDHC_CLOCK_STATES clockState;

    /* Queue of buffer objects. */
    DRV_SDHC_BUFFER_OBJ *queue;

    /* Timer command handle. */
    SYS_TIME_HANDLE cmdTimerHandle;

    /* Timer command state. */
    bool cmdTimerState;

    /* Mutex */
    OSAL_MUTEX_DECLARE(mutex);

} DRV_SDHC_OBJ;

#endif //#ifndef _DRV_SDHC_LOCAL_H

/*******************************************************************************
 End of File
*/
