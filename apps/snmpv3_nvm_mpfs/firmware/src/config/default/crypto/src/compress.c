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
/******************************************************************************
File Name:  compress.c
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


int wc_Compress(byte* out, word32 outSz, const byte* in, word32 inSz, word32 flags)
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
                     DEFLATE_DEFAULT_WINDOWBITS, DEFLATE_DEFAULT_MEMLEVEL,
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


int wc_DeCompress(byte* out, word32 outSz, const byte* in, word32 inSz)
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

    if (inflateInit2(&stream, DEFLATE_DEFAULT_WINDOWBITS) != Z_OK)
        return DECOMPRESS_INIT_E;

    if (inflate(&stream, Z_FINISH) != Z_STREAM_END) {
        inflateEnd(&stream);
        return DECOMPRESS_E;
    }
    
    result = (int)stream.total_out;

    if (inflateEnd(&stream) != Z_OK)
        result = DECOMPRESS_E;

    return result;
}


#endif /* HAVE_LIBZ */

