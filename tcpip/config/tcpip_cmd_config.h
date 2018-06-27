/*******************************************************************************
  TCP/IP Commands configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_cmd_config.h

  Summary:
    TCP/IP Commands configuration file
    
  Description:
    This file contains the TCP/IP commands configuration options
    
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
#ifndef _TCPIPCMD_CONFIG_H_
#define _TCPIPCMD_CONFIG_H_

// enable the Wi-Fi related commands
#define TCPIP_STACK_COMMANDS_WIFI_ENABLE


// enable the storage for stack/interface up/down commands
#define TCPIP_STACK_COMMANDS_STORAGE_ENABLE

// default number of ICMP Echo requests to send by default
#define     TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS         4

// default number of milliseconds to wait between requests
#define     TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY    1000

// default number of milliseconds to give up, if no echo reply
#define     TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT          5000

// default size of the buffer for sending an echo request
#define     TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE    1000


// default size of the data in an echo request
// could be <= TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE
#define     TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE    100



#endif  // _TCPIPCMD_CONFIG_H_

