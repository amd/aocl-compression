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

size_t Test_ZSTD_decompress_usingDDict(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, const ZSTD_DDict* ddict) {
    return ZSTD_decompress_usingDDict(dctx, dst, dstCapacity, src, srcSize, ddict);
}

size_t Test_ZSTD_decompressBegin_usingDict(ZSTD_DCtx* dctx, const void* dict, size_t dictSize) {
    return ZSTD_decompressBegin_usingDict(dctx, dict, dictSize);
}

unsigned Test_ZSTD_getDictID_fromDict(const void* dict, size_t dictSize) {
    return ZSTD_getDictID_fromDict(dict, dictSize);
}

unsigned Test_ZSTD_getDictID_fromCDict(const ZSTD_CDict* cdict) {
    return ZSTD_getDictID_fromCDict(cdict);
}

unsigned Test_ZSTD_getDictID_fromDDict(const ZSTD_DDict* ddict) {
    return ZSTD_getDictID_fromDDict(ddict);
}

unsigned Test_ZSTD_getDictID_fromFrame(const void* src, size_t srcSize) {
    return ZSTD_getDictID_fromFrame(src, srcSize);
}

size_t Test_ZSTD_sizeof_CDict(const ZSTD_CDict* cdict) {
    return ZSTD_sizeof_CDict(cdict);
}

size_t Test_ZSTD_sizeof_DDict(const ZSTD_DDict* ddict) {
    return ZSTD_sizeof_DDict(ddict);
}

size_t Test_ZSTD_estimateCDictSize(size_t dictSize, int compressionLevel) {
    return ZSTD_estimateCDictSize(dictSize, compressionLevel);
}

size_t Test_ZSTD_estimateCDictSize_advanced(
    size_t dictSize, ZSTD_compressionParameters cParams,
    ZSTD_dictLoadMethod_e dictLoadMethod) {
    return ZSTD_estimateCDictSize_advanced(dictSize, cParams, dictLoadMethod);
}

size_t Test_ZSTD_estimateDDictSize(size_t dictSize, ZSTD_dictLoadMethod_e dictLoadMethod) {
    return ZSTD_estimateDDictSize(dictSize, dictLoadMethod);
}

const ZSTD_CDict* Test_ZSTD_initStaticCDict(void* workspace, size_t workspaceSize,
    const void* dict, size_t dictSize, ZSTD_dictLoadMethod_e dictLoadMethod,
    ZSTD_dictContentType_e dictContentType, ZSTD_compressionParameters cParams) {
    return ZSTD_initStaticCDict(workspace, workspaceSize, dict, dictSize, dictLoadMethod, dictContentType, cParams);
}

const ZSTD_DDict* Test_ZSTD_initStaticDDict(void* workspace, size_t workspaceSize,
    const void* dict, size_t dictSize, ZSTD_dictLoadMethod_e dictLoadMethod,
    ZSTD_dictContentType_e dictContentType) {
    return ZSTD_initStaticDDict(workspace, workspaceSize, dict, dictSize, dictLoadMethod, dictContentType);
}

size_t Test_ZSTD_CCtx_loadDictionary(ZSTD_CCtx* cctx, const void* dict, size_t dictSize) {
    return ZSTD_CCtx_loadDictionary(cctx, dict, dictSize);
}

size_t Test_ZSTD_CCtx_loadDictionary_advanced(ZSTD_CCtx* cctx, const void* dict, size_t dictSize,
    ZSTD_dictLoadMethod_e dictLoadMethod, ZSTD_dictContentType_e dictContentType) {
    return ZSTD_CCtx_loadDictionary_advanced(cctx, dict, dictSize, dictLoadMethod, dictContentType);
}

size_t Test_ZSTD_CCtx_loadDictionary_byReference(ZSTD_CCtx* cctx, const void* dict, size_t dictSize) {
    return ZSTD_CCtx_loadDictionary_byReference(cctx, dict, dictSize);
}

size_t Test_ZSTD_CCtx_refCDict(ZSTD_CCtx* cctx, const ZSTD_CDict* cdict) {
    return ZSTD_CCtx_refCDict(cctx, cdict);
}

size_t Test_ZSTD_CCtx_refPrefix(ZSTD_CCtx* cctx, const void* prefix, size_t prefixSize) {
    return ZSTD_CCtx_refPrefix(cctx, prefix, prefixSize);
}

size_t Test_ZSTD_CCtx_refPrefix_advanced(ZSTD_CCtx* cctx, const void* prefix, size_t prefixSize,
    ZSTD_dictContentType_e dictContentType) {
    return ZSTD_CCtx_refPrefix_advanced(cctx, prefix, prefixSize, dictContentType);
}

size_t Test_ZSTD_DCtx_loadDictionary(ZSTD_DCtx* dctx, const void* dict, size_t dictSize) {
    return ZSTD_DCtx_loadDictionary(dctx, dict, dictSize);
}

size_t Test_ZSTD_DCtx_loadDictionary_byReference(ZSTD_DCtx* dctx, const void* dict, size_t dictSize) {
    return ZSTD_DCtx_loadDictionary_byReference(dctx, dict, dictSize);
}

size_t Test_ZSTD_DCtx_refDDict(ZSTD_DCtx* dctx, const ZSTD_DDict* ddict) {
    return ZSTD_DCtx_refDDict(dctx, ddict);
}

size_t Test_ZSTD_DCtx_refPrefix(ZSTD_DCtx* dctx, const void* prefix, size_t prefixSize) {
    return ZSTD_DCtx_refPrefix(dctx, prefix, prefixSize);
}

size_t Test_ZSTD_DCtx_refPrefix_advanced(ZSTD_DCtx* dctx, const void* prefix, size_t prefixSize,
    ZSTD_dictContentType_e dictContentType) {
    return ZSTD_DCtx_refPrefix_advanced(dctx, prefix, prefixSize, dictContentType);
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
    ZSTD_MatchState_t matchState;
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

#ifndef DEBUG_ASSERT_ENABLED
/* Assertions get triggered:
ZSTD_compress_insertDictionary -> 
    RETURN_ERROR_IF(dictContentType == ZSTD_dct_fullDict, dictionary_wrong, "");
    assert(0);  // impossible */
TEST_F(ZSTD_ZSTD_createCDict_advanced, AOCL_Compression_zstd_ZSTD_createCDict_advanced_fail_common_10)
{
    create_cdict_dictContentType_invalid(ZSTD_Compress_API::compress_cdict);
}
#endif

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

#ifndef DEBUG_ASSERT_ENABLED
/* Assertions get triggered:
ZSTD_compress_insertDictionary -> 
    RETURN_ERROR_IF(dictContentType == ZSTD_dct_fullDict, dictionary_wrong, "");
    assert(0);  // impossible */
TEST_F(ZSTD_ZSTD_createCDict_advanced2, AOCL_Compression_zstd_ZSTD_createCDict_advanced2_fail_common_10)
{
    create_cdict_dictContentType_invalid(ZSTD_Compress_API::compress_cdict2);
}
#endif

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
        EXPECT_TRUE(has_valid_frames(compressed, compressedLen));
        EXPECT_TRUE(has_valid_frame_content_size(compressed, compressedLen) || has_unknown_frame_content_size(compressed, compressedLen));
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

/***********************************************
* Begin of ZSTD_ZSTD_CCtx_loadDictionary
***********************************************/
class ZSTD_CCtx_loadDictionary_base : public ZSTD_ZSTD_compress_usingDict_base {

public:
    void load_dict_pass(ZSTD_loadDict_type type, ZSTD_dictLoadMethod_e dictLoadMethod = ZSTD_dlm_byCopy,
        ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // pass and validate compress:decompress using dict
        TestLoad_2 d(8000, gtest_data_gen_type::repeated);
        create_dict_raw(d.getOrigSize());
        EXPECT_EQ(run_load_dict(type, getCtx(), getDictBuffer(), getDictSize(), dictLoadMethod, dictContentType), 0); // load dict into cctx

        // compress using dict within cctx and validate
        size_t outLen = run_compress(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT,
            d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL);
        validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(),
            getDictBuffer(), getDictSize());
        counter_validate_compress_no_dict(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(),
            outLen, d.getCompressedSize()); // ensure test doesn't pass silently due to no dict dependency
    }

    void load_dict_pass_formatted(ZSTD_loadDict_type type, ZSTD_dictLoadMethod_e dictLoadMethod = ZSTD_dlm_byCopy,
        ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // pass and validate compress:decompress using formatted dict
        TestLoad_2 d(8000, gtest_data_gen_type::repeated);
        create_dict_formatted(d.getOrigSize());
        EXPECT_EQ(run_load_dict(type, getCtx(), getDictBuffer(), getDictSize(), dictLoadMethod, dictContentType), 0); // load dict into cctx

        // compress using dict within cctx and validate
        size_t outLen = run_compress(ZSTD_Compress_API::compress2, getCtx(), ZSTD_CLEVEL_DEFAULT,
            d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize(), NULL);
        validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize(),
            getDictBuffer(), getDictSize());
        counter_validate_compress_no_dict(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(),
            outLen, d.getCompressedSize()); // ensure test doesn't pass silently due to no dict dependency
    }

    void load_dict_is_null(ZSTD_loadDict_type type, ZSTD_dictLoadMethod_e dictLoadMethod = ZSTD_dlm_byCopy,
        ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dict is null
        create_dict_raw(100);
        EXPECT_EQ(run_load_dict(type, getCtx(), NULL, getDictSize(), dictLoadMethod, dictContentType), 0); // equivalent to dict reset
    }

    void load_dict_dictsize_0(ZSTD_loadDict_type type, ZSTD_dictLoadMethod_e dictLoadMethod = ZSTD_dlm_byCopy,
        ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dictSize is 0
        create_dict_raw(100);
        EXPECT_EQ(run_load_dict(type, getCtx(), getDictBuffer(), 0, dictLoadMethod, dictContentType), 0); // equivalent to dict reset
    }

    void load_dict_is_null_dictsize_0(ZSTD_loadDict_type type, ZSTD_dictLoadMethod_e dictLoadMethod = ZSTD_dlm_byCopy,
        ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dict is null and dictSize is 0
        EXPECT_EQ(run_load_dict(type, getCtx(), NULL, 0, dictLoadMethod, dictContentType), 0); // equivalent to dict reset
    }

    void load_dict_cctx_null(ZSTD_loadDict_type type, ZSTD_dictLoadMethod_e dictLoadMethod = ZSTD_dlm_byCopy,
        ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // cctx null
        create_dict_raw(100);
        EXPECT_EQ(run_load_dict(type, NULL, getDictBuffer(), getDictSize(), dictLoadMethod, dictContentType),
            ERROR(GENERIC));
    }

    void load_dict_cctx_streamStage_not_init(ZSTD_loadDict_type type, ZSTD_dictLoadMethod_e dictLoadMethod = ZSTD_dlm_byCopy,
        ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // cctx streamStage not init
        create_dict_raw(100);
        ZSTD_CCtx* cctx_not_init = getCtx();
        cctx_not_init->streamStage = zcss_flush; // modify streamStage
        EXPECT_EQ(run_load_dict(type, cctx_not_init, getDictBuffer(), getDictSize(), dictLoadMethod, dictContentType),
            ERROR(stage_wrong));
    }

