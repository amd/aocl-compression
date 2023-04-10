/**
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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
 
 /** @file lzma_gtest.cc
 *  
 *  @brief Test cases for LZMA algo.
 *
 *  This file contains the test cases for LZMA method
 *  testing the API level functions of LZMA.
 *
 *  @author Ashish Sriram
 */

#include <string>
#include <climits>
#include "gtest/gtest.h"

#define AOCL_DYNAMIC_DISPATCHER
#include "algos/lzma/Alloc.h"
#include "algos/lzma/LzFind.h"
#include "algos/lzma/LzHash.h"

using namespace std;
typedef unsigned char Byte;
typedef unsigned UInt32;

/*********************************************
 * "Begin" of LZMA_byteMatching
 *********************************************/

/*
    This is a fixture class for testing `AOCL_FIND_MATCHING_BYTES_LEN` macro
*/
class LZMA_byteMatching : public ::testing::Test 
{
public:
    //Call optimized function
    int opt_find_match(unsigned startlen, Byte* pb, Byte* cur, unsigned lenLimit) {
        return Test_AOCL_Find_Matching_Bytes_Len(startlen, pb, cur, lenLimit);
    }

    //Compare bytewise
    int org_find_match(unsigned startlen, Byte* pb, Byte* cur, unsigned lenLimit) {
        unsigned len = startlen;
        while (len < lenLimit) {
            if (pb[len] != cur[len])
                break;
            len++;
        }
        return len;
    }
};

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_match_common)
{
    unsigned lenLimit = 9;
    for (unsigned len = 0; len < 8; ++len) {
        Byte pb[] = "abcd1234";
        Byte cur[] = "abcd1234";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit), 
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_match_common_1 - _8
    }    
}

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_mismatch_common)
{
    unsigned lenLimit = 9;
    unsigned len = 0;
    Byte cur[] = "abcd1234";
    {
        Byte pb[] = "abcd0234";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_mismatch_common_1
    }
    {
        Byte pb[] = "abcd1034";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_mismatch_common_2
    }
    {
        Byte pb[] = "abcd1204";;
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_mismatch_common_3
    }
    {
        Byte pb[] = "abcd1230";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_mismatch_common_4
    }
}

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_shortStr_common)
{
    unsigned lenLimit = 9;
    unsigned len = 0;
    Byte cur[] = "abcd1234";
    {
        Byte pb[] = "abc";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_shortStr_common_1
    }
    {
        Byte pb[] = "ab";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_shortStr_common_2
    }
    {
        Byte pb[] = "a";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_shortStr_common_3
    }
}

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_lenLimit_common)
{
    unsigned lenLimit = 5;
    unsigned len = 0;
    Byte cur[] = "abcd1234";
    {
        Byte pb[] = "abcd1xyz";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_lenLimit_common_1
    }
    {
        Byte pb[] = "abcd0xyz";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_lenLimit_common_2
    }
}

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_longStr_common)
{
    unsigned lenLimit = 9;
    unsigned len = 0;
    Byte cur[] = "qwertyuiopasdfghjklzxcvbnm";
    {
        Byte pb[] = "qwertyuiopasdfghjklzxcvbnm";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_longStr_common_1
    }
    {
        Byte pb[] = "qwertyuiopasfghjkl0xcvbnm";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_longStr_common_2
    }
}

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_remBytes_common)
{
    unsigned lenLimit = 4;
    unsigned len = 1;
    Byte cur[] = "qwer";
    {
        Byte pb[] = "xwer";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_remBytes_common_1
    }
    {
        Byte pb[] = "qxer";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_remBytes_common_2
    }
    {
        Byte pb[] = "qwxr";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_remBytes_common_3
    }
    {
        Byte pb[] = "qwex";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_remBytes_common_4
    }
}

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_lenLimitShort_common)
{
    unsigned len = 0;
    for (unsigned lenLimit = 0; lenLimit < 3; ++lenLimit) {
        Byte pb[] = "abcd1234";
        Byte cur[] = "abcd1234";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_lenLimitShort_common_1 - _3
    }
}

TEST_F(LZMA_byteMatching, AOCL_Compression_lzma_AOCL_FIND_MATCHING_BYTES_LEN_lenGreaterThanLimit_common)
{
    unsigned lenLimit = 5;
    unsigned len = 7;
    Byte pb[] = "abcd1234";
    Byte cur[] = "abcd1234";
    EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
        opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_lenGreaterThanLimit_common_1
}

/*********************************************
 * End of LZMA_byteMatching
 *********************************************/

/*********************************************
* "Begin" of LZMA_hashMaskLimit
*********************************************/

