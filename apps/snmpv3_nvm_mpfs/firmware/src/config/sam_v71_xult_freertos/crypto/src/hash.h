/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    hash.h

  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic 
    Framework Library for PIC32 families of Microchip microcontrollers.
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

#ifndef WOLF_CRYPT_HASH_H
#define WOLF_CRYPT_HASH_H

#include "crypto/src/types.h"
#include "configuration.h"

#ifndef NO_MD5
    #include "crypto/src/md5.h"
#endif
#ifndef NO_SHA
    #include "crypto/src/sha.h"
#endif
#if defined(WOLFSSL_SHA224) || !defined(NO_SHA256)
    #include "crypto/src/sha256.h"
#endif
#if defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512)
    #include "crypto/src/sha512.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* Hash types */
enum wc_HashType {
    WC_HASH_TYPE_NONE = 0,
    WC_HASH_TYPE_MD2 = 1,
    WC_HASH_TYPE_MD4 = 2,
    WC_HASH_TYPE_MD5 = 3,
    WC_HASH_TYPE_SHA = 4, /* SHA-1 (not old SHA-0) */
    WC_HASH_TYPE_SHA224 = 9,
    WC_HASH_TYPE_SHA256 = 5,
    WC_HASH_TYPE_SHA384 = 6,
    WC_HASH_TYPE_SHA512 = 7,
    WC_HASH_TYPE_MD5_SHA = 8,
};

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
    #ifdef WOLFSSL_SHA384
        Sha384 sha384;
    #endif
    #ifdef WOLFSSL_SHA512
        Sha512 sha512;
    #endif
} wc_HashAlg;

/* Find largest possible digest size
   Note if this gets up to the size of 80 or over check smallstack build */
#if defined(WOLFSSL_SHA512)
    #define WC_MAX_DIGEST_SIZE SHA512_DIGEST_SIZE
#elif defined(WOLFSSL_SHA384)
    #define WC_MAX_DIGEST_SIZE SHA384_DIGEST_SIZE
#elif !defined(NO_SHA256)
    #define WC_MAX_DIGEST_SIZE SHA256_DIGEST_SIZE
#elif defined(WOLFSSL_SHA224)
    #define WC_MAX_DIGEST_SIZE SHA224_DIGEST_SIZE
#elif !defined(NO_SHA)
    #define WC_MAX_DIGEST_SIZE SHA_DIGEST_SIZE
#elif !defined(NO_MD5)
    #define WC_MAX_DIGEST_SIZE MD5_DIGEST_SIZE
#else
    #define WC_MAX_DIGEST_SIZE 64 /* default to max size of 64 */
#endif

#if !defined(NO_ASN) || !defined(NO_DH) || defined(HAVE_ECC)
WOLFSSL_API int wc_HashGetOID(enum wc_HashType hash_type);
#endif

WOLFSSL_API int wc_HashGetDigestSize(enum wc_HashType hash_type);
WOLFSSL_API int wc_Hash(enum wc_HashType hash_type,
    const byte* data, word32 data_len,
    byte* hash, word32 hash_len);

/* generic hash operation wrappers */
WOLFSSL_API int wc_HashInit(wc_HashAlg* hash, enum wc_HashType type);
WOLFSSL_API int wc_HashUpdate(wc_HashAlg* hash, enum wc_HashType type,
    const byte* data, word32 dataSz);
WOLFSSL_API int wc_HashFinal(wc_HashAlg* hash, enum wc_HashType type,
    byte* out);


#ifndef NO_MD5
#include "crypto/src/md5.h"
WOLFSSL_API int wc_Md5Hash(const byte* data, word32 len, byte* hash);
#endif

#ifndef NO_SHA
#include "crypto/src/sha.h"
WOLFSSL_API int wc_ShaHash(const byte*, word32, byte*);
#endif

#ifndef NO_SHA256
#include "crypto/src/sha256.h"
WOLFSSL_API int wc_Sha256Hash(const byte*, word32, byte*);

    #if defined(WOLFSSL_SHA224)
        WOLFSSL_API int wc_Sha224Hash(const byte*, word32, byte*);
    #endif /* defined(WOLFSSL_SHA224) */
#endif

#ifdef WOLFSSL_SHA512
#include "crypto/src/sha512.h"
WOLFSSL_API int wc_Sha512Hash(const byte*, word32, byte*);

    #if defined(WOLFSSL_SHA384)
        WOLFSSL_API int wc_Sha384Hash(const byte*, word32, byte*);
    #endif /* defined(WOLFSSL_SHA384) */
#endif /* WOLFSSL_SHA512 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_HASH_H */
