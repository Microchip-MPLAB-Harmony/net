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
#include "system_config.h"
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

void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_TCPIP_WAIT_INIT;


    /* Intialize the app state to wait for
     * media attach. */
#if defined(TCPIP_STACK_COMMAND_ENABLE)
    APP_Commands_Init();
#endif
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
char buffer[180];

void APP_Tasks(void) {
    SYS_STATUS tcpipStat;
    const char *netName, *netBiosName;
    int i, nNets;
    TCPIP_NET_HANDLE netH;

    switch (appData.state) {

        case APP_TCPIP_WAIT_INIT:
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat < 0) { // some error occurred
                SYS_CONSOLE_MESSAGE(" APP: TCP/IP stack initialization failed!\r\n");
                appData.state = APP_TCPIP_ERROR;
            } else if (tcpipStat == SYS_STATUS_READY) {
                // now that the stack is ready we can check the 
                // available interfaces 
                nNets = TCPIP_STACK_NumberOfNetworksGet();

                for (i = 0; i < nNets; i++) {

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
            nNets = TCPIP_STACK_NumberOfNetworksGet();
            for (i = 0; i < nNets; i++) {
                netH = TCPIP_STACK_IndexToNet(i);
                if (!TCPIP_STACK_NetIsReady(netH)) {
                    return; // interface not ready yet!
                }
                IPV4_ADDR           ipAddr;
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                SYS_CONSOLE_MESSAGE(TCPIP_STACK_NetNameGet(netH));
                SYS_CONSOLE_MESSAGE(" IP Address: ");
                SYS_CONSOLE_PRINT("%d.%d.%d.%d \r\n", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);            
            }
            // all interfaces ready. Could start transactions!!!
            appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            //... etc.
            break;
        case APP_TCPIP_WAITING_FOR_COMMAND:
        {
#if defined(TCPIP_STACK_COMMAND_ENABLE)
            if (APP_Send_Packet) {
                int res = getaddrinfo(APP_Hostname_Buffer, NULL, &appData.hints, &appData.addressInfo);
                if (res == 0) {
                    APP_Send_Packet = false;
                    SYS_CONSOLE_MESSAGE("Starting connection\r\n");
                    appData.state = APP_BSD_CREATE_SOCKET;
                    break;
                } else if (res == EAI_AGAIN) {
                    break;
                } else {
                    SYS_CONSOLE_PRINT("getaddrinfo returned %d aborting sendpacket call\r\n", res);
                    APP_Send_Packet = false;
                    break;
                }
            }
#endif
        }
            break;

        case APP_BSD_CREATE_SOCKET:
        {
            SOCKET udpSkt = socket(appData.addressInfo->ai_family, SOCK_DGRAM, IPPROTO_UDP);
            if (udpSkt == SOCKET_ERROR) {
                SYS_CONSOLE_MESSAGE("Aborting because of invalid socket\r\n");
                freeaddrinfo(appData.addressInfo);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                return;
            } else {
                appData.socket = (SOCKET) udpSkt;
            }
        }

        case APP_BSD_BIND:
        {
#if defined(TCPIP_STACK_COMMAND_ENABLE)
            struct sockaddr_in6 addr;
            SOCKADDR_IN *ptr = (SOCKADDR_IN*) (&addr);
            int addrlen = sizeof (struct sockaddr_in6);
            if (appData.addressInfo->ai_family == AF_INET6) {
                addr.sin6_family = AF_INET6;
                addr.sin6_port = atoi(APP_Port_Buffer);
                addr.sin6_addr.in6_u.u6_addr16[0] = 0x0120;
                addr.sin6_addr.in6_u.u6_addr16[1] = 0x7004;
                addr.sin6_addr.in6_u.u6_addr16[2] = 0x0b00;
                addr.sin6_addr.in6_u.u6_addr16[3] = 0x6709;
                addr.sin6_addr.in6_u.u6_addr16[4] = 0x0402;
                addr.sin6_addr.in6_u.u6_addr16[5] = 0xffa3;
                addr.sin6_addr.in6_u.u6_addr16[6] = 0x50fe;
                addr.sin6_addr.in6_u.u6_addr16[7] = 0x978d;

            } else {
                ptr->sin_family = AF_INET;
                ptr->sin_port = atoi(APP_Port_Buffer);
                ptr->sin_addr.S_un.S_addr = IP_ADDR_ANY;
            }
            if (bind(appData.socket, (struct sockaddr*) &addr, addrlen) == SOCKET_ERROR) {
                SYS_CONSOLE_MESSAGE("Aborting because could not bind\r\n");
                freeaddrinfo(appData.addressInfo);
                closesocket(appData.socket);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                break;
            }
#endif
        }

        case APP_TCPIP_SEND:
        {
#if defined(TCPIP_STACK_COMMAND_ENABLE)
            if (appData.addressInfo->ai_family == AF_INET6) {
                ((struct sockaddr_in6*) ((appData.addressInfo->ai_addr)))->sin6_port = atoi(APP_Port_Buffer);
            } else {
                ((struct sockaddr_in*) ((appData.addressInfo->ai_addr)))->sin_port = atoi(APP_Port_Buffer);
            }
            if (sendto(appData.socket, (const char*) &APP_Message_Buffer, strlen(APP_Message_Buffer), 0, appData.addressInfo->ai_addr, appData.addressInfo->ai_addrlen) > 0) {
                appData.state = APP_TCPIP_RECV;
                appData.systemCount = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() * 10);
            } else {
                SYS_CONSOLE_MESSAGE("Aborting because send failed\r\n");
                freeaddrinfo(appData.addressInfo);
                closesocket(appData.socket);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            }
#endif            
        }
            break;

        case APP_TCPIP_RECV:
        {

            int size;
            int i = recvfrom(appData.socket, (char*) &buffer, sizeof (buffer), 0, (struct sockaddr*) (&appData.rxaddr), &size);
            if (i < 0 && errno != EWOULDBLOCK) {
                // Error occured
                SYS_CONSOLE_MESSAGE("Aborting because received failed\r\n");
                freeaddrinfo(appData.addressInfo);
                closesocket(appData.socket);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            } else if (i == 0) {
                break;
            } else if (i < 0 && errno == EWOULDBLOCK) {
                if (SYS_TMR_SystemCountGet() > appData.systemCount) {
                    SYS_CONSOLE_MESSAGE("Aborting because of timeout waiting for reply\r\n");
                    freeaddrinfo(appData.addressInfo);
                    closesocket(appData.socket);
                    appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                }
            } else {
                SYS_CONSOLE_MESSAGE(buffer);
                freeaddrinfo(appData.addressInfo);
                closesocket(appData.socket);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            }
        }
            break;
        default:
            break;
    }
}


/*******************************************************************************
 End of File
 */