/*
    This is a fixture class for testing `AOCL_HC_COMPUTE_HASH_MASK` macro
*/
class LZMA_hashMaskLimit : public ::testing::Test
{
public:
    void is_mask_valid(UInt32 mask, UInt32 block_cnt) {
        //Mask when applied should not allow values outside block_cnt
        EXPECT_LT(mask, block_cnt);

        //Mask must be of the form 00...11... or all 0s, all 1s
        int lz = __builtin_clz(mask);
        int tz = __builtin_ctz(~mask);
        EXPECT_EQ((lz + tz), 32);
    }

    void is_mask_invalid(UInt32 mask, UInt32 block_cnt) {
        //Mask when applied should not allow values outside block_cnt
        EXPECT_GE(mask, block_cnt);

        //Mask must be of the form 00...11... or all 0s, all 1s
        int lz = __builtin_clz(mask);
        int tz = __builtin_ctz(~mask);
        EXPECT_EQ((lz + tz), 32);
    }
};

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_pow2Size_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = (1 << 20);
    block_cnt = sz / 8;
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_valid(mask, block_cnt);
}

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_powGt2Size_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = (1 << 20) + 1;
    block_cnt = sz / 8;
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_valid(mask, block_cnt);
}

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_powLt2Size_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = (1 << 20) - 1;
    block_cnt = sz / 8;
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_valid(mask, block_cnt);
}

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_blkGtSize_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = (1 << 20);
    block_cnt = sz >> 1;
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_valid(mask, block_cnt);
}

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_randomSize_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = 0x1F503F0;
    block_cnt = (sz << 1) - 123;
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_valid(mask, block_cnt);
}

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_minSize_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = kHashGuarentee - 1;
    block_cnt = kHashGuarentee;
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_valid(mask, block_cnt);
}

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_negMinSize_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = kHashGuarentee - 1;
    block_cnt = kHashGuarentee - 1; //invalid block_cnt
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_invalid(mask, block_cnt);
}

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_maxSize_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = 0xFFFFFFFF;
    block_cnt = sz;
    mask = Test_Compute_Hash_Mask(sz, block_cnt);
    is_mask_valid(mask, block_cnt);
}

/*********************************************
 * End of LZMA_hashMaskLimit
 *********************************************/

/*********************************************
* "Begin" of LZMA_hashMaskCollision
*********************************************/
#define UT_kCrcPoly 0xEDB88320
/*
    This is a fixture class for testing `AOCL_HASH5_CALC` macro
    Hash must guarentee byte-1 and byte-2 are same for all items that
    map to a particular hash. Testing this aspect of hash generation here.
*/
class LZMA_hashMaskCollision : public ::testing::Test
{
public:
    void SetUp() override {
        //compute crc
        for (int i = 0; i < 256; i++)
        {
            UInt32 r = (UInt32)i;
            unsigned j;
            for (j = 0; j < 8; j++)
                r = (r >> 1) ^ (UT_kCrcPoly & ((UInt32)0 - (r & 1)));
            p.crc[i] = r;
        }
    }

    /* Hash must guarentee byte-1 and byte-2 are same for all items that
    * map to a particular hash */
    bool validate_hash_collision(Byte* cur, CMatchFinder* p, Byte c1, Byte c2) {
        UInt32 refHash = Test_Compute_Hash(cur, p);
        bool match = false;
        //Test all combinations for byte-1 and byte-2. None of them should match refHash
        for (int i = 0; i < 256; ++i) {
            for (int j = 0; j < 256; ++j) {
                cur[1] = (Byte)i; cur[2] = (Byte)j;
                if (cur[1] == c1 && cur[2] == c2) continue; //skip ref
                UInt32 testHash = Test_Compute_Hash(cur, p);
                if (testHash == refHash) {
                    match = true;
                    break;
                }
            }
        }
        return match;
    }

    CMatchFinder p;
};

TEST_F(LZMA_hashMaskCollision, AOCL_Compression_lzma_AOCL_HASH5_CALC_pos5h19b_common_1)
{
    Byte cur[5]; //5 - byte hash test
    cur[1] = (Byte)'q'; cur[2] = (Byte)'w';
    cur[0] = (Byte)'a'; cur[3] = (Byte)'b'; cur[4] = (Byte)'c'; //fix remaining bytes
    p.hashMask = 0x7FFFF;
    EXPECT_NE(validate_hash_collision(cur, &p, cur[1], cur[2]), true);
}

TEST_F(LZMA_hashMaskCollision, AOCL_Compression_lzma_AOCL_HASH5_CALC_pos5hMax32b_common_1)
{
    Byte cur[5]; //5 - byte hash test
    cur[1] = (Byte)'-'; cur[2] = (Byte)'+';
    cur[0] = (Byte)'1'; cur[3] = (Byte)'2'; cur[4] = (Byte)'3'; //fix remaining bytes
    p.hashMask = 0xFFFFFFFF;
    EXPECT_NE(validate_hash_collision(cur, &p, cur[1], cur[2]), true);
}

