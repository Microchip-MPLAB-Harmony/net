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
#include "tcpip/berkeley_api.h"
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

static void _APP_Commands_StartService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_RelayHost(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_RelayPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_Ipv4ServerPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#ifdef TCPIP_STACK_USE_IPV6
static void _APP_Commands_Ipv6ServerPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
static void _APP_Commands_StopService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_CurrentSetup(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void _APP_Commands_RelayClientHost(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_RelayClientSetup(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _APP_Commands_RelayClientStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void _APP_ServerTasks();
static void _APP_ClientTasks();
static void _APP_RelayServerTasks();
static void _APP_RelayClientTasks();

static const SYS_CMD_DESCRIPTOR appCmdTbl[] ={
    {"start", _APP_Commands_StartService, ": Start the Relay Service."},
    {"relayhost", _APP_Commands_RelayHost, ": Sets the hostname of the host to send relay packets to"},
    {"relayport", _APP_Commands_RelayPort, ": Sets the port to send relay ports to."},
    {"ipv4port", _APP_Commands_Ipv4ServerPort, ": Sets the IPv4 server port to accept packets on"},
#ifdef TCPIP_STACK_USE_IPV6
    {"ipv6port", _APP_Commands_Ipv6ServerPort, ": Sets the IPv6 server port to accept packets on"},
#endif
    {"stop", _APP_Commands_StopService, ": Stop the Relay Service."},
    {"current", _APP_Commands_CurrentSetup, ": Displays the current setup."},
    {"relayclientstart", _APP_Commands_RelayClientStart, ": Start the relay client"},
    {"relayclienthost", _APP_Commands_RelayClientHost, ": Sets the hostname of the host to send relay client packets to"},
    {"relayclientiter", _APP_Commands_RelayClientSetup, ": number of packets for the client to send."},
};

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
    if (!SYS_CMD_ADDGRP(appCmdTbl, sizeof (appCmdTbl) / sizeof (*appCmdTbl), "app", ": app commands")) {
        SYS_ERROR(SYS_ERROR_ERROR, "Failed to create TCPIP Commands\r\n");
        return;
    }
    strcpy(appData.relayHost, "192.168.119.2");
    strcpy(appData.relayClientHost, "192.168.119.2");
    appData.relayPort = 12345;
    appData.ipv4ServerPort = 9876;
    appData.serverSocket4 = SOCKET_ERROR;
    appData.clientSocket = SOCKET_ERROR;
    appData.relayClientSocket = SOCKET_ERROR;
    appData.relayServerSocket4 = SOCKET_ERROR;
    appData.reportInterval = 10;
    appData.relayClientCounter = 1000;
#ifdef TCPIP_STACK_USE_IPV6
    appData.ipv6ServerPort = 9876;
    appData.serverSocket6 = SOCKET_ERROR;
    appData.relayServerSocket6 = SOCKET_ERROR;
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
    int i, nNets;
    TCPIP_NET_HANDLE netH;
    const char *netName, *netBiosName;
    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat < 0) { // some error occurred
                SYS_CONSOLE_MESSAGE(" APP: TCP/IP stack initialization failed!\r\n");
                appData.state = APP_STATE_ERROR;
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
        }
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
            appData.state = APP_STATE_READY_TO_START;
            //... etc.
            break;
        case APP_STATE_READY_TO_START:
        {
            if (appData.serviceStarted) {
                appData.clientState = APP_STATE_DNS_LOOKUP;
                appData.serverState = APP_STATE_SERVER_CREATE_SOCKET;
                appData.relayServerState = APP_STATE_RELAY_SERVER_CREATE_SOCKET;
                appData.state = APP_STATE_STARTED;
                break;
            }
        }
        case APP_STATE_STARTED:
        {
            if (appData.serviceStarted) {
                _APP_ClientTasks();
                _APP_ServerTasks();
                _APP_RelayClientTasks();
                _APP_RelayServerTasks();
            } else {
                if (appData.serverSocket4 != SOCKET_ERROR) {
                    closesocket(appData.serverSocket4);
                    appData.serverSocket4 = SOCKET_ERROR;
                }
                if (appData.clientSocket != SOCKET_ERROR) {
                    closesocket(appData.clientSocket);
                    appData.clientSocket = SOCKET_ERROR;
                }
                if (appData.relayServerSocket4 != SOCKET_ERROR) {
                    closesocket(appData.relayServerSocket4);
                    appData.relayServerSocket4 = SOCKET_ERROR;
                }
                if (appData.relayClientSocket != SOCKET_ERROR) {
                    closesocket(appData.relayClientSocket);
                    appData.relayClientSocket = SOCKET_ERROR;
                }
#ifdef TCPIP_STACK_USE_IPV6
                if (appData.serverSocket6 != SOCKET_ERROR) {
                    closesocket(appData.serverSocket6);
                    appData.serverSocket6 = SOCKET_ERROR;
                }
                if (appData.relayServerSocket6 != SOCKET_ERROR) {
                    closesocket(appData.relayServerSocket6);
                    appData.relayServerSocket6 = SOCKET_ERROR;
                }
#endif
                appData.clientState = APP_STATE_CLIENT_CLOSED;
                appData.serverState = APP_STATE_SERVER_CLOSED;
                appData.relayClientState = APP_STATE_RELAY_CLIENT_CLOSED;
                appData.relayServerState = APP_STATE_RELAY_SERVER_CLOSED;
                appData.state = APP_STATE_READY_TO_START;

                appData.serverLastReport = 0;
                appData.clientLastReport = 0;
                appData.relayServerLastReport = 0;
                appData.relayClientLastReport = 0;

                appData.serverTotalPackets = 0;
                appData.serverPacketsSinceLastReport = 0;
                appData.serverTotalSize = 0;
                appData.serverSizeSinceLastReport = 0;

                appData.clientTotalPackets = 0;
                appData.clientPacketsSinceLastReport = 0;
                appData.clientTotalSize = 0;
                appData.clientSizeSinceLastReport = 0;

                appData.relayServerTotalPackets = 0;
                appData.relayServerPacketsSinceLastReport = 0;
                appData.relayServerTotalSize = 0;
                appData.relayServerSizeSinceLastReport = 0;

                appData.relayClientTotalPackets = 0;
                appData.relayClientPacketsSinceLastReport = 0;
                appData.relayClientTotalSize = 0;
                appData.relayClientSizeSinceLastReport = 0;

                appData.relayClientNumberOfPackets = 0;
                appData.relayServerPacketCount = 0;
                appData.relayServerPacketsDropped = 0;

                break;
            }
        }


            /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

void _APP_Commands_StartService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    appData.serviceStarted = true;
}

void _APP_Commands_RelayHost(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if (argc != 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: relayhost <host name>\r\n");
        return;
    }
    strcpy(appData.relayHost, argv[1]);
}

void _APP_Commands_RelayPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if (argc != 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: relayport <port number>\r\n");
        return;
    }
    appData.relayPort = atoi(argv[1]);
}

void _APP_Commands_Ipv4ServerPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if (argc != 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ipv4port <server port number>\r\n");
        return;
    }
    appData.ipv4ServerPort = atoi(argv[1]);
}
#ifdef TCPIP_STACK_USE_IPV6

