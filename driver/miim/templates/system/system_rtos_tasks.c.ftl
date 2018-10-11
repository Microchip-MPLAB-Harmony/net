<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>void _DRV_MIIM_Task(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>        DRV_MIIM_Tasks(sysObj.drvMiim);
             <#if DRV_MIIM_RTOS_USE_DELAY >
    <#lt>        vTaskDelay(${DRV_MIIM_RTOS_DELAY} / portTICK_PERIOD_MS);
             </#if>
    <#lt>    }
    <#lt>}
</#if>