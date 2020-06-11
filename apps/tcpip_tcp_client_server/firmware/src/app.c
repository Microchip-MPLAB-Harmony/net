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

#include "app.h"

#include "tcpip/tcpip.h"

#include "app_commands.h"
#include "tcpip/tcpip_common_ports.h"

#define SERVER_PORT 9760

int32_t _APP_ParseUrl(char *uri, char **host, char **path, uint16_t * port);
int8_t _APP_PumpDNS(const char * hostname, IPV4_ADDR *ipv4Addr);

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

APP_DATA appData;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_TCPIP_WAIT_INIT;
    
    appData.clientState = APP_TCPIP_WAIT_INIT;
    appData.serverState = APP_TCPIP_WAIT_INIT;
    appData.serverSocket = INVALID_SOCKET;
    appData.clientSocket = INVALID_SOCKET;
#if defined(TCPIP_STACK_COMMAND_ENABLE)
    APP_Commands_Init();
#endif

}
void _APP_ClientTasks()
{
    switch(appData.clientState)
    {
        case APP_TCPIP_WAITING_FOR_COMMAND:
        {
#if defined(TCPIP_STACK_COMMAND_ENABLE)
            if (APP_URL_Buffer[0] != '\0')
            {
                TCPIP_DNS_RESULT result;
                appData.port = TCPIP_HTTP_SERVER_PORT;
                if (_APP_ParseUrl(APP_URL_Buffer, &appData.host, &appData.path, &appData.port))
                {
                    SYS_CONSOLE_PRINT("Could not parse URL '%s'\r\n", APP_URL_Buffer);
                    APP_URL_Buffer[0] = '\0';
                    break;
                }
                result = TCPIP_DNS_Resolve(appData.host, TCPIP_DNS_TYPE_A);
                if (result == TCPIP_DNS_RES_NAME_IS_IPADDRESS)
                {
                    IPV4_ADDR addr;
                    TCPIP_Helper_StringToIPAddress(appData.host, &addr);
                    appData.clientSocket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                                                          appData.port,
                                                          (IP_MULTI_ADDRESS*) &addr);
                    if (appData.clientSocket == INVALID_SOCKET)
                    {
                        SYS_CONSOLE_MESSAGE("Could not start connection\r\n");
                        appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                    }
                    SYS_CONSOLE_MESSAGE("Starting connection\r\n");
                    appData.clientState = APP_TCPIP_WAIT_FOR_CONNECTION;
                    break;
                }
                if (result < 0)
                {
                    SYS_CONSOLE_MESSAGE("Error in DNS aborting\r\n");
                    APP_URL_Buffer[0] = '\0';
                    break;
                }
                appData.clientState = APP_TCPIP_WAIT_ON_DNS;
                APP_URL_Buffer[0] = '\0';
            }
            else
            {
                appData.state = APP_TCPIP_WAIT_FOR_IP;
            }
#endif
        }
        break;

        case APP_TCPIP_WAIT_ON_DNS:
        {
            IPV4_ADDR addr;
            switch (_APP_PumpDNS(appData.host, &addr))
            {
                case -1:
                {
                    // Some sort of error, already reported
                    appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                }
                break;
                case 0:
                {
                    // Still waiting
                }
                break;
                case 1:
                {
                    appData.clientSocket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                                                          appData.port,
                                                          (IP_MULTI_ADDRESS*) &addr);
                    if (appData.clientSocket == INVALID_SOCKET)
                    {
                        SYS_CONSOLE_MESSAGE("Could not start connection\r\n");
                        appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                    }
                    SYS_CONSOLE_MESSAGE("Starting connection\r\n");
                    appData.clientState = APP_TCPIP_WAIT_FOR_CONNECTION;
                }
                break;
            }
        }
        break;

        case APP_TCPIP_WAIT_FOR_CONNECTION:
        {
            char buffer[MAX_URL_SIZE];
            if (!TCPIP_TCP_IsConnected(appData.clientSocket))
            {
                break;
            }
            if(TCPIP_TCP_PutIsReady(appData.clientSocket) == 0)
            {
                break;
            }
            sprintf(buffer, "GET /%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Connection: close\r\n\r\n", appData.path ? appData.path : "null", appData.host);
            SYS_CONSOLE_PRINT("Sending data %s\r\n", buffer);
            TCPIP_TCP_ArrayPut(appData.clientSocket, (uint8_t*)buffer, strlen(buffer) + 1);
            appData.clientState = APP_TCPIP_WAIT_FOR_RESPONSE;
        }
        break;

        case APP_TCPIP_WAIT_FOR_RESPONSE:
        {
            char buffer[80];
            memset(buffer, 0, sizeof(buffer));
            while (TCPIP_TCP_GetIsReady(appData.clientSocket))
            {
                TCPIP_TCP_ArrayGet(appData.clientSocket, (uint8_t*)buffer, sizeof(buffer) - 1);
                SYS_CONSOLE_PRINT("%s", buffer);
            }

            if (!TCPIP_TCP_IsConnected(appData.clientSocket) || TCPIP_TCP_WasDisconnected(appData.clientSocket))
            {
                SYS_CONSOLE_MESSAGE("\r\nConnection Closed\r\n");
                TCPIP_TCP_Close(appData.clientSocket);
                appData.clientSocket = INVALID_SOCKET;
                appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                break;
            }
        }
        break;
        default:
            break;
    }
}

