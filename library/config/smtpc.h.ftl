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

/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.

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
<#if CONFIG_TCPIP_USE_SMTPC_CLIENT == true>
/*** SMTPC Configuration ***/
#define TCPIP_STACK_USE_SMTPC
#define TCPIP_SMTPC_MAIL_CONNECTIONS 	            ${CONFIG_TCPIP_SMTPC_MAIL_CONNECTIONS}
#define TCPIP_SMTPC_CLIENT_MESSAGE_DATE 	        "${CONFIG_TCPIP_SMTPC_CLIENT_MESSAGE_DATE}"
#define TCPIP_SMTPC_SERVER_REPLY_TIMEOUT 	        ${CONFIG_TCPIP_SMTPC_SERVER_REPLY_TIMEOUT}
#define TCPIP_SMTPC_SERVER_DATA_TIMEOUT 	        ${CONFIG_TCPIP_SMTPC_SERVER_DATA_TIMEOUT}
#define TCPIP_SMTPC_TLS_HANDSHAKE_TIMEOUT 	        ${CONFIG_TCPIP_SMTPC_TLS_HANDSHAKE_TIMEOUT}
#define TCPIP_SMTPC_MAIL_RETRIES 	                ${CONFIG_TCPIP_SMTPC_MAIL_RETRIES}
#define TCPIP_SMTPC_SERVER_TRANSIENT_RETRY_TIMEOUT  ${CONFIG_TCPIP_SMTPC_SERVER_TRANSIENT_RETRY_TIMEOUT}
#define TCPIP_SMTPC_INTERNAL_RETRY_TIMEOUT          ${CONFIG_TCPIP_SMTPC_INTERNAL_RETRY_TIMEOUT}
#define TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE 	    ${CONFIG_TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE}
#define TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE 	    ${CONFIG_TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE}
#define TCPIP_SMTPC_CLIENT_ADDR_BUFFER_SIZE 	    ${CONFIG_TCPIP_SMTPC_CLIENT_ADDR_BUFFER_SIZE}
#define TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE 	        ${CONFIG_TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE}
#define TCPIP_SMTPC_SKT_TX_BUFF_SIZE			    ${CONFIG_TCPIP_SMTPC_SKT_TX_BUFF_SIZE}
#define TCPIP_SMTPC_SKT_RX_BUFF_SIZE			    ${CONFIG_TCPIP_SMTPC_SKT_RX_BUFF_SIZE}
#define TCPIP_SMTPC_TASK_TICK_RATE			        ${CONFIG_TCPIP_SMTPC_TASK_TICK_RATE}
<#if CONFIG_TCPIP_SMTPC_USE_MAIL_COMMAND == true>
#define TCPIP_SMTPC_USE_MAIL_COMMAND
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
