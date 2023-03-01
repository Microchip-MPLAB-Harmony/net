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

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
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



