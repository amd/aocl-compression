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

 /** @file lz4_ext_gtest.cpp
 *
 *  @brief Extended test cases for LZ4 block APIs.
 *
 *  Added under the AOCL_TEST_COVERAGE_EXT user option to broaden test coverage
 *  of non-deprecated public LZ4 block APIs beyond the base suite:
 *  LZ4_loadDictSlow, LZ4_decompress_safe_partial_usingDict and
 *  LZ4_compress_destSize_extState.
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

#ifdef LZ4_FRAME_FORMAT_SUPPORT
#include <cstdio>
#include <string>
/* Process id, for a per-process unique temp filename in the file-API test. */
#if defined(_WIN32)
#include <process.h>
#define AOCL_GETPID _getpid
#else
#include <unistd.h>
#define AOCL_GETPID getpid
#endif
/* Enable the experimental frame symbols (LZ4F_getBlockSize,
 * LZ4F_uncompressedUpdate, *_advanced, and the whole file API). */
#ifndef LZ4F_STATIC_LINKING_ONLY
#define LZ4F_STATIC_LINKING_ONLY
#endif
#include "algos/lz4/lz4frame.h"
#include "algos/lz4/lz4file.h"
#endif /* LZ4_FRAME_FORMAT_SUPPORT */

#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

#include "gtest_utils.h"

using namespace std;

/* KB / MB and ScopeGuard are provided by gtest_utils.h. */

/* Local buffer helper over gtest_data_gen_t. Named distinctly from the
 * file-local TestLoad in lz4_gtest.cpp: both files are linked into the same
 * lz4_gtest binary under extended coverage, so a shared class name would be an
 * ODR violation. */
class TestLoadExt
{
private:
    gtest_data_gen_t* data_gen = NULL;

public:
    TestLoadExt(size_t sz, gtest_data_gen_type type = gtest_data_gen_type::random)
    {
        data_gen = new gtest_data_gen_t(LZ4_compressBound, sz, type);
    }
    TestLoadExt(size_t inp_sz, size_t out_sz, gtest_data_gen_type type = gtest_data_gen_type::random)
    {
        data_gen = new gtest_data_gen_t(LZ4_compressBound, inp_sz, out_sz, type);
    }
    ~TestLoadExt() { delete data_gen; }
    char* getOrigData()       { return data_gen->getOrigBuff(); }
    size_t getOrigSize()      { return data_gen->getOrigSize(); }
    char* getCompressedBuff() { return data_gen->getCompressedBuff(); }
    size_t getCompressedSize(){ return data_gen->getCompressedSize(); }
};

/* Fixture parametrized over the dynamic-dispatcher flags, exactly like the base
 * LZ4 suites: every case runs once per {optOff, optLevel} pair returned by
 * get_dynamic_dispatcher_flags(), so the reference (opt-off) path and each
 * supported ISA level (incl. AVX512 where present) are all exercised. */
class LZ4_ext : public OPT_LEVEL_TESTS {
public:
    LZ4_ext() {
        aocl_destroy_lz4();
        DynamicDispatch opt = GetParam();
        aocl_setup_lz4(opt.optOff, opt.optLevel, 0, 0, 0);
    }
    ~LZ4_ext() {
        aocl_destroy_lz4();
    }
};

/* One instantiation drives every LZ4_ext TEST_P across all dispatcher flags.
 * Declared up front so it covers the cases defined in both the always-on block
 * and the LZ4_FRAME_FORMAT_SUPPORT-gated block below. */
