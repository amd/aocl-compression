/**
 * Copyright (C) 2023-2025, Advanced Micro Devices. All rights reserved.
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

#include "zstd_gtest.h"

#ifdef AOCL_TEST_FUZZER
#include "fuzztest/fuzztest.h"
#endif

#include "gtest/gtest_utils.h"

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
size_t Test_ZSTD_compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize, int compressionLevel)
{
    return ZSTD_compress(dst, dstCapacity, src, srcSize, compressionLevel);
}

// Test wrapper function for API ZSTD_decompress()
size_t Test_ZSTD_decompress(void* dst, size_t dstCapacity, const void* src, size_t srcSize)
{
    return ZSTD_decompress(dst, dstCapacity, src, srcSize);
}

// Test wrapper function for Test_ZSTD_isError()
bool Test_ZSTD_isError(size_t len)
{
    return (bool)ZSTD_isError(len);
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

size_t Test_ZSTD_compressBound(size_t srcSize) {
    return ZSTD_compressBound(srcSize);
}

size_t Test_AOCL_ZSTD_compressBound(size_t srcSize, ZSTD_parameters params) {
    return AOCL_ZSTD_compressBound(srcSize, params);
}


size_t Test_ZSTD_writeSkippableFrame(void* dst, size_t dstCapacity,
    const void* src, size_t srcSize, unsigned magicVariant) {
    return ZSTD_writeSkippableFrame(dst, dstCapacity, src, srcSize, magicVariant);
}

const char* Test_ZSTD_getErrorName(size_t code) {
    return ZSTD_getErrorName(code);
}

unsigned long long Test_ZSTD_getFrameContentSize(const void* src, size_t srcSize)
{
    return ZSTD_getFrameContentSize(src, srcSize);
}

unsigned long long Test_ZSTD_getDecompressedSize(const void* src, size_t srcSize)
{
    return ZSTD_getDecompressedSize(src, srcSize);
}

ZSTD_parameters Test_ZSTD_getParams(int compressionLevel, unsigned long long srcSizeHint, size_t dictSize) {
    return ZSTD_getParams(compressionLevel, srcSizeHint, dictSize);
}

ZSTD_compressionParameters Test_ZSTD_adjustCParams(ZSTD_compressionParameters cPar, unsigned long long srcSize,
    size_t dictSize) {
    return ZSTD_adjustCParams(cPar, srcSize, dictSize);
}

size_t Test_ZSTD_checkCParams(ZSTD_compressionParameters params) {
    return ZSTD_checkCParams(params);
}

ZSTD_bounds Test_ZSTD_cParam_getBounds(ZSTD_cParameter cParam) {
    return ZSTD_cParam_getBounds(cParam);
}

ZSTD_bounds Test_ZSTD_dParam_getBounds(ZSTD_dParameter dParam) {
    return ZSTD_dParam_getBounds(dParam);
}

size_t Test_ZSTD_decodingBufferSize_min(unsigned long long windowSize, unsigned long long frameContentSize) {
    return ZSTD_decodingBufferSize_min(windowSize, frameContentSize);
}

unsigned long long Test_ZSTD_findDecompressedSize(const void* src, size_t srcSize) {
    return ZSTD_findDecompressedSize(src, srcSize);
}

size_t Test_ZSTD_findFrameCompressedSize(const void* src, size_t srcSize) {
    return ZSTD_findFrameCompressedSize(src, srcSize);
}

size_t Test_ZSTD_getFrameHeader(ZSTD_frameHeader* zfhPtr, const void* src, size_t srcSize) {
    return ZSTD_getFrameHeader(zfhPtr, src, srcSize);
}

size_t Test_ZSTD_getFrameHeader_advanced(ZSTD_frameHeader* zfhPtr, const void* src, size_t srcSize, ZSTD_format_e format) {
    return ZSTD_getFrameHeader_advanced(zfhPtr, src, srcSize, format);
}

ZSTD_compressionParameters Test_ZSTD_getCParams(int compressionLevel, unsigned long long estimatedSrcSize, size_t dictSize) {
    return ZSTD_getCParams(compressionLevel, estimatedSrcSize, dictSize);
}

unsigned Test_ZSTD_isSkippableFrame(const void* buffer, size_t size) {
    return ZSTD_isSkippableFrame(buffer, size);
}

size_t Test_ZSTD_readSkippableFrame(void* dst, size_t dstCapacity, unsigned* magicVariant, const void* src, size_t srcSize) {
    return ZSTD_readSkippableFrame(dst, dstCapacity, magicVariant, src, srcSize);
}

int Test_ZSTD_defaultCLevel(void) {
    return ZSTD_defaultCLevel();
}

int Test_ZSTD_minCLevel(void) {
    return ZSTD_minCLevel();
}

int Test_ZSTD_maxCLevel(void) {
    return ZSTD_maxCLevel();
}

size_t Test_ZSTD_generateSequences(ZSTD_CCtx* zc, ZSTD_Sequence* outSeqs,
    size_t outSeqsSize, const void* src, size_t srcSize) {
    return ZSTD_generateSequences(zc, outSeqs, outSeqsSize, src, srcSize);
}

size_t Test_ZSTD_compressSequences(ZSTD_CCtx* cctx, void* dst, size_t dstSize,
    const ZSTD_Sequence* inSeqs, size_t inSeqsSize, const void* src, size_t srcSize) {
    return ZSTD_compressSequences(cctx, dst, dstSize, inSeqs, inSeqsSize, src, srcSize);
}

size_t Test_ZSTD_decompressionMargin(void const* src, size_t srcSize) {
    return ZSTD_decompressionMargin(src, srcSize);
}

void* Test_ZSTD_custom_alloc_pass(void* opaque, size_t size) {
    if (opaque)
        *((int*)opaque) = 1;
    return malloc(size);
}

void Test_ZSTD_custom_free_pass(void* opaque, void* address) {
    if (opaque)
        *((int*)opaque) = 0;
    free(address);
}

void* Test_ZSTD_custom_alloc_fail(void* opaque, size_t size) {
    return NULL;
}

void Test_ZSTD_custom_free_fail(void* opaque, void* address) {
}

bool zstd_check_uncompressed_equal_to_original(const char* src, size_t srcSize,
    const char* compressed, size_t compressedLen, ZSTD_decompress_fp decomp_fp)
{
    char* uncompressed = (char*)calloc(srcSize, sizeof(char));


    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    size_t uncompressedLenRes = decomp_fp(dctx, uncompressed, srcSize, compressed, compressedLen);

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

bool has_valid_frame_content_size(char* compressed, unsigned compressedLen) {
    unsigned long long sz = Test_ZSTD_getFrameContentSize(compressed, compressedLen);
    return (sz != ZSTD_CONTENTSIZE_UNKNOWN) && (sz != ZSTD_CONTENTSIZE_ERROR);
}

bool has_unknown_frame_content_size(char* compressed, unsigned compressedLen) {
    unsigned long long sz = Test_ZSTD_getFrameContentSize(compressed, compressedLen);
    return (sz == ZSTD_CONTENTSIZE_UNKNOWN);
}

/* Read a block. Return 1 if last block, 0 if not last block, -1 error.
*  Block_Header uses 3 bytes, written using little-endian convention. It contains 3 fields :
*  Last_Block	Block_Type	Block_Size
*  bit 0	    bits 1-2  	bits 3-23
*/
#define BLOCK_HEADER_BYTES 3
static int read_block(const char* start, const char* end, size_t& block_sz) {
    if (start > (end - BLOCK_HEADER_BYTES))
        return -1; //not enough space for block header
    char bh_bytes[4];
    memcpy(bh_bytes, start, BLOCK_HEADER_BYTES); bh_bytes[3] = 0;
    uint32_t block_header = MEM_read32((const void*)bh_bytes);
    int is_last = block_header & 0x01; // bit 0
    block_header >>= 3; // skip 3 lower bits
    block_sz = block_header;
    return is_last;
}

/* Checks 1st frame in 'compressed'.
* If it is a zstd frame, checks if it is valid.
* Return frame size or ZSTD_error
*/
size_t has_valid_frame(char* compressed, unsigned compressedLen) {
    char* cur = compressed;
    const char* end = compressed + compressedLen;

    //read header
    ZSTD_frameHeader fh;
    size_t ret = Test_ZSTD_getFrameHeader(&fh, cur, compressedLen);
    EXPECT_EQ(ret, 0); //valid frame header obtained
    if (ret != 0) return ERROR(GENERIC);

    if (fh.frameType == ZSTD_skippableFrame) {
        size_t frame_size = fh.frameContentSize + ZSTD_SKIPPABLEHEADERSIZE;
        if (frame_size > compressedLen) 
            return ERROR(GENERIC); //data size of skip frame is not ok
        cur += frame_size;
    }
    else if (fh.frameType == ZSTD_frame) {
        cur += fh.headerSize;
        //read blocks
        size_t block_sz = fh.headerSize;
        while (cur < end) { //read until last block
            int is_last = read_block(cur, end, block_sz);
            cur += (BLOCK_HEADER_BYTES + block_sz); //bytes for header + content
            if (is_last) break;
        }
        //read optional checksum
        if (fh.checksumFlag) {
            cur += 4;
        }
        if (cur > end) return ERROR(GENERIC); //overflow
    }
    else {
        return ERROR(GENERIC); //unsupported frame type
    }
    return (size_t)(cur - compressed);
}

bool has_valid_frames(char* compressed, unsigned compressedLen) {
    unsigned parsedLen = 0;
    while (parsedLen < compressedLen) {
        size_t ret = has_valid_frame(compressed + parsedLen, (compressedLen - parsedLen));
        if (ZSTD_isError(ret)) return false;
        parsedLen += ret;
    }
    return true;
}

/* Insert a valid zstd frame via dynamic dispatcher (might introduce more frames based on optimized implementation)
 * Recommended for tests that are evaluating reference/optimized implementation */
size_t insert_frame(void* dst, size_t dstCapacity, const void* src, size_t srcSize) {
    return Test_ZSTD_compress(dst, dstCapacity, src, srcSize, ZSTD_CLEVEL_DEFAULT);
}

/* Insert a single zstd frame. Calls ZSTD_compress2 that does not have MT support.
 * Recommended for creation of ZSTD frame to test things like properties, etc */
size_t insert_frame_reference(void* dst, size_t dstCapacity, const void* src, size_t srcSize) {
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    EXPECT_NE(cctx, nullptr);
    
    size_t disableFdsFrame = 1;
    Test_ZSTD_CCtx_setFdsRuntimeParams(cctx, disableFdsFrame); // no additional FDS frames

    ZSTD_parameters zparams;
    zparams = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, srcSize, 0);
    zparams.fParams.contentSizeFlag = 1;
    zparams.fParams.checksumFlag = 1;
    CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParams(cctx, zparams));
    size_t ret = Test_ZSTD_compress2(cctx, dst, dstCapacity, src, srcSize);
    ZSTD_freeCCtx(cctx);
    return ret;
}

// Insert a valid skippable frame
size_t insert_skippable_frame(void* dst, size_t dstCapacity, const void* src, size_t srcSize, unsigned magicVarint) {
    return Test_ZSTD_writeSkippableFrame(dst, dstCapacity, src, srcSize, magicVarint);
}

// Insert a valid frame followed by additional garbage bytes into dst
size_t insert_frame_overwrite(char* dst, size_t dstCapacity, char* src, size_t srcSize)
{
    size_t frameSize = srcSize - 32;
    size_t cprLen = insert_frame_reference(dst, dstCapacity, src, frameSize); // Compress frameSize bytes into zstd frame

    EXPECT_LE(cprLen, (dstCapacity - 32));
    if (cprLen < (dstCapacity - 32)) {
        memset(dst + cprLen, '*', 32); //add additional garbage bytes
        cprLen += 32;
    }

    return cprLen;
}

/* Insert N valid frames
*  If dstCapacity is insufficient for N frames, fewer frames may be written */
size_t insert_N_frames(char* dst, size_t dstCapacity, char* src, size_t srcSize, size_t N, size_t& srcWritten)
{
    srcWritten = 0;
    EXPECT_GE(srcSize, N);
    if (srcSize < N) return 0;

    const char* dst_start = dst;
    size_t remCapacity = dstCapacity;
    size_t frameSize = srcSize / N; // split src into N parts and compress each into a separate frame
    size_t remSize = srcSize % N;
    size_t writtenN = 0;

    size_t cprLen;
    char* cur = src;
    for (; writtenN < N; ++writtenN) {
        cprLen = insert_frame_reference(dst, remCapacity, cur, frameSize); // Compress frameSize bytes into a zstd frame
        if (Test_ZSTD_isError(cprLen))
            break; // dstCapacity insufficient will also end up here
        dst += cprLen;
        remCapacity -= cprLen;
        cur += frameSize;
    }
    if (!Test_ZSTD_isError(cprLen) && (remSize > 0)) { //write remaining bytes
        cprLen = insert_frame_reference(dst, remCapacity, cur, remSize); // Compress remSize bytes into a zstd frame
        if (!Test_ZSTD_isError(cprLen)) {
            writtenN++;
            dst += cprLen;
            cur += remSize;
        }
    }

    EXPECT_GT(writtenN, 1); // Atleast 2 frames must be written
    srcWritten = (size_t)(cur - src);
    return (size_t)(dst - dst_start);
}

