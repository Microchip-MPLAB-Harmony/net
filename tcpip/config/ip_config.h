/**********************************************************
  Company:
    Microchip Technology Inc.
	
  File Name:
    ip_config.h
	
  Summary:
    Internet Protocol (IP) Configuration file.
	
  Description:
    This file contains the IP module configuration options.
  **********************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2011 released Microchip Technology Inc.  All rights
reserved.

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


// The IP task processing rate: number of milliseconds to generate an IP tick.
// This is the tick that advances the IP state machine.
// The default value is around 30 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
// The IP state machine tick is used only when fragmentation is enabled.
#define TCPIP_IPV4_TASK_TICK_RATE       37

#endif  // _IP_CONFIG_H_

