/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    arc4.h

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



#ifndef WOLF_CRYPT_ARC4_H
#define WOLF_CRYPT_ARC4_H

#include "crypto/src/types.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

enum {
	ARC4_ENC_TYPE   = 4,    /* cipher unique type */
    ARC4_STATE_SIZE = 256,
    RC4_KEY_SIZE    = 16,   /* always 128bit           */
};

/* ARC4 encryption and decryption */
typedef struct Arc4 {
    byte x;
    byte y;
    byte state[ARC4_STATE_SIZE];
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
    void* heap;
} Arc4;

WOLFSSL_API int wc_Arc4Process(Arc4*, byte*, const byte*, word32);
WOLFSSL_API int wc_Arc4SetKey(Arc4*, const byte*, word32);

WOLFSSL_API int  wc_Arc4Init(Arc4*, void*, int);
WOLFSSL_API void wc_Arc4Free(Arc4*);

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* WOLF_CRYPT_ARC4_H */

