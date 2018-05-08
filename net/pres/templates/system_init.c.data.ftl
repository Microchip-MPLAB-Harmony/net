<#--include "/framework/net/pres/tls/templates/system_init.c.data.ftl"-->
<#if CONFIG_NET_PRES_USE>
    <#macro NET_PRES_HRM_TRANS_DATA
        CONN
        PEER>
    .fpOpen        = (NET_PRES_TransOpen)TCPIP_${CONN}_${PEER}Open,
    .fpLocalBind         = (NET_PRES_TransBind)TCPIP_${CONN}_Bind,
    .fpRemoteBind        = (NET_PRES_TransBind)TCPIP_${CONN}_RemoteBind,
    <#if CONN == "TCP">
    <#if CONFIG_TCPIP_TCP_DYNAMIC_OPTIONS == true>
    .fpOptionGet         = (NET_PRES_TransOption)TCPIP_${CONN}_OptionsGet,
    .fpOptionSet         = (NET_PRES_TransOption)TCPIP_${CONN}_OptionsSet,
    <#else>
    .fpOptionGet         = NULL,
    .fpOptionSet         = NULL,
    </#if>
    <#else>
    .fpOptionGet         = (NET_PRES_TransOption)TCPIP_${CONN}_OptionsGet,
    .fpOptionSet         = (NET_PRES_TransOption)TCPIP_${CONN}_OptionsSet,
    </#if>
    .fpIsConnected       = (NET_PRES_TransBool)TCPIP_${CONN}_IsConnected,
    <#if CONN == "TCP">
    .fpWasReset          = (NET_PRES_TransBool)TCPIP_${CONN}_WasReset,
    <#else>
    .fpWasReset          = NULL,
    </#if>
    .fpDisconnect        = (NET_PRES_TransBool)TCPIP_${CONN}_Disconnect,
    <#if CONN == "TCP">
    .fpConnect           = (NET_PRES_TransBool)TCPIP_${CONN}_Connect,
    <#else>
    .fpConnect          = NULL,
    </#if>
    .fpClose             = (NET_PRES_TransClose)TCPIP_${CONN}_Close,
    .fpSocketInfoGet     = (NET_PRES_TransSocketInfoGet)TCPIP_${CONN}_SocketInfoGet,
    .fpFlush             = (NET_PRES_TransBool)TCPIP_${CONN}_Flush,
    <#if CONN == "TCP">
    .fpPeek              = (NET_PRES_TransPeek)TCPIP_${CONN}_ArrayPeek,
    <#else>
    .fpPeek              = NULL,
    </#if>
    .fpDiscard           = (NET_PRES_TransDiscard)TCPIP_${CONN}_Discard,
    .fpHandlerRegister   = (NET_PRES_TransHandlerRegister)TCPIP_${CONN}_SignalHandlerRegister,
    .fpHandlerDeregister = (NET_PRES_TransSignalHandlerDeregister)TCPIP_${CONN}_SignalHandlerDeregister,
    .fpRead              = (NET_PRES_TransRead)TCPIP_${CONN}_ArrayGet,
    .fpWrite             = (NET_PRES_TransWrite)TCPIP_${CONN}_ArrayPut,
    .fpReadyToRead       = (NET_PRES_TransReady)TCPIP_${CONN}_GetIsReady,
    .fpReadyToWrite      = (NET_PRES_TransReady)TCPIP_${CONN}_PutIsReady,
    .fpIsPortDefaultSecure = (NET_PRES_TransIsPortDefaultSecured)TCPIP_Helper_${CONN}SecurePortGet,
    </#macro>
    <#macro NET_PRES_TRANS_DATA
        INST_NUMBER>
