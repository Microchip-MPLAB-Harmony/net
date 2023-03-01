<#--
/*******************************************************************************
  HTTP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    http.h.ftl

  Summary:
    HTTP Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
-->

<#if TCPIP_STACK_USE_HTTP_SERVER == true>
/*** HTTP Configuration ***/
#define TCPIP_STACK_USE_HTTP_SERVER
#define TCPIP_HTTP_MAX_HEADER_LEN		    		${TCPIP_HTTP_MAX_HEADER_LEN}
#define TCPIP_HTTP_CACHE_LEN		        		"${TCPIP_HTTP_CACHE_LEN?c}"
#define TCPIP_HTTP_TIMEOUT		            		${TCPIP_HTTP_TIMEOUT}
#define TCPIP_HTTP_MAX_CONNECTIONS		    		${TCPIP_HTTP_MAX_CONNECTIONS}
#define TCPIP_HTTP_DEFAULT_FILE		        		"${TCPIP_HTTP_DEFAULT_FILE}"
#define TCPIP_HTTP_DEFAULT_LEN		        		${TCPIP_HTTP_DEFAULT_LEN}
#define TCPIP_HTTP_MAX_DATA_LEN		        		${TCPIP_HTTP_MAX_DATA_LEN}
#define TCPIP_HTTP_MIN_CALLBACK_FREE				${TCPIP_HTTP_MIN_CALLBACK_FREE}
#define TCPIP_HTTP_SKT_TX_BUFF_SIZE		    		${TCPIP_HTTP_SKT_TX_BUFF_SIZE}
#define TCPIP_HTTP_SKT_RX_BUFF_SIZE		    		${TCPIP_HTTP_SKT_RX_BUFF_SIZE}
#define TCPIP_HTTP_CONFIG_FLAGS		        		${TCPIP_HTTP_CONFIG_FLAGS}
#define TCPIP_HTTP_WEB_DIR		        		    "${TCPIP_HTTP_WEB_DIR}"
<#if TCPIP_HTTP_FILE_UPLOAD_ENABLE == true>
#define TCPIP_HTTP_FILE_UPLOAD_ENABLE
#define TCPIP_HTTP_FILE_UPLOAD_NAME					"${TCPIP_HTTP_FILE_UPLOAD_NAME}"
#define MPFS_UPLOAD_MOUNT_PATH						"${TCPIP_HTTP_MPFS_NVM_PATH}"
#define MPFS_UPLOAD_NVM_VOL							"${TCPIP_HTTP_MPFS_NVM_VOL}"
#define MPFS_UPLOAD_DISK_NO							${TCPIP_HTTP_MPFS_NVM_NUM}
</#if>
<#if TCPIP_HTTP_USE_POST == true>
#define TCPIP_HTTP_USE_POST
</#if>
<#if TCPIP_HTTP_USE_COOKIES == true>
#define TCPIP_HTTP_USE_COOKIES
</#if>
<#if TCPIP_HTTP_USE_BASE64_DECODE == true>
#define TCPIP_HTTP_USE_BASE64_DECODE
</#if>
<#if TCPIP_HTTP_USE_AUTHENTICATION == true>
#define TCPIP_HTTP_USE_AUTHENTICATION
</#if>
<#if TCPIP_HTTP_NO_AUTH_WITHOUT_SSL == true>
#define TCPIP_HTTP_NO_AUTH_WITHOUT_SSL
</#if>
#define TCPIP_HTTP_TASK_RATE					${TCPIP_HTTP_TASK_RATE}
<#if TCPIP_HTTP_MALLOC_FUNC?has_content>
#define TCPIP_HTTP_MALLOC_FUNC                     ${TCPIP_HTTP_MALLOC_FUNC}
<#else>
#define TCPIP_HTTP_MALLOC_FUNC                     0
</#if>
<#if TCPIP_HTTP_FREE_FUNC?has_content>
#define TCPIP_HTTP_FREE_FUNC                     ${TCPIP_HTTP_FREE_FUNC}
<#else>
#define TCPIP_HTTP_FREE_FUNC                        0
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
