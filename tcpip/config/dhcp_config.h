/*******************************************************************************
  Dynamic Host Configuration Protocol (DCHP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcp_config.h

  Summary:
    DCHP configuration file
    
  Description:
    This file contains the DCHP module configuration options

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
#ifndef _DCHP_CONFIG_H_
#define _DCHP_CONFIG_H_


// Defines how long to wait before a DHCP lease is acquired
// when the DHCP module is enabled, seconds
#define TCPIP_DHCP_TIMEOUT              (10)


// The DHCP task processing rate: number of milliseconds to generate an DHCP tick.
// Used by the DHCP state machine
// The default value is 200 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DHCP_TASK_TICK_RATE        (200)

// Default value for the enable/disable the DHCP client at stack start-up.
// Note: the interface initialization setting in TCPIP_NETWORK_CONFIG takes precedence!
#define TCPIP_DHCP_CLIENT_ENABLED        1


// Maximum size of a host name to be advertised to the DHCP server
// default value is 20
#define TCPIP_DHCP_HOST_NAME_SIZE       20

// enable the usage of the Boot file name received from the DHCP server
#define TCPIP_DHCP_STORE_BOOT_FILE_NAME

// size of the storage for the Boot file name
// should always be <= 128
// default value is 128
#define TCPIP_DHCP_BOOT_FILE_NAME_SIZE      128


// Enable the time server option
// Default is 0
#define TCPIP_DHCP_USE_OPTION_TIME_SERVER       0

// Max number of time server addresses to store
// Should be less than 256!
// Only if TCPIP_DHCP_USE_OPTION_TIME_SERVER is enabled
#define TCPIP_DHCP_TIME_SERVER_ADDRESSES        1

// Enable the NTP server option
// Default is 0
#define TCPIP_DHCP_USE_OPTION_NTP_SERVER       0

// Max number of NTP server addresses to store
// Should be less than 256!
// Only if TCPIP_DHCP_USE_OPTION_NTP_SERVER is enabled
#define TCPIP_DHCP_NTP_SERVER_ADDRESSES        1 

// Iime to wait for an ARP probe when checking a lease, milliseconds
// Default value: 1 second
// Note: if 0, then the ARP probe (required by the standard) is skipped
#define TCPIP_DHCP_ARP_LEASE_CHECK_TMO       1000

// Time to wait before attempting a retry when the ARP validation failed, seconds
// Standard sets it to 10 secs
// Relevant only if TCPIP_DHCP_LEASE_CHECK_TMO != 0
#define TCPIP_DHCP_WAIT_ARP_FAIL_CHECK_TMO   10


#endif  // _DCHP_CONFIG_H_
