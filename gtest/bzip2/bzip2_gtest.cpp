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
 
 /** @file bzip2_gtest.cpp
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
#include "gtest_utils.h"

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

typedef struct {
    FILE*     handle;
    Char      buf[BZ_MAX_UNUSED];
    Int32     bufN;
    Bool      writing;
    bz_stream strm;
    Int32     lastErr;
    Bool      initialisedOk;
}
bzFile;

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

    unsigned int getTotalInLo32()
    {
        return stream->total_in_lo32;
    }

    unsigned int getTotalInHi32()
    {
        return stream->total_in_hi32;
    }

    unsigned int getTotalOutLo32()
    {
        return stream->total_out_lo32;
    }

    unsigned int getTotalOutHi32()
    {
        return stream->total_out_hi32;
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

    void setTotalInLo32(unsigned int total_in_lo32)
    {
        stream->total_in_lo32 = total_in_lo32;
    }

    void setTotalOutHi32(unsigned int total_out_hi32)
    {
        stream->total_out_hi32 = total_out_hi32;
    }

    void setTotalInHi32(unsigned int total_in_hi32)
    {
        stream->total_in_hi32 = total_in_hi32;
    }

    void setTotalOutLo32(unsigned int total_out_lo32)
    {
        stream->total_out_lo32 = total_out_lo32;
    }

    void setState(int st)
    {
        T * statePtr = (T *)stream->state;
        statePtr->state = st;
    }

    void setBzAlloc(void* (*alloc)(void*, int, int))
    {
        stream->bzalloc = alloc;
    }

    void setBzFree(void (*bzfree)(void*, void*))
    {
        stream->bzfree = bzfree;
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

    int get_nBlock()
    {
        return getStatePtr()->nblock;
    }

    int getNumZ()
    {
        return getStatePtr()->numZ;
    }

    int get_state_out_pos()
    {
        return getStatePtr()->state_out_pos;
    }

    unsigned int getStateInChar()
    {
        return getStatePtr()->state_in_ch;
    }

    int getStateInLen()
    {
        return getStatePtr()->state_in_len;
    }

    int getBlockNo()
    {
        return getStatePtr()->blockNo;
    }

    int getState()
    {
        return getStatePtr()->state;
    }

    int getMode()
    {
        return getStatePtr()->mode;
    }

    int getVerbosity()
    {
        return getStatePtr()->verbosity;
    }

    int getBlockSize100k()
    {
        return getStatePtr()->blockSize100k;
    }

    int getWorkFactor()
    {
        return getStatePtr()->workFactor;
    }

    unsigned int * getPtr()
    {
        return getStatePtr()->ptr;
    }

    unsigned char * getBlock()
    {
        return getStatePtr()->block;
    }

    unsigned short * getMtfv()
    {
        return getStatePtr()->mtfv;
    }

    unsigned char * getZbits()
    {
        return getStatePtr()->zbits;
    }

    unsigned int * getFtab()
    {
        return getStatePtr()->ftab;
    }

    unsigned int getCombinedCRC()
    {
        return getStatePtr()->combinedCRC;
    }

    unsigned int getBlockCRC()
    {
        return getStatePtr()->blockCRC;
    }

    int getStateOutPos()
    {
        return getStatePtr()->state_out_pos;
    }

    void setMode(int mode)
    {
        EState * statePtr = getStatePtr();
        statePtr->mode = mode;
    }

    void setAvailInExpect(unsigned int avail_in_expect)
    {
        EState * statePtr = getStatePtr();
        statePtr->avail_in_expect = avail_in_expect;
    }

    void setStateOutPos(int state_out_pos)
    {
        EState * statePtr = getStatePtr();
        statePtr->state_out_pos = state_out_pos;
    }

    void setNumZ(int numZ)
    {
        EState * statePtr = getStatePtr();
        statePtr->numZ = numZ;
    }

    void setEState(EState * estate)
    {
        stream->state = estate;
    }

    void setStateInLen(int state_in_len)
    {
        EState * statePtr = getStatePtr();
        statePtr->state_in_len = state_in_len;
    }

    void setStateInCh(unsigned int state_in_ch)
    {
        EState * statePtr = getStatePtr();
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
        DState * statePtr = getStatePtr();
        return statePtr->state;
    }

    int getnBlock()
    {
        DState * statePtr = getStatePtr();
        return statePtr->save_nblock;
    }

    int getbsLive()
    {
        return getStatePtr()->bsLive;
    }

    int getbsBuff()
    {
        return getStatePtr()->bsBuff;
    }

    int getCalculatedCombinedCRC()
    {
        return getStatePtr()->calculatedCombinedCRC;
    }

    unsigned char getSmallDecompress()
    {
        return getStatePtr()->smallDecompress;
    }

    unsigned char getBlockRandomised()
    {
        return getStatePtr()->blockRandomised;
    }

    unsigned char * getll4()
    {
        return getStatePtr()->ll4;
    }

    unsigned short * getll16()
    {
        return getStatePtr()->ll16;
    }

    unsigned int * gettt()
    {
        return getStatePtr()->tt;
    }

    int getCurrentBlockNo()
    {
        return getStatePtr()->currBlockNo;
    }

    int getVerbosity()
    {
        return getStatePtr()->verbosity;
    }

    void setDState(DState * st)
    {
        stream->state = st;
    }

    void setnBlockUsed(int nblock_used)
    {
        DState * statePtr = getStatePtr();
        statePtr->nblock_used = nblock_used;
    }

    void setSmallDecompress(unsigned char smallDecompress)
    {
        DState * statePtr = getStatePtr();
        statePtr->smallDecompress = smallDecompress;
    }

    void setBlockRandomised(unsigned char blockRandomised)
    {
        DState * statePtr = getStatePtr();
        statePtr->blockRandomised = blockRandomised;
    }

    void setStateOutLen(int state_out_len)
    {
        DState * statePtr = getStatePtr();
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

    static const char * LibVersion()
    {
        return BZ2_bzlibVersion();
    }


    static BZFILE* ReadOpen(int* bzerror, FILE* f, int verbosity, int small,void* unused, int nUnused)
    {
        return BZ2_bzReadOpen(bzerror, f, verbosity, small, unused, nUnused);
    }

    static void ReadClose(int * bzerror, BZFILE* b)
    {
        BZ2_bzReadClose(bzerror, b);
    }

    static void ReadGetUnused(int * bzerror, BZFILE* b, void** unused, int* nUnused)
    {
        BZ2_bzReadGetUnused(bzerror, b, unused, nUnused);
    }

    static int Read(int * bzerror, BZFILE* b, void* buf, int len)
    {
        return BZ2_bzRead(bzerror, b, buf, len);
    }

    static BZFILE* WriteOpen(int * bzerror, FILE* f, int blockSize100k, int verbosity, int workFactor)
    {
        return BZ2_bzWriteOpen(bzerror, f, blockSize100k, verbosity, workFactor);
    }

    static void Write(int * bzerror, BZFILE* b, void* buf, int len)
    {
        BZ2_bzWrite(bzerror, b, buf, len);
    }

    static void WriteClose(int * bzerror, BZFILE* b, int abandon, unsigned int* nbytes_in, unsigned int* nbytes_out)
    {
        BZ2_bzWriteClose(bzerror, b, abandon, nbytes_in, nbytes_out);
    }

    static void WriteClose64(int * bzerror, BZFILE* b, int abandon, unsigned int* nbytes_in_lo32, unsigned int* nbytes_in_hi32, unsigned int* nbytes_out_lo32, unsigned int* nbytes_out_hi32)
    {
        BZ2_bzWriteClose64(bzerror, b, abandon, nbytes_in_lo32, nbytes_out_hi32, nbytes_out_lo32, nbytes_out_hi32);
    }
};


/*
    Util functions
*/

/*
    For file I/O test, a new file is created with the name of the test case, this is necessary because
    when run with ctest -j, tests will run in parallel and will not overwrite each other's file
*/
string get_file_name()
{

    string file_name = testing::UnitTest::GetInstance()->current_test_info()->name();

    // Parameterised tests contain '/' character in their name,
    // but file name can't contain '/' character, so replacing it with '_'
    for(auto &c: file_name)
    {
        if(c == '/')
            c = '_';
    }
    return file_name;
}

/*
    This function is used to verify the bzip2 frame format compliance,
    it checks the magic number, if blockSize100k is within limit,
    `block header` and returns true iff all are valid.
*/
bool is_valid_bzip2_frame(char * data)
{
    string magic = "BZh";
    if(memcmp(data+0, magic.c_str(), 3) != 0)
        return false;
    int blockSize100k = data[3];
    if(blockSize100k-BZ_HDR_0 < 1 || blockSize100k-BZ_HDR_0 > 9)
        return false;
    string block_header = "1AY&SY";
    if(memcmp(data+4, block_header.c_str(), 6) != 0)
        return false;
    return true;
}

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
    EXPECT_EQ(uncompressedLen, orginalLen);
    if(memcmp(uncompressedBuf.data(), original, orginalLen) != 0)
        return false;
    return is_valid_bzip2_frame(compressed);
}

/*
    Custom memory allocation function.
*/
void * mem_alloc_pass(void * opaque, int items, int size)
{
    return calloc(items, size);
}

/*
    Custom memory allocation function which returns NULL always (used for testing fail cases).
*/
void * mem_alloc_fail(void * opaque, int items, int size)
{
    return NULL;
}

/*
    Custom memory allocation function which returns NULL for large input (used for testing fail cases).
*/
void * mem_alloc_fail_for_large_input(void * opaque, int items, int size)
{
    if(items > 100000)
    {
        return NULL;
    }
    return calloc(items, size);
}

/*
    Custom memory deallocation function.
*/
void mem_free(void * opaque, void * addr)
{
    free(addr);
}

/*********************************************
 * "Begin" of libsais Tests
 ********************************************/

class BZIP2_LIBSAIS : public ::testing::Test
{
public:
    string unBWT(vector<int> &dest, string s, int pointer)
    {
        // Here BWT string is constructed from sorted indexes.
        int n = s.size();
        string bwt = "";
        for(int i=0;i<n;i++)
        {
            bwt += (unsigned char)dest[i];
        }

        // Buckets of each character are initialized,
        // where each bucket contains number of occurances of a char in BWT str.
        int arr[256] = {0};
        for(int i=0;i<n;i++)
        {
            int j = (unsigned char)bwt[i];
            arr[j]++;
        }
        map<int,int> mp;
        vector<int> v(n);
        int j= n-1;

        // Sorting of `bwt` str, and storing rank a character is done.
        // Here rank of a character is not overall rank but local rank within a bucket.
        for(int i=255;i>=0;i--)
        {
            while(arr[i])
            {
                v[j] = arr[i]<<8 | i;
                arr[i]--;
                j--;
            }
        }
        j = 0;

        // Here for each "index:value" pair where:
        // v[index]=value;
        // `mp` stores the pair for reverse matching i.e,
        // mp[value]=index;
        for(auto c: bwt)
        {
            int l = (unsigned char)c;
            arr[l]++;
            int k = arr[l] << 8 | l;
            mp[k] = j++;
        }
        string ans = "";
        j=0;
        int k=pointer;

        // Inverse bijective transform of BWT.
        while(j<n)
        {
            int temp = v[k];
            ans += (unsigned char)(temp & 255);
            k = mp[temp];
            j++;
        }
        return ans;
    }
};


TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_1)
{
    /*
        Here n is length of string & m is length of LMS indexes.
        For current case n = 10, m = n/2
        this input checks the below condition in  function `libsais_gather_lms_suffixes_8u`
        "if(m <= (omp_block_size-1)/2) return;"
        And also last character is LMS character.
    */
    string s = "idzdzargib";
    int n = s.size();
    vector<int> dest = vector<int>(n+AOCL_LIBSAIS_FS);

    int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

    string unbwt = unBWT(dest, s, origIndex);
    EXPECT_EQ(unbwt, s);
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_2)
{
    /*
        First level n = 24, m = 10, second level n = 10, m = n/2
        in second level last index is LMS character and number of LMS indexes are half that of input length
        This input checks overlapping condition of LMS generated in `libsais_reconstruct_compacted_lms_suffixes_32s_2k_omp`
        If m = n/2, then there occurs overlap of one element between distinct and non-distinct element,
        we avoid this by storing the to be overlapped distinct element in a `temp` variable,
        by this input we check that condition.
        Also first character being LMS character is covered.
    */
    string s = "or/bookmarks/bookmarksOv";
    int n = s.size();
    vector<int> dest = vector<int>(n+AOCL_LIBSAIS_FS);

    int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

    string unbwt = unBWT(dest, s, origIndex);
    EXPECT_EQ(unbwt, s);
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_3)
{
    // In second level LMS indexes generated is only 1
    string s = "duprwuvuvn";
    int n = s.size();
    vector<int> dest = vector<int>(n+AOCL_LIBSAIS_FS);

    int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

    string unbwt = unBWT(dest, s, origIndex);
    EXPECT_EQ(unbwt, s);
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_4)
{
    /*
        For the below input, 8 LMS indexes are generated all renamed with same name in 1st level,
        so in the 2nd level 8 repeated characters are given as input array,
        so in the second level of function call zero LMS indexes are generated,
        this is a special case, to deal with such case without applying any LMS sorting we can directly initiate it to any order.
    */
    string s = " 0  0  0  0  0  0  0  0 ";

    int n = s.size();
    vector<int> dest = vector<int>(n+AOCL_LIBSAIS_FS);

    int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

    string unbwt = unBWT(dest, s, origIndex);
    EXPECT_EQ(unbwt, s);
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_5)
{
    /*
        Similar to previous test, except repeated characters are to be dealt in first level itself.
    */
    string s = "1111111111111111111111";

    int n = s.size();
    vector<int> dest = vector<int>(n+AOCL_LIBSAIS_FS);

    int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

    string unbwt = unBWT(dest, s, origIndex);
    EXPECT_EQ(unbwt, s);
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_6)
{
    /*
        In second level, no uniques are present i.e, `f=0` but m = n/2
    */
    string s = "mkkldmkkld";

    int n = s.size();
    vector<int> dest = vector<int>(n+AOCL_LIBSAIS_FS);

    int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

    string unbwt = unBWT(dest, s, origIndex);
    EXPECT_EQ(unbwt, s);
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_7)
{
    /*
        Inputs that caused failures while modifying libsais to BWT compatible code.
    */
    vector<string> test_input_strings = {
        "g.lag.lag.lag",
        "cadbcadbcadbc",
        "mmiissiissiippii",
        "nwlrbbmqbhcdarzowkkyhiddqscdxrjmowfrxsjybldbefsarcbynecdyggxxpklorellnmpapqfwkhopkmcoqhnwnkuewhsqmgb",
        "zvfrkmlnozjkpqpxrjxkitzyxacbhhkicqc",
        "wnpkwjkpbummzdzhayflrugawcbabrayhrk",
        "><int val='1'/><int val='~2'/><int val='1'/><eqCheck/><geqtimesgeeq><int val='1'/",
        "r id='25'/><int val='1'/><int val='~String:r id='25'/><int val='1'/><int val='~",
        "><andi><ander><anderString:><andi><ander><ander",
        "<andi><btr><ander><ander><ander><andel><",
        "ny></andi></andi></andi></lm></impi></an",
        "sel><sel><String:sel><sel><",
        "plateString:plate",
        "",
        "o",
        "oo"
    };

    for(string s: test_input_strings){

        int n = s.size();
        vector<int> dest = vector<int>(n+AOCL_LIBSAIS_FS);

        int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

        string unbwt = unBWT(dest, s, origIndex);
        EXPECT_EQ(unbwt, s);
    }
}

// Test cases 8, 9, and 10 use a large input to verify whether 2 bytes of padding are provided per recursion level.
TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_8)
{
    string s = "g.la";
    for (int i = 0; i < 18; i++)
        s += s;
    s = s + 'g';
    int n = s.size();
    vector<int> dest = vector<int>(n + AOCL_LIBSAIS_FS);

    int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

    string unbwt = unBWT(dest, s, origIndex);
    EXPECT_EQ(unbwt, s);
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_9) // Large inputs to generate, m = n/2, where n%2==0, throughout the recursive calls (except last few calls).
{
    vector<char> arr_input[2] = {{2, 1, 3, 0, 2, 1, 3, 0}, {2, 0, 3, 1, 2, 0, 3, 1}};
    for (int i = 0; i < 2; i++)
    {
        string s = "";
        for (auto c : arr_input[i])
            s += c;
        int j = 4;
        while (s.size() <= 1 << 20)
        {
            int n = s.size();
            vector<int> dest = vector<int>(n + AOCL_LIBSAIS_FS);

            int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

            string unbwt = unBWT(dest, s, origIndex);
            EXPECT_EQ(unbwt, s);

            string ss = s;
            s = "";
            for (auto c : ss)
            {
                s += j;
                s += c;
            }
            j++;
        }
    }
}

TEST_F(BZIP2_LIBSAIS, AOCL_Compression_libsais_pass_common_10) // Large inputs to generate, m = n/2, where n%2==1, throughout the recursive calls (except first call).
{
    vector<char> test_input_string = {0};
    string s = "";
    for (auto c : test_input_string)
        s += c;
    int j = 1;
    while (s.size() <= 1 << 20)
    {
        int n = s.size();
        vector<int> dest = vector<int>(n + AOCL_LIBSAIS_FS);

        int origIndex = Test_libsais((unsigned char *)s.data(), (int *)dest.data(), n, AOCL_LIBSAIS_FS, nullptr);

        string unbwt = unBWT(dest, s, origIndex);
        EXPECT_EQ(unbwt, s);

        string ss = s;
        s = "";
        for (auto c : ss)
        {
            s += j;
            s += c;
        }
        j++;
    }
}

/*********************************************
 * "End" of libsais Tests
 ********************************************/

/*********************************************
 * "Begin" of BZIP2_bzCompressInit Tests
 ********************************************/
class BZIP2_BZ2_bzCompressInit : public ::testing::Test
{
    public:
    EStateClass * strm = NULL;

    void SetUp() override {
        strm = new EStateClass();
    }

    void TearDown() override {
        delete strm;
    }

    void pass_check()
    {
        EXPECT_EQ(strm->getBlockNo(), 1);
        EXPECT_EQ(strm->getState(), BZ_S_INPUT);
        EXPECT_EQ(strm->getMode(), BZ_M_RUNNING);
        EXPECT_EQ(strm->getCombinedCRC(), 0);
        EXPECT_EQ(strm->getBlockCRC(),  0xffffffffL);
        EXPECT_EQ(strm->get_nBlock(), 0);
        EXPECT_EQ(strm->getStateInChar(), 256);
        EXPECT_EQ(strm->getStateInLen(), 0);
        EXPECT_EQ(strm->getNumZ(), 0);
        EXPECT_EQ(strm->getStateOutPos(), 0);

        EXPECT_NE(strm->getFtab(), nullptr);
        EXPECT_NE(strm->getPtr(), nullptr);
        EXPECT_NE(strm->getBlock(), nullptr);
        EXPECT_NE(strm->getMtfv(), nullptr);
        EXPECT_EQ(strm->getZbits(), nullptr);

        EXPECT_EQ(strm->getTotalInLo32(), 0);
        EXPECT_EQ(strm->getTotalInHi32(), 0);
        EXPECT_EQ(strm->getTotalOutLo32(), 0);
        EXPECT_EQ(strm->getTotalOutHi32(), 0);
    }
};

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_1)
{
    
    EXPECT_EQ(BZIP2_API::CompressInit(NULL, 5, 2, 2), BZ_PARAM_ERROR);               // strm is NULL
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_2)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 0, 2, 2), BZ_PARAM_ERROR);    // blockSize100 is less than 1
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_3)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 10, 3, 100), BZ_PARAM_ERROR); // blockSize100 is greater than 9
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_4)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 3, -1, 30), BZ_PARAM_ERROR);  // verbosity is less than 0
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_5)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 4, 5, 30), BZ_PARAM_ERROR);   // verbosity greater than 4
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_6)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 2, -1), BZ_PARAM_ERROR);   // workFactor less than 0
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_7)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 2, 2, 251), BZ_PARAM_ERROR);  // workFactor greater than 250
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_8)
{
    strm->setBzAlloc(mem_alloc_fail);   // Using custom memory allocation function which simply returns NULL
    strm->setBzFree(mem_free);
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 0), BZ_MEM_ERROR);
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_fail_common_9)
{
    strm->setBzAlloc(mem_alloc_fail_for_large_input);   // Using custom memory allocation function which returns NULL for large input
    strm->setBzFree(mem_free);
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 0), BZ_MEM_ERROR);
}


TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_pass_common_10)
{ 
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 0), BZ_OK); // All parameters are set to the least acceptable limit
    ASSERT_NE(strm->getStatePtr(), nullptr);
    pass_check();
    EXPECT_EQ(strm->getBlockSize100k(), 1);
    EXPECT_EQ(strm->getVerbosity(), 0);
    EXPECT_EQ(strm->getWorkFactor(), 30);
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_pass_common_11)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 1), BZ_OK); // workFactor is 1
    ASSERT_NE(strm->getStatePtr(), nullptr);
    pass_check();
    EXPECT_EQ(strm->getBlockSize100k(), 1);
    EXPECT_EQ(strm->getVerbosity(), 0);
    EXPECT_EQ(strm->getWorkFactor(), 1);
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_pass_common_12)
{
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 9, 4, 250), BZ_OK);   // All parameters are set to the largest acceptable limit
    ASSERT_NE(strm->getStatePtr(), nullptr);
    pass_check();
    EXPECT_EQ(strm->getBlockSize100k(), 9);
    EXPECT_EQ(strm->getVerbosity(), 4);
    EXPECT_EQ(strm->getWorkFactor(), 250);
}

TEST_F(BZIP2_BZ2_bzCompressInit, AOCL_Compression_bzip2_BZ2_bzCompressInit_pass_common_13)
{
    strm->setBzAlloc(mem_alloc_pass);   // Using custom memory allocation function
    strm->setBzFree(mem_free);          // Using custom memory deallocation function
    EXPECT_EQ(BZIP2_API::CompressInit(strm->getStrm(), 1, 0, 0), BZ_OK);
    ASSERT_NE(strm->getStatePtr(), nullptr);
    pass_check();
    EXPECT_EQ(strm->getBlockSize100k(), 1);
    EXPECT_EQ(strm->getVerbosity(), 0);
    EXPECT_EQ(strm->getWorkFactor(), 30);
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

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_fail_common_1)
{
    EXPECT_EQ(BZIP2_API::CompressEnd(NULL), BZ_PARAM_ERROR);             // strm is NULL
}

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_fail_common_2)
{
    EXPECT_EQ(BZIP2_API::CompressEnd(strm->getStrm()), BZ_PARAM_ERROR);  // strm is not initialized
}

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_fail_common_3)
{
    BZIP2_API::CompressInit(strm->getStrm(), 1, 1, 1);
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::CompressEnd(strm->getStrm()), BZ_PARAM_ERROR);  //  strm->state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_F(BZIP2_BZ2_bzCompressEnd, AOCL_Compression_bzip2_BZ2_bzCompressEnd_pass_common_4)
{
    BZIP2_API::CompressInit(strm->getStrm(), 1, 1, 1);
    EXPECT_EQ(BZIP2_API::CompressEnd(strm->getStrm()), BZ_OK);           //  pass case
    EXPECT_EQ(strm->getStatePtr(), nullptr);
}

/*********************************************
 * "End" of BZ2_bzCompressEnd Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzDecompressInit Tests
 ********************************************/

class BZIP2_BZ2_bzDecompressInit : public ::testing::Test
{
    public:
    DStateClass * strm = NULL;

    void SetUp() override {
        strm = new DStateClass();
    }

    void TearDown() override {
        delete strm;
    }