TEST_F(LZMA_hashMaskCollision, AOCL_Compression_lzma_AOCL_HASH5_CALC_pos5hMin16b_common_1)
{
    Byte cur[5]; //5 - byte hash test
    cur[1] = (Byte)'1'; cur[2] = (Byte)'0';
    cur[0] = (Byte)'x'; cur[3] = (Byte)'y'; cur[4] = (Byte)'z'; //fix remaining bytes
    p.hashMask = kHashGuarentee - 1;
    EXPECT_NE(validate_hash_collision(cur, &p, cur[1], cur[2]), true);
}

TEST_F(LZMA_hashMaskCollision, AOCL_Compression_lzma_AOCL_HASH5_CALC_neg5hMask15b_common_1)
{
    Byte cur[5]; //5 - byte hash test
    cur[1] = (Byte)'1'; cur[2] = (Byte)'0';
    cur[0] = (Byte)'x'; cur[3] = (Byte)'y'; cur[4] = (Byte)'z'; //fix remaining bytes
    p.hashMask = 0x7FFF;
    EXPECT_NE(validate_hash_collision(cur, &p, cur[1], cur[2]), false);
}

/*********************************************
 * End of LZMA_hashMaskCollision
 *********************************************/

/*********************************************
* "Begin" of LZMA_circularBuffer
*********************************************/

/*
    This is a fixture class for testing `CIRC_INC_HEAD` and 'CIRC_DEC_HEAD' macro
*/
class LZMA_circularBuffer : public ::testing::Test
{
public:
    /* [headPtr | hcChain]
    for hcHead = 1 to HASH_CHAIN_MAX, inc by 1
    for hcHead = HASH_CHAIN_MAX, circle back to 1 */
    void circular_buffer_inc_test(UInt32 HASH_CHAIN_SLOT_SZ, UInt32 HASH_CHAIN_MAX) {
        UInt32 hcHead = 1, hcHeadNxt;
        for (int i = hcHead; i < HASH_CHAIN_MAX; ++i) {
            hcHeadNxt = Test_Circular_Inc(hcHead, HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
            EXPECT_EQ(hcHeadNxt, (hcHead + 1));
            hcHead = hcHeadNxt;
        }
        hcHead = HASH_CHAIN_MAX;
        hcHeadNxt = Test_Circular_Inc(hcHead, HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
        EXPECT_EQ(hcHeadNxt, 1);
    }

    void circular_buffer_dec_test(UInt32 HASH_CHAIN_SLOT_SZ, UInt32 HASH_CHAIN_MAX) {
        UInt32 hcHead = HASH_CHAIN_MAX, hcHeadNxt;
        for (int i = hcHead; i > 1; --i) {
            hcHeadNxt = Test_Circular_Dec(hcHead, HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
            EXPECT_EQ(hcHeadNxt, (hcHead - 1));
            hcHead = hcHeadNxt;
        }
        hcHead = 1;
        hcHeadNxt = Test_Circular_Dec(hcHead, HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
        EXPECT_EQ(hcHeadNxt, HASH_CHAIN_MAX);
    }
};

TEST_F(LZMA_circularBuffer, AOCL_Compression_lzma_CIRC_INC_HEAD_Inc8_common_1)
{
    UInt32 HASH_CHAIN_SLOT_SZ = 8;
    UInt32 HASH_CHAIN_MAX = HASH_CHAIN_SLOT_SZ - 1;
    circular_buffer_inc_test(HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
}

TEST_F(LZMA_circularBuffer, AOCL_Compression_lzma_CIRC_INC_HEAD_Inc16_common_1)
{
    UInt32 HASH_CHAIN_SLOT_SZ = 16;
    UInt32 HASH_CHAIN_MAX = HASH_CHAIN_SLOT_SZ - 1;
    circular_buffer_inc_test(HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
}

TEST_F(LZMA_circularBuffer, AOCL_Compression_lzma_CIRC_DEC_HEAD_Dec8_common_1)
{
    UInt32 HASH_CHAIN_SLOT_SZ = 8;
    UInt32 HASH_CHAIN_MAX = HASH_CHAIN_SLOT_SZ - 1;
    circular_buffer_dec_test(HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
}

TEST_F(LZMA_circularBuffer, AOCL_Compression_lzma_CIRC_DEC_HEAD_Dec16_common_1)
{
    UInt32 HASH_CHAIN_SLOT_SZ = 16;
    UInt32 HASH_CHAIN_MAX = HASH_CHAIN_SLOT_SZ - 1;
    circular_buffer_dec_test(HASH_CHAIN_SLOT_SZ, HASH_CHAIN_MAX);
}

/*********************************************
 * End of LZMA_circularBuffer
 *********************************************/