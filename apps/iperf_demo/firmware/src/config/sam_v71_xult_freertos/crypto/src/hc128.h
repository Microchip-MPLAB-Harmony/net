/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    hc128.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
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









//DOM-IGNORE-END


#ifndef WOLF_CRYPT_HC128_H
#define WOLF_CRYPT_HC128_H

#include "crypto/src/types.h"

#ifndef NO_HC128

#ifdef __cplusplus
    extern "C" {
#endif

enum {
	HC128_ENC_TYPE =  WC_CIPHER_HC128,     /* cipher unique type */
};

/* HC-128 stream cipher */
typedef struct HC128 {
    word32 T[1024];             /* P[i] = T[i];  Q[i] = T[1024 + i ]; */
    word32 X[16];
    word32 Y[16];
    word32 counter1024;         /* counter1024 = i mod 1024 at the ith step */
    word32 key[8];
    word32 iv[8];
#ifdef XSTREAM_ALIGN
    void*  heap;  /* heap hint, currently XMALLOC only used with aligning */
#endif
} HC128;


WOLFSSL_API int wc_Hc128_Process(HC128*, byte*, const byte*, word32);
WOLFSSL_API int wc_Hc128_SetKey(HC128*, const byte* key, const byte* iv);

WOLFSSL_LOCAL int wc_Hc128_SetHeap(HC128* ctx, void* heap);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_HC128 */
#endif /* WOLF_CRYPT_HC128_H */

