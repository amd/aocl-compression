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
 *  + renamed main() to zstd_bigdict_main().
*/

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "datagen.h"
#include "mem.h"
#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

static int
compress(ZSTD_CCtx* cctx, ZSTD_DCtx* dctx,
         void* dst, size_t dstCapacity,
         void const* src, size_t srcSize,
         void* roundtrip, ZSTD_EndDirective end)
{
    ZSTD_inBuffer in = {src, srcSize, 0};
    ZSTD_outBuffer out = {dst, dstCapacity, 0};
    int ended = 0;

    while (!ended && (in.pos < in.size || out.pos > 0)) {
        size_t rc;
        out.pos = 0;
        rc = ZSTD_compressStream2(cctx, &out, &in, end);
        if (ZSTD_isError(rc))
            return 1;
        if (end == ZSTD_e_end && rc == 0)
            ended = 1;
        {
            ZSTD_inBuffer rtIn = {dst, out.pos, 0};
            ZSTD_outBuffer rtOut = {roundtrip, srcSize, 0};
            rc = 1;
            while (rtIn.pos < rtIn.size || rtOut.pos > 0) {
                rtOut.pos = 0;
                rc = ZSTD_decompressStream(dctx, &rtOut, &rtIn);
                if (ZSTD_isError(rc)) {
                    fprintf(stderr, "Decompression error: %s\n", ZSTD_getErrorName(rc));
                    return 1;
                }
                if (rc == 0)
                    break;
            }
            if (ended && rc != 0) {
                fprintf(stderr, "Frame not finished!\n");
                return 1;
            }
        }
    }

    return 0;
}

int zstd_bigdict_main(int argc, char** argv)
{
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    const size_t dataSize = (size_t)1 << 30;
    const size_t outSize = ZSTD_compressBound(dataSize);
    const size_t bufferSize = (size_t)1 << 31;
    char* buffer = (char*)malloc(bufferSize);
    void* out = malloc(outSize);
    void* roundtrip = malloc(dataSize);
    (void)argc;
    (void)argv;

    if (!buffer || !out || !roundtrip || !cctx || !dctx) {
        fprintf(stderr, "Allocation failure\n");
        return 1;
    }

    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, 31)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 1)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_overlapLog, 9)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 1)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_strategy, ZSTD_btopt)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_targetLength, 7)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_minMatch, 7)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_searchLog, 1)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_hashLog, 10)))
        return 1;
    if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_chainLog, 10)))
        return 1;

    if (ZSTD_isError(ZSTD_DCtx_setParameter(dctx, ZSTD_d_windowLogMax, 31)))
        return 1;

    RDG_genBuffer(buffer, bufferSize, 1.0, 0.0, 0xbeefcafe);

    /* Compress 30 GB */
    {
        int i;
        for (i = 0; i < 10; ++i) {
            fprintf(stderr, "Compressing 1 GB\n");
            if (compress(cctx, dctx, out, outSize, buffer, dataSize, roundtrip, ZSTD_e_continue))
                return 1;
        }
    }
    fprintf(stderr, "Compressing 1 GB\n");
    if (compress(cctx, dctx, out, outSize, buffer, dataSize, roundtrip, ZSTD_e_end))
        return 1;

    fprintf(stderr, "Success!\n");

    free(roundtrip);
    free(out);
    free(buffer);
    ZSTD_freeDCtx(dctx);
    ZSTD_freeCCtx(cctx);
    return 0;
}
