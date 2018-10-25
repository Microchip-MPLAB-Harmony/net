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
