<#--
/*******************************************************************************
  DHCP server Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcps.h.ftl

  Summary:
    DHCP server Freemarker Template File

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

<#if TCPIP_STACK_USE_DHCP_SERVER == true>
/*** DHCP Server Configuration ***/
#define TCPIP_STACK_USE_DHCP_SERVER
#define TCPIP_DHCPS_TASK_PROCESS_RATE                     	${TCPIP_DHCPS_TASK_PROCESS_RATE}
#define TCPIP_DHCPS_MAX_NUMBER_INSTANCES					${TCPIP_DHCP_SERVER_INSTANCES_NUMBER}
#define TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT                   ${TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT}
#define TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO                  ${TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO}
#define TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK                ${TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK}
<#if TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES == true>
#define TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES              	true
<#else>
#define TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES              	false
</#if>
#define TCPIP_DHCPS_LEASE_DURATION	TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO

<#if TCPIP_DHCP_SERVER_IDX0 == true>
/*** DHCP Server Instance 0 Configuration ***/
<#if TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX0?has_content>
#define TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX0             "${TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX0}"
<#else>
#define TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX0             0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX0?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX0                  "${TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX0}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX0                  0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX0?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX0             "${TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX0}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX0             0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX0?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX0             "${TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX0}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX0             0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX0?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX0         "${TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX0}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX0         0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX0?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX0       "${TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX0}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX0       0
</#if>

#define TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX0                      ${TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX0}

<#if TCPIP_DHCP_SERVER_POOL_ENABLED_IDX0 == true>
#define TCPIP_DHCP_SERVER_POOL_ENABLED_IDX0                         true
<#else>
#define TCPIP_DHCP_SERVER_POOL_ENABLED_IDX0                         false
</#if>

#define TCPIP_DHCP_SERVER_POOL_INDEX_IDX0								0
</#if>

<#if TCPIP_DHCP_SERVER_IDX1 == true>
/*** DHCP Server Instance 1 Configuration ***/
<#if TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX1?has_content>
#define TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX1             "${TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX1}"
<#else>
#define TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX1             0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX1?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX1                  "${TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX1}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX1                  0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX1?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX1             "${TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX1}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX1             0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX1?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX1             "${TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX1}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX1             0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX1?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX1         "${TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX1}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX1         0
</#if>

<#if TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX1?has_content>
#define TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX1       "${TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX1}"
<#else>
#define TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX1       0
</#if>

#define TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX1                      ${TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX1}

<#if TCPIP_DHCP_SERVER_POOL_ENABLED_IDX1 == true>
#define TCPIP_DHCP_SERVER_POOL_ENABLED_IDX1                         true
<#else>
#define TCPIP_DHCP_SERVER_POOL_ENABLED_IDX1                         false
</#if>
#define TCPIP_DHCP_SERVER_POOL_INDEX_IDX1								1
</#if>

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
