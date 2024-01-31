/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCP) Client API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcp.h

  Summary:
    Dynamic Host Configuration Protocol (DHCP) client API definitions.

  Description:
    Provides automatic IP address, subnet mask, gateway address,
    DNS server address, and other configuration parameters on DHCP
    enabled networks.
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

#ifndef __DHCP_H
#define __DHCP_H

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
/* Enumeration: TCPIP_DHCP_EVENT_TYPE

  Summary:
    DHCP Event Type

  Description:
    None.

  Remarks:
    The lease related events are first.
    They are followed by the connection related events.
 */
typedef enum
{
    DHCP_EVENT_NONE     = 0,     // DHCP no event
    // lease related events
    DHCP_EVENT_DISCOVER,         // DHCP discovery sent: cycle started
    DHCP_EVENT_OFFER,            // DHCP offer received
    DHCP_EVENT_REQUEST,          // DHCP request sent
    DHCP_EVENT_ACK,              // DHCP request acknowledge was received
    DHCP_EVENT_DECLINE,          // DHCP decline sent because the ARP probe failed
    DHCP_EVENT_NACK,             // DHCP negative acknowledge was received
    DHCP_EVENT_TIMEOUT,          // DHCP server timeout
    DHCP_EVENT_BOUND,            // DHCP lease obtained
    DHCP_EVENT_REQUEST_RENEW,    // lease request renew sent
    DHCP_EVENT_REQUEST_REBIND,   // lease request rebind sent
    DHCP_EVENT_UNKNOWN,          // an unknown/unsupported DHCP message has been received
    // connection related events
    DHCP_EVENT_CONN_LOST,        // connection to the DHCP server lost
    DHCP_EVENT_CONN_ESTABLISHED, // connection re-established
    DHCP_EVENT_SERVICE_DISABLED, // DHCP service disabled, reverted to the default IP address

} TCPIP_DHCP_EVENT_TYPE;

// *****************************************************************************
/* Structure: TCPIP_DHCP_EVENT_INFO

  Summary:
    Reports DHCP event information.

  Description:
    This data structure is used in the extended DHCP events
    for reporting extended info about an DHCP event.

  Remarks:
    Connection events do not have associated client and server addresses and transaction IDs.
 */
typedef struct
{
    uint32_t        dhcpTimeMs;             // current DHCP time, milliseconds
    IPV4_ADDR       clientAddress;          // for RX events (DHCP_EVENT_OFFER, DHCP_EVENT_ACK, DHCP_EVENT_NACK, DHCP_EVENT_UNKNOWN) this is the destination IPv4 address of the DHCP packet
                                            // for TX and other events this is the client IPv4 address when the DHCP message that triggered the event was sent

    IPV4_ADDR       serverAddress;          // for RX events (DHCP_EVENT_OFFER, DHCP_EVENT_ACK, DHCP_EVENT_NACK, DHCP_EVENT_UNKNOWN) this is the source IPv4 address of the DHCP packet (server that sent the message)
                                            // for TX and other events this is the current bound address.
                                            // If the client is not bound or the network broadcast is used, then this is a broadcast address
    uint32_t        transactionId;          // transaction ID associated with the DHCP message  
                                            // For a connection related event, this is the last ID that has been used
}TCPIP_DHCP_EVENT_INFO;

// *****************************************************************************
/* Enumeration: TCPIP_DHCP_STATUS

  Summary:
    Lists the current status of the DHCP module.

  Description:
    This enumeration lists the current status of the DHCP module.
    Used in getting information about the DHCP state machine. 
 */
typedef enum
{
    TCPIP_DHCP_IDLE = 0,            // idle/inactive state
    TCPIP_DHCP_WAIT_LINK,           // waiting for an active connection
    TCPIP_DHCP_SEND_DISCOVERY,      // sending a Discover message
    TCPIP_DHCP_GET_OFFER,           // waiting for a DHCP Offer
    TCPIP_DHCP_SEND_REQUEST,        // sending a REQUEST message (REQUESTING)
    TCPIP_DHCP_GET_REQUEST_ACK,     // waiting for a Request ACK message
    TCPIP_DHCP_WAIT_LEASE_CHECK,    // waiting for received lease verification
    TCPIP_DHCP_WAIT_LEASE_RETRY,    // waiting for another attempt after the lease verification failed
    TCPIP_DHCP_SKIP_LEASE_CHECK,    // skip the lease verification state
    TCPIP_DHCP_BOUND,               // bound
    TCPIP_DHCP_SEND_RENEW,          // sending a REQUEST message (RENEW state)
    TCPIP_DHCP_GET_RENEW_ACK,       // waiting for ACK in RENEW state
    TCPIP_DHCP_SEND_REBIND,         // sending REQUEST message (REBIND state)
    TCPIP_DHCP_GET_REBIND_ACK,      // waiting for ACK in REBIND state
} TCPIP_DHCP_STATUS;

