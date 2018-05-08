/*******************************************************************************
 Source file for the Net Pres Encryption glue functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*******************************************************************************
File Name: net_pres_enc_glue.c
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

#include "net_pres_enc_glue.h"
#include "net/pres/net_pres_transportapi.h"
#include "net/pres/net_pres_certstore.h"

<#if CONFIG_USE_3RDPARTY_WOLFSSL>
#include "config.h"
#include "wolfssl/ssl.h"
#include "wolfssl/wolfcrypt/logging.h"
#include "wolfssl/wolfcrypt/random.h"

<#assign needSysConsole=false/>
<#list 0..(CONFIG_NET_PRES_INSTANCES?number-1) as idx>
	<#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX${idx}"]>
		<#assign needSysConsole=true/>
    </#if>
</#list>
<#if needSysConsole>
#include "system/command/sys_command.h"
</#if>


static uint8_t _net_pres_wolfsslUsers = 0;
</#if>
<#macro NET_PRES_ENC_PROV_INFOS
    INST_NUMBER>

    <#if .vars["CONFIG_NET_PRES_SUPPORT_ENCRYPTION${INST_NUMBER}"]>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
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
};
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
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
};
            </#if>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST_NUMBER}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST_NUMBER}"]>            
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
};
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST_NUMBER}"]>            
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
};
            </#if>
        </#if>
    </#if>
</#macro>
<#macro NET_PRES_ENC_GLUE_INIT
        INST
        CONNECTION
        TYPE>
		
<#if  .vars["CONFIG_NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX${INST}"]>
void NET_PRES_EncProvider${TYPE}${CONNECTION}Log${INST}(int level, const char * message)
{
	<#assign num = .vars["CONFIG_NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS_IDX${INST}"]/>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
        <#if CONNECTION="Client">
    const uint8_t * caCertsPtr;
    int32_t caCertsLen;
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
		<#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX${INST}"]>
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
    wolfSSL_SetIORecv(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, (CallbackIORecv)&NET_PRES_EncGlue_${TYPE}${CONNECTION}ReceiveCb${INST});
    wolfSSL_SetIOSend(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, (CallbackIOSend)&NET_PRES_EncGlue_${TYPE}${CONNECTION}SendCb${INST});
        <#if CONNECTION="Client">
    if (wolfSSL_CTX_load_verify_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, caCertsPtr, caCertsLen, SSL_FILETYPE_ASN1) != SSL_SUCCESS)
    {
        // Couldn't load the certificates
        //SYS_CONSOLE_MESSAGE("Something went wrong loading the certificates\r\n");
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
        <#else>
    if (wolfSSL_CTX_use_certificate_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, serverCertPtr, serverCertLen, SSL_FILETYPE_ASN1) != SSL_SUCCESS)
    {
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
    if (wolfSSL_CTX_use_PrivateKey_buffer(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, serverKeyPtr, serverKeyLen, SSL_FILETYPE_ASN1) != SSL_SUCCESS)
    {
        wolfSSL_CTX_free(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context);
        return false;
    }
        </#if>
    // Turn off verification, because SNTP is usually blocked by a firewall
    wolfSSL_CTX_set_verify(net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.context, SSL_VERIFY_NONE, 0);
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
bool NET_PRES_EncProvider${TYPE}${CONNECTION}Deinit${INST}()
{
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
    extern  int CheckAvailableSize(WOLFSSL *ssl, int size);
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
<#macro NET_PRES_ENC_GLUE_IS_INIT
        INST
        CONNECTION
        TYPE>
bool NET_PRES_EncProvider${TYPE}${CONNECTION}IsInited${INST}()
{
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
    return net_pres_wolfSSLInfo${TYPE}${CONNECTION}${INST}.isInited;
    <#else>
    //TODO: Enter in code to open a connection with the provider
    return false;
    </#if>
}
</#macro> 
<#macro NET_PRES_ENC_GLUE_WOLF_INFO
    INST>
    <#if .vars["CONFIG_NET_PRES_SUPPORT_ENCRYPTION${INST}"]>
        <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${INST}"]>
                <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST}"]>            
net_pres_wolfsslInfo net_pres_wolfSSLInfoStreamServer${INST};
                </#if>
                <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST}"]>            
net_pres_wolfsslInfo net_pres_wolfSSLInfoStreamClient${INST};
                </#if>
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST}"]>
                <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST}"]>            
net_pres_wolfsslInfo net_pres_wolfSSLInfoDataGramServer${INST};
                </#if>
                <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST}"]>            
net_pres_wolfsslInfo net_pres_wolfSSLInfoDataGramClient${INST};
                </#if>
            </#if>
        </#if>
    </#if>
</#macro>
<#macro NET_PRES_ENC_GLUE_FUNCTIONS
    INST>
    <#if .vars["CONFIG_NET_PRES_SUPPORT_ENCRYPTION${INST}"]>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${INST}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST}"]>
                <@NET_PRES_ENC_GLUE_INIT INST "Server" "Stream"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Server" "Stream"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Server" "Stream"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Server" "Stream"/>                
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST}"]>            
                <@NET_PRES_ENC_GLUE_INIT INST "Client" "Stream"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Client" "Stream"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Client" "Stream"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Client" "Stream"/>                
            </#if>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST}"]>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST}"]>            
                <@NET_PRES_ENC_GLUE_INIT INST "Server" "DataGram"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Server" "DataGram"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Server" "DataGram"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Server" "DataGram"/>                
            </#if>
            <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST}"]>            
                <@NET_PRES_ENC_GLUE_INIT INST "Client" "DataGram"/>
                <@NET_PRES_ENC_GLUE_DEINIT INST "Client" "DataGram"/>
                <@NET_PRES_ENC_GLUE_OPEN INST "Client" "DataGram"/>
                <@NET_PRES_ENC_GLUE_IS_INIT INST "Client" "DataGram"/>                
            </#if>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST}"]> 
            <@NET_PRES_ENC_GLUE_CONNECT INST "Server"/>
        </#if>
        <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST}"]>            
            <@NET_PRES_ENC_GLUE_CONNECT INST "Client"/>
        </#if>
        <@NET_PRES_ENC_GLUE_CLOSE INST/>
        <@NET_PRES_ENC_GLUE_WRITE INST/>
        <@NET_PRES_ENC_GLUE_WRITE_READY INST/>
        <@NET_PRES_ENC_GLUE_READ INST/>
        <@NET_PRES_ENC_GLUE_READ_READY INST/>
        <@NET_PRES_ENC_GLUE_PEEK INST/>
    </#if>
</#macro>
<#macro NET_PRES_WOLF_CB
    INST
    CONNECTION
    TYPE>
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${INST}"]>
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
    return bufferSize;
}
    </#if>
</#macro>
<#macro NET_PRES_WOLF_CBS
    INST>
    <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST}"] && .vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${INST}"]>
        <@NET_PRES_WOLF_CB INST "Server" "Stream"/>
    </#if>
    <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST}"] && .vars["CONFIG_NET_PRES_SUPPORT_STREAM_ENC_IDX${INST}"]>
        <@NET_PRES_WOLF_CB INST "Client" "Stream"/>
    </#if>
    <#if .vars["CONFIG_NET_PRES_SUPPORT_SERVER_ENC_IDX${INST}"] && .vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST}"]>
        <@NET_PRES_WOLF_CB INST "Server" "DataGram"/>
    </#if>
    <#if .vars["CONFIG_NET_PRES_SUPPORT_CLIENT_ENC_IDX${INST}"] && .vars["CONFIG_NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INST}"]>
        <@NET_PRES_WOLF_CB INST "Client" "DataGram"/>
    </#if>
</#macro>
<#if CONFIG_USE_3RDPARTY_WOLFSSL>
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
<#list 0..(CONFIG_NET_PRES_INSTANCES?number-1) as idx>
    <@NET_PRES_ENC_PROV_INFOS idx/>
</#list>
<#list 0..(CONFIG_NET_PRES_INSTANCES?number-1) as idx>
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${idx}"] || .vars["CONFIG_NET_PRES_GENERATE_ENC_STUBS_IDX${idx}"]>
        <@NET_PRES_ENC_GLUE_WOLF_INFO idx/>
    </#if>
</#list>
<#list 0..(CONFIG_NET_PRES_INSTANCES?number-1) as idx>
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${idx}"] || .vars["CONFIG_NET_PRES_GENERATE_ENC_STUBS_IDX${idx}"]>
        <@NET_PRES_WOLF_CBS idx/>
    </#if>
</#list>
<#list 0..(CONFIG_NET_PRES_INSTANCES?number-1) as idx>
    <#if .vars["CONFIG_NET_PRES_USE_WOLF_SSL_IDX${idx}"] || .vars["CONFIG_NET_PRES_GENERATE_ENC_STUBS_IDX${idx}"]>
        <@NET_PRES_ENC_GLUE_FUNCTIONS idx/>
    </#if>
</#list>
