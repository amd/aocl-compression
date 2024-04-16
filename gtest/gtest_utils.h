/**
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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
 
 /** @file gtest_utils.h
 *  
 *  @brief Provides utility functions for individual gtest algos.
 *
 *  @author J Niranjan Reddy
 */

#ifndef _GTEST_UTILS_H_
#define _GTEST_UTILS_H_

#include <random>
#include <vector>
#include <string>
#include <tuple>
#include <functional>
#include <stdexcept>
#include "aocl_compression.h"

using namespace std;

// Struct for generating input parameters for parameterized tests which change the optimization level(code path) dynamically.
typedef struct
{
    int optOff;
    int optLevel;
} DynamicDispatch;


// Parent class for parametrized (optimization level) tests.
#define OPT_LEVEL_TESTS ::testing::TestWithParam<DynamicDispatch>

/*
    This function returns parameters for parameterised test in the form of {optimization on/off, optimization level}.
*/
vector<DynamicDispatch> get_dynamic_dispatcher_flags(aocl_compression_type method = AOCL_COMPRESSOR_ALGOS_NUM);


/* Data generator class to provide different types of random input streams
*  Manages buffers for generated original data and compressed data */
typedef size_t(*gtest_compress_bound_t) (size_t);
enum class gtest_data_gen_type { random, repeated };

class gtest_data_gen_t {
public:
    // Constructors create source data of inp_sz
    gtest_data_gen_t(gtest_compress_bound_t cb, size_t inp_sz,
        gtest_data_gen_type type = gtest_data_gen_type::random) : orig_sz(inp_sz)
    {
        create_source(inp_sz, type);

        // Provides the maximum size that compression may output in a "worst case".
        compressed_sz = cb(inp_sz);
        compressed_data = (char*)malloc(compressed_sz);
    }

    gtest_data_gen_t(gtest_compress_bound_t cb, size_t inp_sz, size_t out_sz,
        gtest_data_gen_type type = gtest_data_gen_type::random) : orig_sz(inp_sz)
    {
        create_source(inp_sz, type);

        compressed_sz = out_sz; // custom out_sz
        compressed_data = (char*)malloc(compressed_sz);
    }

    // Returns pointer to source buffer.
    char* getOrigBuff()
    {
        return orig_data;
    }

    // Returns size of source buffer.
    size_t getOrigSize()
    {
        return orig_sz;
    }

    // Returns pointer to destination buffer (data obtained after compression).
    char* getCompressedBuff()
    {
        return compressed_data;
    }

    // Returns size of destination buffer.
    size_t getCompressedSize()
    {
        return compressed_sz;
    }

    // Fill dst buffer
    bool dstSet(unsigned char val, char* start, char* end) {
        if (start > end || start < compressed_data || end >(compressed_data + compressed_sz))
            return false;
        memset(start, val, end - start);
        return true;
    }

    // Destructor function.
    ~gtest_data_gen_t()
    {
        if (orig_data) free(orig_data);
        if (compressed_data) free(compressed_data);
        orig_data = NULL;
        compressed_data = NULL;
    }

    static void fill_random(char* buf, size_t sz) { // fill buffer with random data
        for (size_t i = 0; i < sz; i++)
        {
            buf[i] = rand() % 255;
        }
    }

    static void fill_repeated(char* buf, size_t sz) { // fill buffer with repeating patterns
        memset(buf, 0, sz);
        size_t cur = 0;
        while (cur < sz) {
            int randId = rand() % randomStrs.size(); // pick a string at random
            int randLen = rand() % randomStrs[randId].size(); // select sub string length
            if (cur + randLen >= sz) break;

            memcpy(buf + cur, randomStrs[randId].c_str(), randLen * sizeof(char));
            cur += randLen;
        }
    }

    static std::vector<size_t> get_array_of_samples(char* samplesBuffer, size_t sz) {
        std::vector<size_t> samplesSizes;
        size_t pos = 0;
        while (pos < sz) {
            int randId = rand() % randomStrs.size(); // pick a string at random
            int repeat = (rand() % 5) + 1;
            std::string str;
            for (int i = 0; i < repeat; ++i) { // append N copies of the string to create samples of different sizes
                str.append(randomStrs[randId]);
            }
            if ((pos + str.size()) > sz) {
                size_t last_sz = sz - pos;
                memcpy(samplesBuffer, str.c_str(), last_sz);
                samplesSizes.push_back(last_sz);
                break;
            }
            size_t cur_sz = str.size();
            memcpy(samplesBuffer, str.c_str(), cur_sz);
            samplesSizes.push_back(cur_sz);
            pos += cur_sz;
        }
        return samplesSizes;
    }

private:
    void create_source(size_t inp_sz, gtest_data_gen_type type) {
        orig_data = (char*)malloc(inp_sz);
        switch (type) {
        case gtest_data_gen_type::repeated:
            fill_repeated(orig_data, orig_sz);
            break;
        default:
            fill_random(orig_data, orig_sz);
            break;
        };
    }

    //source buffer (original data which we intend to compress).
    char* orig_data = NULL;
    const size_t orig_sz = 0;
    //destination buffer (data obtained after compression).
    char* compressed_data = NULL;
    size_t compressed_sz = 0;

    static const std::vector<std::string> randomStrs;
};

