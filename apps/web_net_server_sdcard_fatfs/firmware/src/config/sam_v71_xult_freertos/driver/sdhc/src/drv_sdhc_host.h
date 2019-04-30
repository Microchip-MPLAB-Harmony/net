/*******************************************************************************
  SD Host Controller Driver Interface

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdhc_host.h

  Summary:
    SD Host Controller Driver System Host Definitions

  Description:
    The SD Host Controller driver provides a simple interface to manage the SD
    Host Controller peripheral.  This file defines the interface definitions
    and prototypes for the SD Host Controller driver.
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

#ifndef _DRV_SDHC_HOST_H
#define _DRV_SDHC_HOST_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driver/sdhc/src/drv_sdhc_host_local.h"

/* SD Host Controller Spec version. */
#define SDHOST_HOST_SPEC_VER_1              0
#define SDHOST_HOST_SPEC_VER_2              1
#define SDHOST_HOST_SPEC_VER_3	            2

#define SDHOST_CLOCK_FREQ_400_KHZ           (400000)
#define SDHOST_CLOCK_FREQ_DS_25_MHZ         (25000000)
#define SDHOST_CLOCK_FREQ_HS_50_MHZ         (50000000)

typedef enum
{
    SDHOST_RESET_ALL = 0x01,
    SDHOST_RESET_CMD = 0x02,
    SDHOST_RESET_DAT = 0x04
} SDHOST_RESET_TYPE;

typedef enum
{
    SDHOST_BUS_WIDTH_1_BIT = 0,
    SDHOST_BUS_WIDTH_4_BIT,
} SDHOST_BUS_WIDTH;

typedef enum
{
    SDHOST_SPEED_MODE_NORMAL = 0,
    SDHOST_SPEED_MODE_HIGH
} SDHOST_SPEED_MODE;

typedef enum
{
    SDHOST_READ_RESP_REG_0 = 0,
    SDHOST_READ_RESP_REG_1,
    SDHOST_READ_RESP_REG_2,
    SDHOST_READ_RESP_REG_3,
    SDHOST_READ_RESP_REG_ALL
} SDHOST_READ_RESPONSE_REG;

typedef enum SDHOST_CMD_RESP_TYPE
{
    SDHOST_CMD_RESP_NONE,   /*!< no response type */
    SDHOST_CMD_RESP_R1,     /*!< normal response command */
    SDHOST_CMD_RESP_R1B,    /*!< normal with busy signal */
    SDHOST_CMD_RESP_R2,     /*!< CID, CSD register */
    SDHOST_CMD_RESP_R3,     /*!< OCR register */
    SDHOST_CMD_RESP_R4,     /*!< */
    SDHOST_CMD_RESP_R5,     /*!< */
    SDHOST_CMD_RESP_R6,     /*!< Published RCA response  */
    SDHOST_CMD_RESP_R7      /*!< Card interface condition  */

} SDHOST_CMD_RESP_TYPE;


typedef struct
{
    uint8_t isAttached;
    uint8_t busWidth;

    uint16_t rca;

    uint8_t *scr;
    uint8_t *cid;
    uint8_t *csd;
    uint8_t *ocr;
    uint8_t *switchStatus;

    uint8_t cmd6Mode;
    uint8_t voltWindow;
    /* HC or Normal card. */
    uint8_t cardType;
    /* Capacity of the card in number of blocks. */
    uint32_t discCapacity;

    uint8_t cardVer;
    uint8_t writeProtected;
    uint8_t locked;

    /* Variables to track the command/data status. */
    uint8_t dataCompleted;
    uint8_t commandCompleted;
    uint8_t waitForCmdResp;
    uint8_t waitForData;
    uint8_t isDataPresent;
    uint8_t commandError;
    uint8_t errorFlag;

} SDHOST_CARD_CTXT;

// *****************************************************************************
/* SD Host Controller transfer type

  Summary:
    Specifies whether the SD card transfer is read or write.

  Description:
    This enumeration holds constants to specify whether the SD card transfer
    is read or write.

  Remarks:
    None.
*/

typedef enum
{
    /* SD card transfer is read from the card */
    DRV_SDHC_OPERATION_TYPE_READ,

   /* SD card transfer is write to the card */
    DRV_SDHC_OPERATION_TYPE_WRITE,

}DRV_SDHC_OPERATION_TYPE;

// *****************************************************************************
/* SD Host Controller commands

  Summary:
    Lists different commands supported by the SD card.

  Description:
    This enumeration lists different commands supported by the SD card

  Remarks:
    None.
*/

