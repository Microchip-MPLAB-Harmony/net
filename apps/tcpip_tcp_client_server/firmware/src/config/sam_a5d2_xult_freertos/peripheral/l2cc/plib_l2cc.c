/*******************************************************************************
  L2CC PLIB Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    plib_l2cc.c

  Summary:
    Implemenation of the L2 cache controller PLIB.

  Description:
    This file implements the functionality necessary to initialize and
    interract with the L2 cache controller.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdbool.h>
#include <stdint.h>

#include "device.h"
#include "plib_l2cc.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************
#define L2CC_OFFSET_BIT 5
#define L2CC_INDEX_BIT  9
#define L2CC_TAG_BIT    18

static bool l2cacheIsEnabled(void)
{
    return (L2CC_REGS->L2CC_CR & L2CC_CR_L2CEN_Msk) != 0;
}

static void l2cacheEnable(void)
{
    SFR_REGS->SFR_L2CC_HRAMC = SFR_L2CC_HRAMC_SRAM_SEL(1);
    L2CC_REGS->L2CC_CR |= L2CC_CR_L2CEN_Msk;
    __DSB();
    __ISB();
}


static void l2cacheSetNonExclusive(void)
{
    L2CC_REGS->L2CC_ACR &= ~L2CC_ACR_EXCC_Msk;
}



static void setConfig(void)
{
    L2CC_REGS->L2CC_ACR = L2CC_ACR_HPSO(0) |
                     L2CC_ACR_SBDLE(0) |
                     L2CC_ACR_EMBEN(0) |
                     L2CC_ACR_PEN(0) |
                     L2CC_ACR_SAOEN(0) |
                     L2CC_ACR_FWA(0) |
                     L2CC_ACR_CRPOL(1) |
                     L2CC_ACR_NSLEN(0) |
                     L2CC_ACR_NSIAC(0) |
                     L2CC_ACR_DPEN(1) |
                     L2CC_ACR_IPEN(0);

    L2CC_REGS->L2CC_DCR = L2CC_DCR_DCL(0) |
                     L2CC_DCR_DWB(0);

    L2CC_REGS->L2CC_PCR = L2CC_PCR_OFFSET(0) |
                     L2CC_PCR_NSIDEN(0) |
                     L2CC_PCR_IDLEN(0) |
                     L2CC_PCR_PDEN(0) |
                     L2CC_PCR_DLFWRDIS(0) |
                     L2CC_PCR_DATPEN(1) |
                     L2CC_PCR_INSPEN(0) |
                     L2CC_PCR_DLEN(0);

    L2CC_REGS->L2CC_POWCR = L2CC_POWCR_STBYEN(0) |
                       L2CC_POWCR_DCKGATEN(0);
}

static void dataPrefetchEnable(void)
{
    L2CC_REGS->L2CC_PCR |= L2CC_PCR_DATPEN_Msk;
}

static void instPrefetchEnable(void)
{
    L2CC_REGS->L2CC_PCR |= L2CC_PCR_INSPEN_Msk;
}


static void disableInt(uint32_t sources)
{
    L2CC_REGS->L2CC_IMR &= ~sources;
}

static void clearInt(uint32_t sources)
{
    L2CC_REGS->L2CC_ICR |= sources;
}

static void cacheSync(void)
{
    while (L2CC_REGS->L2CC_CSR & L2CC_CSR_C_Msk);
    L2CC_REGS->L2CC_CSR = L2CC_CSR_C_Msk;
    while (L2CC_REGS->L2CC_CSR & L2CC_CSR_C_Msk);
}

static void invalidatePAL(uint32_t phys_addr)
{
    uint32_t tag, index;

    tag = phys_addr >> (L2CC_OFFSET_BIT + L2CC_INDEX_BIT);
    index = (phys_addr >> L2CC_OFFSET_BIT) & ((1 << L2CC_INDEX_BIT) - 1);
    L2CC_REGS->L2CC_IPALR = L2CC_IPALR_TAG(tag) | L2CC_IPALR_IDX(index) | L2CC_IPALR_C_Msk;
    while (L2CC_REGS->L2CC_IPALR & L2CC_IPALR_C_Msk);
}

static void cleanPAL(uint32_t phys_addr)
{
    uint32_t tag, index;

    tag = phys_addr >> (L2CC_OFFSET_BIT + L2CC_INDEX_BIT);
    index = (phys_addr >> L2CC_OFFSET_BIT) & ((1 << L2CC_INDEX_BIT) - 1);
    L2CC_REGS->L2CC_CPALR = L2CC_CPALR_TAG(tag) | L2CC_CPALR_IDX(index) | L2CC_CPALR_C_Msk;
    while (L2CC_REGS->L2CC_CPALR & L2CC_CPALR_C_Msk);
}

static void cleanInvalidatePAL(uint32_t phys_addr)
{
    uint32_t tag, index;

    tag = phys_addr >> (L2CC_OFFSET_BIT + L2CC_INDEX_BIT);
    index = (phys_addr >> L2CC_OFFSET_BIT) & ((1 << L2CC_INDEX_BIT) - 1);
    L2CC_REGS->L2CC_CIPALR = L2CC_CIPALR_TAG(tag) | L2CC_CIPALR_IDX(index) | L2CC_CIPALR_C_Msk;
    while (L2CC_REGS->L2CC_CIPALR & L2CC_CIPALR_C_Msk);
}

static void invalidateWay(uint8_t way)
{
    L2CC_REGS->L2CC_IWR = way;
    while (L2CC_REGS->L2CC_IWR & way);
}

static void cleanWay(uint8_t way)
{
    L2CC_REGS->L2CC_CWR = way;
    while (L2CC_REGS->L2CC_CWR & way);
}

void cleanInvalidateWay(uint8_t way)
{
    L2CC_REGS->L2CC_CIWR = way;
    while (L2CC_REGS->L2CC_CIWR & way);
}

/*
void l2cc_clean_index(uint32_t phys_addr, uint8_t way)
{
    uint32_t index;

    index = (phys_addr >> L2CC_OFFSET_BIT) & ((1 << L2CC_INDEX_BIT) - 1);
    L2CC_REGS->L2CC_CIR = L2CC_CIR_IDX(index) | L2CC_CIR_WAY(way) | L2CC_CIR_C_Msk;
    while (L2CC_REGS->L2CC_CIR & L2CC_CIR_C_Msk);
}

void l2cc_clean_invalidate_index(uint32_t phys_addr, uint8_t way)
{
    uint32_t index;

    index = (phys_addr >> L2CC_OFFSET_BIT) & ((1 << L2CC_INDEX_BIT) - 1);
    L2CC_REGS->L2CC_CIIR = L2CC_CIIR_IDX(index) | L2CC_CIIR_WAY(way) | L2CC_CIIR_C_Msk;
    while (L2CC_REGS->L2CC_CIIR & L2CC_CIIR_C_Msk);
}

void l2cc_data_lockdown(uint8_t way)
{
    L2CC_REGS->L2CC_DLKR = way;
    while (L2CC_REGS->L2CC_CSR & L2CC_CSR_C_Msk);
}

void l2cc_instruction_lockdown(uint8_t way)
{
    L2CC_REGS->L2CC_ILKR = way;
    while (L2CC_REGS->L2CC_CSR & L2CC_CSR_C_Msk);
}
*/

