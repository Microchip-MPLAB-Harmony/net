/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    md5.h

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

#ifndef WOLF_CRYPT_MD5_H
#define WOLF_CRYPT_MD5_H

#include "crypto/src/types.h"

#ifndef NO_MD5

#ifdef HAVE_FIPS
    #define wc_InitMd5   InitMd5
    #define wc_Md5Update Md5Update
    #define wc_Md5Final  Md5Final
    #define wc_Md5Hash   Md5Hash
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef NO_OLD_WC_NAMES
    #define Md5             wc_Md5
    #define MD5             WC_MD5
    #define MD5_BLOCK_SIZE  WC_MD5_BLOCK_SIZE
    #define MD5_DIGEST_SIZE WC_MD5_DIGEST_SIZE
    #define WC_MD5_PAD_SIZE WC_MD5_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_MD5             =  WC_HASH_TYPE_MD5,
    WC_MD5_BLOCK_SIZE  = 64,
    WC_MD5_DIGEST_SIZE = 16,
    WC_MD5_PAD_SIZE    = 56
};


#ifdef WOLFSSL_MICROCHIP_PIC32MZ
    #include "crypto/src/pic32mz-crypt.h"
#endif
#ifdef STM32_HASH
    #include "crypto/src/port/st/stm32.h"
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

#ifdef WOLFSSL_TI_HASH
    #include "crypto/src/port/ti/ti-hash.h"
#elif defined(WOLFSSL_IMX6_CAAM)
    #include "crypto/src/port/caam/wolfcaam_sha.h"
#else

/* MD5 digest */
typedef struct wc_Md5 {
#ifdef STM32_HASH
    STM32_HASH_Context stmCtx;
#else
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    word32  buffer[WC_MD5_BLOCK_SIZE  / sizeof(word32)];
#ifdef WOLFSSL_PIC32MZ_HASH
    word32  digest[PIC32_DIGEST_SIZE / sizeof(word32)];
#else
    word32  digest[WC_MD5_DIGEST_SIZE / sizeof(word32)];
#endif
    void*   heap;
#ifdef WOLFSSL_PIC32MZ_HASH
    hashUpdCache cache; /* cache for updates */
#endif
#endif /* STM32_HASH */
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
} wc_Md5;

#endif /* WOLFSSL_TI_HASH */

WOLFSSL_API int wc_InitMd5(wc_Md5*);
WOLFSSL_API int wc_InitMd5_ex(wc_Md5*, void*, int);
WOLFSSL_API int wc_Md5Update(wc_Md5*, const byte*, word32);
WOLFSSL_API int wc_Md5Final(wc_Md5*, byte*);
WOLFSSL_API void wc_Md5Free(wc_Md5*);

WOLFSSL_API int  wc_Md5GetHash(wc_Md5*, byte*);
WOLFSSL_API int  wc_Md5Copy(wc_Md5*, wc_Md5*);

#ifdef WOLFSSL_PIC32MZ_HASH
WOLFSSL_API void wc_Md5SizeSet(wc_Md5* md5, word32 len);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_MD5 */
#endif /* WOLF_CRYPT_MD5_H */
