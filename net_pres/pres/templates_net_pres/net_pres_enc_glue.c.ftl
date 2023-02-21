 /*******************************************************************************
 Source file for the Net Pres Encryption glue functions to work with Harmony


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


#include "net_pres_enc_glue.h"
#include "net_pres/pres/net_pres_transportapi.h"
#include "net_pres/pres/net_pres_certstore.h"

<#if (lib_wolfssl.wolfssl)?has_content>
#include "config.h"
#include "wolfssl/ssl.h"
#include "wolfssl/wolfcrypt/logging.h"
#include "wolfssl/wolfcrypt/random.h"

<#if NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX0>	
#include "${NET_PRES_BLOB_MORE_CLIENT_CERT_FILENAME_IDX0}"
</#if>
<#if NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX1>	
#include "${NET_PRES_BLOB_MORE_CLIENT_CERT_FILENAME_IDX1}"
</#if>
<#if NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX2>	
#include "${NET_PRES_BLOB_MORE_CLIENT_CERT_FILENAME_IDX2}"
</#if>

extern  int CheckAvailableSize(WOLFSSL *ssl, int size);
<#if (lib_wolfssl.wolfsslLoadTNGTLSCert)?has_content && ((lib_wolfssl.wolfsslLoadTNGTLSCert) == true)>
#include "wolfssl/wolfcrypt/port/atmel/atmel.h"
</#if>

<#assign needSysConsole=false/>
<#assign numInstance= 1>
<#list 0..(numInstance-1) as idx>	
	<#assign netPresUseWolfSSLDebug = "NET_PRES_USE_WOLF_SSL_DEBUG_LOG"?eval>
	<#if netPresUseWolfSSLDebug?has_content && netPresUseWolfSSLDebug == true>
		<#assign needSysConsole=true/>
    </#if>
</#list>
<#if needSysConsole == true>
#include "system/debug/sys_debug.h"
</#if>

</#if>
<#macro NET_PRES_ENC_PROV_INFOS
    INST_NUMBER>
	<#assign netPresSuppEnc = "NET_PRES_SUPPORT_ENCRYPTION"?eval>
    <#if netPresSuppEnc?has_content && netPresSuppEnc == true>
		<#assign netPresSuppStream = "NET_PRES_SUPPORT_STREAM_ENC"?eval>
		<#if netPresSuppStream?has_content && netPresSuppStream == true>
			<#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>
NET_PRES_EncProviderObject net_pres_EncProviderStreamServer${INST_NUMBER} =
{
    .fpInit =    NET_PRES_EncProviderStreamServerInit${INST_NUMBER},
    .fpDeinit =  NET_PRES_EncProviderStreamServerDeinit${INST_NUMBER},
    .fpOpen =    NET_PRES_EncProviderStreamServerOpen${INST_NUMBER},
    .fpConnect = NET_PRES_EncProviderServerAccept${INST_NUMBER},
    .fpClose =   NET_PRES_EncProviderConnectionClose${INST_NUMBER},
    .fpWrite =   NET_PRES_EncProviderWrite${INST_NUMBER},
    .fpWriteReady =   NET_PRES_EncProviderWriteReady${INST_NUMBER},
    .fpRead =    NET_PRES_EncProviderRead${INST_NUMBER},
    .fpReadReady = NET_PRES_EncProviderReadReady${INST_NUMBER},
    .fpPeek =    NET_PRES_EncProviderPeek${INST_NUMBER},
    .fpIsInited = NET_PRES_EncProviderStreamServerIsInited${INST_NUMBER},
    .fpOutputSize = NET_PRES_EncProviderOutputSize${INST_NUMBER},
    .fpMaxOutputSize = NET_PRES_EncProviderMaxOutputSize${INST_NUMBER},

};
            </#if>
			<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>          
NET_PRES_EncProviderObject net_pres_EncProviderStreamClient${INST_NUMBER} = 
{
    .fpInit =    NET_PRES_EncProviderStreamClientInit${INST_NUMBER},
    .fpDeinit =  NET_PRES_EncProviderStreamClientDeinit${INST_NUMBER},
    .fpOpen =    NET_PRES_EncProviderStreamClientOpen${INST_NUMBER},
    .fpConnect = NET_PRES_EncProviderClientConnect${INST_NUMBER},
    .fpClose =   NET_PRES_EncProviderConnectionClose${INST_NUMBER},
    .fpWrite =   NET_PRES_EncProviderWrite${INST_NUMBER},
    .fpWriteReady =   NET_PRES_EncProviderWriteReady${INST_NUMBER},
    .fpRead =    NET_PRES_EncProviderRead${INST_NUMBER},
    .fpReadReady = NET_PRES_EncProviderReadReady${INST_NUMBER},
    .fpPeek =    NET_PRES_EncProviderPeek${INST_NUMBER},
    .fpIsInited = NET_PRES_EncProviderStreamClientIsInited${INST_NUMBER},
    .fpOutputSize = NET_PRES_EncProviderOutputSize${INST_NUMBER},
    .fpMaxOutputSize = NET_PRES_EncProviderMaxOutputSize${INST_NUMBER},
};
            </#if>
        </#if>
		<#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
			<#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>         
NET_PRES_EncProviderObject net_pres_EncProviderDataGramServer${INST_NUMBER} =
{
    .fpInit =    NET_PRES_EncProviderDataGramServerInit${INST_NUMBER},
    .fpDeinit =  NET_PRES_EncProviderDataGramServerDeinit${INST_NUMBER},
    .fpOpen =    NET_PRES_EncProviderDataGramServerOpen${INST_NUMBER},
    .fpConnect = NET_PRES_EncProviderServerAccept${INST_NUMBER},
    .fpClose =   NET_PRES_EncProviderConnectionClose${INST_NUMBER},
    .fpWrite =   NET_PRES_EncProviderWrite${INST_NUMBER},
    .fpWriteReady =   NET_PRES_EncProviderWriteReady${INST_NUMBER},
    .fpRead =    NET_PRES_EncProviderRead${INST_NUMBER},
    .fpReadReady = NET_PRES_EncProviderReadReady${INST_NUMBER},
    .fpPeek =    NET_PRES_EncProviderPeek${INST_NUMBER},
    .fpIsInited = NET_PRES_EncProviderDataGramServerIsInited${INST_NUMBER},
    .fpOutputSize = NET_PRES_EncProviderOutputSize${INST_NUMBER},
    .fpMaxOutputSize = NET_PRES_EncProviderMaxOutputSize${INST_NUMBER},
};
            </#if>
			<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
            <#if netPresSuppClient?has_content && netPresSuppClient == true>             
NET_PRES_EncProviderObject net_pres_EncProviderDataGramClient${INST_NUMBER} =
{
    .fpInit =    NET_PRES_EncProviderDataGramClientInit${INST_NUMBER},
    .fpDeinit =  NET_PRES_EncProviderDataGramClientDeinit${INST_NUMBER},
    .fpOpen =    NET_PRES_EncProviderDataGramClientOpen${INST_NUMBER},
    .fpConnect = NET_PRES_EncProviderClientConnect${INST_NUMBER},
    .fpClose =   NET_PRES_EncProviderConnectionClose${INST_NUMBER},
    .fpWrite =   NET_PRES_EncProviderWrite${INST_NUMBER},
    .fpWriteReady =   NET_PRES_EncProviderWriteReady${INST_NUMBER},
    .fpRead =    NET_PRES_EncProviderRead${INST_NUMBER},
    .fpReadReady = NET_PRES_EncProviderReadReady${INST_NUMBER},
    .fpPeek =    NET_PRES_EncProviderPeek${INST_NUMBER},
    .fpIsInited = NET_PRES_EncProviderDataGramClientIsInited${INST_NUMBER},
    .fpOutputSize = NET_PRES_EncProviderOutputSize${INST_NUMBER},
    .fpMaxOutputSize = NET_PRES_EncProviderMaxOutputSize${INST_NUMBER},
};
            </#if>
        </#if>
    </#if>
</#macro>
<#macro NET_PRES_ENC_GLUE_INIT
        INST
        CONNECTION
        TYPE>

<#assign netPresUseWolfSSLDebugLog= "NET_PRES_USE_WOLF_SSL_DEBUG_LOG"?eval>
<#if netPresUseWolfSSLDebugLog?has_content && netPresUseWolfSSLDebugLog == true>
void NET_PRES_EncProvider${TYPE}${CONNECTION}Log${INST}(int level, const char * message)
{
	<#assign num = "NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS"?eval>
	static char buffer[${num}][120];
	static int bufNum = 0;
	if (level > 2)
	{
		return;
	}
	snprintf(buffer[bufNum], 120, "wolfSSL (%d): %s\r\n", level, message);
	SYS_CONSOLE_MESSAGE(buffer[bufNum]);
	bufNum ++;
	if (bufNum == ${num})
	{
		bufNum = 0;
	}
}
</#if>
		
bool NET_PRES_EncProvider${TYPE}${CONNECTION}Init${INST}(NET_PRES_TransportObject * transObject)
{
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
        <#if CONNECTION="Client">
    const uint8_t * caCertsPtr;
    int32_t caCertsLen;
	            <#if NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT>
    const uint8_t * deviceCertPtr;
    const uint8_t * pvtKeyPtr;
    int32_t deviceCertLen, pvtKeyLen;
    if (!NET_PRES_CertStoreGetDeviceTlsParams(&deviceCertPtr, &deviceCertLen, &pvtKeyPtr, &pvtKeyLen, ${INST}))
    {
        return false;
    }
            </#if>
    if (!NET_PRES_CertStoreGetCACerts(&caCertsPtr, &caCertsLen, ${INST}))
    {
        return false;
    }
        <#else>
    const uint8_t * serverCertPtr, *serverKeyPtr;
    int32_t serverCertLen, serverKeyLen;
    if (!NET_PRES_CertStoreGetServerCert(&serverCertPtr, &serverCertLen, &serverKeyPtr, &serverKeyLen, ${INST}))
    {
        return false;
    }
        </#if>
    if (_net_pres_wolfsslUsers == 0)
    {
        wolfSSL_Init();
		<#assign netPresUseWolfSSLDebugLog= "NET_PRES_USE_WOLF_SSL_DEBUG_LOG"?eval>
		<#if netPresUseWolfSSLDebugLog?has_content && netPresUseWolfSSLDebugLog == true>
		wolfSSL_SetLoggingCb(NET_PRES_EncProvider${TYPE}${CONNECTION}Log${INST});
		wolfSSL_Debugging_ON();
		</#if>
        _net_pres_wolfsslUsers++;
    }
    net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject = transObject;
        <#if TYPE="Stream">
            <#if CONNECTION="Client">
	net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context = wolfSSL_CTX_new(wolfSSLv23_client_method());
            <#else>
	net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context = wolfSSL_CTX_new(wolfSSLv23_server_method());
            </#if>
        <#else>
            <#if CONNECTION="Client">
    net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context = wolfSSL_CTX_new(wolfDTLSv1_client_method());
            <#else>
    net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context = wolfSSL_CTX_new(wolfDTLSv1_server_method());
            </#if>
        </#if>
    if (net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context == 0)
    {
        return false;
    }
    <#if NET_PRES_BLOB_ENABLE_PEER_CERT_VERIFICATION == false>
    // Turn off verification, because SNTP is usually blocked by a firewall
    wolfSSL_CTX_set_verify(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, SSL_VERIFY_NONE, 0);
		<#else>
	wolfSSL_CTX_set_verify(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, WOLFSSL_VERIFY_PEER, 0);
		</#if>
	
    wolfSSL_SetIORecv(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, (CallbackIORecv)&NET_PRES_EncGlue_${TYPE}${CONNECTION}ReceiveCb${INST});
    wolfSSL_SetIOSend(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, (CallbackIOSend)&NET_PRES_EncGlue_${TYPE}${CONNECTION}SendCb${INST});
        <#if CONNECTION="Client">
    if (wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, caCertsPtr, caCertsLen, SSL_FILETYPE_${NET_PRES_BLOB_CLIENT_CERT_FORMAT}) != SSL_SUCCESS)
    {
        // Couldn't load the CA certificates
        //SYS_CONSOLE_MESSAGE("Something went wrong loading the CA certificates\r\n");
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
<#if NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX0>
	{	
		const uint8_t *tmpCaCertsPtr = ${NET_PRES_BLOB_MORE_CLIENT_CERT_VARIABLE_IDX0};
		int32_t tmpCaCertsLen = ${NET_PRES_BLOB_MORE_CLIENT_CERT_LEN_VARIABLE_IDX0};

		if (wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, tmpCaCertsPtr, tmpCaCertsLen, SSL_FILETYPE_${NET_PRES_BLOB_MORE_CLIENT_CERT_FORMAT_IDX0}) != SSL_SUCCESS)
		{
			// Couldn't load the CA certificates
			//SYS_CONSOLE_MESSAGE("Something went wrong loading the CA certificates\r\n");
			wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
			return false;
		}
	}
</#if>

<#if NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX1>	
	{
		const uint8_t *tmpCaCertsPtr = ${NET_PRES_BLOB_MORE_CLIENT_CERT_VARIABLE_IDX1};
		int32_t tmpCaCertsLen = ${NET_PRES_BLOB_MORE_CLIENT_CERT_LEN_VARIABLE_IDX1};

		if (wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, tmpCaCertsPtr, tmpCaCertsLen, SSL_FILETYPE_${NET_PRES_BLOB_MORE_CLIENT_CERT_FORMAT_IDX1}) != SSL_SUCCESS)
		{
			// Couldn't load the CA certificates
			//SYS_CONSOLE_MESSAGE("Something went wrong loading the CA certificates\r\n");
			wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
			return false;
		}
	}
</#if>

<#if NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX2>	
	{
		const uint8_t *tmpCaCertsPtr = ${NET_PRES_BLOB_MORE_CLIENT_CERT_VARIABLE_IDX2};
		int32_t tmpCaCertsLen = ${NET_PRES_BLOB_MORE_CLIENT_CERT_LEN_VARIABLE_IDX2};

		if (wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, tmpCaCertsPtr, tmpCaCertsLen, SSL_FILETYPE_${NET_PRES_BLOB_MORE_CLIENT_CERT_FORMAT_IDX2}) != SSL_SUCCESS)
		{
			// Couldn't load the CA certificates
			//SYS_CONSOLE_MESSAGE("Something went wrong loading the CA certificates\r\n");
			wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
			return false;
		}
	}
</#if>
			<#if NET_PRES_BLOB_CERT_REPO>
                <#if NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT>
<#if NET_PRES_BLOB_CLIENT_IS_DEVICE_CERT_CHAIN>
    if (wolfSSL_CTX_use_certificate_chain_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, deviceCertPtr, deviceCertLen) != SSL_SUCCESS)
    {
        // Couldn't load the device certificates
        //SYS_CONSOLE_MESSAGE("Something went wrong loading the device certificates\r\n");
        wolfSSL_CTX_free(net_pres_wolfSSLInfoStreamClient0.context);
        return false;
    }
		
<#else>
    if (wolfSSL_CTX_use_certificate_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, deviceCertPtr, deviceCertLen, SSL_FILETYPE_${NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_FORMAT}) != SSL_SUCCESS)
    {
        // Couldn't load the device certificates
        //SYS_CONSOLE_MESSAGE("Something went wrong loading the device certificates\r\n");
        wolfSSL_CTX_free(net_pres_wolfSSLInfoStreamClient0.context);
        return false;
    }
</#if>
    if (wolfSSL_CTX_use_PrivateKey_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, pvtKeyPtr, pvtKeyLen, SSL_FILETYPE_${NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_FORMAT}) != SSL_SUCCESS)
    {
        // Couldn't load the device private key
        //SYS_CONSOLE_MESSAGE("Something went wrong loading the device private key\r\n");
        wolfSSL_CTX_free(net_pres_wolfSSLInfoStreamClient0.context);
        return false;
    }
                </#if>
            </#if>
        <#else>
    if (wolfSSL_CTX_use_certificate_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, serverCertPtr, serverCertLen, SSL_FILETYPE_${NET_PRES_BLOB_SERVER_CERT_FORMAT}) != SSL_SUCCESS)
    {
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
    if (wolfSSL_CTX_use_PrivateKey_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, serverKeyPtr, serverKeyLen, SSL_FILETYPE_${NET_PRES_BLOB_SERVER_CERT_FORMAT}) != SSL_SUCCESS)
    {
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
        </#if>
    <#if (lib_wolfssl.wolfsslLoadTNGTLSCert)?has_content && ((lib_wolfssl.wolfsslLoadTNGTLSCert) == true)>
    /*initialize Trust*Go and load device certificate into the context*/
    atcatls_set_callbacks(net_pres_wolfSSLInfoStreamClient0.context);
    /*Use TLS extension since we support only P256R1 with ECC608 Trust&Go*/
    if (WOLFSSL_SUCCESS != wolfSSL_CTX_UseSupportedCurve(net_pres_wolfSSLInfoStreamClient0.context, WOLFSSL_ECC_SECP256R1)) {
        return false;
    }
	</#if>
    net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.isInited = true;
    return true;
    <#else>
    //TODO: Enter in code to initialize the provider
    return false;
    </#if>
}
</#macro> 
<#macro NET_PRES_ENC_GLUE_DEINIT
        INST
        CONNECTION
        TYPE>
