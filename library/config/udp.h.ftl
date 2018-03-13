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
<#if CONFIG_TCPIP_USE_UDP == true>
/*** UDP Configuration ***/
#define TCPIP_UDP_MAX_SOCKETS		                	${CONFIG_TCPIP_UDP_MAX_SOCKETS}
#define TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE		    	${CONFIG_TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE}
#define TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT    	 	${CONFIG_TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT}
#define TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT			${CONFIG_TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT}
<#if CONFIG_TCPIP_UDP_USE_POOL_BUFFERS == true>
#define TCPIP_UDP_USE_POOL_BUFFERS   true
#define TCPIP_UDP_SOCKET_POOL_BUFFERS		        	${CONFIG_TCPIP_UDP_SOCKET_POOL_BUFFERS}
#define TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE		    	${CONFIG_TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE}
<#else>
#define TCPIP_UDP_USE_POOL_BUFFERS   false
</#if>
<#if CONFIG_TCPIP_UDP_USE_TX_CHECKSUM == true>
#define TCPIP_UDP_USE_TX_CHECKSUM             			true
<#else>
#define TCPIP_UDP_USE_TX_CHECKSUM             			false
</#if>
<#if CONFIG_TCPIP_UDP_USE_RX_CHECKSUM == true>
#define TCPIP_UDP_USE_RX_CHECKSUM             			true
<#else>
#define TCPIP_UDP_USE_RX_CHECKSUM             			false
</#if>
<#if CONFIG_TCPIP_UDP_COMMANDS == true>
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
