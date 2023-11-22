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

#include "utils/utils.h"
#include <immintrin.h>
#include <stdint.h>
#include "zutil.h"

#ifdef AOCL_ZLIB_OPT
#include "aocl_zlib_setup.h"
/* Dynamic dispatcher setup function for native APIs.
 * All native APIs that call aocl optimized functions within their call stack,
 * must call AOCL_SETUP_NATIVE() at the start of the function. This sets up 
 * appropriate code paths to take based on user defined environment variables,
 * as well as cpu instruction set supported by the runtime machine. */
static void aocl_setup_native(void);
#define AOCL_SETUP_NATIVE() aocl_setup_native()

static int setup_ok_zlib_adler = 0; // flag to indicate status of dynamic dispatcher setup

/* Largest prime smaller than 65536 */
#define BASE 65521U
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */
#define NMAX 5552

#define ITER_SZ 64

#define DO1(buf,i)  {sum_A += (buf)[i]; sum_B += sum_A;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf)  DO4(buf,0); DO4(buf,4);

#define DO4_C(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8_C(buf,i)  DO4_C(buf,i); DO4_C(buf,i+4);
#define DO16(buf)   DO8_C(buf,0); DO8_C(buf,8);

#define MOD(a) a %= BASE
#define MOD28(a) a %= BASE
#define MOD63(a) a %= BASE

/* Function pointer holding the optimized variant as per the detected CPU
 * features */
static uint32_t (*adler32_x86_fp)(uint32_t adler, const Bytef* buf, z_size_t len) =
(uint32_t(*)(uint32_t, const Bytef*, z_size_t))adler32;

#ifdef AOCL_ZLIB_AVX_OPT
// This function separation prevents compiler from generating VZEROUPPER instruction
// because of transition from VEX to Non-VEX code resulting in performance drop
static inline uint32_t adler32_rem_len(uint32_t adler, const Bytef *buf, z_size_t len)
{
    uint32_t sum_A = adler & 0xffff;
    uint32_t sum_B = adler >> 16;
    if (len) {
        while (len >= 8)
        {
            len -= 8;
            DO8(buf);
            buf += 8;
        }

        while (len--)
        {
            sum_B += (sum_A += *buf++);
        }

        if (sum_A >= BASE)
            sum_A -= BASE;
        sum_B %= BASE;
    }

    return sum_A | (sum_B << 16);
}

