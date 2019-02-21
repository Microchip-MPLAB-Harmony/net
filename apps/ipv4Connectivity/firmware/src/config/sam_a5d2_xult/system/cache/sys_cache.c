/*******************************************************************************
  Cache System Service Library Implementation Source File

  Company
    Microchip Technology Inc.

  File Name
    sys_cache.c

  Summary
    Cache System Service Library interface implementation.

  Description
    This file implements the interface to the Cache System Service Library.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "device.h"
#include "system/cache/sys_cache.h"
#include "peripheral/l2cc/plib_l2cc.h"

#define DATA_CACHE_IS_ENABLED            (__get_SCTLR() & (uint32_t)SCTLR_C_Msk)
#define INSTRUCTION_CACHE_IS_ENABLED     (__get_SCTLR() & (uint32_t)SCTLR_I_Msk)
#define L2_DATA_CACHE_IS_ENABLED         (L2CC_REGS->L2CC_CR & L2CC_CR_L2CEN_Msk)

// *****************************************************************************
// *****************************************************************************
// Section: Local Functions
// *****************************************************************************
// *****************************************************************************

/*
 * Enable Instruction Cache by setting I bit in SCTLR register.
 */
static inline void L1C_EnableICache(void)
{
    __set_SCTLR( __get_SCTLR() | SCTLR_I_Msk );
    __ISB();
}

/*
 * Disable Instruction Cache by clearing I bit in SCTLR register.
 */
static inline void L1C_DisableICache(void)
{
    __set_SCTLR( __get_SCTLR() & (~SCTLR_I_Msk) );
    __ISB();
}

/*
 * Enable Data Cache by setting C bit in SCTLR register.
 */
static inline void L1C_EnableDCache(void)
{
    __set_SCTLR( __get_SCTLR() | SCTLR_C_Msk );
    __ISB();
}

/*
 * Disable Data Cache by clearing C bit in SCTLR register.
 */
static inline void L1C_DisableDCache(void)
{
    __set_SCTLR( __get_SCTLR() & (~SCTLR_C_Msk) );
    __ISB();
}

// *****************************************************************************
// *****************************************************************************
// Section: System Cache Interface Functions
// *****************************************************************************
// *****************************************************************************

/*
 * Enables both Data and Instruction Caches by setting C and I bits in SCTLR register.
 */
void SYS_CACHE_EnableCaches (void)
{
    if ((DATA_CACHE_IS_ENABLED == 0) && (INSTRUCTION_CACHE_IS_ENABLED == 0)) // If Data and Instruction Caches are disabled
    {
        L1C_EnableCaches();
    }
}

/*
 * Disables both Data and Instruction Caches by clearing C and I bits in SCTLR register.
 */
void SYS_CACHE_DisableCaches (void)
{
    if (DATA_CACHE_IS_ENABLED && INSTRUCTION_CACHE_IS_ENABLED) // If Data and Instruction Caches are enabled
    {
        L1C_DisableCaches();
    }
}

void SYS_CACHE_EnableICache (void)
{
    if (INSTRUCTION_CACHE_IS_ENABLED == 0) // If Instruction Cache is disabled
    {
        L1C_EnableICache();
    }
}

void SYS_CACHE_DisableICache (void)
{
    if (INSTRUCTION_CACHE_IS_ENABLED) // If Instruction Cache is enabled
    {
        L1C_DisableICache();
    }
}

void SYS_CACHE_InvalidateICache (void)
{
    if (INSTRUCTION_CACHE_IS_ENABLED) // If Instruction Cache is enabled
    {
        L1C_InvalidateICacheAll();
    }
}

void SYS_CACHE_EnableDCache (void)
{
    if (DATA_CACHE_IS_ENABLED == 0) // If Data Cache is disabled
    {
        L1C_EnableDCache();
    }
}

void SYS_CACHE_DisableDCache (void)
{
    if (DATA_CACHE_IS_ENABLED) // If Data Cache is enabled
    {
        L1C_DisableDCache();
    }
}

void SYS_CACHE_InvalidateDCache (void)
{
    if (DATA_CACHE_IS_ENABLED)
    {
        L1C_InvalidateDCacheAll();
    }

    if (L2_DATA_CACHE_IS_ENABLED && DATA_CACHE_IS_ENABLED)
    {
        PLIB_L2CC_InvalidateCache();
    }
}

void SYS_CACHE_CleanDCache (void)
{
    if (DATA_CACHE_IS_ENABLED)
    {
        L1C_CleanDCacheAll();
    }

    if (L2_DATA_CACHE_IS_ENABLED && DATA_CACHE_IS_ENABLED)
    {
        PLIB_L2CC_CleanCache();
    }
}

void SYS_CACHE_CleanInvalidateDCache (void)
{
    if (DATA_CACHE_IS_ENABLED)
    {
        L1C_CleanInvalidateDCacheAll();
    }

    if (L2_DATA_CACHE_IS_ENABLED && DATA_CACHE_IS_ENABLED)
    {
        PLIB_L2CC_CleanInvalidateCache();
    }
}

void SYS_CACHE_InvalidateDCache_by_Addr (uint32_t *addr, int32_t size)
{
    if (L2_DATA_CACHE_IS_ENABLED && DATA_CACHE_IS_ENABLED)
    {
        PLIB_L2CC_InvalidateCacheByAddr(addr, size);
    }

    if (DATA_CACHE_IS_ENABLED)
    {
        L1C_InvalidateDCacheAll();
    }
}

void SYS_CACHE_CleanDCache_by_Addr (uint32_t *addr, int32_t size)
{
    if (DATA_CACHE_IS_ENABLED)
    {
        L1C_CleanDCacheAll();
    }

    if (L2_DATA_CACHE_IS_ENABLED && DATA_CACHE_IS_ENABLED)
    {
        PLIB_L2CC_CleanCacheByAddr(addr, size);
    }
}

void SYS_CACHE_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t size)
{
    if (L2_DATA_CACHE_IS_ENABLED || DATA_CACHE_IS_ENABLED)
    {
        L1C_CleanInvalidateDCacheAll();
    }

    if (L2_DATA_CACHE_IS_ENABLED && DATA_CACHE_IS_ENABLED)
    {
        PLIB_L2CC_CleanInvalidateCacheByAddr(addr, size);
    }
}