// *****************************************************************************
/* Structure: TCPIP_DHCP_INFO

  Summary:
    Reports DHCP module information.

  Description:
    This data structure is used for reporting current info
    and status of the DHCP module.
    Used in getting info about the DHCP module. 
 */
typedef struct
{
    TCPIP_DHCP_STATUS           status;             // current status
    uint32_t                    dhcpTime;           // current DHCP time, seconds
    // the following fields are significant only if
    // a lease has been obtained and is currently valid
    // i.e. status >= TCPIP_DHCP_BOUND
    uint32_t                    leaseStartTime;     // time when lease was requested
    uint32_t                    leaseDuration;      // lease duration as returned by the server, seconds
    uint32_t                    renewTime;          // the time for moving to renew state, seconds
    uint32_t                    rebindTime;         // the time for moving to rebind state, seconds
    IPV4_ADDR                   dhcpAddress;        // IPv4 address obtained by DHCP 
    IPV4_ADDR                   subnetMask;         // IPv4 sub net mask obtained by DHCP 
    IPV4_ADDR                   serverAddress;      // IPv4 address of the server that granted the lease  
    const char*                 bootFileName;       // pointer to the bootfile name that was returned by the server
                                                    // This will be 0 if TCPIP_DHCP_STORE_BOOT_FILE_NAME option is not enabled!
    uint8_t                     timeServersNo;      // number of time servers available
                                                    // This will be 0 if TCPIP_DHCP_USE_OPTION_TIME_SERVER option is not enabled
    uint8_t                     ntpServersNo;       // number of NTP servers available
                                                    // This will be 0 if TCPIP_DHCP_USE_OPTION_NTP_SERVER option is not enabled
    const IPV4_ADDR*            timeServers;        // pointer to array of addresses for the time servers
                                                    // size is given by timeServersNo
    const IPV4_ADDR*            ntpServers;         // pointer to array of addresses for the NTP servers
                                                    // size is given by ntpServersNo
}TCPIP_DHCP_INFO;

// *****************************************************************************
/*
  Type:
    TCPIP_DHCP_EVENT_HANDLER
    TCPIP_DHCP_EVENT_HANDLER_EX

  Summary:
    DHCP event handler prototypes.

  Description:
    Prototype of a DHCP simple and extended event handler.
    Clients can register a handler with the DHCP service.
    Once an DHCP event occurs the DHCP service will called the registered handler.

  Parameters:
    hNet    - Interface the DHCP event occurred on.
    evType  - type of event that occurred
    evInfo  - associated event information 
              For the extended handler only
    param   - user supplied parameter.
              Not used by the DHCP module.

  Remarks:
    For the extended handler, evInfo points to a constant structure that should not be modified
    and which is valid only in the context of the event handler call.

    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!
 */

typedef void    (*TCPIP_DHCP_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_TYPE evType, const void* param);
typedef void    (*TCPIP_DHCP_EVENT_HANDLER_EX)(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_TYPE evType, const TCPIP_DHCP_EVENT_INFO* evInfo, const void* param);


// *****************************************************************************
/*
  Type:
    TCPIP_DHCP_HOST_NAME_CALLBACK

  Summary:
    DHCP Host name callback function.

  Description:
    Prototype of a DHCP callback function that returns the host name to 
    be presented to the server by the DHCP client.

    This callback will be called by the DHCP client when communicating
    to the server and a host name is needed.

 Remarks:
    There are certain restictions that apply to the host name strings.
    See the TCPIP_DHCP_HostNameCallbackRegister function.

*/

typedef char*    (*TCPIP_DHCP_HOST_NAME_CALLBACK)(TCPIP_NET_HANDLE hNet);


