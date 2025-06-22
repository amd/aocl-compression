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

size_t Test_ZSTD_compress2(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize) {
    return ZSTD_compress2(cctx, dst, dstCapacity, src, srcSize);
}

size_t Test_ZSTD_compress_advanced(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, const void* dict, size_t dictSize, ZSTD_parameters params)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    return ZSTD_compress_advanced(cctx, dst, dstCapacity, src, srcSize, dict, dictSize, params);
#pragma GCC diagnostic pop
}

ZSTD_CCtx* Test_ZSTD_createCCtx(void) {
    return ZSTD_createCCtx();
}

ZSTD_DCtx* Test_ZSTD_createDCtx(void) {
    return ZSTD_createDCtx();
}

size_t Test_ZSTD_freeCCtx(ZSTD_CCtx* cctx) {
    return ZSTD_freeCCtx(cctx);
}

size_t Test_ZSTD_freeDCtx(ZSTD_DCtx* dctx) {
    return ZSTD_freeDCtx(dctx);
}

size_t Test_ZSTD_compressCCtx(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, int compressionLevel) {
    return ZSTD_compressCCtx(cctx, dst, dstCapacity, src, srcSize, compressionLevel);
}

ZSTD_CCtx* Test_ZSTD_createCCtx_advanced(ZSTD_customMem customMem) {
    return ZSTD_createCCtx_advanced(customMem);
}

ZSTD_DCtx* Test_ZSTD_createDCtx_advanced(ZSTD_customMem customMem) {
    return ZSTD_createDCtx_advanced(customMem);
}

ZSTD_CCtx_params* Test_ZSTD_createCCtxParams(void) {
    return ZSTD_createCCtxParams();
}

size_t Test_ZSTD_freeCCtxParams(ZSTD_CCtx_params* params) {
    return ZSTD_freeCCtxParams(params);
}

size_t Test_ZSTD_CCtx_setParameter(ZSTD_CCtx* cctx, ZSTD_cParameter param, int value) {
    return ZSTD_CCtx_setParameter(cctx, param, value);
}

size_t Test_ZSTD_DCtx_setParameter(ZSTD_DCtx* dctx, ZSTD_dParameter param, int value) {
    return ZSTD_DCtx_setParameter(dctx, param, value);
}

size_t Test_ZSTD_CCtx_getParameter(const ZSTD_CCtx* cctx, ZSTD_cParameter param, int* value) {
    return ZSTD_CCtx_getParameter(cctx, param, value);
}

size_t Test_ZSTD_DCtx_getParameter(ZSTD_DCtx* dctx, ZSTD_dParameter param, int* value) {
    return ZSTD_DCtx_getParameter(dctx, param, value);
}

size_t Test_ZSTD_CCtx_reset(ZSTD_CCtx* cctx, ZSTD_ResetDirective reset) {
    return ZSTD_CCtx_reset(cctx, reset);
}

size_t Test_ZSTD_DCtx_reset(ZSTD_DCtx* dctx, ZSTD_ResetDirective reset) {
    return ZSTD_DCtx_reset(dctx, reset);
}

size_t Test_ZSTD_CCtx_setCParams(ZSTD_CCtx* cctx, ZSTD_compressionParameters cparams) {
    return ZSTD_CCtx_setCParams(cctx, cparams);
}

size_t Test_ZSTD_CCtx_setFParams(ZSTD_CCtx* cctx, ZSTD_frameParameters fparams) {
    return ZSTD_CCtx_setFParams(cctx, fparams);
}

size_t Test_ZSTD_CCtx_setParametersUsingCCtxParams(ZSTD_CCtx* cctx, const ZSTD_CCtx_params* params) {
    return ZSTD_CCtx_setParametersUsingCCtxParams(cctx, params);
}

size_t Test_ZSTD_CCtx_setParams(ZSTD_CCtx* cctx, ZSTD_parameters params) {
    return ZSTD_CCtx_setParams(cctx, params);
}

size_t Test_ZSTD_decompressDCtx(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize)
{
    return ZSTD_decompressDCtx(dctx, dst, dstCapacity, src, srcSize);
}

size_t Test_ZSTD_CCtxParams_init(ZSTD_CCtx_params* cctxParams, int compressionLevel) {
    return ZSTD_CCtxParams_init(cctxParams, compressionLevel);
}

size_t Test_ZSTD_CCtxParams_getParameter(const ZSTD_CCtx_params* cctxParams, ZSTD_cParameter param, int* value) {
    return ZSTD_CCtxParams_getParameter(cctxParams, param, value);
}

size_t Test_ZSTD_CCtxParams_setParameter(ZSTD_CCtx_params* cctxParams, ZSTD_cParameter param, int value) {
    return ZSTD_CCtxParams_setParameter(cctxParams, param, value);
}

size_t Test_ZSTD_estimateCCtxSize(int compressionLevel) {
    return ZSTD_estimateCCtxSize(compressionLevel);
}

size_t Test_ZSTD_estimateCCtxSize_usingCParams(ZSTD_compressionParameters cparams) {
    return ZSTD_estimateCCtxSize_usingCParams(cparams);
}

size_t Test_ZSTD_estimateCCtxSize_usingCCtxParams(const ZSTD_CCtx_params* params) {
    return ZSTD_estimateCCtxSize_usingCCtxParams(params);
}

size_t Test_ZSTD_estimateDCtxSize(void) {
    return ZSTD_estimateDCtxSize();
}

ZSTD_CCtx* Test_ZSTD_initStaticCCtx(void* workspace, size_t workspaceSize) {
    return ZSTD_initStaticCCtx(workspace, workspaceSize);
}

ZSTD_DCtx* Test_ZSTD_initStaticDCtx(void* workspace, size_t workspaceSize) {
    return ZSTD_initStaticDCtx(workspace, workspaceSize);
}

size_t Test_ZSTD_sizeof_CCtx(const ZSTD_CCtx* cctx) {
    return ZSTD_sizeof_CCtx(cctx);
}

size_t Test_ZSTD_sizeof_DCtx(const ZSTD_DCtx* dctx) {
    return ZSTD_sizeof_DCtx(dctx);
}

void Test_ZSTD_registerSequenceProducer(ZSTD_CCtx* cctx,
  void* sequenceProducerState, ZSTD_sequenceProducer_F sequenceProducer){
    ZSTD_registerSequenceProducer(cctx, sequenceProducerState, sequenceProducer);
}

/*********************************************
* Begin of ZSTD_ZSTD_createCCtx
*********************************************/
TEST(ZSTD_ZSTD_createCCtx, AOCL_Compression_zstd_ZSTD_createCCtx_pass_common_1) {
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    EXPECT_EQ(cctx->streamStage, zcss_init);
    EXPECT_EQ(cctx->pledgedSrcSizePlusOne, 0);
    EXPECT_EQ(cctx->cdict, nullptr);
    EXPECT_EQ(cctx->requestedParams.fParams.contentSizeFlag, 1);
    EXPECT_EQ(cctx->requestedParams.compressionLevel, ZSTD_CLEVEL_DEFAULT);
    Test_ZSTD_freeCCtx(cctx);
}
/*********************************************
* End of ZSTD_ZSTD_createCCtx
*********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_createDCtx
*********************************************/
TEST(ZSTD_ZSTD_createDCtx, AOCL_Compression_zstd_ZSTD_createDCtx_pass_common_1) {
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx();
    EXPECT_NE(dctx, nullptr);
    Test_ZSTD_freeDCtx(dctx);
}
/*********************************************
 * End of ZSTD_ZSTD_createDCtx
 * *******************************************/

/*********************************************
* Begin of ZSTD_ZSTD_freeCCtx
*********************************************/
TEST(ZSTD_ZSTD_freeCCtx, AOCL_Compression_zstd_ZSTD_freeCCtx_pass_common_1) {
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx();
    EXPECT_NE(cctx, nullptr);
    EXPECT_EQ(Test_ZSTD_freeCCtx(cctx), 0);
}

TEST(ZSTD_ZSTD_freeCCtx, AOCL_Compression_zstd_ZSTD_freeCCtx_pass_common_2) { // cctx null
    EXPECT_EQ(Test_ZSTD_freeCCtx(NULL), 0);
}

TEST(ZSTD_ZSTD_freeCCtx, AOCL_Compression_zstd_ZSTD_freeCCtx_fail_common_3) { // free static alloc memory
    size_t const workspaceSize = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    void * workspace = malloc(workspaceSize);
    ZSTD_CCtx* cctx = Test_ZSTD_initStaticCCtx(workspace, workspaceSize);
    EXPECT_NE(cctx, nullptr);
    EXPECT_EQ(Test_ZSTD_freeCCtx(cctx),ERROR(memory_allocation)); // free should not be called on cctx set via ZSTD_initStaticCCtx
    free(workspace);
}
/*********************************************
* End of ZSTD_ZSTD_freeCCtx
*********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_freeDCtx
 * *******************************************/
TEST(ZSTD_ZSTD_freeDCtx, AOCL_Compression_zstd_ZSTD_freeDCtx_pass_common_1) {
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx();
    EXPECT_NE(dctx, nullptr);
    EXPECT_EQ(Test_ZSTD_freeDCtx(dctx), 0);
}

TEST(ZSTD_ZSTD_freeDCtx, AOCL_Compression_zstd_ZSTD_freeDCtx_pass_common_2) { // dctx null
    EXPECT_EQ(Test_ZSTD_freeDCtx(NULL), 0);
}

TEST(ZSTD_ZSTD_freeDCtx, AOCL_Compression_zstd_ZSTD_freeDCtx_fail_common_3) { // free static alloc memory
    size_t const workspaceSize = Test_ZSTD_estimateDCtxSize();
    void * workspace = malloc(workspaceSize);
    ZSTD_DCtx* dctx = Test_ZSTD_initStaticDCtx(workspace, workspaceSize);
    EXPECT_NE(dctx, nullptr);
    EXPECT_EQ(Test_ZSTD_freeDCtx(dctx), ERROR(memory_allocation)); // free should not be called on dctx set via ZSTD_initStaticDCtx
    free(workspace);
}
/*********************************************
 * End of ZSTD_ZSTD_freeDCtx
 * *******************************************/

/*********************************************
* Begin of ZSTD_ZSTD_createCCtx_advanced
*********************************************/
TEST(ZSTD_ZSTD_createCCtx_advanced, AOCL_Compression_zstd_ZSTD_createCCtx_advanced_pass_common_1) { //custom allocator
    size_t memSet = 0;
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, Test_ZSTD_custom_free_pass, &memSet };
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx_advanced(cMem);
    EXPECT_NE(cctx, nullptr);
    EXPECT_EQ(memSet, 1);

    ZSTD_freeCCtx(cctx);
    EXPECT_EQ(memSet, 0);
}

TEST(ZSTD_ZSTD_createCCtx_advanced, AOCL_Compression_zstd_ZSTD_createCCtx_advanced_pass_common_2) { //default allocator
    ZSTD_customMem cMem = { NULL, NULL, NULL };
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx_advanced(cMem);
    EXPECT_NE(cctx, nullptr);

    ZSTD_freeCCtx(cctx);
}

TEST(ZSTD_ZSTD_createCCtx_advanced, AOCL_Compression_zstd_ZSTD_createCCtx_advanced_pass_common_3) { //no opaque
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, Test_ZSTD_custom_free_pass, NULL };
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx_advanced(cMem);
    EXPECT_NE(cctx, nullptr);

    ZSTD_freeCCtx(cctx);
}

