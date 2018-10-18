/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    pkcs12.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright © 2013-2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END


#ifndef WOLF_CRYPT_PKCS12_H
#define WOLF_CRYPT_PKCS12_H

#include "crypto/src/types.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef WOLFSSL_TYPES_DEFINED /* do not redeclare from ssl.h */
    typedef struct WC_PKCS12 WC_PKCS12;
#endif

typedef struct WC_DerCertList { /* dereferenced in ssl.c */
    byte* buffer;
    word32 bufferSz;
    struct WC_DerCertList* next;
} WC_DerCertList;



WOLFSSL_API WC_PKCS12* wc_PKCS12_new(void);
WOLFSSL_API void wc_PKCS12_free(WC_PKCS12* pkcs12);
WOLFSSL_API int wc_d2i_PKCS12(const byte* der, word32 derSz, WC_PKCS12* pkcs12);
WOLFSSL_API int wc_PKCS12_parse(WC_PKCS12* pkcs12, const char* psw,
        byte** pkey, word32* pkeySz, byte** cert, word32* certSz,
        WC_DerCertList** ca);

WOLFSSL_LOCAL int wc_PKCS12_SetHeap(WC_PKCS12* pkcs12, void* heap);
WOLFSSL_LOCAL void* wc_PKCS12_GetHeap(WC_PKCS12* pkcs12);


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_PKCS12_H */

