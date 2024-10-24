/*******************************************************************************
  Domain Name System (DNS) Client API Header file
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    dns.h

  Summary:
    DNS definitions and interface file.

  Description:
    This source file contains the DNS client module API.
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

#ifndef __DNS_H
#define __DNS_H

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
  Enumeration:
    TCPIP_DNS_EVENT_TYPE

  Summary:
    This enumeration is used  to notify DNS client applications.

  Description:
    These events are used while notifying to the registered applications.
    
  Remarks:
    None.
*/
typedef enum
{
    TCPIP_DNS_EVENT_NONE,               // DNS no event
    TCPIP_DNS_EVENT_NAME_QUERY,         // DNS Query sent
    TCPIP_DNS_EVENT_NAME_RESOLVED,      // DNS Name resolved
    TCPIP_DNS_EVENT_NAME_EXPIRED,       // name entry expired
    TCPIP_DNS_EVENT_NAME_REMOVED,       // name removed to make room for another entry
    TCPIP_DNS_EVENT_NAME_ERROR,         // no such name reported by the DNS server
    TCPIP_DNS_EVENT_SOCKET_ERROR,       // a DNS probe could not be sent, socket was busy, TX failed, etc.
    TCPIP_DNS_EVENT_NO_INTERFACE,       // a DNS probe could not be sent, no DNS interface could be selected
    TCPIP_DNS_EVENT_INTERNAL_ERROR,     // a DNS internal error occurred. Should not happen.

}TCPIP_DNS_EVENT_TYPE;

// *****************************************************************************
/* Type:
        TCPIP_DNS_EVENT_HANDLER

  Summary:
    Notification handler that can be called when a specific entry is resolved
    and entry is timed out.

  Description:
    The format of a notification handler registered with the DNS module.
    Once an DNS event occurs the DNS service will be called for the registered handler.
    
  Parameters:
    hNet    - the interface on which the DNS event occurred
    evType  - the DNS reported event
    name    - the host name associated with the event
    param   - additional user parameter - see TCPIP_DNS_HandlerRegister 

  Remarks:
    If pNetIf == 0, the notification is called for events on any interface.
*/
typedef void    (*TCPIP_DNS_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, TCPIP_DNS_EVENT_TYPE evType, const char* name, const void* param);

// *****************************************************************************
/*
  Type:
    TCPIP_DNS_HANDLE

  Summary:
    DNS client handle.

  Description:
    A handle that a application needs to use when deregistering a notification handler. 

  Remarks:
    This handle can be used by the application after the event handler has been registered.
*/
typedef const void* TCPIP_DNS_HANDLE;

// *****************************************************************************
/* 
  Enumeration:
    TCPIP_DNS_RESOLVE_TYPE

  Summary:
    DNS query record type.

  Description:
    This enumeration lists the RecordType argument for TCPIP_DNS_Resolve.
    The stack supports DNS_TYPE_A and DNS_TYPE_AAAA.
    
  Remarks:
    None.
*/
typedef enum
{

    TCPIP_DNS_TYPE_A      = 1,        // Indicates an A (standard address) record.
    TCPIP_DNS_TYPE_MX     = 15,       // Indicates an MX (mail exchanger) record.
    TCPIP_DNS_TYPE_AAAA   = 28u,      // Indicates a quad-A (IPv6 address) address record.
    TCPIP_DNS_TYPE_ANY    = 0xff,
}TCPIP_DNS_RESOLVE_TYPE;

// *****************************************************************************
/* 
  Enumeration:
    TCPIP_DNS_ENABLE_FLAGS

  Summary:
    Flags for enabling the DNS service on an interface.

  Description:
    This enumeration lists the TCPIP_DNS_ENABLE_FLAGS argument
    for TCPIP_DNS_Enable.
    
  Remarks:
    See the TCPIP_DNS_Enable description for details
*/