__attribute__((__target__("avx"))) // uses SSSE3 intrinsics
static inline uint32_t adler32_x86_avx(uint32_t adler, const Bytef *buf, z_size_t len)
{
    AOCL_SIMD_UNIT_TEST(DEBUG, logCtx, "Enter");
    uint32_t sum_A = adler & 0xffff;
    uint32_t sum_B = adler >> 16;

    z_size_t  itr_cnt = len / ITER_SZ;
    len -= itr_cnt * ITER_SZ;

    while (itr_cnt)
    {
        __m128i vos, vcs, vbs, batch1, batch2, mad0, mad1;
        z_size_t n = NMAX / ITER_SZ; 
        if (n > itr_cnt)
            n = itr_cnt;
        itr_cnt -= n;

        const __m128i coeff1 = _mm_setr_epi8(64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49);
        const __m128i coeff2 = _mm_setr_epi8(48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33);
        const __m128i coeff3 = _mm_setr_epi8(32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17);
        const __m128i coeff4 = _mm_setr_epi8(16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
        const __m128i zero = _mm_setzero_si128();
        const __m128i octa_ones = _mm_set1_epi16(1);

        vos = _mm_set_epi32(0, 0, 0, sum_A * n);
        vcs = _mm_set_epi32(0, 0, 0, sum_B);
        vbs = zero;

        while(n--)
        {
        /*
            This loop works on 64 byte data in single iteration and stores partial results that helps in computing 
            two 16-bit checksums after exit

            sum_A = sum_A + B1 + B2 + .. + B64
            sum_B = 64*sum_A + 64*B1 + 63*B2 + .. +1*B64

            vbs : stores sum_A's partial computation of consecutive bytes in adjacent four 32-bit numbers
            vcs : stores sum_B's partial computation of consecutive bytes in adjacent four 32-bit numbers
            vos : accumulating vbs results per iteration for future calculation of sum_B
        */
            batch1 = _mm_lddqu_si128((__m128i*)(buf)); // batch1: B1 | B2 | ... B16
            batch2 = _mm_lddqu_si128((__m128i*)(buf + 16)); // batch1: B17 | B18 | ... B32

            // vos: old_vos + vbs
            vos = _mm_add_epi32(vos, vbs);
            // vbs: old_vbs + ( 0x00000000| 0x0000<B1+B2+B3+B4+B5+B6+B7+B8>| 0x00000000| 0x0000<B9+B10+B11+B12+B13+B14+B15+B16>)
            vbs = _mm_add_epi32(vbs, _mm_sad_epu8(batch1, zero));
            // mad0: 1*B16 + 2*B15| ...| ...| ...| ...| ...| ...| 15*B2 + 16*B1
            mad0 = _mm_maddubs_epi16(batch1, coeff1);
            // vcs: old_vsc + ( 1*B16 + 2*B15 + 3*B14 + 4*B13| ...| ...| 13*B4 + 14*B3 + 15*B2 + 16*B1 )
            vcs = _mm_add_epi32(vcs, _mm_madd_epi16(mad0, octa_ones));

            vbs = _mm_add_epi32(vbs, _mm_sad_epu8(batch2, zero));
            mad1 = _mm_maddubs_epi16(batch2, coeff2);
            vcs = _mm_add_epi32(vcs, _mm_madd_epi16(mad1, octa_ones));

            batch1 = _mm_lddqu_si128((__m128i*)(buf + 32));
            batch2 = _mm_lddqu_si128((__m128i*)(buf + 48));

            vbs = _mm_add_epi32(vbs, _mm_sad_epu8(batch1, zero));
            mad0 = _mm_maddubs_epi16(batch1, coeff3);
            vcs = _mm_add_epi32(vcs, _mm_madd_epi16(mad0, octa_ones));

            vbs = _mm_add_epi32(vbs, _mm_sad_epu8(batch2, zero));
            mad1 = _mm_maddubs_epi16(batch2, coeff4);
            vcs = _mm_add_epi32(vcs, _mm_madd_epi16(mad1, octa_ones));

            buf += ITER_SZ;
        }

        vcs = _mm_add_epi32(vcs, _mm_slli_epi32(vos, 6));
        // Shuffling and adding vbs data to compute 64*n byte sum in lower 32-bit number
        vbs = _mm_add_epi32(vbs, _mm_shuffle_epi32(vbs, _MM_SHUFFLE(2,3,0,1)));
        vbs = _mm_add_epi32(vbs, _mm_shuffle_epi32(vbs, _MM_SHUFFLE(1,0,3,2)));

        sum_A += _mm_cvtsi128_si32(vbs);
        // Shuffling and adding vcs data to accumulate sum_B in lower 32-bit number
        vcs = _mm_add_epi32(vcs, _mm_shuffle_epi32(vcs, _MM_SHUFFLE(2,3,0,1)));
        vcs = _mm_add_epi32(vcs, _mm_shuffle_epi32(vcs, _MM_SHUFFLE(1,0,3,2)));

        sum_B = _mm_cvtsi128_si32(vcs);

        sum_A %= BASE;
        sum_B %= BASE;
    }


    return adler32_rem_len(sum_A | (sum_B << 16), buf, len);
}
#endif /* AOCL_ZLIB_AVX_OPT */

#if defined(AOCL_ZLIB_AVX2_OPT) && defined(USE_AOCL_ADLER32_AVX2)
__attribute__((__target__("avx2")))
static inline uint32_t adler32_x86_avx2(uint32_t adler, const Bytef *buf, z_size_t len)
{
    AOCL_SIMD_UNIT_TEST(DEBUG, logCtx, "Enter");
    uint32_t sum_A = adler & 0xffff;
    uint32_t sum_B = adler >> 16;

    z_size_t  itr_cnt = len / ITER_SZ;
    len -= itr_cnt * ITER_SZ;

    while (itr_cnt)
    {
        __m256i vos, vcs, vbs, batch1, mad0;
        z_size_t n = NMAX / ITER_SZ; 
        if (n > itr_cnt)
            n = itr_cnt;
        itr_cnt -= n;

        const __m256i coeff1 = _mm256_setr_epi8(64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33);
        const __m256i coeff2 = _mm256_setr_epi8(32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
        const __m256i zero = _mm256_setzero_si256();
        const __m256i octa_ones = _mm256_set1_epi16(1);

        vos = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0,sum_A * n);
        vcs = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, sum_B);
        vbs = zero;

        while(n--)
        {
            batch1 = _mm256_lddqu_si256((__m256i*)(buf)); // batch1: B1 | B2 | ... B32

            // vos: old_vos + vbs
            vos = _mm256_add_epi32(vos, vbs);
            // vbs: old_vbs + ( 0x00000000| 0x0000<B1+B2+B3+B4+B5+B6+B7+B8>| ... 0x00000000| 0x0000<B25+B26+B27+B28+B29+B30+B31+B32>)
            vbs = _mm256_add_epi32(vbs, _mm256_sad_epu8(batch1, zero));
            // mad0: 1*B32 + 2*B31| ...| ...| ...| ...| ... 31*B2 + 32*B1
            mad0 = _mm256_maddubs_epi16(batch1, coeff1);
            // vcs: old_vsc + ( 1*B32 + 2*B31 + 3*B30 + 4*B29| ...| ... 29*B4 + 30*B3 + 31*B2 + 32*B1 )
            vcs = _mm256_add_epi32(vcs, _mm256_madd_epi16(mad0, octa_ones));

            batch1 = _mm256_lddqu_si256((__m256i*)(buf + 32));

            vbs = _mm256_add_epi32(vbs, _mm256_sad_epu8(batch1, zero));
            mad0 = _mm256_maddubs_epi16(batch1, coeff2);
            vcs = _mm256_add_epi32(vcs, _mm256_madd_epi16(mad0, octa_ones));

            buf += ITER_SZ;
        }

        vcs = _mm256_add_epi32(vcs, _mm256_slli_epi32(vos, 6));

        vbs = _mm256_add_epi32(vbs, _mm256_shuffle_epi32(vbs, _MM_SHUFFLE(2,3,0,1)));

        vbs = _mm256_add_epi32(vbs, _mm256_shuffle_epi32(vbs, _MM_SHUFFLE(1,0,3,2)));

        //sum_A += _mm256_cvtsi256_si32(vbs);
        sum_A += _mm256_extract_epi32(vbs, 0);

        vcs = _mm256_add_epi32(vcs, _mm256_shuffle_epi32(vcs, _MM_SHUFFLE(2,3,0,1)));
        vcs = _mm256_add_epi32(vcs, _mm256_shuffle_epi32(vcs, _MM_SHUFFLE(1,0,3,2)));

        //sum_B = _mm256_cvtsi256_si32(vcs);
        sum_B = _mm256_extract_epi32(vcs, 0);

        sum_A %= BASE;
        sum_B %= BASE;
    }
    return adler32_rem_len(sum_A | (sum_B << 16), buf, len);
}
#endif /* AOCL_ZLIB_AVX2_OPT && USE_AOCL_ADLER32_AVX2 */

