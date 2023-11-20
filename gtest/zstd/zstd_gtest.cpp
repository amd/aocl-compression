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
 
 /** @file zstd_gtest.cc
 *  
 *  @brief Test cases for ZSTD algo.
 *
 *  This file contains the test cases for ZSTD method
 *  testing the API level functions of ZSTD.
 *
 *  @author Partiksha
 */

#include <string>
#include <climits>
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
    char *orig_data;
    size_t orig_sz = 0;
    //destination buffer (kdata obtained after compression).
    char *compressed_data;
    size_t compressed_sz=0;

public:
    // Constructor functions, creates `sz` size of source data.
    TestLoad_2(int sz)
    {
        this->orig_sz = sz;
        orig_data = (char *)malloc(sz);

        // generating random data inside `orig_data` buffer.
        for (int i = 0; i < sz; i++)
        {
            orig_data[i] = rand() % 255;
        }

        // Provides the maximum size that ZSTD compression may output in a "worst case".
        compressed_sz = ZSTD_compressBound(sz);
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
    ~TestLoad_2()
    {
        free(orig_data);
        free(compressed_data);
    }
};

typedef size_t(*ZSTD_decompress_fp)(ZSTD_DCtx* dctx,
    void* dst, size_t dstCapacity,
    const void* src, size_t srcSize);

bool zstd_check_uncompressed_equal_to_original(char *src, unsigned srcSize, 
    char *compressed, unsigned compressedLen, ZSTD_decompress_fp decomp_fp)
{
    int uncompressedLen = srcSize + 10;
    char* uncompressed = (char*)calloc(uncompressedLen, sizeof(char));
    
    
    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    int uncompressedLenRes = decomp_fp(dctx, uncompressed, uncompressedLen, compressed, compressedLen);

    if (uncompressedLenRes < 0) {//error code
        free(uncompressed);
        ZSTD_freeDCtx(dctx);
        return false;
    }

    if (!(srcSize == (unsigned)uncompressedLenRes)) {
        free(uncompressed);
        ZSTD_freeDCtx(dctx);
        return false;
    }

    bool ret = (memcmp(src, uncompressed, srcSize) == 0);
    free(uncompressed);
    ZSTD_freeDCtx(dctx);
    return ret;
}

// Test wrapper function for API ZSTD_versionNumber()
unsigned Test_ZSTD_versionNumber()
{
    return ZSTD_versionNumber();
}

// Test wrapper function for API ZSTD_versionString()
const char* Test_ZSTD_versionString()
{
    return ZSTD_versionString();
}

// Test wrapper function for API ZSTD_compress()
size_t Test_ZSTD_compress(void* dst, size_t dstCapacity,
               const void* src, size_t srcSize,
                     int compressionLevel)
{
    return ZSTD_compress(dst, dstCapacity, src, srcSize, compressionLevel);
}

// Test wrapper function for API ZSTD_decompress()
size_t Test_ZSTD_decompress(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
{
    return ZSTD_decompress(dst, dstCapacity, src, srcSize);
}

// Test wrapper function for ZSTD_isError()
size_t Test_ZSTD_isError(size_t len)
{
    return ZSTD_isError(len);
}

// Test wrapper function for ZSTD_decompressedbound()
unsigned long long Test_ZSTD_decompressBound(const void *src, size_t srcLen)
{
    return ZSTD_decompressBound(src, srcLen); 
}

// Test wrapper function for ZSTD_frameHeaderSize()
size_t Test_ZSTD_frameHeaderSize(const void* src, size_t srcSize)
{
    return ZSTD_frameHeaderSize(src, srcSize);
}

// Test wrapper function for ZSTD_CCtx_setParameter()
size_t Test_ZSTD_CCtx_setParameter(ZSTD_CCtx* cctx, ZSTD_cParameter param, int value) {
    return ZSTD_CCtx_setParameter(cctx, param, value);
}

// Test wrapper function for ZSTD_createCStream()
ZSTD_CStream* Test_ZSTD_createCStream(void) {
    return ZSTD_createCStream();
}

// Test wrapper function for ZSTD_freeCStream()
size_t Test_ZSTD_freeCStream(ZSTD_CStream* zcs) {
    return ZSTD_freeCStream(zcs);
}

// Test wrapper function for ZSTD_initCStream_advanced()
size_t Test_ZSTD_initCStream_advanced(ZSTD_CStream* zcs,
    const void* dict, size_t dictSize,
    ZSTD_parameters params, unsigned long long pss) {
    return ZSTD_initCStream_advanced(zcs, dict, dictSize, params, pss);
}

// Test wrapper function for ZSTD_compressStream()
size_t Test_ZSTD_compressStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output, ZSTD_inBuffer* input) {
    return ZSTD_compressStream(zcs, output, input);
}

// Test wrapper function for ZSTD_endStream()
size_t Test_ZSTD_endStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output) {
    return ZSTD_endStream(zcs, output);
}

// Test wrapper function for ZSTD_compressStream2()
size_t Test_ZSTD_compressStream2(ZSTD_CCtx* cctx,
    ZSTD_outBuffer* output,
    ZSTD_inBuffer* input,
    ZSTD_EndDirective endOp) {
    return ZSTD_compressStream2(cctx, output, input, endOp);
}

// Test wrapper function for ZSTD_initDStream()
size_t Test_ZSTD_initDStream(ZSTD_DStream* zds) {
    return ZSTD_initDStream(zds);
}

// Test wrapper function for ZSTD_decompressStream()
size_t Test_ZSTD_decompressStream(ZSTD_DStream* zds, ZSTD_outBuffer* output, ZSTD_inBuffer* input) {
    return ZSTD_decompressStream(zds, output, input);
}

