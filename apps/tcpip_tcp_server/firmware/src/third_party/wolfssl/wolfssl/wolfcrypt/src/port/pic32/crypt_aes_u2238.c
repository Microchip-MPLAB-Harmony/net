/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    aes.c

  Summary:
    Crypto Framework Library source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2017-2019 Microchip Technology Inc. and its subsidiaries.

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
    #include "config.h"
#endif
#include "configuration.h"
#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO) && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_AES_HW_U2238) && defined(WOLFSSL_HAVE_MCHP_HW_AES_DIRECT)

#include "wolfssl/wolfcrypt/settings.h"


#ifndef NO_AES
#include <stdint.h>

#include "device.h" /* SAME70 system header. */
#include "wolfssl/wolfcrypt/aes.h"

#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/logging.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_aes_u2238.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_aes_hwInt.h"

#undef COMPONENT_TYPEDEF_STYLE
#define COMPONENT_TYPEDEF_STYLE 'N'

/* helper to compose the configuration for the AES */

static void CRYPT_AES_u2238LoadKey(const Aes * aes, uint32_t * key)
{
    uint32_t words = 4;
    switch (aes->hwDesc.ctrla.s.KEYSIZE)
    {
        case CRYPT_AES_U2238_AES192:
            words = 6;
            break;
        case CRYPT_AES_U2238_AES256:
            words = 8;
            break;  
        default:
            break;
    }
    
    int x = 0;
    for (x = 0; x < words; x++)
    {
        AES_REGS->AES_KEYWORD[x] = key[x];
    }    
}

void CRYPT_AES_u2238LoadIV(uint32_t *iv)
{
    AES_REGS->AES_INTVECTV[0] = iv[0];
    AES_REGS->AES_INTVECTV[1] = iv[1];
    AES_REGS->AES_INTVECTV[2] = iv[2];
    AES_REGS->AES_INTVECTV[3] = iv[3];
    
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    ctrlb->s.NEWMSG = 1;
}

static void CRYPT_AES_u2238InOutBlock(const uint8_t * in, uint8_t * out)
{
    //volatile CRYPT_AES_U2238_AES_INTFLAG * intFlag = (CRYPT_AES_U2238_AES_INTFLAG *)(&AES_REGS->AES_INTFLAG);
    CRYPT_AES_U2238_AES_CTRLB * volatile ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    uint32_t * ptr = NULL;
    if (in == NULL)
    {
        AES_REGS->AES_INDATA = 0;
        AES_REGS->AES_INDATA = 0;
        AES_REGS->AES_INDATA = 0;
        AES_REGS->AES_INDATA = 0;
    }
    else
    {
        ptr = (uint32_t *)in;
        AES_REGS->AES_INDATA = ptr[0];
        AES_REGS->AES_INDATA = ptr[1];
        AES_REGS->AES_INDATA = ptr[2];
        AES_REGS->AES_INDATA = ptr[3];
    }
    
    ctrlb->s.START = 1;
    while ((AES_REGS->AES_INTFLAG & 0x1) == 0);

    if (out == NULL)
    {
        uint32_t tmp = AES_REGS->AES_INDATA;
        tmp = AES_REGS->AES_INDATA;
        tmp = AES_REGS->AES_INDATA;
        tmp = AES_REGS->AES_INDATA;
        tmp = tmp;
    }
    else
    {
        ptr = (uint32_t *)out;
        ptr[0] = AES_REGS->AES_INDATA;
        ptr[1] = AES_REGS->AES_INDATA;
        ptr[2] = AES_REGS->AES_INDATA;
        ptr[3] = AES_REGS->AES_INDATA;
    }   
}


int wc_AesEncrypt(Aes* aes, const byte* inBlock, byte* outBlock)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.AESMODE = CRYPT_AES_U2238_ECB;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    
    uint8_t * outPtr = outBlock;
    const uint8_t * inPtr = inBlock;
    
    CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
    return 0;
    
}

#if defined(HAVE_AES_DECRYPT)

