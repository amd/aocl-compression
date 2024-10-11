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

ZSTD_CStream* Test_ZSTD_createCStream(void) {
    return ZSTD_createCStream();
}

ZSTD_DStream* Test_ZSTD_createDStream(void) {
    return ZSTD_createDStream();
}

size_t Test_ZSTD_freeCStream(ZSTD_CStream* zcs) {
    return ZSTD_freeCStream(zcs);
}

size_t Test_ZSTD_freeDStream(ZSTD_DStream* zds) {
    return ZSTD_freeDStream(zds);
}

size_t Test_ZSTD_initCStream_advanced(ZSTD_CStream* zcs,
    const void* dict, size_t dictSize,
    ZSTD_parameters params, unsigned long long pss) {
    return ZSTD_initCStream_advanced(zcs, dict, dictSize, params, pss);
}

size_t Test_ZSTD_compressStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output, ZSTD_inBuffer* input) {
    return ZSTD_compressStream(zcs, output, input);
}

size_t Test_ZSTD_endStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output) {
    return ZSTD_endStream(zcs, output);
}

size_t Test_ZSTD_flushStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output) {
    return ZSTD_flushStream(zcs, output);
}

size_t Test_ZSTD_compressStream2(ZSTD_CCtx* cctx,
    ZSTD_outBuffer* output,
    ZSTD_inBuffer* input,
    ZSTD_EndDirective endOp) {
    return ZSTD_compressStream2(cctx, output, input, endOp);
}

size_t Test_ZSTD_initDStream(ZSTD_DStream* zds) {
    return ZSTD_initDStream(zds);
}

size_t Test_ZSTD_decompressStream(ZSTD_DStream* zds, ZSTD_outBuffer* output, ZSTD_inBuffer* input) {
    return ZSTD_decompressStream(zds, output, input);
}

size_t Test_ZSTD_CStreamInSize(void) {
    return ZSTD_CStreamInSize();
}

size_t Test_ZSTD_DStreamInSize(void) {
    return ZSTD_DStreamInSize();
}

size_t Test_ZSTD_CStreamOutSize(void) {
    return ZSTD_CStreamOutSize();
}

size_t Test_ZSTD_DStreamOutSize(void) {
    return ZSTD_DStreamOutSize();
}

size_t Test_ZSTD_estimateCStreamSize(int compressionLevel){
    return ZSTD_estimateCStreamSize(compressionLevel);
}

size_t Test_ZSTD_estimateCStreamSize_usingCParams(ZSTD_compressionParameters cParams) {
    return ZSTD_estimateCStreamSize_usingCParams(cParams);
}

size_t Test_ZSTD_estimateCStreamSize_usingCCtxParams(const ZSTD_CCtx_params* cctxParams) {
    return ZSTD_estimateCStreamSize_usingCCtxParams(cctxParams);
}

size_t Test_ZSTD_estimateDStreamSize(size_t windowSize) {
    return ZSTD_estimateDStreamSize(windowSize);
}

size_t Test_ZSTD_initCStream(ZSTD_CStream* zcs, int compressionLevel) {
    return ZSTD_initCStream(zcs, compressionLevel);
}

ZSTD_CStream* Test_ZSTD_initStaticCStream(void* workspace, size_t workspaceSize) {
    return ZSTD_initStaticCStream(workspace, workspaceSize);
}

ZSTD_DStream* Test_ZSTD_initStaticDStream(void* workspace, size_t workspaceSize) {
    return ZSTD_initStaticDStream(workspace, workspaceSize);
}

size_t Test_ZSTD_sizeof_CStream(const ZSTD_CStream* zcs) {
    return ZSTD_sizeof_CStream(zcs);
}

size_t Test_ZSTD_sizeof_DStream(const ZSTD_DStream* zds) {
    return ZSTD_sizeof_DStream(zds);
}

size_t Test_ZSTD_decompressBegin(ZSTD_DCtx* dctx) {
    return ZSTD_decompressBegin(dctx);
}

size_t Test_ZSTD_decompressContinue(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize) {
    return ZSTD_decompressContinue(dctx, dst, dstCapacity, src, srcSize);
}

size_t Test_ZSTD_nextSrcSizeToDecompress(ZSTD_DCtx* dctx) {
    return ZSTD_nextSrcSizeToDecompress(dctx);
}

size_t Test_ZSTD_CCtx_setPledgedSrcSize(ZSTD_CCtx* cctx, unsigned long long pledgedSrcSize) {
    return ZSTD_CCtx_setPledgedSrcSize(cctx, pledgedSrcSize);
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
        if (g_dstream == NULL) g_dstream = Test_ZSTD_createDStream();
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
        Test_ZSTD_freeDStream(g_dstream); g_dstream = NULL;
    }

protected:
    ZSTD_compressionParameters cparams;
    ZSTD_CStream* g_cstream = NULL;
    ZSTD_DStream* g_dstream = NULL;
};

/***********************************************
 * Begin of ZSTD_ZSTD_createCStream
 ***********************************************/
TEST(ZSTD_createCStream, AOCL_Compression_zstd_ZSTD_createCStream_pass_common_1) //createCStream
{
    ZSTD_CStream* cstream = Test_ZSTD_createCStream();
    EXPECT_NE(cstream, nullptr);
    Test_ZSTD_freeCStream(cstream);
}

TEST(ZSTD_createCStream, AOCL_Compression_zstd_ZSTD_createCStream_pass_common_2) //ZSTD_CStream typecasted to ZSTD_CCtx
{
    ZSTD_CStream* cstream = Test_ZSTD_createCStream();
    size_t ret = Test_ZSTD_freeCCtx((ZSTD_CCtx*)cstream); //CCtx and CStream are effectively same object (>= v1.3.0)
    EXPECT_EQ(ret, 0);
}
/***********************************************
 * End of ZSTD_ZSTD_createCStream
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_createDStream
 * ***************************************/
TEST(ZSTD_createDStream, AOCL_Compression_zstd_ZSTD_createDStream_pass_common_1) //createDStream
{
    ZSTD_DStream* dstream = Test_ZSTD_createDStream();
    EXPECT_NE(dstream, nullptr);
    Test_ZSTD_freeDStream(dstream);
}

TEST(ZSTD_createDStream, AOCL_Compression_zstd_ZSTD_createDStream_pass_common_2) //ZSTD_DStream typecasted to ZSTD_DCtx
{
    ZSTD_DStream* dstream = Test_ZSTD_createDStream();
    size_t ret = Test_ZSTD_freeDCtx((ZSTD_DCtx*)dstream); //DCtx and DStream are effectively same object (>= v1.3.0)
    EXPECT_EQ(ret, 0);
}
/***********************************************
 * End of ZSTD_ZSTD_createDStream
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_freeCStream
 ***********************************************/
TEST(ZSTD_freeCStream, AOCL_Compression_zstd_ZSTD_freeCStream_pass_common_1) //freeCStream
{
    ZSTD_CStream* cstream = Test_ZSTD_createCStream();
    size_t ret = Test_ZSTD_freeCStream(cstream);
    EXPECT_EQ(ret, 0);
}

TEST(ZSTD_freeCStream, AOCL_Compression_zstd_ZSTD_freeCStream_pass_common_2) //freeCStream NULL
{
    size_t ret = Test_ZSTD_freeCStream(NULL);
    EXPECT_EQ(ret, 0); // no error. does not do anything.
}

TEST(ZSTD_freeCStream, AOCL_Compression_zstd_ZSTD_freeCStream_pass_common_3) //ZSTD_CCtx typecasted to ZSTD_CStream
{
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx();
    size_t ret = Test_ZSTD_freeCStream((ZSTD_CStream*)cctx); //CCtx and CStream are effectively same object (>= v1.3.0)
    EXPECT_EQ(ret, 0);
}
/***********************************************
 * End of ZSTD_ZSTD_freeCStream
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_freeDStream
 * ***************************************/
TEST(ZSTD_freeDStream, AOCL_Compression_zstd_ZSTD_freeDStream_pass_common_1) //freeDStream
{
    ZSTD_DStream* dstream = Test_ZSTD_createDStream();
    size_t ret = Test_ZSTD_freeDStream(dstream);
    EXPECT_EQ(ret, 0);
}

TEST(ZSTD_freeDStream, AOCL_Compression_zstd_ZSTD_freeDStream_pass_common_2) //freeDStream NULL
{
    size_t ret = Test_ZSTD_freeDStream(NULL);
    EXPECT_EQ(ret, 0); // no error. does not do anything.
}

TEST(ZSTD_freeDStream, AOCL_Compression_zstd_ZSTD_freeDStream_pass_common_3) //ZSTD_DCtx typecasted to ZSTD_DStream
{
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx();
    size_t ret = Test_ZSTD_freeDStream((ZSTD_DStream*)dctx); //DCtx and DStream are effectively same object (>= v1.3.0)
    EXPECT_EQ(ret, 0);
}
/***********************************************
 * End of ZSTD_ZSTD_freeDStream
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_estimateCStreamSize
 ***********************************************/
class ZSTD_ZSTD_estimateCStreamSize : public ZSTD_ZSTD_estimateSize {};

TEST_F(ZSTD_ZSTD_estimateCStreamSize, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_pass_common_1) { // monotonically increasing values based on level for 1 to max
    monotonic_increasing(Test_ZSTD_estimateCStreamSize);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_pass_common_2) { // level <= 0
    level_below_0(Test_ZSTD_estimateCStreamSize);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_pass_common_3) { // level > max
    level_above_max(Test_ZSTD_estimateCStreamSize);
}
/***********************************************
 * End of ZSTD_ZSTD_estimateCStreamSize
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_estimateCStreamSize_usingCParams
 ***********************************************/
