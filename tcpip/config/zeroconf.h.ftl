<#--
/*******************************************************************************
  Zero Config Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    zeroconf.h.ftl

  Summary:
    Berkeley Freemarker Template File

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

<#if TCPIP_USE_LINK_ZERO_CONFIG == true>
#define TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
<#if TCPIP_ZC_INFO_ZCLL>
#define TCPIP_ZC_INFO_ZCLL
</#if>
<#if TCPIP_ZC_DEBUG_ZCLL>
#define TCPIP_ZC_DEBUG_ZCLL
</#if>
<#if TCPIP_ZC_WARN_ZCLL>
#define TCPIP_ZC_WARN_ZCLL
</#if>
#define TCPIP_ZC_LL_PROBE_WAIT ${TCPIP_ZC_LL_PROBE_WAIT}
#define TCPIP_ZC_LL_PROBE_MIN ${TCPIP_ZC_LL_PROBE_MIN}
#define TCPIP_ZC_LL_PROBE_MAX ${TCPIP_ZC_LL_PROBE_MAX}
#define TCPIP_ZC_LL_PROBE_NUM ${TCPIP_ZC_LL_PROBE_NUM}
#define TCPIP_ZC_LL_ANNOUNCE_WAIT ${TCPIP_ZC_LL_ANNOUNCE_WAIT}
#define TCPIP_ZC_LL_ANNOUNCE_NUM ${TCPIP_ZC_LL_ANNOUNCE_NUM}
#define TCPIP_ZC_LL_ANNOUNCE_INTERVAL ${TCPIP_ZC_LL_ANNOUNCE_INTERVAL}
#define TCPIP_ZC_LL_MAX_CONFLICTS ${TCPIP_ZC_LL_MAX_CONFLICTS}
#define TCPIP_ZC_LL_RATE_LIMIT_INTERVAL ${TCPIP_ZC_LL_RATE_LIMIT_INTERVAL}
#define TCPIP_ZC_LL_DEFEND_INTERVAL ${TCPIP_ZC_LL_DEFEND_INTERVAL}
#define TCPIP_ZC_LL_IPV4_LLBASE 0x${TCPIP_ZC_LL_IPV4_LLBASE}
#define TCPIP_ZC_LL_IPV4_LLBASE_MASK 0x${TCPIP_ZC_LL_IPV4_LLBASE_MASK}
#define TCPIP_ZC_LL_TASK_TICK_RATE ${TCPIP_ZC_LL_TASK_TICK_RATE}
	<#if TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG == true>
#define TCPIP_STACK_USE_ZEROCONF_MDNS_SD
<#if TCPIP_ZC_INFO_MDNS>
#define TCPIP_ZC_INFO_MDNS
</#if>
<#if TCPIP_ZC_DEBUG_MDNS>
#define TCPIP_ZC_DEBUG_MDNS
</#if>
<#if TCPIP_ZC_WARN_MDNS>
#define TCPIP_ZC_WARN_MDNS
</#if>
#define TCPIP_ZC_MDNS_TASK_TICK_RATE ${TCPIP_ZC_MDNS_TASK_TICK_RATE}
#define TCPIP_ZC_MDNS_PORT ${TCPIP_ZC_MDNS_PORT}
#define TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE ${TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE}
#define TCPIP_ZC_MDNS_MAX_LABEL_SIZE ${TCPIP_ZC_MDNS_MAX_LABEL_SIZE}
#define TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE ${TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE}
#define TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE ${TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE}
#define TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE ${TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE}
#define TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE ${TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE}
#define TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL ${TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL}
#define TCPIP_ZC_MDNS_MAX_RR_NUM ${TCPIP_ZC_MDNS_MAX_RR_NUM}
#define TCPIP_ZC_MDNS_PROBE_WAIT ${TCPIP_ZC_MDNS_PROBE_WAIT}
#define TCPIP_ZC_MDNS_PROBE_INTERVAL ${TCPIP_ZC_MDNS_PROBE_INTERVAL}
#define TCPIP_ZC_MDNS_PROBE_NUM ${TCPIP_ZC_MDNS_PROBE_NUM}
#define TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM ${TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM}
#define TCPIP_ZC_MDNS_ANNOUNCE_NUM ${TCPIP_ZC_MDNS_ANNOUNCE_NUM}
#define TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL ${TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL}
#define TCPIP_ZC_MDNS_ANNOUNCE_WAIT ${TCPIP_ZC_MDNS_ANNOUNCE_WAIT}

	</#if>
</#if>
