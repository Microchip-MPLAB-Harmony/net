/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_sha1_sam6156.c

  Summary:
    Crypto Framework Library source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic
	Framework Library for SAMA5D2 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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

/** \addtogroup sha_module Working with SHA
 * \ingroup peripherals_module
 * The SHA driver provides the interface to configure and use the SHA
 * peripheral.
 * \n
 *
 * The Secure Hash Algorithm (SHA) module requires a padded message
 * according to FIPS180-2 specification. The first block of the
 * message must be indicated to the module by a specific command. The
 * SHA module produces a N-bit message digest each time a block is
 * written and processing period ends. N is 160 for SHA1, 224 for
 * SHA224, 256 for SHA256, 384 for SHA384, 512 for SHA512.
 *
 * To Enable a SHA encryption and decrypt,the user has to follow these
 * few steps:
 * <ul>
 * <li> Configure SHA algorithm mode, key mode, start mode and
 * operation mode by sha_configure(). </li>
 * <li> Set sha_first_block() to indicates that the next block to
 * process is the first one of a message.</li>
 * <li> Input data for encryption by sha_set_input(). </li>
 * <li> To start the encryption process with sha_start()</li>
 * <li> To get the encryption result by sha_get_output() </li>
 * </ul>
 *
 * For more accurate information, please look at the SHA section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref sha.c\n
 * \ref sha.h\n
 */
/*@{*/
/*@}*/

/**
 * \file
 *
 * Implementation of Secure Hash Algorithm (SHA)
 *
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif
#include "configuration.h"
#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO) && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_6156) && defined(WOLFSSL_HAVE_MCHP_HW_SHA1)
#include "wolfssl/wolfcrypt/settings.h"

#include "wolfssl/wolfcrypt/sha.h"

#if !defined(NO_SHA)

#include "definitions.h"
#include "system/cache/sys_cache.h"

#include "wolfssl/wolfcrypt/logging.h"
#include "wolfssl/wolfcrypt/error-crypt.h"

#include "wolfssl/wolfcrypt/port/pic32/crypt_sha1_hw.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_sha_sam6156.h"

int wc_InitSha_ex(wc_Sha* sha, void* heap, int devId)
{
  memset(sha, 0, sizeof(wc_Sha));
  sha->sha_descriptor.ALGO = SHA_HW_SHA1;
  
  sha->digest[0] = 0x01234567;
  //sha->digest[0] = 0x67452301;
  sha->digest[1] = 0x89ABCDEF;
  //sha->digest[1] = 0xefcdab89;
  sha->digest[2] = 0xFEDCBA98;
  //sha->digest[2] = 0x98badcfe;
  sha->digest[3] = 0x76543210;
  //sha->digest[3] = 0x10325476;
  sha->digest[4] = 0xF0E1D2C3;
  //sha->digest[4] = 0xc3d2e1f0;
  
  sha->total_len = 0;
  
  return 0;
}

static int CRYPT_SHA_Process(wc_Sha* sha, const byte* data, word32 len)
{
  //volatile CRYPT_SHA_SAM6156_SHA_CR * shaCr = (CRYPT_SHA_SAM6156_SHA_CR *)(&(SHA_REGS->SHA_CR));
  //volatile CRYPT_SHA_SAM6156_SHA_MR * shaMr = (CRYPT_SHA_SAM6156_SHA_MR *)(&(SHA_REGS->SHA_MR));
  
  CRYPT_SHA_SAM6156_SHA_CR shaCr = {0};
  CRYPT_SHA_SAM6156_SHA_MR shaMr = {0};
  
  volatile CRYPT_SHA_SAM6156_SHA_ISR * shaIsr = (CRYPT_SHA_SAM6156_SHA_ISR*)(&(SHA_REGS->SHA_ISR));
#if defined(_SAMRH71_SHA_COMPONENT_H_)
  volatile uint32_t * ioPtr = &(SHA_REGS->SHA_IODATAR0);
  volatile uint32_t * iPtr = &(SHA_REGS->SHA_IDATAR0);
#else
  
  volatile uint32_t * ioPtr = &(SHA_REGS->SHA_IODATAR[0]);
  volatile uint32_t * iPtr = &(SHA_REGS->SHA_IDATAR[0]);
#endif
  
  shaCr.s.SWRST = 1;
  SHA_REGS->SHA_CR = shaCr.v;
  shaCr.s.SWRST = 0;
  

  shaMr.s.SMOD = SHA_HW_AUTO_START;
  shaMr.s.PROCDLY = 0;
  shaMr.s.UIHV = 1;  //IV is coming from IR0
  shaMr.s.ALGO = sha->sha_descriptor.ALGO;
  shaMr.s.DUALBUFF = 0;
  shaMr.s.CHECK = 0;

  SHA_REGS->SHA_MR = shaMr.v;
  
  shaCr.s.WUIHV = 1; //Load into IR0
  SHA_REGS->SHA_CR = shaCr.v;
  
  // Load in the IV
  for (uint32_t x = 0; x < (SHA_DIGEST_SIZE >> 2); x++)
  {
    iPtr[x] = sha->digest[x];      
  }

  shaCr.s.WUIHV = 0; //Load into Data registers
  shaCr.s.FIRST = 1; //First message, assume this will load the user IV
  SHA_REGS->SHA_CR = shaCr.v;
  shaCr.s.FIRST = 0; //First message, assume this will load the user IV
  

  uint8_t *ptr = (uint8_t *)(data);
  
  for (uint32_t x = 0; x < len; x+=SHA_BLOCK_SIZE)
  {
      for (uint8_t y = 0; y < (SHA_BLOCK_SIZE >> 2); y++)
      {
          uint32_t data = ((ptr[0] & 0xff)) |
                          ((ptr[1] & 0xff) << 8) |
                          ((ptr[2] & 0xff) << 16) |
                          ((ptr[3] & 0xff) << 24);
          iPtr[y] = data;
          ptr+=4;
      }
      //shaCr.s.START = 1;
      //SHA_REGS->SHA_CR = shaCr.v;
      while (shaIsr->s.DATRDY == 0); // Block until processing is done
  }
  for (uint32_t x = 0; x < (SHA_DIGEST_SIZE >> 2); x++)
  {
    sha->digest[x] = ioPtr[x]; // Save the current digest
  }
  return 0;
  
}


int wc_ShaUpdate(wc_Sha* sha, const byte* data, word32 len)
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
        result = CRYPT_SHA_Process(sha, sha->buffer, SHA_BLOCK_SIZE);
        data += fill;
        len -= fill;
        left = 0;
    }

    if (len >= SHA_BLOCK_SIZE)
    {
        result = CRYPT_SHA_Process(sha, data, len & 0xFFFFFFC0);
        data += (len & 0xFFFFFFC0);
        len &= 0x3F;
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



int wc_ShaFinal(wc_Sha* sha, byte* hash)
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

int wc_ShaFinalRaw(wc_Sha* sha, byte* hash)
{
    return wc_ShaFinal(sha, hash);
}



#endif
#endif
