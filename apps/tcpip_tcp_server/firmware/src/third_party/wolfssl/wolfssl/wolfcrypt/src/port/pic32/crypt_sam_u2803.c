/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_sam_u2803.c

  Summary:
    Crypto Framework Library source for CRYA cryptographic functions
    found in the SAML11 Implementation-defined Attribution Unit (IDAU)
    identified as U2803.

  Description:
    This source file contains functions that replace (overload) the
    like-named WolfCrypt functions for AES128 and SHA256 as part of
    the Cryptographic Framework Library for the SAML11 family of 
    Microchip microcontrollers.
 * 
 * Disclaimer: For SAML11 the IDAU is identified as U2803(v1) and CRYA
 * is not specifically identified. For PIC32CM'SA, CRYA is U2805 and is
 * notably different than this version, while the IDAU is identified
 * separately as U2803(v2).
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2016-2019 Microchip Technology Inc. and its subsidiaries.

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

#include "configuration.h"

#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_U2803)

#include "wolfssl/wolfcrypt/settings.h"
#include <wolfssl/wolfcrypt/error-crypt.h>

#include "definitions.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_crya_saml11.h"
 
#define assert_dbug(X) __conditional_software_breakpoint((X))

/* *********************************************************************
   *********************************************************************
   * Helper functions
   *********************************************************************
   *********************************************************************
 */

/* suppress any -Wunused-function error */
static __attribute__((used)) uint32_t endian32(const uint32_t input)
{
    uint32_t answer;
    uint32_t value = input; // make sure the source is in RAM
    
    uint8_t * A = (uint8_t*)&answer;
    uint8_t * V = ((uint8_t*)(&value))+sizeof(value)-1;
    for (int i = 0; i < sizeof(answer); i++)
        *A++ = *V--;
    
    return answer;
}

// Tell-tale global counter of access to the CRYA subroutines
// but only in DEBUG mode. Inspect these values to verify that
// a given conversion used the CRYA hardware.
#define CRYA_TELLTALE   0
#if CRYA_TELLTALE
struct crya_tellTale_SAML11_s
{
    size_t crya_aes128_encrypt;
    size_t crya_aes128_decrypt;
    size_t crya_sha256_process;
    size_t crya_gmult;
};
struct crya_tellTale_SAML11_s crya_tellTale = {0};
#define TELLTALE(r)     crya_tellTale.r++
#else
#define TELLTALE(r)     /* as nothing */
#endif

/* *********************************************************************
   *********************************************************************
   * ROM-call wrappers for validation of pointer alignments
   *********************************************************************
   *********************************************************************
 */

#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO)
#if defined(WOLFSSL_AES_128)
/* These wrappers provide parameter validation and instrumentation
 * for the ROM-based CRYA AES functions, if they exist.
 * TODO: these are duplicated in the drivers/crya library; use those
 */
#if defined(secure_crya_aes128_encrypt)
__attribute__((used))
static void crya_aes128_encrypt
    (const uint8_t *keys, uint32_t key_len, const uint8_t *src, uint8_t *dst)
{
    // TODO: figure out some sort of mutex for the CRYA hardware.
    assert_dbug(16 == key_len);
    assert_dbug(keys && (0 == ((uint32_t)keys%4))); // BAD_ALIGN_E
    assert_dbug(src && (0 == ((uint32_t)src%4)));
    assert_dbug(dst && (0 == ((uint32_t)dst%4)));
    
    TELLTALE(crya_aes128_encrypt);
    secure_crya_aes128_encrypt(keys,key_len,src,dst);
}
#endif

#if defined(secure_crya_aes128_decrypt)
__attribute__((used))
static void crya_aes128_decrypt
    (const uint8_t *keys, uint32_t key_len, const uint8_t *src, uint8_t *dst)
{
    // TODO: figure out some sort of mutex for the CRYA hardware.
    assert_dbug(16 == key_len);
    assert_dbug(keys && (0 == ((uint32_t)keys%4))); // BAD_ALIGN_E
    assert_dbug(src && (0 == ((uint32_t)src%4)));
    assert_dbug(dst && (0 == ((uint32_t)dst%4)));
    
    TELLTALE(crya_aes128_decrypt);
    secure_crya_aes128_decrypt(keys,key_len,src,dst);
}
#endif
#endif // WOLFSSL_AES_128

#if !defined(NO_SHA256) \
 && defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_U2803)
 /* This wrapper provides the RAM buffer necessary for proper CRYA
  * data processing, and anticipates when a mutex will be required for
  * access to the CRYA hardware.
  */
__attribute__((used))
static void crya_sha256_process(uint32_t hash_in_out[8], const uint8_t data[64])
{
    // TODO: figure out some sort of mutex for the CRYA hardware.
    uint32_t ram_buf[64]; // 64 full words
    
    assert_dbug(hash_in_out && (0 == ((uint32_t)hash_in_out%4))); // BAD_ALIGN_E
    assert_dbug(data && (0 == ((uint32_t)data%4)));
    // assert_dbug(ram_buf && (0 == ((uint32_t)ram_buf%4))); == always true

    TELLTALE(crya_sha256_process);
    secure_crya_sha256_process(hash_in_out,data,ram_buf);
}

