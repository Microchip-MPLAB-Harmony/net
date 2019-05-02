/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_sha384_hw.h

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

#ifndef _CRYPTO_SHA384_HW_H_
#define _CRYPTO_SHA384_HW_H_


#if defined(CRYPTO_SHA_HW_6156)
#include "crypt_sha_sam6156.h"
#endif

#include "configuration.h"
#include "types.h"


typedef struct 
{
#if defined(CRYPTO_SHA_HW_6156)
    CRYPT_SHA_SAM6156_shaDescriptor sha_descriptor;
    uint8_t  buffer[SHA384_BLOCK_SIZE];
    uint32_t digest[SHA512_DIGEST_SIZE/4];
    uint64_t total_len;   /* number of bytes to be processed  */
#endif
}crypt_sha384_hw_descriptor;

int CRYPT_SHA384_InitSha(crypt_sha384_hw_descriptor* sha, void* heap, int devId);
int CRYPT_SHA384_Update(crypt_sha384_hw_descriptor* sha, const byte* data, word32 len);
int CRYPT_SHA384_Final(crypt_sha384_hw_descriptor* sha, byte* hash);
int CRYPT_SHA384_FinalRaw(crypt_sha384_hw_descriptor* sha, byte* hash);

#endif
