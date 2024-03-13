/**
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved. Portions of this file consist of AI-generated content.
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
 *  @brief Test cases for ZSTD APIs that use dictionary.
 *
 *  This file contains the test cases for ZSTD method
 *  testing the API level functions of ZSTD that use dictionary.
 *
 *  @author Ashish Sriram
 */
#include <functional>
#include "zstd_gtest.h"
#define ZDICT_STATIC_LINKING_ONLY
#include "algos/zstd/lib/zdict.h"
#include "gtest/gtest_utils.h"

ZSTD_CDict* Test_ZSTD_createCDict(const void* dictBuffer, size_t dictSize, int compressionLevel) {
    return ZSTD_createCDict(dictBuffer, dictSize, compressionLevel);
}

ZSTD_DDict* Test_ZSTD_createDDict(const void* dictBuffer, size_t dictSize) {
    return ZSTD_createDDict(dictBuffer, dictSize);
}

size_t Test_ZSTD_freeCDict(ZSTD_CDict* CDict) {
    return ZSTD_freeCDict(CDict);
}

size_t Test_ZSTD_freeDDict(ZSTD_DDict* DDict) {
    return ZSTD_freeDDict(DDict);
}

ZSTD_CDict* Test_ZSTD_createCDict_advanced(const void* dict, size_t dictSize, ZSTD_dictLoadMethod_e dictLoadMethod,
    ZSTD_dictContentType_e dictContentType, ZSTD_compressionParameters cParams, ZSTD_customMem customMem) {
    return ZSTD_createCDict_advanced(dict, dictSize, dictLoadMethod, dictContentType, cParams, customMem);
}

ZSTD_CDict* Test_ZSTD_createCDict_advanced2(const void* dict, size_t dictSize, ZSTD_dictLoadMethod_e dictLoadMethod,
    ZSTD_dictContentType_e dictContentType, const ZSTD_CCtx_params* cctxParams, ZSTD_customMem customMem) {
    return ZSTD_createCDict_advanced2(dict, dictSize, dictLoadMethod, dictContentType, cctxParams, customMem);
}

size_t Test_ZDICT_trainFromBuffer(void* dictBuffer, size_t dictBufferCapacity,
    const void* samplesBuffer, const size_t* samplesSizes, unsigned nbSamples) {
    return ZDICT_trainFromBuffer(dictBuffer, dictBufferCapacity, samplesBuffer, samplesSizes, nbSamples);
}

size_t Test_ZDICT_finalizeDictionary(void* dstDictBuffer, size_t maxDictSize, const void* dictContent, size_t dictContentSize,
    const void* samplesBuffer, const size_t* samplesSizes, unsigned nbSamples, ZDICT_params_t parameters) {
    return ZDICT_finalizeDictionary(dstDictBuffer, maxDictSize, dictContent, 
        dictContentSize, samplesBuffer, samplesSizes, nbSamples, parameters);
}

ZSTD_DDict* Test_ZSTD_createDDict_advanced(const void* dict, size_t dictSize,
    ZSTD_dictLoadMethod_e dictLoadMethod, ZSTD_dictContentType_e dictContentType, ZSTD_customMem customMem) {
    return ZSTD_createDDict_advanced(dict, dictSize, dictLoadMethod, dictContentType, customMem);
}

size_t Test_ZSTD_compress_usingCDict(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, const ZSTD_CDict* cdict) {
    return ZSTD_compress_usingCDict(cctx, dst, dstCapacity, src, srcSize, cdict);
}

size_t Test_ZSTD_compress_usingDict(ZSTD_CCtx* ctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, const void* dict,size_t dictSize, int compressionLevel) {
    return ZSTD_compress_usingDict(ctx, dst, dstCapacity, src, srcSize, dict, dictSize, compressionLevel);
}

size_t Test_ZSTD_decompress_usingDict(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, const void* dict, size_t dictSize) {
    return ZSTD_decompress_usingDict(dctx, dst, dstCapacity, src, srcSize, dict, dictSize);
}

/* This struct mimics ZSTD_CDict_s.
* As ZSTD_CDict_s members are inaccesible, we copy its contents to this struct
* to access/validate struct members */
struct Test_ZSTD_CDict_s {
    const void* dictContent;
    size_t dictContentSize;
    ZSTD_dictContentType_e dictContentType;
    U32* entropyWorkspace;
    ZSTD_cwksp workspace;
    ZSTD_matchState_t matchState;
    ZSTD_compressedBlockState_t cBlockState;
    ZSTD_customMem customMem;
    U32 dictID;
    int compressionLevel;
    ZSTD_paramSwitch_e useRowMatchFinder;
};

/* This struct mimics ZSTD_DDict_s.
* As ZSTD_DDict_s members are inaccesible, we copy its contents to this struct
* to access/validate struct members */
struct Test_ZSTD_DDict_s {
    void* dictBuffer;
    const void* dictContent;
    size_t dictSize;
    ZSTD_entropyDTables_t entropy;
    U32 dictID;
    U32 entropyPresent;
    ZSTD_customMem cMem;
};