/* *********************************************************************
   *********************************************************************
   * Public entry points
   *********************************************************************
   *********************************************************************
 */
#include "wolfssl/wolfcrypt/sha256.h"

int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
{
    // (void*)heap;
    // (void*)devId;

    /* Need initial hash for SHA256.
     * The WC version has these byte-reversed, but CRYA
     * needs them to be in this order.
     */
    sha256->digest[0] = (0x6a09e667);
    sha256->digest[1] = (0xbb67ae85);
    sha256->digest[2] = (0x3c6ef372);
    sha256->digest[3] = (0xa54ff53a);
    sha256->digest[4] = (0x510e527f);
    sha256->digest[5] = (0x9b05688c);
    sha256->digest[6] = (0x1f83d9ab);
    sha256->digest[7] = (0x5be0cd19);
    
    // Remove any data-noise from the buffer
    memset(sha256->buffer,0,WC_SHA256_BLOCK_SIZE);
    sha256->totalLength = 0;
    sha256->bufferLength = 0;
    return 0;
}

#if !defined(NO_SHA224)
int wc_InitSha224_ex(wc_Sha256* sha224, void* heap, int devId)
{
    // (void*)heap;
    // (void*)devId;

    /* Need initial hash for SHA224.
     * The WC version has these byte-reversed, but CRYA
     * needs them to be in this order.
     */
    sha224->digest[0] = (0xc1059ed8);
    sha224->digest[1] = (0x367cd507);
    sha224->digest[2] = (0x3070dd17);
    sha224->digest[3] = (0xf70e5939);
    sha224->digest[4] = (0xffc00b31);
    sha224->digest[5] = (0x68581511);
    sha224->digest[6] = (0x64f98fa7);
    sha224->digest[7] = (0xbefa4fa4);
    
    // Remove any data-noise from the buffer
    memset(sha224->buffer,0,sizeof(sha224->buffer));
    sha224->totalLength = 0;
    sha224->bufferLength = 0;
    return 0;
}
#endif // NO_SHA224

/* Data encryption process: "gather" operation allows any length
 * data segment to be added to the hash. Repetitive calls are accumulated
 * in the buffer. Full buffers are submitted to CRYA given sufficient
 * data. If the given data itself equals or exceeds a full block, that
 * segment will be submitted in-place to avoid a mem-copy.
 */
int wc_Sha256Update (wc_Sha256* sha256, const uint8_t *input, word32 length)
{
    size_t remainingLength = length;
    while(remainingLength)
    {
        // Check to see if we have any full blocks that can be transferred.
        // Only works if we are not holding any remnant from previous calls.
        while ((0 == sha256->bufferLength)
                && (remainingLength >= WC_SHA256_BLOCK_SIZE))
        {
            crya_sha256_process(sha256->digest,input);
            input += WC_SHA256_BLOCK_SIZE;
            remainingLength -= WC_SHA256_BLOCK_SIZE;
            sha256->totalLength += WC_SHA256_BLOCK_SIZE;
        }

        if (remainingLength > 0)
        {
            size_t bufferSpace = WC_SHA256_BLOCK_SIZE - sha256->bufferLength;
            size_t bytesToCopy = (bufferSpace > remainingLength)?
                /* choose the smaller */    remainingLength : bufferSpace;
            memcpy(&sha256->buffer[sha256->bufferLength],input,bytesToCopy);
            sha256->bufferLength += bytesToCopy;
            remainingLength -= bytesToCopy;
            input += bytesToCopy;
            sha256->totalLength += bytesToCopy;
        
            // If we happen to have filled the buffer, post it
            if (sha256->bufferLength == WC_SHA256_BLOCK_SIZE)
            {
                crya_sha256_process(sha256->digest,sha256->buffer);
                sha256->bufferLength = 0; // i.e., -= WC_SHA256_BLOCK_SIZE;

                // Avoid having residual data in an empty buffer.
                memset(sha256->buffer,0,WC_SHA256_BLOCK_SIZE);
            }
        }
    }
    assert_dbug(sha256->bufferLength < WC_SHA256_BLOCK_SIZE); // strict less-than
    return 0; // no errors from CRYA
}

/* Data encryption process wrap-up. All data having been submitted
 * through wc_Sha256Update(), we are ready to process the padding and
 * length field required by FIPS-180-4. There are two cases to consider:
 *  1. the buffer is completely empty (bufferLength > 0), or
 *  2. the sha256->buffer is partially filled.
 * For the former case, set buffer[0]=0x80 and put the length at the end.
 * For the latter case, post the 0x80 following the data, and then
 * figure out if there is adequate room remaining for the length.
 * Consequently invoke _process() at least once and at most twice.
 * */
