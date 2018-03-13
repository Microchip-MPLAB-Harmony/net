/*******************************************************************************
  Internet Management Protocol (IGMP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    igmp_config.h

  Summary:
    IGMP Configuration file
    
  Description:
    This file contains the IGMP module configuration options
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2016 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _IGMP_CONFIG_H_
#define _IGMP_CONFIG_H_

// This symbol enables the IGMP module to be part of the stack
#define TCPIP_STACK_USE_IGMP

// the maximum number of interfaces that support IGMP
// Should be <= interfaces that are present at the stack initialization
#define TCPIP_IGMP_INTERFACES           1

// maximum number of multicast groups (addresses) for which
// a IGMP application can subscribe
// Each group can listen to multiple sources
// Should be prime number for best performance
#define TCPIP_IGMP_MCAST_GROUPS         7

// If this symbol is !0, then only Join and Leave operations
// are suported, without support for source addresses required by IGMPv3
// This results in smaller code and less RAM needed by the IGMP module.  
#define TCPIP_IGMPV2_SUPPORT_ONLY       0

// maximum number of unicast sources per group
// a IGMP application can listen to
// Each unicast source can be used by multiple sockets
// Should be prime number for best performance
// Note: TCPIP_IGMP_ASM_ALL_SOURCES may take a slot!
// Note: Should be <= 32
// Note: if TCPIP_IGMPV2_SUPPORT_ONLY is enabled then this value is ignored 
#define TCPIP_IGMP_SOURCES_PER_GROUP    11

// maximum number of socket records that are part
// of a IGMP source unicast address for a specific group G
// Note that same socket may need multiple records
// if it listens on multiple interfaces!
// When operating in IGMPv2 style, it represents the total number of sockets
// that can join/leave a specific group.
#define TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE    4


// The default Robustness Variable value
// The Robustness Variable allows tuning for the expected packet loss on a network.
// If a network is expected to be lossy, the Robustness Variable may be increased.
// IGMP is robust to (Robustness Variable - 1) packet losses.
// RFC 3376 IGMPv3: The Robustness Variable MUST NOT be zero, and SHOULD NOT be one. Default: 2
// This is just a default value. The actual value is taken from a router query.
#define TCPIP_IGMP_ROBUSTNESS_VARIABLE      2

// The default Unsolicited Report Interval, milliseconds
// The Unsolicited Report Interval is the time between repetitions of a
// host’s initial report of membership in a group.
// RFC 3376 IGMPv3: Default: 1 second.
#define TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL  1000


// enable/disable user notification functions
#define TCPIP_IGMP_USER_NOTIFICATION       1

// The IGMP task processing rate: number of milliseconds to generate an IGMP tick.
// This is the tick that advances the IGMP state machine.
// The default value is 33 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_IGMP_TASK_TICK_RATE       33


#endif  // _IGMP_CONFIG_H_
