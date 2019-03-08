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
#include "crypt_aes_sam6149.h"
#include "crypt_aes_hwInt.h"

#undef COMPONENT_TYPEDEF_STYLE
#define COMPONENT_TYPEDEF_STYLE 'N'

static void enableAesModule()
{
    const uint32_t AesPmcBit = 1u << (ID_AES - 32);
    if ((PMC_REGS->PMC_PCSR1 & (AesPmcBit)) != (AesPmcBit))
    {
        PMC_REGS->PMC_PCER1 = AesPmcBit;
    }
    AES_REGS->AES_CR = AES_CR_SWRST_Msk;

}

static void disableAesModule()
{
   const uint32_t AesPmcBit = 1u << (ID_AES - 32);
   PMC_REGS->PMC_PCDR1 = AesPmcBit;
}

/** AES interrupt callback function type. */
typedef void (*aes_callback_t)(void);


/* helper to compose the configuration for the AES */

static void CRYPT_AES_sam6149LoadKey(const Aes * aesCfg)
{
    AES_REGS->AES_MR = aesCfg->hwDesc.aesMr.v;
    int x = 0;
    for (x = 0; x < sizeof(AES_REGS->AES_KEYWR) >> 2; x++)
    {
        AES_REGS->AES_KEYWR[x] = ((uint32_t*)aesCfg->hwDesc.aesKey)[x];
    }
    
}

static void CRYPT_AES_sam6149LoadIv(const Aes * aesCfg)
{
    int x = 0;
    for (x = 0; x < sizeof(AES_REGS->AES_IVR) >> 2; x++)
    {
        AES_REGS->AES_IVR[x] = ((uint32_t*)aesCfg->hwDesc.aesIv)[x];        
    }    
}


static void CRYPT_AES_sam6149LoadDesc(const Aes * aesCfg)
{
    CRYPT_AES_sam6149LoadKey(aesCfg);

    CRYPT_AES_sam6149LoadIv(aesCfg);
    
    AES_REGS->AES_AADLENR = aesCfg->hwDesc.aesAddlenr;

}



#if defined(HAVE_AESGCM)

static void CRYPT_AES_sam6149LoadGHash(const Aes * aesCfg)
{
    int x = 0;
    for (x = 0; x < sizeof(AES_REGS->AES_GHASHR) >> 2; x++)
    {
        AES_REGS->AES_GHASHR[x] = ((uint32_t*)aesCfg->hwDesc.aesGHash)[x];        
    }

}


static void CRYPT_AES_sam6149SaveDesc(Aes * aesCfg)
{
    int x;
    for (x = 0; x < sizeof(AES_REGS->AES_GHASHR) >> 2; x++)
    {
        ((uint32_t*)aesCfg->hwDesc.aesGHash)[x] = AES_REGS->AES_GHASHR[x];        
    }
    for (x = 0; x < sizeof(AES_REGS->AES_GCMHR) >> 2; x++)
    {
        ((uint32_t*)aesCfg->hwDesc.aesGcmH)[x] = AES_REGS->AES_GCMHR[x];        
    }
}
#endif

#define MIN(x,y) (x<y)?x:y
#define MAX(x,y) (x>y)?x:y

/* Start actions */
int CRYPT_AES_SetKey(Aes* aes, const byte* userKey, word32 keylen, const byte* iv, int dir)
{
    memset(aes, 0, sizeof(Aes));
    aes->hwDesc.aesMr.s.CKEY = 0xE;  // Must be set on first program
    switch (keylen)
    {
        case (128/8):
            aes->hwDesc.aesMr.s.KEYSIZE = CRYPT_AES_SAM6149_AES128;
            break;
        case (192/8):
            aes->hwDesc.aesMr.s.KEYSIZE = CRYPT_AES_SAM6149_AES192;
            aes->hwDesc.aesMr.s.PROCDLY = 0;
            break;
        case (256/8):
            aes->hwDesc.aesMr.s.KEYSIZE = CRYPT_AES_SAM6149_AES256;
            aes->hwDesc.aesMr.s.PROCDLY = 0;
            break;
        default:
            return BAD_FUNC_ARG;  // Invalid Key size            
    }
    aes->hwDesc.aesMr.s.SMOD = CRYPT_AES_SAM6149_AUTO_START;
    aes->hwDesc.aesMr.s.PROCDLY = 0; // The datasheet is confusingly written.
    if (dir == CRYPT_AES_ENCRYPTION)
    {
        aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_ENCRYPTION;
    }
    else
    {
        aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_DECRYPTION;        
    }
    
    memcpy(aes->hwDesc.aesKey, userKey, MIN(sizeof(aes->hwDesc.aesKey), keylen));
    if (iv != NULL)
    {
        memcpy(aes->hwDesc.aesIv, iv, sizeof(aes->hwDesc.aesIv));
    }

    /* save the particulars about the key and IV */

    return 0;
}

