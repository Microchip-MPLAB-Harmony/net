/*******************************************************************************
  Periodic Interval Timer (PIT) 

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pit.c

  Summary:
    Periodic Interval Timer (PIT) PLIB.

  Description:
    This file defines the interface for the Periodic Interval Timer (PIT).
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
#include "plib_pit.h"
#include "device.h"


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data Types
// *****************************************************************************
// *****************************************************************************
typedef struct
{
    PIT_CALLBACK        callback;
    uintptr_t           context;
    volatile uint32_t   tickCounter;
} PIT_OBJECT;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************
static PIT_OBJECT pit;


void PIT_TimerInitialize(void)
{
    PIT_REGS->PIT_PIVR;
    PIT_REGS->PIT_MR = PIT_MR_PIV(518750-1) | PIT_MR_PITEN(0) | PIT_MR_PITIEN(1);
}

void PIT_TimerRestart(void)
{
    PIT_REGS->PIT_MR &= ~PIT_MR_PITEN_Msk;
    while((PIT_REGS->PIT_PIIR & PIT_PIIR_CPIV_Msk) != 0) {
        ;
    }
    PIT_REGS->PIT_MR |= PIT_MR_PITEN_Msk;
}

void PIT_TimerStart(void)
{
    PIT_REGS->PIT_MR |= PIT_MR_PITEN_Msk;
}

void PIT_TimerStop(void)
{
    PIT_REGS->PIT_MR &= ~PIT_MR_PITEN_Msk;
    while ((PIT_REGS->PIT_PIIR & PIT_PIIR_CPIV_Msk) != 0) {
        ;
    }
}

void PIT_TimerPeriodSet(uint32_t period)
{
    PIT_TimerStop();
    PIT_REGS->PIT_MR &= ~PIT_MR_PIV_Msk;
    PIT_REGS->PIT_MR |= PIT_MR_PIV(period);
    PIT_TimerStart();
}

uint32_t PIT_TimerPeriodGet(void)
{
    return PIT_REGS->PIT_MR & PIT_MR_PIV_Msk;
}

uint32_t PIT_TimerCounterGet(void)
{
    return (PIT_REGS->PIT_PIIR & PIT_PIIR_CPIV_Msk) >> PIT_PIIR_CPIV_Pos;
}

void PIT_TimerCompareSet( uint16_t compare )
{
    (void) compare;
}

uint32_t PIT_TimerFrequencyGet(void)
{
    return 83000000 / 16;
}

void PIT_DelayMs(uint32_t delay)
{
    uint32_t tickStart, delayTicks;

    if((PIT_REGS->PIT_MR & (PIT_MR_PITEN_Msk | PIT_MR_PITIEN_Msk)) == (PIT_MR_PITEN_Msk | PIT_MR_PITIEN_Msk))
    {
        tickStart=pit.tickCounter;
        delayTicks=delay/100;

        while( (pit.tickCounter-tickStart) < delayTicks ) {
            ;
        }
    }
}

void PIT_TimerCallbackSet(PIT_CALLBACK callback, uintptr_t context)
{
    pit.callback = callback;
    pit.context = context;
}

void PIT_InterruptHandler(void)
{
    uint32_t reg = PIT_REGS->PIT_PIVR;
    pit.tickCounter++;
    if(pit.callback)
        pit.callback(pit.context);
}
/*******************************************************************************
 End of File
*/