bool zstd_check_uncompressed_equal_to_original_dict(const char* src, size_t srcSize,
    const char* compressed, size_t compressedLen, const void* dict, size_t dictSize)
{
    char* uncompressed = (char*)calloc(srcSize, sizeof(char));


    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    size_t uncompressedLenRes = Test_ZSTD_decompress_usingDict(dctx, uncompressed, srcSize,
        compressed, compressedLen, dict, dictSize);

    if (Test_ZSTD_isError(uncompressedLenRes)) {
        free(uncompressed);
        ZSTD_freeDCtx(dctx);
        return false;
    }

    if (!(srcSize == uncompressedLenRes)) {
        free(uncompressed);
        ZSTD_freeDCtx(dctx);
        return false;
    }

    bool ret = (memcmp(src, uncompressed, srcSize) == 0);
    free(uncompressed);
    ZSTD_freeDCtx(dctx);
    return ret;
}

/***********************************************
 * Begin of ZSTD_ZSTD_createCDict
 ***********************************************/
class ZSTD_ZSTD_createCDict : public AOCL_setup_zstd {
public:
    ZSTD_ZSTD_createCDict(){
        dictSize = 300;
        dictBuffer = malloc(dictSize);
        for (int i = 0; i < dictSize; i++)
            ((BYTE*)dictBuffer)[i] = rand() % 256;
    }

    ~ZSTD_ZSTD_createCDict() {
        if (dictBuffer)
            free(dictBuffer);
        if (cdict)
            ZSTD_freeCDict(cdict);
    }

    void validate_cdict(int level, size_t sz) {
        ASSERT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));

        EXPECT_EQ(tcdict.compressionLevel, level);
        EXPECT_EQ(tcdict.dictContentSize, sz);

        if (dictBuffer != NULL && sz > 0) {
            EXPECT_EQ(memcmp(tcdict.dictContent, dictBuffer, sz), 0);
        }
    }

    Test_ZSTD_CDict_s get_cdict() {
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));
        return tcdict;
    }

    ZSTD_CDict* cdict = NULL;
    void* dictBuffer = NULL;
    size_t dictSize;
};

TEST_F(ZSTD_ZSTD_createCDict, AOCL_Compression_zstd_ZSTD_createCDict_pass_common_1) //valid dict created
{
    for (int level = 1; level <= 22; ++level) {
        cdict = Test_ZSTD_createCDict(dictBuffer, dictSize, level);
        validate_cdict(level, dictSize);
        if (cdict) {
            ZSTD_freeCDict(cdict);
            cdict = NULL;
        }
    }
}

TEST_F(ZSTD_ZSTD_createCDict, AOCL_Compression_zstd_ZSTD_createCDict_fail_common_2) //dictBuffer is null
{
    void* dictBufferNull = NULL;
    cdict = Test_ZSTD_createCDict(dictBufferNull, dictSize, ZSTD_CLEVEL_DEFAULT);
    ASSERT_NE(cdict, nullptr);
    Test_ZSTD_CDict_s tcdict = get_cdict();
    EXPECT_EQ(tcdict.dictContent, nullptr);
}

TEST_F(ZSTD_ZSTD_createCDict, AOCL_Compression_zstd_ZSTD_createCDict_fail_common_3) //dictSize is 0
{
    cdict = Test_ZSTD_createCDict(dictBuffer, 0, ZSTD_CLEVEL_DEFAULT);
    validate_cdict(ZSTD_CLEVEL_DEFAULT, 0);
}

TEST_F(ZSTD_ZSTD_createCDict, AOCL_Compression_zstd_ZSTD_createCDict_pass_common_4) //level = 0
{
    cdict = Test_ZSTD_createCDict(dictBuffer, dictSize, 0);
    validate_cdict(ZSTD_CLEVEL_DEFAULT, dictSize); // level expected to get set to default
}

TEST_F(ZSTD_ZSTD_createCDict, AOCL_Compression_zstd_ZSTD_createCDict_pass_common_5) //level min
{
    int level = Test_ZSTD_minCLevel();
    cdict = Test_ZSTD_createCDict(dictBuffer, dictSize, level);
    validate_cdict(level, dictSize);
}

TEST_F(ZSTD_ZSTD_createCDict, AOCL_Compression_zstd_ZSTD_createCDict_pass_common_6) //level > max
{
    int level = Test_ZSTD_maxCLevel() + 1;
    cdict = Test_ZSTD_createCDict(dictBuffer, dictSize, level);
    validate_cdict(level, dictSize);
}
/***********************************************
 * End of ZSTD_ZSTD_createCDict
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_freeCDict
 ***********************************************/
TEST(ZSTD_ZSTD_freeCDict, AOCL_Compression_zstd_ZSTD_freeCDict_pass_common_1) //dict is null
{
    ZSTD_CDict* cdict = NULL;
    size_t ret = Test_ZSTD_freeCDict(cdict);
    EXPECT_EQ(ret, 0);
}
//pass cases covered in ZSTD_ZSTD_createCDict
/***********************************************
 * End of ZSTD_ZSTD_freeCDict
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_createDDict
 ***********************************************/
class ZSTD_ZSTD_createDDict : public AOCL_setup_zstd {
public:
    ZSTD_ZSTD_createDDict() {
        dictSize = 300;
        dictBuffer = malloc(dictSize);
        for (int i = 0; i < dictSize; i++)
            ((BYTE*)dictBuffer)[i] = rand() % 256;
    }

    ~ZSTD_ZSTD_createDDict() {
        if (dictBuffer)
            free(dictBuffer);
        if (ddict)
            ZSTD_freeDDict(ddict);
    }

