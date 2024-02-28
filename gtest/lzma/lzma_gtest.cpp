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

#include "algos/lzma/Alloc.h"
#include "algos/lzma/LzFind.h"
#include "algos/lzma/LzHash.h"
#include "algos/lzma/LzmaEnc.h"
#include "algos/lzma/LzmaDec.h"
#include "algos/common/aoclHashChain.h"

using namespace std;

#define DEFAULT_OPT_LEVEL 2 // system running gtest must have AVX support

#ifdef AOCL_LZMA_OPT
void aocl_setup_lzma_test() {
    int optLevel = DEFAULT_OPT_LEVEL;
    aocl_setup_lzma_encode(0, optLevel, 0, 0, 0);
    aocl_setup_lzma_decode(0, optLevel, 0, 0, 0);
}

/* This base class can be used for all fixtures
* that require dynamic dispatcher setup */
class AOCL_setup_lzma : public ::testing::Test {
public:
    AOCL_setup_lzma() {
        aocl_setup_lzma_test();
    }
};

/*********************************************
 * Begin of Unit Tests
 *********************************************/

/*********************************************
 * "Begin" of LZMA_byteMatching
 *********************************************/

 /*
     Fixture class for testing `AOCL_FIND_MATCHING_BYTES_LEN` macro
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
        EXPECT_EQ(org_find_match(len, pb, cur, 3),
            opt_find_match(len, pb, cur, 3)); //AOCL_Compression_lzma_byteMatching_shortStr_common_1
    }
    {
        Byte pb[] = "ab";
        EXPECT_EQ(org_find_match(len, pb, cur, 2),
            opt_find_match(len, pb, cur, 2)); //AOCL_Compression_lzma_byteMatching_shortStr_common_2
    }
    {
        Byte pb[] = "a";
        EXPECT_EQ(org_find_match(len, pb, cur, 1),
            opt_find_match(len, pb, cur, 1)); //AOCL_Compression_lzma_byteMatching_shortStr_common_3
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
    for (unsigned lenLimit = 0; lenLimit < 4; ++lenLimit) {
        Byte pb[] = "abcd1234";
        Byte cur[] = "abcd1234";
        EXPECT_EQ(org_find_match(len, pb, cur, lenLimit),
            opt_find_match(len, pb, cur, lenLimit)); //AOCL_Compression_lzma_byteMatching_lenLimitShort_common_1 - _4
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
     Fixture class for testing `AOCL_HC_COMPUTE_HASH_MASK` macro
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

TEST_F(LZMA_hashMaskLimit, AOCL_Compression_lzma_AOCL_HC_COMPUTE_HASH_MASK_szZero_common_1)
{
    UInt32 sz, block_cnt, mask;
    sz = 0;
    block_cnt = kHashGuarentee;
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
     Fixture class for testing `AOCL_HASH5_CALC` macro
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
    EXPECT_EQ(validate_hash_collision(cur, &p, cur[1], cur[2]), true);
}

/*********************************************
 * End of LZMA_hashMaskCollision
 *********************************************/

 /*********************************************
 * Begin of LZMA_normalizeLzFind
 *********************************************/
#define EMPTY_U32 UINT_MAX
#define LZMA_PROPS_SIZE 5
#define MIN_PAD_SIZE (16 * 1024) //Minimum compressed buffer size
 /*
  Fixture class for testing AOCL_MatchFinder_Normalize3 function
  in LzFind.c
 */
class LZMA_normalizeLzFind : public ::testing::Test
{
public:
    void SetUp() override {
        hash_r = son_r = hash_o = son_o = dict_m = nullptr;
    }

    void TearDown() override
    {
        if (dict_m) free(dict_m);
        if (hash_r) free(hash_r);
        if (hash_o) free(hash_o);
    }

private:
    static CLzRef* util_alloc_refs(size_t num)
    {
        size_t sizeInBytes = (size_t)num * sizeof(CLzRef);
        if (sizeInBytes / sizeof(CLzRef) != num)
            return nullptr;
        return (CLzRef*)malloc(sizeInBytes);
    }

    //Circular buffer not implemented here, do not call for 'pos' out of bounds
    static void fill_slot_ref(CLzRef* hash_r, CLzRef* son_r, UInt32 hash, UInt32 pos) {
        son_r[pos] = hash_r[hash]; //move prev head to son
        hash_r[hash] = pos; //move curent head to hash
    }

    //Circular buffer not implemented here, do not call for 'pos' out of bounds
    static void fill_slot_opt(CLzRef* son_o, UInt32 HASH_CHAIN_SLOT_SZ, UInt32 hash, UInt32 headPos, UInt32 pos) {
        UInt32 bucket = hash * HASH_CHAIN_SLOT_SZ;
        son_o[bucket] = bucket + headPos;
        son_o[bucket + headPos] = pos;
    }

    //Setup reference buffers
    void setup_ref_buffers(UInt32 flexHashSize) {
        hashSizeSum_r = fixedHashSize + flexHashSize;
        hashMask_r = hashMask;
        numRefs_r = hashSizeSum_r + sonSize; //hash and son tables
        hash_r = util_alloc_refs(numRefs_r);
        ASSERT_NE(hash_r, nullptr);
        memset(hash_r, 0, numRefs_r * sizeof(CLzRef));
        son_r = hash_r + hashSizeSum_r;
    }

    //Setup optimized buffers
    void setup_opt_buffers(UInt32 flexHashSize) {
        hashSizeSum_o = fixedHashSize;
        hashMask_o = hashMask;
        numRefs_o = hashSizeSum_o + sonSize + flexHashSize; //hash table, combined hash_head+chain table
        hash_o = util_alloc_refs(numRefs_o);
        memset(hash_o, 0, numRefs_o * sizeof(CLzRef));
        ASSERT_NE(hash_o, nullptr);
        son_o = hash_o + hashSizeSum_o;
    }

    /* Only values of nodes should change during normalization.
    * Positions should remain the same.
    * Before normalization, mapping is created between ref and opt
    * dictionaries. After normalization, same mapping is used
    * to validate if the modified values are the same */
    void setup_validation_map() {
        dict_m = util_alloc_refs(numRefs_r); //for validation
        ASSERT_NE(dict_m, nullptr);
        for (size_t i = 0; i < numRefs_r; ++i) {
            dict_m[i] = EMPTY_U32;
        }
    }

    void fill_test_data(UInt32 flexHashSize, int UT_HASH_CHAIN_SLOT_SZ) {
        //Fill dictionary until cyclicBufferSize. Distribute among hashes
        size_t pos_cnt = 0;
        for (size_t j = 1; j < (size_t)UT_HASH_CHAIN_SLOT_SZ; ++j) {
            for (size_t h = 0; h < flexHashSize; ++h) {
                fill_slot_ref(hash_r + fixedHashSize, son_r, h, pos_cnt);
                fill_slot_opt(son_o, UT_HASH_CHAIN_SLOT_SZ, h, j, pos_cnt);
                pos_cnt++;
                if (pos_cnt == cyclicBufferSize) break;
            }
            if (pos_cnt == cyclicBufferSize) break;
        }

        //Fill fixed hash table
        for (UInt32 h = 0; h < fixedHashSize; ++h) {
            UInt32 val = rand() % cyclicBufferSize; // (cyclicBufferSize - h)
            hash_r[h] = hash_o[h] = val;
        }
    }

    /* Validates initialization of both ref and opt dictionaries are equivalent
    * Generates map of equivalent positions from hash_r to hash_o and saves in dict_m
    * dict_m can be used to compare equivalent nodes post test */
    int hc_validate_normalize_init(CLzRef* hash_r, CLzRef* son_r, CLzRef* hash_o, CLzRef* son_o,
        UInt32 fixedHashSize, UInt32 flexHashSize, UInt32 HASH_CHAIN_SLOT_SZ, CLzRef* dict_m) {
        //Check same node in both dictionaries and validate they are the same

        //Validate fixed hash tables
        for (size_t h = 0; h < fixedHashSize; ++h) {
            if (hash_r[h] != hash_o[h])
                return -1;
            dict_m[h] = h; //1-1 mapping for fixed tables
        }

        //Validate main hash table
        CLzRef* hash_rc = hash_r + fixedHashSize;
        for (size_t h = 0; h < flexHashSize; ++h) {
            //check hash head
            UInt32 bucket = h * HASH_CHAIN_SLOT_SZ;
            UInt32 hcHead = son_o[bucket];
            CLzRef Hashr = hash_rc[h];
            CLzRef Hasho = son_o[hcHead];
            if (Hashr != Hasho)
                return -1;
            dict_m[(hash_rc + h) - hash_r] = (son_o + hcHead) - hash_o; //save offset wrt hash_o/hash_r

            //check chain
            hcHead--;
            int nodeCnt = hcHead - bucket; //chain-len excluding head (head already checked above)
            while (nodeCnt > 0) {
                if (son_r[Hashr] != son_o[hcHead])
                    return -1;
                dict_m[(son_r + Hashr) - hash_r] = (son_o + hcHead) - hash_o;
                Hashr = son_r[Hashr];
                hcHead--;
                nodeCnt--;
            }
        }
        return 0;
    }

    int hc_validate_normalize(CLzRef* dict_r, CLzRef* dict_o, CLzRef* dict_m, size_t sz) {
        //Check equivalent nodes in both dictionaries and validate they are the same
        for (size_t s = 0; s < sz; ++s) {
            size_t idx = dict_m[s];
            if (idx != EMPTY_U32 && dict_r[s] != dict_o[idx])
                return -1;
        }
        return 0;
    }

public:
    void check_params(UInt32 historySize, UInt32 flexHashSize, int level, int slot_sz) {
        ASSERT_EQ(__builtin_popcount(flexHashSize), 1); //must be power of 2
        ASSERT_EQ((slot_sz == 8 || slot_sz == 16), true); //only block sizes 8 and 16 are supported
        ASSERT_GE(historySize, 4);
        if (slot_sz == 8) {
            ASSERT_LT(level, HASH_CHAIN_16_LEVEL); //block size 8 supported only for lower levels
        }
        else {
            ASSERT_GE(level, HASH_CHAIN_16_LEVEL); //block size 16 supported only for higher levels
        }
    }

    void run_normalize_lzfind(UInt32 historySize, UInt32 flexHashSize, int level, int UT_HASH_CHAIN_SLOT_SZ) {
        cyclicBufferSize = historySize + 1; // do not change it
        fixedHashSize = kHash2Size; //2-byte hash is 10-bits
        ASSERT_EQ(__builtin_popcount(flexHashSize), 1); //must be power of 2
        hashMask = flexHashSize - 1;
        sonSize = flexHashSize * (UT_HASH_CHAIN_SLOT_SZ - 1); //keep same size for both ref and opt. We use this to validate all elements.
        UInt32 pos = sonSize; //assume entire dict is full. We will do the same when filling test data
        UInt32 subValue = (pos - historySize - 1); //as per MatchFinder_CheckLimits

        //Allocate buffers
        setup_ref_buffers(flexHashSize);
        setup_validation_map();
        setup_opt_buffers(flexHashSize);
        ASSERT_GT(numRefs_o, cyclicBufferSize);
        ASSERT_GT(numRefs_r, cyclicBufferSize);

        //Fill test data
        fill_test_data(flexHashSize, UT_HASH_CHAIN_SLOT_SZ);

        //Validate before normalize. Do dictionaries match between ref and opt?
        EXPECT_GE(hc_validate_normalize_init(hash_r, son_r, hash_o, son_o, fixedHashSize,
            flexHashSize, UT_HASH_CHAIN_SLOT_SZ, dict_m), 0);

        //Validate before normalize using mapping. Do dictionaries match between ref and opt?
        EXPECT_GE(hc_validate_normalize(hash_r, hash_o, dict_m, numRefs_r), 0);

        Test_HC_MatchFinder_Normalize3(subValue, hash_r, son_r, btMode, fixedHashSize,
            cyclicBufferSize, hashSizeSum_r, numRefs_r, hashMask_r, level);

        Test_AOCL_HC_MatchFinder_Normalize3(subValue, hash_o, son_o, btMode, fixedHashSize,
            cyclicBufferSize, hashSizeSum_o, numRefs_o, hashMask_o, level);

        //Validate after normalize
        EXPECT_GE(hc_validate_normalize(hash_r, hash_o, dict_m, numRefs_r), 0);

        if (dict_m) { free(dict_m); dict_m = nullptr; }
        if (hash_r) { free(hash_r); hash_r = nullptr; }
        if (hash_o) { free(hash_o); hash_o = nullptr; }
    }

private:
    UInt32 hashSizeSum_r, hashMask_r;
    UInt32 hashSizeSum_o, hashMask_o;
    CLzRef* hash_r, * son_r, * dict_m;
    CLzRef* hash_o, * son_o;
    size_t numRefs_r;
    size_t numRefs_o;
    const Byte btMode = 0;
    UInt32 cyclicBufferSize, fixedHashSize, hashMask, sonSize;
};

TEST_F(LZMA_normalizeLzFind, AOCL_Compression_lzma_AOCL_MatchFinder_Normalize3_Hc8_common_1)
{
    UInt32 historySize = (1 << 20);
    UInt32 flexHashSize = historySize >> 2;
    int level = HASH_CHAIN_16_LEVEL - 1;
    int slot_sz = 8;
    check_params(historySize, flexHashSize, level, slot_sz);
    run_normalize_lzfind(historySize, flexHashSize, level, slot_sz);
}

TEST_F(LZMA_normalizeLzFind, AOCL_Compression_lzma_AOCL_MatchFinder_Normalize3_Hc16_common_1)
{
    UInt32 historySize = (1 << 20);
    UInt32 flexHashSize = historySize >> 2;
    int level = HASH_CHAIN_16_LEVEL;
    int slot_sz = 16;
    check_params(historySize, flexHashSize, level, slot_sz);
    run_normalize_lzfind(historySize, flexHashSize, level, slot_sz);
}

