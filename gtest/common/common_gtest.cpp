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

 /** @file common_gtest.cpp
 *
 *  @brief Test cases for common optimizations shared by multiple algos.
 *
 *  This file contains test cases for optimized common macros/functions
 *  used by multiple algos.
 *
 *  @author Ashish Sriram
 */

#include <string>
#include <climits>
#include "gtest/gtest.h"

#include "algos/common/aoclHashChain.h"

using namespace std;

#define TEST_AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX) \
    AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX)

#define TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX) \
    AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX)

#define TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, hashTable, hcHeadPos, prevVal, hashIdx, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyHeadValue) \
    AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, hashTable, hcHeadPos, prevVal, hashIdx, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyHeadValue)

#define TEST_AOCL_COMMON_CEHCFIX_INSERT(chainTable, hashTable, hcHeadPos, prevVal, val, hashIdx, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX) \
    AOCL_COMMON_CEHCFIX_INSERT(chainTable, hashTable, hcHeadPos, prevVal, val, hashIdx, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX)

#define TEST_AOCL_COMMON_CEHCFIX_GET(chainTable, hashTable, hcCur, prevVal, val, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX) \
    AOCL_COMMON_CEHCFIX_GET(chainTable, hashTable, hcCur, prevVal, val, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX)

#define TEST_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT(chainTable, hcCur, val, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyNodeValue, hcHeadPos) \
    AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT(chainTable, hcCur, val, \
    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyNodeValue, hcHeadPos)

/*********************************************
* "Begin" of Common_CehcFix_circBuf
*********************************************/
/*
* Fixture class for testing `TEST_AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD` and
* 'TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD' macros
*/
class Common_CehcFix_circBuf : public ::testing::Test
{
public:
    /* [headPtr | hcChain]
    for hcCur = 1 to HASH_CHAIN_MAX, inc by 1
    for hcCur = HASH_CHAIN_MAX, circle back to 1 */
    void circular_buffer_inc_test(unsigned HASH_CHAIN_OBJECT_SZ, unsigned HASH_CHAIN_MAX) {
        unsigned hcCur = 1, hcHeadNxt;
        // run HASH_CHAIN_MAX times to ensure hcCur gets all possible values
        for (int i = hcCur; i < HASH_CHAIN_MAX; ++i) {
            hcHeadNxt = TEST_AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
            EXPECT_EQ(hcHeadNxt, (hcCur + 1));
            hcCur = hcHeadNxt;
        }
        hcCur = HASH_CHAIN_MAX;
        hcHeadNxt = TEST_AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
        EXPECT_EQ(hcHeadNxt, 1);
    }

    void circular_buffer_dec_test(unsigned HASH_CHAIN_OBJECT_SZ, unsigned HASH_CHAIN_MAX) {
        unsigned hcCur = HASH_CHAIN_MAX, hcHeadNxt;
        // run HASH_CHAIN_MAX times to ensure hcCur gets all possible values
        for (int i = hcCur; i > 1; --i) {
            hcHeadNxt = TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
            EXPECT_EQ(hcHeadNxt, (hcCur - 1));
            hcCur = hcHeadNxt;
        }
        hcCur = 1;
        hcHeadNxt = TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
        EXPECT_EQ(hcHeadNxt, HASH_CHAIN_MAX);
    }
};