TEST(ZSTD_ZSTD_createCCtx_advanced, AOCL_Compression_zstd_ZSTD_createCCtx_advanced_fail_common_4) { //improper allocator
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_fail, Test_ZSTD_custom_free_fail, NULL };
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx_advanced(cMem);
    EXPECT_EQ(cctx, nullptr);

    ZSTD_freeCCtx(cctx);
}

TEST(ZSTD_ZSTD_createCCtx_advanced, AOCL_Compression_zstd_ZSTD_createCCtx_advanced_fail_common_5) { //custom deallocator null
    size_t memSet = 0;
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, NULL, &memSet };
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx_advanced(cMem);
    EXPECT_EQ(cctx, nullptr);
    EXPECT_EQ(memSet, 0);

    ZSTD_freeCCtx(cctx);
}

TEST(ZSTD_ZSTD_createCCtx_advanced, AOCL_Compression_zstd_ZSTD_createCCtx_advanced_fail_common_6) { //custom allocator null
    size_t memSet = 0;
    ZSTD_customMem cMem = { NULL, Test_ZSTD_custom_free_pass, &memSet };
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx_advanced(cMem);
    EXPECT_EQ(cctx, nullptr);
    EXPECT_EQ(memSet, 0);

    ZSTD_freeCCtx(cctx);
}
/*********************************************
* End of ZSTD_ZSTD_createCCtx_advanced
*********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_createDCtx_advanced
 * *******************************************/
TEST(ZSTD_ZSTD_createDCtx_advanced, AOCL_Compression_zstd_ZSTD_createDCtx_advanced_pass_common_1) { //custom allocator
    size_t memSet = 0;
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, Test_ZSTD_custom_free_pass, &memSet };
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx_advanced(cMem);
    EXPECT_NE(dctx, nullptr);
    EXPECT_EQ(memSet, 1);

    ZSTD_freeDCtx(dctx);
    EXPECT_EQ(memSet, 0);
}

TEST(ZSTD_ZSTD_createDCtx_advanced, AOCL_Compression_zstd_ZSTD_createDCtx_advanced_pass_common_2) { //default allocator
    ZSTD_customMem cMem = { NULL, NULL, NULL };
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx_advanced(cMem);
    EXPECT_NE(dctx, nullptr);

    ZSTD_freeDCtx(dctx);
}

TEST(ZSTD_ZSTD_createDCtx_advanced, AOCL_Compression_zstd_ZSTD_createDCtx_advanced_pass_common_3) { //no opaque
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, Test_ZSTD_custom_free_pass, NULL };
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx_advanced(cMem);
    EXPECT_NE(dctx, nullptr);

    ZSTD_freeDCtx(dctx);
}

TEST(ZSTD_ZSTD_createDCtx_advanced, AOCL_Compression_zstd_ZSTD_createDCtx_advanced_fail_common_4) { //improper allocator
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_fail, Test_ZSTD_custom_free_fail, NULL };
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx_advanced(cMem);
    EXPECT_EQ(dctx, nullptr);

    ZSTD_freeDCtx(dctx);
}

TEST(ZSTD_ZSTD_createDCtx_advanced, AOCL_Compression_zstd_ZSTD_createDCtx_advanced_fail_common_5) { //custom deallocator null
    size_t memSet = 0;
    ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, NULL, &memSet };
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx_advanced(cMem);
    EXPECT_EQ(dctx, nullptr);
    EXPECT_EQ(memSet, 0);

    ZSTD_freeDCtx(dctx);
}

TEST(ZSTD_ZSTD_createDCtx_advanced, AOCL_Compression_zstd_ZSTD_createDCtx_advanced_fail_common_6) { //custom allocator null
    size_t memSet = 0;
    ZSTD_customMem cMem = { NULL, Test_ZSTD_custom_free_pass, &memSet };
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx_advanced(cMem);
    EXPECT_EQ(dctx, nullptr);
    EXPECT_EQ(memSet, 0);

    ZSTD_freeDCtx(dctx);
}
/*********************************************
 * End of ZSTD_ZSTD_createDCtx_advanced
 * *******************************************/

/*********************************************
* Begin of ZSTD_ZSTD_createCCtxParams
*********************************************/
TEST(ZSTD_ZSTD_createCCtxParams, AOCL_Compression_zstd_ZSTD_createCCtxParams_pass_common_1) {
    ZSTD_CCtx_params* params = Test_ZSTD_createCCtxParams();
    EXPECT_NE(params, nullptr);
    Test_ZSTD_freeCCtxParams(params);
}
/*********************************************
* End of ZSTD_ZSTD_createCCtxParams
*********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_freeCCtxParams
*********************************************/
TEST(ZSTD_ZSTD_freeCCtxParams, AOCL_Compression_zstd_ZSTD_freeCCtx_pass_common_1) {
    ZSTD_CCtx_params* params = Test_ZSTD_createCCtxParams();
    EXPECT_NE(params, nullptr);
    EXPECT_EQ(Test_ZSTD_freeCCtxParams(params), 0);
}

TEST(ZSTD_ZSTD_freeCCtxParams, AOCL_Compression_zstd_ZSTD_freeCCtx_pass_common_2) { // params null
    EXPECT_EQ(Test_ZSTD_freeCCtxParams(NULL), 0);
}
/*********************************************
* End of ZSTD_ZSTD_freeCCtxParams
*********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_CCtx_setParameter
*********************************************/
class ZSTD_ZSTD_CCtx_setParameter : public AOCL_setup_zstd {
public:

    void SetUp() override {
        cctx = Test_ZSTD_createCCtx();
        EXPECT_NE(cctx, nullptr);
    }

    ~ZSTD_ZSTD_CCtx_setParameter() {
        Test_ZSTD_freeCCtx(cctx);
    }

    ZSTD_CCtx* cctx;
};

TEST_F(ZSTD_ZSTD_CCtx_setParameter, AOCL_Compression_zstd_ZSTD_CCtx_setParameter_pass_common_1) { // valid param and value
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_compressionLevel);
    CHECK_PASS_ZSTD(bounds.error);
    size_t ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
}

#ifdef ZSTD_MULTITHREAD // ZSTD_c_rsyncable = ZSTD_c_experimentalParam1 is not supported when not compiled with multithreading
TEST_F(ZSTD_ZSTD_CCtx_setParameter, AOCL_Compression_zstd_ZSTD_CCtx_setParameter_pass_common_2) { // experimental param
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_experimentalParam1);
    CHECK_PASS_ZSTD(bounds.error);
    size_t ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_experimentalParam1, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_experimentalParam1, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
}
#endif

TEST_F(ZSTD_ZSTD_CCtx_setParameter, AOCL_Compression_zstd_ZSTD_CCtx_setParameter_fail_common_3) { // invalid param
    size_t ret = Test_ZSTD_CCtx_setParameter(cctx, (ZSTD_cParameter)5555, 0);
    EXPECT_EQ(ret, ERROR(parameter_unsupported));
}

TEST_F(ZSTD_ZSTD_CCtx_setParameter, AOCL_Compression_zstd_ZSTD_CCtx_setParameter_fail_common_4) { // invalid value
    size_t ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, get_cparam_below_lower(ZSTD_c_windowLog));
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
    ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, get_cparam_above_upper(ZSTD_c_windowLog));
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_CCtx_setParameter, AOCL_Compression_zstd_ZSTD_CCtx_setParameter_fail_common_5) { // cctx is null
    size_t ret = Test_ZSTD_CCtx_setParameter(NULL, ZSTD_c_compressionLevel, get_cparam_within_bounds(ZSTD_c_compressionLevel));
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtx_setParameter
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_DCtx_setParameter
*********************************************/
class ZSTD_ZSTD_DCtx_setParameter : public AOCL_setup_zstd {
public:

    void SetUp() override {
        dctx = Test_ZSTD_createDCtx();
        EXPECT_NE(dctx, nullptr);
    }

    ~ZSTD_ZSTD_DCtx_setParameter() {
        Test_ZSTD_freeDCtx(dctx);
    }

    ZSTD_DCtx* dctx;
};

TEST_F(ZSTD_ZSTD_DCtx_setParameter, AOCL_Compression_zstd_ZSTD_DCtx_setParameter_pass_common_1) { // valid param and value
    ZSTD_bounds bounds = Test_ZSTD_dParam_getBounds(ZSTD_d_windowLogMax);
    CHECK_PASS_ZSTD(bounds.error);
    size_t ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
}

TEST_F(ZSTD_ZSTD_DCtx_setParameter, AOCL_Compression_zstd_ZSTD_DCtx_setParameter_fail_common_2) { // experimental param
    ZSTD_bounds bounds = Test_ZSTD_dParam_getBounds(ZSTD_d_experimentalParam1);
    CHECK_PASS_ZSTD(bounds.error);
    size_t ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_experimentalParam1, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_experimentalParam1, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
}

TEST_F(ZSTD_ZSTD_DCtx_setParameter, AOCL_Compression_zstd_ZSTD_DCtx_setParameter_fail_common_3) { // invalid param
    size_t ret = Test_ZSTD_DCtx_setParameter(dctx, (ZSTD_dParameter)5555, 0);
    EXPECT_EQ(ret, ERROR(parameter_unsupported));
}

TEST_F(ZSTD_ZSTD_DCtx_setParameter, AOCL_Compression_zstd_ZSTD_DCtx_setParameter_fail_common_4) { // invalid value
    size_t ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, get_dparam_below_lower(ZSTD_d_windowLogMax));
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
    ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, get_dparam_above_upper(ZSTD_d_windowLogMax));
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_DCtx_setParameter, AOCL_Compression_zstd_ZSTD_DCtx_setParameter_fail_common_5) { // dctx is null
    size_t ret = Test_ZSTD_DCtx_setParameter(NULL, ZSTD_d_windowLogMax, get_dparam_within_bounds(ZSTD_d_windowLogMax));
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_setParameter
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_CCtx_getParameter
*********************************************/
class ZSTD_ZSTD_CCtx_getParameter : public AOCL_setup_zstd {
public:

    void SetUp() override {
        cctx = Test_ZSTD_createCCtx();
        EXPECT_NE(cctx, nullptr);
    }

    ~ZSTD_ZSTD_CCtx_getParameter() {
        Test_ZSTD_freeCCtx(cctx);
    }

    ZSTD_CCtx* cctx;
};

TEST_F(ZSTD_ZSTD_CCtx_getParameter, AOCL_Compression_zstd_ZSTD_CCtx_getParameter_pass_common_1) { // valid param and value
    int value = 0;
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_compressionLevel);
    CHECK_PASS_ZSTD(bounds.error);
    
    size_t ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_compressionLevel, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.lowerBound);

    ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_compressionLevel, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.upperBound);
}

#ifdef ZSTD_MULTITHREAD // ZSTD_c_rsyncable = ZSTD_c_experimentalParam1 is not supported when not compiled with multithreading
TEST_F(ZSTD_ZSTD_CCtx_getParameter, AOCL_Compression_zstd_ZSTD_CCtx_getParameter_pass_common_2) { // experimental param
    int value = 0;
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_experimentalParam1);
    CHECK_PASS_ZSTD(bounds.error);

    size_t ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_experimentalParam1, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_experimentalParam1, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.lowerBound);

    ret = Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_experimentalParam1, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_experimentalParam1, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.upperBound);
}
#endif

