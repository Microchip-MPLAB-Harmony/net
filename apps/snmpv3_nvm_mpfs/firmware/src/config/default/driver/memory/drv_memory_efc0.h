/*******************************************************************************
  Memory Driver EFC0 Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_memory_efc0.h

  Summary:
    Memory Driver EFC0 Interface Definition

  Description:
    The Memory Driver provides a interface to access the EFC0 peripheral on the
    microcontroller.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2016 - 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_MEMORY_EFC0_H
#define _DRV_MEMORY_EFC0_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include "configuration.h"
#include "drv_memory_definitions.h"
#include "peripheral/efc/plib_efc0.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif

// DOM-IGNORE-END

DRV_HANDLE DRV_EFC0_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

void DRV_EFC0_Close( const DRV_HANDLE handle );

SYS_STATUS DRV_EFC0_Status( const SYS_MODULE_INDEX drvIndex );

bool DRV_EFC0_SectorErase( const DRV_HANDLE handle, uint32_t address );

bool DRV_EFC0_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

bool DRV_EFC0_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address );

MEMORY_DEVICE_TRANSFER_STATUS DRV_EFC0_TransferStatusGet( const DRV_HANDLE handle );

bool DRV_EFC0_GeometryGet( const DRV_HANDLE handle, MEMORY_DEVICE_GEOMETRY *geometry );

#ifdef __cplusplus
}
#endif

#endif // #ifndef _DRV_MEMORY_EFC0_H
/*******************************************************************************
 End of File
*/