/* AES-CBC */
#ifdef HAVE_AES_CBC


int CRYPT_AES_CbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    /* set all the fields needed to set-up the AES engine */
    aes->hwDesc.aesMr.s.LOD = 0;
    aes->hwDesc.aesMr.s.GTAGEN = 0;
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_CBC;
    aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_ENCRYPTION;

    /* Enable clock for AES */
    enableAesModule();
    /* software reset */
    AES_REGS->AES_CR = AES_CR_SWRST_Msk;
    CRYPT_AES_sam6149LoadDesc(aes);
    AES_REGS->AES_CLENR = sz;

    /* set up pointers for input and output buffers */
    const uint32_t *inptr = (const uint32_t *)in;
    uint32_t *outptr = (uint32_t *)out;

    uint32_t block;   /* 16 bytes = 4 32bit block size */
    for (block = 0; block < sz; block += 16)
    {
        /* Write the data to be ciphered to the input data registers. */
        AES_REGS->AES_IDATAR[0] = *inptr++;
        AES_REGS->AES_IDATAR[1] = *inptr++;
        AES_REGS->AES_IDATAR[2] = *inptr++;
        AES_REGS->AES_IDATAR[3] = *inptr++;

        /* Note the blocking here - state machine this? */
        while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

        /* encrypt complete - read out the data */
        *outptr++ = AES_REGS->AES_ODATAR[0];
        *outptr++ = AES_REGS->AES_ODATAR[1];
        *outptr++ = AES_REGS->AES_ODATAR[2];
        *outptr++ = AES_REGS->AES_ODATAR[3];
    }
    disableAesModule();
    memcpy(aes->hwDesc.aesIv, (out + sz - 16), sizeof(aes->hwDesc.aesIv));

    return 0;
}



#ifdef HAVE_AES_DECRYPT
int CRYPT_AES_CbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    aes->hwDesc.aesMr.s.LOD = 0;
    aes->hwDesc.aesMr.s.GTAGEN = 0;
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_CBC;
    aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_DECRYPTION;

        /* Enable clock for AES */
    enableAesModule();
    /* software reset */
    AES_REGS->AES_CR = AES_CR_SWRST_Msk;
    CRYPT_AES_sam6149LoadDesc(aes);
    AES_REGS->AES_CLENR = sz;

    /* set up pointers for input and output buffers */
    const uint32_t *inptr = (const uint32_t *)in;
    uint32_t *outptr = (uint32_t *)out;

    uint32_t block;   /* 16 bytes = 4 32bit block size */
    for (block = 0; block < sz; block += 16)
    {
        /* Write the data to be ciphered to the input data registers. */
        AES_REGS->AES_IDATAR[0] = *inptr++;
        AES_REGS->AES_IDATAR[1] = *inptr++;
        AES_REGS->AES_IDATAR[2] = *inptr++;
        AES_REGS->AES_IDATAR[3] = *inptr++;

        /* Note the blocking here - state machine this? */
        while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

        /* encrypt complete - read out the data */
        *outptr++ = AES_REGS->AES_ODATAR[0];
        *outptr++ = AES_REGS->AES_ODATAR[1];
        *outptr++ = AES_REGS->AES_ODATAR[2];
        *outptr++ = AES_REGS->AES_ODATAR[3];
    }
    disableAesModule();
    memcpy(aes->hwDesc.aesIv, (in + sz - 16), sizeof(aes->hwDesc.aesIv));

    return 0;
}
#endif /* HAVE_AES_DECRYPT */


