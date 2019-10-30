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
#define SERVER_PORT 9760
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
    appData.clientState = APP_TCPIP_WAIT_INIT;
    appData.serverState = APP_TCPIP_WAIT_INIT;
    appData.tmoMs = 1000;
    
    /* TODO: Initialize your application's state machine and other
     */
#if defined(TCPIP_STACK_COMMAND_ENABLE)
    APP_Commands_Init();
#endif
}

void _APP_ClientTasks()
{
#if defined(TCPIP_STACK_COMMAND_ENABLE) 
    switch(appData.clientState)
    {
        case APP_TCPIP_WAITING_FOR_COMMAND:
        {
         
            if (APP_Send_Packet)
            {
                TCPIP_DNS_RESULT result;
                APP_Send_Packet = false;
                result = TCPIP_DNS_Resolve(APP_Hostname_Buffer, TCPIP_DNS_TYPE_A);
                if (result == TCPIP_DNS_RES_NAME_IS_IPADDRESS)
                {
                    IPV4_ADDR addr;
                    TCPIP_Helper_StringToIPAddress(APP_Hostname_Buffer, &addr);
                                        uint16_t port = atoi(APP_Port_Buffer);
                    appData.clientSocket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                                                          port,
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
                    SYS_CONSOLE_MESSAGE("Error in DNS aborting 2\r\n");
                    break;
                }
                appData.clientState = APP_TCPIP_WAIT_ON_DNS;
            }

        }
        break;

        case APP_TCPIP_WAIT_ON_DNS:
        {
            IPV4_ADDR addr;
            switch (_APP_PumpDNS(APP_Hostname_Buffer, &addr))
            {
                case -1:
                {
                    appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                }
                break;
                case 0:
                {
                }
                break;
                case 1:
                {
                    uint16_t port = atoi(APP_Port_Buffer);
                    appData.clientSocket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                                                          port,
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
            if (!TCPIP_UDP_IsConnected(appData.clientSocket))
            {
                break;
            }
            if(TCPIP_UDP_PutIsReady(appData.clientSocket) == 0)
            {
                break;
            }
            TCPIP_UDP_ArrayPut(appData.clientSocket, (uint8_t*)APP_Message_Buffer, strlen(APP_Message_Buffer));
            TCPIP_UDP_Flush(appData.clientSocket);
            appData.clientState = APP_TCPIP_WAIT_FOR_RESPONSE;
            appData.mTimeOut = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() * (uint64_t)appData.tmoMs) / 1000ull;
        }
        break;

        case APP_TCPIP_WAIT_FOR_RESPONSE:
        {
            char buffer[180];
            memset(buffer, 0, sizeof(buffer));
            if (SYS_TMR_SystemCountGet() > appData.mTimeOut)
            {
                SYS_CONSOLE_MESSAGE("\r\nTime out waiting for response\r\n");
                TCPIP_UDP_Close(appData.clientSocket);
                appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                break;
            }
            if (!TCPIP_UDP_IsConnected(appData.clientSocket))
            {
                SYS_CONSOLE_MESSAGE("\r\nClient Connection Closed\r\n");
                appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                break;
            }
            if (TCPIP_UDP_GetIsReady(appData.clientSocket))
            {
                TCPIP_UDP_ArrayGet(appData.clientSocket, (uint8_t*)buffer, sizeof(buffer) - 1);
                TCPIP_UDP_Discard(appData.clientSocket);
                SYS_CONSOLE_PRINT("Client received %s\r\n", buffer);
                appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
                TCPIP_UDP_Close(appData.clientSocket);

            }
        }
        break;
        default:
            break;
    }
#endif
}

void _APP_ServerTasks( void )
{
    switch(appData.serverState)
    {
        case APP_TCPIP_OPENING_SERVER:
        {
            SYS_CONSOLE_PRINT("Waiting for Client Connection on port: %d\r\n", SERVER_PORT);
            appData.serverSocket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SERVER_PORT, 0);
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
            if (!TCPIP_UDP_IsConnected(appData.serverSocket))
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
            if (!TCPIP_UDP_IsConnected(appData.serverSocket))
            {
                appData.serverState = APP_TCPIP_CLOSING_CONNECTION;
                SYS_CONSOLE_MESSAGE("Server Connection was closed\r\n");
                break;
            }
            int16_t wMaxGet, wMaxPut, wCurrentChunk;
            uint16_t w, w2;
            uint8_t AppBuffer[32 + 1];
            memset(AppBuffer, 0, sizeof(AppBuffer));
            // Figure out how many bytes have been received and how many we can transmit.
            wMaxGet = TCPIP_UDP_GetIsReady(appData.serverSocket);	// Get UDP RX FIFO byte count
            wMaxPut = TCPIP_UDP_PutIsReady(appData.serverSocket);

            //SYS_CONSOLE_PRINT("\t%d bytes are available.\r\n", wMaxGet);
            if (wMaxGet == 0)
            {
                break;
            }

            if (wMaxPut < wMaxGet)
            {
                wMaxGet = wMaxPut;
            }

            SYS_CONSOLE_PRINT("Server RX Buffer has %d bytes in it\r\n", wMaxGet);

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
                int rxed = TCPIP_UDP_ArrayGet(appData.serverSocket, AppBuffer, sizeof(AppBuffer) - 1);

                SYS_CONSOLE_PRINT("\tReceived a message of '%s' and length %d\r\n", AppBuffer, rxed);

                // Perform the "ToUpper" operation on each data byte
                for(w2 = 0; w2 < wCurrentChunk; w2++)
                {
                    char i;
                    i = AppBuffer[w2];
                    if(i >= 'a' && i <= 'z')
                    {
                            i -= ('a' - 'A');
                            AppBuffer[w2] = i;
                    }
                    else if(i == '\x1b')   // escape
                    {
                        SYS_CONSOLE_MESSAGE("Server Connection was closed\r\n");
                    }
                }
                AppBuffer[w2] = 0;

                SYS_CONSOLE_PRINT("\tServer Sending a messages '%s'\r\n", AppBuffer);

                // Transfer the data out of our local processing buffer and into the TCP TX FIFO.
                TCPIP_UDP_ArrayPut(appData.serverSocket, AppBuffer, wCurrentChunk);

                appData.serverState = APP_TCPIP_CLOSING_CONNECTION;
            }
            TCPIP_UDP_Flush(appData.serverSocket);
            TCPIP_UDP_Discard(appData.serverSocket);
        }
        break;
        case APP_TCPIP_CLOSING_CONNECTION:
        {
            			// Close the socket connection.
            TCPIP_UDP_Close(appData.serverSocket);

            appData.serverState = APP_TCPIP_OPENING_SERVER;

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
    int                 i, nNets;
    TCPIP_NET_HANDLE    netH;

    /* Check the application's current state. */
    switch(appData.clientState)
    {
        case APP_TCPIP_WAIT_INIT:
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if(tcpipStat < 0)
            {   // some error occurred
                SYS_CONSOLE_MESSAGE(" APP: TCP/IP stack initialization failed!\r\n");
                appData.clientState = APP_TCPIP_ERROR;
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
                appData.clientState = APP_TCPIP_WAIT_FOR_IP;

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
					return; // interface not ready yet!
				}
                
				ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                if(dwLastIP[i].Val != ipAddr.Val)
                {
                    dwLastIP[i].Val = ipAddr.Val;

                    SYS_CONSOLE_MESSAGE(TCPIP_STACK_NetNameGet(netH));
                    SYS_CONSOLE_MESSAGE(" IP Address: ");
                    SYS_CONSOLE_PRINT("%d.%d.%d.%d \r\n", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                
                }			     
            }
			// all interfaces ready. Could start transactions!!!
			appData.clientState = APP_TCPIP_WAITING_FOR_COMMAND;
			appData.serverState = APP_TCPIP_OPENING_SERVER;
			SYS_CONSOLE_MESSAGE("Waiting for command type: sendudp\r\n");
            break;


        case APP_TCPIP_ERROR:
            break;

        default:
            _APP_ClientTasks();
            _APP_ServerTasks();
            break;
    }
}


int8_t _APP_PumpDNS(const char * hostname, IPV4_ADDR *ipv4Addr)
{
    int8_t  retval = -1;
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

