/*******************************************************************************
  SD Card SPI Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdspi_local.h

  Summary:
    SD Card (SPI) Driver Local Data Structures

  Description:
    Driver Local Data Structures
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef _DRV_SDSPI_LOCAL_H
#define _DRV_SDSPI_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "system/dma/sys_dma.h"
#include "driver/sdspi/drv_sdspi.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

#define _DRV_SDSPI_CLIENT_INDEX_MASK               (0x000000FF)
#define _DRV_SDSPI_INSTANCE_INDEX_MASK             (0x0000FF00)
#define _DRV_SDSPI_TOKEN_MASK                      (0xFFFF0000)
#define _DRV_SDSPI_TOKEN_MAX                       (0x0000FFFF)

#define _DRV_SDSPI_INSTANCE_GET(object)            &gDrvSDSPIObj[object]

#define _DRV_SDSPI_COMMAND_RESPONSE_TRIES           (8)
#define _DRV_SDSPI_R1B_RESP_TIMEOUT                (100)

#define _DRV_SDSPI_READ_TIMEOUT_IN_MS              (250)
#define _DRV_SDSPI_WRITE_TIMEOUT_IN_MS             (250)

#define _DRV_SDSPI_APP_CMD_RESP_TIMEOUT_IN_MS      (1000)
#define _DRV_SDSPI_CSD_TOKEN_TIMEOUT_IN_MS         (1000)

#if defined (DRV_SDSPI_ENABLE_WRITE_PROTECT_CHECK)
    #define _DRV_SDSPI_EnableWriteProtectCheck() true
#else
    #define _DRV_SDSPI_EnableWriteProtectCheck() false
#endif

/* This macro holds the value of the initial communication speed with SD card.
 * SD card only work at <=400kHz SPI frequency during initialization.
*/
#define _DRV_SDSPI_SPI_INITIAL_SPEED                            (400000)

/* No of bytes to be read for SD card CSD. */
#define _DRV_SDSPI_CSD_READ_SIZE                                (20)

/* No of bytes to be read for SD card CSD. */
#define _DRV_SDSPI_CID_READ_SIZE                                (20)

/* SD card V2 device type Check. */
#define _DRV_SDSPI_CHECK_V2_DEVICE                              (0xC0)

/* SD card Block size */
#define _DRV_SDSPI_MEDIA_BLOCK_SIZE                             (512)

/* SD card transmit bit position. */
#define DRV_SDSPI_TRANSMIT_SET                                  (0x40)

/* Write response token bit mask.*/
#define DRV_SDSPI_WRITE_RESPONSE_TOKEN_MASK                     (0x1F)

/* SPI Bus floating */
#define DRV_SDSPI_MMC_FLOATING_BUS                              (0xFF)

/* SD card start single data block token. */
#define DRV_SDSPI_DATA_START_TOKEN                              (0xFE)

/* SD card start multiple blocks token. */
#define DRV_SDSPI_DATA_START_MULTI_BLOCK_TOKEN                  (0xFC)

/* SD card stop transmission token. */
#define DRV_SDSPI_DATA_STOP_TRAN_TOKEN                          (0xFD)

/* SD card data accepted token. */
#define DRV_SDSPI_DATA_ACCEPTED                                 (0x05)

/* SD card R1 response end bit. */
#define CMD_R1_END_BIT_SET                                      (0x01)

/* SD card initial packet size. */
#define DRV_SDSPI_PACKET_SIZE                                   (6)

/* SD card Media initialization array size. */
#define MEDIA_INIT_ARRAY_SIZE                                   (10)

/* SD card Media detect retry period in Milliseconds. */
#define MEDIA_DETECT_RETRY_PERIOD_MS                            (5)

typedef enum
{
    DRV_SDSPI_OPERATION_TYPE_READ,

    DRV_SDSPI_OPERATION_TYPE_WRITE,

} DRV_SDSPI_OPERATION_TYPE;

