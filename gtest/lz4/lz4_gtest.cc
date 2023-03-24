/**
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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
 
 /** @file lz4_gtest.cc
 *  
 *  @brief Test cases for LZ4 algo.
 *
 *  This file contains the test cases for LZ4 method
 *  testing the API level functions of LZ4.
 *
 *  @author J Niranjan Reddy
 */

#include <string>
#include <climits>
#include "gtest/gtest.h"

#define AOCL_DYNAMIC_DISPATCHER
#include "algos/lz4/lz4.h"

using namespace std;

/*
    This class is used for allocating and deallocating memory,
    which acts as a common way for instantiation input and output buffer
    for most of the test cases.    
*/
class TestLoad
{
    //source buffer (original data which we intend to compress).
    char *orig_data;
    size_t orig_sz = 0;
    //destination buffer (kdata obtained after compression).
    char *compressed_data;
    size_t compressed_sz=0;

public:
    // Constructor functions, creates `sz` size of source data.
    TestLoad(int sz)
    {
        this->orig_sz = sz;
        orig_data = (char *)malloc(sz);

        // generating random data inside `orig_data` buffer.
        for (int i = 0; i < sz; i++)
        {
            orig_data[i] = rand() % 255;
        }

        // Provides the maximum size that LZ4 compression may output in a "worst case".
        compressed_sz = LZ4_compressBound(sz);
        compressed_data = (char *)malloc(compressed_sz);
    }
    // Returns pointer to source buffer.
    char *getOrigData()
    {
        return orig_data;
    }
    // Returns size of source buffer.
    size_t getOrigSize()
    {
        return orig_sz;
    }
    // Returns pointer to destination buffer (data obtained after compression).
    char *getCompressedBuff()
    {
        return compressed_data;
    }
    // Returns size of destination data.
    size_t getCompressedSize()
    {
        return compressed_sz;
    }
    // Destructor function.
    ~TestLoad()
    {
        free(orig_data);
        free(compressed_data);
    }
};

/*
    This functions decompresses and checks if the data form `src` pointer
    matches the output of decompressed data from `compressed` pointer.
    This function works as a utility function to verify if the compression
    is happening properly.
*/
bool check_uncompressed_equal_to_original(char *src, unsigned srcSize, char *compressed, unsigned compressedLen)
{
    int uncompressedLen = srcSize + 10;
    char uncompressed[uncompressedLen];

    uncompressedLen = LZ4_decompress_safe(compressed, uncompressed, compressedLen, uncompressedLen);

    if (!(srcSize == uncompressedLen))
        return false;

    return memcmp(src, uncompressed, srcSize) == 0;
}

/***********************************************
 * "Begin" of Version Tests
 ***********************************************/

TEST(LZ4_versionNumber, AOCL_Compression_lz4_LZ4_versionNumber_common_1) // Number
{
    EXPECT_EQ(LZ4_versionNumber(), LZ4_VERSION_NUMBER);
}

TEST(LZ4_versionString, AOCL_Compression_lz4_LZ4_versionString_common_1) // String
{
    EXPECT_STREQ(LZ4_versionString(), LZ4_VERSION_STRING);
}
/*********************************************
 * End of Version Tests
 *********************************************/

/*********************************************
 * Compression Tests Begin
 *********************************************/

TEST(LZ4_compress_default, AOCL_Compression_lz4_LZ4_compress_default_common_1) // compressFail_src_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_default(NULL, d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize()), -1);
}

TEST(LZ4_compress_default, AOCL_Compression_lz4_LZ4_compress_default_common_2) // compressFail_dst_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_default(d.getOrigData(), NULL, d.getOrigSize(), d.getCompressedSize()), -1);
}

TEST(LZ4_compress_default, AOCL_Compression_lz4_LZ4_compress_default_common_3) // compressionPass
{
    TestLoad d(800);

    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize());
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

