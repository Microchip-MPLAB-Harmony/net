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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <device.h>

// *****************************************************************************
/* Function:
    void Matrix_Initialize(void)

  Summary:
    Initialize AHB Masters and Slaves.

  Description:
    Inialize AHB Masters and Slaves and peripheral's as secure or non-secure.

  Remarks:
    Until security is implemented all peripherals will be non-secure.
*/
void Matrix_Initialize(void)
{
    int i;
    uint32_t key;

    for (i=0; i<3; i++) {
        MATRIX0_REGS->MATRIX_SPSELR[i] = MATRIX_SPSELR_Msk;
        MATRIX1_REGS->MATRIX_SPSELR[i] = MATRIX_SPSELR_Msk;
    }

    key = 0xb6d81c4d ^ SFR_REGS->SFR_SN1;
    key &= 0xfffffffe;

    SFR_REGS->SFR_AICREDIR = key | SFR_AICREDIR_NSAIC_Msk;
}

/*******************************************************************************
 End of File
*/
