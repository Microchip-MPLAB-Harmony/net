/*******************************************************************************
  FTP Server Definitions for the Microchip TCP/IP Stack
  
  Company:
    Microchip Technology Inc.
    
  File Name: ftp.h

  Summary:
    An embedded FTP (File Transfer Protocol) server is an excellent addition to
    any network-enabled device.

  Description:
    An embedded FTP (File Transfer Protocol) server is an excellent addition to 
    any network-enabled device. FTP server capability facilitates the uploading 
    of files to, and downloading of files from, an embedded device. Almost all 
    computers have, at the very least, a command line FTP client that will allow 
    a user to connect to an embedded FTP server.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef __FTP_H
#define __FTP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Structure:
    TCPIP_FTP_MODULE_CONFIG

  Summary:
    FTP Sever module runtime and initialization configuration data.

  Description:
    FTP server configuration and initialization data . Configuration
	is part of tcpip_stack_init.c.
*/
typedef struct
{
    uint16_t     nConnections;   // number of simultaneous FTP connections allowed
    uint16_t    dataSktTxBuffSize;  // size of Data socket TX buffer for the associated socket; leave 0 for default
    uint16_t    dataSktRxBuffSize;  // size of Data Socket RX buffer for the associated socket; leave 0 for default
    char     *userName; // FTP login User name. Size should not exceed more than  TCPIP_FTP_USER_NAME_LEN
    char     *password; // FTP login password. Size should not exceed more than TCPIP_FTP_PASSWD_LEN
} TCPIP_FTP_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void  TCPIP_FTP_ServerTask(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs FTP module tasks in the TCP/IP stack.

  Precondition:
    The FTP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_FTP_ServerTask(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __FTP_H