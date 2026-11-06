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

 /** @file lz4hc_ext_gtest.cpp
 *
 *  @brief Extended test cases for LZ4 HC APIs.
 *
 *  Added under the AOCL_TEST_COVERAGE_EXT user option to broaden test coverage
 *  of non-deprecated experimental LZ4 HC APIs beyond the base suite:
 *  LZ4_setCompressionLevel, LZ4_favorDecompressionSpeed and the AOCL variant
 *  AOCL_LZ4_setCompressionLevel.
 */

#ifdef AOCL_TEST_COVERAGE_EXT

#include <cstring>
#include <vector>
#include <functional>
#include "gtest/gtest.h"

#ifndef LZ4_STATIC_LINKING_ONLY
#define LZ4_STATIC_LINKING_ONLY
#endif
#include "algos/lz4/lz4.h"

#ifndef LZ4_HC_STATIC_LINKING_ONLY
#define LZ4_HC_STATIC_LINKING_ONLY
#endif
#include "algos/lz4/lz4hc.h"

#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

#include "gtest_utils.h"

using namespace std;

/* KB and ScopeGuard are provided by gtest_utils.h. */

/* These are experimental (LZ4_HC_STATIC_LINKING_ONLY) APIs, not exported from
 * the shared library on Windows, so the whole suite is gated behind !_WINDOWS
 * per project convention. */
#ifndef _WINDOWS

/* Fixture parametrized over the dynamic-dispatcher flags, like the base LZ4HC
 * suites: each case runs once per {optOff, optLevel} pair, exercising the
 * reference (opt-off) path and every supported ISA level. */
class LZ4HC_ext : public OPT_LEVEL_TESTS {
public:
    LZ4HC_ext() {
        aocl_destroy_lz4hc();
        DynamicDispatch opt = GetParam();
        aocl_setup_lz4hc(opt.optOff, opt.optLevel, 0, 0, 0);
    }
    ~LZ4HC_ext() {
        aocl_destroy_lz4hc();
    }
};

