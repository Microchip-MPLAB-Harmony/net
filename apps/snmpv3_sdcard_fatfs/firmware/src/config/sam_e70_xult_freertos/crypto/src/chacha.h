/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    chacha.h
  
  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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



#ifndef WOLF_CRYPT_CHACHA_H
#define WOLF_CRYPT_CHACHA_H

#include "crypto/src/types.h"

#ifdef HAVE_CHACHA

#ifdef __cplusplus
    extern "C" {
#endif

/* Size of the IV */
#define CHACHA_IV_WORDS    3
#define CHACHA_IV_BYTES    (CHACHA_IV_WORDS * sizeof(word32))

/* Size of ChaCha chunks */
#define CHACHA_CHUNK_WORDS 16
#define CHACHA_CHUNK_BYTES (CHACHA_CHUNK_WORDS * sizeof(word32))

enum {
	CHACHA_ENC_TYPE = 7     /* cipher unique type */
};

typedef struct ChaCha {
    word32 X[CHACHA_CHUNK_WORDS];           /* state of cipher */
} ChaCha;

/**
  * IV(nonce) changes with each record
  * counter is for what value the block counter should start ... usually 0
  */
WOLFSSL_API int wc_Chacha_SetIV(ChaCha* ctx, const byte* inIv, word32 counter);

WOLFSSL_API int wc_Chacha_Process(ChaCha* ctx, byte* cipher, const byte* plain,
                              word32 msglen);
WOLFSSL_API int wc_Chacha_SetKey(ChaCha* ctx, const byte* key, word32 keySz);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_CHACHA */
#endif /* WOLF_CRYPT_CHACHA_H */

