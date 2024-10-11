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
 
 /** @file lz4hc_gtest.cc
 *  
 *  @brief Test cases for LZ4HC algo.
 *
 *  This file contains the test cases for LZ4HC method
 *  testing the API level functions of LZ4HC.
 *
 *  @author Partiksha
 */

#include <string>
#include <climits>
#include "gtest/gtest.h"
#include "gtest/gtest_utils.h"

#include <utils/utils.h>
#include "algos/lz4/lz4.h"

 /*===    Dependency    ===*/
#ifndef LZ4_HC_STATIC_LINKING_ONLY
#define LZ4_HC_STATIC_LINKING_ONLY
#endif
#include "algos/lz4/lz4hc.h"

#ifdef AOCL_TEST_FUZZER
#include "fuzztest/fuzztest.h"
#endif

#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

using namespace std;

/* read function to be called for Hash */
static uint32_t LZ4_read32(const void* memPtr)
{
    uint32_t val; memcpy(&val, memPtr, sizeof(val)); return val;
}

/* Hash function */
#define MINMATCH 4
#define LZ4HC_HASH_LOG 15
#define HASH_FUNCTION(i)         (((i) * 2654435761U) >> ((MINMATCH*8)-LZ4HC_HASH_LOG))
static uint32_t LZ4HC_hashPtr(const void* ptr) { return HASH_FUNCTION(LZ4_read32(ptr)); }

#define KB *(1 <<10)
#define GB *(1U<<30)
#define DEFAULT_OPT_LEVEL 2 // system running gtest must have AVX support

bool use_AOCL_LZ4_streamHC(int opt_off, int compression_level) {
    /* AOCL_LZ4_streamHC_t object is only used for compression levels that support CEHC.
     * Hence enable only for levels 6 to 9 when optimization is enabled. */
    return !(opt_off == 1 || compression_level < 6 || compression_level > 9);
}

class Test_Buffer
{
    /* source buffer (original data which we intend to compress). */
    char *orig_data = NULL;
    size_t orig_sz = 0;

    /* destination buffer (data obtained after compression). */
    char *compressed_data = NULL;
    size_t compressed_sz=0;

public:
    /* Constructor functions, creates `sz` size of source data. */
    Test_Buffer(int sz)
    {
        this->orig_sz = sz;
        orig_data = (char *)malloc(sz);

        /* Generating random data inside `orig_data` buffer. */
        for (int i = 0; i < sz; i++)
        {
            orig_data[i] = rand() % 255;
        }

        /* Provides the maximum size that LZ4/LZ4HC compression may output in a "worst case". */
        compressed_sz = LZ4_compressBound(sz);
        compressed_data = (char *)malloc(compressed_sz);
    }

    Test_Buffer(int inp_sz, int out_sz)
    {
        this->orig_sz = inp_sz;
        orig_data = (char*)malloc(inp_sz);

        // generating random data inside `orig_data` buffer.
        for (int i = 0; i < inp_sz; i++)
        {
            orig_data[i] = rand() % 255;
        }

        compressed_sz = out_sz; // custom out_sz
        compressed_data = (char*)malloc(compressed_sz);
    }

    /* Returns pointer to source buffer. */
    char *getOrigData()
    {
        return orig_data;
    }

    /* Returns size of source buffer. */
    size_t getOrigSize()
    {
        return orig_sz;
    }

    /* Returns pointer to destination buffer (data obtained after compression). */
    char *getCompressedBuff()
    {
        return compressed_data;
    }

    /* Returns size of destination data. */
    size_t getCompressedSize()
    {
        return compressed_sz;
    }

    /* Destructor function. */
    ~Test_Buffer()
    {
        if(orig_data) 
            free(orig_data);
        if(compressed_data)
            free(compressed_data);
    }
};

/* This base class can be used for all fixtures
* that require dynamic dispatcher setup */
class AOCL_setup_lz4hc : public ::testing::Test {
public:
    AOCL_setup_lz4hc() {
        int optLevel = DEFAULT_OPT_LEVEL;
        aocl_setup_lz4hc(0, optLevel, 0, 0, 0);
    }
};

// Create stream
class Stream
{
private:
    LZ4_streamHC_t* stream = NULL;
    int create_using_malloc;
public:
    Stream() { stream = NULL; }

    Stream(int CREATE_USING_MALLOC)   /* creating stream by malloc if parameter is passed as 1, else creating by API. */
    {
        create_using_malloc = CREATE_USING_MALLOC;
        if (create_using_malloc == 1)
        {
            stream = (LZ4_streamHC_t*)malloc(sizeof(LZ4_streamHC_t));
            memset(stream, 0, sizeof(LZ4_streamHC_t));
        }
        else stream = LZ4_createStreamHC();
    }

    LZ4_streamHC_t* Get_Stream()
    {
        return this->stream;
    }

    void Set_Stream(LZ4_streamHC_t* ptr)
    {
        this->stream = ptr;
    }
    int get_stream_Internal_dirty()
    {
        return stream->internal_donotuse.dirty;
    }

    const LZ4_byte* get_stream_Internal_base()
    {
        return stream->internal_donotuse.prefixStart;
    }

    const LZ4HC_CCtx_internal* get_stream_Internal_dictCtx()
    {
        return stream->internal_donotuse.dictCtx;
    }

    int get_stream_Internal_compressionLevel()
    {
        return stream->internal_donotuse.compressionLevel;
    }

    ~Stream()
    {
        if (stream)
        {
            if (create_using_malloc == 1) free(stream);
            else LZ4_freeStreamHC(stream);
        }
    }
};

#ifdef AOCL_LZ4HC_OPT
// Create AOCL stream
class AOCL_Stream
{
private:
    AOCL_LZ4_streamHC_t* AOCL_stream = NULL;
    int create_using_malloc;

public:
    AOCL_Stream() { AOCL_stream = NULL; }

    AOCL_Stream(int CREATE_USING_MALLOC)    /* creating stream by malloc if parameter is passed as 1, else creating by API. */
    {
        create_using_malloc = CREATE_USING_MALLOC;
        if (create_using_malloc == 1)
        {
            AOCL_stream = (AOCL_LZ4_streamHC_t*)malloc(sizeof(AOCL_LZ4_streamHC_t));
            memset(AOCL_stream, 0, sizeof(AOCL_LZ4_streamHC_t));
        }
        else AOCL_stream = AOCL_LZ4_createStreamHC();
    }

    AOCL_LZ4_streamHC_t* Get_Stream()
    {
        return this->AOCL_stream;
    }

    void Set_Stream(AOCL_LZ4_streamHC_t* ptr)
    {
        this->AOCL_stream = ptr;
    }

    int get_stream_Internal_dirty()
    {
        return AOCL_stream->internal_donotuse.dirty;
    }

    void set_stream_Internal_dirty()
    {
        AOCL_stream->internal_donotuse.dirty = 1;
    }

    const LZ4_byte* get_stream_Internal_base()
    {
        return AOCL_stream->internal_donotuse.prefixStart;
    }

    const AOCL_LZ4HC_CCtx_internal* get_stream_Internal_dictCtx()
    {
        return AOCL_stream->internal_donotuse.dictCtx;
    }

    int get_stream_Internal_compressionLevel()
    {
        return AOCL_stream->internal_donotuse.compressionLevel;
    }

    ~AOCL_Stream()
    {
        if (AOCL_stream)
        {
            if (create_using_malloc == 1) free(AOCL_stream);
            else AOCL_LZ4_freeStreamHC(AOCL_stream);
        }
    }
};
#endif

bool lz4hc_check_uncompressed_equal_to_original(char *src, unsigned srcSize, char *compressed, unsigned compressedLen)
{
    int uncompressedLen = srcSize + 10;
    char* uncompressed = (char*)malloc(uncompressedLen * sizeof(char));

    int uncompressedLenRes = LZ4_decompress_safe(compressed, uncompressed, compressedLen, uncompressedLen);

    if (uncompressedLenRes < 0) {//error code
        free(uncompressed);
        return false;
    }

    if (!(srcSize == (unsigned)uncompressedLenRes)) {
        free(uncompressed);
        return false;
    }

    bool ret = (memcmp(src, uncompressed, srcSize) == 0);
    free(uncompressed);
    return ret;
}

/***********************************************
 * "Begin" of LZ4_compress_HC Tests
 ***********************************************/
class LZ4HC_LZ4_compress_HC : public ::testing::TestWithParam<int> {
public:
    // cover the different code paths.
    int num_levels = 4;
    int cLevel[4] = {LZ4HC_CLEVEL_MIN, LZ4HC_CLEVEL_DEFAULT, LZ4HC_CLEVEL_OPT_MIN, LZ4HC_CLEVEL_MAX};

