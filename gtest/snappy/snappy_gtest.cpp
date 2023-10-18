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

#include "algos/snappy/snappy.h"
#include "algos/snappy/snappy-sinksource.h"
#include "algos/snappy/snappy-internal.h"

using namespace std;
using namespace snappy;

#define DEFAULT_OPT_LEVEL 2 // system running gtest must have AVX support

/*******************************************
 * "Begin" of Utility functions
 *******************************************/

void aocl_setup_snappy_test() {
    int optLevel = DEFAULT_OPT_LEVEL;
    aocl_setup_snappy(0, optLevel, 0, 0, 0);
}

 /* This base class can be used for all fixtures
 * that require dynamic dispatcher setup */
class AOCL_setup_snappy : public ::testing::Test {
public:
    AOCL_setup_snappy() {
        aocl_setup_snappy_test();
    }
};

// This a test case generating function, which returns vector of 
// flawed compressed string to test if the responsible functions catches 
// fault compressed strings.
vector<string> fail_cases()
{
    aocl_setup_snappy_test();
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

    return cases;
}

// This function generates simple pass cases.
vector<string> pass_cases()
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

    return v;
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
vector<pair<string, string>> uncompress_pass_cases()
{
    aocl_setup_snappy_test();
    vector<string> v = pass_cases();
    vector<pair<string, string>> cmpr_orig;

    for (auto &element : v)
    {
        string compressed;
        compressed.clear();
        Compress(element.data(), element.size(), &compressed);
        cmpr_orig.push_back({compressed, element});
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

    return cmpr_orig;
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

// Interface for Implementing two variants of a function
// which checks if compressed buffer is valid:
// 1.IsValidCompressedBuffer.
// 2.IsValidCompressed.
class isValid
{
public:
    virtual ~isValid() {};
    virtual bool isVldCmpBfr(string s) = 0;
    virtual bool isVldCmpBfr(const char *c, size_t sz) = 0;
};

// Interface implementation supporting `IsValidCompressedBuffer` function.
class isValidCompressedBuffer : public isValid
{
public:
    bool isVldCmpBfr(string s)
    {
        return IsValidCompressedBuffer(s.data(), s.size());
    }
    bool isVldCmpBfr(const char *c, size_t sz)
    {
        return IsValidCompressedBuffer(c, sz);
    }
};

// Interface implementation supporting `IsValidCompressed` function.
class isValidCompressed : public isValid
{
public:
    bool isVldCmpBfr(string str)
    {
        Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(str.data(), str.size());
        bool return_result = IsValidCompressed(source);
        delete source;
        return return_result;
    }
    bool isVldCmpBfr(const char *c, size_t sz)
    {
        Source *source = SNAPPY_Gtest_Util::ByteArraySource_ext(c, sz);
        bool return_result = IsValidCompressed(source);
        delete source;
        return return_result;
    }
};

/*******************************************
 * "End" of Utility functions              *
 ********************************************/

/**************************************************
 * "Begin" of Test cases
 **************************************************/

TEST(SNAPPY_MaxCompressedLength, all_tests)
{
    EXPECT_EQ(MaxCompressedLength(0), 32);                  // AOCL_Compression_snappy_MaxCompressedLength_common_1
    EXPECT_EQ(MaxCompressedLength(393216), 458784);         // AOCL_Compression_snappy_MaxCompressedLength_common_2
    EXPECT_EQ(MaxCompressedLength(INT_MAX), 2505397620);    // AOCL_Compression_snappy_MaxCompressedLength_common_3
    size_t inp = LLONG_MAX / 2;
    EXPECT_EQ(MaxCompressedLength(inp),5380300354831952585);// AOCL_Compression_snappy_MaxCompressedLength_common_4
}

class SNAPPY_IsValidCompressed_ : public ::testing::TestWithParam<string>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_IsValidCompressed_, Buffer_fail)
{
    isValid *var = new isValidCompressedBuffer();
    string compressed = GetParam();

    EXPECT_FALSE(var->isVldCmpBfr(compressed));

    delete var;
}

class SNAPPY_IsValidCompressed : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_IsValidCompressed, pass)    // AOCL_Compression_snappy_IsValidCompressed_common_10
{
    isValid *var = new isValidCompressed();
    string s = "hello world";
    string compressed;

