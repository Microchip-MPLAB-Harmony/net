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



