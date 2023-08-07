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
#include "api/api.h"
#include "utils/utils.h"
#include "codec_bench.h"
#include "algos/zlib/zlib.h"

//Handle to IPP dynamic library
VOID *hDL[AOCL_COMPRESSOR_ALGOS_NUM] = { NULL };

//Internal wrapper function declarations that make calls to IPP library APIs
INT64 ipp_lz4_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, VOID *hDL,
            INTP *verifyRes);
INT64 ipp_lz4hc_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, VOID *hDL,
            INTP *verifyRes);
INT64 ipp_zlib_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, VOID *hDL,
            INTP *verifyRes);
INT64 ipp_bzip2_run(aocl_codec_bench_info *codec_bench_handle,
            aocl_compression_desc *aocl_codec_handle, VOID *hDL,
            INTP *verifyRes);

INTP ipp_setup(aocl_codec_bench_info *codec_bench_handle,
              aocl_compression_desc *aocl_codec_handle)
{ 
    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");
    
    switch (codec_bench_handle->codec_method)
    {
        case LZ4:
        case LZ4HC:
            hDL[LZ4] = hDL[LZ4HC] =
			    dlopen("liblz4.so", RTLD_NOW | RTLD_DEEPBIND);
            if (hDL[LZ4] == NULL)
            {
                LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in opening dynamic library [liblz4.so]:[%s]",
                dlerror());
                LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
                return -2;
            }
#ifdef _GNU_SOURCE
            if (aocl_codec_handle->printDebugLogs)
            {
                Dl_info dl_info;
                dladdr(dlsym(hDL[LZ4], "LZ4_decompress_safe"), &dl_info);
                LOG_FORMATTED(INFO, aocl_codec_handle->printDebugLogs,
                "Library [liblz4.so] opened from path: [%s]",
                dl_info.dli_fname);
            }
#endif
            break;
        case ZLIB:
            hDL[ZLIB] = dlopen("libz.so", RTLD_NOW | RTLD_DEEPBIND);
            if (hDL[ZLIB] == NULL)
            {
                LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in opening dynamic library [libz.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
                return -2;
            }
#ifdef _GNU_SOURCE
            if (aocl_codec_handle->printDebugLogs)
            {
                Dl_info dl_info;
                dladdr(dlsym(hDL[ZLIB], "uncompress"), &dl_info);
                LOG_FORMATTED(INFO, aocl_codec_handle->printDebugLogs,
                "Library [libz.so] opened from path: [%s]", dl_info.dli_fname);
            }
#endif
            break;
        case BZIP2:
            hDL[BZIP2] = dlopen("libbz2.so", RTLD_NOW | RTLD_DEEPBIND);
            if (hDL[BZIP2] == NULL)
            {
                LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in opening dynamic library [libbz2.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
                return -2;
            }
#ifdef _GNU_SOURCE
            if (aocl_codec_handle->printDebugLogs)
            {
                Dl_info dl_info;
                dladdr(dlsym(hDL[BZIP2], "BZ2_bzBuffToBuffDecompress"), &dl_info);
                LOG_FORMATTED(INFO, aocl_codec_handle->printDebugLogs,
                "Library [libbz2.so] opened from path: [%s]", dl_info.dli_fname);
            }
#endif
            break;
        case LZMA:
        case SNAPPY:
        case ZSTD:
            LOG_UNFORMATTED(ERR, aocl_codec_handle->printDebugLogs,
            "Only supported compression methods are: LZ4, LZ4HC, ZLIB and BZIP2");
            LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
            return -1;
            break;
        case -1:
            hDL[LZ4] = hDL[LZ4HC] = 
                dlopen("liblz4.so", RTLD_NOW | RTLD_DEEPBIND);
            if (hDL[LZ4] == NULL)
            {
                LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in opening dynamic library [liblz4.so]:[%s]",
                dlerror());
                LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
                return -2;
            }
#ifdef _GNU_SOURCE
            if (aocl_codec_handle->printDebugLogs)
            {
                Dl_info dl_info;
                dladdr(dlsym(hDL[LZ4], "LZ4_decompress_safe"), &dl_info);
                LOG_FORMATTED(INFO, aocl_codec_handle->printDebugLogs,
                "Library [liblz4.so] opened from path: [%s]",
                dl_info.dli_fname);
            }
#endif
            hDL[ZLIB] = dlopen("libz.so", RTLD_NOW | RTLD_DEEPBIND);
            if (hDL[ZLIB] == NULL)
            {
                LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in opening dynamic library [libz.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
                return -2;
            }
#ifdef _GNU_SOURCE
            if (aocl_codec_handle->printDebugLogs)
            {
                Dl_info dl_info;
                dladdr(dlsym(hDL[ZLIB], "uncompress"), &dl_info);
                LOG_FORMATTED(INFO, aocl_codec_handle->printDebugLogs,
                "Library [libz.so] opened from path: [%s]", dl_info.dli_fname);
            }
#endif
            hDL[BZIP2] = dlopen("libbz2.so", RTLD_NOW | RTLD_DEEPBIND);
            if (hDL[BZIP2] == NULL)
            {
                LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in opening dynamic library [libbz2.so]:[%s]", dlerror());
                LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
                return -2;
            }
#ifdef _GNU_SOURCE
            if (aocl_codec_handle->printDebugLogs)
            {
                Dl_info dl_info;
                dladdr(dlsym(hDL[BZIP2], "BZ2_bzBuffToBuffDecompress"), &dl_info);
                LOG_FORMATTED(INFO, aocl_codec_handle->printDebugLogs,
                "Library [libbz2.so] opened from path: [%s]", dl_info.dli_fname);
            }
#endif
            break;
        default:
            LOG_UNFORMATTED(ERR, aocl_codec_handle->printDebugLogs,
            "Only supported compression methods are: LZ4, LZ4HC, ZLIB and BZIP2");
            LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
            return -1;
            break;
    }
    
    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
    return 0;
}


INT64 ipp_lz4_run(aocl_codec_bench_info *codec_bench_handle,
                  aocl_compression_desc *aocl_codec_handle, VOID *hDL,
                  INTP *verifyRes)
{
    INT64 resultComp = 0;
    INT64 resultDecomp = 0;
    UINTP inSize, file_size;
    INTP k;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    INT64 (*fCompDL) (CHAR *, CHAR *, UINTP, UINTP);
    INT64 (*fDecompDL) (CHAR *, CHAR *, UINTP, UINTP);

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    fCompDL = (INT64 (*)(CHAR *, CHAR *, UINTP,  UINTP))(uintptr_t)dlsym(hDL, "LZ4_compress_default");
    fDecompDL = (INT64 (*)(CHAR *, CHAR *, UINTP,  UINTP))(uintptr_t)dlsym(hDL, "LZ4_decompress_safe");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        return -2;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->compPtr;
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
                return -1;

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->compPtr;
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
                return -1;

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    return -3;
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
                codec_bench_handle->cBestTime = 
                    (codec_bench_handle->cBestTime == 0) ? 
                    aocl_codec_handle->cTime :
                    (codec_bench_handle->cBestTime > 
                     aocl_codec_handle->cTime) ?
                    aocl_codec_handle->cTime : 
                    codec_bench_handle->cBestTime;
                codec_bench_handle->dBestTime = 
                    (codec_bench_handle->dBestTime == 0) ? 
                    aocl_codec_handle->dTime :
                    (codec_bench_handle->dBestTime > 
                     aocl_codec_handle->dTime) ?
                    aocl_codec_handle->dTime : 
                    codec_bench_handle->dBestTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);
    }

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    return 0;
}

