<#--
/*******************************************************************************
  HTTP File System Wrapper Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_fs_wrapper.h.ftl

  Summary:
    HTTP File System Wrapper Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->
<#if CONFIG_TCPIP_STACK_USE_FS_WRAPPER == true>
/*** TCPIP SYS FS Wrapper ***/
#define SYS_FS_MAX_PATH						${CONFIG_TCPIP_SYS_FS_MAX_PATH}
#define LOCAL_WEBSITE_PATH_FS				"${CONFIG_TCPIP_LOCAL_WEBSITE_PATH}"
#define LOCAL_WEBSITE_PATH					"${CONFIG_TCPIP_LOCAL_WEBSITE_PATH}/"
#define SYS_FS_DRIVE						"${CONFIG_TCPIP_SYS_FS_DRIVE}"
<#if CONFIG_TCPIP_SYS_FS_DRIVE == "FLASH">
#define SYS_FS_NVM_VOL						"${CONFIG_TCPIP_SYS_FS_NVM_VOL}"
<#elseif CONFIG_TCPIP_SYS_FS_DRIVE == "SDCARD">
#define SYS_FS_SD_VOL						"${CONFIG_TCPIP_SYS_FS_SD_VOL}"
<#else>
#define SYS_FS_NVM_VOL						"${CONFIG_TCPIP_SYS_FS_NVM_VOL}"
#define SYS_FS_SD_VOL						"${CONFIG_TCPIP_SYS_FS_SD_VOL}"
</#if>
#define SYS_FS_FATFS_STRING					"${CONFIG_TCPIP_SYS_FS_FATFS_STRING}"
#define SYS_FS_MPFS_STRING					"${CONFIG_TCPIP_SYS_FS_MPFS_STRING}"
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
