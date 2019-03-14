/*******************************************************************************
  L2CC PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_l2cc.h

  Summary:
    L2 Cache Controller PLIB Header FIle

  Description:
    This library provides an interface to interact with the L2 cache controller
    module.
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

#ifndef _PLIB_L2CC_H
#define _PLIB_L2CC_H

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

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
extern void PLIB_L2CC_Initialize(void);

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
extern void PLIB_L2CC_CleanCache(void);

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
extern void PLIB_L2CC_InvalidateCache(void);

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
extern void PLIB_L2CC_CleanInvalidateCache(void);

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
extern void PLIB_L2CC_InvalidateCacheByAddr(uint32_t *addr, uint32_t size);

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
extern void PLIB_L2CC_CleanCacheByAddr(uint32_t *addr, uint32_t size);

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
extern void PLIB_L2CC_CleanInvalidateCacheByAddr(uint32_t *addr, uint32_t size);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif
