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
 
 /** @file bzip2_gtest.cc
 *  
 *  @brief Test cases for BZIP2 algo.
 *
 *  This file contains the test cases for BZIP2 method
 *  testing the API level functions of BZIP2.
 *
 *  @author J Niranjan Reddy
 */

#include <vector>
#include <string>
#include <random>
#include <limits.h>
#include "gtest/gtest.h"

#include "algos/bzip2/bzlib.h"
#include "algos/bzip2/bzlib_private.h"

#define DEFAULT_OPT_LEVEL 2

using namespace std;

/*
    state_type is used for selecting the type of bz_stream->state,
    which can be two different datatypes
    COMPRESS -> EState
    DECOMPRESS -> DState
*/
enum state_type
{
    COMPRESS = 0,
    DECOMPRESS
};


template<typename T>
class StateClass
{
    public:

    bz_stream * stream = NULL;
    
    StateClass()
    {
        stream = new bz_stream;
        memset(stream, 0, sizeof(bz_stream));
    }

    bz_stream * getStrm()
    {
        return stream;
    }

    void setAvailIn(unsigned int avail_in)
    {
        stream->avail_in = avail_in;
    }

    unsigned int getAvailIn()
    {
        return stream->avail_in;
    }

    unsigned int getAvailOut()
    {
        return stream->avail_out;
    }

    void setAvailOut(unsigned int avail_out)
    {
        stream->avail_out = avail_out;
    }

    void setNextOut(char * data)
    {
        stream->next_out = data;
    }

    void setNextIn(char * data)
    {
        stream->next_in = data;
    }

    T * getStatePtr()
    {
        return (T *)stream->state;
    }

    void setStream(bz_stream * strm)
    {
        T * state = (T *)stream->state;
        state->strm = strm;
    }

    void setState(int st)
    {
        T * statePtr = (T *)stream->state;
        statePtr->state = st;
    }

    virtual ~StateClass()
    {
        delete stream;
        stream = NULL;
    }
};

/*
    EStateClass handles get and set operations of `EState` struct.
*/
class EStateClass : public StateClass<EState>
{
    public:

    EStateClass()
    {
        StateClass();
    }

    void setMode(int mode)
    {
        EState * statePtr = (EState *)stream->state;
        statePtr->mode = mode;
    }

    void setAvailInExpect(unsigned int avail_in_expect)
    {
        EState * statePtr = (EState *)stream->state;
        statePtr->avail_in_expect = avail_in_expect;
    }

    void setStateOutPos(int state_out_pos)
    {
        EState * statePtr = (EState *)stream->state;
        statePtr->state_out_pos = state_out_pos;
    }

    void setNumZ(int numZ)
    {
        EState * statePtr = (EState *)stream->state;
        statePtr->numZ = numZ;
    }

    void setEState(EState * estate)
    {
        stream->state = estate;
    }

    void setStateInLen(int state_in_len)
    {
        EState * statePtr = (EState *)stream->state;
        statePtr->state_in_len = state_in_len;
    }

    void setStateInCh(unsigned int state_in_ch)
    {
        EState * statePtr = (EState *)stream->state;
        statePtr->state_in_ch = state_in_ch;
    }

    ~EStateClass()
    {
        if(stream != NULL)
        {
            BZ2_bzCompressEnd(stream);
        }
    }
};

/*
    DStateClass handles get and set operations of `DState` struct.
*/
class DStateClass : public StateClass<DState>
{
    public:

    DStateClass()
    {
        StateClass();
    }

    int getState()
    {
        DState * statePtr = (DState *)stream->state;
        return statePtr->state;
    }

    DState * getStatePtr()
    {
        return (DState *)stream->state;
    }

    int getnBlock()
    {
        DState * statePtr = (DState *)stream->state;
        return statePtr->save_nblock;
    }


    void setDState(DState * st)
    {
        stream->state = st;
    }

    void setnBlockUsed(int nblock_used)
    {
        DState * statePtr = (DState *)stream->state;
        statePtr->nblock_used = nblock_used;
    }

    void setSmallDecompress(unsigned char smallDecompress)
    {
        DState * statePtr = (DState *)stream->state;
        statePtr->smallDecompress = smallDecompress;
    }

