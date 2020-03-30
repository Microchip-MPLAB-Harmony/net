/*******************************************************************************
  SD Host Controller Driver Interface

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdmmc_definitions.h

  Summary:
    SD Host Controller Driver Definitions File

  Description:
    This file defines the definitions and prototypes for the SD Host
    Controller driver.
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

#ifndef _DRV_SDMMC_DEFINITIONS_H
#define _DRV_SDMMC_DEFINITIONS_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

typedef enum
{
    DRV_SDMMC_READ_RESP_REG_0 = 0,
    DRV_SDMMC_READ_RESP_REG_1,
    DRV_SDMMC_READ_RESP_REG_2,
    DRV_SDMMC_READ_RESP_REG_3,
    DRV_SDMMC_READ_RESP_REG_ALL

} DRV_SDMMC_READ_RESPONSE_REG;

typedef enum
{
    DRV_SDMMC_RESET_ALL = 0x01,
    DRV_SDMMC_RESET_CMD = 0x02,
    DRV_SDMMC_RESET_DAT = 0x04

} DRV_SDMMC_RESET_TYPE;

typedef enum
{
    /* Command transfer completed successfully */
    DRV_SDMMC_XFER_STATUS_COMMAND_COMPLETED  = 0x01,

    /* Data transfer completed successfully */
    DRV_SDMMC_XFER_STATUS_DATA_COMPLETED     = 0x02,

    DRV_SDMMC_XFER_STATUS_CARD_INSERTED      = 0x04,

    DRV_SDMMC_XFER_STATUS_CARD_REMOVED       = 0x08

}DRV_SDMMC_XFER_STATUS;

typedef enum
{
    /* 1-bit bus mode */
    DRV_SDMMC_BUS_WIDTH_1_BIT = 0,

    /* 4-bit bus mode */
    DRV_SDMMC_BUS_WIDTH_4_BIT,

    /* 8-bit bus mode (only for emmc) */
    DRV_SDMMC_BUS_WIDTH_8_BIT

} DRV_SDMMC_BUS_WIDTH;

typedef enum
{
    /* SD card default speed - 25/26 MHz */
    DRV_SDMMC_SPEED_MODE_DEFAULT = 0,

    /* SD card high speed - 50/52 MHz */
    DRV_SDMMC_SPEED_MODE_HIGH

} DRV_SDMMC_SPEED_MODE;

typedef enum
{
    /* Use SDCD line to detect card insertion/removal */
    DRV_SDMMC_CD_METHOD_USE_SDCD = 0,

    /* Use command/response method to detect card insertion/removal */
    DRV_SDMMC_CD_METHOD_POLLING,

    /* No card detection method (only for EMMC) */
    DRV_SDMMC_CD_METHOD_NONE,

}DRV_SDMMC_CD_METHOD;

typedef enum
{
    /* SD card transfer is read from the card */
    DRV_SDMMC_OPERATION_TYPE_WRITE = 0,

   /* SD card transfer is write to the card */
    DRV_SDMMC_OPERATION_TYPE_READ,

}DRV_SDMMC_OPERATION_TYPE;

typedef enum
{
    DRV_SDMMC_DATA_TRANSFER_TYPE_SINGLE = 0,
    DRV_SDMMC_DATA_TRANSFER_TYPE_MULTI,
}DRV_SDMMC_DATA_TRANSFER_TYPE;

typedef enum
{
    DRV_SDMMC_DATA_TRANSFER_DIR_WRITE = 0,
    DRV_SDMMC_DATA_TRANSFER_DIR_READ
}DRV_SDMMC_DATA_TRANSFER_DIR;

typedef enum
{
    /* SD Memory  */
    DRV_SDMMC_PROTOCOL_SD,

    /* eMMC device */
    DRV_SDMMC_PROTOCOL_EMMC
}DRV_SDMMC_PROTOCOL;

typedef struct
{
    bool                                 isDataPresent;
    DRV_SDMMC_DATA_TRANSFER_DIR           transferDir;
    DRV_SDMMC_DATA_TRANSFER_TYPE          transferType;

}DRV_SDMMC_DataTransferFlags;

typedef  void (*DRV_SDMMC_CALLBACK) (DRV_SDMMC_XFER_STATUS xferStatus, uintptr_t context);

