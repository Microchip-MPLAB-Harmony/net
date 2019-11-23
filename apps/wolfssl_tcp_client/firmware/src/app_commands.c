/*******************************************************************************
  Sample Application

  File Name:
    app_commands.c

  Summary:
    commands for the tcp client demo app.

  Description:
    
 *******************************************************************************/


// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#include "tcpip/tcpip.h"
#include "app_commands.h"
#include "app.h"
#include "config.h"
#include <wolfssl/ssl.h>

#if defined(TCPIP_STACK_COMMAND_ENABLE)
char wolfSSLLog[1024] = {0};
int wolfSSLLogSize = 0;

extern APP_DATA appData;

static void _APP_Commands_OpenURL(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_IPMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_Stats(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_GetUnixTime(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_WolfSSLLog(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);


static const SYS_CMD_DESCRIPTOR    appCmdTbl[]=
{
    {"openurl", _APP_Commands_OpenURL, ": Connect to a url and do a GET"},
    {"ipmode", _APP_Commands_IPMode, ": Change IP Mode"},
    {"stats", _APP_Commands_Stats, ": Statistics"},
    {"unixtime", _APP_Commands_GetUnixTime, ": Unix Time"},
    {"wolfsslLog", _APP_Commands_WolfSSLLog, ": wolfSSL Log"},
};

bool APP_Commands_Init()
{
    if (!SYS_CMD_ADDGRP(appCmdTbl, sizeof(appCmdTbl)/sizeof(*appCmdTbl), "app", ": app commands"))
    {
        SYS_ERROR(SYS_ERROR_ERROR, "Failed to create TCPIP Commands\r\n", 0);
        return false;
    }

    return true;
}

void _APP_Commands_OpenURL(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    wolfSSLLog[0] = 0;
    wolfSSLLogSize = 0;

    if (argc != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: openurl <url>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: openurl http://www.google.com/\r\n");
    }
    if (appData.state != APP_TCPIP_WAITING_FOR_COMMAND)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Demo is in the wrong state to take this command");
    }
    appData.state = APP_TCPIP_PROCESS_COMMAND;
    strncpy(appData.urlBuffer, argv[1], sizeof(appData.urlBuffer));
}

extern APP_DATA appData;

void _APP_Commands_IPMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if (argc != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ipmode <ANY|4|6>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ipmode 6\r\n");
        return;

    }
    appData.ipMode = atoi(argv[1]);
    
}

void _APP_Commands_Stats(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Raw Bytes Txed: %d\r\n", appData.rawBytesSent);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Raw Bytes Rxed: %d\r\n", appData.rawBytesReceived);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Clear Bytes Txed: %d\r\n", appData.clearBytesSent);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Clear Bytes Rxed: %d\r\n", appData.clearBytesReceived);

    uint32_t freq = SYS_TMR_SystemCountFrequencyGet();
    uint32_t time = ((appData.dnsComplete - appData.testStart) * 1000ull) / freq;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS Lookup Time: %d ms\r\n", time);

    time = ((appData.connectionOpened - appData.dnsComplete) * 1000ull) / freq;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Time to Start TCP Connection: %d ms\r\n", time);

    if (appData.urlBuffer[4] == 's')
    {
        time = ((appData.sslNegComplete - appData.connectionOpened) * 1000ull) / freq;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Time to Negotiate SSL Connection: %d ms\r\n", time);

        time = ((appData.firstRxDataPacket - appData.sslNegComplete) * 1000ull) / freq;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Time to till first packet from server: %d ms\r\n", time);
    }
    else
    {
        time = ((appData.firstRxDataPacket - appData.connectionOpened) * 1000ull) / freq;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Time for first packet from server: %d ms\r\n", time);
    }

    time = ((appData.lastRxDataPacket - appData.firstRxDataPacket) * 1000ull) / freq;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Time for last packet from server: %d ms\r\n", time);
    
}

void _APP_Commands_GetUnixTime(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t sec = TCPIP_SNTP_UTCSecondsGet();
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Time from SNTP: %d\r\n", sec);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Low Rez Timer: %d\r\n", SYS_TIME_CounterGet() /
                             SYS_TIME_FrequencyGet());
    
}

static void _APP_Commands_WolfSSLLog(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    SYS_CONSOLE_MESSAGE(wolfSSLLog);
    
}


#endif
