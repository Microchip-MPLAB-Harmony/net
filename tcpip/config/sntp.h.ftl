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

<#if TCPIP_USE_SNTP_CLIENT == true>
/*** SNTP Configuration ***/
#define TCPIP_STACK_USE_SNTP_CLIENT
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
