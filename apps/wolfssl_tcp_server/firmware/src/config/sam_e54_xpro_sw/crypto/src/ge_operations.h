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
#elif defined(CURVED25519_X64)
  typedef int64_t  ge[4];
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

#endif /* !ED25519_SMALL */

#endif /* HAVE_ED25519 */

#endif /* WOLF_CRYPT_GE_OPERATIONS_H */