    void setBlockRandomised(unsigned char blockRandomised)
    {
        DState * statePtr = (DState *)stream->state;
        statePtr->blockRandomised = blockRandomised;
    }

    void setStateOutLen(int state_out_len)
    {
        DState * statePtr = (DState *)stream->state;
        statePtr->state_out_len = state_out_len;
    }

    ~DStateClass()
    {
        if(stream != NULL)
        {
            BZ2_bzDecompressEnd(stream);
        }
    }
};

/*
    Wrapper class for BZIP2 API's
*/
class BZIP2_API
{
    public:
    
    static int CompressInit(bz_stream* strm, int blockSize100k, int verbosity, int workFactor )
    {
        return BZ2_bzCompressInit(strm, blockSize100k, verbosity, workFactor);
    }

    static int Compress(bz_stream* strm, int action)
    {
        return BZ2_bzCompress(strm, action);
    }

    static int CompressEnd(bz_stream* strm)
    {
        return BZ2_bzCompressEnd(strm);
    }

    static int DecompressInit(bz_stream *strm, int verbosity, int small)
    {
        return BZ2_bzDecompressInit(strm, verbosity, small);
    }

    static int Decompress(bz_stream* strm)
    {
        return BZ2_bzDecompress(strm);
    }

    static int DecompressEnd(bz_stream *strm)
    {
        return BZ2_bzDecompressEnd(strm);
    }

    static int BuffToBuffCompress(char* dest, unsigned int* destLen, char* source, unsigned int sourceLen, int blockSize100k, int verbosity, int workFactor)
    {
        return BZ2_bzBuffToBuffCompress(dest, destLen, source, sourceLen, blockSize100k, verbosity, workFactor);
    }

    static int BuffToBuffDecompress(char* dest, unsigned int* destLen, char*source, unsigned int  sourceLen,int  small, int  verbosity)
    {
        return BZ2_bzBuffToBuffDecompress(dest, destLen, source, sourceLen, small, verbosity);
    }
};

/*
    Common class for calling setup function and inheriting gtest fixture.
*/
class BZIP2_setup_and_gtest_class : public ::testing::Test
{
    public:
    BZIP2_setup_and_gtest_class()
    {
        int optLevel = DEFAULT_OPT_LEVEL;
        aocl_setup_bzip2(0, optLevel, 0, 0, 0);
    }
};

/*********************************************
 * "Begin" of BZIP2_bzCompressInit Tests
 ********************************************/
class BZIP2_BZ2_bzCompressInit : public BZIP2_setup_and_gtest_class
{
    public:
    EStateClass * strm = NULL;

    void SetUp() override {
        strm = new EStateClass();
    }

    void TearDown() override {
        delete strm;
    }
};

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_1)
{
    
    EXPECT_EQ(BZIP2_API::CompressInit(NULL, 5, 2, 2), BZ_PARAM_ERROR);               // strm is NULL
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_2)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 0, 2, 2), BZ_PARAM_ERROR);    // blockSize100 is less than 1
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_3)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 10, 3, 100), BZ_PARAM_ERROR); // blockSize100 is greater than 9
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_4)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 3, -1, 30), BZ_PARAM_ERROR);  // verbosity is less than 0
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_5)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 4, 5, 30), BZ_PARAM_ERROR);   // verbosity greater than 4
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_6)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 2, -1), BZ_PARAM_ERROR);   // workFactor less than 0
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_7)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 2, 2, 251), BZ_PARAM_ERROR);  // workFactor greater than 250
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_8)
{ 
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 0), BZ_OK); // All parameters are set to the least acceptable limit
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_9)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 1), BZ_OK); // workFactor is 1
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_common_10)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 9, 4, 250), BZ_OK);   // All parameters are set to the largest acceptable limit
}

/*********************************************
 * "End" of BZIP2_bzCompressInit Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzCompressEnd Tests
 ********************************************/