int get_cparam_below_lower(ZSTD_cParameter param) {
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(param);
    CHECK_PASS_ZSTD(bounds.error);
    return (bounds.lowerBound - 1);
}

int get_cparam_above_upper(ZSTD_cParameter param) {
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(param);
    CHECK_PASS_ZSTD(bounds.error);
    return (bounds.upperBound + 1);
}

int get_cparam_within_bounds(ZSTD_cParameter param) {
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(param);
    CHECK_PASS_ZSTD(bounds.error);
    return bounds.lowerBound + (int)((bounds.upperBound - bounds.lowerBound) / 2);
}

int get_dparam_below_lower(ZSTD_dParameter param) {
    ZSTD_bounds bounds = Test_ZSTD_dParam_getBounds(param);
    CHECK_PASS_ZSTD(bounds.error);
    return (bounds.lowerBound - 1);
}

int get_dparam_above_upper(ZSTD_dParameter param) {
    ZSTD_bounds bounds = Test_ZSTD_dParam_getBounds(param);
    CHECK_PASS_ZSTD(bounds.error);
    return (bounds.upperBound + 1);
}

int get_dparam_within_bounds(ZSTD_dParameter param) {
    ZSTD_bounds bounds = Test_ZSTD_dParam_getBounds(param);
    CHECK_PASS_ZSTD(bounds.error);
    return bounds.lowerBound + (int)((bounds.upperBound - bounds.lowerBound) / 2);
}

size_t skip_skippable_frames(const char* src, size_t srcSize) {
    const char* cur = src;
    while (Test_ZSTD_isSkippableFrame(cur, srcSize)) {
        ZSTD_frameHeader zfh;
        size_t ret = Test_ZSTD_getFrameHeader(&zfh, cur, srcSize);
        if (ZSTD_isError(ret)) return ret;
        cur += (zfh.frameContentSize + ZSTD_SKIPPABLEHEADERSIZE);
    }
    return (size_t)(cur - src);
}

/***********************************************
 * Begin of Version Tests
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
* Begin of ZSTD_ZSTD_compress
***********************************************/
void ZSTD_ZSTD_compress_base::validate_compress_format(char* compressed, unsigned compressedLen, unsigned dstCapacity) {
    EXPECT_LE(compressedLen, dstCapacity);
    EXPECT_TRUE(has_valid_frames(compressed, compressedLen));
    EXPECT_TRUE(has_valid_frame_content_size(compressed, compressedLen) || has_unknown_frame_content_size(compressed, compressedLen));
}

void ZSTD_ZSTD_compress_base::validate_compress(char* src, unsigned srcSize, char* compressed, unsigned compressedLen, unsigned dstCapacity) {
    validate_compress_format(compressed, compressedLen, dstCapacity);
    EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(src, srcSize, compressed, compressedLen, ZSTD_decompressDCtx));
}

void ZSTD_ZSTD_compress_base::compress_all_levels(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress all supported levels
    for (int cLevel = 0; cLevel <= 22; cLevel++) {
        TestLoad_2 d(8000);
        size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
        validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize());
    }
}

void ZSTD_ZSTD_compress_base::compress_src_null(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress src null
    TestLoad_2 d(800);
    size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), NULL, d.getOrigSize());
    CHECK_FAIL_ZSTD(outLen);
    EXPECT_EQ(outLen, ERROR(srcSize_wrong));
}

void ZSTD_ZSTD_compress_base::compress_dst_null(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress dst null
    TestLoad_2 d(800);
    size_t outLen = run_compress(api, cctx, cLevel, NULL, d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    CHECK_FAIL_ZSTD(outLen);
    EXPECT_EQ(outLen, ERROR(dstSize_tooSmall));
}

void ZSTD_ZSTD_compress_base::compress_insufficient_dstCapacity(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { // compress dstCapacity < ZSTD_compressBound(srcSize)
    TestLoad_2 d(800, 400); // dstCapacity explicitly set to be insufficient
    size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    CHECK_FAIL_ZSTD(outLen);
    EXPECT_EQ(outLen, ERROR(dstSize_tooSmall));
}

void ZSTD_ZSTD_compress_base::compress_srcsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { //compress src size 0
    TestLoad_2 d(800);
    size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), 0);
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    validate_compress_format(d.getCompressedBuff(), outLen, d.getCompressedSize());
}

void ZSTD_ZSTD_compress_base::compress_src_null_srcsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { //compress src null and src size 0
    TestLoad_2 d(800);
    size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), NULL, 0);
    //src size = 0 is a valid input and is expected to return a valid zstd frame 
    CHECK_PASS_ZSTD(outLen);
    validate_compress_format(d.getCompressedBuff(), outLen, d.getCompressedSize());
}

void ZSTD_ZSTD_compress_base::compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel) { //compress src null and src size 0 and dst size 0
    TestLoad_2 d(800);
    size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), 0, NULL, 0);
    CHECK_FAIL_ZSTD(outLen);
    EXPECT_EQ(outLen, ERROR(dstSize_tooSmall));
}

void ZSTD_ZSTD_compress_base::compress_level_lt_min(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { // compress level < 0
    TestLoad_2 d(800);
    int cLevel = Test_ZSTD_minCLevel();
    // For levels < 1, compression parameters are set to 0th entry of the table `ZSTD_defaultCParameters[4][ZSTD_MAX_CLEVEL+1]`
    size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize());
}

void ZSTD_ZSTD_compress_base::compress_level_gt_max(ZSTD_Compress_API api, ZSTD_CCtx* cctx) { //compress level > maximum limit
    TestLoad_2 d(800);
    int cLevel = Test_ZSTD_maxCLevel() + 1;
    // For level > maximum possible level, level will be set to ZSTD_MAX_CLEVEL, which is 22.
    size_t outLen = run_compress(api, cctx, cLevel, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    validate_compress(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, d.getCompressedSize());
}

void ZSTD_ZSTD_compress_base::compress_cctx_null(ZSTD_Compress_API api) { // compress cctx null
    TestLoad_2 d(800);
    size_t outLen = run_compress(api, NULL, ZSTD_CLEVEL_DEFAULT, d.getCompressedBuff(), d.getCompressedSize(), d.getOrigData(), d.getOrigSize());
    EXPECT_EQ(outLen, ERROR(GENERIC));
}

size_t ZSTD_ZSTD_compress_base::run_compress(ZSTD_Compress_API api, ZSTD_CCtx* cctx, int cLevel, void* dst, size_t dstCapacity, const void* src, size_t srcSize) {
    size_t res = 0;
    switch (api) {
    case ZSTD_Compress_API::compress:
    {
        res = Test_ZSTD_compress(dst, dstCapacity, src, srcSize, cLevel);
        break;
    }
    case ZSTD_Compress_API::compress_cctx:
    {
        res = Test_ZSTD_compressCCtx(cctx, dst, dstCapacity, src, srcSize, cLevel);
        break;
    }
    case ZSTD_Compress_API::compress_advanced:
    {
        ZSTD_parameters zparams;
        zparams = Test_ZSTD_getParams(cLevel, srcSize, 0);
        zparams.fParams.contentSizeFlag = 1;
        zparams.fParams.checksumFlag = 1;
        if (cctx) {
            CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParams(cctx, zparams));
        }
        res = Test_ZSTD_compress_advanced(cctx, dst, dstCapacity, src, srcSize, NULL, 0, zparams);
        break;
    }
    case ZSTD_Compress_API::compress2:
    {
        ZSTD_parameters zparams;
        zparams = Test_ZSTD_getParams(cLevel, srcSize, 0);
        zparams.fParams.contentSizeFlag = 1;
        zparams.fParams.checksumFlag = 1;
        if (cctx) {
            CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParams(cctx, zparams));
        }
        res = Test_ZSTD_compress2(cctx, dst, dstCapacity, src, srcSize);
        break;
    }
    case ZSTD_Compress_API::compress_sequence:
    {
        ZSTD_parameters zparams;
        zparams = Test_ZSTD_getParams(cLevel, srcSize, 0);
        zparams.fParams.contentSizeFlag = 1;
        zparams.fParams.checksumFlag = 1;
        if (cctx) {
            CHECK_PASS_ZSTD(Test_ZSTD_CCtx_reset(cctx, ZSTD_reset_session_and_parameters));
            CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParams(cctx, zparams));
        }
        size_t outSeqsSize = srcSize;
        ZSTD_Sequence* const outSeqs = (ZSTD_Sequence*)malloc(srcSize * sizeof(ZSTD_Sequence));
        res = Test_ZSTD_generateSequences(cctx, outSeqs, outSeqsSize, src, srcSize);
        if (Test_ZSTD_isError(res)) {
            free(outSeqs);
            break;
        }
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_reset(cctx, ZSTD_reset_session_and_parameters));
        CHECK_PASS_ZSTD(Test_ZSTD_CCtx_setParameter(cctx, ZSTD_c_blockDelimiters, ZSTD_sf_explicitBlockDelimiters));
        res = Test_ZSTD_compressSequences(cctx, dst, dstCapacity, outSeqs, res, src, srcSize);
        free(outSeqs);
        break;
    }
    default:
        break;
    }
    return res;
}

class ZSTD_ZSTD_compress : public ZSTD_ZSTD_compress_base {};

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_pass_common_1)
{
    compress_all_levels(ZSTD_Compress_API::compress, NULL);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_fail_common_2)
{
    compress_src_null(ZSTD_Compress_API::compress, NULL, ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_fail_common_3)
{
    compress_dst_null(ZSTD_Compress_API::compress, NULL, ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_fail_common_4)
{
    compress_insufficient_dstCapacity(ZSTD_Compress_API::compress, NULL, ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_pass_common_5)
{
    compress_srcsize_0(ZSTD_Compress_API::compress, NULL, ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_pass_common_6)
{
    compress_src_null_srcsize_0(ZSTD_Compress_API::compress, NULL, ZSTD_CLEVEL_DEFAULT);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_pass_common_7)
{
    compress_level_lt_min(ZSTD_Compress_API::compress, NULL);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_pass_common_8)
{
    compress_level_gt_max(ZSTD_Compress_API::compress, NULL);
}

TEST_F(ZSTD_ZSTD_compress, AOCL_Compression_zstd_ZSTD_compress_pass_common_9)
{
    compress_src_null_srcsize_0_dstsize_0(ZSTD_Compress_API::compress, NULL, ZSTD_CLEVEL_DEFAULT);
}
/*********************************************
 * End of ZSTD_ZSTD_compress
 *********************************************/

/***********************************************
 * Begin of ZSTD_ZSTD_decompress
 ***********************************************/
void ZSTD_frame_creator::create_frame()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame(src, d->getCompressedSize(), original, origLen);  // Compress data from `original` buffer to `src` buffer.
}

void ZSTD_frame_creator::create_frame_reference()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame_reference(src, d->getCompressedSize(), original, origLen);  // Compress data from `original` buffer to `src` buffer.
}

void ZSTD_frame_creator::create_frame_overwrite()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame_overwrite(src, d->getCompressedSize(), original, origLen);
}

size_t ZSTD_frame_creator::create_frames_multiple()
{
    size_t N = 7;
    size_t srcWritten = 0;
    src = d->getCompressedBuff();
    srcLen = insert_N_frames(src, d->getCompressedSize(), original, origLen, N, srcWritten);
    return srcWritten;
}

void ZSTD_frame_creator::create_frame_skippable(unsigned magicVarint)
{
    src = d->getCompressedBuff();
    srcLen = insert_skippable_frame(src, d->getCompressedSize(), original, origLen, magicVarint);
}

void ZSTD_frame_creator::create_frame_and_skippable()
{
    src = d->getCompressedBuff();
    srcLen = insert_skippable_frame(src, d->getCompressedSize(), original, origLen / 2, rand() % 15); // 1 skippable frame
    srcLen += insert_frame_reference(src + srcLen, d->getCompressedSize() - srcLen, original + (origLen / 2), origLen / 2); // followed by 1 zstd frame
}

void ZSTD_frame_creator::create_frame_with_no_decomp_size()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame_via_stream(src, d->getCompressedSize(), original, origLen);
}

void ZSTD_frame_creator::create_frame_with_params(ZSTD_frameParameters fparams)
{
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (cctx == NULL) return;
    ZSTD_parameters params = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, origLen, 0);
    params.fParams = fparams;
    CHECK_PASS_ZSTD(ZSTD_CCtx_setParams(cctx, params));
    src = d->getCompressedBuff();
    srcLen = Test_ZSTD_compress2(cctx, src, d->getCompressedSize(), original, origLen);
    CHECK_PASS_ZSTD(srcLen);
    CHECK_PASS_ZSTD(ZSTD_freeCCtx(cctx));
}

