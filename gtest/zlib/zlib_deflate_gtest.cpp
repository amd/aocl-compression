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

/** @file zlib_deflate_gtest.cc
 *
 *  @brief Test cases for ZLIB algo.
 *
 *  This file contains the test cases for ZLIB method
 *  testing the API level deflate based functions of ZLIB.
 *
 *  @author  Ravi Jangra
 */

#include "zlib_gtest.h"

// Helper function to verify API contract, after successful deflateInit call
static inline void verify_deflateInit(z_streamp strm)
{
  EXPECT_EQ(strm->msg, (char *)Z_NULL);
  EXPECT_NE(strm->state, (internal_state *)NULL);
  EXPECT_EQ(strm->total_in, 0);
  EXPECT_EQ(strm->total_out, 0);
}

TEST(AOCL_Compression_zlib, deflateInit__common)
{
  ZLIB_deflate_stream deflateObj;
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), Z_DEFAULT_COMPRESSION, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit__common_8
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);
  verify_deflateInit(deflateObj.get_stream());
}

TEST(AOCL_Compression_zlib, deflateInit__negative)
{
  ZLIB_deflate_stream deflateObj;
  char invalid_version[7] = "0.2.11";
  int valid_level = 3;
  int invalid_structure_size = -1;
  const int invalid_levels[2] = {-2, 10};

  EXPECT_EQ(deflateInit_(NULL, valid_level, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_1
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), valid_level, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit__common_2
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), valid_level, invalid_version, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit__common_3
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), valid_level, ZLIB_VERSION, invalid_structure_size), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit__common_4
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), invalid_levels[0], ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_5
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), invalid_levels[1], ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_6
  deflateObj.get_stream()->zalloc = alloc_null;
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), valid_level, ZLIB_VERSION, (int)sizeof(z_stream)), Z_MEM_ERROR);

}

TEST(AOCL_Compression_zlib, deflateInit__boundary)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), 0, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit__common_7
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);
  verify_deflateInit(deflateObj.get_stream());

  deflateObj.reset_deflate_stream();
  EXPECT_EQ(deflateInit_(deflateObj.get_stream(), 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit__common_9
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);
  verify_deflateInit(deflateObj.get_stream());
}

TEST(AOCL_Compression_zlib, deflateInit2__common)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), 6, Z_DEFLATED, 8, 3, 3, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);  // AOCL_Compression_zlib_deflateInit2__common_18
  EXPECT_EQ(deflateObj.get_stream()->state->level, 6);
  EXPECT_EQ(deflateObj.get_stream()->state->strategy, 3);
  EXPECT_EQ(deflateObj.get_stream()->state->method, Z_DEFLATED);
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);
  verify_deflateInit(deflateObj.get_stream());

  deflateObj.reset_deflate_stream();
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), 7, Z_DEFLATED, 25, 3, 4, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit2__common_19
  EXPECT_EQ(deflateObj.get_stream()->state->level, 7);
  EXPECT_EQ(deflateObj.get_stream()->state->strategy, 4);
  verify_deflateInit(deflateObj.get_stream());
}

