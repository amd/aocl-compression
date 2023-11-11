/**
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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
 

#ifndef _WINDOWS
#define _GNU_SOURCE
#include <dlfcn.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "api/types.h"
#include "api/aocl_compression.h"
#include "codec_bench.h"

//Include the LZ4 and LZ4HC headers
#ifndef AOCL_EXCLUDE_LZ4
#include "algos/lz4/lz4.h"
#endif

#if !defined(AOCL_EXCLUDE_LZ4HC) && !defined(AOCL_EXCLUDE_LZ4)
#include "algos/lz4/lz4hc.h"
#endif

//Include the Snappy headers 
#ifndef AOCL_EXCLUDE_SNAPPY
#include "algos/snappy/snappy-c.h"
#endif

//Include the ZLIB header
#ifndef AOCL_EXCLUDE_ZLIB
#include "algos/zlib/zlib.h"
#endif

//Include the BZIP2 headerz
#ifndef AOCL_EXCLUDE_BZIP2
#include "algos/bzip2/bzlib.h"
#endif

//Include the LZMA headers
#ifndef AOCL_EXCLUDE_LZMA
#include "algos/lzma/Alloc.h"
#include "algos/lzma/LzmaDec.h"
#include "algos/lzma/LzmaEnc.h"
#endif

//Include the ZSTD headers
#ifndef AOCL_EXCLUDE_ZSTD
#define ZSTD_STATIC_LINKING_ONLY
#include "algos/zstd/lib/zstd.h"
#endif

/* Wrapper functions defination for Compression and Decompression.  */

//LZ4
AOCL_INT64 native_lz4_compress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize, AOCL_UINTP level)
{   
    //Perform the compression
    #ifndef AOCL_EXCLUDE_LZ4
    AOCL_INT64 resultComp = LZ4_compress_default(inbuf, outbuf, insize, outsize);
    if (resultComp < 0)
    {
        fprintf(stderr, "LZ4 compression failed. \n");
        return -1;
    }
    return resultComp;
    #else
        return -2;
    #endif
}

AOCL_INT64 native_lz4_decompress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize)
{   
    //Perform the decompression
    #ifndef AOCL_EXCLUDE_LZ4
    AOCL_INT64 resultDecomp = LZ4_decompress_safe(inbuf, outbuf, insize, outsize);
    if (resultDecomp < 0)
    {
        fprintf(stderr, "LZ4 decompression failed. \n");
        return -1;
    }
    return resultDecomp;
    #else
        return -2;
    #endif
}

//LZ4HC
AOCL_INT64 native_lz4hc_compress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize, AOCL_UINTP level)
{
    //Perform the compression
    #if !defined(AOCL_EXCLUDE_LZ4HC) && !defined(AOCL_EXCLUDE_LZ4)
    AOCL_INT64 resultComp = LZ4_compress_HC(inbuf, outbuf, insize, outsize, level);
    if (resultComp < 0)
    {
        fprintf(stderr, "LZ4 compression failed. \n");
        return -1;
    }
    return resultComp;
    #else
        return -2;
    #endif
}

AOCL_INT64 native_lz4hc_decompress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize)
{
    //Perform the decompression
    #if !defined(AOCL_EXCLUDE_LZ4HC) && !defined(AOCL_EXCLUDE_LZ4)
    AOCL_INT64 resultDecomp = LZ4_decompress_safe(inbuf, outbuf, insize, outsize);
    if (resultDecomp < 0)
    {
        fprintf(stderr, "LZ4 decompression failed. \n");
        return -1;
    }
    return resultDecomp;
    #else
        return -2;
    #endif
}

//SNAPPY
AOCL_INT64 native_snappy_compress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize, AOCL_UINTP level)
{
    //Perform the compression
    #ifndef AOCL_EXCLUDE_SNAPPY
    snappy_status status = snappy_compress(inbuf, insize, outbuf, &outsize); 
    if (status != SNAPPY_OK)
    {
        fprintf(stderr, "Snappy compression failed. \n");
        return -1;
    }
    return outsize;
    #else
        return -2; 
    #endif
}