class BZIP2_BZ2_bzCompressEnd : public BZIP2_BZ2_bzCompressInit
{
};

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_common_1)
{
    EXPECT_EQ(BZIP2_API::CompressEnd(NULL), BZ_PARAM_ERROR);             // strm is NULL
}

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_common_2)
{
    EXPECT_EQ(BZIP2_API::CompressEnd(strm->getStrm()), BZ_PARAM_ERROR);  // strm is not initialized
}

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_common_3)
{
    BZIP2_API::CompressInit(strm->getStrm(), 1, 1, 1);
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::CompressEnd(strm->getStrm()), BZ_PARAM_ERROR);  //  strm->state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_common_4)
{
    BZIP2_API::CompressInit(strm->getStrm(), 1, 1, 1);
    EXPECT_EQ(BZIP2_API::CompressEnd(strm->getStrm()), BZ_OK);           //  pass case
    EXPECT_EQ((long long)strm->getStatePtr(), NULL);
}

/*********************************************
 * "End" of BZ2_bzCompressEnd Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzDecompressInit Tests
 ********************************************/

class BZIP2_BZ2_bzDecompressInit :  public BZIP2_setup_and_gtest_class
{
    public:
    DStateClass * strm = NULL;

    void SetUp() override {
        strm = new DStateClass();
    }

    void TearDown() override {
        delete strm;
    }
};

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_common_1)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(NULL, 2, 0), BZ_PARAM_ERROR);                // strm is NULL
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_common_2)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 2, -1), BZ_PARAM_ERROR);    // small is less than 0
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_common_3)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 2, 2), BZ_PARAM_ERROR);     // small is greater than 1
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_common_4)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), -1, 0), BZ_PARAM_ERROR);    // verbosity is less than 0
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_common_5)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 5, 0), BZ_PARAM_ERROR);     // verbosity is greater than 4
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_common_6)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 0, 0), BZ_OK);  // pass case, all arguments are least acceptable values
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_common_7)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 4, 1), BZ_OK);  // pass case, all arguments are highest acceptable values
}

/*********************************************
 * "End" of BZ2_bzDecompressInit Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzDecompressEnd Tests
 ********************************************/

class BZIP2_BZ2_bzDecompressEnd : public BZIP2_BZ2_bzDecompressInit
{
};

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_common_1)
{
    EXPECT_EQ(BZIP2_API::DecompressEnd(NULL), BZ_PARAM_ERROR);               // strm is NULL
}

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_common_2)
{
    EXPECT_EQ(BZIP2_API::DecompressEnd(strm->getStrm()), BZ_PARAM_ERROR);    // strm is not initialized
}

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_common_3)
{
    BZIP2_API::DecompressInit(strm->getStrm(), 1, 1);
    strm->setDState(strm->getStatePtr());
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::DecompressEnd(strm->getStrm()), BZ_PARAM_ERROR);    // strm->state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_common_4)
{
    BZIP2_API::DecompressInit(strm->getStrm(), 1, 1);
    EXPECT_EQ(BZIP2_API::DecompressEnd(strm->getStrm()), BZ_OK);             // pass case
    EXPECT_EQ((long long)strm->getStatePtr(), NULL);
}

/*********************************************
 * "End" of BZ2_bzDecompressEnd Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzBuffToBuffCompress Tests
 ********************************************/

/*
    This function is to verify if compression is done without any mistakes,
    uncompress the compressed data and match with `original` uncompressed data,
    if matches exactly return true, if not return false.
*/
bool verify_uncompressed_equal_original(char * compressed, unsigned int compressedLen, char *original, size_t orginalLen)
{
    unsigned int uncompressedLen = orginalLen;
    vector<char> uncompressedBuf(orginalLen, 'a');
    if(BZIP2_API::BuffToBuffDecompress(uncompressedBuf.data(), &uncompressedLen, compressed, compressedLen, 0, 0) != BZ_OK)
        return false;
    return memcmp(uncompressedBuf.data(), original, orginalLen)==0;
}

class BZIP2_BZ2_bzBuffToBuffCompress : public BZIP2_setup_and_gtest_class
{
    public:
    char dest[20];
    unsigned int destLen = 20;
    char source[11] = "helloWorld";
    unsigned int sourceLen = 10;
    int blockSize100k = 5;
    int verbosity = 0;
    int workFactor = 100;
    
