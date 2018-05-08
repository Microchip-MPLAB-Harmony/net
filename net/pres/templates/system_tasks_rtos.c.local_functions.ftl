<#--include "/framework/net/pres/templates/system_tasks_rtos.c.local_functions.ftl"-->
<#if CONFIG_NET_PRES_USE && CONFIG_NET_PRES_RTOS=="Standalone">
    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _NET_PRES_Tasks(ULONG thread_input)
    <#else>
void _NET_PRES_Tasks(void)
    </#if>
{
    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
    </#if> 
    while(1)
    {
        /* Maintain the TCP/IP Stack*/
        NET_PRES_Tasks(sysObj.netPres);
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_NET_PRES_RTOS_DELAY/>		
    }
}
</#if>
