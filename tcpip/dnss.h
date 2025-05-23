/*******************************************************************************
  Domain Name System (DNS) server Header file

  Company:
    Microchip Technology Inc.
  
  File Name:
    dnss.h

  Summary:
    DNS server definitions and interface file

  Description:
    This source file contains the DNS Server module API
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef H_DNSS_H_
#define H_DNSS_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
/* 
  Enumeration:
    TCPIP_DNSS_RESOLVE_TYPE

  Summary:
    DNS query record type.

  Description:
    This enumeration lists the RecordType argument for TCPIP_DNSS_Resolve.
    The stack supports DNSS_TYPE_A and DNSS_TYPE_AAAA.
    
  Remarks:
    None.
*/
typedef enum
{

    TCPIP_DNSS_TYPE_A      = 1,        // Indicates an A (standard address) record.
    TCPIP_DNSS_TYPE_MX     = 15,       // Indicates an MX (mail exchanger) record.
    TCPIP_DNSS_TYPE_AAAA   = 28u,      // Indicates a quad-A (IPv6 address) address record.
    TCPIP_DNSS_TYPE_ANY    = 0xff,
}TCPIP_DNSS_RESOLVE_TYPE;

// *****************************************************************************
/* Enumeration:
    TCPIP_DNSS_RESULT

  Summary:
    DNS result codes

  Description:
    DNS Server operations results.

  Remarks:
    None.
*/
typedef enum
{
    // success codes
    TCPIP_DNSS_RES_OK                 = 0,    // operation succeeded

    // failure codes
    TCPIP_DNSS_RES_NO_SERVICE         = -1,   // DNS service not implemented or uninitialized
    TCPIP_DNSS_RES_CACHE_FULL         = -2,   // the cache is full and no entry could be added
    TCPIP_DNSS_RES_NO_ENTRY           = -3,   // DNSS no such name
    TCPIP_DNSS_RES_NO_IPADDRESS       = -4,   // No such IP address        
    TCPIP_DNSS_RES_MEMORY_FAIL        = -5,   // out of memory failure   
    TCPIP_DNSS_RES_DUPLICATE_ENTRY    = -6,   // duplicate entry was found
}TCPIP_DNSS_RESULT;

// *****************************************************************************
/* Enumeration:
    TCPIP_DNSS_RESULT_OBSOLETE

  Summary:
    DNS result codes

  Description:
    DNS Server operations results.

  Remarks:
    This type is obsolete and it will be removed.
 Please use the TCPIP_DNSS_RESULT.
*/
typedef enum
{
    // success codes
    DNSS_RES_OK                 = 0,    // operation succeeded

    // failure codes
    DNSS_RES_NO_SERVICE         = -1,   // DNS service not implemented or uninitialized
    DNSS_RES_CACHE_FULL         = -2,   // the cache is full and no entry could be added
    DNSS_RES_NO_ENTRY           = -3,   // DNSS no such name
    DNSS_RES_NO_IPADDRESS       = -4,   // No such IP address        
    DNSS_RES_MEMORY_FAIL        = -5,   // out of memory failure   
    DNSS_RES_DUPLICATE_ENTRY    = -6,   // duplicate entry was found
}TCPIP_DNSS_RESULT_OBSOLETE;


// *****************************************************************************
/* 
  Structure:
    TCPIP_DNSS_MODULE_CONFIG

  Summary:
    Provides a place holder for DNS server configuration.

  Description:
    DNS Server run-time configuration and initialization parameter.

  Remarks:
    None.
*/
typedef struct
{
    bool    deleteOldLease;         // Delete old cache if still in place,
    bool    replyBoardAddr;         // Reply with board address
    // specific DNS server parameters
    size_t  IPv4EntriesPerDNSName;  // Number of IPv4 entries per DNS name. Default value is 1.
    size_t  IPv6EntriesPerDNSName;  // Number of IPv6 address per DNS Name. Default value is 1
                                    // Used only when IPv6 is enabled
                                
}TCPIP_DNSS_MODULE_CONFIG;


// *****************************************************************************
// *****************************************************************************
// Section: DNS Server Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/*
  Function:
    TCPIP_DNSS_RESULT TCPIP_DNSS_CacheEntryRemove(const char* name, IP_ADDRESS_TYPE type, 
                                                 IP_MULTI_ADDRESS* pAdd)

  Summary:
    Remove DNS server cache entry by Hostname and IP Type and IP(v4 or v6) address

  Description:
    This function is used for command prompt dnsDelSrv and to delete a entry IPv4 or 
    IPv6 from hostName. Every time this is used to delete either one IPv4 or IPv6 address
    with respect to the DNS hostname, there will be an expire time for a Host entry.

  Precondition:
    The DNS server must be initialized.

  Parameters:
    name - Hostname
    type - IP_ADDRESS_TYPE_IPV4 or IP_ADDRESS_TYPE_IPV6
    pAdd - v4 or v6 address

  Returns:
    - TCPIP_DNSS_RES_OK - If entry remove is successful
    - TCPIP_DNSS_RES_NO_ENTRY - No such entry is present
    - TCPIP_DNSS_RES_MEMORY_FAIL - No Memory is present for IPv4 or IPv6 address type
*/
TCPIP_DNSS_RESULT TCPIP_DNSS_CacheEntryRemove(const char* name, IP_ADDRESS_TYPE type, IP_MULTI_ADDRESS* pAdd);


