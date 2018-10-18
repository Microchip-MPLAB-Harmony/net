/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    ed25519.h
  
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


#ifndef WOLF_CRYPT_ED25519_H
#define WOLF_CRYPT_ED25519_H

#include "crypto/src/types.h"

#ifdef HAVE_ED25519

#include "crypto/src/fe_operations.h"
#include "crypto/src/ge_operations.h"
#include "crypto/src/random.h"
#include "crypto/src/sha512.h"

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif


/* info about EdDSA curve specifically ed25519, defined as an elliptic curve
   over GF(p) */
/*
    32,                key size
    "ED25519",         curve name
    "2^255-19",        prime number
    "SHA512",          hash function
    "-121665/121666",  value of d
*/

#define ED25519_KEY_SIZE     32 /* private key only */
#define ED25519_SIG_SIZE     64

#define ED25519_PUB_KEY_SIZE 32 /* compressed */
/* both private and public key */
#define ED25519_PRV_KEY_SIZE (ED25519_PUB_KEY_SIZE+ED25519_KEY_SIZE)


#ifndef WC_ED25519KEY_TYPE_DEFINED
    typedef struct ed25519_key ed25519_key;
    #define WC_ED25519KEY_TYPE_DEFINED
#endif

/* An ED25519 Key */
struct ed25519_key {
    byte    p[ED25519_PUB_KEY_SIZE]; /* compressed public key */
    byte    k[ED25519_PRV_KEY_SIZE]; /* private key : 32 secret -- 32 public */
#ifdef FREESCALE_LTC_ECC
    /* uncompressed point coordinates */
    byte pointX[ED25519_KEY_SIZE]; /* recovered X coordinate */
    byte pointY[ED25519_KEY_SIZE]; /* Y coordinate is the public key with The most significant bit of the final octet always zero. */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
};


WOLFSSL_API
int wc_ed25519_make_key(WC_RNG* rng, int keysize, ed25519_key* key);
WOLFSSL_API
int wc_ed25519_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed25519_key* key);
WOLFSSL_API
int wc_ed25519_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                          word32 msglen, int* stat, ed25519_key* key);
WOLFSSL_API
int wc_ed25519_init(ed25519_key* key);
WOLFSSL_API
void wc_ed25519_free(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_import_public(const byte* in, word32 inLen, ed25519_key* key);
WOLFSSL_API
int wc_ed25519_import_private_only(const byte* priv, word32 privSz,
                                                              ed25519_key* key);
WOLFSSL_API
int wc_ed25519_import_private_key(const byte* priv, word32 privSz,
                               const byte* pub, word32 pubSz, ed25519_key* key);
WOLFSSL_API
int wc_ed25519_export_public(ed25519_key*, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed25519_export_private_only(ed25519_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed25519_export_private(ed25519_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed25519_export_key(ed25519_key* key,
                          byte* priv, word32 *privSz,
                          byte* pub, word32 *pubSz);

int wc_ed25519_check_key(ed25519_key* key);

/* size helper */
WOLFSSL_API
int wc_ed25519_size(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_priv_size(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_pub_size(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_sig_size(ed25519_key* key);

#ifdef __cplusplus
    }    /* extern "C" */
#endif

#endif /* HAVE_ED25519 */
#endif /* WOLF_CRYPT_ED25519_H */