    void SetUp() override {
        aocl_destroy_lz4hc();
        int opt_off = GetParam();
        int optLevel = DEFAULT_OPT_LEVEL;
        aocl_setup_lz4hc(opt_off, optLevel, 0, 0, 0);
    }
};

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_fail_common_1) // compress_FAIL_src_is_NULL
{
    for(int num_lvl=0; num_lvl<num_levels; num_lvl++)
    {
        Test_Buffer test_buf(800);
        EXPECT_EQ(LZ4_compress_HC(NULL /*src*/, test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), cLevel[num_lvl]), 0);
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_fail_common_2) // Compress_FAIL_dst_is_NULL
{
    for(int num_lvl=0; num_lvl<num_levels; num_lvl++)
    {
        Test_Buffer test_buf(800);
        EXPECT_EQ(LZ4_compress_HC(test_buf.getOrigData(), NULL /* dst */, test_buf.getOrigSize(), test_buf.getCompressedSize(), cLevel[num_lvl]), 0);
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_3) // Compress_PASS_srcSize_is_0
{
    for(int num_lvl=0; num_lvl<num_levels; num_lvl++)
    {
        Test_Buffer test_buf(800);
        /* passing srcSize as 0 and src as not NULL. */
        EXPECT_EQ(LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), 0 /* srcSize */, test_buf.getCompressedSize(), cLevel[num_lvl]), 1);

        /* Passing src as NULL and srcSize as 0. */
        EXPECT_EQ(LZ4_compress_HC(NULL, test_buf.getCompressedBuff(), 0, test_buf.getCompressedSize(), cLevel[num_lvl]), 1);
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_fail_common_4) // Negative_srcSize_and_destSize
{
    for(int num_lvl=0; num_lvl<num_levels; num_lvl++)
    {
        Test_Buffer test_buf(800);
        /* passing negative value as srcSize and src as not NULL. */
        EXPECT_EQ(LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), -1 /* srcSize */, test_buf.getCompressedSize(), cLevel[num_lvl]), 0);

        /* Passing negative value as destSize and dst as not NULL. */
        EXPECT_EQ(LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), -1 /* dstSize */, cLevel[num_lvl]), 0);
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_5) // compress_PASS
{
    for(int num_lvl=0; num_lvl<num_levels; num_lvl++)
    {
        Test_Buffer test_buf(800);
        int outLen = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), cLevel[num_lvl]);
        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), outLen));
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_fail_common_6) // compress_FAIL_dst_size_not_enough
{
    for(int num_lvl=0; num_lvl<num_levels; num_lvl++)
    {
        Test_Buffer test_buf(800);
        int outLen = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getOrigSize() / 20, cLevel[num_lvl]);
        EXPECT_FALSE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), outLen));
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_7) // Compression_level_greater_than_maximum_limit
{
    Test_Buffer test_buf(800);
    int outLen = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), LZ4HC_CLEVEL_MAX+1 /* level */);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), outLen));
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_8) // Compression_level_less_than_minimum_limit
{
    Test_Buffer test_buf(800);
    int outLen = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), -1 /* level */);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), outLen));
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_9) // inputSize < LZ4_minLength 
{
    for(int num_lvl=0; num_lvl<num_levels; num_lvl++)
    {
        /*  MFLIMIT = 12
        *  LZ4_minLength = (MFLMIIT + 1)
        *  Testing for input size < LZ4_minLength, i.e, 12
        */
        Test_Buffer test_buf(12);
        int compressedSize = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), cLevel[num_lvl]);

        /* No compression for input size < LZ4_minLength 
        * Compressed length will be literal length (i.e, 12) + 1 (1 byte of token)
        */
        EXPECT_EQ(compressedSize, 13);          
        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_10) // simple pass case (all compression levels)
{
    for(int level=0; level<=LZ4HC_CLEVEL_MAX; level++)
    {
        Test_Buffer test_buf(800);
        int compressedSize = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), level /* level */);
        EXPECT_NE(compressedSize, 0);

        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
    }
}

#ifdef AOCL_ENABLE_THREADS

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_11) // pass_case_mt
{
    for(int level=0; level<=LZ4HC_CLEVEL_MAX; level++)
    {
        Test_Buffer test_buf(8*64*1024);
        int compressedSize = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), level /* level */);
        EXPECT_NE(compressedSize, 0);

        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_fail_common_12) // dstCapacity_inadequate_mt
{
    for(int level=0; level<=LZ4HC_CLEVEL_MAX; level++)
    {
        int srcLen = 16*64*1024;
        Test_Buffer test_buf(srcLen, srcLen/2 /* dst_size */);

        int compressedSize = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), level /* level */);
        EXPECT_EQ(compressedSize, 0);
    }
}

TEST_P(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_pass_common_13) // mt_compression_st_decompression
{
    for(int level=0; level<=LZ4HC_CLEVEL_MAX; level++)
    {
        Test_Buffer test_buf(8*64*1024);
        int compressedSize = LZ4_compress_HC(test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), level /* level */);
        EXPECT_NE(compressedSize, 0);

        int rap_metadata_len = aocl_skip_rap_frame_mt(test_buf.getCompressedBuff(), compressedSize);

        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff() + rap_metadata_len, compressedSize - rap_metadata_len));
    }
}

#endif

INSTANTIATE_TEST_SUITE_P(
    LZ4HC_TEST,
    LZ4HC_LZ4_compress_HC,
    ::testing::Values(1, 0));
/*********************************************
 * "End" of LZ4_compress_HC Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_initStreamHC Test
 *********************************************/
class LZ4HC_LZ4_initStreamHC : public ::testing::Test
{
public:
    void *lz4hc_s = NULL;
    void SetUp() override 
    {
        lz4hc_s = malloc(sizeof(LZ4_streamHC_t));
        EXPECT_NE(lz4hc_s, nullptr);
    }

    /* pointer to "internal_donotuse" struct member of stream buffer. */
    char* get_ptr_to_internal_donotuse(LZ4_streamHC_t* buffer)
    {
        LZ4HC_CCtx_internal* hcstate = &(buffer->internal_donotuse);
        return (char*)hcstate;
    }

    /* returns the size of "AOCL_LZ4HC_CCtx_internal" */
    int get_sizeof_ctx_internal()
    {
        return sizeof(LZ4HC_CCtx_internal);
    }

    /* Get the compression level. */
    short* getCLevel(LZ4_streamHC_t* strm)
    {
        return &(strm->internal_donotuse.compressionLevel);
    }

    /* Destructor */
    ~LZ4HC_LZ4_initStreamHC()
    {
        if(lz4hc_s) free(lz4hc_s);
    }
};

TEST_F(LZ4HC_LZ4_initStreamHC, AOCL_Compression_lz4hc_LZ4_initStreamHC_fail_common_1) // passing buffer as NULL
{
    EXPECT_EQ(LZ4_initStreamHC(NULL, sizeof(LZ4_streamHC_t)), nullptr);
}

TEST_F(LZ4HC_LZ4_initStreamHC, AOCL_Compression_lz4hc_LZ4_initStreamHC_fail_common_2) // passing insufficient size of stream
{
    EXPECT_EQ(LZ4_initStreamHC(lz4hc_s, sizeof(LZ4_streamHC_t) - 1), nullptr);
}

TEST_F(LZ4HC_LZ4_initStreamHC, AOCL_Compression_lz4hc_LZ4_initStreamHC_fail_common_3) // wrong alignment
{
    void *lz4hc_state = malloc(sizeof(LZ4_streamHC_t)+1); // one more byte allocated to avoid potential out of access.
    EXPECT_NE(lz4hc_state, nullptr);
    EXPECT_EQ(LZ4_initStreamHC((char*)lz4hc_state + 1, sizeof(LZ4_streamHC_t)), nullptr);
    free(lz4hc_state);
}

TEST_F(LZ4HC_LZ4_initStreamHC, AOCL_Compression_lz4hc_LZ4_initStreamHC_pass_common_4) // simple pass case  
{
    LZ4_streamHC_t* stream = LZ4_initStreamHC(lz4hc_s, sizeof(LZ4_streamHC_t)); 
    EXPECT_NE(stream, nullptr); 
    EXPECT_EQ(*getCLevel(stream), LZ4HC_CLEVEL_DEFAULT);
}

TEST_F(LZ4HC_LZ4_initStreamHC, AOCL_Compression_lz4hc_LZ4_initStreamHC_pass_common_5) // random buffer data 
{
    char* hcstate = get_ptr_to_internal_donotuse((LZ4_streamHC_t*)lz4hc_s);
    int ctx_internal_size = get_sizeof_ctx_internal();
    char *buffer = (char*)lz4hc_s;
    for(int i=0; i<sizeof(LZ4_streamHC_t); i++) buffer[i]=i;
    EXPECT_NE(LZ4_initStreamHC(buffer, sizeof(LZ4_streamHC_t)), nullptr);
    short *cLevel = getCLevel((LZ4_streamHC_t*)buffer);
    EXPECT_EQ(*cLevel, LZ4HC_CLEVEL_DEFAULT);
    *cLevel = 0;
    for(int i=0; i<ctx_internal_size; i++) 
        EXPECT_EQ(hcstate[i],0);
}
/*********************************************
 * "End" of LZ4_initStreamHC Tests
 *********************************************/

/*********************************************
 * "Begin" of AOCL_LZ4_initStreamHC Test
 *********************************************/
class LZ4HC_AOCL_LZ4_initStreamHC : public ::testing::Test
{
public:
    AOCL_LZ4_streamHC_t *lz4hc_s = NULL;
    void SetUp() override 
    {
        lz4hc_s = (AOCL_LZ4_streamHC_t*)malloc(sizeof(AOCL_LZ4_streamHC_t));
        EXPECT_NE(lz4hc_s, nullptr);
    }

    /* pointer to "internal_donotuse" struct member of stream buffer. */
    char* get_ptr_to_internal_donotuse(AOCL_LZ4_streamHC_t* buffer)
    {
        AOCL_LZ4HC_CCtx_internal* hcstate = &(buffer->internal_donotuse);
        return (char*)hcstate;
    }

    /* returns the size of "AOCL_LZ4HC_CCtx_internal" */
    int get_sizeof_ctx_internal()
    {
        return sizeof(AOCL_LZ4HC_CCtx_internal);
    }

    /* Get the compression level. */
    short* getCLevel(AOCL_LZ4_streamHC_t* strm)
    {
        return &(strm->internal_donotuse.compressionLevel);
    }

    /* Destructor */
    ~LZ4HC_AOCL_LZ4_initStreamHC()
    {
        if(lz4hc_s) free(lz4hc_s);
    }
};

TEST_F(LZ4HC_AOCL_LZ4_initStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_initStreamHC_fail_common_1) // passing buffer as NULL
{
    EXPECT_EQ(AOCL_LZ4_initStreamHC(NULL, sizeof(AOCL_LZ4_streamHC_t)), nullptr);
}

TEST_F(LZ4HC_AOCL_LZ4_initStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_initStreamHC_fail_common_2) // passing insufficient size of stream
{
    EXPECT_EQ(AOCL_LZ4_initStreamHC(lz4hc_s, sizeof(AOCL_LZ4_streamHC_t) - 1), nullptr);
}

