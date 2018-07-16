/*******************************************************************************
  Interrupt System Service Library Interface Implementation File

  Company
    Microchip Technology Inc.

  File Name
    sys_int.c

  Summary
    Interrupt system service library implementation.

  Description
    This file implements the interface to the interrupt system service library.

  Remarks:
    None.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

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
#include "system/int/sys_int.h"


// *****************************************************************************
// *****************************************************************************
// Section: Interface Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
void SYS_INT_Enable( void )
{
    __DMB();
    __enable_irq();

    return;
}


// *****************************************************************************
INT_CONTROLLER_STATUS SYS_INT_Disable( void )
{
    INT_CONTROLLER_STATUS processorStatus;

    processorStatus = (INT_CONTROLLER_STATUS) (__get_PRIMASK() == 0);

    __disable_irq();
    __DMB();

    return processorStatus;
}


// *****************************************************************************
void SYS_INT_Restore( INT_CONTROLLER_STATUS state )
{
    if(0 != state )
    {
        __DMB();
        __enable_irq();
    }
    else
    {
        /* Interrupt controller was not enabled */
    }

    return;
}

bool SYS_INT_SourceDisable( INT_SOURCE source )
{
    INT_CONTROLLER_STATUS processorStatus;
    bool intSrcStatus;

    processorStatus = SYS_INT_Disable();

    intSrcStatus = NVIC_GetEnableIRQ(source);

    NVIC_DisableIRQ( source );

    SYS_INT_Restore( processorStatus );

    /* return the source status */
    return intSrcStatus;
}