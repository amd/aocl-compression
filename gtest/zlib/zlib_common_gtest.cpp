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

/** @file zlib_common_gtest.cc
 *
 *  @brief Test cases for ZLIB algo.
 *
 *  This file contains the test cases for ZLIB method
 *  testing the common functions of ZLIB.
 *
 *  @author Ravi Jangra
 */

#include "zlib_gtest.h"
#include "gtest_utils.h"

TEST(AOCL_Compression_zlib, zlibVersion_common)
{
  EXPECT_STREQ(zlibVersion(), ZLIB_VERSION);  // AOCL_compressBoundion_zlib_zlibVersion_common_1
}

#ifndef AOCL_ENABLE_THREADS
TEST(AOCL_Compression_zlib, compressBound_common)
{
  EXPECT_EQ(compressBound(0), 13);  // AOCL_compression_zlib_compressBound_common_1
  if(getenv("AOCL_ZLIB_QUICK_MODE") != NULL) {
    EXPECT_EQ(compressBound(1 << 13), 9229);
    EXPECT_EQ(compressBound(1 << 15), 36877);
    EXPECT_EQ(compressBound(1 << 26), 75497485);
  } else {
    EXPECT_EQ(compressBound(1 << 13), 8207);  // AOCL_compression_zlib_compressBound_common_2
    EXPECT_EQ(compressBound(1 << 15), 32791); // AOCL_compression_zlib_compressBound_common_3
    EXPECT_EQ(compressBound(1 << 26), 67129359);  // AOCL_compression_zlib_compressBound_common_4
  }
}
#endif

TEST_P(AOCL_Compression_zlib, compress_negative)
{
  unsigned long destLen = compressBound(11);
  char c[11] = "helloWorld";
  Bytef *dest = (Bytef *)malloc(destLen);

#ifdef AOCL_ENABLE_THREADS
  EXPECT_EQ(compress(NULL, &destLen, (Bytef *)c, 11), Z_MEM_ERROR);  // AOCL_Compression_zlib_compress_common_1
#else
  EXPECT_EQ(compress(NULL, &destLen, (Bytef*)c, 11), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress_common_1
#endif
  EXPECT_EQ(compress(dest,NULL,(Bytef*)c,10),Z_BUF_ERROR); // AOCL_Compression_zlib_compress_common_2
  EXPECT_EQ(compress(dest, &destLen, NULL, 10), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress_common_3
  EXPECT_EQ(compress(dest, &destLen, (Bytef *)c, 0), Z_BUF_ERROR);  // AOCL_Compression_zlib_compress_common_4

  destLen = 2;
  EXPECT_EQ(compress(dest, &destLen, (Bytef *)c, 11), Z_BUF_ERROR); // AOCL_Compression_zlib_compress_common_5

  free(dest);
  dest = nullptr;
}

TEST_P(AOCL_Compression_zlib, compress_common)
{
  uLong srcLen = 11;
  char c[11] = "helloWorld";

  uLong destLen = compressBound(srcLen);
  uLong uncompressLen = srcLen;
  Bytef uncompressed[uncompressLen];
  Bytef *src = (Bytef *)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress(dest, &destLen, src, srcLen), Z_OK); // AOCL_Compression_zlib_compress_common_6
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
  dest = nullptr;
}

TEST_P(AOCL_Compression_zlib, compress_boundary)
{
  char c[11] = "";
  uLong srcLen = 1;
  
  uLong destLen = compressBound(srcLen);
  uLong uncompressLen = srcLen;
  Bytef uncompressed[uncompressLen];
  Bytef* src = (Bytef*)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress(dest, &destLen, src, srcLen), Z_OK); // AOCL_Compression_zlib_compress_common_7
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
  dest = nullptr;
}

TEST_P(AOCL_Compression_zlib, compress2_negative)
{
  const uLong srcLen = 10;
  Bytef src[srcLen + 1] = "helloWorld";
  unsigned long destLen = compressBound(srcLen);
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(destLen));
  int invalid_clevels[2] = {-2, 23};
  int valid_clevel = 6;
  int invalid_srcLen = 0;


#ifdef AOCL_ENABLE_THREADS
  EXPECT_EQ(compress2(NULL, &destLen, src, srcLen, valid_clevel), Z_MEM_ERROR); // AOCL_Compression_zlib_compress2_common_1
#else
  EXPECT_EQ(compress2(NULL, &destLen, src, srcLen, valid_clevel), Z_STREAM_ERROR); // AOCL_Compression_zlib_compress2_common_1
#endif
  EXPECT_EQ(compress2(dest, NULL, src, srcLen, valid_clevel),Z_BUF_ERROR); // AOCL_Compression_zlib_compress2_common_2
  EXPECT_EQ(compress2(dest, &destLen, NULL, srcLen, valid_clevel), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress2_common_3
  destLen = compressBound(srcLen);
  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, invalid_clevels[0]), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress2_common_4
  destLen = compressBound(srcLen);
  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, invalid_clevels[1]), Z_STREAM_ERROR);  // AOCL_Compression_zlib_compress2_common_5
  EXPECT_EQ(compress2(dest, &destLen, src, invalid_srcLen, valid_clevel), Z_BUF_ERROR); // AOCL_Compression_zlib_compress2_common_6
  destLen = 3; // not enough output buffer
  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, valid_clevel), Z_BUF_ERROR);  // AOCL_Compression_zlib_compress2_common_7
  free(dest);
  dest = nullptr;
}

