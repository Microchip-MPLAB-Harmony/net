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
#include "device_cache.h"
#include "system/cache/sys_cache.h"
#include "peripheral/cache/plib_cache.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Cache Interface Functions
// *****************************************************************************
// *****************************************************************************
void SYS_CACHE_EnableCaches (void)
{
    if (CACHE_CacheCoherencyGet() == CACHE_DISABLE) // If Data and Instruction Cache is disabled
    {
        ICACHE_ENABLE();
        DCACHE_ENABLE();
    }
}

void SYS_CACHE_DisableCaches (void)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE) // If Data and Instruction Cache is enabled
    {
        DCACHE_DISABLE();
        ICACHE_DISABLE();
    }
}
void SYS_CACHE_EnableICache (void)
{
    if (CACHE_CacheCoherencyGet() == CACHE_DISABLE) // If Instruction Cache is disabled
    {
        ICACHE_ENABLE();
    }
}

void SYS_CACHE_DisableICache (void)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE) // If Instruction Cache is enabled
    {
        ICACHE_DISABLE();
    }
}

void SYS_CACHE_EnableDCache (void)
{
    if (CACHE_CacheCoherencyGet() == CACHE_DISABLE) // If Data Cache is disabled
    {
        DCACHE_ENABLE();
    }
}

void SYS_CACHE_DisableDCache (void)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE) // If Data Cache is enabled
    {
        DCACHE_DISABLE();
    }
}

void SYS_CACHE_CleanDCache (void)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE)
    {
        DCACHE_CLEAN();
    }
}

void SYS_CACHE_CleanInvalidateDCache (void)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE)
    {
        DCACHE_INVALIDATE();
    }
}

void SYS_CACHE_InvalidateICache (void)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE)
    {
        ICACHE_INVALIDATE();
    }
}

void SYS_CACHE_InvalidateDCache (void)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE)
    {
        DCACHE_INVALIDATE();
    }
}

void SYS_CACHE_InvalidateDCache_by_Addr (uint32_t *addr, int32_t size)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE) // If Data Cache is enabled
    {
        DCACHE_CLEAN_INVALIDATE_BY_ADDR((uint32_t)addr, (size_t)size);
    }
}

void SYS_CACHE_CleanDCache_by_Addr (uint32_t *addr, int32_t size)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE) // If Data Cache is enabled
    {
        DCACHE_CLEAN_BY_ADDR((uint32_t)addr, (size_t)size);
    }
}

void SYS_CACHE_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t size)
{
    if (CACHE_CacheCoherencyGet() != CACHE_DISABLE) // If Data Cache is enabled
    {
        DCACHE_CLEAN_INVALIDATE_BY_ADDR((uint32_t)addr, (size_t)size);
    }
}
