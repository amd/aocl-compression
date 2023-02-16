/**
 * Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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

#include <immintrin.h>
#include <stdint.h>
#include "deflate.h"

#ifdef AOCL_DYNAMIC_DISPATCHER
/* Function pointer holding the optimized variant as per the detected CPU 
 * features */
static void (*slide_hash_fp)(deflate_state* s);
#endif

#ifdef AOCL_ZLIB_OPT
static inline void slide_hash_c_opt(deflate_state *s)
{
    const uInt wsize = s->w_size;
    const uInt hchnsz = s->hash_size;
    uInt i;
    Pos *hc = s->head;
    for (i = 0; i < hchnsz; i++) {
        Pos v = *hc;
        Pos t = (Pos)wsize;
        *hc++ = (Pos)(v >= t ? v-t: 0);
    }
#ifndef FASTEST
    Pos *pc = s->prev;
    for (uInt j = 0; j < wsize; j++) {
        Pos v = *pc;
        Pos t = (Pos)wsize;
        *pc++ = (Pos)(v >= t ? v-t: 0);
    }
#endif
}

#ifdef AOCL_ZLIB_AVX2_OPT
__attribute__((__target__("avx2")))
static inline void slide_hash_avx2(deflate_state *s)
{
    Pos *hc;
    uint16_t wsz = (uint16_t)s->w_size;
    uInt hchnsz = s->hash_size;
    const __m256i wsize256 = _mm256_set1_epi16((short)wsz);

    hc = s->head;
    for(;hchnsz > 0;hchnsz -= 16) {
        __m256i hres, hval;
        hval = _mm256_lddqu_si256((__m256i *)hc);
        hres = _mm256_subs_epu16(hval, wsize256);
        _mm256_storeu_si256((__m256i *)hc, hres);
        hc += 16;
    }
#ifndef FASTEST
    Pos *pc = s->prev;
    for(;wsz > 0;wsz -= 16) {
        __m256i pres, pval;
        pval = _mm256_lddqu_si256((__m256i *)pc);
        pres = _mm256_subs_epu16(pval, wsize256);
        _mm256_storeu_si256((__m256i *)pc, pres); 
        pc += 16;
    }
#endif
}
#endif /* AOCL_ZLIB_AVX2_OPT */

/* This function intercepts non optimized code path and orchestrate 
 * optimized code flow path */
void ZLIB_INTERNAL slide_hash_x86(deflate_state *s)
{
#ifdef AOCL_DYNAMIC_DISPATCHER
    slide_hash_fp(s);
#elif defined(AOCL_ZLIB_AVX2_OPT)
    slide_hash_avx2(s);
#else
    slide_hash_c_opt(s);
#endif
}
#endif /* AOCL_ZLIB_OPT */

#ifdef AOCL_DYNAMIC_DISPATCHER
void aocl_register_slide_hash_fmv(int optOff, int optLevel,
                                  void (*slide_hash_c_fp)(deflate_state* s))
{
    if (optOff)
    {
        slide_hash_fp = slide_hash_c_fp;
    }
    else
    {
        switch (optLevel)
        {
        case 0://C version
        case 1://SSE version
        case 2://AVX version
            slide_hash_fp = slide_hash_c_opt;
            break;
        case 3://AVX2 version
        default://AVX512 and other versions
            slide_hash_fp = slide_hash_avx2;
            break;
        }
    }
}
#endif
