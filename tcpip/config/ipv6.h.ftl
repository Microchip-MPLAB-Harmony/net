<#--
/*******************************************************************************
  IPv6 Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    ipv6.h.ftl

  Summary:
    IPv6 Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


<#if TCPIP_STACK_USE_IPV6 == true>
/*** IPv6 Configuration ***/
#define TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE 		${TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE}
#define TCPIP_IPV6_MINIMUM_LINK_MTU 					${TCPIP_IPV6_MINIMUM_LINK_MTU}
#define TCPIP_IPV6_DEFAULT_LINK_MTU 					${TCPIP_IPV6_DEFAULT_LINK_MTU}
#define TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT 				${TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT}
#define TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME 			${TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME}
#define TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME 				${TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME}
#define TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT 			${TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT}
#define TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT 	${TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT}
#define TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT 			${TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT}
#define TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT 			${TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT}
#define TCPIP_IPV6_TASK_PROCESS_RATE 					${TCPIP_IPV6_TASK_PROCESS_RATE}
#define TCPIP_IPV6_INIT_TASK_PROCESS_RATE 				${TCPIP_IPV6_INIT_TASK_PROCESS_RATE}
<#if TCPIP_IPV6_ULA_GENERATE_ENABLE == true>
#define TCPIP_IPV6_ULA_GENERATE_ENABLE 					true
<#else>
#define TCPIP_IPV6_ULA_GENERATE_ENABLE 					false
</#if>
#define TCPIP_IPV6_ULA_NTP_ACCESS_TMO 					${TCPIP_IPV6_ULA_NTP_ACCESS_TMO}
#define TCPIP_IPV6_ULA_NTP_VALID_WINDOW 				${TCPIP_IPV6_ULA_NTP_VALID_WINDOW}
#define TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT 				${TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT}
#define TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE 			${TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE}
<#if TCPIP_IPV6_EXTERN_PACKET_PROCESS == true>
#define TCPIP_IPV6_EXTERN_PACKET_PROCESS   true
<#else>
#define TCPIP_IPV6_EXTERN_PACKET_PROCESS   false
</#if>


<#if TCPIP_IPV6_G3_PLC_SUPPORT == true>
#define TCPIP_IPV6_G3_PLC_SUPPORT                       true

<#if TCPIP_IPV6_G3_PLC_BORDER_ROUTER == true>
#define TCPIP_IPV6_G3_PLC_BORDER_ROUTER                 true
<#else>
#define TCPIP_IPV6_G3_PLC_BORDER_ROUTER                 false
</#if>
<#else>
#define TCPIP_IPV6_G3_PLC_SUPPORT                       false
</#if>

<#if TCPIP_IPV6_RIID_ENABLE == true>
#define TCPIP_IPV6_RANDOM_INTERFACE_ID_ENABLE           true

    <#if (TCPIP_IPV6_RIID_SELECTION)?has_content>
        <#if (TCPIP_IPV6_RIID_SELECTION) == "RIID Default Generation">
        <#if (TCPIP_IPV6_RIID_SEC_KEY_F)?has_content>
        <#lt>#define TCPIP_IPV6_RIID_SEC_KEY_F              &${TCPIP_IPV6_RIID_SEC_KEY_F}
        <#else>
        <#lt>#define TCPIP_IPV6_RIID_SEC_KEY_F              NULL
        </#if>
        <#if (TCPIP_IPV6_RIID_NET_IFACE_F)?has_content>
        <#lt>#define TCPIP_IPV6_RIID_NET_IFACE_F            &${TCPIP_IPV6_RIID_NET_IFACE_F}
        <#else>
        <#lt>#define TCPIP_IPV6_RIID_NET_IFACE_F            NULL
        </#if>
        <#if (TCPIP_IPV6_RIID_NET_ID_F)?has_content>
        <#lt>#define TCPIP_IPV6_RIID_NET_ID_F               &${TCPIP_IPV6_RIID_NET_ID_F}
        <#else>
        <#lt>#define TCPIP_IPV6_RIID_NET_ID_F               NULL
        </#if>
        <#else>
            <#if (TCPIP_IPV6_RIID_PRF)?has_content>
            <#lt>#define TCPIP_IPV6_RIID_PRF             &${TCPIP_IPV6_RIID_PRF}
            <#else>
            <#lt>#define TCPIP_IPV6_RIID_PRF             NULL
            </#if>
        </#if>
    </#if>

#define TCPIP_IPV6_START_FLAGS                       \
<#if TCPIP_IPV6_CONFIG_FLAG_RIID == true>
                                                        TCPIP_IPV6_FLAG_RANDOM_INTERFACE_ID |\
</#if>
                                                        TCPIP_IPV6_FLAG_NONE
<#else>
#define TCPIP_IPV6_RANDOM_INTERFACE_ID_ENABLE           false
</#if>


</#if>
