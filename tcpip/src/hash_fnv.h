/*******************************************************************************
  FNV Hash API file

  Company:
    Microchip Technology Inc.
    
  File Name:
    hash_fnv.h

  Summary:
    FNV Hash routines API
    
  Description:
    This source file contains the API functions and definitions 
    for the FNV hash.
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef _HASH_FNV_H_
#define _HASH_FNV_H_

#include <stdlib.h>
#include <stdint.h>

//#define NO_FNV_GCC_OPTIMIZATION

#define FNV_32_PRIME    ((uint32_t)16777619ul)
#define FNV_32_INIT     ((uint32_t)2166136261ul)


// API

uint32_t fnv_32_hash(const void *key, size_t keyLen);


uint32_t fnv_32a_hash(const void *key, size_t keyLen);



#endif  // _HASH_FNV_H_

