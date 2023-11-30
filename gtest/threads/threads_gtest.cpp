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
 
 /** @file threads_gtest.cpp
 *  
 *  @brief Test cases for functions supporting SMP threads.
 *
 *  This file contains the test cases for functions in threads/threads.h.
 *
 *  @author Ashish Sriram
 */

#include <string>
#include <limits>
#include <vector>
#include "gtest/gtest.h"

#include "utils/utils.h"
#include "threads/threads.h"
#include "api/aocl_compression.h"
#include "api/types.h"

using namespace std;

typedef aocl_compression_desc ACD;
typedef aocl_compression_type ACT;

typedef struct {
    AOCL_INTP lower;
    AOCL_INTP upper;
    AOCL_INTP def;
} algo_level_t;

typedef struct {
    int src_sz_factor;
    ACT algo;
} api_test_params_mt;

typedef api_test_params_mt ATP_mt;

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

/*
* This function provides list of valid algo ids and test environment configurations
*/
vector<ATP_mt> get_api_test_params_mt() {
    vector<ATP_mt> atps;
    for (int szFactor = 1; szFactor < 16; szFactor+=4) { // 1KB - 16 MB
#ifndef AOCL_EXCLUDE_LZ4
        atps.push_back({ szFactor, LZ4 });
#endif
#ifndef AOCL_EXCLUDE_SNAPPY
        atps.push_back({ szFactor, SNAPPY });
#endif
#ifndef AOCL_EXCLUDE_ZLIB
        atps.push_back({ szFactor, ZLIB });
#endif
#ifndef AOCL_EXCLUDE_ZSTD
        atps.push_back({ szFactor, ZSTD });
#endif
    }

    if (atps.size() == 0) { //no algo enabled
        atps.push_back({ 0, AOCL_COMPRESSOR_ALGOS_NUM }); //add dummy entry. Else parameterized tests will fail.
    }
    return atps;
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

#ifdef AOCL_ENABLE_THREADS

 /*********************************************
  * Begin Multithreaded Compress Tests
  ********************************************/

  /*
  * Testing all algos that have multithreaded compress/decompress support
  * in multithreaded mode by using input files of varying sizes [1KB - 16 MB].
  * As thread counts are decided based on input file size, this validates the algos
  * for compress/decompress operation with different thread counts.
  */

class API_compress_MT : public ::testing::TestWithParam<ATP_mt> {
public:
    void SetUp() override {
        atp = GetParam();
        const AOCL_UINTP min_sz = 512;
        AOCL_UINTP src_sz = min_sz << atp.src_sz_factor;
        cpr = new TestLoad(src_sz, src_sz * 2, true);
    }

    void destroy() {
        if (atp.algo < AOCL_COMPRESSOR_ALGOS_NUM)
            aocl_llc_destroy(&desc, atp.algo);
        desc.workBuf = nullptr;
    }

    void TearDown() override {
        destroy();
        delete cpr;
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

    //decompress data in desc and match it with src
    void decompress_and_validate(TestLoadBase* src) {
        int64_t dSize = aocl_llc_decompress(&desc, atp.algo);
        EXPECT_EQ(dSize, src->getInpSize()); //is decompressed data size == src size?
        EXPECT_EQ(memcmp(src->getInpData(), desc.outBuf, dSize), 0);
    }

    void run_test() {
        set_ACD_io_bufs(&desc, (TestLoadBase*)cpr); //set desc. inp=uncompressed data, out=empty output buffer.
        setup();

        int64_t cSize = compress();
        EXPECT_GT(cSize, 0);

        TestLoadSingle dpr(cSize, cpr->getOutData(), cpr->getInpSize());
        set_ACD_io_bufs(&desc, (TestLoadBase*)(&dpr));  //set desc. inp=compressed data, out=empty output buffer.
        decompress_and_validate((TestLoadBase*)cpr);
        destroy();
    }

    int64_t run_compress(int num_threads_compr)
    {
        omp_set_num_threads(num_threads_compr);
        uint64_t csize = compress();
        return csize;
    }

    uint64_t run_decompress(int num_threads_decompr)
    {
        omp_set_num_threads(num_threads_decompr);
        int64_t dSize = aocl_llc_decompress(&desc, atp.algo);
        return dSize;
    }

    void run_test_different_threads(int num_threads_compr, int num_threads_decompr) {
        set_ACD_io_bufs(&desc, (TestLoadBase*)cpr); //set desc. inp=uncompressed data, out=empty output buffer.
        setup();

        int64_t cSize = run_compress(num_threads_compr);

        EXPECT_GT(cSize, 0);

        TestLoadSingle dpr(cSize, cpr->getOutData(), cpr->getInpSize());
        set_ACD_io_bufs(&desc, (TestLoadBase*)(&dpr));  //set desc. inp=compressed data, out=empty output buffer.
        uint64_t dSize = run_decompress(num_threads_decompr);

        // validating
        EXPECT_EQ(dSize, cpr->getInpSize()); //is decompressed data size == src size?
        EXPECT_EQ(memcmp(cpr->getInpData(), desc.outBuf, dSize), 0);

        destroy();
    }

    ATP_mt atp;
    ACD desc;

private:
    TestLoad* cpr = nullptr;
};

TEST_P(API_compress_MT, AOCL_Compression_api_aocl_llc_compress_defaultOptOn_common_1) //default optOn
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    run_test();
}

TEST_P(API_compress_MT, AOCL_Compression_api_aocl_llc_compress_defaultOptOff_common_2) //default optOff
{
    skip_test_if_algo_invalid(atp.algo)
    reset_ACD(&desc, algo_levels[atp.algo].def);
    desc.optOff = 1; //switch off optimizations
    run_test();
}

TEST_P(API_compress_MT, AOCL_Compression_api_aocl_llc_compress_thread_count_greater_than_decompr_thread_count_common) // compr_thread_count > decompr_thread_count
{
    skip_test_if_algo_invalid(atp.algo);
    reset_ACD(&desc, algo_levels[atp.algo].def);
    int max_threads = omp_get_max_threads();
    int compr_num_threads = max_threads - 1;
    int decompr_num_threads = max_threads - 2;
    run_test_different_threads(compr_num_threads, decompr_num_threads);
}

TEST_P(API_compress_MT, AOCL_Compression_api_aocl_llc_compress_thread_count_less_than_decompr_thread_count_common) // compr_thread_count < decompr_thread_count
{
    skip_test_if_algo_invalid(atp.algo);
    reset_ACD(&desc, algo_levels[atp.algo].def);
    int max_threads = omp_get_max_threads();
    int compr_num_threads = max_threads - 2;
    int decompr_num_threads = max_threads - 1;
    run_test_different_threads(compr_num_threads, decompr_num_threads);
}

TEST_P(API_compress_MT, AOCL_Compression_api_aocl_llc_compress_and_decompr_thread_count_greater_than_maximum_available_threads_common) // decompr_thread_count > omp_get_max_threads() & compr_thread_count > omp_get_max_threads()
{
    skip_test_if_algo_invalid(atp.algo);
    reset_ACD(&desc, algo_levels[atp.algo].def);
    int max_threads = omp_get_max_threads();
    int compr_num_threads = max_threads + 1;
    int decompr_num_threads = max_threads + 2;
    run_test_different_threads(compr_num_threads, decompr_num_threads);
}

INSTANTIATE_TEST_SUITE_P(
    API_TEST,
    API_compress_MT,
    ::testing::ValuesIn(get_api_test_params_mt()));

/*********************************************
 * End Multithreaded Compress Tests
 ********************************************/

AOCL_INT32 Test_aocl_setup_parallel_compress_mt(aocl_thread_group_t* thread_grp,
    AOCL_CHAR* src, AOCL_CHAR* dst, AOCL_INT32 in_size,
    AOCL_INT32 out_size, AOCL_INT32 window_len,
    AOCL_INT32 window_factor) {
    return aocl_setup_parallel_compress_mt(thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
}

AOCL_INT32 Test_aocl_do_partition_compress_mt(aocl_thread_group_t* thread_grp,
    aocl_thread_info_t* cur_thread_info,
    AOCL_UINT32 cmpr_bound_pad, AOCL_UINT32 thread_id) {
    return aocl_do_partition_compress_mt(thread_grp, cur_thread_info, 
        cmpr_bound_pad, thread_id);
}

void Test_aocl_destroy_parallel_compress_mt(aocl_thread_group_t* thread_grp) {
    aocl_destroy_parallel_compress_mt(thread_grp);
}

AOCL_INT32 Test_aocl_setup_parallel_decompress_mt(aocl_thread_group_t* thread_grp,
    AOCL_CHAR* src, AOCL_CHAR* dst, AOCL_INT32 in_size,
    AOCL_INT32 out_size, AOCL_INT32 use_ST_decompressor) {
    return aocl_setup_parallel_decompress_mt(thread_grp,
        src, dst, in_size, out_size, use_ST_decompressor);
}

AOCL_INT32 Test_aocl_do_partition_decompress_mt(aocl_thread_group_t* thread_grp,
    aocl_thread_info_t* cur_thread_info,
    AOCL_UINT32 cmpr_bound_pad, AOCL_UINT32 thread_id) {
    return aocl_do_partition_decompress_mt(thread_grp,
        cur_thread_info, cmpr_bound_pad, thread_id);
}

void Test_aocl_destroy_parallel_decompress_mt(aocl_thread_group_t* thread_grp) {
    aocl_destroy_parallel_decompress_mt(thread_grp);
}

void init_thread_group(aocl_thread_group_t* thread_grp) {
    thread_grp->threads_info_list = nullptr;
    thread_grp->src = nullptr;
    thread_grp->dst = nullptr;
    thread_grp->src_size = 0;
    thread_grp->dst_size = 0;
    thread_grp->common_part_src_size = 0;
    thread_grp->leftover_part_src_bytes = 0;
    thread_grp->num_threads = 0;
    thread_grp->search_window_length = 0;
}

/*********************************************
* Begin multi-threaded compress setup Tests
*********************************************/
class API_setup_parallel_compress_MT : public ::testing::Test {
public:
    void SetUp() override {
        init_thread_group(&thread_grp);

        in_size  = buff_size;
        out_size = buff_size;
        src = (AOCL_CHAR*)calloc(in_size, sizeof(AOCL_CHAR));
        dst = (AOCL_CHAR*)calloc(out_size, sizeof(AOCL_CHAR));
    }

    void TearDown() override {
        if (src) free(src);
        if (dst) free(dst);
    }

    void validate(AOCL_INT32 rap_frame_len, AOCL_INT32 window_len) {
        // validate thread_grp is set
        EXPECT_EQ(thread_grp.src, src);
        EXPECT_EQ(thread_grp.dst, dst);
        EXPECT_EQ(thread_grp.src_size, in_size);
        EXPECT_EQ(thread_grp.dst_size, out_size);
        EXPECT_EQ(thread_grp.search_window_length, window_len);

        if (rap_frame_len > 0) { // RAP frame is added. Validate it.
            EXPECT_GE(thread_grp.num_threads, 1);

            // validate threads_info_list is set
            EXPECT_NE(thread_grp.threads_info_list, nullptr);
            if (thread_grp.num_threads > 1) {
                // access items in threads_info_list. If allocated properly, this should not produce any out of bound access in memory leak checks.
                for (int i = 0; i < thread_grp.num_threads; ++i) {
                    thread_grp.threads_info_list[i].dst_trap = NULL;
                }
            }

            // validate RAP frame header
            //| <--RAP Magic word(8 bytes)--> | <--RAP Metadata length(4 bytes)--> |
            //<--- Num Main Threads (2 bytes) --> | <-- Num Child Threads (2 bytes) ---> |
            AOCL_CHAR* rap_head = thread_grp.dst;
            EXPECT_EQ(rap_frame_len, RAP_FRAME_LEN_WITH_DECOMP_LENGTH(thread_grp.num_threads, 0));
            EXPECT_EQ(*(AOCL_INT64*)rap_head, RAP_MAGIC_WORD);
            EXPECT_EQ(*(AOCL_UINT32*)(rap_head + 8), rap_frame_len);
            EXPECT_EQ(*(AOCL_SHORT*)(rap_head + 12), thread_grp.num_threads);
            EXPECT_EQ(*(AOCL_SHORT*)(rap_head + 14), 0);
        }
        else { // no RAP frame. Run single threaded.
            EXPECT_EQ(thread_grp.num_threads, 1);
        }
    }

    aocl_thread_group_t thread_grp;
    AOCL_CHAR* src, * dst;
    AOCL_INT32 in_size, out_size;
    const AOCL_INT32 buff_size = 1024 * 16;
};

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_1) { // window_len is a factor of in_size
    AOCL_INT32 window_len = in_size / 16;
    AOCL_INT32 window_factor = 2;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    validate(res, window_len);
    EXPECT_LE(thread_grp.num_threads, 8); // num_parallel_partitions = 8 based on window_len and window_factor
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_2) { // window_len is not a factor of in_size
    AOCL_INT32 window_len = (in_size / 16) + 1;
    AOCL_INT32 window_factor = 2;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    validate(res, window_len);
    EXPECT_LE(thread_grp.num_threads, 8); // num_parallel_partitions = 8 based on window_len and window_factor
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_3) { // window_factor = 1
    AOCL_INT32 window_len = in_size / 16;
    AOCL_INT32 window_factor = 1;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    validate(res, window_len);
    EXPECT_LE(thread_grp.num_threads, 16); // num_parallel_partitions = 16 based on window_len and window_factor
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_4) { // high leftover_part_src_bytes
    AOCL_INT32 window_len = (in_size / 2) + 1; // larger than half
    AOCL_INT32 window_factor = 1;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    validate(res, window_len);
    EXPECT_LE(thread_grp.num_threads, 2); // num_parallel_partitions = 2 based on window_len and window_factor
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_5) { // src_size < chunk_size
    AOCL_INT32 window_len = in_size;
    AOCL_INT32 window_factor = 2;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    validate(res, window_len);
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_6) { // src = NULL, valid
    AOCL_INT32 window_len = in_size / 16;
    AOCL_INT32 window_factor = 2;
    src = NULL;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    validate(res, window_len);
    EXPECT_LE(thread_grp.num_threads, 8); // num_parallel_partitions = 8 based on window_len and window_factor
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_7) { // dst = NULL, invalid
    AOCL_INT32 window_len = in_size / 16;
    AOCL_INT32 window_factor = 2;
    dst = NULL;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    EXPECT_EQ(res, ERR_INVALID_INPUT); //cant write RAP frame header when dst is null
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_8) { // in_size = 0, valid
    AOCL_INT32 window_len = in_size / 16;
    AOCL_INT32 window_factor = 2;
    in_size = 0;
    Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    EXPECT_EQ(thread_grp.num_threads, 1); // single thread. No RAP frame header written.
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_9) { // out_size = 0, valid
    AOCL_INT32 window_len = in_size / 16;
    AOCL_INT32 window_factor = 2;
    out_size = 0;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    validate(res, window_len);
    EXPECT_LE(thread_grp.num_threads, 8); // num_parallel_partitions = 8 based on window_len and window_factor
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_10) { // window_len = 0, invalid
    AOCL_INT32 window_len = 0;
    AOCL_INT32 window_factor = 2;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    EXPECT_EQ(res, ERR_INVALID_INPUT);
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

TEST_F(API_setup_parallel_compress_MT, AOCL_Compression_api_aocl_setup_parallel_compress_mt_common_11) { // window_factor = 0, invalid
    AOCL_INT32 window_len = in_size;
    AOCL_INT32 window_factor = 0;
    AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
        out_size, window_len, window_factor);
    EXPECT_EQ(res, ERR_INVALID_INPUT);
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
}

/*********************************************
* End multi-threaded compress setup Tests
*********************************************/

/*********************************************
* Begin multi-threaded compress partition Tests
*********************************************/
class API_do_partition_compress_MT : public ::testing::Test {
public:
    void SetUp() override {
        init_thread_group(&thread_grp);

        in_size = buff_size;
        out_size = buff_size;
        src = (AOCL_CHAR*)calloc(in_size, sizeof(AOCL_CHAR));
        dst = (AOCL_CHAR*)calloc(out_size, sizeof(AOCL_CHAR));

        AOCL_INT32 window_len = in_size / 16;
        AOCL_INT32 window_factor = 2;
        AOCL_INT32 res = Test_aocl_setup_parallel_compress_mt(&thread_grp, src, dst, in_size,
            out_size, window_len, window_factor);
        ASSERT_GE(res, 0);
    }

    void TearDown() override {
        Test_aocl_destroy_parallel_compress_mt(&thread_grp);
        if (src) free(src);
        if (dst) free(dst);
    }

    void validate(AOCL_UINT32 cmpr_bound_pad) {
        AOCL_UINT32 num_threads = thread_grp.num_threads;
        for (AOCL_UINT32 thread_id = 0; thread_id < num_threads; ++thread_id) {
            // partition_src non-NULL and within src bounds
            EXPECT_NE(thread_grp.threads_info_list[thread_id].partition_src, nullptr);
            EXPECT_GE(thread_grp.threads_info_list[thread_id].partition_src, thread_grp.src);
            EXPECT_LT(thread_grp.threads_info_list[thread_id].partition_src, thread_grp.src + thread_grp.src_size);

            // partition_src_size within bounds
            EXPECT_GE(thread_grp.threads_info_list[thread_id].partition_src_size, thread_grp.common_part_src_size);
            EXPECT_LE(thread_grp.threads_info_list[thread_id].partition_src_size, 
                thread_grp.common_part_src_size + thread_grp.leftover_part_src_bytes);

            // dst buffer allocated
            EXPECT_NE(thread_grp.threads_info_list[thread_id].dst_trap, nullptr);

            // dst buffer size set properly
            EXPECT_EQ(thread_grp.threads_info_list[thread_id].dst_trap_size, 
                thread_grp.threads_info_list[thread_id].partition_src_size + cmpr_bound_pad);
        }
    }

    aocl_thread_group_t thread_grp;
    AOCL_CHAR* src, * dst;
    AOCL_INT32 in_size, out_size;
    const AOCL_INT32 buff_size = 1024 * 16;
};

TEST_F(API_do_partition_compress_MT, AOCL_Compression_api_aocl_do_partition_compress_mt_common_1) { // partition the problem
    aocl_thread_info_t cur_thread_info;
    const AOCL_UINT32 cmpr_bound_pad = 16;
    #pragma omp parallel private(cur_thread_info) shared(thread_grp, cmpr_bound_pad) num_threads(thread_grp.num_threads)
    {
        AOCL_UINT32 thread_id = omp_get_thread_num();
        EXPECT_EQ(Test_aocl_do_partition_compress_mt(&thread_grp, &cur_thread_info, cmpr_bound_pad, thread_id), 0);

        thread_grp.threads_info_list[thread_id].partition_src         = cur_thread_info.partition_src;
        thread_grp.threads_info_list[thread_id].dst_trap              = cur_thread_info.dst_trap;
        thread_grp.threads_info_list[thread_id].additional_state_info = cur_thread_info.additional_state_info;
        thread_grp.threads_info_list[thread_id].partition_src_size    = cur_thread_info.partition_src_size;
        thread_grp.threads_info_list[thread_id].dst_trap_size         = cur_thread_info.dst_trap_size;
        thread_grp.threads_info_list[thread_id].last_bytes_len        = cur_thread_info.last_bytes_len;
        thread_grp.threads_info_list[thread_id].num_child_threads     = 0; // not used as of now
        thread_grp.threads_info_list[thread_id].is_error              = cur_thread_info.is_error;
        thread_grp.threads_info_list[thread_id].thread_id             = cur_thread_info.thread_id;
        thread_grp.threads_info_list[thread_id].next                  = cur_thread_info.next;
    } // #pragma omp parallel
    validate(cmpr_bound_pad);
}

/*********************************************
* End multi-threaded compress partition Tests
*********************************************/

/*********************************************
* Begin multi-threaded compress destroy Tests
*********************************************/
TEST(API_destroy_parallel_compress_MT, AOCL_Compression_api_aocl_destroy_parallel_compress_mt_common_1) { // threads_info_list non-NULL
    aocl_thread_group_t thread_grp;
    thread_grp.num_threads = 16;
    thread_grp.threads_info_list = (aocl_thread_info_t*)calloc(thread_grp.num_threads, sizeof(aocl_thread_info_t));
    for (AOCL_INT32 thread_cnt = 0; thread_cnt < thread_grp.num_threads; thread_cnt++) {
        thread_grp.threads_info_list[thread_cnt].dst_trap = (AOCL_CHAR*)malloc(4);
    }

    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
    
    EXPECT_EQ(thread_grp.threads_info_list, nullptr);
}

TEST(API_destroy_parallel_compress_MT, AOCL_Compression_api_aocl_destroy_parallel_compress_mt_common_2) { // threads_info_list NULL
    aocl_thread_group_t thread_grp;
    thread_grp.threads_info_list = nullptr;
    
    Test_aocl_destroy_parallel_compress_mt(&thread_grp);
    
    EXPECT_EQ(thread_grp.threads_info_list, nullptr);
}
/*********************************************
* End multi-threaded compress destroy Tests
*********************************************/

AOCL_INT32 add_RAP_frame_header(AOCL_CHAR* buf, AOCL_INT32 num_threads) {
    AOCL_INT32 rap_frame_len = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(num_threads, 0);
    *(AOCL_INT64*)buf = RAP_MAGIC_WORD; //For storing the magic word
    buf += RAP_MAGIC_WORD_BYTES;
    *(AOCL_UINT32*)buf = rap_frame_len; //For storing the total RAP frame length
    buf += RAP_METADATA_LEN_BYTES;
    *(AOCL_UINT32*)buf = num_threads; //For storing the no. of threads
    return rap_frame_len;
}

/*********************************************
* Begin multi-threaded decompress setup Tests
*********************************************/
class API_setup_parallel_decompress_MT : public ::testing::Test {
public:
    void SetUp() override {
        init_thread_group(&thread_grp);
        in_size = 0;
        src = NULL;
        out_size = buff_size;
        dst = (AOCL_CHAR*)calloc(out_size, sizeof(AOCL_CHAR));
    }

    void TearDown() override {
        Test_aocl_destroy_parallel_decompress_mt(&thread_grp);
        if (src) free(src);
        if (dst) free(dst);
    }

    void create_src_with_RAP_frame_header(AOCL_INT32 main_threads) {
        in_size = buff_size;
        src = (AOCL_CHAR*)calloc(in_size, sizeof(AOCL_CHAR));
        add_RAP_frame_header(src, main_threads);
    }

    void validate(AOCL_INT32 rap_metadata_len) {
        // validate thread_grp is set
        EXPECT_EQ(thread_grp.src, src);
        EXPECT_EQ(thread_grp.dst, dst);
        EXPECT_EQ(thread_grp.src_size, in_size);
        EXPECT_EQ(thread_grp.dst_size, out_size);

        if (rap_metadata_len > 0 && thread_grp.num_threads > 1) { // RAP frame is added. Validate it.
            // validate threads_info_list is set
            EXPECT_NE(thread_grp.threads_info_list, nullptr);
            if (thread_grp.num_threads > 1) {
                // access items in threads_info_list. If allocated properly, this should not produce any out of bound access in memory leak checks.
                for (int i = 0; i < thread_grp.num_threads; ++i) {
                    thread_grp.threads_info_list[i].dst_trap = NULL;
                }
            }
        }
        else { // no RAP frame / run single threaded.
            EXPECT_EQ(thread_grp.num_threads, 1);
        }
    }

    aocl_thread_group_t thread_grp;
    AOCL_CHAR* src, * dst;
    AOCL_INT32 in_size, out_size;
    const AOCL_INT32 buff_size = 1024 * 16;
};

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_1) { // RAP frame present
    create_src_with_RAP_frame_header(8);

    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    validate(rap_metadata_len);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_2) { // No RAP frame
    in_size = buff_size;
    src = (AOCL_CHAR*)calloc(in_size, sizeof(AOCL_CHAR));

    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    EXPECT_EQ(rap_metadata_len, 0);
    validate(rap_metadata_len);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_3) { // use_ST_decompressor
    create_src_with_RAP_frame_header(8);

    AOCL_INT32 use_ST_decompressor = 1;
    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, use_ST_decompressor);
    EXPECT_GT(rap_metadata_len, 0);
    //thread_grp values are not set when use_ST_decompressor = 1. Hence no validation done here.
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_4) { // src_size < RAP_MAGIC_WORD_BYTES
    in_size = RAP_MAGIC_WORD_BYTES - 1;
    src = (AOCL_CHAR*)calloc(in_size, sizeof(AOCL_CHAR));

    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    EXPECT_EQ(rap_metadata_len, 0);
    validate(rap_metadata_len);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_5) { // num_main_threads = 0
    create_src_with_RAP_frame_header(0); // 0 main threads

    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    EXPECT_EQ(rap_metadata_len, -1);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_6) { // num_main_threads = 1
    create_src_with_RAP_frame_header(1);

    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    EXPECT_GT(rap_metadata_len, 0);
    validate(rap_metadata_len);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_7) { // src = NULL, invalid
    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    EXPECT_EQ(rap_metadata_len, ERR_INVALID_INPUT);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_8) { // dst = NULL, valid
    create_src_with_RAP_frame_header(8);
    if (dst) free(dst);
    dst = NULL;

    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    validate(rap_metadata_len);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_9) { // in_size = 0, valid
    create_src_with_RAP_frame_header(8);
    in_size = 0;
    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    validate(rap_metadata_len);
}

