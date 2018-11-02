/*******************************************************************************
  Dynamic Host Configuration Protocol (DCHP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcp_config.h

  Summary:
    DCHP configuration file
    
  Description:
    This file contains the DCHP module configuration options

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
#ifndef _DCHP_CONFIG_H_
#define _DCHP_CONFIG_H_


// Defines how long to wait before a DHCP lease is acquired
// when the DHCP module is enabled, seconds
#define TCPIP_DHCP_TIMEOUT				(10)


// The DHCP task processing rate: number of milliseconds to generate an DHCP tick.
// Used by the DHCP state machine
// The default value is 200 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_DHCP_TASK_TICK_RATE        (200)

// Default value for the enable/disable the DHCP client at stack start-up.
// Note: the interface initialization setting in TCPIP_NETWORK_CONFIG takes precedence!
#define TCPIP_DHCP_CLIENT_ENABLED        1


// Maximum size of a host name to be advertised to the DHCP server
#define TCPIP_DHCP_HOST_NAME_SIZE       20

// enable the usage of the Boot file name received from the DHCP server
#define TCPIP_DHCP_STORE_BOOT_FILE_NAME

// size of the storage for the Boot file name
// should always be <= 128
// default value is 128
#define TCPIP_DHCP_BOOT_FILE_NAME_SIZE      128

#endif  // _DCHP_CONFIG_H_
