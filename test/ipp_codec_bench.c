/**
 * Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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
 
 /** @file ipp_codec_bench.c
 *
 *  @brief Test bench application to benchmark IPP library compression methods.
 *
 *  This file contains the functions to load, setup, call and benchmark the
 *  compression and decompression methods supported by IPP compression library.
 *
 *  @author S. Biplab Raut
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
#include "algos/zlib/zlib.h"

#ifdef AOCL_ENABLE_LOG_FEATURE
#define LOG_LIBRARY_OPENED(algo, log_ctx, fn_name, lib_name)\
            if (log_ctx.maxLevel)\
            {\
                Dl_info dl_info;\
                dladdr(dlsym(hDL[algo], fn_name), &dl_info);\
                LOG_FORMATTED(INFO, log_ctx,\
                "Library [%s] opened from path: [%s]" , lib_name, dl_info.dli_fname);\
            }
#else
#define LOG_LIBRARY_OPENED(algo, log_ctx, fn_name, lib_name)
#endif /* AOCL_ENABLE_LOG_FEATURE */

//Handle to IPP dynamic library
AOCL_VOID *hDL[AOCL_COMPRESSOR_ALGOS_NUM] = { NULL };

//Internal wrapper function declarations that make calls to IPP library APIs
AOCL_INT64 ipp_lz4_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
            AOCL_INTP *verifyRes);
AOCL_INT64 ipp_lz4hc_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
            AOCL_INTP *verifyRes);
AOCL_INT64 ipp_zlib_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
            AOCL_INTP *verifyRes);
AOCL_INT64 ipp_bzip2_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
            AOCL_INTP *verifyRes);