private:
    size_t run_load_dict(ZSTD_loadDict_type type, ZSTD_CCtx* cctx, const void* dict, size_t dictSize,
        ZSTD_dictLoadMethod_e dictLoadMethod, ZSTD_dictContentType_e dictContentType) {
        switch (type) {
        case ZSTD_loadDict_type::byValue:
            return Test_ZSTD_CCtx_loadDictionary(cctx, dict, dictSize);
        case ZSTD_loadDict_type::byReference:
            return Test_ZSTD_CCtx_loadDictionary_byReference(cctx, dict, dictSize);
        case ZSTD_loadDict_type::advanced:
            return Test_ZSTD_CCtx_loadDictionary_advanced(cctx, dict, dictSize, dictLoadMethod, dictContentType);
        case ZSTD_loadDict_type::refcdict:
        {
            if (dict == NULL || dictSize == 0) {
                ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(UINT_MAX, ZSTD_CLEVEL_DEFAULT, 1);
                ZSTD_CDict* cdict = run_create_cdict(ZSTD_Compress_API::compress_cdict, dict, dictSize,
                    ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams, ZSTD_defaultCMem);
                size_t ret = Test_ZSTD_CCtx_refCDict(cctx, cdict);
                Test_ZSTD_freeCDict(cdict);
                return ret;
            }
            else
                return Test_ZSTD_CCtx_refCDict(cctx, getCDict());
        }
        case ZSTD_loadDict_type::refprefix:
            return Test_ZSTD_CCtx_refPrefix(cctx, dict, dictSize);
        case ZSTD_loadDict_type::refprefixAdv:
            return Test_ZSTD_CCtx_refPrefix_advanced(cctx, dict, dictSize, dictContentType);
        default:
            return ERROR(GENERIC);
        }
    }
};


class ZSTD_CCtx_loadDictionary : public ZSTD_CCtx_loadDictionary_base {};

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_pass_common_2)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_pass_common_3)
{
    load_dict_is_null(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_pass_common_4)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_pass_common_5)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_fail_common_6)
{
    load_dict_cctx_null(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_fail_common_7)
{
    load_dict_cctx_streamStage_not_init(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_CCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_fail_common_8) // cctx static
{
    create_dict_raw(100);

    // create static cctx
    size_t const workspaceSize = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    void* workspace = malloc(workspaceSize);
    ZSTD_CCtx* cctx_static = Test_ZSTD_initStaticCCtx(workspace, workspaceSize);
    ASSERT_NE(cctx_static, nullptr);

    EXPECT_EQ(Test_ZSTD_CCtx_loadDictionary(cctx_static, getDictBuffer(), getDictSize()), ERROR(memory_allocation));
    free(workspace);
}
/***********************************************
 * End of ZSTD_ZSTD_CCtx_loadDictionary
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_CCtx_loadDictionary_byReference
  ***********************************************/
class ZSTD_CCtx_loadDictionary_byReference : public ZSTD_CCtx_loadDictionary_base {};

TEST_F(ZSTD_CCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_byReference_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_CCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_byReference_pass_common_2)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_CCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_byReference_pass_common_3)
{
    load_dict_is_null(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_CCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_byReference_pass_common_4)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_CCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_byReference_pass_common_5)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_CCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_byReference_fail_common_6)
{
    load_dict_cctx_null(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_CCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_byReference_fail_common_7)
{
    load_dict_cctx_streamStage_not_init(ZSTD_loadDict_type::byReference);
}
/***********************************************
 * End of ZSTD_ZSTD_CCtx_loadDictionary_byReference
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_CCtx_loadDictionary_advanced
  ***********************************************/
class ZSTD_CCtx_loadDictionary_advanced : public ZSTD_CCtx_loadDictionary_base {};

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::advanced, ZSTD_dlm_byCopy, ZSTD_dct_auto);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_pass_common_2)
{
    load_dict_pass(ZSTD_loadDict_type::advanced, ZSTD_dlm_byRef, ZSTD_dct_auto);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_pass_common_3)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::advanced, ZSTD_dlm_byCopy, ZSTD_dct_auto);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_pass_common_4)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::advanced, ZSTD_dlm_byRef, ZSTD_dct_auto);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_pass_common_5)
{
    load_dict_is_null(ZSTD_loadDict_type::advanced);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_pass_common_6)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::advanced);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_pass_common_7)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::advanced);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_fail_common_8)
{
    load_dict_cctx_null(ZSTD_loadDict_type::advanced);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_fail_common_9)
{
    load_dict_cctx_streamStage_not_init(ZSTD_loadDict_type::advanced);
}

TEST_F(ZSTD_CCtx_loadDictionary_advanced, AOCL_Compression_zstd_ZSTD_CCtx_loadDictionary_advanced_fail_common_10) // cctx static with ZSTD_dlm_byCopy
{
    create_dict_raw(100);

    // create static cctx
    size_t const workspaceSize = Test_ZSTD_estimateCCtxSize(ZSTD_CLEVEL_DEFAULT);
    void* workspace = malloc(workspaceSize);
    ZSTD_CCtx* cctx_static = Test_ZSTD_initStaticCCtx(workspace, workspaceSize);
    ASSERT_NE(cctx_static, nullptr);

    EXPECT_EQ(Test_ZSTD_CCtx_loadDictionary_advanced(cctx_static, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byCopy, ZSTD_dct_auto), ERROR(memory_allocation));
    free(workspace);
}
/***********************************************
 * End of ZSTD_ZSTD_CCtx_loadDictionary_advanced
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_CCtx_refCDict
  ***********************************************/
class ZSTD_CCtx_refCDict : public ZSTD_CCtx_loadDictionary_base {};

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_pass_common_2)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_pass_common_3)
{
    load_dict_is_null(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_pass_common_4)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_pass_common_5)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_fail_common_6) // cdict is NULL
{
    EXPECT_EQ(Test_ZSTD_CCtx_refCDict(getCtx(), NULL), 0);
}

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_fail_common_7)
{
    load_dict_cctx_null(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_CCtx_refCDict, AOCL_Compression_zstd_ZSTD_CCtx_refCDict_fail_common_8)
{
    load_dict_cctx_streamStage_not_init(ZSTD_loadDict_type::refcdict);
}
/***********************************************
 * End of ZSTD_ZSTD_CCtx_refCDict
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_CCtx_refPrefix
  ***********************************************/
class ZSTD_CCtx_refPrefix : public ZSTD_CCtx_loadDictionary_base {};

TEST_F(ZSTD_CCtx_refPrefix, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_CCtx_refPrefix, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_pass_common_2)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_CCtx_refPrefix, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_pass_common_3)
{
    load_dict_is_null(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_CCtx_refPrefix, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_pass_common_4)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_CCtx_refPrefix, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_pass_common_5)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_CCtx_refPrefix, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_fail_common_6)
{
    load_dict_cctx_null(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_CCtx_refPrefix, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_fail_common_7)
{
    load_dict_cctx_streamStage_not_init(ZSTD_loadDict_type::refprefix);
}
/***********************************************
 * End of ZSTD_ZSTD_CCtx_refPrefix
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_CCtx_refPrefix_advanced
  ***********************************************/
class ZSTD_CCtx_refPrefix_advanced : public ZSTD_CCtx_loadDictionary_base {};

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::refprefixAdv, (ZSTD_dictLoadMethod_e)100, ZSTD_dct_auto);
}

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_pass_common_2)
{
    load_dict_pass(ZSTD_loadDict_type::refprefixAdv, (ZSTD_dictLoadMethod_e)100, ZSTD_dct_rawContent);
}

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_pass_common_3)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::refprefixAdv, (ZSTD_dictLoadMethod_e)100, ZSTD_dct_fullDict);
}

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_pass_common_4)
{
    load_dict_is_null(ZSTD_loadDict_type::refprefixAdv);
}

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_pass_common_5)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::refprefixAdv);
}

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_pass_common_6)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::refprefixAdv);
}

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_fail_common_7)
{
    load_dict_cctx_null(ZSTD_loadDict_type::refprefixAdv);
}