typedef enum
{
    DRV_SDSPI_SPI_TRANSFER_TYPE_BYTE = 0,

    DRV_SDSPI_SPI_TRANSFER_TYPE_BLOCK,

} DRV_SDSPI_SPI_TRANSFER_TYPE;

typedef enum
{
    /* All data was transferred successfully. */

    DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS,

    DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE,

    /* There was an error while processing transfer request. */
    DRV_SDSPI_SPI_TRANSFER_STATUS_ERROR,

} DRV_SDSPI_SPI_TRANSFER_STATUS;


/* SD Card Driver read task Status. */
typedef enum
{

    /* Initial state of the task, check for SD card attach/detach */
    DRV_SDSPI_TASK_START_POLLING_TIMER,

    DRV_SDSPI_TASK_CHECK_DEVICE,

    /* If the card is attached, initialize */
    DRV_SDSPI_TASK_MEDIA_INIT,

    /* Idle State  */
    DRV_SDSPI_TASK_IDLE

} DRV_SDSPI_TASK_STATES;

/* SD Card connection states. */
typedef enum
{
    /* SD Card is attached from the system */
    DRV_SDSPI_IS_DETACHED,

    /* SD Card is attached to the system */
    DRV_SDSPI_IS_ATTACHED

} DRV_SDSPI_ATTACH;

/* SD Card commands. */
typedef enum
{
    /* Command code to reset the SD card */
    CMD_VALUE_GO_IDLE_STATE = 0,

    /* Command code to initialize the SD card */
    CMD_VALUE_SEND_OP_COND  = 1,

    /* This macro defined the command code to check for sector addressing */
    CMD_VALUE_SEND_IF_COND  = 8,

    /* Command code to get the Card Specific Data */
    CMD_VALUE_SEND_CSD      = 9,

    /* Command code to get the Card Information */
    CMD_VALUE_SEND_CID      = 10,

    /* Command code to stop transmission during a multi-block read */
    CMD_VALUE_STOP_TRANSMISSION = 12,

    /* Command code to get the card status information */
    CMD_VALUE_SEND_STATUS       = 13,

    /* Command code to set the block length of the card */
    CMD_VALUE_SET_BLOCKLEN      = 16,

    /* Command code to read one block from the card */
    CMD_VALUE_READ_SINGLE_BLOCK  = 17,

    /* Command code to read multiple blocks from the card */
    CMD_VALUE_READ_MULTI_BLOCK   = 18,

    /* Command code to tell the media how many blocks to pre-erase
     (for faster multi-block writes to follow)
     Note: This is an "application specific" command.  This tells the media how
     many blocks to pre-erase for the subsequent WRITE_MULTI_BLOCK */
    CMD_VALUE_SET_WR_BLK_ERASE_COUNT =  23,

    /* Command code to write one block to the card */
    CMD_VALUE_WRITE_SINGLE_BLOCK  = 24,

    /* Command code to write multiple blocks to the card */
    CMD_VALUE_WRITE_MULTI_BLOCK   = 25,

    /* Command code to set the address of the start of an erase operation */
    CMD_VALUE_TAG_SECTOR_START    = 32,

    /* Command code to set the address of the end of an erase operation */
    CMD_VALUE_TAG_SECTOR_END      = 33,

    /* Command code to erase all previously selected blocks */
    CMD_VALUE_ERASE              =  38,

    /* Command code to initialise an SD card and provide the CSD register value.
    Note: this is an "application specific" command (specific to SD cards)
    and must be preceded by CMD_APP_CMD */
    CMD_VALUE_SD_SEND_OP_COND     = 41,

    /* Command code to begin application specific command inputs */
    CMD_VALUE_APP_CMD             = 55,

    /* Command code to get the OCR register information from the card */
    CMD_VALUE_READ_OCR            = 58,

    /* Command code to disable CRC checking */
    CMD_VALUE_CRC_ON_OFF          = 59,

} DRV_SDSPI_COMMAND_VALUE;

