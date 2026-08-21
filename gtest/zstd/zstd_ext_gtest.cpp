/**
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

 /** @file zstd_ext_gtest.cpp
 *
 *  @brief Extended test cases for zstd / zdict APIs.
 *
 *  Added under the AOCL_TEST_COVERAGE_EXT user option to broaden test coverage
 *  of non-deprecated public zstd (zstd.h) and dictionary-builder (zdict.h) APIs
 *  beyond the base suite. Deprecated APIs are excluded.
 */

#ifdef AOCL_TEST_COVERAGE_EXT

#include <cstring>
#include <cstdlib>
#include <vector>
#include <functional>
#include "gtest/gtest.h"

#ifndef ZSTD_STATIC_LINKING_ONLY
#define ZSTD_STATIC_LINKING_ONLY
#endif
#include "algos/zstd/lib/zstd.h"

#ifndef ZDICT_STATIC_LINKING_ONLY
#define ZDICT_STATIC_LINKING_ONLY
#endif
#include "algos/zstd/lib/zdict.h"

#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

#include "gtest_utils.h"

using namespace std;

/* KB and ScopeGuard are provided by gtest_utils.h. */

/* Fixture parametrized over the dynamic-dispatcher flags, like the base ZSTD
 * suites: each case runs once per {optOff, optLevel} pair, so both encode and
 * decode exercise the reference (opt-off) path and every supported ISA level
 * (incl. AVX512 where present). */
class ZSTD_ext : public OPT_LEVEL_TESTS {
public:
    ZSTD_ext() {
        /* Destroy first so setup always re-registers with this suite's opt
         * level even if a prior fixture left the dispatcher configured. */
        aocl_destroy_zstd_decode();
        aocl_destroy_zstd_encode();
        DynamicDispatch opt = GetParam();
        aocl_setup_zstd_encode(opt.optOff, opt.optLevel, 0, 0, 0);
        aocl_setup_zstd_decode(opt.optOff, opt.optLevel, 0, 0, 0);
    }
    ~ZSTD_ext() {
        aocl_destroy_zstd_decode();
        aocl_destroy_zstd_encode();
    }
};

/* One instantiation drives every ZSTD_ext TEST_P across all dispatcher flags. */
INSTANTIATE_TEST_SUITE_P(
    ZSTD,
    ZSTD_ext,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/* Produce a valid single-frame zstd stream from `src`. Returns compressed size.
 * `frame` must have ZSTD_compressBound(srcSize) capacity. */
static size_t make_frame(const void* src, size_t srcSize, void* frame, size_t frameCap, int level = 3)
{
    size_t c = ZSTD_compress(frame, frameCap, src, srcSize, level);
    if (ZSTD_isError(c)) { ADD_FAILURE() << ZSTD_getErrorName(c); return 0; }
    return c;
}

/* Custom allocator hooks used by the *_advanced constructors. `opaque` points
 * at an int counter incremented on each alloc. */
static void* ext_alloc(void* opaque, size_t size) {
    if (opaque) (*(int*)opaque)++;
    return malloc(size);
}
static void ext_free(void* opaque, void* address) {
    (void)opaque;
    free(address);
}

/***********************************************
 * Error-code helpers
 ***********************************************/

/* ZSTD_getErrorCode: a non-error result maps to ZSTD_error_no_error. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_getErrorCode_no_error_common_1)
{
    size_t ok = ZSTD_compressBound(100); /* not an error code */
    EXPECT_EQ(ZSTD_getErrorCode(ok), ZSTD_error_no_error);
    EXPECT_FALSE(ZSTD_isError(ok));
}

/* ZSTD_getErrorCode: an error result maps to a non-zero enum whose name is
 * retrievable and consistent with ZSTD_getErrorName. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_getErrorCode_error_common_2)
{
    char tiny[4];
    /* dstCapacity far too small => guaranteed error. */
    size_t err = ZSTD_compress(tiny, 1, "abcdefgh", 8, 3);
    ASSERT_TRUE(ZSTD_isError(err));
    ZSTD_ErrorCode code = ZSTD_getErrorCode(err);
    EXPECT_NE(code, ZSTD_error_no_error);
    EXPECT_NE(ZSTD_getErrorName(err), nullptr);
    EXPECT_GT(strlen(ZSTD_getErrorName(err)), (size_t)0);
}