uint32_t ZLIB_INTERNAL adler32_x86_internal(uint32_t sum_A, const Bytef* buf, z_size_t len)
{
    unsigned long sum_B;
#if !defined(AOCL_ZLIB_AVX_OPT)
    unsigned n;
#endif

    if (buf && len >= 32)
    {
        return adler32_x86_fp(sum_A, buf, len);
    }

    /* split Adler-32 into component sums */
    sum_B = (sum_A >> 16) & 0xffff;
    sum_A &= 0xffff;

    /* in case user likes doing a byte at a time, keep it fast */
    if (len == 1)
    {
        sum_A += buf[0];
        if (sum_A >= BASE)
            sum_A -= BASE;
        sum_B += sum_A;
        if (sum_B >= BASE)
            sum_B -= BASE;
        return sum_A | (sum_B << 16);
    }

    /* initial sum_A-32 value (deferred check for len == 1 speed) */
    if (buf == Z_NULL)
        return 1L;

    /* in case short lengths are provided, keep it somewhat fast */
    if (len < 16)
    {
        while (len--)
        {
            sum_A += *buf++;
            sum_B += sum_A;
        }
        if (sum_A >= BASE)
            sum_A -= BASE;
        MOD28(sum_B);            /* only added so many BASE's */
        return sum_A | (sum_B << 16);
    }

#if !defined(AOCL_ZLIB_AVX_OPT)
    /* do length NMAX blocks -- requires just one modulo operation */
    while (len >= NMAX)
    {
        len -= NMAX;
        n = NMAX / 16;          /* NMAX is divisible by 16 */
        do
        {
            DO16(buf);          /* 16 sums unrolled */
            buf += 16;
        } while (--n);
        MOD(sum_A);
        MOD(sum_B);
    }
#endif

    /* do remaining bytes (less than NMAX, still just one modulo) */
    if (len)                   /* avoid modulos if none remaining */
    {
        while (len >= 16)
        {
            len -= 16;
            DO16(buf);
            buf += 16;
        }
        while (len--)
        {
            sum_A += *buf++;
            sum_B += sum_A;
        }
        MOD(sum_A);
        MOD(sum_B);
    }

    /* return recombined sums */
    return sum_A | (sum_B << 16);
}

