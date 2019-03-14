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
/******************************************************************************
 Copyright (C) 2017-2018 Microchip Technology Inc. and its subsidiaries.

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
*******************************************************************************/
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

#undef COMPONENT_TYPEDEF_STYLE
#define COMPONENT_TYPEDEF_STYLE 'N'


/* These should all line up with CMSIS */
/** AES processing mode. */
enum aes_encrypt_mode {
    AES_SAME70_DECRYPTION = 0, /**< Decryption of data will be performed. */
    AES_SAME70_ENCRYPTION,     /**< Encryption of data will be performed. */
};

/** AES cryptographic key size. */
enum aes_key_size {
    AES_KEY_SIZE_128 = 0,   /**< AES key size is 128 bits. */
    AES_KEY_SIZE_192,       /**< AES key size is 192 bits. */
    AES_KEY_SIZE_256,       /**< AES key size is 256 bits. */
};

/** AES start mode. */
enum aes_start_mode {
    AES_MANUAL_START = 0,   /**< Manual start mode. */
    AES_AUTO_START,         /**< Auto start mode. */
    AES_IDATAR0_START,      /**< AES_IDATAR0 access only Auto Mode. */
};

/** AES cipher block mode. */
enum aes_opmode {
    AES_ECB_MODE = 0,       /**< Electronic Codebook (ECB). */
    AES_CBC_MODE,           /**< Cipher Block Chaining (CBC). */
    AES_OFB_MODE,           /**< Output Feedback (OFB). */
    AES_CFB_MODE,           /**< Cipher Feedback (CFB). */
    AES_CTR_MODE,           /**< Counter (CTR). */
    AES_GCM_MODE,           /**< Galois Counter Mode (GCM).*/
};

/** AES Cipher FeedBack (CFB) size. */
enum aes_cfb_size {
    AES_CFB_SIZE_128 = 0,   /**< Cipher feedback data size is 128-bit. */
    AES_CFB_SIZE_64,        /**< Cipher feedback data size is 64-bit. */
    AES_CFB_SIZE_32,        /**< Cipher feedback data size is 32-bit. */
    AES_CFB_SIZE_16,        /**< Cipher feedback data size is 16-bit. */
    AES_CFB_SIZE_8,         /**< Cipher feedback data size is 8-bit. */
};

/** AES interrupt source type. */
typedef enum aes_interrupt_source {
    /** Data ready interrupt.*/
    AES_INTERRUPT_DATA_READY = AES_IER_DATRDY_Msk,
    /** Unspecified register access detection interrupt.*/
    AES_INTERRUPT_UNSPECIFIED_REGISTER_ACCESS = AES_IER_URAD_Msk,

    AES_INTERRUPT_TAG_READY = AES_IER_TAGRDY_Msk,
} aes_interrupt_source_t;


#define AES_INTERRUPT_SOURCE_NUM 3


/** AES interrupt callback function type. */
typedef void (*aes_callback_t)(void);


/* AES configuration */
struct aes_config
{
    /** AES data mode (decryption or encryption). */
    enum aes_encrypt_mode encrypt_mode;

    /** AES key size. */
    enum aes_key_size key_size;

    /** Start mode. */
    enum aes_start_mode start_mode;

    /** AES block cipher operation mode.*/
    enum aes_opmode opmode;

    /** Cipher feedback data size. */
    enum aes_cfb_size cfb_size;

    /** Last output data mode enable/disable. */
    int lod;

    /** Galois Counter Mode (GCM) automatic tag generation enable/disable */
    int gtag_en;

    /** Processing delay parameter. */
    uint32_t processing_delay;
} ;


/* our local configuration */
static struct aes_config aes_configuration;



/* helper to compose the configuration for the AES */
static void AesConfigure(struct aes_config *const p_cfg)
{
    uint32_t ul_mode = 0;

    /* Set processing mode */
    if (p_cfg->encrypt_mode == AES_SAME70_ENCRYPTION)
    {
        /* 0 = decrypt., 1 = encrypt */
        ul_mode |= AES_MR_CIPHER_Msk;
    }

    /* Active dual buffer in DMA mode */
    if (p_cfg->start_mode == AES_IDATAR0_START)
    {
        ul_mode |= AES_MR_DUALBUFF_ACTIVE;
    }

    /* Set start mode */
    ul_mode |= (p_cfg->start_mode << AES_MR_SMOD_Pos);

    /* Set key size */
    ul_mode |= (p_cfg->key_size << AES_MR_KEYSIZE_Pos);

    /* Set Confidentiality mode */
    ul_mode |= (p_cfg->opmode << AES_MR_OPMOD_Pos);

    /* Set CFB size */
    ul_mode |= (p_cfg->cfb_size << AES_MR_CFBS_Pos);

    /* Last Output Data */
    if (p_cfg->lod)
    {
        ul_mode |= AES_MR_LOD_Msk;
    }

    if ((p_cfg->opmode == AES_GCM_MODE) && (p_cfg->gtag_en))
    {
        ul_mode |= AES_MR_GTAGEN_Msk;
    }

    ul_mode |= AES_MR_PROCDLY(p_cfg->processing_delay);

    ul_mode |= AES_MR_CKEY_PASSWD;

    AES_REGS->AES_MR = ul_mode;
}



/* key length enters as number of bytes but we do words so / by 4 */
static void AesWriteKey(const word32 *AesKey, int key_length)
{
    if (AesKey)
    {
        uint8_t i;

        key_length /= 4;
        for (i = 0; i < key_length; i++)
        {
            AES_REGS->AES_KEYWR[i] = *AesKey++;
        }
    }
}


