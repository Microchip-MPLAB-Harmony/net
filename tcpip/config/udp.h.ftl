<#--
/*******************************************************************************
  UDP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    udp.h.ftl

  Summary:
    UDP Freemarker Template File

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

<#if TCPIP_USE_UDP == true>
/*** UDP Configuration ***/
#define TCPIP_UDP_MAX_SOCKETS		                	${TCPIP_UDP_MAX_SOCKETS}
#define TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE		    	${TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE}
#define TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT    	 	${TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT}
#define TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT			${TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT}
<#if TCPIP_UDP_USE_POOL_BUFFERS == true>
#define TCPIP_UDP_USE_POOL_BUFFERS   true
#define TCPIP_UDP_SOCKET_POOL_BUFFERS		        	${TCPIP_UDP_SOCKET_POOL_BUFFERS}
#define TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE		    	${TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE}
<#else>
#define TCPIP_UDP_USE_POOL_BUFFERS   false
</#if>
<#if TCPIP_UDP_USE_TX_CHECKSUM == true>
#define TCPIP_UDP_USE_TX_CHECKSUM             			true
<#else>
#define TCPIP_UDP_USE_TX_CHECKSUM             			false
</#if>
<#if TCPIP_UDP_USE_RX_CHECKSUM == true>
#define TCPIP_UDP_USE_RX_CHECKSUM             			true
<#else>
#define TCPIP_UDP_USE_RX_CHECKSUM             			false
</#if>
<#if TCPIP_UDP_COMMANDS == true>
#define TCPIP_UDP_COMMANDS   true
<#else>
#define TCPIP_UDP_COMMANDS   false
</#if>
<#if TCPIP_UDP_EXTERN_PACKET_PROCESS == true>
#define TCPIP_UDP_EXTERN_PACKET_PROCESS   true
<#else>
#define TCPIP_UDP_EXTERN_PACKET_PROCESS   false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