typedef enum
{
    TCPIP_DNS_ENABLE_DEFAULT    = 0,    // the interface is capable of performing DNS name resolution
    TCPIP_DNS_ENABLE_STRICT,            // only this interface will be used for DNS name resolution 
    TCPIP_DNS_ENABLE_PREFERRED,         // prefer this interface when doing DNS name resolution
}TCPIP_DNS_ENABLE_FLAGS;




// *****************************************************************************
/* Enumeration:
    TCPIP_DNS_RESULT

  Summary:
    DNS client result codes.

  Description:
    DNS Client operations results.

  Remarks:
    None.
*/
typedef enum
{
    // success codes
    TCPIP_DNS_RES_OK                  = 0,    // operation succeeded
    TCPIP_DNS_RES_PENDING,                    // operation is ongoing
    TCPIP_DNS_RES_NAME_IS_IPADDRESS,          // DNS request is a IPv4 or IPv6 address

    // failure codes
    TCPIP_DNS_RES_NO_NAME_ENTRY       = -1,   // no such name exists
    TCPIP_DNS_RES_NO_IP_ENTRY         = -2,   // no such IP type exists
    TCPIP_DNS_RES_NO_IX_ENTRY         = -3,   // no such index exists
    TCPIP_DNS_RES_EMPTY_IX_ENTRY      = -4,   // no entry associated to this index exists
    TCPIP_DNS_RES_SERVER_TMO          = -5,   // DNS server response tmo
    TCPIP_DNS_RES_NO_SERVICE          = -6,   // DNS service not implemented or uninitialized
    TCPIP_DNS_RES_NO_INTERFACE        = -7,   // no interface for DNS traffic available
    TCPIP_DNS_RES_CACHE_FULL          = -8,   // the cache is full and no entry could be added
    TCPIP_DNS_RES_INVALID_HOSTNAME    = -9,   // Invalid hostname
    TCPIP_DNS_RES_SOCKET_ERROR       = -10,   // DNS UDP socket error: not ready, TX error, etc.
    TCPIP_DNS_RES_INTERNAL_ERROR     = -11,   // DNS internal error occurred. Should not happen.
}TCPIP_DNS_RESULT;


// *****************************************************************************
/* 
  Structure:
    TCPIP_DNS_CLIENT_MODULE_CONFIG

  Summary:
    Provides a place holder for DNS client configuration.

  Description:
    DNS module runtime configuration and initialization parameters. 

  Remarks:
    Currently the DNS module operation is supported only over IPv4.

*/
typedef struct
{
    bool            deleteOldLease;     // Delete old cache if still in place,
    // specific DNS parameters
    int             cacheEntries;       // Max number of cache entries 
    uint32_t        entrySolvedTmo;     // Solved entry removed after this tmo if not referenced - seconds
    int             nIPv4Entries;       // Number of IPv4 entries per DNS name and Default value is 1.
    int             nIPv6Entries;       // Number of IPv6 address per DNS Name
                                        // Default value is 1 and is used only when IPv6 is enabled
    IP_ADDRESS_TYPE ipAddressType;      // IP protocol type to use for connecting to the DNS server:
                                        // IPv4 or IPv6
}TCPIP_DNS_CLIENT_MODULE_CONFIG;

// *****************************************************************************
/*
  Structure:
    TCPIP_DNS_ENTRY_QUERY

  Summary:
    DNS module query data for both IPv4 and IPv6 .

  Description:
    DNS module uses this structure to return information about a resolved IPv4 and IPv6 address.

  Remarks:
    None.
*/
typedef struct
{
    // input parameters
    char*               hostName;       // Pointer to a name to receive the host name for that particular entry
    int                 nameLen;        // hostName buffer size
    IPV4_ADDR           *ipv4Entry;     // array of IPv4 entries/addresses to be populated
    int                 nIPv4Entries;   // number of entries in the ipv4Entry[] array;
    IPV6_ADDR           *ipv6Entry;     // array of IPv6 entries/addresses to be populated
    int                 nIPv6Entries;   // number of entries in the ipv6Entry[] array;

    // output results
    TCPIP_DNS_RESULT    status;         // current status for this name:
                                        //  - TCPIP_DNS_RES_OK: name is resolved
                                        //  - TCPIP_DNS_RES_PENDING: name is pending
                                        //  - TCPIP_DNS_RES_SERVER_TMO: server timeout
                                        //
    uint32_t            ttlTime;        // time to live for a solved DNS entry
    TCPIP_NET_HANDLE    hNet;           // interface the name was obtained or on which the query is currently ongoing
    int                 serverIx;       // index of the server used on that interface
    int                 nIPv4ValidEntries;// number of valid entries written to the ipv4Entry[]
    int                 nIPv6ValidEntries;// number of valid entries written to the ipv6Entry[]
}TCPIP_DNS_ENTRY_QUERY;

