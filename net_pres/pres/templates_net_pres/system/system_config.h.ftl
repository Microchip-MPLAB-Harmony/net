<#--
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#--include "/framework/net_pres/pres/tls/templates/system_config.h.ftl"-->
<#if NET_PRES_USE>
/* MPLAB Harmony Net Presentation Layer Definitions*/
#define NET_PRES_NUM_INSTANCE 1
#define NET_PRES_NUM_SOCKETS ${NET_PRES_SOCKETS}

<#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS != "BareMetal">
    <#lt>/* Net Pres RTOS Configurations*/
    <#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "FreeRTOS">
        <#lt>#define NET_PRES_RTOS_STACK_SIZE                ${NET_PRES_RTOS_STACK_SIZE / 4}
    <#else>
        <#lt>#define NET_PRES_RTOS_STACK_SIZE                ${NET_PRES_RTOS_STACK_SIZE}
    </#if>
    <#lt>#define NET_PRES_RTOS_TASK_PRIORITY             ${NET_PRES_RTOS_TASK_PRIORITY}
	
	<#assign numInstance= 1>
	<#assign freertos_present= false/>
    <#list 0..(numInstance-1) as idx>
		<#assign netPresEncProviderIdx = "NET_PRES_ENC_PROVIDE"?eval>		
		<#if netPresEncProviderIdx == "WolfSSL" && (HarmonyCore.SELECT_RTOS) == "FreeRTOS">
			<#assign freertos_present=true/>
        </#if>
    </#list>
	<#if freertos_present == true>	
		<#lt>#define FREERTOS
	</#if>
</#if>
</#if>

	
