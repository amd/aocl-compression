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

#if AOCL_DECOMPRESS_FAST > 1
/*********************************************
* Begin of ZSTD_get_lit_bits, ZSTD_get_mat_bits, ZSTD_get_off_bits, ZSTD_get_mat_len
*********************************************/
#define MAX_LIT_LENGTH 131071
#define MAX_MAT_LENGTH 131071
#define MAX_OFF_LENGTH 536870908
#define MAX_SMALL_LENGTH 128
#define MAX_SEQUENCES_POSSIBLE 4096 /* tests should ensure this is large enough */
#define MAX_MAT_BITS 16

TEST(ZSTD_get_lit_bits, AOCL_Compression_zstd_get_lit_bits_common_1) { // Optimized get_*_bits must be >= actual bits
    for (size_t len = 0; len < MAX_LIT_LENGTH; ++len) {
        EXPECT_GE(Test_get_lit_bits(len), Test_assert_get_lit_bits(len));
    }
}

TEST(ZSTD_get_mat_bits, AOCL_Compression_zstd_get_mat_bits_common_1) { // Optimized get_*_bits must be >= actual bits
    for (size_t len = 0; len < MAX_LIT_LENGTH; ++len) {
        EXPECT_GE(Test_get_mat_bits(len), Test_assert_get_mat_bits(len));
    }
}

TEST(ZSTD_get_off_bits, AOCL_Compression_zstd_get_off_bits_common_1) { // Optimized get_*_bits must be >= actual bits
    for (size_t len = 1; len < MAX_SMALL_LENGTH; ++len) {
        EXPECT_GE(Test_get_off_bits(len), Test_assert_get_off_bits(len));
    }
    for (size_t len = MAX_SMALL_LENGTH; len < MAX_OFF_LENGTH; len <<= 1) { // step faster
        EXPECT_GE(Test_get_mat_bits(len - 1), Test_assert_get_mat_bits(len - 1));
        EXPECT_GE(Test_get_mat_bits(len), Test_assert_get_mat_bits(len));
        EXPECT_GE(Test_get_mat_bits(len + 1), Test_assert_get_mat_bits(len + 1));
    }
}

TEST(ZSTD_get_mat_len, AOCL_Compression_zstd_get_mat_len_common_1) { // Optimized get_*_len must be <= actual len
    for (int bits = 0; bits < MAX_MAT_BITS; bits++) {
        EXPECT_LE(Test_get_mat_len(bits, MAX_MAT_LENGTH), assert_get_mat_len(bits, MAX_MAT_LENGTH));
    }
}
/*********************************************
* End of ZSTD_get_lit_bits, ZSTD_get_mat_bits, ZSTD_get_off_bits, ZSTD_get_mat_len
*********************************************/

/*********************************************
* Begin of ZSTD_is_totalbits_limited_seq_possible
*********************************************/
class ZSTD_is_totalbits_limited_seq_possible : public AOCL_setup_zstd
{
public:
    ZSTD_is_totalbits_limited_seq_possible() {
        anchor = (BYTE*)calloc(MAX_LIT_LENGTH, 1);
        iend = anchor + MAX_LIT_LENGTH;

        seqStore.litStart = (BYTE*)calloc(MAX_LIT_LENGTH, 1);
        seqStore.lit = seqStore.litStart;
        seqStore.maxNbLit = MAX_LIT_LENGTH;

        seqStore.sequencesStart = (seqDef*)calloc(MAX_SEQUENCES_POSSIBLE, sizeof(seqDef));
        seqStore.sequences = seqStore.sequencesStart;
        seqStore.maxNbSeq = MAX_SEQUENCES_POSSIBLE;
    }

    ~ZSTD_is_totalbits_limited_seq_possible() {
        if (anchor) free(anchor);
        if (seqStore.litStart) free(seqStore.litStart);
        if (seqStore.sequencesStart) free(seqStore.sequencesStart);
    }

    void reset_sequences() {
        seqStore.sequences = seqStore.sequencesStart;
        seqStore.lit = seqStore.litStart;
    }

