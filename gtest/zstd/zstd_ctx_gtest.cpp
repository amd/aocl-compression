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
 
 /** @file zstd_ctx_gtest.cpp
 *  
 *  @brief Test cases for ZSTD APIs that use context.
 *
 *  This file contains the test cases for ZSTD method
 *  testing the API level functions of ZSTD that use context.
 *
 *  @author Partiksha
 */

#include "zstd_gtest.h"

class ZSTD_with_cctx : public AOCL_setup_zstd {
public:
    ZSTD_with_cctx()
    {
        cctx = ZSTD_createCCtx();
    }

    ~ZSTD_with_cctx()
    {
        ZSTD_freeCCtx(cctx);
    }

    ZSTD_CCtx* getCtx() {
        return cctx;
    }

private:
    ZSTD_CCtx* cctx;
};

 /***********************************************
  * "Begin" of ZSTD_compressCCtx
  ***********************************************/
class ZSTD_ZSTD_compressCCtx : public ZSTD_with_cctx {
public:
    size_t compress(const void* src, size_t srcSize, void* dst, size_t dstCapacity, int compressionLevel = ZSTD_CLEVEL_DEFAULT)
    {
        return ZSTD_compressCCtx(getCtx(), dst, dstCapacity, src, srcSize, compressionLevel);
    }
};

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_1) //compressCCtx
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_2) //compressCCtx src null
{
    TestLoad_2 d(1024);
    size_t ret = compress(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_3) //compressCCtx dst null
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_4) //compressCCtx dst size 0
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_5) //compressCCtx cctx null
{
    TestLoad_2 d(1024);
    size_t ret = ZSTD_compressCCtx(NULL, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), 9);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_6) //compressCCtx src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_7) //compressCCtx src null and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_8) //compressCCtx compressionLevel < min
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize(), -1);
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_9) //compressCCtx compressionLevel > max
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize(), 23);
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}
/*********************************************
 * End of ZSTD_compressCCtx
 *********************************************/

 /*********************************************
  * Begin of ZSTD_decompressDCtx
  *********************************************/
class ZSTD_ZSTD_decompressDCtx : public ZSTD_ZSTD_decompress_base
{
public:
    // Write a single compressed frame into src
    void SetUp() override
    {
        src = d->getCompressedBuff();
        srcLen = Test_ZSTD_compress(src, d->getCompressedSize(), original, origLen, ZSTD_CLEVEL_DEFAULT); // Compress data from `original` buffer to `src` buffer.
    }

    // dctx is a paramter in `ZSTD_ZSTD_decompressDCtx`
    ZSTD_DCtx* dctx;

    // constructor to create dctx
    ZSTD_ZSTD_decompressDCtx()
    {
        dctx = ZSTD_createDCtx();
    }

    // Test wrapper function for API ZSTD_decompressDCtx()
    size_t Test_ZSTD_decompressDCtx(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        return ZSTD_decompressDCtx(dctx, dst, dstCapacity, src, srcSize);
    }

    ~ZSTD_ZSTD_decompressDCtx()
    {
        ZSTD_freeDCtx(dctx);
    }

};

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_common_1) // dctx_NULL
{
    // ZSTD_error_GENERIC
    int decLen = Test_ZSTD_decompressDCtx(NULL, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decLen));
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_common_2) // src_NULL
{
    // ZSTD_error_GENERIC
    int decLen = Test_ZSTD_decompressDCtx(dctx, output, Test_ZSTD_decompressBound(src, srcLen), NULL, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decLen));
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_common_3) // dst_NULL
{
    // ZSTD_error_dstBuffer_null
    int decLen = Test_ZSTD_decompressDCtx(dctx, NULL, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decLen));
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_common_4) // successfull_decompression
{
    int decLen = Test_ZSTD_decompressDCtx(dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    ASSERT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, decLen));
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_common_5) // decompression_buffer_inadequate
{
    size_t decompressedLen = Test_ZSTD_decompressDCtx(dctx, output, srcLen / 20, src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decompressedLen));
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_common_6) // compressed_size_zero
{
    EXPECT_EQ(Test_ZSTD_decompressDCtx(dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, 0), 0);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_common_7) // compressed_data_contains_errors
{
    // case 1: Introducing errors in case where input size is small
    const int origLen_1 = 100;
    char orig[origLen_1];
    char dst[100];
    int dstCapacity = ZSTD_compressBound(origLen_1);
    int level = 8;

    for (int i = 0; i < 100; i++)
        orig[i] = 'a';


    dstCapacity = Test_ZSTD_compress(dst, dstCapacity, orig, origLen_1, level);
    int frameHeaderSize = Test_ZSTD_frameHeaderSize(dst, dstCapacity);
    size_t decompress_bound = Test_ZSTD_decompressBound(dst, dstCapacity);
    dst[frameHeaderSize + 2] = 2;

    CHECK_FAIL_ZSTD(Test_ZSTD_decompressDCtx(dctx, output, decompress_bound, dst, dstCapacity));;


    // case 2: modifying compressed data of class to introduce error.
    frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
    decompress_bound = Test_ZSTD_decompressBound(src, srcLen);
    this->src[frameHeaderSize + 1] = 'e';
    this->src[frameHeaderSize + 2] = 'r';
    this->src[frameHeaderSize + 3] = 'r';
    this->src[frameHeaderSize + 4] = 'o';
    this->src[frameHeaderSize + 5] = 'r';

    CHECK_FAIL_ZSTD(Test_ZSTD_decompressDCtx(dctx, output, decompress_bound, src, srcLen));

}

/*********************************************
 * End of ZSTD_decompressDCtx
 *********************************************/

 /***********************************************
  * "Begin" of ZSTD_compress2
  ***********************************************/
class ZSTD_ZSTD_compress2 : public ZSTD_with_cctx {
public:
    size_t compress(const void* src, size_t srcSize, void* dst, size_t dstCapacity)
    {
        return ZSTD_compress2(getCtx(), dst, dstCapacity, src, srcSize);
    }
};

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_1) //compress2
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_2) //compress2 src null
{
    TestLoad_2 d(1024);
    size_t ret = compress(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_3) //compress2 dst null
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_4) //compress2 dst size 0
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_5) //compress2 cctx null
{
    TestLoad_2 d(1024);
    size_t ret = ZSTD_compress2(NULL, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_6) //compress2 src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_7) //compress2 src null and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}
/*********************************************
 * End of ZSTD_compress2
 *********************************************/