TEST(LZ4_compress_default, AOCL_Compression_lz4_LZ4_compress_default_common_4) // compressFail_dst_size_not_enough
{
    TestLoad d(800);

    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getOrigSize()/20);
    EXPECT_FALSE(check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

/*********************************************
 * "End" of Compression Tests
 ********************************************/

/*********************************************
 * "Begin" of Decompression Tests
 ********************************************/

/*
    This is a fixture class that is used for testing `LZ4_decompress_safe` function,
    `original` points to source data(randomized data), and compressed into `src` buffer,
    `output` buffer will be used for storing decompressed data.
*/
class LLZ4_decompress_safe : public ::testing::Test
{
public:
    TestLoad *d = NULL;
    // Compressed data is stored in the buffer `src`.
    char *src = NULL;
    int srcLen;
    // Original data which we are about to compress is contained in the buffer `original`.
    char *original = NULL;
    int origLen;
    // Decompressed data will be stored in the buffer `output`.
    char *output = NULL;
    int outLen;

    // By default the `original` buffer of size 800 is initialized upon testcase initialization.
    void SetUp() override
    {
        d = new TestLoad(800);
        original = d->getOrigData();
        origLen = d->getOrigSize();
        src = d->getCompressedBuff();

        // Compress data from `original` buffer to `src` buffer.
        srcLen = LZ4_compress_default(original, src, origLen, d->getCompressedSize());
        outLen = d->getOrigSize()*2 + 10;
        output = (char *)malloc(outLen);
    }
    
    // By default the `original` buffer size is set to 800
    // we can use `setOrigSz(int sz)` function to reset it size to sz.
    void setOrigSz(int sz)
    {
        if (d != NULL)
            delete d;
        d = new TestLoad(sz);
        original = d->getOrigData();
        origLen = d->getOrigSize();
        srcLen = d->getCompressedSize();
        src = d->getCompressedBuff();
        srcLen = LZ4_compress_default(original, src, origLen, srcLen);
    }

    // Reset `output` buffer size to `sz`.
    void setDstSz(int sz)
    {
        if (output != NULL)
            free(output);
        outLen = sz;
        output = (char *)malloc(outLen);
    }

    // Destructor function.
    ~LLZ4_decompress_safe()
    {
        if(d)
            delete d;
        free(output);
    }
};

TEST_F(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_common_1) // src_NULL
{
    EXPECT_EQ(LZ4_decompress_safe(NULL, output, srcLen, LZ4_compressBound(srcLen)), -1);
}

TEST_F(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_common_2) // dst_NULL
{
    EXPECT_EQ(LZ4_decompress_safe(src, NULL, srcLen, LZ4_compressBound(srcLen)), -1);
}

TEST_F(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_common_3) // successfull_decompression
{
    int decLen = LZ4_decompress_safe(src, output, srcLen, LZ4_compressBound(srcLen));
    ASSERT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, decLen));
}

TEST_F(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_common_4) // cmp_siz_inadeqate
{
    EXPECT_LT(LZ4_decompress_safe(src, output, srcLen, srcLen / 20), 0);
}

TEST_F(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_common_5) // cmp_siz_zero
{
    EXPECT_EQ(LZ4_decompress_safe(src, output, 0, LZ4_compressBound(srcLen)), -1);
}

TEST_F(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_common_6 ) // cmp_data_contains_errors
{
    int origLen = 100;
    char orig[origLen];
    char dst[100];
    int dstCapacity = 100;

    for (int i = 0; i < 100; i++)
        orig[i] = 'a';

    dstCapacity = LZ4_compress_default(orig, dst, origLen, dstCapacity);
    dst[2] = 2;

    EXPECT_LT(LZ4_decompress_safe(dst, output, dstCapacity, LZ4_compressBound(srcLen)), 0);
}

/*********************************************
 * "End" of Decompression Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compressBound
 *********************************************/

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_common_1) // 65025
{
    EXPECT_EQ(LZ4_compressBound(65025), 65296);
}

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_common_2) // 0
{
    EXPECT_EQ(LZ4_compressBound(0), 16);
}

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_common_3) // MAX_LIMIT
{
    EXPECT_EQ(LZ4_compressBound(0x7E000000 + 1), 0);
}

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_common_4) // MINUS_ONE
{
    EXPECT_EQ(LZ4_compressBound(-1), 0);
}

/*********************************************
 * "End" of LZ4_compressBound Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_sizeofState
 *********************************************/

TEST(LZ4_sizeofState, AOCL_Compression_lz4_LZ4_sizeofState_common_1) // NULL
{
    EXPECT_EQ(LZ4_sizeofState(), 16416);
}

/*********************************************
 * "End" of LZ4_sizeofState Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compress_fast
 *********************************************/

TEST(LZ4_compress_fast, AOCL_Compression_lz4_LZ4_compress_fast_common_1) // src_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_fast(NULL, d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 1), -1);
}

TEST(LZ4_compress_fast, AOCL_Compression_lz4_LZ4_compress_fast_common_2) // dest_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_fast(d.getOrigData(), NULL, d.getOrigSize(), d.getCompressedSize(), 1), -1);
}

TEST(LZ4_compress_fast, AOCL_Compression_lz4_LZ4_compress_fast_common_3) // Pass
{
    TestLoad d(800);

    int compressedSize = LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 1);
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedSize));
}

TEST(LZ4_compress_fast, AOCL_Compression_lz4_LZ4_compress_fast_common_4) // OutLen_small
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getOrigSize()/260, 1), 0);
}

TEST(LZ4_compress_fast, AOCL_Compression_lz4_LZ4_compress_fast_common_5) // acc_greater_MAX
{
    TestLoad d(800);
    int compressedSize = LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 600000);
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedSize));
}

TEST(LZ4_compress_fast, AOCL_Compression_lz4_LZ4_compress_fast_common_6) // acc_less_MIN
{
    TestLoad d(800);

    int compressedSize = LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 0);
    EXPECT_NE(compressedSize, 0);
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedSize));
}

/*********************************************
 * "End" of LZ4_compress_fast Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compress_destSize
 *********************************************/

TEST(LZ4_compress_destSize, AOCL_Compression_lz4_LZ4_compress_destSize_common_1) // src_NULL
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_destSize(NULL, d.getCompressedBuff(), &srcLen, d.getCompressedSize()), -1);
}

TEST(LZ4_compress_destSize, AOCL_Compression_lz4_LZ4_compress_destSize_common_2) // output_NULL
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), NULL, &srcLen, d.getCompressedSize()), -1);
}

TEST(LZ4_compress_destSize, AOCL_Compression_lz4_LZ4_compress_destSize_common_3) // srcLen_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), NULL, d.getCompressedSize()), -1);
}

