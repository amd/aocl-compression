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
 
 /** @file api_gtest.cc
 *  
 *  @brief Test cases for Unified APIs.
 *
 *  This file contains the test cases for unified apis exposed 
 *  by api/aocl_compression.h.
 *
 *  @author Ashish Sriram
 */

#include <string>
#include <limits>
#include <vector>
#include "gtest/gtest.h"

#include "api/aocl_compression.h"
#include "api/types.h"

using namespace std;

typedef aocl_compression_desc ACD;
typedef aocl_compression_type ACT;

typedef struct {
    INTP lower;
    INTP upper;
    INTP def;
} algo_level_t;

//List of supported compression methods along with their parameters
//The list is ordered as per the enum aocl_compression_type
static const algo_level_t algo_levels[AOCL_COMPRESSOR_ALGOS_NUM] =
{
    {0, 0,  0}, //"LZ4",
    {1, 12, 9}, //"LZ4HC",
    {0, 9,  1}, //"LZMA",
    {1, 9,  6}, //"BZIP2",
    {0, 0,  0}, //"SNAPPY",
    {1, 9,  6}, //"ZLIB",
    {1, 22, 9}  //"ZSTD",
};

#define MAX_MEM_SIZE_FOR_FILE_READ 1073741824 //(1024 MBs)
#define DEFAULT_OPT_LEVEL 2
#define MAX_OPT_LEVEL 2 // Set to test AVX code paths. If your system supports AVX2, AVX-512 this can be increased to 3, 4.

/*
* This function provides list of valid algo ids
*/
vector<ACT> get_algos() {
    vector<ACT> algos;
#ifndef AOCL_EXCLUDE_LZ4
    algos.push_back(LZ4);
#endif
#if !defined(AOCL_EXCLUDE_LZ4HC) && !defined(AOCL_EXCLUDE_LZ4)
    algos.push_back(LZ4HC);
#endif
#ifndef AOCL_EXCLUDE_LZMA
    algos.push_back(LZMA);
#endif
#ifndef AOCL_EXCLUDE_BZIP2
    algos.push_back(BZIP2);
#endif
#ifndef AOCL_EXCLUDE_SNAPPY
    algos.push_back(SNAPPY);
#endif
#ifndef AOCL_EXCLUDE_ZLIB
    algos.push_back(ZLIB);
#endif
#ifndef AOCL_EXCLUDE_ZSTD
    algos.push_back(ZSTD);
#endif

    if (algos.size() == 0) { //no algo enabled
        algos.push_back(AOCL_COMPRESSOR_ALGOS_NUM); //add dummy entry. Else parameterized tests will fail.
    }
    return algos;
}

#define skip_test_if_algo_invalid(algo) { \
    if (algo == AOCL_COMPRESSOR_ALGOS_NUM) { \
        EXPECT_EQ(algo, AOCL_COMPRESSOR_ALGOS_NUM); \
        return; \
    } \
}

class TestLoadBase {
public:
    virtual ~TestLoadBase() {}
    // Returns pointer to source buffer.
    virtual char* getInpData() = 0;
    // Returns size of source buffer.
    virtual size_t getInpSize() = 0;
    // Returns pointer to destination buffer (data obtained after compression).
    virtual char* getOutData() = 0;
    // Returns size of destination data.
    virtual size_t getOutSize() = 0;
};

/*
    This class is used for allocating and deallocating memory,
    which acts as a common way for instantiating input and output buffers
    for most of the test cases.    
*/
class TestLoad : TestLoadBase
{
private:
    //source buffer (original data which we intend to compress/decompress).
    char *inp_data = nullptr;
    size_t inp_sz = 0;
    //destination buffer (data obtained after compression/decompression).
    char *out_data = nullptr;
    size_t out_sz=0;

public:
    // Allocate inp and out buffers
    TestLoad(size_t inpSz, size_t outSz, bool fillInp) {
        inp_sz = inpSz;
        inp_data = (char*)malloc(inp_sz * sizeof(char));

        if (fillInp) {
            // generating random data inside `inp_data` buffer.
            for (size_t i = 0; i < inp_sz; i++)
            {
                inp_data[i] = rand() % 255;
            }
        }

        out_sz = outSz;
        out_data = (char*)malloc(out_sz * sizeof(char));
    }

