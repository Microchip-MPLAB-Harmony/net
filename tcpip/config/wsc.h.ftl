<#--
/*******************************************************************************
  WSC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    wsc.h.ftl

  Summary:
    WSC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_STACK_USE_WS_CLIENT == true>

/*** WSC Configuration ***/
#define TCPIP_STACK_USE_WS_CLIENT
#define TCPIP_WSC_CONNECTIONS 	            ${TCPIP_WSC_CONNECTIONS}
#define TCPIP_WSC_SKT_TX_BUFF_SIZE			${TCPIP_WSC_SKT_TX_BUFF_SIZE}
#define TCPIP_WSC_SKT_RX_BUFF_SIZE			${TCPIP_WSC_SKT_RX_BUFF_SIZE}
#define TCPIP_WSC_TASK_RATE			        ${TCPIP_WSC_TASK_RATE}
#define TCPIP_WSC_SERVER_REPLY_TIMEOUT      ${TCPIP_WSC_SERVER_REPLY_TIMEOUT}
#define TCPIP_WSC_USER_READ_TIMEOUT         ${TCPIP_WSC_USER_READ_TIMEOUT}
#define TCPIP_WSC_SERVER_MAX_LEN 	        ${TCPIP_WSC_SERVER_MAX_LEN}
#define TCPIP_WSC_RESOURCE_MAX_LEN 	        ${TCPIP_WSC_RESOURCE_MAX_LEN}
#define TCPIP_WSC_PROTO_MAX_LEN 	        ${TCPIP_WSC_PROTO_MAX_LEN}
#define TCPIP_WSC_PROTO_MAX_NO 	            ${TCPIP_WSC_PROTO_MAX_NO}
<#if TCPIP_WSC_COMMANDS == true>
#define TCPIP_WSC_COMMANDS                  true
<#else>
#define TCPIP_WSC_COMMANDS                  false
</#if>

<#if TCPIP_WSC_CONNECTION_SEM == true>
#define TCPIP_WSC_CONNECTION_SEM                  true
<#else>
#define TCPIP_WSC_CONNECTION_SEM                  false
</#if>

<#if TCPIP_WSC_LARGE_FRAME_SUPPORT == true>
#define TCPIP_WSC_LARGE_FRAME_SUPPORT       true
</#if>
#define TCPIP_WSC_CONFIG_FLAGS              TCPIP_WSC_MODULE_FLAG_NONE

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
