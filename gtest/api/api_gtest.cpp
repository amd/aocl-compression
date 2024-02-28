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
 
 /** @file api_gtest.cpp
 *  
 *  @brief Test cases for AOCL Compression API.
 *
 *  This file contains the test cases for apis exposed 
 *  by api/aocl_compression.h and api/aocl_threads.h.
 *
 *  @author Ashish Sriram
 */

#include <string>
#include <limits>
#include <vector>
#include "gtest/gtest.h"

#include "utils/utils.h"
#include "api/aocl_compression.h"
#include "api/types.h"

#ifdef AOCL_ENABLE_THREADS
#include "api/aocl_threads.h"
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

using namespace std;

typedef aocl_compression_desc ACD;
typedef aocl_compression_type ACT;

typedef struct {
    AOCL_INTP lower;
    AOCL_INTP upper;
    AOCL_INTP def;
} algo_level_t;

typedef struct {
    int optOff;
    int optLevel;
    ACT algo;
} api_test_params;

typedef api_test_params ATP;

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

#define GET_ARR_CNT(arr) (sizeof(arr) / sizeof(arr[0]))

/*
* Testing if unsupported code paths are taken based on isa requested:
* 
* Add all aocl optimized functions that use instructions that are
* extensions to x86 isa here. Function name and level must be added
* Following level values are supported :
*   1: SSE2, 2:AVX, 3:AVX2, 4:AVX512 and others
* Also, LOG_UNFORMATTED(DEBUG, ...) log must be added within the 
* function body of such optimized functions.
* 
* Based on dynamic dispatcher and AOCL_ENABLE_INSTRUCTIONS environment
* variable, code paths with unsupported instruction sets must not be
* taken. For example, when AOCL_ENABLE_INSTRUCTIONS=AVX, adler32_x86_avx2()
* should not be called. API tests below validate this.
*/ 
const aocl_func_info aocl_simd_funcs[] {
    /* {"func_name", level } */
    {"FastMemcopy64Bytes", 2},
    {"DecompressAllTags_bmi", 3},
    {"ExtractLowBytes_bmi", 3},
    {"DecompressAllTags_avx", 2},
    {"InternalUncompress_avx", 2},
    {"InternalUncompressDirect_avx", 2},
    {"AOCL_SnappyArrayWriter_AVX", 2},
    {"AOCL_SAW_RawUncompress_AVX", 2},
    {"DecompressAllTags_avx", 2},
    {"AOCL_memcpy64", 2},
    {"adler32_x86_avx2", 3},
    {"compare256_avx2", 3},
    {"longest_match_avx2_opt", 3},
    {"adler32_x86_avx", 2},
    {"compare256_avx", 2},
    {"longest_match_avx_opt", 2}
};

/*
* This function provides list of valid algo ids and test environment configurations
*/
vector<ATP> get_api_test_params() {
    vector<ATP> atps;
    for (int optOff = -1; optOff <= 1; optOff++) { // -1:not set, 0: opt, 1:ref
        for (int optLevel = -1; optLevel <= 4; ++optLevel) { // -1:not set, 0:C, 1:SSE, 2:AVX, 3:AVX2, 4:AVX512 and others
#ifndef AOCL_EXCLUDE_LZ4
            atps.push_back({ optOff, optLevel, LZ4 });
#endif
#if !defined(AOCL_EXCLUDE_LZ4HC) && !defined(AOCL_EXCLUDE_LZ4)
            atps.push_back({ optOff, optLevel, LZ4HC });
#endif
#ifndef AOCL_EXCLUDE_LZMA
            atps.push_back({ optOff, optLevel, LZMA });
#endif
#ifndef AOCL_EXCLUDE_BZIP2
            atps.push_back({ optOff, optLevel, BZIP2 });
#endif
#ifndef AOCL_EXCLUDE_SNAPPY
            atps.push_back({ optOff, optLevel, SNAPPY });
#endif
#ifndef AOCL_EXCLUDE_ZLIB
            atps.push_back({ optOff, optLevel, ZLIB });
#endif
#ifndef AOCL_EXCLUDE_ZSTD
            atps.push_back({ optOff, optLevel, ZSTD });
#endif
        }
    }

    if (atps.size() == 0) { //no algo enabled
        atps.push_back({ 0, -1, AOCL_COMPRESSOR_ALGOS_NUM }); //add dummy entry. Else parameterized tests will fail.
    }
    return atps;
}

