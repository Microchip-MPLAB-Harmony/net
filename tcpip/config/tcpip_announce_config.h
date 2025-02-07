/*******************************************************************************
  Announce Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_announce_config.h

  Summary:
    Announce configuration file

  Description:
    This file contains the Announce module configuration options

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2011-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef _TCPIP_ANNOUNCE_CONFIG_H_
#define _TCPIP_ANNOUNCE_CONFIG_H_


// Maximum size of a payload sent once
// Adjust to your needs
#define TCPIP_ANNOUNCE_MAX_PAYLOAD        (512)

// Type of the broadcast used by the Announce module
// This type enables the network directed broadcast
// If it is not defined or it's 0, the network limited broadcast is used
// The default is nework limited broadcast
#define TCPIP_ANNOUNCE_NETWORK_DIRECTED_BCAST   0

// announce task rate, millseconds
// The default value is 333 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_ANNOUNCE_TASK_RATE    333


#endif  // _TCPIP_ANNOUNCE_CONFIG_H_