    void validate_ddict(int sz) {
        ASSERT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));

        EXPECT_EQ(tddict.dictSize, sz);

        if (dictBuffer != nullptr && sz > 0) {
            EXPECT_EQ(memcmp(tddict.dictContent, dictBuffer, sz), 0);
        }
        else
        {
            EXPECT_EQ(tddict.dictContent, dictBuffer);
            EXPECT_EQ(tddict.dictBuffer, nullptr);
        }
    }

    Test_ZSTD_DDict_s get_ddict() {
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));
        return tddict;
    }

    ZSTD_DDict* ddict;
    void* dictBuffer;
    size_t dictSize;
};

TEST_F(ZSTD_ZSTD_createDDict, AOCL_Compression_zstd_ZSTD_createDDict_pass_common_1) //valid dict created
{
    ddict = Test_ZSTD_createDDict(dictBuffer, dictSize);
    validate_ddict(dictSize);
}

TEST_F(ZSTD_ZSTD_createDDict, AOCL_Compression_zstd_ZSTD_createDDict_fail_common_2) //dictBuffer is null
{
    if (dictBuffer) free(dictBuffer);
    dictBuffer = NULL;
    ddict = Test_ZSTD_createDDict(dictBuffer, dictSize);
    validate_ddict(0);
}

TEST_F(ZSTD_ZSTD_createDDict, AOCL_Compression_zstd_ZSTD_createDDict_fail_common_3) //dictSize is 0
{
    ddict = Test_ZSTD_createDDict(dictBuffer, 0);
    validate_ddict(0);
}
/***********************************************
 * End of ZSTD_ZSTD_createDDict
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_freeDDict
 ***********************************************/
TEST(ZSTD_ZSTD_freeDDict, AOCL_Compression_zstd_ZSTD_freeDDict_pass_common_1) //dict is null
{
    ZSTD_DDict* ddict = NULL;
    size_t ret = Test_ZSTD_freeDDict(ddict);
    EXPECT_EQ(ret, 0);
}
//pass cases covered in ZSTD_ZSTD_createDDict
/***********************************************
 * End of ZSTD_ZSTD_freeDDict
 ***********************************************/

class ZSTD_dict_creator : public AOCL_setup_zstd {
public:
    virtual ~ZSTD_dict_creator() {
        if (dictBuffer)
            free(dictBuffer);
    }

    bool create_raw_content_dict(size_t sz) {
        dictSize = sz;
        if (dictBuffer) free(dictBuffer);
        dictBuffer = malloc(sz);
        gtest_data_gen_t::fill_repeated((char*)dictBuffer, dictSize);
        return true;
    }

    bool create_train_from_buffer_dict(size_t sz) {
        dictSize = sz;
        if (dictBuffer) free(dictBuffer);
        dictBuffer = malloc(sz);
        size_t samplesBufferSz = sz * 2; // 100x recommended
        std::vector<char> samplesBuffer(samplesBufferSz, 0);
        std::vector<size_t> samplesSizes = gtest_data_gen_t::get_array_of_samples(samplesBuffer.data(), samplesBufferSz);
        dictSize = Test_ZDICT_trainFromBuffer(dictBuffer, dictSize, samplesBuffer.data(), samplesSizes.data(), samplesSizes.size());
        return !Test_ZSTD_isError(dictSize);
    }

    bool create_finalize_dict(size_t sz, int level) {
        dictSize = sz;
        if (dictBuffer) free(dictBuffer);
        dictBuffer = malloc(sz);
        size_t samplesBufferSz = sz * 2;
        std::vector<char> samplesBuffer(samplesBufferSz, 0);
        std::vector<size_t> samplesSizes = gtest_data_gen_t::get_array_of_samples(samplesBuffer.data(), samplesBufferSz);
        size_t dictContentSize = sz / 2;
        void* dictContent = (char*)dictBuffer + dictContentSize;
        gtest_data_gen_t::fill_repeated((char*)dictContent, dictContentSize);
        ZDICT_params_t params{ level, 0, 0 };

        dictSize = Test_ZDICT_finalizeDictionary(dictBuffer, dictSize, dictContent, dictContentSize,
            samplesBuffer.data(), samplesSizes.data(), samplesSizes.size(), params);
        return !Test_ZSTD_isError(dictSize);
    }

    void* getDictBuffer() {
        return dictBuffer;
    }

    size_t getDictSize() {
        return dictSize;
    }

private:
    void* dictBuffer = NULL;
    size_t dictSize = 0;
};

using gtest_create_dict_t = std::function<bool(size_t)>;

class ZSTD_ZSTD_createCDict_base : public ZSTD_dict_creator {
public:
    ZSTD_ZSTD_createCDict_base() {
    }

    virtual ~ZSTD_ZSTD_createCDict_base() {
        if (cdict)
            ZSTD_freeCDict(cdict);
    }

