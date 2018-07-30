/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    hash.c
  
  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic 
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
File Name:  hash.c
Copyright © 2016 released Microchip Technology Inc.  All rights reserved.

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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "system_config.h"
#include "crypto/src/settings.h"
#include "crypto/src/logging.h"
#include "crypto/src/error-crypt.h"
#ifndef NO_ASN
#include "crypto/src/asn.h"
#endif

#include "crypto/src/hash.h"


#if !defined(NO_ASN) || !defined(NO_DH) || defined(HAVE_ECC)

#ifdef NO_ASN
enum Hash_Sum  {
    MD2h    = 646,
    MD5h    = 649,
    SHAh    =  88,
    SHA224h = 417,
    SHA256h = 414,
    SHA384h = 415,
    SHA512h = 416
};
#endif

int wc_HashGetOID(enum wc_HashType hash_type)
{
    int oid = HASH_TYPE_E; /* Default to hash type error */
    switch(hash_type)
    {
        case WC_HASH_TYPE_MD2:
        #ifdef WOLFSSL_MD2
            oid = MD2h;
        #endif
            break;
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
            oid = MD5h;
        #endif
            break;
        case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
            oid = SHAh;
        #endif
            break;
        case WC_HASH_TYPE_SHA224:
        #if defined(WOLFSSL_SHA224)
            oid = SHA224h;
        #endif
            break;
        case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
            oid = SHA256h;
        #endif
            break;
        case WC_HASH_TYPE_SHA384:
        #if defined(WOLFSSL_SHA512) && defined(WOLFSSL_SHA384)
            oid = SHA384h;
        #endif
            break;
        case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
            oid = SHA512h;
        #endif
            break;

        /* Not Supported */
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            oid = BAD_FUNC_ARG;
            break;
    }
    return oid;
}
#endif

/* Get Hash digest size */
int wc_HashGetDigestSize(enum wc_HashType hash_type)
{
    int dig_size = HASH_TYPE_E; /* Default to hash type error */
    switch(hash_type)
    {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            dig_size = MD5_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            dig_size = SHA_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            dig_size = SHA224_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            dig_size = SHA256_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#if defined(WOLFSSL_SHA512) && defined(WOLFSSL_SHA384)
            dig_size = SHA384_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            dig_size = SHA512_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_MD5_SHA:
#if !defined(NO_MD5) && !defined(NO_SHA)
            dig_size = MD5_DIGEST_SIZE + SHA_DIGEST_SIZE;
#endif
            break;

        /* Not Supported */
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            dig_size = BAD_FUNC_ARG;
            break;
    }
    return dig_size;
}

/* Generic Hashing Wrapper */
int wc_Hash(enum wc_HashType hash_type, const byte* data,
    word32 data_len, byte* hash, word32 hash_len)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */
    word32 dig_size;

    /* Validate hash buffer size */
    dig_size = wc_HashGetDigestSize(hash_type);
    if (hash_len < dig_size) {
        return BUFFER_E;
    }

    /* Suppress possible unused arg if all hashing is disabled */
    (void)data;
    (void)data_len;
    (void)hash;
    (void)hash_len;

    switch(hash_type)
    {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_Md5Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaHash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#if defined(WOLFSSL_SHA512) && defined(WOLFSSL_SHA384)
            ret = wc_Sha384Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_MD5_SHA:
#if !defined(NO_MD5) && !defined(NO_SHA)
            ret = wc_Md5Hash(data, data_len, hash);
            if (ret == 0) {
                ret = wc_ShaHash(data, data_len, &hash[MD5_DIGEST_SIZE]);
            }
#endif
            break;

        /* Not Supported */
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
            break;
    }
    return ret;
}

int wc_HashInit(wc_HashAlg* hash, enum wc_HashType type)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            wc_InitMd5(&hash->md5);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_InitSha(&hash->sha);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_InitSha224(&hash->sha224);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_InitSha256(&hash->sha256);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_InitSha384(&hash->sha384);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_InitSha512(&hash->sha512);
#endif
            break;

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}