#endif /* HAVE_AES_CBC */



/* AES-CTR */
#ifdef WOLFSSL_AES_COUNTER

int CRYPT_AES_CtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    aes->hwDesc.aesMr.s.LOD = 0;
    aes->hwDesc.aesMr.s.GTAGEN = 0;
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_CTR;
    aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_ENCRYPTION;

    enableAesModule();

    CRYPT_AES_sam6149LoadDesc(aes);
    AES_REGS->AES_CLENR = sz;
    

    /* set up pointers for input and output buffers */
    const uint32_t *inptr = (const uint32_t *)in;
    uint32_t *outptr = (uint32_t *)out;

    uint32_t block;   /* 16 bytes = 4 32bit block size */
    for (block = 0; block < sz; block += 16)
    {
        /* Write the data to be ciphered to the input data registers. */
        AES_REGS->AES_IDATAR[0] = *inptr++;
        AES_REGS->AES_IDATAR[1] = *inptr++;
        AES_REGS->AES_IDATAR[2] = *inptr++;
        AES_REGS->AES_IDATAR[3] = *inptr++;

        /* Note the blocking here - state machine this? */
        while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

        /* encrypt complete - read out the data */
        *outptr++ = AES_REGS->AES_ODATAR[0];
        *outptr++ = AES_REGS->AES_ODATAR[1];
        *outptr++ = AES_REGS->AES_ODATAR[2];
        *outptr++ = AES_REGS->AES_ODATAR[3];
    }
    disableAesModule();
    
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

