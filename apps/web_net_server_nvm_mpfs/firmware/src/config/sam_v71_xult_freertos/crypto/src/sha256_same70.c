/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    sha256_same70.c

  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

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




/* code submitted by raphael.huck@efixo.com */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif
#include "configuration.h"
#include "crypto/src/settings.h"

#include "crypto/src/sha256.h"
#include "definitions.h"

#include "core_cm7.h"

#if !defined(NO_SHA256)


#include "crypto/src/logging.h"
#include "crypto/src/error-crypt.h"


#ifdef NO_INLINE
    #include "crypto/src/misc.h"
#else
    #include "crypto/src/misc.c"
#endif




int wc_InitSha256(Sha256* sha256)
{
    /* Enable ICM */
    uint32_t PmcBit = 1u << (ID_ICM - 32);
    if ((PMC_REGS->PMC_PCSR1 & PmcBit) != PmcBit)
    {
        PMC_REGS->PMC_PCER1 = PmcBit;
    }

    
    
    
    sha256->icm_descriptor.cfg.reg = 0x0;
    sha256->icm_descriptor.cfg.bit.is_end_mon = 1;
    sha256->icm_descriptor.cfg.bit.algo = 1;

    
    
    
    /* need initial hash for 256 - byte reversed from internet examples */
    sha256->digest[0] = 0x67E6096A;
    sha256->digest[1] = 0x85AE67BB;
    sha256->digest[2] = 0x72F36E3C;
    sha256->digest[3] = 0x3AF54FA5;
    sha256->digest[4] = 0x7F520E51;
    sha256->digest[5] = 0x8C68059B;
    sha256->digest[6] = 0xABD9831F;
    sha256->digest[7] = 0x19CDE05B;

    sha256->total_len = 0;


    return 0;
}



/* length is in bytes */
static int32_t Sha256Process(Sha256* sha256, const uint8_t *input, word32 length)
{

    ICM_REGS->ICM_CTRL = ICM_CTRL_SWRST(1);
    sha256->icm_descriptor.start_addr = (uint32_t)input;

    /* Transfer size = (tran_size + 1) * 512bits */
    sha256->icm_descriptor.tran_size =  (length >> 6) - 1;
    
    SCB_CleanDCache_by_Addr((uint32_t *)(&(sha256->icm_descriptor)), sizeof(struct icm_descriptor));
    SCB_CleanDCache_by_Addr((uint32_t *)input, length);
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)&(sha256->digest), SHA256_DIGEST_SIZE);


    /* ICM can set up FIPS default starting digest */
    ICM_REGS->ICM_DSCR = (uint32_t)&sha256->icm_descriptor;
    ICM_REGS->ICM_CFG = ICM_CFG_SLBDIS(1) 
                 | ICM_CFG_BBC(0)
                 | ICM_CFG_UALGO_SHA256
                 | ICM_CFG_UIHASH_Msk;

    /* MUST BE ALIGNED at 128! */
    ICM_REGS->ICM_HASH = (uint32_t)(&(sha256->digest));

    ICM_REGS->ICM_UIHVAL[0] = sha256->digest[0];
    ICM_REGS->ICM_UIHVAL[1] = sha256->digest[1];
    ICM_REGS->ICM_UIHVAL[2] = sha256->digest[2];
    ICM_REGS->ICM_UIHVAL[3] = sha256->digest[3];
    ICM_REGS->ICM_UIHVAL[4] = sha256->digest[4];
    ICM_REGS->ICM_UIHVAL[5] = sha256->digest[5];
    ICM_REGS->ICM_UIHVAL[6] = sha256->digest[6];
    ICM_REGS->ICM_UIHVAL[7] = sha256->digest[7];

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

    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)&(sha256->digest), SHA256_DIGEST_SIZE);
    
    return 0;
}




/* len is number of bytes - multiple of 64 only */
int wc_Sha256Update(Sha256* sha256, const byte* data, word32 len)
{
    uint32_t fill;
    uint32_t left;
    int result = 0;

    left = sha256->total_len & 0x3F;
    fill = SHA256_BLOCK_SIZE - left;

    sha256->total_len += len;

    /* buffer is not aligned to 512 bits do the first part */
    if (left && len >= fill)
    {
        memcpy((void *)(sha256->buffer + left), data, fill);
        result = Sha256Process(sha256, sha256->buffer, SHA256_BLOCK_SIZE);
        data += fill;
        len -= fill;
        left = 0;
    }

    /* process a full 64 bytes = 512 bits */
    if (len >= SHA256_BLOCK_SIZE)
    {
        result = Sha256Process(sha256, data, len & 0xFFFFFFC0);
        data += (len & 0xFFFFFFC0);
        len &= 0x3F;
    }

    /* fill in a partial buffer */
    if (len > 0)
    {
        memcpy((void *)(sha256->buffer + left), data, len);
    }

    return result;
}




static const uint8_t sha_padding[SHA256_BLOCK_SIZE * 2] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int wc_Sha256Final(Sha256* sha256, byte* hash)
{
    uint32_t last;
    uint8_t  padn;
    uint8_t  msg_len[8];

    /* Get the number of bits */
    /* create the message bit length block */
    uint64_t total_bits = sha256->total_len << 3;
    msg_len[0] = (uint8_t)(total_bits >> 56);
    msg_len[1] = (uint8_t)(total_bits >> 48);
    msg_len[2] = (uint8_t)(total_bits >> 40);
    msg_len[3] = (uint8_t)(total_bits >> 32);
    msg_len[4] = (uint8_t)(total_bits >> 24);
    msg_len[5] = (uint8_t)(total_bits >> 16);
    msg_len[6] = (uint8_t)(total_bits >>  8);
    msg_len[7] = (uint8_t)(total_bits);
    
    /* pad the last bits with bytes */
    last = sha256->total_len & 0x3F;
    padn = (last < SHA256_PAD_SIZE) ? (SHA256_PAD_SIZE - last) : (120 - last);

    /* future note: Capella will have auto padding with MSGSIZE != 0 */
    wc_Sha256Update(sha256, sha_padding, padn);
    wc_Sha256Update(sha256, msg_len, 8);

    memcpy(hash, (void *)sha256->digest, SHA256_DIGEST_SIZE);

    return wc_InitSha256(sha256);
}



void wc_Sha256Free(Sha256* sha256)
{
    (void)sha256;
}



#endif /* NO_SHA256 */