TEST_P(AOCL_Compression_zlib, compress2_common)
{
  uLong srcLen = 11;
  char c[11] = "helloWorld";

  unsigned long destLen = compressBound(srcLen);
  uLong uncompressLen = srcLen;
  Bytef uncompressed[uncompressLen];
  Bytef *src = (Bytef *)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, 6), Z_OK); // AOCL_Compression_zlib_compress2_common_8
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
  dest = nullptr;
}

TEST_P(AOCL_Compression_zlib, compress2_boundary)
{
  uLong srcLen = 1;
  char c[11] = "";
  
  unsigned long destLen = compressBound(srcLen);
  uLong uncompressLen = srcLen;
  Bytef uncompressed[uncompressLen];
  Bytef* src = (Bytef*)c;
  Bytef* dest = (Bytef*)malloc(destLen * sizeof(Bytef));

  EXPECT_EQ(compress2(dest, &destLen, src, srcLen, 6), Z_OK); // AOCL_Compression_zlib_compress2_common_9
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, dest, destLen), Z_OK);
  ASSERT_EQ(uncompressLen, srcLen);
  EXPECT_STREQ(c, (char *)uncompressed);
  free(dest);
  dest = nullptr;
}

TEST_P(AOCL_Compression_zlib, uncompress2_negative)
{
  string source = "helloWorld";
  uLong compressedLen = compressBound(source.length());
  Bytef compressed[compressedLen];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());

  uLong uncompressLen = source.length();
  Bytef uncompressed[uncompressLen];
  int temp = compressedLen;
  compressedLen = 3; // partial compressed buffer
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress2_common_1
  compressedLen = temp;
  Bytef t = compressed[3];
  compressed[3] = 4; // compressed buffer invalid modification
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress2_common_2
  compressed[3] = t;
  uncompressLen = 3; // insufficient output buffer size
  #ifdef AOCL_ENABLE_THREADS
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_DATA_ERROR); // AOCL_Compression_zlib_uncompress2_common_3
  #else
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_BUF_ERROR); // AOCL_Compression_zlib_uncompress2_common_3
  #endif
  EXPECT_EQ(uncompress2(NULL, &uncompressLen, compressed, &compressedLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_uncompress2_common_4
  EXPECT_EQ(uncompress2(uncompressed,NULL,compressed,&compressedLen),Z_BUF_ERROR); // AOCL_Compression_zlib_uncompress2_common_5
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, NULL, &compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress2_common_6
  EXPECT_EQ(uncompress2(uncompressed,&uncompressLen,compressed, NULL), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress2_common_7
}

TEST_P(AOCL_Compression_zlib, uncompress2_common)
{
  string source = "helloWorld";
  uLong compressedLen = compressBound(source.length());
  Bytef compressed[compressedLen];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());
  uLong uncompressLen = source.length();
  Bytef uncompressed[uncompressLen];
  EXPECT_EQ(uncompress2(uncompressed, &uncompressLen, compressed, &compressedLen), Z_OK);  // AOCL_Compression_zlib_uncompress2_common_8
  EXPECT_EQ(uncompressLen, source.size());
  EXPECT_TRUE(cmpr(source.data(), (char *)uncompressed, source.size()));
}

TEST_P(AOCL_Compression_zlib, uncompress_negative)
{
  string source = "helloWorld";
  uLong compressedLen = compressBound(source.length());
  Bytef compressed[compressedLen];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());

  uLong uncompressLen = source.length();
  Bytef uncompressed[uncompressLen];
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
  EXPECT_EQ(uncompress(uncompressed, NULL, compressed, compressedLen),Z_BUF_ERROR); // AOCL_Compression_zlib_uncompress_common_5
#ifdef AOCL_ENABLE_THREADS
    EXPECT_EQ(uncompress(uncompressed, &uncompressLen, NULL, compressedLen), Z_DATA_ERROR);  // AOCL_Compression_zlib_uncompress_common_6
#else
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, NULL, compressedLen), Z_STREAM_ERROR);  // AOCL_Compression_zlib_uncompress_common_6
#endif
  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, compressed, 0), Z_DATA_ERROR); // AOCL_Compression_zlib_uncompress_common_7
}

TEST_P(AOCL_Compression_zlib, uncompress_common)
{
  string source = "helloWorld";
  uLong compressedLen = compressBound(source.length());
  Bytef compressed[compressedLen];
  compress(compressed, &compressedLen, (Bytef *)source.data(), source.size());
  uLong uncompressLen = source.length();
  Bytef uncompressed[uncompressLen];

  EXPECT_EQ(uncompress(uncompressed, &uncompressLen, compressed, compressedLen), Z_OK);  // AOCL_Compression_zlib_uncompress_common_8
  EXPECT_EQ(uncompressLen, source.size());
  EXPECT_TRUE(cmpr(source.data(), (char *)uncompressed, source.size()));
}

