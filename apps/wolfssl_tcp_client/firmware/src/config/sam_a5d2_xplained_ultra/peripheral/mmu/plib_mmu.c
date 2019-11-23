/*******************************************************************************
   PLIB MMU

  Company:
    Microchip Technology Inc.

  File Name:
    plib_mmu.c

  Summary:
    MMU implementation.

  Description:
    The MMU PLIB provies a simple interface to enable the MMU and caches.
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

#include "device.h"

/* TTB descriptor type for Section descriptor */
#define TTB_TYPE_SECT              (2 << 0)

/* TTB Section Descriptor: Buffered/Non-Buffered (B) */
#define TTB_SECT_WRITE_THROUGH     (0 << 2)
#define TTB_SECT_WRITE_BACK        (1 << 2)

/* TTB Section Descriptor: Cacheable/Non-Cacheable (C) */
#define TTB_SECT_NON_CACHEABLE     (0 << 3)
#define TTB_SECT_CACHEABLE         (1 << 3)

#define TTB_SECT_STRONGLY_ORDERED  (TTB_SECT_NON_CACHEABLE | TTB_SECT_WRITE_THROUGH)
#define TTB_SECT_SHAREABLE_DEVICE  (TTB_SECT_NON_CACHEABLE | TTB_SECT_WRITE_BACK)
#define TTB_SECT_CACHEABLE_WT      (TTB_SECT_CACHEABLE | TTB_SECT_WRITE_THROUGH)
#define TTB_SECT_CACHEABLE_WB      (TTB_SECT_CACHEABLE | TTB_SECT_WRITE_BACK)

/* TTB Section Descriptor: Domain */
#define TTB_SECT_DOMAIN(x)         (((x) & 15) << 5)

/* TTB Section Descriptor: Execute/Execute-Never (XN) */
#define TTB_SECT_EXEC              (0 << 4)
#define TTB_SECT_EXEC_NEVER        (1 << 4)

/* TTB Section Descriptor: Access Privilege (AP) */
#define TTB_SECT_AP_PRIV_ONLY      ((0 << 15) | (1 << 10))
#define TTB_SECT_AP_NO_USER_WRITE  ((0 << 15) | (2 << 10))
#define TTB_SECT_AP_FULL_ACCESS    ((0 << 15) | (3 << 10))
#define TTB_SECT_AP_PRIV_READ_ONLY ((1 << 15) | (1 << 10))
#define TTB_SECT_AP_READ_ONLY      ((1 << 15) | (2 << 10))

/* TTB Section Descriptor: Section Base Address */
#define TTB_SECT_ADDR(x)           ((x) & 0xFFF00000)
/* L1 data cache line size, Number of ways and Number of sets */
#define L1_DATA_CACHE_BYTES        32U
#define L1_DATA_CACHE_WAYS         4U
#define L1_DATA_CACHE_SETS         256U
#define L1_DATA_CACHE_SETWAY(set, way) (((set) << 5) | ((way) << 30))

__ALIGNED(16384) static uint32_t tlb[4096];

// *****************************************************************************
/* Function:
     void mmu_configure(void *tlb)

  Summary:
    Configure MMU by setting TTRB0 address.

*/
static void mmu_configure(void *tlb)
{
    /* Translation Table Base Register 0 */
    __set_TTBR0((uint32_t)tlb);

    /* Domain Access Register */
    /* only domain 15: access are not checked */
    __set_DACR(0xC0000000);

    __DSB();
    __ISB();
}

// *****************************************************************************
/* Function:
     void mmu_enable(void)

  Summary:
    Enable the MMU.

*/
static void mmu_enable(void)
{
    uint32_t control;

    control = __get_SCTLR();
    if ((control & SCTLR_M_Msk) == 0)
        __set_SCTLR(control | SCTLR_M_Msk);
}

void icache_InvalidateAll(void)
{
    __set_ICIALLU(0);
    __ISB();
}

void icache_Enable(void)
{
    uint32_t sctlr = __get_SCTLR();
    if ((sctlr & SCTLR_I_Msk) == 0)
    {
        icache_InvalidateAll();
        __set_SCTLR(sctlr | SCTLR_I_Msk);
    }
}

void icache_Disable(void)
{
    uint32_t sctlr = __get_SCTLR();
    if (sctlr & SCTLR_I_Msk)
    {
        __set_SCTLR(sctlr & ~SCTLR_I_Msk);
        icache_InvalidateAll();
    }
}

