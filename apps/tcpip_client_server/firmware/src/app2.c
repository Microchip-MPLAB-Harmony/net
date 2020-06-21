/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app2.c

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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app2.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

static APP2_DATA    app2Data;


// *****************************************************************************
/* TCP Client Data

  Summary:
    Holds TCP client connection data

  Description:
    These variables control the behavior and operation of the TCP client.

  Remarks:
    The variables are initialized at the application start.
    Some important values that act as communication parameters
    can be controlled through individual commnds.
    
*/

static TCP_SOCKET   appSkt = INVALID_SOCKET;    // application socket
static char         appIpv4RemHost[20] = "192.168.100.101"; // IPv4 server address
static TCP_PORT     appRemPort = 9761;         // IPv4 server listening port

static uint8_t      appRxBuffer[1024];          // RX and TX message buffers
static uint8_t      appTxBuffer[1024];

static bool         appDisplayLocal = false;    // display incoming messages locally flag

static SYS_CMD_DEVICE_NODE* appDisplayCmdNode;  // console communicating device
static const void*  appDisplayIoParam;

static bool         appSendAsync = false;       // send messages continuously/whenever you get a chance flag


static bool         appConnectReported = false; // run time connection flags
static bool         appResetReported = false;
static bool         appReportDisconnect = false;// client has been disconnected report flag


static uint32_t     appRxTotBytes = 0;          // statistics counters
static uint32_t     appTxTotBytes = 0;

// local prototypes
static void appClientOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appClientMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appClientAbort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appClientClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appSetIpv4Server(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appSetSendAsync(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appSetDisplayLocal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appClientStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void appRecvMessage(void);

// TCP client implemented commands
static const SYS_CMD_DESCRIPTOR    appClientCmdTbl[]=
{
    {"topen_c1",        appClientOpen,          ": Open Socket"},
    {"tmsg_c1",         appClientMsg,           ": Send Message"},
    {"tabort_c1",       appClientAbort,         ": Abort socket"},
    {"tclose_c1",       appClientClose,         ": Close Socket"},
    {"tsrv4_c1",        appSetIpv4Server,       ": Set IPv4 Server Address and Port"},
    {"tasync_c1",       appSetSendAsync,        ": Set send async option"},
    {"tdisplay_c1",     appSetDisplayLocal,     ": Set display local option"},
    {"tstat_c1",        appClientStat,          ": Client Stat"},
};


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP2_Initialize ( void )

  Remarks:
    See prototype in app2.h.
 */

void APP2_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app2Data.state = APP2_STATE_INIT;

}



/******************************************************************************
  Function:
    void APP2_Tasks ( void )

    This task implements a TCP client.
    The client connects to a specified TCP server and sends messages to it
    upon request or continuously.
    It also receives incoming messages and optionally displays
    them locally. 
    
  Remarks:
    See prototype in app2.h.
 */

void APP2_Tasks ( void )
{
    int netNo, ix;
    TCPIP_NET_HANDLE    hNet;
    
    /* Check the application's current state. */

    switch ( app2Data.state )
    {
        case APP2_STATE_INIT:

            netNo = TCPIP_STACK_NumberOfNetworksGet();
            for(ix = 0; ix < netNo; ix++)
            {
                hNet = TCPIP_STACK_IndexToNet(ix);
                if(!TCPIP_DHCP_IsBound(hNet))
                {
                    return;
                }
            }

            if (SYS_CMD_ADDGRP(appClientCmdTbl, sizeof(appClientCmdTbl)/sizeof(*appClientCmdTbl), "appTcpCli1", ": appTcpCli1 commands"))
            {
                SYS_CONSOLE_MESSAGE("TcpCli1: Created TCP client commands!\r\n");
                app2Data.state = APP2_STATE_RUN;
            }
            else
            {
                SYS_CONSOLE_MESSAGE("TcpCli1: Failed to create TCP client commands!\r\n");
                return;
            }

            break;

        case APP2_STATE_RUN:
            if(appSkt == INVALID_SOCKET)
            {
                return;
            }


            if(TCPIP_TCP_WasReset(appSkt))
            {
                if(!appResetReported)
                {
                    if(appDisplayCmdNode)
                    {
                        (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, "TcpCli1: Socket Reset Reported!\r\n");
                    }
                    appResetReported = true;
                }
                appSkt = INVALID_SOCKET;
                return;
            }

            if(!TCPIP_TCP_IsConnected(appSkt))
            {   // wait some more
                return;
            }
            else if(!appConnectReported)
            {
                if(appDisplayCmdNode)
                {
                    (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, "TcpCli1: TCP Socket Connected!\r\n");
                }
                appConnectReported = true;
            }

            if(TCPIP_TCP_WasDisconnected(appSkt))
            {
                if(appReportDisconnect == false)
                {
                    appReportDisconnect = true;
                    if(appDisplayCmdNode)
                    {
                        (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, "TcpCli1: TCP Socket Reported Disconnected...\r\n");
                    }
                }
            }

            appRecvMessage();

            if(appSendAsync)
            {
                appClientMsg(0, 0, 0);
            }

            break;


        default:
            break;
    }

}


static void appRecvMessage(void)
{
    uint16_t nBytes;

    nBytes = TCPIP_TCP_GetIsReady(appSkt);
    if(nBytes)
    {
        if(nBytes > sizeof(appRxBuffer) - 1)
        {
            nBytes = sizeof(appRxBuffer) - 1;
        }

        nBytes = TCPIP_TCP_ArrayGet(appSkt, appRxBuffer, nBytes);
        appRxTotBytes += nBytes;
        if(appDisplayLocal)
        {
            appRxBuffer[nBytes] = 0;
            (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, (char*)appRxBuffer);
        }
    }
}

static void appClientOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    IP_MULTI_ADDRESS*   pMAdd;
    IP_MULTI_ADDRESS    appRemAddress;
    bool                hostString = false;

    if(appSkt == INVALID_SOCKET)
    {
        pMAdd = 0;
        if(strlen(appIpv4RemHost) != 0)
        {
            if(TCPIP_Helper_StringToIPAddress(appIpv4RemHost, &appRemAddress.v4Add))
            {
                pMAdd = &appRemAddress;
                hostString = true;
            }
        }
        appSkt = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, appRemPort, pMAdd);
        if(appSkt == INVALID_SOCKET)
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TcpCli1: Failed to Open Client Socket\r\n");
        }
        else
        {
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "TcpCli1: Opened Client Socket %d to %s\r\n", appSkt, hostString ? appIpv4RemHost : "no host");
            if(TCPIP_TCP_WasReset(appSkt) == false)
            {
                (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TcpCli1: Socket Failed to report ini Reset...\r\n");
            }
            appConnectReported = false;
            appResetReported = false;
            appReportDisconnect = false;
            // take over the display
            appDisplayIoParam = pCmdIO->cmdIoParam; 
            appDisplayCmdNode = pCmdIO;
        }

    }

}


