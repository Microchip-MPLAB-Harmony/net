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
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
