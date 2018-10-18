/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    ge_operations.h
  
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


 /* Based On Daniel J Bernstein's ed25519 Public Domain ref10 work. */

#ifndef WOLF_CRYPT_GE_OPERATIONS_H
#define WOLF_CRYPT_GE_OPERATIONS_H

#include "crypto/src/settings.h"

#ifdef HAVE_ED25519

#include "crypto/src/fe_operations.h"

/*
ge means group element.

Here the group is the set of pairs (x,y) of field elements (see fe.h)
satisfying -x^2 + y^2 = 1 + d x^2y^2
where d = -121665/121666.

Representations:
  ge_p2 (projective): (X:Y:Z) satisfying x=X/Z, y=Y/Z
  ge_p3 (extended): (X:Y:Z:T) satisfying x=X/Z, y=Y/Z, XY=ZT
  ge_p1p1 (completed): ((X:Z),(Y:T)) satisfying x=X/Z, y=Y/T
  ge_precomp (Duif): (y+x,y-x,2dxy)
*/

#ifdef ED25519_SMALL
  typedef byte     ge[F25519_SIZE];
#elif defined(CURVED25519_128BIT)
  typedef int64_t  ge[5];
#else
  typedef int32_t  ge[10];
#endif

typedef struct {
  ge X;
  ge Y;
  ge Z;
} ge_p2;

typedef struct {
  ge X;
  ge Y;
  ge Z;
  ge T;
} ge_p3;


WOLFSSL_LOCAL int  ge_compress_key(byte* out, const byte* xIn, const byte* yIn,
                                                                word32 keySz);
WOLFSSL_LOCAL int  ge_frombytes_negate_vartime(ge_p3 *,const unsigned char *);

WOLFSSL_LOCAL int  ge_double_scalarmult_vartime(ge_p2 *,const unsigned char *,
                                         const ge_p3 *,const unsigned char *);
WOLFSSL_LOCAL void ge_scalarmult_base(ge_p3 *,const unsigned char *);
WOLFSSL_LOCAL void sc_reduce(byte* s);
WOLFSSL_LOCAL void sc_muladd(byte* s, const byte* a, const byte* b,
                             const byte* c);
WOLFSSL_LOCAL void ge_tobytes(unsigned char *,const ge_p2 *);
WOLFSSL_LOCAL void ge_p3_tobytes(unsigned char *,const ge_p3 *);


#ifndef ED25519_SMALL
typedef struct {
  ge X;
  ge Y;
  ge Z;
  ge T;
} ge_p1p1;

typedef struct {
  ge yplusx;
  ge yminusx;
  ge xy2d;
} ge_precomp;

typedef struct {
  ge YplusX;
  ge YminusX;
  ge Z;
  ge T2d;
} ge_cached;

WOLFSSL_LOCAL void ge_p2_0(ge_p2 *);
WOLFSSL_LOCAL void ge_p3_0(ge_p3 *);
WOLFSSL_LOCAL void ge_precomp_0(ge_precomp *);
WOLFSSL_LOCAL void ge_p3_to_p2(ge_p2 *,const ge_p3 *);
WOLFSSL_LOCAL void ge_p3_to_cached(ge_cached *,const ge_p3 *);
WOLFSSL_LOCAL void ge_p1p1_to_p2(ge_p2 *,const ge_p1p1 *);
WOLFSSL_LOCAL void ge_p1p1_to_p3(ge_p3 *,const ge_p1p1 *);
WOLFSSL_LOCAL void ge_p2_dbl(ge_p1p1 *,const ge_p2 *);
WOLFSSL_LOCAL void ge_p3_dbl(ge_p1p1 *,const ge_p3 *);

WOLFSSL_LOCAL void ge_madd(ge_p1p1 *,const ge_p3 *,const ge_precomp *);
WOLFSSL_LOCAL void ge_msub(ge_p1p1 *,const ge_p3 *,const ge_precomp *);
WOLFSSL_LOCAL void ge_add(ge_p1p1 *,const ge_p3 *,const ge_cached *);
WOLFSSL_LOCAL void ge_sub(ge_p1p1 *,const ge_p3 *,const ge_cached *);

#endif /* !ED25519_SMALL */

#endif /* HAVE_ED25519 */

#endif /* WOLF_CRYPT_GE_OPERATIONS_H */
