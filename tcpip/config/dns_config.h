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

// Retry lookup timeout for a unsolved entry in the cache, in seconds.
// If the TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO seconds elapsed and the name
// has not been solved then the resolution will be retried.
// Note: currently the stack will make 2 retries per interface
// using both DNS1 and DNS2 to solve the name
// Default value: 1 - 2 seconds
#define TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO      (2)

// Max DNS host name size
// Use an appropriate value, depending on the names that need to be solved
#define TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN       (64)

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