// *****************************************************************************
/* Function:
    void PLIB_L2CC_CleanCache(void);

  Summary:
    Cleans (flushes) the L2 cache.

  Description:
    This function flushes the L2 cache.

  Precondition:
    PLIB_L2CC_Initialize has been called.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_L2CC_CleanCache();
    </code>

*/
void PLIB_L2CC_CleanCache(void)
{
    if (l2cacheIsEnabled()) {
        // forces the address out past level 2
        cleanWay(0xFF);
        // Ensures completion of the L2 clean
        cacheSync();
    }
}

// *****************************************************************************
/* Function:
    void PLIB_L2CC_InvalidateCache(void);

  Summary:
    Invalidates L2 cache entries.

  Description:
    This function will mark all cache entries as invalid.

  Precondition:
    PLIB_L2CC_Initialize has been called.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_L2CC_InvalidateCache();
    </code>

*/
void PLIB_L2CC_InvalidateCache(void)
{
    if (l2cacheIsEnabled()) {
        // forces the address out past level 2
        invalidateWay(0xFF);
        // Ensures completion of the L2 inval
        cacheSync();
    }
}

// *****************************************************************************
/* Function:
    void PLIB_L2CC_CleanInvalidateCache(void);

  Summary:
    Cleans and Invalidates L2 cache entries.

  Description:
    This function will clean all dirty entries and then mark all cache entries
    as invalid.

  Precondition:
    PLIB_L2CC_Initialize has been called.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_L2CC_CleanInvalidateCache();
    </code>

*/
void PLIB_L2CC_CleanInvalidateCache(void)
{
    if (l2cacheIsEnabled()) {
        /* forces the address out past level 2 */
        cleanInvalidateWay(0xFF);
        /* Ensures completion of the L2 inval */
        cacheSync();
    }
}

