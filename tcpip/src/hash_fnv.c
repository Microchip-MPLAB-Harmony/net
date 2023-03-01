/*******************************************************************************
  FNV Hash implementation file

  Summary:
    FNV Hash routines
    
  Description:
    This source file contains the functions that implement the FNV hash
*******************************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/









#include "tcpip/src/hash_fnv.h"


uint32_t fnv_32_hash(const void *key, size_t keyLen)
{
    size_t      ix;
    uint32_t    hval = FNV_32_INIT;
    
    const uint8_t* p = (const uint8_t*)key;

    for(ix = 0; ix < keyLen; ix++)
    {
#if defined(NO_FNV_GCC_OPTIMIZATION)
        hval *= FNV_32_PRIME;
#else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#endif
        hval ^= (uint32_t)*p++;
    }
    return hval;
}


uint32_t fnv_32a_hash(const void *key, size_t keyLen)
{
    size_t      ix;
    uint32_t    hval = FNV_32_INIT;

    const uint8_t* p = (const uint8_t*)key;

    for(ix = 0; ix < keyLen; ix++)
    {
        hval ^= (uint32_t)*p++;

#if defined(NO_FNV_GCC_OPTIMIZATION)
        hval *= FNV_32_PRIME;
#else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#endif
    }

    return hval;
}