TEST_F(LZ4HC_AOCL_LZ4_initStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_initStreamHC_fail_common_3) // wrong alignment
{
    void *lz4hc_state = malloc(sizeof(AOCL_LZ4_streamHC_t)+1); // one more byte allocated to avoid potential out of access.
    EXPECT_NE(lz4hc_state, nullptr);
    EXPECT_EQ(AOCL_LZ4_initStreamHC((char*)lz4hc_state + 1, sizeof(AOCL_LZ4_streamHC_t)), nullptr);
    free(lz4hc_state);
}

TEST_F(LZ4HC_AOCL_LZ4_initStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_initStreamHC_pass_common_4) // simple pass case  
{
    AOCL_LZ4_streamHC_t* stream = AOCL_LZ4_initStreamHC(lz4hc_s, sizeof(AOCL_LZ4_streamHC_t)); 
    EXPECT_NE(stream, nullptr); 
    EXPECT_EQ(*getCLevel(stream), LZ4HC_CLEVEL_DEFAULT);
}

TEST_F(LZ4HC_AOCL_LZ4_initStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_initStreamHC_pass_common_5) // random buffer data 
{
    char *buffer = (char*)lz4hc_s;
    for(int i=0; i<sizeof(AOCL_LZ4_streamHC_t); i++) buffer[i]=i;
    EXPECT_NE(AOCL_LZ4_initStreamHC(buffer, sizeof(AOCL_LZ4_streamHC_t)), nullptr);
    char* hcstate = get_ptr_to_internal_donotuse(lz4hc_s);
    int ctx_internal_size = get_sizeof_ctx_internal();
    short *cLevel = getCLevel((AOCL_LZ4_streamHC_t*)buffer);
    EXPECT_EQ(*cLevel, LZ4HC_CLEVEL_DEFAULT);
    *cLevel = 0;
    for(int i=0; i<ctx_internal_size; i++)
        EXPECT_EQ(hcstate[i],0);
}
/*********************************************
 * "End" of AOCL_LZ4_initStreamHC Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_sizeofStateHC Test
 *********************************************/

TEST(LZ4HC_LZ4_sizeofStateHC, AOCL_Compression_lz4hc_LZ4_sizeofStateHC_pass_common)
{
    EXPECT_EQ(LZ4_sizeofStateHC(), (int)sizeof(LZ4_streamHC_t));
}

/*********************************************
 * "End" of LZ4_sizeofStateHC Test
 *********************************************/

#ifdef AOCL_LZ4HC_OPT

/*********************************************
 * "Begin" of AOCL_LZ4_sizeofStateHC Test
 *********************************************/

TEST(LZ4HC_AOCL_LZ4_sizeofStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_sizeofStateHC_pass_common)
{
    EXPECT_EQ(AOCL_LZ4_sizeofStateHC(), (int)sizeof(AOCL_LZ4_streamHC_t));
}

/*********************************************
 * "End" of AOCL_LZ4_sizeofStateHC Test
 *********************************************/
#endif

/*******************************************************
 * "Begin" of LZ4_compress_HC_extStateHC Tests
 *******************************************************/

class LZ4HC_LZ4_compress_HC_extStateHC :public ::testing::TestWithParam<int>
{
public:
    void SetUp() override {
        aocl_destroy_lz4hc();
        opt_off = GetParam();
        int optLevel = DEFAULT_OPT_LEVEL;
        aocl_setup_lz4hc(opt_off, optLevel, 0, 0, 0);
        is_stream_created = create_stream();
        EXPECT_EQ(is_stream_created, 1); /* Stream created succesfully. */
    }

    virtual int create_stream()
    {
        if (use_AOCL_LZ4_streamHC(opt_off, compression_level))
            strm = AOCL_LZ4_createStreamHC(); 
        else 
            strm = LZ4_createStreamHC();
        EXPECT_NE(strm, nullptr);
        return 1;
    }

    void free_stream() 
    {
        if (use_AOCL_LZ4_streamHC(opt_off, compression_level))
            is_stream_created = AOCL_LZ4_freeStreamHC((AOCL_LZ4_streamHC_t*)strm);
        else
            is_stream_created = LZ4_freeStreamHC((LZ4_streamHC_t*)strm); 
    }

    // Destructor of fixture class `LZ4HC_LZ4_compress_HC_extStateHC`.
    virtual ~LZ4HC_LZ4_compress_HC_extStateHC()
    {
        free_stream();
    }

    void* get_stream() 
    {
        return strm;
    }

    void reset_stream(int _compression_level)
    {
        free_stream();
        compression_level = _compression_level;
        create_stream();
    }

    int get_compression_level()
    {
        return compression_level;
    }

protected:
    int opt_off = -1;               /* Flag to toggle optimizations. */
    void* strm = NULL;              /* Pointer to stream being used. */
    int is_stream_created = -1;     /* 1, if stream created succesfully. */
    int compression_level = LZ4HC_CLEVEL_DEFAULT;

};

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fail_common_1) // statePtr_is_Null
{
    Test_Buffer test_buf(800);
    EXPECT_EQ(LZ4_compress_HC_extStateHC(NULL, test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fail_common_2) // src_NULL_srcLen_not_Null
{
    Test_Buffer test_buf(800);
    EXPECT_EQ(LZ4_compress_HC_extStateHC(get_stream(), NULL /* src */, test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fail_common_3) // dst_NULL_dstLen_not_Null
{
    Test_Buffer test_buf(800);
    EXPECT_EQ(LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), NULL /* dst */, test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_pass_common_4) // srcLen_0
{
    Test_Buffer test_buf(0);
    /* src is not NULL and srcLen is 0. */
    EXPECT_EQ(LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), 0 /* srcSize */, test_buf.getCompressedSize(), get_compression_level()), 1);  // write token (1 Byte)
    /* src is NULL and srcLen is 0. */
    EXPECT_EQ(LZ4_compress_HC_extStateHC(get_stream(), NULL /* src */, test_buf.getCompressedBuff(), 0 /* srcSize */, test_buf.getCompressedSize(), get_compression_level()), 1);  // write token (1 Byte)
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fail_common_5) // Negative_srcSize_and_destSize
{
    Test_Buffer test_buf(800);
    reset_stream(LZ4HC_CLEVEL_MIN);

    /* passing negative value as srcSize and src as not NULL. */
    EXPECT_EQ(LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), -1 /* srcSize */, test_buf.getCompressedSize(), get_compression_level()), 0);

    /* Passing negative value as destSize and dst as not NULL. */
    EXPECT_EQ(LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), -1 /* dstSize */, get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_pass_common_6) // Compression_level_greater_than_maximum_limit
{
    Test_Buffer test_buf(800);
    reset_stream(LZ4HC_CLEVEL_MAX+1);
    int compressedSize = LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_pass_common_7) // Compression_level_less_than_minimum_limit
{
    Test_Buffer test_buf(800);
    reset_stream(-1);
    int compressedSize = LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_pass_common_8) // dstCapacity > LZ4_compressbound()
{
    Test_Buffer test_buf(800);

    int maxDstSize = LZ4_compressBound(test_buf.getOrigSize()) + 1;
    char *dst = (char *)calloc(maxDstSize, sizeof(char));
    int compressedSize = LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), dst, test_buf.getOrigSize(), maxDstSize, get_compression_level());
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), dst, compressedSize));

    if(dst) free(dst);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fail_common_9) // dstCapacity < LZ4_compressbound()
{
    Test_Buffer test_buf(800);
    int maxDstSize = LZ4_compressBound(test_buf.getOrigSize()) / 20;
    char *dst = (char *)calloc(maxDstSize, sizeof(char));

    /*  dstSize < LZ4_compressbound and not enough to store compressed data for this case. 
        Compression Fails.
    */
    int compressedSize = LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), dst, test_buf.getOrigSize(), maxDstSize, get_compression_level());
    EXPECT_EQ(compressedSize, 0);

    if(dst) free(dst);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_pass_common_10) // inputSize < LZ4_minLength 
{
    /*  MFLIMIT = 12
     *  LZ4_minLength = (MFLMIIT + 1)
     *  Testing for input size < LZ4_minLength, i.e, 12
     */
    Test_Buffer test_buf(12);
    int compressedSize = LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());

    /* No compression for input size < LZ4_minLength 
     * Compressed length will be literal length (i.e, 12) + 1 (1 byte of token)
     */
    EXPECT_EQ(compressedSize, 13);          
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_pass_common_11) // simple pass case (all compression levels)
{
    for(int level=0; level<=LZ4HC_CLEVEL_MAX; level++)
    {
        Test_Buffer test_buf(800);
        reset_stream(level);
        int compressedSize = LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
        EXPECT_NE(compressedSize, 0);
    
        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
    }
}

// TEST_P(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_pass_common_12) // Unsupported input size (too large or negative)
// {
//     /* Input size is greater than the maximum acceptable size for API. */
//     Test_Buffer test_buf(LZ4_MAX_INPUT_SIZE + 1);
//     int compressedSize = LZ4_compress_HC_extStateHC(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
    
//     /* No compression */
//     EXPECT_EQ(compressedSize, 0);
// }

INSTANTIATE_TEST_SUITE_P(
    LZ4HC_TEST,
    LZ4HC_LZ4_compress_HC_extStateHC,
    ::testing::Values(1, 0));

/*********************************************
 * "End" of LZ4_compress_HC_extStateHC Tests
 *********************************************/

 /*******************************************************
  * "Begin" of LZ4_compress_HC_extStateHC_fastReset Tests
  *******************************************************/

class LZ4HC_LZ4_compress_HC_extStateHC_fastReset : public LZ4HC_LZ4_compress_HC_extStateHC
{
    public:
    int create_stream() override
    {
        if(use_AOCL_LZ4_streamHC(opt_off, compression_level))
        {
            strm = AOCL_LZ4_createStreamHC();
            AOCL_LZ4_streamHC_t* const ctx = AOCL_LZ4_initStreamHC(strm, sizeof(*ctx));
            if (ctx == NULL) return 0;   /* init failure */
            else return 1;
        }
        else{
            strm = LZ4_createStreamHC();
            LZ4_streamHC_t* const ctx = LZ4_initStreamHC(strm, sizeof(*ctx));
            if (ctx == NULL) return 0;   /* init failure */
            else return 1;
        }
    }

