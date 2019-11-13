/*******************************************************************************
  MMU PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_mmu.h

  Summary:
    MMU PLIB external functions declartions

  Description:
    The MMU PLIB supports the MMU found on Cortex A processors.  This library
    sets up the MMU with a flat 1:1 mapping of the address space and enables
    the instruction and data cache.
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

#ifndef _PLIB_MMU_H    // Guards against multiple inclusion
#define _PLIB_MMU_H

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif


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
void MMU_Initialize(void);

void icache_InvalidateAll(void);
void icache_Enable(void);
void icache_Disable(void);
void dcache_InvalidateAll(void);
void dcache_CleanAll(void);
void dcache_CleanInvalidateAll(void);
void dcache_InvalidateByAddr (uint32_t *addr, uint32_t size);
void dcache_CleanByAddr (uint32_t *addr, uint32_t size);
void dcache_CleanInvalidateByAddr (uint32_t *addr, uint32_t size);
void dcache_Enable(void);
void dcache_Disable(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif // _PLIB_MMU_H
