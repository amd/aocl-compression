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
#include "gtest/gtest_utils.h"

#define ZSTD_STATIC_LINKING_ONLY
#include "algos/zstd/lib/zstd.h"
#include "algos/zstd/lib/compress/zstd_lazy.h"
#include "algos/zstd/lib/decompress/zstd_decompress_block.h"

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

public:
    TestLoad_2(size_t sz, gtest_data_gen_type type = gtest_data_gen_type::random)
    {
        data_gen = new gtest_data_gen_t(ZSTD_compressBound, sz, type);
    }

    TestLoad_2(size_t inp_sz, size_t out_sz, gtest_data_gen_type type = gtest_data_gen_type::random)
    {
        data_gen = new gtest_data_gen_t(ZSTD_compressBound, inp_sz, out_sz, type);
    }

    ~TestLoad_2() {
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

    // Fill dst buffer
    bool dstset(unsigned char val, char* start, char* end) {
        return data_gen->dstSet(val, start, end);
    }
private:
    gtest_data_gen_t* data_gen = NULL;
};

enum class ZSTD_Compress_API {
    compress, compress_advanced, compress2, compress_cctx, compress_stream2_continue, compress_stream2_end,
    compress_stream2_flush, compress_stream_continue, compress_stream_end, compress_stream_flush,
    compress_sequence, compress_cdict, compress_cdict2, compress_dict
};

enum class ZSTD_Decompress_API {
    decompress, decompress_dctx, decompress_stream, decompress_ddict, decompress_dict
};

enum class ZSTD_loadDict_type {
    byValue, byReference, advanced, refcdict, refprefix, refprefixAdv
};

class ZSTD_frame_creator
{
public:
    ZSTD_frame_creator()
    {
        d = new TestLoad_2(800);
        original = d->getOrigData();
        origLen = d->getOrigSize();
    }

    // Destructor function.
    virtual ~ZSTD_frame_creator()
    {
        if (d)
            delete d;
    }

    // helper functions to create different types of frames
    void create_frame();
    void create_frame_overwrite();
    size_t create_frames_multiple();
    void create_frame_skippable(unsigned magicVarint);
    void create_frame_and_skippable();
    void create_frame_with_no_decomp_size();
    void create_frame_with_params(ZSTD_frameParameters fparams);
    void create_frame_prefix_unknown();
    void create_frame_invalid_data_block();
    void create_empty_frame();
    void create_stream_frame();
    void create_frame_reference();
    void skip_until_zstd_frame();

protected:
    // Compressed data is stored in the buffer `src`.
    char* src = NULL;
    size_t srcLen;

    // Original data which we are about to compress is contained in the buffer `original`.
    char* original = NULL;
    size_t origLen;

    TestLoad_2* d = NULL;  
};

class ZSTD_ZSTD_decompress_base : public AOCL_setup_zstd, public ZSTD_frame_creator
{
public:
    ZSTD_ZSTD_decompress_base() : ZSTD_frame_creator()
    {
        /* decompress should not overwrite dst buffer beyond origLen.
        * Lossless compression should produce the exact bytes as original.
        * Hence, exact same size allocated for output buffer */
        outLen = d->getOrigSize();
        output = (char*)malloc(outLen);
    }

    // Destructor function.
    virtual ~ZSTD_ZSTD_decompress_base()
    {
        if (output)
            free(output);
    }

