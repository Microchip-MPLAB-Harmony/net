/*******************************************************************************
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
*******************************************************************************/

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
    APP_MQTT_CONTEXT_STATE ctxtState = appMqttCtx.currState;
    if(0 <= ctxtState && ctxtState < sizeof(APP_MQTT_StateStrTbl) / sizeof(*APP_MQTT_StateStrTbl))
    {
        return APP_MQTT_StateStrTbl[ctxtState];
    }

    return "error";
}

// Local functions implementation

static bool APP_MQTT_InitContext(APP_MQTT_CONTEXT* mqttCtx)
{
    memset(mqttCtx, 0, sizeof(APP_MQTT_CONTEXT));

    strncpy(mqttCtx->broker, APP_MQTT_DEFAULT_BROKER, sizeof(mqttCtx->broker) - 1);
    strncpy(mqttCtx->clientId, APP_MQTT_DEFAULT_CLIENT_ID, sizeof(mqttCtx->clientId) - 1);
    strncpy(mqttCtx->subscribeTopicName, APP_MQTT_DEFAULT_SUBSCRIBE_TOPIC, sizeof(mqttCtx->subscribeTopicName) - 1);
    strncpy(mqttCtx->publishTopicName, APP_MQTT_DEFAULT_PUBLISH_TOPIC, sizeof(mqttCtx->publishTopicName) - 1);
    strncpy(mqttCtx->appName, APP_MQTT_DEFAULT_APP_NAME, sizeof(mqttCtx->appName) - 1);
    strncpy(mqttCtx->publishMessage, APP_MQTT_DEFAULT_PUBLISH_MESSAGE, sizeof(mqttCtx->publishMessage) - 1);
    strncpy(mqttCtx->lastWill, APP_MQTT_DEFAULT_LWT_TOPIC_NAME, sizeof(mqttCtx->lastWill) - 1);
    strncpy(mqttCtx->userName, APP_MQTT_DEFAULT_USER, sizeof(mqttCtx->userName) - 1);
    strncpy(mqttCtx->password, APP_MQTT_DEFAULT_PASS, sizeof(mqttCtx->password) - 1);

    mqttCtx->brokerPort = APP_MQTT_DEFAULT_BROKER_PORT;
    mqttCtx->qos = APP_MQTT_DEFAULT_QOS;
    mqttCtx->keepAliveSec = APP_MQTT_DEFAULT_KEEP_ALIVE;
    mqttCtx->cmdTimeout = APP_MQTT_DEFAULT_CMD_TIMEOUT_MS;
    mqttCtx->connTimeout = APP_MQTT_DEFAULT_CON_TIMEOUT;

    mqttCtx->currPacketId = (uint32_t)SYS_RANDOM_PseudoGet();

    mqttCtx->cleanSession = 1;

    mqttCtx->forceTls = 0;    // NET_PRES enables TLS, as needed
    mqttCtx->enableAuth = APP_MQTT_DEFAULT_AUTH; 

#ifdef WOLFMQTT_V5
    mqttCtx->maxPktSize = APP_MQTT_MAX_CTRL_PKT_SIZE;
    mqttCtx->topicAlias = 1;
    mqttCtx->topicAliasMax = 1;
#endif

    // setup the tx/rx buffers
    mqttCtx->txBuffSize = APP_MQTT_TX_BUFF_SIZE;
    mqttCtx->rxBuffSize = APP_MQTT_RX_BUFF_SIZE;
    mqttCtx->txBuff = (uint8_t*)malloc(mqttCtx->txBuffSize);
    mqttCtx->rxBuff = (uint8_t*)malloc(mqttCtx->rxBuffSize);

    return (mqttCtx->txBuff != 0 && mqttCtx->rxBuff != 0);
}

static void APP_MQTT_DeinitContext(APP_MQTT_CONTEXT* mqttCtx)
{
    if(mqttCtx->txBuff != 0)
    {
        free(mqttCtx->txBuff);
        mqttCtx->txBuff = 0;
    }

    if(mqttCtx->rxBuff != 0)
    {
        free(mqttCtx->rxBuff);
        mqttCtx->rxBuff = 0;
    }
}