    void evalute_sequences(size_t lit, size_t mat, size_t off) {
        const BYTE* ip = anchor + lit;
        int res = Test_is_totalbits_limited_seq_possible(ip, anchor, mat, off);
        if (res) { // if claim is possible, validate generated sequence to see if it is within bounds
            reset_sequences();
            Test_AOCL_ZSTD_storeSequences(&seqStore, ip, anchor, iend, (U32)OFFSET_TO_OFFBASE(off), mat);
            size_t seqCnt = seqStore.sequences - seqStore.sequencesStart;
            seqDef* curSeq = seqStore.sequencesStart;
            for (size_t i = 0; i < seqCnt; ++i) { // for each sequence validate if total_bits is within bounds
                U32 offBase = curSeq[0].offBase;
                if (OFFBASE_IS_OFFSET(offBase)) {
                    U32 offset = OFFBASE_TO_OFFSET(offBase);
                    U16 litLength = curSeq[0].litLength;
                    U16 matchLength = curSeq[0].mlBase + MINMATCH;
                    U32 total_bits = Test_assert_get_lit_bits(litLength) + Test_assert_get_mat_bits(matchLength) + Test_assert_get_off_bits(offset);
                    EXPECT_LT(total_bits, MAX_TOTAL_BITS);
                }
            }
        }
    }

    BYTE* anchor;
    const BYTE* iend;
    seqStore_t seqStore;
};

TEST_F(ZSTD_is_totalbits_limited_seq_possible, AOCL_Compression_zstd_is_totalbits_limited_seq_possible_common_1) { // specific cases
    evalute_sequences(0, MINMATCH, 1); // minimum values. totalbits < MAX_TOTAL_BITS.
    evalute_sequences((size_t)1 << 10, MINMATCH, (size_t)1 << 20); // (llbits + ofbits) >= MAX_TOTAL_BITS.
    evalute_sequences(1, MINMATCH, (size_t)1 << 25); // totalbits > MAX_TOTAL_BITS. unable to split match.
    evalute_sequences(1, 2 * MINMATCH, (size_t)1 << 25);  // totalbits > MAX_TOTAL_BITS. able to split match.
    evalute_sequences(1, (size_t)1 << 8, (size_t)1 << 20);  // totalbits > MAX_TOTAL_BITS. split multiple.
}

TEST_F(ZSTD_is_totalbits_limited_seq_possible, AOCL_Compression_zstd_is_totalbits_limited_seq_possible_common_2) { // small sizes
    // Test for all combinations of small lit, match and offset values
    for (size_t lit = 0; lit < MAX_SMALL_LENGTH; lit++) {
        for (size_t mat = 0; mat < MAX_SMALL_LENGTH; mat++) {
            for (size_t off = 1; off < MAX_SMALL_LENGTH; off++) {
                evalute_sequences(lit, mat, off);
            }
        }
    }
}

TEST_F(ZSTD_is_totalbits_limited_seq_possible, AOCL_Compression_zstd_is_totalbits_limited_seq_possible_common_3) { // large sizes
    // Larger strides for large lit, match and offset values
    for (size_t lit = MAX_SMALL_LENGTH; lit < MAX_LIT_LENGTH; lit <<= 1) {
        for (size_t mat = MAX_SMALL_LENGTH; mat < MAX_MAT_LENGTH; mat <<= 1) {
            for (size_t off = MAX_SMALL_LENGTH; off < MAX_OFF_LENGTH; off <<= 1) {
                evalute_sequences(lit, mat, off);
                evalute_sequences(lit - 1, mat - 1, off - 1);
                evalute_sequences(lit + 1, mat + 1, off + 1);
            }
        }
    }
    // Small lit, larger strides for rest
    for (size_t lit = 0; lit < MAX_SMALL_LENGTH; lit++) {
        for (size_t mat = 1; mat < MAX_MAT_LENGTH; mat <<= 1) {
            for (size_t off = 1; off < MAX_OFF_LENGTH; off <<= 1) {
                evalute_sequences(lit, mat, off);
            }
        }
    }
    // Small mat, larger strides for rest
    for (size_t lit = 1; lit < MAX_LIT_LENGTH; lit <<= 1) {
        for (size_t mat = 0; mat < MAX_SMALL_LENGTH; mat++) {
            for (size_t off = 1; off < MAX_OFF_LENGTH; off <<= 1) {
                evalute_sequences(lit, mat, off);
            }
        }
    }
    // Small off, larger strides for rest
    for (size_t lit = 1; lit < MAX_LIT_LENGTH; lit <<= 1) {
        for (size_t mat = 1; mat < MAX_MAT_LENGTH; mat <<= 1) {
            for (size_t off = 1; off < MAX_SMALL_LENGTH; off++) {
                evalute_sequences(lit, mat, off);
            }
        }
    }
}
/*********************************************
* End of ZSTD_is_totalbits_limited_seq_possible
*********************************************/

