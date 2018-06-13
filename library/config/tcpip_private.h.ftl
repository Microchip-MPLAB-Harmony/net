/*******************************************************************************
  Microchip TCP/IP Stack Include File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_private.h.ftl

  Summary:
   Private include file for the TCPIP stack

  Description:
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef __TCPIP_STACK_PRIVATE_H__
#define __TCPIP_STACK_PRIVATE_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
        
#include "system_config.h"
//#include "system/debug/sys_debug.h"
//#include "system/console/sys_console.h"
#include "system/sys_console_h2_adapter.h"
//#include "system/command/sys_command.h"
#include "system/sys_cmd_h2_adapter.h"
//#include "system/random/sys_random.h"
#include "system/sys_random_h2_adapter.h"
//#include "system/tmr/sys_tmr.h"
#include "system/sys_time_h2_adapter.h"

#include "osal/osal.h"

#include "tcpip/src/common/helpers.h"



// Internal adjustments - dependencies
// change signaling depends on the IPv4
#if defined(TCPIP_STACK_USE_IPV4) && (TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
#define _TCPIP_STACK_INTERFACE_CHANGE_SIGNALING     1
#else
#define _TCPIP_STACK_INTERFACE_CHANGE_SIGNALING     0
#endif  // defined(TCPIP_STACK_USE_IPV4)

// alias interface support depends on IPv4
#if defined(TCPIP_STACK_USE_IPV4) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)
#define _TCPIP_STACK_ALIAS_INTERFACE_SUPPORT     1
#else
#define _TCPIP_STACK_ALIAS_INTERFACE_SUPPORT     0
#endif  // defined(TCPIP_STACK_USE_IPV4) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)

// public module interface
#include "tcpip/tcpip.h"

#include "tcpip/src/tcpip_types.h"
#include "tcpip/src/link_list.h"
#include "tcpip/src/tcpip_heap_alloc.h"

#include "tcpip/tcpip_mac_object.h"

// private stack manager interface
#include "tcpip/src/tcpip_manager_control.h"

<#if (tcpipAnnounce.TCPIP_USE_ANNOUNCE)?has_content &&  (tcpipAnnounce.TCPIP_USE_ANNOUNCE) == true>
#include "tcpip/src/tcpip_announce_manager.h"
</#if>
<#if (tcpipNdp.TCPIP_IPV6_NDP)?has_content>
#include "tcpip/src/ndp_manager.h"
</#if>
<#if (tcpipIPv4.TCPIP_STACK_USE_IPV4)?has_content && (tcpipIPv4.TCPIP_STACK_USE_IPV4) == true>
#include "tcpip/src/ipv4_manager.h"
</#if>
<#if (tcpipIPv6.TCPIP_STACK_USE_IPV6)?has_content && (tcpipIPv6.TCPIP_STACK_USE_IPV6) == true>
#include "tcpip/src/ipv6_manager.h"
#include "tcpip/src/icmpv6_manager.h"
#include "tcpip/src/dhcpv6_manager.h"
</#if>
<#if (tcpipIcmp.TCPIP_STACK_USE_ICMPV4)?has_content &&  (tcpipIcmp.TCPIP_STACK_USE_ICMPV4) == true>
#include "tcpip/src/icmp_manager.h"
</#if>
<#if (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT)?has_content &&  (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT) == true>
#include "tcpip/src/dhcp_manager.h"
</#if>
<#if (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER)?has_content &&  (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER) == true>
#include "tcpip/src/dhcps_manager.h"
</#if>
<#if (tcpipArp.TCPIP_USE_ARP)?has_content && (tcpipArp.TCPIP_USE_ARP) == true>
#include "tcpip/src/arp_manager.h"
</#if>
<#if (tcpipDns.TCPIP_USE_DNS_CLIENT)?has_content &&  (tcpipDns.TCPIP_USE_DNS_CLIENT) == true>
#include "tcpip/src/dns_manager.h"
</#if>
<#if (tcpipTcp.TCPIP_USE_TCP)?has_content &&  (tcpipTcp.TCPIP_USE_TCP) == true>
#include "tcpip/src/tcp_manager.h"
</#if>
<#if (tcpipNbns.TCPIP_USE_NBNS)?has_content &&  (tcpipNbns.TCPIP_USE_NBNS) == true>
#include "tcpip/src/nbns_manager.h"
</#if>
<#if (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER)?has_content &&  (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER) == true>
#include "tcpip/src/http_manager.h"
</#if>
<#if (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER)?has_content &&  (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER) == true>
#include "tcpip/src/http_net_manager.h"
</#if>
<#if (tcpipCmd.TCPIP_STACK_USE_COMMANDS)?has_content &&  (tcpipCmd.TCPIP_STACK_USE_COMMANDS) == true>
#include "tcpip/src/tcpip_commands_manager.h"
</#if>
<#if (tcpipTelnet.TCPIP_USE_TELNET)?has_content &&  (tcpipTelnet.TCPIP_USE_TELNET) == true>
#include "tcpip/src/telnet_manager.h"
</#if>
<#if (tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true>
#include "tcpip/src/udp_manager.h"
</#if>
<#if (tcpipSntp.TCPIP_USE_SNTP_CLIENT)?has_content &&  (tcpipSntp.TCPIP_USE_SNTP_CLIENT) == true>
#include "tcpip/src/sntp_manager.h"
</#if>
<#if (tcpipReboot.TCPIP_USE_REBOOT_SERVER)?has_content &&  (tcpipReboot.TCPIP_USE_REBOOT_SERVER) == true>
#include "tcpip/src/tcpip_reboot_manager.h"
</#if>
<#if (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API)?has_content &&  (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API) == true>
#include "tcpip/src/berkeley_manager.h"
</#if>
<#if (tcpipDnss.TCPIP_USE_DNSS)?has_content &&  (tcpipDnss.TCPIP_USE_DNSS) == true>
#include "tcpip/src/dnss_manager.h"
</#if>
<#if (tcpipFtps.TCPIP_USE_FTP_MODULE)?has_content &&  (tcpipFtps.TCPIP_USE_FTP_MODULE) == true>
#include "tcpip/src/ftp_manager.h"
</#if>
<#if (tcpipDdns.TCPIP_USE_DDNS)?has_content &&  (tcpipDdns.TCPIP_USE_DDNS) == true>
#include "tcpip/src/ddns_manager.h"
</#if>
<#if (tcpipSnmp.TCPIP_USE_SNMP)?has_content &&  (tcpipSnmp.TCPIP_USE_SNMP) == true>
#include "tcpip/src/snmp_manager.h"
</#if>
<#if (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG)?has_content &&  (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG) == true>
#include "tcpip/src/zero_conf_manager.h"
</#if>
#include "tcpip/src/lldp_manager.h"
<#if (tcpipTftpc.TCPIP_USE_TFTPC_MODULE)?has_content &&  (tcpipTftpc.TCPIP_USE_TFTPC_MODULE) == true>
#include "tcpip/src/tftpc_manager.h"
</#if>
<#if (tcpipIperf.TCPIP_USE_IPERF)?has_content &&  (tcpipIperf.TCPIP_USE_IPERF) == true>
#include "tcpip/src/iperf_manager.h"
</#if>
<#if (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT)?has_content &&  (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT) == true>
#include "tcpip/src/smtpc_manager.h"
</#if>
<#if (tcpipIgmp.TCPIP_USE_IGMP)?has_content &&  (tcpipIgmp.TCPIP_USE_IGMP) == true>
#include "tcpip/src/igmp_manager.h"
</#if>
#include "tcpip/src/tcpip_packet.h"
#include "tcpip/src/tcpip_helpers_private.h"
#include "tcpip/src/oahash.h"
#include "tcpip/src/hash_fnv.h"
#include "tcpip/src/tcpip_notify.h"

#endif  // __TCPIP_STACK_PRIVATE_H__