    const unsigned int sourceLenPassCase = 600000;
    const unsigned int destSize = (float)sourceLenPassCase * 1.01 + 600;
    vector<char> sourcePass;
    unsigned int destLenPass = destSize;
    vector<char> destPass;

    void Init()
    {
        sourcePass = vector<char>(sourceLenPassCase, 'a');
        destPass = vector<char>(destSize, 'a');
        // Initilizing `source` with random data
        for(unsigned int i = 0; i < sourceLenPassCase; i++)
        {
            sourcePass[i] = rand() % 255;
        }
    }
};

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_1)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(NULL, &destLen, source, sourceLen, blockSize100k, verbosity, workFactor), BZ_PARAM_ERROR); // dest is NULL
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_2)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, NULL, source, sourceLen, blockSize100k, verbosity, workFactor), BZ_PARAM_ERROR);     // destLen is NULL
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_3)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, NULL, sourceLen, blockSize100k, verbosity, workFactor), BZ_PARAM_ERROR);   // source is NULL
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_4)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, 0, verbosity, workFactor), BZ_PARAM_ERROR);             // blockSize100k is less than 1
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_5)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, 10, verbosity, workFactor), BZ_PARAM_ERROR);            // blockSize100k is greater than 9
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_6)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, -1, workFactor), BZ_PARAM_ERROR);        // verbosity is less than 0
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_7)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, 5, workFactor), BZ_PARAM_ERROR);         // verbosity is greater than 5
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_8)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, verbosity, -1), BZ_PARAM_ERROR);         // workFactor is less than 0
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_9)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, verbosity, 251), BZ_PARAM_ERROR);        // workFactor is greater than 250
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_10)
{
    destLen = 5;    // dest will not have enough space to contain compressed data, so the below test case will return `BZ_OUTBUFF_FULL`
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, verbosity, workFactor), BZ_OUTBUFF_FULL); 
}


TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_11)
{
    Init();
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 1, verbosity, 0), BZ_OK);            // parameters are set to the least acceptable values.
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_12)
{
    Init();
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 9, verbosity, 250), BZ_OK);          // parameters are set to highest acceptable values.
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}

TEST_F(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_common_13)
{
    Init();
    // destLenPass = destSize;
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, blockSize100k, verbosity, 1), BZ_OK);// workFactor is set to 1
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}
/*********************************************
 * "End" of BZ2_bzBuffToBuffCompress Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzBuffToBuffDecompress Tests
 ********************************************/

class BZIP2_BZ2_bzBuffToBuffDecompress : public BZIP2_setup_and_gtest_class
{
    public:
    const unsigned int sourceLen = 600000;
    unsigned int uncompressedLen = sourceLen;
    const unsigned int destSize = sourceLen * 1.01 + 600;
    unsigned int destLen = destSize;
    int blockSize100k = 5;
    int verbosity = 0;
    int workFactor = 100;
    int small = 0;
    vector<char> source, dest, uncompressed;

    void SetUp() override
    {
        source = vector<char>(sourceLen, 'a');
        uncompressed = vector<char>(sourceLen, 'a');
        // Initializing `source` with random data
        for(unsigned int i = 0; i < sourceLen; i++)
        {
            source[i] = rand() % 255;
        }
        dest = vector<char>(destSize, 'a');
        EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest.data(), &destLen, source.data(), sourceLen, 1, verbosity, 0), BZ_OK);
    }

};

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_1)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(NULL, &uncompressedLen, source.data(), sourceLen, small, verbosity), BZ_PARAM_ERROR);                // dest is NULL
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_2)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), NULL, source.data(), sourceLen, small, verbosity), BZ_PARAM_ERROR);             // destLen is NULL
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_3)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, NULL, sourceLen, small, verbosity), BZ_PARAM_ERROR);          // source is NULL
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_4)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, -1, verbosity), BZ_PARAM_ERROR);    // small is less than 0
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_5)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, 2, verbosity), BZ_PARAM_ERROR);     // small is greater than 1
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_6)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, small, -1), BZ_PARAM_ERROR);        // verbosity is less than 0}
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_7)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, small, 5), BZ_PARAM_ERROR);         // verbosity greater than 4
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_8)
{ 
    dest[0]++;  // Modifying compressed buffer at 0 index to check if the method returns `BZ_DATA_ERROR_MAGIC`
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR_MAGIC);
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_9)
{
    dest[5]++;  // Modifying compressed buffer at 5 index to check if the method returns `BZ_DATA_ERROR`
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR);
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_10)
{
    uncompressedLen = sourceLen - 100;  // Providing insufficient  `dest` buffer to check if the method returns `BZ_OUTBUFF_FULL`
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_OUTBUFF_FULL);
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_common_11)
{
    // The end part of compressed buffer is not provided to check if the method returns `BZ_UNEXPECTED_EOF`
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen-100, small, verbosity), BZ_UNEXPECTED_EOF);
}