int CRYPT_AES_GcmJ0Generation(Aes* aes, const unsigned char* iv, unsigned int ivSz)
{
    if (iv == NULL)
    {
        return -1;
    }
    if (ivSz == 12)
    {
        memcpy(aes->hwDesc.aesIv, iv, 12);
        aes->hwDesc.aesIv[(sizeof(aes->hwDesc.aesIv)-1)] = 0x1;
        return 0;
    }
    //1. Set AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?0?.
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_GCM;
    aes->hwDesc.aesMr.s.GTAGEN = 0;

    
    //1. Set the AES Key Register and wait until AES_ISR.DATRDY is set 
    // (GCM hash subkey generation complete);
    enableAesModule();
    CRYPT_AES_sam6149LoadKey(aes);
    /* Note the blocking here - state machine this? */
    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

    

    //2. Configure AES_AADLENR.AADLEN with ?len(IV || 0s+64 || [len(IV)]64)? 
    // in and AES_CLENR.CLEN to ?0?. This will allow running a GHASHH only.
    int numFullBlocks = ivSz / 16;
    if (ivSz % 16 > 0)
    {
        // This is questionable.  The formula says to use the bit size.
        // But the register description is byte size.
        AES_REGS->AES_AADLENR = (numFullBlocks + 2) * 128;
    }
    else
    {
        AES_REGS->AES_AADLENR = (numFullBlocks + 1) * 128;        
    }
    AES_REGS->AES_CLENR = 0;

    //3. Fill AES_IDATARx.IDATA with the message to process (IV || 0s+64 || [len(IV)]64) 
    uint32_t * inPtr = (uint32_t*)iv;
    for (int x = 0; x < numFullBlocks; x++)
    {
        /* Write the data to be ciphered to the input data registers. */
        AES_REGS->AES_IDATAR[0] = *inPtr++;
        AES_REGS->AES_IDATAR[1] = *inPtr++;
        AES_REGS->AES_IDATAR[2] = *inPtr++;
        AES_REGS->AES_IDATAR[3] = *inPtr++;

        /* Note the blocking here - state machine this? */
        while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;        
    }    
    int numPartialBytes = ivSz % 16;
    if (numPartialBytes > 0)
    {
        uint32_t partialPlusPad[4] = {0};
        memcpy(partialPlusPad, inPtr, numPartialBytes);
        AES_REGS->AES_IDATAR[0] = partialPlusPad[0];
        AES_REGS->AES_IDATAR[1] = partialPlusPad[1];
        AES_REGS->AES_IDATAR[2] = partialPlusPad[2];
        AES_REGS->AES_IDATAR[3] = partialPlusPad[3];

        /* Note the blocking here - state machine this? */
        while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;                
    }
    uint8_t finalBlock[16] = {0};
    int bits = ivSz * 8;
    //This may be wrong, but we have to change it to big endian format.  Per
    //NIST AES GCM is big endian
    finalBlock[15] = (uint8_t)(bits&0xff);
    finalBlock[14] = (uint8_t)((bits>>8)&0xff);
    finalBlock[13] = (uint8_t)((bits>>16)&0xff);
    finalBlock[12] = (uint8_t)((bits>>24)&0xff);
    
    AES_REGS->AES_IDATAR[0] = ((uint32_t*)finalBlock)[0];
    AES_REGS->AES_IDATAR[1] = ((uint32_t*)finalBlock)[1];
    AES_REGS->AES_IDATAR[2] = ((uint32_t*)finalBlock)[2];
    AES_REGS->AES_IDATAR[3] = ((uint32_t*)finalBlock)[3];

    /* Note the blocking here - state machine this? */
    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;      

    //4. Read AES_GHASHRx.GHASH to obtain the J0 value
    CRYPT_AES_sam6149SaveDesc(aes);
    memcpy(aes->hwDesc.aesIv, aes->hwDesc.aesGHash, sizeof(aes->hwDesc.aesIv));
    uint32_t tmp = (aes->hwDesc.aesIv[15] & 0xff) |
                   ((aes->hwDesc.aesIv[14] & 0xff) << 8) |
                   ((aes->hwDesc.aesIv[13] & 0xff) << 16) |
                   ((aes->hwDesc.aesIv[12] & 0xff) << 24);
    tmp++;
    aes->hwDesc.aesCtr = ((tmp & 0x000000ff) << 24) |
                         ((tmp & 0x0000ff00) << 8) |
                         ((tmp & 0x00ff0000) >> 8) |
                         ((tmp & 0xff000000) >> 24);
    
    disableAesModule();
    return 0;
}

int CRYPT_AES_GcmResetCtx(Aes * aes)
{
    memset(aes->hwDesc.aesIv, 0, sizeof(aes->hwDesc.aesIv));    
    aes->hwDesc.aesAddlenr = 0;
    memset(aes->hwDesc.aesGHash, 0, sizeof(aes->hwDesc.aesGHash));
    aes->hwDesc.aesCtr = 0;
    memset(aes->hwDesc.aesGcmH, 0, sizeof(aes->hwDesc.aesGcmH));
    return 0;
}

int CRYPT_AES_GsmWriteIv(Aes * aes)
{

    int x = 0;
    for (x = 0; x < 3; x++)
    {
        AES_REGS->AES_IVR[x] = ((uint32_t*)aes->hwDesc.aesIv)[x];        
    }    

    AES_REGS->AES_IVR[3] = aes->hwDesc.aesCtr;
    return 0;
    
}

