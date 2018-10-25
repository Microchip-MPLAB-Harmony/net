<#--
/*******************************************************************************
  TCPIP System Config Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_stack.h.ftl

  Summary:
    TCPIP System Config Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
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

// *****************************************************************************
// *****************************************************************************
// Section: TCPIP Stack Configuration
// *****************************************************************************
// *****************************************************************************

<#if USE_TCPIP_STACK == true>
<#if (tcpipIPv4.TCPIP_STACK_USE_IPV4)?has_content && (tcpipIPv4.TCPIP_STACK_USE_IPV4) == true>
#define TCPIP_STACK_USE_IPV4
</#if>
<#if (tcpipIPv6.TCPIP_STACK_USE_IPV6)?has_content && (tcpipIPv6.TCPIP_STACK_USE_IPV6) == true>
#define TCPIP_STACK_USE_IPV6
</#if>
<#if (tcpipTcp.TCPIP_USE_TCP)?has_content &&  (tcpipTcp.TCPIP_USE_TCP) == true>
#define TCPIP_STACK_USE_TCP
</#if>
<#if (tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true>
#define TCPIP_STACK_USE_UDP
</#if>

#define TCPIP_STACK_TICK_RATE		        		${TCPIP_STACK_TICK_RATE?c}
#define TCPIP_STACK_SECURE_PORT_ENTRIES             ${TCPIP_STACK_SECURE_PORT_ENTRIES?c}

<#if TCPIP_STACK_ALIAS_INTERFACE_SUPPORT == true>
#define TCPIP_STACK_ALIAS_INTERFACE_SUPPORT   true
<#else>
#define TCPIP_STACK_ALIAS_INTERFACE_SUPPORT   false
</#if>

<#if TCPIP_PACKET_LOG_ENABLE == true>
#define TCPIP_PACKET_LOG_ENABLE                     1
#define TCPIP_PKT_LOG_SIZE                          ${TCPIP_PKT_LOG_SIZE?c}
<#else>
#define TCPIP_PACKET_LOG_ENABLE     0
</#if>

/* TCP/IP stack event notification */
<#if TCPIP_STACK_EVENT_NOTIFICATION == true>
#define TCPIP_STACK_USE_EVENT_NOTIFICATION
</#if>
<#if TCPIP_STACK_USER_NOTIFICATION == true>
#define TCPIP_STACK_USER_NOTIFICATION   true
<#else>
#define TCPIP_STACK_USER_NOTIFICATION   false
</#if>
<#if TCPIP_STACK_DOWN_OPERATION == false>
#define TCPIP_STACK_DOWN_OPERATION   false
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   false
<#else>
#define TCPIP_STACK_DOWN_OPERATION   true
<#if TCPIP_STACK_IF_UP_DOWN_OPERATION == true>
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   true
<#else>
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   false
</#if>
</#if>
<#if TCPIP_STACK_DOWN_OPERATION == true>
#define TCPIP_STACK_MAC_DOWN_OPERATION  true
<#else>
<#if TCPIP_STACK_MAC_DOWN_OPERATION == true>
#define TCPIP_STACK_MAC_DOWN_OPERATION   true
<#else>
#define TCPIP_STACK_MAC_DOWN_OPERATION   false
</#if>
</#if>
<#if TCPIP_STACK_INTERFACE_CHANGE_SIGNALING == true>
#define TCPIP_STACK_INTERFACE_CHANGE_SIGNALING   true
<#else>
#define TCPIP_STACK_INTERFACE_CHANGE_SIGNALING   false
</#if>
<#if TCPIP_STACK_CONFIGURATION_SAVE_RESTORE == true>
#define TCPIP_STACK_CONFIGURATION_SAVE_RESTORE   true
<#else>
#define TCPIP_STACK_CONFIGURATION_SAVE_RESTORE   false
</#if>

</#if>