int get_max_opt_level(int optLevel) {
    // use max optLevel when its undecided. Assumes default setting is max supported optLevel.
    return (optLevel == -1) ? 4 : optLevel;
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
void reset_ACD(ACD* desc, AOCL_INTP level) {
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
// #ifdef AOCL_ENABLE_LOG_FEATURE
//     logCtx.maxLevel = -1;
// #endif /* AOCL_ENABLE_LOG_FEATURE */
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
class API_setup : public ::testing::TestWithParam<ATP> {
public:
    void SetUp() override {
        atp = GetParam();
        set_opt_off(atp.optOff);
        set_opt_level(atp.optLevel);
        clear_test_log_counter();
    }

    void TearDown() override {
    }

    void setup_and_validate() {
        EXPECT_NO_THROW(aocl_llc_setup(&desc, atp.algo));
        switch (atp.algo) {
        case ZSTD:
            EXPECT_NE(desc.workBuf, nullptr);  //ztd saves params and context 
            break;
        default:
            EXPECT_EQ(desc.workBuf, nullptr);
            break;
        }

        EXPECT_EQ(validate_simd_func_access(aocl_simd_funcs, GET_ARR_CNT(aocl_simd_funcs),
            get_max_opt_level(atp.optLevel)), 1);
    }

    void destroy() {
        EXPECT_NO_THROW(aocl_llc_destroy(&desc, atp.algo));
    }

    ATP atp;
    ACD desc;
};

TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_empty_common_1) //ACD empty
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    setup_and_validate();
    destroy();
}

TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_optoff_common_2) //ACD empty optOff
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    desc.optOff = 1; //switch off optimizations
    setup_and_validate();
    destroy();
}

TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_ioSet_common_3) //ACD inBuf, outBuf set
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    TestLoad t(800, true);
    set_ACD_io_bufs(&desc, (TestLoadBase*)(&t));
    setup_and_validate();
    destroy();
}

// #ifdef AOCL_ENABLE_LOG_FEATURE
// TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_logs_common) //logs
// {
//     skip_test_if_algo_invalid(atp.algo)
//     //Log levels: ERR:1,...TRACE:4
//     for (int logLevel = 1; logLevel <= 4; ++logLevel) { //AOCL_Compression_api_aocl_llc_setup_logs_common_4-7
//         reset_ACD(&desc, algo_levels[atp.algo].def);
//         logCtx.maxLevel = logLevel;
//         setup_and_validate();
//         destroy();
//     }
// }
// #endif /* AOCL_ENABLE_LOG_FEATURE */

/*
* Crashes
TEST_P(API_setup, AOCL_Compression_api_aocl_llc_setup_invalid_common_1) //Invalid algo id
{
    ACT algo = ATP::AOCL_COMPRESSOR_ALGOS_NUM;
    ACD desc;
    reset_ACD(&desc, algo_levels[algo].def);
    setup_and_validate(&desc, algo);
}*/

INSTANTIATE_TEST_SUITE_P(
    API_TEST,
    API_setup,
    ::testing::ValuesIn(get_api_test_params()));

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

class API_compress : public ::testing::TestWithParam<ATP> {
public:
    void SetUp() override {
        atp = GetParam();
        set_opt_off(atp.optOff);
        set_opt_level(atp.optLevel);
        clear_test_log_counter();
    }

    void destroy() {
        if (atp.algo < AOCL_COMPRESSOR_ALGOS_NUM)
            aocl_llc_destroy(&desc, atp.algo);
        desc.workBuf = nullptr;
    }

    void TearDown() override {
        destroy();
    }

    void setup() {
        ASSERT_NO_THROW(aocl_llc_setup(&desc, atp.algo));
        switch (atp.algo) {
        case ZSTD:
            ASSERT_NE(desc.workBuf, nullptr); //ztd saves params and context 
            break;
        default:
            ASSERT_EQ(desc.workBuf, nullptr);
            break;
        }
    }