    Compress(s.data(), s.size(), &compressed);
    EXPECT_TRUE(var->isVldCmpBfr(compressed));

    delete var;
}

TEST_F(SNAPPY_IsValidCompressed, fail_case)   // AOCL_Compression_snappy_IsValidCompressed_common_11
{
    EXPECT_FALSE(IsValidCompressed(NULL));
}

TEST_P(SNAPPY_IsValidCompressed_, source_fail)
{
    isValid *var = new isValidCompressed();
    string compressed = GetParam();

    EXPECT_FALSE(var->isVldCmpBfr(compressed));

    delete var;
}

class SNAPPY_IsValidCompressedBuffer : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_IsValidCompressedBuffer, pass) // AOCL_Compression_snappy_IsValidCompressedBuffer_common_10
{
    isValid *var = new isValidCompressedBuffer();
    string s = "hello world";
    string compressed;

    Compress(s.data(), s.size(), &compressed);

    EXPECT_TRUE(var->isVldCmpBfr(compressed));

    delete var;
}

TEST_F(SNAPPY_IsValidCompressedBuffer, fail_case) // AOCL_Compression_snappy_IsValidCompressedBuffer_common_11
{
    EXPECT_FALSE(IsValidCompressedBuffer(NULL, 100));
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_IsValidCompressed_,
    ::testing::ValuesIn(fail_cases()));

class SNAPPY_GetUncompressedLength : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_GetUncompressedLength, failure_cases)
{
    string s = "Hello world";
    string result;
    size_t uncompressed_length;
    std::string compressed;
    size_t ulength;

    Compress(s.data(), s.size(), &result);

    EXPECT_FALSE(GetUncompressedLength(NULL, result.size(), &uncompressed_length));         // AOCL_Compression_snappy_GetUncompressedLength_common_1
    EXPECT_FALSE(GetUncompressedLength(result.data(), result.size(), NULL));                // AOCL_Compression_snappy_GetUncompressedLength_common_2

    // TruncatedVarint
    compressed.push_back('\xf0');

    EXPECT_FALSE(GetUncompressedLength(compressed.data(), compressed.size(), &ulength));    // AOCL_Compression_snappy_GetUncompressedLength_common_3

    // UnterminatedVarint
    compressed.clear();
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back(10);

    EXPECT_FALSE(GetUncompressedLength(compressed.data(), compressed.size(), &ulength));    // AOCL_Compression_snappy_GetUncompressedLength_common_4

    // OverflowingVarint
    compressed.clear();
    compressed.push_back('\xfb');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\x7f');

    EXPECT_FALSE(GetUncompressedLength(compressed.data(), compressed.size(), &ulength));    // AOCL_Compression_snappy_GetUncompressedLength_common_5
}

TEST_F(SNAPPY_GetUncompressedLength, pass_case)
{
    string s = "Hello world";
    string result;
    size_t uncompressed_length;

    Compress(s.data(), s.size(), &result);

    EXPECT_TRUE(GetUncompressedLength(result.data(), result.size(), &uncompressed_length)); // // AOCL_Compression_snappy_GetUncompressedLength_common_6
    EXPECT_EQ(s.size(), uncompressed_length);
}

class SNAPPY_GetUncompressedLength_source : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_GetUncompressedLength_source, failure_cases)
{
    uint32_t ulength;
    string s = "Hello world";
    string compressed;
    Compress(s.data(), s.size(), &compressed);
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(GetUncompressedLength(NULL, &ulength));                // AOCL_Compression_snappy_GetUncompressedLength_src_common_1
    EXPECT_FALSE(GetUncompressedLength(src, NULL));                     // AOCL_Compression_snappy_GetUncompressedLength_src_common_2

    compressed.clear();
    // TruncatedVarint
    compressed.push_back('\xf0');

    EXPECT_FALSE(checkUncompressedLength(compressed, &ulength));        // AOCL_Compression_snappy_GetUncompressedLength_src_common_3

    // UnterminatedVarint
    compressed.clear();
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back('\x80');
    compressed.push_back(10);

    EXPECT_FALSE(checkUncompressedLength(compressed, &ulength));        // AOCL_Compression_snappy_GetUncompressedLength_src_common_4

    // OverflowingVarint
    compressed.clear();
    compressed.push_back('\xfb');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\xff');
    compressed.push_back('\x7f');

