/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    sha3.h
  
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


#ifndef WOLF_CRYPT_SHA3_H
#define WOLF_CRYPT_SHA3_H

#include "crypto/src/types.h"

#ifdef WOLFSSL_SHA3

#ifdef HAVE_FIPS
    /* for fips @wc_fips */
    #include "crypto/src/fips.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

/* in bytes */
enum {
    WC_SHA3_224              = WC_HASH_TYPE_SHA3_224,
    WC_SHA3_224_DIGEST_SIZE  = 28,
    WC_SHA3_224_COUNT        = 18,

    WC_SHA3_256              = WC_HASH_TYPE_SHA3_256,
    WC_SHA3_256_DIGEST_SIZE  = 32,
    WC_SHA3_256_COUNT        = 17,

    WC_SHA3_384              = WC_HASH_TYPE_SHA3_384,
    WC_SHA3_384_DIGEST_SIZE  = 48,
    WC_SHA3_384_COUNT        = 13,

    WC_SHA3_512              = WC_HASH_TYPE_SHA3_512,
    WC_SHA3_512_DIGEST_SIZE  = 64,
    WC_SHA3_512_COUNT        =  9,

#ifndef HAVE_SELFTEST
    /* These values are used for HMAC, not SHA-3 directly.
     * They come from from FIPS PUB 202. */
    WC_SHA3_224_BLOCK_SIZE = 144,
    WC_SHA3_256_BLOCK_SIZE = 136,
    WC_SHA3_384_BLOCK_SIZE = 104,
    WC_SHA3_512_BLOCK_SIZE = 72,
#endif
};

#ifndef NO_OLD_WC_NAMES
    #define SHA3_224             WC_SHA3_224
    #define SHA3_224_DIGEST_SIZE WC_SHA3_224_DIGEST_SIZE
    #define SHA3_256             WC_SHA3_256
    #define SHA3_256_DIGEST_SIZE WC_SHA3_256_DIGEST_SIZE
    #define SHA3_384             WC_SHA3_384
    #define SHA3_384_DIGEST_SIZE WC_SHA3_384_DIGEST_SIZE
    #define SHA3_512             WC_SHA3_512
    #define SHA3_512_DIGEST_SIZE WC_SHA3_512_DIGEST_SIZE
    #define Sha3 wc_Sha3
#endif


#ifdef WOLFSSL_XILINX_CRYPT
    #include "wolfssl/wolfcrypt/port/xilinx/xil-sha3.h"
#else
/* Sha3 digest */
typedef struct Sha3 {
    /* State data that is processed for each block. */
    word64 s[25];
    /* Unprocessed message data. */
    byte   t[200];
    /* Index into unprocessed data to place next message byte. */
    byte   i;

    void*  heap;

#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
} wc_Sha3;
#endif


WOLFSSL_API int wc_InitSha3_224(wc_Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_224_Update(wc_Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_224_Final(wc_Sha3*, byte*);
WOLFSSL_API void wc_Sha3_224_Free(wc_Sha3*);
WOLFSSL_API int wc_Sha3_224_GetHash(wc_Sha3*, byte*);
WOLFSSL_API int wc_Sha3_224_Copy(wc_Sha3* src, wc_Sha3* dst);

WOLFSSL_API int wc_InitSha3_256(wc_Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_256_Update(wc_Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_256_Final(wc_Sha3*, byte*);
WOLFSSL_API void wc_Sha3_256_Free(wc_Sha3*);
WOLFSSL_API int wc_Sha3_256_GetHash(wc_Sha3*, byte*);
WOLFSSL_API int wc_Sha3_256_Copy(wc_Sha3* src, wc_Sha3* dst);

WOLFSSL_API int wc_InitSha3_384(wc_Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_384_Update(wc_Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_384_Final(wc_Sha3*, byte*);
WOLFSSL_API void wc_Sha3_384_Free(wc_Sha3*);
WOLFSSL_API int wc_Sha3_384_GetHash(wc_Sha3*, byte*);
WOLFSSL_API int wc_Sha3_384_Copy(wc_Sha3* src, wc_Sha3* dst);

WOLFSSL_API int wc_InitSha3_512(wc_Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_512_Update(wc_Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_512_Final(wc_Sha3*, byte*);
WOLFSSL_API void wc_Sha3_512_Free(wc_Sha3*);
WOLFSSL_API int wc_Sha3_512_GetHash(wc_Sha3*, byte*);
WOLFSSL_API int wc_Sha3_512_Copy(wc_Sha3* src, wc_Sha3* dst);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_SHA3 */
#endif /* WOLF_CRYPT_SHA3_H */

