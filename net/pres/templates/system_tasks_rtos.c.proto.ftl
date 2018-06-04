<#--include "/framework/net/pres/templates/system_tasks_rtos.c.proto.ftl"-->
<#if NET_PRES_USE && NET_PRES_RTOS=="Standalone">
    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _NET_PRES_Tasks(ULONG thread_input);
    <#else>
void _NET_PRES_Tasks(void);
    </#if>
</#if>