    EXPECT_FALSE(checkUncompressedLength(compressed, &ulength));        // AOCL_Compression_snappy_GetUncompressedLength_src_common_5

    delete src;
}

TEST_F(SNAPPY_GetUncompressedLength_source, pass_cases_UncompressedLength)
{
    string s = "Hello world";
    string result;
    uint32_t ulength;

    Compress(s.data(), s.size(), &result);

    EXPECT_TRUE(checkUncompressedLength(result, &ulength));             // AOCL_Compression_snappy_GetUncompressedLength_src_common_6
    EXPECT_EQ(s.size(), ulength);
}

TEST(SNAPPY_RawUncompressToIOVec, IOVecSinkEdgeCases)   // AOCL_Compression_snappy_RawUncompressToIoVec_common_1
{
    // Test some tricky edge cases in the iovec output that are not necessarily
    // exercised by random tests.

    // Our output blocks look like this initially (the last iovec is bigger
    // than depicted):
    // [  ] [ ] [    ] [        ] [        ]
    static const int kLengths[] = {2, 1, 4, 8, 128};

    struct iovec iov[ARRAYSIZE(kLengths)];
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));

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

    EXPECT_TRUE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), iov, ARRAYSIZE(iov)));
    EXPECT_EQ(0, memcmp(iov[0].iov_base, "ab", 2));
    EXPECT_EQ(0, memcmp(iov[1].iov_base, "c", 1));
    EXPECT_EQ(0, memcmp(iov[2].iov_base, "1231", 4));
    EXPECT_EQ(0, memcmp(iov[3].iov_base, "23123123", 8));
    EXPECT_EQ(0, memcmp(iov[4].iov_base, "123bc12", 7));

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
}

TEST(SNAPPY_RawUncompressToIOVec, IOVecLiteralOverflow) // AOCL_Compression_snappy_RawUncompressToIoVec_common_2
{
    // Total length is 7.
    static const int kLengths[] = {3, 4};

    struct iovec iov[ARRAYSIZE(kLengths)];
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));

    std::string compressed;
    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);

    AppendLiteral(compressed, "12345678");

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), iov, ARRAYSIZE(iov)));

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
}

TEST(SNAPPY_RawUncompressToIOVec, IOVecCopyOverflow)    // AOCL_Compression_snappy_RawUncompressToIoVec_common_3
{
    // Total length is 7.
    static const int kLengths[] = {3, 4};

    struct iovec iov[ARRAYSIZE(kLengths)];
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));

    std::string compressed;
    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);

    AppendLiteral(compressed, "123");
    AppendCopy(compressed, 3, 5);

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), iov, ARRAYSIZE(iov)));

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
}

TEST(SNAPPY_RawUncompressToIOVec_source, fail_cases)
{
    // The first two arguments should not point to NULL.
    static const int kLengths[] = {2, 1, 4, 8, 128};
    struct iovec iov[ARRAYSIZE(kLengths)];
    std::string compressed;
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));

    SNAPPY_Gtest_Util::Append32(&compressed, 22);
    AppendLiteral(compressed, "abc123");
    AppendCopy(compressed, 3, 3);
    AppendCopy(compressed, 6, 9);
    AppendCopy(compressed, 17, 4);
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(snappy::RawUncompressToIOVec(NULL, iov, ARRAYSIZE(iov)));  // AOCL_Compression_snappy_RawUncompressToIoVec_src_4
    EXPECT_FALSE(snappy::RawUncompressToIOVec(src, NULL, ARRAYSIZE(iov)));  // AOCL_Compression_snappy_RawUncompressToIoVec_src_5

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
    delete src;
}

TEST(SNAPPY_RawUncompressToIOVec, fail_cases)
{
    // The pointers if points to a NULL value then failure occurs.
    static const int kLengths[] = {2, 1, 4, 8, 128};
    struct iovec iov[ARRAYSIZE(kLengths)];
    std::string compressed;
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));
    
    SNAPPY_Gtest_Util::Append32(&compressed, 22);
    AppendLiteral(compressed, "abc123");
    AppendCopy(compressed, 3, 3);
    AppendCopy(compressed, 6, 9);
    AppendCopy(compressed, 17, 4);

    EXPECT_FALSE(snappy::RawUncompressToIOVec(NULL, compressed.size(), iov, ARRAYSIZE(iov)));               //  AOCL_Compression_snappy_RawUncompressToIoVec_common_4
    EXPECT_FALSE(snappy::RawUncompressToIOVec(compressed.data(), compressed.size(), NULL, ARRAYSIZE(iov))); //  AOCL_Compression_snappy_RawUncompressToIoVec_common_5

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
}

