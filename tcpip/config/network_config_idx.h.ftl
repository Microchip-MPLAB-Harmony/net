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
