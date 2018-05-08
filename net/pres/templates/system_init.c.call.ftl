<#--include "/framework/net/pres/tls/templates/system_init.c.call.ftl"-->
<#if CONFIG_NET_PRES_USE>
    sysObj.netPres = NET_PRES_Initialize(0, (SYS_MODULE_INIT*)&netPresInitData);
</#if>