// *****************************************************************************
/*
  Structure:
    TCPIP_DNS_CLIENT_INFO

  Summary:
    General DNS client info.

  Description:
    DNS module data structure used for getting information about the module settings.

  Remarks:
    None.
*/
typedef struct
{
    TCPIP_NET_HANDLE    strictNet;                      // current strict DNS interface
    TCPIP_NET_HANDLE    prefNet;                        // current preferred DNS interface
    uint32_t            dnsTime;                        // current DNS time, seconds
    uint16_t            pendingEntries;                 // number of entries that need to be solved
    uint16_t            currentEntries;                 // number of solved and unslolved name entries
    uint16_t            totalEntries;                   // total number of supported name entries
}TCPIP_DNS_CLIENT_INFO;

// *****************************************************************************
// *****************************************************************************
// Section: DNS Client Functions
// *****************************************************************************
// *****************************************************************************

//****************************************************************************
/*  Function:
    TCPIP_DNS_RESULT TCPIP_DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type)

  Summary:
    Begins resolution of an address.

  Description:
    This function attempts to resolve a host name to an IP address.
    When called, it will attempt to send a DNS query  to a DNS server for resolving the name.
    Call TCPIP_DNS_IsResolved to determine if name resolution is complete.

  Precondition:
    DNS client module initialized.
    
  Parameters:
    hostName   - A pointer to the null terminated string specifying the
                 host for which to resolve an IP.
    type       - a TCPIP_DNS_RESOLVE_TYPE value specifying the desired resolution

  Returns:
    TCPIP_DNS_RES_OK          - success, name is solved.
    TCPIP_DNS_RES_PENDING     - operation is ongoing
    TCPIP_DNS_RES_NAME_IS_IPADDRESS   - name request is a IPv4 or IPv6 address

    or an error code if an error occurred
    
  Remarks:
    To clear the cache use TCPIP_DNS_Disable(hNet, true);

  */
TCPIP_DNS_RESULT  TCPIP_DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type);


//****************************************************************************
/*  
  Function:
    TCPIP_DNS_RESULT TCPIP_DNS_IsResolved(const char* hostName, IP_MULTI_ADDRESS* hostIP, IP_ADDRESS_TYPE type)

  Summary:
    Determines if the DNS resolution is complete and provides the host IP address.

  Description:
    Call this function to determine if the DNS resolution of an address has
    been completed.  If so, the resolved address will be provided in hostIP.

  Precondition:
    TCPIP_DNS_Resolve has been called.

  Parameters:
    hostName - A pointer to the null terminated string specifying the
               host for which to resolve an IP.
    hostIP   - A pointer to an IP_MULTI_ADDRESS structure in which to store the
               resolved IPv4/IPv6 address if resolution is complete.
               Could be NULL if not needed.
    type     - type of address needed: IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6/IP_ADDRESS_TYPE_ANY

  Return Values:
    - TCPIP_DNS_RES_OK - The DNS client has obtained an IP. hostIP will contain the 
                   resolved address.
    - TCPIP_DNS_RES_PENDING - The resolution process is still in progress
    - TCPIP_DNS_RES_SERVER_TMO - DNS server timed out
    - TCPIP_DNS_RES_NO_NAME_ENTRY - no such entry to be resolved exists

  Remarks:
    The function will set either an IPv6 or an IPv4 address to the hostIP address,
    depending on what's available.

    If type IP_ADDRESS_TYPE_ANY is specified the hostIP will be updated with the first available
    solved address: either IPv6 or IPv4!
*/
TCPIP_DNS_RESULT  TCPIP_DNS_IsResolved(const char* hostName, IP_MULTI_ADDRESS* hostIP, IP_ADDRESS_TYPE type);