    // Allocate inp and out buffers. Use when out size is unknown. Set same as inp.
    TestLoad(size_t sz, bool fillInp) : TestLoad(sz, sz, fillInp) {}

    // Returns pointer to source buffer.
    char *getInpData()
    {
        return inp_data;
    }
    // Returns size of source buffer.
    size_t getInpSize()
    {
        return inp_sz;
    }
    // Returns pointer to destination buffer (data obtained after compression).
    char *getOutData()
    {
        return out_data;
    }
    // Returns size of destination data.
    size_t getOutSize()
    {
        return out_sz;
    }
    // Destructor function.
    ~TestLoad()
    {
        if(inp_data) free(inp_data);
        if(out_data) free(out_data);
    }
};

/*
    This class is used for allocating and deallocating memory,
    which acts as a common way for instantiating input and output buffers
    for most of the test cases.
    Does not manage memory for inp_data
*/
class TestLoadSingle : TestLoadBase
{
private:
    //source buffer (original data which we intend to compress/decompress).
    char* inp_data = nullptr;
    size_t inp_sz = 0;
    //destination buffer (data obtained after compression/decompression).
    char* out_data = nullptr;
    size_t out_sz = 0;

public:
    // Allocate out buffer and set inp buffer
    // Memory managment for inp buffer is not done within the class
    TestLoadSingle(size_t inpSz, char* inpBuf, size_t outSz) {
        inp_sz = inpSz;
        inp_data = inpBuf;

        out_sz = outSz;
        out_data = (char*)malloc(out_sz * sizeof(char));
    }

    // Returns pointer to source buffer.
    char* getInpData()
    {
        return inp_data;
    }
    // Returns size of source buffer.
    size_t getInpSize()
    {
        return inp_sz;
    }
    // Returns pointer to destination buffer (data obtained after compression).
    char* getOutData()
    {
        return out_data;
    }
    // Returns size of destination data.
    size_t getOutSize()
    {
        return out_sz;
    }
    // Destructor function.
    ~TestLoadSingle()
    {
        if(out_data) free(out_data);
    }
};

/*
* Init ACD
*/
void reset_ACD(ACD* desc, INTP level) {
    desc->inBuf = nullptr;
    desc->outBuf = nullptr;
    desc->workBuf = nullptr;
    desc->inSize = 0;
    desc->outSize = 0;
    desc->level = level;
    desc->optVar = 0;
    desc->numThreads = 1;
    desc->numMPIranks = 1;
    desc->memLimit = MAX_MEM_SIZE_FOR_FILE_READ;
    desc->measureStats = 0;
    desc->cSize = 0;
    desc->dSize = 0;
    desc->cTime = 0;
    desc->dTime = 0;
    desc->cSpeed = 0.0;
    desc->dSpeed = 0.0;
    desc->optOff = 0;
    desc->optLevel = DEFAULT_OPT_LEVEL;
    desc->printDebugLogs = 0;
}

void set_ACD_io_bufs(ACD* desc, TestLoadBase* t) {
    desc->inBuf   = t->getInpData();
    desc->inSize  = t->getInpSize();
    desc->outBuf  = t->getOutData();
    desc->outSize = t->getOutSize();
}

/*********************************************
 * Begin Version Tests
 *********************************************/
TEST(API_Version, AOCL_Compression_aocl_llc_version_common_1) {
    EXPECT_STREQ(aocl_llc_version(), (AOCL_COMPRESSION_LIBRARY_VERSION " " AOCL_BUILD_VERSION));
}
/*********************************************
 * End Version Tests
 *********************************************/

