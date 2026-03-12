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

 /** @file zlib_gtest.h
 *  
 *  @brief Common utility classes and functions used by zlib gtests.
 *
 *  This file contains common utility classes and functions used
 *  by zlib gtests.
 *
 *  @author Ravi Jangra
 */

#ifndef _ZLIB_GTEST_H_
#define _ZLIB_GTEST_H_

#include <string>

#ifdef AOCL_TEST_FUZZER
#include "fuzztest/fuzztest.h"
#endif

#include "utils/utils.h" // Note: include this before deflate.h
#include "utils/dispatcher.h"
#include "algos/zlib/zlib.h"
#include "algos/zlib/zutil.h"
#include "algos/zlib/inftrees.h"
#include "algos/zlib/inflate.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "algos/zlib/deflate.h" // header file is internal to zlib module
#ifdef __cplusplus
}
#endif
#include "algos/zlib/aocl_send_bits.h"
#include "algos/zlib/crc32_x86.h"
#include "api/aocl_compression.h"
#include "gtest/gtest.h"

using namespace std;

#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )

// class for running same gtest for different optimization levels and AOCL_ZLIB_QUICK_MODE
class AOCL_Compression_zlib : public ::testing::TestWithParam<tuple<int, int>> {
    void SetUp() override{
        int enable_dquick = get<0>(::testing::TestWithParam<tuple<int, int>>::GetParam());
        test_aocl_zlib_set_enable_dquick(enable_dquick);

        int optLevel = get<1>(::testing::TestWithParam<tuple<int, int>>::GetParam());
        aocl_setup_zlib(0, optLevel, 0, 0, 0);
    }

    void TearDown() override {
        test_aocl_zlib_set_enable_dquick(0);
        aocl_destroy_zlib();
    }
};

// base class for ZLIB stream management
class ZLIB_stream {
  protected:
    z_streamp s = nullptr;

    void generate_zstream(void) {
      s = new z_stream;
      memset(s, 0, sizeof(z_stream));
    }

    void release_zstream(void) {
      if(s != nullptr)
        delete s;
      s = nullptr;
    }

    void reset_zstream(void) {
      release_zstream();
      generate_zstream();
    }

  public:
    ZLIB_stream(void) {
      generate_zstream();
    }

    virtual ~ZLIB_stream(void) {
      release_zstream();
    }

    z_streamp get_stream(void) {
      return s;
    }

};

// concrete classe for ZLIB stream management for deflate
class ZLIB_deflate_stream : public ZLIB_stream {
  public:
    ~ZLIB_deflate_stream(void) {
      deflateEnd(get_stream());
    }

    void reset_deflate_stream(void) {
      deflateEnd(get_stream());
      reset_zstream();
    }
};

// concrete classe for ZLIB stream management for inflate
class ZLIB_inflate_stream : public ZLIB_stream {
  public:
    ~ZLIB_inflate_stream(void) {
      inflateEnd(get_stream());
    }

    void reset_inflate_stream(void) {
      inflateEnd(get_stream());
      reset_zstream();
    }
};

// comparision of two different data types
static bool cmpr(const char *c1, char *c2, int j)
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
static int prepare_deflate_stream(z_stream *ds, Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
  int err;
  const uInt max = (uInt)-1;
  uLong left;

  left = *destLen;
  *destLen = 0;

  ds->zalloc = (alloc_func)0;
  ds->zfree = (free_func)0;
  ds->opaque = (voidpf)0;

  err = deflateInit(ds, level);
  if (err != Z_OK)
    return err;

  ds->next_out = dest;
  ds->avail_out = 0;
  ds->next_in = (z_const Bytef *)source;
  ds->avail_in = 0;
  if (ds->avail_out == 0)
  {
    ds->avail_out = left > (uLong)max ? max : (uInt)left;
    left -= ds->avail_out;
  }
  if (ds->avail_in == 0)
  {
    ds->avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
    sourceLen -= ds->avail_in;
  }

  return Z_OK;
}

static void* alloc_null(void* opaque, uInt items, uInt size)
{
  return nullptr;
}
#endif /* _ZLIB_GTEST_H_ */