    void test_dict_creation(ZSTD_Compress_API api, gtest_create_dict_t create_dict_ptr, ZSTD_dictContentType_e type) {
        EXPECT_TRUE(create_dict_ptr(800)); // create dict using custom creator
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        ZSTD_dictLoadMethod_e validDlm[2] = { ZSTD_dlm_byCopy, ZSTD_dlm_byRef };
        for (int i = 0; i < 2; ++i) { // create and validate
            cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), validDlm[i], type, cparams, ZSTD_defaultCMem);
            validate_cdict(type);
            if (cdict) {
                ZSTD_freeCDict(cdict);
                cdict = NULL;
            }
        }
    }
    
    void create_cdict_dict_is_null(ZSTD_Compress_API api) { // dict is null
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        cdict = run_create_cdict(api, NULL, getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);
        
        ASSERT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));
        EXPECT_EQ(tcdict.dictContent, nullptr);
    }

    void create_cdict_dict_is_null_dictsize_0(ZSTD_Compress_API api) { // dict is null and dictSize is 0
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        cdict = run_create_cdict(api, NULL, 0, ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);
        
        ASSERT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));
        EXPECT_EQ(tcdict.dictContent, nullptr);
        EXPECT_EQ(tcdict.dictContentSize, 0);
    }

    void create_cdict_dictsize_0(ZSTD_Compress_API api) { // dictSize is 0
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        cdict = run_create_cdict(api, getDictBuffer(), 0, ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);
        
        ASSERT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));
        EXPECT_EQ(tcdict.dictContentSize, 0);
    }

    void create_cdict_dictLoadMethod_invalid(ZSTD_Compress_API api) { // dictLoadMethod invalid
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), (ZSTD_dictLoadMethod_e)100, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);
        validate_cdict(ZSTD_dct_auto);
    }

    void create_cdict_dictContentType_invalid(ZSTD_Compress_API api) { // dictContentType invalid
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, (ZSTD_dictContentType_e)100, cparams, ZSTD_defaultCMem);
        EXPECT_EQ(cdict, nullptr);
    }

    void create_cdict_cparams_empty(ZSTD_Compress_API api) { // cparams empty
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams;
        memset(&cparams, 0, sizeof(ZSTD_compressionParameters));
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);
        validate_cdict(ZSTD_dct_auto);
    }

    void create_cdict_custom_memory_allocator_pass(ZSTD_Compress_API api) { // custom memory allocator pass
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, Test_ZSTD_custom_free_pass, NULL };
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, cMem);
        validate_cdict(ZSTD_dct_auto);
    }

    void create_cdict_custom_memory_allocator_no_free(ZSTD_Compress_API api) { // custom memory allocator no free
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, NULL, NULL };
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, cMem);
        EXPECT_EQ(cdict, nullptr);
    }

    void create_cdict_custom_memory_allocator_no_alloc(ZSTD_Compress_API api) { // custom memory allocator no alloc
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        ZSTD_customMem cMem = { NULL, Test_ZSTD_custom_free_pass, NULL };
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, cMem);
        EXPECT_EQ(cdict, nullptr);
    }

    void create_cdict_custom_memory_allocator_fail(ZSTD_Compress_API api) { // custom memory allocator fail
        EXPECT_TRUE(create_raw_content_dict(800));
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_fail, Test_ZSTD_custom_free_fail, NULL };
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, cMem);
        EXPECT_EQ(cdict, nullptr);
    }

    void create_cdict_no_dict_created(ZSTD_Compress_API api) { // no dict created
        //no dict created
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(300, ZSTD_CLEVEL_DEFAULT, 1);
        cdict = run_create_cdict(api, getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);

        ASSERT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));
        EXPECT_EQ(tcdict.dictContent, nullptr);
        EXPECT_EQ(tcdict.dictContentSize, 0);
    }

protected:
    ZSTD_CDict* run_create_cdict(ZSTD_Compress_API api, const void* dict, size_t dictSize, ZSTD_dictLoadMethod_e dictLoadMethod,
        ZSTD_dictContentType_e dictContentType, ZSTD_compressionParameters cParams, ZSTD_customMem customMem) {
        switch(api){
            case ZSTD_Compress_API::compress_cdict:
                return Test_ZSTD_createCDict_advanced(dict, dictSize, dictLoadMethod, dictContentType, cParams, customMem);
            case ZSTD_Compress_API::compress_cdict2:
                ZSTD_CCtx_params cctxParams;
                ZSTD_memset(&cctxParams, 0, sizeof(cctxParams));
                Test_ZSTD_CCtxParams_init(&cctxParams, 0);
                cctxParams.cParams = cParams;
                cctxParams.customMem = customMem;
                return Test_ZSTD_createCDict_advanced2(dict, dictSize, dictLoadMethod, dictContentType, &cctxParams, customMem);
            default:
                return NULL;
        }
    }

private:
    void validate_cdict(ZSTD_dictContentType_e type) {
        ASSERT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));

        EXPECT_EQ(tcdict.dictContentSize, getDictSize());
        EXPECT_EQ(memcmp(tcdict.dictContent, getDictBuffer(), getDictSize()), 0);
        EXPECT_EQ(tcdict.dictContentType, type);
    }

    ZSTD_CDict* cdict = NULL;
};

/***********************************************
 * Begin of ZSTD_ZSTD_createCDict_advanced
 ***********************************************/
