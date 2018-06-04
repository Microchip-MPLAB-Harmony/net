<#--include "/framework/net/pres/templates/system_tasks_rtos.c.create_task.ftl"-->
<#if NET_PRES_USE && NET_PRES_RTOS=="Standalone">
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_NET_PRES_Tasks" TASK_NAME="Net Pres Tasks" TASK_PRI=NET_PRES_RTOS_TASK_PRIORITY TASK_STK_SZ=NET_PRES_RTOS_TASK_SIZE/>
</#if>