    void pass_check()
    {
        EXPECT_EQ(strm->getState(), BZ_X_MAGIC_1);
        EXPECT_EQ(strm->getbsLive(), 0);
        EXPECT_EQ(strm->getbsBuff(), 0);
        EXPECT_EQ(strm->getCalculatedCombinedCRC(), 0);
        EXPECT_EQ(strm->getTotalInLo32(), 0);
        EXPECT_EQ(strm->getTotalInHi32(), 0);
        EXPECT_EQ(strm->getTotalOutLo32(), 0);
        EXPECT_EQ(strm->getTotalOutHi32(), 0);
        
        EXPECT_EQ(strm->getll4(), nullptr);
        EXPECT_EQ(strm->getll16(), nullptr);
        EXPECT_EQ(strm->gettt(), nullptr);
        EXPECT_EQ(strm->getCurrentBlockNo(), 0);
    }
};

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_fail_common_1)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(NULL, 2, 0), BZ_PARAM_ERROR);                // strm is NULL
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_fail_common_2)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 2, -1), BZ_PARAM_ERROR);    // small is less than 0
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_fail_common_3)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 2, 2), BZ_PARAM_ERROR);     // small is greater than 1
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_fail_common_4)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), -1, 0), BZ_PARAM_ERROR);    // verbosity is less than 0
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_fail_common_5)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 5, 0), BZ_PARAM_ERROR);     // verbosity is greater than 4
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_fail_common_6)
{
    strm->setBzAlloc(mem_alloc_fail);   // Using custom memory allocation function which simply returns NULL
    strm->setBzFree(mem_free);
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 4, 1), BZ_MEM_ERROR);
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_pass_common_7)
{
    strm->setBzAlloc(mem_alloc_pass);   // Using custom memory allocation function
    strm->setBzFree(mem_free);          // Using custom memory deallocation function
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 4, 1), BZ_OK);
    ASSERT_NE(strm->getStatePtr(), nullptr);
    pass_check();
    EXPECT_EQ(strm->getVerbosity(), 4);
    EXPECT_EQ(strm->getSmallDecompress(), 1);
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_pass_common_8)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 0, 0), BZ_OK);  // pass case, all arguments are least acceptable values
    ASSERT_NE(strm->getStatePtr(), nullptr);
    pass_check();
    EXPECT_EQ(strm->getVerbosity(), 0);
    EXPECT_EQ(strm->getSmallDecompress(), 0);
}

TEST_F(BZIP2_BZ2_bzDecompressInit, AOCL_Compression_bzip2_BZ2_bzDecompressInit_pass_common_9)
{
    EXPECT_EQ(BZIP2_API::DecompressInit(strm->getStrm(), 4, 1), BZ_OK);  // pass case, all arguments are highest acceptable values
    ASSERT_NE(strm->getStatePtr(), nullptr);
    pass_check();
    EXPECT_EQ(strm->getVerbosity(), 4);
    EXPECT_EQ(strm->getSmallDecompress(), 1);
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

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_fail_common_1)
{
    EXPECT_EQ(BZIP2_API::DecompressEnd(NULL), BZ_PARAM_ERROR);               // strm is NULL
}

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_fail_common_2)
{
    EXPECT_EQ(BZIP2_API::DecompressEnd(strm->getStrm()), BZ_PARAM_ERROR);    // strm is not initialized
}

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_fail_common_3)
{
    BZIP2_API::DecompressInit(strm->getStrm(), 1, 1);
    strm->setDState(strm->getStatePtr());
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::DecompressEnd(strm->getStrm()), BZ_PARAM_ERROR);    // strm->state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_F(BZIP2_BZ2_bzDecompressEnd, AOCL_Compression_bzip2_BZ2_bzDecompressEnd_pass_common_4)
{
    BZIP2_API::DecompressInit(strm->getStrm(), 1, 1);
    EXPECT_EQ(BZIP2_API::DecompressEnd(strm->getStrm()), BZ_OK);             // pass case
    EXPECT_EQ(strm->getStatePtr(), nullptr);
}

/*********************************************
 * "End" of BZ2_bzDecompressEnd Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzBuffToBuffCompress Tests
 ********************************************/

class BZIP2_BZ2_bzBuffToBuffCompress : public OPT_LEVEL_TESTS
{
    public:
    char dest[20];
    unsigned int destLen = 20;
    char source[11] = "helloWorld";
    unsigned int sourceLen = 10;
    int blockSize100k = 5;
    int verbosity = 0;
    int workFactor = 100;
    
    unsigned int sourceLenPassCase = 600000;
    const unsigned int destSize = (float)sourceLenPassCase * 1.01 + 600;
    vector<char> sourcePass;
    unsigned int destLenPass = destSize;
    vector<char> destPass;

    void SetUp() override {
        aocl_destroy_bzip2();
        DynamicDispatch opt = GetParam();
        aocl_setup_bzip2(opt.optOff, opt.optLevel, 0, 0, 0);
    }

    void Init()
    {
        sourcePass = vector<char>(sourceLenPassCase, 'a');
        destPass = vector<char>(destLenPass, 'a');
        // Initilizing `source` with random data
        for(unsigned int i = 0; i < sourceLenPassCase; i++)
        {
            sourcePass[i] = rand() % 255;
        }
    }
};

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_1)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(NULL, &destLen, source, sourceLen, blockSize100k, verbosity, workFactor), BZ_PARAM_ERROR); // dest is NULL
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_2)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, NULL, source, sourceLen, blockSize100k, verbosity, workFactor), BZ_PARAM_ERROR);     // destLen is NULL
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_3)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, NULL, sourceLen, blockSize100k, verbosity, workFactor), BZ_PARAM_ERROR);   // source is NULL
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_4)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, 0, verbosity, workFactor), BZ_PARAM_ERROR);             // blockSize100k is less than 1
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_5)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, 10, verbosity, workFactor), BZ_PARAM_ERROR);            // blockSize100k is greater than 9
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_6)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, -1, workFactor), BZ_PARAM_ERROR);        // verbosity is less than 0
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_7)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, 5, workFactor), BZ_PARAM_ERROR);         // verbosity is greater than 5
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_8)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, verbosity, -1), BZ_PARAM_ERROR);         // workFactor is less than 0
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_9)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, verbosity, 251), BZ_PARAM_ERROR);        // workFactor is greater than 250
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_fail_common_10)
{
    destLen = 5;    // dest will not have enough space to contain compressed data, so the below test case will return `BZ_OUTBUFF_FULL`
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest, &destLen, source, sourceLen, blockSize100k, verbosity, workFactor), BZ_OUTBUFF_FULL); 
}


TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_11)
{
    Init();
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 1, verbosity, 0), BZ_OK);            // parameters are set to the least acceptable values.
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_12)
{
    Init();
    sourceLenPassCase = 1;  // This is to reduce all the verbose output
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 9, 4, 250), BZ_OK);          // parameters are set to highest acceptable values.
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_13)
{
    Init();
    // destLenPass = destSize;
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, blockSize100k, verbosity, 1), BZ_OK);// workFactor is set to 1
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_14)
{
    Init();
    sourceLen = 0;  // 0 input size
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLen, blockSize100k, verbosity, 0), BZ_OK);

    is_valid_bzip2_frame(destPass.data());
    unsigned int uncompressedLen = 1;
    vector<char> uncompressedBuf(uncompressedLen, 'a');
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressedBuf.data(), &uncompressedLen, destPass.data(), destLenPass, 0, 0), BZ_OK);
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_15)
{
    Init();
    for(auto &c: sourcePass)
    {
        c = 'a';    // all input data is same, to make the algorithm switch to fallback algorithm.
    }
    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 1, verbosity, 0), BZ_OK);
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_16)
{
    for(int i=1;i<15;i++)
    {
        sourceLenPassCase = i;
        destLenPass = i+600;
        Init();
        EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 1, verbosity, 0), BZ_OK);            // parameters are set to the least acceptable values.
        EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
    }
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_17)
{
    for(int j=1;j<=8;j++)
    {
        sourceLenPassCase = 256*j;
        destLenPass = sourceLenPassCase+600;
        Init();

        // Increasing characters pattern
        for(int i=0;i<sourceLenPassCase;i++)
        {
            sourcePass[i] = i%256;
        }
        EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 1, verbosity, 0), BZ_OK);            // parameters are set to the least acceptable values.
        EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));

        int k = 0;
        // Decreasing characters pattern
        for(int i=sourceLenPassCase-1;i>=0;i--)
        {
            sourcePass[k++] = i%256;
        }
        destLenPass = sourceLenPassCase+600;
        EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 1, verbosity, 0), BZ_OK);            // parameters are set to the least acceptable values.
        EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
    }
}

TEST_P(BZIP2_BZ2_bzBuffToBuffCompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffCompress_pass_common_18)
{
    sourceLenPassCase = 256*32;
    destLenPass = sourceLenPassCase+600;
    Init();

    // Generate input containing all the characters
    for(int i=0;i<sourceLenPassCase;i++)
    {
        sourcePass[i] = i%256;
    }

    // Shuffle the pattern generated
    for(int i = 0;i<sourceLenPassCase;i++)
    {
        int a = rand()%sourceLenPassCase;
        int b = rand()%sourceLenPassCase;
        swap(sourcePass[a], sourcePass[b]);
    }

    EXPECT_EQ(BZIP2_API::BuffToBuffCompress(destPass.data(), &destLenPass, sourcePass.data(), sourceLenPassCase, 1, verbosity, 0), BZ_OK);            // parameters are set to the least acceptable values.
    EXPECT_TRUE(verify_uncompressed_equal_original(destPass.data(), destLenPass, sourcePass.data(), sourceLenPassCase));
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzBuffToBuffCompress,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzBuffToBuffCompress Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzBuffToBuffDecompress Tests
 ********************************************/

class BZIP2_BZ2_bzBuffToBuffDecompress : public OPT_LEVEL_TESTS
{
    public:
    const unsigned int sourceLen = 600;
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
        // Code path setup
        {
            aocl_destroy_bzip2();
            DynamicDispatch opt = GetParam();
            aocl_setup_bzip2(opt.optOff, opt.optLevel, 0, 0, 0);
        }

        source = vector<char>(sourceLen, 'a');
        uncompressed = vector<char>(sourceLen, 'a');
        // Initializing `source` with random data
        for(unsigned int i = 0; i < sourceLen; i++)
        {
            source[i] = rand() % 255;
        }
        dest = vector<char>(destLen, 'a');
        EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest.data(), &destLen, source.data(), sourceLen, 1, verbosity, 0), BZ_OK);
    }

};

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_1)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(NULL, &uncompressedLen, source.data(), sourceLen, small, verbosity), BZ_PARAM_ERROR);                // dest is NULL
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_2)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), NULL, source.data(), sourceLen, small, verbosity), BZ_PARAM_ERROR);             // destLen is NULL
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_3)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, NULL, sourceLen, small, verbosity), BZ_PARAM_ERROR);          // source is NULL
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_4)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, -1, verbosity), BZ_PARAM_ERROR);    // small is less than 0
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_5)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, 2, verbosity), BZ_PARAM_ERROR);     // small is greater than 1
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_6)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, small, -1), BZ_PARAM_ERROR);        // verbosity is less than 0}
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_7)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, source.data(), sourceLen, small, 5), BZ_PARAM_ERROR);         // verbosity greater than 4
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_8)
{
    for(int i=0; i<=2 ;i++)
    {
        dest[i]++;  // Modifying compressed buffer at index 0 to 2 to check if the method returns `BZ_DATA_ERROR_MAGIC`
        EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR_MAGIC);
        dest[i]--;
    }
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_9)
{
    dest[3]='0';    // blockSize100k < 1
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR_MAGIC);

    dest[3]='0'+10; // blockSize100k > 9
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR_MAGIC);
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_10)
{
    for(int i=4; i<=9 ;i++)
    {
        dest[i]++;  // Modifying magic number of individual blocks to check if the method returns `BZ_DATA_ERROR`
        EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR);
        dest[i]--;
    }
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_11)
{
    for(int i=10; i<=13 ;i++)
    {
        dest[i]++;  // Modifying storedBlockCRC value of individual blocks to check if the method returns `BZ_DATA_ERROR`
        EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR);
        dest[i]--;
    }
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_12)
{
    dest[14] = 1+(((dest[3]-'0')*100000+11)/256/256);   // origPtr of block exceeds its blocksize `BZ_DATA_ERROR`
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_DATA_ERROR);
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_13)
{
    uncompressedLen = sourceLen - 100;  // Providing insufficient  `dest` buffer to check if the method returns `BZ_OUTBUFF_FULL`
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_OUTBUFF_FULL);
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_fail_common_14)
{
    // The end part of compressed buffer is not provided to check if the method returns `BZ_UNEXPECTED_EOF`
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen-100, small, verbosity), BZ_UNEXPECTED_EOF);
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_pass_common_15)
{
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, 1, 0), BZ_OK);
    EXPECT_EQ(uncompressedLen, sourceLen);
    EXPECT_EQ(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_pass_common_16)
{
    for(int i=1;i<10;i++) // blockSize100k = 1 to 9 should pass the test
    {
        dest[3]='0'+i;
        EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, small, verbosity), BZ_OK);
    }
}

