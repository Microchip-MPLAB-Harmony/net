/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCP) Server API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcps.h

  Summary:
    Dynamic Host Configuration Protocol(DHCP) Server APIs.

  Description:
     The DHCP server assigns a free IP address to a requesting 
     client from the range defined in the dhcps_config.h  file.
     Lease time per IP address is decided as per the TMO configuration 
     which is defined in dhcps_config.h.

  Note:
    The dhcps module is obsolete. The new dhcp_server module should be used!
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

#ifndef H_DHCPS_H_
#define H_DHCPS_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// NOTE: The dhcps module is deprecated and should not be used for new projects. The new dhcp_server module should be used.
//
// #warning "The dhcps module is obsolete. The new dhcp_server module should be used!"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_ADDRESS_CONFIG

  Summary:
    DHCP server configuration and IP address range.

  Description:
    DHCP server configuration and network initialization data. Configuration
    is part of tcpip_stack_init.c.
*/
typedef struct
{
     size_t         interfaceIndex;     // Interface Index
     uint32_t       poolIndex;          // Server Pool Index
     const char*    serverIPAddress;    // Server IP address
     const char*    startIPAddRange;    // Start IP address 
     const char*    ipMaskAddress;      // Netmask
     const char*    priDNS;             // Primary DNS server Address
     const char*    secondDNS;          // Secondary DNS server Address
     bool           poolEnabled;        // true if pool is valid , false if pool is invalid
}TCPIP_DHCPS_ADDRESS_CONFIG;

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_MODULE_CONFIG

  Summary:
    DHCP Server module runtime and initialization configuration data.

  Description:
    DHCP server configuration and initialization data . Configuration
    is part of tcpip_stack_init.c.
*/
typedef struct
{
    bool    enabled;                    // enable DHCP server
    bool    deleteOldLease;             // delete old cache if still in place,
    uint32_t dhcpServerCnt;             // Maximum number of DHCP server entries to be supported
    // specific DHCP parameters
    size_t  leaseEntries;               // max number of lease entries
    uint32_t     entrySolvedTmo;        // solved entry removed after this tmo in seconds
    TCPIP_DHCPS_ADDRESS_CONFIG *dhcpServer; // DHCP server lease address configuration details
    
} TCPIP_DHCPS_MODULE_CONFIG;

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_LEASE_ENTRY

  Summary:
    DHCP Server module lease data.

  Description:
    DHCP server lease data
*/
typedef struct
{
    TCPIP_MAC_ADDR    hwAdd; // Client MAC address
    IPV4_ADDR   ipAddress;   // Leased IP address
    uint32_t    leaseTime;   // Lease period
}TCPIP_DHCPS_LEASE_ENTRY;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_DHCPS_POOL_ENTRY_TYPE

  Summary:
    DHCP server pool types are used to get and remove the leased IP address details.

  Description:
    DHCP_SERVER_POOL_ENTRY_ALL -  Get or Remove all the leased address which includes 
                                  both solved and unsolved entries.
    DHCP_SERVER_POOL_ENTRY_IN_USE - Get or Remove only solved leased IP address.
*/

typedef enum
{
    DHCP_SERVER_POOL_ENTRY_ALL,    // Get or Remove all the Leased address
    DHCP_SERVER_POOL_ENTRY_IN_USE, // Get or remove only Leased IP address
}TCPIP_DHCPS_POOL_ENTRY_TYPE;

// *****************************************************************************
/*
  Type:
    TCPIP_DHCPS_LEASE_HANDLE

  Summary:
    DHCP Server Lease Handle 

  Description:
    A handle that server is using to provide the index of a lease entry. 

  Remarks:
    This handle is used by command handler to get the Index of Lease entry.
*/
typedef const void* TCPIP_DHCPS_LEASE_HANDLE;

// *****************************************************************************
// *****************************************************************************
// Section: DHCP Server Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DHCP Server is enabled on the specified interface.

  Description:
    This function returns the current state of the DHCP Server on the specified 
    interface.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    hNet - Interface to query

  Returns:
    - true  - If successful
    - false - If unsuccessful
*/
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet)

  Summary:
    Disables the DHCP Server for the specified interface.

  Description:
    This function disables the DHCP Server for the specified interface.
    If it is already disabled, no action is taken.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    hNet - Interface on which to disable the DHCP Server

  Returns:
    - true  - If successful
    - false - If unsuccessful

  Remarks:
    When the interface continues using its old configuration, it is possible
    that the lease may take sometime to expire. And The communication will 
    be there until it is not expired.Lease time is configured in dhcps_config.h.
 */
bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    void TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet)

  Summary:
    Enables the DHCP Server for the specified interface.

  Description:
    This function enables the DHCP Server for the specified interface, if it is 
    disabled. If it is already enabled, nothing is done.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    hNet - Interface on which to enable the DHCP Server

  Returns:
    - true  - If successful
    - false - If unsuccessful
*/
bool TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    TCPIP_DHCPS_LEASE_HANDLE  TCPIP_DHCPS_LeaseEntryGet(TCPIP_NET_HANDLE netH, 
        TCPIP_DHCPS_LEASE_ENTRY* pLeaseEntry, TCPIP_DHCPS_LEASE_HANDLE leaseHandle);
        
  Summary:
    Get the lease entry details as per TCPIP_DHCPS_LEASE_HANDLE and per interface.

  Description:
    This function returns a lease entry for the  TCPIP_DHCPS_LEASE_HANDLE. if the 
    lease entry is not present for that TCPIP_DHCPS_LEASE_HANDLE, then it will 
    return the next valid lease entry.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    netH - Lease entry for this Interface
    pLeaseEntry - Client lease entry details
    leaseHandle - Lease index 

  Returns:
    - non-zero TCPIP_DHCPS_LEASE_HANDLE - To be used in the subsequent calls 
    - 0 - If end of list or wrong interface, or DHCP server is not running on that interface
*/
TCPIP_DHCPS_LEASE_HANDLE  TCPIP_DHCPS_LeaseEntryGet(TCPIP_NET_HANDLE netH, 
     TCPIP_DHCPS_LEASE_ENTRY* pLeaseEntry, TCPIP_DHCPS_LEASE_HANDLE leaseHandle);

//******************************************************************************
/*
 Function:
    bool TCPIP_DHCPS_RemovePoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type);

  Summary:
    Removes all the entries or only used entries of a certain type belonging to 
    a network interface.

  Description:
    This function is used to remove the DHCP server entries from the pool as per 
    TCPIP_DHCPS_POOL_ENTRY_TYPE. 
    
  Precondition:
    the DHCP Server module should have been initialized.

  Parameters:
    hNet    -   Interface handle to use
    type    -   type of entries to remove:
                DHCP_SERVER_POOL_ENTRY_ALL,
                DHCP_SERVER_POOL_ENTRY_IN_USE

  Returns:
    - true  - If successful
    - false - If unsuccessful

  Remarks:
    None.
*/

bool TCPIP_DHCPS_RemovePoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type);

//******************************************************************************
/*
 Function:
    size_t TCPIP_DHCPS_GetPoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type);

  Summary:
    Get all the entries or only used entries of a certain type belonging to a 
    network interface.

  Description:
    This function is used to get the DHCP server entries from the pool as per 
    TCPIP_DHCPS_POOL_ENTRY_TYPE. 
    
  Precondition:
    The DHCP server module should have been initialized.

  Parameters:
    hNet    -   Interface handle to use
    type    -   type of entries to remove:
                * DHCP_SERVER_POOL_ENTRY_ALL,
                * DHCP_SERVER_POOL_ENTRY_IN_USE

  Returns:
    - the number oF pool entries
    - 0 - If unsuccessful

  Remarks:
    None.
*/

size_t TCPIP_DHCPS_GetPoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type);

//******************************************************************************
/*
 Function:
    bool TCPIP_DHCPS_LeaseEntryRemove(TCPIP_NET_HANDLE netH, TCPIP_MAC_ADDR* hwAdd)

  Summary:
    Remove one entry from the DHCP server leased entry.

  Description:
    This function is used to remove one entry from the leased HASH table with 
    respect to the interface and the MAC address.

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    netH    -   Interface handle to use
    hwAdd   -   MAC address that need to be removed from the HASH table

  Returns:
    - true  - If successful
    - false - If unsuccessful

  Remarks:
     This function is called from the command line to remove one entry and from 
     the Wi-FI Driver module to remove a node that is disconnected from the AP.
*/

bool TCPIP_DHCPS_LeaseEntryRemove(TCPIP_NET_HANDLE netH, TCPIP_MAC_ADDR* hwAdd);

// *****************************************************************************
/*
  Function:
    void  TCPIP_DHCPS_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs DHCP Server module tasks in the TCP/IP stack.

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_DHCPS_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // H_DHCPS_H_
