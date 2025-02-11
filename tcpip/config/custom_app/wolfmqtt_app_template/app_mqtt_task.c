/*
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_mqtt_task.h"

#include "mqtt_net_glue.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// local prototypes
//
static bool APP_MQTT_InitContext(APP_MQTT_CONTEXT* mqttCtx);
static void APP_MQTT_DeinitContext(APP_MQTT_CONTEXT* mqttCtx);

static void APP_MQTT_ClientResult(APP_MQTT_CONTEXT* mqttCtx, const char* message, int resCode);
static void APP_MQTT_StartTimeout(APP_MQTT_CONTEXT* mqttCtx, uint32_t tmoMs);
static bool APP_MQTT_CheckTimeout(APP_MQTT_CONTEXT* mqttCtx);
static uint16_t APP_MQTT_NewPacketId(APP_MQTT_CONTEXT* mqttCtx);

static int APP_MQTT_MessageHandler(MqttClient* mqttClient, MqttMessage *msg, uint8_t msg_new, uint8_t msg_done);

#ifdef WOLFMQTT_DISCONNECT_CB
static int APP_MQTT_DisconnectHandler(MqttClient* mqttClient, int error_code, void* ctx);
#endif

#if defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)
static int APP_MQTT_PropertyHandler(MqttClient* mqttClient, MqttProp *head, void *ctx);
static int APP_MQTT_ProcessProperty(MqttClient* mqttClient, APP_MQTT_CONTEXT* mqttCtx, MqttProp* mqttProp);
#endif  //  defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)

static int APP_MQTT_TLSHandler(MqttClient* mqttClient);

static const char*  APP_MQTT_StateStrTbl[] =
{
    "idle",             // APP_MQTT_STATE_IDLE,
    "begin",            // APP_MQTT_STATE_BEGIN,
    "net_init",         // APP_MQTT_STATE_NET_INIT,
    "begin_sn",         // APP_MQTT_STATE_BEGIN_SN,
    "net_init_sn",      // APP_MQTT_STATE_NET_SN_INIT,
    "init",             // APP_MQTT_STATE_INIT,
    "net_conect",       // APP_MQTT_STATE_NET_CONNECT,
    "cli_connect",      // APP_MQTT_STATE_CLIENT_CONNECT,
    "subscribe",        // APP_MQTT_STATE_SUBSCRIBE,
    "publish",          // APP_MQTT_STATE_PUBLISH,
    "wait_msg",         // APP_MQTT_STATE_START_WAIT,
    "wait_msg",         // APP_MQTT_STATE_WAIT_MSG,
    "sub_st_ping",      // APP_MQTT_STATE_START_SUB_PING,
    "sub_ping",         // APP_MQTT_STATE_SUB_PING,
    "unsubscribe",      // APP_MQTT_STATE_START_UNSUBSCRIBE,
    "unsubscribe",      // APP_MQTT_STATE_UNSUBSCRIBE,
    "cli_disconnect",   // APP_MQTT_STATE_CLIENT_DISCONNECT,
    "i_ping",           // APP_MQTT_STATE_INIT_PING,
    "w_ping",           // APP_MQTT_STATE_WAIT_TO_PING,
    "ping",             // APP_MQTT_STATE_PING,
    "net_disconnect",   // APP_MQTT_STATE_NET_DISCONNECT,
    "done",             // APP_MQTT_STATE_DONE,
};


// local data

static APP_MQTT_CONTEXT appMqttCtx;

// API Implementation

bool APP_MQTT_Init(void)
{
    // create MQTT command group
    if(!APP_MQTT_CommandsInit())
    {
        return false;
    }

    // initialize the MQTT context used in the application
    if(!APP_MQTT_InitContext(&appMqttCtx))
    {
        APP_MQTT_DeinitContext(&appMqttCtx);
        return false;
    }

    return true;
}

APP_MQTT_CONTEXT* APP_MQTT_GetContext(void)
{
    return &appMqttCtx; 
}

const char* APP_MQTT_GetStateString(void)
{
    int ctxtState = (int)appMqttCtx.currState;
    if(0 <= (int)ctxtState && (size_t)ctxtState < sizeof(APP_MQTT_StateStrTbl) / sizeof(*APP_MQTT_StateStrTbl))
    {
        return APP_MQTT_StateStrTbl[ctxtState];
    }

    return "error";
}

// Local functions implementation
static __inline__ size_t __attribute__((always_inline)) FC_MinSize(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

/* MISRA C-2012 Rule 21.3 deviated:4 Deviation record ID -  H3_MISRAC_2012_R_21_3_NET_DR_7 */
/* MISRA C-2012 Directive 4.12 deviated:2 Deviation record ID -  H3_MISRAC_2012_D_4_12_NET_DR_18 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:4 "MISRA C-2012 Rule 21.3" "H3_MISRAC_2012_R_21_3_NET_DR_7" 
#pragma coverity compliance block deviate:2 "MISRA C-2012 Directive 4.12" "H3_MISRAC_2012_D_4_12_NET_DR_18" 
static bool APP_MQTT_InitContext(APP_MQTT_CONTEXT* mqttCtx)
{
    (void)memset(mqttCtx, 0, sizeof(APP_MQTT_CONTEXT));

    (void)strncpy(mqttCtx->broker, APP_MQTT_DEFAULT_BROKER, FC_MinSize(sizeof(mqttCtx->broker) - 1U, strlen(APP_MQTT_DEFAULT_BROKER)));
    (void)strncpy(mqttCtx->clientId, APP_MQTT_DEFAULT_CLIENT_ID, FC_MinSize(sizeof(mqttCtx->clientId) - 1U, strlen(APP_MQTT_DEFAULT_CLIENT_ID)));
    (void)strncpy(mqttCtx->subscribeTopicName, APP_MQTT_DEFAULT_SUBSCRIBE_TOPIC, FC_MinSize(sizeof(mqttCtx->subscribeTopicName) - 1U, strlen(APP_MQTT_DEFAULT_CLIENT_ID)));
    (void)strncpy(mqttCtx->publishTopicName, APP_MQTT_DEFAULT_PUBLISH_TOPIC, FC_MinSize(sizeof(mqttCtx->publishTopicName) - 1U, strlen(APP_MQTT_DEFAULT_PUBLISH_TOPIC)));
    (void)strncpy(mqttCtx->appName, APP_MQTT_DEFAULT_APP_NAME, FC_MinSize(sizeof(mqttCtx->appName) - 1U, strlen(APP_MQTT_DEFAULT_APP_NAME)));
    (void)strncpy(mqttCtx->publishMessage, APP_MQTT_DEFAULT_PUBLISH_MESSAGE, FC_MinSize(sizeof(mqttCtx->publishMessage) - 1U, strlen(APP_MQTT_DEFAULT_PUBLISH_MESSAGE)));
    (void)strncpy(mqttCtx->lastWill, APP_MQTT_DEFAULT_LWT_TOPIC_NAME, FC_MinSize(sizeof(mqttCtx->lastWill) - 1U, strlen(APP_MQTT_DEFAULT_LWT_TOPIC_NAME)));
    (void)strncpy(mqttCtx->userName, APP_MQTT_DEFAULT_USER, FC_MinSize(sizeof(mqttCtx->userName) - 1U, strlen(APP_MQTT_DEFAULT_USER)));
    (void)strncpy(mqttCtx->password, APP_MQTT_DEFAULT_PASS, FC_MinSize(sizeof(mqttCtx->password) - 1U, strlen(APP_MQTT_DEFAULT_PASS)));

    mqttCtx->brokerPort = APP_MQTT_DEFAULT_BROKER_PORT;
    mqttCtx->qos = (uint8_t)APP_MQTT_DEFAULT_QOS;
    mqttCtx->keepAliveSec = APP_MQTT_DEFAULT_KEEP_ALIVE;
    mqttCtx->cmdTimeout = APP_MQTT_DEFAULT_CMD_TIMEOUT_MS;
    mqttCtx->connTimeout = APP_MQTT_DEFAULT_CON_TIMEOUT;

    mqttCtx->currPacketId = (uint32_t)SYS_RANDOM_PseudoGet();

    mqttCtx->cleanSession = 1;

    mqttCtx->forceTls = 0;    // NET_PRES enables TLS, as needed
    mqttCtx->enableAuth = (uint8_t)APP_MQTT_DEFAULT_AUTH; 

#ifdef WOLFMQTT_V5
    mqttCtx->maxPktSize = APP_MQTT_MAX_CTRL_PKT_SIZE;
    mqttCtx->topicAlias = 1;
    mqttCtx->topicAliasMax = 1;
#endif

    // setup the tx/rx buffers
    mqttCtx->txBuffSize = (uint16_t)APP_MQTT_TX_BUFF_SIZE;
    mqttCtx->rxBuffSize = (uint16_t)APP_MQTT_RX_BUFF_SIZE;
    mqttCtx->txBuff = (uint8_t*)malloc(mqttCtx->txBuffSize);
    mqttCtx->rxBuff = (uint8_t*)malloc(mqttCtx->rxBuffSize);

    return (mqttCtx->txBuff != NULL && mqttCtx->rxBuff != NULL);
}

static void APP_MQTT_DeinitContext(APP_MQTT_CONTEXT* mqttCtx)
{
    if(mqttCtx->txBuff != NULL)
    {
        free(mqttCtx->txBuff);
        mqttCtx->txBuff = NULL;
    }

    if(mqttCtx->rxBuff != NULL)
    {
        free(mqttCtx->rxBuff);
        mqttCtx->rxBuff = NULL;
    }
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.3"
#pragma coverity compliance end_block "MISRA C-2012 Directive 4.12"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


/* MISRA C-2012 Rule 21.6 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_21_6_NET_DR_3 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 21.6" "H3_MISRAC_2012_R_21_6_NET_DR_3" 
void APP_MQTT_Task(void)
{
    APP_MQTT_CONTEXT* mqttCtx = &appMqttCtx;
#ifdef WOLFMQTT_V5
    MqttProp* mqttProp;
#endif  // WOLFMQTT_V5
    MqttTopic* pTopic;
    char ping_res_buff[40];


    int resCode;

    switch (mqttCtx->currState)
    {
        case (uint16_t)APP_MQTT_STATE_IDLE:
            break;

        case (uint16_t)APP_MQTT_STATE_BEGIN:
            SYS_CONSOLE_PRINT("MQTT Task - Client Start: QoS %d, broker %s\r\n", mqttCtx->qos, mqttCtx->broker);
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_INIT;
            mqttCtx->errorCode = (int)MQTT_CODE_SUCCESS;
            mqttCtx->errorState = (uint16_t)APP_MQTT_STATE_IDLE;
            break;

        case (uint16_t)APP_MQTT_STATE_NET_INIT:
            // Initialize Net glue layer
            resCode = WMQTT_NETGlue_Initialize(&mqttCtx->mqttNet);
            APP_MQTT_ClientResult(mqttCtx, "WMQTT_NETGlue_Initialize", resCode) ;
            mqttCtx->currState = resCode < 0 ? (uint16_t)APP_MQTT_STATE_DONE : (uint16_t)APP_MQTT_STATE_INIT;
            break;

#ifdef WOLFMQTT_SN
        case (uint16_t)APP_MQTT_STATE_BEGIN_SN:
            SYS_CONSOLE_PRINT("MQTT-SN Task - Client Start: QoS %d, broker %s\r\n", mqttCtx->qos, mqttCtx->broker);
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_SN_INIT;
            mqttCtx->errorCode = (int)MQTT_CODE_SUCCESS;
            mqttCtx->(int)errorState = APP_MQTT_STATE_IDLE;
            break;

        case (uint16_t)APP_MQTT_STATE_NET_SN_INIT:
            // Initialize Net glue layer
            resCode = WMQTT_NETGlue_SNInitialize(&mqttCtx->mqttNet);
            APP_MQTT_ClientResult(mqttCtx, "WMQTT_NETGlue_SNInitialize", resCode) ;
            mqttCtx->currState = resCode < 0 ? (uint16_t)APP_MQTT_STATE_DONE : (uint16_t)APP_MQTT_STATE_INIT;
            break;
#endif  // WOLFMQTT_SN

        case (uint16_t)APP_MQTT_STATE_INIT:
            mqttCtx->pClientId = mqttCtx->clientId;
#ifdef WOLFMQTT_V5
            mqttCtx->propList = 0;
#endif  // WOLFMQTT_V5

            // Initialize MqttClient structure
            resCode = MqttClient_Init(&mqttCtx->mqttClient, &mqttCtx->mqttNet, &APP_MQTT_MessageHandler,
                    mqttCtx->txBuff, (int)mqttCtx->txBuffSize, mqttCtx->rxBuff, (int)mqttCtx->rxBuffSize,
                    (int)mqttCtx->cmdTimeout);

            if (resCode == (int)MQTT_CODE_CONTINUE)
            {   // wait some more
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Init", resCode) ;

            if (resCode != (int)MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_DONE;
                break;
            }
            mqttCtx->mqttClient.ctx = mqttCtx;

#ifdef WOLFMQTT_DISCONNECT_CB
            /* setup disconnect callback */
            resCode = MqttClient_SetDisconnectCallback(&mqttCtx->mqttClient, &APP_MQTT_DisconnectHandler, NULL);
            APP_MQTT_ClientResult(mqttCtx, "MqttClient_SetDisconnectCallback", resCode) ;
            if(resCode < 0)
            {
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_DONE;
                break;
            }