void APP_MQTT_Task(void)
{
    APP_MQTT_CONTEXT* mqttCtx = &appMqttCtx;
#ifdef WOLFMQTT_V5
    MqttProp* mqttProp;
#endif  // WOLFMQTT_V5
    char ping_res_buff[40];


    int resCode;

    switch (mqttCtx->currState)
    {
        case APP_MQTT_STATE_IDLE:
            break;

        case APP_MQTT_STATE_BEGIN:
            SYS_CONSOLE_PRINT("MQTT Task - Client Start: QoS %d, broker %s\r\n", mqttCtx->qos, mqttCtx->broker);
            mqttCtx->currState = APP_MQTT_STATE_NET_INIT;
            mqttCtx->errorCode = MQTT_CODE_SUCCESS;
            mqttCtx->errorState = APP_MQTT_STATE_IDLE;
            break;

        case APP_MQTT_STATE_NET_INIT:
            // Initialize Net glue layer
            resCode = WMQTT_NETGlue_Initialize(&mqttCtx->mqttNet);
            APP_MQTT_ClientResult(mqttCtx, "WMQTT_NETGlue_Initialize", resCode) ;
            mqttCtx->currState = resCode < 0 ? APP_MQTT_STATE_DONE : APP_MQTT_STATE_INIT;
            break;

#ifdef WOLFMQTT_SN
        case APP_MQTT_STATE_BEGIN_SN:
            SYS_CONSOLE_PRINT("MQTT-SN Task - Client Start: QoS %d, broker %s\r\n", mqttCtx->qos, mqttCtx->broker);
            mqttCtx->currState = APP_MQTT_STATE_NET_SN_INIT;
            mqttCtx->errorCode = MQTT_CODE_SUCCESS;
            mqttCtx->errorState = APP_MQTT_STATE_IDLE;
            break;

        case APP_MQTT_STATE_NET_SN_INIT:
            // Initialize Net glue layer
            resCode = WMQTT_NETGlue_SNInitialize(&mqttCtx->mqttNet);
            APP_MQTT_ClientResult(mqttCtx, "WMQTT_NETGlue_SNInitialize", resCode) ;
            mqttCtx->currState = resCode < 0 ? APP_MQTT_STATE_DONE : APP_MQTT_STATE_INIT;
            break;
#endif  // WOLFMQTT_SN

        case APP_MQTT_STATE_INIT:
            mqttCtx->pClientId = mqttCtx->clientId;
#ifdef WOLFMQTT_V5
            mqttCtx->propList = 0;
#endif  // WOLFMQTT_V5

            // Initialize MqttClient structure
            resCode = MqttClient_Init(&mqttCtx->mqttClient, &mqttCtx->mqttNet, APP_MQTT_MessageHandler,
                    mqttCtx->txBuff, mqttCtx->txBuffSize, mqttCtx->rxBuff, mqttCtx->rxBuffSize,
                    mqttCtx->cmdTimeout);

            if (resCode == MQTT_CODE_CONTINUE)
            {   // wait some more
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Init", resCode) ;

            if (resCode != MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = APP_MQTT_STATE_DONE;
                break;
            }
            mqttCtx->mqttClient.ctx = mqttCtx;

#ifdef WOLFMQTT_DISCONNECT_CB
            /* setup disconnect callback */
            resCode = MqttClient_SetDisconnectCallback(&mqttCtx->mqttClient, APP_MQTT_DisconnectHandler, 0);
            APP_MQTT_ClientResult(mqttCtx, "MqttClient_SetDisconnectCallback", resCode) ;
            if(resCode < 0)
            {
                mqttCtx->currState = APP_MQTT_STATE_DONE;
                break;
            }
#endif  // WOLFMQTT_DISCONNECT_CB

#if defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)
            resCode = MqttClient_SetPropertyCallback(&mqttCtx->mqttClient, APP_MQTT_PropertyHandler, 0);
            APP_MQTT_ClientResult(mqttCtx, "MqttClient_SetPropertyCallback", resCode);
            if(resCode < 0)
            {
                mqttCtx->currState = APP_MQTT_STATE_DONE;
                break;
            }
#endif  //  defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)

            mqttCtx->currState = APP_MQTT_STATE_NET_CONNECT;
            break;

        case APP_MQTT_STATE_NET_CONNECT:
            // Connect to broker
            resCode = MqttClient_NetConnect(&mqttCtx->mqttClient, mqttCtx->broker, mqttCtx->brokerPort,
                    mqttCtx->connTimeout, mqttCtx->forceTls, APP_MQTT_TLSHandler);
            if (resCode == MQTT_CODE_CONTINUE)
            {
                break;
            }
            APP_MQTT_ClientResult(mqttCtx, "MqttClient_NetConnect", resCode) ;
            if (resCode != MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = APP_MQTT_STATE_DONE;
                break;
            }

            /* Build connect packet */
            memset(&mqttCtx->mqttConnect, 0, sizeof(mqttCtx->mqttConnect));
            mqttCtx->mqttConnect.keep_alive_sec = mqttCtx->keepAliveSec;
            mqttCtx->mqttConnect.clean_session = mqttCtx->cleanSession;
            mqttCtx->mqttConnect.client_id = mqttCtx->pClientId;

            /* Last will and testament sent by broker to subscribers
               of topic when broker connection is lost */
            memset(&mqttCtx->mqttLwtMsg, 0, sizeof(mqttCtx->mqttLwtMsg));
            mqttCtx->mqttConnect.lwt_msg = &mqttCtx->mqttLwtMsg;
            mqttCtx->mqttConnect.enable_lwt = mqttCtx->enableLwt;
            if (mqttCtx->enableLwt)
            {   // Send client id in LWT payload
                mqttCtx->mqttLwtMsg.qos = mqttCtx->qos;
                mqttCtx->mqttLwtMsg.retain = 0;
                mqttCtx->mqttLwtMsg.topic_name = mqttCtx->lastWill;
                mqttCtx->mqttLwtMsg.buffer = (uint8_t*)mqttCtx->pClientId;
                mqttCtx->mqttLwtMsg.total_len = (uint16_t)strlen(mqttCtx->pClientId);
            }

            if(mqttCtx->enableAuth)
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
            mqttCtx->currState = APP_MQTT_STATE_CLIENT_CONNECT;
            break;

        case APP_MQTT_STATE_CLIENT_CONNECT:
            // Send Connect and wait for Connect Ack
            resCode = MqttClient_Connect(&mqttCtx->mqttClient, &mqttCtx->mqttConnect);
            if (resCode == MQTT_CODE_CONTINUE)
            {
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Connect", resCode) ;
            if (resCode != MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                break;
            }

            // Validate Connect Ack
            SYS_CONSOLE_PRINT("MQTT Task - Connect Ack: Return Code %u, Session Present %s\r\n", mqttCtx->mqttConnect.ack.return_code,
                    (mqttCtx->mqttConnect.ack.flags & MQTT_CONNECT_ACK_FLAG_SESSION_PRESENT) ? "Yes" : "No");

#if defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)
            // print the acquired client ID
            SYS_CONSOLE_PRINT("MQTT Task - Connect Ack: Assigned Client ID: %s", mqttCtx->pClientId);
#endif  //  defined(WOLFMQTT_V5) && defined(WOLFMQTT_PROPERTY_CB)

            mqttCtx->pingCount = 0; 
            if(mqttCtx->mqttCommand == APP_MQTT_COMMAND_PING)
            {   // skip the pub/subscribe; jump to ping
                mqttCtx->currState = APP_MQTT_STATE_INIT_PING;
                break;
            } 
            // set the topic list
            memset(&mqttCtx->mqttSubscribe, 0, sizeof(mqttCtx->mqttSubscribe));
            mqttCtx->mqttTopic.topic_filter = mqttCtx->subscribeTopicName;
            mqttCtx->mqttTopic.qos = mqttCtx->qos;

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

            mqttCtx->currState = APP_MQTT_STATE_SUBSCRIBE;
            break;

        case APP_MQTT_STATE_SUBSCRIBE:

            resCode = MqttClient_Subscribe(&mqttCtx->mqttClient, &mqttCtx->mqttSubscribe);
            if (resCode == MQTT_CODE_CONTINUE)
            {   // wait some more
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Subscribe", resCode) ;
            if (resCode != MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                break;
            }

            // show the successful subscribed topic
            MqttTopic* pTopic = &mqttCtx->mqttSubscribe.topics[0];
            SYS_CONSOLE_PRINT("MQTT Task - Subscribed Topic %s, Qos %u\r\n", pTopic->topic_filter, pTopic->qos);

            // publish the topic
            memset(&mqttCtx->mqttPublish, 0, sizeof(MqttPublish));
            mqttCtx->mqttPublish.retain = 0;
            mqttCtx->mqttPublish.qos = mqttCtx->qos;
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
            
            mqttCtx->requestStop = false;
            mqttCtx->msgReceived = false;
            mqttCtx->waitMsgRetries = APP_MQTT_WAIT_MESSAGE_RETRIES;
            mqttCtx->currState = APP_MQTT_STATE_PUBLISH;
            break;

        case APP_MQTT_STATE_PUBLISH:
            resCode = MqttClient_Publish(&mqttCtx->mqttClient, &mqttCtx->mqttPublish);
            if (resCode == MQTT_CODE_CONTINUE)
            {
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Publish", resCode) ;

            if (resCode != MQTT_CODE_SUCCESS)
            {
                mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                break;
            }
            else
            {
                SYS_CONSOLE_PRINT("MQTT Task - Published Topic: %s\r\n", mqttCtx->mqttPublish.topic_name);
            }

            mqttCtx->currState = APP_MQTT_STATE_START_WAIT;
            // fall through

        case APP_MQTT_STATE_START_WAIT:
            SYS_CONSOLE_PRINT("MQTT Task - Waiting for message. Retry: %d\r\n", APP_MQTT_WAIT_MESSAGE_RETRIES - mqttCtx->waitMsgRetries + 1);
            APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
            mqttCtx->currState = APP_MQTT_STATE_WAIT_MSG;
            break;

        case APP_MQTT_STATE_WAIT_MSG:
            // check if stop requested
            if(mqttCtx->requestStop || mqttCtx->msgReceived)
            {
                if(mqttCtx->requestStop)
                {
                    SYS_CONSOLE_MESSAGE("MQTT Task - requested to stop. Exiting...\r\n");
                }
                else
                {
                    SYS_CONSOLE_MESSAGE("MQTT Task - published message was received. Exiting...\r\n");
                }
                mqttCtx->currState = APP_MQTT_STATE_START_UNSUBSCRIBE;
                break;
            }

            // read the receiving packets
            resCode = MqttClient_WaitMessage(&mqttCtx->mqttClient, mqttCtx->cmdTimeout);
            if(resCode == MQTT_CODE_CONTINUE)
            {   // check for timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_WaitMessage timeout", MQTT_CODE_ERROR_TIMEOUT);
                    APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
                    mqttCtx->currState = APP_MQTT_STATE_START_SUB_PING;
                }
            }
            else if(resCode < 0)
            {   // some error occurred
                APP_MQTT_ClientResult(mqttCtx, "MqttClient_WaitMessage error", resCode) ;
                // try to ping the broker
                mqttCtx->currState = APP_MQTT_STATE_START_SUB_PING;
            }
            // else wait some more to receive the message
            break;

        case APP_MQTT_STATE_START_SUB_PING:
            SYS_CONSOLE_PRINT("MQTT Task - sub pinging the broker: %d\r\n", mqttCtx->waitMsgRetries);
            APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
            mqttCtx->currState = APP_MQTT_STATE_SUB_PING;

        case APP_MQTT_STATE_SUB_PING:
            resCode = MqttClient_Ping(&mqttCtx->mqttClient);
            if (resCode == MQTT_CODE_CONTINUE)
            {   // check the timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_Ping timeout", MQTT_CODE_ERROR_TIMEOUT);
                    mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                }
                break;
            }

            // done with ping
            sprintf(ping_res_buff, "MqttClient_Ping #%d", ++mqttCtx->pingCount);
            APP_MQTT_ClientResult(mqttCtx, ping_res_buff, resCode);
            if (resCode < 0)
            {   // error occurred
                mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
            }
            else
            {   // ping was successful; but still timeout for our message; retry
                if(mqttCtx->waitMsgRetries != 0)
                {
                    mqttCtx->waitMsgRetries--;
                    mqttCtx->currState = APP_MQTT_STATE_START_WAIT;
                    SYS_CONSOLE_PRINT("MQTT Task - ping retrying: %d\r\n", mqttCtx->waitMsgRetries);
                }
                else
                {   // retries exhausted
                    SYS_CONSOLE_MESSAGE("MQTT Task - ping retries exhausted, but no message received! Aborting...\r\n");
                    mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                }
            }
            break;

        case APP_MQTT_STATE_START_UNSUBSCRIBE:
            // prepare to unsubscribe the topic
            memset(&mqttCtx->mqttUnsubscribe, 0, sizeof(mqttCtx->mqttUnsubscribe));
            mqttCtx->mqttUnsubscribe.packet_id = APP_MQTT_NewPacketId(mqttCtx);
            mqttCtx->mqttUnsubscribe.topic_count = 1;
            mqttCtx->mqttUnsubscribe.topics = &mqttCtx->mqttTopic;

            APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
            mqttCtx->currState = APP_MQTT_STATE_UNSUBSCRIBE;
            // no break; fall through

        case APP_MQTT_STATE_UNSUBSCRIBE:
            // unsubscribe the topics
            resCode = MqttClient_Unsubscribe(&mqttCtx->mqttClient, &mqttCtx->mqttUnsubscribe);

            if (resCode == MQTT_CODE_CONTINUE)
            {   // check timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_Unsubscribe", MQTT_CODE_ERROR_TIMEOUT);
                    mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                }
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Unsubscribe", resCode);
            mqttCtx->currState = resCode < 0 ? APP_MQTT_STATE_NET_DISCONNECT : APP_MQTT_STATE_CLIENT_DISCONNECT;
            break;

        case APP_MQTT_STATE_INIT_PING:
            SYS_CONSOLE_MESSAGE("MQTT Task - init pinging the broker\r\n");
            APP_MQTT_StartTimeout(mqttCtx, 1);  // start pinging immediately
            mqttCtx->requestStop = 0; 
            mqttCtx->currState = APP_MQTT_STATE_WAIT_TO_PING;
            break;
            
        case APP_MQTT_STATE_WAIT_TO_PING:
            if(mqttCtx->requestStop)
            {
                SYS_CONSOLE_MESSAGE("MQTT Task - requested to stop. Exiting...\r\n");
                mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                break;
            }

            // check the timeout
            if(APP_MQTT_CheckTimeout(mqttCtx))
            {   // time to ping
                APP_MQTT_StartTimeout(mqttCtx, mqttCtx->cmdTimeout);
                mqttCtx->currState = APP_MQTT_STATE_PING;
            }
            break;

        case APP_MQTT_STATE_PING:
            resCode = MqttClient_Ping(&mqttCtx->mqttClient);
            if (resCode == MQTT_CODE_CONTINUE)
            {   // check the timeout
                if(APP_MQTT_CheckTimeout(mqttCtx))
                {   // timeout
                    APP_MQTT_ClientResult(mqttCtx, "MqttClient_Ping timeout", MQTT_CODE_ERROR_TIMEOUT);
                    mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
                }
                break;
            }

            // done with ping
            sprintf(ping_res_buff, "MqttClient_Ping #%d", ++mqttCtx->pingCount);
            APP_MQTT_ClientResult(mqttCtx, ping_res_buff, resCode);
            if (resCode < 0)
            {   // error occurred
                mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
            }
            else
            {   // ping was successful; keep pinging
                APP_MQTT_StartTimeout(mqttCtx, APP_MQTT_DEFAULT_PING_WAIT_MS);
                mqttCtx->currState = APP_MQTT_STATE_WAIT_TO_PING;
            }
            break;

        case APP_MQTT_STATE_CLIENT_DISCONNECT:
            resCode = MqttClient_Disconnect_ex(&mqttCtx->mqttClient, &mqttCtx->mqttDisconnect);
            if (resCode == MQTT_CODE_CONTINUE)
            {   // wait some more
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_Disconnect_ex", resCode);
            mqttCtx->currState = APP_MQTT_STATE_NET_DISCONNECT;
            // fall through; no break

        case APP_MQTT_STATE_NET_DISCONNECT:
            resCode = MqttClient_NetDisconnect(&mqttCtx->mqttClient);
            if (resCode == MQTT_CODE_CONTINUE)
            {   // wait some more
                break;
            }

            APP_MQTT_ClientResult(mqttCtx, "MqttClient_NetDisconnect", resCode);
            mqttCtx->currState = APP_MQTT_STATE_DONE;
            // fall through; no break

        case APP_MQTT_STATE_DONE:
            // connection cycle ended; clean up
            WMQTT_NETGlue_Deinitialize(&mqttCtx->mqttNet);
            mqttCtx->mqttCommand = APP_MQTT_COMMAND_NONE;
            mqttCtx->currState = APP_MQTT_STATE_IDLE;
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
    uint8_t printBuff[APP_MQTT_CB_MESSAGE_BUFFER_SIZE + 1];
    uint32_t len;
    APP_MQTT_CONTEXT* mqttCtx = (APP_MQTT_CONTEXT*)mqttClient->ctx;


    if (msg_new)
    {   // print the topic
        len = msg->topic_name_len;
        if (len > sizeof(printBuff) - 1)
        {
            len = sizeof(printBuff) - 1;
        }
        memcpy(printBuff, msg->topic_name, len);
        printBuff[len] = '\0';

        SYS_CONSOLE_PRINT("MQTT Task - Received Topic: %s, Qos %d, Len %u\r\n", printBuff, msg->qos, msg->total_len);

        /* check that the published message was received */
        if (strncmp(mqttCtx->publishMessage, (char*)msg->buffer, msg->buffer_len) == 0)
        {   // signal that the expected message was received
            mqttCtx->msgReceived = true;
        }
    }

    // print the message payload
    len = msg->buffer_len;
    if (len > sizeof(printBuff) - 1)
    {
        len = sizeof(printBuff) - 1;
    }
    memcpy(printBuff, msg->buffer, len);
    printBuff[len] = '\0';

    SYS_CONSOLE_PRINT("MQTT Task - Payload (%d - %d): %s\r\n", msg->buffer_pos, msg->buffer_pos + len, printBuff);

    if (msg_done)
    {
        SYS_CONSOLE_MESSAGE("MQTT Task - Message: Done\r\n");
    }

    // Return negative to terminate publish processing
    return MQTT_CODE_SUCCESS;
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
            strncpy(mqttCtx->pClientId, mqttProp->data_str.str, APP_MQTT_MAX_PROP_CLIENT_ID_LEN);
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
    if((uint16_t)mqttCtx->currPacketId == 0)
    {
        mqttCtx->currPacketId++;
    }

    return (uint16_t)mqttCtx->currPacketId;
}

static void APP_MQTT_StartTimeout(APP_MQTT_CONTEXT* mqttCtx, uint32_t tmoMs)
{
    uint32_t currTick = SYS_TMR_TickCountGet();
    mqttCtx->tmoTick = currTick + (tmoMs * SYS_TMR_TickCounterFrequencyGet()) / 1000;
}
// returns true if timeout...
static bool APP_MQTT_CheckTimeout(APP_MQTT_CONTEXT* mqttCtx)
{
    uint32_t currTick = SYS_TMR_TickCountGet();

    return ((int32_t)(currTick - mqttCtx->tmoTick) >= 0);
}

/*******************************************************************************
 End of File
 */
