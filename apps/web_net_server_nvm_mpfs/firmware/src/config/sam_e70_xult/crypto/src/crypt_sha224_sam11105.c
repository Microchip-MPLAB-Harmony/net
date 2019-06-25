/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    sha224_same70.c

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
#include "crypto/src/settings.h"

#include "crypto/src/sha256.h"
#include "definitions.h"
#include "system/cache/sys_cache.h"

#if !defined(NO_SHA224)


#include "crypto/src/logging.h"
#include "crypto/src/error-crypt.h"


#ifdef NO_INLINE
    #include "crypto/src/misc.h"
#else
    #include "crypto/src/misc.c"
#endif

#include "crypt_sha224_hw.h"

struct icm_descriptor actIcmDescriptor __attribute__((aligned (64)));
uint8_t  actBuffer[SHA224_BLOCK_SIZE] __attribute__((aligned (64)));  /* 64 bytes = 512 bits */
uint32_t actDigest[(SHA224_DIGEST_SIZE/4)+1] __attribute__((aligned (128)));


int CRYPT_SHA224_InitSha(crypt_sha224_hw_descriptor* sha224, void* heap, int devId)
{
    /* Enable ICM */
#if !defined(CRYPTO_SHA_HW_U2010)
    uint32_t PmcBit = 1u << (ID_ICM - 32);
    if ((PMC_REGS->PMC_PCSR1 & PmcBit) != PmcBit)
    {
        PMC_REGS->PMC_PCER1 = PmcBit;
    }
#else
    uint32_t apbcBit = 1u << (ID_ICM&31);
    if ((MCLK_REGS->MCLK_APBCMASK & apbcBit) != apbcBit)
    {
        MCLK_REGS->MCLK_APBCMASK |= apbcBit;
    }
#endif
    
    
    
    sha224->icm_descriptor.cfg.reg = 0x0;
    sha224->icm_descriptor.cfg.bit.is_end_mon = 1;
    sha224->icm_descriptor.cfg.bit.algo = 1;

    
    
    
    /* need initial hash for 224 - byte reversed from internet examples */

    sha224->digest[0] = 0xd89e05c1; //0xc1059ed8;
    sha224->digest[1] = 0x07d57c36; //0x367cd507;
    sha224->digest[2] = 0x17dd7030; //0x3070dd17;
    sha224->digest[3] = 0x39590ef7; //0xf70e5939;
    sha224->digest[4] = 0x310bc0ff; //0xffc00b31;
    sha224->digest[5] = 0x11155868; //0x68581511;
    sha224->digest[6] = 0xa78ff964; //0x64f98fa7;
    sha224->digest[7] = 0xa44ffabe; //0xbefa4fa4;
    
    
    sha224->total_len = 0;


    return 0;
}



/* length is in bytes */
static int32_t CRYPT_SHA224_Process(crypt_sha224_hw_descriptor* sha224, const uint8_t *input, word32 length)
{

    ICM_REGS->ICM_CTRL = ICM_CTRL_SWRST(1);
    sha224->icm_descriptor.start_addr = (uint32_t)input;

    /* Transfer size = (tran_size + 1) * 512bits */
    sha224->icm_descriptor.tran_size =  (length >> 6) - 1;

    memcpy(&actIcmDescriptor, &(sha224->icm_descriptor), sizeof(sha224->icm_descriptor));
    
    
    SYS_CACHE_CleanDCache_by_Addr((uint32_t*) (&actIcmDescriptor), sizeof(sha224->icm_descriptor));
    SYS_CACHE_CleanDCache_by_Addr((uint32_t *)input, length);
    SYS_CACHE_CleanInvalidateDCache_by_Addr((uint32_t *)&(sha224->digest), SHA224_DIGEST_SIZE + 4);


    /* ICM can set up FIPS default starting digest */
    ICM_REGS->ICM_DSCR = (uint32_t)&(actIcmDescriptor);
    ICM_REGS->ICM_CFG = ICM_CFG_SLBDIS(1) 
                 | ICM_CFG_BBC(0)
                 | ICM_CFG_UALGO_SHA256
                 | ICM_CFG_UIHASH_Msk;

    /* MUST BE ALIGNED at 128! */
    ICM_REGS->ICM_HASH = (uint32_t)(&(actDigest));

    ICM_REGS->ICM_UIHVAL[0] = sha224->digest[0];
    ICM_REGS->ICM_UIHVAL[1] = sha224->digest[1];
    ICM_REGS->ICM_UIHVAL[2] = sha224->digest[2];
    ICM_REGS->ICM_UIHVAL[3] = sha224->digest[3];
    ICM_REGS->ICM_UIHVAL[4] = sha224->digest[4];
    ICM_REGS->ICM_UIHVAL[5] = sha224->digest[5];
    ICM_REGS->ICM_UIHVAL[6] = sha224->digest[6];
    ICM_REGS->ICM_UIHVAL[7] = sha224->digest[7];

    /* memory barrier - sync all before enable ICM */
    __DMB();

    ICM_REGS->ICM_IER = ICM_IER_RHC(1);
    
    /* enable the ICM and wait */
    ICM_REGS->ICM_CTRL = ICM_CTRL_ENABLE(1);
    while (!(ICM_REGS->ICM_ISR & ICM_ISR_RHC_Msk))
    {
        /* wait - when finished digest will be updated */
    }
    
    ICM_REGS->ICM_CTRL = ICM_CTRL_DISABLE(1);
    ICM_REGS->ICM_CTRL = ICM_CTRL_SWRST(1);

    SYS_CACHE_CleanInvalidateDCache_by_Addr((uint32_t*) (&actDigest), SHA224_DIGEST_SIZE + 4);
    memcpy(sha224->digest, &actDigest, sizeof(sha224->digest));
    
    return 0;
}




