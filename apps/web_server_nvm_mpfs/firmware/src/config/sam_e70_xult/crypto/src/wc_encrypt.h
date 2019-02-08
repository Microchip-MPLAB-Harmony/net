/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    wc_encrypt.h
  
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


/*!
    \file crypto/src/wc_encrypt.h
*/


#ifndef WOLF_CRYPT_ENCRYPT_H
#define WOLF_CRYPT_ENCRYPT_H

#include "crypto/src/types.h"
#include "crypto/src/aes.h"
#include "crypto/src/chacha.h"
#include "crypto/src/des3.h"
#include "crypto/src/arc4.h"

#ifdef __cplusplus
    extern "C" {
#endif

/* determine max cipher key size */
#ifndef NO_AES
    #define WC_MAX_SYM_KEY_SIZE     (AES_MAX_KEY_SIZE/8)
#elif defined(HAVE_CHACHA)
    #define WC_MAX_SYM_KEY_SIZE     CHACHA_MAX_KEY_SZ
#elif !defined(NO_DES3)
    #define WC_MAX_SYM_KEY_SIZE     DES3_KEY_SIZE
#elif !defined(NO_RC4)
    #define WC_MAX_SYM_KEY_SIZE     RC4_KEY_SIZE
#else
    #define WC_MAX_SYM_KEY_SIZE     32
#endif


#if !defined(NO_AES) && defined(HAVE_AES_CBC)
WOLFSSL_API int wc_AesCbcEncryptWithKey(byte* out, const byte* in, word32 inSz,
                                        const byte* key, word32 keySz,
                                        const byte* iv);
WOLFSSL_API int wc_AesCbcDecryptWithKey(byte* out, const byte* in, word32 inSz,
                                        const byte* key, word32 keySz,
                                        const byte* iv);
#endif /* !NO_AES */


#ifndef NO_DES3
WOLFSSL_API int wc_Des_CbcDecryptWithKey(byte* out,
                                         const byte* in, word32 sz,
                                         const byte* key, const byte* iv);
WOLFSSL_API int wc_Des_CbcEncryptWithKey(byte* out,
                                         const byte* in, word32 sz,
                                         const byte* key, const byte* iv);
WOLFSSL_API int wc_Des3_CbcEncryptWithKey(byte* out,
                                          const byte* in, word32 sz,
                                          const byte* key, const byte* iv);
WOLFSSL_API int wc_Des3_CbcDecryptWithKey(byte* out,
                                          const byte* in, word32 sz,
                                          const byte* key, const byte* iv);
#endif /* !NO_DES3 */




#ifdef WOLFSSL_ENCRYPTED_KEYS
    struct EncryptedInfo;
    WOLFSSL_API int wc_BufferKeyDecrypt(struct EncryptedInfo* info, byte* der, word32 derSz,
        const byte* password, int passwordSz, int hashType);
    WOLFSSL_API int wc_BufferKeyEncrypt(struct EncryptedInfo* info, byte* der, word32 derSz,
        const byte* password, int passwordSz, int hashType);
#endif /* WOLFSSL_ENCRYPTED_KEYS */

#ifndef NO_PWDBASED
    WOLFSSL_LOCAL int wc_CryptKey(const char* password, int passwordSz, 
        byte* salt, int saltSz, int iterations, int id, byte* input, int length,
        int version, byte* cbcIv, int enc);
#endif

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLF_CRYPT_ENCRYPT_H */

