/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCPS) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcps_config.h

  Summary:
    DHCPS configuration file
    
  Description:
    This file contains the DHCPS module configuration options
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END
#ifndef _DHCPS_CONFIG_H_
#define _DHCPS_CONFIG_H_

// DHCPS task processing rate, in milliseconds.
// The DHCPS module will process a timer event with this rate
// for maintaining its own queues, processing timeouts, etc.
// Choose it so that the other TMO are multiple of this
// The default value is 200 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DHCPS_TASK_PROCESS_RATE              (200)

// The Maximum Number of entries in the lease table
// Default total number of entries for all the the interface
#define TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT       15

// Timeout for a solved entry in the cache, in seconds.
// The entry will be removed if the TMO lapsed
// and the entry has not been referenced again
#define TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO      (20 * 60)

// Timeout for a unsolved entry , in seconds and 
//should be removed from the entry if there is no REQUEST after OFFER 
#define TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK		(5)

// Timeout for a solved entry in the cache, in seconds
// the entry will be removed if the TMO elapsed
// and the entry has not been referenced again
#define TCPIP_DHCPS_LEASE_DURATION	TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO

// These below IPv4 DHCP server address details are default address and it is assigned to the
// network default network interface. for Other interfaces , tcpip_stack_init.c file
// should be use to configure DHCP_POOL_CONFIG[].
// IPv4 Address range is starting from 100, because the from 1 to 100 is reserved. 
// Reserved Address will be used for the gateway address.
//  Start of IP address Range , network_config.h ipaddress and this start of IP address should be in same SUBNET
// RECOMENDED - network_config.h ipaddress should be 192.168.1.1 if DHCP server ip address range starts
// from 192.168.1.100.
#define TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START	"192.168.1.100"

// DHCP server Address per interface. DHCP server Address selection should  be in the same subnet.
#define TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS	"192.168.1.1"

// DHCP server subnet Address  per interface. 															
#define TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS	"255.255.255.0"

// DHCP server  DNS primary Address
#define TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS	"192.168.1.1"

// DHCP server  DNS Secondary Address
#define TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS   "192.168.1.1"

#endif  // _DHCPS_CONFIG_H_



