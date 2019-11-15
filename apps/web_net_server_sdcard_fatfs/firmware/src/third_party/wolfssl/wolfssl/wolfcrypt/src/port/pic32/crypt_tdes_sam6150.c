/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_tdes_sam6150.c

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



/** \addtogroup tdes_module Working with TDES
 * \ingroup peripherals_module
 * The TDES driver provides the interface to configure and use the TDES
 * peripheral.
 * \n
 *
 * The Data Encryption Standard (DES) and the Triple Data Encryption Algorithm
 * (TDES) specify FIPS-approved cryptographic algorithms that can be used to
 * protect electronic data.
 * The TDES bit in the TDES Mode Register (TDES_MR) is used to select either the
 * single DES or the Triple DES mode.
 *
 * The DES uses 64-bit cryptographic keys to encrypt and decrypt 64-bit data
 * blocks.
 * The 64-bit key is defined in the Key 1 Word Registers (TDES_KEY1WRx).
 *
 * The Triple DES key consists in three DES keys, which set is also referred to
 * as a key bundle. These three 64-bit keys are defined, respectively, in the
 * Key 1, 2 and 3 Word Registers (TDES_KEY1WRx, TDES_KEY2WRx and TDES_KEY3WRx).
 * In Triple DES mode (TDES_MR:TDESMOD set to 1), the TDES_MR:KEYMOD bit is used
 * to select either the two- or three-key algorithm.
 *
 * In order to perform TDES encryption/decryption, the user has to follow these
 * few steps:
 * <ul>
 * <li> A software-triggered hardware reset of the TDES interface is performed
 * by CRYPT_TDES_Init(). This also configures algorithm mode, key mode, 
 * start mode and operation mode.</li>
 * <li> Set DES keys by calling CRYPT_DES_SetKey or CRYPT_TDES_SetKey.</li>
 * <li> For CBC, OFB, and CFB modes, set the IV via CRYPT_TDES_SetIV().</li>
 * <li> Encrypt or Decrypt and retrieve the output by calling CRYPT_TDES_Encrypt 
 * or CRYPT_TDES_Decrypt.</li>
 * </ul>
 *
 * For more accurate information, please look at the TDES section of the
 * Datasheet.
 *
 * Related files:\n
 * \ref tdes.c\n
 * \ref tdes.h\n
 */
