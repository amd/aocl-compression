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

/** @file zlib_inflate_gtest.cc
 *
 *  @brief Test cases for ZLIB algo.
 *
 *  This file contains the test cases for ZLIB method
 *  testing the API level inflate based functions of ZLIB.
 *
 *  @author  Ravi Jangra
 */

#include "zlib_gtest.h"

TEST(ZLIB_inflateInit, Z_OK_)
{
  z_streamp zp = get_z_stream();

  EXPECT_EQ(inflateInit(zp), Z_OK); // AOCL_Compression_zlib_inflateInit_common_1

  release_inflate_stream(zp);
}

TEST(ZLIB_inflateInit, Z_STREAM_ERROR_)
{
  EXPECT_EQ(inflateInit(NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateInit_common_2
}

TEST(ZLIB_inflateInit_, fail_cases)
{
  z_streamp zp = get_z_stream();
  char c[7] = "0.2.11";

  EXPECT_EQ(inflateInit_(NULL, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateInit__common_1
  EXPECT_EQ(inflateInit_(zp, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateInit__common_2
  EXPECT_EQ(inflateInit_(zp, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_inflateInit__common_3
  EXPECT_EQ(inflateInit_(zp, ZLIB_VERSION, 3), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateInit__common_4

  release_inflate_stream(zp);
}

TEST(ZLIB_inflateInit_, pass_cases)
{
  z_streamp zp = get_z_stream();

  EXPECT_EQ(inflateInit_(zp, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_inflateInit__common_5

  release_inflate_stream(zp);
}

TEST(ZLIB_inflateEnd, Z_OK_)
{
  z_streamp zp = get_z_stream();
  inflateInit(zp);

  EXPECT_EQ(inflateEnd(zp), Z_OK);  // AOCL_Compression_zlib_inflateEnd_common_1

  release_z_stream(zp);
}

TEST(ZLIB_inflateEnd, Z_STREAM_ERROR_)
{
  z_streamp zp = get_z_stream();

  EXPECT_EQ(inflateEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateEnd_common_2
  EXPECT_EQ(inflateEnd(zp), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateEnd_common_3

  release_z_stream(zp);
}

TEST(ZLIB_inflateInit2, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateInit2(NULL, 9), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_1
  EXPECT_EQ(inflateInit2(strm, 7), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_2
  EXPECT_EQ(inflateInit2(strm, 17), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_3

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateInit2, pass_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateInit2(strm, 9), Z_OK); //  AOCL_Compression_zlib_inflateInit2_common_4

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateInit2_, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateInit2_(NULL, 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_1
  EXPECT_EQ(inflateInit2_(strm, 9, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_2

  char c[7] = "0.2.11";

  EXPECT_EQ(inflateInit2_(strm, 9, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_3
  EXPECT_EQ(inflateInit2_(strm, 9, ZLIB_VERSION, 3), Z_VERSION_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_4

  EXPECT_EQ(inflateInit2_(strm, 7, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_5
  EXPECT_EQ(inflateInit2_(strm, 17, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_6

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateInit2_, pass_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateInit2_(strm, 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); //  AOCL_Compression_zlib_inflateInit2__common_7

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateSetDictionary, fail_cases)
{
  z_streamp strm = get_z_stream();
  const uInt dictLen = 10;
  Bytef dictionary[dictLen + 1] = "helloWorld";

  EXPECT_EQ(inflateSetDictionary(NULL, dictionary, dictLen), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateSetDictionary_common_1

  inflateInit(strm);
  inflate_state *s = (inflate_state *)strm->state;
  s->mode = HEAD;
  strm->state->wrap = 2;

  EXPECT_EQ(inflateSetDictionary(strm, dictionary, dictLen), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateSetDictionary_common_2

  inflateReset2(strm, -9);
  s->mode = DICT;
  s->check = 0;

  EXPECT_EQ(inflateSetDictionary(strm, dictionary, dictLen), Z_DATA_ERROR); //  AOCL_Compression_zlib_inflateSetDictionary_common_3

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateSetDictionary, pass_cases)
{
  z_streamp strm = get_z_stream();
  const uInt dictLen = 10;
  Bytef dictionary[dictLen + 1] = "helloWorld";
  inflateInit(strm);
  inflate_state *s = (inflate_state *)strm->state;
  s->wrap = 0;

  EXPECT_EQ(inflateSetDictionary(strm, dictionary, dictLen), Z_OK); //  AOCL_Compression_zlib_inflateSetDictionary_common_4
  EXPECT_TRUE(memcmp( s->window, dictionary, dictLen)==0);

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateGetDictionary, fail_cases)
{
  z_streamp strm = get_z_stream();
  const uInt dictAlloc = 10;
  uInt dictLen = dictAlloc;
  char dict[dictAlloc];

  EXPECT_EQ(inflateGetDictionary(strm, (Bytef *)dict, &dictLen), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateGetDictionary_common_1

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateGetDicitonary, pass_cases)
{
  z_streamp strm = get_z_stream();
  const uInt dictLen = 10;
  char dict[dictLen + 1] = "helloWorld";
  uInt destLen;
  char dest[dictLen];
  inflateInit(strm);

  EXPECT_EQ(inflateGetDictionary(strm, (Bytef *)dest, NULL), Z_OK); //  AOCL_Compression_zlib_inflateGetDictionary_common_2
  EXPECT_EQ(inflateGetDictionary(strm, NULL, &destLen), Z_OK);      //  AOCL_Compression_zlib_inflateGetDictionary_common_3

  EXPECT_EQ(inflateGetDictionary(strm, (Bytef *)dest, &destLen), Z_OK); //  AOCL_Compression_zlib_inflateGetDictionary_common_4

  inflate_state *s = (inflate_state *)strm->state;
  s->wrap = 0;
  inflateSetDictionary(strm, (Bytef *)dict, dictLen);

  EXPECT_EQ(inflateGetDictionary(strm, (Bytef *)dest, &destLen), Z_OK); //  AOCL_Compression_zlib_inflateGetDictionary_common_5
  EXPECT_EQ(dictLen, destLen);
  EXPECT_TRUE(cmpr(dict, dest, dictLen));

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateSync, failed_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateSync(strm), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateSync_common_1
  EXPECT_EQ(inflateSync(NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateSync_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateSync(strm), Z_BUF_ERROR);  // AOCL_Compression_zlib_inflateSync_common_3

  inflate_state *s = (inflate_state *)strm->state;
  s->bits = 9;

  EXPECT_EQ(inflateSync(strm), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateSync_common_4

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateSync, pass_cases)
{
  z_streamp strm = get_z_stream();
  inflateInit(strm);
  inflate_state *s = (inflate_state *)strm->state;
  s->bits = 9;
  char c[4];
  c[0] = c[1] = (char)0;
  c[2] = c[3] = (char)0xff;
  strm->next_in = (Bytef *)c;
  strm->avail_in = 4;

  EXPECT_EQ(inflateSync(strm), Z_OK); // AOCL_Compression_zlib_inflateSync_common_5

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateCopy, fail_cases)
{
  z_streamp strm = get_z_stream();
  z_streamp dest = new z_stream;

  EXPECT_EQ(inflateCopy(dest, NULL), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_1
  EXPECT_EQ(inflateCopy(dest, strm), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateCopy(NULL, strm), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_3

  delete dest;
  dest = nullptr;
  release_inflate_stream(strm);
}

TEST(ZLIB_inflateCopy, pass_cases)
{
  z_streamp strm = get_z_stream();
  z_streamp dest = get_z_stream();
  inflateInit(strm);
  EXPECT_EQ(inflateCopy(dest, strm), Z_OK); //  AOCL_Compression_zlib_inflateCopy_common_4

  release_inflate_stream(dest);
  dest = get_z_stream();

  inflate_state *state = (inflate_state *)strm->state;
  state->wbits = 3;
  char c[9] = "abcdefgh";
  state->window = (Bytef *)malloc(sizeof(char) * 8);
  memcpy(state->window, c, 8);
  EXPECT_EQ(inflateCopy(dest, strm), Z_OK); //  AOCL_Compression_zlib_inflateCopy_common_5

  inflate_state *dstate = (inflate_state *)dest->state;
  EXPECT_EQ(dstate->wbits, state->wbits);
  EXPECT_EQ(memcmp(dstate->window, state->window, 1U << state->wbits), 0);
  dest->state = strm->state;
  EXPECT_EQ(memcmp(strm, dest, sizeof(z_stream)), 0);
  dest->state = (internal_state *)dstate;

  inflate_state *temp_dstate = new inflate_state;
  memcpy(temp_dstate, dstate, sizeof(inflate_state));
  dstate->strm = state->strm;
  dstate->next = state->next;
  dstate->window = state->window;
  dstate->lencode = state->lencode;
  dstate->distcode = state->distcode;
  EXPECT_EQ(memcmp(strm->state, dest->state, sizeof(inflate_state)), 0);
  memcpy(dstate, temp_dstate, sizeof(inflate_state));

  dstate = state = nullptr;
  release_inflate_stream(dest);
  release_inflate_stream(strm);
  delete temp_dstate;
  temp_dstate = nullptr;
}

TEST(ZLIB_inflateReset, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateReset(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateReset_common_1
  EXPECT_EQ(inflateReset(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateReset_common_2

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateReset, pass_cases)
{
  z_streamp strm = get_z_stream();
  inflateInit(strm);

  EXPECT_EQ(inflateReset(strm), Z_OK);  // AOCL_Compression_zlib_inflateReset_common_3

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateReset2, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateReset2(strm, 9), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_1
  EXPECT_EQ(inflateReset2(NULL, 9), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateReset2(strm, -7), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateReset2_common_3
  EXPECT_EQ(inflateReset2(strm, -16), Z_STREAM_ERROR);//  AOCL_Compression_zlib_inflateReset2_common_4
  EXPECT_EQ(inflateReset2(strm, 48), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateReset2_common_5
  EXPECT_EQ(inflateReset2(strm, 7), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_6

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateReset2, pass_cases)
{
  z_streamp strm = get_z_stream();
  inflateInit(strm);
  inflate_state *s = (inflate_state *)strm->state;
  s->wbits = 3;
  s->window = (unsigned char *)malloc(8);
  EXPECT_EQ(inflateReset2(strm, 8), Z_OK);      //  AOCL_Compression_zlib_inflateReset2_common_7
  EXPECT_EQ(s->window, (unsigned char *) NULL);
  EXPECT_EQ(s->wbits, 8);

  release_inflate_stream(strm);
}

TEST(ZLIB_inflatePrime, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflatePrime(NULL, 5, 5), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflatePrime_common_1
  EXPECT_EQ(inflatePrime(strm, 5, 5), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflatePrime_common_2
  
  inflateInit(strm);
  EXPECT_EQ(inflatePrime(strm, 17, 4), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflatePrime_common_3

  inflate_state *state = (inflate_state *)strm->state;
  state->bits = 25;

  EXPECT_EQ(inflatePrime(strm, 10, (1 << 10) - 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflatePrime_common_4

  release_inflate_stream(strm);
}

TEST(ZLIB_inflatePrime, pass_cases)
{
  z_streamp strm = get_z_stream();
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;

  EXPECT_EQ(inflatePrime(strm, -1, 4), Z_OK); // AOCL_Compression_zlib_inflatePrime_common_5
  EXPECT_EQ(state->hold, 0);
  EXPECT_EQ(state->bits, 0);

  state->bits = 2;

  EXPECT_EQ(inflatePrime(strm, 3, 3), Z_OK);  // AOCL_Compression_zlib_inflatePrime_common_6
  EXPECT_EQ(state->hold, 12);
  EXPECT_EQ(state->bits, 5);

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateMark, all_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateMark(strm), -(1L << 16));  // AOCL_Compression_zlib_inflateMark_common_1
  EXPECT_EQ(inflateMark(NULL), -(1L << 16));  // AOCL_Compression_zlib_inflateMark_common_2

  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;

  state->back = 1;
  state->mode = TABLE;
  EXPECT_EQ(inflateMark(strm), 1 << 16);  // AOCL_Compression_zlib_inflateMark_common_3

  state->back = 0;
  state->mode = COPY;
  state->length = 100;
  EXPECT_EQ(inflateMark(strm), 100);  // AOCL_Compression_zlib_inflateMark_common_4

  state->mode = MATCH;
  state->length = 2;
  state->was = 100;
  EXPECT_EQ(inflateMark(strm), 98); // AOCL_Compression_zlib_inflateMark_common_5

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateGetHeader, fail_cases)
{
  z_streamp strm = get_z_stream();
  gz_headerp gz = (gz_headerp)malloc(sizeof(gz_header));

  EXPECT_EQ(inflateGetHeader(NULL, gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_1
  EXPECT_EQ(inflateGetHeader(strm, gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_2

  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;
  state->wrap = 0;

  EXPECT_EQ(inflateGetHeader(strm, gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_3

  free(gz);
  gz = nullptr;
  release_inflate_stream(strm);
}

TEST(ZLIB_inflateGetHeader, pass_cases)
{
  z_streamp strm = get_z_stream();
  gz_headerp gz = (gz_headerp)malloc(sizeof(gz_header));
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;
  state->wrap = 2;
  gz->done = 1;

  EXPECT_EQ(inflateGetHeader(strm, gz), Z_OK);  // AOCL_Compression_zlib_inflateGetHeader_common_4
  EXPECT_EQ(state->head, gz);
  EXPECT_EQ(gz->done, 0);

  free(gz);
  gz = nullptr;
  release_inflate_stream(strm);
}

TEST(ZLIB_inflateBackInit, fail_cases)
{
  z_streamp strm = get_z_stream();
  const int windowBits = 12;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit(NULL, windowBits, window), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit_common_1
  EXPECT_EQ(inflateBackInit(strm, 7, window), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit_common_2
  EXPECT_EQ(inflateBackInit(strm, 16, window), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit_common_3
  EXPECT_EQ(inflateBackInit(strm, 9, NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit_common_4

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateBackInit, pass_cases)
{
  z_streamp strm = get_z_stream();
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit(strm, windowBits, window), Z_OK); // AOCL_Compression_zlib_inflateBackInit_common_5

  inflateBackEnd(strm);
  release_z_stream(strm);
}

TEST(ZLIB_inflateBackInit_, fail_cases)
{
  z_streamp strm = get_z_stream();
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit_(NULL, windowBits, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit__common_1
  EXPECT_EQ(inflateBackInit_(strm, 7, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_2
  EXPECT_EQ(inflateBackInit_(strm, 16, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit__common_3
  EXPECT_EQ(inflateBackInit_(strm, 9, NULL, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_4

  char c[2] = "0";

  EXPECT_EQ(inflateBackInit_(strm, windowBits, window, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_5
  EXPECT_EQ(inflateBackInit_(strm, windowBits, window, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_inflateBackInit__common_6
  EXPECT_EQ(inflateBackInit_(strm, windowBits, window, ZLIB_VERSION, 2), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_7

  inflateBackEnd(strm);
  release_z_stream(strm);
}

TEST(ZLIB_inflateBackInit_, pass_cases)
{
  z_streamp strm = get_z_stream();
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit_(strm, windowBits, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_inflateBackInit__common_8

  inflateBackEnd(strm);
  release_z_stream(strm);
}

TEST(ZLIB_inflateBackEnd, all_cases)
{
  z_streamp strm = get_z_stream();
  const int windowBits = 9;
  unsigned char window[1 << windowBits];
  void *v;

  // fail cases
  EXPECT_EQ(inflateBackEnd(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_1
  EXPECT_EQ(inflateBackEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_2

  inflateBackInit(strm, windowBits, window);

  internal_state *st = strm->state;
  strm->state = 0;
  EXPECT_EQ(inflateBackEnd(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_3
  strm->state = st;

  v = (void *)(strm->zfree);
  strm->zfree = 0;
  EXPECT_EQ(inflateBackEnd(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_4

  strm->zfree = (void (*)(void *, void *))v;

  // pass case
  EXPECT_EQ(inflateBackEnd(strm), Z_OK);
  EXPECT_EQ(strm->state, (internal_state *)NULL); // AOCL_Compression_zlib_inflateBackEnd_common_5

  release_z_stream(strm);
}

TEST(ZLIB_inflateSyncPoint, fail_cases)
{
  z_streamp strm = get_z_stream();
  EXPECT_EQ(inflateSyncPoint(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateSyncPoint_common_1
  EXPECT_EQ(inflateSyncPoint(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateSyncPoint_common_2
  release_inflate_stream(strm);
}

TEST(ZLIB_inflateSyncPoint, pass_cases)
{
  z_streamp strm = get_z_stream();
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;

  state->bits = 1;
  state->mode = SYNC;
  EXPECT_EQ(inflateSyncPoint(strm), false); // AOCL_Compression_zlib_inflateSyncPoint_common_3

  state->bits = 0;
  state->mode = SYNC;
  EXPECT_EQ(inflateSyncPoint(strm), false); // AOCL_Compression_zlib_inflateSyncPoint_common_4

  state->bits = 1;
  state->mode = STORED;
  EXPECT_EQ(inflateSyncPoint(strm), false); // AOCL_Compression_zlib_inflateSyncPoint_common_5

  state->bits = 0;
  state->mode = STORED;
  EXPECT_EQ(inflateSyncPoint(strm), true);  // AOCL_Compression_zlib_inflateSyncPoint_common_6

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateResetKeep, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateResetKeep(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateResetKeep_common_1
  EXPECT_EQ(inflateResetKeep(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateResetKeep_common_2

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateResetKeep, pass_cases)
{
  z_streamp strm = get_z_stream();
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;

  strm->adler = 3;
  state->wrap = 0;
  EXPECT_EQ(inflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_inflateResetKeep_common_3
  EXPECT_EQ(strm->adler, 3);

  strm->adler = 0;
  state->wrap = 1;
  EXPECT_EQ(inflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_inflateResetKeep_common_4
  EXPECT_EQ(strm->adler, 1);

  strm->adler = 2;
  state->wrap = 2;
  EXPECT_EQ(inflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_inflateResetKeep_common_5
  EXPECT_EQ(strm->adler, 0);

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateUndermine, all_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateUndermine(NULL, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_1
  EXPECT_EQ(inflateUndermine(strm, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateUndermine(strm, 1), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_3
  EXPECT_EQ(inflateUndermine(strm, 0), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_4

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateValidate, all_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateValidate(NULL, 1), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateValidate_common_1
  EXPECT_EQ(inflateValidate(strm, 1), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateValidate_common_2

  inflateInit(strm);

  inflate_state *state =(inflate_state *) strm->state;
  state->wrap = 8;
  EXPECT_EQ(inflateValidate(strm, 1), Z_OK);  // AOCL_Compression_zlib_inflateValidate_common_3
  EXPECT_EQ(state->wrap, 12);

  state->wrap = 15;
  EXPECT_EQ(inflateValidate(strm, 0), Z_OK);  // AOCL_Compression_zlib_inflateValidate_common_4
  EXPECT_EQ(state->wrap, 11);

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateCodesUsed, fail_cases)
{
  z_streamp strm = get_z_stream();

  EXPECT_EQ(inflateCodesUsed(strm), (unsigned long)-1); // AOCL_Compression_zlib_inflateCodesUsed_common_1
  EXPECT_EQ(inflateCodesUsed(NULL), (unsigned long)-1); // AOCL_Compression_zlib_inflateCodesUsed_common_2

  release_inflate_stream(strm);
}

TEST(ZLIB_inflateCodesUsed, pass_cases)
{
  z_streamp strm = get_z_stream();
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;

  state->next = &(state->codes[500]);
  EXPECT_EQ(inflateCodesUsed(strm), 500); // AOCL_Compression_zlib_inflateCodesUsed_common_3

  state->next = &(state->codes[0]);
  EXPECT_EQ(inflateCodesUsed(strm), 0); // AOCL_Compression_zlib_inflateCodesUsed_common_4

  state->next = &(state->codes[ENOUGH - 1]);
  EXPECT_EQ(inflateCodesUsed(strm), ENOUGH - 1);  // AOCL_Compression_zlib_inflateCodesUsed_common_5

  release_inflate_stream(strm);
}

/* inflate small amount of data and validate with adler32 checksum */
const char* orig = "The quick brown fox jumped over the lazy dog";

z_const unsigned char comp[] = {
    0x78, 0x9c, 0x0b, 0xc9, 0x48, 0x55, 0x28, 0x2c, 0xcd, 0x4c, 0xce, 0x56, 0x48,
    0x2a, 0xca, 0x2f, 0xcf, 0x53, 0x48, 0xcb, 0xaf, 0x50, 0xc8, 0x2a, 0xcd, 0x2d,
    0x48, 0x4d, 0x51, 0xc8, 0x2f, 0x4b, 0x2d, 0x52, 0x28, 0xc9, 0x48, 0x55, 0xc8,
    0x49, 0xac, 0xaa, 0x54, 0x48, 0xc9, 0x4f, 0x07, 0x00, 0x6b, 0x93, 0x10, 0x30
};

class ZLIB_inflate : public AOCL_setup_zlib {
};

TEST_F(ZLIB_inflate, AOCL_Compression_zlib_inflate_adler32_1)
{
    unsigned char uncomp[1024];
    z_stream strm;

    memset(&strm, 0, sizeof(strm));

    int err = inflateInit2(&strm, 32 + MAX_WBITS);
    EXPECT_EQ(err, Z_OK);

    strm.next_in = comp;
    strm.avail_in = sizeof(comp);
    strm.next_out = uncomp;
    strm.avail_out = sizeof(uncomp);

    err = inflate(&strm, Z_NO_FLUSH);
    EXPECT_EQ(err, Z_STREAM_END);

    EXPECT_EQ(strm.adler, 0x6b931030); // match the checksum with checksum value of orig

    err = inflateEnd(&strm);
    EXPECT_EQ(err, Z_OK);

    EXPECT_TRUE(memcmp(uncomp, orig, MIN(strm.total_out, strlen(orig))) == 0);
}
