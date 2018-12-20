<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>void _NET_PRES_Tasks(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>        NET_PRES_Tasks(sysObj.netPres);
             <#if NET_PRES_RTOS_USE_DELAY >
    <#lt>        vTaskDelay(${NET_PRES_RTOS_DELAY} / portTICK_PERIOD_MS);
             </#if>
    <#lt>    }
    <#lt>}
</#if>