bool NET_PRES_EncProvider${TYPE}${CONNECTION}Deinit${INST}(void)
{
	<#if (lib_wolfssl.wolfsslLoadTNGTLSCert)?has_content && ((lib_wolfssl.wolfsslLoadTNGTLSCert) == true)>
    atmel_finish();
    </#if>
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
    net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.isInited = false;
    _net_pres_wolfsslUsers--;
    if (_net_pres_wolfsslUsers == 0)
    {
        wolfSSL_Cleanup();
    }
    return true;
    <#else>
    //TODO: Enter in code to deinitialize the provider
    return false;
    </#if>
}
</#macro> 
<#macro NET_PRES_ENC_GLUE_OPEN
        INST
        CONNECTION
        TYPE>
bool NET_PRES_EncProvider${TYPE}${CONNECTION}Open${INST}(uintptr_t transHandle, void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
        WOLFSSL* ssl = wolfSSL_new(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        if (ssl == NULL)
        {
            return false;
        }
        if (wolfSSL_set_fd(ssl, transHandle) != SSL_SUCCESS)
        {
            wolfSSL_free(ssl);
            return false;
        }
		<#if ((lib_wolfssl.wolfsslTlsSni?has_content) && (lib_wolfssl.wolfsslTlsSni) == true)>
        if (wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, NET_PRES_SNI_HOST_NAME, strlen(NET_PRES_SNI_HOST_NAME)) != WOLFSSL_SUCCESS)
        {
            return false;
        }
		</#if>
		<#if ((lib_wolfssl.wolfsslTlsAlpn?has_content) && (lib_wolfssl.wolfsslTlsAlpn) == true)>
        if (wolfSSL_UseALPN(ssl, NET_PRES_ALPN_PROTOCOL_NAME_LIST, sizeof(NET_PRES_ALPN_PROTOCOL_NAME_LIST),WOLFSSL_ALPN_FAILED_ON_MISMATCH) != WOLFSSL_SUCCESS)
        {
            return false;
        }
		</#if>
        memcpy(providerData, &ssl, sizeof(WOLFSSL*));
        return true;
    <#else>
    //TODO: Enter in code to open a connection with the provider
    return false;
    </#if>
}
</#macro> 
<#macro NET_PRES_ENC_GLUE_CONNECT
        INST
        CONNECTION>
    <#if CONNECTION=="Server">
        <#assign ACCEPT="Accept"/>
    <#else>
        <#assign ACCEPT="Connect"/>
    </#if>