static void appClientMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    static const char* appMsg = "Message from Client TcpCli1: %d\r\n";
    static const char fillMsg[] = "aaaabbbbccccddddeeeeffff";
    static uint32_t appMsgCnt = 0;

    int minBytes, avlblTxSpace, nBytes;
    char sbuff[100];

    if(appSkt != INVALID_SOCKET)
    {
        minBytes = sprintf(sbuff, appMsg, appMsgCnt);

        avlblTxSpace = TCPIP_TCP_PutIsReady(appSkt);
        if(avlblTxSpace >= minBytes)
        {
            if(sizeof(appTxBuffer) > minBytes)
            {   // fill the rest of the appTxBuffer
                memcpy(appTxBuffer, sbuff, minBytes);
                uint8_t* pBuff = appTxBuffer + minBytes;
                int fillSize = sizeof(appTxBuffer) - minBytes;
                while(fillSize)
                {
                    int fillChunk = sizeof(fillMsg);
                    if(fillChunk > fillSize)
                    {
                        fillChunk = fillSize;
                    }
                    memcpy(pBuff, fillMsg, fillChunk);
                    pBuff += fillChunk;
                    fillSize -= fillChunk;
                }

                nBytes = TCPIP_TCP_ArrayPut(appSkt, appTxBuffer, sizeof(appTxBuffer));
            }
            else
            {
                nBytes = TCPIP_TCP_ArrayPut(appSkt, (uint8_t*)sbuff, minBytes);
            }
            appTxTotBytes += nBytes;
            appMsgCnt++;
        }
    }

}

static void appClientAbort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt != INVALID_SOCKET)
    {
        if(argc == 2 && strcmp(argv[1], "rem") == 0)
        {
            TCPIP_TCP_Put(appSkt, 0x18); 
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Cli: Send rem abort\r\n");
        }
        else
        {
            TCPIP_TCP_Abort(appSkt, false);
            appResetReported = false;
            appSkt = INVALID_SOCKET;
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Cli: Socket aborted\r\n");
        }
    }

}

static void appClientClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt != INVALID_SOCKET)
    {
        if(argc == 2 && strcmp(argv[1], "rem") == 0)
        {
            TCPIP_TCP_Put(appSkt, 0x1b); 
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Cli: Send rem close\r\n");
        }
        else
        {
            TCPIP_TCP_Close(appSkt);
            appResetReported = false;
            appSkt = INVALID_SOCKET;
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Cli: Socket closed\r\n");
        }
    }
}

static void appSetIpv4Server(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(argc > 1)
    {
        IPV4_ADDR ipAddr;

        if(TCPIP_Helper_StringToIPAddress(argv[1], &ipAddr))
        {
            strcpy(appIpv4RemHost, argv[1]);
        }
    }
    if(argc > 2)
    {
        appRemPort = atoi(argv[2]);
    }

    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Server address: %s, Port: %d\r\n", appIpv4RemHost, appRemPort);
}

static void appSetSendAsync(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(argc >= 2)
    {
        int setAsync = atoi(argv[1]);
        appSendAsync = setAsync != 0;
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set async to: %d\r\n", appSendAsync);
    }

}
 
static void appSetDisplayLocal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(argc >= 2)
    {
        int setDisplay = atoi(argv[1]);
        if(setDisplay != 0)
        {   // take over the display
            appDisplayIoParam = pCmdIO->cmdIoParam; 
            appDisplayCmdNode = pCmdIO;
            appDisplayLocal = 1;
        }
        else
        {
            appDisplayLocal = 0;
        }
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set display to: %1d\r\n", appDisplayLocal);
    }

}


static void appClientStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Client Stat - RX: %d, TX: %d\r\n", appRxTotBytes, appTxTotBytes);

    if(argc == 2)
    {
        if(strcmp(argv[1], "clr") == 0)
        {
            appRxTotBytes = appTxTotBytes = 0;
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Stat cleared\r\n");
        }
    }
}

/*******************************************************************************
 End of File
 */