/*********************************************
 * End of LZMA_normalizeLzFind
 *********************************************/

 /*********************************************
 * Begin of LZMA_hcGetMatchesSpecLzFind
 *********************************************/
#define MIN_MATCH_LEN 3
#define CUT_VALUE 32
#define OUT_SIZE (CUT_VALUE * 2) //<l,d> pairs
#define SET_LENGTH(i) (i.size())
#define SET_OFFSET(i) (i.size()+1) //Hc_GetMatchesSpec returns (delta-1) for offset

 //Add <l,_> of the pair to be matched
#define SET_MATCH_LENGTH(match) \
        matches[lm] = SET_LENGTH(match); lm += 2;

//Add <_,d> of the pair to be matched
#define SET_MATCH_OFFSET(inp) \
        matches[om] = SET_OFFSET(inp); om -= 2;

//Add nodes to hash chain object in son
#define ADD_NODE_TO_CHAIN(inp) { \
    UInt32 val = inp.size(); \
    UInt32 HASH_CHAIN_OBJECT_SZ = blockSz; \
    UInt32 HASH_CHAIN_MAX = HASH_CHAIN_OBJECT_SZ - 1; \
    UInt32 hashIdx = 0; \
    AOCL_COMMON_CEHCFIX_GET_HEAD(son, 0, hcHead, 0, hashIdx, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, 0); \
    AOCL_COMMON_CEHCFIX_INSERT(son, 0, hcHead, 0, val, hashIdx, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX) \
}

//We expect to get matches for these. Update offset value in matches[].
#define ADD_MATCH_STR(i, str) { \
        ADD_NODE_TO_CHAIN(inp) \
        SET_MATCH_OFFSET(inp); \
        inp += str; \
        }

//We do not expect matches for these
#define ADD_INP_STR(i, str) { \
        ADD_NODE_TO_CHAIN(inp) \
        inp += str; \
        }

 /*
     Fixture class for testing AOCL_Hc_GetMatchesSpec_8, AOCL_Hc_GetMatchesSpec_16
 */
class LZMA_hcGetMatchesSpecLzFind : public ::testing::Test
{
public:
    void Init_Buffers(CLzRef son[], UInt32 blockSz) {
        for (int i = 0; i < OUT_SIZE; ++i) {
            matches[i] = 0;
            ldPairs[i] = 0;
        }
        //son[0] = 0; //head ptr
        for (UInt32 i = 0; i < blockSz; ++i) {
            son[i] = 0;
        }
    }

    //Supplied test inputs that expect to match, must ensure MIN_MATCH_LEN bytes match with curstr
    void validate_match_candidates(std::vector<std::string>& matchStrs, std::string& curstr) {
        ASSERT_GE(curstr.size(), MIN_MATCH_LEN);
        for (size_t i = 0; i < matchStrs.size(); ++i) {
            ASSERT_GE(matchStrs[i].size(), MIN_MATCH_LEN);
            for (int j = 0; j < MIN_MATCH_LEN; ++j) {
                ASSERT_EQ(matchStrs[i][j], curstr[j]);
            }
        }
    }

    void set_match_lengths(std::vector<std::string>& matchStrs) {
        int lm = 0;
        for (size_t i = 0; i < matchStrs.size(); ++i) {
            SET_MATCH_LENGTH(matchStrs[i]);
        }
    }

    //Call AOCL_Hc_GetMatchesSpec_* and get ldPairs[]. Compare with expected values in matches[]
    void test_hc_get_matches_spec(std::string inp, size_t lenLimit, UInt32 hcHead,
        CLzRef* son, int blockSz, size_t curSz) {
        //Adjust expected offset values relative to final length of inp
        UInt32 pos = inp.size() - curSz;
        const char* cur = inp.c_str() + pos;
        for (int i = 1; i < OUT_SIZE; i += 2) { //for offsets
            if (matches[i] > 0)
                matches[i] = pos - matches[i]; //offset wrt pos
        }

        const size_t cyclicBufferPos = 0; //not used
        unsigned maxLen = MIN_MATCH_LEN;
        bool match = true;
        UInt32 hv = hcHead / blockSz;
        Test_Hc_GetMatchesSpec(lenLimit, hcHead, hv, pos, (const Byte*)cur, son,
            cyclicBufferPos, cyclicBufferSize, CUT_VALUE, ldPairs, maxLen, blockSz);
        for (int i = 0; i < OUT_SIZE; ++i) {
            if (ldPairs[i] != matches[i]) {
                match = false;
                break;
            }
        }
        EXPECT_EQ(match, true);
    }

    UInt32 matches[OUT_SIZE]; //<l,d> pairs for validation

private:
    UInt32 ldPairs[OUT_SIZE]; //<l,d> pairs output
    const UInt32 cyclicBufferSize = UINT_MAX; //large value
};

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_8_common_1) //longer matches down the chain
{
    const size_t blockSz = 8;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead;
    std::string curstr = "qwertyabcd";
    std::vector<std::string> matchStrs =
    { "qwerty" , "qwertyab", "qwertyabcd" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);
    set_match_lengths(matchStrs);
    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    ADD_INP_STR(7, matchStrs[2]);
    ADD_INP_STR(6, "qwertyabced ");
    ADD_INP_STR(5, "qwerty ");
    ADD_MATCH_STR(4, matchStrs[2]);
    ADD_MATCH_STR(3, matchStrs[1]);
    ADD_INP_STR(2, "qwerty ");
    ADD_MATCH_STR(1, matchStrs[0]);
    inp += curstr; //current string to match against
    size_t lenLimit = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, lenLimit);
}

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_8_common_2) //longest match at closest offset
{
    const size_t blockSz = 8;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead;
    std::string curstr = "qwertyabcd";
    std::vector<std::string> matchStrs =
    { "qwertyabcd" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);
    set_match_lengths(matchStrs);
    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    ADD_INP_STR(7, "qwertyabc");
    ADD_INP_STR(6, "qwertyabced ");
    ADD_INP_STR(5, "qwertyabc ");
    ADD_INP_STR(4, "qwertyab ");
    ADD_INP_STR(3, "qwertya ");
    ADD_INP_STR(2, "qwerty ");
    ADD_MATCH_STR(1, matchStrs[0]); //longest match found here, shorter matches at further offsets are ignored
    inp += curstr; //current string to match against
    size_t lenLimit = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, lenLimit);
}

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_8_common_3) //does not get longest match due to UInt16 optimization
{
    const size_t blockSz = 8;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead;
    std::string curstr = "qwertyabcde";
    std::vector<std::string> matchStrs =
    { "qwertya", "qwertyabc" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);
    set_match_lengths(matchStrs);
    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    ADD_INP_STR(7, "qwertyabcd");
    ADD_INP_STR(6, "qwertyabced ");
    ADD_INP_STR(5, "qwerty ");
    /* In general, in AOCL_HC_GETMATCHES_SPEC, as "qwertyabc" match is found, 
    * "qwertyabcd" won't match, due to UInt16 comparison optimization: 
    * "if (GetUi16(cur + checkLen) == GetUi16(cur + checkLen + diff) { ... }"
    * However, when maxLen == (lenLimit - 1), checkLen gets set to (lenLimit - 2)
    * allowing such a match to be possible. 
    * This test case is designed so that maxLen != (lenLimit - 1) at this point.*/
    ADD_INP_STR(4, "qwertyabcd"); //does not match here, 1-byte additional match only.
    ADD_MATCH_STR(3, matchStrs[1]);
    ADD_INP_STR(2, "qwerty ");
    ADD_MATCH_STR(1, matchStrs[0]);
    inp += curstr; //current string to match against
    size_t lenLimit = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, lenLimit);
}

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_8_common_4) //Head at 4
{
    const size_t blockSz = 8;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead, hcBase = 0;
    std::string curstr = "qwertyabcd";
    std::vector<std::string> matchStrs =
    { "qwerty", "qwertyab" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);
    set_match_lengths(matchStrs);
    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    son[hcBase] = 4; //start with head at 4
    ADD_INP_STR(4, "qwertyabef");
    ADD_INP_STR(3, "qwertyabc7987");
    ADD_INP_STR(2, "qwerty ");
    ADD_INP_STR(1, "qwerty ");
    ADD_MATCH_STR(7, matchStrs[1]);
    ADD_INP_STR(6, "qwer7645783");
    ADD_MATCH_STR(5, matchStrs[0]);
    inp += curstr; //current string to match against
    size_t lenLimit = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, lenLimit);
}

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_8_common_5) //Head at 1
{
    const size_t blockSz = 8;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead, hcBase = 0;
    std::string curstr = "qwertyabcd";
    std::vector<std::string> matchStrs =
    { "qwerty", "qwertyabc" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);
    set_match_lengths(matchStrs);
    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    son[hcBase] = 1; //start with head at 1
    ADD_INP_STR(1, "qwerty ");
    ADD_INP_STR(7, "qwerty#$%$%");
    ADD_INP_STR(6, "qwertyabe");
    ADD_MATCH_STR(5, matchStrs[1]);
    ADD_INP_STR(4, "qwe:&*89");
    ADD_INP_STR(3, "qwer76987");
    ADD_MATCH_STR(2, matchStrs[0]);
    inp += curstr; //current string to match against
    size_t lenLimit = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, lenLimit);
}

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_8_common_6) //match length > lenLimit
{
    const size_t blockSz = 8;
    const size_t lenLimit = 6;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead;
    std::string curstr = "qwertyabcd";
    std::vector<std::string> matchStrs =
    { "qwertyabcd" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);

    matches[0] = lenLimit; //expect to get match of lenLimit, although actual match length is longer

    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    ADD_INP_STR(7, "qwertyabc");
    ADD_INP_STR(6, "qwertyabced ");
    ADD_INP_STR(5, "qwertyabc ");
    ADD_INP_STR(4, "qwertyab ");
    ADD_INP_STR(3, "qwertya ");
    ADD_INP_STR(2, "qwerty ");
    ADD_MATCH_STR(1, matchStrs[0]); //longest match found here, shorter matches at further offsets are ignored
    inp += curstr; //current string to match against
    size_t curSz = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, curSz);
}

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_16_common_1) //block sz 16
{
    const size_t blockSz = 16;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead;
    std::string curstr = "qwertyabcd";
    std::vector<std::string> matchStrs =
    { "qwerty", "qwertyab", "qwertyabcd" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);
    set_match_lengths(matchStrs);
    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    ADD_INP_STR(15, "qwert7987");
    ADD_INP_STR(14, "qwert^&&*");
    ADD_INP_STR(13, "qwertFGGHJ");
    ADD_INP_STR(12, "qwertsdad");
    ADD_INP_STR(11, "qwert");
    ADD_INP_STR(10, "qwert");
    ADD_INP_STR(9, "qwert");
    ADD_MATCH_STR(8, matchStrs[2]);
    ADD_INP_STR(7, "qwerty123");
    ADD_INP_STR(6, "qwertyabced ");
    ADD_INP_STR(5, "qwertywe");
    ADD_INP_STR(4, "qwerty ");
    ADD_MATCH_STR(3, matchStrs[1]);
    ADD_INP_STR(2, "asdfg");
    ADD_MATCH_STR(1, matchStrs[0]);
    inp += curstr; //current string to match against
    size_t lenLimit = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, lenLimit);
}

TEST_F(LZMA_hcGetMatchesSpecLzFind, AOCL_Compression_lzma_AOCL_Hc_GetMatchesSpec_16_common_2) //block sz 16, partially full buffer
{
    const size_t blockSz = 16;
    CLzRef son[blockSz];
    Init_Buffers(son, blockSz);

    //Setup input stream
    UInt32 hcHead, hcBase = 0;
    std::string curstr = "qwertyabcd";
    std::vector<std::string> matchStrs =
    { "qwerty", "qwertyab", "qwertyabcd" }; //strings we expect to get matches for
    validate_match_candidates(matchStrs, curstr);
    set_match_lengths(matchStrs);
    std::string inp;
    int om = (matchStrs.size() * 2) - 1;
    son[hcBase] = 9; //start with head at 9. Do not fill all nodes
    ADD_INP_STR(9, "qwert");
    ADD_MATCH_STR(8, matchStrs[2]);
    ADD_INP_STR(7, "qwerty123");
    ADD_INP_STR(6, "qwertyabced ");
    ADD_INP_STR(5, "qwertywe");
    ADD_INP_STR(4, "qwerty ");
    ADD_MATCH_STR(3, matchStrs[1]);
    ADD_INP_STR(2, "asdfg");
    ADD_MATCH_STR(1, matchStrs[0]);
    inp += curstr; //current string to match against
    size_t lenLimit = SET_LENGTH(curstr);

    test_hc_get_matches_spec(inp, lenLimit, hcHead, son, blockSz, lenLimit);
}

/*********************************************
 * End of LZMA_hcGetMatchesSpecLzFind
 *********************************************/

/*********************************************
 * Begin of LZMA_decRangeCoder
 *********************************************/
#define NUM_BASE_PROBS 1984
#define SYM_SIZE 1024
/*
    Fixture class for testing range coder updates
*/
class LZMA_decRangeCoder : public ::testing::Test
{
public:
    void SetUp() override {
        //Init buffers
        buffer = (Byte*)malloc(bufsz * sizeof(Byte));
        for (size_t i = 0; i < bufsz; ++i) {
            buffer[i] = (Byte)(rand() % UINT8_MAX);
        }

        for (size_t i = 0; i < symsz; ++i) {
            symbols[i] = (Byte)(rand() % UINT8_MAX);
        }

        for (int i = 0; i < NUM_BASE_PROBS; ++i) {
            prob[i] = (CLzmaProb)(rand() % UINT16_MAX);
        }
    }

    void TearDown() override {
        free(buffer);
    }

    typedef std::pair<UInt32, UInt32> RCPair;
    typedef void (*Fp_Normal_Literal_Dec)(const Byte* buf, UInt32* _range,
        UInt32* _code, CLzmaProb* prob, unsigned symbol);

