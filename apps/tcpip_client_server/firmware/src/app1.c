/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app1.c

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

#include "app1.h"

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

static APP1_DATA    app1Data;


// *****************************************************************************
/* TCP Server Data

  Summary:
    Holds TCP server connection data

  Description:
    These variables control the behavior and operation of the TCP server.

  Remarks:
    The variables are initialized at the application start.
    Some important values that act as communication parameters
    can be controlled through individual commnds.
    
*/


static TCP_SOCKET   appSkt = INVALID_SOCKET;    // application socket
static TCP_PORT     appListenPort = 9760;       // server listening port

static uint8_t      appBuffer[1024];            // messages buffer
static uint8_t*     appStartBuff;               // current buffer pointers
static uint8_t*     appEndBuff;
static uint8_t*     appCurrWrPtr;
static uint8_t*     appCurrRdPtr;

static uint32_t     appRxTotBytes = 0;          // statistics counters
static uint32_t     appTxTotBytes = 0;

static bool         appIsConnected = false;     // client connected flag
static bool         appReportDisconnect = false;// client has been disconnected report flag

static int          appTcpTxSize = 2048;        // socket TX and RX buffer size
static int          appTcpRxSize = 2048;

static bool         appDisplayLocal = false;    // display incoming messages locally flag

static SYS_CMD_DEVICE_NODE* appDisplayCmdNode;  // console communicating device 
static const void*  appDisplayIoParam;

// local prototypes
static void appServerOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appServerMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appServerAbort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appServerClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appSetTcpTxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appSetTcpRxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appSetDisplayLocal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void appServerStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void appRecvMessage(void);

// TCP server implemented commands
static const SYS_CMD_DESCRIPTOR    appSrvCmdTbl[]=
{
    {"topen_s1",        appServerOpen,          ": Open Socket"},
    {"tmsg_s1",         appServerMsg,           ": Send Message"},
    {"tabort_s1",       appServerAbort,         ": Abort socket"},
    {"tclose_s1",       appServerClose,         ": Close Socket"},
    {"ttxsize_s1",      appSetTcpTxSize,        ": Set TX size "},
    {"trxsize_s1",      appSetTcpRxSize,        ": Set RX size "},
    {"tdisplay_s1",     appSetDisplayLocal,     ": Set display local option"},
    {"tstat_s1",        appServerStat,          ": Server Stat"},
};




// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP1_Initialize ( void )

  Remarks:
    See prototype in app1.h.
 */

void APP1_Initialize ( void )
{
    app1Data.state = APP1_STATE_INIT;
}


/******************************************************************************
  Function:
    void APP1_Tasks ( void )

    This task implements a TCP relay server.
    The server is listening for incoming connections and 
    relays the data back to the sender.
    Optionally, the incoming messages are displayed locally.
    
  Remarks:
    See prototype in app1.h.
 */

void APP1_Tasks ( void )
{
    int netNo, ix;
    TCPIP_NET_HANDLE    hNet;


    switch(app1Data.state)
    {
        case APP1_STATE_INIT:

            netNo = TCPIP_STACK_NumberOfNetworksGet();
            for(ix = 0; ix < netNo; ix++)
            {
                hNet = TCPIP_STACK_IndexToNet(ix);
                if(!TCPIP_DHCP_IsBound(hNet))
                {
                    return;
                }
            }

            if (SYS_CMD_ADDGRP(appSrvCmdTbl, sizeof(appSrvCmdTbl)/sizeof(*appSrvCmdTbl), "appTcpSrv1", ": appTcpSrv1 commands"))
            {
                SYS_CONSOLE_MESSAGE("TcpSrv1: Created TCP server commands!\r\n");

                appStartBuff = appBuffer;
                appEndBuff = appBuffer + sizeof(appBuffer);
                appCurrWrPtr = appStartBuff;
                appCurrRdPtr = appStartBuff;

                appRxTotBytes = appTxTotBytes = 0;

                app1Data.state = APP1_STATE_RUN;
            }
            else
            {
                SYS_CONSOLE_MESSAGE("TcpSrv1: Failed to create TCP server commands!\r\n");
                return;
            }

            break;

        case APP1_STATE_RUN:

            if(appSkt == INVALID_SOCKET)
            {
                return;
            }

            if(!TCPIP_TCP_IsConnected(appSkt))
            {   // no client connected
                appIsConnected = false;
                return;
            }

            // client connected
            if(appIsConnected == 0)
            {   // first time
                if(appDisplayCmdNode)
                {
                    (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, "Srv: Socket Reported Connect...\r\n");
                }
                appIsConnected = true;

                appRxTotBytes = appTxTotBytes = 0;
            }

            if(TCPIP_TCP_WasReset(appSkt))
            {
                if(appDisplayCmdNode)
                {
                    (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, "Srv: Socket Reported Reset...\r\n");
                }
            }

            if(TCPIP_TCP_WasDisconnected(appSkt))
            {
                if(appReportDisconnect == false)
                {
                    appReportDisconnect = true;
                    if(appDisplayCmdNode)
                    {
                        (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, "Srv: Socket Reported Disconnected...\r\n");
                    }
                }
            }

            appRecvMessage();


            break;


        default:
            break;
    }
}

