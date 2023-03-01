/*******************************************************************************
  TCP/IP Reboot Server API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_reboot.h

  Summary:
    TCP/IP Reboot Server APIs.

  Description:
     The Reboot server listens on a UDP connection and
     resets the system upon request.

*******************************************************************************/

//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef __TCPIP_REBOOT_H
#define __TCPIP_REBOOT_H

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
    TCPIP_REBOOT_CONFIG

  Summary:
    TCPIP Reboot server configuration.

  Description:
    TCPIP Reboot server configuration and initialization data.
*/
typedef struct
{
    void* reserved;
}TCPIP_REBOOT_CONFIG;


// *****************************************************************************
// *****************************************************************************
// Section: Reboot Server Function
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void  TCPIP_REBOOT_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs Reboot server module tasks in the TCP/IP stack.

  Precondition:
    The Reboot server module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_REBOOT_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // __TCPIP_REBOOT_H
