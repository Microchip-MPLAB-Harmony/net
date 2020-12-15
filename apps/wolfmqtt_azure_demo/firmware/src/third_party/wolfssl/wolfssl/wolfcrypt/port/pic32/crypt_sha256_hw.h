/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_sha256_hw.h

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

#ifndef _CRYPTO_SHA256_HW_H_
#define _CRYPTO_SHA256_HW_H_


#include "configuration.h"


#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_11105)
#include "crypt_sha_sam11105.h"
#elif defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_6156)
#include "crypt_sha_sam6156.h"
#endif

struct wc_Sha256 
{
    int devId;
    word32 flags; /* enum wc_HashFlags in hash.h */

#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_11105)
    struct icm_descriptor icm_descriptor;
    uint8_t  buffer[SHA256_BLOCK_SIZE];  /* 64 bytes = 512 bits */
    uint32_t digest[SHA256_DIGEST_SIZE/4];
    uint64_t total_len;   /* number of bytes to be processed  */
#elif defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_6156)
    CRYPT_SHA_SAM6156_shaDescriptor sha_descriptor;
    uint8_t  buffer[SHA256_BLOCK_SIZE];
    uint32_t digest[SHA256_DIGEST_SIZE/4];
    uint64_t total_len;   /* number of bytes to be processed  */
#elif defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_U2803) /* SAML11  */  \
   || defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_U2805) /* PIC32CM */
    uint8_t  buffer[SHA256_BLOCK_SIZE];
    uint32_t digest[SHA256_DIGEST_SIZE/sizeof(uint32_t)];
    size_t   bufferLength; /* number of bytes currently in the buffer */
    uint64_t totalLength;  /* cumulative number of bytes processed  */
#endif
};
#ifndef WC_SHA256_TYPE_DEFINED
    typedef struct wc_Sha256 wc_Sha256;
    #define WC_SHA256_TYPE_DEFINED
#endif

#endif
