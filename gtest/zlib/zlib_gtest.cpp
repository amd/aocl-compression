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

/** @file zlib_gtest.cc
 *
 *  @brief Test cases for ZLIB algo.
 *
 *  This file contains the test cases for ZLIB method
 *  testing the API level functions of ZLIB.
 *
 *  @author J Niranjan Reddy
 */

#include <string>

#include "algos/zlib/zlib.h"
#include "algos/zlib/zutil.h"
#include "algos/zlib/inftrees.h"
#include "algos/zlib/inflate.h"
#include "algos/zlib/deflate.h"
#include "algos/zlib/aocl_zlib_test.h"
#include "api/aocl_compression.h"
#include "gtest/gtest.h"

using namespace std;

#define DEFAULT_OPT_LEVEL 2 // system running gtest must have AVX support

#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )

/* This base class can be used for all fixtures
* that require dynamic dispatcher setup */
class AOCL_setup_zlib : public ::testing::Test {
public:
    AOCL_setup_zlib() {
        int optLevel = DEFAULT_OPT_LEVEL;
        aocl_setup_zlib(0, optLevel, 0, 0, 0);
    }
};

z_streamp strm;

// This is for creating a new z_stream data type
z_streamp nzp()
{
  z_streamp strm = new z_stream;
  memset(strm, 0, sizeof(z_stream));

  return strm;
}

// This is to free z_stream
void fzp(z_streamp strm)
{
    delete strm;
}

// This releases memory that is dynamically allocated for deflate pointers
void rstd(z_streamp strm)
{
  deflateEnd(strm);
  fzp(strm);
}

// This releases memory that is dynamically allocated pointers
void rsti(z_streamp strm)
{
  inflateEnd(strm);
  fzp(strm);
}

// comparision of two different data types
bool cmpr(const char *c1, char *c2, int j)
{
  for (int i = 0; i < j; i++)
  {
    if (c1[i] != c2[i])
    {
      return false;
    }
  }

  return true;
}

// This acts as a setup function before calling deflate function.
int call_before_deflate(z_stream *stream, Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
  int err;
  const uInt max = (uInt)-1;
  uLong left;

  left = *destLen;
  *destLen = 0;

  stream->zalloc = (alloc_func)0;
  stream->zfree = (free_func)0;
  stream->opaque = (voidpf)0;

  err = deflateInit(stream, level);
  if (err != Z_OK)
    return err;

  stream->next_out = dest;
  stream->avail_out = 0;
  stream->next_in = (z_const Bytef *)source;
  stream->avail_in = 0;
  if (stream->avail_out == 0)
  {
    stream->avail_out = left > (uLong)max ? max : (uInt)left;
    left -= stream->avail_out;
  }
  if (stream->avail_in == 0)
  {
    stream->avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
    sourceLen -= stream->avail_in;
  }

  return Z_OK;
}

TEST(ZLIB_zlibVersion, basic)
{
  EXPECT_STREQ(zlibVersion(), ZLIB_VERSION);  // AOCL_compressBoundion_zlib_zlibVersion_common_1
}

TEST(ZLIB_deflateInit, Z_OK_)
{
  z_streamp zp = nzp();

  EXPECT_EQ(deflateInit(zp, 0), Z_OK);  // AOCL_Compression_zlib_deflateInit_common_1
  
  rstd(zp);
  zp = nzp();
  EXPECT_EQ(deflateInit(zp, -1), Z_OK); // AOCL_Compression_zlib_deflateInit_common_2
  
  rstd(zp);
  zp = nzp();
  EXPECT_EQ(deflateInit(zp, 9), Z_OK);  // AOCL_Compression_zlib_deflateInit_common_3

  rstd(zp);
}

