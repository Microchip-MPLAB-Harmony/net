/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypt_sha_sam11105.h

  Summary:
    Crypto Framework Libarary interface file for hardware Cryptography

  Description:
    This file is used to interfacing with the ICM hardware for conducting 
    SHA hashing.
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
#ifndef _CRYPT_SHA_SAM11105_H_
#define _CRYPT_SHA_SAM11105_H_

#include "definitions.h"

/**
 *  ICM region descriptor in main list.
 */
struct icm_descriptor {
	/** The first byte address of the region */
	uint32_t start_addr;
	/** The configuration value of the region */
	union rcfg {
		struct {
			/** Compare Digest(true) or Write Back Digest(false) */
			uint32_t is_compare_mode:1;
			/**
			 * The next region descriptor address loaded is the current region
			 * identifier descriptor address incremented by 0x10(false) or is ICM_DSCR(true).
			 */
			uint32_t is_wrap:1;
			/** The current descriptor terminates the monitoring(true) or not(false) */
			uint32_t is_end_mon:1;
			uint32_t reserved1:1;
			/** Enable(true) or Disable(false) Region Hash Completed Interrupt */
			uint32_t reg_hash_int_en:1;
			/** Enable(true) or Disable(false) Digest Mismatch Interrupt */
			uint32_t dig_mis_int_en:1;
			/** Enable(true) or Disable(false) Bus Error Interrupt */
			uint32_t bus_err_int_en:1;
			/** Enable(true) or Disable(false) Wrap Condition Interrupt */
			uint32_t wrap_con_int_en:1;
			/** Enable(true) or Disable(false) End Bit Condition Interrupt */
			uint32_t ebit_con_int_en:1;
			/** Enable(true) or Disable(false) Monitoring Status Updated Condition Interrupt */
			uint32_t status_upt_con_int_en:1;
			/** SHA processing runtime is the longest(true) one or shortest(false) one */
			uint32_t is_pro_dly:1;
			uint32_t reserved2:1;
			/** SHA Algorithm */
			uint32_t algo:3;
			uint32_t reserved3:9;
			/** The value of HPROT AHB signal when the ICM retrieves the memory region */
			uint32_t mem_reg_val:6;
			uint32_t reserved4:2;
		} bit;
		uint32_t reg;
	} cfg;
    /**
     * The number of blocks(512 bits) transferred from the memory to the SHA engine.
     * Transfer size = (tran_size + 1) * 512bits
     * The maximum size is 65536.
     */
    uint32_t tran_size;
    /** The address of next region descriptor in secondary list */
    uint32_t *next_addr;
} ;



//static struct icm_descriptor icm_descriptor __attribute__((aligned (64)));

#endif