#endif  // WOLFMQTT_DISCONNECT_CB

#if defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)
            resCode = MqttClient_SetPropertyCallback(&mqttCtx->mqttClient, APP_MQTT_PropertyHandler, 0);
            APP_MQTT_ClientResult(mqttCtx, "MqttClient_SetPropertyCallback", resCode);
            if(resCode < 0)
            {
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_DONE;
                break;
            }
#endif  //  defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)

            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_CONNECT;
            break;

        case (uint16_t)APP_MQTT_STATE_NET_CONNECT:
            // Connect to broker
            resCode = MqttClient_NetConnect(&mqttCtx->mqttClient, mqttCtx->broker, mqttCtx->brokerPort,
                    (int)mqttCtx->connTimeout, (int)mqttCtx->forceTls, &APP_MQTT_TLSHandler);
            if (resCode == (int)MQTT_CODE_CONTINUE)
            {
                break;
            }
            APP_MQTT_ClientResult(mqttCtx, "MqttClient_NetConnect", resCode) ;
            if (resCode != (int)MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_DONE;
                break;
            }

            /* Build connect packet */
            (void)memset(&mqttCtx->mqttConnect, 0, sizeof(mqttCtx->mqttConnect));
            mqttCtx->mqttConnect.keep_alive_sec = mqttCtx->keepAliveSec;
            mqttCtx->mqttConnect.clean_session = mqttCtx->cleanSession;
            mqttCtx->mqttConnect.client_id = mqttCtx->pClientId;

            /* Last will and testament sent by broker to subscribers
               of topic when broker connection is lost */
            (void)memset(&mqttCtx->mqttLwtMsg, 0, sizeof(mqttCtx->mqttLwtMsg));
            mqttCtx->mqttConnect.lwt_msg = &mqttCtx->mqttLwtMsg;
            mqttCtx->mqttConnect.enable_lwt = mqttCtx->enableLwt;
            if (mqttCtx->enableLwt != 0U)
            {   // Send client id in LWT payload
                mqttCtx->mqttLwtMsg.qos = (MqttQoS)mqttCtx->qos;
                mqttCtx->mqttLwtMsg.retain = 0;
                mqttCtx->mqttLwtMsg.topic_name = mqttCtx->lastWill;
                mqttCtx->mqttLwtMsg.buffer = (uint8_t*)mqttCtx->pClientId;
                mqttCtx->mqttLwtMsg.total_len = (uint16_t)strlen(mqttCtx->pClientId);
            }

            if(mqttCtx->enableAuth != 0U)
            {   // Optional authentication
                mqttCtx->mqttConnect.username = mqttCtx->userName;
                mqttCtx->mqttConnect.password = mqttCtx->password;
            }
