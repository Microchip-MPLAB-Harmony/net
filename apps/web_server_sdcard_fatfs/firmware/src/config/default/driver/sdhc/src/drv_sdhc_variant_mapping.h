/*******************************************************************************
  SD Card Driver Feature Variant Implementations

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdcard_variant_mapping.h

  Summary:
    SDHC Driver Feature Variant Implementations

  Description:
    This file implements the functions which differ based on different parts
    and various implementations of the same feature.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_SDHC_VARIANT_MAPPING_H
#define _DRV_SDHC_VARIANT_MAPPING_H

// *****************************************************************************
// *****************************************************************************
// Section: Feature Variant Mapping
// *****************************************************************************
// *****************************************************************************
/* Some variants are determined by hardware feature existence, some features
   are determined user configuration of the driver, and some variants are
   combination of the two.
*/

#if defined (DRV_SDHC_SYS_FS_REGISTER)
#define DRV_SDHC_RegisterWithSysFs(x, y, z) SYS_FS_MEDIA_MANAGER_Register((SYS_MODULE_OBJ)x, (SYS_MODULE_INDEX)y, &z, SYS_FS_MEDIA_TYPE_SD_CARD)
#else
#define DRV_SDHC_RegisterWithSysFs(x, y, z)
#endif

#endif //_DRV_SDHC_VARIANT_MAPPING_H

/*******************************************************************************
 End of File
*/

