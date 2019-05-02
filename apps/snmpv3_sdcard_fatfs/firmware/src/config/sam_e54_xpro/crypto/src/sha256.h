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
**************************************************************************/
//DOM-IGNORE-END



/* code submitted by raphael.huck@efixo.com */

#ifndef WOLF_CRYPT_SHA256_H
#define WOLF_CRYPT_SHA256_H

#include "crypto/src/types.h"
#include "crypto/src/settings.h"

#ifndef NO_SHA256

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include <wolfssl/wolfcrypt/fips.h>
#endif /* HAVE_FIPS_VERSION >= 2 */

#if defined(HAVE_FIPS) && \
	(!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))
    #define wc_Sha256             Sha256
    #define WC_SHA256             SHA256
    #define WC_SHA256_BLOCK_SIZE  SHA256_BLOCK_SIZE
    #define WC_SHA256_DIGEST_SIZE SHA256_DIGEST_SIZE
    #define WC_SHA256_PAD_SIZE    SHA256_PAD_SIZE

    #ifdef WOLFSSL_SHA224
        #define wc_Sha224             Sha224
        #define WC_SHA224             SHA224
        #define WC_SHA224_BLOCK_SIZE  SHA224_BLOCK_SIZE
        #define WC_SHA224_DIGEST_SIZE SHA224_DIGEST_SIZE
        #define WC_SHA224_PAD_SIZE    SHA224_PAD_SIZE
    #endif

    /* for fips @wc_fips */
    #include "crypto/src/sha256.h"
#endif

#ifdef FREESCALE_LTC_SHA
    #include "fsl_ltc.h"
#endif

#if defined(WOLFSSL_SAME70_HASH)
#include "crypto/src/same70-hash.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_MICROCHIP_PIC32MZ
    #include "crypto/src/pic32mz-crypt.h"
#endif
#ifdef STM32_HASH
    #include <wolfssl/wolfcrypt/port/st/stm32.h>
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif
#if defined(WOLFSSL_DEVCRYPTO) && defined(WOLFSSL_DEVCRYPTO_HASH)
    #include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>
#endif
#if defined(WOLFSSL_ESP32WROOM32_CRYPT)
    #include "wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h"
#endif

#if defined(_MSC_VER)
    #define SHA256_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
    #define SHA256_NOINLINE __attribute__((noinline))
#else
    #define SHA256_NOINLINE
#endif

#if !defined(NO_OLD_SHA_NAMES)
    #define SHA256             WC_SHA256
#endif

#ifndef NO_OLD_WC_NAMES
    #define Sha256             wc_Sha256
    #define SHA256_BLOCK_SIZE  WC_SHA256_BLOCK_SIZE
    #define SHA256_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
    #define SHA256_PAD_SIZE    WC_SHA256_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_SHA256              =  WC_HASH_TYPE_SHA256,
    WC_SHA256_BLOCK_SIZE   = 64,
    WC_SHA256_DIGEST_SIZE  = 32,
    WC_SHA256_PAD_SIZE     = 56
};


#ifdef WOLFSSL_TI_HASH
    #include "wolfssl/wolfcrypt/port/ti/ti-hash.h"
#elif defined(WOLFSSL_IMX6_CAAM)
    #include "wolfssl/wolfcrypt/port/caam/wolfcaam_sha.h"
#elif defined(WOLFSSL_AFALG_HASH)
    #include "wolfssl/wolfcrypt/port/af_alg/afalg_hash.h"
#elif (defined(HAVE_MICROCHIP_HARMONY3_HW_SHA256) || defined(HAVE_MICROCHIP_HARMONY3_HW_SHA224))  && !defined(WOLFSSL_PIC32MZ_HASH)
    #include "crypto/src/crypt_sha256_hw.h"
    #define wc_Sha256 crypt_sha256_hw_descriptor