#ifdef WOLFMQTT_V5
            mqttCtx->mqttClient.packet_sz_max = mqttCtx->maxPktSize;
            mqttCtx->mqttClient.enable_eauth = mqttCtx->enableEAuth;

            if (mqttCtx->enableEAuth == 1)
            {   // enhanced auth
                /* Add property: Authentication Method */
                mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttConnect.props);
                if(mqttProp)
                {
                    mqttProp->type = MQTT_PROP_AUTH_METHOD;
                    mqttProp->data_str.str = (char*)APP_MQTT_DEFAULT_EAUTH_METHOD;
                    mqttProp->data_str.len = (uint16_t)strlen(mqttProp->data_str.str);
                }
            }
            // Request Response Info
            mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttConnect.props);
            if(mqttProp)
            {
                mqttProp->type = MQTT_PROP_REQ_RESP_INFO;
                mqttProp->data_byte = 1;
            }
            // Request Problem Info
            mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttConnect.props);
            if(mqttProp)
            {
                mqttProp->type = MQTT_PROP_REQ_PROB_INFO;
                mqttProp->data_byte = 1;
            }
            // Set the max packet size
            mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttConnect.props);
            if(mqttProp)
            {
                mqttProp->type = MQTT_PROP_MAX_PACKET_SZ;
                mqttProp->data_int = (uint32_t)mqttCtx->maxPktSize;
            }
            // set the topic alias max
            mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttConnect.props);
            if(mqttProp)
            {
                mqttProp->type = MQTT_PROP_TOPIC_ALIAS_MAX;
                mqttProp->data_short = mqttCtx->topicAliasMax;
            }

            mqttCtx->propList = mqttCtx->mqttConnect.props; 