TEST(LZ4_compress_destSize, AOCL_Compression_lz4_LZ4_compress_destSize_common_4) // Pass
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();
    int compressedLen = LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, d.getCompressedSize());
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedLen));
}

TEST(LZ4_compress_destSize, AOCL_Compression_lz4_LZ4_compress_destSize_common_5) // destLen_is_zero
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, 0), 0);
}

/*********************************************
 * End of LZ4_compress_destSize
 *********************************************/

/*********************************************
 * "Begin" of LZ4_decompress_safe_partial
 *********************************************/
class LLZ4_decompress_safe_partial : public LLZ4_decompress_safe
{
};

TEST_F(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_common_1) // src_NULL
{
    EXPECT_EQ(LZ4_decompress_safe_partial(NULL, output, srcLen, outLen, outLen), -1);
}

TEST_F(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_common_2) // dst_NULL
{
    EXPECT_EQ(LZ4_decompress_safe_partial(src, NULL, srcLen, outLen, outLen), -1);
}

TEST_F(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_common_3) // srcLen_zero
{
    EXPECT_EQ(LZ4_decompress_safe_partial(src, output, 0, outLen, outLen), -1);
}

TEST_F(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_common_4) // Pass
{
    int decLen = LZ4_decompress_safe_partial(src, output, srcLen, outLen, outLen);
    ASSERT_EQ(decLen, origLen);
    EXPECT_EQ(0, memcmp(output, original, decLen));
}

TEST_F(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_common_5) // srcLen_1
{
    setOrigSz(1);
    setDstSz(10);
    srcLen = LZ4_compress_default(original, src, origLen, srcLen);

    EXPECT_EQ(srcLen, 2);
    EXPECT_EQ(LZ4_decompress_safe_partial(src, output, srcLen, outLen, outLen), 1);
    EXPECT_EQ(memcmp(output, original, origLen), 0);
}

TEST_F(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_common_6) // decompress_partial
{
    int decLen = LZ4_decompress_safe_partial(src, output, srcLen, srcLen / 2, srcLen);
    EXPECT_EQ(decLen, srcLen/2);
    EXPECT_EQ(0, memcmp(output, original, decLen));
}
/*********************************************
 * End of LZ4_decompress_safe_partial
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compress_fast_extState
 *********************************************/

/*
    This is a fixture class for testing `LZ4_compress_fast_extState` function.
    `src` source data that we are going to compress is maintained here.
    `dst` compressed data is stored here.
*/
class LLZ4_compress_fast_extState : public ::testing::Test
{

protected:

    char *src = NULL;
    int srcSize;
    char *dst = NULL;
    int dstSize;
    // For the function `LZ4_compress_fast_extState`, `state` is a parameter.
    LZ4_stream_t *state = NULL;

    // Initialization of state variable takes place at start of each test case.
    void SetUp() override
    {
        state = LZ4_createStream();
    }

    // Initialize or reset `src` buffer.
    void setSrcSize(int sz)
    {
        if(src)
            free(src);
        srcSize = sz;
        src = (char *)malloc(srcSize);
        for (int i = 0; i < srcSize; i++)
        {
            src[i] = rand() % 255;
        }
    }

    // Initialize or reset `dst` buffer.
    void setDstSize(int sz)
    {
        if(dst)
            free(dst);
        dstSize = sz;
        dst = (char *)malloc(dstSize);
    }