static inline void aocl_setup_adler32_fmv(int optOff, int optLevel)
{
    if (UNLIKELY(optOff == 1))
    {
        adler32_x86_fp = (uint32_t(*)(uint32_t, const Bytef*, z_size_t))adler32;
    }
    else
    {
        switch (optLevel)
        {
        case 0://C version
        case 1://SSE version
            adler32_x86_fp = (uint32_t(*)(uint32_t, const Bytef*, z_size_t))adler32;
            break;
        case 2://AVX version
#ifdef AOCL_ZLIB_AVX_OPT
            adler32_x86_fp = adler32_x86_avx;
#else
            adler32_x86_fp = (uint32_t(*)(uint32_t, const Bytef*, z_size_t))adler32;
#endif
            break;
        case -1: // undecided. use defaults based on compiler flags
        case 3://AVX2 version
        default://AVX512 and other versions
#if defined(AOCL_ZLIB_AVX2_OPT) && defined(USE_AOCL_ADLER32_AVX2)
            adler32_x86_fp = adler32_x86_avx2;
#elif defined(AOCL_ZLIB_AVX_OPT)
            adler32_x86_fp = adler32_x86_avx;
#else
            adler32_x86_fp = (uint32_t(*)(uint32_t, const Bytef*, z_size_t))adler32;
#endif
            break;
        }
    }
}

void ZLIB_INTERNAL aocl_setup_adler32(int optOff, int optLevel){
    AOCL_ENTER_CRITICAL(setup_zlib_adler)
    if (!setup_ok_zlib_adler) {
        optOff = optOff ? 1 : get_disable_opt_flags(0);
        aocl_setup_adler32_fmv(optOff, optLevel);
        setup_ok_zlib_adler = 1;
    }
    AOCL_EXIT_CRITICAL(setup_zlib_adler)
}

static void aocl_setup_native(void) {
    AOCL_ENTER_CRITICAL(setup_zlib_adler)
    if (!setup_ok_zlib_adler) {
        int optLevel = get_cpu_opt_flags(0);
        int optOff = get_disable_opt_flags(0);
        aocl_setup_adler32_fmv(optOff, optLevel);
        setup_ok_zlib_adler = 1;
    }
    AOCL_EXIT_CRITICAL(setup_zlib_adler)
}

void ZLIB_INTERNAL aocl_destroy_adler32(void) {
    AOCL_ENTER_CRITICAL(setup_zlib_adler)
    setup_ok_zlib_adler = 0;
    AOCL_EXIT_CRITICAL(setup_zlib_adler)
}

#endif /* AOCL_ZLIB_OPT */

/* This function intercepts non optimized code path and orchestrate
 * optimized code flow path */
uint32_t ZEXPORT adler32_x86(uint32_t sum_A, const Bytef* buf, z_size_t len)
{
#ifdef AOCL_ZLIB_OPT
    AOCL_SETUP_NATIVE();
    return adler32_x86_internal(sum_A, buf, len);
#else
    return adler32(sum_A, buf, len);
#endif /* AOCL_ZLIB_OPT */
}