#endif
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_CLIENT_CONNECT;
            break;

        case (uint16_t)APP_MQTT_STATE_CLIENT_CONNECT:
            // Send Connect and wait for Connect Ack
            resCode = MqttClient_Connect(&mqttCtx->mqttClient, &mqttCtx->mqttConnect);
            if (resCode == (int)MQTT_CODE_CONTINUE)
            {
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Connect", resCode) ;
            if (resCode != (int)MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                break;
            }

            // Validate Connect Ack
            SYS_CONSOLE_PRINT("MQTT Task - Connect Ack: Return Code %u, Session Present %s\r\n", mqttCtx->mqttConnect.ack.return_code,
                    (mqttCtx->mqttConnect.ack.flags & (uint8_t)MQTT_CONNECT_ACK_FLAG_SESSION_PRESENT) != 0U ? "Yes" : "No");

#if defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)
            // print the acquired client ID
            SYS_CONSOLE_PRINT("MQTT Task - Connect Ack: Assigned Client ID: %s", mqttCtx->pClientId);
#endif  //  defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)

            mqttCtx->pingCount = 0; 
            if(mqttCtx->mqttCommand == (uint8_t)APP_MQTT_COMMAND_PING)
            {   // skip the pub/subscribe; jump to ping
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_INIT_PING;
                break;
            } 
            // set the topic list
            (void)memset(&mqttCtx->mqttSubscribe, 0, sizeof(mqttCtx->mqttSubscribe));
            mqttCtx->mqttTopic.topic_filter = mqttCtx->subscribeTopicName;
            mqttCtx->mqttTopic.qos = (MqttQoS)mqttCtx->qos;

#ifdef WOLFMQTT_V5
            if (mqttCtx->subIdAvailable)
            {   // set the subscription identifier
                mqttCtx->mqttTopic.sub_id = 1;
                mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttSubscribe.props);
                if(mqttProp)
                {
                    mqttProp->type = MQTT_PROP_SUBSCRIPTION_ID;
                    mqttProp->data_int = mqttCtx->mqttTopic.sub_id;
                }
                mqttCtx->propList = mqttCtx->mqttSubscribe.props; 
            }
