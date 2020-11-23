/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto.c

  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic
	Framework Library for PIC32 families of Microchip microcontrollers.
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


/* Implements Microchip CRYPTO API layer */
#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif
#include "configuration.h"

#include "crypto/crypto.h"

#include "wolfssl/wolfcrypt/md5.h"
#include "wolfssl/wolfcrypt/sha.h"
#include "wolfssl/wolfcrypt/sha256.h"
#include "wolfssl/wolfcrypt/sha512.h"
#include "wolfssl/wolfcrypt/hmac.h"
#include "wolfssl/wolfcrypt/compress.h"
#include "wolfssl/wolfcrypt/random.h"
#include "wolfssl/wolfcrypt/des3.h"
#include "wolfssl/wolfcrypt/aes.h"
#include "wolfssl/wolfcrypt/rsa.h"
#include "wolfssl/wolfcrypt/ecc.h"
#include "wolfssl/wolfcrypt/error-crypt.h"

#ifndef NO_MD5
/* Initialize MD5 */
int CRYPT_MD5_Initialize(CRYPT_MD5_CTX* md5)
{
    typedef char md5_test[sizeof(CRYPT_MD5_CTX) >= sizeof(Md5) ? 1 : -1];
    (void)sizeof(md5_test);

    if (md5 == NULL)
        return BAD_FUNC_ARG;

    wc_InitMd5((Md5*)md5);

    return 0;
}