    // Destructor function of `LLZ4_compress_fast_extState`.
    ~LLZ4_compress_fast_extState()
    {
        free(dst);
        free(src);
        free(state);
    }
};

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_1) // state_is_Null
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_fast_extState(NULL, src, dst, srcSize, dstSize, 5), -1);
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_2) // src_NULL_srcLen_not_NULL
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_fast_extState(state, NULL, dst, srcSize, dstSize, 5), -1);
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_3) // dst_NULL
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_fast_extState(state, src, NULL, srcSize, dstSize, 5), -1);
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_4) // src_NULL_srcSize_0
{
    setSrcSize(1000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_fast_extState(state, NULL, dst, 0, dstSize, 2), 1);
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_5) // acceleration_is_greater_than_max
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 600000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_6) // acceleration_is_less_than_min
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 0);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_7) // if_coverage1
{
    setSrcSize(60000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 2);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_8) // if_coverage2
{
    setSrcSize(70000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 6000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_9) // if_coverage3
{
    setSrcSize(7000);
    setDstSize(7000);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 6000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_common_10) // if_coverage4
{
    setSrcSize(70000);
    setDstSize(70000);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 6000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

/*********************************************
 * End of LZ4_compress_fast_extState
 *********************************************/

/*********************************************
 * "Begin" of LZ4_AOCL_LZ4_compress_fast_extState
 *********************************************/

class LZ4_AOCL_LZ4_compress_fast_extState : public LLZ4_compress_fast_extState
{
};

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_1) // state_is_Null
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(AOCL_LZ4_compress_fast_extState(NULL, src, dst, srcSize, dstSize, 5), -1);
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_2) // src_NULL_srcLen_not_NULL
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(AOCL_LZ4_compress_fast_extState(state, NULL, dst, srcSize, dstSize, 5), -1);
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_3) // dst_NULL
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(AOCL_LZ4_compress_fast_extState(state, src, NULL, srcSize, dstSize, 5), -1);
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_4) // src_NULL_srcSize_0
{
    setSrcSize(1000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(AOCL_LZ4_compress_fast_extState(state, NULL, dst, 0, dstSize, 2), 1);
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_5) // acceleration_is_greater_than_max
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = AOCL_LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 600000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_6) // acceleration_is_less_than_min
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = AOCL_LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, -100);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_7) // if_coverage1
{
    setSrcSize(60000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = AOCL_LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 2);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_8) // if_coverage2
{
    setSrcSize(70000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = AOCL_LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 6000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_9) // if_coverage3
{
    setSrcSize(7000);
    setDstSize(7000);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    int compressedSize = AOCL_LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 6000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4_AOCL_LZ4_compress_fast_extState, AOCL_Compression_lz4_AOCL_LZ4_compress_fast_extState_common_10) // if_coverage4
{
    setSrcSize(70000);
    setDstSize(70000);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    int compressedSize = AOCL_LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 6000);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

/*********************************************
 * End of LZ4_compress_fast_extState
 *********************************************/

/*********************************************
 * "Begin" of LZ4_createStream
 *********************************************/

TEST(LZ4_createStream, AOCL_Compression_lz4_LZ4_createStream_common_1) // LZ4_freeStream
{
    LZ4_stream_t *stream = LZ4_createStream();
    ASSERT_NE((long long)stream, NULL);
    EXPECT_EQ(LZ4_freeStream(stream), 0);
}

/*********************************************
 * End of LZ4_createStream
 *********************************************/

/*********************************************
 * "Begin" of LZ4_createStream
 *********************************************/

TEST(LZ4_freeStream, AOCL_Compression_lz4_LZ4_freeStream_common_1) // NULL_ptr
{
    EXPECT_EQ(LZ4_freeStream(NULL), 0);
}

TEST(LZ4_freeStream, AOCL_Compression_lz4_LZ4_freeStream_common_2) // free_legitimate_stream
{
    LZ4_stream_t *stream = LZ4_createStream();
    ASSERT_NE((long long)stream, NULL);
    EXPECT_EQ(LZ4_freeStream(stream), 0);
}

/*********************************************
 * End of LZ4_createStream
 *********************************************/

/*********************************************
 * "Begin" of LZ4_resetStream_fast
 *********************************************/

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_1) // Legitimate_ptr
{
    LZ4_stream_t *stream = (LZ4_stream_t *)malloc(sizeof(LZ4_stream_t));
    ASSERT_NE((long long)stream, NULL);
    memset(stream, 0, sizeof(LZ4_stream_t)); 
    EXPECT_NO_THROW(LZ4_resetStream_fast(stream));
    EXPECT_NO_THROW(LZ4_freeStream(stream));
}

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_2) // LZ4_createStream
{
    LZ4_stream_t *stream = LZ4_createStream();
    EXPECT_NO_THROW(LZ4_resetStream_fast(stream));
    EXPECT_NO_THROW(LZ4_freeStream(stream));
}

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_3) // input_NULL
{
    EXPECT_NO_THROW(LZ4_resetStream_fast(NULL));
}
/*********************************************
 * End of LZ4_resetStream_fast
 *********************************************/

/*********************************************
 * "Begin" of LZ4_loadDict
 *********************************************/

TEST(LZ4_loadDict, AOCL_Compression_lz4_LZ4_loadDict_common_1) // stream_NULL
{
    char dict[11] = "helloWorld";
    EXPECT_EQ(LZ4_loadDict(NULL, dict, 10), -1);
}

TEST(LZ4_loadDict, AOCL_Compression_lz4_LZ4_loadDict_common_2) // dictionary_NULL
{
    LZ4_stream_t *stream = LZ4_createStream();
    int dictSize = 66000;
    char *dict = (char *)malloc(dictSize);
    // Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    
    EXPECT_EQ(LZ4_loadDict(stream, NULL, dictSize), -1);
    
    LZ4_freeStream(stream);
    free(dict);
}

TEST(LZ4_loadDict, AOCL_Compression_lz4_LZ4_loadDict_common_3) // dictSize_7
{
    LZ4_stream_t *stream = LZ4_createStream();
    int dictSize = 7;
    char *dict = (char *)malloc(dictSize);
    // Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    
    EXPECT_EQ(LZ4_loadDict(stream, dict, dictSize), 0);
    EXPECT_NE(stream->internal_donotuse.dictionary, (unsigned char *)dict);

    LZ4_freeStream(stream);
    free(dict);
}

TEST(LZ4_loadDict, AOCL_Compression_lz4_LZ4_loadDict_common_4) // dictSize_66000
{
    LZ4_stream_t *stream = LZ4_createStream();
    int dictSize = 66000;
    char *dict = (char *)malloc(dictSize);
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    
    EXPECT_EQ(LZ4_loadDict(stream, dict, dictSize), 64*1024);
    LZ4_stream_t_internal* internal_p = &(stream->internal_donotuse);
    EXPECT_EQ(internal_p->dictionary, (unsigned char *)(dict+(dictSize-64*1024)));
    EXPECT_EQ(internal_p->dictSize, 64*1024);
    EXPECT_EQ(internal_p->tableType, 2); // byU32(tableType_t)

    LZ4_freeStream(stream);
    free(dict);
}

