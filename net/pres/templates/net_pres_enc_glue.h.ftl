 /*******************************************************************************
 Header file for the wolfSSL glue functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/


#ifndef _NET_TLS_WOLFSSL_GLUE_H_
#define _NET_TLS_WOLFSSL_GLUE_H_

#include "configuration.h"
#include "net/pres/net_pres.h"
#include "net/pres/net_pres_encryptionproviderapi.h"
#ifdef __CPLUSPLUS
extern "C" {
#endif
<#macro netPresEncGlueHeader
    INST_NUMBER>
	
	<#assign netPresSuppEnc = "NET_PRES_SUPPORT_ENCRYPTION${INST_NUMBER}">
    <#if .vars[netPresSuppEnc]?has_content>
        <#if .vars["NET_PRES_SUPPORT_STREAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamServer${INST_NUMBER};
            </#if>
            <#if .vars["NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamClient${INST_NUMBER};
            </#if>
        </#if>
        <#if .vars["NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderDataGramServer${INST_NUMBER};
            </#if>
            <#if .vars["NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderDataGramClient${INST_NUMBER};
            </#if>
        </#if>
        <#if .vars["NET_PRES_SUPPORT_STREAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderStreamServerInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamServerDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderStreamServerOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamServerIsInited${INST_NUMBER}();
            </#if>
            <#if .vars["NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderStreamClientInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamClientDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderStreamClientOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamClientIsInited${INST_NUMBER}();
            </#if>
        </#if>
        <#if .vars["NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderDataGramServerInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderDataGramServerDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderDataGramServerOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderDataGramServerIsInited${INST_NUMBER}();
            </#if>
            <#if .vars["NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderDataGramClientInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderDataGramClientDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderDataGramClientOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderDataGramClientIsInited${INST_NUMBER}();
            </#if>
        </#if>
        <#if .vars["NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
NET_PRES_EncSessionStatus NET_PRES_EncProviderServerAccept${INST_NUMBER}(void * providerData);
        </#if>
        <#if .vars["NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
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
    </#if>
</#macro>
<#list 0..(__INSTANCE_COUNT?number-1) as idx>
    <@netPresEncGlueHeader idx/>
</#list>
#ifdef __CPLUSPLUS
}
#endif
#endif //_NET_TLS_WOLFSSL_GLUE_H_