INT64 ipp_lz4hc_run(aocl_codec_bench_info *codec_bench_handle,
                    aocl_compression_desc *aocl_codec_handle, VOID *hDL,
                    INTP *verifyRes)
{
    INT64 resultComp = 0;
    INT64 resultDecomp = 0;
    UINTP inSize, file_size;
    INTP k;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    INT64 (*fCompDL) (CHAR *, CHAR *, UINTP, UINTP, UINTP);
    INT64 (*fDecompDL) (CHAR *, CHAR *, UINTP, UINTP);

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    fCompDL = (INT64 (*)(CHAR *, CHAR *, UINTP,  UINTP,  UINTP))(uintptr_t)dlsym(hDL, "LZ4_compress_HC");
    fDecompDL = (INT64 (*)(CHAR *, CHAR *, UINTP,  UINTP))(uintptr_t)dlsym(hDL, "LZ4_decompress_safe");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        return -2;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->compPtr;
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
                return -1;

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->compPtr;
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
                return -1;

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    return -3;
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
                codec_bench_handle->cBestTime = 
                    (codec_bench_handle->cBestTime == 0) ? 
                    aocl_codec_handle->cTime :
                    (codec_bench_handle->cBestTime > 
                     aocl_codec_handle->cTime) ?
                    aocl_codec_handle->cTime : 
                    codec_bench_handle->cBestTime;
                codec_bench_handle->dBestTime = 
                    (codec_bench_handle->dBestTime == 0) ? 
                    aocl_codec_handle->dTime :
                    (codec_bench_handle->dBestTime > 
                     aocl_codec_handle->dTime) ?
                    aocl_codec_handle->dTime : 
                    codec_bench_handle->dBestTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);
    }

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    return 0;
}