AOCL_INT64 native_snappy_decompress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize)
{
    //Perform the decompression
    #ifndef AOCL_EXCLUDE_SNAPPY
    snappy_status status = snappy_uncompress(inbuf, insize, outbuf, &outsize);
    if (status != SNAPPY_OK)
    {
        fprintf(stderr, "Snappy Decompression failed. \n");
        return -1;
    }
    return outsize;
    #else
        return -2;
    #endif
}

//ZLIB
AOCL_INT64 native_zlib_compress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize, AOCL_UINTP level)
{   
    #ifndef AOCL_EXCLUDE_ZLIB
    uLongf destLen = outsize;

    //Perform the compression
    AOCL_INTP result = compress2((AOCL_UINT8 *)outbuf, &destLen, (AOCL_UINT8 *)inbuf, insize, level);
    if (result != Z_OK)
    {
        fprintf(stderr, "ZLIB compression failed. \n");
        return -1;
    }
    //AOCL_UINTP outsize = destLen;
    return destLen;
    #else
        return -2;
    #endif
}

AOCL_INT64 native_zlib_decompress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize)
{   
    #ifndef AOCL_EXCLUDE_ZLIB
    uLongf destLen = outsize;

    //Perform the decompression
    AOCL_INTP result = uncompress((AOCL_UINT8 *)outbuf, &destLen, (AOCL_UINT8 *)inbuf, insize);
    if (result != Z_OK)
    {
        fprintf(stderr, "ZLIB Decompression failed. \n");
        return -1;
    }
    return destLen;
    #else
        return -2;
    #endif
}

//BZIP2
AOCL_INT64 native_bzip2_compress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize, AOCL_UINTP level)
{
    #ifndef AOCL_EXCLUDE_BZIP2
    AOCL_UINT32 outSizeL = outsize;
    //workFactor(best compression) and verbosity are default(0,0)
    AOCL_INTP result = BZ2_bzBuffToBuffCompress(outbuf, &outSizeL, 
                inbuf, insize, level, 0, 0);

    if (result != BZ_OK)
    {
        fprintf(stderr, "BZIP2 Compression failed. \n");
        return -1;
    }
    return outSizeL;
    #else
        return -2;
    #endif
}

AOCL_INT64 native_bzip2_decompress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize)
{   
    //Perform the decompression
    #ifndef AOCL_EXCLUDE_BZIP2
    AOCL_UINT32 outSizeL = outsize;
    AOCL_INTP result = BZ2_bzBuffToBuffDecompress(outbuf, &outSizeL, inbuf, insize, 0, 0);

    if (result != BZ_OK)
    {
        fprintf(stderr, "BZIP2 decompression failed. \n");
        return -1;
    }
    return outSizeL;
    #else
        return -2;
    #endif
}

//LZMA
#ifndef AOCL_EXCLUDE_LZMA
#define IGNORE_VAR(x) (void)x;
static void *LzmaAlloc(ISzAllocPtr p, size_t size) { IGNORE_VAR(p); return malloc(size); }
static void LzmaFree(ISzAllocPtr p, void *address) { IGNORE_VAR(p); free(address); }
#endif
AOCL_INT64 native_lzma_compress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize, AOCL_UINTP level)
{   
    #ifndef AOCL_EXCLUDE_LZMA
    CLzmaEncProps encProps;
    AOCL_UINTP headerSize = LZMA_PROPS_SIZE;
    SizeT outLen = outsize - LZMA_PROPS_SIZE;
	
    LzmaEncProps_Init(&encProps);
    encProps.level = level;
	const ISzAlloc lzma_alloc = { LzmaAlloc, LzmaFree};

    //Perform the compression
    AOCL_INTP result = LzmaEncode((AOCL_UINT8 *)outbuf+LZMA_PROPS_SIZE, &outLen, (AOCL_UINT8 *)inbuf, 
                     insize, &encProps, (AOCL_UINT8 *)outbuf, &headerSize, 0, NULL, 
                     &lzma_alloc, &lzma_alloc);
	if (result != SZ_OK)
    {
        fprintf(stderr, "LZMA compression failed. \n");
        return -1;
    }
    return LZMA_PROPS_SIZE + outLen;
    #else
        return -2;
    #endif
}