    int64_t compress() {
        return aocl_llc_compress(&desc, atp.algo);
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
        int64_t dSize = aocl_llc_decompress(&desc, atp.algo);
        EXPECT_EQ(dSize, src->getInpSize()); //is decompressed data size == src size?
        EXPECT_EQ(memcmp(src->getInpData(), desc.outBuf, src->getInpSize()), 0);

        EXPECT_EQ(validate_simd_func_access(aocl_simd_funcs, GET_ARR_CNT(aocl_simd_funcs),
            get_max_opt_level(atp.optLevel)), 1);
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

    ATP atp;
    ACD desc;
};

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_defaultOptOn_common_1) //default optOn
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    TestLoad cpr(800, 1600, true);
    run_test((TestLoadBase*)(&cpr));
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_defaultOptOff_common_2) //default optOff
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    desc.optOff = 1; //switch off optimizations
    TestLoad cpr(800, 1600, true);
    run_test((TestLoadBase*)(&cpr));
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_measureStats_common_3) //measure stats
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    desc.measureStats = 1; //record stats
    TestLoad cpr(800, 1600, true);
    run_test((TestLoadBase*)(&cpr), true);
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_memLimit_common_4) //with different memLimits
{
    skip_test_if_algo_invalid(atp.algo)
    {
        reset_ACD(&desc, algo_levels[atp.algo].def);
        desc.memLimit = 1 << 9; //512KB. memLimit < input size
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr)); //memLimit param is ignored by all algos. Works like default optOn.
    }
    {
        reset_ACD(&desc, algo_levels[atp.algo].def);
        desc.memLimit = (size_t)1 << 30; //1GB
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr));
    }
    {
        reset_ACD(&desc, algo_levels[atp.algo].def);
        desc.memLimit = (size_t)1 << 31; //>1GB
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr));
    }
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_inpNull_common_5) //input buffer null
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    TestLoad cpr(800, 1600, true);
    desc.inBuf   = nullptr;
    desc.inSize  = cpr.getInpSize();
    desc.outBuf  = cpr.getOutData();
    desc.outSize = cpr.getOutSize();
    setup();

    int64_t cSize = compress();

    // printf("cSize by algo %d %ld", (int)atp.algo, cSize);

    EXPECT_LT(cSize, 0); //compress failed 

}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_inpSzZero_common_6) //input size 0
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    TestLoad cpr(800, 1600, true);
    desc.inBuf   = cpr.getInpData();
    desc.inSize  = 0;
    desc.outBuf  = cpr.getOutData();
    desc.outSize = cpr.getOutSize();
    setup();

    int64_t cSize = compress();
    switch (atp.algo) { //most algos do not treat this as an error case
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
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
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
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
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
    skip_test_if_algo_invalid(atp.algo)
    //AOCL_Compression_api_aocl_llc_compress_levelsOptOn_common_1-313
    for (int cpuOptLvl = 0; cpuOptLvl <= MAX_OPT_LEVEL; cpuOptLvl++) { //with optOn, test all dynamic dispatcher supported levels
        for (AOCL_INTP level = algo_levels[atp.algo].lower; level <= algo_levels[atp.algo].upper; level++) {
            if (level == algo_levels[atp.algo].def && cpuOptLvl == DEFAULT_OPT_LEVEL) continue; //run non-default config only
            reset_ACD(&desc, level);
            desc.optLevel = cpuOptLvl;
            TestLoad cpr(800, 1600, true);
            run_test((TestLoadBase*)(&cpr));
        }
    }
}

TEST_P(API_compress, AOCL_Compression_api_aocl_llc_compress_levelsOptOff_common) //all levels optOff
{
    skip_test_if_algo_invalid(atp.algo)
    //AOCL_Compression_api_aocl_llc_compress_levelsOptOff_common_1-57
    for (AOCL_INTP level = algo_levels[atp.algo].lower; level <= algo_levels[atp.algo].upper; level++) {
        if (level == algo_levels[atp.algo].def) continue;  //run non-default config only
        reset_ACD(&desc, level);
        desc.optOff = 1; //switch off optimizations
        TestLoad cpr(800, 1600, true);
        run_test((TestLoadBase*)(&cpr));
    }
}

INSTANTIATE_TEST_SUITE_P(
    API_TEST,
    API_compress,
    ::testing::ValuesIn(get_api_test_params()));
/*********************************************
 * End Compress Tests
 ********************************************/

 /*********************************************
  * Begin Decompress Tests
  *********************************************/
