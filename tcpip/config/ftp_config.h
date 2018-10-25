/*******************************************************************************
  File Transfer Protocol (FTP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftp_config.h

  Summary:
    FTP configuration file
    
  Description:
    This file contains the FTP module configuration options
    
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
#ifndef _FTP_CONFIG_H_
#define _FTP_CONFIG_H_


// Specifies the max length for user name
#define TCPIP_FTP_USER_NAME_LEN		(10)

// Specifies the max length of FTP login password
#define TCPIP_FTP_PASSWD_LEN      (10)

// Comment this line out to disable MPFS
#define TCPIP_FTP_PUT_ENABLED

// Maximum number of FTP connections allowed 
#define TCPIP_FTP_MAX_CONNECTIONS		(1)


// Define the size of the TX buffer for the FTP Data socket
// Use 0 for default TCP TX buffer size.
// The default recommended value for high throughput is > 2MSS (3 KB).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
#define     TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE   0

// Define the size of the RX buffer for the FTP Data socket
// Use 0 for default TCP RX buffer size.
// The default recommended value for high throughput is > 2MSS (3 KB).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
#define     TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE   0


// The FTP server task rate, milliseconds
// The default value is 33 milliseconds.
// The lower the rate (higher the frequency) the higher the FTP server priority
// and higher transfer rates could be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define     TCPIP_FTPS_TASK_TICK_RATE       33

// FTP timeout, seconds
#define TCPIP_FTP_TIMEOUT                 	(1200ul)

#endif  // _FTP_CONFIG_H_
