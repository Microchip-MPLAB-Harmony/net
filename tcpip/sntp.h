/*******************************************************************************
  SNTP Client Module API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    sntp.h

  Summary:
    The SNTP module implements the Simple Network Time Protocol. The module
    (by default) updates its internal time every 10 minutes using a pool of 
    public global time servers.

  Description:
    The SNTP module implements the Simple Network Time Protocol. The module 
    updates its internal time every 10 minutes (the interval is user configurable)
    using a pool of public global time servers.
    It then calculates time differences using the internal SYS_TMR timer module.
********************************************************************************/
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

#ifndef __SNTP_H
#define __SNTP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// Placeholder for SNTP Module Configuration
typedef struct
{
    const char*     ntp_server;             // the NTP server to contact; name or IP address
                                            // if NULL or empty, SNTP won't attempt any connection
    const char*     ntp_interface;          // the default NTP interface to use
    IP_ADDRESS_TYPE ntp_connection_type;    // the IPv4/IPv6 connection type
    uint32_t        ntp_reply_timeout;      // timeout for the server reply in seconds
    uint32_t        ntp_stamp_timeout;      // timeout for the timestamp in seconds
    uint32_t        ntp_success_interval;   // server query interval after an successful update, in seconds
    uint32_t        ntp_error_interval;     // server query interval after an error, in seconds
} TCPIP_SNTP_MODULE_CONFIG;


// *****************************************************************************
/* TCPIP_SNTP_RESULT Enumeration

  Summary:
    Provides a list of possible results for the SNTP module.

  Description:
    Provides a list of possible SNTP results.

  Remarks:
    None
 */
typedef enum
{
    // success codes
    SNTP_RES_OK,                // the operation was successful
    SNTP_RES_PROGRESS,          // an NTP operation is in progress

    // error codes
    SNTP_RES_BUSY               = -1,   // module is busy
    SNTP_RES_TSTAMP_ERROR       = -2,   // no timestamp is available, server not contacted
    SNTP_RES_TSTAMP_STALE       = -3,   // timestamp is stale, there's no recent timestamp
    SNTP_RES_SKT_ERR            = -4,   // NTP socket operation error
    SNTP_RES_SKT_BIND_ERR       = -5,   // NTP socket bind failed
    SNTP_RES_NTP_SERVER_TMO     = -6,   // NTP server could not be accessed
    SNTP_RES_NTP_VERSION_ERR    = -7,   // wrong NTP version received
    SNTP_RES_NTP_TSTAMP_ERR     = -8,   // wrong NTP timestamp received
    SNTP_RES_NTP_SYNC_ERR       = -9,   // NTP time synchronization error
    SNTP_RES_NTP_KOD_ERR        = -10,  // an NTP KissOfDeath code has been received
    SNTP_RES_NTP_DNS_ERR        = -11,  // an NTP DNS error
    SNTP_RES_NTP_IF_ERR         = -12,  // an NTP interface error
    SNTP_RES_NTP_CONN_ERR       = -13,  // an NTP connection type error
} TCPIP_SNTP_RESULT;


// *****************************************************************************
/* TCPIP_SNTP_TIME_STAMP structure

  Summary:
    Timestamp data structure used by the SNTP module.

  Description:
    Describes the structure of the timestamp used by the SNTP module.

  Remarks:
    Timestamp formats are based on the prime epoch i.e. the base date is the
    0 h 1 January 1900 UTC for which the timestamp is 0.

    The tStampFraction is the least significant 32 bit number
    and represents 2 ^ 32 fractions of a second.

    The tStampSeconds is the most significant 32 bit number
    and represents seconds.
    
    The value of the timestamp is equal to:
    tStampSeconds + tStampFraction / 2 ^ 32 seconds
    
    The timestamp format is converted to host machine order.
 */

typedef union
{
    struct
    {
        uint32_t    tStampFraction;     // 2 ^ 32 fractionary part
        uint32_t    tStampSeconds;      // seconds
    };
    uint64_t    llStamp;
}TCPIP_SNTP_TIME_STAMP;


// *****************************************************************************
/* TCPIP_SNTP_EVENT_TIME_DATA structure

  Summary:
    Data structure used by the SNTP module to report a timestamp event.

  Description:
    Describes the structure of the data used by the SNTP module to report
    a successful acquisition of a timestamp from the NTP server.

  Remarks:
    This data is reported as part of a TCPIP_SNTP_EVENT_TSTAMP_OK event. 
 */