TEST(AOCL_Compression_zlib, deflateInit2__negative)
{
  ZLIB_deflate_stream deflateObj;
  const int valid_memLevel = 5;
  const int valid_strategy = 2;
  const int valid_windowBits = 11;
  const int valid_level = 6;
  const int invalid_method = 0;
  const int invalid_levels[2] = {-2, 23};
  const int invalid_windowBits[6] = {-7, -16, 7, 16, 23, 32};
  const int invalid_memLevels[2] = {0, 10};
  const int invalid_strategy[2] = {-1, 5};
  const int invalid_structure_size = -1;
  char invalid_version[7] = "0.2.11";

  EXPECT_EQ(deflateInit2_(NULL, valid_level, Z_DEFLATED, valid_windowBits, valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_1
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), invalid_levels[0], Z_DEFLATED, valid_windowBits, valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_2
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), invalid_levels[1], Z_DEFLATED, valid_windowBits, valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_3
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, invalid_method, valid_windowBits, valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_4
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, invalid_windowBits[0], valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_5
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, invalid_windowBits[1], valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_6
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, invalid_windowBits[2], valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_7
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, invalid_windowBits[3], valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_8
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, invalid_windowBits[4], valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_9
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, invalid_windowBits[5], valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_10
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, invalid_memLevels[0], valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_11
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, invalid_memLevels[1], valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_12
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, valid_memLevel, invalid_strategy[0], ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_13
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, valid_memLevel, invalid_strategy[1], ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_14
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, valid_memLevel, valid_strategy, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit2__common_15
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, valid_memLevel, valid_strategy, invalid_version, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit2__common_16
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, valid_memLevel, valid_strategy, ZLIB_VERSION, invalid_structure_size), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit2__common_17
  deflateObj.get_stream()->zalloc = alloc_null;
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_level, Z_DEFLATED, valid_windowBits, valid_memLevel, valid_strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_MEM_ERROR);
}

TEST(AOCL_Compression_zlib, deflateInit2__boundary)
{
  ZLIB_deflate_stream deflateObj;
  const int valid_levels[2] = {-1, 9};
  const int valid_windowBits[6] = {8, 15, 25, 31, -9, -15};
  const int valid_memLevels[2] = {1, 9};
  const int valid_strategy[2] = {0, 4};

  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_levels[0], Z_DEFLATED, valid_windowBits[0], valid_memLevels[0], valid_strategy[0], ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);
  verify_deflateInit(deflateObj.get_stream());

  deflateObj.reset_deflate_stream();
  EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_levels[1], Z_DEFLATED, valid_windowBits[1], valid_memLevels[1], valid_strategy[1], ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);
  verify_deflateInit(deflateObj.get_stream());

  // valid_levels[0] and valid_levels[1] are already tested above
  for(int i = 2; i < 6; i++) {
    deflateObj.reset_deflate_stream();
    EXPECT_EQ(deflateInit2_(deflateObj.get_stream(), valid_levels[1], Z_DEFLATED, valid_windowBits[i], valid_memLevels[0], valid_strategy[1], ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);
    verify_deflateInit(deflateObj.get_stream());
  }
}