#endif

            // Subscribe Topic
            mqttCtx->mqttSubscribe.packet_id = APP_MQTT_NewPacketId(mqttCtx);
            mqttCtx->mqttSubscribe.topic_count = 1;
            mqttCtx->mqttSubscribe.topics = &mqttCtx->mqttTopic;

            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_SUBSCRIBE;
            break;

        case (uint16_t)APP_MQTT_STATE_SUBSCRIBE:

            resCode = MqttClient_Subscribe(&mqttCtx->mqttClient, &mqttCtx->mqttSubscribe);
            if (resCode == (int)MQTT_CODE_CONTINUE)
            {   // wait some more
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Subscribe", resCode) ;
            if (resCode != (int)MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                break;
            }

            // show the successful subscribed topic
            pTopic = &mqttCtx->mqttSubscribe.topics[0];
            SYS_CONSOLE_PRINT("MQTT Task - Subscribed Topic %s, Qos %u\r\n", pTopic->topic_filter, pTopic->qos);

            // publish the topic
            (void)memset(&mqttCtx->mqttPublish, 0, sizeof(MqttPublish));
            mqttCtx->mqttPublish.retain = 0;
            mqttCtx->mqttPublish.qos = (MqttQoS)mqttCtx->qos;
            mqttCtx->mqttPublish.duplicate = 0;
            mqttCtx->mqttPublish.topic_name = mqttCtx->publishTopicName;
            mqttCtx->mqttPublish.packet_id = APP_MQTT_NewPacketId(mqttCtx);
            mqttCtx->mqttPublish.buffer = (uint8_t*)mqttCtx->publishMessage;
            mqttCtx->mqttPublish.total_len = (uint16_t)strlen(mqttCtx->publishMessage);

#ifdef WOLFMQTT_V5
            // payload format indicator
            mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttPublish.props);
            if(mqttProp)
            {
                mqttProp->type = MQTT_PROP_PLAYLOAD_FORMAT_IND;
                mqttProp->data_int = 1;
            }
            // set the content type
            mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttPublish.props);
            if(mqttProp)
            {
                mqttProp->type = MQTT_PROP_CONTENT_TYPE;
                mqttProp->data_str.str = APP_MQTT_DEFAULT_CONTENT_TYPE;
                mqttProp->data_str.len = (uint16_t)strlen(mqttProp->data_str.str);
            }
            if(mqttCtx->topicAliasMax > mqttCtx->topicAlias && mqttCtx->topicAlias > 0)
            {   // set the topic alias
                mqttProp = MqttClient_PropsAdd(&mqttCtx->mqttPublish.props);
                if(mqttProp)
                {
                    mqttProp->type = MQTT_PROP_TOPIC_ALIAS;
                    mqttProp->data_short = mqttCtx->topicAlias;
                }
            }
            mqttCtx->propList = mqttCtx->mqttPublish.props; 