class ZSTD_ZSTD_estimateCStreamSize_usingCParams : public ZSTD_ZSTD_estimateSize_usingCParams {};

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCParams_pass_common_1) { // no source estimate
    size_t size_no_params = Test_ZSTD_estimateCStreamSize(ZSTD_CLEVEL_DEFAULT);
    no_source_estimate(size_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCStreamSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCParams_pass_common_2) { // with source estimate
    size_t size_no_params = Test_ZSTD_estimateCStreamSize(ZSTD_CLEVEL_DEFAULT);
    with_source_estimate(size_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCStreamSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCParams_pass_common_3) { // level <= 0
    level_below_0(Test_ZSTD_estimateCStreamSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCParams_pass_common_4) { // level = 8, row based matchfinder
    size_t size_no_params = Test_ZSTD_estimateCStreamSize(8);
    with_source_estimate(size_no_params, 8, Test_ZSTD_estimateCStreamSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCParams_pass_common_5) { // level > max
    level_above_max(Test_ZSTD_estimateCStreamSize_usingCParams);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateCStreamSize_usingCParams
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams
 * *********************************************/
class ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams : public ZSTD_ZSTD_estimateSize_usingCCtxParams {};

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCCtxParams_pass_common_1) { // no source estimate
    size_t size_no_params = Test_ZSTD_estimateCStreamSize(ZSTD_CLEVEL_DEFAULT);
    no_source_estimate(size_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCStreamSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCCtxParams_pass_common_2) { // with source estimate
    size_t size_no_params = Test_ZSTD_estimateCStreamSize(ZSTD_CLEVEL_DEFAULT);
    with_source_estimate(size_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCStreamSize_usingCCtxParams);
}


TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCCtxParams_pass_common_3) { // with source estimate stable mode
    size_t size_no_params = Test_ZSTD_estimateCStreamSize(ZSTD_CLEVEL_DEFAULT);
    with_source_estimate_stable(size_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCStreamSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCCtxParams_pass_common_4) { // level <= 0
    level_below_0(Test_ZSTD_estimateCStreamSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCCtxParams_pass_common_5) { // level = 8, row based matchfinder
    size_t size_no_params = Test_ZSTD_estimateCStreamSize(8);
    with_source_estimate(size_no_params, 8, Test_ZSTD_estimateCStreamSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCStreamSize_usingCCtxParams_pass_common_6) { // level > max
    level_above_max(Test_ZSTD_estimateCStreamSize_usingCCtxParams);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateCStreamSize_usingCCtxParams
 * *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_estimateDStreamSize
 * *********************************************/
TEST(ZSTD_ZSTD_estimateDStreamSize, AOCL_Compression_zstd_ZSTD_estimateDStreamSize_pass_common_1) { //windowSize < ZSTD_BLOCKSIZE_MAX
    size_t sz_stream = Test_ZSTD_estimateDStreamSize(ZSTD_BLOCKSIZE_MAX - 1);
    size_t sz = Test_ZSTD_estimateDCtxSize();
    EXPECT_GT(sz_stream, sz); //stream size should be > ctx size to account for stream buffer sizes
}

TEST(ZSTD_ZSTD_estimateDStreamSize, AOCL_Compression_zstd_ZSTD_estimateDStreamSize_pass_common_2) { //windowSize == ZSTD_BLOCKSIZE_MAX
    size_t sz_stream = Test_ZSTD_estimateDStreamSize(ZSTD_BLOCKSIZE_MAX);
    size_t sz = Test_ZSTD_estimateDCtxSize();
    EXPECT_GT(sz_stream, sz); //stream size should be > ctx size to account for stream buffer sizes
}

TEST(ZSTD_ZSTD_estimateDStreamSize, AOCL_Compression_zstd_ZSTD_estimateDStreamSize_pass_common_3) { //windowSize > ZSTD_BLOCKSIZE_MAX
    size_t sz_stream = Test_ZSTD_estimateDStreamSize(ZSTD_BLOCKSIZE_MAX + 1);
    size_t sz = Test_ZSTD_estimateDCtxSize();
    EXPECT_GT(sz_stream, sz); //stream size should be > ctx size to account for stream buffer sizes
}
/*********************************************
 * End of ZSTD_ZSTD_estimateDStreamSize
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_initStaticCStream
 * *********************************************/
class ZSTD_ZSTD_initStaticCStream : public ZSTD_ZSTD_initStatic {};

TEST_F(ZSTD_ZSTD_initStaticCStream, AOCL_Compression_zstd_Test_ZSTD_initStaticCStream_pass_common_1) { 
    workspace_sufficient(ZSTD_Compress_API::compress_cctx);
}

TEST_F(ZSTD_ZSTD_initStaticCStream, AOCL_Compression_zstd_Test_ZSTD_initStaticCStream_fail_common_2) {
    workspace_null(ZSTD_Compress_API::compress_cctx);
}

TEST_F(ZSTD_ZSTD_initStaticCStream, AOCL_Compression_zstd_Test_ZSTD_initStaticCStream_fail_common_3) {
    workspace_too_small(ZSTD_Compress_API::compress_cctx);
}

TEST_F(ZSTD_ZSTD_initStaticCStream, AOCL_Compression_zstd_Test_ZSTD_initStaticCStream_fail_common_4) {
    workspace_not_aligned(ZSTD_Compress_API::compress_cctx);
}
/*********************************************
 * End of ZSTD_ZSTD_initStaticCStream
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_sizeof_CStream
 * *********************************************/
TEST(ZSTD_ZSTD_sizeof_CStream, AOCL_Compression_zstd_ZSTD_sizeof_CStream_pass_common_1) { // cstream created using ZSTD_createCStream
    ZSTD_CStream* cstream = Test_ZSTD_createCStream();
    size_t size = Test_ZSTD_sizeof_CStream(cstream);
    EXPECT_GT(size, 0);
    Test_ZSTD_freeCStream(cstream);
}

TEST(ZSTD_ZSTD_sizeof_CStream, AOCL_Compression_zstd_ZSTD_sizeof_CStream_pass_common_2) { // cstream created using ZSTD_initStaticCStream
    size_t const workspaceSize = Test_ZSTD_estimateCStreamSize(ZSTD_CLEVEL_DEFAULT);
    void * workspace = malloc(workspaceSize);
    ZSTD_CStream* cstream = Test_ZSTD_initStaticCStream(workspace, workspaceSize);
    size_t size = Test_ZSTD_sizeof_CStream(cstream);
    EXPECT_GT(size, 0);
    free(workspace);
}

TEST(ZSTD_ZSTD_sizeof_CStream, AOCL_Compression_zstd_ZSTD_sizeof_CStream_pass_common_3) { // cstream empty
    ZSTD_CStream cstream;
    memset(&cstream, 0, sizeof(cstream));
    size_t size = Test_ZSTD_sizeof_CStream(&cstream);
    EXPECT_GT(size, 0);
} 

TEST(ZSTD_ZSTD_sizeof_CStream, AOCL_Compression_zstd_ZSTD_sizeof_CStream_fail_common_4) { // cstream null
    size_t size = Test_ZSTD_sizeof_CStream(NULL);
    EXPECT_EQ(size, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_sizeof_CStream
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_sizeof_DStream
 * *********************************************/
TEST(ZSTD_ZSTD_sizeof_DStream, AOCL_Compression_zstd_ZSTD_sizeof_DStream_pass_common_1) { // dstream created using ZSTD_createDStream
    ZSTD_DStream* dstream = Test_ZSTD_createDStream();
    size_t size = Test_ZSTD_sizeof_DStream(dstream);
    EXPECT_GT(size, 0);
    Test_ZSTD_freeDStream(dstream);
}

TEST(ZSTD_ZSTD_sizeof_DStream, AOCL_Compression_zstd_ZSTD_sizeof_DStream_pass_common_2) { // dstream created using ZSTD_initStaticDStream
    size_t const workspaceSize = Test_ZSTD_estimateDStreamSize(ZSTD_BLOCKSIZE_MAX);
    void * workspace = malloc(workspaceSize);
    ZSTD_DStream* dstream = Test_ZSTD_initStaticDStream(workspace, workspaceSize);
    size_t size = Test_ZSTD_sizeof_DStream(dstream);
    EXPECT_GT(size, 0);
    free(workspace);
}

TEST(ZSTD_ZSTD_sizeof_DStream, AOCL_Compression_zstd_ZSTD_sizeof_DStream_pass_common_3) { // dstream empty
    ZSTD_DStream dstream;
    memset(&dstream, 0, sizeof(dstream));
    size_t size = Test_ZSTD_sizeof_DStream(&dstream);
    EXPECT_GT(size, 0);
}

TEST(ZSTD_ZSTD_sizeof_DStream, AOCL_Compression_zstd_ZSTD_sizeof_DStream_fail_common_4) { // dstream null
    size_t size = Test_ZSTD_sizeof_DStream(NULL);
    EXPECT_EQ(size, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_sizeof_DStream
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_initCStream
 *********************************************/
class ZSTD_ZSTD_initCStream : public AOCL_setup_zstd {
public:

    void SetUp() override {
        zcs = Test_ZSTD_createCStream();
        EXPECT_NE(zcs, nullptr);
    }

    ~ZSTD_ZSTD_initCStream() {
        Test_ZSTD_freeCStream(zcs);
    }

    size_t compress_continue(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        CHECK_PASS_ZSTD(Test_ZSTD_compressStream2(zcs, &buffOut, &buffIn, ZSTD_e_continue));
        return buffOut.pos;
    }

    void setup_session(TestLoad_2* d)
    {
        int value;
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParameter(zcs, ZSTD_c_compressionLevel, 9)); //set param to non-default value
        compress_continue(d->getCompressedBuff(), d->getCompressedSize(), d->getOrigData(), d->getOrigSize()); // start compression
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(zcs, ZSTD_c_compressionLevel, &value));
        EXPECT_EQ(value, 9);
    }

    void validate_reset(int compressionLevel) {
        EXPECT_EQ(zcs->streamStage, zcss_init); // session reset
        EXPECT_EQ(zcs->cdict, nullptr); // dict reset
        int value;
        CHECK_PASS_ZSTD(ZSTD_CCtx_getParameter(zcs, ZSTD_c_compressionLevel, &value));
        EXPECT_EQ(value, compressionLevel); // level compressionLevel
    }
    ZSTD_CStream* zcs;

private:
    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
};

TEST_F(ZSTD_ZSTD_initCStream, AOCL_Compression_zstd_ZSTD_initCStream_pass_common_1) { // reset active session
    TestLoad_2 d(800);
    setup_session(&d);
    CHECK_PASS_ZSTD(Test_ZSTD_initCStream(zcs, 1)); // reset session with level 1
    validate_reset(1);
}

TEST_F(ZSTD_ZSTD_initCStream, AOCL_Compression_zstd_ZSTD_initCStream_pass_common_2) { // no active session
    CHECK_PASS_ZSTD(Test_ZSTD_initCStream(zcs, 2)); // set session with level 2
    validate_reset(2);
}

TEST_F(ZSTD_ZSTD_initCStream, AOCL_Compression_zstd_ZSTD_initCStream_fail_common_2) { // zcs null
    size_t ret = Test_ZSTD_initCStream(NULL, 1);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_initCStream, AOCL_Compression_zstd_ZSTD_initCStream_pass_common_3) { // level < 0
    int minClevel = ZSTD_minCLevel();
    CHECK_PASS_ZSTD(Test_ZSTD_initCStream(zcs, minClevel));
    validate_reset(minClevel);
}

TEST_F(ZSTD_ZSTD_initCStream, AOCL_Compression_zstd_ZSTD_initCStream_pass_common_4) { // level = 0
    CHECK_PASS_ZSTD(Test_ZSTD_initCStream(zcs, 0));
    validate_reset(ZSTD_CLEVEL_DEFAULT); // set to default
}

TEST_F(ZSTD_ZSTD_initCStream, AOCL_Compression_zstd_ZSTD_initCStream_pass_common_5) { // level > max
    CHECK_PASS_ZSTD(Test_ZSTD_initCStream(zcs, ZSTD_maxCLevel() + 1));
    validate_reset(ZSTD_maxCLevel()); // clamped to maxClevel
}
/*********************************************
 * End of ZSTD_ZSTD_initCStream
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_initDStream
 * *********************************************/
class ZSTD_ZSTD_initDStream : public AOCL_setup_zstd {
public:

    void SetUp() override {
        zcs = Test_ZSTD_createCStream();
        zds = Test_ZSTD_createDStream();
        EXPECT_NE(zcs, nullptr);
        EXPECT_NE(zds, nullptr);
    }

    ~ZSTD_ZSTD_initDStream() {
        Test_ZSTD_freeCStream(zcs);
        Test_ZSTD_freeDStream(zds);
    }

    size_t compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        size_t cSize = Test_ZSTD_compressStream(zcs, &buffOut, &buffIn);
        CHECK_PASS_ZSTD(cSize);
        cSize = Test_ZSTD_flushStream(zcs, &buffOut);
        CHECK_PASS_ZSTD(cSize);
        return buffOut.pos;
    }

    size_t decompress_continue(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        size_t ret = Test_ZSTD_decompressStream(zds, &buffOut, &buffIn);
        return buffOut.pos;
    }

    void setup_session(TestLoad_2* d)
    {
        int value;
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParameter(zcs, ZSTD_c_windowLog, windowLog)); //set param to non-default value
        CHECK_PASS_ZSTD(Test_ZSTD_DCtx_setParameter(zds, ZSTD_d_windowLogMax, windowLog)); //set param to non-default value
        size_t compressedSize = compress(d->getCompressedBuff(), d->getCompressedSize(), d->getOrigData(), d->getOrigSize()); // compress
        const size_t srcSz = compressedSize / 2;
        const size_t dstSz = d->getOrigSize() / 2;
        decompress_continue(d->getOrigData(), dstSz, d->getCompressedBuff(), srcSz); // start decompression
        CHECK_PASS_ZSTD(Test_ZSTD_DCtx_getParameter(zds, ZSTD_d_windowLogMax, &value));
        EXPECT_EQ(value, windowLog);
    }

    void validate_reset(int windowLogMax) {
        EXPECT_EQ(zds->streamStage, zdss_init); // session reset
        EXPECT_EQ(zds->ddict, nullptr); // dict reset
        int value;
        CHECK_PASS_ZSTD(ZSTD_DCtx_getParameter(zds, ZSTD_d_windowLogMax, &value));
        EXPECT_EQ(value, windowLogMax); // level windowLogMax
    }

protected:
    const int windowLog = 18;
    ZSTD_CStream* zcs;
    ZSTD_DStream* zds;

private:
    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
};

TEST_F(ZSTD_ZSTD_initDStream, AOCL_Compression_zstd_ZSTD_initDStream_pass_common_1) { // reset active session ZSTD_f_zstd1
    TestLoad_2 d(800);
    setup_session(&d);
    CHECK_PASS_ZSTD(Test_ZSTD_initDStream(zds)); // reset session
    validate_reset(windowLog);
}

TEST_F(ZSTD_ZSTD_initDStream, AOCL_Compression_zstd_ZSTD_initDStream_pass_common_2) { // reset active session ZSTD_f_zstd1_magicless
    TestLoad_2 d(800);
    Test_ZSTD_CCtx_setParameter(zcs, ZSTD_c_format, ZSTD_f_zstd1_magicless);
    Test_ZSTD_DCtx_setParameter(zds, ZSTD_d_format, ZSTD_f_zstd1_magicless);
    setup_session(&d);
    CHECK_PASS_ZSTD(Test_ZSTD_initDStream(zds)); // reset session
    validate_reset(windowLog);
}

TEST_F(ZSTD_ZSTD_initDStream, AOCL_Compression_zstd_ZSTD_initDStream_pass_common_3) { // no active session
    CHECK_PASS_ZSTD(Test_ZSTD_initDStream(zds)); // set session
    validate_reset(ZSTD_WINDOWLOG_LIMIT_DEFAULT);
}

TEST_F(ZSTD_ZSTD_initDStream, AOCL_Compression_zstd_ZSTD_initDStream_fail_common_4) { // zds null
    size_t ret = Test_ZSTD_initDStream(NULL);
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_initDStream
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_compressStream
 * *********************************************/
class ZSTD_compress_stream_base : public ZSTD_stream_base {
public:
    ZSTD_compress_stream_base() : ZSTD_stream_base() {}

    void compress_one_shot(ZSTD_Compress_API api_continue, ZSTD_Compress_API api_end) //single call fully consumed
    {
        TestLoad_2 d(1024);
        size_t ret = run_compress(api_continue, true, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret);
        ret = run_compress(api_end, false , NULL, NULL, NULL, NULL);
        CHECK_PASS_ZSTD(ret);
        EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
        EXPECT_EQ(buffIn.pos, buffIn.size); //fully consumed
    }

    void compress_pass(ZSTD_Compress_API api, bool reset_buffers) //single call
    {
        TestLoad_2 d(1024);
        size_t ret = run_compress(api, reset_buffers, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret);
    }

    void compress_src_null(ZSTD_Compress_API api) //src NULL
    {
        TestLoad_2 d(1024);
        size_t ret = run_compress(api, true, d.getCompressedBuff(), d.getCompressedSize(), NULL, d.getOrigSize());
        if (api == ZSTD_Compress_API::compress_stream_continue || api == ZSTD_Compress_API::compress_stream2_continue) { //in continue mode
            CHECK_PASS_ZSTD(ret); //pass. expect future calls to process data
        }
        else { //in end mode
            EXPECT_EQ(ret, ERROR(srcSize_wrong));
        }
        EXPECT_EQ(buffOut.pos, 0); // no data written
    }

    void compress_dst_null(ZSTD_Compress_API api) //dst NULL
    {
        TestLoad_2 d(1024);
        size_t ret = run_compress(api, true, NULL, d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret); //pass. expect future calls to process data
        EXPECT_EQ(buffOut.pos, 0); // no data written
    }

    void compress_dst_zero(ZSTD_Compress_API api) //dst size 0
    {
        TestLoad_2 d(1024);
        size_t ret = run_compress(api, true, d.getCompressedBuff(), 0, d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret); //pass. expect future calls to process data
        EXPECT_EQ(buffOut.pos, 0);; // no data written
    }

    void compress_srcsize_0(ZSTD_Compress_API api) //src size 0
    {
        TestLoad_2 d(1024);
        size_t ret = run_compress(api, true, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), 0);
        CHECK_PASS_ZSTD(ret); //pass
        if (api == ZSTD_Compress_API::compress_stream_continue || api == ZSTD_Compress_API::compress_stream2_continue) { //in continue mode
            EXPECT_EQ(buffOut.pos, 0); // no data written. expect future calls to pass data
        }
        else { //in end mode
            EXPECT_GT(buffOut.pos, 0); //writes valid frame with headers and no data
        }
    }

    void compress_src_null_srcsize_0(ZSTD_Compress_API api)  //src NULL and src size 0
    {
        TestLoad_2 d(1024);
        size_t ret = run_compress(api, true, d.getCompressedBuff(), d.getCompressedSize(), NULL, 0);
        CHECK_PASS_ZSTD(ret); //pass
        if (api == ZSTD_Compress_API::compress_stream_continue || api == ZSTD_Compress_API::compress_stream2_continue) { //in continue mode
            EXPECT_EQ(buffOut.pos, 0); // no data written. expect future calls to pass data
        }
        else { //in end mode
            EXPECT_GT(buffOut.pos, 0); //writes valid frame with headers and no data
        }
    }

    void compress_invalid_inp_pos(ZSTD_Compress_API api) //invalid input pos
    {
        TestLoad_2 d(1024);
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = d.getCompressedSize();
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = d.getOrigSize();
        buffIn.pos = d.getOrigSize() + 1; // > buffIn.size
        size_t ret = run_compress(api, false, NULL, NULL, NULL, NULL);
        EXPECT_EQ(ret, ERROR(srcSize_wrong));
    }
    
    void compress_invalid_out_pos(ZSTD_Compress_API api) //invalid output pos
    {
        TestLoad_2 d(1024);
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = d.getCompressedSize();
        buffOut.pos = d.getCompressedSize() + 1; // > buffOut.size
        buffIn.src = d.getOrigData();
        buffIn.size = d.getOrigSize();
        buffIn.pos = 0;
        size_t ret = run_compress(api, false, NULL, NULL, NULL, NULL);
        EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
    }

    void compress_complete(ZSTD_Compress_API api_continue, ZSTD_Compress_API api_end) // test a case where frame is fully compressed and stream end is called
    {
        TestLoad_2 d(1024);

        //compress fully
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = d.getCompressedSize();
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = d.getOrigSize();
        buffIn.pos = 0;
        size_t ret = run_compress(api_continue, false, NULL, NULL, NULL, NULL);
        CHECK_PASS_ZSTD(ret);

        //call stream end
        size_t toFlush = run_compress(api_end, false, NULL, NULL, NULL, NULL);
        EXPECT_EQ(toFlush, 0); // expect this to be 0 as frame is fully consumed (buffOut sufficient) and checksumFlag is false
    }

    void compress_incomplete(ZSTD_Compress_API api_continue, ZSTD_Compress_API api_end, ZSTD_Compress_API api_flush) // test a case where frame is partially compressed and stream end is called followed by flush
    {
        TestLoad_2 d(1024);

        //compress and flush partially to a buffer with size less than expected compressed stream size
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = d.getCompressedSize() / 10;
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = d.getOrigSize();
        buffIn.pos = 0;
        size_t ret = run_compress(api_continue, false, NULL, NULL, NULL, NULL);
        CHECK_PASS_ZSTD(ret);

        //call stream end
        size_t toFlush = run_compress(api_end, false, NULL, NULL, NULL, NULL);
        CHECK_PASS_ZSTD(toFlush);
        EXPECT_GT(toFlush, 0); // expect this to be > 0 as bytes are not yet fully flushed (buffOut insufficient)

        //call flush by setting output buffer to 'toFlush'
        buffOut.size += toFlush;
        toFlush = run_compress(api_flush, false, NULL, NULL, NULL, NULL);
        EXPECT_EQ(toFlush, 0); // fully consumed
    }

protected:
    size_t run_compress(ZSTD_Compress_API api, bool reset_buffers, void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        if (reset_buffers) {
            buffOut.dst = dst;
            buffOut.size = dstCapacity;
            buffOut.pos = 0;
            buffIn.src = src;
            buffIn.size = srcSize;
            buffIn.pos = 0;
        }

        /* Streaming APIs expect ctx, output and input buffers to be valid non-NULL objects. Hence, not testing for these. */
        switch (api) {
        case ZSTD_Compress_API::compress_stream2_continue:
            return Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_continue);
        case ZSTD_Compress_API::compress_stream2_end:
            return Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        case ZSTD_Compress_API::compress_stream2_flush:
            return Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_flush);
        case ZSTD_Compress_API::compress_stream_continue:
            return Test_ZSTD_compressStream(g_cstream, &buffOut, &buffIn);
        case ZSTD_Compress_API::compress_stream_end:
            return Test_ZSTD_endStream(g_cstream, &buffOut);
        case ZSTD_Compress_API::compress_stream_flush:
            return Test_ZSTD_flushStream(g_cstream, &buffOut);
        default:
            return ERROR(GENERIC);
        }
    }

    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
};

class ZSTD_ZSTD_compressStream : public ZSTD_compress_stream_base {
public:
    ZSTD_ZSTD_compressStream() : ZSTD_compress_stream_base()
    {
        setup(NULL);

        ZSTD_parameters p;
        ZSTD_frameParameters f = { 1 /* contentSizeHeader*/, 0, 0 };
        p.fParams = f;
        p.cParams = cparams;

        Test_ZSTD_initCStream_advanced(g_cstream, NULL, 0, p, ZSTD_CONTENTSIZE_UNKNOWN);
    }
};

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_1)
{
    compress_one_shot(ZSTD_Compress_API::compress_stream_continue, ZSTD_Compress_API::compress_stream_end);
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress_stream_continue);
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress_stream_continue);
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_4)
{
    compress_dst_zero(ZSTD_Compress_API::compress_stream_continue);
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_stream_continue);
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_stream_continue);
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_fail_common_7)
{
    compress_invalid_inp_pos(ZSTD_Compress_API::compress_stream_continue);
}

TEST_F(ZSTD_ZSTD_compressStream, AOCL_Compression_zstd_ZSTD_compressStream_fail_common_8)
{
    compress_invalid_out_pos(ZSTD_Compress_API::compress_stream_continue);
}
/*********************************************
 * End of ZSTD_ZSTD_compressStream
 *********************************************/

/***********************************************
* Begin of ZSTD_ZSTD_compressStream2
***********************************************/
class ZSTD_ZSTD_compressStream2 : public ZSTD_compress_stream_base {
public:
    ZSTD_ZSTD_compressStream2() : ZSTD_compress_stream_base()
    {
        setup(NULL);
    }

    size_t compress_end(const void* src, size_t srcSize, void* dst, size_t dstCapacity)
    {
        /* Streaming APIs expect ctx, output and input buffers to be valid non-NULL objects. Hence, not testing for these. */
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;

        size_t ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        if (!Test_ZSTD_isError(ret))
            return buffOut.pos;
        else
            return ret;
    }

    size_t compress_continue(const void* src, size_t srcSize, void* dst, size_t dstCapacity)
    {
        /* Streaming APIs expect ctx, output and input buffers to be valid non-NULL objects. Hence, not testing for these. */
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;

        size_t ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_continue);
        if (Test_ZSTD_isError(ret)) 
            return ret;
        
        ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        if (!Test_ZSTD_isError(ret))
            return buffOut.pos;
        else
            return ret;
    }

    size_t compress_iter(TestLoad_2& d, size_t srcStep) { //compress iteratively using ZSTD_e_continue
        size_t ret;
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = d.getCompressedSize();
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = srcStep;
        buffIn.pos = 0;

        size_t prev_pos = buffIn.pos;
        do {
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_continue);
            if (Test_ZSTD_isError(ret))
                break;
            EXPECT_GT(buffIn.pos, prev_pos); //ZSTD_e_continue is guaranteed to make some forward progress
            prev_pos = buffIn.pos;
            buffIn.size += srcStep;
        } while (buffIn.size <= d.getOrigSize());

        if (!Test_ZSTD_isError(ret)) {
            buffIn.size = d.getOrigSize(); //remaining src bytes
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        }
        return ret;
    }

    size_t compress_iter_flush(TestLoad_2& d, size_t srcStep) { //compress iteratively using ZSTD_e_flush
        size_t ret;
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = d.getCompressedSize(); //provide large enough to hold all compressed data. This ensures forward progress on output in each iter.
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = srcStep;
        buffIn.pos = 0;
    
        size_t prev_in_pos = buffIn.pos;
        size_t prev_out_pos = buffOut.pos;
        do {
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_flush);
            if (Test_ZSTD_isError(ret))
                break;
            EXPECT_GT(buffIn.pos, prev_in_pos); //ZSTD_e_flush is guaranteed to make some forward progress
            EXPECT_GT(buffOut.pos, prev_out_pos); //flush whatever data that might remain stuck within internal buffer
            prev_in_pos = buffIn.pos;
            prev_out_pos = buffOut.pos;
            buffIn.size += srcStep;
        } while (buffIn.size <= d.getOrigSize());

        if (!Test_ZSTD_isError(ret)) {
            buffIn.size = d.getOrigSize(); //remaining src bytes
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        }
        return ret;
    }

    size_t compress_iter_flush_2(TestLoad_2& d, size_t dstStep) { //compress iteratively using ZSTD_e_flush. dst buffer insufficient.
        size_t ret;
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = dstStep; //provide small dst buffer. This ensures forward progress on output in each iter.
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = d.getOrigSize();
        buffIn.pos = 0;

        size_t prev_out_pos = buffOut.pos;
        do {
            //flush whatever data that might remain stuck within internal buffer upto the provided dst buffer size
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_flush);
            if (Test_ZSTD_isError(ret))
                break;
            if(buffOut.pos == prev_out_pos)
                break; // If d.getCompressedSize() is larger than compressed data, there is no more data to flush.
            prev_out_pos = buffOut.pos;
            buffOut.size += dstStep;
        } while (buffOut.size <= d.getCompressedSize());

        if (!Test_ZSTD_isError(ret)) {
            buffOut.size = d.getCompressedSize(); //remaining dst bytes
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        }
        return ret;
    }

    void set_num_ref_threads(int numThreads) {
        ZSTD_CCtx_setParameter(g_cstream, ZSTD_c_nbWorkers, numThreads);
    }
};

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_1)
{
    compress_one_shot(ZSTD_Compress_API::compress_stream2_continue, ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress_stream2_continue);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_fail_common_3)
{
    compress_src_null(ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_4)
{
    compress_dst_null(ZSTD_Compress_API::compress_stream2_continue);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_5)
{
    compress_dst_null(ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_6)
{
    compress_dst_zero(ZSTD_Compress_API::compress_stream2_continue);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_7)
{
    compress_dst_zero(ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_8)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_stream2_continue);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_9)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_10)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_stream2_continue);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_11)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_12) //insufficient dst buffer
{
    TestLoad_2 d(1024);
    size_t dstPartial = d.getCompressedSize() / 2;
    d.dstset('x', d.getCompressedBuff() + dstPartial, d.getCompressedBuff() + d.getCompressedSize()); // set 2nd half. These should not get overwritten.
    size_t outLen = compress_end(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), dstPartial);
    CHECK_PASS_ZSTD(outLen);

    //check for no overflow
    EXPECT_LT(outLen, d.getCompressedSize()); 
    char* cpr = d.getCompressedBuff() + dstPartial;
    bool overwritten = false;
    for (size_t i = 0; i < (d.getCompressedSize() - dstPartial); ++i) {
        if (cpr[i] != 'x') {
            overwritten = true;
            break;
        }
    }
    EXPECT_FALSE(overwritten);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_13) //continue & end
{
    TestLoad_2 d(1024);
    size_t outLen = compress_continue(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), d.getCompressedSize());
    CHECK_PASS_ZSTD(outLen);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_14) //compress over multiple calls. small srcStep.
{
    TestLoad_2 d(150 * 1024); //> 128 KB
    size_t srcStep = 4 * 1024; //small srcStep. Test_ZSTD_compressStream2 will just consume data without writing to output until it has sufficient data to compress in one go.
    CHECK_PASS_ZSTD(compress_iter(d, srcStep));
    EXPECT_EQ(buffIn.pos, buffIn.size); // all bytes consumed
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_15) //compress over multiple calls. small srcStep with flush.
{
    TestLoad_2 d(150 * 1024); //> 128 KB
    size_t srcStep = 4 * 1024; //small srcStep. Test_ZSTD_compressStream2 will just consume data without writing to output until it has sufficient data to compress in one go.
    CHECK_PASS_ZSTD(compress_iter_flush(d, srcStep));
    EXPECT_EQ(buffIn.pos, buffIn.size); // all bytes consumed
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_16) //compress over multiple calls. large srcStep (auto flush).
{
    TestLoad_2 d(512 * 1024);
    size_t srcStep = 130 * 1024; //large srcStep, > 128KB
    CHECK_PASS_ZSTD(compress_iter(d, srcStep));
    EXPECT_EQ(buffIn.pos, buffIn.size); // all bytes consumed
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_17) //flush over multiple calls. dst insufficient
{
    TestLoad_2 d(150 * 1024);
    size_t dstStep = 4 * 1024; //small dstStep
    CHECK_PASS_ZSTD(compress_iter_flush_2(d, dstStep));
    EXPECT_EQ(buffIn.pos, buffIn.size); // all bytes consumed
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_fail_common_18) //invalid endOp
{
    TestLoad_2 d(1024);
    buffOut.dst = d.getCompressedBuff();
    buffOut.size = d.getCompressedSize();
    buffOut.pos = 0;
    buffIn.src = d.getOrigData();
    buffIn.size = d.getOrigSize();
    buffIn.pos = 0;
    size_t ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, (ZSTD_EndDirective)3);
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_fail_common_19)
{
    compress_invalid_inp_pos(ZSTD_Compress_API::compress_stream2_continue);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_fail_common_20)
{
    compress_invalid_inp_pos(ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_fail_common_21)
{
    compress_invalid_out_pos(ZSTD_Compress_API::compress_stream2_continue);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_fail_common_22)
{
    compress_invalid_out_pos(ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_23)
{
    compress_complete(ZSTD_Compress_API::compress_stream2_continue, ZSTD_Compress_API::compress_stream2_end);
}

TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_pass_common_24)
{
    compress_incomplete(ZSTD_Compress_API::compress_stream2_continue, ZSTD_Compress_API::compress_stream2_end, ZSTD_Compress_API::compress_stream2_flush);
}

#ifdef AOCL_ENABLE_THREADS
/* Library must be built with ZSTD_MULTITHREAD flag defined for this test to take multithreaded code path in zstd reference */
TEST_F(ZSTD_ZSTD_compressStream2, AOCL_Compression_zstd_ZSTD_compressStream2_common_25) //reference multi-threaded
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
* Begin of ZSTD_ZSTD_endStream
***********************************************/
class ZSTD_ZSTD_endStream : public ZSTD_compress_stream_base {
public:
    ZSTD_ZSTD_endStream() : ZSTD_compress_stream_base()
    {
        setup(NULL);
    }
};

TEST_F(ZSTD_ZSTD_endStream, AOCL_Compression_zstd_ZSTD_endStream_pass_common_1)
{
    compress_pass(ZSTD_Compress_API::compress_stream_end, true);
}

TEST_F(ZSTD_ZSTD_endStream, AOCL_Compression_zstd_ZSTD_endStream_pass_common_2)
{
    compress_dst_null(ZSTD_Compress_API::compress_stream_end);
}

TEST_F(ZSTD_ZSTD_endStream, AOCL_Compression_zstd_ZSTD_endStream_pass_common_3)
{
    compress_dst_zero(ZSTD_Compress_API::compress_stream_end);
}

TEST_F(ZSTD_ZSTD_endStream, AOCL_Compression_zstd_ZSTD_endStream_fail_common_4)
{
    compress_invalid_out_pos(ZSTD_Compress_API::compress_stream_end);
}

TEST_F(ZSTD_ZSTD_endStream, AOCL_Compression_zstd_ZSTD_endStream_pass_common_5)
{
    compress_complete(ZSTD_Compress_API::compress_stream_continue, ZSTD_Compress_API::compress_stream_end);
}

TEST_F(ZSTD_ZSTD_endStream, AOCL_Compression_zstd_ZSTD_endStream_pass_common_6)
{
    compress_incomplete(ZSTD_Compress_API::compress_stream_continue, ZSTD_Compress_API::compress_stream_end, ZSTD_Compress_API::compress_stream_flush);
}
/*********************************************
 * End of ZSTD_ZSTD_endStream
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_flushStream
 * *********************************************/
class ZSTD_ZSTD_flushStream : public ZSTD_compress_stream_base {
public:
    ZSTD_ZSTD_flushStream() : ZSTD_compress_stream_base()
    {
        setup(NULL);
    }

    size_t compress_iter_flush(TestLoad_2& d, size_t srcStep) { //compress iteratively flushStream each time.
        size_t ret;
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = d.getCompressedSize(); //provide large enough to hold all compressed data. This ensures forward progress on output in each iter.
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = srcStep;
        buffIn.pos = 0;

        size_t prev_in_pos = buffIn.pos;
        size_t prev_out_pos = buffOut.pos;
        do {
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_continue); //compress
            if (Test_ZSTD_isError(ret))
                break;
            ret = Test_ZSTD_flushStream(g_cstream, &buffOut); //flush
            if (Test_ZSTD_isError(ret))
                break;
            EXPECT_GT(buffIn.pos, prev_in_pos); //flush is guaranteed to make some forward progress
            EXPECT_GT(buffOut.pos, prev_out_pos); //flush whatever data that might remain stuck within internal buffer
            prev_in_pos = buffIn.pos;
            prev_out_pos = buffOut.pos;
            buffIn.size += srcStep;
        } while (buffIn.size <= d.getOrigSize());

        if (!Test_ZSTD_isError(ret)) {
            buffIn.size = d.getOrigSize(); //remaining src bytes
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        }
        return ret;
    }

    size_t compress_iter_flush_2(TestLoad_2& d, size_t dstStep) { //compress iteratively flushStream each time. dst buffer insufficient.
        size_t ret;
        buffOut.dst = d.getCompressedBuff();
        buffOut.size = dstStep; //provide small dst buffer. This ensures forward progress on output in each iter.
        buffOut.pos = 0;
        buffIn.src = d.getOrigData();
        buffIn.size = d.getOrigSize();
        buffIn.pos = 0;

        size_t prev_out_pos = buffOut.pos;
        do {
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_continue); //compress
            if (Test_ZSTD_isError(ret))
                break;
            //flush whatever data that might remain stuck within internal buffer upto the provided dst buffer size
            ret = Test_ZSTD_flushStream(g_cstream, &buffOut); //flush
            if (Test_ZSTD_isError(ret))
                break;
            if(buffOut.pos == prev_out_pos)
                break; // If d.getCompressedSize() is larger than compressed data, there is no more data to flush.
            prev_out_pos = buffOut.pos;
            buffOut.size += dstStep;
        } while (buffOut.size <= d.getCompressedSize());

        if (!Test_ZSTD_isError(ret)) {
            buffOut.size = d.getCompressedSize(); //remaining dst bytes
            ret = Test_ZSTD_compressStream2(g_cstream, &buffOut, &buffIn, ZSTD_e_end);
        }
        return ret;
    }
};