/*********************************************
 * Begin Setup/Destroy Tests
 *********************************************/
class API_setup : public ::testing::TestWithParam<ACT> {
public:
    void SetUp() override {
        algo = GetParam();
    }

    void setup_and_validate() {
        EXPECT_NO_THROW(aocl_llc_setup(&desc, algo));
        switch (algo) {
        case ZSTD:
            EXPECT_NE(desc.workBuf, nullptr);  //ztd saves params and context 
            break;
        default:
            EXPECT_EQ(desc.workBuf, nullptr);
            break;
        }
    }

    void destroy() {
        EXPECT_NO_THROW(aocl_llc_destroy(&desc, algo));
    }

    ACT algo;
    ACD desc;
};

TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_empty_common_1) //ACD empty
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_validate();
    destroy();
}

TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_optoff_common_2) //ACD empty optOff
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    desc.optOff = 1; //switch off optimizations
    setup_and_validate();
    destroy();
}

TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_ioSet_common_3) //ACD inBuf, outBuf set
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    TestLoad t(800, true);
    set_ACD_io_bufs(&desc, (TestLoadBase*)(&t));
    setup_and_validate();
    destroy();
}

TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_logs_common) //logs
{
    skip_test_if_algo_invalid(algo)
    //Log levels: ERR:1,...TRACE:4
    for (int logLevel = 1; logLevel <= 4; ++logLevel) { //AOCL_Compression_api_aocl_llc_setup_logs_common_4-7
        reset_ACD(&desc, algo_levels[algo].def);
        desc.printDebugLogs = logLevel;
        setup_and_validate();
        destroy();
    }
}

/*
* Crashes
TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_invalid_common_1) //Invalid algo id
{
    ACT algo = ACT::AOCL_COMPRESSOR_ALGOS_NUM;
    ACD desc;
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_validate(&desc, algo);
}*/

INSTANTIATE_TEST_SUITE_P(
    API_TEST,
    API_setup,
    ::testing::ValuesIn(get_algos()));

TEST(API_setup, AOCL_Compression_api_aocl_llc_setup_invalidAlgo_common_1) //invalid algo
{
    ACT algo;
    ACD desc;
    reset_ACD(&desc, 0);
    algo = AOCL_COMPRESSOR_ALGOS_NUM;
    EXPECT_EQ(aocl_llc_setup(&desc, algo), ERR_UNSUPPORTED_METHOD);
}

static void test_excluded_algo(ACD* desc, ACT algo){
    reset_ACD(desc, 1);
    EXPECT_EQ(aocl_llc_setup(desc, algo), ERR_EXCLUDED_METHOD);
}

TEST(API_setup, AOCL_Compression_api_aocl_llc_setup_excludedMethod_common_1) //excluded method
{
    ACD desc;
#ifdef AOCL_EXCLUDE_LZ4
    test_excluded_algo(&desc, LZ4);
#endif
#ifdef AOCL_EXCLUDE_LZ4HC
    test_excluded_algo(&desc, LZ4HC);
#endif
#ifdef AOCL_EXCLUDE_LZMA
    test_excluded_algo(&desc, LZMA);
#endif
#ifdef AOCL_EXCLUDE_BZIP2
    test_excluded_algo(&desc, BZIP2);
#endif
#ifdef AOCL_EXCLUDE_SNAPPY
    test_excluded_algo(&desc, SNAPPY);
#endif
#ifdef AOCL_EXCLUDE_ZLIB
    test_excluded_algo(&desc, ZLIB);
#endif
#ifdef AOCL_EXCLUDE_ZSTD
    test_excluded_algo(&desc, ZSTD);
#endif
}
/*********************************************
 * End Setup/Destroy Tests
 ********************************************/



 /*********************************************
  * Begin Compress Tests
  *********************************************/
