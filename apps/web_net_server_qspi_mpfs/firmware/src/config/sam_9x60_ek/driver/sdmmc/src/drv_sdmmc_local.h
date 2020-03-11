/*******************************************************************************
  SD Host Controller Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdmmc_local.h

  Summary:
    SD Host Controller driver local declarations, structures and function
    prototypes.

  Description:
    This file contains the SD Host Controller driver's local declarations
    and definitions.
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


#ifndef _DRV_SDMMC_LOCAL_H
#define _DRV_SDMMC_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "driver/sdmmc/drv_sdmmc.h"
#include "system/time/sys_time.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros
// *****************************************************************************
// *****************************************************************************
/* SDMMC Driver Handle Macros*/
#define DRV_SDMMC_INDEX_MASK                   (0x000000FF)
#define DRV_SDMMC_INSTANCE_MASK                (0x0000FF00)
#define DRV_SDMMC_TOKEN_MAX                    (0xFFFF)

#define DRV_SDMMC_COMMAND_STATUS_SUCCESS         0x00
#define DRV_SDMMC_COMMAND_STATUS_ERROR           0x01
#define DRV_SDMMC_COMMAND_STATUS_TIMEOUT_ERROR   0x02
#define DRV_SDMMC_COMMAND_STATUS_CRC_ERROR       0x03

#define DRV_SDMMC_COMMAND_TIMEOUT_ERROR          0x0001
#define DRV_SDMMC_COMMAND_CRC_ERROR              0x0002
#define DRV_SDMMC_COMMAND_END_BIT_ERROR          0x0004
#define DRV_SDMMC_COMMAND_INDEX_ERROR            0x0008
// Using the reserved bit to indicate card removal event
#define DRV_SDMMC_CARD_REMOVAL_ERROR             0x8000
#define DRV_SDMMC_ANY_COMMAND_ERRORS             0x800F
#define DRV_SDMMC_DATA_TIMEOUT_ERROR             0x0010
#define DRV_SDMMC_DATA_CRC_ERROR                 0x0020
#define DRV_SDMMC_DATA_END_BIT_ERROR             0x0040
#define DRV_SDMMC_ADMA_ERROR                     0x0200
#define DRV_SDMMC_ANY_DATA_ERRORS                0x8270

#define DRV_SDMMC_CSD_BUFFER_LEN                 (16)
#define DRV_SDMMC_CID_BUFFER_LEN                 (16)
#define DRV_SDMMC_SCR_BUFFER_LEN                 (32)
#define DRV_SDMMC_SWITCH_STATUS_BUFFER_LEN       (64)


#define DRV_SDMMC_CLOCK_FREQ_400_KHZ           (400000)
#define DRV_SDMMC_CLOCK_FREQ_DS_25_MHZ         (25000000)
#define DRV_SDMMC_CLOCK_FREQ_HS_50_MHZ         (50000000)

// *****************************************************************************
/* SDMMC Read/Write/Erase Region Index Numbers

  Summary:
    SDMMC Geometry Table Index definitions.

  Description:
    These constants provide SDMMC Geometry Table index definitions.

  Remarks:
    None
*/
#define GEOMETRY_TABLE_READ_ENTRY   (0)
#define GEOMETRY_TABLE_WRITE_ENTRY  (1)
#define GEOMETRY_TABLE_ERASE_ENTRY  (2)

// *****************************************************************************
// *****************************************************************************
// Section: SD Host Controller constants
// *****************************************************************************
// *****************************************************************************

typedef enum
{
    /* Normal SD Host Controller */
    DRV_SDMMC_CARD_TYPE_STANDARD,

    /* SDMMC type Card */
    DRV_SDMMC_CARD_TYPE_HC

}DRV_SDMMC_CARD_TYPE;