TEST(SNAPPY_RawUncompressToIOVec_source, IOVecSinkEdgeCases)    //  AOCL_Compression_snappy_RawUncompressToIoVec_src_1
{
    // Test some tricky edge cases in the iovec output that are not necessarily
    // exercised by random tests.

    // Our output blocks look like this initially (the last iovec is bigger
    // than depicted):
    // [  ] [ ] [    ] [        ] [        ]
    static const int kLengths[] = {2, 1, 4, 8, 128};
    std::string compressed;
    struct iovec iov[ARRAYSIZE(kLengths)];
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));

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

    EXPECT_TRUE(snappy::RawUncompressToIOVec(src, iov, ARRAYSIZE(iov)));
    EXPECT_EQ(0, memcmp(iov[0].iov_base, "ab", 2));
    EXPECT_EQ(0, memcmp(iov[1].iov_base, "c", 1));
    EXPECT_EQ(0, memcmp(iov[2].iov_base, "1231", 4));
    EXPECT_EQ(0, memcmp(iov[3].iov_base, "23123123", 8));
    EXPECT_EQ(0, memcmp(iov[4].iov_base, "123bc12", 7));

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
    delete src;
}

TEST(SNAPPY_RawUncompressToIOVec_source, IOVecLiteralOverflow)  // AOCL_Compression_snappy_RawUncompressToIoVec_src_2
{
    static const int kLengths[] = {3, 4};
    struct iovec iov[ARRAYSIZE(kLengths)];
    std::string compressed;
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));

    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);
    AppendLiteral(compressed, "12345678");
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(src, iov, ARRAYSIZE(iov)));

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
    delete src;
}

TEST(SNAPPY_RawUncompressToIOVec_source, IOVecCopyOverflow) //  AOCL_Compression_snappy_RawUncompressToIoVec_src_3
{
    static const int kLengths[] = {3, 4};
    struct iovec iov[ARRAYSIZE(kLengths)];
    std::string compressed;
    iov_init(iov, kLengths, ARRAYSIZE(kLengths));

    // We are constructing a compressed string of length 8.
    SNAPPY_Gtest_Util::Append32(&compressed, 8);
    AppendLiteral(compressed, "123");
    AppendCopy(compressed, 3, 5);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    // Total length of iovec is 7 but uncompressed length is 8 so overflow occurs.
    EXPECT_FALSE(snappy::RawUncompressToIOVec(src, iov, ARRAYSIZE(iov)));

    for (int i = 0; i < ARRAYSIZE(kLengths); ++i)
    {
        delete[] reinterpret_cast<char *>(iov[i].iov_base);
    }
    delete src;
}


class SNAPPY_Uncompress_ : public ::testing::TestWithParam<string>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_Uncompress_, Uncompress_with_buffer)
{

    string compressed = GetParam();
    string result;
    EXPECT_FALSE(Uncompress(compressed.data(), compressed.size(), &result));
}

TEST_P(SNAPPY_Uncompress_, Uncompress_using_Source)
{

    string compressed = GetParam();
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
    SNAPPY_Uncompress_,
    ::testing::ValuesIn(fail_cases()));

class SNAPPY_RawUncompress_ : public ::testing::TestWithParam<string>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_RawUncompress_, with_buffer)
{
    string compressed = GetParam();
    char result[1000000];

    EXPECT_FALSE(RawUncompress(compressed.data(), compressed.size(), result));
}

TEST_P(SNAPPY_RawUncompress_, using_Source)
{

    string compressed = GetParam();
    char result[100000];
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(RawUncompress(src, result));

    delete src;
}

TEST(SNAPPY_RawUncompress_source, fail_compress)
{
    // Arguments should not be NULL for the function `RawUncompress`.

    char uncompressed[100];
    string compressed = "helloWorld";
    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(RawUncompress(NULL, uncompressed));    // AOCL_Compression_snappy_RawUncompress_src_common_16
    EXPECT_FALSE(RawUncompress(src, NULL));             // AOCL_Compression_snappy_RawUncompress_src_common_17

    delete src;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_,
    ::testing::ValuesIn(fail_cases()));