TEST(LZ4_loadDict, AOCL_Compression_lz4_LZ4_loadDict_common_5) // dictSize_1024
{
    LZ4_stream_t *stream = LZ4_createStream();
    int dictSize = 1024;
    char *dict = (char *)malloc(dictSize);
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    
    EXPECT_EQ(LZ4_loadDict(stream, dict, dictSize), 1024);
    LZ4_stream_t_internal* internal_p = &(stream->internal_donotuse);
    EXPECT_EQ(internal_p->dictionary, (unsigned char *)dict);
    EXPECT_EQ(internal_p->dictSize, 1024);
    EXPECT_EQ(internal_p->tableType, 2); // byU32(tableType_t)
    
    LZ4_freeStream(stream);
    free(dict);
}

/*********************************************
 * End of LZ4_loadDict
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compress_fast_continue
 *********************************************/

class LLZ4_compress_fast_continue : public LLZ4_compress_fast_extState
{
};

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_1) // lz4_stream_NULL
{
    setSrcSize(1024);
    setDstSize(10000);
    EXPECT_EQ(LZ4_compress_fast_continue(NULL, src, dst, srcSize, dstSize, 50), -1);
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_2) // source_NULL
{
    setSrcSize(1024);
    setDstSize(10000);
    EXPECT_EQ(LZ4_compress_fast_continue(state, NULL, dst, srcSize, dstSize, 50), -1);
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_3) // dest_NULL
{
    setSrcSize(1024);
    setDstSize(10000);
    EXPECT_EQ(LZ4_compress_fast_continue(state, src, NULL, srcSize, dstSize, 50), -1);
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_4) // input_sz1
{
    setSrcSize(1024);
    setDstSize(10000);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 50);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_5) // input_sz2
{
    setSrcSize(5120);
    setDstSize(10000);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 50);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_6) // acceleration_greater_max
{
    setSrcSize(100000);
    setDstSize(150000);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 600000);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_7) // acceleration_less_than_min
{
    setSrcSize(100000);
    setDstSize(150000);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, -100);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_8) // invalidate_tiny_dictionaries
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 3;
    state->internal_donotuse.dictSize = dictSize;

    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_9) // prefix_mode_1
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 100;
    char dict[200];
    memcpy(&dict[100], src, 100);
    
    for (int i = 0; i < 100; i++)
    {
        dict[i] = i % 255;
    }
    
    LZ4_loadDict(state, dict, dictSize);
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is less than 64kb
    int compressedLen = LZ4_compress_fast_continue(state, &dict[100], dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(&dict[100], srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_10) // prefix_mode_2
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 64 * 1024;
    char dict[dictSize+srcSize];
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    memcpy(&dict[dictSize],src,srcSize);
    LZ4_loadDict(state, dict, dictSize);
    state->internal_donotuse.dictCtx = (LZ4_stream_t_internal *)malloc(sizeof(LZ4_stream_t_internal));
    LZ4_stream_t_internal *d = (LZ4_stream_t_internal *)state->internal_donotuse.dictCtx;
    memcpy(d, state, sizeof(LZ4_stream_t_internal));
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is equal to 64kb
    int compressedLen = LZ4_compress_fast_continue(state, &dict[dictSize], dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(&dict[dictSize], srcSize, dst, compressedLen));
    
    free(d);
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_11) // using_LoadDict_less_64kb
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 100;
    char dict[dictSize];
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDict(state, dict, dictSize);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_12) // using_LoadDict_64kb
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 64 * 1024;
    char dict[dictSize];
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDict(state, dict, dictSize);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_13) // external_dictionary_mode
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 100;
    char dict[dictSize];
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDict(state, dict, dictSize);
    state->internal_donotuse.dictCtx = (LZ4_stream_t_internal *)malloc(sizeof(LZ4_stream_t_internal));
    LZ4_stream_t_internal *d = (LZ4_stream_t_internal *)state->internal_donotuse.dictCtx;
    memcpy(d, state, sizeof(LZ4_stream_t_internal));
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 30);
    
    EXPECT_NE(compressedLen, 0);
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
    
    free(d);
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_14) // external_dictionary_mode_64kb_dict_size
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 64 * 1024;
    char dict[dictSize];
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDict(state, dict, dictSize);
    state->internal_donotuse.dictCtx = (LZ4_stream_t_internal *)malloc(sizeof(LZ4_stream_t_internal));
    LZ4_stream_t_internal *d = (LZ4_stream_t_internal *)state->internal_donotuse.dictCtx;
    memcpy(d, state, sizeof(LZ4_stream_t_internal));
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
    
    free(d);
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_15) // overlapping_source_dict
{
    setSrcSize(150);
    setDstSize(200);
    int dictSize = 100;
    char dict[200];
    
    memcpy(dict, src, 100);
    for (int i = 150; i < 200; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDict(state, &dict[100], dictSize);
    
    int compressedLen = LZ4_compress_fast_continue(state, dict, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
}

TEST_F(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_common_16) // overlap_greater_than_64kb
{
    setSrcSize(150);
    setDstSize(200);
    int dictSize = 65 * 1024;
    char dict[150 + dictSize];
    
    memcpy(dict, src, srcSize);
    for (int i = 150; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDict(state, &dict[100], dictSize);
    state->internal_donotuse.dictionary = (const LZ4_byte *)&dict[100]; //
    state->internal_donotuse.dictSize += 100;
    
    int compressedLen = LZ4_compress_fast_continue(state, dict, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
}

/*********************************************
 * End of LZ4_compress_fast_continue
 *********************************************/

/*********************************************
 * "Begin" of LZ4_saveDict
 *********************************************/

/*
    Fixture class for testing `LZ4_saveDict` function.
*/
class LLZ4_saveDict : public ::testing::Test
{
protected:
    
    LZ4_stream_t *stream = NULL;
    char *dict = NULL;
    int dictSize;
    
    // Initialize a LZ4_stream ptr.
    void SetUp() override
    {
        stream = LZ4_createStream();
        LZ4_resetStream_fast(stream);
    }
    
    // Initialize or reset a `dict` buffer.
    int_fast64_t loadDict(int sz)
    {
        if(dict)
            free(dict);
        dictSize = sz;
        dict = (char *)malloc(dictSize);
        for(int i=0; i<sz; i++)
        {
            dict[i] = i % 256;
        }
        return LZ4_loadDict(stream, dict, dictSize);
    }
    
    // Destructor function of `LLZ4_saveDict` class.
    ~LLZ4_saveDict()
    {
        if (dict != NULL)
            free(dict);
        if (stream != NULL)
            free(stream);
    }
};

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_common_1) // dictonary_size_greater_64kb
{
    char *data = (char *)malloc(100000);
    
    for(int i=0; i<100000; i++)
    {
        data[i] = i % 256;
    }

    EXPECT_EQ(loadDict(66000),64 * 1024);
    EXPECT_EQ(LZ4_saveDict(stream, data, dictSize), 64 * 1024);

    LZ4_stream_t_internal *dic = &(stream->internal_donotuse);

    EXPECT_EQ(dic->dictSize, 64 * 1024);
    EXPECT_EQ(memcmp(dic->dictionary, data, 64 * 1024), 0);

    free(data);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_common_2) // dictionary_and_dictSize_is_zero
{
    EXPECT_EQ(LZ4_saveDict(stream, NULL, 0), 0);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_common_3) // stream_NULL
{
    char dict[11]="helloWorld";
    EXPECT_EQ(LZ4_saveDict(NULL, dict, 10), -1);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_common_4) // dictionary_is_NULL_dictSize_not_zero
{
    EXPECT_EQ(LZ4_saveDict(stream, NULL, 3), -1);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_common_5) // dictionary_size_greater_than_parameter
{
    loadDict(10000);
    char *data = (char *)malloc(100000);
    for(int i=0; i<100000; i++)
        data[i]=i;
    
    EXPECT_EQ(LZ4_saveDict(stream, data, 100000), 10000);
    LZ4_stream_t_internal* internal_p = &(stream->internal_donotuse);
    EXPECT_EQ(internal_p->dictSize, dictSize);
    EXPECT_EQ(memcmp(internal_p->dictionary, data, internal_p->dictSize), 0);
    
    free(data);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_common_6) // no_loaded_dictionary
{
    char data[11] = "helloWorld";
    
    EXPECT_EQ(LZ4_saveDict(stream, data, 0), 0);
    
    LZ4_stream_t_internal *dic = &(stream->internal_donotuse);
    EXPECT_EQ(dic->dictSize, 0);
}

/*********************************************
 * End of LZ4_saveDict
 *********************************************/

/*********************************************
 * "Begin" of LZ4_createStreamDecode
 *********************************************/

TEST(LZ4_createStreamDecode, AOCL_Compression_lz4_LZ4_createStreamDecode_common_1) // Not_NULL
{
    LZ4_streamDecode_t *lz = LZ4_createStreamDecode();
    
    ASSERT_NE((long long)lz, NULL);
    
    LZ4_freeStreamDecode(lz);
}

/*********************************************
 * End of LZ4_createStreamDecode
 *********************************************/

/*********************************************
 * "Begin" of LZ4_freeStreamDecode
 *********************************************/

TEST(LZ4_freeStreamDecode, AOCL_Compression_lz4_LZ4_freeStreamDecode_common_1) // NULL_INPUT
{
    EXPECT_EQ(LZ4_freeStreamDecode(NULL), 0);
}

TEST(LZ4_freeStreamDecode, AOCL_Compression_lz4_LZ4_freeStreamDecode_common_2) // Not_NULL_INPUT
{
    LZ4_streamDecode_t *lz = LZ4_createStreamDecode();
    EXPECT_NE((long long)lz, NULL);
    EXPECT_EQ(LZ4_freeStreamDecode(lz), 0);
}

/*********************************************
 * End of LZ4_freeStreamDecode
 *********************************************/

/*********************************************
 * "Begin" of LZ4_setStreamDecode
 *********************************************/

TEST(LZ4_setStreamDecode, AOCL_Compression_lz4_LZ4_setStreamDecode_common_1) // Not_NULL_INPUT
{
    LZ4_streamDecode_t *lz = LZ4_createStreamDecode();
    int dictSize = 1024;
    char *dict = (char *)malloc(dictSize);
    
    EXPECT_EQ(LZ4_setStreamDecode(lz, dict, dictSize), 1);

    LZ4_freeStreamDecode(lz);
    free(dict);
}

TEST(LZ4_setStreamDecode, AOCL_Compression_lz4_LZ4_setStreamDecode_common_2) // LZ4_streamDecode_NULL
{
    LZ4_streamDecode_t* lz = LZ4_createStreamDecode();
    int dictSize = 1024;
    char* dict = (char*)malloc(dictSize);
    
    EXPECT_EQ(LZ4_setStreamDecode(NULL, dict, dictSize), -1);
    
    LZ4_freeStreamDecode(lz);
    free(dict);
}

/*********************************************
 * End of LZ4_setStreamDecode
 *********************************************/

/*********************************************
 * "Begin" of LZ4_decoderRingBufferSize
 *********************************************/

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_common_1) // INPUT_MINUS_1
{
    EXPECT_EQ(LZ4_decoderRingBufferSize(-1), 0);
}

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_common_2) // INPUT_10
{
    EXPECT_EQ(LZ4_decoderRingBufferSize(10), 65566);
}

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_common_3) // INPUT_INT_MAX
{
    EXPECT_EQ(LZ4_decoderRingBufferSize(INT_MAX), 0);
}

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_common_4) // INPUT_128
{
    EXPECT_EQ(LZ4_decoderRingBufferSize(100000), 165550);
}
/*********************************************
 * End of LZ4_decoderRingBufferSize
 *********************************************/

