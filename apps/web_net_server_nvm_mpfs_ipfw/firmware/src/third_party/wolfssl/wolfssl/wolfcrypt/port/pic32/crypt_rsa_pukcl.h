/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_ecc_pukcl.h
  
  Summary:
    Crypto Framework Library header the pukcl ecc functions

  Description:
    This header contains the function definitions for the pukcl ecc functions
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _CRYPT_RSA_PUKCL_H_
#define _CRYPT_RSA_PUKCL_H_

#include "configuration.h"
#include "wolfssl/wolfcrypt/cryptocb.h"

#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_RSA_HW_PUKCC) && defined(WOLFSSL_HAVE_MCHP_HW_RSA)

/*
 * Determine the mapping for parameters in Crypto RAM for the RSA operation (without CRT)
 * If some data has to be put after the <PRECOMP> area, consult the PUKCL chapter in datasheet
 * compute the size of this area depending on the window size (1,2,3 or 4).
 */
#define BASE_MODULUS(a) AT91C_BASE_CRYPTO_RAM
#define BASE_CONSTANT(a) (BASE_MODULUS(a) + ((a) + 4))
#define BASE_MESSAGE(a) (BASE_CONSTANT(a) + ((a) + 8))
#define BASE_PRECOMP(a) (BASE_MESSAGE(a) + ((a) + 16))
#define BASE_EXP_BLINDED(a) (BASE_PRECOMP(a) + 3 * (a + 4) + 8)

/*
 * Determine the mapping for computation of Phi(N)
 */
#define KEYGEN_BASE_P(a) AT91C_BASE_CRYPTO_RAM
#define KEYGEN_BASE_PRECOMP(a) (KEYGEN_BASE_P(a) + (a) + 4)
#define KEYGEN_BASE_YGCD(a) (KEYGEN_BASE_PRECOMP(a) + (a) + 4)

/*
 * Determine the mapping for parameters in Crypto RAM for the RSA operation with CRT
 * If some data has to be put after the <PRECOMP> area, consult the PUKCL chapter in datasheet
 * compute the size of this area depending on the window size (1,2,3 or 4).
 *   CRT mapping
 *   Data Name                Size                          Start Offset
 *   => Modulus               [2 * u2PrimeLength +  8]      0
 *   => Message               [2 * u2PrimeLength + 16]      2 * u2PrimeLength +  8
 *   => Exponent              [2 * u2PrimeLength + 8]       4 * u2PrimeLength +  24
 *   => EP                    [u2PrimeLength + 4]			5 * u2PrimeLength +  28
 *   => EQ                    [u2PrimeLength + 4]			6 * u2PrimeLength +  32
 *   => Precomp               [option dependant]            8 * u2PrimeLength +  36
 *----------------------------------------------------------------------------------------------
 *   GCD mapping
 *   Data Name                Size                          Start Offset
 *   => GCD A                 [2 * u2PrimeLength + 8]       8 * u2PrimeLength +  36
 *   => GCD Z                 [2 * u2PrimeLength + 8]       10 * u2PrimeLength +  44
 *   => GCD X                 [u2PrimeLength + 4]           11 * u2PrimeLength +  48
 *   => GCD Y                 [u2PrimeLength + 4]           12 * u2PrimeLength +  52
 *----------------------------------------------------------------------------------------------
 *   Division mapping
 *   Data Name                Size                          Start Offset
 *   => DivNum                [2 * u2PrimeLength + 8]       8 * u2PrimeLength +  36
 *   => DivWork               [64]                          10 * u2PrimeLength +  44
 *----------------------------------------------------------------------------------------------
 */
#define BASECRT_MODULUS(a) AT91C_BASE_CRYPTO_RAM
#define LENCRT_MODULUS(a) (u2)(2 * (a) + 8)
#define BASECRT_P(a) (BASECRT_MODULUS(a))
#define BASECRT_Q(a) (BASECRT_MODULUS(a) + (a) + 4)
#define BASECRT_MESSAGE(a) (BASECRT_MODULUS(a) + LENCRT_MODULUS(a))
#define LENCRT_MESSAGE(a) (u2)(2 * (a) + 16)
#define BASECRT_EXP(a) (BASECRT_MESSAGE(a) + LENCRT_MESSAGE(a))
#define LENCRT_EXP(a) (u2)(2 * (a) + 8)
#define BASECRT_EXP_EP(a) (BASECRT_EXP(a) + 4)
#define BASECRT_EXP_EQ(a) (BASECRT_EXP(a) + (a) + 8)
#define BASECRT_PRECOMP(a) (BASECRT_EXP(a) + LENCRT_EXP(a))

#define BASECRT_GCDA(a) (BASECRT_PRECOMP(a))
#define BASECRT_GCDZ(a) (BASECRT_GCDA(a) + (a) + 4)
#define BASECRT_GCDX(a) (BASECRT_GCDZ(a) + (a) + 8)
#define BASECRT_GCDY(a) (BASECRT_GCDX(a) + (a) + 4)
#define BASECRT_WORK(a) (BASECRT_GCDY(a) + (a) + 4)

#define BASECRT_DIVNUM(a) (BASECRT_PRECOMP(a) + (a) + 4)
#define BASECRT_DIVWORK(a) (BASECRT_DIVNUM(a) + (2 * (a) + 8))

