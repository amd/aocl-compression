/**
 * Copyright (C) 2023-2026, Advanced Micro Devices. All rights reserved.
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
 
 /** @file snappy_gtest.cc
 *  
 *  @brief Test cases for SNAPPY algo.
 *
 *  This file contains the test cases for SNAPPY method
 *  testing the API level functions of SNAPPY.
 *
 *  @author J Niranjan Reddy
 */

#include <vector>
#include <string>
#include <random>
#include <limits.h>
#include "gtest/gtest.h"
#include "gtest/gtest_utils.h"

#include "algos/snappy/snappy.h"
#include "algos/snappy/snappy-sinksource.h"
#include "algos/snappy/snappy-internal.h"
#include "algos/snappy/snappy-c.h"
#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif /* AOCL_ENABLE_THREADS */

using namespace std;
using namespace snappy;

/*******************************************
 * "Begin" of Utility functions
 *******************************************/
#define get_dynamic_dispatcher_flags()  get_dynamic_dispatcher_flags(SNAPPY)
// This class acts as a parent class for all the test cases which takes
// a combination of input parameters and different optimization levels.
template <typename T>
class Combined_Dynamic_dispatcher: public ::testing::TestWithParam<tuple<DynamicDispatch, T>>
{
public:
    Combined_Dynamic_dispatcher()
    {
        DynamicDispatch opt = get<0>(::testing::TestWithParam<tuple<DynamicDispatch, T>>::GetParam());
        aocl_destroy_snappy();
        aocl_setup_snappy(opt.optOff, opt.optLevel, 0, 0, 0);
    }
};

// Parameterised tests take more time if raw strings are passed directly when ran in ctest mode.
// So a class is wrapped around a string so that direct access of string
// for test case name generation is restricted.
class STR_WRAPPER
{
    public:
    string parameter;
};

// Similar to the above class but to wrap around a pair of strings.
class PAIR_WRAPPER
{
    public:
    pair<string,string> parameter;
};


// This class acts as a parent class for test cases which needs to run with available optimization levels.
class Standalone_Dynamic_dispatcher: public ::testing::TestWithParam<DynamicDispatch>
{
public:
    Standalone_Dynamic_dispatcher()
    {
        DynamicDispatch opt = GetParam();
        aocl_destroy_snappy();
        aocl_setup_snappy(opt.optOff, opt.optLevel, 0, 0, 0);
    }
};

// This a test case generating function, which returns vector of 
// flawed compressed string to test if the responsible functions catches 
// fault compressed strings.
vector<STR_WRAPPER> uncompress_fail_cases()
{
    vector<string> cases;
    string compressed;

    // Check for an infinite loop caused by a copy with offset==0
    //  \x40              Length (must be > kMaxIncrementCopyOverflow)
    //  \x12\x00\x00      Copy with offset==0, length==5
    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_1
    // AOCL_Compression_snappy_IsValidCompressed_common_1
    // AOCL_Compression_snappy_Uncompress_common_1
    // AOCL_Compression_snappy_Uncompress_src_common_1
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_1
    // AOCL_Compression_snappy_RawUncompress_common_1
    // AOCL_Compression_snappy_RawUncompress_src_common_1
    compressed = "\x40\x12";
    compressed.push_back('\x00');
    compressed.push_back('\x00');
    cases.push_back(compressed);

    // Zero offset copy
    //   \x05              Length
    //   \x12\x00\x00      Copy with offset==0, length==5
    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_2
    // AOCL_Compression_snappy_IsValidCompressed_common_2
    // AOCL_Compression_snappy_Uncompress_common_2
    // AOCL_Compression_snappy_Uncompress_src_common_2
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_2
    // AOCL_Compression_snappy_RawUncompress_common_2
    // AOCL_Compression_snappy_RawUncompress_src_common_2
    compressed = "\x05\x12";
    compressed.push_back('\x00');
    compressed.push_back('\x00');
    cases.push_back(compressed);

    // OverflowingVarint
    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_3
    // AOCL_Compression_snappy_IsValidCompressed_common_3
    // AOCL_Compression_snappy_Uncompress_common_3
    // AOCL_Compression_snappy_Uncompress_src_common_3
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_3
    // AOCL_Compression_snappy_RawUncompress_common_3
    // AOCL_Compression_snappy_RawUncompress_src_common_3
    compressed.clear();
    compressed.push_back('\xfb');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\x7f');
    cases.push_back(compressed);

    // UnterminatedVarint
    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_4
    // AOCL_Compression_snappy_IsValidCompressed_common_4
    // AOCL_Compression_snappy_Uncompress_common_4
    // AOCL_Compression_snappy_Uncompress_src_common_4
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_4
    // AOCL_Compression_snappy_RawUncompress_common_4
    compressed.clear();
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back(10);
    cases.push_back(compressed);

    // TruncatedVarint
    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_5
    // AOCL_Compression_snappy_IsValidCompressed_common_5
    // AOCL_Compression_snappy_Uncompress_common_5
    // AOCL_Compression_snappy_Uncompress_src_common_5
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_5
    // AOCL_Compression_snappy_RawUncompress_common_5
    compressed.clear();
    compressed.push_back('\xf0');
    cases.push_back(compressed);

    string source = "making sure we don't crash with corrupted input";
    string dest;

    Compress(source.data(), source.size(), &dest);

    // Modifying compressed string to introduce errors in compressed string.
    dest[1]--;
    dest[3]++;

    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_6
    // AOCL_Compression_snappy_IsValidCompressed_common_6
    // AOCL_Compression_snappy_Uncompress_common_6
    // AOCL_Compression_snappy_Uncompress_src_common_6
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_6
    // AOCL_Compression_snappy_RawUncompress_common_6
    cases.push_back(dest);

    // This is testing for a security bug - a buffer that decompresses to 100k
    // but we lie in the snappy header and only reserve 0 bytes of memory :)
    source.resize(100000);

    for (char &source_char : source)
    {
        source_char = 'A';
    }

    snappy::Compress(source.data(), source.size(), &dest);
    dest[0] = dest[1] = dest[2] = dest[3] = 0;

    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_7
    // AOCL_Compression_snappy_IsValidCompressed_common_7
    // AOCL_Compression_snappy_Uncompress_common_7
    // AOCL_Compression_snappy_Uncompress_src_common_7
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_7
    // AOCL_Compression_snappy_RawUncompress_common_7
    cases.push_back(dest);

    // Another security check; check a crazy big length can't DoS us with an
    // over-allocation.
    // Currently this is done only for 32-bit builds.  On 64-bit builds,
    // where 3 GB might be an acceptable allocation size, Uncompress()
    // attempts to decompress, and sometimes causes the test to run out of
    // memory.
    dest[0] = dest[1] = dest[2] = dest[3] = '\xff';
    // This decodes to a really large size, i.e., about 3 GB.
    dest[4] = 'k';

    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_8
    // AOCL_Compression_snappy_IsValidCompressed_common_8
    // AOCL_Compression_snappy_Uncompress_common_8
    // AOCL_Compression_snappy_Uncompress_src_common_8
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_8
    // AOCL_Compression_snappy_RawUncompress_common_8
    cases.push_back(dest);

    // This decodes to about 2 MB; much smaller, but should still fail.
    dest[0] = dest[1] = dest[2] = '\xff';
    dest[3] = 0x00;

    // AOCL_Compression_snappy_IsValidCompressedBuffer_common_9
    // AOCL_Compression_snappy_IsValidCompressed_common_9
    // AOCL_Compression_snappy_Uncompress_common_9
    // AOCL_Compression_snappy_Uncompress_src_common_9
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_9
    // AOCL_Compression_snappy_RawUncompress_common_9
    cases.push_back(dest);

    vector<STR_WRAPPER> temp_vec;
    for(auto &element: cases)
    {
        STR_WRAPPER temp_str_wrapper;
        temp_str_wrapper.parameter = element;
        temp_vec.push_back(temp_str_wrapper);
    }
    return temp_vec;
}

// This function generates simple pass cases.
vector<STR_WRAPPER> pass_cases()
{
    vector<string> v;

    // AOCL_Compression_snappy_Compress_common_1
    // AOCL_Compression_snappy_Compress_src_common_1
    // AOCL_Compression_snappy_RawCompress_common_1
    // AOCL_Compression_snappy_Uncompress_common_10
    // AOCL_Compression_snappy_Uncompress_src_common_10
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_10
    // AOCL_Compression_snappy_RawUncompress_common_10
    // AOCL_Compression_snappy_RawUncompress_src_common_10
    v.push_back("");
    // AOCL_Compression_snappy_Compress_common_2
    // AOCL_Compression_snappy_Compress_src_common_2
    // AOCL_Compression_snappy_RawCompress_common_2
    // AOCL_Compression_snappy_Uncompress_common_11
    // AOCL_Compression_snappy_Uncompress_src_common_11
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_11
    // AOCL_Compression_snappy_RawUncompress_common_11
    // AOCL_Compression_snappy_RawUncompress_src_common_11
    v.push_back("a");
    // AOCL_Compression_snappy_Compress_common_3
    // AOCL_Compression_snappy_Compress_src_common_3
    // AOCL_Compression_snappy_RawCompress_common_3
    // AOCL_Compression_snappy_Uncompress_common_12
    // AOCL_Compression_snappy_Uncompress_src_common_12
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_12
    // AOCL_Compression_snappy_RawUncompress_common_12
    // AOCL_Compression_snappy_RawUncompress_src_common_12
    v.push_back("abc");
    // AOCL_Compression_snappy_Compress_common_4
    // AOCL_Compression_snappy_Compress_src_common_4
    // AOCL_Compression_snappy_RawCompress_common_4
    // AOCL_Compression_snappy_Uncompress_common_13
    // AOCL_Compression_snappy_Uncompress_src_common_13
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_13
    // AOCL_Compression_snappy_RawUncompress_common_13
    // AOCL_Compression_snappy_RawUncompress_src_common_13
    v.push_back("abcaaaaaaa" + std::string(65536, 'b') + std::string("aaaaa") + "abc");

    vector<STR_WRAPPER> temp_vec;
    for(auto &element: v)
    {
        STR_WRAPPER temp_str_wrapper;
        temp_str_wrapper.parameter = element;
        temp_vec.push_back(temp_str_wrapper);
    }
    return temp_vec;
}

// After decompression of the string `compressed` the length obtained
// is stored in data pointed by variable `length`, if length is succussfully
// found out this function will return true, if not it will return false.
bool checkUncompressedLength(string compressed, uint32_t *length)
{
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    bool return_value = snappy::GetUncompressedLength(src, length);
    delete src;
    return return_value;
}

// Append a string `literal` to an existing compressed string pointed
// by the pointer `dst` in snappy compressed format.
void AppendLiteral(std::string &dst, const std::string &literal)
{
    if (literal.empty())
        return;
    int n = literal.size() - 1;
    if (n < 60)
    {
        // Fit length in tag byte
        dst.push_back(0 | (n << 2));
    }
    else
    {
        // Encode in upcoming bytes
        char number[4];
        int count = 0;
        while (n > 0)
        {
            number[count++] = n & 0xff;
            n >>= 8;
        }
        dst.push_back(0 | ((59 + count) << 2));
        dst.append(std::string(number, count));
    }
    dst.append(literal);
}

// This function inserts data into a compressed string pointed
// by pointer `dst` such that, the data instructs the decompresser
// to copy data from `offset` position from the end of the current
// compressed string of length `length`. 
void AppendCopy(std::string &dst, unsigned offset, int length)
{
    while (length > 0)
    {
        // Figure out how much to copy in one shot
        int to_copy;
        if (length >= 68)
        {
            to_copy = 64;
        }
        else if (length > 64)
        {
            to_copy = 60;
        }
        else
        {
            to_copy = length;
        }
        length -= to_copy;

        if ((to_copy >= 4) && (to_copy < 12) && (offset < 2048))
        {
            assert(to_copy - 4 < 8); // Must fit in 3 bits
            dst.push_back(1 | ((to_copy - 4) << 2) | ((offset >> 8) << 5));
            dst.push_back(offset & 0xff);
        }
        else if (offset < 65536)
        {
            dst.push_back(2 | ((to_copy - 1) << 2));
            dst.push_back(offset & 0xff);
            dst.push_back(offset >> 8);
        }
        else
        {
            dst.push_back(3 | ((to_copy - 1) << 2));
            dst.push_back(offset & 0xff);
            dst.push_back((offset >> 8) & 0xff);
            dst.push_back((offset >> 16) & 0xff);
            dst.push_back((offset >> 24) & 0xff);
        }
    }
}

// Special case for decompression, compression provided by snappy can't generate
// four byte offset, so we artificially generate four byte offset by this function.
pair<string, string> fourByteOffset()
{
    // The new compressor cannot generate four-byte offsets since
    // it chops up the input into 32KB pieces.  So we hand-emit the
    // copy manually.

    // The two fragments that make up the input string.
    std::string fragment1 = "012345689abcdefghijklmnopqrstuvwxyz";
    std::string fragment2 = "some other string";

    // How many times each fragment is emitted.
    const int n1 = 2;
    const int n2 = 100000 / fragment2.size();
    const size_t length = n1 * fragment1.size() + n2 * fragment2.size();
    std::string compressed;

    SNAPPY_Gtest_Util::Append32(&compressed, length);
    AppendLiteral(compressed, fragment1);
    std::string src = fragment1;
    for (int i = 0; i < n2; ++i)
    {
        AppendLiteral(compressed, fragment2);
        src += fragment2;
    }
    AppendCopy(compressed, src.size(), fragment1.size());
    src += fragment1;

    return {compressed, src};
}

