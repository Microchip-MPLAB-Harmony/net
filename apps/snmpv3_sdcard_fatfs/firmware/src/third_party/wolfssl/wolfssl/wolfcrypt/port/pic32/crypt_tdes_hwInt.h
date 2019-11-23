/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_tdes_hw.h

  Summary:
    Crypto Framework Libarary interface file for hardware RNG

  Description:
    This file contains the interface that is required to be implemented by
    the TDES hardware driver.
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

#ifndef _CRYPT_TDES_HW_INT_H_
#define _CRYPT_TDES_HW_INT_H_

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include "configuration.h"
#include "wolfssl/wolfcrypt/des3.h"


/*----------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/
int  CRYPT_DES_SetKey(Des* des, const byte* key, const byte* iv, int dir);
void CRYPT_DES_SetIV(Des* des, const byte* iv);
int  CRYPT_DES_CbcEncrypt(Des* des, byte* out, const byte* in, word32 sz);
int  CRYPT_DES_CbcDecrypt(Des* des, byte* out, const byte* in, word32 sz);
int  CRYPT_DES_EcbEncrypt(Des* des, byte* out, const byte* in, word32 sz);
int  CRYPT_DES3_EcbEncrypt(Des3* des, byte* out, const byte* in, word32 sz);

int  CRYPT_DES3_SetKey(Des3* des, const byte* key, const byte* iv, int dir);
int  CRYPT_DES3_SetIV(Des3* des, const byte* iv);
int  CRYPT_DES3_CbcEncrypt(Des3* des, byte* out, const byte* in, word32 sz);
int  CRYPT_DES3_CbcDecrypt(Des3* des, byte* out, const byte* in, word32 sz);
int  CRYPT_DES3_Init(Des3*, void*, int);
void CRYPT_DES3_Free(Des3*);


#endif /* _CRYPT_TDES_HW_H_ */
