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
    
 
