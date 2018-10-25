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

<#if TCPIP_STACK_USE_DHCP_SERVER == true>
/*** DHCP Server Configuration ***/
#define TCPIP_STACK_USE_DHCP_SERVER
#define TCPIP_DHCPS_TASK_PROCESS_RATE                               ${TCPIP_DHCPS_TASK_PROCESS_RATE}
#define TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT                           ${TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT}
#define TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO                          ${TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO}
#define TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK                        ${TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK}
<#if TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES == true>
#define TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES                        true
<#else>
#define TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES                        false
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
</#if>

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
