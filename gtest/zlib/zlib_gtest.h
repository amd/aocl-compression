/**
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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
#include <string>

#include "algos/zlib/zlib.h"
#include "algos/zlib/zutil.h"
#include "algos/zlib/inftrees.h"
#include "algos/zlib/inflate.h"
#include "algos/zlib/deflate.h"
#include "algos/zlib/aocl_zlib_test.h"
#include "algos/zlib/aocl_send_bits.h"
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

// This is for creating a new z_stream data type
static z_streamp get_z_stream(void)
{
  z_streamp s = new z_stream;
  memset(s, 0, sizeof(z_stream));

  return s;
}

// This is to free z_stream
static void release_z_stream(z_streamp &s)
{
    if(s != nullptr)
      delete s;
    s = nullptr;
}

// This releases memory that is dynamically allocated for deflate pointers
static void release_deflate_stream(z_streamp &ds)
{
  deflateEnd(ds);
  release_z_stream(ds);
}

// This releases memory that is dynamically allocated pointers
static void release_inflate_stream(z_streamp &is)
{
  inflateEnd(is);
  release_z_stream(is);
}

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