TEST_F(Common_CehcFix_circBuf, AOCL_Compression_common_TEST_AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD_Inc8_common_1)
{
    unsigned HASH_CHAIN_OBJECT_SZ = 8;
    unsigned HASH_CHAIN_MAX = HASH_CHAIN_OBJECT_SZ - 1;
    circular_buffer_inc_test(HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
}

TEST_F(Common_CehcFix_circBuf, AOCL_Compression_common_TEST_AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD_Inc16_common_1)
{
    unsigned HASH_CHAIN_OBJECT_SZ = 16;
    unsigned HASH_CHAIN_MAX = HASH_CHAIN_OBJECT_SZ - 1;
    circular_buffer_inc_test(HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
}

TEST_F(Common_CehcFix_circBuf, AOCL_Compression_common_TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD_Dec8_common_1)
{
    unsigned HASH_CHAIN_OBJECT_SZ = 8;
    unsigned HASH_CHAIN_MAX = HASH_CHAIN_OBJECT_SZ - 1;
    circular_buffer_dec_test(HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
}

TEST_F(Common_CehcFix_circBuf, AOCL_Compression_common_TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD_Dec16_common_1)
{
    unsigned HASH_CHAIN_OBJECT_SZ = 16;
    unsigned HASH_CHAIN_MAX = HASH_CHAIN_OBJECT_SZ - 1;
    circular_buffer_dec_test(HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
}

/*********************************************
 * End of Common_CehcFix_circBuf
 *********************************************/

class CehcTable : public ::testing::Test
{
public:
    void SetUp() override {
        ctSzBytes = sizeof(unsigned) * (CHAIN_CNT * HASH_CHAIN_OBJECT_SZ);
        chainTable = (unsigned*)malloc(ctSzBytes);
        memset(chainTable, 0, ctSzBytes);
    }

    void TearDown() override {
        free(chainTable);
    }

    unsigned  kEmptyValue = 0;
    const int CHAIN_CNT = 128;
    const int HASH_CHAIN_OBJECT_SZ = 8;
    const int HASH_CHAIN_MAX = HASH_CHAIN_OBJECT_SZ - 1;
    unsigned* chainTable;
    size_t ctSzBytes;
};


 /*********************************************
 * "Begin" of Common_CehcFix_chainHead
 *********************************************/
 /*
* Fixture class for testing 'TEST_AOCL_COMMON_CEHCFIX_GET_HEAD' macro
 */
class Common_CehcFix_chainHead : public CehcTable
{
public:
    unsigned head_on_init(unsigned hashIdx) {
        return ((hashIdx * HASH_CHAIN_OBJECT_SZ) + 1);
    }
};

TEST_F(Common_CehcFix_chainHead, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_GET_HEAD_emptyHead_common_1)
{
    unsigned hashIdx = rand() % CHAIN_CNT;
    unsigned hcCur;
    TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcCur, 0, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue);
    EXPECT_EQ(hcCur, head_on_init(hashIdx));
}

TEST_F(Common_CehcFix_chainHead, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_GET_HEAD_emptyHead_common_2)
{
    kEmptyValue = UINT32_MAX;
    memset(chainTable, 0xFF, ctSzBytes); //empty values all 1s (UINT32_MAX)

    unsigned hashIdx = rand() % CHAIN_CNT;
    unsigned hcCur;
    TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcCur, 0, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue);
    EXPECT_EQ(hcCur, head_on_init(hashIdx));
}

TEST_F(Common_CehcFix_chainHead, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_GET_HEAD_nonEmptyHead_common_1)
{
    unsigned hashIdx = rand() % CHAIN_CNT;
    unsigned hcHeadPos, val = 1;
    TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcHeadPos, 0, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue); //1st get, head is empty

    TEST_AOCL_COMMON_CEHCFIX_INSERT(chainTable, 0, hcHeadPos, 0, val, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX); //set node

    TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcHeadPos, 0, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue); //2nd get

    EXPECT_EQ(hcHeadPos, TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(head_on_init(hashIdx),
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX)); //prev pos as head moved on set node
}

TEST_F(Common_CehcFix_chainHead, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_GET_HEAD_nonEmptyHead_common_2)
{
    kEmptyValue = UINT32_MAX;
    memset(chainTable, 0xFF, ctSzBytes); //empty values all 1s (UINT32_MAX)

    unsigned hashIdx = rand() % CHAIN_CNT;
    unsigned hcHeadPos, val = 1;
    TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcHeadPos, 0, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue); //1st get, head is empty

    TEST_AOCL_COMMON_CEHCFIX_INSERT(chainTable, 0, hcHeadPos, 0, val, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX); //set node

    TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcHeadPos, 0, hashIdx,
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue); //2nd get

    EXPECT_EQ(hcHeadPos, TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(head_on_init(hashIdx),
        HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX)); //prev pos as head moved on set node
}

/*********************************************
 * End of Common_CehcFix_chainHead
 *********************************************/

