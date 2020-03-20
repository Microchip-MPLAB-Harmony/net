/* 
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_crya_saml11.h

  Summary:
    Crypto Framework Library interface file for CRYA hardware in SAML11

  Description:
    This file contains the interface that is required when using the
    AES and SHA accelerators in SAML11CM'SA as described in DS60001513.
 */

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2020 Microchip Technology Inc. and its subsidiaries.

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

#ifndef CRYPTO_CRYA_SAML11_H
#define	CRYPTO_CRYA_SAML11_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
/* Requirements:
   1. Only callable from Secure execution environment
   2. All pointers are 32b aligned (4-byte alignment)
   3. Entry-point names are derived from the data sheet,
      but with bit-counts noted to future-proof the names
*/
// FIPS Publication 180-4
typedef void (*crya_sha256_process_t)
    (uint32_t hash_in_out[8], const uint8_t data[64], uint32_t ram_buf[64]);
#define secure_crya_sha256_process ((crya_sha256_process_t ) (0x02001900 | 0x1))
 
// FIPS Publication 197 -- key must be 128b (16B)
typedef void (*crya_aes128_encrypt_t)
    (const uint8_t *keys, uint32_t key_len, const uint8_t *src, uint8_t *dst);
typedef void (*crya_aes128_decrypt_t)
    (const uint8_t *keys, uint32_t key_len, const uint8_t *src, uint8_t *dst);
#define secure_crya_aes128_encrypt ((crya_aes128_encrypt_t ) (0x02001904 | 0x1))
#define secure_crya_aes128_decrypt ((crya_aes128_decrypt_t ) (0x02001908 | 0x1))
 
// NIST Special Publication 800-38D -- GCM mode inner loop
typedef void (*crya_gf_mult128_t)
    (const uint32_t *block1, const uint32_t *block2, uint32_t *dst);
#define secure_crya_gf_mult128 ((crya_gf_mult128_t ) (0x0200190C | 0x1))


#ifdef	__cplusplus
}
#endif

#endif	/* CRYPTO_CRYA_SAML11_H */

