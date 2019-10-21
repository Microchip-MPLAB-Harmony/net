/******************************************************************************
  SD Card (SPI) Driver File System Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdspi_file_system.c

  Summary:
    SD Card (SPI) Driver Interface Definition

  Description:
    The SD Card Driver provides a interface to access the SD Card. This file
    implements the SD Card Driver file system interface.
    This file should be included in the project if SD Card driver functionality
    with File system is needed.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/sdspi/drv_sdspi.h"
#include "system/fs/sys_fs_media_manager.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

/* FS Function registration table. */
typedef SYS_FS_MEDIA_COMMAND_STATUS (* CommandStatusGetType)( DRV_HANDLE, SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE );    

const SYS_FS_MEDIA_FUNCTIONS sdspiMediaFunctions =
{
    .mediaStatusGet     = DRV_SDSPI_IsAttached,
    .mediaGeometryGet   = DRV_SDSPI_GeometryGet,
    .sectorRead         = DRV_SDSPI_Read,
    .sectorWrite        = DRV_SDSPI_Write,
    .eventHandlerset    = DRV_SDSPI_EventHandlerSet,
    .commandStatusGet   = (CommandStatusGetType) DRV_SDSPI_CommandStatusGet,
    .Read               = DRV_SDSPI_Read,
    .erase              = NULL,
    .addressGet         = NULL,
    .open               = DRV_SDSPI_Open,
    .close              = DRV_SDSPI_Close,
    .tasks              = NULL,
};

// *****************************************************************************
// *****************************************************************************
// Section: MEMORY Driver File system interface Routines
// *****************************************************************************
// *****************************************************************************

void DRV_SDSPI_RegisterWithSysFs( const SYS_MODULE_INDEX drvIndex)
{
    SYS_FS_MEDIA_MANAGER_Register
    (
        (SYS_MODULE_OBJ)drvIndex,
        (SYS_MODULE_INDEX)drvIndex,
        &sdspiMediaFunctions,
        (SYS_FS_MEDIA_TYPE)SYS_FS_MEDIA_TYPE_SD_CARD
    );
}
