/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_sha1_hw.h

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

#ifndef _CRYPTO_AES_HW_H_
#define _CRYPTO_AES_HW_H_

#include "configuration.h"
#include "aes.h"

#if defined(CRYPTO_SHA_HW_11105)
#include "crypt_aes_sam6149.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct 
{
#if defined(CRYPTO_SHA_HW_11105)
    CRYPT_AES_SAM6149_AES_MR aesMr;
    uint8_t aesKey[32];
    uint8_t aesIv[16];
    uint32_t aesAddlenr;
    uint8_t aesGHash[16];
    uint32_t aesCtr;
    uint8_t aesGcmH[16];
#endif
    
}crypt_aes_hw_descriptor;


#ifdef __cplusplus
}
#endif


#endif