//****************************************************************************
/*  
  Function:
    TCPIP_DNS_RESULT  TCPIP_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6);

  Summary:
    Determines if the DNS resolution is complete and provides the host IP address.

  Description:
    Call this function to determine if the DNS name resolution has been completed.
    This function allows for retrieval of separate IPv4 and IPv6 addresses for a name.

  Precondition:
    TCPIP_DNS_Resolve has been called.

  Parameters:
    hostName - A pointer to the null terminated string specifying the
               host for which to resolve an IP.
    hostIPv4 - A pointer to an IPV4_ADDR structure in which to store the
               resolved IPv4 address if resolution is complete.
               Could be NULL if not needed.
    hostIPv6 - A pointer to an IPV6_ADDR structure in which to store the
               resolved IPv6 address if resolution is complete.
               Could be NULL if not needed.

  Return Values:
    - TCPIP_DNS_RES_OK - The DNS client has obtained an IP. hostIP will contain the 
                   resolved address.
    - TCPIP_DNS_RES_PENDING - The resolution process is still in progress
    - TCPIP_DNS_RES_SERVER_TMO - DNS server timed out
    - TCPIP_DNS_RES_NO_NAME_ENTRY - no such entry to be resolved exists

  Remarks:
    The function will set either an IPv6 or an IPv4 address to the hostIP address,
    depending on what's available.
*/
TCPIP_DNS_RESULT  TCPIP_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6);


//****************************************************************************
/*  
  Function:
    int TCPIP_DNS_GetIPv4Addresses(const char* hostName, int startIndex, IPV4_ADDR* pIPv4Addr, int nIPv4Addresses);

  Summary:
    Get IPV4 addresses for a DNS resolved name.
    
  Description:
    This function will return IPv4 addresses for a host name
    if the DNS resolution has been completed.  

  Precondition:
    TCPIP_DNS_Resolve has been called.

  Parameters:
    hostName    - A pointer to the null terminated string specifying the
                  host name.
    startIndex  - starting index of the IP address to be returned when multiple addresses
                  are available
                  The max number of addresses that can be stored for a host name is given by:
                  TCPIP_DNS_CLIENT_MODULE_CONFIG::nIPv4Entries.
                  The current number of valid entries for an address is given by
                  TCPIP_DNS_GetIPAddressesNumber().
                  A valid index is [0, TCPIP_DNS_GetIPAddressesNumber(IP_ADDRESS_TYPE_IPV4));
    pIPv4Addr   - pointer to array of IPv4 addresses to store the host IPv4 addresses
    nIPv4Addresses  - number of IPv4 addresses in the pIPv4Addr array.
        
  Returns:
    > 0     - the number of addresses copied to the pIPv4Addr array
    0       - if the host name was not found, invalid index, bad parameter, etc.

    
  Remarks:
    None.
*/
int TCPIP_DNS_GetIPv4Addresses(const char* hostName, int startIndex, IPV4_ADDR* pIPv4Addr, int nIPv4Addresses);

