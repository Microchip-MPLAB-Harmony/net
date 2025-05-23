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
/*
Copyright (C) 2016-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
*/

// DOM-IGNORE-END

#ifndef H_TCPIP_STACK_PRIVATE_H_
#define H_TCPIP_STACK_PRIVATE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
        
#include "device.h"
#include "system_config.h"
#include "system/debug/sys_debug.h"
#include "system/sys_random_h2_adapter.h"
#include "system/sys_time_h2_adapter.h"

#include "osal/osal.h"

#include "tcpip/src/common/helpers.h"



// Internal adjustments - dependencies
// change signaling depends on the IPv4
#if defined(TCPIP_STACK_USE_IPV4) && (TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
#define M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING     1
#else
#define M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING     0
#endif  // defined(TCPIP_STACK_USE_IPV4)

// alias interface support depends on IPv4
#if defined(TCPIP_STACK_USE_IPV4) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)
#define M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT     1
#else
#define M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT     0
#endif  // defined(TCPIP_STACK_USE_IPV4) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)

// debug symbols

#define M_TCPIP_STACK_DEBUG_MASK_BASIC       0x01    // enable the TCPIPStack_Assert and TCPIPStack_Condition calls
#define M_TCPIP_STACK_ENABLE_ASSERT_LOOP     0       // if !0, then an assert call will loop forever 
                                                    // should be 0 for a release build
#define M_TCPIP_STACK_ENABLE_COND_LOOP       0       // if !0, then an condition call will loop forever 
                                                    // should be 0 for a release build

// enabled debugging masks 
#define M_TCPIP_STACK_DEBUG_LEVEL            (M_TCPIP_STACK_DEBUG_MASK_BASIC)  // usually the basic debugging is enabled so asserts are not ignored


// assert and condition functions that all stack modules could use
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_BASIC) != 0)
void TCPIPStack_Assert(bool cond, const char* fileName, const char* funcName, int lineNo);
void TCPIPStack_Condition(bool cond, const char* fileName, const char* funcName, int lineNo);
#else
#define TCPIPStack_Assert(cond, fileName, funcName, lineNo)
#define TCPIPStack_Condition(cond, fileName, funcName, lineNo)
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_BASIC) != 0)


// public module interface
#include "tcpip/tcpip.h"

#include "net_pres/pres/net_pres_socketapi.h"

#include "tcpip/src/tcpip_types.h"
#include "tcpip/src/link_list.h"
#include "tcpip/src/tcpip_heap_alloc.h"

#include "tcpip/tcpip_mac_object.h"

// private stack manager interface
#include "tcpip/src/tcpip_manager_control.h"

<#if (tcpipAnnounce.TCPIP_USE_ANNOUNCE)?has_content &&  (tcpipAnnounce.TCPIP_USE_ANNOUNCE) == true>
#include "tcpip/src/tcpip_announce_manager.h"
</#if>
<#if (tcpipNdp.TCPIP_USE_NDP)?has_content && (tcpipNdp.TCPIP_USE_NDP) == true>
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
<#if (tcpipDhcpServer.TCPIP_STACK_USE_DHCP_SERVER_V2)?has_content &&  (tcpipDhcpServer.TCPIP_STACK_USE_DHCP_SERVER_V2) == true>
#include "tcpip/src/dhcp_server_manager.h"
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
<#if (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER)?has_content &&  (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER) == true>
#include "tcpip/src/http_net_manager.h"
</#if>
<#if (tcpipHttpSrvV2.TCPIP_STACK_USE_HTTP_SERVER_V2)?has_content &&  (tcpipHttpSrvV2.TCPIP_STACK_USE_HTTP_SERVER_V2) == true>
#include "tcpip/src/http_server_manager.h"
</#if>
<#if (tcpipCmd.TCPIP_STACK_USE_COMMANDS)?has_content &&  (tcpipCmd.TCPIP_STACK_USE_COMMANDS) == true>
#include "tcpip/src/tcpip_commands_manager.h"
</#if>
<#if (tcpipTelnet.TCPIP_USE_TELNET)?has_content &&  (tcpipTelnet.TCPIP_USE_TELNET) == true>
#include "tcpip/src/telnet_manager.h"
</#if>
<#if ((tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true) ||
((tcpipIPv4.TCPIP_STACK_USE_IPV4)?has_content && (tcpipIPv4.TCPIP_STACK_USE_IPV4) == true)>
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
<#if (tcpipTftpc.TCPIP_USE_TFTPC_MODULE)?has_content &&  (tcpipTftpc.TCPIP_USE_TFTPC_MODULE) == true>
#include "tcpip/src/tftpc_manager.h"
</#if>
<#if (tcpipTftps.TCPIP_USE_TFTPS_MODULE)?has_content &&  (tcpipTftps.TCPIP_USE_TFTPS_MODULE) == true>
#include "tcpip/src/tftps_manager.h"
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
<#if (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT)?has_content && (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT) == true>
#include "tcpip/src/ftpc_manager.h"
</#if>
<#if (tcpipWSC.TCPIP_STACK_USE_WS_CLIENT)?has_content &&  (tcpipWSC.TCPIP_STACK_USE_WS_CLIENT) == true>
#include "tcpip/src/wsc_manager.h"
</#if>
<#if (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE)?has_content && (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE) == true>
#include "tcpip/src/tcpip_mac_bridge_manager.h"
</#if>
#include "tcpip/src/tcpip_packet.h"
#include "tcpip/src/tcpip_helpers_private.h"
#include "tcpip/src/oahash.h"
#include "tcpip/src/hash_fnv.h"
#include "tcpip/src/tcpip_notify.h"

#endif  // H_TCPIP_STACK_PRIVATE_H_
