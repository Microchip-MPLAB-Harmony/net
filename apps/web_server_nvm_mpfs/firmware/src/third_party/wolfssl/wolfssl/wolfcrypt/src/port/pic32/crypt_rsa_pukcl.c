/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_rsa_pukcl.c

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

#include "wolfssl/wolfcrypt/port/pic32/crypt_rsa_pukcl.h"
#include "wolfssl/wolfcrypt/rsa.h"
#include "wolfssl/wolfcrypt/cryptocb.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_pukcl_functions.h"
#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_Hardware_pb.h"
#include "wolfssl/wolfcrypt/port/pic32/CryptoLib_Headers_pb.h"
#include "wolfssl/wolfcrypt/random.h"
#include <string.h>
#include <stdint.h>

#if defined(WOLFSSL_HAVE_MCHP_HW_CRYPTO_RSA_HW_PUKCC) && defined(WOLFSSL_HAVE_MCHP_HW_RSA)

#define MODULUS_IS_PRESENT 1
#define PUBLIC_EXPONENT_IS_PRESENT (1 << 1)
#define PRIVATE_EXPONENT_IS_PRESENT (1 << 2)
#define PRIME_P_IS_PRESENT (1 << 3)
#define PRIME_Q_IS_PRESENT (1 << 4)
#define PHI_N_IS_PRESENT (1 << 5)
#define MODE_CRT (1 << 6)
#define MODE_NO_CRT (1 << 7)

typedef struct {

	/* Mask that indicates which fields are initialized */
	u1 u2FieldIsPresent;
	/* Modulus */
	pfu1 pfu1Modulus;
	/* Effective length of the modulus */
	u2 u2ModulusLength;
	/* Public exponent */
	pfu1 pfu1PublicExponent;
	/* Effective length of the public exponent */
	u2 u2PublicExponentLength;
	/* Private exponent */
	pfu1 pfu1PrivateExponent;
	/* Effective length of the private exponent */
	u2 u2PrivateExponentLength;
	/* Prime factor P of N */
	pfu1 pfu1PrimeP;
	/* Prime factor Q of N */
	pfu1 pfu1PrimeQ;
	/* Phi(N) = (P-1)*(Q-1) */
	pfu1 pfu1PhiN;
} _rsa_params;

_rsa_params     parametersRSA;
_rsa_params *   pParametersRSA;

static u1 au1RandomTable[12];

/* RSA key size used is 4096 bits = 512 bytes */
#define KEYSPACESIZE 512

/* This data segment is designed to store RSA keys once created in RAM  */
u1 au1KeySpace[KEYSPACESIZE];

/**
 * \brief Copy from SRAM to PUKCC RAM
 *
 * Copy bytes from SRAM and load them to PUKCC RAM. Change the endianess
 */
void pukcc_memcopy(pu1 pu1Dest, const byte* pu1Src, u2 u2Length)
{
	u2  u2Cpt;
	pu1 pu1PtrDest;

	u2Cpt      = 0;
	pu1PtrDest = pu1Dest;
	while (u2Cpt < u2Length) {
		*(pu1PtrDest++) = pu1Src[u2Length - u2Cpt - 1];
		u2Cpt++;
	}
}



/**
 * \brief Computes Phi(N)
 *
 * This function computes  Phi(N) = (P - 1)*(Q - 1)
 * for RSA modular exponentiation
 *
 * P is supposed to be stored in the KEYGEN_BASE_P area
 * Q in the KEYGEN_BASE_PRECOMP area
 * After computation, Phi(N) is stored at KEYGEN_BASE_YGCD address
 */
extern u2 compute_phi_n(u2 u2PrimeLength)
{

	/* Computes P-1 and Q-1
	 * Since both are P and Q are prime, clear the least significant bit
	 * to find P-1 and Q-1
	 */
	*(pu1)(KEYGEN_BASE_PRECOMP(u2PrimeLength)) &= 0xfe;
	*(pu1)(KEYGEN_BASE_P(u2PrimeLength)) &= 0xfe;

	/* Computes N = P*Q */
	PUKCL(Specific).Gf2n    = 0;
	PUKCL(Specific).CarryIn = 0;
	PUKCL(u2Option)         = SET_MULTIPLIEROPTION(PUKCL_FMULT_ONLY) | SET_CARRYOPTION(CARRY_NONE);

	PUKCL_Fmult(nu1XBase)  = (nu1)(KEYGEN_BASE_P(u2PrimeLength));
	PUKCL_Fmult(u2XLength) = u2PrimeLength;
	PUKCL_Fmult(nu1YBase)  = (nu1)(KEYGEN_BASE_PRECOMP(u2PrimeLength));
	PUKCL_Fmult(u2YLength) = u2PrimeLength;
	PUKCL_Fmult(nu1RBase)  = (nu1)(KEYGEN_BASE_YGCD(u2PrimeLength));
	PUKCL_Fmult(nu1ZBase)  = (nu1)(KEYGEN_BASE_YGCD(u2PrimeLength));

	/* Processes N */
	vPUKCL_Process(Fmult, pvPUKCLParam);

	/* Restores P and Q from P-1 and Q from Q-1 */
	*(pu1)(KEYGEN_BASE_PRECOMP(u2PrimeLength)) |= 0x01;
	*(pu1)(KEYGEN_BASE_P(u2PrimeLength)) |= 0x01;

	return PUKCL(u2Status);
}

/**
 * \brief Computes the CNS constant for modular exponentiation
 *
 * Computes the CNS constant for modular exponentiation
 * The function performs Barrett reduction to form Cns value
 */
static u2 modular_operations_setup(pfu1 pfu1Modulus, u2 u2ModulusLength)
{
	/* Establish Mapping for the RSA Operations
	 * Note that some parameters of the reduction overlap those of the reduction
	 * PUKCL_ExpMod(pu1ModBase)  is the same as PUKCL_RedMod(pu1ModBase)
	 * PUKCL_ExpMod(pu1CnsBase)  is the same as PUKCL_RedMod(pu1CnsBase)
	 * PUKCL_ExpMod(u2ModLength) is the same as PUKCL_RedMod(u2ModLength)
	 */
	PUKCL_RedMod(nu1ModBase)  = (nu1)BASE_MODULUS(u2ModulusLength);
	PUKCL_RedMod(nu1CnsBase)  = (nu1)BASE_CONSTANT(u2ModulusLength);
	PUKCL_RedMod(u2ModLength) = u2ModulusLength;
	PUKCL_RedMod(nu1RBase)    = (nu1)BASE_MESSAGE(u2ModulusLength);
	PUKCL_RedMod(nu1XBase)    = (nu1)BASE_PRECOMP(u2ModulusLength);

	/* Get the modulus and swap the bytes since PUKCC wants them lsb first)
	 */
	pukcc_memcopy((pu1)BASE_MODULUS(u2ModulusLength), pfu1Modulus, (u2)(u2ModulusLength + 4));

	/* Computes the Reduction constant CNS */
	PUKCL(u2Option) = PUKCL_REDMOD_SETUP;
	vPUKCL_Process(RedMod, pvPUKCLParam);

	return PUKCL(u2Status);
}

