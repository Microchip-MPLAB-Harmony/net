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

// print buffer
char printBuffer[180];

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
    appData.tmoMs = 1000;
    
    /* TODO: Initialize your application's state machine and other
     */
    APP_Commands_Init();
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
    /* Check the application's current state. */
    switch ( appData.state )
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
			appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            SYS_CONSOLE_MESSAGE("Waiting for command type: sendudp\r\n");
            
			break;
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
                    appData.socket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                                                          port,
                                                          (IP_MULTI_ADDRESS*) &addr);
                    if (appData.socket == INVALID_SOCKET)
                    {
                        SYS_CONSOLE_MESSAGE("Could not start connection\r\n");
                        appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                    }
                    SYS_CONSOLE_MESSAGE("Starting connection\r\n");
                    appData.state = APP_TCPIP_WAIT_FOR_CONNECTION;
                    break;
                }
                if (result < 0)
                {
                    SYS_CONSOLE_MESSAGE("Error in DNS aborting 2\r\n");
                    break;
                }
                appData.state = APP_TCPIP_WAIT_ON_DNS;
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
                    // Some sort of error, already reported
                    appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                }
                break;
                case 0:
                {
                    // Still waiting
                }
                break;
                case 1:
                {
                    uint16_t port = atoi(APP_Port_Buffer);
                    appData.socket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                                                          port,
                                                          (IP_MULTI_ADDRESS*) &addr);
                    if (appData.socket == INVALID_SOCKET)
                    {
                        SYS_CONSOLE_MESSAGE("Could not start connection\r\n");
                        appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                    }
                    SYS_CONSOLE_MESSAGE("Starting connection\r\n");
                    appData.state = APP_TCPIP_WAIT_FOR_CONNECTION;
                }
                break;
            }
        }
        break;

        case APP_TCPIP_WAIT_FOR_CONNECTION:
        {
            if (!TCPIP_UDP_IsConnected(appData.socket))
            {
                break;
            }
            if(TCPIP_UDP_PutIsReady(appData.socket) == 0)
            {
                break;
            }
            TCPIP_UDP_ArrayPut(appData.socket, (uint8_t*)APP_Message_Buffer, strlen(APP_Message_Buffer));
            TCPIP_UDP_Flush(appData.socket);
            appData.mTimeOut = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() * (uint64_t)appData.tmoMs) / 1000ull;
            appData.state = APP_TCPIP_WAIT_FOR_RESPONSE;
        }
        break;

        case APP_TCPIP_WAIT_FOR_RESPONSE:
        {
            memset(printBuffer, 0, sizeof(printBuffer));
            if (SYS_TMR_SystemCountGet() > appData.mTimeOut)
            {
                SYS_CONSOLE_MESSAGE("\r\nTime out waiting for response\r\n");
                TCPIP_UDP_Close(appData.socket);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                break;
            }
            if (!TCPIP_UDP_IsConnected(appData.socket))
            {
                SYS_CONSOLE_MESSAGE("\r\nConnection Closed\r\n");
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                break;
            }
            if (TCPIP_UDP_GetIsReady(appData.socket))
            {
                TCPIP_UDP_ArrayGet(appData.socket, (uint8_t*)printBuffer, sizeof(printBuffer) - 1);
                TCPIP_UDP_Discard(appData.socket);
                SYS_CONSOLE_PRINT("%s", printBuffer);
                TCPIP_UDP_Close(appData.socket);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;

            }
        }

        break;
        default:
            break;
    }
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
    }

    return retval;
}
 

/*******************************************************************************
 End of File
 */