/* SD Card Responses. */
typedef enum
{
    /* R1 type response */
    RESPONSE_R1,

    /* R1b type response */
    RESPONSE_R1b,

    /* R2 type response */
    RESPONSE_R2,

    /* R3 type response */
    RESPONSE_R3,

    /* R7 type response */
    RESPONSE_R7

} DRV_SDSPI_RESPONSES;

/* SD Card command indices */
typedef enum
{
    /* Index of in the CMD_GO_IDLE_STATE command 'Command array' */
    DRV_SDSPI_GO_IDLE_STATE,

    /* Index of in the CMD_SEND_OP_COND command 'Command array' */
    DRV_SDSPI_SEND_OP_COND,

    /* Index of in the CMD_SEND_IF_COND command 'Command array' */
    DRV_SDSPI_SEND_IF_COND,

    /* Index of in the CMD_SEND_CSD command 'Command array' */
    DRV_SDSPI_SEND_CSD,

    /* Index of in the CMD_SEND_CID command 'Command array' */
    DRV_SDSPI_SEND_CID,

    /* Index of in the CMD_STOP_TRANSMISSION command 'Command array' */
    DRV_SDSPI_STOP_TRANSMISSION,

    /* Index of in the CMD_SEND_STATUS command 'Command array' */
    DRV_SDSPI_SEND_STATUS,

    /* Index of in the CMD_SET_BLOCKLEN command 'Command array' */
    DRV_SDSPI_SET_BLOCKLEN,

    /* Index of in the CMD_READ_SINGLE_BLOCK command 'Command array' */
    DRV_SDSPI_READ_SINGLE_BLOCK,

    /* Index of in the CMD_READ_MULTI_BLOCK command 'Command array' */
    DRV_SDSPI_READ_MULTI_BLOCK,

    /* Index of in the CMD_WRITE_SINGLE_BLOCK command 'Command array' */
    DRV_SDSPI_WRITE_SINGLE_BLOCK,

    /* Index of in the CMD_WRITE_MULTI_BLOCK command 'Command array' */
    DRV_SDSPI_WRITE_MULTI_BLOCK,

    /* Index of in the CMD_TAG_SECTOR_START command 'Command array' */
    DRV_SDSPI_TAG_SECTOR_START,

    /* Index of in the CMD_TAG_SECTOR_END command 'Command array' */
    DRV_SDSPI_TAG_SECTOR_END,

    /* Index of in the CMD_ERASE command 'Command array' */
    DRV_SDSPI_ERASE,

    /* Index of in the CMD_APP_CMD command 'Command array' */
    DRV_SDSPI_APP_CMD,

    /* Index of in the CMD_READ_OCR command 'Command array' */
    DRV_SDSPI_READ_OCR,

    /* Index of in the CMD_CRC_ON_OFF command 'Command array' */
    DRV_SDSPI_CRC_ON_OFF,

    /* Index of in the CMD_SD_SEND_OP_COND command 'Command array' */
    DRV_SDSPI_SD_SEND_OP_COND,

    /* Index of in the CMD_SET_WR_BLK_ERASE_COUNT command 'Command array' */
    DRV_SDSPI_SET_WR_BLK_ERASE_COUNT

} DRV_SDSPI_COMMANDS;

/* SD Card type. */
typedef enum
{
    /* Normal SD Card */
    DRV_SDSPI_MODE_NORMAL,

    /* SDHC type Card */
    DRV_SDSPI_MODE_HC

} DRV_SDSPI_TYPE;

/* SD Card initialization states. */
typedef enum
{
    /* Initial state */
    DRV_SDSPI_CMD_DETECT_START_INIT,

    DRV_SDSPI_CMD_DETECT_START_POLLING_TIMER,

    DRV_SDSPI_CMD_DETECT_WAIT_POLL_TIMER_EXPIRY,

    DRV_SDSPI_CMD_DETECT_CHECK_FOR_CARD,

    /* Check whether the card has been detached. */
    DRV_SDSPI_CMD_DETECT_CHECK_FOR_DETACH

} DRV_SDSPI_CMD_DETECT_STATES;