/***********************************************
 * "Begin" of Version Tests
 ***********************************************/

TEST(ZSTD_versionNumber, AOCL_Compression_zstd_ZSTD_versionNumber_common_1) // Number
{
    EXPECT_EQ(Test_ZSTD_versionNumber(), ZSTD_VERSION_NUMBER);
}

TEST(ZSTD_versionString, AOCL_Compression_zstd_ZSTD_versionString_common_2) // String
{
    EXPECT_STREQ(Test_ZSTD_versionString(), ZSTD_VERSION_STRING);
}
/*********************************************
 * End of Version Tests
 *********************************************/


/***********************************************
 * "Begin" of ZSTD_compress
 ***********************************************/
class ZSTD_ZSTD_compress : public AOCL_setup_zstd {
};

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_ZSTD_ZSTD_compress_common_1) // compress_FAIL_src_is_NULL
{
    TestLoad_2 d(8000);
    
    // ZSTD_error_GENERIC
    size_t outLen = Test_ZSTD_compress(d.getCompressedBuff(), d.getCompressedSize(), NULL, d.getOrigSize(), 1);
    EXPECT_TRUE(Test_ZSTD_isError(outLen));
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_ZSTD_ZSTD_compress_common_2) // Compress_FAIL_dst_is_NULL
{
    TestLoad_2 d(800);

    // ZSTD_error_dstBuffer_null
    size_t outLen = Test_ZSTD_compress(NULL, d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), 1);
    EXPECT_TRUE(Test_ZSTD_isError(outLen));
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_ZSTD_ZSTD_compress_common_3 ) // compress_PASS
{
    for(int cLevel=1; cLevel<=22; cLevel++) {
        TestLoad_2 d(8000);
        size_t outLen = Test_ZSTD_compress(d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(),d.getOrigSize(), cLevel);
        EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
    }
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_ZSTD_ZSTD_compress_common_4) // compress_FAIL_dst_size_not_enough
{
    TestLoad_2 d(800);
    size_t outLen = Test_ZSTD_compress(d.getCompressedBuff(), d.getOrigSize() / 20, d.getOrigData(), d.getOrigSize(), 1);
    EXPECT_TRUE(Test_ZSTD_isError(outLen));
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_ZSTD_ZSTD_compress_common_5) // Compression_level_less_than_minimum_limit
{
    TestLoad_2 d(800);
    int cLevel = -1;
    /* For levels < 1, compression parameters are set to 0th entry of the table `ZSTD_defaultCParameters[4][ZSTD_MAX_CLEVEL+1]`
     * while other levels uses the corresponding entry to set compression parameters.
     */
    size_t outLen = Test_ZSTD_compress(d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cLevel);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_ZSTD_ZSTD_compress_common_6) // Compression_level_greater_than_maximum_limit
{
    TestLoad_2 d(800);
    int cLevel = 23;
    // For level > maximum possible level, level will be set to ZSTD_MAX_CLEVEL, which is 22.
    size_t outLen = Test_ZSTD_compress(d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(),d.getOrigSize(), cLevel);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

/*********************************************
 * End of ZSTD_compress
 *********************************************/


/***********************************************
 * "Begin" of ZSTD_decompress
 ***********************************************/

class ZSTD_ZSTD_decompress : public AOCL_setup_zstd
{
public:
    TestLoad_2 *d = NULL;
    
    // Compressed data is stored in the buffer `src`.
    char *src = NULL;
    int srcLen;
    
    // Original data which we are about to compress is contained in the buffer `original`.
    char *original = NULL;
    int origLen;
    
    // Decompressed data will be stored in the buffer `output`.
    char *output = NULL;
    int outLen;

    //compression Level 
    int cLevel = ZSTD_CLEVEL_DEFAULT;

    // By default the `original` buffer of size 800 is initialized upon testcase initialization.
    void SetUp() override
    {
        d = new TestLoad_2(800);
        original = d->getOrigData();
        origLen = d->getOrigSize();
        src = d->getCompressedBuff();

        // Compress data from `original` buffer to `src` buffer.
        srcLen = Test_ZSTD_compress(src, d->getCompressedSize(), original, origLen, cLevel);
        outLen = d->getOrigSize()*2 + 10;
        output = (char *)malloc(outLen);
    }
    
    // By default the `original` buffer size is set to 800
    // we can use `setOrigSz(int sz)` function to reset it size to sz.
    void setOrigSz(int sz)
    {
        if (d != NULL)
            delete d;
        d = new TestLoad_2(sz);
        original = d->getOrigData();
        origLen = d->getOrigSize();
        srcLen = d->getCompressedSize();
        src = d->getCompressedBuff();
        srcLen = Test_ZSTD_compress(src, srcLen, original, origLen, cLevel);
    }

    // Reset `output` buffer size to `sz`.
    void setDstSz(int sz)
    {
        if (output)
            free(output);
        outLen = sz;
        output = (char *)malloc(outLen);
    }

    // set compression level
    void set_clevel(int level)
    {
        cLevel = level;
    }

    // Destructor function.
    ~ZSTD_ZSTD_decompress()
    {
        if(d)
            delete d;
        if(output)
            free(output);
    }
};

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_common_1) // src_NULL
{
    // ZSTD_error_GENERIC
    size_t decLen = Test_ZSTD_decompress(output, Test_ZSTD_decompressBound(src, srcLen), NULL, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decLen));
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_common_2) // dst_NULL
{
    // ZSTD_error_dstBuffer_null
    size_t decLen = Test_ZSTD_decompress(NULL, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decLen));
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_common_3) // successfull_decompression
{
    int decLen = Test_ZSTD_decompress(output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    ASSERT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, decLen));
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_common_4) // decompression_buffer_inadequate
{
    size_t decompressedLen = Test_ZSTD_decompress(output, srcLen / 20, src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decompressedLen));
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_common_5) // compressed_size_zero
{
    EXPECT_EQ(Test_ZSTD_decompress(output, Test_ZSTD_decompressBound(src, srcLen), src, 0), 0);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_common_6) // compressed_data_contains_errors
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

    size_t decompressedLen = Test_ZSTD_decompress(output, decompress_bound, dst, dstCapacity);
    EXPECT_TRUE(Test_ZSTD_isError(decompressedLen));


    // case 2: modifying compressed data of class to introduce error.
    frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
    decompress_bound = Test_ZSTD_decompressBound(src, srcLen);
    this->src[frameHeaderSize+1] = 'e';
    this->src[frameHeaderSize+2] = 'r';
    this->src[frameHeaderSize+3] = 'r';
    this->src[frameHeaderSize+4] = 'o';
    this->src[frameHeaderSize+5] = 'r';

    size_t decompressedLen_2 = Test_ZSTD_decompress(output, decompress_bound, src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decompressedLen_2));
}