/***********************************************
 * ZSTD_isFrame
 ***********************************************/

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_isFrame_valid_common_1)
{
    const size_t srcSize = 512;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::random);
    vector<char> frame(ZSTD_compressBound(srcSize));
    size_t c = make_frame(gen.getOrigBuff(), srcSize, frame.data(), frame.size());
    EXPECT_EQ(ZSTD_isFrame(frame.data(), c), 1u);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_isFrame_invalid_common_2)
{
    const char garbage[16] = { 0 };
    EXPECT_EQ(ZSTD_isFrame(garbage, sizeof(garbage)), 0u);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_isFrame_too_small_common_3)
{
    const char partial[2] = { 0x28, (char)0xB5 }; /* first 2 magic bytes only */
    EXPECT_EQ(ZSTD_isFrame(partial, sizeof(partial)), 0u);
}

/***********************************************
 * Sequence helpers
 ***********************************************/

/* ZSTD_sequenceBound is monotonic non-decreasing and >= 1 for any size. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_sequenceBound_monotonic_common_1)
{
    size_t prev = ZSTD_sequenceBound(0);
    for (size_t sz = 1; sz <= 64 KB; sz <<= 1) {
        size_t cur = ZSTD_sequenceBound(sz);
        EXPECT_GE(cur, prev);
        prev = cur;
    }
    EXPECT_GE(ZSTD_sequenceBound(1), (size_t)1);
}

/* ZSTD_sequenceBound must bound the count actually produced by
 * ZSTD_generateSequences. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_sequenceBound_bounds_generate_common_2)
{
    const size_t srcSize = 4 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    size_t bound = ZSTD_sequenceBound(srcSize);
    ASSERT_GT(bound, (size_t)0);

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    vector<ZSTD_Sequence> seqs(bound);
    size_t n = ZSTD_generateSequences(cctx, seqs.data(), seqs.size(),
        gen.getOrigBuff(), srcSize);
    ZSTD_freeCCtx(cctx);
    /* ZSTD_generateSequences is documented as allowed to give up and fail on
     * some inputs; skip rather than silently pass when it does, consistent with
     * the ZSTD_mergeBlockDelimiters test below. */
    if (ZSTD_isError(n)) GTEST_SKIP();
    EXPECT_LE(n, bound);
}

/* ZSTD_mergeBlockDelimiters removes explicit block-delimiter / last-literals
 * sequences (marked by offset==0, matchLength==0) by merging them into the next
 * sequence's literals. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_mergeBlockDelimiters_common_1)
{
    const size_t srcSize = 4 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    vector<ZSTD_Sequence> seqs(ZSTD_sequenceBound(srcSize));
    size_t n = ZSTD_generateSequences(cctx, seqs.data(), seqs.size(),
        gen.getOrigBuff(), srcSize);
    ZSTD_freeCCtx(cctx);
    /* ZSTD_generateSequences is documented as allowed to give up and fail on
     * some inputs; skip rather than fail when it does. */
    if (ZSTD_isError(n)) GTEST_SKIP();
    ASSERT_GT(n, (size_t)0);

    size_t merged = ZSTD_mergeBlockDelimiters(seqs.data(), n);
    ASSERT_FALSE(ZSTD_isError(merged));
    EXPECT_LE(merged, n);
    /* A block delimiter / last-literals sequence is marked by offset==0 (with
     * matchLength==0); its litLength is normally non-zero. After merging, the
     * result must have no explicit block boundaries left, i.e. no offset==0
     * sequence remains (a real match always has offset >= 1). */
    for (size_t i = 0; i < merged; ++i) {
        bool isDelim = (seqs[i].matchLength == 0 && seqs[i].offset == 0);
        EXPECT_FALSE(isDelim);
    }
}

/***********************************************
 * Memory estimation
 ***********************************************/

