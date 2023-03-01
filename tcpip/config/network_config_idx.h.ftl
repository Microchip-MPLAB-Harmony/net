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

<#--
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${INDEX?string}"]?has_content && .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${idx}"] = "PIC32WK">
/* Network Configuration Index ${INDEX?string} */
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}     "${.vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${INDEX?string}"]}"
<#else>  
/* Network Configuration Index ${INDEX?string} */
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string} "${.vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"]}"
</#if>
<#if .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"]?has_content>
    <#if .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "ETHMAC">
        <#if ((tcpipNetConfig.TCPIP_STACK_NETWORK_DEV_SERIES)?has_content) && (tcpipNetConfig.TCPIP_STACK_NETWORK_DEV_SERIES != "PIC32MZW")>
            <#lt>#define TCPIP_IF_ETHMAC
        <#else>
            <#lt>#define TCPIP_IF_PIC32MZW_ETHMAC   
        </#if>
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "GMAC">
        <#if ((tcpipNetConfig.TCPIP_STACK_NETWORK_DEV_SERIES)?has_content) && ((tcpipNetConfig.TCPIP_STACK_NETWORK_DEV_SERIES)?contains("PIC32CZ"))>
            <#lt>#define TCPIP_IF_PIC32CZ_GMAC
        <#elseif ((tcpipNetConfig.TCPIP_STACK_NETWORK_DEV_SERIES)?has_content) && ((tcpipNetConfig.TCPIP_STACK_NETWORK_DEV_SERIES)?contains("SAM9X7"))>
            <#lt>#define TCPIP_IF_GMAC_SAM9X7  
        <#else>
            <#lt>#define TCPIP_IF_GMAC  
        </#if>
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "GMAC0">
        <#lt>#define TCPIP_IF_GMAC0
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "GMAC1">
        <#lt>#define TCPIP_IF_GMAC1
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "EMAC0">
        <#lt>#define TCPIP_IF_EMAC0
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "EMAC1">
        <#lt>#define TCPIP_IF_EMAC1
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${INDEX?string}"]?has_content && .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_WK_NAME_IDX${idx}"] = "PIC32WK">
        <#lt>#define TCPIP_IF_PIC32WK
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "MRF24WN">
        <#lt>#define TCPIP_IF_MRF24WN
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "WINC">
        <#lt>#define TCPIP_IF_WINC
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "WILC1000">
        <#lt>#define TCPIP_IF_WILC1000
    <#elseif .vars["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${INDEX?string}"] = "PIC32MZW1">
        <#lt>#define TCPIP_IF_PIC32MZW1
    </#if>
</#if>

#define TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${INDEX?string}              "${.vars["TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${INDEX?string}"]}"
<#if .vars["TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}               "${.vars["TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${INDEX?string}               0
</#if>

#define TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${INDEX?string}         "${.vars["TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${INDEX?string}            "${.vars["TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${INDEX?string}            "${.vars["TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_DNS_IDX${INDEX?string}                "${.vars["TCPIP_NETWORK_DEFAULT_DNS_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${INDEX?string}         "${.vars["TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${INDEX?string}         "${.vars["TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${INDEX?string}"]}"
#define TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX${INDEX?string}            \
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
<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${INDEX?string}"]?has_content && .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${INDEX?string}"]>
                                                    TCPIP_NETWORK_CONFIG_IPV6_ADDRESS |\
</#if>
                                                    TCPIP_NETWORK_CONFIG_IP_STATIC
                                                    
#define TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${INDEX?string}         ${.vars["TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${INDEX?string}"]}

<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${INDEX?string}"]?has_content && .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${INDEX?string}"]>
<#if .vars["TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}           "${.vars["TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${INDEX?string}           0
</#if>
<#if .vars["TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string} ${.vars["TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string}"]}
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${INDEX?string} 0
</#if>
<#if .vars["TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_ID${INDEX?string}"]?has_content>
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWA_IDX${INDEX?string}Y           "${.vars["TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${INDEX?string}"]}"
<#else>
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${INDEX?string}           0
</#if>
</#if>
