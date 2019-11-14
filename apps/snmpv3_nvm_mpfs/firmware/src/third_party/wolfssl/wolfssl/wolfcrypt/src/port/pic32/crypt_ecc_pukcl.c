/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_ecc_pukcl.c

  Summary:
    Crypto Framework Libarary source file for hardware Cryptography

  Description:
    This file provides an example for interfacing with the PUKCC module
    on the SAME5x device family.
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



#include "wolfssl/wolfcrypt/port/pic32/crypt_ecc_pukcl.h"
#include "wolfssl/wolfcrypt/ecc.h"
#include "wolfssl/wolfcrypt/cryptocb.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_pukcl_functions.h"
#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_Hardware_pb.h"
#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_Headers_pb.h"
#include "wolfssl/wolfcrypt/random.h"
#include "wolfssl/wolfcrypt/integer.h"
#include "wolfssl/wolfcrypt/asn.h"
#include <string.h>
#include <stdint.h>


#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_ECC_HW_PUKCC) && defined(WOLFSSL_HAVE_MCHP_HW_ECC)

//******************************************************************************
// Memory mapping for ECC scalar multiplication
//******************************************************************************
#define BASE_SCA_MUL_MODULO(a, b) AT91C_BASE_CRYPTO_RAM
#define BASE_SCA_MUL_CNS(a, b) (BASE_SCA_MUL_MODULO(a, b) + a + 4)
#define BASE_SCA_MUL_POINT_A(a, b) (BASE_SCA_MUL_CNS(a, b) + a + 12)
#define BASE_SCA_MUL_POINT_A_X(a, b) (BASE_SCA_MUL_POINT_A(a, b))
#define BASE_SCA_MUL_POINT_A_Y(a, b) (BASE_SCA_MUL_POINT_A_X(a, b) + a + 4)
#define BASE_SCA_MUL_POINT_A_Z(a, b) (BASE_SCA_MUL_POINT_A_Y(a, b) + a + 4)
#define BASE_SCA_MUL_A(a, b) (BASE_SCA_MUL_POINT_A_Z(a, b) + a + 4 + 8)
#define BASE_SCA_MUL_SCALAR(a, b) (BASE_SCA_MUL_A(a, b) + a + 4)
#define BASE_SCA_MUL_ORDER(a, b) (BASE_SCA_MUL_SCALAR(a, b) + b + 4)
#define BASE_SCA_MUL_WORKSPACE(a, b) (BASE_SCA_MUL_ORDER(a, b) + b + 4)

//******************************************************************************
// Memory mapping for ECDSA signature
//******************************************************************************
#define BASE_ECDSA_MODULO(a, b) AT91C_BASE_CRYPTO_RAM
#define BASE_ECDSA_CNS(a, b) (BASE_ECDSA_MODULO(a, b) + a + 4)
#define BASE_ECDSA_POINT_A(a, b) (BASE_ECDSA_CNS(a, b) + b + 12)
#define BASE_ECDSA_POINT_A_X(a, b) (BASE_ECDSA_POINT_A(a, b))
#define BASE_ECDSA_POINT_A_Y(a, b) (BASE_ECDSA_POINT_A_X(a, b) + a + 4)
#define BASE_ECDSA_POINT_A_Z(a, b) (BASE_ECDSA_POINT_A_Y(a, b) + a + 4)
#define BASE_ECDSA_A(a, b) (BASE_ECDSA_POINT_A_Z(a, b) + a + 4)
#define BASE_PRIVATE_KEY(a, b) (BASE_ECDSA_A(a, b) + a + 4)
#define BASE_ECDSA_SCALAR(a, b) (BASE_PRIVATE_KEY(a, b) + b + 4)
#define BASE_ECDSA_ORDER(a, b) (BASE_ECDSA_SCALAR(a, b) + b + 4)
#define BASE_ECDSA_HASH(a, b) (BASE_ECDSA_ORDER(a, b) + b + 4)
#define BASE_ECDSA_WORKSPACE(a, b) (BASE_ECDSA_HASH(a, b) + b + 4)