class SNAPPY_Uncompress_f : public AOCL_setup_snappy
{
};

TEST_F(SNAPPY_Uncompress_f, pass_case) // AOCL_Compression_snappy_Uncompress_common_15
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

class SNAPPY_Uncompress_p : public ::testing::TestWithParam<pair<string, string>>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_Uncompress_p, Uncompress_with_buffer)
{

    pair<string, string> p = GetParam();
    string &compressed = p.first;
    string &original = p.second;
    string result;

    EXPECT_TRUE(Uncompress(compressed.data(), compressed.size(), &result));
    EXPECT_EQ(original, result);
}

TEST_P(SNAPPY_Uncompress_p, Uncompress_using_Source)
{

    pair<string, string> p = GetParam();
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

class SNAPPY_Uncompress : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_Uncompress, fail_case)
{
    string decompressed;
    char c[11] = "helloWorld";
    
    EXPECT_FALSE(Uncompress(NULL, 100, &decompressed)); // AOCL_Compression_snappy_Uncompress_common_16
    EXPECT_FALSE(Uncompress(c, 10, NULL));              // AOCL_Compression_snappy_Uncompress_common_17
}

TEST_F(SNAPPY_Uncompress, fail_case2)
{
    char c[1] = "";
    string compressed;
    int j=Compress(c,0,&compressed);

    EXPECT_FALSE(Uncompress(compressed.data(), j, NULL));   // AOCL_Compression_snappy_Uncompress_common_18
}

TEST(SNAPPY_Uncompress_source, fail_cases)
{
    string compressed = "helloWorld";
    string result;
    result.resize(100);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    EXPECT_FALSE(Uncompress(NULL, sink));   // AOCL_Compression_snappy_Uncompress_src_common_16
    EXPECT_FALSE(Uncompress(src, NULL));    // AOCL_Compression_snappy_Uncompress_src_common_17

    delete src;
    delete sink;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Uncompress_p,
    ::testing::ValuesIn(uncompress_pass_cases()));

class SNAPPY_UncompressAsMuchAsPossible_ : public ::testing::TestWithParam<pair<string, string>>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

class SNAPPY_UncompressAsMuchAsPossible_f : public ::testing::TestWithParam<string>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_UncompressAsMuchAsPossible_, pass)
{
    pair<string, string> p = GetParam();
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

TEST_P(SNAPPY_UncompressAsMuchAsPossible_f, fail_cases)
{
    string compressed = GetParam();
    string result;
    result.resize(100);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    size_t sz = UncompressAsMuchAsPossible(src, sink);

    EXPECT_EQ(sz, 0);

    delete src;
    delete sink;
}

TEST(SNAPPY_UncompressAsMuchAsPossible, fail_cases_NULL)
{
    string compressed = "helloWorld";
    string result;
    result.resize(100);

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());
    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(string_as_array(&result));

    EXPECT_FALSE(UncompressAsMuchAsPossible(NULL, sink));   // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_16
    EXPECT_FALSE(UncompressAsMuchAsPossible(src, NULL));    // AOCL_Compression_snappy_UncompressAsMuchAsPossible_common_17

    delete src;
    delete sink;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_UncompressAsMuchAsPossible_,
    ::testing::ValuesIn(uncompress_pass_cases()));

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_UncompressAsMuchAsPossible_f,
    ::testing::ValuesIn(fail_cases()));

class SNAPPY_RawUncompress_p : public ::testing::TestWithParam<pair<string, string>>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_RawUncompress_p, with_buffer)
{

    pair<string, string> p = GetParam();
    string &compressed = p.first;
    string &original = p.second;
    string result;
    result.resize(original.size());

    EXPECT_TRUE(RawUncompress(compressed.data(), compressed.size(), string_as_array(&result)));
    EXPECT_EQ(result, original);
}

TEST_P(SNAPPY_RawUncompress_p, using_Source)
{

    pair<string, string> p = GetParam();
    string &compressed = p.first;
    string &original = p.second;
    char result[1000000];

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_TRUE(RawUncompress(src, result));
    EXPECT_TRUE(memcmp(result, original.data(), original.size())==0);

    delete src;
}