static void appRecvMessage(void)
{

    int rxSpace = appEndBuff - appCurrWrPtr;
    int rxBytes = TCPIP_TCP_GetIsReady(appSkt);
    if(rxSpace && rxBytes)
    {
        rxBytes = TCPIP_TCP_ArrayGet(appSkt, appCurrWrPtr, rxSpace);
        if(appDisplayLocal)
        {
            *(appCurrWrPtr + rxBytes) = '\0';
            (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, (char*)appCurrWrPtr);
        }
        appCurrWrPtr += rxBytes;
        appRxTotBytes += rxBytes;
    }

    int pendingTxBytes = appCurrWrPtr - appCurrRdPtr;
    if(pendingTxBytes)
    {
        if(TCPIP_TCP_PutIsReady(appSkt))
        {
            int txBytes = TCPIP_TCP_ArrayPut(appSkt, appCurrRdPtr, pendingTxBytes);
            appCurrRdPtr += txBytes;
            appTxTotBytes += txBytes;
        }
    }

    pendingTxBytes = appCurrWrPtr - appCurrRdPtr;
    if(pendingTxBytes == 0)
    {
        appCurrWrPtr = appStartBuff;
        appCurrRdPtr = appStartBuff;
    }
}


static void appServerOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt == INVALID_SOCKET)
    {
        appSkt = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, appListenPort, 0);
        if(appSkt == INVALID_SOCKET)
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Srv: Failed to Open Socket\r\n");
        }
        else
        {
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Srv: Opened Socket %d, listening on port: %d\r\n", appSkt, appListenPort);
            if(TCPIP_TCP_WasReset(appSkt) == false)
            {
                (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Srv: Socket Failed to report ini Reset...\r\n");
            }

            if(appTcpTxSize != 0)
            {
                if(!TCPIP_TCP_OptionsSet(appSkt, TCP_OPTION_TX_BUFF, (void*)appTcpTxSize))
                {
                    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Failed to set TX size to: %d%\r\n", appTcpTxSize);
                }
            }

            if(appTcpRxSize != 0)
            {
                if(!TCPIP_TCP_OptionsSet(appSkt, TCP_OPTION_RX_BUFF, (void*)appTcpRxSize))
                {
                    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Failed to set RX size to: %d%\r\n", appTcpRxSize);
                }
            }
            appIsConnected = false;
            appReportDisconnect = false;
            // take over the display
            appDisplayIoParam = pCmdIO->cmdIoParam; 
            appDisplayCmdNode = pCmdIO;
        }

    }

}

static void appServerMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    static const char* appMsg = "Message from Server: %d\r\n";
    static uint32_t appMsgCnt = 0;

    int nBytes;
    char sbuff[100];

    if(appSkt != INVALID_SOCKET && appIsConnected)
    {

        nBytes = sprintf(sbuff, appMsg, appMsgCnt++);

        if(TCPIP_TCP_PutIsReady(appSkt) >= nBytes)
        {
            TCPIP_TCP_ArrayPut(appSkt, (uint8_t*)sbuff, nBytes);
            TCPIP_TCP_Flush(appSkt);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Srv: Failed to send TCP data...\r\n");
        }
    }

}

static void appServerAbort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt != INVALID_SOCKET)
    {
        TCPIP_TCP_Abort(appSkt, false);
        appIsConnected = false;
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Srv: Socket aborted\r\n");
    }
}

static void appServerClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt != INVALID_SOCKET)
    {
        TCPIP_TCP_Close(appSkt);
        appSkt = INVALID_SOCKET;
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Srv: Socket closed\r\n");
    }
}

static void appSetTcpTxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint32_t txSize;

    if(argc == 2)
    {
        txSize = atoi(argv[1]);
        if(txSize > 0 && txSize < 65536)
        {
            appTcpTxSize = txSize;
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set TX size to: %d%\r\n", txSize);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Invalid TX size\r\n");
        }
    }    

}

static void appSetTcpRxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint32_t rxSize;

    if(argc == 2)
    {
        rxSize = atoi(argv[1]);
        if(rxSize > 0 && rxSize < 65536)
        {
            appTcpRxSize = rxSize;
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set RX size to: %d%\r\n", rxSize);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Invalid RX size\r\n");
        }
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


static void appServerStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int pendingTxBytes = appCurrWrPtr - appCurrRdPtr;
    int pendingRxBytes = TCPIP_TCP_GetIsReady(appSkt);
    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Server Stat - RX: %d, pend: %d, TX: %d, pend: %d\r\n", appRxTotBytes, pendingRxBytes, appTxTotBytes, pendingTxBytes);

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
