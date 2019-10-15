/*******************************************************************************
  FTP Client Module API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftpc_config.h

  Summary:
    The FTP Client module implements the file transfer protocol. 

  Description:
    The FTPC module is the protocol which is used to transfer files.
********************************************************************************/
    
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _FTPC_CONFIG_H_
#define _FTPC_CONFIG_H_

// The FTP Client task rate in milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_FTPC_TASK_TICK_RATE              (5)

// Specify the Default Transmit Buffer Size for the FTP Client Data Socket
// if 0, the TCP Socket Transmit buffer size will be used by FTPC data socket
#define TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT      		(0)


// Specify the Default Receive Buffer Size for the FTP Client Data Socket
// if 0, the TCP Socket Receive buffer size will be used by FTPC data socket
#define TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT         	(0)

// Specify the time-out for FTP Client commands, in seconds
// This is the time to wait for a response from FTP server
#define TCPIP_FTPC_TMO           (2) 

// Specify the maximum number of supported simultaneous FTP
// client connections.
#define TCPIP_FTPC_MAX_NUM_CLIENT                 (3)

// Specify whether FTP Client command in the tcpip_commands
// is enabled.
#define TCPIP_FTPC_COMMANDS                 false


#endif  // _FTPC_CONFIG_H_
