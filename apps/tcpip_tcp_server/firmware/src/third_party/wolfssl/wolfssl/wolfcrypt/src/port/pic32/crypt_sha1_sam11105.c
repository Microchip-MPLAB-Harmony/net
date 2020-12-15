/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    sha_same70.c

  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2016-2019 Microchip Technology Inc. and its subsidiaries.

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
#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO) && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_11105) && defined(WOLFSSL_HAVE_MCHP_HW_SHA1)

#include "wolfssl/wolfcrypt/settings.h"
#include "system/cache/sys_cache.h"

#if !defined(NO_SHA)

#include "wolfssl/wolfcrypt/sha.h"
#include "definitions.h"

#include "wolfssl/wolfcrypt/port/pic32/crypt_sha1_hw.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_sha_sam11105.h"

static struct icm_descriptor actIcmDescriptor __attribute__((aligned (64)));
static uint8_t  actBuffer[SHA_BLOCK_SIZE] __attribute__((aligned (64)));  /* 64 bytes = 512 bits */
static uint32_t actDigest[SHA_DIGEST_SIZE/4] __attribute__((aligned (128)));

int wc_InitSha_ex(Sha* sha, void* heap, int devId)
{
    /* Enable ICM if not already */

    /* ICM does the SHA work */
    memset(sha, 0, sizeof(Sha));
    sha->icm_descriptor.cfg.reg = 0x0;
    sha->icm_descriptor.cfg.bit.is_end_mon = 1;

    sha->digest[0] = 0x01234567;
    sha->digest[1] = 0x89ABCDEF;
    sha->digest[2] = 0xFEDCBA98;
    sha->digest[3] = 0x76543210;
    sha->digest[4] = 0xF0E1D2C3;

    sha->total_len = 0;

    return 0;
}



static int32_t CRYPT_SHA1_Process(Sha *sha, const uint8_t *input, word32 length)
{
	ICM_REGS->ICM_CTRL = ICM_CTRL_SWRST(1);
    
    sha->icm_descriptor.start_addr = (uint32_t)input;

    /* Transfer size = (tran_size + 1) * 512bits */
    sha->icm_descriptor.tran_size =  (length >> 6) - 1;

	memcpy(&actIcmDescriptor, &(sha->icm_descriptor), sizeof(sha->icm_descriptor));
    SYS_CACHE_CleanDCache_by_Addr((uint32_t*) (&actIcmDescriptor), sizeof(sha->icm_descriptor));
    SYS_CACHE_CleanDCache_by_Addr((uint32_t *)input, length);
    //SYS_CACHE_CleanInvalidateDCache_by_Addr((uint32_t *)&(sha->digest), SHA_DIGEST_SIZE);

        /* memory barrier */
    __DMB();

    
    /* ICM can set up FIPS default starting digest */
    ICM_REGS->ICM_DSCR = (uint32_t)&(actIcmDescriptor);


    /* configured so initial hash digest is auto-loaded */
    ICM_REGS->ICM_CFG = ICM_CFG_SLBDIS(1)
                 | ICM_CFG_BBC(0)
                 | ICM_CFG_UALGO_SHA1
                 | ICM_CFG_UIHASH_Msk;

    /* digest must be 128 aligned */
    ICM_REGS->ICM_HASH = (uint32_t)(&(actDigest));
    ICM_REGS->ICM_UIHVAL[0] = sha->digest[0];
    ICM_REGS->ICM_UIHVAL[1] = sha->digest[1];
    ICM_REGS->ICM_UIHVAL[2] = sha->digest[2];
    ICM_REGS->ICM_UIHVAL[3] = sha->digest[3];
    ICM_REGS->ICM_UIHVAL[4] = sha->digest[4];


    /* enable the ICM and wait ICM CTRL is write only - don't OR it */
    ICM_REGS->ICM_CTRL = ICM_CTRL_ENABLE(1);
    while (!(ICM_REGS->ICM_ISR & ICM_ISR_RHC_Msk))
    {
        /* wait - when finished digest will be updated */
    }
    ICM_REGS->ICM_CTRL = ICM_CTRL_DISABLE(1);
    ICM_REGS->ICM_CTRL = ICM_CTRL_SWRST(1);

    SYS_CACHE_InvalidateDCache_by_Addr((uint32_t *)&(actDigest), SHA_DIGEST_SIZE);
        /* memory barrier */
    __DMB();

	memcpy(sha->digest, &actDigest, sizeof(sha->digest));
    return 0;
}



/* From the crypto DataAdd */
int wc_ShaUpdate(Sha* sha, const byte* data, word32 len)
{
    uint32_t fill;
    uint32_t left;
    uint32_t result = 0;

    left = sha->total_len & 0x3F;
    fill = SHA_BLOCK_SIZE - left;
    sha->total_len += len;

    if (left && len >= fill)
    {
        memcpy((void *)(sha->buffer + left), data, fill);
        memcpy(&actBuffer, &(sha->buffer), sizeof(sha->buffer));
        result = CRYPT_SHA1_Process(sha, actBuffer, SHA_BLOCK_SIZE);
        data += fill;
        len -= fill;
        left = 0;
    }

    if (len >= SHA_BLOCK_SIZE)
    {
        if ((((uint32_t)data) & 63) != 0)
        {
            // Data is not aligned!
            while (len >= SHA_BLOCK_SIZE)
            {
                memcpy(&actBuffer, data, SHA_BLOCK_SIZE);
                result = CRYPT_SHA1_Process(sha, actBuffer, SHA_BLOCK_SIZE);
                data+=64;
                len -=64;                
            }
        }
        else {
        result = CRYPT_SHA1_Process(sha, data, len & 0xFFFFFFC0);
        data += (len & 0xFFFFFFC0);
        len &= 0x3F;
		}
    }

    if( len > 0 )
    {
        memcpy((void *)(sha->buffer + left), data, len);
    }

    return result;
}




static const uint8_t sha_padding[SHA_BLOCK_SIZE] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



int wc_ShaFinal(Sha* sha, byte* hash)
{
    uint32_t last;
    uint8_t  padn;
    uint8_t  msg_len[8];

    /* Get the number of bits */
    /* create the message bit length block */
    uint64_t total_bits = sha->total_len << 3;
    msg_len[0] = (uint8_t)(total_bits >> 56);
    msg_len[1] = (uint8_t)(total_bits >> 48);
    msg_len[2] = (uint8_t)(total_bits >> 40);
    msg_len[3] = (uint8_t)(total_bits >> 32);
    msg_len[4] = (uint8_t)(total_bits >> 24);
    msg_len[5] = (uint8_t)(total_bits >> 16);
    msg_len[6] = (uint8_t)(total_bits >>  8);
    msg_len[7] = (uint8_t)(total_bits);

    last = sha->total_len & 0x3F;
    padn = (last < SHA_PAD_SIZE) ? (SHA_PAD_SIZE - last) : (120 - last);

    wc_ShaUpdate(sha, sha_padding, padn);

    wc_ShaUpdate(sha, msg_len, 8);

    memcpy(hash, sha->digest, SHA_DIGEST_SIZE);

    return wc_InitSha_ex(sha, NULL, 0);
}

int wc_ShaFinalRaw(Sha* sha, byte* hash)
{
    return wc_ShaFinal(sha, hash);
}

#endif /* NO_SHA */
#endif