// *****************************************************************************
/* Function:
    void PLIB_L2CC_InvalidateCacheByAddr(uint32_t *addr, uint32_t size);

  Summary:
    Invalidates L2 cache entries.

  Description:
    This function will mark cache entries within the givin address range as
    invalid.

  Precondition:
    PLIB_L2CC_Initialize has been called.

  Parameters:
    addr        - The start address of the memory range to be invalidated

    size        - The size of the memory range to be invalidated

  Returns:
    None.

  Example:
    <code>
    PLIB_L2CC_InvalidateCacheByAddr(dmaMem, dmaMemSize);
    </code>

*/
void PLIB_L2CC_InvalidateCacheByAddr(uint32_t *addr, uint32_t size)
{
    uint32_t start = (uint32_t)addr;
    uint32_t end = start + size;
    uint32_t current = start & ~0x1f;
    if (l2cacheIsEnabled()) {
        while (current <= end) {
            invalidatePAL(current);
            current += 32;
        }
        invalidatePAL(end);
    }
}

// *****************************************************************************
/* Function:
    void PLIB_L2CC_CleanCacheByAddr(uint32_t *addr, uint32_t size);

  Summary:
    Flushes L2 cache entries.

  Description:
    This function will flush all dirty cache entries within the given address
    range.

  Precondition:
    PLIB_L2CC_Initialize has been called.

  Parameters:
    addr        - The start address of the memory range to be cleaned

    size        - The size of the memory range to be cleaned

  Returns:
    None.

  Example:
    <code>
    PLIB_L2CC_CleanCacheByAddr(dmaMem, dmaMemSize);
    </code>

*/
void PLIB_L2CC_CleanCacheByAddr(uint32_t *addr, uint32_t size)
{
    uint32_t start = (uint32_t)addr;
    uint32_t end = start + size;
    uint32_t current = start & ~0x1f;
    if (l2cacheIsEnabled()) {
        while (current <= end) {
            cleanPAL(current);
            current += 32;
        }
        cleanPAL(end);
    }
}

// *****************************************************************************
/* Function:
    void PLIB_L2CC_CleanInvalidateCacheByAddr(uint32_t *addr, uint32_t size);

  Summary:
    Cleans and Invalidates L2 cache entries.

  Description:
    This function will clean all dirty entries and then mark all cache entries
    as invalid within the given range.

  Precondition:
    PLIB_L2CC_Initialize has been called.

  Parameters:
    addr        - The start address of the memory range to be cleaned

    size        - The size of the memory range to be cleaned

  Returns:
    None.

  Example:
    <code>
    PLIB_L2CC_CleanInvalidateCacheByAddr(dmaMem, dmaMemSize);
    </code>

*/
void PLIB_L2CC_CleanInvalidateCacheByAddr(uint32_t *addr, uint32_t size)
{
    uint32_t start = (uint32_t)addr;
    uint32_t end = start + size;
    uint32_t current = start & ~0x1f;
    if (l2cacheIsEnabled()) {
        while (current <= end) {
            cleanInvalidatePAL(current);
            current += 32;
        }
        cleanInvalidatePAL(end);
    }
}

// *****************************************************************************
/* Function:
    void PLIB_L2CC_Initialize(void);

  Summary:
    Initialize the L2 cache controller.

  Description:
    This function initializes the L2 cache controller.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_L2CC_Initialize();
    </code>

*/
void PLIB_L2CC_Initialize(void)
{

    /* Set configuration */
    setConfig();



    /* Enable Prefetch */
    instPrefetchEnable();
    dataPrefetchEnable();

    /* Invalidate whole L2CC */
    invalidateWay(0xFF);

    /* Disable all L2CC Interrupt */
    disableInt(0x1FF);

    /* Clear all L2CC Interrupt */
    clearInt(0x1FF);

    /* Set non-exclusive mode */
    l2cacheSetNonExclusive();

    /* Enable L2CC */
    l2cacheEnable();
}
