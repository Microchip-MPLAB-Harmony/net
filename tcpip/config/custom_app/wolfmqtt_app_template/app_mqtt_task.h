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

/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_mqtt_task.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_MQTT_TASK_H_
#define _APP_MQTT_TASK_H_

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h" 
#include "tcpip/tcpip.h"
#include "wolfmqtt/mqtt_client.h"

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* APP MQTT Client Data

  Summary:
    Holds MQTT client data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

/* MQTT client application state */
typedef enum 
{
    APP_MQTT_STATE_IDLE    = 0,
    APP_MQTT_STATE_BEGIN,
    APP_MQTT_STATE_NET_INIT,
    APP_MQTT_STATE_BEGIN_SN,
    APP_MQTT_STATE_NET_SN_INIT,
    APP_MQTT_STATE_INIT,
    APP_MQTT_STATE_NET_CONNECT,
    APP_MQTT_STATE_CLIENT_CONNECT,
    APP_MQTT_STATE_SUBSCRIBE,
    APP_MQTT_STATE_PUBLISH,
    APP_MQTT_STATE_START_WAIT,
    APP_MQTT_STATE_WAIT_MSG,
    APP_MQTT_STATE_START_SUB_PING,
    APP_MQTT_STATE_SUB_PING,
    APP_MQTT_STATE_START_UNSUBSCRIBE,
    APP_MQTT_STATE_UNSUBSCRIBE,
    APP_MQTT_STATE_CLIENT_DISCONNECT,
    APP_MQTT_STATE_INIT_PING,
    APP_MQTT_STATE_WAIT_TO_PING,
    APP_MQTT_STATE_PING,
    APP_MQTT_STATE_NET_DISCONNECT,
    APP_MQTT_STATE_DONE,
    APP_MQTT_STATE_ERROR,
} APP_MQTT_CONTEXT_STATE;

// *****************************************************************************
/* APP MQTT Command

  Summary:
    Describes the MQTT command

  Description:
    This enumerated type describes the current MQTT command

  Remarks:
    None
 */

/* MQTT client command */
typedef enum 
{
    APP_MQTT_COMMAND_NONE       = 0,    // no command, invalid
    APP_MQTT_COMMAND_SUBSCRIBE,         // subscribe/publish
    APP_MQTT_COMMAND_PING,              // ping broker 

} APP_MQTT_COMMAND;

// demo app configuration
// adjust to your needs
#define APP_MQTT_MAX_APP_NAME_SIZE  32
#define APP_MQTT_DEFAULT_APP_NAME   "MQTT_NET_Client App" 

// Default MQTT broker to use
#define APP_MQTT_MAX_BROKER_SIZE    32
#define APP_MQTT_DEFAULT_BROKER     "test.mosquitto.org"
//#define APP_MQTT_DEFAULT_BROKER     "mqtt.eclipseprojects.io"
#define APP_MQTT_DEFAULT_BROKER_PORT 1883

#define APP_MQTT_MAX_CLIENT_SIZE    32
#define APP_MQTT_DEFAULT_CLIENT_ID  "MQTT_NET_Client"

#define APP_MQTT_MAX_SUBSCRIBE_TOPIC_SIZE     32
#define APP_MQTT_DEFAULT_SUBSCRIBE_TOPIC "MQTT_NET_Client topic"

#define APP_MQTT_MAX_PUBLISH_TOPIC_SIZE   32
#define APP_MQTT_DEFAULT_PUBLISH_TOPIC    "MQTT_NET_Client topic"

#define APP_MQTT_MAX_LWT_SIZE       32
#define APP_MQTT_DEFAULT_LWT_TOPIC_NAME "MQTT_NET_Client LWT topic"


#define APP_MQTT_MAX_PUBLISH_MESSAGE_SIZE   64
#define APP_MQTT_DEFAULT_PUBLISH_MESSAGE    "MQTT NET Demo Test Message"

#define APP_MQTT_MAX_USER_SIZE      64
#define APP_MQTT_DEFAULT_USER       "MQTT_NET_User"

#define APP_MQTT_MAX_PASS_SIZE      16
#define APP_MQTT_DEFAULT_PASS       "MQTT_NET_Pass"

#define APP_MQTT_TX_BUFF_SIZE       1024
#define APP_MQTT_RX_BUFF_SIZE       1024

#define APP_MQTT_MAX_KALIVE_TIMEOUT 1000
#define APP_MQTT_DEFAULT_KEEP_ALIVE 60

#define APP_MQTT_DEFAULT_CON_TIMEOUT    5000    // ms
#define APP_MQTT_DEFAULT_QOS            MQTT_QOS_0

