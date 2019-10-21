/**************************************************************************
  Company:
    Microchip Technology Inc.
	
  File Name:
    telnet.h
	
  Summary:
    Telnet provides bidirectional, interactive communication between two
    nodes on the Internet or on a Local Area Network.
	
  Description:
    Telnet Server Module for Microchip TCP/IP Stack. The telnet protocol is
    explained in RFC 854.                                                  
  **************************************************************************/
//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/








//DOM-IGNORE-END

#ifndef __TELNET_H
#define __TELNET_H

#include <stdint.h>
#include <stdbool.h>

#include "tcpip/tcp.h"
#include "net_pres/pres/net_pres.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
/*
  Enumeration:
    TCPIP_TELNET_STATE

  Summary:
    Telnet connection state.

  Description:
    Enumeration describing the possible states of a telnet connection.
*/

typedef	enum
{
    TCPIP_TELNET_IDLE = 0,
    TCPIP_TELNET_GET_LOGIN,
    TCPIP_TELNET_GET_PASSWORD,
    TCPIP_TELNET_AUTHENTICATED,
    TCPIP_TELNET_CONNECTED
} TCPIP_TELNET_STATE;

// *****************************************************************************
/*
  Structure:
    TCPIP_TELNET_CONN_INFO

  Summary:
    telnet connection information.

  Description:
    Gets information about a telnet connection.
*/
typedef struct
{
    uint16_t            connIx;         // telnet connection index: [0, TCPIP_TELNET_MODULE_CONFIG::nConnections - 1]
    uint16_t            state;          // TCPIP_TELNET_STATE: Current telnet connection state
    int16_t             presSkt;        // NET_PRES_SKT_HANDLE_T: connection presentation socket
    int16_t             tcpSkt;         // TCP_SOCKET: underlying transport socket
    TCP_SOCKET_INFO     tcpInfo;        // transport socket information
} TCPIP_TELNET_CONN_INFO;


// *****************************************************************************
/*
  Type:
    TCPIP_TELNET_HANDLE

  Summary:
    Defines a telnet handle.

  Description:
    Definition of an processing handle used for
    registration of callbacks with the telnet module

*/
typedef const void* TCPIP_TELNET_HANDLE;

// *****************************************************************************
/* telnet authentication callback

  Function:
    bool <FunctionName>(const char* user, const char* password, const TCPIP_TELNET_CONN_INFO* pInfo, const void* hParam);

  Summary:
    Pointer to a function (handler) that will get called to process telnet authentication

  Description:
    Pointer to a function that will be called by the telnet module
    when a login operation is ongoing and user authentication
    needs to be performed

  Precondition:
    None

  Parameters:
    user        - the user name entered as part of the login process
    password    - the password entered as part of the login process
    pInfo       - pointer to an TCPIP_TELNET_CONN_INFO carying specific connection information
                  to help in the authentication process
    hParam      - parameter that was passed at the time of the handler registration                  

  Returns:
    true - if the authentication is successful and the telnet connection should be allowed
    false - if the authentication failed and the connection should be aborted

  Remarks:
    The handler is called in the telnet context.
    The handler should be kept as short as possible as it shouldn't block

    This is the PREFERRED method of authenticating the telnet connection.

    Until the moment the authentication handler is registered
    any login attempts will fail!

    The pInfo will be NULL if the telnet module is initialized with
    TCPIP_TELNET_AUTHENTICATION_INFO == true
    
 */
typedef bool(*TCPIP_TELNET_AUTH_HANDLER)(const char* user, const char* password, const TCPIP_TELNET_CONN_INFO* pInfo, const void* hParam);

// *****************************************************************************
/*
  Enumeration:
    TCPIP_TELNET_FLAGS

  Summary:
    telnet configuration flags.

  Description:
    Flags for the configuration of the telnet module

  Remarks:
    Only 16 bits are supported

*/
typedef enum
{
    TCPIP_TELNET_FLAG_NONE                  = 0x0000,   // no flag set

    TCPIP_TELNET_FLAG_NO_DELAY              = 0x0001,   // create the telnet sockets with NO_DELAY option

    TCPIP_TELNET_FLAG_PASS_CONTROL_CHARS    = 0x0002,   // pass the telnet commands, options, controls, etc.
                                                        // within the messages to the console
                                                        // by default they are removed and only printable
                                                        // characters are sent to the console
                                                        // Enable only for debug or special purposes.



}TCPIP_TELNET_FLAGS;