/**
 * \brief Blind the exponent
 *
 * Blinds the private exponent with PHI(N) and a random
 * number k to : E' = E + k*PHI(N). The blinded exponent
 * is located at BASE_EXP_BLINDED address after the operation
 */
static u2 exponent_blinding_with_phi_n(u2 u2ModulusLength, const byte* pfu1PrivateExponent, u2 u2ExponentLength, const byte* pfu1PhiN)
{
	u4 u4MulValue;

	/* Some spaces cannot be used for computations
	 * such as BASE_MODULUS or BASE_CONSTANT
	 * since they are initialized after CNS set up
	 */

	/* It is recommended to generate random number for u4MulValue
	 * This example just copies an existing number
	 */
	u4MulValue = *((pu2)&au1RandomTable[0]);

	/* Copies E and PHI(N) into RAM PUKCC */
	memset((pu1)BASE_PRECOMP(u2ModulusLength), 0, u2ExponentLength + 4);
	memcpy((pu1)BASE_PRECOMP(u2ModulusLength), pfu1PrivateExponent + 4, u2ExponentLength);
	memcpy((pu1)BASE_MESSAGE(u2ModulusLength), pfu1PhiN + 4, u2ModulusLength);

	/* Computes the blinded exponent
	 * E' = E + k*Phi(N)
	 * This new exponent is stored in BASE_EXP_BLINDED
	 * Using Smult operation
	 * R = Z + X*MulValue
	 * X is Phi(N) at BASE_MODULUS address
	 * Z is u2PrivateExponent at BASE_PRECOMP address
	 * R is E' at BASE_EXP_BLINDED address
	 */
	PUKCL(Specific).Gf2n    = 0;
	PUKCL(Specific).CarryIn = 0;
	PUKCL(u2Option)         = SET_MULTIPLIEROPTION(PUKCL_SMULT_ADD) | SET_CARRYOPTION(CARRY_NONE);
	PUKCL_Smult(nu1XBase)   = (nu1)(BASE_MESSAGE(u2ModulusLength));
	PUKCL_Smult(u2XLength)  = u2ModulusLength;
	PUKCL_Smult(u4MulValue) = u4MulValue;
	PUKCL_Smult(u2RLength)  = (u2)(u2ExponentLength + 4);
	PUKCL_Smult(nu1RBase)   = (nu1)(BASE_EXP_BLINDED(u2ModulusLength) + 4);
	PUKCL_Smult(nu1ZBase)   = (nu1)(BASE_PRECOMP(u2ModulusLength));

	/* Computes the Smult */
	vPUKCL_Process(Smult, pvPUKCLParam);
	if (PUKCL(u2Status) == PUKCL_OK) {
		memset((pu1)BASE_EXP_BLINDED(u2ModulusLength), 0, 4);
	}
	return PUKCL(u2Status);
}

/**
 * \brief Computes modular exponent without using CRT
 *
 * Computes modular exponent of a message depending
 * on the CNS constant (previously processed)
 * The CNS value has already been computed with
 * the 'modular_operations_setup' function.
 * It is already stored at 'PUKCL_ExpMod(pu1CnsBase)' memory space.
 */
static u2 modular_exponentiation_without_crt(const byte* pfu1Message, u1 u1GetMessage, const byte* pfu1Exponent, u2 u2ExponentLength,
                                             u2 u2Mode, const byte* pfu1PhiN)
{
	/* Settings parameters before exponentiation with blinding */
	if (u2Mode & EXPONENT_BLINDING) {
		/* Computes blinded exponent
		 * The result is stored in BASE_EXP_BLINDED
		 */
		exponent_blinding_with_phi_n(pParametersRSA->u2ModulusLength, pfu1Exponent, u2ExponentLength, pfu1PhiN);

		PUKCL_ExpMod(u2ModLength) = pParametersRSA->u2ModulusLength;
		PUKCL_ExpMod(nu1ModBase)  = (nu1)(BASE_MODULUS(pParametersRSA->u2ModulusLength));
		PUKCL_ExpMod(nu1CnsBase)  = (nu1)(BASE_CONSTANT(pParametersRSA->u2ModulusLength));
		PUKCL_ExpMod(pfu1ExpBase) = (pfu1)BASE_EXP_BLINDED(PUKCL_ExpMod(u2ModLength));
		PUKCL_ExpMod(u2ExpLength) = (u2)(u2ExponentLength + 8);
	}
	/* Without blinding */
	else {
		PUKCL_ExpMod(u2ModLength) = pParametersRSA->u2ModulusLength;
		PUKCL_ExpMod(nu1ModBase)  = (nu1)(BASE_MODULUS(pParametersRSA->u2ModulusLength));
		PUKCL_ExpMod(nu1CnsBase)  = (nu1)(BASE_CONSTANT(pParametersRSA->u2ModulusLength));
		/* Let's use the private exponent without blinding */
		PUKCL_ExpMod(pfu1ExpBase) = pfu1Exponent;
		PUKCL_ExpMod(u2ExpLength) = u2ExponentLength;
	}
	PUKCL_ExpMod(nu1XBase)       = (nu1)(BASE_MESSAGE(PUKCL_ExpMod(u2ModLength)));
	PUKCL_ExpMod(nu1PrecompBase) = (nu1)(BASE_PRECOMP(PUKCL_ExpMod(u2ModLength)));
	PUKCL_ExpMod(u1Blinding)     = 0x00;

	/* Copy the message */
	if (u1GetMessage)
		pukcc_memcopy((pu1)BASE_MESSAGE(PUKCL_ExpMod(u2ModLength)), pfu1Message, PUKCL_ExpMod(u2ModLength));

	if (u2Mode & PRIVATE_EXPONENTIATION)
		/* Launches the exponentiation in SECURE mode,
		 * with the exponent in PUKCC RAM
		 */
		PUKCL(u2Option) = PUKCL_EXPMOD_WINDOWSIZE_1 | PUKCL_EXPMOD_REGULARRSA | PUKCL_EXPMOD_EXPINPUKCCRAM;
	else
		/* Launches the exponentiation in FAST mode,
		 * with the exponent in PUKCC RAM
		 */
		PUKCL(u2Option) = PUKCL_EXPMOD_WINDOWSIZE_1 | PUKCL_EXPMOD_FASTRSA | PUKCL_EXPMOD_EXPINPUKCCRAM;

	/* Modular Exponentiation */
	vPUKCL_Process(ExpMod, pvPUKCLParam);

	/* The result can be caught at 'PUKCL_ExpMod(pu1XBase)' address */
	return PUKCL(u2Status);
}

