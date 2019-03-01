/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    signature.c
  
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


#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "configuration.h"
#include "crypto/src/settings.h"
#include "crypto/src/signature.h"
#include "crypto/src/error-crypt.h"
#include "crypto/src/logging.h"
#ifndef NO_ASN
#include "crypto/src/asn.h"
#endif
#ifdef HAVE_ECC
#include "crypto/src/ecc.h"
#endif
#ifndef NO_RSA
#include "crypto/src/rsa.h"
#endif

/* If ECC and RSA are disabled then disable signature wrapper */
#if (!defined(HAVE_ECC) || (defined(HAVE_ECC) && !defined(HAVE_ECC_SIGN) \
    && !defined(HAVE_ECC_VERIFY))) && defined(NO_RSA)
    #undef NO_SIG_WRAPPER
    #define NO_SIG_WRAPPER
#endif

/* Signature wrapper disabled check */
#ifndef NO_SIG_WRAPPER

#if !defined(NO_RSA) && !defined(NO_ASN)
static int wc_SignatureDerEncode(enum wc_HashType hash_type, byte* hash_data,
    word32 hash_len, word32* hash_enc_len)
{
    int ret, oid;

    ret = wc_HashGetOID(hash_type);
    if (ret < 0) {
        return ret;
    }
    oid = ret;

    ret = wc_EncodeSignature(hash_data, hash_data, hash_len, oid);
    if (ret > 0) {
        *hash_enc_len = ret;
        ret = 0;
    }

    return ret;
}
#endif /* !NO_RSA && !NO_ASN */

int wc_SignatureGetSize(enum wc_SignatureType sig_type,
    const void* key, word32 key_len)
{
    int sig_len = BAD_FUNC_ARG;

    /* Suppress possible unused args if all signature types are disabled */
    (void)key;
    (void)key_len;

    switch(sig_type) {
        case WC_SIGNATURE_TYPE_ECC:
#ifdef HAVE_ECC
            /* Sanity check that void* key is at least ecc_key in size */
            if (key_len >= sizeof(ecc_key)) {
                sig_len = wc_ecc_sig_size((ecc_key*)key);
            }
            else {
                WOLFSSL_MSG("wc_SignatureGetSize: Invalid ECC key size");
            }
#else
            sig_len = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_RSA_W_ENC:
        case WC_SIGNATURE_TYPE_RSA:
#ifndef NO_RSA
            /* Sanity check that void* key is at least RsaKey in size */
            if (key_len >= sizeof(RsaKey)) {
                sig_len = wc_RsaEncryptSize((RsaKey*)key);
            }
            else {
                WOLFSSL_MSG("wc_SignatureGetSize: Invalid RsaKey key size");
            }
#else
            sig_len = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_NONE:
        default:
            sig_len = BAD_FUNC_ARG;
            break;
    }
    return sig_len;
}

