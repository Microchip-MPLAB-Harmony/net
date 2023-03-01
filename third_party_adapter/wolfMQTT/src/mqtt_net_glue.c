/*******************************************************************************
  wolfMQTT NET Glue Source File

  Summary: 
    wolfMQTT MQTTNET glue file
    
  Description:
    This file provides the MQTTNET glue file for using
    wolfMQTT with the Harmony TCP/IP stack
*******************************************************************************/

/*
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include "configuration.h"
#include "net_pres/pres/net_pres_socketapi.h"
#include "tcpip/tcpip.h"
#include "system/debug/sys_debug.h"
#include "system/sys_time_h2_adapter.h"

#include "mqtt_net_glue.h"
#include "wolfmqtt/mqtt_client.h"

#ifndef WOLFMQTT_NONBLOCK
    #error wolfMQTT build for Harmony must have WOLFMQTT_NONBLOCK defined
#endif

typedef enum
{
    NET_GLUE_IDLE = 0,
    NET_GLUE_CONNECT_START,
    NET_GLUE_WAIT_DNS,
    NET_GLUE_SOCKET_GET,
    NET_GLUE_SOCKET_CONNECT,
    NET_GLUE_SOCKET_WAIT_TLS,
    NET_GLUE_SOCKET_WRITE,
    NET_GLUE_SOCKET_READ,

} WMQTT_NET_GLUE_STAT;

typedef struct
{
    TCPIP_DNS_RESOLVE_TYPE  dnsType;
    NET_PRES_SKT_HANDLE_T   sktH;
    NET_PRES_SKT_PORT_T     brokerPort;
    IP_ADDRESS_TYPE         addType;
    IP_MULTI_ADDRESS        brokerAdd;
    uint32_t                tmoTick;
    WMQTT_NET_GLUE_STAT     stat;
    uint32_t                opBytes; 
#ifdef WOLFMQTT_SN
    bool                    isSNContext;    // created for SN
#endif  // WOLFMQTT_SN
    char                    brokerName[WMQTT_NET_GLUE_MAX_BROKER_NAME + 1];
} WMQTT_NET_CONTEXT;


// enum with the local error codes/results
typedef enum
{
    WMQTT_NET_GLUE_RES_OK   = 0,

    // errors
    WMQTT_NET_GLUE_DNS_ERR              = -1,
    WMQTT_NET_GLUE_SKT_CREATE_ERR       = -2,
    WMQTT_NET_GLUE_SKT_TX_SIZE_ERR      = -3,
    WMQTT_NET_GLUE_SKT_RX_SIZE_ERR      = -4,
    WMQTT_NET_GLUE_SKT_BIND_ERR         = -5,
    WMQTT_NET_GLUE_SKT_RESET_ERR        = -6,
    WMQTT_NET_GLUE_TIMEOUT              = -7,
    WMQTT_NET_GLUE_START_ENCRYPT_ERR    = -8,   // couldn't start encryption. is wolfSSL linked in?
    WMQTT_NET_GLUE_TLS_NEGOTIATION_ERR  = -9,   // TLS negotiation failed...


    WMQTT_NET_GLUE_INTERNAL_ERR         = -10,  // should NOT happen


}WMQTT_NET_GLUE_RES;

// Local prototypes

static int  WMQTT_NETGlue_Error(WMQTT_NET_CONTEXT* net_glue_ctx, WMQTT_NET_GLUE_RES res, const char* message, int lineNo);
static void WMQTT_NETGlue_SetTmo(WMQTT_NET_CONTEXT* net_glue_ctx, int timeout_ms);
static WMQTT_NET_GLUE_RES WMQTT_NETGlue_CheckConnection(WMQTT_NET_CONTEXT* net_glue_ctx);

static int WMQTT_NETGlue_Connect(void* context, const char* host, word16 port, int timeout_ms);
static int WMQTT_NETGlue_Write(void* context, const uint8_t* buff, int buff_len, int timeout_ms);
static int WMQTT_NETGlue_Read(void* context, uint8_t* buff, int buff_len, int timeout_ms);
static int WMQTT_NETGlue_Disconnect(void* context);

#ifdef WOLFMQTT_SN
static int WMQTT_NETGlue_Peek(void* context, uint8_t* buff, int buff_len, int timeout_ms);
static int  WMQTT_NETGlue_CreateUDPSocket(WMQTT_NET_CONTEXT* net_glue_ctx);
#endif  // WOLFMQTT_SN

static int WMQTT_NETGlue_DoInitialize(MqttNet* net, bool isSN);
static int WMQTT_NETGlue_DoRead(void* context, uint8_t* buff, int buff_len, int timeout_ms, bool peek);

static int  WMQTT_NETGlue_CreateSocket(WMQTT_NET_CONTEXT* net_glue_ctx);
static int  WMQTT_NETGlue_CreateTCPSocket(WMQTT_NET_CONTEXT* net_glue_ctx);

// Public API implementation
int WMQTT_NETGlue_Initialize(MqttNet* net)
{
    return WMQTT_NETGlue_DoInitialize(net, false);
}

#ifdef WOLFMQTT_SN
int WMQTT_NETGlue_SNInitialize(MqttNet* net)
{
    return WMQTT_NETGlue_DoInitialize(net, true);
}
#endif  // WOLFMQTT_SN


int WMQTT_NETGlue_Deinitialize(MqttNet* net)
{
    if (net)
    {
        WMQTT_NET_CONTEXT* net_glue_ctx = (WMQTT_NET_CONTEXT*)net->context;
        if(net_glue_ctx != 0)
        {
            if (net_glue_ctx->sktH != NET_PRES_INVALID_SOCKET)
            {
                NET_PRES_SocketClose(net_glue_ctx->sktH);
                net_glue_ctx->sktH = NET_PRES_INVALID_SOCKET;
            }
            WMQTT_NET_GLUE_FREE(net_glue_ctx);
        }
        memset(net, 0, sizeof(MqttNet));
    }
    return MQTT_CODE_SUCCESS;
}


// local functions

// initializes a NET glue context
static int WMQTT_NETGlue_DoInitialize(MqttNet* net, bool isSN)
{
    if (net)
    {
        memset(net, 0, sizeof(*net));
        WMQTT_NET_CONTEXT* wmqtt_net_context = (WMQTT_NET_CONTEXT*)WMQTT_NET_GLUE_MALLOC(sizeof(*wmqtt_net_context));
        if(wmqtt_net_context == 0)
        {
            return MQTT_CODE_ERROR_MEMORY;
        }
        memset(wmqtt_net_context, 0, sizeof(*wmqtt_net_context));
        wmqtt_net_context->stat = NET_GLUE_IDLE;

        // populate the MqttNet data
        net->connect = WMQTT_NETGlue_Connect;
        net->read = WMQTT_NETGlue_Read;
        net->write = WMQTT_NETGlue_Write;
        net->disconnect = WMQTT_NETGlue_Disconnect;
        net->context = wmqtt_net_context;

#ifdef WOLFMQTT_SN
        if(isSN)
        {
            wmqtt_net_context->isSNContext = true;
            net->peek = WMQTT_NETGlue_Peek;
        }
#endif  // WOLFMQTT_SN
    }

    return MQTT_CODE_SUCCESS; 
}

// implements the MqttNet->connect function
// no DTLS support for SN
static int WMQTT_NETGlue_Connect(void* context, const char* host, word16 port, int timeout_ms)
{
    TCPIP_DNS_RESULT dnsRes;
    WMQTT_NET_GLUE_RES netRes;
    int     sktRes;
    bool    startTls;

    WMQTT_NET_CONTEXT* net_glue_ctx = (WMQTT_NET_CONTEXT*)context;

    /* Get address information for host and locate IPv4 */
    switch(net_glue_ctx->stat)
    {
        case NET_GLUE_IDLE:
            // just starting
            net_glue_ctx->stat = NET_GLUE_CONNECT_START;
            // fall through!

        case NET_GLUE_CONNECT_START:
            if(strlen(host) > sizeof(net_glue_ctx->brokerName) - 1)
            {
                return MQTT_CODE_ERROR_BAD_ARG;
            }

            strcpy(net_glue_ctx->brokerName, host);
            net_glue_ctx->brokerPort = (NET_PRES_SKT_PORT_T)port;

#if (WMQTT_NET_GLUE_IPV6 != 0)
            net_glue_ctx->addType = IP_ADDRESS_TYPE_IPV6;
            net_glue_ctx->dnsType = TCPIP_DNS_TYPE_AAAA;
#else
            net_glue_ctx->addType = IP_ADDRESS_TYPE_IPV4;
            net_glue_ctx->dnsType = TCPIP_DNS_TYPE_A;
#endif  // (WMQTT_NET_GLUE_IPV6 != 0)


            // start resolving the broker name
            dnsRes = TCPIP_DNS_Resolve(net_glue_ctx->brokerName, net_glue_ctx->dnsType);
    
            if(dnsRes < 0)
            {
                return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_DNS_ERR, __func__, __LINE__);
            } 

            net_glue_ctx->stat = NET_GLUE_WAIT_DNS;
            return MQTT_CODE_CONTINUE;

        case NET_GLUE_WAIT_DNS:

            dnsRes = TCPIP_DNS_IsResolved(net_glue_ctx->brokerName, &net_glue_ctx->brokerAdd, net_glue_ctx->addType);
            if(dnsRes < 0)
            {
                return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_DNS_ERR, __func__, __LINE__);
            } 
            else if(dnsRes != TCPIP_DNS_RES_PENDING)
            {   // DNS done; advance
                net_glue_ctx->stat = NET_GLUE_SOCKET_GET;
            }
            return MQTT_CODE_CONTINUE;

        case NET_GLUE_SOCKET_GET:
            /* Create socket */
            sktRes = WMQTT_NETGlue_CreateSocket(net_glue_ctx); 
            if(sktRes != MQTT_CODE_SUCCESS)
            {   // something went wrong
                break;
            }

            NET_PRES_SocketConnect(net_glue_ctx->sktH);
            WMQTT_NETGlue_SetTmo(net_glue_ctx, timeout_ms);
            net_glue_ctx->stat = NET_GLUE_SOCKET_CONNECT;
