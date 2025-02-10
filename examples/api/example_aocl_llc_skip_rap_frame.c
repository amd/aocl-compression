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

 /** @file example_aocl_llc_skip_rap_frame.c
 *
 *  @brief Example for AOCL-Compression unified API in multithreaded (MT) mode.
 *
 *  The following test program shows the sample usage and calling sequence of 
 *  AOCL - compression APIs to get ST compatible compressed stream from a stream 
 *  produced by AOCL MT compressor
 * 
 *  Library must be built with AOCL_ENABLE_THREADS=ON to run this example.
 * 
 *  @note To run the program:
 *  example_aocl_llc_skip_rap_frame <input_file_to_be_compressed>
 *
 *  @author Ashish Sriram
 */

#include <stdio.h> 
#include <stdlib.h>
#include "aocl_compression.h"

int main(int argc, char** argv)
{
    aocl_compression_desc aocl_compression_ds;
    aocl_compression_desc* aocl_compression_handle = &aocl_compression_ds;
    FILE* inFp = NULL;
    int file_size = 0;
    char* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    int64_t resultCompBound = 0, resultComp = 0, resultDecomp = 0;

    printf("Running example_aocl_llc_skip_rap_frame\n");
    printf("Demonstrates obtaining format-compliant compressed stream from a stream produced by AOCL multi-threaded compressor\n");
    if (argc < 2)
    {
        printf("Provide input test file path\n");
        return -1;
    }
    inFp = fopen(argv[1], "rb");
    fseek(inFp, 0L, SEEK_END);
    file_size = ftell(inFp);
    rewind(inFp);

    aocl_compression_type method = LZ4; // One of the compression methods as per aocl_compression_type
    aocl_compression_handle->level = 0;
    aocl_compression_handle->optVar = 0;
    aocl_compression_handle->optOff = 0;
    aocl_compression_handle->measureStats = 0;

    // 1. setup and create a handle
    if (aocl_llc_setup(aocl_compression_handle, method) != 0)
    {
        printf("Setup: failed\n");
        goto error_exit;
    }

    // 2. allocate buffers
    aocl_compression_handle->inSize = file_size;
    resultCompBound = aocl_llc_compressBound(method, aocl_compression_handle->inSize);
    if (resultCompBound < 0)
    {
        printf("CompressBound: failed\n");
        goto error_exit;
    }
    aocl_compression_handle->outSize = resultCompBound;
    inPtr = (char*)calloc(1, aocl_compression_handle->inSize);
    compPtr = (char*)calloc(1, aocl_compression_handle->outSize);
    decompPtr = (char*)calloc(1, aocl_compression_handle->inSize);
    aocl_compression_handle->inBuf = inPtr;
    aocl_compression_handle->outBuf = compPtr;
    file_size = fread(inPtr, 1, file_size, inFp);


    // 3. MT compress
    resultComp = aocl_llc_compress(aocl_compression_handle, method);

    if (resultComp <= 0)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    //4. ST decompress
    // Get number of bytes for the RAP frame
    int rap_frame_len = aocl_llc_skip_rap_frame((char*)compPtr, resultComp);

    // Skip RAP frame in input stream and pass this to ST decompressor
    aocl_compression_handle->inSize = resultComp - rap_frame_len;
    aocl_compression_handle->outSize = file_size;
    aocl_compression_handle->inBuf = compPtr + rap_frame_len;
    aocl_compression_handle->outBuf = decompPtr;

    // Pass format compliant stream to aocl decompressor (or any legacy ST decompressor)
    resultDecomp = aocl_llc_decompress(aocl_compression_handle, method);

    if (resultDecomp <= 0)
    {
        printf("Decompression Failure\n");
        goto error_exit;
    }
    printf("Decompression: done\n");

    // 5. destroy handle
    aocl_llc_destroy(aocl_compression_handle, method);
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