/*********************************************
 * End of ZSTD_decompress
 *********************************************/

/*********************************************
 * Begin of ZSTD_getframeContentSize
 *********************************************/

#define ZSTD_CONTENTSIZE_UNKNOWN (0ULL - 1)
#define ZSTD_CONTENTSIZE_ERROR   (0ULL - 2)
class ZSTD_getframeContentSize: public ZSTD_ZSTD_decompress
{
protected:
    // Test wrapper function for API ZSTD_getFrameContentSize()
    unsigned long long Test_ZSTD_getFrameContentSize(const void *src, size_t srcSize)
    {
        return ZSTD_getFrameContentSize(src, srcSize);
    }
};

TEST_F(ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_common_1)   // pass- size >= `ZSTD_frameHeaderSize_max`
{
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, srcLen);
    int decLen = Test_ZSTD_decompress(output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_EQ(val, decLen);
}

TEST_F(ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_common_2)   // Fail- size < `ZSTD_frameHeaderSize_min i.e, 1`
{
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, 1);
    EXPECT_EQ(val, ZSTD_CONTENTSIZE_ERROR);
}

/*********************************************
 * End of ZSTD_getframeContentSize
 *********************************************/


/*********************************************
 * Begin of ZSTD_getDecompressedSize
 *********************************************/

class ZSTD_ZSTD_getDecompressedSize: public ZSTD_ZSTD_decompress
{
protected:
    // Test wrapper function for API ZSTD_getDecompressedSize()
    unsigned long long Test_ZSTD_getDecompressedSize(const void* src, size_t srcSize)
    {
        return ZSTD_getDecompressedSize(src, srcSize);
    }
};

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_common_1)   // pass- size is valid
{
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, srcLen);
    int decLen = Test_ZSTD_decompress(output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_EQ(val, decLen);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_common_2)   // fail- size = 1
{
    EXPECT_EQ(Test_ZSTD_getDecompressedSize(src, 1), 0);
}

/*********************************************
 * End of ZSTD_getDecompressedSize
 *********************************************/


/*********************************************
 * Begin of ZSTD_findFrameCompressedSize
 *********************************************/

class ZSTD_ZSTD_findFrameCompressedSize: public ZSTD_ZSTD_decompress
{
protected:
    // Test wrapper function for API ZSTD_findFrameCompressedSize()
    size_t Test_ZSTD_findFrameCompressedSize(const void *src, size_t srcSize)
    {
        return ZSTD_findFrameCompressedSize(src, srcSize);
    }
};

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_common_1)   // srcLen >= at least as large as the frame contained
{
    while(srcLen >= 5){
        unsigned long long frameSrcSize = Test_ZSTD_findFrameCompressedSize(src, srcLen);
        src = src + frameSrcSize;
        srcLen -= frameSrcSize;
    }
    EXPECT_EQ(0, srcLen);
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_common_2)   // srcLen < the size that frame contained
{
    srcLen = 5;
    unsigned long long frameSrcSize = Test_ZSTD_findFrameCompressedSize(src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(frameSrcSize)); 
}

/*********************************************
 * End of ZSTD_findFrameCompressedSize
 *********************************************/


/*********************************************
 * Begin of ZSTD_compress_advanced
 *********************************************/

class ZSTD_ZSTD_compressed_advanced: public AOCL_setup_zstd
{
public:
    // dctx is a paramter in `ZSTD_ZSTD_decompressDCtx`
    ZSTD_CCtx* cctx;
    ZSTD_parameters param;
    
    // constructor to create dctx
    void SetUp() override
    {
        cctx = ZSTD_createCCtx();
    }

    // Test wrapper function for API ZSTD_compress_advanced()
    size_t Test_ZSTD_compress_advanced (ZSTD_CCtx* cctx,
                               void* dst, size_t dstCapacity,
                         const void* src, size_t srcSize,
                         const void* dict,size_t dictSize,
                               ZSTD_parameters params)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        return ZSTD_compress_advanced(cctx, dst, dstCapacity, src, srcSize, dict, dictSize, params);
#pragma GCC diagnostic pop
    }

    ~ZSTD_ZSTD_compressed_advanced()
    {
        ZSTD_freeCCtx(cctx);
    }

};

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_common_1)
{
    TestLoad_2 d(8000);
    int level = 9;

    param = ZSTD_getParams(level, d.getOrigSize(), 0);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, level);

    size_t outLen_1 = Test_ZSTD_compress_advanced(cctx, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, 0, param);

    aocl_setup_zstd_encode(1, 0, 0, 0, 0);   // optoff = 1
    
    param = ZSTD_getParams(level, d.getOrigSize(), 0);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, level);

    size_t outLen_2 = Test_ZSTD_compress_advanced(cctx, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, 0, param);

    EXPECT_EQ(outLen_1, outLen_2);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen_1, ZSTD_decompressDCtx));
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen_2, ZSTD_decompressDCtx));
}

