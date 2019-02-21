/*******************************************************************************
  AIC PLIB Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    plib_aic.h

  Summary:
    Function implementations for the AIC PLIB.

  Description:
    This PLIB provides a simple interface to configure the Advanced Interrupt
    Controller.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: AIC Implementation
// *****************************************************************************
// *****************************************************************************
extern IrqData  irqData[2];
extern uint32_t irqDataEntryCount;
void DefaultInterruptHandlerForSpurious( void );

void
INT_Initialize( void )
{   
    const unsigned      MaxInterruptDepth = 8;
    const unsigned      MaxNumPeripherals = 0x7F;           // 127
    const uint32_t      keyGuard = 0xb6d81c4d;
    uint32_t            ii;
    aic_registers_t *   aicPtr;

    __disable_irq();
    __DMB();                                                // Data Memory Barrier
    __ISB();                                                // Instruction Synchronization Barrier
    ////// secure to nonSecure redirection
    SFR_REGS->SFR_AICREDIR = (keyGuard ^ SFR_REGS->SFR_SN1) | SFR_AICREDIR_NSAIC_Msk;
    ////// nonsecure registers
    aicPtr = (aic_registers_t *) AIC_REGS;
    for( ii= 0; ii < MaxNumPeripherals; ++ii )
    {
        aicPtr->AIC_SSR = AIC_SSR_INTSEL( ii );
        aicPtr->AIC_IDCR = AIC_IDCR_Msk;
        __DSB();
        __ISB();
        aicPtr->AIC_ICCR = AIC_ICCR_INTCLR_Msk;
    }
    for( ii = 0; ii < MaxInterruptDepth; ++ii )
    {   // pop all possible nested interrupts from internal hw stack
        aicPtr->AIC_EOICR = AIC_EOICR_ENDIT_Msk;
    }

    for( ii = 0; ii < irqDataEntryCount; ++ii )
    {   // inspect irqData array in interrupts.c to see the configuration data 
        aicPtr = (aic_registers_t *) irqData[ ii ].targetRegisters;
        aicPtr->AIC_SSR = AIC_SSR_INTSEL( irqData[ ii ].peripheralId );
        aicPtr->AIC_SMR = (aicPtr->AIC_SMR & ~AIC_SMR_SRCTYPE_Msk)  | AIC_SMR_SRCTYPE( irqData[ ii ].srcType );
        aicPtr->AIC_SMR = (aicPtr->AIC_SMR & ~AIC_SMR_PRIORITY_Msk) | (irqData[ ii ].priority << AIC_SMR_PRIORITY_Pos);
        aicPtr->AIC_SPU = (uint32_t) DefaultInterruptHandlerForSpurious;
        aicPtr->AIC_SVR = (uint32_t) irqData[ ii ].handler;
        aicPtr->AIC_IECR = AIC_IECR_Msk;
    }
    //////
    __DSB();                                                // Data Synchronization Barrier
    __enable_irq();
    __ISB();                                                // Allow pended interrupts to be recognized immediately
}
