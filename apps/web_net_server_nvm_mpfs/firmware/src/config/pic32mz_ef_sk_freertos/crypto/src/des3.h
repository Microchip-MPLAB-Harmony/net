/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    des3.h

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


#ifndef WOLF_CRYPT_DES3_H
#define WOLF_CRYPT_DES3_H

#include "crypto/src/types.h"

#ifndef NO_DES3

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include <wolfssl/wolfcrypt/fips.h>
#endif /* HAVE_FIPS_VERSION >= 2 */

#if defined(HAVE_FIPS) && \
	(!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))
/* included for fips @wc_fips */
#include "crypto/src/des3.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* these are required for FIPS and non-FIPS */
enum {
    DES_KEY_SIZE        =  8,  /* des                     */
    DES3_KEY_SIZE       = 24,  /* 3 des ede               */
    DES_IV_SIZE         =  8,  /* should be the same as DES_BLOCK_SIZE */
};


/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

enum {
    DES_ENC_TYPE    = WC_CIPHER_DES,     /* cipher unique type */
    DES3_ENC_TYPE   = WC_CIPHER_DES3,    /* cipher unique type */

    DES_BLOCK_SIZE  = 8,
    DES_KS_SIZE     = 32,    /* internal DES key buffer size */

    DES_ENCRYPTION  = 0,
    DES_DECRYPTION  = 1
};

#define DES_IVLEN 8
#define DES_KEYLEN 8
#define DES3_IVLEN 8
#define DES3_KEYLEN 24


#if defined(STM32_CRYPTO)
enum {
    DES_CBC = 0,
    DES_ECB = 1
};
#endif

#if defined(HAVE_MICROCHIP_HARMONY3_HW_TDES) && !defined(WOLFSSL_PIC32MZ_CRYPT)
    #include "crypto/src/crypt_tdes_hw.h"
    #define Des crypt_tdes_hw_descriptor
    #define Des3 crypt_tdes_hw_descriptor
#else

/* DES encryption and decryption */
typedef struct Des {
    word32 reg[DES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
    word32 tmp[DES_BLOCK_SIZE / sizeof(word32)];      /* same         */
    word32 key[DES_KS_SIZE];
} Des;



/* DES3 encryption and decryption */
typedef struct Des3 {
    word32 key[3][DES_KS_SIZE];
    word32 reg[DES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
    word32 tmp[DES_BLOCK_SIZE / sizeof(word32)];      /* same         */
#ifdef WOLFSSL_ASYNC_CRYPT
    const byte* key_raw;
    const byte* iv_raw;
    WC_ASYNC_DEV asyncDev;
#endif
    void* heap;
} Des3;
#endif /* HAVE_MICROCHIP_HARMONY3_HW_TDES */
#endif /* HAVE_FIPS */


WOLFSSL_API int  wc_Des_SetKey(Des* des, const byte* key,
                               const byte* iv, int dir);
WOLFSSL_API void wc_Des_SetIV(Des* des, const byte* iv);
WOLFSSL_API int  wc_Des_CbcEncrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);
WOLFSSL_API int  wc_Des_CbcDecrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);
WOLFSSL_API int  wc_Des_EcbEncrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);
WOLFSSL_API int wc_Des3_EcbEncrypt(Des3* des, byte* out,
                                   const byte* in, word32 sz);


/* ECB decrypt same process as encrypt but with decrypt key */
#define wc_Des_EcbDecrypt  wc_Des_EcbEncrypt
#define wc_Des3_EcbDecrypt wc_Des3_EcbEncrypt

WOLFSSL_API int  wc_Des3_SetKey(Des3* des, const byte* key,
                                const byte* iv,int dir);
WOLFSSL_API int  wc_Des3_SetIV(Des3* des, const byte* iv);
WOLFSSL_API int  wc_Des3_CbcEncrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz);
WOLFSSL_API int  wc_Des3_CbcDecrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz);

/* These are only required when using either:
  static memory (WOLFSSL_STATIC_MEMORY) or asynchronous (WOLFSSL_ASYNC_CRYPT) */
WOLFSSL_API int  wc_Des3Init(Des3*, void*, int);
WOLFSSL_API void wc_Des3Free(Des3*);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_DES3 */
#endif /* WOLF_CRYPT_DES3_H */

