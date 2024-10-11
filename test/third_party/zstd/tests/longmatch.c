/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

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

/* AOCL changes:
 *  + renamed main() to zstd_longmatch_main().
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"
#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

static int
compress(ZSTD_CStream *ctx, ZSTD_outBuffer out, const void *data, size_t size)
{
  ZSTD_inBuffer in = { data, size, 0 };
  while (in.pos < in.size) {
    ZSTD_outBuffer tmp = out;
    const size_t rc = ZSTD_compressStream(ctx, &tmp, &in);
    if (ZSTD_isError(rc)) return 1;
  }
  { ZSTD_outBuffer tmp = out;
    const size_t rc = ZSTD_flushStream(ctx, &tmp);
    if (rc != 0) { return 1; }
  }
  return 0;
}

int zstd_longmatch_main(int argc, char** argv)
{
  ZSTD_CStream* ctx;
  unsigned windowLog = 18;
  (void)argc;
  (void)argv;
  /* Create stream */
  ctx = ZSTD_createCCtx();
  if (!ctx) { return 1; }
  /* Set parameters */
  if (ZSTD_isError(ZSTD_CCtx_setParameter(ctx, ZSTD_c_windowLog, windowLog)))
    return 2;
  if (ZSTD_isError(ZSTD_CCtx_setParameter(ctx, ZSTD_c_chainLog, 13)))
    return 2;
  if (ZSTD_isError(ZSTD_CCtx_setParameter(ctx, ZSTD_c_hashLog, 14)))
    return 2;
  if (ZSTD_isError(ZSTD_CCtx_setParameter(ctx, ZSTD_c_searchLog, 1)))
    return 2;
  if (ZSTD_isError(ZSTD_CCtx_setParameter(ctx, ZSTD_c_minMatch, 7)))
    return 2;
  if (ZSTD_isError(ZSTD_CCtx_setParameter(ctx, ZSTD_c_targetLength, 16)))
    return 2;
  if (ZSTD_isError(ZSTD_CCtx_setParameter(ctx, ZSTD_c_strategy, ZSTD_fast)))
    return 2;
  {
    U64 compressed = 0;
    const U64 toCompress = ((U64)1) << 33;
    const size_t size = 1 << windowLog;
    size_t pos = 0;
    char *srcBuffer = (char*) malloc(1 << windowLog);
    char *dstBuffer = (char*) malloc(ZSTD_compressBound(1 << windowLog));
    ZSTD_outBuffer out = { dstBuffer, ZSTD_compressBound(1 << windowLog), 0 };
    const char match[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t randomData = (1 << windowLog) - 2*sizeof(match);
    size_t i;
    printf("\n ===   Long Match Test   === \n");
    printf("Creating random data to produce long matches \n");
    for (i = 0; i < sizeof(match); ++i) {
      srcBuffer[i] = match[i];
    }
    for (i = 0; i < randomData; ++i) {
      srcBuffer[sizeof(match) + i] = (char)(rand() & 0xFF);
    }
    for (i = 0; i < sizeof(match); ++i) {
      srcBuffer[sizeof(match) + randomData + i] = match[i];
    }
    printf("Compressing, trying to generate a segfault \n");
    if (compress(ctx, out, srcBuffer, size)) {
      return 1;
    }
    compressed += size;
    while (compressed < toCompress) {
      const size_t block = rand() % (size - pos + 1);
      if (pos == size) { pos = 0; }
      if (compress(ctx, out, srcBuffer + pos, block)) {
        return 1;
      }
      pos += block;
      compressed += block;
    }
    printf("Compression completed successfully (no error triggered)\n");
    free(srcBuffer);
    free(dstBuffer);
  }
  ZSTD_freeCCtx(ctx);
  return 0;
}