TEST_F(ZSTD_ZSTD_CCtx_getParameter, AOCL_Compression_zstd_ZSTD_CCtx_getParameter_fail_common_3) { // invalid param
    int value = 0;
    size_t ret = Test_ZSTD_CCtx_getParameter(cctx, (ZSTD_cParameter)5555, &value);
    EXPECT_EQ(ret, ERROR(parameter_unsupported));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtx_getParameter
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_DCtx_getParameter
 * *******************************************/
class ZSTD_ZSTD_DCtx_getParameter : public AOCL_setup_zstd {
public:

    void SetUp() override {
        dctx = Test_ZSTD_createDCtx();
        EXPECT_NE(dctx, nullptr);
    }

    ~ZSTD_ZSTD_DCtx_getParameter() {
        Test_ZSTD_freeDCtx(dctx);
    }

    ZSTD_DCtx* dctx;
};

TEST_F(ZSTD_ZSTD_DCtx_getParameter, AOCL_Compression_zstd_ZSTD_DCtx_getParameter_pass_common_1) { // valid param and value
    int value = 0;
    ZSTD_bounds bounds = Test_ZSTD_dParam_getBounds(ZSTD_d_windowLogMax);
    CHECK_PASS_ZSTD(bounds.error);

    size_t ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.lowerBound);

    ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.upperBound);
}

TEST_F(ZSTD_ZSTD_DCtx_getParameter, AOCL_Compression_zstd_ZSTD_DCtx_getParameter_fail_common_2) { // experimental param
    int value = 0;
    ZSTD_bounds bounds = Test_ZSTD_dParam_getBounds(ZSTD_d_experimentalParam1);
    CHECK_PASS_ZSTD(bounds.error);

    size_t ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_experimentalParam1, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_DCtx_getParameter(dctx, ZSTD_d_experimentalParam1, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.lowerBound);

    ret = Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_experimentalParam1, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_DCtx_getParameter(dctx, ZSTD_d_experimentalParam1, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.upperBound);
}

TEST_F(ZSTD_ZSTD_DCtx_getParameter, AOCL_Compression_zstd_ZSTD_DCtx_getParameter_fail_common_3) { // invalid param
    int value = 0;
    size_t ret = Test_ZSTD_DCtx_getParameter(dctx, (ZSTD_dParameter)5555, &value);
    EXPECT_EQ(ret, ERROR(parameter_unsupported));
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_getParameter
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_CCtx_setParams
*********************************************/
class ZSTD_ZSTD_CCtx_setParams_base : public AOCL_setup_zstd {
public:

    void SetUp() override {
        cctx = Test_ZSTD_createCCtx();
        EXPECT_NE(cctx, nullptr);

        // set valid values here. values are overriden by test cases as need be
        memset(&params, 0, sizeof(ZSTD_parameters));
        params.cParams.windowLog       = get_cparam_within_bounds(ZSTD_c_windowLog);
        params.cParams.chainLog        = get_cparam_within_bounds(ZSTD_c_chainLog);
        params.cParams.hashLog         = get_cparam_within_bounds(ZSTD_c_hashLog);
        params.cParams.searchLog       = get_cparam_within_bounds(ZSTD_c_searchLog);
        params.cParams.minMatch        = get_cparam_within_bounds(ZSTD_c_minMatch);
        params.cParams.targetLength    = get_cparam_within_bounds(ZSTD_c_targetLength);
        params.cParams.strategy        = (ZSTD_strategy)get_cparam_within_bounds(ZSTD_c_strategy);
        params.fParams.contentSizeFlag = 0;
        params.fParams.checksumFlag    = 0;
        params.fParams.noDictIDFlag    = 0;
    }

    void validate_cparams() {
        int value = 0;
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_windowLog, &value)); EXPECT_EQ(value, params.cParams.windowLog);
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_chainLog, &value)); EXPECT_EQ(value, params.cParams.chainLog);
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_hashLog, &value)); EXPECT_EQ(value, params.cParams.hashLog);
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_searchLog, &value)); EXPECT_EQ(value, params.cParams.searchLog);
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_minMatch, &value)); EXPECT_EQ(value, params.cParams.minMatch);
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_targetLength, &value)); EXPECT_EQ(value, params.cParams.targetLength);
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_strategy, &value)); EXPECT_EQ(value, (int)params.cParams.strategy);
    }

    virtual ~ZSTD_ZSTD_CCtx_setParams_base() {
        Test_ZSTD_freeCCtx(cctx);
    }

    ZSTD_CCtx* cctx;
    ZSTD_parameters params;
};

class ZSTD_ZSTD_CCtx_setParams : public ZSTD_ZSTD_CCtx_setParams_base {};

TEST_F(ZSTD_ZSTD_CCtx_setParams, AOCL_Compression_zstd_ZSTD_CCtx_setParams_pass_common_1) { // valid values
    int value = 0;
    size_t ret = Test_ZSTD_CCtx_setParams(cctx, params);
    CHECK_PASS_ZSTD(ret);
    validate_cparams();
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_contentSizeFlag, &value)); EXPECT_EQ(value, 0);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_checksumFlag, &value)); EXPECT_EQ(value, 0);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_dictIDFlag, &value)); EXPECT_EQ(value, 1); // noDictIDFlag = !ZSTD_c_dictIDFlag
}

TEST_F(ZSTD_ZSTD_CCtx_setParams, AOCL_Compression_zstd_ZSTD_CCtx_setParams_fail_common_2) { // invalid cParams < min
    params.cParams.windowLog       = get_cparam_below_lower(ZSTD_c_windowLog);
    params.cParams.chainLog        = get_cparam_below_lower(ZSTD_c_chainLog);
    params.cParams.hashLog         = get_cparam_below_lower(ZSTD_c_hashLog);
    params.cParams.searchLog       = get_cparam_below_lower(ZSTD_c_searchLog);
    params.cParams.minMatch        = get_cparam_below_lower(ZSTD_c_minMatch);
    params.cParams.targetLength    = get_cparam_below_lower(ZSTD_c_targetLength);
    size_t ret = Test_ZSTD_CCtx_setParams(cctx, params);
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_CCtx_setParams, AOCL_Compression_zstd_ZSTD_CCtx_setParams_fail_common_3) { // invalid cParams > max
    params.cParams.windowLog       = get_cparam_above_upper(ZSTD_c_windowLog);
    params.cParams.chainLog        = get_cparam_above_upper(ZSTD_c_chainLog);
    params.cParams.hashLog         = get_cparam_above_upper(ZSTD_c_hashLog);
    params.cParams.searchLog       = get_cparam_above_upper(ZSTD_c_searchLog);
    params.cParams.minMatch        = get_cparam_above_upper(ZSTD_c_minMatch);
    params.cParams.targetLength    = get_cparam_above_upper(ZSTD_c_targetLength);
    size_t ret = Test_ZSTD_CCtx_setParams(cctx, params);
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_CCtx_setParams, AOCL_Compression_zstd_ZSTD_CCtx_setParams_pass_common_4) { // invalid fParams < min, values get set to 0 or 1
    int value = 0;
    params.fParams.contentSizeFlag = get_cparam_below_lower(ZSTD_c_contentSizeFlag);
    params.fParams.checksumFlag    = get_cparam_below_lower(ZSTD_c_checksumFlag);
    params.fParams.noDictIDFlag    = get_cparam_below_lower(ZSTD_c_dictIDFlag);
    size_t ret = Test_ZSTD_CCtx_setParams(cctx, params);
    CHECK_PASS_ZSTD(ret);
    validate_cparams();
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_contentSizeFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_checksumFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_dictIDFlag, &value)); EXPECT_EQ(value, 0); // noDictIDFlag = !ZSTD_c_dictIDFlag
}

TEST_F(ZSTD_ZSTD_CCtx_setParams, AOCL_Compression_zstd_ZSTD_CCtx_setParams_pass_common_5) { // invalid fParams > max, values get set to 0 or 1
    int value = 0;
    params.fParams.contentSizeFlag = get_cparam_above_upper(ZSTD_c_contentSizeFlag);
    params.fParams.checksumFlag    = get_cparam_above_upper(ZSTD_c_checksumFlag);
    params.fParams.noDictIDFlag    = get_cparam_above_upper(ZSTD_c_dictIDFlag);
    size_t ret = Test_ZSTD_CCtx_setParams(cctx, params);
    CHECK_PASS_ZSTD(ret);
    validate_cparams();
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_contentSizeFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_checksumFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_dictIDFlag, &value)); EXPECT_EQ(value, 0); // noDictIDFlag = !ZSTD_c_dictIDFlag
}

TEST_F(ZSTD_ZSTD_CCtx_setParams, AOCL_Compression_zstd_ZSTD_CCtx_setParams_fail_common_4) { // cctx null
    size_t ret = Test_ZSTD_CCtx_setParams(NULL, params);
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtx_setParams
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_CCtx_setCParams
*********************************************/
class ZSTD_ZSTD_CCtx_setCParams : public ZSTD_ZSTD_CCtx_setParams_base {};

TEST_F(ZSTD_ZSTD_CCtx_setCParams, AOCL_Compression_zstd_ZSTD_CCtx_setCParams_pass_common_1) { // valid values
    size_t ret = Test_ZSTD_CCtx_setCParams(cctx, params.cParams);
    CHECK_PASS_ZSTD(ret);
    validate_cparams();
}

TEST_F(ZSTD_ZSTD_CCtx_setCParams, AOCL_Compression_zstd_ZSTD_CCtx_setCParams_fail_common_2) { // invalid cParams < min
    params.cParams.windowLog       = get_cparam_below_lower(ZSTD_c_windowLog);
    params.cParams.chainLog        = get_cparam_below_lower(ZSTD_c_chainLog);
    params.cParams.hashLog         = get_cparam_below_lower(ZSTD_c_hashLog);
    params.cParams.searchLog       = get_cparam_below_lower(ZSTD_c_searchLog);
    params.cParams.minMatch        = get_cparam_below_lower(ZSTD_c_minMatch);
    params.cParams.targetLength    = get_cparam_below_lower(ZSTD_c_targetLength);
    size_t ret = Test_ZSTD_CCtx_setCParams(cctx, params.cParams);
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_CCtx_setCParams, AOCL_Compression_zstd_ZSTD_CCtx_setCParams_fail_common_3) { // invalid cParams > max
    params.cParams.windowLog       = get_cparam_above_upper(ZSTD_c_windowLog);
    params.cParams.chainLog        = get_cparam_above_upper(ZSTD_c_chainLog);
    params.cParams.hashLog         = get_cparam_above_upper(ZSTD_c_hashLog);
    params.cParams.searchLog       = get_cparam_above_upper(ZSTD_c_searchLog);
    params.cParams.minMatch        = get_cparam_above_upper(ZSTD_c_minMatch);
    params.cParams.targetLength    = get_cparam_above_upper(ZSTD_c_targetLength);
    size_t ret = Test_ZSTD_CCtx_setCParams(cctx, params.cParams);
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_CCtx_setCParams, AOCL_Compression_zstd_ZSTD_CCtx_setCParams_fail_common_4) { // cctx null
    size_t ret = Test_ZSTD_CCtx_setCParams(NULL, params.cParams);
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtx_setCParams
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_CCtx_setFParams
*********************************************/
class ZSTD_ZSTD_CCtx_setFParams : public AOCL_setup_zstd {
public:

    void SetUp() override {
        cctx = Test_ZSTD_createCCtx();
        EXPECT_NE(cctx, nullptr);
    }

    ~ZSTD_ZSTD_CCtx_setFParams() {
        Test_ZSTD_freeCCtx(cctx);
    }

    ZSTD_CCtx* cctx;
    ZSTD_frameParameters params;
};

TEST_F(ZSTD_ZSTD_CCtx_setFParams, AOCL_Compression_zstd_ZSTD_CCtx_setFParams_pass_common_1) { // valid values
    int value = 0;
    params.contentSizeFlag = 0;
    params.checksumFlag = 0;
    params.noDictIDFlag = 0;
    size_t ret = Test_ZSTD_CCtx_setFParams(cctx, params);
    CHECK_PASS_ZSTD(ret);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_contentSizeFlag, &value)); EXPECT_EQ(value, 0);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_checksumFlag, &value)); EXPECT_EQ(value, 0);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_dictIDFlag, &value)); EXPECT_EQ(value, 1); // noDictIDFlag = !ZSTD_c_dictIDFlag
}