// Test case generating function for testing uncompress functions supported
// by snappy.
vector<PAIR_WRAPPER> uncompress_pass_cases()
{
    vector<STR_WRAPPER> inputs_to_compress = pass_cases();
    vector<pair<string, string>> cmpr_orig;

    for (auto &element : inputs_to_compress)
    {
        string compressed;
        compressed.clear();
        Compress(element.parameter.data(), element.parameter.size(), &compressed);
        cmpr_orig.push_back({compressed, element.parameter});
    }
    // The new compressor cannot generate four-byte offsets since
    // it chops up the input into 32KB pieces.  So we hand-emit the
    // copy manually.
    // AOCL_Compression_snappy_Uncompress_common_14
    // AOCL_Compression_snappy_Uncompress_src_common_14
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_14
    // AOCL_Compression_snappy_RawUncompress_common_14
    // AOCL_Compression_snappy_RawUncompress_src_common_14
    cmpr_orig.push_back(fourByteOffset());

    // Check that we do not read past end of input
    // Make a compressed string that ends with a single-byte literal
    std::string compressed;
    SNAPPY_Gtest_Util::Append32(&compressed, 1);
    AppendLiteral(compressed, "x");
    // AOCL_Compression_snappy_Uncompress_common_15
    // AOCL_Compression_snappy_Uncompress_src_common_15
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_15
    // AOCL_Compression_snappy_RawUncompress_common_15
    // AOCL_Compression_snappy_RawUncompress_src_common_15
    cmpr_orig.push_back({compressed, "x"});

    // Test for Copies 01, 10 and 11.
    compressed.clear();
    SNAPPY_Gtest_Util::Append32(&compressed, 65547); //total size to alloc for compressed data: 3+2+65540+2
    string original = "abc";
    AppendLiteral(compressed, original); //literal [abc]
    AppendCopy(compressed, 3, 2); //10 copy [abc][ab]
    original += "ab";
    const string next10 = "ababababab";
    // create offset > 65536 by appending more copies 
    for (int i = 65540; i > 0; i -= 10) {
        AppendCopy(compressed, 2, 10); //01 copies [abc][ab][ababababab]...[ababababab]
        original += next10;
    }
    AppendCopy(compressed, 65540, 2); //11 copy [abc][ab][ababababab]...[ababababab][ab]
    original += "ab";
    // AOCL_Compression_snappy_Uncompress_common_19
    // AOCL_Compression_snappy_Uncompress_src_common_19
    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_19
    // AOCL_Compression_snappy_RawUncompress_common_19
    // AOCL_Compression_snappy_RawUncompress_src_common_19
    cmpr_orig.push_back({ compressed, original });

    vector<PAIR_WRAPPER> temp_vec;
    for(auto &element: cmpr_orig)
    {
        PAIR_WRAPPER temp_pair_wrapper;
        temp_pair_wrapper.parameter = element;
        temp_vec.push_back(temp_pair_wrapper);
    }
    return temp_vec;
}

// Utility function for data allocation of iovec datatype, according to
// kLengths array.
void iov_init(iovec iov[],const int kLengths[],int kLengths_size)
{
    for (int i = 0; i < kLengths_size; ++i)
    {
        iov[i].iov_base = new char[kLengths[i]];
        iov[i].iov_len = kLengths[i];
    }
}

/*******************************************
 * "End" of Utility functions              *
 ********************************************/

/**************************************************
 * "Begin" of Test cases
 **************************************************/

/*********************************************
* Begin of MaxCompressedLength
**********************************************/

TEST(SNAPPY_MaxCompressedLength, all_tests)
{
    EXPECT_EQ(MaxCompressedLength(0), 32);                  // AOCL_Compression_snappy_MaxCompressedLength_common_1
#ifndef AOCL_ENABLE_THREADS
    EXPECT_EQ(MaxCompressedLength(393216), 458784);         // AOCL_Compression_snappy_MaxCompressedLength_common_2
    EXPECT_EQ(MaxCompressedLength(INT_MAX), 2505397620);    // AOCL_Compression_snappy_MaxCompressedLength_common_3
    size_t inp = LLONG_MAX / 2;
    EXPECT_EQ(MaxCompressedLength(inp),5380300354831952585);// AOCL_Compression_snappy_MaxCompressedLength_common_4
#endif
}

/*********************************************
* End of MaxCompressedLength
**********************************************/

/*********************************************
* Begin of snappy_max_compressed_length
**********************************************/

TEST(SNAPPY_snappy_max_compressed_length, all_tests)
{
    EXPECT_EQ(snappy_max_compressed_length(0), 32);                  // AOCL_Compression_snappy_snappy_max_compressed_length_common_1
#ifndef AOCL_ENABLE_THREADS
    EXPECT_EQ(snappy_max_compressed_length(393216), 458784);         // AOCL_Compression_snappy_snappy_max_compressed_length_common_2
    EXPECT_EQ(snappy_max_compressed_length(INT_MAX), 2505397620);    // AOCL_Compression_snappy_snappy_max_compressed_length_common_3
    size_t inp = LLONG_MAX / 2;
    EXPECT_EQ(snappy_max_compressed_length(inp), 5380300354831952585);// AOCL_Compression_snappy_snappy_max_compressed_length_common_4
#endif
}

/*********************************************
* End of snappy_max_compressed_length
**********************************************/

/*********************************************
* Begin of IsValidCompressed
**********************************************/

bool IsValidCompressed(string str)
{
    Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(str.data(), str.size());
    bool return_result = IsValidCompressed(source);
    delete source;
    return return_result;
}

class SNAPPY_IsValidCompressed_multiple_fail : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_IsValidCompressed_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_IsValidCompressed_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_IsValidCompressed_multiple_fail, AOCL_Compression_snappy_IsValidCompressed_fail_common_1_to_9)
{
    string compressed = get<1>(GetParam()).parameter;

    EXPECT_FALSE(IsValidCompressed(compressed));
}

TEST_P(SNAPPY_IsValidCompressed_single, AOCL_Compression_snappy_IsValidCompressed_fail_common_10)
{
    EXPECT_FALSE(IsValidCompressed(NULL));
}

TEST_P(SNAPPY_IsValidCompressed_single, AOCL_Compression_snappy_IsValidCompressed_common_pass_11)
{
    string s = "hello world";
    string compressed;

    Compress(s.data(), s.size(), &compressed);
    EXPECT_TRUE(IsValidCompressed(compressed));
}

TEST_P(SNAPPY_IsValidCompressed_multiple, AOCL_Compression_snappy_IsValidCompressed_pass_common_12)
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string compressed = p.first;

    EXPECT_TRUE(IsValidCompressed(compressed));
}

TEST_P(SNAPPY_IsValidCompressed_single, AOCL_Compression_snappy_IsValidCompressed_fail_common_13)
{
    pair<string, string> p = fourByteOffset();
    string compressed = p.first.substr(0, p.first.size() - 1);

    EXPECT_FALSE(IsValidCompressed(compressed));
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_IsValidCompressed_multiple_fail,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_IsValidCompressed_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_IsValidCompressed_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
* End of IsValidCompressed
**********************************************/

/*********************************************
* Begin of IsValidCompressedBuffer
**********************************************/

class SNAPPY_IsValidCompressedBuffer_multiple_fail : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_IsValidCompressedBuffer_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_IsValidCompressedBuffer_single: public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_IsValidCompressedBuffer_multiple_fail, AOCL_Compression_snappy_IsValidCompressedBuffer_fail_common_1_to_9)
{
    string compressed = get<1>(GetParam()).parameter;

    EXPECT_FALSE(IsValidCompressedBuffer(compressed.data(), compressed.size()));
}

TEST_P(SNAPPY_IsValidCompressedBuffer_single, AOCL_Compression_snappy_IsValidCompressedBuffer_fail_common_10)
{
    EXPECT_FALSE(IsValidCompressedBuffer(NULL, 100));
}

TEST_P(SNAPPY_IsValidCompressedBuffer_single, AOCL_Compression_snappy_IsValidCompressedBuffer_pass_common_11)
{
    string s = "hello world";
    string compressed;

    Compress(s.data(), s.size(), &compressed);

    EXPECT_TRUE(IsValidCompressedBuffer(compressed.data(), compressed.size()));
}

TEST_P(SNAPPY_IsValidCompressedBuffer_multiple, AOCL_Compression_snappy_IsValidCompressedBuffer_pass_common_12)
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string compressed = p.first;

    EXPECT_TRUE(IsValidCompressedBuffer(compressed.data(), compressed.size()));
}

TEST_P(SNAPPY_IsValidCompressedBuffer_multiple, AOCL_Compression_snappy_IsValidCompressedBuffer_fail_common_13)
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string compressed = p.first;

    // Partial compressed buffer provided
    EXPECT_FALSE(IsValidCompressedBuffer(compressed.data(), compressed.size()-1));
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_IsValidCompressedBuffer_multiple_fail,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_IsValidCompressedBuffer_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_IsValidCompressedBuffer_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
* End of IsValidCompressedBuffer
**********************************************/

/*********************************************
* Begin of snappy_validate_compressed_buffer
**********************************************/

class SNAPPY_snappy_validate_compressed_buffer_multiple_fail : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_snappy_validate_compressed_buffer_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_snappy_validate_compressed_buffer_single: public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_snappy_validate_compressed_buffer_multiple_fail, AOCL_Compression_snappy_snappy_validate_compressed_buffer_fail_common_1_to_9) // Fail cases
{
    string compressed = get<1>(GetParam()).parameter;

    EXPECT_EQ(snappy_validate_compressed_buffer(compressed.data(), compressed.size()), SNAPPY_INVALID_INPUT);
}

TEST_P(SNAPPY_snappy_validate_compressed_buffer_single, AOCL_Compression_snappy_snappy_validate_compressed_buffer_fail_common_10) // Buffer is NULL
{
    EXPECT_EQ(snappy_validate_compressed_buffer(NULL, 100), SNAPPY_INVALID_INPUT);
}


TEST_P(SNAPPY_snappy_validate_compressed_buffer_multiple, AOCL_Compression_snappy_snappy_validate_compressed_buffer_pass_common_11) // edge cases for successfull compression
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string compressed = p.first;

    EXPECT_EQ(snappy_validate_compressed_buffer(compressed.data(), compressed.size()), SNAPPY_OK);
}

TEST_P(SNAPPY_snappy_validate_compressed_buffer_multiple, AOCL_Compression_snappy_snappy_validate_compressed_buffer_fail_common_12)
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string compressed = p.first;

    // Partial compressed buffer provided
    EXPECT_EQ(snappy_validate_compressed_buffer(compressed.data(), compressed.size()-1), SNAPPY_INVALID_INPUT);
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_validate_compressed_buffer_multiple_fail,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_validate_compressed_buffer_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_validate_compressed_buffer_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************
* End of snappy_validate_compressed_buffer
**********************************************/

