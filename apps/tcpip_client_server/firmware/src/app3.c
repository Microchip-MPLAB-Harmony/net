/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app3.c

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
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app3.h"

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
    
    Application strings and buffers are defined outside this structure.
*/

static APP3_DATA    app3Data;

// *****************************************************************************
/* UDP Server Data

  Summary:
    Holds UDP server connection data

  Description:
    These variables control the behavior and operation of the UDP server.

  Remarks:
    The variables are initialized at the application start.
    Some important values that act as communication parameters
    can be controlled through individual commnds.
    
*/


static UDP_SOCKET   appSkt = INVALID_SOCKET;    // application socket
static UDP_PORT     appListenPort = 32323;      // server listening port

static uint8_t      appBuffer[2048 + 1];        // messages buffer
static uint8_t*     appStartBuff;               // current buffer pointers
static uint8_t*     appEndBuff;
static uint8_t*     appCurrWrPtr;
static uint8_t*     appCurrRdPtr;

static uint32_t     appRxTotBytes = 0;          // statistics counters
static uint32_t     appTxTotBytes = 0;

static bool         appSetStrictPort = false;   // strict port accept connections flag
static bool         appSetStrictNet = false;    // strict network accept connections flag
static bool         appSetStrictAddress = false;// strict address accept connections flag

static int          appUdpTxSize = 1024;        // socket TX buffer size

static bool         appDisplayLocal = false;    // display locally the received messages

static SYS_CMD_DEVICE_NODE* appDisplayCmdNode;  // console communicating device 
static const void*  appDisplayIoParam;


// local prototypes
static int appServerOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appServerClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appServerStrictNet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appServerStrictPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appServerStrictAddress(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appSetDisplayLocal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appServerStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appSetTxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);


static void    appRecvMessage(void);

// UDP server implemented commands
static const SYS_CMD_DESCRIPTOR    appServerCmdTbl[]=
{
    {"uopen_s1",       appServerOpen,              ": Open Socket"},
    {"uclose_s1",      appServerClose,             ": Close Socket"},
    {"ustnet_s1",      appServerStrictNet,         ": Set Strict Net"},
    {"ustport_s1",     appServerStrictPort,        ": Set Strict Port"},
    {"ustadd_s1",      appServerStrictAddress,     ": Set Strict Address"},
    {"udisplay_s1",    appSetDisplayLocal,         ": Set display local option"},
    {"ustat_s1",       appServerStat,              ": Server Stat"},
	{"utxsize_s1",     appSetTxSize,               ": Set TX size "},

};


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP3_Initialize ( void )

  Remarks:
    See prototype in app3.h.
 */

void APP3_Initialize ( void )
{
    app3Data.state = APP3_STATE_INIT;
}


/******************************************************************************
  Function:
    void APP3_Tasks ( void )

    This task implements a UDP relay server.
    The server is listening for incoming connections and 
    relays the data back to the sender.
    Optionally, the incoming messages are displayed locally.
    
  Remarks:
    See prototype in app3.h.
 */

void APP3_Tasks ( void )
{
    int netNo, ix;
    TCPIP_NET_HANDLE    hNet;


    switch(app3Data.state)
    {
        case APP3_STATE_INIT:

            netNo = TCPIP_STACK_NumberOfNetworksGet();
            for(ix = 0; ix < netNo; ix++)
            {
                hNet = TCPIP_STACK_IndexToNet(ix);
                if(!TCPIP_STACK_NetIsLinked(hNet))
                {
                    return;
                }

                if(TCPIP_DHCP_IsEnabled(hNet) && !TCPIP_DHCP_IsBound(hNet))
                {
                    return;
                }
            }

            if (SYS_CMD_ADDGRP(appServerCmdTbl, sizeof(appServerCmdTbl)/sizeof(*appServerCmdTbl), "appUdpSrv1", ": appUdpSrv1 commands"))
            {
                SYS_CONSOLE_MESSAGE("UdpSrv1: Created UDP Server commands!\r\n");

                appStartBuff = appBuffer;
                appEndBuff = appBuffer + sizeof(appBuffer) - 1;
                appCurrWrPtr = appStartBuff;
                appCurrRdPtr = appStartBuff;

                appRxTotBytes = appTxTotBytes = 0;

                app3Data.state = APP3_STATE_RUN;
            }
            else
            {
                SYS_CONSOLE_MESSAGE("UdpSrv1: Failed to create UDP Server commands!\r\n");
                return;
            }

            break;

        case APP3_STATE_RUN:

            if(appSkt != INVALID_SOCKET)
            {
                appRecvMessage();
            }
            break;

        default:
            break;
    }

}

