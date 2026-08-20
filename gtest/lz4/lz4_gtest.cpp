/**
 * Copyright (C) 2023-2026, Advanced Micro Devices. All rights reserved.
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
 
 /** @file lz4_gtest.cpp
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

#ifndef LZ4_STATIC_LINKING_ONLY
#define LZ4_STATIC_LINKING_ONLY
#endif
#include "algos/lz4/lz4.h"

#ifdef AOCL_TEST_FUZZER
#include "fuzztest/fuzztest.h"
#endif

#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

#include "gtest_utils.h"

using namespace std;

#define DEFAULT_OPT_LEVEL 2 // system running gtest must have AVX support
/*
 * LZ4_ACCELERATION_DEFAULT :
 * Any "acceleration" value less than this threshold
 * get treated as LZ4_ACCELERATION_DEFAULT instead.
 */
#define LZ4_ACCELERATION_DEFAULT 1
/*
 * LZ4_ACCELERATION_MAX :
 * Any "acceleration" value higher than this threshold
 * get treated as LZ4_ACCELERATION_MAX instead.
 */
#define LZ4_ACCELERATION_MAX 65537
#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

/*
    This class is used for allocating and deallocating memory,
    which acts as a common way for instantiation input and output buffer
    for most of the test cases.    
*/
class TestLoad
{
private:
    gtest_data_gen_t* data_gen = NULL;

public:
    TestLoad(size_t sz, gtest_data_gen_type type = gtest_data_gen_type::random)
    {
        data_gen = new gtest_data_gen_t(LZ4_compressBound, sz, type);
    }

    TestLoad(size_t inp_sz, size_t out_sz, gtest_data_gen_type type = gtest_data_gen_type::random)
    {
        data_gen = new gtest_data_gen_t(LZ4_compressBound, inp_sz, out_sz, type);
    }

    ~TestLoad() {
        delete data_gen;
    }

    char* getOrigData()
    {
        return data_gen->getOrigBuff();
    }

    size_t getOrigSize()
    {
        return data_gen->getOrigSize();
    }

    char* getCompressedBuff()
    {
        return data_gen->getCompressedBuff();
    }

    size_t getCompressedSize()
    {
        return data_gen->getCompressedSize();
    }
};

/* This base class can be used for all fixtures
* that require dynamic dispatcher setup */
class AOCL_setup_lz4 : public OPT_LEVEL_TESTS {
public:
    AOCL_setup_lz4() {
        aocl_destroy_lz4();
        DynamicDispatch opt = GetParam();
        aocl_setup_lz4(opt.optOff, opt.optLevel, 0, 0, 0);
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
    int uncompressedLen = srcSize;
    char* uncompressed = (char*)malloc(uncompressedLen * sizeof(char));

    int uncompressedLenRes = LZ4_decompress_safe(compressed, uncompressed, compressedLen, uncompressedLen);

    if (uncompressedLenRes < 0) {//error code
        free(uncompressed);
        return false;
    }

    if (!(srcSize == (unsigned)uncompressedLenRes)) {
        free(uncompressed);
        return false;
    }

    bool ret = (memcmp(src, uncompressed, srcSize) == 0);
    free(uncompressed);
    return ret;
}

/***********************************************
 * "Begin" of Version Tests
 ***********************************************/

TEST(LZ4_versionNumber, AOCL_Compression_lz4_LZ4_versionNumber_pass_common_1) // Number
{
    EXPECT_EQ(LZ4_versionNumber(), LZ4_VERSION_NUMBER);
}

TEST(LZ4_versionString, AOCL_Compression_lz4_LZ4_versionString_pass_common_1) // String
{
    EXPECT_STREQ(LZ4_versionString(), LZ4_VERSION_STRING);
}
/*********************************************
 * End of Version Tests
 *********************************************/

/*********************************************
 * Compression Tests Begin
 *********************************************/
class LZ4_compress_default_test : public AOCL_setup_lz4 {
};
TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_1) // compressFail_src_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_default(NULL, d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize()), 0);
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_2) // compressFail_dst_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_default(d.getOrigData(), NULL, d.getOrigSize(), d.getCompressedSize()), 0);
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_pass_common_3) // compressionPass
{
    TestLoad d(800);

    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize());
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_4) // compressFail_dst_size_not_enough
{
    TestLoad d(800, 400 /* dst_size */);

    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0);
    EXPECT_FALSE(check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_pass_common_5) // srcSize<13_all_literals
{
    TestLoad d(12);

    // Input too small, no compression (all literals)
    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize());
    EXPECT_EQ(outLen, d.getOrigSize() + 1);  // token byte + 12 literals
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_pass_common_6) // src_NULL_inputSize_0
{
    TestLoad d(0);

    int outLen = LZ4_compress_default(NULL, d.getCompressedBuff(), 0, d.getCompressedSize());
    // generates an empty block
    EXPECT_EQ(outLen, 1);
    EXPECT_EQ(d.getCompressedBuff()[0], 0);
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_7) // src_NULL_inputSize_0_dstCapacity_0
{
    TestLoad d(0);

    // no output, can't write anything
    EXPECT_EQ(LZ4_compress_default(NULL, d.getCompressedBuff(), 0, 0), 0);

}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_8) // negative_size
{
    TestLoad d(800);

    // negative srcLen: Unsupported srcSize, too large or negative
    int srcLen = -1;
    int dstLen = d.getOrigSize();
    EXPECT_EQ(LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), srcLen, dstLen), 0);
    
    // negative dstLen: cannot compress within `dst` budget. 
    //                  Stored indexes in hash table are nonetheless fine
    srcLen = d.getOrigSize();
    dstLen = -1;
    EXPECT_EQ(LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), srcLen, dstLen), 0);

}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_9) // Edge case for using `AOCL_LZ4_wildCopy16` instead of `LZ4_wildCopy8`.
{
    vector<char> source = {(char)(9), (char)(62), (char)(165), (char)(148), (char)(148), (char)(148), (char)(13), (char)(0), (char)(161), (char)(44), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(147), (char)(147), (char)(147), (char)(147), (char)(147), (char)(188), (char)(210), (char)(133), (char)(210), (char)(242), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210), (char)(169), (char)(218), (char)(210), (char)(210), (char)(210), (char)(210), (char)(210)};
    bool atLeastOneSuccess = false;
    for(int destLen = 1; destLen <= LZ4_compressBound(source.size()); destLen++)
    {
        int srcLen = source.size();
        vector<char> dest(destLen, 0);
        int compressedLength = LZ4_compress_default((const char *)source.data(), dest.data(), srcLen, destLen);
        if(compressedLength > 0)
        {
            atLeastOneSuccess = true;
            EXPECT_TRUE(check_uncompressed_equal_to_original(source.data(), srcLen, dest.data(), compressedLength));
        }
    }
    EXPECT_TRUE(atLeastOneSuccess);
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_10) // Edge case for using `AOCL_LZ4_wildCopy16` instead of `LZ4_wildCopy8`.
{
    vector<char> source = {'\254', '\240', 'T', 'R', 'Y', '\335', 'R', 'D', '\311', '\311', '\311', '\311', '\311', '\311', '\311', '\311', 'R', '\311', 'x', '\311', '\311', '\311', '\206', '\311', '\311', '\311', '\311', '_', '\232', '\253', '\232', '\232', '\242', '\242', '\242', '\232', '\232', '\232', '\232', '\275', '\275', '\275', '\232', '\232', '\232', '\232', '\232', '\244', '\244', '\244', '\244', '\244', '\244', '\244', '\244', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\021', '\244', 'T', 'I', 'G', '_', '\005', 'S', 'E', 'S', 'S', 'N'};
    bool atLeastOneSuccess = false;
    for(int destLen = 1; destLen <= LZ4_compressBound(source.size()); destLen++)
    {
        int srcLen = source.size();
        vector<char> dest(destLen, 0);
        int compressedLength = LZ4_compress_default((const char *)source.data(), dest.data(), srcLen, destLen);
        if(compressedLength > 0)
        {
            atLeastOneSuccess = true;
            EXPECT_TRUE(check_uncompressed_equal_to_original(source.data(), srcLen, dest.data(), compressedLength));
        }
    }
    EXPECT_TRUE(atLeastOneSuccess);
}

#ifdef AOCL_ENABLE_THREADS

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_pass_common_11) // pass_case_mt
{
    TestLoad d(8*64 KB);

    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize());
    EXPECT_NE(outLen, 0);
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_fail_common_12) // dstCapacity_inadequate_mt
{
    int srcLen = 16*64 KB;
    TestLoad d(srcLen, srcLen/2 /* dst_size */);

    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0);
}

TEST_P(LZ4_compress_default_test, AOCL_Compression_lz4_LZ4_compress_default_pass_common_13) // mt_compression_st_decompression
{
    TestLoad d(8*64 KB);

    int outLen = LZ4_compress_default(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize());
    EXPECT_NE(outLen, 0);

    int rap_metadata_len = aocl_skip_rap_frame_mt(d.getCompressedBuff(), outLen);

    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff() + rap_metadata_len, outLen - rap_metadata_len));
}

#endif

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LZ4_compress_default_test,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

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
class LLZ4_decompress_safe : public AOCL_setup_lz4
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
        EXPECT_GT(srcLen, 0);
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
        EXPECT_GT(srcLen, 0);
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

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_1) // src_NULL
{
    EXPECT_EQ(LZ4_decompress_safe(NULL, output, srcLen, origLen), -1);
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_2) // dst_NULL
{
    EXPECT_EQ(LZ4_decompress_safe(src, NULL, srcLen, origLen), -1);
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_3) // successfull_decompression
{
    int decLen = LZ4_decompress_safe(src, output, srcLen, origLen);
    ASSERT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_4) // dstCapacity_less_by_x_bytes
{
    int x = 5;
    for(int i=origLen-x; i<origLen; i++)
        output[i] = 0;

    EXPECT_LT(LZ4_decompress_safe(src, output, srcLen, origLen-x), 0);

    // should not overrun the `output` buffer
    for(int i=origLen-x; i<origLen; i++)
        EXPECT_EQ(output[i], 0);
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_5) // dstCapacity_0
{
    EXPECT_EQ(LZ4_decompress_safe(src, output, srcLen, 0), -1);
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_6) // dstCapacity_0_srcLen_1
{
    // srcLen == 1 && *src == 0
    char inp = 0;
    EXPECT_EQ(LZ4_decompress_safe(&inp, output, 1, 0), 0);
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_7) // inp_siz_zero
{
    EXPECT_EQ(LZ4_decompress_safe(src, output, 0, origLen), -1);
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_8) // exact_input_not_consumed
{
    // Encounters error: "exact input not consumed" 
    // inp_siz_more
    int decLen = LZ4_decompress_safe(src, output, srcLen+1, origLen);
    EXPECT_LT(decLen, 0);

    // inp_size_less
    decLen = LZ4_decompress_safe(src, output, srcLen-1, origLen);
    EXPECT_LT(decLen, 0);
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_9) // dstCapacity_<_FASTLOOP_SAFE_DISTANCE
{
    setOrigSz(63);
    setDstSz(63*2 + 10);
    int decLen = LZ4_decompress_safe(src, output, srcLen, origLen);
    ASSERT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_10) // input_<_5
{
    int orig_len = 3;
    setOrigSz(orig_len);
    setDstSz(2*orig_len + 10);
    // Special : if input is smaller than 5 bytes, there is only one sequence, 
    //           it contains the whole input as literals.

    EXPECT_EQ(srcLen, orig_len + 1);
    EXPECT_EQ(memcmp(original, src + 1, orig_len),0);

    int decLen = LZ4_decompress_safe(src, output, srcLen, outLen);
    EXPECT_EQ(decLen, orig_len);
    EXPECT_EQ(0, memcmp(output, original, orig_len));
}