void _APP_ServerTasks()
{
    switch(appData.serverState)
    {
        case APP_TCPIP_OPENING_SERVER:
        {
            SYS_CONSOLE_PRINT("Waiting for Client Connection on port: %d\r\n", SERVER_PORT);
            appData.serverSocket = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SERVER_PORT, 0);
            if (appData.serverSocket == INVALID_SOCKET)
            {
                SYS_CONSOLE_MESSAGE("Couldn't open server socket\r\n");
                break;
            }
            appData.serverState = APP_TCPIP_WAIT_FOR_CONNECTION;
        }
        break;

        case APP_TCPIP_WAIT_FOR_CONNECTION:
        {
            if (!TCPIP_TCP_IsConnected(appData.serverSocket))
            {
                return;
            }
            else
            {
                // We got a connection
                appData.serverState = APP_TCPIP_SERVING_CONNECTION;
                SYS_CONSOLE_MESSAGE("Server Received a connection\r\n");
            }
        }
        break;

        case APP_TCPIP_SERVING_CONNECTION:
        {
            if (!TCPIP_TCP_IsConnected(appData.serverSocket) || TCPIP_TCP_WasDisconnected(appData.serverSocket))
            {
                appData.serverState = APP_TCPIP_CLOSING_CONNECTION;
                SYS_CONSOLE_MESSAGE("Connection was closed\r\n");
                break;
            }
            int16_t wMaxGet, wMaxPut, wCurrentChunk;
            uint16_t w, w2;
            uint8_t AppBuffer[160 + 1];
            // Figure out how many bytes have been received and how many we can transmit.
            wMaxGet = TCPIP_TCP_GetIsReady(appData.serverSocket);	// Get TCP RX FIFO byte count
            wMaxPut = TCPIP_TCP_PutIsReady(appData.serverSocket);	// Get TCP TX FIFO free space

            // Make sure we don't take more bytes out of the RX FIFO than we can put into the TX FIFO
            if(wMaxPut < wMaxGet)
                    wMaxGet = wMaxPut;

            // Process all bytes that we can
            // This is implemented as a loop, processing up to sizeof(AppBuffer) bytes at a time.
            // This limits memory usage while maximizing performance.  Single byte Gets and Puts are a lot slower than multibyte GetArrays and PutArrays.
            wCurrentChunk = sizeof(AppBuffer) - 1;
            for(w = 0; w < wMaxGet; w += sizeof(AppBuffer) - 1)
            {
                // Make sure the last chunk, which will likely be smaller than sizeof(AppBuffer), is treated correctly.
                if(w + sizeof(AppBuffer) - 1 > wMaxGet)
                    wCurrentChunk = wMaxGet - w;

                // Transfer the data out of the TCP RX FIFO and into our local processing buffer.
                TCPIP_TCP_ArrayGet(appData.serverSocket, AppBuffer, wCurrentChunk);

                // Perform the "ToUpper" operation on each data byte
                for(w2 = 0; w2 < wCurrentChunk; w2++)
                {
                    char i = AppBuffer[w2];
                    if(i >= 'a' && i <= 'z')
                    {
                            i -= ('a' - 'A');
                            AppBuffer[w2] = i;
                    }
                    else if(i == '\x1b')   //escape
                    {
                        appData.serverState = APP_TCPIP_CLOSING_CONNECTION;
                        SYS_CONSOLE_MESSAGE("Connection was closed\r\n");
                    }
                }
                AppBuffer[w2] = 0;  // end the console string properly


                // Transfer the data out of our local processing buffer and into the TCP TX FIFO.
                SYS_CONSOLE_PRINT("Server Sending %s\r\n", AppBuffer);
                TCPIP_TCP_ArrayPut(appData.serverSocket, AppBuffer, wCurrentChunk);

                // No need to perform any flush.  TCP data in TX FIFO will automatically transmit itself after it accumulates for a while.  If you want to decrease latency (at the expense of wasting network bandwidth on TCP overhead), perform and explicit flush via the TCPFlush() API.
            }
        }
        break;
        case APP_TCPIP_CLOSING_CONNECTION:
        {
            // Close the socket connection.
            TCPIP_TCP_Close(appData.serverSocket);
            appData.serverSocket = INVALID_SOCKET;
            appData.state = APP_TCPIP_WAIT_FOR_IP;
        }
        break;
        default:
            break;
    }

}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    SYS_STATUS          tcpipStat;
    const char          *netName, *netBiosName;
    static IPV4_ADDR    dwLastIP[2] = { {-1}, {-1} };
    IPV4_ADDR           ipAddr;
    TCPIP_NET_HANDLE    netH;
    int                 i, nNets;

    switch(appData.state)
    {
        case APP_TCPIP_WAIT_INIT:
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if(tcpipStat < 0)
            {   // some error occurred
                SYS_CONSOLE_MESSAGE(" APP: TCP/IP stack initialization failed!\r\n");
                appData.state = APP_TCPIP_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                // now that the stack is ready we can check the
                // available interfaces
                nNets = TCPIP_STACK_NumberOfNetworksGet();
                for(i = 0; i < nNets; i++)
                {

                    netH = TCPIP_STACK_IndexToNet(i);
                    netName = TCPIP_STACK_NetNameGet(netH);
                    netBiosName = TCPIP_STACK_NetBIOSName(netH);

#if defined(TCPIP_STACK_USE_NBNS)
                    SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS enabled\r\n", netName, netBiosName);
#else
                    SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS disabled\r\n", netName, netBiosName);
#endif  // defined(TCPIP_STACK_USE_NBNS)
                    (void)netName;          // avoid compiler warning 
                    (void)netBiosName;      // if SYS_CONSOLE_PRINT is null macro

                }
                appData.state = APP_TCPIP_WAIT_FOR_IP;

            }
            break;


        case APP_TCPIP_WAIT_FOR_IP:

            // if the IP address of an interface has changed
            // display the new value on the system console
            nNets = TCPIP_STACK_NumberOfNetworksGet();

            for (i = 0; i < nNets; i++)
            {
                netH = TCPIP_STACK_IndexToNet(i);
                if(!TCPIP_STACK_NetIsReady(netH))
                {
                    return;    // interface not ready yet!
                }
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                if(dwLastIP[i].Val != ipAddr.Val)
                {
                    dwLastIP[i].Val = ipAddr.Val;

                    SYS_CONSOLE_MESSAGE(TCPIP_STACK_NetNameGet(netH));
                    SYS_CONSOLE_MESSAGE(" IP Address: ");
                    SYS_CONSOLE_PRINT("%d.%d.%d.%d \r\n", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                    SYS_CONSOLE_MESSAGE("Waiting for command type: openurl <url>\r\n");
                }
                appData.state = appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                if(appData.serverSocket == INVALID_SOCKET)
                {
                    appData.serverState = APP_TCPIP_OPENING_SERVER;
                }
            }
            break;

        case APP_TCPIP_ERROR:
            break;

        default:
            _APP_ClientTasks();
            _APP_ServerTasks();
            break;
    }
}
 
