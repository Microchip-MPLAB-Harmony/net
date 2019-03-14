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



#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif
#include "configuration.h"

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


static WC_INLINE byte MakeByte(word32* x, word32* y, byte* s)
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

