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

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// local prototypes
//

static void     Command_Mqtt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void     CmdMqttStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttPing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttStop(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttQos(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttKalive(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttId(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttLwtMessage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttUser(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttPass(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttSubsTopic(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttPublishTopic(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttBroker(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttName(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttTmo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttTls(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttAuth(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttLwt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttTxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttRxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttMessage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttClean(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
#ifdef WOLFMQTT_V5
static void     CmdMqttCtrlSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
static void     CmdMqttEAuth(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
#endif  //  WOLFMQTT_V5
static void     CmdMqttTime(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);

static bool     ConvStringtoInt(char* str, int* pRes, int lim_low, int lim_high);
static void     MqttChangeBufferSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx, bool isTx);

// local data
static const SYS_CMD_DESCRIPTOR    mqttCmdTbl[]=
{
    {"mqtt",     Command_Mqtt,    "MQTT commands"},
};

typedef void    (*command_mqtt_fnc)(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx);
typedef struct
{
    const char*         cmdString;      // string identifying the command
    command_mqtt_fnc    fnc;            // processing function
    uint16_t            minParams;      // min required number of parameters
    uint16_t            maxParams;      // max number of parameters
    const char*         cmdHelp;        // brief command description
}CMD_MQTT_PROC_DCPT;

#define     CMD_MQTT_HELP_START     "Starts an MQTT/MQTT-SN connection to broker"
#define     CMD_MQTT_HELP_PING      "Starts an MQTT/MQTT-SN connection to ping the broker"
#define     CMD_MQTT_HELP_STOP      "Stops an ongoing MQTT connection"
#define     CMD_MQTT_HELP_STAT      "Prints the current MQTT task state"
#define     CMD_MQTT_HELP_PORT      "Sets the MQTT broker connection port"
#define     CMD_MQTT_HELP_QOS       "Sets the MQTT connection QoS value"
#define     CMD_MQTT_HELP_KALIVE    "Sets the MQTT connection Keep Alive interval, seconds"
#define     CMD_MQTT_HELP_ID        "Sets the MQTT client Id"
#define     CMD_MQTT_HELP_LWT_MESSAGE "Sets the MQTT Last Will and Testament message"
#define     CMD_MQTT_HELP_MESSAGE   "Sets the message to be published"
#define     CMD_MQTT_HELP_USER      "Sets the MQTT connection user name"
#define     CMD_MQTT_HELP_PASS      "Sets the MQTT connection password"
#define     CMD_MQTT_HELP_SUBSCRIBE "Sets the MQTT subscribe topic name"
#define     CMD_MQTT_HELP_PUBLISH   "Sets the MQTT publish topic name"
#define     CMD_MQTT_HELP_BROKER    "Sets the MQTT broker to connect to"
#define     CMD_MQTT_HELP_NAME      "Sets the MQTT application name"
#define     CMD_MQTT_HELP_TMO       "Sets the MQTT connection wait reply timeout"
#define     CMD_MQTT_HELP_TLS       "Sets the MQTT connection force TLS mode"
#define     CMD_MQTT_HELP_AUTH      "Sets the MQTT connection authentication mode"
#define     CMD_MQTT_HELP_LWT       "Sets the MQTT LWT mode"
#define     CMD_MQTT_HELP_TX_SIZE   "Sets the MQTT connection TX buffer size"
#define     CMD_MQTT_HELP_RX_SIZE   "Sets the MQTT connection RX buffer size"
#define     CMD_MQTT_HELP_CLEAN     "Sets the MQTT clean session flag"
#define     CMD_MQTT_HELP_CTRL_SIZE "Sets the MQTT control packet size"
#define     CMD_MQTT_HELP_EAUTH     "Sets the MQTT V5 enhanced authentication mode"
#define     CMD_MQTT_HELP_HELP      "This help command"
#define     CMD_MQTT_HELP_TIME      "Gets the SNTP time"

static const CMD_MQTT_PROC_DCPT cmd_mqtt_proc_dcpt_tbl[] =
{
    // string               // fnc              // minParams    // maxParams    // cmdHelp
    {"start",               CmdMqttStart,        2,              3,             CMD_MQTT_HELP_START },
    {"ping",                CmdMqttPing,         2,              2,             CMD_MQTT_HELP_PING },
    {"stop",                CmdMqttStop,         2,              2,             CMD_MQTT_HELP_STOP  },
    {"stat",                CmdMqttStat,         2,              2,             CMD_MQTT_HELP_STAT  },
    {"port",                CmdMqttPort,         2,              3,             CMD_MQTT_HELP_PORT  },
    {"qos",                 CmdMqttQos,          2,              3,             CMD_MQTT_HELP_QOS   },
    {"kalive",              CmdMqttKalive,       2,              3,             CMD_MQTT_HELP_KALIVE},
    {"id",                  CmdMqttId,           2,              3,             CMD_MQTT_HELP_ID    },
    {"lwtmsg",              CmdMqttLwtMessage,   2,              3,             CMD_MQTT_HELP_LWT_MESSAGE},
    {"message",             CmdMqttMessage,      2,              3,             CMD_MQTT_HELP_MESSAGE},
    {"user",                CmdMqttUser,         2,              3,             CMD_MQTT_HELP_USER  },
    {"pass",                CmdMqttPass,         2,              3,             CMD_MQTT_HELP_PASS  },
    {"subscribe",           CmdMqttSubsTopic,    2,              3,             CMD_MQTT_HELP_SUBSCRIBE },
    {"publish",             CmdMqttPublishTopic, 2,              3,             CMD_MQTT_HELP_PUBLISH },
    {"broker",              CmdMqttBroker,       2,              3,             CMD_MQTT_HELP_BROKER},
    {"name",                CmdMqttName,         2,              3,             CMD_MQTT_HELP_NAME},
    {"tmo",                 CmdMqttTmo,          2,              3,             CMD_MQTT_HELP_TMO   },
    {"tls",                 CmdMqttTls,          2,              3,             CMD_MQTT_HELP_TLS   },
    {"auth",                CmdMqttAuth,         2,              3,             CMD_MQTT_HELP_AUTH  },
    {"lwt",                 CmdMqttLwt,          2,              3,             CMD_MQTT_HELP_LWT   },
    {"txbuf",               CmdMqttTxSize,       2,              3,             CMD_MQTT_HELP_TX_SIZE},
    {"rxbuf",               CmdMqttRxSize,       2,              3,             CMD_MQTT_HELP_RX_SIZE},
    {"clean",               CmdMqttClean,        2,              3,             CMD_MQTT_HELP_CLEAN},
#ifdef WOLFMQTT_V5
    {"ctrlsize",            CmdMqttCtrlSize,     2,              3,             CMD_MQTT_HELP_CTRL_SIZE},
    {"eauth",               CmdMqttEAuth,        2,              3,             CMD_MQTT_HELP_EAUTH  },
#endif  //  WOLFMQTT_V5
    {"help",                CmdMqttHelp,         2,              2,             CMD_MQTT_HELP_HELP  },
    {"time",                CmdMqttTime,         2,              2,             CMD_MQTT_HELP_TIME  },
};


// API Implementation

bool APP_MQTT_CommandsInit(void)
{
    // create MQTT command group
    if (!SYS_CMD_ADDGRP(mqttCmdTbl, sizeof(mqttCmdTbl) / sizeof(*mqttCmdTbl), "mqtt", ": mqtt commands"))
    {
        SYS_CONSOLE_MESSAGE("Failed to create mqtt Commands\r\n");
        return false;
    }

    SYS_CONSOLE_MESSAGE("Created the mqtt Commands\r\n");
    return true;
}


// Local functions implementation

static void Command_Mqtt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(argc == 1)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mqtt command takes at least one parameter. Try 'mqtt help\r\n");
        return;
    }

    int ix;
    const CMD_MQTT_PROC_DCPT* procDcpt = cmd_mqtt_proc_dcpt_tbl;

    for(ix = 0; ix < sizeof(cmd_mqtt_proc_dcpt_tbl) / sizeof(*cmd_mqtt_proc_dcpt_tbl); ix++, procDcpt++)
    {
        if(strcmp(argv[1], procDcpt->cmdString) == 0)
        {   // found command
            if(argc < procDcpt->minParams || argc > procDcpt->maxParams)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: command %s takes min %d and max %d parameters.\r\n", procDcpt->cmdString, procDcpt->minParams, procDcpt->maxParams);
            }
            else
            {
                APP_MQTT_CONTEXT* pMqttCtx = APP_MQTT_GetContext();
                (*procDcpt->fnc)(pCmdIO, argc, argv, pMqttCtx);
            }
            return;
        }
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: command %s unknown. Try 'mqtt help'\r\n", argv[1]);
}


static void CmdMqttStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(pMqttCtx->currState != APP_MQTT_STATE_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "MQTT demo is already running\r\n");
        return;
    }

    // restart the test

#ifdef WOLFMQTT_SN
    bool useSN = false;
    if(argc > 2)
    {
        if(strcmp(argv[2], "sn") == 0)
        {
            useSN = true;
        }
        else
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: param %s unknown. Use 'sn' for MQTT-SN\r\n", argv[2]);
            return;
        }
    }

    pMqttCtx->mqttCommand = APP_MQTT_COMMAND_SUBSCRIBE;
    pMqttCtx->currState = useSN ? APP_MQTT_STATE_BEGIN_SN : APP_MQTT_STATE_BEGIN;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "MQTT%sdemo has been started\r\n", useSN ? "-SN " : " ");
#else
    pMqttCtx->mqttCommand = APP_MQTT_COMMAND_SUBSCRIBE;
    pMqttCtx->currState = APP_MQTT_STATE_BEGIN;
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "MQTT pub/sub demo has been started\r\n");
#endif  // WOLFMQTT_SN
}

static void CmdMqttPing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(pMqttCtx->currState != APP_MQTT_STATE_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "MQTT demo is already running\r\n");
        return;
    }

    // restart the test
    pMqttCtx->mqttCommand = APP_MQTT_COMMAND_PING;
    pMqttCtx->currState = APP_MQTT_STATE_BEGIN;
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "MQTT ping demo has been started\r\n");
}

static void CmdMqttStop(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(pMqttCtx->currState == APP_MQTT_STATE_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "MQTT demo is already stopped\r\n");
    }
    else
    {
        pMqttCtx->requestStop = true;
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "MQTT demo has been requested to stop\r\n");
    }
}

static void CmdMqttStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    const char* stateStr = APP_MQTT_GetStateString();

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "MQTT task current state is: %s\r\n", stateStr);
}

