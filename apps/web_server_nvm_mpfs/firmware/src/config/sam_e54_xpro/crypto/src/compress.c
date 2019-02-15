/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    compress.c
  
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
    #include "config.h"
#endif
#include "configuration.h"

#include "crypto/src/settings.h"

#ifdef HAVE_LIBZ


#include "crypto/src/compress.h"
#include "crypto/src/error-crypt.h"
#include "crypto/src/logging.h"
#ifdef NO_INLINE
    #include "crypto/src/misc.h"
#else
    #define WOLFSSL_MISC_INCLUDED
    #include "crypto/src/misc.c"
#endif

#include "crypto/src/zlib-1.2.7/zlib.h"


/* alloc user allocs to work with zlib */
static void* myAlloc(void* opaque, unsigned int item, unsigned int size)
{
    (void)opaque;
    return XMALLOC(item * size, opaque, DYNAMIC_TYPE_LIBZ);
}


static void myFree(void* opaque, void* memory)
{
    (void)opaque;
    XFREE(memory, opaque, DYNAMIC_TYPE_LIBZ);
}


#ifdef HAVE_MCAPI
    #define DEFLATE_DEFAULT_WINDOWBITS  11
    #define DEFLATE_DEFAULT_MEMLEVEL     1
#else
    #define DEFLATE_DEFAULT_WINDOWBITS 15
    #define DEFLATE_DEFAULT_MEMLEVEL    8
#endif


/*
 * out - pointer to destination buffer
 * outSz - size of destination buffer
 * in - pointer to source buffer to compress
 * inSz - size of source to compress
 * flags - flags to control how compress operates
 *
 * return:
 *    negative - error code
 *    positive - bytes stored in out buffer
 *
 * Note, the output buffer still needs to be larger than the input buffer.
 * The right chunk of data won't compress at all, and the lookup table will
 * add to the size of the output. The libz code says the compressed
 * buffer should be srcSz + 0.1% + 12.
 */
int wc_Compress_ex(byte* out, word32 outSz, const byte* in, word32 inSz,
    word32 flags, word32 windowBits)
{
    z_stream stream;
    int result = 0;

    stream.next_in = (Bytef*)in;
    stream.avail_in = (uInt)inSz;
#ifdef MAXSEG_64K
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != inSz) return COMPRESS_INIT_E;
#endif
    stream.next_out = out;
    stream.avail_out = (uInt)outSz;
    if ((uLong)stream.avail_out != outSz) return COMPRESS_INIT_E;

    stream.zalloc = (alloc_func)myAlloc;
    stream.zfree = (free_func)myFree;
    stream.opaque = (voidpf)0;

    if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                     DEFLATE_DEFAULT_WINDOWBITS | windowBits,
                     DEFLATE_DEFAULT_MEMLEVEL,
                     flags ? Z_FIXED : Z_DEFAULT_STRATEGY) != Z_OK)
        return COMPRESS_INIT_E;

    if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
        deflateEnd(&stream);
        return COMPRESS_E;
    }

    result = (int)stream.total_out;

    if (deflateEnd(&stream) != Z_OK)
        result = COMPRESS_E;

    return result;
}

int wc_Compress(byte* out, word32 outSz, const byte* in, word32 inSz, word32 flags)
{
    return wc_Compress_ex(out, outSz, in, inSz, flags, 0);
}


/* windowBits:
* deflateInit() and inflateInit(), as well as deflateInit2() and inflateInit2()
    with windowBits in 0..15 all process zlib-wrapped deflate data.
    (See RFC 1950 and RFC 1951.)
* deflateInit2() and inflateInit2() with negative windowBits in -1..-15 process
    raw deflate data with no header or trailer.
* deflateInit2() and inflateInit2() with windowBits in 16..31, i.e. 16
    added to 0..15, process gzip-wrapped deflate data (RFC 1952).
* inflateInit2() with windowBits in 32..47 (32 added to 0..15) will
    automatically detect either a gzip or zlib header (but not raw deflate
    data), and decompress accordingly.
*/
int wc_DeCompress_ex(byte* out, word32 outSz, const byte* in, word32 inSz,
    int windowBits)
/*
 * out - pointer to destination buffer
 * outSz - size of destination buffer
 * in - pointer to source buffer to compress
 * inSz - size of source to compress
 * windowBits - flags to control how decompress operates
 *
 * return:
 *    negative - error code
 *    positive - bytes stored in out buffer
 */
{
    z_stream stream;
    int result = 0;

    stream.next_in = (Bytef*)in;
    stream.avail_in = (uInt)inSz;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != inSz) return DECOMPRESS_INIT_E;

    stream.next_out = out;
    stream.avail_out = (uInt)outSz;
    if ((uLong)stream.avail_out != outSz) return DECOMPRESS_INIT_E;

    stream.zalloc = (alloc_func)myAlloc;
    stream.zfree = (free_func)myFree;
    stream.opaque = (voidpf)0;

    if (inflateInit2(&stream, DEFLATE_DEFAULT_WINDOWBITS | windowBits) != Z_OK)
        return DECOMPRESS_INIT_E;

    result = inflate(&stream, Z_FINISH);
    if (result != Z_STREAM_END) {
        inflateEnd(&stream);
        return DECOMPRESS_E;
    }

    result = (int)stream.total_out;

    if (inflateEnd(&stream) != Z_OK)
        result = DECOMPRESS_E;

    return result;
}


int wc_DeCompress(byte* out, word32 outSz, const byte* in, word32 inSz)
{
    return wc_DeCompress_ex(out, outSz, in, inSz, 0);
}


#endif /* HAVE_LIBZ */