NET_PRES_EncSessionStatus NET_PRES_EncProvider${CONNECTION}${ACCEPT}${INST}(void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
        <#if CONNECTION=="Server">
    int result = wolfSSL_accept(ssl);
        <#else>
    int result = wolfSSL_connect(ssl);
        </#if>
    switch (result)
    {
        case SSL_SUCCESS:
            return NET_PRES_ENC_SS_OPEN;
        default:
        {
            int error = wolfSSL_get_error(ssl, result);
            switch (error)
            {
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
        <#if CONNECTION=="Server">
                    return NET_PRES_ENC_SS_SERVER_NEGOTIATING;
        <#else>
                    return NET_PRES_ENC_SS_CLIENT_NEGOTIATING;
        </#if>
                default:
                    return NET_PRES_ENC_SS_FAILED;
            }
        }
    }
    <#else>
    //TODO: Enter in code to ${ACCEPT} a connection through the provider
    return NET_PRES_ENC_SS_FAILED;
    </#if>
}
</#macro> 
<#macro NET_PRES_ENC_GLUE_CLOSE
        INST>
NET_PRES_EncSessionStatus NET_PRES_EncProviderConnectionClose${INST}(void * providerData)
{
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
    wolfSSL_free(ssl);
    return NET_PRES_ENC_SS_CLOSED;
    <#else>
    //TODO: Enter in code to close a connection through provider
    return false;
    </#if>
}
</#macro> 
<#macro NET_PRES_ENC_GLUE_WRITE
        INST>