/**
 * \brief Computes modular exponent with CRT
 *
 * Computes modular exponent of a message depending
 * on the CNS constant (previously processed)
 * The CNS value has already been computed with
 * the "modular_operations_setup" function.
 * It is already stored in the "PUKCL_ExpMod(pu1CnsBase)" memory space.
 */
static u2 modular_exponentiation_with_crt(const byte* pfu1Message, const byte* pfu1Exponent, pfu1 pfu1P, pfu1 pfu1Q,
                                          u2 u2ExponentLength, u1 u1GetMessage, u2 u2PrimeLength, u2 u2Mode)
{
	pu4 pu4Ptr;

	/* Clear GCDA area used by the GCD service */
	PUKCL_Fill(nu1RBase)    = (nu1)(BASECRT_GCDA(u2PrimeLength));
	PUKCL_Fill(u2RLength)   = (u2)(2 * u2PrimeLength + 8);
	PUKCL_Fill(u4FillValue) = 0L;
	vPUKCL_Process(Fill, pvPUKCLParam);

	/* Clear GCDZ area used by the GCD service */
	PUKCL_Fill(nu1RBase) = (nu1)(BASECRT_GCDZ(u2PrimeLength));
	vPUKCL_Process(Fill, pvPUKCLParam);
	if (PUKCL(u2Status) != PUKCL_OK)
		return PUKCL(u2Status);

	/* Compute the R value */
	PUKCL_GCD(nu1XBase)     = (nu1)(BASECRT_GCDX(u2PrimeLength));
	PUKCL_GCD(nu1YBase)     = (nu1)(BASECRT_GCDY(u2PrimeLength));
	PUKCL_GCD(nu1ABase)     = (nu1)(BASECRT_GCDA(u2PrimeLength));
	PUKCL_GCD(nu1ZBase)     = (nu1)(BASECRT_GCDZ(u2PrimeLength));
	PUKCL_GCD(nu1WorkSpace) = (nu1)(BASECRT_WORK(u2PrimeLength));
	PUKCL_GCD(u2Length)     = (u2)(u2PrimeLength + 4);

	/* Copies the Prime, and the 4 padding bytes too */
	pukcc_memcopy((pu1)BASECRT_GCDX(u2PrimeLength), pfu1P, (u2)(u2PrimeLength + 4));
	pukcc_memcopy((pu1)BASECRT_GCDY(u2PrimeLength), pfu1Q, (u2)(u2PrimeLength + 4));

	/* Launch the GCD */
	vPUKCL_Process(GCD, pvPUKCLParam);
	if (PUKCL(u2Status) != PUKCL_OK)
		return PUKCL(u2Status);

	/* Note that the R value is now stored at "BASECRT_PRECOMP" address
	 * It will be used later in this code
	 * Now, compute EQ and EP
	 * EP = Exponent mod (P-1)
	 * EQ = Exponent mod (Q-1)
	 */

	/* Copy the Primes again (they are scratched by the GCD)
	 * and the 4 padding bytes too
	 */
	pukcc_memcopy((pu1)BASECRT_P(u2PrimeLength), pfu1P, (u2)(u2PrimeLength + 4));
	pukcc_memcopy((pu1)BASECRT_Q(u2PrimeLength), pfu1Q, (u2)(u2PrimeLength + 4));

	/* Decrement the P and Q values for EP and EQ computation */
	*((pu1)BASECRT_P(u2PrimeLength)) &= 0xfe;
	*((pu1)BASECRT_Q(u2PrimeLength)) &= 0xfe;

	/* Compute EP and EQ */
	memcpy((pu1)BASECRT_DIVNUM(u2PrimeLength), pfu1Exponent + 4, u2ExponentLength);
	PUKCL_Div(nu1NumBase)  = (nu1)(BASECRT_DIVNUM(u2PrimeLength));
	PUKCL_Div(u2NumLength) = u2ExponentLength;
	PUKCL_Div(nu1ModBase)  = (nu1)(BASECRT_P(u2PrimeLength));
	PUKCL_Div(u2ModLength) = u2PrimeLength;
	/* Since quotient is not used, set the address as zero */
	PUKCL_Div(nu1QuoBase)   = 0;
	PUKCL_Div(nu1WorkSpace) = (nu1)(BASECRT_DIVWORK(u2PrimeLength));
	PUKCL_Div(nu1RBase)     = (nu1)(BASECRT_EXP_EP(u2PrimeLength));
	vPUKCL_Process(Div, pvPUKCLParam);

	/* First, get the (full) exponent */
	memcpy((pu1)BASECRT_DIVNUM(u2PrimeLength), pfu1Exponent + 4, u2ExponentLength);

	PUKCL_Div(nu1ModBase) = (nu1)(BASECRT_Q(u2PrimeLength));
	PUKCL_Div(nu1RBase)   = (nu1)(BASECRT_EXP_EQ(u2PrimeLength));
	vPUKCL_Process(Div, pvPUKCLParam);
	if (PUKCL(u2Status) != PUKCL_OK) {
		return PUKCL(u2Status);
	}

	/* Adds a few zeros for padding */
	*(pu4)(BASECRT_EXP_EQ(u2PrimeLength) - 4) = 0L;
	*(pu4)(BASECRT_EXP_EP(u2PrimeLength) - 4) = 0L;
	/*
	 * Note that EP and EQ values are now stored at "BASECRT_EXP" address
	 * Now, configure and use the CRT service
	 */

	/* Prepares the last fields of the mapping */
	PUKCL_CRT(nu1ModBase)     = (nu1)(BASECRT_MODULUS(u2PrimeLength));
	PUKCL_CRT(u2ModLength)    = u2PrimeLength;
	PUKCL_CRT(nu1XBase)       = (nu1)(BASECRT_MESSAGE(u2PrimeLength));
	PUKCL_CRT(nu1PrecompBase) = (nu1)(BASECRT_PRECOMP(u2PrimeLength));
	PUKCL_CRT(pfu1ExpBase)    = (pfu1)BASECRT_EXP(u2PrimeLength);
	PUKCL_CRT(u2ExpLength)    = u2PrimeLength;
	PUKCL_CRT(u1Blinding)     = 0x00;

	/* Copies the Primes again (they are scratched by the GCD),
	 * and the 4 padding bytes too !
	 */
	pukcc_memcopy((pu1)BASECRT_MODULUS(u2PrimeLength), pfu1P, (u2)(u2PrimeLength + 4));
	pukcc_memcopy((pu1)BASECRT_MODULUS(u2PrimeLength) + u2PrimeLength + 4, pfu1Q, (u2)(u2PrimeLength + 4));

	/* Adds a few zeros */
	*(pu4)(BASECRT_MODULUS(u2PrimeLength) + u2PrimeLength)         = 0L;
	*(pu4)(BASECRT_MODULUS(u2PrimeLength) + 2 * u2PrimeLength + 4) = 0L;

	/* Clear four words at the top of the Message Base */
	pu4Ptr      = (pu4)(BASECRT_MESSAGE(u2PrimeLength) + u2PrimeLength * 2);
	*(pu4Ptr++) = 0L;
	*(pu4Ptr++) = 0L;
	*(pu4Ptr++) = 0L;
	*(pu4Ptr++) = 0L;

	/* Clear one word at MSB of EP and EQ */
	*(pu4)(BASECRT_EXP_EP(u2PrimeLength) - 4) = 0L;
	*(pu4)(BASECRT_EXP_EQ(u2PrimeLength) - 4) = 0L;

	/* Gets the message if necessary */
	if (u1GetMessage) {
		pukcc_memcopy((pu1)BASECRT_MESSAGE(u2PrimeLength), pfu1Message, (u2)(u2PrimeLength * 2));
	}

	if (u2Mode == PRIVATE_EXPONENTIATION) {
		/* Launches the exponentiation in SECURE mode, with the exponent in PUKCC RAM */
		PUKCL(u2Option) = PUKCL_EXPMOD_WINDOWSIZE_1 | PUKCL_EXPMOD_REGULARRSA | PUKCL_EXPMOD_EXPINPUKCCRAM;
	} else {
		/* Launches the exponentiation in FAST mode, with the exponent in PUKCC RAM */
		PUKCL(u2Option) = PUKCL_EXPMOD_WINDOWSIZE_1 | PUKCL_EXPMOD_FASTRSA | PUKCL_EXPMOD_EXPINPUKCCRAM;
	}

	/* Modular exponentiation with CRT */
	vPUKCL_Process(CRT, pvPUKCLParam);

	return PUKCL(u2Status);
}

