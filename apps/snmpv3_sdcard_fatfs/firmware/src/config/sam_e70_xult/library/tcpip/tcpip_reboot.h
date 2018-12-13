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
