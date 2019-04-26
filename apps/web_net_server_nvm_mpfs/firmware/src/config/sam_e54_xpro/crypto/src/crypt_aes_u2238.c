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

#include "crypto/src/settings.h"


#ifndef NO_AES
#include <stdint.h>

#include "device.h" /* SAME70 system header. */
#include "aes.h"

#include "crypto/crypto.h"
#include "crypto/src/error-crypt.h"
#include "crypto/src/logging.h"
#include "crypt_aes_u2238.h"
#include "crypt_aes_hwInt.h"

#undef COMPONENT_TYPEDEF_STYLE
#define COMPONENT_TYPEDEF_STYLE 'N'

static void CRYPT_AES_EnableClock()
{
    uint32_t apbcBit = 1u << (ID_AES&31);
    if ((MCLK_REGS->MCLK_APBCMASK & apbcBit) != apbcBit)
    {
        MCLK_REGS->MCLK_APBCMASK |= apbcBit;
    }
    
}

static void CRYPT_AES_DisableClock()
{
    uint32_t apbcBit = 1u << (ID_AES&31);
    if ((MCLK_REGS->MCLK_APBCMASK & apbcBit) == apbcBit)
    {
        MCLK_REGS->MCLK_APBCMASK ^= apbcBit;
    }
}

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

static void CRYPT_AES_u2238LoadIV(uint32_t *iv)
{
    AES_REGS->AES_INTVECTV[0] = iv[0];
    AES_REGS->AES_INTVECTV[1] = iv[1];
    AES_REGS->AES_INTVECTV[2] = iv[2];
    AES_REGS->AES_INTVECTV[3] = iv[3];
    
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    ctrlb->s.NEWMSG = 1;
}

/*
static void CRYPT_AES_u2238LoadGHash(const Aes * aes)
{
    for (int x = 0; x < sizeof(AES_REGS->AES_GHASH) >> 2; x++)
    {
        AES_REGS->AES_GHASH[x] = ((uint32_t*)aes->hwDesc.aesGHash)[x];
    }
}
*/

static void CRYPT_AES_u2238SaveGHash(const Aes * aes)
{
    for (int x = 0; x < sizeof(AES_REGS->AES_GHASH) >> 2; x++)
    {
        ((uint32_t*)aes->hwDesc.aesGHash)[x] = AES_REGS->AES_GHASH[x];
    } 
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

static void CRYPT_AES_u2238InOutBlock(const uint8_t * in, uint8_t * out)
{
    //volatile CRYPT_AES_U2238_AES_INTFLAG * intFlag = (CRYPT_AES_U2238_AES_INTFLAG *)(&AES_REGS->AES_INTFLAG);
    CRYPT_AES_U2238_AES_CTRLB * volatile ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    
    uint32_t * ptr = (uint32_t *)in;
    AES_REGS->AES_INDATA = ptr[0];
    AES_REGS->AES_INDATA = ptr[1];
    AES_REGS->AES_INDATA = ptr[2];
    AES_REGS->AES_INDATA = ptr[3];
    
    ctrlb->s.START = 1;
    while ((AES_REGS->AES_INTFLAG & 0x1) == 0);

    ptr = (uint32_t *)out;
    ptr[0] = AES_REGS->AES_INDATA;
    ptr[1] = AES_REGS->AES_INDATA;
    ptr[2] = AES_REGS->AES_INDATA;
    ptr[3] = AES_REGS->AES_INDATA;
        
}



/* Start actions */
int CRYPT_AES_SetKey(Aes* aes, const byte* userKey, word32 keylen, const byte* iv, int dir)
{
    memset(aes, 0, sizeof(Aes));
    
    switch (keylen)
    {
        case (128/8):
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES128;
            break;
        case (192/8):
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES192;
            break;
        case (256/8):
            aes->hwDesc.ctrla.s.KEYSIZE = CRYPT_AES_U2238_AES256;
            break;
        default:
            return BAD_FUNC_ARG;  // Invalid Key size            
    }
    aes->hwDesc.ctrla.s.STARTMODE = CRYPT_AES_U2238_AUTO_START;
    if (dir == CRYPT_AES_ENCRYPTION)
    {
        aes->hwDesc.ctrla.s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    }
    else
    {
        aes->hwDesc.ctrla.s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    }
    
    memcpy(aes->hwDesc.aesKey, userKey, keylen < sizeof(aes->hwDesc.aesKey) ? keylen : sizeof(aes->hwDesc.aesKey));
    if (iv != NULL)
    {
        memcpy(aes->hwDesc.aesIv, iv, sizeof(aes->hwDesc.aesIv));
    }

    /* save the particulars about the key and IV */

    return 0;
}

/* AES-CBC */

#ifdef HAVE_AES_ECB

int CRYPT_AES_EcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    CRYPT_AES_EnableClock();
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrla->s.AESMODE = CRYPT_AES_U2238_ECB;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);
    
    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    CRYPT_AES_DisableClock();
    return 0;
}