int wc_HashUpdate(wc_HashAlg* hash, enum wc_HashType type, const byte* data,
                  word32 dataSz)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL || data == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            wc_Md5Update(&hash->md5, data, dataSz);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaUpdate(&hash->sha, data, dataSz);
            if (ret != 0)
                return ret;
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Update(&hash->sha224, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Update(&hash->sha256, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384Update(&hash->sha384, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Update(&hash->sha512, data, dataSz);
#endif
            break;

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}

int wc_HashFinal(wc_HashAlg* hash, enum wc_HashType type, byte* out)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL || out == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            wc_Md5Final(&hash->md5, out);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaFinal(&hash->sha, out);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Final(&hash->sha224, out);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Final(&hash->sha256, out);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384Final(&hash->sha384, out);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Final(&hash->sha512, out);
#endif
            break;

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}


#if !defined(WOLFSSL_TI_HASH)

#if !defined(NO_MD5)
    int wc_Md5Hash(const byte* data, word32 len, byte* hash)
    {
        int ret;
    #ifdef WOLFSSL_SMALL_STACK
        Md5* md5;
    #else
        Md5  md5[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        md5 = (Md5*)XMALLOC(sizeof(Md5), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (md5 == NULL)
            return MEMORY_E;
    #endif

        ret = wc_InitMd5(md5);
        if (ret == 0) {
            ret = wc_Md5Update(md5, data, len);
            if (ret == 0) {
                ret = wc_Md5Final(md5, hash);
            }
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(md5, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_MD5 */

#if !defined(NO_SHA)
    int wc_ShaHash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        Sha* sha;
    #else
        Sha sha[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha = (Sha*)XMALLOC(sizeof(Sha), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sha == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha(sha)) != 0) {
            WOLFSSL_MSG("wc_InitSha failed");
        }
        else {
            wc_ShaUpdate(sha, data, len);
            wc_ShaFinal(sha, hash);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_SHA */

#if defined(WOLFSSL_SHA224)
int wc_Sha224Hash(const byte* data, word32 len, byte* hash)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    Sha224* sha224;
#else
    Sha224 sha224[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
    sha224 = (Sha224*)XMALLOC(sizeof(Sha224), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (sha224 == NULL)
        return MEMORY_E;
#endif

    if ((ret = wc_InitSha224(sha224)) != 0) {
        WOLFSSL_MSG("InitSha224 failed");
    }
    else if ((ret = wc_Sha224Update(sha224, data, len)) != 0) {
        WOLFSSL_MSG("Sha224Update failed");
    }
    else if ((ret = wc_Sha224Final(sha224, hash)) != 0) {
        WOLFSSL_MSG("Sha224Final failed");
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sha224, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}
#endif /* WOLFSSL_SHA224 */

#if !defined(NO_SHA256)
    int wc_Sha256Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        Sha256* sha256;
    #else
        Sha256 sha256[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha256 = (Sha256*)XMALLOC(sizeof(Sha256), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sha256 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha256(sha256)) != 0) {
            WOLFSSL_MSG("InitSha256 failed");
        }
        else if ((ret = wc_Sha256Update(sha256, data, len)) != 0) {
            WOLFSSL_MSG("Sha256Update failed");
        }
        else if ((ret = wc_Sha256Final(sha256, hash)) != 0) {
            WOLFSSL_MSG("Sha256Final failed");
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha256, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_SHA256 */

#endif /* !defined(WOLFSSL_TI_HASH) */


#if defined(WOLFSSL_SHA512)
    int wc_Sha512Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        Sha512* sha512;
    #else
        Sha512 sha512[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha512 = (Sha512*)XMALLOC(sizeof(Sha512), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sha512 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha512(sha512)) != 0) {
            WOLFSSL_MSG("InitSha512 failed");
        }
        else if ((ret = wc_Sha512Update(sha512, data, len)) != 0) {
            WOLFSSL_MSG("Sha512Update failed");
        }
        else if ((ret = wc_Sha512Final(sha512, hash)) != 0) {
            WOLFSSL_MSG("Sha512Final failed");
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha512, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }

    #if defined(WOLFSSL_SHA384)
        int wc_Sha384Hash(const byte* data, word32 len, byte* hash)
        {
            int ret = 0;
        #ifdef WOLFSSL_SMALL_STACK
            Sha384* sha384;
        #else
            Sha384 sha384[1];
        #endif

        #ifdef WOLFSSL_SMALL_STACK
            sha384 = (Sha384*)XMALLOC(sizeof(Sha384), NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (sha384 == NULL)
                return MEMORY_E;
        #endif

            if ((ret = wc_InitSha384(sha384)) != 0) {
                WOLFSSL_MSG("InitSha384 failed");
            }
            else if ((ret = wc_Sha384Update(sha384, data, len)) != 0) {
                WOLFSSL_MSG("Sha384Update failed");
            }
            else if ((ret = wc_Sha384Final(sha384, hash)) != 0) {
                WOLFSSL_MSG("Sha384Final failed");
            }

        #ifdef WOLFSSL_SMALL_STACK
            XFREE(sha384, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        #endif

            return ret;
        }
    #endif /* WOLFSSL_SHA384 */
#endif /* WOLFSSL_SHA512 */