typedef void (*DRV_SDMMC_PLIB_CALLBACK_REGISTER)(DRV_SDMMC_CALLBACK callback, uintptr_t context);
typedef void (*DRV_SDMMC_PLIB_INIT_MODULE)(void);
typedef bool (*DRV_SDMMC_PLIB_SET_CLOCK)(uint32_t clock);
typedef void (*DRV_SDMMC_PLIB_CLOCK_ENABLE)(void);
typedef bool (*DRV_SDMMC_PLIB_IS_CMD_LINE_BUSY)(void);
typedef bool (*DRV_SDMMC_PLIB_IS_DATA_LINE_BUSY)(void);
typedef void (*DRV_SDMMC_PLIB_RESET_ERROR)(DRV_SDMMC_RESET_TYPE resetType);
typedef void (*DRV_SDMMC_PLIB_SEND_COMMAND)( uint8_t opCode, uint32_t argument, uint8_t respType, DRV_SDMMC_DataTransferFlags flags );
typedef void (*DRV_SDMMC_PLIB_READ_RESPONSE)(DRV_SDMMC_READ_RESPONSE_REG respReg, uint32_t* response );
typedef void (*DRV_SDMMC_PLIB_CARD_DETECT_ENABLE)(void);
typedef void (*DRV_SDMMC_PLIB_CARD_DETECT_DISABLE)(void);
typedef void (*DRV_SDMMC_PLIB_WRITE_PROTECT_ENABLE)(void);
typedef void (*DRV_SDMMC_PLIB_WRITE_PROTECT_DISABLE)(void);
typedef void (*DRV_SDMMC_PLIB_SET_BLOCK_COUNT)(uint16_t numBlocks);
typedef void (*DRV_SDMMC_PLIB_SET_BLOCK_SIZE)(uint16_t blockSize );
typedef void (*DRV_SDMMC_PLIB_SET_BUS_WIDTH)(DRV_SDMMC_BUS_WIDTH busWidth);
typedef void (*DRV_SDMMC_PLIB_SET_SPEED_MODE)(DRV_SDMMC_SPEED_MODE speedMode );
typedef void (*DRV_SDMMC_PLIB_SETUP_DMA)( uint8_t* buffer, uint32_t numBytes, DRV_SDMMC_OPERATION_TYPE operation);
typedef bool (*DRV_SDMMC_PLIB_IS_CARD_ATTACHED)( void );
typedef bool (*DRV_SDMMC_PLIB_IS_WRITE_PROTECTED)( void );
typedef uint16_t (*DRV_SDMMC_PLIB_GET_COMMAND_ERROR)(void);
typedef uint16_t (*DRV_SDMMC_PLIB_GET_DATA_ERROR)(void);

// *****************************************************************************
/* SDMMC Driver PLIB Interface Data

  Summary:
    Defines the data required to initialize the SDMMC driver PLIB Interface.

  Description:
    This data type defines the data required to initialize the SDSPI driver
    PLIB Interface.

  Remarks:
    None.
*/
typedef struct
{
    DRV_SDMMC_PLIB_CALLBACK_REGISTER             sdhostCallbackRegister;
    DRV_SDMMC_PLIB_INIT_MODULE                   sdhostInitModule;
    DRV_SDMMC_PLIB_SET_CLOCK                     sdhostSetClock;
    DRV_SDMMC_PLIB_CLOCK_ENABLE                  sdhostClockEnable;
    DRV_SDMMC_PLIB_IS_CMD_LINE_BUSY              sdhostIsCmdLineBusy;
    DRV_SDMMC_PLIB_IS_DATA_LINE_BUSY             sdhostIsDatLineBusy;
    DRV_SDMMC_PLIB_RESET_ERROR                   sdhostResetError;
    DRV_SDMMC_PLIB_SEND_COMMAND                  sdhostSendCommand;
    DRV_SDMMC_PLIB_READ_RESPONSE                 sdhostReadResponse;
    DRV_SDMMC_PLIB_SET_BLOCK_COUNT               sdhostSetBlockCount;
    DRV_SDMMC_PLIB_SET_BLOCK_SIZE                sdhostSetBlockSize;
    DRV_SDMMC_PLIB_SET_BUS_WIDTH                 sdhostSetBusWidth;
    DRV_SDMMC_PLIB_SET_SPEED_MODE                sdhostSetSpeedMode;
    DRV_SDMMC_PLIB_SETUP_DMA                     sdhostSetupDma;
    DRV_SDMMC_PLIB_IS_CARD_ATTACHED              sdhostIsCardAttached;
    DRV_SDMMC_PLIB_IS_WRITE_PROTECTED            sdhostIsWriteProtected;
    DRV_SDMMC_PLIB_GET_COMMAND_ERROR             sdhostGetCommandError;
    DRV_SDMMC_PLIB_GET_DATA_ERROR                sdhostGetDataError;
} DRV_SDMMC_PLIB_API;

// *****************************************************************************
/* SDMMC Driver Initialization Data

  Summary:
    Defines the data required to initialize the SDMMC driver

  Description:
    This data type defines the data required to initialize or the SDMMC driver.

  Remarks:
    None.
*/

typedef struct _DRV_SDMMC_INIT
{
    /* PLIB API set used by the driver to access the peripheral */
    const DRV_SDMMC_PLIB_API*    sdmmcPlib;

    /* Memory Pool for Client Objects */
    uintptr_t                   clientObjPool;

    /* Number of clients */
    uint32_t                    numClients;

    /* Size of buffer objects queue */
    uint32_t                    bufferObjPoolSize;

    /* Pointer to the buffer pool */
    uintptr_t                   bufferObjPool;

    /* SDMMC Protocol */
    DRV_SDMMC_PROTOCOL       	  protocol;

    /* Rate at which card insertion/removal is checked if polling method is selected */
    uint32_t                    cardDetectionPollingIntervalMs;

    /* Card detection method */
    DRV_SDMMC_CD_METHOD         cardDetectionMethod;

    /* Whether SD Card detection is enabled or not */
    bool                        isCardDetectEnabled;

    /* Whether SD Card write protection check is enabled or not */
    bool                        isWriteProtectCheckEnabled;

    /* Selects between default and high speed modes */
    DRV_SDMMC_SPEED_MODE         speedMode;

    /* Selects between 1-bit and 4-bit bus width */
    DRV_SDMMC_BUS_WIDTH          busWidth;

    /* Whether the driver should register its services with the file system */
    bool                        isFsEnabled;
} DRV_SDMMC_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif      // #ifndef _DRV_SDMMC_DEFINITIONS_H