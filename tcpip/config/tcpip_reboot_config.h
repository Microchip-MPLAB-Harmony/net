/*******************************************************************************
  Reboot Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_reboot_config.h

  Summary:
    Configuration file
    
  Description:
    This file contains the Reboot module configuration options
    
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
#ifndef _TCPIP_REBOOT_CONFIG_H_
#define _TCPIP_REBOOT_CONFIG_H_

// For improved security, you might want to limit reboot capabilities 
// to only users on the same IP subnet.
// Define TCPIP_REBOOT_SAME_SUBNET_ONLY to enable this access restriction.
#define TCPIP_REBOOT_SAME_SUBNET_ONLY

// the mesage needed to be sent accross the net to reboot the machine
#define TCPIP_REBOOT_MESSAGE      "MCHP Reboot"

// the periodic rate of the Reboot task
// The default value is 1130 milliseconds.
// This module listens for incoming reboot requests
// and a high operation frequency is not required.
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_REBOOT_TASK_TICK_RATE     1130


#endif  // _TCPIP_REBOOT_CONFIG_H_