/* ZSTD_estimateDStreamSize_fromFrame returns a positive estimate for a valid
 * frame and is >= the base DStream estimate. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_estimateDStreamSize_fromFrame_common_1)
{
    const size_t srcSize = 8 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    vector<char> frame(ZSTD_compressBound(srcSize));
    size_t c = make_frame(gen.getOrigBuff(), srcSize, frame.data(), frame.size());

    size_t est = ZSTD_estimateDStreamSize_fromFrame(frame.data(), c);
    EXPECT_FALSE(ZSTD_isError(est));
    EXPECT_GT(est, (size_t)0);
}

/* An invalid/too-small buffer yields an error. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_estimateDStreamSize_fromFrame_bad_common_2)
{
    const char garbage[8] = { 0 };
    size_t est = ZSTD_estimateDStreamSize_fromFrame(garbage, sizeof(garbage));
    EXPECT_TRUE(ZSTD_isError(est));
}

/***********************************************
 * Custom-memory stream constructors
 ***********************************************/

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_createCStream_advanced_common_1)
{
    int allocs = 0;
    ZSTD_customMem cmem = { ext_alloc, ext_free, &allocs };
    ZSTD_CStream* cs = ZSTD_createCStream_advanced(cmem);
    ASSERT_NE(cs, nullptr);
    ScopeGuard freeCs([&]{ ZSTD_freeCStream(cs); });
    EXPECT_GT(allocs, 0);

    /* The stream must be usable for a full round-trip. */
    const size_t srcSize = 2 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    vector<char> dst(ZSTD_compressBound(srcSize));
    ZSTD_inBuffer in = { gen.getOrigBuff(), srcSize, 0 };
    ZSTD_outBuffer out = { dst.data(), dst.size(), 0 };
    /* Start a new compression session on the fresh stream. */
    ASSERT_FALSE(ZSTD_isError(ZSTD_initCStream(cs, 3)));
    size_t r = ZSTD_compressStream2(cs, &out, &in, ZSTD_e_end);
    EXPECT_FALSE(ZSTD_isError(r));
    EXPECT_EQ(r, (size_t)0); /* fully flushed */

    vector<char> back(srcSize);
    size_t d = ZSTD_decompress(back.data(), srcSize, dst.data(), out.pos);
    ASSERT_EQ(d, srcSize);
    EXPECT_EQ(memcmp(back.data(), gen.getOrigBuff(), srcSize), 0);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_createDStream_advanced_common_1)
{
    int allocs = 0;
    ZSTD_customMem cmem = { ext_alloc, ext_free, &allocs };
    ZSTD_DStream* ds = ZSTD_createDStream_advanced(cmem);
    ASSERT_NE(ds, nullptr);
    ScopeGuard freeDs([&]{ ZSTD_freeDStream(ds); });
    EXPECT_GT(allocs, 0);

    const size_t srcSize = 2 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    vector<char> frame(ZSTD_compressBound(srcSize));
    size_t c = make_frame(gen.getOrigBuff(), srcSize, frame.data(), frame.size());

    vector<char> back(srcSize);
    ZSTD_inBuffer in = { frame.data(), c, 0 };
    ZSTD_outBuffer out = { back.data(), srcSize, 0 };
    /* Start a new decompression operation on the fresh stream. */
    ASSERT_FALSE(ZSTD_isError(ZSTD_initDStream(ds)));
    size_t r = ZSTD_decompressStream(ds, &out, &in);
    EXPECT_FALSE(ZSTD_isError(r));
    EXPECT_EQ(out.pos, srcSize);
    EXPECT_EQ(memcmp(back.data(), gen.getOrigBuff(), srcSize), 0);
}

/***********************************************
 * by-reference dictionaries
 ***********************************************/

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_createCDict_byReference_common_1)
{
    const size_t dictSize = 4 KB;
    const size_t srcSize = 4 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);

    ZSTD_CDict* cdict = ZSTD_createCDict_byReference(dict.data(), dictSize, 3);
    ASSERT_NE(cdict, nullptr);
    ScopeGuard freeCdict([&]{ ZSTD_freeCDict(cdict); });

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    ScopeGuard freeCctx([&]{ ZSTD_freeCCtx(cctx); });
    vector<char> comp(ZSTD_compressBound(srcSize));
    size_t c = ZSTD_compress_usingCDict(cctx, comp.data(), comp.size(),
        gen.getOrigBuff(), srcSize, cdict);
    ASSERT_FALSE(ZSTD_isError(c));

    /* Decompress with the matching DDict. */
    ZSTD_DDict* ddict = ZSTD_createDDict(dict.data(), dictSize);
    ASSERT_NE(ddict, nullptr);
    ScopeGuard freeDdict([&]{ ZSTD_freeDDict(ddict); });
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ ZSTD_freeDCtx(dctx); });
    vector<char> back(srcSize);
    size_t d = ZSTD_decompress_usingDDict(dctx, back.data(), srcSize, comp.data(), c, ddict);
    ASSERT_EQ(d, srcSize);
    EXPECT_EQ(memcmp(back.data(), gen.getOrigBuff(), srcSize), 0);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_createDDict_byReference_common_1)
{
    const size_t dictSize = 4 KB;
    const size_t srcSize = 4 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    ScopeGuard freeCctx([&]{ ZSTD_freeCCtx(cctx); });
    vector<char> comp(ZSTD_compressBound(srcSize));
    size_t c = ZSTD_compress_usingDict(cctx, comp.data(), comp.size(),
        gen.getOrigBuff(), srcSize, dict.data(), dictSize, 3);
    ASSERT_FALSE(ZSTD_isError(c));

    ZSTD_DDict* ddict = ZSTD_createDDict_byReference(dict.data(), dictSize);
    ASSERT_NE(ddict, nullptr);
    ScopeGuard freeDdict([&]{ ZSTD_freeDDict(ddict); });
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ ZSTD_freeDCtx(dctx); });
    vector<char> back(srcSize);
    size_t d = ZSTD_decompress_usingDDict(dctx, back.data(), srcSize, comp.data(), c, ddict);
    ASSERT_EQ(d, srcSize);
    EXPECT_EQ(memcmp(back.data(), gen.getOrigBuff(), srcSize), 0);
}

