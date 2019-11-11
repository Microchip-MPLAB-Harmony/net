/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_pukcl_functions.c

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

#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_typedef_pb.h"
#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_mapping_pb.h"
#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_cf_pb.h"
#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_Headers_pb.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_pukcl_functions.h"
#include <stdint.h>
#include <string.h>

PPUKCL_PARAM pvPUKCLParam;
PUKCL_PARAM  PUKCLParam;

#define PUKCL_SELFTEST_CHECKSUM1 0x6E70DDD2
#define PUKCL_SELFTEST_CHECKSUM2 0x25c8d64f


int8_t SelfTestState = 0;

int8_t crypt_pukcl_SelfTest(void)
{
    if (SelfTestState != 0)
    {
        return SelfTestState;
    }
	/* Clear contents of PUKCLParam */
	memset(&PUKCLParam, 0, sizeof(PUKCL_PARAM));

	pvPUKCLParam = &PUKCLParam;
	vPUKCL_Process(SelfTest, pvPUKCLParam);

	while (PUKCL(u2Status) != PUKCL_OK)
		;

    
	if (pvPUKCLParam->P.PUKCL_SelfTest.u4Version != PUKCL_VERSION)
    {
        SelfTestState = -1;
    }
    else if (pvPUKCLParam->P.PUKCL_SelfTest.u4CheckNum1 != PUKCL_SELFTEST_CHECKSUM1)
    {
        SelfTestState = -2;
    }
	else if (pvPUKCLParam->P.PUKCL_SelfTest.u4CheckNum2 != PUKCL_SELFTEST_CHECKSUM2)
    {
        SelfTestState = -3;
    }
    else
    {
        SelfTestState = 1;
    }
    return SelfTestState;
}

uint32_t SecureCopy(uint8_t * dst, uint32_t dstSz, const uint8_t * src, uint32_t srcSz, uint8_t reverse)
{
    // clean out the destination.
    memset(dst, 0, dstSz);
    uint32_t bytesToCopy = dstSz < srcSz ? dstSz : srcSz;
    if (reverse == 0)
    {
        uint32_t x = 0;
        for (x = 0; x < bytesToCopy; x++)
        {
            *(dst++) = *(src++);
        }
    }
    else
    {
        uint8_t * revDst = dst + bytesToCopy - 1;
        uint32_t x = 0;
        for (x = 0; x < bytesToCopy; x++)
        {
            *(revDst--) = *(src++);
        }        
    }
    return bytesToCopy;
}