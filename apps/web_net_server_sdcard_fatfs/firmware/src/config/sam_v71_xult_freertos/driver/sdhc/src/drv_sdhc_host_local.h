/*******************************************************************************
  SD Host Controller Driver Interface

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdhc_host_local.h

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

#ifndef _DRV_SDHC_HOST_LOCAL_H
#define _DRV_SDHC_HOST_LOCAL_H

#include  "device.h"

#define SDHOST_SCR_REG_LEN (8)
#define SDHOST_CID_REG_LEN (16)
#define SDHOST_OCR_REG_LEN (4)
#define SDHOST_CSD_REG_LEN (16)
#define SDHOST_SWITCH_STATUS_REG_LEN (64)
#define SDHOST_ERROR_INTERRUPT (HSMCI_SR_CSTOE_Msk | HSMCI_SR_RTOE_Msk | HSMCI_SR_RENDE_Msk | HSMCI_SR_RDIRE_Msk | HSMCI_SR_RINDE_Msk)
#define SDHOST_DATA_ERROR       (HSMCI_SR_UNRE_Msk | HSMCI_SR_OVRE_Msk | HSMCI_SR_DTOE_Msk | HSMCI_SR_DCRCE_Msk)
#define OCR_VDD_27_28          (1lu << 15)
#define OCR_VDD_28_29          (1lu << 16)
#define OCR_VDD_29_30          (1lu << 17)
#define OCR_VDD_30_31          (1lu << 18)
#define OCR_VDD_31_32          (1lu << 19)
#define OCR_VDD_32_33          (1lu << 20)
#define SDMMC_RESP_PRESENT      (1lu << 8)
//! 8 bit response (SPI only)
#define SDMMC_RESP_8            (1lu << 9)
//! 32 bit response (SPI only)
#define SDMMC_RESP_32           (1lu << 10)
//! 136 bit response (MCI only)
#define SDMMC_RESP_136          (1lu << 11)
//! Expect valid crc (MCI only)
#define SDMMC_RESP_CRC          (1lu << 12)
//! Card may send busy
#define SDMMC_RESP_BUSY         (1lu << 13)
// Open drain for a braodcast command (bc)
// or to enter in inactive state (MCI only)
#define SDMMC_CMD_OPENDRAIN     (1lu << 14)
//! To signal a data write operation
#define SDMMC_CMD_WRITE         (1lu << 15)
//! To signal a data transfer in stream mode
#define SDMMC_CMD_STREAM        (1lu << 18)
//! To signal a data transfer in single block mode
#define SDMMC_CMD_SINGLE_BLOCK  (1lu << 19)
//! To signal a data transfer in multi block mode
#define SDMMC_CMD_MULTI_BLOCK   (1lu << 20)
//! @}
#define SD_MMC_VOLTAGE_SUPPORT \
(OCR_VDD_27_28 | OCR_VDD_28_29 | \
OCR_VDD_29_30 | OCR_VDD_30_31 | \
OCR_VDD_31_32 | OCR_VDD_32_33)
//! \name Set of flags to define a reponse type
//! @{
#define SDMMC_CMD_NO_RESP (0)
#define SDMMC_CMD_R1      (SDMMC_RESP_PRESENT | SDMMC_RESP_CRC)
#define SDMMC_CMD_R1B     (SDMMC_RESP_PRESENT | SDMMC_RESP_CRC | SDMMC_RESP_BUSY)
#define SDMMC_CMD_R2      (SDMMC_RESP_PRESENT | SDMMC_RESP_8 | SDMMC_RESP_136 | SDMMC_RESP_CRC)
#define SDMMC_CMD_R3      (SDMMC_RESP_PRESENT | SDMMC_RESP_32)
#define SDMMC_CMD_R4      (SDMMC_RESP_PRESENT | SDMMC_RESP_32)
#define SDMMC_CMD_R5      (SDMMC_RESP_PRESENT | SDMMC_RESP_8 | SDMMC_RESP_CRC)
#define SDMMC_CMD_R6      (SDMMC_RESP_PRESENT | SDMMC_RESP_CRC)
#define SDMMC_CMD_R7      (SDMMC_RESP_PRESENT | SDMMC_RESP_32 | SDMMC_RESP_CRC)

#endif