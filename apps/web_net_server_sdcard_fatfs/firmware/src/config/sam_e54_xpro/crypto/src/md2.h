/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    md2.h

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

#ifndef WOLF_CRYPT_MD2_H
#define WOLF_CRYPT_MD2_H

#include "crypto/src/types.h"

#ifdef WOLFSSL_MD2

#ifdef __cplusplus
    extern "C" {
#endif

/* in bytes */
enum {
    MD2             =  WC_HASH_TYPE_MD2,
    MD2_BLOCK_SIZE  = 16,
    MD2_DIGEST_SIZE = 16,
    MD2_PAD_SIZE    = 16,
    MD2_X_SIZE      = 48
};


/* Md2 digest */
typedef struct Md2 {
    word32  count;   /* bytes % PAD_SIZE  */
    byte    X[MD2_X_SIZE];
    byte    C[MD2_BLOCK_SIZE];
    byte    buffer[MD2_BLOCK_SIZE];
} Md2;


WOLFSSL_API void wc_InitMd2(Md2*);
WOLFSSL_API void wc_Md2Update(Md2*, const byte*, word32);
WOLFSSL_API void wc_Md2Final(Md2*, byte*);
WOLFSSL_API int  wc_Md2Hash(const byte*, word32, byte*);


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_MD2 */
#endif /* WOLF_CRYPT_MD2_H */

