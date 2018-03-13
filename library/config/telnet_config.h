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
#ifndef _TELNET_CONFIG_H_
#define _TELNET_CONFIG_H_


// Set up configuration parameter defaults if not overridden 

// Force all connecting clients to be secured and connected via
// TCPIP_TELNET_SERVER_SECURE_PORT.
// Connections on port TCPIP_TELNET_SERVER_PORT will be ignored.
//#define TCPIP_TELNET_REJECT_UNSECURED

#if !defined(TCPIP_TELNET_MAX_CONNECTIONS)
    // Maximum number of Telnet connections
    #define TCPIP_TELNET_MAX_CONNECTIONS  (2u)
#endif

#if !defined(TCPIP_TELNET_USERNAME)
    // Default Telnet user name
    #define TCPIP_TELNET_USERNAME     "admin"
#endif

#if !defined(TCPIP_TELNET_PASSWORD)
    // Default Telnet password
    #define TCPIP_TELNET_PASSWORD     "microchip"
#endif


// telnet task rate, milliseconds
// The default value is 100 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_TELNET_TASK_TICK_RATE   100

#endif  // _TELNET_CONFIG_H_