/*********************************************
* Begin of GetUncompressedLength
**********************************************/

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_fail_common_1)
{
    string s = "Hello world";
    string result;
    size_t ulength;

    Compress(s.data(), s.size(), &result);

    EXPECT_FALSE(GetUncompressedLength(NULL, result.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_fail_common_2) // compressed length = 0
{
    size_t ulength;
    string compressed;

    EXPECT_FALSE(GetUncompressedLength(compressed.data(), 0, &ulength));
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_fail_common_3)
{
    string s = "Hello world";
    string result;

    Compress(s.data(), s.size(), &result);

    EXPECT_FALSE(GetUncompressedLength(result.data(), result.size(), NULL));
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_fail_common_4) // TruncatedVarint
{
    size_t ulength;
    string compressed;
    compressed.push_back('\xf0');

    EXPECT_FALSE(GetUncompressedLength(compressed.data(), compressed.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_fail_common_5) // UnterminatedVarint
{
    string compressed;
    size_t ulength;
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back(10);

    EXPECT_FALSE(GetUncompressedLength(compressed.data(), compressed.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_fail_common_6) // Value too long edge case
{
    string compressed;
    size_t ulength;
    compressed.clear();
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x10');

    EXPECT_FALSE(GetUncompressedLength(compressed.data(), compressed.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_fail_common_7) // OverflowingVarint
{
    string compressed;
    size_t ulength;
    compressed.push_back('\xfb');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\x7f');

    for(int i=compressed.size();i>=0;i--)
    {
        EXPECT_FALSE(GetUncompressedLength(compressed.data(), i, &ulength));
    }
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_pass_common_8) // simple pass test case
{
    string s = "Hello world";
    string result;
    size_t uncompressed_length;

    Compress(s.data(), s.size(), &result);

    EXPECT_TRUE(GetUncompressedLength(result.data(), result.size(), &uncompressed_length));
    EXPECT_EQ(s.size(), uncompressed_length);
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_pass_common_9) // input of 0 uncompressed length
{
    string result;
    result.push_back('\x00');
    size_t uncompressed_length;
    EXPECT_TRUE(GetUncompressedLength(result.data(), result.size(), &uncompressed_length));
    EXPECT_EQ(uncompressed_length, 0);
}

TEST(SNAPPY_GetUncompressedLength, AOCL_Compression_snappy_GetUncompressedLength_pass_common_10) // input of max uncompressed length for each character
{
    string result;
    result.push_back('\x7f');
    size_t uncompressed_length;
    for(int i=0;i<4;i++)
    {
        EXPECT_TRUE(GetUncompressedLength(result.data(), result.size(), &uncompressed_length));
        EXPECT_EQ(uncompressed_length, (1<<(7*(i+1)))-1);
        result[i] = '\xff';
        result.push_back('\x7f');
    }
    result[4] = '\x0f';
    // Max supported length
    EXPECT_TRUE(GetUncompressedLength(result.data(), result.size(), &uncompressed_length));
    EXPECT_EQ(uncompressed_length, ((1ULL)<<32) - 1);
}

/*********************************************
* End of GetUncompressedLength
**********************************************/

/*********************************************
* Begin of snappy_uncompressed_length
**********************************************/

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_fail_common_1)
{
    string s = "Hello world";
    string result;
    size_t ulength;

    Compress(s.data(), s.size(), &result);

    EXPECT_EQ(snappy_uncompressed_length(NULL, result.size(), &ulength), SNAPPY_INVALID_INPUT);
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_fail_common_2)
{
    string s = "Hello world";
    string result;

    Compress(s.data(), s.size(), &result);

    EXPECT_EQ(snappy_uncompressed_length(result.data(), result.size(), NULL), SNAPPY_INVALID_INPUT);
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_fail_common_3) // TruncatedVarint
{
    size_t ulength;
    string compressed;
    compressed.push_back('\xf0');

    EXPECT_EQ(snappy_uncompressed_length(compressed.data(), compressed.size(), &ulength), SNAPPY_INVALID_INPUT);
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_fail_common_4) // UnterminatedVarint
{
    string compressed;
    size_t ulength;
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back(10);

    EXPECT_EQ(snappy_uncompressed_length(compressed.data(), compressed.size(), &ulength), SNAPPY_INVALID_INPUT);
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_fail_common_5) // Value too long edge case
{
    string compressed;
    size_t ulength;
    compressed.clear();
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x10');

    EXPECT_EQ(snappy_uncompressed_length(compressed.data(), compressed.size(), &ulength), SNAPPY_INVALID_INPUT);
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_fail_common_6) // OverflowingVarint
{
    string compressed;
    size_t ulength;
    compressed.push_back('\xfb');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\x7f');

    for(int i=compressed.size();i>=0;i--)
    {
        EXPECT_EQ(snappy_uncompressed_length(compressed.data(), i, &ulength), SNAPPY_INVALID_INPUT);
    }
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_pass_common_7) // simple pass test case
{
    string s = "Hello world";
    string result;
    size_t uncompressed_length;

    Compress(s.data(), s.size(), &result);

    EXPECT_EQ(snappy_uncompressed_length(result.data(), result.size(), &uncompressed_length), SNAPPY_OK);
    EXPECT_EQ(s.size(), uncompressed_length);
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_pass_common_8) // input of 0 uncompressed length
{
    string result;
    result.push_back('\x00');
    size_t uncompressed_length;
    EXPECT_EQ(snappy_uncompressed_length(result.data(), result.size(), &uncompressed_length), SNAPPY_OK);
    EXPECT_EQ(uncompressed_length, 0);
}

TEST(SNAPPY_snappy_uncompressed_length, AOCL_Compression_snappy_snappy_uncompressed_length_pass_common_9) // input of max uncompressed length for each character
{
    string result;
    result.push_back('\x7f');
    size_t uncompressed_length;
    for(int i=0;i<4;i++)
    {
        EXPECT_EQ(snappy_uncompressed_length(result.data(), result.size(), &uncompressed_length), SNAPPY_OK);
        EXPECT_EQ(uncompressed_length, (1<<(7*(i+1)))-1);
        result[i] = '\xff';
        result.push_back('\x7f');
    }
    result[4] = '\x0f';
    // Max supported length
    EXPECT_EQ(snappy_uncompressed_length(result.data(), result.size(), &uncompressed_length), SNAPPY_OK);
    EXPECT_EQ(uncompressed_length, ((1ULL)<<32) - 1);
}

/*********************************************
* End of snappy_uncompressed_length
**********************************************/

#ifdef AOCL_ENABLE_THREADS
/********************************************************
* Begin of GetUncompressedLengthFromMTCompressedBuffer
*********************************************************/

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_1)
{
    string s = "Hello world";
    string result;
    size_t ulength;

    Compress(s.data(), s.size(), &result);

    EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(NULL, result.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_2)
{
    string s = "Hello world";
    string result;

    Compress(s.data(), s.size(), &result);

    EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(result.data(), result.size(), NULL));
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_3) // TruncatedVarint
{
    size_t ulength;
    string compressed;
    compressed.push_back('\xf0');

    EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(compressed.data(), compressed.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_4) // UnterminatedVarint
{
    string compressed;
    size_t ulength;
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back(10);

    EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(compressed.data(), compressed.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_5) // Value too long edge case
{
    string compressed;
    size_t ulength;
    compressed.clear();
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x10');

    EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(compressed.data(), compressed.size(), &ulength));
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_6) // OverflowingVarint
{
    string compressed;
    size_t ulength;
    compressed.push_back('\xfb');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\x7f');

    for(int i=compressed.size();i>=0;i--)
    {
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(compressed.data(), i, &ulength));
    }
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_pass_common_7) // simple pass test case
{
    string s = "Hello world";
    string result;
    size_t uncompressed_length;

    Compress(s.data(), s.size(), &result);

    EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(result.data(), result.size(), &uncompressed_length));
    EXPECT_EQ(s.size(), uncompressed_length);
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_pass_common_8) // input of 0 uncompressed length
{
    string result;
    result.push_back('\x00');
    size_t uncompressed_length;
    EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(result.data(), result.size(), &uncompressed_length));
    EXPECT_EQ(uncompressed_length, 0);
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_pass_common_9) // input of max uncompressed length for each character
{
    string result;
    result.push_back('\x7f');
    size_t uncompressed_length;
    for(int i=0;i<4;i++)
    {
        EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(result.data(), result.size(), &uncompressed_length));
        EXPECT_EQ(uncompressed_length, (1<<(7*(i+1)))-1);
        result[i] = '\xff';
        result.push_back('\x7f');
    }
    result[4] = '\x0f';
    // Max supported length
    EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(result.data(), result.size(), &uncompressed_length));
    EXPECT_EQ(uncompressed_length, ((1ULL)<<32) - 1);
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_pass_common_10) // Huge input size is provided, to ensure multi-thread code path
{
    size_t original_size = 1e6;
    string orig(original_size, 'a');
    vector<char> compressed(MaxCompressedLength(original_size));
    size_t compressed_length;
    size_t uncompressed_length;
    for(int i=0; i < original_size ; i++)
    {
        orig[i]= rand()%256;
    }

    RawCompress(orig.data(), orig.size(), compressed.data(), &compressed_length);
    EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(compressed.data(), compressed_length, &uncompressed_length));
    EXPECT_EQ(original_size, uncompressed_length);
}

TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_11) // compressed buffer corrupted
{
    size_t original_size = 1e6;
    string orig(original_size, 'a');
    vector<char> compressed(MaxCompressedLength(original_size));
    size_t compressed_length;
    size_t uncompressed_length;
    for(int i=0; i < original_size ; i++)
    {
        orig[i] = rand()%256;
    }

    RawCompress(orig.data(), orig.size(), compressed.data(), &compressed_length);
    // Compressed data being corrupted
    compressed[0]--;
    (GetUncompressedLengthFromMTCompressedBuffer(compressed.data(), compressed.size(), &uncompressed_length));
    EXPECT_NE(original_size, uncompressed_length);
}

// Crafted stream: a valid RAP magic word but an inconsistent RAP header
// (rap_metadata_len > buffer size) makes aocl_setup_parallel_decompress_mt
// return a negative error code. GetUncompressedLengthFromMTCompressedBuffer must
// reject it (return false) and must NOT compute start + offset / n - offset with
// a negative offset, which would read out of bounds (start[-1]).
TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_common_12) // negative setup offset (invalid RAP header)
{
    // 8-byte magic "AOCL_LLC" (little-endian 0x434C4C5F4C434F41), then a
    // rap_metadata_len far larger than the buffer, then a thread count.
    vector<char> buf(32, '\0');
    const unsigned char magic[8] = {0x41,0x4F,0x43,0x4C,0x5F,0x4C,0x4C,0x43};
    for (int i = 0; i < 8; ++i) buf[i] = (char)magic[i];
    // rap_metadata_len = 0xFFFFFFFF (> buf.size()) at offset 8 -> header invalid
    buf[8] = (char)0xFF; buf[9] = (char)0xFF; buf[10] = (char)0xFF; buf[11] = (char)0xFF;
    // num_main_threads = 2 at offset 12
    buf[12] = 2; buf[13] = 0; buf[14] = 0; buf[15] = 0;

    size_t ulength = 0;
    // Must return false without any out-of-bounds access / crash.
    EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(buf.data(), buf.size(), &ulength));
}

// Helper: produce a multithreaded (RAP framed) stream. Returns false if the
// compressor did not emit a RAP frame (e.g. single thread available).
static bool Snappy_MakeRapStream(size_t original_size, vector<char>& compressed,
                                 size_t& compressed_length, uint32_t& rap_frame_len)
{
    string orig(original_size, '\0');
    for (size_t i = 0; i < original_size; ++i) orig[i] = (char)(rand() % 256);
    compressed.assign(MaxCompressedLength(original_size), 0);
    compressed_length = 0;
    RawCompress(orig.data(), orig.size(), compressed.data(), &compressed_length);
    if (compressed_length < RAP_START_OF_PARTITIONS ||
        memcmp(compressed.data(), "AOCL_LLC", RAP_MAGIC_WORD_BYTES) != 0)
        return false;
    memcpy(&rap_frame_len, compressed.data() + RAP_MAGIC_WORD_BYTES, sizeof(rap_frame_len));
    compressed.resize(compressed_length);
    return true;
}

// Crafted RAP partition tables must be rejected.
TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_crafted_partitions_13)
{
    vector<char> compressed;
    size_t compressed_length;
    uint32_t rap_frame_len;
    if (!Snappy_MakeRapStream(1e6, compressed, compressed_length, rap_frame_len))
        GTEST_SKIP() << "Compressor did not emit a RAP frame (single thread)";

    size_t ulength = 0;
    ASSERT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(compressed.data(), compressed_length, &ulength));
    ASSERT_EQ(ulength, (size_t)1e6);

    const size_t entry0 = RAP_START_OF_PARTITIONS;
    const size_t off_pos = entry0;
    const size_t len_pos = entry0 + RAP_OFFSET_BYTES;
    const size_t dec_pos = entry0 + RAP_OFFSET_BYTES + RAP_LEN_BYTES;
    auto put32 = [](vector<char>& b, size_t pos, uint32_t v) { memcpy(b.data() + pos, &v, sizeof(v)); };
    auto get32 = [](const vector<char>& b, size_t pos) { uint32_t v; memcpy(&v, b.data() + pos, sizeof(v)); return v; };

    { // partition decompressed lengths do not add up to the outer length
        vector<char> c = compressed;
        put32(c, dec_pos, get32(c, dec_pos) + 1);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // negative partition decompressed length
        vector<char> c = compressed;
        put32(c, dec_pos, 0x80000000u);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // partition offset beyond the buffer
        vector<char> c = compressed;
        put32(c, off_pos, (uint32_t)c.size() + 1);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // partition offset inside the RAP frame
        vector<char> c = compressed;
        put32(c, off_pos, 0);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // partition length runs past the buffer
        vector<char> c = compressed;
        put32(c, len_pos, (uint32_t)c.size());
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // negative partition length
        vector<char> c = compressed;
        put32(c, len_pos, 0xFFFFFFFFu);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // empty partition declaring nonzero decompressed output (sum unchanged)
        vector<char> c = compressed;
        ASSERT_NE(get32(c, dec_pos), 0u);
        put32(c, len_pos, 0);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
        // The C API must not report success with part of the output unwritten.
        vector<char> out((size_t)1e6);
        size_t out_len = out.size();
        EXPECT_NE(snappy_uncompress(c.data(), c.size(), out.data(), &out_len), SNAPPY_OK);
    }
    { // overlapping partitions: partition 1 starts where partition 0 starts
        vector<char> c = compressed;
        put32(c, off_pos + RAP_DATA_BYTES_WITH_DECOMP_LEN, get32(c, off_pos));
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // out-of-order partitions: offsets of partitions 0 and 1 swapped
        vector<char> c = compressed;
        uint32_t off0 = get32(c, off_pos);
        uint32_t off1 = get32(c, off_pos + RAP_DATA_BYTES_WITH_DECOMP_LEN);
        put32(c, off_pos, off1);
        put32(c, off_pos + RAP_DATA_BYTES_WITH_DECOMP_LEN, off0);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
    { // gap: first partition does not start right after the outer varint
        vector<char> c = compressed;
        put32(c, off_pos, get32(c, off_pos) + 1);
        put32(c, len_pos, get32(c, len_pos) - 1);
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
    }
}