void dcache_InvalidateAll(void)
{
    uint32_t set, way;

    for (way = 0; way < L1_DATA_CACHE_WAYS; way++)
    {
        for (set = 0; set < L1_DATA_CACHE_SETS; set++)
        {
            __set_DCISW(L1_DATA_CACHE_SETWAY(set, way));
        }
    }
    __DSB();
}

void dcache_CleanAll(void)
{
    uint32_t set, way;

    for (way = 0; way < L1_DATA_CACHE_WAYS; way++)
    {
        for (set = 0; set < L1_DATA_CACHE_SETS; set++)
        {
            __set_DCCSW(L1_DATA_CACHE_SETWAY(set, way));
        }
    }
    __DSB();
}

void dcache_CleanInvalidateAll(void)
{
    uint32_t set, way;

    for (way = 0; way < L1_DATA_CACHE_WAYS; way++)
    {
        for (set = 0; set < L1_DATA_CACHE_SETS; set++)
        {
            __set_DCCISW(L1_DATA_CACHE_SETWAY(set, way));
        }
    }
    __DSB();
}

void dcache_InvalidateByAddr (uint32_t *addr, uint32_t size)
{
    uint32_t mva = (uint32_t)addr & ~(L1_DATA_CACHE_BYTES - 1);

    for ( ; mva < ((uint32_t)addr + size); mva += L1_DATA_CACHE_BYTES)
    {
        __set_DCIMVAC(mva);
        __DMB();
    }
    __DSB();
}

void dcache_CleanByAddr (uint32_t *addr, uint32_t size)
{
    uint32_t mva = (uint32_t)addr & ~(L1_DATA_CACHE_BYTES - 1);

    for ( ; mva < ((uint32_t)addr + size); mva += L1_DATA_CACHE_BYTES)
    {
        __set_DCCMVAC(mva);
        __DMB();
    }
    __DSB();
}

void dcache_CleanInvalidateByAddr (uint32_t *addr, uint32_t size)
{
    uint32_t mva = (uint32_t)addr & ~(L1_DATA_CACHE_BYTES - 1);

    for ( ; mva < ((uint32_t)addr + size); mva += L1_DATA_CACHE_BYTES)
    {
        __set_DCCIMVAC((uint32_t)mva);
        __DMB();
    }
    __DSB();
}

void dcache_Enable(void)
{
    uint32_t sctlr = __get_SCTLR();
    if ((sctlr & SCTLR_C_Msk) == 0)
    {
        dcache_InvalidateAll();
        __set_SCTLR(sctlr | SCTLR_C_Msk);
    }
}

void dcache_Disable(void)
{
    uint32_t sctlr = __get_SCTLR();
    if (sctlr & SCTLR_C_Msk)
    {
        dcache_CleanAll();
        __set_SCTLR(sctlr & ~SCTLR_C_Msk);
        dcache_InvalidateAll();
    }
}


