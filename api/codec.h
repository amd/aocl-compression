/**
 * Copyright (C) 2022, Advanced Micro Devices. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
 
 /** @file codec.h
 *  
 *  @brief Prototypes of wrapper functions of the supported codec methods.
 *
 *  This file contains the prototypes of wrapper functions of the supported
 *  compression and decompression methods along with a data structure to
 *  hold the function pointers of each of them.
 *
 *  @author S. Biplab Raut
 */
 
#ifndef CODEC_H
#define CODEC_H

typedef INT64 (*comp_decomp_fp)(CHAR *inStream, UINTP inSize, CHAR *outStream,
                                UINTP outSize, UINTP, UINTP, CHAR*);
typedef CHAR* (*setup_fp)(INTP optOff, INTP optLevel, UINTP inSize, UINTP,
                          UINTP);
typedef VOID  (*destroy_fp)(CHAR *memBuff);

//Method 1
#ifndef AOCL_EXCLUDE_BZIP2
        INT64 aocl_bzip2_compress(CHAR *inBuf, UINTP inSize, CHAR *outBuf,
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        INT64 aocl_bzip2_decompress(CHAR *inBuf, UINTP inSize, CHAR *outBuf,
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        CHAR *aocl_bzip2_setup(INTP optOff, INTP optLevel,
                           UINTP insize, UINTP level, UINTP windowLog);
#else
	#define aocl_bzip2_compress NULL
	#define aocl_bzip2_decompress NULL
        #define aocl_bzip2_setup NULL
#endif
//Method 2
#ifndef AOCL_EXCLUDE_LZ4
        INT64 aocl_lz4_compress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        INT64 aocl_lz4_decompress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        CHAR *aocl_lz4_setup(INTP optOff, INTP optLevel,
                         UINTP insize, UINTP level, UINTP windowLog);
#else
	#define aocl_lz4_compress NULL
	#define aocl_lz4_decompress NULL
        #define aocl_lz4_setup NULL
#endif
//Method 3
#ifndef AOCL_EXCLUDE_LZ4HC
        INT64 aocl_lz4hc_compress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        INT64 aocl_lz4hc_decompress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        CHAR *aocl_lz4hc_setup(INTP optOff, INTP optLevel,
                         UINTP insize, UINTP level, UINTP windowLog);
#else
	#define aocl_lz4hc_compress NULL
	#define aocl_lz4hc_decompress NULL
        #define aocl_lz4hc_setup NULL
#endif
//Method 4
#ifndef AOCL_EXCLUDE_LZMA
        INT64 aocl_lzma_compress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        INT64 aocl_lzma_decompress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        CHAR *aocl_lzma_setup(INTP optOff, INTP optLevel,
                         UINTP insize, UINTP level, UINTP windowLog);
#else
	#define aocl_lzma_compress NULL
	#define aocl_lzma_decompress NULL
        #define aocl_lzma_setup NULL
#endif
//Method 5
#ifndef AOCL_EXCLUDE_SNAPPY
        INT64 aocl_snappy_compress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        INT64 aocl_snappy_decompress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        CHAR *aocl_snappy_setup(INTP optOff, INTP optLevel,
                         UINTP insize, UINTP level, UINTP windowLog);
#else
	#define aocl_snappy_compress NULL
	#define aocl_snappy_decompress NULL
        #define aocl_snappy_setup NULL
#endif
//Method 6
#ifndef AOCL_EXCLUDE_ZLIB
        INT64 aocl_zlib_compress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        INT64 aocl_zlib_decompress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        CHAR *aocl_zlib_setup(INTP optOff, INTP optLevel,
                         UINTP insize, UINTP level, UINTP windowLog);
#else
	#define aocl_zlib_compress NULL
	#define aocl_zlib_decompress NULL
        #define aocl_zlib_setup NULL
#endif
//Method 7
#ifndef AOCL_EXCLUDE_ZSTD
        INT64 aocl_zstd_compress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        INT64 aocl_zstd_decompress(CHAR *inBuf, UINTP inSize, CHAR *outBuf, 
                         UINTP outSize, UINTP level, UINTP optVar, CHAR *workBuf);
        CHAR *aocl_zstd_setup(INTP optOff, INTP optLevel,
                         UINTP insize, UINTP level, UINTP windowLog);
        VOID aocl_zstd_destroy(CHAR *workmem);
#else
	#define aocl_zstd_compress NULL
	#define aocl_zstd_decompress NULL
	#define aocl_zstd_setup NULL
	#define aocl_zstd_destroy NULL
#endif

typedef struct
{
    const CHAR* codec_name;
    const CHAR* codec_version;
    comp_decomp_fp compress;
    comp_decomp_fp decompress;
    setup_fp setup;
    destroy_fp destroy;
} aocl_codec_t;

static const aocl_codec_t aocl_codec[AOCL_COMPRESSOR_ALGOS_NUM] =
{
    { "lz4",    "1.9.3",      aocl_lz4_compress,    aocl_lz4_decompress,    aocl_lz4_setup,    NULL },
    { "lz4hc",  "1.9.3",      aocl_lz4hc_compress,  aocl_lz4_decompress,    aocl_lz4hc_setup,  NULL },
    { "lzma",   "22.01",      aocl_lzma_compress,   aocl_lzma_decompress,   aocl_lzma_setup,   NULL },
    { "bzip2",  "1.0.8",      aocl_bzip2_compress,  aocl_bzip2_decompress,  aocl_bzip2_setup,  NULL },
    { "snappy", "2020-07-11", aocl_snappy_compress, aocl_snappy_decompress, aocl_snappy_setup, NULL },
    { "zlib",   "1.2.11",     aocl_zlib_compress,   aocl_zlib_decompress,   aocl_zlib_setup,   NULL },
    { "zstd",   "1.5.0",      aocl_zstd_compress,   aocl_zstd_decompress,   aocl_zstd_setup,   aocl_zstd_destroy }
};

#endif
