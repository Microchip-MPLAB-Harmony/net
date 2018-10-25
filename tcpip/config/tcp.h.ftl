<#--
/*******************************************************************************
  TCP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcp.h.ftl

  Summary:
    TCP Freemarker Template File

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

<#if TCPIP_USE_TCP == true>
/*** TCP Configuration ***/
#define TCPIP_TCP_MAX_SEG_SIZE_TX		        	${TCPIP_TCP_MAX_SEG_SIZE_TX}
#define TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE			${TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE}
#define TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE			${TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE}
<#if TCPIP_TCP_DYNAMIC_OPTIONS == true>
#define TCPIP_TCP_DYNAMIC_OPTIONS             			true
<#else>
#define TCPIP_TCP_DYNAMIC_OPTIONS             			false
</#if>
#define TCPIP_TCP_START_TIMEOUT_VAL		        	${TCPIP_TCP_START_TIMEOUT_VAL}
#define TCPIP_TCP_DELAYED_ACK_TIMEOUT		    		${TCPIP_TCP_DELAYED_ACK_TIMEOUT}
#define TCPIP_TCP_FIN_WAIT_2_TIMEOUT		    		${TCPIP_TCP_FIN_WAIT_2_TIMEOUT}
#define TCPIP_TCP_KEEP_ALIVE_TIMEOUT		    		${TCPIP_TCP_KEEP_ALIVE_TIMEOUT}
#define TCPIP_TCP_CLOSE_WAIT_TIMEOUT		    		${TCPIP_TCP_CLOSE_WAIT_TIMEOUT}
#define TCPIP_TCP_MAX_RETRIES		            		${TCPIP_TCP_MAX_RETRIES}
#define TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES			${TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES}
#define TCPIP_TCP_MAX_SYN_RETRIES		        	${TCPIP_TCP_MAX_SYN_RETRIES}
#define TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL			${TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL}
#define TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL			${TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL}
#define TCPIP_TCP_MAX_SOCKETS		                ${TCPIP_TCP_MAX_SOCKETS}
#define TCPIP_TCP_TASK_TICK_RATE		        	${TCPIP_TCP_TASK_TICK_RATE}
#define TCPIP_TCP_MSL_TIMEOUT		        	    ${TCPIP_TCP_MSL_TIMEOUT}
#define TCPIP_TCP_QUIET_TIME		        	    ${TCPIP_TCP_QUIET_TIME}
<#if TCPIP_TCP_COMMANDS == true>
#define TCPIP_TCP_COMMANDS   true
<#else>
#define TCPIP_TCP_COMMANDS   false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
