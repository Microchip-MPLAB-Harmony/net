<#--
/*******************************************************************************
  network_config_idx0 Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    network_config_idx_lib_init.c.ftl

  Summary:
    network_config_idx0 Freemarker Template File

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

const TCPIP_NETWORK_CONFIG __attribute__((unused))  TCPIP_HOSTS_CONFIGURATION[] =
{
<#macro genhostConfigs idx>
<#if .vars["TCPIP_STACK_NETWORK_CONFIG_IDX${idx}"]>
/*** Network Configuration Index 0 ***/
    {
        TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${idx},       // interface
        TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${idx},            // hostName
        TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${idx},             // macAddr
        TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${idx},           // ipAddr
        TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${idx},              // ipMask
        TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${idx},              // gateway
        TCPIP_NETWORK_DEFAULT_DNS_IDX${idx},                  // priDNS
        TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${idx},           // secondDNS
        TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${idx},           // powerMode
        TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX${idx},      // startFlags
       &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${idx},           // pMacObject
<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${idx}"]>
        TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${idx},         // ipv6Addr
        TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${idx},   // ipv6PrefixLen
        TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${idx},         // ipv6Gateway 
</#if>
    },
</#if>
</#macro>
<#list 0..(TCPIP_STACK_NETWORK_CONFIG_NUMBER?number-1) as idx>
	<@genhostConfigs idx/>
</#list>
};