void _APP_Commands_Ipv6ServerPort(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if (argc != 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ipv6port <server port number>\r\n");
        return;
    }
    appData.ipv6ServerPort = atoi(argv[1]);
}
#endif

void _APP_Commands_StopService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    appData.serviceStarted = false;
}

void _APP_Commands_CurrentSetup(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Current Setup: %s\r\n", appData.serviceStarted ? "started" : "stopped");
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tRelay Host '%s'\r\n", appData.relayHost);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tRelay Port %d\r\n", appData.relayPort);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tIPv4 Server  Port %d\r\n", appData.ipv4ServerPort);
#ifdef TCPIP_STACK_USE_IPV6
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tIPv6 Server  Port %d\r\n", appData.ipv6ServerPort);
#endif
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tRelay Client Host '%s'\r\n", appData.relayClientHost);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tRelay Client Host Iterations '%d'\r\n", appData.relayClientCounter);
}

void _APP_Commands_RelayClientHost(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if (argc != 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: relayhost <host name>\r\n");
        return;
    }
    strcpy(appData.relayClientHost, argv[1]);
}

void _APP_Commands_RelayClientSetup(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if (argc != 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: relayiter <number of packets>\r\n");
        return;
    }
    appData.relayClientCounter = atoi(argv[1]);
}

uint8_t dataBuffer[1500];
uint8_t dataBuffer2[1500];

