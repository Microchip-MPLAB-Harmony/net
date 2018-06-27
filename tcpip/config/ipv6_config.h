/*******************************************************************************
  Internet Control Message Protocol for IPv6 (ICMPv6) Configuration file
  
  Company:
    Microchip Technology Inc.
	
  File Name:
    ipv6_config.h

  Summary:
    IPv6 configuration file
    
  Description:
    This file contains the IPv6 module configuration options
    
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

#ifndef _IPv6_CONFIG_H_
#define _IPv6_CONFIG_H_


// Sets the minimum allocation unit for the payload size
#define TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE    (64u)

// Sets the lower bounds of the Maximum Transmission Unit
#define TCPIP_IPV6_MINIMUM_LINK_MTU               1280u

// Default Maximum Transmission Unit
#define TCPIP_IPV6_DEFAULT_LINK_MTU               1500u

// IPv4 Time-to-Live parameter
#define TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT          64u

// Default 30 seconds , Router advertisement reachable time
#define TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME    30u

// 1 second, Process the router advertisement's retransmission time
#define TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME        1000u

// This option defines the maximum number of queued packets per remote.
// If an additional packet needs to be queued, the oldest packet in the queue will be removed.
#define TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT      1

// Time-out of stale neighbor discovery packets.
// 0u will cause packets to persist indefinitely.
#define TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT         600ul           // 10 minutes

// This option defines the maximum number of multicast queued IPv6
// If an additional packet is queued, the oldest packet in the queue will be removed.
#define TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT         4

// This option defines the number of seconds an IPv6 multicast packet will remain in the
// queue before being timed out
#define TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT      10u

// IPv6 task processing rate, milliseconds
// The default value is 1000 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_IPV6_TASK_PROCESS_RATE              (1000)

// IPv6 initialize task processing rate, milliseconds
// The default value is 32 milliseconds.
#define TCPIP_IPV6_INIT_TASK_PROCESS_RATE           (32)

// NTP access time-out for the IPv6 ULA address generation, ms
#define TCPIP_IPV6_ULA_NTP_ACCESS_TMO               (12000)

// the NTP time stamp validity window, ms
// if a stamp was obtained outside this interval from the moment of the request
// a new request will be issued
#define TCPIP_IPV6_ULA_NTP_VALID_WINDOW           (1000)

// Fragmentation packet time-out value. default value is 60 .
#define TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT      60


// RX fragmented buffer size should be equal to the total original packet size of  ICMPv6 ECHO request packets .
// ex - Let Transmit ICMPv6 Echo request packet whose original packet size is 1500byte from the Global address of HOST1 to the
// global address of HOST2 and if the packet is going to be fragmented then packet will be broken more than packets.
// Each packet will have IPv6 header(40 bytes)+ Fragmentation header ( 8 bytes) + ICMPv6 Echo request header(8 bytes)
// + Payload (data packet).PING6(1500=40+8+8+1452 bytes). Here data packet size is 1452. If the data packet size is getting changed
// then this following macro should be rectified to get proper ICMPv6 ECHO response.
// This is the Maximum RX fragmented buffer size.
#define TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE 1514


#endif  // _IPv6_CONFIG_H_
