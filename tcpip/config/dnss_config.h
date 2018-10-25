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
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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