int CRYPT_AES_GsmRunBlocks(Aes * aes, const unsigned int* blockIn, unsigned int blockInSz, unsigned int* out)
{
    if (blockInSz == 0)
    {
        return 0;
    }
    int numFullBlocks = blockInSz / 16;
    for (int x = 0; x < numFullBlocks; x++)
    {
        /* Write the data to be ciphered to the input data registers. */
        AES_REGS->AES_IDATAR[0] = *blockIn++;
        AES_REGS->AES_IDATAR[1] = *blockIn++;
        AES_REGS->AES_IDATAR[2] = *blockIn++;
        AES_REGS->AES_IDATAR[3] = *blockIn++;

        /* Note the blocking here - state machine this? */
        while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

        if (out != NULL)
        {
            /* encrypt complete - read out the data */
            *out++ = AES_REGS->AES_ODATAR[0];
            *out++ = AES_REGS->AES_ODATAR[1];
            *out++ = AES_REGS->AES_ODATAR[2];
            *out++ = AES_REGS->AES_ODATAR[3];
        }
    }
    
    int numBytes = blockInSz % 16;
    if (numBytes > 0)
    {
        uint32_t partialPlusPad[4] = {0};
        memcpy(partialPlusPad, blockIn, blockInSz % 16);
        AES_REGS->AES_IDATAR[0] = partialPlusPad[0];
        AES_REGS->AES_IDATAR[1] = partialPlusPad[1];
        AES_REGS->AES_IDATAR[2] = partialPlusPad[2];
        AES_REGS->AES_IDATAR[3] = partialPlusPad[3];
        while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

        if (out != NULL)
        {
            /* encrypt complete - read out the data */
            if (numBytes >= 4)
            {
                *out++ = AES_REGS->AES_ODATAR[0];
                if (numBytes >= 8)
                {
                    *out++ = AES_REGS->AES_ODATAR[1];
                    if (numBytes >= 12)
                    {
                        *out++ = AES_REGS->AES_ODATAR[2];
                        if (numBytes > 12)
                        {
                            uint32_t tmp = AES_REGS->AES_ODATAR[3];
                            memcpy(out, &tmp, numBytes-12);                                                
                        }
                    }
                    else
                    {
                        uint32_t tmp = AES_REGS->AES_ODATAR[2];
                        memcpy(out, &tmp, numBytes-8);                    
                    }
                }
                else
                {
                    uint32_t tmp = AES_REGS->AES_ODATAR[1];
                    memcpy(out, &tmp, numBytes-4);                    
                }
            }
            else
            {
                uint32_t tmp = AES_REGS->AES_ODATAR[0];
                memcpy(out, &tmp, numBytes);
            }
        }
    }
    return 0;
}

int CRYPT_AES_GcmCmpMsgWithTag(Aes * aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz )
{
    //3. Calculate the J0 value
    aes->hwDesc.aesCtr = 0x02000000;
    CRYPT_AES_GcmJ0Generation(aes, iv, ivSz);
    enableAesModule();

    //1. Set AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_GCM;
    aes->hwDesc.aesMr.s.GTAGEN = 1;
    
    //2. cSet AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    CRYPT_AES_sam6149LoadKey(aes);
    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

    //4. Set AES_IVRx.IV with inc32(J0) (J0 + 1 on 32 bits).
    CRYPT_AES_GsmWriteIv(aes);
    
    AES_REGS->AES_AADLENR = authInSz;
    AES_REGS->AES_CLENR = sz;
    
    CRYPT_AES_GsmRunBlocks(aes, (const unsigned int*)authIn, authInSz, NULL);
    CRYPT_AES_GsmRunBlocks(aes, (const unsigned int*)in, sz, (unsigned int*)out);
    
    if (authInSz != 0 || sz != 0)
    {
        // Block waiting on the tag
        while (!(AES_REGS->AES_ISR & AES_ISR_TAGRDY_Msk))  ;
    }    
    for (int x = 0; x < 4; x++)
    {
        ((uint32_t*)authTag)[x] = AES_REGS->AES_TAGR[x];
    }
    CRYPT_AES_sam6149SaveDesc(aes);
    
    disableAesModule();
    return 0;    
}

int crypt_aesGcm1stMsgFrag(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   const unsigned char* authIn, unsigned int authInSz)
{
    //3. Calculate the J0 value
    aes->hwDesc.aesCtr = 0x02000000;
    CRYPT_AES_GcmJ0Generation(aes, iv, ivSz);
    enableAesModule();

    //1. Set AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_GCM;
    aes->hwDesc.aesMr.s.GTAGEN = 0;
    
    //2. cSet AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    CRYPT_AES_sam6149LoadKey(aes);
    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

    //4. Set AES_IVRx.IV with inc32(J0) (J0 + 1 on 32 bits).
    CRYPT_AES_GsmWriteIv(aes);
    
    AES_REGS->AES_AADLENR = authInSz;
    AES_REGS->AES_CLENR = sz;
    
    CRYPT_AES_GsmRunBlocks(aes, (const unsigned int*)authIn, authInSz, NULL);
    CRYPT_AES_GsmRunBlocks(aes, (const unsigned int*)in, sz, (unsigned int*)out);
    
    CRYPT_AES_sam6149SaveDesc(aes);
    
    disableAesModule();    
    return 0;
}