#ifdef AOCL_ENABLE_THREADS
TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_common_2) // compress multithreaded. decompress reference. format compliance test.
{
    TestLoad_2 d(1024 * 1024 * 32); //use larger input so that compression gets triggered on multiple threads
    int level = 3;

    param = ZSTD_getParams(level, d.getOrigSize(), 0);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, level);

    //Compress using multithreaded compressor
    size_t outLen = Test_ZSTD_compress_advanced(cctx, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, 0, param);
    //Decompress using reference decompressor Test_ZSTD_decompressDCtxRef
    //As ZSTD writes RAP frame inside skippable frame, compressed output must be format compliant
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, 
                Test_ZSTD_decompressDCtxRef));
}
#endif

/*********************************************
 * End of ZSTD_compress_advanced
 *********************************************/


/*********************************************
 * Begin of ZSTD_decompressDCtx
 *********************************************/
class ZSTD_ZSTD_decompressDCtx: public ZSTD_ZSTD_decompress
{
public:
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

    size_t decompressedLen = Test_ZSTD_decompressDCtx(dctx, output, decompress_bound, dst, dstCapacity);
    EXPECT_TRUE(Test_ZSTD_isError(decompressedLen));


    // case 2: modifying compressed data of class to introduce error.
    frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
    decompress_bound = Test_ZSTD_decompressBound(src, srcLen);
    this->src[frameHeaderSize+1] = 'e';
    this->src[frameHeaderSize+2] = 'r';
    this->src[frameHeaderSize+3] = 'r';
    this->src[frameHeaderSize+4] = 'o';
    this->src[frameHeaderSize+5] = 'r';

    size_t decompressedLen_2 = Test_ZSTD_decompressDCtx(dctx, output, decompress_bound, src, srcLen);
    EXPECT_TRUE(Test_ZSTD_isError(decompressedLen_2));

}

/*********************************************
 * End of ZSTD_decompressDCtx
 *********************************************/

#ifdef AOCL_ZSTD_OPT
 /*********************************************
  * Begin of ZSTD_AOCL_ZSTD_row_getMatchMask
  *********************************************/
class ZSTD_AOCL_ZSTD_row_getMatchMask : public ::testing::TestWithParam<int> {
public:
    typedef unsigned char BYTE;
    enum MATCH_TYPE { MATCH_TYPE_ALL, MATCH_TYPE_SOME, MATCH_TYPE_NONE };
    void SetUp() override {
        srand(0);
        rowEntries = GetParam();
        tagRow = (BYTE*)malloc(sizeof(BYTE) * rowEntries);
        headRow = (BYTE*)malloc(sizeof(BYTE) * rowEntries);
    }

    void TearDown() override {
        if (tagRow)
            free(tagRow);
        if (headRow)
            free(headRow);
    }

    //Fill bytes in tagRow based on MATCH_TYPE selected
    void setup_tag_row(BYTE tag, unsigned matchPos[], int matchSz, MATCH_TYPE type) {
        if (type == MATCH_TYPE_ALL) {
            for (int i = 0; i < rowEntries; ++i) { //fill all with tag bytes
                tagRow[i] = tag;
            }
        }
        else {
            for (int i = 0; i < rowEntries; ++i) { //fill with random non-tag bytes
                BYTE cur = rand() % 256;
                if (cur == tag)
                    cur++;
                tagRow[i] = cur;
            }
        }

        if (type == MATCH_TYPE_SOME) {
            for (int i = 0; i < matchSz; ++i) { //set some bytes to tag
                ASSERT_LT(matchPos[i], rowEntries);
                tagRow[matchPos[i]] = tag;
            }
        }
    }

    //validate if expected mask is generated
    void validate_mask(BYTE tag, U64 mask, int head) {
        //rotate tagRow left by head and save in headRow
        ASSERT_LT(head, rowEntries);
        head = (rowEntries - head) % rowEntries;
        for (int i = 0; i < rowEntries; ++i) {
            int headpos = (head + i) % rowEntries;
            headRow[headpos] = tagRow[i];
        }
        
        //check each bit in mask
        for (int i = 0; i < rowEntries; ++i) {
            int curbit = mask & 0x01;
            if (i == head) {
                //test head pos bit. Should always be 0.
                EXPECT_EQ(curbit, 0);
            }
            else {
                //test rest of the mask
                if (headRow[i] == tag) {
                    EXPECT_EQ(curbit, 1);
                }
                else {
                    EXPECT_EQ(curbit, 0);
                }
            }
            mask >>= 1;
        }
    }

    uint32_t rowEntries; //size of tag row
    BYTE* tagRow; //input tag row bytes
    BYTE* headRow; //temp buffer to hold rotated tag bytes
};

// all bytes set to tag
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_allMatch_common_1) {
    BYTE tag = 'a';
    const int head = 0;
    setup_tag_row(tag, { 0 }, 0, MATCH_TYPE_ALL);
    U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
    validate_mask(tag, mask, head);
}

// some bytes set to tag
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_someMatch_common_1) {
    BYTE tag = 'a';
    const int head = 0;
    unsigned matchPos[6] = { 1, 3, 7, 8, rowEntries - 3, rowEntries - 1 };
    setup_tag_row(tag, matchPos, 6, MATCH_TYPE_SOME);
    U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
    validate_mask(tag, mask, head);
}