typedef enum
{
    DRV_SDMMC_CMD_RESP_NONE,   /*!< no response type */
    DRV_SDMMC_CMD_RESP_R1,     /*!< normal response command */
    DRV_SDMMC_CMD_RESP_R1B,    /*!< normal with busy signal */
    DRV_SDMMC_CMD_RESP_R2,     /*!< CID, CSD register */
    DRV_SDMMC_CMD_RESP_R3,     /*!< OCR register */
    DRV_SDMMC_CMD_RESP_R4,     /*!< */
    DRV_SDMMC_CMD_RESP_R5,     /*!< */
    DRV_SDMMC_CMD_RESP_R6,     /*!< Published RCA response  */
    DRV_SDMMC_CMD_RESP_R7      /*!< Card interface condition  */

} DRV_SDMMC_CMD_RESP_TYPE;

typedef enum
{
    DRV_SDMMC_INIT_SET_INIT_SPPED = 0,
    DRV_SDMMC_INIT_START_POLLING_TIMEOUT,
    DRV_SDMMC_INIT_WAIT_POLLING_TIMEOUT,
    DRV_SDMMC_INIT_RESET_CARD,
    DRV_SDMMC_INIT_RESET_DELAY,
    DRV_SDMMC_INIT_CHK_IFACE_CONDITION,
    DRV_SDMMC_INIT_SEND_APP_CMD,
    DRV_SDMMC_INIT_SEND_ACMD41,
    DRV_SDMMC_INIT_ALL_SEND_CID,
    DRV_SDMMC_INIT_PUBLISH_RCA,
    DRV_SDMMC_INIT_READ_CSD,
    DRV_SDMMC_INIT_READ_EXT_CSD_SETUP,
    DRV_SDMMC_INIT_READ_EXT_CSD,
    DRV_SDMMC_INIT_WAIT_EXT_CSD,
    DRV_SDMMC_INIT_CHANGE_CLK_FREQ,
    DRV_SDMMC_INIT_SELECT_CARD,
    DRV_SDMMC_INIT_CHK_CARD_STATE,
    DRV_SDMMC_INIT_PRE_SCR_STATE,
    DRV_SDMMC_INIT_SEND_SCR,
    DRV_SDMMC_INIT_PARSE_SCR_DATA,
    DRV_SDMMC_INIT_SET_CARD_BUS_WIDTH,
    DRV_SDMMC_INIT_SET_BUS_WIDTH,
    DRV_SDMMC_INIT_CARD_VER_CHECK,
    DRV_SDMMC_INIT_PRE_SWITCH_CMD,
    DRV_SDMMC_INIT_SWITCH_CMD,
    DRV_SDMMC_INIT_SWITCH_STATUS,
    DRV_SDMMC_INIT_SET_CARD_HS_FREQ,
    DRV_SDMMC_INIT_SET_HS_FREQ,
    DRV_SDMMC_INIT_SET_BLOCK_LENGTH,
    DRV_SDMMC_INIT_DESELECT_CARD,
    DRV_SDMMC_INIT_DONE,
    DRV_SDMMC_INIT_ERROR,
} DRV_SDMMC_INIT_STATES;

typedef enum
{
    DRV_SDMMC_TASK_WAIT_FOR_DEVICE_ATTACH = 0,
    DRV_SDMMC_TASK_MEDIA_INIT,
    DRV_SDMMC_TASK_PROCESS_QUEUE,
    DRV_SDMMC_TASK_CHECK_CARD_DETACH,
    DRV_SDMMC_TASK_SELECT_CARD,
    DRV_SDMMC_TASK_SETUP_XFER,
    DRV_SDMMC_TASK_XFER_COMMAND,
    DRV_SDMMC_TASK_XFER_STATUS,
    DRV_SDMMC_TASK_SEND_STOP_TRANS_CMD,
    DRV_SDMMC_TASK_CHECK_CARD_STATUS,
    DRV_SDMMC_TASK_DESELECT_CARD,
    DRV_SDMMC_TASK_ERROR,
    DRV_SDMMC_TASK_TRANSFER_COMPLETE,
    DRV_SDMMC_TASK_HANDLE_CARD_DETACH

} DRV_SDMMC_TASK_STATES;