#ifdef AOCL_ENABLE_THREADS
TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_11) // successfull_decompression_mt
{
    int orig_len = 8*64 KB;
    setOrigSz(orig_len);
    setDstSz(orig_len);
    int decLen = LZ4_decompress_safe(src, output, srcLen, outLen);
    ASSERT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_12) // dstCapacity_inadequate_mt
{
    int orig_len = 8*64 KB;
    setOrigSz(orig_len);
    setDstSz(orig_len/20);
    int decLen = LZ4_decompress_safe(src, output, srcLen, outLen);
    EXPECT_LT(decLen, 0);
}

#ifdef AOCL_LZ4_AVX_OPT
TEST_P(LLZ4_decompress_safe, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_13) // mt_post_match_copy_guard
{
    /* Non-last MT partitions may legally end after a match sequence. */
    const unsigned char compressed[] = {
        0x40,                   /* 4 literal bytes, 4-byte match */
        'a', 'b', 'c', 'd',
        0x04, 0x00              /* offset = 4 */
    };
    const char expected[] = "abcdabcd";
    const char next_partition_output[] = "WXYZ1234";
    const int decoded_len = 8;
    char dst[32];
    const int dst_capacity = (int)sizeof(dst);

    memset(dst, 0, sizeof(dst));
    memcpy(dst + decoded_len, next_partition_output, sizeof(next_partition_output) - 1);

    int decLen = Test_AOCL_LZ4_decompress_generic_mt(
        (const char*)compressed, dst, (int)sizeof(compressed), dst_capacity,
        0 /* non-last MT partition */);

    ASSERT_EQ(decoded_len, decLen);
    EXPECT_EQ(0, memcmp(dst, expected, decoded_len));
    EXPECT_EQ(0, memcmp(dst + decoded_len, next_partition_output,
                        sizeof(next_partition_output) - 1));
}
#endif /* AOCL_LZ4_AVX_OPT */
#endif

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_decompress_safe,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));


class LLZ4_decompress_safe_curated_input : public AOCL_setup_lz4
{
public:

    // Decompressed data will be stored in the buffer `output`.
    char *output = NULL;
    int outLen;

    // Set `output` buffer size to `sz`.
    void setDstSz(int sz)
    {
        outLen = sz;
        output = (char *)malloc(outLen);
    }

    // Destructor function.
    ~LLZ4_decompress_safe_curated_input()
    {
        if (output)
        {
            free(output);
            output = NULL;
        }
    }
};

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_1) // inp_data_contains_errors
{
    const int origLen = 100;
    char orig[origLen];
    int dstCapacity = LZ4_compressBound(origLen);
    char dst[dstCapacity];

    for (int i = 0; i < 100; i++)
        orig[i] = 'a';

    dstCapacity = LZ4_compress_default(orig, dst, origLen, dstCapacity);
    EXPECT_GT(dstCapacity, 0);
    dst[2] = 2;

    EXPECT_LT(LZ4_decompress_safe(dst, output, dstCapacity, origLen), 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_2) // last_lit_bytes_missing
{
    int x = 154;
    const int origLen = 2*x;

    char orig[origLen];
    int dstCapacity = LZ4_compressBound(origLen);
    char dst[dstCapacity];

    for (int i = 0; i < x; i++)
        orig[i] = i;

    memcpy(&orig[x], &orig, x);

    dstCapacity = LZ4_compress_default(orig, dst, origLen, dstCapacity);
    EXPECT_GT(dstCapacity, 0);
    setDstSz(origLen);

    // Last sequence: 1 byte of token followed by 5 bytes of uncompressed literal.
    // Encounters error: "Last LASTLITERALS bytes must be uncompressed literals".
    int uncompressedLen = LZ4_decompress_safe(dst, output, dstCapacity, origLen-6);
    EXPECT_LT(uncompressedLen, 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_3) // rvl_error
{
    int src_len = 16;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xF0;                    // token byte : 1111 0000
    compressed_src[1] = 0;                             // additional literal length
    for(int i=2; i<14; i++)                            // literals   [ Number of literals stored
        compressed_src[i] = i % 255;                   //              is less than literal length ]

    LZ4_writeLE16_wrapper(&compressed_src[14], 10);    // offset

    // rvl (read variable length) error: read limit reached while decoding additional literal length
    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_4) // rvl_error_2
{
    int src_len = 21;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xDF;                    // token byte : 1101 1111
    for(int i=1; i<14; i++)                            // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[14], 10);    // offset
    compressed_src[16]= 0xFF;                          // additional match length
    compressed_src[17]= 0x0F;                          // additional match length

    // last sequence
    compressed_src[18] = (char)0x20;                   // token byte
    for(int i=19; i<21; i++)                           // last literals [ less than 5 bytes ]
        compressed_src[i] = i % 255;

    // rvl (read variable length) error: read limit reached while decoding additional match length
    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_5) // offset_outside_buffer
{
    int src_len = 24;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    // Case 1: offset = 125
    compressed_src[0] = (char)0xEF;                    // token byte : 1110 1111
    for(int i=1; i<15; i++)                            // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[15], 125);   // offset
    compressed_src[17]= 0;                             // additional match length

    // last sequence
    compressed_src[18] = (char)0x50;                   // token byte
    for(int i=19; i<24; i++)                           // last literals
        compressed_src[i] = i % 255;                   

    // Error : offset outside buffers
    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_6) // lit_len_>_src_&_dst_len
{
    int src_len = 24;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xFF;                    // token byte : 1111 1111
    compressed_src[1] = (char)0xFE;                    // additional literal length [ 254 ]
    for(int i=2; i<15; i++)                            // literals   [ Number of literals stored is
        compressed_src[i] = i % 255;                   //              not equal to literal length ]

    LZ4_writeLE16_wrapper(&compressed_src[15], 10);    // offset
    compressed_src[17]= 0;                             // additional match length

    // last sequence
    compressed_src[18] = (char)0x50;                   // token byte
    for(int i=19; i<24; i++)                           // last literals
        compressed_src[i] = i % 255;                   

    // literal length more than src, dst buffer length
    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_7) // match_len_>_src_&_dst_len
{
    int src_len = 24;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xEF;                    // token byte : 1110 1111
    for(int i=1; i<15; i++)                            // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[15], 10);    // offset
    compressed_src[17]= (char)254;                     // additional match length [ 254 ]

    // last sequence
    compressed_src[18] = (char)0x50;                   // token byte
    for(int i=19; i<24; i++)                           // last literals
        compressed_src[i] = i % 255;                   

    // match length more than src, dst buffer length
    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_8) // last_seq_NOT_token_literals
{
    int src_len = 128;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xFE;                    // token byte : 1111 1110
    compressed_src[1] = (char)0x6D;                    // additional literal length [ 109 ]
    for(int i=2; i<126; i++)                           // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[126], 15);   // offset               

    // Last sequence: | token | (additional lit len) | literals | offset | instead of 
    //                | token | literals |
    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_9) // last_lit_len_<_5
{
    int src_len = 277;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xF7;                    // token byte : 1111 0111
    compressed_src[1] = (char)0xFE;                    // additional literal length
    for(int i=2; i<271; i++)                           // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[271], 20);    // offset

    // last sequence
    compressed_src[273] = (char)0xA0;                   // token byte      [ lit length 10 ]
    for(int i=274; i<277; i++)                          // last literals   [ but only 3 literals stored ]
        compressed_src[i] = i % 255;                                    

    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);  
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_10) // negative_additional_len
{
    int src_len = 27;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    // case 1: Additional literal length - Negative

    compressed_src[0] = (char)0xFF;                    // token byte : 1111 1111
    compressed_src[1] = -100;                          // additional literal length [ negative ]
    for(int i=2; i<17; i++)                            // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[17], 15);    // offset
    compressed_src[19]= 0;                             // additional match length [ zero ]

    // last sequence
    compressed_src[20] = (char)0x60;                   // token byte
    for(int i=21; i<27; i++)                           // last literals
        compressed_src[i] = i % 255;                                    

    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0);

    // case 2: Additional match length - Negative

    compressed_src[1] = 0;                             // additional literal length [ zero ]
    compressed_src[19] = -100;                         // additional match length [ negative ]
    
    EXPECT_LT(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 0); 
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_11) // only_last_seq
{
    int src_len = 273;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xF0;                     // token byte : 1111 0000
    compressed_src[1] = (char)255;                      // additional literal length
    compressed_src[2] = 0;                              // additional literal length
    for(int i=3; i<273; i++)                            // literals [ 270 ]
        compressed_src[i] = i % 255;

    EXPECT_EQ(LZ4_decompress_safe(compressed_src, output, src_len, outLen), 270); 
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_fail_common_12) // offset_zero
{
    int orig_len = 200;
    char orig_data[orig_len];
    int cpr_srcLen = LZ4_compressBound(orig_len);
    char cpr_src[cpr_srcLen];

    for(int i=0; i<orig_len; i++)
        orig_data[i] = 'a';

    // cpr_src:
    // | token  |    a   | offset  | additional match length | token  | last literals |
    // | 1 byte | 1 byte | 2 bytes |        1 byte           | 1 byte |    5 bytes    |

    cpr_srcLen = LZ4_compress_default(orig_data, cpr_src, orig_len, cpr_srcLen);
    EXPECT_GT(cpr_srcLen, 0);
    setDstSz(orig_len);

    LZ4_writeLE16_wrapper(&cpr_src[2], 0);      // over-write offset byte

    int decLen = LZ4_decompress_safe(cpr_src, output, cpr_srcLen, outLen);
    EXPECT_EQ(decLen, orig_len);                                            // decompressed length = original length
    EXPECT_NE(0, memcmp(output, orig_data, orig_len));                      // But, decompressed data NOT same as original data
}

TEST_P(LLZ4_decompress_safe_curated_input, AOCL_Compression_lz4_LZ4_decompress_safe_pass_common_13) // matchLength_>_=_<_offset
{
    int cpr_srcLen = 66063;
    char cpr_src[cpr_srcLen];
    setDstSz(cpr_srcLen*2);
    
    // token
    cpr_src[0] = (char)0xFF;
    
    // additional literal length
    for (int i=1; i<257; i++)
        cpr_src[i] = (char)0xFF;
    cpr_src[257] = (char)0xF5;

    // literals [ 65540 ]
    for(int i=258; i<65798; i++)
        cpr_src[i] = i % 255;

    // offset
    LZ4_writeLE16_wrapper(&cpr_src[65798], 65535);

    // case 1: matchlength ( 65536 ) > offset
    // --------------------------------------

    // additional match length [ 65521 ]
    for(int i=65800; i<66056 ; i++)
        cpr_src[i] = (char)0xFF;
    cpr_src[66056] = (char)0xF1;
    
	// Last sequence
    cpr_src[66057] = (char)0x50;
    for(int i=66058; i<66063; i++)
        cpr_src[i] = i % 255; 
    
    int decLen = LZ4_decompress_safe(cpr_src, output, cpr_srcLen, outLen);
    EXPECT_GT(decLen, 0);

    // case 2: matchlength ( 65535 ) = offset
    // --------------------------------------

    cpr_src[66056] = (char)0xF0;
    decLen = LZ4_decompress_safe(cpr_src, output, cpr_srcLen, outLen);
    EXPECT_GT(decLen, 0);

    // case 3: matchlength ( 65295 ) < offset
    // --------------------------------------

    cpr_src[66056] = (char)0x00;
    decLen = LZ4_decompress_safe(cpr_src, output, cpr_srcLen, outLen);
    EXPECT_GT(decLen, 0);

}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_decompress_safe_curated_input,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of Decompression Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compressBound
 *********************************************/

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_pass_common_1) // 65025
{
    EXPECT_EQ(LZ4_compressBound(65025), 65296);
}

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_pass_common_2) // 0
{
    EXPECT_EQ(LZ4_compressBound(0), 16);
}

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_fail_common_3) // MAX_LIMIT
{
    EXPECT_EQ(LZ4_compressBound(0x7E000000 + 1), 0);
}

