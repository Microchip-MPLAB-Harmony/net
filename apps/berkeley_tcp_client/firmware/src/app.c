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
#include "system_config.h"

#include "tcpip/tcpip.h"

#include "app_commands.h"
#include <sys/errno.h>
#include <errno.h>
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

APP_DATA appData ={
    .state = APP_WAITING_FOR_INITIALIZATION
};
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
    appData.state = APP_WAITING_FOR_INITIALIZATION;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
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

void APP_Tasks(void) {
    SYS_STATUS tcpipStat;
    const char *netName, *netBiosName;
    struct hostent * hostInfo;
    int i, nNets;
    TCPIP_NET_HANDLE netH;
    /* Check the application's current state. */
    switch (appData.state) {
        case APP_WAITING_FOR_INITIALIZATION:

            // wait for TCP/IP stack initialization
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
                    SYS_CONSOLE_PRINT("Interface %s on host %s - NBNS enabled\r\n", netName, netBiosName);
#else
                    SYS_CONSOLE_PRINT("Interface %s on host %s - NBNS disabled\r\n", netName, netBiosName);
#endif  // defined(TCPIP_STACK_USE_NBNS)
                    (void)netName;          // avoid compiler warning 
                    (void)netBiosName;      // if SYS_CONSOLE_PRINT is null macro

                }
                appData.state = APP_TCPIP_WAIT_FOR_IP;
            }

            break;
        case APP_TCPIP_WAIT_FOR_IP:
        {
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
            appData.state = APP_TCPIP_WAITING_FOR_COMMAND_READY;
            //... etc.
            break;
        }
        case APP_TCPIP_WAITING_FOR_COMMAND_READY:
        {
            appData.state = APP_TCPIP_WAITING_FOR_COMMAND_INPUT;
        }
            break;
        case APP_TCPIP_WAITING_FOR_COMMAND_INPUT:
        {
#if defined(TCPIP_STACK_COMMAND_ENABLE)
            if (APP_URL_Buffer[0] != '\0') {
                if (_APP_ParseUrl(APP_URL_Buffer, &appData.host, &appData.path, &appData.port)) {
                    SYS_CONSOLE_PRINT("Could not parse URL '%s'\r\n", APP_URL_Buffer);
                    APP_URL_Buffer[0] = '\0';
                    break;
                }
                appData.state = APP_DNS_START_RESOLUTION;
            }
#endif

        }
            break;

        case APP_DNS_START_RESOLUTION:
        {
            hostInfo = gethostbyname(appData.host);
            if (hostInfo != NULL) {
                memcpy(&appData.addr.sin_addr.S_un.S_addr,
                        *(hostInfo->h_addr_list), sizeof (IPV4_ADDR));
                appData.state = APP_BSD_START;
            } else {
                break;
            }
        }

        case APP_BSD_START:
        {
            int tcpSkt;
            // Create a socket for this client to connect with
            if ((tcpSkt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) {
                return;
            } else {
                appData.socket = (SOCKET) tcpSkt;
            }

            SYS_CONSOLE_MESSAGE("BSD TCP client: Connecting...\r\n");

            appData.state = APP_BSD_CONNECT;
        }
            break;

        case APP_BSD_CONNECT:
        {
            int addrlen;
            // addr.sin_addr.S_un.S_addr destination IP address was set earlier in DNS step
            appData.addr.sin_port = appData.port;
            addrlen = sizeof (struct sockaddr);
            if (connect(appData.socket, (struct sockaddr*) &appData.addr, addrlen) < 0)
                return;

            appData.state = APP_BSD_SEND;
        }
            break;
        case APP_BSD_SEND:
        {
            //send TCP data
            char buffer[MAX_URL_SIZE];
            sprintf(buffer, "GET /%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Connection: close\r\n\r\n", appData.path, appData.host);

            send(appData.socket, (const char*) buffer, strlen((char*) buffer), 0);
            appData.state = APP_BSD_OPERATION;
        }
            break;

        case APP_BSD_OPERATION:
            // Obtian and print the server reply
        {
            char recvBuffer[80];
            i = recv(appData.socket, recvBuffer, sizeof (recvBuffer) - 1, 0); //get the data from the recv queue

            if (i <= 0) // close or error condition
            {
                if (i == 0 || errno != EWOULDBLOCK) {
                    appData.state = APP_BSD_CLOSE; // true error/close condition
                }
                // else just wait for some more data

                break;
            }

            recvBuffer[i] = '\0'; // Null terminate data
            SYS_CONSOLE_PRINT("%s", recvBuffer);

            if (appData.state == APP_BSD_OPERATION)
                break;
        }
            break;

        case APP_BSD_CLOSE:
            closesocket(appData.socket);
            SYS_CONSOLE_MESSAGE("\r\nConnection Closed\r\n");
            appData.state = APP_TCPIP_WAITING_FOR_COMMAND_INPUT;

#if defined(TCPIP_STACK_COMMAND_ENABLE)
            APP_URL_Buffer[0] = '\0';
#endif
            // No break needed
        default:
            return;
    }
}

int32_t _APP_ParseUrl(char *uri, char **host, char **path, uint16_t * port) {
    char * pos;
    pos = strstr(uri, "//"); //Check to see if its a proper URL
    *port = 80;


    if (!pos) {
        return -1;
    }
    *host = pos + 2; // This is where the host should start

    pos = strchr(* host, ':');

    if (!pos) {
        pos = strchr(*host, '/');
        if (!pos) {
            *path = NULL;
        } else {
            *pos = '\0';
            *path = pos + 1;
        }
    } else {
        *pos = '\0';
        char * portc = pos + 1;

        pos = strchr(portc, '/');
        if (!pos) {
            *path = NULL;
        } else {
            *pos = '\0';
            *path = pos + 1;
        }
        *port = atoi(portc);
    }
    return 0;
}

int8_t _APP_PumpDNS(const char * hostname, IPV4_ADDR *ipv4Addr) {
    TCPIP_DNS_RESULT result = TCPIP_DNS_IsResolved(hostname, (IP_MULTI_ADDRESS*) ipv4Addr, IP_ADDRESS_TYPE_IPV4);

    switch (result) {
        case TCPIP_DNS_RES_OK:
        {
            SYS_CONSOLE_PRINT("Resolved IPAddress : %d.%d.%d.%d\r\n", ipv4Addr->v[0], ipv4Addr->v[1], ipv4Addr->v[2], ipv4Addr->v[3]);
            return 1;
        }
        case TCPIP_DNS_RES_PENDING:
            return 0;
        case TCPIP_DNS_RES_SERVER_TMO:
        default:
            SYS_CONSOLE_MESSAGE("TCPIP_DNS_IsResolved returned failure\r\n");
            return -1;
    }
    // Should not be here!
    return -1;

}
/*******************************************************************************
 End of File
 */

