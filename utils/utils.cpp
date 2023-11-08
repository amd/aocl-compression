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

/** @file utils.c
 *  
 *  @brief Utility functions that are used by library framework and methods.
 *
 *  This file contains the utility functions to provide functionalities like
 *  CPU feature detection and others.
 *
 *  @author S. Biplab Raut
 */

#include "api/types.h"
#include "api/aocl_compression.h"
#include "utils/utils.h"

#ifdef AOCL_ENABLE_LOG_FEATURE

#ifdef __cplusplus
extern "C" {
#endif

aocl_log_ctx logCtx = {-1};

#ifdef __cplusplus
}
#endif

#endif /* AOCL_ENABLE_LOG_FEATURE */

AOCL_INTP is_SSE2_supported(aocl_compression_desc *aocl_codec_handle)
{
    AOCL_INTP ret;
    AOCL_INTP eax, ebx, ecx, edx;
    cpu_features_detection(0x00000001, 0, &eax, &ebx, &ecx, &edx);
    ret = ((edx & (1 << 26)) != 0);
    LOG_FORMATTED(INFO, logCtx,
        "SSE2 SIMD %s supported", (ret ? "is" : "is not"));
    return ret;
}

AOCL_INTP is_AVX_supported(aocl_compression_desc *aocl_codec_handle)
{
    AOCL_INTP ret;
    AOCL_INTP eax, ebx, ecx, edx;
    cpu_features_detection(0x00000001, 0, &eax, &ebx, &ecx, &edx);
    ret = ((ecx & 0x18000000) == 0x18000000);
    LOG_FORMATTED(INFO, logCtx,
        "AVX SIMD %s supported", (ret ? "is" : "is not"));
    return ret;
}

AOCL_INTP is_AVX2_supported(aocl_compression_desc *aocl_codec_handle)
{
    AOCL_INTP ret;
    AOCL_INTP eax, ebx, ecx, edx;
    cpu_features_detection(0x00000007, 0, &eax, &ebx, &ecx, &edx);
    ret = ((ebx & (1 << 5)) != 0);
    LOG_FORMATTED(INFO, logCtx,
        "AVX2 SIMD %s supported", (ret ? "is" : "is not"));
    return ret;
}

static inline AOCL_INTP xgetbv(AOCL_INTP opt)
{
    int eax, edx;
    __asm__(".byte 0x0f, 0x01, 0xd0" : "=a"(eax), "=d"(edx) : "c" (opt));
    return eax;
}

AOCL_INTP is_AVX512_supported(aocl_compression_desc *aocl_codec_handle)
{
    AOCL_INTP ret = 0;
    AOCL_INTP eax, ebx, ecx, edx;
    //Below is the set of checks for AVX512 detection
    //1. Check CPU support for ZMM state management using OSXSAVE 
    //Its support also implies that XGETBV is enabled for application use
    cpu_features_detection(0x1, 0, &eax, &ebx, &ecx, &edx);
    if ((ecx & 0x08000000) == 0x08000000)
    {
        //2. Check OS support for XGETBV instruction and ZMM register state
        AOCL_INTP reg_support_bits = (7 << 5) | (1 << 2) | (1 << 1);
        if ((xgetbv(0) & reg_support_bits) == reg_support_bits)
        {
            //3. Check CPU support for AVX-512 Foundation instructions
            cpu_features_detection(7, 0, &eax, &ebx, &ecx, &edx);
            if (ebx & (1 << 16))
            {
                ret = 1;
            }
        }
    }
    LOG_FORMATTED(INFO, logCtx,
        "AVX512 SIMD %s supported", (ret ? "is" : "is not"));
    return ret;
}

AOCL_VOID set_cpu_opt_flags(AOCL_VOID *handle)
{
    LOG_UNFORMATTED(TRACE, logCtx, "Enter");
    
    aocl_compression_desc *aocl_codec_handle = (aocl_compression_desc *)handle;
    
    aocl_codec_handle->optLevel = is_SSE2_supported(aocl_codec_handle);
    aocl_codec_handle->optLevel += is_AVX_supported(aocl_codec_handle);
    aocl_codec_handle->optLevel += is_AVX2_supported(aocl_codec_handle);
    aocl_codec_handle->optLevel += is_AVX512_supported(aocl_codec_handle);
    
    LOG_UNFORMATTED(TRACE, logCtx, "Exit");
}