void test_crc32_x86(uLong crc, const Bytef* buf, uInt len) {
    int highest_supported_level = get_cpu_opt_flags(0);
    uLong ref = crc32_z_c(crc, buf, len);
#ifdef AOCL_ZLIB_AVX_OPT
    if (highest_supported_level >= 2) // >= AVX
        EXPECT_EQ(crc32_z_x86_avx(crc, buf, len), ref);
#endif /* AOCL_ZLIB_AVX_OPT */
#ifdef AOCL_ZLIB_AVX512_OPT
    if (highest_supported_level >= 4) // >= AVX512
        EXPECT_EQ(crc32_z_x86_avx512(crc, buf, len), ref);
#endif /* AOCL_ZLIB_AVX512_OPT */
}

// common boundary test case for checksum APIs to minimize memory footprint while running in parallel
TEST(AOCL_Compression_zlib, checksum_boundary_common)
{
  size_t len = UINT32_MAX;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }
  uLong adler = 0xFFFFFFFF;

  len = 5552;
  EXPECT_EQ(adler32_z(adler, buf, len), 3013531973);  // AOCL_Compression_zlib_adler32_z_common_6
  EXPECT_EQ(adler32(adler, buf, len), 3013531973);  // AOCL_Compression_zlib_adler32_common_6

  len = 0;
  EXPECT_EQ(adler32_z(adler, buf, len), 917518);
  EXPECT_EQ(adler32(adler, buf, len), 917518);

  len = UINT32_MAX;
  EXPECT_EQ(adler32_z(adler, buf, len), 365981486);
  EXPECT_EQ(adler32(adler, buf, len), 365981486);
  EXPECT_EQ(crc32_z(7, buf, len), 7);
  EXPECT_EQ(crc32(7, buf, len), 7);
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));
  test_crc32_x86(adler, buf, len);
  
  EXPECT_EQ(adler32_combine(adler32(1, buf, 255), adler32(1, buf + 255, 1000), 1000), adler32(1, buf, 1255));

  EXPECT_EQ(crc32_combine(crc32(0, buf, 255), crc32(0, buf + 255, 1000), 1000), crc32(0, buf, 1255));

  EXPECT_EQ(crc32_combine_op(crc32(0, buf, 255), crc32(0, buf + 255, 1000), crc32_combine_gen(1000)), crc32_combine(crc32(0, buf, 255), crc32(0, buf + 255, 1000), 1000));

  EXPECT_EQ(crc32(7, buf, 100000), 2630052870);  // AOCL_Compression_zlib_crc32_common_2
  EXPECT_EQ(crc32_z(7, buf, 100000), 2630052870);  // AOCL_Compression_zlib_crc32_z_common_2

  free(buf);
  buf = nullptr;

}

TEST(AOCL_Compression_zlib, adler32_z_common)
{
  size_t len = 19;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }

  uLong adler = 1 << 16;
  len = 10;
  EXPECT_EQ(adler32_z(adler, buf, len), 54132871); // AOCL_Compression_zlib_adler32_z_common_1

  adler = 0;
  len = 10;
  EXPECT_EQ(adler32_z(adler, NULL, len), 1);  // AOCL_Compression_zlib_adler32_z_common_2

  adler = ((uLong)1L << 31) - 1;
  len = 1;
  EXPECT_EQ(adler32_z(adler, buf, len), 2149515296);  // AOCL_Compression_zlib_adler32_z_common_3

  len = 10;
  EXPECT_EQ(adler32_z(adler, buf, len), 2210660501);  // AOCL_Compression_zlib_adler32_z_common_4

  len = 19;
  EXPECT_EQ(adler32_z(adler, buf, len), 2314272953); // AOCL_Compression_zlib_adler32_z_common_5

  free(buf);
  buf = nullptr;
}

TEST(AOCL_Compression_zlib, adler32_common)
{
  size_t len = 19;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }

  uLong adler = 1 << 16;
  len = 10;
  EXPECT_EQ(adler32(adler, buf, len), 54132871); // AOCL_Compression_zlib_adler32_common_1

  adler = 0;
  len = 10;
  EXPECT_EQ(adler32(adler, NULL, len), 1);  // AOCL_Compression_zlib_adler32_common_2

  adler = ((uLong)1L << 31) - 1;
  len = 1;
  EXPECT_EQ(adler32(adler, buf, len), 2149515296);  // AOCL_Compression_zlib_adler32_common_3

  len = 10;
  EXPECT_EQ(adler32(adler, buf, len), 2210660501);  // AOCL_Compression_zlib_adler32_common_4

  len = 19;
  EXPECT_EQ(adler32(adler, buf, len), 2314272953); // AOCL_Compression_zlib_adler32_common_5

  free(buf);
  buf = nullptr;
}

TEST(AOCL_Compression_zlib, adler32_combine_common)
{
  EXPECT_EQ(adler32_combine( 1, 0, 30), 0); // AOCL_Compression_zlib_adler32_combine_common_1
  EXPECT_EQ(adler32_combine( 1 << 31, 1 << 30, 1 << 30), 4291100656); // AOCL_Compression_zlib_adler32_combine_common_2
  EXPECT_EQ(adler32_combine( 1 << 31, 1 << 30, -1), 0xffffffffUL);
}

