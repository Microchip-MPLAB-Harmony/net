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
void DefaultInterruptHandler( void )
{
    while( 1 ){
    }
}
uint32_t spuriousEventCount = 0;
void DefaultInterruptHandlerForSpurious( void )
{
    ++spuriousEventCount;
}
/*  Weak definitions for default handlers.  Users may override these with
    implementations of their own or provide alternate functions to the 
    interrupt controller.  PLIB implementations provide alternate functions, 
    with the name {periph}_InterruptHandler so as they will not collide with
    user {periph}_Handler definitions.
*/
void SAIC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PMU_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void PIT_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void WDT_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void GMAC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void XDMAC0_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void XDMAC1_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void ICM_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void AES_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void AESB_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void TDES_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void SHA_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void MPDDRC_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void MATRIX1_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void MATRIX0_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void SECUMOD_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void HSMC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOA_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM0_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM1_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM2_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM3_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void FLEXCOM4_Handler( void )            __attribute__((weak, alias("DefaultInterruptHandler")));
void UART0_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void UART1_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void UART2_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void UART3_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void UART4_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void TWIHS0_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void TWIHS1_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void SDMMC0_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void SDMMC1_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void SPI0_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void SPI1_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void TC0_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void TC1_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void PWM_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void ADC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void UHPHS_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void UDPHS_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void SSC0_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void SSC1_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void LCDC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void ISC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void TRNG_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PDMIC_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void AIC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void SFC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void SECURAM_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void QSPI0_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void QSPI1_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void I2SC0_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void I2SC1_Handler( void )               __attribute__((weak, alias("DefaultInterruptHandler")));
void MCAN0_INT0_Handler( void )          __attribute__((weak, alias("DefaultInterruptHandler")));
void MCAN1_INT0_Handler( void )          __attribute__((weak, alias("DefaultInterruptHandler")));
void PTC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void CLASSD_Handler( void )              __attribute__((weak, alias("DefaultInterruptHandler")));
void L2CC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void MCAN0_INT1_Handler( void )          __attribute__((weak, alias("DefaultInterruptHandler")));
void MCAN1_INT1_Handler( void )          __attribute__((weak, alias("DefaultInterruptHandler")));
void GMAC_Q1_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void GMAC_Q2_Handler( void )             __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOB_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void PIOD_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void SDMMC0_TIMER_Handler( void )        __attribute__((weak, alias("DefaultInterruptHandler")));
void SDMMC1_TIMER_Handler( void )        __attribute__((weak, alias("DefaultInterruptHandler")));
void SYSC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void ACC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void RXLP_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));


/* Weak definitions for sub-handlers in vectors shared by multiple interrupts */
void PMC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));
void RSTC_Handler( void )                __attribute__((weak, alias("DefaultInterruptHandler")));
void RTC_Handler( void )                 __attribute__((weak, alias("DefaultInterruptHandler")));

void FreeRTOS_Tick_Handler(              void );
void GMAC_InterruptHandler(              void );
void UART1_InterruptHandler(             void );
void TC0_InterruptHandler(               void );

/* Handlers for vectors that are shared by multiple interrupts */
void SYSC_SharedHandler( void )
{
}

/* data for irq register initialization */
IrqData irqData[] = {
    { 3,   (uint32_t) AIC_REGS,    FreeRTOS_Tick_Handler,      AIC_SMR_SRCTYPE_INT_LEVEL_SENSITIVE_Val,  AIC_SMR_PRIORITY_MINIMUM_Val   },
    { 5,   (uint32_t) AIC_REGS,    GMAC_InterruptHandler,      AIC_SMR_SRCTYPE_INT_LEVEL_SENSITIVE_Val,  AIC_SMR_PRIORITY_MINIMUM_Val   },
    { 25,  (uint32_t) AIC_REGS,    UART1_InterruptHandler,     AIC_SMR_SRCTYPE_INT_LEVEL_SENSITIVE_Val,  AIC_SMR_PRIORITY_MINIMUM_Val   },
    { 35,  (uint32_t) AIC_REGS,    TC0_InterruptHandler,       AIC_SMR_SRCTYPE_INT_LEVEL_SENSITIVE_Val,  AIC_SMR_PRIORITY_MINIMUM_Val   },
};

uint32_t irqDataEntryCount = sizeof( irqData ) / sizeof( irqData[ 0 ]);



/*******************************************************************************
 End of File
*/
