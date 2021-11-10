/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCPS) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcps_config.h

  Summary:
    DHCPS configuration file
    
  Description:
    This file contains the DHCPS module configuration options

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2021 Microchip Technology Inc. and its subsidiaries.

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
#ifndef _DHCPS_CONFIG_H_
#define _DHCPS_CONFIG_H_

// DHCPS task processing rate, in milliseconds.
// The DHCPS module will process a timer event with this rate
// for maintaining its own queues, processing timeouts, etc.
// Choose it so that the other TMO are multiple of this
// The default value is 200 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DHCPS_TASK_PROCESS_RATE              (200)


// maximum size of the client identifier field
// needs to be >= size of a MAC Address
// Note: when a client is requesting a lease using a longer client ID 
// the ID will be truncated!
//
#define TCPIP_DHCPS_CLIENT_ID_MAX_SIZE 16


// maximum number of interfaces on which DHCPs could run
// Note that this also the maximum number of interfaces in the stack for this build!
#define TCPIP_DHCPS_MAX_INTERFACES      4

// Maximum number of leases that the DHCP server can support for an interface
// The actual number of leases per interface could be smaller than this
// Multiple of 32
#define TCPIP_DHCPS_MAX_LEASES          64

// size of the buffer when sending a ICMP echo request for address conflict detection
// should be between 8 and 32 bytes
#define TCPIP_DHCPS_ICMP_ECHO_DATA_SIZE 16

// number of retries if ICMP is not able to send a ping
// this should only happen if there are too many ICMP echo requests ongoing
#define TCPIP_DHCPS_ICMP_ECHO_RETRIES   2

// maximum number of values for the options to be sent to clients
// a value defined as 0 excludes that option
#define TCPIP_DHCPS_OPTION_ROUTER_VALUES        1
#define TCPIP_DHCPS_OPTION_DNS_VALUES           2
#define TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES   1
#define TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES   1
#define TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES    1

// suppress the 'renewal T1' and 'rebinding T2' options when replying to a client request
// by default thse options are enabled
// #define TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS

// enable the report of error events using the event notification mechanism
#define TCPIP_DHCPS_REPORT_ERROR_EVENT          1

// enable the report of client events using the event notification mechanism
#define TCPIP_DHCPS_REPORT_CLIENT_EVENT         1

// maximum number of event registrations
// if 0 or not defined, there is no event registration
#define TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS     2

// maintain DHCPs statistics counts
#define TCPIP_DHCPS_ENABLE_STATISTICS           1

// allow dynamic manipulation of the DHCPs DB
#define TCPIP_DHCPS_DYNAMIC_DB_ACCESS           1

// enable multi-threaded access
#define TCPIP_DHCPS_MULTI_THREADED_ACCESS       1



#endif  // _DHCPS_CONFIG_H_