/*********************************************
* Begin of ZSTD_AOCL_is_FdsSupported
*********************************************/
class ZSTD_AOCL_is_FdsSupported : public AOCL_setup_zstd
{
public:
    ZSTD_AOCL_is_FdsSupported() {
        zc = ZSTD_createCCtx();
    }

    ~ZSTD_AOCL_is_FdsSupported() {
        ZSTD_freeCCtx(zc);
    }

    ZSTD_CCtx* zc;
};

TEST_F(ZSTD_AOCL_is_FdsSupported, AOCL_Compression_zstd_AOCL_is_FdsSupported_common_pass_1) { // all true cases
    // ZSTD_noDict
    zc->blockState.matchState.dictMatchState = NULL;
    int hasExtDict = 0;
    // test for all supported strategies
    for (int strat = (int)ZSTD_fast; strat <= (int)ZSTD_lazy2; strat++) {
        zc->appliedParams.cParams.strategy = (ZSTD_strategy)strat;
        zc->appliedParams.useRowMatchFinder = ZSTD_ps_enable;
        EXPECT_TRUE(Test_AOCL_is_FdsSupported(hasExtDict, zc));
    }
}

TEST_F(ZSTD_AOCL_is_FdsSupported, AOCL_Compression_zstd_AOCL_is_FdsSupported_common_pass_2) { // false cases dict modes
    // ZSTD_extDict
    zc->blockState.matchState.dictMatchState = NULL;
    int hasExtDict = 1;
    for (int strat = (int)ZSTD_fast; strat <= (int)ZSTD_btultra2; strat++) { // all strats
        zc->appliedParams.cParams.strategy = (ZSTD_strategy)strat;
        EXPECT_FALSE(Test_AOCL_is_FdsSupported(hasExtDict, zc));
    }

    // ZSTD_dictMatchState
    ZSTD_matchState_t ms;
    ms.dedicatedDictSearch = ZSTD_dictMatchState;
    zc->blockState.matchState.dictMatchState = &ms;
    hasExtDict = 0;
    for (int strat = (int)ZSTD_fast; strat <= (int)ZSTD_btultra2; strat++) { // all strats
        zc->appliedParams.cParams.strategy = (ZSTD_strategy)strat;
        EXPECT_FALSE(Test_AOCL_is_FdsSupported(hasExtDict, zc));
    }

    // ZSTD_dedicatedDictSearch
    ms.dedicatedDictSearch = ZSTD_dedicatedDictSearch;
    zc->blockState.matchState.dictMatchState = &ms;
    hasExtDict = 0;
    for (int strat = ZSTD_fast; strat <= ZSTD_btultra2; strat++) { // all strats
        zc->appliedParams.cParams.strategy = (ZSTD_strategy)strat;
        EXPECT_FALSE(Test_AOCL_is_FdsSupported(hasExtDict, zc));
    }
}