/**
 * \brief Computes modular exponent using CRT with blinding exponent
 *
 * Instead of use intermediate values :
 * EP = E mod (P - 1),
 * EQ = E mod (Q - 1),
 * This function uses
 * EP' = E mod (P - 1) + k*(P - 1),
 * EQ' = E mod (P - 1) + k'*(Q - 1).
 * where k and k' are 16 bits random numbers
 */
static u2 modular_exponentiation_with_crt_with_blinding(const byte* pfu1Message, const byte* pfu1Exponent, pfu1 pfu1P, pfu1 pfu1Q,
                                                        u2 u2ExponentLength, u1 u1GetMessage, u2 u2PrimeLength,
                                                        u2 u2Mode)
{
	pu4 pu4Ptr;
	u4  u4MulValue;

	/* Clear GCDA area used by the GCD service */
	PUKCL_Fill(nu1RBase)    = (nu1)(BASECRTB_GCDA(u2PrimeLength));
	PUKCL_Fill(u2RLength)   = (u2)(2 * u2PrimeLength + 8);
	PUKCL_Fill(u4FillValue) = 0L;
	vPUKCL_Process(Fill, pvPUKCLParam);

	/* Clear GCDZ area used by the GCD service */
	PUKCL_Fill(nu1RBase) = (nu1)(BASECRTB_GCDZ(u2PrimeLength));
	vPUKCL_Process(Fill, pvPUKCLParam);
	if (PUKCL(u2Status) != PUKCL_OK)
		return PUKCL(u2Status);

	/* Compute the R value */
	PUKCL_GCD(nu1XBase)     = (nu1)(BASECRTB_GCDX(u2PrimeLength));
	PUKCL_GCD(nu1YBase)     = (nu1)(BASECRTB_GCDY(u2PrimeLength));
	PUKCL_GCD(nu1ABase)     = (nu1)(BASECRTB_GCDA(u2PrimeLength));
	PUKCL_GCD(nu1ZBase)     = (nu1)(BASECRTB_GCDZ(u2PrimeLength));
	PUKCL_GCD(nu1WorkSpace) = (nu1)(BASECRTB_WORK(u2PrimeLength));
	PUKCL_GCD(u2Length)     = (u2)(u2PrimeLength + 4);

	/* Copies the Prime, and the 4 padding bytes too */
	pukcc_memcopy((pu1)BASECRTB_GCDX(u2PrimeLength), pfu1P, (u2)(u2PrimeLength + 4));
	pukcc_memcopy((pu1)BASECRTB_GCDY(u2PrimeLength), pfu1Q, (u2)(u2PrimeLength + 4));

	/* Launches the GCD */
	vPUKCL_Process(GCD, pvPUKCLParam);
	if (PUKCL(u2Status) != PUKCL_OK) {
		return PUKCL(u2Status);
	}

	/* Note that the R value is now stored at "BASECRTB_PRECOMP" address
	 * It will be used later in this code
	 * Now compute EQ' and EP'
	 * EP' = Exponent mod (P-1) + k*(P-1)
	 * EQ' = Exponent mod (Q-1) + k'*(Q-1)
	 */

	/* Copy the Primes again (they are scratched by the GCD)
	 * and the 4 padding bytes too
	 */
	pukcc_memcopy((pu1)BASECRTB_P(u2PrimeLength), pfu1P, (u2)(u2PrimeLength + 4));
	pukcc_memcopy((pu1)BASECRTB_Q(u2PrimeLength), pfu1Q, (u2)(u2PrimeLength + 4));

	/* Decrements the P and Q values for EP and EQ computation */
	*((pu1)BASECRTB_P(u2PrimeLength)) &= 0xfe;
	*((pu1)BASECRTB_Q(u2PrimeLength)) &= 0xfe;

	/* Then computes EP and EQ */
	memcpy((pu1)BASECRTB_DIVNUM(u2PrimeLength), pfu1Exponent + 4, u2ExponentLength);
	PUKCL_Div(nu1NumBase)  = (nu1)(BASECRTB_DIVNUM(u2PrimeLength));
	PUKCL_Div(u2NumLength) = u2ExponentLength;
	PUKCL_Div(nu1ModBase)  = (nu1)(BASECRTB_P(u2PrimeLength));
	PUKCL_Div(u2ModLength) = u2PrimeLength;
	/* Since quotient is not used, set the address as zero */
	PUKCL_Div(nu1QuoBase)   = 0;
	PUKCL_Div(nu1WorkSpace) = (nu1)(BASECRTB_DIVWORK(u2PrimeLength));
	PUKCL_Div(nu1RBase)     = (nu1)(BASECRTB_EXP_EP(u2PrimeLength));
	vPUKCL_Process(Div, pvPUKCLParam);

	/* First, get the (full) exponent */
	memcpy((pu1)BASECRTB_DIVNUM(u2PrimeLength), pfu1Exponent + 4, u2ExponentLength);
	PUKCL_Div(nu1ModBase) = (nu1)(BASECRTB_Q(u2PrimeLength));
	PUKCL_Div(nu1RBase)   = (nu1)(BASECRTB_EXP_EQ(u2PrimeLength));
	vPUKCL_Process(Div, pvPUKCLParam);

	if (PUKCL(u2Status) != PUKCL_OK) {
		return PUKCL(u2Status);
	}

	/* Blinding
	 * EP' = EP + k * (P-1)
	 * Generates a 16 bit random number k
	 */

	/* It is recommended to generate random number for u4MulValue
	 * This example just copies an existing number
	 */
	u4MulValue = *((pu2)&au1RandomTable[4]);

	/* Add zeros for padding */
	*(pu4)(BASECRTB_EXP_EP(u2PrimeLength) + u2PrimeLength) = 0L;

	/* Computes EP = EP + k * (P-1)
	 * Computes the blinded exponent
	 * This new exponent is stored in BASE_EXP_BLINDED
	 * Using the SMULT operation
	 * R = Z + X*MulValue
	 * X is Phi(N) at BASECRTB_P address
	 * Z is u2PrivateExponent at BASECRTB_EXP_EP address
	 * R is E' at BASECRTB_EXP_EP address
	 */
	PUKCL(Specific).Gf2n    = 0;
	PUKCL(Specific).CarryIn = 0;
	PUKCL(u2Option)         = SET_MULTIPLIEROPTION(PUKCL_SMULT_ADD) | SET_CARRYOPTION(CARRY_NONE);
	PUKCL_Smult(nu1XBase)   = (nu1)(BASECRTB_P(u2PrimeLength));
	PUKCL_Smult(u2XLength)  = u2PrimeLength;
	PUKCL_Smult(u4MulValue) = u4MulValue;
	PUKCL_Smult(u2RLength)  = (u2)(u2PrimeLength + 4);
	PUKCL_Smult(nu1RBase)   = (nu1)(BASECRTB_EXP_EP(u2PrimeLength));
	PUKCL_Smult(nu1ZBase)   = (nu1)(BASECRTB_EXP_EP(u2PrimeLength));

	/* Computes the Smult */
	vPUKCL_Process(Smult, pvPUKCLParam);
	if (PUKCL(u2Status) != PUKCL_OK) {
		return PUKCL(u2Status);
	}

	/* Blinding
	 * EQ' = EQ + k' * (P-1)
	 * Generates a 16 bit random number k'
	 */

	/* Gets the random (it is preferable to use a true random number) */
	u4MulValue = *((pu2)&au1RandomTable[8]);

	/* Add a few zeros for padding */
	*(pu4)(BASECRTB_EXP_EQ(u2PrimeLength) + u2PrimeLength) = 0L;

	/* Computes EQ = EQ + k' * (Q-1)
	 * Computes the blinded exponent
	 * This new exponent is stored in BASE_EXP_BLINDED
	 * Using ths SMULT operation
	 * R = Z + X*MulValue
	 * X is Phi(N) at BASECRTB_Q address
	 * Z is u2PrivateExponent at BASECRTB_EXP_EQ address
	 * R is E' at BASECRTB_EXP_EQ address
	 */
	PUKCL(Specific).Gf2n    = 0;
	PUKCL(Specific).CarryIn = 0;
	PUKCL(u2Option)         = SET_MULTIPLIEROPTION(PUKCL_SMULT_ADD) | SET_CARRYOPTION(CARRY_NONE);
	PUKCL_Smult(nu1XBase)   = (nu1)(BASECRTB_Q(u2PrimeLength));
	PUKCL_Smult(u2XLength)  = u2PrimeLength;
	PUKCL_Smult(u4MulValue) = u4MulValue;
	PUKCL_Smult(u2RLength)  = (u2)(u2PrimeLength + 4);
	PUKCL(u2Option)         = SET_MULTIPLIEROPTION(PUKCL_SMULT_ADD) | SET_CARRYOPTION(CARRY_NONE);
	PUKCL_Smult(u4MulValue) = u4MulValue;
	PUKCL_Smult(nu1RBase)   = (nu1)(BASECRTB_EXP_EQ(u2PrimeLength));
	PUKCL_Smult(nu1ZBase)   = (nu1)(BASECRTB_EXP_EQ(u2PrimeLength));

	/* Computes the Smult */
	vPUKCL_Process(Smult, pvPUKCLParam);
	if (PUKCL(u2Status) != PUKCL_OK) {
		return PUKCL(u2Status);
	}

	/* Adds a few zeros for padding */
	*(pu4)(BASECRTB_EXP_EQ(u2PrimeLength) - 4) = 0L;
	*(pu4)(BASECRTB_EXP_EP(u2PrimeLength) - 4) = 0L;
	/* Note that EP' and EQ' values are now stored at "BASECRTB_EXP" address */

	/* Prepares the fields of the mapping for CRT */
	PUKCL_CRT(nu1ModBase)     = (nu1)(BASECRTB_MODULUS(u2PrimeLength));
	PUKCL_CRT(u2ModLength)    = u2PrimeLength;
	PUKCL_CRT(nu1XBase)       = (nu1)(BASECRTB_MESSAGE(u2PrimeLength));
	PUKCL_CRT(nu1PrecompBase) = (nu1)(BASECRTB_PRECOMP(u2PrimeLength));
	PUKCL_CRT(pfu1ExpBase)    = (pfu1)BASECRTB_EXP(u2PrimeLength);
	PUKCL_CRT(u2ExpLength)    = (u2)(u2PrimeLength + 4);
	PUKCL_CRT(u1Blinding)     = 0x00;

	/* Copies the Primes again (they are scratched by the GCD),
	 * and the 4 padding bytes too
	 */
	pukcc_memcopy((pu1)BASECRTB_MODULUS(u2PrimeLength), pfu1P, (u2)(u2PrimeLength + 4));
	pukcc_memcopy((pu1)BASECRTB_MODULUS(u2PrimeLength) + u2PrimeLength + 4, pfu1Q, (u2)(u2PrimeLength + 4));

	/* Add a few zeros */
	*(pu4)(BASECRTB_MODULUS(u2PrimeLength) + u2PrimeLength)         = 0L;
	*(pu4)(BASECRTB_MODULUS(u2PrimeLength) + 2 * u2PrimeLength + 4) = 0L;

	/* Clear four words at the top of the Message Base */
	pu4Ptr      = (pu4)(BASECRTB_MESSAGE(u2PrimeLength) + u2PrimeLength * 2);
	*(pu4Ptr++) = 0L;
	*(pu4Ptr++) = 0L;
	*(pu4Ptr++) = 0L;
	*(pu4Ptr++) = 0L;

	/* Insert zeros at the top of the Message Base */
	*(pu4)(BASECRTB_EXP_EP(u2PrimeLength) - 4) = 0L;
	*(pu4)(BASECRTB_EXP_EQ(u2PrimeLength) - 4) = 0L;

	/* Gets the message if necessary */
	if (u1GetMessage) {
		pukcc_memcopy((pu1)BASECRTB_MESSAGE(u2PrimeLength), pfu1Message, (u2)(u2PrimeLength * 2));
	}

	if (u2Mode == PRIVATE_EXPONENTIATION) {
		/* Launches the exponentiation in SECURE mode, with the exponent in PUKCC RAM */
		PUKCL(u2Option) = PUKCL_EXPMOD_WINDOWSIZE_1 | PUKCL_EXPMOD_REGULARRSA | PUKCL_EXPMOD_EXPINPUKCCRAM;
	} else {
		/* Launches the exponentiation in FAST mode, with the exponent in PUKCC RAM */
		PUKCL(u2Option) = PUKCL_EXPMOD_WINDOWSIZE_1 | PUKCL_EXPMOD_FASTRSA | PUKCL_EXPMOD_EXPINPUKCCRAM;
	}

	/* Modular exponentiation with CRT */
	vPUKCL_Process(CRT, pvPUKCLParam);

	return PUKCL(u2Status);
}