TEST(LZ4_compressBound, AOCL_Compression_lz4_LZ4_compressBound_fail_common_4) // MINUS_ONE
{
    EXPECT_EQ(LZ4_compressBound(-1), 0);
}

/*********************************************
 * "End" of LZ4_compressBound Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_sizeofState
 *********************************************/

TEST(LZ4_sizeofState, AOCL_Compression_lz4_LZ4_sizeofState_pass_common_1) // NULL
{
    EXPECT_EQ(LZ4_sizeofState(), 16416);
}

/*********************************************
 * "End" of LZ4_sizeofState Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compress_fast
 *********************************************/
class LZ4_compress_fast_test : public AOCL_setup_lz4 {
};

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_fail_common_1) // src_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_fast(NULL, d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 1), 0);
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_fail_common_2) // dest_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_fast(d.getOrigData(), NULL, d.getOrigSize(), d.getCompressedSize(), 1), 0);
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_pass_common_3) // Pass
{
    TestLoad d(800);

    int compressedSize = LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 1);
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedSize));
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_fail_common_4) // OutLen_small
{
    TestLoad d(800, 400 /* dst_size */);

    EXPECT_EQ(LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 1), 0);
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_pass_common_5) // acc_greater_MAX
{
    TestLoad d(800);
    int compressedSize = LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), LZ4_ACCELERATION_MAX+1);
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedSize));
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_pass_common_6) // acc_less_MIN
{
    TestLoad d(800);

    int compressedSize = LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), LZ4_ACCELERATION_DEFAULT-1);
    EXPECT_NE(compressedSize, 0);
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedSize));
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_pass_common_7) // srcSize<13_all_literals
{
    TestLoad d(12);

    // Input too small, no compression (all literals)
    int outLen = LZ4_compress_fast(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(), d.getCompressedSize(), 1);
    EXPECT_EQ(outLen, d.getOrigSize() + 1);  // token byte + 12 literals
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_pass_common_8) // src_NULL_inputSize_0
{
    TestLoad d(0);

    int outLen = LZ4_compress_fast(NULL, d.getCompressedBuff(), 0, d.getCompressedSize(), 1);
    // generates an empty block
    EXPECT_EQ(outLen, 1);
    EXPECT_EQ(d.getCompressedBuff()[0], 0);
}

TEST_P(LZ4_compress_fast_test, AOCL_Compression_lz4_LZ4_compress_fast_fail_common_9) // src_NULL_inputSize_0_dstCapacity_0
{
    TestLoad d(0);

    // no output, can't write anything
    EXPECT_EQ(LZ4_compress_fast(NULL, d.getCompressedBuff(), 0, 0, 1), 0);

}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LZ4_compress_fast_test,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of LZ4_compress_fast Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compress_destSize
 *********************************************/
class LZ4_compress_destSize_test : public AOCL_setup_lz4 {
public:
    vector<char> genRandAlphabets(int i)
    {
        vector<char> buff;
        while(i)
        {
            buff.push_back(96+rand()%26);
            i--;
        }
        return buff;
    }
};

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_1) // src_NULL
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_destSize(NULL, d.getCompressedBuff(), &srcLen, d.getCompressedSize()), 0);
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_2) // output_NULL
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), NULL, &srcLen, d.getCompressedSize()), 0);
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_3) // srcLen_NULL
{
    TestLoad d(800);

    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), NULL, d.getCompressedSize()), 0);
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_pass_common_4) // Pass
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();
    int compressedLen = LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, d.getCompressedSize());
    // should consume the entire src buffer
    EXPECT_EQ(srcLen, d.getOrigSize());
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedLen));
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_pass_common_5) // srcLec_<_LZ4_64Klimit_&&_targetDestSize_<_srcLen
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();
    int targetDestSize = srcLen - 100;
    int compressedLen = LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, targetDestSize);
    // should not read entire src buffer
    EXPECT_LT(srcLen, d.getOrigSize());
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),srcLen,d.getCompressedBuff(),compressedLen));
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_pass_common_6) // srcLec_>=_LZ4_64Klimit_&&_targetDestSize_<_srcLen
{
    TestLoad d(65547);
    int srcLen = d.getOrigSize();
    int targetDestSize = srcLen - 100;
    int compressedLen = LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, targetDestSize);
    // should not read entire src buffer
    EXPECT_LT(srcLen, d.getOrigSize());
    EXPECT_TRUE(check_uncompressed_equal_to_original(d.getOrigData(),srcLen,d.getCompressedBuff(),compressedLen));
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_7) // targetDestSize_0
{
    TestLoad d(800);
    int srcLen = d.getOrigSize();
    // Impossible to store anything
    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, 0), 0);
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_pass_common_8) // src_NULL_srcLen_0
{
    TestLoad d(0);
    int srcLen = d.getOrigSize();
    int targetDestSize = d.getCompressedSize();
    int outLen = LZ4_compress_destSize(NULL, d.getCompressedBuff(), &srcLen, targetDestSize);
    // generates an empty block
    EXPECT_EQ(outLen,1);
    EXPECT_EQ(d.getCompressedBuff()[0],0);
    EXPECT_EQ(srcLen , 0); // inputConsumed = 0
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_9) // src_NULL_srcLen_0_targetDestSize_0
{
    TestLoad d(0);

    int srcLen = d.getOrigSize();
    int targetDestSize = srcLen;

    // no output, can't write anything
    EXPECT_EQ(LZ4_compress_destSize(NULL, d.getCompressedBuff(), &srcLen, targetDestSize), 0);

}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_10) // negative_size
{
    TestLoad d(800);

    // negative srcLen: Unsupported srcSize, too large or negative
    int srcLen = -1;
    int targetDestSize = d.getCompressedSize();
    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, targetDestSize), 0);
    
    // negative targetDestSize: Impossible to store anything
    srcLen = d.getOrigSize();
    targetDestSize = -1;
    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), &srcLen, targetDestSize), 0);

}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_11) // srcSizePtr_is_NULL
{
    TestLoad d(800);

    // src is not NULL and srcSizePtr is NULL.
    EXPECT_EQ(LZ4_compress_destSize(d.getOrigData(), d.getCompressedBuff(), NULL /* srcSizePtr */, d.getCompressedSize()), 0);
    // src is NULL and srcSizePtr is NULL.
    EXPECT_EQ(LZ4_compress_destSize(NULL /* src */, d.getCompressedBuff(), NULL /* srcSizePtr */, d.getCompressedSize()), 0);
}

TEST_P(LZ4_compress_destSize_test, AOCL_Compression_lz4_LZ4_compress_destSize_fail_common_12) // edge cases when "outputDirective == fillOutput" && when destination buffer is about to get filled to its max capacity.
{
    int sourceLength = 40;
    bool atLeastOneSuccess = false;
    for (int targetDestSize = 1; targetDestSize <= LZ4_compressBound(sourceLength); targetDestSize++)
    {
        int srcLen = sourceLength;
        vector<char> src = genRandAlphabets(srcLen);
        char *srcBuff = src.data();

        // Repeated values to find a match
        memset(srcBuff + 10, 'a', 10);
        memset(srcBuff + 30, 'a', 5);

        vector<char> dest(targetDestSize);
        char *compressedBuffer = dest.data();
        int compressedLen = LZ4_compress_destSize(srcBuff, compressedBuffer, &srcLen, targetDestSize);
        if(compressedLen > 0)
            atLeastOneSuccess = true;
        EXPECT_TRUE(check_uncompressed_equal_to_original(srcBuff, srcLen, compressedBuffer, compressedLen));
    }
    EXPECT_TRUE(atLeastOneSuccess);
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LZ4_compress_destSize_test,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));


/*********************************************
 * End of LZ4_compress_destSize
 *********************************************/

/*********************************************
 * "Begin" of LZ4_decompress_safe_partial
 *********************************************/