TEST_F(ZSTD_ZSTD_flushStream, AOCL_Compression_zstd_ZSTD_flushStream_pass_common_1)
{
    compress_pass(ZSTD_Compress_API::compress_stream_flush, true);
}

TEST_F(ZSTD_ZSTD_flushStream, AOCL_Compression_zstd_ZSTD_flushStream_pass_common_2)
{
    compress_dst_null(ZSTD_Compress_API::compress_stream_flush);
}

TEST_F(ZSTD_ZSTD_flushStream, AOCL_Compression_zstd_ZSTD_flushStream_pass_common_3)
{
    compress_dst_zero(ZSTD_Compress_API::compress_stream_flush);
}

TEST_F(ZSTD_ZSTD_flushStream, AOCL_Compression_zstd_ZSTD_flushStream_fail_common_4)
{
    compress_invalid_out_pos(ZSTD_Compress_API::compress_stream_flush);
}

TEST_F(ZSTD_ZSTD_flushStream, AOCL_Compression_zstd_ZSTD_flushStream_pass_common_5) //compress over multiple calls. small srcStep with flush.
{
    TestLoad_2 d(150 * 1024); //> 128 KB
    size_t srcStep = 4 * 1024; //small srcStep. Test_ZSTD_compressStream2 will just consume data without writing to output until it has sufficient data to compress in one go.
    CHECK_PASS_ZSTD(compress_iter_flush(d, srcStep));
    EXPECT_EQ(buffIn.pos, buffIn.size); // all bytes consumed
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_flushStream, AOCL_Compression_zstd_ZSTD_flushStream_pass_common_6) //flush over multiple calls. dst insufficient.
{
    TestLoad_2 d(150 * 1024);
    size_t dstStep = 4 * 1024; //small dstStep
    CHECK_PASS_ZSTD(compress_iter_flush_2(d, dstStep));
    EXPECT_EQ(buffIn.pos, buffIn.size); // all bytes consumed
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
}
/*********************************************
 * End of ZSTD_ZSTD_flushStream
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_CStreamInSize
 *********************************************/