//****************************************************************************
/*  
  Function:
    int TCPIP_DNS_GetIPv6Addresses(const char* hostName, int startIndex, IPV6_ADDR* pIPv6Addr, int nIPv6Addresses);

  Summary:
    Get IPV6 addresses for a DNS resolved name.
    
  Description:
    This function will return IPv6 addresses for a host name
    if the DNS resolution has been completed.  

  Precondition:
    TCPIP_DNS_Resolve has been called.

  Parameters:
    hostName    - A pointer to the null terminated string specifying the
                  host name.
    startIndex  - starting index of the IP address to be returned when multiple addresses
                  are available
                  The max number of addresses that can be stored for a host name is given by:
                  TCPIP_DNS_CLIENT_MODULE_CONFIG::nIPv6Entries.
                  The current number of valid entries for an address is given by
                  TCPIP_DNS_GetIPAddressesNumber().
                  A valid index is [0, TCPIP_DNS_GetIPAddressesNumber(IP_ADDRESS_TYPE_IPV6));
    pIPv6Addr   - pointer to array of IPv6 addresses to store the host IPv6 addresses
    nIPv6Addresses  - number of IPv6 addresses in the pIPv6Addr array.
        
  Returns:
    > 0     - the number of addresses copied to the pIPv6Addr array
    0       - if the host name was not found, invalid index, bad parameter, etc.

    
  Remarks:
    None.
*/
int TCPIP_DNS_GetIPv6Addresses(const char* hostName, int startIndex, IPV6_ADDR* pIPv6Addr, int nIPv6Addresses);

//****************************************************************************
/*  
  Function:
    int TCPIP_DNS_GetIPAddressesNumber(const char* hostName, IP_ADDRESS_TYPE type)

  Summary:
    Get the count of resolved IPv4 and/or IPv6 address for a host name.
    
  Description:
    This function returns the total count of IPv4 and/or IPv6 addresses
    that exist for a resolved host name.
   
  Precondition:
    TCPIP_DNS_Resolve has been called.

  Parameters:
    hostName - A pointer to the null terminated string specifying the
               host name 
    type     - IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6/IP_ADDRESS_TYPE_ANY

  Return Values:
    Number of resolved IPv4 and/or IPv6 addresses for the host name.
        
  Remarks:
    None.
  */
int TCPIP_DNS_GetIPAddressesNumber(const char* hostName, IP_ADDRESS_TYPE type);

//****************************************************************************
/*  
  Function:
    bool TCPIP_DNS_IsEnabled(CPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DNS client is enabled on that specified interface.

  Description:
    This function returns the current state of DNS Client on the specified interface.

  Precondition:
    The DNS module must be initialized.

  Parameters:
    hNet - Interface to query.

  Returns:
    - true  - if the DNS client service is enabled on the specified interface
    - false - if the DNS client service is not enabled on the specified interface
*/
bool TCPIP_DNS_IsEnabled(TCPIP_NET_HANDLE hNet);

//****************************************************************************
/*
  Function:
    bool TCPIP_DNS_Enable(TCPIP_NET_HANDLE hNet, TCPIP_DNS_ENABLE_FLAGS flags)

  Summary:
    Enables the DNS Client for the specified interface.

  Description:
    This function enables the DNS Client name resolution for the specified interface.
    The additional flags give better control on how the name resolution is performed.

   
  Precondition:
    The DNS module must be initialized.

  Parameters:
    hNet    - Interface to enable the DNS Client on
    flags   - specify further attributes for this interface: act as a strict, preferred or default interface

  Returns:
    - true  - if successful
    - false - if unsuccessful: the requested interface could not be selected for DNS name resolving.

  Remarks:
    The interface selection for the name resolution tries to find a valid interface, i.e. an interface that is up
    and has a valid DNS server.
    The selection is done following these rules:
    - if a strict interface is set, only that interface is used for name resolution
    - else if there is a preferred interface, that one will be tried first
    - else the default interface is used
    - else any available interface will be used
    Additionally, if a retry is attempted using the same selected interface,
    an alternate DNS server from that interface will be selected, if available. 

   Only one strict interface can exist at any time.
   Selecting a new strict interface will replace the old one.
  
   Only one preferred interface can exist at any time.
   Selecting a new preferred interface will replace the old one.

   The selected interface has to be up and running for the call to succeed
*/
bool TCPIP_DNS_Enable(TCPIP_NET_HANDLE hNet, TCPIP_DNS_ENABLE_FLAGS flags);
//*****************************************************************************
/*
  Function:
    bool TCPIP_DNS_Disable(TCPIP_NET_HANDLE hNet, bool clearCache)

  Summary:
    Disables the DNS Client for the specified interface.

  Description:
    This function disables the DNS Client for the specified interface.

  Precondition:
    The DNS module must be initialized.

  Parameters:
    hNet        - Interface for which to disable the DNS Client.
    clearCache  - If true, all the existent name entries will be cleared from the cache


  Returns:
    - true  - if successful
    - false - if unsuccessful

  Remarks:
    When the DNS client is disabled on a requested interface
    the previously solved names will still be part of the cache
    and will expire when their timeout occurs.
    If the TTL for a name sent by the DNS server was ignored and another default/arbitrary
    value was used, then the entry will stay cached until that timeout occurs
    (i.e. timeout not specified by the DNS server).
    To avoid this, you can clear the cache by setting the clearCache parameter to true.

    If the disabled interface matches the strict interface set by TCPIP_DNS_Enable
    this function will set the strict interface to 0.

    If the disabled interface matches the preferred interface set by TCPIP_DNS_Enable
    this function will set the preferred interface to 0.
*/
bool TCPIP_DNS_Disable(TCPIP_NET_HANDLE hNet, bool clearCache);