int wc_SignatureVerifyHash(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* hash_data, word32 hash_len,
    const byte* sig, word32 sig_len,
    const void* key, word32 key_len)
{
    int ret;

    /* Check arguments */
    if (hash_data == NULL || hash_len <= 0 ||
        sig == NULL || sig_len <= 0 ||
        key == NULL || key_len <= 0) {
        return BAD_FUNC_ARG;
    }

    /* Validate signature len (1 to max is okay) */
    if ((int)sig_len > wc_SignatureGetSize(sig_type, key, key_len)) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid sig type/len");
        return BAD_FUNC_ARG;
    }

    /* Validate hash size */
    ret = wc_HashGetDigestSize(hash_type);
    if (ret < 0) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid hash type/len");
        return ret;
    }
    ret = 0;

    /* Verify signature using hash */
    switch (sig_type) {
        case WC_SIGNATURE_TYPE_ECC:
        {
#if defined(HAVE_ECC) && defined(HAVE_ECC_VERIFY)
            int is_valid_sig = 0;

            /* Perform verification of signature using provided ECC key */
            do {
            #ifdef WOLFSSL_ASYNC_CRYPT
                ret = wc_AsyncWait(ret, &((ecc_key*)key)->asyncDev,
                    WC_ASYNC_FLAG_CALL_AGAIN);
            #endif
            if (ret >= 0)
                ret = wc_ecc_verify_hash(sig, sig_len, hash_data, hash_len,
                    &is_valid_sig, (ecc_key*)key);
            } while (ret == WC_PENDING_E);
            if (ret != 0 || is_valid_sig != 1) {
                ret = SIG_VERIFY_E;
            }
#else
            ret = SIG_TYPE_E;
#endif
            break;
        }

        case WC_SIGNATURE_TYPE_RSA_W_ENC:
        case WC_SIGNATURE_TYPE_RSA:
        {
#ifndef NO_RSA
            word32 plain_len = hash_len;
            byte *plain_data;

            /* Make sure the plain text output is at least key size */
            if (plain_len < sig_len) {
                plain_len = sig_len;
            }
            plain_data = (byte*)XMALLOC(plain_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (plain_data) {
                /* Perform verification of signature using provided RSA key */
                do {
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wc_AsyncWait(ret, &((RsaKey*)key)->asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                if (ret >= 0)
                    ret = wc_RsaSSL_Verify(sig, sig_len, plain_data,
                        plain_len, (RsaKey*)key);
                } while (ret == WC_PENDING_E);
                if (ret >= 0) {
                    if ((word32)ret == hash_len &&
                            XMEMCMP(plain_data, hash_data, hash_len) == 0) {
                        ret = 0; /* Success */
                    }
                    else {
                        WOLFSSL_MSG("RSA Signature Verify difference!");
                        ret = SIG_VERIFY_E;
                    }
                }
                XFREE(plain_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            }
            else {
                ret = MEMORY_E;
            }
#else
            ret = SIG_TYPE_E;
#endif
            break;
        }

        case WC_SIGNATURE_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
            break;
    }

    return ret;
}

int wc_SignatureVerify(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    const byte* sig, word32 sig_len,
    const void* key, word32 key_len)
{
    int ret;
    word32 hash_len, hash_enc_len;
#ifdef WOLFSSL_SMALL_STACK
    byte *hash_data;
#else
    byte hash_data[MAX_DER_DIGEST_SZ];
#endif

    /* Check arguments */
    if (data == NULL || data_len <= 0 ||
        sig == NULL || sig_len <= 0 ||
        key == NULL || key_len <= 0) {
        return BAD_FUNC_ARG;
    }

    /* Validate signature len (1 to max is okay) */
    if ((int)sig_len > wc_SignatureGetSize(sig_type, key, key_len)) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid sig type/len");
        return BAD_FUNC_ARG;
    }

    /* Validate hash size */
    ret = wc_HashGetDigestSize(hash_type);
    if (ret < 0) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid hash type/len");
        return ret;
    }
    hash_enc_len = hash_len = ret;

#ifndef NO_RSA
    if (sig_type == WC_SIGNATURE_TYPE_RSA_W_ENC) {
        /* For RSA with ASN.1 encoding include room */
        hash_enc_len += MAX_DER_DIGEST_ASN_SZ;
    }
#endif

#ifdef WOLFSSL_SMALL_STACK
    /* Allocate temporary buffer for hash data */
    hash_data = (byte*)XMALLOC(hash_enc_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (hash_data == NULL) {
        return MEMORY_E;
    }
#endif

    /* Perform hash of data */
    ret = wc_Hash(hash_type, data, data_len, hash_data, hash_len);
    if (ret == 0) {
        /* Handle RSA with DER encoding */
        if (sig_type == WC_SIGNATURE_TYPE_RSA_W_ENC) {
        #if defined(NO_RSA) || defined(NO_ASN)
            ret = SIG_TYPE_E;
        #else
            ret = wc_SignatureDerEncode(hash_type, hash_data, hash_len,
                &hash_enc_len);
        #endif
        }

        if (ret == 0) {
            /* Verify signature using hash */
            ret = wc_SignatureVerifyHash(hash_type, sig_type,
                hash_data, hash_enc_len, sig, sig_len, key, key_len);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}


int wc_SignatureGenerateHash(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* hash_data, word32 hash_len,
    byte* sig, word32 *sig_len,
    const void* key, word32 key_len, WC_RNG* rng)
{
    int ret;

    /* Suppress possible unused arg if all signature types are disabled */
    (void)rng;

    /* Check arguments */
    if (hash_data == NULL || hash_len <= 0 ||
        sig == NULL || sig_len == NULL || *sig_len <= 0 ||
        key == NULL || key_len <= 0) {
        return BAD_FUNC_ARG;
    }

    /* Validate signature len (needs to be at least max) */
    if ((int)*sig_len < wc_SignatureGetSize(sig_type, key, key_len)) {
        WOLFSSL_MSG("wc_SignatureGenerate: Invalid sig type/len");
        return BAD_FUNC_ARG;
    }

    /* Validate hash size */
    ret = wc_HashGetDigestSize(hash_type);
    if (ret < 0) {
        WOLFSSL_MSG("wc_SignatureGenerate: Invalid hash type/len");
        return ret;
    }
    ret = 0;

    /* Create signature using hash as data */
    switch (sig_type) {
        case WC_SIGNATURE_TYPE_ECC:
#if defined(HAVE_ECC) && defined(HAVE_ECC_SIGN)
            /* Create signature using provided ECC key */
            do {
            #ifdef WOLFSSL_ASYNC_CRYPT
                ret = wc_AsyncWait(ret, &((ecc_key*)key)->asyncDev,
                    WC_ASYNC_FLAG_CALL_AGAIN);
            #endif
            if (ret >= 0)
                ret = wc_ecc_sign_hash(hash_data, hash_len, sig, sig_len,
                    rng, (ecc_key*)key);
            } while (ret == WC_PENDING_E);
#else
            ret = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_RSA_W_ENC:
        case WC_SIGNATURE_TYPE_RSA:
#if !defined(NO_RSA) && !defined(WOLFSSL_RSA_PUBLIC_ONLY)
            /* Create signature using provided RSA key */
            do {
            #ifdef WOLFSSL_ASYNC_CRYPT
                ret = wc_AsyncWait(ret, &((RsaKey*)key)->asyncDev,
                    WC_ASYNC_FLAG_CALL_AGAIN);
            #endif
                if (ret >= 0)
                    ret = wc_RsaSSL_Sign(hash_data, hash_len, sig, *sig_len,
                        (RsaKey*)key, rng);
            } while (ret == WC_PENDING_E);
            if (ret >= 0) {
                *sig_len = ret;
                ret = 0; /* Success */
            }
#else
            ret = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
            break;
    }

    return ret;
}

int wc_SignatureGenerate(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    byte* sig, word32 *sig_len,
    const void* key, word32 key_len, WC_RNG* rng)
{
    int ret;
    word32 hash_len, hash_enc_len;
#ifdef WOLFSSL_SMALL_STACK
    byte *hash_data;
#else
    byte hash_data[MAX_DER_DIGEST_SZ];
#endif

    /* Check arguments */
    if (data == NULL || data_len <= 0 ||
        sig == NULL || sig_len == NULL || *sig_len <= 0 ||
        key == NULL || key_len <= 0) {
        return BAD_FUNC_ARG;
    }

    /* Validate signature len (needs to be at least max) */
    if ((int)*sig_len < wc_SignatureGetSize(sig_type, key, key_len)) {
        WOLFSSL_MSG("wc_SignatureGenerate: Invalid sig type/len");
        return BAD_FUNC_ARG;
    }

    /* Validate hash size */
    ret = wc_HashGetDigestSize(hash_type);
    if (ret < 0) {
        WOLFSSL_MSG("wc_SignatureGenerate: Invalid hash type/len");
        return ret;
    }
    hash_enc_len = hash_len = ret;

#if !defined(NO_RSA) && !defined(WOLFSSL_RSA_PUBLIC_ONLY)
    if (sig_type == WC_SIGNATURE_TYPE_RSA_W_ENC) {
        /* For RSA with ASN.1 encoding include room */
        hash_enc_len += MAX_DER_DIGEST_ASN_SZ;
    }
#endif

#ifdef WOLFSSL_SMALL_STACK
    /* Allocate temporary buffer for hash data */
    hash_data = (byte*)XMALLOC(hash_enc_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (hash_data == NULL) {
        return MEMORY_E;
    }
#endif

    /* Perform hash of data */
    ret = wc_Hash(hash_type, data, data_len, hash_data, hash_len);
    if (ret == 0) {
        /* Handle RSA with DER encoding */
        if (sig_type == WC_SIGNATURE_TYPE_RSA_W_ENC) {
        #if defined(NO_RSA) || defined(NO_ASN) || \
                                                defined(WOLFSSL_RSA_PUBLIC_ONLY)
            ret = SIG_TYPE_E;
        #else
            ret = wc_SignatureDerEncode(hash_type, hash_data, hash_len,
                &hash_enc_len);
        #endif
        }

        if (ret == 0) {
            /* Generate signature using hash */
            ret = wc_SignatureGenerateHash(hash_type, sig_type,
                hash_data, hash_enc_len, sig, sig_len, key, key_len, rng);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif /* NO_SIG_WRAPPER */