float get_speed(size_t sz, uint64_t time) {
    return (time > 0) ? (((float)sz * 1000.0) / (float)time) : 0.0;
}

class API_compress : public ::testing::TestWithParam<ACT> {
public:
    void SetUp() override {
        algo = GetParam();
    }

    void destroy() {
        if (algo < AOCL_COMPRESSOR_ALGOS_NUM && desc.workBuf != nullptr)
            aocl_llc_destroy(&desc, algo);
        desc.workBuf = nullptr;
    }

    void TearDown() override {
        destroy();
    }

    void setup() {
        ASSERT_NO_THROW(aocl_llc_setup(&desc, algo));
        switch (algo) {
        case ZSTD:
            ASSERT_NE(desc.workBuf, nullptr); //ztd saves params and context 
            break;
        default:
            ASSERT_EQ(desc.workBuf, nullptr);
            break;
        }
    }

    int64_t compress() {
        return aocl_llc_compress(&desc, algo);
    }

    //validate if compression and measured stats are ok
    void compress_passed_ms(int64_t cSize) {
        EXPECT_GT(cSize, 0);
        EXPECT_EQ(cSize, desc.cSize);
        EXPECT_GT(desc.cTime, 0);
        EXPECT_NEAR(desc.cSpeed, get_speed(desc.inSize, desc.cTime), 0.001);
    }

    //decompress data in desc and match it with src
    void decompress_and_validate(TestLoadBase* src) {
        int64_t dSize = aocl_llc_decompress(&desc, algo);
        EXPECT_EQ(dSize, src->getInpSize()); //is decompressed data size == src size?
        EXPECT_EQ(memcmp(src->getInpData(), desc.outBuf, dSize), 0);
    }

    void run_test(TestLoadBase* cpr, bool measureStats = false) {
        set_ACD_io_bufs(&desc, cpr); //set desc. inp=uncompressed data, out=empty output buffer.
        setup();

        int64_t cSize = compress();
        if (measureStats)
            compress_passed_ms(cSize);
        else
            EXPECT_GT(cSize, 0);

        TestLoadSingle dpr(cSize, cpr->getOutData(), cpr->getInpSize());
        set_ACD_io_bufs(&desc, (TestLoadBase*)(&dpr));  //set desc. inp=compressed data, out=empty output buffer.
        decompress_and_validate(cpr);
        destroy();
    }