class ZSTD_ZSTD_createCDict_advanced : public ZSTD_ZSTD_createCDict_base {};

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_1) // valid params raw content dict
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict, [this](size_t sz) { return create_raw_content_dict(sz); }, ZSTD_dct_rawContent);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_2) // valid params full dict
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict, [this](size_t sz) { return create_train_from_buffer_dict(sz); }, ZSTD_dct_fullDict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_3) // valid params auto on raw
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict, [this](size_t sz) { return create_raw_content_dict(sz); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_4) // valid params auto on full
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict, [this](size_t sz) { return create_train_from_buffer_dict(sz); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_5) // valid params auto on finalize
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict, [this](size_t sz) { return create_finalize_dict(sz, ZSTD_CLEVEL_DEFAULT); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_6)
{
    create_cdict_dict_is_null(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_7)
{
    create_cdict_dict_is_null_dictsize_0(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_8)
{
    create_cdict_dictsize_0(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_9)
{
    create_cdict_dictLoadMethod_invalid(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_fail_common_10)
{
    create_cdict_dictContentType_invalid(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_11)
{
    create_cdict_cparams_empty(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_12)
{
    create_cdict_custom_memory_allocator_pass(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_fail_common_13)
{
    create_cdict_custom_memory_allocator_no_free(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_fail_common_14)
{
    create_cdict_custom_memory_allocator_no_alloc(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_fail_common_15)
{
    create_cdict_custom_memory_allocator_fail(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_pass_common_16)
{
    create_cdict_no_dict_created(ZSTD_Compress_API::compress_cdict);
}
/***********************************************
 * End of ZSTD_ZSTD_createCDict_advanced
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_createCDict_advanced2
 ***********************************************/
class ZSTD_ZSTD_createCDict_advanced2 : public ZSTD_ZSTD_createCDict_base {};

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_1) // valid params raw content dict
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict2, [this](size_t sz) { return create_raw_content_dict(sz); }, ZSTD_dct_rawContent);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_2) // valid params full dict
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict2, [this](size_t sz) { return create_train_from_buffer_dict(sz); }, ZSTD_dct_fullDict);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_3) // valid params auto on raw
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict2, [this](size_t sz) { return create_raw_content_dict(sz); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_4) // valid params auto on full
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict2, [this](size_t sz) { return create_train_from_buffer_dict(sz); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_5) // valid params auto on finalize
{
    test_dict_creation(ZSTD_Compress_API::compress_cdict2, [this](size_t sz) { return create_finalize_dict(sz, ZSTD_CLEVEL_DEFAULT); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_6)
{
    create_cdict_dict_is_null(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_7)
{
    create_cdict_dict_is_null_dictsize_0(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_8)
{
    create_cdict_dictsize_0(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_9)
{
    create_cdict_dictLoadMethod_invalid(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_fail_common_10)
{
    create_cdict_dictContentType_invalid(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_11)
{
    create_cdict_cparams_empty(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_12)
{
    create_cdict_custom_memory_allocator_pass(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_fail_common_13)
{
    create_cdict_custom_memory_allocator_no_free(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_fail_common_14)
{
    create_cdict_custom_memory_allocator_no_alloc(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_fail_common_15)
{
    create_cdict_custom_memory_allocator_fail(ZSTD_Compress_API::compress_cdict2);
}

TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_pass_common_16)
{
    create_cdict_no_dict_created(ZSTD_Compress_API::compress_cdict2);
}
/***********************************************
 * End of ZSTD_ZSTD_createCDict_advanced2
 ***********************************************/

class ZSTD_ZSTD_createDDict_base : public ZSTD_dict_creator {
public:
    ZSTD_ZSTD_createDDict_base() {
    }

    virtual ~ZSTD_ZSTD_createDDict_base() {
        if (ddict)
            ZSTD_freeDDict(ddict);
    }

    void test_dict_creation(gtest_create_dict_t create_dict_ptr, ZSTD_dictContentType_e type) {
        EXPECT_TRUE(create_dict_ptr(800)); // create dict using custom creator
        ZSTD_dictLoadMethod_e validDlm[2] = { ZSTD_dlm_byCopy, ZSTD_dlm_byRef };
        for (int i = 0; i < 2; ++i) { // create and validate
            ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), validDlm[i], type, ZSTD_defaultCMem);
            validate_ddict();
            if (ddict) {
                ZSTD_freeDDict(ddict);
                ddict = NULL;
            }
        }
    }

    void create_ddict_dict_is_null() // dict is null
    {
        ddict = Test_ZSTD_createDDict_advanced(NULL, getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, ZSTD_defaultCMem);
        ASSERT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));
        EXPECT_EQ(tddict.dictContent, nullptr);
    }

    void create_ddict_dict_is_null_dictsize_0() // dict is null and dictSize is 0
    {
        ddict = Test_ZSTD_createDDict_advanced(NULL, 0, ZSTD_dlm_byCopy, ZSTD_dct_auto, ZSTD_defaultCMem);
        ASSERT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));
        EXPECT_EQ(tddict.dictContent, nullptr);
        EXPECT_EQ(tddict.dictSize, 0);
    }

    void create_ddict_dictsize_0() // dictSize is 0
    {
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), 0, ZSTD_dlm_byCopy, ZSTD_dct_auto, ZSTD_defaultCMem);
        ASSERT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));
        EXPECT_EQ(tddict.dictSize, 0);
    }

    void create_ddict_dictLoadMethod_invalid() // dictLoadMethod invalid
    {
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), (ZSTD_dictLoadMethod_e)100, ZSTD_dct_auto, ZSTD_defaultCMem);
        validate_ddict();
    }

    void create_ddict_dictContentType_invalid() // dictContentType invalid
    {
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, (ZSTD_dictContentType_e)100, ZSTD_defaultCMem);
        validate_ddict();
    }

    void create_ddict_custom_memory_allocator_pass() // custom memory allocator pass
    {
        ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, Test_ZSTD_custom_free_pass, NULL };
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cMem);
        validate_ddict();
    }

    void create_ddict_custom_memory_allocator_no_free() // custom memory allocator no free
    {
        ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_pass, NULL, NULL };
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cMem);
        EXPECT_EQ(ddict, nullptr);
    }

    void create_ddict_custom_memory_allocator_no_alloc() // custom memory allocator no alloc
    {
        ZSTD_customMem cMem = { NULL, Test_ZSTD_custom_free_pass, NULL };
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cMem);
        EXPECT_EQ(ddict, nullptr);
    }

    void create_ddict_custom_memory_allocator_fail() // custom memory allocator fail
    {
        ZSTD_customMem cMem = { Test_ZSTD_custom_alloc_fail, Test_ZSTD_custom_free_fail, NULL };
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, cMem);
        EXPECT_EQ(ddict, nullptr);
    }

    void create_ddict_no_dict_created() // no dict created
    {
        ddict = Test_ZSTD_createDDict_advanced(getDictBuffer(), getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto, ZSTD_defaultCMem);
        ASSERT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));
        EXPECT_EQ(tddict.dictContent, nullptr);
        EXPECT_EQ(tddict.dictSize, 0);
    }

