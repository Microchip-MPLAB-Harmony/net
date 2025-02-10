<#--
Copyright (C) 2018-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
<#if HarmonyCore.SELECT_RTOS == "BareMetal">    
    <#if DRV_MIIM_INSTANCES_NUMBER gte 1>
        <#lt><#list 0 ..(DRV_MIIM_INSTANCES_NUMBER - 1) as i >
        <#lt>   ${DRV_MIIM_DRIVER_OBJECT}.miim_Tasks(sysObj.drvMiim_${i});
        <#lt></#list>
    </#if>  
<#elseif HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>    xTaskCreate( _DRV_MIIM_Task,
    <#lt>        "DRV_MIIM_Tasks",
    <#lt>        DRV_MIIM_RTOS_STACK_SIZE,
    <#lt>        (void*)NULL,
    <#lt>        DRV_MIIM_RTOS_TASK_PRIORITY,
    <#lt>        (TaskHandle_t*)NULL
    <#lt>    );
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "ThreadX">
    <#lt>    tx_byte_allocate(&byte_pool_0,
    <#lt>        (VOID **) &_DRV_MIIM_Task_Stk_Ptr,
    <#lt>        DRV_MIIM_RTOS_STACK_SIZE,
    <#lt>        TX_NO_WAIT
    <#lt>    );

    <#lt>    tx_thread_create(&_DRV_MIIM_Task_TCB,
    <#lt>        "DRV_MIIM_TASKS",
    <#lt>        _DRV_MIIM_Tasks,
    <#lt>        0,
    <#lt>        _DRV_MIIM_Task_Stk_Ptr,
    <#lt>        DRV_MIIM_RTOS_STACK_SIZE,
    <#lt>        DRV_MIIM_RTOS_TASK_PRIORITY,
    <#lt>        DRV_MIIM_RTOS_TASK_PRIORITY,
    <#lt>        TX_NO_TIME_SLICE,
    <#lt>        TX_AUTO_START
    <#lt>    );
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->

