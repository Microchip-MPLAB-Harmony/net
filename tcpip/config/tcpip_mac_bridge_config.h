/**********************************************************
  Company:
    Microchip Technology Inc.
	
  File Name:
    ip_config.h
	
  Summary:
    Internet Protocol (IP) Configuration file.
	
  Description:
    This file contains the IP module configuration options.

    This file is not part of the project, it is a configuration template file only. 
  **********************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _TCPIP_MAC_BRIDGE_CONFIG_H_
#define _TCPIP_MAC_BRIDGE_CONFIG_H_


// defining this symbol will include the MAC bridge to the stack build
#define TCPIP_STACK_USE_MAC_BRIDGE


// number of entries in the filtering database (FDB)
// depends on the traffic to be handled, how many hosts on that network
// For best performance the number of entries should be prime
// Also, keep in mind that the performance degrades when the load factor gets >= 0.7
// Adjust accordingly!
// Notes:
//  - the host own interfaces/ports have their MAC addresses automatically added to the FDB
//    The FDB has to be big enough to allow this, otherwise the bridge operation will result in error
//  - by default packets with destination for these internal ports
//    are excluded from the bridge forwarding
//  - setting can be overridden by the initialization
#define TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES      17

// timeout for an entry to be purged from the table, in seconds
// the entry will be removed if it has been inactive for this long
// Recommended default is 300 seconds
#define TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT          300

// the maximum transit delay of a packet within the bridge
// packet will be dropped if not completely forwarded within this time
// Recommended default is 1 second
// Maximum suggested value is 4 seconds
#define TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY      1

// Maximum number of ports supported by the bridge
// Needed for data allocation, etc.
// Corresponds to the maximum number of interfaces that are bridged
// It should normally match the TCPIP_MAC_BRIDGE_CONFIG::bridgeTableSize
// Maximum supported value is currently 32
#define TCPIP_MAC_BRIDGE_MAX_PORTS_NO           2 

// enable the MAC bridge statistics 
// The bridge module will keep counts of the forwarded or rejected packets, etc.
// See the TCPIP_MAC_BRIDGE_STAT data structure 
#define TCPIP_MAC_BRIDGE_STATISTICS             1 

// enable the MAC bridge event notification 
// Allows the registration of an event handler
// to be notified of bridge events
// See TCPIP_MAC_BRIDGE_EVENT, TCPIP_MAC_BRIDGE_EVENT_HANDLER 
#define TCPIP_MAC_BRIDGE_EVENT_NOTIFY           1 

// number of packets in the pool for run time forwarding
// this pool is allocated at the bridge initialization time
// and then replenished when needed
// if 0, then there's no pool and allocation occurs as needed
// at run time, each packet being allocated exactly the needed size
// adjust based on the network traffic
// Note: the packet pool is used only for packets that need
//      to be processed by the host too.
//      Otherwise the packets are directly forwarded to the destination ports
#define TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE       8

// size of the packets in the packet pool
// Note that incoming packets that are larger than this size will be dropped
// Relevant only when TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE != 0
#define TCPIP_MAC_BRIDGE_PACKET_SIZE    1536 

// Number of packets to replenish the pool, when it becomes empty
// Relevant only when TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE != 0
// When running out of packets a new allocation 
// of this number of packets will be attempted 
#define TCPIP_MAC_BRIDGE_PACKET_POOL_REPLENISH      2

// number of descriptors in the pool for run time forwarding
// this pool is allocated at the bridge initialization time
// and then replenished when needed
// Each packet to be forwarded needs an descriptor
// Could be greater than the TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE 
#define TCPIP_MAC_BRIDGE_DCPT_POOL_SIZE         16

// Number of descriptors to replenish the pool, when it becomes empty
// Extra allocation of this number of descriptors will be attempted
#define TCPIP_MAC_BRIDGE_DCPT_POOL_REPLENISH    4

// TCPIP MAC bridge task rate, ms
// the bridge uses its task for maintenance and clean up
// This is the period to run the bridge task
// The default value is around 300 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_MAC_BRIDGE_TASK_RATE          333

// Enable run time access to the FDB
// If enabled dynamic FDB access API is enabled
// Otherwise the API doe not exist
// Default is disabled
//
// Note that this option is expensive because
// it requires synchronization when accessing the FDB from user threads
// and can interfere with the packet bridge dispatch process
#define TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS     0


// enable the MAC bridge commands
// commands to se the contents of the FDB, to add static entries, etc.
#define TCPIP_STACK_MAC_BRIDGE_COMMANDS     1

// when the 'glue' is enabled, the bridge host ports/interfaces can be reached from each bridge port
// i.e. you can ping bridge port j while connected to port i.
// However, the IP addresses of the 2 ports should be part of the same network for this to work
// Default is disabled, so this symbol should be undefined/false
//#define TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS      0


#endif  // _TCPIP_MAC_BRIDGE_CONFIG_H_