    RCPair run_normal_lit_dec(Fp_Normal_Literal_Dec test_func,
        const Byte* buffer, unsigned const symbols[], CLzmaProb prob[], const size_t bufsz, const size_t symsz) {
        UInt32 range = 0xFFFFFFFF; //start with max range value
        UInt32 code = (UInt32)buffer[1] << 24 |
            (UInt32)buffer[2] << 16 |
            (UInt32)buffer[3] << 8 |
            (UInt32)buffer[4];
        const Byte* buf = buffer + 5;
        for (size_t i = 5; i < bufsz; ++i) {
            test_func(buf, &range, &code, prob, symbols[i % symsz]);
            buf++;
        }
        return std::make_pair(range, code);
    }

    RCPair run_normal_lit_dec_low_range(Fp_Normal_Literal_Dec test_func,
        const Byte* buffer, unsigned const symbols[], CLzmaProb prob[], const size_t bufsz, const size_t symsz) {
        const UInt32 kTopValue = ((UInt32)1 << 24); //#define kTopValue ((UInt32)1 << kNumTopBits)
        UInt32 range = kTopValue; //start with lower range value
        UInt32 code = (UInt32)buffer[1] << 24 |
            (UInt32)buffer[2] << 16 |
            (UInt32)buffer[3] << 8 |
            (UInt32)buffer[4];
        const Byte* buf = buffer + 5;
        for (size_t i = 5; i < bufsz; ++i) {
            test_func(buf, &range, &code, prob, symbols[i % symsz]);
            buf++;
        }
        return std::make_pair(range, code);
    }

    void check_equal(const CLzmaProb prob_ref[], const CLzmaProb prob_opt[],
        RCPair rc_ref, RCPair rc_opt) {
        EXPECT_EQ(rc_ref.first, rc_opt.first);
        EXPECT_EQ(rc_ref.second, rc_opt.second);

        bool match = true;
        for (int i = 0; i < NUM_BASE_PROBS; ++i) {
            if (prob_ref[i] != prob_opt[i])
            {
                match = false;
                break;
            }
        }
        EXPECT_EQ(match, true);
    }

    const size_t bufsz = 1024 * 1024 * 8;
    Byte* buffer;
    const size_t symsz = SYM_SIZE;
    unsigned symbols[SYM_SIZE];
    CLzmaProb prob[NUM_BASE_PROBS];
};

TEST_F(LZMA_decRangeCoder, AOCL_Compression_lzma_AOCL_GET_BIT2_common_1) {
    CLzmaProb prob_ref[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_ref[i] = prob[i];
    }
    RCPair rc_ref = run_normal_lit_dec(Test_Rc_Get_Bit_2_Dec_Ref, buffer, symbols, prob_ref, bufsz, symsz);

    CLzmaProb prob_opt[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_opt[i] = prob[i];
    }
    RCPair rc_opt = run_normal_lit_dec(Test_Rc_Get_Bit_2_Dec_Opt, buffer, symbols, prob_opt, bufsz, symsz);

    check_equal(prob_ref, prob_opt, rc_ref, rc_opt);
}

TEST_F(LZMA_decRangeCoder, AOCL_Compression_lzma_AOCL_REV_BIT_common_1) {
    CLzmaProb prob_ref[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_ref[i] = prob[i];
    }
    RCPair rc_ref = run_normal_lit_dec(Test_Rc_Rev_Bit_Dec_Ref, buffer, symbols, prob_ref, bufsz, symsz);

    CLzmaProb prob_opt[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_opt[i] = prob[i];
    }
    RCPair rc_opt = run_normal_lit_dec(Test_Rc_Rev_Bit_Dec_Opt, buffer, symbols, prob_opt, bufsz, symsz);

    check_equal(prob_ref, prob_opt, rc_ref, rc_opt);
}

TEST_F(LZMA_decRangeCoder, AOCL_Compression_lzma_AOCL_GET_BIT2_lowRange_common_1) {
    CLzmaProb prob_ref[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_ref[i] = prob[i];
    }
    RCPair rc_ref = run_normal_lit_dec_low_range(Test_Rc_Get_Bit_2_Dec_Ref, buffer, symbols, prob_ref, bufsz, symsz);

    CLzmaProb prob_opt[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_opt[i] = prob[i];
    }
    RCPair rc_opt = run_normal_lit_dec_low_range(Test_Rc_Get_Bit_2_Dec_Opt, buffer, symbols, prob_opt, bufsz, symsz);

    check_equal(prob_ref, prob_opt, rc_ref, rc_opt);
}

TEST_F(LZMA_decRangeCoder, AOCL_Compression_lzma_AOCL_REV_BIT_lowRange_common_1) {
    CLzmaProb prob_ref[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_ref[i] = prob[i];
    }
    RCPair rc_ref = run_normal_lit_dec_low_range(Test_Rc_Rev_Bit_Dec_Ref, buffer, symbols, prob_ref, bufsz, symsz);

    CLzmaProb prob_opt[NUM_BASE_PROBS];
    for (int i = 0; i < NUM_BASE_PROBS; ++i) {
        prob_opt[i] = prob[i];
    }
    RCPair rc_opt = run_normal_lit_dec_low_range(Test_Rc_Rev_Bit_Dec_Opt, buffer, symbols, prob_opt, bufsz, symsz);

    check_equal(prob_ref, prob_opt, rc_ref, rc_opt);
}
/*********************************************
 * End of LZMA_decRangeCoder
 *********************************************/

/*********************************************
 * End of Unit Tests
 *********************************************/

/*********************************************
 * Begin API tests for LzmaEnc
 *********************************************/
static void* SzAlloc(ISzAllocPtr p, size_t size) { return malloc(size); }
static void SzFree(ISzAllocPtr p, void* address) { free(address); }
static void* SzAllocDummy(ISzAllocPtr p, size_t size) { return nullptr; }
static void SzFreeDummy(ISzAllocPtr p, void* address) { return; }

/*********************************************
* Begin of LZMA_encPropsInit
*********************************************/
TEST(LZMA_encPropsInit, AOCL_Compression_lzma_LzmaEncProps_Init_common_1) //check initialization is valid
{
    CLzmaEncProps p;
    LzmaEncProps_Init(&p);
    EXPECT_EQ(p.level, 5);
    EXPECT_EQ(p.dictSize, 0);
    EXPECT_EQ(p.mc, 0);
    EXPECT_EQ(p.writeEndMark, 0);
    EXPECT_EQ(p.affinity, 0);
    EXPECT_EQ(p.lc, -1);
    EXPECT_EQ(p.lp, -1);
    EXPECT_EQ(p.pb, -1);
    EXPECT_EQ(p.algo, -1);
    EXPECT_EQ(p.fb, -1);
    EXPECT_EQ(p.btMode, -1);
    EXPECT_EQ(p.numHashBytes, -1);
    EXPECT_EQ(p.numThreads, -1);
#ifdef AOCL_LZMA_OPT
    EXPECT_EQ(p.srcLen, 0);
#endif
}
/*********************************************
 * End of LZMA_encPropsInit
 *********************************************/

 /*********************************************
 * Begin of LZMA_encPropsNormalize
 *********************************************/
#define LZMA_MIN_LEVEL 0
#define LZMA_MAX_LEVEL 9
 /*
     Fixture class for testing AOCL_LzmaEncProps_Normalize
 */
class LZMA_encPropsNormalize : public AOCL_setup_lzma
{
public:
    void validate_defaults(CLzmaEncProps* p, int level) {
        EXPECT_EQ(p->level, level);
        EXPECT_EQ(p->lc, 3);
        EXPECT_EQ(p->lp, 0);
        EXPECT_EQ(p->pb, 2);
        if (p->level < 5) {
            EXPECT_EQ(p->algo, 0);
            EXPECT_EQ(p->btMode, 0);
            EXPECT_EQ(p->numHashBytes, 5);
        }
        else {
            EXPECT_EQ(p->algo, 1);
            EXPECT_EQ(p->btMode, 1);
            EXPECT_EQ(p->numHashBytes, 4);
        }

        if (p->level < 7) {
            EXPECT_EQ(p->fb, 32);
            if (p->btMode == 0) {
                EXPECT_EQ(p->mc, 16);
            }
            else {
                EXPECT_EQ(p->mc, 32);
            }
        }
        else {
            EXPECT_EQ(p->fb, 64);
            EXPECT_EQ(p->mc, 48);
        }
        EXPECT_EQ(p->numThreads, 1);
    }

    void set_user_settings(CLzmaEncProps* p) {
        p->level = 4;
        p->dictSize = ((UInt32)1 << 16);
        p->lc = 2;
        p->lp = 1;
        p->pb = 1;
        p->algo = 1;
        p->fb = 32;
        p->btMode = 1;
        p->numHashBytes = 5;
        p->mc = 32;
    }

    void validate_user_settings(CLzmaEncProps* p) {
        EXPECT_EQ(p->level, 4);
        EXPECT_EQ(p->dictSize, ((UInt32)1 << 16));
        EXPECT_EQ(p->lc, 2);
        EXPECT_EQ(p->lp, 1);
        EXPECT_EQ(p->pb, 1);
        EXPECT_EQ(p->algo, 1);
        EXPECT_EQ(p->fb, 32);
        EXPECT_EQ(p->btMode, 1);
        EXPECT_EQ(p->numHashBytes, 5);
        EXPECT_EQ(p->mc, 32);
    }

    void validate_ces(int btMode, size_t srcLen, int numHashBytes, int ces, int expected_ces) {
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        p.btMode = btMode;
        p.srcLen = srcLen;
        p.numHashBytes = numHashBytes;
        p.cacheEfficientStrategy = ces;
        AOCL_LzmaEncProps_Normalize(&p);
        EXPECT_EQ(p.cacheEfficientStrategy, expected_ces);
    }
};

TEST_F(LZMA_encPropsNormalize, AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_defaults_common) //defaults
{
    CLzmaEncProps p;
    for (int i = LZMA_MIN_LEVEL; i <= LZMA_MAX_LEVEL; ++i) {
        LzmaEncProps_Init(&p); //reset for each test
        p.level = i;
        Test_LzmaEncProps_Normalize_Dyn(&p);
        validate_defaults(&p, i); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_defaults_common_1 - 10
    }
}

TEST_F(LZMA_encPropsNormalize, AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_dictSize_common) //dictSizes
{
    { //reference settings
        CLzmaEncProps p;
        UInt32 dictSizes[10] = { 65536, 262144, 1048576, 4194304, 8388608, 16777216, 33554432, 33554432, 67108864, 67108864 };
        for (int i = LZMA_MIN_LEVEL; i <= LZMA_MAX_LEVEL; ++i) {
            LzmaEncProps_Init(&p); //reset for each test
            p.level = i;
            LzmaEncProps_Normalize(&p);
            EXPECT_EQ(p.dictSize, dictSizes[i]); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_dictSizeRef_common_1 - 10
        }
    }
#ifdef AOCL_LZMA_OPT
    { //optimized settings
        CLzmaEncProps p;
        UInt32 dictSizes[10] = { 524288, 1048576, 2097152, 8388608, 16777216, 16777216, 33554432, 33554432, 67108864, 67108864 };
        for (int i = LZMA_MIN_LEVEL; i <= LZMA_MAX_LEVEL; ++i) {
            LzmaEncProps_Init(&p); //reset for each test
            p.level = i;
            p.cacheEfficientStrategy = 1;
            AOCL_LzmaEncProps_Normalize(&p);
            EXPECT_EQ(p.dictSize, dictSizes[i]); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_dictSizeOpt_common_11 - 20
        }
    }
    { //optimized settings
        CLzmaEncProps p;
        UInt32 dictSizes[10] = { 65536, 262144, 1048576, 4194304, 8388608, 16777216, 33554432, 33554432, 67108864, 67108864 };
        for (int i = LZMA_MIN_LEVEL; i <= LZMA_MAX_LEVEL; ++i) {
            LzmaEncProps_Init(&p); //reset for each test
            p.level = i;
            p.cacheEfficientStrategy = 0;
            AOCL_LzmaEncProps_Normalize(&p);
            EXPECT_EQ(p.dictSize, dictSizes[i]); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_dictSizeOpt_common_21 - 30
        }
    }
#endif
}

TEST_F(LZMA_encPropsNormalize, AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_ces_common) //cache efficient settings
{
    /* Test strategy to enable cache efficient hash chain implementation:
    * If ces = -1, use this strategy:
    *   srcLen < MAX_SIZE_FOR_CE_HC_OFF : disabled
    *   MAX_SIZE_FOR_CE_HC_OFF <= srcLen < MIN_SIZE_FOR_CE_HC_ON : enabled/disabled based on numHashBytes
    *   srcLen >= MIN_SIZE_FOR_CE_HC_ON : enabled
    * else
    *   ces as set by user
    */
    //validate_ces(btMode, srcLen, numHashBytes, ces, expected_ces)
    validate_ces(1,    MIN_SIZE_FOR_CE_HC_ON, 5, -1, 0); //btMode=1
    validate_ces(0,                        0, 5, -1, 0); //srcLen=0
    validate_ces(0, MAX_SIZE_FOR_CE_HC_OFF-1, 5, -1, 0); //srcLen<MAX_SIZE_FOR_CE_HC_OFF
    validate_ces(0,   MAX_SIZE_FOR_CE_HC_OFF, 5, -1, 0); //MAX_SIZE_FOR_CE_HC_OFF <= srcLen < MIN_SIZE_FOR_CE_HC_ON, numHashBytes=5
    validate_ces(0,   MAX_SIZE_FOR_CE_HC_OFF, 4, -1, 1); //MAX_SIZE_FOR_CE_HC_OFF <= srcLen < MIN_SIZE_FOR_CE_HC_ON, numHashBytes=4
    validate_ces(0,    MIN_SIZE_FOR_CE_HC_ON, 5, -1, 1); //srcLen>=MIN_SIZE_FOR_CE_HC_ON
    validate_ces(0,    MIN_SIZE_FOR_CE_HC_ON, 5,  1, 1); //ces=1
    validate_ces(0,    MIN_SIZE_FOR_CE_HC_ON, 5,  0, 0); //ces=0
}

