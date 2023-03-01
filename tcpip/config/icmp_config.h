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
#ifndef _ICMP_CONFIG_H_
#define _ICMP_CONFIG_H_

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


// Maximum number of parallel outgoing ICMP echo requests supported at one time
// Default value is 4
#define TCPIP_STACK_MAX_CLIENT_ECHO_REQUESTS    4

#endif  // _ICMP_CONFIG_H_