TEST_F(API_setup_parallel_decompress_MT, AOCL_Compression_api_aocl_setup_parallel_decompress_mt_common_10) { // out_size = 0, valid
    create_src_with_RAP_frame_header(8);
    out_size = 0;

    AOCL_INT32 rap_metadata_len = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
        out_size, 0);
    validate(rap_metadata_len);
}
/*********************************************
* End multi-threaded decompress setup Tests
*********************************************/

/*********************************************
* Begin multi-threaded decompress partition Tests
*********************************************/
class API_do_partition_decompress_MT : public ::testing::Test {
public:
    void SetUp() override {
        //init
        init_thread_group(&thread_grp);
        out_size = buff_size;
        dst = (AOCL_CHAR*)calloc(out_size, sizeof(AOCL_CHAR));
        for (AOCL_UINT32 thread_cnt = 0; thread_cnt < num_threads; ++thread_cnt) {
            cpr_chunk_len[thread_cnt] = 32 * (thread_cnt + 1); //add different values for rap len for each chunk
            dpr_chunk_len[thread_cnt] = 48 * (thread_cnt + 1);
        }

        //create compressed stream with RAP frame
        in_size = buff_size;
        src = (AOCL_CHAR*)calloc(in_size, sizeof(AOCL_CHAR));
        add_RAP_frame_header(src, num_threads); //add RAP frame header
        AOCL_INT32 dst_offset = RAP_START_OF_PARTITIONS;
        //add RAP frame metadata
        AOCL_CHAR* src_ptr = src + dst_offset;
        for (AOCL_UINT32 thread_cnt = 0; thread_cnt < num_threads; thread_cnt++)
        {
            *(AOCL_UINT32*)src_ptr = dst_offset; //for storing this thread's RAP offset
            src_ptr += RAP_OFFSET_BYTES;

            AOCL_INT32 cur_rap_len = cpr_chunk_len[thread_cnt];
            *(AOCL_INT32*)src_ptr = cur_rap_len; //for storing this thread's RAP length
            src_ptr += RAP_LEN_BYTES;

            *(AOCL_INT32*)src_ptr = dpr_chunk_len[thread_cnt];
            src_ptr += DECOMP_LEN_BYTES;

            dst_offset += cur_rap_len;
        }

        //setup
        AOCL_INT32 res = Test_aocl_setup_parallel_decompress_mt(&thread_grp, src, dst, in_size,
            out_size, 0);
        ASSERT_GE(res, 0);
    }