static inline uint32_t cp15_read_sctlr(void)
{
    uint32_t sctlr = 0;
    asm("mrc p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
    return sctlr;
}

static inline void cp15_write_sctlr(uint32_t value)
{
    asm("mcr p15, 0, %0, c1, c0, 0" :: "r"(value));
}

// *****************************************************************************
/* Function:
    void MMU_Initialize(void);

  Summary:
    Initialize and enable MMU.

  Description:
    Initialize the MMU with a flat address map (e.g. physical and virtual
    addresses are the same) and enable MMU and caches.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.
*/
void MMU_Initialize(void)
{
    uint32_t addr;

    /* Reset table entries */
    for (addr = 0; addr < 4096; addr++)
        tlb[addr] = 0;

    /* 0x00000000: ROM */
    tlb[0x000] = TTB_SECT_ADDR(0x00000000)
               | TTB_SECT_AP_READ_ONLY
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC
               | TTB_SECT_CACHEABLE_WB
               | TTB_TYPE_SECT;

    /* 0x00100000: NFC SRAM */
    tlb[0x001] = TTB_SECT_ADDR(0x00100000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;

    /* 0x00200000: SRAM */
    tlb[0x002] = TTB_SECT_ADDR(0x00200000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC
               | TTB_SECT_CACHEABLE_WB
               | TTB_TYPE_SECT;

    /* 0x00300000: UDPHS (RAM) */
    tlb[0x003] = TTB_SECT_ADDR(0x00300000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;

    /* 0x00400000: UHPHS (OHCI) */
    tlb[0x004] = TTB_SECT_ADDR(0x00400000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;

    /* 0x00500000: UDPHS (EHCI) */
    tlb[0x005] = TTB_SECT_ADDR(0x00500000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;

    /* 0x00600000: AXIMX */
    tlb[0x006] = TTB_SECT_ADDR(0x00600000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;

    /* 0x00700000: DAP */
    tlb[0x007] = TTB_SECT_ADDR(0x00700000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;
    /* 0x00800000: pPP */
    tlb[0x008] = TTB_SECT_ADDR(0x00800000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;
    /* 0x00a00000: L2CC */
    tlb[0x00a] = TTB_SECT_ADDR(0x00a00000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;
    tlb[0x00b] = TTB_SECT_ADDR(0x00b00000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC_NEVER
               | TTB_SECT_SHAREABLE_DEVICE
               | TTB_TYPE_SECT;

    /* 0x10000000: EBI Chip Select 0 */
    for (addr = 0x100; addr < 0x200; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC_NEVER
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0x20000000: DDR Chip Select */
    /* (16MB strongly ordered, 448MB cacheable) */
    for (addr = 0x200; addr < 0x210; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;
    for (addr = 0x210; addr < 0x400; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC
                      | TTB_SECT_CACHEABLE_WB
                      | TTB_TYPE_SECT;

    /* 0x40000000: DDR AESB Chip Select */
    for (addr = 0x400; addr < 0x600; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC
                      | TTB_SECT_CACHEABLE_WB
                      | TTB_TYPE_SECT;

    /* 0x60000000: EBI Chip Select 1 */
    for (addr = 0x600; addr < 0x700; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC_NEVER
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0x70000000: EBI Chip Select 2 */
    for (addr = 0x700; addr < 0x800; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC_NEVER
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0x80000000: EBI Chip Select 3 */
    for (addr = 0x800; addr < 0x900; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC_NEVER
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0x90000000: QSPI0/1 AESB MEM */
    for (addr = 0x900; addr < 0xa00; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0xa0000000: SDMMC0 */
    for (addr = 0xa00; addr < 0xb00; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC_NEVER
                      //| TTB_SECT_SHAREABLE_DEVICE
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0xb0000000: SDMMC1 */
    for (addr = 0xb00; addr < 0xc00; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC_NEVER
                      //| TTB_SECT_SHAREABLE_DEVICE
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0xc0000000: NFC Command Register */
    for (addr = 0xc00; addr < 0xd00; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC_NEVER
                      //| TTB_SECT_SHAREABLE_DEVICE
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0xd0000000: QSPI0/1 MEM */
    for (addr = 0xd00; addr < 0xe00; addr++)
        tlb[addr] = TTB_SECT_ADDR(addr << 20)
                      | TTB_SECT_AP_FULL_ACCESS
                      | TTB_SECT_DOMAIN(0xf)
                      | TTB_SECT_EXEC
                      | TTB_SECT_STRONGLY_ORDERED
                      | TTB_TYPE_SECT;

    /* 0xf0000000: Internal Peripherals */
    tlb[0xf00] = TTB_SECT_ADDR(0xf0000000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC
               | TTB_SECT_STRONGLY_ORDERED
               | TTB_TYPE_SECT;

    /* 0xf8000000: Internal Peripherals */
    tlb[0xf80] = TTB_SECT_ADDR(0xf8000000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC
               | TTB_SECT_STRONGLY_ORDERED
               | TTB_TYPE_SECT;

    /* 0xfc000000: Internal Peripherals */
    tlb[0xfc0] = TTB_SECT_ADDR(0xfc000000)
               | TTB_SECT_AP_FULL_ACCESS
               | TTB_SECT_DOMAIN(0xf)
               | TTB_SECT_EXEC
               | TTB_SECT_STRONGLY_ORDERED
               | TTB_TYPE_SECT;

    /* Enable MMU, I-Cache and D-Cache */
    mmu_configure(tlb);
    icache_Enable();
    mmu_enable();
    dcache_Enable();

    // disable the processor alignment fault testing
    uint32_t sctlrValue = cp15_read_sctlr();
    sctlrValue &= ~0x00000002;
    cp15_write_sctlr( sctlrValue );
}