//******************************************************************************
// Memory mapping for Modular Reduction
//******************************************************************************
#define BASE_REDMOD_MODULO(a) AT91C_BASE_CRYPTO_RAM
#define BASE_REDMOD_CNSBASE(a) (BASE_REDMOD_MODULO(a) + a + 4)
#define BASE_REDMOD_RBASE(a) (BASE_REDMOD_CNSBASE(a) + a + 12)
#define BASE_REDMOD_XBASE(a) (BASE_REDMOD_RBASE(a) + 68)


//******************************************************************************
// Memory mapping for ECDSA signature verification
//******************************************************************************
#define BASE_ECDSAV_MODULO(a, b) AT91C_BASE_CRYPTO_RAM
#define BASE_ECDSAV_CNS(a, b) (BASE_ECDSAV_MODULO(a, b) + a + 4)
#define BASE_ECDSAV_ORDER(a, b) (BASE_ECDSAV_CNS(a, b) + b + 12)
#define BASE_ECDSAV_SIGNATURE(a, b) (BASE_ECDSAV_ORDER(a, b) + b + 12)
#define BASE_ECDSAV_SIGNATURE_R(a, b) (BASE_ECDSAV_SIGNATURE(a, b))
#define BASE_ECDSAV_SIGNATURE_S(a, b) (BASE_ECDSAV_SIGNATURE_R(a, b) + b + 4)
#define BASE_ECDSAV_HASH(a, b) (BASE_ECDSAV_SIGNATURE(a, b) + 2 * b + 8)
#define BASE_ECDSAV_POINT_A(a, b) (BASE_ECDSAV_HASH(a, b) + b + 4)
#define BASE_ECDSAV_POINT_A_X(a, b) (BASE_ECDSAV_POINT_A(a, b))
#define BASE_ECDSAV_POINT_A_Y(a, b) (BASE_ECDSAV_POINT_A_X(a, b) + a + 4)
#define BASE_ECDSAV_POINT_A_Z(a, b) (BASE_ECDSAV_POINT_A_Y(a, b) + a + 4)
#define BASE_ECDSAV_PUBLIC_KEY(a, b) (BASE_ECDSAV_POINT_A_Z(a, b) + a + 4)
#define BASE_ECDSAV_PUBLIC_KEY_X(a, b) (BASE_ECDSAV_PUBLIC_KEY(a, b))
#define BASE_ECDSAV_PUBLIC_KEY_Y(a, b) (BASE_ECDSAV_PUBLIC_KEY_X(a, b) + a + 4)
#define BASE_ECDSAV_PUBLIC_KEY_Z(a, b) (BASE_ECDSAV_PUBLIC_KEY_Y(a, b) + a + 4)
#define BASE_ECDSAV_A(a, b) (BASE_ECDSAV_PUBLIC_KEY_Z(a, b) + a + 4)
#define BASE_ECDSAV_WORKSPACE(a, b) (BASE_ECDSAV_A(a, b) + a + 4)

//******************************************************************************
// Memory mapping for ECDSA Quick verification
//******************************************************************************
#define BASE_ECDSAQUICKV_MODCNS(a, b, wA, wB) (pu1) AT91C_BASE_CRYPTO_RAM
#define BASE_ECDSAQUICKV_ORDER(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_MODCNS(a, b, wA, wB) + (a + 4) + (MAX(a, b) + 12))
#define BASE_ECDSAQUICKV_HASH(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_ORDER(a, b, wA, wB) + b + 4)

#define BASE_ECDSAQUICKV_POINT_A(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_HASH(a, b, wA, wB) + b + 4)
#define BASE_ECDSAQUICKV_POINT_A_X(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_POINT_A(a, b, wA, wB))
#define BASE_ECDSAQUICKV_POINT_A_Y(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_POINT_A_X(a, b, wA, wB) + a + 4)
#define BASE_ECDSAQUICKV_POINT_A_Z(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_POINT_A_Y(a, b, wA, wB) + a + 4)

#define BASE_ECDSAQUICKV_PUBLIC_KEY(a, b, wA, wB)                                                                      \
	(pu1)(BASE_ECDSAQUICKV_POINT_A(a, b, wA, wB) + ((1 << (wA - 2)) * 3 * (a + 4)))
#define BASE_ECDSAQUICKV_PUBLIC_KEY_X(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_PUBLIC_KEY(a, b, wA, wB))
#define BASE_ECDSAQUICKV_PUBLIC_KEY_Y(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_PUBLIC_KEY_X(a, b, wA, wB) + a + 4)
#define BASE_ECDSAQUICKV_PUBLIC_KEY_Z(a, b, wA, wB) (pu1)(BASE_ECDSAQUICKV_PUBLIC_KEY_Y(a, b, wA, wB) + a + 4)