TEST(AOCL_Compression_zlib, crc32_combine_common)
{
  EXPECT_EQ(crc32_combine(1, 0, 30), 1012982536); // AOCL_Compression_zlib_crc32_combine_common_1
  EXPECT_EQ(crc32_combine( 1 << 30, 1 << 29, 1 << 28), 1114707486); // AOCL_Compression_zlib_crc32_combine_common_2
}

TEST(AOCL_Compression_zlib, crc32_combine_gen_common)
{
  EXPECT_EQ(crc32_combine_gen(30), 2654359659);
  EXPECT_EQ(crc32_combine_gen(1 << 28), 3303156796);
  EXPECT_EQ(crc32_combine_gen(0), 2147483648);
  EXPECT_EQ(crc32_combine_gen(LONG_MAX), 1832725837);
}

TEST(AOCL_Compression_zlib, crc32_combine_op_common)
{
  EXPECT_EQ(crc32_combine_op(1, 1, 1), 2852767882);
  EXPECT_EQ(crc32_combine_op(-1, -1, -1), 3246877207);
}

TEST(AOCL_Compression_zlib, crc32_common)
{
  z_size_t len = 1;
  Bytef *c = (Bytef *)malloc(len);

  for (z_size_t i = 0; i < len; i++)
  {
    c[i] = i % 255;
  }

  EXPECT_EQ(crc32(1, NULL, 1), 0);  // AOCL_Compression_zlib_crc32_common_1
  EXPECT_EQ(crc32(1, c, 0), 1);

  free(c);
  c = nullptr;
}

TEST(AOCL_Compression_zlib, crc32_z_common)
{
  z_size_t len = 1;
  Bytef *c = (Bytef *)malloc(len);

  for (z_size_t i = 0; i < len; i++)
  {
    c[i] = i % 255;
  }

  EXPECT_EQ(crc32_z(1, NULL, 1), 0);  // AOCL_Compression_zlib_crc32_z_common_1
  EXPECT_EQ(crc32_z(1, c, 0), 1);

  free(c);
  c = nullptr;
}

TEST(AOCL_Compression_zlib, zError_common)
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

#ifdef AOCL_ZLIB_OPT

TEST_P(AOCL_Compression_zlib, adler32_x86_common)
{
  size_t len = 5552;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }

  uLong adler = 1 << 16;
  len = 10;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len)); // AOCL_Compression_zlib_adler32_x86_common_1

  adler = 0;
  len = 10;
  EXPECT_EQ(adler32_x86(adler, NULL, len), adler32(adler, NULL, len));  // AOCL_Compression_zlib_adler32_x86_common_2

  adler = ((uLong)1L << 31) - 1;
  len = 1;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_3

  len = 10;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_4

  len = 19;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len)); // AOCL_Compression_zlib_adler32_x86_common_5

  len = 5552;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_6

  len = 64;
  adler = 1;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_7

  len = 1;
  adler = 0xFFFFFFFF;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_8

  len = 60;
  adler = 0xFFFFFFFF;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));  // AOCL_Compression_zlib_adler32_x86_common_9

  len = 0;
  EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));
  //This test case is moved to non-parameterized test to save runtime memory consumption
  //Non parameterized test will run only default optimized path
  //len = UINT32_MAX;
  //EXPECT_EQ(adler32_x86(adler, buf, len), adler32(adler, buf, len));

  free(buf);
  buf = nullptr;
}

TEST(AOCL_Compression_zlib, crc32_x86_common)
{
  size_t len = 5552;
  Bytef *buf = (Bytef *)malloc(len);
  for (size_t i = 0; i < len; i++)
  {
    buf[len - i - 1] = i % 255;
  }

  uLong crc = 1 << 16;
  len = 10;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_1

  crc = 0;
  len = 10;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_2

  crc = ((uLong)1L << 31) - 1;
  len = 1;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_3

  len = 10;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_4

  len = 19;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_5

  len = 5552;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_6

  len = 64;
  crc = 1;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_7

  len = 1;
  crc = 0xFFFFFFFF;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_8

  len = 60;
  crc = 0xFFFFFFFF;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_9

  len = 0;
  test_crc32_x86(crc, buf, len); // AOCL_Compression_zlib_crc32_x86_common_10

  crc = 1 << 16;
  len = 10;
  EXPECT_EQ(crc32_z(crc, NULL, len), 0);  // AOCL_Compression_zlib_crc32_x86_common_11
  
  free(buf);
  buf = nullptr;
}
#endif

#if defined(AOCL_ZLIB_OPT) && defined(AOCL_INTERNAL_TEST)
#ifdef __cplusplus
extern "C" {
    extern void AOCL_bi_flush(deflate_state* s);
    extern void AOCL_bi_windup(deflate_state* s);
}
#endif

/*=====================================
 *  Test cases for AOCL_bi_flush
 *=====================================*/
class ZLIB_AOCL_bi_flush : public ::testing::Test
{
private:
    ZLIB_deflate_stream deflateObj;
protected:
    z_streamp strm;
    int val;
    deflate_state* state;

