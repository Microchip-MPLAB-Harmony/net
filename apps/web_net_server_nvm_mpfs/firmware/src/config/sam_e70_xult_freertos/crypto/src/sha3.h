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


#ifndef WOLF_CRYPT_SHA3_H
#define WOLF_CRYPT_SHA3_H

#include "crypto/src/types.h"

#ifdef WOLFSSL_SHA3

#ifdef HAVE_FIPS
    /* for fips @wc_fips */
    #include "crypto/src/sha3.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef HAVE_FIPS /* avoid redefinition of structs */

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

/* in bytes */
enum {
    SHA3_224              = 10,   /* hash type unique */
    SHA3_224_DIGEST_SIZE  = 28,
    SHA3_224_COUNT        = 18,

    SHA3_256              = 11,   /* hash type unique */
    SHA3_256_DIGEST_SIZE  = 32,
    SHA3_256_COUNT        = 17,

    SHA3_384              = 12,   /* hash type unique */
    SHA3_384_DIGEST_SIZE  = 48,
    SHA3_384_COUNT        = 13,

    SHA3_512              = 13,   /* hash type unique */
    SHA3_512_DIGEST_SIZE  = 64,
    SHA3_512_COUNT        =  9
};


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
} Sha3;
#endif
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha3_224(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_224_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_224_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_224_Free(Sha3*);
WOLFSSL_API int wc_Sha3_224_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_224_Copy(Sha3* src, Sha3* dst);

WOLFSSL_API int wc_InitSha3_256(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_256_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_256_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_256_Free(Sha3*);
WOLFSSL_API int wc_Sha3_256_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_256_Copy(Sha3* src, Sha3* dst);

WOLFSSL_API int wc_InitSha3_384(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_384_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_384_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_384_Free(Sha3*);
WOLFSSL_API int wc_Sha3_384_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_384_Copy(Sha3* src, Sha3* dst);

WOLFSSL_API int wc_InitSha3_512(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_512_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_512_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_512_Free(Sha3*);
WOLFSSL_API int wc_Sha3_512_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_512_Copy(Sha3* src, Sha3* dst);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_SHA3 */
#endif /* WOLF_CRYPT_SHA3_H */