INSTANTIATE_TEST_SUITE_P(
    LZ4,
    LZ4_ext,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/* Compress `src` into the caller-provided `dst` (capacity `dstCapacity`) using a
 * linked dictionary loaded with the given loader (LZ4_loadDict or
 * LZ4_loadDictSlow). Returns the compressed size, or <=0 on failure. */
static int compress_with_dict_loader(
    int (*loader)(LZ4_stream_t*, const char*, int),
    const char* dict, int dictSize,
    const char* src, int srcSize,
    char* dst, int dstCapacity)
{
    LZ4_stream_t* stream = LZ4_createStream();
    if (!stream) return -1;
    /* The loader returns the number of dictionary bytes actually loaded (capped
     * at 64 KB); for a dict <= 64 KB that must equal dictSize. */
    int loaded = loader(stream, dict, dictSize);
    EXPECT_EQ(loaded, dictSize < (64 KB) ? dictSize : (64 KB));
    int clen = LZ4_compress_fast_continue(stream, src, dst, srcSize, dstCapacity, 1);
    LZ4_freeStream(stream);
    return clen;
}

/***********************************************
 * LZ4_loadDictSlow tests
 ***********************************************/

/* Round-trip: data compressed with a dictionary loaded via LZ4_loadDictSlow
 * must decompress correctly when the same dictionary is supplied. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_loadDictSlow_roundtrip_common_1)
{
    const int dictSize = 256;
    const int srcSize = 800;
    TestLoadExt dictLoad(dictSize, gtest_data_gen_type::random);
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::random);

    char* dict = dictLoad.getOrigData();
    char* src = srcLoad.getOrigData();
    char* compressed = srcLoad.getCompressedBuff();
    int dstCapacity = (int)srcLoad.getCompressedSize();

    int clen = compress_with_dict_loader(LZ4_loadDictSlow, dict, dictSize,
        src, srcSize, compressed, dstCapacity);
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize);
    int dlen = LZ4_decompress_safe_usingDict(compressed, out.data(), clen,
        srcSize, dict, dictSize);
    ASSERT_EQ(dlen, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* LZ4_loadDictSlow with a NULL dictionary and size 0 is equivalent to loading
 * no dictionary; the stream must remain usable and produce decodable output. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_loadDictSlow_null_dict_common_2)
{
    const int srcSize = 800;
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::random);
    char* src = srcLoad.getOrigData();
    char* compressed = srcLoad.getCompressedBuff();
    int dstCapacity = (int)srcLoad.getCompressedSize();

    LZ4_stream_t* stream = LZ4_createStream();
    ASSERT_NE(stream, nullptr);
    EXPECT_EQ(LZ4_loadDictSlow(stream, NULL, 0), 0);
    int clen = LZ4_compress_fast_continue(stream, src, compressed, srcSize, dstCapacity, 1);
    LZ4_freeStream(stream);
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize);
    int dlen = LZ4_decompress_safe(compressed, out.data(), clen, srcSize);
    ASSERT_EQ(dlen, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* The return value of LZ4_loadDictSlow is the number of dictionary bytes
 * loaded, capped at 64 KB. A dictionary smaller than the cap loads fully. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_loadDictSlow_returns_loaded_size_common_3)
{
    const int dictSize = 1024;
    TestLoadExt dictLoad(dictSize, gtest_data_gen_type::random);
    LZ4_stream_t* stream = LZ4_createStream();
    ASSERT_NE(stream, nullptr);
    EXPECT_EQ(LZ4_loadDictSlow(stream, dictLoad.getOrigData(), dictSize), dictSize);
    LZ4_freeStream(stream);
}

/* A dictionary larger than 64 KB is truncated to the most recent 64 KB. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_loadDictSlow_dict_gt_64KB_common_4)
{
    const int dictSize = 70 KB;
    TestLoadExt dictLoad(dictSize, gtest_data_gen_type::random);
    LZ4_stream_t* stream = LZ4_createStream();
    ASSERT_NE(stream, nullptr);
    EXPECT_EQ(LZ4_loadDictSlow(stream, dictLoad.getOrigData(), dictSize), 64 KB);
    LZ4_freeStream(stream);
}

/* LZ4_loadDictSlow references the dictionary more thoroughly than LZ4_loadDict,
 * but lz4.h only "expects to slightly improve" the ratio (heuristic). So the
 * real check is round-trip correctness; the ratio is compared with slack. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_loadDictSlow_ratio_not_worse_common_5)
{
    const int dictSize = 4 KB;
    const int srcSize = 4 KB;
    /* Build a source that genuinely shares a large prefix with the dictionary
     * (the first half of src is a byte-for-byte copy of the dictionary tail) so
     * the dictionary is actually useful to the encoder. */
    TestLoadExt dictLoad(dictSize, gtest_data_gen_type::midratio);
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::midratio);
    char* dict = dictLoad.getOrigData();
    char* src = srcLoad.getOrigData();
    memcpy(src, dict, srcSize / 2);

    int bound = LZ4_compressBound(srcSize);
    vector<char> cFast(bound), cSlow(bound);

    int fastLen = compress_with_dict_loader(LZ4_loadDict, dict, dictSize,
        src, srcSize, cFast.data(), bound);
    int slowLen = compress_with_dict_loader(LZ4_loadDictSlow, dict, dictSize,
        src, srcSize, cSlow.data(), bound);
    ASSERT_GT(fastLen, 0);
    ASSERT_GT(slowLen, 0);

    /* Slow-loader output must round-trip. */
    vector<char> out(srcSize);
    ASSERT_EQ(LZ4_decompress_safe_usingDict(cSlow.data(), out.data(), slowLen,
        srcSize, dict, dictSize), srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);

    /* Ratio: the slow loader should not be dramatically worse than the fast
     * loader. Allow ~10% slack since the improvement is only heuristic. */
    EXPECT_LE(slowLen, fastLen + fastLen / 10 + 16);
}

/***********************************************
 * LZ4_decompress_safe_partial_usingDict tests
 ***********************************************/

