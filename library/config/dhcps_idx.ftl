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
    
 