void ZSTD_frame_creator::create_frame_prefix_unknown()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame_reference(src, d->getCompressedSize(), original, origLen);
    src[0] = 0;
}

void ZSTD_frame_creator::create_frame_invalid_data_block()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame_reference(src, d->getCompressedSize(), original, origLen);
    size_t frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
    src[frameHeaderSize + 1] = 'e'; // corrupt data succeeding frame header
    src[frameHeaderSize + 2] = 'r';
    src[frameHeaderSize + 3] = 'r';
    src[frameHeaderSize + 4] = 'o';
    src[frameHeaderSize + 5] = 'r';
}

void ZSTD_frame_creator::create_empty_frame()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame_reference(src, d->getCompressedSize(), NULL, 0);
}

void ZSTD_frame_creator::create_stream_frame()
{
    src = d->getCompressedBuff();
    srcLen = insert_frame_via_stream(src, d->getCompressedSize(), original, origLen);
}

void ZSTD_frame_creator::skip_until_zstd_frame() {
    //skip non zstd frames if any
    size_t skip_size = skip_skippable_frames(src, srcLen);
    ASSERT_FALSE(ZSTD_isError(skip_size));
    src += skip_size;
    srcLen -= skip_size;
}

size_t ZSTD_ZSTD_decompress_base::run_decompress(ZSTD_Decompress_API api, ZSTD_DCtx* dctx,
    void* dst, size_t dstCapacity, const void* src, size_t srcSize) {
    size_t res = 0;
    switch (api) {
    case ZSTD_Decompress_API::decompress:
    {
        res = Test_ZSTD_decompress(dst, dstCapacity, src, srcSize);
        break;
    }
    case ZSTD_Decompress_API::decompress_dctx:
    {
        res = Test_ZSTD_decompressDCtx(dctx, dst, dstCapacity, src, srcSize);
        break;
    }
    default:
        break;
    }
    return res;
}

void ZSTD_ZSTD_decompress_base::validate_decompress(const char* original, unsigned origLen, const char* output, unsigned outputLen) {
    EXPECT_EQ(origLen, outputLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

void ZSTD_ZSTD_decompress_base::decompress_pass(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // pass
    create_frame();
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    validate_decompress(original, origLen, output, decLen);
}

void ZSTD_ZSTD_decompress_base::decompress_skip(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // pass skip frame
    create_frame_skippable(rand() % 15);
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_FALSE(ZSTD_isError(decLen));
}

void ZSTD_ZSTD_decompress_base::decompress_src_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src null
    create_frame();
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), NULL, srcLen);
    EXPECT_EQ(decLen, ERROR(srcSize_wrong));
}

void ZSTD_ZSTD_decompress_base::decompress_srcsize_less(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src size < entire frame
    create_frame();
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen - 1);
    EXPECT_EQ(decLen, ERROR(srcSize_wrong));
}

void ZSTD_ZSTD_decompress_base::decompress_srcsize_more(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src size > entire frame
    create_frame();
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen + 1);
    EXPECT_EQ(decLen, ERROR(srcSize_wrong));
}

void ZSTD_ZSTD_decompress_base::decompress_dst_null(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress dst null
    create_frame();
    size_t decLen = run_decompress(api, dctx, NULL, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_EQ(decLen, ERROR(dstBuffer_null));
}

void ZSTD_ZSTD_decompress_base::decompress_buffer_inadequate(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompression buffer inadequate
    create_frame();
    size_t decLen = run_decompress(api, dctx, output, srcLen / 20, src, srcLen);
    EXPECT_EQ(decLen, ERROR(dstSize_tooSmall));
}

void ZSTD_ZSTD_decompress_base::decompress_srcsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src 0
    create_frame();
    EXPECT_EQ(run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, 0), 0);
}

void ZSTD_ZSTD_decompress_base::decompress_src_null_srcsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src 0 and src null
    create_frame();
    EXPECT_EQ(run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), NULL, 0), 0);
}

void ZSTD_ZSTD_decompress_base::decompress_src_null_srcsize_0_dstsize_0(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress src 0 and src null and dst 0
    create_frame();
    EXPECT_EQ(run_decompress(api, dctx, output, 0, NULL, 0), 0);
}

void ZSTD_ZSTD_decompress_base::decompress_corrupt_magic_number(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress corrupt frame : magic number
    create_frame_reference();
    size_t decompress_bound = Test_ZSTD_decompressBound(src, srcLen);
    src[0] = 0; // corrupt magic number (present in first 4 bytes)

    size_t decLen = run_decompress(api, dctx, output, decompress_bound, src, srcLen);
    EXPECT_EQ(decLen, ERROR(prefix_unknown));
}

void ZSTD_ZSTD_decompress_base::decompress_corrupt_frame_header(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress corrupt frame : Frame_Header
    create_frame_reference();

    //get header info of zstd frame
    size_t frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
    size_t decompress_bound = Test_ZSTD_decompressBound(src, srcLen);

    EXPECT_GT(frameHeaderSize, 4); // magic number + header
    size_t byte_id = 4 /*magic number*/ + 1 /*Frame_Header_Descriptor*/;
    for (; byte_id < frameHeaderSize; byte_id++) { // corrupt Frame_Header fields succeeding Frame_Header_Descriptor 
        src[byte_id] = 0;
    }

    size_t decLen = run_decompress(api, dctx, output, decompress_bound, src, srcLen);
    EXPECT_EQ(decLen, ERROR(corruption_detected));
}

void ZSTD_ZSTD_decompress_base::decompress_corrupt_data_block(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress corrupt frame : Data_block 
    create_frame_reference();

    size_t frameHeaderSize = Test_ZSTD_frameHeaderSize(src, srcLen);
    size_t decompress_bound = Test_ZSTD_decompressBound(src, srcLen);
    src[frameHeaderSize + 1] = 'e'; //corrupt data succeeding frame header
    src[frameHeaderSize + 2] = 'r';
    src[frameHeaderSize + 3] = 'r';
    src[frameHeaderSize + 4] = 'o';
    src[frameHeaderSize + 5] = 'r';

    size_t decLen = run_decompress(api, dctx, output, decompress_bound, src, srcLen);
    EXPECT_EQ(decLen, ERROR(srcSize_wrong));
}

void ZSTD_ZSTD_decompress_base::decompress_garbage_after_frame(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress garbage after valid frame
    create_frame_overwrite(); // there should be no additional bytes after the end of a frame. Expect decompress to fail.
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_EQ(decLen, ERROR(srcSize_wrong));
}

void ZSTD_ZSTD_decompress_base::decompress_N_frames(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress N frames
    size_t srcWritten = create_frames_multiple(); // having multiple frames in a stream is valid input. Expect decompress to pass.
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    validate_decompress(original, srcWritten, output, decLen);
}

void ZSTD_ZSTD_decompress_base::decompress_skippable_frame(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress skippable frame
    create_frame_skippable(rand() % 15); // having a skippable frame is valid input. Expect decompress to pass.
    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    CHECK_PASS_ZSTD(decLen);
}

void ZSTD_ZSTD_decompress_base::decompress_checksumFlag_set(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress checksumFlag set
    ZSTD_frameParameters fparams;
    fparams.checksumFlag = 1;
    fparams.contentSizeFlag = 0;
    fparams.noDictIDFlag = 0;
    create_frame_with_params(fparams);

    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    CHECK_PASS_ZSTD(decLen);
    EXPECT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

void ZSTD_ZSTD_decompress_base::decompress_checksum_wrong(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress checksum wrong
    ZSTD_frameParameters fparams;
    fparams.checksumFlag = 1;
    fparams.contentSizeFlag = 0;
    fparams.noDictIDFlag = 0;
    create_frame_with_params(fparams);
    MEM_write32((src + srcLen - 4), 0); // corrupt checksum

    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_EQ(decLen, ERROR(checksum_wrong));
}

void ZSTD_ZSTD_decompress_base::decompress_contentSizeFlag_set(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress contentSizeFlag set
    ZSTD_frameParameters fparams;
    fparams.contentSizeFlag = 1;
    fparams.checksumFlag = 0;
    fparams.noDictIDFlag = 0;
    create_frame_with_params(fparams);

    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    CHECK_PASS_ZSTD(decLen);
    EXPECT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

void ZSTD_ZSTD_decompress_base::decompress_noDictIDFlag_set(ZSTD_Decompress_API api, ZSTD_DCtx* dctx) { // decompress noDictIDFlag set
    ZSTD_frameParameters fparams;
    fparams.noDictIDFlag = 1;
    fparams.checksumFlag = 0;
    fparams.contentSizeFlag = 0;
    create_frame_with_params(fparams);

    size_t decLen = run_decompress(api, dctx, output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    CHECK_PASS_ZSTD(decLen);
    EXPECT_EQ(origLen, decLen);
    EXPECT_EQ(0, memcmp(output, original, origLen));
}

class ZSTD_ZSTD_decompress : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_1)
{
    decompress_pass(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_2)
{
    decompress_src_null(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_3)
{
    decompress_dst_null(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_4)
{
    decompress_buffer_inadequate(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_5)
{
    decompress_srcsize_0(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_6)
{
    decompress_src_null_srcsize_0(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_7)
{
    decompress_corrupt_magic_number(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_8)
{
    decompress_corrupt_frame_header(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_9)
{
    decompress_corrupt_data_block(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_10)
{
    decompress_garbage_after_frame(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_11)
{
    decompress_N_frames(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_12)
{
    decompress_skippable_frame(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_13)
{
    decompress_checksumFlag_set(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_14)
{
    decompress_checksum_wrong(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_15)
{
    decompress_contentSizeFlag_set(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_16)
{
    decompress_noDictIDFlag_set(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_17)
{
    decompress_srcsize_less(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_fail_common_18)
{
    decompress_srcsize_more(ZSTD_Decompress_API::decompress, NULL);
}

TEST_F(ZSTD_ZSTD_decompress, AOCL_Compression_zstd_ZSTD_decompress_pass_common_19)
{
    decompress_src_null_srcsize_0_dstsize_0(ZSTD_Decompress_API::decompress, NULL);
}
/*********************************************
 * End of ZSTD_ZSTD_decompress
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_decompressionMargin
 *********************************************/
class ZSTD_ZSTD_decompressionMargin : public ZSTD_ZSTD_decompress_base {
public:
    ZSTD_ZSTD_decompressionMargin() {
        dmOutput = NULL;
        dmInput = NULL;
    }

    ~ZSTD_ZSTD_decompressionMargin() {
        if (dmOutput) free(dmOutput);
    }

    /*  ______________________ dmOutput Buffer ________________________
     * |                                                              |
     * |                                        ___ dmInput Buffer ___|
     * |                                       |                      |
     * v                                       v_______srcLen_________v
     * |---------------------------------------|-----------|----------|
     * ^                                                   ^          ^
     * |_____________________ origLen _____________________|_ margin _|
    */
    void setup_overlapping_io_buffers(size_t margin) {
        if (Test_ZSTD_isError(margin)) return;
        size_t const dmOutputSize = (origLen + margin);
        dmOutput = (char*)malloc(dmOutputSize);
        dmInput = dmOutput + dmOutputSize - srcLen;
        EXPECT_LT(srcLen, origLen + margin);
        memcpy(dmInput, src, srcLen);
    }

    void decompress_and_validate(size_t origWritten) {
        size_t decLen = Test_ZSTD_decompress(dmOutput, origWritten, dmInput, srcLen);
        CHECK_PASS_ZSTD(decLen);
        validate_decompress(original, origWritten, dmOutput, decLen);
    }

    void decompress_and_validate() {
        decompress_and_validate(origLen);
    }

    void decompress_only() { // non-zstd frames
        size_t decLen = Test_ZSTD_decompress(dmOutput, origLen, dmInput, srcLen);
        CHECK_PASS_ZSTD(decLen);
    }

    char* dmOutput; // output decompressed data
    char* dmInput;  // input compressed data
};

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_pass_common_1) // zstd frame
{
    create_frame(); // create compressed frame
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    CHECK_PASS_ZSTD(margin);
    setup_overlapping_io_buffers(margin);
    decompress_and_validate(); // decompress using overlapping io buffers
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_pass_common_2) // skippable frame
{
    create_frame_skippable(rand() % 15); // create skippable frame
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    CHECK_PASS_ZSTD(margin);
    setup_overlapping_io_buffers(margin);
    decompress_only(); // decompress using overlapping io buffers
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_pass_common_3) // frame and skippable frame
{
    create_frame_and_skippable(); // create frame and skippable frame
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    CHECK_PASS_ZSTD(margin);
    setup_overlapping_io_buffers(margin);
    decompress_only(); // decompress using overlapping io buffers
}
   
TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_pass_common_4) // empty frame
{
    create_empty_frame(); // create empty frame
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    CHECK_PASS_ZSTD(margin);
    setup_overlapping_io_buffers(margin);
    decompress_only(); // decompress using overlapping io buffers
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_pass_common_5) // N frames
{
    size_t srcWritten = create_frames_multiple(); // create N frames
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    CHECK_PASS_ZSTD(margin);
    setup_overlapping_io_buffers(margin);
    decompress_and_validate(srcWritten); // decompress using overlapping io buffers
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_fail_common_6) // prefix unknown
{
    create_frame_prefix_unknown(); // create frame with prefix unknown
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    EXPECT_EQ(margin, ERROR(prefix_unknown));
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_fail_common_7) // invalid data block
{
    create_frame_invalid_data_block(); // create frame with invalid data block
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    EXPECT_EQ(margin, ERROR(corruption_detected));
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_fail_common_8) // src is null
{
    create_frame();
    size_t margin = Test_ZSTD_decompressionMargin(NULL, srcLen);
    EXPECT_EQ(margin, ERROR(GENERIC));
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_pass_common_9) // srcLen = 0
{
    create_frame();
    size_t margin = Test_ZSTD_decompressionMargin(src, 0);
    EXPECT_EQ(margin, 0);
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_fail_common_10) // frame srcLen - 1
{
    create_frame();
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen - 1);
    EXPECT_EQ(margin, ERROR(corruption_detected)); // as contents of frame do not match the size indicated in frame header
}

TEST_F(ZSTD_ZSTD_decompressionMargin, AOCL_Compression_zstd_Test_ZSTD_decompressionMargin_fail_common_11) // frame srcLen + garbage data
{
    create_frame_overwrite(); // Insert a valid frame followed by additional garbage bytes into dst
    size_t margin = Test_ZSTD_decompressionMargin(src, srcLen);
    CHECK_FAIL_ZSTD(margin); // Error code is dependent on nature of garbage data
}
/*********************************************
 * End of ZSTD_ZSTD_decompressionMargin
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_getframeContentSize
 *********************************************/
class ZSTD_ZSTD_getframeContentSize : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_pass_common_1) // size >= `ZSTD_frameHeaderSize_max`
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, srcLen);
    size_t decLen = Test_ZSTD_decompress(output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_EQ(val, decLen);
}

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_fail_common_2) // size < `ZSTD_frameHeaderSize_max`
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, 1);
    EXPECT_EQ(val, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_fail_common_3) // src not a zstd frame
{
    create_frame();
    memset(src, 0, srcLen); //erase zstd frame
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, srcLen);
    EXPECT_EQ(val, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_fail_common_4) // decompressed size unknown
{
    create_stream_frame(); // frame inserted via streaming api wont have decompressed size
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, srcLen);
    EXPECT_EQ(val, ZSTD_CONTENTSIZE_UNKNOWN);
}

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_pass_common_5) // empty frame
{
    create_empty_frame();
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, srcLen);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_pass_common_6) // src is null
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getFrameContentSize(NULL, srcLen);
    EXPECT_EQ(val, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_pass_common_7) // srcLen = 0
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getFrameContentSize(src, 0);
    EXPECT_EQ(val, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_getframeContentSize, AOCL_Compression_zstd_ZSTD_getFrameContentSize_pass_common_8) // src is null and srcLen = 0
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getFrameContentSize(NULL, 0);
    EXPECT_EQ(val, ZSTD_CONTENTSIZE_ERROR);
}
/*********************************************
 * End of ZSTD_ZSTD_getframeContentSize
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_getDecompressedSize
 *********************************************/
class ZSTD_ZSTD_getDecompressedSize : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_pass_common_1) // size >= `ZSTD_frameHeaderSize_max`
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, srcLen);
    size_t decLen = Test_ZSTD_decompress(output, Test_ZSTD_decompressBound(src, srcLen), src, srcLen);
    EXPECT_EQ(val, decLen);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_fail_common_2) // size < `ZSTD_frameHeaderSize_max`
{
    create_frame_reference();
    size_t fhsz = Test_ZSTD_frameHeaderSize(src, srcLen) - 1;
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, fhsz);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_fail_common_3) // size < minInputSize
{
    create_frame_reference();
    size_t fhsz = 4; // ZSTD_startingInputLength(ZSTD_f_zstd1) - 1
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, fhsz);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_fail_common_4) // src not a zstd frame
{
    create_frame_prefix_unknown(); // corrupt magic number
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, srcLen);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_fail_common_5) // decompressed size unknown
{
    create_stream_frame(); // frame inserted via streaming api wont have decompressed size
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, srcLen);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_pass_common_6) // empty frame
{
    create_empty_frame();
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, srcLen);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_pass_common_7) // src is null
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getDecompressedSize(NULL, srcLen);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_pass_common_8) // srcLen = 0
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getDecompressedSize(src, 0);
    EXPECT_EQ(val, 0);
}