class LLZ4_decompress_safe_partial : public LLZ4_decompress_safe
{
};

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_fail_common_1) // src_NULL
{
    EXPECT_EQ(LZ4_decompress_safe_partial(NULL, output, srcLen, outLen, outLen), -1);
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_fail_common_2) // dst_NULL
{
    EXPECT_EQ(LZ4_decompress_safe_partial(src, NULL, srcLen, outLen, outLen), -1);
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_fail_common_3) // srcLen_zero
{
    EXPECT_EQ(LZ4_decompress_safe_partial(src, output, 0, outLen, outLen), -1);
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_pass_common_4) // Pass
{
    int decLen = LZ4_decompress_safe_partial(src, output, srcLen, outLen, outLen);
    ASSERT_EQ(decLen, origLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_pass_common_5) // srcLen_1
{
    setOrigSz(1);
    setDstSz(10);
    // srcLen : compressed bytes

    EXPECT_EQ(srcLen, 2);
    EXPECT_EQ(LZ4_decompress_safe_partial(src, output, srcLen, outLen, outLen), 1);
    EXPECT_EQ(memcmp(output, original, origLen), 0);
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_pass_common_6) // decompress_partial
{
    int decLen = LZ4_decompress_safe_partial(src, output, srcLen, srcLen / 2, srcLen);
    EXPECT_EQ(decLen, srcLen/2);
    EXPECT_EQ(0, memcmp(output, original, srcLen/2));
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_pass_common_7) // targetOutputSize_zero
{
    // Empty output buffer
    EXPECT_EQ(LZ4_decompress_safe_partial(src, output, srcLen, 0, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_fail_common_8) // rvl_error
{
    int src_len = 16;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xF0;                    // token byte : 1111 0000
    compressed_src[1] = 0;                             // additional literal length
    for(int i=2; i<14; i++)                            // literals   [ Number of literals stored
        compressed_src[i] = i % 255;                   //              is less than literal length ]

    LZ4_writeLE16_wrapper(&compressed_src[14], 10);    // offset

    // rvl (read variable length) error: read limit reached while decoding additional literal length
    EXPECT_LT(LZ4_decompress_safe_partial(compressed_src, output, src_len, outLen, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_fail_common_9) // rvl_error_2
{
    int src_len = 21;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xDF;                    // token byte : 1101 1111
    for(int i=1; i<14; i++)                            // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[14], 10);    // offset
    compressed_src[16]= 0xFF;                          // additional match length
    compressed_src[17]= 0x0F;                          // additional match length

    // last sequence
    compressed_src[18] = (char)0x20;                   // token byte
    for(int i=19; i<21; i++)                           // last literals [less than 5 bytes]
        compressed_src[i] = i % 255;

    // rvl (read variable length) error: read limit reached while decoding additional match length
    EXPECT_LT(LZ4_decompress_safe_partial(compressed_src, output, src_len, outLen, outLen), 0);
}

TEST_P(LLZ4_decompress_safe_partial, AOCL_Compression_lz4_LZ4_decompress_safe_partial_fail_common_10) // offset_outside_buffer
{
    int src_len = 24;
    char compressed_src[src_len];
    setDstSz(src_len*2);

    compressed_src[0] = (char)0xEF;                    // token byte : 1110 1111
    for(int i=1; i<15; i++)                            // literals
        compressed_src[i] = i % 255;

    LZ4_writeLE16_wrapper(&compressed_src[15], 125);   // offset
    compressed_src[17]= 0;                             // additional match length

    // last sequence
    compressed_src[18] = (char)0x50;                   // token byte
    for(int i=19; i<24; i++)                           // last literals
        compressed_src[i] = i % 255;                   

    // Error : offset outside buffers
    EXPECT_LT(LZ4_decompress_safe_partial(compressed_src, output, src_len, outLen, outLen), 0);
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_decompress_safe_partial,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

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
class LLZ4_compress_fast_extState : public AOCL_setup_lz4
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

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_fail_common_1) // state_is_Null
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_fast_extState(NULL, src, dst, srcSize, dstSize, 5), 0);
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_fail_common_2) // src_NULL_srcLen_not_NULL
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_fast_extState(state, NULL, dst, srcSize, dstSize, 5), 0);
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_fail_common_3) // dst_NULL
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_fast_extState(state, src, NULL, srcSize, dstSize, 5), 0);
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_pass_common_4) // src_NULL_srcSize_0
{
    setSrcSize(1000);
    setDstSize(LZ4_compressBound(srcSize));
    // generates an empty block
    EXPECT_EQ(LZ4_compress_fast_extState(state, NULL, dst, 0, dstSize, 2), 1);
    EXPECT_EQ(dst[0], 0);
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_pass_common_5) // acceleration_is_greater_than_max
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_MAX+1);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_pass_common_6) // acceleration_is_less_than_min
{
    setSrcSize(100000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT-1);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_pass_common_7) // if_coverage1
{
    setSrcSize(800);
    setDstSize(LZ4_compressBound(srcSize));

    // case 1: with dstSize >= LZ4_compressBound(srcSize) and srcSize < LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value 0, notLimited, byU16 respectively.
    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 2);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_pass_common_8) // if_coverage2
{
    setSrcSize(65547);
    setDstSize(LZ4_compressBound(srcSize));

    // case 2: with dstSize >= LZ4_compressBound(srcSize) and srcSize >= LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value 0, notLimited, byPtr/byU32 respectively.
    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_pass_common_9) // if_coverage3
{
    setSrcSize(800);
    setDstSize(800);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    // case 3: with dstSize < LZ4_compressBound(srcSize) and srcSize < LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value dstSize, limitedOutput, byU16 respectively.
    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState, AOCL_Compression_lz4_LZ4_compress_fast_extState_pass_common_10) // if_coverage4
{
    setSrcSize(65547);
    setDstSize(65547);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    // case 4: with dstSize < LZ4_compressBound(srcSize) and srcSize >= LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value dstSize, limitedOutput, byPtr/byU32 respectively. 
    int compressedSize = LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_compress_fast_extState,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * End of LZ4_compress_fast_extState
 *********************************************/

#ifndef _WINDOWS

/************************************************
 * Begin of LZ4_compress_fast_extState_fastReset
 ************************************************/

class LLZ4_compress_fast_extState_fastReset : public LLZ4_compress_fast_extState
{};

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_pass_common_1) // src_NULL_srcSize_0
{
    setSrcSize(1000);
    setDstSize(LZ4_compressBound(srcSize));
    // generates an empty block
    EXPECT_EQ(LZ4_compress_fast_extState_fastReset(state, NULL, dst, 0, dstSize, 2), 1);
    EXPECT_EQ(dst[0], 0);
}

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_pass_common_2) // acceleration_is_greater_than_max
{
    setSrcSize(1000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState_fastReset(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_MAX+1);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_pass_common_3) // acceleration_is_less_than_min
{
    setSrcSize(1000);
    setDstSize(LZ4_compressBound(srcSize));

    int compressedSize = LZ4_compress_fast_extState_fastReset(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT-1);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_pass_common_4) // if_coverage1
{
    setSrcSize(800);
    setDstSize(LZ4_compressBound(srcSize));

    // case 1: with dstSize >= LZ4_compressBound(srcSize) and srcSize < LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value 0, notLimited, byU16 respectively.
    int compressedSize = LZ4_compress_fast_extState_fastReset(state, src, dst, srcSize, dstSize, 2);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_pass_common_5) // if_coverage2
{
    setSrcSize(65547);
    setDstSize(LZ4_compressBound(srcSize));

    // case 2: with dstSize >= LZ4_compressBound(srcSize) and srcSize >= LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value 0, notLimited, byPtr/byU32 respectively.
    int compressedSize = LZ4_compress_fast_extState_fastReset(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_pass_common_6) // if_coverage3
{
    setSrcSize(800);
    setDstSize(800);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    // case 3: with dstSize < LZ4_compressBound(srcSize) and srcSize < LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value dstSize, limitedOutput, byU16 respectively.
    int compressedSize = LZ4_compress_fast_extState_fastReset(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_pass_common_7) // if_coverage4
{
    setSrcSize(65547);
    setDstSize(65547);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    // case 4: with dstSize < LZ4_compressBound(srcSize) and srcSize >= LZ4_64Klimit
    //         dstCapacity, limitedOutput_directive, tableType assumes value dstSize, limitedOutput, byPtr/byU32 respectively. 
    int compressedSize = LZ4_compress_fast_extState_fastReset(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_P(LLZ4_compress_fast_extState_fastReset, AOCL_Compression_lz4_LZ4_compress_fast_extState_fastReset_fail_common_8) // dstCapacity_insufficient
{
    setSrcSize(1000);
    setDstSize(LZ4_compressBound(srcSize)/10);
    
    int compressedSize = LZ4_compress_fast_extState_fastReset(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT);
    EXPECT_EQ(compressedSize, 0);

}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_compress_fast_extState_fastReset,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/************************************************
 * End of LZ4_compress_fast_extState_fastReset
 ************************************************/

/************************************************
 * Begin of LZ4_attach_dictionary
 ************************************************/
class LLZ4_attach_dictionary : public ::testing::Test
{
protected:    
    LZ4_stream_t *dictionaryStream = NULL;
    LZ4_stream_t *workingStream = NULL;

    void SetUp() override
    {
        workingStream = LZ4_createStream();
        ASSERT_NE(workingStream, nullptr);
        
        dictionaryStream = LZ4_createStream();
        ASSERT_NE(dictionaryStream, nullptr);

        int dictLen = 1024;
        char dict[1024] = {0};
        for(int i=0; i<dictLen; i++)
        {
            dict[i] = i % 255;
        }
        EXPECT_NE(LZ4_loadDict(dictionaryStream, dict, dictLen), -1);
        
    }

    ~LLZ4_attach_dictionary()
    {
        LZ4_freeStream(workingStream);
        LZ4_freeStream(dictionaryStream);
    }
};

TEST_F(LLZ4_attach_dictionary, AOCL_Compression_lz4_LZ4_attach_dictionary_pass_common_1) // dictionaryStream_NULL
{
    LZ4_attach_dictionary(workingStream, /* dictionaryStream */ NULL);
    EXPECT_EQ(workingStream->internal_donotuse.dictCtx, nullptr);
}

TEST_F(LLZ4_attach_dictionary, AOCL_Compression_lz4_LZ4_attach_dictionary_pass_common_2) // dictSize_0
{
    dictionaryStream->internal_donotuse.dictSize = 0;
    LZ4_attach_dictionary(workingStream, dictionaryStream);
    /* Empty dictionary not attached*/
    EXPECT_EQ(workingStream->internal_donotuse.dictCtx, nullptr);
}

TEST_F(LLZ4_attach_dictionary, AOCL_Compression_lz4_LZ4_attach_dictionary_pass_common_3) // currentOffset_0
{
    dictionaryStream->internal_donotuse.currentOffset = 0;
    LZ4_attach_dictionary(workingStream, dictionaryStream);
    
    EXPECT_EQ(workingStream->internal_donotuse.currentOffset, 64 KB);
    EXPECT_NE(workingStream->internal_donotuse.dictCtx, nullptr);
}

TEST_F(LLZ4_attach_dictionary, AOCL_Compression_lz4_LZ4_attach_dictionary_pass_common_4) // simple_pass_case
{
    LZ4_attach_dictionary(workingStream, dictionaryStream);
    EXPECT_NE(workingStream->internal_donotuse.dictCtx, nullptr);
}

/************************************************
 * End of LZ4_attach_dictionary
 ************************************************/

#endif

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
 * "Begin" of LZ4_initStream
 *********************************************/
class LLZ4_initStream : public ::testing::Test {

protected:
    LZ4_stream_t *lz4s = NULL;

    void SetUp() override 
    {
        lz4s = (LZ4_stream_t*)malloc(sizeof(LZ4_stream_t));
        ASSERT_NE((long long)lz4s, NULL);
    }

    ~LLZ4_initStream() {
        free(lz4s);
        lz4s = nullptr;
    }
};

TEST_F(LLZ4_initStream, AOCL_Compression_lz4_LZ4_initStream_fail_common_1) // buffer_NULL
{
    EXPECT_EQ(LZ4_initStream(NULL, sizeof(LZ4_stream_t)), nullptr);
}

TEST_F(LLZ4_initStream, AOCL_Compression_lz4_LZ4_initStream_fail_common_2) // size_cond_not_met
{
    EXPECT_EQ(LZ4_initStream(lz4s, sizeof(LZ4_stream_t)-1), nullptr);
}

TEST_F(LLZ4_initStream, AOCL_Compression_lz4_LZ4_initStream_fail_common_3) // bad_alignment
{
    EXPECT_EQ(LZ4_initStream((char*)lz4s+1, sizeof(LZ4_stream_t)), nullptr);
}

TEST_F(LLZ4_initStream, AOCL_Compression_lz4_LZ4_initStream_pass_common_4) // pass_case
{
    EXPECT_NE(LZ4_initStream(lz4s, sizeof(LZ4_stream_t)), nullptr);
}

TEST_F(LLZ4_initStream, AOCL_Compression_lz4_LZ4_initStream_pass_common_5) // arbitrary_buffer_with_random_data
{
    char *buff = (char*)lz4s;
    ASSERT_NE((long long)buff, NULL);
    for(int i=0; i<sizeof(LZ4_stream_t); i++)
    {
        buff[i] = i;
    }

    EXPECT_NE(LZ4_initStream(buff, sizeof(LZ4_stream_t)), nullptr);
    for(int i=0; i<sizeof(LZ4_stream_t); i++)
    {
        EXPECT_EQ(buff[i], 0);
    }

}

/*********************************************
 * End of LZ4_initStream
 *********************************************/

/*********************************************
 * "Begin" of LZ4_freeStream
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
 * End of LZ4_freeStream
 *********************************************/

/*********************************************
 * "Begin" of LZ4_resetStream_fast
 *********************************************/

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_1) // Legitimate_ptr
{
    LZ4_stream_t *stream = (LZ4_stream_t *)malloc(sizeof(LZ4_stream_t));
    ASSERT_NE((long long)stream, NULL);
    memset(stream, 0, sizeof(LZ4_stream_t)); 
    LZ4_resetStream_fast(stream);
    LZ4_freeStream(stream);
}

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_2) // LZ4_createStream
{
    LZ4_stream_t *stream = LZ4_createStream();
    LZ4_resetStream_fast(stream);
    LZ4_freeStream(stream);
}

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_3) // input_NULL
{
    LZ4_resetStream_fast(NULL);
}

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_4) // tableType_2_currentOffset_>_1GB
{
    LZ4_stream_t *stream = LZ4_createStream();

    LZ4_stream_t_internal* internal_p = &(stream->internal_donotuse);
    internal_p->tableType = 2;
    internal_p->currentOffset = 1 GB + 1;

    LZ4_resetStream_fast(stream);

    EXPECT_EQ(internal_p->currentOffset, 0);
    EXPECT_EQ(internal_p->tableType, 0);    
    EXPECT_EQ(internal_p->dictSize, 0);
    EXPECT_EQ(internal_p->dictCtx, nullptr);
    EXPECT_EQ(internal_p->dictionary, nullptr);

    LZ4_freeStream(stream);
}

TEST(LZ4_resetStream_fast, AOCL_Compression_lz4_LZ4_resetStream_fast_common_5) // tableType_0_currentOffset_>_1GB
{
    LZ4_stream_t *stream = LZ4_createStream();

    LZ4_stream_t_internal* internal_p = &(stream->internal_donotuse);
    internal_p->tableType = 0;
    internal_p->currentOffset = 1 GB + 1;

    LZ4_resetStream_fast(stream);

    EXPECT_EQ(internal_p->currentOffset, (1 GB + 1) + 64 KB);
    EXPECT_EQ(internal_p->tableType, 0);
    EXPECT_EQ(internal_p->dictSize, 0);
    EXPECT_EQ(internal_p->dictCtx, nullptr);
    EXPECT_EQ(internal_p->dictionary, nullptr);

    LZ4_freeStream(stream);
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
    
    EXPECT_EQ(LZ4_loadDict(stream, dict, dictSize), 64 KB);
    LZ4_stream_t_internal* internal_p = &(stream->internal_donotuse);
    EXPECT_EQ(internal_p->dictionary, (unsigned char *)(dict+(dictSize-64 KB)));
    EXPECT_EQ(internal_p->dictSize, 64 KB);
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

TEST(LZ4_loadDict, AOCL_Compression_lz4_LZ4_loadDict_common_6) // dictionary_NULL_dictSize_7
{
    LZ4_stream_t *stream = LZ4_createStream();
    int dictSize = 7;
    EXPECT_EQ(LZ4_loadDict(stream, NULL, dictSize), 0);

    LZ4_freeStream(stream);
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

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_fail_common_1) // lz4_stream_NULL
{
    setSrcSize(1024);
    setDstSize(10000);
    EXPECT_EQ(LZ4_compress_fast_continue(NULL, src, dst, srcSize, dstSize, 50), 0);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_fail_common_2) // source_NULL
{
    setSrcSize(1024);
    setDstSize(10000);
    EXPECT_EQ(LZ4_compress_fast_continue(state, NULL, dst, srcSize, dstSize, 50), 0);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_fail_common_3) // dest_NULL
{
    setSrcSize(1024);
    setDstSize(10000);
    EXPECT_EQ(LZ4_compress_fast_continue(state, src, NULL, srcSize, dstSize, 50), 0);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_4) // input_sz1
{
    setSrcSize(1024);
    setDstSize(10000);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 50);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_5) // acceleration_greater_max
{
    setSrcSize(100000);
    setDstSize(150000);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_MAX+1);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_6) // acceleration_less_than_min
{
    setSrcSize(100000);
    setDstSize(150000);
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, LZ4_ACCELERATION_DEFAULT-100);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_7) // invalidate_tiny_dictionaries
{
    setSrcSize(100);
    setDstSize(150);
    int dictSize = 3;
    state->internal_donotuse.dictSize = dictSize;

    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_8) // prefix_mode_1
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

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_9) // prefix_mode_2
{
    setSrcSize(100);
    setDstSize(150);
    const int dictSize = 64 KB;
    char* dict = (char*)malloc((dictSize + srcSize) * sizeof(char));
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
    free(dict);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_10) // using_LoadDict_less_64kb
{
    setSrcSize(100);
    setDstSize(150);
    const int dictSize = 100;
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

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_11) // using_LoadDict_64kb
{
    setSrcSize(100);
    setDstSize(150);
    const int dictSize = 64 KB;
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

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_12) // external_dictionary_mode
{
    setSrcSize(100);
    setDstSize(150);
    const int dictSize = 100;
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

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_13) // external_dictionary_mode_srcSize_>_4KB
{
    setSrcSize((4 KB) + 1);
    setDstSize(LZ4_compressBound(srcSize));
    const int dictSize = 1 KB;
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
    
    // copies the dictionary's tables into the active context,
    // therefore, compression loop now, looks into only one table.
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 1);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
    
    free(d);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_14) // overlapping_source_dict
{
    setSrcSize(150);
    setDstSize(200);
    const int dictSize = 100;
    char dict[200] = {0};
    
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

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_15) // overlap_greater_than_64kb
{
    setSrcSize(150);
    setDstSize(200);
    const int dictSize = 65 KB;
    char dict[150 + dictSize] ={0};
    
    memcpy(dict, src, srcSize);
    for (int i = 150; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDict(state, &dict[100], dictSize);
    state->internal_donotuse.dictionary = (const LZ4_byte *)&dict[100]; // to ensure overlap
    state->internal_donotuse.dictSize += 100;
    
    int compressedLen = LZ4_compress_fast_continue(state, dict, dst, srcSize, dstSize, 30);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_pass_common_16) // src_NULL_srcSize_0
{
    setSrcSize(0);
    setDstSize(LZ4_compressBound(srcSize));
    int outLen = LZ4_compress_fast_continue(state, NULL, dst, 0, dstSize, 50);
    // generates an empty block
    EXPECT_EQ(outLen, 1);
    EXPECT_EQ(dst[0], 0);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_fail_common_17) // src_NULL_srcSize_0_dstSize_0
{
    setSrcSize(0);
    setDstSize(srcSize);
    // no output, can't write anything
    EXPECT_EQ(LZ4_compress_fast_continue(state, NULL, dst, srcSize, dstSize, 1), 0);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_fail_common_18) // src_len_1byte_short
{
    setSrcSize(1600);
    setDstSize(LZ4_compressBound(srcSize));
    int dictSize = 700;
    char* dict = src;
    char* src_ptr = src + dictSize;
    int src_len = srcSize - dictSize;
    LZ4_loadDict(state, dict, dictSize);
    int compressedLen = LZ4_compress_fast_continue (state, src_ptr, dst, src_len, src_len-1, 1);
    
    EXPECT_EQ(compressedLen, 0);
}

TEST_P(LLZ4_compress_fast_continue, AOCL_Compression_lz4_LZ4_compress_fast_continue_fail_common_19) // corrupted_extDict_NULL_dictionary
{

    char dict[1024] = {0};
    for(int i = 0; i < 1024; i++) {
        dict[i] = (char)(i % 256);
    }
    ASSERT_GT(LZ4_loadDict(state, dict, 1024), 0);
    
    // Corrupt the dictionary pointer
    state->internal_donotuse.dictionary = NULL;
    // dictSize remains non-zero from loadDict
    
    setSrcSize(1024);
    setDstSize(10000);
    for(int i = 0; i < srcSize; i++) {
        src[i] = (char)((i + 100) % 256);
    }
    
    int compressedLen = LZ4_compress_fast_continue(state, src, dst, srcSize, dstSize, 1);
    
    // Fail gracefully and returns 0 due to NULL dictBase in usingExtDict path
    EXPECT_EQ(compressedLen, 0);
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_compress_fast_continue,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));


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

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_pass_common_1) // dictonary_size_greater_64kb
{
    char *data = (char *)malloc(100000);
    
    for(int i=0; i<100000; i++)
    {
        data[i] = i % 256;
    }

    EXPECT_EQ(loadDict(66000),64 KB);
    EXPECT_EQ(LZ4_saveDict(stream, data, dictSize), 64 KB);

    LZ4_stream_t_internal *dic = &(stream->internal_donotuse);

    EXPECT_EQ(dic->dictSize, 64 KB);
    EXPECT_EQ(memcmp(dic->dictionary, data, 64 KB), 0);

    free(data);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_fail_common_2) // dictionary_and_dictSize_is_zero
{
    EXPECT_EQ(LZ4_saveDict(stream, NULL, 0), 0);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_fail_common_3) // stream_NULL
{
    char dict[11]="helloWorld";
    EXPECT_EQ(LZ4_saveDict(NULL, dict, 10), 0);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_fail_common_4) // dictionary_is_NULL_dictSize_not_zero
{
    EXPECT_EQ(LZ4_saveDict(stream, NULL, 3), 0);
}

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_pass_common_5) // dictionary_size_greater_than_parameter
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

