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
vector<DynamicDispatch> get_dynamic_dispatcher_flags();


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
#endif /* _GTEST_UTILS_H_ */