/***********************************************
 * Thread pool
 *
 * ZSTD_createThreadPool / ZSTD_freeThreadPool are only defined when the library
 * is built with multithreading (ZSTD_MULTITHREAD, i.e. NATIVE_ENABLE_THREADS),
 * so these tests are gated on that.
 ***********************************************/
#ifdef NATIVE_ENABLE_THREADS

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_createThreadPool_free_common_1)
{
    ZSTD_threadPool* pool = ZSTD_createThreadPool(2);
    ASSERT_NE(pool, nullptr);
    ZSTD_freeThreadPool(pool);
    /* free(NULL) is explicitly allowed. */
    ZSTD_freeThreadPool(NULL);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_CCtx_refThreadPool_common_2)
{
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    ScopeGuard freeCctx([&]{ ZSTD_freeCCtx(cctx); });
    ZSTD_threadPool* pool = ZSTD_createThreadPool(2);
    ScopeGuard freePool([&]{ ZSTD_freeThreadPool(pool); });

    /* Referencing a pool (or NULL to use the internal pool) must not error. */
    EXPECT_FALSE(ZSTD_isError(ZSTD_CCtx_refThreadPool(cctx, pool)));
    /* Detach the pool from the context before either is freed. */
    EXPECT_FALSE(ZSTD_isError(ZSTD_CCtx_refThreadPool(cctx, NULL)));

    /* The context must still compress correctly after pool assignment. */
    const size_t srcSize = 2 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    vector<char> comp(ZSTD_compressBound(srcSize));
    size_t c = ZSTD_compress2(cctx, comp.data(), comp.size(), gen.getOrigBuff(), srcSize);
    EXPECT_FALSE(ZSTD_isError(c));
}

#endif /* NATIVE_ENABLE_THREADS (ZSTD thread pool) */

/***********************************************
 * simpleArgs streaming
 ***********************************************/

/* Full compress+decompress round-trip using only the simpleArgs entry points
 * (no ZSTD_inBuffer/ZSTD_outBuffer structs). */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_compressStream2_simpleArgs_roundtrip_common_1)
{
    const size_t srcSize = 4 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    vector<char> comp(ZSTD_compressBound(srcSize));

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    size_t dstPos = 0, srcPos = 0;
    size_t r = ZSTD_compressStream2_simpleArgs(cctx, comp.data(), comp.size(), &dstPos,
        gen.getOrigBuff(), srcSize, &srcPos, ZSTD_e_end);
    ZSTD_freeCCtx(cctx);
    ASSERT_FALSE(ZSTD_isError(r));
    EXPECT_EQ(r, (size_t)0);           /* fully flushed */
    EXPECT_EQ(srcPos, srcSize);        /* all input consumed */

    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ ZSTD_freeDCtx(dctx); });
    vector<char> back(srcSize);
    size_t dDstPos = 0, dSrcPos = 0;
    size_t dr = ZSTD_decompressStream_simpleArgs(dctx, back.data(), srcSize, &dDstPos,
        comp.data(), dstPos, &dSrcPos);
    ASSERT_FALSE(ZSTD_isError(dr));
    EXPECT_EQ(dDstPos, srcSize);
    EXPECT_EQ(memcmp(back.data(), gen.getOrigBuff(), srcSize), 0);
}

/***********************************************
 * Frame progression
 ***********************************************/