int32_t NET_PRES_EncProviderWrite${INST}(void * providerData, const uint8_t * buffer, uint16_t size)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_write(ssl, buffer, size);
    if (ret < 0)
    {
        return 0;
    }    
    return ret;
    <#else>
    //TODO: Enter in  code to write data through the provider
    return 0;
    </#if>
}
</#macro> 

<#macro NET_PRES_ENC_GLUE_WRITE_READY
        INST>
uint16_t NET_PRES_EncProviderWriteReady${INST}(void * providerData, uint16_t reqSize, uint16_t minSize)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    
    char buffer;
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));

    int ret = wolfSSL_write(ssl, &buffer, 0);
    if(ret < 0)
    {
        return 0;
    }

    ret = CheckAvailableSize(ssl, reqSize);
    if(ret == 0)
    {   // success
        return reqSize;
    }
    if(minSize != 0)
    {
        ret = CheckAvailableSize(ssl, minSize);
        if(ret == 0)
        {   // success
            return minSize;
        }
    }

    return 0;
    <#else>
    //TODO: Enter in  code to check write ready through the provider
    return 0;
    </#if>
}
</#macro> 

<#macro NET_PRES_ENC_GLUE_READ
        INST>
int32_t NET_PRES_EncProviderRead${INST}(void * providerData, uint8_t * buffer, uint16_t size)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_read(ssl, buffer, size);
    if (ret < 0)
    {
        return 0;
    } 
    return ret;
    <#else>
    //TODO: Enter in  code to read data from the provider
    return 0;
    </#if>
}
</#macro> 

