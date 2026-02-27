/**
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

/** @file zlib_disable_opt_gtest.cpp
 *
 *  @brief Test cases for AOCL_DISABLE_OPT environment variable handling in ZLIB.
 *
 *  This file contains test cases to verify that zlibOptOff flag is properly
 *  initialized when AOCL_DISABLE_OPT environment variable is set, ensuring
 *  that optimizations can be disabled as intended.
 *
 *  @author  Anand Kumar
 */

#include "zlib_gtest.h"
#include "algos/zlib/deflate.h"  // for test_aocl_zlib_get_zlibOptOff

/**
 * Test fixture for AOCL_DISABLE_OPT tests
 * Manages environment variable setup/cleanup and zlib cleanup
 */
class ZLIB_disable_opt : public ::testing::Test {
public:
    void SetUp() override {
        // Reset zlib state before each test to ensure clean initialization
        aocl_destroy_zlib();
    }

    void setup_env(const char* value) {
        int ret = set_env_var(env_var, value);
        ASSERT_EQ(ret, 0);
    }

    void cleanup_env() {
        int ret = unset_env_var(env_var);
        ASSERT_EQ(ret, 0);
        // Reset zlib state after each test
        aocl_destroy_zlib();
    }

    void TearDown() override {
        cleanup_env();
    }

private:
    const char* env_var = "AOCL_DISABLE_OPT";
};

/*********************************************
 * Test zlibOptOff initialization via compress.c API
 *********************************************/

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_compress_api_ON)
{
    // Set AOCL_DISABLE_OPT=ON
    setup_env("ON");
    
    // Call setup through compress.c API
    aocl_setup_zlib(0, 0, 0, 0, 0);
    
    // Verify zlibOptOff is set to 1 (optimizations disabled)
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should be 1 when AOCL_DISABLE_OPT=ON via compress API";
}

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_compress_api_OFF)
{
    // Set AOCL_DISABLE_OPT=OFF
    setup_env("OFF");
    
    // Call setup through compress.c API
    aocl_setup_zlib(0, 0, 0, 0, 0);
    
    // Verify zlibOptOff is set to 0 (optimizations enabled)
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 0) << "zlibOptOff should be 0 when AOCL_DISABLE_OPT=OFF via compress API";
}

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_compress_api_not_set)
{
    // Don't set AOCL_DISABLE_OPT (ensure it's unset)
    cleanup_env();
    
    // Call setup through compress.c API
    aocl_setup_zlib(0, 0, 0, 0, 0);
    
    // Verify zlibOptOff defaults to 0 (optimizations enabled)
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 0) << "zlibOptOff should default to 0 when AOCL_DISABLE_OPT is not set";
}

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_param_overrides_env)
{
    // Set AOCL_DISABLE_OPT to OFF (optimizations enabled via env)
    setup_env("OFF");
    
    // Call setup with optOff=1 (user wants optimizations disabled)
    aocl_setup_zlib(1, 0, 0, 0, 0);
    
    // Verify zlibOptOff is 1 (user parameter takes priority over env variable)
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should be 1 when aocl_setup_zlib parameter is 1, even if AOCL_DISABLE_OPT=OFF";
}

/*********************************************
 * Test zlibOptOff initialization via deflate native API
 *********************************************/

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_deflate_native_ON)
{
    // Set AOCL_DISABLE_OPT=ON
    setup_env("ON");
    
    // Call deflate API directly (triggers aocl_setup_native in deflate.c)
    ZLIB_deflate_stream deflateObj;
    int ret = deflateInit(deflateObj.get_stream(), Z_DEFAULT_COMPRESSION);
    ASSERT_EQ(ret, Z_OK);
    
    // Verify zlibOptOff is set to 1
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should be 1 when AOCL_DISABLE_OPT=ON via deflate native API";
}

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_deflate_native_OFF)
{
    // Set AOCL_DISABLE_OPT=OFF
    setup_env("OFF");
    
    // Call deflate API directly
    ZLIB_deflate_stream deflateObj;
    int ret = deflateInit(deflateObj.get_stream(), Z_DEFAULT_COMPRESSION);
    ASSERT_EQ(ret, Z_OK);
    
    // Verify zlibOptOff is set to 0
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 0) << "zlibOptOff should be 0 when AOCL_DISABLE_OPT=OFF via deflate native API";
}