AOCL_INT64 native_lzma_decompress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize)
{   
    #ifndef AOCL_EXCLUDE_LZMA
    SizeT outLen = outsize;
    SizeT srcLen = insize - LZMA_PROPS_SIZE;
    ELzmaStatus status;
	const ISzAlloc lzma_alloc = { LzmaAlloc, LzmaFree};
	
    //Perform the decompression
    AOCL_INTP result = LzmaDecode((AOCL_UINT8 *)outbuf, &outLen, (AOCL_UINT8 *)inbuf+LZMA_PROPS_SIZE, 
                     &srcLen, (AOCL_UINT8 *)inbuf, LZMA_PROPS_SIZE, LZMA_FINISH_END,
                     &status, &lzma_alloc);
    if (result != SZ_OK)
    {
        fprintf(stderr, "LZMA Decompression failed. \n");
        return -1;
    }
    return outLen;
    #else
        return -2;
    #endif
}

//ZSTD
AOCL_INT64 native_zstd_compress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize, AOCL_UINTP level)
{
    #ifndef AOCL_EXCLUDE_ZSTD
    #define ZSTD_STATIC_LINKING_ONLY
    AOCL_INT64 res;
    ZSTD_CCtx*  cctx = ZSTD_createCCtx();
    if (!cctx)
    {
        fprintf(stderr, "ZSTD compression context creation failed. \n");
        return -1;
    }
    ZSTD_parameters zparams = ZSTD_getParams(level, insize, 0);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    //Perform the compression
    res = ZSTD_compress_advanced(cctx, outbuf, outsize, inbuf, insize, NULL, 0, zparams);
    #pragma GCC diagnostic pop

    if (ZSTD_isError(res)) {
        fprintf(stderr, "ZSTD compression error: %s", ZSTD_getErrorName(res));
        res = -1;
    }

    if (cctx)
        ZSTD_freeCCtx(cctx);

    return res;
    #else
        return -2;
    #endif 
}
AOCL_INT64 native_zstd_decompress(AOCL_CHAR *inbuf, AOCL_UINTP insize, AOCL_CHAR *outbuf, AOCL_UINTP outsize)
{
    #ifndef AOCL_EXCLUDE_ZSTD
    #define ZSTD_STATIC_LINKING_ONLY
    AOCL_INT64 res;
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    if(!dctx)
    {
        fprintf(stderr, "ZSTD decompression context creation failed. \n");
        return -1;
    }

    //Perform the decompression
    res = ZSTD_decompressDCtx(dctx, outbuf, outsize, inbuf, insize);
    if (ZSTD_isError(res))
    {
        fprintf(stderr, "ZSTD decompression error: %s", ZSTD_getErrorName(res));
        res = -1;

    }

    if(dctx)
        ZSTD_freeDCtx(dctx);

    return res;
    #else
        return -2;
    #endif
}

// Function pointer type defination
typedef AOCL_INT64  (*native_compress)(AOCL_CHAR *,  AOCL_UINTP, AOCL_CHAR *, AOCL_UINTP, AOCL_UINTP); 
typedef AOCL_INT64  (*native_decompress)(AOCL_CHAR *,  AOCL_UINTP, AOCL_CHAR *, AOCL_UINTP); 

/* Perform the native compression through a function pointer */
AOCL_INT64 native_run_compress(aocl_compression_desc* aocl_codec_handle, native_compress compress)
{
    #ifdef WIN32
        timer clkTick;
    #endif
        timeVal startTime, endTime;

    initTimer(clkTick);
    getTime(startTime);
    AOCL_INT64 resultComp = compress(aocl_codec_handle->inBuf, aocl_codec_handle->inSize, 
        aocl_codec_handle->outBuf, aocl_codec_handle->outSize, aocl_codec_handle->level);
    getTime(endTime);
    aocl_codec_handle->cTime = diffTime(clkTick, startTime, endTime);
    return resultComp;
}

