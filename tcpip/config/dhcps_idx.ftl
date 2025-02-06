<#--
/*******************************************************************************
  dhcps_idx Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcps_idx.ftl

  Summary:
    dhcps_idx Freemarker Template File

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


config TCPIP_DHCP_SERVER_INSTANCES_NUMBER_GT_${INSTANCE+1}
    bool
    depends on TCPIP_STACK_USE_DHCP_SERVER
<#if INSTANCE != 0>
	default n if TCPIP_DHCP_SERVER_INSTANCES_NUMBER_GT_${INSTANCE} = n     
</#if>	
	default n if TCPIP_DHCP_SERVER_INSTANCES_NUMBER = ${INSTANCE+1}
	default y

config TCPIP_DHCP_SERVER_IDX${INSTANCE}
    depends on TCPIP_STACK_USE_DHCP_SERVER 
<#if INSTANCE != 0>
	             && TCPIP_DHCP_SERVER_INSTANCES_NUMBER_GT_${INSTANCE}
</#if>
    bool "DHCP Server Instance ${INSTANCE}"
    default y
    ---help---
    
    ---endhelp---
    
   
config TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX${INSTANCE}
    string "DHCPS Address Range Start"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
    default "192.168.1.100"
	---help---
	IDH_HTML_TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START
	---endhelp---
  
config TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX${INSTANCE}
    string "DHCPS Server IP Address"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
    default "192.168.1.1"
	---help---
	IDH_HTML_TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS
	---endhelp---
    
config TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX${INSTANCE}
    string "DHCPS Netmask"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
    default "255.255.255.0"
	---help---
	IDH_HTML_TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS
	---endhelp---

config TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX${INSTANCE}
    string "Default Gateway"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
    default "192.168.1.1"
	---help---
	IDH_HTML_TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS
	---endhelp---
  
config TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX${INSTANCE}
    string "Primary DNS Server Address"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
    default "192.168.1.1"
	---help---
	IDH_HTML_TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS
	---endhelp---
    
config TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX${INSTANCE}
    string "Secondary DNS Server Address"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
    default "192.168.1.1"
	---help---
	IDH_HTML_TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS
	---endhelp---

config TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX${INSTANCE}
    int "Interface Index for the DHCP Server"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
<#if INSTANCE != 0>
    default  ${INSTANCE}
<#else>
    default 0
</#if>
    
	---help---
    IDH_HTML_TCPIP_DHCPS_ADDRESS_CONFIG
	---endhelp---

config TCPIP_DHCP_SERVER_POOL_ENABLED_IDX${INSTANCE}
    bool "Pool Enabled"
    depends on TCPIP_STACK_USE_DHCP_SERVER
    depends on TCPIP_DHCP_SERVER_IDX${INSTANCE}
    default y
	---help---
    IDH_HTML_TCPIP_DHCPS_ADDRESS_CONFIG
	---endhelp---
    
 
