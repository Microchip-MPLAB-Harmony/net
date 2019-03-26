/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_sha_sam6156.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for SAMA5D2 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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

#ifndef CRYPT_SHA_SAM6156_H_
#define CRYPT_SHA_SAM6156_H_


/*------------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include "packs/ATSAMA5D27_DFP/atsama5d27.h"

/* block and digest bytes per algorithm */
#define CRYPT_SHA1_BLOCK_BYTES     64   /* 512 bits */
#define CRYPT_SHA1_DIGEST_BYTES    20   /* 160 bits */

#define CRYPT_SHA224_BLOCK_BYTES   64   /* 512 bits */
#define CRYPT_SHA224_DIGEST_BYTES  28   /* 224 bits */

#define CRYPT_SHA256_BLOCK_BYTES   64   /* 512 bits */
#define CRYPT_SHA256_DIGEST_BYTES  32   /* 256 bits */

#define CRYPT_SHA384_BLOCK_BYTES   128  /* 1024 bits */
#define CRYPT_SHA384_DIGEST_BYTES   48  /* 384 bits */

#define CRYPT_SHA512_BLOCK_BYTES   128  /* 1024 bits */
#define CRYPT_SHA512_DIGEST_BYTES   64  /* 512 bits */

/* SHA_MR Start Mode bits 1..0 */
enum sha_start_mode {
    SHA_MANUAL_START = 0, /* To begin processing, must set SHA_CR.START */
    SHA_AUTO_START,       /* Processing starts as soon as correct number of 
                             SHA_IDATARx is written. No action in the SHA_CR 
                             is necessary. */
    SHA_IDATAR0_START,    /* access only mode (mandatory when DMA is used) */
};

/* SHA_MR processing delay bit 4 */
enum sha_proc_delay {
    SHA_SHORTEST_RUNTIME = 0, /* runtime period is 72 clock cycles*/
    SHA_LONGEST_RUNTIME       /* runtime period is 194 clock cycles
                                 (reduces SHA bandwidth requirement and
                                  reduces system bus overload) */
};

/* SHA_MR user initial hash values bit 5 */
enum sha_UIHV {
    SHA_STD_UIHV = 0, /* start algo with standard initial values per FIPS180-2*/
    SHA_USER_UIHV     /* start algo with user initial hash values stored in IR1*/
};

/* SHA_MR User Initial or Expected Hash Value Registers bit 6 */
enum sha_UIEHV {
    SHA_STD_UIEHV = 0, /* use standard initial values per FIPS180-2*/
    SHA_USER_UIEHV     /* use user initial hash values stored in IR0 */
};

/* SHA_MR Algorithm bits 11..8 */
enum sha_algo {
    SHA_1_ALGO = 0, 
    SHA_256_ALGO,
    SHA_384_ALGO,
    SHA_512_ALGO,
    SHA_224_ALGO,
    HMAC_SHA_1_ALGO,
    HMAC_SHA_256_ALGO,
    HMAC_SHA_384_ALGO,
    HMAC_SHA_512_ALGO,    
    HMAC_SHA_224_ALGO     
};

/* SHA_MR Dual Input Buffer bit 16 */
enum sha_dualbuff {
    SHA_DUALBUFF_INACTIVE = 0, /* SHA_IDATARx and SHA_IODATARx cannot be written
                                  during processing of previous block*/
    SHA_DUALBUFF_ACTIVE        /* SHA_IDATARx and SHA_IODATARx can be written 
                                  during processing of previous block when 
                                  SMOD value = 2*/
};

/* SHA_MR Hash Check bits 25..24 */
enum sha_hashcheck {
    SHA_HASHCHECK_NONE = 0, /* No check */
    SHA_HASHCHECK_EHV,      /* Check is performed with expected hash stored in 
                               internal expected hash value registers */
    SHA_HASHCHECK_MSG       /* Check is performed with expected hash provided 
                               after the message */
};

/* SHA configuration */
struct sha_descriptor
{
	/** SHA start mode */
	enum sha_start_mode start_mode;

	/** SHA processing delay */
	enum sha_proc_delay proc_delay;

	/** SHA user initial hash values */
	enum sha_UIHV UIHV;

	/** User Initial or Expected Hash Value Registers */
	enum sha_UIEHV UIEHV;

	/** Algorithm */
	enum sha_algo algo;

	/** Dual Input Buffer */
	enum sha_dualbuff dualbuff;

	/** Hash Check */
	enum sha_hashcheck hashcheck;

	/** Check count */
	uint16_t checkcount;
};


#endif /* CRYPT_SHA_SAM6156_H_ */
