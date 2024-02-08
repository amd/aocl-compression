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
 
 /** @file zstd_stream_gtest.cpp
 *  
 *  @brief Test cases for ZSTD APIs that use stream.
 *
 *  This file contains the test cases for ZSTD method
 *  testing the API level functions of ZSTD that use stream.
 *
 *  @author Ashish Sriram
 */
#include "zstd_gtest.h"

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

// Insert a valid zstd frame via streaming mode
size_t insert_frame_via_stream(void* dst, size_t dstCapacity, const void* src, size_t srcSize) {
    //create stream object
    ZSTD_parameters p;
    memset(&p, 0, sizeof(ZSTD_parameters));    
    ZSTD_CStream* g_cstream = Test_ZSTD_createCStream();
    Test_ZSTD_initCStream_advanced(g_cstream, NULL, 0, p, ZSTD_CONTENTSIZE_UNKNOWN);

    //compress
    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
    buffOut.dst = dst;
    buffOut.size = dstCapacity;
    buffOut.pos = 0;
    buffIn.src = src;
    buffIn.size = srcSize;
    buffIn.pos = 0;
    size_t ret = Test_ZSTD_compressStream(g_cstream, &buffOut, &buffIn);
    CHECK_PASS_ZSTD(ret);
    if (buffIn.pos > 0) { //compress ok
        ret = Test_ZSTD_endStream(g_cstream, &buffOut);
        CHECK_PASS_ZSTD(ret);
    }

    Test_ZSTD_freeCStream(g_cstream); g_cstream = NULL;
    return buffOut.pos;
}

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
 * Begin of ZSTD_ZSTD_compressStream
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
        CHECK_PASS_ZSTD(ret);
        if (buffIn.pos > 0) { //compress ok
            ret = Test_ZSTD_endStream(g_cstream, &buffOut);
            CHECK_PASS_ZSTD(ret);
        }
        return buffOut.pos;
    }
};

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_1) //compressStream
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_2) //compressStream src NULL
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0); //no error, expects future calls to pass src data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_3) //compressStream dst NULL
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_4) //compressStream dst size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_5) //compressStream src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to pass src data
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_6) //compressStream src NULL and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to pass src data
}

/*********************************************
 * End of ZSTD_ZSTD_compressStream
 *********************************************/

/***********************************************
* Begin of ZSTD_ZSTD_compressStream2
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
        if (!Test_ZSTD_isError(ret))
            return buffOut.pos;
        else
            return ret;
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
        CHECK_PASS_ZSTD(ret);
        ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        CHECK_PASS_ZSTD(ret);
        return buffOut.pos;
    }

    void set_num_ref_threads(int numThreads) {
        ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_nbWorkers, numThreads);
    }
};

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_1) //end
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_fail_common_2) //end src NULL
{
    TestLoad_2 d(1024);
    size_t ret = compress_end(NULL, d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_3) //end dst NULL
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), NULL, d.getCompressedSize());
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_4) //end dst size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), 0);
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(outLen, 0);  //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_5) //end, insufficient dst
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize() - 1);
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_6) //continue & end
{
    TestLoad_2 d(1024);
    size_t outLen = compress_continue(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_7) //end src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(d.getOrigData(), 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_8) //end src NULL and src size 0
{
    TestLoad_2 d(1024);
    size_t outLen = compress_end(NULL, 0, d.getCompressedBuff(), d.getCompressedSize());
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    EXPECT_EQ(*((uint32_t*)d.getCompressedBuff()), ZSTD_MAGICNUMBER); //validate that it is a valid zstd frame
}

#ifdef AOCL_ENABLE_THREADS
/* Library must be built with ZSTD_MULTITHREAD flag defined for this test to take multithreaded code path in zstd reference */
TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_common_7) //reference multi-threaded
{
    TestLoad_2 d((512 * 1024) + 1); //ZSTDMT_JOBSIZE_MIN + 1 : minimum size for reference multi-threading to get activated
    int max_threads = omp_get_max_threads();
    set_num_ref_threads(max_threads);
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}
#endif

/*********************************************
 * End of ZSTD_ZSTD_compressStream2
 *********************************************/

/***********************************************
* Begin of ZSTD_ZSTD_decompressStream
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
        if (!Test_ZSTD_isError(ret))
            return buffOut.pos;
        else
            return ret;
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

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_1) //decompressStream
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(d.getCompressedBuff(), getCompressedSize(), getOutData(), getOutSize());
    CHECK_PASS_ZSTD(decLen);
    ASSERT_EQ(d.getOrigSize(), decLen);
    EXPECT_EQ(0, validate(d.getOrigData(), decLen));
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_2) //decompressStream src NULL
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t ret = decompress(NULL, getCompressedSize(), getOutData(), getOutSize());
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_3) //decompressStream dst NULL
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t ret = decompress(d.getCompressedBuff(), getCompressedSize(), NULL, getOutSize());
    EXPECT_EQ(ret, ERROR(dstBuffer_null));
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_4) //decompressStream dst size 0
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    int decLen = decompress(d.getCompressedBuff(), getCompressedSize(), getOutData(), 0); //no error, expects future calls to flush dst data
    CHECK_PASS_ZSTD(decLen);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to flush dst data
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_5) //decompressStream src size 0
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(d.getCompressedBuff(), 0, getOutData(), getOutSize());
    CHECK_PASS_ZSTD(decLen);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to push src data
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_6) //decompressStream src null and src size 0
{
    TestLoad_2 d(1024);
    compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    size_t decLen = decompress(NULL, 0, getOutData(), getOutSize());
    CHECK_PASS_ZSTD(decLen);
    EXPECT_EQ(decLen, 0); //no error, expects future calls to push src data
}

/*********************************************
 * End of ZSTD_ZSTD_decompressStream
 *********************************************/