static void AesWriteIV(word32 *iv)
{
    /* put AES IV into place */
    AES_REGS->AES_IVR[0] = iv[0];
    AES_REGS->AES_IVR[1] = iv[1];
    AES_REGS->AES_IVR[2] = iv[2];
    AES_REGS->AES_IVR[3] = iv[3];
}



/* Start actions */
int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen, const byte* iv, int dir)
{
    /* valid key length = 16, 24, 32 */
    if (!((keylen == 16) || (keylen == 24) || (keylen == 32)))
    {
        return BAD_FUNC_ARG;
    }

    /* Enable clock for AES */
    uint32_t AesPmcBit = 1u << (ID_AES - 32);
    if ((PMC_REGS->PMC_PCSR1 & (AesPmcBit)) != (AesPmcBit))
    {
        PMC_REGS->PMC_PCER1 = AesPmcBit;

        /* software reset */
        AES_REGS->AES_CR = AES_CR_SWRST_Msk;

    }

    /* save the particulars about the key and IV */
    memcpy(aes->key_ce, userKey, keylen);
    aes->keylen = keylen;
    wc_AesSetIV(aes, iv);

    return 0;
}




#if defined(WOLFSSL_AES_DIRECT) || defined(WOLFSSL_AES_COUNTER)

/* AES-CTR and AES-DIRECT need to use this for key setup, no aesni yet */
int wc_AesSetKeyDirect(Aes* aes,
                        const byte* userKey,
                        word32 keylen,
                        const byte* iv,
                        int dir)
{
    /* pass this on */
    return wc_AesSetKey(aes, userKey, keylen, iv, dir);
}

#endif /* WOLFSSL_AES_DIRECT || WOLFSSL_AES_COUNTER */



/* wc_AesSetIV is shared between software and hardware */
int wc_AesSetIV(Aes* aes, const byte* iv)
{
    if (iv)
    {
        memcpy((void*)aes->iv_ce, (void*)iv, AES_BLOCK_SIZE);
    }
    else
    {
        memset((void*)aes->iv_ce, 0, AES_BLOCK_SIZE);
    }

    return 0;
}



#if defined(WOLFSSL_AES_DIRECT)

/* AES-DIRECT */
void wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in)
{
    /* one block is 4 32bit words, 16 bytes */
    wc_AesCbcEncrypt(aes, out, in, AES_BLOCK_SIZE);
}



/* Allow direct access to one block decrypt */
void wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in)
{
    /* one block is 4 32bit words, 16 bytes */
    wc_AesCbcDecrypt(aes, out, in, AES_BLOCK_SIZE);
}

#endif  /* AES DIRECT */




/* AES-CBC */
#ifdef HAVE_AES_CBC


int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    /* set all the fields needed to set-up the AES engine */
    aes_configuration.lod = 0;
    aes_configuration.gtag_en = 0;
    aes_configuration.processing_delay = 0;
    aes_configuration.opmode = AES_CBC_MODE;
    aes_configuration.start_mode = AES_AUTO_START;
    aes_configuration.cfb_size = AES_CFB_SIZE_128;
    aes_configuration.encrypt_mode = AES_SAME70_ENCRYPTION;

    /* keylen is in bytes from call - CMSIS is enum */
    switch (aes->keylen)
    {
        default:
        case 16: aes_configuration.key_size = AES_KEY_SIZE_128; break;
        case 24: aes_configuration.key_size = AES_KEY_SIZE_192; break;
        case 32: aes_configuration.key_size = AES_KEY_SIZE_256; break;
    }

    AesConfigure(&aes_configuration);
    AesWriteKey(aes->key_ce, aes->keylen);
    AesWriteIV(aes->iv_ce);


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

    /* Last IV equals last cipher text */
    wc_AesSetIV(aes, out + sz - 16);

    return 0;
}



#ifdef HAVE_AES_DECRYPT
int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    aes_configuration.encrypt_mode = AES_SAME70_DECRYPTION;
    aes_configuration.key_size = AES_KEY_SIZE_128;
    aes_configuration.start_mode = AES_AUTO_START;
    aes_configuration.opmode = AES_CBC_MODE;
    aes_configuration.cfb_size = AES_CFB_SIZE_128;
    aes_configuration.lod = 0;
    aes_configuration.gtag_en = 0;
    aes_configuration.processing_delay = 0;

    AesConfigure(&aes_configuration);
    AesWriteKey(aes->key_ce, aes->keylen);
    AesWriteIV(aes->iv_ce);

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

    /* Last IV equals last cipher text */
    wc_AesSetIV(aes, in + sz - 16);

    return 0;
}
#endif /* HAVE_AES_DECRYPT */


#endif /* HAVE_AES_CBC */



/* AES-CTR */
#ifdef WOLFSSL_AES_COUNTER

int wc_AesCtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    aes_configuration.encrypt_mode = AES_SAME70_ENCRYPTION;
    aes_configuration.key_size = AES_KEY_SIZE_128;
    aes_configuration.start_mode = AES_AUTO_START;
    aes_configuration.opmode = AES_CTR_MODE;
    aes_configuration.cfb_size = AES_CFB_SIZE_128;
    aes_configuration.lod = 0;
    aes_configuration.gtag_en = 0;
    aes_configuration.processing_delay = 0;

    AesConfigure(&aes_configuration);
    AesWriteKey(aes->key_ce, aes->keylen);
    AesWriteIV(aes->iv_ce);

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
    
    return 0;
}


#endif /* WOLFSSL_AES_COUNTER */



#ifdef HAVE_AESCCM

#ifdef WOLFSSL_PIC32MZ_CRYPT
    #error "PIC32MZ doesn't currently support AES-CCM mode"

#endif
#endif /* HAVE_AESCCM */


#endif /* NO_AES */