class API_decompress : public ::testing::TestWithParam<ATP> {
public:
    void SetUp() override {
        atp = GetParam();
        set_opt_off(atp.optOff);
        set_opt_level(atp.optLevel);
        clear_test_log_counter();

        cpr = new TestLoad(800, 1600, true);
    }

    void TearDown() override {
        if (atp.algo < AOCL_COMPRESSOR_ALGOS_NUM)
            aocl_llc_destroy(&desc, atp.algo);

        if(cpr) delete cpr;
    }

    //setup and compress
    void setup_and_compress() {
        set_ACD_io_bufs(&desc, (TestLoadBase*)(cpr)); //set desc. inp=uncompressed data, out=empty output buffer.
        
        //setup
        ASSERT_NO_THROW(aocl_llc_setup(&desc, atp.algo));
        switch (atp.algo) {
        case ZSTD:
            ASSERT_NE(desc.workBuf, nullptr); //ztd saves params and context 
            break;
        default:
            ASSERT_EQ(desc.workBuf, nullptr);
            break;
        }

        //compress
        cSize = aocl_llc_compress(&desc, atp.algo);
        ASSERT_GT(cSize, 0);
    }

    //decompress data in desc and match it with src
    void decompress_and_validate() {
        //decompress
        TestLoadSingle dpr(cSize, cpr->getOutData(), cpr->getInpSize());
        set_ACD_io_bufs(&desc, (TestLoadBase*)(&dpr));  //set desc. inp=compressed data, out=empty output buffer.
        int64_t dSize = aocl_llc_decompress(&desc, atp.algo);

        //validate
        EXPECT_EQ(dSize, cpr->getInpSize()); //is decompressed data size == src size?
        EXPECT_EQ(memcmp(cpr->getInpData(), desc.outBuf, cpr->getInpSize()), 0);

        EXPECT_EQ(validate_simd_func_access(aocl_simd_funcs, GET_ARR_CNT(aocl_simd_funcs),
            get_max_opt_level(atp.optLevel)), 1);
    }

    int64_t get_cSize() {
        return cSize;
    }

    TestLoadBase* get_cprPtr() {
        return (TestLoadBase*)(cpr);
    }

    ATP atp;
    ACD desc;

private:
    int64_t cSize = 0;
    TestLoad* cpr = nullptr;
};

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_defaultOptOn_common_1) //default optOn
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    setup_and_compress();
    decompress_and_validate();
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_defaultOptOff_common_1) //default optOff
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    desc.optOff = 1;
    setup_and_compress();
    decompress_and_validate();
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_inpNull_common_1) //inp NULL
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = nullptr;
    desc.inSize  = dpr->getInpSize();
    desc.outBuf  = dpr->getOutData();
    desc.outSize = dpr->getOutSize();
    int64_t dSize = aocl_llc_decompress(&desc, atp.algo);

    EXPECT_LT(dSize, 0); //decompress failed
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_inpSzZero_common_1) //inp size = 0
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = dpr->getInpData();
    desc.inSize  = 0;
    desc.outBuf  = dpr->getOutData();
    desc.outSize = dpr->getOutSize();
    int64_t dSize = aocl_llc_decompress(&desc, atp.algo);

    EXPECT_LE(dSize, 0); //decompress failed / nothing to decompress
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_outNull_common_1) //out NULL
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = dpr->getInpData();
    desc.inSize  = dpr->getInpSize();
    desc.outBuf  = nullptr;
    desc.outSize = dpr->getOutSize();
    int64_t dSize = aocl_llc_decompress(&desc, atp.algo);

    EXPECT_LT(dSize, 0); //decompress failed
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_outSzZero_common_1) //out size = 0
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    setup_and_compress();
    
    //decompress
    TestLoadBase* cpr = get_cprPtr();
    TestLoadSingle dprs(get_cSize(), cpr->getOutData(), cpr->getInpSize());
    TestLoadBase* dpr = (TestLoadBase*)(&dprs);
    desc.inBuf   = dpr->getInpData();
    desc.inSize  = dpr->getInpSize();
    desc.outBuf  = dpr->getOutData();
    desc.outSize = 0;
    int64_t dSize = aocl_llc_decompress(&desc, atp.algo);
    EXPECT_LT(dSize, 0); //decompress failed
}

TEST_P(API_decompress, AOCL_Compression_api_aocl_llc_decompress_invalidCprData_common_1) //invalid compressed data
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
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
    int64_t dSize = aocl_llc_decompress(&desc, atp.algo);

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
    ::testing::ValuesIn(get_api_test_params()));