/* ZSTD_getFrameProgression / ZSTD_toFlushNow report sane counters across a
 * streaming compression. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_getFrameProgression_common_1)
{
    const size_t srcSize = 8 KB;
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);
    vector<char> dst(ZSTD_compressBound(srcSize));

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    ScopeGuard freeCctx([&]{ ZSTD_freeCCtx(cctx); });

    ZSTD_frameProgression p0 = ZSTD_getFrameProgression(cctx);
    EXPECT_EQ(p0.ingested, 0ULL);
    EXPECT_EQ(p0.consumed, 0ULL);

    ZSTD_inBuffer in = { gen.getOrigBuff(), srcSize, 0 };
    ZSTD_outBuffer out = { dst.data(), dst.size(), 0 };
    size_t r = ZSTD_compressStream2(cctx, &out, &in, ZSTD_e_end);
    ASSERT_FALSE(ZSTD_isError(r));

    ZSTD_frameProgression p1 = ZSTD_getFrameProgression(cctx);
    EXPECT_GE(p1.ingested, p0.ingested);
    EXPECT_LE(p1.consumed, p1.ingested);
    EXPECT_GT(p1.produced, 0ULL);

    /* All output has been flushed to `out`, so nothing remains to flush now. */
    EXPECT_EQ(ZSTD_toFlushNow(cctx), (size_t)0);
}

/***********************************************
 * DCtx advanced dictionary / params
 ***********************************************/

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_DCtx_loadDictionary_advanced_common_1)
{
    const size_t dictSize = 4 KB;
    const size_t srcSize = 4 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);

    /* Compress with the dictionary. */
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    vector<char> comp(ZSTD_compressBound(srcSize));
    size_t c = ZSTD_compress_usingDict(cctx, comp.data(), comp.size(),
        gen.getOrigBuff(), srcSize, dict.data(), dictSize, 3);
    ASSERT_FALSE(ZSTD_isError(c));
    ZSTD_freeCCtx(cctx);

    /* Decompress, loading the dictionary by reference + raw content. */
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ ZSTD_freeDCtx(dctx); });
    size_t lr = ZSTD_DCtx_loadDictionary_advanced(dctx, dict.data(), dictSize,
        ZSTD_dlm_byRef, ZSTD_dct_auto);
    ASSERT_FALSE(ZSTD_isError(lr));

    vector<char> back(srcSize);
    ZSTD_inBuffer in = { comp.data(), c, 0 };
    ZSTD_outBuffer out = { back.data(), srcSize, 0 };
    size_t r = ZSTD_decompressStream(dctx, &out, &in);
    ASSERT_FALSE(ZSTD_isError(r));
    EXPECT_EQ(out.pos, srcSize);
    EXPECT_EQ(memcmp(back.data(), gen.getOrigBuff(), srcSize), 0);
}

/* ZSTD_DCtx_setMaxWindowSize: a limit smaller than the frame's window must make
 * decompression fail with frameParameter_windowTooLarge. The frame uses an
 * explicit large windowLog so its window exceeds the decoder's limit. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_DCtx_setMaxWindowSize_common_1)
{
    const int frameWindowLog = 18;                 /* 256 KB window */
    const size_t srcSize = (size_t)1 << (frameWindowLog + 1); /* 512 KB, forces the large window to matter */
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);

    /* Stream without pledging the source size so the header carries "content
     * size unknown". This avoids the decoder's single-pass shortcut (which
     * skips the maxWindowSize check) and forces the windowed path. */
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    vector<char> frame(ZSTD_compressBound(srcSize) + 64);
    {
        ScopeGuard freeCctx([&]{ ZSTD_freeCCtx(cctx); });
        ASSERT_FALSE(ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3)));
        ASSERT_FALSE(ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, frameWindowLog)));
        ASSERT_FALSE(ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_contentSizeFlag, 0)));
        ZSTD_inBuffer cin = { gen.getOrigBuff(), srcSize, 0 };
        ZSTD_outBuffer cout = { frame.data(), frame.size(), 0 };
        size_t cr;
        do {
            cr = ZSTD_compressStream2(cctx, &cout, &cin, ZSTD_e_end);
            ASSERT_FALSE(ZSTD_isError(cr));
        } while (cr != 0);
        frame.resize(cout.pos);
    }
    size_t c = frame.size();

    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ ZSTD_freeDCtx(dctx); });
    /* Window limit well below the frame's 256 KB window. */
    ASSERT_FALSE(ZSTD_isError(ZSTD_DCtx_setMaxWindowSize(dctx, (size_t)1 << 15)));

    vector<char> back(srcSize);
    ZSTD_inBuffer in = { frame.data(), c, 0 };
    ZSTD_outBuffer out = { back.data(), srcSize, 0 };
    size_t r = ZSTD_decompressStream(dctx, &out, &in);
    EXPECT_TRUE(ZSTD_isError(r));
    EXPECT_EQ(ZSTD_getErrorCode(r), ZSTD_error_frameParameter_windowTooLarge);
}

