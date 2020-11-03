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
*******************************************************************************/

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

#ifndef __DHCPS_H
#define __DHCPS_H

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
    TCPIP_DHCPS_ADDRESS_CONFIG

  Summary:
    DHCP server configuration and IP address range.

  Description:
    DHCP server configuration and network initialization data. Configuration
	is part of tcpip_stack_init.c.
*/
typedef struct
{
     int            interfaceIndex;     // Interface Index
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
    bool    enabled;   				    // enable DHCP server
    bool    deleteOldLease;  		    // delete old cache if still in place,
    // specific DHCP parameters
    size_t  leaseEntries;   		    // max number of lease entries
    uint32_t     entrySolvedTmo; 		// solved entry removed after this tmo in seconds
    TCPIP_DHCPS_ADDRESS_CONFIG *dhcpServer; // DHCP server lease address configuration details
   // uint32_t    dhcpServerCnt;        // Max DHCP server support
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
    - true	- If successful
    - false	- If unsuccessful
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
    - true	- If successful
    - false	- If unsuccessful

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
    - true	- If successful
    - false	- If unsuccessful
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
    - true	- If successful
    - false	- If unsuccessful

  Remarks:
    None.
*/

bool TCPIP_DHCPS_RemovePoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type);

//******************************************************************************
/*
 Function:
    int TCPIP_DHCPS_GetPoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type);

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
    - true	- If successful
    - false	- If unsuccessful

  Remarks:
    None.
*/

int TCPIP_DHCPS_GetPoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type);

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
    - true	- If successful
    - false	- If unsuccessful

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

#endif // __DHCPS_H