TEST(SNAPPY_RawUncompress, fail_cases)
{
    char c[100];
    string s = "Hello world";
    string result;

    Compress(s.data(), s.size(), &result);

    EXPECT_FALSE(RawUncompress(NULL, 100, c));                         // AOCL_Compression_snappy_RawUncompress_common_16
    EXPECT_FALSE(RawUncompress(result.data(), result.size(), NULL));   // AOCL_Compression_snappy_RawUncompress_common_17
}

TEST(SNAPPY_RawUncompress_source, fail_cases)
{
    string compressed = "helloWorld";
    string result;
    result.resize(100);
    char uncompressed[100];

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(compressed.data(), compressed.size());

    EXPECT_FALSE(RawUncompress(NULL, uncompressed));    // AOCL_Compression_snappy_RawUncompress_src_common_16
    EXPECT_FALSE(RawUncompress(src, NULL));             // AOCL_Compression_snappy_RawUncompress_src_common_17

    delete src;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_RawUncompress_p,
    ::testing::ValuesIn(uncompress_pass_cases()));

class SNAPPY_Compress_ : public ::testing::TestWithParam<string>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_Compress_, with_buffer)
{
    std::string compressed;
    std::string uncompressed;
    string input = GetParam();

    const size_t written = snappy::Compress(input.data(), input.size(), &compressed);

    EXPECT_EQ(written, compressed.size());
    EXPECT_TRUE(snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed));
    EXPECT_EQ(uncompressed, input);
}

class SNAPPY_Compress : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_Compress, fail_case1)   // AOCL_Compression_snappy_Compress_common_5
{
    string dest;

    EXPECT_FALSE(Compress(NULL, 10, &dest));
}

TEST_F(SNAPPY_Compress, fail_case2)   // AOCL_Compression_snappy_Compress_common_6
{
    const char src[11] = "helloWorld";

    EXPECT_FALSE(Compress(src, 10, NULL));
}

TEST_P(SNAPPY_Compress_, using_source)
{
    string input = GetParam();
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

class SNAPPY_Compress_using_source : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_Compress_using_source, fail_case1)  // AOCL_Compression_snappy_Compress_src_common_5
{
    char result[10];

    Sink *sink = SNAPPY_Gtest_Util::UncheckedByteArraySink_ext(result);

    EXPECT_FALSE(Compress(NULL, sink));

    delete sink;
}

TEST_F(SNAPPY_Compress_using_source, fail_case2)  // AOCL_Compression_snappy_Compress_src_common_6
{
    string source = "helloWorld";

    Source *src = SNAPPY_Gtest_Util::ByteArraySource_ext(source.data(), source.size());

    EXPECT_FALSE(Compress(src, NULL));

    delete src;
}

INSTANTIATE_TEST_SUITE_P(
    SNAPPY,
    SNAPPY_Compress_,
    ::testing::ValuesIn(pass_cases()));

class SNAPPY_RawCompress_ : public ::testing::TestWithParam<string>
{
    void SetUp() override {
        aocl_setup_snappy_test();
    }
};

TEST_P(SNAPPY_RawCompress_, pass_cases)
{
    string input = GetParam();

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

class SNAPPY_RawCompress : public AOCL_setup_snappy {
};

TEST_F(SNAPPY_RawCompress, fail_case1)    // AOCL_Compression_snappy_RawCompress_common_5
{
    size_t c_len = -1;
    char compressed[50];

    RawCompress(NULL, 0, compressed, &c_len);

    EXPECT_EQ(c_len, (size_t)(-1));
}

TEST_F(SNAPPY_RawCompress, fail_case2)    // AOCL_Compression_snappy_RawCompress_common_6
{
    char src[11] = "helloWorld";
    size_t c_len = -1;

    RawCompress(src, 10, NULL, &c_len);

    EXPECT_EQ(c_len, (size_t)(-1));
}

TEST_F(SNAPPY_RawCompress, fail_case3)    // AOCL_Compression_snappy_RawCompress_common_7
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
    SNAPPY_RawCompress_,
    ::testing::ValuesIn(pass_cases()),
     [](const ::testing::TestParamInfo<string>& info) {
        static int j=-1;
        j++;
        std::string name = "AOCL_Compresssion_snappy_RawCompress_";
        name+=to_string(j);
        return name;
    });

/*******************************************
 * "End" of Test cases                     
 *******************************************/