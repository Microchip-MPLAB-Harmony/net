<#--include "/framework/net/pres/tls/templates/system_config.h.ftl"-->
<#if NET_PRES_USE>
/* MPLAB Harmony Net Presentation Layer Definitions*/
#define NET_PRES_NUM_INSTANCE ${__INSTANCE_COUNT}
#define NET_PRES_NUM_SOCKETS ${NET_PRES_SOCKETS}

<#if HarmonyCore.SELECT_RTOS != "BareMetal">
    <#lt>/* Net Pres RTOS Configurations*/
    <#lt>#define NET_PRES_RTOS_STACK_SIZE           ${NET_PRES_RTOS_STACK_SIZE}
    <#lt>#define NET_PRES_RTOS_TASK_PRIORITY             ${NET_PRES_RTOS_TASK_PRIORITY}
</#if>

</#if>