TEST(ZLIB_deflateInit, Z_STREAM_ERROR_)
{
  z_streamp zp = nzp();

  EXPECT_EQ(deflateInit(zp, -2), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit_common_4
  EXPECT_EQ(deflateInit(zp, 10), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit_common_5
  EXPECT_EQ(deflateInit(NULL, 3), Z_STREAM_ERROR);// AOCL_Compression_zlib_deflateInit_common_6

  deflateEnd(zp);
  fzp(zp);
}

TEST(ZLIB_deflateInit_, fail_cases)
{
  z_streamp zp = nzp();
  char incorrect_version[7] = "0.2.11";
  int windowBits = 3;

  EXPECT_EQ(deflateInit_(NULL, windowBits, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_1
  EXPECT_EQ(deflateInit_(zp, windowBits, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_deflateInit__common_2
  EXPECT_EQ(deflateInit_(zp, windowBits, incorrect_version, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_deflateInit__common_3
  EXPECT_EQ(deflateInit_(zp, windowBits, ZLIB_VERSION, 2), Z_VERSION_ERROR);  // AOCL_Compression_zlib_deflateInit__common_4
  EXPECT_EQ(deflateInit_(zp, -2, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_5
  EXPECT_EQ(deflateInit_(zp, 10, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateInit__common_6

  deflateEnd(zp);
  fzp(zp);
}

TEST(ZLIB_deflateInit_, pass_cases)
{
  z_streamp zp = nzp();

  EXPECT_EQ(deflateInit_(zp, 0, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);  // AOCL_Compression_zlib_deflateInit__common_7
  
  rstd(zp);
  zp = nzp();
  EXPECT_EQ(deflateInit_(zp, -1, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit__common_8
  
  rstd(zp);
  zp = nzp();
  EXPECT_EQ(deflateInit_(zp, 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);  // AOCL_Compression_zlib_deflateInit__common_9

  rstd(zp);
}

TEST(ZLIB_deflateEnd, Z_OK_)
{
  z_streamp zp = nzp();
  deflateInit(zp, 0);

  EXPECT_EQ(deflateEnd(zp), Z_OK);  // AOCL_Compression_zlib_deflateEnd_common_1

  fzp(zp);
}

TEST(ZLIB_deflateEnd, Z_STREAM_ERROR_)
{
  z_streamp zp = nzp();

  EXPECT_EQ(deflateEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateEnd_common_2
  EXPECT_EQ(deflateEnd(zp), Z_STREAM_ERROR);    // AOCL_Compression_zlib_deflateEnd_common_3

  fzp(zp);
}

TEST(ZLIB_inflateInit, Z_OK_)
{
  z_streamp zp = nzp();

  EXPECT_EQ(inflateInit(zp), Z_OK); // AOCL_Compression_zlib_inflateInit_common_1

  inflateEnd(zp);
  fzp(zp);
}

TEST(ZLIB_inflateInit, Z_STREAM_ERROR_)
{
  EXPECT_EQ(inflateInit(NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateInit_common_2
}

TEST(ZLIB_inflateInit_, fail_cases)
{
  z_streamp zp = nzp();
  char c[7] = "0.2.11";

  EXPECT_EQ(inflateInit_(NULL, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateInit__common_1
  EXPECT_EQ(inflateInit_(zp, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateInit__common_2
  EXPECT_EQ(inflateInit_(zp, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); // AOCL_Compression_zlib_inflateInit__common_3
  EXPECT_EQ(inflateInit_(zp, ZLIB_VERSION, 3), Z_VERSION_ERROR);  // AOCL_Compression_zlib_inflateInit__common_4

  inflateEnd(zp);
  fzp(zp);
}

TEST(ZLIB_inflateInit_, pass_cases)
{
  z_streamp zp = nzp();

  EXPECT_EQ(inflateInit_(zp, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_inflateInit__common_5

  inflateEnd(zp);
  fzp(zp);
}

TEST(ZLIB_inflateEnd, Z_OK_)
{
  z_streamp zp = nzp();
  inflateInit(zp);

  EXPECT_EQ(inflateEnd(zp), Z_OK);  // AOCL_Compression_zlib_inflateEnd_common_1

  fzp(zp);
}

TEST(ZLIB_inflateEnd, Z_STREAM_ERROR_)
{
  z_streamp zp = nzp();

  EXPECT_EQ(inflateEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateEnd_common_2
  EXPECT_EQ(inflateEnd(zp), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateEnd_common_3

  fzp(zp);
}

TEST(ZLIB_compressBound, basic)
{
  EXPECT_EQ(compressBound(0), 13);  // AOCL_compressBoundion_zlib_compressBound_common_1
  EXPECT_EQ(compressBound(1 << 13), 8207);  // AOCL_compressBoundion_zlib_compressBound_common_2
  EXPECT_EQ(compressBound(1 << 15), 32791); // AOCL_compressBoundion_zlib_compressBound_common_3
  EXPECT_EQ(compressBound(1 << 26), 67129359);  // AOCL_compressBoundion_zlib_compressBound_common_4
}

class ZLIB_compress : public AOCL_setup_zlib {
};

TEST_F(ZLIB_compress, fail_cases)
{
  unsigned long destLen = compressBound(11);
  char c[11] = "helloWorld";
  Bytef *dest = (Bytef *)malloc(destLen);

  EXPECT_EQ(compress(NULL, &destLen, (Bytef *)c, 11), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress_common_1
  EXPECT_EQ(compress(dest,NULL,(Bytef*)c,10),Z_STREAM_ERROR); // AOCL_Compression_zlib_compress_common_2
  EXPECT_EQ(compress(dest, &destLen, NULL, 10), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress_common_3
  EXPECT_EQ(compress(dest, &destLen, (Bytef *)c, 0), Z_BUF_ERROR);  // AOCL_Compression_zlib_compress_common_4

  destLen = 2;
  EXPECT_EQ(compress(dest, &destLen, (Bytef *)c, 11), Z_BUF_ERROR); // AOCL_Compression_zlib_compress_common_5

  free(dest);
}

TEST_F(ZLIB_compress, pass)
{
  uLong srcLen = 11;
  char c[11] = "helloWorld";

  uLong destLen = compressBound(11);
  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];
  Bytef *src = (Bytef *)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress(dest, &destLen, src, srcLen), Z_OK); // AOCL_Compression_zlib_compress_common_6
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
}

TEST_F(ZLIB_compress, pass2)
{
  char c[11] = "";
  uLong srcLen = 1;
  
  uLong destLen = compressBound(11);
  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];
  Bytef* src = (Bytef*)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress(dest, &destLen, src, srcLen), Z_OK); // AOCL_Compression_zlib_compress_common_7
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
}

class ZLIB_compress2 : public AOCL_setup_zlib {
};

TEST_F(ZLIB_compress2, fail_cases)
{
  const uLong srcLen = 10;
  Bytef src[srcLen + 1] = "helloWorld";
  unsigned long destLen = compressBound(srcLen);
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(destLen));

  EXPECT_EQ(compress2(NULL, &destLen, src, srcLen, 6), Z_STREAM_ERROR); // AOCL_Compression_zlib_compress2_common_1
  EXPECT_EQ(compress2(dest,NULL,src,srcLen,6),Z_STREAM_ERROR); // AOCL_Compression_zlib_compress2_common_2
  EXPECT_EQ(compress2(dest, &destLen, NULL, srcLen, 6), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress2_common_3
  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, -2), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress2_common_4
  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, 23), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress2_common_5
  EXPECT_EQ(compress2(dest, &destLen, src, 0, 6), Z_BUF_ERROR); // AOCL_Compression_zlib_compress2_common_6
  destLen = 3;
  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, 6), Z_BUF_ERROR);  // AOCL_Compression_zlib_compress2_common_7
  free(dest);
}

TEST_F(ZLIB_compress2, pass)
{
  uLong srcLen = 11;
  char c[11] = "helloWorld";

  unsigned long destLen = compressBound(11);
  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];
  Bytef *src = (Bytef *)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, 6), Z_OK); // AOCL_Compression_zlib_compress2_common_8
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
}

TEST_F(ZLIB_compress2, pass2)
{
  uLong srcLen = 1;
  char c[11] = "";
  
  unsigned long destLen = compressBound(11);
  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];
  Bytef* src = (Bytef*)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, 6), Z_OK); // AOCL_Compression_zlib_compress2_common_9
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
}

class ZLIB_uncompress2 : public AOCL_setup_zlib {
};

TEST_F(ZLIB_uncompress2, fail_cases)
{
  string source = "helloWorld";
  const uLong compressedAlloc = 100;
  uLong compressedLen = compressedAlloc;
  Bytef compressed[compressedAlloc];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());

  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];
  int temp = compressedLen;
  compressedLen = 3;
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress2_common_1
  compressedLen = temp;
  Bytef t = compressed[3];
  compressed[3] = 4;
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress2_common_2
  compressed[3] = t;
  uncompressLen = 3;
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_BUF_ERROR); // AOCL_Compression_zlib_uncompress2_common_3
  EXPECT_EQ(uncompress2(NULL, &uncompressLen, compressed, &compressedLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_uncompress2_common_4
  EXPECT_EQ(uncompress2(uncompressed,NULL,compressed,&compressedLen),Z_STREAM_ERROR); // AOCL_Compression_zlib_uncompress2_common_5
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, NULL, &compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress2_common_6
  EXPECT_EQ(uncompress2(uncompressed,&uncompressLen,compressed,NULL),Z_STREAM_ERROR);  // AOCL_Compression_zlib_uncompress2_common_7
}

TEST_F(ZLIB_uncompress2, pass)
{
  string source = "helloWorld";
  const uLong compressedAlloc = 100;
  uLong compressedLen = compressedAlloc;
  Bytef compressed[compressedAlloc];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());
  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_OK);  // AOCL_Compression_zlib_uncompress2_common_8
  EXPECT_EQ(uncompressLen, source.size());
  EXPECT_TRUE(cmpr(source.data(), (char *)uncompressed, source.size()));
}

class ZLIB_uncompress : public AOCL_setup_zlib {
};

TEST_F(ZLIB_uncompress, fail_cases)
{
  string source = "helloWorld";
  const uLong compressedAlloc = 100;
  uLong compressedLen = compressedAlloc;
  Bytef compressed[compressedAlloc];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());

  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];
  int temp = compressedLen;
  compressedLen = 3;
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, compressed, compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress_common_1
  compressedLen = temp;
  Bytef t = compressed[3];
  compressed[3] = 4;
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, compressed, compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress_common_2
  compressed[3] = t;
  uncompressLen = 3;
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, compressed, compressedLen), Z_BUF_ERROR); // AOCL_Compression_zlib_uncompress_common_3
  EXPECT_EQ(uncompress(NULL, &uncompressLen, compressed, compressedLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_uncompress_common_4
  EXPECT_EQ(uncompress(uncompressed, NULL, compressed, compressedLen),Z_STREAM_ERROR); // AOCL_Compression_zlib_uncompress_common_5
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, NULL, compressedLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_uncompress_common_6
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, compressed, 0), Z_DATA_ERROR); // AOCL_Compression_zlib_uncompress_common_7
}

TEST_F(ZLIB_uncompress, pass)
{
  string source = "helloWorld";
  const uLong compressedAlloc = 100;
  uLong compressedLen = compressedAlloc;
  Bytef compressed[compressedAlloc];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());
  const uLong uncompressAlloc = 100;
  uLong uncompressLen = uncompressAlloc;
  Bytef uncompressed[uncompressAlloc];

  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, compressed, compressedLen), Z_OK);  // AOCL_Compression_zlib_uncompress_common_8
  EXPECT_EQ(uncompressLen, source.size());
  EXPECT_TRUE(cmpr(source.data(), (char *)uncompressed, source.size()));
}

TEST(ZLIB_deflateInit2, fail_cases)
{
  z_streamp strm = nzp();
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

  rstd(strm);
}

TEST(ZLIB_deflateInit2, pass_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 8, 3, 3), Z_OK);  // AOCL_Compression_zlib_deflateInit2_common_15
  
  rstd(strm);
  strm = nzp();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 15, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_16
  
  rstd(strm);
  strm = nzp();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 25, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_17
  
  rstd(strm);
  strm = nzp();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, 31, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_18
  
  rstd(strm);
  strm = nzp();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, -9, 3, 3), Z_OK); // AOCL_Compression_zlib_deflateInit2_common_19
  
  rstd(strm);
  strm = nzp();
  EXPECT_EQ(deflateInit2(strm, 6, Z_DEFLATED, -15, 3, 3), Z_OK);// AOCL_Compression_zlib_deflateInit2_common_20

  rstd(strm);
}

TEST(ZLIB_deflateInit2_, fail_cases)
{
  z_streamp strm = nzp();
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

  rstd(strm);
}

TEST(ZLIB_deflateInit2_, pass_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(deflateInit2_(strm, 6, Z_DEFLATED, 8, 3, 3, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK);  // AOCL_Compression_zlib_deflateInit2__common_18
  EXPECT_EQ(strm->state->level, 6);
  EXPECT_EQ(strm->state->strategy, 3);
  EXPECT_EQ(strm->state->method, Z_DEFLATED);

  rstd(strm);
  strm = nzp();

  EXPECT_EQ(deflateInit2_(strm, 7, Z_DEFLATED, 25, 3, 4, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_deflateInit2__common_19
  EXPECT_EQ(strm->state->level, 7);
  EXPECT_EQ(strm->state->strategy, 4);

  rstd(strm);
}

/*
TEST(ZLIB_deflate, fail_cases)
{
  z_streamp strm = nzp();
  const uLong srcLen = 10;
  const char c[srcLen + 1] = "helloWorld";
  Bytef *src = (Bytef *)c;
  uLong dstLen = 20;
  char cc[dstLen];
  Bytef *dst = (Bytef *)cc;
  call_before_deflate(strm, dst, &dstLen, src, srcLen, 3);

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
  rstd(strm);
}

TEST(ZLIB_deflate, pass_cases)
{
  z_streamp strm = nzp();
  const uLong srcLen = 10;
  char c[srcLen + 1] = "helloWorld";
  Bytef *src = (Bytef *)c;
  uLong dstLen = 20;
  char cc[dstLen];
  Bytef *dst = (Bytef *)cc;
  call_before_deflate(strm, dst, &dstLen, src, srcLen, 3);

  EXPECT_EQ(deflate(strm, Z_NO_FLUSH), Z_OK);

  deflateEnd(strm);
  fzp(strm);
}
*/

TEST(ZLIB_deflateSetDictionary, fail_cases)
{
  z_streamp strm = nzp();
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

  rstd(strm);
}

TEST(ZLIB_deflateSetDictionary, pass_case)
{
  z_streamp strm = nzp();
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

  rstd(strm);
}

TEST(ZLIB_deflateGetDictionary, fail_cases)
{
  Bytef *dict = nullptr;
  uInt dictLen;
  z_streamp strm = nzp();

  EXPECT_EQ(deflateGetDictionary(strm, dict, &dictLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateGetDictionary_common_1
  EXPECT_EQ(deflateGetDictionary(NULL, dict, &dictLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateGetDictionary_common_2

  rstd(strm);
}

TEST(ZLIB_deflateGetDictionary, pass_cases)
{
  Bytef *dict = (Bytef *)malloc(sizeof(Bytef) * 40000);
  uInt dictLen;
  z_streamp strm = nzp();
  deflateInit(strm, 4);
  const int sDictLen = 10;
  char c[sDictLen + 1] = "abcdefghij";
  Bytef *sDict = (Bytef *)c;

  deflateSetDictionary(strm, sDict, sDictLen);

  EXPECT_EQ(deflateGetDictionary(strm, dict, &dictLen), Z_OK);  // AOCL_Compression_zlib_deflateGetDictionary_common_3
  EXPECT_EQ(dictLen, sDictLen);
  EXPECT_TRUE(cmpr(c, (char *)dict, dictLen));

  deflateEnd(strm);
  fzp(strm);
  free(dict);
}

TEST(ZLIB_deflateCopy, fail_cases)
{
  z_streamp dest = nzp();
  z_streamp src = nzp();

  EXPECT_EQ(deflateCopy(dest, NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateCopy_common_1
  EXPECT_EQ(deflateCopy(NULL, src), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateCopy_common_2
  EXPECT_EQ(deflateCopy(dest, src), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateCopy_common_3

  fzp(dest);
  fzp(src);
  // deflateCopy()
}

TEST(ZLIB_deflateCopy, pass_case)
{
  z_streamp dest = nzp();
  z_streamp src = nzp();
  deflateInit(src, 5);

  EXPECT_EQ(deflateCopy(dest, src), Z_OK);  // AOCL_Compression_zlib_deflateCopy_common_4

  rstd(dest);
  rstd(src);
}

TEST(ZLIB_deflateReset, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(deflateReset(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateReset_common_1
  EXPECT_EQ(deflateReset(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateReset_common_2

  rstd(strm);
}

TEST(ZLIB_deflateReset, pass_cases)
{
  z_streamp strm = nzp();
  deflateInit(strm, 4);

  EXPECT_EQ(deflateReset(strm), Z_OK);  // AOCL_Compression_zlib_deflateReset_common_3
  
  EXPECT_EQ(strm->total_in, 0);
  EXPECT_EQ(strm->total_out, 0);
  EXPECT_EQ(strm->msg, (const char *)Z_NULL);
  EXPECT_EQ(strm->data_type, Z_UNKNOWN);
  EXPECT_EQ(strm->state->pending, 0);
  EXPECT_EQ(strm->state->last_flush, Z_NO_FLUSH);

  rstd(strm);
}

class ZLIB_deflateParams : public AOCL_setup_zlib {
};

TEST_F(ZLIB_deflateParams, fail_cases)
{
  z_streamp strm = nzp();
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
  state->high_water = 1;
  EXPECT_EQ(deflateParams(strm, level, 2), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateParams_common_7
  deflateEnd(strm);
  uLongf dstLen = 50;
  uLongf srcLen = 10;
  Bytef *dest = (Bytef *)malloc(dstLen);
  Bytef *src = (Bytef *)malloc(srcLen);
  call_before_deflate(strm, dest, &dstLen, src, srcLen, 0);
  state = (deflate_state *)strm->state;
  state->high_water = 1;
  strm->avail_out = 0;

  EXPECT_EQ(deflateParams(strm, level, 3), Z_BUF_ERROR);  // AOCL_Compression_zlib_deflateParams_common_8

  free(dest);
  free(src);
  rstd(strm);
}

TEST_F(ZLIB_deflateParams, pass_cases)
{
  z_streamp strm = nzp();
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

  rstd(strm);
}

TEST(ZLIB_deflateTune, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(deflateTune(strm, 1, 1, 1, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_deflateTune_common_1

  rstd(strm);
}

TEST(ZLIB_deflateTune, pass_case)
{
  z_streamp strm = nzp();
  deflateInit(strm, 4);
  deflate_state *state = strm->state;

  EXPECT_EQ(deflateTune(strm, 1, 2, 3, 4), Z_OK); // AOCL_Compression_zlib_deflateTune_common_2
  EXPECT_EQ(state->good_match, 1);
  EXPECT_EQ(state->max_lazy_match, 2);
  EXPECT_EQ(state->nice_match, 3);
  EXPECT_EQ(state->max_chain_length, 4);

  rstd(strm);
}

TEST(ZLIB_deflateBound, all_cases)
{
  z_streamp strm = nzp();
  int sourceLen = 1 << 6;

  EXPECT_EQ(deflateBound(strm, sourceLen), 84); // AOCL_Compression_zlib_deflateBound_common_1

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

  EXPECT_EQ(deflateBound(strm, 0), 45); // AOCL_Compression_zlib_deflateBound_common_7

  free(gz);
  rstd(strm);
  // s->gzhead!=NULL user supplied gzip header
}

TEST(ZLIB_deflatePending, fail_cases)
{
  z_streamp strm = nzp();
  unsigned pending;
  int bits;

  EXPECT_EQ(deflatePending(NULL, &pending, &bits), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflatePending_common_1
  EXPECT_EQ(deflatePending(strm, &pending, &bits), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflatePending_common_2

  rstd(strm);
}

TEST(ZLIB_deflatePending, pass_cases)
{
  z_streamp strm = nzp();
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

  rstd(strm);
}

TEST(ZLIB_deflatePrime, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(deflatePrime(NULL, 3, 3), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_deflatePrime_common_1
  EXPECT_EQ(deflatePrime(strm, 3, 3), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_deflatePrime_common_2

  deflateInit(strm, 3);
  strm->state->pending_out = (Bytef *)strm->state->d_buf + 10;

  EXPECT_EQ(deflatePrime(strm, 3, 3), Z_BUF_ERROR); //  AOCL_Compression_zlib_deflatePrime_common_3

  rstd(strm);
}

TEST(ZLIB_deflatePrime, pass_cases)
{
  z_streamp strm = nzp();
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

  rstd(strm);
}

TEST(ZLIB_deflateSetHeader, fail_cases)
{
  z_streamp strm = nzp();
  gz_header head;

  EXPECT_EQ(deflateSetHeader(NULL, &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_1
  EXPECT_EQ(deflateSetHeader(strm, &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_2

  deflateInit(strm, 4);
  strm->state->wrap = 1;

  EXPECT_EQ(deflateSetHeader(strm, &head), Z_STREAM_ERROR); //  AOCL_Compression_zlib_deflateSetHeader_common_3

  rstd(strm);
}

TEST(ZLIB_deflateSetHeader, pass)
{
  z_streamp strm = nzp();
  gz_header head;

  deflateInit(strm, 4);
  strm->state->wrap = 2;
  EXPECT_EQ(deflateSetHeader(strm, &head), Z_OK); //  AOCL_Compression_zlib_deflateSetHeader_common_4
  EXPECT_EQ(strm->state->gzhead, &head);

  strm->state->gzhead = &head;
  strm->state->wrap = 2;
  EXPECT_EQ(deflateSetHeader(strm, NULL), Z_OK);  //  AOCL_Compression_zlib_deflateSetHeader_common_5
  EXPECT_EQ(strm->state->gzhead,(gz_headerp) NULL);

  rstd(strm);
}

TEST(ZLIB_inflateInit2, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateInit2(NULL, 9), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_1
  EXPECT_EQ(inflateInit2(strm, 7), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_2
  EXPECT_EQ(inflateInit2(strm, 17), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2_common_3

  rsti(strm);
}

TEST(ZLIB_inflateInit2, pass_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateInit2(strm, 9), Z_OK); //  AOCL_Compression_zlib_inflateInit2_common_4

  rsti(strm);
}

TEST(ZLIB_inflateInit2_, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateInit2_(NULL, 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_1
  EXPECT_EQ(inflateInit2_(strm, 9, NULL, (int)sizeof(z_stream)), Z_VERSION_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_2

  char c[7] = "0.2.11";

  EXPECT_EQ(inflateInit2_(strm, 9, c, (int)sizeof(z_stream)), Z_VERSION_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_3
  EXPECT_EQ(inflateInit2_(strm, 9, ZLIB_VERSION, 3), Z_VERSION_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_4

  EXPECT_EQ(inflateInit2_(strm, 7, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateInit2__common_5
  EXPECT_EQ(inflateInit2_(strm, 17, ZLIB_VERSION, (int)sizeof(z_stream)), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateInit2__common_6

  rsti(strm);
}

TEST(ZLIB_inflateInit2_, pass_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateInit2_(strm, 9, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); //  AOCL_Compression_zlib_inflateInit2__common_7

  rsti(strm);
}

TEST(ZLIB_inflateSetDictionary, fail_cases)
{
  z_streamp strm = nzp();
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

  rsti(strm);
}

TEST(ZLIB_inflateSetDictionary, pass_cases)
{
  z_streamp strm = nzp();
  const uInt dictLen = 10;
  Bytef dictionary[dictLen + 1] = "helloWorld";
  inflateInit(strm);
  inflate_state *s = (inflate_state *)strm->state;
  s->wrap = 0;

  EXPECT_EQ(inflateSetDictionary(strm, dictionary, dictLen), Z_OK); //  AOCL_Compression_zlib_inflateSetDictionary_common_4
  EXPECT_TRUE(memcmp( s->window, dictionary, dictLen)==0);

  rsti(strm);
}

TEST(ZLIB_inflateGetDictionary, fail_cases)
{
  z_streamp strm = nzp();
  const uInt dictAlloc = 10;
  uInt dictLen = dictAlloc;
  char dict[dictAlloc];

  EXPECT_EQ(inflateGetDictionary(strm, (Bytef *)dict, &dictLen), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateGetDictionary_common_1

  rsti(strm);
}

TEST(ZLIB_inflateGetDicitonary, pass_cases)
{
  z_streamp strm = nzp();
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

  rsti(strm);
}

TEST(ZLIB_adler32_z, all_cases)
{
  size_t len = 5552;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }

  uLong adler = 1 << 16;
  len = 10;
  EXPECT_EQ(adler32_z(adler, buf, len), 695732091); // AOCL_Compression_zlib_adler32_z_common_1

  adler = 0;
  len = 10;
  EXPECT_EQ(adler32_z(adler, NULL, len), 1);  // AOCL_Compression_zlib_adler32_z_common_2

  adler = ((uLong)1L << 31) - 1;
  len = 1;
  EXPECT_EQ(adler32_z(adler, buf, len), 2161180882);  // AOCL_Compression_zlib_adler32_z_common_3

  len = 10;
  EXPECT_EQ(adler32_z(adler, buf, len), 2852259721);  // AOCL_Compression_zlib_adler32_z_common_4

  len = 19;
  EXPECT_EQ(adler32_z(adler, buf, len), 236719599); // AOCL_Compression_zlib_adler32_z_common_5

  len = 5552;
  EXPECT_EQ(adler32_z(adler, buf, len), 71412899);  // AOCL_Compression_zlib_adler32_z_common_6

  free(buf);
}

TEST(ZLIB_adler32, all_cases)
{
  size_t len = 5552;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }

  uLong adler = 1 << 16;
  len = 10;
  EXPECT_EQ(adler32(adler, buf, len), 695732091); // AOCL_Compression_zlib_adler32_common_1

  adler = 0;
  len = 10;
  EXPECT_EQ(adler32(adler, NULL, len), 1);  // AOCL_Compression_zlib_adler32_common_2

  adler = ((uLong)1L << 31) - 1;
  len = 1;
  EXPECT_EQ(adler32(adler, buf, len), 2161180882);  // AOCL_Compression_zlib_adler32_common_3

  len = 10;
  EXPECT_EQ(adler32(adler, buf, len), 2852259721);  // AOCL_Compression_zlib_adler32_common_4

  len = 19;
  EXPECT_EQ(adler32(adler, buf, len), 236719599); // AOCL_Compression_zlib_adler32_common_5

  len = 5552;
  EXPECT_EQ(adler32(adler, buf, len), 71412899);  // AOCL_Compression_zlib_adler32_common_6

  free(buf);
}

TEST(ZLIB_adler32_combine, all_cases)
{
  EXPECT_EQ(adler32_combine( 1, 0, 30), 0); // AOCL_Compression_zlib_adler32_combine_common_1
  EXPECT_EQ(adler32_combine( 1 << 31, 1 << 30, 1 << 30), 4291100656); // AOCL_Compression_zlib_adler32_combine_common_2
}

TEST(ZLIB_crc32_combine, all_cases)
{
  EXPECT_EQ(crc32_combine(1, 0, 30), 1012982536); // AOCL_Compression_zlib_crc32_combine_common_1
  EXPECT_EQ(crc32_combine( 1 << 30, 1 << 29, 1 << 28), 1114707486); // AOCL_Compression_zlib_crc32_combine_common_2
}

TEST(ZLIB_crc32, all_cases)
{
  const int len = 100000;
  unsigned char c[len];
  for (int i = 0; i < len; i++)
  {
    c[i] = i % 255;
  }

  EXPECT_EQ(crc32(1, NULL, 1), 0);  // AOCL_Compression_zlib_crc32_common_1
  EXPECT_EQ(crc32(7, c, len), 1142422866);  // AOCL_Compression_zlib_crc32_common_2
}

TEST(ZLIB_crc32_z, all_cases)
{
  const int len = 100000;
  unsigned char c[len];

  for (int i = 0; i < len; i++)
  {
    c[i] = i % 255;
  }

  EXPECT_EQ(crc32_z(1, NULL, 1), 0);  // AOCL_Compression_zlib_crc32_z_common_1
  EXPECT_EQ(crc32_z(7, c, len), 1142422866);  // AOCL_Compression_zlib_crc32_z_common_2
}

TEST(ZLIB_zError, basic)
{
  EXPECT_STREQ(zError(Z_OK), ""); // AOCL_Compression_zlib_zError_common_1
  EXPECT_STREQ(zError(Z_STREAM_END), "stream end"); // AOCL_Compression_zlib_zError_common_2
  EXPECT_STREQ(zError(Z_NEED_DICT), "need dictionary"); // AOCL_Compression_zlib_zError_common_3
  EXPECT_STREQ(zError(Z_ERRNO), "file error");  // AOCL_Compression_zlib_zError_common_4
  EXPECT_STREQ(zError(Z_STREAM_ERROR), "stream error"); // AOCL_Compression_zlib_zError_common_5
  EXPECT_STREQ(zError(Z_DATA_ERROR), "data error"); // AOCL_Compression_zlib_zError_common_6
  EXPECT_STREQ(zError(Z_MEM_ERROR), "insufficient memory"); // AOCL_Compression_zlib_zError_common_7
  EXPECT_STREQ(zError(Z_BUF_ERROR), "buffer error");  // AOCL_Compression_zlib_zError_common_8
  EXPECT_STREQ(zError(Z_VERSION_ERROR), "incompatible version");  // AOCL_Compression_zlib_zError_common_9
  EXPECT_STREQ(zError(-7), ""); // AOCL_Compression_zlib_zError_common_10
  //<-7 && >2 cases are not handled
}

TEST(ZLIB_inflateSync, failed_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateSync(strm), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateSync_common_1
  EXPECT_EQ(inflateSync(NULL), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateSync_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateSync(strm), Z_BUF_ERROR);  // AOCL_Compression_zlib_inflateSync_common_3

  inflate_state *s = (inflate_state *)strm->state;
  s->bits = 9;

  EXPECT_EQ(inflateSync(strm), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateSync_common_4

  rsti(strm);
}

TEST(ZLIB_inflateSync, pass_cases)
{
  z_streamp strm = nzp();
  inflateInit(strm);
  inflate_state *s = (inflate_state *)strm->state;
  s->bits = 9;
  char c[4];
  c[0] = c[1] = (char)0;
  c[2] = c[3] = (char)0xff;
  strm->next_in = (Bytef *)c;
  strm->avail_in = 4;

  EXPECT_EQ(inflateSync(strm), Z_OK); // AOCL_Compression_zlib_inflateSync_common_5

  rsti(strm);
}

TEST(ZLIB_inflateCopy, fail_cases)
{
  z_streamp strm = nzp();
  z_streamp dest = new z_stream;

  EXPECT_EQ(inflateCopy(dest, NULL), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_1
  EXPECT_EQ(inflateCopy(dest, strm), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateCopy(NULL, strm), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateCopy_common_3

  delete dest;
  rsti(strm);
}

TEST(ZLIB_inflateCopy, pass_cases)
{
  z_streamp strm = nzp();
  z_streamp dest = nzp();
  inflateInit(strm);
  EXPECT_EQ(inflateCopy(dest, strm), Z_OK); //  AOCL_Compression_zlib_inflateCopy_common_4

  rsti(dest);
  dest = nzp();

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

  rsti(dest);
  rsti(strm);
  delete temp_dstate;
}

TEST(ZLIB_inflateReset, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateReset(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateReset_common_1
  EXPECT_EQ(inflateReset(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateReset_common_2

  rsti(strm);
}

TEST(ZLIB_inflateReset, pass_cases)
{
  z_streamp strm = nzp();
  inflateInit(strm);

  EXPECT_EQ(inflateReset(strm), Z_OK);  // AOCL_Compression_zlib_inflateReset_common_3

  rsti(strm);
}

TEST(ZLIB_inflateReset2, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateReset2(strm, 9), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_1
  EXPECT_EQ(inflateReset2(NULL, 9), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateReset2(strm, -7), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateReset2_common_3
  EXPECT_EQ(inflateReset2(strm, -16), Z_STREAM_ERROR);//  AOCL_Compression_zlib_inflateReset2_common_4
  EXPECT_EQ(inflateReset2(strm, 48), Z_STREAM_ERROR); //  AOCL_Compression_zlib_inflateReset2_common_5
  EXPECT_EQ(inflateReset2(strm, 7), Z_STREAM_ERROR);  //  AOCL_Compression_zlib_inflateReset2_common_6

  rsti(strm);
}

TEST(ZLIB_inflateReset2, pass_cases)
{
  z_streamp strm = nzp();
  inflateInit(strm);
  inflate_state *s = (inflate_state *)strm->state;
  s->wbits = 3;
  s->window = (unsigned char *)malloc(8);
  EXPECT_EQ(inflateReset2(strm, 8), Z_OK);      //  AOCL_Compression_zlib_inflateReset2_common_7
  EXPECT_EQ(s->window, (unsigned char *) NULL);
  EXPECT_EQ(s->wbits, 8);

  rsti(strm);
}

TEST(ZLIB_inflatePrime, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflatePrime(NULL, 5, 5), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflatePrime_common_1
  EXPECT_EQ(inflatePrime(strm, 5, 5), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflatePrime_common_2
  
  inflateInit(strm);
  EXPECT_EQ(inflatePrime(strm, 17, 4), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflatePrime_common_3

  inflate_state *state = (inflate_state *)strm->state;
  state->bits = 25;

  EXPECT_EQ(inflatePrime(strm, 10, (1 << 10) - 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflatePrime_common_4

  rsti(strm);
}

TEST(ZLIB_inflatePrime, pass_cases)
{
  z_streamp strm = nzp();
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;

  EXPECT_EQ(inflatePrime(strm, -1, 4), Z_OK); // AOCL_Compression_zlib_inflatePrime_common_5
  EXPECT_EQ(state->hold, 0);
  EXPECT_EQ(state->bits, 0);

  state->bits = 2;

  EXPECT_EQ(inflatePrime(strm, 3, 3), Z_OK);  // AOCL_Compression_zlib_inflatePrime_common_6
  EXPECT_EQ(state->hold, 12);
  EXPECT_EQ(state->bits, 5);

  rsti(strm);
}

TEST(ZLIB_inflateMark, all_cases)
{
  z_streamp strm = nzp();

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

  rsti(strm);
}

TEST(ZLIB_inflateGetHeader, fail_cases)
{
  z_streamp strm = nzp();
  gz_headerp gz = (gz_headerp)malloc(sizeof(gz_header));

  EXPECT_EQ(inflateGetHeader(NULL, gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_1
  EXPECT_EQ(inflateGetHeader(strm, gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_2

  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;
  state->wrap = 0;

  EXPECT_EQ(inflateGetHeader(strm, gz), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateGetHeader_common_3

  free(gz);
  rsti(strm);
}

TEST(ZLIB_inflateGetHeader, pass_cases)
{
  z_streamp strm = nzp();
  gz_headerp gz = (gz_headerp)malloc(sizeof(gz_header));
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;
  state->wrap = 2;
  gz->done = 1;

  EXPECT_EQ(inflateGetHeader(strm, gz), Z_OK);  // AOCL_Compression_zlib_inflateGetHeader_common_4
  EXPECT_EQ(state->head, gz);
  EXPECT_EQ(gz->done, 0);

  free(gz);
  rsti(strm);
}

TEST(ZLIB_inflateBackInit, fail_cases)
{
  z_streamp strm = nzp();
  const int windowBits = 12;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit(NULL, windowBits, window), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit_common_1
  EXPECT_EQ(inflateBackInit(strm, 7, window), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit_common_2
  EXPECT_EQ(inflateBackInit(strm, 16, window), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateBackInit_common_3
  EXPECT_EQ(inflateBackInit(strm, 9, NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackInit_common_4

  rsti(strm);
}

TEST(ZLIB_inflateBackInit, pass_cases)
{
  z_streamp strm = nzp();
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit(strm, windowBits, window), Z_OK); // AOCL_Compression_zlib_inflateBackInit_common_5

  inflateBackEnd(strm);
  fzp(strm);
}

TEST(ZLIB_inflateBackInit_, fail_cases)
{
  z_streamp strm = nzp();
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
  fzp(strm);
}

TEST(ZLIB_inflateBackInit_, pass_cases)
{
  z_streamp strm = nzp();
  const int windowBits = 9;
  unsigned char window[1 << windowBits];

  EXPECT_EQ(inflateBackInit_(strm, windowBits, window, ZLIB_VERSION, (int)sizeof(z_stream)), Z_OK); // AOCL_Compression_zlib_inflateBackInit__common_8

  inflateBackEnd(strm);
  fzp(strm);
}

TEST(ZLIB_inflateBackEnd, all_cases)
{
  z_streamp strm = nzp();
  const int windowBits = 9;
  unsigned char window[1 << windowBits];
  void *v;

  // fail cases
  EXPECT_EQ(inflateBackEnd(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_1
  EXPECT_EQ(inflateBackEnd(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateBackEnd_common_2

  inflateBackInit(strm, windowBits, window);

  internal_state * st = strm->state;
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

  fzp(strm);
}

TEST(ZLIB_inflateSyncPoint, fail_cases)
{
  z_streamp strm = nzp();
  EXPECT_EQ(inflateSyncPoint(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateSyncPoint_common_1
  EXPECT_EQ(inflateSyncPoint(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateSyncPoint_common_2
  rsti(strm);
}

TEST(ZLIB_inflateSyncPoint, pass_cases)
{
  z_streamp strm = nzp();
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

  rsti(strm);
}

TEST(ZLIB_inflateResetKeep, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateResetKeep(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateResetKeep_common_1
  EXPECT_EQ(inflateResetKeep(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_inflateResetKeep_common_2

  rsti(strm);
}

TEST(ZLIB_inflateResetKeep, pass_cases)
{
  z_streamp strm = nzp();
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

  rsti(strm);
}

TEST(ZLIB_deflateResetKeep, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(deflateResetKeep(strm), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateResetKeep_common_1
  EXPECT_EQ(deflateResetKeep(NULL), Z_STREAM_ERROR);  // AOCL_Compression_zlib_deflateResetKeep_common_2

  rstd(strm);
}

TEST(ZLIB_deflateResetKeep, pass_cases)
{
  z_streamp strm = nzp();
  deflateInit(strm, 3);
  deflate_state *state = (deflate_state *)strm->state;

  // 2,1,0
  state->wrap = -2;
  state->last_flush = Z_FINISH;

  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_3
  EXPECT_EQ(state->wrap, 2);
  EXPECT_EQ(state->last_flush, Z_NO_FLUSH);
  EXPECT_EQ(strm->adler, 0);

  state->wrap = 1;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_4
  EXPECT_EQ(state->wrap, 1);
  EXPECT_EQ(state->last_flush, Z_NO_FLUSH);
  EXPECT_EQ(strm->adler, 1);

  state->wrap = 0;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_5
  EXPECT_EQ(state->wrap, 0);
  EXPECT_EQ(state->last_flush, Z_NO_FLUSH);
  EXPECT_EQ(strm->adler, 1);

  state->wrap = -100;
  state->last_flush = Z_FINISH;
  EXPECT_EQ(deflateResetKeep(strm), Z_OK);  // AOCL_Compression_zlib_deflateResetKeep_common_6
  EXPECT_EQ(state->wrap, 100);
  EXPECT_EQ(state->last_flush, Z_NO_FLUSH);
  EXPECT_EQ(strm->adler, 1);

  rstd(strm);
}

TEST(ZLIB_inflateUndermine, all_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateUndermine(NULL, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_1
  EXPECT_EQ(inflateUndermine(strm, 1), Z_STREAM_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_2

  inflateInit(strm);

  EXPECT_EQ(inflateUndermine(strm, 1), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_3
  EXPECT_EQ(inflateUndermine(strm, 0), Z_DATA_ERROR); // AOCL_Compression_zlib_inflateUndermine_common_4

  rsti(strm);
}

TEST(ZLIB_inflateValidate, all_cases)
{
  z_streamp strm = nzp();

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

  rsti(strm);
}

TEST(ZLIB_inflateCodesUsed, fail_cases)
{
  z_streamp strm = nzp();

  EXPECT_EQ(inflateCodesUsed(strm), (unsigned long)-1); // AOCL_Compression_zlib_inflateCodesUsed_common_1
  EXPECT_EQ(inflateCodesUsed(NULL), (unsigned long)-1); // AOCL_Compression_zlib_inflateCodesUsed_common_2

  rsti(strm);
}

TEST(ZLIB_inflateCodesUsed, pass_cases)
{
  z_streamp strm = nzp();
  inflateInit(strm);
  inflate_state *state = (inflate_state *)strm->state;

  state->next = &(state->codes[500]);
  EXPECT_EQ(inflateCodesUsed(strm), 500); // AOCL_Compression_zlib_inflateCodesUsed_common_3

  state->next = &(state->codes[0]);
  EXPECT_EQ(inflateCodesUsed(strm), 0); // AOCL_Compression_zlib_inflateCodesUsed_common_4

  state->next = &(state->codes[ENOUGH - 1]);
  EXPECT_EQ(inflateCodesUsed(strm), ENOUGH - 1);  // AOCL_Compression_zlib_inflateCodesUsed_common_5

  rsti(strm);
}

class ZLIB_adler32_x86 : public AOCL_setup_zlib {
};

TEST_F(ZLIB_adler32_x86, all_cases)
{
  size_t len = 5552;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }

  uLong adler = 1 << 16;
  len = 10;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len)); // AOCL_Compression_zlib_adler32_x86_common_1

  adler = 0;
  len = 10;
  EXPECT_EQ(Test_adler32_x86(adler, NULL, len), adler32(adler, NULL, len));  // AOCL_Compression_zlib_adler32_x86_common_2

  adler = ((uLong)1L << 31) - 1;
  len = 1;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_3

  len = 10;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_4

  len = 19;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len)); // AOCL_Compression_zlib_adler32_x86_common_5

  len = 5552;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_6

  len = 64;
  adler = 1;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_7

  len = 1;
  adler = 0xFFFFFFFF;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_8

  len = 60;
  adler = 0xFFFFFFFF;
  EXPECT_EQ(Test_adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_9

  free(buf);
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
}