// snappy_validate_compressed_buffer accepts RAP framed streams and agrees with
// snappy_uncompress on valid and tampered streams.
TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_validate_compressed_buffer_mt_matches_uncompress_16)
{
    vector<char> compressed;
    size_t compressed_length;
    uint32_t rap_frame_len;
    if (!Snappy_MakeRapStream(1e6, compressed, compressed_length, rap_frame_len))
        GTEST_SKIP() << "Compressor did not emit a RAP frame (single thread)";

    auto verdicts_match = [](vector<char>& c, bool expect_valid) {
        vector<char> out((size_t)1e6);
        size_t out_len = out.size();
        bool decoded = (snappy_uncompress(c.data(), c.size(), out.data(), &out_len) == SNAPPY_OK);
        bool valid = (snappy_validate_compressed_buffer(c.data(), c.size()) == SNAPPY_OK);
        EXPECT_EQ(valid, decoded);
        EXPECT_EQ(valid, expect_valid);
    };

    verdicts_match(compressed, true);

    const size_t entry1 = RAP_START_OF_PARTITIONS + RAP_DATA_BYTES_WITH_DECOMP_LEN;
    { // corrupt a tag byte inside partition 1
        vector<char> c = compressed;
        uint32_t off1;
        memcpy(&off1, c.data() + entry1, sizeof(off1));
        c[off1] = (char)0xFF;
        verdicts_match(c, false);
    }
    { // per-partition lengths wrong but their sum still matches the outer length:
      // partition 0 declares one byte less, partition 1 one byte more
        vector<char> c = compressed;
        const size_t dec0_pos = RAP_START_OF_PARTITIONS + RAP_OFFSET_BYTES + RAP_LEN_BYTES;
        const size_t dec1_pos = entry1 + RAP_OFFSET_BYTES + RAP_LEN_BYTES;
        uint32_t dec0, dec1;
        memcpy(&dec0, c.data() + dec0_pos, sizeof(dec0));
        memcpy(&dec1, c.data() + dec1_pos, sizeof(dec1));
        dec0 -= 1;
        dec1 += 1;
        memcpy(c.data() + dec0_pos, &dec0, sizeof(dec0));
        memcpy(c.data() + dec1_pos, &dec1, sizeof(dec1));
        size_t ulength = 0;
        EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
        verdicts_match(c, false);
    }
    { // invalid RAP header
        vector<char> c = compressed;
        uint32_t bad = (uint32_t)c.size() + 1;
        memcpy(c.data() + RAP_MAGIC_WORD_BYTES, &bad, sizeof(bad));
        verdicts_match(c, false);
    }
    { // trailing data after the last partition (an incomplete literal tag)
        vector<char> c = compressed;
        c.push_back((char)0x04);
        size_t ulength = 0;
        EXPECT_FALSE(GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength));
        verdicts_match(c, false);
    }
}

// Hand-built RAP stream: two partitions, each a one-byte literal ('A', 'B'),
// declaring the given decompressed lengths; outer length = sum of declared.
static vector<char> Snappy_MakeTwoLiteralRapStream(uint32_t decl0, uint32_t decl1)
{
    const uint32_t frame_len = RAP_START_OF_PARTITIONS + 2 * RAP_DATA_BYTES_WITH_DECOMP_LEN;
    vector<char> s(frame_len, 0);
    memcpy(s.data(), "AOCL_LLC", RAP_MAGIC_WORD_BYTES);
    memcpy(s.data() + RAP_MAGIC_WORD_BYTES, &frame_len, sizeof(frame_len));
    const uint32_t two = 2;
    memcpy(s.data() + RAP_MAGIC_WORD_BYTES + RAP_METADATA_LEN_BYTES, &two, sizeof(two));
    s.push_back((char)(decl0 + decl1)); // outer length varint (< 128)
    const uint32_t part_len = 2;        // literal tag (length 1) + one byte
    const uint32_t decl[2] = { decl0, decl1 };
    for (uint32_t i = 0; i < 2; ++i) {
        uint32_t off = (uint32_t)s.size() + i * part_len;
        char* e = s.data() + RAP_START_OF_PARTITIONS + i * RAP_DATA_BYTES_WITH_DECOMP_LEN;
        memcpy(e, &off, sizeof(off));
        memcpy(e + RAP_OFFSET_BYTES, &part_len, sizeof(part_len));
        memcpy(e + RAP_OFFSET_BYTES + RAP_LEN_BYTES, &decl[i], sizeof(decl[i]));
    }
    s.push_back((char)0x00); s.push_back('A');
    s.push_back((char)0x00); s.push_back('B');
    return s;
}

// Validation and decompression agree regardless of the number of workers: the
// one-worker path decodes each partition with its declared length.
TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_rap_one_worker_parity_17)
{
    for (int one_worker = 0; one_worker <= 1; ++one_worker) {
        if (one_worker) test_omp_max_threads_set(1);
        { // declared lengths [1, 1]: valid, output "AB"
            vector<char> s = Snappy_MakeTwoLiteralRapStream(1, 1);
            EXPECT_EQ(snappy_validate_compressed_buffer(s.data(), s.size()), SNAPPY_OK);
            char out[2] = { 0, 0 };
            size_t out_len = sizeof(out);
            EXPECT_EQ(snappy_uncompress(s.data(), s.size(), out, &out_len), SNAPPY_OK)
                << "one_worker=" << one_worker;
            EXPECT_EQ(out_len, 2u);
            EXPECT_EQ(0, memcmp(out, "AB", 2));
        }
        { // declared lengths [0, 2]: sum matches, but per-partition lengths are wrong
            vector<char> s = Snappy_MakeTwoLiteralRapStream(0, 2);
            size_t ulength = 0;
            EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(s.data(), s.size(), &ulength));
            EXPECT_EQ(snappy_validate_compressed_buffer(s.data(), s.size()), SNAPPY_INVALID_INPUT);
            char out[2] = { 0, 0 };
            size_t out_len = sizeof(out);
            EXPECT_NE(snappy_uncompress(s.data(), s.size(), out, &out_len), SNAPPY_OK)
                << "one_worker=" << one_worker;
        }
        if (one_worker) test_omp_max_threads_reset();
    }
}

// Truncation at every interesting boundary must be rejected without OOB reads.
TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_fail_truncated_14)
{
    vector<char> compressed;
    size_t compressed_length;
    uint32_t rap_frame_len;
    if (!Snappy_MakeRapStream(1e6, compressed, compressed_length, rap_frame_len))
        GTEST_SKIP() << "Compressor did not emit a RAP frame (single thread)";

    size_t ulength = 0;
    const size_t cuts[] = { RAP_MAGIC_WORD_BYTES + 2, RAP_START_OF_PARTITIONS,
                            (size_t)rap_frame_len - 1, (size_t)rap_frame_len,
                            (size_t)rap_frame_len + 1, compressed_length - 1 };
    for (size_t cut : cuts) {
        // Exact-size heap copy so ASan reports any read past the end.
        vector<char> c(compressed.begin(), compressed.begin() + cut);
        bool ok = GetUncompressedLengthFromMTCompressedBuffer(c.data(), c.size(), &ulength);
        if (cut >= RAP_START_OF_PARTITIONS)
            EXPECT_FALSE(ok) << "cut=" << cut;
    }
}

// Unaligned buffers are handled (header fields are not read through casted pointers).
TEST(SNAPPY_GetUncompressedLengthFromMTCompressedBuffer, AOCL_Compression_snappy_GetUncompressedLengthFromMTCompressedBuffer_pass_unaligned_15)
{
    vector<char> compressed;
    size_t compressed_length;
    uint32_t rap_frame_len;
    if (!Snappy_MakeRapStream(1e6, compressed, compressed_length, rap_frame_len))
        GTEST_SKIP() << "Compressor did not emit a RAP frame (single thread)";

    vector<char> shifted(compressed_length + 1);
    memcpy(shifted.data() + 1, compressed.data(), compressed_length);
    size_t ulength = 0;
    EXPECT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(shifted.data() + 1, compressed_length, &ulength));
    EXPECT_EQ(ulength, (size_t)1e6);
}

/****************************************************
* End of GetUncompressedLengthFromMTCompressedBuffer
*****************************************************/

// A valid multithreaded (RAP) stream with a non-zero length must be rejected
// when the output buffer is NULL (parity with the single-threaded path).
// Multithreaded decompression is available only through the C API.
TEST(SNAPPY_RawUncompress_MT, AOCL_Compression_snappy_RawUncompress_mt_null_output_rejected)
{
    // 1e6 bytes reliably takes the multithreaded RAP path.
    size_t original_size = 1e6;
    string orig(original_size, '\0');
    for (size_t i = 0; i < original_size; ++i) orig[i] = (char)(rand() % 256);

    vector<char> compressed(MaxCompressedLength(original_size));
    size_t compressed_length = 0;
    RawCompress(orig.data(), orig.size(), compressed.data(), &compressed_length);

    // NULL output buffer for a non-empty stream: must return false, not crash.
    EXPECT_FALSE(RawUncompress(compressed.data(), compressed_length, (char*)NULL));
    size_t uncompressed_length = original_size;
    EXPECT_NE(snappy_uncompress(compressed.data(), compressed_length, (char*)NULL,
                                &uncompressed_length), SNAPPY_OK);
}

// C API decompresses a multithreaded (RAP framed) stream produced by the
// multithreaded C++ compressor.
TEST(SNAPPY_RawUncompress_MT, AOCL_Compression_snappy_snappy_uncompress_mt_pass)
{
    size_t original_size = 1e6;
    string orig(original_size, '\0');
    for (size_t i = 0; i < original_size; ++i) orig[i] = (char)(rand() % 256);

    vector<char> compressed(MaxCompressedLength(original_size));
    size_t compressed_length = 0;
    RawCompress(orig.data(), orig.size(), compressed.data(), &compressed_length);

    size_t uncompressed_length = 0;
    ASSERT_EQ(snappy_uncompressed_length(compressed.data(), compressed_length,
                                         &uncompressed_length), SNAPPY_OK);
    ASSERT_EQ(uncompressed_length, original_size);

    vector<char> uncompressed(uncompressed_length);
    EXPECT_EQ(snappy_uncompress(compressed.data(), compressed_length,
                                uncompressed.data(), &uncompressed_length), SNAPPY_OK);
    EXPECT_EQ(uncompressed_length, original_size);
    EXPECT_EQ(0, memcmp(uncompressed.data(), orig.data(), original_size));

    // Output one byte short of the uncompressed length is rejected.
    size_t short_length = original_size - 1;
    vector<char> short_out(short_length);
    EXPECT_EQ(snappy_uncompress(compressed.data(), compressed_length,
                                short_out.data(), &short_length), SNAPPY_BUFFER_TOO_SMALL);
}

// C++ uncompress APIs are single-threaded: they skip the RAP frame of a
// multithreaded stream and decode the remaining Snappy stream in one thread.
TEST(SNAPPY_RawUncompress_MT, AOCL_Compression_snappy_RawUncompress_skip_rap_frame_st_pass)
{
    size_t original_size = 1e6;
    string orig(original_size, '\0');
    for (size_t i = 0; i < original_size; ++i) orig[i] = (char)(rand() % 256);

    vector<char> compressed(MaxCompressedLength(original_size));
    size_t compressed_length = 0;
    RawCompress(orig.data(), orig.size(), compressed.data(), &compressed_length);

    // Ensure the stream actually carries a RAP frame ("AOCL_LLC" magic word).
    if (compressed_length < 8 || memcmp(compressed.data(), "AOCL_LLC", 8) != 0)
        GTEST_SKIP() << "Compressor did not emit a RAP frame (single thread)";

    size_t uncompressed_length = 0;
    ASSERT_TRUE(GetUncompressedLengthFromMTCompressedBuffer(compressed.data(),
                compressed_length, &uncompressed_length));
    ASSERT_EQ(uncompressed_length, original_size);
    // Plain varint parse at offset 0 does not see the real length.
    size_t st_length = 0;
    EXPECT_FALSE(GetUncompressedLength(compressed.data(), compressed_length, &st_length) &&
                 st_length == original_size);

    // RawUncompress(const char*, ...)
    vector<char> uncompressed(original_size);
    EXPECT_TRUE(RawUncompress(compressed.data(), compressed_length, uncompressed.data()));
    EXPECT_EQ(0, memcmp(uncompressed.data(), orig.data(), original_size));

    // RawUncompressToIOVec(const char*, ...)
    vector<char> uncompressed_iov(original_size);
    struct iovec iov[2];
    iov[0].iov_base = uncompressed_iov.data();
    iov[0].iov_len = original_size / 2;
    iov[1].iov_base = uncompressed_iov.data() + original_size / 2;
    iov[1].iov_len = original_size - original_size / 2;
    EXPECT_TRUE(RawUncompressToIOVec(compressed.data(), compressed_length, iov, 2));
    EXPECT_EQ(0, memcmp(uncompressed_iov.data(), orig.data(), original_size));

    // Uncompress(const char*, size_t, std::string*)
    string uncompressed_str;
    EXPECT_TRUE(Uncompress(compressed.data(), compressed_length, &uncompressed_str));
    EXPECT_EQ(uncompressed_str, orig);
}

