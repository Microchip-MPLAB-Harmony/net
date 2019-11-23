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

// *****************************************************************************
// *****************************************************************************
// Section: System Cache Interface Functions
// *****************************************************************************
// *****************************************************************************

/*
 * Enable both Data and Instruction Caches.
 */
void SYS_CACHE_EnableCaches (void)
{
    L1_ICACHE_ENABLE();
    L1_DCACHE_ENABLE();
}

/*
 * Disable both Data and Instruction Caches.
 */
void SYS_CACHE_DisableCaches (void)
{
    L1_ICACHE_DISABLE();
    L1_DCACHE_DISABLE();
}

/*
 * Enable Instruction Cache.
 */
void SYS_CACHE_EnableICache (void)
{
    L1_ICACHE_ENABLE();
}

/*
 * Disable Instruction Cache.
 */
void SYS_CACHE_DisableICache (void)
{
    L1_ICACHE_DISABLE();
}

/*
 * Invalidate Instruction Cache.
 */
void SYS_CACHE_InvalidateICache (void)
{
    L1_ICACHE_INVALIDATE_ALL();
}

/*
 * Enable Data Cache.
 */
void SYS_CACHE_EnableDCache (void)
{
    L1_DCACHE_ENABLE();
}

/*
 * Disable Data Cache.
 */
void SYS_CACHE_DisableDCache (void)
{
    L1_DCACHE_DISABLE();
}

/*
 * Invalidate whole Data Cache.
 */
void SYS_CACHE_InvalidateDCache (void)
{
    DCACHE_INVALIDATE_ALL();
}

/*
 * Clean whole Data Cache.
 */
void SYS_CACHE_CleanDCache (void)
{
    DCACHE_CLEAN_ALL();
}

/*
 * Clean and Invalidate whole Data Cache.
 */
void SYS_CACHE_CleanInvalidateDCache (void)
{
    DCACHE_CLEAN_INVALIDATE_ALL();
}

/*
 * Invalidate Data Cache by address.
 */
void SYS_CACHE_InvalidateDCache_by_Addr (uint32_t *addr, int32_t size)
{
    DCACHE_INVALIDATE_BY_ADDR(addr,(uint32_t)size);
}

/*
 * Clean Data Cache by address.
 */
void SYS_CACHE_CleanDCache_by_Addr (uint32_t *addr, int32_t size)
{
    DCACHE_CLEAN_BY_ADDR(addr,(uint32_t)size);
}

/*
 * Clean and Invalidate Data Cache by address.
 */
void SYS_CACHE_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t size)
{
    DCACHE_CLEAN_INVALIDATE_BY_ADDR(addr,(uint32_t)size);
}