// no bytes set to tag
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_nonematch_common_1) {
    BYTE tag = 'a';
    const int head = 0;
    setup_tag_row(tag, { 0 }, 0, MATCH_TYPE_NONE);
    U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
    validate_mask(tag, mask, head);
}

// test with head rotation
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_headRotated_common_1) {
    BYTE tag = 'a';
    for (int head = 1; head < rowEntries; ++head) {
        unsigned matchPos[6] = { 1, 3, 7, 8, rowEntries - 3, rowEntries - 1 };
        setup_tag_row(tag, matchPos, 6, MATCH_TYPE_ALL);
        U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
        validate_mask(tag, mask, head);
    }
}

INSTANTIATE_TEST_SUITE_P(
    AOCL_ZSTD_row_getMatchMask_TEST,
    ZSTD_AOCL_ZSTD_row_getMatchMask,
    ::testing::ValuesIn({ 16, 32, 64 })); // 3 configurations supported for rowEntries

/*********************************************
 * End of ZSTD_AOCL_ZSTD_row_getMatchMask
 *********************************************/
#endif

 /*********************************************
  * Begin of ZSTD_ZSTD_selectBlockCompressor
  *********************************************/

  // Test valid compressors are set on optOff
TEST(ZSTD_ZSTD_selectBlockCompressor, AOCL_Compression_zstd_ZSTD_selectBlockCompressor_optOff_common_1)
{
    int aoclOptFlag = 0; //optOff
    for (int strat = 1; strat <= 9; ++strat) { //Refer to ZSTD_strategy for valid range of values
        for (int useRowMatchFinder = 0; useRowMatchFinder <= 1; useRowMatchFinder++) {
            for (int dictMode = 0; dictMode <= 3; ++dictMode) { //Refer to ZSTD_dictMode_e for valid range of values
                int ret = Test_ZSTD_selectBlockCompressor(strat, useRowMatchFinder, dictMode, aoclOptFlag);
                EXPECT_EQ(ret, 0);
            }
        }
    }
}

// Test valid compressors are set on optOn
TEST(ZSTD_ZSTD_selectBlockCompressor, AOCL_Compression_zstd_ZSTD_selectBlockCompressor_optOn_common_1)
{
    int aoclOptFlag = 1; //optOn
    for (int strat = 1; strat <= 9; ++strat) { //Refer to ZSTD_strategy for valid range of values
        for (int useRowMatchFinder = 0; useRowMatchFinder <= 1; useRowMatchFinder++) {
            for (int dictMode = 0; dictMode <= 3; ++dictMode) { //Refer to ZSTD_dictMode_e for valid range of values
                int ret = Test_ZSTD_selectBlockCompressor(strat, useRowMatchFinder, dictMode, aoclOptFlag);
                EXPECT_EQ(ret, 0);
            }
        }
    }
}

/*********************************************
* End of ZSTD_ZSTD_selectBlockCompressor
*********************************************/

#ifdef AOCL_ZSTD_OPT
/*********************************************
* Begin of ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long
*********************************************/
class ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long : public AOCL_setup_zstd
{
public:
    ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long() : stream(nullptr), src(nullptr), dst(nullptr) {}

    ~ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long() {
        if (stream) free(stream);
    }

    void create(size_t len, size_t slen) {
        ASSERT_GE(len, slen * 2); //enough space to hold src and dst
        buf_len = len + WILDCOPY_OVERLENGTH;
        src_len = slen;

        stream = (char*)malloc(sizeof(char) * buf_len);
        src = stream;
        reset();
    }

    void reset() {
        ASSERT_NE(src, nullptr);
        memset(stream, 0, sizeof(char) * buf_len);

        for (size_t i = 0; i < src_len; ++i) { //fill non-0 values for src
            src[i] = (i % 256);
            if (src[i] == 0) src[i] = 1;
        }
    }

    void setDst(size_t pos) {
        ASSERT_LE(pos + src_len, buf_len - WILDCOPY_OVERLENGTH); //enough space in dst to hold src_len
        ASSERT_NE(src, nullptr);
        dst = src + pos;
    }

    void validate() {
        EXPECT_EQ(memcmp(src, dst, src_len), 0); //validate src and dst are equal


        if (dst > (src + src_len))
        { //validate bytes HERE are not polluted: [src...src+length..<HERE>..dst..dst+len]
            bool polluted = false;
            char* cur = src + src_len;
            char* end = dst;
            while (cur < end) {
                if ((*cur) != 0) {
                    polluted = true;
                    break;
                }
                cur++;
            }
            EXPECT_NE(polluted, true);
        }
        //[src...src+length...dst..dst+len..<HERE>..buf_len]. Ok to pollute here
    }

    char* getSrc() {
        return src;
    }