TEST_F(ZSTD_CCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_CCtx_refPrefix_advanced_fail_common_8)
{
    load_dict_cctx_streamStage_not_init(ZSTD_loadDict_type::refprefixAdv);
}
/***********************************************
 * End of ZSTD_ZSTD_CCtx_refPrefix_advanced
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_decompress_usingDDict
  ***********************************************/
class ZSTD_decompress_usingDict_base : public ZSTD_dict_creator {
public:
    ZSTD_decompress_usingDict_base() {
        cctx = Test_ZSTD_createCCtx();
        dctx = Test_ZSTD_createDCtx();
    }

    virtual ~ZSTD_decompress_usingDict_base() {
        if (output)
            free(output);
        delete d;

        Test_ZSTD_freeCDict(cdict);
        Test_ZSTD_freeCCtx(cctx);
        Test_ZSTD_freeDDict(ddict);
        Test_ZSTD_freeDCtx(dctx);
    }

    void validate_decompress(const char* original, unsigned origLen, const char* output, unsigned outputLen) {
        EXPECT_EQ(origLen, outputLen);
        EXPECT_EQ(0, memcmp(output, original, origLen));
    }

    void decompress_pass(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // pass
        compress_repeated(800);
        size_t decLen = run_decompress(api, dctx, output, outLen, d->getCompressedBuff(), compressedLen, ddict);
        CHECK_PASS_ZSTD(decLen);
        validate_decompress(d->getOrigData(), d->getOrigSize(), output, decLen);
    }

    void decompress_src_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src null
        compress_repeated(800);
        size_t decLen = run_decompress(api, dctx, output, outLen, NULL, compressedLen, ddict);
        CHECK_FAIL_ZSTD(decLen);
        EXPECT_EQ(decLen, ERROR(srcSize_wrong));
    }

    void decompress_dst_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress dst null
        compress_repeated(800);
        size_t decLen = run_decompress(api, dctx, NULL, outLen, d->getCompressedBuff(), compressedLen, ddict);
        CHECK_FAIL_ZSTD(decLen);
        EXPECT_EQ(decLen, ERROR(dstSize_tooSmall)); //FIXME: input type being repeated or random change the error code!!!
    }

    void decompress_insufficient_dstCapacity(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress dstCapacity insufficient
        compress_repeated(800);
        size_t dstCapacity = outLen - 1; // insufficient dst
        size_t decLen = run_decompress(api, dctx, output, dstCapacity, d->getCompressedBuff(), compressedLen, ddict);
        CHECK_FAIL_ZSTD(decLen);
        EXPECT_EQ(decLen, ERROR(dstSize_tooSmall));
    }

    void decompress_srcsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src size 0
        compress_repeated(800);
        size_t decLen = run_decompress(api, dctx, output, outLen, d->getCompressedBuff(), 0, ddict);
        CHECK_PASS_ZSTD(decLen);
        EXPECT_EQ(decLen, 0);
    }

    void decompress_src_null_srcsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src null and src size 0
        compress_repeated(800);
        size_t decLen = run_decompress(api, dctx, output, outLen, NULL, 0, ddict);
        CHECK_PASS_ZSTD(decLen);
        EXPECT_EQ(decLen, 0);
    }

    void decompress_src_null_srcsize_0_dstsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src null and src size 0 and dst size 0
        compress_repeated(800);
        size_t decLen = run_decompress(api, dctx, output, 0, NULL, 0, ddict);
        CHECK_PASS_ZSTD(decLen);
        EXPECT_EQ(decLen, 0);
    }

    void decompress_dctx_null(ZSTD_Decompress_API api) { // decompress dctx null
        compress_repeated(800);
        size_t decLen = run_decompress(api, NULL, output, outLen, d->getCompressedBuff(), compressedLen, ddict);
        EXPECT_EQ(decLen, ERROR(GENERIC));
    }

    void decompress_ddict_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress ddict null
        // create cdict
        EXPECT_TRUE(create_raw_content_dict(900));
        cdict = Test_ZSTD_createCDict(getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
        ASSERT_NE(cdict, nullptr);

        // compress using cdict
        d = new TestLoad_2(800);
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);
        compressedLen = Test_ZSTD_compress_usingDict(cctx, d->getCompressedBuff(), d->getCompressedSize(),
            d->getOrigData(), d->getOrigSize(), getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
        CHECK_PASS_ZSTD(compressedLen);

        // run without ddict
        /* output of run_decompress() is dependent on nature of input.
        * If the input stream has matches in dictionary, then passing NULL here will result in
        * the API returning data_corruption error.
        * If no such matches are found, then dictionary is not required and it will pass and
        * provide valid decompressed output.
        * Hence only checking for no crash here and not validating the returned result. */
        run_decompress(api, dctx, output, outLen, d->getCompressedBuff(), compressedLen, NULL);
    }

    void decompress_cdict_ddict_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress ddict null, no dict used to compress
        // compress without dict
        d = new TestLoad_2(800);
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);
        compressedLen = Test_ZSTD_compress2(cctx, d->getCompressedBuff(), d->getCompressedSize(),
            d->getOrigData(), d->getOrigSize());
        CHECK_PASS_ZSTD(compressedLen);

        // run without ddict
        size_t decLen = run_decompress(api, dctx, output, outLen, d->getCompressedBuff(), compressedLen, NULL);
        CHECK_PASS_ZSTD(decLen);
        validate_decompress(d->getOrigData(), d->getOrigSize(), output, decLen);
    }

    ZSTD_DCtx* getDCtx() {
        return dctx;
    }

    ZSTD_DDict* getDDict() {
        return ddict;
    }

protected:
    void compress_repeated(size_t sz) { // compress using a raw content dictionary
        create_dict_raw();

        d = new TestLoad_2(sz, gtest_data_gen_type::repeated);
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);

        compressedLen = Test_ZSTD_compress_usingDict(cctx, d->getCompressedBuff(), d->getCompressedSize(),
            d->getOrigData(), d->getOrigSize(), getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
        CHECK_PASS_ZSTD(compressedLen);
    }

    void compress_repeated_formatted(size_t sz) { // compress using a format compilant dictionary
        create_dict_formatted();

        d = new TestLoad_2(sz, gtest_data_gen_type::repeated);
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);

        compressedLen = Test_ZSTD_compress_usingDict(cctx, d->getCompressedBuff(), d->getCompressedSize(),
            d->getOrigData(), d->getOrigSize(), getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
        CHECK_PASS_ZSTD(compressedLen);
    }

    void compress_no_dict(size_t sz) { // compress without dict
        d = new TestLoad_2(sz);
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);

        compressedLen = Test_ZSTD_compress2(cctx, d->getCompressedBuff(), d->getCompressedSize(),
            d->getOrigData(), d->getOrigSize());
        CHECK_PASS_ZSTD(compressedLen);
    }


    char* get_output_buf() {
        return output;
    }

    size_t get_output_capacity() {
        return outLen;
    }

    char* get_orig_data()
    {
        return d->getOrigData();
    }

    size_t get_orig_size()
    {
        return d->getOrigSize();
    }

    char* get_compressed_buff()
    {
        return d->getCompressedBuff();
    }

    size_t get_compressed_capacity()
    {
        return d->getCompressedSize();
    }

    size_t get_compressed_len() {
        return compressedLen;
    }

protected:
    size_t run_decompress(ZSTD_Decompress_API api, ZSTD_DCtx* dctx, void* dst, size_t dstCapacity,
        const void* src, size_t srcSize, ZSTD_DDict* ddict) {
        switch (api) {
        case ZSTD_Decompress_API::decompress_ddict:
        {
            return Test_ZSTD_decompress_usingDDict(dctx, dst, dstCapacity, src, srcSize, ddict);
        }
        case ZSTD_Decompress_API::decompress_dict:
        {
            return Test_ZSTD_decompress_usingDict(dctx, dst, dstCapacity, src, srcSize, getDictBuffer(), getDictSize());
        }
        case ZSTD_Decompress_API::decompress_dctx:
        {
            return Test_ZSTD_decompressDCtx(dctx, dst, dstCapacity, src, srcSize);
        }
        default:
            return ERROR(GENERIC);
        }
    }

    size_t set_fds_runtime_params(size_t disableFdsFrame) {
        return Test_ZSTD_CCtx_setFdsRuntimeParams(cctx, disableFdsFrame);
    }

