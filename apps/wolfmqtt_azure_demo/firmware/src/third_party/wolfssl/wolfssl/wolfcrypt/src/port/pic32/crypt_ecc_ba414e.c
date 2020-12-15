
/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_ecc_ba414e.c

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



#include "configuration.h"


#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_ECC_HW_BA414E) && defined(WOLFSSL_HAVE_MCHP_HW_ECC)
#include "wolfssl/wolfcrypt/port/pic32/crypt_ecc_ba414e.h"
#include "wolfssl/wolfcrypt/ecc.h"
#include "wolfssl/wolfcrypt/cryptocb.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/random.h"
#include "wolfssl/wolfcrypt/integer.h"
#include "wolfssl/wolfcrypt/asn.h"
#include <string.h>
#include <stdint.h>

#include "driver/ba414e/drv_ba414e.h"

int Crypt_ECC_HandleReq(int devId, wc_CryptoInfo* info, void* ctx)
{
    DRV_HANDLE ba414Handle = DRV_BA414E_Open(DRV_BA414E_INDEX_0, DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_BLOCKING);
    int ret = CRYPTOCB_UNAVAILABLE;
    if (ba414Handle != DRV_HANDLE_INVALID)
    {        
        if (info->pk.type == WC_PK_TYPE_ECDSA_SIGN)
        {
            ret = Crypt_ECC_HandleEccSignReq(devId, info, ctx, ba414Handle);
        }

        if (info->pk.type == WC_PK_TYPE_ECDSA_VERIFY)
        {
            ret = Crypt_ECC_HandleEccVerifyReq(devId, info, ctx, ba414Handle);
        }
        DRV_BA414E_Close(ba414Handle);
    }
    return ret;
}

DRV_BA414E_ECC_DOMAIN domain;
/* This is a random scratch pad and should change on every run of the ECC sign operation*/
static uint8_t eccScratchPad[70] = {
    0x7c, 0xec, 0xc1, 0xb4, 0x19, 0x15, 0x41, 0xe7, 0x2f, 0xa5, 0x03, 0xc3, 0x00, 0x0a, 0xba, 0x99,
    0x58, 0x35, 0xd2, 0xab, 0xb9, 0x8d, 0xd2, 0x52, 0x3e, 0xb9, 0xda, 0x54, 0xaf, 0x05, 0x94, 0xc6};


int Crypt_ECC_HandleEccSignReq(int devId, wc_CryptoInfo* info, void* ctx, DRV_HANDLE ba414eClient)
{
    memset(&domain, 0, sizeof(DRV_BA414E_ECC_DOMAIN));
    
    domain.keySize = info->pk.eccsign.key->dp->size;
    domain.opSize = info->pk.eccsign.key->dp->size >> 3;
    mp_int primeField;
    mp_clear(&primeField);
    mp_read_radix(&primeField, info->pk.eccsign.key->dp->prime, MP_RADIX_HEX);
    domain.primeField = (uint8_t*)&(primeField.dp);

    mp_int order;
    mp_clear(&order);
    mp_read_radix(&order, info->pk.eccsign.key->dp->order, MP_RADIX_HEX);
    domain.order = (uint8_t*)&(order.dp);

    mp_int generatorX;
    mp_clear(&generatorX);
    mp_read_radix(&generatorX, info->pk.eccsign.key->dp->Gx, MP_RADIX_HEX);
    domain.generatorX = (uint8_t*)&(generatorX.dp);

    mp_int generatorY;
    mp_clear(&generatorY);
    mp_read_radix(&generatorY, info->pk.eccsign.key->dp->Gy, MP_RADIX_HEX);
    domain.generatorY = (uint8_t*)&(generatorY.dp);

    mp_int Af;
    mp_clear(&Af);
    mp_read_radix(&Af, info->pk.eccsign.key->dp->Af, MP_RADIX_HEX);
    domain.a = (uint8_t*)&(Af.dp);

    mp_int Bf;
    mp_clear(&Bf);
    mp_read_radix(&Bf, info->pk.eccsign.key->dp->Bf, MP_RADIX_HEX);
    domain.b = (uint8_t*)&(Bf.dp);
    
    WC_RNG rngCtx;
    wc_InitRng(&rngCtx);
    wc_RNG_GenerateBlock(&rngCtx, eccScratchPad, domain.keySize);
    
    memset(info->pk.eccsign.out, 0, *(info->pk.eccsign.outlen));

    mp_int r, s;
    mp_clear(&r);
    mp_clear(&s);
    
    DRV_BA414E_OP_RESULT ret = DRV_BA414E_ECDSA_Sign(ba414eClient, 
            &domain, 
            (uint8_t*)&(r.dp),
            (uint8_t*)&(s.dp),
            (uint8_t*)&(info->pk.eccsign.key->k.dp),
            eccScratchPad,
            info->pk.eccsign.in,
            info->pk.eccsign.inlen,
            0, 0            
            );
    
    if (ret == DRV_BA414E_OP_SUCCESS)
    {    
        r.used = domain.keySize/4;
        s.used = domain.keySize/4;
        memset(info->pk.eccsign.out, 0, *(info->pk.eccsign.outlen));
        ret = StoreECC_DSA_Sig(info->pk.eccsign.out, info->pk.eccsign.outlen, &r, &s);
    }
    return ret;
}

