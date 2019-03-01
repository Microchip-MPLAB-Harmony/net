/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    chacha20_poly1305.c
  
  Summary:
    Crypto Framework Library source for cryptographic functions.

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
    #include <config.h>
#endif

#include "crypto/src/settings.h"

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)

#include "crypto/src/chacha20_poly1305.h"
#include "crypto/src/error-crypt.h"
#include "crypto/src/logging.h"
#include "crypto/src/chacha.h"
#include "crypto/src/poly1305.h"

#ifdef NO_INLINE
#include "crypto/src/misc.h"
#else
#define WOLFSSL_MISC_INCLUDED
#include "crypto/src/misc.c"
#endif

#ifdef CHACHA_AEAD_TEST
#include <stdio.h>
#endif

#define CHACHA20_POLY1305_AEAD_INITIAL_COUNTER  0
#define CHACHA20_POLY1305_MAC_PADDING_ALIGNMENT 16

static void word32ToLittle64(const word32 inLittle32, byte outLittle64[8]);
static int calculateAuthTag(
                  const byte inAuthKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                  const byte* inAAD, const word32 inAADLen,
                  const byte *inCiphertext, const word32 inCiphertextLen,
                  byte outAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE]);

int wc_ChaCha20Poly1305_Encrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inPlaintext, const word32 inPlaintextLen,
                byte* outCiphertext,
                byte outAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE])
{
    int err;
    byte poly1305Key[CHACHA20_POLY1305_AEAD_KEYSIZE];
    ChaCha chaChaCtx;

    /* Validate function arguments */

    if (!inKey || !inIV ||
        !inPlaintext || !inPlaintextLen ||
        !outCiphertext ||
        !outAuthTag)
    {
        return BAD_FUNC_ARG;
    }

    XMEMSET(poly1305Key, 0, sizeof(poly1305Key));

    /* Create the Poly1305 key */
    err = wc_Chacha_SetKey(&chaChaCtx, inKey, CHACHA20_POLY1305_AEAD_KEYSIZE);
    if (err != 0) return err;

    err = wc_Chacha_SetIV(&chaChaCtx, inIV,
                           CHACHA20_POLY1305_AEAD_INITIAL_COUNTER);
    if (err != 0) return err;

    err = wc_Chacha_Process(&chaChaCtx, poly1305Key, poly1305Key,
                             CHACHA20_POLY1305_AEAD_KEYSIZE);
    if (err != 0) return err;

    /* Encrypt the plaintext using ChaCha20 */
    err = wc_Chacha_Process(&chaChaCtx, outCiphertext, inPlaintext,
                            inPlaintextLen);
    /* Calculate the Poly1305 auth tag */
    if (err == 0)
        err = calculateAuthTag(poly1305Key,
                               inAAD, inAADLen,
                               outCiphertext, inPlaintextLen,
                               outAuthTag);
    ForceZero(poly1305Key, sizeof(poly1305Key));

    return err;
}


int wc_ChaCha20Poly1305_Decrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inCiphertext, const word32 inCiphertextLen,
                const byte inAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE],
                byte* outPlaintext)
{
    int err;
    byte poly1305Key[CHACHA20_POLY1305_AEAD_KEYSIZE];
    ChaCha chaChaCtx;
    byte calculatedAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE];

    /* Validate function arguments */

    if (!inKey || !inIV ||
        !inCiphertext || !inCiphertextLen ||
        !inAuthTag ||
        !outPlaintext)
    {
        return BAD_FUNC_ARG;
    }

    XMEMSET(calculatedAuthTag, 0, sizeof(calculatedAuthTag));
    XMEMSET(poly1305Key, 0, sizeof(poly1305Key));

    /* Create the Poly1305 key */
    err = wc_Chacha_SetKey(&chaChaCtx, inKey, CHACHA20_POLY1305_AEAD_KEYSIZE);
    if (err != 0) return err;

    err = wc_Chacha_SetIV(&chaChaCtx, inIV,
                           CHACHA20_POLY1305_AEAD_INITIAL_COUNTER);
    if (err != 0) return err;

    err = wc_Chacha_Process(&chaChaCtx, poly1305Key, poly1305Key,
                             CHACHA20_POLY1305_AEAD_KEYSIZE);
    if (err != 0) return err;

    /* Calculate the Poly1305 auth tag */
    err = calculateAuthTag(poly1305Key,
                           inAAD, inAADLen,
                           inCiphertext, inCiphertextLen,
                           calculatedAuthTag);

    /* Compare the calculated auth tag with the received one */
    if (err == 0 && ConstantCompare(inAuthTag, calculatedAuthTag,
                                    CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE) != 0)
    {
        err = MAC_CMP_FAILED_E;
    }

    /* Decrypt the received ciphertext */
    if (err == 0)
        err = wc_Chacha_Process(&chaChaCtx, outPlaintext, inCiphertext,
                                inCiphertextLen);
    ForceZero(poly1305Key, sizeof(poly1305Key));

    return err;
}


