/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    wolfmath.h
  
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

#if defined(HAVE_WOLF_BIGINT) && !defined(WOLF_BIGINT_DEFINED)
    /* raw big integer */
    typedef struct WC_BIGINT {
        byte*   buf;
        word32  len;
        void*   heap;
    } WC_BIGINT;

    #define WOLF_BIGINT_DEFINED
#endif


/* only define functions if mp_int has been declared */
#ifdef MP_INT_DEFINED

#ifndef __WOLFMATH_H__
#define __WOLFMATH_H__

    /* timing resistance array */
    #if !defined(WC_NO_CACHE_RESISTANT) && \
        ((defined(HAVE_ECC) && defined(ECC_TIMING_RESISTANT)) || \
         (defined(USE_FAST_MATH) && defined(TFM_TIMING_RESISTANT)))

        extern const wolfssl_word wc_off_on_addr[2];
    #endif

    /* common math functions */
    int get_digit_count(mp_int* a);
    mp_digit get_digit(mp_int* a, int n);
    int get_rand_digit(WC_RNG* rng, mp_digit* d);
    int mp_rand(mp_int* a, int digits, WC_RNG* rng);


    #ifdef HAVE_WOLF_BIGINT
        void wc_bigint_init(WC_BIGINT* a);
        int wc_bigint_alloc(WC_BIGINT* a, word32 sz);
        int wc_bigint_from_unsigned_bin(WC_BIGINT* a, const byte* in, word32 inlen);
        int wc_bigint_to_unsigned_bin(WC_BIGINT* a, byte* out, word32* outlen);
        void wc_bigint_zero(WC_BIGINT* a);
        void wc_bigint_free(WC_BIGINT* a);

        int wc_mp_to_bigint(mp_int* src, WC_BIGINT* dst);
        int wc_bigint_to_mp(WC_BIGINT* src, mp_int* dst);
    #endif /* HAVE_WOLF_BIGINT */

#endif /* __WOLFMATH_H__ */

#endif /* MP_INT_DEFINED */