TEST(AOCL_Compression_zlib, deflateEnd_common)
{
  ZLIB_deflate_stream deflateObj;

  deflateInit(deflateObj.get_stream(), 0);
  EXPECT_EQ(deflateEnd(deflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_deflateEnd_common_1
}

TEST(AOCL_Compression_zlib, deflateEnd_negative)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflateEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateEnd_common_2
  EXPECT_EQ(deflateEnd(deflateObj.get_stream()), Z_STREAM_ERROR);    // AOCL_Compression_zlib_deflateEnd_common_3
  deflateInit(deflateObj.get_stream(), 4);
  ((deflate_state*)deflateObj.get_stream()->state)->status = BUSY_STATE;
  EXPECT_EQ(deflateEnd(deflateObj.get_stream()), Z_DATA_ERROR);
}

TEST_P(AOCL_Compression_zlib, deflateSetDictionary_negative)
{
  ZLIB_deflate_stream deflateObj;
  const int sDictLen = 10;
  char c[sDictLen + 1] = "abcdefghij";
  Bytef *sDict = (Bytef *)c;

  // AOCL_Compression_zlib_deflateSetDictionary_common_1
  EXPECT_EQ(deflateSetDictionary(deflateObj.get_stream(), sDict, sDictLen), Z_STREAM_ERROR); // stream uninitialized

  deflateInit(deflateObj.get_stream(), 4);

  // AOCL_Compression_zlib_deflateSetDictionary_common_2
  EXPECT_EQ(deflateSetDictionary(deflateObj.get_stream(), NULL, sDictLen), Z_STREAM_ERROR); // dictionary null

  deflateObj.reset_deflate_stream();
  deflateInit2(deflateObj.get_stream(), 4, 8, 26, 4, 4);

  // AOCL_Compression_zlib_deflateSetDictionary_common_3
  EXPECT_EQ(deflateSetDictionary(deflateObj.get_stream(), sDict, sDictLen), Z_STREAM_ERROR); // wrap == 2 due to windowBits==26
}

TEST_P(AOCL_Compression_zlib, deflateSetDictionary_common)
{
  ZLIB_deflate_stream deflateObj;
  Bytef *sDict, *gDict;
  const uLong sDictLen = 4000;
  uInt gDictLen = 0;
  char cc[sDictLen], gc[sDictLen];
  for (uLong i = 0; i < sDictLen; i++)
  {
    cc[i] = rand() % 255;
  }
  deflateInit(deflateObj.get_stream(), 4);
  sDict = (Bytef *)cc;
  gDict = (Bytef *)gc;

  // AOCL_Compression_zlib_deflateSetDictionary_common_4
  EXPECT_EQ(deflateSetDictionary(deflateObj.get_stream(), sDict, 10), Z_OK); // wrap==1
  deflateGetDictionary(deflateObj.get_stream(), gDict, &gDictLen);
  EXPECT_EQ(gDictLen , 10);
  EXPECT_EQ(memcmp(sDict, gDict, 10), 0);

  deflateEnd(deflateObj.get_stream());
  deflateInit2(deflateObj.get_stream(), 4, 8, -9, 4, 4);

  // AOCL_Compression_zlib_deflateSetDictionary_common_5
  EXPECT_EQ(deflateSetDictionary(deflateObj.get_stream(), sDict, sDictLen), Z_OK); // wrap==0
  deflateGetDictionary(deflateObj.get_stream(), gDict, &gDictLen);
  EXPECT_EQ(gDictLen , (deflateObj.get_stream()->state)->w_size);
  EXPECT_EQ(memcmp(sDict + sDictLen - gDictLen, gDict, gDictLen), 0);
}

TEST_P(AOCL_Compression_zlib, deflateGetDictionary_negative)
{
  Bytef *dict = nullptr;
  uInt dictLen;
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflateGetDictionary(deflateObj.get_stream(), dict, &dictLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateGetDictionary_common_1
  EXPECT_EQ(deflateGetDictionary(NULL, dict, &dictLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateGetDictionary_common_2
}

TEST_P(AOCL_Compression_zlib, deflateGetDictionary_common)
{
  Bytef *dict = (Bytef *)malloc(sizeof(Bytef) * 40000);
  uInt dictLen;
  ZLIB_deflate_stream deflateObj;
  deflateInit(deflateObj.get_stream(), 4);
  const int sDictLen = 10;
  char c[sDictLen + 1] = "abcdefghij";
  Bytef *sDict = (Bytef *)c;

  deflateSetDictionary(deflateObj.get_stream(), sDict, sDictLen);

  EXPECT_EQ(deflateGetDictionary(deflateObj.get_stream(), dict, &dictLen), Z_OK);  // AOCL_Compression_zlib_deflateGetDictionary_common_3
  EXPECT_EQ(dictLen, sDictLen);
  EXPECT_TRUE(cmpr(c, (char *)dict, dictLen));

  free(dict);
  dict = nullptr;
}

TEST(AOCL_Compression_zlib, deflateCopy_negative)
{
  ZLIB_deflate_stream dest;
  ZLIB_deflate_stream src;

  EXPECT_EQ(deflateCopy(dest.get_stream(), NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateCopy_common_1
  EXPECT_EQ(deflateCopy(NULL, src.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateCopy_common_2
  EXPECT_EQ(deflateCopy(dest.get_stream(), src.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateCopy_common_3
  
  deflateInit(src.get_stream(), 4);
  src.get_stream()->zalloc = alloc_null;
  EXPECT_EQ(deflateCopy(dest.get_stream(), src.get_stream()), Z_MEM_ERROR);
  dest.get_stream()->state = (internal_state *)0;
}

TEST(AOCL_Compression_zlib, deflateCopy_common)
{
  ZLIB_deflate_stream dest;
  ZLIB_deflate_stream src;
  deflateInit(src.get_stream(), 5);

  EXPECT_EQ(deflateCopy(dest.get_stream(), src.get_stream()), Z_OK);  // AOCL_Compression_zlib_deflateCopy_common_4
}

TEST(AOCL_Compression_zlib, deflateReset_negative)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflateReset(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateReset_common_1
  EXPECT_EQ(deflateReset(deflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateReset_common_2
}

TEST(AOCL_Compression_zlib, deflateReset_common)
{
  ZLIB_deflate_stream deflateObj;
  deflateInit(deflateObj.get_stream(), 4);

  EXPECT_EQ(deflateReset(deflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_deflateReset_common_3
  EXPECT_EQ(deflateObj.get_stream()->total_in, 0);
  EXPECT_EQ(deflateObj.get_stream()->total_out, 0);
  EXPECT_EQ(deflateObj.get_stream()->msg, (const char *)Z_NULL);
  EXPECT_EQ(deflateObj.get_stream()->data_type, Z_UNKNOWN);
  EXPECT_EQ(deflateObj.get_stream()->state->pending, 0);
  EXPECT_EQ(deflateObj.get_stream()->state->last_flush, -2);
}

TEST_P(AOCL_Compression_zlib, deflateParams_negative)
{
  ZLIB_deflate_stream deflateObj;
  int level = -1;
  int strategy = 3;
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_1

  EXPECT_EQ(deflateParams(NULL, level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_2
  deflateInit(deflateObj.get_stream(), 6);
  level = -2;
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_3
  level = 10;
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_4
  level = 0;
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, -1), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_5
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, 5), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateParams_common_6

  deflate_state *state = (deflate_state *)deflateObj.get_stream()->state;
  state->last_flush = 1;
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, 2), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateParams_common_7
  deflateEnd(deflateObj.get_stream());
  uLongf dstLen = 50;
  uLongf srcLen = 10;
  Bytef *dest = (Bytef *)malloc(dstLen);
  Bytef *src = (Bytef *)malloc(srcLen);
  prepare_deflate_stream(deflateObj.get_stream(), dest, &dstLen, src, srcLen, 0);
  state = (deflate_state *)deflateObj.get_stream()->state;
  state->last_flush = 1;
  deflateObj.get_stream()->avail_out = 0;

  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, 3), Z_BUF_ERROR);  // AOCL_Compression_zlib_deflateParams_common_8

  free(dest);
  free(src);
  dest = src = nullptr;
}

TEST_P(AOCL_Compression_zlib, deflateParams_common)
{
  ZLIB_deflate_stream deflateObj;
  int level;
  int strategy;
  int cnt = 0;
  deflateInit(deflateObj.get_stream(), 0);
  deflate_state *state = (deflate_state *)deflateObj.get_stream()->state;

  level = 3;
  strategy = 3;
  state->matches = 1;
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, strategy), Z_OK);  // AOCL_Compression_zlib_deflateParams_common_9
  EXPECT_EQ(state->level, level);
  EXPECT_EQ(state->strategy, strategy);

  state->level = 0;
  level = 1;
  strategy = 2;
  state->matches = 2;
  for (uInt i = 0; i < state->hash_size; i++)
  {
    state->head[i] = 1;
  }
  EXPECT_EQ(deflateParams(deflateObj.get_stream(), level, strategy), Z_OK);  // AOCL_Compression_zlib_deflateParams_common_10
  EXPECT_EQ(state->level, level);
  EXPECT_EQ(state->strategy, strategy);

  for (uInt i = 0; i < state->hash_size; i++)
  {
    cnt += state->head[i] == 0;
  }
  EXPECT_EQ(state->hash_size, cnt);
}

TEST(AOCL_Compression_zlib, deflateTune_negative)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflateTune(deflateObj.get_stream(), 1, 1, 1, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateTune_common_1
  EXPECT_EQ(deflateTune(NULL, 1, 2, 1, 4), Z_STREAM_ERROR);
}

TEST(AOCL_Compression_zlib, deflateTune_common)
{
  ZLIB_deflate_stream deflateObj;
  deflateInit(deflateObj.get_stream(), 4);
  deflate_state *state = deflateObj.get_stream()->state;

  EXPECT_EQ(deflateTune(deflateObj.get_stream(), 1, 2, 3, 4), Z_OK); // AOCL_Compression_zlib_deflateTune_common_2
  EXPECT_EQ(state->good_match, 1);
  EXPECT_EQ(state->max_lazy_match, 2);
  EXPECT_EQ(state->nice_match, 3);
  EXPECT_EQ(state->max_chain_length, 4);
}

TEST(AOCL_Compression_zlib, deflateBound_common)
{
  ZLIB_deflate_stream deflateObj;
  int sourceLen = 1 << 6;

  EXPECT_EQ(deflateBound(deflateObj.get_stream(), sourceLen), 82); // AOCL_Compression_zlib_deflateBound_common_1

  deflateInit(deflateObj.get_stream(), 2);
  deflate_state *state = (deflate_state *)deflateObj.get_stream()->state;
  state->wrap = 3;
  sourceLen = 1 << 25;

  EXPECT_EQ(deflateBound(deflateObj.get_stream(), sourceLen), 33564686); // AOCL_Compression_zlib_deflateBound_common_2

  state->wrap = 0;
  sourceLen = 100;

  EXPECT_EQ(deflateBound(deflateObj.get_stream(), sourceLen), 107);  // AOCL_Compression_zlib_deflateBound_common_3

  state->wrap = 1;

  EXPECT_EQ(deflateBound(deflateObj.get_stream(), sourceLen), 113);  // AOCL_Compression_zlib_deflateBound_common_4

  state->wrap = 1;
  state->strstart = 1;

  EXPECT_EQ(deflateBound(deflateObj.get_stream(), sourceLen), 117);  // AOCL_Compression_zlib_deflateBound_common_5

  //EXPECT_EQ(deflateBound(deflateObj.get_stream(), ULONG_MAX), 4296278157); // deflateBound is undeterministic

  gz_headerp gz = (gz_headerp)malloc(sizeof(gz_header));
  state->gzhead = gz;
  string extra = "extra";
  gz->extra = (Bytef *)extra.c_str();
  gz->extra_len = extra.size();
  string name = "name";
  gz->name = (Bytef *)name.c_str();
  string comment = "comment";
  gz->comment = (Bytef *)comment.c_str();
  gz->hcrc = 1;

  state->wrap = 2;

  EXPECT_EQ(deflateBound(deflateObj.get_stream(), sourceLen), 147);  // AOCL_Compression_zlib_deflateBound_common_6

  state->w_bits = 14;

  EXPECT_EQ(deflateBound(deflateObj.get_stream(), 0), 44); // AOCL_Compression_zlib_deflateBound_common_7

  free(gz);
  gz = nullptr;
}

TEST(AOCL_Compression_zlib, deflatePending_negative)
{
  ZLIB_deflate_stream deflateObj;
  unsigned pending;
  int bits;

  EXPECT_EQ(deflatePending(NULL, &pending, &bits), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflatePending_common_1
  EXPECT_EQ(deflatePending(deflateObj.get_stream(), &pending, &bits), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflatePending_common_2
}

TEST(AOCL_Compression_zlib, deflatePending_common)
{
  ZLIB_deflate_stream deflateObj;
  unsigned pending=0;
  int bits=0;
  deflateInit(deflateObj.get_stream(), 3);

  deflateObj.get_stream()->state->pending = 4;
  deflateObj.get_stream()->state->bi_valid = 9;
  EXPECT_EQ(deflatePending(deflateObj.get_stream(), &pending, &bits), Z_OK); // AOCL_Compression_zlib_deflatePending_common_3
  EXPECT_EQ(pending, 4);
  EXPECT_EQ(bits, 9);

  deflateObj.get_stream()->state->bi_valid = 3;
  EXPECT_EQ(deflatePending(deflateObj.get_stream(), NULL, &bits), Z_OK); //  AOCL_Compression_zlib_deflatePending_common_4
  EXPECT_EQ(bits, 3);

  deflateObj.get_stream()->state->pending = 1;
  EXPECT_EQ(deflatePending(deflateObj.get_stream(), &pending, NULL), Z_OK);  //  AOCL_Compression_zlib_deflatePending_common_5
  EXPECT_EQ(pending, 1);
}

TEST(AOCL_Compression_zlib, deflatePrime_negative)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflatePrime(NULL, 3, 3), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_deflatePrime_common_1
  EXPECT_EQ(deflatePrime(deflateObj.get_stream(), 3, 3), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_deflatePrime_common_2
  deflateInit(deflateObj.get_stream(), 5);
  EXPECT_EQ(deflatePrime(deflateObj.get_stream(), -1, 3), Z_BUF_ERROR);
  EXPECT_EQ(deflatePrime(deflateObj.get_stream(), 17, 3), Z_BUF_ERROR);

  deflateObj.reset_deflate_stream();
  deflateInit(deflateObj.get_stream(), 3);
  #ifdef LIT_MEM
  deflateObj.get_stream()->state->pending_out = (Bytef *)deflateObj.get_stream()->state->d_buf + 10;
  #else
  deflateObj.get_stream()->state->pending_out = (Bytef *)deflateObj.get_stream()->state->sym_buf + 10;
  #endif

  EXPECT_EQ(deflatePrime(deflateObj.get_stream(), 3, 3), Z_BUF_ERROR); //  AOCL_Compression_zlib_deflatePrime_common_3
}

TEST(AOCL_Compression_zlib, deflatePrime_common)
{
  ZLIB_deflate_stream deflateObj;
  deflateInit(deflateObj.get_stream(), 3);

  deflateObj.get_stream()->state->bi_buf = 0;
  deflateObj.get_stream()->state->bi_valid = 0;
  EXPECT_EQ(deflatePrime(deflateObj.get_stream(), 3, 4), Z_OK);  //  AOCL_Compression_zlib_deflatePrime_common_4
  EXPECT_EQ(deflateObj.get_stream()->state->bi_valid, 3);
  EXPECT_EQ(deflateObj.get_stream()->state->bi_buf, 4);

  deflateObj.get_stream()->state->bi_buf = 1;
  deflateObj.get_stream()->state->bi_valid = 8;
  EXPECT_EQ(deflatePrime(deflateObj.get_stream(), 8, 1), Z_OK);  //  AOCL_Compression_zlib_deflatePrime_common_5
  EXPECT_EQ(deflateObj.get_stream()->state->bi_buf, 0);
  EXPECT_EQ(deflateObj.get_stream()->state->bi_valid, 0);
}

TEST(AOCL_Compression_zlib, deflateSetHeader_negative)
{
  ZLIB_deflate_stream deflateObj;
  gz_header head;

  EXPECT_EQ(deflateSetHeader(NULL, &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_1
  EXPECT_EQ(deflateSetHeader(deflateObj.get_stream(), &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_2

  deflateInit(deflateObj.get_stream(), 4);
  deflateObj.get_stream()->state->wrap = 1;

  EXPECT_EQ(deflateSetHeader(deflateObj.get_stream(), &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_3
}

TEST(AOCL_Compression_zlib, deflateSetHeader_common)
{
  ZLIB_deflate_stream deflateObj;
  gz_header head;

  deflateInit(deflateObj.get_stream(), 4);
  deflateObj.get_stream()->state->wrap = 2;
  EXPECT_EQ(deflateSetHeader(deflateObj.get_stream(), &head), Z_OK); //  AOCL_Compression_zlib_deflateSetHeader_common_4
  EXPECT_EQ(deflateObj.get_stream()->state->gzhead, &head);

  deflateObj.get_stream()->state->gzhead = &head;
  deflateObj.get_stream()->state->wrap = 2;
  EXPECT_EQ(deflateSetHeader(deflateObj.get_stream(), NULL), Z_OK);  //  AOCL_Compression_zlib_deflateSetHeader_common_5
  EXPECT_EQ(deflateObj.get_stream()->state->gzhead,(gz_headerp) NULL);
}

TEST_P(AOCL_Compression_zlib, deflateResetKeep_negative)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflateResetKeep(deflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateResetKeep_common_1
  EXPECT_EQ(deflateResetKeep(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateResetKeep_common_2
}

TEST_P(AOCL_Compression_zlib, deflateResetKeep_common)
{
  ZLIB_deflate_stream deflateObj;
  deflateInit(deflateObj.get_stream(), 3);
  deflate_state *state = (deflate_state *)deflateObj.get_stream()->state;

  // 2,1,0
  state->wrap = -2;
  state->last_flush = Z_FINISH;

  EXPECT_EQ(deflateResetKeep(deflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_3
  EXPECT_EQ(state->wrap, 2);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(deflateObj.get_stream()->adler, 0);

  state->wrap = 1;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(deflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_4
  EXPECT_EQ(state->wrap, 1);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);

  state->wrap = 0;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(deflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_5
  EXPECT_EQ(state->wrap, 0);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);

  state->wrap = -100;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(deflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_6
  EXPECT_EQ(state->wrap, 100);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(deflateObj.get_stream()->adler, 1);
}

TEST(AOCL_Compression_zlib, deflate_negative)
{
  ZLIB_deflate_stream deflateObj;

  EXPECT_EQ(deflate(NULL, Z_NO_FLUSH), Z_STREAM_ERROR);

  deflateInit(deflateObj.get_stream(), 4);
  EXPECT_EQ(deflate(deflateObj.get_stream(), -1), Z_STREAM_ERROR);
  EXPECT_EQ(deflate(deflateObj.get_stream(), 6), Z_STREAM_ERROR);

  deflateObj.get_stream()->avail_out = 0;
  Bytef temp_buf = 0xFF;
  deflateObj.get_stream()->next_out = &temp_buf;
  EXPECT_EQ(deflate(deflateObj.get_stream(), Z_BLOCK), Z_BUF_ERROR);
}
/* Test deflate() and inflate() with small buffers */
static z_const char hello[] = "hello, hello!";
static const int hello_len = sizeof(hello);

void deflate_small_buffers_(int level)
{
    z_stream c_strm, d_strm;
    uint8_t compr[128], uncompr[128];
    z_size_t compr_len = sizeof(compr), uncompr_len = sizeof(uncompr);
    int err;

    memset(&c_strm, 0, sizeof(c_strm));
    memset(&d_strm, 0, sizeof(d_strm));

    EXPECT_EQ(deflateInit(&c_strm, level), Z_OK);

    c_strm.next_in  = (z_const unsigned char *)hello;
    c_strm.next_out = compr;

    while (c_strm.total_in != hello_len && c_strm.total_out < compr_len) {
        c_strm.avail_in = c_strm.avail_out = 1; /* force small buffers */
        EXPECT_EQ(deflate(&c_strm, Z_NO_FLUSH), Z_OK);
    }
    /* Finish the stream, still forcing small buffers */
    for (;;) {
        c_strm.avail_out = 1;
        err = deflate(&c_strm, Z_FINISH);
        if (err == Z_STREAM_END) break;
        EXPECT_EQ(err, Z_OK);
    }

    EXPECT_EQ(deflateEnd(&c_strm), Z_OK);

    strcpy((char*)uncompr, "garbage");

    d_strm.next_in  = compr;
    d_strm.next_out = uncompr;

    EXPECT_EQ(inflateInit(&d_strm), Z_OK);

    while (d_strm.total_out < uncompr_len && d_strm.total_in < compr_len) {
        d_strm.avail_in = d_strm.avail_out = 1; /* force small buffers */
        err = inflate(&d_strm, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        EXPECT_EQ(err, Z_OK);
    }

    EXPECT_EQ(inflateEnd(&d_strm), Z_OK);

    EXPECT_STREQ((char*)uncompr, hello);
}

TEST_P(AOCL_Compression_zlib, deflate_small_buffers)
{
    deflate_small_buffers_(1); // validate level 1
    deflate_small_buffers_(Z_DEFAULT_COMPRESSION); // validate level 6
    deflate_small_buffers_(9); // validate level 9
}

/* Test deflate() and inflate() with large buffers */
#define COMPR_BUFFER_SIZE (48 * 1024)
#define UNCOMPR_BUFFER_SIZE (32 * 1024)
#define UNCOMPR_RAND_SIZE (8 * 1024)

void deflate_large_buffers_(int level)
{
  z_stream c_strm, d_strm;
    uint8_t *compr, *uncompr;
    uint32_t compr_len, uncompr_len;
    int32_t i;
    time_t now;
    int err;

    memset(&c_strm, 0, sizeof(c_strm));
    memset(&d_strm, 0, sizeof(d_strm));

    compr = (uint8_t *)calloc(1, COMPR_BUFFER_SIZE);
    ASSERT_TRUE(compr != NULL);
    uncompr = (uint8_t *)calloc(1, UNCOMPR_BUFFER_SIZE);
    ASSERT_TRUE(uncompr != NULL);

    compr_len = COMPR_BUFFER_SIZE;
    uncompr_len = UNCOMPR_BUFFER_SIZE;

    srand((unsigned)time(&now));
    for (i = 0; i < UNCOMPR_RAND_SIZE; i++)
        uncompr[i] = (uint8_t)(rand() % 256);

    EXPECT_EQ(deflateInit(&c_strm, level), Z_OK);

    c_strm.next_out = compr;
    c_strm.avail_out = compr_len;
    c_strm.next_in = uncompr;
    c_strm.avail_in = uncompr_len;

    EXPECT_EQ(deflate(&c_strm, Z_NO_FLUSH), Z_OK);
    EXPECT_EQ(c_strm.avail_in, 0);

    EXPECT_EQ(deflate(&c_strm, Z_FINISH), Z_STREAM_END);

    EXPECT_EQ(deflateEnd(&c_strm), Z_OK);

    d_strm.next_in  = compr;
    d_strm.avail_in = compr_len;
    d_strm.next_out = uncompr;

    EXPECT_EQ(inflateInit(&d_strm), Z_OK);

    for (;;) {
        d_strm.next_out = uncompr;            /* discard the output */
        d_strm.avail_out = uncompr_len;
        err = inflate(&d_strm, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        EXPECT_EQ(err, Z_OK);
    }

    EXPECT_EQ(inflateEnd(&d_strm), Z_OK);

    EXPECT_EQ(d_strm.total_out, uncompr_len);

    free(compr);
    free(uncompr);
    compr = uncompr = nullptr;
}

TEST_P(AOCL_Compression_zlib, deflate_large_buffers)
{
    deflate_large_buffers_(1); // validate level 1
    deflate_large_buffers_(Z_DEFAULT_COMPRESSION); // validate level 6
    deflate_large_buffers_(9); // validate level 9
}

#if defined(AOCL_INTERNAL_TEST)
#define MAX_SEARCH_DIST 32768 // refer deflate_quick.c

#ifdef __cplusplus
extern "C" {
#endif
extern z_const unsigned quick_dist_codes[MAX_SEARCH_DIST];
extern z_const ct_data static_dtree[D_CODES];
extern const int extra_dbits[D_CODES];
extern const int base_dist[D_CODES];
#ifdef __cplusplus
}
#endif

TEST(AOCL_Compression_zlib, deflate_quick_dist_code_verify)
{
  unsigned value = 0;
  for(unsigned i = 0; i < MAX_SEARCH_DIST; i++)
  {
    value = d_code(i);
    value = ( static_dtree[value].fc.code << 8 ) | ( static_dtree[value].dl.len + extra_dbits[value] ) | ( (i - base_dist[value]) << 13);
    EXPECT_EQ(quick_dist_codes[i], value);
  }
}
#endif /* AOCL_INTERNAL_TEST */