static void CmdMqttPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int portNo;
        bool res = ConvStringtoInt(argv[2], &portNo, 0, 65535);
        if(res == false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid port no: %s\r\n", argv[2]);
            return;
        }

        pMqttCtx->brokerPort = portNo; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: broker port is: %d\r\n", pMqttCtx->brokerPort);
}

static void CmdMqttQos(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int qos;
        bool res = ConvStringtoInt(argv[2], &qos, MQTT_QOS_0, MQTT_QOS_2);
        if(res == false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid QoS: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->qos = (MqttQoS)qos; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Qos is: %d\r\n", pMqttCtx->qos);
}

static void CmdMqttKalive(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int keepAlive;

        bool res = ConvStringtoInt(argv[2], &keepAlive, 0, APP_MQTT_MAX_KALIVE_TIMEOUT);
        if(res ==  false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid Keep_Alive timeout: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->keepAliveSec = keepAlive; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Keep_Alive timeout is: %d\r\n", pMqttCtx->keepAliveSec);
}

static void CmdMqttId(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        strncpy(pMqttCtx->clientId, argv[2], sizeof(pMqttCtx->clientId) - 1); 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Client id is: %s\r\n", pMqttCtx->clientId);
}

static void CmdMqttLwtMessage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    int currIx = 2;
    argc -= 2;

    while(argc)
    {
        strncat(pMqttCtx->lastWill, argv[currIx], sizeof(pMqttCtx->lastWill) - 1 - strlen(pMqttCtx->lastWill));
        argc--;
        currIx++;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Client LWT message is: %s\r\n", pMqttCtx->lastWill);
}

static void CmdMqttMessage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    int currIx = 2;
    argc -= 2;

    while(argc)
    {
        strncat(pMqttCtx->publishMessage, argv[currIx], sizeof(pMqttCtx->publishMessage) - 1 - strlen(pMqttCtx->publishMessage));
        argc--;
        currIx++;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Publish message is: %s\r\n", pMqttCtx->publishMessage);
}

static void CmdMqttUser(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        strncpy(pMqttCtx->userName, argv[2], sizeof(pMqttCtx->userName) - 1); 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: User is: %s\r\n", pMqttCtx->userName);
}

static void CmdMqttPass(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        strncpy(pMqttCtx->password, argv[2], sizeof(pMqttCtx->password) - 1); 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Password is: %s\r\n", pMqttCtx->password);
}

static void CmdMqttSubsTopic(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        strncpy(pMqttCtx->subscribeTopicName, argv[2], sizeof(pMqttCtx->subscribeTopicName) - 1); 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Subscribe Topic is: %s\r\n", pMqttCtx->subscribeTopicName);
}

static void CmdMqttPublishTopic(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        strncpy(pMqttCtx->publishTopicName, argv[2], sizeof(pMqttCtx->publishTopicName) - 1); 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Publish Topic is: %s\r\n", pMqttCtx->publishTopicName);
}

static void CmdMqttBroker(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        strncpy(pMqttCtx->broker, argv[2], sizeof(pMqttCtx->broker) - 1); 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Broker is: %s\r\n", pMqttCtx->broker);
}

static void CmdMqttName(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        strncpy(pMqttCtx->appName, argv[2], sizeof(pMqttCtx->appName) - 1); 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: App name is: %s\r\n", pMqttCtx->appName);
}

static void CmdMqttTmo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int timeout;

        bool res = ConvStringtoInt(argv[2], &timeout, 0, APP_MQTT_MAX_WAIT_TIMEOUT);
        if(res ==  false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid Wait timeout: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->cmdTimeout = timeout; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Keep_Alive timeout is: %d ms\r\n", pMqttCtx->cmdTimeout);
}

static void CmdMqttTls(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int forceTls;

        bool res = ConvStringtoInt(argv[2], &forceTls, 0, 1);
        if(res ==  false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid TLS selection: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->forceTls = forceTls; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: force TLS is: %d\r\n", pMqttCtx->forceTls);
}

static void CmdMqttAuth(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int enableAuth;

        bool res = ConvStringtoInt(argv[2], &enableAuth, 0, 1);
        if(res ==  false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid Auth selection: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->enableAuth = enableAuth; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Authentication is: %s\r\n", pMqttCtx->enableAuth ? "enabled" : "disabled");
}

static void CmdMqttLwt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int enableLwt;

        bool res = ConvStringtoInt(argv[2], &enableLwt, 0, 1);
        if(res ==  false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid LWT selection: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->enableLwt = enableLwt; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: LWT is: %s\r\n", pMqttCtx->enableLwt ? "enabled" : "disabled");
}

static void CmdMqttTxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    MqttChangeBufferSize(pCmdIO, argc, argv, pMqttCtx, true);
}

static void CmdMqttRxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    MqttChangeBufferSize(pCmdIO, argc, argv, pMqttCtx, false);
}

static void MqttChangeBufferSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx, bool isTx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        if(pMqttCtx->currState != APP_MQTT_STATE_IDLE)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mqtt: cannot change buffer size while running!\r\n");
        }
        else
        {
            int newBuffSize, oldBuffSize;
            uint8_t* newAllocBuff, *oldAllocBuff;
            uint8_t** ppNewBuff;
            uint16_t* pNewSize;

            bool res = ConvStringtoInt(argv[2], &newBuffSize, 0, 65535);
            if(res ==  false)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid buffer size: %s\r\n", argv[2]);
                return;
            }

            if(isTx)
            {
                oldBuffSize = pMqttCtx->txBuffSize;
                oldAllocBuff = pMqttCtx->txBuff;
                ppNewBuff = &pMqttCtx->txBuff;
                pNewSize = &pMqttCtx->txBuffSize;
            }
            else
            {
                oldBuffSize = pMqttCtx->rxBuffSize;
                oldAllocBuff = pMqttCtx->rxBuff;
                ppNewBuff = &pMqttCtx->rxBuff;
                pNewSize = &pMqttCtx->rxBuffSize;
            }

            if(newBuffSize != oldBuffSize)
            {
                newAllocBuff = (uint8_t*)malloc(newBuffSize);
                if(newAllocBuff != 0)
                {
                    *ppNewBuff = newAllocBuff;
                    *pNewSize = newBuffSize; 
                    free(oldAllocBuff);
                }
                else
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: failed to allocate %d bytes!%d\r\n", newBuffSize);
                }
            }
        }
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: %s buffer size is: %d\r\n", isTx? "TX" : "RX", pMqttCtx->txBuffSize);
}