#endif
            
            mqttCtx->requestStop = 0U;
            mqttCtx->msgReceived = 0U;
            mqttCtx->waitMsgRetries = (uint16_t)APP_MQTT_WAIT_MESSAGE_RETRIES;
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_PUBLISH;
            break;

        case (uint16_t)APP_MQTT_STATE_PUBLISH:
        case (uint16_t)APP_MQTT_STATE_START_WAIT:
            if(mqttCtx->currState == (uint16_t)APP_MQTT_STATE_PUBLISH)
            {
                resCode = MqttClient_Publish(&mqttCtx->mqttClient, &mqttCtx->mqttPublish);
                if (resCode == (int)MQTT_CODE_CONTINUE)
                {
                    break;
                }

                APP_MQTT_ClientResult(mqttCtx, "MqttClient_Publish", resCode) ;

                if (resCode != (int)MQTT_CODE_SUCCESS)
                {
                    mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                    break;
                }
                else
                {
                    SYS_CONSOLE_PRINT("MQTT Task - Published Topic: %s\r\n", mqttCtx->mqttPublish.topic_name);
                }

                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_START_WAIT;
            }
            // case (uint16_t)APP_MQTT_STATE_START_WAIT:
            // fall through

            SYS_CONSOLE_PRINT("MQTT Task - Waiting for message. Retry: %d\r\n", (uint16_t)APP_MQTT_WAIT_MESSAGE_RETRIES - mqttCtx->waitMsgRetries + 1U);
            APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_WAIT_MSG;
            break;

        case (uint16_t)APP_MQTT_STATE_WAIT_MSG:
            // check if stop requested
            if(mqttCtx->requestStop != 0U || mqttCtx->msgReceived != 0U)
            {
                if(mqttCtx->requestStop != 0U )
                {
                    SYS_CONSOLE_MESSAGE("MQTT Task - requested to stop. Exiting...\r\n");
                }
                else
                {
                    SYS_CONSOLE_MESSAGE("MQTT Task - published message was received. Exiting...\r\n");
                }
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_START_UNSUBSCRIBE;
                break;
            }

            // read the receiving packets
            resCode = MqttClient_WaitMessage(&mqttCtx->mqttClient, (int)mqttCtx->cmdTimeout);
            if(resCode == (int)MQTT_CODE_CONTINUE)
            {   // check for timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_WaitMessage timeout", (int)MQTT_CODE_ERROR_TIMEOUT);
                    APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
                    mqttCtx->currState = (uint16_t)APP_MQTT_STATE_START_SUB_PING;
                }
            }
            else if(resCode < 0)
            {   // some error occurred
                APP_MQTT_ClientResult(mqttCtx, "MqttClient_WaitMessage error", resCode) ;
                // try to ping the broker
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_START_SUB_PING;
            }
            else
            {
                // else wait some more to receive the message
            }
            break;

        case (uint16_t)APP_MQTT_STATE_START_SUB_PING:
        case (uint16_t)APP_MQTT_STATE_SUB_PING:
            if(mqttCtx->currState == (uint16_t)APP_MQTT_STATE_START_SUB_PING)
            {
                SYS_CONSOLE_PRINT("MQTT Task - sub pinging the broker: %d\r\n", mqttCtx->waitMsgRetries);
                APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_SUB_PING;
            }
            // fall through
            // case (uint16_t)APP_MQTT_STATE_SUB_PING:
            resCode = MqttClient_Ping(&mqttCtx->mqttClient);
            if (resCode == (int)MQTT_CODE_CONTINUE)
            {   // check the timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_Ping timeout", (int)MQTT_CODE_ERROR_TIMEOUT);
                    mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                }
                break;
            }

            // done with ping
            (void)sprintf(ping_res_buff, "MqttClient_Ping #%d", ++mqttCtx->pingCount);
            APP_MQTT_ClientResult(mqttCtx, ping_res_buff, resCode);
            if (resCode < 0)
            {   // error occurred
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
            }
            else
            {   // ping was successful; but still timeout for our message; retry
                if(mqttCtx->waitMsgRetries != 0U)
                {
                    mqttCtx->waitMsgRetries--;
                    mqttCtx->currState = (uint16_t)APP_MQTT_STATE_START_WAIT;
                    SYS_CONSOLE_PRINT("MQTT Task - ping retrying: %d\r\n", mqttCtx->waitMsgRetries);
                }
                else
                {   // retries exhausted
                    SYS_CONSOLE_MESSAGE("MQTT Task - ping retries exhausted, but no message received! Aborting...\r\n");
                    mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                }
            }
            break;

        case (uint16_t)APP_MQTT_STATE_START_UNSUBSCRIBE:
        case (uint16_t)APP_MQTT_STATE_UNSUBSCRIBE:
            if(mqttCtx->currState == (uint16_t)APP_MQTT_STATE_START_UNSUBSCRIBE)
            {   // prepare to unsubscribe the topic
                (void)memset(&mqttCtx->mqttUnsubscribe, 0, sizeof(mqttCtx->mqttUnsubscribe));
                mqttCtx->mqttUnsubscribe.packet_id = APP_MQTT_NewPacketId(mqttCtx);
                mqttCtx->mqttUnsubscribe.topic_count = 1;
                mqttCtx->mqttUnsubscribe.topics = &mqttCtx->mqttTopic;

                APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_UNSUBSCRIBE;
            }
            // no break; fall through
            // case (uint16_t)APP_MQTT_STATE_UNSUBSCRIBE:
            // unsubscribe the topics
            resCode = MqttClient_Unsubscribe(&mqttCtx->mqttClient, &mqttCtx->mqttUnsubscribe);

            if (resCode == (int)MQTT_CODE_CONTINUE)
            {   // check timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_Unsubscribe", (int)MQTT_CODE_ERROR_TIMEOUT);
                    mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                }
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Unsubscribe", resCode);
            mqttCtx->currState = resCode < 0 ? (uint16_t)APP_MQTT_STATE_NET_DISCONNECT : (uint16_t)APP_MQTT_STATE_CLIENT_DISCONNECT;
            break;

        case (uint16_t)APP_MQTT_STATE_INIT_PING:
            SYS_CONSOLE_MESSAGE("MQTT Task - init pinging the broker\r\n");
            APP_MQTT_StartTimeout(mqttCtx, 1);  // start pinging immediately
            mqttCtx->requestStop = 0U; 
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_WAIT_TO_PING;
            break;
            
        case (uint16_t)APP_MQTT_STATE_WAIT_TO_PING:
            if(mqttCtx->requestStop != 0U)
            {
                SYS_CONSOLE_MESSAGE("MQTT Task - requested to stop. Exiting...\r\n");
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                break;
            }

            // check the timeout
            if(APP_MQTT_CheckTimeout(mqttCtx))
            {   // time to ping
                APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_PING;
            }
            break;

        case (uint16_t)APP_MQTT_STATE_PING:
            resCode = MqttClient_Ping(&mqttCtx->mqttClient);
            if (resCode == (int)MQTT_CODE_CONTINUE)
            {   // check the timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_Ping timeout", (int)MQTT_CODE_ERROR_TIMEOUT);
                    mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
                }
                break;
            }

            // done with ping
            (void)sprintf(ping_res_buff, "MqttClient_Ping #%d", ++mqttCtx->pingCount);
            APP_MQTT_ClientResult(mqttCtx, ping_res_buff, resCode);
            if (resCode < 0)
            {   // error occurred
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
            }
            else
            {   // ping was successful; keep pinging
                APP_MQTT_StartTimeout(mqttCtx, APP_MQTT_DEFAULT_PING_WAIT_MS);
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_WAIT_TO_PING;
            }
            break;

        case (uint16_t)APP_MQTT_STATE_CLIENT_DISCONNECT:
        case (uint16_t)APP_MQTT_STATE_NET_DISCONNECT:
            if(mqttCtx->currState == (uint16_t)APP_MQTT_STATE_CLIENT_DISCONNECT)
            {
                resCode = MqttClient_Disconnect_ex(&mqttCtx->mqttClient, &mqttCtx->mqttDisconnect);
                if (resCode == (int)MQTT_CODE_CONTINUE)
                {   // wait some more
                    break;
                }

                APP_MQTT_ClientResult(mqttCtx, "MqttClient_Disconnect_ex", resCode);
                mqttCtx->currState = (uint16_t)APP_MQTT_STATE_NET_DISCONNECT;
            }
            // fall through; no break
            // case (uint16_t)APP_MQTT_STATE_NET_DISCONNECT:

            resCode = MqttClient_NetDisconnect(&mqttCtx->mqttClient);
            if (resCode == (int)MQTT_CODE_CONTINUE)
            {   // wait some more
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_NetDisconnect", resCode);
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_DONE;
            break;

        case (uint16_t)APP_MQTT_STATE_DONE:
            // connection cycle ended; clean up
            (void)WMQTT_NETGlue_Deinitialize(&mqttCtx->mqttNet);
            mqttCtx->mqttCommand = (uint8_t)APP_MQTT_COMMAND_NONE;
            mqttCtx->currState = (uint16_t)APP_MQTT_STATE_IDLE;
            // print the result
            if(mqttCtx->errorCode == 0)
            {
                SYS_CONSOLE_MESSAGE("MQTT Task - MQTT cycle ended successfully!\r\n");
            }
            else
            {
#ifndef WOLFMQTT_NO_ERROR_STRINGS
                const char* errString = MqttClient_ReturnCodeToString(mqttCtx->errorCode);
                SYS_CONSOLE_PRINT("MQTT Task - MQTT cycle Failed in state: %d, error: %s!\r\n", mqttCtx->errorState, errString);
#else
                SYS_CONSOLE_PRINT("MQTT Task - MQTT cycle Failed in state: %d, error code: %d!\r\n", mqttCtx->errorState, mqttCtx->errorCode);
#endif  // WOLFMQTT_NO_ERROR_STRINGS
            }
            break;

        default:    // APP_MQTT_STATE_ERROR
            // should never happen
            SYS_CONSOLE_PRINT("MQTT Task - Error: %d\r\n", mqttCtx->currState);
            break;
    }
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