/*********************************************
* Begin of Common_CehcFix_chainInsert
*********************************************/
/*
* Fixture class for testing `AOCL_COMMON_CEHCFIX_GET_HEAD` and 
* `AOCL_COMMON_CEHCFIX_INSERT` macros
* 
* This test runs a typical Insert operation sequence:
* 1. Call AOCL_COMMON_CEHCFIX_GET_HEAD() to get hcHeadPos.
* 2. Add new node in hashChain at the position next to hcHeadPos 
*    using AOCL_COMMON_CEHCFIX_INSERT().
*/
class Common_CehcFix_chainInsert : public CehcTable
{
};

TEST_F(Common_CehcFix_chainInsert, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_INSERT_common_1)
{
    unsigned hashIdx = rand() % CHAIN_CNT;
    unsigned hcBase = hashIdx * HASH_CHAIN_OBJECT_SZ;
    unsigned hcHeadPos = 0;
    
    for (int i = 0; i < HASH_CHAIN_MAX; i++) { // run HASH_CHAIN_MAX times to test insertion at all nodes
        TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcHeadPos, 0, hashIdx,
            HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue); // get hcHeadPos

        unsigned nextHead = TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(hcHeadPos,
            HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX); // compute nextHead value for validation

        unsigned pos = rand();
        TEST_AOCL_COMMON_CEHCFIX_INSERT(chainTable, 0, hcHeadPos, 0, pos, hashIdx,
            HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX); // insert node at next position

        EXPECT_EQ(chainTable[nextHead], pos); // check if inserted at next position
        EXPECT_EQ(chainTable[hcBase], nextHead); // check if hcHeadPos points to nextHead
    }
}

/*********************************************
 * End of Common_CehcFix_chainInsert
 *********************************************/

 /*********************************************
* Begin of Common_CehcFix_chainSearch
 *********************************************/
 /*
* Fixture class for testing `TEST_AOCL_COMMON_CEHCFIX_GET` and
* `TEST_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT` macros
* 
* This test runs a typical Search operation sequence:
* 1. Call AOCL_COMMON_CEHCFIX_GET_HEAD() to get hcHeadPos. [called from setup_block_chain()].
* 2. Call AOCL_COMMON_CEHCFIX_GET() to read value at this position
*    in the hash chain.
* 3. Subsequently, keep calling AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT() 
*    to move to next nodes in the chain and read values there.
 */
class Common_CehcFix_chainSearch : public CehcTable
{
public:
    void setup_block_chain(int count) {
        unsigned hashIdx = rand() % CHAIN_CNT;
        if (count == 0) { // just setup head. Do not insert node
            TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcHeadPos, 0, hashIdx,
                HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue);
        }
        else {
            int pos = 1;
            for (int i = 0; i < count; ++i) { // fill block chain
                TEST_AOCL_COMMON_CEHCFIX_GET_HEAD(chainTable, 0, hcHeadPos, 0, hashIdx,
                    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue);
                TEST_AOCL_COMMON_CEHCFIX_INSERT(chainTable, 0, hcHeadPos, 0, pos, hashIdx,
                    HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
                pos = get_next_circ(pos);
            }
        }
    }

    unsigned get_next_circ(unsigned hcCur) {
        return TEST_AOCL_COMMON_CEHCFIX_CIRC_INC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
    }