    char* getDst() {
        return dst;
    }

private:
    char* stream, *src, *dst;
    size_t buf_len, src_len;
};

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_1) // (dst-src) >= WILDCOPY_VECLEN, length < WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN - 1;
    size_t test_cnt = 8;
    size_t buf_len = length + WILDCOPY_VECLEN + test_cnt; //(dst bytes) + (gap btw dst,src) + (test for > WILDCOPY_VECLEN gap)

    int ovtype = 0;
    create(buf_len, length);

    for (int i = WILDCOPY_VECLEN; i < (WILDCOPY_VECLEN + test_cnt); ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_2) // (dst-src) >= WILDCOPY_VECLEN, length > WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN + 1;
    size_t test_cnt = 8;
    size_t buf_len = length + WILDCOPY_VECLEN + test_cnt; //(dst bytes) + (gap btw dst,src) + (test for > WILDCOPY_VECLEN gap)

    int ovtype = 0;
    create(buf_len, length);

    for (int i = WILDCOPY_VECLEN; i < (WILDCOPY_VECLEN + test_cnt); ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_3) // 8 <= (dst-src) < WILDCOPY_VECLEN, length < WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN - 1;
    size_t buf_len = length + WILDCOPY_VECLEN; //(dst bytes) + (gap btw dst,src)

    int ovtype = 1;
    create(buf_len, length);

    for (int i = 8; i < WILDCOPY_VECLEN; ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_4) // 8 <= (dst-src) < WILDCOPY_VECLEN, length > WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN + 1;
    size_t buf_len = length + length; //(dst bytes) + (src bytes)

    int ovtype = 1;
    create(buf_len, length);

    for (int i = 8; i < WILDCOPY_VECLEN; ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

/*********************************************
* End of ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long
*********************************************/
#endif /* AOCL_ZSTD_OPT */

#ifdef AOCL_ENABLE_THREADS
/*********************************************
* Begin of ZSTD_GET_WINDOW_FACTOR
*********************************************/
TEST(ZSTD_ZSTD_GET_WINDOW_FACTOR, AOCL_Compression_zstd_ZSTD_GET_WINDOW_FACTOR_common_1)
{
    size_t srcSize = 0;
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (100 * 1024 * 1024) - 1; //< 100 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (100 * 1024 * 1024); //100 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (100 * 1024 * 1024) + 1; //>100 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (200 * 1024 * 1024); //200 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 2);

    srcSize = (300 * 1024 * 1024); //300 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 3);

    srcSize = (400 * 1024 * 1024); //400 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 4);

    srcSize = (400 * 1024 * 1024) + 1; //>400 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 4);
}
/*********************************************
* End of ZSTD_GET_WINDOW_FACTOR
*********************************************/
#endif /* AOCL_ENABLE_THREADS */

#define ZSTD_EXPECT_NO_ERR 0
#define ZSTD_EXPECT_ERR 1
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
 * "Begin" of ZSTD_compress2
 ***********************************************/
class ZSTD_ZSTD_compress2 : public ZSTD_with_cctx {
public:
    size_t compress(const void* src, size_t srcSize, void* dst, size_t dstCapacity) 
    {
        return ZSTD_compress2(getCtx(), dst, dstCapacity, src, srcSize);
    }
};

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_ZSTD_ZSTD_compress2_pass_common_1) //compress2
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_ZSTD_ZSTD_compress2_fail_common_2) //compress2 src null
{
    TestLoad_2 d(1024);
    size_t ret = compress(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_ZSTD_ZSTD_compress2_fail_common_3) //compress2 dst null
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_ZSTD_ZSTD_compress2_fail_common_4) //compress2 dst size 0
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_ZSTD_ZSTD_compress2_fail_common_5) //compress2 cctx null
{
    TestLoad_2 d(1024);
    size_t ret = ZSTD_compress2(NULL, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_ZSTD_ZSTD_compress2_pass_common_6) //compress2 src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return valid zstd frame 
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_ZSTD_ZSTD_compress2_pass_common_7) //compress2 src null and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return valid zstd frame 
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}
/*********************************************
 * End of ZSTD_compress2
 *********************************************/

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

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_pass_common_1) //compressCCtx
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_fail_common_2) //compressCCtx src null
{
    TestLoad_2 d(1024);
    size_t ret = compress(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_fail_common_3) //compressCCtx dst null
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_fail_common_4) //compressCCtx dst size 0
{
    TestLoad_2 d(1024);
    size_t ret = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_fail_common_5) //compressCCtx cctx null
{
    TestLoad_2 d(1024);
    size_t ret = ZSTD_compressCCtx(NULL, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), 9);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_pass_common_6) //compressCCtx src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return valid zstd frame 
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_pass_common_7) //compressCCtx src null and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return valid zstd frame 
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_pass_common_8) //compressCCtx compressionLevel < min
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize(), -1);
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_ZSTD_ZSTD_compressCCtx_pass_common_9) //compressCCtx compressionLevel > max
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize(), 23);
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}
/*********************************************
 * End of ZSTD_compressCCtx
 *********************************************/

class ZSTD_stream_base : public AOCL_setup_zstd {
public:
    ZSTD_stream_base() 
    {
        if (g_cstream == NULL) g_cstream = Test_ZSTD_createCStream();
        if (g_dstream == NULL) g_dstream = ZSTD_createDStream();
    }

    void setup(ZSTD_compressionParameters* cparamsPtr, int level = -1) 
    {
        if (cparamsPtr == NULL) {
            // { W,  C,  H,   S,  L, TL, strat }
            cparams = ZSTD_compressionParameters{ 22, 20, 21,  4,  5, 16, ZSTD_lazy2 }; /* settings for : srcSize > 256 KB, level  9 */
        }
        else {
            cparams = *cparamsPtr;
        }
        Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_windowLog, (int)cparams.windowLog);
        Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_hashLog, (int)cparams.hashLog);
        Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_chainLog, (int)cparams.chainLog);
        Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_searchLog, (int)cparams.searchLog);
        Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_minMatch, (int)cparams.minMatch);
        Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_targetLength, (int)cparams.targetLength);
        Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_strategy, cparams.strategy);

        if (level == -1) {
            Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_compressionLevel, 9);
        }
        else {
            Test_ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_compressionLevel, level);
        }
    }

    virtual ~ZSTD_stream_base() 
    {
        Test_ZSTD_freeCStream(g_cstream); g_cstream = NULL;
        ZSTD_freeDStream(g_dstream); g_dstream = NULL;
    }

protected:
    ZSTD_compressionParameters cparams;
    ZSTD_CStream* g_cstream = NULL;
    ZSTD_DStream* g_dstream = NULL;
};

