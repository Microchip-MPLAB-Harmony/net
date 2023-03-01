/*******************************************************************************
  FTP Server Definitions for the Microchip TCP/IP Stack
  
  Company:
    Microchip Technology Inc.
    
  File Name: ftp.h

  Summary:
    An embedded FTP (File Transfer Protocol) server is an excellent addition to
    any network-enabled device.

  Description:
    An embedded FTP (File Transfer Protocol) server is an excellent addition to 
    any network-enabled device. FTP server capability facilitates the uploading 
    of files to, and downloading of files from, an embedded device. Almost all 
    computers have, at the very least, a command line FTP client that will allow 
    a user to connect to an embedded FTP server.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/








//DOM-IGNORE-END

#ifndef __FTP_H
#define __FTP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/*
  Structure:
    TCPIP_FTP_CONN_INFO

  Summary:
    FTP connection information.

  Description:
    Gets information about a FTP connection.
*/
typedef struct
{
    uint16_t            connIx;         // FTP connection index: [0, TCPIP_FTP_MODULE_CONFIG::nConnections - 1]
    int16_t             presSkt;        // NET_PRES_SKT_HANDLE_T: connection presentation socket
                                        // Note: Currently -1, reserved for future use
    int16_t             tcpSkt;         // TCP_SOCKET: underlying FTP comamnd socket
    TCP_SOCKET_INFO     tcpInfo;        // FTP command socket information
} TCPIP_FTP_CONN_INFO;


// *****************************************************************************
/*
  Type:
    TCPIP_FTP_HANDLE

  Summary:
    Defines a FTP handle.

  Description:
    Definition of an processing handle used for
    registration of callbacks with the FTP module

*/
typedef const void* TCPIP_FTP_HANDLE;

// *****************************************************************************
/* FTP authentication callback

  Function:
    bool <FunctionName>(const char* user, const char* password, const TCPIP_FTP_CONN_INFO* pInfo, const void* hParam);

  Summary:
    Pointer to a function (handler) that will get called to process FTP authentication

  Description:
    Pointer to a function that will be called by the FTP module
    when a login operation is ongoing and user authentication
    needs to be performed

  Precondition:
    None

  Parameters:
    user        - the user name entered as part of the login process
    password    - the password entered as part of the login process
    pInfo       - pointer to an TCPIP_FTP_CONN_INFO carying specific connection information
                  to help in the authentication process
    hParam      - parameter that was passed at the time of the handler registration                  

  Returns:
    true - if the authentication is successful and the FTP connection should be allowed
    false - if the authentication failed and the connection should be aborted

  Remarks:
    The handler is called in the FTP context.
    The handler should be kept as short as possible as it shouldn't block

    This is the PREFERRED method of authenticating the FTP connection.

    Until the moment the authentication handler is registered
    any login attempts will fail!

    The pInfo will be NULL if the FTP module is initialized with
    TCPIP_FTP_AUTHENTICATION_INFO == true
    
 */
typedef bool(*TCPIP_FTP_AUTH_HANDLER)(const char* user, const char* password, const TCPIP_FTP_CONN_INFO* pInfo, const void* hParam);



