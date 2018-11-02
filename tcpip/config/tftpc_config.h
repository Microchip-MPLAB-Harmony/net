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
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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
