<#--
/*******************************************************************************
  SMTPC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    smtpc.h.ftl

  Summary:
    SMTPC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2016-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_USE_SMTPC_CLIENT == true>
/*** SMTPC Configuration ***/
#define TCPIP_STACK_USE_SMTPC
#define TCPIP_SMTPC_MAIL_CONNECTIONS 	            ${TCPIP_SMTPC_MAIL_CONNECTIONS}
#define TCPIP_SMTPC_CLIENT_MESSAGE_DATE 	        "${TCPIP_SMTPC_CLIENT_MESSAGE_DATE}"
#define TCPIP_SMTPC_SERVER_REPLY_TIMEOUT 	        ${TCPIP_SMTPC_SERVER_REPLY_TIMEOUT}
#define TCPIP_SMTPC_SERVER_DATA_TIMEOUT 	        ${TCPIP_SMTPC_SERVER_DATA_TIMEOUT}
#define TCPIP_SMTPC_TLS_HANDSHAKE_TIMEOUT 	        ${TCPIP_SMTPC_TLS_HANDSHAKE_TIMEOUT}
#define TCPIP_SMTPC_MAIL_RETRIES 	                ${TCPIP_SMTPC_MAIL_RETRIES}
#define TCPIP_SMTPC_SERVER_TRANSIENT_RETRY_TIMEOUT  ${TCPIP_SMTPC_SERVER_TRANSIENT_RETRY_TIMEOUT}
#define TCPIP_SMTPC_INTERNAL_RETRY_TIMEOUT          ${TCPIP_SMTPC_INTERNAL_RETRY_TIMEOUT}
#define TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE 	    ${TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE}
#define TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE 	    ${TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE}
#define TCPIP_SMTPC_CLIENT_ADDR_BUFFER_SIZE 	    ${TCPIP_SMTPC_CLIENT_ADDR_BUFFER_SIZE}
#define TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE 	        ${TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE}
#define TCPIP_SMTPC_SKT_TX_BUFF_SIZE			    ${TCPIP_SMTPC_SKT_TX_BUFF_SIZE}
#define TCPIP_SMTPC_SKT_RX_BUFF_SIZE			    ${TCPIP_SMTPC_SKT_RX_BUFF_SIZE}
#define TCPIP_SMTPC_TASK_TICK_RATE			        ${TCPIP_SMTPC_TASK_TICK_RATE}
<#if TCPIP_SMTPC_USE_MAIL_COMMAND == true>
#define TCPIP_SMTPC_USE_MAIL_COMMAND
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