    void SetUp() override
    {
        strm = deflateObj.get_stream();
        deflateInit(strm, 6);
        state = (deflate_state*)strm->state;
        val = 170; /* 1010 1010 */
        state->bi_valid = 0; /* number of bits set in state->bi_buf */
        state->pending = 0;  /* number of bytes present in state->pending_buf. */
    }
    ~ZLIB_AOCL_bi_flush()
    {
        strm = nullptr;
    }
};

TEST_F(ZLIB_AOCL_bi_flush, AOCL_Compression_AOCL_bi_flush_common_1)
{
    /* Setting 64 bits in bit buffer */
    for (int i = 0; i < 8; i++) {
        state->bi_buf |= (uint64_t)val << state->bi_valid;
        state->bi_valid += 8;
    }
    /* 64 bits are set in state->bi_buf */
    EXPECT_EQ(state->bi_valid, 64);

    AOCL_bi_flush(state);
    EXPECT_EQ(state->pending, 8);
    /* Checking if all the 64 bits are copied to state->pending_buf properly. */
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(state->pending_buf[state->pending - 8 + i], 170);
    }
    /* Number of bits remaining in buffer (should not be more than 7). */
    EXPECT_EQ(state->bi_valid, 0);  /* Expected number of bits in this case is zero. */
    EXPECT_EQ(state->bi_buf, 0);
}

TEST_F(ZLIB_AOCL_bi_flush, AOCL_Compression_AOCL_bi_flush_common_2)
{
    /* Setting 56 bits in bit buffer. */
    for (int i = 0; i < 7; i++) {
        state->bi_buf |= (uint64_t)(val+i) << state->bi_valid;
        state->bi_valid += 8;
    }
    /* Setting another 7 bits in bit buffer */
    int val2 = 122; /* "0111 1010" */
    state->bi_buf |= (uint64_t)val2 << state->bi_valid;
    state->bi_valid += 7;
    EXPECT_EQ(state->bi_valid, 63); /* Total of 63 bits are set in state->bi_buf */

    AOCL_bi_flush(state);

    EXPECT_EQ(state->pending, 7);
    /* Checking if AOCL_bi_flush() copies the data to state->pending_buf properly. */
    for (int i = 0; i < 7; i++) {
        EXPECT_EQ(state->pending_buf[state->pending - 7 + i], val + i);
    }
    /* Number of bits remaining in buffer (should not be more than 7). */
    EXPECT_EQ(state->bi_valid, 7); /* Expected number of bits in this case is 7. */
    EXPECT_EQ(state->bi_buf, 122);
}

TEST_F(ZLIB_AOCL_bi_flush, AOCL_Compression_AOCL_bi_flush_common_3)
{
    /* setting 32 bits in bit buffer */
    for (int i = 0; i < 4; i++) {
        state->bi_buf |= (uint64_t)(val+i) << state->bi_valid;
        state->bi_valid += 8;
    }
    /* setting another one bit in bit buffer */
    int one = 1; /* "0000 0001" */
    state->bi_buf |= (uint64_t)one << state->bi_valid;
    state->bi_valid += 1;

    /* total 33 bits are set */
    EXPECT_EQ(state->bi_valid, 33);

    AOCL_bi_flush(state);

    EXPECT_EQ(state->pending, 4);
    /* Checking if AOCL_bi_flush() copies the data to state->pending_buf properly. */
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(state->pending_buf[state->pending - 4 + i], val+i);
    }
    /* Number of bits remaining in buffer (should not be more than 7). */
    EXPECT_EQ(state->bi_valid, 1);  /* Expected number of bits in this case is 1. */
    EXPECT_EQ(state->bi_buf, 1);
}

TEST_F(ZLIB_AOCL_bi_flush, AOCL_Compression_AOCL_bi_flush_common_4)  /* state->bi_valid > 8 */
{
    /* setting 8 bits in bit buffer */
    state->bi_buf |= (uint64_t)(val) << state->bi_valid;
    state->bi_valid += 8;

    /* setting another one bit in bit buffer */
    int one = 1; /* "0000 0001" */
    state->bi_buf |= (uint64_t)one << state->bi_valid;
    state->bi_valid += 1;

    /* total 9 bits are set */
    EXPECT_EQ(state->bi_valid, 9);

    AOCL_bi_flush(state);

    EXPECT_EQ(state->pending, 1);
    /* Checking if AOCL_bi_flush() copies the data to state->pending_buf properly. */

    EXPECT_EQ(state->pending_buf[state->pending - 1], val);

    /* Number of bits remaining in buffer (should not be more than 7). */
    EXPECT_EQ(state->bi_valid, 1);  /* Expected number of bits in this case is 1. */
    EXPECT_EQ(state->bi_buf, 1);
}

/*=====================================
 *  Test cases for AOCL_bi_windup
 *=====================================*/
class ZLIB_AOCL_bi_windup : public ZLIB_AOCL_bi_flush {
};