static int calculateAuthTag(
                const byte inAuthKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte *inAAD, const word32 inAADLen,
                const byte *inCiphertext, const word32 inCiphertextLen,
                 byte outAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE])
{
    int err;
    Poly1305 poly1305Ctx;
    byte padding[CHACHA20_POLY1305_MAC_PADDING_ALIGNMENT - 1];
    word32 paddingLen;
    byte little64[16];

    XMEMSET(padding, 0, sizeof(padding));

    /* Initialize Poly1305 */
    err = wc_Poly1305SetKey(&poly1305Ctx, inAuthKey,
                            CHACHA20_POLY1305_AEAD_KEYSIZE);
    if (err)
        return err;

    /* Create the authTag by MAC'ing the following items: */
    /* -- AAD */
    if (inAAD && inAADLen)
    {
        err = wc_Poly1305Update(&poly1305Ctx, inAAD, inAADLen);
        /* -- padding1: pad the AAD to 16 bytes */
        paddingLen = -(int)inAADLen &
                                  (CHACHA20_POLY1305_MAC_PADDING_ALIGNMENT - 1);
        if (paddingLen)
            err += wc_Poly1305Update(&poly1305Ctx, padding, paddingLen);

        if (err)
            return err;
    }

    /* -- Ciphertext */
    err = wc_Poly1305Update(&poly1305Ctx, inCiphertext, inCiphertextLen);
    if (err)
        return err;

    /* -- padding2: pad the ciphertext to 16 bytes */
    paddingLen = -(int)inCiphertextLen &
                                  (CHACHA20_POLY1305_MAC_PADDING_ALIGNMENT - 1);
    if (paddingLen)
    {
        err = wc_Poly1305Update(&poly1305Ctx, padding, paddingLen);
        if (err)
            return err;
    }

    /* -- AAD length as a 64-bit little endian integer */
    word32ToLittle64(inAADLen, little64);
    /* -- Ciphertext length as a 64-bit little endian integer */
    word32ToLittle64(inCiphertextLen, little64 + 8);
    err = wc_Poly1305Update(&poly1305Ctx, little64, sizeof(little64));
    if (err)
        return err;

    /* Finalize the auth tag */
    err = wc_Poly1305Final(&poly1305Ctx, outAuthTag);

    return err;
}


static void word32ToLittle64(const word32 inLittle32, byte outLittle64[8])
{
#ifndef WOLFSSL_X86_64_BUILD
    XMEMSET(outLittle64 + 4, 0, 4);

    outLittle64[0] = (byte)(inLittle32 & 0x000000FF);
    outLittle64[1] = (byte)((inLittle32 & 0x0000FF00) >> 8);
    outLittle64[2] = (byte)((inLittle32 & 0x00FF0000) >> 16);
    outLittle64[3] = (byte)((inLittle32 & 0xFF000000) >> 24);
#else
    *(word64*)outLittle64 = inLittle32;
#endif
}


#endif /* HAVE_CHACHA && HAVE_POLY1305 */
