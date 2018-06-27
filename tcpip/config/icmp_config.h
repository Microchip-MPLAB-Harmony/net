/*******************************************************************************
  Internet Control Message Protocol (ICMP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    icmp_config.h

  Summary:
    ICMP configuration file
    
  Description:
    This file contains the ICMP module configuration options
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
#ifndef _ICMP_CONFIG_H_
#define _ICMP_CONFIG_H_

// allow ICMP client user notification
// if enabled, the TCPIP_ICMP_CallbackRegister/TCPIP_ICMP_CallbackDeregister
// functions exist and can be used
// Note that if disabled, the tcpip console ping command won't be available
#define TCPIP_ICMP_CLIENT_USER_NOTIFICATION           true 


// Timeout for a request that was sent and no reply was received
// milliseconds
#define TCPIP_ICMP_ECHO_REQUEST_TIMEOUT                500

// allow replies to echo requests sent to a broadcast address
// by default this should be disabled, to prevent broadcast flood attacks
#define TCPIP_ICMP_ECHO_ALLOW_BROADCASTS              0

// The ICMP task processing rate: number of milliseconds to generate an ICMP tick.
// This is the tick that advances the ICMP state machine.
// The default value is around 30 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_ICMP_TASK_TICK_RATE                 33

#endif  // _ICMP_CONFIG_H_