TEST_F(LLZ4_saveDict, AOCL_Compression_lz4_LZ4_saveDict_fail_common_6) // no_loaded_dictionary
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

TEST(LZ4_createStreamDecode, AOCL_Compression_lz4_LZ4_createStreamDecode_pass_common_1) // Not_NULL
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

TEST(LZ4_freeStreamDecode, AOCL_Compression_lz4_LZ4_freeStreamDecode_pass_common_1) // NULL_INPUT
{
    EXPECT_EQ(LZ4_freeStreamDecode(NULL), 0);
}

TEST(LZ4_freeStreamDecode, AOCL_Compression_lz4_LZ4_freeStreamDecode_pass_common_2) // Not_NULL_INPUT
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

TEST(LZ4_setStreamDecode, AOCL_Compression_lz4_LZ4_setStreamDecode_pass_common_1) // Not_NULL_INPUT
{
    LZ4_streamDecode_t *lz = LZ4_createStreamDecode();
    int dictSize = 1024;
    char *dict = (char *)malloc(dictSize);
    
    EXPECT_EQ(LZ4_setStreamDecode(lz, dict, dictSize), 1);
    EXPECT_EQ(lz->internal_donotuse.prefixEnd, (const unsigned char*)dict+dictSize);

    LZ4_freeStreamDecode(lz);
    free(dict);
}

TEST(LZ4_setStreamDecode, AOCL_Compression_lz4_LZ4_setStreamDecode_fail_common_2) // LZ4_streamDecode_NULL
{
    LZ4_streamDecode_t* lz = LZ4_createStreamDecode();
    int dictSize = 1024;
    char* dict = (char*)malloc(dictSize);
    
    EXPECT_EQ(LZ4_setStreamDecode(NULL, dict, dictSize), 0);
    
    LZ4_freeStreamDecode(lz);
    free(dict);
}

TEST(LZ4_setStreamDecode, AOCL_Compression_lz4_LZ4_setStreamDecode_pass_common_3) // dict_NULL_dictSize_0
{
    LZ4_streamDecode_t* lz = LZ4_createStreamDecode();
    
    EXPECT_EQ(LZ4_setStreamDecode(lz, NULL, 0), 1);
    EXPECT_EQ(lz->internal_donotuse.prefixEnd, nullptr);

    LZ4_freeStreamDecode(lz);
}

/*********************************************
 * End of LZ4_setStreamDecode
 *********************************************/

/*********************************************
 * "Begin" of LZ4_decoderRingBufferSize
 *********************************************/

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_fail_common_1) // INPUT_MINUS_1
{
    EXPECT_EQ(LZ4_decoderRingBufferSize(-1), 0);
}

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_pass_common_2) // INPUT_10
{
    EXPECT_EQ(LZ4_decoderRingBufferSize(10), 65566);
}

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_fail_common_3) // INPUT_INT_MAX
{
    EXPECT_EQ(LZ4_decoderRingBufferSize(INT_MAX), 0);
}

