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




uint32_t SYS_RANDOM_CryptoGet( void )
{
    union
    {
        uint32_t    u32;
        uint8_t     u8[4];
    }sUint = {0};

    CRYPT_RNG_CTX rngCtx;
    
    CRYPT_RNG_Initialize(&rngCtx);

    CRYPT_RNG_BlockGenerate(&rngCtx, (unsigned char*)sUint.u8, sizeof(sUint.u8));

    return sUint.u32;
}



void SYS_RANDOM_CryptoBlockGet( void *buffer, size_t size )
{
    if(buffer != 0 && size != 0)
    {
        CRYPT_RNG_CTX rngCtx;
        CRYPT_RNG_Initialize(&rngCtx);

        CRYPT_RNG_BlockGenerate(&rngCtx, (uint8_t*)buffer, size);
    }
}

uint8_t SYS_RANDOM_CryptoByteGet( void )
{
    CRYPT_RNG_CTX rngCtx;
    CRYPT_RNG_Initialize(&rngCtx);

    uint8_t rNo = 0;
    CRYPT_RNG_Get(&rngCtx, (unsigned char*)&rNo);

    return rNo;
}