/**
 * \brief Initializes parameters for RSA operation
 *
 * Initializes parameters for RSA operation
 */
u2 rsa_params_init(u1 u1Mode, pfu1 pfu1Modulus, u2 u2ModulusLength, pfu1 pfu1PublicExponent, u2 u2PublicExponentLength,
                   pfu1 pfu1PrivateExponent, u2 u2PrivateExponentLength, pfu1 pfu1PrimeP, pfu1 pfu1PrimeQ)
{
	u2 u2Result;
    pParametersRSA = &parametersRSA;
	pParametersRSA->u2FieldIsPresent = 0;

	/* Common parameters for CRT and without CRT mode */

	/* Initializes the modulus length */
	if (u2ModulusLength == 0) {
		return ERR_MODULUS_LENGTH;
	}

	pParametersRSA->u2ModulusLength = u2ModulusLength;

	/* Initializes the public exponent and its length */
	if (u1Mode & PUBLIC_EXPONENT_IS_PRESENT) {
		/* Testing the length of the public exponent */
		if (u2PublicExponentLength == 0) {
			return ERR_PUB_EXP_LENGTH;
		}
		pParametersRSA->u2PublicExponentLength = u2PublicExponentLength;
		/* Testing the public Exponent pointer */
		if (pfu1PublicExponent == 0) {
			return ERR_PUB_EXP;
		}
		/* Pointing the pfu1PublicExponent address */
		pParametersRSA->pfu1PublicExponent = pfu1PublicExponent;
		/* Mask updating */
		pParametersRSA->u2FieldIsPresent |= PUBLIC_EXPONENT_IS_PRESENT;
	}

	/* Initializes the private exponent */
	if (u1Mode & PRIVATE_EXPONENT_IS_PRESENT) {
		/* Testing the length of the private exponent */
		if (u2PrivateExponentLength == 0) {
			return ERR_PRI_EXP_LENGTH;
		}
		pParametersRSA->u2PrivateExponentLength = u2PrivateExponentLength;

		/* Testing the private exponent pointer */
		if (pfu1PrivateExponent == 0) {
			return ERR_PRI_EXP;
		}

		/* Pointing the pfu1PrivateExponent address */
		pParametersRSA->pfu1PrivateExponent = pfu1PrivateExponent;

		/* Mask updating */
		pParametersRSA->u2FieldIsPresent |= PRIVATE_EXPONENT_IS_PRESENT;
	}

	/* Parameters for CRT mode */

	/* Initializes prime P */
	if (u1Mode & PRIME_P_IS_PRESENT) {
		/* Testing the prime factor P */
		if (pfu1PrimeP == 0) {
			return ERR_PRIME_P;
		}
		/* Pointing the pfu1PrimeP address */
		pParametersRSA->pfu1PrimeP = pfu1PrimeP;
		/* Mask updating */
		pParametersRSA->u2FieldIsPresent |= PRIME_P_IS_PRESENT;
	}

	/*  Initializes prime Q*/
	if (u1Mode & PRIME_Q_IS_PRESENT) {
		/* Testing the prime factor Q */
		if (pfu1PrimeQ == 0) {
			return ERR_PRIME_Q;
		}
		/* Pointing the address of time number */
		pParametersRSA->pfu1PrimeQ = pfu1PrimeQ;
		/* Mask updating */
		pParametersRSA->u2FieldIsPresent |= PRIME_Q_IS_PRESENT;
	}

	/* Parameters for "without CRT" mode */
	/* Initializes the modulus */
	if (u1Mode & MODULUS_IS_PRESENT) {
		/* Testing the modulus */
		if (pfu1Modulus == 0) {
			return ERR_MODULUS;
		}
		/* Pointing the pfu1Modulus address */
		pParametersRSA->pfu1Modulus = pfu1Modulus;
		/* Mask updating */
		pParametersRSA->u2FieldIsPresent |= MODULUS_IS_PRESENT;
	}

	/* Computes PHI(N) */
	if ((pParametersRSA->u2FieldIsPresent & PRIME_Q_IS_PRESENT)
	    && (pParametersRSA->u2FieldIsPresent & PRIME_P_IS_PRESENT)) {
		/* Memory copy of P and Q */
		pukcc_memcopy((pu1)KEYGEN_BASE_P(u2ModulusLength / 2), pfu1PrimeP, (u2)(u2ModulusLength / 2 + 4));
		pukcc_memcopy((pu1)KEYGEN_BASE_PRECOMP(u2ModulusLength / 2), pfu1PrimeQ, (u2)(u2ModulusLength / 2 + 4));

		/* Computes */
		u2Result = compute_phi_n((u2)(u2ModulusLength / 2));
		if (u2Result) {
			return ERR_PHI_N_GENERATION;
		}

		memset((void *)&au1KeySpace[0], 0, sizeof(au1KeySpace));
		memcpy((pu1)&au1KeySpace[4], (pfu1)KEYGEN_BASE_YGCD(u2ModulusLength / 2), u2ModulusLength);

		/* RAM address */
		pParametersRSA->pfu1PhiN = (pfu1)&au1KeySpace[0];
		/* Mask updating */
		pParametersRSA->u2FieldIsPresent |= PHI_N_IS_PRESENT;
		/* Erasing PHI(N) in RAM PUKCC */
		memset((pu1)KEYGEN_BASE_YGCD(u2ModulusLength / 2), 0, u2ModulusLength);
	}

	return 0;
}