/*********************************************
 * "Begin" of LZ4_decompress_safe_continue
 *********************************************/

class LLZ4_decompress_safe_continue : public LLZ4_decompress_safe
{
};

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_1) // stream_NULL
{
    EXPECT_EQ(LZ4_decompress_safe_continue(NULL, src, output, srcLen, outLen), -1);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_2) // src_NULL
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    EXPECT_EQ(LZ4_decompress_safe_continue(decode, NULL, output, srcLen, outLen), -1);
    
    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_3) // dest_NULL
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    EXPECT_EQ(LZ4_decompress_safe_continue(decode, src, NULL, srcLen, outLen), -1);
    
    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_4) // simple_pass_case1
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    
    EXPECT_EQ(0, memcmp(output, original, decompress));
    
    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_5) // simple_pass_case2
{   
    int outlen_2 = 1000;
    char *output2 = (char *)malloc(outlen_2);
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();

    // This is executed to make prefixSize != 0.
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    
    EXPECT_EQ(0, memcmp(output, original, decompress));
    
    decompress = LZ4_decompress_safe_continue(decode, src, output2, srcLen, outlen_2);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output2, original, decompress));
    
    LZ4_freeStreamDecode(decode);
    free(output2);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_6) // Overflow
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();

    // When overflow occurs the return value would be less than zero.
    EXPECT_LT(LZ4_decompress_safe_continue(decode, src, output, srcLen, 400),0);

    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_7) // Overflow2
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    // This line is for making prefixSize != 0.
    ASSERT_EQ(LZ4_decompress_safe_continue(decode, src, output, srcLen, 800), origLen);
    EXPECT_EQ(memcmp(original, output, origLen), 0);
    
    int uncompressed_len = LZ4_decompress_safe_continue(decode, src, output, srcLen, 100);
    
    // When overflow occurs the return value would be less than zero.
    EXPECT_LT(uncompressed_len, 0);
    
    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_8) // Middle_if_test_case
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output, original, decompress));
    
    // The condition (lz4sd->prefixEnd == (BYTE*)dest) is made by previous decompress call
    // covers: lz4sd->extDictSize == 0 
    decompress = LZ4_decompress_safe_continue(decode, src, output + decompress, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output + decompress, original, decompress));
    
    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_9) // Middle_if_test_case2
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output, original, decompress));
    
    // The condition (lz4sd->prefixEnd == (BYTE*)dest) is made by previous decompress call
    // covers: lz4sd->extDictSize == 0 , and we expect return value to be less than zero.
    int uncompressed_len = LZ4_decompress_safe_continue(decode, src, output + decompress, srcLen, 100);
    EXPECT_LT(uncompressed_len, 0);

    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_10) // Middle_if_test_case3
{
    setOrigSz(1024 * 4);
    setDstSz(4 * 1024 * 4);

    // dict allocation
    char *dict = (char *)malloc(1024);
    for(int i=0;i<1024;i++)
    {
        dict[i] = i % 256;
    }

    // compression
    LZ4_stream_t *lz = LZ4_createStream();
    LZ4_resetStream_fast(lz);
    LZ4_loadDict(lz, dict, 1024);
    
    srcLen = LZ4_compress_fast_continue(lz, original, src, origLen, srcLen, 1);
    EXPECT_NE(srcLen, 0);

    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    LZ4_setStreamDecode(decode, dict, 1024);
    
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
    
    // here prefixSize == 64kb and extDictSize != 0
    EXPECT_EQ(LZ4_decompress_safe_continue(decode, src, output + decompress, srcLen, outLen - decompress), origLen);
    EXPECT_EQ(0, memcmp(output + decompress, original, origLen));
    
    free(dict);
    LZ4_freeStream(lz);
    LZ4_freeStreamDecode(decode);
}

