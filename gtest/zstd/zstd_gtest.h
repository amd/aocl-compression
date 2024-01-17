/**
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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

 /*
  * Copyright (c) Meta Platforms, Inc. and affiliates.
  * All rights reserved.
  *
  * This source code is licensed under both the BSD-style license (found in the
  * LICENSE file in the root directory of this source tree) and the GPLv2 (found
  * in the COPYING file in the root directory of this source tree).
  * You may select, at your option, one of the above-listed licenses.
  * 
  * https://github.com/facebook/zstd/blob/dev/tests/fullbench.c#L345
  */
 
 /** @file zstd_gtest.h
 *  
 *  @brief Common utility classes and functions used by zstd gtests.
 *
 *  This file contains common utility classes and functions used
 *  by zstd gtests.
 *
 *  @author Ashish Sriram
 */

#include <string>
#include <climits>
#include <cstring>
#include "gtest/gtest.h"

#ifndef AOCL_EXCLUDE_ZSTD
#define ZSTD_STATIC_LINKING_ONLY
#include "algos/zstd/lib/zstd.h"
#include "algos/zstd/lib/compress/zstd_lazy.h"
#include "algos/zstd/lib/decompress/zstd_decompress_block.h"
#endif

#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

#define DEFAULT_OPT_LEVEL 2 // system running gtest must have AVX support

 /* This base class can be used for all fixtures
  * that require dynamic dispatcher setup */
class AOCL_setup_zstd : public ::testing::Test {
public:
    AOCL_setup_zstd() {
        int optLevel = DEFAULT_OPT_LEVEL;
        aocl_setup_zstd_encode(0, optLevel, 0, 0, 0);
        aocl_setup_zstd_decode(0, optLevel, 0, 0, 0);
    }
};

#define ZSTD_COMPRESS_HEAPMODE

class TestLoad_2
{
    //source buffer (original data which we intend to compress).
    char* orig_data = NULL;
    size_t orig_sz = 0;
    //destination buffer (kdata obtained after compression).
    char* compressed_data = NULL;
    size_t compressed_sz = 0;

public:
    // Constructor functions, creates `sz` size of source data.
    TestLoad_2(int sz)
    {
        this->orig_sz = sz;
        orig_data = (char*)malloc(sz);

        // generating random data inside `orig_data` buffer.
        for (int i = 0; i < sz; i++)
        {
            orig_data[i] = rand() % 255;
        }

        // Provides the maximum size that ZSTD compression may output in a "worst case".
        compressed_sz = ZSTD_compressBound(sz);
        compressed_data = (char*)malloc(compressed_sz);
    }
    // Returns pointer to source buffer.
    char* getOrigData()
    {
        return orig_data;
    }
    // Returns size of source buffer.
    size_t getOrigSize()
    {
        return orig_sz;
    }
    // Returns pointer to destination buffer (data obtained after compression).
    char* getCompressedBuff()
    {
        return compressed_data;
    }
    // Returns size of destination data.
    size_t getCompressedSize()
    {
        return compressed_sz;
    }
    // Destructor function.
    ~TestLoad_2()
    {
        free(orig_data);
        free(compressed_data);
        orig_data = NULL;
        compressed_data = NULL;
    }
};

class ZSTD_ZSTD_decompress_base : public AOCL_setup_zstd
{
public:
    // Compressed data is stored in the buffer `src`.
    char* src = NULL;
    size_t srcLen;

    // Original data which we are about to compress is contained in the buffer `original`.
    char* original = NULL;
    size_t origLen;

    // Decompressed data will be stored in the buffer `output`.
    char* output = NULL;
    size_t outLen;

    ZSTD_ZSTD_decompress_base()
    {
        d = new TestLoad_2(800);
        original = d->getOrigData();
        origLen = d->getOrigSize();

        /* decompress should not overwrite dst buffer beyond origLen.
        * Lossless compression should produce the exact bytes as original.
        * Hence, exact same size allocated for output buffer */
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);
    }

    // Destructor function.
    virtual ~ZSTD_ZSTD_decompress_base()
    {
        if (d)
            delete d;
        if (output)
            free(output);
    }

protected:
    TestLoad_2* d = NULL;
};

typedef size_t(*ZSTD_decompress_fp)(ZSTD_DCtx* dctx,
    void* dst, size_t dstCapacity,
    const void* src, size_t srcSize);

/* Helper functions */
bool zstd_check_uncompressed_equal_to_original(char* src, unsigned srcSize,
    char* compressed, unsigned compressedLen, ZSTD_decompress_fp decomp_fp);

/* Commonly used wrapper functions */
bool Test_ZSTD_isError(size_t len);
unsigned long long Test_ZSTD_decompressBound(const void* src, size_t srcLen);
size_t Test_ZSTD_compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize, int compressionLevel);
size_t Test_ZSTD_frameHeaderSize(const void* src, size_t srcSize);
size_t Test_ZSTD_CCtx_setParameter(ZSTD_CCtx* cctx, ZSTD_cParameter param, int value);

#define CHECK_PASS_ZSTD(foo) EXPECT_FALSE(Test_ZSTD_isError(foo));
#define CHECK_FAIL_ZSTD(foo) EXPECT_TRUE(Test_ZSTD_isError(foo));