private:
    void validate_ddict() {
        ASSERT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));

        EXPECT_EQ(tddict.dictSize, getDictSize());
        EXPECT_EQ(memcmp(tddict.dictContent, getDictBuffer(), getDictSize()), 0);
    }

    ZSTD_DDict* ddict = NULL;
};

/***********************************************
 * Begin of ZSTD_ZSTD_createDDict_advanced
 ***********************************************/
class ZSTD_ZSTD_createDDict_advanced : public ZSTD_ZSTD_createDDict_base {};

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_1) // valid params raw content dict
{
    test_dict_creation([this](size_t sz) { return create_raw_content_dict(sz); }, ZSTD_dct_rawContent);
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_2) // valid params full dict
{
    test_dict_creation([this](size_t sz) { return create_train_from_buffer_dict(sz); }, ZSTD_dct_fullDict);
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_3) // valid params auto on raw
{
    test_dict_creation([this](size_t sz) { return create_raw_content_dict(sz); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_4) // valid params auto on full
{
    test_dict_creation([this](size_t sz) { return create_train_from_buffer_dict(sz); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_5) // valid params auto on finalize
{
    test_dict_creation([this](size_t sz) { return create_finalize_dict(sz, ZSTD_CLEVEL_DEFAULT); }, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_6)
{
    create_ddict_dict_is_null();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_7)
{
    create_ddict_dict_is_null_dictsize_0();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_8)
{
    create_ddict_dictsize_0();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_9)
{
    create_ddict_dictLoadMethod_invalid();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_10)
{
    create_ddict_dictContentType_invalid();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_11)
{
    create_ddict_custom_memory_allocator_pass();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_fail_common_12)
{
    create_ddict_custom_memory_allocator_no_free();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_fail_common_13)
{
    create_ddict_custom_memory_allocator_no_alloc();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_fail_common_14)
{
    create_ddict_custom_memory_allocator_fail();
}

TEST_F(ZSTD_ZSTD_createDDict_advanced, AOCL_Compression_zstd_ZSTD_createDDict_advanced_pass_common_15)
{
    create_ddict_no_dict_created();
}
/***********************************************
 * End of ZSTD_ZSTD_createDDict_advanced
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_compress_usingCDict
 ***********************************************/
class ZSTD_ZSTD_compress_usingDict_base : public ZSTD_ZSTD_createCDict_base {
public:
    ZSTD_ZSTD_compress_usingDict_base()
    {
        cctx = Test_ZSTD_createCCtx();
    }

    ~ZSTD_ZSTD_compress_usingDict_base()
    {
        Test_ZSTD_freeCDict(cdict);
        Test_ZSTD_freeCCtx(cctx);
    }

    ZSTD_CCtx* getCtx() {
        return cctx;
    }

    ZSTD_CDict* getCDict() {
        return cdict;
    }

    void validate_compress_format(char* compressed, unsigned compressedLen, unsigned dstCapacity) {
        EXPECT_LE(compressedLen, dstCapacity);
        EXPECT_TRUE(is_valid_zstd_frame(compressed, compressedLen));
        EXPECT_TRUE(has_valid_frame_content_size(compressed, compressedLen));
    }

    void validate_compress(char* src, unsigned srcSize, char* compressed, unsigned compressedLen, unsigned dstCapacity,
        const void* dict, size_t dictSize) {
        validate_compress_format(compressed, compressedLen, dstCapacity);
        EXPECT_TRUE(zstd_check_uncompressed_equal_to_original_dict(src, srcSize, compressed, compressedLen, dict, dictSize));
    }

    void validate_compress_no_dict(char* src, unsigned srcSize, char* compressed, unsigned compressedLen, unsigned dstCapacity) {
        validate_compress_format(compressed, compressedLen, dstCapacity);
        EXPECT_TRUE(zstd_check_uncompressed_equal_to_original_dict(src, srcSize, compressed, compressedLen, NULL, 0)); /* no dict passed */
    }

    /* This test is used to ensure the test case is properly designed, so that the compressed stream must have
    *  a dependency on the dictionary. If it is not the case, zstd_check_uncompressed_equal_to_original_dict()
    *  will pass as there is no dependency on the dict. It will expose cases where tests pass silently because
    * there is no dict dependency in the compressed stream */
    void counter_validate_compress_no_dict(char* src, unsigned srcSize, char* compressed, unsigned compressedLen, unsigned dstCapacity) {
        EXPECT_FALSE(zstd_check_uncompressed_equal_to_original_dict(src, srcSize, compressed, compressedLen, NULL, 0));
    }

    void compress_all_levels(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress all supported levels
        create_dict_raw(8000);
        for (int cLevel = 0; cLevel <= 22; cLevel++) {
            TestLoad_2 d(8000, gtest_data_gen_type::repeated);
            size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
            validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), getDictBuffer(), getDictSize());
        }
    }

    void compress_src_null(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress src null
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), NULL, d.getOrigSize(), cdict);
        CHECK_FAIL_ZSTD(outLen);
        EXPECT_EQ(outLen, ERROR(srcSize_wrong));
    }

    void compress_dst_null(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress dst null
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        size_t outLen = run_compress(api, cctx, cLevel, NULL, d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
        CHECK_FAIL_ZSTD(outLen);
        EXPECT_EQ(outLen, ERROR(dstSize_tooSmall));
    }

    void compress_insufficient_dstCapacity(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress dstCapacity < ZSTD_compressBound(srcSize)
        TestLoad_2 d(800, 100, gtest_data_gen_type::repeated); // insufficient dst
        create_dict_raw(d.getOrigSize());
        size_t dstCapacity = d.getCompressedSize();
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), dstCapacity, d.getOrigData(), d.getOrigSize(), cdict);
        CHECK_FAIL_ZSTD(outLen);
        EXPECT_EQ(outLen, ERROR(dstSize_tooSmall));
    }

    void compress_srcsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress src size 0
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), 0, cdict);
        //src size = 0 is a valid input and is expected to return a valid zstd frame 
        CHECK_PASS_ZSTD(outLen);
        validate_compress_format(d.getCompressedBuff(), outLen, d.getCompressedSize());
    }

    void compress_src_null_srcsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress src null and src size 0
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), NULL, 0, cdict);
        //src size = 0 is a valid input and is expected to return a valid zstd frame 
        CHECK_PASS_ZSTD(outLen);
        validate_compress_format(d.getCompressedBuff(), outLen, d.getCompressedSize());
    }

    void compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress src null and src size 0 and dst size 0
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), 0, NULL, 0, cdict);
        CHECK_FAIL_ZSTD(outLen);
        EXPECT_EQ(outLen, ERROR(dstSize_tooSmall));
    }

    void compress_level_lt_min(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress level < 0
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        int cLevel = Test_ZSTD_minCLevel();
        // For levels < 1, compression parameters are set to 0th entry of the table `ZSTD_defaultCParameters[4][ZSTD_MAX_CLEVEL+1]`
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
        validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), getDictBuffer(), getDictSize());
    }

    void compress_level_gt_max(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress level > maximum limit
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        int cLevel = Test_ZSTD_maxCLevel() + 1;
        // For level > maximum possible level, level will be set to ZSTD_MAX_CLEVEL, which is 22.
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
        validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), getDictBuffer(), getDictSize());
    }

    void compress_cctx_null(ZSTD_Compress_API api) { // compress cctx null
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        size_t outLen = run_compress(api, NULL, ZSTD_CLEVEL_DEFAULT, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
        EXPECT_EQ(outLen, ERROR(GENERIC));
    }

    void compress_cdict_null(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress cdict null
        // no cdict created
        TestLoad_2 d(800, gtest_data_gen_type::repeated);
        size_t outLen = run_compress(api, cctx, ZSTD_CLEVEL_DEFAULT, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
        EXPECT_EQ(outLen, ERROR(dictionary_wrong));
    }

    void compress_train_from_buffer_dict(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress train_from_buffer_dict
        TestLoad_2 d(8000, gtest_data_gen_type::repeated);
        create_dict_formatted(d.getOrigSize());
        size_t outLen = run_compress(api, cctx, ZSTD_CLEVEL_DEFAULT, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
        validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), getDictBuffer(), getDictSize());
    }

    void compress_finalize_dict(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress finalize_dict
        TestLoad_2 d(8000, gtest_data_gen_type::repeated);
        create_dict_finalized(d.getOrigSize());
        size_t outLen = run_compress(api, cctx, ZSTD_CLEVEL_DEFAULT, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), cdict);
        validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), getDictBuffer(), getDictSize());
    }

    void create_dict_raw(size_t srcSize) {
        EXPECT_TRUE(create_raw_content_dict(800));
        create_dict(srcSize);
    }

    void create_dict_formatted(size_t srcSize) {
        EXPECT_TRUE(create_train_from_buffer_dict(8000));
        create_dict(srcSize);
    }

    void create_dict_finalized(size_t srcSize) {
        EXPECT_TRUE(create_finalize_dict(8000, ZSTD_CLEVEL_DEFAULT));
        create_dict(srcSize);
    }