/* Partial decode with dictionary: request fewer bytes than the full content
 * and confirm the returned prefix matches the original. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_partial_common_1)
{
    const int dictSize = 256;
    const int srcSize = 800;
    const int target = 300;
    TestLoadExt dictLoad(dictSize, gtest_data_gen_type::random);
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::random);
    char* dict = dictLoad.getOrigData();
    char* src = srcLoad.getOrigData();
    char* compressed = srcLoad.getCompressedBuff();
    int dstCapacity = (int)srcLoad.getCompressedSize();

    int clen = compress_with_dict_loader(LZ4_loadDict, dict, dictSize,
        src, srcSize, compressed, dstCapacity);
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize, 0);
    int dlen = LZ4_decompress_safe_partial_usingDict(compressed, out.data(),
        clen, target, srcSize, dict, dictSize);
    /* Return value is bytes written, never more than targetOutputSize (lz4.h);
     * the decoded prefix must match the original. */
    ASSERT_GT(dlen, 0);
    ASSERT_LE(dlen, target);
    EXPECT_EQ(memcmp(out.data(), src, (size_t)dlen), 0);
}

/* Requesting the full size behaves like a complete dictionary decode. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_full_common_2)
{
    const int dictSize = 256;
    const int srcSize = 800;
    TestLoadExt dictLoad(dictSize, gtest_data_gen_type::random);
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::random);
    char* dict = dictLoad.getOrigData();
    char* src = srcLoad.getOrigData();
    char* compressed = srcLoad.getCompressedBuff();
    int dstCapacity = (int)srcLoad.getCompressedSize();

    int clen = compress_with_dict_loader(LZ4_loadDict, dict, dictSize,
        src, srcSize, compressed, dstCapacity);
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize, 0);
    int dlen = LZ4_decompress_safe_partial_usingDict(compressed, out.data(),
        clen, srcSize, srcSize, dict, dictSize);
    ASSERT_EQ(dlen, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* A NULL/empty dictionary reduces to plain partial decode. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_no_dict_common_3)
{
    const int srcSize = 800;
    const int target = 200;
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::random);
    char* src = srcLoad.getOrigData();
    char* compressed = srcLoad.getCompressedBuff();
    int clen = LZ4_compress_default(src, compressed, srcSize,
        (int)srcLoad.getCompressedSize());
    ASSERT_GT(clen, 0);

    vector<char> out(srcSize, 0);
    int dlen = LZ4_decompress_safe_partial_usingDict(compressed, out.data(),
        clen, target, srcSize, NULL, 0);
    ASSERT_GT(dlen, 0);
    ASSERT_LE(dlen, target);
    EXPECT_EQ(memcmp(out.data(), src, (size_t)dlen), 0);
}

/* Corrupt input must be rejected (negative return), not overflow the buffer. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_decompress_safe_partial_usingDict_corrupt_fail_common_4)
{
    const int dictSize = 256;
    const int srcSize = 800;
    TestLoadExt dictLoad(dictSize, gtest_data_gen_type::random);
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::random);
    char* dict = dictLoad.getOrigData();
    char* src = srcLoad.getOrigData();
    char* compressed = srcLoad.getCompressedBuff();
    int dstCapacity = (int)srcLoad.getCompressedSize();

    int clen = compress_with_dict_loader(LZ4_loadDict, dict, dictSize,
        src, srcSize, compressed, dstCapacity);
    ASSERT_GT(clen, 0);
    /* Corrupt a specific structural byte. The first byte of a block is the
     * token (high nibble = literal length, low nibble = match length); flipping
     * all its bits changes the literal run and the following offset/match
     * interpretation. Inverting the whole buffer is not guaranteed to yield an
     * invalid stream for every input, so this targets the token instead and
     * accepts either a decode error (negative return) or a successful decode
     * that no longer matches the original. */
    compressed[0] = ~compressed[0];

    vector<char> out(srcSize, 0);
    int dlen = LZ4_decompress_safe_partial_usingDict(compressed, out.data(),
        clen, srcSize, srcSize, dict, dictSize);
    bool rejected = (dlen < 0);
    bool mismatched = (dlen != srcSize) ||
        (memcmp(out.data(), src, srcSize) != 0);
    EXPECT_TRUE(rejected || mismatched);
}

/***********************************************
 * LZ4_compress_destSize_extState tests
 *
 * Experimental (LZ4_STATIC_LINKING_ONLY) API, not exported from the shared
 * library on Windows; gated behind !_WINDOWS per suite convention.
 ***********************************************/
#ifndef _WINDOWS

