/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    dsa.c
  
  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic 
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
File Name:  dsa.c
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

#ifndef NO_DSA

#include "crypto/src/random.h"
#include "crypto/src/integer.h"
#include "crypto/src/error-crypt.h"
#include "crypto/src/logging.h"
#include "crypto/src/sha.h"
#include "crypto/src/dsa.h"

#ifdef NO_INLINE
    #include "crypto/src/misc.h"
#else
    #define WOLFSSL_MISC_INCLUDED
    #include "crypto/src/misc.c"
#endif


enum {
    DSA_HALF_SIZE = 20,   /* r and s size  */
    DSA_SIG_SIZE  = 40    /* signature size */
};



int wc_InitDsaKey(DsaKey* key)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    key->type = -1;  /* haven't decided yet */
    key->heap = NULL;

    return mp_init_multi(
        /* public  alloc parts */
        &key->p,
        &key->q,
        &key->g,
        &key->y,

        /* private alloc parts */
        &key->x,
        NULL
    );
}


int wc_InitDsaKey_h(DsaKey* key, void* h)
{
    int ret = wc_InitDsaKey(key);
    if (ret == 0)
        key->heap = h;

    return ret;
}


void wc_FreeDsaKey(DsaKey* key)
{
    if (key == NULL)
        return;

    if (key->type == DSA_PRIVATE)
        mp_forcezero(&key->x);

    mp_clear(&key->x);
    mp_clear(&key->y);
    mp_clear(&key->g);
    mp_clear(&key->q);
    mp_clear(&key->p);
}

#ifdef WOLFSSL_KEY_GEN

int wc_MakeDsaKey(WC_RNG *rng, DsaKey *dsa)
{
    unsigned char *buf;
    int qsize, err;

    if (rng == NULL || dsa == NULL)
        return BAD_FUNC_ARG;

    qsize = mp_unsigned_bin_size(&dsa->q);
    if (qsize == 0)
        return BAD_FUNC_ARG;

    /* allocate ram */
    buf = (unsigned char *)XMALLOC(qsize, dsa->heap,
                                   DYNAMIC_TYPE_TMP_BUFFER);
    if (buf == NULL)
        return MEMORY_E;

    if (mp_init(&dsa->x) != MP_OKAY) {
        XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MP_INIT_E;
    }

    do {
        /* make a random exponent mod q */
        err = wc_RNG_GenerateBlock(rng, buf, qsize);
        if (err != MP_OKAY) {
            mp_clear(&dsa->x);
            XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return err;
        }

        err = mp_read_unsigned_bin(&dsa->x, buf, qsize);
        if (err != MP_OKAY) {
            mp_clear(&dsa->x);
            XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return err;
        }
    } while (mp_cmp_d(&dsa->x, 1) != MP_GT);

    XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);

    if (mp_init(&dsa->y) != MP_OKAY) {
        mp_clear(&dsa->x);
        return MP_INIT_E;
    }

    /* public key : y = g^x mod p */
    err = mp_exptmod(&dsa->g, &dsa->x, &dsa->p, &dsa->y);
    if (err != MP_OKAY) {
        mp_clear(&dsa->x);
        mp_clear(&dsa->y);
        return err;
    }

    dsa->type = DSA_PRIVATE;

    return MP_OKAY;
}

