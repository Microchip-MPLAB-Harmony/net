<#--
/*******************************************************************************
  network_config_idx0 Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    network_config_idx0.h.ftl

  Summary:
    network_config_idx0 Freemarker Template File

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
<#macro genConfigs idx>
<#if .vars["CONFIG_TCPIP_STACK_NETWORK_CONFIG_IDX${idx}"]>
<#if .vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${idx}"] = "PIC32WK">
/*** Network Configuration Index 0 ***/
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}		"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${idx}"]}"
<#else>  
/*** Network Configuration Index 0 ***/
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}		"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}"]}"
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}"] = "PIC32INT">
#define TCPIP_IF_PIC32INT
<#elseif .vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}"] = "PIC32CINT">
#define TCPIP_IF_PIC32CINT
<#elseif .vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${idx}"] = "PIC32WK">
#define TCPIP_IF_PIC32WK
<#elseif .vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}"] = "MRF24WN">
#define TCPIP_IF_MRF24WN
<#elseif .vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}"] = "WINC1500">
#define TCPIP_IF_WINC1500
<#elseif .vars["CONFIG_TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx}"] = "WILC1000">
#define TCPIP_IF_WILC1000
</#if>
#define TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${idx}				"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${idx}"]}"
<#if .vars["CONFIG_TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${idx}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${idx}				"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${idx}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${idx}				0
</#if>
#define TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${idx}			"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${idx}"]}"
#define TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${idx}				"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${idx}"]}"
#define TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${idx}				"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${idx}"]}"
#define TCPIP_NETWORK_DEFAULT_DNS_IDX${idx}					"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_DNS_IDX${idx}"]}"
#define TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${idx}			"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${idx}"]}"
#define TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${idx}			"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${idx}"]}"
#define TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX${idx}			\
<#if .vars["CONFIG_TCPIP_NETWORK_INTERFACE_FLAG_DHCP_CLIENT_IDX${idx}"]>
													TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON |\
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_INTERFACE_FLAG_ZCLL_IDX${idx}"]>
													TCPIP_NETWORK_CONFIG_ZCLL_ON |\
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_INTERFACE_FLAG_DHCP_SERVER_IDX${idx}"]>
													TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON |\
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_INTERFACE_FLAG_DNS_CLIENT_IDX${idx}"]>
													TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON |\
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_INTERFACE_FLAG_DNS_SERVER_IDX${idx}"]>
													TCPIP_NETWORK_CONFIG_DNS_SERVER_ON |\
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_CONFIG_MULTICAST_IDX${idx}"]>
													TCPIP_NETWORK_CONFIG_MULTICAST_ON |\
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${idx}"]>
													TCPIP_NETWORK_CONFIG_IPV6_ADDRESS |\
</#if>
													TCPIP_NETWORK_CONFIG_IP_STATIC
#define TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${idx}			${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${idx}"]}
<#if .vars["CONFIG_TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${idx}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${idx}			"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${idx}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${idx}			0
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${idx}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${idx}	${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${idx}"]}
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${idx}	0
</#if>
<#if .vars["CONFIG_TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_ID${idx}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWA_IDX${idx}Y			"${.vars["CONFIG_TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${idx}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${idx}			0
</#if>
</#if>
</#macro>
<#list 0..(CONFIG_TCPIP_STACK_NETWORK_CONFIG_NUMBER?number-1) as idx>
	<@genConfigs idx/>
</#list>