/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "configuration.h"
#include "sys_random_h2_adapter.h"

#include "crypto/crypto.h"


static CRYPT_RNG_CTX sysRandCtx;
static CRYPT_RNG_CTX* pRandCtx = 0;


SYS_MODULE_OBJ SYS_RANDOM_CryptoInitialize(void)
{
    if(CRYPT_RNG_Initialize(&sysRandCtx) >= 0)
    {   // success
        pRandCtx = &sysRandCtx;
    }
    else
    {   // failure
        pRandCtx = 0;
    }

    return (SYS_MODULE_OBJ)pRandCtx;    
}

void SYS_RANDOM_CryptoDeinitialize( SYS_MODULE_OBJ object )
{
    if(pRandCtx != 0 && (CRYPT_RNG_CTX*)object == pRandCtx)
    {
        // CRYPT_RNG_Deinitialize(pRandCtx);
        pRandCtx = 0;
    } 
}

static CRYPT_RNG_CTX* _SYS_RANDOM_CryptoContext(void)
{
    if(pRandCtx == 0)
    {
        SYS_RANDOM_CryptoInitialize();
    }

    return pRandCtx;    
} 

uint32_t SYS_RANDOM_CryptoGet( void )
{
    union
    {
        uint32_t    u32;
        uint8_t     u8[4];
    }sUint = {0};

    CRYPT_RNG_CTX* pCtx = _SYS_RANDOM_CryptoContext();

    if(pCtx)
    {
        CRYPT_RNG_BlockGenerate(pCtx, (unsigned char*)sUint.u8, sizeof(sUint.u8));
        return sUint.u32;
    }

    return 0;
}



size_t SYS_RANDOM_CryptoBlockGet( void *buffer, size_t size )
{
    if(buffer != 0 && size != 0)
    {
        CRYPT_RNG_CTX* pCtx = _SYS_RANDOM_CryptoContext();
        if(pCtx)
        {
            CRYPT_RNG_BlockGenerate(pCtx, (uint8_t*)buffer, size);
            return size;
        }
    }

    return 0;
}

uint8_t SYS_RANDOM_CryptoByteGet( void )
{
    uint8_t rNo = 0;
    CRYPT_RNG_CTX* pCtx = _SYS_RANDOM_CryptoContext();
    if(pCtx)
    {
        CRYPT_RNG_Get(pCtx, &rNo);
    }

    return rNo;
}