TEST(LZ4_decoderRingBufferSize, AOCL_Compression_lz4_LZ4_decoderRingBufferSize_pass_common_4) // INPUT_100000
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

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_fail_common_1) // stream_NULL
{
    EXPECT_EQ(LZ4_decompress_safe_continue(NULL, src, output, srcLen, outLen), -1);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_fail_common_2) // src_NULL
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    EXPECT_EQ(LZ4_decompress_safe_continue(decode, NULL, output, srcLen, outLen), -1);
    
    LZ4_freeStreamDecode(decode);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_fail_common_3) // dest_NULL
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    EXPECT_EQ(LZ4_decompress_safe_continue(decode, src, NULL, srcLen, outLen), -1);
    
    LZ4_freeStreamDecode(decode);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_pass_common_4) // simple_pass_case1
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    
    EXPECT_EQ(0, memcmp(output, original, origLen));
    
    LZ4_freeStreamDecode(decode);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_pass_common_5) // simple_pass_case2
{   
    int outlen_2 = 1000;
    char *output2 = (char *)malloc(outlen_2);
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();

    // This is executed to make prefixSize != 0.
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    
    EXPECT_EQ(0, memcmp(output, original, origLen));
    
    decompress = LZ4_decompress_safe_continue(decode, src, output2, srcLen, outlen_2);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output2, original, origLen));
    
    LZ4_freeStreamDecode(decode);
    free(output2);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_fail_common_6) // Overflow
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();

    // When overflow occurs the return value would be less than zero.
    EXPECT_LT(LZ4_decompress_safe_continue(decode, src, output, srcLen, 400),0);

    LZ4_freeStreamDecode(decode);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_fail_common_7) // Overflow2
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    // This line is for making prefixSize != 0.
    EXPECT_EQ(LZ4_decompress_safe_continue(decode, src, output, srcLen, 800), origLen);
    EXPECT_EQ(memcmp(original, output, origLen), 0);
    
    int uncompressed_len = LZ4_decompress_safe_continue(decode, src, output, srcLen, 100);
    
    // When overflow occurs the return value would be less than zero.
    EXPECT_LT(uncompressed_len, 0);
    
    LZ4_freeStreamDecode(decode);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_pass_common_8) // Middle_if_test_case
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
    
    // The condition (lz4sd->prefixEnd == (BYTE*)dest) is made by previous decompress call
    // covers: lz4sd->extDictSize == 0 
    decompress = LZ4_decompress_safe_continue(decode, src, output + decompress, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output + decompress, original, origLen));
    
    LZ4_freeStreamDecode(decode);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_fail_common_9) // Middle_if_test_case2
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    
    int decompress = LZ4_decompress_safe_continue(decode, src, output, srcLen, outLen);
    EXPECT_EQ(decompress, origLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
    
    // The condition (lz4sd->prefixEnd == (BYTE*)dest) is made by previous decompress call
    // covers: lz4sd->extDictSize == 0 , and we expect return value to be less than zero.
    int uncompressed_len = LZ4_decompress_safe_continue(decode, src, output + decompress, srcLen, 100);
    EXPECT_LT(uncompressed_len, 0);

    LZ4_freeStreamDecode(decode);
}

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_pass_common_10) // Middle_if_test_case3
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

TEST_P(LLZ4_decompress_safe_continue, AOCL_Compression_lz4_LZ4_decompress_safe_continue_pass_common_11) // Middle_if_test_case4
{
    setOrigSz(67000);
    setDstSz(77000);

    // compression
    const int dLen = 64 KB;
    LZ4_stream_t *lz = LZ4_createStream();
    LZ4_resetStream_fast(lz);
    char* dict_and_dest = (char*)malloc((dLen + outLen) * sizeof(char));
    
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
    free(dict_and_dest);
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_decompress_safe_continue,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

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
class LLZ4_decompress_safe_usingDict : public AOCL_setup_lz4
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
        srcLen = LZ4_compressBound(origLen);
        outLen = origLen;
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

TEST_P(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_fail_common_1) // src_NULL
{
    setDictSz(64000);
    char* out = (char*)malloc((dictLen + outLen) * sizeof(char));
    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    if(out)
        memcpy(out, dict, dictLen);
    
    EXPECT_EQ(LZ4_decompress_safe_usingDict(NULL, output, srcLen, outLen, dict, dictLen), -1);
    free(out);
}

TEST_P(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_fail_common_2) // output_NULL
{
    setDictSz(64000);
    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    ASSERT_EQ(LZ4_decompress_safe_usingDict(src, NULL, srcLen, outLen, dict, dictLen), -1);
}

TEST_P(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_pass_common_3) // Pass
{
    srcLen = LZ4_compress_default(original, src, origLen, srcLen);
    EXPECT_GT(srcLen, 0);
    EXPECT_EQ(LZ4_decompress_safe_usingDict(src, output, srcLen, outLen, NULL, 0), origLen);

    EXPECT_EQ(memcmp(output, original, origLen), 0);
}

TEST_P(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_pass_common_4) // PassUsingDict
{
    setDictSz(1024);
    
    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    
    ASSERT_EQ(LZ4_decompress_safe_usingDict(src, output, srcLen, outLen, dict, dictLen), origLen);
    EXPECT_EQ(memcmp(output, original, origLen), 0);
}

TEST_P(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_pass_common_5) // PassUsingDict_size_65536
{
    setDictSz(65536);
    char* out = (char*)malloc((dictLen + outLen) * sizeof(char));
    
    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    if(out && srcLen >= 0)
    {
        memcpy(out, dict, dictLen);
    
        EXPECT_EQ(LZ4_decompress_safe_usingDict(src, out + dictLen, srcLen, outLen, out, dictLen), origLen);
        EXPECT_EQ(memcmp(out + dictLen, original, origLen), 0);
    }
    free(out);
}

TEST_P(LLZ4_decompress_safe_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_usingDict_pass_common_6) // PassUsingDict_size_64000
{
    setDictSz(64000);
    char* out = (char*)malloc((dictLen + outLen) * sizeof(char));

    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    if(out != NULL && srcLen > 0)
    {
        memcpy(out, dict, dictLen);
    
        EXPECT_EQ(LZ4_decompress_safe_usingDict(src, out + dictLen, srcLen, outLen, out, dictLen), origLen);
        EXPECT_EQ(memcmp(out + dictLen, original, origLen), 0);
    }
    free(out);
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_decompress_safe_usingDict,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * End of LZ4_decompress_safe_usingDict
 *********************************************/

/***************************************************
 * "Begin" of LZ4_decompress_safe_partial_usingDict
 ***************************************************/
class LLZ4_decompress_safe_partial_usingDict : public LLZ4_decompress_safe_usingDict
{
public:
    char* out = NULL;

    void setup_for_prefixDict(int dictLen)
    {
        setDictSz(dictLen);
        out = (char*)malloc((dictLen + outLen) * sizeof(char));
        EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    
        srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, LZ4_ACCELERATION_MAX);
        EXPECT_GT(srcLen, 0);
        memcpy(out, dict, dictLen);
    }

    // Destructor function of `LLZ4_decompress_safe_partial_usingDict` class.
    ~LLZ4_decompress_safe_partial_usingDict()
    {
        if(out)
            free(out);
    }
};

TEST_P(LLZ4_decompress_safe_partial_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_fail_common_1) // src_NULL
{
    setDictSz(1024);
    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 1);

    int targetOutputSize = srcLen/2;    
    ASSERT_EQ(LZ4_decompress_safe_partial_usingDict(NULL /*src*/, output, srcLen, targetOutputSize, outLen, dict, dictLen), -1);
}

TEST_P(LLZ4_decompress_safe_partial_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_fail_common_2) // dst_NULL
{
    setDictSz(64000);
    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 1);
    
    int targetOutputSize = srcLen/2;
    ASSERT_EQ(LZ4_decompress_safe_partial_usingDict(src, NULL /*dst*/, srcLen, targetOutputSize, outLen, dict, dictLen), -1);
}

TEST_P(LLZ4_decompress_safe_partial_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_pass_common_3) // dictStart_NULL_dictSize_0
{
    srcLen = LZ4_compress_default(original, src, origLen, srcLen);
    EXPECT_GT(srcLen, 0);

    int targetOutputSize = srcLen/2;
    EXPECT_EQ(LZ4_decompress_safe_partial_usingDict(src, output, srcLen, targetOutputSize, outLen, NULL, 0), targetOutputSize);
    EXPECT_EQ(memcmp(output, original, targetOutputSize), 0);
}

TEST_P(LLZ4_decompress_safe_partial_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_pass_common_4) // PassUsingDict
{
    setDictSz(1024);
    EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
    srcLen = LZ4_compress_fast_continue(stream, original, src, origLen, srcLen, 5000);
    EXPECT_GT(srcLen, 0);

    int targetOutputSize = srcLen/2;
    ASSERT_EQ(LZ4_decompress_safe_partial_usingDict(src, output, srcLen, targetOutputSize, outLen, dict, dictLen), targetOutputSize);
    EXPECT_EQ(memcmp(output, original, targetOutputSize), 0);
}

TEST_P(LLZ4_decompress_safe_partial_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_pass_common_5) // prefixDict_>=_64KB-1
{
    setup_for_prefixDict(65536); /* dictLen = 64KB */

    int targetOutputSize = srcLen/2;
    ASSERT_EQ(LZ4_decompress_safe_partial_usingDict(src, out + dictLen, srcLen, targetOutputSize, outLen, out, dictLen), targetOutputSize);
    EXPECT_EQ(memcmp(out + dictLen, original, targetOutputSize), 0);
}

