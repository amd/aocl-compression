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

TEST(AOCL_Compression_zlib, inflateInit_common)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateInit(inflateObj.get_stream()), Z_OK); // AOCL_Compression_zlib_inflateInit_common_1
}

TEST(AOCL_Compression_zlib, inflateInit_negative)
{
  EXPECT_EQ(inflateInit(NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateInit_common_2
}

TEST(AOCL_Compression_zlib, inflateInit__negative)
{
  ZLIB_inflate_stream inflateObj;
  char c[7] = "0.2.11";

  EXPECT_EQ(inflateInit_(NULL, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateInit__common_1
  EXPECT_EQ(inflateInit_(inflateObj.get_stream(), NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateInit__common_2
  EXPECT_EQ(inflateInit_(inflateObj.get_stream(), c, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_inflateInit__common_3
  EXPECT_EQ(inflateInit_(inflateObj.get_stream(), ZLIB_VERSION, 3), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateInit__common_4
  inflateObj.get_stream()->zalloc = alloc_null;
  EXPECT_EQ(inflateInit_(inflateObj.get_stream(), ZLIB_VERSION, (int)sizeof(z_stream)), Z_MEM_ERROR);
}

TEST(AOCL_Compression_zlib, inflateInit__common)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateInit_(inflateObj.get_stream(), ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_inflateInit__common_5
}

TEST(AOCL_Compression_zlib, inflateEnd_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());

  EXPECT_EQ(inflateEnd(inflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_inflateEnd_common_1
}

TEST(AOCL_Compression_zlib, inflateEnd_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateEnd_common_2
  EXPECT_EQ(inflateEnd(inflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateEnd_common_3
}

TEST(AOCL_Compression_zlib, inflateInit2_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateInit2(NULL, 9), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_1
  EXPECT_EQ(inflateInit2(inflateObj.get_stream(), 7), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_2
  EXPECT_EQ(inflateInit2(inflateObj.get_stream(), 17), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_3
  inflateObj.get_stream()->zalloc = alloc_null;
  EXPECT_EQ(inflateInit2(inflateObj.get_stream(), 8), Z_MEM_ERROR);
}

TEST(AOCL_Compression_zlib, inflateInit2_common)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateInit2(inflateObj.get_stream(), 9), Z_OK); //  AOCL_Compression_zlib_inflateInit2_common_4
}

TEST(AOCL_Compression_zlib, inflateInit2__negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateInit2_(NULL, 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_1
  EXPECT_EQ(inflateInit2_(inflateObj.get_stream(), 9, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_2

  char c[7] = "0.2.11";

  EXPECT_EQ(inflateInit2_(inflateObj.get_stream(), 9, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_3
  EXPECT_EQ(inflateInit2_(inflateObj.get_stream(), 9, ZLIB_VERSION, 3), Z_VERSION_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_4

  EXPECT_EQ(inflateInit2_(inflateObj.get_stream(), 7, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_5
  EXPECT_EQ(inflateInit2_(inflateObj.get_stream(), 17, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_6

  inflateObj.get_stream()->zalloc = alloc_null;

  EXPECT_EQ(inflateInit2_(inflateObj.get_stream(), 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_MEM_ERROR);
}

TEST(AOCL_Compression_zlib, inflateInit2__common)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateInit2_(inflateObj.get_stream(), 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); //  AOCL_Compression_zlib_inflateInit2__common_7
}

TEST_P(AOCL_Compression_zlib, inflateSetDictionary_negative)
{
  ZLIB_inflate_stream inflateObj;
  const uInt dictLen = 10;
  Bytef dictionary[dictLen + 1] = "helloWorld";

  EXPECT_EQ(inflateSetDictionary(NULL, dictionary, dictLen), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateSetDictionary_common_1

  inflateInit(inflateObj.get_stream());
  inflate_state *s = (inflate_state *)inflateObj.get_stream()->state;
  s->mode = HEAD;
  inflateObj.get_stream()->state->wrap = 2;

  EXPECT_EQ(inflateSetDictionary(inflateObj.get_stream(), dictionary, dictLen), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateSetDictionary_common_2

  inflateReset2(inflateObj.get_stream(), -9);
  s->mode = DICT;
  s->check = 0;

  EXPECT_EQ(inflateSetDictionary(inflateObj.get_stream(), dictionary, dictLen), Z_DATA_ERROR); //  AOCL_Compression_zlib_inflateSetDictionary_common_3
}

TEST_P(AOCL_Compression_zlib, inflateSetDictionary_common)
{
  ZLIB_inflate_stream inflateObj;
  const uInt dictLen = 10;
  Bytef dictionary[dictLen + 1] = "helloWorld";
  inflateInit(inflateObj.get_stream());
  inflate_state *s = (inflate_state *)inflateObj.get_stream()->state;
  s->wrap = 0;

  EXPECT_EQ(inflateSetDictionary(inflateObj.get_stream(), dictionary, dictLen), Z_OK); //  AOCL_Compression_zlib_inflateSetDictionary_common_4
  EXPECT_TRUE(memcmp(s->window, dictionary, dictLen)==0);
}

TEST(AOCL_Compression_zlib, inflateGetDictionary_negative)
{
  ZLIB_inflate_stream inflateObj;
  const uInt dictAlloc = 10;
  uInt dictLen = dictAlloc;
  char dict[dictAlloc];

  EXPECT_EQ(inflateGetDictionary(inflateObj.get_stream(), (Bytef *)dict, &dictLen), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateGetDictionary_common_1
  EXPECT_EQ(inflateGetDictionary(NULL, (Bytef *)dict, &dictLen), Z_STREAM_ERROR);
}

TEST(AOCL_Compression_zlib, inflateGetDictionary_common)
{
  ZLIB_inflate_stream inflateObj;
  const uInt dictLen = 10;
  char dict[dictLen + 1] = "helloWorld";
  uInt destLen;
  char dest[dictLen];
  inflateInit(inflateObj.get_stream());

  EXPECT_EQ(inflateGetDictionary(inflateObj.get_stream(), (Bytef *)dest, NULL), Z_OK); //  AOCL_Compression_zlib_inflateGetDictionary_common_2
  EXPECT_EQ(inflateGetDictionary(inflateObj.get_stream(), NULL, &destLen), Z_OK);      //  AOCL_Compression_zlib_inflateGetDictionary_common_3

  EXPECT_EQ(inflateGetDictionary(inflateObj.get_stream(), (Bytef *)dest, &destLen), Z_OK); //  AOCL_Compression_zlib_inflateGetDictionary_common_4

  inflate_state *s = (inflate_state *)inflateObj.get_stream()->state;
  s->wrap = 0;
  inflateSetDictionary(inflateObj.get_stream(), (Bytef *)dict, dictLen);

  EXPECT_EQ(inflateGetDictionary(inflateObj.get_stream(), (Bytef *)dest, &destLen), Z_OK); //  AOCL_Compression_zlib_inflateGetDictionary_common_5
  EXPECT_EQ(dictLen, destLen);
  EXPECT_TRUE(cmpr(dict, dest, dictLen));
}

TEST(AOCL_Compression_zlib, inflateSync_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateSync(inflateObj.get_stream()), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateSync_common_1
  EXPECT_EQ(inflateSync(NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateSync_common_2

  inflateInit(inflateObj.get_stream());

  EXPECT_EQ(inflateSync(inflateObj.get_stream()), Z_BUF_ERROR);  // AOCL_Compression_zlib_inflateSync_common_3

  inflate_state *s = (inflate_state *)inflateObj.get_stream()->state;
  s->bits = 9;

  EXPECT_EQ(inflateSync(inflateObj.get_stream()), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateSync_common_4
}

TEST(AOCL_Compression_zlib, inflateSync_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());
  inflate_state *s = (inflate_state *)inflateObj.get_stream()->state;
  s->bits = 9;
  char c[6];
  c[0] = c[1] = (char)0;
  c[2] = c[3] = (char)0xff;
  c[4] = c[5] = (char)0x2f;
  inflateObj.get_stream()->next_in = (Bytef *)c;
  inflateObj.get_stream()->avail_in = 6;

  EXPECT_EQ(inflateSync(inflateObj.get_stream()), Z_OK); // AOCL_Compression_zlib_inflateSync_common_5
  EXPECT_EQ(inflateObj.get_stream()->next_in, (Bytef *)&c[4]);
  EXPECT_EQ(inflateObj.get_stream()->avail_in, 2);
}

TEST(AOCL_Compression_zlib, inflateCopy_negative)
{
  ZLIB_inflate_stream strm;
  ZLIB_inflate_stream dest;

  EXPECT_EQ(inflateCopy(dest.get_stream(), NULL), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_1
  EXPECT_EQ(inflateCopy(dest.get_stream(), strm.get_stream()), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_2

  inflateInit(strm.get_stream());

  EXPECT_EQ(inflateCopy(NULL, strm.get_stream()), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_3
  strm.get_stream()->zalloc = alloc_null;
  EXPECT_EQ(inflateCopy(dest.get_stream(), strm.get_stream()), Z_MEM_ERROR);
}

TEST(AOCL_Compression_zlib, inflateCopy_common)
{
  ZLIB_inflate_stream strm;
  ZLIB_inflate_stream dest;
  inflateInit(strm.get_stream());
  EXPECT_EQ(inflateCopy(dest.get_stream(), strm.get_stream()), Z_OK); //  AOCL_Compression_zlib_inflateCopy_common_4

  dest.reset_inflate_stream();

  inflate_state *state = (inflate_state *)strm.get_stream()->state;
  state->wbits = 3;
  char c[9] = "abcdefgh";
  state->window = (Bytef *)malloc(sizeof(char) * 8);
  memcpy(state->window, c, 8);
  EXPECT_EQ(inflateCopy(dest.get_stream(), strm.get_stream()), Z_OK); //  AOCL_Compression_zlib_inflateCopy_common_5

  inflate_state *dstate = (inflate_state *)dest.get_stream()->state;
  EXPECT_EQ(dstate->wbits, state->wbits);
  EXPECT_EQ(memcmp(dstate->window, state->window, 1U << state->wbits), 0);
  dest.get_stream()->state = strm.get_stream()->state;
  EXPECT_EQ(memcmp(strm.get_stream(), dest.get_stream(), sizeof(z_stream)), 0);
  dest.get_stream()->state = (internal_state *)dstate;

  inflate_state *temp_dstate = new inflate_state;
  memcpy(temp_dstate, dstate, sizeof(inflate_state));
  dstate->strm = state->strm;
  dstate->next = state->next;
  dstate->window = state->window;
  dstate->lencode = state->lencode;
  dstate->distcode = state->distcode;
  EXPECT_EQ(memcmp(strm.get_stream()->state, dest.get_stream()->state, sizeof(inflate_state)), 0);
  memcpy(dstate, temp_dstate, sizeof(inflate_state));

  dstate = state = nullptr;
  delete temp_dstate;
  temp_dstate = nullptr;
}

TEST(AOCL_Compression_zlib, inflateReset_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateReset(inflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateReset_common_1
  EXPECT_EQ(inflateReset(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateReset_common_2
}

TEST(AOCL_Compression_zlib, inflateReset_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());

  EXPECT_EQ(inflateReset(inflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_inflateReset_common_3
  EXPECT_NE(inflateObj.get_stream()->zalloc, (alloc_func)Z_NULL);
  EXPECT_NE(inflateObj.get_stream()->zfree, (free_func)Z_NULL);
  EXPECT_NE(inflateObj.get_stream()->state, (internal_state *) Z_NULL);
  EXPECT_EQ(inflateObj.get_stream()->total_in, 0);
  EXPECT_EQ(inflateObj.get_stream()->total_out, 0);
  EXPECT_EQ(inflateObj.get_stream()->msg, (char*) Z_NULL);
}

TEST(AOCL_Compression_zlib, inflateReset2_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateReset2(inflateObj.get_stream(), 9), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_1
  EXPECT_EQ(inflateReset2(NULL, 9), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_2

  inflateInit(inflateObj.get_stream());

  EXPECT_EQ(inflateReset2(inflateObj.get_stream(), -7), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateReset2_common_3
  EXPECT_EQ(inflateReset2(inflateObj.get_stream(), -16), Z_STREAM_ERROR);//  AOCL_Compression_zlib_inflateReset2_common_4
  EXPECT_EQ(inflateReset2(inflateObj.get_stream(), 48), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateReset2_common_5
  EXPECT_EQ(inflateReset2(inflateObj.get_stream(), 7), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_6
}

TEST(AOCL_Compression_zlib, inflateReset2_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());
  inflate_state *s = (inflate_state *)inflateObj.get_stream()->state;
  s->wbits = 3;
  s->window = (unsigned char *)malloc(8);
  EXPECT_EQ(inflateReset2(inflateObj.get_stream(), 8), Z_OK);      //  AOCL_Compression_zlib_inflateReset2_common_7
  EXPECT_EQ(s->window, (unsigned char *) NULL);
  EXPECT_EQ(s->wbits, 8);
}

TEST(AOCL_Compression_zlib, inflatePrime_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflatePrime(NULL, 5, 5), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflatePrime_common_1
  EXPECT_EQ(inflatePrime(inflateObj.get_stream(), 5, 5), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflatePrime_common_2
  
  inflateInit(inflateObj.get_stream());
  EXPECT_EQ(inflatePrime(inflateObj.get_stream(), 17, 4), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflatePrime_common_3

  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;
  state->bits = 25;

  EXPECT_EQ(inflatePrime(inflateObj.get_stream(), 10, (1 << 10) - 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflatePrime_common_4
}

TEST(AOCL_Compression_zlib, inflatePrime_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());
  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;

  EXPECT_EQ(inflatePrime(inflateObj.get_stream(), -1, 4), Z_OK); // AOCL_Compression_zlib_inflatePrime_common_5
  EXPECT_EQ(state->hold, 0);
  EXPECT_EQ(state->bits, 0);

  state->bits = 2;

  EXPECT_EQ(inflatePrime(inflateObj.get_stream(), 3, 3), Z_OK);  // AOCL_Compression_zlib_inflatePrime_common_6
  EXPECT_EQ(state->hold, 12);
  EXPECT_EQ(state->bits, 5);
}

TEST(AOCL_Compression_zlib, inflateMark_common)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateMark(inflateObj.get_stream()), -(1L << 16));  // AOCL_Compression_zlib_inflateMark_common_1
  EXPECT_EQ(inflateMark(NULL), -(1L << 16));  // AOCL_Compression_zlib_inflateMark_common_2

  inflateInit(inflateObj.get_stream());
  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;

  state->back = 1;
  state->mode = TABLE;
  EXPECT_EQ(inflateMark(inflateObj.get_stream()), 1 << 16);  // AOCL_Compression_zlib_inflateMark_common_3

  state->back = 0;
  state->mode = COPY;
  state->length = 100;
  EXPECT_EQ(inflateMark(inflateObj.get_stream()), 100);  // AOCL_Compression_zlib_inflateMark_common_4

  state->mode = MATCH;
  state->length = 2;
  state->was = 100;
  EXPECT_EQ(inflateMark(inflateObj.get_stream()), 98); // AOCL_Compression_zlib_inflateMark_common_5
}

TEST(AOCL_Compression_zlib, inflateGetHeader_negative)
{
  ZLIB_inflate_stream inflateObj;
  gz_headerp gz = (gz_headerp)malloc(sizeof(gz_header));

  EXPECT_EQ(inflateGetHeader(NULL, gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_1
  EXPECT_EQ(inflateGetHeader(inflateObj.get_stream(), gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_2

  inflateInit(inflateObj.get_stream());
  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;
  state->wrap = 0;

  EXPECT_EQ(inflateGetHeader(inflateObj.get_stream(), gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_3

  free(gz);
  gz = nullptr;
}

TEST(AOCL_Compression_zlib, inflateGetHeader_common)
{
  ZLIB_inflate_stream inflateObj;
  gz_headerp gz = (gz_headerp)malloc(sizeof(gz_header));
  inflateInit(inflateObj.get_stream());
  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;
  state->wrap = 2;
  gz->done = 1;

  EXPECT_EQ(inflateGetHeader(inflateObj.get_stream(), gz), Z_OK);  // AOCL_Compression_zlib_inflateGetHeader_common_4
  EXPECT_EQ(state->head, gz);
  EXPECT_EQ(gz->done, 0);

  free(gz);
  gz = nullptr;
}

TEST(AOCL_Compression_zlib, inflateBackInit_negative)
{
  ZLIB_inflate_stream inflateObj;
  const int windowBits = 12;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit(NULL, windowBits, window), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit_common_1
  EXPECT_EQ(inflateBackInit(inflateObj.get_stream(), 7, window), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit_common_2
  EXPECT_EQ(inflateBackInit(inflateObj.get_stream(), 16, window), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit_common_3
  EXPECT_EQ(inflateBackInit(inflateObj.get_stream(), 9, NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit_common_4
}

TEST(AOCL_Compression_zlib, inflateBackInit_common)
{
  ZLIB_inflate_stream inflateObj;
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit(inflateObj.get_stream(), windowBits, window), Z_OK); // AOCL_Compression_zlib_inflateBackInit_common_5

  inflateBackEnd(inflateObj.get_stream());
}

TEST(AOCL_Compression_zlib, inflateBackInit__negative)
{
  ZLIB_inflate_stream inflateObj;
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit_(NULL, windowBits, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit__common_1
  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), 7, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_2
  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), 16, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit__common_3
  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), 9, NULL, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_4

  char c[2] = "0";

  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), windowBits, window, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_5
  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), windowBits, window, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_inflateBackInit__common_6
  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), windowBits, window, ZLIB_VERSION, 2), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateBackInit__common_7
  inflateObj.get_stream()->zalloc = alloc_null;
  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), windowBits, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_MEM_ERROR); 

  inflateBackEnd(inflateObj.get_stream());
}

TEST(AOCL_Compression_zlib, inflateBackInit__common)
{
  ZLIB_inflate_stream inflateObj;
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit_(inflateObj.get_stream(), windowBits, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_inflateBackInit__common_8

  inflateBackEnd(inflateObj.get_stream());
}

TEST(AOCL_Compression_zlib, inflateBackEnd_common)
{
  ZLIB_inflate_stream inflateObj;
  const int windowBits = 9;
  unsigned char window[1 << windowBits];
  void *v;

  // fail cases
  EXPECT_EQ(inflateBackEnd(inflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_1
  EXPECT_EQ(inflateBackEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_2

  inflateBackInit(inflateObj.get_stream(), windowBits, window);

  internal_state *st = inflateObj.get_stream()->state;
  inflateObj.get_stream()->state = 0;
  EXPECT_EQ(inflateBackEnd(inflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_3
  inflateObj.get_stream()->state = st;

  v = (void *)(inflateObj.get_stream()->zfree);
  inflateObj.get_stream()->zfree = 0;
  EXPECT_EQ(inflateBackEnd(inflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_4

  inflateObj.get_stream()->zfree = (void (*)(void *, void *))v;

  // pass case
  EXPECT_EQ(inflateBackEnd(inflateObj.get_stream()), Z_OK);
  EXPECT_EQ(inflateObj.get_stream()->state, (internal_state *)NULL); // AOCL_Compression_zlib_inflateBackEnd_common_5
}

TEST(AOCL_Compression_zlib, inflateSyncPoint_negative)
{
  ZLIB_inflate_stream inflateObj;
  EXPECT_EQ(inflateSyncPoint(inflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateSyncPoint_common_1
  EXPECT_EQ(inflateSyncPoint(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateSyncPoint_common_2
}

TEST(AOCL_Compression_zlib, inflateSyncPoint_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());
  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;

  state->bits = 1;
  state->mode = SYNC;
  EXPECT_EQ(inflateSyncPoint(inflateObj.get_stream()), false); // AOCL_Compression_zlib_inflateSyncPoint_common_3

  state->bits = 0;
  state->mode = SYNC;
  EXPECT_EQ(inflateSyncPoint(inflateObj.get_stream()), false); // AOCL_Compression_zlib_inflateSyncPoint_common_4

  state->bits = 1;
  state->mode = STORED;
  EXPECT_EQ(inflateSyncPoint(inflateObj.get_stream()), false); // AOCL_Compression_zlib_inflateSyncPoint_common_5

  state->bits = 0;
  state->mode = STORED;
  EXPECT_EQ(inflateSyncPoint(inflateObj.get_stream()), true);  // AOCL_Compression_zlib_inflateSyncPoint_common_6
}

TEST(AOCL_Compression_zlib, inflateResetKeep_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateResetKeep(inflateObj.get_stream()), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateResetKeep_common_1
  EXPECT_EQ(inflateResetKeep(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateResetKeep_common_2
}

TEST(AOCL_Compression_zlib, inflateResetKeep_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());
  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;

  inflateObj.get_stream()->adler = 3;
  state->wrap = 0;
  EXPECT_EQ(inflateResetKeep(inflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_inflateResetKeep_common_3
  EXPECT_EQ(inflateObj.get_stream()->adler, 3);

  inflateObj.get_stream()->adler = 0;
  state->wrap = 1;
  EXPECT_EQ(inflateResetKeep(inflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_inflateResetKeep_common_4
  EXPECT_EQ(inflateObj.get_stream()->adler, 1);

  inflateObj.get_stream()->adler = 2;
  state->wrap = 2;
  EXPECT_EQ(inflateResetKeep(inflateObj.get_stream()), Z_OK);  // AOCL_Compression_zlib_inflateResetKeep_common_5
  EXPECT_EQ(inflateObj.get_stream()->adler, 0);
}

TEST(AOCL_Compression_zlib, inflateUndermine_common)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateUndermine(NULL, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_1
  EXPECT_EQ(inflateUndermine(inflateObj.get_stream(), 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_2

  inflateInit(inflateObj.get_stream());

  EXPECT_EQ(inflateUndermine(inflateObj.get_stream(), 1), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_3
  EXPECT_EQ(inflateUndermine(inflateObj.get_stream(), 0), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_4
}

TEST(AOCL_Compression_zlib, inflateValidate_common)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateValidate(NULL, 1), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateValidate_common_1
  EXPECT_EQ(inflateValidate(inflateObj.get_stream(), 1), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateValidate_common_2

  inflateInit(inflateObj.get_stream());

  inflate_state *state =(inflate_state *) inflateObj.get_stream()->state;
  state->wrap = 8;
  EXPECT_EQ(inflateValidate(inflateObj.get_stream(), 1), Z_OK);  // AOCL_Compression_zlib_inflateValidate_common_3
  EXPECT_EQ(state->wrap, 12);

  state->wrap = 15;
  EXPECT_EQ(inflateValidate(inflateObj.get_stream(), 0), Z_OK);  // AOCL_Compression_zlib_inflateValidate_common_4
  EXPECT_EQ(state->wrap, 11);
}

TEST(AOCL_Compression_zlib, inflateCodesUsed_negative)
{
  ZLIB_inflate_stream inflateObj;

  EXPECT_EQ(inflateCodesUsed(inflateObj.get_stream()), (unsigned long)-1); // AOCL_Compression_zlib_inflateCodesUsed_common_1
  EXPECT_EQ(inflateCodesUsed(NULL), (unsigned long)-1); // AOCL_Compression_zlib_inflateCodesUsed_common_2
}

TEST(AOCL_Compression_zlib, inflateCodesUsed_common)
{
  ZLIB_inflate_stream inflateObj;
  inflateInit(inflateObj.get_stream());
  inflate_state *state = (inflate_state *)inflateObj.get_stream()->state;

  state->next = &(state->codes[500]);
  EXPECT_EQ(inflateCodesUsed(inflateObj.get_stream()), 500); // AOCL_Compression_zlib_inflateCodesUsed_common_3

  state->next = &(state->codes[0]);
  EXPECT_EQ(inflateCodesUsed(inflateObj.get_stream()), 0); // AOCL_Compression_zlib_inflateCodesUsed_common_4

  state->next = &(state->codes[ENOUGH - 1]);
  EXPECT_EQ(inflateCodesUsed(inflateObj.get_stream()), ENOUGH - 1);  // AOCL_Compression_zlib_inflateCodesUsed_common_5
}

TEST(AOCL_Compression_zlib, inflate_negative)
{
  EXPECT_EQ(inflate(NULL, Z_NO_FLUSH), Z_STREAM_ERROR);
}

/* inflate small amount of data and validate with adler32 checksum */
const char* orig = "The quick brown fox jumped over the lazy dog";

z_const unsigned char comp[] = {
    0x78, 0x9c, 0x0b, 0xc9, 0x48, 0x55, 0x28, 0x2c, 0xcd, 0x4c, 0xce, 0x56, 0x48,
    0x2a, 0xca, 0x2f, 0xcf, 0x53, 0x48, 0xcb, 0xaf, 0x50, 0xc8, 0x2a, 0xcd, 0x2d,
    0x48, 0x4d, 0x51, 0xc8, 0x2f, 0x4b, 0x2d, 0x52, 0x28, 0xc9, 0x48, 0x55, 0xc8,
    0x49, 0xac, 0xaa, 0x54, 0x48, 0xc9, 0x4f, 0x07, 0x00, 0x6b, 0x93, 0x10, 0x30
};

TEST_P(AOCL_Compression_zlib, inflate_adler32_1)
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

// This will generate parameterized tests for all the cpp files
INSTANTIATE_TEST_SUITE_P(
    AOCL_Compression_zlib_Parameterized_Tests, AOCL_Compression_zlib,
    ::testing::ValuesIn(get_supported_optlevels())
);
