<#----------------------------------------------------------------------------
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
---------------------------------------------------------------------------->

<#--include "/net_pres/pres/tls/templates/system_init.c.data.ftl"-->
<#if NET_PRES_USE>
    <#macro NET_PRES_HRM_TRANS_DATA
        CONN
        PEER>
    .fpOpen        = (NET_PRES_TransOpen)TCPIP_${CONN}_${PEER}Open,
    .fpLocalBind         = (NET_PRES_TransBind)TCPIP_${CONN}_Bind,
    .fpRemoteBind        = (NET_PRES_TransBind)TCPIP_${CONN}_RemoteBind,
    <#if CONN == "TCP">
    <#if ((tcpipTcp.TCPIP_TCP_DYNAMIC_OPTIONS)?has_content &&  (tcpipTcp.TCPIP_TCP_DYNAMIC_OPTIONS) == true)>
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
    .fpWasDisconnected   = (NET_PRES_TransBool)TCPIP_${CONN}_WasDisconnected,
    <#else>
    .fpWasReset          = NULL,
    .fpWasDisconnected   = NULL,
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
#include "net_pres/pres/net_pres_enc_glue.h"
        <#assign useStream= "NET_PRES_SUPPORT_STREAM"?eval>
        <#assign useDataGram= "NET_PRES_SUPPORT_DATAGRAM"?eval> 
        <#assign useServer= "NET_PRES_SUPPORT_SERVER"?eval> 
        <#assign useClient= "NET_PRES_SUPPORT_CLIENT"?eval> 
        <#assign useHarmonyTcp= "NET_PRES_TRANSPORT_AS_TCPIP"?eval> 

        <#if useHarmonyTcp && useStream && useServer && ((tcpipTcp.TCPIP_USE_TCP)?has_content &&  (tcpipTcp.TCPIP_USE_TCP) == true)>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}SS = {
            <@NET_PRES_HRM_TRANS_DATA "TCP" "Server"/>
};
        </#if>
        <#if useHarmonyTcp && useStream && useClient && ((tcpipTcp.TCPIP_USE_TCP)?has_content &&  (tcpipTcp.TCPIP_USE_TCP) == true)>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}SC = {
            <@NET_PRES_HRM_TRANS_DATA "TCP" "Client"/>
};
        </#if>
        <#if useHarmonyTcp && useDataGram && useServer && ((tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true)>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}DS = {
            <@NET_PRES_HRM_TRANS_DATA "UDP" "Server"/>
};
        </#if>
        <#if useHarmonyTcp && useDataGram && useClient && ((tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true)>        
static const NET_PRES_TransportObject netPresTransObject${INST_NUMBER}DC = {
            <@NET_PRES_HRM_TRANS_DATA "UDP" "Client"/>
};
        </#if>
    </#macro>
    <#assign numInstance= 1>
    <#list 0..(numInstance-1) as idx>	
		<@NET_PRES_TRANS_DATA 
			INST_NUMBER=idx
		/>
		<#assign supportEncryption= "NET_PRES_SUPPORT_ENCRYPTION"?eval> 
		<#assign supportStream= "NET_PRES_SUPPORT_STREAM_ENC"?eval> 
		<#assign supportDataGram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#assign supportServer=  "NET_PRES_SUPPORT_SERVER_ENC"?eval>
		<#assign supportClient=  "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
    </#list>

static const NET_PRES_INST_DATA netPresCfgs[] = 
{  
    <#list 0..(numInstance-1) as idx>
        
    {
            <#assign useStream=  "NET_PRES_SUPPORT_STREAM"?eval> 
            <#assign useDataGram= "NET_PRES_SUPPORT_DATAGRAM"?eval>
            <#assign useServer= "NET_PRES_SUPPORT_SERVER"?eval>
            <#assign useClient=  "NET_PRES_SUPPORT_CLIENT"?eval>
            <#assign useHarmonyTcp= "NET_PRES_TRANSPORT_AS_TCPIP"?eval>
            <#if useHarmonyTcp && useStream && useServer && ((tcpipTcp.TCPIP_USE_TCP)?has_content &&  (tcpipTcp.TCPIP_USE_TCP) == true)>        
        .pTransObject_ss = &netPresTransObject${idx}SS,
            </#if>
            <#if useHarmonyTcp && useStream && useClient && ((tcpipTcp.TCPIP_USE_TCP)?has_content &&  (tcpipTcp.TCPIP_USE_TCP) == true)>        
        .pTransObject_sc = &netPresTransObject${idx}SC,
            </#if>
            <#if useHarmonyTcp && useDataGram && useServer && ((tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true)>        
        .pTransObject_ds = &netPresTransObject${idx}DS,
            </#if>
            <#if useHarmonyTcp && useDataGram && useClient && ((tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true)>        
        .pTransObject_dc = &netPresTransObject${idx}DC,
            </#if>
            <#if supportEncryption && supportStream && supportServer >
        .pProvObject_ss = &net_pres_EncProviderStreamServer${idx},
            <#else>
        .pProvObject_ss = NULL,
            </#if>
            <#if supportEncryption && supportStream && supportClient>
        .pProvObject_sc = &net_pres_EncProviderStreamClient${idx},
            <#else>
        .pProvObject_sc = NULL,
            </#if>
            <#if supportEncryption && supportDataGram && supportServer>
        .pProvObject_ds = &net_pres_EncProviderDataGramServer${idx},
            <#else>
        .pProvObject_ds = NULL,
            </#if>
            <#if supportEncryption && supportDataGram && supportClient>
        .pProvObject_dc = &net_pres_EncProviderDataGramClient${idx},
            <#else>
        .pProvObject_dc = NULL,
            </#if>
    },
        
     </#list>
};

static const NET_PRES_INIT_DATA netPresInitData = 
{
    .numLayers = sizeof(netPresCfgs) / sizeof(NET_PRES_INST_DATA),
    .pInitData = netPresCfgs
};
  
 
</#if>