// *****************************************************************************
/*
  Type:
    TCPIP_DHCP_HANDLE

  Summary:
    DHCP handle.

  Description:
    A handle that a client can use after the event handler has been registered.
 */

typedef const void* TCPIP_DHCP_HANDLE;


// *****************************************************************************
/* DHCP Module Configuration

  Summary:
    DHCP Module Configuration run-time parameters.

  Description:
    This structure contains the data that's passed to the DHCP module
    at the TCP/IP stack initialization.

 */

typedef struct
{
    bool    dhcpEnable;     // DHCP client enable at module start-up 
                            // Note: overridden by TCPIP_NETWORK_CONFIG::startFlags!
    int     dhcpTmo;        // timeout to wait for DHCP lease, seconds
    int     dhcpCliPort;    // client port for DHCP client transactions
    int     dhcpSrvPort;    // remote server port for DHCP server messages

} TCPIP_DHCP_MODULE_CONFIG;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - General
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    void TCPIP_DHCP_Enable(TCPIP_NET_HANDLE hNet)

  Summary:
    Enables the DHCP client for the specified interface.

  Description:
    This function enables the DHCP client for the specified interface, if it is 
    disabled. If it is already enabled, no action is taken.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet - Interface to enable the DHCP client on.

  Returns:
    - true  - if successful
    - false - if unsuccessful
 */
bool TCPIP_DHCP_Enable(TCPIP_NET_HANDLE hNet);


//*****************************************************************************
/*
  Function:
    bool TCPIP_DHCP_Disable(TCPIP_NET_HANDLE hNet)

  Summary:
    Disables the DHCP Client for the specified interface.

  Description:
    This function disables the DHCP client for the specified interface.
    If it is already disabled, no action is taken.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    pNetIf - Interface to disable the DHCP client on.

  Returns:
    - true  - if successful
    - false - if unsuccessful

  Remarks:
    When the DHCP client is disabled and the interface continues using its old configuration,
    it is possible that the lease may expire and the DHCP server provide the IP address
    to another client.
    The application should not keep the old lease unless it is sure 
    that there is no danger of conflict.
 */
bool TCPIP_DHCP_Disable(TCPIP_NET_HANDLE hNet );


//*****************************************************************************
/*
  Function:
    void TCPIP_DHCP_Renew(TCPIP_NET_HANDLE hNet)

  Summary:
    Renews the DHCP lease for the specified interface.

  Description:
    This function attempts to contact the server and renew the DHCP lease for 
    the specified interface.
    The interface should have the DHCP enabled and in bound state
    for this call to succeed.

  Precondition:
    The DHCP module must be initialized and enabled, and have a valid lease.

  Parameters:
    hNet - Interface on which to renew the DHCP lease.

  Returns:
    - true  - if successful
    - false - if unsuccessful
 */
bool TCPIP_DHCP_Renew(TCPIP_NET_HANDLE hNet);


//*****************************************************************************
/*
  Function:
    void TCPIP_DHCP_Request(TCPIP_NET_HANDLE hNet, IPV4_ADDR reqAddress)

  Summary:
    Requests the supplied IPv4 address from a DHCP server.

  Description:
    If the DHCP client is not enabled on that interface, this call will first try to enable it.
    If this succeeds or the DHCP client was already enabled, the following steps are taken: 
    The DHCP client probes the DHCP server and requests the supplied IPv4 address
    as a valid lease for the specified interface.
    If the server acknowledges the request, then this is the new IPv4 address
    of the interface.
    If the DHCP server rejects the request, then the whole DHCP
    process is resumed starting with the DHCP Discovery phase.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet - Interface to renew the DHCP lease on.

  Returns:
    - true  - if successful
    - false - if the supplied IP address is invalid or the DHCP client
              is in the middle of a transaction


 Remarks:
    The requested IPv4 address should be a previous lease that was granted to the host.
    This call should be used when the host is restarting.

 */
bool TCPIP_DHCP_Request(TCPIP_NET_HANDLE hNet, IPV4_ADDR reqAddress);


