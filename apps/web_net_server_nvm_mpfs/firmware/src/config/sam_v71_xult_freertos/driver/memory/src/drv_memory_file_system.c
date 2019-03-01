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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/memory/src/drv_memory_local.h"
#include "system/fs/sys_fs_media_manager.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

/* FS Function registration table. */
typedef SYS_FS_MEDIA_COMMAND_STATUS (* CommandStatusGetType)( DRV_HANDLE, SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE );

const SYS_FS_MEDIA_FUNCTIONS memoryMediaFunctions =
{
    .mediaStatusGet     = DRV_MEMORY_IsAttached,
    .mediaGeometryGet   = DRV_MEMORY_GeometryGet,
    .sectorRead         = DRV_MEMORY_Read,
    .sectorWrite        = DRV_MEMORY_EraseWrite,
    .eventHandlerset    = DRV_MEMORY_TransferHandlerSet,
    .commandStatusGet   = (CommandStatusGetType)DRV_MEMORY_CommandStatusGet,
    .Read               = DRV_MEMORY_Read,
    .erase              = DRV_MEMORY_Erase,
    .addressGet         = DRV_MEMORY_AddressGet,
    .open               = DRV_MEMORY_Open,
    .close              = DRV_MEMORY_Close,
    .tasks              = NULL,
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