int32_t _APP_ParseUrl(char *uri, char **host, char **path, uint16_t * port)
{
    char * pos;
    pos = strstr(uri, "//"); //Check to see if its a proper URL

    if ( !pos )
    {
        return -1;
    }
    *host = pos + 2; // This is where the host should start

    pos = strchr( * host, ':');

    if ( !pos )
    {
        pos = strchr(*host, '/');
        if (!pos)
        {
            *path = NULL;
        }
        else
        {
            *pos = '\0';
            *path = pos + 1;
        }
    }
    else
    {
        *pos = '\0';
        char * portc = pos + 1;

        pos = strchr(portc, '/');
        if (!pos)
        {
            *path = NULL;
        }
        else
        {
            *pos = '\0';
            *path = pos + 1;
        }
        *port = atoi(portc);
    }
    return 0;
}

int8_t _APP_PumpDNS(const char * hostname, IPV4_ADDR *ipv4Addr)
{
    int8_t retval = -1;
    TCPIP_DNS_RESULT result = TCPIP_DNS_IsResolved(hostname, (IP_MULTI_ADDRESS*)ipv4Addr, IP_ADDRESS_TYPE_IPV4);

    switch (result)
    {
        case TCPIP_DNS_RES_OK:
            // We now have an IPv4 Address
            // Open a socket
            retval = 1;
            break;
        case TCPIP_DNS_RES_PENDING:
            retval = 0;
            break;
        case TCPIP_DNS_RES_SERVER_TMO:
        default:
            SYS_CONSOLE_MESSAGE("TCPIP_DNS_IsResolved returned failure\r\n");
            break;
    }
    return retval;
}

/*******************************************************************************
 End of File
 */