AOCL_INTP ipp_setup(aocl_codec_bench_info *codec_bench_handle,
              aocl_compression_desc *aocl_codec_handle)
{ 
    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");
    AOCL_CHAR ippDir[MAX_FILENAME_LEN] = {0};
    AOCL_UINT32 ippDirLen = strlen(codec_bench_handle->ippDir);
    if (ippDirLen > (MAX_FILENAME_LEN - 1)) { // cant fit name + '\0' in ippDir[]
        LOG_FORMATTED(ERR, log_ctx,
            "ippDir name longer than %d characters", (int)(MAX_FILENAME_LEN));
        return ERR_CODEC_BENCH_ARGS;
    }
    else if ((ippDirLen == (MAX_FILENAME_LEN - 1)) && ippDir[ippDirLen - 1] != '/') { // cant fit name + '/' + '\0' in ippDir[]
        LOG_FORMATTED(ERR, log_ctx,
            "ippDir name with trailing slash longer than %d characters", (int)(MAX_FILENAME_LEN));
        return ERR_CODEC_BENCH_ARGS;
    }

    strncpy(ippDir, codec_bench_handle->ippDir, MAX_FILENAME_LEN);

    if(!(ippDirLen == 0 || (ippDirLen < (MAX_FILENAME_LEN-1) && ippDir[ippDirLen-1] == '/' ))) // add trailing '/' to dir name if missing
    {
        ippDir[ippDirLen] = '/';
        ippDir[ippDirLen+1] = '\0';
    }

    switch (codec_bench_handle->codec_method)
    {
        case LZ4:
        case LZ4HC:
            {
                AOCL_CHAR ippFullPath[MAX_FILENAME_LEN+10] = {0};
                strcpy(ippFullPath, ippDir);
                strcat(ippFullPath, "liblz4.so");
                hDL[LZ4] = hDL[LZ4HC] =
			        dlopen(ippFullPath, RTLD_NOW | RTLD_DEEPBIND);
            }
            if (hDL[LZ4] == NULL)
            {
                LOG_FORMATTED(ERR, log_ctx,
                "Error in opening dynamic library [liblz4.so]:[%s]",
                dlerror());
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_METHOD;
            }
#ifdef _GNU_SOURCE
            LOG_LIBRARY_OPENED(LZ4, log_ctx, "LZ4_decompress_safe", "liblz4.so")
#endif
            break;
        case ZLIB:
            {
                AOCL_CHAR ippFullPath[MAX_FILENAME_LEN+10] = {0};
                strcpy(ippFullPath, ippDir);
                strcat(ippFullPath, "libz.so");
                hDL[ZLIB] = dlopen(ippFullPath, RTLD_NOW | RTLD_DEEPBIND);
            }
            if (hDL[ZLIB] == NULL)
            {
                LOG_FORMATTED(ERR, log_ctx,
                "Error in opening dynamic library [libz.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_METHOD;
            }
#ifdef _GNU_SOURCE
            LOG_LIBRARY_OPENED(ZLIB, log_ctx, "uncompress", "libz.so")
#endif
            break;
        case BZIP2:
            {
                AOCL_CHAR ippFullPath[MAX_FILENAME_LEN+10] = {0};
                strcpy(ippFullPath, ippDir);
                strcat(ippFullPath, "libbz2.so");
                hDL[BZIP2] = dlopen(ippFullPath, RTLD_NOW | RTLD_DEEPBIND);
            }
            if (hDL[BZIP2] == NULL)
            {
                LOG_FORMATTED(ERR, log_ctx,
                "Error in opening dynamic library [libbz2.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_METHOD;
            }
#ifdef _GNU_SOURCE
            LOG_LIBRARY_OPENED(BZIP2, log_ctx, "BZ2_bzBuffToBuffDecompress", "libbz2.so")
#endif
            break;
        case LZMA:
        case SNAPPY:
        case ZSTD:
            LOG_UNFORMATTED(ERR, log_ctx,
            "Only supported compression methods are: LZ4, LZ4HC, ZLIB and BZIP2");
            LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
            return ERR_CODEC_BENCH_ARGS;
            break;
        case -1:
            {
                AOCL_CHAR ippFullPath[MAX_FILENAME_LEN+10] = {0};
                strcpy(ippFullPath, ippDir);
                strcat(ippFullPath, "liblz4.so");
                hDL[LZ4] = hDL[LZ4HC] = 
                    dlopen(ippFullPath, RTLD_NOW | RTLD_DEEPBIND);
            }
            if (hDL[LZ4] == NULL)
            {
                LOG_FORMATTED(ERR, log_ctx,
                "Error in opening dynamic library [liblz4.so]:[%s]",
                dlerror());
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_METHOD;
            }
#ifdef _GNU_SOURCE
            LOG_LIBRARY_OPENED(LZ4, log_ctx, "LZ4_decompress_safe", "liblz4.so")
#endif
            {
                AOCL_CHAR ippFullPath[MAX_FILENAME_LEN+10] = {0};
                strcpy(ippFullPath, ippDir);
                strcat(ippFullPath, "libz.so");
                hDL[ZLIB] = dlopen(ippFullPath, RTLD_NOW | RTLD_DEEPBIND);
            }
            if (hDL[ZLIB] == NULL)
            {
                LOG_FORMATTED(ERR, log_ctx,
                "Error in opening dynamic library [libz.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_METHOD;
            }
#ifdef _GNU_SOURCE
            LOG_LIBRARY_OPENED(ZLIB, log_ctx, "uncompress", "libz.so")
#endif
            {
                AOCL_CHAR ippFullPath[MAX_FILENAME_LEN+10] = {0};
                strcpy(ippFullPath, ippDir);
                strcat(ippFullPath, "libbz2.so");
                hDL[BZIP2] = dlopen(ippFullPath, RTLD_NOW | RTLD_DEEPBIND);
            }
            if (hDL[BZIP2] == NULL)
            {
                LOG_FORMATTED(ERR, log_ctx,
                "Error in opening dynamic library [libbz2.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_METHOD;
            }
#ifdef _GNU_SOURCE
            LOG_LIBRARY_OPENED(BZIP2, log_ctx, "BZ2_bzBuffToBuffDecompress", "libbz2.so")
#endif
            break;
        default:
            LOG_UNFORMATTED(ERR, log_ctx,
            "Only supported compression methods are: LZ4, LZ4HC, ZLIB and BZIP2");
            LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
            return ERR_CODEC_BENCH_ARGS;
            break;
    }
    
    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
    return 0;
}


AOCL_INT64 ipp_lz4_run(aocl_codec_bench_info *codec_bench_handle,
                  aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
                  AOCL_INTP *verifyRes)
{
    AOCL_INT64 resultComp = 0;
    AOCL_INT64 resultDecomp = 0;
    AOCL_UINTP inSize, file_size;
    AOCL_INTP k;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    AOCL_INT64 (*fCompDL) (AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP, AOCL_UINTP);
    AOCL_INT64 (*fDecompDL) (AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP, AOCL_UINTP);

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    fCompDL = (AOCL_INT64 (*)(AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP,  AOCL_UINTP))(uintptr_t)dlsym(hDL, "LZ4_compress_default");
    fDecompDL = (AOCL_INT64 (*)(AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP,  AOCL_UINTP))(uintptr_t)dlsym(hDL, "LZ4_decompress_safe");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, log_ctx,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return ERR_CODEC_BENCH_METHOD;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {
        AOCL_UINT64 temp_cBestTime = 0;
        AOCL_UINT64 temp_dBestTime = 0;
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
            getTime(startTime);
            resultComp = fCompDL(aocl_codec_handle->inBuf,
                                 aocl_codec_handle->outBuf,
                                 aocl_codec_handle->inSize,
                                 aocl_codec_handle->outSize);
            getTime(endTime);
            aocl_codec_handle->cSize = resultComp;
            aocl_codec_handle->cTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->cSpeed = (aocl_codec_handle->inSize * 1000.0) /
                                        aocl_codec_handle->cTime;

            if (resultComp <= 0)
            {
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->outPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
            getTime(startTime);
            resultDecomp = fDecompDL(aocl_codec_handle->inBuf,
                                     aocl_codec_handle->outBuf,
                                     aocl_codec_handle->inSize,
                                     aocl_codec_handle->outSize);
            getTime(endTime);
            aocl_codec_handle->dSize = resultDecomp;
            aocl_codec_handle->dTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->dSpeed = (aocl_codec_handle->dSize * 1000.0) /
                                        aocl_codec_handle->dTime;

            if (resultDecomp <= 0)
            {
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                    return ERR_CODEC_BENCH_FILE_IO;
                }
            }
            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->cTime +=
                    aocl_codec_handle->cTime;
                codec_bench_handle->cSize +=
                    aocl_codec_handle->cSize;
                codec_bench_handle->dTime +=
                    aocl_codec_handle->dTime;
                codec_bench_handle->dSize +=
                    aocl_codec_handle->dSize;
                
                temp_cBestTime += aocl_codec_handle->cTime;
                temp_dBestTime += aocl_codec_handle->dTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);

        if (codec_bench_handle->print_stats)
        {
            codec_bench_handle->cBestTime = temp_cBestTime < codec_bench_handle->cBestTime ?
                temp_cBestTime : codec_bench_handle->cBestTime;
            codec_bench_handle->dBestTime = temp_dBestTime < codec_bench_handle->dBestTime ?
                temp_dBestTime : codec_bench_handle->dBestTime;
        }
    }

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return 0;
}

AOCL_INT64 ipp_lz4hc_run(aocl_codec_bench_info *codec_bench_handle,
                    aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
                    AOCL_INTP *verifyRes)
{
    AOCL_INT64 resultComp = 0;
    AOCL_INT64 resultDecomp = 0;
    AOCL_UINTP inSize, file_size;
    AOCL_INTP k;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    AOCL_INT64 (*fCompDL) (AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP, AOCL_UINTP, AOCL_UINTP);
    AOCL_INT64 (*fDecompDL) (AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP, AOCL_UINTP);

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    fCompDL = (AOCL_INT64 (*)(AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP,  AOCL_UINTP,  AOCL_UINTP))(uintptr_t)dlsym(hDL, "LZ4_compress_HC");
    fDecompDL = (AOCL_INT64 (*)(AOCL_CHAR *, AOCL_CHAR *, AOCL_UINTP,  AOCL_UINTP))(uintptr_t)dlsym(hDL, "LZ4_decompress_safe");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, log_ctx,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return ERR_CODEC_BENCH_METHOD;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {
        AOCL_UINT64 temp_cBestTime = 0;
        AOCL_UINT64 temp_dBestTime = 0;
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
            getTime(startTime);
            resultComp = fCompDL(aocl_codec_handle->inBuf,
                                 aocl_codec_handle->outBuf,
                                 aocl_codec_handle->inSize,
                                 aocl_codec_handle->outSize,
                                 aocl_codec_handle->level);
            getTime(endTime);
            aocl_codec_handle->cSize = resultComp;
            aocl_codec_handle->cTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->cSpeed = (aocl_codec_handle->inSize * 1000.0) /
                                        aocl_codec_handle->cTime;

            if (resultComp <= 0)
            {
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->outPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
            getTime(startTime);
            resultDecomp = fDecompDL(aocl_codec_handle->inBuf,
                                     aocl_codec_handle->outBuf,
                                     aocl_codec_handle->inSize,
                                     aocl_codec_handle->outSize);
            getTime(endTime);
            aocl_codec_handle->dSize = resultDecomp;
            aocl_codec_handle->dTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->dSpeed = (aocl_codec_handle->dSize * 1000.0) /
                                        aocl_codec_handle->dTime;

            if (resultDecomp <= 0)
            {
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                    return ERR_CODEC_BENCH_FILE_IO;
                }
            }
            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->cTime +=
                    aocl_codec_handle->cTime;
                codec_bench_handle->cSize +=
                    aocl_codec_handle->cSize;
                codec_bench_handle->dTime +=
                    aocl_codec_handle->dTime;
                codec_bench_handle->dSize +=
                    aocl_codec_handle->dSize;
                
                temp_cBestTime += aocl_codec_handle->cTime;
                temp_dBestTime += aocl_codec_handle->dTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);

        if (codec_bench_handle->print_stats)
        {
            codec_bench_handle->cBestTime = temp_cBestTime < codec_bench_handle->cBestTime ?
                temp_cBestTime : codec_bench_handle->cBestTime;
            codec_bench_handle->dBestTime = temp_dBestTime < codec_bench_handle->dBestTime ?
                temp_dBestTime : codec_bench_handle->dBestTime;
        }
    }

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return 0;
}

AOCL_INT64 ipp_zlib_run(aocl_codec_bench_info *codec_bench_handle,
                   aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
                   AOCL_INTP *verifyRes)
{
    AOCL_INT64 resultComp = 0;
    AOCL_INT64 resultDecomp = 0;
    AOCL_UINTP inSize, file_size;
    AOCL_INTP k, ret;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    AOCL_INT64 (*fCompDL) (AOCL_UINT8 *, uLongf *, AOCL_UINT8 *, AOCL_UINTP, AOCL_UINTP);
    AOCL_INT64 (*fDecompDL) (AOCL_UINT8 *, uLongf *, AOCL_UINT8 *, AOCL_UINTP);

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    fCompDL = (AOCL_INT64 (*)(AOCL_UINT8 *, uLongf *, AOCL_UINT8 *, AOCL_UINTP,  AOCL_UINTP))(uintptr_t)dlsym(hDL, "compress2");
    fDecompDL = (AOCL_INT64 (*)(AOCL_UINT8 *, uLongf *, AOCL_UINT8 *, AOCL_UINTP))(uintptr_t)dlsym(hDL, "uncompress");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, log_ctx,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return ERR_CODEC_BENCH_METHOD;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {    
        AOCL_UINT64 temp_cBestTime = 0;
        AOCL_UINT64 temp_dBestTime = 0;
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
            getTime(startTime);
            ret = fCompDL((AOCL_UINT8 *)aocl_codec_handle->outBuf,
                          (uLongf *)&aocl_codec_handle->outSize,
                          (AOCL_UINT8 *)aocl_codec_handle->inBuf,
                          (AOCL_UINTP)aocl_codec_handle->inSize,
                          (AOCL_UINTP)aocl_codec_handle->level);
            getTime(endTime);
            aocl_codec_handle->cSize = resultComp = aocl_codec_handle->outSize;
            aocl_codec_handle->cTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->cSpeed = (aocl_codec_handle->inSize * 1000.0) /
                                        aocl_codec_handle->cTime;

            if (ret != Z_OK)
            {
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->outPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
            getTime(startTime);
            ret = fDecompDL((AOCL_UINT8 *)aocl_codec_handle->outBuf,
                            (uLongf *)&aocl_codec_handle->outSize,
                            (AOCL_UINT8 *)aocl_codec_handle->inBuf,
                            aocl_codec_handle->inSize);
            getTime(endTime);
            aocl_codec_handle->dSize = resultDecomp = aocl_codec_handle->outSize;
            aocl_codec_handle->dTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->dSpeed = (aocl_codec_handle->dSize * 1000.0) /
                                        aocl_codec_handle->dTime;

            if (ret != Z_OK)
                return ERR_CODEC_BENCH_ARGS;

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                    return ERR_CODEC_BENCH_FILE_IO;
                }
            }
            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->cTime +=
                    aocl_codec_handle->cTime;
                codec_bench_handle->cSize +=
                    aocl_codec_handle->cSize;
                codec_bench_handle->dTime +=
                    aocl_codec_handle->dTime;
                codec_bench_handle->dSize +=
                    aocl_codec_handle->dSize;
                
                temp_cBestTime += aocl_codec_handle->cTime;
                temp_dBestTime += aocl_codec_handle->dTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);

        if (codec_bench_handle->print_stats)
        {
            codec_bench_handle->cBestTime = temp_cBestTime < codec_bench_handle->cBestTime ?
                temp_cBestTime : codec_bench_handle->cBestTime;
            codec_bench_handle->dBestTime = temp_dBestTime < codec_bench_handle->dBestTime ?
                temp_dBestTime : codec_bench_handle->dBestTime;
        }
    }

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return 0;
}