TEST_F(ZLIB_AOCL_bi_windup, AOCL_Compression_zlib_AOCL_bi_windup_common_1)
{
    /* Setting 56 bits in bit buffer */
    for (int i = 0; i < 7; i++) {
        state->bi_buf |= (uint64_t)(val+i) << state->bi_valid;
        state->bi_valid += 8;
    }
    /* Setting another one bit in bit buffer */
    int one = 1; /* " 0000 0001" */
    state->bi_buf |= (uint64_t)one << state->bi_valid;
    state->bi_valid += 1;

    /* 57 bits are set in state->bi_buf */
    EXPECT_EQ(state->bi_valid, 57);

    AOCL_bi_windup(state);

    EXPECT_EQ(state->pending, 8);
    /* Checking if AOCL_bi_windup flushes the bit buffer to state->pending_buf properly. */
    for (int i = 0; i < 7; i++) {
        EXPECT_EQ(state->pending_buf[state->pending - 8 + i], 170+i);
    }
    EXPECT_EQ(state->pending_buf[state->pending - 8 + 7], 1);

    EXPECT_EQ(state->bi_valid, 0);
    EXPECT_EQ(state->bi_buf, 0);
}

TEST_F(ZLIB_AOCL_bi_windup, AOCL_Compression_zlib_AOCL_bi_windup_common_2)
{
    /* Setting 48 bits in bit buffer */
    for (int i = 0; i < 6; i++) {
        state->bi_buf |= (uint64_t)(val+i) << state->bi_valid;
        state->bi_valid += 8;
    }
    /* Setting another 7 bits in bit buffer */
    int val2 = 122; /* "0111 1010" */
    state->bi_buf |= (uint64_t)val2 << state->bi_valid;
    state->bi_valid += 7;

    /* 55 bits are set in state->bi_buf */
    EXPECT_EQ(state->bi_valid, 55);

    AOCL_bi_windup(state);

    EXPECT_EQ(state->pending, 7);
    /* Checking if AOCL_bi_windup flushes the bit buffer to state->pending_buf properly. */
    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(state->pending_buf[state->pending - 7 + i], val+i);
    }
    EXPECT_EQ(state->pending_buf[state->pending - 7 + 6], 122);

    EXPECT_EQ(state->bi_valid, 0);
    EXPECT_EQ(state->bi_buf, 0);
}

TEST_F(ZLIB_AOCL_bi_windup, AOCL_Compression_zlib_AOCL_bi_windup_common_3)  /* state->bi_valid > 8 */
{
    /* Setting 8 bits in bit buffer */
    state->bi_buf |= (uint64_t)(val) << state->bi_valid;
    state->bi_valid += 8;

    /* Setting another 1 bits in bit buffer */
    int one = 1; /* "0000 0001" */
    state->bi_buf |= (uint64_t)one << state->bi_valid;
    state->bi_valid += 1;

    /* 9 bits are set in state->bi_buf */
    EXPECT_EQ(state->bi_valid, 9);

    AOCL_bi_windup(state);

    EXPECT_EQ(state->pending, 2);
    /* Checking if AOCL_bi_windup flushes the bit buffer to state->pending_buf properly. */
    EXPECT_EQ(state->pending_buf[state->pending - 2], 170);
    EXPECT_EQ(state->pending_buf[state->pending - 1], 1);

    EXPECT_EQ(state->bi_valid, 0);
    EXPECT_EQ(state->bi_buf, 0);
}

TEST_F(ZLIB_AOCL_bi_windup, AOCL_Compression_zlib_AOCL_bi_windup_common_4)  /* state->bi_valid > 0 */
{
    /* Setting another 1 bits in bit buffer */
    int one = 1; /* "0000 0001" */
    state->bi_buf |= (uint64_t)one << state->bi_valid;
    state->bi_valid += 1;

    /* 1 bit are set in state->bi_buf */
    EXPECT_EQ(state->bi_valid, 1);

    AOCL_bi_windup(state);

    EXPECT_EQ(state->pending, 1); /* number of bytes moved to pending buffer on calling AOCL_bi_windup. */
    /* Checking if AOCL_bi_windup flushes the bit buffer to state->pending_buf properly. */
    EXPECT_EQ(state->pending_buf[state->pending - 1], 1);

    EXPECT_EQ(state->bi_valid, 0);
    EXPECT_EQ(state->bi_buf, 0);
}

/*=====================================
 *  Test cases for AOCL_send_bits
 *=====================================*/
class ZLIB_AOCL_send_bits : public ::testing::Test
{
private:
    ZLIB_deflate_stream deflateObj;
protected:
    z_streamp strm;
    int val;
    deflate_state* state;
    int value_to_send;
    int length;

    void SetUp() override
    {
        strm = deflateObj.get_stream();
        deflateInit(strm, 6);
        state = (deflate_state*)strm->state;
        val = 170; /* 1010 1010 */
        state->bi_valid = 0; /* number of bits set in state->bi_buf */
        state->pending = 0;  /* number of bytes present in pending buffer. */
    }

    ~ZLIB_AOCL_send_bits()
    {
        strm = nullptr;
    }
};

