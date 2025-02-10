/*******************************************************************************
 Header file for the wolfSSL glue functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*
Copyright (C) 2013-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/


#ifndef H_NET_TLS_WOLFSSL_GLUE_H_
#define H_NET_TLS_WOLFSSL_GLUE_H_

#include "configuration.h"
#include "net_pres/pres/net_pres.h"
#include "net_pres/pres/net_pres_encryptionproviderapi.h"
#ifdef __CPLUSPLUS
extern "C" {
#endif
<#macro netPresEncGlueHeader
    INST_NUMBER>
	<#assign netPresSuppEnc = "NET_PRES_SUPPORT_ENCRYPTION"?eval>
	<#if netPresSuppEnc?has_content && netPresSuppEnc == true>
		<#assign netPresSuppStream = "NET_PRES_SUPPORT_STREAM_ENC"?eval>
		<#if netPresSuppStream?has_content && netPresSuppStream == true>
			<#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>           
extern Net_ProvObject net_ProvStreamServer${INST_NUMBER};
            </#if>
			<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>            
extern Net_ProvObject net_ProvStreamClient${INST_NUMBER};
            </#if>
        </#if>
		<#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>          
extern Net_ProvObject net_ProvDataGramServer${INST_NUMBER};
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>            
extern Net_ProvObject net_ProvDataGramClient${INST_NUMBER};
            </#if>
        </#if>
        <#assign netPresSuppStream = "NET_PRES_SUPPORT_STREAM_ENC"?eval>
		<#if netPresSuppStream?has_content && netPresSuppStream == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>            
bool Net_ProvStreamServerInit${INST_NUMBER}(struct S_NET_PRES_TransportObject * transObject);
bool Net_ProvStreamServerDeinit${INST_NUMBER}(void);
bool Net_ProvStreamServerOpen${INST_NUMBER}(SYS_MODULE_OBJ obj, uintptr_t presHandle, uintptr_t transHandle, void * providerData);
bool Net_ProvStreamServerIsInited${INST_NUMBER}(void);
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>             
bool Net_ProvStreamClientInit${INST_NUMBER}(struct S_NET_PRES_TransportObject * transObject);
bool Net_ProvStreamClientDeinit${INST_NUMBER}(void);
bool Net_ProvStreamClientOpen${INST_NUMBER}(SYS_MODULE_OBJ obj, uintptr_t presHandle, uintptr_t transHandle, void * providerData);
bool Net_ProvStreamClientIsInited${INST_NUMBER}(void);
				<#if NET_PRES_BLOB_RUNTIME_CERT_SUPPORT == true>
bool NET_PRES_SetCertificate(unsigned char* in, long sz, int format);
				</#if>
            </#if>
        </#if>
        <#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>           
bool Net_ProvDataGramServerInit${INST_NUMBER}(struct S_NET_PRES_TransportObject * transObject);
bool Net_ProvDataGramServerDeinit${INST_NUMBER}(void);
bool Net_ProvDataGramServerOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool Net_ProvDataGramServerIsInited${INST_NUMBER}(void);
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>             
bool Net_ProvDataGramClientInit${INST_NUMBER}(struct S_NET_PRES_TransportObject * transObject);
bool Net_ProvDataGramClientDeinit${INST_NUMBER}(void);
bool Net_ProvDataGramClientOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool Net_ProvDataGramClientIsInited${INST_NUMBER}(void);
            </#if>
        </#if>
        <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
		<#if netPresSuppServer?has_content && netPresSuppServer == true>            
NET_PRES_EncSessionStatus Net_ProvServerAccept${INST_NUMBER}(void * providerData);
        </#if>
        <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
		<#if netPresSuppClient?has_content && netPresSuppClient == true>          
NET_PRES_EncSessionStatus Net_ProvClientConnect${INST_NUMBER}(void * providerData);
        </#if>
NET_PRES_EncSessionStatus Net_ProvConnectionClose${INST_NUMBER}(void * providerData);
int32_t Net_ProvWrite${INST_NUMBER}(void * providerData, const uint8_t * dataBuff, uint16_t size);
uint16_t  Net_ProvWriteReady${INST_NUMBER}(void * providerData, uint16_t reqSize, uint16_t minSize);
int32_t Net_ProvRead${INST_NUMBER}(void * providerData, uint8_t * dataBuff, uint16_t size);
int32_t Net_ProvReadReady${INST_NUMBER}(void * providerData);
int32_t Net_ProvPeek${INST_NUMBER}(void * providerData, uint8_t * dataBuff, uint16_t size);
int32_t Net_ProvOutputSize${INST_NUMBER}(void * providerData, int32_t inSize);
int32_t Net_ProvMaxOutputSize${INST_NUMBER}(void * providerData);
	<#if ((lib_wolfssl.wolfsslTlsSni?has_content) && (lib_wolfssl.wolfsslTlsSni) == true)>
        <#assign netPresSNICbackEn= "NET_PRES_SNI_CALLBACK_ENABLE"?eval>
        <#if (netPresSNICbackEn?has_content) && (netPresSNICbackEn == true)>
            <#lt>
        <#else>
            <#lt>#define NET_PRES_SNI_HOST_NAME		"${NET_PRES_SUPPORT_SNI_HOST_NAME}"
        </#if>
	</#if>
	<#if ((lib_wolfssl.wolfsslTlsAlpn?has_content) && (lib_wolfssl.wolfsslTlsAlpn) == true)>
		<#lt>#define NET_PRES_ALPN_PROTOCOL_NAME_LIST		"${NET_PRES_SUPPORT_ALPN_PROTOCOL_NAME}"
	</#if>
    </#if>
</#macro>

<#assign numInstance= 1>
<#list 0..(numInstance-1) as idx>	
    <@netPresEncGlueHeader idx/>
</#list>
#ifdef __CPLUSPLUS
}
#endif
#endif //H_NET_TLS_WOLFSSL_GLUE_H_