void _APP_ServerTasks() {
    if (appData.serverLastReport != 0) {
        if (SYS_TMR_SystemCountGet() > appData.serverLastReport) {
            appData.serverLastReport = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() * appData.reportInterval);
            SYS_CONSOLE_PRINT("\r\nServer Report interval %lld:\r\n", appData.reportInterval);
            SYS_CONSOLE_MESSAGE("=================================\r\n");
            SYS_CONSOLE_PRINT("Total Bytes: %d\r\n", appData.serverTotalSize);
            SYS_CONSOLE_PRINT("Total Packets: %d\r\n", appData.serverTotalPackets);
            SYS_CONSOLE_PRINT("Bytes Since Last Report: %d\r\n", appData.serverSizeSinceLastReport);
            SYS_CONSOLE_PRINT("Packets Since Last Report: %d\r\n", appData.serverPacketsSinceLastReport);
            appData.serverSizeSinceLastReport = 0;
            appData.serverPacketsSinceLastReport = 0;
        }
    }
    switch (appData.serverState) {
        case APP_STATE_SERVER_CLOSED:
            break;
        case APP_STATE_SERVER_CREATE_SOCKET:
        {
            uint16_t bufSize = 1470;
            appData.serverSocket4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (appData.serverSocket4 == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not create IPv4 Server Socket error %d\r\n", errno);
                break;
            }
            setsockopt(appData.serverSocket4, SOL_SOCKET, SO_SNDBUF, (const uint8_t*) &bufSize, 2);
#ifdef TCPIP_STACK_USE_IPV6
            appData.serverSocket6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
            if (appData.serverSocket6 == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not create IPv6 Server Socket error %d\r\n", errno);
                break;
            }
            setsockopt(appData.serverSocket6, SOL_SOCKET, SO_SNDBUF, (const uint8_t*) &bufSize, 2);
#endif
            appData.serverState = APP_STATE_SERVER_BIND;
        }
            break;
        case APP_STATE_SERVER_BIND:
        {
            struct sockaddr_in6 addr;
            SOCKADDR_IN *ptr = (SOCKADDR_IN*) (&addr);
            int addrlen = sizeof (struct sockaddr_in6);
            memset(ptr, 0, addrlen);
            ptr->sin_family = AF_INET;
            ptr->sin_port = appData.ipv4ServerPort;
            ptr->sin_addr.S_un.S_addr = IP_ADDR_ANY;
            if (bind(appData.serverSocket4, (struct sockaddr*) &addr, addrlen) == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not bind IPv4 Server Socket error %d\r\n", errno);
                break;
            }
            SYS_CONSOLE_PRINT("IPv4 Server Ready on Port %d\r\n", ptr->sin_port);
#ifdef TCPIP_STACK_USE_IPV6
            memset(ptr, 0, addrlen);
            addr.sin6_family = AF_INET6;
            addr.sin6_port = appData.ipv6ServerPort;
            ptr->sin_addr.S_un.S_addr = IP_ADDR_ANY;
            IPV6_ADDR_HANDLE prevHandle, nextHandle;
            prevHandle = 0;
            TCPIP_NET_HANDLE netH;
            IPV6_ADDR_STRUCT currIpv6Add;
            netH = TCPIP_STACK_IndexToNet(0);
            do {
                nextHandle = TCPIP_STACK_NetIPv6AddressGet(netH, IPV6_ADDR_TYPE_UNICAST, &currIpv6Add, prevHandle);
                if (nextHandle) {
                    if (currIpv6Add.flags.scope == IPV6_ADDR_SCOPE_GLOBAL) {
                        memcpy(&(addr.sin6_addr), &(currIpv6Add.address), sizeof (IPV6_ADDR));
                        break;
                    }
                    prevHandle = nextHandle;
                }
            } while (nextHandle != 0);

            if (bind(appData.serverSocket6, (struct sockaddr*) &addr, addrlen) == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not bind IPv6 Server Socket error %d\r\n", errno);
                break;
            }
            char addrBuff[44];
            addrBuff[0] = 0;
            TCPIP_Helper_IPv6AddressToString(&currIpv6Add.address, addrBuff, sizeof (addrBuff));
            SYS_CONSOLE_PRINT("IPv6 Server read on address %s Port %d\r\n", addrBuff, addr.sin6_port);
#endif
            appData.serverState = APP_STATE_WAITING_FOR_CONNECTIONS;

        }
            break;
        case APP_STATE_WAITING_FOR_CONNECTIONS:
        {
            struct sockaddr_in6 clientaddr;
            int len = sizeof (struct sockaddr_in);

            // Figure out how many bytes have been received and how many we can transmit.
            int i = recvfrom(appData.serverSocket4, (char*) dataBuffer, sizeof (dataBuffer), 0, (struct sockaddr *) &clientaddr, &len);

#ifdef TCPIP_STACK_USE_IPV6
            if (i <= 0) {
                i = recvfrom(appData.serverSocket6, (char*) dataBuffer, sizeof (dataBuffer), 0, (struct sockaddr *) &clientaddr, &len);
            }
#endif

            if (i <= 0) {
                break;
            }
            //SYS_CONSOLE_PRINT("Received Data %d\r\n", i);

            if (appData.serverLastReport == 0) {
                appData.serverLastReport = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() * appData.reportInterval);
            }
            appData.serverTotalSize += i;
            appData.serverTotalPackets++;
            appData.serverSizeSinceLastReport += i;
            appData.serverPacketsSinceLastReport++;
            if (appData.clientState == APP_STATE_READY) {
                i = sendto(appData.clientSocket, (const char *) dataBuffer, i, 0, (const struct sockaddr*) &appData.clientAddr, sizeof (struct sockaddr_in6));
            }

        }
            break;
    }

}

