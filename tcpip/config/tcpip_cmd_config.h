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
/*****************************************************************************
 Copyright (C) 2011-2018 Microchip Technology Inc. and its subsidiaries.

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