#if (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
            SYS_CONSOLE_PRINT("WMQTT_NET_GLUE Info: %s\r\n", "Started Connect");
#endif  // (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
            // fall_through;

        case NET_GLUE_SOCKET_CONNECT:
            if (!NET_PRES_SocketIsConnected(net_glue_ctx->sktH))
            {   // wait some more
                netRes = WMQTT_NETGlue_CheckConnection(net_glue_ctx);
                if(netRes != WMQTT_NET_GLUE_RES_OK)
                {
                    return WMQTT_NETGlue_Error(net_glue_ctx, netRes, __func__, __LINE__);
                }

                return MQTT_CODE_CONTINUE;
            }
            
#if (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
            SYS_CONSOLE_PRINT("WMQTT_NET_GLUE Info: %s\r\n", "Connected Successfully");
#endif  // (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
            // OK, we're connected
            // check if we need encryption
#if (WMQTT_NET_GLUE_FORCE_TLS != 0)
            startTls = true;
#else
            startTls = TCPIP_Helper_TCPSecurePortGet(net_glue_ctx->brokerPort);
#endif  // (WMQTT_NET_GLUE_FORCE_TLS != 0)

            if(startTls)
            {
#if (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
                SYS_CONSOLE_PRINT("WMQTT_NET_GLUE Info: %s\r\n", "Start TLS");
#endif  // (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
                if (!NET_PRES_SocketEncryptSocket(net_glue_ctx->sktH))
                {
                    return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_START_ENCRYPT_ERR, __func__, __LINE__);
                }
                WMQTT_NETGlue_SetTmo(net_glue_ctx, timeout_ms);
                net_glue_ctx->stat = NET_GLUE_SOCKET_WAIT_TLS;
            }
            else
            {   // all done
                net_glue_ctx->stat = NET_GLUE_IDLE;
                return MQTT_CODE_SUCCESS;
            }

            return MQTT_CODE_CONTINUE;

        case NET_GLUE_SOCKET_WAIT_TLS:
            if(NET_PRES_SocketIsNegotiatingEncryption(net_glue_ctx->sktH))
            {
                netRes = WMQTT_NETGlue_CheckConnection(net_glue_ctx);
                if(netRes != WMQTT_NET_GLUE_RES_OK)
                {
                    return WMQTT_NETGlue_Error(net_glue_ctx, netRes, __func__, __LINE__);
                }
                
                // wait some more
                return MQTT_CODE_CONTINUE;
            }

            if (!NET_PRES_SocketIsSecure(net_glue_ctx->sktH))
            {
                return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_TLS_NEGOTIATION_ERR, __func__, __LINE__);
            }