TEST_F(ZSTD_ZSTD_getDecompressedSize, AOCL_Compression_zstd_ZSTD_getDecompressedSize_pass_common_9) // src is null and srcLen = 0
{
    create_frame_reference();
    unsigned long long val = Test_ZSTD_getDecompressedSize(NULL, 0);
    EXPECT_EQ(val, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_getDecompressedSize
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_getErrorName
*********************************************/
static const char* const invalidErrorCodeMsg = "Unspecified error code";
TEST(ZSTD_ZSTD_getErrorName, AOCL_Compression_zstd_ZSTD_getErrorName_pass_common_1) { // valid error code
    // Expect to get an error name string that is not NULL or empty
    const char* name_no_error = Test_ZSTD_getErrorName(ERROR(no_error));
    ASSERT_NE(name_no_error, nullptr);
    EXPECT_STRNE(name_no_error, "");
    EXPECT_STRNE(name_no_error, invalidErrorCodeMsg);

    // Expect to get an error name string that is not NULL or empty
    const char* name_error = Test_ZSTD_getErrorName(ERROR(GENERIC));
    ASSERT_NE(name_error, nullptr);
    EXPECT_STRNE(name_error, "");
    EXPECT_STRNE(name_error, invalidErrorCodeMsg);
}

TEST(ZSTD_ZSTD_getErrorName, AOCL_Compression_zstd_ZSTD_getErrorName_fail_common_2) { // invalid error code
    const char* name_invalid = Test_ZSTD_getErrorName(ERROR(maxCode) + 1); // pass invalid error code
    ASSERT_NE(name_invalid, nullptr);
    EXPECT_STREQ(name_invalid, invalidErrorCodeMsg);
}
/*********************************************
 * End of ZSTD_ZSTD_getErrorName
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_getParams
*********************************************/
class ZSTD_ZSTD_getParams : public AOCL_setup_zstd
{
public:
    void validate_parameters(ZSTD_parameters& params) {
        // validate compression parameters are within bounds
        EXPECT_EQ(Test_ZSTD_checkCParams(params.cParams), 0);

        // validate frame parameters are set to default
        EXPECT_EQ(params.fParams.contentSizeFlag, 1);
        // EXPECT_EQ(params.fParams.checksumFlag, 0); this flag is not set by ZSTD_getParams
        // EXPECT_EQ(params.fParams.noDictIDFlag, 0); this flag is not set by ZSTD_getParams
    }
};

TEST_F(ZSTD_ZSTD_getParams, AOCL_Compression_zstd_ZSTD_getParams_pass_common_1) { // check if default ZSTD_frameParameters are set irrespective of input
    {
        ZSTD_parameters params = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, 0, 0);
        validate_parameters(params);
    }
    {
        ZSTD_parameters params = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, ULLONG_MAX, 0);
        validate_parameters(params);
    }
    {
        ZSTD_parameters params = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, 1024, 1025); // dictSize > estimatedSrcSize
        validate_parameters(params);
    }
    {
        ZSTD_parameters params = Test_ZSTD_getParams(ZSTD_CLEVEL_DEFAULT, 1024, 1023); // dictSize < estimatedSrcSize
        validate_parameters(params);
    }
    for (int compressionLevel = -1; compressionLevel <= 23; compressionLevel++) // all levels including out of bound ones
    {
        ZSTD_parameters params = Test_ZSTD_getParams(compressionLevel, 1024, 1024);
        validate_parameters(params);
    }
}
/*********************************************
 * End of ZSTD_ZSTD_getParams
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_getCParams
*********************************************/
class ZSTD_ZSTD_getCParams : public AOCL_setup_zstd
{
public:
    void validate_parameters(ZSTD_compressionParameters& cParams) {
        // validate compression parameters are within bounds
        EXPECT_EQ(Test_ZSTD_checkCParams(cParams), 0);
    }
};

TEST_F(ZSTD_ZSTD_getCParams, AOCL_Compression_zstd_ZSTD_getCParams_pass_common_1) { // check if default ZSTD_frameParameters are set irrespective of input
    {
        ZSTD_compressionParameters cParams = Test_ZSTD_getCParams(ZSTD_CLEVEL_DEFAULT, 0, 0);
        validate_parameters(cParams);
    }
    {
        ZSTD_compressionParameters cParams = Test_ZSTD_getCParams(ZSTD_CLEVEL_DEFAULT, ULLONG_MAX, 0);
        validate_parameters(cParams);
    }
    {
        ZSTD_compressionParameters cParams = Test_ZSTD_getCParams(ZSTD_CLEVEL_DEFAULT, 1024, 1025); // dictSize > estimatedSrcSize
        validate_parameters(cParams);
    }
    {
        ZSTD_compressionParameters cParams = Test_ZSTD_getCParams(ZSTD_CLEVEL_DEFAULT, 1024, 1023); // dictSize < estimatedSrcSize
        validate_parameters(cParams);
    }
    for (int compressionLevel = -1; compressionLevel <= 23; compressionLevel++) // all levels including out of bound ones
    {
        ZSTD_compressionParameters cParams = Test_ZSTD_getCParams(compressionLevel, 1024, 1024);
        validate_parameters(cParams);
    }
}
/*********************************************
 * End of ZSTD_ZSTD_getCParams
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_isError
*********************************************/
TEST(ZSTD_ZSTD_isError, AOCL_Compression_zstd_ZSTD_isError_pass_common_1) { // valid error codes
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(GENERIC)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(prefix_unknown)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(version_unsupported)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(frameParameter_unsupported)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(frameParameter_windowTooLarge)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(corruption_detected)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(checksum_wrong)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(literals_headerWrong)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(dictionary_corrupted)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(dictionary_wrong)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(dictionaryCreation_failed)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(parameter_unsupported)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(parameter_combination_unsupported)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(parameter_outOfBound)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(tableLog_tooLarge)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(maxSymbolValue_tooLarge)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(maxSymbolValue_tooSmall)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(stabilityCondition_notRespected)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(stage_wrong)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(init_missing)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(memory_allocation)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(workSpace_tooSmall)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(dstSize_tooSmall)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(srcSize_wrong)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(dstBuffer_null)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(noForwardProgress_destFull)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(noForwardProgress_inputEmpty)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(frameIndex_tooLarge)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(seekableIO)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(dstBuffer_wrong)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(srcBuffer_wrong)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(sequenceProducer_failed)));
    EXPECT_TRUE(Test_ZSTD_isError(ERROR(externalSequences_invalid)));
}

TEST(ZSTD_ZSTD_getErrorName, AOCL_Compression_zstd_ZSTD_isError_fail_common_2) { // invalid error codes
    EXPECT_FALSE(Test_ZSTD_isError(ERROR(no_error)));
    EXPECT_FALSE(Test_ZSTD_isError(ERROR(maxCode)));
    EXPECT_FALSE(Test_ZSTD_isError(ERROR(maxCode)-1));
}
/*********************************************
 * End of ZSTD_ZSTD_isError
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_adjustCParams
*********************************************/
class ZSTD_ZSTD_adjustCParams : public AOCL_setup_zstd
{
public:
    void validate_parameters(ZSTD_compressionParameters& cParams) {
        // validate compression parameters are within bounds
        EXPECT_EQ(Test_ZSTD_checkCParams(cParams), 0);
    }
};

TEST_F(ZSTD_ZSTD_adjustCParams, AOCL_Compression_zstd_ZSTD_adjustCParams_pass_common_1) { // valid inputs
    ZSTD_compressionParameters cPar;
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    ZSTD_compressionParameters cParams = Test_ZSTD_adjustCParams(cPar, 1024, 1204);
    validate_parameters(cParams);
}

TEST_F(ZSTD_ZSTD_adjustCParams, AOCL_Compression_zstd_ZSTD_adjustCParams_pass_common_2) { // srcSize unknown
    ZSTD_compressionParameters cPar;
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    ZSTD_compressionParameters cParams = Test_ZSTD_adjustCParams(cPar, ZSTD_CONTENTSIZE_UNKNOWN, 1204);
    validate_parameters(cParams);
}

