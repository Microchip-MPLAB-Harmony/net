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

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
