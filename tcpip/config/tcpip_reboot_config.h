/*******************************************************************************
  Reboot Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_reboot_config.h

  Summary:
    Configuration file
    
  Description:
    This file contains the Reboot module configuration options

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
#ifndef _TCPIP_REBOOT_CONFIG_H_
#define _TCPIP_REBOOT_CONFIG_H_

// For improved security, you might want to limit reboot capabilities 
// to only users on the same IP subnet.
// Define TCPIP_REBOOT_SAME_SUBNET_ONLY to enable this access restriction.
#define TCPIP_REBOOT_SAME_SUBNET_ONLY

// the mesage needed to be sent accross the net to reboot the machine
#define TCPIP_REBOOT_MESSAGE      "MCHP Reboot"

// the periodic rate of the Reboot task
// The default value is 1130 milliseconds.
// This module listens for incoming reboot requests
// and a high operation frequency is not required.
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_REBOOT_TASK_TICK_RATE     1130


#endif  // _TCPIP_REBOOT_CONFIG_H_



