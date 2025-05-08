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

 /** @file example_LzmaEncode.c
 *
 *  @brief Example for AOCL-Compression lzma native API.
 *
 *  The following test program shows the sample usage and calling sequence of lzma
 *  native API within AOCL - compression to compress and decompress a test input
 *
 *  @note To run the program:
 *  example_LzmaEncode <input_file_to_be_compressed>
 * 
 *  @author Ashish Sriram
 */

#include <stdio.h> 
#include <stdlib.h>
#include "LzmaDec.h"
#include "LzmaEnc.h"

#define IGNORE_VAR(x) (void)x;
static void* LzmaAlloc(ISzAllocPtr p, size_t size) { IGNORE_VAR(p); return malloc(size); }
static void LzmaFree(ISzAllocPtr p, void* address) { IGNORE_VAR(p); free(address); }

int main(int argc, char** argv)
{
    FILE* inFp = NULL;
    SizeT inpSize = 0;
    Byte* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    SizeT outSize = 0;
    SRes resultComp, resultDecomp;
    int level = 1;

    printf("Running example_LzmaEncode\n");
    printf("Demonstrates using native APIs for LZMA compression and decompression\n");
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
    outSize = inpSize * 2; // adjust as per compression bound for the input being fed
    if (outSize == 0)
    {
        printf("CompressBound: failed\n");
        goto error_exit;
    }
    inPtr     = (Byte*)calloc(1, inpSize);
    compPtr   = (Byte*)calloc(1, outSize);
    decompPtr = (Byte*)calloc(1, inpSize);
    inpSize = fread(inPtr, 1, inpSize, inFp);

    // 2. compress
    CLzmaEncProps encProps;
    SizeT headerSize = LZMA_PROPS_SIZE;
    SizeT outLen = outSize - LZMA_PROPS_SIZE;
    LzmaEncProps_Init(&encProps);
    encProps.level = level;
    const ISzAlloc lzma_alloc = { LzmaAlloc, LzmaFree };
    resultComp = LzmaEncode(compPtr + LZMA_PROPS_SIZE, &outLen, inPtr, inpSize, &encProps,
                            compPtr, &headerSize, 0, NULL, &lzma_alloc, &lzma_alloc);
    if (resultComp != SZ_OK)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    // 3. decompress
    SizeT srcLen = (LZMA_PROPS_SIZE + outLen) - LZMA_PROPS_SIZE;
    outLen = inpSize;
    ELzmaStatus status;
    resultDecomp = LzmaDecode(decompPtr, &outLen, compPtr + LZMA_PROPS_SIZE, &srcLen, compPtr,
                              LZMA_PROPS_SIZE, LZMA_FINISH_END, &status, &lzma_alloc);
    if (resultDecomp != SZ_OK)
    {
        printf("Decompression Failure\n");
        goto error_exit;
    }
    printf("Decompression: done\n");

    // 4. cleanup
error_exit:
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
