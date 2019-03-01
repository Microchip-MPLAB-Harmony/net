/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    poly1305.h

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


#ifndef WOLF_CRYPT_POLY1305_H
#define WOLF_CRYPT_POLY1305_H

#include "crypto/src/types.h"

#ifdef HAVE_POLY1305

#ifdef __cplusplus
    extern "C" {
#endif

/* auto detect between 32bit / 64bit */
#if defined(__SIZEOF_INT128__) && defined(__LP64__)
#define WC_HAS_SIZEOF_INT128_64BIT
#endif

#if defined(_MSC_VER) && defined(_M_X64)
#define WC_HAS_MSVC_64BIT
#endif

#if (defined(__GNUC__) && defined(__LP64__) && \
        ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 4))))
#define WC_HAS_GCC_4_4_64BIT
#endif

#ifdef USE_INTEL_SPEEDUP
#elif (defined(WC_HAS_SIZEOF_INT128_64BIT) || defined(WC_HAS_MSVC_64BIT) ||  \
       defined(WC_HAS_GCC_4_4_64BIT))
#define POLY130564
#else
#define POLY130532
#endif

enum {
    POLY1305 = 7,
    POLY1305_BLOCK_SIZE = 16,
    POLY1305_DIGEST_SIZE = 16,
};

#define WC_POLY1305_PAD_SZ 16
#define WC_POLY1305_MAC_SZ 16

/* Poly1305 state */
typedef struct Poly1305 {
#ifdef USE_INTEL_SPEEDUP
    word64 r[3];
    word64 h[3];
    word64 pad[2];
    word64 hh[20];
    word32 r1[8];
    word32 r2[8];
    word32 r3[8];
    word32 r4[8];
    word64 hm[16];
    unsigned char buffer[8*POLY1305_BLOCK_SIZE];
    size_t leftover;
    unsigned char finished;
    unsigned char started;
#else
#if defined(POLY130564)
    word64 r[3];
    word64 h[3];
    word64 pad[2];
#else
    word32 r[5];
    word32 h[5];
    word32 pad[4];
#endif
    size_t leftover;
    unsigned char buffer[POLY1305_BLOCK_SIZE];
    unsigned char finished;
#endif
} Poly1305;

/* does init */

WOLFSSL_API int wc_Poly1305SetKey(Poly1305* poly1305, const byte* key,
                                  word32 kySz);
WOLFSSL_API int wc_Poly1305Update(Poly1305* poly1305, const byte*, word32);
WOLFSSL_API int wc_Poly1305Final(Poly1305* poly1305, byte* tag);
WOLFSSL_API int wc_Poly1305_MAC(Poly1305* ctx, byte* additional, word32 addSz,
                               byte* input, word32 sz, byte* tag, word32 tagSz);
#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_POLY1305 */
#endif /* WOLF_CRYPT_POLY1305_H */

