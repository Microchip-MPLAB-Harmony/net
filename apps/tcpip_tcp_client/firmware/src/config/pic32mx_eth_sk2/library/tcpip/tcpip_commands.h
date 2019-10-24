/**************************************************************************
TCP/IP Stack Run-time Commands API Definitions Header File

  Company:
    Microchip Technology Inc.
	
  File Name:
    tcpip_commands.h
	
  Summary:
    TCP/IP stack run time commands.
	
  Description:
    This module provides TCP/IP commands for run time tuning and status reporting
    using the system command processor and console.

  **************************************************************************/
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

#ifndef __TCPIP_COMMANDS_H
#define __TCPIP_COMMANDS_H

#include <stdint.h>
#include <stdbool.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************

// TCPIP command configuration/initialization
typedef struct
{
    void*   reserved;
}TCPIP_COMMAND_MODULE_CONFIG;


// *****************************************************************************
/*
  Function:
    void  TCPIP_COMMAND_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs TCP/IP command module tasks in the TCP/IP stack.

  Precondition:
    The TCP/IP cCmmand module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_COMMAND_Task(void);
    
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif	//#ifndef __TCPIP_COMMANDS_H
