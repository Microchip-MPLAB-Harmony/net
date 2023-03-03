/*******************************************************************************
 Header file for the wolfSSL glue functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*
Copyright (C) 2013-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


#ifndef _NET_TLS_WOLFSSL_GLUE_H_
#define _NET_TLS_WOLFSSL_GLUE_H_

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
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamServer${INST_NUMBER};
            </#if>
			<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>            
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamClient${INST_NUMBER};
            </#if>
        </#if>
		<#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>          
extern NET_PRES_EncProviderObject net_pres_EncProviderDataGramServer${INST_NUMBER};
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>            
extern NET_PRES_EncProviderObject net_pres_EncProviderDataGramClient${INST_NUMBER};
            </#if>
        </#if>
        <#assign netPresSuppStream = "NET_PRES_SUPPORT_STREAM_ENC"?eval>
		<#if netPresSuppStream?has_content && netPresSuppStream == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>            
bool NET_PRES_EncProviderStreamServerInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamServerDeinit${INST_NUMBER}(void);
bool NET_PRES_EncProviderStreamServerOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamServerIsInited${INST_NUMBER}(void);
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>             
bool NET_PRES_EncProviderStreamClientInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamClientDeinit${INST_NUMBER}(void);
bool NET_PRES_EncProviderStreamClientOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamClientIsInited${INST_NUMBER}(void);
				<#if NET_PRES_BLOB_RUNTIME_CERT_SUPPORT == true>
bool NET_PRES_SetCertificate(unsigned char* in, long sz, int format);
				</#if>
            </#if>
        </#if>
        <#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>           
bool NET_PRES_EncProviderDataGramServerInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderDataGramServerDeinit${INST_NUMBER}(void);
bool NET_PRES_EncProviderDataGramServerOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderDataGramServerIsInited${INST_NUMBER}(void);
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>             
bool NET_PRES_EncProviderDataGramClientInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderDataGramClientDeinit${INST_NUMBER}(void);
bool NET_PRES_EncProviderDataGramClientOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderDataGramClientIsInited${INST_NUMBER}(void);
            </#if>
        </#if>
        <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
		<#if netPresSuppServer?has_content && netPresSuppServer == true>            
NET_PRES_EncSessionStatus NET_PRES_EncProviderServerAccept${INST_NUMBER}(void * providerData);
        </#if>
        <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
		<#if netPresSuppClient?has_content && netPresSuppClient == true>          
NET_PRES_EncSessionStatus NET_PRES_EncProviderClientConnect${INST_NUMBER}(void * providerData);
        </#if>
NET_PRES_EncSessionStatus NET_PRES_EncProviderConnectionClose${INST_NUMBER}(void * providerData);
int32_t NET_PRES_EncProviderWrite${INST_NUMBER}(void * providerData, const uint8_t * buffer, uint16_t size);
uint16_t  NET_PRES_EncProviderWriteReady${INST_NUMBER}(void * providerData, uint16_t reqSize, uint16_t minSize);
int32_t NET_PRES_EncProviderRead${INST_NUMBER}(void * providerData, uint8_t * buffer, uint16_t size);
int32_t NET_PRES_EncProviderReadReady${INST_NUMBER}(void * providerData);
int32_t NET_PRES_EncProviderPeek${INST_NUMBER}(void * providerData, uint8_t * buffer, uint16_t size);
int32_t NET_PRES_EncProviderOutputSize${INST_NUMBER}(void * providerData, int32_t inSize);
int32_t NET_PRES_EncProviderMaxOutputSize${INST_NUMBER}(void * providerData);
	<#if ((lib_wolfssl.wolfsslTlsSni?has_content) && (lib_wolfssl.wolfsslTlsSni) == true)>
		<#lt>#define NET_PRES_SNI_HOST_NAME		"${NET_PRES_SUPPORT_SNI_HOST_NAME}"
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
#endif //_NET_TLS_WOLFSSL_GLUE_H_
