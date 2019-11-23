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

#ifndef _CRYPTO_AES_SAM6149_H_
#define _CRYPTO_AES_SAM6149_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "wolfssl/wolfcrypt/aes.h"

typedef union {
    struct {
        uint8_t START : 1;
        uint8_t : 7;
        uint8_t SWRST : 1;
        uint32_t : 23;
    }s;
    uint32_t v;
}CRYPT_AES_SAM6149_AES_CR;

typedef enum  {
    CRYPT_AES_SAM6149_DECRYPTION = 0, /**< Decryption of data will be performed. */
    CRYPT_AES_SAM6149_ENCRYPTION,     /**< Encryption of data will be performed. */
}crypt_aes_sam6149_ciphers;

typedef enum  {
    CRYPT_AES_SAM6149_AES128,
    CRYPT_AES_SAM6149_AES192,
    CRYPT_AES_SAM6149_AES256,    
}crypt_aes_sam6149_keySizes;

typedef enum crypt_aes_sam6149_startModes {
    CRYPT_AES_SAM6149_MANUAL_START,
    CRYPT_AES_SAM6149_AUTO_START,
    CRYPT_AES_SAM6149_IDATAR0_START
}crypt_aes_sam6149_startModes;

typedef enum crypt_aes_sam6149_opModes {
    CRYPT_AES_SAM6149_ECB = 0,       /**< Electronic Codebook (ECB). */
    CRYPT_AES_SAM6149_CBC,           /**< Cipher Block Chaining (CBC). */
    CRYPT_AES_SAM6149_OFB,           /**< Output Feedback (OFB). */
    CRYPT_AES_SAM6149_CFB,           /**< Cipher Feedback (CFB). */
    CRYPT_AES_SAM6149_CTR,           /**< Counter (CTR). */
    CRYPT_AES_SAM6149_GCM,           /**< Galois Counter Mode (GCM).*/
}crypt_aes_sam6149_opModes;

typedef enum  {
    CRYPT_AES_SAM6149_SIZE_128BIT = 0,   /**< Cipher feedback data size is 128-bit. */
    CRYPT_AES_SAM6149_SIZE_64BIT,        /**< Cipher feedback data size is 64-bit. */
    CRYPT_AES_SAM6149_SIZE_32BIT,        /**< Cipher feedback data size is 32-bit. */
    CRYPT_AES_SAM6149_SIZE_16BIT,        /**< Cipher feedback data size is 16-bit. */
    CRYPT_AES_SAM6149_SIZE_8BIT,         /**< Cipher feedback data size is 8-bit. */
}crypt_aes_sam6149_cfbSizes;

typedef union {
    struct {
        crypt_aes_sam6149_ciphers CIPHER : 1;
        uint8_t GTAGEN : 1;
        uint8_t : 1;
        uint8_t DUALBUFF : 1;
        crypt_aes_sam6149_keySizes PROCDLY : 4;
        crypt_aes_sam6149_startModes SMOD : 2;
        crypt_aes_sam6149_keySizes KEYSIZE : 2;
        crypt_aes_sam6149_opModes OPMODE : 3;
        uint8_t LOD : 1;
        crypt_aes_sam6149_cfbSizes CFBS : 3;
        uint8_t : 1;
        uint8_t CKEY : 4;
        uint8_t : 8;
    }s;
    uint32_t v;
}CRYPT_AES_SAM6149_AES_MR;

typedef union {
    struct {
        uint8_t DATRDY : 1;
        uint8_t : 7;
        uint8_t URAD : 1;
        uint8_t : 7;
        uint8_t TAGRDY : 1;
        uint16_t : 15;
    }s;
    uint32_t v;
} CRYPT_AES_SAM6149_AES_IER;

typedef CRYPT_AES_SAM6149_AES_IER CRYPT_AES_SAM6149_AES_IDR;
typedef CRYPT_AES_SAM6149_AES_IER CRYPT_AES_SAM6149_AES_IMR;

typedef enum 
{
    CRYPT_AES_SAM6149_IDR_WR_PROCESSING,
    CRYPT_AES_SAM6149_ODR_RD_PROCESSING,
    CRYPT_AES_SAM6149_MR_WR_PROCESSING,
    CRYPT_AES_SAM6149_ODR_RD_SUBKGEN,
    CRYPT_AES_SAM6149_MR_WR_SUBKGEN,
    CRYPT_AES_SAM6149_WOR_RD_ACCESS,
}crypt_aes_sam6149_URATs;

typedef union {    
    struct {
        uint8_t DATRDY : 1;
        uint8_t : 7;
        uint8_t URAD : 1;
        uint8_t : 3;
        crypt_aes_sam6149_URATs URAT : 4;
        uint8_t TAGRDY : 1;
        uint16_t : 15;
    }s;
    uint32_t v;
}CRYPT_AES_SAM6149_AES_ISR;




#ifdef __cplusplus
}
#endif





#endif