void _APP_ClientTasks() {
    switch (appData.clientState) {
        case APP_STATE_CLIENT_CLOSED:
            break;
        case APP_STATE_DNS_LOOKUP:
        {
            int res = getaddrinfo(appData.relayHost, NULL, &appData.hints, &appData.addinfo);
            if (res == 0) {
                memcpy(&appData.clientAddr, appData.addinfo[0].ai_addr, appData.addinfo[0].ai_addrlen);
                freeaddrinfo(appData.addinfo);
                if (appData.clientAddr.sin6_family == AF_INET) {
                    struct sockaddr_in * ptr = (struct sockaddr_in *) &appData.clientAddr;
                    ptr->sin_port = appData.relayPort;
                } else {
                    appData.clientAddr.sin6_port = appData.relayPort;
                }
                appData.clientState = APP_STATE_CREATE_SOCKET;
            }
        }
            break;
        case APP_STATE_CREATE_SOCKET:
        {
            uint16_t bufSize = 1470;
            appData.clientSocket = socket(appData.clientAddr.sin6_family, SOCK_DGRAM, IPPROTO_UDP);
            setsockopt(appData.clientSocket, SOL_SOCKET, SO_SNDBUF, (const uint8_t*) &bufSize, 2);
            appData.clientState = APP_STATE_READY;
            SYS_CONSOLE_MESSAGE("Client Ready\r\n");

        }
            break;
        case APP_STATE_READY:
            break;
        default:
            break;
    }
}

