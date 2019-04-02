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
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/
#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application States

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
	/* Application's state machine's initial state. */
	APP_STATE_INIT=0,

	/* TODO: Define states used by the application state machine. */
        APP_STATE_WAIT_FOR_IP,
        APP_STATE_READY_TO_START,
        APP_STATE_STARTED,
        APP_STATE_ERROR,

} APP_STATES;

typedef enum
{
    APP_STATE_SERVER_CLOSED,
    APP_STATE_SERVER_CREATE_SOCKET,
    APP_STATE_SERVER_BIND,
    APP_STATE_WAITING_FOR_CONNECTIONS,
} APP_SERVER_STATES;

typedef enum
{
    APP_STATE_CLIENT_CLOSED,
    APP_STATE_DNS_LOOKUP,
    APP_STATE_WAIT_FOR_DNS,
    APP_STATE_CREATE_SOCKET,
    APP_STATE_READY
} APP_CLIENT_STATES;

typedef enum
{
    APP_STATE_RELAY_CLIENT_CLOSED,
    APP_STATE_RELAY_CLIENT_DNS_LOOKUP,
    APP_STATE_RELAY_CLIENT_CREATE_SOCKET,
    APP_STATE_RELAY_CLIENT_RUNNING,
}APP_RELAY_CLIENT_STATES;


typedef enum
{
    APP_STATE_RELAY_SERVER_CLOSED,
    APP_STATE_RELAY_SERVER_CREATE_SOCKET,
    APP_STATE_RELAY_SERVER_BIND,
    APP_STATE_RELAY_SERVER_SERVING
}APP_RELAY_SERVER_STATES;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;
    APP_SERVER_STATES serverState;
    APP_CLIENT_STATES clientState;
    APP_RELAY_CLIENT_STATES relayClientState;
    APP_RELAY_SERVER_STATES relayServerState;

    /* TODO: Define any additional data used by the application. */

    bool serviceStarted;
    SOCKET serverSocket4;
    SOCKET serverSocket6;
    SOCKET clientSocket;
    SOCKET relayClientSocket;
    SOCKET relayServerSocket4;
    SOCKET relayServerSocket6;

    char relayHost[256];
    uint16_t relayPort;
    uint16_t ipv4ServerPort;
    uint16_t ipv6ServerPort;
    struct sockaddr_in6 clientAddr;
    struct addrinfo hints;
    struct addrinfo *addinfo;

    char relayClientHost[256];
    uint16_t relayClientPort;
    struct sockaddr_in6 relayClientAddr;
    struct addrinfo *relayaddinfo;

    uint64_t serverLastReport;
    uint64_t clientLastReport;
    uint64_t relayServerLastReport;
    uint64_t relayClientLastReport;
    uint64_t reportInterval;

    uint32_t serverTotalPackets;
    uint32_t serverPacketsSinceLastReport;
    uint32_t serverTotalSize;
    uint32_t serverSizeSinceLastReport;

    uint32_t clientTotalPackets;
    uint32_t clientPacketsSinceLastReport;
    uint32_t clientTotalSize;
    uint32_t clientSizeSinceLastReport;

    uint32_t relayServerTotalPackets;
    uint32_t relayServerPacketsSinceLastReport;
    uint32_t relayServerTotalSize;
    uint32_t relayServerSizeSinceLastReport;

    uint32_t relayClientTotalPackets;
    uint32_t relayClientPacketsSinceLastReport;
    uint32_t relayClientTotalSize;
    uint32_t relayClientSizeSinceLastReport;

    uint32_t relayClientCounter;
    uint32_t relayClientNumberOfPackets;

    uint32_t relayServerPacketsDropped;
    uint32_t relayServerPacketsDroppedSinceLastReport;
    uint32_t relayServerPacketCount;

} APP_DATA;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

	
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the 
    application in its initial state and prepares it to run so that its 
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );


#endif /* _APP_H */
/*******************************************************************************
 End of File
 */

