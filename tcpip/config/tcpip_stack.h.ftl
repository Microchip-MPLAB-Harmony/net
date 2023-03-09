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
#define TCPIP_STACK_LINK_RATE		        		${TCPIP_STACK_LINK_RATE?c}

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
<#if TCPIP_STACK_EXTERN_PACKET_PROCESS == true>
#define TCPIP_STACK_EXTERN_PACKET_PROCESS   true
<#else>
#define TCPIP_STACK_EXTERN_PACKET_PROCESS   false
</#if>
<#if TCPIP_STACK_RUN_TIME_INIT == true>
#define TCPIP_STACK_RUN_TIME_INIT   true
<#else>
#define TCPIP_STACK_RUN_TIME_INIT   false
</#if>

#define TCPIP_STACK_INTMAC_COUNT           ${TCPIP_STACK_INTMAC_INTERFACE_NUM}
</#if>
