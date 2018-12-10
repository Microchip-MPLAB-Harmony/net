/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    wolfmath.c
  
  Summary:
    Crypto Framework Library source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic 
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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


/* common functions for either math library */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

/* in case user set USE_FAST_MATH there */
#include "crypto/src/settings.h"

#ifdef USE_FAST_MATH
    #include "crypto/src/tfm.h"
#else
    #include "crypto/src/integer.h"
#endif

#include "crypto/src/error-crypt.h"
#include "crypto/src/logging.h"

#if defined(USE_FAST_MATH) || !defined(NO_BIG_INT)

#ifdef WOLFSSL_ASYNC_CRYPT
    #include "crypto/src/async.h"
#endif

#ifdef NO_INLINE
    #include "crypto/src/misc.h"
#else
    #define WOLFSSL_MISC_INCLUDED
    #include "crypto/src/misc.c"
#endif


#if !defined(WC_NO_CACHE_RESISTANT) && \
    ((defined(HAVE_ECC) && defined(ECC_TIMING_RESISTANT)) || \
     (defined(USE_FAST_MATH) && defined(TFM_TIMING_RESISTANT)))

    /* all off / all on pointer addresses for constant calculations */
    /* ecc.c uses same table */
    const wolfssl_word wc_off_on_addr[2] =
    {
    #if defined(WC_64BIT_CPU)
        W64LIT(0x0000000000000000),
        W64LIT(0xffffffffffffffff)
    #elif defined(WC_16BIT_CPU)
        0x0000U,
        0xffffU
    #else
        /* 32 bit */
        0x00000000U,
        0xffffffffU
    #endif
    };
#endif


int get_digit_count(mp_int* a)
{
    if (a == NULL)
        return 0;

    return a->used;
}

mp_digit get_digit(mp_int* a, int n)
{
    if (a == NULL)
        return 0;

    return (n >= a->used || n < 0) ? 0 : a->dp[n];
}

int get_rand_digit(WC_RNG* rng, mp_digit* d)
{
    return wc_RNG_GenerateBlock(rng, (byte*)d, sizeof(mp_digit));
}

#ifdef WC_RSA_BLINDING
int mp_rand(mp_int* a, int digits, WC_RNG* rng)
{
    int ret;
    mp_digit d;

    if (rng == NULL)
        return MISSING_RNG_E;

    if (a == NULL)
        return BAD_FUNC_ARG;

    mp_zero(a);
    if (digits <= 0) {
        return MP_OKAY;
    }

    /* first place a random non-zero digit */
    do {
        ret = get_rand_digit(rng, &d);
        if (ret != 0) {
            return ret;
        }
    } while (d == 0);

    if ((ret = mp_add_d(a, d, a)) != MP_OKAY) {
        return ret;
    }

    while (--digits > 0) {
        if ((ret = mp_lshd(a, 1)) != MP_OKAY) {
            return ret;
        }
        if ((ret = get_rand_digit(rng, &d)) != 0) {
            return ret;
        }
        if ((ret = mp_add_d(a, d, a)) != MP_OKAY) {
            return ret;
        }
    }

    return ret;
}
#endif /* WC_RSA_BLINDING */


#ifdef HAVE_WOLF_BIGINT
void wc_bigint_init(WC_BIGINT* a)
{
    if (a != NULL) {
        a->buf = NULL;
        a->len = 0;
        a->heap = NULL;
    }
}

int wc_bigint_alloc(WC_BIGINT* a, word32 sz)
{
    int err = MP_OKAY;

    if (a == NULL)
        return BAD_FUNC_ARG;

    if (sz > 0) {
        if (a->buf && sz > a->len) {
            wc_bigint_free(a);
        }
        if (a->buf == NULL) {
            a->buf = (byte*)XMALLOC(sz, a->heap, DYNAMIC_TYPE_WOLF_BIGINT);
        }
        if (a->buf == NULL) {
            err = MP_MEM;
        }
        else {
            XMEMSET(a->buf, 0, sz);
        }
    }
    a->len = sz;

    return err;
}

/* assumes input is big endian format */
int wc_bigint_from_unsigned_bin(WC_BIGINT* a, const byte* in, word32 inlen)
{
    int err;

    if (a == NULL || in == NULL || inlen == 0)
        return BAD_FUNC_ARG;

    err = wc_bigint_alloc(a, inlen);
    if (err == 0) {
        XMEMCPY(a->buf, in, inlen);
    }

    return err;
}

int wc_bigint_to_unsigned_bin(WC_BIGINT* a, byte* out, word32* outlen)
{
    word32 sz;

    if (a == NULL || out == NULL || outlen == NULL || *outlen == 0)
        return BAD_FUNC_ARG;

    /* trim to fit into output buffer */
    sz = a->len;
    if (a->len > *outlen) {
        WOLFSSL_MSG("wc_bigint_export: Truncating output");
        sz = *outlen;
    }

    if (a->buf) {
        XMEMCPY(out, a->buf, sz);
    }

    *outlen = sz;

    return MP_OKAY;
}

void wc_bigint_zero(WC_BIGINT* a)
{
    if (a && a->buf) {
        ForceZero(a->buf, a->len);
    }
}

void wc_bigint_free(WC_BIGINT* a)
{
    if (a) {
        if (a->buf) {
          XFREE(a->buf, a->heap, DYNAMIC_TYPE_WOLF_BIGINT);
        }
        a->buf = NULL;
        a->len = 0;
    }
}

int wc_mp_to_bigint(mp_int* src, WC_BIGINT* dst)
{
    int err;
    word32 sz;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    sz = mp_unsigned_bin_size(src);
    err = wc_bigint_alloc(dst, sz);
    if (err == MP_OKAY)
        err = mp_to_unsigned_bin(src, dst->buf);

    return err;
}

int wc_bigint_to_mp(WC_BIGINT* src, mp_int* dst)
{
    int err;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    if (src->buf == NULL)
        return BAD_FUNC_ARG;

    err = mp_read_unsigned_bin(dst, src->buf, src->len);
    wc_bigint_free(src);

    return err;
}

#endif /* HAVE_WOLF_BIGINT */

#endif /* USE_FAST_MATH || !NO_BIG_INT */