TEST_F(ZSTD_ZSTD_CCtx_setFParams, AOCL_Compression_zstd_ZSTD_CCtx_setFParams_pass_common_2) { // invalid fParams < min, values get set to 0 or 1
    int value = 0;
    params.contentSizeFlag = get_cparam_below_lower(ZSTD_c_contentSizeFlag);
    params.checksumFlag    = get_cparam_below_lower(ZSTD_c_checksumFlag);
    params.noDictIDFlag    = get_cparam_below_lower(ZSTD_c_dictIDFlag);
    size_t ret = Test_ZSTD_CCtx_setFParams(cctx, params);
    CHECK_PASS_ZSTD(ret);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_contentSizeFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_checksumFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_dictIDFlag, &value)); EXPECT_EQ(value, 0); // noDictIDFlag = !ZSTD_c_dictIDFlag
}

TEST_F(ZSTD_ZSTD_CCtx_setFParams, AOCL_Compression_zstd_ZSTD_CCtx_setFParams_pass_common_3) { // invalid fParams > max, values get set to 0 or 1
    int value = 0;
    params.contentSizeFlag = get_cparam_above_upper(ZSTD_c_contentSizeFlag);
    params.checksumFlag    = get_cparam_above_upper(ZSTD_c_checksumFlag);
    params.noDictIDFlag    = get_cparam_above_upper(ZSTD_c_dictIDFlag);
    size_t ret = Test_ZSTD_CCtx_setFParams(cctx, params);
    CHECK_PASS_ZSTD(ret);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_contentSizeFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_checksumFlag, &value)); EXPECT_EQ(value, 1);
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_dictIDFlag, &value)); EXPECT_EQ(value, 0); // noDictIDFlag = !ZSTD_c_dictIDFlag
}

TEST_F(ZSTD_ZSTD_CCtx_setFParams, AOCL_Compression_zstd_ZSTD_CCtx_setFParams_fail_common_4) { // cctx null
    size_t ret = Test_ZSTD_CCtx_setFParams(NULL, params);
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtx_setFParams
 *********************************************/

/*********************************************
* Begin of ZSTD_CCtx_setParametersUsingCCtxParams
*********************************************/
class ZSTD_ZSTD_CCtx_setParametersUsingCCtxParams : public AOCL_setup_zstd {
public:

    void SetUp() override {
        cctx = Test_ZSTD_createCCtx();
        cctxParams = Test_ZSTD_createCCtxParams();
        EXPECT_NE(cctx, nullptr);
        EXPECT_NE(cctxParams, nullptr);
    }

    ~ZSTD_ZSTD_CCtx_setParametersUsingCCtxParams() {
        Test_ZSTD_freeCCtx(cctx);
        Test_ZSTD_freeCCtxParams(cctxParams);
    }

    ZSTD_CCtx* cctx;
    ZSTD_CCtx_params* cctxParams;
};

TEST_F(ZSTD_ZSTD_CCtx_setParametersUsingCCtxParams, AOCL_Compression_zstd_ZSTD_CCtx_setParametersUsingCCtxParams_pass_common_1) { // valid
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_compressionLevel, get_cparam_within_bounds(ZSTD_c_compressionLevel)));
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_windowLog, get_cparam_within_bounds(ZSTD_c_windowLog)));
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_hashLog, get_cparam_within_bounds(ZSTD_c_hashLog)));
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_chainLog, get_cparam_within_bounds(ZSTD_c_chainLog)));
    
    size_t ret = Test_ZSTD_CCtx_setParametersUsingCCtxParams(cctx, cctxParams);
    CHECK_PASS_ZSTD(ret);
}

TEST_F(ZSTD_ZSTD_CCtx_setParametersUsingCCtxParams, AOCL_Compression_zstd_ZSTD_CCtx_setParametersUsingCCtxParams_fail_common_2) { // cctx is null
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_compressionLevel, get_cparam_within_bounds(ZSTD_c_compressionLevel)));

    size_t ret = Test_ZSTD_CCtx_setParametersUsingCCtxParams(NULL, cctxParams);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_CCtx_setParametersUsingCCtxParams, AOCL_Compression_zstd_ZSTD_CCtx_setParametersUsingCCtxParams_fail_common_3) { // cctxParams is null
    size_t ret = Test_ZSTD_CCtx_setParametersUsingCCtxParams(cctx, NULL);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_CCtx_setParametersUsingCCtxParams, AOCL_Compression_zstd_ZSTD_CCtx_setParametersUsingCCtxParams_fail_common_4) { // streamStage != zcss_init
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_compressionLevel, get_cparam_within_bounds(ZSTD_c_compressionLevel)));
    cctx->streamStage = zcss_flush;
    size_t ret = Test_ZSTD_CCtx_setParametersUsingCCtxParams(cctx, cctxParams);
    EXPECT_EQ(ret, ERROR(stage_wrong));
}
/*********************************************
 * End of ZSTD_CCtx_setParametersUsingCCtxParams
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_CCtxParams_init
*********************************************/
void ZSTD_ZSTD_CCtxParams_init::SetUp() {
    cctxParams = Test_ZSTD_createCCtxParams();
    EXPECT_NE(cctxParams, nullptr);
}

ZSTD_ZSTD_CCtxParams_init::~ZSTD_ZSTD_CCtxParams_init() {
    Test_ZSTD_freeCCtxParams(cctxParams);
}

TEST_F(ZSTD_ZSTD_CCtxParams_init, AOCL_Compression_zstd_ZSTD_CCtxParams_init_pass_common_1) { // valid
    EXPECT_EQ(Test_ZSTD_CCtxParams_init(cctxParams, ZSTD_CLEVEL_DEFAULT), 0);
    EXPECT_EQ(cctxParams->compressionLevel, ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_CCtxParams_init, AOCL_Compression_zstd_ZSTD_CCtxParams_init_fail_common_2) { // cctxParams is null
    EXPECT_EQ(Test_ZSTD_CCtxParams_init(NULL, ZSTD_CLEVEL_DEFAULT), ERROR(GENERIC));
}
/*********************************************
* End of ZSTD_ZSTD_CCtxParams_init
*********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_CCtxParams_getParameter
 * *********************************************/
class ZSTD_ZSTD_CCtxParams_getParameter : public ZSTD_ZSTD_CCtxParams_init {};

TEST_F(ZSTD_ZSTD_CCtxParams_getParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_getParameter_pass_common_1) { // valid param and value
    int value = 0;
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_compressionLevel);
    CHECK_PASS_ZSTD(bounds.error);

    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_compressionLevel, bounds.lowerBound));
    size_t ret = Test_ZSTD_CCtxParams_getParameter(cctxParams, ZSTD_c_compressionLevel, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.lowerBound);

    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_compressionLevel, bounds.upperBound));
    ret = Test_ZSTD_CCtxParams_getParameter(cctxParams, ZSTD_c_compressionLevel, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.upperBound);
}

#ifdef ZSTD_MULTITHREAD // ZSTD_c_rsyncable = ZSTD_c_experimentalParam1 is not supported when not compiled with multithreading
TEST_F(ZSTD_ZSTD_CCtxParams_getParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_getParameter_pass_common_2) { // experimental param
    int value = 0;
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_experimentalParam1);
    CHECK_PASS_ZSTD(bounds.error);

    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_experimentalParam1, bounds.lowerBound));
    size_t ret = Test_ZSTD_CCtxParams_getParameter(cctxParams, ZSTD_c_experimentalParam1, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.lowerBound);

    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_experimentalParam1, bounds.upperBound));
    ret = Test_ZSTD_CCtxParams_getParameter(cctxParams, ZSTD_c_experimentalParam1, &value);
    CHECK_PASS_ZSTD(ret);
    EXPECT_EQ(value, bounds.upperBound);
}
#endif

TEST_F(ZSTD_ZSTD_CCtxParams_getParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_getParameter_fail_common_3) { // invalid param
    int value = 0;
    size_t ret = Test_ZSTD_CCtxParams_getParameter(cctxParams, (ZSTD_cParameter)5555, &value);
    EXPECT_EQ(ret, ERROR(parameter_unsupported));
}

TEST_F(ZSTD_ZSTD_CCtxParams_getParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_getParameter_fail_common_4) { // cctxParams is null
    int value = 0;
    size_t ret = Test_ZSTD_CCtxParams_getParameter(NULL, ZSTD_c_compressionLevel, &value);
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtxParams_getParameter
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_CCtxParams_setParameter
 * *********************************************/
class ZSTD_ZSTD_CCtxParams_setParameter : public ZSTD_ZSTD_CCtxParams_init {};

TEST_F(ZSTD_ZSTD_CCtxParams_setParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_setParameter_pass_common_1) { // valid param and value
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_compressionLevel);
    CHECK_PASS_ZSTD(bounds.error);
    size_t ret = Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_compressionLevel, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_compressionLevel, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
}

#ifdef ZSTD_MULTITHREAD // ZSTD_c_rsyncable = ZSTD_c_experimentalParam1 is not supported when not compiled with multithreading
TEST_F(ZSTD_ZSTD_CCtxParams_setParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_setParameter_pass_common_2) { // experimental param
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_experimentalParam1);
    CHECK_PASS_ZSTD(bounds.error);
    size_t ret = Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_experimentalParam1, bounds.lowerBound);
    CHECK_PASS_ZSTD(ret);
    ret = Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_experimentalParam1, bounds.upperBound);
    CHECK_PASS_ZSTD(ret);
}
#endif

TEST_F(ZSTD_ZSTD_CCtxParams_setParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_setParameter_fail_common_3) { // invalid param
    size_t ret = Test_ZSTD_CCtxParams_setParameter(cctxParams, (ZSTD_cParameter)5555, 0);
    EXPECT_EQ(ret, ERROR(parameter_unsupported));
}

TEST_F(ZSTD_ZSTD_CCtxParams_setParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_setParameter_fail_common_4) { // invalid value
    size_t ret = Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_windowLog, get_cparam_below_lower(ZSTD_c_windowLog));
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
    ret = Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_windowLog, get_cparam_above_upper(ZSTD_c_windowLog));
    EXPECT_EQ(ret, ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_CCtxParams_setParameter, AOCL_Compression_zstd_ZSTD_CCtxParams_setParameter_fail_common_5) { // cctxParams is null
    size_t ret = Test_ZSTD_CCtxParams_setParameter(NULL, ZSTD_c_windowLog, get_cparam_within_bounds(ZSTD_c_windowLog));
    EXPECT_EQ(ret, ERROR(GENERIC));
}
/*********************************************
 * End of ZSTD_ZSTD_CCtxParams_setParameter
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_CCtx_reset
 * *********************************************/
class ZSTD_ZSTD_CCtx_reset : public AOCL_setup_zstd {
public:

    void SetUp() override {
        cctx = Test_ZSTD_createCCtx();
        EXPECT_NE(cctx, nullptr);
    }

    ~ZSTD_ZSTD_CCtx_reset() {
        Test_ZSTD_freeCCtx(cctx);
    }

    size_t compress_continue(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        CHECK_PASS_ZSTD(Test_ZSTD_compressStream2(cctx, &buffOut, &buffIn, ZSTD_e_continue));
        return buffOut.pos;
    }

