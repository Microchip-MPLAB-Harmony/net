/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    fips_test.h
  
  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright © 2013-2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END



#ifndef WOLF_CRYPT_FIPS_TEST_H
#define WOLF_CRYPT_FIPS_TEST_H

#include "crypto/src/types.h"


#ifdef __cplusplus
    extern "C" {
#endif

/* Known Answer Test string inputs are hex, internal */
CYASSL_LOCAL int DoKnownAnswerTests(char*, int);


/* FIPS failure callback */
typedef void(*wolfCrypt_fips_cb)(int ok, int err, const char* hash);

/* Public set function */
CYASSL_API int wolfCrypt_SetCb_fips(wolfCrypt_fips_cb cbf);

/* Public get status functions */
CYASSL_API int wolfCrypt_GetStatus_fips(void);
CYASSL_API const char* wolfCrypt_GetCoreHash_fips(void);

#ifdef HAVE_FORCE_FIPS_FAILURE
    /* Public function to force failure mode for operational testing */
    CYASSL_API int wolfCrypt_SetStatus_fips(int);
#endif


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_FIPS_TEST_H */