TEST_F(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_common_11) // Middle_if_test_case4
{
    setOrigSz(67000);
    setDstSz(77000);

    // compression
    int dLen = 64 * 1024;
    LZ4_stream_t *lz = LZ4_createStream();
    LZ4_resetStream_fast(lz);
    char dict_and_dest[dLen + outLen];
    
    for (int i = 0; i < dLen; i++)
    {
        dict_and_dest[i] = rand() % 255;
    }
    LZ4_loadDict(lz, dict_and_dest, dLen);
    
    srcLen = LZ4_compress_fast_continue(lz, original, src, origLen, srcLen, 1);
    EXPECT_NE(srcLen, 0);

    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    LZ4_setStreamDecode(decode, dict_and_dest, dLen);
    
    // here prefixSize == 64kb
    int decompress = LZ4_decompress_safe_continue(decode, src, &dict_and_dest[dLen], srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(&dict_and_dest[dLen], original, origLen));

    LZ4_freeStream(lz);
    LZ4_freeStreamDecode(decode);
}

/*********************************************
 * End of LZ4_decompress_safe_continue
 *********************************************/

/*********************************************
 * "Begin" of LZ4_decompress_safe_usingDict
 *********************************************/

/*
    This is a fixture class for testing `LZ4_decompress_safe_usingDict` function.
    Initialization and destruction of data for testing `LZ4_decompress_safe_usingDict`
    is taken care by this fixture.
*/
class LLZ4_decompress_safe_usingDict : public ::testing::Test
{
public:
    // Compressed data is contained in `src` buffer, this acts as a source
    // for decompression.
    char *src = NULL;
    int srcLen;
    // Original data which is about to be compressed is contained in `original` buffer.
    char *original = NULL;
    int origLen;
    //
    char *output = NULL;
    int outLen;
    // `dict` points to Dictionary buffer.
    char *dict = NULL;
    int dictLen;
    LZ4_stream_t *stream = NULL;

