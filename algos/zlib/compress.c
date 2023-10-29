/* compress.c -- compress a memory buffer
 * Copyright (C) 1995-2005, 2014, 2016 Jean-loup Gailly, Mark Adler
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#define ZLIB_INTERNAL
#include "zlib.h"
#include "utils/utils.h"

#ifdef AOCL_ZLIB_OPT
/* Dynamic dispatcher setup function for native APIs.
 * All native APIs that call aocl optimized functions within their call stack,
 * must call AOCL_SETUP_NATIVE() at the start of the function. This sets up 
 * appropriate code paths to take based on user defined environment variables,
 * as well as cpu instruction set supported by the runtime machine. */
static void aocl_setup_native(void);
#define AOCL_SETUP_NATIVE() aocl_setup_native()
#else
#define AOCL_SETUP_NATIVE()
#endif

int zlibOptOff = 0; // default, run reference code
static int setup_ok_zlib = 0; // flag to indicate status of dynamic dispatcher setup

/* AOCL-Compression defined setup function that sets up ZLIB with the right
*  AMD optimized zlib routines depending upon the CPU features. */
ZEXTERN char * ZEXPORT aocl_setup_zlib(int optOff, int optLevel, int insize,
    int level, int windowLog)
{
    AOCL_ENTER_CRITICAL(setup_zlib)
    if (!setup_ok_zlib) {
        optOff = optOff ? 1 : get_disable_opt_flags(0);
        zlibOptOff = optOff;
        aocl_setup_deflate(optOff, optLevel);
        aocl_setup_tree   (optOff, optLevel);
        aocl_setup_inflate(optOff, optLevel);
        aocl_setup_adler32(optOff, optLevel);
        setup_ok_zlib = 1;
    }
    AOCL_EXIT_CRITICAL(setup_zlib)
    return NULL;
}

#ifdef AOCL_ZLIB_OPT
static void aocl_setup_native(void) {
    AOCL_ENTER_CRITICAL(setup_zlib)
    if (!setup_ok_zlib) {
        int optLevel = get_cpu_opt_flags(0);
        int optOff = get_disable_opt_flags(0);
        zlibOptOff = optOff;
        aocl_setup_deflate(optOff, optLevel);
        aocl_setup_tree   (optOff, optLevel);
        aocl_setup_inflate(optOff, optLevel);
        aocl_setup_adler32(optOff, optLevel);
        setup_ok_zlib = 1;
    }
    AOCL_EXIT_CRITICAL(setup_zlib)
}
#endif

ZEXTERN void ZEXPORT aocl_destroy_zlib (void) {
    AOCL_ENTER_CRITICAL(setup_zlib)
    setup_ok_zlib = 0;
    AOCL_EXIT_CRITICAL(setup_zlib)
    aocl_destroy_adler32();
    aocl_destroy_deflate();
    aocl_destroy_tree();
    aocl_destroy_inflate();
}

/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
int ZEXPORT compress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen, int level) {
    LOG_UNFORMATTED(TRACE, logCtx, "Enter");
    AOCL_SETUP_NATIVE();
    if(destLen == NULL)
    {
        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return Z_BUF_ERROR;
    }
    
    z_stream stream;
    int err;
    const uInt max = (uInt)-1;
    uLong left;

    left = *destLen;
    *destLen = 0;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit(&stream, level);
    if (err != Z_OK)
    {
        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return err;
    }

    stream.next_out = dest;
    stream.avail_out = 0;
    stream.next_in = (z_const Bytef *)source;
    stream.avail_in = 0;

    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong)max ? max : (uInt)left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
            sourceLen -= stream.avail_in;
        }
        err = deflate(&stream, sourceLen ? Z_NO_FLUSH : Z_FINISH);
    } while (err == Z_OK);

    *destLen = stream.total_out;
    deflateEnd(&stream);
    LOG_UNFORMATTED(INFO, logCtx, "Exit");
    
    return err == Z_STREAM_END ? Z_OK : err;
}

/* ===========================================================================
 */
int ZEXPORT compress(Bytef *dest, uLongf *destLen, const Bytef *source,
                     uLong sourceLen) {
    return compress2(dest, destLen, source, sourceLen, Z_DEFAULT_COMPRESSION);
}

/* ===========================================================================
     If the default memLevel or windowBits for deflateInit() is changed, then
   this function needs to be updated.
 */
uLong ZEXPORT compressBound(uLong sourceLen) {
    return sourceLen + (sourceLen >> 12) + (sourceLen >> 14) +
           (sourceLen >> 25) + 13;
}