int wc_AesDecrypt(Aes* aes, const byte* inBlock, byte* outBlock)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.AESMODE = CRYPT_AES_U2238_ECB;
    ctrla->s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    
    uint8_t * outPtr = outBlock;
    const uint8_t * inPtr = inBlock;
    
    CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
    return 0;
    
}

#endif

#if defined(WOLFSSL_HAVE_MCHP_HW_AES_ECB) && defined(HAVE_AES_ECB)

int wc_AesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.AESMODE = CRYPT_AES_U2238_ECB;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    
    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    return 0;
}
#if defined(HAVE_AES_DECRYPT)

int wc_AesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);

    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;    
    ctrla->s.AESMODE = CRYPT_AES_U2238_ECB;
    ctrla->s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
    
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    
    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    return 0;
}
#endif


#endif

#if defined(WOLFSSL_HAVE_MCHP_HW_AES_CBC) && defined(HAVE_AES_CBC)

int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.AESMODE = CRYPT_AES_U2238_CBC;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->reg);

    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    memcpy(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    return 0;
}
#if defined(HAVE_AES_DECRYPT)

int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);

    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;    
    ctrla->s.AESMODE = CRYPT_AES_U2238_CBC;
    ctrla->s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
    
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->reg);

    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    memcpy(aes->reg, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    return 0;
}
#endif


#endif

#if defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_HAVE_MCHP_HW_AES_CTR)

int wc_AesCtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.AESMODE = CRYPT_AES_U2238_CTR;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->reg);

    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    memcpy(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    return 0;
}

#endif

#if defined(WOLFSSL_HAVE_MCHP_HW_AES_GCM) && defined(HAVE_AESGCM)  

int  wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len)
{
    memset(aes, 0, sizeof(Aes));
    memcpy(aes->key, key, len);
    aes->keylen = len;
    return 0;
}

static void CRYPT_AES_u2238LoadDataBlock(uint8_t * in, uint8_t gfm)
{
    //volatile CRYPT_AES_U2238_AES_INTFLAG * intFlag = (CRYPT_AES_U2238_AES_INTFLAG *)(&AES_REGS->AES_INTFLAG);
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    
    uint32_t * ptr = (uint32_t *)in;
    AES_REGS->AES_INDATA = ptr[0];
    AES_REGS->AES_INDATA = ptr[1];
    AES_REGS->AES_INDATA = ptr[2];
    AES_REGS->AES_INDATA = ptr[3];
    
    ctrlb->s.START = 1;
    if (gfm == 0)
    {
        while ((AES_REGS->AES_INTFLAG & 0x1) == 0);
    }
    else
    {
        while ((AES_REGS->AES_INTFLAG & 0x2) == 0);
    }
        
}

void CRYPT_AES_GcmLoadKey(Aes* aes)
{
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);

    //Soft reset
    ctrla->s.SWRST = 1;

    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;    
    ctrla->s.AESMODE = CRYPT_AES_U2238_ECB;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    switch (aes->keylen)
    {
        case 16:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES128;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case 24:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES192;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case 32:
            ctrla->s.KEYSIZE = CRYPT_AES_U2238_AES256;
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
    }
            
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;

    uint8_t workBuf[16] = {0};
    CRYPT_AES_u2238LoadDataBlock(workBuf, 0);
    
    ctrla->s.ENABLE = 0;
    ctrla->s.STARTMODE = CRYPT_AES_U2238_MANUAL_START;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrla->s.AESMODE = CRYPT_AES_U2238_GCM;
    ctrla->s.CTYPE = 0xf;
    
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrlb->s.GFMUL = 1;
    ctrla->s.ENABLE = 1;
}

