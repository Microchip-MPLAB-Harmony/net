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

#include "definitions.h" /* for potential custom handler names */

extern void main(void);
void __iar_data_init3(void);

__STATIC_INLINE void TCM_Disable(void);
__STATIC_INLINE void TCM_Configure(uint32_t tcmSize);
__STATIC_INLINE void ICache_Enable(void);
__STATIC_INLINE void DCache_Enable(void);
#if (__FPU_USED == 1)
__STATIC_INLINE void FPU_Enable(void);
#endif //__FPU_USED

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

#if (__FPU_USED == 1)

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
#endif /* (__FPU_USED == 1) */
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

/* Disable TCM memory */
__STATIC_INLINE void TCM_Disable(void)
{
    __DSB();
    __ISB();
    SCB->ITCMCR &= ~(uint32_t)SCB_ITCMCR_EN_Msk;
    SCB->DTCMCR &= ~(uint32_t)SCB_ITCMCR_EN_Msk;
    __DSB();
    __ISB();
}


void Reset_Handler(void)
{
    #if (__FPU_USED == 1)
    /* Enable the FPU if the application is built with --fpu option */
    FPU_Enable();
    #endif /* __FPU_USED */

    TCM_Configure(0);
    /* Disable TCM  */
    TCM_Disable();

     /* Execute relocations & zero BSS */
     __iar_data_init3();

    #pragma section=".intvec"
    uint32_t* pSrc;

    /*  Set the vector-table base address in FLASH */
    pSrc = __sfb( ".intvec" );
    SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);
    /* Initialize MPU */
    MPU_Initialize();

    /* Enable Instruction Cache */
    ICache_Enable();

    /* Enable Data Cache    */
    DCache_Enable();

    /* branch to main function */
    main();

    /* program done, block in infinite loop */
    while (1);
}
