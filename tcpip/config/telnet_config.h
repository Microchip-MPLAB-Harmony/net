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

