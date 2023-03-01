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

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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