/* Perform the native decompression through a function pointer */
AOCL_INT64 native_run_decompress(aocl_compression_desc* aocl_codec_handle, native_decompress decompress)
{
    #ifdef WIN32
        timer clkTick;
    #endif
        timeVal startTime, endTime;

    initTimer(clkTick);
    getTime(startTime);
    AOCL_INT64 resultDecomp = decompress(aocl_codec_handle->inBuf, aocl_codec_handle->inSize, 
        aocl_codec_handle->outBuf, aocl_codec_handle->outSize);
    getTime(endTime);
    aocl_codec_handle->dTime = diffTime(clkTick, startTime, endTime);
    return resultDecomp;
}

/* Run for a particular codec and level */
AOCL_INTP native_bench_codec_run(aocl_compression_desc* aocl_codec_handle,
    aocl_codec_bench_info* codec_bench_handle,
    aocl_compression_type codec, AOCL_INTP level) 
{
    AOCL_INTP status = 0;
    AOCL_UINTP inSize, file_size;
    FILE* inFp = codec_bench_handle->fp;
    AOCL_INT64 resultComp = 0;
    AOCL_INT64 resultDecomp = 0;

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    codec_bench_handle->cTime = 0;
    codec_bench_handle->cSize = 0;
    codec_bench_handle->dTime = 0;
    codec_bench_handle->dSize = 0;
    codec_bench_handle->cBestTime = UINT64_MAX;
    codec_bench_handle->dBestTime = UINT64_MAX;
    aocl_codec_handle->level = level;
    
    for (AOCL_INTP k = 0; k < codec_bench_handle->iterations; k++)
    {
        AOCL_UINT64 temp_cTime = 0;
        AOCL_UINT64 temp_dTime = 0;
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT /* Compress and decompress */ ||
            codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS /* Compress only */) 
        {
            while (inSize)
            {
                inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp); // read data in blocks of inSize

                // compress
                aocl_codec_handle->inSize = inSize;
                aocl_codec_handle->outSize = codec_bench_handle->outSize;
                aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
                aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
                switch (codec)
                {   
                    case LZ4:
                        resultComp = native_run_compress(aocl_codec_handle, native_lz4_compress);
                        break;
                    case LZ4HC:
                        resultComp = native_run_compress(aocl_codec_handle, native_lz4hc_compress);
                        break;
                    case SNAPPY:
                        resultComp = native_run_compress(aocl_codec_handle, native_snappy_compress);
                        break;
                    case ZLIB:
                        resultComp = native_run_compress(aocl_codec_handle, native_zlib_compress);
                        break;
                    case BZIP2:
                        resultComp = native_run_compress(aocl_codec_handle, native_bzip2_compress);
                        break;
                    case LZMA:
                        resultComp = native_run_compress(aocl_codec_handle, native_lzma_compress);
                        break;
                     case ZSTD:
                        resultComp = native_run_compress(aocl_codec_handle, native_zstd_compress);
                        break;
                    default:
                        return -2;
                }
                if (resultComp <= 0)
                {   
                    if (resultComp == -2)
                    {
                        printf("COMPRESSION Native API [%s-%td] [Filename:%s] failed. The library is not built with Method %s\n",
                            codec_list[codec].codec_name,
                            level, codec_bench_handle->fName, codec_list[codec].codec_name);   
                    }
                    else
                        printf("COMPRESSION Native API [%s-%td] [Filename:%s] Compression: failed\n",
                            codec_list[codec].codec_name,
                            level, codec_bench_handle->fName);
                    status = -1;
                    break;
                }
                aocl_codec_handle->cSize = resultComp;
                aocl_codec_handle->cSpeed = (aocl_codec_handle->inSize * 1000.0) /
                                    aocl_codec_handle->cTime;
                if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT) 
                {
                    // decompress
                    // use file compressed above as input
                    aocl_codec_handle->inSize = resultComp;
                    aocl_codec_handle->outSize = inSize;
                    aocl_codec_handle->inBuf = codec_bench_handle->outPtr;
                    aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
                    switch (codec)
                    {   
                        case LZ4:
                            resultDecomp = native_run_decompress(aocl_codec_handle, native_lz4_decompress);
                            break;
                        case LZ4HC:
                            resultDecomp = native_run_decompress(aocl_codec_handle, native_lz4hc_decompress); 
                            break;
                        case SNAPPY:;
                            resultDecomp =  native_run_decompress(aocl_codec_handle, native_snappy_decompress);
                            break;
                        case ZLIB:
                            resultDecomp = native_run_decompress(aocl_codec_handle, native_zlib_decompress);
                            break;
                        case BZIP2:
                            resultDecomp = native_run_decompress(aocl_codec_handle, native_bzip2_decompress);
                            break;
                        case LZMA:
                            resultDecomp = native_run_decompress(aocl_codec_handle, native_lzma_decompress);
                            break;
                        case ZSTD:
                            resultDecomp = native_run_decompress(aocl_codec_handle, native_zstd_decompress);
                            break;
                        default:
                            return -2;
                    }
                    
                    if (resultDecomp <= 0)
                    {
                        printf("COMPRESSION Native API [%s-%td] [Filename:%s] Decompression: failed\n",
                            codec_list[codec].codec_name,
                            level, codec_bench_handle->fName);
                        status = -1;
                        break;
                    }
                    aocl_codec_handle->dSize = resultDecomp;
                    aocl_codec_handle->dSpeed = (aocl_codec_handle->dSize * 1000.0) /
                                        aocl_codec_handle->dTime;
                    if (codec_bench_handle->verify) // verification supported only if decompress is enabled
                    {
                        if (memcmp(codec_bench_handle->inPtr,
                            codec_bench_handle->decompPtr, inSize) != 0)
                        {
                            printf("COMPRESSION Native API [%s-%td] [Filename:%s] verification: failed\n",
                                codec_list[codec].codec_name,
                                level, codec_bench_handle->fName);
                            status = -1;
                            break;
                        }
                    }
                }

                if (codec_bench_handle->print_stats) 
                {
                    // compression stats
                    codec_bench_handle->cTime += aocl_codec_handle->cTime;
                    codec_bench_handle->cSize += aocl_codec_handle->cSize;
                    temp_cTime += aocl_codec_handle->cTime;

                    if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT) 
                    {
                        // decompression stats if both are enabled
                        codec_bench_handle->dTime += aocl_codec_handle->dTime;
                        codec_bench_handle->dSize += aocl_codec_handle->dSize;
                        temp_dTime += aocl_codec_handle->dTime;
                    }
                }
                file_size -= inSize;
                inSize = (file_size > inSize) ? inSize : file_size;
            }
            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->cBestTime = temp_cTime < codec_bench_handle->cBestTime ?
                    temp_cTime : codec_bench_handle->cBestTime;
                if(codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT)
                {
                    codec_bench_handle->dBestTime = temp_dTime < codec_bench_handle->dBestTime ?
                        temp_dTime : codec_bench_handle->dBestTime;
                }
            }
        }
        else 
        {   /* Decompress only */
            /* In this mode, 
            * inFp is the input compressed data.
            * valFp is the decompressed data for validation.
            * Decompression is done in a single block. Hence,
            * max file size of decompressed data supported 
            * is MAX_MEM_SIZE_FOR_FILE_READ */
            FILE* valFp = codec_bench_handle->valFp;

            // load input compressed file
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            // decompress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
            switch (codec_bench_handle->codec_method)
            {   
                case LZ4:
                    resultDecomp = native_run_decompress(aocl_codec_handle, native_lz4_decompress);
                    break;
                case LZ4HC:
                    resultDecomp = native_run_decompress(aocl_codec_handle, native_lz4hc_decompress); 
                    break;
                case SNAPPY:;
                    resultDecomp =  native_run_decompress(aocl_codec_handle, native_snappy_decompress);
                    break;
                case ZLIB:
                    resultDecomp = native_run_decompress(aocl_codec_handle, native_zlib_decompress);
                    break;
                case BZIP2:
                    resultDecomp = native_run_decompress(aocl_codec_handle, native_bzip2_decompress);
                    break;
                case LZMA:
                    resultDecomp = native_run_decompress(aocl_codec_handle, native_lzma_decompress);
                    break;
                case ZSTD:
                    resultDecomp = native_run_decompress(aocl_codec_handle, native_zstd_decompress);
                    break;
                default:
                    return -1;
            }
            if (resultDecomp <= 0)
            {
                printf("COMPRESSION Native API [%s-%td] [Filename:%s] Decompression: failed\n",
                    codec_list[codec].codec_name,
                    level, codec_bench_handle->fName);
                status = -1;
                break;
            }

            if (codec_bench_handle->verify)
            {
                if (valFp == NULL) 
                {
                    printf("COMPRESSION Native API [%s-%td] [Filename:%s] verification file not provided\n",
                        codec_list[codec].codec_name,
                        level, codec_bench_handle->fName);
                    status = -1;
                    break;
                }
                // load decompressed file for validation
                codec_bench_handle->outSize = fread(codec_bench_handle->decompPtr,
                    1, codec_bench_handle->outSize, valFp);

                if (memcmp(codec_bench_handle->outPtr,
                    codec_bench_handle->decompPtr, codec_bench_handle->outSize) != 0)
                {
                    printf("COMPRESSION Native API [%s-%td] [Filename:%s] verification: failed\n",
                        codec_list[codec].codec_name,
                        level, codec_bench_handle->fName);
                    status = -1;
                    break;
                }
            }

            if (codec_bench_handle->print_stats)
            { // decompression stats
                codec_bench_handle->dTime += aocl_codec_handle->dTime;
                codec_bench_handle->dSize += aocl_codec_handle->dSize;
                temp_dTime += aocl_codec_handle->dTime;
            }

            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->dBestTime = temp_dTime < codec_bench_handle->dBestTime ?
                                                    temp_dTime : codec_bench_handle->dBestTime;
            }
        }

        rewind(inFp);
        if (status != 0)
            break;
    }

    if (status != 0)
    {
        LOG_FORMATTED(ERR, log_ctx,
            "Compression/Decompression/Verification operation failed for codec [%s].", codec_list[codec].codec_name);
        return status;
    }

    if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
        codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS) 
    {
        if (codec_bench_handle->verify)
        {
            printf("COMPRESSION Native API [%s-%td] [Filename:%s] verification: passed\n",
                codec_list[codec].codec_name,
                level, codec_bench_handle->fName);

        }
    }

    if (codec_bench_handle->print_stats)
    {
        printf("COMPRESSION Native API [%s-%td] [Filename:%s] -------------------------------------\n",
            codec_list[codec].codec_name,
            level, codec_bench_handle->fName);

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
            codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS) 
        {
            codec_bench_handle->cSpeed = (codec_bench_handle->file_size *
                codec_bench_handle->iterations * 1000.0) /
                codec_bench_handle->cTime;
            codec_bench_handle->cBestSpeed =
                (codec_bench_handle->file_size * 1000.0) /
                codec_bench_handle->cBestTime;
            printf("Compression:         speed(avg) %.2f MB/s, time(avg) %.2f ms, size %zu, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                codec_bench_handle->cSpeed,
                codec_bench_handle->cTime /
                (codec_bench_handle->iterations * 1000000.0),
                codec_bench_handle->cSize / codec_bench_handle->iterations,
                codec_bench_handle->cBestSpeed,
                codec_bench_handle->cBestTime / 1000000.0);
        }

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
            codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS) 
        {
            if(codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT)
            {
                codec_bench_handle->dSpeed = (codec_bench_handle->file_size *
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->dTime;
                codec_bench_handle->dBestSpeed = (codec_bench_handle->file_size * 1000.0) /
                    codec_bench_handle->dBestTime;
            }
            else
            {
                codec_bench_handle->dSpeed = (resultDecomp *
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->dTime;
                codec_bench_handle->dBestSpeed = (resultDecomp * 1000.0) /
                    codec_bench_handle->dBestTime;
            }
            printf("Decompression:       speed(avg) %.2f MB/s, time(avg) %.2f ms, size %zu, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                codec_bench_handle->dSpeed,
                codec_bench_handle->dTime /
                (codec_bench_handle->iterations * 1000000.0),
                codec_bench_handle->dSize / codec_bench_handle->iterations,
                codec_bench_handle->dBestSpeed,
                codec_bench_handle->dBestTime / 1000000.0);
        }

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
            codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS) 
        {
            printf("Ratio:               %.2f\n",
                (((codec_bench_handle->cSize * 100.0) /
                    codec_bench_handle->iterations) /
                    codec_bench_handle->file_size));
        }
    }

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
    return status;
}

