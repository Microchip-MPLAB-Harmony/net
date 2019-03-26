/*******************************************************************************
  MPU PLIB Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_mpu_local.h

  Summary:
    MPU PLIB Header File

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

#ifndef PLIB_MPU_LOCAL_H
#define PLIB_MPU_LOCAL_H

#include "device.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Local: **** Do Not Use ****
// *****************************************************************************
// *****

/* Region Address and Index */
#define MPU_RASR_SIZE(value) ((MPU_RASR_SIZE_Msk & ((value) << MPU_RASR_SIZE_Pos)))
#define MPU_RBAR_REGION(value) ((MPU_RBAR_REGION_Msk & ((value) << MPU_RBAR_REGION_Pos)))
#define MPU_RASR_SRD(value) ((MPU_RASR_SRD_Msk & ((value) << MPU_RASR_SRD_Pos)))
#define MPU_REGION(region, addr) (((addr) & MPU_RBAR_ADDR_Msk) | MPU_RBAR_REGION(region) | MPU_RBAR_VALID_Msk)

/* Region Sizes: region size is 2^(value+1)*/
#define MPU_REGION_SIZE(value) (MPU_RASR_SIZE(value))

/* Bitfield to disable some subregions
 * (1 bit for each 1/8 of region, region must be bigger than 128 bytes) */
#define MPU_SUBREGION_DISABLE(value) (MPU_RASR_SRD(value))

/* --- Access Privilege constants --- */
#define   MPU_RASR_AP_NOACCESS_Val                    (0x0U)    /* Access Privilege: no access for all */
#define   MPU_RASR_AP_NOACCESS_PRIV_READWRITE_Val     (0x1U)    /* Access Privilege: no access for unprivileged, read/write for privileged */
#define   MPU_RASR_AP_READONLY_PRIV_READWRITE_Val     (0x2U)    /* Access Privilege: readonly for unprivileged, read/write for privileged */
#define   MPU_RASR_AP_READWRITE_Val                   (0x3U)    /* Access Privilege: read/write for all */
#define   MPU_RASR_AP_NOACCESS_PRIV_READONLY_Val      (0x5U)    /* Access Privilege: no access for unprivileged, readonly for privileged */
#define   MPU_RASR_AP_READONLY_Val                    (0x7U)    /* Access Privilege: readonly for all */

#define MPU_RASR_AP(value)                 (MPU_RASR_AP_Msk & ((value) << MPU_RASR_AP_Pos))

/* --- Memory Types Attributes --- */
#define MPU_RASR_TEX(value)                (MPU_RASR_TEX_Msk & ((value) << MPU_RASR_TEX_Pos))

#define MPU_ATTR_STRONGLY_ORDERED (MPU_RASR_TEX(0))                                      /* Strongly-Ordered Shareable */
#define MPU_ATTR_DEVICE           (MPU_RASR_TEX(0) | MPU_RASR_B_Msk)                     /* Device Shareable */
#define MPU_ATTR_NORMAL_WT        (MPU_RASR_TEX(0) | MPU_RASR_C_Msk)                     /* Normal, Write-Through Read Allocate */
#define MPU_ATTR_NORMAL_WB        (MPU_RASR_TEX(0) | MPU_RASR_C_Msk | MPU_RASR_B_Msk)    /* Normal, Write-Back Read Allocate */
#define MPU_ATTR_NORMAL_WB_WA     (MPU_RASR_TEX(1) | MPU_RASR_C_Msk | MPU_RASR_B_Msk)    /* Normal, Write-Back Read/Write Allocate */
#define MPU_ATTR_NORMAL           (MPU_RASR_TEX(1))                                      /* Normal, Non-cacheable */

/* Other Attributes */
#define MPU_ATTR_SHAREABLE     (MPU_RASR_S_Msk)
#define MPU_ATTR_EXECUTE_NEVER (MPU_RASR_XN_Msk)
#define MPU_ATTR_ENABLE        (MPU_RASR_ENABLE_Msk)



// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_MPU_LOCAL_H