int crypt_aesGcmMoreMsgFrag(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz)
{
    //3. Calculate the J0 value
    enableAesModule();

    //1. Set AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_GCM;
    aes->hwDesc.aesMr.s.GTAGEN = 0;
    
    //2. cSet AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    CRYPT_AES_sam6149LoadKey(aes);
    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

    //4. Set AES_IVRx.IV with inc32(J0) (J0 + 1 on 32 bits).
    CRYPT_AES_GsmWriteIv(aes);
    
    AES_REGS->AES_AADLENR = 0;
    AES_REGS->AES_CLENR = sz;
    CRYPT_AES_sam6149LoadGHash(aes);    
    CRYPT_AES_GsmRunBlocks(aes, (const unsigned int*)in, sz, (unsigned int*)out);
    
    CRYPT_AES_sam6149SaveDesc(aes);
    
    disableAesModule();    
    return 0;
    
}

int crypt_aesGcmGenerateTag(Aes* aes, unsigned int sz, unsigned char* authTag, unsigned int authTagSz, unsigned int authInSz)
{
    //3. Calculate the J0 value
    enableAesModule();

    //1. Set AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_GCM;
    aes->hwDesc.aesMr.s.GTAGEN = 0;
    
    //2. Set AES_MR.OPMOD to GCM and AES_MR.GTAGEN to ?1?.
    CRYPT_AES_sam6149LoadKey(aes);
    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk));
    
    //3. Configure AES_AADLENR.AADLEN to 0x10 (16 bytes) and AES_CLENR.CLEN to ?0?.
    AES_REGS->AES_AADLENR = 0x10;
    AES_REGS->AES_CLENR = 0;
    
    //4. Fill AES_GHASHRx.GHASH with the state of the GHASH field stored at the end of the message
    // processing.
    CRYPT_AES_sam6149LoadGHash(aes);
    
    //5, Fill AES_IDATARx.IDATA according to the SMOD configuration used with ?len(AAD)64 || len(C)64?
    authInSz = authInSz * 8;
    sz = sz * 8;
    
    AES_REGS->AES_IDATAR[0] = 0;
    AES_REGS->AES_IDATAR[1] = (((authInSz << 24) & 0xff000000) | 
                       ((authInSz << 8) & 0x00ff0000) |
                       ((authInSz >> 8) & 0x0000ff00) |
                       ((authInSz >> 24) & 0x000000ff));
    AES_REGS->AES_IDATAR[2] = 0;
    AES_REGS->AES_IDATAR[3] = ((sz << 24) & 0xff000000) | 
                       ((sz << 8) & 0x00ff0000) |
                       ((sz >> 8) & 0x0000ff00) |
                       ((sz >> 24) & 0x000000ff);
    
    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;
    
    //6. Read AES_GHASHRx.GHASH to obtain the current value of the hash.
    CRYPT_AES_sam6149SaveDesc(aes);
    AES_REGS->AES_CR = AES_CR_SWRST_Msk;
    //Processing T = GCTRK(J0, S):
    
    
    //Set AES_MR.OPMOD to CTR
    aes->hwDesc.aesMr.s.LOD = 0;
    aes->hwDesc.aesMr.s.GTAGEN = 0;
    aes->hwDesc.aesMr.s.OPMODE = CRYPT_AES_SAM6149_CTR;
    aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_ENCRYPTION;

    //2. Set AES_IVRx.IV with ?J0? value.
    CRYPT_AES_sam6149LoadDesc(aes);

    
    //3. Fill AES_IDATARx.IDATA with the GHASH value read at step 6 and wait for DATRDY to be set
    AES_REGS->AES_IDATAR[0] = ((uint32_t*)aes->hwDesc.aesGHash)[0];
    AES_REGS->AES_IDATAR[1] = ((uint32_t*)aes->hwDesc.aesGHash)[1];
    AES_REGS->AES_IDATAR[2] = ((uint32_t*)aes->hwDesc.aesGHash)[2];
    AES_REGS->AES_IDATAR[3] = ((uint32_t*)aes->hwDesc.aesGHash)[3];

    while (!(AES_REGS->AES_ISR & AES_ISR_DATRDY_Msk))  ;

    //4. Read AES_ODATARx.ODATA to obtain the GCM Tag value.
    uint32_t tgSt[4];
    tgSt[0] = AES_REGS->AES_ODATAR[0];
    tgSt[1] = AES_REGS->AES_ODATAR[1];
    tgSt[2] = AES_REGS->AES_ODATAR[2];
    tgSt[3] = AES_REGS->AES_ODATAR[3];
    
    memcpy(authTag, tgSt, authTagSz);
    return 0;
}