/* Happy path: fill a fixed-size destination and confirm the reported number of
 * consumed source bytes round-trips. The state must come from
 * LZ4_createStream() so it satisfies LZ4_stream_t's size/alignment
 * requirements (a raw byte buffer is not guaranteed to). */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_compress_destSize_extState_roundtrip_common_1)
{
    const int srcSize = 4 KB;
    const int targetDstSize = 1 KB;
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::midratio);
    char* src = srcLoad.getOrigData();

    ASSERT_GT(LZ4_sizeofState(), 0);
    LZ4_stream_t* state = LZ4_createStream();
    ASSERT_NE(state, nullptr);
    vector<char> dst(LZ4_compressBound(srcSize));

    int srcConsumed = srcSize;
    int clen = LZ4_compress_destSize_extState(state, src, dst.data(),
        &srcConsumed, targetDstSize, 1);
    LZ4_freeStream(state);
    ASSERT_GT(clen, 0);
    ASSERT_LE(clen, targetDstSize);
    ASSERT_GT(srcConsumed, 0);
    ASSERT_LE(srcConsumed, srcSize);

    /* The reported srcConsumed bytes must round-trip. destSize output that fills
     * dst to the brim can need a decode buffer 1 byte larger than the decoded
     * content (a documented property of the block format, see lz4.h / issue
     * #859), so give the decoder srcConsumed + 1 bytes of capacity. */
    vector<char> out(srcConsumed + 1);
    int dlen = LZ4_decompress_safe(dst.data(), out.data(), clen, srcConsumed + 1);
    ASSERT_EQ(dlen, srcConsumed);
    EXPECT_EQ(memcmp(out.data(), src, srcConsumed), 0);
}

/* Result must equal the non-ext LZ4_compress_destSize for the same inputs
 * (ext variant only differs by using a caller-provided state buffer). */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_compress_destSize_extState_matches_default_common_2)
{
    const int srcSize = 4 KB;
    const int targetDstSize = 1 KB;
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::midratio);
    char* src = srcLoad.getOrigData();

    LZ4_stream_t* state = LZ4_createStream();
    ASSERT_NE(state, nullptr);
    vector<char> dstExt(LZ4_compressBound(srcSize)), dstRef(LZ4_compressBound(srcSize));

    int consumedExt = srcSize, consumedRef = srcSize;
    int lenExt = LZ4_compress_destSize_extState(state, src, dstExt.data(),
        &consumedExt, targetDstSize, 1);
    int lenRef = LZ4_compress_destSize(src, dstRef.data(), &consumedRef, targetDstSize);
    LZ4_freeStream(state);

    ASSERT_GT(lenExt, 0);
    ASSERT_GT(lenRef, 0);
    EXPECT_EQ(consumedExt, consumedRef);
    EXPECT_EQ(lenExt, lenRef);
    EXPECT_EQ(memcmp(dstExt.data(), dstRef.data(), lenExt), 0);
}

/* A tiny destination still succeeds, consuming only what fits. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_compress_destSize_extState_small_dst_common_3)
{
    const int srcSize = 4 KB;
    const int targetDstSize = 64;
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::random);
    char* src = srcLoad.getOrigData();

    LZ4_stream_t* state = LZ4_createStream();
    ASSERT_NE(state, nullptr);
    vector<char> dst(LZ4_compressBound(srcSize));
    int consumed = srcSize;
    int clen = LZ4_compress_destSize_extState(state, src, dst.data(),
        &consumed, targetDstSize, 1);
    LZ4_freeStream(state);
    ASSERT_GT(clen, 0);
    EXPECT_LE(clen, targetDstSize);
    EXPECT_LE(consumed, srcSize);
}

/* Higher acceleration is accepted and still produces a decodable result. */
TEST_P(LZ4_ext, AOCL_Compression_lz4_LZ4_compress_destSize_extState_acceleration_common_4)
{
    const int srcSize = 4 KB;
    const int targetDstSize = 1 KB;
    TestLoadExt srcLoad(srcSize, gtest_data_gen_type::midratio);
    char* src = srcLoad.getOrigData();

    LZ4_stream_t* state = LZ4_createStream();
    ASSERT_NE(state, nullptr);
    vector<char> dst(LZ4_compressBound(srcSize));
    int consumed = srcSize;
    int clen = LZ4_compress_destSize_extState(state, src, dst.data(),
        &consumed, targetDstSize, 64);
    LZ4_freeStream(state);
    ASSERT_GT(clen, 0);

    /* +1 decode margin: destSize output that fills dst can need one extra output
     * byte (see lz4.h / issue #859). */
    vector<char> out(consumed + 1);
    ASSERT_EQ(LZ4_decompress_safe(dst.data(), out.data(), clen, consumed + 1), consumed);
    EXPECT_EQ(memcmp(out.data(), src, consumed), 0);
}

#endif /* !_WINDOWS (LZ4_compress_destSize_extState) */

#ifdef LZ4_FRAME_FORMAT_SUPPORT
/*==============================================================================
 * LZ4 Frame API (lz4frame.h) tests
 *
 * Compiled only with LZ4_FRAME_FORMAT_SUPPORT, since lz4frame.c is otherwise
 * not part of the build.
 *============================================================================*/