<#macro NET_PRES_ENC_GLUE_READ_READY
        INST>

int32_t NET_PRES_EncProviderReadReady${INST}(void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
    int32_t ret = wolfSSL_pending(ssl);
    if (ret == 0) // wolfSSL_pending() doesn't check the underlying layer.
    {
        char buffer;
        if (wolfSSL_peek(ssl, &buffer, 1) == 0)
        {
            return 0;
        }
        ret = wolfSSL_pending(ssl);
    }
    return ret;
    <#else>
    //TODO: Enter in  code to read data from the provider
    return 0;
    </#if>
}
        
</#macro>

<#macro NET_PRES_ENC_GLUE_PEEK
        INST>
int32_t NET_PRES_EncProviderPeek${INST}(void * providerData, uint8_t * buffer, uint16_t size)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_peek(ssl, buffer, size);
    if (ret < 0)
    {
        return 0;
    }  
    return ret;
    <#else>
    //TODO: Enter in  code to peek at data held by provider
    return 0;
    </#if>
}
</#macro> 

<#macro NET_PRES_ENC_GLUE_OUT_SIZE
        INST>
int32_t NET_PRES_EncProviderOutputSize${INST}(void * providerData, int32_t inSize)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_GetOutputSize(ssl, inSize);
    if (ret < 0)
    {
        return 0;
    }  
    return ret;
    <#else>
    //TODO: Enter in  code to get the provider output size
    return 0;
    </#if>
}
</#macro> 