// *****************************************************************************
/* 
  Function:
    TCPIP_DNS_HANDLE 
    TCPIP_DNS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DNS_EVENT_HANDLER handler, const void* hParam);

  Summary:
    Registers a DNS client Handler.

  Description:
    This function registers a DNS client event handler.
    The DNS client module will call the registered handler when a
    DNS client event (TCPIP_DNS_EVENT_TYPE) occurs.

  Precondition:
    The DNS module must be initialized.

  Parameters:
    hNet    - Interface handle.
              Use hNet == 0 to register on all interfaces available.
    handler - Handler to be called when a DNS client event occurs.
    hParam  - Pointer to non-volatile ASCIIZ string to be used in the handler call.
              It is used as a domain/host name.
              If not NULL, a DNS notification will be delivered
              only for a name resolution that matches the hParam.
              If the hParam == 0, then the notification is triggered
              for any host name resolution. 

  Returns:
    - Returns a valid handle if the call succeeds 
    - Returns null handle if the call failed (out of memory, for example)

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!   
 */
TCPIP_DNS_HANDLE TCPIP_DNS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DNS_EVENT_HANDLER handler, const void* hParam);

// *****************************************************************************
/* Function:
    bool TCPIP_DNS_HandlerDeRegister(TCPIP_DNS_HANDLE hDns);

  Summary:
    Deregisters a previously registered DNS client handler.
    
  Description:
    This function deregisters the DNS client event handler.

  Precondition:
    The DNS module must be initialized.

  Parameters:
    hDns   - A handle returned by a previous call to TCPIP_DNS_HandlerRegister.

  Returns:
   - true  - if the call succeeds
   - false - if no such handler is registered
 */
bool TCPIP_DNS_HandlerDeRegister(TCPIP_DNS_HANDLE hDns);

// *****************************************************************************
/* Function:
   TCPIP_DNS_RESULT TCPIP_DNS_RemoveEntry(const char *hostName)

  Summary:
    Remove a hostname from the DNS Hash entry.

  Description:
    This function is used to remove an entry (host name) from the DNS cache.

  Precondition:
    The DNS module must be initialized.

  Parameters:
    hostName - Domain name to be inserted.
    
  Returns:
    - TCPIP_DNS_RES_OK - If name was successfully removed

    - TCPIP_DNS_RES_INVALID_HOSTNAME - invalid name supplied
    - TCPIP_DNS_RES_NO_SERVICE - DNS resolver non existent/uninitialized.
    - TCPIP_DNS_RES_NO_NAME_ENTRY - no such name exists

  Remarks:
    None.
 */
TCPIP_DNS_RESULT TCPIP_DNS_RemoveEntry(const char *hostName);

// *****************************************************************************
/* Function:
   TCPIP_DNS_RESULT TCPIP_DNS_RemoveAll(void)

  Summary:
    Removes all the cached entries from DNS resolver.

  Description:
    This function is used to remove all the entries from the DNS cache.
    It removes both  the solved and unresolved entries.

  Precondition:
    The DNS module must be initialized.

  Parameters:
    None.

  Returns:
    - TCPIP_DNS_RES_OK - If successful
    - TCPIP_DNS_RES_NO_SERVICE - DNS resolver non existent/uninitialized.

  Remarks:
    None.
 */
