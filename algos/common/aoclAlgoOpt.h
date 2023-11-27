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

 /** @file aoclAlgoOpt.h
 *
 *  @brief AOCL Optimization flags.
 *
 *  For native compilation comment out below flags based on ISA support
 *
 *  @author Ashish Sriram
 */

/* BZIP2 */
#define AOCL_BZIP2_OPT
#ifdef AOCL_BZIP2_OPT
    #define AOCL_BZIP2_AVX_OPT
#endif

/* LZ4 */
#define AOCL_LZ4_OPT
#ifdef AOCL_LZ4_OPT
    #define AOCL_LZ4_AVX_OPT
#endif /* AOCL_LZ4_OPT */

/* LZ4HC */
#define AOCL_LZ4HC_OPT

/* LZMA */
#define AOCL_LZMA_OPT

/* SNAPPY */
#define AOCL_SNAPPY_OPT
#ifdef AOCL_SNAPPY_OPT
    #define AOCL_SNAPPY_AVX_OPT
    #ifdef AOCL_SNAPPY_AVX_OPT
        #define AOCL_SNAPPY_AVX2_OPT
    #endif /* AOCL_SNAPPY_AVX_OPT */
#endif /* AOCL_SNAPPY_OPT */

/* ZSTD */
#define AOCL_ZSTD_OPT

/* ZLIB */
#define AOCL_ZLIB_OPT
#ifdef AOCL_ZLIB_OPT
     #define AOCL_ZLIB_SSE2_OPT
     #ifdef AOCL_ZLIB_SSE2_OPT
          #define AOCL_ZLIB_AVX_OPT
          #ifdef AOCL_ZLIB_AVX_OPT
               #define AOCL_ZLIB_AVX2_OPT
               #ifdef AOCL_ZLIB_AVX2_OPT
                    #define AOCL_ZLIB_AVX512_OPT
               #endif /* AOCL_ZLIB_AVX2_OPT */
          #endif /* AOCL_ZLIB_AVX_OPT */
     #endif /* AOCL_ZLIB_SSE2_OPT */
#endif /* AOCL_ZLIB_OPT */
