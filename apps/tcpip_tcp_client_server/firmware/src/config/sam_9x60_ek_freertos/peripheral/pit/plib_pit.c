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
    PIT_REGS->PIT_MR = PIT_MR_PIV(12500-1) | PIT_MR_PITEN(0) | PIT_MR_PITIEN(1);
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
    return 200000000 / 16;
}

void PIT_DelayMs(uint32_t delay)
{
    uint32_t tickStart, delayTicks;

    if((PIT_REGS->PIT_MR & (PIT_MR_PITEN_Msk | PIT_MR_PITIEN_Msk)) == (PIT_MR_PITEN_Msk | PIT_MR_PITIEN_Msk))
    {
        tickStart=pit.tickCounter;
        delayTicks=delay/1;

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
    uint32_t interruptStatus = PIT_REGS->PIT_SR;
    if( interruptStatus ) {
        volatile uint32_t reg = PIT_REGS->PIT_PIVR;
        (void)reg;
        pit.tickCounter++;
        if(pit.callback) {
            pit.callback(pit.context);
        }
    }
}
/*******************************************************************************
 End of File
*/