TEST_F(LZMA_encPropsNormalize, AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_reduceSize_common) //reduceSize
{
    { //reference settings
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        p.reduceSize = ((UInt32)1 << 16);
        LzmaEncProps_Normalize(&p);
        EXPECT_EQ(p.dictSize, p.reduceSize); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_reduceSize_common_1
    }
#ifdef AOCL_LZMA_OPT
    { //optimized settings
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        p.reduceSize = ((UInt32)1 << 16);
        AOCL_LzmaEncProps_Normalize(&p);
        EXPECT_EQ(p.dictSize, p.reduceSize); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_reduceSize_common_2
    }
#endif
}

TEST_F(LZMA_encPropsNormalize, AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_kReduceMin_common) //kReduceMin
{
    const UInt32 kReduceMin = ((UInt32)1 << 12);
    { //reference settings
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        p.reduceSize = ((UInt32)1 << 10);
        LzmaEncProps_Normalize(&p);
        EXPECT_EQ(p.dictSize, kReduceMin); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_kReduceMin_common_1
    }
#ifdef AOCL_LZMA_OPT
    { //optimized settings
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        p.reduceSize = ((UInt32)1 << 10);
        AOCL_LzmaEncProps_Normalize(&p);
        EXPECT_EQ(p.dictSize, kReduceMin); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_kReduceMin_common_2
    }
#endif
}

TEST_F(LZMA_encPropsNormalize, AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_userSettings_common) //userSettings
{
    { //reference settings
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        set_user_settings(&p);
        LzmaEncProps_Normalize(&p);
        validate_user_settings(&p); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_userSettings_common_1
    }
#ifdef AOCL_LZMA_OPT
    { //optimized settings
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        set_user_settings(&p);
        AOCL_LzmaEncProps_Normalize(&p);
        validate_user_settings(&p); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_userSettings_common_2
    }
#endif
}

TEST_F(LZMA_encPropsNormalize, AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_minDictSize_common) //minDictSize for hash chain dict
{
#ifdef AOCL_LZMA_OPT
    {
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        p.btMode = 0;
        p.dictSize = kHashGuarentee;
        p.level = HASH_CHAIN_16_LEVEL - 1;
        p.cacheEfficientStrategy = 1;
        AOCL_LzmaEncProps_Normalize(&p); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_minDictSize_common_1
        EXPECT_EQ(p.dictSize, kHashGuarentee * HASH_CHAIN_SLOT_SZ_8);
    }
    {
        CLzmaEncProps p;
        LzmaEncProps_Init(&p);
        p.btMode = 0;
        p.dictSize = kHashGuarentee;
        p.level = HASH_CHAIN_16_LEVEL;
        p.cacheEfficientStrategy = 1;
        AOCL_LzmaEncProps_Normalize(&p); //AOCL_Compression_lzma_AOCL_LzmaEncProps_Normalize_minDictSize_common_2
        EXPECT_EQ(p.dictSize, kHashGuarentee * HASH_CHAIN_SLOT_SZ_16);
    }
#endif
}
/*********************************************
 * End of LZMA_encPropsNormalize
 *********************************************/

/*********************************************
* Begin of LZMA_encPropsGetDictSize
*********************************************/
class LZMA_encPropsGetDictSize : public AOCL_setup_lzma {
};

TEST_F(LZMA_encPropsGetDictSize, AOCL_Compression_lzma_LzmaEncProps_GetDictSize_common_1)
{
    CLzmaEncProps p;
    LzmaEncProps_Init(&p);
    p.dictSize = ((UInt32)1 << 20);
    UInt32 dictSize = LzmaEncProps_GetDictSize(&p);
    EXPECT_EQ(p.dictSize, dictSize);
}
/*********************************************
* End of LZMA_encPropsGetDictSize
*********************************************/

/*********************************************
* Begin of LZMA_encCreate
*********************************************/
TEST(LZMA_encCreate, AOCL_Compression_lzma_LzmaEnc_Create_pass_common_1) //LzmaEnc create and destroy
{
    const ISzAlloc g_Alloc = { SzAlloc, SzFree };
    const ISzAlloc g_AllocBig = { SzAlloc, SzFree };
    CLzmaEncHandle p = LzmaEnc_Create(&g_Alloc);

    EXPECT_NE(p, nullptr);

    LzmaEnc_Destroy(p, &g_Alloc, &g_AllocBig);
}

TEST(LZMA_encCreate, AOCL_Compression_lzma_LzmaEnc_Create_fail_common_1) //LzmaEnc create with invalid allocator
{
    const ISzAlloc g_Alloc = { SzAllocDummy, SzFreeDummy };
    CLzmaEncHandle p = LzmaEnc_Create(&g_Alloc);

    EXPECT_EQ(p, nullptr);
}

TEST(LZMA_encCreate, AOCL_Compression_lzma_LzmaEnc_Create_nullAlloc_common_1) //LzmaEnc create with no allocator
{
    CLzmaEncHandle p = LzmaEnc_Create(NULL);

    EXPECT_EQ(p, nullptr);
}
/*********************************************
* End of LZMA_encCreate
*********************************************/

/*********************************************
* Begin of LZMA_encSetProps
*********************************************/
/*
    Fixture class for testing AOCL_LzmaEnc_SetProps
*/
class LZMA_encSetProps : public AOCL_setup_lzma
{
public:
    void SetUp() override {
        p = LzmaEnc_Create(&g_Alloc);
    }

    void TearDown() override
    {
        LzmaEnc_Destroy(p, &g_Alloc, &g_AllocBig);
    }

    CLzmaEncHandle p;
    CLzmaEncProps props;

private:
    const ISzAlloc g_Alloc = { SzAlloc, SzFree };
    const ISzAlloc g_AllocBig = { SzAlloc, SzFree };
};

TEST_F(LZMA_encSetProps, AOCL_Compression_lzma_AOCL_LzmaEnc_SetProps_valid_common_1)
{
    LzmaEncProps_Init(&props); //necessary, else props might have invalid values
    SRes res = Test_SetProps_Dyn(p, &props);
    EXPECT_EQ(res, SZ_OK);
    TestCLzmaEnc params = Get_CLzmaEnc_Params(p);

    //These params are same across reference and optimized versions
    EXPECT_EQ(params.numFastBytes, 32);
    EXPECT_EQ(params.lc, 3);
    EXPECT_EQ(params.lp, 0);
    EXPECT_EQ(params.pb, 2);
    EXPECT_EQ(params.fastMode, 0);
    EXPECT_EQ(params.writeEndMark, 0);
    EXPECT_EQ(params.btMode, 1);
    EXPECT_EQ(params.cutValue, 32);
    EXPECT_EQ(params.level, 5);
    EXPECT_EQ(params.numHashBytes, 4);
    
    //These params differ between reference and optimized versions
    //EXPECT_EQ(params.dictSize, 0);
    //EXPECT_EQ(params.cacheEfficientSearch, 0);
}

TEST_F(LZMA_encSetProps, AOCL_Compression_lzma_AOCL_LzmaEnc_SetProps_invalid_common_1)
{
    {
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.lc = 9;
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_ERROR_PARAM);
    }
    {
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.lp = 5;
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_ERROR_PARAM);
    }
    {
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.pb = 5;
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_ERROR_PARAM);
    }
    /*{
        const UInt32 kNumLogBits = (11 + sizeof(size_t) / 8 * 3);
        const UInt64 kDicLogSizeMaxCompress = (UInt64)1 << ((kNumLogBits - 1) * 2 + 7);
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.dictSize = kDicLogSizeMaxCompress + 1; //does not fit in UInt32. Test cannot pass.
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_ERROR_PARAM);
    }*/
}

TEST_F(LZMA_encSetProps, AOCL_Compression_lzma_AOCL_LzmaEnc_SetProps_fbLow_common_1) //test numFastBytes setting
{
    LzmaEncProps_Init(&props); //necessary, else props might have invalid values
    props.fb = 4; // fb < 5
    SRes res = Test_SetProps_Dyn(p, &props);
    EXPECT_EQ(res, SZ_OK);
    TestCLzmaEnc params = Get_CLzmaEnc_Params(p);
    EXPECT_EQ(params.numFastBytes, 5);
}

TEST_F(LZMA_encSetProps, AOCL_Compression_lzma_AOCL_LzmaEnc_SetProps_fbHigh_common_1) //test numFastBytes setting
{
    LzmaEncProps_Init(&props); //necessary, else props might have invalid values
    const unsigned c_LZMA_MATCH_LEN_MAX = 273; //#define LZMA_MATCH_LEN_MAX (LZMA_MATCH_LEN_MIN + kLenNumSymbolsTotal - 1) // max match len 273
    props.fb = c_LZMA_MATCH_LEN_MAX + 1; // fb > LZMA_MATCH_LEN_MAX
    SRes res = Test_SetProps_Dyn(p, &props);
    EXPECT_EQ(res, SZ_OK);
    TestCLzmaEnc params = Get_CLzmaEnc_Params(p);
    EXPECT_EQ(params.numFastBytes, c_LZMA_MATCH_LEN_MAX);
}

TEST_F(LZMA_encSetProps, AOCL_Compression_lzma_AOCL_LzmaEnc_SetProps_nhb_common_1) //test numHashBytes setting
{
    unsigned nhb_expected[7] = { 2,2,2,3,4,5,5 }; // SetProps should set nhb values to these
    for (unsigned nhb = 0; nhb < 7; ++nhb) {
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.btMode = 1; // btMode
        props.numHashBytes = nhb;
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_OK);
        TestCLzmaEnc params = Get_CLzmaEnc_Params(p);
        EXPECT_EQ(params.numHashBytes, nhb_expected[nhb]); //p->numHashBytes should be set to expected values
    }
}

TEST_F(LZMA_encSetProps, AOCL_Compression_lzma_AOCL_LzmaEnc_SetProps_ces_common_1) //test cacheEfficientSearch setting
{
    {
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.cacheEfficientStrategy = 0;
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_OK);
        TestCLzmaEnc params = Get_CLzmaEnc_Params(p);
        EXPECT_EQ(params.cacheEfficientSearch, 0);
    }
    {
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.cacheEfficientStrategy = 1;
        props.btMode = 1;
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_OK);
        TestCLzmaEnc params = Get_CLzmaEnc_Params(p);
        EXPECT_EQ(params.cacheEfficientSearch, 0);
    }
    {
        LzmaEncProps_Init(&props); //necessary, else props might have invalid values
        props.cacheEfficientStrategy = 1;
        props.btMode = 0;
        SRes res = Test_SetProps_Dyn(p, &props);
        EXPECT_EQ(res, SZ_OK);
        TestCLzmaEnc params = Get_CLzmaEnc_Params(p);
        EXPECT_EQ(params.cacheEfficientSearch, 1);
    }
}

/*********************************************
* End of LZMA_encSetProps
*********************************************/

/*********************************************
* Begin of LZMA_encSetDataSize
*********************************************/
TEST(LZMA_encSetDataSize, AOCL_Compression_lzma_LzmaEnc_SetDataSize_common_1)
{
    const ISzAlloc g_Alloc = { SzAlloc, SzFree };
    const ISzAlloc g_AllocBig = { SzAlloc, SzFree };
    CLzmaEncHandle p = LzmaEnc_Create(&g_Alloc);

    UInt64 expectedDataSize = (UInt32)1 << 10;
    UInt64 eds = Test_SetDataSize(p, expectedDataSize);
    EXPECT_EQ(eds, expectedDataSize);

    LzmaEnc_Destroy(p, &g_Alloc, &g_AllocBig);
}
/*********************************************
* End of LZMA_encSetDataSize
*********************************************/

/*********************************************
* Begin of LZMA_writeProperties
*********************************************/
/*
    Fixture class for testing LzmaEnc_WriteProperties
*/
class LZMA_writeProperties : public ::testing::Test
{
public:
    void SetUp() override {
        p = LzmaEnc_Create(&g_Alloc);
        ASSERT_NE(p, nullptr);
    }

    void TearDown() override
    {
        LzmaEnc_Destroy(p, &g_Alloc, &g_AllocBig);
    }

    CLzmaEncHandle p;

private:
    const ISzAlloc g_Alloc = { SzAlloc, SzFree };
    const ISzAlloc g_AllocBig = { SzAlloc, SzFree };
};

TEST_F(LZMA_writeProperties, AOCL_Compression_lzma_LzmaEnc_WriteProperties_valid_common_1) //set value derived from pb,lp,lc in props
{
    Byte props[LZMA_PROPS_SIZE];
    SizeT size = LZMA_PROPS_SIZE;
    SRes res = LzmaEnc_WriteProperties(p, props, &size);
    EXPECT_EQ(res, SZ_OK);
    EXPECT_EQ(props[0], 0x5d); //(Byte)((p->pb * 5 + p->lp) * 9 + p->lc);
    EXPECT_EQ(*((UInt32*)(props + 1)), ((UInt32)1 << 24));
}