/* Fuzz testing utils */
#define FUZZ_TEST_NAME ::testing::UnitTest::GetInstance()->current_test_info()->name()
#define FUZZ_CPR_SEED "compress_fuzz"
#define FUZZ_SIZE_MAX 512 // allocating buffers larger than this will slow down fuzz testing. Limit all src and dst buffers to this size.
#define FUZZ_CPR_RATIO 30.0 // expect compression ratio to be 30 or higher

static std::string get_env_var_str(const char* env_var) {
    const char* corpus_dir = std::getenv(env_var);
    if (corpus_dir == NULL) 
        return std::string(); // empty string
    else 
        return std::string(corpus_dir);
}

template <typename T>
static T get_env_var_abs(const char* env_var, T default_val) {
    std::string env_str = get_env_var_str(env_var);
    if (env_str.empty()) 
        return default_val;
    else {
        try {
            T ret;
            if (std::is_same<T, int>::value)
                ret = std::stoi(env_str);
            else
                ret = (T)std::stof(env_str);
            return ret < 0 ? default_val : ret;
        } catch (std::invalid_argument& e) {
            return default_val;
        } catch (std::out_of_range& e) {
            return default_val;
        } catch (...) {
            return default_val;
        }
    }
}

/* To run a single fuzz test by feeding in an external corpus of seeds: Enabled with cmake option AOCL_TEST_FUZZER_WITH_CORPUS.
   Place folders containing seed files in the directory pointed by environment variable AOCL_FUZZ_CORPUS_DIR.
   If AOCL_FUZZ_CORPUS_DIR is not set, current directory is used as parent directory.
   Sub-folders under this must be as follows:
   *   /compress_fuzz : Must contain uncompressed raw files for compress API fuzz tests.
   *   /xxxxxxxx_fuzz : Folders with individual fuzz test names must contain compressed files 
                        for respective methods used for decompress API fuzz tests.
                        Example: /LZ4_decompress_safe_fuzz, /RawUncompress_fuzz, etc
*/
#define READ_FUZZ_CPR_SEED() fuzztest::ReadFilesFromDirectory(absl::StrCat(get_env_var_str("AOCL_FUZZ_CORPUS_DIR"), FUZZ_CPR_SEED));
#define READ_FUZZ_DPR_SEED() fuzztest::ReadFilesFromDirectory(absl::StrCat(get_env_var_str("AOCL_FUZZ_CORPUS_DIR"), FUZZ_TEST_NAME));

#define READ_FUZZ_SIZE_MAX() get_env_var_abs<int>("AOCL_FUZZ_SIZE_MAX", FUZZ_SIZE_MAX)
#define READ_FUZZ_CPR_RATIO() get_env_var_abs<float>("AOCL_FUZZ_CPR_RATIO", FUZZ_CPR_RATIO)
static inline size_t limit_fuzz_size_max(size_t sz) {
    size_t fuzzSizeMax = READ_FUZZ_SIZE_MAX();
    return (sz > fuzzSizeMax) ? fuzzSizeMax : sz;
}

template <typename T>
using fuzz_cpr_seed_t = vector<tuple<vector<T>, size_t, int, int, int>>;
template <typename T>
using fuzz_dpr_seed_t = vector<tuple<vector<T>, size_t, int, int>>;

template <typename T> 
fuzz_cpr_seed_t<T> get_fuzz_cpr_seeds(gtest_compress_bound_t compress_bound_ptr, int minLevel, int maxLevel,
vector<tuple<string>>& seed_files) {
    fuzz_cpr_seed_t<T> seeds;
    bool optOff = 0;
    int optLevel = 0;
    int level = minLevel;
    size_t fuzzSizeMax = READ_FUZZ_SIZE_MAX();
    for (auto& src_file : seed_files) {
        auto src_data = get<0>(src_file);
        size_t src_sz = (src_data.size() > fuzzSizeMax) ? fuzzSizeMax : src_data.size();
        auto src_end = src_data.begin() + src_sz;
        vector<T> src(src_data.begin(), src_end);
        size_t dst_sz = compress_bound_ptr(src.size());
        dst_sz = dst_sz > fuzzSizeMax ? fuzzSizeMax : dst_sz;
        seeds.push_back({ src, dst_sz, level, optOff, optLevel });
        level = (level == maxLevel) ? minLevel : (level+1);
        optOff = !optOff;
        optLevel++; optLevel %= 5;
    }
    return seeds;
}

template <typename T> 
fuzz_dpr_seed_t<T> get_fuzz_dpr_seeds(vector<tuple<string>>& seed_files) {
    fuzz_dpr_seed_t<T> seeds;
    bool optOff = 0;
    int optLevel = 0;
    size_t fuzzSizeMax = READ_FUZZ_SIZE_MAX();
    float ratio = READ_FUZZ_CPR_RATIO();
    for (auto& src_file : seed_files) {
        auto src_data = get<0>(src_file);
        size_t src_sz = (src_data.size() > fuzzSizeMax) ? fuzzSizeMax : src_data.size();
        auto src_end = src_data.begin() + src_sz;
        vector<T> src(src_data.begin(), src_end);
        size_t dst_sz = src.size() * 100.0 / ratio;
        dst_sz = dst_sz > fuzzSizeMax ? fuzzSizeMax : dst_sz;
        seeds.push_back({ src, dst_sz, optOff, optLevel });
        optOff = !optOff;
        optLevel++; optLevel %= 5;
    }
    return seeds;
}

#endif /* _GTEST_UTILS_H_ */
