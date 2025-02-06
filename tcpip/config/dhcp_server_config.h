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
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
// Note that this could be less than the maximum number of interfaces
// in the stack for this build (TCPIP_STACK_NETWORK_INTERAFCE_COUNT)!
#define TCPIP_DHCPS_INTERFACE_COUNT      2

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
// by default these options are enabled
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



