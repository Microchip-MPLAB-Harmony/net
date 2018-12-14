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