/*******************************************************************************
  Sample Application

  File Name:
    app_commands.c

  Summary:
    commands for the tcp client demo app.

  Description:
    
 *******************************************************************************/


// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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
// DOM-IGNORE-END


#include "app_commands.h"
#include "app.h"
#include "tcpip/tcpip.h"
#if defined(TCPIP_STACK_COMMAND_ENABLE)



static void _APP_Commands_SendUDPPacket(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_SetOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_GetOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_SetTimeout(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static const SYS_CMD_DESCRIPTOR    appCmdTbl[]=
{
    {"sendudp",     _APP_Commands_SendUDPPacket,        ": Sends the UDP Packet"},
    {"getopt",      _APP_Commands_GetOptions,           ": Gets the hostname, port and message"},
    {"setopt",      _APP_Commands_SetOptions,           ": Sets the current hostname, port, and message"},
    {"settmo",      _APP_Commands_SetTimeout,           ": Sets the current receive timeout"},
};

bool APP_Commands_Init()
{
    if (!SYS_CMD_ADDGRP(appCmdTbl, sizeof(appCmdTbl)/sizeof(*appCmdTbl), "app", ": app commands"))
    {
        SYS_ERROR(SYS_ERROR_ERROR, "Failed to create TCPIP Commands\r\n");
        return false;
    }
    strcpy(APP_Hostname_Buffer, "10.0.1.4");
    strcpy(APP_Port_Buffer, "9760");
    strcpy(APP_Message_Buffer, "Hello");
    APP_Send_Packet = false;
    return true;
}

char APP_Hostname_Buffer[MAX_URL_SIZE];
char APP_Port_Buffer[6];
char APP_Message_Buffer[MAX_URL_SIZE];
bool APP_Send_Packet = false;

void _APP_Commands_SendUDPPacket(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;


    if (argc != 1)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: sendudp\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: sendudp\r\n");
        return;
    }
    APP_Send_Packet = true;
}

void _APP_Commands_SetOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;


    if (argc != 4)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setopt <hostname> <port> <message>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setopt 10.0.1.4 9760 Hello\r\n");
        return;
    }

    strcpy(APP_Hostname_Buffer, argv[1]);
    strcpy(APP_Port_Buffer, argv[2]);
    strcpy(APP_Message_Buffer, argv[3]);
}

char bufferArea[3][80];

void _APP_Commands_GetOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    
    if (argc != 1)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: getopt\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: getopt\r\n");
        return;
    }

     (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Current UDP Options:\r\n");
     sprintf(bufferArea[0], "\thostname: '%s'\r\n", APP_Hostname_Buffer);
     (*pCmdIO->pCmdApi->msg)(cmdIoParam, bufferArea[0]);
     sprintf(bufferArea[1], "\tport: '%s'\r\n", APP_Port_Buffer);
     (*pCmdIO->pCmdApi->msg)(cmdIoParam, bufferArea[1]);
     sprintf(bufferArea[2], "\tmessage: '%s'\r\n", APP_Message_Buffer);
     (*pCmdIO->pCmdApi->msg)(cmdIoParam, bufferArea[2]);

}

extern APP_DATA appData;
void _APP_Commands_SetTimeout(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;


    if (argc != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: settmo <ms>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: settmo 2000\r\n");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Current tmo is: %d\r\n", appData.tmoMs);
        return;
    }

    int tmoMs = atoi(argv[1]);
    if (tmoMs == 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s is not a valid tmo. Retry.\r\n", argv[1]);
    }
    else
    {
        appData.tmoMs = tmoMs; 
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Set the timeout to: %d\r\n", tmoMs);
    }


}

#endif