/**
 * \brief Performs the RSA exponentiation
 *
 * Performs the RSA exponentiation with or without (default) CRT mode
 * ulMode describes the mode of exponentiation that shall be used.
 * It is a combination of macros that indicates which fields have to be initialized
 * and which mode of exponentiation is used
 */
u2 rsa_exponentiation(u1 u1Mode, const byte* pfu1Message)
{
	u2 result;

	/* Validate the parameters */
	if ((u1Mode & MODE_NO_CRT) && (u1Mode & MODE_CRT)) {
		return ERR_MODE;
	}
	if ((u1Mode & PUBLIC_EXPONENTIATION) && (u1Mode & PRIVATE_EXPONENTIATION)) {
		return ERR_MODE;
	}
	if (!((u1Mode & PUBLIC_EXPONENTIATION) || (u1Mode & PRIVATE_EXPONENTIATION))) {
		return ERR_MODE;
	}
	if (((u1Mode & PUBLIC_EXPONENTIATION) && (u1Mode & EXPONENT_BLINDING))) {
		return ERR_MODE;
	}
	if (((u1Mode & PUBLIC_EXPONENTIATION) && (u1Mode & MODE_CRT))) {
		return ERR_MODE;
	}

	/*
	 * RSA without CRT in two modes :
	 *   - public exponentiation
	 *   - private exponentiation
	 */
	if (!(u1Mode & MODE_CRT)) {
		/* Testing if Modulus is present */
		if (!(pParametersRSA->u2FieldIsPresent & MODULUS_IS_PRESENT)) {
			return ERR_MODULUS;
		}

		/* Computes CNS Constant and maps the memory in PUKCC RAM */
		result = modular_operations_setup(pParametersRSA->pfu1Modulus, pParametersRSA->u2ModulusLength);
		if (result != PUKCL_OK) {
			return result;
		}

		/*
		 * Computes the RSA exponentiation of the Message
		 * ** It's a public exponentiation if :
		 *    - u1Mode is set to PUBLIC_EXPONENTIATION
		 *    - pfu1PublicExponent is initialized
		 * ** It's a private exponentiation if :
		 *    - u1Mode is set to PRIVATE_EXPONENTIATION
		 *    - pfu1PrivateExponent is initialised
		 */
		if (u1Mode & PUBLIC_EXPONENTIATION) {
			/* Testing if public Exponent is OK */
			if (!(pParametersRSA->u2FieldIsPresent & PUBLIC_EXPONENT_IS_PRESENT)) {
				return ERR_PUB_EXP;
			}

			/* Public Exponentiation */
			result = modular_exponentiation_without_crt(pfu1Message,
			                                            1,
			                                            pParametersRSA->pfu1PublicExponent,
			                                            pParametersRSA->u2PublicExponentLength,
			                                            PUBLIC_EXPONENTIATION,
			                                            0);

			if (result != PUKCL_OK) {
				return result;
			}
		}
		/* Private exponentiation */
		else {
			/* Testing if private Exponent is OK */
			if (!(pParametersRSA->u2FieldIsPresent & PRIVATE_EXPONENT_IS_PRESENT)) {
				return ERR_PRI_EXP;
			}

			if (u1Mode & EXPONENT_BLINDING) {
				/* Testing if PHI(N) is present */
				if (!(pParametersRSA->u2FieldIsPresent & PHI_N_IS_PRESENT)) {
					return ERR_PHI_N;
				}

				/* Private exponentiation with blinding */
				result = modular_exponentiation_without_crt(pfu1Message,
				                                            1,
				                                            pParametersRSA->pfu1PrivateExponent,
				                                            pParametersRSA->u2PrivateExponentLength,
				                                            PRIVATE_EXPONENTIATION | EXPONENT_BLINDING,
				                                            pParametersRSA->pfu1PhiN);
			} else {
				/* Private exponentiation */
				result = modular_exponentiation_without_crt(pfu1Message,
				                                            1,
				                                            pParametersRSA->pfu1PrivateExponent,
				                                            pParametersRSA->u2PrivateExponentLength,
				                                            PRIVATE_EXPONENTIATION,
				                                            0);
			}

			if (result != PUKCL_OK) {
				return result;
			}
		}
	}

	/*
	 * RSA with CRT can only be used for private exponentiation
	 */
	else {
		/* Testing if Prime P is present */
		if (!(pParametersRSA->u2FieldIsPresent & PRIME_P_IS_PRESENT)) {
			return ERR_PRIME_P;
		}

		/* Testing if Prime Q is present */
		if (!(pParametersRSA->u2FieldIsPresent & PRIME_Q_IS_PRESENT)) {
			return ERR_PRIME_Q;
		}

		/* Computes the RSA exponentiation of the Message*/

		/* Testing if private Exponent is OK */
		if (!(pParametersRSA->u2FieldIsPresent & PRIVATE_EXPONENT_IS_PRESENT)) {
			return ERR_PRI_EXP;
		}

		if (u1Mode & EXPONENT_BLINDING) {
			/* Private exponentiation with CRT and blinding */
			result = modular_exponentiation_with_crt_with_blinding(pfu1Message,
			                                                       pParametersRSA->pfu1PrivateExponent,
			                                                       pParametersRSA->pfu1PrimeP,
			                                                       pParametersRSA->pfu1PrimeQ,
			                                                       pParametersRSA->u2PrivateExponentLength,
			                                                       1,
			                                                       (u2)(pParametersRSA->u2ModulusLength / 2),
			                                                       PRIVATE_EXPONENTIATION);
		} else {
			/* Private exponentiation with CRT */
			result = modular_exponentiation_with_crt(pfu1Message,
			                                         pParametersRSA->pfu1PrivateExponent,
			                                         pParametersRSA->pfu1PrimeP,
			                                         pParametersRSA->pfu1PrimeQ,
			                                         pParametersRSA->u2PrivateExponentLength,
			                                         1,
			                                         (u2)(pParametersRSA->u2ModulusLength / 2),
			                                         PRIVATE_EXPONENTIATION);
		}

		if (result != PUKCL_OK) {
			return result;
		}
	}
	return 0;
}



