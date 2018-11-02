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
