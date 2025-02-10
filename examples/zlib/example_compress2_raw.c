/**
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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

 /** @file example_compress2_raw.c
 *
 *  @brief Example for AOCL-Compression zlib native API.
 *
 *  The following test program shows the sample usage and calling sequence of zlib
 *  native API within AOCL - compression to compress and decompress a test input
 *
 *  @note To run the program:
 *  example_compress2_raw <input_file_to_be_compressed>
 * 
 *  @author Niranjan Reddy
 */
#include <stdio.h>
#include <stdlib.h>
#include "zlib.h"

int main(int argc, char** argv)
{
    FILE* inFp = NULL;
    uLong inpSize = 0;
    Bytef* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    uLong outSize = 0;
    int resultComp, resultDecomp;
    int level = 6;

    printf("Running example_compress2_raw\n");
    printf("Demonstrates using native APIs for raw compression and decompression\n");
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
    outSize = compressBound(inpSize);
    if (outSize == 0)
    {
        printf("compressBound: failed\n");
        goto error_exit;
    }
    inPtr     = (Bytef*)calloc(1, inpSize);
    compPtr   = (Bytef*)calloc(1, outSize);
    decompPtr = (Bytef*)calloc(1, inpSize);
    inpSize = fread(inPtr, 1, inpSize, inFp);

    // 2. compress
    resultComp = compress2_raw(compPtr, &outSize, inPtr, inpSize, level);
    if (resultComp != Z_OK)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    // 3. decompress
    resultDecomp = uncompress2_raw(decompPtr, &inpSize, compPtr, &outSize);
    if (resultDecomp != Z_OK)
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