/*
 * Determine the mapping for the RSA operation (with CRT)
 * If some data has to be put after the <PRECOMP> area, consult the PUKCL chapter in datasheet
 * compute the size of this area depending on the window size (1,2,3 or 4).
 *   CRT mapping
 *   Data Name                Size                          Start Offset
 *   => Modulus               [2 * u2PrimeLength +  8]      0
 *   => Message               [2 * u2PrimeLength + 16]      2 * u2PrimeLength +  8
 *   => Exponent              [2 * u2PrimeLength + 8]       4 * u2PrimeLength +  24
 *   => EP                    [u2PrimeLength + 4]			5 * u2PrimeLength +  28
 *   => EQ                    [u2PrimeLength + 4]			6 * u2PrimeLength +  32
 *   => Precomp               [option dependant]            8 * u2PrimeLength +  36
 *----------------------------------------------------------------------------------------------
 *   GCD mapping
 *   Data Name                Size                          Start Offset
 *   => GCD A                 [2 * u2PrimeLength + 8]       8 * u2PrimeLength +  36
 *   => GCD Z                 [2 * u2PrimeLength + 8]       10 * u2PrimeLength +  44
 *   => GCD X                 [u2PrimeLength + 4]           11 * u2PrimeLength +  48
 *   => GCD Y                 [u2PrimeLength + 4]           12 * u2PrimeLength +  52
 *----------------------------------------------------------------------------------------------
 *   Division mapping
 *   Data Name                Size                          Start Offset
 *   => DivNum                [2 * u2PrimeLength + 8]       8 * u2PrimeLength +  36
 *   => DivWork               [64]                          10 * u2PrimeLength +  44
 *----------------------------------------------------------------------------------------------
 */
#define BASECRTB_MODULUS(a) AT91C_BASE_CRYPTO_RAM
#define LENCRTB_MODULUS(a) (u2)(2 * (a) + 8)
#define BASECRTB_P(a) (BASECRTB_MODULUS(a))
#define BASECRTB_Q(a) (BASECRTB_MODULUS(a) + (a) + 4)
#define BASECRTB_MESSAGE(a) (BASECRTB_MODULUS(a) + LENCRTB_MODULUS(a))
#define LENCRTB_MESSAGE(a) (u2)(2 * (a) + 16)
#define BASECRTB_EXP(a) (BASECRTB_MESSAGE(a) + LENCRTB_MESSAGE(a))
#define LENCRTB_EXP(a) (u2)(2 * (a) + 16)
#define BASECRTB_EXP_EP(a) (BASECRTB_EXP(a) + 4)
#define BASECRTB_EXP_EQ(a) (BASECRTB_EXP(a) + (a) + 12)
#define BASECRTB_PRECOMP(a) (BASECRTB_EXP(a) + LENCRTB_EXP(a))

#define BASECRTB_GCDA(a) (BASECRTB_PRECOMP(a))
#define BASECRTB_GCDZ(a) (BASECRTB_GCDA(a) + (a) + 4)
#define BASECRTB_GCDX(a) (BASECRTB_GCDZ(a) + (a) + 8)
#define BASECRTB_GCDY(a) (BASECRTB_GCDX(a) + (a) + 4)
#define BASECRTB_WORK(a) (BASECRTB_GCDY(a) + (a) + 4)

#define BASECRTB_DIVNUM(a) (BASECRTB_PRECOMP(a) + (a) + 4)
#define BASECRTB_DIVWORK(a) (BASECRTB_DIVNUM(a) + (2 * (a) + 8))

/* Addresses for the SHA1 */
#define BASESHA1_HASH AT91C_BASE_CRYPTO_RAM
#define BASESHA1_MESSAGE (BASESHA1_HASH + 20)

/*
 * Macro definition used to test which RSA
 * arguments are initialized
 */
#define MODULUS_IS_PRESENT 1
#define PUBLIC_EXPONENT_IS_PRESENT (1 << 1)
#define PRIVATE_EXPONENT_IS_PRESENT (1 << 2)
#define PRIME_P_IS_PRESENT (1 << 3)
#define PRIME_Q_IS_PRESENT (1 << 4)
#define PHI_N_IS_PRESENT (1 << 5)
#define MODE_CRT (1 << 6)
#define MODE_NO_CRT (1 << 7)

/*
 * Macro definition used for computation of RSA exponentiation
 */
#define PUBLIC_EXPONENTIATION 1
#define PRIVATE_EXPONENTIATION (1 << 2)
#define EXPONENT_BLINDING (1 << 3)

/*
 * Error codes for RSA use (encryption, decryption, signature,
 * keys management)
 */
typedef enum {
	ERR_MODULUS_LENGTH = 1,
	ERR_PUB_EXP_LENGTH,
	ERR_PRI_EXP_LENGTH,
	ERR_PUB_EXP,
	ERR_PRI_EXP,
	ERR_PRIME_P,
	ERR_PRIME_Q,
	ERR_MODULUS,
	ERR_EXPONENTIATION,
	ERR_MODE,
	ERR_CO_PRIMALITY,
	ERR_P_GENERATION,
	ERR_Q_GENERATION,
	ERR_PRIVATE_KEY_GENERATION,
	ERR_N_GENERATION,
	ERR_PHI_N_GENERATION,
	ERR_PHI_N,
	ERR_HASH_INCONSISTENT
} RSAErrors;

int Crypt_RSA_HandleReq(int devId, wc_CryptoInfo* info, void* ctx);
int Crypt_RSA_PublicEncrypt(int devId, wc_CryptoInfo* info, void* ctx);
int Crypt_RSA_PublicDecrypt(int devId, wc_CryptoInfo* info, void* ctx);
int Crypt_RSA_PrivateEncrypt(int devId, wc_CryptoInfo* info, void* ctx);
int Crypt_RSA_PrivateDecrypt(int devId, wc_CryptoInfo* info, void* ctx);

#endif

#endif //_CRYPT_RSA_PUKCL_H_