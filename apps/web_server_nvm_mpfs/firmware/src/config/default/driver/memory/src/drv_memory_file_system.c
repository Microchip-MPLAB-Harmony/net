/******************************************************************************
  MEMORY Driver File System Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_memory_file_system.c

  Summary:
    MEMORY Driver Interface Definition

  Description:
    The MEMORY Driver provides a interface to access the MEMORY on the PIC32
    microcontroller. This file implements the MEMORY Driver file system interface.
    This file should be included in the project if MEMORY driver functionality with
    File system is needed.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/memory/src/drv_memory_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

/* FS Function registration table. */

const SYS_FS_MEDIA_FUNCTIONS memoryMediaFunctions =
{
    .mediaStatusGet     = DRV_MEMORY_IsAttached,
    .mediaGeometryGet   = DRV_MEMORY_GeometryGet,
    .sectorRead         = DRV_MEMORY_AsyncRead,
    .sectorWrite        = DRV_MEMORY_AsyncEraseWrite,
    .eventHandlerset    = DRV_MEMORY_TransferHandlerSet,
    .commandStatusGet   = (void *)DRV_MEMORY_CommandStatusGet,
    .Read               = DRV_MEMORY_AsyncRead,
    .erase              = DRV_MEMORY_AsyncErase,
    .addressGet         = DRV_MEMORY_AddressGet,
    .open               = DRV_MEMORY_Open,
    .close              = DRV_MEMORY_Close,
    .tasks              = DRV_MEMORY_Tasks,
};

// *****************************************************************************
// *****************************************************************************
// Section: MEMORY Driver File system interface Routines
// *****************************************************************************
// *****************************************************************************

void DRV_MEMORY_RegisterWithSysFs( const SYS_MODULE_INDEX drvIndex, uint8_t mediaType)
{
    SYS_FS_MEDIA_MANAGER_Register
    (
        (SYS_MODULE_OBJ)drvIndex,
        (SYS_MODULE_INDEX)drvIndex,
        &memoryMediaFunctions,
        (SYS_FS_MEDIA_TYPE)mediaType
    );
}
