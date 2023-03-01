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

<#if TCPIP_USE_SNTP_CLIENT == true>
/*** SNTP Configuration ***/
#define TCPIP_STACK_USE_SNTP_CLIENT
#define TCPIP_NTP_DEFAULT_IF		        	"${TCPIP_NTP_DEFAULT_IF}"
<#if TCPIP_NTP_STRING_VERSION == "SNTP_VERSION_3">
#define TCPIP_NTP_VERSION                       3
<#else>
#define TCPIP_NTP_VERSION             			4
</#if>
#define TCPIP_NTP_DEFAULT_CONNECTION_TYPE   	${TCPIP_NTP_DEFAULT_CONNECTION_TYPE}
#define TCPIP_NTP_EPOCH		                	${TCPIP_NTP_EPOCH}ul
#define TCPIP_NTP_REPLY_TIMEOUT		        	${TCPIP_NTP_REPLY_TIMEOUT}
#define TCPIP_NTP_MAX_STRATUM		        	${TCPIP_NTP_MAX_STRATUM}
#define TCPIP_NTP_TIME_STAMP_TMO				${TCPIP_NTP_TIME_STAMP_TMO}
#define TCPIP_NTP_SERVER		        		"${TCPIP_NTP_SERVER}"
#define TCPIP_NTP_SERVER_MAX_LENGTH				${TCPIP_NTP_SERVER_MAX_LENGTH}
#define TCPIP_NTP_QUERY_INTERVAL				${TCPIP_NTP_QUERY_INTERVAL}
#define TCPIP_NTP_FAST_QUERY_INTERVAL	    	${TCPIP_NTP_FAST_QUERY_INTERVAL}
#define TCPIP_NTP_TASK_TICK_RATE				${TCPIP_NTP_TASK_TICK_RATE}
#define TCPIP_NTP_RX_QUEUE_LIMIT				${TCPIP_NTP_RX_QUEUE_LIMIT}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
