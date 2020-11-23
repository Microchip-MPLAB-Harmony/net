/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_sha256_sam6156.c

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
 * written and processing period ends. N is 160 for SHA1, 256 for
 * SHA256, 256 for SHA256, 384 for SHA384, 512 for SHA512.
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
#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO) && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_6156) && defined(WOLFSSL_HAVE_MCHP_HW_SHA264)
#include "wolfssl/wolfcrypt/settings.h"


#if !defined(NO_SHA256)

#include "wolfssl/wolfcrypt/sha256.h"
#include "system/cache/sys_cache.h"

#include "definitions.h"

#include "wolfssl/wolfcrypt/logging.h"
#include "wolfssl/wolfcrypt/error-crypt.h"

#include "wolfssl/wolfcrypt/port/pic32/crypt_sha256_hw.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_sha_sam6156.h"

int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
{
  memset(sha256, 0, sizeof(wc_Sha256));
  sha256->sha_descriptor.ALGO = SHA_HW_SHA256;
  
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

static int CRYPT_SHA256_Process(wc_Sha256* sha256, const byte* data, word32 len)
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
  shaMr.s.ALGO = sha256->sha_descriptor.ALGO;
  shaMr.s.DUALBUFF = 0;
  shaMr.s.CHECK = 0;

  SHA_REGS->SHA_MR = shaMr.v;
  
  shaCr.s.WUIHV = 1; //Load into IR0
  SHA_REGS->SHA_CR = shaCr.v;
  
  // Load in the IV
  for (uint32_t x = 0; x < (SHA256_DIGEST_SIZE >> 2); x++)
  {
    iPtr[x] = sha256->digest[x];      
  }

  shaCr.s.WUIHV = 0; //Load into Data registers
  shaCr.s.FIRST = 1; //First message, assume this will load the user IV
  SHA_REGS->SHA_CR = shaCr.v;
  shaCr.s.FIRST = 0; //First message, assume this will load the user IV
  

  uint8_t *ptr = (uint8_t *)(data);
  
  for (uint32_t x = 0; x < len; x+=SHA256_BLOCK_SIZE)
  {
      for (uint8_t y = 0; y < (SHA256_BLOCK_SIZE >> 2); y++)
      {
          uint32_t datay = ((ptr[0] & 0xff)) |
                           ((ptr[1] & 0xff) << 8) |
                           ((ptr[2] & 0xff) << 16) |
                           ((ptr[3] & 0xff) << 24);
          iPtr[y] = datay;
          ptr+=4;
      }
      //shaCr.s.START = 1;
      //SHA_REGS->SHA_CR = shaCr.v;
      while (shaIsr->s.DATRDY == 0); // Block until processing is done
  }
  for (uint32_t x = 0; x < (SHA256_DIGEST_SIZE >> 2); x++)
  {
    sha256->digest[x] = ioPtr[x]; // Save the current digest
  }
  return 0;
  
}


int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
{
    uint32_t fill;
    uint32_t left;
    uint32_t result = 0;

    left = sha256->total_len & 0x3F;
    fill = SHA256_BLOCK_SIZE - left;
    sha256->total_len += len;

    if (left && len >= fill)
    {
        memcpy((void *)(sha256->buffer + left), data, fill);
        result = CRYPT_SHA256_Process(sha256, sha256->buffer, SHA256_BLOCK_SIZE);
        data += fill;
        len -= fill;
        left = 0;
    }

    if (len >= SHA256_BLOCK_SIZE)
    {
        result = CRYPT_SHA256_Process(sha256, data, len & 0xFFFFFFC0);
        data += (len & 0xFFFFFFC0);
        len &= 0x3F;
    }

    if( len > 0 )
    {
        memcpy((void *)(sha256->buffer + left), data, len);
    }

    return result; 
  
}

static const uint8_t sha256_padding[SHA256_BLOCK_SIZE] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
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

    last = sha256->total_len & 0x3F;
    padn = (last < SHA256_PAD_SIZE) ? (SHA256_PAD_SIZE - last) : (120 - last);

    wc_Sha256Update(sha256, sha256_padding, padn);

    wc_Sha256Update(sha256, msg_len, 8);

    memcpy(hash, sha256->digest, SHA256_DIGEST_SIZE);

    return wc_InitSha256_ex(sha256, NULL, 0);
}
int wc_Sha256FinalRaw(wc_Sha256* sha256, byte* hash)
{
    return wc_Sha256Final(sha256, hash);
}



#endif
#endif
