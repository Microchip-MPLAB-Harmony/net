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
/*
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