TEST_P(LLZ4_decompress_safe_partial_usingDict, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_pass_common_6) // prefixDict_<_64KB-1
{
    setup_for_prefixDict(1024); /* dictLen = 1024 */

    int targetOutputSize = srcLen/2;
    ASSERT_EQ(LZ4_decompress_safe_partial_usingDict(src, out + dictLen, srcLen, targetOutputSize, outLen, out, dictLen), targetOutputSize);
    EXPECT_EQ(memcmp(out + dictLen, original, targetOutputSize), 0);
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_decompress_safe_partial_usingDict,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/***********************************************
 * End of LZ4_decompress_safe_partial_usingDict
 ***********************************************/

/*********************************************
 * "Begin" of LZ4_compress_forceExtDict
 *********************************************/
class LLZ4_compress_forceExtDict : public LLZ4_compress_fast_continue
{
public:
    char* dict = NULL;
    int dictSize = 0;
    void SetUp() override
    {
        state = LZ4_createStream();

        dictSize = 1024;
        dict = (char *)malloc(dictSize);
        for(int i=0; i<dictSize; i++)
        {
            dict[i] = i % 256;
        }
        EXPECT_NE(LZ4_loadDict(state, dict, dictSize), -1);
    }

    void loadDictOfSz(int dictSz)
    {
        if(state) {
            LZ4_freeStream(state);
            state = NULL;
        }
        if(dict) {
            free(dict);
            dict = NULL;
        }
        state = LZ4_createStream();

        dictSize = dictSz;
        dict = (char *)malloc(dictSize);
        for(int i=0; i<dictSize; i++)
        {
            dict[i] = i % 256;
        }
        EXPECT_NE(LZ4_loadDict(state, dict, dictSize), -1);
    }

    ~LLZ4_compress_forceExtDict()
    {
        if (dict)
            free(dict);
    }
};

TEST_P(LLZ4_compress_forceExtDict, AOCL_Compression_lz4_LZ4_compress_forceExtDict_fail_common_1) // state_NULL
{
    setSrcSize(1024);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(Test_LZ4_compress_forceExtDict(NULL /* state */, src, dst, srcSize), 0);
}

TEST_P(LLZ4_compress_forceExtDict, AOCL_Compression_lz4_LZ4_compress_forceExtDict_fail_common_2) // src_NULL
{
    setSrcSize(1024);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(Test_LZ4_compress_forceExtDict(state, NULL /* src */, dst, srcSize), 0);
}

TEST_P(LLZ4_compress_forceExtDict, AOCL_Compression_lz4_LZ4_compress_forceExtDict_fail_common_3) // dst_NULL
{
    setSrcSize(1024);
    setDstSize(LZ4_compressBound(srcSize));
    EXPECT_EQ(Test_LZ4_compress_forceExtDict(state, src, NULL /* dst */, srcSize), 0);
}

TEST_P(LLZ4_compress_forceExtDict, AOCL_Compression_lz4_LZ4_compress_forceExtDict_fail_common_4) // srcSize_negative
{
    setSrcSize(0);
    setDstSize(LZ4_compressBound(srcSize));
    // srcSize negative: Unsupported srcSize, too large or negative
    EXPECT_EQ(Test_LZ4_compress_forceExtDict(state, src, dst, -1 /* srcSize */), 0);
}

TEST_P(LLZ4_compress_forceExtDict, AOCL_Compression_lz4_LZ4_compress_forceExtDict_pass_common_5) // src_NULL_srcSize_0
{
    setSrcSize(0);
    setDstSize(LZ4_compressBound(srcSize));
    int outLen = Test_LZ4_compress_forceExtDict(state, NULL, dst, 0);
    // generates an empty block
    EXPECT_EQ(outLen, 1);
    EXPECT_EQ(dst[0], 0);
}

TEST_P(LLZ4_compress_forceExtDict, AOCL_Compression_lz4_LZ4_compress_forceExtDict_pass_common_6) // usingExtDict_dictSmall
{
    setSrcSize(1024);
    setDstSize(LZ4_compressBound(srcSize));

    int outLen = Test_LZ4_compress_forceExtDict(state, src, dst, srcSize);
    EXPECT_GT(outLen, 0);

    // validate
    check_uncompressed_equal_to_original(src, srcSize, dst, outLen);
}

TEST_P(LLZ4_compress_forceExtDict, AOCL_Compression_lz4_LZ4_compress_forceExtDict_pass_common_7) // usingExtDict_noDictIssue
{
    setSrcSize(6 KB);
    setDstSize(LZ4_compressBound(srcSize));
    loadDictOfSz(64 KB + 1);

    int outLen = Test_LZ4_compress_forceExtDict(state, src, dst, srcSize);
    EXPECT_GT(outLen, 0);

    // validate
    check_uncompressed_equal_to_original(src, srcSize, dst, outLen);
}

INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LLZ4_compress_forceExtDict,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of LZ4_compress_forceExtDict
 *********************************************/

/*********************************************
 * "Begin" of LZ4_renormDictT
 *********************************************/
class LLZ4_renormDictT : public ::testing::Test
{
public:
    LZ4_stream_t *stream = NULL;
    LZ4_stream_t_internal *lz4sd = NULL;
    int nextSize = INT_MAX - 1;
    int dictLen = 0;
    char *dict = NULL;
    
    void SetUp() override
    {
        stream = LZ4_createStream();

        dictLen = 1024;
        dict = (char *)malloc(dictLen);
        for(int i=0; i<dictLen; i++)
        {
            dict[i] = i % 255;
        }

        EXPECT_NE(LZ4_loadDict(stream, dict, dictLen), -1);
        lz4sd = &(stream->internal_donotuse);
    }

    void update_dictSize(int dictSize)
    {
        if (dict)
        {
            free(dict);
            dict = NULL;
        }
        dictLen = dictSize;
        dict = (char *)malloc(dictLen);
        for(int i=0; i<dictLen; i++)
        {
            dict[i] = i % 255;
        } 
        lz4sd->dictSize = dictSize;    
        lz4sd->dictionary = (const LZ4_byte*) dict;
    }

    void validate()
    {
        EXPECT_EQ(lz4sd->currentOffset, 64 KB);
        EXPECT_LE(lz4sd->dictSize, 64 KB);
        int minDictSize = dictLen < 64 KB ? dictLen: 64 KB;
        EXPECT_EQ(lz4sd->dictionary, (const LZ4_byte*)dict + dictLen - minDictSize);
    }

    ~LLZ4_renormDictT()
    {
        LZ4_freeStream(stream);
        if(dict)
            free(dict);

    }   
};

TEST_F(LLZ4_renormDictT, AOCL_Compression_lz4_LZ4_renormDictT_pass_common_1) // dictSize modified to > 64 KB 
{
    update_dictSize(64 KB + 1);   // dictSize modified to > 64 KB
    Test_LZ4_renormDictT(lz4sd, nextSize);
    validate();
}

TEST_F(LLZ4_renormDictT, AOCL_Compression_lz4_LZ4_renormDictT_pass_common_2) // currentOffset modified to > 64 KB
{    
    lz4sd->currentOffset = (64 KB + 1); // currentOffset modified to > 64 KB
    Test_LZ4_renormDictT(lz4sd, nextSize);
    validate();
}

TEST_F(LLZ4_renormDictT, AOCL_Compression_lz4_LZ4_renormDictT_pass_common_3) // Pass
{
    Test_LZ4_renormDictT(lz4sd, nextSize);
    validate();
}

/*********************************************
 * "End" of LZ4_renormDictT
 *********************************************/

/*********************************************
 * "Begin" of LZ4_AOCL_LZ4_wildCopy64_AVX
 *********************************************/

/*
* AOCL_LZ4_wildCopy64_AVX() is AVX implementation of wildCopy that copies two times 32 bytes in an iteration. 
*/
#ifdef AOCL_LZ4_AVX_OPT
class LZ4_AOCL_LZ4_wildCopy64_AVX : public ::testing::Test
{
protected:
    
    char* srcPtr = NULL;
    char* dstPtr = NULL;
    
    // Initialize pointers.
    void SetUp() override
    {
        int srcLen = 500;    
        srcPtr = (char *)malloc(srcLen);
        for (int i = 0; i < srcLen; i++)
        {
            // random numbers from 1 to 250
            srcPtr[i] = (rand() % 250) + 1;
        }

        int dstLen = 500;   
        dstPtr = (char *)malloc(dstLen);
        for (int i = 0; i < dstLen; i++)
        {
            // Null (NULL)
            dstPtr[i] = 0;
        }
    }
    
    // Destructor function of `LZ4_AOCL_LZ4_wildCopy64_AVX` class.
    ~LZ4_AOCL_LZ4_wildCopy64_AVX()
    {
        if (srcPtr) { free(srcPtr); }            
        if (dstPtr) { free(dstPtr); }
    }
};

TEST_F(LZ4_AOCL_LZ4_wildCopy64_AVX, AOCL_Compression_lz4_AOCL_LZ4_wildCopy64_AVX_pass_common_1) // len_multiple_of_64
{
    // case1: When no. of bytes to copy is multiple of 64
    //        exact no. of bytes are copied
    int length = 64;

    Test_AOCL_LZ4_wildCopy64_AVX(dstPtr, srcPtr, dstPtr+length);
    EXPECT_EQ(memcmp(dstPtr, srcPtr, length), 0);
    EXPECT_NE(memcmp(dstPtr, srcPtr, length+1), 0); 
}

TEST_F(LZ4_AOCL_LZ4_wildCopy64_AVX, AOCL_Compression_lz4_AOCL_LZ4_wildCopy64_AVX_pass_common_2) // len_NOT_multiple_of_64
{
    // case2: When no. of bytes to copy is not a multiple of 64
    //        additional byte/s is/are copied 
    int length = 1;

    Test_AOCL_LZ4_wildCopy64_AVX(dstPtr, srcPtr, dstPtr+length);                 
    EXPECT_EQ(memcmp(dstPtr, srcPtr, 64), 0);          // additional 63 bytes are copied            
    EXPECT_NE(memcmp(dstPtr, srcPtr, 65), 0);                

    length = 127;

    Test_AOCL_LZ4_wildCopy64_AVX(dstPtr, srcPtr, dstPtr+length);               
    EXPECT_EQ(memcmp(dstPtr, srcPtr, 128), 0);         // additional 1 byte is copied                 
    EXPECT_NE(memcmp(dstPtr, srcPtr, 129), 0); 
}

TEST_F(LZ4_AOCL_LZ4_wildCopy64_AVX, AOCL_Compression_lz4_AOCL_LZ4_wildCopy64_AVX_fail_common_3) // offset_less_than_32
{
    // case 1: offset < 32 

    // The function cannot be called to copy 100 bytes starting from `src` to `dst` which is 31 bytes apart,
    // with an expectation that 100 bytes from `dst` must be same as 100 bytes from `src` after the function call.

    // Since offset < 32, there is an overlap between `src` and `dst`
    // due to which length (100) bytes from `dst` is not same as length
    // (100) bytes from `src`.

    // There occurs no overlap between `src` and `dst` when (offset >= 32).

    int length = 100;
    int offset = 31;

    // fill valid (not NULL) data in dstPtr (until dst+offset) to copy
    char c = 'A';
    for(int i=0; i<offset; i++){
        dstPtr[i] = c++;
    }

    char* src = dstPtr;             
    char* dst = dstPtr+offset;

    Test_AOCL_LZ4_wildCopy64_AVX(dst, src, dst+length);
    EXPECT_NE(memcmp(dst, src, length), 0);
}

TEST_F(LZ4_AOCL_LZ4_wildCopy64_AVX, AOCL_Compression_lz4_AOCL_LZ4_wildCopy64_AVX_pass_common_4) // offset_equal_32
{
    // case 2: offset = 32
    int length = 100;
    int offset = 32;

    char c = 'A';
    for(int i=0; i<offset; i++){
        dstPtr[i] = c++;
    }

    char* src = dstPtr;
    char* dst = dstPtr+offset;

    Test_AOCL_LZ4_wildCopy64_AVX(dst, src, dst+length);
    EXPECT_EQ(memcmp(dst, src, length), 0);
}

TEST_F(LZ4_AOCL_LZ4_wildCopy64_AVX, AOCL_Compression_lz4_AOCL_LZ4_wildCopy64_AVX_pass_common_5) // offset_greater_than_32
{
    // case 3: offset > 32
    int length = 100;
    int offset = 33;

    char c = 'A';
    for(int i=0; i<offset; i++){
        dstPtr[i] = c++;
    }

    char* src = dstPtr;
    char* dst = dstPtr+offset;

    Test_AOCL_LZ4_wildCopy64_AVX(dst, src, dst+length);
    EXPECT_EQ(memcmp(dst, src, length), 0);
}
#endif /* AOCL_LZ4_AVX_OPT */

/*********************************************
 * End of LZ4_AOCL_LZ4_wildCopy64_AVX
 ********************************************/ 

/*********************************************
 * "Begin" of AOCL_LZ4_hash5
 *********************************************/

#ifdef AOCL_LZ4_OPT
TEST(LZ4_AOCL_LZ4_hash5, AOCL_Compression_lz4_AOCL_LZ4_hash5_pass_common_1) // Simple edge cases, checking lowest and highest value behaviour
{
    unsigned long long seq = 0;
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq, 2), 0);

    seq = 0xFFFFFFFFFFFFFFFF;
    EXPECT_LE(Test_AOCL_LZ4_hash5(seq, 2), LZ4_HASH_SIZE_U32);
}