TEST_F(BZIP2_BZ2_bzBuffToBuffDecompress, pass_cases)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, 1, 0), BZ_OK);
    EXPECT_EQ(uncompressedLen, sourceLen);
    EXPECT_EQ(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}

/*********************************************
 * "End" of BZ2_bzBuffToBuffDecompress Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzCompress Tests
 ********************************************/

class BZIP2_BZ2_bzCompress : public BZIP2_setup_and_gtest_class
{
    protected:
    EStateClass * strm = NULL;
    public:
    vector<char> input;
    vector<char> output;
    unsigned int inputSize, outputSize, inputGiven;

    void SetUp() override
    {
        strm = new EStateClass();
        BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 0);
        strm->setEState(strm->getStatePtr());

        inputSize = 600000;
        input = vector<char>(inputSize);
        // Initializing `source` with random data
        for(unsigned int i=0; i < inputSize;i++)
            input[i] = rand()%256;

        outputSize = inputSize*(2);
        output = vector<char>(outputSize);

        // Initializing strm, with input and output buffers.
        strm->setNextIn(input.data());
        strm->setNextOut(output.data());
        strm->setAvailIn(inputSize);
        strm->setAvailOut(outputSize);
    }

    void TearDown() override {
        if(strm != NULL)
        {
            BZIP2_API::CompressEnd(strm->getStrm());
            delete strm;
        }
    }
};

class BZIP2_BZ2_bzCompress_failCases : public BZIP2_BZ2_bzCompressInit
{
    public:
    EStateClass * strm = NULL;

    void SetUp() override
    {
        strm = new EStateClass();
        BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 0);
    }

    void TearDown() override {

        if(strm != NULL)
        {
            BZIP2_API::CompressEnd(strm->getStrm());
            delete strm;
        }
    }
};

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_1)
{
    EXPECT_EQ(BZIP2_API::Compress(NULL, BZ_M_RUNNING), BZ_PARAM_ERROR);              // strm is NULL
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_2)
{
    EStateClass * strm2 = new EStateClass();
    strm2->setEState(NULL);
    EXPECT_EQ(BZIP2_API::Compress(strm2->getStrm(), BZ_M_RUNNING), BZ_PARAM_ERROR);   // strm is not initialized
    delete strm2;
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_3)
{
    strm->setEState(strm->getStatePtr());
    strm->setMode(BZ_M_RUNNING);
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_PARAM_ERROR);         // strm->state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_4)
{
    strm->setEState(strm->getStatePtr());
    strm->setMode(-1);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_PARAM_ERROR);         // mode is -1
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_5)
{
    strm->setMode(BZ_M_IDLE);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_SEQUENCE_ERROR);      // mode is `BZ_M_IDLE`
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_6)
{
    strm->setMode(BZ_M_IDLE);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_SEQUENCE_ERROR);    // mode is `BZ_M_IDLE`
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_7)
{
    strm->setMode(BZ_M_IDLE);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // mode is `BZ_M_IDLE`
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_8)
{
    strm->setMode(BZ_M_RUNNING);
    strm->setAvailInExpect(1);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // no input data is provided
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_9)
{
    strm->setMode(BZ_M_RUNNING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_PARAM_ERROR);         // no input data is provided
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_10)
{
    strm->setMode(BZ_M_RUNNING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), -1), BZ_PARAM_ERROR);             // action is -1
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_11)
{
    strm->setMode(BZ_M_FLUSHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_SEQUENCE_ERROR);      // action should be equal to BZ_FLUSH
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_12)
{
    strm->setMode(BZ_M_FLUSHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // action should be equal to BZ_FLUSH
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_13)
{
    strm->setMode(BZ_M_FLUSHING);
    strm->setAvailIn(1);
    strm->setAvailInExpect(0);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_SEQUENCE_ERROR);    // avail_in != avail_in_expect
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_14)
{
    strm->setMode(BZ_M_FINISHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_SEQUENCE_ERROR);      // action should be equal to BZ_M_FINISHING
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_15)
{
    strm->setMode(BZ_M_FINISHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_SEQUENCE_ERROR);    // action should be equal to BZ_M_FINISHING    
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_16)
{
    strm->setMode(BZ_M_FINISHING);
    strm->setAvailIn(1);
    strm->setAvailInExpect(0);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // avail_in != avail_in_expect
}