/* modulus_size in bits */
int wc_MakeDsaParameters(WC_RNG *rng, int modulus_size, DsaKey *dsa)
{
    mp_int  tmp, tmp2;
    int     err, msize, qsize,
            loop_check_prime = 0,
            check_prime = MP_NO;
    unsigned char   *buf;

    if (rng == NULL || dsa == NULL)
        return BAD_FUNC_ARG;

    /* set group size in bytes from modulus size
     * FIPS 186-4 defines valid values (1024, 160) (2048, 256) (3072, 256)
     */
    switch (modulus_size) {
        case 1024:
            qsize = 20;
            break;
        case 2048:
        case 3072:
            qsize = 32;
            break;
        default:
            return BAD_FUNC_ARG;
            break;
    }

    /* modulus size in bytes */
    msize = modulus_size / 8;

    /* allocate ram */
    buf = (unsigned char *)XMALLOC(msize - qsize,
                                   dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (buf == NULL) {
        return MEMORY_E;
    }

    /* make a random string that will be multplied against q */
    err = wc_RNG_GenerateBlock(rng, buf, msize - qsize);
    if (err != MP_OKAY) {
        XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return err;
    }

    /* force magnitude */
    buf[0] |= 0xC0;

    /* force even */
    buf[msize - qsize - 1] &= ~1;

    if (mp_init_multi(&tmp2, &dsa->p, &dsa->q, 0, 0, 0) != MP_OKAY) {
        mp_clear(&dsa->q);
        XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MP_INIT_E;
    }

    err = mp_read_unsigned_bin(&tmp2, buf, msize - qsize);
    if (err != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp2);
        XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return err;
    }
    XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);

    /* make our prime q */
    err = mp_rand_prime(&dsa->q, qsize, rng, NULL);
    if (err != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp2);
        return err;
    }

    /* p = random * q */
    err = mp_mul(&dsa->q, &tmp2, &dsa->p);
    if (err != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp2);
        return err;
    }

    /* p = random * q + 1, so q is a prime divisor of p-1 */
    err = mp_add_d(&dsa->p, 1, &dsa->p);
    if (err != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp2);
        return err;
    }

    if (mp_init(&tmp) != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp2);
        return MP_INIT_E;
    }

    /* tmp = 2q  */
    err = mp_add(&dsa->q, &dsa->q, &tmp);
    if (err != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp);
        mp_clear(&tmp2);
        return err;
    }

    /* loop until p is prime */
    while (check_prime == MP_NO) {
        err = mp_prime_is_prime(&dsa->p, 8, &check_prime);
        if (err != MP_OKAY) {
            mp_clear(&dsa->q);
            mp_clear(&dsa->p);
            mp_clear(&tmp);
            mp_clear(&tmp2);
            return err;
        }

        if (check_prime != MP_YES) {
            /* p += 2q */
            err = mp_add(&tmp, &dsa->p, &dsa->p);
            if (err != MP_OKAY) {
                mp_clear(&dsa->q);
                mp_clear(&dsa->p);
                mp_clear(&tmp);
                mp_clear(&tmp2);
                return err;
            }

            loop_check_prime++;
        }
    }

    /* tmp2 += (2*loop_check_prime)
     * to have p = (q * tmp2) + 1 prime
     */
    if (loop_check_prime) {
        err = mp_add_d(&tmp2, 2*loop_check_prime, &tmp2);
        if (err != MP_OKAY) {
            mp_clear(&dsa->q);
            mp_clear(&dsa->p);
            mp_clear(&tmp);
            mp_clear(&tmp2);
            return err;
        }
    }

    if (mp_init(&dsa->g) != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp);
        mp_clear(&tmp2);
        return MP_INIT_E;
    }

    /* find a value g for which g^tmp2 != 1 */
    if (mp_set(&dsa->g, 1) != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&tmp);
        mp_clear(&tmp2);
        return MP_INIT_E;
    }

    do {
        err = mp_add_d(&dsa->g, 1, &dsa->g);
        if (err != MP_OKAY) {
            mp_clear(&dsa->q);
            mp_clear(&dsa->p);
            mp_clear(&dsa->g);
            mp_clear(&tmp);
            mp_clear(&tmp2);
            return err;
        }

        err = mp_exptmod(&dsa->g, &tmp2, &dsa->p, &tmp);
        if (err != MP_OKAY) {
            mp_clear(&dsa->q);
            mp_clear(&dsa->p);
            mp_clear(&dsa->g);
            mp_clear(&tmp);
            mp_clear(&tmp2);
            return err;
        }

    } while (mp_cmp_d(&tmp, 1) == MP_EQ);

    /* at this point tmp generates a group of order q mod p */
    mp_exch(&tmp, &dsa->g);

    mp_clear(&tmp);
    mp_clear(&tmp2);

    return MP_OKAY;
}
#endif /* WOLFSSL_KEY_GEN */


