/**
 * Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
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
 
 /** @file gtest_utils.cpp
 *  
 *  @brief Provides utility functions for individual gtest algos.
 *
 *  @author J Niranjan Reddy
 */

#include "utils/utils.h"
#include "gtest_utils.h"
/*
    This function returns parameters for parameterised test in the form of {optimization on/off, optimization level}.
*/
vector<DynamicDispatch> get_dynamic_dispatcher_flags(aocl_compression_type method)
{
    vector<DynamicDispatch> params;
    // For methods other than SNAPPY, if optimization is off there is only one code path that dynamic dispatcher selects
    // but for SNAPPY, even if optimization is off, there are different intrinsic code paths to choose from depending on
    // machine support.
    if(method == SNAPPY)
    {
        for(int i = get_cpu_opt_flags(0); i > 0; i--)
        {
            params.push_back({1, i});   // With Intrinsics & optimization is off, only for snappy
        }
    }
    params.push_back({1, 0});   // No optimization

    for(int i = get_cpu_opt_flags(0); i >= 0; i--)
    {
        params.push_back({0, i});   // With Intrinsics & AOCL path
    }
    return params;
}

vector<int> get_supported_optlevels(void) {
    vector<int> optlevels;
    int highest_supported_level = get_cpu_opt_flags(0);
    while (highest_supported_level >= 0) {
        optlevels.push_back(highest_supported_level);
        highest_supported_level--;
    }
    return optlevels;
}

const std::vector<std::string> gtest_data_gen_t::randomStrs({ "qwertyuiop", "asdfghjkl", "zxcvbnm", "1234567890",
"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM", "!@#$%^&*()" });

bool gtest_data_gen_t::dstSet(unsigned char val, char* start, char* end) { // fill dst buffer
    if (start > end || start < compressed_data || end >(compressed_data + compressed_sz))
        return false;
    memset(start, val, end - start);
    return true;
}

void gtest_data_gen_t::fill_random(char* buf, size_t sz) { // fill buffer with random data
    for (size_t i = 0; i < sz; i++)
    {
        buf[i] = rand() % 255;
    }
}

void gtest_data_gen_t::fill_repeated(char* buf, size_t sz) { // fill buffer with repeating patterns
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

void gtest_data_gen_t::fill_lowratio(char* buf, size_t sz) { // fill buffer with highly compressible data
    memset(buf, 0, sz);
    int randId = 0; // pick a specific string
    size_t cur = 0;
    while (cur < sz) {
        int randLen = rand() % randomStrs[randId].size(); // select sub string length
        if (cur + randLen >= sz) break;

        memcpy(buf + cur, randomStrs[randId].c_str(), randLen * sizeof(char));
        cur += randLen;
    }
}

void gtest_data_gen_t::fill_highratio(char* buf, size_t sz) { // fill buffer with low compressible data
    fill_random(buf, sz);
}

void gtest_data_gen_t::fill_midratio(char* buf, size_t sz) { // fill buffer with moderately compressible data
    memset(buf, 0, sz);
    for (size_t i = 0; i < sz; i += 128) {
        if (i + 128 > sz) break;
        if (i % 256 == 0) {
            fill_lowratio(buf + i, 128); // fill with low compressible data
        }
        else {
            fill_highratio(buf + i, 128); // fill with high compressible data
        }
    }
}

void gtest_data_gen_t::fill_overlapcopy(char* buf, size_t sz) { // fill buffer with data that creates overlapping copies
    memset(buf, 0, sz);
    for (size_t i = 0; i < sz; i += 6) {
        if (i + 6 > sz) break;
        memcpy(buf + i, "abcdef", 6 * sizeof(char)); // fill with repeating pattern
    }
}

void gtest_data_gen_t::fill_longmatch(char* buf, size_t sz) { // fill buffer with data that creates long matches
    fill_random(buf, sz);
    //insert pattern once every 256*1024 bytes
    for (size_t i = 0; i < sz; i += 256*1024) {
        if (i + 12 > sz) break;
        memcpy(buf + i, "abcdefghijkl", 12 * sizeof(char)); // fill pattern
    }
}

std::vector<size_t> gtest_data_gen_t::get_array_of_samples(char* samplesBuffer, size_t sz) {
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

void gtest_data_gen_t::fill_source(gtest_data_gen_type type) {
    switch (type) {
        case gtest_data_gen_type::repeated:
            fill_repeated(orig_data, orig_sz);
            break;
        case gtest_data_gen_type::lowratio:
            fill_lowratio(orig_data, orig_sz);
            break;
        case gtest_data_gen_type::midratio:
            fill_midratio(orig_data, orig_sz);
            break;
        case gtest_data_gen_type::highratio:
            fill_highratio(orig_data, orig_sz);
            break;
        case gtest_data_gen_type::overlapcopy:
            fill_overlapcopy(orig_data, orig_sz);
            break;
        case gtest_data_gen_type::longmatch:
            fill_longmatch(orig_data, orig_sz);
            break;
        default:
            fill_random(orig_data, orig_sz);
            break;
    };
}

void gtest_data_gen_t::create_source(size_t inp_sz, gtest_data_gen_type type) {
    orig_data = (char*)malloc(inp_sz);
    if (orig_data == NULL) {
        throw std::runtime_error("Failed to allocate memory for source buffer.");
    }
    fill_source(type);
}