/***********************************************
 * "Begin" of ZSTD_ZSTD_compressStream
 ***********************************************/
class ZSTD_ZSTD_compressStream : public ZSTD_stream_base {
public:
    ZSTD_ZSTD_compressStream() : ZSTD_stream_base() 
    {
        setup(NULL);

        ZSTD_parameters p;
        ZSTD_frameParameters f = { 1 /* contentSizeHeader*/, 0, 0 };
        p.fParams = f;
        p.cParams = cparams;

        Test_ZSTD_initCStream_advanced(g_cstream, NULL, 0, p, ZSTD_CONTENTSIZE_UNKNOWN);
    }

    size_t compress(const void* src, size_t srcSize, void* dst, size_t dstCapacity) 
    {
        ZSTD_outBuffer buffOut;
        ZSTD_inBuffer buffIn;
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        /* Streaming APIs expect ctx, output and input buffers to be valid non-NULL objects. Hence, not testing for these. */
        size_t ret = Test_ZSTD_compressStream(g_cstream, &buffOut, &buffIn);
        EXPECT_EQ(ZSTD_isError(ret), ZSTD_EXPECT_NO_ERR);
        if (buffIn.pos > 0) { //compress ok
            ret = Test_ZSTD_endStream(g_cstream, &buffOut);
            EXPECT_EQ(ZSTD_isError(ret), ZSTD_EXPECT_NO_ERR);
        }
        return buffOut.pos;
    }
};

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_ZSTD_ZSTD_compressStream_pass_common_1) //compressStream
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_ZSTD_ZSTD_compressStream_pass_common_2) //compressStream src NULL
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0); //no error, expects future calls to pass src data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_ZSTD_ZSTD_compressStream_pass_common_3) //compressStream dst NULL
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_ZSTD_ZSTD_compressStream_pass_common_4) //compressStream dst size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_ZSTD_ZSTD_compressStream_pass_common_5) //compressStream src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to pass src data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_ZSTD_ZSTD_compressStream_pass_common_6) //compressStream src NULL and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to pass src data
}

/*********************************************
 * End of ZSTD_ZSTD_compressStream
 *********************************************/

/***********************************************
 * "Begin" of ZSTD_ZSTD_compress_extDict
 ***********************************************/
#define FIRST_BLOCK_SIZE 8
class ZSTD_ZSTD_compress_extDict : public AOCL_setup_zstd {
public:
    ZSTD_parameters setup(int level, int opt_on) {
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(512 KB, level, opt_on);
        if (g_zcc == NULL) g_zcc = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_compressionLevel, level);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_windowLog, (int)cparams.windowLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_hashLog, (int)cparams.hashLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_chainLog, (int)cparams.chainLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_searchLog, (int)cparams.searchLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_minMatch, (int)cparams.minMatch);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_targetLength, (int)cparams.targetLength);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_strategy, cparams.strategy);

        ZSTD_parameters p;
        ZSTD_frameParameters f = { 1 /* contentSizeHeader*/, 0, 0 };
        p.fParams = f;
        p.cParams = cparams;

        return p;
    }

    /* compress in multiple blocks. 1st block gets used as dict for 2nd block and
    *_extDict functions gets called for the 2nd block */
    size_t multi_block_compress(ZSTD_parameters p, void* dst, size_t dstCapacity, void* src, size_t srcSize) {
        BYTE firstBlockBuf[FIRST_BLOCK_SIZE];
        size_t ret = ZSTD_compressBegin_advanced(g_zcc, NULL, 0, p, srcSize);
        if (ZSTD_isError(ret)) {
            EXPECT_EQ(ret, 0); //ret > 0 if error. Test should fail.
            return 0;
        }
        memcpy(firstBlockBuf, src, FIRST_BLOCK_SIZE);

        size_t outLen = 0;
        //compress first block
        //first block acts as extDict for 2nd block
        {
            ret = ZSTD_compressContinue(g_zcc,
                dst, dstCapacity,
                firstBlockBuf, FIRST_BLOCK_SIZE);
            if (ZSTD_isError(ret)) {
                EXPECT_EQ(ret, 0); //ret > 0 if error. Test should fail.
                return 0;
            }
            dst = (BYTE*)dst + ret;
            dstCapacity -= ret;
            outLen += ret;
        }

        //compress second block
        ret = ZSTD_compressEnd(g_zcc, dst, dstCapacity,
            (const BYTE*)src + FIRST_BLOCK_SIZE,
            srcSize - FIRST_BLOCK_SIZE);
        if (ZSTD_isError(ret)) {
            EXPECT_EQ(ret, 0); //ret > 0 if error. Test should fail.
            return 0;
        }
        outLen += ret;

        return outLen;
    }

private:
    ZSTD_CCtx* g_zcc = NULL;
};


TEST_F(ZSTD_ZSTD_compress_extDict, AOCL_Compression_ZSTD_ZSTD_compressStream_common_1) //compress multiple blocks
{
    for (int level = 0; level <= 22; ++level) {
        for (int opt_on = 0; opt_on <= 1; ++opt_on) {
            //setup
            TestLoad_2 d(1024);
            void* src = d.getOrigData();
            size_t srcSize = d.getOrigSize();
            void* dst = d.getCompressedBuff();
            size_t dstCapacity = d.getCompressedSize();
            ZSTD_parameters p = setup(level, opt_on);

            //compress
            size_t outLen = multi_block_compress(p, dst, dstCapacity, src, srcSize);

            //validate
            EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
        }
    }
}

/*********************************************
 * End of ZSTD_ZSTD_compress_extDict
 *********************************************/

 /***********************************************
  * "Begin" of ZSTD_ZSTD_compressStream2
  ***********************************************/