#if (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
            SYS_CONSOLE_PRINT("WMQTT_NET_GLUE Info: %s\r\n", "Secure Connection Established");
#endif  // (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
            // good to go
            net_glue_ctx->stat = NET_GLUE_IDLE;
            return MQTT_CODE_SUCCESS;


        default:
            break;
    }

    return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_INTERNAL_ERR, __func__, __LINE__);
}

// creates the NET glue socket
static int WMQTT_NETGlue_CreateSocket(WMQTT_NET_CONTEXT* net_glue_ctx)
{
#ifdef WOLFMQTT_SN
        if(net_glue_ctx->isSNContext)
        {
            return WMQTT_NETGlue_CreateUDPSocket(net_glue_ctx);
        }
#endif  // WOLFMQTT_SN
        return WMQTT_NETGlue_CreateTCPSocket(net_glue_ctx);
}


static int WMQTT_NETGlue_CreateTCPSocket(WMQTT_NET_CONTEXT* net_glue_ctx)
{
    net_glue_ctx->sktH = NET_PRES_SocketOpen(0, NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT, (NET_PRES_SKT_ADDR_T)net_glue_ctx->addType, net_glue_ctx->brokerPort, 0, 0);
    if (net_glue_ctx->sktH == NET_PRES_INVALID_SOCKET)
    {
        return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_SKT_CREATE_ERR, __func__, __LINE__);
    }