TEST_F(ZSTD_ZSTD_adjustCParams, AOCL_Compression_zstd_ZSTD_adjustCParams_pass_common_3) { // dictSize 0
    ZSTD_compressionParameters cPar;
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    ZSTD_compressionParameters cParams = Test_ZSTD_adjustCParams(cPar, 1024, 0);
    validate_parameters(cParams);
}

TEST_F(ZSTD_ZSTD_adjustCParams, AOCL_Compression_zstd_ZSTD_adjustCParams_pass_common_4) { // cPar invalid. < min
    ZSTD_compressionParameters cPar;
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    cPar.windowLog    = get_cparam_below_lower(ZSTD_c_windowLog);
    cPar.chainLog     = get_cparam_below_lower(ZSTD_c_chainLog);
    cPar.hashLog      = get_cparam_below_lower(ZSTD_c_hashLog);
    cPar.searchLog    = get_cparam_below_lower(ZSTD_c_searchLog);
    cPar.minMatch     = get_cparam_below_lower(ZSTD_c_minMatch);
    cPar.targetLength = get_cparam_below_lower(ZSTD_c_targetLength);
    ZSTD_compressionParameters cParams = Test_ZSTD_adjustCParams(cPar, 1024, 1024);
    validate_parameters(cParams);
}

TEST_F(ZSTD_ZSTD_adjustCParams, AOCL_Compression_zstd_ZSTD_adjustCParams_pass_common_5) { // cPar invalid. > max
    ZSTD_compressionParameters cPar;
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    cPar.windowLog    = get_cparam_above_upper(ZSTD_c_windowLog);
    cPar.chainLog     = get_cparam_above_upper(ZSTD_c_chainLog);
    cPar.hashLog      = get_cparam_above_upper(ZSTD_c_hashLog);
    cPar.searchLog    = get_cparam_above_upper(ZSTD_c_searchLog);
    cPar.minMatch     = get_cparam_above_upper(ZSTD_c_minMatch);
    cPar.targetLength = get_cparam_above_upper(ZSTD_c_targetLength);
    ZSTD_compressionParameters cParams = Test_ZSTD_adjustCParams(cPar, 1024, 1024);
    validate_parameters(cParams);
}
/*********************************************
 * End of ZSTD_ZSTD_adjustCParams
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_checkCParams
*********************************************/
class ZSTD_ZSTD_checkCParams : public AOCL_setup_zstd {};

TEST_F(ZSTD_ZSTD_checkCParams, AOCL_Compression_zstd_ZSTD_checkCParams_pass_common_1) { //params in valid range
    ZSTD_compressionParameters cPar; // fill out of bound values
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    ZSTD_compressionParameters cParams = Test_ZSTD_adjustCParams(cPar, 1024, 1024);
    EXPECT_EQ(Test_ZSTD_checkCParams(cParams), 0);
}

TEST_F(ZSTD_ZSTD_checkCParams, AOCL_Compression_zstd_ZSTD_checkCParams_fail_common_2) { // cPar invalid. < min
    ZSTD_compressionParameters cPar;
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    cPar.windowLog    = get_cparam_below_lower(ZSTD_c_windowLog);
    cPar.chainLog     = get_cparam_below_lower(ZSTD_c_chainLog);
    cPar.hashLog      = get_cparam_below_lower(ZSTD_c_hashLog);
    cPar.searchLog    = get_cparam_below_lower(ZSTD_c_searchLog);
    cPar.minMatch     = get_cparam_below_lower(ZSTD_c_minMatch);
    cPar.targetLength = get_cparam_below_lower(ZSTD_c_targetLength);
    EXPECT_EQ(Test_ZSTD_checkCParams(cPar), ERROR(parameter_outOfBound));
}

TEST_F(ZSTD_ZSTD_checkCParams, AOCL_Compression_zstd_ZSTD_checkCParams_fail_common_3) { // cPar invalid. > max
    ZSTD_compressionParameters cPar;
    memset(&cPar, 0, sizeof(ZSTD_compressionParameters));
    cPar.windowLog    = get_cparam_above_upper(ZSTD_c_windowLog);
    cPar.chainLog     = get_cparam_above_upper(ZSTD_c_chainLog);
    cPar.hashLog      = get_cparam_above_upper(ZSTD_c_hashLog);
    cPar.searchLog    = get_cparam_above_upper(ZSTD_c_searchLog);
    cPar.minMatch     = get_cparam_above_upper(ZSTD_c_minMatch);
    cPar.targetLength = get_cparam_above_upper(ZSTD_c_targetLength);
    EXPECT_EQ(Test_ZSTD_checkCParams(cPar), ERROR(parameter_outOfBound));
}
/*********************************************
 * End of ZSTD_ZSTD_checkCParams
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_decodingBufferSize_min
*********************************************/
TEST(ZSTD_ZSTD_decodingBufferSize_min, AOCL_Compression_zstd_ZSTD_decodingBufferSize_pass_common_1) { // frameContentSize known
    unsigned long long frameContentSize = ZSTD_BLOCKSIZE_MAX * 3;
    size_t res = 0;
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_windowLog);
    CHECK_PASS_ZSTD(bounds.error);

    res = Test_ZSTD_decodingBufferSize_min(ZSTD_BLOCKSIZE_MAX - 1, frameContentSize); // windowSize < ZSTD_BLOCKSIZE_MAX
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);

    res = Test_ZSTD_decodingBufferSize_min(ZSTD_BLOCKSIZE_MAX + 1, frameContentSize); // windowSize > ZSTD_BLOCKSIZE_MAX
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);

    res = Test_ZSTD_decodingBufferSize_min(bounds.upperBound, frameContentSize); // windowSize lower bound
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);

    res = Test_ZSTD_decodingBufferSize_min(bounds.lowerBound, frameContentSize); // windowSize upper bound
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);

    frameContentSize = ZSTD_BLOCKSIZE_MAX / 2;

    res = Test_ZSTD_decodingBufferSize_min(ZSTD_BLOCKSIZE_MAX - 1, frameContentSize); // windowSize < ZSTD_BLOCKSIZE_MAX
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);

    res = Test_ZSTD_decodingBufferSize_min(ZSTD_BLOCKSIZE_MAX + 1, frameContentSize); // windowSize > ZSTD_BLOCKSIZE_MAX
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);

    res = Test_ZSTD_decodingBufferSize_min(bounds.upperBound, frameContentSize); // windowSize lower bound
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);

    res = Test_ZSTD_decodingBufferSize_min(bounds.lowerBound, frameContentSize); // windowSize upper bound
    CHECK_PASS_ZSTD(res); EXPECT_LE(res, frameContentSize);
}

