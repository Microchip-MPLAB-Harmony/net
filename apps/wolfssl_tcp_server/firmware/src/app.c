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
#include <wolfssl/ssl.h>
#include <tcpip/src/hash_fnv.h>

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define HTTP_SERVER_PORT 80
#define HTTPS_SERVER_PORT 443

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

static void APP_ClearText_Tasks(IP_ADDRESS_TYPE ipType, NET_PRES_SKT_HANDLE_T * socket, APP_STATES *state);
static void APP_SslText_Tasks(IP_ADDRESS_TYPE ipType, NET_PRES_SKT_HANDLE_T * info, APP_STATES *state);

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
    appData.state = APP_STATE_INIT;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    appData.statect4 = APP_STATE_OPENING_SERVER_SOCKET;
    appData.statect6 = APP_STATE_OPENING_SERVER_SOCKET;
    appData.statessl4 = APP_STATE_OPENING_SERVER_SOCKET;
    appData.statessl6 = APP_STATE_OPENING_SERVER_SOCKET;
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
    int i, nNets;
    TCPIP_NET_HANDLE netH;
    /* Check the application's current state. */
    switch (appData.state) {
        case APP_STATE_INIT:
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
                appData.state = APP_STATE_WAIT_FOR_IP;

            }
            break;

        case APP_STATE_WAIT_FOR_IP:

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
            appData.state = APP_STATE_RUNNING;
            //... etc.
            break;
        case APP_STATE_RUNNING:
        {
            APP_ClearText_Tasks(IP_ADDRESS_TYPE_IPV4, &appData.ct4socket, &appData.statect4);
            APP_SslText_Tasks(IP_ADDRESS_TYPE_IPV4, &appData.et4socket, &appData.statessl4);
#ifdef TCPIP_STACK_USE_IPV6
            APP_ClearText_Tasks(IP_ADDRESS_TYPE_IPV6, &appData.ct6socket, &appData.statect6);
            APP_SslText_Tasks(IP_ADDRESS_TYPE_IPV6, &appData.et6socket, &appData.statessl6);
#endif
        }
            break;
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
char buffer[513];
char successBuffer[] = "HTTP/1.1 200 Success\r\n"
        "Connection: Close\r\n"
        "Content-Type:text/html\r\n"
        "\r\n<html><head><title>Test Page</title></head><body>Nothing here</body></html>\r\n";

void APP_ClearText_Tasks(IP_ADDRESS_TYPE ipType, NET_PRES_SKT_HANDLE_T * socket, APP_STATES *state) {
    NET_PRES_SKT_ERROR_T error;
    switch (*state) {
        case APP_STATE_OPENING_SERVER_SOCKET:
        {
            SYS_CONSOLE_PRINT("Waiting for Client Connection on port: %d\r\n", HTTP_SERVER_PORT);
            *socket = NET_PRES_SocketOpen(0, NET_PRES_SKT_UNENCRYPTED_STREAM_SERVER, ipType, HTTP_SERVER_PORT, 0, &error);
            if (*socket == NET_PRES_INVALID_SOCKET) {
                SYS_CONSOLE_MESSAGE("Couldn't open server socket\r\n");
                break;
            }
            *state = APP_STATE_WAITING_FOR_CONNECTION;
        }
            break;
        case APP_STATE_WAITING_FOR_CONNECTION:
        {
            if (!NET_PRES_SocketIsConnected(*socket)) {
                return;
            }
            *state = APP_STATE_SERVING_RX;
            SYS_CONSOLE_MESSAGE("Received a clear text connection\r\n");
        }
        case APP_STATE_SERVING_RX:
        {
            if (!NET_PRES_SocketIsConnected(*socket)) {
                *state = APP_STATE_CLOSING_CONNECTION;
                SYS_CONSOLE_MESSAGE("Clear Text connection was closed\r\n");
                break;
            }
            if (NET_PRES_SocketReadIsReady(*socket) != 0) {
                int len = NET_PRES_SocketRead(*socket, (uint8_t*) buffer, 512);
                buffer[len] = 0;
                SYS_CONSOLE_PRINT("Received Data: '%s'\r\n", buffer);
                *state = APP_STATE_SERVING_TX;
            }
        }
            break;
        case APP_STATE_SERVING_TX:
        {
            if (!NET_PRES_SocketIsConnected(*socket)) {
                *state = APP_STATE_CLOSING_CONNECTION;
                SYS_CONSOLE_MESSAGE("Clear Text connection was closed\r\n");
                break;
            }
            if (NET_PRES_SocketWriteIsReady(*socket, strlen(successBuffer), strlen(successBuffer)) != 0) {
                NET_PRES_SocketWrite(*socket, (uint8_t*) successBuffer, strlen(successBuffer));
                *state = APP_STATE_CLOSING_CONNECTION;
            }
        }
            break;
        case APP_STATE_CLOSING_CONNECTION:
        {
            NET_PRES_SocketClose(*socket);
            *state = APP_STATE_OPENING_SERVER_SOCKET;
        }
        default:
            break;
    }
}