    ACT algo;
    ACD desc;
};

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_defaultOptOn_common_1) //default optOn
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    TestLoad cpr(800, 1600, true);
    run_test((TestLoadBase*)(&cpr));
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_defaultOptOff_common_2) //default optOff
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    desc.optOff = 1; //switch off optimizations
    TestLoad cpr(800, 1600, true);
    run_test((TestLoadBase*)(&cpr));
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_measureStats_common_3) //measure stats
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    desc.measureStats = 1; //record stats
    TestLoad cpr(800, 1600, true);
    run_test((TestLoadBase*)(&cpr), true);
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_memLimit_common_4) //with different memLimits
{
    skip_test_if_algo_invalid(algo)
    {
        reset_ACD(&desc, algo_levels[algo].def);
        desc.memLimit = 1 << 9; //512KB. memLimit < input size
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr)); //memLimit param is ignored by all algos. Works like default optOn.
    }
    {
        reset_ACD(&desc, algo_levels[algo].def);
        desc.memLimit = (size_t)1 << 30; //1GB
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr));
    }
    {
        reset_ACD(&desc, algo_levels[algo].def);
        desc.memLimit = (size_t)1 << 31; //>1GB
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr));
    }
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_inpNull_common_5) //input buffer null
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    TestLoad cpr(800, 1600, true);
    desc.inBuf   = nullptr;
    desc.inSize  = cpr.getInpSize();
    desc.outBuf  = cpr.getOutData();
    desc.outSize = cpr.getOutSize();
    setup();

    int64_t cSize = compress();

    printf("cSize by algo %d %ld", algo, cSize);

    EXPECT_LT(cSize, 0); //compress failed
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_inpSzZero_common_6) //input size 0
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    TestLoad cpr(800, 1600, true);
    desc.inBuf   = cpr.getInpData();
    desc.inSize  = 0;
    desc.outBuf  = cpr.getOutData();
    desc.outSize = cpr.getOutSize();
    setup();

    int64_t cSize = compress();
    switch (algo) { //most algos do not treat this as an error case
    case LZ4:
        EXPECT_EQ(cSize, 1);
        break;
    case LZ4HC:
        EXPECT_EQ(cSize, 1);
        break;
    case BZIP2:
        EXPECT_GT(cSize, 0);
        break;
    case SNAPPY:
        EXPECT_EQ(cSize, 1);
        break;
    case ZLIB:
        EXPECT_GT(cSize, 0);
        break;
    case ZSTD:
        EXPECT_GT(cSize, 0);
        break;
    default:
        EXPECT_LT(cSize, 0); //compress failed
        break;
    }
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_outNull_common_7) //output buffer null
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    TestLoad cpr(800, 1600, true);
    desc.inBuf   = cpr.getInpData();
    desc.inSize  = cpr.getInpSize();
    desc.outBuf  = nullptr;
    desc.outSize = cpr.getOutSize();
    setup();

    int64_t cSize = compress();

    EXPECT_LT(cSize, 0); //compress failed
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_outSzZero_common_8) //output size 0
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    TestLoad cpr(800, 1600, true);
    desc.inBuf   = cpr.getInpData();
    desc.inSize  = cpr.getInpSize();
    desc.outBuf  = cpr.getOutData();
    desc.outSize = 0;
    setup();

    int64_t cSize = compress();

    EXPECT_LT(cSize, 0); //compress failed
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_levelsOptOn_common) //all levels optOn
{
    skip_test_if_algo_invalid(algo)
    //AOCL_Compression_api_aocl_llc_compress_levelsOptOn_common_1-313
    for (int cpuOptLvl = 0; cpuOptLvl <= MAX_OPT_LEVEL; cpuOptLvl++) { //with optOn, test all dynamic dispatcher supported levels
        for (INTP level = algo_levels[algo].lower; level <= algo_levels[algo].upper; level++) {
            if (level == algo_levels[algo].def && cpuOptLvl == DEFAULT_OPT_LEVEL) continue; //run non-default config only
            reset_ACD(&desc, level);
            desc.optLevel = cpuOptLvl;
            TestLoad cpr(800, 1600, true);
            run_test((TestLoadBase*)(&cpr));
        }
    }
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_levelsOptOff_common) //all levels optOff
{
    skip_test_if_algo_invalid(algo)
    //AOCL_Compression_api_aocl_llc_compress_levelsOptOff_common_1-57
    for (INTP level = algo_levels[algo].lower; level <= algo_levels[algo].upper; level++) {
        if (level == algo_levels[algo].def) continue;  //run non-default config only
        reset_ACD(&desc, level);
        desc.optOff = 1; //switch off optimizations
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr));
    }
}

INSTANTIATE_TEST_SUITE_P(
    API_TEST,
    API_compress,
    ::testing::ValuesIn(get_algos()));
/*********************************************
 * End Compress Tests
 ********************************************/

 /*********************************************
  * Begin Decompress Tests
  *********************************************/
class API_decompress : public ::testing::TestWithParam<ACT> {
public:
    void SetUp() override {
        algo = GetParam();
        cpr = new TestLoad(800, 1600, true);
    }

    void TearDown() override {
        if (algo < AOCL_COMPRESSOR_ALGOS_NUM)
            aocl_llc_destroy(&desc, algo);

        if(cpr) delete cpr;
    }

