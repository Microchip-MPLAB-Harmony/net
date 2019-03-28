/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    cpuid.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
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



#ifndef WOLF_CRYPT_CPUID_H
#define WOLF_CRYPT_CPUID_H


#include "crypto/src/types.h"


#ifdef __cplusplus
    extern "C" {
#endif

#if defined(WOLFSSL_X86_64_BUILD) || defined(USE_INTEL_SPEEDUP) || \
    defined(WOLFSSL_AESNI)
    #define CPUID_AVX1   0x0001
    #define CPUID_AVX2   0x0002
    #define CPUID_RDRAND 0x0004
    #define CPUID_RDSEED 0x0008
    #define CPUID_BMI2   0x0010   /* MULX, RORX */
    #define CPUID_AESNI  0x0020
    #define CPUID_ADX    0x0040   /* ADCX, ADOX */

    #define IS_INTEL_AVX1(f)    ((f) & CPUID_AVX1)
    #define IS_INTEL_AVX2(f)    ((f) & CPUID_AVX2)
    #define IS_INTEL_RDRAND(f)  ((f) & CPUID_RDRAND)
    #define IS_INTEL_RDSEED(f)  ((f) & CPUID_RDSEED)
    #define IS_INTEL_BMI2(f)    ((f) & CPUID_BMI2)
    #define IS_INTEL_AESNI(f)   ((f) & CPUID_AESNI)
    #define IS_INTEL_ADX(f)     ((f) & CPUID_ADX)

    void cpuid_set_flags(void);
    word32 cpuid_get_flags(void);
#endif

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif /* WOLF_CRYPT_CPUID_H */
