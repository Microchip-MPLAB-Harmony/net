/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    fe_operations.h
  
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



#ifndef WOLF_CRYPT_FE_OPERATIONS_H
#define WOLF_CRYPT_FE_OPERATIONS_H

#include "crypto/src/settings.h"

#if defined(HAVE_CURVE25519) || defined(HAVE_ED25519)

#if !defined(CURVE25519_SMALL) || !defined(ED25519_SMALL)
    #include <stdint.h>
#endif

#include "crypto/src/types."

#if defined(HAVE___UINT128_T) && !defined(NO_CURVED25519_128BIT)
    #define CURVED25519_128BIT
#endif

/*
fe means field element.
Here the field is \Z/(2^255-19).
An element t, entries t[0]...t[9], represents the integer
t[0]+2^26 t[1]+2^51 t[2]+2^77 t[3]+2^102 t[4]+...+2^230 t[9].
Bounds on each t[i] vary depending on context.
*/

#if defined(CURVE25519_SMALL) || defined(ED25519_SMALL)
    #define F25519_SIZE 32

    WOLFSSL_LOCAL void lm_copy(byte*, const byte*);
    WOLFSSL_LOCAL void lm_add(byte*, const byte*, const byte*);
    WOLFSSL_LOCAL void lm_sub(byte*, const byte*, const byte*);
    WOLFSSL_LOCAL void lm_neg(byte*,const byte*);
    WOLFSSL_LOCAL void lm_invert(byte*, const byte*);
    WOLFSSL_LOCAL void lm_mul(byte*,const byte*,const byte*);
#endif


#if !defined(FREESCALE_LTC_ECC)
WOLFSSL_LOCAL int  curve25519(byte * q, byte * n, byte * p);
#endif

/* default to be faster but take more memory */
#if !defined(CURVE25519_SMALL) || !defined(ED25519_SMALL)

#if defined(CURVED25519_128BIT)
    typedef int64_t  fe[5];
#else
    typedef int32_t  fe[10];
#endif

WOLFSSL_LOCAL void fe_copy(fe, const fe);
WOLFSSL_LOCAL void fe_add(fe, const fe, const fe);
WOLFSSL_LOCAL void fe_neg(fe,const fe);
WOLFSSL_LOCAL void fe_sub(fe, const fe, const fe);
WOLFSSL_LOCAL void fe_invert(fe, const fe);
WOLFSSL_LOCAL void fe_mul(fe,const fe,const fe);


/* Based On Daniel J Bernstein's curve25519 and ed25519 Public Domain ref10
   work. */

WOLFSSL_LOCAL void fe_0(fe);
WOLFSSL_LOCAL void fe_1(fe);
WOLFSSL_LOCAL int  fe_isnonzero(const fe);
WOLFSSL_LOCAL int  fe_isnegative(const fe);
WOLFSSL_LOCAL void fe_tobytes(unsigned char *, const fe);
WOLFSSL_LOCAL void fe_sq(fe, const fe);
WOLFSSL_LOCAL void fe_sq2(fe,const fe);
WOLFSSL_LOCAL void fe_frombytes(fe,const unsigned char *);
WOLFSSL_LOCAL void fe_cswap(fe, fe, int);
WOLFSSL_LOCAL void fe_mul121666(fe,fe);
WOLFSSL_LOCAL void fe_cmov(fe,const fe, int);
WOLFSSL_LOCAL void fe_pow22523(fe,const fe);

/* 64 type needed for SHA512 */
WOLFSSL_LOCAL uint64_t load_3(const unsigned char *in);
WOLFSSL_LOCAL uint64_t load_4(const unsigned char *in);

#endif /* !CURVE25519_SMALL || !ED25519_SMALL */

/* Use less memory and only 32bit types or less, but is slower
   Based on Daniel Beer's public domain work. */
#if defined(CURVE25519_SMALL) || defined(ED25519_SMALL)
static const byte c25519_base_x[F25519_SIZE] = {9};
static const byte f25519_zero[F25519_SIZE]   = {0};
static const byte f25519_one[F25519_SIZE]    = {1};
static const byte fprime_zero[F25519_SIZE]   = {0};
static const byte fprime_one[F25519_SIZE]    = {1};

WOLFSSL_LOCAL void fe_load(byte *x, word32 c);
WOLFSSL_LOCAL void fe_normalize(byte *x);
WOLFSSL_LOCAL void fe_inv__distinct(byte *r, const byte *x);

/* Conditional copy. If condition == 0, then zero is copied to dst. If
 * condition == 1, then one is copied to dst. Any other value results in
 * undefined behavior.
 */
WOLFSSL_LOCAL void fe_select(byte *dst, const byte *zero, const byte *one,
		   byte condition);

/* Multiply a point by a small constant. The two pointers are not
 * required to be distinct.
 *
 * The constant must be less than 2^24.
 */
WOLFSSL_LOCAL void fe_mul_c(byte *r, const byte *a, word32 b);
WOLFSSL_LOCAL void fe_mul__distinct(byte *r, const byte *a, const byte *b);

/* Compute one of the square roots of the field element, if the element
 * is square. The other square is -r.
 *
 * If the input is not square, the returned value is a valid field
 * element, but not the correct answer. If you don't already know that
 * your element is square, you should square the return value and test.
 */
WOLFSSL_LOCAL void fe_sqrt(byte *r, const byte *x);

/* Conditional copy. If condition == 0, then zero is copied to dst. If
 * condition == 1, then one is copied to dst. Any other value results in
 * undefined behavior.
 */
WOLFSSL_LOCAL void fprime_select(byte *dst, const byte *zero, const byte *one,
		                         byte condition);
WOLFSSL_LOCAL void fprime_add(byte *r, const byte *a, const byte *modulus);
WOLFSSL_LOCAL void fprime_sub(byte *r, const byte *a, const byte *modulus);
WOLFSSL_LOCAL void fprime_mul(byte *r, const byte *a, const byte *b,
		                      const byte *modulus);
WOLFSSL_LOCAL void fprime_copy(byte *x, const byte *a);

#endif /* CURVE25519_SMALL || ED25519_SMALL */
#endif /* HAVE_CURVE25519 || HAVE_ED25519 */

#endif /* WOLF_CRYPT_FE_OPERATIONS_H */