int CRYPT_AES_GcmEncrypt(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz)
{
    aes->hwDesc.aesMr.s.LOD = 0;
    aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_ENCRYPTION;
    int ret = 0;   
    if (sz != 0 || authInSz != 0)
    {
        if (aes->hwDesc.aesCtr == 0)
        {
            if (authTag != NULL)
            {
                ret = CRYPT_AES_GcmCmpMsgWithTag(aes, out, in, sz, iv, ivSz, authTag, authTagSz, authIn, authInSz);
                CRYPT_AES_GcmResetCtx(aes);
                return ret;
            }
            else
            {
                return crypt_aesGcm1stMsgFrag(aes, out, in, sz, iv, ivSz, authIn, authInSz);
            }        
        }
        ret = crypt_aesGcmMoreMsgFrag(aes, out, in, sz);
    }
    if (authTag != NULL)
    {
        if (aes->hwDesc.aesCtr == 0)
        {
            CRYPT_AES_GcmJ0Generation(aes, iv, ivSz);
        }
        ret = crypt_aesGcmGenerateTag(aes, sz, authTag, authTagSz, authInSz);
        CRYPT_AES_GcmResetCtx(aes);
    }
    
    return ret;
}

int CRYPT_AES_GcmDecrypt(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   const unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz)
{
    aes->hwDesc.aesMr.s.LOD = 0;
    aes->hwDesc.aesMr.s.CIPHER = CRYPT_AES_SAM6149_DECRYPTION;
    int ret = 0;
    if (sz != 0 || authInSz != 0)
    {
        if (aes->hwDesc.aesCtr == 0)
        {
            if (authTag != NULL)
            {
                unsigned char cmpAuthTag[16] =  {0};
                ret = CRYPT_AES_GcmCmpMsgWithTag(aes, out, in, sz, iv, ivSz, cmpAuthTag, sizeof(cmpAuthTag), authIn, authInSz);
                CRYPT_AES_GcmResetCtx(aes);

                if (memcmp(authTag, cmpAuthTag, authTagSz))
                {
                    return AES_GCM_AUTH_E;
                }
                return 0;
            }
            else
            {
                return crypt_aesGcm1stMsgFrag(aes, out, in, sz, iv, ivSz, authIn, authInSz);
            }        
        }
        ret = crypt_aesGcmMoreMsgFrag(aes, out, in, sz);
    }
    if (authTag != NULL)
    {
        if (aes->hwDesc.aesCtr == 0)
        {
            CRYPT_AES_GcmJ0Generation(aes, iv, ivSz);
        }
        unsigned char cmpAuthTag[16] =  {0};
        ret = crypt_aesGcmGenerateTag(aes, sz, cmpAuthTag, sizeof(cmpAuthTag), authInSz);
        CRYPT_AES_GcmResetCtx(aes);
        if (memcmp(authTag, cmpAuthTag, authTagSz))
        {
            return AES_GCM_AUTH_E;
        }
    }
    return ret;
}

#endif

#endif /* NO_AES */


