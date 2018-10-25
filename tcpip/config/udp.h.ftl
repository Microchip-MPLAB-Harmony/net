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
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