//*****************************************************************************
/*
  Function:
    bool TCPIP_DHCP_IsEnabled(TCPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DHCP client is enabled on the specified interface.

  Description:
    This function returns the current state of the DHCP client on the specified 
    interface.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet- Interface to query.

  Returns:
    - true  - if the DHCP client service is enabled on the specified interface
    - false - if the DHCP client service is not enabled on the specified interface
 */
bool TCPIP_DHCP_IsEnabled(TCPIP_NET_HANDLE hNet);


//*****************************************************************************
/*
  Function:
    bool TCPIP_DHCP_IsActive(TCPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DHCP client is currently active on the specified interface.

  Description:
    This function returns the current state of the DHCP client on the specified interface.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet- Interface to query.

  Returns:
    - true  - if the DHCP client service is currently active on the specified interface
    - false - if the DHCP client service is not active on the specified interface

 Remarks:
    The DHCP client service could be enabled bot not active.
    For example when there was no DHCP server detected on the network.

 */
bool TCPIP_DHCP_IsActive(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    bool TCPIP_DHCP_IsBound(TCPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DHCP client has an IP address lease on the specified
    interface.

  Description:
    This functin returns the status of the current IP address lease on the specified
    interface.

  Precondition:
    None.

  Parameters:
    hNet - Interface to query

  Returns:
    - true  - DHCP client has obtained an IP address lease (and likely other
              parameters) and these values are currently being used
    - false - No IP address is currently leased
 */
bool TCPIP_DHCP_IsBound(TCPIP_NET_HANDLE hNet);


//****************************************************************************
/*
  Function:
    bool TCPIP_DHCP_IsServerDetected(TCPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DHCP client on the specified interface has been able
    to contact a DHCP server.

  Description:
    This function determines if the DHCP client on the specified interface received 
    any reply from a DHCP server.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet- Interface to query.

  Returns:
    - true  - At least one DHCP server is attached to the specified network
              interface
    - false - No DHCP servers are currently detected on the specified network
              interface
 */
 bool TCPIP_DHCP_IsServerDetected(TCPIP_NET_HANDLE hNet);


// *****************************************************************************
/* Function:
    bool TCPIP_DHCP_RequestTimeoutSet(TCPIP_NET_HANDLE hNet, uint16_t initTmo, 
                                      uint16_t dhcpBaseTmo)

  Summary:
    Sets the DHCP client request and base time-out values.

  Description:
    This function allows the run time adjustment of the DHCP time-out values.
    It specifies for how long the client has to wait for a valid DHCP server reply
    during the initialization process until acquisition of the host address is considered
    to have failed.
    It also sets the DHCP base timeout for DHCP transactions with the server.
    This is automatically incremented by the DHCP client using an exponential back-off
    algorithm.
    Recommended value is 2 <= dhcpBaseTmo <= 64 seconds.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet        - Interface handle
    initTmo     - Initialization time-out to wait for a valid lease acquisition, in seconds
    dhcpBaseTmo - DHCP time-out value for DHCP requests, in seconds

  Returns:
    - true  - if successful
    - false - if a wrong interface handle or time-out value was provided 
    
 */

bool TCPIP_DHCP_RequestTimeoutSet(TCPIP_NET_HANDLE hNet, uint16_t initTmo, 
                                  uint16_t dhcpBaseTmo);



// *****************************************************************************
/* Function:
    TCPIP_DHCP_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_HANDLER handler, const void* hParam)

  Summary:
    Registers a simple DHCP Handler.

  Description:
    This function registers a simple DHCP event handler (not carrying extended event info).
    The DHCP module will call the registered handler when a
    DHCP event (TCPIP_DHCP_EVENT_TYPE) occurs.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet    - Interface handle.
              Use hNet == 0 to register on all interfaces available.
    handler - Handler to be called when a DHCP event occurs.
    hParam  - Parameter to be used in the handler call.
              This is user supplied and is not used by the DHCP module.


  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (out of memory, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    The hParam is passed by the client and will be used by the DHCP when the
    notification is made. It is used for per-thread content or if more modules,
    for example, share the same handler and need a way to differentiate the
    callback.
 */

TCPIP_DHCP_HANDLE      TCPIP_DHCP_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_HANDLER handler, const void* hParam);

// *****************************************************************************
/* Function:
    TCPIP_DHCP_HandlerRegisterEx(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_HANDLER_EX exHandler, const void* hParam)

  Summary:
    Registers an extended DHCP Handler.

  Description:
    This function registers an extended DHCP event handler which will carry extended event info.
    The DHCP module will call the registered handler when a
    DHCP event (TCPIP_DHCP_EVENT_TYPE) occurs.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet    - Interface handle.
              Use hNet == 0 to register on all interfaces available.
    exHandler - Handler to be called when a DHCP event occurs.
    hParam  - Parameter to be used in the handler call.
              This is user supplied and is not used by the DHCP module.


  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (out of memory, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    The hParam is passed by the client and will be used by the DHCP when the
    notification is made. It is used for per-thread content or if more modules,
    for example, share the same handler and need a way to differentiate the
    callback.
 */

TCPIP_DHCP_HANDLE      TCPIP_DHCP_HandlerRegisterEx(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_HANDLER_EX exHandler, const void* hParam);

// *****************************************************************************
/* Function:
    TCPIP_DHCP_HandlerDeRegister(TCPIP_DHCP_HANDLE hDhcp)

  Summary:
    Deregisters a previously registered DHCP handler.
    
  Description:
    This function deregisters a simple or extended DHCP event handler.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hDhcp   - A handle returned by a previous call to TCPIP_DHCP_HandlerRegister/TCPIP_DHCP_HandlerRegisterEx.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered
 */

bool             TCPIP_DHCP_HandlerDeRegister(TCPIP_DHCP_HANDLE hDhcp);

//*****************************************************************************
/*
  Function:
    bool TCPIP_DHCP_InfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_INFO* pDhcpInfo);

  Summary:
    Returns information about the DHCP client on the specified interface.

  Description:
    This function returns the current state and lease info of the DHCP client
    on the specified interface.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet      - Interface to query.
    pDhcpInfo - storage to return the DHCP info
                Could be NULL if not needed

  Returns:
    - true  - if the interface is enabled and exists and the DHCP client service 
              is enabled on that interface and a lease is acquired
    - false - otherwise
 */
bool TCPIP_DHCP_InfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_INFO* pDhcpInfo);