typedef struct
{
    TCPIP_SNTP_TIME_STAMP   tStamp;     // prime epoch based timestamp as obtained form the NTP server
    uint64_t                tStampTick; // the 64 bit system tick when this timestamp has been obtained
    uint32_t                tUnixSeconds;   // number of seconds converted to UNIX epoch format
                                            // i.e. since 1 Jan 1970;
    uint32_t                tMilliseconds;  // fractional part converted to milliseconds
    uint32_t                nUpdates;       // number of successful updates from the NTP server so far
}TCPIP_SNTP_EVENT_TIME_DATA;


// *****************************************************************************
/* TCPIP_SNTP_EVENT Enumeration

  Summary:
    List of events for the SNTP module.

  Description:
    Provides a list of run time SNTP events .

  Remarks:
    None
 */
typedef enum
{
    TCPIP_SNTP_EVENT_NONE           = 0,    // invalid event
    // success
    TCPIP_SNTP_EVENT_TSTAMP_OK      = 1,    // a new NTP timestamp has been acquired successfully
    // error events
    TCPIP_SNTP_EVENT_DNS_ERROR      = -1,   // NTP server name DNS failed 
    TCPIP_SNTP_EVENT_IF_ERROR       = -2,   // could not select a valid NTP interface
    TCPIP_SNTP_EVENT_SKT_ERROR      = -3,   // could not bind the socket to the NTP interface
                                            // or timeout on the socket TX
    TCPIP_SNTP_EVENT_SERVER_TMO     = -4,   // no reply received from the NTP server
    TCPIP_SNTP_EVENT_VER_ERROR      = -5,   // the server response has a wrong version
    TCPIP_SNTP_EVENT_TSTAMP_ERROR   = -6,   // the server response had wrong timestamp
    TCPIP_SNTP_EVENT_KOD_ERROR      = -7,   // the server replied with a "Kiss O' Death" code
    TCPIP_SNTP_EVENT_SYNC_ERROR     = -8,   // the server response has a synchronization error

}TCPIP_SNTP_EVENT;


// *****************************************************************************
/*
  Type:
    TCPIP_SNTP_EVENT_HANDLER

  Summary:
    SNTP event handler prototype.

  Description:
    Prototype of a SNTP event handler. Clients can register a handler with the
    SNTP service. Once an SNTP event occurs the SNTP service will called the
    registered handler.
    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!

    param  - Parameter that is supplied by the SNTP module in the handler call
             according to the event
             - if evType < 0 (an error event - TCPIP_SNTP_EVENT_DNS_ERROR ... TCPIP_SNTP_EVENT_SYNC_ERROR):
                evParam represents the 32 bit system tick when the event occurred.

             - if evType == TCPIP_SNTP_EVENT_TSTAMP_OK:
                evParam represents a const pointer to a TCPIP_SNTP_EVENT_TIME_DATA
                describing the timestamp received.

 */

typedef void    (*TCPIP_SNTP_EVENT_HANDLER)(TCPIP_SNTP_EVENT evType, const void* evParam);

// *****************************************************************************
/*
  Type:
    TCPIP_SNTP_HANDLE

  Summary:
    SNTP handle.

  Description:
    A handle that a client can use after the event handler has been registered.
 */

typedef const void* TCPIP_SNTP_HANDLE;



//*****************************************************************************
//*****************************************************************************
// Section: Functions
//*****************************************************************************
//*****************************************************************************


//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT TCPIP_SNTP_ConnectionParamSet(TCPIP_NET_HANDLE netH, IP_ADDRESS_TYPE ntpConnType, const char* ntpServer);

  Summary:
    Sets the current SNTP connection parameters.

  Description:
    This function sets the parameters for the next SNTP connections.
    
  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    netH            - new interface to use as default SNTP interface
                      if 0, the current interface is not changed
    ntpConnType     - type of connection to make: IPv4 or IPv6
                      if IP_ADDRESS_TYPE_ANY, the current setting is not changed
    ntpServer       - the NTP server to be used; name or an IP address can be used
                      if NULL or empty, SNTP won't attempt any connection

  Returns:
    - SNTP_RES_OK - if the call succeeded
    - TCPIP_SNTP_RESULT error code - if the call did not succeed

  Remarks:
    None.
 */