TEST(ZSTD_ZSTD_decodingBufferSize_min, AOCL_Compression_zstd_ZSTD_decodingBufferSize_fail_common_2) { // frameContentSize unknown
    size_t res = 0;
    ZSTD_bounds bounds = Test_ZSTD_cParam_getBounds(ZSTD_c_windowLog);
    CHECK_PASS_ZSTD(bounds.error);

    res = Test_ZSTD_decodingBufferSize_min(ZSTD_BLOCKSIZE_MAX - 1, ZSTD_CONTENTSIZE_UNKNOWN); // windowSize < ZSTD_BLOCKSIZE_MAX
    CHECK_PASS_ZSTD(res);

    res = Test_ZSTD_decodingBufferSize_min(ZSTD_BLOCKSIZE_MAX + 1, ZSTD_CONTENTSIZE_UNKNOWN); // windowSize > ZSTD_BLOCKSIZE_MAX
    CHECK_PASS_ZSTD(res);

    res = Test_ZSTD_decodingBufferSize_min(bounds.upperBound, ZSTD_CONTENTSIZE_UNKNOWN); // windowSize lower bound
    CHECK_PASS_ZSTD(res);

    res = Test_ZSTD_decodingBufferSize_min(bounds.lowerBound, ZSTD_CONTENTSIZE_UNKNOWN); // windowSize upper bound
    CHECK_PASS_ZSTD(res);
}
/*********************************************
 * End of ZSTD_ZSTD_decodingBufferSize_min
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_findDecompressedSize
*********************************************/
class ZSTD_ZSTD_findDecompressedSize : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_pass_common_1) { // single frame
    create_frame_reference();
    size_t ret = Test_ZSTD_findDecompressedSize(src, srcLen);
    EXPECT_EQ(ret, d->getOrigSize());
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_pass_common_2) { // single skippable frame
    create_frame_skippable(rand() % 15);
    size_t ret = Test_ZSTD_findDecompressedSize(src, srcLen);
    EXPECT_EQ(ret, 0); // only skip frame exists. hence decompressed size is expected to be 0
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_pass_common_3) { // both zstd frame and skippable frame
    create_frame_and_skippable();
    size_t ret = Test_ZSTD_findDecompressedSize(src, srcLen);
    EXPECT_EQ(ret, d->getOrigSize() / 2); // only half of the data was decomrpessed. rest put into skippable.
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_pass_common_4) { // multiple frames
    size_t srcWritten = create_frames_multiple();
    size_t ret = Test_ZSTD_findDecompressedSize(src, srcLen);
    EXPECT_EQ(ret, srcWritten); // decompressed size of all data in all successive frames
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_fail_common_5) { // invalid src with data after frame boundary
    create_frame_overwrite();
    size_t ret = Test_ZSTD_findDecompressedSize(src, srcLen);
    EXPECT_EQ(ret, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_fail_common_6) { // decompressedSize field not set
    create_frame_with_no_decomp_size();
    size_t ret = Test_ZSTD_findDecompressedSize(src, srcLen);
    EXPECT_EQ(ret, ZSTD_CONTENTSIZE_UNKNOWN);
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_fail_common_7) { // srcLen < the size of frame contained
    create_frame_reference();
    size_t ret = Test_ZSTD_findDecompressedSize(src, srcLen - 1);
    EXPECT_EQ(ret, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_fail_common_8) { // src is NULL
    create_frame_reference();
    size_t ret = Test_ZSTD_findDecompressedSize(NULL, srcLen);
    EXPECT_EQ(ret, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_pass_common_9) { // srcLen = 0
    create_frame_reference();
    size_t ret = Test_ZSTD_findDecompressedSize(src, 0);
    EXPECT_EQ(ret, 0);
}

TEST_F(ZSTD_ZSTD_findDecompressedSize, AOCL_Compression_zstd_ZSTD_findDecompressedSize_pass_common_10) { // src is NULL and srcLen = 0
    create_frame_reference();
    size_t ret = Test_ZSTD_findDecompressedSize(NULL, 0);
    EXPECT_EQ(ret, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_findDecompressedSize
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_findFrameCompressedSize
*********************************************/
class ZSTD_ZSTD_findFrameCompressedSize : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_pass_common_1) { // single frame
    create_frame_reference();
    size_t ret = Test_ZSTD_findFrameCompressedSize(src, srcLen);
    EXPECT_EQ(ret, srcLen);
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_pass_common_2) { // single skippable frame
    create_frame_skippable(rand() % 15);
    size_t ret = Test_ZSTD_findFrameCompressedSize(src, srcLen);
    EXPECT_EQ(ret, srcLen);
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_pass_common_3) { // both zstd frame and skippable frame
    create_frame_and_skippable();
    while (srcLen > 0) {
        size_t frameSrcSize = Test_ZSTD_findFrameCompressedSize(src, srcLen); //get compressed size 1 frame at a time
        CHECK_PASS_ZSTD(frameSrcSize);
        src = src + frameSrcSize;
        srcLen -= frameSrcSize;
    }
    EXPECT_EQ(srcLen, 0); // all compressed frame sizes should add up to srcLen
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_pass_common_4) { // multiple frames
    size_t srcWritten = create_frames_multiple();
    while (srcLen > 0) {
        size_t frameSrcSize = Test_ZSTD_findFrameCompressedSize(src, srcLen); //get compressed size 1 frame at a time
        CHECK_PASS_ZSTD(frameSrcSize);
        src = src + frameSrcSize;
        srcLen -= frameSrcSize;
    }
    EXPECT_EQ(srcLen, 0); // all compressed frame sizes should add up to srcLen
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_pass_common_5) { // invalid src with data after frame boundary
    create_frame_overwrite();
    size_t ret = Test_ZSTD_findFrameCompressedSize(src, srcLen);
    EXPECT_LT(ret, srcLen); // bytes after 1st frame not read yet
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_pass_common_6) { // decompressedSize field not set
    create_frame_with_no_decomp_size();
    size_t ret = Test_ZSTD_findFrameCompressedSize(src, srcLen);
    EXPECT_EQ(ret, srcLen);
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_fail_common_7) { // srcLen < the size of frame contained
    create_frame_reference();
    size_t ret = Test_ZSTD_findFrameCompressedSize(src, srcLen - 1);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_fail_common_8) { // src is null
    create_frame_reference();
    size_t ret = Test_ZSTD_findFrameCompressedSize(NULL, srcLen);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_fail_common_9) { // srcLen = 0
    create_frame_reference();
    size_t ret = Test_ZSTD_findFrameCompressedSize(src, 0);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_findFrameCompressedSize, AOCL_Compression_zstd_ZSTD_findFrameCompressedSize_fail_common_10) { // src is null and srcLen = 0
    create_frame_reference();
    size_t ret = Test_ZSTD_findFrameCompressedSize(NULL, 0);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}
/*********************************************
 * End of ZSTD_ZSTD_findFrameCompressedSize
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_frameHeaderSize
*********************************************/
class ZSTD_ZSTD_frameHeaderSize : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_frameHeaderSize, AOCL_Compression_zstd_ZSTD_frameHeaderSize_pass_common_1) { // valid frame
    create_frame_reference();
    size_t fhSize = Test_ZSTD_frameHeaderSize(src, srcLen);
    CHECK_PASS_ZSTD(fhSize);

    //validate if correct frame size is returned by passing the stream with just the header to get header function
    ZSTD_frameHeader fh;
    CHECK_PASS_ZSTD(Test_ZSTD_getFrameHeader(&fh, src, fhSize));
}

TEST_F(ZSTD_ZSTD_frameHeaderSize, AOCL_Compression_zstd_ZSTD_frameHeaderSize_fail_common_2)   // srcLen < minimum header size
{
    create_frame_reference();
    size_t fhSize = Test_ZSTD_frameHeaderSize(src, 4); // minimum input size required to query frame header size = 5 bytes
    EXPECT_EQ(fhSize, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_frameHeaderSize, AOCL_Compression_zstd_ZSTD_frameHeaderSize_fail_common_3)   // src is null
{
    create_frame_reference();
    size_t fhSize = Test_ZSTD_frameHeaderSize(NULL, srcLen);
    EXPECT_EQ(fhSize, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_frameHeaderSize, AOCL_Compression_zstd_ZSTD_frameHeaderSize_fail_common_4)   // srcLen = 0
{
    create_frame_reference();
    size_t fhSize = Test_ZSTD_frameHeaderSize(src, 0);
    EXPECT_EQ(fhSize, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_frameHeaderSize, AOCL_Compression_zstd_ZSTD_frameHeaderSize_fail_common_5)   // src is null and srcLen = 0
{
    create_frame_reference();
    size_t fhSize = Test_ZSTD_frameHeaderSize(NULL, 0);
    EXPECT_EQ(fhSize, ERROR(srcSize_wrong));
}
/*********************************************
 * End of ZSTD_ZSTD_frameHeaderSize
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_getFrameHeader
*********************************************/
enum class ZSTD_frameHeaderType { simple, adv, adv_no_magic };
class ZSTD_ZSTD_getFrameHeader_base : public ZSTD_ZSTD_decompress_base {
public:
    void zstd_frames_flags_set(ZSTD_frameHeaderType fht) { // valid zstd frames flags set
        ZSTD_frameParameters fparams;
        fparams.checksumFlag = 1;
        fparams.contentSizeFlag = 1;
        fparams.noDictIDFlag = 1;
        create_frame_with_params(fparams);
        skip_until_zstd_frame();

        ZSTD_frameHeader fh;
        size_t ret = run_get_frameHeader(fht, &fh, src, srcLen);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(fh.checksumFlag, 1);
        EXPECT_NE(fh.frameContentSize, ZSTD_CONTENTSIZE_UNKNOWN);
        EXPECT_EQ(fh.dictID, 0);
        EXPECT_EQ(fh.frameType, ZSTD_frame);
        EXPECT_GT(fh.headerSize, 0);
    }

    void zstd_frames_flags_not_set(ZSTD_frameHeaderType fht) { // valid zstd frames flags not set
        ZSTD_frameParameters fparams;
        fparams.checksumFlag = 0;
        fparams.contentSizeFlag = 0;
        fparams.noDictIDFlag = 0;
        create_frame_with_params(fparams);
        skip_until_zstd_frame();

        ZSTD_frameHeader fh;
        size_t ret = Test_ZSTD_getFrameHeader(&fh, src, srcLen);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(fh.checksumFlag, 0);
        EXPECT_EQ(fh.frameContentSize, ZSTD_CONTENTSIZE_UNKNOWN);
        EXPECT_EQ(fh.dictID, 0);
        EXPECT_EQ(fh.frameType, ZSTD_frame);
        EXPECT_GT(fh.headerSize, 0);
    }

    void zstd_skippable_frames(ZSTD_frameHeaderType fht) { // skippable frames
        create_frame_skippable(rand() % 15);
        ZSTD_frameHeader fh;
        size_t ret = run_get_frameHeader(fht, &fh, src, srcLen);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(fh.frameType, ZSTD_skippableFrame);
        EXPECT_EQ(fh.headerSize, ZSTD_SKIPPABLEHEADERSIZE);
    }

    size_t zstd_srcLen_small(ZSTD_frameHeaderType fht, int smallLen) { // srcLen < minimum header size
        create_frame_reference();
        ZSTD_frameHeader fh;
        return run_get_frameHeader(fht, &fh, src, smallLen); // minimum input size required to query frame header size
    }

    void zstd_src_null(ZSTD_frameHeaderType fht) { // src is null
        create_frame_reference();
        ZSTD_frameHeader fh;
        size_t ret = run_get_frameHeader(fht, &fh, NULL, srcLen);
        EXPECT_EQ(ret, ERROR(GENERIC));
    }

    size_t zstd_src_len_0(ZSTD_frameHeaderType fht) { // srcLen = 0
        create_frame_reference();
        ZSTD_frameHeader fh;
        return run_get_frameHeader(fht, &fh, src, 0); // minimum input size required to query frame header size
    }

    size_t zstd_src_null_len_0(ZSTD_frameHeaderType fht) { // src is null and srcLen = 0
        create_frame();
        ZSTD_frameHeader fh;
        return run_get_frameHeader(fht, &fh, NULL, 0); // minimum input size required to query frame header size
    }

    void zstd_src_invalid_format(ZSTD_frameHeaderType fht) { // src invalid format
        create_frame_prefix_unknown();
        ZSTD_frameHeader fh;
        size_t ret = run_get_frameHeader(fht, &fh, src, srcLen);
        EXPECT_EQ(ret, ERROR(prefix_unknown));
    }

    void zstd_wrong_data(ZSTD_frameHeaderType fht) { // header ok, but wrong data
        create_frame_invalid_data_block();
        ZSTD_frameHeader fh;
        size_t ret = run_get_frameHeader(fht, &fh, src, srcLen);
        EXPECT_EQ(ret, 0);
    }

private:
    size_t run_get_frameHeader(ZSTD_frameHeaderType fht, ZSTD_frameHeader* zfhPtr, char* src, size_t srcSize) {
        switch (fht) {
        case ZSTD_frameHeaderType::simple:
        {
            return Test_ZSTD_getFrameHeader(zfhPtr, src, srcSize);
        }
        case ZSTD_frameHeaderType::adv:
        {
            return Test_ZSTD_getFrameHeader_advanced(zfhPtr, src, srcSize, ZSTD_f_zstd1);
        }
        case ZSTD_frameHeaderType::adv_no_magic:
        {
            /* ZSTD_f_zstd1_magicless: Variant of zstd frame format, without initial 4 - bytes magic number.
            *  Skipping first 4 bytes in src to test in this mode */
            if (src != NULL && srcSize >= 4)
                return Test_ZSTD_getFrameHeader_advanced(zfhPtr, src + 4, srcSize - 4, ZSTD_f_zstd1_magicless);
            else
                return Test_ZSTD_getFrameHeader_advanced(zfhPtr, NULL, srcSize, ZSTD_f_zstd1_magicless); // testing negative cases only
        }
        default:
            return ERROR(GENERIC);
        }

    }
};

class ZSTD_ZSTD_getFrameHeader : public ZSTD_ZSTD_getFrameHeader_base {};

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_pass_common_1) { 
    zstd_frames_flags_set(ZSTD_frameHeaderType::simple);
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_pass_common_2) {
    zstd_frames_flags_not_set(ZSTD_frameHeaderType::simple);
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_pass_common_3) {
    zstd_skippable_frames(ZSTD_frameHeaderType::simple);
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_fail_common_4) {
    size_t ret = zstd_srcLen_small(ZSTD_frameHeaderType::simple, 4);
    EXPECT_EQ(ret, 5); // wanted `srcSize` amount. fh not filled
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_fail_common_5) {
    zstd_src_null(ZSTD_frameHeaderType::simple);
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_fail_common_6) {
    size_t ret = zstd_src_len_0(ZSTD_frameHeaderType::simple);
    EXPECT_EQ(ret, 5); // wanted `srcSize` amount. fh not filled
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_fail_common_7) {
    size_t ret = zstd_src_null_len_0(ZSTD_frameHeaderType::simple);
    EXPECT_EQ(ret, 5); // wanted `srcSize` amount. fh not filled
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_fail_common_8) {
    zstd_src_invalid_format(ZSTD_frameHeaderType::simple);
}

TEST_F(ZSTD_ZSTD_getFrameHeader, AOCL_Compression_zstd_ZSTD_getFrameHeader_pass_common_9) {
    zstd_wrong_data(ZSTD_frameHeaderType::simple);
}
/*********************************************
 * End of ZSTD_ZSTD_getFrameHeader
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_getFrameHeader_advanced
*********************************************/
class ZSTD_ZSTD_getFrameHeader_advanced : public ZSTD_ZSTD_getFrameHeader_base {};

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_pass_common_1) {
    zstd_frames_flags_set(ZSTD_frameHeaderType::adv);
    zstd_frames_flags_set(ZSTD_frameHeaderType::adv_no_magic);
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_pass_common_2) {
    zstd_frames_flags_not_set(ZSTD_frameHeaderType::adv);
    zstd_frames_flags_not_set(ZSTD_frameHeaderType::adv_no_magic);
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_pass_common_3) {
    zstd_skippable_frames(ZSTD_frameHeaderType::adv);
    //zstd_skippable_frames(ZSTD_frameHeaderType::adv_no_magic); as there is no magic number, can't distinguish skippable frame from normal frame
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_fail_common_4) { // srcLen < minimum header size
    {
        size_t ret = zstd_srcLen_small(ZSTD_frameHeaderType::adv, 4);
        EXPECT_EQ(ret, 5); // wanted `srcSize` amount. fh not filled
    }
    {
        size_t ret = zstd_srcLen_small(ZSTD_frameHeaderType::adv_no_magic, 4);
        EXPECT_EQ(ret, 1); // wanted `srcSize` amount. fh not filled
    }
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_fail_common_5) { // src is null
    zstd_src_null(ZSTD_frameHeaderType::adv);
    zstd_src_null(ZSTD_frameHeaderType::adv_no_magic);
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_fail_common_6) { // srcLen = 0
    {
        size_t ret = zstd_src_len_0(ZSTD_frameHeaderType::adv);
        EXPECT_EQ(ret, 5); // wanted `srcSize` amount. fh not filled
    }
    {
        size_t ret = zstd_src_len_0(ZSTD_frameHeaderType::adv_no_magic);
        EXPECT_EQ(ret, 1); // wanted `srcSize` amount. fh not filled
    }
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_fail_common_7) { // src is null and srcLen = 0
    {
        size_t ret = zstd_src_null_len_0(ZSTD_frameHeaderType::adv);
        EXPECT_EQ(ret, 5); // wanted `srcSize` amount. fh not filled
    }
    {
        size_t ret = zstd_src_null_len_0(ZSTD_frameHeaderType::adv_no_magic);
        EXPECT_EQ(ret, 1); // wanted `srcSize` amount. fh not filled
    }
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_fail_common_8) { // src invalid format
    zstd_src_invalid_format(ZSTD_frameHeaderType::adv);
    //zstd_src_invalid_format(ZSTD_frameHeaderType::adv_no_magic); not applicable as there is no magic number to invalidate here
}

TEST_F(ZSTD_ZSTD_getFrameHeader_advanced, AOCL_Compression_zstd_ZSTD_getFrameHeader_advanced_pass_common_9) { // header ok, but wrong data
    zstd_wrong_data(ZSTD_frameHeaderType::adv);
    zstd_wrong_data(ZSTD_frameHeaderType::adv_no_magic);
}
/*********************************************
 * End of ZSTD_ZSTD_getFrameHeader_advanced
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_isSkippableFrame
*********************************************/
class ZSTD_ZSTD_isSkippableFrame : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_isSkippableFrame, AOCL_Compression_zstd_ZSTD_isSkippableFrame_pass_common_1) { // skippable frame
    create_frame_skippable(rand() % 15);
    EXPECT_TRUE(Test_ZSTD_isSkippableFrame(src, srcLen));
}

TEST_F(ZSTD_ZSTD_isSkippableFrame, AOCL_Compression_zstd_ZSTD_isSkippableFrame_fail_common_2) { // zstd frame
    create_frame_reference();
    EXPECT_FALSE(Test_ZSTD_isSkippableFrame(src, srcLen));
}

TEST_F(ZSTD_ZSTD_isSkippableFrame, AOCL_Compression_zstd_ZSTD_isSkippableFrame_fail_common_3) { // invalid frame prefix
    create_frame_prefix_unknown();
    EXPECT_FALSE(Test_ZSTD_isSkippableFrame(src, srcLen));
}

TEST_F(ZSTD_ZSTD_isSkippableFrame, AOCL_Compression_zstd_ZSTD_isSkippableFrame_fail_common_4) { // src is null
    create_frame_skippable(rand() % 15);
    EXPECT_FALSE(Test_ZSTD_isSkippableFrame(NULL, srcLen));
}

TEST_F(ZSTD_ZSTD_isSkippableFrame, AOCL_Compression_zstd_ZSTD_isSkippableFrame_fail_common_5) { // srcLen < magic number
    create_frame_skippable(rand() % 15);
    EXPECT_FALSE(Test_ZSTD_isSkippableFrame(src, 3));
}

TEST_F(ZSTD_ZSTD_isSkippableFrame, AOCL_Compression_zstd_ZSTD_isSkippableFrame_fail_common_6) { // srcLen = 0
    create_frame_skippable(rand() % 15);
    EXPECT_FALSE(Test_ZSTD_isSkippableFrame(src, 0));
}

TEST_F(ZSTD_ZSTD_isSkippableFrame, AOCL_Compression_zstd_ZSTD_isSkippableFrame_fail_common_7) { // src is null and srcLen = 0
    create_frame_skippable(rand() % 15);
    EXPECT_FALSE(Test_ZSTD_isSkippableFrame(NULL, 0));
}
/*********************************************
 * End of ZSTD_ZSTD_isSkippableFrame
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_readSkippableFrame
*********************************************/
class ZSTD_ZSTD_readSkippableFrame : public ZSTD_ZSTD_decompress_base {};

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_pass_common_1) { // skippable frame
    create_frame_skippable(5);
    unsigned magicVarint = 0;
    size_t ret = Test_ZSTD_readSkippableFrame(output, outLen, &magicVarint, src, srcLen);
    EXPECT_EQ(ret, origLen);
    EXPECT_EQ(magicVarint, 5);
    EXPECT_EQ(memcmp(original, output, origLen), 0);
}

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_pass_common_2) { // skippable frame magic varint not set
    create_frame_skippable(5);
    size_t ret = Test_ZSTD_readSkippableFrame(output, outLen, NULL, src, srcLen);
    EXPECT_EQ(ret, origLen);
    EXPECT_EQ(memcmp(original, output, origLen), 0);
}

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_fail_common_3) { // zstd frame
    create_frame_reference();
    size_t ret = Test_ZSTD_readSkippableFrame(output, outLen, NULL, src, srcLen);
    EXPECT_EQ(ret, ERROR(frameParameter_unsupported));
}

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_fail_common_4) { // dstCapacity insufficient
    create_frame_skippable(5);
    size_t ret = Test_ZSTD_readSkippableFrame(output, outLen / 2, NULL, src, srcLen);
    EXPECT_EQ(ret, ERROR(dstSize_tooSmall));
}

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_fail_common_5) { // src is null
    create_frame_skippable(5);
    size_t ret = Test_ZSTD_readSkippableFrame(output, outLen, NULL, NULL, srcLen);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_fail_common_6) { // srcLen = 0
    create_frame_skippable(5);
    size_t ret = Test_ZSTD_readSkippableFrame(output, outLen, NULL, src, 0);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_fail_common_7) { // src is null and srcLen = 0
    create_frame_skippable(5);
    size_t ret = Test_ZSTD_readSkippableFrame(output, outLen, NULL, NULL, 0);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_ZSTD_readSkippableFrame, AOCL_Compression_zstd_ZSTD_readSkippableFrame_pass_common_8) { // dst is null
    create_frame_skippable(5);
    size_t ret = Test_ZSTD_readSkippableFrame(NULL, outLen, NULL, src, srcLen);
    EXPECT_EQ(ret, origLen); //if dst is NULL, it returns skippable frame data size but does not copy any data
}
/*********************************************
 * End of ZSTD_ZSTD_readSkippableFrame
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_compressBound
*********************************************/
TEST(ZSTD_ZSTD_compressBound, AOCL_Compression_zstd_ZSTD_compressBound_pass_common_1) { // valid
    size_t ret = Test_ZSTD_compressBound(0);
    EXPECT_GT(ret, 0);

    ret = Test_ZSTD_compressBound(1024);
    EXPECT_GT(ret, 1024);

    // max supported value
#if AOCL_DECOMPRESS_FAST > 1
    // max supported value when AOCL_DECOMPRESS_FAST > 1 is `ZSTD_MAX_INPUT_SIZE - 23`
    // to account for FDS skippable frame.
    ret = Test_ZSTD_compressBound(ZSTD_MAX_INPUT_SIZE - 23);
    EXPECT_GT(ret, ZSTD_MAX_INPUT_SIZE - 23);
#else
    ret = Test_ZSTD_compressBound(ZSTD_MAX_INPUT_SIZE - 1);
    EXPECT_GT(ret, ZSTD_MAX_INPUT_SIZE - 1);
#endif
}

