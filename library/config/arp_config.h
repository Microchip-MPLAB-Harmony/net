/*******************************************************************************
  Address Resolution Protocol (ARP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    arp_config.h

  Summary:
    ARP configuration file
    
  Description:
    This file contains the ARP module configuration options
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2011 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _ARP_CONFIG_H_
#define _ARP_CONFIG_H_


// configuration options

// Number of entries in the cache.
// Default number of entries per interface.
#define TCPIP_ARP_CACHE_ENTRIES       5

// Timeout for a solved entry in the cache, in seconds.
// The entry will be removed if the tmo elapsed
// and the entry has not been referenced again
#define TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO      (20 * 60)

// Timeout for a cache entry pending to be solved, in seconds.
// The entry will be removed if the tmo elapsed
// and the entry has not been solved.
// A solved entry moves to the solved entries timeout.
#define TCPIP_ARP_CACHE_PENDING_ENTRY_TMO      (1 * 60)

// Timeout for resending an ARP request for a pending entry.
// In order to prevent the ARP flooding the standard recommends
// it to be greater than 1 sec.
// It should be less than TCPIP_ARP_CACHE_PENDING_ENTRY_TMO  
#define TCPIP_ARP_CACHE_PENDING_RETRY_TMO       (2)


// Max percentage of permanent entries in the cache.
// Note that since permanent entries cannot be removed
// they tend to degrade the efficiency of the cache
// look up.  
#define TCPIP_ARP_CACHE_PERMANENT_QUOTA         50


// Default purge threshold, percentage
// Once the number of resolved entries in the cache gets
// beyond the threshold some resolved entries will be purged.
#define TCPIP_ARP_CACHE_PURGE_THRESHOLD         75

// The number of entries to delete, once the threshold is reached.
#define TCPIP_ARP_CACHE_PURGE_QUANTA            3

// Number of ARP requests generated for resolving an entry.
#define TCPIP_ARP_CACHE_ENTRY_RETRIES           3

// Number of ARP requests generated when sending a gratuitous ARP probe.
// Default value should be 1.
#define TCPIP_ARP_GRATUITOUS_PROBE_COUNT        1

// On initialization, delete the old cache if still in place
// Else don't re-initialize
// Default should be 1
#define ARP_CACHE_DELETE_OLD               1


// ARP task processing rate, in seconds.
// The ARP module will process a timer event with this rate
// for maintaining its own queues, processing timeouts, etc.
// Choose it so that the other ARP_CACHE_xxx_TMO are multiple of this
// The default value is 2 seconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_ARP_TASK_PROCESS_RATE              (2)


// ARP caches configuration for aliases.
// If this symbol is true, then the alias interfaces
// share the cache with the primary interface.
// Otherwise each interface uses its own cache.
// Default value should be 1 if the alias interfaces
// are in the same network with the primary interface
#define TCPIP_ARP_PRIMARY_CACHE_ONLY        1

#endif  // _ARP_CONFIG_H_



