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
 *  @brief Example for AOCL-Compression inflate/deflate API in multithreaded (MT) mode.
 *
 *  The following test program shows the sample usage and calling sequence of 
 *  new deflate/inflate APIs introduced in AOCL-Compression. They can be used to compress 
 *  and decompress a test input in raw deflate/inflate format. These APIs are usable in
 *  multithreaded (MT) mode only as processing is done on multiple threads.
 *
 *  @note To run the program:
 *  example_compress2_raw <input_file_to_be_compressed>
 * 
 *  @author Niranjan Reddy
 */
#if !defined(_WIN32) && !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#if !defined(_WIN32)
#include <sys/types.h>
#endif
#include "zlib.h"

int main(int argc, char** argv)
{
    FILE* inFp = NULL;
#if defined(_WIN32)
    __int64 fileSize = 0;
#else
    off_t fileSize = 0;
#endif
    z_size_t inpSize = 0;
    Bytef* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    z_size_t outSize = 0;
    int resultComp, resultDecomp;
    int level = 6;
    int exitCode = -1;

    printf("Running example_compress2_raw\n");
    printf("Demonstrates using native APIs for raw compression and decompression\n");
    if (argc < 2)
    {
        printf("Provide input test file path\n");
        return -1;
    }
    inFp = fopen(argv[1], "rb");
    if (inFp == NULL)
    {
        printf("Input file open failed\n");
        return -1;
    }
#if defined(_WIN32)
    if (_fseeki64(inFp, 0, SEEK_END) != 0)
#else
    if (fseeko(inFp, 0, SEEK_END) != 0)
#endif
    {
        printf("Input file seek failed\n");
        goto error_exit;
    }
#if defined(_WIN32)
    fileSize = _ftelli64(inFp);
#else
    fileSize = ftello(inFp);
#endif
    if (fileSize < 0)
    {
        printf("Input file tell failed\n");
        goto error_exit;
    }

    if ((uintmax_t)fileSize > (uintmax_t)(z_size_t)-1)
    {
        printf("Input file is too large\n");
        goto error_exit;
    }
    inpSize = (z_size_t)fileSize;
#if defined(_WIN32)
    if (_fseeki64(inFp, 0, SEEK_SET) != 0)
#else
    if (fseeko(inFp, 0, SEEK_SET) != 0)
#endif
    {
        printf("Input file seek failed\n");
        goto error_exit;
    }

    // 1. allocate buffers
    outSize = compressBound_z(inpSize);
    if (outSize == 0 || outSize == (z_size_t)-1 || outSize < inpSize)
    {
        printf("compressBound_z: failed\n");
        goto error_exit;
    }
    inPtr     = (Bytef*)calloc(1, inpSize == 0 ? 1 : inpSize);
    compPtr   = (Bytef*)calloc(1, outSize);
    decompPtr = (Bytef*)calloc(1, inpSize == 0 ? 1 : inpSize);
    if (inPtr == NULL || compPtr == NULL || decompPtr == NULL)
    {
        printf("Memory allocation failed\n");
        goto error_exit;
    }
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
    exitCode = 0;

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
    return exitCode;
}
