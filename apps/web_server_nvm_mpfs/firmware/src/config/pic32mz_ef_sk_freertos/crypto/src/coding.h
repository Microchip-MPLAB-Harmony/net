/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    coding.h

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

#ifndef WOLF_CRYPT_CODING_H
#define WOLF_CRYPT_CODING_H

#include "crypto/src/types.h"

#ifdef __cplusplus
    extern "C" {
#endif


WOLFSSL_API int Base64_Decode(const byte* in, word32 inLen, byte* out,
                               word32* outLen);

#if defined(OPENSSL_EXTRA) || defined(SESSION_CERTS) || defined(WOLFSSL_KEY_GEN) \
   || defined(WOLFSSL_CERT_GEN) || defined(HAVE_WEBSERVER) || !defined(NO_DSA)
    #ifndef WOLFSSL_BASE64_ENCODE
        #define WOLFSSL_BASE64_ENCODE
    #endif
#endif


#ifdef WOLFSSL_BASE64_ENCODE
    enum Escaped {
        WC_STD_ENC = 0,       /* normal \n line ending encoding */
        WC_ESC_NL_ENC,        /* use escape sequence encoding   */
        WC_NO_NL_ENC          /* no encoding at all             */
    }; /* Encoding types */

    /* encode isn't */
    WOLFSSL_API
    int Base64_Encode(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);
    WOLFSSL_API
    int Base64_EncodeEsc(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);
    WOLFSSL_API
    int Base64_Encode_NoNl(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
    defined(HAVE_WEBSERVER) || defined(HAVE_FIPS) || \
    defined(HAVE_ECC_CDH) || defined(HAVE_SELFTEST) || \
    defined(WOLFSSL_ENCRYPTED_KEYS)
    #ifndef WOLFSSL_BASE16
        #define WOLFSSL_BASE16
    #endif
#endif

#ifdef WOLFSSL_BASE16
    WOLFSSL_API
    int Base16_Decode(const byte* in, word32 inLen, byte* out, word32* outLen);
    WOLFSSL_API
    int Base16_Encode(const byte* in, word32 inLen, byte* out, word32* outLen);
#endif


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_CODING_H */