#else
/* wc_Sha256 digest */
typedef struct wc_Sha256 {
#ifdef FREESCALE_LTC_SHA
    ltc_hash_ctx_t ctx;
#elif defined(STM32_HASH)
    STM32_HASH_Context stmCtx;
#elif defined(WOLFSSL_SAME70_HASH)
    struct icm_descriptor icm_descriptor __attribute__((aligned (64)));
    uint8_t  buffer[SHA256_BLOCK_SIZE] __attribute__((aligned (64)));  /* 64 bytes = 512 bits */
    uint32_t digest[SHA256_DIGEST_SIZE/4] __attribute__((aligned (128)));
    uint64_t total_len;   /* number of bytes to be processed  */
#else
    /* alignment on digest and buffer speeds up ARMv8 crypto operations */
    ALIGN16 word32  digest[WC_SHA256_DIGEST_SIZE / sizeof(word32)];
    ALIGN16 word32  buffer[WC_SHA256_BLOCK_SIZE  / sizeof(word32)];
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    void*   heap;
#ifdef USE_INTEL_SPEEDUP
    const byte* data;
#endif
#ifdef WOLFSSL_PIC32MZ_HASH
    hashUpdCache cache; /* cache for updates */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_SMALL_STACK_CACHE
    word32* W;
#endif
#ifdef WOLFSSL_DEVCRYPTO_HASH
    WC_CRYPTODEV ctx;
    byte*  msg;
    word32 used;
    word32 len;
#endif
#if defined(WOLFSSL_ESP32WROOM32_CRYPT) && \
   !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH)
    WC_ESP32SHA ctx;
#endif
#endif
} wc_Sha256;

#endif

#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha256(wc_Sha256*);
WOLFSSL_API int wc_InitHmacSha256(wc_Sha256*);
WOLFSSL_API int wc_InitSha256_ex(wc_Sha256*, void*, int);
WOLFSSL_API int wc_Sha256Update(wc_Sha256*, const byte*, word32);
WOLFSSL_API int wc_Sha256FinalRaw(wc_Sha256*, byte*);
WOLFSSL_API int wc_Sha256Final(wc_Sha256*, byte*);
WOLFSSL_API void wc_Sha256Free(wc_Sha256*);

WOLFSSL_API int wc_Sha256GetHash(wc_Sha256*, byte*);
WOLFSSL_API int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst);

#ifdef WOLFSSL_PIC32MZ_HASH
WOLFSSL_API void wc_Sha256SizeSet(wc_Sha256*, word32);
#endif

#ifdef WOLFSSL_SHA224
/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifndef NO_OLD_WC_NAMES
    #define Sha224             wc_Sha224
    #define SHA224             WC_SHA224
    #define SHA224_BLOCK_SIZE  WC_SHA224_BLOCK_SIZE
    #define SHA224_DIGEST_SIZE WC_SHA224_DIGEST_SIZE
    #define SHA224_PAD_SIZE    WC_SHA224_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_SHA224              =   WC_HASH_TYPE_SHA224,
    WC_SHA224_BLOCK_SIZE   =   WC_SHA256_BLOCK_SIZE,
    WC_SHA224_DIGEST_SIZE  =   28,
    WC_SHA224_PAD_SIZE     =   WC_SHA256_PAD_SIZE
};
#if defined(HAVE_MICROCHIP_HARMONY3_HW_SHA224)  && !defined(WOLFSSL_PIC32MZ_HASH)
    #include "crypto/src/crypt_sha224_hw.h"
    #define wc_Sha224 crypt_sha224_hw_descriptor
#else
typedef wc_Sha256 wc_Sha224;
#endif

#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha224(wc_Sha224*);
WOLFSSL_API int wc_InitHmacSha224(wc_Sha224*);
WOLFSSL_API int wc_InitSha224_ex(wc_Sha224*, void*, int);
WOLFSSL_API int wc_Sha224Update(wc_Sha224*, const byte*, word32);
WOLFSSL_API int wc_Sha224Final(wc_Sha224*, byte*);
WOLFSSL_API void wc_Sha224Free(wc_Sha224*);

WOLFSSL_API int wc_Sha224GetHash(wc_Sha224*, byte*);
WOLFSSL_API int wc_Sha224Copy(wc_Sha224* src, wc_Sha224* dst);

#endif /* WOLFSSL_SHA224 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_SHA256 */
#endif /* WOLF_CRYPT_SHA256_H */