#if (WMQTT_NET_SKT_TX_BUFF != 0) || (WMQTT_NET_SKT_RX_BUFF != 0)
    TCP_SOCKET_INFO sktInfo;
    NET_PRES_SocketInfoGet(net_glue_ctx->sktH, &sktInfo);

#if (WMQTT_NET_SKT_TX_BUFF != 0)
    if(sktInfo. txSize < WMQTT_NET_SKT_TX_BUFF)
    {   // set the new TX size
        void* tcpBuffSize = (void*)(unsigned int)WMQTT_NET_SKT_TX_BUFF;
        if(!NET_PRES_SocketOptionsSet(net_glue_ctx->sktH, TCP_OPTION_TX_BUFF, tcpBuffSize))
        {
            return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_SKT_TX_SIZE_ERR, __func__, __LINE__);
        }
    }
#endif  // (WMQTT_NET_SKT_TX_BUFF != 0)
#if (WMQTT_NET_SKT_RX_BUFF != 0)
    if(sktInfo. rxSize < WMQTT_NET_SKT_RX_BUFF)
    {   // set the new TX size
        void* tcpBuffSize = (void*)(unsigned int)WMQTT_NET_SKT_RX_BUFF;
        if(!NET_PRES_SocketOptionsSet(net_glue_ctx->sktH, TCP_OPTION_RX_BUFF, tcpBuffSize))
        {
            return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_SKT_RX_SIZE_ERR, __func__, __LINE__);
        }
    }
