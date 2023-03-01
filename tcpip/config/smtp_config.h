/*******************************************************************************
  Simple Mail Transfer Protocol (SMTP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    smtp_config.h

  Summary:
    SMTP configuration file

  Description:
    This file contains the SMTP module configuration options

    This file is not part of the project, it is a configuration template file only. 

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef _SMTPZ_CONFIG_H_
#define _SMTP_CONFIG_H_


// How long to wait before assuming the connection
// has been dropped (default 8 seconds)
#define TCPIP_SMTP_SERVER_REPLY_TIMEOUT     (8)

// the minimum amount of data to ask from the transport/encryption layer
// when querying the write space
#define TCPIP_SMTP_WRITE_READY_SPACE        150


// the max size of data to be written in a discrete string/array
// email operation: TCPIP_SMTP_StringPut/TCPIP_SMTP_ArrayPut.
// Excess characters will be discarded. 
// Note that the higher this value, the greater the size of the underlying
// TCP socket TX buffer
// Adjust as needed. Normally should not exceed 512 characters.
#define TCPIP_SMTP_MAX_WRITE_SIZE            512

// SMTP task rate, milliseconds
// The default value is 55 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_SMTP_TASK_TICK_RATE           (55)

#endif  // _SMTP_CONFIG_H_