int CRYPT_AES_EcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);

    CRYPT_AES_EnableClock();
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrla->s.AESMODE = CRYPT_AES_U2238_ECB;
    ctrla->s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);
    
    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    CRYPT_AES_DisableClock();
    return 0;
}

#endif


#ifdef HAVE_AES_CBC


int CRYPT_AES_CbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);

    CRYPT_AES_EnableClock();
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrla->s.AESMODE = CRYPT_AES_U2238_CBC;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->hwDesc.aesIv);
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
    outPtr-=16;
    if (blocks > 0)
    {
        memcpy(aes->hwDesc.aesIv, outPtr, 16);
    }
    CRYPT_AES_DisableClock();

    return 0;
}



#ifdef HAVE_AES_DECRYPT
int CRYPT_AES_CbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);

    CRYPT_AES_EnableClock();
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrla->s.AESMODE = CRYPT_AES_U2238_CBC;
    ctrla->s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->hwDesc.aesIv);

    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }
    outPtr-=16;
    if (blocks > 0)
    {
        memcpy(aes->hwDesc.aesIv, outPtr, 16);
    }
    CRYPT_AES_DisableClock();

    return 0;
}
#endif /* HAVE_AES_DECRYPT */


#endif /* HAVE_AES_CBC */



/* AES-CTR */
#ifdef WOLFSSL_AES_COUNTER

int CRYPT_AES_CtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    volatile CRYPT_AES_U2238_AES_CTRLB * ctrlb = (CRYPT_AES_U2238_AES_CTRLB  *)(&AES_REGS->AES_CTRLB);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);

    CRYPT_AES_EnableClock();
    
    ctrla->s.SWRST = 1;
    ctrla->s.ENABLE = 0;
    ctrla->v = 0;
    ctrlb->v = 0;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrla->s.AESMODE = CRYPT_AES_U2238_CTR;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    ctrla->s.ENABLE = 1;
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);    
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->hwDesc.aesIv);

    uint8_t * outPtr = out;
    const uint8_t * inPtr = in;
    
    uint32_t blocks = sz >> 4;
    for (int x = 0; x < blocks; x++)
    {
        CRYPT_AES_u2238InOutBlock(inPtr, outPtr);
        inPtr+=16;
        outPtr+=16;
    }

    CRYPT_AES_DisableClock();
    return 0;
}


#endif /* WOLFSSL_AES_COUNTER */


void CRYPT_AES_Free(Aes* aes)
{
    
}
int  CRYPT_AES_Init(Aes* aes, void* heap, int devId)
{
    memset(aes, 0, sizeof(Aes));
    return 0;
}
#ifdef HAVE_AESCCM

#ifdef WOLFSSL_PIC32MZ_CRYPT
    #error "PIC32MZ doesn't currently support AES-CCM mode"

#endif
#endif /* HAVE_AESCCM */

#ifdef HAVE_AESGCM



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
    ctrla->s.ENABLE = 1;
            
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);

    uint8_t workBuf[16] = {0};
    CRYPT_AES_u2238LoadDataBlock(workBuf, 0);
    
    ctrla->s.ENABLE = 0;
    ctrla->s.STARTMODE = CRYPT_AES_U2238_MANUAL_START;
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    ctrla->s.KEYSIZE = aes->hwDesc.ctrla.s.KEYSIZE;
    ctrla->s.AESMODE = CRYPT_AES_U2238_GCM;
    ctrla->s.CTYPE = 0xf;
    ctrla->s.ENABLE = 1;
    
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);
    ctrlb->s.GFMUL = 1;
}

