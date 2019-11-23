/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_aes_sam6149.h

  Summary:
    Crypto Framework Libarary interface file for hardware RNG

  Description:
    This file contains the interface that is required to be implemented by
    the RNG hardware driver..
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

#ifndef _CRYPTO_AES_U2238_H_
#define _CRYPTO_AES_U2238_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "wolfssl/wolfcrypt/aes.h"

typedef enum  {
    CRYPT_AES_U2238_ECB = 0,       /**< Electronic Codebook (ECB). */
    CRYPT_AES_U2238_CBC,           /**< Cipher Block Chaining (CBC). */
    CRYPT_AES_U2238_OFB,           /**< Output Feedback (OFB). */
    CRYPT_AES_U2238_CFB,           /**< Cipher Feedback (CFB). */
    CRYPT_AES_U2238_CTR,           /**< Counter (CTR). */
    CRYPT_AES_U2238_CCM,           /**< CCM.*/
    CRYPT_AES_U2238_GCM,           /**< Galois Counter Mode (GCM).*/
}crypt_aes_u2238_opModes;

typedef enum  {
    CRYPT_AES_U2238_SIZE_128BIT = 0,   /**< Cipher feedback data size is 128-bit. */
    CRYPT_AES_U2238_SIZE_64BIT,        /**< Cipher feedback data size is 64-bit. */
    CRYPT_AES_U2238_SIZE_32BIT,        /**< Cipher feedback data size is 32-bit. */
    CRYPT_AES_U2238_SIZE_16BIT,        /**< Cipher feedback data size is 16-bit. */
    CRYPT_AES_U2238_SIZE_8BIT,         /**< Cipher feedback data size is 8-bit. */
}crypt_aes_u2238_cfbSizes;

typedef enum  {
    CRYPT_AES_U2238_AES128,
    CRYPT_AES_U2238_AES192,
    CRYPT_AES_U2238_AES256,    
}crypt_aes_u2238_keySizes;

typedef enum  {
    CRYPT_AES_U2238_DECRYPTION = 0, /**< Decryption of data will be performed. */
    CRYPT_AES_U2238_ENCRYPTION,     /**< Encryption of data will be performed. */
}crypt_aes_u2238_ciphers;

typedef enum  {
    CRYPT_AES_U2238_MANUAL_START,
    CRYPT_AES_U2238_AUTO_START,
}crypt_aes_u2238_startModes;

typedef union {
    struct {
        uint8_t SWRST : 1;
        uint8_t ENABLE : 1;
        crypt_aes_u2238_opModes AESMODE : 3;
        crypt_aes_u2238_cfbSizes CFBS : 3;
        crypt_aes_u2238_keySizes KEYSIZE : 2;
        crypt_aes_u2238_ciphers CIPER : 2;
        crypt_aes_u2238_startModes STARTMODE : 1;
        uint8_t LOD : 1;
        uint8_t KEYGEN : 1;
        uint8_t XORKEY : 1;
        uint8_t : 1;
        uint8_t CTYPE : 4;
        uint16_t : 12;
    }s;
    uint32_t v;
}CRYPT_AES_U2238_AES_CTRLA;


typedef union {
    struct {
        uint8_t START : 1;
        uint8_t NEWMSG : 1;
        uint8_t EOM : 1;
        uint8_t GFMUL : 1;
        uint8_t : 4;
    }s;
    uint8_t v;
}CRYPT_AES_U2238_AES_CTRLB;

typedef union {
    struct {
        uint8_t ENCCMP : 1;
        uint8_t GFMCMP : 1;
        uint8_t : 6;
    }s;
    uint8_t v;
}CRYPT_AES_U2238_AES_INTENCLR;

#define CRYPT_AES_U2238_AES_INTENSET CRYPT_AES_U2238_AES_INTENCLR
#define CRYPT_AES_U2238_AES_INTFLAG CRYPT_AES_U2238_AES_INTENCLR

typedef union {
    struct {
        uint8_t INDATAPTR : 2;
        uint8_t : 6;
    }s;
    uint8_t v;
}CRYPT_AES_U2238_AES_DATABUFPTR;

typedef union {
    struct {
        uint8_t DBGRUN : 1;
        uint8_t : 7;
    }s;
    uint8_t v;
}CRYPT_AES_U2238_AES_DBGCTRL;



#ifdef __cplusplus
}
#endif





#endif