typedef enum
{
    /* Command code to reset the SD card */
    DRV_SDMMC_CMD_GO_IDLE_STATE = 0,

    /* Command code to initialize the SD card */
    DRV_SDMMC_CMD_SEND_OP_COND  = 1,

    /* Broadcast command code to get all card IDs */
    DRV_SDMMC_CMD_ALL_SEND_CID  = 2,

    /* Command card to respond with its RCA, tells it to go to standby state */
    DRV_SDMMC_CMD_SEND_RCA  = 3,

    /* ACMD6 sets the card's bus width between 1-bit and 4-bit, only available when the card is unlocked */
    DRV_SDMMC_CMD_SWITCH_FUNCTION = 6,

    /* ACMD6 sets the card's bus width between 1-bit and 4-bit, only available when the card is unlocked */
    DRV_SDMMC_CMD_SET_BUS_WIDTH  = 6,

    /* Select/Deselect card message, sends the card to transfer state */
    DRV_SDMMC_CMD_SELECT_DESELECT_CARD = 7,

    /* This macro defined the command code to check for sector addressing */
    DRV_SDMMC_CMD_SEND_IF_COND  = 8,

    /* for MMC CMD8 is used to fetch ext csd */
    DRV_SDMMC_CMD_SEND_EXT_CSD = 8,

    /* Command code to get the Card Specific Data */
    DRV_SDMMC_CMD_SEND_CSD      = 9,

    /* Command code to get the Card Information */
    DRV_SDMMC_CMD_SEND_CID      = 10,

    /* Command code to stop transmission during a multi-block read */
    DRV_SDMMC_CMD_STOP_TRANSMISSION = 12,

    /* Command code to get the card status information */
    DRV_SDMMC_CMD_SEND_STATUS       = 13,

    /* Command code to set the block length of the card */
    DRV_SDMMC_CMD_SET_BLOCKLEN      = 16,

    /* Command code to read one block from the card */
    DRV_SDMMC_CMD_READ_SINGLE_BLOCK  = 17,

    /* Command code to read multiple blocks from the card */
    DRV_SDMMC_CMD_READ_MULTI_BLOCK   = 18,

    /* Command code to tell the media how many blocks to pre-erase
     (for faster multi-block writes to follow)
     Note: This is an "application specific" command.  This tells the media how
     many blocks to pre-erase for the subsequent WRITE_MULTI_BLOCK */
    DRV_SDMMC_CMD_SET_WR_BLK_ERASE_COUNT =  23,

    /* Command code to write one block to the card */
    DRV_SDMMC_CMD_WRITE_SINGLE_BLOCK  = 24,

    /* Command code to write multiple blocks to the card */
    DRV_SDMMC_CMD_WRITE_MULTI_BLOCK   = 25,

    /* Command code to set the address of the start of an erase operation */
    DRV_SDMMC_CMD_TAG_SECTOR_START    = 32,

    /* Command code to set the address of the end of an erase operation */
    DRV_SDMMC_CMD_TAG_SECTOR_END      = 33,

    /* Command code to erase all previously selected blocks */
    DRV_SDMMC_CMD_ERASE              =  38,

    /* Command code to initialize an SD card and provide the CSD register value.
    Note: this is an "application specific" command (specific to SD cards)
    and must be preceded by CMD_APP_CMD */
    DRV_SDMMC_CMD_SD_SEND_OP_COND     = 41,

    /* Command code to get the SCR register information from the card */
    DRV_SDMMC_CMD_READ_SCR            = 51,

    /* Command code to begin application specific command inputs */
    DRV_SDMMC_CMD_APP_CMD             = 55,

    /* Command code to get the OCR register information from the card */
    DRV_SDMMC_CMD_READ_OCR            = 58,

    /* Command code to disable CRC checking */
    DRV_SDMMC_CMD_CRC_ON_OFF          = 59,

} DRV_SDMMC_SD_COMMAND;