TCPIP_SNTP_RESULT    TCPIP_SNTP_ConnectionParamSet(TCPIP_NET_HANDLE netH, IP_ADDRESS_TYPE ntpConnType, const char* ntpServer);

//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT TCPIP_SNTP_ConnectionInitiate(void);

  Summary:
    Forces a connection to the NTP server

  Description:
    This function will start a connection to the NTP server
    
  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    None.

  Returns:
    - SNTP_RES_OK - if the call succeeded
    - SNTP_RES_PROGRESS  - if a connection already in progress
                           another one could not be started

  Remarks:
    None.
 */
TCPIP_SNTP_RESULT     TCPIP_SNTP_ConnectionInitiate(void);

//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT TCPIP_SNTP_Disable(void);

  Summary:
    Disables the NTP client module

  Description:
    This function will disable the NTP client module
    This means that it will no longer transmit or receive packets
    In this state it can be configurated using TCPIP_SNTP_ConnectionParamSet 
    
  Precondition:
    The TSNTP module should have been initialized.

  Parameters:
    None.

  Returns:
    - SNTP_RES_OK - the call succeeded

  Remarks:
    After a call to TCPIP_SNTP_Disable() the SNTP module will be idle
    until a call to TCPIP_SNTP_Enable is made.
 */
TCPIP_SNTP_RESULT     TCPIP_SNTP_Disable(void);

//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT TCPIP_SNTP_Enable(void);

  Summary:
    Enables the NTP client module

  Description:
    This function will enable the NTP client module if it was 
    previously disabled.
    The module will resume functionality, transmitting and receiving data
    and acquiring time stamps
    
  Precondition:
    The TSNTP module should have been initialized.

  Parameters:
    None.

  Returns:
    - SNTP_RES_OK - the call succeeded

  Remarks:
    After a call to TCPIP_SNTP_Disable() the SNTP module will be idle
    until a call to TCPIP_SNTP_Enable is made.

    When enabled, SNTP will try to connect to the NTP server
    and acquire a time stamp
 */
TCPIP_SNTP_RESULT     TCPIP_SNTP_Enable(void);

//*****************************************************************************
/*
  Function:
    bool     TCPIP_SNTP_IsEnabled(void);

  Summary:
    Returns the enable/disable state of the NTP client module

  Description:
    This function returns the current enabled/disabled state of the module.
    
  Precondition:
    The TSNTP module should have been initialized.

  Parameters:
    None.

  Returns:
    - true - if SNTP module is enabled
    - false - if SNTP module is disabled

  Remarks:
    None
 */
bool     TCPIP_SNTP_IsEnabled(void);

//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT TCPIP_SNTP_TimeGet(uint32_t* pUTCSeconds, uint32_t* pMs);

  Summary:
    Obtains the current time in UTC format from the SNTP module.

  Description:
    This function obtains the current time as reported by the SNTP module.
    Use this value for absolute time stamping.
    The value returned is (by default) in UTC format:
    the number of seconds since 01-Jan-1970 00:00:00.

    This function also returns the current millisecond obtained from an NTP server.
    
  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    pUTCSeconds - pointer to store the current UTC seconds 
                  could be NULL if the UTC time is not needed
    pMs         - pointer to store the current millisecond

  Returns:
    - SNTP_RES_OK - if the call succeeded and the values are accurate
    - SNTP_RES_TSTAMP_STALE error code - if there is no recent timestamp
    - SNTP_RES_TSTAMP_ERROR error code - if there is no available timestamp

  Remarks:
    When SNTP_RES_TSTAMP_ERROR is returned, the time values are meaningless
    and should not be used.
 */
TCPIP_SNTP_RESULT     TCPIP_SNTP_TimeGet(uint32_t* pUTCSeconds, uint32_t* pMs);

//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT     TCPIP_SNTP_TimeStampStatus(void);

  Summary:
    Gets the current status of the timestamp;

  Description:
    This function gets the timestamp status as it was obtained from an NTP server.
    
  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    None

  Returns:
    - SNTP_RES_OK - if there is a valid timestamp
    - SNTP_RES_TSTAMP_STALE error code - there is a timestamp, but it's old
    - SNTP_RES_TSTAMP_ERROR error code - if there is no available timestamp

  Remarks:
    The result is info only and could change immediately.
 */
