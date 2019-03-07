/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    aes.h

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


#ifndef WOLF_CRYPT_AES_H
#define WOLF_CRYPT_AES_H

#include "crypto/src/types.h"

#ifndef NO_AES

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include "crypto/src/fips.h"
#endif /* HAVE_FIPS_VERSION >= 2 */

/* included for fips @wc_fips */
#if defined(HAVE_FIPS) && \
    (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))
#include <cyassl/ctaocrypt/aes.h>
#if defined(CYASSL_AES_COUNTER) && !defined(WOLFSSL_AES_COUNTER)
    #define WOLFSSL_AES_COUNTER
#endif
#if !defined(WOLFSSL_AES_DIRECT) && defined(CYASSL_AES_DIRECT)
    #define WOLFSSL_AES_DIRECT
#endif
#endif

#ifndef WC_NO_RNG
    #include "crypto/src/random.h"
#endif
#ifdef STM32_CRYPTO
    #include "crypto/src/port/st/stm32.h"
#endif

#ifdef WOLFSSL_AESNI

#include <wmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

#endif /* WOLFSSL_AESNI */


#ifdef WOLFSSL_XILINX_CRYPT
#include "xsecure_aes.h"
#endif

#ifdef WOLFSSL_AFALG
/* included for struct msghdr */
#include <sys/socket.h>
#endif

#if defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC)
#include "crypto/src/port/devcrypto/wc_devcrypto.h"
#endif



#if defined(HAVE_AESGCM) && !defined(WC_NO_RNG)
    #include "crypto/src/random.h"
#endif


#ifdef __cplusplus
    extern "C" {
#endif

/* these are required for FIPS and non-FIPS */
enum {
    AES_128_KEY_SIZE    = 16,  /* for 128 bit             */
    AES_192_KEY_SIZE    = 24,  /* for 192 bit             */
    AES_256_KEY_SIZE    = 32,  /* for 256 bit             */

    AES_IV_SIZE         = 16,  /* always block size       */
};


/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

enum {
    AES_ENC_TYPE   = WC_CIPHER_AES,   /* cipher unique type */
    AES_ENCRYPTION = 0,
    AES_DECRYPTION = 1,

    AES_BLOCK_SIZE      = 16,

    KEYWRAP_BLOCK_SIZE  = 8,

    GCM_NONCE_MAX_SZ = 16, /* wolfCrypt's maximum nonce size allowed. */
    GCM_NONCE_MID_SZ = 12, /* The usual default nonce size for AES-GCM. */
    GCM_NONCE_MIN_SZ = 8,  /* wolfCrypt's minimum nonce size allowed. */
    CCM_NONCE_MIN_SZ = 7,
    CCM_NONCE_MAX_SZ = 13,
    CTR_SZ   = 4,
    AES_IV_FIXED_SZ = 4,

#ifdef HAVE_PKCS11
    AES_MAX_ID_LEN   = 32,
#endif
};
#if defined(HAVE_MICROCHIP_HARMONY3_HW_AES)
#include "crypt_aes_hw.h"
#endif

typedef struct Aes {
#if defined(HAVE_MICROCHIP_HARMONY3_HW_AES)
    crypt_aes_hw_descriptor hwDesc;
#else
    /* AESNI needs key first, rounds 2nd, not sure why yet */
    ALIGN16 word32 key[60];
    word32  rounds;
    int     keylen;

    ALIGN16 word32 reg[AES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
    ALIGN16 word32 tmp[AES_BLOCK_SIZE / sizeof(word32)];      /* same         */

#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM)
    word32 invokeCtr[2];
    word32 nonceSz;
#endif
#ifdef HAVE_AESGCM
    ALIGN16 byte H[AES_BLOCK_SIZE];
#ifdef GCM_TABLE
    /* key-based fast multiplication table. */
    ALIGN16 byte M0[256][AES_BLOCK_SIZE];
#endif /* GCM_TABLE */
#endif /* HAVE_AESGCM */
#ifdef WOLFSSL_AESNI
    byte use_aesni;
#endif /* WOLFSSL_AESNI */
#ifdef WOLF_CRYPTO_DEV
    int   devId;
#endif
#ifdef HAVE_PKCS11
    byte id[AES_MAX_ID_LEN];
    int  idLen;
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    word32 asyncKey[AES_MAX_KEY_SIZE/8/sizeof(word32)]; /* raw key */
    word32 asyncIv[AES_BLOCK_SIZE/sizeof(word32)]; /* raw IV */
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
#if defined(WOLFSSL_AES_COUNTER) || defined(WOLFSSL_AES_CFB)
    word32  left;            /* unused bytes left from last call */
#endif
#if defined( WOLFSSL_PIC32MZ_CRYPT ) || defined (WOLFSSL_SAME70_CRYPT)
    word32 key_ce[AES_BLOCK_SIZE*2/sizeof(word32)];
    word32 iv_ce [AES_BLOCK_SIZE  /sizeof(word32)];
#endif
#ifdef WOLFSSL_XILINX_CRYPT
    XSecure_Aes xilAes;
    XCsuDma     dma;
    word32      key_init[8];
    word32      kup;
#endif
#ifdef WOLFSSL_AFALG
    int alFd; /* server socket to bind to */
    int rdFd; /* socket to read from */
    struct msghdr msg;
    int dir;  /* flag for encrpyt or decrypt */
#endif
#if defined(WOLFSSL_DEVCRYPTO) && \
    (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))
    word32       devKey[AES_MAX_KEY_SIZE/WOLFSSL_BIT_SIZE/sizeof(word32)]; /* raw key */
    WC_CRYPTODEV ctx;
#endif
    void*  heap; /* memory hint to use */
#endif
} Aes;

#ifdef WOLFSSL_AES_XTS
typedef struct XtsAes {
    Aes aes;
    Aes tweak;
} XtsAes;
#endif

#ifdef HAVE_AESGCM
typedef struct Gmac {
    Aes aes;
} Gmac;
#endif /* HAVE_AESGCM */
#endif /* HAVE_FIPS */


/* Authenticate cipher function prototypes */
typedef int (*wc_AesAuthEncryptFunc)(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
typedef int (*wc_AesAuthDecryptFunc)(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/* AES-CBC */
WOLFSSL_API int  wc_AesSetKey(Aes* aes, const byte* key, word32 len,
                              const byte* iv, int dir);
WOLFSSL_API int  wc_AesSetIV(Aes* aes, const byte* iv);

#ifdef HAVE_AES_CBC
WOLFSSL_API int  wc_AesCbcEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
WOLFSSL_API int  wc_AesCbcDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
#endif

#ifdef WOLFSSL_AES_CFB
WOLFSSL_API int wc_AesCfbEncrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#ifdef HAVE_AES_DECRYPT
WOLFSSL_API int wc_AesCfbDecrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_CFB */

#ifdef HAVE_AES_ECB
WOLFSSL_API int wc_AesEcbEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
WOLFSSL_API int wc_AesEcbDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
#endif

/* AES-CTR */
#ifdef WOLFSSL_AES_COUNTER
 WOLFSSL_API int wc_AesCtrEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz);
#endif
/* AES-DIRECT */
#if defined(WOLFSSL_AES_DIRECT)
 WOLFSSL_API void wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in);
 WOLFSSL_API void wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in);
 WOLFSSL_API int  wc_AesSetKeyDirect(Aes* aes, const byte* key, word32 len,
                                const byte* iv, int dir);