protected:
    size_t run_compress(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel,
        void* dst, size_t dstCapacity, const void* src, size_t srcSize, ZSTD_CDict* cdict) {
        switch (api) {
        case ZSTD_Compress_API::compress_cdict:
        {
            return Test_ZSTD_compress_usingCDict(cctx, dst, dstCapacity, src, srcSize, cdict);
        }
        case ZSTD_Compress_API::compress_dict:
        {
            return Test_ZSTD_compress_usingDict(cctx, dst, dstCapacity, src, srcSize, getDictBuffer(), getDictSize(), cLevel);
        }
        case ZSTD_Compress_API::compress2: // expect dict to be set within cctx prior to call
        {
            ZSTD_parameters zparams;
            zparams = Test_ZSTD_getParams(cLevel, srcSize, 0);
            zparams.fParams.contentSizeFlag = 1;
            if (cctx) {
                CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParams(cctx, zparams));
            }
            return Test_ZSTD_compress2(cctx, dst, dstCapacity, src, srcSize);
        }
        default:
            return ERROR(GENERIC);
        }
    }

private:
    void create_dict(size_t srcSize) {
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(srcSize, ZSTD_CLEVEL_DEFAULT, 1);
        cdict = run_create_cdict(ZSTD_Compress_API::compress_cdict, getDictBuffer(), getDictSize(),
            ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);
        ASSERT_NE(cdict, nullptr);
    }

    ZSTD_CDict* cdict = NULL;
    ZSTD_CCtx* cctx = NULL;
};