// *****************************************************************************
/*
  Structure:
    TCPIP_FTP_MODULE_CONFIG

  Summary:
    FTP Sever module runtime and initialization configuration data.

  Description:
    FTP server configuration and initialization data . Configuration
    is part of tcpip_stack_init.c.

  Remarks:
    The obsolete authentication members will be eventually removed from the
    FTP server module configuration data.
    New projects should use the run time authentication!
*/
typedef struct
{
    uint16_t    cmdPort;        // FTP listening command port: TCPIP_FTP_COMMAND_PORT (21, 990) etc.
    uint16_t    dataPort;       // FTP listening data port: TCPIP_FTP_DATA_PORT (20, 989) etc.
    uint16_t    nConnections;   // number of simultaneous FTP connections allowed
    uint16_t    dataSktTxBuffSize;  // size of Data socket TX buffer for the associated socket; leave 0 for default
    uint16_t    dataSktRxBuffSize;  // size of Data Socket RX buffer for the associated socket; leave 0 for default
    // The FTP server will access files located under this top directory.
    // No access is possible above this directory.
    // As a good practice to follow, use something like: "/mnt_point/ftp/"
    // to limit an external user access to this only directory in the file system
    // when accessing files.
    // NOTE: to allow access to the whole file system use the root directory:
    // "/mnt_point/" or "/" depending on your SYS_FS settings/mounting
    // But this usage should be discouraged because it gives a FTP user 
    // connected to the FTP server access to all system files!
    const char* mountPath;

    // Obsolete authentication scheme members!
    // These members have become obsolete and will be eventually removed.
    // The FTP authentication should be done by registering an authentication handler with TCPIP_FTP_AuthenticationRegister
    // To use the obsolete authentication scheme, the FTP module should be built with TCPIP_FTPS_OBSOLETE_AUTHENTICATION == true!
    //
    char     *userName; // FTP login User name. Size should not exceed more than  TCPIP_FTP_USER_NAME_LEN
    char     *password; // FTP login password. Size should not exceed more than TCPIP_FTP_PASSWD_LEN
} TCPIP_FTP_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    TCPIP_FTP_HANDLE     TCPIP_FTP_AuthenticationRegister(TCPIP_FTP_AUTH_HANDLER authHandler, const void* handlerParam);

  Summary:
    Sets a new FTP authentication handler.

  Description:
    This function registers a new authentication handler.
    FTP module will call the handler when an user tries to login
    and needs to be authenticated.

  Precondition:
    FTP properly initialized

  Parameters:
    authHandler     - the handler which will be called for authentication
    handlerParam    - handler parameter that will be called with the handler
                      unused by FTP

  Returns:
    - a valid TCPIP_FTP_HANDLE - if the operation succeeded
    - NULL - if the operation failed

  Example:
    <code>
    TCPIP_FTP_HANDLE authHandle = TCPIP_FTP_AuthenticationRegister( myAuthHandler, myParam );
    </code>

  Remarks:
    Currently only one authentication handler is supported by the FTP module.
    The call will fail if a handler is already registered.
    Use TCPIP_FTP_AuthenticationDeregister first

    This is the PREFERRED authentication method!
    If there's no registrated handler, any login attempt will fail!

    The build should have TCPIP_FTPS_OBSOLETE_AUTHENTICATION == false!

  */
TCPIP_FTP_HANDLE     TCPIP_FTP_AuthenticationRegister(TCPIP_FTP_AUTH_HANDLER authHandler, const void* handlerParam);

//*******************************************************************************
/*
  Function:
    bool    TCPIP_FTP_AuthenticationDeregister(TCPIP_FTP_HANDLE authHandle);

  Summary:
    Deregisters a previously registered authentication handler.

  Description:
    This function removes a authentication handler.

  Precondition:
    FTP properly initialized

  Parameters:
    authHandle   - TCPIP_FTP_HANDLE obtained by a call to TCPIP_FTP_AuthenticationRegister


  Returns:
    - true  - if the operation succeeded
    - false - if the operation failed

  Example:
    <code>
    TCPIP_FTP_HANDLE myHandle = TCPIP_FTP_AuthenticationRegister(myAuthHandler, myParam );
    // process incoming login requests
    // now we're done with it 
    // no more users will be able to login
    TCPIP_FTP_AuthenticationDeregister(myHandle);
    </code>

  Remarks:
    The build should have TCPIP_FTPS_OBSOLETE_AUTHENTICATION == false!

  */
bool    TCPIP_FTP_AuthenticationDeregister(TCPIP_FTP_HANDLE authHandle);

     

// *****************************************************************************
/*
  Function:
    void  TCPIP_FTP_ServerTask(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs FTP module tasks in the TCP/IP stack.

  Precondition:
    The FTP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_FTP_ServerTask(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __FTP_H
