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

#include "algos/lz4/lz4.h"
#include "algos/lz4/lz4hc.h"

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

class TestLoad_1
{
    //source buffer (original data which we intend to compress).
    char *orig_data = NULL;
    size_t orig_sz = 0;
    //destination buffer (kdata obtained after compression).
    char *compressed_data = NULL;
    size_t compressed_sz=0;

public:
    // Constructor functions, creates `sz` size of source data.
    TestLoad_1(int sz)
    {
        this->orig_sz = sz;
        orig_data = (char *)malloc(sz);

        // generating random data inside `orig_data` buffer.
        for (int i = 0; i < sz; i++)
        {
            orig_data[i] = rand() % 255;
        }

        // Provides the maximum size that LZ4/LZ4HC compression may output in a "worst case".
        compressed_sz = LZ4_compressBound(sz);
        compressed_data = (char *)malloc(compressed_sz);
    }
    // Returns pointer to source buffer.
    char *getOrigData()
    {
        return orig_data;
    }
    // Returns size of source buffer.
    size_t getOrigSize()
    {
        return orig_sz;
    }
    // Returns pointer to destination buffer (data obtained after compression).
    char *getCompressedBuff()
    {
        return compressed_data;
    }
    // Returns size of destination data.
    size_t getCompressedSize()
    {
        return compressed_sz;
    }
    // Destructor function.
    ~TestLoad_1()
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
        return stream->internal_donotuse.base;
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
        return AOCL_stream->internal_donotuse.base;
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
class LZ4HC_LZ4_compress_HC : public AOCL_setup_lz4hc {
};

TEST_F(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_common_1) // compress_FAIL_src_is_NULL
{
    TestLoad_1 d(800);
    EXPECT_EQ(LZ4_compress_HC(NULL, d.getCompressedBuff(), d.getOrigSize(),d.getCompressedSize(), 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_common_2) // Compress_FAIL_dst_is_NULL
{
    TestLoad_1 d(800);
    EXPECT_EQ(LZ4_compress_HC(d.getOrigData(), NULL, d.getOrigSize(),d.getCompressedSize(), 1), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_common_3 ) // compress_PASS
{
    TestLoad_1 d(800);
    int outLen = LZ4_compress_HC(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(),d.getCompressedSize(), 1);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

TEST_F(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_common_4) // compress_FAIL_dst_size_not_enough
{
    TestLoad_1 d(800);
    int outLen = LZ4_compress_HC(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(),d.getOrigSize() / 20, 1);
    EXPECT_FALSE(lz4hc_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

TEST_F(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_common_5) // Compression_level_less_than_minimum_limit
{
    TestLoad_1 d(800);
    int outLen = LZ4_compress_HC(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(),d.getCompressedSize(), -1);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

TEST_F(LZ4HC_LZ4_compress_HC, AOCL_Compression_lz4hc_LZ4_compress_HC_common_6) // Compression_level_greater_than_maximum_limit
{
    TestLoad_1 d(800);
    int outLen = LZ4_compress_HC(d.getOrigData(), d.getCompressedBuff(), d.getOrigSize(),d.getCompressedSize(), 13);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen));
}

/*********************************************
 * "End" of LZ4_compress_HC Tests
 *********************************************/


/*********************************************
 * "Begin" of LZ4_sizeofStateHC Test
 *********************************************/

TEST(LZ4HC_LZ4_sizeofStateHC, AOCL_Compression_lz4hc_LZ4_sizeofStateHC_common)
{
    EXPECT_EQ(LZ4_sizeofStateHC(), 262200);  
}

/*********************************************
 * "End" of LZ4_sizeofStateHC Test
 *********************************************/

#ifdef AOCL_LZ4HC_OPT
 /*********************************************
  * "Begin" of AOCL_LZ4_sizeofStateHC Test
  *********************************************/

TEST(LZ4HC_AOCL_LZ4_sizeofStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_sizeofStateHC_common)
{
    EXPECT_EQ(AOCL_LZ4_sizeofStateHC(), 16908344);
}

/*********************************************
 * "End" of AOCL_LZ4_sizeofStateHC Test
 *********************************************/
#endif

/*************************************************
 * "Begin" of LZ4_compress_HC_extStateHC Tests
 *************************************************/

class LZ4HC_LZ4_compress_HC_extStateHC : public AOCL_setup_lz4hc
{

protected:

    char *src = NULL;
    int srcSize= 0;
    char *dst = NULL;
    int dstSize =0;
    // For the function `LZ4_compress_HC_extStateHC`, `statePtr` is a parameter, declared in test cases. 

    // Initialize or reset `src` buffer.
    void setSrc(int sz)
    {
        if(src)
            free(src);
        srcSize = sz;
        src = (char *)malloc(srcSize);
        for (int i = 0; i < srcSize; i++)
        {
            src[i] = rand() % 255;
        }
    }

    // Initialize or reset `dst` buffer.
    void setDst(int sz)
    {
        if(dst)
            free(dst);
        dstSize = sz;
        dst = (char *)malloc(dstSize);
    }

    // Destructor function of `LLZ4_compress_HC_extStateHC`.
    ~LZ4HC_LZ4_compress_HC_extStateHC()
    {
        if(dst)
            free(dst);
        if(src)
            free(src);
    }
};

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_1) // statePtr_is_Null
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    EXPECT_EQ(LZ4_compress_HC_extStateHC(NULL, src, dst, srcSize, dstSize, 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_2) // src_NULL_srcLen_not_Null
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), NULL, dst, srcSize, dstSize, 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_3) // dst_NULL_dstLen_not_Null
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, NULL, srcSize, dstSize, 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_4) // src_not_NULL_srcLen_0
{
    setSrc(0);
    setDst(LZ4_compressBound(srcSize));
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, 0, dstSize, 9), 1);  // write token (1 Byte)
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_5) // Compression_level_is_greater_than_max
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    Stream statePtr(0);
    int compressedSize = LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, 13);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_6) // Compression_level_is_less_than_min
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    Stream statePtr(0);
    int compressedSize = LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, -1);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}


TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_7) // if_coverage1
{
    setSrc(60000);
    setDst(LZ4_compressBound(srcSize));
    Stream statePtr(0);
    int compressedSize = LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, 9);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_8) // if_coverage2
{
    setSrc(70000);
    setDst(LZ4_compressBound(srcSize));
    Stream statePtr(0);
    int compressedSize = LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, 9);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_9) // if_coverage3
{
    setSrc(7000);
    setDst(7000);
    Stream statePtr(0);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    int compressedSize = LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, 9);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

TEST_F(LZ4HC_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_LZ4_compress_HC_extStateHC_common_10) // if_coverage4
{
    setSrc(70000);
    setDst(70000);
    Stream statePtr(0);
    
    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }
    
    int compressedSize = LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, 9);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src,srcSize,dst,compressedSize));
}

/*********************************************
 * "End" of LZ4_compress_HC_extStateHC Tests
 *********************************************/

#ifdef AOCL_LZ4HC_OPT
 /*************************************************
  * "Begin" of AOCL_LZ4_compress_HC_extStateHC Tests
  *************************************************/

class LZ4HC_AOCL_LZ4_compress_HC_extStateHC : public AOCL_setup_lz4hc
{

protected:

    char* src = NULL;
    int srcSize = 0;
    char* dst = NULL;
    int dstSize = 0;
    // For the function `LZ4_compress_HC_extStateHC`, `statePtr` is a parameter, declared in test cases. 

    // Initialize or reset `src` buffer.
    void setSrc(int sz)
    {
        if (src)
            free(src);
        srcSize = sz;
        src = (char*)malloc(srcSize);
        for (int i = 0; i < srcSize; i++)
        {
            src[i] = rand() % 255;
        }
    }

    // Initialize or reset `dst` buffer.
    void setDst(int sz)
    {
        if (dst)
            free(dst);
        dstSize = sz;
        dst = (char*)malloc(dstSize);
    }