// *****************************************************************************
/*
  Structure:
    TCPIP_TELNET_MODULE_CONFIG

  Summary:
    telnet module run time configuration/initialization data.

  Description:
    telnet module configuration/initialization
*/
typedef struct
{
    uint16_t                    nConnections;   // number of simultaneous telnet connections allowed
    uint16_t                    sktTxBuffSize;  // size of TX buffer for the associated socket; leave 0 for default
    uint16_t                    sktRxBuffSize;  // size of RX buffer for the associated socket; leave 0 for default
    uint16_t                    listenPort;     // telnet listening port: 23, 992, etc.
    uint16_t                    configFlags;    // TCPIP_TELNET_FLAGS configuration flags

}TCPIP_TELNET_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Telnet API Functions
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    TCPIP_TELNET_HANDLE     TCPIP_TELNET_AuthenticationRegister(TCPIP_TELNET_AUTH_HANDLER authHandler, const void* handlerParam);

  Summary:
    Sets a new telnet authentication handler.

  Description:
    This function registers a new authentication handler.
    telnet module will call the handler when an user tries to login
    and needs to be authenticated.

  Precondition:
    telnet properly initialized

  Parameters:
    authHandler     - the handler which will be called for authentication
    handlerParam    - handler parameter that will be called with the handler
                      unused by telnet

  Returns:
    - a valid TCPIP_TELNET_HANDLE - if the operation succeeded
    - NULL - if the operation failed

  Example:
    <code>
    TCPIP_TELNET_HANDLE authHandle = TCPIP_TELNET_AuthenticationRegister( myAuthHandler, myParam );
    </code>

  Remarks:
    Currently only one authentication handler is supported by the telnet module.
    The call will fail if a handler is already registered.
    Use TCPIP_TELNET_AuthenticationDeregister first

    This is the PREFERRED authentication method!
    If there's no registrated handler, any login attempt will fail!

    The build should have TCPIP_TELNET_OBSOLETE_AUTHENTICATION == false!

  */
TCPIP_TELNET_HANDLE     TCPIP_TELNET_AuthenticationRegister(TCPIP_TELNET_AUTH_HANDLER authHandler, const void* handlerParam);


//*******************************************************************************
/*
  Function:
    bool    TCPIP_TELNET_AuthenticationDeregister(TCPIP_TELNET_HANDLE authHandle);

  Summary:
    Deregisters a previously registered authentication handler.

  Description:
    This function removes a authentication handler.

  Precondition:
    telnet properly initialized

  Parameters:
    authHandle   - TCPIP_TELNET_HANDLE obtained by a call to TCPIP_TELNET_AuthenticationRegister


  Returns:
    - true  - if the operation succeeded
    - false - if the operation failed

  Example:
    <code>
    TCPIP_TELNET_HANDLE myHandle = TCPIP_TELNET_AuthenticationRegister(myAuthHandler, myParam );
    // process incoming login requests
    // now we're done with it 
    // no more users will be able to login
    TCPIP_TELNET_AuthenticationDeregister(myHandle);
    </code>

  Remarks:
    The build should have TCPIP_TELNET_OBSOLETE_AUTHENTICATION == false!

  */
bool    TCPIP_TELNET_AuthenticationDeregister(TCPIP_TELNET_HANDLE authHandle);


// *****************************************************************************
/*
  Function:
    uint16_t  TCPIP_TELNET_ConnectionsGet(void)

  Summary:
    Gets the number of the telnet connections

  Description:
    This function gets the number of the telnet connections.

  Precondition:
    The Telnet module should have been initialized.

  Parameters:
    None.

  Returns:
    number of telnet connections:
        the initialization parameter TCPIP_TELNET_MODULE_CONFIG::nConnections;

  Remarks:
    None.
*/
uint16_t     TCPIP_TELNET_ConnectionsGet(void);

// *****************************************************************************
/*
  Function:
    bool  TCPIP_TELNET_ConnectionInfoGet(uint16_t connIx, TCPIP_TELNET_CONN_INFO* pInfo)

  Summary:
    Gets information about a telnet connection

  Description:
    This function gets information about a telnet connection.

  Precondition:
    The Telnet module should have been initialized.

  Parameters:
    connIx  - index of the connection to get the info for.
              valid range is  [0, TCPIP_TELNET_MODULE_CONFIG::nConnections - 1]
    pInfo   - pointer to a TCPIP_TELNET_CONN_INFO to be filled with connection data
              Could be NULL if not needed.

  Returns:
    true if the connIx is a valid telnet connection number
    false otherwise

  Remarks:
    None.
*/
bool    TCPIP_TELNET_ConnectionInfoGet(uint16_t connIx, TCPIP_TELNET_CONN_INFO* pInfo);

// *****************************************************************************
/*
  Function:
    void  TCPIP_TELNET_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs Telnet module tasks in the TCP/IP stack.

  Precondition:
    The Telnet module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_TELNET_Task(void);
    
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif	//#ifndef __TELNET_H