<#macro NET_PRES_ENC_GLUE_MAX_OUT_SIZE
        INST>
int32_t NET_PRES_EncProviderMaxOutputSize${INST}(void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    memcpy(&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_GetMaxOutputSize(ssl);
    if (ret < 0)
    {
        return 0;
    }  
    return ret;
    <#else>
    //TODO: Enter in  code to get the provider max output size
    return 0;
    </#if>
}
</#macro> 

<#macro NET_PRES_ENC_GLUE_IS_INIT
        INST
        CONNECTION
        TYPE>
bool NET_PRES_EncProvider${TYPE}${CONNECTION}IsInited${INST}(void)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    return net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.isInited;
    <#else>
    //TODO: Enter in code to open a connection with the provider
    return false;
    </#if>
}
</#macro> 
<#macro NET_PRES_ENC_GLUE_WOLF_INFO
    INST>
	<#assign netPresSuppEnc = "NET_PRES_SUPPORT_ENCRYPTION"?eval>
    <#if netPresSuppEnc?has_content && netPresSuppEnc == true>
        <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
		<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
			<#assign netPresSuppStream = "NET_PRES_SUPPORT_STREAM_ENC"?eval>
			<#if netPresSuppStream?has_content && netPresSuppStream == true>
				<#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
				<#if netPresSuppServer?has_content && netPresSuppServer == true>         
