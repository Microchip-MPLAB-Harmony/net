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
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
----------------------------------------------------------------------------->

<#if TCPIP_STACK_USE_FS_WRAPPER == true>
/*** TCPIP SYS FS Wrapper ***/
#define SYS_FS_MAX_PATH						${TCPIP_SYS_FS_MAX_PATH}
#define LOCAL_WEBSITE_PATH_FS				"${TCPIP_LOCAL_WEBSITE_PATH}"
#define LOCAL_WEBSITE_PATH					"${TCPIP_LOCAL_WEBSITE_PATH}/"
#define SYS_FS_DRIVE						"${TCPIP_SYS_FS_DRIVE}"
<#if TCPIP_SYS_FS_DRIVE == "FLASH">
#define SYS_FS_NVM_VOL						"${TCPIP_SYS_FS_NVM_VOL}"
<#elseif TCPIP_SYS_FS_DRIVE == "SDCARD">
#define SYS_FS_SD_VOL						"${TCPIP_SYS_FS_SD_VOL}"
<#else>
#define SYS_FS_NVM_VOL						"${TCPIP_SYS_FS_NVM_VOL}"
#define SYS_FS_SD_VOL						"${TCPIP_SYS_FS_SD_VOL}"
</#if>
#define SYS_FS_FATFS_STRING					"${TCPIP_SYS_FS_FATFS_STRING}"
#define SYS_FS_MPFS_STRING					"${TCPIP_SYS_FS_MPFS_STRING}"
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
