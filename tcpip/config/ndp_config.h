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
/*****************************************************************************
 Copyright (C) 2011-2018 Microchip Technology Inc. and its subsidiaries.

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
