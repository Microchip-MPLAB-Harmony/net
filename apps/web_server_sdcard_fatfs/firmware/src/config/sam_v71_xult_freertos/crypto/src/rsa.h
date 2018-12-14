/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    rsa.h

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

#ifndef WOLF_CRYPT_RSA_H
#define WOLF_CRYPT_RSA_H

#include "crypto/src/types.h"

#ifndef NO_RSA

/* allow for user to plug in own crypto */
#if !defined(HAVE_FIPS) && (defined(HAVE_USER_RSA) || defined(HAVE_FAST_RSA))
    #include "user_rsa.h"
#else

#ifdef HAVE_FIPS
/* for fips @wc_fips */
#include <cyassl/ctaocrypt/rsa.h>
#if defined(CYASSL_KEY_GEN) && !defined(WOLFSSL_KEY_GEN)
    #define WOLFSSL_KEY_GEN
#endif
#else
#include "crypto/src/integer.h"
#include "crypto/src/random.h"

#endif /* HAVE_FIPS */

/* header file needed for OAEP padding */
#include "crypto/src/hash.h"


#ifdef __cplusplus
    extern "C" {
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS)

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
    #ifdef WOLFSSL_CERT_GEN
        #include "crypto/src/asn.h"
    #endif
#endif

enum {
    RSA_PUBLIC   = 0,
    RSA_PRIVATE  = 1,

    RSA_TYPE_UNKNOWN    = -1,
    RSA_PUBLIC_ENCRYPT  = 0,
    RSA_PUBLIC_DECRYPT  = 1,
    RSA_PRIVATE_ENCRYPT = 2,
    RSA_PRIVATE_DECRYPT = 3,

    RSA_BLOCK_TYPE_1 = 1,
    RSA_BLOCK_TYPE_2 = 2,

    RSA_MIN_SIZE = 512,
    RSA_MAX_SIZE = 4096,

    RSA_MIN_PAD_SZ   = 11,     /* separator + 0 + pad value + 8 pads */

    RSA_PSS_PAD_SZ = 8
};


/* RSA */
struct RsaKey {
    mp_int n, e, d, p, q, dP, dQ, u;
    void* heap;                               /* for user memory overrides */
    byte* data;                               /* temp buffer for async RSA */
    int   type;                               /* public or private */
    int   state;
    word32 dataLen;
#ifdef WC_RSA_BLINDING
    WC_RNG* rng;                              /* for PrivateDecrypt blinding */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
    #ifdef WOLFSSL_CERT_GEN
        CertSignCtx certSignCtx; /* context info for cert sign (MakeSignature) */
    #endif
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_XILINX_CRYPT
    word32 pubExp; /* to keep values in scope they are here in struct */
    byte*  mod;
    XSecure_Rsa xRsa;
#endif
    byte   dataIsAlloc;
};

#ifndef WC_RSAKEY_TYPE_DEFINED
    typedef struct RsaKey RsaKey;
    #define WC_RSAKEY_TYPE_DEFINED
#endif

#endif /*HAVE_FIPS */

WOLFSSL_API int  wc_InitRsaKey(RsaKey* key, void* heap);
WOLFSSL_API int  wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId);
WOLFSSL_API int  wc_FreeRsaKey(RsaKey* key);
#ifdef WOLFSSL_XILINX_CRYPT
WOLFSSL_LOCAL int wc_InitRsaHw(RsaKey* key);
#endif /* WOLFSSL_XILINX_CRYPT */

WOLFSSL_LOCAL int wc_RsaFunction(const byte* in, word32 inLen, byte* out,
                           word32* outLen, int type, RsaKey* key, WC_RNG* rng);

WOLFSSL_API int  wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key);
WOLFSSL_API int  wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPSS_Sign(const byte* in, word32 inLen, byte* out,
                                word32 outLen, enum wc_HashType hash, int mgf,
                                RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out,
                                    RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_VerifyInline(byte* in, word32 inLen, byte** out,
                                        enum wc_HashType hash, int mgf,
                                        RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_CheckPadding(const byte* in, word32 inLen, byte* sig,
                                        word32 sigSz,
                                        enum wc_HashType hashType);

WOLFSSL_API int  wc_RsaEncryptSize(RsaKey* key);

#ifndef HAVE_FIPS /* to avoid asn duplicate symbols @wc_fips */
WOLFSSL_API int  wc_RsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                                               RsaKey*, word32);
WOLFSSL_API int  wc_RsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                                               RsaKey*, word32);
WOLFSSL_API int  wc_RsaPublicKeyDecodeRaw(const byte* n, word32 nSz,
                                        const byte* e, word32 eSz, RsaKey* key);
#ifdef WOLFSSL_KEY_GEN
    WOLFSSL_API int wc_RsaKeyToDer(RsaKey*, byte* output, word32 inLen);
#endif

WOLFSSL_API int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng);

/*
   choice of padding added after fips, so not available when using fips RSA
 */

/* Mask Generation Function Identifiers */
#define WC_MGF1NONE   0
#define WC_MGF1SHA1   26
#define WC_MGF1SHA224 4
#define WC_MGF1SHA256 1
#define WC_MGF1SHA384 2
#define WC_MGF1SHA512 3

/* Padding types */
#define WC_RSA_PKCSV15_PAD 0
#define WC_RSA_OAEP_PAD    1
#define WC_RSA_PSS_PAD     2

WOLFSSL_API int  wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                   word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 lableSz);
WOLFSSL_API int  wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen,
                   byte* out, word32 outLen, RsaKey* key, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 lableSz);
WOLFSSL_API int  wc_RsaPrivateDecryptInline_ex(byte* in, word32 inLen,
                      byte** out, RsaKey* key, int type, enum wc_HashType hash,
                      int mgf, byte* label, word32 lableSz);
#endif /* HAVE_FIPS*/
WOLFSSL_API int  wc_RsaFlattenPublicKey(RsaKey*, byte*, word32*, byte*,
                                                                       word32*);

#ifdef WOLFSSL_KEY_GEN
    WOLFSSL_API int wc_RsaKeyToPublicDer(RsaKey*, byte* output, word32 inLen);
    WOLFSSL_API int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng);
#endif

#endif /* HAVE_USER_RSA */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_RSA */
#endif /* WOLF_CRYPT_RSA_H */