//*****************************************************************************
/*
  Function:
    bool TCPIP_DNSS_IsEnabled(TCPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DNS Server is enabled on the specified interface.

  Description:
    This function returns the current state of the DNS Server on the specified 
    interface.

  Precondition:
    The DNS server must be initialized.

  Parameters:
    hNet- Interface to query.

  Returns:
    - true  - if the DNS Server is enabled on the specified interface
    - false - if the DNS Server is not enabled on the specified interface
*/
bool TCPIP_DNSS_IsEnabled(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    bool TCPIP_DNSS_Enable(TCPIP_NET_HANDLE hNet)

  Summary:
    Enables the DNS server for the specified interface.

  Description:
    This function enables the DNS Server for the specified interface, if it is disabled.
    If it is already enabled, nothing is done.

  Precondition:
    The DNS server must be initialized.

  Parameters:
    hNet - Interface to enable the DNS Server on.

  Returns:
    - true - if successful
    - false - if unsuccessful
*/
bool TCPIP_DNSS_Enable(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    bool TCPIP_DNSS_Disable(TCPIP_NET_HANDLE hNet)

  Summary:
    Disables the DNS Server for the specified interface.

  Description:
    This function disables the DNS Server for the specified interface.
    If it is already disabled, no action is taken.

  Precondition:
    The DNS server must be initialized.

  Parameters:
    hNet - Interface on which to disable the DNS Server.

  Returns:
    - true  - if successful
    - false - if unsuccessful

  Remarks:
    none
*/
bool TCPIP_DNSS_Disable(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    TCPIP_DNSS_RESULT TCPIP_DNSS_EntryAdd(const char* name, IP_ADDRESS_TYPE type, 
                                 IP_MULTI_ADDRESS* pAdd,uint32_t entryTimeout);

  Summary:
    Add a IPv4 or IPv6 entry to the DNS server table.

  Description:
    This function is used to add a IPv4 or IPv6 entry.
    Every time this is used to add either one IPv4 or IPv6 address
    with respect to the DNS hostname, there will be an expire time for a Host entry.
    "entryTimeout" parameter is used as the timeout value for this specific entry.

  Precondition:
    DNS Server Initialized.

  Parameters:
    name           - Hostname
    type           - IP_ADDRESS_TYPE_IPV4 or IP_ADDRESS_TYPE_IPV6
    pAdd           - IPv4 or IPv6 address
    entryTimeout   - Time-out value for the entry
    
  Returns:
    - TCPIP_DNSS_RES_OK - If entry remove is successful
    - TCPIP_DNSS_RES_NO_ENTRY - If Hostname is NULL value and Invalid IP type
    - TCPIP_DNSS_RES_MEMORY_FAIL - No Memory is present for IPv4 or IPv6 address type
    - TCPIP_DNSS_RES_CACHE_FULL - If there is no space for the new entry

  Remarks:
     When the DNS server entryTimeout value is 0,it specifies that the entry 
     is a permanent entry. user can remove this entry by using
     TCPIP_DNSS_CacheEntryRemove(). TTL time for this entry will be the value of 
     TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME.
*/

TCPIP_DNSS_RESULT TCPIP_DNSS_EntryAdd(const char* name, IP_ADDRESS_TYPE type, 
IP_MULTI_ADDRESS* pAdd,uint32_t entryTimeout);

//*****************************************************************************
/*
  Function:
    TCPIP_DNSS_RESULT TCPIP_DNSS_EntryGet(uint8_t* hostName, IP_ADDRESS_TYPE type, size_t index, IP_MULTI_ADDRESS* pGetAdd, uint32_t* ttlTime);

  Summary:
    Get DNS server IP address details from resolver pool entry.

  Description:
    This function is used to get the DNS server IPv4 or IPv6 address from resolver 
    pool entry as per hostname and IP type and from the index value. This IP type 
    can be a IPv4 and IPv6 type. This is used for DNS record type.
    
  Precondition:
    DNSServerInit should be called.

  Parameters:
    hostName - DNS host name
    type     - DNS IP type ( it will be ipv4 or ipv6 type and it is mapped to 
               DNS record type) this can be used for DNS record type
    index    - get the next entry after this index value
    pGetAdd  - get the zero th IPv4 or IPv6 address
    ttlTime  - timeout value
    
  Returns:
    TCPIP_DNSS_RESULT
    
  Remarks:
    None.
*/
TCPIP_DNSS_RESULT TCPIP_DNSS_EntryGet(uint8_t* hostName, IP_ADDRESS_TYPE type, size_t index, IP_MULTI_ADDRESS* pGetAdd, uint32_t* ttlTime);

//*****************************************************************************
/*
  Function:
    TCPIP_DNSS_RESULT TCPIP_DNSS_AddressCntGet(size_t index, char* hostName, size_t hostSize, size_t* ipCount);

  Summary:
    Get DNS Server IPv4 or IPv4 Address count details for the Input value of Index

  Description:
    This function is used to get the number of IPv4 and IPv6 address entry details
    from hash entry table. Here input parameter is index value. Output will be the 
    hostname and IP address count.
    IP address count is the summation of both IPv4 and IPv6 address.
 
  Precondition:
    DNSServerInit should be called.

  Parameters:
    index       - index values to be used
    hostName    - buffer to hold DNS host name
    hostSize    - size of the buffer
    ipCount     - the number of IPv4 or IPv6 Server addresses
    
  Returns:
    TCPIP_DNSS_RESULT
    
  Remarks:
    None.
*/
TCPIP_DNSS_RESULT TCPIP_DNSS_AddressCntGet(size_t index, char* hostName, size_t hostSize, size_t* ipCount);

// *****************************************************************************
/*
  Function:
    void  TCPIP_DNSS_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs DNSS module tasks in the TCP/IP stack.

  Precondition:
    The DNSS module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_DNSS_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // H_DNSS_H_