    unsigned get_prev_circ(unsigned hcCur) {
        return TEST_AOCL_COMMON_CEHCFIX_CIRC_DEC_HEAD(hcCur, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
    }

    unsigned hcHeadPos;
};

TEST_F(Common_CehcFix_chainSearch, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT_chainFull_common_1)
{
    const unsigned setNodes = HASH_CHAIN_MAX;
    setup_block_chain(setNodes);

    unsigned pos;
    unsigned hcHeadRef, hcCur;
    hcHeadRef = hcCur = hcHeadPos;
    unsigned posCheck = setNodes;
    TEST_AOCL_COMMON_CEHCFIX_GET(chainTable, 0, hcCur, 0,
        pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
    EXPECT_EQ(hcCur, hcHeadRef);
    EXPECT_EQ(pos, posCheck);

    int i = 1;
    for (; i < HASH_CHAIN_MAX; ++i) { // run HASH_CHAIN_MAX times to ensure hcCur gets all possible values
        hcHeadRef = get_next_circ(hcHeadRef);
        posCheck = get_prev_circ(posCheck);
        TEST_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT(chainTable, hcCur,
            pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue, hcHeadPos);
        EXPECT_EQ(hcCur, hcHeadRef);
        EXPECT_EQ(pos, posCheck);
    }
    EXPECT_EQ(i, HASH_CHAIN_MAX); // ensure loop does not exit prematurely
}

TEST_F(Common_CehcFix_chainSearch, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT_chainEmpty_common_1)
{
    setup_block_chain(0);

    unsigned pos;
    unsigned hcHeadRef, hcCur;
    hcHeadRef = hcCur = hcHeadPos;
    TEST_AOCL_COMMON_CEHCFIX_GET(chainTable, 0, hcCur, 0,
        pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
    EXPECT_EQ(hcCur, hcHeadRef);
    EXPECT_EQ(pos, kEmptyValue);

    int i = 1;
    for (int i = 1; i < HASH_CHAIN_MAX; ++i) {
        TEST_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT(chainTable, hcCur,
            pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue, hcHeadPos); // as there are no nodes in chain, this should exit loop.
    }
    EXPECT_EQ(i, 1); // ensure loop exited on 1st check
}

TEST_F(Common_CehcFix_chainSearch, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT_chainPartial_common_1)
{
    const unsigned setNodes = HASH_CHAIN_MAX - 1; // do not fill all nodes in chain
    setup_block_chain(setNodes);

    unsigned pos;
    unsigned hcHeadRef, hcCur;
    hcHeadRef = hcCur = hcHeadPos;
    unsigned posCheck = setNodes;
    TEST_AOCL_COMMON_CEHCFIX_GET(chainTable, 0, hcCur, 0,
        pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
    EXPECT_EQ(hcCur, hcHeadRef);
    EXPECT_EQ(pos, posCheck);

    int i = 1;
    for (; i < HASH_CHAIN_MAX; ++i) { // try to run HASH_CHAIN_MAX times. Loop must exit on end of chain.
        hcHeadRef = get_next_circ(hcHeadRef);
        posCheck = get_prev_circ(posCheck);
        TEST_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT(chainTable, hcCur,
            pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue, hcHeadPos);
        EXPECT_EQ(hcCur, hcHeadRef);
        EXPECT_EQ(pos, posCheck);
    }
    EXPECT_EQ(i, setNodes); // ensure loop exited on Nth check
}

TEST_F(Common_CehcFix_chainSearch, AOCL_Compression_common_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT_rollOver_common_1)
{
    const unsigned setNodes = HASH_CHAIN_MAX; // set all nodes in chain
    setup_block_chain(setNodes);

    unsigned pos;
    unsigned hcHeadRef, hcCur;
    hcHeadRef = hcCur = hcHeadPos;
    unsigned posCheck = setNodes;
    TEST_AOCL_COMMON_CEHCFIX_GET(chainTable, 0, hcCur, 0,
        pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX);
    EXPECT_EQ(hcCur, hcHeadRef);
    EXPECT_EQ(pos, posCheck);

    int i = 1;
    for (; i < HASH_CHAIN_MAX + 3; ++i) { // run > HASH_CHAIN_MAX times to test if rollOver check is triggered
        hcHeadRef = get_next_circ(hcHeadRef);
        posCheck = get_prev_circ(posCheck);
        TEST_AOCL_COMMON_CEHCFIX_MOVE_TO_NEXT(chainTable, hcCur,
            pos, HASH_CHAIN_OBJECT_SZ, HASH_CHAIN_MAX, kEmptyValue, hcHeadPos);
        EXPECT_EQ(hcCur, hcHeadRef);
        EXPECT_EQ(pos, posCheck);
    }
    EXPECT_EQ(hcCur, hcHeadPos); // validate roll over check
    EXPECT_EQ(i, HASH_CHAIN_MAX); // validate check was triggered before for loop condition evaluates to false
}

/*********************************************
 * End of Common_CehcFix_chainSearch
 *********************************************/