    bool initialize_stream()
    {
        memset(strm, 0, sizeof(strm));
        if (use_AOCL_LZ4_streamHC(opt_off, compression_level))
        {
            AOCL_LZ4_streamHC_t* const ctx = AOCL_LZ4_initStreamHC(strm, sizeof(*ctx));
            if (ctx == NULL) return false;   /* init failure */
            else return true;
        }
        else{
            LZ4_streamHC_t* const ctx = LZ4_initStreamHC(strm, sizeof(*ctx));
            if (ctx == NULL) return false;   /* init failure */
            else return true;
        } 
    }

};

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_fail_common_1) // statePtr_is_Null
{
    Test_Buffer test_buf(800);
    EXPECT_EQ(Test_LZ4_compress_HC_extStateHC_fastReset(NULL, test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_fail_common_2) // src_NULL_srcLen_not_0
{
    Test_Buffer test_buf(800);
    EXPECT_EQ(Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), NULL /* src */, test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_fail_common_3) // dst_NULL_dstLen_not_Null
{
    Test_Buffer test_buf(800);
    EXPECT_EQ(Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), NULL /* dst */, test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_pass_common_4) // srcLen_0
{
    Test_Buffer test_buf(0);
    /* src is not null and srcLen is 0. */
    EXPECT_EQ(Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), 0 /* srcSize */, test_buf.getCompressedSize(), get_compression_level()), 1); // write token (1 Byte)
    /* src is NULL and srcLen is 0. */
    EXPECT_EQ(Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), NULL /* src */, test_buf.getCompressedBuff(), 0 /* srcSize */, test_buf.getCompressedSize(), get_compression_level()), 1);  // write token (1 Byte)
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_fail_common_5) // Negative_srcSize_and_destSize
{
    Test_Buffer test_buf(800);
    reset_stream(LZ4HC_CLEVEL_MIN);

    /* passing negative value as srcSize and src as not NULL. */
    EXPECT_EQ(Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), -1 /* srcSize */, test_buf.getCompressedSize(), get_compression_level()), 0);

    /* Passing negative value as destSize and dst as not NULL. */
    EXPECT_EQ(Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), -1 /* dstSize */, get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_pass_common_6) // Compression_level_greater_than_maximum_limit
{
    Test_Buffer test_buf(800);
    reset_stream(LZ4HC_CLEVEL_MAX + 1);
    int compressedSize = Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_pass_common_7) // Compression_level_less_than_minimum_limit
{
    Test_Buffer test_buf(800);
    reset_stream(-1);
    int compressedSize = Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_pass_common_8) // dstCapacity >= LZ4_compressbound()
{
    Test_Buffer test_buf(800);
    int compressedSize = Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_fail_common_9) // dstCapacity < LZ4_compressbound()
{
    Test_Buffer test_buf(800);
    int maxDstSize = LZ4_compressBound(test_buf.getOrigSize()) / 20;
    char *dst = (char *)calloc(maxDstSize, sizeof(char));
    /*  dstSize < LZ4_compressbound and not enough to store compressed data for this case. 
        Compression Fails.
    */
    int compressedSize = Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), dst, test_buf.getOrigSize(), maxDstSize, get_compression_level());
    EXPECT_EQ(compressedSize, 0);

    if(dst) free(dst);
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_pass_common_10) // inputSize < LZ4_minLength 
{
    /*  MFLIMIT = 12
     *  LZ4_minLength = (MFLMIIT + 1)
     *  Testing for input size < LZ4_minLength, i.e, 12
     */
    Test_Buffer test_buf(12);
    int compressedSize = Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());

    /* No compression for input size < LZ4_minLength 
     * Compressed length will be literal length (i.e, 12) + 1 (1 byte of token)
     */
    EXPECT_EQ(compressedSize, 13);          
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_extStateHC_fastReset, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_fastReset_pass_common_11) // simple pass case (all compression levels)
{
    for(int level=0; level<=LZ4HC_CLEVEL_MAX; level++)
    {
        Test_Buffer test_buf(800);
        reset_stream(level);
        int compressedSize = Test_LZ4_compress_HC_extStateHC_fastReset(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize(), get_compression_level());
        EXPECT_NE(compressedSize, 0);
        

        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
        
        // initialize the stream to avoid initialisation related error. 
        bool is_init_stream = initialize_stream();
        EXPECT_EQ(is_init_stream, true);
    }
}

INSTANTIATE_TEST_SUITE_P(
    LZ4HC_TEST,
    LZ4HC_LZ4_compress_HC_extStateHC_fastReset,
    ::testing::Values(1, 0));

/***************************************************************
 * "End" of LZ4_compress_HC_extStateHC_fastReset Tests
 ***************************************************************/

/*********************************************
 * "Begin" of LZ4_compress_HC_destSize Tests
 *********************************************/

class LZ4HC_LZ4_compress_HC_destSize : public LZ4HC_LZ4_compress_HC_extStateHC{
};


TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_fail_common_1) // stateHC_NULL
{
    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_destSize(NULL /* stateHC */, test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_fail_common_2) // src_NULL
{
    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), NULL /* src */, test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_fail_common_3) // dst_NULL
{
    
    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), NULL /* dst */, &srcLen, test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_pass_common_4) // srcLen is 0
{
    Test_Buffer test_buf(800);
    int srcLen = 0;
    /* src is not NULL and srcLen is 0. */
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level()), 1);
    /* src is NULL and srcLen is 0. */
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), NULL /* src */, test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level()), 1);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_fail_common_5) // srcSizePtr_is_NULL
{
    Test_Buffer test_buf(800);
    /* src is not NULL and srcSizePtr is NULL. */
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), NULL /* srcSizePtr */, test_buf.getCompressedSize(), get_compression_level()), 0);
    /* src is NULL and srcSizePtr is NULL. */
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), NULL /* src */, test_buf.getCompressedBuff(), NULL /* srcSizePtr */, test_buf.getCompressedSize(), get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_fail_common_6) // dstLen_0
{
    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, 0 /* targetDstSize */, get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_fail_common_7) // Negative_srcSize_and_destSize
{
    Test_Buffer test_buf(800);
    /* passing negative value as srcSize and src as not NULL. */
    int srcLen = -1;
    reset_stream(LZ4HC_CLEVEL_MIN);
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level()), 0);

    /* Passing negative value as destSize and dst as not NULL. */
    srcLen = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, -1 /* dstSize */, get_compression_level()), 0);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_pass_common_8) // Compression_level_greater_than_maximum_limit
{

    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    reset_stream(LZ4HC_CLEVEL_MAX+1);
    int compressedLen = LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level());
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(),srcLen,test_buf.getCompressedBuff(),compressedLen));
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_pass_common_9) // Compression_level_less_than_minimum_limit
{

    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    reset_stream(-1);
    int compressedLen = LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level());
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(),srcLen,test_buf.getCompressedBuff(),compressedLen));
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_pass_common_10) // Compression succesful when dstCapacity >= LZ4_compressbound()
{
    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    int compressedSize = LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level());
    EXPECT_NE(compressedSize, 0);

    EXPECT_EQ(srcLen, test_buf.getOrigSize()); /* Compressed input data till the end. */

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcLen, test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_pass_common_11) // compression successful when dstCapacity < LZ4_compressbound()
{
    Test_Buffer test_buf(800);
    int srcLen = test_buf.getOrigSize();
    int maxDstSize = LZ4_compressBound(test_buf.getOrigSize()) / 20;
    char *dst = (char *)calloc(maxDstSize, sizeof(char));
    /*  dstSize < LZ4_compressbound and not enough to store compressed data for this case. 
        srcLen will be upadated with the datalen being compressed with the available dstCapacity. 
    */
    int compressedSize = LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), dst, &srcLen, maxDstSize, get_compression_level());
    EXPECT_NE(compressedSize, 0);
    EXPECT_LT(srcLen, test_buf.getOrigSize()); /* Compressed partial input data based on the dstCapacity available. */

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcLen, dst, compressedSize));
    if(dst) free(dst);
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_pass_common_12) // inputSize < LZ4_minLength 
{
    /*  MFLIMIT = 12
     *  LZ4_minLength = (MFLMIIT + 1)
     *  Testing for input size < LZ4_minLength, i.e, 12
     */
    Test_Buffer test_buf(12);
    int srcLen = test_buf.getOrigSize();
    int compressedSize = LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level());

    /* No compression for input size < LZ4_minLength 
     * Compressed length will be literal length (i.e, 12) + 1 (1 byte of token)
     */
    EXPECT_EQ(compressedSize, 13);          
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcLen, test_buf.getCompressedBuff(), compressedSize));
}

TEST_P(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_pass_common_13) // simple pass case (all compression levels)
{
    for(int level=0; level<=LZ4HC_CLEVEL_MAX; level++)
    {
        Test_Buffer test_buf(800);
        int srcLen = test_buf.getOrigSize();
        reset_stream(level);
        int compressedSize = LZ4_compress_HC_destSize(get_stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcLen, test_buf.getCompressedSize(), get_compression_level());
        EXPECT_NE(compressedSize, 0);

        EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedSize));
    }
}

INSTANTIATE_TEST_SUITE_P(
    LZ4HC_TEST,
    LZ4HC_LZ4_compress_HC_destSize,
    ::testing::Values(1, 0));

/*********************************************
 * "End" of LZ4_compress_HC_destSize Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_createStreamHC Test
 *********************************************/
