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
 Copyright (C) 2013-2019 Microchip Technology Inc. and its subsidiaries.

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

#include "crypto/src/hash.h"

#if defined(HAVE_FIPS) && \
	(!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))
/* for fips @wc_fips */
    #include "crypto/src/hmac.h"
    #define WC_HMAC_BLOCK_SIZE HMAC_BLOCK_SIZE
#endif


#if defined(HAVE_FIPS) && \
	defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
	#include "crypto/src/fips.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

#ifndef NO_OLD_WC_NAMES
    #define HMAC_BLOCK_SIZE WC_HMAC_BLOCK_SIZE
#endif

enum {
    HMAC_FIPS_MIN_KEY = 14,   /* 112 bit key length minimum */

    IPAD    = 0x36,
    OPAD    = 0x5C,

/* If any hash is not enabled, add the ID here. */
#ifdef NO_MD5
    WC_MD5     = WC_HASH_TYPE_MD5,
#endif
#ifdef NO_SHA
    WC_SHA     = WC_HASH_TYPE_SHA,
#endif
#ifdef NO_SHA256
    WC_SHA256  = WC_HASH_TYPE_SHA256,
#endif
#ifndef WOLFSSL_SHA512
    WC_SHA512  = WC_HASH_TYPE_SHA512,
#endif
#ifndef WOLFSSL_SHA384
    WC_SHA384  = WC_HASH_TYPE_SHA384,
#endif
#ifndef HAVE_BLAKE2
    BLAKE2B_ID = WC_HASH_TYPE_BLAKE2B,
#endif
#ifndef WOLFSSL_SHA224
    WC_SHA224  = WC_HASH_TYPE_SHA224,
#endif
#ifndef WOLFSSL_SHA3
    WC_SHA3_224 = WC_HASH_TYPE_SHA3_224,
    WC_SHA3_256 = WC_HASH_TYPE_SHA3_256,
    WC_SHA3_384 = WC_HASH_TYPE_SHA3_384,
    WC_SHA3_512 = WC_HASH_TYPE_SHA3_512,
#endif
};

/* Select the largest available hash for the buffer size. */
#define WC_HMAC_BLOCK_SIZE WC_MAX_BLOCK_SIZE

#if !defined(WOLFSSL_SHA3) && !defined(WOLFSSL_SHA512) && !defined(HAVE_BLAKE2) && \
    !defined(WOLFSSL_SHA384) && defined(NO_SHA256) && defined(WOLFSSL_SHA224) && \
     defined(NO_SHA) && defined(NO_MD5)
    #error "You have to have some kind of hash if you want to use HMAC."
#endif


/* hash union */
typedef union {
#ifndef NO_MD5
    wc_Md5 md5;
#endif
#ifndef NO_SHA
    wc_Sha sha;
#endif
#ifdef WOLFSSL_SHA224
    wc_Sha224 sha224;
#endif
#ifndef NO_SHA256
    wc_Sha256 sha256;
#endif
#ifdef WOLFSSL_SHA384
    wc_Sha384 sha384;
#endif
#ifdef WOLFSSL_SHA512
    wc_Sha512 sha512;
#endif
#ifdef HAVE_BLAKE2
    Blake2b blake2b;
#endif
#ifdef WOLFSSL_SHA3
    wc_Sha3 sha3;
#endif
} Hash;

/* Hmac digest */
typedef struct Hmac {
    Hash    hash;
    word32  ipad[WC_HMAC_BLOCK_SIZE  / sizeof(word32)];  /* same block size all*/
    word32  opad[WC_HMAC_BLOCK_SIZE  / sizeof(word32)];
    word32  innerHash[WC_MAX_DIGEST_SIZE / sizeof(word32)];
    void*   heap;                 /* heap hint */
    byte    macType;              /* md5 sha or sha256 */
    byte    innerHashKeyed;       /* keyed flag */

#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
    word16       keyLen;          /* hmac key length (key in ipad) */
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

WOLFSSL_LOCAL int _InitHmac(Hmac* hmac, int type, void* heap);

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