#endif  // (WMQTT_NET_SKT_RX_BUFF != 0)
#endif  // (WMQTT_NET_SKT_TX_BUFF != 0) || (WMQTT_NET_SKT_RX_BUFF != 0)

    if(!NET_PRES_SocketRemoteBind(net_glue_ctx->sktH, (NET_PRES_SKT_ADDR_T)net_glue_ctx->addType,  net_glue_ctx->brokerPort, (NET_PRES_ADDRESS*)&net_glue_ctx->brokerAdd))
    {   // failed
        return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_SKT_BIND_ERR, __func__, __LINE__);
    }

    NET_PRES_SocketWasReset(net_glue_ctx->sktH);
    return MQTT_CODE_SUCCESS;
}

#ifdef WOLFMQTT_SN
static int  WMQTT_NETGlue_CreateUDPSocket(WMQTT_NET_CONTEXT* net_glue_ctx)
{
    net_glue_ctx->sktH = NET_PRES_SocketOpen(0, NET_PRES_SKT_UNENCRYPTED_DATAGRAM_CLIENT, (NET_PRES_SKT_ADDR_T)net_glue_ctx->addType, net_glue_ctx->brokerPort, 0, 0);
    if (net_glue_ctx->sktH == NET_PRES_INVALID_SOCKET)
    {
        return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_SKT_CREATE_ERR, __func__, __LINE__);
    }

#if (WMQTT_NET_SKT_TX_BUFF != 0)
    UDP_SOCKET_INFO sktInfo;
    NET_PRES_SocketInfoGet(net_glue_ctx->sktH, &sktInfo);

    if(sktInfo. txSize < WMQTT_NET_SKT_TX_BUFF)
    {   // set the new TX size
        void* udpBuffSize = (void*)(unsigned int)WMQTT_NET_SKT_TX_BUFF;
        if(!NET_PRES_SocketOptionsSet(net_glue_ctx->sktH, UDP_OPTION_TX_BUFF, udpBuffSize))
        {
            return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_SKT_TX_SIZE_ERR, __func__, __LINE__);
        }
    }
#endif  // (WMQTT_NET_SKT_TX_BUFF != 0)

    if(!NET_PRES_SocketRemoteBind(net_glue_ctx->sktH, (NET_PRES_SKT_ADDR_T)net_glue_ctx->addType,  net_glue_ctx->brokerPort, (NET_PRES_ADDRESS*)&net_glue_ctx->brokerAdd))
    {   // failed
        return WMQTT_NETGlue_Error(net_glue_ctx, WMQTT_NET_GLUE_SKT_BIND_ERR, __func__, __LINE__);
    }

    return MQTT_CODE_SUCCESS;
}

#endif  // WOLFMQTT_SN