class ZSTD_ZSTD_CStreamInSize : public ZSTD_compress_stream_base {
public:
    ZSTD_ZSTD_CStreamInSize() : ZSTD_compress_stream_base() {}

    void run_and_validate_compress(TestLoad_2& d){
        size_t ret = run_compress(ZSTD_Compress_API::compress_stream2_continue, true, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret);
        ret = run_compress(ZSTD_Compress_API::compress_stream2_end, false , d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret);
        EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
        EXPECT_EQ(buffIn.pos, buffIn.size); //fully consumed
    }
};

TEST_F(ZSTD_ZSTD_CStreamInSize, AOCL_Compression_zstd_ZSTD_CStreamInSize_pass_common_1) //use recommended size for input
{
    size_t ret = Test_ZSTD_CStreamInSize();
    EXPECT_GT(ret, 0);
    TestLoad_2 d(ret);
    run_and_validate_compress(d);
}

TEST_F(ZSTD_ZSTD_CStreamInSize, AOCL_Compression_zstd_ZSTD_CStreamInSize_pass_common_2) //use > recommended size for input
{
    size_t ret = Test_ZSTD_CStreamInSize();
    EXPECT_GT(ret, 0);
    TestLoad_2 d(ret + 1);
    run_and_validate_compress(d);
}

