/*******************************************************************************
 Source file for the Net Pres Encryption glue functions to work with Harmony


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
/* MISRA C-2012 Rule 8.5 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_8_5_NET_DR_15 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 8.5" "H3_MISRAC_2012_R_8_5_NET_DR_15" 
extern int CheckAvailableSize(WOLFSSL *ssl, int size);
#pragma coverity compliance end_block "MISRA C-2012 Rule 8.5"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

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
Net_ProvObject net_ProvStreamServer${INST_NUMBER} =
{
    .fpInit =    &Net_ProvStreamServerInit${INST_NUMBER},
    .fpDeinit =  &Net_ProvStreamServerDeinit${INST_NUMBER},
    .fpOpen =    &Net_ProvStreamServerOpen${INST_NUMBER},
    .fpConnect = &Net_ProvServerAccept${INST_NUMBER},
    .fpClose =   &Net_ProvConnectionClose${INST_NUMBER},
    .fpWrite =   &Net_ProvWrite${INST_NUMBER},
    .fpWriteReady =   &Net_ProvWriteReady${INST_NUMBER},
    .fpRead =    &Net_ProvRead${INST_NUMBER},
    .fpReadReady = &Net_ProvReadReady${INST_NUMBER},
    .fpPeek =    &Net_ProvPeek${INST_NUMBER},
    .fpIsInited = &Net_ProvStreamServerIsInited${INST_NUMBER},
    .fpOutputSize = &Net_ProvOutputSize${INST_NUMBER},
    .fpMaxOutputSize = &Net_ProvMaxOutputSize${INST_NUMBER},

};
            </#if>
			<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
			<#if netPresSuppClient?has_content && netPresSuppClient == true>          
Net_ProvObject net_ProvStreamClient${INST_NUMBER} = 
{
    .fpInit =    &Net_ProvStreamClientInit${INST_NUMBER},
    .fpDeinit =  &Net_ProvStreamClientDeinit${INST_NUMBER},
    .fpOpen =    &Net_ProvStreamClientOpen${INST_NUMBER},
    .fpConnect = &Net_ProvClientConnect${INST_NUMBER},
    .fpClose =   &Net_ProvConnectionClose${INST_NUMBER},
    .fpWrite =   &Net_ProvWrite${INST_NUMBER},
    .fpWriteReady =   &Net_ProvWriteReady${INST_NUMBER},
    .fpRead =    &Net_ProvRead${INST_NUMBER},
    .fpReadReady = &Net_ProvReadReady${INST_NUMBER},
    .fpPeek =    &Net_ProvPeek${INST_NUMBER},
    .fpIsInited = &Net_ProvStreamClientIsInited${INST_NUMBER},
    .fpOutputSize = &Net_ProvOutputSize${INST_NUMBER},
    .fpMaxOutputSize = &Net_ProvMaxOutputSize${INST_NUMBER},
};
            </#if>
        </#if>
		<#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
		<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
			<#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
			<#if netPresSuppServer?has_content && netPresSuppServer == true>         
Net_ProvObject net_ProvDataGramServer${INST_NUMBER} =
{
    .fpInit =    &Net_ProvDataGramServerInit${INST_NUMBER},
    .fpDeinit =  &Net_ProvDataGramServerDeinit${INST_NUMBER},
    .fpOpen =    &Net_ProvDataGramServerOpen${INST_NUMBER},
    .fpConnect = &Net_ProvServerAccept${INST_NUMBER},
    .fpClose =   &Net_ProvConnectionClose${INST_NUMBER},
    .fpWrite =   &Net_ProvWrite${INST_NUMBER},
    .fpWriteReady =   &Net_ProvWriteReady${INST_NUMBER},
    .fpRead =    &Net_ProvRead${INST_NUMBER},
    .fpReadReady = &Net_ProvReadReady${INST_NUMBER},
    .fpPeek =    &Net_ProvPeek${INST_NUMBER},
    .fpIsInited = &Net_ProvDataGramServerIsInited${INST_NUMBER},
    .fpOutputSize = &Net_ProvOutputSize${INST_NUMBER},
    .fpMaxOutputSize = &Net_ProvMaxOutputSize${INST_NUMBER},
};
            </#if>
			<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
            <#if netPresSuppClient?has_content && netPresSuppClient == true>             
Net_ProvObject net_ProvDataGramClient${INST_NUMBER} =
{
    .fpInit =    &Net_ProvDataGramClientInit${INST_NUMBER},
    .fpDeinit =  &Net_ProvDataGramClientDeinit${INST_NUMBER},
    .fpOpen =    &Net_ProvDataGramClientOpen${INST_NUMBER},
    .fpConnect = &Net_ProvClientConnect${INST_NUMBER},
    .fpClose =   &Net_ProvConnectionClose${INST_NUMBER},
    .fpWrite =   &Net_ProvWrite${INST_NUMBER},
    .fpWriteReady =   &Net_ProvWriteReady${INST_NUMBER},
    .fpRead =    &Net_ProvRead${INST_NUMBER},
    .fpReadReady = &Net_ProvReadReady${INST_NUMBER},
    .fpPeek =    &Net_ProvPeek${INST_NUMBER},
    .fpIsInited = &Net_ProvDataGramClientIsInited${INST_NUMBER},
    .fpOutputSize = &Net_ProvOutputSize${INST_NUMBER},
    .fpMaxOutputSize = &Net_ProvMaxOutputSize${INST_NUMBER},
};
            </#if>
        </#if>
    </#if>
</#macro>
<#if NET_PRES_BLOB_RUNTIME_CERT_SUPPORT == true>
#define NET_PRES_MAX_CERT_LEN	4096
unsigned char g_NewCertFile[NET_PRES_MAX_CERT_LEN];
long g_NewCertSz = 0;
int g_NewCertFormat;
</#if>
<#macro NET_PRES_ENC_GLUE_INIT
        INST
        CONNECTION
        TYPE>

<#assign netPresUseWolfSSLDebugLog= "NET_PRES_USE_WOLF_SSL_DEBUG_LOG"?eval>
<#if netPresUseWolfSSLDebugLog?has_content && netPresUseWolfSSLDebugLog == true>
void Net_Prov${TYPE}${CONNECTION}Log${INST}(int level, const char * message)
{
	<#assign num = "NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS"?eval>
	static char dataBuff[${num}][120];
	static int bufNum = 0;
	if (level > 2)
	{
		return;
	}
	snprintf(dataBuff[bufNum], 120, "wolfSSL (%d): %s\r\n", level, message);
	SYS_CONSOLE_MESSAGE(dataBuff[bufNum]);
	bufNum ++;
	if (bufNum == ${num})
	{
		bufNum = 0;
	}
}
</#if>
		
bool Net_Prov${TYPE}${CONNECTION}Init${INST}(struct S_NET_PRES_TransportObject * transObject)
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
    if (net_pres_wolfsslUsers == 0U)
    {
        (void)wolfSSL_Init();
		<#assign netPresUseWolfSSLDebugLog= "NET_PRES_USE_WOLF_SSL_DEBUG_LOG"?eval>
		<#if netPresUseWolfSSLDebugLog?has_content && netPresUseWolfSSLDebugLog == true>
		wolfSSL_SetLoggingCb(Net_Prov${TYPE}${CONNECTION}Log${INST});
		wolfSSL_Debugging_ON();
		</#if>
        net_pres_wolfsslUsers++;
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
    if (net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context == NULL)
    {
        return false;
    }
    <#if NET_PRES_BLOB_ENABLE_PEER_CERT_VERIFICATION == false>
    // Turn off verification, because SNTP is usually blocked by a firewall
    wolfSSL_CTX_set_verify(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, SSL_VERIFY_NONE, NULL);
		<#else>
	wolfSSL_CTX_set_verify(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, WOLFSSL_VERIFY_PEER, NULL);
		</#if>
	
    union
    {
        int (*receiveCb)(void *sslin, char *buf, int sz, void *ctx);
        CallbackIORecv ioRecv;
        int (*sendCb)(void *sslin, char *buf, int sz, void *ctx);
        CallbackIOSend ioSend;
    }U_RCV_CBACK;
    U_RCV_CBACK.receiveCb = &NET_Glue_${TYPE}${CONNECTION}ReceiveCb${INST};
    wolfSSL_SetIORecv(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, U_RCV_CBACK.ioRecv);
    U_RCV_CBACK.sendCb = &NET_Glue_${TYPE}${CONNECTION}SendCb${INST};
    wolfSSL_SetIOSend(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, U_RCV_CBACK.ioSend);
        <#if CONNECTION="Client">
    if (wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, caCertsPtr, caCertsLen, SSL_FILETYPE_${NET_PRES_BLOB_CLIENT_CERT_FORMAT}) != SSL_SUCCESS)
    {
        // Couldn't load the CA certificates
        //SYS_CONSOLE_MESSAGE("Something went wrong loading the CA certificates\r\n");
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
<#if INST == 0>
<#if TYPE == "Stream">
<#if CONNECTION == "Client">
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
</#if>
</#if>
</#if>
<#if INST == 0>
<#if TYPE == "Stream">
<#if CONNECTION == "Client">
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
</#if>
</#if>
</#if>
<#if INST == 0>
<#if TYPE == "Stream">
<#if CONNECTION == "Client">
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
</#if>
</#if>
</#if>
<#if NET_PRES_BLOB_RUNTIME_CERT_SUPPORT == true>
<#if INST == 0>
<#if TYPE == "Stream">
<#if CONNECTION == "Client">
	if(g_NewCertSz)
	{
		if (wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, g_NewCertFile, g_NewCertSz, g_NewCertFormat) != SSL_SUCCESS)
		{
			// Couldn't load the CA certificates
			//SYS_CONSOLE_MESSAGE("Something went wrong loading the CA certificates\r\n");
			wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
			return false;
		}
		g_NewCertSz = 0;
	}
</#if>
</#if>
</#if>
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
bool Net_Prov${TYPE}${CONNECTION}Deinit${INST}(void)
{
	<#if (lib_wolfssl.wolfsslLoadTNGTLSCert)?has_content && ((lib_wolfssl.wolfsslLoadTNGTLSCert) == true)>
    atmel_finish();
    </#if>
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
    net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.isInited = false;
    net_pres_wolfsslUsers--;
    if (net_pres_wolfsslUsers == 0U)
    {
        (void)wolfSSL_Cleanup();
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
bool Net_Prov${TYPE}${CONNECTION}Open${INST}(SYS_MODULE_OBJ obj, uintptr_t presHandle, uintptr_t transHandle, void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    <#lt>    WOLFSSL* ssl = wolfSSL_new(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
    <#lt>    if (ssl == NULL)
    <#lt>    {
    <#lt>        return false;
    <#lt>    }
    <#lt>    if (wolfSSL_set_fd(ssl, transHandle) != SSL_SUCCESS)
    <#lt>    {
    <#lt>        wolfSSL_free(ssl);
    <#lt>        return false;
    <#lt>    }
		<#if ((lib_wolfssl.wolfsslTlsSni?has_content) && (lib_wolfssl.wolfsslTlsSni) == true)>
            <#assign netPresSNICbackEn= "NET_PRES_SNI_CALLBACK_ENABLE"?eval>
            <#if (netPresSNICbackEn?has_content) && (netPresSNICbackEn == true)>
            <#lt>   NET_PRES_SNI_CALLBACK sniCback = NET_PRES_SniCallbackGet(obj);
            <#lt>   if(sniCback != NULL)
            <#lt>   {
            <#lt>       const char* sni_name = sniCback(presHandle);
            <#lt>       if (wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, sni_name, strlen(sni_name)) != WOLFSSL_SUCCESS)
            <#lt>       {
            <#lt>           return false;
            <#lt>       }
            <#lt>    }
            <#else>
            <#lt>    if (wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, NET_PRES_SNI_HOST_NAME, strlen(NET_PRES_SNI_HOST_NAME)) != WOLFSSL_SUCCESS)
            <#lt>    {
            <#lt>        return false;
            <#lt>    }
            </#if>
		</#if>
		<#if ((lib_wolfssl.wolfsslTlsAlpn?has_content) && (lib_wolfssl.wolfsslTlsAlpn) == true)>
    <#lt>    if (wolfSSL_UseALPN(ssl, NET_PRES_ALPN_PROTOCOL_NAME_LIST, sizeof(NET_PRES_ALPN_PROTOCOL_NAME_LIST),WOLFSSL_ALPN_FAILED_ON_MISMATCH) != WOLFSSL_SUCCESS)
    <#lt>    {
    <#lt>        return false;
    <#lt>    }
		</#if>
    <#lt>    (void)memcpy(providerData, (void*)&ssl, sizeof(WOLFSSL*));
    <#lt>    return true;
    <#else>
    <#lt>//TODO: Enter in code to open a connection with the provider
    <#lt>return false;
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
NET_PRES_EncSessionStatus Net_Prov${CONNECTION}${ACCEPT}${INST}(void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
        <#if CONNECTION=="Server">
    int result = wolfSSL_accept(ssl);
        <#else>
    int result = wolfSSL_connect(ssl);
        </#if>
    if(result == SSL_SUCCESS)
    {
        return NET_PRES_ENC_SS_OPEN;
    }
    else
    {
        int error = wolfSSL_get_error(ssl, result);
        if(error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE)
        {
            return NET_PRES_ENC_SS_SERVER_NEGOTIATING;
        }
        else
        {
            return NET_PRES_ENC_SS_FAILED;
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
NET_PRES_EncSessionStatus Net_ProvConnectionClose${INST}(void * providerData)
{
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
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
int32_t Net_ProvWrite${INST}(void * providerData, const uint8_t * dataBuff, uint16_t size)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_write(ssl, dataBuff, (int)size);
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
uint16_t Net_ProvWriteReady${INST}(void * providerData, uint16_t reqSize, uint16_t minSize)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    
    char dataBuff = '\0';
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));

    int ret = wolfSSL_write(ssl, &dataBuff, 0);
    if(ret < 0)
    {
        return 0;
    }

    ret = CheckAvailableSize(ssl, (int)reqSize);
    if(ret == 0)
    {   // success
        return reqSize;
    }
    if(minSize != 0U)
    {
        ret = CheckAvailableSize(ssl, (int)minSize);
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
int32_t Net_ProvRead${INST}(void * providerData, uint8_t * dataBuff, uint16_t size)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_read(ssl, dataBuff, (int)size);
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

int32_t Net_ProvReadReady${INST}(void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
    int32_t ret = wolfSSL_pending(ssl);
    if (ret == 0) // wolfSSL_pending() doesn't check the underlying layer.
    {
        char dataBuff = '\0';
        if (wolfSSL_peek(ssl, &dataBuff, 1) == 0)
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
int32_t Net_ProvPeek${INST}(void * providerData, uint8_t * dataBuff, uint16_t size)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
    int ret = wolfSSL_peek(ssl, dataBuff, (int)size);
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
int32_t Net_ProvOutputSize${INST}(void * providerData, int32_t inSize)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
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
int32_t Net_ProvMaxOutputSize${INST}(void * providerData)
{
    <#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
    WOLFSSL* ssl;
    (void)memcpy((void*)&ssl, providerData, sizeof(WOLFSSL*));
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
bool Net_Prov${TYPE}${CONNECTION}IsInited${INST}(void)
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
static net_pres_wolfsslInfo net_pres_wolfSSLInfoStreamServer${INST};
                </#if>
				<#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
				<#if netPresSuppClient?has_content && netPresSuppClient == true>           
static net_pres_wolfsslInfo net_pres_wolfSSLInfoStreamClient${INST};
                </#if>
            </#if>
            <#assign netPresSuppDatagram= "NET_PRES_SUPPORT_DATAGRAM_ENC"?eval>
			<#if netPresSuppDatagram?has_content && netPresSuppDatagram == true>
                <#assign netPresSuppServer= "NET_PRES_SUPPORT_SERVER_ENC"?eval>
				<#if netPresSuppServer?has_content && netPresSuppServer == true>             
static net_pres_wolfsslInfo net_pres_wolfSSLInfoDataGramServer${INST};
                </#if>
                <#assign netPresSuppClient= "NET_PRES_SUPPORT_CLIENT_ENC"?eval>
				<#if netPresSuppClient?has_content && netPresSuppClient == true>           
static net_pres_wolfsslInfo net_pres_wolfSSLInfoDataGramClient${INST};
                </#if>
            </#if>
        </#if>
    </#if>
</#macro>
<#macro NET_PRES_ENC_SET_CERT_FUNCTION
        INST
        CONNECTION
        TYPE>
<#if NET_PRES_BLOB_RUNTIME_CERT_SUPPORT == true>
bool NET_PRES_SetCertificate(unsigned char* in, long sz, int format)
{
    int ret = 0;
    if(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context == NULL)
    {
		if(sz > NET_PRES_MAX_CERT_LEN)
			return false;
			
        (void)memcpy(g_NewCertFile, in, sz);
        g_NewCertSz = sz;
		g_NewCertFormat = format;
        return true;
    }
    
    ret = wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, in, sz, format);
    if (ret != SSL_SUCCESS)
    {
        // Couldn't load the CA certificates
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
    return true;
}
</#if>
</#macro>		
<#macro NET_PRES_ENC_GLUE_FUNCTIONS
    INST>
	<#assign netPresSuppEnc = "NET_PRES_SUPPORT_ENCRYPTION"?eval>
    <#if netPresSuppEnc?has_content && netPresSuppEnc == true>
	<#assign netPresUseWolfSSL= "NET_PRES_USE_WOLF_SSL"?eval>
	<#if netPresUseWolfSSL?has_content && netPresUseWolfSSL == true>
static uint8_t net_pres_wolfsslUsers = 0U;
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
				<#if INST == 0>
					<@NET_PRES_ENC_SET_CERT_FUNCTION INST "Client" "Stream"/>
				</#if>	
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
static int NET_Glue_${TYPE}${CONNECTION}ReceiveCb${INST}(void *sslin, char *buf, int sz, void *ctx)
{
    int fd = *(int *)ctx;
    uint16_t bufferSize;
    bufferSize = (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpReadyToRead)((uintptr_t)fd);
    if (bufferSize == 0U)
    {
        return (int)WOLFSSL_CBIO_ERR_WANT_READ;
    }
    bufferSize = (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpRead)((uintptr_t)fd, (uint8_t*)buf, sz);
    return (int)bufferSize;
}
static int NET_Glue_${TYPE}${CONNECTION}SendCb${INST}(void *sslin, char *buf, int sz, void *ctx)
{
    int fd = *(int *)ctx;
    uint16_t bufferSize;
    bufferSize = (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpReadyToWrite)((uintptr_t)fd);
    if (bufferSize == 0U)
    {
        return (int)WOLFSSL_CBIO_ERR_WANT_WRITE;
    }

    bufferSize =  (*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpWrite)((uintptr_t)fd, (uint8_t*)buf, (uint16_t)sz);
	<#if TYPE = "DataGram">
	(void)(*net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.transObject->fpFlush)((uintptr_t)fd);
	</#if>
    return (int)bufferSize;
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

/* MISRA C-2012 Rule 8.4 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_8_4_NET_DR_16 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 8.4" "H3_MISRAC_2012_R_8_4_NET_DR_16" 
// Temporary fix till crypto library is upgraded to recent wolfssl versions.
int  InitRng(RNG* rng)
{
    return wc_InitRng(rng);
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 8.4"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

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