TEST_P(BZIP2_BZ2_bzBuffToBuffDecompress, AOCL_Compression_bzip2_BZ2_bzBuffToBuffDecompress_pass_common_17)   // uncompressed buffer length > original buffer length
{
    uncompressedLen = sourceLen + 100;
    uncompressed = vector<char>(uncompressedLen, 'a');
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressed.data(), &uncompressedLen, dest.data(), destLen, 1, 0), BZ_OK);
    EXPECT_EQ(uncompressedLen, sourceLen);
    EXPECT_EQ(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzBuffToBuffDecompress,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzBuffToBuffDecompress Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzCompress Tests
 ********************************************/

class BZIP2_BZ2_bzCompress : public OPT_LEVEL_TESTS
{
    protected:
    EStateClass * strm = NULL;
    public:
    vector<char> input;
    vector<char> output;
    unsigned int inputSize, outputSize, inputGiven;

    void SetUp() override
    {
        // Code path setup
        {
            aocl_destroy_bzip2();
            DynamicDispatch opt = GetParam();
            aocl_setup_bzip2(opt.optOff, opt.optLevel, 0, 0, 0);
        }

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

class BZIP2_BZ2_bzCompress_failCases : public OPT_LEVEL_TESTS
{
    public:
    EStateClass * strm = NULL;

    void SetUp() override
    {
        // Code path setup
        {
            aocl_destroy_bzip2();
            DynamicDispatch opt = GetParam();
            aocl_setup_bzip2(opt.optOff, opt.optLevel, 0, 0, 0);
        }

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

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_1)
{
    EXPECT_EQ(BZIP2_API::Compress(NULL, BZ_M_RUNNING), BZ_PARAM_ERROR);              // strm is NULL
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_2)
{
    EStateClass * strm2 = new EStateClass();
    strm2->setEState(NULL);
    EXPECT_EQ(BZIP2_API::Compress(strm2->getStrm(), BZ_M_RUNNING), BZ_PARAM_ERROR);   // strm is not initialized
    delete strm2;
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_3)
{
    strm->setEState(strm->getStatePtr());
    strm->setMode(BZ_M_RUNNING);
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_PARAM_ERROR);         // strm->state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_4)
{
    strm->setEState(strm->getStatePtr());
    strm->setMode(-1);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_PARAM_ERROR);         // mode is -1
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_5)
{
    strm->setMode(BZ_M_IDLE);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_SEQUENCE_ERROR);      // mode is `BZ_M_IDLE`
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_6)
{
    strm->setMode(BZ_M_IDLE);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_SEQUENCE_ERROR);    // mode is `BZ_M_IDLE`
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_7)
{
    strm->setMode(BZ_M_IDLE);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // mode is `BZ_M_IDLE`
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_8)
{
    strm->setMode(BZ_M_RUNNING);
    strm->setAvailInExpect(1);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // no input data is provided
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_9)
{
    strm->setMode(BZ_M_RUNNING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_PARAM_ERROR);         // no input data is provided
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_10)
{
    strm->setMode(BZ_M_RUNNING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), -1), BZ_PARAM_ERROR);             // action is -1
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_11)
{
    strm->setMode(BZ_M_FLUSHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_SEQUENCE_ERROR);      // action should be equal to BZ_FLUSH
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_12)
{
    strm->setMode(BZ_M_FLUSHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // action should be equal to BZ_FLUSH
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_13)
{
    strm->setMode(BZ_M_FLUSHING);
    strm->setAvailIn(1);
    strm->setAvailInExpect(0);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_SEQUENCE_ERROR);    // avail_in != avail_in_expect
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_14)
{
    strm->setMode(BZ_M_FINISHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_SEQUENCE_ERROR);      // action should be equal to BZ_M_FINISHING
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_15)
{
    strm->setMode(BZ_M_FINISHING);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_SEQUENCE_ERROR);    // action should be equal to BZ_M_FINISHING    
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_16)
{
    strm->setMode(BZ_M_FINISHING);
    strm->setAvailIn(1);
    strm->setAvailInExpect(0);
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // avail_in != avail_in_expect
}

TEST_P(BZIP2_BZ2_bzCompress_failCases, AOCL_Compression_bzip2_BZ2_bzCompress_fail_common_17)
{
    strm->setMode(BZ_M_FINISHING);
    strm->setAvailInExpect(strm->getAvailIn());
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_SEQUENCE_ERROR);   // no input is available to process
}

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_18)
{
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_RUN), BZ_RUN_OK);              // mode is BZ_M_RUNNING, pass case with BZ_RUN
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_19)
{
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FLUSH), BZ_RUN_OK);            // mode is BZ_M_RUNNING, pass case with BZ_FLUSH
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);       
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_20)
{
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);       // mode is BZ_M_RUNNING, pass case with BZ_FINISH
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_21)
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

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_22)
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

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_23)
{
    strm->setMode(BZ_M_FINISHING);
    strm->setAvailInExpect(strm->getAvailIn());
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);      //  pass case for BZ_FINISH with mode BZ_M_FINISHING
    EXPECT_TRUE(verify_uncompressed_equal_original(output.data(), outputSize - strm->getAvailOut(), input.data(), inputSize));
}

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_24) // mode is BZ_M_FINISHING
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

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_25) // mode is BZ_M_FINISHING
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

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_26) // mode BZ_M_FINISHING
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

TEST_P(BZIP2_BZ2_bzCompress, AOCL_Compression_bzip2_BZ2_bzCompress_pass_common_27)
{
    strm->setAvailIn(0);    //  pass case for input size = 0
    strm->setAvailInExpect(strm->getAvailIn());
    EXPECT_EQ(BZIP2_API::Compress(strm->getStrm(), BZ_FINISH), BZ_STREAM_END);      

    is_valid_bzip2_frame(output.data());
    unsigned int uncompressedLen = 1;
    vector<char> uncompressedBuff(1, 'a');
    EXPECT_EQ(BZIP2_API::BuffToBuffDecompress(uncompressedBuff.data(), &uncompressedLen, output.data(), outputSize - strm->getAvailOut(), 0, 0), BZ_OK);
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzCompress_failCases,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzCompress,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzCompress Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzDecompress Tests
 ********************************************/

class BZIP2_BZ2_bzDecompress : public OPT_LEVEL_TESTS
{
    protected:
    DStateClass * strm = NULL;
    public:
    const unsigned int sourceLen = 6000;
    unsigned int uncompressedLen = sourceLen;
    const unsigned int destSize = sourceLen * 1.01 + 600;
    unsigned int destLen = destSize;
    int blockSize100k = 1;
    int verbosity = 0;
    int workFactor = 0;
    vector<char> source, dest, uncompressed;
    
    void SetUp() override
    {
        // Code path setup
        {
            aocl_destroy_bzip2();
            DynamicDispatch opt = GetParam();
            aocl_setup_bzip2(opt.optOff, opt.optLevel, 0, 0, 0);
        }

        destLen = destSize;
        uncompressedLen = sourceLen;
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
        EXPECT_EQ(BZIP2_API::BuffToBuffCompress(dest.data(), &destLen, source.data(), sourceLen, blockSize100k, verbosity, workFactor), BZ_OK);
    }

    void Init()
    {
        strm->setDState(strm->getStatePtr());
        strm->setAvailIn(destLen);
        strm->setAvailOut(uncompressedLen);
        strm->setNextIn(dest.data());
        strm->setNextOut(uncompressed.data());
    }

    void TearDown() override {
        
        if(strm != NULL)
        {
            BZIP2_API::DecompressEnd(strm->getStrm());
            delete strm;
        }
    }
};

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_1)
{
    EXPECT_EQ(BZIP2_API::Decompress(NULL), BZ_PARAM_ERROR);              // NULL is passed
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_2)
{
    DState * temp_ptr = strm->getStatePtr();
    strm->setDState(NULL);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_PARAM_ERROR);   // state is pointed to NULL
    strm->setDState(temp_ptr);
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_3)
{
    strm->setDState(strm->getStatePtr());
    strm->setStream(NULL);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_PARAM_ERROR);   // state->strm != strm
    strm->setStream(strm->getStrm());
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_4)
{
    int temp_state = strm->getState();
    strm->setState(BZ_X_IDLE);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_SEQUENCE_ERROR);   // state is IDLE
    strm->setState(temp_state);
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_5)
{
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    dest[0]++;  // Modifying compressed buffer at 0 index to check if the method returns `BZ_DATA_ERROR_MAGIC`
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR_MAGIC);     // BZ_DATA_ERROR_MAGIC , s->state != BZ_X_OUTPUT
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_6)
{
    dest[15]++; // Modifying compressed buffer at 15 index to check if the method returns `BZ_DATA_ERROR`
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);           // BZ_DATA_ERROR, BZ_X_OUTPUT && s->calculatedBlockCRC != s->storedBlockCRC
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_7) // This case is for covering `return BZ_OK;` in the first while loop in BZ2_bzDcompress
{
    strm->setDState(strm->getStatePtr());
    strm->setAvailIn(destLen);
    strm->setAvailOut(0);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    strm->setnBlockUsed(strm->getnBlock()+2);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_OK); // BZ_OK

    EXPECT_NE(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_8) // Fail
{
    strm->setDState(strm->getStatePtr());
    strm->setState(BZ_X_OUTPUT);

    strm->setBlockRandomised(0);
    strm->setStateOutLen(0);
    strm->setnBlockUsed(strm->getnBlock()+2);       // This condition ensures that unRLE_obuf_to_output_FAST returns true(corrupt data),
                                                    // which will cause BZ2_bzDecompress to return BZ_DATA_ERROR, this test case ensures
                                                    // that condition is covered.
    strm->setSmallDecompress(0);
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);  // BZ_DATA_ERROR
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_9) // Same as previous test case, but with smallDecompress = 1 & avail_out = 1
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

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_10)
{
    strm->setDState(strm->getStatePtr());
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    dest[destLen-2]--;  // Corrupting compressed buffer at index destLen-2 to check if the method returns `BZ_DATA_ERROR`
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR); // BZ_DATA_ERROR
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_11)
{
    Init();

    for(int i=0; i<=2 ;i++)
    {
        dest[i]++;  // Modifying compressed buffer at index 0 to 2 to check if the method returns `BZ_DATA_ERROR_MAGIC`
        EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR_MAGIC);
        dest[i]--;
    }
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_12)
{
    Init();

    dest[3]='0';    // blockSize100k < 1
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR_MAGIC);
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_13)
{
    Init();

    dest[3]='0'+10; // blockSize100k > 9
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR_MAGIC);
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_14)
{
    Init();

    for(int i=4; i<=9 ;i++)
    {
        dest[i]++;  // Modifying magic number of individual blocks to check if the method returns `BZ_DATA_ERROR`
        EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);
        dest[i]--;
    }
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_15)
{
    Init();

    for(int i=10; i<=13 ;i++)
    {
        dest[i]++;  // Modifying storedBlockCRC value of individual blocks to check if the method returns `BZ_DATA_ERROR`
        EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);
        dest[i]--;
    }
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_fail_common_16)
{
    Init();

    dest[14] = 1+(((dest[3]-'0')*100000+11)/256/256);   // origPtr of block exceeds its blocksize `BZ_DATA_ERROR`
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_DATA_ERROR);
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_pass_common_17)
{
    for(int i=1;i<10;i++) // blockSize100k = 1 to 9 should pass the test
    {
        TearDown();
        SetUp();
        Init();
        dest[3]='0'+ i;
        EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_STREAM_END);
    }
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_pass_common_18) // pass
{
    strm->setDState(strm->getStatePtr());
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());
    EXPECT_EQ(BZIP2_API::Decompress(strm->getStrm()), BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}

TEST_P(BZIP2_BZ2_bzDecompress, AOCL_Compression_bzip2_BZ2_bzDecompress_pass_common_19) // pass
{
    strm->setDState(strm->getStatePtr());
    strm->setAvailIn(destLen);
    strm->setAvailOut(uncompressedLen);
    strm->setNextIn(dest.data());
    strm->setNextOut(uncompressed.data());

    int temp_destLen = destLen;
    strm->setAvailIn(0);
    int decompress_return = 0;

    while (temp_destLen > 0)
    {
        if(strm->getAvailIn() == 0)
        {
            temp_destLen--;
            strm->setAvailIn(1); // Compressed data is supplied one byte at a time.
        }
        strm->setAvailOut(strm->getAvailOut()+1); // Trying to remove data one byte at a time.
        decompress_return = BZIP2_API::Decompress(strm->getStrm());
    }
    
    EXPECT_EQ(decompress_return, BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed.data(), source.data(), sourceLen), 0);
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzDecompress,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzDecompress Tests
 ********************************************/


/*********************************************
 * "Begin" of BZ2_bzlibVersion Tests
 ********************************************/

class BZIP2_BZ2_bzlibVersion : public ::testing::Test
{};