    void TearDown() override {
        Test_aocl_destroy_parallel_decompress_mt(&thread_grp);
        if (src) free(src);
        if (dst) free(dst);
    }

    void validate(AOCL_UINT32 cmpr_bound_pad) {
        AOCL_UINT32 num_threads = thread_grp.num_threads;
        if (num_threads > 1) { //sufficient num of threads exists on decompressor side to process compressed chunks in parallel
            AOCL_INT32 dst_offset = RAP_START_OF_PARTITIONS;
            for (AOCL_UINT32 thread_id = 0; thread_id < num_threads; ++thread_id) {
                // partition_src non-NULL and are pointing to start of each chunk
                EXPECT_NE(thread_grp.threads_info_list[thread_id].partition_src, nullptr);
                EXPECT_EQ(thread_grp.threads_info_list[thread_id].partition_src, thread_grp.src + dst_offset);

                // partition_src_size
                EXPECT_EQ(thread_grp.threads_info_list[thread_id].partition_src_size, cpr_chunk_len[thread_id]);

                // dst buffer allocated
                EXPECT_NE(thread_grp.threads_info_list[thread_id].dst_trap, nullptr);

                // dst buffer size set properly
                EXPECT_EQ(thread_grp.threads_info_list[thread_id].dst_trap_size, dpr_chunk_len[thread_id] + cmpr_bound_pad);

                dst_offset += cpr_chunk_len[thread_id];
            }
        }
    }