TEST_F(ZSTD_ZSTD_CStreamInSize, AOCL_Compression_zstd_ZSTD_CStreamInSize_pass_common_3) //use < recommended size for input
{
    size_t ret = Test_ZSTD_CStreamInSize();
    EXPECT_GT(ret, 0);
    TestLoad_2 d(ret - 1);
    run_and_validate_compress(d);
}
/*********************************************
 * End of ZSTD_ZSTD_CStreamInSize
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_CStreamOutSize
 *********************************************/
class ZSTD_ZSTD_CStreamOutSize : public ZSTD_compress_stream_base {
public:
    ZSTD_ZSTD_CStreamOutSize() : ZSTD_compress_stream_base() {}

    void run_and_validate_compress(TestLoad_2& d){
        size_t ret = run_compress(ZSTD_Compress_API::compress_stream2_continue, true, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret);
        ret = run_compress(ZSTD_Compress_API::compress_stream2_end, false , d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        CHECK_PASS_ZSTD(ret);
        EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), buffOut.pos, ZSTD_decompressDCtx));
        EXPECT_EQ(buffIn.pos, buffIn.size); //fully consumed
    }
};

TEST_F(ZSTD_ZSTD_CStreamOutSize, AOCL_Compression_zstd_ZSTD_CStreamOutSize_pass_common_1) //use recommended size for output
{
    size_t inp_size = ZSTD_BLOCKSIZE_MAX; // set small enough for ZSTD_compressBound check to pass
    size_t out_size = Test_ZSTD_CStreamOutSize();
    EXPECT_GT(out_size, 0);
    
    EXPECT_GE(out_size, ZSTD_compressBound(inp_size)); // necessary for compression and validation to succeed
    
    TestLoad_2 d(inp_size, out_size); //recommended size for output
    run_and_validate_compress(d);
}

