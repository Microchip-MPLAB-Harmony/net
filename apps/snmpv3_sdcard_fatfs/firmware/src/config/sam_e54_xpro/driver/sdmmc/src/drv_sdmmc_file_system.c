/******************************************************************************
  SDMMC Driver File System Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdmmc_file_system.c

  Summary:
    SDMMC Driver File System Interface Implementation

  Description:
    This file registers the SDMMC Driver capabilities with the file system
    interface.
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

#include "driver/sdmmc/drv_sdmmc.h"
#include "system/fs/sys_fs_media_manager.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

/* FS Function registration table. */
typedef SYS_FS_MEDIA_COMMAND_STATUS (* CommandStatusGetType)( DRV_HANDLE, SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE );

const SYS_FS_MEDIA_FUNCTIONS sdmmcMediaFunctions =
{
    .mediaStatusGet     = DRV_SDMMC_IsAttached,
    .mediaGeometryGet   = DRV_SDMMC_GeometryGet,
    .sectorRead         = DRV_SDMMC_AsyncRead,
    .sectorWrite        = DRV_SDMMC_AsyncWrite,
    .eventHandlerset    = DRV_SDMMC_EventHandlerSet,
    .commandStatusGet   = (CommandStatusGetType)DRV_SDMMC_CommandStatus,
    .open               = DRV_SDMMC_Open,
    .close              = DRV_SDMMC_Close,
    .tasks              = DRV_SDMMC_Tasks
};

// *****************************************************************************
// *****************************************************************************
// Section: SDMMC Driver File system interface Routines
// *****************************************************************************
// *****************************************************************************

void DRV_SDMMC_RegisterWithSysFs( const SYS_MODULE_INDEX drvIndex)
{
    SYS_FS_MEDIA_MANAGER_Register
    (
        (SYS_MODULE_OBJ)drvIndex,
        (SYS_MODULE_INDEX)drvIndex,
        &sdmmcMediaFunctions,
        SYS_FS_MEDIA_TYPE_SD_CARD
    );
}
