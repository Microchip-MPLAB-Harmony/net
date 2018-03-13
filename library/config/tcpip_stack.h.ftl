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
<#if CONFIG_USE_TCPIP_STACK == true>
<#if CONFIG_TCPIP_STACK_USE_IPV4 == true>
#define TCPIP_STACK_USE_IPV4
</#if>
<#if CONFIG_TCPIP_STACK_USE_IPV6 == true>
#define TCPIP_STACK_USE_IPV6
</#if>
<#if CONFIG_TCPIP_USE_TCP == true>
#define TCPIP_STACK_USE_TCP
</#if>
<#if CONFIG_TCPIP_USE_UDP == true>
#define TCPIP_STACK_USE_UDP
</#if>

#define TCPIP_STACK_TICK_RATE		        		${CONFIG_TCPIP_STACK_TICK_RATE}
#define TCPIP_STACK_SECURE_PORT_ENTRIES             ${CONFIG_TCPIP_STACK_SECURE_PORT_ENTRIES}

<#if CONFIG_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT == true>
#define TCPIP_STACK_ALIAS_INTERFACE_SUPPORT   true
<#else>
#define TCPIP_STACK_ALIAS_INTERFACE_SUPPORT   false
</#if>

<#if CONFIG_TCPIP_PACKET_LOG_ENABLE == true>
#define TCPIP_PACKET_LOG_ENABLE                     1
#define TCPIP_PKT_LOG_SIZE                          ${CONFIG_TCPIP_PKT_LOG_SIZE}
<#else>
#define TCPIP_PACKET_LOG_ENABLE     0
</#if>

/* TCP/IP stack event notification */
<#if CONFIG_TCPIP_STACK_EVENT_NOTIFICATION == true>
#define TCPIP_STACK_USE_EVENT_NOTIFICATION
</#if>
<#if CONFIG_TCPIP_STACK_USER_NOTIFICATION == true>
#define TCPIP_STACK_USER_NOTIFICATION   true
<#else>
#define TCPIP_STACK_USER_NOTIFICATION   false
</#if>
<#if CONFIG_TCPIP_STACK_DOWN_OPERATION == false>
#define TCPIP_STACK_DOWN_OPERATION   false
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   false
<#else>
#define TCPIP_STACK_DOWN_OPERATION   true
<#if CONFIG_TCPIP_STACK_IF_UP_DOWN_OPERATION == true>
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   true
<#else>
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   false
</#if>
</#if>
<#if CONFIG_TCPIP_STACK_DOWN_OPERATION == true>
#define TCPIP_STACK_MAC_DOWN_OPERATION  true
<#else>
<#if CONFIG_TCPIP_STACK_MAC_DOWN_OPERATION == true>
#define TCPIP_STACK_MAC_DOWN_OPERATION   true
<#else>
#define TCPIP_STACK_MAC_DOWN_OPERATION   false
</#if>
</#if>
<#if CONFIG_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING == true>
#define TCPIP_STACK_INTERFACE_CHANGE_SIGNALING   true
<#else>
#define TCPIP_STACK_INTERFACE_CHANGE_SIGNALING   false
</#if>
<#if CONFIG_TCPIP_STACK_CONFIGURATION_SAVE_RESTORE == true>
#define TCPIP_STACK_CONFIGURATION_SAVE_RESTORE   true
<#else>
#define TCPIP_STACK_CONFIGURATION_SAVE_RESTORE   false
</#if>
<#include "/framework/tcpip/config/tcpip_heap.h.ftl">
<#include "/framework/tcpip/config/arp.h.ftl">
<#include "/framework/tcpip/config/berkeley_api.h.ftl">
<#include "/framework/tcpip/config/ddns.h.ftl">
<#include "/framework/tcpip/config/dhcp.h.ftl">
<#include "/framework/tcpip/config/dhcps.h.ftl">
<#include "/framework/tcpip/config/dns.h.ftl">
<#include "/framework/tcpip/config/dnss.h.ftl">
<#include "/framework/tcpip/config/ftp.h.ftl">
<#include "/framework/tcpip/config/http.h.ftl">
<#include "/framework/tcpip/config/http_net.h.ftl">
<#include "/framework/tcpip/config/icmp.h.ftl">
<#include "/framework/tcpip/config/iperf.h.ftl">
<#include "/framework/tcpip/config/ipv6.h.ftl">
<#include "/framework/tcpip/config/nbns.h.ftl">
<#include "/framework/tcpip/config/smtp.h.ftl">
<#include "/framework/tcpip/config/smtpc.h.ftl">
<#include "/framework/tcpip/config/sntp.h.ftl">
<#include "/framework/tcpip/config/snmp.h.ftl">
<#include "/framework/tcpip/config/tcp.h.ftl">
<#include "/framework/tcpip/config/tcpip_announce.h.ftl">
<#if (CONFIG_DSTBDPIC32CZ == true)>
<#include "/framework/tcpip/config/tcpip_mac_pic32c.h.ftl">
<#else>
<#include "/framework/tcpip/config/tcpip_mac_pic32m.h.ftl">
</#if>
<#include "/framework/tcpip/config/tcpip_reboot.h.ftl">
<#include "/framework/tcpip/config/telnet.h.ftl">
<#include "/framework/tcpip/config/udp.h.ftl">
<#include "/framework/tcpip/config/zeroconf.h.ftl">
<#include "/framework/tcpip/config/tcpip_cmd.h.ftl">
<#include "/framework/tcpip/config/tftpc.h.ftl">
<#include "/framework/tcpip/config/ip.h.ftl">
<#include "/framework/tcpip/config/igmp.h.ftl">
<#include "/framework/tcpip/config/network_config_idx.h.ftl">
<#if CONFIG_TCPIP_STACK_USE_HTTP_SERVER == true || CONFIG_TCPIP_STACK_USE_HTTP_NET_SERVER == true || CONFIG_TCPIP_USE_SNMP == true || CONFIG_TCPIP_USE_FTP_MODULE == true || CONFIG_TCPIP_USE_TFTPC_MODULE>
<#include "/framework/tcpip/config/sys_fs_wrapper.h.ftl">
</#if>
</#if>