void CRYPT_AES_GcmLoadKeyCalculateJ0(Aes* aes, const uint8_t * iv, uint32_t iv_len)
{
    memset(aes->reg, 0, sizeof(aes->reg));
    if (iv_len == 12)
    {
        
        memcpy(aes->reg, iv, 12);
        
        ((uint8_t*)(aes->reg))[15] = 0x1;
        return;
                
    }    
    uint32_t workBuf[4] = {0};
    CRYPT_AES_u2238LoadDataBlock((uint8_t*)workBuf, 1);
    
    uint32_t * ptr = (uint32_t*)iv;
    int32_t left_len = iv_len;
    while (0 < left_len)
    {
        uint32_t useLength = left_len < 16 ? left_len : 16;
        memset(workBuf, 0, sizeof(workBuf));
        memcpy(workBuf, ptr, useLength);
        CRYPT_AES_u2238LoadDataBlock((uint8_t*)workBuf, 1);
        ptr+=4;
        left_len -= 16;        
    }
    
    memset(workBuf, 0, sizeof(workBuf));
    uint32_t tmpIvLen = iv_len << 3;
    
    ((uint8_t*)workBuf)[12] = ((tmpIvLen) >> 24) & 0xFF;
    ((uint8_t*)workBuf)[13] = ((tmpIvLen) >> 16) & 0xFF;
    ((uint8_t*)workBuf)[14] = ((tmpIvLen) >> 8) & 0xFF;
    ((uint8_t*)workBuf)[15] = (tmpIvLen) & 0xFF;

    CRYPT_AES_u2238LoadDataBlock((uint8_t*)workBuf, 1);
            
    // Grab the GHASH
    for (int x = 0; x < 4; x++)
    {
        ((uint32_t*)(aes->reg))[x] = AES_REGS->AES_GHASH[x];
        AES_REGS->AES_GHASH[x] = 0;
    }    
}

void CRYPT_AES_GCMProcessAAD(Aes* aes, const uint8_t * aad, uint32_t aad_len)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);

    uint32_t * ptr = (uint32_t*)aad;
    int32_t left_len = aad_len;
    uint32_t workBuf[4] = {0};
    
    while (0 < left_len)
    {
        uint32_t useLength = left_len < 16 ? left_len : 16;
        memset(workBuf, 0, sizeof(workBuf));
        memcpy(workBuf, ptr, useLength);
        CRYPT_AES_u2238LoadDataBlock((uint8_t*)workBuf, 1);
        ptr+=4;
        left_len -= 16;
    }
    //Clear the GFMUL
    ctrlb->s.GFMUL = 0;
    aes->invokeCtr[1] = aad_len;
}

void CRYPT_AES_GCMRunBlocks(Aes* aes, const uint8_t * in, uint8_t * out, uint32_t len)
{
    // Set up the counter.
    // Convert from big -> Little
    uint8_t * ivPtr = (uint8_t*)(aes->reg);
    uint32_t ctr = ivPtr[12] << 24 |
                   ivPtr[13] << 16 |
                   ivPtr[14] << 8 |
                   ivPtr[15];
    // Add in the current counter
    ctr = ctr + aes->invokeCtr[0];
    // Convert from little -> big
    ctr = (ctr << 24) | 
         ((ctr << 8) & 0x00ff0000) |
         ((ctr >> 8) & 0xff00) |
         ((ctr >> 24) & 0xff);
    
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_INTFLAG * intFlag = (CRYPT_AES_U2238_AES_INTFLAG *)(&AES_REGS->AES_INTFLAG);
    
    ctrla->s.ENABLE = 1;
    intFlag->s.ENCCMP = 1;
    ctrlb->s.NEWMSG = 1;
    
    AES_REGS->AES_CIPLEN = len;
    
    uint32_t workBuf[4] = {0};
    memcpy(workBuf, aes->reg, 12);
    workBuf[3] = ctr;

    CRYPT_AES_u2238LoadIV(workBuf);
    
    const uint32_t * inPtr = (uint32_t*)in;
    uint32_t * outPtr = (uint32_t*)out;
    uint32_t numBlocks = len >> 4;
    
    aes->invokeCtr[0] += numBlocks;
    
    for (int x = 0; x < numBlocks; x++)
    {
        if (((len & 0xf) == 0) && (x == (numBlocks -1)))
        {
            ctrlb->s.EOM = 1;
        }
        CRYPT_AES_u2238InOutBlock((const uint8_t*)inPtr, (uint8_t*)outPtr);
        if (inPtr != NULL)
        {
            inPtr+=4;
        }
        if (outPtr != NULL)
        {
            outPtr+=4;
        }
    }
    
    if ((len & 0xf) != 0)
    {
        if (inPtr != NULL)
        {
            memcpy(workBuf, inPtr, (len & 0xf));
        }
        ctrlb->s.EOM = 1;
        CRYPT_AES_u2238InOutBlock((const uint8_t*)workBuf, (uint8_t*)workBuf);
        if (outPtr != NULL)
        {
            memcpy(outPtr, workBuf, (len & 0xf));
        }
    }
    
}