net_pres_wolfsslInfo net_pres_wolfSSLInfoStreamServer${INST};
                </#if>
				<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
				<#if netPresSuppClient?has_content && netPresSuppClient == true>           
net_pres_wolfsslInfo net_pres_wolfSSLInfoStreamClient${INST};
                </#if>
            </#if>
            <#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
			<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
                <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
				<#if netPresSuppServer?has_content && netPresSuppServer == true>             
net_pres_wolfsslInfo net_pres_wolfSSLInfoDataGramServer${INST};
                </#if>
                <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
				<#if netPresSuppClient?has_content && netPresSuppClient == true>           
net_pres_wolfsslInfo net_pres_wolfSSLInfoDataGramClient${INST};
                </#if>
            </#if>
        </#if>
    </#if>
</#macro>
<#macro NET_PRES_ENC_GLUE_FUNCTIONS
    INST>
	<#assign netPresSuppEnc = "NET_PRES_SUPPORT_ENCRYPTION"?eval>
    <#if netPresSuppEnc?has_content && netPresSuppEnc == true>
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
static uint8_t _net_pres_wolfsslUsers = 0;
	</#if>
        <#assign netPresSuppStream = "NET_PRES_SUPPORT_STREAM_ENC"?eval>
		<#if netPresSuppStream?has_content && netPresSuppStream == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true> 
                <@NET_PRES_ENC_GLUE_INIT INST "Server" "Stream"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Server" "Stream"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Server" "Stream"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Server" "Stream"/>                
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>            
                <@NET_PRES_ENC_GLUE_INIT INST "Client" "Stream"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Client" "Stream"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Client" "Stream"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Client" "Stream"/>                
            </#if>
        </#if>
        <#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
            <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>            
                <@NET_PRES_ENC_GLUE_INIT INST "Server" "DataGram"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Server" "DataGram"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Server" "DataGram"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Server" "DataGram"/>                
            </#if>
            <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>           
                <@NET_PRES_ENC_GLUE_INIT INST "Client" "DataGram"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Client" "DataGram"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Client" "DataGram"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Client" "DataGram"/>                
            </#if>
        </#if>
        <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
		<#if netPresSuppServer?has_content && netPresSuppServer == true>
            <@NET_PRES_ENC_GLUE_CONNECT INST "Server"/>
        </#if>
        <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
		<#if netPresSuppClient?has_content && netPresSuppClient == true>            
            <@NET_PRES_ENC_GLUE_CONNECT INST "Client"/>
        </#if>
        <@NET_PRES_ENC_GLUE_CLOSE INST/>
        <@NET_PRES_ENC_GLUE_WRITE INST/>
        <@NET_PRES_ENC_GLUE_WRITE_READY INST/>
        <@NET_PRES_ENC_GLUE_READ INST/>
        <@NET_PRES_ENC_GLUE_READ_READY INST/>
        <@NET_PRES_ENC_GLUE_PEEK INST/>
        <@NET_PRES_ENC_GLUE_OUT_SIZE INST/>
        <@NET_PRES_ENC_GLUE_MAX_OUT_SIZE INST/>
    </#if>
