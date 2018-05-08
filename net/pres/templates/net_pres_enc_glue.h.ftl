/*******************************************************************************
 Header file for the wolfSSL glue functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*******************************************************************************
File Name: net_tls_wolfssl_glue.h
Copyright (c) 2013 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

#ifndef _NET_TLS_WOLFSSL_GLUE_H_
#define _NET_TLS_WOLFSSL_GLUE_H_

#include "system_config.h"
#include "net/pres/net_pres.h"
#include "net/pres/net_pres_encryptionproviderapi.h"
#ifdef __CPLUSPLUS
extern "C" {
#endif
<#macro netPresEncGlueHeader
    INST_NUMBER>
    <#if .vars["CONFIG_NET_PRES_SUPPORT_ENCRYPTION${INST_NUMBER}"]>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamServer${INST_NUMBER};
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamClient${INST_NUMBER};
            </#if>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderDataGramServer${INST_NUMBER};
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
extern NET_PRES_EncProviderObject net_pres_EncProviderDataGramClient${INST_NUMBER};
            </#if>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderStreamServerInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamServerDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderStreamServerOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamServerIsInited${INST_NUMBER}();
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderStreamClientInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamClientDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderStreamClientOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamClientIsInited${INST_NUMBER}();
            </#if>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderDataGramServerInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderDataGramServerDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderDataGramServerOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderDataGramServerIsInited${INST_NUMBER}();
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
bool NET_PRES_EncProviderDataGramClientInit${INST_NUMBER}(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderDataGramClientDeinit${INST_NUMBER}();
bool NET_PRES_EncProviderDataGramClientOpen${INST_NUMBER}(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderDataGramClientIsInited${INST_NUMBER}();
            </#if>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
NET_PRES_EncSessionStatus NET_PRES_EncProviderServerAccept${INST_NUMBER}(void * providerData);
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
NET_PRES_EncSessionStatus NET_PRES_EncProviderClientConnect${INST_NUMBER}(void * providerData);
        </#if>
NET_PRES_EncSessionStatus NET_PRES_EncProviderConnectionClose${INST_NUMBER}(void * providerData);
int32_t NET_PRES_EncProviderWrite${INST_NUMBER}(void * providerData, const uint8_t * buffer, uint16_t size);
uint16_t  NET_PRES_EncProviderWriteReady${INST_NUMBER}(void * providerData, uint16_t reqSize, uint16_t minSize);
int32_t NET_PRES_EncProviderRead${INST_NUMBER}(void * providerData, uint8_t * buffer, uint16_t size);
int32_t NET_PRES_EncProviderReadReady${INST_NUMBER}(void * providerData);
int32_t NET_PRES_EncProviderPeek${INST_NUMBER}(void * providerData, uint8_t * buffer, uint16_t size);
    </#if>
</#macro>
<#list 0..(CONFIG_NET_PRES_INSTANCES?number-1) as idx>
    <@netPresEncGlueHeader idx/>
</#list>
#ifdef __CPLUSPLUS
}
#endif
#endif //_NET_TLS_WOLFSSL_GLUE_H_
