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

<#----------------------------------------------------------------------------
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

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