class ZSTD_ZSTD_compressStream2 : public ZSTD_stream_base {
public:
    ZSTD_ZSTD_compressStream2() : ZSTD_stream_base() 
    {
        setup(NULL);
    }

    size_t compress_end(const void* src, size_t srcSize, void* dst, size_t dstCapacity) 
    {
        ZSTD_outBuffer buffOut;
        ZSTD_inBuffer buffIn;
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        /* Streaming APIs expect ctx, output and input buffers to be valid non-NULL objects. Hence, not testing for these. */
        size_t ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        return buffOut.pos;
    }

    size_t compress_continue(const void* src, size_t srcSize, void* dst, size_t dstCapacity) 
    {
        ZSTD_outBuffer buffOut;
        ZSTD_inBuffer buffIn;
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        size_t ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_continue);
        EXPECT_EQ(ZSTD_isError(ret), ZSTD_EXPECT_NO_ERR);
        ret        = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        EXPECT_EQ(ZSTD_isError(ret), ZSTD_EXPECT_NO_ERR);
        return buffOut.pos;
    }

    void set_num_ref_threads(int numThreads) {
        ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_nbWorkers, numThreads);
    }
};

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_1) //end
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_2) //end src NULL
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to push src data
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_3) //end dst NULL
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_4) //end dst size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_5) //end, insufficient dst
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize() - 1);
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_6) //continue & end
{
    TestLoad_2 d(1024);
    size_t outLen = compress_continue(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_7) //end src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return valid zstd frame 
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_pass_common_8) //end src NULL and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return valid zstd frame 
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

#ifdef AOCL_ENABLE_THREADS
/* Library must be built with ZSTD_MULTITHREAD flag defined for this test to take multithreaded code path in zstd reference */
TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_ZSTD_ZSTD_compressStream2_common_7) //reference multi-threaded
{
    TestLoad_2 d((512 * 1024) + 1); //ZSTDMT_JOBSIZE_MIN + 1 : minimum size for reference multi-threading to get activated
    int max_threads = omp_get_max_threads();
    set_num_ref_threads(max_threads);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ZSTD_isError(outLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}
#endif

/*********************************************
 * End of ZSTD_ZSTD_compressStream2
 *********************************************/

/***********************************************
* "Begin" of ZSTD_ZSTD_decompressStream
***********************************************/
class ZSTD_ZSTD_decompressStream : public ZSTD_stream_base {
public:
    ZSTD_ZSTD_decompressStream() : ZSTD_stream_base() {}

    ~ZSTD_ZSTD_decompressStream() 
    {
        if (out) free(out);
    }

    void compress(const void* src, size_t srcSize, void* dst, size_t dstCapacity) 
    {
        g_cSize = Test_ZSTD_compress(dst, dstCapacity, src, srcSize, 9);
        outCapacity = srcSize;
        out = (char*)malloc(outCapacity);
        ASSERT_GT(g_cSize, 0);
    }

    size_t decompress(const void* src, size_t srcSize, void* dst, size_t dstCapacity) 
    {
        ZSTD_outBuffer buffOut;
        ZSTD_inBuffer buffIn;
        Test_ZSTD_initDStream(g_dstream);
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        /* Streaming APIs expect ctx, output and input buffers to be valid non-NULL objects. Hence, not testing for these. */
        size_t ret = Test_ZSTD_decompressStream(g_dstream, &buffOut, &buffIn);
        return buffOut.pos;
    }

    int validate(char* ref, size_t len) 
    {
        return memcmp(out, ref, len);
    }

    char* getOutData() 
    {
        return out;
    }

    size_t getOutSize() 
    {
        return outCapacity;
    }

    size_t getCompressedSize() 
    {
        return g_cSize;
    }

private:
    char* out;
    size_t outCapacity;
    size_t g_cSize;
};

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_ZSTD_ZSTD_decompressStream_pass_common_1) //decompressStream
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(d.getCompressedBuff(), getCompressedSize(), getOutData(), getOutSize());
    EXPECT_EQ(ZSTD_isError(decLen), ZSTD_EXPECT_NO_ERR);
    ASSERT_EQ(d.getOrigSize(), decLen);
    EXPECT_EQ(0, validate(d.getOrigData(), decLen));
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_ZSTD_ZSTD_decompressStream_pass_common_2) //decompressStream src NULL
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(NULL, getCompressedSize(), getOutData(), getOutSize());
    EXPECT_EQ(ZSTD_isError(decLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to push src data
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_ZSTD_ZSTD_decompressStream_pass_common_4) //decompressStream dst NULL
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(d.getCompressedBuff(), getCompressedSize(), NULL, getOutSize());  //no error, expects future calls to flush dst data
    EXPECT_EQ(ZSTD_isError(decLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_ZSTD_ZSTD_decompressStream_pass_common_5) //decompressStream dst size 0
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    int decLen = decompress(d.getCompressedBuff(), getCompressedSize(), getOutData(), 0); //no error, expects future calls to flush dst data
    EXPECT_EQ(ZSTD_isError(decLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_ZSTD_ZSTD_decompressStream_pass_common_6) //decompressStream src size 0
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(d.getCompressedBuff(), 0, getOutData(), getOutSize());
    EXPECT_EQ(ZSTD_isError(decLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to push src data
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_ZSTD_ZSTD_decompressStream_pass_common_7) //decompressStream src null and src size 0
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(NULL, 0, getOutData(), getOutSize());
    EXPECT_EQ(ZSTD_isError(decLen), ZSTD_EXPECT_NO_ERR);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to push src data
}

/*********************************************
 * End of ZSTD_ZSTD_decompressStream
 *********************************************/
