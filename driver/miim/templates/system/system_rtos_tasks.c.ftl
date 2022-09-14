<#----------------------------------------------------------------------------
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
---------------------------------------------------------------------------->

<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>void _DRV_MIIM_Task(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>       <#list 0 ..(DRV_MIIM_INSTANCES_NUMBER -1) as i >
    <#lt>        DRV_MIIM_Tasks(sysObj.drvMiim_${i});
    <#lt>       </#list>
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
    <#lt>       <#list 0 ..(DRV_MIIM_INSTANCES_NUMBER -1) as i >
    <#lt>        DRV_MIIM_Tasks(sysObj.drvMiim_${i});
    <#lt>       </#list>
             <#if DRV_MIIM_RTOS_USE_DELAY >
    <#lt>        tx_thread_sleep((ULONG)(${DRV_MIIM_RTOS_DELAY} / (TX_TICK_PERIOD_MS)));
             </#if>
    <#lt>    }
    <#lt>}
</#if>
