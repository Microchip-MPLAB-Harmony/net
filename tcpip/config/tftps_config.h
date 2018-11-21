/*******************************************************************************
  TFTP Server Module API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tftps_config.h

  Summary:
    The TFTP server module implements the server for trivial file transfer protocol. 

  Description:
    The TFTP module is the simple protocol which is used to transfer files.It 
    has been implemented on top of Internet User Datagram Protocol.
    If the server grants the request, the connection is opened and the file is 
    sent in fixed length blocks of 512 bytes. 
    Each data packet contains one block of data, and must be acknowledged by an
    acknowledgment packet before the next packet can be sent.
********************************************************************************/
    
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _TFTPS_CONFIG_H_
#define _TFTPS_CONFIG_H_

// The TFTP server task rate in milliseconds.
// The default value is 100 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_TFTPS_TASK_TICK_RATE              (60)

// The number of attempts to retransmit  the previous packet before declaring a TIMEOUT error.
#define TCPIP_TFTPS_RETRANSMIT_COUNT                 (3u)


// The number of seconds to wait before declaring a TIMEOUT in seconds error on GET or PUT command processing.
#define TCPIP_TFTPC_CMD_PROCESS_TIMEOUT         (10ul)

// The number of Clients requests will be processed for the TFTP server stack.
#define TCPIP_TFTPS_CLIENT_NUMBER                 (2ul)

// allow TFTP Server user notification
// if enabled, the TCPIP_TFTPS_HandlerRegister/TCPIP_TFTPS_HandlerDeRegister
// functions exist and can be used
#define TCPIP_TFTPS_USER_NOTIFICATION           false 



// The maximum size for a file in bytes to be accepted.TFTP Opcode defined by RFC 2347.
#define TCPIP_TFTPS_DEFAULT_FILESIZE                (64000)

// The maximum value for the file name size.
// Even though the RFC allows blocks of up to 65464 bytes, 
// In practice the limit is set to 1468 bytes: the size of an 
// Ethernet MTU minus the headers of TFTP (4 bytes), UDP (8 bytes) and IP (20 bytes)
#define TCPIP_TFTPS_DEFAULT_BLOCK_SIZE                (32)

// This time is used during the TFTP retransmission 
// and TFTP Client wait for these many seconds before retransmitting .
#define TCPIP_TFTPS_DEFAULT_TIMEOUT           false 

#endif  // _TFTPS_CONFIG_H_