static void _APP_RelayServerTasks() {
    if (appData.relayServerLastReport != 0) {
        if (SYS_TMR_SystemCountGet() > appData.relayServerLastReport) {
            appData.relayServerLastReport = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() * appData.reportInterval);
            SYS_CONSOLE_PRINT("\r\nRelay Server Report interval %lld:\r\n", appData.reportInterval);
            SYS_CONSOLE_MESSAGE("=================================\r\n");
            SYS_CONSOLE_PRINT("Total Bytes: %d\r\n", appData.relayServerTotalSize);
            SYS_CONSOLE_PRINT("Total Packets: %d\r\n", appData.relayServerTotalPackets);
            SYS_CONSOLE_PRINT("Total Packets Dropped: %d\r\n", appData.relayServerPacketsDropped);
            SYS_CONSOLE_PRINT("Bytes Since Last Report: %d\r\n", appData.relayServerSizeSinceLastReport);
            SYS_CONSOLE_PRINT("Packets Since Last Report: %d\r\n", appData.relayServerPacketsSinceLastReport);
            SYS_CONSOLE_PRINT("Packets Dropped Since Last Report: %d\r\n", appData.relayServerPacketsDroppedSinceLastReport);
            appData.relayServerSizeSinceLastReport = 0;
            appData.relayServerPacketsSinceLastReport = 0;
            appData.relayServerPacketsDroppedSinceLastReport = 0;
        }
    }
    switch (appData.relayServerState) {
        case APP_STATE_RELAY_SERVER_CLOSED:
            break;
        case APP_STATE_RELAY_SERVER_CREATE_SOCKET:
        {
            uint16_t bufSize = 1470;
            appData.relayServerSocket4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (appData.relayServerSocket4 == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not create IPv4 Relay Server Socket error %d\r\n", errno);
                break;
            }
            setsockopt(appData.relayServerSocket4, SOL_SOCKET, SO_SNDBUF, (const uint8_t*) &bufSize, 2);
#ifdef TCPIP_STACK_USE_IPV6
            appData.relayServerSocket6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
            if (appData.relayServerSocket6 == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not create IPv6 Server Socket error %d\r\n", errno);
                break;
            }
            setsockopt(appData.relayServerSocket6, SOL_SOCKET, SO_SNDBUF, (const uint8_t*) &bufSize, 2);
#endif
            appData.relayServerState = APP_STATE_RELAY_SERVER_BIND;
        }
            break;
        case APP_STATE_RELAY_SERVER_BIND:
        {
            struct sockaddr_in6 addr;
            SOCKADDR_IN *ptr = (SOCKADDR_IN*) (&addr);
            int addrlen = sizeof (struct sockaddr_in6);
            memset(ptr, 0, addrlen);
            ptr->sin_family = AF_INET;
            ptr->sin_port = appData.relayPort;
            ptr->sin_addr.S_un.S_addr = IP_ADDR_ANY;
            if (bind(appData.relayServerSocket4, (struct sockaddr*) &addr, addrlen) == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not bind IPv4 Relay Server Socket error %d\r\n", errno);
                break;
            }
            SYS_CONSOLE_PRINT("IPv4 Relay Server Ready on Port %d\r\n", ptr->sin_port);
#ifdef TCPIP_STACK_USE_IPV6
            memset(ptr, 0, addrlen);
            addr.sin6_family = AF_INET6;
            addr.sin6_port = appData.relayPort;
            ptr->sin_addr.S_un.S_addr = IP_ADDR_ANY;
            IPV6_ADDR_HANDLE prevHandle, nextHandle;
            prevHandle = 0;
            TCPIP_NET_HANDLE netH;
            IPV6_ADDR_STRUCT currIpv6Add;
            netH = TCPIP_STACK_IndexToNet(0);
            do {
                nextHandle = TCPIP_STACK_NetIPv6AddressGet(netH, IPV6_ADDR_TYPE_UNICAST, &currIpv6Add, prevHandle);
                if (nextHandle) {
                    if (currIpv6Add.flags.scope == IPV6_ADDR_SCOPE_GLOBAL) {
                        memcpy(&(addr.sin6_addr), &(currIpv6Add.address), sizeof (IPV6_ADDR));
                        break;
                    }
                    prevHandle = nextHandle;
                }
            } while (nextHandle != 0);

            if (bind(appData.relayServerSocket6, (struct sockaddr*) &addr, addrlen) == SOCKET_ERROR) {
                SYS_CONSOLE_PRINT("Could not bind IPv6 Relay Server Socket error %d\r\n", errno);
                break;
            }
            char addrBuff[44];
            addrBuff[0] = 0;
            TCPIP_Helper_IPv6AddressToString(&currIpv6Add.address, addrBuff, sizeof (addrBuff));
            SYS_CONSOLE_PRINT("IPv6 Relay Server read on address %s Port %d\r\n", addrBuff, addr.sin6_port);
#endif
            appData.relayServerState = APP_STATE_RELAY_SERVER_SERVING;

        }
            break;
        case APP_STATE_RELAY_SERVER_SERVING:
        {
            struct sockaddr_in6 clientaddr;
            int len = sizeof (struct sockaddr_in);

            // Figure out how many bytes have been received and how many we can transmit.
            int i = recvfrom(appData.relayServerSocket4, (char*) dataBuffer2, sizeof (dataBuffer2), 0, (struct sockaddr *) &clientaddr, &len);

#ifdef TCPIP_STACK_USE_IPV6
            if (i <= 0) {
                i = recvfrom(appData.relayServerSocket6, (char*) dataBuffer2, sizeof (dataBuffer2), 0, (struct sockaddr *) &clientaddr, &len);
            }
#endif

            if (i <= 0) {
                break;
            }
            //SYS_CONSOLE_PRINT("Relay Server Received Data %d\r\n", i);
            uint32_t num = atoi((char*) dataBuffer2);
            if (num > appData.relayServerPacketCount + 1) {
                appData.relayServerPacketsDropped++;
                appData.relayServerPacketsDroppedSinceLastReport++;
            }
            appData.relayServerPacketCount = num;
            if (appData.relayServerLastReport == 0) {
                appData.relayServerLastReport = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() * appData.reportInterval);
            }
            appData.relayServerTotalSize += i;
            appData.relayServerTotalPackets++;
            appData.relayServerSizeSinceLastReport += i;
            appData.relayServerPacketsSinceLastReport++;

        }
            break;
        default:
            break;
    }


}