AOCL_INT64 ipp_bzip2_run(aocl_codec_bench_info *codec_bench_handle,
                   aocl_compression_desc *aocl_codec_handle, AOCL_VOID *hDL,
                   AOCL_INTP *verifyRes)
{
    AOCL_INT64 resultComp = 0;
    AOCL_INT64 resultDecomp = 0;
    AOCL_UINTP inSize, file_size;
    AOCL_INTP k, ret;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    AOCL_INT64 (*fCompDL) (AOCL_CHAR *, AOCL_UINT32 *, AOCL_CHAR *, AOCL_UINT32, AOCL_UINT32, AOCL_UINT32, AOCL_UINT32);
    AOCL_INT64 (*fDecompDL) (AOCL_CHAR *, AOCL_UINT32 *, AOCL_CHAR *, AOCL_UINT32, AOCL_UINT32, AOCL_UINT32);

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    fCompDL = (AOCL_INT64 (*)(AOCL_CHAR *, AOCL_UINT32 *, AOCL_CHAR *, AOCL_UINT32,  AOCL_UINT32,  AOCL_UINT32,  AOCL_UINT32))(uintptr_t)dlsym(hDL, "BZ2_bzBuffToBuffCompress");
    fDecompDL = (AOCL_INT64 (*)(AOCL_CHAR *, AOCL_UINT32 *, AOCL_CHAR *, AOCL_UINT32,  AOCL_UINT32,  AOCL_UINT32))(uintptr_t)dlsym(hDL, "BZ2_bzBuffToBuffDecompress");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, log_ctx,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return ERR_CODEC_BENCH_METHOD;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {
        AOCL_UINT64 temp_cBestTime = 0;
        AOCL_UINT64 temp_dBestTime = 0;
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
            getTime(startTime);
            ret = fCompDL((AOCL_CHAR *)aocl_codec_handle->outBuf,
                          (AOCL_UINT32 *)&aocl_codec_handle->outSize,
                          (AOCL_CHAR *)aocl_codec_handle->inBuf,
                          (AOCL_UINT32)aocl_codec_handle->inSize,
                          (AOCL_INT32)aocl_codec_handle->level, 0, 0);
            getTime(endTime);
            aocl_codec_handle->cSize = resultComp = aocl_codec_handle->outSize;
            aocl_codec_handle->cTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->cSpeed = (aocl_codec_handle->inSize * 1000.0) /
                                        aocl_codec_handle->cTime;

            if (ret != Z_OK)
            {
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->outPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
            getTime(startTime);
            ret = fDecompDL((AOCL_CHAR *)aocl_codec_handle->outBuf,
                            (AOCL_UINT32 *)&aocl_codec_handle->outSize,
                            (AOCL_CHAR *)aocl_codec_handle->inBuf,
                            (AOCL_UINT32)aocl_codec_handle->inSize, 0, 0);
            getTime(endTime);
            aocl_codec_handle->dSize = resultDecomp = aocl_codec_handle->outSize;
            aocl_codec_handle->dTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->dSpeed = (aocl_codec_handle->dSize * 1000.0) /
                                        aocl_codec_handle->dTime;

            if (ret != Z_OK)
            {
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                    return ERR_CODEC_BENCH_FILE_IO;
                }
            }
            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->cTime +=
                    aocl_codec_handle->cTime;
                codec_bench_handle->cSize +=
                    aocl_codec_handle->cSize;
                codec_bench_handle->dTime +=
                    aocl_codec_handle->dTime;
                codec_bench_handle->dSize +=
                    aocl_codec_handle->dSize;

                temp_cBestTime += aocl_codec_handle->cTime;
                temp_dBestTime += aocl_codec_handle->dTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);

        if (codec_bench_handle->print_stats)
        {
            codec_bench_handle->cBestTime = temp_cBestTime < codec_bench_handle->cBestTime ?
                temp_cBestTime : codec_bench_handle->cBestTime;
            codec_bench_handle->dBestTime = temp_dBestTime < codec_bench_handle->dBestTime ?
                temp_dBestTime : codec_bench_handle->dBestTime;
        }
    }

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return 0;
}