    aocl_thread_group_t thread_grp;
    AOCL_CHAR* src, * dst;
    AOCL_INT32 in_size, out_size;
    const AOCL_INT32 buff_size = 1024 * 16;
    const AOCL_INT32 num_threads = 4;
    AOCL_INT32 cpr_chunk_len[4]; //one per thread. chunk sizes of compressed data.
    AOCL_INT32 dpr_chunk_len[4]; //one per thread. chunk sizes of expected decompressed data.
};

TEST_F(API_do_partition_decompress_MT, AOCL_Compression_api_aocl_do_partition_decompress_mt_common_1) { // partition the problem
    aocl_thread_info_t cur_thread_info;
    const AOCL_UINT32 cmpr_bound_pad = 16;
    //for(int thread_id =0; thread_id< num_threads;++thread_id)
#pragma omp parallel private(cur_thread_info) shared(thread_grp, cmpr_bound_pad) num_threads(thread_grp.num_threads)
    {
        AOCL_UINT32 thread_id = omp_get_thread_num();
        EXPECT_EQ(Test_aocl_do_partition_decompress_mt(&thread_grp, &cur_thread_info, cmpr_bound_pad, thread_id), 0);

        thread_grp.threads_info_list[thread_id].partition_src         = cur_thread_info.partition_src;
        thread_grp.threads_info_list[thread_id].dst_trap              = cur_thread_info.dst_trap;
        thread_grp.threads_info_list[thread_id].additional_state_info = cur_thread_info.additional_state_info;
        thread_grp.threads_info_list[thread_id].partition_src_size    = cur_thread_info.partition_src_size;
        thread_grp.threads_info_list[thread_id].dst_trap_size         = cur_thread_info.dst_trap_size;
        thread_grp.threads_info_list[thread_id].last_bytes_len        = cur_thread_info.last_bytes_len;
        thread_grp.threads_info_list[thread_id].num_child_threads     = 0; // not used as of now
        thread_grp.threads_info_list[thread_id].is_error              = cur_thread_info.is_error;
        thread_grp.threads_info_list[thread_id].thread_id             = cur_thread_info.thread_id;
        thread_grp.threads_info_list[thread_id].next                  = cur_thread_info.next;
    } // #pragma omp parallel
    validate(cmpr_bound_pad);
}