int CRYPT_MD5_DataSizeSet(CRYPT_MD5_CTX* md5, unsigned int sz)
{
    if (md5 == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_PIC32MZ_HASH
    wc_Md5SizeSet((Md5*)md5, sz);
#endif

    return 0;
}


/* Add data to MD5 */
int CRYPT_MD5_DataAdd(CRYPT_MD5_CTX* md5, const unsigned char* input,
                      unsigned int sz)
{
    if (md5 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Md5Update((Md5*)md5, input, sz);
}


/* Get MD5 Final into digest */
int CRYPT_MD5_Finalize(CRYPT_MD5_CTX* md5, unsigned char* digest)
{
    if (md5 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Md5Final((Md5*)md5, digest);
}
#endif  // NO_MD5

#ifndef NO_SHA
/* Initialize SHA */
int CRYPT_SHA_Initialize(CRYPT_SHA_CTX* sha)
{
    typedef char sha_test[sizeof(CRYPT_SHA_CTX) >= sizeof(Sha) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha((Sha*)sha);
}

int CRYPT_SHA_DataSizeSet(CRYPT_SHA_CTX* sha, unsigned int sz)
{
    if (sha == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_PIC32MZ_HASH
    wc_ShaSizeSet((Sha*)sha, sz);
#endif

    return 0;
}


/* Add data to SHA */
int CRYPT_SHA_DataAdd(CRYPT_SHA_CTX* sha, const unsigned char* input,
                       unsigned int sz)
{
    if (sha == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_ShaUpdate((Sha*)sha, input, sz);
}


/* Get SHA Final into digest */
int CRYPT_SHA_Finalize(CRYPT_SHA_CTX* sha, unsigned char* digest)
{
    if (sha == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_ShaFinal((Sha*)sha, digest);
}
#endif // NO_SHA

#ifndef NO_SHA256
/* Initialize SHA-256 */
int CRYPT_SHA256_Initialize(CRYPT_SHA256_CTX* sha256)
{
    typedef char sha_test[sizeof(CRYPT_SHA256_CTX) >= sizeof(Sha256) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha256 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha256((Sha256*)sha256);
}

int CRYPT_SHA256_DataSizeSet(CRYPT_SHA256_CTX* sha256, unsigned int sz)
{
    if (sha256 == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_PIC32MZ_HASH
    wc_Sha256SizeSet((Sha256*)sha256, sz);
#endif

    return 0;
}


/* Add data to SHA-256 */
int CRYPT_SHA256_DataAdd(CRYPT_SHA256_CTX* sha256, const unsigned char* input,
                         unsigned int sz)
{
    if (sha256 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha256Update((Sha256*)sha256, input, sz);
}


/* Get SHA-256 Final into digest */
int CRYPT_SHA256_Finalize(CRYPT_SHA256_CTX* sha256, unsigned char* digest)
{
    if (sha256 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha256Final((Sha256*)sha256, digest);
}
#endif // NO_SHA256
#ifdef WOLFSSL_SHA224

/* Initialize SHA-224 */
int CRYPT_SHA224_Initialize(CRYPT_SHA256_CTX* sha224)
{
    typedef char sha_test[sizeof(CRYPT_SHA256_CTX) >= sizeof(Sha224) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha224 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha224((Sha224*)sha224);
}


/* Add data to SHA-224 */
int CRYPT_SHA224_DataAdd(CRYPT_SHA256_CTX* sha224, const unsigned char* input,
                         unsigned int sz)
{
    if (sha224 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha224Update((Sha224*)sha224, input, sz);
}


/* Get SHA-224 Final into digest */
int CRYPT_SHA224_Finalize(CRYPT_SHA256_CTX* sha224, unsigned char* digest)
{
    if (sha224 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha224Final((Sha224*)sha224, digest);
}

#endif  // WOLFSSL_SHA224
#ifdef WOLFSSL_SHA384

/* Initialize SHA-384 */
int CRYPT_SHA384_Initialize(CRYPT_SHA384_CTX* sha384)
{
    typedef char sha_test[sizeof(CRYPT_SHA384_CTX) >= sizeof(Sha384) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha384 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha384((Sha384*)sha384);
}


/* Add data to SHA-384 */
int CRYPT_SHA384_DataAdd(CRYPT_SHA384_CTX* sha384, const unsigned char* input,
                         unsigned int sz)
{
    if (sha384 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha384Update((Sha384*)sha384, input, sz);
}


/* Get SHA-384 Final into digest */
int CRYPT_SHA384_Finalize(CRYPT_SHA384_CTX* sha384, unsigned char* digest)
{
    if (sha384 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha384Final((Sha384*)sha384, digest);
}

#endif  // WOLFSSL_SHA384

#ifdef WOLFSSL_SHA512

/* Initialize SHA-512 */
int CRYPT_SHA512_Initialize(CRYPT_SHA512_CTX* sha512)
{
    typedef char sha_test[sizeof(CRYPT_SHA512_CTX) >= sizeof(Sha512) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha512 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha512((Sha512*)sha512);
}


/* Add data to SHA-512 */
int CRYPT_SHA512_DataAdd(CRYPT_SHA512_CTX* sha512, const unsigned char* input,
                         unsigned int sz)
{
    if (sha512 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha512Update((Sha512*)sha512, input, sz);
}


/* Get SHA-512 Final into digest */
int CRYPT_SHA512_Finalize(CRYPT_SHA512_CTX* sha512, unsigned char* digest)
{
    if (sha512 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha512Final((Sha512*)sha512, digest);
}

#endif  // WOLFSSL_SHA512

#ifndef NO_HMAC

/* Set HMAC key with type */
int CRYPT_HMAC_SetKey(CRYPT_HMAC_CTX* hmac, int type, const unsigned char* key,
                      unsigned int sz)
{
    typedef char hmac_test[sizeof(CRYPT_HMAC_CTX) >= sizeof(Hmac) ? 1 : -1];
    (void)sizeof(hmac_test);

    if (hmac == NULL || key == NULL || 0 == sz)
        return BAD_FUNC_ARG;

    int wc_sha;
    if      (CRYPT_HMAC_SHA    == type) wc_sha = WC_SHA;
    else if (CRYPT_HMAC_SHA256 == type) wc_sha = WC_SHA256;
    else if (CRYPT_HMAC_SHA384 == type) wc_sha = WC_SHA384;
    else if (CRYPT_HMAC_SHA512 == type) wc_sha = WC_SHA512;
    else return BAD_FUNC_ARG;

    return wc_HmacSetKey((Hmac*)hmac, wc_sha, key, sz);
}


int CRYPT_HMAC_DataAdd(CRYPT_HMAC_CTX* hmac, const unsigned char* input,
                       unsigned int sz)
{
    if (hmac == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_HmacUpdate((Hmac*)hmac, input, sz);
}


/* Get HMAC Final into digest */
int CRYPT_HMAC_Finalize(CRYPT_HMAC_CTX* hmac, unsigned char* digest)
{
    if (hmac == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_HmacFinal((Hmac*)hmac, digest);
}

#endif // NO_HMAC

#ifdef HAVE_LIBZ

/* Huffman Compression, set flag to do static, otherwise dynamic */
/* return compressed size, otherwise < 0 for error */
int CRYPT_HUFFMAN_Compress(unsigned char* out, unsigned int outSz,
                           const unsigned char* in, unsigned int inSz,
                           unsigned int flags)
{
    if (out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_Compress(out, outSz, in, inSz, flags);
}


/* Huffman DeCompression, self determines type */
/* return decompressed size, otherwise < 0 for error */
int CRYPT_HUFFMAN_DeCompress(unsigned char* out, unsigned int outSz,
                             const unsigned char* in, unsigned int inSz)
{
    if (out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_DeCompress(out, outSz, in, inSz);
}

#endif  // HAVE_LIBZ

#ifndef NO_RNG

/* RNG Initialize, < 0 on error */
int CRYPT_RNG_Initialize(CRYPT_RNG_CTX* rng)
{
    typedef char rng_test[sizeof(CRYPT_RNG_CTX) >= sizeof(WC_RNG) ? 1 : -1];
    (void)sizeof(rng_test);

    if (rng == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_MICROCHIP_SAME70
	return same70_InitRng();
#else
    return wc_InitRng((WC_RNG*)rng);
#endif
}


/* RNG Get single bytes, < 0 on error */
int CRYPT_RNG_Get(CRYPT_RNG_CTX* rng, unsigned char* b)
{
    if (rng == NULL || b == NULL)
        return BAD_FUNC_ARG;

    return wc_RNG_GenerateByte((WC_RNG*)rng, (byte*)b);
}


/* RNG Block Generation of sz bytes, < 0 on error */
int CRYPT_RNG_BlockGenerate(CRYPT_RNG_CTX* rng, unsigned char* b,
                            unsigned int sz)
{
    if (rng == NULL || b == NULL)
        return BAD_FUNC_ARG;

#if defined(WOLFSSL_MICROCHIP_SAME70)
    return same70_RNG_GenerateBlock(b, sz);
#else
    return wc_RNG_GenerateBlock((WC_RNG*)rng, b, sz);
#endif
}

#endif

#ifndef NO_DES3
/* Triple DES Key Set, may have iv, will have direction */
int CRYPT_TDES_KeySet(CRYPT_TDES_CTX* tdes, const unsigned char* key,
                      const unsigned char* iv, int dir)
{
    typedef char tdes_test[sizeof(CRYPT_TDES_CTX) >= sizeof(Des3) ? 1 : -1];
    (void)sizeof(tdes_test);

    if (tdes == NULL || key == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_SetKey((Des3*)tdes, key, iv, dir);
}


/* Triple DES Iv Set, sometimes added later */
int CRYPT_TDES_IvSet(CRYPT_TDES_CTX* tdes, const unsigned char* iv)
{
    if (tdes == NULL || iv == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_SetIV((Des3*)tdes, iv);
}


/* Triple DES CBC Encrypt */
int CRYPT_TDES_CBC_Encrypt(CRYPT_TDES_CTX* tdes, unsigned char* out,
                           const unsigned char* in, unsigned int inSz)
{
    if (tdes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_CbcEncrypt((Des3*)tdes, out, in, inSz);
}


/* Triple DES CBC Decrypt */
int CRYPT_TDES_CBC_Decrypt(CRYPT_TDES_CTX* tdes, unsigned char* out,
                           const unsigned char* in, unsigned int inSz)
{
    if (tdes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_CbcDecrypt((Des3*)tdes, out, in, inSz);
}

#endif

#ifndef NO_AES

/* AES Key Set, may have iv, will have direction */
int CRYPT_AES_KeySet(CRYPT_AES_CTX* aes, const unsigned char* key,
                     unsigned int keyLen, const unsigned char* iv, int dir)
{
    typedef char aes_test[sizeof(CRYPT_AES_CTX) >= sizeof(Aes) ? 1 : -1];
    (void)sizeof(aes_test);

    if (aes == NULL || key == NULL)
        return BAD_FUNC_ARG;

    return wc_AesSetKey((Aes*)aes, key, keyLen, iv, dir);
}


/* AES Iv Set, sometimes added later */
int CRYPT_AES_IvSet(CRYPT_AES_CTX* aes, const unsigned char* iv)
{
    if (aes == NULL || iv == NULL)
        return BAD_FUNC_ARG;

    return wc_AesSetIV((Aes*)aes, iv);
}


#ifdef HAVE_AES_CBC
int CRYPT_AES_CBC_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                          const unsigned char* in, unsigned int inSz)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_AesCbcEncrypt((Aes*)aes, out, in, inSz);
}


/* AES CBC Decrypt */
int CRYPT_AES_CBC_Decrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                          const unsigned char* in, unsigned int inSz)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_AesCbcDecrypt((Aes*)aes, out, in, inSz);
}
#endif /* HAVE_AES_CBC */

#ifdef WOLFSSL_AES_COUNTER

/* AES CTR Encrypt (used for decrypt too, with ENCRYPT key setup) */
int CRYPT_AES_CTR_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                          const unsigned char* in, unsigned int inSz)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_AesCtrEncrypt((Aes*)aes, out, in, inSz);
}

#endif /* WOLFSSL_AES_COUNTER */

#if defined(HAVE_AESGCM)
int CRYPT_AES_GCM_SetKey(CRYPT_AES_CTX* aes, const unsigned char* key, unsigned int len)
{
    typedef char aes_test[sizeof(CRYPT_AES_CTX) >= sizeof(Aes) ? 1 : -1];
    (void)sizeof(aes_test);

    if (aes == NULL || key == NULL)
    {
        return BAD_FUNC_ARG;
    }
    return wc_AesGcmSetKey((Aes*)aes, key, len);
}

int CRYPT_AES_GCM_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz)
{
    if (aes == NULL || out == NULL)
    {
         return BAD_FUNC_ARG;
    }
    return wc_AesGcmEncrypt((Aes*)aes, out, in, sz, iv, ivSz, authTag, authTagSz, authIn, authInSz);
}

int CRYPT_AES_GCM_Decrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   const unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz)
{
    if (aes == NULL)
    {
         return BAD_FUNC_ARG;
    }
    return wc_AesGcmDecrypt((Aes*)aes, out, in, sz, iv, ivSz, authTag, authTagSz, authIn, authInSz);
}

#endif

#ifdef WOLFSSL_AES_DIRECT

/* AES Direct mode encrypt, one block at a time */
int CRYPT_AES_DIRECT_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                             const unsigned char* in)
{
    if (aes == NULL)
        return BAD_FUNC_ARG;

    wc_AesEncryptDirect((Aes*)aes, out, in);

    return 0;
}


/* AES Direct mode decrypt, one block at a time */
int CRYPT_AES_DIRECT_Decrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                             const unsigned char* in)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    wc_AesDecryptDirect((Aes*)aes, out, in);

    return 0;
}

#endif /* WOLFSSL_AES_DIRECT */
#endif /* NO_AES */

#ifndef NO_RSA

/* RSA Initialize */
int CRYPT_RSA_Initialize(CRYPT_RSA_CTX* rsa)
{
    if (rsa == NULL)
        return BAD_FUNC_ARG;

    rsa->holder = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_RSA);
    if (rsa->holder == NULL)
        return -1;

    return wc_InitRsaKey_ex((RsaKey*)rsa->holder, NULL, 0);
}


/* RSA Free resources */
int CRYPT_RSA_Free(CRYPT_RSA_CTX* rsa)
{
    if (rsa == NULL)
        return BAD_FUNC_ARG;

    wc_FreeRsaKey((RsaKey*)rsa->holder);
    XFREE(rsa->holder, NULL, DYNAMIC_TYPE_RSA);
    rsa->holder = NULL;

    return 0;
}


/* RSA Public key decode ASN.1 */
int CRYPT_RSA_PublicKeyDecode(CRYPT_RSA_CTX* rsa, const unsigned char* in,
                              unsigned int inSz)
{
    unsigned int idx = 0;
    (void)idx;

    if (rsa == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPublicKeyDecode(in, &idx, (RsaKey*)rsa->holder, inSz);
}


/* RSA Private key decode ASN.1 */
int CRYPT_RSA_PrivateKeyDecode(CRYPT_RSA_CTX* rsa, const unsigned char* in,
                               unsigned int inSz)
{
    unsigned int idx = 0;
    (void)idx;

    if (rsa == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPrivateKeyDecode(in, &idx, (RsaKey*)rsa->holder, inSz);
}


/* RSA Public Encrypt */
int CRYPT_RSA_PublicEncrypt(CRYPT_RSA_CTX* rsa, unsigned char* out,
                            unsigned int outSz, const unsigned char* in,
                            unsigned int inSz, CRYPT_RNG_CTX* rng)
{
    if (rsa == NULL || in == NULL || out == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPublicEncrypt(in, inSz, out, outSz, (RsaKey*)rsa->holder,
                            (WC_RNG*)rng);
}


/* RSA Private Decrypt */
int CRYPT_RSA_PrivateDecrypt(CRYPT_RSA_CTX* rsa, unsigned char* out,
                             unsigned int outSz, const unsigned char* in,
                             unsigned int inSz)
{
    if (rsa == NULL || in == NULL || out == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPrivateDecrypt(in, inSz, out, outSz, (RsaKey*)rsa->holder);
}


/* RSA Get Encrypt size helper */
int CRYPT_RSA_EncryptSizeGet(CRYPT_RSA_CTX* rsa)
{
    if (rsa == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaEncryptSize((RsaKey*)rsa->holder);
}

#endif // NO_RSA

#ifdef HAVE_ECC

/* ECC init */
int CRYPT_ECC_Initialize(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    ecc->holder = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
    if (ecc->holder == NULL)
        return -1;

    return wc_ecc_init((ecc_key*)ecc->holder);
}


/* ECC free resources */
int CRYPT_ECC_Free(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    wc_ecc_free((ecc_key*)ecc->holder);
    XFREE(ecc->holder, NULL, DYNAMIC_TYPE_ECC);
    ecc->holder = NULL;

    return 0;
}


/* ECC Public x963 Export */
int CRYPT_ECC_PublicExport(CRYPT_ECC_CTX* ecc, unsigned char* out,
                           unsigned int outSz, unsigned int* usedSz)
{
    int          ret;
    unsigned int inOut = outSz;

    if (ecc == NULL || out == NULL)
        return BAD_FUNC_ARG;

    ret = wc_ecc_export_x963((ecc_key*)ecc->holder, out, &inOut);
    *usedSz = inOut;

    return ret;
}


/* ECC Public x963 Import */
int CRYPT_ECC_PublicImport(CRYPT_ECC_CTX* ecc, const unsigned char* in,
                           unsigned int inSz)
{
    if (ecc == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_import_x963(in, inSz, (ecc_key*)ecc->holder);
}


/* ECC Private x963 Import */
int CRYPT_ECC_PrivateImport(CRYPT_ECC_CTX* ecc, const unsigned char* priv,
         unsigned int privSz, const unsigned char* pub, unsigned int pubSz)
{
    if (ecc == NULL || priv == NULL || pub == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_import_private_key(priv, privSz, pub, pubSz,
                                 (ecc_key*)ecc->holder);
}


/* ECC DHE Make key */
int CRYPT_ECC_DHE_KeyMake(CRYPT_ECC_CTX* ecc, CRYPT_RNG_CTX* rng, int keySz)
{
    if (ecc == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_make_key((WC_RNG*)rng, keySz, (ecc_key*)ecc->holder);
}


/* ECC DHE Make shared secret with our private and peer public */
int CRYPT_ECC_DHE_SharedSecretMake(CRYPT_ECC_CTX* priv, CRYPT_ECC_CTX* pub,
                  unsigned char* out, unsigned int outSz, unsigned int* usedSz)
{
    int ret;
    unsigned int inOut = outSz;

    if (priv == NULL || pub == NULL || out == NULL || usedSz == NULL)
        return BAD_FUNC_ARG;

    ret = wc_ecc_shared_secret((ecc_key*)priv->holder, (ecc_key*)pub->holder,
                            out, &inOut);
    *usedSz = inOut;

    return ret;
}


/* ECC DSA Hash Sign */
int CRYPT_ECC_DSA_HashSign(CRYPT_ECC_CTX* ecc, CRYPT_RNG_CTX* rng,
                           unsigned char* sig, unsigned int sigSz,
                           unsigned int* usedSz, const unsigned char* in,
                           unsigned int inSz)
{
    int ret;
    unsigned int inOut = sigSz;

    if (ecc == NULL || rng == NULL || sig == NULL || usedSz == NULL ||
                                                                in == NULL)
        return BAD_FUNC_ARG;

    ret = wc_ecc_sign_hash(in, inSz, sig, &inOut, (WC_RNG*)rng,
                       (ecc_key*)ecc->holder);
    *usedSz = inOut;

    return ret;
}


/* ECC DSA Hash Verify */
int CRYPT_ECC_DSA_HashVerify(CRYPT_ECC_CTX* ecc, const unsigned char* sig,
                             unsigned int sigSz, unsigned char* hash,
                             unsigned int hashSz, int* status)
{
    if (ecc == NULL || sig == NULL || hash == NULL || status == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_verify_hash(sig, sigSz, hash, hashSz, status,
                          (ecc_key*)ecc->holder);
}


/* ECC get key size helper */
int CRYPT_ECC_KeySizeGet(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_size((ecc_key*)ecc->holder);
}


/* ECC get signature size helper */
int CRYPT_ECC_SignatureSizeGet(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_sig_size((ecc_key*)ecc->holder);
}
#endif // HAVE_ECC


/* Save error string from err to str which needs to be >= 80 chars */
int CRYPT_ERROR_StringGet(int err, char* str)
{
    if (str == NULL)
        return BAD_FUNC_ARG;

    wc_ErrorString(err, str);

    return 0;
}

