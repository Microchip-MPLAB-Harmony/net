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

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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



