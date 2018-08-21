/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    arc4.c
  
  Summary:
    Crypto Framework Libarary source for cryptographic functions.

  Description:
    This source file contains functions that make up the Cryptographic 
	Framework Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
File Name:  arc4.c
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



#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif
#include "system_config.h"

#include "crypto/src/settings.h"

#ifndef NO_RC4

#include "crypto/src/error-crypt.h"
#include "crypto/src/arc4.h"


int wc_Arc4SetKey(Arc4* arc4, const byte* key, word32 length)
{
    int ret = 0;
    word32 i;
    word32 keyIndex = 0, stateIndex = 0;

    if (arc4 == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4) && \
        defined(HAVE_CAVIUM) && !defined(HAVE_CAVIUM_V)
    if (arc4->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ARC4) {
        return NitroxArc4SetKey(arc4, key, length);
    }
#endif

    arc4->x = 1;
    arc4->y = 0;

    for (i = 0; i < ARC4_STATE_SIZE; i++)
        arc4->state[i] = (byte)i;

    for (i = 0; i < ARC4_STATE_SIZE; i++) {
        word32 a = arc4->state[i];
        stateIndex += key[keyIndex] + a;
        stateIndex &= 0xFF;
        arc4->state[i] = arc4->state[stateIndex];
        arc4->state[stateIndex] = (byte)a;

        if (++keyIndex >= length)
            keyIndex = 0;
    }

    return ret;
}


static INLINE byte MakeByte(word32* x, word32* y, byte* s)
{
    word32 a = s[*x], b;
    *y = (*y+a) & 0xff;

    b = s[*y];
    s[*x] = (byte)b;
    s[*y] = (byte)a;
    *x = (*x+1) & 0xff;

    return s[(a+b) & 0xff];
}


int wc_Arc4Process(Arc4* arc4, byte* out, const byte* in, word32 length)
{
    int ret = 0;
    word32 x;
    word32 y;

    if (arc4 == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4) && \
        defined(HAVE_CAVIUM) && !defined(HAVE_CAVIUM_V)
    if (arc4->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ARC4) {
        return NitroxArc4Process(arc4, out, in, length);
    }
#endif

    x = arc4->x;
    y = arc4->y;

    while(length--)
        *out++ = *in++ ^ MakeByte(&x, &y, arc4->state);

    arc4->x = (byte)x;
    arc4->y = (byte)y;

    return ret;
}

/* Initialize Arc4 for use with async device */
int wc_Arc4Init(Arc4* arc4, void* heap, int devId)
{
    int ret = 0;

    if (arc4 == NULL)
        return BAD_FUNC_ARG;

    arc4->heap = heap;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4)
    ret = wolfAsync_DevCtxInit(&arc4->asyncDev, WOLFSSL_ASYNC_MARKER_ARC4,
        arc4->heap, devId);
#else
    (void)devId;
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}


/* Free Arc4 from use with async device */
void wc_Arc4Free(Arc4* arc4)
{
    if (arc4 == NULL)
        return;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4)
    wolfAsync_DevCtxFree(&arc4->asyncDev, WOLFSSL_ASYNC_MARKER_ARC4);
#endif /* WOLFSSL_ASYNC_CRYPT */
}

#endif /* NO_RC4 */

