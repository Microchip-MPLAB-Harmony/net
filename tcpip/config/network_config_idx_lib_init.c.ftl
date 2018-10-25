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