/* SD Card initialization stages. */
typedef enum
{
    DRV_SDSPI_INIT_SPI,

    /* Card ramp up time. Issue clock cycles. */
    DRV_SDSPI_INIT_RAMP_TIME,

    /* Reset the SD Card using CMD0 */
    DRV_SDSPI_INIT_RESET_SDCARD,

    /* Check the card's interface condition */
    DRV_SDSPI_INIT_CHK_IFACE_CONDITION,

    /* Send APP CMD ACMD41 to check if the card's internal init is done */
    DRV_SDSPI_INIT_SEND_ACMD41,

    /* Read the Card's Operating Conditions Register */
    DRV_SDSPI_INIT_READ_OCR,

    /* Card's internal init is complete. Increase the SPI frequency. */
    DRV_SDSPI_INIT_INCR_CLOCK_SPEED,

    /* Issue command to read the card's Card Specific Data register */
    DRV_SDSPI_INIT_READ_CSD,

    /* Read card identification data */
    DRV_SDSPI_INIT_READ_CID,

    /* Issue command to turn off the CRC */
    DRV_SDSPI_INIT_TURN_OFF_CRC,

    /* Set the block length of the card */
    DRV_SDSPI_INIT_SET_BLOCKLEN,

    /* SD Card Init is done */
    DRV_SDSPI_INIT_SD_INIT_DONE,

    /* Error initializing the SD card */
    DRV_SDSPI_INIT_ERROR

} DRV_SDSPI_INIT_STATE;

/* SD Card Command data structure. */
typedef struct
{
    /* Command code */
    DRV_SDSPI_COMMAND_VALUE      commandCode;

    /* CRC value for that command */
    uint8_t      crc;

    /* Response type */
    DRV_SDSPI_RESPONSES    responseType;

    /* Response Length */
    uint32_t    responseLength;

} DRV_SDSPI_CMD_OBJ;

/* SD Card R1 type response format. */
typedef union
{
    /* Byte-wise access */
    uint8_t byte;

    /* This structure allows bitwise access of the response */
    struct
    {
        /* Card is in idle state */
        uint8_t inIdleState:1;

        /* Erase reset flag */
        uint8_t eraseReset:1;

        /* Illegal command flag */
        uint8_t illegalCommand:1;

        /* CRC error flag */
        uint8_t crcError:1;

        /* Erase sequence error flag */
        uint8_t eraseSequenceError:1;

        /* Address error flag */
        uint8_t addressError:1;

        /* Parameter flag */
        uint8_t parameterError:1;

        /* Unused bit 7 */
        uint8_t unusedB7:1;
    };

} DRV_SDSPI_RESPONSE_1;

/* SD Card R2 type response format. */
typedef union
{
    /* Get both the bytes */
    uint16_t word;

    struct
    {
        /* Byte-wise access */
        uint8_t      byte0;
        uint8_t      byte1;
    };
    struct
    {
        /* Card is in idle state */
        uint16_t inIdleState:1;

        /* Erase reset flag */
        uint16_t eraseReset:1;

        /* Illegal command flag */
        uint16_t illegalCommand:1;

        /* CRC error flag */
        uint16_t crcError:1;

        /* Erase sequence error flag */
        uint16_t eraseSequenceError:1;

        /* Address error flag */
        uint16_t addressError:1;

        /* Parameter error flag */
        uint16_t parameterError:1;

        /* Un-used bit */
        uint16_t unusedB7:1;

        /* Card is locked? */
        uint16_t cardIsLocked:1;

        /* WP erase skip| lock/unlock command failed */
        uint16_t wpEraseSkipLockFail:1;

        /* Error */
        uint16_t error:1;

        /* CC error */
        uint16_t ccError:1;

        /* Card ECC fail */
        uint16_t cardEccFail:1;

        /* WP violation */
        uint16_t wpViolation:1;

        /* Erase parameter */
        uint16_t eraseParam:1;

        /* out of range or CSD over write */
        uint16_t outrangeCsdOverWrite:1;
    };

} DRV_SDSPI_RESPONSE_2;