static void APP_MQTT_ClientResult(APP_MQTT_CONTEXT* mqttCtx, const char* message, int resCode)
{
#ifndef WOLFMQTT_NO_ERROR_STRINGS
    const char* resString = MqttClient_ReturnCodeToString(resCode);
    SYS_CONSOLE_PRINT("MQTT Task - run message: %s, res: %s\r\n", message, resString);
#else
    SYS_CONSOLE_PRINT("MQTT Task - run message: %s, res: %d\r\n", message, resCode);
#endif

    if(resCode < 0)
    {   // record only the first failure
        if(mqttCtx->errorCode == 0)
        {
            mqttCtx->errorState = mqttCtx->currState;
            mqttCtx->errorCode = resCode;
        }
    }

#ifdef WOLFMQTT_V5
if (mqttCtx->propList != 0)
    {   // release the allocated properties
        MqttClient_PropsFree(mqttCtx->propList);
        mqttCtx->propList = 0;
    }
#endif
}

static int APP_MQTT_MessageHandler(MqttClient* mqttClient, MqttMessage *msg, uint8_t msg_new, uint8_t msg_done)
{
    union
    {
        uint8_t u8[APP_MQTT_CB_MESSAGE_BUFFER_SIZE + 1];
        char    c8[APP_MQTT_CB_MESSAGE_BUFFER_SIZE + 1];
    }uprintBuff;

    uint32_t len;
    APP_MQTT_CONTEXT* mqttCtx = (APP_MQTT_CONTEXT*)mqttClient->ctx;


    if (msg_new != 0U)
    {   // print the topic
        len = msg->topic_name_len;
        if (len > sizeof(uprintBuff) - 1U)
        {
            len = sizeof(uprintBuff) - 1U;
        }
        (void)memcpy(uprintBuff.c8, msg->topic_name, len);
        uprintBuff.c8[len] = '\0';

        SYS_CONSOLE_PRINT("MQTT Task - Received Topic: %s, Qos %d, Len %u\r\n", uprintBuff.c8, msg->qos, msg->total_len);

        /* check that the published message was received */
        if (strncmp(mqttCtx->publishMessage, (char*)msg->buffer, msg->buffer_len) == 0)
        {   // signal that the expected message was received
            mqttCtx->msgReceived = 1U;
        }
    }

    // print the message payload
    len = msg->buffer_len;
    if (len > sizeof(uprintBuff) - 1U)
    {
        len = sizeof(uprintBuff) - 1U;
    }
    (void)memcpy(uprintBuff.u8, msg->buffer, len);
    uprintBuff.u8[len] = 0;

    SYS_CONSOLE_PRINT("MQTT Task - Payload (%d - %d): %s\r\n", msg->buffer_pos, msg->buffer_pos + len, uprintBuff.c8);

    if (msg_done != 0U)
    {
        SYS_CONSOLE_MESSAGE("MQTT Task - Message: Done\r\n");
    }

    // Return negative to terminate publish processing
    return (int)MQTT_CODE_SUCCESS;
}

