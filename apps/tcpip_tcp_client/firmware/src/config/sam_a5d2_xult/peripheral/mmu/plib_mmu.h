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
