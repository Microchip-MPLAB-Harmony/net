/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    cmac.c
  
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












#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "crypto/src/settings.h"

#if defined(WOLFSSL_CMAC) && !defined(NO_AES) && defined(WOLFSSL_AES_DIRECT)

#if defined(HAVE_FIPS) && \
	defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)

    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$n")
        #pragma const_seg(".fipsB$n")
    #endif
#endif

#ifdef NO_INLINE
    #include "crypto/src/misc.h"
#else
    #define WOLFSSL_MISC_INCLUDED
    #include "crypto/src/misc.c"
#endif

#include "crypto/src/error-crypt.h"
#include "crypto/src/aes.h"
#include "crypto/src/cmac.h"


static void ShiftAndXorRb(byte* out, byte* in)
{
    int i, j, xorRb;
    int mask = 0, last = 0;
    byte Rb = 0x87;

    xorRb = (in[0] & 0x80) != 0;

    for (i = 1, j = AES_BLOCK_SIZE - 1; i <= AES_BLOCK_SIZE; i++, j--) {
        last = (in[j] & 0x80) ? 1 : 0;
        out[j] = (byte)((in[j] << 1) | mask);
        mask = last;
        if (xorRb) {
            out[j] ^= Rb;
            Rb = 0;
        }
    }
}


int wc_InitCmac(Cmac* cmac, const byte* key, word32 keySz,
                int type, void* unused)
{
    int ret;

    (void)unused;

    if (cmac == NULL || key == NULL || keySz == 0 || type != WC_CMAC_AES)
        return BAD_FUNC_ARG;

    XMEMSET(cmac, 0, sizeof(Cmac));
    ret = wc_AesSetKey(&cmac->aes, key, keySz, NULL, AES_ENCRYPTION);
    if (ret == 0) {
        byte l[AES_BLOCK_SIZE];

        XMEMSET(l, 0, AES_BLOCK_SIZE);
        wc_AesEncryptDirect(&cmac->aes, l, l);
        ShiftAndXorRb(cmac->k1, l);
        ShiftAndXorRb(cmac->k2, cmac->k1);
        ForceZero(l, AES_BLOCK_SIZE);
    }
    return ret;
}


int wc_CmacUpdate(Cmac* cmac, const byte* in, word32 inSz)
{
    if ((cmac == NULL) || (in == NULL && inSz != 0))
        return BAD_FUNC_ARG;

    while (inSz != 0) {
        word32 add = min(inSz, AES_BLOCK_SIZE - cmac->bufferSz);
        XMEMCPY(&cmac->buffer[cmac->bufferSz], in, add);

        cmac->bufferSz += add;
        in += add;
        inSz -= add;

        if (cmac->bufferSz == AES_BLOCK_SIZE && inSz != 0) {
            if (cmac->totalSz != 0)
                xorbuf(cmac->buffer, cmac->digest, AES_BLOCK_SIZE);
            wc_AesEncryptDirect(&cmac->aes,
                                cmac->digest,
                                cmac->buffer);
            cmac->totalSz += AES_BLOCK_SIZE;
            cmac->bufferSz = 0;
        }
    }

    return 0;
}


int wc_CmacFinal(Cmac* cmac, byte* out, word32* outSz)
{
    const byte* subKey;

    if (cmac == NULL || out == NULL || outSz == NULL)
        return BAD_FUNC_ARG;

    if (*outSz < WC_CMAC_TAG_MIN_SZ || *outSz > WC_CMAC_TAG_MAX_SZ)
        return BUFFER_E;

    if (cmac->bufferSz == AES_BLOCK_SIZE) {
        subKey = cmac->k1;
    }
    else {
        word32 remainder = AES_BLOCK_SIZE - cmac->bufferSz;

        if (remainder == 0)
            remainder = AES_BLOCK_SIZE;

        if (remainder > 1)
            XMEMSET(cmac->buffer + AES_BLOCK_SIZE - remainder, 0, remainder);
        cmac->buffer[AES_BLOCK_SIZE - remainder] = 0x80;
        subKey = cmac->k2;
    }
    xorbuf(cmac->buffer, cmac->digest, AES_BLOCK_SIZE);
    xorbuf(cmac->buffer, subKey, AES_BLOCK_SIZE);
    wc_AesEncryptDirect(&cmac->aes, cmac->digest, cmac->buffer);

    XMEMCPY(out, cmac->digest, *outSz);

    ForceZero(cmac, sizeof(Cmac));

    return 0;
}


int wc_AesCmacGenerate(byte* out, word32* outSz,
                       const byte* in, word32 inSz,
                       const byte* key, word32 keySz)
{
    Cmac cmac;
    int ret;

    if (out == NULL || (in == NULL && inSz > 0) || key == NULL || keySz == 0)
        return BAD_FUNC_ARG;

    ret = wc_InitCmac(&cmac, key, keySz, WC_CMAC_AES, NULL);
    if (ret != 0)
        return ret;

    ret = wc_CmacUpdate(&cmac, in, inSz);
    if (ret != 0)
        return ret;

    ret = wc_CmacFinal(&cmac, out, outSz);
    if (ret != 0)
        return ret;

    return 0;
}


int wc_AesCmacVerify(const byte* check, word32 checkSz,
                     const byte* in, word32 inSz,
                     const byte* key, word32 keySz)
{
    byte a[AES_BLOCK_SIZE];
    word32 aSz = sizeof(a);
    int result;
    int compareRet;

    if (check == NULL || checkSz == 0 || (in == NULL && inSz != 0) ||
        key == NULL || keySz == 0)

        return BAD_FUNC_ARG;

    XMEMSET(a, 0, aSz);
    result = wc_AesCmacGenerate(a, &aSz, in, inSz, key, keySz);
    compareRet = ConstantCompare(check, a, min(checkSz, aSz));

    if (result == 0)
        result = compareRet ? 1 : 0;

    return result;
}


#endif /* WOLFSSL_CMAC && NO_AES && WOLFSSL_AES_DIRECT */