void APP_SslText_Tasks(IP_ADDRESS_TYPE ipType, NET_PRES_SKT_HANDLE_T * socket, APP_STATES *state) {
    NET_PRES_SKT_ERROR_T error;
    switch (*state) {
        case APP_STATE_OPENING_SERVER_SOCKET:
        {
            SYS_CONSOLE_PRINT("Waiting for Client Connection on port: %d\r\n", HTTPS_SERVER_PORT);
            *socket = NET_PRES_SocketOpen(0, NET_PRES_SKT_ENCRYPTED_STREAM_SERVER, ipType, HTTPS_SERVER_PORT, 0, &error);
            if (*socket == NET_PRES_INVALID_SOCKET) {
                SYS_CONSOLE_MESSAGE("Couldn't open server socket\r\n");
                break;
            }
            *state = APP_STATE_WAITING_FOR_CONNECTION;
        }
            break;
        case APP_STATE_WAITING_FOR_CONNECTION:
        {
            if (!NET_PRES_SocketIsConnected(*socket)) {
                return;
            }
            *state = APP_STATE_NEGOTIATING_SSL;
            SYS_CONSOLE_MESSAGE("Received a clear ssl connection\r\n");
        }
        case APP_STATE_NEGOTIATING_SSL:
        {
            if (NET_PRES_SocketIsNegotiatingEncryption(*socket)) {
                break;
            }
            if (!NET_PRES_SocketIsSecure(*socket)) {
                SYS_CONSOLE_MESSAGE("SSL Connection Negotiation Failed - Aborting\r\n");
                *state = APP_STATE_CLOSING_CONNECTION;
                break;
            }
            SYS_CONSOLE_MESSAGE("SSL Connection Opened: Starting Clear Text Communication\r\n");
            *state = APP_STATE_SERVING_RX;
            break;
        }
        case APP_STATE_SERVING_RX:
        {
            if (!NET_PRES_SocketIsConnected(*socket)) {
                *state = APP_STATE_CLOSING_CONNECTION;
                SYS_CONSOLE_MESSAGE("Clear Text connection was closed\r\n");
                break;
            }
            if (NET_PRES_SocketReadIsReady(*socket) != 0) {
                int len = NET_PRES_SocketRead(*socket, (uint8_t*) buffer, 512);
                buffer[len] = 0;
                SYS_CONSOLE_PRINT("Received Data: '%s'\r\n", buffer);
                *state = APP_STATE_SERVING_TX;
            }
        }
            break;
        case APP_STATE_SERVING_TX:
        {
            if (!NET_PRES_SocketIsConnected(*socket)) {
                *state = APP_STATE_CLOSING_CONNECTION;
                SYS_CONSOLE_MESSAGE("Clear Text connection was closed\r\n");
                break;
            }
            if (NET_PRES_SocketWriteIsReady(*socket, strlen(successBuffer), strlen(successBuffer)) != 0) {
                NET_PRES_SocketWrite(*socket, (uint8_t*) successBuffer, strlen(successBuffer));
                *state = APP_STATE_CLOSING_CONNECTION;
            }
        }
            break;
        case APP_STATE_CLOSING_CONNECTION:
        {
            NET_PRES_SocketClose(*socket);
            *state = APP_STATE_OPENING_SERVER_SOCKET;
        }
        default:
            break;
    }

}



/*******************************************************************************
 End of File
 */