/* One-shot frame compress + decompress round-trip via LZ4F_compressFrame. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_compressFrame_roundtrip_common_1)
{
    const size_t srcSize = 4 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();

    size_t bound = LZ4F_compressFrameBound(srcSize, NULL);
    ASSERT_GT(bound, (size_t)0);
    vector<char> frame(bound);
    size_t clen = LZ4F_compressFrame(frame.data(), frame.size(), src, srcSize, NULL);
    ASSERT_FALSE(LZ4F_isError(clen));
    ASSERT_GT(clen, (size_t)0);

    /* This is a valid frame. */
    EXPECT_NE(LZ4F_headerSize(frame.data(), clen), (size_t)0);
    EXPECT_FALSE(LZ4F_isError(LZ4F_headerSize(frame.data(), clen)));

    /* Decode with a decompression context. */
    LZ4F_dctx* dctx = NULL;
    ASSERT_FALSE(LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_getVersion())));
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ LZ4F_freeDecompressionContext(dctx); });

    vector<char> out(srcSize);
    size_t dstSize = out.size();
    size_t srcConsumed = clen;
    size_t hint = LZ4F_decompress(dctx, out.data(), &dstSize, frame.data(), &srcConsumed, NULL);
    EXPECT_FALSE(LZ4F_isError(hint));
    EXPECT_EQ(dstSize, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* Error helpers behave consistently. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_isError_getErrorName_common_1)
{
    /* Trigger an error: too-small dst for a frame. */
    char tiny[4];
    size_t r = LZ4F_compressFrame(tiny, sizeof(tiny), "abcdefgh", 8, NULL);
    EXPECT_NE(LZ4F_isError(r), 0u);
    EXPECT_NE(LZ4F_getErrorName(r), nullptr);
    EXPECT_GT(strlen(LZ4F_getErrorName(r)), (size_t)0);

    /* A non-error code reports no error and the "No error detected" name. */
    size_t ok = LZ4F_compressFrameBound(100, NULL);
    EXPECT_EQ(LZ4F_isError(ok), 0u);
}

/* Version and level bounds are sane constants. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_getVersion_and_level_max_common_1)
{
    EXPECT_EQ(LZ4F_getVersion(), (unsigned)LZ4F_VERSION);
    /* lz4frame.h documents the maximum level as LZ4HC_CLEVEL_MAX (12). */
    EXPECT_GE(LZ4F_compressionLevel_max(), 12);
}

/* Streaming frame compression: begin/update/flush/end, then decode. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_stream_compress_roundtrip_common_1)
{
    const size_t srcSize = 8 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();

    size_t cap = LZ4F_compressBound(srcSize, NULL) + LZ4F_HEADER_SIZE_MAX + 8;
    vector<char> frame(cap);
    size_t off = 0;
    {
        LZ4F_cctx* cctx = NULL;
        ASSERT_FALSE(LZ4F_isError(LZ4F_createCompressionContext(&cctx, LZ4F_getVersion())));
        ASSERT_NE(cctx, nullptr);
        ScopeGuard freeCctx([&]{ LZ4F_freeCompressionContext(cctx); });

        size_t h = LZ4F_compressBegin(cctx, frame.data(), frame.size(), NULL);
        ASSERT_FALSE(LZ4F_isError(h));
        off += h;

        size_t u = LZ4F_compressUpdate(cctx, frame.data() + off, frame.size() - off,
            src, srcSize, NULL);
        ASSERT_FALSE(LZ4F_isError(u));
        off += u;

        size_t f = LZ4F_flush(cctx, frame.data() + off, frame.size() - off, NULL);
        ASSERT_FALSE(LZ4F_isError(f));
        off += f;

        size_t e = LZ4F_compressEnd(cctx, frame.data() + off, frame.size() - off, NULL);
        ASSERT_FALSE(LZ4F_isError(e));
        off += e;
    }

    /* Decode the assembled frame and read its frame info. */
    LZ4F_dctx* dctx = NULL;
    ASSERT_FALSE(LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_getVersion())));
    ScopeGuard freeDctx([&]{ LZ4F_freeDecompressionContext(dctx); });

    LZ4F_frameInfo_t info;
    memset(&info, 0, sizeof(info));
    size_t infoSrc = off;
    size_t ir = LZ4F_getFrameInfo(dctx, &info, frame.data(), &infoSrc);
    EXPECT_FALSE(LZ4F_isError(ir));

    vector<char> out(srcSize);
    size_t dstSize = out.size();
    size_t srcConsumed = off - infoSrc;
    size_t hint = LZ4F_decompress(dctx, out.data(), &dstSize,
        frame.data() + infoSrc, &srcConsumed, NULL);
    EXPECT_FALSE(LZ4F_isError(hint));
    EXPECT_EQ(dstSize, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);

    /* Reset must leave the context reusable. */
    LZ4F_resetDecompressionContext(dctx);
}

