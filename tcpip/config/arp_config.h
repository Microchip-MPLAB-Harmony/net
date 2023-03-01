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

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2011-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


// ARP task processing rate, in milliseconds.
// The ARP module will process a timer event with this rate
// for maintaining its own queues, processing timeouts, etc.
// Choose it so that the other ARP_CACHE_xxx_TMO are multiple of this
// The default value is 2 seconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_ARP_TASK_PROCESS_RATE              (2000)


// ARP caches configuration for aliases.
// If this symbol is true, then the alias interfaces
// share the cache with the primary interface.
// Otherwise each interface uses its own cache.
// Default value should be 1 if the alias interfaces
// are in the same network with the primary interface
#define TCPIP_ARP_PRIMARY_CACHE_ONLY        1

#endif  // _ARP_CONFIG_H_