// *****************************************************************************
/* SD Card R7 and R3 type response format

  Summary:
    Different ways to access R3/R7 type response packet.

  Description:
    This union represents different ways to access an SD card R3/R7 type
    response packet.

  Remarks:
    Note: The SD card argument response field is 32-bit, big endian format.
    However, the C compiler stores 32-bit values in little endian in RAM. When
    writing to the _returnVal/argument bytes, make sure to byte swap the order
    from which it arrived over the SPI from the SD card.
*/

typedef union
{
    /* Get all the bytes */
    uint8_t responseBytes[5];
    /* This structure allows bitwise access of the response */
    struct
    {
        struct
        {
            /* Card is in idle state */
            uint8_t inIdleState:1;

            /* Erase reset flag */
            uint8_t eraseReset:1;

            /* Illegal command flag */
            uint8_t illegalCommand:1;

            /* CRC error flag */
            uint8_t crcError:1;

            /* Erase sequence error flag */
            uint8_t eraseSequenceError:1;

            /* Address error flag */
            uint8_t addressError:1;

            /* Parameter error flag */
            uint8_t parameterError:1;

            /* un-used bit B7 */
            uint8_t unusedB7:1;

        }bits;

        /* Read the complete register */
        uint32_t ocrRegister;

    } bitwise;

} DRV_SDSPI_RESPONSE_7;

/* SD Card Responses. */

typedef union
{
    /* SD Card response 1 */
    DRV_SDSPI_RESPONSE_1  response1;

    /* SD Card response 2 */
    DRV_SDSPI_RESPONSE_2  response2;

    /* SD Card response 7 */
    DRV_SDSPI_RESPONSE_7  response7;

} DRV_SDSPI_RESPONSE_PACKETS;