static void CmdMqttClean(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int cleanSession;

        bool res = ConvStringtoInt(argv[2], &cleanSession, 0, 1);
        if(res ==  false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid Clean selection: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->cleanSession = cleanSession; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Clean session is: %d\r\n", pMqttCtx->cleanSession);
}

#ifdef WOLFMQTT_V5
static void CmdMqttCtrlSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int packetSize;
        bool res = ConvStringtoInt(argv[2], &packetSize, 0, 65535);
        if(res == false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid packet size: %s\r\n", argv[2]);
            return;
        }

        pMqttCtx->maxPktSize = packetSize; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: control packet size is: %d\r\n", pMqttCtx->maxPktSize);
}

static void CmdMqttEAuth(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 3)
    {
        int enhAuth;

        bool res = ConvStringtoInt(argv[2], &enhAuth, 0, 1);
        if(res ==  false)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: invalid Auth selection: %s\r\n", argv[2]);
            return;
        }
        pMqttCtx->enableEAuth = enhAuth; 
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "mqtt: Enhanced Authentication is: %s\r\n", pMqttCtx->enableEAuth ? "enabled" : "disabled");
}

#endif  //  WOLFMQTT_V5

static void CmdMqttHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    int ix;
    const CMD_MQTT_PROC_DCPT* procDcpt = cmd_mqtt_proc_dcpt_tbl;

    for(ix = 0; ix < sizeof(cmd_mqtt_proc_dcpt_tbl) / sizeof(*cmd_mqtt_proc_dcpt_tbl); ix++, procDcpt++)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s: %s\r\n", procDcpt->cmdString, procDcpt->cmdHelp);
    }
}


static void CmdMqttTime(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, APP_MQTT_CONTEXT* pMqttCtx)
{
    uint32_t utc_seconds;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    TCPIP_SNTP_RESULT res = TCPIP_SNTP_TimeGet(&utc_seconds, 0);

    if(res == 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Current Unix Timestamp is: %ld\r\n", utc_seconds);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to get current Unix Timestamp: %d\r\n", res);
    }
}

// other helpers

static bool ConvStringtoInt(char* str, int* pRes, int lim_low, int lim_high)
{
    while(str)
    {
        int len = strlen(str);
        if(len == 0)
        {
            break;
        }
        bool incr = false;
        if(str[len - 1] == '0')
        {
            str[len - 1]++;
            incr = true;
        }
        int conv = atoi(str);
        if(conv == 0)
        {
            break;
        }
        if(incr)
        {
            conv--;
        }

        if(conv <lim_low || conv > lim_high)
        {
            break;
        }

        *pRes = conv;
        return true;
    }

    return false;

}





/*******************************************************************************
 End of File
 */
