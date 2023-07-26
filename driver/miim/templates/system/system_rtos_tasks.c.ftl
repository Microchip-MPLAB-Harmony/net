<#--
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>void _DRV_MIIM_Task(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>       <#if DRV_MIIM_INSTANCES_NUMBER gte 1>
    <#lt>       <#list 0 ..(DRV_MIIM_INSTANCES_NUMBER - 1) as i >
    <#lt>       ${DRV_MIIM_DRIVER_OBJECT}.DRV_MIIM_Tasks(sysObj.drvMiim_${i});
    <#lt>       </#list>
    <#lt>       </#if>
    <#lt>       <#if DRV_MIIM_RTOS_USE_DELAY >
    <#lt>        vTaskDelay(${DRV_MIIM_RTOS_DELAY} / portTICK_PERIOD_MS);
    <#lt>       </#if>
    <#lt>    }
    <#lt>}
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "ThreadX">
    <#lt>TX_THREAD      _DRV_MIIM_Task_TCB;
    <#lt>uint8_t*       _DRV_MIIM_Task_Stk_Ptr;

    <#lt>static void _DRV_MIIM_Tasks( ULONG thread_input )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>       <#if DRV_MIIM_INSTANCES_NUMBER gte 1>
    <#lt>       <#list 0 ..(DRV_MIIM_INSTANCES_NUMBER - 1) as i >
    <#lt>        ${DRV_MIIM_DRIVER_OBJECT}.DRV_MIIM_Tasks(sysObj.drvMiim_${i});
    <#lt>       </#list>
    <#lt>       </#if>
    <#lt>       <#if DRV_MIIM_RTOS_USE_DELAY >
    <#lt>        tx_thread_sleep((ULONG)(${DRV_MIIM_RTOS_DELAY} / (TX_TICK_PERIOD_MS)));
    <#lt>       </#if>
    <#lt>    }
    <#lt>}
</#if>