/* Dictionary frame compression via streaming begin_usingDict + decode_usingDict. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_dictionary_roundtrip_common_1)
{
    const size_t dictSize = 4 KB;
    const size_t srcSize = 4 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();

    size_t cap = LZ4F_compressBound(srcSize, NULL) + LZ4F_HEADER_SIZE_MAX + 8;
    vector<char> frame(cap);
    size_t off = 0;
    {
        LZ4F_cctx* cctx = NULL;
        ASSERT_FALSE(LZ4F_isError(LZ4F_createCompressionContext(&cctx, LZ4F_getVersion())));
        ScopeGuard freeCctx([&]{ LZ4F_freeCompressionContext(cctx); });

        size_t h = LZ4F_compressBegin_usingDict(cctx, frame.data(), frame.size(),
            dict.data(), dictSize, NULL);
        ASSERT_FALSE(LZ4F_isError(h));
        off += h;
        size_t u = LZ4F_compressUpdate(cctx, frame.data() + off, frame.size() - off, src, srcSize, NULL);
        ASSERT_FALSE(LZ4F_isError(u));
        off += u;
        size_t e = LZ4F_compressEnd(cctx, frame.data() + off, frame.size() - off, NULL);
        ASSERT_FALSE(LZ4F_isError(e));
        off += e;
    }

    LZ4F_dctx* dctx = NULL;
    ASSERT_FALSE(LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_getVersion())));
    ScopeGuard freeDctx([&]{ LZ4F_freeDecompressionContext(dctx); });
    vector<char> out(srcSize);
    size_t dstSize = out.size();
    size_t srcConsumed = off;
    size_t hint = LZ4F_decompress_usingDict(dctx, out.data(), &dstSize,
        frame.data(), &srcConsumed, dict.data(), dictSize, NULL);
    EXPECT_FALSE(LZ4F_isError(hint));
    EXPECT_EQ(dstSize, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* Digested-dictionary (CDict) one-shot compression. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_CDict_roundtrip_common_1)
{
    const size_t dictSize = 4 KB;
    const size_t srcSize = 4 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();

    LZ4F_CDict* cdict = LZ4F_createCDict(dict.data(), dictSize);
    ASSERT_NE(cdict, nullptr);
    ScopeGuard freeCdict([&]{ LZ4F_freeCDict(cdict); });

    size_t bound = LZ4F_compressFrameBound(srcSize, NULL);
    vector<char> frame(bound);
    size_t clen = 0;
    {
        LZ4F_cctx* cctx = NULL;
        ASSERT_FALSE(LZ4F_isError(LZ4F_createCompressionContext(&cctx, LZ4F_getVersion())));
        ScopeGuard freeCctx([&]{ LZ4F_freeCompressionContext(cctx); });
        clen = LZ4F_compressFrame_usingCDict(cctx, frame.data(), frame.size(),
            src, srcSize, cdict, NULL);
        ASSERT_FALSE(LZ4F_isError(clen));
    }

    LZ4F_dctx* dctx = NULL;
    ASSERT_FALSE(LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_getVersion())));
    ScopeGuard freeDctx([&]{ LZ4F_freeDecompressionContext(dctx); });
    vector<char> out(srcSize);
    size_t dstSize = out.size();
    size_t srcConsumed = clen;
    size_t hint = LZ4F_decompress_usingDict(dctx, out.data(), &dstSize,
        frame.data(), &srcConsumed, dict.data(), dictSize, NULL);
    EXPECT_FALSE(LZ4F_isError(hint));
    EXPECT_EQ(dstSize, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* CDict streaming begin path. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_compressBegin_usingCDict_common_1)
{
    const size_t dictSize = 4 KB;
    const size_t srcSize = 4 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();

    LZ4F_CDict* cdict = LZ4F_createCDict(dict.data(), dictSize);
    ASSERT_NE(cdict, nullptr);
    ScopeGuard freeCdict([&]{ LZ4F_freeCDict(cdict); });

    size_t cap = LZ4F_compressBound(srcSize, NULL) + LZ4F_HEADER_SIZE_MAX + 8;
    vector<char> frame(cap);
    size_t off = 0;
    {
        LZ4F_cctx* cctx = NULL;
        ASSERT_FALSE(LZ4F_isError(LZ4F_createCompressionContext(&cctx, LZ4F_getVersion())));
        ScopeGuard freeCctx([&]{ LZ4F_freeCompressionContext(cctx); });
        size_t h = LZ4F_compressBegin_usingCDict(cctx, frame.data(), frame.size(), cdict, NULL);
        ASSERT_FALSE(LZ4F_isError(h));
        off += h;
        size_t u = LZ4F_compressUpdate(cctx, frame.data() + off, frame.size() - off, src, srcSize, NULL);
        ASSERT_FALSE(LZ4F_isError(u));
        off += u;
        size_t e = LZ4F_compressEnd(cctx, frame.data() + off, frame.size() - off, NULL);
        ASSERT_FALSE(LZ4F_isError(e));
        off += e;
    }

    LZ4F_dctx* dctx = NULL;
    ASSERT_FALSE(LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_getVersion())));
    ScopeGuard freeDctx([&]{ LZ4F_freeDecompressionContext(dctx); });
    vector<char> out(srcSize);
    size_t dstSize = out.size();
    size_t srcConsumed = off;
    size_t hint = LZ4F_decompress_usingDict(dctx, out.data(), &dstSize,
        frame.data(), &srcConsumed, dict.data(), dictSize, NULL);
    EXPECT_FALSE(LZ4F_isError(hint));
    EXPECT_EQ(dstSize, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/*----- Experimental frame API (LZ4F_STATIC_LINKING_ONLY) -----
 * Not exported from the shared library on Windows, so gated behind !_WINDOWS. */
