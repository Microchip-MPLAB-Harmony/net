/*******************************************************************************
  L1, L2 Cache Header

  File Name:
    cache_cortex_a.h

  Summary:
    Preprocessor definitions to provide L1 and L2 Cache control.

  Description:
    An MPLAB PLIB or Project can include this header to perform cache cleans,
    invalidates etc. For the DCache and ICache.

  Remarks:
    This header should not define any prototypes or data definitions, or
    include any files that do.  The file only provides macro definitions for
    build-time.

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

#ifndef CACHE_CORTEX_A_H
#define CACHE_CORTEX_A_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/

#include <stdint.h>
#include "peripheral/l2cc/plib_l2cc.h"
#include "peripheral/mmu/plib_mmu.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: L1, L2 Cache Configuration
// *****************************************************************************
// *****************************************************************************
#define L1_ICACHE_IN_USE                               true
#define L1_ICACHE_ENABLE()                             icache_Enable()
#define L1_ICACHE_DISABLE()                            icache_Disable()
#define L1_ICACHE_INVALIDATE_ALL()                     icache_InvalidateAll()

#define L1_DCACHE_IN_USE                               true
#define L1_DCACHE_ENABLE()                             dcache_Enable()
#define L1_DCACHE_DISABLE()                            dcache_Disable()
#define L1_DCACHE_CLEAN_ALL()                          dcache_CleanAll()
#define L1_DCACHE_INVALIDATE_ALL()                     dcache_InvalidateAll()
#define L1_DCACHE_CLEAN_INVALIDATE_ALL()               dcache_CleanInvalidateAll()

#define L2_DCACHE_IN_USE                               true
#define L2_DCACHE_CLEAN_ALL()                          PLIB_L2CC_CleanCache()
#define L2_DCACHE_INVALIDATE_BY_ADDR(addr,sz)          PLIB_L2CC_InvalidateCacheByAddr(addr,sz)
//
#define DCACHE_CLEAN_BY_ADDR(addr,sz)                  dcache_CleanByAddr(addr,sz);\
                                                       PLIB_L2CC_CleanCacheByAddr(addr,sz)
#define DCACHE_INVALIDATE_BY_ADDR(addr,sz)             PLIB_L2CC_InvalidateCacheByAddr(addr,sz);\
                                                       dcache_InvalidateByAddr(addr,sz)
#define DCACHE_CLEAN_INVALIDATE_BY_ADDR(addr,sz)       dcache_CleanInvalidateByAddr(addr,sz);\
                                                       PLIB_L2CC_CleanInvalidateCacheByAddr(addr,sz)
#define DCACHE_CLEAN_ALL()                             dcache_CleanAll();\
                                                       PLIB_L2CC_CleanCache()
#define DCACHE_INVALIDATE_ALL()                        PLIB_L2CC_InvalidateCache();\
                                                       dcache_InvalidateAll()
#define DCACHE_CLEAN_INVALIDATE_ALL()                  dcache_CleanInvalidateAll();\
                                                       PLIB_L2CC_CleanInvalidateCache()
//
#define DATA_CACHE_ENABLED                             true

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // end of header
