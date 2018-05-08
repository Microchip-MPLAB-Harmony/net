<#--include "/framework/net/pres/templates/system_tasks_rtos.c.task_call.ftl"-->
<#if CONFIG_NET_PRES_USE && CONFIG_NET_PRES_RTOS!="Standalone">
    NET_PRES_Tasks(sysObj.netPres);
</#if>