TEST(ZSTD_ZSTD_compressBound, AOCL_Compression_zstd_ZSTD_compressBound_fail_common_2) { // src size out of bounds
#if AOCL_DECOMPRESS_FAST > 1
    size_t ret = Test_ZSTD_compressBound(ZSTD_MAX_INPUT_SIZE - 22);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
#else
    size_t ret = Test_ZSTD_compressBound(ZSTD_MAX_INPUT_SIZE);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
#endif   
}
/*********************************************
 * End of ZSTD_ZSTD_compressBound
 *********************************************/

/*********************************************
* Begin of ZSTD_AOCL_ZSTD_compressBound
*********************************************/
class ZSTD_AOCL_ZSTD_compressBound : public AOCL_setup_zstd
{
    public:
        ZSTD_parameters params;

        void Test_AOCL_ZSTD_compressBound_for_windowLog_range(size_t srcSize) {
            size_t upperBound = Test_ZSTD_compressBound(srcSize);
            for (int i = ZSTD_WINDOWLOG_MIN; i < ZSTD_WINDOWLOG_MAX; i++) {
                params.cParams.windowLog = i;
                size_t ret = Test_AOCL_ZSTD_compressBound(srcSize, params);
                EXPECT_GT(ret, srcSize);
                EXPECT_LE(ret, upperBound);
            }
        }
};
TEST_F(ZSTD_AOCL_ZSTD_compressBound, AOCL_Compression_zstd_AOCL_ZSTD_compressBound_pass_common_1) { // valid
    
    Test_AOCL_ZSTD_compressBound_for_windowLog_range(0);
    Test_AOCL_ZSTD_compressBound_for_windowLog_range(1024);

    // max supported value
#if AOCL_DECOMPRESS_FAST > 1
    // max supported value when AOCL_DECOMPRESS_FAST > 1 is `ZSTD_MAX_INPUT_SIZE - 23`
    // to account for FDS skippable frame.
    Test_AOCL_ZSTD_compressBound_for_windowLog_range(ZSTD_MAX_INPUT_SIZE - 23);
#else
    Test_AOCL_ZSTD_compressBound_for_windowLog_range(ZSTD_MAX_INPUT_SIZE - 1);
#endif
}

TEST_F(ZSTD_AOCL_ZSTD_compressBound, AOCL_Compression_zstd_AOCL_ZSTD_compressBound_fail_common_2) { // src size out of bounds

    // If srcSize is out of bound, irrespective of windowLog, AOCL_ZSTD_compressBound() returns ERROR(srcSize_wrong).
    params.cParams.windowLog = ZSTD_WINDOWLOG_MIN;

#if AOCL_DECOMPRESS_FAST > 1
    size_t ret = Test_AOCL_ZSTD_compressBound(ZSTD_MAX_INPUT_SIZE - 22, params);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
#else
    size_t ret = Test_AOCL_ZSTD_compressBound(ZSTD_MAX_INPUT_SIZE, params);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
#endif   
}

#ifdef AOCL_ENABLE_THREADS
TEST_F(ZSTD_AOCL_ZSTD_compressBound, AOCL_Compression_zstd_AOCL_ZSTD_compressBound_fail_common_3) { // invalid windowLog

    // valid windowLog range [ZSTD_WINDOWLOG_MIN, ZSTD_WINDOWLOG_MAX)
    params.cParams.windowLog = ZSTD_WINDOWLOG_MAX;
    EXPECT_EQ(Test_AOCL_ZSTD_compressBound(1024, params), ERROR(srcSize_wrong)); 
}
#endif


/*********************************************
 * End of ZSTD_AOCL_ZSTD_compressBound
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_decomrpessBound
 *********************************************/
