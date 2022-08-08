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
 
 /** @file api.c
 *  
 *  @brief Interface APIs and data structures of AOCL Codec library
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
INT64 aocl_codec_compress(aocl_codec_desc *handle,
                          aocl_codec_type codec_type)
{
    INT64 ret;
    timer clkTick;
    timeVal startTime, endTime;

    LOG(TRACE, enableLogs, "Enter");

    LOG(INFO, enableLogs,
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
    
    LOG(TRACE, enableLogs, "Exit");

    return ret;
}

//Unified API function to decompress the input
INT64 aocl_codec_decompress(aocl_codec_desc *handle,
                            aocl_codec_type codec_type)
{
    INT64 ret;
    timer clkTick;
    timeVal startTime, endTime;
    
    LOG(TRACE, enableLogs, "Enter");

    LOG(INFO, enableLogs,
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

    LOG(TRACE, enableLogs, "Exit");

    return ret;
}

//API to setup and initialize memory for the codec method
VOID aocl_codec_setup(aocl_codec_desc *handle,
                      aocl_codec_type codec_type)
{
    enableLogs = handle->printDebugLogs;

    LOG(TRACE, enableLogs, "Enter");

    LOG(INFO, enableLogs,
       "All optimizations are turned %s", (handle->optOff ? "off" : "on"));

    if (!handle->optOff)
        set_cpu_opt_flags((VOID *)handle);

    LOG(INFO, enableLogs,
       "Calling setup method for: %s", aocl_codec[codec_type].codec_name);

    if (aocl_codec[codec_type].setup)
    {
        handle->workBuf = aocl_codec[codec_type].setup (handle->optOff,
                                                        handle->optLevel,
                                                        handle->inSize,
                                                        handle->level,
                                                        handle->optVar);
    }

    LOG(TRACE, enableLogs, "Exit");
}

//API to destroy memory and deinit the codec method
VOID aocl_codec_destroy(aocl_codec_desc *handle,
                        aocl_codec_type codec_type)
{
    LOG(TRACE, enableLogs, "Enter");

    LOG(INFO, enableLogs,
       "Calling destroy method for: %s", aocl_codec[codec_type].codec_name);

    if (aocl_codec[codec_type].destroy)
    {
        aocl_codec[codec_type].destroy(handle->workBuf);
    }

    LOG(TRACE, enableLogs, "Exit");
}

//API to return the compression library version string
const CHAR *aocl_codec_version(VOID)
{
    return LIBRARY_VERSION;
}