TEST_F(ZLIB_AOCL_send_bits, AOCL_Compression_zlib_AOCL_send_bits_common_1)
{
    /* Check if data is kept safe in bit buffer when it is empty. */
    value_to_send = 358;    /* "1 0110 0110" */
    length = 9;             /* bit length of value to send */

    /* Storing to confirm that AOCL_send_bits doesn't change this value for length < 64. */
    int pending_count = state->pending;  /* number of valid bytes in pending buffer */
    memset(state->pending_buf, 0, state->pending_buf_size);
    uint8_t byte1 = state->pending_buf[pending_count];
    uint8_t byte2 = state->pending_buf[pending_count + 1];

    AOCL_send_bits(state, value_to_send, length);
    /* Nothing is copied to pending buffer untill bi buffer is full. */

    EXPECT_EQ(state->bi_buf, 358);
    EXPECT_EQ(state->bi_valid, 9);

    /* Check to confirm no data byte copied to state->pending_buf */
    EXPECT_EQ(state->pending, pending_count);
    /* In case data is copied to state->pending_buf, it will be atmost 2 bytes,
     * because length sent above is 9. */
    EXPECT_EQ(state->pending_buf[pending_count], byte1);
    EXPECT_EQ(state->pending_buf[pending_count + 1], byte2);
}

TEST_F(ZLIB_AOCL_send_bits, AOCL_Compression_zlib_AOCL_send_bits_common_2)
{
    /* Check if data is copied properly to pending buffer when bit buffer is already full. */
    value_to_send = 358;    /* "1 0110 0110" */
    length = 9;             /* bit length of value to send */
    for (int i = 0; i < 8; i++) {
        state->bi_buf |= (uint64_t)(val+i) << state->bi_valid;
        state->bi_valid += 8;
    }
    AOCL_send_bits(state, value_to_send, length);

    EXPECT_EQ(state->pending, 8);    /* 8 Bytes moved to pending buffer, remainig bits are stored in bit buffer. */

    for (int i = 0; i < 7; i++) {
        EXPECT_EQ(state->pending_buf[state->pending - 8 + i], val+i);
    }
    EXPECT_EQ(state->bi_buf, 358);
    EXPECT_EQ(state->bi_valid, 9);
}

TEST_F(ZLIB_AOCL_send_bits, AOCL_Compression_zlib_AOCL_send_bits_common_3)
{
    /* Check if data is copied properly to pending buffer when bit buffer may overflow because of new insert. */
    value_to_send = 359;    /* "1 0110 0111" */
    length = 9;             /* bit length of value to send */

    /* Setting 56 bits of bit buffer */
    for (int i = 0; i < 7; i++) {
        state->bi_buf |= (uint64_t)(val+i) << state->bi_valid;
        state->bi_valid += 8;
    }
    /* Setting another 7 bits in bit buffer */
    int val2 = 122; /* "0111 1010" */
    state->bi_buf |= (uint64_t)val2 << state->bi_valid;
    state->bi_valid += 7;
    /* 63 Bits in bit buffer  be like "122 176 175 174 173 172 171 170". */
    /* Least significant bit from value_to_send will be inserted in bit buffer. */
    /* 63 bits of bit buffer will be " 250 176 175 174 173 172 171 170" */
    AOCL_send_bits(state, value_to_send, length); /* Copies data from bit buffer when it is full to pending buffer(LSB First). */

    EXPECT_EQ(state->pending, 8);

    for (int i = 0; i < 7; i++) {
        EXPECT_EQ(state->pending_buf[state->pending - 8 + i], val+i);
    }
    EXPECT_EQ(state->pending_buf[state->pending - 8 + 7], 250);

    /* Least sinificant bit is already inserted into pending buffer,
     * Remaining bits of value_to_send are present in bit buffer which are "1011 0011" which is 179. */
    EXPECT_EQ(state->bi_buf, 179);
    EXPECT_EQ(state->bi_valid, 8);
}
#endif /* AOCL_ZLIB_OPT && AOCL_INTERNAL_TEST */

/*********************************************
 * Begin fuzz tests for zlib
 *********************************************/
#ifdef AOCL_TEST_FUZZER
void compress2_fuzz(vector<Bytef> source, size_t dest_sz,
                    int level, int optOff, int optLevel)
{
  aocl_setup_zlib(optOff, optLevel, 0, 0, 0);

  uLong destLen = dest_sz > ULONG_MAX ? ULONG_MAX : dest_sz;
  uLong srcLen = source.size();
  vector<Bytef> dest(destLen, 0);

  compress2(dest.data(), &destLen, (const Bytef *)source.data(), srcLen, level);
  aocl_destroy_zlib();
}
FUZZ_TEST(AOCL_Compression_zlib, compress2_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<Bytef>>(),
            fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
            fuzztest::InRange<int>(-1, 9),
            fuzztest::InRange<int>(0, 1),
            fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_cpr_seed_t<Bytef> {
  auto seed_files = READ_FUZZ_CPR_SEED();
  return get_fuzz_cpr_seeds<Bytef>([](size_t src_sz) -> size_t {
    size_t dst_sz = (size_t)compressBound((uLong)src_sz);
    return limit_fuzz_size_max(dst_sz);
  }, -1, 9, seed_files);
})
#endif
;