    // Destructor function of `LLZ4_compress_HC_extStateHC`.
    ~LZ4HC_AOCL_LZ4_compress_HC_extStateHC()
    {
        if (dst)
            free(dst);
        if (src)
            free(src);
    }
};

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_1) // statePtr_is_Null
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    EXPECT_EQ(AOCL_LZ4_compress_HC_extStateHC(NULL, src, dst, srcSize, dstSize, 9), 0);
}

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_2) // src_NULL_srcLen_not_Null
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    AOCL_Stream statePtr(0);
    EXPECT_EQ(AOCL_LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), NULL, dst, srcSize, dstSize, 9), 0);
}

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_3) // dst_NULL_dstLen_not_Null
{
    setSrc(100000);
    setDst(LZ4_compressBound(srcSize));
    AOCL_Stream statePtr(0);
    EXPECT_EQ(AOCL_LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, NULL, srcSize, dstSize, 9), 0);
}

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_4) // src_not_NULL_srcLen_0
{
    setSrc(0);
    setDst(LZ4_compressBound(srcSize));
    AOCL_Stream statePtr(0);
    EXPECT_EQ(AOCL_LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, 0, dstSize, 9), 1);  // write token (1 Byte)
}

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_5) // Coverage for level 5 i.e, less than allowed levels
{
    setSrc(60000);
    setDst(LZ4_compressBound(srcSize));
    AOCL_Stream statePtr(0);
    int cLevel = 5;
    int compressedSize = AOCL_LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, cLevel);
    EXPECT_EQ(compressedSize, 0);

}

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_6) // Coverage for level 6 i.e, minimum allowed level
{
    setSrc(70000);
    setDst(LZ4_compressBound(srcSize));
    AOCL_Stream statePtr(0);
    int cLevel = 6;
    int compressedSize = AOCL_LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, cLevel);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedSize));
}

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_7) // Coverage for level 9 i.e, maximum allowed level
{
    setSrc(7000);
    setDst(7000);
    AOCL_Stream statePtr(0);
    int cLevel = 9;

    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }

    int compressedSize = AOCL_LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, cLevel);
    EXPECT_NE(compressedSize, 0);

    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedSize));
}

TEST_F(LZ4HC_AOCL_LZ4_compress_HC_extStateHC, AOCL_Compression_lz4hc_AOCL_LZ4_compress_HC_extStateHC_common_8) // Coverage for level 10, i.e, level greater than allowed levels
{
    setSrc(70000);
    setDst(70000);
    AOCL_Stream statePtr(0);
    int cLevel = 10;

    for (int i = 0; i < srcSize; i++)
    {
        src[i] = 100;
    }

    int compressedSize = AOCL_LZ4_compress_HC_extStateHC(statePtr.Get_Stream(), src, dst, srcSize, dstSize, cLevel);
    EXPECT_EQ(compressedSize, 0);
}

/*************************************************
 * "End" of AOCL_LZ4_compress_HC_extStateHC Tests
 *************************************************/
#endif

/*********************************************
 * "Begin" of LZ4_compress_HC_destSize Tests
 *********************************************/
