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

