/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_tdes_sam6150.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for SAMA5D2 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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

#ifndef CRYPT_TDES_SAM6150_H_
#define CRYPT_TDES_SAM6150_H_


/*------------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include "configuration.h"
#include <stdint.h>

#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_TDES_HW_6150)

typedef union {
  struct {
    uint8_t START : 1;
    uint8_t :7;
    uint8_t SWRST : 1;
    uint8_t : 7;
    uint16_t : 16;
  } s;
  uint32_t v;  
} CRYPTO_TDES_SAM6150_CR;

typedef enum {
  CRYPTO_TDES_SAM6150_SINGLE_DES = 0,
  CRYPTO_TDES_SAM6150_TRIPLE_DES = 1,
  CRYPTO_TDES_SAM6150_XTEA = 2,
} CRYPTO_TDES_SAM6150_DesModes;

typedef enum {
  CRYPTO_TDES_SAM6150_MANUAL_START = 0,
  CRYPTO_TDES_SAM6150_AUTO_START = 1,
  CRYPTO_TDES_SAM6150_IDATAR0_START = 2,
} CRYPTO_TDES_SAM6150_StartModes;

typedef enum {
  CRYPTO_TDES_SAM6150_ECB = 0,
  CRYPTO_TDES_SAM6150_CBC = 1,
  CRYPTO_TDES_SAM6150_OFB = 2,
  CRYPTO_TDES_SAM6150_CFB = 3,
} CRYPTO_TDES_SAM6150_OpModes;

typedef enum {
  CRYPTO_TDES_SAM6150_SIZE_64BIT = 0,
  CRYPTO_TDES_SAM6150_SIZE_32BIT = 1,
  CRYPTO_TDES_SAM6150_SIZE_16BIT = 2,
  CRYPTO_TDES_SAM6150_SIZE_8BIT = 3
} CRYPTO_TDES_SAM6150_CFBSes;
    
typedef union {
  struct {
    uint8_t CIPHER : 1;
    CRYPTO_TDES_SAM6150_DesModes TDESMOD : 2;
    uint8_t :1;
    uint8_t KEYMOD : 1;
    uint8_t : 3;
    CRYPTO_TDES_SAM6150_StartModes SMOD : 2;
    uint8_t : 2;
    CRYPTO_TDES_SAM6150_OpModes OPMOD : 2;
    uint8_t : 1;
    uint8_t LOD : 1;
    CRYPTO_TDES_SAM6150_CFBSes CFBS : 2;
    uint8_t : 6;
    uint8_t : 8;
  } s;
  uint32_t v;
} CRYPTO_TDES_SAM6150_MR;
  
typedef union {
  struct {
    uint8_t DATRDY : 1;
    uint8_t : 7;
    uint8_t URAD : 1;
    uint8_t : 7;
    uint16_t : 16;
  } s;
  uint32_t v;
} CRYPTO_TDES_SAM6150_IER;

#define CRYPTO_TDES_SAM6150_IDR CRYPTO_TDES_SAM6150_IER
#define CRYPTO_TDES_SAM6150_IMR CRYPTO_TDES_SAM6150_IER
#define CRYPTO_TDES_SAM6150_ISR CRYPTO_TDES_SAM6150_IER

typedef union {
  struct {
    uint8_t XTEA_RNDS : 6;
    uint8_t : 2;
    uint32_t : 24;
  } s;
  uint32_t v;
} CRYPTO_TDES_SAM6150_XTEA_RNDR;

#endif

#endif /* CRYPT_TDES_SAM6150_H_ */