</#macro>
<#macro NET_PRES_WOLF_CB
    INST
    CONNECTION
    TYPE>
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
int NET_PRES_EncGlue_${TYPE}${CONNECTION}ReceiveCb${INST}(void *sslin, char *buf, int sz, void *ctx)
{
    int fd = *(int *)ctx;
    uint16_t bufferSize;
    bufferSize = (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpReadyToRead)((uintptr_t)fd);
    if (bufferSize == 0)
    {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
    bufferSize = (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpRead)((uintptr_t)fd, (uint8_t*)buf, sz);
    return bufferSize;
}
int NET_PRES_EncGlue_${TYPE}${CONNECTION}SendCb${INST}(void *sslin, char *buf, int sz, void *ctx)
{
    int fd = *(int *)ctx;
    uint16_t bufferSize;
    bufferSize = (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpReadyToWrite)((uintptr_t)fd);
    if (bufferSize == 0)
    {
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
    }

    bufferSize =  (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpWrite)((uintptr_t)fd, (uint8_t*)buf, (uint16_t)sz);
	<#if TYPE = "DataGram">
	(*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpFlush)((uintptr_t)fd);
	</#if>
    return bufferSize;
}
    </#if>
</#macro>
<#macro NET_PRES_WOLF_CBS
    INST>
	<#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
	<#assign netPresSuppStream = "NET_PRES_SUPPORT_STREAM_ENC"?eval>
	<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
	<#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
	<#assign netPresSuppEnc = "NET_PRES_SUPPORT_ENCRYPTION"?eval>
	<#if netPresSuppEnc?has_content && netPresSuppEnc == true>
    <#if netPresSuppServer?has_content && (netPresSuppServer == true) && netPresSuppStream?has_content && (netPresSuppStream == true)>
        <@NET_PRES_WOLF_CB INST "Server" "Stream"/>
    </#if>
    <#if netPresSuppClient?has_content && (netPresSuppClient == true) && netPresSuppStream?has_content && (netPresSuppStream == true)>
        <@NET_PRES_WOLF_CB INST "Client" "Stream"/>
    </#if>
    <#if netPresSuppServer?has_content && (netPresSuppServer == true) && netPresSuppDatagram?has_content && (netPresSuppDatagram == true)>
        <@NET_PRES_WOLF_CB INST "Server" "DataGram"/>
    </#if>
    <#if netPresSuppClient?has_content && (netPresSuppClient == true) && netPresSuppDatagram?has_content && (netPresSuppDatagram == true)>
        <@NET_PRES_WOLF_CB INST "Client" "DataGram"/>
    </#if>
	</#if>
</#macro>
<#if (lib_wolfssl.wolfssl)?has_content && lib_wolfssl.wolfssl == true>
typedef struct 
{
    WOLFSSL_CTX* context;
    NET_PRES_TransportObject * transObject;
    bool isInited;
}net_pres_wolfsslInfo;

// Temporary fix till crypto library is upgraded to recent wolfssl versions.
int  InitRng(RNG* rng)
{
    return wc_InitRng(rng);
}

</#if>
<#assign numInstance= 1>
<#list 0..(numInstance-1) as idx>	
    <@NET_PRES_ENC_PROV_INFOS idx/>
</#list>
<#assign numInstance= 1>
<#list 0..(numInstance-1) as idx>		
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#assign netPresGenEncStub= "NET_PRES_GENERATE_ENC_STUBS"?eval>
	
    <#if (netPresUseWolfSSL?has_content && (netPresUseWolfSSL == true)) || (netPresGenEncStub?has_content && (netPresGenEncStub == true))>
        <@NET_PRES_ENC_GLUE_WOLF_INFO idx/>
    </#if>
</#list>
<#assign numInstance= 1>
<#list 0..(numInstance-1) as idx>	
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#assign netPresGenEncStub= "NET_PRES_GENERATE_ENC_STUBS"?eval>
	
    <#if (netPresUseWolfSSL?has_content && (netPresUseWolfSSL == true)) || (netPresGenEncStub?has_content && (netPresGenEncStub == true))>
        <@NET_PRES_WOLF_CBS idx/>
    </#if>
</#list>
<#assign numInstance= 1>
<#list 0..(numInstance-1) as idx>	
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#assign netPresGenEncStub= "NET_PRES_GENERATE_ENC_STUBS"?eval>
	
    <#if (netPresUseWolfSSL?has_content && (netPresUseWolfSSL == true)) || (netPresGenEncStub?has_content && (netPresGenEncStub == true))>
        <@NET_PRES_ENC_GLUE_FUNCTIONS idx/>
    </#if>
</#list>
