/******************************************************************************
  SDHC Driver File System Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdhc_file_system.c

  Summary:
    SDHC Driver Interface Definition

  Description:
    The SDHC Driver provides a interface to access the SDHC on the PIC32
    microcontroller. This file implements the SDHC Driver file system interface.
    This file should be included in the project if SDHC driver functionality with
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

#include "driver/sdhc/drv_sdhc.h"
#include "system/fs/sys_fs_media_manager.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

/* FS Function registration table. */

const SYS_FS_MEDIA_FUNCTIONS sdhcMediaFunctions =
{
    .mediaStatusGet     = DRV_SDHC_IsAttached,
    .mediaGeometryGet   = DRV_SDHC_GeometryGet,
    .sectorRead         = DRV_SDHC_AsyncRead,
    .sectorWrite        = DRV_SDHC_AsyncWrite,
    .eventHandlerset    = DRV_SDHC_EventHandlerSet,
    .commandStatusGet   = (void *)DRV_SDHC_CommandStatusGet,
    .Read               = DRV_SDHC_AsyncRead,
    .erase              = NULL,
    .addressGet         = NULL,
    .open               = DRV_SDHC_Open,
    .close              = DRV_SDHC_Close,
    .tasks              = DRV_SDHC_Tasks,
};

// *****************************************************************************
// *****************************************************************************
// Section: SDHC Driver File system interface Routines
// *****************************************************************************
// *****************************************************************************

void DRV_SDHC_RegisterWithSysFs( const SYS_MODULE_INDEX drvIndex)
{
    SYS_FS_MEDIA_MANAGER_Register
    (
        (SYS_MODULE_OBJ)drvIndex,
        (SYS_MODULE_INDEX)drvIndex,
        &sdhcMediaFunctions,
        SYS_FS_MEDIA_TYPE_SD_CARD
    );
}
