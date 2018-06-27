/*******************************************************************************
  Domain Name System Server (DNSS) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    dnss_config.h

  Summary:
    DNSS configuration file
    
  Description:
    This file contains the DNSS module configuration options
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _DNSS_CONFIG_H_
#define _DNSS_CONFIG_H_

// Default DNS host name length 
#define TCPIP_DNSS_HOST_NAME_LEN               64u

// Reply DNS info with Board info only if the requested DNS host name is not present
// if TCPIP_DNSS_REPLY_BOARD_ADDR != 1 , then return no such name
// This is used for a boolean variable . the value should be 0 or 1
#define TCPIP_DNSS_REPLY_BOARD_ADDR            1

// Maximum and default number of IPv4 entries will be allowed to be configured from command prompt.
// and these many entries will be allowed to be sent in response for the DNS query with record type TCPIP_DNS_TYPE_A.
#define TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS       2

// Maximum and default number of IPv6 entries will be allowed to be configured from command prompt.
// and these many entries will be allowed to be sent in response for the DNS query with record type TCPIP_DNS_TYPE_AAAA.
#define TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS       1

// Maximum DNS server Cache entries. It is the sum of TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS and 
// TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS.
#define TCPIP_DNSS_CACHE_MAX_SERVER_ENTRIES     (TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS+TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS)

// Default TTL time for a IP address is 10 minutes
#define TCPIP_DNSS_TTL_TIME                     (10*60)

// DNS Server time out task processing rate, in milliseconds.
// The DNS Server module will process a timer event with this rate
// for processing its own state machine, etc.
// The default value is 33 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DNSS_TASK_PROCESS_RATE	(33)



#endif  // _DNSS_CONFIG_H_