/*@{*/
/*@}*/
/**
 * \file
 *
 * Implementation of the Triple Data Encryption Algorithm (TDES).
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "configuration.h"
#if defined(WOLFSSL_HAVE_MCHP_3DES) && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_TDES_HW_6150) && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO)
#include "wolfssl/wolfcrypt/port/pic32/crypt_tdes_hwInt.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_tdes_sam6150.h"

#include "definitions.h"


WOLFSSL_API int  wc_Des_SetKey(Des* des, const byte* key,
                               const byte* iv, int dir)
{
  memcpy(des->key, key, 8);
  memcpy(des->reg, iv, 8);
  return 0;
}
void CRYPT_DES_SetIV(Des* des, const byte* iv)
{
  memcpy(des->reg, iv, 8);  
}

static void CRYPT_DES_LoadKeyIv(uint32_t * key, uint32_t * iv)
{
  TDES_REGS->TDES_KEY1WR[0] = key[0];
  TDES_REGS->TDES_KEY1WR[1] = key[1];

  if (iv != NULL)
  {
    TDES_REGS->TDES_IVR[0] = iv[0];
    TDES_REGS->TDES_IVR[1] = iv[1]; 
  }
}

static void CRYPT_DES_ProcessBlocks(uint32_t* out, const uint32_t* in, uint32_t sz)
{
    volatile CRYPTO_TDES_SAM6150_ISR * isr = (CRYPTO_TDES_SAM6150_ISR*)(&TDES_REGS->TDES_ISR);
    uint32_t numBlocks = sz >> 3;
    for (int x = 0; x < numBlocks; x++)
    {
        TDES_REGS->TDES_IDATAR[0] = *in++;
        TDES_REGS->TDES_IDATAR[1] = *in++;
        while(isr->s.DATRDY == 0);
        *out++ = TDES_REGS->TDES_ODATAR[0];
        *out++ = TDES_REGS->TDES_ODATAR[1];
    }
}

WOLFSSL_API int  wc_Des_CbcEncrypt(Des* des, byte* out,
                                   const byte* in, word32 sz)
{
  CRYPTO_TDES_SAM6150_CR cr = {0};
  cr.s.SWRST = 1;
  TDES_REGS->TDES_CR = cr.v;
  cr.v = 0;
  
  CRYPTO_TDES_SAM6150_MR mr = {0};
  mr.s.CIPHER = 1;
  mr.s.TDESMOD = CRYPTO_TDES_SAM6150_SINGLE_DES;
  mr.s.KEYMOD = 0;
  mr.s.SMOD = CRYPTO_TDES_SAM6150_AUTO_START;
  mr.s.OPMOD = CRYPTO_TDES_SAM6150_CBC;
  
  TDES_REGS->TDES_MR = mr.v;
  
  CRYPT_DES_LoadKeyIv((uint32_t*)(&(des->key[0])), (uint32_t*)(&(des->reg[0])));

  CRYPT_DES_ProcessBlocks((uint32_t*)out, (const uint32_t*)in, sz);
  
  memcpy(des->reg, &(out[sz-8]), 8);
  
  return 0;
}

WOLFSSL_API int  wc_Des_CbcDecrypt(Des* des, byte* out,
                                   const byte* in, word32 sz)
{
  CRYPTO_TDES_SAM6150_CR cr = {0};
  cr.s.SWRST = 1;
  TDES_REGS->TDES_CR = cr.v;
  cr.v = 0;
  
  CRYPTO_TDES_SAM6150_MR mr = {0};
  mr.s.CIPHER = 0;
  mr.s.TDESMOD = CRYPTO_TDES_SAM6150_SINGLE_DES;
  mr.s.KEYMOD = 0;
  mr.s.SMOD = CRYPTO_TDES_SAM6150_AUTO_START;
  mr.s.OPMOD = CRYPTO_TDES_SAM6150_CBC;
  
  TDES_REGS->TDES_MR = mr.v;
  
  CRYPT_DES_LoadKeyIv((uint32_t*)(&(des->key[0])), (uint32_t *)(&(des->reg[0])));

  CRYPT_DES_ProcessBlocks((uint32_t*)out, (const uint32_t*)in, sz);
  
  memcpy(des->reg, &(in[sz-8]), 8);
  return 0;
}

int  CRYPT_DES_EcbEncrypt(Des* des, byte* out, const byte* in, word32 sz)
{
  CRYPTO_TDES_SAM6150_CR cr = {0};
  cr.s.SWRST = 1;
  TDES_REGS->TDES_CR = cr.v;
  cr.v = 0;
  
  CRYPTO_TDES_SAM6150_MR mr = {0};
  mr.s.CIPHER = 1;
  mr.s.TDESMOD = CRYPTO_TDES_SAM6150_SINGLE_DES;
  mr.s.KEYMOD = 0;
  mr.s.SMOD = CRYPTO_TDES_SAM6150_AUTO_START;
  mr.s.OPMOD = CRYPTO_TDES_SAM6150_ECB;
  
  TDES_REGS->TDES_MR = mr.v;
  
  CRYPT_DES_LoadKeyIv(&des->key[0], NULL);

  CRYPT_DES_ProcessBlocks((uint32_t*)out, (const uint32_t*)in, sz);
  
  

  return 0;
}



static void CRYPT_DES3_LoadKeyIv(uint32_t * key, uint32_t * iv)
{
  TDES_REGS->TDES_KEY1WR[0] = key[0];
  TDES_REGS->TDES_KEY1WR[1] = key[1];
  TDES_REGS->TDES_KEY2WR[0] = key[2];
  TDES_REGS->TDES_KEY2WR[1] = key[3];
  TDES_REGS->TDES_KEY3WR[0] = key[4];
  TDES_REGS->TDES_KEY3WR[1] = key[5];

  if (iv != NULL)
  {
    TDES_REGS->TDES_IVR[0] = iv[0];
    TDES_REGS->TDES_IVR[1] = iv[1]; 
  }
}


int  CRYPT_DES3_EcbEncrypt(Des3* des, byte* out, const byte* in, word32 sz)
{
  CRYPTO_TDES_SAM6150_CR cr = {0};
  cr.s.SWRST = 1;
  TDES_REGS->TDES_CR = cr.v;
  cr.v = 0;
  
  CRYPTO_TDES_SAM6150_MR mr = {0};
  mr.s.CIPHER = 1;
  mr.s.TDESMOD = CRYPTO_TDES_SAM6150_TRIPLE_DES;
  mr.s.KEYMOD = 0;
  mr.s.SMOD = CRYPTO_TDES_SAM6150_AUTO_START;
  mr.s.OPMOD = CRYPTO_TDES_SAM6150_ECB;
  
  TDES_REGS->TDES_MR = mr.v;
  
  CRYPT_DES3_LoadKeyIv(des->key[0], NULL);

  CRYPT_DES_ProcessBlocks((uint32_t*)out, (const uint32_t*)in, sz);
  return 0;
}

WOLFSSL_API int  wc_Des3_SetKey(Des3* des, const byte* key,
                                const byte* iv,int dir)
{
  memcpy(des->key, key, 24);
  memcpy(des->reg, iv, 8);  
  return 0;
}

int  CRYPT_DES3_SetIV(Des3* des, const byte* iv)
{
  memcpy(des->reg, iv, 8);    
  return 0;
}

WOLFSSL_API int  wc_Des3_CbcEncrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz)
{
  CRYPTO_TDES_SAM6150_CR cr = {0};
  cr.s.SWRST = 1;
  TDES_REGS->TDES_CR = cr.v;
  cr.v = 0;
  
  CRYPTO_TDES_SAM6150_MR mr = {0};
  mr.s.CIPHER = 1;
  mr.s.TDESMOD = CRYPTO_TDES_SAM6150_TRIPLE_DES;
  mr.s.KEYMOD = 0;
  mr.s.SMOD = CRYPTO_TDES_SAM6150_AUTO_START;
  mr.s.OPMOD = CRYPTO_TDES_SAM6150_CBC;
  
  TDES_REGS->TDES_MR = mr.v;
  
  CRYPT_DES3_LoadKeyIv(des->key[0], des->reg);

  CRYPT_DES_ProcessBlocks((uint32_t*)out, (const uint32_t*)in, sz);
  
  memcpy(des->reg, &(out[sz-8]), 8);
  
  return 0;
}

WOLFSSL_API int  wc_Des3_CbcDecrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz)
{
  CRYPTO_TDES_SAM6150_CR cr = {0};
  cr.s.SWRST = 1;
  TDES_REGS->TDES_CR = cr.v;
  cr.v = 0;
  
  CRYPTO_TDES_SAM6150_MR mr = {0};
  mr.s.CIPHER = 0;
  mr.s.TDESMOD = CRYPTO_TDES_SAM6150_TRIPLE_DES;
  mr.s.KEYMOD = 0;
  mr.s.SMOD = CRYPTO_TDES_SAM6150_AUTO_START;
  mr.s.OPMOD = CRYPTO_TDES_SAM6150_CBC;
  
  TDES_REGS->TDES_MR = mr.v;
  
  CRYPT_DES3_LoadKeyIv(des->key[0], des->reg);

  CRYPT_DES_ProcessBlocks((uint32_t*)out, (const uint32_t*)in, sz);
  
  memcpy(des->reg, &(in[sz-8]), 8);
  return 0;
}

int  CRYPT_DES3_Init(Des3* des, void* heap, int id)
{
  memset(des, 0, sizeof(Des3));
  return 0;
}

void CRYPT_DES3_Free(Des3* des)
{
}


/*----------------------------------------------------------------------------
 * Functions used by both TDES and DES 
 *----------------------------------------------------------------------------*/


#endif
