<#--
/*******************************************************************************
  SNTP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sntp.h.ftl

  Summary:
    SNTP Freemarker Template File

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
<#if TCPIP_USE_SNTP_CLIENT == true>
/*** SNTP Configuration ***/
#define TCPIP_STACK_USE_SNTP_CLIENT
<#-- H3_ToDo 
<#if CONFIG_PIC32WK == true>
#define TCPIP_NTP_DEFAULT_IF		        		"${TCPIP_NTP_DEFAULT_IF_WK}"
<#else>
#define TCPIP_NTP_DEFAULT_IF		        		"${TCPIP_NTP_DEFAULT_IF}"
</#if>
-->
/** H3_ToDo */
#define TCPIP_NTP_DEFAULT_IF		        		"${TCPIP_NTP_DEFAULT_IF}"
<#if TCPIP_NTP_STRING_VERSION == "SNTP_VERSION_3">
#define TCPIP_NTP_VERSION                           		3
<#else>
#define TCPIP_NTP_VERSION             			    	4
</#if>
#define TCPIP_NTP_DEFAULT_CONNECTION_TYPE   			${TCPIP_NTP_DEFAULT_CONNECTION_TYPE}
#define TCPIP_NTP_EPOCH		                		${TCPIP_NTP_EPOCH}ul
#define TCPIP_NTP_REPLY_TIMEOUT		        		${TCPIP_NTP_REPLY_TIMEOUT}
#define TCPIP_NTP_MAX_STRATUM		        		${TCPIP_NTP_MAX_STRATUM}
#define TCPIP_NTP_TIME_STAMP_TMO				${TCPIP_NTP_TIME_STAMP_TMO}
#define TCPIP_NTP_SERVER		        		"${TCPIP_NTP_SERVER}"
#define TCPIP_NTP_SERVER_MAX_LENGTH				${TCPIP_NTP_SERVER_MAX_LENGTH}
#define TCPIP_NTP_QUERY_INTERVAL				${TCPIP_NTP_QUERY_INTERVAL}
#define TCPIP_NTP_FAST_QUERY_INTERVAL	    			${TCPIP_NTP_FAST_QUERY_INTERVAL}
#define TCPIP_NTP_TASK_TICK_RATE				${TCPIP_NTP_TASK_TICK_RATE}
#define TCPIP_NTP_RX_QUEUE_LIMIT				${TCPIP_NTP_RX_QUEUE_LIMIT}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