TEST(LZ4_AOCL_LZ4_hash5, AOCL_Compression_lz4_AOCL_LZ4_hash5_pass_common_2)
{
    // Least significant 5 bytes are same, Highest significant 3 bytes may vary
    // Hash function output is expected to be same.
    unsigned long long seq1;
    unsigned long long seq2;
#if AOCL_LZ4_HASH_BITS_USED == 41
    seq1 = 0xFFFFFFFF12345678; // 0xFFFFF'1111'[FF12345678]
    seq2 = 0xFFFFFDFF12345678; // 0xFFFFF'1101'[FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0x000000FF12345678; // 0x00000'0000'[FF12345678]
    seq2 = 0x000002FF12345678; // 0x00000'0010'[FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0x123456FF12345678; // 0x12345'0110'[FF12345678]
    seq2 = 0xFEB124FF12345678; // 0xFEB12'0100'[FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0x000000FF12345678; // 0x000000[FF12345678]  41st bit is different, lower 40bits are same
    seq2 = 0x000001FF12345678; // 0x000001[FF12345678]
    EXPECT_NE(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));
#elif AOCL_LZ4_HASH_BITS_USED == 44
    seq1 = 0xFFFFFFFF12345678; // 0xFFFFF[FFF12345678]
    seq2 = 0xFFFFEFFF12345678; // 0xFFFFE[FFF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0x000000FF12345678; // 0x00000[0FF12345678]
    seq2 = 0x000010FF12345678; // 0x00001[0FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0x123465FF12345678; // 0x12346[5FF12345678]
    seq2 = 0xFEB135FF12345678; // 0xFEB13[5FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0xFFFFFFFF12345678; // 0xFFFFF'1111'[FF12345678] 44th bit is different, lower 43bits are same
    seq2 = 0xFFFFF7FF12345678; // 0xFFFFF'0111'[FF12345678]
    EXPECT_NE(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));
#else
    seq1 = 0xFFFFFFFF12345678; // 0xFFFFFF[FF12345678]
    seq2 = 0xFFFFFEFF12345678; // 0xFFFFFE[FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0x000000FF12345678; // 0x000000[FF12345678]
    seq2 = 0x000001FF12345678; // 0x000001[FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0x123456FF12345678; // 0x123456[FF12345678]
    seq2 = 0xFEB123FF12345678; // 0xFEB123[FF12345678]
    EXPECT_EQ(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0xFFFFFFFF12345678; // 0xFFFFFF'1111'[F12345678] 40th bit is different, lower 39bits are same
    seq2 = 0xFFFFFF7F12345678; // 0xFFFFFF'0111'[F12345678]
    EXPECT_NE(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));
#endif /* AOCL_LZ4_HASH_BITS_USED */
}

TEST(LZ4_AOCL_LZ4_hash5, AOCL_Compression_lz4_AOCL_LZ4_hash5_fail_common_3)
{
    // Most significant 5 bytes are same, but the Least significant 3 bytes differ
    // Hash function output might vary.

    unsigned long long seq1 = 0xFF12345678FFFFFF; // 0x[FF12345678]FFFFFF
    unsigned long long seq2 = 0xFF12345678FFFFFE; // 0x[FF12345678]FFFFFE
    EXPECT_NE(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));

    seq1 = 0xFF12345678123456; // 0x[FF12345678]123456
    seq2 = 0xFF12345678FEB123; // 0x[FF12345678]FEB123
    EXPECT_NE(Test_AOCL_LZ4_hash5(seq1, 2), Test_AOCL_LZ4_hash5(seq2, 2));
}

TEST(LZ4_AOCL_LZ4_hash5, AOCL_Compression_lz4_AOCL_LZ4_hash5_pass_common_4)
{
    // Whatever random value is given as the input,
    // Hash function output is expected to be less than Hash table size.
    
    unsigned long long seq = 0;
    
    for(int i=0; i<100000; i++)
    {
        seq = ((rand()*1ULL)<<32) | (rand()*1ULL); // For generating 64 bit random value.
        EXPECT_LT(Test_AOCL_LZ4_hash5(seq, 2), LZ4_HASH_SIZE_U32);
    }
}
#endif /* AOCL_LZ4_OPT */
/*********************************************
 * "End" of AOCL_LZ4_hash5
 *********************************************/

/*********************************************
 * Begin fuzz tests for lz4
 *********************************************/
#ifdef AOCL_TEST_FUZZER
void LZ4_compress_default_fuzz(vector<char> source, size_t dest_sz,
                               int level, int optOff, int optLevel)
{
    (void)(level);
    aocl_setup_lz4(optOff, optLevel, 0, 0, 0);

    int destLen = dest_sz > INT_MAX ? INT_MAX : dest_sz;
    int srcLen = source.size();
    vector<char> dest(destLen);

    int ret = LZ4_compress_default((const char*)source.data(), dest.data(), srcLen, destLen);

    int OrigLen = source.size();
    vector<char> decompressed(OrigLen);
    //When source size is zero , compress function generates empty block which cannot be decompressed.
    if(ret > 0 && source.size() != 0 )
    {
        int ret2 = LZ4_decompress_safe(dest.data(), decompressed.data(), ret, OrigLen);
        EXPECT_GT(ret2, 0); 
        if(ret2 > 0)       
            EXPECT_EQ(0,memcmp(decompressed.data(),source.data(), source.size()));
    }
    aocl_destroy_lz4();
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_compress_default_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 0),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_cpr_seed_t<char> {
  auto seed_files = READ_FUZZ_CPR_SEED();
  return get_fuzz_cpr_seeds<char>([](size_t src_sz) -> size_t {
    size_t dst_sz = (size_t)LZ4_compressBound((int)src_sz);
    return limit_fuzz_size_max(dst_sz);
  }, 0, 0, seed_files);
})
#endif
;

void LZ4_decompress_safe_fuzz(vector<char> source, size_t dest_sz,
                              int optOff, int optLevel)
{
    aocl_setup_lz4(optOff, optLevel, 0, 0, 0);

    int destLen = dest_sz;
    int srcLen = source.size();
    vector<char> dest(destLen, 0);

    LZ4_decompress_safe(source.data(), dest.data(), srcLen, destLen);

    aocl_destroy_lz4();
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_decompress_safe_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_dpr_seed_t<char> {
  auto seed_files = READ_FUZZ_DPR_SEED();
  return get_fuzz_dpr_seeds<char>(seed_files);
})
#endif
;

void LZ4_compress_fast_fuzz(int dest_len, vector<char> source, int acceleration)
{
    vector<char> dest(dest_len);
    int ret = LZ4_compress_fast((const char *)source.data(), dest.data(), source.size(), dest_len, acceleration);
    int OrigLen = source.size();
    vector<char> decompressed(OrigLen, 0);
    //When source size is zero , compress function generates empty block which cannot be decompressed.
    if(ret > 0 && source.size() != 0)
    {
        int ret2 = LZ4_decompress_safe(dest.data(), decompressed.data(), ret, OrigLen);
        EXPECT_GT(ret2, 0);        
        if(ret2 > 0 )       
            EXPECT_EQ(0,memcmp(decompressed.data(),source.data(), source.size()));
    }
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_compress_fast_fuzz)
.WithDomains(fuzztest::InRange<int>(1,10000),
             fuzztest::Arbitrary<std::vector<char>>(),
             fuzztest::InRange<int>(0, 65538));

void LZ4_compress_destSize_fuzz(int dest_len, vector<char> source)
{
    int srcLen = source.size();
    vector<char> dest(dest_len);
    int ret = LZ4_compress_destSize(source.data(), dest.data(), &srcLen, dest_len);
    int OrigLen = source.size();
    vector<char> decompressed(OrigLen, 0);
    //When source size is zero , compress function generates empty block which cannot be decompressed.
    if(ret > 0 && srcLen != 0)
    {
        int ret2 = LZ4_decompress_safe(dest.data(), decompressed.data(), ret, OrigLen);
        EXPECT_GT(ret2, 0);      
        if(ret2 > 0 )       
            EXPECT_EQ(0,memcmp(decompressed.data(),source.data(), srcLen));
    }
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_compress_destSize_fuzz)
    .WithDomains(fuzztest::InRange<int>(1,10000),
                fuzztest::Arbitrary<std::vector<char>>()
                );

void LZ4_compress_fast_continue_fuzz(vector<char> dict, vector<char> source, int dest_len, int acceleration)
{
    vector<char> dest(dest_len);
    LZ4_stream_t *stream = LZ4_createStream();
    LZ4_loadDict(stream, dict.data(), dict.size());
    int ret = LZ4_compress_fast_continue(stream, source.data(), dest.data(), source.size(), dest.capacity(), acceleration);
    int OrigLen = source.size();
    vector<char> decompressed(OrigLen, 0);
    //When source size is zero , compress function generates empty block which cannot be decompressed.
    if(ret > 0 && source.size() != 0)
    {
        int ret2 = LZ4_decompress_safe(dest.data(), decompressed.data(), ret, OrigLen);
        EXPECT_GT(ret2, 0);        
        if(ret2 > 0 )       
            EXPECT_EQ(0,memcmp(decompressed.data(),source.data(), source.size()));
    }
    free(stream);
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_compress_fast_continue_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1,10000),
                fuzztest::InRange<int>(0, 65538)
                );

void LZ4_decompress_safe_continue_fuzz(vector<char> dict, int orig_len, vector<char> source)
{
    LZ4_streamDecode_t *decode = LZ4_createStreamDecode();
    vector<char> decompressed(orig_len);
    LZ4_setStreamDecode(decode, dict.data(), dict.size());    
    LZ4_decompress_safe_continue(decode, source.data(), decompressed.data(), source.size(), orig_len);
    LZ4_freeStreamDecode(decode);
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_decompress_safe_continue_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1,10000),
                fuzztest::Arbitrary<std::vector<char>>()
                );

void LZ4_decompress_safe_usingDict_fuzz(vector<char> dict, vector<char> source, int dest_len)
{
    vector<char> dest(dest_len);
    LZ4_decompress_safe_usingDict(source.data(), dest.data() , source.size(), dest_len, dict.data(), dict.size());
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_decompress_safe_usingDict_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1,10000)
                );

void LZ4_decompress_safe_usingDict_prefixmode_fuzz(vector<char> dict, vector<char> source, int dest_len)
{
    vector<char> dest(dest_len);
    vector<char> dict_source = dict;    
    dict_source.insert(dict_source.end(), source.begin(), source.end());
    LZ4_decompress_safe_usingDict(dict_source.data() + dict.size(), dest.data() , source.size(), dest_len, dict_source.data(), dict.size());
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_decompress_safe_usingDict_prefixmode_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1,10000)
                );

void LZ4_decompress_safe_partial_usingDict_fuzz(vector<char> dict, vector<char> source, int dest_len)
{
    vector<char> dest(dest_len);
    LZ4_decompress_safe_partial_usingDict(source.data(), dest.data() , source.size(), dest_len, dest_len, dict.data(), dict.size());
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_decompress_safe_partial_usingDict_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1,10000)
                );

void LZ4_decompress_safe_partial_usingDict_prefixmode_fuzz(vector<char> dict, vector<char> source, int dest_len)
{
    vector<char> dest(dest_len);
    vector<char> dict_source = dict;
    dict_source.insert(dict_source.end(), source.begin(), source.end());
    LZ4_decompress_safe_partial_usingDict(dict_source.data()+dict.size(), dest.data() , source.size(), dest_len, dest_len, dict_source.data(), dict.size());
}
FUZZ_TEST(AOCL_Compression_lz4, LZ4_decompress_safe_partial_usingDict_prefixmode_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1,10000)
                );

#endif /* AOCL_TEST_FUZZER */
/*********************************************
 * End fuzz tests for lz4
 *********************************************/
