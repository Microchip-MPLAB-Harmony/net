/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    wc_pkcs11.h

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

#ifndef _WOLFPKCS11_H_
#define _WOLFPKCS11_H_

#include "crypto/src/types.h"

#ifdef HAVE_PKCS11

#include "crypto/src/cryptodev.h"
#include "crypto/src/pkcs11.h"

typedef struct Pkcs11Dev {
    void*             dlHandle;         /* Handle to library  */
    CK_FUNCTION_LIST* func;             /* Array of functions */
    void*             heap;
} Pkcs11Dev;

typedef struct Pkcs11Token {
    CK_FUNCTION_LIST* func;             /* Table of PKCS#11 function from lib */
    CK_SLOT_ID        slotId;           /* Id of slot to use                  */
    CK_SESSION_HANDLE handle;           /* Handle to active session           */
    CK_UTF8CHAR_PTR   userPin;          /* User's PIN to login with           */
    CK_ULONG          userPinSz;        /* Size of user's PIN in bytes        */
} Pkcs11Token;

typedef struct Pkcs11Session {
    CK_FUNCTION_LIST* func;             /* Table of PKCS#11 function from lib */
    CK_SLOT_ID        slotId;           /* Id of slot to use                  */
    CK_SESSION_HANDLE handle;           /* Handle to active session           */
} Pkcs11Session;

#ifdef __cplusplus
    extern "C" {
#endif

/* Types of keys that can be stored. */
enum Pkcs11KeyType {
    PKCS11_KEY_TYPE_AES_GCM,
    PKCS11_KEY_TYPE_RSA,
    PKCS11_KEY_TYPE_EC,
};


WOLFSSL_API int wc_Pkcs11_Initialize(Pkcs11Dev* dev, const char* library,
                                     void* heap);
WOLFSSL_API void wc_Pkcs11_Finalize(Pkcs11Dev* dev);

WOLFSSL_API int wc_Pkcs11Token_Init(Pkcs11Token* token, Pkcs11Dev* dev,
    int slotId, const char* tokenName, const unsigned char *userPin,
    int userPinSz);
WOLFSSL_API void wc_Pkcs11Token_Final(Pkcs11Token* token);
WOLFSSL_API int wc_Pkcs11Token_Open(Pkcs11Token* token, int readWrite);
WOLFSSL_API void wc_Pkcs11Token_Close(Pkcs11Token* token);

WOLFSSL_API int wc_Pkcs11StoreKey(Pkcs11Token* token, int type, int clear,
    void* key);

WOLFSSL_API int wc_Pkcs11_CryptoDevCb(int devId, wc_CryptoInfo* info,
    void* ctx);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_PKCS11 */

#endif /* _WOLFPKCS11_H_ */