TEST_F(ZSTD_ZSTD_CStreamOutSize, AOCL_Compression_zstd_ZSTD_CStreamOutSize_pass_common_2) //use > recommended size for output
{
    size_t inp_size = ZSTD_BLOCKSIZE_MAX; // set small enough for ZSTD_compressBound check to pass
    size_t out_size = Test_ZSTD_CStreamOutSize();
    EXPECT_GT(out_size, 0);
    
    EXPECT_GE(out_size, ZSTD_compressBound(inp_size)); // necessary for compression and validation to succeed
    
    TestLoad_2 d(inp_size, out_size + 1); //> recommended size for output
    run_and_validate_compress(d);
}

TEST_F(ZSTD_ZSTD_CStreamOutSize, AOCL_Compression_zstd_ZSTD_CStreamOutSize_pass_common_3) //use < recommended size for output
{
    size_t inp_size = ZSTD_BLOCKSIZE_MAX - 1024; // set small enough for ZSTD_compressBound check to pass
    size_t out_size = Test_ZSTD_CStreamOutSize();
    EXPECT_GT(out_size, 0);
    
    EXPECT_GE(out_size, ZSTD_compressBound(inp_size)); // necessary for compression and validation to succeed
    
    TestLoad_2 d(inp_size, out_size - 1); //< recommended size for output
    run_and_validate_compress(d);
}
/*********************************************
 * End of ZSTD_ZSTD_CStreamOutSize
 *********************************************/