    //setup and compress
    void setup_and_compress() {
        set_ACD_io_bufs(&desc, (TestLoadBase*)(cpr)); //set desc. inp=uncompressed data, out=empty output buffer.
        
        //setup
        ASSERT_NO_THROW(aocl_llc_setup(&desc, algo));
        switch (algo) {
        case ZSTD:
            ASSERT_NE(desc.workBuf, nullptr); //ztd saves params and context 
            break;
        default:
            ASSERT_EQ(desc.workBuf, nullptr);
            break;
        }

        //compress
        cSize = aocl_llc_compress(&desc, algo);
        ASSERT_GT(cSize, 0);
    }

    //decompress data in desc and match it with src
    void decompress_and_validate() {
        //decompress
        TestLoadSingle dpr(cSize, cpr->getOutData(), cpr->getInpSize());
        set_ACD_io_bufs(&desc, (TestLoadBase*)(&dpr));  //set desc. inp=compressed data, out=empty output buffer.
        int64_t dSize = aocl_llc_decompress(&desc, algo);

        //validate
        EXPECT_EQ(dSize, cpr->getInpSize()); //is decompressed data size == src size?
        EXPECT_EQ(memcmp(cpr->getInpData(), desc.outBuf, dSize), 0);
    }

    int64_t get_cSize() {
        return cSize;
    }

    TestLoadBase* get_cprPtr() {
        return (TestLoadBase*)(cpr);
    }

    ACT algo;
    ACD desc;

private:
    int64_t cSize = 0;
    TestLoad* cpr = nullptr;
};

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_defaultOptOn_common_1) //default optOn
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_compress();
    decompress_and_validate();
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_defaultOptOff_common_1) //default optOff
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    desc.optOff = 1;
    setup_and_compress();
    decompress_and_validate();
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_inpNull_common_1) //inp NULL
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = nullptr;
    desc.inSize  = dpr->getInpSize();
    desc.outBuf  = dpr->getOutData();
    desc.outSize = dpr->getOutSize();
    int64_t dSize = aocl_llc_decompress(&desc, algo);

    EXPECT_LT(dSize, 0); //decompress failed
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_inpSzZero_common_1) //inp size = 0
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = dpr->getInpData();
    desc.inSize  = 0;
    desc.outBuf  = dpr->getOutData();
    desc.outSize = dpr->getOutSize();
    int64_t dSize = aocl_llc_decompress(&desc, algo);

    EXPECT_LT(dSize, 0); //decompress failed
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_outNull_common_1) //out NULL
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = dpr->getInpData();
    desc.inSize  = dpr->getInpSize();
    desc.outBuf  = nullptr;
    desc.outSize = dpr->getOutSize();
    int64_t dSize = aocl_llc_decompress(&desc, algo);

    EXPECT_LT(dSize, 0); //decompress failed
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_outSzZero_common_1) //out size = 0
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = dpr->getInpData();
    desc.inSize  = dpr->getInpSize();
    desc.outBuf  = dpr->getOutData();
    desc.outSize = 0;
    int64_t dSize = aocl_llc_decompress(&desc, algo);
    EXPECT_LT(dSize, 0); //decompress failed
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_invalidCprData_common_1) //invalid compressed data
{
    skip_test_if_algo_invalid(algo)
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    set_ACD_io_bufs(&desc, dpr);

    //modify few bytes in the middle of compressed stream.
    size_t mid = get_cSize() / 2;
    for (size_t i = mid; i < mid + 16; ++i) {
        desc.inBuf[i] = (char)(rand() % 255);
    }
    int64_t dSize = aocl_llc_decompress(&desc, algo);

    if (dSize > 0) {
        EXPECT_NE(memcmp(cpr->getInpData(), desc.outBuf, dSize), 0); //mismatch with src
    }
    else {
        EXPECT_LT(dSize, 0); //decompress failed
    }
}

INSTANTIATE_TEST_SUITE_P(
    API_TEST,
    API_decompress,
    ::testing::ValuesIn(get_algos()));
/*********************************************
 * End Decompress Tests
 ********************************************/