class ZSTD_ZSTD_compress_usingCDict : public ZSTD_ZSTD_compress_usingDict_base {};

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_pass_common_1)
{
    compress_all_levels(ZSTD_Compress_API::compress_cdict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_fail_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress_cdict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_fail_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress_cdict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_fail_common_4)
{
    compress_insufficient_dstCapacity(ZSTD_Compress_API::compress_cdict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_cdict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_cdict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_fail_common_7)
{
    compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API::compress_cdict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_pass_common_8)
{
    compress_level_lt_min(ZSTD_Compress_API::compress_cdict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_pass_common_9)
{
    compress_level_gt_max(ZSTD_Compress_API::compress_cdict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_fail_common_10)
{
    compress_cctx_null(ZSTD_Compress_API::compress_cdict);
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_fail_common_11)
{
    compress_cdict_null(ZSTD_Compress_API::compress_cdict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_pass_common_12)
{
    compress_train_from_buffer_dict(ZSTD_Compress_API::compress_cdict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingCDict, AOCL_Compression_zstd_ZSTD_compress_usingCDict_pass_common_13)
{
    compress_finalize_dict(ZSTD_Compress_API::compress_cdict, getCtx());
}
/***********************************************
 * End of ZSTD_ZSTD_compress_usingCDict
 ***********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_compress_usingDict
 ***********************************************/
class ZSTD_ZSTD_compress_usingDict : public ZSTD_ZSTD_compress_usingDict_base {};

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_1)
{
    compress_all_levels(ZSTD_Compress_API::compress_dict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_fail_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress_dict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_fail_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress_dict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_fail_common_4)
{
    compress_insufficient_dstCapacity(ZSTD_Compress_API::compress_dict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress_dict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress_dict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_fail_common_7)
{
    compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API::compress_dict, getCtx(), ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_8)
{
    compress_level_lt_min(ZSTD_Compress_API::compress_dict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_9)
{
    compress_level_gt_max(ZSTD_Compress_API::compress_dict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_fail_common_10)
{
    compress_cctx_null(ZSTD_Compress_API::compress_dict);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_11)
{
    compress_train_from_buffer_dict(ZSTD_Compress_API::compress_dict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_12)
{
    compress_finalize_dict(ZSTD_Compress_API::compress_dict, getCtx());
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_13) // dict is null
{
    TestLoad_2 d(800, gtest_data_gen_type::repeated);
    size_t outLen = Test_ZSTD_compress_usingDict(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, getDictSize(), ZSTD_CLEVEL_DEFAULT);
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), NULL, getDictSize());
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_14) // dictSize is 0
{
    TestLoad_2 d(800, gtest_data_gen_type::repeated);
    size_t outLen = Test_ZSTD_compress_usingDict(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), getDictBuffer(), 0, ZSTD_CLEVEL_DEFAULT);
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), getDictBuffer(), 0);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_15) // dict is null and dictSize is 0
{
    TestLoad_2 d(800, gtest_data_gen_type::repeated);
    size_t outLen = Test_ZSTD_compress_usingDict(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL, 0, ZSTD_CLEVEL_DEFAULT);
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(), NULL, 0);
}

TEST_F(ZSTD_ZSTD_compress_usingDict, AOCL_Compression_zstd_ZSTD_compress_usingDict_pass_common_16) // dictSize < 8. dict should be ignored.
{
    TestLoad_2 d(800, gtest_data_gen_type::repeated);
    size_t outLen = Test_ZSTD_compress_usingDict(getCtx(), d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), getDictBuffer(), 7, ZSTD_CLEVEL_DEFAULT);
    validate_compress_no_dict(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize()); // expect no dependence on dict
}
/***********************************************
 * End of ZSTD_ZSTD_compress_usingDict
 ***********************************************/