#define BASE_ECDSAQUICKV_AWORKSPACE(a, b, wA, wB)                                                                      \
	(pu1)(BASE_ECDSAQUICKV_PUBLIC_KEY(a, b, wA, wB) + ((1 << (wB - 2)) * 3 * (a + 4)))

#define BASE_ECDSAQUICKV_SIGNATURE_R_INCPKCCRAM(a, b, wA, wB)                                                          \
	(pu1)(BASE_ECDSAQUICKV_AWORKSPACE(a, b, wA, wB) + a + 4 + (8 * MAX(a, b)) + 44)
#define BASE_ECDSAQUICKV_SIGNATURE_S_INCPKCCRAM(a, b, wA, wB)                                                          \
	(pu1)(BASE_ECDSAQUICKV_SIGNATURE_R_INCPKCCRAM(a, b, wA, wB) + b + 4)

//******************************************************************************
// Memory mapping for conversion
//******************************************************************************
#define BASE_CONV_MODULO(a) AT91C_BASE_CRYPTO_RAM
#define BASE_CONV_CNS(a) (BASE_CONV_MODULO(a) + a + 4)
#define BASE_CONV_POINT_A(a) (BASE_CONV_CNS(a) + a + 8)
#define BASE_CONV_POINT_A_X(a) (BASE_CONV_POINT_A(a))
#define BASE_CONV_POINT_A_Y(a) (BASE_CONV_POINT_A_X(a) + a + 4)
#define BASE_CONV_POINT_A_Z(a) (BASE_CONV_POINT_A_Y(a) + a + 4)
#define BASE_CONV_RANDOM(a) (BASE_CONV_POINT_A_Z(a) + a + 4)
#define BASE_CONV_A(a) (BASE_CONV_RANDOM(a) + a + 4)
#define BASE_CONV_B(a) (BASE_CONV_A(a) + a + 4)
#define BASE_CONV_WORKSPACE(a) (BASE_CONV_B(a) + a + 4)

//******************************************************************************
// Memory mapping for doubling
//******************************************************************************
#define BASE_DOUBLING_MODULO(a) AT91C_BASE_CRYPTO_RAM
#define BASE_DOUBLING_CNS(a) (BASE_DOUBLING_MODULO(a) + a + 4)
#define BASE_DOUBLING_POINT_A(a) (BASE_DOUBLING_CNS(a) + a + 8)
#define BASE_DOUBLING_POINT_A_X(a) (BASE_DOUBLING_POINT_A(a))
#define BASE_DOUBLING_POINT_A_Y(a) (BASE_DOUBLING_POINT_A_X(a) + a + 4)
#define BASE_DOUBLING_POINT_A_Z(a) (BASE_DOUBLING_POINT_A_Y(a) + a + 4)
#define BASE_DOUBLING_A(a) (BASE_DOUBLING_POINT_A_Z(a) + a + 4)
#define BASE_DOUBLING_WORKSPACE(a) (BASE_DOUBLING_A(a) + a + 4)

//******************************************************************************
// Memory mapping for addition
//******************************************************************************
#define BASE_ADDITION_MODULO(a) AT91C_BASE_CRYPTO_RAM
#define BASE_ADDITION_CNS(a) (BASE_ADDITION_MODULO(a) + a + 4)
#define BASE_ADDITION_POINT_A(a) (BASE_ADDITION_CNS(a) + a + 8)
#define BASE_ADDITION_POINT_A_X(a) (BASE_ADDITION_POINT_A(a))
#define BASE_ADDITION_POINT_A_Y(a) (BASE_ADDITION_POINT_A_X(a) + a + 4)
#define BASE_ADDITION_POINT_A_Z(a) (BASE_ADDITION_POINT_A_Y(a) + a + 4)
#define BASE_ADDITION_POINT_B(a) (BASE_ADDITION_POINT_A_Z(a) + a + 4)
#define BASE_ADDITION_POINT_B_X(a) (BASE_ADDITION_POINT_B(a))
#define BASE_ADDITION_POINT_B_Y(a) (BASE_ADDITION_POINT_B_X(a) + a + 4)
#define BASE_ADDITION_POINT_B_Z(a) (BASE_ADDITION_POINT_B_Y(a) + a + 4)
#define BASE_ADDITION_WORKSPACE(a) (BASE_ADDITION_POINT_B_Z(a) + a + 4)