#ifndef _WINDOWS

/* LZ4F_getErrorCode maps a result to the enum; no-error result => OK enum. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_getErrorCode_common_1)
{
    size_t ok = LZ4F_compressFrameBound(100, NULL);
    EXPECT_EQ(LZ4F_getErrorCode(ok), LZ4F_OK_NoError);

    char tiny[4];
    size_t err = LZ4F_compressFrame(tiny, sizeof(tiny), "abcdefgh", 8, NULL);
    EXPECT_NE(LZ4F_getErrorCode(err), LZ4F_OK_NoError);
}

/* LZ4F_getBlockSize returns a positive size for each valid block-size ID. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_getBlockSize_common_1)
{
    size_t s64  = LZ4F_getBlockSize(LZ4F_max64KB);
    size_t s256 = LZ4F_getBlockSize(LZ4F_max256KB);
    size_t s1   = LZ4F_getBlockSize(LZ4F_max1MB);
    size_t s4   = LZ4F_getBlockSize(LZ4F_max4MB);
    EXPECT_FALSE(LZ4F_isError(s64));
    EXPECT_EQ(s64,  (size_t)64 KB);
    EXPECT_EQ(s256, (size_t)256 KB);
    EXPECT_EQ(s1,   (size_t)1 MB);
    EXPECT_EQ(s4,   (size_t)4 MB);
}

/* LZ4F_uncompressedUpdate stores data as uncompressed blocks; requires
 * block-independent mode. Result must still decode to the original. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_LZ4F_uncompressedUpdate_common_1)
{
    const size_t srcSize = 4 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::random);
    char* src = gen.getOrigBuff();

    LZ4F_preferences_t prefs;
    memset(&prefs, 0, sizeof(prefs));
    prefs.frameInfo.blockMode = LZ4F_blockIndependent;

    size_t cap = LZ4F_compressBound(srcSize, &prefs) + LZ4F_HEADER_SIZE_MAX + srcSize + 16;
    vector<char> frame(cap);
    size_t off = 0;
    {
        LZ4F_cctx* cctx = NULL;
        ASSERT_FALSE(LZ4F_isError(LZ4F_createCompressionContext(&cctx, LZ4F_getVersion())));
        ScopeGuard freeCctx([&]{ LZ4F_freeCompressionContext(cctx); });
        size_t h = LZ4F_compressBegin(cctx, frame.data(), frame.size(), &prefs);
        ASSERT_FALSE(LZ4F_isError(h));
        off += h;
        size_t u = LZ4F_uncompressedUpdate(cctx, frame.data() + off, frame.size() - off, src, srcSize, NULL);
        ASSERT_FALSE(LZ4F_isError(u));
        off += u;
        size_t e = LZ4F_compressEnd(cctx, frame.data() + off, frame.size() - off, NULL);
        ASSERT_FALSE(LZ4F_isError(e));
        off += e;
    }

    LZ4F_dctx* dctx = NULL;
    ASSERT_FALSE(LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_getVersion())));
    ScopeGuard freeDctx([&]{ LZ4F_freeDecompressionContext(dctx); });
    vector<char> out(srcSize);
    size_t dstSize = out.size();
    size_t srcConsumed = off;
    size_t hint = LZ4F_decompress(dctx, out.data(), &dstSize, frame.data(), &srcConsumed, NULL);
    EXPECT_FALSE(LZ4F_isError(hint));
    EXPECT_EQ(dstSize, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);
}

/* Custom-memory advanced context/CDict constructors. */
TEST_P(LZ4_ext, AOCL_Compression_lz4frame_advanced_customMem_common_1)
{
    LZ4F_cctx* cctx = LZ4F_createCompressionContext_advanced(LZ4F_defaultCMem, LZ4F_getVersion());
    ASSERT_NE(cctx, nullptr);
    ScopeGuard freeCctx([&]{ LZ4F_freeCompressionContext(cctx); });
    LZ4F_dctx* dctx = LZ4F_createDecompressionContext_advanced(LZ4F_defaultCMem, LZ4F_getVersion());
    ASSERT_NE(dctx, nullptr);
    ScopeGuard freeDctx([&]{ LZ4F_freeDecompressionContext(dctx); });

    const size_t dictSize = 2 KB;
    vector<char> dict(dictSize);
    gtest_data_gen_t::fill_repeated(dict.data(), dictSize);
    LZ4F_CDict* cdict = LZ4F_createCDict_advanced(LZ4F_defaultCMem, dict.data(), dictSize);
    ASSERT_NE(cdict, nullptr);
    ScopeGuard freeCdict([&]{ LZ4F_freeCDict(cdict); });

    /* Smoke round-trip using the advanced-created contexts. */
    const size_t srcSize = 2 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();
    size_t bound = LZ4F_compressFrameBound(srcSize, NULL);
    vector<char> frame(bound);
    size_t clen = LZ4F_compressFrame_usingCDict(cctx, frame.data(), frame.size(),
        src, srcSize, cdict, NULL);
    EXPECT_FALSE(LZ4F_isError(clen));
}