class LZ4HC_LZ4_compress_HC_destSize : public AOCL_setup_lz4hc {
};

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_1) // statePtr_NULL
{
    TestLoad_1 d(800);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_HC_destSize(NULL, d.getOrigData(), d.getCompressedBuff(), &srcLen, d.getCompressedSize(), 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_2) // src_NULL
{
    TestLoad_1 d(800);
    Stream statePtr(0);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_HC_destSize(statePtr.Get_Stream(), NULL, d.getCompressedBuff(), &srcLen, d.getCompressedSize(), 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_3) // dest_NULL
{
    
    TestLoad_1 d(800);
    Stream statePtr(0);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_HC_destSize(statePtr.Get_Stream(), d.getOrigData(), NULL, &srcLen, d.getCompressedSize(), 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_4) // srcLen_NULL
{
    TestLoad_1 d(800);
    Stream statePtr(0);

    EXPECT_EQ(LZ4_compress_HC_destSize(statePtr.Get_Stream(), d.getOrigData(), d.getCompressedBuff(), NULL, d.getCompressedSize(), 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_5) // dstLen_0
{
    TestLoad_1 d(800);
    Stream statePtr(0);
    int srcLen = d.getOrigSize();

    EXPECT_EQ(LZ4_compress_HC_destSize(statePtr.Get_Stream(), d.getOrigData(), d.getCompressedBuff(), &srcLen, 0, 9), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_6) // Pass
{
    TestLoad_1 d(800);
    Stream statePtr(0);
    int srcLen = d.getOrigSize();
    int compressedLen = LZ4_compress_HC_destSize(statePtr.Get_Stream(), d.getOrigData(), d.getCompressedBuff(), &srcLen, d.getCompressedSize(), 9);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_7) // Compression_level_less_than_minimum
{
    TestLoad_1 d(800);
    Stream statePtr(0);
    int srcLen = d.getOrigSize();

    int compressedLen = LZ4_compress_HC_destSize(statePtr.Get_Stream(), d.getOrigData(), d.getCompressedBuff(), &srcLen, d.getCompressedSize(), -1);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_destSize_common_8) // Compression_level_is_greater_than_max
{
    TestLoad_1 d(800);
    Stream statePtr(0);
    int srcLen = d.getOrigSize();

    int compressedLen = LZ4_compress_HC_destSize(statePtr.Get_Stream(), d.getOrigData(), d.getCompressedBuff(), &srcLen, d.getCompressedSize(), 13);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(d.getOrigData(),d.getOrigSize(),d.getCompressedBuff(),compressedLen));
}

/*********************************************
 * "End" of LZ4_compress_HC_destSize Tests
 *********************************************/

/*********************************************
 * "Begin" of LZ4_createStreamHC Test
 *********************************************/
TEST(LZ4HC_LZ4_createStreamHC, AOCL_Compression_lz4hc_LZ4_createStreamHC_common) // LZ4_freeStreamHC
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

TEST(LZ4HC_AOCL_LZ4_createStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_createStreamHC_common) // AOCL_LZ4_freeStreamHC
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

TEST(LZ4HC_LZ4_freeStreamHC, AOCL_Compression_lz4hc_LZ4_freeStreamHC_common_1) // NULL_ptr
{
    EXPECT_EQ(LZ4_freeStreamHC(NULL), 0);
}

TEST(LZ4HC_LZ4_freeStreamHC, AOCL_Compression_lz4hc_LZ4_freeStreamHC_common_2) // free_legitimate_stream
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

TEST(LZ4HC_AOCL_LZ4_freeStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_freeStreamHC_common_1) // NULL_ptr
{
    EXPECT_EQ(AOCL_LZ4_freeStreamHC(NULL), 0);
}

TEST(LZ4HC_AOCL_LZ4_freeStreamHC, AOCL_Compression_lz4hc_AOCL_LZ4_freeStreamHC_common_2) // free_legitimate_stream
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

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_common_1) // Compression_level_less_than_minimum
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

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_common_2) // Compression_level_greater_than_maximum
{
    Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = 13;
    LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 12);
}

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_common_3) // Legitimate_ptr
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

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_common_4) // LZ4_createStreamHC
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

TEST(LZ4HC_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_LZ4_resetStreamHC_fast_common_5) // input_NULL
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

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_common_1) // Compression_level_less_than_minimum
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

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_common_2) // Compression_level_greater_than_maximum
{
    AOCL_Stream State_stream(1);       /* created using malloc. */
    ASSERT_NE((long long)State_stream.Get_Stream(), NULL);
    int compressionLevel = 13;
    AOCL_LZ4_resetStreamHC_fast(State_stream.Get_Stream(), compressionLevel);
    EXPECT_EQ(State_stream.get_stream_Internal_dirty(), 0);
    EXPECT_EQ(State_stream.get_stream_Internal_base(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_dictCtx(), nullptr);
    EXPECT_EQ(State_stream.get_stream_Internal_compressionLevel(), 12);
}

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_common_3) // Legitimate_ptr
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

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_common_4) // LZ4_createStreamHC
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

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_common_5) // State_stream->internal_donotuse.dirty set to 1.
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

TEST(LZ4HC_AOCL_LZ4_resetStreamHC_fast, AOCL_Compression_lz4hc_AOCL_LZ4_resetStreamHC_fast_common_6) // input_NULL
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

    const LZ4_byte* get_ctx_dictBase(LZ4HC_CCtx_internal* ctxPtr)
    {
        return ctxPtr->dictBase;
    }

    // Destructor function of `LLZ4_loadDictHC`.
    ~LZ4HC_LZ4_loadDictHC()
    {
        LZ4_freeStreamHC(stream);
    }
};

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_common_1) // stream_NULL
{
    char dict[11] = "helloWorld";
    EXPECT_EQ(LZ4_loadDictHC(NULL, dict, 10), 0);
}

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_common_2) // dictionary_NULL
{
    int dictSize = 66000;
    EXPECT_EQ(LZ4_loadDictHC(stream, NULL, dictSize), 0);
}

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_common_3) // dictSize_<_64KB
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
    EXPECT_NE(get_ctx_dictBase(ctxPtr), (unsigned char *)dict);

    free(dict);
}

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_common_4) // dictSize_>_64KB
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

