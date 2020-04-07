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
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CPSR_I_Pos
#define CPSR_I_Pos      7U
#endif

#ifndef CPSR_I_Msk
#define CPSR_I_Msk      (1UL << CPSR_I_Pos)
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Implementation
// *****************************************************************************
// *****************************************************************************

// private methods *************************************************************

static inline unsigned int __get_CPSR( void )
{
    unsigned int value = 0;
    asm volatile( "MRS %0, cpsr" : "=r"(value) );
    return value;
}

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
INT_IsInterruptEnabled( INT_SOURCE aSrcSelection )
{
    bool retval = false;
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    bool processorStatus = INT_IrqDisable();
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    if( aicPtr->AIC_IMR & AIC_IMR_Msk ){
        retval = true;
    }
    INT_IrqRestore(processorStatus);
    return( retval );
}

void
INT_InterruptEnable( INT_SOURCE aSrcSelection )
{
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    bool processorStatus = INT_IrqDisable();
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_IECR = AIC_IECR_Msk;
    INT_IrqRestore(processorStatus);
    return;
}

bool
INT_InterruptDisable( INT_SOURCE aSrcSelection )
{
    bool previousValue = INT_IsInterruptEnabled( aSrcSelection );
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    bool processorStatus = INT_IrqDisable();
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_IDCR = AIC_IDCR_Msk;
    __DSB();
    __ISB();
    INT_IrqRestore(processorStatus);
    return( previousValue );
}

void
INT_InterruptRestore( INT_SOURCE aSrcSelection, bool state )
{
    if( state ) 
    {
        aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
        bool processorStatus = INT_IrqDisable();
        aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
        aicPtr->AIC_IECR = AIC_IECR_Msk;
        INT_IrqRestore(processorStatus);
    }
    return;
}

bool
INT_IsInterruptPendingFor( INT_SOURCE aSrcSelection )
{
    bool retval = false;
    if( aSrcSelection < 127 )
    {
        uint32_t regValue = 0;
        uint32_t regSelection = aSrcSelection >> 5; // 32 status bits per register 
        uint32_t bitSelection = aSrcSelection - (regSelection << 5);
        aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
        bool processorStatus = INT_IrqDisable();
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
        INT_IrqRestore(processorStatus);
        if( regValue & (0x00000001Ul << bitSelection) ) {
            retval = true;
        }
    }
    return( retval );
}

void
INT_InterruptPendingSet( INT_SOURCE aSrcSelection )
{
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    bool processorStatus = INT_IrqDisable();
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_ISCR = AIC_ISCR_Msk;
    INT_IrqRestore(processorStatus);
    return;
}

void
INT_InterruptPendingClear( INT_SOURCE aSrcSelection )
{
    aic_registers_t * aicPtr = _aicInstanceGet( aSrcSelection );
    bool processorStatus = INT_IrqDisable();
    aicPtr->AIC_SSR = AIC_SSR_INTSEL( (uint32_t) aSrcSelection );
    aicPtr->AIC_ICCR = AIC_ICCR_Msk;
    INT_IrqRestore(processorStatus);
    return;
}