    // This is function is called before the beginning of every test case.
    void SetUp() override
    {
        origLen = 100000;
        srcLen = 200000;
        outLen = 150000;
        original = (char *)malloc(origLen);
        src = (char *)malloc(srcLen);
        output = (char *)malloc(outLen);
        // Random data is initilized in `original` buffer. 
        for (int i = 0; i < origLen; i++)
        {
            original[i] = rand() % 250;
        }
        stream = LZ4_createStream();
        LZ4_resetStream_fast(stream);
    }
    
    // Initialization or reseting of dictionary size is done through this function.
    void setDictSz(int sz)
    {
        if(dict)
            free(dict);
        dictLen = sz;
        dict = (char *)malloc(dictLen);
        for (int i = 0; i < dictLen; i++)
            dict[i] = rand() % 255;
    }
    
    // Destructor function of `LLZ4_decompress_safe_usingDict` class.
    ~LLZ4_decompress_safe_usingDict()
    {
        free(src);
        free(output);
        free(dict);
        LZ4_freeStream(stream);
        free(original);
    }
};

TEST_F(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_common_1) // src_NULL
{
    setDictSz(64000);
    char out[dictLen + outLen];
    LZ4_loadDict(stream, dict, dictLen);
    
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    memcpy(out, dict, dictLen);
    
    ASSERT_EQ(LZ4_decompress_safe_usingDict(NULL, output, srcLen, outLen, dict, dictLen), -1);
}

TEST_F(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_common_2) // output_NULL
{
    setDictSz(64000);
    LZ4_loadDict(stream, dict, dictLen);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    ASSERT_EQ(LZ4_decompress_safe_usingDict(src, NULL, srcLen, outLen, dict, dictLen), -1);
}

TEST_F(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_common_3) // Pass
{
    srcLen = LZ4_compress_default(original, src, origLen, srcLen);
    EXPECT_EQ(LZ4_decompress_safe_usingDict(src, output, srcLen, outLen, NULL, 0), origLen);
}

TEST_F(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_common_4) // PassUsingDict
{
    setDictSz(1024);
    
    LZ4_loadDict(stream, dict, dictLen);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    
    ASSERT_EQ(LZ4_decompress_safe_usingDict(src, output, srcLen, outLen, dict, dictLen), origLen);
    EXPECT_EQ(memcmp(output, original, origLen), 0);
}

TEST_F(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_common_5) // PassUsingDict_size_65000
{
    setDictSz(65000);
    char out[dictLen + outLen];
    
    LZ4_loadDict(stream, dict, dictLen);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    memcpy(out, dict, dictLen);
    
    ASSERT_EQ(LZ4_decompress_safe_usingDict(src, out + dictLen, srcLen, outLen, out, dictLen), origLen);
    EXPECT_EQ(memcmp(out + dictLen, original, origLen), 0);
}

TEST_F(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_common_6) // PassUsingDict_size_64000
{
    setDictSz(64000);
    char out[dictLen + outLen];

    LZ4_loadDict(stream, dict, dictLen);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    memcpy(out, dict, dictLen);
    
    ASSERT_EQ(LZ4_decompress_safe_usingDict(src, out + dictLen, srcLen, outLen, out, dictLen), origLen);
    EXPECT_EQ(memcmp(out + dictLen, original, origLen), 0);
}


/*********************************************
 * End of LZ4_decompress_safe_usingDict
 *********************************************/