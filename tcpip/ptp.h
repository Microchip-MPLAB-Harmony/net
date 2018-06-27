/*******************************************************************************
 IEEE 1588 PTP API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ptp.h

  Summary:
    IEEE 1588 PTP API

  Description:
    Interface to the Precision Time Protocol described in IEEE 1588
    - Reference: IEEE 1588-2008
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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

#ifndef __PTP_H__
#define __PTP_H__


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
    TCPIP_PTP_MODULE_CONFIG

  Summary:
    PTP module run-time configuration/initialization data.

  Description:
    This structure defines PTP module run-time configuration/initialization data.
*/
//
typedef struct
{

} TCPIP_PTP_MODULE_CONFIG;


// *****************************************************************************
// *****************************************************************************
// Section: General Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/*
  Function:
    void  TCPIP_PTP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs PTP module tasks in the TCP/IP stack.

  Precondition:
    The PTP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_PTP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __PTP_H__

/*
 End of File
*/
