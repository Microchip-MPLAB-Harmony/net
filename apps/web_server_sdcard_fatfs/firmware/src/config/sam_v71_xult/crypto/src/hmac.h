/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    hmac.h
  
  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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

//DOM-IGNORE-END

#ifndef NO_HMAC

#ifndef WOLF_CRYPT_HMAC_H
#define WOLF_CRYPT_HMAC_H

#include "crypto/src/types.h"

#ifndef NO_MD5
    #include "crypto/src/md5.h"
#endif

#ifndef NO_SHA
#include "crypto/src/sha.h"
#endif

#if !defined(NO_SHA256) || defined(WOLFSSL_SHA224)
    #include "crypto/src/sha256.h"
#endif

#ifdef WOLFSSL_SHA512
    #include "crypto/src/sha512.h"
#endif

#ifdef HAVE_BLAKE2
    #include "crypto/src/blake2.h"
#endif

#ifdef HAVE_FIPS
/* for fips */
    #include "crypto/src/hmac.h"
#endif


#ifdef __cplusplus
    extern "C" {
#endif
#ifndef HAVE_FIPS

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

enum {
    HMAC_FIPS_MIN_KEY = 14,   /* 112 bit key length minimum */

    IPAD    = 0x36,
    OPAD    = 0x5C,

/* If any hash is not enabled, add the ID here. */
#ifdef NO_MD5
    MD5     = 0,
#endif
#ifdef NO_SHA
    SHA     = 1,
#endif
#ifdef NO_SHA256
    SHA256  = 2,
#endif
#ifndef WOLFSSL_SHA512
    SHA512  = 4,
#endif
#ifndef WOLFSSL_SHA384
    SHA384  = 5,
#endif
#ifndef HAVE_BLAKE2
    BLAKE2B_ID = 7,
#endif
#ifndef WOLFSSL_SHA224
    SHA224  = 8,
#endif

/* Select the largest available hash for the buffer size. */
#if defined(WOLFSSL_SHA512)
    MAX_DIGEST_SIZE = SHA512_DIGEST_SIZE,
    HMAC_BLOCK_SIZE = SHA512_BLOCK_SIZE,
#elif defined(HAVE_BLAKE2)
    MAX_DIGEST_SIZE = BLAKE2B_OUTBYTES,
    HMAC_BLOCK_SIZE = BLAKE2B_BLOCKBYTES,
#elif defined(WOLFSSL_SHA384)
    MAX_DIGEST_SIZE = SHA384_DIGEST_SIZE,
    HMAC_BLOCK_SIZE = SHA384_BLOCK_SIZE
#elif !defined(NO_SHA256)
    MAX_DIGEST_SIZE = SHA256_DIGEST_SIZE,
    HMAC_BLOCK_SIZE = SHA256_BLOCK_SIZE
#elif defined(WOLFSSL_SHA224)
    MAX_DIGEST_SIZE = SHA224_DIGEST_SIZE,
    HMAC_BLOCK_SIZE = SHA224_BLOCK_SIZE
#elif !defined(NO_SHA)
    MAX_DIGEST_SIZE = SHA_DIGEST_SIZE,
    HMAC_BLOCK_SIZE = SHA_BLOCK_SIZE,
#elif !defined(NO_MD5)
    MAX_DIGEST_SIZE = MD5_DIGEST_SIZE,
    HMAC_BLOCK_SIZE = MD5_BLOCK_SIZE,
#else
    #error "You have to have some kind of hash if you want to use HMAC."
#endif
};


/* hash union */
typedef union {
#ifndef NO_MD5
    Md5 md5;
#endif
#ifndef NO_SHA
    Sha sha;
#endif
#ifdef WOLFSSL_SHA224
    Sha224 sha224;
#endif
#ifndef NO_SHA256
    Sha256 sha256;
#endif
#ifdef WOLFSSL_SHA512
#ifdef WOLFSSL_SHA384
    Sha384 sha384;
#endif
    Sha512 sha512;
#endif
#ifdef HAVE_BLAKE2
    Blake2b blake2b;
#endif
} Hash;

/* Hmac digest */
typedef struct Hmac {
    Hash    hash;
    word32  ipad[HMAC_BLOCK_SIZE  / sizeof(word32)];  /* same block size all*/
    word32  opad[HMAC_BLOCK_SIZE  / sizeof(word32)];
    word32  innerHash[MAX_DIGEST_SIZE / sizeof(word32)];
    void*   heap;                 /* heap hint */
    byte    macType;              /* md5 sha or sha256 */
    byte    innerHashKeyed;       /* keyed flag */

#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
    word16       keyLen;          /* hmac key length (key in ipad) */
    #ifdef HAVE_CAVIUM
        byte*    data;            /* buffered input data for one call */
        word16   dataLen;
    #endif /* HAVE_CAVIUM */
#endif /* WOLFSSL_ASYNC_CRYPT */
} Hmac;

#endif /* HAVE_FIPS */

/* does init */
WOLFSSL_API int wc_HmacSetKey(Hmac*, int type, const byte* key, word32 keySz);
WOLFSSL_API int wc_HmacUpdate(Hmac*, const byte*, word32);
WOLFSSL_API int wc_HmacFinal(Hmac*, byte*);
WOLFSSL_API int wc_HmacSizeByType(int type);

WOLFSSL_API int wc_HmacInit(Hmac* hmac, void* heap, int devId);
WOLFSSL_API void wc_HmacFree(Hmac*);

WOLFSSL_API int wolfSSL_GetHmacMaxSize(void);

#ifdef HAVE_HKDF

WOLFSSL_API int wc_HKDF_Extract(int type, const byte* salt, word32 saltSz,
                                const byte* inKey, word32 inKeySz, byte* out);
WOLFSSL_API int wc_HKDF_Expand(int type, const byte* inKey, word32 inKeySz,
                               const byte* info, word32 infoSz,
                               byte* out,        word32 outSz);

WOLFSSL_API int wc_HKDF(int type, const byte* inKey, word32 inKeySz,
                    const byte* salt, word32 saltSz,
                    const byte* info, word32 infoSz,
                    byte* out, word32 outSz);

#endif /* HAVE_HKDF */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_HMAC_H */

#endif /* NO_HMAC */