TEST(LZ4HC_LZ4_createStreamHC, AOCL_Compression_lz4hc_LZ4_createStreamHC_pass_common) // LZ4_freeStreamHC
{
    Stream State_stream(0);       /* created using API. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    EXPECT_EQ(LZ4_freeStreamHC(State_stream.Get_Stream()), 0);
    State_stream.Set_Stream(NULL);
}
/*********************************************
 * "End" of LZ4_createStreamHC Test
 *********************************************/

#ifdef AOCL_LZ4HC_OPT
 /*********************************************
  * "Begin" of AOCL_LZ4_createStreamHC Test
  *********************************************/

TEST(LZ4HC_AOCL_LZ4_createStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_createStreamHC_pass_common) // AOCL_LZ4_freeStreamHC
{
    AOCL_Stream State_stream(0);       /* created using API. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    EXPECT_EQ(AOCL_LZ4_freeStreamHC(State_stream.Get_Stream()), 0);
    State_stream.Set_Stream(NULL);
}

/*********************************************
 * "End" of AOCL_LZ4_createStreamHC Test
 *********************************************/
#endif

/*********************************************
 * "Begin" of LZ4_freeStreamHC Tests
 *********************************************/

TEST(LZ4HC_LZ4_freeStreamHC, AOCL_Compression_lz4hc_LZ4_freeStreamHC_fail_common_1) // NULL_ptr
{
    EXPECT_EQ(LZ4_freeStreamHC(NULL), 0);
}

TEST(LZ4HC_LZ4_freeStreamHC, AOCL_Compression_lz4hc_LZ4_freeStreamHC_pass_common_2) // free_legitimate_stream
{
    Stream State_stream(0);       /* created using API. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    EXPECT_EQ(LZ4_freeStreamHC(State_stream.Get_Stream()), 0);
    State_stream.Set_Stream(NULL);
}

/*********************************************
 * "End" of LZ4_freeStreamHC Tests
 *********************************************/

#ifdef AOCL_LZ4HC_OPT
/*********************************************
 * "Begin" of AOCL_LZ4_freeStreamHC Tests
 *********************************************/

TEST(LZ4HC_AOCL_LZ4_freeStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_freeStreamHC_fail_common_1) // NULL_ptr
{
    EXPECT_EQ(AOCL_LZ4_freeStreamHC(NULL), 0);
}

TEST(LZ4HC_AOCL_LZ4_freeStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_freeStreamHC_pass_common_2) // free_legitimate_stream
{
    AOCL_Stream State_stream(0);       /* created using API. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    EXPECT_EQ(AOCL_LZ4_freeStreamHC(State_stream.Get_Stream()), 0);
    State_stream.Set_Stream(NULL);
}
#endif

/*********************************************
 * "End" of AOCL_LZ4_freeStreamHC Tests
 *********************************************/


/*********************************************
 * "Begin" of LZ4_resetStreamHC_fast Tests
 *********************************************/

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_pass_common_1) // Compression_level_less_than_minimum
{
    Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = -1;

    LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 9);
}

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_pass_common_2) // Compression_level_greater_than_maximum
{
    Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = LZ4HC_CLEVEL_MAX+1;
    LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 12);
}

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_pass_common_3) // Legitimate_ptr
{
    Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = 9;
    LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 9);
}

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_pass_common_4) // LZ4_createStreamHC
{
    Stream State_stream(0);       /* created using API. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = 7;
    LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 7);
}

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_fail_common_5) // input_NULL
{
    Stream State_stream;       /* created NULL stream */
    LZ4_resetStreamHC_fast(State_stream.Get_Stream(), 9);
    EXPECT_EQ(State_stream.Get_Stream(), nullptr);
}

/*********************************************
 * "End" of LZ4_resetStream_fast Tests
 *********************************************/

#ifdef AOCL_LZ4HC_OPT
 /*********************************************
  * "Begin" of AOCL_LZ4_resetStreamHC_fast Tests
  *********************************************/

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_pass_common_1) // Compression_level_less_than_minimum
{
    AOCL_Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = -1;

    AOCL_LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 9);
}

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_pass_common_2) // Compression_level_greater_than_maximum
{
    AOCL_Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = LZ4HC_CLEVEL_MAX+1;
    AOCL_LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 12);
}

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_pass_common_3) // Legitimate_ptr
{
    AOCL_Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = 9;
    AOCL_LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 9);
}

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_pass_common_4) // LZ4_createStreamHC
{
    AOCL_Stream State_stream(0);       /* created using API. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = 7;
    AOCL_LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 7);
}

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_pass_common_5) // State_stream->internal_donotuse.dirty set to 1.
{
    AOCL_Stream State_stream(0);       /* created using API. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    State_stream.set_stream_Internal_dirty();
    int compressionLevel = 7;
    AOCL_LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 7);
}

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_fail_common_6) // input_NULL
{
    AOCL_Stream State_stream;       /* created NULL stream. */
    AOCL_LZ4_resetStreamHC_fast(State_stream.Get_Stream(), 9);
    EXPECT_EQ(State_stream.Get_Stream(), nullptr);
}

/*********************************************
 * "End" of AOCL_LZ4_resetStream_fast Tests
 *********************************************/
#endif

/*********************************************
 * "Begin" of LZ4_loadDictHC Tests
 *********************************************/

class LZ4HC_LZ4_loadDictHC : public ::testing::Test
{
protected:

    // For the function `LZ4_loadDictHC`, `stream` is a parameter.
    LZ4_streamHC_t *stream = NULL;
    LZ4HC_CCtx_internal* ctxPtr = NULL;

    // Initialization of stream variable takes place at start of each test case.
    void SetUp() override
    {
        stream = LZ4_createStreamHC();
    }

    void set_ctx_to_stream_Internal(LZ4_streamHC_t* stream)
    {
        ctxPtr = &stream->internal_donotuse;
    }

    const LZ4_byte* get_ctx_dictStart(LZ4HC_CCtx_internal* ctxPtr)
    {
        return ctxPtr->dictStart;
    }

    const LZ4_byte* get_ctx_End(LZ4HC_CCtx_internal* ctxPtr)
    {
        return ctxPtr->end;
    }

    // Destructor function of `LZ4HC_LZ4_loadDictHC`.
    ~LZ4HC_LZ4_loadDictHC()
    {
        LZ4_freeStreamHC(stream);
    }
};

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_fail_common_1) // stream_NULL
{
    char dict[11] = "helloWorld";
    EXPECT_EQ(LZ4_loadDictHC(NULL, dict, 10), 0);
}

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_fail_common_2) // dictionary_NULL
{
    int dictSize = 66000;
    EXPECT_EQ(LZ4_loadDictHC(stream, NULL, dictSize), 0);
}

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_pass_common_3) // dictSize_<_64KB
{
    set_ctx_to_stream_Internal(stream);
    int dictSize = 6553;
    char *dict = (char *)malloc(dictSize);
    // Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    
    EXPECT_EQ(LZ4_loadDictHC(stream, dict, dictSize), 6553);
    EXPECT_EQ(get_ctx_dictStart(ctxPtr), (unsigned char *)dict);
    EXPECT_EQ(get_ctx_End(ctxPtr), (unsigned char*)dict + dictSize);

    free(dict);
}

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_pass_common_4) // dictSize_>_64KB
{
    int dictSize = 65560;
    char *dict = (char *)malloc(dictSize);
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    
    EXPECT_EQ(LZ4_loadDictHC(stream, dict, dictSize), 65536);
    free(dict);
}

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_pass_common_5) // dictSize <= 4
{
    int dictSize = 65560;
    char *dict = (char *)malloc(dictSize);
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }

    dictSize = 3;

    EXPECT_EQ(LZ4_loadDictHC(stream, dict, dictSize), 3);
    free(dict);
}
/*********************************************
 * "End" of LZ4_loadDictHC Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_compress_HC_continue Tests
 *********************************************/

class LZ4HC_LZ4_compress_HC_continue : public AOCL_setup_lz4hc
{
protected:
    // Helper varibles used in test suites.
    // Initialize below using member functions.
    LZ4HC_CCtx_internal *d = NULL;
    LZ4HC_CCtx_internal ctx;

    void set_statePtr_dictCtx(LZ4_streamHC_t* State)
    {
        State->internal_donotuse.dictCtx = (LZ4HC_CCtx_internal *)malloc(sizeof(LZ4HC_CCtx_internal));
    }

    // setting a pointer to statePtr's dictCtx 
    void set_ptr_to_Internal_dictCtx(LZ4_streamHC_t* State)
    {
        d = (LZ4HC_CCtx_internal *)State->internal_donotuse.dictCtx;
    }

    // size of LZ4HC_CCtx_internal
    int SizeOf_LZ4HC_CCtx_internal()
    {
        return sizeof(LZ4HC_CCtx_internal);
    }

