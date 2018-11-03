/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    sha.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright © 2013-2017 released Microchip Technology Inc.  All rights reserved.

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
*******************************************************************************/
//DOM-IGNORE-END

#ifndef WOLF_CRYPT_SHA_H
#define WOLF_CRYPT_SHA_H

#include "configuration.h"
#include "crypto/src/types.h"

#ifndef NO_SHA

#ifdef HAVE_FIPS
/* for fips @wc_fips */
#include "crypto/src/sha.h"
#endif

#if defined(WOLFSSL_SAME70_HASH)
#include "crypto/src/same70-hash.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef HAVE_FIPS /* avoid redefining structs */

#ifdef WOLFSSL_MICROCHIP_PIC32MZ
    #include "crypto/src/pic32mz-crypt.h"
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

/* in bytes */
enum {
#if defined(STM32F2_HASH) || defined(STM32F4_HASH)
    SHA_REG_SIZE     =  4,    /* STM32 register size, bytes */
#endif
    SHA              =  1,    /* hash type unique */
    SHA_BLOCK_SIZE   = 64,
    SHA_DIGEST_SIZE  = 20,
    SHA_PAD_SIZE     = 56
};


#ifndef WOLFSSL_TI_HASH
/* Sha digest */
typedef struct Sha {
#if defined(WOLFSSL_SAME70_HASH)
    struct icm_descriptor icm_descriptor __attribute__((aligned (64)));
    uint8_t  buffer[SHA_BLOCK_SIZE]   __attribute__((aligned (64)));
    uint32_t digest[SHA_DIGEST_SIZE/4] __attribute__((aligned (128)));
    uint64_t total_len;   /* number of bytes to be processed  */
#else
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    word32  buffer[SHA_BLOCK_SIZE  / sizeof(word32)];
    word32  digest[SHA_DIGEST_SIZE / sizeof(word32)];
    void*   heap;
    #ifdef WOLFSSL_PIC32MZ_HASH
        hashUpdCache cache; /* cache for updates */
    #endif
#endif
} Sha;

#else
    #include "wolfssl/wolfcrypt/port/ti/ti-hash.h"
#endif /* WOLFSSL_TI_HASH */


#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha(Sha*);
WOLFSSL_API int wc_InitSha_ex(Sha* sha, void* heap, int devId);
WOLFSSL_API int wc_ShaUpdate(Sha*, const byte*, word32);
WOLFSSL_API int wc_ShaFinal(Sha*, byte*);
WOLFSSL_API void wc_ShaFree(Sha*);

WOLFSSL_API int wc_ShaGetHash(Sha*, byte*);
WOLFSSL_API int wc_ShaCopy(Sha*, Sha*);

#ifdef WOLFSSL_PIC32MZ_HASH
WOLFSSL_API void wc_ShaSizeSet(Sha* sha, word32 len);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_SHA */
#endif /* WOLF_CRYPT_SHA_H */

