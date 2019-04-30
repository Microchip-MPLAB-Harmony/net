/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    sha512.h

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

#ifndef WOLF_CRYPT_SHA512_H
#define WOLF_CRYPT_SHA512_H

#include "crypto/src/types.h"

#if defined(WOLFSSL_SHA512) || defined(WOLFSSL_SHA384)

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include "crypto/src/fips.h"
#endif /* HAVE_FIPS_VERSION >= 2 */

#if defined(HAVE_FIPS) && \
	(!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))
    #ifdef WOLFSSL_SHA512
        #define wc_Sha512             Sha512
        #define WC_SHA512             SHA512
        #define WC_SHA512_BLOCK_SIZE  SHA512_BLOCK_SIZE
        #define WC_SHA512_DIGEST_SIZE SHA512_DIGEST_SIZE
        #define WC_SHA512_PAD_SIZE    SHA512_PAD_SIZE
    #endif /* WOLFSSL_SHA512 */
    #ifdef WOLFSSL_SHA384
        #define wc_Sha384             Sha384
        #define WC_SHA384             SHA384
        #define WC_SHA384_BLOCK_SIZE  SHA384_BLOCK_SIZE
        #define WC_SHA384_DIGEST_SIZE SHA384_DIGEST_SIZE
        #define WC_SHA384_PAD_SIZE    SHA384_PAD_SIZE
    #endif /* WOLFSSL_SHA384 */

    #define CYASSL_SHA512
    #if defined(WOLFSSL_SHA384)
        #define CYASSL_SHA384
    #endif
	/* for fips @wc_fips */
    #include <cyassl/ctaocrypt/sha512.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

#if defined(_MSC_VER)
    #define SHA512_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
    #define SHA512_NOINLINE __attribute__((noinline))
#else
    #define SHA512_NOINLINE
#endif

#ifdef WOLFSSL_SHA512

#if !defined(NO_OLD_SHA_NAMES)
    #define SHA512             WC_SHA512
#endif

#if !defined(NO_OLD_WC_NAMES)
    #define Sha512             wc_Sha512
    #define SHA512_BLOCK_SIZE  WC_SHA512_BLOCK_SIZE
    #define SHA512_DIGEST_SIZE WC_SHA512_DIGEST_SIZE
    #define SHA512_PAD_SIZE    WC_SHA512_PAD_SIZE
#endif

#endif /* WOLFSSL_SHA512 */

/* in bytes */
enum {
#ifdef WOLFSSL_SHA512
    WC_SHA512              =   WC_HASH_TYPE_SHA512,
#endif
    WC_SHA512_BLOCK_SIZE   = 128,
    WC_SHA512_DIGEST_SIZE  =  64,
    WC_SHA512_PAD_SIZE     = 112
};


#ifdef WOLFSSL_IMX6_CAAM
    #include "wolfssl/wolfcrypt/port/caam/wolfcaam_sha.h"
#elif defined(HAVE_MICROCHIP_HARMONY3_HW_SHA512)
    #include "crypto/src/crypt_sha512_hw.h"
    #define wc_Sha512 crypt_sha512_hw_descriptor
#else
/* wc_Sha512 digest */
typedef struct wc_Sha512 {
    word64  digest[WC_SHA512_DIGEST_SIZE / sizeof(word64)];
    word64  buffer[WC_SHA512_BLOCK_SIZE  / sizeof(word64)];
    word32  buffLen;   /* in bytes          */
    word64  loLen;     /* length in bytes   */
    word64  hiLen;     /* length in bytes   */
    void*   heap;
#ifdef USE_INTEL_SPEEDUP
    const byte* data;
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_SMALL_STACK_CACHE
    word64* W;
#endif
} wc_Sha512;
#endif

#endif /* HAVE_FIPS */

#ifdef WOLFSSL_SHA512

WOLFSSL_API int wc_InitSha512(wc_Sha512*);
WOLFSSL_API int wc_InitSha512_ex(wc_Sha512*, void*, int);
WOLFSSL_API int wc_Sha512Update(wc_Sha512*, const byte*, word32);
WOLFSSL_API int wc_Sha512FinalRaw(wc_Sha512*, byte*);
WOLFSSL_API int wc_Sha512Final(wc_Sha512*, byte*);
WOLFSSL_API void wc_Sha512Free(wc_Sha512*);

WOLFSSL_API int wc_Sha512GetHash(wc_Sha512*, byte*);
WOLFSSL_API int wc_Sha512Copy(wc_Sha512* src, wc_Sha512* dst);

#endif /* WOLFSSL_SHA512 */

#if defined(WOLFSSL_SHA384)

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#if !defined(NO_OLD_SHA_NAMES)
    #define SHA384             WC_SHA384
#endif

#if !defined(NO_OLD_WC_NAMES)
    #define Sha384             wc_Sha384
    #define SHA384_BLOCK_SIZE  WC_SHA384_BLOCK_SIZE
    #define SHA384_DIGEST_SIZE WC_SHA384_DIGEST_SIZE
    #define SHA384_PAD_SIZE    WC_SHA384_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_SHA384              =   WC_HASH_TYPE_SHA384,
    WC_SHA384_BLOCK_SIZE   =   WC_SHA512_BLOCK_SIZE,
    WC_SHA384_DIGEST_SIZE  =   48,
    WC_SHA384_PAD_SIZE     =   WC_SHA512_PAD_SIZE
};
#if defined(HAVE_MICROCHIP_HARMONY3_HW_SHA384)
    #include "crypto/src/crypt_sha384_hw.h"
    #define wc_Sha384 crypt_sha384_hw_descriptor
#else

typedef wc_Sha512 wc_Sha384;
#endif
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha384(wc_Sha384*);
WOLFSSL_API int wc_InitSha384_ex(wc_Sha384*, void*, int);
WOLFSSL_API int wc_Sha384Update(wc_Sha384*, const byte*, word32);
WOLFSSL_API int wc_Sha384FinalRaw(wc_Sha384*, byte*);
WOLFSSL_API int wc_Sha384Final(wc_Sha384*, byte*);
WOLFSSL_API void wc_Sha384Free(wc_Sha384*);

WOLFSSL_API int wc_Sha384GetHash(wc_Sha384*, byte*);
WOLFSSL_API int wc_Sha384Copy(wc_Sha384* src, wc_Sha384* dst);

#endif /* WOLFSSL_SHA384 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_SHA512 || WOLFSSL_SHA384 */
#endif /* WOLF_CRYPT_SHA512_H */