TEST_F(LZ4HC_LZ4_loadDictHC, AOCL_Compression_lz4hc_LZ4_loadDictHC_common_5) // dictSize <= 4
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

class LZ4HC_LZ4_compress_HC_continue : public LZ4HC_LZ4_compress_HC_extStateHC
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
        ctx.dictBase = dict;
    }
};

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_1) // statePtr_NULL
{
    setSrc(1024);
    setDst(10000);
    EXPECT_EQ(LZ4_compress_HC_continue(NULL, src, dst, srcSize, dstSize), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_2) // source_NULL
{
    setSrc(1024);
    setDst(10000);
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_continue(statePtr.Get_Stream(), NULL, dst, srcSize, dstSize), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_3) // dest_NULL
{
    setSrc(1024);
    setDst(10000);
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_continue(statePtr.Get_Stream(), src, NULL, srcSize, dstSize), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_4) // input_sz1
{
    setSrc(1024);
    setDst(10000);
    Stream statePtr(0);
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), src, dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_5) // input_sz2
{
    setSrc(5120);
    setDst(10000);
    Stream statePtr(0);
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), src, dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_6) // prefix_mode_1
{
    setSrc(100);
    setDst(150);
    Stream statePtr(0);

    int dictSize = 100;
    char dict[200];
    memcpy(&dict[100], src, 100);
    
    for (int i = 0; i < 100; i++)
    {
        dict[i] = i % 255;
    }
    
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is less than 64kb
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), &dict[100], dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[100], srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_7) // prefix_mode_2
{
    setSrc(100);
    setDst(150);
    Stream statePtr(0);

    const int dictSize = 64 * 1024;
    char* dict = (char*)malloc((dictSize + srcSize) * sizeof(char));
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    memcpy(&dict[dictSize],src,srcSize);
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is equal to 64kb
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), &dict[dictSize], dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[dictSize], srcSize, dst, compressedLen));

    free(d);
    free(dict);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_8) // using_LoadDictHC_less_64kb
{
    setSrc(100);
    setDst(150);
    const int dictSize = 100;
    char dict[dictSize];
    Stream statePtr(0);

    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), src, dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_9) // using_LoadDictHC_64kb
{
    setSrc(100);
    setDst(150);
    const int dictSize = 64 * 1024;
    char dict[dictSize];
    Stream statePtr(0);

    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), src, dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_10) // external_dictionary_mode
{
    setSrc(100);
    setDst(150);
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
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), src, dst, srcSize, dstSize);
    
    EXPECT_NE(compressedLen, 0);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));

    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_11) // external_dictionary_mode_64kb_dict_size
{
    setSrc(100);
    setDst(150);
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

    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), src, dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));

    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_12) // overlapping_source_dict
{
    setSrc(150);
    setDst(200);
    Stream statePtr(0);

    const int dictSize = 100;
    char dict[200] = {0};
    
    memcpy(dict, src, 100);
    for (int i = 150; i < 200; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), dict, dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_common_13) // overlap_greater_than_64kb
{
    setSrc(150);
    setDst(200);
    Stream statePtr(0);
    const int dictSize = 65 * 1024;
    char dict[150 + dictSize] = {0};
    
    memcpy(dict, src, srcSize);
    for (int i = 150; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);

    // initialise ctx pointer to statePtr->internal_donotuse
    initialize_ctx_and_dictBase(statePtr.Get_Stream(), (const LZ4_byte *)&dict[100]);
    
    int compressedLen = LZ4_compress_HC_continue(statePtr.Get_Stream(), dict, dst, srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
}

/*********************************************
 * "End" of LZ4_compress_HC_continue Tests
 *********************************************/

/*************************************************
 * "Begin" of LZ4_compress_HC_extStateHC Tests
 *************************************************/

class LZ4HC_LZ4_compress_HC_continue_destSize : public LZ4HC_LZ4_compress_HC_continue
{
};

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_1) // statePtr_NULL
{
    setSrc(1024);
    setDst(10000);
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_continue_destSize(NULL, src, dst, &srcSize, dstSize), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_2) // source_NULL
{
    setSrc(1024);
    setDst(10000);
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), NULL, dst, &srcSize, dstSize), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_3) // dest_NULL
{
    setSrc(1024);
    setDst(10000);
    Stream statePtr(0);
    EXPECT_EQ(LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), src, NULL, &srcSize, dstSize), 0);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_4) // input_sz1
{
    setSrc(1024);
    setDst(10000);
    Stream statePtr(0);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), src, dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_5) // input_sz2
{
    setSrc(5120);
    setDst(10000);
    Stream statePtr(0);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), src, dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_6) // prefix_mode_1
{
    setSrc(100);
    setDst(150);
    Stream statePtr(0);
    const int dictSize = 100;
    char dict[200];
    memcpy(&dict[100], src, 100);
    
    for (int i = 0; i < 100; i++)
    {
        dict[i] = i % 255;
    }
    
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is less than 64kb
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), &dict[100], dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[100], srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_7) // prefix_mode_2
{
    setSrc(100);
    setDst(150);
    Stream statePtr(0);
    const int dictSize = 64 * 1024;
    char* dict = (char*)malloc((dictSize + srcSize) * sizeof(char));
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    memcpy(&dict[dictSize],src,srcSize);
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    set_statePtr_dictCtx(statePtr.Get_Stream());
    set_ptr_to_Internal_dictCtx(statePtr.Get_Stream());
    memcpy(d, statePtr.Get_Stream(), SizeOf_LZ4HC_CCtx_internal());
    // This test case is to cover the condition where
    // dictEnd == (const BYTE*)source
    // And dict size is equal to 64kb
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), &dict[dictSize], dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(&dict[dictSize], srcSize, dst, compressedLen));
    
    free(d);
    free(dict);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_8) // using_LoadDictHC_less_64kb
{
    setSrc(100);
    setDst(150);
    Stream statePtr(0);
    const int dictSize = 100;
    char dict[dictSize] = { 0 };
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), src, dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_9) // using_LoadDictHC_64kb
{
    setSrc(100);
    setDst(150);
    Stream statePtr(0);
    const int dictSize = 64 * 1024;
    char dict[dictSize] = { 0 };
    //Initialize a dictionary.
    for (int i = 0; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), dict, dictSize);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), src, dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_10) // external_dictionary_mode
{
    setSrc(100);
    setDst(150);
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
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), src, dst, &srcSize, dstSize);
    
    EXPECT_NE(compressedLen, 0);
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
    
    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_11) // external_dictionary_mode_64kb_dict_size
{
    setSrc(100);
    setDst(150);
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
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), src, dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(src, srcSize, dst, compressedLen));
    
    free(d);
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_12) // overlapping_source_dict
{
    setSrc(150);
    setDst(200);
    Stream statePtr(0);

    const int dictSize = 100;
    char dict[200] = { 0 };
    
    memcpy(dict, src, 100);
    for (int i = 150; i < 200; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), dict, dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_13) // overlap_greater_than_64kb
{
    setSrc(150);
    setDst(200);
    Stream statePtr(0);

    const int dictSize = 65 * 1024;
    char dict[150 + dictSize] = { 0 };
    
    memcpy(dict, src, srcSize);
    for (int i = 150; i < dictSize; i++)
    {
        dict[i] = i % 255;
    }
    LZ4_loadDictHC(statePtr.Get_Stream(), &dict[100], dictSize);
    initialize_ctx_and_dictBase(statePtr.Get_Stream(), (const LZ4_byte *)&dict[100]);
    
    int compressedLen = LZ4_compress_HC_continue_destSize(statePtr.Get_Stream(), dict, dst, &srcSize, dstSize);
    EXPECT_NE(compressedLen, 0);
    
    EXPECT_TRUE(lz4hc_check_uncompressed_equal_to_original(dict, srcSize, dst, compressedLen));
}