/*********************************************
* End multi-threaded decompress partition Tests
*********************************************/

/*********************************************
* Begin multi-threaded decompress destroy Tests
*********************************************/
TEST(API_destroy_parallel_decompress_MT, AOCL_Compression_api_aocl_destroy_parallel_decompress_mt_common_1) { // threads_info_list non-NULL
    aocl_thread_group_t thread_grp;
    thread_grp.num_threads = 16;
    thread_grp.threads_info_list = (aocl_thread_info_t*)calloc(thread_grp.num_threads, sizeof(aocl_thread_info_t));
    for (AOCL_INT32 thread_cnt = 0; thread_cnt < thread_grp.num_threads; thread_cnt++) {
        thread_grp.threads_info_list[thread_cnt].dst_trap = (AOCL_CHAR*)malloc(4);
    }

    Test_aocl_destroy_parallel_decompress_mt(&thread_grp);

    EXPECT_EQ(thread_grp.threads_info_list, nullptr);
}

TEST(API_destroy_parallel_decompress_MT, AOCL_Compression_api_aocl_destroy_parallel_decompress_mt_common_2) { // threads_info_list NULL
    aocl_thread_group_t thread_grp;
    thread_grp.threads_info_list = nullptr;

    Test_aocl_destroy_parallel_decompress_mt(&thread_grp);

    EXPECT_EQ(thread_grp.threads_info_list, nullptr);
}
/*********************************************
* End multi-threaded compress destroy Tests
*********************************************/

#endif /* AOCL_ENABLE_THREADS */
