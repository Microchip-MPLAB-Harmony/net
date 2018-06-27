/*******************************************************************************
  Trivial File Transfer Protocol (TFTP) Client Configuration file
 
  Company:
    Microchip Technology Inc.
    
  File Name:
    tftpc_config.h

  Summary:
    TFTP Client configuration file.

  Description:
    This file contains the TFTP Client module configuration options.
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _TFTP_CONFIG_H_
#define _TFTP_CONFIG_H_


// The default TFTP interface for multi-homed hosts.
#define TCPIP_TFTPC_DEFAULT_IF      "PIC32INT"

// The number of seconds to wait before declaring a TIMEOUT error on GET.
#define TCPIP_TFTPC_CMD_PROCESS_TIMEOUT         (3ul)

// The number of seconds to wait before declaring a TIMEOUT error on PUT.
#define TCPIP_TFTPC_ARP_TIMEOUT                 (3ul)

// The number of attempts before declaring a TIMEOUT error.
#define TCPIP_TFTPC_MAX_RETRIES                 (3u)

// The TFTP client task rate in milliseconds.
// The default value is 100 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_TFTPC_TASK_TICK_RATE              (100)

// The maximum TFTP host name length size.
#define TCPIP_TFTPC_HOSTNAME_LEN                (32)

// The maximum value for the file name size.
#define TCPIP_TFTPC_FILENAME_LEN                (32)

// allow TFTP client user notification
// if enabled, the TCPIP_TFTPC_HandlerRegister/TCPIP_TFTPC_HandlerDeRegister
// functions exist and can be used
#define TCPIP_TFTPC_USER_NOTIFICATION           false 

#endif  // _TFTP_CONFIG_H_