TEST_F(LZ4HC_LZ4_compress_HC_continue_destSize, AOCL_Compression_lz4hc_LZ4_compress_HC_continue_destSize_common_14) // dst < LZ4_compressBound(src)
{
    setSrc(150);
    Stream statePtr(0);

    int targetDstSize = LZ4_compressBound(srcSize);
    setDst(targetDstSize * 0.75);

    const int dictSize = 65 * 1024;
    char dict[150 + dictSize] = {0};
    
    memcpy(dict, src, srcSize);
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
        return ctx->base;
    }
    
    
    // Destructor function of `LLZ4_saveDictHC` class.
    ~LZ4HC_LZ4_saveDictHC()
    {
        if (dict)
            free(dict);
        if (stream)
            LZ4_freeStreamHC(stream);
    }
};

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_common_1) // dictonary_size_greater_64kb
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

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_common_2) // dictionary_and_dictSize_is_zero
{
    EXPECT_EQ(LZ4_saveDictHC(stream, NULL, 0), 0);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_common_3) // stream_NULL
{
    char dict[11]="helloWorld";
    EXPECT_EQ(LZ4_saveDictHC(NULL, dict, 10), 0);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_common_4) // dictionary_is_NULL_dictSize_not_zero
{
    EXPECT_EQ(LZ4_saveDictHC(stream, NULL, 3), 0);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_common_5) // dictionary_size_greater_than_parameter
{
    loadDictHC(10000);
    char *data = (char *)malloc(100000);
    for(int i=0; i<100000; i++)
        data[i]=i;
    
    EXPECT_EQ(LZ4_saveDictHC(stream, data, 100000), 10000);
    EXPECT_EQ(memcmp(get_stream_Internal_end() - 10000 , data, 10000), 0);
    
    free(data);
}

