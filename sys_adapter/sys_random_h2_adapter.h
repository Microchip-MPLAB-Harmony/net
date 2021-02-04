/*******************************************************************************
  Adapter Random System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_random_h2_adapter.h

  Summary
    Random system service library interface.

  Description
    This is the adapter file defines the interface to the Random system service 
    library from H2. The random Clock service APIs are used to provide an adapter 
    routines which are part of H2.

  Remarks:
   
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2017-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

// DOM-IGNORE-END

#ifndef SYS_RANDOM_ADPATER_H    // Guards against multiple inclusion
#define SYS_RANDOM_ADPATER_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "toolchain_specifics.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/*  The following data type definitions are used by the functions in this
    interface and should be considered part it.
*/

static __inline__ void __attribute__((always_inline)) SYS_RANDOM_PseudoSeedSet( uint32_t seed )
{
    srand(seed);
}

static __inline__ uint32_t __attribute__((always_inline)) SYS_RANDOM_PseudoGet( void )
{
    return (uint32_t)rand();
}

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_RANDOM_CryptoInitialize(void);

  Summary:
    Initializes the cryptographic Random Number Generator

  Description:
	This function performs the initialization of the
    Cryptographic-quality Random Number Generator.

  Precondition:
    None

  Parameters:
    None.

  Returns:
    A valid SYS_MODULE_OBJ value (!0) if initialization succeeded 
    0 if error.
    
  Remarks:
    The crypto random functions will call this function internally
    to make sure proper initialization occurred.
*/

SYS_MODULE_OBJ SYS_RANDOM_CryptoInitialize(void);

// *****************************************************************************
/* Function:
    void SYS_RANDOM_CryptoDeinitialize( SYS_MODULE_OBJ object );

  Summary:
    Deinitializes the cryptographic Random Number Generator

  Description:
	This function performs the deinitialization of the
    Cryptographic-quality Random Number Generator.

  Precondition:
    SYS_RANDOM_CryptoInitialize() has been called

  Parameters:
    object  - valid handle returned by SYS_RANDOM_CryptoInitialize.

  Returns:
    None
    
  Remarks:
    None
*/

void SYS_RANDOM_CryptoDeinitialize( SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
    uint32_t SYS_RANDOM_CryptoGet( void );

  Summary:
    Returns a random 32 bit value from the cryptographic quality Random Number 
	Generator. 

  Description:
	This function returns a 32-bit random number from the Cryptographic-
	quality Random Number Generator.

  Precondition:
    None

  Parameters:
    None.

  Returns:
    32-bit random number.
    0 if error.
    
  Remarks:
    None
*/

uint32_t SYS_RANDOM_CryptoGet( void );


// *****************************************************************************
/* Function:
    size_t SYS_RANDOM_CryptoBlockGet( uint8_t buffer, size_t bufferSize );

  Summary:
    Generates a sequence of random bytes using the cryptographic quality Random
	Number Generator. 

  Description:
	This function uses the Cryptographic-quality Random Number Generator to fill
	a block of data with random numbers.

  Precondition:
    None

  Parameters:
    buffer - Pointer to the memory location to fill with random data.
    size - The amount of random data, in bytes, to put in memory.

  Returns:
    size of the generated block.
    0 if error.
    
  Remarks:
    None
*/

size_t SYS_RANDOM_CryptoBlockGet( void *buffer, size_t size );

// *****************************************************************************
/* Function:
    uint8_t SYS_RANDOM_CryptoByteGet( void );

  Summary:
    Returns a random byte from the cryptographic quality Random Number Generator. 

  Description:
	This function returns a single byte from the Cryptographic-quality Random
	Number Generator.

  Precondition:
    None

  Parameters:
    None.

  Returns:
    Returns one byte of cryptographic-quality random data.
    0 if error.
    
  Remarks:
    None.
*/

uint8_t SYS_RANDOM_CryptoByteGet( void );




// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_RANDOM_ADPATER_H