int wc_Sha256Final_noCopy(wc_Sha256* sha256)
{
    // If this is not true then _process failed to post a full buffer.
    assert_dbug(WC_SHA256_BLOCK_SIZE > sha256->bufferLength);
    sha256->buffer[sha256->bufferLength] = 0x80; // required by FIPS-180-4
    sha256->bufferLength++; // keep the metadata in sync

    // If there are not 8 bytes remaining, post what we have
    // and reset the buffer to prepare for posting the length field.
    // a.k.a. ((WC_SHA256_BLOCK_SIZE-sha256->bufferLength) < 8)
    if ((WC_SHA256_BLOCK_SIZE-8) < sha256->bufferLength)
    {
        crya_sha256_process(sha256->digest,sha256->buffer);
        memset(sha256->buffer,0,WC_SHA256_BLOCK_SIZE);
        sha256->bufferLength = 0; // keep the metadata in sync
    }
    
    // We know for certain that the final 8 bytes are now all zero
    // (whether we have data in the low-part of the buffer or not)
    // so the length field is always the last few bytes.
    {
        // The buffered value is bit-length instead of byte-length
        // Note that totalLength is a 64b integer, so we will be also.
        uint64_t bitLength = sha256->totalLength * 8; // bits-per-byte
        assert_dbug(8 >= sizeof(bitLength)); // verify an assumption
        
        // FIPS-180-4 requires big-endian format for the length field.
        uint8_t * R = &sha256->buffer[SHA256_BLOCK_SIZE-1]; // last buffer byte
        for (int i = 0; i < sizeof(bitLength); i++) // foreach byte
        {
            *R-- = (uint8_t)(bitLength & 0xFF);
            bitLength >>= 8; // endian independent
        }
    }

    // The final buffer is ready to be posted.
    crya_sha256_process(sha256->digest,sha256->buffer);
    return 0;
}

int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
{
    int answer = wc_Sha256Final_noCopy(sha256);

    if (0 == answer)
    {
        // Endian swap here because CRYA is big-endian and we are not.
        // There is magic here because hash[] is not typed like digest[].
        uint32_t * h = (uint32_t*)hash;
        for (int i = 0; i < WC_SHA256_DIGEST_SIZE/sizeof(h[0]); i++)
            h[i] = endian32((sha256->digest)[i]);
        
        // We do this because the WC original does it, to scrub the state data.
        return wc_InitSha256_ex(sha256, NULL, 0);
    }

    return answer;
}

#if !defined(NO_SHA224)
// Except as noted above, SHA224 is identical to SHA256.
int wc_Sha224Update(wc_Sha256* sha224, const uint8_t *input, word32 length)
{
    return wc_Sha256Update(sha224,input,length);
}
int wc_Sha224Final(wc_Sha256* sha224, byte* hash)
{
    int answer = wc_Sha256Final_noCopy(sha224);

    if (0 == answer)
    {
        // Endian swap here because CRYA is big-endian and we are not.
        // There is magic here because hash[] is not typed like digest[].
        uint32_t * h = (uint32_t*)hash;
        for (int i = 0; i < WC_SHA224_DIGEST_SIZE/sizeof(h[0]); i++)
            h[i] = endian32((sha224->digest)[i]);
        
        // We do this because the WC original does it, to scrub the state data.
        return wc_InitSha224_ex(sha224, NULL, 0);
    }

    return answer;
}
#endif // NO_SHA224
#endif /* NO_SHA256 */


#if !defined(NO_AES)     \
 && !defined(NO_AES_128) \
 && defined(WOLFSSL_HAVE_MCHP_HW_AES_DIRECT)
#include <wolfssl/wolfcrypt/aes.h>

/* Trouble here:
 * SAML11 CYRA supports only 128b keys. Calling here with other sizes
 * will not convert correctly. However, WC does not always check 
 * the return value of these routines, so there you are. 
 */

int wc_AesEncrypt(Aes* aes, const byte* inBlock, byte* outBlock)
{
    static bool beenHereBefore = false;
    
    if (128/8 == aes->keylen)
    {
        crya_aes128_encrypt
            ((uint8_t*)(aes->key), aes->keylen, inBlock, outBlock);
        return 0;
    }

    assert_dbug(beenHereBefore); // see note above
    beenHereBefore |= true;
    return WC_HW_E;
    // or UNSUPPORTED_SUITE, FATAL_ERROR, BAD_FUNC_ARG, KEYUSAGE_E, INVALID_PARAMETER
}

int wc_AesDecrypt(Aes* aes, const byte* inBlock, byte* outBlock)
{
    static bool beenHereBefore = false;
    
    if (128/8 == aes->keylen)
    {
        crya_aes128_decrypt
            ((uint8_t*)(aes->key), aes->keylen, inBlock, outBlock);
        return 0;
    }

    assert_dbug(beenHereBefore); // see note above
    beenHereBefore |= true;
    return WC_HW_E;
    // or UNSUPPORTED_SUITE, FATAL_ERROR, BAD_FUNC_ARG, KEYUSAGE_E, INVALID_PARAMETER
}
#endif /* WOLFSSL_HAVE_MCHP_HW_AES_DIRECT */
#endif /* WOLFSSL_HAVE_MCHP_HW_CRYPTO */
#endif /* WOLFSSL_HAVE_MCHP_HW_CRYPTO_SHA_HW_U2803 */
