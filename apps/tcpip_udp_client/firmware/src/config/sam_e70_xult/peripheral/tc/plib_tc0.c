/*******************************************************************************
  TC Peripheral Library Interface Source File

  Company
    Microchip Technology Inc.

  File Name
    plib_tc0.c

  Summary
    TC peripheral library source file.

  Description
    This file implements the interface to the TC peripheral library.  This
    library provides access to and control of the associated peripheral
    instance.

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

/*  This section lists the other files that are included in this file.
*/
#include "device.h"
#include "plib_tc0.h"

 

 

static uint32_t TC0_CH0_TimerStatus;  /* saves interrupt status */

/* Callback object for channel 0 */
TC_CALLBACK_OBJECT TC0_CH0_CallbackObj;

/* Initialize channel in timer mode */
void TC0_CH0_TimerInitialize (void)
{
    /* Use peripheral clock */
    TC0_REGS->TC_CHANNEL[0].TC_EMR = TC_EMR_NODIVCLK_Msk;
    /* clock selection and waveform selection */
    TC0_REGS->TC_CHANNEL[0].TC_CMR =  TC_CMR_WAVSEL_UP_RC | TC_CMR_WAVE_Msk ;

    /* write period */
    TC0_REGS->TC_CHANNEL[0].TC_RC = 60060U;

    /* enable interrupt */
    TC0_REGS->TC_CHANNEL[0].TC_IER = TC_IER_CPCS_Msk;
    TC0_CH0_CallbackObj.callback_fn = NULL;
}

/* Start the timer */
void TC0_CH0_TimerStart (void)
{
    TC0_REGS->TC_CHANNEL[0].TC_CCR = (TC_CCR_CLKEN_Msk | TC_CCR_SWTRG_Msk);
}

/* Stop the timer */
void TC0_CH0_TimerStop (void)
{
    TC0_REGS->TC_CHANNEL[0].TC_CCR = (TC_CCR_CLKDIS_Msk);
}

/* Configure timer period */
void TC0_CH0_TimerPeriodSet (uint16_t period)
{
    TC0_REGS->TC_CHANNEL[0].TC_RC = period;
}

/* Read timer period */
uint16_t TC0_CH0_TimerPeriodGet (void)
{
    return TC0_REGS->TC_CHANNEL[0].TC_RC;
}

/* Read timer counter value */
uint16_t TC0_CH0_TimerCounterGet (void)
{
    return TC0_REGS->TC_CHANNEL[0].TC_CV;
}

/* Check if timer period status is set */
bool TC0_CH0_TimerPeriodHasExpired(void)
{
    bool timer_status;
    NVIC_DisableIRQ(TC0_CH0_IRQn);
    timer_status = ((TC0_CH0_TimerStatus | TC0_REGS->TC_CHANNEL[0].TC_SR) & TC_SR_CPCS_Msk) >> TC_SR_CPCS_Pos;
    TC0_CH0_TimerStatus = 0U;
    NVIC_EnableIRQ(TC0_CH0_IRQn);
    return timer_status;
}

/* Register callback for period interrupt */
void TC0_CH0_TimerCallbackRegister(TC_CALLBACK callback, uintptr_t context)
{
    TC0_CH0_CallbackObj.callback_fn = callback;
    TC0_CH0_CallbackObj.context = context;
}

void TC0_CH0_InterruptHandler(void)
{
    TC0_CH0_TimerStatus = TC0_REGS->TC_CHANNEL[0].TC_SR;
    /* Call registered callback function */
    if (TC0_CH0_CallbackObj.callback_fn != NULL)
    {
        TC0_CH0_CallbackObj.callback_fn(TC0_CH0_CallbackObj.context);
    }
}
 

 

 

 
 
 
 
 

/**
 End of File
*/
