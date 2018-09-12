/**
 * Copyright (c) 2016-2017 Microchip Technology Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "definitions.h" /* for potential custom handler names */
#include <libpic32c.h>
#include <sys/cdefs.h>
#include <stdbool.h>

/*
 *  The MPLAB X Simulator does not yet support simulation of programming the
 *  GPNVM bits yet. We can remove this once it supports the FRDY bit.
 */
#ifdef __MPLAB_DEBUGGER_SIMULATOR
#define __XC32_SKIP_STARTUP_GPNVM_WAIT
#endif

/*
 *  This startup code relies on features that are specific to the MPLAB XC32
 *  toolchain. Do not use it with other toolchains.
 */
#ifndef __XC32
#warning This startup code is intended for use with the MPLAB XC32 Compiler only.
#endif

/* Initialize segments */
extern uint32_t __svectors;

int main(void);
extern void __attribute__((long_call)) __libc_init_array(void);

/* Declaration of Reset handler (may be custom) */
void __attribute__((optimize("-O1"), long_call)) Reset_Handler(void);

/* Device Vector information is available in interrupt.c file */

__STATIC_INLINE void TCM_Disable(void);
__STATIC_INLINE void TCM_Enable(void);
__STATIC_INLINE void TCM_Configure(uint32_t tcmSize);
__STATIC_INLINE void ICache_Enable(void);
__STATIC_INLINE void DCache_Enable(void);
__STATIC_INLINE void FPU_Enable(void);

/* Enable Instruction Cache */
__STATIC_INLINE void ICache_Enable(void)
{
    SCB_EnableICache();
}

/* Enable Data Cache */
__STATIC_INLINE void DCache_Enable(void)
{
    SCB_EnableDCache();
}

#if (__ARM_FP==14) || (__ARM_FP==4)

/* Enable FPU */
__STATIC_INLINE void FPU_Enable(void)
{
uint32_t prim;
    prim = __get_PRIMASK();
    __disable_irq();

     SCB->CPACR |= (0xFu << 20);
    __DSB();
    __ISB();

    if (!prim)
    {
        __enable_irq();
    }
}
#endif /* (__ARM_FP==14) || (__ARM_FP==4) */



#define GPNVM_TCM_SIZE_Pos        7u
#define GPNVM_TCM_SIZE_Msk        (0x3u << GPNVM_TCM_SIZE_Pos)

/** Program GPNVM fuse for TCM configuration */
__STATIC_INLINE void TCM_Configure(uint32_t neededGpnvmValue)
{
    static uint32_t gpnvmReg;
    static uint32_t cmd;

    /* Read GPNVM fuse setting  */
    EFC_REGS->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_GGPB);

    while (!(EFC_REGS->EEFC_FSR & EEFC_FSR_FRDY_Msk))
    {
    }

    gpnvmReg=EFC_REGS->EEFC_FRR;

    /* Program only if change is needed */
    if (((gpnvmReg & GPNVM_TCM_SIZE_Msk)>>GPNVM_TCM_SIZE_Pos) != neededGpnvmValue)
    {
        if(neededGpnvmValue & 0x2)
            cmd=EEFC_FCR_FCMD_SGPB;
        else
            cmd=EEFC_FCR_FCMD_CGPB;

        EFC_REGS->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | cmd | EEFC_FCR_FARG(8));
        while (!(EFC_REGS->EEFC_FSR & EEFC_FSR_FRDY_Msk))
        {
        }

        if(neededGpnvmValue & 0x1)
            cmd=EEFC_FCR_FCMD_SGPB;
        else
            cmd=EEFC_FCR_FCMD_CGPB;

        EFC_REGS->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | cmd | EEFC_FCR_FARG(7));
        while (!(EFC_REGS->EEFC_FSR & EEFC_FSR_FRDY_Msk))
        {
        }

        /* Reset the device for the programmed fuse value to take effect */
        RSTC_REGS->RSTC_CR = RSTC_CR_KEY_PASSWD | RSTC_CR_PROCRST_Msk;
    }
}


/** Enable TCM memory */
__STATIC_INLINE void __attribute__((optimize("-O1"))) TCM_Enable(void)
{
    __DSB();
    __ISB();
    SCB->ITCMCR = (SCB_ITCMCR_EN_Msk  | SCB_ITCMCR_RMW_Msk | SCB_ITCMCR_RETEN_Msk);
    SCB->DTCMCR = (SCB_DTCMCR_EN_Msk | SCB_DTCMCR_RMW_Msk | SCB_DTCMCR_RETEN_Msk);
    __DSB();
    __ISB();
}

/* Disable TCM memory */
__STATIC_INLINE void __attribute__((optimize("-O1"))) TCM_Disable(void)
{
    __DSB();
    __ISB();
    SCB->ITCMCR &= ~(uint32_t)SCB_ITCMCR_EN_Msk;
    SCB->DTCMCR &= ~(uint32_t)SCB_ITCMCR_EN_Msk;
    __DSB();
    __ISB();
}








/* Optional application-provided functions */
extern void __attribute__((weak,long_call)) _on_reset(void);
extern void __attribute__((weak,long_call)) _on_bootstrap(void);

/* Reserved for use by the MPLAB XC32 Compiler */
extern void __attribute__((weak,long_call)) __xc32_on_reset(void);
extern void __attribute__((weak,long_call)) __xc32_on_bootstrap(void);


/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void __attribute__((optimize("-O1"), section(".text.Reset_Handler"), long_call)) Reset_Handler(void)
{
    uint32_t *pSrc;

    /* Call the optional application-provided _on_reset() function. */
    if (_on_reset)
    {
        _on_reset();
    }

    /* Reserved for use by MPLAB XC32. */
    if (__xc32_on_reset)
        __xc32_on_reset();

#if (__ARM_FP==14) || (__ARM_FP==4)
    /* Enable the FPU if the application is built with -mfloat-abi=softfp or -mfloat-abi=hard */
    FPU_Enable();
#endif

    /* Disable TCM  */
    TCM_Disable();

    /* Initialize data after TCM is enabled.
     * Data initialization from the XC32 .dinit template */
    __pic32c_data_initialization();
	

#  ifdef SCB_VTOR_TBLOFF_Msk
    /*  Set the vector-table base address in FLASH */
    pSrc = (uint32_t *) & __svectors;
    SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);
#  endif /* SCB_VTOR_TBLOFF_Msk */

    /* Initialize the C library */
    __libc_init_array();

    /* Initialize MPU */
    MPU_Initialize();

    /* Enable Instruction Cache */
    ICache_Enable();

    /* Enable Data Cache    */
    DCache_Enable();

    /* Call the optional application-provided _on_bootstrap() function. */
    if (_on_bootstrap)
    {
        _on_bootstrap();
    }

    /* Reserved for use by MPLAB XC32. */
    if (__xc32_on_bootstrap)
    {
        __xc32_on_bootstrap();
    }

    /* Branch to application's main function */
    main();
#if (defined(__DEBUG) || defined(__DEBUG_D)) && defined(__XC32)
    __builtin_software_breakpoint();
#endif
    /* Infinite loop */
    while (1) {}
}
