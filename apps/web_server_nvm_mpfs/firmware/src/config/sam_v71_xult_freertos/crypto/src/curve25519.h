/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    curve25519.h

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


#ifndef WOLF_CRYPT_CURVE25519_H
#define WOLF_CRYPT_CURVE25519_H

#include "crypto/src/types.h"

#ifdef HAVE_CURVE25519

#include "crypto/src/fe_operations.h"
#include "crypto/src/random.h"

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#define CURVE25519_KEYSIZE 32

/* curve25519 set type */
typedef struct {
    int size;       /* The size of the curve in octets */
    const char* name;     /* name of this curve */
} curve25519_set_type;


/* ECC point, the internal structure is Little endian
 * the mathematical functions used the endianess */
typedef struct {
    byte point[CURVE25519_KEYSIZE];
    #ifdef FREESCALE_LTC_ECC
        byte pointY[CURVE25519_KEYSIZE];
    #endif
} ECPoint;

/* A CURVE25519 Key */
typedef struct curve25519_key {
    int idx;            /* Index into the ecc_sets[] for the parameters of
                           this curve if -1, this key is using user supplied
                           curve in dp */
    const curve25519_set_type* dp;   /* domain parameters, either points to
                                   curves (idx >= 0) or user supplied */
    ECPoint   p;        /* public key  */
    ECPoint   k;        /* private key */

#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
} curve25519_key;

enum {
    EC25519_LITTLE_ENDIAN=0,
    EC25519_BIG_ENDIAN=1
};

WOLFSSL_API
int wc_curve25519_make_key(WC_RNG* rng, int keysize, curve25519_key* key);

WOLFSSL_API
int wc_curve25519_shared_secret(curve25519_key* private_key,
                                curve25519_key* public_key,
                                byte* out, word32* outlen);

WOLFSSL_API
int wc_curve25519_shared_secret_ex(curve25519_key* private_key,
                                   curve25519_key* public_key,
                                   byte* out, word32* outlen, int endian);

WOLFSSL_API
int wc_curve25519_init(curve25519_key* key);

WOLFSSL_API
void wc_curve25519_free(curve25519_key* key);


/* raw key helpers */
WOLFSSL_API
int wc_curve25519_import_private(const byte* priv, word32 privSz,
                                 curve25519_key* key);
WOLFSSL_API
int wc_curve25519_import_private_ex(const byte* priv, word32 privSz,
                                    curve25519_key* key, int endian);

WOLFSSL_API
int wc_curve25519_import_private_raw(const byte* priv, word32 privSz,
                            const byte* pub, word32 pubSz, curve25519_key* key);
WOLFSSL_API
int wc_curve25519_import_private_raw_ex(const byte* priv, word32 privSz,
                                        const byte* pub, word32 pubSz,
                                        curve25519_key* key, int endian);
WOLFSSL_API
int wc_curve25519_export_private_raw(curve25519_key* key, byte* out,
                                     word32* outLen);
WOLFSSL_API
int wc_curve25519_export_private_raw_ex(curve25519_key* key, byte* out,
                                        word32* outLen, int endian);

WOLFSSL_API
int wc_curve25519_import_public(const byte* in, word32 inLen,
                                curve25519_key* key);
WOLFSSL_API
int wc_curve25519_import_public_ex(const byte* in, word32 inLen,
                                   curve25519_key* key, int endian);

WOLFSSL_API
int wc_curve25519_export_public(curve25519_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_curve25519_export_public_ex(curve25519_key* key, byte* out,
                                   word32* outLen, int endian);

WOLFSSL_API
int wc_curve25519_export_key_raw(curve25519_key* key,
                                 byte* priv, word32 *privSz,
                                 byte* pub, word32 *pubSz);
WOLFSSL_API
int wc_curve25519_export_key_raw_ex(curve25519_key* key,
                                    byte* priv, word32 *privSz,
                                    byte* pub, word32 *pubSz,
                                    int endian);
/* size helper */
WOLFSSL_API
int wc_curve25519_size(curve25519_key* key);

#ifdef __cplusplus
    }    /* extern "C" */
#endif

#endif /* HAVE_CURVE25519 */
#endif /* WOLF_CRYPT_CURVE25519_H */

