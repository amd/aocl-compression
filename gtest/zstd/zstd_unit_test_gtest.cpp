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
 
 /** @file zstd_unit_test_gtest.cpp
 *  
 *  @brief Test cases for unit testing AOCL optimizations introduced in ZSTD algo.
 *
 *  This file contains the test cases for unit testing AOCL optimized functions
 *  introduced in ZSTD.
 *
 *  @author Ashish Sriram
 */
#include "zstd_gtest.h"

#ifdef AOCL_ZSTD_OPT
/*********************************************
* Begin of ZSTD_AOCL_ZSTD_row_getMatchMask
*********************************************/
class ZSTD_AOCL_ZSTD_row_getMatchMask : public ::testing::TestWithParam<int> {
public:
    typedef unsigned char BYTE;
    enum MATCH_TYPE { MATCH_TYPE_ALL, MATCH_TYPE_SOME, MATCH_TYPE_NONE };
    void SetUp() override {
        srand(0);
        rowEntries = GetParam();
        tagRow = (BYTE*)malloc(sizeof(BYTE) * rowEntries);
        headRow = (BYTE*)malloc(sizeof(BYTE) * rowEntries);
    }

    void TearDown() override {
        if (tagRow)
            free(tagRow);
        if (headRow)
            free(headRow);
    }

    //Fill bytes in tagRow based on MATCH_TYPE selected
    void setup_tag_row(BYTE tag, unsigned matchPos[], int matchSz, MATCH_TYPE type) {
        if (type == MATCH_TYPE_ALL) {
            for (int i = 0; i < rowEntries; ++i) { //fill all with tag bytes
                tagRow[i] = tag;
            }
        }
        else {
            for (int i = 0; i < rowEntries; ++i) { //fill with random non-tag bytes
                BYTE cur = rand() % 256;
                if (cur == tag)
                    cur++;
                tagRow[i] = cur;
            }
        }

        if (type == MATCH_TYPE_SOME) {
            for (int i = 0; i < matchSz; ++i) { //set some bytes to tag
                ASSERT_LT(matchPos[i], rowEntries);
                tagRow[matchPos[i]] = tag;
            }
        }
    }

    //validate if expected mask is generated
    void validate_mask(BYTE tag, U64 mask, int head) {
        //rotate tagRow left by head and save in headRow
        ASSERT_LT(head, rowEntries);
        head = (rowEntries - head) % rowEntries;
        for (int i = 0; i < rowEntries; ++i) {
            int headpos = (head + i) % rowEntries;
            headRow[headpos] = tagRow[i];
        }

        //check each bit in mask
        for (int i = 0; i < rowEntries; ++i) {
            int curbit = mask & 0x01;
            if (i == head) {
                //test head pos bit. Should always be 0.
                EXPECT_EQ(curbit, 0);
            }
            else {
                //test rest of the mask
                if (headRow[i] == tag) {
                    EXPECT_EQ(curbit, 1);
                }
                else {
                    EXPECT_EQ(curbit, 0);
                }
            }
            mask >>= 1;
        }
    }

    uint32_t rowEntries; //size of tag row
    BYTE* tagRow; //input tag row bytes
    BYTE* headRow; //temp buffer to hold rotated tag bytes
};

// all bytes set to tag
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_allMatch_common_1) {
    BYTE tag = 'a';
    const int head = 0;
    setup_tag_row(tag, { 0 }, 0, MATCH_TYPE_ALL);
    U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
    validate_mask(tag, mask, head);
}

// some bytes set to tag
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_someMatch_common_1) {
    BYTE tag = 'a';
    const int head = 0;
    unsigned matchPos[6] = { 1, 3, 7, 8, rowEntries - 3, rowEntries - 1 };
    setup_tag_row(tag, matchPos, 6, MATCH_TYPE_SOME);
    U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
    validate_mask(tag, mask, head);
}

// no bytes set to tag
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_nonematch_common_1) {
    BYTE tag = 'a';
    const int head = 0;
    setup_tag_row(tag, { 0 }, 0, MATCH_TYPE_NONE);
    U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
    validate_mask(tag, mask, head);
}