/* Net Presentation Layer Data Definitions */
#include "framework/net/pres/net_pres_enc_glue.h"
        <#assign useStream=.vars["CONFIG_NET_PRES_SUPPORT_STREAM_IDX${INST_NUMBER}"]>
        <#assign useDataGram=.vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_IDX${INST_NUMBER}"]>
        <#assign useServer=.vars["CONFIG_NET_PRES_SUPPORT_SERVER_IDX${INST_NUMBER}"]>
        <#assign useClient=.vars["CONFIG_NET_PRES_SUPPORT_CLIENT_IDX${INST_NUMBER}"]>
        <#assign useHarmonyTcp=.vars["CONFIG_NET_PRES_TRANSPORT_AS_TCPIP_IDX${INST_NUMBER}"]>

        <#if useHarmonyTcp && useStream && useServer && CONFIG_TCPIP_USE_TCP>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}SS = {
            <@NET_PRES_HRM_TRANS_DATA "TCP" "Server"/>
};
        </#if>
        <#if useHarmonyTcp && useStream && useClient && CONFIG_TCPIP_USE_TCP>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}SC = {
            <@NET_PRES_HRM_TRANS_DATA "TCP" "Client"/>
};
        </#if>
        <#if useHarmonyTcp && useDataGram && useServer && CONFIG_TCPIP_USE_UDP>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}DS = {
            <@NET_PRES_HRM_TRANS_DATA "UDP" "Server"/>
};
        </#if>
        <#if useHarmonyTcp && useDataGram && useClient && CONFIG_TCPIP_USE_UDP>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}DC = {
            <@NET_PRES_HRM_TRANS_DATA "UDP" "Client"/>
};
        </#if>
    </#macro>
    <#assign numInstance=CONFIG_NET_PRES_INSTANCES?number>
    <#list 0..(numInstance-1) as idx>
        <#if .vars["CONFIG_NET_PRES_IDX${idx}"]>
            <@NET_PRES_TRANS_DATA 
                INST_NUMBER=idx
            />
            <#assign supportStream=.vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${idx}"]>
            <#assign supportDataGram=.vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${idx}"]>
            <#assign supportServer=.vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${idx}"]>
            <#assign supportClient=.vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${idx}"]>
        </#if>
    </#list>
static const NET_PRES_INST_DATA netPresCfgs[] = 
{
    <#list 0..(numInstance-1) as idx>
        <#if .vars["CONFIG_NET_PRES_IDX${idx}"]>
    {
            <#assign useStream=.vars["CONFIG_NET_PRES_SUPPORT_STREAM_IDX${idx}"]>
            <#assign useDataGram=.vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_IDX${idx}"]>
            <#assign useServer=.vars["CONFIG_NET_PRES_SUPPORT_SERVER_IDX${idx}"]>
            <#assign useClient=.vars["CONFIG_NET_PRES_SUPPORT_CLIENT_IDX${idx}"]>
            <#assign useHarmonyTcp=.vars["CONFIG_NET_PRES_TRANSPORT_AS_TCPIP_IDX${idx}"]>
            <#if useHarmonyTcp && useStream && useServer && CONFIG_TCPIP_USE_TCP>        
        .pTransObject_ss = &netPresTransObject${idx}SS,
            </#if>
            <#if useHarmonyTcp && useStream && useClient && CONFIG_TCPIP_USE_TCP>        
        .pTransObject_sc = &netPresTransObject${idx}SC,
            </#if>
            <#if useHarmonyTcp && useDataGram && useServer && CONFIG_TCPIP_USE_UDP>        
        .pTransObject_ds = &netPresTransObject${idx}DS,
            </#if>
            <#if useHarmonyTcp && useDataGram && useClient && CONFIG_TCPIP_USE_UDP>        
        .pTransObject_dc = &netPresTransObject${idx}DC,
            </#if>
            <#if supportStream && supportServer>
        .pProvObject_ss = &net_pres_EncProviderStreamServer${idx},
            <#else>
        .pProvObject_ss = NULL,
            </#if>
            <#if supportStream && supportClient>
        .pProvObject_sc = &net_pres_EncProviderStreamClient${idx},
            <#else>
        .pProvObject_sc = NULL,
            </#if>
            <#if supportDataGram && supportServer>
        .pProvObject_ds = &net_pres_EncProviderDataGramServer${idx},
            <#else>
        .pProvObject_ds = NULL,
            </#if>
            <#if supportDataGram && supportClient>
        .pProvObject_dc = &net_pres_EncProviderDataGramClient${idx},
            <#else>
        .pProvObject_dc = NULL,
            </#if>
    },
        </#if>     
     </#list>
};

static const NET_PRES_INIT_DATA netPresInitData = 
{
    .numLayers = sizeof(netPresCfgs) / sizeof(NET_PRES_INST_DATA),
    .pInitData = netPresCfgs
};
  
 
</#if>
