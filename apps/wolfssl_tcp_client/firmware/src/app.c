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
#include "app_commands.h"
#include <wolfssl/ssl.h>
#include <tcpip/src/hash_fnv.h>

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

int32_t _APP_ParseUrl(char *uri, char **host, char **path, uint16_t * port);

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************
#ifndef htons

uint16_t htons(uint16_t n) {
    return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}
#endif

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void) {
    memset(&appData, 0, sizeof (appData));
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    APP_Commands_Init();
    appData.ipMode = 4;
    appData.cyaSSLLogEnabled = 0;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
char networkBuffer[256];

void APP_Tasks(void) {
    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {
            appData.state = APP_TCPIP_WAIT_FOR_TCPIP_INIT;
            SYS_CONSOLE_MESSAGE(" APP: Initialization\r\n");
            break;
        }

        case APP_TCPIP_WAIT_FOR_TCPIP_INIT:
        {
            SYS_STATUS tcpipStat;
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            const char *netName, *netBiosName;
            int i, nNets;
            TCPIP_NET_HANDLE netH;
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
        }
        case APP_TCPIP_WAIT_FOR_IP:
        {
            int i, nNets;
            TCPIP_NET_HANDLE netH;
            nNets = TCPIP_STACK_NumberOfNetworksGet();
            for (i = 0; i < nNets; i++) {
                netH = TCPIP_STACK_IndexToNet(i);
                if (!TCPIP_STACK_NetIsReady(netH)) {
                    return; // interface not ready yet!
                }
                IPV4_ADDR ipAddr;
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                SYS_CONSOLE_MESSAGE(TCPIP_STACK_NetNameGet(netH));
                SYS_CONSOLE_MESSAGE(" IP Address: ");
                SYS_CONSOLE_PRINT("%d.%d.%d.%d \r\n", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
            }
            // all interfaces ready. Could start transactions!!!
            appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            //... etc.
            break;
        }
        case APP_TCPIP_WAITING_FOR_COMMAND:
            break;
        case APP_TCPIP_PROCESS_COMMAND:
        {
            if (_APP_ParseUrl(appData.urlBuffer, &appData.host, &appData.path, &appData.port)) {
                SYS_CONSOLE_PRINT("Could not parse URL '%s'\r\n", appData.urlBuffer);
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                break;
            }
            appData.testStart = SYS_TMR_SystemCountGet();
            appData.dnsComplete = 0;
            appData.connectionOpened = 0;
            appData.sslNegComplete = 0;
            appData.firstRxDataPacket = 0;
            appData.lastRxDataPacket = 0;
            appData.rawBytesReceived = 0;
            appData.rawBytesSent = 0;
            appData.clearBytesReceived = 0;
            appData.clearBytesSent = 0;
            //appData.cyaSSLLogEnabled = 1; // Uncomment this if you want cyassl debugging output.  Make sure its selected in MHC too


            /* First check to see if host is an IPv4 or IPv6 address*/
            if (TCPIP_Helper_StringToIPAddress(appData.host, &appData.address.v4Add)) {
                appData.queryState = 4;
                if (appData.ipMode != 0 && appData.ipMode != 4) {
                    SYS_CONSOLE_PRINT("Inputed URL is an IPv4 Address but ipmode is set to %d\r\n", appData.ipMode);
                    appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                    break;
                } else {
                    SYS_CONSOLE_PRINT("Using IPv4 Address: %d.%d.%d.%d for host '%s'\r\n",
                            appData.address.v4Add.v[0],
                            appData.address.v4Add.v[1],
                            appData.address.v4Add.v[2],
                            appData.address.v4Add.v[3],
                            appData.host);
                }
            } else if (TCPIP_Helper_StringToIPv6Address(appData.host, &appData.address.v6Add)) {
                appData.queryState = 6;
                if (appData.ipMode != 0 && appData.ipMode != 6) {
                    SYS_CONSOLE_PRINT("Inputed URL is an IPv6 Address but ipmode is set to %d\r\n", appData.ipMode);
                    appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                    break;
                } else {
                    SYS_CONSOLE_PRINT("Using IPv6 Address: %x:%x:%x:%x:%x:%x:%x:%x for host '%s'\r\n",
                            appData.address.v6Add.w[0],
                            appData.address.v6Add.w[1],
                            appData.address.v6Add.w[2],
                            appData.address.v6Add.w[3],
                            appData.address.v6Add.w[4],
                            appData.address.v6Add.w[5],
                            appData.address.v6Add.w[6],
                            appData.address.v6Add.w[7],
                            appData.host);
                }
            } else {
                TCPIP_DNS_RESULT result;
                SYS_CONSOLE_PRINT("Using DNS to Resolve '%s'\r\n", appData.host);
                if (appData.ipMode == 0 || appData.ipMode == 6) {
                    result = TCPIP_DNS_Resolve(appData.host, TCPIP_DNS_TYPE_AAAA);
                    appData.queryState = 6;
                } else {
                    result = TCPIP_DNS_Resolve(appData.host, TCPIP_DNS_TYPE_A);
                    appData.queryState = 4;
                }
                SYS_ASSERT(result != TCPIP_DNS_RES_NAME_IS_IPADDRESS, "DNS Result is TCPIP_DNS_RES_NAME_IS_IPADDRESS, which should not happen since we already checked");
                if (result >= 0) {
                    appData.state = APP_TCPIP_WAIT_ON_DNS;
                } else {
                    SYS_CONSOLE_PRINT("DNS Query returned %d Aborting\r\n", result);
                    appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                }
                break;
            }
            // If we're here, we have a valid IP address
            appData.state = APP_TCPIP_START_CONNECTION;
            break;
        }
        case APP_TCPIP_WAIT_ON_DNS:
        {
            TCPIP_DNS_RESULT result = TCPIP_DNS_IsResolved(appData.host, &appData.address, appData.ipMode == 4 ? IP_ADDRESS_TYPE_IPV4 : IP_ADDRESS_TYPE_IPV6);
            switch (result) {
                case TCPIP_DNS_RES_PENDING:
                {
                }
                    break;
                case TCPIP_DNS_RES_OK:
                {
                    if (appData.queryState == 4) {
                        SYS_CONSOLE_PRINT("DNS Resolved IPv4 Address: %d.%d.%d.%d for host '%s'\r\n",
                                appData.address.v4Add.v[0],
                                appData.address.v4Add.v[1],
                                appData.address.v4Add.v[2],
                                appData.address.v4Add.v[3],
                                appData.host);
                    } else {
                        SYS_CONSOLE_PRINT("DNS Resolved IPv6 Address: %x:%x:%x:%x:%x:%x:%x:%x for host '%s'\r\n",
                                htons(appData.address.v6Add.w[0]),
                                htons(appData.address.v6Add.w[1]),
                                htons(appData.address.v6Add.w[2]),
                                htons(appData.address.v6Add.w[3]),
                                htons(appData.address.v6Add.w[4]),
                                htons(appData.address.v6Add.w[5]),
                                htons(appData.address.v6Add.w[6]),
                                htons(appData.address.v6Add.w[7]),
                                appData.host);
                    }
                    appData.state = APP_TCPIP_START_CONNECTION;
                    break;
                }
                default:
                {
                    if (appData.queryState == 4 || appData.ipMode == 6) {
                        SYS_CONSOLE_PRINT("DNS Is Resolved returned %d Aborting\r\n", result);
                        appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                        break;
                    } else {
                        SYS_CONSOLE_PRINT("DNS Is Resolved returned %d trying IPv4 Address\r\n", result);
                        result = TCPIP_DNS_Resolve(appData.host, TCPIP_DNS_TYPE_A);
                        appData.queryState = 4;
                        if (result >= 0) {
                            appData.state = APP_TCPIP_WAIT_ON_DNS;
                        } else {
                            SYS_CONSOLE_PRINT("DNS Query returned %d Aborting\r\n", result);
                            appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
                        }
                    }
                }
            }
        }
            break;
        case APP_TCPIP_START_CONNECTION:
        {
            // If we're here it means that we have a proper address.
            appData.dnsComplete = SYS_TMR_SystemCountGet();
            if (appData.queryState == 4) {
                SYS_CONSOLE_PRINT("Starting TCP/IPv4 Connection to : %d.%d.%d.%d port  '%d'\r\n",
                        appData.address.v4Add.v[0],
                        appData.address.v4Add.v[1],
                        appData.address.v4Add.v[2],
                        appData.address.v4Add.v[3],
                        appData.port);
                appData.socket = NET_PRES_SocketOpen(0,
                        NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT,
                        IP_ADDRESS_TYPE_IPV4,
                        appData.port,
                        (NET_PRES_ADDRESS *) & appData.address,
                        NULL);
            } else {
                SYS_CONSOLE_PRINT("Starting TCP/IPv6 Address: %x:%x:%x:%x:%x:%x:%x:%x port '%d'\r\n",
                        htons(appData.address.v6Add.w[0]),
                        htons(appData.address.v6Add.w[1]),
                        htons(appData.address.v6Add.w[2]),
                        htons(appData.address.v6Add.w[3]),
                        htons(appData.address.v6Add.w[4]),
                        htons(appData.address.v6Add.w[5]),
                        htons(appData.address.v6Add.w[6]),
                        htons(appData.address.v6Add.w[7]),
                        appData.port);
                appData.socket = NET_PRES_SocketOpen(0,
                        NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT,
                        IP_ADDRESS_TYPE_IPV6,
                        appData.port,
                        (NET_PRES_ADDRESS *) & appData.address,
                        NULL);
            }
            NET_PRES_SocketWasReset(appData.socket);
            if (appData.socket == INVALID_SOCKET) {
                SYS_CONSOLE_MESSAGE("Could not create socket - aborting\r\n");
                appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            }
            //SYS_CONSOLE_MESSAGE("Starting connection\r\n");
            appData.state = APP_TCPIP_WAIT_FOR_CONNECTION;
        }
        case APP_TCPIP_WAIT_FOR_CONNECTION:
        {
            if (!NET_PRES_SocketIsConnected(appData.socket)) {
                break;
            }
            appData.connectionOpened = SYS_TMR_SystemCountGet();
            if (appData.urlBuffer[4] == 's') {
                SYS_CONSOLE_MESSAGE("Connection Opened: Starting SSL Negotiation\r\n");
                if (!NET_PRES_SocketEncryptSocket(appData.socket)) {
                    SYS_CONSOLE_MESSAGE("SSL Create Connection Failed - Aborting\r\n");
                    appData.state = APP_TCPIP_CLOSE_CONNECTION;
                } else {
                    appData.state = APP_TCPIP_WAIT_FOR_SSL_CONNECT;

                }
            } else {
                SYS_CONSOLE_MESSAGE("Connection Opened: Starting Clear Text Communication\r\n");
                appData.state = APP_TCPIP_SEND_REQUEST;
            }
            break;
        }
        case APP_TCPIP_SEND_REQUEST:
        {
            if (NET_PRES_SocketWriteIsReady(appData.socket, sizeof (networkBuffer), sizeof (networkBuffer)) == 0) {
                break;
            }
            sprintf(networkBuffer, "GET /%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Connection: close\r\n\r\n", appData.path, appData.host);
            NET_PRES_SocketWrite(appData.socket, (uint8_t*) networkBuffer, strlen(networkBuffer));
            appData.clearBytesSent += strlen(networkBuffer);
            appData.rawBytesSent += strlen(networkBuffer);
            appData.state = APP_TCPIP_WAIT_FOR_RESPONSE;
            break;
        }
        case APP_TCPIP_WAIT_FOR_RESPONSE:
        {
            if (NET_PRES_SocketReadIsReady(appData.socket) == 0) {
                if (NET_PRES_SocketWasReset(appData.socket) || NET_PRES_SocketWasDisconnected(appData.socket)) {
                    appData.state = APP_TCPIP_CLOSE_CONNECTION;
                }
                break;
            }
            if (appData.firstRxDataPacket == 0) {
                appData.firstRxDataPacket = SYS_TMR_SystemCountGet();
            }
            appData.lastRxDataPacket = SYS_TMR_SystemCountGet();
            memset(networkBuffer, 0, sizeof (networkBuffer));
            uint16_t res = NET_PRES_SocketRead(appData.socket, (uint8_t*) networkBuffer, sizeof (networkBuffer));

            // Google has stopped obeying the 'connection close' command.  So this is a cheap way to see if we got to the 
            // end of the html page.
            if (strstr(networkBuffer, "</html>") != 0) {
                appData.state = APP_TCPIP_CLOSE_CONNECTION;
            }

            appData.clearBytesReceived += res;
            appData.rawBytesReceived += res;
            break;
        }
        case APP_TCPIP_WAIT_FOR_SSL_CONNECT:
        {
            if (NET_PRES_SocketIsNegotiatingEncryption(appData.socket)) {
                break;
            }
            if (!NET_PRES_SocketIsSecure(appData.socket)) {
                SYS_CONSOLE_MESSAGE("SSL Connection Negotiation Failed - Aborting\r\n");
                appData.state = APP_TCPIP_CLOSE_CONNECTION;
                break;
            }
            SYS_CONSOLE_MESSAGE("SSL Connection Opened: Starting Clear Text Communication\r\n");
            appData.cyaSSLLogEnabled = 0;
            appData.sslNegComplete = SYS_TMR_SystemCountGet();
            ;
            appData.state = APP_TCPIP_SEND_REQUEST_SSL;
            break;
        }
        case APP_TCPIP_SEND_REQUEST_SSL:
        {
            sprintf(networkBuffer, "GET /%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Connection: close\r\n\r\n", appData.path, appData.host);
            int ret;
            ret = NET_PRES_SocketWrite(appData.socket, (uint8_t*) networkBuffer, strlen(networkBuffer));
            appData.clearBytesSent += ret;
            appData.state = APP_TCPIP_WAIT_FOR_RESPONSE_SSL;
            break;
        }
        case APP_TCPIP_WAIT_FOR_RESPONSE_SSL:
        {
            if (NET_PRES_SocketReadIsReady(appData.socket) == 0) {
                if (NET_PRES_SocketWasReset(appData.socket) || NET_PRES_SocketWasDisconnected(appData.socket)) {
                    appData.state = APP_TCPIP_CLOSE_CONNECTION;
                }
                break;
            }
            if (appData.firstRxDataPacket == 0) {
                appData.firstRxDataPacket = SYS_TMR_SystemCountGet();
            }
            appData.lastRxDataPacket = SYS_TMR_SystemCountGet();
            memset(networkBuffer, 0, sizeof (networkBuffer));
            uint16_t res = NET_PRES_SocketRead(appData.socket, (uint8_t*) networkBuffer, sizeof (networkBuffer));

            // Google has stopped obeying the 'connection close' command.  So this is a cheap way to see if we got to the 
            // end of the html page.
            if (strstr(networkBuffer, "</html>") != 0) {
                appData.state = APP_TCPIP_CLOSE_CONNECTION;
            }
            appData.clearBytesReceived += res;
            appData.rawBytesReceived += res;
            break;
        }

        case APP_TCPIP_CLOSE_CONNECTION:
        {
            NET_PRES_SocketClose(appData.socket);
            SYS_CONSOLE_MESSAGE("Connection Closed\r\n");
            appData.state = APP_TCPIP_WAITING_FOR_COMMAND;
            break;
        }

        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

int32_t _APP_ParseUrl(char *uri, char **host, char **path, uint16_t * port) {
    char * pos;
    pos = strstr(uri, "://"); //Check to see if its a proper URL


    if (!pos) {
        return -1;
    }
    *pos = '\0';

    uint32_t protoHash = fnv_32a_hash(uri, strlen(uri));
    //SYS_CONSOLE_PRINT("Protocol Hash '%s' is 0x%x\r\n", uri, protoHash);

    switch (protoHash) {
        case APP_HTTP_FNV:
            *port = 80;
            break;
        case APP_HTTPS_FNV:
            *port = 443;
            break;
    }


    *host = pos + 3; // This is where the host should start
    pos = *host;
    if (**host == '[') {
        // IPv6 address
        pos = strchr(*host, ']');
        *host = *host + 1;
        *pos = '\0';
        pos++;
    } else {
        char * pos2 = strchr(pos, ':');
        if (pos2 != NULL) {
            pos = pos2;
        } else {
            pos2 = strchr(pos, '/');
            if (pos2 == NULL) {
                return -1;
            }
            pos = pos2;
        }
    }

    if (*pos != ':') {
        pos = strchr(pos, '/');
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



/*******************************************************************************
 End of File
 */

