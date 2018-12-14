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

#ifndef WOLF_CRYPT_SHA512_H
#define WOLF_CRYPT_SHA512_H

#include "crypto/src/types.h"

#ifdef WOLFSSL_SHA512

/* for fips @wc_fips */
#ifdef HAVE_FIPS
    #define CYASSL_SHA512
    #if defined(WOLFSSL_SHA384)
        #define CYASSL_SHA384
    #endif
    #include "crypto/src/sha512.h"
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
    SHA512              =   4,   /* hash type unique */
    SHA512_BLOCK_SIZE   = 128,
    SHA512_DIGEST_SIZE  =  64,
    SHA512_PAD_SIZE     = 112
};


/* Sha512 digest */
typedef struct Sha512 {
    word32  buffLen;   /* in bytes          */
    word64  loLen;     /* length in bytes   */
    word64  hiLen;     /* length in bytes   */
    word64  digest[SHA512_DIGEST_SIZE / sizeof(word64)];
    word64  buffer[SHA512_BLOCK_SIZE  / sizeof(word64)];
    void*   heap;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
} Sha512;

#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha512(Sha512*);
WOLFSSL_API int wc_InitSha512_ex(Sha512*, void*, int);
WOLFSSL_API int wc_Sha512Update(Sha512*, const byte*, word32);
WOLFSSL_API int wc_Sha512Final(Sha512*, byte*);
WOLFSSL_API void wc_Sha512Free(Sha512*);

WOLFSSL_API int wc_Sha512GetHash(Sha512*, byte*);
WOLFSSL_API int wc_Sha512Copy(Sha512* src, Sha512* dst);

#if defined(WOLFSSL_SHA384)

#ifndef HAVE_FIPS /* avoid redefinition of structs */
/* in bytes */
enum {
    SHA384              =   5,   /* hash type unique */
    SHA384_BLOCK_SIZE   =   SHA512_BLOCK_SIZE,
    SHA384_DIGEST_SIZE  =   48,
    SHA384_PAD_SIZE     =   SHA512_PAD_SIZE
};

typedef Sha512 Sha384;
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha384(Sha384*);
WOLFSSL_API int wc_InitSha384_ex(Sha384*, void*, int);
WOLFSSL_API int wc_Sha384Update(Sha384*, const byte*, word32);
WOLFSSL_API int wc_Sha384Final(Sha384*, byte*);
WOLFSSL_API void wc_Sha384Free(Sha384*);

WOLFSSL_API int wc_Sha384GetHash(Sha384*, byte*);
WOLFSSL_API int wc_Sha384Copy(Sha384* src, Sha384* dst);

#endif /* WOLFSSL_SHA384 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_SHA512 */
#endif /* WOLF_CRYPT_SHA512_H */

