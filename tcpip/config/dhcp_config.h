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
#ifndef _DCHP_CONFIG_H_
#define _DCHP_CONFIG_H_


// Defines how long to wait before a DHCP lease is acquired
// when the DHCP module is enabled, seconds
#define TCPIP_DHCP_TIMEOUT				(10)


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
#define TCPIP_DHCP_HOST_NAME_SIZE       20

// enable the usage of the Boot file name received from the DHCP server
#define TCPIP_DHCP_STORE_BOOT_FILE_NAME

// size of the storage for the Boot file name
// should always be <= 128
// default value is 128
#define TCPIP_DHCP_BOOT_FILE_NAME_SIZE      128

#endif  // _DCHP_CONFIG_H_
