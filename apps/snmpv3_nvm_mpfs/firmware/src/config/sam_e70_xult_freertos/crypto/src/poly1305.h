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

#if (defined(WC_HAS_SIZEOF_INT128_64BIT) || defined(WC_HAS_MSVC_64BIT) ||  \
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
	unsigned char final;
} Poly1305;


/* does init */

WOLFSSL_API int wc_Poly1305SetKey(Poly1305* poly1305, const byte* key, word32 kySz);
WOLFSSL_API int wc_Poly1305Update(Poly1305* poly1305, const byte*, word32);
WOLFSSL_API int wc_Poly1305Final(Poly1305* poly1305, byte* tag);
WOLFSSL_API int wc_Poly1305_MAC(Poly1305* ctx, byte* additional, word32 addSz,
                               byte* input, word32 sz, byte* tag, word32 tagSz);
#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_POLY1305 */
#endif /* WOLF_CRYPT_POLY1305_H */

