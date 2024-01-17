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

 /*
  * Copyright (c) Meta Platforms, Inc. and affiliates.
  * All rights reserved.
  *
  * This source code is licensed under both the BSD-style license (found in the
  * LICENSE file in the root directory of this source tree) and the GPLv2 (found
  * in the COPYING file in the root directory of this source tree).
  * You may select, at your option, one of the above-listed licenses.
  * 
  * https://github.com/facebook/zstd/blob/dev/tests/fullbench.c#L345
  */
 
 /** @file zstd_stream_gtest.cpp
 *  
 *  @brief Test cases for ZSTD APIs that use dictionary.
 *
 *  This file contains the test cases for ZSTD method
 *  testing the API level functions of ZSTD that use dictionary.
 *
 *  @author Ashish Sriram
 */
#include "zstd_gtest.h"

/***********************************************
* Begin of ZSTD_ZSTD_compress_extDict
***********************************************/
#define FIRST_BLOCK_SIZE 8
class ZSTD_ZSTD_compress_extDict : public AOCL_setup_zstd {
public:
    ~ZSTD_ZSTD_compress_extDict() {
        if (g_zcc)
            ZSTD_freeCCtx(g_zcc);
    }

    ZSTD_parameters setup(int level, int opt_on) {
        ZSTD_compressionParameters cparams = Test_Get_ZSTD_defaultCParameters(512 KB, level, opt_on);
        if (g_zcc == NULL) g_zcc = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_compressionLevel, level);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_windowLog, (int)cparams.windowLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_hashLog, (int)cparams.hashLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_chainLog, (int)cparams.chainLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_searchLog, (int)cparams.searchLog);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_minMatch, (int)cparams.minMatch);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_targetLength, (int)cparams.targetLength);
        ZSTD_CCtx_setParameter(g_zcc, ZSTD_c_strategy, cparams.strategy);

        ZSTD_parameters p;
        ZSTD_frameParameters f = { 1 /* contentSizeHeader*/, 0, 0 };
        p.fParams = f;
        p.cParams = cparams;

        return p;
    }

    /* compress in multiple blocks. 1st block gets used as dict for 2nd block and
    *_extDict functions gets called for the 2nd block */
    size_t multi_block_compress(ZSTD_parameters p, void* dst, size_t dstCapacity, void* src, size_t srcSize) {
        BYTE firstBlockBuf[FIRST_BLOCK_SIZE];
        size_t ret = ZSTD_compressBegin_advanced(g_zcc, NULL, 0, p, srcSize);
        if (Test_ZSTD_isError(ret)) {
            EXPECT_EQ(ret, 0); //ret > 0 if error. Test should fail.
            return 0;
        }
        memcpy(firstBlockBuf, src, FIRST_BLOCK_SIZE);

        size_t outLen = 0;
        //compress first block
        //first block acts as extDict for 2nd block
        {
            ret = ZSTD_compressContinue(g_zcc,
                dst, dstCapacity,
                firstBlockBuf, FIRST_BLOCK_SIZE);
            if (Test_ZSTD_isError(ret)) {
                EXPECT_EQ(ret, 0); //ret > 0 if error. Test should fail.
                return 0;
            }
            dst = (BYTE*)dst + ret;
            dstCapacity -= ret;
            outLen += ret;
        }

        //compress second block
        ret = ZSTD_compressEnd(g_zcc, dst, dstCapacity,
            (const BYTE*)src + FIRST_BLOCK_SIZE,
            srcSize - FIRST_BLOCK_SIZE);
        if (Test_ZSTD_isError(ret)) {
            EXPECT_EQ(ret, 0); //ret > 0 if error. Test should fail.
            return 0;
        }
        outLen += ret;

        return outLen;
    }

private:
    ZSTD_CCtx* g_zcc = NULL;
};


TEST_F(ZSTD_ZSTD_compress_extDict, AOCL_Compression_zstd_ZSTD_compressStream_common_1) //compress multiple blocks
{
    for (int level = 0; level <= 22; ++level) {
        for (int opt_on = 0; opt_on <= 1; ++opt_on) {
            //setup
            TestLoad_2 d(1024);
            void* src = d.getOrigData();
            size_t srcSize = d.getOrigSize();
            void* dst = d.getCompressedBuff();
            size_t dstCapacity = d.getCompressedSize();
            ZSTD_parameters p = setup(level, opt_on);

            //compress
            size_t outLen = multi_block_compress(p, dst, dstCapacity, src, srcSize);

            //validate
            EXPECT_TRUE(zstd_check_uncompressed_equal_to_original(d.getOrigData(), d.getOrigSize(), d.getCompressedBuff(), outLen, ZSTD_decompressDCtx));
        }
    }
}

/*********************************************
 * End of ZSTD_ZSTD_compress_extDict
 *********************************************/