// A RAP magic word with an invalid RAP header must be rejected by the C++
// uncompress APIs without out-of-bounds access.
TEST(SNAPPY_RawUncompress_MT, AOCL_Compression_snappy_RawUncompress_invalid_rap_frame_fail)
{
    vector<char> buf(32, '\0');
    const unsigned char magic[8] = {0x41,0x4F,0x43,0x4C,0x5F,0x4C,0x4C,0x43};
    for (int i = 0; i < 8; ++i) buf[i] = (char)magic[i];
    // rap_metadata_len = 0xFFFFFFFF (> buf.size()) -> header invalid
    buf[8] = (char)0xFF; buf[9] = (char)0xFF; buf[10] = (char)0xFF; buf[11] = (char)0xFF;
    buf[12] = 2;

    vector<char> out(1024);
    EXPECT_FALSE(RawUncompress(buf.data(), buf.size(), out.data()));
    string out_str;
    EXPECT_FALSE(Uncompress(buf.data(), buf.size(), &out_str));
}

// A multithreaded compression setup failure is reported explicitly through the
// C API (error status) and the C++ API (zero compressed length).
TEST(SNAPPY_RawUncompress_MT, AOCL_Compression_snappy_compress_mt_setup_failure_reported)
{
    string orig(4096, 'a');
    vector<char> compressed(snappy_max_compressed_length(orig.size()));

    test_setup_parallel_compress_mt_fail_set(1);

    size_t c_len = compressed.size();
    EXPECT_EQ(snappy_compress(orig.data(), orig.size(), compressed.data(), &c_len),
              SNAPPY_INVALID_INPUT);

    size_t cpp_len = 12345;
    RawCompress(orig.data(), orig.size(), compressed.data(), &cpp_len);
    EXPECT_EQ(cpp_len, 0u);

    test_setup_parallel_compress_mt_fail_set(0);

    c_len = compressed.size();
    EXPECT_EQ(snappy_compress(orig.data(), orig.size(), compressed.data(), &c_len), SNAPPY_OK);
    EXPECT_GT(c_len, 0u);
}

// Two consecutive header-valid RAP frames ahead of a Snappy stream: only one
// frame is skipped. The second frame's first byte ('A' = 65) is then the
// length varint, so the output is sized for 65 bytes; the larger stream behind
// the second frame must not be decoded into it.
TEST(SNAPPY_RawUncompress_MT, AOCL_Compression_snappy_RawUncompress_nested_rap_frame_fail)
{
    const size_t inner_size = 4096;
    string inner_src(inner_size, 'a');
    string inner;
    Compress(inner_src.data(), inner_src.size(), &inner);

    // One-partition RAP frame header: magic, frame length, thread count, one entry.
    const uint32_t frame_len = RAP_START_OF_PARTITIONS + RAP_DATA_BYTES_WITH_DECOMP_LEN;
    vector<char> frame(frame_len, 0);
    memcpy(frame.data(), "AOCL_LLC", RAP_MAGIC_WORD_BYTES);
    memcpy(frame.data() + RAP_MAGIC_WORD_BYTES, &frame_len, sizeof(frame_len));
    const uint32_t one = 1;
    memcpy(frame.data() + RAP_MAGIC_WORD_BYTES + RAP_METADATA_LEN_BYTES, &one, sizeof(one));

    vector<char> buf;
    buf.insert(buf.end(), frame.begin(), frame.end());
    buf.insert(buf.end(), frame.begin(), frame.end());
    buf.insert(buf.end(), inner.begin(), inner.end());

    // Uncompress(std::string*) sizes the string from the stripped stream.
    string out_str;
    EXPECT_FALSE(Uncompress(buf.data(), buf.size(), &out_str));

    // RawUncompress into an exact 65-byte heap buffer (ASan catches overflow).
    vector<char> out_small(65);
    EXPECT_FALSE(RawUncompress(buf.data(), buf.size(), out_small.data()));
}
#endif

/*************************************************************************
* Begin of `bool GetUncompressedLength(Source* source, uint32_t* result)`
**************************************************************************/
class SNAPPY_GetUncompressedLength_src_single :  public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_fail_common_1)
{
    uint32_t ulength;

    EXPECT_FALSE(GetUncompressedLength(NULL, &ulength));
}

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_fail_common_2)
{
    string s = "Hello world";
    string compressed;
    Compress(s.data(), s.size(), &compressed);
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(GetUncompressedLength(src, NULL));
    delete src;
}

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_fail_common_3)    // TruncatedVarint
{
    uint32_t ulength;
    string compressed;
    compressed.push_back('\xf0');

    EXPECT_FALSE(checkUncompressedLength(compressed, &ulength));
}

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_fail_common_4)    // UnterminatedVarint
{
    uint32_t ulength;
    string compressed;

    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back(10);

    EXPECT_FALSE(checkUncompressedLength(compressed, &ulength));
}

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_fail_common_5)    // Value too long edge case
{
    uint32_t ulength;
    string compressed;
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x10');

    EXPECT_FALSE(checkUncompressedLength(compressed, &ulength));
}

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_fail_common_6)    // OverflowingVarint
{
    uint32_t ulength;
    string compressed;

    compressed.push_back('\xfb');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\x7f');

    for(int i=compressed.size(); i>=0; i--)
    {   
        {
            Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), i);
            bool return_value = snappy::GetUncompressedLength(src, &ulength);
            EXPECT_FALSE(return_value);
            delete src;
        }
    }
}

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_pass_common_7)    // Simple pass case
{
    string s = "Hello world";
    string result;
    uint32_t ulength;

    Compress(s.data(), s.size(), &result);

    EXPECT_TRUE(checkUncompressedLength(result, &ulength));
    EXPECT_EQ(s.size(), ulength);
}

TEST_P(SNAPPY_GetUncompressedLength_src_single, AOCL_Compression_snappy_GetUncompressedLength_src_pass_common_8)    // Highest permissable length
{
    string result;
    uint32_t ulength;
    result.push_back('\xff');
    result.push_back('\xff');
    result.push_back('\xff');
    result.push_back('\xff');
    result.push_back('\x0f');

    EXPECT_TRUE(checkUncompressedLength(result, &ulength));
    EXPECT_EQ(ulength, ((1ULL)<<32) - 1);
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_GetUncompressedLength_src_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*************************************************************************
* End of `bool GetUncompressedLength(Source* source, uint32_t* result)`
**************************************************************************/

/*********************************************
* Begin of RawUncompressToIOVec
**********************************************/

class SNAPPY_RawUncompressToIOVec_multiple :  public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{
    public:

    ~SNAPPY_RawUncompressToIOVec_multiple()
    {
        for (int i = 0; i < iov.size(); ++i)
        {
            delete[] reinterpret_cast<char *>(iov[i].iov_base);
        }
    }

    void init(int original_length, int iov_number_elements, vector<int> &kLengths, vector<iovec> &iov)
    {
        iov = vector<iovec>(iov_number_elements);
        kLengths = vector<int>(iov_number_elements);
        size_t remaining_sum = original_length;
        for(int i=0; i<iov_number_elements-1; i++)
        {
            int max_limit = remaining_sum - (iov_number_elements-i-1);
            kLengths[i] = 1 + rand()%max_limit;
            remaining_sum -= kLengths[i];
        }
        kLengths[iov_number_elements-1] = remaining_sum;
        iov_init(iov.data(), kLengths.data(), iov_number_elements);
    }

    vector<iovec> iov;
};

class SNAPPY_RawUncompressToIOVec_multiple_fail_cases :  public Combined_Dynamic_dispatcher<STR_WRAPPER>
{
    public:

    ~SNAPPY_RawUncompressToIOVec_multiple_fail_cases()
    {
        for (int i = 0; i < iov.size(); ++i)
        {
            delete[] reinterpret_cast<char *>(iov[i].iov_base);
        }
    }

    vector<iovec> iov;
};

class SNAPPY_RawUncompressToIOVec_single :  public Standalone_Dynamic_dispatcher
{
    public:

    ~SNAPPY_RawUncompressToIOVec_single()
    {
        for (int i = 0; i < iov.size(); ++i)
        {
            delete[] reinterpret_cast<char *>(iov[i].iov_base);
        }
    }

    void init(int original_length, int iov_number_elements, vector<int> &kLengths, vector<iovec> &iov)
    {
        iov = vector<iovec>(iov_number_elements);
        kLengths = vector<int>(iov_number_elements);
        size_t remaining_sum = original_length;
        for(int i=0; i<iov_number_elements-1; i++)
        {
            int max_limit = remaining_sum - (iov_number_elements-i-1);
            kLengths[i] = 1 + rand()%max_limit;
            remaining_sum -= kLengths[i];
        }
        kLengths[iov_number_elements-1] = remaining_sum;
        iov_init(iov.data(), kLengths.data(), iov_number_elements);
    }

    vector<iovec> iov;
};

TEST_P(SNAPPY_RawUncompressToIOVec_multiple, AOCL_Compression_snappy_RawUncompressToIOVec_pass_common_1) // pass cases
{
    pair<string,string> c_u_pair = get<1>(GetParam()).parameter;

    size_t original_length = c_u_pair.second.size();
    if(original_length == 0)
    {
        return;
    }

    int iov_number_elements = 5;
    if(original_length < 5)
    {
        iov_number_elements = original_length;
    }

    vector<int> kLengths;
    init(original_length, iov_number_elements, kLengths, iov);

    EXPECT_TRUE(snappy::RawUncompressToIOVec(c_u_pair.first.data(), c_u_pair.first.size(), iov.data(), iov_number_elements));
    
    int compare_from_index = 0;
    for(int i=0;i<iov_number_elements;i++)
    {
        EXPECT_EQ(0, memcmp(iov[i].iov_base, &c_u_pair.second[compare_from_index], kLengths[i]));
        compare_from_index += kLengths[i];
    }
}


TEST_P(SNAPPY_RawUncompressToIOVec_single, AOCL_Compression_snappy_RawUncompressToIOVec_fail_common_2) // Insufficeint uncompress memory
{
    pair<string,string> c_u_pair = fourByteOffset();

    size_t original_length = c_u_pair.second.size();
    if(original_length == 0)
    {
        return;
    }

    int iov_number_elements = 5;
    if(original_length < 5)
    {
        iov_number_elements = original_length;
    }

    vector<int> kLengths;
    init(original_length, iov_number_elements, kLengths, iov);

    EXPECT_FALSE(snappy::RawUncompressToIOVec(c_u_pair.first.data(), c_u_pair.first.size(), iov.data(), iov_number_elements-1));

}

TEST_P(SNAPPY_RawUncompressToIOVec_single, AOCL_Compression_snappy_RawUncompressToIOVec_fail_common_3) // partial_compressed_buffer
{
    pair<string,string> c_u_pair = fourByteOffset();

    size_t original_length = c_u_pair.second.size();
    if(original_length == 0)
    {
        return;
    }

    int iov_number_elements = 5;
    if(original_length < 5)
    {
        iov_number_elements = original_length;
    }

    vector<int> kLengths;
    init(original_length, iov_number_elements, kLengths, iov);

    EXPECT_FALSE(snappy::RawUncompressToIOVec(c_u_pair.first.data(), min(c_u_pair.first.size()-1, (size_t)0), iov.data(), iov_number_elements));

}

TEST_P(SNAPPY_RawUncompressToIOVec_multiple_fail_cases, AOCL_Compression_snappy_RawUncompressToIOVec_fail_common_4) // failure edge cases
{
    string compressed = get<1>(GetParam()).parameter;

    const int iov_number_elements = 5;
    static const int kLengths[] = {2, 20, 4, 100000, 128};

    iov = vector<iovec>(iov_number_elements);
    iov_init(iov.data(), kLengths, iov_number_elements);

    EXPECT_FALSE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), iov.data(), iov_number_elements));
    
}

TEST_P(SNAPPY_RawUncompressToIOVec_single, AOCL_Compression_snappy_RawUncompressToIOVec_pass_common_5)   // IOVecSinkEdgeCases
{
    // Test some tricky edge cases in the iovec output that are not necessarily
    // exercised by random tests.

    // Our output blocks look like this initially (the last iovec is bigger
    // than depicted):
    // [  ] [ ] [    ] [        ] [        ]
    static const int kLengths[] = {2, 1, 4, 8, 128};

    iov = vector<iovec>(ARRAYSIZE(kLengths));
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));

    std::string compressed;
    SNAPPY_Gtest_Util::Append32(&compressed, 22);

    // A literal whose output crosses three blocks.
    // [ab] [c] [123 ] [        ] [        ]
    AppendLiteral(compressed, "abc123");

    // A copy whose output crosses two blocks (source and destination
    // segments marked).
    // [ab] [c] [1231] [23      ] [        ]
    //           ^--^   --
    AppendCopy(compressed, 3, 3);

    // A copy where the input is, at first, in the block before the output:
    //
    // [ab] [c] [1231] [231231  ] [        ]
    //           ^---     ^---
    // Then during the copy, the pointers move such that the input and
    // output pointers are in the same block:
    //
    // [ab] [c] [1231] [23123123] [        ]
    //                  ^-    ^-
    // And then they move again, so that the output pointer is no longer
    // in the same block as the input pointer:
    // [ab] [c] [1231] [23123123] [123     ]
    //                    ^--      ^--
    AppendCopy(compressed, 6, 9);

    // Finally, a copy where the input is from several blocks back,
    // and it also crosses three blocks:
    //
    // [ab] [c] [1231] [23123123] [123b    ]
    //   ^                            ^
    // [ab] [c] [1231] [23123123] [123bc   ]
    //       ^                         ^
    // [ab] [c] [1231] [23123123] [123bc12 ]
    //           ^-                     ^-
    AppendCopy(compressed, 17, 4);

    EXPECT_TRUE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), iov.data(), ARRAYSIZE(kLengths)));
    EXPECT_EQ(0, memcmp(iov[0].iov_base, "ab", 2));
    EXPECT_EQ(0, memcmp(iov[1].iov_base, "c", 1));
    EXPECT_EQ(0, memcmp(iov[2].iov_base, "1231", 4));
    EXPECT_EQ(0, memcmp(iov[3].iov_base, "23123123", 8));
    EXPECT_EQ(0, memcmp(iov[4].iov_base, "123bc12", 7));

}

