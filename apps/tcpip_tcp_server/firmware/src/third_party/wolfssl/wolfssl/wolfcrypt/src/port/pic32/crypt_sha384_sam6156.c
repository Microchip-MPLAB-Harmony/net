/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_sha384_sam6156.c

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
 * written and processing period ends. N is 160 for SHA1, 384 for
 * SHA384, 384 for SHA384, 384 for SHA384, 512 for SHA512.
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
#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO) && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_6156) && defined(WOLFSSL_HAVE_MCHP_HW_SHA2128)
#include "wolfssl/wolfcrypt/settings.h"


#if defined(WOLFSSL_SHA384)

#include "wolfssl/wolfcrypt/sha512.h"
#include "system/cache/sys_cache.h"

#include "definitions.h"

#include "wolfssl/wolfcrypt/logging.h"
#include "wolfssl/wolfcrypt/error-crypt.h"

#include "wolfssl/wolfcrypt/port/pic32/crypt_sha384_hw.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_sha_sam6156.h"

int wc_InitSha384_ex(wc_Sha384* sha384, void* heap, int devId)
{
  memset(sha384, 0, sizeof(wc_Sha384));
  sha384->sha_descriptor.ALGO = SHA_HW_SHA384;

#if 0
    sha384->digest[1] = 0x5d9dbbcb; //0xcbbb9d5d;
    sha384->digest[0] = 0xd89e05c1; //0xc1059ed8;
    sha384->digest[3] = 0x2a299a62; //0x629a292a;
    sha384->digest[2] = 0x07d57c36; //0x367cd507;  
    sha384->digest[5] = 0x5a015991; //0x9159015a;
    sha384->digest[4] = 0x17dd7030; //0x3070dd17;  
    sha384->digest[7] = 0xd8ec2f15; //0x152fecd8;
    sha384->digest[6] = 0x39590ef7; //0xf70e5939;  
    sha384->digest[9] = 0x67263367; //0x67332667;
    sha384->digest[8] = 0x310bc0ff; //0xffc00b31;  
    sha384->digest[11] = 0x874ab48e; //0x8eb44a87;
    sha384->digest[10] = 0x11155868; //0x68581511;  
    sha384->digest[13] = 0x0d2e0cdb; //0xdb0c2e0d;
    sha384->digest[12] = 0xa78ff964; //0x64f98fa7;
    sha384->digest[15] = 0x1d48b547; //0x47b5481d;
    sha384->digest[14] = 0xa44ffabe; //0xbefa4fa4;  
#endif
    
  sha384->total_len = 0;
  
  return 0;
}

static int CRYPT_SHA384_Process(wc_Sha384* sha384, const byte* data, word32 len)
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
  if (sha384->digest[0] != 0)
  {
    shaMr.s.UIHV = 1;  //IV is coming from IR0
  }
  shaMr.s.ALGO = sha384->sha_descriptor.ALGO;
  shaMr.s.DUALBUFF = 0;
  shaMr.s.CHECK = 0;

  SHA_REGS->SHA_MR = shaMr.v;
  
  
  if (sha384->digest[0] != 0)
  {
  shaCr.s.WUIHV = 1; //Load into IR0
  SHA_REGS->SHA_CR = shaCr.v;
  
  // Load in the IV
  for (uint32_t x = 0; x < (SHA512_DIGEST_SIZE >> 2); x++)
  {
    iPtr[x] = sha384->digest[x];      
  }

  shaCr.s.WUIHV = 0; //Load into Data registers
  }
  shaCr.s.FIRST = 1; //First message, assume this will load the user IV
  SHA_REGS->SHA_CR = shaCr.v;
  shaCr.s.FIRST = 0; //First message, assume this will load the user IV
  

  uint8_t *ptr = (uint8_t *)(data);
  
  for (uint32_t x = 0; x < len; x+=SHA384_BLOCK_SIZE)
  {
      for (uint8_t y = 0; y < (SHA384_BLOCK_SIZE >> 2); y++)
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
  for (uint32_t x = 0; x < (SHA512_DIGEST_SIZE >> 2); x++)
  {
    sha384->digest[x] = ioPtr[x]; // Save the current digest
  }
  return 0;
  
}


int wc_Sha384Update(wc_Sha384* sha384, const byte* data, word32 len)
{
    uint32_t fill;
    uint32_t left;
    uint32_t result = 0;

    left = sha384->total_len & 0x7F;
    fill = SHA384_BLOCK_SIZE - left;
    sha384->total_len += len;

    if (left && len >= fill)
    {
        memcpy((void *)(sha384->buffer + left), data, fill);
        result = CRYPT_SHA384_Process(sha384, sha384->buffer, SHA384_BLOCK_SIZE);
        data += fill;
        len -= fill;
        left = 0;
    }

    if (len >= SHA384_BLOCK_SIZE)
    {
        result = CRYPT_SHA384_Process(sha384, data, len & 0xFFFFFF80);
        data += (len & 0xFFFFFF80);
        len &= 0x7F;
    }

    if( len > 0 )
    {
        memcpy((void *)(sha384->buffer + left), data, len);
    }

    return result; 
  
}

static const uint8_t sha384_padding[SHA384_BLOCK_SIZE] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



int wc_Sha384Final(wc_Sha384* sha384, byte* hash)
{
    uint32_t last;
    uint8_t  padn;
    uint8_t  msg_len[16] = {0};

    /* Get the number of bits */
    /* create the message bit length block */
    uint64_t total_bits = sha384->total_len << 3;
    msg_len[8] = (uint8_t)(total_bits >> 56);
    msg_len[9] = (uint8_t)(total_bits >> 48);
    msg_len[10] = (uint8_t)(total_bits >> 40);
    msg_len[11] = (uint8_t)(total_bits >> 32);
    msg_len[12] = (uint8_t)(total_bits >> 24);
    msg_len[13] = (uint8_t)(total_bits >> 16);
    msg_len[14] = (uint8_t)(total_bits >>  8);
    msg_len[15] = (uint8_t)(total_bits);

    last = sha384->total_len & 0x7F;
    padn = (last < SHA384_PAD_SIZE) ? (SHA384_PAD_SIZE - last) : (240 - last);

    wc_Sha384Update(sha384, sha384_padding, padn);

    wc_Sha384Update(sha384, msg_len, 16);

    memcpy(hash, sha384->digest, SHA384_DIGEST_SIZE);

    return wc_InitSha384_ex(sha384, NULL, 0);
}
int wc_Sha384FinalRaw(wc_Sha384* sha384, byte* hash)
{
    return wc_Sha384Final(sha384, hash);
}



#endif
#endif