int Crypt_RSA_HandleReq(int devId, wc_CryptoInfo* info, void* ctx)
{
    if (crypt_pukcl_SelfTest() < 0)
    {
        return CRYPTOCB_UNAVAILABLE;
    }
    if (info->algo_type != WC_ALGO_TYPE_PK || info->pk.type != WC_PK_TYPE_RSA)
    {
        return CRYPTOCB_UNAVAILABLE;
    }
    switch (info->pk.rsa.type)
    {
        case RSA_PUBLIC_ENCRYPT:
            return Crypt_RSA_PublicEncrypt(devId, info, ctx);
        case RSA_PUBLIC_DECRYPT:
            return Crypt_RSA_PublicDecrypt(devId, info, ctx);
        case RSA_PRIVATE_ENCRYPT:
            return Crypt_RSA_PrivateEncrypt(devId, info, ctx);
        case RSA_PRIVATE_DECRYPT:
            return Crypt_RSA_PrivateDecrypt(devId, info, ctx);
        default:
            return CRYPTOCB_UNAVAILABLE;
    }
    
}

uint8_t exponent[(FP_SIZE<<2) + 4];
uint8_t modulus[(FP_SIZE<<2) + 4];


int Crypt_RSA_PublicEncrypt(int devId, wc_CryptoInfo* info, void* ctx)
{
    u2 u2Result;
    memset(exponent, 0, sizeof(exponent));
    memset(modulus, 0, sizeof(modulus));
    memcpy(exponent+4, info->pk.rsa.key->e.dp, info->pk.rsa.key->e.used*4);
    pukcc_memcopy(modulus+4, (pu1)(&info->pk.rsa.key->n.dp), info->pk.rsa.key->n.used*4);
    
    WC_RNG rng;
    wc_InitRng(&rng);
    wc_RNG_GenerateBlock(&rng, au1RandomTable, sizeof(au1RandomTable));
    
    u2Result = rsa_params_init(MODULUS_IS_PRESENT | PUBLIC_EXPONENT_IS_PRESENT,
                           (pfu1)modulus,
                           info->pk.rsa.key->n.used *4,
                           (pfu1)exponent,
                           info->pk.rsa.key->e.used *4,
                           0,
                           0,
                           0,
                           0);
    
    if (u2Result == 0)
    {
        u2Result = rsa_exponentiation(PUBLIC_EXPONENTIATION | MODE_NO_CRT, info->pk.rsa.in);
        pukcc_memcopy(info->pk.rsa.out, (pu1)BASE_MESSAGE(PUKCL_ExpMod(u2ModLength)), *(info->pk.rsa.outLen));
        return 0;
    }
                
    return CRYPTOCB_UNAVAILABLE;
}
int Crypt_RSA_PublicDecrypt(int devId, wc_CryptoInfo* info, void* ctx)
{
    return Crypt_RSA_PublicEncrypt(devId, info, ctx);
}

