/*******************************************************************************
  System interrupt interface using the AIC

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int_aic.c

  Summary:
    Implementation of AIC functions required at the Harmony driver level.
    These functions are supplemental to those contained in plib_aic.c and 
    are not intended to be called by code at the PLIB level.

  Description:
    Implementation of AIC functions intended to be protected and exclusively 
    used at the Harmony driver, service, level.  Some of these functions may 
    have no effect on the operational status of the interrupt controller if 
    the implementation is not relevant to the hardware.

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
#include "system/int/sys_int.h"

// map AIC methods to SYS_INT names, AIC names are better description of implementation
#define INT_AreIrqsEnabled       	SYS_INT_IsEnabled
#define INT_IrqEnable              	SYS_INT_Enable
#define INT_IrqDisable             	SYS_INT_Disable
#define INT_IrqRestore             	SYS_INT_Restore
#define INT_InterruptEnable        	SYS_INT_SourceEnable
#define INT_InterruptDisable       	SYS_INT_SourceDisable
#define INT_InterruptRestore        SYS_INT_SourceRestore
#define INT_IsInterruptEnabled     	SYS_INT_SourceIsEnabled
#define INT_IsInterruptPendingFor  	SYS_INT_SourceStatusGet
#define INT_InterruptPendingSet    	SYS_INT_SourceStatusSet
#define INT_InterruptPendingClear  	SYS_INT_SourceStatusClear

// *****************************************************************************
// *****************************************************************************
// Section: Implementation
// *****************************************************************************
// *****************************************************************************

// private methods *************************************************************

static aic_registers_t *
_aicInstanceGet( IRQn_Type aSrcSelection )
{	return( AIC_REGS ); }

// public methods **************************************************************

bool
INT_AreIrqsEnabled( void )
{
	bool retval = true;

	if( CPSR_I_Msk & __get_CPSR() ){
		retval = false;
	}
	return( retval );
}

void
INT_IrqEnable( void )
{
    __DMB();
    __enable_irq();
    return;
}

bool
INT_IrqDisable( void )
{
    bool previousValue = INT_AreIrqsEnabled();
    __disable_irq();
    __DMB();
    return( previousValue );
}

void
INT_IrqRestore( bool state )
{
    if( state == true )
    {
        __DMB();
        __enable_irq();
    }
    else
    {
        __disable_irq();
        __DMB();
    }
    return;
}

bool
INT_IsInterruptEnabled( IRQn_Type aSrcSelection )
{
    bool retval = false;
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    if( aicPtr->AIC_IMR & AIC_IMR_Msk ){
        retval = true;
    }
    return( retval );
}

void
INT_InterruptEnable( IRQn_Type aSrcSelection )
{
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_IECR = AIC_IECR_Msk;
    return;
}

bool
INT_InterruptDisable( IRQn_Type aSrcSelection )
{
    bool previousValue = INT_IsInterruptEnabled( aSrcSelection );
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_IDCR = AIC_IDCR_Msk;
    __DSB();
    __ISB();
    return( previousValue );
}

void
INT_InterruptRestore( IRQn_Type aSrcSelection, bool state )
{
    if( state ) {
        aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
        aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
        aicPtr->AIC_IECR = AIC_IECR_Msk;
    }
    return;
}

bool
INT_IsInterruptPendingFor( IRQn_Type aSrcSelection )
{
    bool retval = false;
    if( aSrcSelection < 127 )
    {
        uint32_t regValue = 0;
        uint32_t regSelection = aSrcSelection >> 5; // 32 status bits per register 
        uint32_t bitSelection = aSrcSelection - (regSelection << 5);
        aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
        aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
        switch( regSelection )    
        {
        case 0:
            regValue = aicPtr->AIC_IPR0;
            break;
        case 1:
            regValue = aicPtr->AIC_IPR1;
            break;
        case 2:
            regValue = aicPtr->AIC_IPR2;
            break;
        case 3:
        default:
            // regValue = aicPtr->AIC_IPR3;
            break;
        }
        if( regValue & (0x00000001Ul << bitSelection) ) {
            retval = true;
        }
    }
    return( retval );
}

void
INT_InterruptPendingSet( IRQn_Type aSrcSelection )
{
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_ISCR = AIC_ISCR_Msk;
    return;
}

void
INT_InterruptPendingClear( IRQn_Type aSrcSelection )
{
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_ICCR = AIC_ICCR_Msk;
    return;
}