    void setup_session(TestLoad_2* d)
    {
        int value;
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 9)); //set param to non-default value
        compress_continue(d->getCompressedBuff(), d->getCompressedSize(), d->getOrigData(), d->getOrigSize()); // start compression
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_getParameter(cctx, ZSTD_c_compressionLevel, &value));
        EXPECT_EQ(value, 9);
    }

    void end_session() // must be called only after setup_session() call
    {
        CHECK_PASS_ZSTD(Test_ZSTD_compressStream2(cctx, &buffOut, &buffIn, ZSTD_e_end));
    }

    ZSTD_CCtx* cctx;

private:
    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
};

TEST_F(ZSTD_ZSTD_CCtx_reset, AOCL_Compression_zstd_ZSTD_CCtx_reset_pass_common_1) { // reset session
    TestLoad_2 d(800);
    setup_session(&d);
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only)); // reset session
    CHECK_PASS_ZSTD(ZSTD_CCtx_getParameter(cctx, ZSTD_c_compressionLevel, &value));
    EXPECT_EQ(value, 9); // value still the same as only session is reset
}

TEST_F(ZSTD_ZSTD_CCtx_reset, AOCL_Compression_zstd_ZSTD_CCtx_reset_fail_common_2) { // reset parameter during active session
    TestLoad_2 d(800);
    setup_session(&d);
    CHECK_FAIL_ZSTD(Test_ZSTD_CCtx_reset(cctx, ZSTD_reset_parameters)); // reset parameter fails as session is active
}

TEST_F(ZSTD_ZSTD_CCtx_reset, AOCL_Compression_zstd_ZSTD_CCtx_reset_pass_common_3) { // reset parameter
    TestLoad_2 d(800);
    setup_session(&d);
    end_session(); //stop active session
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_reset(cctx, ZSTD_reset_parameters)); // reset parameter
    CHECK_PASS_ZSTD(ZSTD_CCtx_getParameter(cctx, ZSTD_c_compressionLevel, &value));
    EXPECT_EQ(value, ZSTD_CLEVEL_DEFAULT); // value reset to default
}

TEST_F(ZSTD_ZSTD_CCtx_reset, AOCL_Compression_zstd_ZSTD_CCtx_reset_pass_common_4) { // reset session and parameter
    TestLoad_2 d(800);
    setup_session(&d);
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_reset(cctx, ZSTD_reset_session_and_parameters)); // reset session and parameter
    CHECK_PASS_ZSTD(ZSTD_CCtx_getParameter(cctx, ZSTD_c_compressionLevel, &value));
    EXPECT_EQ(value, ZSTD_CLEVEL_DEFAULT); // value reset to default
}

TEST_F(ZSTD_ZSTD_CCtx_reset, AOCL_Compression_zstd_ZSTD_CCtx_reset_fail_common_5) { // cctx is null
    TestLoad_2 d(800);
    setup_session(&d);
    size_t ret = Test_ZSTD_CCtx_reset(NULL, ZSTD_reset_parameters);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_CCtx_reset, AOCL_Compression_zstd_ZSTD_CCtx_reset_fail_common_6) { // invalid ZSTD_ResetDirective
    TestLoad_2 d(800);
    setup_session(&d);
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_reset(cctx, (ZSTD_ResetDirective)(0))); // does nothing but passes
    CHECK_PASS_ZSTD(ZSTD_CCtx_getParameter(cctx, ZSTD_c_compressionLevel, &value));
    EXPECT_EQ(value, 9); // value still the same
}
/*********************************************
 * End of ZSTD_ZSTD_CCtx_reset
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_DCtx_reset
 * *********************************************/
class ZSTD_ZSTD_DCtx_reset : public AOCL_setup_zstd {
public:

    void SetUp() override {
        dctx = Test_ZSTD_createDCtx();
        EXPECT_NE(dctx, nullptr);
    }

    ~ZSTD_ZSTD_DCtx_reset() {
        Test_ZSTD_freeDCtx(dctx);
    }

    size_t compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        size_t cSize = Test_ZSTD_compress(dst, dstCapacity, src, srcSize, ZSTD_CLEVEL_DEFAULT);
        CHECK_PASS_ZSTD(cSize);
        return cSize;
    }

    size_t decompress_continue(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
    {
        buffOut.dst = dst;
        buffOut.size = dstCapacity;
        buffOut.pos = 0;
        buffIn.src = src;
        buffIn.size = srcSize;
        buffIn.pos = 0;
        CHECK_PASS_ZSTD(Test_ZSTD_decompressStream(dctx, &buffOut, &buffIn));
        return buffOut.pos;
    }

    void setup_session(TestLoad_2* d)
    {
        int value;
        CHECK_PASS_ZSTD(Test_ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, windowLogMax)); //set param to non-default value
        compressedSize = compress(d->getCompressedBuff(), d->getCompressedSize(), d->getOrigData(), d->getOrigSize()); // compress
        const size_t srcSz = compressedSize / 2;
        const size_t dstSz = d->getOrigSize() / 2;
        decompress_continue(d->getOrigData(), dstSz, d->getCompressedBuff(), srcSz); // start decompression
        CHECK_PASS_ZSTD(Test_ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value));
        EXPECT_EQ(value, windowLogMax);
    }

    void end_session(TestLoad_2* d) // must be called only after setup_session() call
    {
        buffOut.size = buffOut.pos + (d->getOrigSize() - buffOut.pos); // set to space for remaining bytes
        buffIn.size = buffIn.pos + (compressedSize - buffIn.pos); // set to remaining bytes
        size_t rem = Test_ZSTD_decompressStream(dctx, &buffOut, &buffIn);
        EXPECT_EQ(rem, 0); //all bytes consumed
    }

    ZSTD_DCtx* dctx;
    const int windowLogMax = (ZSTD_WINDOWLOG_LIMIT_DEFAULT - 1); // set to non-default

private:
    size_t compressedSize;
    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
};

TEST_F(ZSTD_ZSTD_DCtx_reset, AOCL_Compression_zstd_ZSTD_DCtx_reset_pass_common_1) { // reset session
    TestLoad_2 d(800);
    setup_session(&d);
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_DCtx_reset(dctx, ZSTD_reset_session_only)); // reset session
    CHECK_PASS_ZSTD(ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value));
    EXPECT_EQ(value, windowLogMax); // value still the same as only session is reset
}

TEST_F(ZSTD_ZSTD_DCtx_reset, AOCL_Compression_zstd_ZSTD_DCtx_reset_fail_common_2) { // reset parameter during active session
    dctx->streamStage = zdss_flush;
    CHECK_FAIL_ZSTD(Test_ZSTD_DCtx_reset(dctx, ZSTD_reset_parameters)); // reset parameter fails as session is active
}

TEST_F(ZSTD_ZSTD_DCtx_reset, AOCL_Compression_zstd_ZSTD_DCtx_reset_pass_common_3) { // reset parameter
    TestLoad_2 d(800);
    setup_session(&d);
    end_session(&d); //stop active session
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_DCtx_reset(dctx, ZSTD_reset_parameters)); // reset parameter
    CHECK_PASS_ZSTD(ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value));
    EXPECT_EQ(value, ZSTD_WINDOWLOG_LIMIT_DEFAULT); // value reset to default
}

TEST_F(ZSTD_ZSTD_DCtx_reset, AOCL_Compression_zstd_ZSTD_DCtx_reset_pass_common_4) { // reset session and parameter
    TestLoad_2 d(800);
    setup_session(&d);
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_DCtx_reset(dctx, ZSTD_reset_session_and_parameters)); // reset session and parameter
    CHECK_PASS_ZSTD(ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value));
    EXPECT_EQ(value, ZSTD_WINDOWLOG_LIMIT_DEFAULT); // value reset to default
}

TEST_F(ZSTD_ZSTD_DCtx_reset, AOCL_Compression_zstd_ZSTD_DCtx_reset_fail_common_5) { // dctx is null
    TestLoad_2 d(800);
    setup_session(&d);
    size_t ret = Test_ZSTD_DCtx_reset(NULL, ZSTD_reset_parameters);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_DCtx_reset, AOCL_Compression_zstd_ZSTD_DCtx_reset_fail_common_6) { // invalid ZSTD_ResetDirective
    TestLoad_2 d(800);
    setup_session(&d);
    int value;
    CHECK_PASS_ZSTD(Test_ZSTD_DCtx_reset(dctx, (ZSTD_ResetDirective)(0))); // does nothing but passes
    CHECK_PASS_ZSTD(ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value));
    EXPECT_EQ(value, windowLogMax); // value still the same
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_reset
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_estimateCCtxSize
 * *********************************************/
void ZSTD_ZSTD_estimateSize::monotonic_increasing(ZSTD_estimateSize_fp fp) // monotonically increasing values based on level for 1 to max
{ 
    int minLevel = 1;
    int maxLevel = Test_ZSTD_maxCLevel();
    size_t prevSize = 0;
    for (int i = minLevel; i <= maxLevel; i++) {
        size_t size = fp(i);
        EXPECT_GE(size, prevSize);
        prevSize = size;
    }
}

void ZSTD_ZSTD_estimateSize::level_below_0(ZSTD_estimateSize_fp fp) // level <= 0
{
    size_t sizeN = fp(0);
    EXPECT_GT(sizeN, 0);

    int minLevel = Test_ZSTD_minCLevel();
    size_t sizeMin = fp(minLevel);
    EXPECT_GT(sizeMin, 0);
}

void ZSTD_ZSTD_estimateSize::level_above_max(ZSTD_estimateSize_fp fp) // level > max
{
    int maxLevel = Test_ZSTD_maxCLevel();
    size_t sizeMax = fp(maxLevel);
    size_t sizeP = fp(maxLevel + 1);
    EXPECT_GT(sizeP, 0);
    EXPECT_EQ(sizeMax, sizeP);
}

class ZSTD_ZSTD_estimateCCtxSize : public ZSTD_ZSTD_estimateSize {};

TEST_F(ZSTD_ZSTD_estimateCCtxSize, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_pass_common_1) { // monotonically increasing values based on level for 1 to max
    monotonic_increasing(Test_ZSTD_estimateCCtxSize);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_pass_common_2) { // level <= 0
    level_below_0(Test_ZSTD_estimateCCtxSize);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_pass_common_3) { // level > max
    level_above_max(Test_ZSTD_estimateCCtxSize);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateCCtxSize
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_estimateCCtxSize_usingCParams
 * *********************************************/
void ZSTD_ZSTD_estimateSize_usingCParams::no_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCParams_fp fp) // no source estimate
{
    ZSTD_compressionParameters params = Test_ZSTD_getCParams(level, 0, 0);
    size_t size_params = fp(params);
    EXPECT_GT(size_params, 0);
    EXPECT_EQ(sz_no_params, size_params); // ZSTD_estimateSize_usingCParams should provide same estimate as ZSTD_estimateSize
}

void ZSTD_ZSTD_estimateSize_usingCParams::with_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCParams_fp fp) // with source estimate
{
    ZSTD_compressionParameters params = Test_ZSTD_getCParams(level, 128, 0); // source size estimate provided
    size_t size_params = fp(params);
    EXPECT_GT(size_params, 0);
    EXPECT_LT(size_params, sz_no_params); // ZSTD_estimateSize_usingCParams should provide tighter estimate
}

void ZSTD_ZSTD_estimateSize_usingCParams::level_below_0(ZSTD_estimateSize_usingCParams_fp fp) // level <= 0
{
    {
        ZSTD_compressionParameters params = Test_ZSTD_getCParams(0, 0, 0);
        size_t sizeN = fp(params);
        EXPECT_GT(sizeN, 0);
    }
    {
        int minLevel = Test_ZSTD_minCLevel();
        ZSTD_compressionParameters params = Test_ZSTD_getCParams(minLevel, 0, 0);
        size_t sizeMin = fp(params);
        EXPECT_GT(sizeMin, 0);
    }
}