static uint8_t eccScratchPad[70] = {0x00};
mp_int scratchLI;

int Crypt_ECC_HandleReq(int devId, wc_CryptoInfo* info, void* ctx)
{
    if (crypt_pukcl_SelfTest() < 0)
    {
        return CRYPTOCB_UNAVAILABLE;
    }

    if (info->pk.type == WC_PK_TYPE_ECDSA_SIGN)
    {
        return Crypt_ECC_HandleEccSignReq(devId, info, ctx);
    }
    
    if (info->pk.type == WC_PK_TYPE_ECDSA_VERIFY)
    {
        return Crypt_ECC_HandleEccVerifyReq(devId, info, ctx);
    }
    
    return CRYPTOCB_UNAVAILABLE;
}


int Crypt_ECC_HandleEccSignReq(int devId, wc_CryptoInfo* info, void* ctx)
{
    uint8_t size = info->pk.eccsign.key->dp->size;

    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccsign.key->dp->prime, MP_RADIX_HEX);
        
    SecureCopy((void*)BASE_REDMOD_MODULO(size), size + 4, (void*)(&scratchLI.dp), size, 0);
    //SecureCopy(eccScratchPad, size + 4, scratchLI.dp, size, 0);
    memset((void*)BASE_REDMOD_CNSBASE(size), 0, size + 12);
    memset((void*)BASE_REDMOD_RBASE(size), 0, 68);
    memset((void*)BASE_REDMOD_XBASE(size), 0, (size * 2) + 8);
    
    memset(&PUKCLParam, 0, sizeof(PUKCL_PARAM));
    PUKCL(u2Option) = PUKCL_REDMOD_SETUP;
    PUKCL(Specific).CarryIn = 0;
    PUKCL(Specific).Gf2n = 0;
    PUKCL_RedMod(u2ModLength) = size;
    PUKCL_RedMod(nu1ModBase) = (nu1)(BASE_REDMOD_MODULO(size));
    PUKCL_RedMod(nu1CnsBase) = (nu1)(BASE_REDMOD_CNSBASE(size));
    PUKCL_RedMod(nu1RBase) = (nu1)(BASE_REDMOD_RBASE(size));
    PUKCL_RedMod(nu1XBase) = (nu1)(BASE_REDMOD_XBASE(size));
    
    vPUKCL_Process(RedMod, pvPUKCLParam);
    
    uint32_t ret = PUKCL(u2Status);
    
    if (ret != PUKCL_OK)
    {
        return ret;
    }
    
    memset(&PUKCLParam, 0, sizeof(PUKCL_PARAM));
    
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccsign.key->dp->Gx, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSA_POINT_A_X(size, size), size+4, (void*)(&scratchLI.dp), size, 0);
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccsign.key->dp->Gy, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSA_POINT_A_Y(size, size), size+4, (void*)(&scratchLI.dp), size, 0);
    uint8_t z = 1;
    SecureCopy((void*)BASE_ECDSA_POINT_A_Z(size, size), size+4, &z, sizeof(z), 0);
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccsign.key->dp->order, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSA_ORDER(size, size), size+4, (void*)(&scratchLI.dp), size, 0);
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccsign.key->dp->Af, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSA_A(size, size), size+4, (void*)(&scratchLI.dp), size, 0);
    SecureCopy((void*)BASE_PRIVATE_KEY(size, size), size+4, (void*)(&info->pk.eccsign.key->k.dp), size, 0);
    
    WC_RNG rngCtx;
    wc_InitRng(&rngCtx);
    wc_RNG_GenerateBlock(&rngCtx, eccScratchPad, size);
    
    SecureCopy((void*)BASE_ECDSA_SCALAR(size, size), size + 4, eccScratchPad, size, 0);
    SecureCopy((void*)BASE_ECDSA_HASH(size, size), size+4, info->pk.eccsign.in, MIN(info->pk.eccsign.inlen, size), 1);
    memset((void*)BASE_ECDSA_WORKSPACE(size, size), 0, (8*size) + 44);
    
    PUKCL_ZpEcDsaGenerate(nu1ModBase)        = (nu1)BASE_ECDSA_MODULO(size, size);
	PUKCL_ZpEcDsaGenerate(nu1CnsBase)        = (nu1)BASE_ECDSA_CNS(size, size);
	PUKCL_ZpEcDsaGenerate(nu1PointABase)     = (nu1)BASE_ECDSA_POINT_A(size, size);
	PUKCL_ZpEcDsaGenerate(nu1PrivateKey)     = (nu1)BASE_PRIVATE_KEY(size, size);
	PUKCL_ZpEcDsaGenerate(nu1ScalarNumber)   = (nu1)BASE_ECDSA_SCALAR(size, size);
	PUKCL_ZpEcDsaGenerate(nu1OrderPointBase) = (nu1)BASE_ECDSA_ORDER(size, size);
	PUKCL_ZpEcDsaGenerate(nu1ABase)          = (nu1)BASE_ECDSA_A(size, size);
	PUKCL_ZpEcDsaGenerate(nu1Workspace)      = (nu1)BASE_ECDSA_WORKSPACE(size, size);
	PUKCL_ZpEcDsaGenerate(nu1HashBase)       = (nu1)BASE_ECDSA_HASH(size, size);
	PUKCL_ZpEcDsaGenerate(u2ModLength)       = size;
	PUKCL_ZpEcDsaGenerate(u2ScalarLength)    = size;

    	// Launch the signature generation !
	// See PUKCL_Rc_pb.h for possible u2Status Values !
	vPUKCL_Process(ZpEcDsaGenerateFast, pvPUKCLParam);
    ret = PUKCL(u2Status);
    
    if (ret != PUKCL_OK)
    {
		return ret;
    }

    	// Copies the result into pfu1Signature
    mp_int r, s;
    mp_clear(&r);
    mp_clear(&s);
    
    SecureCopy((void*)(&r.dp), size, (void*)BASE_ECDSA_POINT_A_X(size, size), size, 0);
    
    r.used = size/4;
    
    
    SecureCopy((void*)(&s.dp), size, (void*)BASE_ECDSA_POINT_A_Y(size, size), size, 0);
    s.used = size/4;
    
    return StoreECC_DSA_Sig(info->pk.eccsign.out, info->pk.eccsign.outlen, &r, &s);
    
    
}