TCPIP_SNTP_RESULT     TCPIP_SNTP_TimeStampStatus(void);

//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT TCPIP_SNTP_TimeStampGet(TCPIP_SNTP_TIME_STAMP* pTStamp, uint32_t* pLastUpdate);

  Summary:
    Gets the last valid timestamp obtained from an NTP server.

  Description:
    This function gets the last valid timestamp obtained from an NTP server.
    
  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    pTStamp     - pointer to a 64 bit buffer to store the last NTP timestamp 
                  could be NULL if the timestamp not needed
    pLastUpdate - pointer to store the last timestamp update tick 
                  could be NULL if the update time not needed

  Returns:
    - SNTP_RES_OK - if the call succeeded
    - SNTP_RES_TSTAMP_STALE error code - if there is no recent timestamp
    - SNTP_RES_TSTAMP_ERROR error code - if there is no available timestamp

  Remarks:
    The timestamp is returned in host order (Little Endian)
    with the most significant 32 bits representing seconds
    and the least significant 32 bits representing fraction of seconds (seconds / 2 ^ 32).
 */
TCPIP_SNTP_RESULT     TCPIP_SNTP_TimeStampGet(TCPIP_SNTP_TIME_STAMP* pTStamp, uint32_t* pLastUpdate);


//*****************************************************************************
/*
  Function:
    TCPIP_SNTP_RESULT TCPIP_SNTP_LastErrorGet(void);

  Summary:
    Gets the last error code set in the NTP server.

  Description:
    This function returns the last NTP error code and clears the current error code.
    
  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    None.

  Returns:
    The last error code encountered by the NTP module.

  Remarks:
    None/
 */
TCPIP_SNTP_RESULT     TCPIP_SNTP_LastErrorGet(void);

// *****************************************************************************
/* Function:
   TCPIP_SNTP_HANDLE  TCPIP_SNTP_HandlerRegister(TCPIP_SNTP_EVENT_HANDLER handler);

  Summary:
    Registers a SNTP Handler.

  Description:
    This function registers a SNTP event handler.
    The SNTP module will call the registered handler when a
    SNTP event (TCPIP_SNTP_EVENT) occurs.

  Precondition:
    The SNTP module must be initialized.

  Parameters:
    handler - Handler to be called when a SNTP event occurs.

  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (an event handler already registered, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    Currently only one event handler is supported.
    To register a new one, use TCPIP_SNTP_HandlerDeRegister first
 */

TCPIP_SNTP_HANDLE      TCPIP_SNTP_HandlerRegister(TCPIP_SNTP_EVENT_HANDLER handler);


// *****************************************************************************
/* Function:
    TCPIP_SNTP_HandlerDeRegister(TCPIP_SNTP_HANDLE hSntp)

  Summary:
    Deregisters a previously registered SNTP handler.
    
  Description:
    This function deregisters the SNTP event handler.

  Precondition:
    The SNTP module must be initialized.

  Parameters:
    hSntp   - A handle returned by a previous call to TCPIP_SNTP_HandlerRegister.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered
 */

bool             TCPIP_SNTP_HandlerDeRegister(TCPIP_SNTP_HANDLE hSntp);


// *****************************************************************************
/*
  Function:
    void  TCPIP_SNTP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs SNTP module tasks in the TCP/IP stack.

  Precondition:
    The SNTP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_SNTP_Task(void);


//*****************************************************************************
//*****************************************************************************
// Section: Obsolete Functions
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
/*
  Function:
    uint32_t TCPIP_SNTP_UTCSecondsGet(void)

  Summary:
    Obtains the current time from the SNTP module.

  Description:
    This function obtains the current time as reported by the SNTP module.
    Use this value for absolute time stamping.  The value returned is (by
    default) the number of seconds since 01-Jan-1970 00:00:00.

  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    None.

  Returns:
    The number of seconds since the Unix epoch.  (Default 01-Jan-1970 00:00:00)

  Remarks:
    Do not use this function for time difference measurements.
    The SYS_TMR/TMR_DRV modules are more appropriate for those requirements.

    Do not use this function without first checking that a valid timestamp has obtained
    from the NTP server!

    This function is obsolete and it will be eventually removed.
    Use the new version TCPIP_SNTP_TimeGet
 */
uint32_t    TCPIP_SNTP_UTCSecondsGet(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __SNTP_H
