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

__STATIC_INLINE void FPU_Enable(void);

#if (__FPU_PRESENT)

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
#endif
__STATIC_INLINE void TCM_Enable(void);
__STATIC_INLINE void TCM_Configure(uint32_t tcmSize);
__STATIC_INLINE void ICache_Enable(void);
/** Program CMCC CSIZESW bits for TCM and cache configuration */
__STATIC_INLINE void TCM_Configure(uint32_t tcmSize)
{
    CMCC_REGS->CMCC_CFG = CMCC_CFG_CSIZESW(tcmSize);
}

/** Enable TCM memory */
__STATIC_INLINE void  TCM_Enable(void)
{
    /* TCM cannot be enabled or disabled in SAME5x/SAMD5x family*/
}

__STATIC_INLINE void ICache_Enable(void)
{
    CMCC_REGS->CMCC_CTRL &= ~(CMCC_CTRL_CEN_Msk);
    while((CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk) == CMCC_SR_CSTS_Msk)
    {
        /*Wait for the operation to complete*/
    }
    CMCC_REGS->CMCC_CFG |= (CMCC_CFG_DCDIS_Msk);
    CMCC_REGS->CMCC_CTRL = (CMCC_CTRL_CEN_Msk);
}

void Reset_Handler(void)
{
    #if (__FPU_PRESENT)
    /* Enable the FPU if the application is built with -mfloat-abi=softfp or -mfloat-abi=hard */
    FPU_Enable();
    #endif

    TCM_Configure(2);
    /* Enable TCM   */
    TCM_Enable();

     /* Execute relocations & zero BSS */
     __iar_data_init3();

    #pragma section=".intvec"
    uint32_t* pSrc;

    /*  Set the vector-table base address in FLASH */
    pSrc = __sfb( ".intvec" );
    SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

    /* Enable Instruction Cache */
    ICache_Enable();


    /* branch to main function */
    main();

    /* program done, block in infinite loop */
    while (1);
}
