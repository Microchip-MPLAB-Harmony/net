/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_wolfcryptcb.c

  Summary:
    Crypto Framework Libarary source file for the wolfcrypt callback

  Description:
    This file contains the wolfssl callback for public key cryptography
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
#include "wolfssl/wolfcrypt/port/pic32/crypt_wolfcryptcb.h"
#include "wolfssl/wolfcrypt/cryptocb.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_rsa_pukcl.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_ecc_pukcl.h"


int CRYPT_WCCB_Callback(int devId, wc_CryptoInfo* info, void* ctx)
{
    if (info->algo_type == WC_ALGO_TYPE_PK)
    {
#if defined(WOLFSSL_HAVE_MCHP_HW_RSA)
        if (info->pk.type == WC_PK_TYPE_RSA)
        {
            return Crypt_RSA_HandleReq(devId, info, ctx);
        }
#endif
#if defined(WOLFSSL_HAVE_MCHP_HW_ECC)
        if ((info->pk.type == WC_PK_TYPE_ECDSA_SIGN) ||
            (info->pk.type == WC_PK_TYPE_ECDSA_VERIFY))
        {
            return Crypt_ECC_HandleReq(devId, info, ctx);
        }
#endif
    }    
    return CRYPTOCB_UNAVAILABLE;
}

void CRYPT_WCCB_Initialize()
{
    wc_CryptoCb_RegisterDevice(0, CRYPT_WCCB_Callback, NULL);
}