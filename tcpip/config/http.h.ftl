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

<#if TCPIP_STACK_USE_HTTP_SERVER == true>
/*** HTTP Configuration ***/
#define TCPIP_STACK_USE_HTTP_SERVER
#define TCPIP_HTTP_MAX_HEADER_LEN		    		${TCPIP_HTTP_MAX_HEADER_LEN}
#define TCPIP_HTTP_CACHE_LEN		        		"${TCPIP_HTTP_CACHE_LEN}"
#define TCPIP_HTTP_TIMEOUT		            		${TCPIP_HTTP_TIMEOUT}
#define TCPIP_HTTP_MAX_CONNECTIONS		    		${TCPIP_HTTP_MAX_CONNECTIONS}
#define TCPIP_HTTP_DEFAULT_FILE		        		"${TCPIP_HTTP_DEFAULT_FILE}"
#define TCPIP_HTTPS_DEFAULT_FILE	        		"${TCPIP_HTTPS_DEFAULT_FILE}"
#define TCPIP_HTTP_DEFAULT_LEN		        		${TCPIP_HTTP_DEFAULT_LEN}
#define TCPIP_HTTP_MAX_DATA_LEN		        		${TCPIP_HTTP_MAX_DATA_LEN}
#define TCPIP_HTTP_MIN_CALLBACK_FREE				${TCPIP_HTTP_MIN_CALLBACK_FREE}
#define TCPIP_HTTP_SKT_TX_BUFF_SIZE		    		${TCPIP_HTTP_SKT_TX_BUFF_SIZE}
#define TCPIP_HTTP_SKT_RX_BUFF_SIZE		    		${TCPIP_HTTP_SKT_RX_BUFF_SIZE}
#define TCPIP_HTTP_CONFIG_FLAGS		        		${TCPIP_HTTP_CONFIG_FLAGS}
<#if TCPIP_HTTP_FILE_UPLOAD_ENABLE == true>
#define TCPIP_HTTP_FILE_UPLOAD_ENABLE
#define TCPIP_HTTP_FILE_UPLOAD_NAME				"${TCPIP_HTTP_FILE_UPLOAD_NAME}"
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