/* len is number of bytes - multiple of 64 only */
int CRYPT_SHA224_Update(crypt_sha224_hw_descriptor* sha224, const byte* data, word32 len)
{
    uint32_t fill;
    uint32_t left;
    int result = 0;

    left = sha224->total_len & 0x3F;
    fill = SHA224_BLOCK_SIZE - left;

    sha224->total_len += len;

    /* buffer is not aligned to 512 bits do the first part */
    if (left && len >= fill)
    {
        memcpy((void *)(sha224->buffer + left), data, fill);
        memcpy(&actBuffer, &(sha224->buffer), sizeof(sha224->buffer));

        result = CRYPT_SHA224_Process(sha224, actBuffer, SHA224_BLOCK_SIZE);
        data += fill;
        len -= fill;
        left = 0;
    }

    /* process a full 64 bytes = 512 bits */
    if (len >= SHA224_BLOCK_SIZE)
    {
        if ((((uint32_t)data) & 63) != 0)
        {
            // Data is not aligned!
            while (len >= SHA224_BLOCK_SIZE)
            {
                memcpy(&actBuffer, data, SHA224_BLOCK_SIZE);
                result = CRYPT_SHA224_Process(sha224, actBuffer, SHA224_BLOCK_SIZE);
                data+=64;
                len -=64;                
            }
        }
        else {
        
            result = CRYPT_SHA224_Process(sha224, data, len & 0xFFFFFFC0);
            data += (len & 0xFFFFFFC0);
            len &= 0x3F;
        }
    }

    /* fill in a partial buffer */
    if (len > 0)
    {
        memcpy((void *)(sha224->buffer + left), data, len);
    }

    return result;
}




static const uint8_t sha_padding[SHA224_BLOCK_SIZE * 2] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int CRYPT_SHA224_Final(crypt_sha224_hw_descriptor* sha224, byte* hash)
{
    uint32_t last;
    uint8_t  padn;
    uint8_t  msg_len[8];

    /* Get the number of bits */
    /* create the message bit length block */
    uint64_t total_bits = sha224->total_len << 3;
    msg_len[0] = (uint8_t)(total_bits >> 56);
    msg_len[1] = (uint8_t)(total_bits >> 48);
    msg_len[2] = (uint8_t)(total_bits >> 40);
    msg_len[3] = (uint8_t)(total_bits >> 32);
    msg_len[4] = (uint8_t)(total_bits >> 24);
    msg_len[5] = (uint8_t)(total_bits >> 16);
    msg_len[6] = (uint8_t)(total_bits >>  8);
    msg_len[7] = (uint8_t)(total_bits);
    
    /* pad the last bits with bytes */
    last = sha224->total_len & 0x3F;
    padn = (last < SHA224_PAD_SIZE) ? (SHA224_PAD_SIZE - last) : (120 - last);

    /* future note: Capella will have auto padding with MSGSIZE != 0 */
    CRYPT_SHA224_Update(sha224, sha_padding, padn);
    CRYPT_SHA224_Update(sha224, msg_len, 8);

    memcpy(hash, (void *)sha224->digest, SHA224_DIGEST_SIZE);

    return CRYPT_SHA224_InitSha(sha224, NULL, 0);
}



void CRYPT_SHA224_Free(crypt_sha224_hw_descriptor* sha224)
{
    (void)sha224;
}

int CRYPT_SHA224_FinalRaw(crypt_sha224_hw_descriptor* sha224, byte* hash)
{
    return CRYPT_SHA224_Final(sha224, hash);
}

#endif /* NO_SHA224 */

