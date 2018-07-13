/*******************************************************************************
  FNV Hash implementation file

  Summary:
    FNV Hash routines
    
  Description:
    This source file contains the functions that implement the FNV hash
*******************************************************************************/

/*******************************************************************************
File Name:  hash_fnv.c
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 

THIRD PARTY SOFTWARE:  Notwithstanding anything to the contrary, any third party software
accompanying this software is subject to the terms and conditions of the third party’s
license agreement or terms. To the extent required by third party licenses covering such
software, the terms of the third party license will apply in lieu of the terms provided
in this notice. To the extent the terms of such third party licenses prohibit any of the
restrictions described herein, such restrictions will not apply to such third party software. 
 
MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.


FNV Code terms of use:  

 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *      chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
*******************************************************************************/

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


