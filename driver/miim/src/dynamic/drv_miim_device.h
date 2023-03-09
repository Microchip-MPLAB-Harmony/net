/*******************************************************************************
  MIIM Driver Device API

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_device.h

  Summary:
    MIIM driver device API for different internal MACs.

  Description:
    This file contains the API for MIIM platform specific driver operations.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2018-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

//DOM-IGNORE-END

#ifndef _DRV_MIIM_DEVICE_H
#define _DRV_MIIM_DEVICE_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>


// sets up the device: enable the device, sets the clock and other settings
DRV_MIIM_RESULT DRV_MIIM_DeviceSetup(uintptr_t miimId, const DRV_MIIM_SETUP* pSetUp);

// enables the MIIM port
void DRV_MIIM_PortEnable(uintptr_t miimId);

// disables the MIIM port
void DRV_MIIM_PortDisable(uintptr_t miimId);
    
// returns true if the device is busy with an ongoing operation
// false otherwise
bool DRV_MIIM_PortBusy(uintptr_t miimId);

// writes wData using the supplied address and register
void DRV_MIIM_WriteData(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx, uint16_t wData);

// initiates a data read using the supplied address and register    
void DRV_MIIM_ReadStart(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx);

// returns the MIIM read data
uint16_t DRV_MIIM_ReadDataGet(uintptr_t miimId);
    
// enables the scan
// should return:
//      DRV_MIIM_TXFER_ERROR if scan not supported
//      DRV_MIIM_TXFER_SCAN_STALE if data is stale
//      DRV_MIIM_TXFER_SCAN_VALID if data is valid
DRV_MIIM_TXFER_STAT DRV_MIIM_ScanEnable(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx);

// disables the scan
void DRV_MIIM_ScanDisable(uintptr_t miimId);
    
// returns true if scan data is available
//  if so, the data is saved at the scanData address (if not NULL)
//  and the hw 'data valid' indicator (if exists) is cleared, for the next scan
//
bool DRV_MIIM_GetScanData(uintptr_t miimId, uint32_t* scanData);
    

#endif //#ifndef _DRV_MIIM_DEVICE_H

/*******************************************************************************
 End of File
*/

