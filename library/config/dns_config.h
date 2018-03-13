/*******************************************************************************
  Domain Name Service (CNS) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    dns_config.h

  Summary:
    DNS configuration file
    
  Description:
    This file contains the DNS module configuration options
    
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
#ifndef _DNS_CONFIG_H_
#define _DNS_CONFIG_H_


// When the DNS Client connected to the DNS Server
// this is the elapsed time after which an the communication is considered
// to have timed failed if there was no reply from the server
// In seconds
#define TCPIP_DNS_CLIENT_SERVER_TMO		(1*60)

// DNS Client task processing rate, in milliseconds.
// The DNS Client module will process a timer event with this rate
// for processing its own state machine, etc.
// The default value is 200 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DNS_CLIENT_TASK_PROCESS_RATE	(200)

// Number of DNS resolver entries 
#define TCPIP_DNS_CLIENT_CACHE_ENTRIES    5

// DNS client cache entry time-out.
// If this symbol is zero then the entry time-out will be the one specified by the DNS server
// when the name was solved.
// Otherwise this value will be used as the cache entry time-out.
// Default should be 0.
#define TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO 0

// Maximum and default number of IPv4 answers to be considered 
// while processing DNS response from server for a query.
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS       5

// Maximum and default number of IPv6 answers to be considered 
// while processing DNS response from server for a query.
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS       1

// This parameter can be used to choose ithe type of IP connection
// for the DNS client: IPv4 or IPv6. 
// Currently only IPv4 is supported and this parameter is not used.
// Reserved for future development 
#define TCPIP_DNS_CLIENT_ADDRESS_TYPE       IP_ADDRESS_TYPE_IPV4

// Default TTL time  for a solved entry in the cache
// This value will be used when the DNS server TTL value for an entry is 0
#define TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL      (20 * 60)

// Time-out for the a unsolved name, in seconds.
// The name resolution will be aborted if the TMO elapsed
// and the name could not be solved
// Should be greater than TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO
#define TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO      (10)

// Retry lookup for a unsolved entry in the cache, in seconds.
// If the TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO seconds elapsed and the name
// has not been solved then the name entry will be marked with server timeout and
// the resolution will be retried.
// Should be less than TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO
#define TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO      (3)

// Max DNS host name size
#define TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN  32

// allow DNS client user notification
// if enabled, the TCPIP_DNS_HandlerRegister/TCPIP_DNS_HandlerDeRegister
// functions exist and can be used
#define TCPIP_DNS_CLIENT_USER_NOTIFICATION           false 

// Max number of interfaces to take part in the DNS selection algorithm
// Should be always greater than 1:
//      - the default interface should always be considered for DNS resolution
// Depending on how many active interfaces select those to be considered
// for DNS resolution 
#define TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES     4


#endif  // _DNS_CONFIG_H_