typedef enum
{
    DRV_SDMMC_CMD_LINE_STATE_CHECK = 0,
    DRV_SDMMC_CMD_FRAME_AND_SEND_CMD,
    DRV_SDMMC_CMD_CHECK_TRANSFER_COMPLETE,
    DRV_SDMMC_CMD_EXEC_IS_COMPLETE

} DRV_SDMMC_COMMAND_STATES;

typedef enum
{
    /* Set the clock divider value. */
    DRV_SDMMC_CLOCK_SET_DIVIDER = 0,

    /* Wait for the internal clock stable bit to be set. */
    DRV_SDMMC_CLOCK_PRE_ENABLE_DELAY,

    /* Enable the clock. */
    DRV_SDMMC_CLOCK_ENABLE,

    /* Delay post clock enable. */
    DRV_SDMMC_CLOCK_POST_ENABLE_DELAY,

    /* Clock set is complete. */
    DRV_SDMMC_CLOCK_SET_COMPLETE

} DRV_SDMMC_CLOCK_STATES;

typedef struct
{
    uint8_t __ALIGNED(4)            cidBuffer[DRV_SDMMC_CID_BUFFER_LEN];
    uint8_t __ALIGNED(4)            csdBuffer[DRV_SDMMC_CSD_BUFFER_LEN];
    uint8_t __ALIGNED(32)           scrBuffer[DRV_SDMMC_SCR_BUFFER_LEN];
    uint8_t __ALIGNED(32)           switchStatusBuffer[DRV_SDMMC_SWITCH_STATUS_BUFFER_LEN];
    bool                            isAttached;
    DRV_SDMMC_BUS_WIDTH             busWidth;
    uint16_t                        rca;
    uint8_t                         cmd6Mode;
    uint8_t                         voltWindow;
    /* SDSD or SDHC card type */
    DRV_SDMMC_CARD_TYPE             cardType;
    /* Capacity of the card in number of blocks. */
    uint32_t                        discCapacity;
    uint8_t                         cardVer;
    bool                            isWriteProtected;
    bool                            isLocked;
    /* Variables to track the command/data transfer status. */
    bool                            isDataCompleted;
    bool                            isCommandCompleted;
    uint16_t                        errorFlag;
    uint32_t                        currentSpeed;
} DRV_SDHOST_CARD_CTXT;

// *****************************************************************************
/* SD Host Controller Driver Client Object

  Summary:
    Defines the object required for the maintenance of the software clients

  Description:
    This defines the object required for the maintenance of the software
    clients instance. This object exists once per client instance.

  Remarks:
    None
*/

typedef struct
{
    /* The hardware instance index associated with the client */
    SYS_MODULE_INDEX                    drvIndex;

    /* Flag to indicate in use  */
    bool                                inUse;

    /* The intent with which the client was opened */
    DRV_IO_INTENT                       intent;

    /* Client specific event handler */
    DRV_SDMMC_EVENT_HANDLER             eventHandler;

    /* Client specific context */
    uintptr_t                           context;

    /* Client handle assigned to this client object when it was opened */
    DRV_HANDLE                          clientHandle;

} DRV_SDMMC_CLIENT_OBJ;


typedef struct DRV_SDMMC_BUFFER_OBJ
{
    /* True if object is allocated */
    bool                                inUse;

    /* Handle to the client that owns this buffer object */
    DRV_HANDLE                          clientHandle;

    /* Present status of this command */
    DRV_SDMMC_COMMAND_STATUS            status;

    /* Current command handle of this buffer object */
    DRV_SDMMC_COMMAND_HANDLE            commandHandle;

    /* Pointer to the source/destination buffer */
    uint8_t*                            buffer;

    /* Start address of the operation. */
    uint32_t                            blockStart;

    /* Number of blocks */
    uint32_t                            nBlocks;

    /* Op code associated with the buffer object. */
    uint8_t                             opCode;

    /* Operation type - read/write */
    DRV_SDMMC_OPERATION_TYPE            opType;

    /* Pointer to the next buffer in the queue */
    struct DRV_SDMMC_BUFFER_OBJ*        next;

} DRV_SDMMC_BUFFER_OBJ;