TEST_F(LZ4HC_LZ4_saveDictHC, AOCL_Compression_lz4hc_LZ4_saveDictHC_common_6) // no_loaded_dictionary
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_1)  // 7 Bytes reverse match
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_2)  // 7 Bytes reverse match(last byte mismatch)
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_3)  // 8 Bytes reverse match
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_4)  // 8 Bytes reverse match(last byte mismatch)
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_5)  // 9 Bytes reverse match
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_6)  // 9 Bytes reverse match(last byte mismatch)
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_7)  // 10 Bytes reverse match
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_8)  // 10 Bytes reverse match(last byte mismatch)
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_9)  // 11 Bytes reverse match
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_10)  // 11 Bytes reverse match(last byte mismatch)
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_11)  // 12 Bytes reverse match
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_12)  // 12 Bytes reverse match(last byte mismatch)
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_13)  // 13 Bytes reverse match
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_14)  // 13 Bytes reverse match(last byte mismatch)
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_15)  // long string reverse match to check loop iterations
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_16)  // iMin < limit
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_17)  // mMin < limit
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_18)  // mMin != match and iMin != ip
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

TEST_F(LZ4HC_AOCL_LZ4HC_countBack, AOCL_Compression_lz4hc_AOCL_LZ4HC_countBack_common_19)  // mMin > match and iMin > ip
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
    AOCL_LZ4_streamHC_t* const state = (AOCL_LZ4_streamHC_t*)malloc(sizeof(AOCL_LZ4_streamHC_t));
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
        free(state);
    }
};

TEST_F(LZ4HC_AOCL_LZ4HC_Insert, AOCL_Compression_lz4hc_AOCL_LZ4HC_Insert_common_1)  
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

TEST_F(LZ4HC_AOCL_LZ4HC_Insert, AOCL_Compression_lz4hc_AOCL_LZ4HC_Insert_common_2)
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

TEST_F(LZ4HC_AOCL_LZ4HC_InsertAndGetWiderMatch, AOCL_Compression_lz4hc_AOCL_LZ4HC_InsertAndGetWiderMatch_common_1)   // Disabled PatternAnslysis
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

TEST_F(LZ4HC_AOCL_LZ4HC_InsertAndGetWiderMatch, AOCL_Compression_lz4hc_AOCL_LZ4HC_InsertAndGetWiderMatch_common_2) // Enabled Pattern Analysis
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