AOCL_INTP native_api_bench_run(aocl_compression_desc *aocl_codec_handle,
                   aocl_codec_bench_info *codec_bench_handle)
{
    AOCL_INTP retStatus = 0;

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    aocl_codec_handle->memLimit = codec_bench_handle->mem_limit;

    if (codec_bench_handle->print_stats == 0 &&
        codec_bench_handle->verify == 1)
        codec_bench_handle->iterations = 1;

    if (codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS
        && codec_bench_handle->codec_method < 0) 
    {
        /* data decompressed using a certain codec can only be decompressed
         * by that particular codec. Run-all-codecs mode is not supported in this case */
        LOG_UNFORMATTED(ERR, log_ctx,
            "Codec not specified. Specify codec using -e when running -rdecompress mode.");
        printf("Codec not specified. Specify codec using -e when running -rdecompress mode.\n\n");
        return -2;
    }

    if (codec_bench_handle->codec_method < 0) // run for multiple codecs
    {
        for (aocl_compression_type codec = LZ4; codec < AOCL_COMPRESSOR_ALGOS_NUM; codec++)
        {
            aocl_codec_handle->optVar = codec_list[codec].extra_param;
            for (AOCL_INTP level = codec_list[codec].lower_level; level <= codec_list[codec].upper_level; level++)
            {
                AOCL_INTP status = native_bench_codec_run(aocl_codec_handle, codec_bench_handle, 
                    codec, level);
                if (status < 0) 
                {
                    retStatus = status;
                    if (status == -2)
                        break;
                }
            }
            
        }
    }
    else
    {
        AOCL_INTP def_level = aocl_codec_handle->level;
        AOCL_INTP lower_level, upper_level;

        if (def_level == UNINIT_LEVEL)
        {
            lower_level = 
                codec_list[codec_bench_handle->codec_method].lower_level;
            upper_level = 
                codec_list[codec_bench_handle->codec_method].upper_level;
        }
        else
        {
            lower_level = upper_level = def_level;
        }
        if (aocl_codec_handle->optVar == UNINIT_OPT_VAR)
            aocl_codec_handle->optVar = 
                codec_list[codec_bench_handle->codec_method].extra_param;

        if (codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS) 
        {
            /* In decompress only mode, given input file is decompressed.
             * Level information is not used. Run only once. */
            AOCL_INTP status = native_bench_codec_run(aocl_codec_handle, codec_bench_handle,
                codec_bench_handle->codec_method, def_level);
            if (status < 0) 
            {
                retStatus = status;
                return retStatus;
            }
        }
        else 
        {
            for (AOCL_INTP level = lower_level; level <= upper_level; level++) // run for requested levels
            {
                AOCL_INTP status = native_bench_codec_run(aocl_codec_handle, codec_bench_handle,
                    codec_bench_handle->codec_method, level);
                if (status < 0) 
                {
                    retStatus = status;
                    if (status == -2)
                        break;
                }
            }
        }
    }
    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return retStatus;
}
