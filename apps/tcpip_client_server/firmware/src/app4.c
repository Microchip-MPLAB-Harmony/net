/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app4.c

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

#include "app4.h"
#include <errno.h>
#include <sys/errno.h>

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

static APP4_DATA    app4Data;

// *****************************************************************************
/* UDP Client Data

  Summary:
    Holds UDP client connection data

  Description:
    These variables control the behavior and operation of the UDP client.

  Remarks:
    The variables are initialized at the application start.
    Some important values that act as communication parameters
    can be controlled through individual commnds.
    
*/



static UDP_SOCKET   appSkt = INVALID_SOCKET;    // application socket
static char         appIpv4RemHost[20] = "192.168.100.101"; // IPv4 server address
static UDP_PORT     appRemPort = 32324;         // IPv4 server listening port

static uint8_t      appRxBuffer[1024];          // RX and TX message buffers
static uint8_t      appTxBuffer[1024];

static bool         appDisplayLocal = false;    // display incoming messages locally flag

static SYS_CMD_DEVICE_NODE* appDisplayCmdNode;  // console communicating device
static const void*  appDisplayIoParam;

static bool         appSendAsync = false;       // send messages continuously/whenever you get a chance flag

static int          appUdpTxSize = 1024;        // socket TX buffer size


static uint32_t    appRxTotBytes = 0;           // statistics counters
static uint32_t    appTxTotBytes = 0;

// local prototypes
static int appClientOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appClientMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appClientClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appSetIpv4Server(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appSetSendAsync(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appSetDisplayLocal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appClientStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int appSetTxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void appRecvData(void);

// UDP client implemented commands
static const SYS_CMD_DESCRIPTOR    appClientCmdTbl[]=
{
    {"uopen_c1",        appClientOpen,          ": Open Socket"},
    {"umsg_c1",         appClientMsg,           ": Send Message"},
    {"uclose_c1",       appClientClose,         ": Close -rem Socket"},
    {"usrv4_c1",        appSetIpv4Server,       ": Set IPv4 Server Address and Port"},
    {"uasync_c1",       appSetSendAsync,        ": Set send async option"},
    {"udisplay_c1",     appSetDisplayLocal,     ": Set display local option"},
    {"ustat_c1",        appClientStat,          ": Client Stat"},
    {"utxsize_c1",      appSetTxSize,           ": Set TX size "},
};


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP4_Initialize ( void )

  Remarks:
    See prototype in app4.h.
 */

void APP4_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app4Data.state = APP4_STATE_INIT;

}




/******************************************************************************
  Function:
    void APP4_Tasks ( void )

    This task implements a UDP client.
    The client connects to a specified UDP server and sends messages to it
    upon request or continuously.
    It also receives incoming messages and optionally displays
    them locally. 
    
  Remarks:
    See prototype in app4.h.
 */

void APP4_Tasks ( void )
{
    int netNo, ix;
    TCPIP_NET_HANDLE    hNet;
    
    /* Check the application's current state. */

    switch ( app4Data.state )
    {
        case APP4_STATE_INIT:

            netNo = TCPIP_STACK_NumberOfNetworksGet();
            for(ix = 0; ix < netNo; ix++)
            {
                hNet = TCPIP_STACK_IndexToNet(ix);
                if(!TCPIP_DHCP_IsBound(hNet))
                {
                    return;
                }
            }

            if (SYS_CMD_ADDGRP(appClientCmdTbl, sizeof(appClientCmdTbl)/sizeof(*appClientCmdTbl), "appUdpCli1", ": appUdpCli1 commands"))
            {
                SYS_CONSOLE_MESSAGE("UdpCli1: Created UDP client commands!\r\n");
                app4Data.state = APP4_STATE_RUN;
            }
            else
            {
                SYS_CONSOLE_MESSAGE("UdpCli1: Failed to create UDP client commands!\r\n");
                return;
            }

            break;

        case APP4_STATE_RUN:
            if(appSkt == INVALID_SOCKET)
            {
                return;
            }

            appRecvData();

            if(appSendAsync)
            {
                appClientMsg(0, 0, 0);
            }

            break;


        default:
            break;
    }
}

static void appRecvData(void)
{
    uint16_t nBytes;

    nBytes = TCPIP_UDP_GetIsReady(appSkt);
    if(nBytes)
    {
        if(nBytes > sizeof(appRxBuffer) - 1)
        {
            nBytes = sizeof(appRxBuffer) - 1;
        }

        nBytes = TCPIP_UDP_ArrayGet(appSkt, appRxBuffer, nBytes);
        appRxTotBytes += nBytes;
        if(appDisplayLocal)
        {
            appRxBuffer[nBytes] = 0;
            (*appDisplayCmdNode->pCmdApi->msg)(appDisplayIoParam, (char*)appRxBuffer);
        }
    }
}


static int appClientOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    IP_MULTI_ADDRESS    appRemAddress;
    IP_MULTI_ADDRESS*   pMAdd;
    bool hostString = false;

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
        appSkt = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4, appRemPort, pMAdd);
        if(appSkt == INVALID_SOCKET)
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "UdpCli1: Failed to Open Client Socket\r\n");
        }
        else
        {
            if(appUdpTxSize != 0)
            {
                if(!TCPIP_UDP_OptionsSet(appSkt, UDP_OPTION_TX_BUFF, (void*)appUdpTxSize))
                {
                    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Failed to set TX size to: %d%\r\n", appUdpTxSize);
                }
            }
            (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "UdpCli1: Opened Client Socket %d to %s\r\n", appSkt, hostString ? appIpv4RemHost : "no host");
            // take over the display
            appDisplayIoParam = pCmdIO->cmdIoParam; 
            appDisplayCmdNode = pCmdIO;
        }

    }

    return true;
}


static int appClientMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    static const char* appMsg = "Message from Client UdpCli1: %d\r\n";
    static const char fillMsg[] = "aaaabbbbccccddddeeeeffff";
    static uint32_t appMsgCnt = 0;

    int minBytes, avlblTxSpace, nBytes;
    char sbuff[100];

    if(appSkt != INVALID_SOCKET)
    {
        minBytes = sprintf(sbuff, appMsg, appMsgCnt);

        avlblTxSpace = TCPIP_UDP_PutIsReady(appSkt);
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

                nBytes = TCPIP_UDP_ArrayPut(appSkt, appTxBuffer, sizeof(appTxBuffer));
            }
            else
            {
                nBytes = TCPIP_UDP_ArrayPut(appSkt, (uint8_t*)sbuff, minBytes);
            }
            TCPIP_UDP_Flush(appSkt);
            appTxTotBytes += nBytes;
            appMsgCnt++;
        }
    }

    return true;
}

static int appClientClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appSkt != INVALID_SOCKET)
    {
        TCPIP_UDP_Close(appSkt);
        appSkt = INVALID_SOCKET;
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "Cli: Socket closed\r\n");
    }
    return true;
}

static int appSetIpv4Server(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
    return true;
}

static int appSetSendAsync(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(argc >= 2)
    {
        int setAsync = atoi(argv[1]);
        appSendAsync = setAsync != 0;
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "Set async to: %d\r\n", appSendAsync);
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


static int appClientStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
/*******************************************************************************
 End of File
 */