TEST_F(BZIP2_BZ2_bzlibVersion, AOCL_Compression_bzip2_BZ2_bzlibVersion_common_1)
{
    EXPECT_STREQ(BZIP2_API::LibVersion(), BZ_VERSION);
}

/*********************************************
 * "End" of BZ2_bzlibVersion Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzWriteOpen Tests
 ********************************************/

class BZIP2_BZ2_bzWriteOpen : public ::testing::Test
{
    public:
    int bzerror = 0;
    int blockSize100k = 1;
    int verbosity = 0;
    int workFactor = 0;
    FILE * pFile = NULL;
    BZFILE * bzf = NULL;
    string file_name = "";

    void SetUp() override
    {
        file_name = get_file_name();

        pFile = fopen(file_name.c_str(), "wb");
    }

    void pass(int blockSize100k, int verbosity, int workFactor)
    {
        bzFile * bzf_ptr_temp = (bzFile *)bzf;
        ASSERT_NE(bzf_ptr_temp, nullptr);
        EXPECT_EQ(bzf_ptr_temp->initialisedOk, true);
        EXPECT_EQ(bzf_ptr_temp->bufN, 0);
        EXPECT_EQ(bzf_ptr_temp->writing, true);
        EXPECT_EQ(bzf_ptr_temp->strm.avail_in, 0);
        
        EState *bz = (EState *)(bzf_ptr_temp->strm.state);
        ASSERT_NE(bz, nullptr);
        EXPECT_EQ(bz->blockSize100k, blockSize100k);
        EXPECT_EQ(bz->verbosity, verbosity);
        EXPECT_EQ(bz->workFactor, workFactor);
    }

    void TearDown() override {
        if(pFile)
            fclose(pFile);
        remove(file_name.c_str());
    }
};

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_1) // pFile is NULL
{
    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, NULL, blockSize100k, verbosity, workFactor), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_2) // blockSize100k is less than 1
{
    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, pFile, 0, verbosity, workFactor), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_3) // blockSize100k is greater than 9
{
    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, pFile, 10, verbosity, workFactor), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_4) // verbosity is less than 0
{
    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, pFile, blockSize100k, -1, workFactor), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_5) // verbosity is greater than 4
{
    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, pFile, blockSize100k, 5, workFactor), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_6) // workFactor is less than 0
{
    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, pFile, blockSize100k, verbosity, -1), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_7) // workFactor is greater than 250
{
    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, pFile, blockSize100k, verbosity, 251), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_fail_common_8) // Deliberately introducing error in file ptr which gets caught by ferror()
{
    string temp_file = "temp_"+ file_name;
    FILE * err_file_temp = fopen(temp_file.c_str(), "w"); 
    char c = fgetc(err_file_temp);  // trying to read from a write only mode file, which will set the error flag

    EXPECT_EQ(BZIP2_API::WriteOpen(&bzerror, err_file_temp, blockSize100k, verbosity, workFactor), nullptr);
    
    EXPECT_EQ(bzerror, BZ_IO_ERROR);
    fclose(err_file_temp);
    remove(temp_file.c_str());
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_pass_common_9)
{
    bzf = BZIP2_API::WriteOpen(&bzerror, pFile, blockSize100k, verbosity, 0); // WorkFactor is 0, which will change to 30, as per the implementation
    EXPECT_NE(bzf, nullptr);
    EXPECT_EQ(bzerror, BZ_OK);
    pass(blockSize100k, verbosity, 30); // Since workFactor will change to 30

    BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL); // Deallocation of bzf and associated buffers
    EXPECT_EQ(bzerror, BZ_OK);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_pass_common_10)
{
    bzf = BZIP2_API::WriteOpen(&bzerror, pFile, 9, 4, 250); // All parameters are set to highest levels.
    EXPECT_NE(bzf, nullptr);
    EXPECT_EQ(bzerror, BZ_OK);
    pass(9, 4, 250);

    BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL); // Deallocation of bzf and associated buffers
    EXPECT_EQ(bzerror, BZ_OK);
}

TEST_F(BZIP2_BZ2_bzWriteOpen, AOCL_Compression_bzip2_BZ2_bzWriteOpen_pass_common_11)
{
    bzf = BZIP2_API::WriteOpen(&bzerror, pFile, 1, 0, 1); // All parameters are set to lowest levels.
    EXPECT_NE(bzf, nullptr);
    EXPECT_EQ(bzerror, BZ_OK);
    pass(1, 0, 1);

    BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL); // Deallocation of bzf and associated buffers
    EXPECT_EQ(bzerror, BZ_OK);
}

/*********************************************
 * "End" of BZ2_bzWriteOpen Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzWrite Tests
 ********************************************/

class BZIP2_BZ2_bzWrite : public OPT_LEVEL_TESTS
{
    public:
    vector<char> orig_buf;
    int orig_len = 1000;
    int bzerror = 0;
    int blockSize100k = 1;
    int verbosity = 0;
    int workFactor = 0;
    FILE * pFile = NULL;
    BZFILE * bzf = NULL;
    string file_name = "";

    void SetUp() override
    {
        // Code path setup
        {
            aocl_destroy_bzip2();
            DynamicDispatch opt = GetParam();
            aocl_setup_bzip2(opt.optOff, opt.optLevel, 0, 0, 0);
        }

        file_name =  get_file_name();

        pFile = fopen(file_name.c_str(), "wb");
        bzf = BZIP2_API::WriteOpen(&bzerror, pFile, blockSize100k, verbosity, workFactor);
        orig_buf = vector<char>(orig_len);
        for(int i=0; i<orig_len; i++)
        {
            orig_buf[i] = rand() % 255;
        }
    }

    bool verify_bzip2_file(vector<char> &original_buf, int original_len, string file_name)
    {
        FILE * pFile = fopen(file_name.c_str(), "rb");
        if(pFile == NULL)
            return false;
        int bz_err = 0;

        // Initialize bzfile
        BZFILE * bzfile = BZIP2_API::ReadOpen(&bz_err, pFile, verbosity, 0, NULL, 0);
        EXPECT_NE(bzfile, nullptr);
        EXPECT_EQ(bz_err, BZ_OK);

        // Decompress to uncompressed_buf
        int uncompressed_len = original_len;
        vector<char> uncompressed_buf = vector<char>(uncompressed_len);
        uncompressed_len = BZIP2_API::Read(&bz_err, bzfile, uncompressed_buf.data(), uncompressed_len);
        EXPECT_EQ(bz_err, BZ_STREAM_END);

        // Verify if original_buf matches with uncompressed_buf & thier lengths are same
        EXPECT_EQ(uncompressed_len, original_len);
        int memcmp_result = memcmp(original_buf.data(), uncompressed_buf.data(), original_len);
        EXPECT_EQ(memcmp_result, 0);

        // Deallocating bzfile and associated buffers
        BZIP2_API::ReadClose(&bz_err, bzfile);
        fclose(pFile);
        return memcmp_result == 0;
    }

    void TearDown() override {
        if(bzf)
            BZ2_bzWriteClose(&bzerror, bzf, 0, NULL, NULL);
        if(pFile)
            fclose(pFile);
        bzf = NULL;
        pFile = NULL;
        remove(file_name.c_str());
    }
};

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_fail_common_1) // bzf is NULL
{
    BZIP2_API::Write(&bzerror, NULL, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_fail_common_2) // buff is NULL
{
    BZIP2_API::Write(&bzerror, bzf, NULL, orig_len);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_fail_common_3) // length is negative
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), -1);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_fail_common_4) // bzf->writing is set to 0
{
    bzFile * bzf_ptr_temp = (bzFile *)bzf;
    int writing_temp = bzf_ptr_temp->writing;
    bzf_ptr_temp->writing = 0;

    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);

    bzf_ptr_temp->writing = writing_temp;
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_fail_common_5) // Deliberately introducing error in file ptr which gets caught by ferror()
{
    bzFile * bzf_ptr_temp = (bzFile *)bzf;
    FILE * file_handle_temp = bzf_ptr_temp->handle;

    string temp_file_name = "temp_"+ file_name;
    FILE * err_file_temp = fopen(temp_file_name.c_str(), "w"); 
    char c = fgetc(err_file_temp);  // trying to read from a write only mode file, which will set the error flag
    bzf_ptr_temp->handle = err_file_temp;

    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_IO_ERROR);

    bzf_ptr_temp->handle = file_handle_temp;
    fclose(err_file_temp);
    remove(temp_file_name.c_str());
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_fail_common_6) // bzf->strm.state is set to NULL
{
    bzFile * temp_bzf_ptr = (bzFile *)bzf;
    void * temp_state = temp_bzf_ptr->strm.state;
    temp_bzf_ptr->strm.state = NULL;

    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);  // internal call to BZ2_bzCompress throws error
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);

    temp_bzf_ptr->strm.state = temp_state;
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_fail_common_7)
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len); 
    EXPECT_EQ(bzerror, BZ_OK);

    // BZ2_bzWriteClose is not called yet, so the last block is not written to the file.
    // So, the file is corrupt and BZ2_bzRead will throw (BZ_UNEXPECTED_EOF)
    {
        FILE * pFile = fopen(file_name.c_str(), "rb");
        int bzerror = 0;
        BZFILE * bzfile = BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, NULL, 0);
        EXPECT_NE(bzfile, nullptr);
        EXPECT_EQ(bzerror, BZ_OK);

        // Reading the file and check for errors.
        int uncompressed_len = orig_len;
        vector<char>uncompressed(uncompressed_len);
        uncompressed_len = BZIP2_API::Read(&bzerror, bzfile, uncompressed.data(), uncompressed_len);
        EXPECT_EQ(bzerror, BZ_UNEXPECTED_EOF);
        fclose(pFile);

        BZIP2_API::ReadClose(&bzerror, bzfile); // Freeing the memory
    }
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_pass_common_8) // simple pass case
{
    orig_len = 1e5;
    vector<char> orig_buf(orig_len);
    for(int i = 0; i < orig_len; i++)
    {
        orig_buf[i] = rand() % 255;
    }

    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    // Leftover bytes are written, and memory is freed by calling BZ2_bzWriteClose
    BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL);
    EXPECT_EQ(bzerror, BZ_OK);
    // BZIP2_API::WriteClose has freed bzf memory, but still bzf != NULL
    // so setting it to NULL will avoid calling WriteClose again in TearDown()
    bzf = NULL;

    EXPECT_TRUE(verify_bzip2_file(orig_buf, orig_len, file_name));
}

TEST_P(BZIP2_BZ2_bzWrite, AOCL_Compression_bzip2_BZ2_bzWrite_pass_common_9)
{
    orig_len = 1e5;
    orig_buf = vector<char>(orig_len);
    for(int i=0; i < orig_len; i++)
    {
        orig_buf[i] = rand() % 255;
    }
    for(int i=0; i < orig_len; i++)  // BZ2_bzWrite is called by providing 1 byte of output buffer at each iteration
    {
        BZIP2_API::Write(&bzerror, bzf, &orig_buf[i], 1);
        EXPECT_EQ(bzerror, BZ_OK);
    }

    // Leftover bytes are written, and memory is freed by calling BZ2_bzWriteClose
    BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL);
    EXPECT_EQ(bzerror, BZ_OK);
    bzf = NULL;
    
    EXPECT_TRUE(verify_bzip2_file(orig_buf, orig_len, file_name));
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzWrite,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzWrite Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzWriteClose Tests
 ********************************************/

class BZIP2_BZ2_bzWriteClose : public BZIP2_BZ2_bzWrite
{
    public:
    // Similar to earlier declaration of verify_bzip2_file, but this function is called
    // when a test case is supposed to fail
    bool verify_bzip2_file_fail(vector<char> &orig_buf, int orig_len, string file_name)
    {
        // data initializations
        int bz_err = 0;
        int uncompressed_len = orig_len;
        vector<char> uncompressed_buf(uncompressed_len);
        FILE * pFile = fopen(file_name.c_str(), "rb");
        if(!pFile) return false;

        BZFILE * bzfile = BZIP2_API::ReadOpen(&bz_err, pFile, verbosity, 0, NULL, 0);
        if(bzfile == nullptr || bz_err != BZ_OK)
        {
            fclose(pFile);
            return false;
        }

        // Decompress into uncompressed_buf
        uncompressed_len = BZIP2_API::Read(&bz_err, bzfile, uncompressed_buf.data(), orig_len);

        // verification
        bool ret = bz_err == BZ_STREAM_END;
        ret &= orig_len == uncompressed_len;
        ret &= memcmp(orig_buf.data(), uncompressed_buf.data(), orig_len) == 0;

        // free up memory
        BZIP2_API::ReadClose(&bz_err, bzfile);
        fclose(pFile);
        return ret;
    }