TEST_F(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_common_17)
{
    strm->setMode(BZ_M_FINISHING);
    strm->setAvailInExpect(strm->getAvailIn());
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // no input is available to process
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_18)
{
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_RUN_OK);              // mode is BZ_M_RUNNING, pass case with BZ_RUN
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_19)
{
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_RUN_OK);            // mode is BZ_M_RUNNING, pass case with BZ_FLUSH
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);       
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_20)
{
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);       // mode is BZ_M_RUNNING, pass case with BZ_FINISH
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_21)
{
    strm->setMode(BZ_M_FLUSHING);
    strm->setAvailInExpect(strm->getAvailIn());
    // strm->avail_out = 600;
    strm->setStateOutPos(0);
    strm->setNumZ(1);
    strm->setState(BZ_S_OUTPUT);
    strm->setAvailOut(0);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_FLUSH_OK);         // mode is BZ_M_FLUSHING, avail_in_expect > 0
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_22)
{
    strm->setMode(BZ_M_FLUSHING);
    strm->setAvailIn(0);
    strm->setAvailInExpect(0);
    strm->setAvailOut(0);
    strm->setStateInCh(255);
    strm->setStateInLen(1);
    strm->setStateOutPos(0);
    strm->setNumZ(1);
    strm->setState(BZ_S_OUTPUT);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_FLUSH_OK);         // state is BZ_M_FLUSHING  s->state_out_pos < s->numZ
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_23)
{
    strm->setMode(BZ_M_FINISHING);
    strm->setAvailInExpect(strm->getAvailIn());
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);      //  pass case for BZ_FINISH with mode BZ_M_FINISHING
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_24) // mode is BZ_M_FINISHING
{
    strm->setMode(BZ_M_FINISHING);
    strm->setAvailInExpect(strm->getAvailIn());
    
    int currentOutLimit = strm->getAvailIn()/2;
    strm->setAvailOut(currentOutLimit);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_FINISH_OK); // s->avail_in_expect > 0
    strm->setAvailOut(outputSize);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize+currentOutLimit - (strm->getAvailOut()), input.data(), inputSize));
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_25) // mode is BZ_M_FINISHING
{
    strm->setMode(BZ_M_FINISHING);
    int currentOutputLimit = strm->getAvailIn()/2;      //  Only half of the Output is available
    strm->setAvailInExpect(strm->getAvailIn());
    strm->setAvailOut(currentOutputLimit);

    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_FINISH_OK);    // Compress and copy as much data as we can fit in the available output

    strm->setAvailOut(outputSize);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);   // Rest of the compressed output is copied to the output
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize+currentOutputLimit - (strm->getAvailOut()), input.data(), inputSize));
}

TEST_F(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_common_26) // mode BZ_M_FINISHING
{
    strm->setMode(BZ_M_FINISHING);
    inputGiven = strm->getAvailIn()/20;         // consider small amount of data as input
    strm->setAvailIn(inputGiven);               //
    strm->setAvailInExpect(inputGiven);
    int currentOutLimit = inputGiven/20;        // At first iteration, just provide little output for compressed buffer
    strm->setAvailOut(currentOutLimit);

    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_FINISH_OK); // s->state_out_pos < s->numZ
    strm->setAvailOut(inputGiven);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);   // second iteration, processing full input.
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), inputGiven+currentOutLimit - (strm->getAvailOut()), input.data(), inputGiven));
}
/*********************************************
 * "End" of BZ2_bzCompress Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzDecompress Tests
 ********************************************/