TEST_F(LZMA_writeProperties, AOCL_Compression_lzma_LzmaEnc_WriteProperties_invalidSz_common_1)
{
    Byte props[LZMA_PROPS_SIZE];
    SizeT size = 4; //invalid size
    SRes res = LzmaEnc_WriteProperties(p, props, &size);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_writeProperties, AOCL_Compression_lzma_LzmaEnc_WriteProperties_nullCLzmaEncHandle_common_1)
{
    Byte props[LZMA_PROPS_SIZE];
    SizeT size = LZMA_PROPS_SIZE;
    SRes res = LzmaEnc_WriteProperties(NULL, props, &size);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_writeProperties, AOCL_Compression_lzma_LzmaEnc_WriteProperties_nullProps_common_1)
{
    SizeT size = LZMA_PROPS_SIZE;
    SRes res = LzmaEnc_WriteProperties(p, NULL, &size);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_writeProperties, AOCL_Compression_lzma_LzmaEnc_WriteProperties_nullpropsSz_common_1)
{
    Byte props[LZMA_PROPS_SIZE];
    SRes res = LzmaEnc_WriteProperties(p, props, NULL);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_writeProperties, AOCL_Compression_lzma_LzmaEnc_WriteProperties_dictSize_common) //adjust and set dictSize in props 
{
    Byte props[LZMA_PROPS_SIZE];
    SizeT size = LZMA_PROPS_SIZE;
    {
        UInt32 dictSize = ((UInt32)1 << 21);
        SRes res = Test_WriteProperties(p, props, &size, dictSize);
        EXPECT_EQ(res, SZ_OK);
        EXPECT_EQ(*((UInt32*)(props + 1)), ((UInt32)1 << 21)); //AOCL_Compression_lzma_writeProperties_dictSize_common_1
    }
    {
        UInt32 dictSize = ((UInt32)1 << 21) - 1;
        SRes res = Test_WriteProperties(p, props, &size, dictSize);
        EXPECT_EQ(res, SZ_OK);
        EXPECT_EQ(*((UInt32*)(props + 1)), ((UInt32)1 << 21)); //AOCL_Compression_lzma_writeProperties_dictSize_common_2
    }
}
/*********************************************
* End of LZMA_writeProperties
*********************************************/

/*********************************************
* Begin of LZMA_isWriteEndMark
*********************************************/
TEST(LZMA_isWriteEndMark, AOCL_Compression_lzma_LzmaEnc_IsWriteEndMark_common_1)
{
    const ISzAlloc g_Alloc = { SzAlloc, SzFree };
    const ISzAlloc g_AllocBig = { SzAlloc, SzFree };
    CLzmaEncHandle p = LzmaEnc_Create(&g_Alloc);

    unsigned wemSet = 32;
    unsigned wem = Test_IsWriteEndMark(p, wemSet);
    EXPECT_EQ(wem, wemSet);

    LzmaEnc_Destroy(p, &g_Alloc, &g_AllocBig);
}
/*********************************************
* End of LZMA_isWriteEndMark
*********************************************/

/*
    Base class for allocating and filling up buffers used for compression/decompression tests
*/
class LZMA_buffersBase {
public:
    const ISzAlloc g_Alloc = { SzAlloc, SzFree };
    const ISzAlloc g_AllocBig = { SzAlloc, SzFree };

protected:
    void buffers_setup()
    {
        //allocate buffers
        outSize = compression_bound(inSize);
        inPtr = (char*)allocMem(inSize, 1);
        compPtr = (char*)allocMem(outSize, 0);
        decompPtr = (char*)allocMem(inSize, 0);
        ASSERT_NE(inPtr, nullptr);
        ASSERT_NE(compPtr, nullptr);
        ASSERT_NE(decompPtr, nullptr);

        //load input buffer
        memset(inPtr, 0, inSize);
        size_t cur = 0;
        while (cur < inSize) { //fill buffer with repeating patterns
            int randId = rand() % randomStrs.size(); //pick a string at random
            int randLen = rand() % randomStrs[randId].size(); //select sub string length
            if (cur + randLen >= inSize) break;

            memcpy(inPtr + cur, randomStrs[randId].c_str(), randLen * sizeof(char));
            cur += randLen;
        }
    }

    void* allocMem(size_t size, ptrdiff_t zeroInit)
    {
        void* bufPtr = (zeroInit == 0) ? calloc(1, size) : malloc(size);
        return bufPtr;
    }

    size_t compression_bound(size_t inSize)
    {
        size_t outSize = (inSize + (inSize / 6) + MIN_PAD_SIZE);
        return outSize;
    }

    std::vector<std::string> randomStrs = { "qwertyuiop", "asdfghjkl", "zxcvbnm", "1234567890",
        "QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM", "!@#$%^&*()" };

    char* inPtr = nullptr, * compPtr = nullptr, * decompPtr = nullptr;
    size_t inSize = 0;
    size_t outSize = 0;
    SizeT outLen;
};

/*********************************************
* Begin of LZMA_memEncode
*********************************************/
/*
    Base class for all fixtures that test/use lzma encoder
*/
class LZMA_encodeBase : public LZMA_buffersBase, public ::testing::TestWithParam<size_t> {
public:
    void decomp_validate(size_t resultComp) {
        //compress
        EXPECT_GT(resultComp, 0);

        //decompress
        int64_t resultDecomp = decompress(compPtr, resultComp, decompPtr, inSize);
        EXPECT_GT(resultDecomp, 0);

        //validate
        EXPECT_EQ(memcmp(inPtr, decompPtr, inSize), 0); //decompressed data must match input
    }

protected:
    void SetUpEncode(size_t _inSize) {
        aocl_setup_lzma_test();
        inSize = _inSize;
        //setup buffers
        buffers_setup();
        headerSize = LZMA_PROPS_SIZE;
    }

    void TearDownEncode() {
        //free buffers
        if (inPtr) free(inPtr);
        if (compPtr) free(compPtr);
        if (decompPtr) free(decompPtr);
    }

    CLzmaEncProps props;
    size_t headerSize;

private:
    int64_t decompress(char* inPtr, size_t inSize, char* outbuf, size_t outSize) {
        ptrdiff_t res;
        SizeT outLen = outSize;
        SizeT srcLen = inSize - LZMA_PROPS_SIZE;
        ELzmaStatus status;

        res = LzmaDecode((uint8_t*)outbuf, &outLen, (uint8_t*)inPtr + LZMA_PROPS_SIZE,
            &srcLen, (uint8_t*)inPtr, LZMA_PROPS_SIZE, LZMA_FINISH_END,
            &status, &g_Alloc);
        if (res != SZ_OK)
            return 0;

        return outLen;
    }
};

/*
    Fixture class for testing LZMA_memEncode
    LZMA_memEncode API is used for in-memory encoding
*/
class LZMA_memEncode : public LZMA_encodeBase
{
public:
    void SetUp() override {
        size_t inSize = GetParam();
        SetUpEncode(inSize);
    }

    void TearDown() override
    {
        TearDownEncode();
    }

    void setup_encoder() {
        outLen = outSize - LZMA_PROPS_SIZE;
        LzmaEncProps_Init(&props);
        p = LzmaEnc_Create(&g_Alloc);
        ASSERT_NE(p, nullptr);
    }

    void free_encoder() {
        LzmaEnc_Destroy(p, &g_Alloc, &g_AllocBig);
    }

    void set_properties() {
        LzmaEncProps_Init(&props); //necessary, else props will have invalid values
#ifdef AOCL_LZMA_OPT
        props.srcLen = GetParam();
#endif
        SRes res = Test_SetProps_Dyn(p, &props);
        ASSERT_EQ(res, SZ_OK);

        //write header
        res = LzmaEnc_WriteProperties(p, (uint8_t*)compPtr, &headerSize);
        ASSERT_EQ(res, SZ_OK);
    }

    CLzmaEncHandle p = nullptr;
};

TEST_P(LZMA_memEncode, AOCL_Compression_lzma_LzmaEnc_MemEncode_common)
{
    //Test for all levels
    for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level) { //AOCL_Compression_lzma_LzmaEnc_MemEncode_common_1 - 10
        //setup
        setup_encoder();
        props.level = level;
        set_properties();

        //compress using LzmaEnc_MemEncode
        SRes res = LzmaEnc_MemEncode(p, (uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, (uint8_t*)inPtr, inSize,
            0, NULL, &g_Alloc, &g_AllocBig);
        EXPECT_EQ(res, SZ_OK);

        //decompress and validate
        decomp_validate(LZMA_PROPS_SIZE + outLen);

        //free
        free_encoder();
    }
}

/*
* Run all LZMA_memEncode tests for 2 settings:
*   + Not USE_CACHE_EFFICIENT_HASH_CHAIN: expectedDataSize < MAX_SIZE_FOR_CE_HC_OFF
*   + USE_CACHE_EFFICIENT_HASH_CHAIN: expectedDataSize >= MIN_SIZE_FOR_CE_HC_ON
*/
INSTANTIATE_TEST_SUITE_P(
    LZMA_encode,
    LZMA_memEncode,
    ::testing::Values((size_t)(MAX_SIZE_FOR_CE_HC_OFF-1), (size_t)(MIN_SIZE_FOR_CE_HC_ON)));
/*********************************************
* End of LZMA_memEncode
*********************************************/

/*********************************************
* Begin of LZMA_encodeFile
*********************************************/
/*
    Fixture class for testing LZMA_encodeFile
    LZMA_encodeFile is used for file encoding
*/
class LZMA_encodeFile : public LZMA_encodeBase
{
public:
    void SetUp() override {
        size_t inSize = GetParam();
        SetUpEncode(inSize);
    }

    void TearDown() override
    {
        TearDownEncode();
    }

    void init() {
        outLen = outSize - LZMA_PROPS_SIZE;
        LzmaEncProps_Init(&props); //necessary, else props will have invalid values
    }

