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
