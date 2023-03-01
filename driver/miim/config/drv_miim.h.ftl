<#--
/*******************************************************************************
  MIIM Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim.h.ftl

  Summary:
    MIIM Driver Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2016-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
<#if DRV_MIIM_USE_DRIVER>
/*** MIIM Driver Configuration ***/
<#lt><#assign intMac_array = [] />
<#lt><#list 0 ..(tcpipStack.TCPIP_STACK_NET_INTERFACE_NUM -1) as i >
    <#lt><#assign isIntMac = "tcpipStack.TCPIP_STACK_INT_MAC_IDX${i}"?eval>
    <#lt><#if (isIntMac == true)>
    <#lt><#assign intMacName = "tcpipStack.TCPIP_STACK_NET_INTERFACE_NAME_IDX${i}"?eval>    
        <#lt><#assign intMac_array = intMac_array + [intMacName] />
    <#lt></#if> 
<#lt></#list>
<#lt><#assign miim_count = 0 />
<#list intMac_array as mac>
	<#if ((tcpipStack.TCPIP_DEVICE_FAMILY?has_content) && (tcpipStack.TCPIP_DEVICE_FAMILY  == "PIC32M"))>
		<#lt>#define DRV_MIIM_ETH_MODULE_ID_${miim_count}                ${drvPic32mEthmac.TCPIP_EMAC_MODULE_ID}
	<#else>
		<#lt>#define DRV_MIIM_ETH_MODULE_ID_${miim_count}                ${mac}_BASE_ADDRESS
	</#if>
    <#lt>#define DRV_MIIM_DRIVER_INDEX_${miim_count}                 ${.vars["DRV_MIIM_DRIVER_INDEX" + miim_count?string]}
    <#lt><#assign miim_count = miim_count + 1 />
</#list>
#define DRV_MIIM_INSTANCES_NUMBER           ${DRV_MIIM_INSTANCES_NUMBER}
#define DRV_MIIM_INSTANCE_OPERATIONS        ${DRV_MIIM_INSTANCE_OPERATIONS}
#define DRV_MIIM_INSTANCE_CLIENTS           ${DRV_MIIM_INSTANCE_CLIENTS}
<#if DRV_MIIM_CLIENT_OP_PROTECTION == true>
#define DRV_MIIM_CLIENT_OP_PROTECTION   true
<#else>
#define DRV_MIIM_CLIENT_OP_PROTECTION   false
</#if>
<#if DRV_MIIM_COMMANDS == true>
#define DRV_MIIM_COMMANDS   true
<#else>
#define DRV_MIIM_COMMANDS   false
</#if>
#define DRV_MIIM_DRIVER_OBJECT              ${DRV_MIIM_DRIVER_OBJECT}            
</#if>