    void execute() {
        //compress using LzmaEncode
        SRes res = LzmaEncode((uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, (uint8_t*)inPtr, inSize,
            &props, (uint8_t*)compPtr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
        EXPECT_EQ(res, SZ_OK);

        //decompress and validate
        decomp_validate(LZMA_PROPS_SIZE + outLen);
    }

    void execute_invalid() { //expect to return encode failed
        //compress using LzmaEncode
        SRes res = LzmaEncode((uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, (uint8_t*)inPtr, inSize,
            &props, (uint8_t*)compPtr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
        EXPECT_NE(res, SZ_OK);
    }
};

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_valid_common)
{
    //Test for all levels
    for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level) {
        //setup
        init();
        props.level = level;

        execute(); //AOCL_Compression_lzma_LzmaEncode_valid_common_1 - 10
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_minDict_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 1;
    props.dictSize = (unsigned)1 << 12;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_maxDict_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 9;
    props.dictSize = (unsigned)3 << 29; //64-bit version

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_2ByteHashHc_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 1;
    props.numHashBytes = 2;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_2ByteHashBt_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 9;
    props.numHashBytes = 2;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_3ByteHashHc_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 1;
    props.numHashBytes = 3;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_3ByteHashBt_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 9;
    props.numHashBytes = 3;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_4ByteHashHc_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 1;
    props.numHashBytes = 4;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_4ByteHashBt_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 9;
    props.numHashBytes = 4;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_5ByteHashHc_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 1;
    props.numHashBytes = 5;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_5ByteHashBt_common_1)
{
    //for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level)
    //setup
    init();
    props.level = 9;
    props.numHashBytes = 5;

    execute();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_lc_common)
{
    for (int lc = -1; lc <= 8; ++lc) { /* 0 <= lc <= 8, default = 3 */
        //setup
        init();
        props.level = 1;
        props.lc = lc;

        execute(); //AOCL_Compression_lzma_LzmaEncode_lc_common_1 - 10
    }
    {
        init();
        props.level = 1;
        props.lc = 9; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_lc_common_11
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_lp_common)
{
    for (int lp = -1; lp <= 4; ++lp) { /* 0 <= lp <= 4, default = 0 */
        //setup
        init();
        props.level = 1;
        props.lp = lp;

        execute(); //AOCL_Compression_lzma_LzmaEncode_lp_common_1 - 6
    }
    {
        init();
        props.level = 1;
        props.lp = 5; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_lp_common_7
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_pb_common)
{
    for (int pb = -1; pb <= 4; ++pb) { /* 0 <= pb <= 4, default = 2 */
        //setup
        init();
        props.level = 1;
        props.pb = pb;

        execute(); //AOCL_Compression_lzma_LzmaEncode_pb_common_1 - 6
    }
    {
        init();
        props.level = 1;
        props.pb = 5; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_pb_common_7
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_algo_common)
{
    for (int algo = -1; algo <= 1;++algo) { /* 0 - fast, 1 - normal, default = 1 */
        //setup
        init();
        props.algo = algo;

        execute(); //AOCL_Compression_lzma_LzmaEncode_algo_common_1 - 3
    }
    {
        init();
        props.algo = 2; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_algo_common_4
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_btMode_common)
{
    for (int btMode = -1; btMode <= 1; ++btMode) { /* 0 - hashChain Mode, 1 - binTree mode - normal, default = 1 */
        //setup
        init();
        props.btMode = btMode;

        execute(); //AOCL_Compression_lzma_LzmaEncode_btMode_common_1 - 3
    }
    {
        init();
        props.btMode = 2; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_btMode_common_4
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_invalidNumHashBytes_common_1)
{
    //setup
    init();
    props.numHashBytes = 6; //out of bound

    execute_invalid();
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_fb_common)
{
    /* 5 <= fb <= 273, default = 32 */
    {
        init();
        props.level = 1;
        props.fb = -1; //auto

        execute(); //AOCL_Compression_lzma_LzmaEncode_fb_common_1
    }
    { 
        init();
        props.level = 1;
        props.fb = 5; //min

        execute(); //AOCL_Compression_lzma_LzmaEncode_fb_common_2
    }
    {
        init();
        props.level = 1;
        props.fb = 139; //mid

        execute(); //AOCL_Compression_lzma_LzmaEncode_fb_common_3
    }
    {
        init();
        props.level = 1;
        props.fb = 273; //max

        execute(); //AOCL_Compression_lzma_LzmaEncode_fb_common_4
    }
    {
        init();
        props.level = 1;
        props.fb = 4; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_fb_common_5
    }
    {
        init();
        props.level = 1;
        props.fb = 274; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_fb_common_6
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_mc_common)
{
    /* 1 <= mc <= (1 << 30), default = 32 */
    {
        init();
        props.level = 1;
        props.mc = 0; //auto

        execute(); //AOCL_Compression_lzma_LzmaEncode_mc_common_1
    }
    {
        init();
        props.level = 1;
        props.mc = 1; //min

        execute(); //AOCL_Compression_lzma_LzmaEncode_mc_common_2
    }
    {
        init();
        props.level = 1;
        props.mc = ((UInt32)1 << 15); //mid

        execute(); //AOCL_Compression_lzma_LzmaEncode_mc_common_3
    }
    {
        init();
        props.level = 1;
        props.mc = ((UInt32)1 << 30); //max

        execute(); //AOCL_Compression_lzma_LzmaEncode_mc_common_4
    }
    {
        init();
        props.level = 1;
        props.mc = ((UInt32)1 << 30) + 1; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_mc_common_5
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_writeEndMark_common)
{
    for (int wem = 0; wem <= 1; ++wem) { /* 0 - do not write EOPM, 1 - write EOPM, default = 0 */
        //setup
        init();
        props.writeEndMark = wem;

        execute(); //AOCL_Compression_lzma_LzmaEncode_writeEndMark_common_1 - 2
    }
    {
        init();
        props.writeEndMark = 2; //out of bound

        execute_invalid(); //AOCL_Compression_lzma_LzmaEncode_writeEndMark_common_3
    }
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_noSrc_common_1)
{
    //input buffer: src: = nullptr
    SRes res = LzmaEncode((uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, nullptr, inSize,
        &props, (uint8_t*)compPtr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
    EXPECT_NE(res, SZ_OK);
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_noSrcSz_common_1)
{
    SizeT invalidInSize = 0; //srcLen empty
    SRes res = LzmaEncode((uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, (uint8_t*)inPtr, invalidInSize,
        &props, (uint8_t*)compPtr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
    EXPECT_NE(res, SZ_OK);
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_noDst_common_1)
{
    //output buffer: dest = nullptr
    SRes res = LzmaEncode(nullptr, &outLen, (uint8_t*)inPtr, inSize,
        &props, (uint8_t*)compPtr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
    EXPECT_NE(res, SZ_OK);
}

TEST_P(LZMA_encodeFile, AOCL_Compression_lzma_LzmaEncode_noHeader_common_1)
{
    //header buffer: propsEncoded = nullptr
    SRes res = LzmaEncode((uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, (uint8_t*)inPtr, inSize,
        &props, nullptr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
    EXPECT_NE(res, SZ_OK);
}

/*
* Run all LZMA_encodeFile tests for 2 settings:
*   + Not USE_CACHE_EFFICIENT_HASH_CHAIN: expectedDataSize < MAX_SIZE_FOR_CE_HC_OFF
*   + USE_CACHE_EFFICIENT_HASH_CHAIN: expectedDataSize >= MIN_SIZE_FOR_CE_HC_ON
*/
INSTANTIATE_TEST_SUITE_P(
    LZMA_encode,
    LZMA_encodeFile,
    ::testing::Values((size_t)(MAX_SIZE_FOR_CE_HC_OFF-1), (size_t)(MIN_SIZE_FOR_CE_HC_ON)));

/*********************************************
* End of LZMA_encodeFile
*********************************************/

/*********************************************
 * End API tests for LzmaEnc
 *********************************************/

/*********************************************
 * Begin API tests for LzmaDec
 *********************************************/

/*********************************************
* Begin of LZMA_decInit
*********************************************/
#define kMatchSpecLenStartTest 274
TEST(LZMA_decInit, AOCL_Compression_lzma_LzmaDec_Init_common_1) //check initialization is valid
{
    CLzmaDec p;
    LzmaDec_Init(&p);
    EXPECT_EQ(p.dicPos, 0);
    EXPECT_EQ(p.tempBufSize, 0);
    EXPECT_EQ(p.processedPos, 0);
    EXPECT_EQ(p.checkDicSize, 0);
    EXPECT_EQ(p.remainLen, kMatchSpecLenStartTest + 2);
}
/*********************************************
 * End of LZMA_decInit
 *********************************************/

/*********************************************
* Begin of LZMA_allocFreeBase
*********************************************/
#define LZMA_DIC_MIN (1 << 12)
#define NUM_PROBS_DEFAULT 8128
/*
    Base class for alloc free and props set APIs
*/
class LZMA_allocFreeBase : public AOCL_setup_lzma {
protected:
    void SetUpBase() {
        pe = LzmaEnc_Create(&g_Alloc); //create encoder
        ASSERT_NE(pe, nullptr);

        LzmaDec_Construct(&pd); //create decoder
    }

    void TearDownBase() {
        LzmaEnc_Destroy(pe, &g_Alloc, &g_AllocBig);
    }

public:
    void write_properties() { //write header to props
        SizeT size = LZMA_PROPS_SIZE;
        SRes resw = LzmaEnc_WriteProperties(pe, props, &size);
        ASSERT_EQ(resw, SZ_OK);
    }

    void write_default_properties() { //set defaults then write to props
        CLzmaEncProps propsEnc;
        LzmaEncProps_Init(&propsEnc);
        SRes reswd = Test_SetProps_Dyn(pe, &propsEnc);
        ASSERT_EQ(reswd, SZ_OK);
        write_properties(); //write header to props
    }

    void validate_props(int lc, int lp, int pb, UInt32 dicSize) {
        EXPECT_EQ((int)pd.prop.lc, lc);
        EXPECT_EQ((int)pd.prop.lp, lp);
        EXPECT_EQ((int)pd.prop.pb, pb);
        EXPECT_EQ(pd.prop.dicSize, dicSize);
    }

    CLzmaEncHandle pe;
    CLzmaDec pd;

    Byte props[LZMA_PROPS_SIZE];

    const ISzAlloc g_Alloc = { SzAlloc, SzFree };
    const ISzAlloc g_AllocBig = { SzAlloc, SzFree };
};
/*********************************************
* "End" of LZMA_allocFreeBase
*********************************************/

/*********************************************
* Begin of LZMA_propsDecode
*********************************************/
/*
    Fixture class for testing LzmaProps_Decode
*/
class LZMA_propsDecode : public LZMA_allocFreeBase {
public:
    void SetUp() override {
        SetUpBase();
    }

    void TearDown() override
    {
        TearDownBase();
    }

    CLzmaProps propNew;
};

TEST_F(LZMA_propsDecode, AOCL_Compression_lzma_LzmaProps_Decode_defaults_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    SRes res = LzmaProps_Decode(&propNew, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    pd.prop = propNew;
    validate_props(3, 0, 2, ((UInt32)1 << 24));
}

TEST_F(LZMA_propsDecode, AOCL_Compression_lzma_LzmaProps_Decode_user_common_1)
{
    //set user defined properties
    const int lc = 4, lp = 1, pb = 2;
    const UInt32 dicSize = ((UInt32)1 << 21);
    CLzmaEncProps propsEnc;
    LzmaEncProps_Init(&propsEnc);
    propsEnc.lc = lc;
    propsEnc.lp = lp;
    propsEnc.pb = pb;
    propsEnc.dictSize = dicSize;
    SRes res = Test_SetProps_Dyn(pe, &propsEnc);
    ASSERT_EQ(res, SZ_OK);

    write_properties(); //write properties into props

    //set pd entries by reading bytes in props
    res = LzmaProps_Decode(&propNew, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    pd.prop = propNew;
    validate_props(lc, lp, pb, dicSize);
}

TEST_F(LZMA_propsDecode, AOCL_Compression_lzma_LzmaProps_Decode_minDict_common_1)
{
    //set user defined properties
    const int lc = 3, lp = 0, pb = 2;
    const UInt32 dicSize = ((UInt32)1 << 10); //set to < LZMA_DIC_MIN
    CLzmaEncProps propsEnc;
    LzmaEncProps_Init(&propsEnc);
    propsEnc.lc = lc;
    propsEnc.lp = lp;
    propsEnc.pb = pb;
    propsEnc.dictSize = dicSize;
    SRes res = Test_SetProps_Dyn(pe, &propsEnc);
    ASSERT_EQ(res, SZ_OK);

    write_properties(); //write properties into props

    //set pd entries by reading bytes in props
    res = LzmaProps_Decode(&propNew, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    pd.prop = propNew;
    validate_props(lc, lp, pb, LZMA_DIC_MIN);
}

TEST_F(LZMA_propsDecode, AOCL_Compression_lzma_LzmaProps_Decode_invalidSz_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    SRes res = LzmaProps_Decode(&propNew, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE - 1); //size < LZMA_PROPS_SIZE

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_ERROR_UNSUPPORTED);
}

TEST_F(LZMA_propsDecode, AOCL_Compression_lzma_LzmaProps_Decode_invalidParam_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    props[0] = (Byte)(9 * 5 * 5);
    SRes res = LzmaProps_Decode(&propNew, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_ERROR_UNSUPPORTED);
}
/*********************************************
 * End of LZMA_propsDecode
 *********************************************/

 /*********************************************
 * Begin of LZMA_allocFreeProbs
 *********************************************/
/*
    Fixture class for testing LzmaDec_AllocateProbs, LzmaDec_FreeProbs
*/
class LZMA_allocFreeProbs : public LZMA_allocFreeBase {
public:
    void SetUp() override {
        SetUpBase();
    }

    void TearDown() override
    {
        TearDownBase();
    }

    void validate_props_probs(int lc, int lp, int pb, UInt32 dicSize, UInt32 numProbs) {
        validate_props(lc, lp, pb, dicSize);
        EXPECT_EQ(pd.numProbs, numProbs);
    }

    void validate_free(CLzmaDec* p) {
        EXPECT_EQ((p->probs == NULL), true);
    }
};

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_defaults_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    SRes res = LzmaDec_AllocateProbs(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_probs(3, 0, 2, ((UInt32)1 << 24), NUM_PROBS_DEFAULT);

    LzmaDec_FreeProbs(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_user_common_1)
{
    //set user defined properties
    const int lc = 4, lp = 1, pb = 2;
    const UInt32 dicSize = ((UInt32)1 << 21);
    CLzmaEncProps propsEnc;
    LzmaEncProps_Init(&propsEnc);
    propsEnc.lc = lc;
    propsEnc.lp = lp;
    propsEnc.pb = pb;
    propsEnc.dictSize = dicSize;
    SRes res = Test_SetProps_Dyn(pe, &propsEnc);
    ASSERT_EQ(res, SZ_OK);

    write_properties(); //write properties into props

    //set pd entries by reading bytes in props
    res = LzmaDec_AllocateProbs(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_probs(lc, lp, pb, dicSize, 26560);

    LzmaDec_FreeProbs(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_minDict_common_1)
{
    //set user defined properties
    const int lc = 3, lp = 0, pb = 2;
    const UInt32 dicSize = ((UInt32)1 << 10);
    CLzmaEncProps propsEnc;
    LzmaEncProps_Init(&propsEnc);
    propsEnc.lc = lc;
    propsEnc.lp = lp;
    propsEnc.pb = pb;
    propsEnc.dictSize = dicSize;
    SRes res = Test_SetProps_Dyn(pe, &propsEnc);
    ASSERT_EQ(res, SZ_OK);

    write_properties(); //write properties into props

    //set pd entries by reading bytes in props
    res = LzmaDec_AllocateProbs(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_probs(lc, lp, pb, LZMA_DIC_MIN, NUM_PROBS_DEFAULT);

    LzmaDec_FreeProbs(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_invalidSz_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    SRes res = LzmaDec_AllocateProbs(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE - 1, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_ERROR_UNSUPPORTED);

    LzmaDec_FreeProbs(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_invalidParam_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    props[0] = (Byte)(9 * 5 * 5);
    SRes res = LzmaDec_AllocateProbs(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_ERROR_UNSUPPORTED);

    LzmaDec_FreeProbs(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_nullCLzmaDec_common_1)
{
    write_default_properties(); //write default properties into props

    SRes res = LzmaDec_AllocateProbs(NULL, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_nullProps_common_1)
{
    write_default_properties(); //write default properties into props

    SRes res = LzmaDec_AllocateProbs(&pd, NULL, LZMA_PROPS_SIZE, &g_Alloc);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_allocFreeProbs, AOCL_Compression_lzma_LzmaDec_AllocateProbs_nullAlloc_common_1)
{
    write_default_properties(); //write default properties into props

    SRes res = LzmaDec_AllocateProbs(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, NULL);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

/*********************************************
 * End of LZMA_allocFreeProbs
 *********************************************/


 /*********************************************
 * Begin of LZMA_allocFree
 *********************************************/
/*
    Fixture class for testing LzmaDec_Allocate, LzmaDec_Free
*/
class LZMA_allocFree : public LZMA_allocFreeBase {
public:
    void SetUp() override {
        SetUpBase();
    }

    void TearDown() override
    {
        TearDownBase();
    }

    void validate_props_buf(int lc, int lp, int pb, UInt32 dicSize, UInt32 dicBufSize, UInt32 numProbs) {
        validate_props(lc, lp, pb, dicSize);
        EXPECT_EQ(pd.numProbs, numProbs);
        EXPECT_EQ(pd.dicBufSize, dicBufSize);
    }

    void validate_free(CLzmaDec* p) {
        EXPECT_EQ((p->dic == NULL), true);
        EXPECT_EQ((p->probs == NULL), true);
    }
};

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_defaults_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    SRes res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_buf(3, 0, 2, ((UInt32)1 << 24), ((UInt32)1 << 24), NUM_PROBS_DEFAULT);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_user_common_1)
{
    //set user defined properties
    const int lc = 4, lp = 1, pb = 2;
    const UInt32 dicSize = ((UInt32)1 << 21);
    CLzmaEncProps propsEnc;
    LzmaEncProps_Init(&propsEnc);
    propsEnc.lc = lc;
    propsEnc.lp = lp;
    propsEnc.pb = pb;
    propsEnc.dictSize = dicSize;
    SRes res = Test_SetProps_Dyn(pe, &propsEnc);
    ASSERT_EQ(res, SZ_OK);

    write_properties(); //write properties into props

    //set pd entries by reading bytes in props
    res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_buf(lc, lp, pb, dicSize, dicSize, 26560);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_minDict_common_1)
{
    //set user defined properties
    const int lc = 3, lp = 0, pb = 2;
    const UInt32 dicSize = ((UInt32)1 << 10);
    CLzmaEncProps propsEnc;
    LzmaEncProps_Init(&propsEnc);
    propsEnc.lc = lc;
    propsEnc.lp = lp;
    propsEnc.pb = pb;
    propsEnc.dictSize = dicSize;
    SRes res = Test_SetProps_Dyn(pe, &propsEnc);
    ASSERT_EQ(res, SZ_OK);

    write_properties(); //write properties into props

    //set pd entries by reading bytes in props
    res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_buf(lc, lp, pb, LZMA_DIC_MIN, LZMA_DIC_MIN, NUM_PROBS_DEFAULT);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_invalidSz_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    SRes res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE - 1, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_ERROR_UNSUPPORTED);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_invalidParam_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    props[0] = (Byte)(9 * 5 * 5);
    SRes res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_ERROR_UNSUPPORTED);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_dicFree_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    free(pd.dic); //release dic here
    SRes res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_buf(3, 0, 2, ((UInt32)1 << 24), ((UInt32)1 << 24), NUM_PROBS_DEFAULT);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_dicBufAlter_common_1)
{
    write_default_properties(); //write default properties into props

    //set pd entries by reading bytes in props
    pd.dicBufSize = ((UInt32)1 << 20); //alter size
    SRes res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_buf(3, 0, 2, ((UInt32)1 << 24), ((UInt32)1 << 24), NUM_PROBS_DEFAULT);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_highDictSz_common_1)
{
    //set user defined properties
    const int lc = 3, lp = 0, pb = 2;
    const UInt32 dicSize = ((UInt32)1 << 30); //large dict size
    CLzmaEncProps propsEnc;
    LzmaEncProps_Init(&propsEnc);
    propsEnc.lc = lc;
    propsEnc.lp = lp;
    propsEnc.pb = pb;
    propsEnc.dictSize = dicSize;
    SRes res = Test_SetProps_Dyn(pe, &propsEnc);
    ASSERT_EQ(res, SZ_OK);

    write_properties(); //write properties into props

    //set pd entries by reading bytes in props
    res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);

    //validate properties are set correctly
    EXPECT_EQ(res, SZ_OK);
    validate_props_buf(lc, lp, pb, dicSize, dicSize, NUM_PROBS_DEFAULT);

    LzmaDec_Free(&pd, &g_Alloc);
    validate_free(&pd);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_nullCLzmaDec_common_1)
{
    write_default_properties(); //write default properties into props

    SRes res = LzmaDec_Allocate(NULL, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, &g_Alloc);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_nullProps_common_1)
{
    write_default_properties(); //write default properties into props

    SRes res = LzmaDec_Allocate(&pd, NULL, LZMA_PROPS_SIZE, &g_Alloc);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}

TEST_F(LZMA_allocFree, AOCL_Compression_lzma_LzmaDec_Allocate_nullAlloc_common_1)
{
    write_default_properties(); //write default properties into props

    SRes res = LzmaDec_Allocate(&pd, (uint8_t*)(&props[0]), LZMA_PROPS_SIZE, NULL);
    EXPECT_EQ(res, SZ_ERROR_PARAM);
}
/*********************************************
 * End of LZMA_allocFree
 *********************************************/

/*********************************************
 * Begin of LZMA_decodeBase
 *********************************************/
/*
    Fixture class for testing
*/
class LZMA_decodeBase : public LZMA_buffersBase, public AOCL_setup_lzma {
public:
    void validate() {
        EXPECT_EQ(memcmp(inPtr, decompPtr, inSize), 0); //decompressed data must match input
    }

protected:
    void SetUpBuffers() {
        //setup buffers
        inSize = 1024 * 1024; //1MB
        buffers_setup();
        headerSize = LZMA_PROPS_SIZE;
    }

    void TearDownBuffers() {
        //free buffers
        if (inPtr) free(inPtr);
        if (compPtr) free(compPtr);
        if (decompPtr) free(decompPtr);
    }

    CLzmaEncProps props;
    size_t headerSize;
};
/*********************************************
 * End of LZMA_decodeBase
 *********************************************/

 /*********************************************
 * Begin of LZMA_decodeDict
 *********************************************/
#define RC_INIT_SIZE 5 //skip 1st byte. use next 4 for code initialization
/*
    Fixture class for testing LzmaDec_DecodeToDic
*/
class LZMA_decodeDict : public LZMA_decodeBase
{
public:
    enum class NullCase {
        DEC_PTR, SRC, SRC_LEN, STATUS
    };

    void SetUp() override {
        SetUpBuffers();
    }

    void TearDown() override
    {
        TearDownBuffers();
    }

    void encode() {
        outLen = outSize - LZMA_PROPS_SIZE;
        LzmaEncProps_Init(&props); //necessary, else props will have invalid values

        //compress using LzmaEncode
        SRes res = LzmaEncode((uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, (uint8_t*)inPtr, inSize,
            &props, (uint8_t*)compPtr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
        ASSERT_EQ(res, SZ_OK);
    }

    void decode(Byte* dest, SizeT* destLen, const Byte* src, SizeT* srcLen,
        const Byte* propData, unsigned propSize) {
        CLzmaDec p;
        SizeT outSize = *destLen, inSize = *srcLen;
        *destLen = *srcLen = 0;
        ELzmaStatus status = LZMA_STATUS_NOT_SPECIFIED;
        EXPECT_GE(inSize, RC_INIT_SIZE);

        LzmaDec_Construct(&p);
        EXPECT_EQ(LzmaDec_AllocateProbs(&p, propData, propSize, &g_Alloc), SZ_OK);

        p.dic = dest;
        p.dicBufSize = outSize;
        LzmaDec_Init(&p);
        *srcLen = inSize;
        SRes res = LzmaDec_DecodeToDic(&p, outSize, src, srcLen, LZMA_FINISH_END, &status);
        *destLen = p.dicPos;
        EXPECT_EQ(res, SZ_OK);
        EXPECT_EQ(status, LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK);

        LzmaDec_FreeProbs(&p, &g_Alloc);
    }

    void decode_invalid_init(Byte* dest, SizeT* destLen, const Byte* src, SizeT* srcLen,
        const Byte* propData, unsigned propSize) {
        CLzmaDec p;
        SizeT outSize = *destLen, inSize = *srcLen;
        *destLen = *srcLen = 0;
        ELzmaStatus status = LZMA_STATUS_NOT_SPECIFIED;
        EXPECT_GE(inSize, RC_INIT_SIZE);

        LzmaDec_Construct(&p);
        EXPECT_EQ(LzmaDec_AllocateProbs(&p, propData, propSize, &g_Alloc), SZ_OK);

        p.dic = dest;
        p.dicBufSize = outSize;
        //LzmaDec_Init(&p); //skip init. Set invalid values
        p.dicPos = 0;
        p.processedPos = 0;
        p.checkDicSize = 0;
        p.tempBufSize = 0;
        p.remainLen = kMatchSpecLenStartTest + 3;

        *srcLen = inSize;
        SRes res = LzmaDec_DecodeToDic(&p, outSize, src, srcLen, LZMA_FINISH_END, &status);
        *destLen = p.dicPos;
        EXPECT_NE(res, SZ_OK); //expect error code

        LzmaDec_FreeProbs(&p, &g_Alloc);
    }

    void decode_null_checks(Byte* dest, SizeT* destLen, const Byte* src, SizeT* srcLen,
        const Byte* propData, unsigned propSize, NullCase nullCase) {
        CLzmaDec p;
        SizeT outSize = *destLen, inSize = *srcLen;
        *destLen = *srcLen = 0;
        ELzmaStatus status = LZMA_STATUS_NOT_SPECIFIED;
        EXPECT_GE(inSize, RC_INIT_SIZE);

        LzmaDec_Construct(&p);
        EXPECT_EQ(LzmaDec_AllocateProbs(&p, propData, propSize, &g_Alloc), SZ_OK);

        p.dic = dest;
        p.dicBufSize = outSize;
        LzmaDec_Init(&p);
        *srcLen = inSize;
        SRes res;
        switch (nullCase) { // test for various NULL input cases based on nullCase value passed
        case NullCase::DEC_PTR:
            res = LzmaDec_DecodeToDic(NULL, outSize, src, srcLen, LZMA_FINISH_END, &status);
            break;
        case NullCase::SRC:
            res = LzmaDec_DecodeToDic(&p, outSize, NULL, srcLen, LZMA_FINISH_END, &status);
            break;
        case NullCase::SRC_LEN:
            res = LzmaDec_DecodeToDic(&p, outSize, src, NULL, LZMA_FINISH_END, &status);
            break;
        case NullCase::STATUS:
            res = LzmaDec_DecodeToDic(&p, outSize, src, srcLen, LZMA_FINISH_END, NULL);
            break;
        default:
            return;
        }
        EXPECT_EQ(res, SZ_ERROR_PARAM); // all NULL cases should return SZ_ERROR_PARAM
        
        LzmaDec_FreeProbs(&p, &g_Alloc);
    }
};

TEST_F(LZMA_decodeDict, AOCL_Compression_lzma_LzmaDec_DecodeToDic_valid_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);
    EXPECT_GT(outLen, 0);

    //validate
    validate();
}

TEST_F(LZMA_decodeDict, AOCL_Compression_lzma_LzmaDec_DecodeToDic_invalidInit_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    decode_invalid_init((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);
}

TEST_F(LZMA_decodeDict, AOCL_Compression_lzma_LzmaDec_DecodeToDic_nullCLzmaDec_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    decode_null_checks((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE, NullCase::DEC_PTR); // CLzmaDec NULL
}

TEST_F(LZMA_decodeDict, AOCL_Compression_lzma_LzmaDec_DecodeToDic_nullSrc_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    decode_null_checks((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE, NullCase::SRC); // src NULL
}

TEST_F(LZMA_decodeDict, AOCL_Compression_lzma_LzmaDec_DecodeToDic_nullSrcLen_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    decode_null_checks((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE, NullCase::SRC_LEN); // srcLen NULL
}

TEST_F(LZMA_decodeDict, AOCL_Compression_lzma_LzmaDec_DecodeToDic_nullStatus_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    decode_null_checks((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE, NullCase::STATUS); // status NULL
}
/*********************************************
* End of LZMA_decodeDict
*********************************************/

/*********************************************
* Begin of LZMA_decodeFile
*********************************************/
/*
    Fixture class for testing LzmaDecode
*/
class LZMA_decodeFile : public LZMA_decodeBase
{
public:
    void SetUp() override {
        SetUpBuffers();
    }

    void TearDown() override
    {
        TearDownBuffers();
    }

    void encode() {
        outLen = outSize - LZMA_PROPS_SIZE;
        LzmaEncProps_Init(&props); //necessary, else props will have invalid values

        //compress using LzmaEncode
        SRes res = LzmaEncode((uint8_t*)compPtr + LZMA_PROPS_SIZE, &outLen, (uint8_t*)inPtr, inSize,
            &props, (uint8_t*)compPtr, &headerSize, 0, NULL, &g_Alloc, &g_AllocBig);
        ASSERT_EQ(res, SZ_OK);
    }

    SRes decode(Byte* dest, SizeT* destLen, const Byte* src, SizeT* srcLen,
        const Byte* propData, unsigned propSize) {
        ELzmaStatus status;
        return LzmaDecode(dest, destLen, src, srcLen,
            propData, propSize, LZMA_FINISH_END, &status, &g_Alloc);
    }
};

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_valid_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    SRes res = decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);

    EXPECT_EQ(res, SZ_OK);
    EXPECT_GT(outLen, 0);

    //validate
    validate();
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_invalidInSize_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    //size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = 4; //invalid srcLen

    SRes res = decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);

    EXPECT_EQ(res, SZ_ERROR_INPUT_EOF);
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_invalidHeader_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    compPtr[0] = (Byte)(9 * 5 * 5); //invalid header value
    SRes res = decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);

    EXPECT_NE(res, SZ_OK);
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_invalidHeaderSz_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;
    unsigned invalidPropSize = LZMA_PROPS_SIZE - 1; //invalid header size

    SRes res = decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, invalidPropSize);

    EXPECT_NE(res, SZ_OK);
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_invalidCompressedInp_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    for (int i = 0; i < 8; ++i) { // scramble compressed data
        compPtr[LZMA_PROPS_SIZE + i] = rand() % 256;
    }
    SRes res = decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);

    EXPECT_NE(res, SZ_OK);
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_srcNull_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    //src = nullptr
    SRes res = decode((uint8_t*)decompPtr, &outLen, nullptr,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);

    EXPECT_NE(res, SZ_OK);
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_srcSzZero_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    //size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = 0; //src len 0

    SRes res = decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);

    EXPECT_NE(res, SZ_OK);
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_dstNull_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    //dest = nullptr
    SRes res = decode(nullptr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, (uint8_t*)compPtr, LZMA_PROPS_SIZE);

    EXPECT_NE(res, SZ_OK);
}

TEST_F(LZMA_decodeFile, AOCL_Compression_lzma_LzmaDecode_headerNull_common_1)
{
    encode(); //encode and get compressed stream

    //decompress
    size_t d_inSize = LZMA_PROPS_SIZE + outLen;
    size_t d_outSize = inSize;
    SizeT outLen = d_outSize;
    SizeT srcLen = d_inSize - LZMA_PROPS_SIZE;

    //propData = nullptr
    SRes res = decode((uint8_t*)decompPtr, &outLen, (uint8_t*)compPtr + LZMA_PROPS_SIZE,
        &srcLen, nullptr, LZMA_PROPS_SIZE);

    EXPECT_NE(res, SZ_OK);
}

/*********************************************
* End of LZMA_decodeFile
*********************************************/

/*********************************************
 * End API tests for LzmaDec
 *********************************************/
#endif /* AOCL_LZMA_OPT */

/*********************************************
 * Begin API tests for xz utils lzma
 *********************************************/
#ifdef AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL
#include "algos/lzma/lzma.h"

lzma_ret Test_lzma_easy_buffer_encode(uint32_t preset, lzma_check check,
    const lzma_allocator* allocator, const uint8_t* in,
    size_t in_size, uint8_t* out, size_t* out_pos, size_t out_size) {
    return lzma_easy_buffer_encode(preset, check, allocator, in, in_size, out, out_pos, out_size);
}

lzma_ret Test_lzma_stream_buffer_decode(uint64_t* memlimit, uint32_t flags,
    const lzma_allocator* allocator,
    const uint8_t* in, size_t* in_pos, size_t in_size,
    uint8_t* out, size_t* out_pos, size_t out_size) {
    return lzma_stream_buffer_decode(memlimit, flags, allocator, in, in_pos, in_size, out, out_pos, out_size);
}

uint32_t Test_lzma_version_number(void) {
    return lzma_version_number();
}

const char* Test_lzma_version_string(void) {
    return lzma_version_string();
}

/*
    Base class for all fixtures that test/use xz lzma encoder
*/
class LZMA_XZ_encodeBase : public LZMA_buffersBase, public AOCL_setup_lzma {
public:
    void decomp_validate(size_t resultComp) {
        //compress
        EXPECT_GT(resultComp, 0);

        //decompress
        int64_t resultDecomp = decompress(compPtr, resultComp, decompPtr, inSize);
        EXPECT_GT(resultDecomp, 0);

        //validate
        EXPECT_EQ(memcmp(inPtr, decompPtr, inSize), 0); //decompressed data must match input
    }

    CLzmaEncProps props;

protected:
    void SetUpEncode() {
        inSize = 1024 * 1024; //1MB
        //setup buffers
        buffers_setup();
    }

    void TearDownEncode() {
        //free buffers
        if (inPtr) free(inPtr);
        if (compPtr) free(compPtr);
        if (decompPtr) free(decompPtr);
    }

private:
    int64_t decompress(char* inPtr, size_t inSize, char* outBuf, size_t outSize) {
        uint64_t memlimit = 1 << 25;
        size_t in_pos = 0, out_pos = 0;
        lzma_ret ret_lzma = lzma_stream_buffer_decode(&memlimit, 0, NULL,
            (uint8_t*)inPtr,
            &in_pos,
            inSize,
            (uint8_t*)outBuf,
            &out_pos,
            outSize);
        if (ret_lzma != LZMA_OK)
            return 0;

        return (int64_t)out_pos;
    }
};

/*
    Base class for all fixtures that test/use xz lzma decoder
*/
class LZMA_XZ_decodeBase : public LZMA_buffersBase, public AOCL_setup_lzma 
{
public:
    void validate() {
        EXPECT_EQ(memcmp(inPtr, decompPtr, inSize), 0); //decompressed data must match input
    }

protected:
    void SetUpBuffers() {
        //setup buffers
        inSize = 1024 * 1024; //1MB
        buffers_setup();
    }

    void TearDownBuffers() {
        //free buffers
        if (inPtr) free(inPtr);
        if (compPtr) free(compPtr);
        if (decompPtr) free(decompPtr);
    }
};

/*********************************************
* Begin of LZMA_XZ_encodeFile
*********************************************/
/*
    Fixture class for testing LZMA_encodeFile
    LZMA_encodeFile is used for file encoding
*/
class LZMA_XZ_encodeFile : public LZMA_XZ_encodeBase
{
public:
    void SetUp() override {
        SetUpEncode();
    }

    void TearDown() override {
        TearDownEncode();
    }

    void execute(uint32_t preset, lzma_check check) {
        //compress using lzma_easy_buffer_encode
        size_t out_pos = 0;
        lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(preset, check, NULL /*lzma_allocator*/,
            (uint8_t*)inPtr, inSize,
            (uint8_t*)compPtr, &out_pos, outSize);

        EXPECT_EQ(ret_lzma, LZMA_OK);

        //decompress and validate
        decomp_validate(out_pos);
    }

    void execute_invalid(uint32_t preset, lzma_check check) {
        //compress using lzma_easy_buffer_encode
        size_t out_pos = 0;
        lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(preset, check, NULL /*lzma_allocator*/,
            (uint8_t*)inPtr, inSize,
            (uint8_t*)compPtr, &out_pos, outSize);

        EXPECT_NE(ret_lzma, LZMA_OK);
    }
};

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_valid_common)
{
    //Test for all levels
    for (int level = LZMA_MIN_LEVEL; level <= LZMA_MAX_LEVEL; ++level) {
        execute(level, LZMA_CHECK_NONE); //AOCL_Compression_lzma_lzma_easy_buffer_encode_valid_common_1 - 10
    }
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidPreset_common_1)
{
    execute_invalid(LZMA_MIN_LEVEL-1, LZMA_CHECK_NONE); //level < LZMA_MIN_LEVEL
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidPreset_common_2)
{
    execute_invalid(LZMA_MAX_LEVEL + 1, LZMA_CHECK_NONE); //level > LZMA_MAX_LEVEL
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidPreset_common_3)
{
    //preset format : extreme....flags......level
    //bit number    : 31.........30_to_6....5_to_0
    uint32_t preset = LZMA_MIN_LEVEL;
    for (int i = 6; i < 31; ++i) {
        preset |= (UINT32_C(1) << i); //set different flags in preset (flags not supported in aocl lzma)
        execute_invalid(preset, LZMA_CHECK_NONE);
    }
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidPreset_common_4)
{
    uint32_t preset = LZMA_MIN_LEVEL;
    preset |= (UINT32_C(1) << 31); //LZMA_PRESET_EXTREME
    execute_invalid(preset, LZMA_CHECK_NONE);
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidCheck_common_1)
{
    execute_invalid(LZMA_MAX_LEVEL, LZMA_CHECK_CRC32); //check not supported in aocl lzma
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidCheck_common_2)
{
    execute_invalid(LZMA_MAX_LEVEL, LZMA_CHECK_CRC64); //check not supported in aocl lzma
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidCheck_common_3)
{
    execute_invalid(LZMA_MAX_LEVEL, LZMA_CHECK_SHA256); //check not supported in aocl lzma
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_noSrc_common_1)
{
    //input buffer: in = nullptr
    size_t out_pos = 0;
    lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(LZMA_MIN_LEVEL, LZMA_CHECK_NONE, NULL /*lzma_allocator*/,
        nullptr, inSize,
        (uint8_t*)compPtr, &out_pos, outSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_noSrcSz_common_1)
{
    SizeT invalidInSize = 0; //in_size empty
    size_t out_pos = 0;
    lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(LZMA_MIN_LEVEL, LZMA_CHECK_NONE, NULL /*lzma_allocator*/,
        (uint8_t*)inPtr, invalidInSize,
        (uint8_t*)compPtr, &out_pos, outSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_noDst_common_1)
{
    //output buffer: out = nullptr
    size_t out_pos = 0;
    lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(LZMA_MIN_LEVEL, LZMA_CHECK_NONE, NULL /*lzma_allocator*/,
        (uint8_t*)inPtr, inSize,
        nullptr, &out_pos, outSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_noDstPos_common_1)
{
    //output buffer pos: out_pos = nullptr
    lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(LZMA_MIN_LEVEL, LZMA_CHECK_NONE, NULL /*lzma_allocator*/,
        (uint8_t*)inPtr, inSize,
        (uint8_t*)compPtr, nullptr, outSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_encodeFile, AOCL_Compression_lzma_lzma_easy_buffer_encode_invalidDstPos_common_1)
{
    //output buffer pos: out_pos > out_size
    size_t out_pos = outSize + 1;
    lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(LZMA_MIN_LEVEL, LZMA_CHECK_NONE, NULL /*lzma_allocator*/,
        (uint8_t*)inPtr, inSize,
        (uint8_t*)compPtr, &out_pos, outSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

/*********************************************
* End of LZMA_XZ_encodeFile
*********************************************/

/*********************************************
* Begin of LZMA_XZ_decodeFile
*********************************************/
/*
    Fixture class for testing LzmaDecode
*/
class LZMA_XZ_decodeFile : public LZMA_XZ_decodeBase
{
public:
    void SetUp() override {
        SetUpBuffers();
    }

    void TearDown() override
    {
        TearDownBuffers();
    }

    size_t encode() {
        //compress using lzma_easy_buffer_encode
        size_t out_pos = 0;
        lzma_ret ret_lzma = Test_lzma_easy_buffer_encode(LZMA_MIN_LEVEL, LZMA_CHECK_NONE, NULL /*lzma_allocator*/,
            (uint8_t*)inPtr, inSize,
            (uint8_t*)compPtr, &out_pos, outSize);
        EXPECT_EQ(ret_lzma, LZMA_OK);
        return out_pos;
    }

    size_t decode(char* inPtr, size_t inSize, char* outBuf, size_t outSize, lzma_ret& ret_lzma) {
        uint64_t memlimit = 1 << 25;
        size_t in_pos = 0, out_pos = 0;
        ret_lzma = Test_lzma_stream_buffer_decode(&memlimit, 0, NULL,
            (uint8_t*)inPtr, &in_pos, inSize,
            (uint8_t*)outBuf, &out_pos, outSize);
        if (ret_lzma != LZMA_OK)
            return 0;
        return out_pos;
    }

    void decode_valid(size_t compSz) {
        lzma_ret ret_lzma;
        size_t decompSz = decode(compPtr, compSz, decompPtr, inSize, ret_lzma);
        EXPECT_EQ(ret_lzma, LZMA_OK);
        EXPECT_EQ(decompSz, inSize);
        EXPECT_EQ(memcmp(inPtr, decompPtr, inSize), 0); //decompressed data must match input
    }

    void decode_invalid(size_t compSz) {
        lzma_ret ret_lzma;
        decode(compPtr, compSz, decompPtr, inSize, ret_lzma);
        EXPECT_NE(ret_lzma, LZMA_OK);
    }

    void validate() {
        
    }
};

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_valid_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream
    decode_valid(compSz); //decompress and validate
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_invalidCompressedInp_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    for (int i = 0; i < 8; ++i) { // scramble compressed data
        compPtr[LZMA_PROPS_SIZE + i] = rand() % 256;
    }

    decode_invalid(compSz); //decompress and validate
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_srcNull_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    lzma_ret ret_lzma;
    decode(NULL, compSz, decompPtr, inSize, ret_lzma); //src = nullptr
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_srcSzZero_common_1)
{
    encode(); //encode and get compressed stream

    lzma_ret ret_lzma;
    decode(compPtr, 0, decompPtr, inSize, ret_lzma); //src size = 0
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_dstNull_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    lzma_ret ret_lzma;
    decode(compPtr, compSz, NULL, inSize, ret_lzma); //dst = nullptr
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_inPosNull_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    size_t out_pos = 0;
    lzma_ret ret_lzma = Test_lzma_stream_buffer_decode(NULL, 0, NULL,
        (uint8_t*)compPtr, NULL, compSz,
        (uint8_t*)decompPtr, &out_pos, inSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_outPosNull_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    size_t in_pos = 0;
    lzma_ret ret_lzma = Test_lzma_stream_buffer_decode(NULL, 0, NULL,
        (uint8_t*)compPtr, &in_pos, compSz,
        (uint8_t*)decompPtr, NULL, inSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_invalidInPos_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    size_t in_pos = inSize + 1, out_pos = 0; //in_pos > inSize
    lzma_ret ret_lzma = Test_lzma_stream_buffer_decode(NULL, 0, NULL,
        (uint8_t*)compPtr, &in_pos, compSz,
        (uint8_t*)decompPtr, &out_pos, inSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_invalidOutPos_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    size_t in_pos = 0, out_pos = outSize + 1; //out_pos > outSize
    lzma_ret ret_lzma = Test_lzma_stream_buffer_decode(NULL, 0, NULL,
        (uint8_t*)compPtr, &in_pos, compSz,
        (uint8_t*)decompPtr, &out_pos, inSize);
    EXPECT_NE(ret_lzma, LZMA_OK);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_invalidFlag_common_1)
{
    size_t compSz = encode(); //encode and get compressed stream

    size_t in_pos = 0, out_pos = 0;
    uint32_t flags = 0x04; //invalid flag : LZMA_TELL_ANY_CHECK
    lzma_ret ret_lzma = Test_lzma_stream_buffer_decode(NULL, flags, NULL,
        (uint8_t*)compPtr, &in_pos, compSz,
        (uint8_t*)decompPtr, &out_pos, inSize);
    EXPECT_EQ(ret_lzma, LZMA_OPTIONS_ERROR);
}

TEST_F(LZMA_XZ_decodeFile, AOCL_Compression_lzma_lzma_stream_buffer_decode_invalidFlag_common_2)
{
    size_t compSz = encode(); //encode and get compressed stream

    size_t in_pos = 0, out_pos = 0;
    uint32_t flags = (UINT32_C(1) << 7); //invalid flag : not in LZMA_SUPPORTED_FLAGS
    lzma_ret ret_lzma = Test_lzma_stream_buffer_decode(NULL, flags, NULL,
        (uint8_t*)compPtr, &in_pos, compSz,
        (uint8_t*)decompPtr, &out_pos, inSize);
    EXPECT_EQ(ret_lzma, LZMA_OPTIONS_ERROR);
}

/*********************************************
* End of LZMA_XZ_decodeFile
*********************************************/

/*********************************************
* Begin of LZMA_XZ_version
*********************************************/
TEST(LZMA_XZ_version, AOCL_Compression_lzma_lzma_version_number_common_1)
{
    uint32_t ver = Test_lzma_version_number();
    EXPECT_EQ(ver, LZMA_VERSION);
}

TEST(LZMA_XZ_version, AOCL_Compression_lzma_lzma_version_string_common_1)
{
    const char* ver = Test_lzma_version_string();
    EXPECT_STREQ(ver, LZMA_VERSION_STRING);
}

/*********************************************
* End of LZMA_XZ_version
*********************************************/

#endif
/*********************************************
 * End API tests for xz utils lzma
 *********************************************/
