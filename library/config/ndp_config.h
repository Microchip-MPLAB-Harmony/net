/*******************************************************************************
  Neighbor Discovery Protocol (NDP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    ndp_config.h

  Summary:
    NDP configuration file
    
  Description:
    This file contains the NDP module configuration options
    
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
#ifndef _NDP_CONFIG_H_
#define _NDP_CONFIG_H_


// Neighbor Discovery Host constants
#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATION_DELAY              1u              // 1 s
#define TCPIP_IPV6_NDP_RTR_SOLICITATION_INTERVAL               4u              // 4 s
#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATIONS                   3u              // 3 transmissions

// Neighbor Discovery Node constants
#define TCPIP_IPV6_NDP_MAX_MULTICAST_SOLICIT                   3u              // 3 transmissions
#define TCPIP_IPV6_NDP_MAX_UNICAST_SOLICIT                     3u              // 3 transmissions
#define TCPIP_IPV6_NDP_MAX_ANYCAST_DELAY_TIME                  1u              // 1 s
#define TCPIP_IPV6_NDP_MAX_NEIGHBOR_ADVERTISEMENT              3u              // 3 transmissions
#define TCPIP_IPV6_NDP_REACHABLE_TIME                          30u             // 30 s
#define TCPIP_IPV6_NDP_RETRANS_TIMER                           1u              // 1 s
#define TCPIP_IPV6_NDP_DELAY_FIRST_PROBE_TIME                  5u              // 5 s

// Sets the lifetime to 2 hours
#define TCPIP_IPV6_NDP_VALID_LIFETIME_TWO_HOURS            (60 * 60 * 2)

// Sets the maximum transmit unit increase timeout in seconds
#define TCPIP_IPV6_MTU_INCREASE_TIMEOUT               600ul           // 600 seconds

// The NDP task rate, milliseconds
// The default value is 32 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_IPV6_NDP_TASK_TIMER_RATE                     (32)



#endif  // _NDP_CONFIG_H_
