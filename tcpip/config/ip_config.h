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
/*
Copyright (C) 2011-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _IP_CONFIG_H_
#define _IP_CONFIG_H_

// If 1, enable IPv4 fragmentation processing, RX + TX
// If 0 (default), IPv4 fragmentation not supported
#define TCPIP_IPV4_FRAGMENTATION        1

// Initial fragment timeout, in seconds
// Timer Lower Bound (RFC 791)
// The RFC states this value to be 15 seconds
#define TCPIP_IPV4_FRAGMENT_TIMEOUT     15

// Upper limit for the number of fragmented streams
// that could be handled simultaneously
#define TCPIP_IPV4_FRAGMENT_MAX_STREAMS     3

// Upper limit for the number of fragments in a 
// fragmented stream
// This also imposes a limit on how large a packet could be
// depending on the MTU and fragmentations that occur
// If more fragments received for a certain stream,
// the stream will be discarded
#define TCPIP_IPV4_FRAGMENT_MAX_NUMBER     4

// This setting enables/disables the processing of the RX packets
// by an external handler
// The user of the IPv4 can register an external function to 
// process the incoming packets
// If true, the functionality is built in and could be used by the application
// If false, the functionality does not exist and the generated code is slightly smaller 
#define TCPIP_IPV4_EXTERN_PACKET_PROCESS true

// enable IPv4 related commands
#define TCPIP_IPV4_COMMANDS                 0

// Enable traffic forwarding at the IPv4 layer
#define TCPIP_IPV4_FORWARDING_ENABLE        1

// Maximum number of entries in the routing table
#define TCPIP_IPV4_FORWARDING_TABLE_MAX_SIZE    10

// Number of entries in the routing table as part of the initialization
#define TCPIP_IPV4_FORWARDING_TABLE_ENTRIES    5

// Enable support for forwarding table initialization using ASCII strings 
#define TCPIP_IPV4_FORWARDING_TABLE_ASCII   1

// Enable support for dynamic forwarding table API
#define TCPIP_IPV4_FORWARDING_DYNAMIC_API     1

// Number of slots in the IPv4 ARP queue
// The number of entries that IPv4 can queue up for ARP resolution.
// Usually it should match the number of total ARP cache entries for all interfaces
#define TCPIP_IPV4_ARP_SLOTS                10

// The IP task processing rate: number of milliseconds to generate an IP tick.
// This is the tick that advances the IP state machine.
// The default value is around 30 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
// The IP state machine tick is used only when fragmentation is enabled.
#define TCPIP_IPV4_TASK_TICK_RATE       37

#endif  // _IP_CONFIG_H_

