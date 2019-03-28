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

/* raw export functions */
WOLFSSL_API int wc_DsaImportParamsRaw(DsaKey* dsa, const char* p,
                                      const char* q, const char* g);
WOLFSSL_API int wc_DsaImportParamsRawCheck(DsaKey* dsa, const char* p,
                                      const char* q, const char* g,
                                      int trusted, WC_RNG* rng);
WOLFSSL_API int wc_DsaExportParamsRaw(DsaKey* dsa, byte* p, word32* pSz,
                                      byte* q, word32* qSz, byte* g,
                                      word32* gSz);
WOLFSSL_API int wc_DsaExportKeyRaw(DsaKey* dsa, byte* x, word32* xSz, byte* y,
                                   word32* ySz);
#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_DSA */
#endif /* WOLF_CRYPT_DSA_H */