char cannedData[] = "                 This file contains the source code for the MPLAB Harmony application.  It implements the logic of the application's state machine and it may call API routines of other MPLAB Harmony modules in the system, such as drivers, system services, and middleware.  However, it does not call any of the system interfaces (such as the \"Initialize\" and \"Tasks\" functions) of any of the modules in the system or make any assumptions about when those functions are called.  That is the responsibility of the configuration-specific system files.  Microchip licenses to you the right to use, modify, copy and distribute Software only when embedded on a Microchip microcontroller or digital signal controller that is integrated into your product or third party product (pursuant to the sublicense terms in the accompanying license agreement).  You should refer to the license agreement accompanying this Software for additional information regarding your rights and obligations. SOFTWARE AND DOCUMENTATION ARE PROVIDED \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY";

static void _APP_RelayClientTasks() {
    switch (appData.relayClientState) {
        case APP_STATE_RELAY_CLIENT_CLOSED:
            break;
        case APP_STATE_RELAY_CLIENT_DNS_LOOKUP:
        {
            int res = getaddrinfo(appData.relayClientHost, NULL, &appData.hints, &appData.relayaddinfo);
            if (res == 0) {
                memcpy(&appData.relayClientAddr, appData.relayaddinfo[0].ai_addr, appData.relayaddinfo[0].ai_addrlen);
                freeaddrinfo(appData.relayaddinfo);
                if (appData.clientAddr.sin6_family == AF_INET) {
                    struct sockaddr_in * ptr = (struct sockaddr_in *) &appData.relayClientAddr;
                    ptr->sin_port = appData.ipv4ServerPort;
                } else {
                    appData.relayClientAddr.sin6_port = appData.ipv6ServerPort;
                }
                appData.relayClientState = APP_STATE_RELAY_CLIENT_CREATE_SOCKET;
            }
        }
            break;
        case APP_STATE_RELAY_CLIENT_CREATE_SOCKET:
        {
            uint16_t bufSize = 1470;
            appData.relayClientSocket = socket(appData.relayClientAddr.sin6_family, SOCK_DGRAM, IPPROTO_UDP);
            setsockopt(appData.relayClientSocket, SOL_SOCKET, SO_SNDBUF, (const uint8_t*) &bufSize, 2);
            appData.relayClientState = APP_STATE_RELAY_CLIENT_RUNNING;
            SYS_CONSOLE_MESSAGE("Relay Client Starting\r\n");

        }
            break;
        case APP_STATE_RELAY_CLIENT_RUNNING:
        {
#if !defined(__PIC32C__) && !defined(__SAMA5D2)
            itoa(cannedData, appData.relayClientNumberOfPackets, 10);
#else
            itoa(appData.relayClientNumberOfPackets, cannedData, 10);
#endif            
            int i = sendto(appData.relayClientSocket, (const char *) cannedData, sizeof (cannedData), 0, (const struct sockaddr*) &appData.relayClientAddr, sizeof (struct sockaddr_in6));
            if (i < 0) {
                break;
            }
            appData.relayClientNumberOfPackets++;
            if (appData.relayClientNumberOfPackets == appData.relayClientCounter) {
                closesocket(appData.relayClientSocket);
                appData.relayClientSocket = SOCKET_ERROR;
                appData.relayClientState = APP_STATE_RELAY_CLIENT_CLOSED;
                appData.relayClientNumberOfPackets = 0;
                SYS_CONSOLE_MESSAGE("Relay Client Done\r\n");
            }
        }
            break;
    }

}

static void _APP_Commands_RelayClientStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    appData.relayClientState = APP_STATE_RELAY_CLIENT_DNS_LOOKUP;
}


/*******************************************************************************
 End of File
 */
