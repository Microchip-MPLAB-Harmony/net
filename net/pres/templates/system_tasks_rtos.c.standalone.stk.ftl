<#--include "/framework/net/pres/templates/system_tasks_rtos.c.standalone.ftl"-->
<#if CONFIG_NET_PRES_USE && CONFIG_NET_PRES_RTOS=="Standalone">
OS_STK _NET_PRES_TasksStk[${CONFIG_USB_RTOS_TASK_SIZE}];
</#if>