class BZIP2_BZ2_bzDecompress : public BZIP2_setup_and_gtest_class
{
    protected:
    DStateClass * strm = NULL;
    public:
    const unsigned int sourceLen = 6000;
    unsigned int uncompressedLen = sourceLen;
    const unsigned int destSize = sourceLen * 1.01 + 600;
    unsigned int destLen = destSize;
    int blockSize100k = 5;
    int verbosity = 0;
    int workFactor = 100;
    vector<char> source, dest, uncompressed;
    void SetUp() override
    {
        strm = new DStateClass();
        BZ2_bzDecompressInit(strm->getStrm(), 0, 0);
        source = vector<char>(sourceLen, 'a');
        uncompressed = vector<char>(sourceLen, 'a');
        // Initializing `source` with random data
        for(unsigned int i = 0; i < sourceLen; i++)
        {
            source[i] = rand() % 255;
        }
        dest = vector<char>(destSize, 'a');
        EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest.data(), &destLen, source.data(), sourceLen, 1, verbosity, 0), BZ_OK);
    }

    void TearDown() override {
        
        if(strm != NULL)
        {
            BZIP2_API::DecompressEnd(strm->getStrm());
            delete strm;
        }
    }
};

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_1)
{
    EXPECT_EQ(BZIP2_API::Decompress(NULL), BZ_PARAM_ERROR);              // NULL is passed
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_2)
{
    DState * temp_ptr = strm->getStatePtr();
    strm->setDState(NULL);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_PARAM_ERROR);   // state is pointed to NULL
    strm->setDState(temp_ptr);
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_3)
{
    strm->setDState(strm->getStatePtr());
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_PARAM_ERROR);   // state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_4)
{
    int temp_state = strm->getState();
    strm->setState(BZ_X_IDLE);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_SEQUENCE_ERROR);   // state is IDLE
    strm->setState(temp_state);
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_5)
{
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    dest[0]++;  // Modifying compressed buffer at 0 index to check if the method returns `BZ_DATA_ERROR_MAGIC`
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR_MAGIC);     // BZ_DATA_ERROR_MAGIC , s->state != BZ_X_OUTPUT
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_6)
{
    dest[15]++; // Modifying compressed buffer at 15 index to check if the method returns `BZ_DATA_ERROR`
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);           // BZ_DATA_ERROR, BZ_X_OUTPUT && s->calculatedBlockCRC != s->storedBlockCRC
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_7) // This case is for covering `return BZ_OK;` in the first while loop in BZ2_bzDompress
{
    strm->setDState(strm->getStatePtr());
    dest[15]--;
    strm->setAvailIn(destLen);
    strm->setAvailOut(0);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    strm->setnBlockUsed(strm->getnBlock()+2);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_OK); // BZ_OK

    EXPECT_NE(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_8) // Fail
{
    strm->setDState(strm->getStatePtr());
    strm->setState(BZ_X_OUTPUT);

    strm->setSmallDecompress(0);
    strm->setBlockRandomised(0);
    strm->setStateOutLen(0);
    strm->setnBlockUsed(strm->getnBlock()+2);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);  // BZ_DATA_ERROR
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_9)
{
    strm->setDState(strm->getStatePtr());
    strm->setState(BZ_X_OUTPUT);

    strm->setBlockRandomised(0);
    strm->setStateOutLen(0);
    strm->setnBlockUsed(strm->getnBlock()+2);
    strm->setSmallDecompress(1);
    strm->setAvailOut(1);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);  // BZ_DATA_ERROR
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_10)
{
    strm->setDState(strm->getStatePtr());
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    dest[destLen-2]--;
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR); // BZ_DATA_ERROR
}

TEST_F(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_common_11) // pass
{
    strm->setDState(strm->getStatePtr());
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}
/*********************************************
 * "End" of BZ2_bzDecompress Tests
 ********************************************/