void uncompress_fuzz(vector<Bytef> source, size_t dest_sz,
                    int optOff, int optLevel)
{
  aocl_setup_zlib(optOff, optLevel, 0, 0, 0);

  uLong destLen = dest_sz;
  uLong srcLen = source.size();
  vector<Bytef> dest(destLen, 0);

  uncompress(dest.data(), &destLen, source.data(), srcLen);

  aocl_destroy_zlib();
}
FUZZ_TEST(AOCL_Compression_zlib, uncompress_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<Bytef>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_dpr_seed_t<Bytef> {
  auto seed_files = READ_FUZZ_DPR_SEED();
  return get_fuzz_dpr_seeds<Bytef>(seed_files);
})
#endif
;

#ifdef AOCL_ENABLE_THREADS
void compress2_gzip_fuzz(vector<Bytef> source, size_t dest_sz,
                    int level, int optOff, int optLevel)
{
  aocl_setup_zlib(optOff, optLevel, 0, 0, 0);

  uLong destLen = dest_sz > ULONG_MAX ? ULONG_MAX : dest_sz;
  uLong srcLen = source.size();
  vector<Bytef> dest(destLen, 0);

  compress2_gzip(dest.data(), &destLen, (const Bytef *)source.data(), srcLen, level);
  aocl_destroy_zlib();
}
FUZZ_TEST(AOCL_Compression_zlib, compress2_gzip_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<Bytef>>(),
            fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
            fuzztest::InRange<int>(-1, 9),
            fuzztest::InRange<int>(0, 1),
            fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_cpr_seed_t<Bytef> {
  auto seed_files = READ_FUZZ_CPR_SEED();
  return get_fuzz_cpr_seeds<Bytef>([](size_t src_sz) -> size_t {
    size_t dst_sz = (size_t)compressBound_gzip((uLong)src_sz);
    return limit_fuzz_size_max(dst_sz);
  }, -1, 9, seed_files);
})
#endif
;

void uncompress2_gzip_fuzz(vector<Bytef> source, size_t dest_sz,
                    int optOff, int optLevel)
{
  aocl_setup_zlib(optOff, optLevel, 0, 0, 0);

  uLong destLen = dest_sz;
  uLong srcLen = source.size();
  vector<Bytef> dest(destLen, 0);

  uncompress2_gzip(dest.data(), &destLen, source.data(), &srcLen);

  aocl_destroy_zlib();
}
FUZZ_TEST(AOCL_Compression_zlib, uncompress2_gzip_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<Bytef>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_dpr_seed_t<Bytef> {
  auto seed_files = READ_FUZZ_DPR_SEED();
  return get_fuzz_dpr_seeds<Bytef>(seed_files);
})
#endif
;

void compress2_raw_fuzz(vector<Bytef> source, size_t dest_sz,
                    int level, int optOff, int optLevel)
{
  aocl_setup_zlib(optOff, optLevel, 0, 0, 0);

  uLong destLen = dest_sz > ULONG_MAX ? ULONG_MAX : dest_sz;
  uLong srcLen = source.size();
  vector<Bytef> dest(destLen, 0);

  compress2_raw(dest.data(), &destLen, (const Bytef *)source.data(), srcLen, level);
  aocl_destroy_zlib();
}
FUZZ_TEST(AOCL_Compression_zlib, compress2_raw_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<Bytef>>(),
            fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
            fuzztest::InRange<int>(-1, 9),
            fuzztest::InRange<int>(0, 1),
            fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_cpr_seed_t<Bytef> {
  auto seed_files = READ_FUZZ_CPR_SEED();
  return get_fuzz_cpr_seeds<Bytef>([](size_t src_sz) -> size_t {
    size_t dst_sz = (size_t)compressBound((uLong)src_sz);
    return limit_fuzz_size_max(dst_sz);
  }, -1, 9, seed_files);
})
#endif
;

void uncompress2_raw_fuzz(vector<Bytef> source, size_t dest_sz,
                    int optOff, int optLevel)
{
  aocl_setup_zlib(optOff, optLevel, 0, 0, 0);

  uLong destLen = dest_sz;
  uLong srcLen = source.size();
  vector<Bytef> dest(destLen, 0);

  uncompress2_raw(dest.data(), &destLen, source.data(), &srcLen);

  aocl_destroy_zlib();
}
FUZZ_TEST(AOCL_Compression_zlib, uncompress2_raw_fuzz)
.WithDomains(fuzztest::Arbitrary<vector<Bytef>>(),
             fuzztest::InRange<size_t>(0, READ_FUZZ_SIZE_MAX()),
             fuzztest::InRange<int>(0, 1),
             fuzztest::InRange<int>(0, 4))
#ifdef AOCL_TEST_FUZZER_WITH_CORPUS
.WithSeeds([]() -> fuzz_dpr_seed_t<Bytef> {
  auto seed_files = READ_FUZZ_DPR_SEED();
  return get_fuzz_dpr_seeds<Bytef>(seed_files);
})
#endif
;
#endif /* AOCL_ENABLE_THREADS */
#endif /* AOCL_TEST_FUZZER */
/*********************************************
 * End fuzz tests for zlib
 *********************************************/