INT64 ipp_zlib_run(aocl_codec_bench_info *codec_bench_handle,
                   aocl_compression_desc *aocl_codec_handle, VOID *hDL,
                   INTP *verifyRes)
{
    INT64 resultComp = 0;
    INT64 resultDecomp = 0;
    UINTP inSize, file_size;
    INTP k, ret;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    INT64 (*fCompDL) (UINT8 *, uLongf *, UINT8 *, UINTP, UINTP);
    INT64 (*fDecompDL) (UINT8 *, uLongf *, UINT8 *, UINTP);

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    fCompDL = (INT64 (*)(UINT8 *, uLongf *, UINT8 *, UINTP,  UINTP))(uintptr_t)dlsym(hDL, "compress2");
    fDecompDL = (INT64 (*)(UINT8 *, uLongf *, UINT8 *, UINTP))(uintptr_t)dlsym(hDL, "uncompress");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        return -2;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->compPtr;
            getTime(startTime);
            ret = fCompDL((UINT8 *)aocl_codec_handle->outBuf,
                          (uLongf *)&aocl_codec_handle->outSize,
                          (UINT8 *)aocl_codec_handle->inBuf,
                          (UINTP)aocl_codec_handle->inSize,
                          (UINTP)aocl_codec_handle->level);
            getTime(endTime);
            aocl_codec_handle->cSize = resultComp = aocl_codec_handle->outSize;
            aocl_codec_handle->cTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->cSpeed = (aocl_codec_handle->inSize * 1000.0) /
                                        aocl_codec_handle->cTime;

            if (ret != Z_OK)
                return -1;

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->compPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
            getTime(startTime);
            ret = fDecompDL((UINT8 *)aocl_codec_handle->outBuf,
                            (uLongf *)&aocl_codec_handle->outSize,
                            (UINT8 *)aocl_codec_handle->inBuf,
                            aocl_codec_handle->inSize);
            getTime(endTime);
            aocl_codec_handle->dSize = resultDecomp = aocl_codec_handle->outSize;
            aocl_codec_handle->dTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->dSpeed = (aocl_codec_handle->dSize * 1000.0) /
                                        aocl_codec_handle->dTime;

            if (ret != Z_OK)
                return -1;

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    return -3;
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
                codec_bench_handle->cBestTime = 
                    (codec_bench_handle->cBestTime == 0) ? 
                    aocl_codec_handle->cTime :
                    (codec_bench_handle->cBestTime > 
                     aocl_codec_handle->cTime) ?
                    aocl_codec_handle->cTime : 
                    codec_bench_handle->cBestTime;
                codec_bench_handle->dBestTime = 
                    (codec_bench_handle->dBestTime == 0) ? 
                    aocl_codec_handle->dTime :
                    (codec_bench_handle->dBestTime > 
                     aocl_codec_handle->dTime) ?
                    aocl_codec_handle->dTime : 
                    codec_bench_handle->dBestTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);
    }

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    return 0;
}

