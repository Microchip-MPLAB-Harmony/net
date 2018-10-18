/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    idea.h

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


#ifndef WOLF_CRYPT_IDEA_H
#define WOLF_CRYPT_IDEA_H

#include "crypto/src/types.h"

#ifdef HAVE_IDEA

#ifdef __cplusplus
    extern "C" {
#endif

enum {
    IDEA_MODULO     = 0x10001,             /* 2^16+1 */
    IDEA_2EXP16     = 0x10000,             /* 2^16 */
    IDEA_MASK       = 0xFFFF,              /* 16 bits set to one */
    IDEA_ROUNDS     = 8,                   /* number of rounds for IDEA */
    IDEA_SK_NUM     = (6*IDEA_ROUNDS + 4), /* number of subkeys */
    IDEA_KEY_SIZE   = 16,                  /* size of key in bytes */
    IDEA_BLOCK_SIZE = 8,                   /* size of IDEA blocks in bytes */
    IDEA_IV_SIZE    = 8,                   /* size of IDEA IV in bytes */
    IDEA_ENCRYPTION = 0,
    IDEA_DECRYPTION = 1
};

/* IDEA encryption and decryption */
typedef struct Idea {
    word32  reg[IDEA_BLOCK_SIZE / sizeof(word32)]; /* for CBC mode */
    word32  tmp[IDEA_BLOCK_SIZE / sizeof(word32)]; /* for CBC mode */
    word16  skey[IDEA_SK_NUM]; /* 832 bits expanded key */
} Idea;

WOLFSSL_API int wc_IdeaSetKey(Idea *idea, const byte* key, word16 keySz,
                              const byte *iv, int dir);
WOLFSSL_API int wc_IdeaSetIV(Idea *idea, const byte* iv);
WOLFSSL_API int wc_IdeaCipher(Idea *idea, byte* out, const byte* in);
WOLFSSL_API int wc_IdeaCbcEncrypt(Idea *idea, byte* out,
                                  const byte* in, word32 len);
WOLFSSL_API int wc_IdeaCbcDecrypt(Idea *idea, byte* out,
                                  const byte* in, word32 len);
#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_IDEA */
#endif /* WOLF_CRYPT_IDEA_H */
