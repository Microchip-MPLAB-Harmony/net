/*******************************************************************************
  Telnet Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    telnet_config.h

  Summary:
    Configuration file

  Description:
    This file contains the Telnet module configuration options

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
#ifndef _TELNET_CONFIG_H_
#define _TELNET_CONFIG_H_


// Set up configuration parameter defaults if not overridden 

// Maximum number of Telnet connections
#define TCPIP_TELNET_MAX_CONNECTIONS  (2)

// Size of the telnet socket TX buffer
// if long messages are sent over the telnet connection
// the socket should have a large enough buffer to write the data at once
//
// Leave 0 for default TCP TX buffer size
#define TCPIP_TELNET_SKT_TX_BUFF_SIZE      0

// Size of the telnet socket RX buffer
// If long messages are sent over the telnet connection
// the socket should have a large enough buffer to receive the data
//
// Leave 0 for default TCP RX buffer size
#define TCPIP_TELNET_SKT_RX_BUFF_SIZE      0

// Port on which the telnet server is listening
// Default is TCPIP_TELNET_SERVER_PORT (23) or
// TCPIP_TELNET_SERVER_SECURE_PORT (992) 
//
// Adjust as needed
#define TCPIP_TELNET_LISTEN_PORT    23


// Size of the internal print buffer
// This buffer is used when the telnet console printf style function
// is called
// Adjust based on the length of the messages to be formatted with print statements
//
// This buffer is created in the automatic stack.
//       Make sure that there's enough stack space for this buffer
#define TCPIP_TELNET_PRINT_BUFF_SIZE       200 

// Size of the internal line buffer
// This buffer is used for receiving and assembling the password, authentication
// and regular characters
// Adjust based on the length of the commands sent to the telnet server
//
// This buffer is created in the automatic stack.
//       Make sure that there's enough stack space for this buffer
#define TCPIP_TELNET_LINE_BUFF_SIZE       80

// Maximum size of the internal buffer to store the user name
// Adjust based on the length of the user names
// allowed for the telnet connections
//
// A buffer of this size if allocated for each connection
// to store the login user name 
//
// Longer user names will be truncated to this size
#define TCPIP_TELNET_USERNAME_SIZE       15

// Use the telnet connection information data
// as part of the authentication callback
//
// If false
//      no connection info data is provided to 
//      the authentication callback
//      This is useful for the default case when
//      all connections will use the same username/password.
//
//  If true
//      the authentication callback will contain the connection info data
//      to allow different processing based on the
//      source of the authentication request
//
// This flag is relevant only when TCPIP_TELNET_OBSOLETE_AUTHENTICATION == false
#define TCPIP_TELNET_AUTHENTICATION_CONN_INFO     true

// telnet obsolete authentication method disable/enable
// 
// If false
//      This is the default and the PREFERRED case!
//      The telnet module will call a registrated 
//      callback to authenticate the users.
//      Is is the preferred authentication method.
//      It requires the run time registration of an authentication handler
//
//      Until the moment the authentication handler is registered
//      any login attempts will fail!
//
// If true
//      The old OBSOLETE authentication method using the build symbols
//      TCPIP_TELNET_USERNAME and TCPIP_TELNET_PASSWORD is used
//      This method is DEPRECATED and it will be eventually removed!
//
//      It is recommended that you leave the setting to false!
#define TCPIP_TELNET_OBSOLETE_AUTHENTICATION    false

// Default Telnet user name
//
// DEPRECATED. this is used only when there's no authentication callback defined!
//
// Use only when TCPIP_TELNET_OBSOLETE_AUTHENTICATION == true
#define TCPIP_TELNET_USERNAME     "admin"

// Default Telnet password
// DEPRECATED. this is used only when there's no authentication callback defined!
//
// Use only when TCPIP_TELNET_OBSOLETE_AUTHENTICATION == true
#define TCPIP_TELNET_PASSWORD     "microchip"

// telnet task rate, milliseconds
//
// The default value is 100 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
//
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_TELNET_TASK_TICK_RATE   100

#endif  // _TELNET_CONFIG_H_