// *****************************************************************************
/* SD Host Controller Driver Hardware Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None
*/

typedef struct
{
    /* The status of the driver */
    SYS_STATUS                      status;

    /* Flag to indicate in use  */
    bool                            inUse;

    /* Card detection method to use */
    DRV_SDMMC_CD_METHOD             cardDetectionMethod;

    /* Rate at which card insertion/removal is checked if polling method is selected */
    uint32_t                        cardDetectionPollingIntervalMs;

    /* SDMMC PLIB API */
    const DRV_SDMMC_PLIB_API*       sdmmcPlib;

    /* Card context data */
    DRV_SDHOST_CARD_CTXT            cardCtxt;

    /* Flag to indicate if the driver is used in exclusive mode of operation */
    bool                            isExclusive;

    /* Instance specific token counter used to generate unique client/transfer handles */
    uint16_t                        sdmmcTokenCount;

    /* Size of buffer objects queue */
    uint32_t                        bufferObjPoolSize;

    /* Pointer to the buffer pool */
    uintptr_t                       bufferObjPool;

    /* Linked list of buffer objects */
    uintptr_t                       bufferObjList;

    /* Number of active clients */
    size_t                          nClients;

    /* Maximum number of clients */
    size_t                          nClientsMax;

    /* Memory Pool for Client Objects */
    uintptr_t                       clientObjPool;

    /* Whether the write protect pin should be used to check if the card is
     * write protected. */
    bool                            isWriteProtectCheckEnabled;

    /* Sent command status. */
    uint8_t                         commandStatus;

    /* Status of the device */
    SYS_MEDIA_STATUS                mediaState;

    /* This variable holds the current state of the DRV_SDMMC_Tasks */
    DRV_SDMMC_TASK_STATES           taskState;

    /* Different stages in media initialization */
    DRV_SDMMC_INIT_STATES           initState;

    DRV_SDMMC_INIT_STATES           nextInitState;

    /* Different states in sending a command */
    DRV_SDMMC_COMMAND_STATES        cmdState;

    /* Data transfer flags */
    DRV_SDMMC_DataTransferFlags     dataTransferFlags;

    /* Different states in setting up the clock */
    DRV_SDMMC_CLOCK_STATES          clockState;

    /* System Timer Handle */
    SYS_TIME_HANDLE                 tmrHandle;

    SYS_TIME_HANDLE                 generalTimerHandle;

    /* Speed mode - Default Speed or High Speed mode of operation. */
    DRV_SDMMC_SPEED_MODE            speedMode;

    /* Bus width to be used for the card. */
    DRV_SDMMC_BUS_WIDTH             busWidth;

    /* Timer command handle. */
    SYS_TIME_HANDLE                 cmdTimerHandle;

    /* Timer command state. */
    bool                            isCmdTimerExpired;

    /* Variable used to track the number of trials of a particular operation */
    uint16_t                        trials;

    /* SDMMC driver geometry object */
    SYS_MEDIA_GEOMETRY              mediaGeometryObj;

    /* SDMMC driver media geometry table. */
    SYS_MEDIA_REGION_GEOMETRY       mediaGeometryTable[3];

    /* Mutex to protect access to the transfer objects */
    OSAL_MUTEX_DECLARE(mutex);

    /* Mutex to protect access to the client objects */
    OSAL_MUTEX_DECLARE(mutexClientObjects);

} DRV_SDMMC_OBJ;

#endif //#ifndef _DRV_SDMMC_LOCAL_H