static void CRYPT_AES_u2238SaveGHash(const Aes * aes)
{
    for (int x = 0; x < sizeof(AES_REGS->AES_GHASH) >> 2; x++)
    {
        ((uint32_t*)aes->tmp)[x] = AES_REGS->AES_GHASH[x];
    } 
}

void CRYPT_AES_GCMGenerateFinalGHash(Aes* aes, uint32_t len)
{
    uint32_t workBuf[4] = {0};
    
    uint32_t addLen = aes->invokeCtr[1] * 8;
    uint32_t lenBits = len * 8;
    
    uint32_t addLenBE = ((addLen >> 24 ) & 0x000000ff) |
                        ((addLen >> 8 ) & 0x0000ff00) |
                        ((addLen << 8 ) & 0x00ff0000) |
                        ((addLen << 24 ) & 0xff000000);

    uint32_t lenBE = ((lenBits >> 24 ) & 0x000000ff) |
                     ((lenBits >> 8 ) & 0x0000ff00) |
                     ((lenBits << 8 ) & 0x00ff0000) |
                     ((lenBits << 24 ) & 0xff000000);

    workBuf[1] = addLenBE;
    workBuf[3] = lenBE;

    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    ctrlb->s.GFMUL = 1;

    CRYPT_AES_u2238LoadDataBlock((uint8_t*)workBuf, 1);
    
    ctrlb->s.GFMUL = 0;
    ctrla->s.ENABLE = 0;
    CRYPT_AES_u2238SaveGHash(aes);
    
    
}

void CRYPT_AES_GCMGenerateTag(Aes* aes, uint32_t * tag, uint32_t tag_len)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);

    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    ctrla->s.AESMODE = CRYPT_AES_U2238_CTR;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrlb->v = 0;
    
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->key);
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->reg);
    
    ctrlb->s.NEWMSG = 1;
    
    uint8_t workBuf[16] = {0};
    CRYPT_AES_u2238InOutBlock((uint8_t*)aes->tmp, (uint8_t*)workBuf);
    memcpy(tag, workBuf, tag_len);
    
    ctrla->s.SWRST = 1;
    ctrla->v = 0;    
}

  int  wc_AesGcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz)
 {
    aes->invokeCtr[0] = 1;
    CRYPT_AES_GcmLoadKey(aes);
    CRYPT_AES_GcmLoadKeyCalculateJ0(aes, iv, ivSz);
    CRYPT_AES_GCMProcessAAD(aes, authIn, authInSz);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    CRYPT_AES_GCMRunBlocks(aes, in, out, sz);
    CRYPT_AES_GCMGenerateFinalGHash(aes, sz);     
    CRYPT_AES_GCMGenerateTag(aes, (uint32_t*)authTag, authTagSz);
    return 0; 
 }
#if defined(HAVE_AES_DECRYPT)

int  wc_AesGcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz)
{
    aes->invokeCtr[0] = 1;
    CRYPT_AES_GcmLoadKey(aes);
    CRYPT_AES_GcmLoadKeyCalculateJ0(aes, iv, ivSz);
    CRYPT_AES_GCMProcessAAD(aes, authIn, authInSz);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    ctrla->s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    CRYPT_AES_GCMRunBlocks(aes, in, out, sz);
    CRYPT_AES_GCMGenerateFinalGHash(aes, sz);     
    CRYPT_AES_GCMGenerateTag(aes, (uint32_t*)authTag, authTagSz);
    return 0; 
 }
#endif

#endif

#endif /* NO_AES */

#endif
