/*******************************************************************************
  Matrix (AHB) PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_matrix.c

  Summary:
    AHB Matrix PLIB implementation file

  Description:
    Configure AHB masters and slaves.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#ifndef _PLIB_MATRIX_H    // Guards against multiple inclusion
#define _PLIB_MATRIX_H

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif




// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void Matrix_Initialize(void)

  Summary:
    Initialize AHB Masters and Slaves.

  Description:
    Inialize AHB Masters and Slaves and peripheral's as secure or non-secure.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    Until security is implemented all peripherals will be non-secure.
*/
void Matrix_Initialize(void);


#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif // _FILE_TEMPLATE_HEADER_H

/*******************************************************************************
 End of File
*/