#define APP_MQTT_DEFAULT_AUTH           false

#define APP_MQTT_DEFAULT_CMD_TIMEOUT_MS     30000
#define APP_MQTT_MAX_WAIT_TIMEOUT           120000

#define APP_MQTT_DEFAULT_PING_WAIT_MS       5000    // time between 2 pings, ms

#define APP_MQTT_CB_MESSAGE_BUFFER_SIZE     120

#define APP_MQTT_WAIT_MESSAGE_RETRIES       3

// WOLFMQTT_V5: max size of the MQTT control packet 
#define APP_MQTT_MAX_CTRL_PKT_SIZE          768

// WOLFMQTT_V5: Enhanced authentication method
#define APP_MQTT_DEFAULT_EAUTH_METHOD       "EXTERNAL"

// WOLFMQTT_V5: Default property content type
#define APP_MQTT_DEFAULT_CONTENT_TYPE       "wolf_type"

// WOLFMQTT_V5: Max client ID length
#define APP_MQTT_MAX_PROP_CLIENT_ID_LEN     32


/* MQTT Client application context */
typedef struct
{
    // current state
    uint16_t        currState;      // APP_MQTT_CONTEXT_STATE value
    uint16_t        errorState;     // APP_MQTT_CONTEXT_STATE value
    int             errorCode;
    uint32_t        currPacketId;

    // wolfMQTT containers
    MqttClient      mqttClient;
    MqttNet         mqttNet;
    MqttConnect     mqttConnect;
    MqttMessage     mqttLwtMsg;
    MqttSubscribe   mqttSubscribe;
    MqttUnsubscribe mqttUnsubscribe;
    MqttTopic       mqttTopic;
    MqttPublish     mqttPublish;
    MqttDisconnect  mqttDisconnect;

    //  app configuration
    uint8_t*        txBuff;
    uint8_t*        rxBuff;
    uint16_t        txBuffSize;
    uint16_t        rxBuffSize;

    uint8_t         qos;            // MqttQoS value
    uint8_t         forceTls;       // currently not used as NET_PRES handles it using the port number
    uint8_t         enableAuth;
    uint8_t         enableLwt;
    uint8_t         cleanSession;
    uint8_t         requestStop;
    uint8_t         msgReceived;
    uint8_t         mqttCommand;    //  current APP_MQTT_COMMAND

    uint16_t        brokerPort;
    uint16_t        waitMsgRetries;
    uint32_t        connTimeout;    // connection timeout, ms
    uint32_t        cmdTimeout;     // command timeout, ms
    uint32_t        tmoTick;        // tick when the wait expires
    char*           pClientId;
    uint16_t        keepAliveSec;
    uint16_t        pingCount;

#ifdef WOLFMQTT_V5
    uint16_t        maxPktSize;     // max packet accepted
    uint16_t        topicAlias;
    uint16_t        topicAliasMax;
    uint8_t         subIdAvailable; // Subscription ID available
    uint8_t         enableEAuth;    // Enhanced authentication
    MqttProp*       propList;       // list of added properties       
#ifdef WOLFMQTT_PROPERTY_CB    
    char            mqttPropClientId[APP_MQTT_MAX_PROP_CLIENT_ID_LEN + 1];
#endif  // WOLFMQTT_PROPERTY_CB    
#endif

    char            appName[APP_MQTT_MAX_APP_NAME_SIZE + 1] ;
    char            broker[APP_MQTT_MAX_BROKER_SIZE + 1];
    char            userName[APP_MQTT_MAX_USER_SIZE + 1];
    char            password[APP_MQTT_MAX_PASS_SIZE + 1];
    char            subscribeTopicName[APP_MQTT_MAX_SUBSCRIBE_TOPIC_SIZE + 1];
    char            publishTopicName[APP_MQTT_MAX_PUBLISH_TOPIC_SIZE + 1];
    char            clientId[APP_MQTT_MAX_CLIENT_SIZE + 1];
    char            lastWill[APP_MQTT_MAX_LWT_SIZE + 1];
    char            publishMessage[APP_MQTT_MAX_PUBLISH_MESSAGE_SIZE + 1];

} APP_MQTT_CONTEXT; 


// *****************************************************************************
// *****************************************************************************
// Section: External Declarations
// *****************************************************************************
// *****************************************************************************

bool        APP_MQTT_Init(void);

bool        APP_MQTT_CommandsInit(void);

void        APP_MQTT_Task(void);

APP_MQTT_CONTEXT*    APP_MQTT_GetContext(void);

const char* APP_MQTT_GetStateString(void);

#endif /* _APP_MQTT_TASK_H_ */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