INT64 ipp_bzip2_run(aocl_codec_bench_info *codec_bench_handle,
                   aocl_compression_desc *aocl_codec_handle, VOID *hDL,
                   INTP *verifyRes)
{
    INT64 resultComp = 0;
    INT64 resultDecomp = 0;
    UINTP inSize, file_size;
    INTP k, ret;
#ifdef WINDOWS_
    timer clkTick;
#endif
    timeVal startTime, endTime;
    FILE *inFp = codec_bench_handle->fp;
    INT64 (*fCompDL) (CHAR *, UINT32 *, CHAR *, UINT32, UINT32, UINT32, UINT32);
    INT64 (*fDecompDL) (CHAR *, UINT32 *, CHAR *, UINT32, UINT32, UINT32);

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    fCompDL = (INT64 (*)(CHAR *, UINT32 *, CHAR *, UINT32,  UINT32,  UINT32,  UINT32))(uintptr_t)dlsym(hDL, "BZ2_bzBuffToBuffCompress");
    fDecompDL = (INT64 (*)(CHAR *, UINT32 *, CHAR *, UINT32,  UINT32,  UINT32))(uintptr_t)dlsym(hDL, "BZ2_bzBuffToBuffDecompress");
    if (fCompDL == NULL || fDecompDL == NULL)
    {
        LOG_FORMATTED(ERR, aocl_codec_handle->printDebugLogs,
        "Error in loading symbols from dynamic library: [%s]", dlerror());
        return -2;
    }
#ifdef WINDOWS_
    initTimer(clkTick);
#endif
    for (k = 0; k < codec_bench_handle->iterations; k++)
    {
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        while (inSize)
        {
            inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

            //compress
            aocl_codec_handle->inSize = inSize;
            aocl_codec_handle->outSize = codec_bench_handle->outSize;
            aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->compPtr;
            getTime(startTime);
            ret = fCompDL((CHAR *)aocl_codec_handle->outBuf,
                          (UINT32 *)&aocl_codec_handle->outSize,
                          (CHAR *)aocl_codec_handle->inBuf,
                          (UINT32)aocl_codec_handle->inSize,
                          (INT32)aocl_codec_handle->level, 0, 0);
            getTime(endTime);
            aocl_codec_handle->cSize = resultComp = aocl_codec_handle->outSize;
            aocl_codec_handle->cTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->cSpeed = (aocl_codec_handle->inSize * 1000.0) /
                                        aocl_codec_handle->cTime;

            if (ret != Z_OK)
                return -1;

            //decompress
            aocl_codec_handle->inSize = resultComp;
            aocl_codec_handle->outSize = inSize;
            aocl_codec_handle->inBuf = codec_bench_handle->compPtr;
            aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
            getTime(startTime);
            ret = fDecompDL((CHAR *)aocl_codec_handle->outBuf,
                            (UINT32 *)&aocl_codec_handle->outSize,
                            (CHAR *)aocl_codec_handle->inBuf,
                            (UINT32)aocl_codec_handle->inSize, 0, 0);
            getTime(endTime);
            aocl_codec_handle->dSize = resultDecomp = aocl_codec_handle->outSize;
            aocl_codec_handle->dTime = diffTime(clkTick, startTime, endTime);
            aocl_codec_handle->dSpeed = (aocl_codec_handle->dSize * 1000.0) /
                                        aocl_codec_handle->dTime;

            if (ret != Z_OK)
                return -1;

            if (codec_bench_handle->verify)
            {
                *verifyRes = memcmp(codec_bench_handle->inPtr,
                                    codec_bench_handle->decompPtr, inSize);
                if (*verifyRes != 0)
                {
                    return -3;
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
                codec_bench_handle->cBestTime = 
                    (codec_bench_handle->cBestTime == 0) ? 
                    aocl_codec_handle->cTime :
                    (codec_bench_handle->cBestTime > 
                     aocl_codec_handle->cTime) ?
                    aocl_codec_handle->cTime : 
                    codec_bench_handle->cBestTime;
                codec_bench_handle->dBestTime = 
                    (codec_bench_handle->dBestTime == 0) ? 
                    aocl_codec_handle->dTime :
                    (codec_bench_handle->dBestTime > 
                     aocl_codec_handle->dTime) ?
                    aocl_codec_handle->dTime : 
                    codec_bench_handle->dBestTime;
            }
            file_size -= inSize;
            inSize = (file_size > inSize) ? inSize : file_size;
        }
        rewind(inFp);
    }

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    return 0;
}

INT64 ipp_run(aocl_codec_bench_info *codec_bench_handle,
              aocl_compression_desc *aocl_codec_handle)
{
    aocl_compression_type i; 
    INTP l, verifyRes, ret;

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

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
                codec_bench_handle->cBestTime = 0;
                codec_bench_handle->dBestTime = 0;
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
                return -2;

        }
        
        if (ret != 0 && ret != -3)
        {
            LOG_UNFORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in executing the dynamic library");
            return -1;
        }

            if (codec_bench_handle->verify)
            {
                if (verifyRes != 0)
                {
                    printf("IPP [%s-%ld] [Filename:%s] verification: failed\n",
                           codec_list[i].codec_name,
                           l, codec_bench_handle->fName);
                    return -1;
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
                    codec_bench_handle->cSpeed = (codec_bench_handle->inSize * 
                        codec_bench_handle->iterations * 1000.0) / 
                        codec_bench_handle->cTime;
                    codec_bench_handle->dSpeed = (codec_bench_handle->inSize * 
                        codec_bench_handle->iterations * 1000.0) / 
                        codec_bench_handle->dTime;
                    codec_bench_handle->cBestSpeed = 
                        (codec_bench_handle->inSize * 1000.0) / 
                        codec_bench_handle->cBestTime;
                    codec_bench_handle->dBestSpeed = 
                        (codec_bench_handle->inSize * 1000.0) / 
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
                           codec_bench_handle->inSize));
                }
            }
        }
    }
    else
    {
        INTP def_level = aocl_codec_handle->level;
        INTP lower_level, upper_level;

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
            codec_bench_handle->cBestTime = 0;
            codec_bench_handle->dBestTime = 0;
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
                return -2;
        }

        if (ret != 0 && ret != -3)
        {
            LOG_UNFORMATTED(ERR, aocl_codec_handle->printDebugLogs,
                "Error in executing the dynamic library");
            return -1;
        }

            if (codec_bench_handle->verify)
            {
                if (verifyRes != 0)
                {
                    printf("IPP [%s-%ld] [Filename:%s] verification: failed\n",
                            codec_list[codec_bench_handle->codec_method].codec_name,
                            aocl_codec_handle->level,
                            codec_bench_handle->fName);
                    return -1;
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
                codec_bench_handle->cSpeed = (codec_bench_handle->inSize * 
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->cTime;
                codec_bench_handle->dSpeed = (codec_bench_handle->inSize * 
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->dTime;
                codec_bench_handle->cBestSpeed = (codec_bench_handle->inSize * 
                    1000.0) / codec_bench_handle->cBestTime;
                codec_bench_handle->dBestSpeed = (codec_bench_handle->inSize * 
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
                        codec_bench_handle->inSize));
            }
        }
    }
    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    return 0;
}

INTP ipp_destroy(aocl_compression_desc *aocl_codec_handle)
{
    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");
    for (aocl_compression_type i = LZ4; i < AOCL_COMPRESSOR_ALGOS_NUM; i++)
    {
        if (hDL[i])
            dlclose(hDL[i]);
    }
    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
    return 0;
}

INTP ipp_bench_run(aocl_compression_desc *aocl_codec_handle,
                  aocl_codec_bench_info *codec_bench_handle)
{
    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    INT64 ret = 0;
    
    ret = ipp_setup(codec_bench_handle, aocl_codec_handle);
    if (ret != 0)
        goto error_exit;
    
    ret = ipp_run(codec_bench_handle, aocl_codec_handle);
    if (ret != 0)
        goto error_exit;

    ipp_destroy(aocl_codec_handle);

    LOG_UNFORMATTED(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    return 0;

error_exit:
    return ret;
}