// test with head rotation
TEST_P(ZSTD_AOCL_ZSTD_row_getMatchMask, AOCL_Compression_zstd_AOCL_ZSTD_row_getMatchMask_headRotated_common_1) {
    BYTE tag = 'a';
    for (int head = 1; head < rowEntries; ++head) {
        unsigned matchPos[6] = { 1, 3, 7, 8, rowEntries - 3, rowEntries - 1 };
        setup_tag_row(tag, matchPos, 6, MATCH_TYPE_ALL);
        U64 mask = Test_AOCL_ZSTD_row_getMatchMask(tagRow, tag, head, rowEntries);
        validate_mask(tag, mask, head);
    }
}

INSTANTIATE_TEST_SUITE_P(
    AOCL_ZSTD_row_getMatchMask_TEST,
    ZSTD_AOCL_ZSTD_row_getMatchMask,
    ::testing::ValuesIn({ 16, 32, 64 })); // 3 configurations supported for rowEntries

/*********************************************
 * End of ZSTD_AOCL_ZSTD_row_getMatchMask
 *********************************************/
#endif

/*********************************************
* Begin of ZSTD_ZSTD_selectBlockCompressor
*********************************************/

  // Test valid compressors are set on optOff
TEST(ZSTD_ZSTD_selectBlockCompressor, AOCL_Compression_zstd_ZSTD_selectBlockCompressor_optOff_common_1)
{
    int aoclOptFlag = 0; //optOff
    for (int strat = 1; strat <= 9; ++strat) { //Refer to ZSTD_strategy for valid range of values
        for (int useRowMatchFinder = 0; useRowMatchFinder <= 1; useRowMatchFinder++) {
            for (int dictMode = 0; dictMode <= 3; ++dictMode) { //Refer to ZSTD_dictMode_e for valid range of values
                int ret = Test_ZSTD_selectBlockCompressor(strat, useRowMatchFinder, dictMode, aoclOptFlag);
                EXPECT_EQ(ret, 0);
            }
        }
    }
}

// Test valid compressors are set on optOn
TEST(ZSTD_ZSTD_selectBlockCompressor, AOCL_Compression_zstd_ZSTD_selectBlockCompressor_optOn_common_1)
{
    int aoclOptFlag = 1; //optOn
    for (int strat = 1; strat <= 9; ++strat) { //Refer to ZSTD_strategy for valid range of values
        for (int useRowMatchFinder = 0; useRowMatchFinder <= 1; useRowMatchFinder++) {
            for (int dictMode = 0; dictMode <= 3; ++dictMode) { //Refer to ZSTD_dictMode_e for valid range of values
                int ret = Test_ZSTD_selectBlockCompressor(strat, useRowMatchFinder, dictMode, aoclOptFlag);
                EXPECT_EQ(ret, 0);
            }
        }
    }
}

/*********************************************
* End of ZSTD_ZSTD_selectBlockCompressor
*********************************************/

#ifdef AOCL_ZSTD_OPT
/*********************************************
* Begin of ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long
*********************************************/
class ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long : public AOCL_setup_zstd
{
public:
    ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long() : stream(nullptr), src(nullptr), dst(nullptr) {}

    ~ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long() {
        if (stream) free(stream);
    }

    void create(size_t len, size_t slen) {
        ASSERT_GE(len, slen * 2); //enough space to hold src and dst
        buf_len = len + WILDCOPY_OVERLENGTH;
        src_len = slen;

        stream = (char*)malloc(sizeof(char) * buf_len);
        src = stream;
        reset();
    }

    void reset() {
        ASSERT_NE(src, nullptr);
        memset(stream, 0, sizeof(char) * buf_len);

        for (size_t i = 0; i < src_len; ++i) { //fill non-0 values for src
            src[i] = (i % 256);
            if (src[i] == 0) src[i] = 1;
        }
    }

    void setDst(size_t pos) {
        ASSERT_LE(pos + src_len, buf_len - WILDCOPY_OVERLENGTH); //enough space in dst to hold src_len
        ASSERT_NE(src, nullptr);
        dst = src + pos;
    }