TEST_P(SNAPPY_RawUncompressToIOVec_single, AOCL_Compression_snappy_RawUncompressToIOVec_fail_common_6)    // IOVecLiteralOverflow
{
    // Total length is 7.
    static const int kLengths[] = {3, 4};

    iov = vector<iovec>(ARRAYSIZE(kLengths));

    std::string compressed;
    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);

    AppendLiteral(compressed, "12345678");

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), iov.data(), ARRAYSIZE(kLengths)));

}

TEST_P(SNAPPY_RawUncompressToIOVec_single, AOCL_Compression_snappy_RawUncompressToIOVec_fail_common_7)    // IOVecCopyOverflow
{
    // Total length is 7.
    static const int kLengths[] = {3, 4};

    iov = vector<iovec>(ARRAYSIZE(kLengths));
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));

    std::string compressed;
    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);

    AppendLiteral(compressed, "123");
    AppendCopy(compressed, 3, 5);

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), iov.data(), ARRAYSIZE(kLengths)));

}

TEST_P(SNAPPY_RawUncompressToIOVec_single, AOCL_Compression_snappy_RawUncompressToIOVec_fail_common_8)   // compressed data is NULL
{
    // If the pointers points to a NULL value then failure occurs.
    static const int kLengths[] = {2, 1, 4, 8, 128};
    iov = vector<iovec>(ARRAYSIZE(kLengths));
    std::string compressed;
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));
    
    SNAPPY_Gtest_Util::Append32(&compressed, 22);
    AppendLiteral(compressed, "abc123");
    AppendCopy(compressed, 3, 3);
    AppendCopy(compressed, 6, 9);
    AppendCopy(compressed, 17, 4);

    EXPECT_FALSE(snappy::RawUncompressToIOVec(NULL, compressed.size(), iov.data(), ARRAYSIZE(kLengths)));

}

TEST_P(SNAPPY_RawUncompressToIOVec_single, AOCL_Compression_snappy_RawUncompressToIOVec_fail_common_9)    // iovec is NULL
{
    // The pointers if points to a NULL value then failure occurs.
    static const int kLengths[] = {2, 1, 4, 8, 128};
    iov = vector<iovec>(ARRAYSIZE(kLengths));
    std::string compressed;
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));
    
    SNAPPY_Gtest_Util::Append32(&compressed, 22);
    AppendLiteral(compressed, "abc123");
    AppendCopy(compressed, 3, 3);
    AppendCopy(compressed, 6, 9);
    AppendCopy(compressed, 17, 4);

    EXPECT_FALSE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), NULL, ARRAYSIZE(kLengths)));

}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompressToIOVec_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompressToIOVec_multiple_fail_cases,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompressToIOVec_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

/***********************************************
* End of RawUncompressToIOVec
************************************************/

/************************************************************************************************
* Begin of `RawUncompressToIOVec(Source* compressed, const struct iovec* iov, size_t iov_cnt);`
*************************************************************************************************/

class SNAPPY_RawUncompressToIOVec_src_single :  public Standalone_Dynamic_dispatcher
{
    public:
    ~SNAPPY_RawUncompressToIOVec_src_single()
    {
        for (int i = 0; i < iov.size(); ++i)
        {
            delete[] reinterpret_cast<char *>(iov[i].iov_base);
        }
    }

    vector<iovec> iov;
};

TEST_P(SNAPPY_RawUncompressToIOVec_src_single, AOCL_Compression_snappy_RawUncompressToIoVec_src_fail_common_1)
{
    // The first two arguments should not point to NULL.
    static const int kLengths[] = {2, 1, 4, 8, 128};
    iov = vector<iovec>(ARRAYSIZE(kLengths));
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));

    EXPECT_FALSE(snappy::RawUncompressToIOVec(NULL, iov.data(), ARRAYSIZE(kLengths)));

}

TEST_P(SNAPPY_RawUncompressToIOVec_src_single, AOCL_Compression_snappy_RawUncompressToIoVec_src_fail_common_2)
{
    // The first two arguments should not point to NULL.
    std::string compressed;

    SNAPPY_Gtest_Util::Append32(&compressed, 22);
    AppendLiteral(compressed, "abc123");
    AppendCopy(compressed, 3, 3);
    AppendCopy(compressed, 6, 9);
    AppendCopy(compressed, 17, 4);
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    EXPECT_FALSE(snappy::RawUncompressToIOVec(src, NULL, 5));
    delete src;
}

TEST_P(SNAPPY_RawUncompressToIOVec_src_single, AOCL_Compression_snappy_RawUncompressToIoVec_src_pass_common_3)    // IOVecSinkEdgeCases
{
    // Test some tricky edge cases in the iovec output that are not necessarily
    // exercised by random tests.

    // Our output blocks look like this initially (the last iovec is bigger
    // than depicted):
    // [  ] [ ] [    ] [        ] [        ]
    static const int kLengths[] = {2, 1, 4, 8, 128};
    std::string compressed;
    iov = vector<iovec>(ARRAYSIZE(kLengths));
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));

    SNAPPY_Gtest_Util::Append32(&compressed, 22);

    // A literal whose output crosses three blocks.
    // [ab] [c] [123 ] [        ] [        ]
    AppendLiteral(compressed, "abc123");

    // A copy whose output crosses two blocks (source and destination
    // segments marked).
    // [ab] [c] [1231] [23      ] [        ]
    //           ^--^   --
    AppendCopy(compressed, 3, 3);

    // A copy where the input is, at first, in the block before the output:
    //
    // [ab] [c] [1231] [231231  ] [        ]
    //           ^---     ^---
    // Then during the copy, the pointers move such that the input and
    // output pointers are in the same block:
    //
    // [ab] [c] [1231] [23123123] [        ]
    //                  ^-    ^-
    // And then they move again, so that the output pointer is no longer
    // in the same block as the input pointer:
    // [ab] [c] [1231] [23123123] [123     ]
    //                    ^--      ^--
    AppendCopy(compressed, 6, 9);

    // Finally, a copy where the input is from several blocks back,
    // and it also crosses three blocks:
    //
    // [ab] [c] [1231] [23123123] [123b    ]
    //   ^                            ^
    // [ab] [c] [1231] [23123123] [123bc   ]
    //       ^                         ^
    // [ab] [c] [1231] [23123123] [123bc12 ]
    //           ^-                     ^-
    AppendCopy(compressed, 17, 4);
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_TRUE(snappy::RawUncompressToIOVec(src, iov.data(), ARRAYSIZE(kLengths)));
    EXPECT_EQ(0, memcmp(iov[0].iov_base, "ab", 2));
    EXPECT_EQ(0, memcmp(iov[1].iov_base, "c", 1));
    EXPECT_EQ(0, memcmp(iov[2].iov_base, "1231", 4));
    EXPECT_EQ(0, memcmp(iov[3].iov_base, "23123123", 8));
    EXPECT_EQ(0, memcmp(iov[4].iov_base, "123bc12", 7));

    delete src;
}

TEST_P(SNAPPY_RawUncompressToIOVec_src_single, AOCL_Compression_snappy_RawUncompressToIoVec_src_fail_common_4)    // IOVecLiteralOverflow
{
    static const int kLengths[] = {3, 4};
    iov = vector<iovec>(ARRAYSIZE(kLengths));
    std::string compressed;
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));

    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);
    AppendLiteral(compressed, "12345678");
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(src, iov.data(), ARRAYSIZE(kLengths)));

    delete src;
}

TEST_P(SNAPPY_RawUncompressToIOVec_src_single, AOCL_Compression_snappy_RawUncompressToIoVec_src_fail_common_5)    // IOVecCopyOverflow
{
    static const int kLengths[] = {3, 4};
    iov = vector<iovec>(ARRAYSIZE(kLengths));
    std::string compressed;
    iov_init(iov.data(), kLengths, ARRAYSIZE(kLengths));

    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);
    AppendLiteral(compressed, "123");
    AppendCopy(compressed, 3, 5);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(src, iov.data(), ARRAYSIZE(kLengths)));

    delete src;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompressToIOVec_src_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/************************************************************************************************
* End of `RawUncompressToIOVec(Source* compressed, const struct iovec* iov, size_t iov_cnt);`
*************************************************************************************************/

/********************************************************************
* Begin of `bool Uncompress(Source* compressed, Sink* uncompressed);`
*********************************************************************/

class SNAPPY_Uncompress_src_fail : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_Uncompress_src : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_Uncompress_src_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_Uncompress_src, AOCL_Compression_snappy_Uncompress_src_pass_common_1) // pass cases
{

    pair<string, string> p = get<1>(GetParam()).parameter;
    string &compressed = p.first;
    string &original = p.second;
    string result;
    result.resize(original.size());

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    EXPECT_TRUE(Uncompress(src, sink));
    EXPECT_EQ(original, result);

    delete src;
    delete sink;
}

TEST_P(SNAPPY_Uncompress_src_single, AOCL_Compression_snappy_Uncompress_src_fail_common_2) // Partial compressed buffer
{
    pair<string, string> p = fourByteOffset();
    string &compressed = p.first;
    string &original = p.second;
    string result;
    result.resize(original.size());

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size() - 1);
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    EXPECT_FALSE(Uncompress(src, sink));

    delete src;
    delete sink;
}

TEST_P(SNAPPY_Uncompress_src_single, AOCL_Compression_snappy_Uncompress_src_fail_common_3) // NULL checks
{
    string compressed = "helloWorld";
    string result;
    result.resize(100);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    EXPECT_FALSE(Uncompress(NULL, sink));
    EXPECT_FALSE(Uncompress(src, NULL));

    delete src;
    delete sink;
}

TEST_P(SNAPPY_Uncompress_src_fail, AOCL_Compression_snappy_Uncompress_src_fail_common_4) // Fail cases
{

    string compressed = get<1>(GetParam()).parameter;
    string result;
    result.resize(100000);
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    EXPECT_FALSE(Uncompress(src, sink));

    delete src;
    delete sink;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Uncompress_src_fail,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Uncompress_src,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Uncompress_src_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/******************************************************************
* End of `bool Uncompress(Source* compressed, Sink* uncompressed);`
*******************************************************************/

/*********************************************************************************************************
* Begin of `bool Uncompress(const char* compressed, size_t compressed_length, std::string* uncompressed);`
**********************************************************************************************************/
class SNAPPY_Uncompress_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_Uncompress_fail_multiple : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_Uncompress_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_Uncompress_single, AOCL_Compression_snappy_Uncompress_pass_common_1) // pass case edge
{
    // Check that we do not read past end of input

    // Make a compressed string that ends with a single-byte literal
    std::string compressed;
    std::string uncompressed;

    SNAPPY_Gtest_Util::Append32(&compressed, 1);
    AppendLiteral(compressed, "x");
    string c(compressed);

    EXPECT_TRUE(snappy::Uncompress(c.data(), c.size(), &uncompressed));
    EXPECT_EQ(uncompressed, std::string("x"));
}

TEST_P(SNAPPY_Uncompress_fail_multiple, AOCL_Compression_snappy_Uncompress_fail_common_2) // fail cases
{
    string compressed = get<1>(GetParam()).parameter;
    string result;
    EXPECT_FALSE(Uncompress(compressed.data(), compressed.size(), &result));
}

TEST_P(SNAPPY_Uncompress_multiple, AOCL_Compression_snappy_Uncompress_pass_common_3) // pass cases
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string &compressed = p.first;
    string &original = p.second;
    string result;

    EXPECT_TRUE(Uncompress(compressed.data(), compressed.size(), &result));
    EXPECT_EQ(original, result);
}

TEST_P(SNAPPY_Uncompress_single, AOCL_Compression_snappy_Uncompress_fail_common_4) // NULL checks
{
    string decompressed;
    char c[11] = "helloWorld";
    
    EXPECT_FALSE(Uncompress(NULL, 100, &decompressed));
    EXPECT_FALSE(Uncompress(c, 10, NULL));
}

TEST_P(SNAPPY_Uncompress_single, AOCL_Compression_snappy_Uncompress_pass_common_5) // fourByteOffset
{
    pair<string, string> p = fourByteOffset();

    string decompressed;

    EXPECT_FALSE(Uncompress(p.first.data(), min(p.first.size()-10, (size_t)0), &decompressed));   // AOCL_Compression_snappy_Uncompress_common_18
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Uncompress_fail_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Uncompress_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Uncompress_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*********************************************************************************************************
* End of `bool Uncompress(const char* compressed, size_t compressed_length, std::string* uncompressed);`
**********************************************************************************************************/

/****************************************************************************************
* Begin of `size_t UncompressAsMuchAsPossible(Source* compressed, Sink* uncompressed);`
*****************************************************************************************/

class SNAPPY_UncompressAsMuchAsPossible_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_UncompressAsMuchAsPossible_fail_multiple : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_UncompressAsMuchAsPossible_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_UncompressAsMuchAsPossible_multiple, AOCL_Compression_snappy_UncompressAsMuchAsPossible_pass_common_1) // pass
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string &original = p.second;
    string &compressed = p.first;
    string result;
    result.resize(original.size());

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    size_t sz = UncompressAsMuchAsPossible(src, sink);

    EXPECT_EQ(sz, original.size());
    EXPECT_EQ(original, result);

    delete src;
    delete sink;
}