    void initialize_ctx_and_dictBase(LZ4_streamHC_t* state, const LZ4_byte* dict)
    {
        ctx = state->internal_donotuse;
        ctx.dictStart = dict;
    }
};

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_fail_common_1) // stateHCPtr_NULL
{
    Test_Buffer test_buf(1024);
    EXPECT_EQ(LZ4_compress_HC_continue(NULL /* stateHCPtr */, test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize()), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_fail_common_2) // source_NULL
{
    Test_Buffer test_buf(1024);
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_continue(statePtr.Get_Stream(), NULL /* src */, test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize()), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_fail_common_3) // dest_NULL
{
    Test_Buffer test_buf(1024);
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_continue(statePtr.Get_Stream(), test_buf.getOrigData(), NULL /* dst */, test_buf.getOrigSize(), test_buf.getCompressedSize()), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_4) // input_sz1
{
    Test_Buffer test_buf(1024);
    Stream statePtr(0);
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_5) // input_sz2
{
    Test_Buffer test_buf(5120);
    Stream statePtr(0);
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_6) // prefix_mode_1
{
    Test_Buffer test_buf(100);
    Stream statePtr(0);

    int dictSize = 100;
    char dict[200];
    memcpy(&dict[100], test_buf.getOrigData(), 100);
    
    for (int i = 0; i < 100; i++)
    {
        dict[i] = i % 255;
    }
    
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is less than 64kb
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), &dict[100], test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[100], test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_7) // prefix_mode_2
{
    Test_Buffer test_buf(100);
    Stream statePtr(0);

    const int dictSize = 64 * 1024;
    char* dict = (char*)malloc((dictSize + test_buf.getOrigSize()) * sizeof(char));
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    memcpy(&dict[dictSize],test_buf.getOrigData(),test_buf.getOrigSize());
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is equal to 64kb
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), &dict[dictSize], test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[dictSize], test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));

    free(d);
    free(dict);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_8) // using_LoadDictHC_less_64kb
{
    Test_Buffer test_buf(100);
    const int dictSize = 100;
    char dict[dictSize];
    Stream statePtr(0);

    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_9) // using_LoadDictHC_64kb
{
    Test_Buffer test_buf(100);
    const int dictSize = 64 * 1024;
    char dict[dictSize];
    Stream statePtr(0);

    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_10) // external_dictionary_mode
{
    Test_Buffer test_buf(100);
    const int dictSize = 100;
    char dict[dictSize];
    Stream statePtr(0);

    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    
    EXPECT_NE(compressedLen, 0);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));

    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_11) // external_dictionary_mode_64kb_dict_size
{
    Test_Buffer test_buf(100);
    Stream statePtr(0);

    const int dictSize = 64 * 1024;
    char dict[dictSize];
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());

    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));

    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_12) // overlapping_source_dict
{
    Test_Buffer test_buf(150);
    Stream statePtr(0);

    const int dictSize = 100;
    char dict[200] = {0};
    
    memcpy(dict, test_buf.getOrigData(), 100);
    for (int i = 150; i < 200; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), dict, test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_pass_common_13) // overlap_greater_than_64kb
{
    Test_Buffer test_buf(150);
    Stream statePtr(0);
    const int dictSize = 65 * 1024;
    char dict[150 + dictSize] = {0};
    
    memcpy(dict, test_buf.getOrigData(), test_buf.getOrigSize());
    for (int i = 150; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);

    // initialise ctx pointer to statePtr->internal_donotuse
    initialize_ctx_and_dictBase(statePtr.Get_Stream(), (const LZ4_byte *)&dict[100]);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), dict, test_buf.getCompressedBuff(), test_buf.getOrigSize(), test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, test_buf.getOrigSize(), test_buf.getCompressedBuff(), compressedLen));
}

/*********************************************
 * "End" of LZ4_compress_HC_continue Tests
 *********************************************/

/*************************************************
 * "Begin" of LZ4_compress_HC_extStateHC Tests
 *************************************************/

class LZ4HC_LZ4_compress_HC_continue_destSize : public LZ4HC_LZ4_compress_HC_continue
{
protected:
    int srcSize = 0;
};

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_fail_common_1) // statePtr_NULL
{
    Test_Buffer test_buf(1024);
    Stream statePtr(0);
    srcSize = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_continue_destSize(NULL, test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize()), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_fail_common_2) // source_NULL
{
    Test_Buffer test_buf(1024);
    Stream statePtr(0);
    srcSize = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), NULL, test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize()), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_fail_common_3) // dest_NULL
{
    Test_Buffer test_buf(1024);
    Stream statePtr(0);
    srcSize = test_buf.getOrigSize();
    EXPECT_EQ(LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), test_buf.getOrigData(), NULL, &srcSize, test_buf.getCompressedSize()), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_4) // input_sz1
{
    Test_Buffer test_buf(1024);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcSize, test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_5) // input_sz2
{
    Test_Buffer test_buf(5120);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcSize, test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_6) // prefix_mode_1
{
    Test_Buffer test_buf(100);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);
    const int dictSize = 100;
    char dict[200];
    memcpy(&dict[100], test_buf.getOrigData(), 100);
    
    for (int i = 0; i < 100; i++)
    {
        dict[i] = i % 255;
    }
    
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is less than 64kb
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), &dict[100], test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[100], srcSize, test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_7) // prefix_mode_2
{
    Test_Buffer test_buf(100);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);
    const int dictSize = 64 * 1024;
    char* dict = (char*)malloc((dictSize + srcSize) * sizeof(char));
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    memcpy(&dict[dictSize],test_buf.getOrigData(),srcSize);
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is equal to 64kb
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), &dict[dictSize], test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[dictSize], srcSize, test_buf.getCompressedBuff(), compressedLen));
    
    free(d);
    free(dict);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_8) // using_LoadDictHC_less_64kb
{
    Test_Buffer test_buf(100);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);
    const int dictSize = 100;
    char dict[dictSize] = { 0 };
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcSize, test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_9) // using_LoadDictHC_64kb
{
    Test_Buffer test_buf(100);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);
    const int dictSize = 64 * 1024;
    char dict[dictSize] = { 0 };
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcSize, test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_10) // external_dictionary_mode
{
    Test_Buffer test_buf(100);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);

    const int dictSize = 100;
    char dict[dictSize] = { 0 };
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    
    EXPECT_NE(compressedLen, 0);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcSize, test_buf.getCompressedBuff(), compressedLen));
    
    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_11) // external_dictionary_mode_64kb_dict_size
{
    Test_Buffer test_buf(100);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);

    const int dictSize = 64 * 1024;
    char dict[dictSize] = { 0 };
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), test_buf.getOrigData(), test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(test_buf.getOrigData(), srcSize, test_buf.getCompressedBuff(), compressedLen));
    
    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_12) // overlapping_source_dict
{
    Test_Buffer test_buf(150);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);

    const int dictSize = 100;
    char dict[200] = { 0 };
    
    memcpy(dict, test_buf.getOrigData(), 100);
    for (int i = 150; i < 200; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), dict, test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, srcSize, test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_13) // overlap_greater_than_64kb
{
    Test_Buffer test_buf(150);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);

    const int dictSize = 65 * 1024;
    char dict[150 + dictSize] = { 0 };
    
    memcpy(dict, test_buf.getOrigData(), srcSize);
    for (int i = 150; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);
    initialize_ctx_and_dictBase(statePtr.Get_Stream(), (const LZ4_byte *)&dict[100]);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), dict, test_buf.getCompressedBuff(), &srcSize, test_buf.getCompressedSize());
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, srcSize, test_buf.getCompressedBuff(), compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_pass_common_14) // destCapacity < LZ4_compressBound(test_buf.getOrigData())
{
    Test_Buffer test_buf(150);
    srcSize = test_buf.getOrigSize();
    Stream statePtr(0);

    int targetDstSize = test_buf.getCompressedSize();
    int dstSize = targetDstSize * 0.75;
    char* dst = (char *)calloc(dstSize, sizeof(char));

    const int dictSize = 65 * 1024;
    char dict[150 + dictSize] = {0};
    
    memcpy(dict, test_buf.getOrigData(), srcSize);
    for (int i = 150; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);
    initialize_ctx_and_dictBase(statePtr.Get_Stream(), (const LZ4_byte *)&dict[100]);
    int previous_srcSize = srcSize;

    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), dict, dst, &srcSize, dstSize);  // srcSize will be updated to new value.
    EXPECT_NE(compressedLen, 0);
    EXPECT_NE(previous_srcSize, srcSize);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
    
    if(dst) free(dst);
}

/*****************************************************
 * "End" of LZ4_continue_HC_continue_destSize Tests
 *****************************************************/

/*********************************************
 * "Begin" of LZ4_saveDictHC Tests
 *********************************************/

/*
    Fixture class for testing `LZ4_saveDictHC` function.
*/
class LZ4HC_LZ4_saveDictHC : public ::testing::Test
{
protected:
    
    LZ4_streamHC_t *stream = NULL;
    char *dict = NULL;
    int dictSize = 0;
    LZ4HC_CCtx_internal *ctx = NULL;
    
    // Initialize a LZ4_streamHC ptr.
    void SetUp() override
    {
        stream = LZ4_createStreamHC();
        LZ4_resetStreamHC_fast(stream, 9);
    }
    
    // Initialize or reset a `dict` buffer.
    int_fast64_t loadDictHC(int sz)
    {
        if(dict)
            free(dict);
        dictSize = sz;
        dict = (char *)malloc(dictSize);
        for(int i=0; i<sz; i++)
        {
            dict[i] = i % 256;
        }
        return LZ4_loadDictHC(stream, dict, dictSize);
    }
    
    // returns pointer to end member of LZ4HC_CCtx_internal
    const LZ4_byte* get_stream_Internal_end()
    {
        LZ4HC_CCtx_internal* internal_p = &(stream->internal_donotuse);
        return internal_p->end;
    }

    void set_ctx_to_stream_Internal(LZ4_streamHC_t* stream)
    {
        ctx = &(stream->internal_donotuse);
    }
    
    const LZ4_byte* get_ctx_end()
    {
        return ctx->end;
    }

    const LZ4_byte* get_ctx_base()
    {
        return ctx->prefixStart;
    }
    
    
    // Destructor function of `LZ4HC_LZ4_saveDictHC` class.
    ~LZ4HC_LZ4_saveDictHC()
    {
        if (dict)
            free(dict);
        if (stream)
            LZ4_freeStreamHC(stream);
    }
};

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_pass_common_1) // dictonary_size_greater_64kb
{
    char *data = (char *)malloc(100000);
    
    for(int i=0; i<100000; i++)
    {
        data[i] = i % 256;
    }

    EXPECT_EQ(loadDictHC(66000),64 * 1024);
    EXPECT_EQ(LZ4_saveDictHC(stream, data, dictSize), 64 * 1024);

    set_ctx_to_stream_Internal(stream);

    EXPECT_EQ(memcmp(get_ctx_end() - 64 * 1024, data, 64 * 1024), 0);

    free(data);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_fail_common_2) // dictionary_and_dictSize_is_zero
{
    EXPECT_EQ(LZ4_saveDictHC(stream, NULL, 0), 0);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_fail_common_3) // stream_NULL
{
    char dict[11]="helloWorld";
    EXPECT_EQ(LZ4_saveDictHC(NULL, dict, 10), 0);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_fail_common_4) // dictionary_is_NULL_dictSize_not_zero
{
    EXPECT_EQ(LZ4_saveDictHC(stream, NULL, 3), 0);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_pass_common_5) // dictionary_size_greater_than_parameter
{
    loadDictHC(10000);
    char *data = (char *)malloc(100000);
    for(int i=0; i<100000; i++)
        data[i]=i;
    
    EXPECT_EQ(LZ4_saveDictHC(stream, data, 100000), 10000);
    EXPECT_EQ(memcmp(get_stream_Internal_end() - 10000 , data, 10000), 0);
    
    free(data);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_fail_common_6) // no_loaded_dictionary
{
    char data[11] = "helloWorld";
    
    EXPECT_EQ(LZ4_saveDictHC(stream, data, 0), 0);
    
    
    set_ctx_to_stream_Internal(stream);
    EXPECT_EQ(get_ctx_end() - get_ctx_base(), 0);
}

