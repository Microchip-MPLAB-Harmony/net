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