TEST_P(SNAPPY_UncompressAsMuchAsPossible_single, AOCL_Compression_snappy_UncompressAsMuchAsPossible_pass_common_2)  // partial compressed buffer provided
{
    pair<string, string> p = fourByteOffset();
    string &original = p.second;
    string &compressed = p.first;
    string result;
    result.resize(original.size());

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size()-10);
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    size_t sz = UncompressAsMuchAsPossible(src, sink);

    EXPECT_NE(sz, 0);
    EXPECT_EQ(0, memcmp(original.data(), result.data(), sz));

    delete src;
    delete sink;
}

TEST_P(SNAPPY_UncompressAsMuchAsPossible_fail_multiple, AOCL_Compression_snappy_UncompressAsMuchAsPossible_fail_common_3) // fail cases
{
    string compressed = get<1>(GetParam()).parameter;
    string result;
    result.resize(100);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    size_t sz = UncompressAsMuchAsPossible(src, sink);

    EXPECT_EQ(sz, 0);

    delete src;
    delete sink;
}

TEST_P(SNAPPY_UncompressAsMuchAsPossible_single, AOCL_Compression_snappy_UncompressAsMuchAsPossible_fail_common_4) // NULL checks
{
    string compressed = "helloWorld";
    string result;
    result.resize(100);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    EXPECT_FALSE(UncompressAsMuchAsPossible(NULL, sink));
    EXPECT_FALSE(UncompressAsMuchAsPossible(src, NULL));

    delete src;
    delete sink;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_UncompressAsMuchAsPossible_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_UncompressAsMuchAsPossible_fail_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_UncompressAsMuchAsPossible_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/****************************************************************************************
* End of `size_t UncompressAsMuchAsPossible(Source* compressed, Sink* uncompressed);`
*****************************************************************************************/

/*******************************************************************************************************
* Begin of `bool RawUncompress(const char* compressed, size_t compressed_length, char* uncompressed);`
********************************************************************************************************/

class SNAPPY_RawUncompress_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_RawUncompress_fail_multiple : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_RawUncompress_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_RawUncompress_fail_multiple, AOCL_Compression_snappy_RawUncompress_fail_common_1) // fail cases
{
    string compressed = get<1>(GetParam()).parameter;
    char result[1000000];

    EXPECT_FALSE(RawUncompress(compressed.data(), compressed.size(), result));
}

TEST_P(SNAPPY_RawUncompress_multiple, AOCL_Compression_snappy_RawUncompress_pass_common_2)
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string &compressed = p.first;
    string &original = p.second;
    string result;
    result.resize(original.size());

    EXPECT_TRUE(RawUncompress(compressed.data(), compressed.size(), string_as_array(&result)));
    EXPECT_EQ(result, original);
}

TEST_P(SNAPPY_RawUncompress_multiple, AOCL_Compression_snappy_RawUncompress_fail_common_3) // partial buffer provided
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string &compressed = p.first;
    string &original = p.second;
    string result;
    result.resize(original.size());

    // Full compressed buffer is not provided
    EXPECT_FALSE(RawUncompress(compressed.data(), min(compressed.size()-10, (size_t)0), string_as_array(&result)));
}


TEST_P(SNAPPY_RawUncompress_single, AOCL_Compression_snappy_RawUncompress_fail_common_4) // NULL checks
{
    char c[100];
    string s = "Hello world";
    string result;

    Compress(s.data(), s.size(), &result);

    EXPECT_FALSE(RawUncompress(NULL, 100, c));
    EXPECT_FALSE(RawUncompress(result.data(), result.size(), NULL));
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_fail_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/*****************************************************************************************************
* End of `bool RawUncompress(const char* compressed, size_t compressed_length, char* uncompressed);`
******************************************************************************************************/

/***********************************************************************************************************
* Begin of `snappy_status snappy_uncompress(const char* compressed,
                                size_t compressed_length, char* uncompressed, size_t* uncompressed_length);`
************************************************************************************************************/

class SNAPPY_snappy_uncompress_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_snappy_uncompress_fail_multiple : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_snappy_uncompress_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_snappy_uncompress_fail_multiple, AOCL_Compression_snappy_snappy_uncompress_fail_common_1) // uncompress_fail_cases
{
    string compressed = get<1>(GetParam()).parameter;
    size_t uncompressed_size = 1000000;
    vector<char> uncompressed(uncompressed_size);

    EXPECT_NE(snappy_uncompress(compressed.data(), compressed.size(), uncompressed.data(), &uncompressed_size), SNAPPY_OK);
}

TEST_P(SNAPPY_snappy_uncompress_multiple, AOCL_Compression_snappy_snappy_uncompress_pass_common_2) // pass cases
{
    pair<string, string> p = get<1>(GetParam()).parameter;
    string &compressed = p.first;
    string &original = p.second;
    size_t uncompressed_size = original.size();
    vector<char> uncompressed(uncompressed_size);

    EXPECT_EQ(snappy_uncompress(compressed.data(), compressed.size(), uncompressed.data(), &uncompressed_size), SNAPPY_OK);
    ASSERT_EQ(uncompressed_size, original.size());
    EXPECT_EQ(0, memcmp(uncompressed.data(), original.data(), uncompressed_size));
}

TEST_P(SNAPPY_snappy_uncompress_single, AOCL_Compression_snappy_snappy_uncompress_fail_common_3) // uncompressed_buffer_size_insufficient
{
    pair<string, string> p = fourByteOffset();
    string &compressed = p.first;
    string &original = p.second;
    string result;
    size_t uncompressed_size = original.size()-1;
    vector<char> uncompressed(uncompressed_size);

    EXPECT_EQ(snappy_uncompress(compressed.data(), compressed.size(), uncompressed.data(), &uncompressed_size), SNAPPY_BUFFER_TOO_SMALL);
}

TEST_P(SNAPPY_snappy_uncompress_single, AOCL_Compression_snappy_snappy_uncompress_fail_common_4) // compressed is NULL
{
    char c[100];
    string s = "Hello world";
    string result;
    size_t uncompressed_size = s.size();

    Compress(s.data(), s.size(), &result);

    EXPECT_EQ(snappy_uncompress(NULL, 100, c, &uncompressed_size), SNAPPY_INVALID_INPUT);                         // AOCL_Compression_snappy_snappy_uncompress_common_16
}

TEST_P(SNAPPY_snappy_uncompress_single, AOCL_Compression_snappy_snappy_uncompress_fail_common_5) // uncompress is NULL
{
    string s = "Hello world";
    string result;
    size_t uncompressed_size = s.size();

    Compress(s.data(), s.size(), &result);

    EXPECT_EQ(snappy_uncompress(result.data(), result.size(), NULL, &uncompressed_size), SNAPPY_INVALID_INPUT);   // AOCL_Compression_snappy_snappy_uncompress_common_17
}

TEST_P(SNAPPY_snappy_uncompress_single, AOCL_Compression_snappy_snappy_uncompress_fail_common_6) // uncompressed length pointer is NULL
{
    char c[100];
    string s = "Hello world";
    string result;
    size_t uncompressed_size = snappy_max_compressed_length(s.size());

    Compress(s.data(), s.size(), &result);

    EXPECT_EQ(snappy_uncompress(result.data(), result.size(), c, NULL), SNAPPY_INVALID_INPUT);
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_uncompress_fail_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_uncompress_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_uncompress_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/***********************************************************************************************************
* End of `snappy_status snappy_uncompress(const char* compressed,
                                size_t compressed_length, char* uncompressed, size_t* uncompressed_length);`
************************************************************************************************************/

/****************************************************************************
* Begin of `bool RawUncompress(Source* compressed, char* uncompressed);`
*****************************************************************************/

class SNAPPY_RawUncompress_src_multiple : public Combined_Dynamic_dispatcher<PAIR_WRAPPER>
{};

class SNAPPY_RawUncompress_src_multiple_fail : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_RawUncompress_src_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_RawUncompress_src_multiple_fail, AOCL_Compression_snappy_RawUncompress_src_fail_common_1) // fail cases
{

    string compressed = get<1>(GetParam()).parameter;
    char result[100000];
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(RawUncompress(src, result));

    delete src;
}

TEST_P(SNAPPY_RawUncompress_src_multiple, AOCL_Compression_snappy_RawUncompress_src_pass_common_2) // pass cases
{

    pair<string, string> p = get<1>(GetParam()).parameter;
    string &compressed = p.first;
    string &original = p.second;
    char result[1000000];

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_TRUE(RawUncompress(src, result));
    EXPECT_TRUE(memcmp(result, original.data(), original.size())==0);

    delete src;
}

TEST_P(SNAPPY_RawUncompress_src_single, AOCL_Compression_snappy_RawUncompress_src_fail_common_3) // NULL checks
{
    string compressed = "helloWorld";
    string result;
    result.resize(100);
    char uncompressed[100];

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(RawUncompress(NULL, uncompressed));
    EXPECT_FALSE(RawUncompress(src, NULL));

    delete src;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_src_multiple_fail,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_fail_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_src_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(uncompress_pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_src_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));


/****************************************************************************
* End of `bool RawUncompress(Source* compressed, char* uncompressed);`
*****************************************************************************/

/**********************************************************************************************
* Begin of `size_t Compress(const char* input, size_t input_length, std::string* compressed);`
***********************************************************************************************/

class SNAPPY_Compress_multiple : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_Compress_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_Compress_multiple, AOCL_Compression_snappy_Compress_pass_common_1)  // simple pass case
{
    string compressed;
    string uncompressed;
    string input = get<1>(GetParam()).parameter;

    const size_t written = snappy::Compress(input.data(), input.size(), &compressed);

    EXPECT_EQ(written, compressed.size());
    EXPECT_TRUE(snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed));
    EXPECT_EQ(uncompressed, input);
}

TEST_P(SNAPPY_Compress_single, AOCL_Compression_snappy_Compress_fail_common_2)
{
    string dest;

    EXPECT_FALSE(Compress(NULL, 10, &dest));
}

TEST_P(SNAPPY_Compress_single, AOCL_Compression_snappy_Compress_fail_common_3)
{
    const char src[11] = "helloWorld";

    EXPECT_FALSE(Compress(src, 10, NULL));
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Compress_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Compress_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/**********************************************************************************************
* End of `size_t Compress(const char* input, size_t input_length, std::string* compressed);`
***********************************************************************************************/

/************************************************************
* Begin of `size_t Compress(Source* source, Sink* sink);`
*************************************************************/

class SNAPPY_Compress_src_multiple : public SNAPPY_Compress_multiple
{};

class SNAPPY_Compress_src_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_Compress_src_multiple, AOCL_Compression_snappy_Compress_src_pass_common_1)
{
    string input = get<1>(GetParam()).parameter;
    Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(input.data(), input.size());
    char *c = (char *)malloc(MaxCompressedLength(input.size()));
    Sink *compressed = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(c);
    // Compress data from `source` into `compressed`.
    const size_t written = Compress(source, compressed);

    std::string uncompressed;
    uncompressed.resize(input.size());

    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&uncompressed));
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(c, written);

    // Checks if compression happened successfull by checking if Decompressed output matches
    // the original input.
    EXPECT_TRUE(snappy::Uncompress(src, sink));
    EXPECT_EQ(uncompressed, input);

    delete src;
    delete sink;
    delete compressed;
    free(c);
    delete source;
}

TEST_P(SNAPPY_Compress_src_single, AOCL_Compression_snappy_Compress_src_fail_common_2)      // src is NULL
{
    char result[10];

    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(result);

    EXPECT_FALSE(Compress(NULL, sink));

    delete sink;
}

TEST_P(SNAPPY_Compress_src_single, AOCL_Compression_snappy_Compress_src_fail_common_3)      // sink is NULL
{
    string source = "helloWorld";

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(source.data(), source.size());

    EXPECT_FALSE(Compress(src, NULL));

    delete src;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Compress_src_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Compress_src_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/************************************************************
* Begin of `size_t Compress(Source* source, Sink* sink);`
*************************************************************/

/***************************************************************************************************************
* Begin of `void RawCompress(const char* input,size_t input_length,char* compressed, size_t* compressed_length)`
****************************************************************************************************************/

class SNAPPY_RawCompress_multiple : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_RawCompress_single : public Standalone_Dynamic_dispatcher
{};


TEST_P(SNAPPY_RawCompress_multiple, AOCL_Compression_snappy_RawCompress_pass_common_1)
{
    string input = get<1>(GetParam()).parameter;

    std::string uncompressed;
    size_t written;
    char *c = (char *)malloc(MaxCompressedLength(input.size()));
    // Compress data from `input` to buffer `c`
    RawCompress(input.data(), input.size(), c, &written);
    EXPECT_TRUE(snappy::IsValidCompressedBuffer(c, written));

    uncompressed.resize(input.size());

    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&uncompressed));
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(c, written);

    // Checks if compression happened successfull by checking if Decompressed output matches
    // the original input.
    EXPECT_TRUE(snappy::Uncompress(src, sink));
    EXPECT_EQ(uncompressed, input);

    delete src;
    delete sink;
    free(c);
}

TEST_P(SNAPPY_RawCompress_single, AOCL_Compression_snappy_RawCompress_fail_common_2)
{
    const size_t temp_len = 2+(rand()%50);
    size_t c_len = temp_len;
    vector<char> compressed(temp_len, 0);

    RawCompress(NULL, 1, compressed.data(), &c_len);
    EXPECT_FALSE(IsValidCompressedBuffer(compressed.data(), c_len));

    EXPECT_EQ(c_len, temp_len);
}

TEST_P(SNAPPY_RawCompress_single, AOCL_Compression_snappy_RawCompress_pass_common_3) // Trying to compress empty buffer
{
    size_t c_len = 50;
    char compressed[50];

    RawCompress(NULL, 0, compressed, &c_len);
    EXPECT_TRUE(IsValidCompressedBuffer(compressed, c_len));

    char temp_buffer[10];
    for(int i=0;i<10;i++)
    {
        temp_buffer[i] = rand()%256;
    }
    char uncompressed[10];
    memcpy(uncompressed, temp_buffer, 10);

    EXPECT_TRUE(RawUncompress(compressed, c_len, uncompressed));
    // check if uncompressed buffer is not disturbed, because the raw data being compressed is of size 0
    EXPECT_EQ(0, memcmp(uncompressed, temp_buffer, 10));
}

TEST_P(SNAPPY_RawCompress_single, AOCL_Compression_snappy_RawCompress_fail_common_4) // NULL check for compressed buffer
{
    char src[11] = "helloWorld";
    size_t c_len = -1;

    RawCompress(src, 10, NULL, &c_len);

    EXPECT_EQ(c_len, (size_t)(-1));
}

TEST_P(SNAPPY_RawCompress_single, AOCL_Compression_snappy_RawCompress_fail_common_5) // NULL check for compressed length pointer
{
    char src[11] = "helloWorld";
    char compressed[50];
    compressed[0] = 5;

    RawCompress(src, 10, compressed, NULL);

    EXPECT_EQ(compressed[0], 5);
    EXPECT_STREQ(src,"helloWorld");
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawCompress_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawCompress_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/**************************************************************************************************************
* End of `void RawCompress(const char* input,size_t input_length,char* compressed, size_t* compressed_length)`
***************************************************************************************************************/

/***************************************************************************************************************
* Begin of `void snappy_compress(const char* input,size_t input_length,char* compressed, size_t* compressed_length)`
****************************************************************************************************************/

class SNAPPY_snappy_compress_multiple : public Combined_Dynamic_dispatcher<STR_WRAPPER>
{};

class SNAPPY_snappy_compress_single : public Standalone_Dynamic_dispatcher
{};

TEST_P(SNAPPY_snappy_compress_multiple, AOCL_Compression_snappy_snappy_compress_pass_common_1)
{
    string input = get<1>(GetParam()).parameter;

    std::string uncompressed;
    size_t written = MaxCompressedLength(input.size());
    char *c = (char *)malloc(written);
    // Compress data from `input` to buffer `c`
    EXPECT_EQ(snappy_compress(input.data(), input.size(), c, &written), SNAPPY_OK);
    EXPECT_TRUE(snappy::IsValidCompressedBuffer(c, written));

    uncompressed.resize(input.size());

    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&uncompressed));
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(c, written);

    // Checks if compression happened successfully, by checking if Decompressed output matches
    // the original input.
    EXPECT_TRUE(snappy::Uncompress(src, sink));
    EXPECT_EQ(uncompressed, input);

    delete src;
    delete sink;
    free(c);
}

TEST_P(SNAPPY_snappy_compress_single, AOCL_Compression_snappy_snappy_compress_fail_common_2) // compressed buffer size insufficient
{
    vector<char> input(1000, 'a');

    std::string uncompressed;
    size_t written = MaxCompressedLength(input.size())-1;
    vector<char> c(written);
    // Compress data from `input` to buffer `c`
    EXPECT_EQ(snappy_compress(input.data(), input.size(), c.data(), &written), SNAPPY_BUFFER_TOO_SMALL);
    EXPECT_FALSE(snappy::IsValidCompressedBuffer(c.data(), written));
}

TEST_P(SNAPPY_snappy_compress_single, AOCL_Compression_snappy_snappy_compress_fail_common_3) // NULL check for compressed buffer
{
    const size_t temp_len = 2+(rand()%50);
    size_t c_len = temp_len;
    vector<char> compressed(temp_len, 0);

    EXPECT_EQ(snappy_compress(NULL, 1, compressed.data(), &c_len), SNAPPY_INVALID_INPUT);
    EXPECT_FALSE(IsValidCompressedBuffer(compressed.data(), c_len));

    EXPECT_EQ(c_len, temp_len);
}

TEST_P(SNAPPY_snappy_compress_single, AOCL_Compression_snappy_snappy_compress_pass_common_4) // Trying to compress empty buffer
{
    size_t c_len = 50;
    char compressed[50];

    EXPECT_EQ(snappy_compress(NULL, 0, compressed, &c_len), SNAPPY_OK);
    EXPECT_TRUE(IsValidCompressedBuffer(compressed, c_len));

    char temp_buffer[10];
    for(int i=0;i<10;i++)
    {
        temp_buffer[i] = rand()%256;
    }
    char uncompressed[10];
    memcpy(uncompressed, temp_buffer, 10);

    EXPECT_TRUE(RawUncompress(compressed, c_len, uncompressed));
    // check if uncompressed buffer is not disturbed, because the raw data being compressed is of size 0
    EXPECT_EQ(0, memcmp(uncompressed, temp_buffer, 10));
}

TEST_P(SNAPPY_snappy_compress_single, AOCL_Compression_snappy_snappy_compress_fail_common_5) // NULL check for compressed buffer
{
    char src[11] = "helloWorld";
    size_t c_len = -1;

    EXPECT_EQ(snappy_compress(src, 10, NULL, &c_len), SNAPPY_INVALID_INPUT);

    EXPECT_EQ(c_len, (size_t)(-1));
}

TEST_P(SNAPPY_snappy_compress_single, AOCL_Compression_snappy_snappy_compress_fail_common_6) // NULL check for compressed length pointer
{
    char src[11] = "helloWorld";
    char compressed[50];
    compressed[0] = 5;

    EXPECT_EQ(snappy_compress(src, 10, compressed, NULL), SNAPPY_INVALID_INPUT);

    EXPECT_EQ(compressed[0], 5);
    // check if uncompressed buffer is not disturbed, because the raw data being compressed is of size 0
    EXPECT_STREQ(src,"helloWorld");
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_compress_multiple,
    ::testing::Combine(::testing::ValuesIn(get_dynamic_dispatcher_flags()), ::testing::ValuesIn(pass_cases())));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_snappy_compress_single,
    ::testing::ValuesIn(get_dynamic_dispatcher_flags()));

/**************************************************************************************************************
* End of `void snappy_compress(const char* input,size_t input_length,char* compressed, size_t* compressed_length)`
***************************************************************************************************************/

/*******************************************
 * "End" of Test cases                     
 *******************************************/

/*********************************************
 * Begin fuzz tests for snappy
 *********************************************/
#ifdef AOCL_TEST_FUZZER
#include "fuzztest/fuzztest.h"

void RawCompress_fuzz(vector<char> source, size_t dest_sz,
                      int level, int optOff, int optLevel)
{
    (void)(level); (void)(dest_sz);
    aocl_setup_snappy(optOff, optLevel, 0, 0, 0);

    // dest should be at least the size MaxCompressedLength(srcSize), or else out of bound memory access error occurs.
    vector<char> dest = vector<char>(MaxCompressedLength(source.size()));
    // destLen stores the size of the compressed data
    size_t destLen;
    RawCompress(source.data(), source.size(), dest.data(), &destLen);
    if(destLen > 0)
    {        
        size_t result = 0;
        #ifdef AOCL_ENABLE_THREADS
            if(!GetUncompressedLengthFromMTCompressedBuffer(dest.data(), destLen, &result))
                return;
        #else
            if(!GetUncompressedLength(dest.data(), destLen, &result))
                return;
        #endif        
        vector<char> decompressed(result);
        int ret = RawUncompress(dest.data(), destLen, decompressed.data());
        EXPECT_EQ(ret, true);
        if(ret)       
          EXPECT_EQ(0,memcmp(decompressed.data(),source.data(), source.size()));
    }   

    aocl_destroy_snappy();
}
FUZZ_TEST(AOCL_Compression_snappy, RawCompress_fuzz)
.WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 0),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_cpr_seed_t<char> {
    auto seed_files = READ_FUZZ_CPR_SEED();
        return get_fuzz_cpr_seeds<char>([](size_t src_sz) -> size_t {
        size_t dst_sz = MaxCompressedLength(src_sz);
        return limit_fuzz_size_max(dst_sz);
    }, 0, 0, seed_files);
})
#endif
;

void RawUncompress_fuzz(vector<char> source, size_t dest_sz,
                        int optOff, int optLevel)
{
    (void)(dest_sz);
    aocl_setup_snappy(optOff, optLevel, 0, 0, 0);

    size_t result = 0;
    #ifdef AOCL_ENABLE_THREADS
        if(!GetUncompressedLengthFromMTCompressedBuffer(source.data(), source.size(), &result))
            return;
    #else
        if(!GetUncompressedLength(source.data(), source.size(), &result))
            return;
    #endif
    // dest should be at least the size of the uncompressed length, or else out of bound memory access error occurs.
    vector<char> dest(result);
    RawUncompress(source.data(), source.size(), dest.data());

    aocl_destroy_snappy();
}
FUZZ_TEST(AOCL_Compression_snappy, RawUncompress_fuzz)
.WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_dpr_seed_t<char> {
  auto seed_files = READ_FUZZ_DPR_SEED();
  return get_fuzz_dpr_seeds<char>(seed_files);
})
#endif
;

void Compress_fuzz(vector<char> input)
{
    std::string compressed = "";
    int ret = Compress(input.data(), input.size(), &compressed);
    if(ret)
    {
        std::string decompressed = "";
        int ret2 = Uncompress(compressed.data(), compressed.size(), &decompressed);
        EXPECT_EQ(ret2, true);
        if(ret2)
            EXPECT_EQ(0,memcmp(decompressed.data(),input.data(), input.size()));
    }
}
FUZZ_TEST(AOCL_Compression_snappy, Compress_fuzz);

void Uncompress_fuzz(vector<char> input)
{
    std::string compressed = "";
    Uncompress(input.data(), input.size(), &compressed);
}
FUZZ_TEST(AOCL_Compression_snappy, Uncompress_fuzz);

void Uncompress_source_sink_fuzz(vector<char> compressed)
{
    string result;
    result.resize(compressed.size()*10);

    Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    Uncompress(source, sink);
    delete source;
    delete sink;
}
FUZZ_TEST(AOCL_Compression_snappy, Uncompress_source_sink_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>()
                );

void Compress_source_sink_fuzz(vector<char> input)
{
    if(input.size() == 0)
        return;
    string result;
    result.resize(MaxCompressedLength(input.size()));

    Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(input.data(), input.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    int ret = Compress(source, sink);
    if(ret > 0)
    {        
        string orig;
        orig.resize(input.size());       
        Sink *decompressed = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&orig));
        Source *compressed = SNAPPY_Gtest_Util::ByteArraySource_ext(string_as_array(&result), ret);
        int ret2 = Uncompress(compressed, decompressed);
        EXPECT_EQ(ret2, true);
        if(ret2)
        { 
            EXPECT_EQ(0,memcmp(string_as_array(&orig),input.data(), input.size()));
        }
        delete decompressed;
        delete compressed;
    }
    delete source;
    delete sink;    
}
FUZZ_TEST(AOCL_Compression_snappy, Compress_source_sink_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>()
                );

void UncompressAsMuchAsPossible_fuzz(vector<char> src)
{
    string result;
    result.resize(src.size()*10);

    Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(src.data(), src.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));
    UncompressAsMuchAsPossible(source,sink);
    delete source;
    delete sink;
}
FUZZ_TEST(AOCL_Compression_snappy, UncompressAsMuchAsPossible_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>()                      
                );

void RawUncompress_source_fuzz(vector<char> src)
{
    string result;
    result.resize(src.size()*10);

    Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(src.data(), src.size());
    RawUncompress(source, string_as_array(&result));
    delete source;
}
FUZZ_TEST(AOCL_Compression_snappy, RawUncompress_source_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>()                             
                );

void IsValidCompressedBuffer_fuzz(vector<char> src)
{
    IsValidCompressedBuffer(src.data(), src.size());
}
FUZZ_TEST(AOCL_Compression_snappy, IsValidCompressedBuffer_fuzz);

void RawUncompressToIOVec_fuzz(vector<char> src, vector<int> input)
{
    //int *kLengths = &input[0];
    vector<iovec> iov = vector<iovec>(input.size());
 
    iov_init(iov.data(), input.data(), input.size());
    RawUncompressToIOVec(src.data(), src.size(), iov.data(), iov.size());

    for (int i = 0; i < iov.size(); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
}
FUZZ_TEST(AOCL_Compression_snappy, RawUncompressToIOVec_fuzz)
    .WithDomains(fuzztest::Arbitrary<std::vector<char>>(),
                fuzztest::VectorOf(fuzztest::InRange(1,10000)).WithMinSize(1).WithMaxSize(10)
                );

#endif /* AOCL_TEST_FUZZER */
/*********************************************
 * End fuzz tests for snappy
 *********************************************/