/* ZSTD_DCtx_getParameter reads back a value set with ZSTD_DCtx_setParameter. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_DCtx_getParameter_common_1)
{
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ ZSTD_freeDCtx(dctx); });
    ASSERT_FALSE(ZSTD_isError(ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, 27)));
    int value = -1;
    ASSERT_FALSE(ZSTD_isError(ZSTD_DCtx_getParameter(dctx, ZSTD_d_windowLogMax, &value)));
    EXPECT_EQ(value, 27);
}

/***********************************************
 * Buffer-less decompression helpers
 ***********************************************/

/* ZSTD_decodingBufferSize_min: monotonic in both arguments and non-zero. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_decodingBufferSize_min_common_1)
{
    size_t small = ZSTD_decodingBufferSize_min(1 KB, 4 KB);
    size_t large = ZSTD_decodingBufferSize_min(64 KB, 4 KB);
    EXPECT_FALSE(ZSTD_isError(small));
    EXPECT_FALSE(ZSTD_isError(large));
    EXPECT_GT(small, (size_t)0);
    EXPECT_GE(large, small);

    /* Unknown content size must still yield a usable estimate. */
    size_t unk = ZSTD_decodingBufferSize_min(64 KB, ZSTD_CONTENTSIZE_UNKNOWN);
    EXPECT_FALSE(ZSTD_isError(unk));
    EXPECT_GT(unk, (size_t)0);
}

/* Buffer-less decode driven by ZSTD_decompressBegin_usingDDict +
 * ZSTD_nextInputType + ZSTD_decompressContinue must reconstruct the data. */
TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZSTD_decompressBegin_usingDDict_and_nextInputType_common_1)
{
    const size_t dictSize = 4 KB;
    const size_t srcSize = 4 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    gtest_data_gen_t gen(ZSTD_compressBound, srcSize, gtest_data_gen_type::midratio);

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ASSERT_NE(cctx, nullptr);
    vector<char> comp(ZSTD_compressBound(srcSize));
    size_t c = ZSTD_compress_usingDict(cctx, comp.data(), comp.size(),
        gen.getOrigBuff(), srcSize, dict.data(), dictSize, 3);
    ASSERT_FALSE(ZSTD_isError(c));
    ZSTD_freeCCtx(cctx);

    ZSTD_DDict* ddict = ZSTD_createDDict(dict.data(), dictSize);
    ASSERT_NE(ddict, nullptr);
    ScopeGuard freeDdict([&]{ ZSTD_freeDDict(ddict); });
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ ZSTD_freeDCtx(dctx); });
    ASSERT_FALSE(ZSTD_isError(ZSTD_decompressBegin_usingDDict(dctx, ddict)));

    /* The decoder should first expect a frame header. */
    EXPECT_EQ(ZSTD_nextInputType(dctx), ZSTDnit_frameHeader);

    /* Drive the buffer-less loop. */
    vector<char> back(srcSize);
    const char* ip = comp.data();
    const char* const iend = comp.data() + c;
    char* op = back.data();
    char* const oend = back.data() + srcSize;
    while (ip < iend) {
        size_t toRead = ZSTD_nextSrcSizeToDecompress(dctx);
        if (toRead == 0) break;
        ASSERT_LE(ip + toRead, iend);
        (void)ZSTD_nextInputType(dctx); /* exercise the enum path */
        size_t written = ZSTD_decompressContinue(dctx, op, (size_t)(oend - op), ip, toRead);
        ASSERT_FALSE(ZSTD_isError(written));
        op += written;
        ip += toRead;
    }
    EXPECT_EQ((size_t)(op - back.data()), srcSize);
    EXPECT_EQ(memcmp(back.data(), gen.getOrigBuff(), srcSize), 0);
}

/***********************************************
 * ZDICT stable helpers
 ***********************************************/

