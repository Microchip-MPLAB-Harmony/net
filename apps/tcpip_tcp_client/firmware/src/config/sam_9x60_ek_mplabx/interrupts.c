/*******************************************************************************
 System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt.c

  Summary:
    Interrupt vectors mapping

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
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

#include "configuration.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************

/* Brief default interrupt handler for unused IRQs */
void __attribute__((optimize("-O1"),section(".text.DefaultInterruptHandler"),long_call, noreturn))DefaultInterruptHandler( void )
{
#if defined(__DEBUG) || defined(__DEBUG_D)
    asm("BKPT");
#endif
    while( 1 ){
    }
}
uint32_t spuriousEventCount = 0;
void __attribute__((optimize("-O1"),section(".text.DefaultInterruptHandlerForSpurious"),long_call))DefaultInterruptHandlerForSpurious( void )
{
   ++spuriousEventCount;
}
/*  Weak definitions for default handlers.  Users may override these with
    implementations of their own or provide alternate functions to the 
    interrupt controller.  PLIB implementations provide alternate functions, 
    with the name {periph}_InterruptHandler so as they will not collide with
    user {periph}_Handler definitions.
*/
void EXT_FIQ_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void SYSC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOA_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOB_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM0_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM1_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM2_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM3_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM6_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM7_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM8_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void SDMMC0_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM4_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM5_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM9_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM10_Handler( void )           __attribute__((weak, alias("DefaultInterruptHandler")));
void TC0_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void PWM_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void ADC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void XDMAC_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void MATRIX_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void UHPHS_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void UDPHS_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void EMAC0_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void LCDC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void SDMMC1_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void EMAC1_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void SSC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void CAN0_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void CAN1_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void EXT_IRQ_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM11_Handler( void )           __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM12_Handler( void )           __attribute__((weak, alias("DefaultInterruptHandler")));
void I2SMCC_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void QSPI_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void GFX2D_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void PIT64B_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void TRNG_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void AES_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void TDES_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void SHA_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void CLASSD_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void ISI_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOD_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void TC1_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void OTPC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void DBGU_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void ECC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void MC_Handler( void )                  __attribute__((weak, alias("DefaultInterruptHandler")));


/* Weak definitions for sub-handlers in vectors shared by multiple interrupts */
void PMC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void RSTC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void RTT_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void PIT_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void WDT_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void RTC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void PMECC_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void PMERRLOC_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void SDRAMC_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void MPDDRC_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void SMC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));

void TC0_InterruptHandler(               void );
void EMAC0_InterruptHandler(             void );
void DBGU_InterruptHandler(              void );

/* Handlers for vectors that are shared by multiple interrupts */
void SYSC_SharedHandler( void )
{
}
void ECC_SharedHandler( void )
{
}
void MC_SharedHandler( void )
{
}

/* data for irq register initialization */
IrqData irqData[] = {
    { 17,  (uint32_t) AIC_REGS,    TC0_InterruptHandler,       AIC_SMR_SRCTYPE_INT_LEVEL_SENSITIVE_Val,  0x0 },
    { 24,  (uint32_t) AIC_REGS,    EMAC0_InterruptHandler,     AIC_SMR_SRCTYPE_INT_LEVEL_SENSITIVE_Val,  0x0 },
    { 47,  (uint32_t) AIC_REGS,    DBGU_InterruptHandler,      AIC_SMR_SRCTYPE_INT_LEVEL_SENSITIVE_Val,  0x0 },
};

uint32_t irqDataEntryCount = sizeof( irqData ) / sizeof( irqData[ 0 ]);



/*******************************************************************************
 End of File
*/
