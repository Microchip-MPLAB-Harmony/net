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
#ifndef _FTP_CONFIG_H_
#define _FTP_CONFIG_H_


// Specifies the max length for user name
#define TCPIP_FTP_USER_NAME_LEN     (10)

// Specifies the max length of FTP login password
#define TCPIP_FTP_PASSWD_LEN      (10)

// Comment this line out to disable MPFS
#define TCPIP_FTP_PUT_ENABLED

// Maximum number of FTP connections allowed 
#define TCPIP_FTP_MAX_CONNECTIONS       (1)


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
#define TCPIP_FTP_TIMEOUT                   (1200ul)

#endif  // _FTP_CONFIG_H_
