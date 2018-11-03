/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    sha256.h

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




/* code submitted by raphael.huck@efixo.com */

#ifndef WOLF_CRYPT_SHA256_H
#define WOLF_CRYPT_SHA256_H

#include "crypto/src/types.h"
#include "crypto/src/settings.h"

#ifndef NO_SHA256

#ifdef HAVE_FIPS
    /* for fips @wc_fips */
    #include "crypto/src/sha256.h"
#endif

#if defined(WOLFSSL_SAME70_HASH)
#include "crypto/src/same70-hash.h"
#endif


#ifdef __cplusplus
    extern "C" {
#endif

#ifndef HAVE_FIPS /* avoid redefinition of structs */

#ifdef WOLFSSL_MICROCHIP_PIC32MZ
    #include "crypto/src/pic32mz-crypt.h"
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

/* in bytes */
enum {
    SHA256              =  2,   /* hash type unique */
    SHA256_BLOCK_SIZE   = 64,
    SHA256_DIGEST_SIZE  = 32,
    SHA256_PAD_SIZE     = 56
};

#ifndef WOLFSSL_TI_HASH

/* Sha256 digest */
typedef struct Sha256 {
#if defined(WOLFSSL_SAME70_HASH)
    struct icm_descriptor icm_descriptor __attribute__((aligned (64)));
    uint8_t  buffer[SHA256_BLOCK_SIZE] __attribute__((aligned (64)));  /* 64 bytes = 512 bits */
    uint32_t digest[SHA256_DIGEST_SIZE/4] __attribute__((aligned (128)));
    uint64_t total_len;   /* number of bytes to be processed  */
#else
    /* alignment on digest and buffer speeds up ARMv8 crypto operations */
    ALIGN16 word32  digest[SHA256_DIGEST_SIZE / sizeof(word32)];
    ALIGN16 word32  buffer[SHA256_BLOCK_SIZE  / sizeof(word32)];
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    void*   heap;
#ifdef WOLFSSL_PIC32MZ_HASH
    hashUpdCache cache; /* cache for updates */
#endif
#endif
} Sha256;

#else
    #include "wolfssl/wolfcrypt/port/ti/ti-hash.h"
#endif

#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha256(Sha256*);
WOLFSSL_API int wc_InitSha256_ex(Sha256*, void*, int);
WOLFSSL_API int wc_Sha256Update(Sha256*, const byte*, word32);
WOLFSSL_API int wc_Sha256Final(Sha256*, byte*);
WOLFSSL_API void wc_Sha256Free(Sha256*);

WOLFSSL_API int wc_Sha256GetHash(Sha256*, byte*);
WOLFSSL_API int wc_Sha256Copy(Sha256* src, Sha256* dst);

#ifdef WOLFSSL_PIC32MZ_HASH
WOLFSSL_API void wc_Sha256SizeSet(Sha256*, word32);
#endif

#ifdef WOLFSSL_SHA224

#ifndef HAVE_FIPS /* avoid redefinition of structs */
/* in bytes */
enum {
    SHA224              =   8,   /* hash type unique */
    SHA224_BLOCK_SIZE   =   SHA256_BLOCK_SIZE,
    SHA224_DIGEST_SIZE  =   28,
    SHA224_PAD_SIZE     =   SHA256_PAD_SIZE
};

typedef Sha256 Sha224;
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha224(Sha224*);
WOLFSSL_API int wc_InitSha224_ex(Sha224*, void*, int);
WOLFSSL_API int wc_Sha224Update(Sha224*, const byte*, word32);
WOLFSSL_API int wc_Sha224Final(Sha224*, byte*);
WOLFSSL_API void wc_Sha224Free(Sha224*);

WOLFSSL_API int wc_Sha224GetHash(Sha224*, byte*);
WOLFSSL_API int wc_Sha224Copy(Sha224* src, Sha224* dst);

#endif /* WOLFSSL_SHA224 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_SHA256 */
#endif /* WOLF_CRYPT_SHA256_H */

