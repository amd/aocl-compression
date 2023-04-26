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
 
 /** @file api.c
 *  
 *  @brief Interface APIs and data structures of AOCL Compression library
 *
 *  This file contains the unified interface API set and associated
 *  data structure.
 *
 *  @author S. Biplab Raut
 */
 
#include "types.h"
#include "utils/utils.h"
#include "api.h"
#include "codec.h"

static INT32 enableLogs = 0;

//Unified API function to compress the input
INT64 aocl_llc_compress(aocl_compression_desc *handle,
                        aocl_compression_type codec_type)
{
    INT64 ret;
#ifdef WIN32
    timer clkTick;
#endif
    timeVal startTime, endTime;

    LOG_UNFORMATTED(TRACE, enableLogs, "Enter");

    LOG_FORMATTED(INFO, enableLogs,
       "Calling compression method: %s", aocl_codec[codec_type].codec_name);
    initTimer(clkTick);
    getTime(startTime);
    
    ret = aocl_codec[codec_type].compress (handle->inBuf,
                                          handle->inSize,
                                          handle->outBuf,
                                          handle->outSize,
                                          handle->level,
                                          handle->optVar,
                                          handle->workBuf);
    
    getTime(endTime);
    if (handle->measureStats == 1)
    {
        handle->cSize = ret;
        handle->cTime = diffTime(clkTick, startTime, endTime);
        handle->cSpeed = (handle->inSize * 1000.0) / handle->cTime;
    }
    
    LOG_UNFORMATTED(TRACE, enableLogs, "Exit");

    return ret;
}

//Unified API function to decompress the input
INT64 aocl_llc_decompress(aocl_compression_desc *handle,
                          aocl_compression_type codec_type)
{
    INT64 ret;
#ifdef WIN32
    timer clkTick;
#endif
    timeVal startTime, endTime;
    
    LOG_UNFORMATTED(TRACE, enableLogs, "Enter");

    LOG_FORMATTED(INFO, enableLogs,
       "Calling decompression method: %s", aocl_codec[codec_type].codec_name);
    initTimer(clkTick);
    getTime(startTime);
    
    ret = aocl_codec[codec_type].decompress (handle->inBuf,
                                            handle->inSize,
                                            handle->outBuf,
                                            handle->outSize,
                                            handle->level,
                                            handle->optVar,
                                            handle->workBuf);
    
    getTime(endTime);
    if (handle->measureStats == 1)
    {
        handle->dSize = ret;
        handle->dTime = diffTime(clkTick, startTime, endTime);
        handle->dSpeed = (handle->dSize * 1000.0) / handle->dTime;
    }

    LOG_UNFORMATTED(TRACE, enableLogs, "Exit");

    return ret;
}

//API to setup and initialize memory for the compression method
INT32 aocl_llc_setup(aocl_compression_desc *handle,
                    aocl_compression_type codec_type)
{
    enableLogs = handle->printDebugLogs;

    LOG_UNFORMATTED(TRACE, enableLogs, "Enter");

    if ((codec_type < LZ4) || (codec_type >= AOCL_COMPRESSOR_ALGOS_NUM))
    {
        LOG_UNFORMATTED(ERR, enableLogs,
            "setup failed !! compression method is not supported.");
        return ERR_UNSUPPORTED_METHOD;
    }

    LOG_FORMATTED(INFO, enableLogs,
       "All optimizations are turned %s", (handle->optOff ? "off" : "on"));

    if (!handle->optOff)
        set_cpu_opt_flags((VOID *)handle);

    LOG_FORMATTED(INFO, enableLogs,
       "Calling setup method for: %s", aocl_codec[codec_type].codec_name);

    if (aocl_codec[codec_type].setup)
    {
        handle->workBuf = aocl_codec[codec_type].setup (handle->optOff,
                                                        handle->optLevel,
                                                        handle->inSize,
                                                        handle->level,
                                                        handle->optVar);
    }
    else
    {
        LOG_UNFORMATTED(ERR, enableLogs,
            "setup failed !! compression method is excluded from this library build.");
        LOG_UNFORMATTED(TRACE, enableLogs, "Exit");
        return ERR_EXCLUDED_METHOD;
    }

    LOG_UNFORMATTED(TRACE, enableLogs, "Exit");
    return 0;
}

//API to destroy memory and deinit the compression method
VOID aocl_llc_destroy(aocl_compression_desc *handle,
                      aocl_compression_type codec_type)
{
    LOG_UNFORMATTED(TRACE, enableLogs, "Enter");

    LOG_FORMATTED(INFO, enableLogs,
       "Calling destroy method for: %s", aocl_codec[codec_type].codec_name);

    if (aocl_codec[codec_type].destroy)
    {
        aocl_codec[codec_type].destroy(handle->workBuf);
    }

    LOG_UNFORMATTED(TRACE, enableLogs, "Exit");
}

//API to return the compression library version string
const CHAR *aocl_llc_version(VOID)
{
    return (LIBRARY_VERSION " " AOCL_BUILD_VERSION);
}