#endif

#ifdef HAVE_AESGCM
#ifdef WOLFSSL_XILINX_CRYPT
 WOLFSSL_API int  wc_AesGcmSetKey_ex(Aes* aes, const byte* key, word32 len,
         word32 kup);
#endif
 WOLFSSL_API int  wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len);
 WOLFSSL_API int  wc_AesGcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesGcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

#ifndef WC_NO_RNG
 WOLFSSL_API int  wc_AesGcmSetExtIV(Aes* aes, const byte* iv, word32 ivSz);
 WOLFSSL_API int  wc_AesGcmSetIV(Aes* aes, word32 ivSz,
                                   const byte* ivFixed, word32 ivFixedSz,
                                   WC_RNG* rng);
 WOLFSSL_API int  wc_AesGcmEncrypt_ex(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   byte* ivOut, word32 ivOutSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
#endif /* WC_NO_RNG */

 WOLFSSL_API int wc_GmacSetKey(Gmac* gmac, const byte* key, word32 len);
 WOLFSSL_API int wc_GmacUpdate(Gmac* gmac, const byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               byte* authTag, word32 authTagSz);
#ifndef WC_NO_RNG
 WOLFSSL_API int wc_Gmac(const byte* key, word32 keySz, byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               byte* authTag, word32 authTagSz, WC_RNG* rng);
 WOLFSSL_API int wc_GmacVerify(const byte* key, word32 keySz,
                               const byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               const byte* authTag, word32 authTagSz);
#endif /* WC_NO_RNG */
 WOLFSSL_LOCAL void GHASH(Aes* aes, const byte* a, word32 aSz, const byte* c,
                               word32 cSz, byte* s, word32 sSz);
#endif /* HAVE_AESGCM */
#ifdef HAVE_AESCCM
 WOLFSSL_API int  wc_AesCcmSetKey(Aes* aes, const byte* key, word32 keySz);
 WOLFSSL_API int  wc_AesCcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesCcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesCcmSetNonce(Aes* aes,
                                   const byte* nonce, word32 nonceSz);
 WOLFSSL_API int  wc_AesCcmEncrypt_ex(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   byte* ivOut, word32 ivOutSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
#endif /* HAVE_AESCCM */
#ifdef HAVE_AES_KEYWRAP
 WOLFSSL_API int  wc_AesKeyWrap(const byte* key, word32 keySz,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
 WOLFSSL_API int  wc_AesKeyUnWrap(const byte* key, word32 keySz,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
#endif /* HAVE_AES_KEYWRAP */

#ifdef WOLFSSL_AES_XTS

WOLFSSL_API int wc_AesXtsSetKey(XtsAes* aes, const byte* key,
         word32 len, int dir, void* heap, int devId);

WOLFSSL_API int wc_AesXtsEncryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

WOLFSSL_API int wc_AesXtsDecryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

WOLFSSL_API int wc_AesXtsEncrypt(XtsAes* aes, byte* out,
         const byte* in, word32 sz, const byte* i, word32 iSz);

WOLFSSL_API int wc_AesXtsDecrypt(XtsAes* aes, byte* out,
        const byte* in, word32 sz, const byte* i, word32 iSz);

WOLFSSL_API int wc_AesXtsFree(XtsAes* aes);
#endif

WOLFSSL_API int wc_AesGetKeySize(Aes* aes, word32* keySize);

WOLFSSL_API int  wc_AesInit(Aes* aes, void* heap, int devId);
#ifdef HAVE_PKCS11
WOLFSSL_API int  wc_AesInit_Id(Aes* aes, unsigned char* id, int len, void* heap,
        int devId);
#endif
WOLFSSL_API void wc_AesFree(Aes* aes);

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* NO_AES */
#endif /* WOLF_CRYPT_AES_H */
