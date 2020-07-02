/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_sha1_hw.h

  Summary:
    Crypto Framework Libarary interface file for hardware AES

  Description:
    This file contains the interface that is required to be implemented by
    the AES hardware driver..
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2019 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _CRYPTO_AES_HW_INT_H_
#define _CRYPTO_AES_HW_INT_H_

#include "configuration.h"
#include "wolfssl/wolfcrypt/aes.h"

#ifdef __cplusplus
extern "C" {
#endif

int CRYPT_AES_SetKey(Aes* aes, const byte* userKey, word32 keylen, const byte* iv, int dir);
int CRYPT_AES_CbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz);
int CRYPT_AES_CbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz);
int CRYPT_AES_CtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz);

void CRYPT_AES_Free(Aes* aes);
int  CRYPT_AES_Init(Aes* aes, void* heap, int devId);

#if defined(HAVE_AESGCM)
 int CRYPT_AES_GcmEncrypt(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz);
 int CRYPT_AES_GcmDecrypt(Aes* aes, unsigned char* out,
                                   const unsigned char* in, unsigned int sz,
                                   const unsigned char* iv, unsigned int ivSz,
                                   const unsigned char* authTag, unsigned int authTagSz,
                                   const unsigned char* authIn, unsigned int authInSz);

#endif

#ifdef __cplusplus
}
#endif


#endif
