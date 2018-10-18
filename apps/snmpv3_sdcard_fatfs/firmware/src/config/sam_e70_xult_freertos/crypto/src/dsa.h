/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    dsa.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright © 2013 released Microchip Technology Inc.  All rights reserved.

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


#ifndef WOLF_CRYPT_DSA_H
#define WOLF_CRYPT_DSA_H

#include "crypto/src/types.h"

#ifndef NO_DSA

#include "crypto/src/integer.h"
#include "crypto/src/random.h"

/* for DSA reverse compatibility */
#define InitDsaKey wc_InitDsaKey
#define FreeDsaKey wc_FreeDsaKey
#define DsaSign wc_DsaSign
#define DsaVerify wc_DsaVerify
#define DsaPublicKeyDecode wc_DsaPublicKeyDecode
#define DsaPrivateKeyDecode wc_DsaPrivateKeyDecode
#define DsaKeyToDer wc_DsaKeyToDer

#ifdef __cplusplus
    extern "C" {
#endif


enum {
    DSA_PUBLIC   = 0,
    DSA_PRIVATE  = 1
};

/* DSA */
typedef struct DsaKey {
    mp_int p, q, g, y, x;
    int   type;                               /* public or private */
    void* heap;                               /* memory hint */
} DsaKey;

WOLFSSL_API int wc_InitDsaKey(DsaKey* key);
WOLFSSL_API int wc_InitDsaKey_h(DsaKey* key, void* h);
WOLFSSL_API void wc_FreeDsaKey(DsaKey* key);
WOLFSSL_API int wc_DsaSign(const byte* digest, byte* out,
                           DsaKey* key, WC_RNG* rng);
WOLFSSL_API int wc_DsaVerify(const byte* digest, const byte* sig,
                             DsaKey* key, int* answer);
WOLFSSL_API int wc_DsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                      DsaKey*, word32);
WOLFSSL_API int wc_DsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                       DsaKey*, word32);
WOLFSSL_API int wc_DsaKeyToDer(DsaKey* key, byte* output, word32 inLen);

#ifdef WOLFSSL_KEY_GEN
WOLFSSL_API int wc_MakeDsaKey(WC_RNG *rng, DsaKey *dsa);
WOLFSSL_API int wc_MakeDsaParameters(WC_RNG *rng, int modulus_size, DsaKey *dsa);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_DSA */
#endif /* WOLF_CRYPT_DSA_H */

