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

 /** @file aocl_thirdparty_zstd_test.h
 *  
 *  @brief Function definitions for common functions used in testing ZSTD.
 *
 *  @author Ravi Jangra
 */
#ifndef AOCL_THIRDPARTY_ZSTD_TEST_H
#define AOCL_THIRDPARTY_ZSTD_TEST_H

#ifdef AOCL_FDS_CORRECTION
static size_t cnt = 0;
#define ZSTD_FDS_FRAME_SIZE (FDS_FRAME_LENGTH + ZSTD_SKIPPABLEHEADERSIZE)
/* Decompress multiple frames in input. */
static size_t Test_decompressStreamMultiple(ZSTD_DStream* zds, ZSTD_outBuffer* output, ZSTD_inBuffer* input) {
    /* As multiple frames might be present, exiting on ret == 0 will return on 1st frame. 
     * Instead call ZSTD_decompressStream multiple times. 
     * If ret = 0 and input->pos < input->size : More frames exist in input. Continue to process.
     * If ret = 0 and input->pos = input->size : All frames processed. Exit.
     * If ret != 0 and input->pos = input->size : More input is needed. Exit. */
    size_t ret = 0;
    while (!ret && input->pos < input->size) {
        ret = ZSTD_decompressStream(zds, output, input);
        if (ZSTD_isError(ret)) {
            LOG_FORMATTED(ERR, logCtx, "%s",ZSTD_getErrorName(ret));
            return ret;
        }
        if (!ret && input->pos < input->size && 
            !ZSTD_isFrame(input->src + input->pos, input->size - input->pos)) {
            /* Subsequent data in input->pos is not a valid frame. Most likely all input
             * has been consumed and remaining data in input is garbage. Return. */
            return ret;
        }
    }
    return ret;
}

/* Same as Test_decompressStreamMultiple but with input_size provided externally */
static size_t Test_decompressStreamMultipleWithSize(ZSTD_DStream* zds, ZSTD_outBuffer* output, ZSTD_inBuffer* input,
    size_t input_size) {
    size_t ret = 0;
    while (!ret && input->pos < input_size) {
        ret = ZSTD_decompressStream(zds, output, input);
        if (ZSTD_isError(ret)) {
            LOG_FORMATTED(ERR, logCtx, "%s", ZSTD_getErrorName(ret));
            return ret;
        }
        if (!ret && input->pos < input_size &&
            !ZSTD_isFrame(input->src + input->pos, input_size - input->pos)) {
            return ret;
        }
    }
    return ret;
}

/* Skip skippable frames if any until a zstd frame is found. */
static size_t Test_skipSkippableFrames(const BYTE* src, size_t srcSize) {
    const BYTE* cur = src;
    while (ZSTD_isSkippableFrame(cur, srcSize)) {
        ZSTD_frameHeader zfh;
        size_t ret = ZSTD_getFrameHeader(&zfh, cur, srcSize);
        if (ZSTD_isError(ret)) return ret;
        cur += (zfh.frameContentSize + ZSTD_SKIPPABLEHEADERSIZE);
    }
    return (size_t)(cur - src);
}

static size_t Test_refCompress(void* dst, size_t dstCapacity, const void* src, size_t srcSize, int cLevel) {
    ZSTD_CCtx* cctx_fds = ZSTD_createCCtx();
    ZSTD_CCtx_setParameter(cctx_fds, ZSTD_c_compressionLevel, cLevel);
    size_t disableFdsFrame = 1;
    Test_ZSTD_CCtx_setFdsRuntimeParams(cctx_fds, disableFdsFrame); // no additional FDS frames
    size_t g_cSize = ZSTD_compress2(cctx_fds, dst, dstCapacity, src, srcSize);
    ZSTD_freeCCtx(cctx_fds);
    return g_cSize;
}

#else
 /* Wrapper ZSTD_decompressStream */
static size_t Test_decompressStreamMultipleWithSize(ZSTD_DStream* zds, ZSTD_outBuffer* output, ZSTD_inBuffer* input,
    size_t input_size) {
    (void)input_size;
    return ZSTD_decompressStream(zds, output, input);
}
#endif /* AOCL_FDS_CORRECTION */
#endif /* AOCL_THIRDPARTY_ZSTD_TEST_H */