static void appRecvMessage(void)
{

    int rxSpace = appEndBuff - appCurrWrPtr;
    int rxAvlblBytes = TCPIP_UDP_GetIsReady(appSkt);
    if(rxSpace && rxAvlblBytes)
    {
        if(rxAvlblBytes > rxSpace)
        {
            rxSpace = rxAvlblBytes;
        }

        int rxBytes = TCPIP_UDP_ArrayGet(appSkt, appCurrWrPtr, rxSpace);
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
        if(TCPIP_UDP_PutIsReady(appSkt))
        {
            int txBytes = TCPIP_UDP_ArrayPut(appSkt, appCurrRdPtr, pendingTxBytes);
            TCPIP_UDP_Flush(appSkt);
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

static int appServerOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt == INVALID_SOCKET)
    {
        appSkt = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, appListenPort, 0);

        if(appSkt == INVALID_SOCKET)
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Server: Failed to Open Socket\r\n");
        }
        else
        {
            if(appSetStrictPort)
            {
                TCPIP_UDP_OptionsSet(appSkt, UDP_OPTION_STRICT_PORT, (void*)true);
            }
            if(appSetStrictNet)
            {
                TCPIP_UDP_OptionsSet(appSkt, UDP_OPTION_STRICT_NET, (void*)true);
            }
            if(appSetStrictAddress)
            {
                TCPIP_UDP_OptionsSet(appSkt, UDP_OPTION_STRICT_ADDRESS, (void*)true);
            }

            if(appUdpTxSize != 0)
            {
                if(!TCPIP_UDP_OptionsSet(appSkt, UDP_OPTION_TX_BUFF, (void*)appUdpTxSize))
                {
                    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Failed to set TX size to: %d%\r\n", appUdpTxSize);
                }
            }
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Server: Opened Socket %d, listening on port: %d\r\n", appSkt, appListenPort);
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Server: Opened Socket - SPort: %1d, SNet: %1d, SAdd: %1d\r\n", appSetStrictPort, appSetStrictNet, appSetStrictAddress);
            // take over the display
            appDisplayIoParam = pCmdIO->cmdIoParam; 
            appDisplayCmdNode = pCmdIO;
        }
    }
    
    return true;
}

static int appServerClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt != INVALID_SOCKET)
    {
        TCPIP_UDP_Close(appSkt);
        appSkt = INVALID_SOCKET;
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Cli: Socket closed\r\n");
    }
    return true;
}

static int appServerStrictNet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int strictNet;

    if(argc >= 2)
    {
        strictNet = atoi(argv[1]);
        appSetStrictNet = (strictNet != 0); 
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set Strict Net to: %1d\r\n", appSetStrictNet);
    }    

    return true;

}

static int appServerStrictPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int strictPort;

    if(argc >= 2)
    {
        strictPort = atoi(argv[1]);
        appSetStrictPort = (strictPort != 0); 
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set Strict Port to: %1d\r\n", appSetStrictPort);
    }    

    return true;

}

static int appServerStrictAddress(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int strictAddress;

    if(argc >= 2)
    {
        strictAddress = atoi(argv[1]);
        appSetStrictAddress = (strictAddress != 0); 
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set Strict Address to: %1d\r\n", appSetStrictAddress);
    }    

    return true;

}

static int appSetDisplayLocal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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

    return true;
}

static int appServerStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int pendingTxBytes = appCurrWrPtr - appCurrRdPtr;
    int pendingRxBytes = TCPIP_UDP_GetIsReady(appSkt);
    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Server Stat - RX: %d, pend: %d, TX: %d, pend: %d\r\n", appRxTotBytes, pendingRxBytes, appTxTotBytes, pendingTxBytes);

    if(argc == 2)
    {
        if(strcmp(argv[1], "clr") == 0)
        {
            appRxTotBytes = appTxTotBytes = 0;
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Stat cleared\r\n");
        }
    }
    return true;
}

static int appSetTxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint32_t txSize;

    if(argc == 2)
    {
        txSize = atoi(argv[1]);
        if(txSize > 0 && txSize < 65536)
        {
            appUdpTxSize = txSize;
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set TX size to: %d%\r\n", txSize);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Invalid TX size\r\n");
        }
    }    

    return true;
}