// *****************************************************************************
/* Function:
    bool TCPIP_DHCP_HostNameCallbackRegister(TCPIP_NET_HANDLE hNet, 
                      TCPIP_DHCP_HOST_NAME_CALLBACK nameCallback, bool writeBack)

  Summary:
    Registers a DHCP host name callback with the DHCP client.

  Description:
    This function registers a DHCP host name callback function.
    The DHCP module will call the registered callback when a
    host name needs to be presented to the DHCP server.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hNet         - Interface handle.
                   Use hNet == 0 to register on all interfaces available.
    nameCallback - Callback to be called to get a DHCP host name
    writeBack    - if true, the resulting name will be written to the address returned
                   by the nameCallback (it shouldn't be const!)


  Returns:
    - true  - if the call succeeds
    - false - if error (no such interface, etc.)

  Remarks:
    The nameCallback function has to be valid for the length of DHCP client communication 
    with the server, i.e. when the reported status is bound or some error.

    There is no corresponding deregister function.
    Simply call TCPIP_DHCP_HostNameCallbackRegister with the nameCallback set to 0.

    If such a callback is not registered, then the NetBios name will be used for the DHCP host name.

    The callback function has to return a character string that's compatible
    with the rules imposed for the host names:
    - Host  names  may  contain  only  alphanumeric characters, minus signs ("-"), and periods (".")
    - They must begin with an  alphabetic  character  and  end  with  an  alphanumeric  character
    
    To enforce these rules the extra processing is performed internally
    (on either user supplied host name or NetBios name):
    - spaces, if present, will be stripped from the string
    - illegal characters will be replaced by TCPIP_DHCP_HOST_REPLACE_CHAR 
      character (default is lower case 'x')

   If after processing the resulting string is null, the DHCP host name option is skipped.

    The callback function has to return a character string that's at least 2 characters
    in size;

 */

bool    TCPIP_DHCP_HostNameCallbackRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_HOST_NAME_CALLBACK nameCallback, bool writeBack);

// *****************************************************************************
/*
  Function:
    void  TCPIP_DHCP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs DHCP module tasks in the TCP/IP stack.

  Precondition:
    DHCP module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_DHCP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __DHCP_H

/*
 End of File
 */
