/**
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

 /** @file example_LZ4_compress_HC.c
 *
 *  @brief Example for AOCL-Compression lz4hc native API.
 *
 *  The following test program shows the sample usage and calling sequence of lz4hc
 *  native API within AOCL - compression to compress and decompress a test input
 *
 *  @note To run the program:
 *  example_LZ4_compress_HC <input_file_to_be_compressed>
 * 
 *  @author Ashish Sriram
 */

#include <stdio.h> 
#include <stdlib.h>
#include "lz4.h"
#include "lz4hc.h"

int main(int argc, char** argv)
{
    FILE* inFp = NULL;
    unsigned inpSize = 0;
    char* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    unsigned outSize = 0;
    int resultComp = 0, resultDecomp = 0;
    int level = 9;

    printf("Running example_LZ4_compress_HC\n");
    printf("Demonstrates using native APIs for LZ4HC compression and decompression\n");
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
    outSize = LZ4_compressBound(inpSize);
    if (outSize == 0)
    {
        printf("CompressBound: failed\n");
        goto error_exit;
    }
    inPtr     = (char*)calloc(1, inpSize);
    compPtr   = (char*)calloc(1, outSize);
    decompPtr = (char*)calloc(1, inpSize);
    inpSize = fread(inPtr, 1, inpSize, inFp);

    // 2. compress
    resultComp = LZ4_compress_HC(inPtr, compPtr, inpSize, outSize, level);
    if (resultComp <= 0)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    // 3. decompress
    resultDecomp = LZ4_decompress_safe(compPtr, decompPtr, resultComp, inpSize);
    if (resultDecomp < 0)
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
    return 0;
}
