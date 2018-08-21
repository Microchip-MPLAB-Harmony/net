/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    misc.c
  
  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic 
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
File Name:  misc.c
Copyright © 2013 released Microchip Technology Inc.  All rights reserved.

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



#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif
#include "system_config.h"

#include "crypto/src/settings.h"

#ifndef WOLF_CRYPT_MISC_C
#define WOLF_CRYPT_MISC_C

#include "crypto/src/misc.h"

/* inlining these functions is a huge speed increase and a small size decrease,
   because the functions are smaller than function call setup/cleanup, e.g.,
   md5 benchmark is twice as fast with inline.  If you don't want it, then
   define NO_INLINE and compile this file into wolfssl, otherwise it's used as
   a source header
 */

#ifdef NO_INLINE
    #define STATIC
#else
    #define STATIC static
#endif

/* Check for if compiling misc.c when not needed. */
#if !defined(WOLFSSL_MISC_INCLUDED) && !defined(NO_INLINE)
/*    #warning misc.c does not need to be compiled when using inline (NO_INLINE not defined) */
#else


#if defined(__ICCARM__)
    #include <intrinsics.h>
#endif


#ifdef INTEL_INTRINSICS

    #include <stdlib.h>      /* get intrinsic definitions */

    /* for non visual studio probably need no long version, 32 bit only
     * i.e., _rotl and _rotr */
    #pragma intrinsic(_lrotl, _lrotr)

    STATIC INLINE word32 rotlFixed(word32 x, word32 y)
    {
        return y ? _lrotl(x, y) : x;
    }

    STATIC INLINE word32 rotrFixed(word32 x, word32 y)
    {
        return y ? _lrotr(x, y) : x;
    }

#else /* generic */

    STATIC INLINE word32 rotlFixed(word32 x, word32 y)
    {
        return (x << y) | (x >> (sizeof(y) * 8 - y));
    }


    STATIC INLINE word32 rotrFixed(word32 x, word32 y)
    {
        return (x >> y) | (x << (sizeof(y) * 8 - y));
    }

#endif


STATIC INLINE word32 ByteReverseWord32(word32 value)
{
#ifdef PPC_INTRINSICS
    /* PPC: load reverse indexed instruction */
    return (word32)__lwbrx(&value,0);
#elif defined(__ICCARM__)
    return (word32)__REV(value);
#elif defined(KEIL_INTRINSICS)
    return (word32)__rev(value);
#elif defined(FAST_ROTATE)
    /* 5 instructions with rotate instruction, 9 without */
    return (rotrFixed(value, 8U) & 0xff00ff00) |
           (rotlFixed(value, 8U) & 0x00ff00ff);
#else
    /* 6 instructions with rotate instruction, 8 without */
    value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
    return rotlFixed(value, 16U);
#endif
}


STATIC INLINE void ByteReverseWords(word32* out, const word32* in,
                                    word32 byteCount)
{
    word32 count = byteCount/(word32)sizeof(word32), i;

    for (i = 0; i < count; i++)
        out[i] = ByteReverseWord32(in[i]);

}


#ifdef WORD64_AVAILABLE


STATIC INLINE word64 rotlFixed64(word64 x, word64 y)
{
    return (x << y) | (x >> (sizeof(y) * 8 - y));
}


STATIC INLINE word64 rotrFixed64(word64 x, word64 y)
{
    return (x >> y) | (x << (sizeof(y) * 8 - y));
}


STATIC INLINE word64 ByteReverseWord64(word64 value)
{
#if defined(WOLFCRYPT_SLOW_WORD64)
	return (word64)(ByteReverseWord32((word32)value)) << 32 |
                    ByteReverseWord32((word32)(value>>32));
#else
	value = ((value & W64LIT(0xFF00FF00FF00FF00)) >> 8) |
            ((value & W64LIT(0x00FF00FF00FF00FF)) << 8);
	value = ((value & W64LIT(0xFFFF0000FFFF0000)) >> 16) |
            ((value & W64LIT(0x0000FFFF0000FFFF)) << 16);
	return rotlFixed64(value, 32U);
#endif
}


STATIC INLINE void ByteReverseWords64(word64* out, const word64* in,
                                      word32 byteCount)
{
    word32 count = byteCount/(word32)sizeof(word64), i;

    for (i = 0; i < count; i++)
        out[i] = ByteReverseWord64(in[i]);

}

#endif /* WORD64_AVAILABLE */


STATIC INLINE void XorWords(wolfssl_word* r, const wolfssl_word* a, word32 n)
{
    word32 i;

    for (i = 0; i < n; i++) r[i] ^= a[i];
}


STATIC INLINE void xorbuf(void* buf, const void* mask, word32 count)
{
    if (((wolfssl_word)buf | (wolfssl_word)mask | count) % WOLFSSL_WORD_SIZE == 0)
        XorWords( (wolfssl_word*)buf,
                  (const wolfssl_word*)mask, count / WOLFSSL_WORD_SIZE);
    else {
        word32 i;
        byte*       b = (byte*)buf;
        const byte* m = (const byte*)mask;

        for (i = 0; i < count; i++) b[i] ^= m[i];
    }
}


/* Make sure compiler doesn't skip */
STATIC INLINE void ForceZero(const void* mem, word32 len)
{
    volatile byte* z = (volatile byte*)mem;
#ifdef WOLFSSL_X86_64_BUILD
    volatile word64* w;

    for (w = (volatile word64*)z; len >= sizeof(*w); len -= sizeof(*w))
        *w++ = 0;
    z = (volatile byte*)w;
#endif
    while (len--) *z++ = 0;
}


/* check all length bytes for equality, return 0 on success */
STATIC INLINE int ConstantCompare(const byte* a, const byte* b, int length)
{
    int i;
    int compareSum = 0;

    for (i = 0; i < length; i++) {
        compareSum |= a[i] ^ b[i];
    }

    return compareSum;
}


#ifndef WOLFSSL_HAVE_MIN
    #define WOLFSSL_HAVE_MIN
    #if defined(HAVE_FIPS) && !defined(min) /* so ifdef check passes */
        #define min min
    #endif
    STATIC INLINE word32 min(word32 a, word32 b)
    {
        return a > b ? b : a;
    }
#endif /* !WOLFSSL_HAVE_MIN */

#ifndef WOLFSSL_HAVE_MAX
    #define WOLFSSL_HAVE_MAX
    #if defined(HAVE_FIPS) && !defined(max) /* so ifdef check passes */
        #define max max
    #endif
    STATIC INLINE word32 max(word32 a, word32 b)
    {
        return a > b ? a : b;
    }
#endif /* !WOLFSSL_HAVE_MAX */


#undef STATIC

#endif /* !WOLFSSL_MISC_INCLUDED && !NO_INLINE */

#endif /* WOLF_CRYPT_MISC_C */