typedef enum
{
    /* Command code to reset the SD card */
    DRV_SDHC_CMD_GO_IDLE_STATE = 0,

    /* Command code to initialize the SD card */
    DRV_SDHC_CMD_SEND_OP_COND  = 1,

    /* Broadcast command code to get all card IDs */
    DRV_SDHC_CMD_ALL_SEND_CID  = 2,

    /* Command card to respond with its RCA, tells it to go to standby state */
    DRV_SDHC_CMD_SEND_RCA  = 3,

    /* ACMD6 sets the card's bus width between 1-bit and 4-bit, only available when the card is unlocked */
    DRV_SDHC_CMD_SWITCH_FUNCTION = 6,
    /* ACMD6 sets the card's bus width between 1-bit and 4-bit, only available when the card is unlocked */
    DRV_SDHC_CMD_SET_BUS_WIDTH  = 6,

    /* Select/Deselect card message, sends the card to transfer state */
    DRV_SDHC_CMD_SELECT_DESELECT_CARD = 7,

    /* This macro defined the command code to check for sector addressing */
    DRV_SDHC_CMD_SEND_IF_COND  = 8,

    /* Command code to get the Card Specific Data */
    DRV_SDHC_CMD_SEND_CSD      = 9,

    /* Command code to get the Card Information */
    DRV_SDHC_CMD_SEND_CID      = 10,

    /* Command code to stop transmission during a multi-block read */
    DRV_SDHC_CMD_STOP_TRANSMISSION = 12,

    /* Command code to get the card status information */
    DRV_SDHC_CMD_SEND_STATUS       = 13,

    /* Command code to set the block length of the card */
    DRV_SDHC_CMD_SET_BLOCKLEN      = 16,

    /* Command code to read one block from the card */
    DRV_SDHC_CMD_READ_SINGLE_BLOCK  = 17,

    /* Command code to read multiple blocks from the card */
    DRV_SDHC_CMD_READ_MULTI_BLOCK   = 18,

    /* Command code to tell the media how many blocks to pre-erase
     (for faster multi-block writes to follow)
     Note: This is an "application specific" command.  This tells the media how
     many blocks to pre-erase for the subsequent WRITE_MULTI_BLOCK */
    DRV_SDHC_CMD_SET_WR_BLK_ERASE_COUNT =  23,

    /* Command code to write one block to the card */
    DRV_SDHC_CMD_WRITE_SINGLE_BLOCK  = 24,

    /* Command code to write multiple blocks to the card */
    DRV_SDHC_CMD_WRITE_MULTI_BLOCK   = 25,

    /* Command code to set the address of the start of an erase operation */
    DRV_SDHC_CMD_TAG_SECTOR_START    = 32,

    /* Command code to set the address of the end of an erase operation */
    DRV_SDHC_CMD_TAG_SECTOR_END      = 33,

    /* Command code to erase all previously selected blocks */
    DRV_SDHC_CMD_ERASE              =  38,

    /* Command code to initialise an SD card and provide the CSD register value.
    Note: this is an "application specific" command (specific to SD cards)
    and must be preceded by CMD_APP_CMD */
    DRV_SDHC_CMD_SD_SEND_OP_COND     = 41,

    /* Command code to get the SCR register information from the card */
    DRV_SDHC_CMD_READ_SCR            = 51,

    /* Command code to begin application specific command inputs */
    DRV_SDHC_CMD_APP_CMD             = 55,

    /* Command code to get the OCR register information from the card */
    DRV_SDHC_CMD_READ_OCR            = 58,

    /* Command code to disable CRC checking */
    DRV_SDHC_CMD_CRC_ON_OFF          = 59,

} DRV_SDHC_SD_COMMAND;

void sdhostResetError
(
    SDHOST_RESET_TYPE resetType
);

void sdhostSetBusWidth
(
    SDHOST_BUS_WIDTH busWidth
);

void sdhostSetSpeedMode
(
    SDHOST_SPEED_MODE speedMode
);

void sdhostSetupDma
(
    uint8_t *buffer,
    uint16_t numBytes,
    DRV_SDHC_OPERATION_TYPE operation
);

void sdhostSetBlockSize
(
    uint16_t blockSize
);

void sdhostSetBlockCount
(
    uint16_t numBlocks
);

bool sdhostIsCardAttached
(
    void
);

void sdhostSetClock
(
    uint32_t clock
);

void sdhostClockEnable
(
    void
);

void sdhostClockDisable
(
    void
);

void sdhostInterruptHandler
(
    SDHOST_CARD_CTXT *cardCtxt
);

bool sdhostIsCmdLineBusy
(
    void
);

bool sdhostIsDat0LineBusy
(
    void
);

void sdhostReadResponse
(
    SDHOST_READ_RESPONSE_REG respReg,
    uint32_t *response
);

void sdhostSendCommand
(
    uint8_t opCode,
    uint8_t respType,
    uint8_t dataPresent,
    uint32_t argument
);

bool sdhostIsWriteProtected
(
    void
);

bool sdhostInit
(
    SDHOST_CARD_CTXT **cardCtxt
);

void sdhostSetThreshold
(
    void
);

void sdhostCardDetectEnable
(
    void
);

void sdhostCardDetectDisable
(
    void
);

void sdhostWriteProtectEnable
(
    void
);

void sdhostWriteProtectDisable
(
    void
);
void sdhostParseCsd
(
    SDHOST_CARD_CTXT *cardCtxt
);
#endif