void ZSTD_ZSTD_estimateSize_usingCParams::level_above_max(ZSTD_estimateSize_usingCParams_fp fp) // level > max
{
    size_t sizeP, sizeMax;
    int maxLevel = Test_ZSTD_maxCLevel();
    {
        ZSTD_compressionParameters params = Test_ZSTD_getCParams(maxLevel, 0, 0);
        sizeMax = fp(params);
    }
    {
        ZSTD_compressionParameters params = Test_ZSTD_getCParams(maxLevel + 1, 0, 0);
        sizeP = fp(params);
        EXPECT_GT(sizeP, 0);
        EXPECT_EQ(sizeMax, sizeP);
    }
}

class ZSTD_ZSTD_estimateCCtxSize_usingCParams : public ZSTD_ZSTD_estimateSize_usingCParams {};

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCParams_pass_common_1) { // no source estimate
    size_t size_no_params = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    no_source_estimate(size_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCCtxSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCParams_pass_common_2) { // with source estimate
    size_t size_no_params = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    with_source_estimate(size_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCCtxSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCParams_pass_common_3) { // level <= 0
    level_below_0(Test_ZSTD_estimateCCtxSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCParams_pass_common_4) { // level = 8, row based matchfinder
    size_t size_no_params = Test_ZSTD_estimateCCtxSize(8);
    with_source_estimate(size_no_params, 8, Test_ZSTD_estimateCCtxSize_usingCParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCParams_pass_common_5) { // level > max
    level_above_max(Test_ZSTD_estimateCCtxSize_usingCParams);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateCCtxSize_usingCParams
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams
 * *********************************************/
void ZSTD_ZSTD_estimateSize_usingCCtxParams::no_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCCtxParams_fp fp) // no source estimate
{
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_init(cctxParams, level));
    size_t size_params = fp(cctxParams);
    EXPECT_GT(size_params, 0);
    EXPECT_EQ(sz_no_params, size_params); // ZSTD_estimateSize_usingCCtxParams should provide same estimate as ZSTD_estimateSize
}

void ZSTD_ZSTD_estimateSize_usingCCtxParams::with_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCCtxParams_fp fp) // with source estimate
{
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_init(cctxParams, level));
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_srcSizeHint, 128)); // source size estimate provided
    size_t size_params = fp(cctxParams);
    EXPECT_GT(size_params, 0);
    EXPECT_LT(size_params, sz_no_params); // ZSTD_estimateSize_usingCCtxParams should provide tighter estimate
}

void ZSTD_ZSTD_estimateSize_usingCCtxParams::with_source_estimate_stable(size_t sz_no_params, int level, ZSTD_estimateSize_usingCCtxParams_fp fp) // with source estimate stable mode
{
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_init(cctxParams, level));
    cctxParams->inBufferMode = ZSTD_bm_stable;
    cctxParams->outBufferMode = ZSTD_bm_stable;
    CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_setParameter(cctxParams, ZSTD_c_srcSizeHint, 128)); // source size estimate provided
    size_t size_params = fp(cctxParams);
    EXPECT_GT(size_params, 0);
    EXPECT_LT(size_params, sz_no_params); // ZSTD_estimateSize_usingCCtxParams should provide tighter estimate
}

void ZSTD_ZSTD_estimateSize_usingCCtxParams::level_below_0(ZSTD_estimateSize_usingCCtxParams_fp fp) // level <= 0
{
    {
        CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_init(cctxParams, 0));
        size_t sizeN = fp(cctxParams);
        EXPECT_GT(sizeN, 0);
    }
    {
        int minLevel = Test_ZSTD_minCLevel();
        CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_init(cctxParams, minLevel));
        size_t sizeMin = fp(cctxParams);
        EXPECT_GT(sizeMin, 0);
    }
}

void ZSTD_ZSTD_estimateSize_usingCCtxParams::level_above_max(ZSTD_estimateSize_usingCCtxParams_fp fp) // level > max
{
    size_t sizeP, sizeMax;
    int maxLevel = Test_ZSTD_maxCLevel();
    {
        CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_init(cctxParams, maxLevel));
        sizeMax = fp(cctxParams);
    }
    {
        CHECK_PASS_ZSTD(Test_ZSTD_CCtxParams_init(cctxParams, maxLevel + 1));
        sizeP = fp(cctxParams);
        EXPECT_GT(sizeP, 0);
        EXPECT_EQ(sizeMax, sizeP);
    }
}

class ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams : public ZSTD_ZSTD_estimateSize_usingCCtxParams {};

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCCtxParams_pass_common_1) { // no source estimate
    size_t sz_no_params = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    no_source_estimate(sz_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCCtxSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCCtxParams_pass_common_2) { // with source estimate
    size_t sz_no_params = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    with_source_estimate(sz_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCCtxSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCCtxParams_pass_common_3) { // with source estimate stable mode
    size_t sz_no_params = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    with_source_estimate_stable(sz_no_params, ZSTD_CLEVEL_DEFAULT, Test_ZSTD_estimateCCtxSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCCtxParams_pass_common_4) { // level <= 0
    level_below_0(Test_ZSTD_estimateCCtxSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCCtxParams_pass_common_5) { // level = 8, row based matchfinder
    size_t sz_no_params = Test_ZSTD_estimateCCtxSize(8);
    with_source_estimate(sz_no_params, 8, Test_ZSTD_estimateCCtxSize_usingCCtxParams);
}

TEST_F(ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams, AOCL_Compression_zstd_ZSTD_estimateCCtxSize_usingCCtxParams_pass_common_6) { // level > max
    level_above_max(Test_ZSTD_estimateCCtxSize_usingCCtxParams);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateCCtxSize_usingCCtxParams
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_initStaticCCtx
 * *********************************************/
void ZSTD_ZSTD_initStatic::workspace_sufficient(ZSTD_Compress_API api) // large enough workspace
{
    size_t workspaceSize = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    void* workspace = malloc(workspaceSize);
    void* cctx = run_init(api, workspace, workspaceSize);
    EXPECT_NE(cctx, nullptr);
    free(workspace);
}

void ZSTD_ZSTD_initStatic::workspace_null(ZSTD_Compress_API api) // workspace is null
{
    void* cctx = run_init(api, NULL, 0);
    EXPECT_EQ(cctx, nullptr);
}

void ZSTD_ZSTD_initStatic::workspace_too_small(ZSTD_Compress_API api) // workspaceSize is too small
{
    size_t workspaceSize = sizeof(ZSTD_CCtx) - 1;
    void* workspace = malloc(workspaceSize);
    void* cctx = run_init(api, workspace, workspaceSize);
    EXPECT_EQ(cctx, nullptr);
    free(workspace);
}

void ZSTD_ZSTD_initStatic::workspace_not_aligned(ZSTD_Compress_API api) // workspace not 8-byte aligned
{
    size_t workspaceSize = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    void* workspace = malloc(workspaceSize + 1);
    void* cctx = run_init(api, (void*)((size_t)workspace + 1), workspaceSize);
    EXPECT_EQ(cctx, nullptr);
    free(workspace);
}

void* ZSTD_ZSTD_initStatic::run_init(ZSTD_Compress_API api, void* workspace, size_t workspaceSize)
{
    switch(api){
        case ZSTD_Compress_API::compress_cctx:
            return (void*)Test_ZSTD_initStaticCCtx(workspace, workspaceSize);
        case ZSTD_Compress_API::compress_stream_end:
            return (void*)Test_ZSTD_initStaticCStream(workspace, workspaceSize);
        default:
            return NULL;
    };
}

class ZSTD_ZSTD_initStaticCCtx : public ZSTD_ZSTD_initStatic {};

TEST_F(ZSTD_ZSTD_initStaticCCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticCCtx_pass_common_1) { 
    workspace_sufficient(ZSTD_Compress_API::compress_cctx);
}

TEST_F(ZSTD_ZSTD_initStaticCCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticCCtx_fail_common_2) {
    workspace_null(ZSTD_Compress_API::compress_cctx);
}

TEST_F(ZSTD_ZSTD_initStaticCCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticCCtx_fail_common_3) {
    workspace_too_small(ZSTD_Compress_API::compress_cctx);
}

TEST_F(ZSTD_ZSTD_initStaticCCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticCCtx_fail_common_4) {
    workspace_not_aligned(ZSTD_Compress_API::compress_cctx);
}
/*********************************************
 * End of ZSTD_ZSTD_initStaticCCtx
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_initStaticDCtx
 * *********************************************/
TEST(ZSTD_ZSTD_initStaticDCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticDCtx_pass_common_1) { // large enough workspace
    size_t const workspaceSize = Test_ZSTD_estimateDCtxSize();
    void * workspace = malloc(workspaceSize);
    ZSTD_DCtx* dctx = Test_ZSTD_initStaticDCtx(workspace, workspaceSize);
    EXPECT_NE(dctx, nullptr);
    free(workspace);
}

TEST(ZSTD_ZSTD_initStaticDCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticDCtx_fail_common_2) { // workspace is null
    ZSTD_DCtx* dctx = Test_ZSTD_initStaticDCtx(NULL, 0);
    EXPECT_EQ(dctx, nullptr);
}

TEST(ZSTD_ZSTD_initStaticDCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticDCtx_fail_common_3) { // workspaceSize is too small
    size_t const workspaceSize = Test_ZSTD_estimateDCtxSize() - 1;
    void * workspace = malloc(workspaceSize);
    ZSTD_DCtx* dctx = Test_ZSTD_initStaticDCtx(workspace, workspaceSize);
    EXPECT_EQ(dctx, nullptr);
    free(workspace);
}

TEST(ZSTD_ZSTD_initStaticDCtx, AOCL_Compression_zstd_Test_ZSTD_initStaticDCtx_fail_common_4) { // workspace not 8-byte aligned
    size_t const workspaceSize = Test_ZSTD_estimateDCtxSize();
    void * workspace = malloc(workspaceSize + 1);
    ZSTD_DCtx* dctx = Test_ZSTD_initStaticDCtx((void*)((size_t)workspace + 1), workspaceSize);
    EXPECT_EQ(dctx, nullptr);
    free(workspace);
}
/*********************************************
 * End of ZSTD_ZSTD_initStaticDCtx
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_estimateDCtxSize
 * *********************************************/
TEST(ZSTD_ZSTD_estimateDCtxSize, AOCL_Compression_zstd_ZSTD_estimateDCtxSize_pass_common_1) {
    size_t size = Test_ZSTD_estimateDCtxSize();
    EXPECT_GE(size, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateDCtxSize
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_sizeof_CCtx
 * *********************************************/
TEST(ZSTD_ZSTD_sizeof_CCtx, AOCL_Compression_zstd_ZSTD_sizeof_CCtx_pass_common_1) { // cctx created using ZSTD_createCCtx
    ZSTD_CCtx* cctx = Test_ZSTD_createCCtx();
    size_t size = Test_ZSTD_sizeof_CCtx(cctx);
    EXPECT_GT(size, 0);
    Test_ZSTD_freeCCtx(cctx);
}

TEST(ZSTD_ZSTD_sizeof_CCtx, AOCL_Compression_zstd_ZSTD_sizeof_CCtx_pass_common_2) { // cctx created using ZSTD_initStaticCCtx
    size_t const workspaceSize = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    void * workspace = malloc(workspaceSize);
    ZSTD_CCtx* cctx = Test_ZSTD_initStaticCCtx(workspace, workspaceSize);
    size_t size = Test_ZSTD_sizeof_CCtx(cctx);
    EXPECT_GT(size, 0);
    free(workspace);
}

TEST(ZSTD_ZSTD_sizeof_CCtx, AOCL_Compression_zstd_ZSTD_sizeof_CCtx_pass_common_3) { // cctx empty
    ZSTD_CCtx cctx;
    memset(&cctx, 0, sizeof(cctx));
    size_t size = Test_ZSTD_sizeof_CCtx(&cctx);
    EXPECT_GT(size, 0);
} 

TEST(ZSTD_ZSTD_sizeof_CCtx, AOCL_Compression_zstd_ZSTD_sizeof_CCtx_fail_common_4) { // cctx null
    size_t size = Test_ZSTD_sizeof_CCtx(NULL);
    EXPECT_EQ(size, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_sizeof_CCtx
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_sizeof_DCtx
 * *********************************************/
TEST(ZSTD_ZSTD_sizeof_DCtx, AOCL_Compression_zstd_ZSTD_sizeof_DCtx_pass_common_1) { // dctx created using ZSTD_createDCtx
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx();
    size_t size = Test_ZSTD_sizeof_DCtx(dctx);
    EXPECT_GT(size, 0);
    Test_ZSTD_freeDCtx(dctx);
}

TEST(ZSTD_ZSTD_sizeof_DCtx, AOCL_Compression_zstd_ZSTD_sizeof_DCtx_pass_common_2) { // dctx created using ZSTD_initStaticDCtx
    size_t const workspaceSize = Test_ZSTD_estimateDCtxSize();
    void * workspace = malloc(workspaceSize);
    ZSTD_DCtx* dctx = Test_ZSTD_initStaticDCtx(workspace, workspaceSize);
    size_t size = Test_ZSTD_sizeof_DCtx(dctx);
    EXPECT_GT(size, 0);
    free(workspace);
}

TEST(ZSTD_ZSTD_sizeof_DCtx, AOCL_Compression_zstd_ZSTD_sizeof_DCtx_pass_common_3) { // dctx empty
    ZSTD_DCtx dctx;
    memset(&dctx, 0, sizeof(dctx));
    size_t size = Test_ZSTD_sizeof_DCtx(&dctx);
    EXPECT_GT(size, 0);
}

TEST(ZSTD_ZSTD_sizeof_DCtx, AOCL_Compression_zstd_ZSTD_sizeof_DCtx_fail_common_4) { // dctx null
    size_t size = Test_ZSTD_sizeof_DCtx(NULL);
    EXPECT_EQ(size, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_sizeof_DCtx
 *********************************************/

/***********************************************
* Begin of ZSTD_ZSTD_compressCCtx
***********************************************/

class ZSTD_with_cctx : public ZSTD_ZSTD_compress_base {
public:
    ZSTD_with_cctx()
    {
        cctx = Test_ZSTD_createCCtx();
    }

    ~ZSTD_with_cctx()
    {
        Test_ZSTD_freeCCtx(cctx);
    }

    ZSTD_CCtx* getCtx() {
        return cctx;
    }

private:
    ZSTD_CCtx* cctx;
};

class ZSTD_ZSTD_compressCCtx : public ZSTD_with_cctx {};

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_1)
{
    compress_all_levels(ZSTD_Compress_API::compress_cctx, getCtx());
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress_cctx, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress_cctx, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_4)
{
    compress_insufficient_dstCapacity(ZSTD_Compress_API::compress_cctx, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_cctx, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_cctx, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_7)
{
    compress_level_lt_min(ZSTD_Compress_API::compress_cctx, getCtx());
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_pass_common_8)
{
    compress_level_gt_max(ZSTD_Compress_API::compress_cctx, getCtx());
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_9)
{
    compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API::compress_cctx, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressCCtx, AOCL_Compression_zstd_ZSTD_compressCCtx_fail_common_10)
{
    compress_cctx_null(ZSTD_Compress_API::compress_cctx);
}
/*********************************************
 * End of ZSTD_ZSTD_compressCCtx
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_decompressDCtx
*********************************************/
class ZSTD_ZSTD_decompressDCtx : public ZSTD_ZSTD_decompress_base
{
public:
    ZSTD_DCtx* dctx;

    // constructor to create dctx
    ZSTD_ZSTD_decompressDCtx()
    {
        dctx = ZSTD_createDCtx();
    }

    ~ZSTD_ZSTD_decompressDCtx()
    {
        ZSTD_freeDCtx(dctx);
    }

    void decompress_dctx_null(ZSTD_Decompress_API api) { // decompress dctx null
        create_frame();
        size_t decLen = run_decompress(api, NULL, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
        EXPECT_EQ(decLen, ERROR(GENERIC));
    }
};

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_1)
{
    decompress_pass(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_2)
{
    decompress_src_null(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_3)
{
    decompress_dst_null(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_4)
{
    decompress_buffer_inadequate(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_5)
{
    decompress_srcsize_0(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_6)
{
    decompress_src_null_srcsize_0(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_7)
{
    decompress_corrupt_magic_number(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_8)
{
    decompress_corrupt_frame_header(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_9)
{
    decompress_corrupt_data_block(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_10)
{
    decompress_garbage_after_frame(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_11)
{
    decompress_N_frames(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_12)
{
    decompress_checksumFlag_set(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_13)
{
    decompress_checksum_wrong(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_14)
{
    decompress_contentSizeFlag_set(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_15)
{
    decompress_noDictIDFlag_set(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_16)
{
    decompress_srcsize_less(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_17)
{
    decompress_srcsize_more(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_18)
{
    decompress_src_null_srcsize_0_dstsize_0(ZSTD_Decompress_API::decompress_dctx, dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_fail_common_19)
{
    decompress_dctx_null(ZSTD_Decompress_API::decompress_dctx);
}

TEST_F(ZSTD_ZSTD_decompressDCtx, AOCL_Compression_zstd_ZSTD_decompressDCtx_pass_common_20)
{
    decompress_skip(ZSTD_Decompress_API::decompress_dctx, dctx);
}
/*********************************************
 * End of ZSTD_ZSTD_decompressDCtx
 *********************************************/

/***********************************************
* Begin of ZSTD_ZSTD_compress2
***********************************************/
class ZSTD_ZSTD_compress2 : public ZSTD_with_cctx {};

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_1)
{
    compress_all_levels(ZSTD_Compress_API::compress2, getCtx());
}

#ifndef DEBUG_ASSERT_ENABLED
/* Assertions get triggered:
assert(input->size == 0)
assert(output->size == 0) */
TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT);
}
#endif

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_4)
{
    compress_insufficient_dstCapacity(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_7)
{
    compress_level_lt_min(ZSTD_Compress_API::compress2, getCtx());
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_pass_common_8)
{
    compress_level_gt_max(ZSTD_Compress_API::compress2, getCtx());
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_9)
{
    compress_cctx_null(ZSTD_Compress_API::compress2);
}

TEST_F(ZSTD_ZSTD_compress2, AOCL_Compression_zstd_ZSTD_compress2_fail_common_10)
{
    compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT);
}
/*********************************************
 * End of ZSTD_ZSTD_compress2
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_compress_advanced
*********************************************/
class ZSTD_ZSTD_compressed_advanced : public ZSTD_with_cctx {
public:
    void compress_outofbound_params(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // out of bound params
        TestLoad_2 d(800);
        ZSTD_parameters zparams;
        zparams = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, d.getOrigSize(), 0);
        ZSTD_compressionParameters cPar; // set out of bound params
        cPar.windowLog = get_cparam_above_upper(ZSTD_c_windowLog);
        cPar.chainLog = get_cparam_above_upper(ZSTD_c_chainLog);
        cPar.hashLog = get_cparam_above_upper(ZSTD_c_hashLog);
        cPar.searchLog = get_cparam_above_upper(ZSTD_c_searchLog);
        cPar.minMatch = get_cparam_above_upper(ZSTD_c_minMatch);
        cPar.targetLength = get_cparam_above_upper(ZSTD_c_targetLength);
        zparams.cParams = cPar;

        size_t outLen = Test_ZSTD_compress_advanced(cctx, d.getCompressedBuff(), d.getCompressedSize(),
            d.getOrigData(), d.getOrigSize(), NULL, 0, zparams);
        EXPECT_EQ(outLen, ERROR(parameter_outOfBound));
    }
};

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_1)
{
    compress_all_levels(ZSTD_Compress_API::compress_advanced, getCtx());
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_fail_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress_advanced, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_fail_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress_advanced, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_fail_common_4)
{
    compress_insufficient_dstCapacity(ZSTD_Compress_API::compress_advanced, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_advanced, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_advanced, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_7)
{
    compress_level_lt_min(ZSTD_Compress_API::compress_advanced, getCtx());
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_8)
{
    compress_level_gt_max(ZSTD_Compress_API::compress_advanced, getCtx());
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_fail_common_9)
{
    compress_cctx_null(ZSTD_Compress_API::compress_advanced);
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_fail_common_10)
{
    compress_outofbound_params(ZSTD_Compress_API::compress_advanced, getCtx());
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_fail_common_11)
{
    compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API::compress_advanced, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

#ifdef AOCL_ENABLE_THREADS
TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_12) // compress multithreaded. decompress reference. format compliance test.
{
    TestLoad_2 d((1024 * 1024 * 32) + 5); //use larger input so that compression gets triggered on multiple threads
    int level = 3;
    ZSTD_parameters param;
    param = Test_ZSTD_getParams(level, d.getOrigSize(), 0);
    Test_ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_compressionLevel, level);

    //Compress using multithreaded compressor
    size_t outLen = Test_ZSTD_compress_advanced(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, 0, param);
    //Decompress using reference decompressor Test_ZSTD_decompressDCtxRef
    //As ZSTD writes RAP frame inside skippable frame, compressed output must be format compliant
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen,
        Test_ZSTD_decompressDCtxRef));
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_13) //compress MT threads < decompress MT threads
{
    ASSERT_NE(test_omp_max_threads_set(5), 0); // system must have atleast 5 threads
    TestLoad_2 d((1024 * 1024 * 32) + 5); //use larger input so that compression gets triggered on multiple threads
    int level = 3;
    ZSTD_parameters param;
    param = Test_ZSTD_getParams(level, d.getOrigSize(), 0);
    Test_ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_compressionLevel, level);

    //Compress using multithreaded compressor
    size_t outLen = Test_ZSTD_compress_advanced(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, 0, param);
    
    //Decompress using multithreaded decompressor
    test_omp_max_threads_reset();
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen,
        Test_ZSTD_decompressDCtx));
}

TEST_F(ZSTD_ZSTD_compressed_advanced, AOCL_Compression_zstd_ZSTD_compress_advanced_pass_common_14) //compress MT threads > decompress MT threads
{
    test_omp_max_threads_reset();
    TestLoad_2 d((1024 * 1024 * 32) + 5); //use larger input so that compression gets triggered on multiple threads
    int level = 3;
    ZSTD_parameters param;
    param = Test_ZSTD_getParams(level, d.getOrigSize(), 0);
    Test_ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_compressionLevel, level);

    //Compress using multithreaded compressor
    size_t outLen = Test_ZSTD_compress_advanced(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, 0, param);

    //Decompress using multithreaded decompressor
    ASSERT_NE(test_omp_max_threads_set(5), 0); // system must have atleast 5 threads
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen,
        Test_ZSTD_decompressDCtx));
}
#endif
/*********************************************
 * End of ZSTD_ZSTD_compress_advanced
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_generateSequences
 ***********************************************/
class ZSTD_ZSTD_generateSequences : public ZSTD_with_cctx {};

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_pass_common_1)
{
    compress_all_levels(ZSTD_Compress_API::compress_sequence, getCtx());
}

#ifndef DEBUG_ASSERT_ENABLED
/* Assertions get triggered:
assert(input->size == 0)
assert(output->size == 0) */
TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_fail_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress_sequence, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_fail_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress_sequence, getCtx(), ZSTD_CLEVEL_DEFAULT);
}
#endif

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_fail_common_4)
{
    compress_insufficient_dstCapacity(ZSTD_Compress_API::compress_sequence, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_sequence, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_sequence, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_pass_common_7)
{
    compress_level_lt_min(ZSTD_Compress_API::compress_sequence, getCtx());
}

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_pass_common_8)
{
    compress_level_gt_max(ZSTD_Compress_API::compress_sequence, getCtx());
}

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_fail_common_9)
{
    compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API::compress_sequence, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_generateSequences, AOCL_Compression_zstd_ZSTD_generateSequences_fail_common_10)
{
    compress_cctx_null(ZSTD_Compress_API::compress_sequence);
}
/*********************************************
 * End of ZSTD_ZSTD_generateSequences
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_compressSequences
 ***********************************************/
class ZSTD_ZSTD_compressSequences : public ZSTD_with_cctx {
public:
    ZSTD_ZSTD_compressSequences() 
    {
        sequences = NULL;
    }

    ~ZSTD_ZSTD_compressSequences() 
    {
        if (sequences)
            free(sequences);
    }

    void alloc_sequences(size_t sequencesCnt) 
    {
        if (sequencesCnt > 0) {
            sequences = (ZSTD_Sequence*)malloc(sizeof(ZSTD_Sequence) * sequencesCnt);
            memset(sequences, 0, sizeof(ZSTD_Sequence) * sequencesCnt);
        }
    }

    size_t generate_sequences(ZSTD_CCtx* cctx, const void* src, size_t srcSize)
    {
        ZSTD_parameters zparams;
        zparams = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, srcSize, 0);
        zparams.fParams.contentSizeFlag = 1;
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParams(cctx, zparams));
        size_t outSeqsSize = srcSize;
        sequences = (ZSTD_Sequence*)malloc(srcSize * sizeof(ZSTD_Sequence));
        outSeqsSize = Test_ZSTD_generateSequences(cctx, sequences, outSeqsSize, src, srcSize);
        CHECK_PASS_ZSTD(outSeqsSize);
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_reset(cctx, ZSTD_reset_session_and_parameters));
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_blockDelimiters, ZSTD_sf_explicitBlockDelimiters));
        return outSeqsSize;
    }

    ZSTD_Sequence* sequences;
};

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_pass_common_1) // number of sequences = 0
{
    alloc_sequences(1);
    TestLoad_2 d(800);
    size_t ret = Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), sequences, 0, d.getOrigData(), d.getOrigSize());
    CHECK_PASS_ZSTD(ret);
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_2) // sequences is null
{
    size_t sequencesCnt = 1;
    TestLoad_2 d(800);
    size_t ret = Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), NULL, sequencesCnt, d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_3) // sequences is null and number of sequences = 0
{
    TestLoad_2 d(800);
    size_t ret = Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), NULL, 0, d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_4) // src is null
{
    TestLoad_2 d(800);
    size_t sequencesCnt = generate_sequences(getCtx(), d.getOrigData(), d.getOrigSize());
    size_t ret = Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), sequences, sequencesCnt, NULL, d.getOrigSize());
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_5) // dst is null
{
    TestLoad_2 d(800);
    size_t sequencesCnt = generate_sequences(getCtx(), d.getOrigData(), d.getOrigSize());
    size_t ret = Test_ZSTD_compressSequences(getCtx(), NULL, d.getCompressedSize(), sequences, sequencesCnt, d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_6) // insufficient dst capacity
{
    TestLoad_2 d(800, 80);
    size_t sequencesCnt = generate_sequences(getCtx(), d.getOrigData(), d.getOrigSize());
    size_t ret = Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), sequences, sequencesCnt, d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_pass_common_7) //src is null and srcSize = 0
{
    TestLoad_2 d(800);
    size_t sequencesCnt = generate_sequences(getCtx(), d.getOrigData(), d.getOrigSize());
    CHECK_PASS_ZSTD(Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), sequences, sequencesCnt, NULL, 0)); // valid. empty frame written.
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_8) //cctx is null
{
    TestLoad_2 d(800);
    size_t sequencesCnt = generate_sequences(getCtx(), d.getOrigData(), d.getOrigSize());
    size_t ret = Test_ZSTD_compressSequences(NULL, d.getCompressedBuff(), d.getCompressedSize(), sequences, sequencesCnt, d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_9) //srcSize less than expected
{
    TestLoad_2 d(800);
    size_t sequencesCnt = generate_sequences(getCtx(), d.getOrigData(), d.getOrigSize());
    size_t ret = Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), sequences, sequencesCnt, d.getOrigData(), d.getOrigSize() - 1);
    EXPECT_EQ(ret, ERROR(externalSequences_invalid));
}