/*********************************************
 * "End" of LZ4_saveDictHC Tests
 *********************************************/

#ifdef AOCL_LZ4HC_OPT
/*********************************************
 * "Begin" of AOCL_LZ4HC_countBack Tests
 *********************************************/
class LZ4HC_AOCL_LZ4HC_countBack : public AOCL_setup_lz4hc
{
protected:
    
    LZ4_byte* ip = NULL;;
    LZ4_byte* match = NULL;
    LZ4_byte *iMin = NULL;
    LZ4_byte *mMin = NULL;
    
    // Initialize pointers.
    void SetUp() override
    {
        ip = NULL;
        match = NULL;
        iMin = ip;
        mMin = match;
    }
    
    // Initialize or reset a `ip` buffer.
    LZ4_byte* initialise_ip_string(int sz, const char *c)
    {
        if(ip)
            free(ip);
        ip = (LZ4_byte *)malloc(sz);
        memcpy(ip, c, sz);
        return ip;
    }
    
    // Initialize or reset a `matchPtr` buffer.
    LZ4_byte* initialise_match_string(int sz, const char *c)
    {
        if(match)
            free(match);
        match = (LZ4_byte *)malloc(sz);
        memcpy(match, c, sz);
        return match;
    }
    // Destructor function of `LZ4HC_AOCL_LZ4HC_countBack` class.
    ~LZ4HC_AOCL_LZ4HC_countBack()
    {
        if (ip)
            free(ip);
        if (match)
            free(match);
    }
};

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_1)  // 7 Bytes reverse match
{
    const char *p = "abcdefgh";
    const char *m = "abcdefgh";

    int sz = 8; // size is 8 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_2)  // 7 Bytes reverse match(last byte mismatch)
{
    const char *p = "abcdefgh";
    const char *m = "Abcdefgh";

    int sz = 8; // size is 8 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_3)  // 8 Bytes reverse match
{
    const char *p = "abcdefghi";
    const char *m = "abcdefghi";

    int sz = 9;  // size is 9 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_4)  // 8 Bytes reverse match(last byte mismatch)
{
    const char *p = "abcdefghi";
    const char *m = "Abcdefghi";

    int sz = 9; // size is 9 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_5)  // 9 Bytes reverse match
{
    const char *p = "abcdefghij";
    const char *m = "abcdefghij";

    int sz = 10;  // size is 10 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_6)  // 9 Bytes reverse match(last byte mismatch)
{
    const char *p = "abcdefghij";
    const char *m = "Abcdefghij";

    int sz = 10; // size is 10 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_7)  // 10 Bytes reverse match
{
    const char *p = "abcdefghijk";
    const char *m = "abcdefghijk";

    int sz = 11;  // size is 11 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_8)  // 10 Bytes reverse match(last byte mismatch)
{
    const char *p = "abcdefghijk";
    const char *m = "Abcdefghijk";

    int sz = 11; // size is 11 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), -9);
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_9)  // 11 Bytes reverse match
{
    const char *p = "abcdefghijkl";
    const char *m = "abcdefghijkl";

    int sz = 12;  // size is 12 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_10)  // 11 Bytes reverse match(last byte mismatch)
{
    const char *p = "abcdefghijkl";
    const char *m = "Abcdefghijkl";

    int sz = 12; // size is 12 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_11)  // 12 Bytes reverse match
{
    const char *p = "abcdefghijklm";
    const char *m = "abcdefghijklm";

    int sz = 13;  // size is 13 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_12)  // 12 Bytes reverse match(last byte mismatch)
{
    const char *p = "abcdefghijklm";
    const char *m = "Abcdefghijklm";

    int sz = 13; // size is 12 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_13)  // 13 Bytes reverse match
{
    const char *p = "abcdefghijklmn";
    const char *m = "abcdefghijklmn";

    int sz = 14;  // size is 14 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_14)  // 13 Bytes reverse match(last byte mismatch)
{
    const char *p = "abcdefghijklmn";
    const char *m = "Abcdefghijklmn";

    int sz = 14; // size is 14 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_15)  // long string reverse match to check loop iterations
{
    const char *p = "abcdefghijklmnopqrstuvwx";
    const char *m = "Abcdefghijklmnopqrstuvwx";

    int sz = 24;  // size is 24 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_16)  // iMin < limit
{
    const char *p = "abcdefghijklmnopqrstuvwx";
    const char *m = "abcdefghijklmnopqrstuvwx";

    int sz = 24; // size is 24 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip - 2;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_17)  // mMin < limit
{
    const char *p = "abcdefghijklmnopqrstuvwx";
    const char *m = "abcdefghijklmnopqrstuvwx";

    int sz = 24; // size is 24 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match - 2;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_18)  // mMin != match and iMin != ip
{
    const char *p = "abcdefghijklmnopqrstuvwx";
    const char *m = "abcdefghijklmnopqrstuvwx";

    int sz = 24; // size is 24 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip + 9;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match + 4;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_pass_common_19)  // mMin > match and iMin > ip
{
    const char *p = "abcdefghijklmnopqrstuvwx";
    const char *m = "abcdefghijklmnopqrstuvwx";

    int sz = 14; // size is 14 Bytes
    ip = initialise_ip_string(sz, p);
    iMin = ip + sz + 3;
    LZ4_byte* ip2 = ip + sz - 1;
    match = initialise_match_string(sz, m);
    mMin = match + sz + 4;
    LZ4_byte* match2 = match + sz - 1;
    
    EXPECT_EQ(Test_AOCL_LZ4HC_countBack(ip2, match2, iMin, mMin), Test_LZ4HC_countBack(ip2, match2, iMin, mMin));
}
/*********************************************
 * "End" of AOCL_LZ4HC_countBack Tests
 *********************************************/

 /*********************************************
  * "Begin" of AOCL_LZ4HC_insert Tests
  *********************************************/

class LZ4HC_AOCL_LZ4HC_Insert : public AOCL_setup_lz4hc
{
protected:
    AOCL_LZ4_streamHC_t* const state = AOCL_LZ4_createStreamHC();
    AOCL_LZ4HC_CCtx_internal* hc4 = NULL;

    short compressionLevel = 6;

    // Initialize pointers.
    void SetUp() override
    {
        AOCL_LZ4_streamHC_t* const ctx = AOCL_LZ4_initStreamHC(state, sizeof(*ctx));
        if (ctx != NULL) {
            hc4 = &((AOCL_LZ4_streamHC_t*)state)->internal_donotuse;
            AOCL_LZ4_resetStreamHC_fast((AOCL_LZ4_streamHC_t*)state, this->compressionLevel);
        }
    }


    // Destructor function of `LZ4HC_AOCL_LZ4HC_insert` class.
    ~LZ4HC_AOCL_LZ4HC_Insert()
    {
        AOCL_LZ4_freeStreamHC(state);
    }
};

TEST_F(LZ4HC_AOCL_LZ4HC_Insert, AOCL_Compression_lz4hc_AOCL_LZ4HC_Insert_pass_common_1)  
{
    
    /* Test case to determine hashchain for particular hash value
     * and check latest index matched string is accessed with hcHead. */
    // level 6
    int hash_chain_slot_sz = 16;
    int hash_chain_max = 15;
    
    const char* str = "abcdlmnoabcdklmnabcdabcde";
    LZ4_byte* src = (LZ4_byte*)str;

    Test_AOCL_LZ4HC_init_internal(hc4, src);
    /* Inserting the complete string uptill iHighLimit */
    const LZ4_byte* iHighLimit = src + strlen(str) - 3;
    Test_AOCL_LZ4HC_Insert(hc4, iHighLimit, hash_chain_max, hash_chain_slot_sz);

    /* for sub-string "abcd", determining hashchain block and position in hashChain */
    uint32_t hashIdx = LZ4HC_hashPtr(src+20);
    size_t Hcbase = (size_t)hashIdx * hash_chain_slot_sz;
    size_t hcHeadPos = hc4->chainTable[Hcbase];
    EXPECT_EQ((uint16_t)hc4->chainTable[hcHeadPos], 20);

    /* for sub-string "klmn", determining hashchain block and position in hashChain */
    const char* src_2 = "klmn";
    hashIdx = LZ4HC_hashPtr(src_2);
    Hcbase = (size_t)hashIdx * hash_chain_slot_sz;
    hcHeadPos = hc4->chainTable[Hcbase];
    EXPECT_EQ((uint16_t)hc4->chainTable[hcHeadPos], 12);
}

TEST_F(LZ4HC_AOCL_LZ4HC_Insert, AOCL_Compression_lz4hc_AOCL_LZ4HC_Insert_pass_common_2)
{

    /* Test case to determine hashchain for particular hash value
     * and check latest index matched string in accessed with hcHead
     * when the number of entries are greater than hash chain slot size. */
    // level 7
    int hash_chain_slot_sz = 32;
    int hash_chain_max = 31;

    const char* str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    LZ4_byte* src = (LZ4_byte*)str;

    Test_AOCL_LZ4HC_init_internal(hc4, src);
    /* Inserting the complete string uptill iHighLimit */
    const LZ4_byte* iHighLimit = src + strlen(str) - 3;
    Test_AOCL_LZ4HC_Insert(hc4, iHighLimit, hash_chain_max, hash_chain_slot_sz);

    /* For string "aaaa", checking if hashchain is still accessible even after 31 entries into it.
     * and Rotation is working correctly in hash Chain or not. */
    uint32_t hashIdx = LZ4HC_hashPtr(src + 32);
    size_t Hcbase = (size_t)hashIdx * hash_chain_slot_sz;
    size_t hcHeadPos = hc4->chainTable[Hcbase];
    EXPECT_EQ((uint16_t)hc4->chainTable[hcHeadPos], 32); 
}

/*********************************************
  * "End" of AOCL_LZ4HC_insert Tests
  *********************************************/

/*****************************************************
 * "Begin" of AOCL_LZ4HC_insertAndGetWiderMatch Tests
 *****************************************************/

class LZ4HC_AOCL_LZ4HC_InsertAndGetWiderMatch : public LZ4HC_AOCL_LZ4HC_Insert
{
};

TEST_F(LZ4HC_AOCL_LZ4HC_InsertAndGetWiderMatch, AOCL_Compression_lz4hc_AOCL_LZ4HC_InsertAndGetWiderMatch_pass_common_1)   // Disabled PatternAnslysis
{

    /* Test case to determine hashchain for particular hash value
     * and check latest index matched string in accessed with hcHead. */
     // level 6
    int hash_chain_slot_sz = 16;
    int hash_chain_max = 15;

    const char* str = "ABCDlmnoABCDMlmnABCDabcdeABCDMkbhlm";
    LZ4_byte* src = (LZ4_byte*)str;
    LZ4_byte* ip = src + 25;

    Test_AOCL_LZ4HC_init_internal(hc4, src);
    Test_AOCL_LZ4HC_Insert(hc4, src + 25, hash_chain_max, hash_chain_slot_sz); /* insert upto second occurence of sub-string "ABCDM" */

    const LZ4_byte* ref = NULL;
    const LZ4_byte* startpos = ip;
    int longest = 3;

    /* setting iHighLimit to ensure that pointer does not read memory beyond limit during byte comparison. */
    const LZ4_byte* iHighLimit = src + strlen(str) - 3;

    int result = Test_AOCL_LZ4HC_InsertAndGetWiderMatch(hc4, ip, ip, iHighLimit, longest, &ref, &startpos, 
        32 /* maxNbAttempts */, 0 /* patternAnalysis */, 0 /*chainSwap*/, noDictCtx, favorCompressionRatio, hash_chain_max, hash_chain_slot_sz);
    EXPECT_EQ(result, 5);             /* length of match */
    EXPECT_EQ(ref, src + 8);          /* ref is match position */
    EXPECT_EQ(startpos, src + 25);    /* startpos is the point in the string for which match is searched. */
}

TEST_F(LZ4HC_AOCL_LZ4HC_InsertAndGetWiderMatch, AOCL_Compression_lz4hc_AOCL_LZ4HC_InsertAndGetWiderMatch_pass_common_2) // Enabled Pattern Analysis
{

    /* Test case to determine hashchain for particular hash value
     * and check latest index matched string in accessed with hcHead. */
     // level 6
    int hash_chain_slot_sz = 16;
    int hash_chain_max = 15;

    const char* str = "aaaaaaaaaaaaaaaaaaaEDCBaaaaaaaaaaaa123";
    LZ4_byte* src = (LZ4_byte*)str;
    LZ4_byte* ip = (LZ4_byte*)str + 23;

    Test_AOCL_LZ4HC_init_internal(hc4, src);
    Test_AOCL_LZ4HC_Insert(hc4, src + 23, hash_chain_max, hash_chain_slot_sz); /* insert till the occurence of sub-string EDCA */

    const LZ4_byte* ref = NULL;
    const LZ4_byte* startpos = ip;
    int longest = 3;

    /* setting iHighLimit to ensure that pointer does not read memory beyond limit during byte comparison. */
    const LZ4_byte *iHighLimit = src + strlen(str) - 3; 
    
    int result = Test_AOCL_LZ4HC_InsertAndGetWiderMatch(hc4, ip, ip, iHighLimit, longest, &ref, &startpos,
        32 /* maxNbAttempts */, 1 /* patternAnalysis */, 0 /*chainSwap*/, noDictCtx, favorCompressionRatio, hash_chain_max, hash_chain_slot_sz);
    EXPECT_EQ(result, 12);            /* length of match */
    EXPECT_EQ(ref, src + 7);          /* ref is match position */
    EXPECT_EQ(startpos, src + 23);    /*startpos is the point in the string for which match is searched. */
}

/*****************************************************
 * "End" of AOCL_LZ4HC_insertAndGetWiderMatch Tests
 *****************************************************/
#endif

/*********************************************
 * Begin fuzz tests for lz4hc
 *********************************************/
#ifdef AOCL_TEST_FUZZER
void LZ4_compress_HC_fuzz(vector<char> source, size_t dest_sz,
                          int level, int optOff, int optLevel)
{
  aocl_setup_lz4hc(optOff, optLevel, 0, 0, 0);

  int destLen = dest_sz > INT_MAX ? INT_MAX : dest_sz;
  int srcLen = source.size();
  vector<char> dest(destLen, 0);

  LZ4_compress_HC((const char*)source.data(), dest.data(), srcLen, destLen, level);

  aocl_destroy_lz4hc();
}
FUZZ_TEST(AOCL_Compression_lz4hc, LZ4_compress_HC_fuzz)
    .WithDomains(fuzztest::Arbitrary<vector<char>>(),
                fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
                fuzztest::InRange<int>(-1, 12),
                fuzztest::InRange<int>(0, 1),
                fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
    .WithSeeds([]() -> fuzz_cpr_seed_t<char> {
    auto seed_files = READ_FUZZ_CPR_SEED();
    return get_fuzz_cpr_seeds<char>([](size_t src_sz) -> size_t {
        size_t dst_sz = (size_t)LZ4_compressBound((int)src_sz);
        return limit_fuzz_size_max(dst_sz);
    }, -1, 12, seed_files);
})
#endif
;

static void* setup_LZ4HC_stream(int optOff, int compressionLevel, int optLevel) 
{ 
    aocl_setup_lz4hc(optOff, optLevel, 0, 0,0); 
    void *stream = NULL;    
    if(optOff || compressionLevel<6 || compressionLevel>9)
        stream = LZ4_createStreamHC();       
    else 
        stream = AOCL_LZ4_createStreamHC();
    return stream; 
}

static void destroy_LZ4HC_stream(void* stream, int optOff, int compressionLevel)
{
    if(optOff || compressionLevel<6 || compressionLevel>9)
        LZ4_freeStreamHC((LZ4_streamHC_t*)stream);
    else 
        AOCL_LZ4_freeStreamHC((AOCL_LZ4_streamHC_t*)stream);  
    aocl_destroy_lz4hc();
}

void LZ4_compress_HC_continue_fuzz(vector<char> src, int out_len)
{
    vector<char> dst(out_len);
    LZ4_streamHC_t* stream = LZ4_createStreamHC();   
    LZ4_compress_HC_continue (stream, src.data(), dst.data(), src.size(), out_len);
    LZ4_freeStreamHC(stream);
}
FUZZ_TEST(AOCL_Compression_lz4hc, LZ4_compress_HC_continue_fuzz)
    .WithDomains(fuzztest::Arbitrary<vector<char>>(),
                 fuzztest::InRange<int>(1, 10000));

void LZ4_compress_HC_continue_destSize_fuzz(vector<char> src, int out_len)
{
    int src_size = src.size();     
    vector<char> dst(out_len);
    LZ4_streamHC_t* stream = LZ4_createStreamHC();    
    LZ4_compress_HC_continue_destSize(stream,  src.data(), dst.data(),&src_size, out_len);
    LZ4_freeStreamHC(stream);
}
FUZZ_TEST(AOCL_Compression_lz4hc, LZ4_compress_HC_continue_destSize_fuzz)
    .WithDomains(fuzztest::Arbitrary<vector<char>>(),
                 fuzztest::InRange<int>(1, 10000));

void LZ4_compress_HC_extStateHC_fuzz(vector<char> src, int out_len, int compressionLevel, int optOff, int optLevel)
{
    vector<char> dst(out_len);
    void *stream = setup_LZ4HC_stream(optOff, compressionLevel, optLevel);
    LZ4_compress_HC_extStateHC(stream, src.data(), dst.data(), src.size(), out_len, compressionLevel);
    destroy_LZ4HC_stream(stream, optOff,compressionLevel);
}
FUZZ_TEST(AOCL_Compression_lz4hc, LZ4_compress_HC_extStateHC_fuzz)
    .WithDomains(fuzztest::Arbitrary<vector<char>>(),
                fuzztest::InRange<int>(1, 10000),
                fuzztest::InRange<int>(-1, 13),
                fuzztest::InRange<int>(0, 1),
                fuzztest::InRange<int>(0, 4));

void LZ4_compress_HC_destSize_fuzz(vector<char> src, int out_len, int compressionLevel, int optOff, int optLevel)
{    
    int srcSize = src.size();
    vector<char> dst(out_len);
    void *stream = setup_LZ4HC_stream(optOff, compressionLevel, optLevel);
    LZ4_compress_HC_destSize(stream, src.data(), dst.data(), &srcSize, out_len, compressionLevel);
    destroy_LZ4HC_stream(stream, optOff,compressionLevel);
}
FUZZ_TEST(AOCL_Compression_lz4hc, LZ4_compress_HC_destSize_fuzz)
    .WithDomains(fuzztest::Arbitrary<vector<char>>(),
                fuzztest::InRange<int>(1, 10000),
                fuzztest::InRange<int>(-1, 13),
                fuzztest::InRange<int>(0, 1),
                fuzztest::InRange<int>(0, 4));

#endif /* AOCL_TEST_FUZZER */
/*********************************************
 * End fuzz tests for lz4hc
 *********************************************/
