/**
 * Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
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

 /** @file example_ZSTD_compress_advanced.c
 *
 *  @brief Example for AOCL-Compression zstd native API.
 *
 *  The following test program shows the sample usage and calling sequence of zstd
 *  native API within AOCL - compression to compress and decompress a test input
 *
 *  @note To run the program:
 *  example_ZSTD_compress_advanced <input_file_to_be_compressed>
 * 
 *  @author Ashish Sriram
 */

#include <stdio.h> 
#include <stdlib.h>
#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

int main(int argc, char** argv)
{
    FILE* inFp = NULL;
    size_t inpSize = 0;
    char* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    size_t outSize = 0;
    size_t resultComp, resultDecomp;
    ZSTD_CCtx* cctx = NULL;
    ZSTD_DCtx* dctx = NULL;
    int level = 3;

    printf("Running example_ZSTD_compress_advanced\n");
    printf("Demonstrates using native APIs for ZSTD compression and decompression\n");
    if (argc < 2)
    {
        printf("Provide input test file path\n");
        return -1;
    }
    inFp = fopen(argv[1], "rb");
    fseek(inFp, 0L, SEEK_END);
    inpSize = ftell(inFp);
    rewind(inFp);

    // 1. allocate buffers
    outSize = ZSTD_compressBound(inpSize);
    if (ZSTD_isError(outSize))
    {
        printf("CompressBound: failed\n");
        goto error_exit;
    }
    inPtr     = (char*)calloc(1, inpSize);
    compPtr   = (char*)calloc(1, outSize);
    decompPtr = (char*)calloc(1, inpSize);
    inpSize = fread(inPtr, 1, inpSize, inFp);

    // 2. compress
    cctx = ZSTD_createCCtx();
    if (!cctx)
    {
        printf("Compression: context creation failed\n");
        goto error_exit;
    }
    ZSTD_parameters zparams = ZSTD_getParams(level, inpSize, 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    resultComp = ZSTD_compress_advanced(cctx, compPtr, outSize, inPtr, inpSize, NULL, 0, zparams);
#pragma GCC diagnostic pop
    if (ZSTD_isError(resultComp))
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    // 3. decompress
    dctx = ZSTD_createDCtx();
    if (!dctx)
    {
        printf("Decompression: context creation failed\n");
        goto error_exit;
    }
    resultDecomp = ZSTD_decompressDCtx(dctx, decompPtr, inpSize, compPtr, resultComp);
    if (ZSTD_isError(resultDecomp))
    {
        printf("Decompression Failure\n");
        goto error_exit;
    }
    printf("Decompression: done\n");

    // 4. cleanup
error_exit:
    if (cctx) 
        ZSTD_freeCCtx(cctx);
    if (dctx)
        ZSTD_freeDCtx(dctx);
    if (inPtr)
        free(inPtr);
    if (compPtr)
        free(compPtr);
    if (decompPtr)
        free(decompPtr);
    if (inFp)
        fclose(inFp);
    return 0;
}