int Crypt_ECC_HandleEccVerifyReq(int devId, wc_CryptoInfo* info, void* ctx, DRV_HANDLE ba414eClient)
{
    memset(&domain, 0, sizeof(DRV_BA414E_ECC_DOMAIN));
    mp_int r, s;
    mp_clear(&r);
    mp_clear(&s);
    

    domain.keySize = info->pk.eccsign.key->dp->size;
    DecodeECC_DSA_Sig(info->pk.eccverify.sig, info->pk.eccverify.siglen, &r, &s);

    domain.opSize = info->pk.eccsign.key->dp->size >> 3;
    mp_int primeField;
    mp_clear(&primeField);
    mp_read_radix(&primeField, info->pk.eccverify.key->dp->prime, MP_RADIX_HEX);
    domain.primeField = (uint8_t*)&(primeField.dp);

    mp_int order;
    mp_clear(&order);
    mp_read_radix(&order, info->pk.eccverify.key->dp->order, MP_RADIX_HEX);
    domain.order = (uint8_t*)&(order.dp);

    mp_int generatorX;
    mp_clear(&generatorX);
    mp_read_radix(&generatorX, info->pk.eccverify.key->dp->Gx, MP_RADIX_HEX);
    domain.generatorX = (uint8_t*)&(generatorX.dp);

    mp_int generatorY;
    mp_clear(&generatorY);
    mp_read_radix(&generatorY, info->pk.eccverify.key->dp->Gy, MP_RADIX_HEX);
    domain.generatorY = (uint8_t*)&(generatorY.dp);

    mp_int Af;
    mp_clear(&Af);
    mp_read_radix(&Af, info->pk.eccverify.key->dp->Af, MP_RADIX_HEX);
    domain.a = (uint8_t*)&(Af.dp);

    mp_int Bf;
    mp_clear(&Bf);
    mp_read_radix(&Bf, info->pk.eccverify.key->dp->Bf, MP_RADIX_HEX);
    domain.b = (uint8_t*)&(Bf.dp);
    
    DRV_BA414E_OP_RESULT ret = DRV_BA414E_ECDSA_Verify(
            ba414eClient, &domain, 
            (uint8_t*)&(info->pk.eccverify.key->pubkey.x[0].dp),
            (uint8_t*)&(info->pk.eccverify.key->pubkey.y[0].dp),
            (uint8_t*)&(r.dp),
            (uint8_t*)&(s.dp),
            info->pk.eccverify.hash,
            info->pk.eccverify.hashlen,
            0, 0            
            );
    
    if (ret == DRV_BA414E_OP_SUCCESS)
    {
        *info->pk.eccverify.res = 1;
        return 0;
    }
    else if (ret ==  DRV_BA414E_OP_SIGN_VERIFY_FAIL)
    {
        *info->pk.eccverify.res = 0;
        return 0;        
    }
    
    return ret;
    
}


#endif
