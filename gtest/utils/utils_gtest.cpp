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
 
 /** @file utils_gtest.cpp
 *  
 *  @brief Test cases for Unified APIs.
 *
 *  This file contains the test cases for utility functions
 *  in utils/
 *
 *  @author Ashish Sriram
 */

#include <string>
#include <limits>
#include <vector>
#include "gtest/gtest.h"
#include "utils/utils.h"

using namespace std;

/*********************************************
 * Begin test_get_disable_opt_flags Tests
 *********************************************/
int test_get_disable_opt_flags() {
    return get_disable_opt_flags(0);
}

class utils_get_disable_opt_flags : public ::testing::Test {
public:
    void setup(const char* value) {
        int ret = set_env_var(env_var, value);
        ASSERT_EQ(ret, 0);
    }

    void destroy() {
        int ret = unset_env_var(env_var);
        ASSERT_EQ(ret, 0);
    }

private:
    const char* env_var = "AOCL_DISABLE_OPT";
};

TEST_F(utils_get_disable_opt_flags, AOCL_Compression_utils_get_disable_opt_flags_common_1) //AOCL_DISABLE_OPT=ON
{
    setup("ON");
    EXPECT_EQ(test_get_disable_opt_flags(), 1);
    destroy();
}

TEST_F(utils_get_disable_opt_flags, AOCL_Compression_utils_get_disable_opt_flags_common_2) //AOCL_DISABLE_OPT=OFF
{
    setup("OFF");
    EXPECT_EQ(test_get_disable_opt_flags(), 0);
    destroy();
}

TEST_F(utils_get_disable_opt_flags, AOCL_Compression_utils_get_disable_opt_flags_common_3) //AOCL_DISABLE_OPT not present
{
    destroy();
    EXPECT_EQ(test_get_disable_opt_flags(), 0);
}
/*********************************************
 * End test_get_disable_opt_flags Tests
 ********************************************/

 /*********************************************
  * Begin test_get_enabled_inst Tests
  *********************************************/
class utils_get_enabled_inst : public ::testing::Test {
public:
    void setup(const char* value) {
        int ret = set_env_var(env_var, value);
        ASSERT_EQ(ret, 0);
    }

    void destroy() {
        int ret = unset_env_var(env_var);
        ASSERT_EQ(ret, 0);
    }

private:
    const char* env_var = "AOCL_ENABLE_INSTRUCTIONS";
};

TEST_F(utils_get_enabled_inst, AOCL_Compression_utils_get_disable_opt_flags_common_1) //AOCL_ENABLE_INSTRUCTIONS=AVX512
{
    setup("AVX512");
    EXPECT_EQ(test_get_enabled_inst(), 4);
    destroy();
}

TEST_F(utils_get_enabled_inst, AOCL_Compression_utils_get_disable_opt_flags_common_2) //AOCL_ENABLE_INSTRUCTIONS=AVX2
{
    setup("AVX2");
    EXPECT_EQ(test_get_enabled_inst(), 3);
    destroy();
}

TEST_F(utils_get_enabled_inst, AOCL_Compression_utils_get_disable_opt_flags_common_3) //AOCL_ENABLE_INSTRUCTIONS=AVX
{
    setup("AVX");
    EXPECT_EQ(test_get_enabled_inst(), 2);
    destroy();
}

TEST_F(utils_get_enabled_inst, AOCL_Compression_utils_get_disable_opt_flags_common_4) //AOCL_ENABLE_INSTRUCTIONS=SSE2
{
    setup("SSE2");
    EXPECT_EQ(test_get_enabled_inst(), 1);
    destroy();
}

TEST_F(utils_get_enabled_inst, AOCL_Compression_utils_get_disable_opt_flags_common_5) //AOCL_ENABLE_INSTRUCTIONS=DUMMY
{
    setup("DUMMY");
    EXPECT_EQ(test_get_enabled_inst(), 0);
    destroy();
}

TEST_F(utils_get_enabled_inst, AOCL_Compression_utils_get_disable_opt_flags_common_6) //AOCL_ENABLE_INSTRUCTIONS not present
{
    destroy();
#ifndef AOCL_DYNAMIC_DISPATCHER
    EXPECT_EQ(test_get_enabled_inst(), -1);
#else
    EXPECT_EQ(test_get_enabled_inst(), 4);
#endif
}
/*********************************************
 * End test_get_enabled_inst Tests
 ********************************************/