void CRYPT_AES_GcmLoadKeyCalculateJ0(Aes* aes, const uint8_t * iv, uint32_t iv_len)
{
    memset(aes->hwDesc.aesIv, 0, sizeof(aes->hwDesc.aesIv));
    if (iv_len == 12)
    {
        
        memcpy(aes->hwDesc.aesIv, iv, 12);
        aes->hwDesc.aesIv[15] = 0x1;
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
    workBuf[12] = ((iv_len << 3) >> 24) & 0xFF;
    workBuf[13] = ((iv_len << 3) >> 16) & 0xFF;
    workBuf[14] = ((iv_len << 3) >> 8) & 0xFF;
    workBuf[15] = (iv_len << 3) & 0xFF;

    CRYPT_AES_u2238LoadDataBlock((uint8_t*)workBuf, 1);
            
    // Grab the GHASH
    for (int x = 0; x < 4; x++)
    {
        aes->hwDesc.aesIv[x] = AES_REGS->AES_GHASH[x];
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
    aes->hwDesc.aesAadlen = aad_len;
}

void CRYPT_AES_GCMRunBlocks(Aes* aes, const uint8_t * in, uint8_t * out, uint32_t len)
{
    // Set up the counter.
    // Convert from big -> Little
    uint32_t ctr = aes->hwDesc.aesIv[12] << 24 |
                   aes->hwDesc.aesIv[13] << 16 |
                   aes->hwDesc.aesIv[14] << 8 |
                   aes->hwDesc.aesIv[15];
    // Add in the current counter
    ctr = ctr + aes->hwDesc.aesCtr;
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
    memcpy(workBuf, aes->hwDesc.aesIv, 12);
    workBuf[3] = ctr;

    CRYPT_AES_u2238LoadIV(workBuf);
    
    const uint32_t * inPtr = (uint32_t*)in;
    uint32_t * outPtr = (uint32_t*)out;
    uint32_t numBlocks = len >> 4;
    
    aes->hwDesc.aesCtr += numBlocks;
    
    for (int x = 0; x < numBlocks; x++)
    {
        if (((len & 0xf) == 0) && (x == (numBlocks -1)))
        {
            ctrlb->s.EOM = 1;
        }
        CRYPT_AES_u2238InOutBlock((const uint8_t*)inPtr, (uint8_t*)outPtr);
        inPtr+=4;
        outPtr+=4;
    }
    
    if ((len & 0xf) != 0)
    {
        memcpy(workBuf, inPtr, (len & 0xf));
        ctrlb->s.EOM = 1;
        CRYPT_AES_u2238InOutBlock((const uint8_t*)workBuf, (uint8_t*)workBuf);
        memcpy(outPtr, workBuf, (len & 0xf));
    }
    
    memset(workBuf, 0, sizeof(workBuf));  
}

void CRYPT_AES_GCMGenerateFinalGHash(Aes* aes, uint32_t len)
{
    uint32_t workBuf[4] = {0};
    
    uint32_t addLen = aes->hwDesc.aesAadlen * 8;
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
    ctrla->s.ENABLE = 1;
    ctrlb->v = 0;
    
    CRYPT_AES_u2238LoadKey(aes, (uint32_t*)aes->hwDesc.aesKey);
    CRYPT_AES_u2238LoadIV((uint32_t*)aes->hwDesc.aesIv);
    
    ctrlb->s.NEWMSG = 1;
    
    uint8_t workBuf[16] = {0};
    CRYPT_AES_u2238InOutBlock(aes->hwDesc.aesGHash, workBuf);
    memcpy(tag, workBuf, tag_len);
    
    ctrla->s.SWRST = 1;
    ctrla->v = 0;    
}

 int CRYPT_AES_GcmEncrypt(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz)
 {
    aes->hwDesc.aesCtr = 1;
    CRYPT_AES_EnableClock();
    CRYPT_AES_GcmLoadKey(aes);
    CRYPT_AES_GcmLoadKeyCalculateJ0(aes, iv, ivSz);
    CRYPT_AES_GCMProcessAAD(aes, authIn, authInSz);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    ctrla->s.CIPER = CRYPT_AES_U2238_ENCRYPTION;
    CRYPT_AES_GCMRunBlocks(aes, in, out, sz);
    CRYPT_AES_GCMGenerateFinalGHash(aes, sz);     
    CRYPT_AES_GCMGenerateTag(aes, (uint32_t*)authTag, authTagSz);
    CRYPT_AES_DisableClock() ;
    return 0; 
 }
 int CRYPT_AES_GcmDecrypt(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   const unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz)
 {
    aes->hwDesc.aesCtr = 1;
    CRYPT_AES_EnableClock();
    CRYPT_AES_GcmLoadKey(aes);
    CRYPT_AES_GcmLoadKeyCalculateJ0(aes, iv, ivSz);
    CRYPT_AES_GCMProcessAAD(aes, authIn, authInSz);
    volatile CRYPT_AES_U2238_AES_CTRLA * ctrla = (CRYPT_AES_U2238_AES_CTRLA  *)(&AES_REGS->AES_CTRLA);
    ctrla->s.CIPER = CRYPT_AES_U2238_DECRYPTION;
    CRYPT_AES_GCMRunBlocks(aes, in, out, sz);
    CRYPT_AES_GCMGenerateFinalGHash(aes, sz);     
    CRYPT_AES_GCMGenerateTag(aes, (uint32_t*)authTag, authTagSz);
    CRYPT_AES_DisableClock();
    return 0; 
 }



#endif

#endif /* NO_AES */


