/*******************************************************************************
  PPP Driver Configuration Definitions for the template version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ppp_config.h

  Summary:
    PPP MAC driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.

  Note:    
    The PPP driver definitions are in the headers:
    "driver/ppp/drv_ppp_mac.h"
    "driver/ppp/drv_ppp.h"
    "driver/ppp/drv_hdlc_obj.h"
*******************************************************************************/

/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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



// 1. build time symbols:

// enable the PPP interface
#define TCPIP_STACK_USE_PPP_INTERFACE

// enable HDLC statistics
#define HDLC_ENABLE_STATISTICS     false

// enable PPP statistics
#define PPP_ENABLE_STATISTICS   false

// maximum number of statistics callback registrations that could be performed
#define PPP_MAX_STAT_REGISTRATIONS  1

// enable sending echo requests and receiving replies
#define PPP_ECHO_REQUEST_ENABLE true

// maximum number of possible simultaneous PPP echo requests
#define PPP_MAX_ECHO_REQUESTS    2

// enable notifications
#define PPP_NOTIFICATIONS_ENABLE        false

// maximum number of event registrations that could be performed
#define PPP_MAX_EVENT_REGISTRATIONS     1 

// maximum number of segments in a packet
// a more 'segmented' packet will be rejected and not transmitted
// normally should be set to 2 - TCP uses 2 segments
// but user created packets could contain multiple segments
#define PPP_MAX_PACKET_SEGMENTS         4


// for iperf: TX queue limit
// because the interface is slow, the queue limit needs to be higher
#define TCPIP_IPERF_PPP_TX_QUEUE_LIMIT  5

// for console commands - if TCPIP_STACK_USE_PPP_INTERFACE defined 
// enable HDLC related console commands
#define TCPIP_STACK_HDLC_COMMANDS false

// for console commands - if TCPIP_STACK_USE_PPP_INTERFACE defined 
// enable PPP related console commands
#define TCPIP_STACK_PPP_COMMANDS false

// for console commands - if TCPIP_STACK_USE_PPP_INTERFACE defined 
// timeout for receiving a echo request response - ms
#define TCPIP_STACK_COMMANDS_PPP_ECHO_TIMEOUT   100

// for console commands - if TCPIP_STACK_USE_PPP_INTERFACE defined 
// size of the echo request to send
#define TCPIP_STACK_COMMANDS_PPP_ECHO_REQUEST_DATA_SIZE 20

// for console commands - if TCPIP_STACK_USE_PPP_INTERFACE defined 
// number of echo requests to send
#define TCPIP_STACK_COMMANDS_PPP_ECHO_REQUESTS     4

// for console commands - if TCPIP_STACK_USE_PPP_INTERFACE defined 
// delay between different echo requests, ms
#define TCPIP_STACK_COMMANDS_PPP_ECHO_REQUEST_DELAY     100