/* SDSPI Driver Instance Object.*/
typedef struct
{
    /* Pointer to the buffer used during sending commands and receiving responses
     * on the SPI bus. Buffer must be cache line aligned when using DMA with cache or
     * the buffer must be in coherent memory region */
    volatile uint8_t                    cmdRespBuffer[32];

    /* Flag to indicate this object is in use  */
    bool                                inUse;

    /* Flag to indicate that SD Card is being used in exclusive access mode */
    bool                                isExclusive;

    /* Memory pool for Client Objects */
    uintptr_t                           clientObjPool;

    /* Keep track of the number of clients that have opened this driver */
    size_t                              nClients;

    /* Maximum number of clients */
    size_t                              nClientsMax;

    /* Present status of the command being executed */
    DRV_SDSPI_COMMAND_STATUS            cmdStatus;

    /* This is an instance specific token counter used to generate unique client
     * handles
     */
    uint16_t                            clientToken;

    /* This is an instance specific token counter used to generate unique buffer
     * handles
     */
    uint16_t                            commandToken;

    DRV_SDSPI_COMMAND_HANDLE            commandHandle;

    /* The status of the driver */
    SYS_STATUS                          status;

    /* This variable holds the current state of the DRV_SDSPI_Task */
    DRV_SDSPI_TASK_STATES               taskState;

    /* Different stages of initialization */
    DRV_SDSPI_CMD_DETECT_STATES         cmdDetectState;

    /* Different stages in media initialization */
    DRV_SDSPI_INIT_STATE                mediaInitState;

    /* Handle to the timer used for command timeouts */
    SYS_TIME_HANDLE                     cardPollingTmrHandle;

    /* Handle to the timer used for read/write transaction timeouts. */
    SYS_TIME_HANDLE                     timerHandle;

    /* Handle to the timer used for command timeouts */
    SYS_TIME_HANDLE                     cmdRespTmrHandle;

    volatile bool                       cardPollingTimerExpired;

    volatile bool                       timerExpired;

    volatile bool                       cmdRespTmrExpired;

    DRV_SDSPI_SPI_TRANSFER_TYPE         sdcardSPITransferType;

    /* Flag to indicate if the device is Standard Speed or High Speed */
    uint8_t                             sdHcHost;

    /* PLIB API list that will be used by the driver to access the hardware */
    const DRV_SDSPI_PLIB_INTERFACE*     spiPlib;

    const uint32_t*                     remapDataBits;

    const uint32_t*                     remapClockPolarity;

    const uint32_t*                     remapClockPhase;

    SYS_PORT_PIN                        chipSelectPin;

    SYS_PORT_PIN                        writeProtectPin;

    /* Speed at which SD card communication should happen */
    uint32_t                            sdcardSpeedHz;

    uint32_t                            pollingIntervalMs;

    uint32_t                            blockStartAddress;

    /* Number of sectors in the SD card */
    uint32_t                            discCapacity;

    /* Flags to track the device presence/absence */
    DRV_SDSPI_ATTACH                    isAttached;

    /* Flag to indicate the SD Card last attached status */
    DRV_SDSPI_ATTACH                    isAttachedLastStatus;

    /* Write protect state */
    uint8_t                             isWriteProtected;

    /* Flag indicating the presence of the SD Card */
    DRV_SDSPI_ATTACH                    mediaState;

    /* SD card type, will be updated by initialization function */
    DRV_SDSPI_TYPE                      sdCardType;

    uint8_t                             csdData[_DRV_SDSPI_CSD_READ_SIZE];

    uint8_t                             cidData[_DRV_SDSPI_CID_READ_SIZE];

    uint8_t                             cmdResponse[5];

    /* SDSPI driver geometry object */
    SYS_MEDIA_GEOMETRY                  mediaGeometryObj;

    /* SDSPI driver media geometry table. */
    SYS_MEDIA_REGION_GEOMETRY           mediaGeometryTable[3];

    /* Transmit DMA Channel */
    SYS_DMA_CHANNEL                     txDMAChannel;

    /* Receive DMA Channel */
    SYS_DMA_CHANNEL                     rxDMAChannel;

    /* This is the SPI transmit register address. Used for DMA operation. */
    void*                               txAddress;

    /* This is the SPI receive register address. Used for DMA operation. */
    void*                               rxAddress;

    /*Pointer to the buffer where the received data needs to be copied */
    void*                               pReceiveData;

    /*Pointer to the buffer containing data to be transmitted */
    void*                               pTransmitData;

    /* Number of bytes pending to be written */
    size_t                              txPending;

    /* Number of bytes to pending to be read */
    size_t                              rxPending;

    /* Number of bytes transferred */
    size_t                              nBytesTransferred;

    /* Buffer for transmitting/receiving dummy data */
    uint8_t                             dummyDataBuffer[512];


    bool                                isFsEnabled;

    volatile DRV_SDSPI_SPI_TRANSFER_STATUS   spiTransferStatus;

    /* Mutex to protect access to SDCard */
    OSAL_MUTEX_DECLARE(transferMutex);

    /* Mutex to protect access to the client object pool */
    OSAL_MUTEX_DECLARE(clientMutex);

    /* Semaphore to wait for data exchange to complete. This is released from ISR */
    OSAL_SEM_DECLARE(transferDone);

} DRV_SDSPI_OBJ;

/* SD Card Driver Client Object. */
typedef struct _DRV_SDSPI_CLIENT_OBJ_STRUCT
{
    /* The hardware instance index associate with the client */
    uint8_t                                        drvIndex;

    /* Flag to indicate in use  */
    bool                                           inUse;

    /* The intent with which the client was opened */
    DRV_IO_INTENT                                  intent;

    /* Client specific event handler */
    DRV_SDSPI_EVENT_HANDLER                        eventHandler;

    /* Client specific context */
    uintptr_t                                      context;

    /* Client handle assigned to this client object when it was opened */
    DRV_HANDLE                                     clientHandle;

} DRV_SDSPI_CLIENT_OBJ;

#endif //#ifndef _DRV_SDSPI_LOCAL_H