    protected:
    unsigned int in = 0, out = 0;
};

// In cases 1 to 3, BZ2_bzWriteClose function is returned abruptly, because of which memory is not freed 
// so when TearDown() is called, it will free the memory properly.
TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_fail_common_1) // bzf is NULL
{
    BZIP2_API::WriteClose(&bzerror, NULL, 0, &in, &out);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(in, 0);
    EXPECT_EQ(out, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));
}

TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_fail_common_2) // bzf->writing is set to 0

{
    bzFile * bzf_ptr_temp = (bzFile *)bzf;
    int writing_temp = bzf_ptr_temp->writing;
    bzf_ptr_temp->writing = 0;

    BZIP2_API::WriteClose(&bzerror, bzf, 0, &in, &out);

    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    EXPECT_EQ(((bzFile*)bzf)->lastErr, BZ_SEQUENCE_ERROR);
    EXPECT_EQ(in, 0);
    EXPECT_EQ(out, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));

    bzf_ptr_temp->writing = writing_temp;
}

TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_fail_common_3) // Deliberately introducing error in file ptr which gets caught by ferror()
{
    bzFile * bzf_ptr_temp = (bzFile *)bzf;
    FILE * file_ptr_temp = bzf_ptr_temp->handle;

    string temp_file_name = "temp_" + file_name;
    FILE * err_file_temp = fopen(temp_file_name.c_str(), "w"); 
    char c = fgetc(err_file_temp);  // trying to read from a write only mode file, which will set the error flag
    bzf_ptr_temp->handle = err_file_temp;

    BZIP2_API::WriteClose(&bzerror, bzf, 0, &in, &out);
    EXPECT_EQ(bzerror, BZ_IO_ERROR);
    EXPECT_EQ(((bzFile *)bzf)->lastErr, BZ_IO_ERROR);
    EXPECT_EQ(in, 0);
    EXPECT_EQ(out, 0);

    bzf_ptr_temp->handle = file_ptr_temp;
    fclose(err_file_temp);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));
    remove(temp_file_name.c_str());
}

TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_fail_common_4) // abandon is set to 1
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    BZIP2_API::WriteClose(&bzerror, bzf, 1, &in, &out);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(in, orig_len);
    EXPECT_EQ(out, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));

    // BZIP2_API::WriteClose has freed bzf memory, but still bzf != NULL
    // so setting it to NULL will avoid calling WriteClose again in TearDown()
    bzf = NULL;
}

TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_fail_common_5) // Error occured when BZ2_bzWrite is called
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), -1);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);

    BZIP2_API::WriteClose(&bzerror, bzf, 0, &in, &out);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(in, 0);
    EXPECT_EQ(out, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, 0, file_name));

    bzf = NULL;
}

TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_fail_common_6)
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    bzFile * temp_bzf = (bzFile *)bzf;
    EState * e = (EState *)temp_bzf->strm.state;
    e->mode = BZ_M_IDLE;     // Internal call to BZ2_bzCompress will throw an error

    BZIP2_API::WriteClose(&bzerror, bzf, 0, &in, &out);

    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));
//  BZ2_bzWriteClose function is returned abruptly, because of which memory is not freed from bzf pointer
//  so when TearDown() is called, it will free the memory properly.
}

TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_pass_common_7)   // simple pass case
{
    orig_len = 1e5;
    orig_buf = vector<char>(orig_len);
    for(int i=0; i < orig_len; i++)
    {
        orig_buf[i] = rand() % 255;
    }
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    BZIP2_API::WriteClose(&bzerror, bzf, 0, &in, &out);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(in, orig_len);
    EXPECT_NE(out, 0);
    EXPECT_TRUE(verify_bzip2_file(orig_buf, orig_len, file_name));

    bzf = NULL;
}

TEST_P(BZIP2_BZ2_bzWriteClose, AOCL_Compression_bzip2_BZ2_bzWriteClose_pass_common_8)
{
    orig_len = 1e5;
    orig_buf = vector<char>(orig_len);
    for(int i=0; i < orig_len; i++)
    {
        orig_buf[i] = rand() % 255;
    }
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL);  // all input & output bytes pointers passed to function are set to NULL.

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_TRUE(verify_bzip2_file(orig_buf, orig_len, file_name));
    
    bzf = NULL;
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzWriteClose,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzWriteClose Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzWriteClose64 Tests
 ********************************************/

class BZIP2_BZ2_bzWriteClose64 : public BZIP2_BZ2_bzWriteClose
{
    public:

    unsigned int in_low = 0, in_high = 0, out_low = 0, out_high = 0;
};

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_fail_common_1) // bzf is NULL
{
    BZIP2_API::WriteClose64(&bzerror, NULL, 0, &in_low, &in_high, &out_low, &out_high);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(((bzFile *)bzf)->lastErr, BZ_OK);
    EXPECT_EQ(in_low, 0);
    EXPECT_EQ(in_high, 0);
    EXPECT_EQ(out_low, 0);
    EXPECT_EQ(out_high, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));
}

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_fail_common_2) // bzf->writing is set to 0

{
    bzFile * bzf_ptr_temp = (bzFile *)bzf;
    int t = bzf_ptr_temp->writing;
    bzf_ptr_temp->writing = 0;

    BZIP2_API::WriteClose64(&bzerror, bzf, 0, &in_low, &in_high, &out_low, &out_high);

    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    EXPECT_EQ(((bzFile*)bzf)->lastErr, BZ_SEQUENCE_ERROR);
    EXPECT_EQ(in_low, 0);
    EXPECT_EQ(in_high, 0);
    EXPECT_EQ(out_low, 0);
    EXPECT_EQ(out_high, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));

    bzf_ptr_temp->writing = t;
}

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_fail_common_3) // Deliberately introducing error in file ptr which gets caught by ferror()
{
    bzFile * bzf_ptr_temp = (bzFile *)bzf;
    FILE * file_ptr_temp = bzf_ptr_temp->handle;

    string temp_file_name = "temp_" + file_name;
    FILE * err_file_temp = fopen(temp_file_name.c_str(), "w"); 
    char c = fgetc(err_file_temp);  // trying to read from a write only mode file, which will set the error flag
    bzf_ptr_temp->handle = err_file_temp;

    BZIP2_API::WriteClose64(&bzerror, bzf, 0, &in_low, &in_high, &out_low, &out_high);

    EXPECT_EQ(bzerror, BZ_IO_ERROR);
    EXPECT_EQ(((bzFile *)bzf)->lastErr, BZ_IO_ERROR);
    EXPECT_EQ(in_low, 0);
    EXPECT_EQ(in_high, 0);
    EXPECT_EQ(out_low, 0);
    EXPECT_EQ(out_high, 0);

    bzf_ptr_temp->handle = file_ptr_temp;
    fclose(err_file_temp);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));
    remove(temp_file_name.c_str());
}

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_fail_common_4) // abandon is set to 1
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    BZIP2_API::WriteClose64(&bzerror, bzf, 1, &in_low, &in_high, &out_low, &out_high);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(in_low, orig_len);
    EXPECT_EQ(in_high, 0);
    EXPECT_EQ(out_low, 0);
    EXPECT_EQ(out_high, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));

    // BZIP2_API::WriteClose has freed bzf memory, but still bzf != NULL
    // so setting it to NULL will avoid calling WriteClose again in TearDown()
    bzf = NULL;
}

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_fail_common_5) // BZ2_bzWrite is called with incorrect parameters
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), -1);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);

    BZIP2_API::WriteClose64(&bzerror, bzf, 0, &in_low, &in_high, &out_low, &out_high);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(in_low, 0);
    EXPECT_EQ(in_high, 0);
    EXPECT_EQ(out_low, 0);
    EXPECT_EQ(out_high, 0);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, 0, file_name));

    bzf = NULL;
}

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_fail_common_6)
{
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    bzFile * temp_bzf = (bzFile *)bzf;
    EState * e = (EState *)temp_bzf->strm.state;
    e->mode = BZ_M_IDLE;     // Internal call to BZ2_bzCompress will throw an error

    BZIP2_API::WriteClose64(&bzerror, bzf, 0, &in_low, &in_high, &out_low, &out_high);

    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    EXPECT_FALSE(verify_bzip2_file_fail(orig_buf, orig_len, file_name));
}

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_pass_common_7) // simple pass case
{
    orig_len = 1e5;
    orig_buf = vector<char>(orig_len);
    for(int i=0; i < orig_len; i++)
    {
        orig_buf[i] = rand() % 255;
    }
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    BZIP2_API::WriteClose64(&bzerror, bzf, 0, &in_low, &in_high, &out_low, &out_high); // bzf is freed along with associated data when WriteClose64 is called

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(in_low, orig_len);
    EXPECT_EQ(in_high, 0);
    EXPECT_NE(out_low, 0);
    EXPECT_EQ(out_high, 0);
    EXPECT_TRUE(verify_bzip2_file(orig_buf, orig_len, file_name));

    bzf = NULL;
}

TEST_P(BZIP2_BZ2_bzWriteClose64, AOCL_Compression_bzip2_BZ2_bzWriteClose64_pass_common_8)
{
    orig_len = 1e5;
    orig_buf = vector<char>(orig_len);
    for(int i=0; i < orig_len; i++)
    {
        orig_buf[i] = rand() % 255;
    }
    BZIP2_API::Write(&bzerror, bzf, orig_buf.data(), orig_len);
    EXPECT_EQ(bzerror, BZ_OK);

    BZIP2_API::WriteClose64(&bzerror, bzf, 0, NULL, NULL, NULL, NULL);  // all input & output bytes pointers passed to function are set to NULL.

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_TRUE(verify_bzip2_file(orig_buf, orig_len, file_name));

    bzf = NULL;
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzWriteClose64,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzWriteClose64 Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzReadOpen Tests
 ********************************************/

class BZIP2_BZ2_bzReadOpen : public ::testing::Test
{
    public:
    int bzerror = 0;
    int verbosity = 0;
    FILE * pFile = NULL;
    bzFile * bzf = NULL;
    string file_name = "";

    vector<char> orig_buffer;
    int orig_len = 0;

    void create_test_file()
    {
        orig_len = 1000;
        orig_buffer = vector<char>(orig_len);
        for(int i=0; i<orig_len; i++)
        {
            orig_buffer[i] = rand() % 255;
        }
        pFile = fopen(file_name.c_str(), "wb");
        EXPECT_NE(pFile, nullptr);

        BZFILE * bzf = BZIP2_API::WriteOpen(&bzerror, pFile, 1, 0, 0);
        EXPECT_NE(bzf, nullptr);

        BZIP2_API::Write(&bzerror, bzf, orig_buffer.data(), orig_len);
        EXPECT_EQ(bzerror, BZ_OK);

        BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL);    // bzf & associated buffers are freed when WriteClose is called
        EXPECT_EQ(bzerror, BZ_OK);
        fclose(pFile);
    }

    void SetUp() override
    {
        file_name =  get_file_name();

        create_test_file();
        pFile = fopen(file_name.c_str(), "rb");
    }

    void check_pass()
    {
        EXPECT_EQ(bzf->initialisedOk, true);
        EXPECT_EQ(bzf->writing, false);
        EXPECT_NE(bzf->strm.bzalloc, nullptr);
        EXPECT_NE(bzf->strm.bzfree, nullptr);
        EXPECT_EQ(bzf->strm.opaque, nullptr);
    }

    void TearDown() override {
        if(pFile)
            fclose(pFile);
        BZIP2_API::ReadClose(&bzerror, bzf);
        remove(file_name.c_str());
    }
};

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_1) // pFile is NULL
{
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, NULL, verbosity, 0, NULL, 0), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_2) // verbosity is less than 0
{
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, pFile, -1, 0, NULL, 0), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_3) // verbosity is greater than 4
{
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, pFile, 5, 0, NULL, 0), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_4)   // small is -1
{
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, -1, NULL, 0), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_5)   // small is 2
{
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 2, NULL, 0), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_6) // unused==NULL & nUnused != 0
{
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, NULL, 1), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_7) // unused!=NULL & nUnused < 0
{
    void * unused = malloc(1);
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, unused, -1), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
    free(unused);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_8) // unused!=NULL & nUnused > BZ_MAX_UNUSED
{
    void * unused = malloc(1);
    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, unused, BZ_MAX_UNUSED+1), nullptr);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
    free(unused);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_fail_common_9) // Deliberately introducing error in file ptr which gets caught by ferror()
{
    FILE * err_file_temp = fopen(file_name.c_str(), "w"); 
    char c = fgetc(err_file_temp);  // trying to read from a write only mode file, which will set the error flag

    EXPECT_EQ(BZIP2_API::ReadOpen(&bzerror, err_file_temp, verbosity, 0, NULL, 0), nullptr);

    EXPECT_EQ(bzerror, BZ_IO_ERROR);
    fclose(err_file_temp);
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_pass_common_10) // unused buffer is passed
{
    int nUnused = 100;
    vector<char> unused(nUnused);
    for(int i=0; i<nUnused; i++)
    {
        unused[i] = rand() % 255;
    }

    bzf = (bzFile *)BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, unused.data(), nUnused);

    EXPECT_EQ(bzerror, BZ_OK);
    ASSERT_NE(bzf, nullptr);
    EXPECT_EQ(memcmp(bzf->buf, unused.data(), nUnused), 0);
    EXPECT_EQ(nUnused, bzf->bufN);
    check_pass();
}