int wc_DsaSign(const byte* digest, byte* out, DsaKey* key, WC_RNG* rng)
{
    mp_int k, kInv, r, s, H;
    int    ret, sz;
    byte   buffer[DSA_HALF_SIZE];
    byte*  tmp = out;  /* initial output pointer */

    sz = min((int)sizeof(buffer), mp_unsigned_bin_size(&key->q));

    if (mp_init_multi(&k, &kInv, &r, &s, &H, 0) != MP_OKAY)
        return MP_INIT_E;

    do {
        /* generate k */
        ret = wc_RNG_GenerateBlock(rng, buffer, sz);
        if (ret != 0)
            return ret;

        buffer[0] |= 0x0C;

        if (mp_read_unsigned_bin(&k, buffer, sz) != MP_OKAY)
            ret = MP_READ_E;

        /* k is a random numnber and it should be less than q
         * if k greater than repeat
         */
    } while (mp_cmp(&k, &key->q) != MP_LT);

    if (ret == 0 && mp_cmp_d(&k, 1) != MP_GT)
        ret = MP_CMP_E;

    /* inverse k mod q */
    if (ret == 0 && mp_invmod(&k, &key->q, &kInv) != MP_OKAY)
        ret = MP_INVMOD_E;

    /* generate r, r = (g exp k mod p) mod q */
    if (ret == 0 && mp_exptmod(&key->g, &k, &key->p, &r) != MP_OKAY)
        ret = MP_EXPTMOD_E;

    if (ret == 0 && mp_mod(&r, &key->q, &r) != MP_OKAY)
        ret = MP_MOD_E;

    /* generate H from sha digest */
    if (ret == 0 && mp_read_unsigned_bin(&H, digest,SHA_DIGEST_SIZE) != MP_OKAY)
        ret = MP_READ_E;

    /* generate s, s = (kInv * (H + x*r)) % q */
    if (ret == 0 && mp_mul(&key->x, &r, &s) != MP_OKAY)
        ret = MP_MUL_E;

    if (ret == 0 && mp_add(&s, &H, &s) != MP_OKAY)
        ret = MP_ADD_E;

    if (ret == 0 && mp_mulmod(&s, &kInv, &key->q, &s) != MP_OKAY)
        ret = MP_MULMOD_E;

    /* detect zero r or s */
    if (ret == 0 && (mp_iszero(&r) == MP_YES || mp_iszero(&s) == MP_YES))
        ret = MP_ZERO_E;

    /* write out */
    if (ret == 0)  {
        int rSz = mp_unsigned_bin_size(&r);
        int sSz = mp_unsigned_bin_size(&s);

        while (rSz++ < DSA_HALF_SIZE) {
            *out++ = 0x00;  /* pad front with zeros */
        }

        if (mp_to_unsigned_bin(&r, out) != MP_OKAY)
            ret = MP_TO_E;
        else {
            out = tmp + DSA_HALF_SIZE;  /* advance to s in output */
            while (sSz++ < DSA_HALF_SIZE) {
                *out++ = 0x00;  /* pad front with zeros */
            }
            ret = mp_to_unsigned_bin(&s, out);
        }
    }

    mp_clear(&H);
    mp_clear(&s);
    mp_clear(&r);
    mp_clear(&kInv);
    mp_clear(&k);

    return ret;
}


int wc_DsaVerify(const byte* digest, const byte* sig, DsaKey* key, int* answer)
{
    mp_int w, u1, u2, v, r, s;
    int    ret = 0;

    if (mp_init_multi(&w, &u1, &u2, &v, &r, &s) != MP_OKAY)
        return MP_INIT_E;

    /* set r and s from signature */
    if (mp_read_unsigned_bin(&r, sig, DSA_HALF_SIZE) != MP_OKAY ||
        mp_read_unsigned_bin(&s, sig + DSA_HALF_SIZE, DSA_HALF_SIZE) != MP_OKAY)
        ret = MP_READ_E;

    /* sanity checks */
    if (ret == 0) {
        if (mp_iszero(&r) == MP_YES || mp_iszero(&s) == MP_YES ||
                mp_cmp(&r, &key->q) != MP_LT || mp_cmp(&s, &key->q) != MP_LT) {
            ret = MP_ZERO_E;
        }
    }

    /* put H into u1 from sha digest */
    if (ret == 0 && mp_read_unsigned_bin(&u1,digest,SHA_DIGEST_SIZE) != MP_OKAY)
        ret = MP_READ_E;

    /* w = s invmod q */
    if (ret == 0 && mp_invmod(&s, &key->q, &w) != MP_OKAY)
        ret = MP_INVMOD_E;

    /* u1 = (H * w) % q */
    if (ret == 0 && mp_mulmod(&u1, &w, &key->q, &u1) != MP_OKAY)
        ret = MP_MULMOD_E;

    /* u2 = (r * w) % q */
    if (ret == 0 && mp_mulmod(&r, &w, &key->q, &u2) != MP_OKAY)
        ret = MP_MULMOD_E;

    /* verify v = ((g^u1 * y^u2) mod p) mod q */
    if (ret == 0 && mp_exptmod(&key->g, &u1, &key->p, &u1) != MP_OKAY)
        ret = MP_EXPTMOD_E;

    if (ret == 0 && mp_exptmod(&key->y, &u2, &key->p, &u2) != MP_OKAY)
        ret = MP_EXPTMOD_E;

    if (ret == 0 && mp_mulmod(&u1, &u2, &key->p, &v) != MP_OKAY)
        ret = MP_MULMOD_E;

    if (ret == 0 && mp_mod(&v, &key->q, &v) != MP_OKAY)
        ret = MP_MULMOD_E;

    /* do they match */
    if (ret == 0 && mp_cmp(&r, &v) == MP_EQ)
        *answer = 1;
    else
        *answer = 0;

    mp_clear(&s);
    mp_clear(&r);
    mp_clear(&u1);
    mp_clear(&u2);
    mp_clear(&w);
    mp_clear(&v);

    return ret;
}


#endif /* NO_DSA */