#if defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)
// The property callback is called after decoding a packet that contains at least one property.
// The property list is deallocated after returning from the callback.
static int APP_MQTT_PropertyHandler(MqttClient* mqttClient, MqttProp* head, void* ctx)
{
    (void)ctx;
    if(mqttClient == 0 || mqttClient->ctx == 0)
    {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    MqttProp* mqttProp = head;
    APP_MQTT_CONTEXT* mqttCtx = (APP_MQTT_CONTEXT*)mqttClient->ctx;

    int resCode = 0;
    while (mqttProp)
    {
        resCode = APP_MQTT_ProcessProperty(mqttClient, mqttCtx, mqttProp);
        if(resCode < 0)
        {   // some error occurred
            return resCode;
        }

        // keep processing
        mqttProp = mqttProp->next;
    }

    return 0;
}

static int APP_MQTT_ProcessProperty(MqttClient* mqttClient, APP_MQTT_CONTEXT* mqttCtx, MqttProp* mqttProp)
{

    switch (mqttProp->type)
    {
        case MQTT_PROP_ASSIGNED_CLIENT_ID:
            // store the assigned client ID
            mqttCtx->pClientId = mqttCtx->mqttPropClientId;
            (void)strncpy(mqttCtx->pClientId, mqttProp->data_str.str, APP_MQTT_MAX_PROP_CLIENT_ID_LEN);
            break;

        case MQTT_PROP_SUBSCRIPTION_ID_AVAIL:
            mqttCtx->subIdAvailable = mqttProp->data_byte != 0;
            break;

        case MQTT_PROP_TOPIC_ALIAS_MAX:
            if(mqttCtx->topicAliasMax >= mqttProp->data_short)
            {
                mqttCtx->topicAliasMax = mqttProp->data_short;
            }
            break;

        case MQTT_PROP_MAX_PACKET_SZ:
            if (mqttProp->data_int <= 0 || mqttProp->data_int > MQTT_PACKET_SZ_MAX)
            {   // Protocol error
                return MQTT_CODE_ERROR_PROPERTY;
            }

            if(mqttClient->packet_sz_max >= mqttProp->data_int)
            {
                mqttClient->packet_sz_max = mqttProp->data_int;
            }

            break;

        case MQTT_PROP_SERVER_KEEP_ALIVE:
            mqttCtx->keepAliveSec = mqttProp->data_short;
            break;

        case MQTT_PROP_MAX_QOS:
            mqttClient->max_qos = mqttProp->data_byte;
            break;

        case MQTT_PROP_RETAIN_AVAIL:
            mqttClient->retain_avail = mqttProp->data_byte;
            break;

        case MQTT_PROP_REASON_STR:
            SYS_CONSOLE_PRINT("MQTT Task - PropertyHandler: Reason String: %s", mqttProp->data_str.str);
            break;

        case MQTT_PROP_PLAYLOAD_FORMAT_IND:
        case MQTT_PROP_MSG_EXPIRY_INTERVAL:
        case MQTT_PROP_CONTENT_TYPE:
        case MQTT_PROP_RESP_TOPIC:
        case MQTT_PROP_CORRELATION_DATA:
        case MQTT_PROP_SUBSCRIPTION_ID:
        case MQTT_PROP_SESSION_EXPIRY_INTERVAL:
        case MQTT_PROP_TOPIC_ALIAS:
        case MQTT_PROP_TYPE_MAX:
        case MQTT_PROP_RECEIVE_MAX:
        case MQTT_PROP_USER_PROP:
        case MQTT_PROP_WILDCARD_SUB_AVAIL:
        case MQTT_PROP_SHARED_SUBSCRIPTION_AVAIL:
        case MQTT_PROP_RESP_INFO:
        case MQTT_PROP_SERVER_REF:
        case MQTT_PROP_AUTH_METHOD:
        case MQTT_PROP_AUTH_DATA:
            // not processed - ignore
            break;

        case MQTT_PROP_REQ_PROB_INFO:
        case MQTT_PROP_WILL_DELAY_INTERVAL:
        case MQTT_PROP_REQ_RESP_INFO:
        case MQTT_PROP_NONE:
        default:
            // Invalid
            return MQTT_CODE_ERROR_PROPERTY;
    }

    return 0;
}
#endif  //  defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)

static int APP_MQTT_TLSHandler(MqttClient* mqttClient)
{
    (void)mqttClient;
    return 0;
}

#ifdef WOLFMQTT_DISCONNECT_CB
static int APP_MQTT_DisconnectHandler(MqttClient* mqttClient, int error_code, void* ctx)
{
    (void)ctx;
    APP_MQTT_CONTEXT* mqttCtx = (APP_MQTT_CONTEXT*)mqttClient->ctx;
    SYS_CONSOLE_PRINT("MQTT Task - Client: %s, Disconnect error: %d\r\n", mqttCtx->pClientId, error_code);
    return 0;
}
#endif  // WOLFMQTT_DISCONNECT_CB


static uint16_t APP_MQTT_NewPacketId(APP_MQTT_CONTEXT* mqttCtx)
{
    mqttCtx->currPacketId++;
    if((uint16_t)mqttCtx->currPacketId == 0U)
    {
        mqttCtx->currPacketId++;
    }

    return (uint16_t)mqttCtx->currPacketId;
}

static void APP_MQTT_StartTimeout(APP_MQTT_CONTEXT* mqttCtx, uint32_t tmoMs)
{
    uint32_t currTick = SYS_TMR_TickCountGet();
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    mqttCtx->tmoTick = currTick + (tmoMs * sysFreq) / 1000U;
}
// returns true if timeout...
static bool APP_MQTT_CheckTimeout(APP_MQTT_CONTEXT* mqttCtx)
{
    uint32_t currTick = SYS_TMR_TickCountGet();

    return ((int32_t)currTick - (int32_t)mqttCtx->tmoTick) >= 0;
}

/*******************************************************************************
 End of File
 */
