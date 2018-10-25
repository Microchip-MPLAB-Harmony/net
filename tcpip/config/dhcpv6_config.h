/*******************************************************************************
  IPv6 Dynamic Host Configuration Protocol (DCHPv6) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcpv6_config.h

  Summary:
    DCHPv6 configuration file
    
  Description:
    This file contains the DCHPv6 module configuration options
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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
#ifndef _DCHPV6_CONFIG_H_
#define _DCHPV6_CONFIG_H_

// This symbol enables the DHCPv6 client as being part of the build
#define TCPIP_STACK_USE_DHCPV6_CLIENT

// Clients listen for DHCP messages on UDP port:
#define TCPIP_DHCPV6_CLIENT_PORT       546

// Servers and relay agents listen for DHCP messages on UDP port:
#define TCPIP_DHCPV6_SERVER_PORT       547

// minimum size of a UDP buffer
#define TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE 512

// Default DUID type to be used by the client
#define TCPIP_DHCPV6_CLIENT_DUID_TYPE   TCPIP_DHCPV6_DUID_TYPE_LL


// maximum number of IANA descriptors per client
#define TCPIP_DHCPV6_IANA_DESCRIPTORS_NO        4

// maximum number of IATA descriptors per client
#define TCPIP_DHCPV6_IATA_DESCRIPTORS_NO        2

// maximum number of free IA descriptors per client
#define TCPIP_DHCPV6_IA_FREE_DESCRIPTORS_NO        2



// default values for IANA T1, T2 
#define TCPIP_DHCPV6_IANA_DEFAULT_T1                0
#define TCPIP_DHCPV6_IANA_DEFAULT_T2                0

// default values for IATA T1, T2 
// If 0, the timeout will be infinite (0xffffffff)
#define TCPIP_DHCPV6_IATA_DEFAULT_T1                0
#define TCPIP_DHCPV6_IATA_DEFAULT_T2                0

// default values for IANA T1, T2 in a solicit message 
#define TCPIP_DHCPV6_IANA_SOLICIT_T1                0
#define TCPIP_DHCPV6_IANA_SOLICIT_T2                0


// default values for IANA descriptors in a solicit message

// default addresses for IANA in a solicit message
#define TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO      0

// default values for the IANA Solicit addresses
// irrelevant if TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO == 0
// should be a value for each TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO 
#define TCPIP_DHCPV6_IANA_SOLICIT_DEFAULT_ADDRESS               "::0"


// default values for IATA descriptors in a solicit message

// default addresses for IATA in a solicit message
#define TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO          0

// default values for the IANA Solicit addresses
// irrelevant if TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO == 0
// should be a value for each TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO 
#define TCPIP_DHCPV6_IATA_SOLICIT_DEFAULT_ADDRESS               0

// default lifetimes for the solicited addresses
#define TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_PREF_LTIME    0
#define TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_VALID_LTIME   0


// number of character to reserve for a server status code associated message
#define TCPIP_DHCPV6_STATUS_CODE_MESSAGE_LEN            0


// number of DNS servers to store from a DHCP server reply
#define TCPIP_DHCPV6_DNS_SERVERS_NO            2

// space for the Domain Search List option - multiple of 16
#define TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE        64



// default value of the buffer to assemble messages, etc.
#define TCPIP_DHCPV6_MESSAGE_BUFFER_SIZE   512


// default number of buffers
#define TCPIP_DHCPV6_MESSAGE_BUFFERS       8

// preference value that forces the server selection
// 8 bit value!
#define TCPIP_DHCPV6_FORCED_SERVER_PREFERENCE   255


// defining this symbol will skip the DAD processing for DHCPv6 generated addresses
#define TCPIP_DHCPV6_SKIP_DAD_PROCESS

// The DHCPv6 task processing rate: number of milliseconds to generate an DHCPv6 tick.
// Used by the DHCPv6 state machine
// The default value is 100 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DHCPV6_TASK_TICK_RATE        (100)

// allow DHCPV6 client user notification
// if enabled, the TCPIP_DHCPV6_HandlerRegister/TCPIP_DHCPV6_HandlerDeRegister
// functions exist and can be used
#define TCPIP_DHCPV6_USER_NOTIFICATION           false 


#endif  // _DCHPV6_CONFIG_H_