AOCL_INT64 ipp_run(aocl_codec_bench_info *codec_bench_handle,
              aocl_compression_desc *aocl_codec_handle)
{
    aocl_compression_type i; 
    AOCL_INTP l, verifyRes, ret;

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    aocl_codec_handle->memLimit = codec_bench_handle->mem_limit;

    if (codec_bench_handle->print_stats == 0 &&
        codec_bench_handle->verify == 1)
        codec_bench_handle->iterations = 1;


    if (codec_bench_handle->codec_method < 0)
    {
        for (i = LZ4; i < AOCL_COMPRESSOR_ALGOS_NUM; i++)
        {
            if (i == LZMA || i == BZIP2 || i == SNAPPY || i == ZSTD)
                continue;

            aocl_codec_handle->optVar = codec_list[i].extra_param;

            for (l = codec_list[i].lower_level;
                 l <= codec_list[i].upper_level; l++)
            {
                codec_bench_handle->cTime = 0;
                codec_bench_handle->cSize = 0;
                codec_bench_handle->dTime = 0;
                codec_bench_handle->dSize = 0;
                codec_bench_handle->cBestTime = UINT64_MAX;
                codec_bench_handle->dBestTime = UINT64_MAX;
                aocl_codec_handle->level = l;

                switch (i)
                {
                    case LZ4:
                        ret = ipp_lz4_run(codec_bench_handle,
                                        aocl_codec_handle, hDL[i],
                                        &verifyRes);
                        break;
                    case LZ4HC:
                        ret = ipp_lz4hc_run(codec_bench_handle,
                                            aocl_codec_handle, hDL[i],
                                            &verifyRes);
                        break;
                    case ZLIB:
                        ret = ipp_zlib_run(codec_bench_handle,
                                        aocl_codec_handle, hDL[i],
                                        &verifyRes);
                        break;
                    case BZIP2:
                        ret = ipp_bzip2_run(codec_bench_handle,
                                        aocl_codec_handle, hDL[i],
                                        &verifyRes);
                        break;
                    default:
                        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                        return ERR_CODEC_BENCH_METHOD;

                }
                
                if (ret != 0 && ret != -3)
                {
                    LOG_UNFORMATTED(ERR, log_ctx,
                        "Error in executing the dynamic library");
                    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                    return ERR_CODEC_BENCH_ARGS;
                }

                if (codec_bench_handle->verify)
                {
                    if (verifyRes != 0)
                    {
                        printf("IPP [%s-%ld] [Filename:%s] verification: failed\n",
                               codec_list[i].codec_name,
                               l, codec_bench_handle->fName);
                        return ERR_CODEC_BENCH_ARGS;
                    }
                    else
                    {
                        printf("IPP [%s-%ld] [Filename:%s] verification: passed\n",
                               codec_list[i].codec_name,
                               l, codec_bench_handle->fName);
                    }
                }
                if (codec_bench_handle->print_stats)
                {
                    codec_bench_handle->cSpeed = (codec_bench_handle->file_size * 
                        codec_bench_handle->iterations * 1000.0) / 
                        codec_bench_handle->cTime;
                    codec_bench_handle->dSpeed = (codec_bench_handle->file_size *
                        codec_bench_handle->iterations * 1000.0) /
                        codec_bench_handle->dTime;
                    codec_bench_handle->cBestSpeed = 
                        (codec_bench_handle->file_size * 1000.0) / 
                        codec_bench_handle->cBestTime;
                    codec_bench_handle->dBestSpeed = 
                        (codec_bench_handle->file_size * 1000.0) / 
                        codec_bench_handle->dBestTime;
                    printf("IPP [%s-%ld] [Filename:%s] -------------------------------------\n",
                           codec_list[i].codec_name,
                           l, codec_bench_handle->fName);
                    printf("Compression:         speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                           codec_bench_handle->cSpeed,
                           codec_bench_handle->cTime/
                           (codec_bench_handle->iterations * 1000000.0),
                           codec_bench_handle->cSize/codec_bench_handle->iterations,
                           codec_bench_handle->cBestSpeed,
                           codec_bench_handle->cBestTime/1000000.0);
                    printf("Decompression:       speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n", 
                           codec_bench_handle->dSpeed,
                           codec_bench_handle->dTime/
                           (codec_bench_handle->iterations * 1000000.0),
                           codec_bench_handle->dSize/codec_bench_handle->iterations,
                           codec_bench_handle->dBestSpeed,
                           codec_bench_handle->dBestTime/1000000.0);
                    printf("Ratio:               %.2f\n",
                           (((codec_bench_handle->cSize*100.0)/
                           codec_bench_handle->iterations)/
                           codec_bench_handle->file_size));
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
                
        for (l = lower_level; l <= upper_level; l++)
        {
            codec_bench_handle->cTime = 0;
            codec_bench_handle->cSize = 0;
            codec_bench_handle->dTime = 0;
            codec_bench_handle->dSize = 0;
            codec_bench_handle->cBestTime = UINT64_MAX;
            codec_bench_handle->dBestTime = UINT64_MAX;
            aocl_codec_handle->level = l;

            switch (codec_bench_handle->codec_method)
            {
                case LZ4:
                    ret = ipp_lz4_run(codec_bench_handle,
                                    aocl_codec_handle,
                                    hDL[codec_bench_handle->codec_method],
                                    &verifyRes);
                    break;
                case LZ4HC:
                    ret = ipp_lz4hc_run(codec_bench_handle,
                                        aocl_codec_handle,
                                        hDL[codec_bench_handle->codec_method],
                                        &verifyRes);
                    break;
                case ZLIB:
                    ret = ipp_zlib_run(codec_bench_handle,
                                    aocl_codec_handle,
                                    hDL[codec_bench_handle->codec_method],
                                    &verifyRes);
                    break;
                case BZIP2:
                    ret = ipp_bzip2_run(codec_bench_handle,
                                    aocl_codec_handle, hDL[codec_bench_handle->codec_method],
                                    &verifyRes);
                    break;
                default:
                    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                    return ERR_CODEC_BENCH_METHOD;
            }

            if (ret != 0 && ret != -3)
            {
                LOG_UNFORMATTED(ERR, log_ctx,
                    "Error in executing the dynamic library");
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return ERR_CODEC_BENCH_ARGS;
            }

            if (codec_bench_handle->verify)
            {
                if (verifyRes != 0)
                {
                    printf("IPP [%s-%ld] [Filename:%s] verification: failed\n",
                            codec_list[codec_bench_handle->codec_method].codec_name,
                            aocl_codec_handle->level,
                            codec_bench_handle->fName);
                    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                    return ERR_CODEC_BENCH_ARGS;
                }
                else
                {
                    printf("IPP [%s-%ld] [Filename:%s] verification: passed\n", 
                            codec_list[codec_bench_handle->codec_method].codec_name,
                            aocl_codec_handle->level,
                            codec_bench_handle->fName);
                }
			    
            }
            if (codec_bench_handle->print_stats)
            {        
                codec_bench_handle->cSpeed = (codec_bench_handle->file_size * 
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->cTime;
                codec_bench_handle->dSpeed = (codec_bench_handle->file_size * 
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->dTime;
                codec_bench_handle->cBestSpeed = (codec_bench_handle->file_size * 
                    1000.0) / codec_bench_handle->cBestTime;
                codec_bench_handle->dBestSpeed = (codec_bench_handle->file_size * 
                    1000.0) / codec_bench_handle->dBestTime;
                printf("IPP [%s-%ld] [Filename:%s] -------------------------------------\n",
                        codec_list[codec_bench_handle->codec_method].codec_name,
                        aocl_codec_handle->level, codec_bench_handle->fName);
                printf("Compression:         speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                        codec_bench_handle->cSpeed,
                        codec_bench_handle->cTime/
                        (codec_bench_handle->iterations * 1000000.0),
                        codec_bench_handle->cSize/codec_bench_handle->iterations,
                        codec_bench_handle->cBestSpeed,
                        codec_bench_handle->cBestTime/1000000.0);
                printf("Decompression:       speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n", 
                        codec_bench_handle->dSpeed,
                        codec_bench_handle->dTime/
                        (codec_bench_handle->iterations * 1000000.0),
                        codec_bench_handle->dSize/codec_bench_handle->iterations,
                        codec_bench_handle->dBestSpeed,
                        codec_bench_handle->dBestTime/1000000.0);
                printf("Ratio:               %.2f\n",
                        (((codec_bench_handle->cSize*100.0)/
                        codec_bench_handle->iterations)/
                        codec_bench_handle->file_size));
            }
        }
    }
    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return 0;
}

AOCL_INTP ipp_destroy(aocl_compression_desc *aocl_codec_handle)
{
    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");
    for (aocl_compression_type i = LZ4; i < AOCL_COMPRESSOR_ALGOS_NUM; i++)
    {
        if (hDL[i])
            dlclose(hDL[i]);
    }
    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
    return 0;
}

AOCL_INTP ipp_bench_run(aocl_compression_desc *aocl_codec_handle,
                  aocl_codec_bench_info *codec_bench_handle)
{
    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    AOCL_INT64 ret = 0;
    
    ret = ipp_setup(codec_bench_handle, aocl_codec_handle);
    if (ret != 0)
        goto error_exit;
    
    ret = ipp_run(codec_bench_handle, aocl_codec_handle);
    if (ret != 0)
        goto error_exit;

    ipp_destroy(aocl_codec_handle);

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return 0;

error_exit:
    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
    return ret;
}