TEST_F(ZSTD_ZSTD_compressSequences, AOCL_Compression_zstd_ZSTD_compressSequences_fail_common_10) //srcSize more than expected
{
    TestLoad_2 d(800);
    size_t sequencesCnt = generate_sequences(getCtx(), d.getOrigData(), d.getOrigSize());
    size_t ret = Test_ZSTD_compressSequences(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), sequences, sequencesCnt, d.getOrigData(), d.getOrigSize() + 1);
    EXPECT_EQ(ret, ERROR(externalSequences_invalid));
}
//pass cases and different frames are covered as part of ZSTD_ZSTD_generateSequences
/*********************************************
 * End of ZSTD_ZSTD_compressSequences
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_registerSequenceProducer
 ***********************************************/
size_t ValidSequenceProducer(void* sequenceProducerState, ZSTD_Sequence* outSeqs, size_t outSeqsSize,
  const void* src, size_t srcSize, const void* dict, size_t dictSize, int compressionLevel, size_t windowSize) {
    ZSTD_CCtx* cctx = (ZSTD_CCtx*)sequenceProducerState;
    ZSTD_parameters zparams;
    zparams = Test_ZSTD_getParams(compressionLevel, srcSize, 0);
    zparams.fParams.contentSizeFlag = 1;
    if (cctx) {
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParams(cctx, zparams));
    }
    memset(outSeqs, 0, outSeqsSize);
    size_t res = Test_ZSTD_generateSequences(cctx, outSeqs, outSeqsSize, src, srcSize);
    return res;
}

size_t DummySequenceProducer(void* sequenceProducerState, ZSTD_Sequence* outSeqs, size_t outSeqsSize,
    const void* src, size_t srcSize, const void* dict, size_t dictSize, int compressionLevel, size_t windowSize) {
    memset(outSeqs, 0, outSeqsSize);
    return 0;
}

size_t ErrorSequenceProducer(void* sequenceProducerState, ZSTD_Sequence* outSeqs, size_t outSeqsSize,
    const void* src, size_t srcSize, const void* dict, size_t dictSize, int compressionLevel, size_t windowSize) {
    return ERROR(GENERIC);
}

class ZSTD_ZSTD_registerSequenceProducer : public ZSTD_with_cctx {
public:
    ZSTD_ZSTD_registerSequenceProducer() {
        sps = Test_ZSTD_createCCtx();
    }

    ~ZSTD_ZSTD_registerSequenceProducer() {
        Test_ZSTD_freeCCtx(sps);
    }

    ZSTD_CCtx* sps;
};

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_pass_common_1) // valid custom sequence producer
{
    Test_ZSTD_registerSequenceProducer(getCtx(), sps, ValidSequenceProducer); // register valid producer
    EXPECT_NE(getCtx()->requestedParams.extSeqProdFunc, nullptr);
    EXPECT_NE(getCtx()->requestedParams.extSeqProdState, nullptr);

    // compress and validate
    TestLoad_2 d(800);
    CHECK_PASS_ZSTD(ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_enableSeqProducerFallback, 0));
    size_t outLen = ZSTD_compress2(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    CHECK_PASS_ZSTD(outLen);
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize());
}

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_fail_common_2) // dummy custom sequence producer
{
    Test_ZSTD_registerSequenceProducer(getCtx(), sps, DummySequenceProducer); // register sequence producer that does nothing
    EXPECT_NE(getCtx()->requestedParams.extSeqProdFunc, nullptr);
    EXPECT_NE(getCtx()->requestedParams.extSeqProdState, nullptr);

    // compress
    TestLoad_2 d(800);
    CHECK_PASS_ZSTD(ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_enableSeqProducerFallback, 0));
    CHECK_FAIL_ZSTD(ZSTD_compress2(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize()));
}

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_pass_common_3) // dummy custom sequence producer with fallback
{
    Test_ZSTD_registerSequenceProducer(getCtx(), sps, DummySequenceProducer); // register sequence producer that does nothing
    EXPECT_NE(getCtx()->requestedParams.extSeqProdFunc, nullptr);
    EXPECT_NE(getCtx()->requestedParams.extSeqProdState, nullptr);

    // compress and validate
    TestLoad_2 d(800);
    CHECK_PASS_ZSTD(ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_enableSeqProducerFallback, 1)); // allow fallback to default producer
    size_t outLen = ZSTD_compress2(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    CHECK_PASS_ZSTD(outLen);
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize());
}

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_fail_common_4) // error custom sequence producer
{
    Test_ZSTD_registerSequenceProducer(getCtx(), sps, ErrorSequenceProducer); // register sequence producer that fails
    EXPECT_NE(getCtx()->requestedParams.extSeqProdFunc, nullptr);
    EXPECT_NE(getCtx()->requestedParams.extSeqProdState, nullptr);

    // compress
    TestLoad_2 d(800);
    CHECK_PASS_ZSTD(ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_enableSeqProducerFallback, 0));
    CHECK_FAIL_ZSTD(ZSTD_compress2(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize()));
}

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_pass_common_5) // error custom sequence producer with fallback
{
    Test_ZSTD_registerSequenceProducer(getCtx(), sps, ErrorSequenceProducer); // register sequence producer that does nothing
    EXPECT_NE(getCtx()->requestedParams.extSeqProdFunc, nullptr);
    EXPECT_NE(getCtx()->requestedParams.extSeqProdState, nullptr);

    // compress and validate
    TestLoad_2 d(800);
    CHECK_PASS_ZSTD(ZSTD_CCtx_setParameter(getCtx(), ZSTD_c_enableSeqProducerFallback, 1)); // allow fallback to default producer
    size_t outLen = ZSTD_compress2(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    CHECK_PASS_ZSTD(outLen);
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize());
}

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_fail_common_6) // cctx is null
{
    Test_ZSTD_registerSequenceProducer(NULL, sps, ValidSequenceProducer); // no feedback. testing for no crash.
}

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_fail_common_7) // sequence producer is null
{
    Test_ZSTD_registerSequenceProducer(getCtx(), sps, NULL); // no feedback. testing for no crash.
    EXPECT_EQ(getCtx()->requestedParams.extSeqProdFunc, nullptr);
    EXPECT_EQ(getCtx()->requestedParams.extSeqProdState, nullptr);
}

TEST_F(ZSTD_ZSTD_registerSequenceProducer, AOCL_Compression_zstd_ZSTD_registerSequenceProducer_fail_common_8) // sequence producer state is null
{
    Test_ZSTD_registerSequenceProducer(getCtx(), NULL, ValidSequenceProducer); // no feedback. testing for no crash.
    EXPECT_NE(getCtx()->requestedParams.extSeqProdFunc, nullptr);
    EXPECT_EQ(getCtx()->requestedParams.extSeqProdState, nullptr);
}
/*********************************************
 * End of ZSTD_ZSTD_registerSequenceProducer
 *********************************************/