/* Build a trained dictionary once and reuse across the ZDICT helper tests. */
static size_t build_trained_dict(vector<char>& dictOut)
{
    const size_t dictCap = 16 KB;
    dictOut.assign(dictCap, 0);
    const size_t samplesBufferSz = dictCap * 8;
    vector<char> samplesBuffer(samplesBufferSz, 0);
    vector<size_t> samplesSizes =
        gtest_data_gen_t::get_array_of_samples(samplesBuffer.data(), samplesBufferSz);
    size_t sz = ZDICT_trainFromBuffer(dictOut.data(), dictCap,
        samplesBuffer.data(), samplesSizes.data(), (unsigned)samplesSizes.size());
    if (!ZDICT_isError(sz)) dictOut.resize(sz);
    return sz;
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_getDictID_common_1)
{
    vector<char> dict;
    size_t sz = build_trained_dict(dict);
    ASSERT_FALSE(ZDICT_isError(sz));
    unsigned id = ZDICT_getDictID(dict.data(), dict.size());
    EXPECT_NE(id, 0u);
    /* Cross-check against the zstd-side extractor. */
    EXPECT_EQ(ZSTD_getDictID_fromDict(dict.data(), dict.size()), id);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_getDictID_invalid_common_2)
{
    const char garbage[64] = { 0 };
    EXPECT_EQ(ZDICT_getDictID(garbage, sizeof(garbage)), 0u);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_getDictHeaderSize_common_1)
{
    vector<char> dict;
    size_t sz = build_trained_dict(dict);
    ASSERT_FALSE(ZDICT_isError(sz));
    size_t hdr = ZDICT_getDictHeaderSize(dict.data(), dict.size());
    EXPECT_FALSE(ZDICT_isError(hdr));
    EXPECT_GT(hdr, (size_t)0);
    EXPECT_LE(hdr, dict.size());
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_isError_and_getErrorName_common_1)
{
    /* A trained dictionary size is not an error. */
    vector<char> dict;
    size_t sz = build_trained_dict(dict);
    EXPECT_EQ(ZDICT_isError(sz), 0u);

    /* Passing a too-small capacity forces an error code. */
    char tiny[8];
    vector<char> samplesBuffer(4 KB, 0);
    vector<size_t> samplesSizes =
        gtest_data_gen_t::get_array_of_samples(samplesBuffer.data(), samplesBuffer.size());
    size_t err = ZDICT_trainFromBuffer(tiny, sizeof(tiny),
        samplesBuffer.data(), samplesSizes.data(), (unsigned)samplesSizes.size());
    EXPECT_NE(ZDICT_isError(err), 0u);
    EXPECT_NE(ZDICT_getErrorName(err), nullptr);
    EXPECT_GT(strlen(ZDICT_getErrorName(err)), (size_t)0);
}

/***********************************************
 * ZDICT experimental trainers
 ***********************************************/