/***********************************************
* Begin of ZSTD_ZSTD_decompressStream
***********************************************/
class ZSTD_ZSTD_decompressStream : public ZSTD_stream_base, public ZSTD_frame_creator {
public:
    ZSTD_ZSTD_decompressStream() : ZSTD_stream_base(), ZSTD_frame_creator() {
        /* decompress should not overwrite dst buffer beyond origLen.
        * Lossless compression should produce the exact bytes as original.
        * Hence, exact same size allocated for output buffer */
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);
    }

    virtual ~ZSTD_ZSTD_decompressStream()
    {
        if (output)
            free(output);
    }

    void init_buffers() {
        create_frame();
        buffOut.dst = output;
        buffOut.size = outLen;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcLen;
        buffIn.pos = 0;
    }

    size_t run_decompress(ZSTD_Decompress_API api, bool reset_buffers, void* dst, size_t dstCapacity,
     const void* src, size_t srcSize, size_t* flushed) {
        if (reset_buffers) {
            buffOut.dst = dst;
            buffOut.size = dstCapacity;
            buffOut.pos = 0;
            buffIn.src = src;
            buffIn.size = srcSize;
            buffIn.pos = 0;
        }

        switch (api) {
        case ZSTD_Decompress_API::decompress_stream:
        {
            /* Streaming APIs expect ctx, output and input buffers to be valid non-NULL objects. Hence, not testing for these. */
            size_t ret = Test_ZSTD_decompressStream(g_dstream, &buffOut, &buffIn);
            *flushed = buffOut.pos;
            return ret;
        }
        default:
            return 0;
        }
    }

    size_t run_decompress_iter_multi_frame(ZSTD_Decompress_API api, bool reset_buffers, void* dst, size_t dstCapacity,
        const void* src, size_t srcSize, size_t* flushed) {
        size_t ret = run_decompress(api, true, dst, dstCapacity, src, srcSize, flushed);
        if (Test_ZSTD_isError(ret)) return ret;
        while (buffIn.pos < buffIn.size) { // as multiple frames are present, exiting on ret == 0 will return on 1st frame. Instead consume all input.
            ret = run_decompress(api, false, dst, dstCapacity, src, srcSize, flushed);
            if (Test_ZSTD_isError(ret)) return ret;
        }
        return ret;
    }

    void validate_decompress(const char* original, size_t origLen, const char* output, size_t outputLen, size_t rem)
    {
        EXPECT_EQ(rem, 0); // all bytes consumed
        EXPECT_EQ(origLen, outputLen);
        EXPECT_EQ(0, memcmp(output, original, origLen));
    }

    void decompress_pass(ZSTD_Decompress_API api) { // pass
        create_frame();
        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, src, srcLen, &flushed);
        validate_decompress(original, origLen, output, flushed, ret);
    }

    void decompress_src_null(ZSTD_Decompress_API api) { // decompress src null
        create_frame();
        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, NULL, srcLen, &flushed);
        EXPECT_EQ(ret, ERROR(srcSize_wrong));
    }

    void decompress_dst_null(ZSTD_Decompress_API api) { // decompress dst null
        create_frame();
        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, NULL, outLen, src, srcLen, &flushed);
        EXPECT_EQ(ret, ERROR(dstBuffer_null));
    }

    void decompress_buffer_inadequate(ZSTD_Decompress_API api) { // decompression buffer inadequate
        create_frame_reference(); /* testing for buffer being inadequate to hold decompressed zstd frame */
        size_t flushed = 0;
        size_t ret = run_decompress(api, true, output, srcLen / 20, src, srcLen, &flushed);
        EXPECT_GT(ret, 0); // not all bytes flushed
        CHECK_PASS_ZSTD(ret);
    }

    void decompress_srcsize_0(ZSTD_Decompress_API api) { // decompress src 0
        create_frame();
        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, src, 0, &flushed);
        CHECK_PASS_ZSTD(ret);
        EXPECT_EQ(flushed, 0);
    }

    void decompress_src_null_srcsize_0(ZSTD_Decompress_API api) { // decompress src 0 and src null
        create_frame();
        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, NULL, 0, &flushed);
        CHECK_PASS_ZSTD(ret);
        EXPECT_EQ(flushed, 0);
    }

    void decompress_corrupt_magic_number(ZSTD_Decompress_API api) { // decompress corrupt frame : magic number
        create_frame();
        size_t decompress_bound = Test_ZSTD_decompressBound(src, srcLen);
        src[0] = 0; // corrupt magic number (present in first 4 bytes)

        size_t flushed = 0;
        size_t ret = run_decompress(api, true, output, decompress_bound, src, srcLen, &flushed);
        EXPECT_EQ(ret, ERROR(prefix_unknown));
    }

    void decompress_corrupt_frame_header(ZSTD_Decompress_API api) { // decompress corrupt frame : Frame_Header
        create_frame_reference();
        size_t frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
        size_t decompress_bound = Test_ZSTD_decompressBound(src, srcLen);

        EXPECT_GT(frameHeaderSize, 4); // magic number + header
        size_t byte_id = 4 /*magic number*/ + 1 /*Frame_Header_Descriptor*/;
        char temp;
        for (; byte_id < frameHeaderSize; byte_id++) { // corrupt Frame_Header fields succeeding Frame_Header_Descriptor 
            temp = src[byte_id];
            src[byte_id] = 0; // if non-0 values are set, behavior might change, some might return a different error code, some might result in decompress passing.
            size_t flushed = 0;
            size_t ret = run_decompress(api, true, output, decompress_bound, src, srcLen, &flushed);
            EXPECT_EQ(ret, ERROR(corruption_detected));
            src[byte_id] = temp;
        }
    }

    void decompress_corrupt_data_block(ZSTD_Decompress_API api) { // decompress corrupt frame : Data_block 
        create_frame_reference();
        size_t frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
        size_t decompress_bound = Test_ZSTD_decompressBound(src, srcLen);
        src[frameHeaderSize + 1] = 'e'; //corrupt data succeeding frame header
        src[frameHeaderSize + 2] = 'r';
        src[frameHeaderSize + 3] = 'r';
        src[frameHeaderSize + 4] = 'o';
        src[frameHeaderSize + 5] = 'r';

        size_t flushed = 0;
        size_t ret = run_decompress(api, true, output, decompress_bound, src, srcLen, &flushed);
        EXPECT_EQ(ret, ERROR(corruption_detected));
    }

    void decompress_garbage_after_frame(ZSTD_Decompress_API api) { // decompress garbage after valid frame
        create_frame_overwrite();
        // Additional bytes after a valid frame might belong to the next frame for which remaining bytes are not yet available
        // They are ignored and decompression is successful
        size_t flushed = 0;
        size_t ret = run_decompress(api, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
    }

    void decompress_N_frames(ZSTD_Decompress_API api) { // decompress N frames
        size_t srcWritten = create_frames_multiple(); // having multiple frames in a stream is valid input. Expect decompress to pass.
        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
        validate_decompress(original, srcWritten, output, flushed, ret);
    }

    void decompress_skippable_frame(ZSTD_Decompress_API api) { // decompress skippable frame
        create_frame_skippable(rand() % 15); // having a skippable frame is valid input. Expect decompress to pass.
        size_t flushed = 0;
        size_t ret = run_decompress(api, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
    }

    void decompress_checksumFlag_set(ZSTD_Decompress_API api) { // decompress checksumFlag set
        ZSTD_frameParameters fparams;
        fparams.checksumFlag = 1;
        fparams.contentSizeFlag = 0;
        fparams.noDictIDFlag = 0;
        create_frame_with_params(fparams);

        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
        validate_decompress(original, origLen, output, flushed, ret);
    }

    void decompress_checksum_wrong(ZSTD_Decompress_API api) { // decompress checksum wrong
        ZSTD_frameParameters fparams;
        fparams.checksumFlag = 1;
        fparams.contentSizeFlag = 0;
        fparams.noDictIDFlag = 0;
        create_frame_with_params(fparams);
        MEM_write32((src + srcLen - 4), 0); // corrupt checksum

        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, src, srcLen, &flushed);
        EXPECT_EQ(ret, ERROR(checksum_wrong));
    }

    void decompress_contentSizeFlag_set(ZSTD_Decompress_API api) { // decompress contentSizeFlag set
        ZSTD_frameParameters fparams;
        fparams.contentSizeFlag = 1;
        fparams.checksumFlag = 0;
        fparams.noDictIDFlag = 0;
        create_frame_with_params(fparams);

        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
        validate_decompress(original, origLen, output, flushed, ret);
    }

    void decompress_noDictIDFlag_set(ZSTD_Decompress_API api) { // decompress noDictIDFlag set
        ZSTD_frameParameters fparams;
        fparams.noDictIDFlag = 1;
        fparams.checksumFlag = 0;
        fparams.contentSizeFlag = 0;
        create_frame_with_params(fparams);

        size_t flushed = 0;
        size_t ret = run_decompress_iter_multi_frame(api, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
        validate_decompress(original, origLen, output, flushed, ret);
    }

    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
    
    // Decompressed data will be stored in the buffer `output`.
    char* output = NULL;
    size_t outLen;
};

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_1) //decompressStream
{
    decompress_pass(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_2) //decompress src null
{
    decompress_src_null(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_3) //decompress dst null
{
    decompress_dst_null(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_4) //decompression buffer inadequate
{
    decompress_buffer_inadequate(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_5) //decompress src 0
{
    decompress_srcsize_0(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_6) //decompress src 0 and src null
{
    decompress_src_null_srcsize_0(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_7) //decompress corrupt magic number
{
    decompress_corrupt_magic_number(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_8) //decompress corrupt frame : Frame_Header
{
    decompress_corrupt_frame_header(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_9) //decompress corrupt frame : Data_block
{
    decompress_corrupt_data_block(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_10) //decompress garbage after valid frame
{
    decompress_garbage_after_frame(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_11) //decompress N frames
{
    decompress_N_frames(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_12) //decompress skippable frame
{
    decompress_skippable_frame(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_13) //decompress checksumFlag set
{
    decompress_checksumFlag_set(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_14) //decompress checksum wrong
{
    decompress_checksum_wrong(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_15) //decompress contentSizeFlag set
{
    decompress_contentSizeFlag_set(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_16) //decompress noDictIDFlag set
{
    decompress_noDictIDFlag_set(ZSTD_Decompress_API::decompress_stream);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_17) //invalid input pos
{
    init_buffers();
    buffIn.pos = srcLen + 1; //invalid input pos

    size_t ret = Test_ZSTD_decompressStream(g_dstream, &buffOut, &buffIn);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_fail_common_18) //invalid output pos
{
    init_buffers();
    buffOut.pos = outLen + 1; //invalid output pos

    size_t ret = Test_ZSTD_decompressStream(g_dstream, &buffOut, &buffIn);
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_19) //decompress bytewise
{
    const size_t stepSz = 1;
    init_buffers();
    buffIn.size  = stepSz;
    buffOut.size = stepSz;
    size_t ret = 0;
    do {
        do {
            ret = Test_ZSTD_decompressStream(g_dstream, &buffOut, &buffIn);
            buffIn.size += stepSz;
            buffOut.size += stepSz;
        } while (ret);
    } while (buffIn.size < srcLen); /* until all input is fed */
    validate_decompress(original, origLen, output, buffOut.pos, ret);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_20) //decompress bytewise, inp grows faster than out
{
    const size_t stepSzIn  = 5;
    const size_t stepSzOut = 2;
    init_buffers();
    buffIn.size = 0;
    buffOut.size = 0;
    size_t ret = 0;
    do {
        do {
            if (buffIn.size < srcLen) buffIn.size += stepSzIn;
            if (buffOut.size < outLen)buffOut.size += stepSzOut;
            ret = Test_ZSTD_decompressStream(g_dstream, &buffOut, &buffIn);
        } while (ret);
    } while (buffIn.size < srcLen); /* until all input is fed */
    validate_decompress(original, origLen, output, buffOut.pos, ret);
}

TEST_F(ZSTD_ZSTD_decompressStream, AOCL_Compression_zstd_ZSTD_decompressStream_pass_common_21) //decompress bytewise, out grows faster than inp
{
    const size_t stepSzIn = 3;
    const size_t stepSzOut = 8;
    init_buffers();
    buffIn.size = 0;
    buffOut.size = 0;
    size_t ret = 0;
    do {
        do {
            if (buffIn.size < srcLen) buffIn.size += stepSzIn;
            if (buffOut.size < outLen)buffOut.size += stepSzOut;
            ret = Test_ZSTD_decompressStream(g_dstream, &buffOut, &buffIn);
        } while (ret);
    } while (buffIn.size < srcLen); /* until all input is fed */
    validate_decompress(original, origLen, output, buffOut.pos, ret);
}
/*********************************************
 * End of ZSTD_ZSTD_decompressStream
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_DStreamInSize
 *********************************************/
class ZSTD_ZSTD_DStreamInSize : public ZSTD_ZSTD_decompressStream {
public:
    ZSTD_ZSTD_DStreamInSize() : ZSTD_ZSTD_decompressStream() {}

    void run_and_decompress(TestLoad_2& d, size_t srcSize)
    {
        create_frame();
        size_t flushed = 0;
        size_t ret = run_decompress(ZSTD_Decompress_API::decompress_stream, true, output, outLen, src, srcSize, &flushed);
        CHECK_PASS_ZSTD(ret);
        //hintInSize might be less than actual compressed size
        //Hence no validation done here as not all data might be decompressed yet
    }
};

TEST_F(ZSTD_ZSTD_DStreamInSize, AOCL_Compression_zstd_ZSTD_DStreamInSize_pass_common_1) //use recommended size for input
{
    size_t hintCInSize = Test_ZSTD_CStreamInSize();
    EXPECT_GT(hintCInSize, 0);
    
    TestLoad_2 d(hintCInSize);
    size_t hintDInSize = Test_ZSTD_DStreamInSize();
    EXPECT_GT(hintDInSize, 0);
    run_and_decompress(d, hintDInSize);
}

TEST_F(ZSTD_ZSTD_DStreamInSize, AOCL_Compression_zstd_ZSTD_DStreamInSize_pass_common_2) //use > recommended size for input
{
    size_t hintCInSize = Test_ZSTD_CStreamInSize();
    EXPECT_GT(hintCInSize, 0);
    
    TestLoad_2 d(hintCInSize);
    size_t hintDInSize = Test_ZSTD_DStreamInSize();
    EXPECT_GT(hintDInSize, 0);
    run_and_decompress(d, hintDInSize + 1);
}

TEST_F(ZSTD_ZSTD_DStreamInSize, AOCL_Compression_zstd_ZSTD_DStreamInSize_pass_common_3) //use < recommended size for input
{
    size_t hintCInSize = Test_ZSTD_CStreamInSize();
    EXPECT_GT(hintCInSize, 0);
    
    TestLoad_2 d(hintCInSize);
    size_t hintDInSize = Test_ZSTD_DStreamInSize();
    EXPECT_GT(hintDInSize, 0);
    run_and_decompress(d, hintDInSize - 1);
}
/*********************************************
 * End of ZSTD_ZSTD_DStreamInSize
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_DStreamOutSize
 * *********************************************/
class ZSTD_ZSTD_DStreamOutSize : public ZSTD_ZSTD_decompressStream {
public:
    ZSTD_ZSTD_DStreamOutSize() : ZSTD_ZSTD_decompressStream() {}

    void run_and_validate_decompress(TestLoad_2& d, size_t dstCapacity) //dstCapacity must be >= hintDOutSize
    {
        create_frame_reference();
        size_t flushed = 0;
        size_t ret = run_decompress(ZSTD_Decompress_API::decompress_stream, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
        //hintOutSize might be less than complete decompressed size. Hence no validation done here.
        //Recommended size for output buffer, guarantees to successfully flush at least one complete block in all circumstances.
        EXPECT_GT(buffOut.pos, 0); //Hence, check for buffOut progress here.
    }

    void run_and_decompress(TestLoad_2& d, size_t dstCapacity)
    {
        create_frame_reference();
        size_t flushed = 0;
        size_t ret = run_decompress(ZSTD_Decompress_API::decompress_stream, true, output, outLen, src, srcLen, &flushed);
        CHECK_PASS_ZSTD(ret);
        //hintOutSize might be less than complete decompressed size. Hence no validation done here.
    }
};

TEST_F(ZSTD_ZSTD_DStreamOutSize, AOCL_Compression_zstd_ZSTD_DStreamOutSize_pass_common_1) //use recommended size for output
{
    size_t hintCInSize = Test_ZSTD_CStreamInSize();
    EXPECT_GT(hintCInSize, 0);
    
    TestLoad_2 d(hintCInSize);
    size_t hintDOutSize = Test_ZSTD_DStreamOutSize();
    EXPECT_GT(hintDOutSize, 0);
    run_and_validate_decompress(d, hintDOutSize);
}

TEST_F(ZSTD_ZSTD_DStreamOutSize, AOCL_Compression_zstd_ZSTD_DStreamOutSize_pass_common_2) //use > recommended size for output
{
    size_t hintCInSize = Test_ZSTD_CStreamInSize();
    EXPECT_GT(hintCInSize, 0);
    
    TestLoad_2 d(hintCInSize);
    size_t hintDOutSize = Test_ZSTD_DStreamOutSize();
    EXPECT_GT(hintDOutSize, 0);
    run_and_validate_decompress(d, hintDOutSize + 1);
}

TEST_F(ZSTD_ZSTD_DStreamOutSize, AOCL_Compression_zstd_ZSTD_DStreamOutSize_pass_common_3) //use < recommended size for output
{
    size_t hintCInSize = Test_ZSTD_CStreamInSize();
    EXPECT_GT(hintCInSize, 0);
    
    TestLoad_2 d(hintCInSize);
    size_t hintDOutSize = Test_ZSTD_DStreamOutSize();
    EXPECT_GT(hintDOutSize, 0);
    run_and_decompress(d, hintDOutSize - 1); // as dstCapacity is < hintDOutSize, successful flush is not guarenteed.
}
/*********************************************
 * End of ZSTD_ZSTD_DStreamOutSize
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_decompressBegin
 *********************************************/
TEST(ZSTD_ZSTD_decompressBegin, AOCL_Compression_zstd_ZSTD_decompressBegin_fail_common_1) // dctx is null
{
    size_t ret = Test_ZSTD_decompressBegin(NULL);
    EXPECT_EQ(ret, ERROR(GENERIC));
}
// other cases tested as part of decompressContinue
/*********************************************
 * End of ZSTD_ZSTD_decompressBegin
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_nextSrcSizeToDecompress
 *********************************************/
TEST(ZSTD_ZSTD_nextSrcSizeToDecompress, AOCL_Compression_zstd_ZSTD_nextSrcSizeToDecompress_fail_common_1) // dctx is null
{
    size_t ret = Test_ZSTD_nextSrcSizeToDecompress(NULL);
    EXPECT_EQ(ret, ERROR(GENERIC));
}
// other cases tested as part of decompressContinue
/*********************************************
 * End of ZSTD_ZSTD_nextSrcSizeToDecompress
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_decompressContinue
 *********************************************/
class ZSTD_ZSTD_decompressContinue : public ZSTD_ZSTD_decompress_base {
public:
    ZSTD_ZSTD_decompressContinue()
    {
        dctx = ZSTD_createDCtx();
    }

    ~ZSTD_ZSTD_decompressContinue()
    {
        if (dctx)
            ZSTD_freeDCtx(dctx);
    }

    void buffer_less_streaming_multi_frame_iter(size_t& curCprLen, size_t& curDprLen) {
        CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin(dctx));
        while (curCprLen < srcLen) {
            size_t const srcSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
            if (srcSize == 0)
                break; // frame fully decoded
            size_t const dprSize = Test_ZSTD_decompressContinue(dctx, output + curDprLen, outLen - curDprLen, src + curCprLen, srcSize);
            CHECK_PASS_ZSTD(dprSize);
            curDprLen += dprSize;
            curCprLen += srcSize;
        }
    }

    size_t buffer_less_streaming_multi_frame_pass() {
        size_t curCprLen = 0;
        size_t curDprLen = 0;
        while (curCprLen < srcLen) {
            CHECK_PASS_ZSTD(Test_ZSTD_DCtx_reset(dctx, ZSTD_reset_session_only)); //ZSTD_reset_session_and_parameters
            buffer_less_streaming_multi_frame_iter(curCprLen, curDprLen);
        }
        EXPECT_EQ(curCprLen, srcLen); // compressed data fully read
        return curDprLen;
    }

    ZSTD_DCtx* dctx = NULL;
};

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_pass_common_1) //zstd frame
{
    create_frame();
    size_t curDprLen = buffer_less_streaming_multi_frame_pass();
    validate_decompress(original, origLen, output, curDprLen);
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_pass_common_2) //skippable frame
{
    create_frame_skippable(rand() % 15);
    buffer_less_streaming_multi_frame_pass();
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_pass_common_3) //multiple frames
{
    size_t srcWritten = create_frames_multiple();
    size_t curDprLen = buffer_less_streaming_multi_frame_pass();
    validate_decompress(original, srcWritten, output, curDprLen);
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_4) //src is null
{
    create_frame();
    CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin(dctx));
    size_t const srcSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
    size_t const dprSize = Test_ZSTD_decompressContinue(dctx, output, outLen, NULL, srcSize);
    EXPECT_EQ(dprSize, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_5) //dst is null
{
    create_frame();
    CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin(dctx));
    size_t const srcSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
    size_t const dprSize = Test_ZSTD_decompressContinue(dctx, NULL, outLen, src, srcSize);
    EXPECT_EQ(dprSize, ERROR(dstBuffer_null));
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_pass_common_6) //dstCapacity is 0
{
    create_frame();
    CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin(dctx));
    size_t const srcSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
    size_t const dprSize = Test_ZSTD_decompressContinue(dctx, output, 0, src, srcSize);
    EXPECT_EQ(dprSize, 0);
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_7) //srcSize is 0
{
    create_frame();
    CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin(dctx));
    size_t const dprSize = Test_ZSTD_decompressContinue(dctx, output, outLen, src, 0);
    EXPECT_EQ(dprSize, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_8) //src is null and srcSize is 0
{
    create_frame();
    CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin(dctx));
    size_t const dprSize = Test_ZSTD_decompressContinue(dctx, output, outLen, NULL, 0);
    EXPECT_EQ(dprSize, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_decompressContinue, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_9) //dctx is null
{
    create_frame();
    size_t const srcSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
    size_t const dprSize = Test_ZSTD_decompressContinue(NULL, output, outLen, src, srcSize);
    EXPECT_EQ(dprSize, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_decompressContinue
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_CCtx_setPledgedSrcSize
 *********************************************/
class ZSTD_ZSTD_CCtx_setPledgedSrcSize : public ZSTD_compress_stream_base {
public:
    ZSTD_ZSTD_CCtx_setPledgedSrcSize()
    {
        d = new TestLoad_2(800);
    }

    ~ZSTD_ZSTD_CCtx_setPledgedSrcSize()
    {
        if (d)
            delete d;
    }
    
    size_t compress_one_pass(size_t inSize) {
        size_t ret;
        buffOut.dst = d->getCompressedBuff();
        buffOut.size = d->getCompressedSize();
        buffOut.pos = 0;
        buffIn.src = d->getOrigData();
        buffIn.size = inSize;
        buffIn.pos = 0;
        ret = ZSTD_compressStream(g_cstream, &buffOut, &buffIn);
        if (ZSTD_isError(ret))
            return ret;
        return ZSTD_endStream(g_cstream, &buffOut);
    }

    TestLoad_2* d;
};

TEST_F(ZSTD_ZSTD_CCtx_setPledgedSrcSize, AOCL_Compression_zstd_ZSTD_CCtx_setPledgedSrcSize_pass_common_1) // inSize == pledgedSrcSize
{
    size_t const pledgedSrcSize = d->getOrigSize();
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setPledgedSrcSize(g_cstream, pledgedSrcSize));
    size_t ret = compress_one_pass(pledgedSrcSize);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(buffIn.pos, buffIn.size); // all input consumed
}

TEST_F(ZSTD_ZSTD_CCtx_setPledgedSrcSize, AOCL_Compression_zstd_ZSTD_CCtx_setPledgedSrcSize_fail_common_2) // inSize < pledgedSrcSize
{
    size_t const pledgedSrcSize = d->getOrigSize();
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setPledgedSrcSize(g_cstream, pledgedSrcSize));
    size_t ret = compress_one_pass(pledgedSrcSize - 1);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_CCtx_setPledgedSrcSize, AOCL_Compression_zstd_ZSTD_CCtx_setPledgedSrcSize_fail_common_3) // inSize > pledgedSrcSize
{
    size_t const pledgedSrcSize = d->getOrigSize();
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setPledgedSrcSize(g_cstream, pledgedSrcSize - 1));
    size_t ret = compress_one_pass(pledgedSrcSize);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_CCtx_setPledgedSrcSize, AOCL_Compression_zstd_ZSTD_CCtx_setPledgedSrcSize_fail_common_4) // cctx is null
{
    size_t const pledgedSrcSize = d->getOrigSize();
    size_t ret = Test_ZSTD_CCtx_setPledgedSrcSize(NULL, pledgedSrcSize);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_CCtx_setPledgedSrcSize, AOCL_Compression_zstd_ZSTD_CCtx_setPledgedSrcSize_fail_common_5) // streamStage != zcss_init
{
    size_t const pledgedSrcSize = d->getOrigSize();
    g_cstream->streamStage = zcss_flush; // != zcss_init
    size_t ret = Test_ZSTD_CCtx_setPledgedSrcSize(g_cstream, pledgedSrcSize);
    EXPECT_EQ(ret, ERROR(stage_wrong));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtx_setPledgedSrcSize
 *********************************************/