TEST_F(ZSTD_AOCL_is_FdsSupported, AOCL_Compression_zstd_AOCL_is_FdsSupported_common_pass_3) { // useRowMatchFinder
    // ZSTD_noDict
    zc->blockState.matchState.dictMatchState = NULL;
    int hasExtDict = 0;
    // test fast and dfast. useRowMatchFinder does not matter
    for (int strat = (int)ZSTD_fast; strat <= (int)ZSTD_dfast; strat++) {
        zc->appliedParams.cParams.strategy = (ZSTD_strategy)strat;
        for (int rmf = (int)ZSTD_ps_auto; rmf <= (int)ZSTD_ps_disable; rmf++) {
            zc->appliedParams.useRowMatchFinder = (ZSTD_paramSwitch_e)rmf;
            EXPECT_TRUE(Test_AOCL_is_FdsSupported(hasExtDict, zc));
        }
    }
    // test greedy, lazy and lazy2. useRowMatchFinder needed.
    for (int strat = (int)ZSTD_greedy; strat <= (int)ZSTD_lazy2; strat++) {
        zc->appliedParams.cParams.strategy = (ZSTD_strategy)strat;
        {
            zc->appliedParams.useRowMatchFinder = ZSTD_ps_auto;
            EXPECT_FALSE(Test_AOCL_is_FdsSupported(hasExtDict, zc));
        }
        {
            zc->appliedParams.useRowMatchFinder = ZSTD_ps_disable;
            EXPECT_FALSE(Test_AOCL_is_FdsSupported(hasExtDict, zc));
        }
    }
}
/*********************************************
* End of ZSTD_AOCL_is_FdsSupported
*********************************************/

/*********************************************
* Begin of ZSTD_AOCL_ZSTD_window_needsExtDict
*********************************************/
class ZSTD_AOCL_ZSTD_window_needsExtDict : public AOCL_setup_zstd
{
public:
    ZSTD_AOCL_ZSTD_window_needsExtDict() {
        buffer = (BYTE*)calloc(bufferSz, 1);
    }

    ~ZSTD_AOCL_ZSTD_window_needsExtDict() {
        if(buffer) free(buffer);
    }

    void validate(int needsExtDict, int forceNonContiguous) {
        Test_ZSTD_window_update(&win, src, srcSize, forceNonContiguous);
        EXPECT_EQ(needsExtDict, (win.lowLimit < win.dictLimit)); // needsExtDict obtained from AOCL_ZSTD_window_needsExtDict() must match ZSTD_window_update() settings
    }

    BYTE* get_ptr(size_t pos) {
        EXPECT_LT(pos, bufferSz);
        if (pos >= bufferSz) return nullptr;
        return buffer + pos;
    }

    void set_inp_and_dict_no_overlap_contig() {
        win.base = get_ptr(0);
        win.dictBase = get_ptr(0);
        win.dictLimit = 512;
        win.nextSrc = get_ptr(513); //input and dictionary dont overlap
        win.nbOverflowCorrections = 0;
        src = win.nextSrc;
        srcSize = 10;
    }

    void set_inp_and_dict_no_overlap_noncontig() {
        win.base = get_ptr(0);
        win.dictBase = get_ptr(0);
        win.dictLimit = 512;
        win.nextSrc = get_ptr(513); //input and dictionary dont overlap
        win.nbOverflowCorrections = 0;
        src = win.nextSrc + 1;
        srcSize = 10;
    }

    void set_inp_and_dict_overlap(int ll_lt_dl) {
        //input and dictionary overlap
        win.base = get_ptr(0);
        win.dictBase = get_ptr(10);
        win.nextSrc = get_ptr(50);
        src = win.nextSrc;
        if (ll_lt_dl) 
        { //dB----ip-----dB+lL----dB+dL----ip+srcSize
            win.dictLimit = 120;
            win.lowLimit = 100;
        }
        else 
        { //dB----ip----dB+dL----dB+lL----ip+srcSize
            win.dictLimit = 100;
            win.lowLimit = 120;
        }
        srcSize = 100; //ip+srcSize = 150
        win.nbOverflowCorrections = 0;
    }