class ZSTD_ZSTD_decompressBound : public AOCL_setup_zstd, public ZSTD_frame_creator {};

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_1) { // zstd frame
    create_frame_reference();
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_GT(ret, 0);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_2) { // skippable frame
    create_frame_skippable(rand() % 15);
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_EQ(ret, 0);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_3) { // multiple frames
    create_frames_multiple();
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_GT(ret, 0);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_4) { // frame and skippable
    create_frame_and_skippable();
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_GT(ret, 0);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_5) { // frame with no decomp size
    create_frame_with_no_decomp_size();
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_GE(ret, ZSTD_BLOCKSIZE_MAX); // if unknown, estimate based on num_of_blocks * max_block_size
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_6) { // frame with params
    ZSTD_frameParameters fparams{ 1, 1, 1 };
    create_frame_with_params(fparams);
    skip_until_zstd_frame();
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_GT(ret, 0);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_fail_common_7) { // invalid frame prefix
    create_frame_prefix_unknown();
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_EQ(ret, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_8) { // empty frame
    create_empty_frame();
    size_t ret = Test_ZSTD_decompressBound(src, srcLen);
    EXPECT_EQ(ret, 0);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_fail_common_9) { // src is null
    create_frame_reference();
    size_t ret = Test_ZSTD_decompressBound(NULL, srcLen);
    EXPECT_EQ(ret, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_fail_common_10) { // srcLen < ZSTD_SKIPPABLEHEADERSIZE
    create_frame_reference();
    size_t ret = Test_ZSTD_decompressBound(src, ZSTD_SKIPPABLEHEADERSIZE - 1);
    EXPECT_EQ(ret, ZSTD_CONTENTSIZE_ERROR);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_fail_common_11) { // src is null and srcLen = 0
    create_frame_reference();
    size_t ret = Test_ZSTD_decompressBound(NULL, 0);
    EXPECT_EQ(ret, 0);
}

TEST_F(ZSTD_ZSTD_decompressBound, AOCL_Compression_zstd_ZSTD_decompressBound_pass_common_12) { // srcLen = 0
    create_frame_reference();
    size_t ret = Test_ZSTD_decompressBound(src, 0);
    EXPECT_EQ(ret, 0);
}
/*********************************************
 * End of ZSTD_ZSTD_decompressBound
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_cParam_getBounds
*********************************************/
TEST(ZSTD_ZSTD_cParam_getBounds, AOCL_Compression_zstd_ZSTD_cParam_getBounds_pass_common_1) { // valid param
    ZSTD_bounds ret = Test_ZSTD_cParam_getBounds(ZSTD_c_compressionLevel);
    CHECK_PASS_ZSTD(ret.error);
}

TEST(ZSTD_ZSTD_cParam_getBounds, AOCL_Compression_zstd_ZSTD_cParam_getBounds_pass_common_2) { // experimental param
    ZSTD_bounds ret = Test_ZSTD_cParam_getBounds(ZSTD_c_experimentalParam1);
    CHECK_PASS_ZSTD(ret.error);
}

TEST(ZSTD_ZSTD_cParam_getBounds , AOCL_Compression_zstd_ZSTD_cParam_getBounds_fail_common_3) { // invalid param
    ZSTD_bounds ret = Test_ZSTD_cParam_getBounds((ZSTD_cParameter)5555);
    CHECK_FAIL_ZSTD(ret.error);
}
/*********************************************
 * End of ZSTD_ZSTD_cParam_getBounds
 *********************************************/

/*********************************************
 * Begin of ZSTD_ZSTD_dParam_getBounds
 **********************************************/
TEST(ZSTD_ZSTD_dParam_getBounds, AOCL_Compression_zstd_ZSTD_dParam_getBounds_pass_common_1) { // valid param
    ZSTD_bounds ret = Test_ZSTD_dParam_getBounds(ZSTD_d_windowLogMax);
    CHECK_PASS_ZSTD(ret.error);
}

TEST(ZSTD_ZSTD_dParam_getBounds, AOCL_Compression_zstd_ZSTD_dParam_getBounds_pass_common_2) { // experimental param
    ZSTD_bounds ret = Test_ZSTD_dParam_getBounds(ZSTD_d_experimentalParam1);
    CHECK_PASS_ZSTD(ret.error);
}

TEST(ZSTD_ZSTD_dParam_getBounds, AOCL_Compression_zstd_ZSTD_dParam_getBounds_fail_common_3) { // invalid param
    ZSTD_bounds ret = Test_ZSTD_dParam_getBounds((ZSTD_dParameter)5555);
    CHECK_FAIL_ZSTD(ret.error);
}
/*********************************************
 * End of ZSTD_ZSTD_dParam_getBounds
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_defaultCLevel
*********************************************/
TEST(ZSTD_ZSTD_defaultCLevel, AOCL_Compression_zstd_ZSTD_defaultCLevel_pass_common_1) {
    EXPECT_EQ(Test_ZSTD_defaultCLevel(), ZSTD_CLEVEL_DEFAULT);
}
/*********************************************
 * End of ZSTD_ZSTD_defaultCLevel
 *********************************************/

/*********************************************
* Begin of ZSTD_ZSTD_minCLevel
*********************************************/
TEST(ZSTD_ZSTD_minCLevel, AOCL_Compression_zstd_ZSTD_minCLevel_pass_common_1) {
    EXPECT_LT(Test_ZSTD_minCLevel(), ZSTD_CLEVEL_DEFAULT);
}
/*********************************************
 * End of ZSTD_ZSTD_minCLevel
 *********************************************/

 /*********************************************
 * Begin of ZSTD_ZSTD_maxCLevel
 *********************************************/
TEST(ZSTD_ZSTD_maxCLevel, AOCL_Compression_zstd_ZSTD_maxCLevel_pass_common_1) {
    EXPECT_GT(Test_ZSTD_maxCLevel(), ZSTD_CLEVEL_DEFAULT);
}
/*********************************************
 * End of ZSTD_ZSTD_maxCLevel
 *********************************************/

/*********************************************
 * Begin fuzz tests for zstd
 *********************************************/
#ifdef AOCL_TEST_FUZZER
void ZSTD_compress_advanced_fuzz(vector<char> source, size_t dest_sz,
                                 int level, int optOff, int optLevel,
                                 vector<char> dict)
{
    aocl_setup_zstd_encode(optOff, optLevel, 0, 0, 0);

    size_t destLen = dest_sz;
    size_t srcLen = source.size();
    size_t dictLen = dict.size();
    vector<char> dest(destLen, 0);

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ZSTD_parameters zparams = ZSTD_getParams(level, srcLen, dictLen);
    zparams.fParams.contentSizeFlag = 1;
    int ret = ZSTD_compress_advanced(cctx, dest.data(), destLen, source.data(), srcLen, dict.data(), dictLen, zparams);
    if(!ZSTD_isError(ret))
    {
        aocl_setup_zstd_decode(optOff, optLevel, 0, 0, 0);
        size_t origlen = source.size();
        vector<char> decompressed(origlen);        

        ZSTD_DCtx* dctx = ZSTD_createDCtx();
        int ret2 = ZSTD_decompressDCtx(dctx, decompressed.data(), origlen, dest.data(), ret);
        EXPECT_EQ(ZSTD_isError(ret2), false);
        if (!ZSTD_isError(ret2))       
            ASSERT_EQ(0,memcmp(decompressed.data(),source.data(), source.size()));
        if (dctx) ZSTD_freeDCtx(dctx);

        aocl_destroy_zstd_decode();     
    }
    if (cctx) ZSTD_freeCCtx(cctx);

    aocl_destroy_zstd_encode();
}
FUZZ_TEST(AOCL_Compression_zstd, ZSTD_compress_advanced_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(-1, 22),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4),
             fuzztest::Arbitrary<vector<char>>())
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> vector<tuple<vector<char>, size_t, int, int, int, vector<char>>> {
    auto seed_files = READ_FUZZ_CPR_SEED();
    vector<tuple<vector<char>, size_t, int, int, int, vector<char>>> seeds;
    fuzz_cpr_seed_t<char> seeds_base = get_fuzz_cpr_seeds<char>([](size_t src_sz) -> size_t {
        size_t dst_sz = ZSTD_compressBound(src_sz);
        return limit_fuzz_size_max(dst_sz);
        }, -1, 22, seed_files);
    // include additional parameter in seed : dict
    for (auto& seed : seeds_base) {
        vector<char> dict(get<0>(seed).begin(), get<0>(seed).end()); // created from source data
        seeds.push_back({ get<0>(seed), get<1>(seed), get<2>(seed), get<3>(seed), get<4>(seed), dict });
    }
    return seeds;
})
#endif
;

void ZSTD_decompressDCtx_fuzz(vector<char> source, size_t dest_sz,
                              int optOff, int optLevel)
{
    aocl_setup_zstd_decode(optOff, optLevel, 0, 0, 0);

    size_t destLen = dest_sz;
    size_t srcLen = source.size();
    vector<char> dest(destLen, 0);

    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    ZSTD_decompressDCtx(dctx, dest.data(), destLen, source.data(), srcLen);
    if (dctx) ZSTD_freeDCtx(dctx);

    aocl_destroy_zstd_decode();
}
FUZZ_TEST(AOCL_Compression_zstd, ZSTD_decompressDCtx_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_dpr_seed_t<char> {
    auto seed_files = READ_FUZZ_DPR_SEED();
    return get_fuzz_dpr_seeds<char>(seed_files);
})
#endif
;

void ZSTD_compress2_fuzz(int dest_len, std::vector<char> source, std::vector<char> dict, 
                            int compressionLevel, int contentSizeFlag,int checksumFlag, int noDictIDFlag )
{
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    EXPECT_NE(cctx, nullptr);

    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;

    ZSTD_parameters zparams = ZSTD_getParams(compressionLevel, source.size(), dict.size());
    zparams.fParams.contentSizeFlag = contentSizeFlag; 
    zparams.fParams.checksumFlag = checksumFlag; 
    zparams.fParams.noDictIDFlag = noDictIDFlag; 
    ZSTD_CCtx_setParams(cctx, zparams);
    ZSTD_CCtx_loadDictionary(cctx, dict.data(), dict.size());

    std::vector<char> dest(dest_len);
    int ret = ZSTD_compress2(cctx, dest.data(),dest_len, source.data(), source.size());
    if(!ZSTD_isError(ret))
    {
        ZSTD_DCtx* dctx =  ZSTD_createDCtx();
        size_t origlen = source.size();
        std::vector<char> decompressed(origlen);
        size_t ret2 = ZSTD_decompress_usingDict(dctx, decompressed.data(), origlen,
                            dest.data(), ret,
                            dict.data(), dict.size());
        EXPECT_EQ(ZSTD_isError(ret2), false);
        if(!ZSTD_isError(ret2))
        {
            ASSERT_EQ(0,memcmp(decompressed.data(),source.data(), source.size()));
        }

        if (dctx) ZSTD_freeDCtx(dctx);
    }
    
    if (cctx) ZSTD_freeCCtx(cctx);
}
FUZZ_TEST(AOCL_Compression_zstd, ZSTD_compress2_fuzz)
    .WithDomains(fuzztest::InRange<int>(1, 10000),
                 fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 22),
                 fuzztest::InRange<int>(0, 1),
                 fuzztest::InRange<int>(0, 1),
                 fuzztest::InRange<int>(0, 1));

#ifndef AOCL_DECOMPRESS_FAST
/* TODO: Modify test case to run multiple compress/decompress calls until all frames are created/processed. 
 *       This is needed for FDS mode as well as to support non ZSTD_e_flush endop */
void ZSTD_compressStream2_fuzz(std::vector<char> source, unsigned int endop, std::vector<char> dict, 
                            int compressionLevel, int contentSizeFlag, int checksumFlag, 
                            int noDictIDFlag)
{
    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
    size_t ret = 0;
    ZSTD_EndDirective directive = ZSTD_e_flush;
    (void)(endop); // for other endop values compression might not complete in a single call. We can't test decompression and validation without flush.

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    EXPECT_NE(cctx, nullptr);

    ZSTD_parameters zparams = ZSTD_getParams(compressionLevel, source.size(), dict.size());
    zparams.fParams.contentSizeFlag = contentSizeFlag; 
    zparams.fParams.checksumFlag = checksumFlag; 
    zparams.fParams.noDictIDFlag = noDictIDFlag; 
    ZSTD_CCtx_setParams(cctx, zparams);
    ZSTD_CCtx_loadDictionary(cctx, dict.data(), dict.size());
    
    int dest_len = ZSTD_compressBound(source.size());
    std::vector<char> output(dest_len);
    buffIn.size  = source.size();
    buffIn.pos = 0;
    buffIn.src = source.data();
    buffOut.size = dest_len;
    buffOut.pos = 0;
    buffOut.dst = output.data();
    ret = ZSTD_compressStream2(cctx, &buffOut, &buffIn, directive);
    if(!ZSTD_isError(ret))
    {
        ZSTD_DStream* zds = ZSTD_createDStream();
        EXPECT_NE(zds, nullptr);

        std::vector<char> decompressed(dest_len);
        buffIn.size = buffOut.pos;
        buffIn.pos = 0;
        buffIn.src = output.data();
        buffOut.size = decompressed.size();
        buffOut.pos = 0;
        buffOut.dst = decompressed.data();
        int ret2 = ZSTD_decompressStream(zds, &buffOut, &buffIn);
        EXPECT_EQ(ZSTD_isError(ret2), false) << ZSTD_getErrorName(ret2);
        if(!ZSTD_isError(ret2))
        {
            ASSERT_EQ(0,memcmp(decompressed.data(),source.data(), source.size()));
        }
        if(zds) ZSTD_freeDStream(zds);
    }
    if (cctx) ZSTD_freeCCtx(cctx);
}

FUZZ_TEST(AOCL_Compression_zstd, ZSTD_compressStream2_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),                 
                 fuzztest::InRange<int>(0, 2),
                 fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 22),
                 fuzztest::InRange<int>(0, 1),
                 fuzztest::InRange<int>(0, 1),
                 fuzztest::InRange<int>(0, 1)
                    );
#endif /* !AOCL_DECOMPRESS_FAST */

void ZSTD_decompressStream_fuzz(std::vector<char> source, int dest_len)
{    
    ZSTD_outBuffer buffOut;
    ZSTD_inBuffer buffIn;
    size_t ret = 0;

    ZSTD_DStream* zds = ZSTD_createDStream();
    EXPECT_NE(zds, nullptr);

    std::vector<char> output(dest_len);
    buffOut.dst = output.data();
    buffIn.src = source.data();
    buffOut.size = output.size();
    buffIn.size = source.size();
    ZSTD_decompressStream(zds, &buffOut, &buffIn);
    if(zds) ZSTD_freeDStream(zds);
}
FUZZ_TEST(AOCL_Compression_zstd, ZSTD_decompressStream_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 10000)
                );

void ZSTD_compress_usingDict_fuzz(std::vector<char> input, int out_len, std::vector<char> dict, int compressionLevel)
{
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    std::vector<char> output(out_len);
    int ret = ZSTD_compress_usingDict(cctx,output.data(), out_len,
                           input.data(), input.size(),
                           dict.data(), dict.size(), compressionLevel);
    if(!ZSTD_isError(ret))
    {
        ZSTD_DCtx* dctx =  ZSTD_createDCtx();
        size_t origlen = input.size();
        std::vector<char> decompressed(origlen);
        size_t ret2 = ZSTD_decompress_usingDict(dctx, decompressed.data(), origlen,
                            output.data(), ret,
                            dict.data(), dict.size());
        EXPECT_EQ(ZSTD_isError(ret2), false);
        if(!ZSTD_isError(ret2))
        {
            ASSERT_EQ(0,memcmp(decompressed.data(),input.data(), input.size()));
        }

        if (dctx) ZSTD_freeDCtx(dctx);
    }
    if (cctx) ZSTD_freeCCtx(cctx);
}
FUZZ_TEST(AOCL_Compression_zstd, ZSTD_compress_usingDict_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 10000),
                 fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 22));

void ZSTD_decompress_usingDict_fuzz(std::vector<char> input, int out_len, std::vector<char> dict)
{
    ZSTD_DCtx* dctx =  ZSTD_createDCtx();
    std::vector<char> output(out_len);
    ZSTD_decompress_usingDict(dctx, output.data(), out_len,
                           input.data(), input.size(),
                           dict.data(), dict.size());
    if (dctx) ZSTD_freeDCtx(dctx);
}
FUZZ_TEST(AOCL_Compression_zstd, ZSTD_decompress_usingDict_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 10000),
                 fuzztest::Arbitrary<std::vector<char>>());

#endif /* AOCL_TEST_FUZZER */
/*********************************************
 * End fuzz tests for zstd
 *********************************************/
