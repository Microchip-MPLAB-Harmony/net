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
<#if CONFIG_TCPIP_STACK_USE_HTTP_SERVER == true>
/*** HTTP Configuration ***/
#define TCPIP_STACK_USE_HTTP_SERVER
#define TCPIP_HTTP_MAX_HEADER_LEN		    		${CONFIG_TCPIP_HTTP_MAX_HEADER_LEN}
#define TCPIP_HTTP_CACHE_LEN		        		"${CONFIG_TCPIP_HTTP_CACHE_LEN}"
#define TCPIP_HTTP_TIMEOUT		            		${CONFIG_TCPIP_HTTP_TIMEOUT}
#define TCPIP_HTTP_MAX_CONNECTIONS		    		${CONFIG_TCPIP_HTTP_MAX_CONNECTIONS}
#define TCPIP_HTTP_DEFAULT_FILE		        		"${CONFIG_TCPIP_HTTP_DEFAULT_FILE}"
#define TCPIP_HTTPS_DEFAULT_FILE	        		"${CONFIG_TCPIP_HTTPS_DEFAULT_FILE}"
#define TCPIP_HTTP_DEFAULT_LEN		        		${CONFIG_TCPIP_HTTP_DEFAULT_LEN}
#define TCPIP_HTTP_MAX_DATA_LEN		        		${CONFIG_TCPIP_HTTP_MAX_DATA_LEN}
#define TCPIP_HTTP_MIN_CALLBACK_FREE				${CONFIG_TCPIP_HTTP_MIN_CALLBACK_FREE}
#define TCPIP_HTTP_SKT_TX_BUFF_SIZE		    		${CONFIG_TCPIP_HTTP_SKT_TX_BUFF_SIZE}
#define TCPIP_HTTP_SKT_RX_BUFF_SIZE		    		${CONFIG_TCPIP_HTTP_SKT_RX_BUFF_SIZE}
#define TCPIP_HTTP_CONFIG_FLAGS		        		${CONFIG_TCPIP_HTTP_CONFIG_FLAGS}
<#if CONFIG_TCPIP_HTTP_FILE_UPLOAD_ENABLE == true>
#define TCPIP_HTTP_FILE_UPLOAD_ENABLE
#define TCPIP_HTTP_FILE_UPLOAD_NAME				"${CONFIG_TCPIP_HTTP_FILE_UPLOAD_NAME}"
</#if>
<#if CONFIG_TCPIP_HTTP_USE_POST == true>
#define TCPIP_HTTP_USE_POST
</#if>
<#if CONFIG_TCPIP_HTTP_USE_COOKIES == true>
#define TCPIP_HTTP_USE_COOKIES
</#if>
<#if CONFIG_TCPIP_HTTP_USE_BASE64_DECODE == true>
#define TCPIP_HTTP_USE_BASE64_DECODE
</#if>
<#if CONFIG_TCPIP_HTTP_USE_AUTHENTICATION == true>
#define TCPIP_HTTP_USE_AUTHENTICATION
</#if>
<#if CONFIG_TCPIP_HTTP_NO_AUTH_WITHOUT_SSL == true>
#define TCPIP_HTTP_NO_AUTH_WITHOUT_SSL
</#if>
#define TCPIP_HTTP_TASK_RATE					${CONFIG_TCPIP_HTTP_TASK_RATE}
<#if CONFIG_TCPIP_HTTP_MALLOC_FUNC?has_content>
#define TCPIP_HTTP_MALLOC_FUNC                     ${CONFIG_TCPIP_HTTP_MALLOC_FUNC}
<#else>
#define TCPIP_HTTP_MALLOC_FUNC                     0
</#if>
<#if CONFIG_TCPIP_HTTP_FREE_FUNC?has_content>
#define TCPIP_HTTP_FREE_FUNC                     ${CONFIG_TCPIP_HTTP_FREE_FUNC}
<#else>
#define TCPIP_HTTP_FREE_FUNC                        0
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