    const BYTE* src;
    ZSTD_window_t win;
    size_t srcSize;

private:
    BYTE* buffer;
    const size_t bufferSz = 1024;
};

TEST_F(ZSTD_AOCL_ZSTD_window_needsExtDict, AOCL_Compression_zstd_AOCL_ZSTD_window_needsExtDict_common_pass_1) { //src = window->nextSrc, input and dictionary dont overlap
    {  // lowLimit < dictLimit
        set_inp_and_dict_no_overlap_contig();
        int forceNonContiguous = 0;
        win.lowLimit = win.dictLimit - 1;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
    {  // lowLimit >= dictLimit
        set_inp_and_dict_no_overlap_contig();
        int forceNonContiguous = 0;
        win.lowLimit = win.dictLimit;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
    {  // forceNonContiguous
        set_inp_and_dict_no_overlap_contig();
        int forceNonContiguous = 1;
        win.lowLimit = win.dictLimit;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
}

TEST_F(ZSTD_AOCL_ZSTD_window_needsExtDict, AOCL_Compression_zstd_AOCL_ZSTD_window_needsExtDict_common_pass_2) { //src != window->nextSrc, input and dictionary dont overlap
    {  // lowLimit < dictLimit
        set_inp_and_dict_no_overlap_noncontig();
        int forceNonContiguous = 0;
        win.lowLimit = win.dictLimit - 1;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
    {  // lowLimit >= dictLimit
        set_inp_and_dict_no_overlap_noncontig();
        int forceNonContiguous = 0;
        win.lowLimit = win.dictLimit;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
    {  // forceNonContiguous
        set_inp_and_dict_no_overlap_noncontig();
        int forceNonContiguous = 1;
        win.lowLimit = win.dictLimit;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
}

TEST_F(ZSTD_AOCL_ZSTD_window_needsExtDict, AOCL_Compression_zstd_AOCL_ZSTD_window_needsExtDict_common_pass_3) { //input and dictionary overlap
    {  // lowLimit < dictLimit
        set_inp_and_dict_overlap(1);
        int forceNonContiguous = 0;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
    {  // lowLimit >= dictLimit
        set_inp_and_dict_overlap(0);
        int forceNonContiguous = 0;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
    {  // forceNonContiguous
        set_inp_and_dict_overlap(1);
        int forceNonContiguous = 1;
        int needsExtDict = Test_AOCL_ZSTD_window_needsExtDict(&win, src, srcSize, forceNonContiguous);
        validate(needsExtDict, forceNonContiguous);
    }
}
/*********************************************
* End of ZSTD_AOCL_ZSTD_window_needsExtDict
*********************************************/

/*********************************************
* Begin of ZSTD_AOCL_ZSTD_writeFdsFrame
*********************************************/
class ZSTD_AOCL_ZSTD_writeFdsFrame : public AOCL_setup_zstd
{
public:
    void alloc_dst(size_t sz) {
        dst = calloc(sz, 1);
    }

    ~ZSTD_AOCL_ZSTD_writeFdsFrame() {
        if (dst) free(dst);
    }

    void validate_fds_frame() {
        char* cur = (char*)dst;
        cur += ZSTD_SKIPPABLEHEADERSIZE;
        EXPECT_EQ(*((U64*)cur), FDS_MAGIC_WORD);
#if AOCL_DECOMPRESS_FAST == 2
        EXPECT_EQ(*((U64*)(cur + FDS_MAGIC_WORD_BYTES)), FDS_FAST2_NOTB_SO4_NOEXT_REP2);
#endif
    }
    void* dst;
};

TEST_F(ZSTD_AOCL_ZSTD_writeFdsFrame, AOCL_Compression_zstd_AOCL_ZSTD_writeFdsFrame_common_pass_1) { // dstCapacity sufficient
    size_t dstCapacity = FDS_FRAME_LENGTH + ZSTD_SKIPPABLEHEADERSIZE;
    alloc_dst(dstCapacity);
    EXPECT_EQ(dstCapacity, Test_AOCL_ZSTD_writeFdsFrame(dst, dstCapacity));
    validate_fds_frame();
}

TEST_F(ZSTD_AOCL_ZSTD_writeFdsFrame, AOCL_Compression_zstd_AOCL_ZSTD_writeFdsFrame_common_fail_2) { // dstCapacity insufficient
    size_t dstCapacity = FDS_FRAME_LENGTH + ZSTD_SKIPPABLEHEADERSIZE - 1;
    alloc_dst(dstCapacity);
    EXPECT_EQ(Test_AOCL_ZSTD_writeFdsFrame(dst, dstCapacity), ERROR(dstSize_tooSmall));
}
/*********************************************
* End of ZSTD_AOCL_ZSTD_writeFdsFrame
*********************************************/

/*********************************************
* Begin of ZSTD_AOCL_ZSTD_readFdsFrame
*********************************************/
class ZSTD_AOCL_ZSTD_readFdsFrame : public AOCL_setup_zstd
{
public:
    ZSTD_AOCL_ZSTD_readFdsFrame() {
        dctx = ZSTD_createDCtx();
        dctx->fds = 0;
    }

    void alloc_src(size_t sz) {
        src = calloc(sz, 1);
    }

    ~ZSTD_AOCL_ZSTD_readFdsFrame() {
        if (src) free(src);
        ZSTD_freeDCtx(dctx);
    }

    void write_valid_fds_frame(size_t sz) {
        EXPECT_FALSE(ZSTD_isError(Test_AOCL_ZSTD_writeFdsFrame(src, sz)));
    }

    ZSTD_DCtx* dctx;
    void* src;
};

TEST_F(ZSTD_AOCL_ZSTD_readFdsFrame, AOCL_Compression_zstd_AOCL_ZSTD_readFdsFrame_common_pass_1) { // read valid FDS frame
    size_t srcSz = FDS_FRAME_LENGTH + ZSTD_SKIPPABLEHEADERSIZE;
    alloc_src(srcSz);
    write_valid_fds_frame(srcSz);
    Test_AOCL_ZSTD_readFdsFrame(dctx, (char*)src + ZSTD_SKIPPABLEHEADERSIZE, srcSz - ZSTD_SKIPPABLEHEADERSIZE);
    EXPECT_EQ(dctx->fds, FDS_FAST2_NOTB_SO4_NOEXT_REP2);
}

TEST_F(ZSTD_AOCL_ZSTD_readFdsFrame, AOCL_Compression_zstd_AOCL_ZSTD_writeFdsFrame_common_fail_2) { // srcSize insufficient
    size_t srcSz = FDS_FRAME_LENGTH + ZSTD_SKIPPABLEHEADERSIZE;
    alloc_src(srcSz);
    write_valid_fds_frame(srcSz);
    Test_AOCL_ZSTD_readFdsFrame(dctx, (char*)src + ZSTD_SKIPPABLEHEADERSIZE, srcSz - ZSTD_SKIPPABLEHEADERSIZE - 1); // srcSize insufficient
    EXPECT_EQ(dctx->fds, 0);
}

TEST_F(ZSTD_AOCL_ZSTD_readFdsFrame, AOCL_Compression_zstd_AOCL_ZSTD_writeFdsFrame_common_fail_3) { // not an FDS frame
    size_t srcSz = FDS_FRAME_LENGTH + ZSTD_SKIPPABLEHEADERSIZE;
    alloc_src(srcSz);
    
    //write non FDS frame
    char fds[FDS_FRAME_LENGTH];
    *((U64*)fds) = FDS_MAGIC_WORD - 1; // wrong magic word
    *((U64*)(fds + FDS_MAGIC_WORD_BYTES)) = FDS_FAST2_NOTB_SO4_NOEXT_REP2;
    EXPECT_FALSE(ZSTD_isError(ZSTD_writeSkippableFrame(src, srcSz, fds, FDS_FRAME_LENGTH, 0)));

    Test_AOCL_ZSTD_readFdsFrame(dctx, (char*)src + ZSTD_SKIPPABLEHEADERSIZE, srcSz - ZSTD_SKIPPABLEHEADERSIZE);
    EXPECT_EQ(dctx->fds, 0);
}

TEST_F(ZSTD_AOCL_ZSTD_readFdsFrame, AOCL_Compression_zstd_AOCL_ZSTD_writeFdsFrame_common_fail_4) { // type not FDS_FAST2_NOTB_SO4_NOEXT_REP2
    size_t srcSz = FDS_FRAME_LENGTH + ZSTD_SKIPPABLEHEADERSIZE;
    alloc_src(srcSz);
    
    //write non FDS frame
    char fds[FDS_FRAME_LENGTH];
    *((U64*)fds) = FDS_MAGIC_WORD;
    *((U64*)(fds + FDS_MAGIC_WORD_BYTES)) = FDS_FAST2_NOTB_SO4_NOEXT_REP2 - 1; // wrong type
    EXPECT_FALSE(ZSTD_isError(ZSTD_writeSkippableFrame(src, srcSz, fds, FDS_FRAME_LENGTH, 0)));

    Test_AOCL_ZSTD_readFdsFrame(dctx, (char*)src + ZSTD_SKIPPABLEHEADERSIZE, srcSz - ZSTD_SKIPPABLEHEADERSIZE);
    EXPECT_EQ(dctx->fds, 0);
}
/*********************************************
* End of ZSTD_AOCL_ZSTD_readFdsFrame
*********************************************/
#endif /* AOCL_DECOMPRESS_FAST > 1 */
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
    dst = calloc(dstCapacity, 1);
    EXPECT_TRUE(write_skippable_RAP_frame((AOCL_CHAR*)dst, dstCapacity, srcSize, mainThreads));

    CHECK_PASS_ZSTD(Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_2) // no skippable frame
{
    size_t dstCapacity = 128;
    dst = calloc(dstCapacity, 1);

    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(frameParameter_unsupported));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_3) // skippable frame with no RAP frame in it
{
    const int mainThreads = 5;
    size_t dstCapacity = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + ZSTD_SKIPPABLEHEADERSIZE;
    dst = calloc(dstCapacity, 1);
    CHECK_PASS_ZSTD(Test_AOCL_ZSTD_writeSkippableFrameHeader(dst, dstCapacity, srcSize, rand() % 15)); // write skippable header

    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(GENERIC));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_4) // srcSize too small
{
    const int mainThreads = 5;
    size_t dstCapacity = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + ZSTD_SKIPPABLEHEADERSIZE;
    dst = calloc(dstCapacity, 1);
    EXPECT_TRUE(write_skippable_RAP_frame((AOCL_CHAR*)dst, dstCapacity, srcSize, mainThreads));

    dstCapacity = ZSTD_SKIPPABLEHEADERSIZE + RAP_MAGIC_WORD_BYTES - 1;
    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

TEST_F(ZSTD_AOCL_ZSTD_readSkippableFrameHeader, AOCL_Compression_zstd_AOCL_ZSTD_readSkippableFrameHeader_fail_common_5) // skippableFrameSize > srcSize
{
    const int mainThreads = 5;
    size_t dstCapacity = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + ZSTD_SKIPPABLEHEADERSIZE;
    dst = calloc(dstCapacity, 1);
    MEM_writeLE32((char*)dst, (U32)(ZSTD_MAGIC_SKIPPABLE_START + 0)); // write skippable header
    MEM_writeLE32((char*)dst + 4, (U32)(dstCapacity + 1)); // write invalid skip frame size

    size_t ret = Test_AOCL_ZSTD_readSkippableRAPFrameHeader(dst, dstCapacity);
    EXPECT_EQ(ret, ERROR(srcSize_wrong));
}

/*********************************************
* End of ZSTD_AOCL_ZSTD_readSkippableFrameHeader
*********************************************/
#endif /* AOCL_ENABLE_THREADS */