int Crypt_ECC_HandleEccVerifyReq(int devId, wc_CryptoInfo* info, void* ctx)
{
    uint8_t size = info->pk.eccverify.key->dp->size;

    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccverify.key->dp->prime, MP_RADIX_HEX);
        
    SecureCopy((void*)BASE_REDMOD_MODULO(size), size + 4, (void*)(&scratchLI.dp), size, 0);
    //SecureCopy(eccScratchPad, size + 4, scratchLI.dp, size, 0);
    memset((void*)BASE_REDMOD_CNSBASE(size), 0, size + 12);
    memset((void*)BASE_REDMOD_RBASE(size), 0, 68);
    memset((void*)BASE_REDMOD_XBASE(size), 0, (size * 2) + 8);
    
    memset(&PUKCLParam, 0, sizeof(PUKCL_PARAM));
    PUKCL(u2Option) = PUKCL_REDMOD_SETUP;
    PUKCL(Specific).CarryIn = 0;
    PUKCL(Specific).Gf2n = 0;
    PUKCL_RedMod(u2ModLength) = size;
    PUKCL_RedMod(nu1ModBase) = (nu1)(BASE_REDMOD_MODULO(size));
    PUKCL_RedMod(nu1CnsBase) = (nu1)(BASE_REDMOD_CNSBASE(size));
    PUKCL_RedMod(nu1RBase) = (nu1)(BASE_REDMOD_RBASE(size));
    PUKCL_RedMod(nu1XBase) = (nu1)(BASE_REDMOD_XBASE(size));
    
    vPUKCL_Process(RedMod, pvPUKCLParam);
    
    uint32_t ret = PUKCL(u2Status);
    
    if (ret != PUKCL_OK)
    {
        return ret;
    }

        memset(&PUKCLParam, 0, sizeof(PUKCL_PARAM));
    
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccverify.key->dp->Gx, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSAV_POINT_A_X(size, size), size+4, (void*)(&scratchLI.dp), size, 0);
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccverify.key->dp->Gy, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSAV_POINT_A_Y(size, size), size+4, (void*)(&scratchLI.dp), size, 0);
    uint8_t z = 1;
    SecureCopy((void*)BASE_ECDSAV_POINT_A_Z(size, size), size+4, &z, sizeof(z), 0);
    
    
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccverify.key->dp->order, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSAV_ORDER(size, size), size+4, (void*)(&scratchLI.dp), size, 0);
    mp_clear(&scratchLI);
    mp_read_radix(&scratchLI, info->pk.eccverify.key->dp->Af, MP_RADIX_HEX);
    SecureCopy((void*)BASE_ECDSAV_A(size, size), size+4, (void*)(&scratchLI.dp), size, 0);

    SecureCopy((void*)BASE_ECDSAV_PUBLIC_KEY_X(size, size), size+4, (void*)(&info->pk.eccverify.key->pubkey.x[0].dp), info->pk.eccverify.key->pubkey.x[0].used * 4, 0);
    SecureCopy((void*)BASE_ECDSAV_PUBLIC_KEY_Y(size, size), size+4, (void*)(&info->pk.eccverify.key->pubkey.y[0].dp), info->pk.eccverify.key->pubkey.y[0].used * 4, 0);
    SecureCopy((void*)BASE_ECDSAV_PUBLIC_KEY_Z(size, size), size+4, (void*)(&info->pk.eccverify.key->pubkey.z[0].dp), info->pk.eccverify.key->pubkey.z[0].used * 4, 0);
    
    SecureCopy((void*)BASE_ECDSAV_HASH(size, size), size+4, info->pk.eccverify.hash, MIN(info->pk.eccverify.hashlen, size), 1);    
    
    mp_int r, s;
    mp_clear(&r);
    mp_clear(&s);

    ret = DecodeECC_DSA_Sig(info->pk.eccverify.sig, info->pk.eccverify.siglen, &r, &s);
    if (ret != 0)
    {
        return ret;
    }
    SecureCopy((void*)BASE_ECDSAV_SIGNATURE_R(size, size), size+4, (void*)(r.dp), r.used *4, 0);
    SecureCopy((void*)BASE_ECDSAV_SIGNATURE_S(size, size), size+4, (void*)(s.dp), s.used *4, 0);
    
    memset((void*)BASE_ECDSAV_WORKSPACE(size, size), 0, (8*size)+44);
    
    PUKCL(u2Option) = 0;
    PUKCL_ZpEcDsaVerify(u2ModLength) = size;
    PUKCL_ZpEcDsaVerify(nu1ModBase) = (nu1)BASE_ECDSAV_MODULO(size, size);
    PUKCL_ZpEcDsaVerify(nu1CnsBase) = (nu1)BASE_ECDSAV_CNS(size, size);
    PUKCL_ZpEcDsaVerify(nu1PointABase) = (nu1)BASE_ECDSAV_POINT_A(size, size);
    PUKCL_ZpEcDsaVerify(nu1OrderPointBase) = (nu1)BASE_ECDSAV_ORDER(size, size);
    PUKCL_ZpEcDsaVerify(nu1ABase) = (nu1)BASE_ECDSAV_A(size, size);
    PUKCL_ZpEcDsaVerify(nu1PointPublicKeyGen) = (nu1)BASE_ECDSAV_PUBLIC_KEY(size, size);
    PUKCL_ZpEcDsaVerify(u2ScalarLength) = size;
    PUKCL_ZpEcDsaVerify(nu1HashBase) = (nu1)BASE_ECDSAV_HASH(size, size);
    PUKCL_ZpEcDsaVerify(nu1PointSignature) = (nu1)BASE_ECDSAV_SIGNATURE(size, size);
    PUKCL_ZpEcDsaVerify(nu1Workspace) = (nu1)BASE_ECDSAV_WORKSPACE(size, size);    
    
    vPUKCL_Process(ZpEcDsaVerifyFast, pvPUKCLParam);
    
    ret = PUKCL(u2Status);
    
    if (ret == PUKCL_OK)
    {
        *info->pk.eccverify.res = 1;
        return 0;
    }
    else if (ret ==  PUKCL_WRONG_SIGNATURE)
    {
        *info->pk.eccverify.res = 0;
        return 0;        
    }
    
    return ret;
    
}

#endif