/*********************************************
 * End Decompress Tests
 ********************************************/

#ifdef AOCL_ENABLE_THREADS

AOCL_INT32 Test_aocl_get_rap_frame_bound_mt() {
    return aocl_get_rap_frame_bound_mt();
}

AOCL_INT32 Test_aocl_skip_rap_frame_mt(char* src, int32_t src_size) {
    return aocl_skip_rap_frame_mt(src, src_size);
}

/*********************************************
* Begin rap frame bound Tests
*********************************************/
TEST(API_get_rap_frame_bound_MT, AOCL_Compression_api_aocl_get_rap_frame_bound_mt_common_1) {
    AOCL_UINT32 max_threads = Test_aocl_get_rap_frame_bound_mt();
    EXPECT_EQ(max_threads, RAP_FRAME_LEN_WITH_DECOMP_LENGTH(omp_get_max_threads(), 0));
}
/*********************************************
* End rap frame bound Tests
*********************************************/

/*********************************************
* Begin skip rap frame Tests
*********************************************/
class API_skip_rap_frame_MT : public ::testing::Test {
public:
    void SetUp() override {
        src = NULL;
        src_size = 0;
    }

    void TearDown() override {
        if (src) free(src);
    }

    AOCL_INT32 add_rap_frame(int mainThreads) {
        src = (AOCL_CHAR*)malloc(src_size * sizeof(AOCL_CHAR));
        AOCL_CHAR* src_ptr = src;
        AOCL_INT32 rap_frame_len = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0);
        *(AOCL_INT64*)src_ptr = RAP_MAGIC_WORD; //For storing the magic word
        src_ptr += RAP_MAGIC_WORD_BYTES;
        *(AOCL_UINT32*)src_ptr = rap_frame_len; //For storing the total RAP frame length
        src_ptr += RAP_METADATA_LEN_BYTES;
        *(AOCL_UINT32*)src_ptr = mainThreads; //For storing the no. of threads
        return rap_frame_len;
    }

    AOCL_CHAR* src;
    size_t src_size;
};

TEST_F(API_skip_rap_frame_MT, AOCL_Compression_api_aocl_skip_rap_frame_mt_common_1) { //RAP frame only
    //Create RAP frame
    int mainThreads = 2;
    src_size = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0);
    AOCL_INT32 val_size = add_rap_frame(mainThreads);

    EXPECT_EQ(Test_aocl_skip_rap_frame_mt(src, src_size), val_size);
}

TEST_F(API_skip_rap_frame_MT, AOCL_Compression_api_aocl_skip_rap_frame_mt_common_2) { //RAP frame + stream
    //Create RAP frame
    int mainThreads = 2;
    src_size = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, 0) + 1024; //1024 for stream
    AOCL_INT32 val_size = add_rap_frame(mainThreads);

    EXPECT_EQ(Test_aocl_skip_rap_frame_mt(src, src_size), val_size);
}

TEST_F(API_skip_rap_frame_MT, AOCL_Compression_api_aocl_skip_rap_frame_mt_common_3) { //No RAP frame
    //No RAP frame
    src_size = 1024; //1024 for stream
    src = (AOCL_CHAR*)malloc(src_size * sizeof(AOCL_CHAR));
    memset(src, 0, src_size);

    EXPECT_EQ(Test_aocl_skip_rap_frame_mt(src, src_size), 0);
}

TEST_F(API_skip_rap_frame_MT, AOCL_Compression_api_aocl_skip_rap_frame_mt_common_4) { //src size too small
    //Src smaller than magic word
    src_size = RAP_MAGIC_WORD_BYTES - 1;
    src = (AOCL_CHAR*)malloc(src_size * sizeof(AOCL_CHAR));
    memset(src, 0, src_size);

    EXPECT_EQ(Test_aocl_skip_rap_frame_mt(src, src_size), 0);
}

TEST_F(API_skip_rap_frame_MT, AOCL_Compression_api_aocl_skip_rap_frame_mt_common_5) { //src null
    EXPECT_EQ(Test_aocl_skip_rap_frame_mt(src, src_size), ERR_INVALID_INPUT);
}
/*********************************************
* End skip rap frame Tests
*********************************************/

#endif /* AOCL_ENABLE_THREADS */
