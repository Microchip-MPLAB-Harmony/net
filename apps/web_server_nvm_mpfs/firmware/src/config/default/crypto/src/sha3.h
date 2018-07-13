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