TEST_F(BZIP2_BZ2_bzReadOpen, AOCL_Compression_bzip2_BZ2_bzReadOpen_pass_common_11) // unused buffer is not set
{
    bzf = (bzFile *)BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, NULL, 0);
    
    EXPECT_EQ(bzerror, BZ_OK);
    ASSERT_NE(bzf, nullptr);
    check_pass();
}

/*********************************************
 * "End" of BZ2_bzReadOpen Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzRead Tests
 ********************************************/

class BZIP2_BZ2_bzRead : public OPT_LEVEL_TESTS
{
    public:
    int bzerror = 0;
    int verbosity = 0;
    FILE * pFile = NULL;
    bzFile * bzf = NULL;
    string file_name = "";

    vector<char> orig_buffer;
    int orig_len = 0;

    vector<char> uncompressed_buf;
    int uncompressed_len;

    void SetUp() override
    {
        // Code path setup
        {
            aocl_destroy_bzip2();
            DynamicDispatch opt = GetParam();
            aocl_setup_bzip2(opt.optOff, opt.optLevel, 0, 0, 0);
        }

        file_name = get_file_name();

        create_test_file();
        uncompressed_len = orig_len;
        uncompressed_buf = vector<char>(uncompressed_len);
        pFile = fopen(file_name.c_str(), "rb");
        bzf = (bzFile *)BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, NULL, 0);
    }

    void create_test_file()
    {
        orig_len = 1e6;
        orig_buffer = vector<char>(orig_len);
        for(int i=0; i<orig_len; i++)
        {
            orig_buffer[i] = rand() % 255;
        }
        pFile = fopen(file_name.c_str(), "wb");
        EXPECT_NE(pFile, nullptr);

        BZFILE * bzf = BZIP2_API::WriteOpen(&bzerror, pFile, 1, 0, 0);
        EXPECT_NE(bzf, nullptr);

        BZIP2_API::Write(&bzerror, bzf, orig_buffer.data(), orig_len);
        EXPECT_EQ(bzerror, BZ_OK);

        BZIP2_API::WriteClose(&bzerror, bzf, 0, NULL, NULL);    // bzf & associated buffers are freed when WriteClose is called
        EXPECT_EQ(bzerror, BZ_OK);
        fclose(pFile);
    }

    void TearDown() override {
        if(bzf)
            BZIP2_API::ReadClose(&bzerror, bzf);
        if(pFile)
            fclose(pFile);
        bzf = NULL;
        pFile = NULL;
        remove(file_name.c_str());
    }
};

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_1) // bzf is NULL
{
    BZIP2_API::Read(&bzerror, NULL, uncompressed_buf.data(), uncompressed_len);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_2) // buff is NULL
{
    BZIP2_API::Read(&bzerror, bzf, NULL, uncompressed_len);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_3) // len is negative
{
    BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), -1);
    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_4) // bzf->writing is set to 1
{
    bzFile * bzf_temp_ptr = (bzFile *)bzf;
    int temp_writing = bzf_temp_ptr->writing;
    bzf_temp_ptr->writing = 1;
    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), 0);
    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    bzf_temp_ptr->writing = temp_writing;
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_5) // len is 0
{
    BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), 0);
    EXPECT_EQ(bzerror, BZ_OK);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_6) // Deliberately introducing error in file ptr which gets caught by ferror()
{
    bzFile * temp_bzf = (bzFile *)bzf;
    FILE * temp_f = temp_bzf->handle;
    string temp_file_name = "temp_" + file_name;
    FILE * err_file_temp = fopen(temp_file_name.c_str(), "w"); 
    char c = fgetc(err_file_temp);  // trying to read from a write only mode file, which will set the error flag
    temp_bzf->handle = err_file_temp;

    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), 0);

    EXPECT_EQ(bzerror, BZ_IO_ERROR);
    temp_bzf->handle = temp_f;
    fclose(err_file_temp);
    remove(temp_file_name.c_str());
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_7) // pFile is fseeked to EOF
{
    fseek(pFile, 0, SEEK_END);

    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), 0);

    EXPECT_EQ(bzerror, BZ_UNEXPECTED_EOF);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_8) // BZIP2 file magic number is corrupted
{
    {
        FILE *input_file, *output_file;
        vector<char> buffer;
        long file_size;
        size_t result;

        // Open the input file for reading
        input_file = fopen(file_name.c_str(), "rb");
        fseek(input_file, 0, SEEK_END);
        file_size = ftell(input_file);
        rewind(input_file);

        // Allocate memory for the buffer
        buffer = vector<char>(file_size);
        // Read the entire file into the buffer
        result = fread(buffer.data(), 1, file_size, input_file);
        buffer[1]++;
        // Close the input file
        fclose(input_file);

        // Open the output file for writing
        output_file = fopen(file_name.c_str(), "wb");

        // Write the contents of the buffer to the output file
        result = fwrite(buffer.data(), 1, file_size, output_file);
        fclose(output_file);
    }
    
    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), 0);
    EXPECT_EQ(bzerror, BZ_DATA_ERROR_MAGIC);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_fail_common_9) // BZIP2 file data is corrupted
{
    {
        FILE *input_file, *output_file;
        vector<char> buffer;
        long file_size;
        size_t result;

        // Open the input file for reading
        input_file = fopen(file_name.c_str(), "rb");
        fseek(input_file, 0, SEEK_END);
        file_size = ftell(input_file);
        rewind(input_file);

        // Allocate memory for the buffer
        buffer = vector<char>(file_size);
        // Read the entire file into the buffer
        result = fread(buffer.data(), 1, file_size, input_file);
        buffer[100]++;
        // Close the input file
        fclose(input_file);

        // Open the output file for writing
        output_file = fopen(file_name.c_str(), "wb");

        // Write the contents of the buffer to the output file
        result = fwrite(buffer.data(), 1, file_size, output_file);
        fclose(output_file);
    }

    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), 0);
    EXPECT_EQ(bzerror, BZ_DATA_ERROR);
}



TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_pass_common_10) // Some initial data from the file is read and loaded using ReadOpen
{
    uncompressed_len = orig_len;
    uncompressed_buf = vector<char>(uncompressed_len);
    BZIP2_API::ReadClose(&bzerror, bzf);
    int temp_len = 100;
    vector<char> temp(temp_len);
    for(int i=0;i<temp.size();i++)
        temp[i] = fgetc(pFile);
    bzf = (bzFile *)BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, temp.data(), temp_len);

    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), orig_len);

    EXPECT_EQ(bzerror, BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed_buf.data(), orig_buffer.data(), orig_len), 0);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_pass_common_11) // BZ2_bzRead is provided 1 byte at a time for output buffer
{
    uncompressed_len = orig_len;
    uncompressed_buf = vector<char>(uncompressed_len);
    for(int i=0; i < uncompressed_len; i++)
    {
        EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, &uncompressed_buf[i], 1), 1);
        if(i < orig_len-1)
            EXPECT_EQ(bzerror, BZ_OK);
        else
            EXPECT_EQ(bzerror, BZ_STREAM_END);
    }
    EXPECT_EQ(memcmp(uncompressed_buf.data(), orig_buffer.data(), uncompressed_len), 0);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_pass_common_12) // len is exactly equal to original length
{
    uncompressed_len = orig_len;
    uncompressed_buf = vector<char>(uncompressed_len);

    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), orig_len);

    EXPECT_EQ(bzerror, BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed_buf.data(), orig_buffer.data(), orig_len), 0);
}

TEST_P(BZIP2_BZ2_bzRead, AOCL_Compression_bzip2_BZ2_bzRead_pass_common_13) // len is greater than original length
{
    uncompressed_len = orig_len+1+(rand()%orig_len);
    uncompressed_buf = vector<char>(uncompressed_len);

    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), orig_len);

    EXPECT_EQ(bzerror, BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed_buf.data(), orig_buffer.data(), orig_len), 0);
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzRead,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
 * "End" of BZ2_bzRead Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzReadClose Tests
 ********************************************/
class BZIP2_BZ2_bzReadClose : public BZIP2_BZ2_bzReadOpen
{
    public:
    void SetUp() override
    {
        file_name = get_file_name();

        create_test_file();
        pFile = fopen(file_name.c_str(), "rb");
        bzf = (bzFile *)BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 0, NULL, 0);
    }
    void TearDown() override {
        if(bzf)
            BZIP2_API::ReadClose(&bzerror, bzf);
        if(pFile)
            fclose(pFile);
        bzf = NULL;
        pFile = NULL;
        remove(file_name.c_str());
    }
};

TEST_F(BZIP2_BZ2_bzReadClose, AOCL_Compression_bzip2_BZ2_bzReadClose_fail_common_1) // bzf is NULL
{
    BZIP2_API::ReadClose(&bzerror, NULL);
    EXPECT_EQ(bzerror, BZ_OK);
}

TEST_F(BZIP2_BZ2_bzReadClose, AOCL_Compression_bzip2_BZ2_bzReadClose_fail_common_2) // bzf->writing is 1
{
    bzFile * temp_bzf = (bzFile *)bzf;
    int t = temp_bzf->writing;
    temp_bzf->writing = 1;

    BZIP2_API::ReadClose(&bzerror, bzf);

    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    temp_bzf->writing = t;
}

TEST_F(BZIP2_BZ2_bzReadClose, AOCL_Compression_bzip2_BZ2_bzReadClose_fail_common_3) // BZ2_bzWriteOpen is called instead of BZ2_ReadOpen
{
    BZFILE * b = BZIP2_API::WriteOpen(&bzerror, pFile, 1, 0, 0);

    BZIP2_API::ReadClose(&bzerror, b);

    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    EXPECT_NE(((bzFile *)b)->strm.state, nullptr);
    BZIP2_API::WriteClose(&bzerror, b, 0, 0, 0);
}

TEST_F(BZIP2_BZ2_bzReadClose, AOCL_Compression_bzip2_BZ2_bzReadClose_fail_common_4) // bzf->initilisedOk = false, so BZ2_bzDecompressEnd is not called
{
    bzf->initialisedOk = false;
    DState * st = (DState *)(bzf->strm.state);

    BZIP2_API::ReadClose(&bzerror, bzf);

    EXPECT_EQ(bzerror, BZ_OK);
    free(st->tt);
    free(st->ll16);
    free(st);
    bzf = NULL;
}

TEST_F(BZIP2_BZ2_bzReadClose, AOCL_Compression_bzip2_BZ2_bzReadClose_pass_common_5) // pass case
{
    BZIP2_API::ReadClose(&bzerror, bzf);

    EXPECT_EQ(bzerror, BZ_OK);
    bzf = NULL;
}

/*********************************************
 * "End" of BZ2_bzReadClose Tests
 ********************************************/

/*********************************************
 * "Begin" of BZ2_bzReadGetUnused Tests
 ********************************************/

class BZIP2_BZ2_bzReadGetUnused : public BZIP2_BZ2_bzRead
{};

TEST_P(BZIP2_BZ2_bzReadGetUnused, AOCL_Compression_bzip2_BZ2_bzReadGetUnused_fail_common_1) // bzf is NULL
{
    void * unused = NULL;
    int nUnused = 0;

    BZIP2_API::ReadGetUnused(&bzerror, NULL, &unused, &nUnused);

    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
    EXPECT_EQ(unused, nullptr);
    EXPECT_EQ(nUnused, 0);
}