// implements the MqttNet->write function
static int WMQTT_NETGlue_Write(void* context, const uint8_t* buff, int buff_len, int timeout_ms)
{
    WMQTT_NET_GLUE_RES netRes;
    WMQTT_NET_CONTEXT* net_glue_ctx = (WMQTT_NET_CONTEXT*)context;

    if (net_glue_ctx == 0 || buff == 0 || buff_len <= 0)
    {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if(net_glue_ctx->stat != NET_GLUE_SOCKET_WRITE)
    {   // first time we start the write op;
        WMQTT_NETGlue_SetTmo(net_glue_ctx, timeout_ms);
        net_glue_ctx->stat = NET_GLUE_SOCKET_WRITE;
        net_glue_ctx->opBytes = 0;
    }

    const uint8_t* pData = buff + net_glue_ctx->opBytes; 
    uint16_t wr_bytes = NET_PRES_SocketWrite(net_glue_ctx->sktH, pData, buff_len - net_glue_ctx->opBytes);

    net_glue_ctx->opBytes += wr_bytes;
    if(net_glue_ctx->opBytes < buff_len)
    {   // check timeout!
        netRes = WMQTT_NETGlue_CheckConnection(net_glue_ctx);
        if(netRes != WMQTT_NET_GLUE_RES_OK)
        {
            return WMQTT_NETGlue_Error(net_glue_ctx, netRes, __func__, __LINE__);
        }
        return MQTT_CODE_CONTINUE;
    }


    // all good
    net_glue_ctx->stat = NET_GLUE_IDLE;
    return buff_len; 
}

// implements the MqttNet->read function
static int WMQTT_NETGlue_Read(void* context, uint8_t* buff, int buff_len, int timeout_ms)
{
    return WMQTT_NETGlue_DoRead(context, buff, buff_len, timeout_ms, false);
}

#ifdef WOLFMQTT_SN
static int WMQTT_NETGlue_Peek(void* context, uint8_t* buff, int buff_len, int timeout_ms)
{
    return WMQTT_NETGlue_DoRead(context, buff, buff_len, timeout_ms, true);
}
#endif  // WOLFMQTT_SN

static int WMQTT_NETGlue_DoRead(void* context, uint8_t* buff, int buff_len, int timeout_ms, bool peek)
{
    WMQTT_NET_GLUE_RES netRes;
    WMQTT_NET_CONTEXT* net_glue_ctx = (WMQTT_NET_CONTEXT*)context;
    uint8_t* pBuff; 
    uint16_t rd_bytes;

    if (net_glue_ctx == 0 || buff == 0 || buff_len <= 0)
    {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if(net_glue_ctx->stat != NET_GLUE_SOCKET_READ)
    {   // first time we start the write op;
        WMQTT_NETGlue_SetTmo(net_glue_ctx, timeout_ms);
        net_glue_ctx->stat = NET_GLUE_SOCKET_READ;
        net_glue_ctx->opBytes = 0;
    }

    pBuff = buff + net_glue_ctx->opBytes; 
    if(peek)
    {
        rd_bytes = NET_PRES_SocketPeek(net_glue_ctx->sktH, pBuff, buff_len - net_glue_ctx->opBytes);
    }
    else
    {
        rd_bytes = NET_PRES_SocketRead(net_glue_ctx->sktH, pBuff, buff_len - net_glue_ctx->opBytes);
    }

    net_glue_ctx->opBytes += rd_bytes;
    if(net_glue_ctx->opBytes < buff_len)
    {   // check timeout!
        netRes = WMQTT_NETGlue_CheckConnection(net_glue_ctx);
        if(netRes != WMQTT_NET_GLUE_RES_OK)
        {
            return WMQTT_NETGlue_Error(net_glue_ctx, netRes, __func__, __LINE__);
        }
        return MQTT_CODE_CONTINUE;
    }

    // all good

    net_glue_ctx->stat = NET_GLUE_IDLE;
    return buff_len;
}


// implements the MqttNet->disconnect function
static int WMQTT_NETGlue_Disconnect(void* context)
{
    WMQTT_NET_CONTEXT* net_glue_ctx = (WMQTT_NET_CONTEXT*)context;
    if (net_glue_ctx)
    {
        if (net_glue_ctx->sktH != NET_PRES_INVALID_SOCKET)
        {
            NET_PRES_SocketClose(net_glue_ctx->sktH);
            net_glue_ctx->sktH = NET_PRES_INVALID_SOCKET;
        }

        net_glue_ctx->stat = NET_GLUE_IDLE;
    }
    return 0;
}




// returns an MqttPacketResponseCodes code accordingly and also display error messages if debug is enabled 
#if (WMQTT_NET_GLUE_DEBUG_ENABLE != 0) && (WMQTT_NET_GLUE_ERROR_STRINGS != 0)
static const char* WMQTT_NETGlue_ErrorStrTbl[] = 
{
    "OK",               // WMQTT_NET_GLUE_RES_OK
    "DNS",              // WMQTT_NET_GLUE_DNS_ERR
    "Skt Create",       // WMQTT_NET_GLUE_SKT_CREATE_ERR
    "Skt TX size",      // WMQTT_NET_GLUE_SKT_TX_SIZE_ERR
    "Skt RX size",      // WMQTT_NET_GLUE_SKT_RX_SIZE_ERR
    "Skt Bind",         // WMQTT_NET_GLUE_SKT_BIND_ERR
    "Skt Reset",        // WMQTT_NET_GLUE_SKT_RESET_ERR
    "Skt Timeout",      // WMQTT_NET_GLUE_TIMEOUT
    "Start Encryption", // WMQTT_NET_GLUE_START_ENCRYPT_ERR
    "TLS Negotiation",  // WMQTT_NET_GLUE_TLS_NEGOTIATION_ERR
};
#endif  // (WMQTT_NET_GLUE_DEBUG_ENABLE != 0) && (WMQTT_NET_GLUE_ERROR_STRINGS != 0)

static int WMQTT_NETGlue_Error(WMQTT_NET_CONTEXT* net_glue_ctx, WMQTT_NET_GLUE_RES res, const char* message, int lineNo)
{
    if(res < 0)
    {
#if (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)
#if (WMQTT_NET_GLUE_ERROR_STRINGS != 0)
        int abs_res = -res;
        const char* err_str;
        if(abs_res < sizeof(WMQTT_NETGlue_ErrorStrTbl) / sizeof(*WMQTT_NETGlue_ErrorStrTbl))
        {
            err_str = WMQTT_NETGlue_ErrorStrTbl[abs_res];
        }
        else
        {
            err_str = "Internal";
        }

        SYS_CONSOLE_PRINT("WMQTT_NET_GLUE Error: %s, occurred in func: %s, line: %d, \r\n", err_str, message, lineNo);
#else
        SYS_CONSOLE_PRINT("WMQTT_NET_GLUE Error: %d, occurred in func: %s, line: %d, \r\n", res, message, lineNo);
#endif  // (WMQTT_NET_GLUE_ERROR_STRINGS != 0)
#endif  // (WMQTT_NET_GLUE_DEBUG_ENABLE != 0)

        net_glue_ctx->stat =  NET_GLUE_IDLE;
        int wolfRes;
        switch(res)
        {
            case WMQTT_NET_GLUE_TIMEOUT:
                wolfRes = MQTT_CODE_ERROR_TIMEOUT;
                break;

            case WMQTT_NET_GLUE_START_ENCRYPT_ERR:
            case WMQTT_NET_GLUE_TLS_NEGOTIATION_ERR:
                wolfRes = MQTT_CODE_ERROR_TLS_CONNECT;
                break;

            default:
                wolfRes = MQTT_CODE_ERROR_NETWORK;
                break;


        }
        return wolfRes;
    }


    return res;
}

static void WMQTT_NETGlue_SetTmo(WMQTT_NET_CONTEXT* net_glue_ctx, int timeout_ms)
{
    if(timeout_ms == 0)
    {   // set a big timeout
        timeout_ms = 0xffffffff / 2;
    }

    net_glue_ctx->tmoTick = SYS_TMR_TickCountGet() + (timeout_ms * SYS_TMR_TickCounterFrequencyGet()) / 1000;
}

static WMQTT_NET_GLUE_RES WMQTT_NETGlue_CheckConnection(WMQTT_NET_CONTEXT* net_glue_ctx)
{
    if((int32_t)(SYS_TMR_TickCountGet() - net_glue_ctx->tmoTick) >= 0)
    {   // tmo
        return WMQTT_NET_GLUE_TIMEOUT;
    }
    else if(NET_PRES_SocketWasReset(net_glue_ctx->sktH) || NET_PRES_SocketWasDisconnected(net_glue_ctx->sktH))
    {
        return WMQTT_NET_GLUE_SKT_RESET_ERR;
    }

    return WMQTT_NET_GLUE_RES_OK; 
}