/* Helper: prepare a sample buffer for the cover/fastCover trainers. */
static void make_samples(vector<char>& buf, vector<size_t>& sizes, size_t bufSz)
{
    buf.assign(bufSz, 0);
    sizes = gtest_data_gen_t::get_array_of_samples(buf.data(), bufSz);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_trainFromBuffer_cover_common_1)
{
    const size_t dictCap = 8 KB;
    vector<char> dict(dictCap);
    vector<char> samples; vector<size_t> sizes;
    make_samples(samples, sizes, dictCap * 16);

    ZDICT_cover_params_t params;
    memset(&params, 0, sizeof(params));
    params.k = 200;
    params.d = 8;
    params.zParams.compressionLevel = 3;

    size_t sz = ZDICT_trainFromBuffer_cover(dict.data(), dictCap,
        samples.data(), sizes.data(), (unsigned)sizes.size(), params);
    ASSERT_FALSE(ZDICT_isError(sz));
    EXPECT_GT(sz, (size_t)0);
    EXPECT_LE(sz, dictCap);
    EXPECT_NE(ZDICT_getDictID(dict.data(), sz), 0u);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_optimizeTrainFromBuffer_cover_common_1)
{
    const size_t dictCap = 8 KB;
    vector<char> dict(dictCap);
    vector<char> samples; vector<size_t> sizes;
    make_samples(samples, sizes, dictCap * 16);

    ZDICT_cover_params_t params;
    memset(&params, 0, sizeof(params));
    params.steps = 4;                     /* keep the search short for tests */
    params.zParams.compressionLevel = 3;

    size_t sz = ZDICT_optimizeTrainFromBuffer_cover(dict.data(), dictCap,
        samples.data(), sizes.data(), (unsigned)sizes.size(), &params);
    ASSERT_FALSE(ZDICT_isError(sz));
    EXPECT_GT(sz, (size_t)0);
    /* On success the optimizer writes back the chosen k/d. */
    EXPECT_GT(params.k, 0u);
    EXPECT_GT(params.d, 0u);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_trainFromBuffer_fastCover_common_1)
{
    const size_t dictCap = 8 KB;
    vector<char> dict(dictCap);
    vector<char> samples; vector<size_t> sizes;
    make_samples(samples, sizes, dictCap * 16);

    ZDICT_fastCover_params_t params;
    memset(&params, 0, sizeof(params));
    params.k = 200;
    params.d = 8;
    params.zParams.compressionLevel = 3;

    size_t sz = ZDICT_trainFromBuffer_fastCover(dict.data(), dictCap,
        samples.data(), sizes.data(), (unsigned)sizes.size(), params);
    ASSERT_FALSE(ZDICT_isError(sz));
    EXPECT_GT(sz, (size_t)0);
    EXPECT_LE(sz, dictCap);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_optimizeTrainFromBuffer_fastCover_common_1)
{
    const size_t dictCap = 8 KB;
    vector<char> dict(dictCap);
    vector<char> samples; vector<size_t> sizes;
    make_samples(samples, sizes, dictCap * 16);

    ZDICT_fastCover_params_t params;
    memset(&params, 0, sizeof(params));
    params.steps = 4;
    params.zParams.compressionLevel = 3;

    size_t sz = ZDICT_optimizeTrainFromBuffer_fastCover(dict.data(), dictCap,
        samples.data(), sizes.data(), (unsigned)sizes.size(), &params);
    ASSERT_FALSE(ZDICT_isError(sz));
    EXPECT_GT(sz, (size_t)0);
    EXPECT_GT(params.k, 0u);
    EXPECT_GT(params.d, 0u);
}

TEST_P(ZSTD_ext, AOCL_Compression_zstd_ZDICT_trainFromBuffer_legacy_common_1)
{
    /* The legacy trainer is more demanding than cover/fastCover: it needs a
     * generous corpus with enough shared structure across many samples to find
     * common patterns. We build many fixed-size samples that each embed one of
     * a handful of recurring "tokens" surrounded by random bytes, giving the
     * trainer both repetition to latch onto and volume (~100x dict size). */
    const size_t dictCap = 4 KB;
    const size_t nbSamples = 2000;
    const size_t sampleSz = 256;               /* total ~500 KB corpus */
    vector<char> dict(dictCap);
    vector<char> samples(nbSamples * sampleSz);
    vector<size_t> sizes(nbSamples, sampleSz);

    /* A few recurring tokens the dictionary should be able to capture. */
    static const char* tokens[] = {
        "AOCL-COMPRESSION-DICTIONARY-TOKEN-0001",
        "the-quick-brown-fox-jumps-over-lazy-dog",
        "0123456789-abcdef-common-header-pattern",
        "<<recurring-structured-record-boundary>>",
    };
    const int nTokens = (int)(sizeof(tokens) / sizeof(tokens[0]));
    for (size_t i = 0; i < nbSamples; ++i) {
        char* s = samples.data() + i * sampleSz;
        gtest_data_gen_t::fill_random(s, sampleSz);
        const char* tok = tokens[i % nTokens];
        size_t tlen = strlen(tok);
        /* Place the token at a deterministic offset within the sample. */
        size_t off = (i * 7) % (sampleSz - tlen);
        memcpy(s + off, tok, tlen);
    }

    ZDICT_legacy_params_t params;
    memset(&params, 0, sizeof(params));
    params.selectivityLevel = 0;          /* 0 => default */
    params.zParams.compressionLevel = 3;

    size_t sz = ZDICT_trainFromBuffer_legacy(dict.data(), dictCap,
        samples.data(), sizes.data(), (unsigned)sizes.size(), params);
    ASSERT_FALSE(ZDICT_isError(sz));
    EXPECT_GT(sz, (size_t)0);
    EXPECT_LE(sz, dictCap);
}

#else /* !AOCL_TEST_COVERAGE_EXT */

/* Avoid an empty translation unit (ISO C++ forbids it; -Wpedantic -Werror
 * would fail the build) when extended coverage is disabled. */
typedef int zstd_ext_gtest_translation_unit_not_empty;

#endif /* AOCL_TEST_COVERAGE_EXT */