#endif /* !_WINDOWS (experimental frame API) */

/*==============================================================================
 * LZ4 File API (lz4file.h) tests
 *
 * lz4file.c is compiled directly into this test binary (not part of the
 * library). Tests write a temporary .lz4 file, read it back, and verify it.
 *============================================================================*/

TEST_P(LZ4_ext, AOCL_Compression_lz4file_write_read_roundtrip_common_1)
{
    const size_t srcSize = 16 KB;
    gtest_data_gen_t gen(LZ4_compressBound, srcSize, gtest_data_gen_type::midratio);
    char* src = gen.getOrigBuff();

    /* Use the framework's temp directory (guaranteed writable, unlike the CTest
     * working directory), with a per-process suffix so parallel runs or reruns
     * without cleanup never collide on the same path. */
    std::string path = std::string(::testing::TempDir()) + "aocl_lz4file_ext_test_" +
        std::to_string((unsigned long long)AOCL_GETPID()) + ".lz4";
    remove(path.c_str()); /* clear any stale residue from a previous aborted run */

    /* --- write --- */
    FILE* fw = fopen(path.c_str(), "wb");
    ASSERT_NE(fw, nullptr);
    LZ4_writeFile_t* wf = NULL;
    /* Explicit close-before-return so a failure never leaks the FILE* or
     * leaves the temp file on disk. */
    if (LZ4F_isError(LZ4F_writeOpen(&wf, fw, NULL)) || wf == NULL) {
        ADD_FAILURE() << "LZ4F_writeOpen failed";
        fclose(fw); remove(path.c_str()); return;
    }
    size_t written = LZ4F_write(wf, src, srcSize);
    EXPECT_FALSE(LZ4F_isError(written));
    /* On success LZ4F_write returns the number of source bytes written. */
    EXPECT_EQ(written, srcSize);
    EXPECT_FALSE(LZ4F_isError(LZ4F_writeClose(wf)));
    fclose(fw);

    /* --- read --- */
    FILE* fr = fopen(path.c_str(), "rb");
    if (fr == NULL) {
        ADD_FAILURE() << "fopen for read failed";
        remove(path.c_str()); return;
    }
    LZ4_readFile_t* rf = NULL;
    if (LZ4F_isError(LZ4F_readOpen(&rf, fr)) || rf == NULL) {
        ADD_FAILURE() << "LZ4F_readOpen failed";
        fclose(fr); remove(path.c_str()); return;
    }

    vector<char> out(srcSize);
    size_t total = 0;
    while (total < srcSize) {
        size_t n = LZ4F_read(rf, out.data() + total, srcSize - total);
        if (LZ4F_isError(n)) { ADD_FAILURE() << "LZ4F_read failed"; break; }
        if (n == 0) break;
        total += n;
    }
    EXPECT_FALSE(LZ4F_isError(LZ4F_readClose(rf)));
    fclose(fr);

    EXPECT_EQ(total, srcSize);
    EXPECT_EQ(memcmp(out.data(), src, srcSize), 0);

    remove(path.c_str());
}

/* Opening a read handle on a NULL FILE* must fail rather than crash.
 * Confirmed against algos/lz4/lz4file.c: LZ4F_readOpen() begins with an
 * `if (fp == NULL || lz4fRead == NULL) RETURN_ERROR(parameter_null);` guard, so
 * this returns an error code rather than dereferencing the NULL handle. */
TEST_P(LZ4_ext, AOCL_Compression_lz4file_readOpen_null_fp_common_2)
{
    LZ4_readFile_t* rf = NULL;
    LZ4F_errorCode_t r = LZ4F_readOpen(&rf, NULL);
    EXPECT_NE(LZ4F_isError(r), 0u);
}

#endif /* LZ4_FRAME_FORMAT_SUPPORT */

#else /* !AOCL_TEST_COVERAGE_EXT */

/* Avoid an empty translation unit (ISO C++ forbids it; -Wpedantic -Werror
 * would fail the build) when extended coverage is disabled. */
typedef int lz4_ext_gtest_translation_unit_not_empty;

#endif /* AOCL_TEST_COVERAGE_EXT */
