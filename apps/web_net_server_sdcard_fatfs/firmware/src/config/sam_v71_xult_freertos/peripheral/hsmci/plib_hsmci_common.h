/*******************************************************************************
  HSMCI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_hsmci_common.h

  Summary:
    Common HSMCI PLIB Header File

  Description:
    None

*******************************************************************************/

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

#ifndef PLIB_HSMCI_COMMON_H
#define PLIB_HSMCI_COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

#include  "device.h"

#define HSMCI_CLOCK_FREQ_400_KHZ           (400000)
#define HSMCI_CLOCK_FREQ_DS_25_MHZ         (25000000)
#define HSMCI_CLOCK_FREQ_HS_50_MHZ         (50000000)
#define HSMCI_CLOCK_FREQ_HS_26_MHZ         (26000000)
#define HSMCI_CLOCK_FREQ_HS_52_MHZ         (52000000)

typedef enum
{
    HSMCI_DATA_TRANSFER_TYPE_SINGLE = 0,
    HSMCI_DATA_TRANSFER_TYPE_MULTI,

}HSMCI_DATA_TRANSFER_TYPE;

typedef enum
{
    HSMCI_DATA_TRANSFER_DIR_WRITE = 0,
    HSMCI_DATA_TRANSFER_DIR_READ

}HSMCI_DATA_TRANSFER_DIR;

typedef struct
{
    bool                              isDataPresent;
    HSMCI_DATA_TRANSFER_DIR           transferDir;
    HSMCI_DATA_TRANSFER_TYPE          transferType;

}HSMCI_DataTransferFlags;

typedef enum
{
    HSMCI_CMD_TIMEOUT_ERROR         = 0x0001,
    HSMCI_CMD_CRC_ERROR             = 0x0002,
    HSMCI_CMD_END_BIT_ERROR         = 0x0004,
    HSMCI_CMD_INDEX_ERROR           = 0x0008,
    HSMCI_DATA_TIMEOUT_ERROR        = 0x0010,
    HSMCI_DATA_CRC_ERROR            = 0x0020,
    HSMCI_DATA_UNDERRUN_ERROR       = 0x0040,
    HSMCI_DATA_OVERRUN_ERROR        = 0x0080,

}HSMCI_ERROR_FLAGS;

typedef enum
{
    HSMCI_BUS_WIDTH_1_BIT = 0,
    HSMCI_BUS_WIDTH_4_BIT

} HSMCI_BUS_WIDTH;

typedef enum
{
    HSMCI_SPEED_MODE_NORMAL = 0,
    HSMCI_SPEED_MODE_HIGH

} HSMCI_SPEED_MODE;

typedef enum
{
    HSMCI_READ_RESP_REG_0 = 0,
    HSMCI_READ_RESP_REG_1,
    HSMCI_READ_RESP_REG_2,
    HSMCI_READ_RESP_REG_3,
    HSMCI_READ_RESP_REG_ALL

} HSMCI_READ_RESPONSE_REG;

typedef enum
{
    HSMCI_CMD_RESP_NONE,   /*!< no response type */
    HSMCI_CMD_RESP_R1,     /*!< normal response command */
    HSMCI_CMD_RESP_R1B,    /*!< normal with busy signal */
    HSMCI_CMD_RESP_R2,     /*!< CID, CSD register */
    HSMCI_CMD_RESP_R3,     /*!< OCR register */
    HSMCI_CMD_RESP_R4,     /*!< */
    HSMCI_CMD_RESP_R5,     /*!< */
    HSMCI_CMD_RESP_R6,     /*!< Published RCA response  */
    HSMCI_CMD_RESP_R7      /*!< Card interface condition  */

} HSMCI_CMD_RESP_TYPE;

typedef enum
{
    HSMCI_XFER_STATUS_CMD_COMPLETED  = 0x01,
    HSMCI_XFER_STATUS_DATA_COMPLETED = 0x02

}HSMCI_XFER_STATUS;

typedef void (*HSMCI_CALLBACK) (HSMCI_XFER_STATUS xferStatus, uintptr_t context);

typedef struct
{
    bool                    isCmdInProgress;
    bool                    isDataInProgress;
    HSMCI_ERROR_FLAGS       errorStatus;
    uint32_t                blockSize;
    uint32_t                nBlocks;
    HSMCI_CALLBACK          callback;
    uintptr_t               context;
} HSMCI_OBJECT;

#define HSMCI_CMD_ERROR         (HSMCI_SR_RTOE_Msk | HSMCI_SR_RENDE_Msk | HSMCI_SR_RDIRE_Msk | HSMCI_SR_RINDE_Msk | HSMCI_SR_RCRCE_Msk)
#define HSMCI_DATA_ERROR        (HSMCI_SR_UNRE_Msk | HSMCI_SR_OVRE_Msk | HSMCI_SR_DTOE_Msk | HSMCI_SR_DCRCE_Msk)

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_HSMCI_COMMON_H
