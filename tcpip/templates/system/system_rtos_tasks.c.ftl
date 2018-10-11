<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>void _TCPIP_STACK_Task(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>        TCPIP_STACK_Task(sysObj.tcpip);
             <#if TCPIP_STACK_RTOS_USE_DELAY >
    <#lt>        vTaskDelay(${TCPIP_STACK_RTOS_DELAY} / portTICK_PERIOD_MS);
             </#if>
    <#lt>    }
    <#lt>}
</#if>