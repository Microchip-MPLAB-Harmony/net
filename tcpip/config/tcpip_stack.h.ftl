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