/* One instantiation drives every LZ4HC_ext TEST_P across all dispatcher flags. */
INSTANTIATE_TEST_SUITE_P(
    LZ4HC,
    LZ4HC_ext,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/* Compress `src` (single block) through an HC stream reset then set to `level`
 * via LZ4_setCompressionLevel. Returns compressed size, or <=0 on failure. */
static int hc_stream_compress_with_level(int level, const char* src, int srcSize,
    char* dst, int dstCapacity, bool favorDecSpeed = false)
{
    LZ4_streamHC_t* s = LZ4_createStreamHC();
    if (!s) return -1;
    LZ4_resetStreamHC_fast(s, LZ4HC_CLEVEL_DEFAULT);
    LZ4_setCompressionLevel(s, level);
    if (favorDecSpeed)
        LZ4_favorDecompressionSpeed(s, 1);
    int clen = LZ4_compress_HC_continue(s, src, dst, srcSize, dstCapacity);
    LZ4_freeStreamHC(s);
    return clen;
}

/***********************************************
 * LZ4_setCompressionLevel tests
 ***********************************************/

/* Round-trip: a stream whose level is set via LZ4_setCompressionLevel must
 * still produce output that decodes back to the original. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_LZ4_setCompressionLevel_roundtrip_common_1)
{
    const int srcSize = 4 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(srcSize);
    vector<char> dst(bound);

    int clen = hc_stream_compress_with_level(LZ4HC_CLEVEL_MAX, src, srcSize,
        dst.data(), bound);
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize);
    ASSERT_EQ(LZ4_decompress_safe(dst.data(), out.data(), clen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* Both min and max levels must produce decodable output. LZ4HC does not
 * guarantee that a higher level always yields smaller-or-equal output for
 * arbitrary input, so the size relationship is only checked with generous
 * slack (highly-compressible data) rather than asserted strictly. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_LZ4_setCompressionLevel_higher_not_worse_common_2)
{
    const int srcSize = 8 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::lowratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(srcSize);
    vector<char> low(bound), high(bound);

    int lowLen = hc_stream_compress_with_level(LZ4HC_CLEVEL_MIN, src, srcSize, low.data(), bound);
    int highLen = hc_stream_compress_with_level(LZ4HC_CLEVEL_MAX, src, srcSize, high.data(), bound);
    ASSERT_GT(lowLen, 0);
    ASSERT_GT(highLen, 0);

    /* Both outputs must round-trip. */
    vector<char> out(srcSize);
    ASSERT_EQ(LZ4_decompress_safe(low.data(), out.data(), lowLen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
    ASSERT_EQ(LZ4_decompress_safe(high.data(), out.data(), highLen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);

    /* The max level should not be dramatically worse than the min level. */
    EXPECT_LE(highLen, lowLen + lowLen / 10 + 16);
}

/* A level below the minimum is clamped rather than rejected; the stream stays
 * usable and round-trips. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_LZ4_setCompressionLevel_below_min_clamped_common_3)
{
    const int srcSize = 2 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(srcSize);
    vector<char> dst(bound);

    int clen = hc_stream_compress_with_level(0, src, srcSize, dst.data(), bound);
    ASSERT_GT(clen, 0);
    vector<char> out(srcSize);
    ASSERT_EQ(LZ4_decompress_safe(dst.data(), out.data(), clen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* A level above the maximum is clamped to LZ4HC_CLEVEL_MAX; result matches a
 * stream explicitly set to the max level. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_LZ4_setCompressionLevel_above_max_clamped_common_4)
{
    const int srcSize = 4 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(srcSize);
    vector<char> aboveMax(bound), atMax(bound);

    int aboveLen = hc_stream_compress_with_level(LZ4HC_CLEVEL_MAX + 10, src, srcSize, aboveMax.data(), bound);
    int maxLen = hc_stream_compress_with_level(LZ4HC_CLEVEL_MAX, src, srcSize, atMax.data(), bound);
    ASSERT_GT(aboveLen, 0);
    ASSERT_GT(maxLen, 0);
    EXPECT_EQ(aboveLen, maxLen);
}

/* Changing the level mid-stream (between blocks) is permitted and both blocks
 * decode correctly. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_LZ4_setCompressionLevel_change_between_blocks_common_5)
{
    const int blk = 2 KB;
    gtest_data_gen_t gen(LZ4_compressBound, 2 * blk, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(blk);
    vector<char> c1(bound), c2(bound);

    LZ4_streamHC_t* s = LZ4_createStreamHC();
    ASSERT_NE(s, nullptr);
    LZ4_resetStreamHC_fast(s, LZ4HC_CLEVEL_DEFAULT);

    LZ4_setCompressionLevel(s, LZ4HC_CLEVEL_MIN);
    int l1 = LZ4_compress_HC_continue(s, src, c1.data(), blk, bound);
    LZ4_setCompressionLevel(s, LZ4HC_CLEVEL_MAX);
    int l2 = LZ4_compress_HC_continue(s, src + blk, c2.data(), blk, bound);
    LZ4_freeStreamHC(s);
    ASSERT_GT(l1, 0);
    ASSERT_GT(l2, 0);

    /* Decode as a linked stream (block 2 references block 1). */
    LZ4_streamDecode_t* d = LZ4_createStreamDecode();
    ASSERT_NE(d, nullptr);
    ScopeGuard freeD([&]{ LZ4_freeStreamDecode(d); });
    vector<char> out(2 * blk);
    ASSERT_EQ(LZ4_decompress_safe_continue(d, c1.data(), out.data(), l1, blk), blk);
    ASSERT_EQ(LZ4_decompress_safe_continue(d, c2.data(), out.data() + blk, l2, blk), blk);
    EXPECT_EQ(memcmp(out.data(), src, 2 * blk), 0);
}

/***********************************************
 * LZ4_favorDecompressionSpeed tests
 ***********************************************/

/* With favor flag enabled at an opt-parser level, output still round-trips. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_LZ4_favorDecompressionSpeed_roundtrip_common_1)
{
    const int srcSize = 8 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(srcSize);
    vector<char> dst(bound);

    /* LZ4HC_CLEVEL_OPT_MIN is where the optimal parser (and thus the favor
     * flag) takes effect. */
    int clen = hc_stream_compress_with_level(LZ4HC_CLEVEL_OPT_MIN, src, srcSize,
        dst.data(), bound, /*favorDecSpeed=*/true);
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize);
    ASSERT_EQ(LZ4_decompress_safe(dst.data(), out.data(), clen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* Disabling favor (favor = 0) is also valid and round-trips. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_LZ4_favorDecompressionSpeed_disabled_common_2)
{
    const int srcSize = 8 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(srcSize);
    vector<char> dst(bound);

    LZ4_streamHC_t* s = LZ4_createStreamHC();
    ASSERT_NE(s, nullptr);
    LZ4_resetStreamHC_fast(s, LZ4HC_CLEVEL_OPT_MIN);
    LZ4_favorDecompressionSpeed(s, 0);
    int clen = LZ4_compress_HC_continue(s, src, dst.data(), srcSize, bound);
    LZ4_freeStreamHC(s);
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize);
    ASSERT_EQ(LZ4_decompress_safe(dst.data(), out.data(), clen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

#ifdef AOCL_LZ4HC_OPT
/***********************************************
 * AOCL_LZ4_setCompressionLevel tests
 ***********************************************/

/* The AOCL variant stores the requested level in the stream's internal state.
 * Its layout differs from native LZ4_streamHC_t, so validate the setter via the
 * internal field rather than a compress-continue over a reinterpreted pointer. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_AOCL_LZ4_setCompressionLevel_stores_level_common_1)
{
    AOCL_LZ4_streamHC_t* s = AOCL_LZ4_createStreamHC();
    ASSERT_NE(s, nullptr);
    AOCL_LZ4_resetStreamHC_fast(s, LZ4HC_CLEVEL_DEFAULT);
    AOCL_LZ4_setCompressionLevel(s, LZ4HC_CLEVEL_MAX);
    EXPECT_EQ(s->internal_donotuse.compressionLevel, LZ4HC_CLEVEL_MAX);
    AOCL_LZ4_freeStreamHC(s);
}

/* Level can be changed again on the same AOCL stream. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_AOCL_LZ4_setCompressionLevel_change_common_2)
{
    AOCL_LZ4_streamHC_t* s = AOCL_LZ4_createStreamHC();
    ASSERT_NE(s, nullptr);
    AOCL_LZ4_resetStreamHC_fast(s, LZ4HC_CLEVEL_DEFAULT);
    AOCL_LZ4_setCompressionLevel(s, LZ4HC_CLEVEL_MIN);
    EXPECT_EQ(s->internal_donotuse.compressionLevel, LZ4HC_CLEVEL_MIN);
    AOCL_LZ4_setCompressionLevel(s, LZ4HC_CLEVEL_MAX);
    EXPECT_EQ(s->internal_donotuse.compressionLevel, LZ4HC_CLEVEL_MAX);
    AOCL_LZ4_freeStreamHC(s);
}

/* Behavioral coverage for the AOCL HC stream (the field-only tests above assert
 * a struct member, not compression behaviour). This drives real compression
 * through the AOCL (CEHC) stream at two levels and confirms each round-trips and
 * that the level actually changes the emitted bytes. The AOCL_LZ4_streamHC_t
 * path is only selected for levels 6..9 with optimizations enabled, so this uses
 * those endpoints (the min/max of the CEHC-applicable range) and runs only for
 * the opt-on parameter; opt-off uses the native stream, already covered by the
 * LZ4_setCompressionLevel cases above. The public AOCL compression entry point
 * is LZ4_compress_HC_extStateHC(), which dispatches to the AOCL variant for this
 * level range when the AOCL stream is passed as the state buffer. */
TEST_P(LZ4HC_ext, AOCL_Compression_lz4hc_AOCL_LZ4_setCompressionLevel_roundtrip_common_3)
{
    DynamicDispatch opt = GetParam();
    if (opt.optOff) GTEST_SKIP(); /* AOCL/CEHC path only runs with optimizations on. */

    const int loLevel = 6;   /* CEHC range lower bound */
    const int hiLevel = 9;   /* CEHC range upper bound */
    const int srcSize = 32 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    int bound = LZ4_compressBound(srcSize);
    vector<char> lo(bound), hi(bound);

    AOCL_LZ4_streamHC_t* s = AOCL_LZ4_createStreamHC();
    ASSERT_NE(s, nullptr);
    ScopeGuard freeStream([&]{ AOCL_LZ4_freeStreamHC(s); });

    AOCL_LZ4_resetStreamHC_fast(s, LZ4HC_CLEVEL_DEFAULT);
    AOCL_LZ4_setCompressionLevel(s, loLevel);
    ASSERT_EQ(s->internal_donotuse.compressionLevel, loLevel);
    int loLen = LZ4_compress_HC_extStateHC(s, src, lo.data(), srcSize, bound, loLevel);

    AOCL_LZ4_resetStreamHC_fast(s, LZ4HC_CLEVEL_DEFAULT);
    AOCL_LZ4_setCompressionLevel(s, hiLevel);
    ASSERT_EQ(s->internal_donotuse.compressionLevel, hiLevel);
    int hiLen = LZ4_compress_HC_extStateHC(s, src, hi.data(), srcSize, bound, hiLevel);

    ASSERT_GT(loLen, 0);
    ASSERT_GT(hiLen, 0);

    /* Both levels must round-trip to the original. */
    vector<char> out(srcSize);
    ASSERT_EQ(LZ4_decompress_safe(lo.data(), out.data(), loLen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
    ASSERT_EQ(LZ4_decompress_safe(hi.data(), out.data(), hiLen, srcSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);

    /* The level must have an observable effect: the two encodings differ in
     * length or in content. */
    bool differ = (loLen != hiLen) ||
        (memcmp(lo.data(), hi.data(), (size_t)loLen) != 0);
    EXPECT_TRUE(differ);
}
#endif /* AOCL_LZ4HC_OPT */

#endif /* !_WINDOWS */

#else /* !AOCL_TEST_COVERAGE_EXT */

/* Avoid an empty translation unit (ISO C++ forbids it; -Wpedantic -Werror
 * would fail the build) when extended coverage is disabled. */
typedef int lz4hc_ext_gtest_translation_unit_not_empty;

#endif /* AOCL_TEST_COVERAGE_EXT */