    void validate() {
        EXPECT_EQ(memcmp(src, dst, src_len), 0); //validate src and dst are equal


        if (dst > (src + src_len))
        { //validate bytes HERE are not polluted: [src...src+length..<HERE>..dst..dst+len]
            bool polluted = false;
            char* cur = src + src_len;
            char* end = dst;
            while (cur < end) {
                if ((*cur) != 0) {
                    polluted = true;
                    break;
                }
                cur++;
            }
            EXPECT_NE(polluted, true);
        }
        //[src...src+length...dst..dst+len..<HERE>..buf_len]. Ok to pollute here
    }

    char* getSrc() {
        return src;
    }

    char* getDst() {
        return dst;
    }

private:
    char* stream, * src, * dst;
    size_t buf_len, src_len;
};

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_1) // (dst-src) >= WILDCOPY_VECLEN, length < WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN - 1;
    size_t test_cnt = 8;
    size_t buf_len = length + WILDCOPY_VECLEN + test_cnt; //(dst bytes) + (gap btw dst,src) + (test for > WILDCOPY_VECLEN gap)

    int ovtype = 0;
    create(buf_len, length);

    for (int i = WILDCOPY_VECLEN; i < (WILDCOPY_VECLEN + test_cnt); ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_2) // (dst-src) >= WILDCOPY_VECLEN, length > WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN + 1;
    size_t test_cnt = 8;
    size_t buf_len = length + WILDCOPY_VECLEN + test_cnt; //(dst bytes) + (gap btw dst,src) + (test for > WILDCOPY_VECLEN gap)

    int ovtype = 0;
    create(buf_len, length);

    for (int i = WILDCOPY_VECLEN; i < (WILDCOPY_VECLEN + test_cnt); ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_3) // 8 <= (dst-src) < WILDCOPY_VECLEN, length < WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN - 1;
    size_t buf_len = length + WILDCOPY_VECLEN; //(dst bytes) + (gap btw dst,src)

    int ovtype = 1;
    create(buf_len, length);

    for (int i = 8; i < WILDCOPY_VECLEN; ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

TEST_F(ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long, AOCL_Compression_zstd_AOCL_ZSTD_wildcopy_long_common_4) // 8 <= (dst-src) < WILDCOPY_VECLEN, length > WILDCOPY_VECLEN
{
    size_t length = WILDCOPY_VECLEN + 1;
    size_t buf_len = length + length; //(dst bytes) + (src bytes)

    int ovtype = 1;
    create(buf_len, length);

    for (int i = 8; i < WILDCOPY_VECLEN; ++i) {
        setDst(i);
        TEST_AOCL_ZSTD_wildcopy_long((void*)getDst(), (void*)getSrc(), length, ovtype);
        validate();
        reset();
    }
}

/*********************************************
* End of ZSTD_ZSTD_AOCL_ZSTD_wildcopy_long
*********************************************/
#endif /* AOCL_ZSTD_OPT */

#ifdef AOCL_ENABLE_THREADS
/*********************************************
* Begin of ZSTD_ZSTD_GET_WINDOW_FACTOR
*********************************************/
TEST(ZSTD_ZSTD_GET_WINDOW_FACTOR, AOCL_Compression_zstd_ZSTD_GET_WINDOW_FACTOR_common_1)
{
    size_t srcSize = 0;
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (100 * 1024 * 1024) - 1; //< 100 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (100 * 1024 * 1024); //100 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (100 * 1024 * 1024) + 1; //>100 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 1);

    srcSize = (200 * 1024 * 1024); //200 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 2);

    srcSize = (300 * 1024 * 1024); //300 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 3);

    srcSize = (400 * 1024 * 1024); //400 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 4);

    srcSize = (400 * 1024 * 1024) + 1; //>400 MB
    EXPECT_EQ(Test_ZSTD_getWindowFactor(srcSize), 4);
}
/*********************************************
* End of ZSTD_ZSTD_GET_WINDOW_FACTOR
*********************************************/

/*********************************************
* Begin of ZSTD_AOCL_ZSTD_readSkippableFrameHeader
*********************************************/
class ZSTD_AOCL_ZSTD_readSkippableFrameHeader : public AOCL_setup_zstd {
public:
    ZSTD_AOCL_ZSTD_readSkippableFrameHeader() {
        src = malloc(32);
        srcSize = 32;
        memset(src, 0, 32);
        dst = NULL;
        dstCapacity = 0;
    }

    ~ZSTD_AOCL_ZSTD_readSkippableFrameHeader() {
        if (src)
            free(src);
        if (dst)
            free(dst);
    }

    bool write_RAP_frame_header(AOCL_CHAR* dst, size_t dstCapacity, int mainThreads) { // dst must have allocated sufficient size
        size_t rap_frame_len = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0);
        if (dstCapacity < rap_frame_len)
            return false;

        *(AOCL_INT64*)dst = RAP_MAGIC_WORD; //For storing the magic word
        dst += RAP_MAGIC_WORD_BYTES;
        *(AOCL_UINT32*)dst = rap_frame_len; //For storing the total RAP frame length
        dst += RAP_METADATA_LEN_BYTES;
        *(AOCL_UINT32*)dst = mainThreads; //For storing the no. of threads
        return true;
    }

    bool write_skippable_RAP_frame(AOCL_CHAR* dst, size_t dstCapacity, size_t srcSize, int mainThreads) { // dst must have allocated sufficient size
        size_t skip_frame_len = Test_AOCL_ZSTD_writeSkippableFrameHeader(dst, dstCapacity, srcSize, rand() % 15);
        if (Test_ZSTD_isError(skip_frame_len)) return false;
        return write_RAP_frame_header(dst + skip_frame_len, dstCapacity - skip_frame_len, mainThreads);
    }

    void* src, * dst;
    size_t srcSize, dstCapacity;
};

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_pass_common_1) // valid RAP frame present
{
    const int mainThreads = 5;
    size_t dstCapacity = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + ZSTD_SKIPPABLEHEADERSIZE;
    dst = malloc(dstCapacity);
    EXPECT_TRUE(write_skippable_RAP_frame((AOCL_CHAR*)dst, dstCapacity, srcSize, mainThreads));

    CHECK_PASS_ZSTD(Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_2) // no skippable frame
{
    size_t dstCapacity = 128;
    dst = malloc(dstCapacity);

    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(frameParameter_unsupported));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_3) // skippable frame with no RAP frame in it
{
    const int mainThreads = 5;
    size_t dstCapacity = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + ZSTD_SKIPPABLEHEADERSIZE;
    dst = malloc(dstCapacity);
    CHECK_PASS_ZSTD(Test_AOCL_ZSTD_writeSkippableFrameHeader(dst, dstCapacity, srcSize, rand() % 15)); // write skippable header

    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_4) // srcSize too small
{
    const int mainThreads = 5;
    size_t dstCapacity = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + ZSTD_SKIPPABLEHEADERSIZE;
    dst = malloc(dstCapacity);
    EXPECT_TRUE(write_skippable_RAP_frame((AOCL_CHAR*)dst, dstCapacity, srcSize, mainThreads));

    dstCapacity = ZSTD_SKIPPABLEHEADERSIZE + RAP_MAGIC_WORD_BYTES - 1;
    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_5) // skippableFrameSize > srcSize
{
    const int mainThreads = 5;
    size_t dstCapacity = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + ZSTD_SKIPPABLEHEADERSIZE;
    dst = malloc(dstCapacity);
    MEM_writeLE32((char*)dst, (U32)(ZSTD_MAGIC_SKIPPABLE_START + 0)); // write skippable header
    MEM_writeLE32((char*)dst + 4, (U32)(dstCapacity + 1)); // write invalid skip frame size

    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

/*********************************************
* End of ZSTD_AOCL_ZSTD_readSkippableFrameHeader
*********************************************/
#endif /* AOCL_ENABLE_THREADS */
