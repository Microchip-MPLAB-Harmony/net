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
<#if TCPIP_TCP_EXTERN_PACKET_PROCESS == true>
#define TCPIP_TCP_EXTERN_PACKET_PROCESS   true
<#else>
#define TCPIP_TCP_EXTERN_PACKET_PROCESS   false
</#if>
#define TCPIP_TCP_DISABLE_CRYPTO_USAGE		        	    ${TCPIP_TCP_DISABLE_CRYPTO_USAGE?c}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
