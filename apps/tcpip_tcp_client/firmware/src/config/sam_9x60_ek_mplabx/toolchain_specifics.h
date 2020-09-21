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

#ifndef TOOLCHAIN_SPECIFICS_H
#define TOOLCHAIN_SPECIFICS_H

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif


#ifndef __NOP
#define __NOP __arm926_nop
static inline void __arm926_nop(void)
{
    asm("nop");
}
#endif //__NOP

static inline void __disable_irq( void )
{   // read, modify and write back the CPSR
    asm("MRS r0, cpsr");
    asm("ORR r0, r0, #0xC0");
    asm("MSR cpsr_c, r0");
}

static inline void __enable_irq( void )
{   // read, modify and write back the CPSR
    asm("MRS r0, cpsr");
    asm("BIC r0, r0, #0x80");
    asm("MSR cpsr_c, r0");
}

#ifndef __DMB
#define __DMB    __arm926_dmb
static inline void __arm926_dmb(void)
{
    asm("" ::: "memory");
}
#endif //__DMB

#ifndef __DSB
#define __DSB    __arm926_dsb
static inline void __arm926_dsb(void)
{
    asm("mcr p15, 0, %0, c7, c10, 4" :: "r"(0) : "memory");
}
#endif //__DSB

#ifndef __ISB
#define __ISB __arm926_isb
static inline void __arm926_isb(void)
{
    asm("" ::: "memory");
}
#endif //__ISB

#define __ALIGNED(x) __attribute__((aligned(x)))

#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif //__STATIC_INLINE

#ifndef   __WEAK
#define __WEAK __attribute__((weak))
#endif // __WEAK
#include <sys/types.h>

#define NO_INIT        __attribute__((section(".no_init")))
#define SECTION(a)     __attribute__((__section__(a)))

#define CACHE_LINE_SIZE    (32u)
#define CACHE_ALIGN        __ALIGNED(CACHE_LINE_SIZE)

#ifndef FORMAT_ATTRIBUTE
   #define FORMAT_ATTRIBUTE(archetype, string_index, first_to_check)  __attribute__ ((format (archetype, string_index, first_to_check)))
#endif


#ifdef __cplusplus
}
#endif

#endif // end of header