/*********************************************
 * Test zlibOptOff initialization via inflate native API
 *********************************************/

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_inflate_native_ON)
{
    // Set AOCL_DISABLE_OPT=ON
    setup_env("ON");
    
    // Call inflate API directly (triggers aocl_setup_native in inflate.c)
    ZLIB_inflate_stream inflateObj;
    int ret = inflateInit(inflateObj.get_stream());
    ASSERT_EQ(ret, Z_OK);
    
    // Verify zlibOptOff is set to 1
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should be 1 when AOCL_DISABLE_OPT=ON via inflate native API";
}

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_inflate_native_OFF)
{
    // Set AOCL_DISABLE_OPT=OFF
    setup_env("OFF");
    
    // Call inflate API directly
    ZLIB_inflate_stream inflateObj;
    int ret = inflateInit(inflateObj.get_stream());
    ASSERT_EQ(ret, Z_OK);
    
    // Verify zlibOptOff is set to 0
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 0) << "zlibOptOff should be 0 when AOCL_DISABLE_OPT=OFF via inflate native API";
}

/*********************************************
 * Test actual compression/decompression behavior with AOCL_DISABLE_OPT
 *********************************************/

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_deflate_with_opt_disabled)
{
    // Set AOCL_DISABLE_OPT=ON
    setup_env("ON");
    
    // Prepare test data
    const char* test_data = "This is test data for compression with optimizations disabled.";
    uLong srcLen = strlen(test_data);
    uLong destLen = compressBound(srcLen);
    Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));
    ASSERT_NE(dest, nullptr) << "Memory allocation failed for dest";
    
    // Compress data
    int ret = compress(dest, &destLen, (const Bytef*)test_data, srcLen);
    if (ret != Z_OK) {
        free(dest);
        FAIL() << "Compression should succeed with AOCL_DISABLE_OPT=ON";
    }
    
    // Verify zlibOptOff was set correctly
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should remain 1 throughout compression";
    
    // Decompress to verify correctness
    uLong uncompLen = srcLen;
    Bytef* uncomp = (Bytef*)malloc(uncompLen * sizeof(Bytef));
    if (uncomp == nullptr) {
        free(dest);
        FAIL() << "Memory allocation failed for uncomp";
    }
    ret = uncompress(uncomp, &uncompLen, dest, destLen);
    if (ret != Z_OK) {
        free(dest);
        free(uncomp);
        FAIL() << "Decompression should succeed";
    }
    EXPECT_EQ(uncompLen, srcLen) << "Decompressed size should match original";
    EXPECT_EQ(memcmp(uncomp, test_data, srcLen), 0) << "Decompressed data should match original";
    
    free(dest);
    free(uncomp);
}

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_inflate_with_opt_disabled)
{
    // First compress with optimizations enabled
    const char* test_data = "Test data for inflation with optimizations disabled.";
    uLong srcLen = strlen(test_data);
    uLong destLen = compressBound(srcLen);
    Bytef* compressed = (Bytef*)malloc(destLen * sizeof(Bytef));
    ASSERT_NE(compressed, nullptr) << "Memory allocation failed for compressed";
    
    int ret = compress(compressed, &destLen, (const Bytef*)test_data, srcLen);
    if (ret != Z_OK) {
        free(compressed);
        FAIL() << "Initial compression failed";
    }
    
    // Clean up and set AOCL_DISABLE_OPT=ON
    aocl_destroy_zlib();
    setup_env("ON");
    
    // Decompress with optimizations disabled
    uLong uncompLen = srcLen;
    Bytef* uncompressed = (Bytef*)malloc(uncompLen * sizeof(Bytef));
    if (uncompressed == nullptr) {
        free(compressed);
        FAIL() << "Memory allocation failed for uncompressed";
    }
    ret = uncompress(uncompressed, &uncompLen, compressed, destLen);
    if (ret != Z_OK) {
        free(compressed);
        free(uncompressed);
        FAIL() << "Decompression should succeed with AOCL_DISABLE_OPT=ON";
    }
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should be 1 during decompression";
    EXPECT_EQ(uncompLen, srcLen) << "Decompressed size should match original";
    EXPECT_EQ(memcmp(uncompressed, test_data, srcLen), 0) << "Decompressed data should match original";
    
    free(compressed);
    free(uncompressed);
}

/*********************************************
 * Test that zlibOptOff persists across multiple operations
 *********************************************/

TEST_F(ZLIB_disable_opt, AOCL_Compression_zlib_zlibOptOff_persistence)
{
    // Set AOCL_DISABLE_OPT=ON
    setup_env("ON");
    
    // Initialize with deflate
    {
        ZLIB_deflate_stream deflateObj;
        int ret = deflateInit(deflateObj.get_stream(), Z_DEFAULT_COMPRESSION);
        ASSERT_EQ(ret, Z_OK);
        EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should be 1 after deflateInit";
    }
    
    // Now initialize inflate - zlibOptOff should still be 1
    {
        ZLIB_inflate_stream inflateObj;
        int ret = inflateInit(inflateObj.get_stream());
        ASSERT_EQ(ret, Z_OK);
        EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should persist as 1 after inflateInit";
    }
    
    // Verify it's still 1
    EXPECT_EQ(test_aocl_zlib_get_zlibOptOff(), 1) << "zlibOptOff should remain 1 throughout session";
}