int Crypt_RSA_PrivateEncrypt(int devId, wc_CryptoInfo* info, void* ctx)
{
    u2 u2Result;
    memset(exponent, 0, sizeof(exponent));
    memset(modulus, 0, sizeof(modulus));
    memcpy(exponent+4, info->pk.rsa.key->d.dp, info->pk.rsa.key->d.used*4);
    pukcc_memcopy(modulus+4, (pu1)(&info->pk.rsa.key->n.dp), info->pk.rsa.key->n.used*4);
    
    wc_RNG_GenerateBlock(info->rng.rng, au1RandomTable, sizeof(au1RandomTable));
    
    u2Result = rsa_params_init(MODULUS_IS_PRESENT | PRIVATE_EXPONENT_IS_PRESENT,
                           (pfu1)modulus,
                           info->pk.rsa.key->n.used *4,
                           0,
                           0,
                           (pfu1)exponent,
                           info->pk.rsa.key->d.used *4,
                           0,
                           0);
    
    if (u2Result == 0)
    {
        u2Result = rsa_exponentiation(PRIVATE_EXPONENTIATION | MODE_NO_CRT, info->pk.rsa.in);
        pukcc_memcopy(info->pk.rsa.out, (pu1)BASE_MESSAGE(PUKCL_ExpMod(u2ModLength)), *(info->pk.rsa.outLen));
        return 0;
    }
    return CRYPTOCB_UNAVAILABLE;
}
int Crypt_RSA_PrivateDecrypt(int devId, wc_CryptoInfo* info, void* ctx)
{
    return Crypt_RSA_PrivateEncrypt(devId, info, ctx);
}

#endif