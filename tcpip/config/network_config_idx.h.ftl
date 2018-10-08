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
<#if .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${INDEX?string}"]?has_content && .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${idx}"] = "PIC32WK">
/* Network Configuration Index ${INDEX?string} */
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}		"${.vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${INDEX?string}"]}"
<#else>  
/* Network Configuration Index ${INDEX?string} */
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}	"${.vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"]}"
</#if>
<#if .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"]?has_content>
<#if .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "ETHMAC">
#define TCPIP_IF_ETHMAC
<#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "GMAC">
#define TCPIP_IF_GMAC
<#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${INDEX?string}"] = "PIC32WK">
#define TCPIP_IF_PIC32WK
<#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "MRF24WN">
#define TCPIP_IF_MRF24WN
<#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "WINC1500">
#define TCPIP_IF_WINC1500
<#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "WILC1000">
#define TCPIP_IF_WILC1000
</#if>
</#if>

#define TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${INDEX?string}				"${.vars["TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${INDEX?string}"]}"
<#if .vars["TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}				"${.vars["TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}				0
</#if>

#define TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${INDEX?string}			"${.vars["TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${INDEX?string}			"${.vars["TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${INDEX?string}			"${.vars["TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_DNS_IDX${INDEX?string}				"${.vars["TCPIP_NETWORK_DEFAULT_DNS_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${INDEX?string}			"${.vars["TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${INDEX?string}			"${.vars["TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX${INDEX?string}			\
<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_DHCP_IDX${INDEX?string}"] == "DHCP_CLIENT_ENABLE">
													TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON |\
<#elseif .vars["TCPIP_NETWORK_INTERFACE_FLAG_DHCP_IDX${INDEX?string}"] == "ZEROCONF_LL_ENABLE">
													TCPIP_NETWORK_CONFIG_ZCLL_ON |\
<#elseif .vars["TCPIP_NETWORK_INTERFACE_FLAG_DHCP_IDX${INDEX?string}"] == "DHCP_SERVER_ENABLE">
													TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON |\
</#if>
<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_DNS_IDX${INDEX?string}"] == "DNS_CLIENT_ENABLE">
													TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON |\
<#elseif .vars["TCPIP_NETWORK_INTERFACE_FLAG_DNS_IDX${INDEX?string}"] == "DNS_SERVER_ENABLE">
													TCPIP_NETWORK_CONFIG_DNS_SERVER_ON |\
</#if>
<#if .vars["TCPIP_NETWORK_CONFIG_MULTICAST_IDX${INDEX?string}"]>
													TCPIP_NETWORK_CONFIG_MULTICAST_ON |\
</#if>
<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${INDEX?string}"]>
													TCPIP_NETWORK_CONFIG_IPV6_ADDRESS |\
</#if>
													TCPIP_NETWORK_CONFIG_IP_STATIC
													
#define TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${INDEX?string}			${.vars["TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${INDEX?string}"]}

<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${INDEX?string}"]>
<#if .vars["TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}			"${.vars["TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}			0
</#if>
<#if .vars["TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string}	${.vars["TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string}"]}
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string}	0
</#if>
<#if .vars["TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_ID${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWA_IDX${INDEX?string}Y			"${.vars["TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${INDEX?string}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${INDEX?string}			0
</#if>
</#if>