TEST_P(BZIP2_BZ2_bzReadGetUnused, AOCL_Compression_bzip2_BZ2_bzReadGetUnused_fail_common_2) // BZ2_bzRead is not called before calling ReadGetUnused
{
    void * unused = NULL;
    int nUnused = 0;

    BZIP2_API::ReadGetUnused(&bzerror, bzf, &unused, &nUnused);

    EXPECT_EQ(bzerror, BZ_SEQUENCE_ERROR);
    EXPECT_EQ(unused, nullptr);
    EXPECT_EQ(nUnused, 0);
}

TEST_P(BZIP2_BZ2_bzReadGetUnused, AOCL_Compression_bzip2_BZ2_bzReadGetUnused_fail_common_3) // unused is NULL
{
    void * unused = NULL;
    int nUnused = 0;
    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), orig_len);
    EXPECT_EQ(bzerror, BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed_buf.data(), orig_buffer.data(), orig_len), 0);

    BZIP2_API::ReadGetUnused(&bzerror, bzf, NULL, &nUnused);

    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
    EXPECT_EQ(unused, nullptr);
    EXPECT_EQ(nUnused, 0);
}

TEST_P(BZIP2_BZ2_bzReadGetUnused, AOCL_Compression_bzip2_BZ2_bzReadGetUnused_fail_common_4) // nUnused is NULL
{
    void * unused = NULL;
    int nUnused = 0;
    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), orig_len);
    EXPECT_EQ(bzerror, BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed_buf.data(), orig_buffer.data(), orig_len), 0);

    BZIP2_API::ReadGetUnused(&bzerror, bzf, &unused, NULL);

    EXPECT_EQ(bzerror, BZ_PARAM_ERROR);
    EXPECT_EQ(unused, nullptr);
    EXPECT_EQ(nUnused, 0);
}

TEST_P(BZIP2_BZ2_bzReadGetUnused, AOCL_Compression_bzip2_BZ2_bzReadGetUnused_pass_common_5) // Extra data added at the end of bzip2 compressed data
{
    int extra_len = 100;
    vector<char> extra_buffer(extra_len);   // This is the extra buffer of length 100
    for(int i=0;i<extra_len;i++)
        extra_buffer[i]=rand()%255;
    {
        FILE *input_file, *output_file;
        vector<char> buffer;
        long file_size;
        size_t result;

        // Open the input file for reading
        input_file = fopen(file_name.c_str(), "rb");
        fseek(input_file, 0, SEEK_END);
        file_size = ftell(input_file);
        rewind(input_file);

        // Allocate memory for the buffer for reading the file and also incorporating extra data
        buffer = vector<char>(file_size+extra_len);
        for(int i=0;i<extra_len;i++)
            buffer[i+file_size]=extra_buffer[i];
            
        // Read the entire file into the buffer
        result = fread(buffer.data(), 1, file_size, input_file);
        fclose(input_file);

        // Open the output file for writing
        output_file = fopen(file_name.c_str(), "wb");

        // Write the contents of the buffer to the output file, which `compressed data` + `extra data`
        result = fwrite(buffer.data(), 1, buffer.size(), output_file);
        fclose(output_file);
    }
    // Data is decompressed until it encounters BZ_STREAM_END, and the leftover data from compressed file is not processed
    EXPECT_EQ(BZIP2_API::Read(&bzerror, bzf, uncompressed_buf.data(), uncompressed_len), orig_len);
    EXPECT_EQ(bzerror, BZ_STREAM_END);
    EXPECT_EQ(memcmp(uncompressed_buf.data(), orig_buffer.data(), orig_len), 0);
    void * unused = NULL;
    int nUnused = 0;

    // Here unused will point to the leftover data & uUnused stores its length
    BZIP2_API::ReadGetUnused(&bzerror, bzf, &unused, &nUnused);

    EXPECT_EQ(bzerror, BZ_OK);
    EXPECT_EQ(nUnused, extra_len);
    EXPECT_EQ(memcmp(unused, extra_buffer.data(), extra_len), 0); // Check if the leftover data is same as the extra data that was added into compressed file
}

INSTANTIATE_TEST_SUITE_P(
    BZIP2,
    BZIP2_BZ2_bzReadGetUnused,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));


/*********************************************
 * "End" of BZ2_bzReadGetUnused Tests
 ********************************************/

/*********************************************
 * Begin fuzz tests for bzip2
 *********************************************/
#ifdef AOCL_TEST_FUZZER
#include "fuzztest/fuzztest.h"
void BuffToBuffCompress_fuzz(std::vector<char> source, size_t dest_sz,
                             int level, int optOff, int optLevel, int workFactor)
{
    aocl_setup_bzip2(optOff, optLevel, 0, 0, 0);

    unsigned destLen = dest_sz > UINT_MAX ? UINT_MAX : dest_sz;
    vector<char> dest(destLen, 0);

    // Verbosity is set to 0 to avoid extensive logs
    BZIP2_API::BuffToBuffCompress(dest.data(), &destLen, source.data(), source.size(), level, 0, workFactor);

    aocl_destroy_bzip2();
}
FUZZ_TEST(AOCL_Compression_bzip2, BuffToBuffCompress_fuzz)
.WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(1, 9),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4),
             fuzztest::InRange<int>(0, 250))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> vector<tuple<vector<char>, size_t, int, int, int, int>> {
    auto seed_files = READ_FUZZ_CPR_SEED();
    vector<tuple<vector<char>, size_t, int, int, int, int>> seeds;
    auto seeds_base = get_fuzz_cpr_seeds<char>([](size_t src_sz) -> size_t {
        size_t dst_sz = (size_t)BZ2_bzCompressBound((unsigned)src_sz);
        return limit_fuzz_size_max(dst_sz);
        }, 1, 9, seed_files);
    int workFactor = 0; // include additional parameter in seed : workFactor
    for (auto& seed : seeds_base) {
        seeds.push_back({ get<0>(seed), get<1>(seed), get<2>(seed), get<3>(seed), get<4>(seed), workFactor });
        workFactor += 10; workFactor %= 250;
    }
    return seeds;
})
#endif
;

void BuffToBuffDecompress_fuzz(vector<char> source, size_t dest_sz,
                               int optOff, int optLevel, int small)
{
    aocl_setup_bzip2(optOff, optLevel, 0, 0, 0);

    unsigned int destLen = dest_sz;
    vector<char> dest(destLen, 0);

    // Verbosity is set to 0 to avoid extensive logs
    BZIP2_API::BuffToBuffDecompress(dest.data(), &destLen, source.data(), source.size(), small, 0);

    aocl_destroy_bzip2();
}
FUZZ_TEST(AOCL_Compression_bzip2, BuffToBuffDecompress_fuzz)
.WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4),
             fuzztest::InRange<int>(0, 1))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> vector<tuple<vector<char>, size_t, int, int, int>> {
    auto seed_files = READ_FUZZ_DPR_SEED();
    vector<tuple<vector<char>, size_t, int, int, int>> seeds;
    auto seeds_base = get_fuzz_dpr_seeds<char>(seed_files);
    int small = 0; // include additional parameter in seed : small
    for (auto& seed : seeds_base) {
        seeds.push_back({ get<0>(seed), get<1>(seed), get<2>(seed), get<3>(seed), small });
        small = !small;
    }
    return seeds;
})
#endif
;

void BZ2_bzCompress_fuzz(std::vector<char> input, int out_len, int action, int block_size, int verbosity, int work_factor)
{
    EStateClass *strm = new EStateClass();
    std::vector<char> output(out_len);
    BZIP2_API::CompressInit(strm->getStrm(), block_size, verbosity, work_factor);
    strm->setEState(strm->getStatePtr());        
    strm->setNextIn(input.data());
    strm->setNextOut(output.data());
    strm->setAvailIn(input.size());
    strm->setAvailOut(out_len);
    if(action==0)
        strm->setMode(BZ_RUN);
    if(action==1)
        strm->setMode(BZ_FLUSH);
    if(action==2)
        strm->setMode(BZ_FINISH); 
     
    BZIP2_API::Compress(strm->getStrm(), action);
    BZIP2_API::CompressEnd(strm->getStrm());
    delete strm;
}
 
FUZZ_TEST(AOCL_Compression_bzip2, BZ2_bzCompress_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1, 10000),
                fuzztest::InRange<int>(0, 2),
                fuzztest::InRange<int>(1, 9),
                fuzztest::InRange<int>(0, 4),
                fuzztest::InRange<int>(0, 250)
                );

void BZ2_bzDecompress_fuzz(std::vector<char> input, int out_len, int verbosity, int small)
{
    DStateClass *strm = new DStateClass();
    std::vector<char> output(out_len);
    strm->setNextIn(input.data());
    strm->setNextOut(output.data());
    strm->setAvailIn(input.size());
    strm->setAvailOut(out_len);

    BZIP2_API::DecompressInit(strm->getStrm(), verbosity, small);
    BZIP2_API::Decompress(strm->getStrm());
    BZIP2_API::DecompressEnd(strm->getStrm());
    delete strm;
}

FUZZ_TEST(AOCL_Compression_bzip2, BZ2_bzDecompress_fuzz)
.WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::InRange<int>(1, 10000),
                fuzztest::InRange<int>(0, 4),
                fuzztest::InRange<int>(0, 1)
                );

void BZ2_bzWrite_fuzz(std::vector<char> inputbuffer, int block_size, int verbosity, int work_factor, int abandon,int writetype)
{
    int bzerror = 0;
    unsigned int in_low = 0, in_high = 0, out_low = 0, out_high = 0, in = 0, out = 0;
    string file_name = get_file_name();
       
    FILE *pFile = fopen(file_name.c_str(), "wb");
    EXPECT_NE(pFile, nullptr);

    BZFILE * bzf = BZIP2_API::WriteOpen(&bzerror, pFile, block_size, verbosity, work_factor);
    EXPECT_EQ(bzerror, BZ_OK);

    BZIP2_API::Write(&bzerror, bzf, inputbuffer.data(), inputbuffer.size());
   
    if(writetype == 0)
        BZIP2_API::WriteClose(&bzerror, bzf, abandon, &in, &out);

    if(writetype == 1)
        BZIP2_API::WriteClose64(&bzerror, bzf, abandon, &in_low, &in_high, &out_low, &out_high);
    EXPECT_EQ(bzerror, BZ_OK);

    fclose(pFile);
    EXPECT_EQ(remove(file_name.c_str()), 0);
}

FUZZ_TEST(AOCL_Compression_bzip2, BZ2_bzWrite_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 9),
                 fuzztest::InRange<int>(0, 4),
                 fuzztest::InRange<int>(0, 250),
                 fuzztest::InRange<int>(0, 1),
                 fuzztest::InRange<int>(0, 1));

void BZ2_bzRead_fuzz(std::vector<char> inputbuffer,int block_size, int verbosity, int work_factor, int small,
                          int nUnused, int out_len)
{
    int bzerror = 0;
    string file_name = get_file_name();
    
    FILE *pFile = fopen(file_name.c_str(), "wb");
    EXPECT_NE(pFile, nullptr);

    int n = fwrite(inputbuffer.data(),1, inputbuffer.size(), pFile);
    EXPECT_EQ(n, inputbuffer.size());
  
    fclose(pFile);

    pFile = fopen(file_name.c_str(), "rb");
    EXPECT_NE(pFile, nullptr);

    std:vector<char> unusedbuffer(nUnused);
    BZFILE *bzf = BZIP2_API::ReadOpen(&bzerror, pFile, verbosity, 1, unusedbuffer.data(), nUnused);
    EXPECT_EQ(bzerror, BZ_OK);
    ASSERT_NE(bzf, nullptr);

    vector<char> outputbuffer(out_len);        
    BZIP2_API::Read(&bzerror, bzf, outputbuffer.data(), out_len);

    BZIP2_API::ReadClose(&bzerror, bzf);
    EXPECT_EQ(bzerror, BZ_OK);

    fclose(pFile);
    EXPECT_EQ(remove(file_name.c_str()), 0);        
}

FUZZ_TEST(AOCL_Compression_bzip2, BZ2_bzRead_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                 fuzztest::InRange<int>(1, 9),
                 fuzztest::InRange<int>(0, 4),
                 fuzztest::InRange<int>(0, 250),
                 fuzztest::InRange<int>(0, 1),
                 fuzztest::InRange<int>(0, BZ_MAX_UNUSED),
                 fuzztest::InRange<int>(1, 10000)
                 );
#endif /* AOCL_TEST_FUZZER */
/*********************************************
 * End fuzz tests for bzip2
 *********************************************/
