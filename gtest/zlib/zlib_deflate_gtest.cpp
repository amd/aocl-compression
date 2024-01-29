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

TEST(ZLIB_deflateInit, Z_STREAM_ERROR_)
{
  z_streamp zp = get_z_stream();

  EXPECT_EQ(deflateInit(zp, -2), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit_common_4
  EXPECT_EQ(deflateInit(zp, 10), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit_common_5
  EXPECT_EQ(deflateInit(NULL, 3), Z_STREAM_ERROR);// AOCL_Compression_zlib_deflateInit_common_6

  release_deflate_stream(zp);
}

TEST(ZLIB_deflateInit_, fail_cases)
{
  z_streamp zp = get_z_stream();
  char incorrect_version[7] = "0.2.11";
  int windowBits = 3;

  EXPECT_EQ(deflateInit_(NULL, windowBits, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_1
  EXPECT_EQ(deflateInit_(zp, windowBits, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_deflateInit__common_2
  EXPECT_EQ(deflateInit_(zp, windowBits, incorrect_version, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit__common_3
  EXPECT_EQ(deflateInit_(zp, windowBits, ZLIB_VERSION, 2), Z_VERSION_ERROR);  // AOCL_Compression_zlib_deflateInit__common_4
  EXPECT_EQ(deflateInit_(zp, -2, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_5
  EXPECT_EQ(deflateInit_(zp, 10, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_6

  release_deflate_stream(zp);
}

TEST(ZLIB_deflateInit_, pass_cases)
{
  z_streamp zp = get_z_stream();

  EXPECT_EQ(deflateInit_(zp, 0, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);  // AOCL_Compression_zlib_deflateInit__common_7
  
  release_deflate_stream(zp);
  zp = get_z_stream();
  EXPECT_EQ(deflateInit_(zp, -1, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit__common_8
  
  release_deflate_stream(zp);
  zp = get_z_stream();
  EXPECT_EQ(deflateInit_(zp, 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);  // AOCL_Compression_zlib_deflateInit__common_9

  release_deflate_stream(zp);
}

TEST(ZLIB_deflateEnd, Z_OK_)
{
  z_streamp zp = get_z_stream();
  deflateInit(zp, 0);

  EXPECT_EQ(deflateEnd(zp), Z_OK);  // AOCL_Compression_zlib_deflateEnd_common_1

  release_z_stream(zp);
}

TEST(ZLIB_deflateEnd, Z_STREAM_ERROR_)
{
  z_streamp zp = get_z_stream();

  EXPECT_EQ(deflateEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateEnd_common_2
  EXPECT_EQ(deflateEnd(zp), Z_STREAM_ERROR);    // AOCL_Compression_zlib_deflateEnd_common_3

  release_z_stream(zp);
}

TEST(ZLIB_deflateInit2, fail_cases)
{
  z_streamp strm = get_z_stream();
  int memLevel = 5;
  int strategy = 2;

  EXPECT_EQ(deflateInit2(NULL, 6, Z_DEFLATED, 11, memLevel, strategy), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_1
  EXPECT_EQ(deflateInit2(strm, -2, Z_DEFLATED, 11, memLevel, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2_common_2
  EXPECT_EQ(deflateInit2(strm, 10, Z_DEFLATED, 11, memLevel, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2_common_3
  EXPECT_EQ(deflateInit2(strm, 6, 0, 11, memLevel, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2_common_4
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, -8, memLevel, strategy), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_5
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, -16, memLevel, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2_common_6
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, -7, memLevel, strategy), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_7
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 16, memLevel, strategy), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_8
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 23, memLevel, strategy), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_9
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 24, memLevel, strategy), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_10
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 8, 0, strategy), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_11
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 8, 10, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2_common_12
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 8, memLevel, -1), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2_common_13
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 8, memLevel, 5), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2_common_14

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateInit2, pass_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 8, 3, 3), Z_OK);  // AOCL_Compression_zlib_deflateInit2_common_15
  
  release_deflate_stream(strm);
  strm = get_z_stream();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 15, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_16
  
  release_deflate_stream(strm);
  strm = get_z_stream();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 25, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_17
  
  release_deflate_stream(strm);
  strm = get_z_stream();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 31, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_18
  
  release_deflate_stream(strm);
  strm = get_z_stream();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, -9, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_19
  
  release_deflate_stream(strm);
  strm = get_z_stream();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, -15, 3, 3), Z_OK);// AOCL_Compression_zlib_deflateInit2_common_20

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateInit2_, fail_cases)
{
  z_streamp strm = get_z_stream();
  const int memLevel = 5;
  const int strategy = 2;
  const int windowBits = 11;

  EXPECT_EQ(deflateInit2_(NULL, 6, Z_DEFLATED, windowBits, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_1
  EXPECT_EQ(deflateInit2_(strm, -2, Z_DEFLATED, windowBits, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_2
  EXPECT_EQ(deflateInit2_(strm, 23, Z_DEFLATED, windowBits, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_3
  EXPECT_EQ(deflateInit2_(strm, 6, 0, windowBits, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_4
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, -8, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_5
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, -16, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_6
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, -7, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_7
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, 16, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_8
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, 23, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_9
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, 24, memLevel, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_10
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, windowBits, 0, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_11
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, windowBits, 10, strategy, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_12
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, windowBits, memLevel, -1, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit2__common_13
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, windowBits, memLevel, 5, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_14
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, windowBits, memLevel, strategy, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_15
  char c[2] = "0";
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, windowBits, memLevel, strategy, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit2__common_16
  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, windowBits, memLevel, strategy, ZLIB_VERSION, 3), Z_VERSION_ERROR);  // AOCL_Compression_zlib_deflateInit2__common_17

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateInit2_, pass_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, 8, 3, 3, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);  // AOCL_Compression_zlib_deflateInit2__common_18
  EXPECT_EQ(strm->state->level, 6);
  EXPECT_EQ(strm->state->strategy, 3);
  EXPECT_EQ(strm->state->method, Z_DEFLATED);

  release_deflate_stream(strm);
  strm = get_z_stream();

  EXPECT_EQ(deflateInit2_(strm, 7, Z_DEFLATED, 25, 3, 4, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit2__common_19
  EXPECT_EQ(strm->state->level, 7);
  EXPECT_EQ(strm->state->strategy, 4);

  release_deflate_stream(strm);
}

/*
TEST(ZLIB_deflate, fail_cases)
{
  z_streamp strm = get_z_stream();
  const uLong srcLen = 10;
  const char c[srcLen + 1] = "helloWorld";
  Bytef *src = (Bytef *)c;
  uLong dstLen = 20;
  char cc[dstLen];
  Bytef *dst = (Bytef *)cc;
  prepare_deflate_stream(strm, dst, &dstLen, src, srcLen, 3);

  EXPECT_EQ(deflate(NULL, Z_NO_FLUSH), Z_STREAM_ERROR);
  EXPECT_EQ(deflate(strm, 6), Z_STREAM_ERROR);
  EXPECT_EQ(deflate(strm, -1), Z_STREAM_ERROR);

  Bytef *temp = strm->next_out;
  strm->next_out = NULL;

  EXPECT_EQ(deflate(strm, Z_NO_FLUSH), Z_STREAM_ERROR);

  strm->next_out = temp;
  temp = strm->next_in;
  strm->next_in = NULL;

  EXPECT_EQ(deflate(strm, Z_NO_FLUSH), Z_STREAM_ERROR);

  strm->next_in = temp;
  z_streamp z_stream_temp = new z_stream;
  memcpy(z_stream_temp, strm, sizeof(z_stream));

  // strm->avail
  free(z_stream_temp);
  release_deflate_stream(strm);
}

TEST(ZLIB_deflate, pass_cases)
{
  z_streamp strm = get_z_stream();
  const uLong srcLen = 10;
  char c[srcLen + 1] = "helloWorld";
  Bytef *src = (Bytef *)c;
  uLong dstLen = 20;
  char cc[dstLen];
  Bytef *dst = (Bytef *)cc;
  prepare_deflate_stream(strm, dst, &dstLen, src, srcLen, 3);

  EXPECT_EQ(deflate(strm, Z_NO_FLUSH), Z_OK);

  deflateEnd(strm);
  release_z_stream(strm);
}
*/

TEST(ZLIB_deflateSetDictionary, fail_cases)
{
  z_streamp strm = get_z_stream();
  const int sDictLen = 10;
  char c[sDictLen + 1] = "abcdefghij";
  Bytef *sDict = (Bytef *)c;

  // AOCL_Compression_zlib_deflateSetDictionary_common_1
  EXPECT_EQ(deflateSetDictionary(strm, sDict, sDictLen), Z_STREAM_ERROR); // stream uninitialized

  deflateInit(strm, 4);

  // AOCL_Compression_zlib_deflateSetDictionary_common_2
  EXPECT_EQ(deflateSetDictionary(strm, NULL, sDictLen), Z_STREAM_ERROR); // dictionary null

  deflateEnd(strm);
  memset(strm, 0, sizeof(z_stream));
  deflateInit2(strm, 4, 8, 26, 4, 4);

  // AOCL_Compression_zlib_deflateSetDictionary_common_3
  EXPECT_EQ(deflateSetDictionary(strm, sDict, sDictLen), Z_STREAM_ERROR); // wrap == 2 due to windowBits==26

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateSetDictionary, pass_case)
{
  z_streamp strm = get_z_stream();
  Bytef *sDict;
  const uLong sDictLen = 4000;
  char cc[sDictLen];
  for (uLong i = 0; i < sDictLen; i++)
  {
    cc[i] = rand() % 255;
  }
  deflateInit(strm, 4);
  sDict = (Bytef *)cc;

  // AOCL_Compression_zlib_deflateSetDictionary_common_4
  EXPECT_EQ(deflateSetDictionary(strm, sDict, 10), Z_OK); // wrap==1

  deflateEnd(strm);
  deflateInit2(strm, 4, 8, -9, 4, 4);

  // AOCL_Compression_zlib_deflateSetDictionary_common_5
  EXPECT_EQ(deflateSetDictionary(strm, sDict, sDictLen), Z_OK); // wrap==0

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateGetDictionary, fail_cases)
{
  Bytef *dict = nullptr;
  uInt dictLen;
  z_streamp strm = get_z_stream();

  EXPECT_EQ(deflateGetDictionary(strm, dict, &dictLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateGetDictionary_common_1
  EXPECT_EQ(deflateGetDictionary(NULL, dict, &dictLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateGetDictionary_common_2

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateGetDictionary, pass_cases)
{
  Bytef *dict = (Bytef *)malloc(sizeof(Bytef) * 40000);
  uInt dictLen;
  z_streamp strm = get_z_stream();
  deflateInit(strm, 4);
  const int sDictLen = 10;
  char c[sDictLen + 1] = "abcdefghij";
  Bytef *sDict = (Bytef *)c;

  deflateSetDictionary(strm, sDict, sDictLen);

  EXPECT_EQ(deflateGetDictionary(strm, dict, &dictLen), Z_OK);  // AOCL_Compression_zlib_deflateGetDictionary_common_3
  EXPECT_EQ(dictLen, sDictLen);
  EXPECT_TRUE(cmpr(c, (char *)dict, dictLen));

  deflateEnd(strm);
  release_z_stream(strm);
  free(dict);
  dict = nullptr;
}

TEST(ZLIB_deflateCopy, fail_cases)
{
  z_streamp dest = get_z_stream();
  z_streamp src = get_z_stream();

  EXPECT_EQ(deflateCopy(dest, NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateCopy_common_1
  EXPECT_EQ(deflateCopy(NULL, src), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateCopy_common_2
  EXPECT_EQ(deflateCopy(dest, src), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateCopy_common_3

  release_z_stream(dest);
  release_z_stream(src);
  // deflateCopy()
}

TEST(ZLIB_deflateCopy, pass_case)
{
  z_streamp dest = get_z_stream();
  z_streamp src = get_z_stream();
  deflateInit(src, 5);

  EXPECT_EQ(deflateCopy(dest, src), Z_OK);  // AOCL_Compression_zlib_deflateCopy_common_4

  release_deflate_stream(dest);
  release_deflate_stream(src);
}

TEST(ZLIB_deflateReset, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(deflateReset(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateReset_common_1
  EXPECT_EQ(deflateReset(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateReset_common_2

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateReset, pass_cases)
{
  z_streamp strm = get_z_stream();
  deflateInit(strm, 4);

  EXPECT_EQ(deflateReset(strm), Z_OK);  // AOCL_Compression_zlib_deflateReset_common_3
  
  EXPECT_EQ(strm->total_in, 0);
  EXPECT_EQ(strm->total_out, 0);
  EXPECT_EQ(strm->msg, (const char *)Z_NULL);
  EXPECT_EQ(strm->data_type, Z_UNKNOWN);
  EXPECT_EQ(strm->state->pending, 0);
  EXPECT_EQ(strm->state->last_flush, -2);

  release_deflate_stream(strm);
}

class ZLIB_deflateParams : public AOCL_setup_zlib {
};

TEST_F(ZLIB_deflateParams, fail_cases)
{
  z_streamp strm = get_z_stream();
  int level = -1;
  int strategy = 3;
  EXPECT_EQ(deflateParams(strm, level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_1

  EXPECT_EQ(deflateParams(NULL, level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_2
  deflateInit(strm, 6);
  level = -2;
  EXPECT_EQ(deflateParams(strm, level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_3
  level = 10;
  EXPECT_EQ(deflateParams(strm, level, strategy), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_4
  level = 0;
  EXPECT_EQ(deflateParams(strm, level, -1), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateParams_common_5
  EXPECT_EQ(deflateParams(strm, level, 5), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateParams_common_6

  deflate_state *state = (deflate_state *)strm->state;
  state->last_flush = 1;
  EXPECT_EQ(deflateParams(strm, level, 2), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateParams_common_7
  deflateEnd(strm);
  uLongf dstLen = 50;
  uLongf srcLen = 10;
  Bytef *dest = (Bytef *)malloc(dstLen);
  Bytef *src = (Bytef *)malloc(srcLen);
  prepare_deflate_stream(strm, dest, &dstLen, src, srcLen, 0);
  state = (deflate_state *)strm->state;
  state->last_flush = 1;
  strm->avail_out = 0;

  EXPECT_EQ(deflateParams(strm, level, 3), Z_BUF_ERROR);  // AOCL_Compression_zlib_deflateParams_common_8

  free(dest);
  free(src);
  dest = src = nullptr;
  release_deflate_stream(strm);
}

TEST_F(ZLIB_deflateParams, pass_cases)
{
  z_streamp strm = get_z_stream();
  int level;
  int strategy;
  int cnt = 0;
  deflateInit(strm, 0);
  deflate_state *state = (deflate_state *)strm->state;

  level = 3;
  strategy = 3;
  state->matches = 1;
  EXPECT_EQ(deflateParams(strm, level, strategy), Z_OK);  // AOCL_Compression_zlib_deflateParams_common_9
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
  EXPECT_EQ(deflateParams(strm, level, strategy), Z_OK);  // AOCL_Compression_zlib_deflateParams_common_10
  EXPECT_EQ(state->level, level);
  EXPECT_EQ(state->strategy, strategy);

  for (uInt i = 0; i < state->hash_size; i++)
  {
    cnt += state->head[i] == 0;
  }
  EXPECT_EQ(state->hash_size, cnt);

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateTune, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(deflateTune(strm, 1, 1, 1, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateTune_common_1

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateTune, pass_case)
{
  z_streamp strm = get_z_stream();
  deflateInit(strm, 4);
  deflate_state *state = strm->state;

  EXPECT_EQ(deflateTune(strm, 1, 2, 3, 4), Z_OK); // AOCL_Compression_zlib_deflateTune_common_2
  EXPECT_EQ(state->good_match, 1);
  EXPECT_EQ(state->max_lazy_match, 2);
  EXPECT_EQ(state->nice_match, 3);
  EXPECT_EQ(state->max_chain_length, 4);

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateBound, all_cases)
{
  z_streamp strm = get_z_stream();
  int sourceLen = 1 << 6;

  EXPECT_EQ(deflateBound(strm, sourceLen), 82); // AOCL_Compression_zlib_deflateBound_common_1

  deflateInit(strm, 2);
  deflate_state *state = (deflate_state *)strm->state;
  state->wrap = 3;
  sourceLen = 1 << 25;

  EXPECT_EQ(deflateBound(strm, sourceLen), 33564686); // AOCL_Compression_zlib_deflateBound_common_2

  state->wrap = 0;
  sourceLen = 100;

  EXPECT_EQ(deflateBound(strm, sourceLen), 107);  // AOCL_Compression_zlib_deflateBound_common_3

  state->wrap = 1;

  EXPECT_EQ(deflateBound(strm, sourceLen), 113);  // AOCL_Compression_zlib_deflateBound_common_4

  state->wrap = 1;
  state->strstart = 1;

  EXPECT_EQ(deflateBound(strm, sourceLen), 117);  // AOCL_Compression_zlib_deflateBound_common_5

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

  EXPECT_EQ(deflateBound(strm, sourceLen), 147);  // AOCL_Compression_zlib_deflateBound_common_6

  state->w_bits = 14;

  EXPECT_EQ(deflateBound(strm, 0), 44); // AOCL_Compression_zlib_deflateBound_common_7

  free(gz);
  gz = nullptr;
  release_deflate_stream(strm);
  // s->gzhead!=NULL user supplied gzip header
}

TEST(ZLIB_deflatePending, fail_cases)
{
  z_streamp strm = get_z_stream();
  unsigned pending;
  int bits;

  EXPECT_EQ(deflatePending(NULL, &pending, &bits), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflatePending_common_1
  EXPECT_EQ(deflatePending(strm, &pending, &bits), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflatePending_common_2

  release_deflate_stream(strm);
}

TEST(ZLIB_deflatePending, pass_cases)
{
  z_streamp strm = get_z_stream();
  unsigned pending=0;
  int bits=0;
  deflateInit(strm, 3);

  strm->state->pending = 4;
  strm->state->bi_valid = 9;
  EXPECT_EQ(deflatePending(strm, &pending, &bits), Z_OK); // AOCL_Compression_zlib_deflatePending_common_3
  EXPECT_EQ(pending, 4);
  EXPECT_EQ(bits, 9);

  strm->state->bi_valid = 3;
  EXPECT_EQ(deflatePending(strm, NULL, &bits), Z_OK); //  AOCL_Compression_zlib_deflatePending_common_4
  EXPECT_EQ(bits, 3);

  strm->state->pending = 1;
  EXPECT_EQ(deflatePending(strm, &pending, NULL), Z_OK);  //  AOCL_Compression_zlib_deflatePending_common_5
  EXPECT_EQ(pending, 1);

  release_deflate_stream(strm);
}

TEST(ZLIB_deflatePrime, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(deflatePrime(NULL, 3, 3), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_deflatePrime_common_1
  EXPECT_EQ(deflatePrime(strm, 3, 3), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_deflatePrime_common_2

  deflateInit(strm, 3);
  #ifdef LIT_MEM
  strm->state->pending_out = (Bytef *)strm->state->d_buf + 10;
  #else
  strm->state->pending_out = (Bytef *)strm->state->sym_buf + 10;
  #endif

  EXPECT_EQ(deflatePrime(strm, 3, 3), Z_BUF_ERROR); //  AOCL_Compression_zlib_deflatePrime_common_3

  release_deflate_stream(strm);
}

TEST(ZLIB_deflatePrime, pass_cases)
{
  z_streamp strm = get_z_stream();
  deflateInit(strm, 3);

  strm->state->bi_buf = 0;
  strm->state->bi_valid = 0;
  EXPECT_EQ(deflatePrime(strm, 3, 4), Z_OK);  //  AOCL_Compression_zlib_deflatePrime_common_4
  EXPECT_EQ(strm->state->bi_valid, 3);
  EXPECT_EQ(strm->state->bi_buf, 4);

  strm->state->bi_buf = 1;
  strm->state->bi_valid = 8;
  EXPECT_EQ(deflatePrime(strm, 8, 1), Z_OK);  //  AOCL_Compression_zlib_deflatePrime_common_5
  EXPECT_EQ(strm->state->bi_buf, 0);
  EXPECT_EQ(strm->state->bi_valid, 0);

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateSetHeader, fail_cases)
{
  z_streamp strm = get_z_stream();
  gz_header head;

  EXPECT_EQ(deflateSetHeader(NULL, &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_1
  EXPECT_EQ(deflateSetHeader(strm, &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_2

  deflateInit(strm, 4);
  strm->state->wrap = 1;

  EXPECT_EQ(deflateSetHeader(strm, &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_3

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateSetHeader, pass)
{
  z_streamp strm = get_z_stream();
  gz_header head;

  deflateInit(strm, 4);
  strm->state->wrap = 2;
  EXPECT_EQ(deflateSetHeader(strm, &head), Z_OK); //  AOCL_Compression_zlib_deflateSetHeader_common_4
  EXPECT_EQ(strm->state->gzhead, &head);

  strm->state->gzhead = &head;
  strm->state->wrap = 2;
  EXPECT_EQ(deflateSetHeader(strm, NULL), Z_OK);  //  AOCL_Compression_zlib_deflateSetHeader_common_5
  EXPECT_EQ(strm->state->gzhead,(gz_headerp) NULL);

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateResetKeep, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(deflateResetKeep(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateResetKeep_common_1
  EXPECT_EQ(deflateResetKeep(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateResetKeep_common_2

  release_deflate_stream(strm);
}

TEST(ZLIB_deflateResetKeep, pass_cases)
{
  z_streamp strm = get_z_stream();
  deflateInit(strm, 3);
  deflate_state *state = (deflate_state *)strm->state;

  // 2,1,0
  state->wrap = -2;
  state->last_flush = Z_FINISH;

  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_3
  EXPECT_EQ(state->wrap, 2);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(strm->adler, 0);

  state->wrap = 1;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_4
  EXPECT_EQ(state->wrap, 1);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(strm->adler, 1);

  state->wrap = 0;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_5
  EXPECT_EQ(state->wrap, 0);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(strm->adler, 1);

  state->wrap = -100;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_6
  EXPECT_EQ(state->wrap, 100);
  EXPECT_EQ(state->last_flush, -2);
  EXPECT_EQ(strm->adler, 1);

  release_deflate_stream(strm);
}

/* Test deflate() and inflate() with small buffers */
static z_const char hello[] = "hello, hello!";
static const int hello_len = sizeof(hello);

class ZLIB_deflate : public AOCL_setup_zlib {
};

TEST_F(ZLIB_deflate, AOCL_Compression_zlib_deflate_small_buffers_1) {
    z_stream c_strm, d_strm;
    uint8_t compr[128], uncompr[128];
    z_size_t compr_len = sizeof(compr), uncompr_len = sizeof(uncompr);
    int err;

    memset(&c_strm, 0, sizeof(c_strm));
    memset(&d_strm, 0, sizeof(d_strm));

    EXPECT_EQ(deflateInit(&c_strm, 1), Z_OK);

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

TEST_F(ZLIB_deflate, AOCL_Compression_zlib_deflate_small_buffers_2) {
    z_stream c_strm, d_strm;
    uint8_t compr[128], uncompr[128];
    z_size_t compr_len = sizeof(compr), uncompr_len = sizeof(uncompr);
    int err;

    memset(&c_strm, 0, sizeof(c_strm));
    memset(&d_strm, 0, sizeof(d_strm));

    EXPECT_EQ(deflateInit(&c_strm, Z_DEFAULT_COMPRESSION), Z_OK);

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

TEST_F(ZLIB_deflate, AOCL_Compression_zlib_deflate_small_buffers_3) {
    z_stream c_strm, d_strm;
    uint8_t compr[128], uncompr[128];
    z_size_t compr_len = sizeof(compr), uncompr_len = sizeof(uncompr);
    int err;

    memset(&c_strm, 0, sizeof(c_strm));
    memset(&d_strm, 0, sizeof(d_strm));

    EXPECT_EQ(deflateInit(&c_strm, 9), Z_OK);

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

/* Test deflate() and inflate() with large buffers */
#define COMPR_BUFFER_SIZE (48 * 1024)
#define UNCOMPR_BUFFER_SIZE (32 * 1024)
#define UNCOMPR_RAND_SIZE (8 * 1024)

TEST_F(ZLIB_deflate, AOCL_Compression_zlib_deflate_large_buffers_1)
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

    EXPECT_EQ(deflateInit(&c_strm, 1), Z_OK);

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

TEST_F(ZLIB_deflate, AOCL_Compression_zlib_deflate_large_buffers_2)
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

    EXPECT_EQ(deflateInit(&c_strm, Z_DEFAULT_COMPRESSION), Z_OK);

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

TEST_F(ZLIB_deflate, AOCL_Compression_zlib_deflate_large_buffers_3)
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

    EXPECT_EQ(deflateInit(&c_strm, 9), Z_OK);

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

#ifdef AOCL_ZLIB_DEFLATE_FAST_MODE
TEST(ZLIB_deflate_quick, AOCL_Compression_zlib_defqck_dist_code_verify)
{
  EXPECT_EQ(Test_quick_dist_code(),0);
}
#endif /* AOCL_ZLIB_DEFLATE_FAST_MODE */