TCPIP_DNS_RESULT TCPIP_DNS_RemoveAll(void);

// *****************************************************************************
/* Function:
   TCPIP_DNS_RESULT TCPIP_DNS_EntryQuery(TCPIP_DNS_ENTRY_QUERY *pDnsQuery, int queryIndex);

  Summary:
    Queries a DNS Resolver specific entry.

  Description:
    This function is used to query the DNS client for a specified entry.
    The entry to be queried is selected by its index.

  Precondition:
    The DNS client module must be initialized.

  Parameters:
    pDnsQuery   - address to store the the query result
    queryIndex  - entry index to be selected; should start with 0
    
  Returns:
    - TCPIP_DNS_RES_OK - valid address for this index, successful
    
    Errors:
    - TCPIP_DNS_RES_NO_SERVICE - DNS resolver non existent/uninitialized.
    - TCPIP_DNS_RES_INVALID_HOSTNAME - invalid string, len, pDnsQuery provided
    - TCPIP_DNS_RES_EMPTY_IX_ENTRY - no name associated with this entry
    - TCPIP_DNS_RES_NO_IX_ENTRY - invalid query index

  Remarks:
    None

 */
TCPIP_DNS_RESULT TCPIP_DNS_EntryQuery(TCPIP_DNS_ENTRY_QUERY *pDnsQuery, int queryIndex);

//****************************************************************************
/*  Function:
    TCPIP_DNS_RESULT TCPIP_DNS_Send_Query(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type)

  Summary:
    Forces resolution of an address.

  Description:
    This function attempts to send a query packet for the supplied host name and DNS type.


  Precondition:
    The DNS client module must be initialized.
    
  Parameters:
    hostName   - A pointer to the null terminated string specifying the
                 host for which to resolve an IP.
    type       - a TCPIP_DNS_RESOLVE_TYPE value specifying the desired resolution

  Returns:
    TCPIP_DNS_RES_OK          - success, name is solved.
    TCPIP_DNS_RES_PENDING     - operation is ongoing
    TCPIP_DNS_RES_NAME_IS_IPADDRESS   - name request is a IPv4 or IPv6 address

    or an error code if an error occurred
    
   
  Remarks:
    If the name is already part of the DNS resolution process (has been previously
    requested with TCPIP_DNS_Resolve or TCPIP_DNS_Send_Query) the function
    will force a new DNS query .
    If the name resolution is already solved and completed, this function will mark it
    as incomplete and a new response from the server will be requested.

    If the name was not part of the DNS client resolution,
    then this function is equivalent to TCPIP_DNS_Resolve().

  */
TCPIP_DNS_RESULT TCPIP_DNS_Send_Query(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type);

//****************************************************************************
/*  Function:
    TCPIP_DNS_RESULT TCPIP_DNS_ClientInfoGet(TCPIP_DNS_CLIENT_INFO* pClientInfo)

  Summary:
    Get the current DNS client parameters.

  Description:
    This function is used to get the current settings of the DNS client.

  Precondition:
    The DNS client module must be initialized.

  Parameters:
    pClientInfo     - pointer to a TCPIP_DNS_CLIENT_INFO data structure to receive the DNS client information.

  Returns:
    - TCPIP_DNS_RES_OK on success
    - TCPIP_DNS_RES_NO_SERVICE - DNS resolver non existent/uninitialized.

  Remarks:
    None

*/
TCPIP_DNS_RESULT TCPIP_DNS_ClientInfoGet(TCPIP_DNS_CLIENT_INFO* pClientInfo);

// *****************************************************************************
/*
  Function:
    void  TCPIP_DNS_ClientTask(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs DNS module tasks in the TCP/IP stack.

  Precondition:
    The DNS module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_DNS_ClientTask(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __DNS_H