private:
    void create_dict_raw() {
        EXPECT_TRUE(create_raw_content_dict(900));
        cdict = Test_ZSTD_createCDict(getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
        ddict = Test_ZSTD_createDDict(getDictBuffer(), getDictSize()); // created using same dictBuffer as cdict
        ASSERT_NE(cdict, nullptr);
        ASSERT_NE(ddict, nullptr);
    }

    void create_dict_formatted() {
        EXPECT_TRUE(create_train_from_buffer_dict(900));
        cdict = Test_ZSTD_createCDict(getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
        ddict = Test_ZSTD_createDDict(getDictBuffer(), getDictSize()); // created using same dictBuffer as cdict
        ASSERT_NE(cdict, nullptr);
        ASSERT_NE(ddict, nullptr);
    }

    // Decompressed data will be stored in the buffer `output`.
    char* output = NULL;
    size_t outLen;
    size_t compressedLen;
    TestLoad_2* d = NULL;

    ZSTD_CDict* cdict = NULL;
    ZSTD_CCtx* cctx = NULL;
    ZSTD_DDict* ddict = NULL;
    ZSTD_DCtx* dctx = NULL;

};

class ZSTD_ZSTD_decompress_usingDDict : public ZSTD_decompress_usingDict_base {};

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_pass_common_1)
{
    decompress_pass(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_fail_common_2)
{
    decompress_src_null(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_fail_common_3)
{
    decompress_dst_null(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_fail_common_4)
{
    decompress_insufficient_dstCapacity(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_pass_common_5)
{
    decompress_srcsize_0(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_pass_common_6)
{
    decompress_src_null_srcsize_0(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_pass_common_7)
{
    decompress_src_null_srcsize_0_dstsize_0(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_fail_common_8)
{
    decompress_dctx_null(ZSTD_Decompress_API::decompress_ddict);
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_pass_common_9)
{
    decompress_ddict_null(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDDict, AOCL_Compression_zstd_ZSTD_decompress_usingDDict_pass_common_10)
{
    decompress_cdict_ddict_null(ZSTD_Decompress_API::decompress_ddict, getDCtx());
}
/***********************************************
 * End of ZSTD_ZSTD_decompress_usingDDict
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_decompress_usingDict
  ***********************************************/
class ZSTD_ZSTD_decompress_usingDict : public ZSTD_decompress_usingDict_base {};

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_pass_common_1)
{
    decompress_pass(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_fail_common_2)
{
    decompress_src_null(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_fail_common_3)
{
    decompress_dst_null(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_fail_common_4)
{
    decompress_insufficient_dstCapacity(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_pass_common_5)
{
    decompress_srcsize_0(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_pass_common_6)
{
    decompress_src_null_srcsize_0(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_pass_common_7)
{
    decompress_src_null_srcsize_0_dstsize_0(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_fail_common_8)
{
    decompress_dctx_null(ZSTD_Decompress_API::decompress_dict);
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_pass_common_9)
{
    decompress_ddict_null(ZSTD_Decompress_API::decompress_dict, getDCtx());
}

TEST_F(ZSTD_ZSTD_decompress_usingDict, AOCL_Compression_zstd_ZSTD_decompress_usingDict_pass_common_10)
{
    decompress_cdict_ddict_null(ZSTD_Decompress_API::decompress_dict, getDCtx());
}
/***********************************************
 * End of ZSTD_ZSTD_decompress_usingDict
 ***********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_decompressBegin_usingDict
  *********************************************/
class ZSTD_ZSTD_decompressBegin_usingDict : public ZSTD_dict_creator {};
TEST_F(ZSTD_ZSTD_decompressBegin_usingDict, AOCL_Compression_zstd_ZSTD_decompressBegin_usingDict_fail_common_1) // dctx is null
{
    EXPECT_TRUE(create_raw_content_dict(800));
    size_t ret = Test_ZSTD_decompressBegin_usingDict(NULL, getDictBuffer(), getDictSize());
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_decompressBegin_usingDict, AOCL_Compression_zstd_ZSTD_decompressBegin_usingDict_pass_common_2) // dict is null
{
    EXPECT_TRUE(create_raw_content_dict(800));
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx();
    size_t ret = Test_ZSTD_decompressBegin_usingDict(dctx, NULL, getDictSize());
    EXPECT_EQ(ret, 0);
    Test_ZSTD_freeDCtx(dctx);
}

TEST_F(ZSTD_ZSTD_decompressBegin_usingDict, AOCL_Compression_zstd_ZSTD_decompressBegin_usingDict_pass_common_3) // dictSize is 0
{
    EXPECT_TRUE(create_raw_content_dict(800));
    ZSTD_DCtx* dctx = Test_ZSTD_createDCtx();
    size_t ret = Test_ZSTD_decompressBegin_usingDict(dctx, getDictBuffer(), 0);
    EXPECT_EQ(ret, 0);
    Test_ZSTD_freeDCtx(dctx);
}
// other cases tested as part of ZSTD_ZSTD_decompressContinue_usingDict
/*********************************************
 * End of ZSTD_ZSTD_decompressBegin_usingDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_decompressContinue_usingDict
  *********************************************/
class ZSTD_ZSTD_decompressContinue_usingDict : public ZSTD_decompress_usingDict_base {
public:
    ZSTD_ZSTD_decompressContinue_usingDict()
    {
        dctx = ZSTD_createDCtx();
    }

    ~ZSTD_ZSTD_decompressContinue_usingDict()
    {
        if (dctx)
            ZSTD_freeDCtx(dctx);
    }

    void decompress_pass() { // pass
        compress_repeated(800);
        size_t decLen = buffer_less_streaming_pass();
        CHECK_PASS_ZSTD(decLen);
        validate_decompress(get_orig_data(), get_orig_size(), get_output_buf(), decLen);
    }

    void decompress_src_null() { // decompress src null
        compress_repeated(800);
        CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin_usingDict(dctx, getDictBuffer(), getDictSize()));
        size_t const srcSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
        size_t const dprSize = Test_ZSTD_decompressContinue(dctx, get_output_buf(), get_output_capacity(), NULL, srcSize);
        EXPECT_EQ(dprSize, ERROR(srcSize_wrong));
    }

    void decompress_dst_null() { // decompress dst null
        compress_repeated(800);
        CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin_usingDict(dctx, getDictBuffer(), getDictSize()));
        size_t const srcSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
        size_t const dprSize = Test_ZSTD_decompressContinue(dctx, NULL, get_output_capacity(), get_orig_data(), srcSize);
        EXPECT_EQ(dprSize, ERROR(dstBuffer_null));
    }

    void decompress_dstCapacity_insufficient() { // decompress dstCapacity insufficient
        compress_repeated(800);
        size_t curCprLen = 0;
        size_t dstCapacity = get_output_capacity() / 10;
        size_t curDprLen = buffer_less_streaming_multiple(curCprLen, get_output_buf(), dstCapacity,
            get_compressed_buff(), get_compressed_len());
        EXPECT_EQ(curDprLen, ERROR(dstSize_tooSmall));
    }

    void decompress_srcSize_0() { // decompress srcSize is 0
        compress_repeated(800);
        CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin_usingDict(dctx, getDictBuffer(), getDictSize()));
        size_t const dprSize = Test_ZSTD_decompressContinue(dctx, get_output_buf(), get_output_capacity(), get_orig_data(), 0);
        EXPECT_EQ(dprSize, ERROR(srcSize_wrong));
    }

    void decompress_src_null_srcSize_0() { // decompress src null and srcSize is 0
        compress_repeated(800);
        CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin_usingDict(dctx, getDictBuffer(), getDictSize()));
        size_t const dprSize = Test_ZSTD_decompressContinue(dctx, get_output_buf(), get_output_capacity(), NULL, 0);
        EXPECT_EQ(dprSize, ERROR(srcSize_wrong));
    }

private:
    size_t buffer_less_streaming_multiple(size_t& curCprLen,
        void* dst, size_t dstCapacity, const void* src, size_t srcSize) {
        curCprLen = 0;
        size_t curDprLen = 0;
        size_t cprSize = 0;
        while (curCprLen < srcSize) {
            CHECK_PASS_ZSTD(Test_ZSTD_decompressBegin_usingDict(dctx, getDictBuffer(), getDictSize())); // new frame
            cprSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
            while (cprSize) {
                size_t const dprSize = Test_ZSTD_decompressContinue(dctx, (char*)dst + curDprLen,
                    dstCapacity - curDprLen, (char*)src + curCprLen, cprSize);
                if (ZSTD_isError(dprSize)) return dprSize;
                curDprLen += dprSize;
                curCprLen += cprSize;
                cprSize = Test_ZSTD_nextSrcSizeToDecompress(dctx);
            }
        }
        return curDprLen;
    }

    size_t buffer_less_streaming_pass() {
        size_t curCprLen = 0;
         size_t curDprLen = buffer_less_streaming_multiple(curCprLen, get_output_buf(), get_output_capacity(),
                                       get_compressed_buff(), get_compressed_len());
        EXPECT_FALSE(ZSTD_isError(curDprLen));
        EXPECT_EQ(Test_ZSTD_nextSrcSizeToDecompress(dctx), 0); // frame fully decoded
        EXPECT_EQ(curCprLen, get_compressed_len()); // compressed data fully read
        return curDprLen;
    }

    ZSTD_DCtx* dctx = NULL;
};

TEST_F(ZSTD_ZSTD_decompressContinue_usingDict, AOCL_Compression_zstd_ZSTD_decompressContinue_pass_common_1)
{
    decompress_pass();
}

TEST_F(ZSTD_ZSTD_decompressContinue_usingDict, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_2)
{
    decompress_src_null();
}

TEST_F(ZSTD_ZSTD_decompressContinue_usingDict, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_3)
{
    decompress_dst_null();
}

TEST_F(ZSTD_ZSTD_decompressContinue_usingDict, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_4)
{
    decompress_dstCapacity_insufficient();
}

TEST_F(ZSTD_ZSTD_decompressContinue_usingDict, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_5)
{
    decompress_srcSize_0();
}

TEST_F(ZSTD_ZSTD_decompressContinue_usingDict, AOCL_Compression_zstd_ZSTD_decompressContinue_fail_common_6)
{
    decompress_src_null_srcSize_0();
}
/*********************************************
 * End of ZSTD_ZSTD_decompressContinue_usingDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_DCtx_loadDictionary
  ***********************************************/
class ZSTD_ZSTD_DCtx_loadDictionary_base : public ZSTD_decompress_usingDict_base {
public:
    void load_dict_pass(ZSTD_loadDict_type type, ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // pass and validate compress & decompress using dict
        compress_repeated(8000);
        EXPECT_EQ(run_load_dict(type, getDCtx(), getDictBuffer(), getDictSize(), dictContentType), 0); // load dict into cctx

        // compress using dict within dctx and validate
        size_t decLen = run_decompress(ZSTD_Decompress_API::decompress_dctx, getDCtx(), get_output_buf(),
            get_output_capacity(), get_compressed_buff(), get_compressed_len(), NULL);
        CHECK_PASS_ZSTD(decLen);
        validate_decompress(get_orig_data(), get_orig_size(), get_output_buf(), decLen);
    }

    void load_dict_pass_formatted(ZSTD_loadDict_type type, ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // pass and validate compress & decompress using formatted dict
        compress_repeated_formatted(8000);
        EXPECT_EQ(run_load_dict(type, getDCtx(), getDictBuffer(), getDictSize(), dictContentType), 0); // load dict into cctx

        // compress using dict within dctx and validate
        size_t decLen = run_decompress(ZSTD_Decompress_API::decompress_dctx, getDCtx(), get_output_buf(),
            get_output_capacity(), get_compressed_buff(), get_compressed_len(), NULL);
        CHECK_PASS_ZSTD(decLen);
        validate_decompress(get_orig_data(), get_orig_size(), get_output_buf(), decLen);
    }

    void load_dict_is_null(ZSTD_loadDict_type type, ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dict is null
        compress_repeated(100);
        EXPECT_EQ(run_load_dict(type, getDCtx(), NULL, getDictSize(), dictContentType), 0); // equivalent to dict reset
    }

    void load_dict_dictsize_0(ZSTD_loadDict_type type, ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dictSize is 0
        compress_repeated(100);
        EXPECT_EQ(run_load_dict(type, getDCtx(), getDictBuffer(), 0, dictContentType), 0); // equivalent to dict reset
    }

    void load_dict_is_null_dictsize_0(ZSTD_loadDict_type type, ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dict is null and dictSize is 0
        compress_repeated(100);
        EXPECT_EQ(run_load_dict(type, getDCtx(), NULL, 0, dictContentType), 0); // equivalent to dict reset
    }

    void load_dict_dctx_null(ZSTD_loadDict_type type, ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dctx is null
        compress_repeated(100);
        EXPECT_EQ(run_load_dict(type, NULL, getDictBuffer(), getDictSize(), dictContentType), ERROR(GENERIC));
    }

    void load_dict_dctx_streamStage_not_init(ZSTD_loadDict_type type, ZSTD_dictContentType_e dictContentType = ZSTD_dct_auto) { // dctx streamStage not init
        compress_repeated(100);
        ZSTD_DCtx* dctx_no_init = getDCtx();
        dctx_no_init->streamStage = zdss_flush; // modify streamStage
        EXPECT_EQ(run_load_dict(type, dctx_no_init, getDictBuffer(), getDictSize(), dictContentType), ERROR(stage_wrong));
    }

private:
    size_t run_load_dict(ZSTD_loadDict_type type, ZSTD_DCtx* dctx, const void* dict, size_t dictSize,
        ZSTD_dictContentType_e dictContentType) {
        switch (type) {
        case ZSTD_loadDict_type::byValue:
            return Test_ZSTD_DCtx_loadDictionary(dctx, dict, dictSize);
        case ZSTD_loadDict_type::byReference:
            return Test_ZSTD_DCtx_loadDictionary_byReference(dctx, dict, dictSize);
        case ZSTD_loadDict_type::refcdict:
        {
            if (dict == NULL || dictSize == 0) {
                ZSTD_DDict* ddict = Test_ZSTD_createDDict(dict, dictSize);
                size_t ret =  Test_ZSTD_DCtx_refDDict(dctx, ddict);
                Test_ZSTD_freeDDict(ddict);
                return ret;
            }
            else
                return Test_ZSTD_DCtx_refDDict(dctx, getDDict());
        }
        case ZSTD_loadDict_type::refprefix:
            return Test_ZSTD_DCtx_refPrefix(dctx, dict, dictSize);
        case ZSTD_loadDict_type::refprefixAdv:
            return Test_ZSTD_DCtx_refPrefix_advanced(dctx, dict, dictSize, dictContentType);
        default:
            return ERROR(GENERIC);
        }
    }
};

class ZSTD_ZSTD_DCtx_loadDictionary : public ZSTD_ZSTD_DCtx_loadDictionary_base {};

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_pass_common_2)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_pass_common_3)
{
    load_dict_is_null(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_pass_common_4)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_pass_common_5)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_fail_common_6)
{
    load_dict_dctx_null(ZSTD_loadDict_type::byValue);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_fail_common_7)
{
    load_dict_dctx_streamStage_not_init(ZSTD_loadDict_type::byValue);
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_loadDictionary
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_DCtx_loadDictionary_byReference
  ***********************************************/
class ZSTD_ZSTD_DCtx_loadDictionary_byReference : public ZSTD_ZSTD_DCtx_loadDictionary_base {};

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_byReference_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_byReference_pass_common_2)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_byReference_pass_common_3)
{
    load_dict_is_null(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_byReference_pass_common_4)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_byReference_pass_common_5)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_byReference_fail_common_6)
{
    load_dict_dctx_null(ZSTD_loadDict_type::byReference);
}

TEST_F(ZSTD_ZSTD_DCtx_loadDictionary_byReference, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_byReference_fail_common_7)
{
    load_dict_dctx_streamStage_not_init(ZSTD_loadDict_type::byReference);
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_loadDictionary_byReference
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_DCtx_refDDict
  ***********************************************/
class ZSTD_ZSTD_DCtx_refDDict : public ZSTD_ZSTD_DCtx_loadDictionary_base {};

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_pass_common_1)
{
    load_dict_pass(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_pass_common_2)
{
    load_dict_pass_formatted(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_pass_common_3)
{
    load_dict_is_null(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_pass_common_4)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_pass_common_5)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_pass_common_6) // ddict is NULL
{
    EXPECT_EQ(Test_ZSTD_DCtx_refDDict(getDCtx(), NULL), 0);
}

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_fail_common_7)
{
    load_dict_dctx_null(ZSTD_loadDict_type::refcdict);
}

TEST_F(ZSTD_ZSTD_DCtx_refDDict, AOCL_Compression_zstd_ZSTD_DCtx_refDDict_fail_common_8)
{
    load_dict_dctx_streamStage_not_init(ZSTD_loadDict_type::refcdict);
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_refDDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_DCtx_refPrefix
  ***********************************************/
class ZSTD_ZSTD_DCtx_refPrefix : public ZSTD_ZSTD_DCtx_loadDictionary_base {};

TEST_F(ZSTD_ZSTD_DCtx_refPrefix, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_pass_common_1)
{
    // By default, the prefix is treated as raw content. Hence no load_dict_pass_formatted() test added here.
    load_dict_pass(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_pass_common_2)
{
    load_dict_is_null(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_pass_common_3)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_pass_common_4)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_fail_common_5)
{
    load_dict_dctx_null(ZSTD_loadDict_type::refprefix);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_fail_common_6)
{
    load_dict_dctx_streamStage_not_init(ZSTD_loadDict_type::refprefix);
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_refPrefix
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_DCtx_refPrefix_advanced
  ***********************************************/
class ZSTD_ZSTD_DCtx_refPrefix_advanced : public ZSTD_ZSTD_DCtx_loadDictionary_base {};

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_pass_common_1) // pass ZSTD_dct_auto raw
{
    load_dict_pass(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_pass_common_2) // pass ZSTD_dct_auto formatted
{
    load_dict_pass_formatted(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_pass_common_3) // pass ZSTD_dct_rawContent
{
    load_dict_pass(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_rawContent);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_pass_common_4) // pass ZSTD_dct_fullDict
{
    load_dict_pass_formatted(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_fullDict);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_pass_common_5)
{
    load_dict_is_null(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_pass_common_6)
{
    load_dict_dictsize_0(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_pass_common_7)
{
    load_dict_is_null_dictsize_0(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_fail_common_8)
{
    load_dict_dctx_null(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_DCtx_refPrefix_advanced, AOCL_Compression_zstd_ZSTD_DCtx_refPrefix_advanced_fail_common_9)
{
    load_dict_dctx_streamStage_not_init(ZSTD_loadDict_type::refprefixAdv, ZSTD_dct_auto);
}
/*********************************************
 * End of ZSTD_ZSTD_DCtx_refPrefix_advanced
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_getDictID_fromDict
  ***********************************************/
class ZSTD_ZSTD_getDictID_fromDict : public ZSTD_dict_creator {};
TEST_F(ZSTD_ZSTD_getDictID_fromDict, AOCL_Compression_zstd_ZSTD_getDictID_fromDict_pass_common_1) // create format compliant dictionary
{
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    size_t dictID = Test_ZSTD_getDictID_fromDict(getDictBuffer(), getDictSize());
    EXPECT_NE(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromDict, AOCL_Compression_zstd_ZSTD_getDictID_fromDict_fail_common_2) // create non-format compliant dictionary
{
    EXPECT_TRUE(create_raw_content_dict(800));
    size_t dictID = Test_ZSTD_getDictID_fromDict(getDictBuffer(), getDictSize());
    EXPECT_EQ(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromDict, AOCL_Compression_zstd_ZSTD_getDictID_fromDict_fail_common_3) // dict is null
{
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    size_t dictID = Test_ZSTD_getDictID_fromDict(NULL, getDictSize());
    EXPECT_EQ(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromDict, AOCL_Compression_zstd_ZSTD_getDictID_fromDict_fail_common_4) // dictSize < 8
{
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    size_t dictID = Test_ZSTD_getDictID_fromDict(getDictBuffer(), 7);
    EXPECT_EQ(dictID, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_getDictID_fromDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_getDictID_fromCDict
  ***********************************************/
class ZSTD_ZSTD_getDictID_fromCDict : public ZSTD_dict_creator {};
TEST_F(ZSTD_ZSTD_getDictID_fromCDict, AOCL_Compression_zstd_ZSTD_getDictID_fromCDict_pass_common_1) // create format compliant dictionary
{
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    ZSTD_CDict* cdict = Test_ZSTD_createCDict(getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
    ASSERT_NE(cdict, nullptr);
    size_t dictID = Test_ZSTD_getDictID_fromCDict(cdict);
    EXPECT_NE(dictID, 0);
    Test_ZSTD_freeCDict(cdict);
}

TEST_F(ZSTD_ZSTD_getDictID_fromCDict, AOCL_Compression_zstd_ZSTD_getDictID_fromCDict_fail_common_2) // create non-format compliant dictionary
{
    EXPECT_TRUE(create_raw_content_dict(800));
    ZSTD_CDict* cdict = Test_ZSTD_createCDict(getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
    ASSERT_NE(cdict, nullptr);
    size_t dictID = Test_ZSTD_getDictID_fromCDict(cdict);
    EXPECT_EQ(dictID, 0);
    Test_ZSTD_freeCDict(cdict);
}

TEST_F(ZSTD_ZSTD_getDictID_fromCDict, AOCL_Compression_zstd_ZSTD_getDictID_fromCDict_fail_common_3) // cdict is null
{
    size_t dictID = Test_ZSTD_getDictID_fromCDict(NULL);
    EXPECT_EQ(dictID, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_getDictID_fromCDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_getDictID_fromDDict
  ***********************************************/
class ZSTD_ZSTD_getDictID_fromDDict : public ZSTD_dict_creator {};
TEST_F(ZSTD_ZSTD_getDictID_fromDDict, AOCL_Compression_zstd_ZSTD_getDictID_fromDDict_pass_common_1) // create format compliant dictionary
{
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    ZSTD_DDict* ddict = Test_ZSTD_createDDict(getDictBuffer(), getDictSize());
    ASSERT_NE(ddict, nullptr);
    size_t dictID = Test_ZSTD_getDictID_fromDDict(ddict);
    EXPECT_NE(dictID, 0);
    Test_ZSTD_freeDDict(ddict);
}

TEST_F(ZSTD_ZSTD_getDictID_fromDDict, AOCL_Compression_zstd_ZSTD_getDictID_fromDDict_fail_common_2) // create non-format compliant dictionary
{
    EXPECT_TRUE(create_raw_content_dict(800));
    ZSTD_DDict* ddict = Test_ZSTD_createDDict(getDictBuffer(), getDictSize());
    ASSERT_NE(ddict, nullptr);
    size_t dictID = Test_ZSTD_getDictID_fromDDict(ddict);
    EXPECT_EQ(dictID, 0);
    Test_ZSTD_freeDDict(ddict);
}

TEST_F(ZSTD_ZSTD_getDictID_fromDDict, AOCL_Compression_zstd_ZSTD_getDictID_fromDDict_fail_common_3) // ddict is null
{
    size_t dictID = Test_ZSTD_getDictID_fromDDict(NULL);
    EXPECT_EQ(dictID, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_getDictID_fromDDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_getDictID_fromFrame
  ***********************************************/
class ZSTD_ZSTD_getDictID_fromFrame : public ZSTD_decompress_usingDict_base {
public:
    ZSTD_ZSTD_getDictID_fromFrame() {
        size_t disableFdsFrame = 1;
        set_fds_runtime_params(disableFdsFrame);
    }
};
TEST_F(ZSTD_ZSTD_getDictID_fromFrame, AOCL_Compression_zstd_ZSTD_getDictID_fromFrame_pass_common_1) // create format compliant dictionary
{
    compress_repeated_formatted(800);
    size_t dictID = Test_ZSTD_getDictID_fromFrame(get_compressed_buff(), get_compressed_capacity());
    EXPECT_NE(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromFrame, AOCL_Compression_zstd_ZSTD_getDictID_fromFrame_fail_common_2) // create non-format compliant dictionary
{
    compress_repeated(800);
    size_t dictID = Test_ZSTD_getDictID_fromFrame(get_compressed_buff(), get_compressed_capacity());
    EXPECT_EQ(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromFrame, AOCL_Compression_zstd_ZSTD_getDictID_fromFrame_fail_common_3) // compressed stream requires no dictionary
{
    compress_no_dict(800);
    size_t dictID = Test_ZSTD_getDictID_fromFrame(get_compressed_buff(), get_compressed_capacity());
    EXPECT_EQ(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromFrame, AOCL_Compression_zstd_ZSTD_getDictID_fromFrame_fail_common_4) // compressed stream is null
{
    compress_repeated_formatted(800);
    size_t dictID = Test_ZSTD_getDictID_fromFrame(NULL, get_compressed_capacity());
    EXPECT_EQ(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromFrame, AOCL_Compression_zstd_ZSTD_getDictID_fromFrame_fail_common_5) // compressed stream size too small
{
    compress_repeated_formatted(800);
    size_t dictID = Test_ZSTD_getDictID_fromFrame(get_compressed_buff(), 4); // size < ZSTD_FRAMEHEADERSIZE_PREFIX(ZSTD_f_zstd1) 
    EXPECT_EQ(dictID, 0);
}

TEST_F(ZSTD_ZSTD_getDictID_fromFrame, AOCL_Compression_zstd_ZSTD_getDictID_fromFrame_fail_common_6) // non zstd frame
{
    char* non_zstd_frame = (char*)malloc(800);
    memset(non_zstd_frame, 0, 800);
    size_t dictID = Test_ZSTD_getDictID_fromFrame(non_zstd_frame, 800);
    EXPECT_EQ(dictID, 0);
    free(non_zstd_frame);
}
/*********************************************
 * End of ZSTD_ZSTD_getDictID_fromFrame
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_sizeof_CDict
  ***********************************************/
class ZSTD_ZSTD_sizeof_CDict : public ZSTD_dict_creator {};
TEST_F(ZSTD_ZSTD_sizeof_CDict, AOCL_Compression_zstd_ZSTD_sizeof_CDict_pass_common_1) // create format compliant dictionary
{
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    ZSTD_CDict* cdict = Test_ZSTD_createCDict(getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
    ASSERT_NE(cdict, nullptr);
    size_t cdictSize = Test_ZSTD_sizeof_CDict(cdict);
    EXPECT_NE(cdictSize, 0);
    Test_ZSTD_freeCDict(cdict);
}

TEST_F(ZSTD_ZSTD_sizeof_CDict, AOCL_Compression_zstd_ZSTD_sizeof_CDict_pass_common_2) // create non-format compliant dictionary
{
    EXPECT_TRUE(create_raw_content_dict(800));
    ZSTD_CDict* cdict = Test_ZSTD_createCDict(getDictBuffer(), getDictSize(), ZSTD_CLEVEL_DEFAULT);
    ASSERT_NE(cdict, nullptr);
    size_t cdictSize = Test_ZSTD_sizeof_CDict(cdict);
    EXPECT_NE(cdictSize, 0);
    Test_ZSTD_freeCDict(cdict);
}

TEST_F(ZSTD_ZSTD_sizeof_CDict, AOCL_Compression_zstd_ZSTD_sizeof_CDict_pass_common_3) // cdict is null
{
    size_t cdictSize = Test_ZSTD_sizeof_CDict(NULL);
    EXPECT_EQ(cdictSize, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_sizeof_CDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_sizeof_DDict
  ***********************************************/
class ZSTD_ZSTD_sizeof_DDict : public ZSTD_dict_creator {};
TEST_F(ZSTD_ZSTD_sizeof_DDict, AOCL_Compression_zstd_ZSTD_sizeof_DDict_pass_common_1) // create format compliant dictionary
{
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    ZSTD_DDict* ddict = Test_ZSTD_createDDict(getDictBuffer(), getDictSize());
    ASSERT_NE(ddict, nullptr);
    size_t ddictSize = Test_ZSTD_sizeof_DDict(ddict);
    EXPECT_NE(ddictSize, 0);
    Test_ZSTD_freeDDict(ddict);
}

TEST_F(ZSTD_ZSTD_sizeof_DDict, AOCL_Compression_zstd_ZSTD_sizeof_DDict_pass_common_2) // create non-format compliant dictionary
{
    EXPECT_TRUE(create_raw_content_dict(800));
    ZSTD_DDict* ddict = Test_ZSTD_createDDict(getDictBuffer(), getDictSize());
    ASSERT_NE(ddict, nullptr);
    size_t ddictSize = Test_ZSTD_sizeof_DDict(ddict);
    EXPECT_NE(ddictSize, 0);
    Test_ZSTD_freeDDict(ddict);
}

TEST_F(ZSTD_ZSTD_sizeof_DDict, AOCL_Compression_zstd_ZSTD_sizeof_DDict_pass_common_3) // ddict is null
{
    size_t ddictSize = Test_ZSTD_sizeof_DDict(NULL);
    EXPECT_EQ(ddictSize, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_sizeof_DDict
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_estimateCDictSize_advanced
  ***********************************************/
TEST(ZSTD_ZSTD_estimateCDictSize_advanced, AOCL_Compression_zstd_ZSTD_estimateCDictSize_advanced_pass_common_1) // pass
{
    ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(256 KB, ZSTD_CLEVEL_DEFAULT, 0);
    size_t cdictSize = Test_ZSTD_estimateCDictSize_advanced(800, cparams, ZSTD_dlm_byCopy);
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize_advanced, AOCL_Compression_zstd_ZSTD_estimateCDictSize_advanced_pass_common_2) // pass
{
    ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(1 MB, 9, 1);
    size_t cdictSize = Test_ZSTD_estimateCDictSize_advanced(UINT64_MAX, cparams, ZSTD_dlm_byRef);
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize_advanced, AOCL_Compression_zstd_ZSTD_estimateCDictSize_advanced_pass_common_3) // invalid dictLoadMethod
{
    ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(256 KB, ZSTD_CLEVEL_DEFAULT, 0);
    size_t cdictSize = Test_ZSTD_estimateCDictSize_advanced(800, cparams, (ZSTD_dictLoadMethod_e)3);
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize_advanced, AOCL_Compression_zstd_ZSTD_estimateCDictSize_advanced_pass_common_4) // dictSize is 0
{
    ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(256 KB, ZSTD_CLEVEL_DEFAULT, 0);
    size_t cdictSize = Test_ZSTD_estimateCDictSize_advanced(0, cparams, ZSTD_dlm_byCopy);
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize_advanced, AOCL_Compression_zstd_ZSTD_estimateCDictSize_advanced_pass_common_5) // cparams empty
{
    ZSTD_compressionParameters cparams;
    memset(&cparams, 0, sizeof(ZSTD_compressionParameters));
    size_t cdictSize = Test_ZSTD_estimateCDictSize_advanced(800, cparams, ZSTD_dlm_byCopy);
    EXPECT_GT(cdictSize, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateCDictSize_advanced
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_estimateCDictSize
  ***********************************************/
TEST(ZSTD_ZSTD_estimateCDictSize, AOCL_Compression_zstd_ZSTD_estimateCDictSize_pass_common_1) // pass
{
    size_t cdictSize = Test_ZSTD_estimateCDictSize(800, ZSTD_CLEVEL_DEFAULT);
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize, AOCL_Compression_zstd_ZSTD_estimateCDictSize_pass_common_2) // pass
{
    size_t cdictSize = Test_ZSTD_estimateCDictSize(UINT64_MAX, 9);
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize, AOCL_Compression_zstd_ZSTD_estimateCDictSize_pass_common_3) // cLevel minimum
{
    size_t cdictSize = Test_ZSTD_estimateCDictSize(800, Test_ZSTD_minCLevel());
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize, AOCL_Compression_zstd_ZSTD_estimateCDictSize_pass_common_4) // cLevel > maximum
{
    size_t cdictSize = Test_ZSTD_estimateCDictSize(800, Test_ZSTD_maxCLevel() + 1);
    EXPECT_GT(cdictSize, 0);
}

TEST(ZSTD_ZSTD_estimateCDictSize, AOCL_Compression_zstd_ZSTD_estimateCDictSize_pass_common_5) // dictSize is 0
{
    size_t cdictSize = Test_ZSTD_estimateCDictSize(0, ZSTD_CLEVEL_DEFAULT);
    EXPECT_GT(cdictSize, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateCDictSize
 *********************************************/

 /***********************************************
  * Begin of ZSTD_ZSTD_estimateDDictSize
  ***********************************************/
TEST(ZSTD_ZSTD_estimateDDictSize, AOCL_Compression_zstd_ZSTD_estimateDDictSize_pass_common_1) // pass
{
    size_t ddictSize = Test_ZSTD_estimateDDictSize(800, ZSTD_dlm_byCopy);
    EXPECT_GT(ddictSize, 0);
}

TEST(ZSTD_ZSTD_estimateDDictSize, AOCL_Compression_zstd_ZSTD_estimateDDictSize_pass_common_2) // pass
{
    size_t ddictSize = Test_ZSTD_estimateDDictSize(UINT64_MAX, ZSTD_dlm_byRef);
    EXPECT_GT(ddictSize, 0);
}

TEST(ZSTD_ZSTD_estimateDDictSize, AOCL_Compression_zstd_ZSTD_estimateDDictSize_pass_common_3) // invalid dictLoadMethod
{
    size_t ddictSize = Test_ZSTD_estimateDDictSize(800, (ZSTD_dictLoadMethod_e)3);
    EXPECT_GT(ddictSize, 0);
}

TEST(ZSTD_ZSTD_estimateDDictSize, AOCL_Compression_zstd_ZSTD_estimateDDictSize_pass_common_4) // dictSize is 0
{
    size_t ddictSize = Test_ZSTD_estimateDDictSize(0, ZSTD_dlm_byCopy);
    EXPECT_GT(ddictSize, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_estimateDDictSize
 *********************************************/

 /*********************************************
  * Begin of ZSTD_ZSTD_initStaticCDict
  * *********************************************/
class ZSTD_ZSTD_initStaticCDict : public ZSTD_dict_creator {
public:
    ~ZSTD_ZSTD_initStaticCDict() {
        if (workspace)
            free(workspace);
    }

    void create_dict_copy() {
        EXPECT_TRUE(create_train_from_buffer_dict(800));
        cparams = Test_Get_ZSTD_defaultCParameters(256 KB, ZSTD_CLEVEL_DEFAULT, 0);
        workspaceSize = Test_ZSTD_estimateCDictSize_advanced(getDictSize(), cparams, ZSTD_dlm_byCopy);
        workspace = malloc(workspaceSize);
    }

    Test_ZSTD_CDict_s get_cdict(const ZSTD_CDict* cdict) {
        EXPECT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));
        return tcdict;
    }

    void validate_cdict(const ZSTD_CDict* cdict, ZSTD_dictContentType_e type) {
        ASSERT_NE(cdict, nullptr);
        Test_ZSTD_CDict_s tcdict;
        memcpy((void*)(&tcdict), cdict, sizeof(Test_ZSTD_CDict_s));

        EXPECT_EQ(tcdict.dictContentSize, getDictSize());
        EXPECT_EQ(memcmp(tcdict.dictContent, getDictBuffer(), getDictSize()), 0);
        EXPECT_EQ(tcdict.dictContentType, type);
    }


    void* workspace = NULL;
    size_t workspaceSize = 0;
    ZSTD_compressionParameters cparams;
};

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_pass_common_1) { // large enough workspace ZSTD_dlm_byCopy
    create_dict_copy();
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    validate_cdict(cdict, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_pass_common_2) { // large enough workspace ZSTD_dlm_byRef
    EXPECT_TRUE(create_raw_content_dict(100));
    cparams = Test_Get_ZSTD_defaultCParameters(1 MB, 9, 1);
    workspaceSize = Test_ZSTD_estimateCDictSize_advanced(getDictSize(), cparams, ZSTD_dlm_byRef);
    workspace = malloc(workspaceSize);
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byRef, ZSTD_dct_rawContent, cparams);
    validate_cdict(cdict, ZSTD_dct_rawContent);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_fail_common_3) { // workspace is null
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    cparams = Test_Get_ZSTD_defaultCParameters(256 KB, ZSTD_CLEVEL_DEFAULT, 0);
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(NULL, 0, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    EXPECT_EQ(cdict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_fail_common_4) { // workspaceSize is too small
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    cparams = Test_Get_ZSTD_defaultCParameters(256 KB, ZSTD_CLEVEL_DEFAULT, 0);
    workspaceSize = Test_ZSTD_estimateCDictSize_advanced(getDictSize(), cparams, ZSTD_dlm_byCopy);
    workspaceSize /= 2; // insufficient workspace
    workspace = malloc(workspaceSize);
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    EXPECT_EQ(cdict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_fail_common_5) { // workspace not 8-byte aligned
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    cparams = Test_Get_ZSTD_defaultCParameters(256 KB, ZSTD_CLEVEL_DEFAULT, 0);
    workspaceSize = Test_ZSTD_estimateCDictSize_advanced(getDictSize(), cparams, ZSTD_dlm_byCopy);
    workspace = malloc(workspaceSize + 1);
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict((void*)((size_t)workspace + 1), workspaceSize, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    EXPECT_EQ(cdict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_fail_common_6) { // dict is null
    create_dict_copy();
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, NULL, getDictSize(),
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    Test_ZSTD_CDict_s tcdict = get_cdict(cdict);
    EXPECT_EQ(tcdict.dictContent, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_fail_common_7) { // dictSize is 0
    create_dict_copy();
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, getDictBuffer(), 0,
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    Test_ZSTD_CDict_s tcdict = get_cdict(cdict);
    EXPECT_EQ(tcdict.dictContentSize, 0);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_fail_common_8) { // dict is null and dictSize is 0
    create_dict_copy();
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, NULL, 0,
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    Test_ZSTD_CDict_s tcdict = get_cdict(cdict);
    EXPECT_EQ(tcdict.dictContent, nullptr);
    EXPECT_EQ(tcdict.dictContentSize, 0);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_pass_common_9) { // dictLoadMethod invalid
    create_dict_copy();
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, getDictBuffer(), getDictSize(),
        (ZSTD_dictLoadMethod_e)100, ZSTD_dct_auto, cparams);
    validate_cdict(cdict, ZSTD_dct_auto);
}

TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_pass_common_10) { // dictContentType invalid
    create_dict_copy();
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byCopy, (ZSTD_dictContentType_e)100, cparams);
    validate_cdict(cdict, (ZSTD_dictContentType_e)100); // does not fail. accepts invalid value
}

#ifndef DEBUG_ASSERT_ENABLED
/* Assertions get triggered:
`!ZSTD_checkCParams(params.cParams)' failed. */
TEST_F(ZSTD_ZSTD_initStaticCDict, AOCL_Compression_zstd_Test_ZSTD_initStaticCDict_pass_common_11) { // cparams empty
    create_dict_copy();
    memset(&cparams, 0, sizeof(ZSTD_compressionParameters));
    const ZSTD_CDict* cdict = Test_ZSTD_initStaticCDict(workspace, workspaceSize, getDictBuffer(), getDictSize(),
        ZSTD_dlm_byCopy, ZSTD_dct_auto, cparams);
    validate_cdict(cdict, ZSTD_dct_auto);
}
#endif
/*********************************************
 * End of ZSTD_ZSTD_initStaticCDict
 *********************************************/

 /*********************************************
  * Begin of ZSTD_ZSTD_initStaticDDict
  **********************************************/
class ZSTD_ZSTD_initStaticDDict : public ZSTD_dict_creator {
public:
    ~ZSTD_ZSTD_initStaticDDict() {
        if (workspace)
            free(workspace);
    }

    void create_dict_copy() {
        EXPECT_TRUE(create_train_from_buffer_dict(800));
        workspaceSize = Test_ZSTD_estimateDDictSize(getDictSize(), ZSTD_dlm_byCopy);
        workspace = malloc(workspaceSize);
    }

    Test_ZSTD_DDict_s get_ddict(const ZSTD_DDict* ddict) {
        EXPECT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));
        return tddict;
    }

    void validate_ddict(const ZSTD_DDict* ddict) {
        ASSERT_NE(ddict, nullptr);
        Test_ZSTD_DDict_s tddict;
        memcpy((void*)(&tddict), ddict, sizeof(Test_ZSTD_DDict_s));

        EXPECT_EQ(tddict.dictSize, getDictSize());
        EXPECT_EQ(memcmp(tddict.dictContent, getDictBuffer(), getDictSize()), 0);
    }

    void* workspace = NULL;
    size_t workspaceSize = 0;
};

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_pass_common_1) { // large enough workspace ZSTD_dlm_byCopy
    create_dict_copy();
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, getDictBuffer(),
        getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto);
    validate_ddict(ddict);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_pass_common_2) { // large enough workspace ZSTD_dlm_byRef
    EXPECT_TRUE(create_raw_content_dict(100));
    workspaceSize = Test_ZSTD_estimateDDictSize(getDictSize(), ZSTD_dlm_byRef);
    workspace = malloc(workspaceSize);
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, getDictBuffer(),
        getDictSize(), ZSTD_dlm_byRef, ZSTD_dct_rawContent);
    validate_ddict(ddict);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_fail_common_3) { // workspace is null
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(NULL, 0, getDictBuffer(),
        getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto);
    EXPECT_EQ(ddict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_fail_common_4) { // workspaceSize is too small
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    workspaceSize = Test_ZSTD_estimateDDictSize(getDictSize(), ZSTD_dlm_byCopy);
    workspaceSize /= 2; // insufficient workspace
    workspace = malloc(workspaceSize);
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, getDictBuffer(),
        getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto);
    EXPECT_EQ(ddict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_fail_common_5) { // workspace not 8-byte aligned
    EXPECT_TRUE(create_train_from_buffer_dict(800));
    workspaceSize = Test_ZSTD_estimateDDictSize(getDictSize(), ZSTD_dlm_byCopy);
    workspace = malloc(workspaceSize + 1);
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict((void*)((size_t)workspace + 1), workspaceSize, getDictBuffer(),
        getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto);
    EXPECT_EQ(ddict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_fail_common_6) { // dict is null
    create_dict_copy();
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, NULL,
        getDictSize(), ZSTD_dlm_byCopy, ZSTD_dct_auto);
    EXPECT_EQ(ddict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_fail_common_7) { // dictSize is 0
    create_dict_copy();
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, getDictBuffer(),
        0, ZSTD_dlm_byCopy, ZSTD_dct_auto);
    Test_ZSTD_DDict_s tddict = get_ddict(ddict);
    EXPECT_EQ(tddict.dictSize, 0);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_fail_common_8) { // dict is null and dictSize is 0
    create_dict_copy();
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, NULL,
        0, ZSTD_dlm_byCopy, ZSTD_dct_auto);
    EXPECT_EQ(ddict, nullptr);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_pass_common_9) { // dictLoadMethod invalid
    create_dict_copy();
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, getDictBuffer(),
        getDictSize(), (ZSTD_dictLoadMethod_e)100, ZSTD_dct_auto);
    validate_ddict(ddict);
}

TEST_F(ZSTD_ZSTD_initStaticDDict, AOCL_Compression_zstd_Test_ZSTD_initStaticDDict_pass_common_10) { // dictContentType invalid
    create_dict_copy();
    const ZSTD_DDict* ddict = Test_ZSTD_initStaticDDict(workspace, workspaceSize, getDictBuffer(),
        getDictSize(), ZSTD_dlm_byCopy, (ZSTD_dictContentType_e)100);
    validate_ddict(ddict);
}
/*********************************************
 * End of ZSTD_ZSTD_initStaticDDict
 *********************************************/

 /***********************************************
 * Begin of ZSTD_ZSTD_compress_extDict
 ***********************************************/
#define FIRST_BLOCK_SIZE 8
class ZSTD_ZSTD_compress_extDict : public ::testing::TestWithParam<std::tuple<int, int>> {
public:
    void SetUp() override {
        int level = std::get<0>(GetParam());
        int optOff = std::get<1>(GetParam());
        int optLevel = DEFAULT_OPT_LEVEL;
        aocl_setup_zstd_encode(optOff, optLevel, 0, level, 0);
        aocl_setup_zstd_decode(optOff, optLevel, 0, level, 0);

        g_zcc = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_compressionLevel, level);
        ZSTD_compressionParameters cparams = Test_ZSTD_getCParams(level, 0, 0);
        ZSTD_frameParameters fparams = { 1 /* contentSizeHeader*/, 0 /* checksumFlag */, 0 /* noDictIDFlag */ };
        params.fParams = fparams;
        params.cParams = cparams;
    }

    void TearDown() override {
        if (g_zcc)
            ZSTD_freeCCtx(g_zcc);
        aocl_destroy_zstd_decode();
        aocl_destroy_zstd_encode();
    }

    /* compress in multiple blocks. 1st block gets used as dict for 2nd block and
    *_extDict functions gets called for the 2nd block */
    size_t multi_block_compress(void* dst, size_t dstCapacity, void* src, size_t srcSize) {
        BYTE firstBlockBuf[FIRST_BLOCK_SIZE];
        size_t ret = ZSTD_compressBegin_advanced(g_zcc, NULL, 0, params, srcSize);
        if (Test_ZSTD_isError(ret)) {
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
            if (Test_ZSTD_isError(ret)) {
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
        if (Test_ZSTD_isError(ret)) {
            EXPECT_EQ(ret, 0); //ret > 0 if error. Test should fail.
            return 0;
        }
        outLen += ret;

        return outLen;
    }

    bool zstd_check_uncompressed_equal_to_original_stream(const char* src, size_t srcSize, 
        const char* compressed, size_t compressedLen) {
        char* uncompressed = (char*)calloc(srcSize, sizeof(char));
        ZSTD_DCtx* const dctx = ZSTD_createDCtx();
        bool ret = false;
        size_t totalCSize = 0;
        size_t totalGenSize = 0;
        while (totalCSize < compressedLen) { /* Loop over multiple frames */
            ZSTD_resetDStream(dctx); /* Context reset to start a new decompression */
            if (Test_ZSTD_isError(ZSTD_decompressBegin(dctx))) 
                goto _cleanup;
            while (totalCSize < compressedLen) {
                size_t const inSize = ZSTD_nextSrcSizeToDecompress(dctx);

                if (inSize == 0) break; /* Frame completed. No more data to provide as src to ZSTD_decompressContinue(). */

                size_t const genSize = ZSTD_decompressContinue(dctx, uncompressed + totalGenSize, srcSize - totalGenSize, compressed + totalCSize, inSize);
                if (Test_ZSTD_isError(genSize))
                    goto _cleanup;
                totalGenSize += genSize;
                totalCSize += inSize;
            }
        }

        if (!(srcSize == totalGenSize))
            goto _cleanup;

        ret = (memcmp(src, uncompressed, srcSize) == 0);

_cleanup:
        free(uncompressed);
        ZSTD_freeDCtx(dctx);
        return ret;
    }

private:
    ZSTD_parameters params;
    ZSTD_CCtx* g_zcc = NULL;
};

TEST_P(ZSTD_ZSTD_compress_extDict, AOCL_Compression_zstd_ZSTD_compressStream_common_1) //compress multiple blocks
{
    //setup
    TestLoad_2 d(1024);
    void* src = d.getOrigData();
    size_t srcSize = d.getOrigSize();
    void* dst = d.getCompressedBuff();
    size_t dstCapacity = d.getCompressedSize();

    //compress
    size_t outLen = multi_block_compress(dst, dstCapacity, src, srcSize);

    //validate
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original_stream(
        d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

INSTANTIATE_TEST_SUITE_P(
    ZSTD_ZSTD_COMPRESS_EXTDICT,
    ZSTD_ZSTD_compress_extDict,
    ::testing::Combine(::testing::Range(0, 23), \
    ::testing::ValuesIn({-1, 0, 1 } /* optOff */)));
/*********************************************
 * End of ZSTD_ZSTD_compress_extDict
 *********************************************/