    // tests for different decompress use cases
    void decompress_pass(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_skip(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_src_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_dst_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_buffer_inadequate(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_srcsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_src_null_srcsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_src_null_srcsize_0_dstsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_corrupt_magic_number(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_corrupt_frame_header(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_corrupt_data_block(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_garbage_after_frame(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_N_frames(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_skippable_frame(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_checksumFlag_set(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_checksum_wrong(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_contentSizeFlag_set(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_noDictIDFlag_set(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_srcsize_less(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);
    void decompress_srcsize_more(ZSTD_Decompress_API api, ZSTD_DCtx* dctx);

protected:
    // Decompressed data will be stored in the buffer `output`.
    char* output = NULL;
    size_t outLen;

    size_t run_decompress(ZSTD_Decompress_API api, ZSTD_DCtx* dctx,
        void* dst, size_t dstCapacity, const void* src, size_t srcSize);
    void validate_decompress(const char* original, unsigned origLen, const char* output, unsigned outputLen);
};

typedef size_t(*ZSTD_decompress_fp)(ZSTD_DCtx* dctx,
    void* dst, size_t dstCapacity,
    const void* src, size_t srcSize);

class ZSTD_ZSTD_compress_base : public AOCL_setup_zstd {
public:
    virtual ~ZSTD_ZSTD_compress_base() {}

    void validate_compress_format(char* compressed, unsigned compressedLen, unsigned dstCapacity);
    void validate_compress(char* src, unsigned srcSize, char* compressed, unsigned compressedLen, unsigned dstCapacity);
    
    void compress_all_levels(ZSTD_Compress_API api, ZSTD_CCtx* cctx);
    void compress_src_null(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel);
    void compress_dst_null(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel);
    void compress_insufficient_dstCapacity(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel);
    void compress_srcsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel);
    void compress_src_null_srcsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel);
    void compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel);
    void compress_level_lt_min(ZSTD_Compress_API api, ZSTD_CCtx* cctx);
    void compress_level_gt_max(ZSTD_Compress_API api, ZSTD_CCtx* cctx);
    void compress_cctx_null(ZSTD_Compress_API api);

private:
    size_t run_compress(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel, void* dst, size_t dstCapacity,
        const void* src, size_t srcSize);
};

typedef size_t(*ZSTD_estimateSize_fp)(int);
typedef size_t(*ZSTD_estimateSize_usingCParams_fp)(ZSTD_compressionParameters cparams);
typedef size_t(*ZSTD_estimateSize_usingCCtxParams_fp)(const ZSTD_CCtx_params* params);

class ZSTD_ZSTD_estimateSize : public AOCL_setup_zstd {
public:
    void monotonic_increasing(ZSTD_estimateSize_fp fp);
    void level_below_0(ZSTD_estimateSize_fp fp);
    void level_above_max(ZSTD_estimateSize_fp fp);
};

class ZSTD_ZSTD_estimateSize_usingCParams : public AOCL_setup_zstd {
public:
    void no_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCParams_fp fp);
    void with_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCParams_fp fp);
    void level_below_0(ZSTD_estimateSize_usingCParams_fp fp);
    void level_above_max(ZSTD_estimateSize_usingCParams_fp fp);
};

class ZSTD_ZSTD_CCtxParams_init : public AOCL_setup_zstd {
public:

    void SetUp() override;
    virtual ~ZSTD_ZSTD_CCtxParams_init();

    ZSTD_CCtx_params* cctxParams;
};

class ZSTD_ZSTD_estimateSize_usingCCtxParams : public ZSTD_ZSTD_CCtxParams_init {
public:
    void no_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCCtxParams_fp fp);
    void with_source_estimate(size_t sz_no_params, int level, ZSTD_estimateSize_usingCCtxParams_fp fp);
    void with_source_estimate_stable(size_t sz_no_params, int level, ZSTD_estimateSize_usingCCtxParams_fp fp);
    void level_below_0(ZSTD_estimateSize_usingCCtxParams_fp fp);
    void level_above_max(ZSTD_estimateSize_usingCCtxParams_fp fp);
};

class ZSTD_ZSTD_initStatic : public AOCL_setup_zstd {
public:
    void workspace_sufficient(ZSTD_Compress_API api);
    void workspace_null(ZSTD_Compress_API api);
    void workspace_too_small(ZSTD_Compress_API api);
    void workspace_not_aligned(ZSTD_Compress_API api);
private:
    void* run_init(ZSTD_Compress_API api, void* workspace, size_t workspaceSize);
};

/* Helper functions */
bool zstd_check_uncompressed_equal_to_original(const char* src, size_t srcSize,
    const char* compressed, size_t compressedLen, ZSTD_decompress_fp decomp_fp);
size_t insert_frame(void* dst, size_t dstCapacity, const void* src, size_t srcSize);
size_t insert_skippable_frame(void* dst, size_t dstCapacity, const void* src, size_t srcSize, unsigned magicVarint);
size_t insert_frame_overwrite(char* dst, size_t dstCapacity, char* src, size_t srcSize);
size_t insert_N_frames(char* dst, size_t dstCapacity, char* src, size_t srcSize, size_t N, size_t& srcWritten);
size_t insert_frame_via_stream(void* dst, size_t dstCapacity, const void* src, size_t srcSize);
bool has_valid_frame_content_size(char* compressed, unsigned compressedLen);
int get_cparam_below_lower(ZSTD_cParameter param);
int get_cparam_above_upper(ZSTD_cParameter param);
int get_cparam_within_bounds(ZSTD_cParameter param);
int get_dparam_below_lower(ZSTD_dParameter param);
int get_dparam_above_upper(ZSTD_dParameter param);
int get_dparam_within_bounds(ZSTD_dParameter param);
bool has_valid_frames(char* compressed, unsigned compressedLen);
void* Test_ZSTD_custom_alloc_pass(void* opaque, size_t size);
void Test_ZSTD_custom_free_pass(void* opaque, void* address);
void* Test_ZSTD_custom_alloc_fail(void* opaque, size_t size);
void Test_ZSTD_custom_free_fail(void* opaque, void* address);

/* Commonly used wrapper functions */
bool Test_ZSTD_isError(size_t len);
ZSTD_parameters Test_ZSTD_getParams(int compressionLevel, unsigned long long srcSizeHint, size_t dictSize);
unsigned long long Test_ZSTD_decompressBound(const void* src, size_t srcLen);
size_t Test_ZSTD_compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize, int compressionLevel);
size_t Test_ZSTD_frameHeaderSize(const void* src, size_t srcSize);
unsigned long long Test_ZSTD_getFrameContentSize(const void* src, size_t srcSize);
size_t Test_ZSTD_compress2(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);
size_t Test_ZSTD_compress_advanced(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, const void* dict, size_t dictSize, ZSTD_parameters params);
ZSTD_CCtx* Test_ZSTD_createCCtx(void);
ZSTD_DCtx* Test_ZSTD_createDCtx(void);
size_t Test_ZSTD_freeCCtx(ZSTD_CCtx* cctx);
size_t Test_ZSTD_freeDCtx(ZSTD_DCtx* cctx);
ZSTD_bounds Test_ZSTD_cParam_getBounds(ZSTD_cParameter cParam);
ZSTD_bounds Test_ZSTD_dParam_getBounds(ZSTD_dParameter dParam);
size_t Test_ZSTD_compressCCtx(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, int compressionLevel);
size_t Test_ZSTD_decompressDCtx(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity,
    const void* src, size_t srcSize);
size_t Test_ZSTD_compressStream2(ZSTD_CCtx* cctx, ZSTD_outBuffer* output,
    ZSTD_inBuffer* input, ZSTD_EndDirective endOp);
size_t Test_ZSTD_decompressStream(ZSTD_DStream* zds, ZSTD_outBuffer* output, ZSTD_inBuffer* input);
int Test_ZSTD_maxCLevel(void);
int Test_ZSTD_minCLevel(void);
ZSTD_compressionParameters Test_ZSTD_getCParams(int compressionLevel, unsigned long long estimatedSrcSize, size_t dictSize);
size_t Test_ZSTD_estimateDCtxSize(void);
ZSTD_CStream* Test_ZSTD_initStaticCStream(void* workspace, size_t workspaceSize);
size_t Test_ZSTD_CCtx_getParameter(const ZSTD_CCtx* cctx, ZSTD_cParameter param, int* value);
size_t Test_ZSTD_CCtx_setParameter(ZSTD_CCtx* cctx, ZSTD_cParameter param, int value);
size_t Test_ZSTD_DCtx_getParameter(ZSTD_DCtx* dctx, ZSTD_dParameter param, int* value);
size_t Test_ZSTD_DCtx_setParameter(ZSTD_DCtx* dctx, ZSTD_dParameter param, int value);
size_t Test_ZSTD_CCtx_setParams(ZSTD_CCtx* cctx, ZSTD_parameters params);
size_t Test_ZSTD_CCtx_reset(ZSTD_CCtx* cctx, ZSTD_ResetDirective reset);
size_t Test_ZSTD_DCtx_reset(ZSTD_DCtx* dctx, ZSTD_ResetDirective reset);
size_t Test_ZSTD_generateSequences(ZSTD_CCtx* zc, ZSTD_Sequence* outSeqs,
    size_t outSeqsSize, const void* src, size_t srcSize);
size_t Test_ZSTD_compressSequences(ZSTD_CCtx* cctx, void* dst, size_t dstSize,
    const ZSTD_Sequence* inSeqs, size_t inSeqsSize, const void* src, size_t srcSize);
size_t Test_ZSTD_CCtxParams_init(ZSTD_CCtx_params* cctxParams, int compressionLevel);
size_t Test_ZSTD_decompressContinue(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);
size_t Test_ZSTD_estimateCCtxSize(int compressionLevel);
ZSTD_CCtx* Test_ZSTD_initStaticCCtx(void* workspace, size_t workspaceSize);
size_t Test_ZSTD_nextSrcSizeToDecompress(ZSTD_DCtx* dctx);

#define CHECK_PASS_ZSTD(foo) EXPECT_FALSE(Test_ZSTD_isError(foo));
#define CHECK_FAIL_ZSTD(foo) EXPECT_TRUE(Test_ZSTD_isError(foo));
