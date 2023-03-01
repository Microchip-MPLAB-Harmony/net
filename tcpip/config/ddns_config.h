/*******************************************************************************
  Dynamic Domain Name Service (DynDNS) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    ddns_config.h

  Summary:
    Dynamic DNS configuration file
    
  Description:
    This file contains the Dynamic DNS module configuration options

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
#ifndef _DYNDNS_CONFIG_H_
#define _DYNDNS_CONFIG_H_


// Client configuration options

// Default CheckIP server for determining current IP address
#define TCPIP_DDNS_CHECKIP_SERVER (const uint8_t*)"checkip.dyndns.com"		

// dynDNS task rate, ms
// The default value is hundreds of  milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DDNS_TASK_TICK_RATE       777

#endif  // _DYNDNS_CONFIG_H_



