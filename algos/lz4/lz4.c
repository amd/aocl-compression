/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011-present, Yann Collet.
   Copyright (C) 2023, Advanced Micro Devices. All rights reserved.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
    - LZ4 homepage : http://www.lz4.org
    - LZ4 source repository : https://github.com/lz4/lz4
*/

/*-************************************
*  Tuning parameters
**************************************/
/*
 * LZ4_HEAPMODE :
 * Select how default compression functions will allocate memory for their hash table,
 * in memory stack (0:default, fastest), or in memory heap (1:requires malloc()).
 */
#ifndef LZ4_HEAPMODE
#  define LZ4_HEAPMODE 0
#endif

/*
 * LZ4_ACCELERATION_DEFAULT :
 * Select "acceleration" for LZ4_compress_fast() when parameter value <= 0
 */
#define LZ4_ACCELERATION_DEFAULT 1
/*
 * LZ4_ACCELERATION_MAX :
 * Any "acceleration" value higher than this threshold
 * get treated as LZ4_ACCELERATION_MAX instead (fix #876)
 */
#define LZ4_ACCELERATION_MAX 65537


/*-************************************
*  CPU Feature Detection
**************************************/
/* LZ4_FORCE_MEMORY_ACCESS
 * By default, access to unaligned memory is controlled by `memcpy()`, which is safe and portable.
 * Unfortunately, on some target/compiler combinations, the generated assembly is sub-optimal.
 * The below switch allow to select different access method for improved performance.
 * Method 0 (default) : use `memcpy()`. Safe and portable.
 * Method 1 : `__packed` statement. It depends on compiler extension (ie, not portable).
 *            This method is safe if your compiler supports it, and *generally* as fast or faster than `memcpy`.
 * Method 2 : direct access. This method is portable but violate C standard.
 *            It can generate buggy code on targets which assembly generation depends on alignment.
 *            But in some circumstances, it's the only known way to get the most performance (ie GCC + ARMv6)
 * See https://fastcompression.blogspot.fr/2015/08/accessing-unaligned-memory.html for details.
 * Prefer these methods in priority order (0 > 1 > 2)
 */
#ifndef LZ4_FORCE_MEMORY_ACCESS   /* can be defined externally */
#  if defined(__GNUC__) && \
  ( defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) \
  || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__) )
#    define LZ4_FORCE_MEMORY_ACCESS 2
#  elif (defined(__INTEL_COMPILER) && !defined(_WIN32)) || defined(__GNUC__)
#    define LZ4_FORCE_MEMORY_ACCESS 1
#  endif
#endif

/*
 * LZ4_FORCE_SW_BITCOUNT
 * Define this parameter if your target system or compiler does not support hardware bit count
 */
#if defined(_MSC_VER) && defined(_WIN32_WCE)   /* Visual Studio for WinCE doesn't support Hardware bit count */
#  undef  LZ4_FORCE_SW_BITCOUNT  /* avoid double def */
#  define LZ4_FORCE_SW_BITCOUNT
#endif



/*-************************************
*  Dependency
**************************************/
/*
 * LZ4_SRC_INCLUDED:
 * Amalgamation flag, whether lz4.c is included
 */
#ifndef LZ4_SRC_INCLUDED
#  define LZ4_SRC_INCLUDED 1
#endif

#ifndef LZ4_STATIC_LINKING_ONLY
#define LZ4_STATIC_LINKING_ONLY
#endif

#ifndef LZ4_DISABLE_DEPRECATE_WARNINGS
#define LZ4_DISABLE_DEPRECATE_WARNINGS /* due to LZ4_decompress_safe_withPrefix64k */
#endif

#define LZ4_STATIC_LINKING_ONLY  /* LZ4_DISTANCE_MAX */
#include "lz4.h"
#include "utils/utils.h"
/* see also "memory routines" below */


/*-************************************
*  Compiler Options
**************************************/
#if defined(_MSC_VER) && (_MSC_VER >= 1400)  /* Visual Studio 2005+ */
#  include <intrin.h>               /* only present in VS2005+ */
#  pragma warning(disable : 4127)   /* disable: C4127: conditional expression is constant */
#endif  /* _MSC_VER */

#ifndef LZ4_FORCE_INLINE
#  ifdef _MSC_VER    /* Visual Studio */
#    define LZ4_FORCE_INLINE static __forceinline
#  else
#    if defined (__cplusplus) || defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* C99 */
#      ifdef __GNUC__
#        define LZ4_FORCE_INLINE static inline __attribute__((always_inline))
#      else
#        define LZ4_FORCE_INLINE static inline
#      endif
#    else
#      define LZ4_FORCE_INLINE static
#    endif /* __STDC_VERSION__ */
#  endif  /* _MSC_VER */
#endif /* LZ4_FORCE_INLINE */

/* LZ4_FORCE_O2 and LZ4_FORCE_INLINE
 * gcc on ppc64le generates an unrolled SIMDized loop for LZ4_wildCopy8,
 * together with a simple 8-byte copy loop as a fall-back path.
 * However, this optimization hurts the decompression speed by >30%,
 * because the execution does not go to the optimized loop
 * for typical compressible data, and all of the preamble checks
 * before going to the fall-back path become useless overhead.
 * This optimization happens only with the -O3 flag, and -O2 generates
 * a simple 8-byte copy loop.
 * With gcc on ppc64le, all of the LZ4_decompress_* and LZ4_wildCopy8
 * functions are annotated with __attribute__((optimize("O2"))),
 * and also LZ4_wildCopy8 is forcibly inlined, so that the O2 attribute
 * of LZ4_wildCopy8 does not affect the compression speed.
 */
#if defined(__PPC64__) && defined(__LITTLE_ENDIAN__) && defined(__GNUC__) && !defined(__clang__)
#  define LZ4_FORCE_O2  __attribute__((optimize("O2")))
#  undef LZ4_FORCE_INLINE
#  define LZ4_FORCE_INLINE  static __inline __attribute__((optimize("O2"),always_inline))
#else
#  define LZ4_FORCE_O2
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 800)) || defined(__clang__)
#  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
#else
#  define expect(expr,value)    (expr)
#endif

#ifndef likely
#define likely(expr)     expect((expr) != 0, 1)
#endif
#ifndef unlikely
#define unlikely(expr)   expect((expr) != 0, 0)
#endif

/* Should the alignment test prove unreliable, for some reason,
 * it can be disabled by setting LZ4_ALIGN_TEST to 0 */
#ifndef LZ4_ALIGN_TEST  /* can be externally provided */
# define LZ4_ALIGN_TEST 1
#endif

#include "utils/utils.h"

#ifdef AOCL_LZ4_OPT
/* Dynamic dispatcher setup function for native APIs.
 * All native APIs that call aocl optimized functions within their call stack,
 * must call AOCL_SETUP_NATIVE() at the start of the function. This sets up 
 * appropriate code paths to take based on user defined environment variables,
 * as well as cpu instruction set supported by the runtime machine. */
static void aocl_setup_native(void);
#define AOCL_SETUP_NATIVE() aocl_setup_native()
#else
#define AOCL_SETUP_NATIVE()
#endif

static int setup_ok_lz4 = 0; // flag to indicate status of dynamic dispatcher setup

/*-************************************
*  Memory routines
**************************************/
#ifdef LZ4_USER_MEMORY_FUNCTIONS
/* memory management functions can be customized by user project.
 * Below functions must exist somewhere in the Project
 * and be available at link time */
void* LZ4_malloc(size_t s);
void* LZ4_calloc(size_t n, size_t s);
void  LZ4_free(void* p);
# define ALLOC(s)          LZ4_malloc(s)
# define ALLOC_AND_ZERO(s) LZ4_calloc(1,s)
# define FREEMEM(p)        LZ4_free(p)
#else
# include <stdlib.h>   /* malloc, calloc, free */
# define ALLOC(s)          malloc(s)
# define ALLOC_AND_ZERO(s) calloc(1,s)
# define FREEMEM(p)        free(p)
#endif

#include <string.h>   /* memset, memcpy */
#define MEM_INIT(p,v,s)   memset((p),(v),(s))


/*-************************************
*  Common Constants
**************************************/
#define MINMATCH 4

#define WILDCOPYLENGTH 8
#define LASTLITERALS   5   /* see ../doc/lz4_Block_format.md#parsing-restrictions */
#define MFLIMIT       12   /* see ../doc/lz4_Block_format.md#parsing-restrictions */
#define MATCH_SAFEGUARD_DISTANCE  ((2*WILDCOPYLENGTH) - MINMATCH)   /* ensure it's possible to write 2 x wildcopyLength without overflowing output buffer */
#define FASTLOOP_SAFE_DISTANCE 64
static const int LZ4_minLength = (MFLIMIT+1);

#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

#define LZ4_DISTANCE_ABSOLUTE_MAX 65535
#if (LZ4_DISTANCE_MAX > LZ4_DISTANCE_ABSOLUTE_MAX)   /* max supported by LZ4 format */
#  error "LZ4_DISTANCE_MAX is too big : must be <= 65535"
#endif

#define ML_BITS  4
#define ML_MASK  ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)


/*-************************************
*  Error detection
**************************************/
/* 
    In Release mode assert statements are disabled by CMake,
    by passing NDEBUG flag, which disables assert statement.
    In Debug mode assert statements are automatically enabled.
*/
#include <assert.h>

#define LZ4_STATIC_ASSERT(c)   { enum { LZ4_static_assert = 1/(int)(!!(c)) }; }   /* use after variable declarations */

#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=2)
#  include <stdio.h>
   static int g_debuglog_enable = 1;
#  define DEBUGLOG(l, ...) {                          \
        if ((g_debuglog_enable) && (l<=LZ4_DEBUG)) {  \
            fprintf(stderr, __FILE__ ": ");           \
            fprintf(stderr, __VA_ARGS__);             \
            fprintf(stderr, " \n");                   \
    }   }
#else
#  define DEBUGLOG(l, ...) {}    /* disabled */
#endif

static int LZ4_isAligned(const void* ptr, size_t alignment)
{
    return ((size_t)ptr & (alignment -1)) == 0;
}


/*-************************************
*  Types
**************************************/
#include <limits.h>
#if defined(__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */)
# include <stdint.h>
  typedef  uint8_t BYTE;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef  int32_t S32;
  typedef uint64_t U64;
  typedef uintptr_t uptrval;
#else
# if UINT_MAX != 4294967295UL
#   error "LZ4 code (when not C++ or C99) assumes that sizeof(int) == 4"
# endif
  typedef unsigned char       BYTE;
  typedef unsigned short      U16;
  typedef unsigned int        U32;
  typedef   signed int        S32;
  typedef unsigned long long  U64;
  typedef size_t              uptrval;   /* generally true, except OpenVMS-64 */
#endif

#if defined(__x86_64__)
  typedef U64    reg_t;   /* 64-bits in x32 mode */
#else
  typedef size_t reg_t;   /* 32-bits in x32 mode */
#endif

typedef enum {
    notLimited = 0,
    limitedOutput = 1,
    fillOutput = 2
} limitedOutput_directive;

#ifdef AOCL_ENABLE_THREADS
#include "threads/threads.h"
#endif

/*-************************************
*  Reading and writing into memory
**************************************/

/**
 * LZ4 relies on memcpy with a constant size being inlined. In freestanding
 * environments, the compiler can't assume the implementation of memcpy() is
 * standard compliant, so it can't apply its specialized memcpy() inlining
 * logic. When possible, use __builtin_memcpy() to tell the compiler to analyze
 * memcpy() as if it were standard compliant, so it can inline it in freestanding
 * environments. This is needed when decompressing the Linux Kernel, for example.
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define LZ4_memcpy(dst, src, size) __builtin_memcpy(dst, src, size)
#else
#define LZ4_memcpy(dst, src, size) memcpy(dst, src, size)
#endif

static unsigned LZ4_isLittleEndian(void)
{
    const union { U32 u; BYTE c[4]; } one = { 1 };   /* don't use static : performance detrimental */
    return one.c[0];
}


#if defined(LZ4_FORCE_MEMORY_ACCESS) && (LZ4_FORCE_MEMORY_ACCESS==2)
/* lie to the compiler about data alignment; use with caution */

static U16 LZ4_read16(const void* memPtr) { return *(const U16*) memPtr; }
static U32 LZ4_read32(const void* memPtr) { return *(const U32*) memPtr; }
static reg_t LZ4_read_ARCH(const void* memPtr) { return *(const reg_t*) memPtr; }

static void LZ4_write16(void* memPtr, U16 value) { *(U16*)memPtr = value; }
static void LZ4_write32(void* memPtr, U32 value) { *(U32*)memPtr = value; }

#elif defined(LZ4_FORCE_MEMORY_ACCESS) && (LZ4_FORCE_MEMORY_ACCESS==1)

/* __pack instructions are safer, but compiler specific, hence potentially problematic for some compilers */
/* currently only defined for gcc and icc */
typedef union { U16 u16; U32 u32; reg_t uArch; } __attribute__((packed)) unalign;

static U16 LZ4_read16(const void* ptr) { return ((const unalign*)ptr)->u16; }
static U32 LZ4_read32(const void* ptr) { return ((const unalign*)ptr)->u32; }
static reg_t LZ4_read_ARCH(const void* ptr) { return ((const unalign*)ptr)->uArch; }

static void LZ4_write16(void* memPtr, U16 value) { ((unalign*)memPtr)->u16 = value; }
static void LZ4_write32(void* memPtr, U32 value) { ((unalign*)memPtr)->u32 = value; }

#else  /* safe and portable access using memcpy() */

static U16 LZ4_read16(const void* memPtr)
{
    U16 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static U32 LZ4_read32(const void* memPtr)
{
    U32 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static reg_t LZ4_read_ARCH(const void* memPtr)
{
    reg_t val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static void LZ4_write16(void* memPtr, U16 value)
{
    LZ4_memcpy(memPtr, &value, sizeof(value));
}

static void LZ4_write32(void* memPtr, U32 value)
{
    LZ4_memcpy(memPtr, &value, sizeof(value));
}

#endif /* LZ4_FORCE_MEMORY_ACCESS */


static U16 LZ4_readLE16(const void* memPtr)
{
    if (LZ4_isLittleEndian()) {
        return LZ4_read16(memPtr);
    } else {
        const BYTE* p = (const BYTE*)memPtr;
        return (U16)((U16)p[0] + (p[1]<<8));
    }
}

static void LZ4_writeLE16(void* memPtr, U16 value)
{
    if (LZ4_isLittleEndian()) {
        LZ4_write16(memPtr, value);
    } else {
        BYTE* p = (BYTE*)memPtr;
        p[0] = (BYTE) value;
        p[1] = (BYTE)(value>>8);
    }
}

/* customized variant of memcpy, which can overwrite up to 8 bytes beyond dstEnd */
LZ4_FORCE_INLINE
void LZ4_wildCopy8(void* dstPtr, const void* srcPtr, void* dstEnd)
{
    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

    do { LZ4_memcpy(d,s,8); d+=8; s+=8; } while (d<e);
}

static const unsigned inc32table[8] = {0, 1, 2,  1,  0,  4, 4, 4};
static const int      dec64table[8] = {0, 0, 0, -1, -4,  1, 2, 3};


#ifndef LZ4_FAST_DEC_LOOP
#  if defined __i386__ || defined _M_IX86 || defined __x86_64__ || defined _M_X64
#    define LZ4_FAST_DEC_LOOP 1
#  elif defined(__aarch64__) && !defined(__clang__)
     /* On aarch64, we disable this optimization for clang because on certain
      * mobile chipsets, performance is reduced with clang. For information
      * refer to https://github.com/lz4/lz4/pull/707 */
#    define LZ4_FAST_DEC_LOOP 1
#  else
#    define LZ4_FAST_DEC_LOOP 0
#  endif
#endif

#if LZ4_FAST_DEC_LOOP

LZ4_FORCE_INLINE void
LZ4_memcpy_using_offset_base(BYTE* dstPtr, const BYTE* srcPtr, BYTE* dstEnd, const size_t offset)
{
    assert(srcPtr + offset == dstPtr);
    if (offset < 8) {
        LZ4_write32(dstPtr, 0);   /* silence an msan warning when offset==0 */
        dstPtr[0] = srcPtr[0];
        dstPtr[1] = srcPtr[1];
        dstPtr[2] = srcPtr[2];
        dstPtr[3] = srcPtr[3];
        srcPtr += inc32table[offset];
        LZ4_memcpy(dstPtr+4, srcPtr, 4);
        srcPtr -= dec64table[offset];
        dstPtr += 8;
    } else {
        LZ4_memcpy(dstPtr, srcPtr, 8);
        dstPtr += 8;
        srcPtr += 8;
    }

    LZ4_wildCopy8(dstPtr, srcPtr, dstEnd);
}

/* customized variant of memcpy, which can overwrite up to 32 bytes beyond dstEnd
 * this version copies two times 16 bytes (instead of one time 32 bytes)
 * because it must be compatible with offsets >= 16. */
LZ4_FORCE_INLINE void
LZ4_wildCopy32(void* dstPtr, const void* srcPtr, void* dstEnd)
{
    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

    do { LZ4_memcpy(d,s,16); LZ4_memcpy(d+16,s+16,16); d+=32; s+=32; } while (d<e);
}

#ifdef AOCL_LZ4_AVX_OPT
#include<immintrin.h>
__attribute__((__target__("avx")))
static inline void
AOCL_memcpy64(BYTE* dst, const BYTE* src){
    AOCL_SIMD_UNIT_TEST(DEBUG, logCtx, "Enter");
    __m256i src_reg = _mm256_lddqu_si256((__m256i*)src);
    _mm256_storeu_si256((__m256i*)dst, src_reg);

    dst+=32; src+=32;

    src_reg = _mm256_lddqu_si256((__m256i*)src);
    _mm256_storeu_si256((__m256i*)dst, src_reg);
}

/* AVX implementation of wildCopy, which can overwrite up to 64 bytes beyond dstEnd
 * this version copies two times 32 bytes in an iteration.
 */
static inline void
AOCL_LZ4_wildCopy64_AVX(void*dstPtr, const void* srcPtr, void*dstEnd){

    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

    do{
        AOCL_memcpy64(d,s);
        s+=64;
        d+=64;
    }while(d<e);

}
#endif /* AOCL_LZ4_AVX_OPT */

/* LZ4_memcpy_using_offset()  presumes :
 * - dstEnd >= dstPtr + MINMATCH
 * - there is at least 8 bytes available to write after dstEnd */
LZ4_FORCE_INLINE void
LZ4_memcpy_using_offset(BYTE* dstPtr, const BYTE* srcPtr, BYTE* dstEnd, const size_t offset)
{
    BYTE v[8];

    assert(dstEnd >= dstPtr + MINMATCH);

    switch(offset) {
    case 1:
        MEM_INIT(v, *srcPtr, 8);
        break;
    case 2:
        LZ4_memcpy(v, srcPtr, 2);
        LZ4_memcpy(&v[2], srcPtr, 2);
        LZ4_memcpy(&v[4], v, 4);
        break;
    case 4:
        LZ4_memcpy(v, srcPtr, 4);
        LZ4_memcpy(&v[4], srcPtr, 4);
        break;
    default:
        LZ4_memcpy_using_offset_base(dstPtr, srcPtr, dstEnd, offset);
        return;
    }

    LZ4_memcpy(dstPtr, v, 8);
    dstPtr += 8;
    while (dstPtr < dstEnd) {
        LZ4_memcpy(dstPtr, v, 8);
        dstPtr += 8;
    }
}
#endif


/*-************************************
*  Common functions
**************************************/
static unsigned LZ4_NbCommonBytes (reg_t val)
{
    assert(val != 0);
    if (LZ4_isLittleEndian()) {
        if (sizeof(val) == 8) {
#       if defined(_MSC_VER) && (_MSC_VER >= 1800) && defined(_M_AMD64) && !defined(LZ4_FORCE_SW_BITCOUNT)
            /* x64 CPUS without BMI support interpret `TZCNT` as `REP BSF` */
            return (unsigned)_tzcnt_u64(val) >> 3;
#       elif defined(_MSC_VER) && defined(_WIN64) && !defined(LZ4_FORCE_SW_BITCOUNT)
            unsigned long r = 0;
            _BitScanForward64(&r, (U64)val);
            return (unsigned)r >> 3;
#       elif (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                                        !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_ctzll((U64)val) >> 3;
#       else
            const U64 m = 0x0101010101010101ULL;
            val ^= val - 1;
            return (unsigned)(((U64)((val & (m - 1)) * m)) >> 56);
#       endif
        } else /* 32 bits */ {
#       if defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(LZ4_FORCE_SW_BITCOUNT)
            unsigned long r;
            _BitScanForward(&r, (U32)val);
            return (unsigned)r >> 3;
#       elif (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                        !defined(__TINYC__) && !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_ctz((U32)val) >> 3;
#       else
            const U32 m = 0x01010101;
            return (unsigned)((((val - 1) ^ val) & (m - 1)) * m) >> 24;
#       endif
        }
    } else   /* Big Endian CPU */ {
        if (sizeof(val)==8) {
#       if (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                        !defined(__TINYC__) && !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_clzll((U64)val) >> 3;
#       else
#if 1
            /* this method is probably faster,
             * but adds a 128 bytes lookup table */
            static const unsigned char ctz7_tab[128] = {
                7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
            };
            U64 const mask = 0x0101010101010101ULL;
            U64 const t = (((val >> 8) - mask) | val) & mask;
            return ctz7_tab[(t * 0x0080402010080402ULL) >> 57];
#else
            /* this method doesn't consume memory space like the previous one,
             * but it contains several branches,
             * that may end up slowing execution */
            static const U32 by32 = sizeof(val)*4;  /* 32 on 64 bits (goal), 16 on 32 bits.
            Just to avoid some static analyzer complaining about shift by 32 on 32-bits target.
            Note that this code path is never triggered in 32-bits mode. */
            unsigned r;
            if (!(val>>by32)) { r=4; } else { r=0; val>>=by32; }
            if (!(val>>16)) { r+=2; val>>=8; } else { val>>=24; }
            r += (!val);
            return r;
#endif
#       endif
        } else /* 32 bits */ {
#       if (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                                        !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_clz((U32)val) >> 3;
#       else
            val >>= 8;
            val = ((((val + 0x00FFFF00) | 0x00FFFFFF) + val) |
              (val + 0x00FF0000)) >> 24;
            return (unsigned)val ^ 3;
#       endif
        }
    }
}


#define STEPSIZE sizeof(reg_t)
LZ4_FORCE_INLINE
unsigned LZ4_count(const BYTE* pIn, const BYTE* pMatch, const BYTE* pInLimit)
{
    const BYTE* const pStart = pIn;

    if (likely(pIn < pInLimit-(STEPSIZE-1))) {
        reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
        if (!diff) {
            pIn+=STEPSIZE; pMatch+=STEPSIZE;
        } else {
            return LZ4_NbCommonBytes(diff);
    }   }

    while (likely(pIn < pInLimit-(STEPSIZE-1))) {
        reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
        if (!diff) { pIn+=STEPSIZE; pMatch+=STEPSIZE; continue; }
        pIn += LZ4_NbCommonBytes(diff);
        return (unsigned)(pIn - pStart);
    }

    if ((STEPSIZE==8) && (pIn<(pInLimit-3)) && (LZ4_read32(pMatch) == LZ4_read32(pIn))) { pIn+=4; pMatch+=4; }
    if ((pIn<(pInLimit-1)) && (LZ4_read16(pMatch) == LZ4_read16(pIn))) { pIn+=2; pMatch+=2; }
    if ((pIn<pInLimit) && (*pMatch == *pIn)) pIn++;
    return (unsigned)(pIn - pStart);
}
 

#ifndef LZ4_COMMONDEFS_ONLY
/* Function pointer holding the optimized function variant as per the detected
 * CPU features */
/* Function pointer definition placed inside #ifndef LZ4_COMMONDEFS_ONLY to avoid
 warnings related to unused variable. */
static int (*LZ4_compress_fast_extState_fp)(void* state, const char* source,
    char* dest, int inputSize,
    int maxOutputSize, int acceleration) = LZ4_compress_fast_extState;

/*-************************************
*  Local Constants
**************************************/
static const int LZ4_64Klimit = ((64 KB) + (MFLIMIT-1));
static const U32 LZ4_skipTrigger = 6;  /* Increase this value ==> compression run slower on incompressible data */


/*-************************************
*  Local Structures and types
**************************************/
typedef enum { clearedTable = 0, byPtr, byU32, byU16 } tableType_t;

/**
 * This enum distinguishes several different modes of accessing previous
 * content in the stream.
 *
 * - noDict        : There is no preceding content.
 * - withPrefix64k : Table entries up to ctx->dictSize before the current blob
 *                   blob being compressed are valid and refer to the preceding
 *                   content (of length ctx->dictSize), which is available
 *                   contiguously preceding in memory the content currently
 *                   being compressed.
 * - usingExtDict  : Like withPrefix64k, but the preceding content is somewhere
 *                   else in memory, starting at ctx->dictionary with length
 *                   ctx->dictSize.
 * - usingDictCtx  : Like usingExtDict, but everything concerning the preceding
 *                   content is in a separate context, pointed to by
 *                   ctx->dictCtx. ctx->dictionary, ctx->dictSize, and table
 *                   entries in the current context that refer to positions
 *                   preceding the beginning of the current compression are
 *                   ignored. Instead, ctx->dictCtx->dictionary and ctx->dictCtx
 *                   ->dictSize describe the location and size of the preceding
 *                   content, and matches are found by looking in the ctx
 *                   ->dictCtx->hashTable.
 */
typedef enum { noDict = 0, withPrefix64k, usingExtDict, usingDictCtx } dict_directive;
typedef enum { noDictIssue = 0, dictSmall } dictIssue_directive;


/*-************************************
*  Local Utils
**************************************/
int LZ4_versionNumber (void) { return LZ4_VERSION_NUMBER; }
const char* LZ4_versionString(void) { return LZ4_VERSION_STRING; }
int LZ4_compressBound(int isize)  { return LZ4_COMPRESSBOUND(isize); }
int LZ4_sizeofState(void) { return LZ4_STREAMSIZE; }


/*-************************************
*  Internal Definitions used in Tests
**************************************/
#if defined (__cplusplus)
extern "C" {
#endif

int LZ4_compress_forceExtDict (LZ4_stream_t* LZ4_dict, const char* source, char* dest, int srcSize);

int LZ4_decompress_safe_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int maxOutputSize,
                                     const void* dictStart, size_t dictSize);

#if defined (__cplusplus)
}
#endif

/*-******************************
*  Compression functions
********************************/
LZ4_FORCE_INLINE U32 LZ4_hash4(U32 sequence, tableType_t const tableType)
{
    if (tableType == byU16)
        return ((sequence * 2654435761U) >> ((MINMATCH*8)-(LZ4_HASHLOG+1)));
    else
        return ((sequence * 2654435761U) >> ((MINMATCH*8)-LZ4_HASHLOG));
}

LZ4_FORCE_INLINE U32 LZ4_hash5(U64 sequence, tableType_t const tableType)
{
    const U32 hashLog = (tableType == byU16) ? LZ4_HASHLOG+1 : LZ4_HASHLOG;
    if (LZ4_isLittleEndian()) {
        const U64 prime5bytes = 889523592379ULL;
        return (U32)(((sequence << 24) * prime5bytes) >> (64 - hashLog));
    } else {
        const U64 prime8bytes = 11400714785074694791ULL;
        return (U32)(((sequence >> 24) * prime8bytes) >> (64 - hashLog));
    }
}

#ifdef AOCL_LZ4_OPT
/*
 *   This function is a variant of LZ4_hash5 function, which uses a new prime number.
 *   This improves Compression speed, with minimal loss in compression ratio.
 */
LZ4_FORCE_INLINE U32 AOCL_LZ4_hash5(U64 sequence, tableType_t const tableType)
{
    const U32 hashLog = (tableType == byU16) ? LZ4_HASHLOG+1 : LZ4_HASHLOG;
    if (LZ4_isLittleEndian()) {
#ifdef AOCL_LZ4_NEW_PRIME_NUMBER
        const U64 prime5bytes = 136444968149183ULL;
#else
        const U64 prime5bytes = 889523592379ULL;
#endif /* AOCL_LZ4_NEW_PRIME_NUMBER */
#ifdef AOCL_LZ4_HASH_BITS_USED
        return (U32)(((sequence << (64 - AOCL_LZ4_HASH_BITS_USED)) * prime5bytes) >> (64 - hashLog));
#else
        return (U32)(((sequence << 24) * prime5bytes) >> (64 - hashLog));
#endif /* AOCL_LZ4_HASH_BITS_USED */
    } else {
        const U64 prime8bytes = 11400714785074694791ULL;
        return (U32)(((sequence >> 24) * prime8bytes) >> (64 - hashLog));
    }
}

#ifdef AOCL_UNIT_TEST
U32 Test_AOCL_LZ4_hash5(U64 sequence, int tableType)
{
    return AOCL_LZ4_hash5(sequence, tableType);
}
#endif /* AOCL_UNIT_TEST */
#endif /* AOCL_LZ4_OPT */

LZ4_FORCE_INLINE U32 LZ4_hashPosition(const void* const p, tableType_t const tableType)
{
    if ((sizeof(reg_t)==8) && (tableType != byU16)) return LZ4_hash5(LZ4_read_ARCH(p), tableType);
    return LZ4_hash4(LZ4_read32(p), tableType);
}

#ifdef AOCL_LZ4_OPT
LZ4_FORCE_INLINE U32 AOCL_LZ4_hashPosition(const void* const p, tableType_t const tableType)
{
    if ((sizeof(reg_t)==8) && (tableType != byU16)) return AOCL_LZ4_hash5(LZ4_read_ARCH(p), tableType);
    return LZ4_hash4(LZ4_read32(p), tableType);
}
#endif /* AOCL_LZ4_OPT */

LZ4_FORCE_INLINE void LZ4_clearHash(U32 h, void* tableBase, tableType_t const tableType)
{
    switch (tableType)
    {
    default: /* fallthrough */
    case clearedTable: { /* illegal! */ assert(0); return; }
    case byPtr: { const BYTE** hashTable = (const BYTE**)tableBase; hashTable[h] = NULL; return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = 0; return; }
    case byU16: { U16* hashTable = (U16*) tableBase; hashTable[h] = 0; return; }
    }
}

LZ4_FORCE_INLINE void LZ4_putIndexOnHash(U32 idx, U32 h, void* tableBase, tableType_t const tableType)
{
    switch (tableType)
    {
    default: /* fallthrough */
    case clearedTable: /* fallthrough */
    case byPtr: { /* illegal! */ assert(0); return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = idx; return; }
    case byU16: { U16* hashTable = (U16*) tableBase; assert(idx < 65536); hashTable[h] = (U16)idx; return; }
    }
}

LZ4_FORCE_INLINE void LZ4_putPositionOnHash(const BYTE* p, U32 h,
                                  void* tableBase, tableType_t const tableType,
                            const BYTE* srcBase)
{
    switch (tableType)
    {
    case clearedTable: { /* illegal! */ assert(0); return; }
    case byPtr: { const BYTE** hashTable = (const BYTE**)tableBase; hashTable[h] = p; return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = (U32)(p-srcBase); return; }
    case byU16: { U16* hashTable = (U16*) tableBase; hashTable[h] = (U16)(p-srcBase); return; }
    }
}

LZ4_FORCE_INLINE void LZ4_putPosition(const BYTE* p, void* tableBase, tableType_t tableType, const BYTE* srcBase)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    LZ4_putPositionOnHash(p, h, tableBase, tableType, srcBase);
}

#ifdef AOCL_LZ4_OPT
LZ4_FORCE_INLINE void AOCL_LZ4_putPosition(const BYTE* p, void* tableBase, tableType_t tableType, const BYTE* srcBase)
{
    U32 const h = AOCL_LZ4_hashPosition(p, tableType);
    LZ4_putPositionOnHash(p, h, tableBase, tableType, srcBase);
}
#endif /* AOCL_LZ4_OPT */

/* LZ4_getIndexOnHash() :
 * Index of match position registered in hash table.
 * hash position must be calculated by using base+index, or dictBase+index.
 * Assumption 1 : only valid if tableType == byU32 or byU16.
 * Assumption 2 : h is presumed valid (within limits of hash table)
 */
LZ4_FORCE_INLINE U32 LZ4_getIndexOnHash(U32 h, const void* tableBase, tableType_t tableType)
{
    LZ4_STATIC_ASSERT(LZ4_MEMORY_USAGE > 2);
    if (tableType == byU32) {
        const U32* const hashTable = (const U32*) tableBase;
        assert(h < (1U << (LZ4_MEMORY_USAGE-2)));
        return hashTable[h];
    }
    if (tableType == byU16) {
        const U16* const hashTable = (const U16*) tableBase;
        assert(h < (1U << (LZ4_MEMORY_USAGE-1)));
        return hashTable[h];
    }
    assert(0); return 0;  /* forbidden case */
}

static const BYTE* LZ4_getPositionOnHash(U32 h, const void* tableBase, tableType_t tableType, const BYTE* srcBase)
{
    if (tableType == byPtr) { const BYTE* const* hashTable = (const BYTE* const*) tableBase; return hashTable[h]; }
    if (tableType == byU32) { const U32* const hashTable = (const U32*) tableBase; return hashTable[h] + srcBase; }
    { const U16* const hashTable = (const U16*) tableBase; return hashTable[h] + srcBase; }   /* default, to ensure a return */
}

LZ4_FORCE_INLINE const BYTE*
LZ4_getPosition(const BYTE* p,
                const void* tableBase, tableType_t tableType,
                const BYTE* srcBase)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    return LZ4_getPositionOnHash(h, tableBase, tableType, srcBase);
}

#ifdef AOCL_LZ4_OPT
LZ4_FORCE_INLINE const BYTE*
AOCL_LZ4_getPosition(const BYTE* p,
                const void* tableBase, tableType_t tableType,
                const BYTE* srcBase)
{
    U32 const h = AOCL_LZ4_hashPosition(p, tableType);
    return LZ4_getPositionOnHash(h, tableBase, tableType, srcBase);
}
#endif /* AOCL_LZ4_OPT */

LZ4_FORCE_INLINE void
LZ4_prepareTable(LZ4_stream_t_internal* const cctx,
           const int inputSize,
           const tableType_t tableType) {
    /* If the table hasn't been used, it's guaranteed to be zeroed out, and is
     * therefore safe to use no matter what mode we're in. Otherwise, we figure
     * out if it's safe to leave as is or whether it needs to be reset.
     */
    if ((tableType_t)cctx->tableType != clearedTable) {
        assert(inputSize >= 0);
        if ((tableType_t)cctx->tableType != tableType
          || ((tableType == byU16) && cctx->currentOffset + (unsigned)inputSize >= 0xFFFFU)
          || ((tableType == byU32) && cctx->currentOffset > 1 GB)
          || tableType == byPtr
          || inputSize >= 4 KB)
        {
            DEBUGLOG(4, "LZ4_prepareTable: Resetting table in %p", cctx);
            MEM_INIT(cctx->hashTable, 0, LZ4_HASHTABLESIZE);
            cctx->currentOffset = 0;
            cctx->tableType = (U32)clearedTable;
        } else {
            DEBUGLOG(4, "LZ4_prepareTable: Re-use hash table (no reset)");
        }
    }

    /* Adding a gap, so all previous entries are > LZ4_DISTANCE_MAX back, is faster
     * than compressing without a gap. However, compressing with
     * currentOffset == 0 is faster still, so we preserve that case.
     */
    if (cctx->currentOffset != 0 && tableType == byU32) {
        DEBUGLOG(5, "LZ4_prepareTable: adding 64KB to currentOffset");
        cctx->currentOffset += 64 KB;
    }

    /* Finally, clear history */
    cctx->dictCtx = NULL;
    cctx->dictionary = NULL;
    cctx->dictSize = 0;
}

/** LZ4_compress_generic() :
 *  inlined, to ensure branches are decided at compilation time.
 *  Presumed already validated at this stage:
 *  - source != NULL
 *  - inputSize > 0
 */
LZ4_FORCE_INLINE int LZ4_compress_generic_validated(
    LZ4_stream_t_internal* const cctx,
    const char* const source,
    char* const dest,
    const int inputSize,
    int* inputConsumed, /* only written when outputDirective == fillOutput */
    const int maxOutputSize,
    const limitedOutput_directive outputDirective,
    const tableType_t tableType,
    const dict_directive dictDirective,
    const dictIssue_directive dictIssue,
    const int acceleration)
{
    int result;
    const BYTE* ip = (const BYTE*)source;

    U32 const startIndex = cctx->currentOffset;
    const BYTE* base = (const BYTE*)source - startIndex;
    const BYTE* lowLimit;

    const LZ4_stream_t_internal* dictCtx = (const LZ4_stream_t_internal*)cctx->dictCtx;
    const BYTE* const dictionary =
        dictDirective == usingDictCtx ? dictCtx->dictionary : cctx->dictionary;
    const U32 dictSize =
        dictDirective == usingDictCtx ? dictCtx->dictSize : cctx->dictSize;
    const U32 dictDelta = (dictDirective == usingDictCtx) ? startIndex - dictCtx->currentOffset : 0;   /* make indexes in dictCtx comparable with index in current context */

    int const maybe_extMem = (dictDirective == usingExtDict) || (dictDirective == usingDictCtx);
    U32 const prefixIdxLimit = startIndex - dictSize;   /* used when dictDirective == dictSmall */
    const BYTE* const dictEnd = dictionary ? dictionary + dictSize : dictionary;
    const BYTE* anchor = (const BYTE*)source;
    const BYTE* const iend = ip + inputSize;
    const BYTE* const mflimitPlusOne = iend - MFLIMIT + 1;
    const BYTE* const matchlimit = iend - LASTLITERALS;

    /* the dictCtx currentOffset is indexed on the start of the dictionary,
     * while a dictionary in the current context precedes the currentOffset */
    const BYTE* dictBase = !dictionary ? NULL : (dictDirective == usingDictCtx) ?
        dictionary + dictSize - dictCtx->currentOffset :
        dictionary + dictSize - startIndex;

    BYTE* op = (BYTE*)dest;
    BYTE* const olimit = op + maxOutputSize;

    U32 offset = 0;
    U32 forwardH;

    DEBUGLOG(5, "LZ4_compress_generic_validated: srcSize=%i, tableType=%u", inputSize, tableType);
    assert(ip != NULL);
    /* If init conditions are not met, we don't have to mark stream
     * as having dirty context, since no action was taken yet */
    if (outputDirective == fillOutput && maxOutputSize < 1) { return 0; } /* Impossible to store anything */
    if ((tableType == byU16) && (inputSize >= LZ4_64Klimit)) { return 0; }  /* Size too large (not within 64K limit) */
    if (tableType == byPtr) assert(dictDirective == noDict);      /* only supported use case with byPtr */
    assert(acceleration >= 1);

    lowLimit = (const BYTE*)source - (dictDirective == withPrefix64k ? dictSize : 0);

    /* Update context state */
    if (dictDirective == usingDictCtx) {
        /* Subsequent linked blocks can't use the dictionary. */
        /* Instead, they use the block we just compressed. */
        cctx->dictCtx = NULL;
        cctx->dictSize = (U32)inputSize;
    } else {
        cctx->dictSize += (U32)inputSize;
    }
    cctx->currentOffset += (U32)inputSize;
    cctx->tableType = (U32)tableType;

    if (inputSize < LZ4_minLength) goto _last_literals;        /* Input too small, no compression (all literals) */

    /* First Byte */
    LZ4_putPosition(ip, cctx->hashTable, tableType, base);
    ip++; forwardH = LZ4_hashPosition(ip, tableType);

    /* Main Loop */
    for (; ; ) {
        const BYTE* match;
        BYTE* token;
        const BYTE* filledIp;

        /* Find a match */
        if (tableType == byPtr) {
            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                match = LZ4_getPositionOnHash(h, cctx->hashTable, tableType, base);
                forwardH = LZ4_hashPosition(forwardIp, tableType);
                LZ4_putPositionOnHash(ip, h, cctx->hashTable, tableType, base);

            } while ((match + LZ4_DISTANCE_MAX < ip)
                || (LZ4_read32(match) != LZ4_read32(ip)));

        } else {   /* byU32, byU16 */

            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                U32 const current = (U32)(forwardIp - base);
                U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
                assert(matchIndex <= current);
                assert(forwardIp - base < (ptrdiff_t)(2 GB - 1));
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                if (dictDirective == usingDictCtx) {
                    if (matchIndex < startIndex) {
                        /* there was no match, try the dictionary */
                        assert(tableType == byU32);
                        matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                        match = dictBase + matchIndex;
                        matchIndex += dictDelta;   /* make dictCtx index comparable with current context */
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else if (dictDirective == usingExtDict) {
                    if (matchIndex < startIndex) {
                        DEBUGLOG(7, "extDict candidate: matchIndex=%5u  <  startIndex=%5u", matchIndex, startIndex);
                        assert(startIndex - matchIndex >= MINMATCH);
                        match = dictBase + matchIndex;
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else {   /* single continuous memory segment */
                    match = base + matchIndex;
                }
                forwardH = LZ4_hashPosition(forwardIp, tableType);
                LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);

                DEBUGLOG(7, "candidate at pos=%u  (offset=%u \n", matchIndex, current - matchIndex);
                if ((dictIssue == dictSmall) && (matchIndex < prefixIdxLimit)) { continue; }    /* match outside of valid area */
                assert(matchIndex < current);
                if (((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
                    && (matchIndex + LZ4_DISTANCE_MAX < current)) {
                    continue;
                } /* too far */
                assert((current - matchIndex) <= LZ4_DISTANCE_MAX);  /* match now expected within distance */

                if (LZ4_read32(match) == LZ4_read32(ip)) {
                    if (maybe_extMem) offset = current - matchIndex;
                    break;   /* match found */
                }

            } while (1);
        }

        /* Catch up */
        filledIp = ip;
        while (((ip > anchor) & (match > lowLimit)) && (unlikely(ip[-1] == match[-1]))) { ip--; match--; }

        /* Encode Literals */
        {   unsigned const litLength = (unsigned)(ip - anchor);
        token = op++;
        if ((outputDirective == limitedOutput) &&  /* Check output buffer overflow */
            (unlikely(op + litLength + (2 + 1 + LASTLITERALS) + (litLength / 255) > olimit))) {
            return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
        }
        if ((outputDirective == fillOutput) &&
            (unlikely(op + (litLength + 240) / 255 /* litlen */ + litLength /* literals */ + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit))) {
            op--;
            goto _last_literals;
        }
        if (litLength >= RUN_MASK) {
            int len = (int)(litLength - RUN_MASK);
            *token = (RUN_MASK << ML_BITS);
            for (; len >= 255; len -= 255) *op++ = 255;
            *op++ = (BYTE)len;
        }
        else *token = (BYTE)(litLength << ML_BITS);

        /* Copy Literals */
        LZ4_wildCopy8(op, anchor, op + litLength);
        op += litLength;
        DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
            (int)(anchor - (const BYTE*)source), litLength, (int)(ip - (const BYTE*)source));
        }

    _next_match:
        /* at this stage, the following variables must be correctly set :
         * - ip : at start of LZ operation
         * - match : at start of previous pattern occurence; can be within current prefix, or within extDict
         * - offset : if maybe_ext_memSegment==1 (constant)
         * - lowLimit : must be == dictionary to mean "match is within extDict"; must be == source otherwise
         * - token and *token : position to write 4-bits for match length; higher 4-bits for literal length supposed already written
         */

        if ((outputDirective == fillOutput) &&
            (op + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit)) {
            /* the match was too close to the end, rewind and go to last literals */
            op = token;
            goto _last_literals;
        }

        /* Encode Offset */
        if (maybe_extMem) {   /* static test */
            DEBUGLOG(6, "             with offset=%u  (ext if > %i)", offset, (int)(ip - (const BYTE*)source));
            assert(offset <= LZ4_DISTANCE_MAX && offset > 0);
            LZ4_writeLE16(op, (U16)offset); op += 2;
        } else {
            DEBUGLOG(6, "             with offset=%u  (same segment)", (U32)(ip - match));
            assert(ip - match <= LZ4_DISTANCE_MAX);
            LZ4_writeLE16(op, (U16)(ip - match)); op += 2;
        }

        /* Encode MatchLength */
        {   unsigned matchCode;

        if ((dictDirective == usingExtDict || dictDirective == usingDictCtx)
            && (lowLimit == dictionary) /* match within extDict */) {
            const BYTE* limit = ip + (dictEnd - match);
            assert(dictEnd > match);
            if (limit > matchlimit) limit = matchlimit;
            matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH, limit);
            ip += (size_t)matchCode + MINMATCH;
            if (ip == limit) {
                unsigned const more = LZ4_count(limit, (const BYTE*)source, matchlimit);
                matchCode += more;
                ip += more;
            }
            DEBUGLOG(6, "             with matchLength=%u starting in extDict", matchCode + MINMATCH);
        } else {
            matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH, matchlimit);
            ip += (size_t)matchCode + MINMATCH;
            DEBUGLOG(6, "             with matchLength=%u", matchCode + MINMATCH);
        }

        if ((outputDirective) &&    /* Check output buffer overflow */
            (unlikely(op + (1 + LASTLITERALS) + (matchCode + 240) / 255 > olimit))) {
            if (outputDirective == fillOutput) {
                /* Match description too long : reduce it */
                U32 newMatchCode = 15 /* in token */ - 1 /* to avoid needing a zero byte */ + ((U32)(olimit - op) - 1 - LASTLITERALS) * 255;
                ip -= matchCode - newMatchCode;
                assert(newMatchCode < matchCode);
                matchCode = newMatchCode;
                if (unlikely(ip <= filledIp)) {
                    /* We have already filled up to filledIp so if ip ends up less than filledIp
                     * we have positions in the hash table beyond the current position. This is
                     * a problem if we reuse the hash table. So we have to remove these positions
                     * from the hash table.
                     */
                    const BYTE* ptr;
                    DEBUGLOG(5, "Clearing %u positions", (U32)(filledIp - ip));
                    for (ptr = ip; ptr <= filledIp; ++ptr) {
                        U32 const h = LZ4_hashPosition(ptr, tableType);
                        LZ4_clearHash(h, cctx->hashTable, tableType);
                    }
                }
            } else {
                assert(outputDirective == limitedOutput);
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
        }
        if (matchCode >= ML_MASK) {
            *token += ML_MASK;
            matchCode -= ML_MASK;
            LZ4_write32(op, 0xFFFFFFFF);
            while (matchCode >= 4 * 255) {
                op += 4;
                LZ4_write32(op, 0xFFFFFFFF);
                matchCode -= 4 * 255;
            }
            op += matchCode / 255;
            *op++ = (BYTE)(matchCode % 255);
        } else
            *token += (BYTE)(matchCode);
        }
        /* Ensure we have enough space for the last literals. */
        assert(!(outputDirective == fillOutput && op + 1 + LASTLITERALS > olimit));

        anchor = ip;

        /* Test end of chunk */
        if (ip >= mflimitPlusOne) break;

        /* Fill table */
        LZ4_putPosition(ip - 2, cctx->hashTable, tableType, base);

        /* Test next position */
        if (tableType == byPtr) {

            match = LZ4_getPosition(ip, cctx->hashTable, tableType, base);
            LZ4_putPosition(ip, cctx->hashTable, tableType, base);
            if ((match + LZ4_DISTANCE_MAX >= ip)
                && (LZ4_read32(match) == LZ4_read32(ip)))
            { token = op++; *token = 0; goto _next_match; }

        } else {   /* byU32, byU16 */

            U32 const h = LZ4_hashPosition(ip, tableType);
            U32 const current = (U32)(ip - base);
            U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (dictDirective == usingDictCtx) {
                if (matchIndex < startIndex) {
                    /* there was no match, try the dictionary */
                    matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                    matchIndex += dictDelta;
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;  /* required for match length counter */
                }
            } else if (dictDirective == usingExtDict) {
                if (matchIndex < startIndex) {
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;   /* required for match length counter */
                }
            } else {   /* single memory segment */
                match = base + matchIndex;
            }
            LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (((dictIssue == dictSmall) ? (matchIndex >= prefixIdxLimit) : 1)
                && (((tableType == byU16) && (LZ4_DISTANCE_MAX == LZ4_DISTANCE_ABSOLUTE_MAX)) ? 1 : (matchIndex + LZ4_DISTANCE_MAX >= current))
                && (LZ4_read32(match) == LZ4_read32(ip))) {
                token = op++;
                *token = 0;
                if (maybe_extMem) offset = current - matchIndex;
                DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                    (int)(anchor - (const BYTE*)source), 0, (int)(ip - (const BYTE*)source));
                goto _next_match;
            }
        }

        /* Prepare next loop */
        forwardH = LZ4_hashPosition(++ip, tableType);

    }

_last_literals:
    /* Encode Last Literals */
    {   size_t lastRun = (size_t)(iend - anchor);
    if ((outputDirective) &&  /* Check output buffer overflow */
        (op + lastRun + 1 + ((lastRun + 255 - RUN_MASK) / 255) > olimit)) {
        if (outputDirective == fillOutput) {
            /* adapt lastRun to fill 'dst' */
            assert(olimit >= op);
            lastRun = (size_t)(olimit - op) - 1/*token*/;
            lastRun -= (lastRun + 256 - RUN_MASK) / 256;  /*additional length tokens*/
        } else {
            assert(outputDirective == limitedOutput);
            return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
        }
    }
    DEBUGLOG(6, "Final literal run : %i literals", (int)lastRun);
    if (lastRun >= RUN_MASK) {
        size_t accumulator = lastRun - RUN_MASK;
        *op++ = RUN_MASK << ML_BITS;
        for (; accumulator >= 255; accumulator -= 255) *op++ = 255;
        *op++ = (BYTE)accumulator;
    } else {
        *op++ = (BYTE)(lastRun << ML_BITS);
    }
    LZ4_memcpy(op, anchor, lastRun);
    ip = anchor + lastRun;
    op += lastRun;
    }

    if (outputDirective == fillOutput) {
        *inputConsumed = (int)(((const char*)ip) - source);
    }
    result = (int)(((char*)op) - dest);
    assert(result > 0);
    DEBUGLOG(5, "LZ4_compress_generic: compressed %i bytes into %i bytes", inputSize, result);
    return result;
}

#ifdef AOCL_LZ4_OPT
/** AOCL_LZ4_compress_generic_validated() :
 *  inlined, to ensure branches are decided at compilation time.
 *  Presumed already validated at this stage:
 *  - source != NULL
 *  - inputSize > 0
 *  Implements AOCL optimized LZ4 compression
 */
LZ4_FORCE_INLINE int AOCL_LZ4_compress_generic_validated(
                 LZ4_stream_t_internal* const cctx,
                 const char* const source,
                 char* const dest,
                 const int inputSize,
                 int *inputConsumed, /* only written when outputDirective == fillOutput */
                 const int maxOutputSize,
                 const limitedOutput_directive outputDirective,
                 const tableType_t tableType,
                 const dict_directive dictDirective,
                 const dictIssue_directive dictIssue,
                 const int acceleration)
{
    int result;
    const BYTE* ip = (const BYTE*) source;

    U32 const startIndex = cctx->currentOffset;
    const BYTE* base = (const BYTE*) source - startIndex;
    const BYTE* lowLimit;

    const LZ4_stream_t_internal* dictCtx = (const LZ4_stream_t_internal*) cctx->dictCtx;
    const BYTE* const dictionary =
        dictDirective == usingDictCtx ? dictCtx->dictionary : cctx->dictionary;
    const U32 dictSize =
        dictDirective == usingDictCtx ? dictCtx->dictSize : cctx->dictSize;
    const U32 dictDelta = (dictDirective == usingDictCtx) ? startIndex - dictCtx->currentOffset : 0;   /* make indexes in dictCtx comparable with index in current context */

    int const maybe_extMem = (dictDirective == usingExtDict) || (dictDirective == usingDictCtx);
    U32 const prefixIdxLimit = startIndex - dictSize;   /* used when dictDirective == dictSmall */
    const BYTE* const dictEnd = dictionary ? dictionary + dictSize : dictionary;
    const BYTE* anchor = (const BYTE*) source;
    const BYTE* const iend = ip + inputSize;
    const BYTE* const mflimitPlusOne = iend - MFLIMIT + 1;
    const BYTE* const matchlimit = iend - LASTLITERALS;

    /* the dictCtx currentOffset is indexed on the start of the dictionary,
     * while a dictionary in the current context precedes the currentOffset */
    const BYTE* dictBase = !dictionary ? NULL : (dictDirective == usingDictCtx) ?
                            dictionary + dictSize - dictCtx->currentOffset :
                            dictionary + dictSize - startIndex;

    BYTE* op = (BYTE*) dest;
    BYTE* const olimit = op + maxOutputSize;

    U32 offset = 0;
    U32 forwardH;

    DEBUGLOG(5, "AOCL_LZ4_compress_generic_validated: srcSize=%i, tableType=%u", inputSize, tableType);
    assert(ip != NULL);
    /* If init conditions are not met, we don't have to mark stream
     * as having dirty context, since no action was taken yet */
    if (outputDirective == fillOutput && maxOutputSize < 1) { return 0; } /* Impossible to store anything */
    if ((tableType == byU16) && (inputSize>=LZ4_64Klimit)) { return 0; }  /* Size too large (not within 64K limit) */
    if (tableType==byPtr) assert(dictDirective==noDict);      /* only supported use case with byPtr */
    assert(acceleration >= 1);

    lowLimit = (const BYTE*)source - (dictDirective == withPrefix64k ? dictSize : 0);

    /* Update context state */
    if (dictDirective == usingDictCtx) {
        /* Subsequent linked blocks can't use the dictionary. */
        /* Instead, they use the block we just compressed. */
        cctx->dictCtx = NULL;
        cctx->dictSize = (U32)inputSize;
    } else {
        cctx->dictSize += (U32)inputSize;
    }
    cctx->currentOffset += (U32)inputSize;
    cctx->tableType = (U32)tableType;

    if (inputSize<LZ4_minLength) goto _last_literals;        /* Input too small, no compression (all literals) */

    /* First Byte */
    AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
    int prevStep = 0;
    int presetMatchNb = 0;
#endif
    /* Main Loop */
    for ( ; ; ) {
        const BYTE* match;
        BYTE* token;
        const BYTE* filledIp;
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
        typedef union { reg_t u; BYTE c[8]; } vecInt;
        vecInt ipPrevData;
        int prevOffset = 0;
#endif

        /* Find a match */
        if (tableType == byPtr) {
            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                match = LZ4_getPositionOnHash(h, cctx->hashTable, tableType, base);
                forwardH = AOCL_LZ4_hashPosition(forwardIp, tableType);
                LZ4_putPositionOnHash(ip, h, cctx->hashTable, tableType, base);

            } while ( (match+LZ4_DISTANCE_MAX < ip)
                   || (LZ4_read32(match) != LZ4_read32(ip)) );

        } else {   /* byU32, byU16 */

            const BYTE* forwardIp = ip;
            int step = 1;
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
            int searchMatchNb = acceleration << (LZ4_skipTrigger-presetMatchNb);
#else
            int searchMatchNb = acceleration << LZ4_skipTrigger;
#endif
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
            U32 ipData;
#endif
            do {
                U32 const h = forwardH;
                U32 const current = (U32)(forwardIp - base);
                U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
                auto U32 matchData;
#endif
                assert(matchIndex <= current);
                assert(forwardIp - base < (ptrdiff_t)(2 GB - 1));
                ip = forwardIp;
                forwardIp += step;

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
                step = (searchMatchNb++ >> (LZ4_skipTrigger-presetMatchNb)) + prevStep;
#else
                step = (searchMatchNb++ >> LZ4_skipTrigger);
#endif

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                if (dictDirective == usingDictCtx) {
                    if (matchIndex < startIndex) {
                        /* there was no match, try the dictionary */
                        assert(tableType == byU32);
                        matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                        match = dictBase + matchIndex;
                        matchIndex += dictDelta;   /* make dictCtx index comparable with current context */
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else if (dictDirective==usingExtDict) {
                    if (matchIndex < startIndex) {
                        DEBUGLOG(7, "extDict candidate: matchIndex=%5u  <  startIndex=%5u", matchIndex, startIndex);
                        assert(startIndex - matchIndex >= MINMATCH);
                        match = dictBase + matchIndex;
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else {   /* single continuous memory segment */
                    match = base + matchIndex;
                }
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
                ipData=*(U32*)ip;
#endif
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
                prevOffset = ((ip - anchor) > 8) ? 8 : (ip - anchor);
#endif
                forwardH = AOCL_LZ4_hashPosition(forwardIp, tableType);
                LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);

                DEBUGLOG(7, "candidate at pos=%u  (offset=%u \n", matchIndex, current - matchIndex);
                if ((dictIssue == dictSmall) && (matchIndex < prefixIdxLimit)) { continue; }    /* match outside of valid area */
                assert(matchIndex < current);
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
                matchData=*(U32*)match;

                if (matchData == ipData) {
#else
                if (LZ4_read32(match) == LZ4_read32(ip)) {
#endif
                    /* The below conditional is moved inside `if(matchData == ipData)` for performance improvement */
                    if ( ((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
                        && (matchIndex+LZ4_DISTANCE_MAX < current)) {
                        continue;
                    } /* too far */
                    assert((current - matchIndex) <= LZ4_DISTANCE_MAX);  /* match now expected within distance */

#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
                    ipPrevData.u = *(reg_t*)(ip - prevOffset);
#endif
                    if (maybe_extMem) offset = current - matchIndex;

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
                    if (step > AOCL_LZ4_MATCH_SKIPPING_THRESHOLD) {
                        prevStep = (step / 2) - 1 ;   /* for the next sequence `step` starts from `half of current step` instead of 1. */
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2
                        presetMatchNb = 1;
#endif
                    } else {
                        prevStep = 0;                 /* for the next sequence `step` starts from 1. */
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2
                        presetMatchNb = 0;
#endif
                    }
#endif

#ifdef AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES

                    /* Hash and update hash table with indexes of ip+1, ip+2 and ip+3.
                       This results in storing additional potential matches which improves 
                       compression ratio. Recommended for higher compressibility use cases.
                    */
                    U32 next_h = AOCL_LZ4_hashPosition(ip+1, tableType);
                    LZ4_putIndexOnHash(current+1, next_h, cctx->hashTable, tableType);

                    next_h = AOCL_LZ4_hashPosition(ip+2, tableType);
                    LZ4_putIndexOnHash(current+2, next_h, cctx->hashTable, tableType);

                    next_h = AOCL_LZ4_hashPosition(ip+3, tableType);
                    LZ4_putIndexOnHash(current+3, next_h, cctx->hashTable, tableType);

#endif /* AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES */

                    break;   /* match found */
                }

            } while(1);
        }

        /* Catch up */
        filledIp = ip;
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
        prevOffset--;
        while ((prevOffset > -1) && ((ip>anchor) & (match > lowLimit)) && (unlikely(ipPrevData.c[prevOffset]==match[-1])))
        { 
            ip--; match--; prevOffset--;
        }
        while (((ip>anchor) & (match > lowLimit)) && (unlikely(ip[-1]==match[-1]))) 
        { 
            ip--; match--; 
        }
#else
        while (((ip > anchor) & (match > lowLimit)) && (unlikely(ip[-1] == match[-1]))) { ip--; match--; }
#endif

        /* Encode Literals */
        {   unsigned const litLength = (unsigned)(ip - anchor);
            token = op++;
            if ((outputDirective == limitedOutput) &&  /* Check output buffer overflow */
                (unlikely(op + litLength + (2 + 1 + LASTLITERALS) + (litLength/255) > olimit)) ) {
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
            if ((outputDirective == fillOutput) &&
                (unlikely(op + (litLength+240)/255 /* litlen */ + litLength /* literals */ + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit))) {
                op--;
                goto _last_literals;
            }
            if (litLength >= RUN_MASK) {
                int len = (int)(litLength - RUN_MASK);
                *token = (RUN_MASK<<ML_BITS);
                for(; len >= 255 ; len-=255) *op++ = 255;
                *op++ = (BYTE)len;
            }
            else *token = (BYTE)(litLength<<ML_BITS);

            /* Copy Literals */
            LZ4_wildCopy8(op, anchor, op+litLength);
            op+=litLength;
            DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                        (int)(anchor-(const BYTE*)source), litLength, (int)(ip-(const BYTE*)source));
        }

_next_match:
        /* at this stage, the following variables must be correctly set :
         * - ip : at start of LZ operation
         * - match : at start of previous pattern occurence; can be within current prefix, or within extDict
         * - offset : if maybe_ext_memSegment==1 (constant)
         * - lowLimit : must be == dictionary to mean "match is within extDict"; must be == source otherwise
         * - token and *token : position to write 4-bits for match length; higher 4-bits for literal length supposed already written
         */

        if ((outputDirective == fillOutput) &&
            (op + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit)) {
            /* the match was too close to the end, rewind and go to last literals */
            op = token;
            goto _last_literals;
        }

        /* Encode Offset */
        if (maybe_extMem) {   /* static test */
            DEBUGLOG(6, "             with offset=%u  (ext if > %i)", offset, (int)(ip - (const BYTE*)source));
            assert(offset <= LZ4_DISTANCE_MAX && offset > 0);
            LZ4_writeLE16(op, (U16)offset); op+=2;
        } else  {
            DEBUGLOG(6, "             with offset=%u  (same segment)", (U32)(ip - match));
            assert(ip-match <= LZ4_DISTANCE_MAX);
            LZ4_writeLE16(op, (U16)(ip - match)); op+=2;
        }

        /* Encode MatchLength */
        {   unsigned matchCode;

            if ( (dictDirective==usingExtDict || dictDirective==usingDictCtx)
              && (lowLimit==dictionary) /* match within extDict */ ) {
                const BYTE* limit = ip + (dictEnd-match);
                assert(dictEnd > match);
                if (limit > matchlimit) limit = matchlimit;
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, limit);
                ip += (size_t)matchCode + MINMATCH;
                if (ip==limit) {
                    unsigned const more = LZ4_count(limit, (const BYTE*)source, matchlimit);
                    matchCode += more;
                    ip += more;
                }
                DEBUGLOG(6, "             with matchLength=%u starting in extDict", matchCode+MINMATCH);
            } else {
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, matchlimit);
                ip += (size_t)matchCode + MINMATCH;
                DEBUGLOG(6, "             with matchLength=%u", matchCode+MINMATCH);
            }

            if ((outputDirective) &&    /* Check output buffer overflow */
                (unlikely(op + (1 + LASTLITERALS) + (matchCode+240)/255 > olimit)) ) {
                if (outputDirective == fillOutput) {
                    /* Match description too long : reduce it */
                    U32 newMatchCode = 15 /* in token */ - 1 /* to avoid needing a zero byte */ + ((U32)(olimit - op) - 1 - LASTLITERALS) * 255;
                    ip -= matchCode - newMatchCode;
                    assert(newMatchCode < matchCode);
                    matchCode = newMatchCode;
                    if (unlikely(ip <= filledIp)) {
                        /* We have already filled up to filledIp so if ip ends up less than filledIp
                         * we have positions in the hash table beyond the current position. This is
                         * a problem if we reuse the hash table. So we have to remove these positions
                         * from the hash table.
                         */
                        const BYTE* ptr;
                        DEBUGLOG(5, "Clearing %u positions", (U32)(filledIp - ip));
                        for (ptr = ip; ptr <= filledIp; ++ptr) {
                            U32 const h = AOCL_LZ4_hashPosition(ptr, tableType);
                            LZ4_clearHash(h, cctx->hashTable, tableType);
                        }
                    }
                } else {
                    assert(outputDirective == limitedOutput);
                    return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
                }
            }
            if (matchCode >= ML_MASK) {
                *token += ML_MASK;
                matchCode -= ML_MASK;
                LZ4_write32(op, 0xFFFFFFFF);
                while (matchCode >= 4*255) {
                    op+=4;
                    LZ4_write32(op, 0xFFFFFFFF);
                    matchCode -= 4*255;
                }
                op += matchCode / 255;
                *op++ = (BYTE)(matchCode % 255);
            } else
                *token += (BYTE)(matchCode);
        }
        /* Ensure we have enough space for the last literals. */
        assert(!(outputDirective == fillOutput && op + 1 + LASTLITERALS > olimit));

        anchor = ip;

        /* Test end of chunk */
        if (ip >= mflimitPlusOne) break;

        /* Fill table */
        AOCL_LZ4_putPosition(ip-2, cctx->hashTable, tableType, base);

        /* Test next position */
        if (tableType == byPtr) {

            match = AOCL_LZ4_getPosition(ip, cctx->hashTable, tableType, base);
            AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
            if ( (match+LZ4_DISTANCE_MAX >= ip)
              && (LZ4_read32(match) == LZ4_read32(ip)) )
            { token=op++; *token=0; goto _next_match; }

        } else {   /* byU32, byU16 */

            U32 const h = AOCL_LZ4_hashPosition(ip, tableType);
            U32 const current = (U32)(ip-base);
            U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (dictDirective == usingDictCtx) {
                if (matchIndex < startIndex) {
                    /* there was no match, try the dictionary */
                    matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                    matchIndex += dictDelta;
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;  /* required for match length counter */
                }
            } else if (dictDirective==usingExtDict) {
                if (matchIndex < startIndex) {
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;   /* required for match length counter */
                }
            } else {   /* single memory segment */
                match = base + matchIndex;
            }
            LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if ( ((dictIssue==dictSmall) ? (matchIndex >= prefixIdxLimit) : 1)
              && (((tableType==byU16) && (LZ4_DISTANCE_MAX == LZ4_DISTANCE_ABSOLUTE_MAX)) ? 1 : (matchIndex+LZ4_DISTANCE_MAX >= current))
              && (LZ4_read32(match) == LZ4_read32(ip)) ) {
                token=op++;
                *token=0;
                if (maybe_extMem) offset = current - matchIndex;
                DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                            (int)(anchor-(const BYTE*)source), 0, (int)(ip-(const BYTE*)source));
                goto _next_match;
            }
        }

        /* Prepare next loop */
        forwardH = AOCL_LZ4_hashPosition(++ip, tableType);

    }

_last_literals:
    /* Encode Last Literals */
    {   size_t lastRun = (size_t)(iend - anchor);
        if ( (outputDirective) &&  /* Check output buffer overflow */
            (op + lastRun + 1 + ((lastRun+255-RUN_MASK)/255) > olimit)) {
            if (outputDirective == fillOutput) {
                /* adapt lastRun to fill 'dst' */
                assert(olimit >= op);
                lastRun  = (size_t)(olimit-op) - 1/*token*/;
                lastRun -= (lastRun + 256 - RUN_MASK) / 256;  /*additional length tokens*/
            } else {
                assert(outputDirective == limitedOutput);
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
        }
        DEBUGLOG(6, "Final literal run : %i literals", (int)lastRun);
        if (lastRun >= RUN_MASK) {
            size_t accumulator = lastRun - RUN_MASK;
            *op++ = RUN_MASK << ML_BITS;
            for(; accumulator >= 255 ; accumulator-=255) *op++ = 255;
            *op++ = (BYTE) accumulator;
        } else {
            *op++ = (BYTE)(lastRun<<ML_BITS);
        }
        LZ4_memcpy(op, anchor, lastRun);
        ip = anchor + lastRun;
        op += lastRun;
    }

    if (outputDirective == fillOutput) {
        *inputConsumed = (int) (((const char*)ip)-source);
    }
    result = (int)(((char*)op) - dest);
    assert(result > 0);
    DEBUGLOG(5, "LZ4_compress_generic: compressed %i bytes into %i bytes", inputSize, result);
    return result;
}

#ifdef AOCL_ENABLE_THREADS
#ifdef AOCL_LZ4_AVX_OPT
/* Even though this function does not use AVX instructions, output format it generates (with RAP frame) is
 * not directly compatible with single threaded decompressor. Hence to pair it with 
 * AOCL_LZ4_decompress_safe_mt it is enabled only for AOCL_LZ4_AVX_OPT case.
*/
//Same as AOCL_LZ4_compress_generic_validated, but with state information for Multi-threaded support
LZ4_FORCE_INLINE int AOCL_LZ4_compress_generic_validated_mt(
    LZ4_stream_t_internal* const cctx,
    const char* const source,
    char* const dest,
    const int inputSize,
    int* inputConsumed, /* only written when outputDirective == fillOutput */
    unsigned char** last_anchor_ptr,
    unsigned int* last_bytes_len,
    const int maxOutputSize,
    const limitedOutput_directive outputDirective,
    const tableType_t tableType,
    const dict_directive dictDirective,
    const dictIssue_directive dictIssue,
    const int acceleration)
{
    int result;
    const BYTE* ip = (const BYTE*)source;

    U32 const startIndex = cctx->currentOffset;
    const BYTE* base = (const BYTE*)source - startIndex;
    const BYTE* lowLimit;

    const LZ4_stream_t_internal* dictCtx = (const LZ4_stream_t_internal*)cctx->dictCtx;
    const BYTE* const dictionary =
        dictDirective == usingDictCtx ? dictCtx->dictionary : cctx->dictionary;
    const U32 dictSize =
        dictDirective == usingDictCtx ? dictCtx->dictSize : cctx->dictSize;
    const U32 dictDelta = (dictDirective == usingDictCtx) ? startIndex - dictCtx->currentOffset : 0;   /* make indexes in dictCtx comparable with index in current context */

    int const maybe_extMem = (dictDirective == usingExtDict) || (dictDirective == usingDictCtx);
    U32 const prefixIdxLimit = startIndex - dictSize;   /* used when dictDirective == dictSmall */
    const BYTE* const dictEnd = dictionary ? dictionary + dictSize : dictionary;
    const BYTE* anchor = (const BYTE*)source;
    const BYTE* const iend = ip + inputSize;
    const BYTE* const mflimitPlusOne = iend - MFLIMIT + 1;
    const BYTE* const matchlimit = iend - LASTLITERALS;

    /* the dictCtx currentOffset is indexed on the start of the dictionary,
     * while a dictionary in the current context precedes the currentOffset */
    const BYTE* dictBase = !dictionary ? NULL : (dictDirective == usingDictCtx) ?
        dictionary + dictSize - dictCtx->currentOffset :
        dictionary + dictSize - startIndex;

    BYTE* op = (BYTE*)dest;
    BYTE* const olimit = op + maxOutputSize;
    BYTE* dst_without_lastLiterals;
    U32 offset = 0;
    U32 forwardH;

    DEBUGLOG(5, "AOCL_LZ4_compress_generic_validated_mt: srcSize=%i, tableType=%u", inputSize, tableType);
    assert(ip != NULL);
    /* If init conditions are not met, we don't have to mark stream
     * as having dirty context, since no action was taken yet */
    if (outputDirective == fillOutput && maxOutputSize < 1) { return 0; } /* Impossible to store anything */
    if ((tableType == byU16) && (inputSize >= LZ4_64Klimit)) { return 0; }  /* Size too large (not within 64K limit) */
    if (tableType == byPtr) assert(dictDirective == noDict);      /* only supported use case with byPtr */
    assert(acceleration >= 1);

    lowLimit = (const BYTE*)source - (dictDirective == withPrefix64k ? dictSize : 0);

    /* Update context state */
    if (dictDirective == usingDictCtx) {
        /* Subsequent linked blocks can't use the dictionary. */
        /* Instead, they use the block we just compressed. */
        cctx->dictCtx = NULL;
        cctx->dictSize = (U32)inputSize;
    }
    else {
        cctx->dictSize += (U32)inputSize;
    }
    cctx->currentOffset += (U32)inputSize;
    cctx->tableType = (U32)tableType;

    if (inputSize < LZ4_minLength) goto _last_literals;        /* Input too small, no compression (all literals) */

    /* First Byte */
    AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
    int prevStep = 0;
    int presetMatchNb = 0;
#endif
    /* Main Loop */
    for (; ; ) {
        const BYTE* match;
        BYTE* token;
        const BYTE* filledIp;
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
        typedef union { reg_t u; BYTE c[8]; } vecInt;
        vecInt ipPrevData;
        int prevOffset = 0;
#endif

        /* Find a match */
        if (tableType == byPtr) {
            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                match = LZ4_getPositionOnHash(h, cctx->hashTable, tableType, base);
                forwardH = AOCL_LZ4_hashPosition(forwardIp, tableType);
                LZ4_putPositionOnHash(ip, h, cctx->hashTable, tableType, base);

            } while ((match + LZ4_DISTANCE_MAX < ip)
                || (LZ4_read32(match) != LZ4_read32(ip)));

        }
        else {   /* byU32, byU16 */

            const BYTE* forwardIp = ip;
            int step = 1;
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
            int searchMatchNb = acceleration << (LZ4_skipTrigger - presetMatchNb);
#else
            int searchMatchNb = acceleration << LZ4_skipTrigger;
#endif
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
            U32 ipData;
#endif
            //printf("Thread [id: %d] : Starting a new match finding loop with ip [%x]\n", omp_get_thread_num(), ip);
            do {
                U32 const h = forwardH;
                U32 const current = (U32)(forwardIp - base);
                U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
                auto U32 matchData;
#endif
                assert(matchIndex <= current);
                assert(forwardIp - base < (ptrdiff_t)(2 GB - 1));
                ip = forwardIp;
                forwardIp += step;

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
                step = (searchMatchNb++ >> (LZ4_skipTrigger - presetMatchNb)) + prevStep;
#else
                step = (searchMatchNb++ >> LZ4_skipTrigger);
#endif
                //printf("Thread [id: %d] : finding match in the loop with ip [%x], step [%d]\n", omp_get_thread_num(), ip, step);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                if (dictDirective == usingDictCtx) {
                    if (matchIndex < startIndex) {
                        /* there was no match, try the dictionary */
                        assert(tableType == byU32);
                        matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                        match = dictBase + matchIndex;
                        matchIndex += dictDelta;   /* make dictCtx index comparable with current context */
                        lowLimit = dictionary;
                    }
                    else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                }
                else if (dictDirective == usingExtDict) {
                    if (matchIndex < startIndex) {
                        DEBUGLOG(7, "extDict candidate: matchIndex=%5u  <  startIndex=%5u", matchIndex, startIndex);
                        assert(startIndex - matchIndex >= MINMATCH);
                        match = dictBase + matchIndex;
                        lowLimit = dictionary;
                    }
                    else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                }
                else {   /* single continuous memory segment */
                    match = base + matchIndex;
                }
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
                ipData = *(U32*)ip;
#endif
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
                prevOffset = ((ip - anchor) > 8) ? 8 : (ip - anchor);
#endif
                forwardH = AOCL_LZ4_hashPosition(forwardIp, tableType);
                LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);

                DEBUGLOG(7, "candidate at pos=%u  (offset=%u \n", matchIndex, current - matchIndex);
                if ((dictIssue == dictSmall) && (matchIndex < prefixIdxLimit)) { continue; }    /* match outside of valid area */
                assert(matchIndex < current);
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_LOAD_EARLY
                matchData = *(U32*)match;

                if (matchData == ipData) {
#else
                if (LZ4_read32(match) == LZ4_read32(ip)) {
#endif
                    /* The below conditional is moved inside `if(matchData == ipData)` for performance improvement */
                    if (((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
                        && (matchIndex + LZ4_DISTANCE_MAX < current)) {
                        continue;
                    } /* too far */
                    assert((current - matchIndex) <= LZ4_DISTANCE_MAX);  /* match now expected within distance */

#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
                    ipPrevData.u = *(reg_t*)(ip - prevOffset);
#endif
                    if (maybe_extMem) offset = current - matchIndex;

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
                    if (step > AOCL_LZ4_MATCH_SKIPPING_THRESHOLD) {
                        prevStep = (step / 2) - 1;   /* for the next sequence `step` starts from `half of current step` instead of 1. */
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2
                        presetMatchNb = 1;
#endif
                    }
                    else {
                        prevStep = 0;                 /* for the next sequence `step` starts from 1. */
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2
                        presetMatchNb = 0;
#endif
                    }
#endif

                    break;   /* match found */
                }

            } while (1);
        }

        /* Catch up */
        filledIp = ip;
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
        prevOffset--;
        while ((prevOffset > -1) && ((ip > anchor) & (match > lowLimit)) && (unlikely(ipPrevData.c[prevOffset] == match[-1])))
        {
            ip--; match--; prevOffset--;
        }
        while (((ip > anchor) & (match > lowLimit)) && (unlikely(ip[-1] == match[-1])))
        {
            ip--; match--;
        }
#else
        while (((ip > anchor) & (match > lowLimit)) && (unlikely(ip[-1] == match[-1]))) { ip--; match--; }
#endif

        /* Encode Literals */
        {   unsigned const litLength = (unsigned)(ip - anchor);
        token = op++;
        if ((outputDirective == limitedOutput) &&  /* Check output buffer overflow */
            (unlikely(op + litLength + (2 + 1 + LASTLITERALS) + (litLength / 255) > olimit))) {
            return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
        }
        if ((outputDirective == fillOutput) &&
            (unlikely(op + (litLength + 240) / 255 /* litlen */ + litLength /* literals */ + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit))) {
            op--;
            goto _last_literals;
        }
        if (litLength >= RUN_MASK) {
            int len = (int)(litLength - RUN_MASK);
            *token = (RUN_MASK << ML_BITS);
            for (; len >= 255; len -= 255) *op++ = 255;
            *op++ = (BYTE)len;
        }
        else *token = (BYTE)(litLength << ML_BITS);

        /* Copy Literals */
        LZ4_wildCopy8(op, anchor, op + litLength);
        op += litLength;
        DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
            (int)(anchor - (const BYTE*)source), litLength, (int)(ip - (const BYTE*)source));
        }

    _next_match:
        /* at this stage, the following variables must be correctly set :
         * - ip : at start of LZ operation
         * - match : at start of previous pattern occurence; can be within current prefix, or within extDict
         * - offset : if maybe_ext_memSegment==1 (constant)
         * - lowLimit : must be == dictionary to mean "match is within extDict"; must be == source otherwise
         * - token and *token : position to write 4-bits for match length; higher 4-bits for literal length supposed already written
         */

        if ((outputDirective == fillOutput) &&
            (op + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit)) {
            /* the match was too close to the end, rewind and go to last literals */
            op = token;
            goto _last_literals;
        }

        /* Encode Offset */
        if (maybe_extMem) {   /* static test */
            DEBUGLOG(6, "             with offset=%u  (ext if > %i)", offset, (int)(ip - (const BYTE*)source));
            assert(offset <= LZ4_DISTANCE_MAX && offset > 0);
            LZ4_writeLE16(op, (U16)offset); op += 2;
        }
        else {
            DEBUGLOG(6, "             with offset=%u  (same segment)", (U32)(ip - match));
            assert(ip - match <= LZ4_DISTANCE_MAX);
            LZ4_writeLE16(op, (U16)(ip - match)); op += 2;
        }

        /* Encode MatchLength */
        {   unsigned matchCode;

        if ((dictDirective == usingExtDict || dictDirective == usingDictCtx)
            && (lowLimit == dictionary) /* match within extDict */) {
            const BYTE* limit = ip + (dictEnd - match);
            assert(dictEnd > match);
            if (limit > matchlimit) limit = matchlimit;
            matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH, limit);
            ip += (size_t)matchCode + MINMATCH;
            if (ip == limit) {
                unsigned const more = LZ4_count(limit, (const BYTE*)source, matchlimit);
                matchCode += more;
                ip += more;
            }
            DEBUGLOG(6, "             with matchLength=%u starting in extDict", matchCode + MINMATCH);
        }
        else {
            matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH, matchlimit);
            ip += (size_t)matchCode + MINMATCH;
            DEBUGLOG(6, "             with matchLength=%u", matchCode + MINMATCH);
        }

        if ((outputDirective) &&    /* Check output buffer overflow */
            (unlikely(op + (1 + LASTLITERALS) + (matchCode + 240) / 255 > olimit))) {
            if (outputDirective == fillOutput) {
                /* Match description too long : reduce it */
                U32 newMatchCode = 15 /* in token */ - 1 /* to avoid needing a zero byte */ + ((U32)(olimit - op) - 1 - LASTLITERALS) * 255;
                ip -= matchCode - newMatchCode;
                assert(newMatchCode < matchCode);
                matchCode = newMatchCode;
                if (unlikely(ip <= filledIp)) {
                    /* We have already filled up to filledIp so if ip ends up less than filledIp
                     * we have positions in the hash table beyond the current position. This is
                     * a problem if we reuse the hash table. So we have to remove these positions
                     * from the hash table.
                     */
                    const BYTE* ptr;
                    DEBUGLOG(5, "Clearing %u positions", (U32)(filledIp - ip));
                    for (ptr = ip; ptr <= filledIp; ++ptr) {
                        U32 const h = AOCL_LZ4_hashPosition(ptr, tableType);
                        LZ4_clearHash(h, cctx->hashTable, tableType);
                    }
                }
            }
            else {
                assert(outputDirective == limitedOutput);
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
        }
        if (matchCode >= ML_MASK) {
            *token += ML_MASK;
            matchCode -= ML_MASK;
            LZ4_write32(op, 0xFFFFFFFF);
            while (matchCode >= 4 * 255) {
                op += 4;
                LZ4_write32(op, 0xFFFFFFFF);
                matchCode -= 4 * 255;
            }
            op += matchCode / 255;
            *op++ = (BYTE)(matchCode % 255);
        }
        else
            *token += (BYTE)(matchCode);
        }
        /* Ensure we have enough space for the last literals. */
        assert(!(outputDirective == fillOutput && op + 1 + LASTLITERALS > olimit));

        anchor = ip;

        /* Test end of chunk */
        if (ip >= mflimitPlusOne) break;

        /* Fill table */
        AOCL_LZ4_putPosition(ip - 2, cctx->hashTable, tableType, base);

        /* Test next position */
        if (tableType == byPtr) {

            match = AOCL_LZ4_getPosition(ip, cctx->hashTable, tableType, base);
            AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
            if ((match + LZ4_DISTANCE_MAX >= ip)
                && (LZ4_read32(match) == LZ4_read32(ip)))
            {
                token = op++; *token = 0; goto _next_match;
            }

        }
        else {   /* byU32, byU16 */

            U32 const h = AOCL_LZ4_hashPosition(ip, tableType);
            U32 const current = (U32)(ip - base);
            U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (dictDirective == usingDictCtx) {
                if (matchIndex < startIndex) {
                    /* there was no match, try the dictionary */
                    matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                    matchIndex += dictDelta;
                }
                else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;  /* required for match length counter */
                }
            }
            else if (dictDirective == usingExtDict) {
                if (matchIndex < startIndex) {
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                }
                else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;   /* required for match length counter */
                }
            }
            else {   /* single memory segment */
                match = base + matchIndex;
            }
            LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (((dictIssue == dictSmall) ? (matchIndex >= prefixIdxLimit) : 1)
                && (((tableType == byU16) && (LZ4_DISTANCE_MAX == LZ4_DISTANCE_ABSOLUTE_MAX)) ? 1 : (matchIndex + LZ4_DISTANCE_MAX >= current))
                && (LZ4_read32(match) == LZ4_read32(ip))) {
                token = op++;
                *token = 0;
                if (maybe_extMem) offset = current - matchIndex;
                DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                    (int)(anchor - (const BYTE*)source), 0, (int)(ip - (const BYTE*)source));
                goto _next_match;
            }
        }

        /* Prepare next loop */
        forwardH = AOCL_LZ4_hashPosition(++ip, tableType);

    }

_last_literals:
    dst_without_lastLiterals = op;
    /* Encode Last Literals */
    {   size_t lastRun = (size_t)(iend - anchor);

    if ((outputDirective) &&  /* Check output buffer overflow */
        (op + lastRun + 1 + ((lastRun + 255 - RUN_MASK) / 255) > olimit)) {
        if (outputDirective == fillOutput) {
            /* adapt lastRun to fill 'dst' */
            assert(olimit >= op);
            lastRun = (size_t)(olimit - op) - 1/*token*/;
            lastRun -= (lastRun + 256 - RUN_MASK) / 256;  /*additional length tokens*/
        }
        else {
            assert(outputDirective == limitedOutput);
            return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
        }
    }
    DEBUGLOG(6, "Final literal run : %i literals", (int)lastRun);

    if (lastRun >= RUN_MASK) {
        size_t accumulator = lastRun - RUN_MASK;
        *op++ = RUN_MASK << ML_BITS;
        for (; accumulator >= 255; accumulator -= 255) *op++ = 255;
        *op++ = (BYTE)accumulator;
    }
    else {
        *op++ = (BYTE)(lastRun << ML_BITS);
    }
    LZ4_memcpy(op, anchor, lastRun);
    ip = anchor + lastRun;
    op += lastRun;
    }

    if (outputDirective == fillOutput) {
        *inputConsumed = (int)(((const char*)ip) - source);
    }
    
    if (last_bytes_len != NULL)
    {
        result = (int)(((char*)dst_without_lastLiterals) - dest);
        *last_anchor_ptr = (BYTE*)anchor;//src pointer until which compressed output is written : To support ST decompression on parallel compressed stream
        *last_bytes_len = (size_t)(iend - anchor);//length of src bytes pending for compression : To support ST decompression on parallel compressed stream
    }
    else
    {
        result = (int)(((char*)op) - dest);
        *last_anchor_ptr = (BYTE*)op;//Write the complete commpressed chunk
        //*last_bytes_len = 0;//Last thread needs no joining with the next chunk
    }

    // result=0 when no match found, (all literals).
    assert(result >= 0);
    DEBUGLOG(5, "LZ4_compress_generic: compressed %i bytes into %i bytes", inputSize, result);
    return result;
}
#endif /* AOCL_LZ4_AVX_OPT */
#endif /* AOCL_ENABLE_THREADS */
#endif /* AOCL_LZ4_OPT */

/** LZ4_compress_generic() :
 *  inlined, to ensure branches are decided at compilation time;
 *  takes care of src == (NULL, 0)
 *  and forward the rest to LZ4_compress_generic_validated */
LZ4_FORCE_INLINE int LZ4_compress_generic(
                 LZ4_stream_t_internal* const cctx,
                 const char* const src,
                 char* const dst,
                 const int srcSize,
                 int *inputConsumed, /* only written when outputDirective == fillOutput */
                 const int dstCapacity,
                 const limitedOutput_directive outputDirective,
                 const tableType_t tableType,
                 const dict_directive dictDirective,
                 const dictIssue_directive dictIssue,
                 const int acceleration)
{
    DEBUGLOG(5, "LZ4_compress_generic: srcSize=%i, dstCapacity=%i",
                srcSize, dstCapacity);

    if ((U32)srcSize > (U32)LZ4_MAX_INPUT_SIZE) { return 0; }  /* Unsupported srcSize, too large (or negative) */
    if (srcSize == 0) {   /* src == NULL supported if srcSize == 0 */
        if (outputDirective != notLimited && dstCapacity <= 0) return 0;  /* no output, can't write anything */
        DEBUGLOG(5, "Generating an empty block");
        assert(outputDirective == notLimited || dstCapacity >= 1);
        assert(dst != NULL);
        dst[0] = 0;
        if (outputDirective == fillOutput) {
            assert (inputConsumed != NULL);
            *inputConsumed = 0;
        }
        return 1;
    }
    assert(src != NULL);

    return LZ4_compress_generic_validated(cctx, src, dst, srcSize,
                inputConsumed, /* only written into if outputDirective == fillOutput */
                dstCapacity, outputDirective,
                tableType, dictDirective, dictIssue, acceleration);
}

#ifdef AOCL_LZ4_OPT
/** AOCL_LZ4_compress_generic() :
 *  inlined, to ensure branches are decided at compilation time;
 *  takes care of src == (NULL, 0)
 *  and forward the rest to AOCL_LZ4_compress_generic_validated */
LZ4_FORCE_INLINE int AOCL_LZ4_compress_generic(
    LZ4_stream_t_internal* const cctx,
    const char* const src,
    char* const dst,
    const int srcSize,
    int* inputConsumed, /* only written when outputDirective == fillOutput */
    const int dstCapacity,
    const limitedOutput_directive outputDirective,
    const tableType_t tableType,
    const dict_directive dictDirective,
    const dictIssue_directive dictIssue,
    const int acceleration)
{
    DEBUGLOG(5, "AOCL_LZ4_compress_generic: srcSize=%i, dstCapacity=%i",
        srcSize, dstCapacity);

    if ((U32)srcSize > (U32)LZ4_MAX_INPUT_SIZE) { return 0; }  /* Unsupported srcSize, too large (or negative) */
    if (srcSize == 0) {   /* src == NULL supported if srcSize == 0 */
        if (outputDirective != notLimited && dstCapacity <= 0) return 0;  /* no output, can't write anything */
        DEBUGLOG(5, "Generating an empty block");
        assert(outputDirective == notLimited || dstCapacity >= 1);
        assert(dst != NULL);
        dst[0] = 0;
        if (outputDirective == fillOutput) {
            assert(inputConsumed != NULL);
            *inputConsumed = 0;
        }
        return 1;
    }
    assert(src != NULL);

    return AOCL_LZ4_compress_generic_validated(cctx, src, dst, srcSize,
        inputConsumed, /* only written into if outputDirective == fillOutput */
        dstCapacity, outputDirective,
        tableType, dictDirective, dictIssue, acceleration);
}
#endif /* AOCL_LZ4_OPT */

#ifdef AOCL_ENABLE_THREADS
#ifdef AOCL_LZ4_AVX_OPT
//For multi-threaded compression
LZ4_FORCE_INLINE int AOCL_LZ4_compress_generic_mt(
    LZ4_stream_t_internal* const cctx,
    const char* const src,
    char* const dst,
    const int srcSize,
    int* inputConsumed, /* only written when outputDirective == fillOutput */
    unsigned char** last_anchor_ptr,
    unsigned int* last_bytes_len,
    const int dstCapacity,
    const limitedOutput_directive outputDirective,
    const tableType_t tableType,
    const dict_directive dictDirective,
    const dictIssue_directive dictIssue,
    const int acceleration)
{
    DEBUGLOG(5, "AOCL_LZ4_compress_generic_mt: srcSize=%i, dstCapacity=%i",
        srcSize, dstCapacity);

    if ((U32)srcSize > (U32)LZ4_MAX_INPUT_SIZE) { return 0; }  /* Unsupported srcSize, too large (or negative) */
    if (srcSize == 0) {   /* src == NULL supported if srcSize == 0 */
        if (outputDirective != notLimited && dstCapacity <= 0) return 0;  /* no output, can't write anything */
        DEBUGLOG(5, "Generating an empty block");
        assert(outputDirective == notLimited || dstCapacity >= 1);
        assert(dst != NULL);
        dst[0] = 0;
        if (outputDirective == fillOutput) {
            assert(inputConsumed != NULL);
            *inputConsumed = 0;
        }
        return 1;
    }
    assert(src != NULL);

    return AOCL_LZ4_compress_generic_validated_mt(cctx, src, dst, srcSize,
        inputConsumed, /* only written into if outputDirective == fillOutput */
        last_anchor_ptr,
        last_bytes_len,
        dstCapacity, outputDirective,
        tableType, dictDirective, dictIssue, acceleration);
}
#endif /* AOCL_LZ4_AVX_OPT */
#endif /* AOCL_ENABLE_THREADS */

int LZ4_compress_fast_extState(void* state, const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    AOCL_SETUP_NATIVE();
    if(state==NULL || (source==NULL && inputSize!=0) || dest==NULL)
        return 0;
    
    LZ4_stream_t_internal* const ctx = & LZ4_initStream(state, sizeof(LZ4_stream_t)) -> internal_donotuse;
    assert(ctx != NULL);
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;
    if (maxOutputSize >= LZ4_compressBound(inputSize)) {
        if (inputSize < LZ4_64Klimit) {
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, byU16, noDict, noDictIssue, acceleration);
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    } else {
        if (inputSize < LZ4_64Klimit) {
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, byU16, noDict, noDictIssue, acceleration);
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}

#ifdef AOCL_LZ4_OPT
int AOCL_LZ4_compress_fast_extState(void* state, const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    AOCL_SETUP_NATIVE();
    if(state==NULL || (source==NULL && inputSize!=0) || dest==NULL)
        return 0;
    
    LZ4_stream_t_internal* const ctx = &LZ4_initStream(state, sizeof(LZ4_stream_t))->internal_donotuse;
    assert(ctx != NULL);
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;
    if (maxOutputSize >= LZ4_compressBound(inputSize)) {
        if (inputSize < LZ4_64Klimit) {
            return AOCL_LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, byU16, noDict, noDictIssue, acceleration);
        }
        else {
            const tableType_t tableType = ((sizeof(void*) == 4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return AOCL_LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    }
    else {
        if (inputSize < LZ4_64Klimit) {
            return AOCL_LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, byU16, noDict, noDictIssue, acceleration);
        }
        else {
            const tableType_t tableType = ((sizeof(void*) == 4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return AOCL_LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}
#endif /* AOCL_LZ4_OPT */

#ifdef AOCL_ENABLE_THREADS
#ifdef AOCL_LZ4_AVX_OPT
//For mutli-threaded compression
int AOCL_LZ4_compress_fast_extState_mt(void* state, const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration, unsigned char** last_anchor_ptr, unsigned int* last_bytes_len)
{
    if (state == NULL || (source == NULL && inputSize != 0) || dest == NULL)
        return 0;

    LZ4_stream_t_internal* const ctx = &LZ4_initStream(state, sizeof(LZ4_stream_t))->internal_donotuse;
    assert(ctx != NULL);
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;
    if (maxOutputSize >= LZ4_compressBound(inputSize)) {
        if (inputSize < LZ4_64Klimit) {
            return AOCL_LZ4_compress_generic_mt(ctx, source, dest, inputSize, NULL, last_anchor_ptr, last_bytes_len, 0, notLimited, byU16, noDict, noDictIssue, acceleration);
        }
        else {
            const tableType_t tableType = ((sizeof(void*) == 4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return AOCL_LZ4_compress_generic_mt(ctx, source, dest, inputSize, NULL, last_anchor_ptr, last_bytes_len, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    }
    else {
        if (inputSize < LZ4_64Klimit) {
            return AOCL_LZ4_compress_generic_mt(ctx, source, dest, inputSize, NULL, last_anchor_ptr, last_bytes_len, maxOutputSize, limitedOutput, byU16, noDict, noDictIssue, acceleration);
        }
        else {
            const tableType_t tableType = ((sizeof(void*) == 4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return AOCL_LZ4_compress_generic_mt(ctx, source, dest, inputSize, NULL, last_anchor_ptr, last_bytes_len, maxOutputSize, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}
#endif /* AOCL_LZ4_AVX_OPT */
#endif /* AOCL_ENABLE_THREADS */

/**
 * LZ4_compress_fast_extState_fastReset() :
 * A variant of LZ4_compress_fast_extState().
 *
 * Using this variant avoids an expensive initialization step. It is only safe
 * to call if the state buffer is known to be correctly initialized already
 * (see comment in lz4.h on LZ4_resetStream_fast() for a definition of
 * "correctly initialized").
 */
int LZ4_compress_fast_extState_fastReset(void* state, const char* src, char* dst, int srcSize, int dstCapacity, int acceleration)
{
    AOCL_SETUP_NATIVE();
    LZ4_stream_t_internal* ctx = &((LZ4_stream_t*)state)->internal_donotuse;
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;

    if (dstCapacity >= LZ4_compressBound(srcSize)) {
        if (srcSize < LZ4_64Klimit) {
            const tableType_t tableType = byU16;
            LZ4_prepareTable(ctx, srcSize, tableType);
            if (ctx->currentOffset) {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, dictSmall, acceleration);
            } else {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
            }
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            LZ4_prepareTable(ctx, srcSize, tableType);
            return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    } else {
        if (srcSize < LZ4_64Klimit) {
            const tableType_t tableType = byU16;
            LZ4_prepareTable(ctx, srcSize, tableType);
            if (ctx->currentOffset) {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, dictSmall, acceleration);
            } else {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, noDictIssue, acceleration);
            }
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            LZ4_prepareTable(ctx, srcSize, tableType);
            return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}

#ifdef AOCL_ENABLE_THREADS
int LZ4_compress_fast_ST(const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    int result;

#if (LZ4_HEAPMODE)
    LZ4_stream_t* ctxPtr = ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
    if (ctxPtr == NULL) return 0;
#else
    LZ4_stream_t ctx;
    LZ4_stream_t* const ctxPtr = &ctx;
#endif
#ifdef AOCL_LZ4_OPT
    result = LZ4_compress_fast_extState_fp(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
#else
    result = LZ4_compress_fast_extState(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
#endif

#if (LZ4_HEAPMODE)
    FREEMEM(ctxPtr);
#endif

    return result;
}

#ifdef AOCL_LZ4_AVX_OPT
/* This function does not use any AVX code, but it produces output with RAP frame added.
* This data is not compatible with the single threaded decompress APIs. Hence, it is placed under
* AOCL_LZ4_AVX_OPT and made to pair with AOCL_LZ4_decompress_safe_mt().
*/
int AOCL_LZ4_compress_fast_mt(const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration){
    if ((source == NULL && inputSize != 0) || dest == NULL)
        return 0;
    
    int result;
    aocl_thread_group_t thread_group_handle;
    aocl_thread_info_t cur_thread_info;
    aocl_thread_info_t prev_thread_info;
    AOCL_INT32 rap_metadata_len = -1;
    AOCL_UINT32 thread_cnt = 0;
    AOCL_UINT32 dst_offset = 0;
    
    rap_metadata_len = aocl_setup_parallel_compress_mt(&thread_group_handle, (char *)source,
                                                 dest, inputSize, maxOutputSize,
                                                 LZ4_COMPRESS_INPLACE_MARGIN,
                                                 WINDOW_FACTOR);
    if (rap_metadata_len < 0)
        return 0;
 
    if (thread_group_handle.num_threads == 1)
    {
        return LZ4_compress_fast_ST(source, dest, inputSize, maxOutputSize, acceleration);
    }
    else
    {
#ifdef AOCL_THREADS_LOG
        printf("Compress Thread [id: %d] : Before parallel region\n", omp_get_thread_num());
#endif

#pragma omp parallel private(cur_thread_info) shared(thread_group_handle) num_threads(thread_group_handle.num_threads)
        {
#ifdef AOCL_THREADS_LOG
            printf("Compress Thread [id: %d] : Inside parallel region\n", omp_get_thread_num());
#endif
            AOCL_UCHAR *last_anchor_ptr = NULL;
            AOCL_UINT32 cmpr_bound_pad = ((thread_group_handle.common_part_src_size + 
                                        thread_group_handle.leftover_part_src_bytes) / 255) + 
                                        16 + rap_metadata_len;
            AOCL_UINT32 is_error = 1;
            AOCL_UINT32 thread_id = omp_get_thread_num();
            AOCL_INT32 local_result = -1;
            AOCL_UINT32 last_bytes_len = 0;

            if (aocl_do_partition_compress_mt(&thread_group_handle, &cur_thread_info, cmpr_bound_pad, thread_id) == 0)
            {

#if (LZ4_HEAPMODE)
                LZ4_stream_t* ctxPtr = ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
                if (ctxPtr == NULL) return 0;
#else
                LZ4_stream_t ctx;
                LZ4_stream_t* const ctxPtr = &ctx;
#endif
                local_result = AOCL_LZ4_compress_fast_extState_mt(ctxPtr,
                    cur_thread_info.partition_src, cur_thread_info.dst_trap,
                    cur_thread_info.partition_src_size, 
                    cur_thread_info.dst_trap_size, acceleration, 
                    &last_anchor_ptr, (thread_id != (thread_group_handle.num_threads - 1)) ? &last_bytes_len : NULL);
#if (LZ4_HEAPMODE)
                FREEMEM(ctxPtr);
#endif
                is_error = 0;
            }//aocl_do_partition_compress_mt
            
            thread_group_handle.threads_info_list[thread_id].partition_src = cur_thread_info.partition_src;
            thread_group_handle.threads_info_list[thread_id].dst_trap = cur_thread_info.dst_trap;
            thread_group_handle.threads_info_list[thread_id].additional_state_info = (AOCL_VOID *)last_anchor_ptr;
            thread_group_handle.threads_info_list[thread_id].dst_trap_size = local_result;
            thread_group_handle.threads_info_list[thread_id].partition_src_size = cur_thread_info.partition_src_size;
            thread_group_handle.threads_info_list[thread_id].last_bytes_len = last_bytes_len;
            thread_group_handle.threads_info_list[thread_id].is_error = is_error;
            thread_group_handle.threads_info_list[thread_id].num_child_threads = 0;
#ifdef AOCL_THREADS_LOG
            //printf("Compress Thread [id: %d] : Compression output length [%d], original source length [%d]\n",
            //                                                  omp_get_thread_num(), local_result, inputSize);
#endif
        }//#pragma omp parallel
#ifdef AOCL_THREADS_LOG
        printf("Compress Thread [id: %d] : After parallel region\n", omp_get_thread_num());
#endif

        //Post processing in single-threaded mode: Prepares RAP frame and joins the last sequences of the neighboring threads
        
        // <-- RAP Header -->
        //Add at the start of the stream : Although it can be at the end or at any other point in the stream, but it is more easier for parsing at the start
        AOCL_CHAR* dst_org = thread_group_handle.dst;
        AOCL_CHAR* dst_ptr = dst_org;
        AOCL_UINT32 prev_offset, prev_len;
        AOCL_UINT32 decomp_len;
        thread_group_handle.dst += rap_metadata_len;
        dst_ptr += RAP_START_OF_PARTITIONS;
        // <-- RAP Header -->

        // <-- RAP Metadata payload -->
        //For the first thread:
        prev_thread_info = thread_group_handle.threads_info_list[thread_cnt++];
        //In case of any thread partitioning or alloc errors, exit the compression process with error
        if (prev_thread_info.is_error || prev_thread_info.dst_trap_size < 0)
        {
            result = 0;
            aocl_destroy_parallel_compress_mt(&thread_group_handle);
#ifdef AOCL_THREADS_LOG
            printf("Compress Thread [id: %d] : Encountered ERROR\n", thread_cnt-1);
#endif
            return result;
        }
        //Copy first chunk as it is to the output final buffer
        memcpy((thread_group_handle.dst + dst_offset), prev_thread_info.dst_trap, prev_thread_info.dst_trap_size);
        *(AOCL_UINT32*)dst_ptr = rap_metadata_len; //For storing this thread's RAP offset
        dst_ptr += RAP_OFFSET_BYTES;
        *(AOCL_INT32*)dst_ptr = prev_thread_info.dst_trap_size; //For storing this thread's RAP length
        dst_ptr += RAP_LEN_BYTES;
        //For storing this thread's decompressed (src) length
        decomp_len = prev_thread_info.partition_src_size - prev_thread_info.last_bytes_len;
        if (((AOCL_UCHAR *)prev_thread_info.additional_state_info - (AOCL_UCHAR*)prev_thread_info.partition_src) !=
            decomp_len)
        {
#ifdef AOCL_THREADS_LOG
            printf("Compress Thread [id: %d] : Error in last bytes position\n", thread_cnt);
#endif
            result = 0;
            aocl_destroy_parallel_compress_mt(&thread_group_handle);
            return result;
        }
        *(AOCL_INT32*)dst_ptr = decomp_len;
        dst_ptr += DECOMP_LEN_BYTES;
        thread_group_handle.dst += prev_thread_info.dst_trap_size;

        prev_offset = rap_metadata_len;
        prev_len = prev_thread_info.dst_trap_size;

        //For next threads:
        for (; thread_cnt < thread_group_handle.num_threads; thread_cnt++)
        {
            size_t cur_token, new_token, cur_lit;
            cur_thread_info = thread_group_handle.threads_info_list[thread_cnt];
            //In case of any thread partitioning or alloc errors, exit the compression process with error
            if (cur_thread_info.is_error || cur_thread_info.dst_trap_size < 0)
            {
                result = 0;
                aocl_destroy_parallel_compress_mt(&thread_group_handle);
#ifdef AOCL_THREADS_LOG
                printf("Compress Thread [id: %d] : Encountered ERROR\n", thread_cnt);
#endif
                return result;
            }
            dst_offset = 0;
            //thread_group_handle.dst += dst_offset;

            //post processing to join parallely decodable chunks into a contiguous stream to allow
            //standard decoder to process it in ST mode as well
            //If cur thread's dst_trap_size = 0 (all literals), then write it to output final buffer
            //along with the previous chunk's left over bytes (literals)
            if (cur_thread_info.dst_trap_size == 0 && cur_thread_info.last_bytes_len)
            {
                cur_thread_info.last_bytes_len = cur_thread_info.last_bytes_len + prev_thread_info.last_bytes_len;
                cur_thread_info.additional_state_info = prev_thread_info.additional_state_info;
                *(AOCL_UINT32*)dst_ptr = (prev_offset + prev_len); //For storing this thread's RAP offset
                *(AOCL_INT32*)(dst_ptr + RAP_OFFSET_BYTES) = dst_offset; //For storing this thread's RAP length
                dst_ptr += RAP_DATA_BYTES;
                //For storing this thread's decompressed (src) length
                decomp_len = 0;
                *(AOCL_INT32*)dst_ptr = decomp_len;
                dst_ptr += DECOMP_LEN_BYTES;
                prev_thread_info = cur_thread_info;
                prev_offset = (prev_offset + prev_len);
                prev_len = dst_offset;
            }
            else //Normal situation when cur thread's dst_trap_size > 0
            {
                cur_token = *(AOCL_UCHAR*)cur_thread_info.dst_trap;
                cur_thread_info.dst_trap++;
                cur_thread_info.dst_trap_size--;
                cur_lit = (cur_token >> 4);
                new_token = cur_lit + prev_thread_info.last_bytes_len;
                if (new_token >= RUN_MASK)
                {
                    size_t accumulator = new_token - RUN_MASK;
                    *thread_group_handle.dst++ = (BYTE)((RUN_MASK << ML_BITS) | (cur_token & 0xF));
                    dst_offset++;
                    for (; accumulator >= 255; accumulator -= 255)
                    {
                        *thread_group_handle.dst++ = (BYTE)255;
                        dst_offset++;
                    }
                    if (cur_lit >= RUN_MASK)
                    {
                        while (*(AOCL_UCHAR*)cur_thread_info.dst_trap == 255)
                        {
                            *thread_group_handle.dst++ = (BYTE)255;
                            dst_offset++;
                            cur_thread_info.dst_trap++;
                            cur_thread_info.dst_trap_size--;
                        }
                        new_token = *(AOCL_UCHAR*)cur_thread_info.dst_trap;
                        cur_thread_info.dst_trap++;
                        cur_thread_info.dst_trap_size--;
                        accumulator += new_token;
                        if (accumulator >= 255)
                        {
                            *thread_group_handle.dst++ = (BYTE)255;
                            dst_offset++;
                            accumulator -= 255;
                        }
                    }
                    *thread_group_handle.dst++ = (BYTE)accumulator;
                    dst_offset++;
                }
                else
                {
                    *thread_group_handle.dst++ = (BYTE)((new_token << ML_BITS) | (cur_token & 0xF));
                    dst_offset++;
                }

                //Copy prev thread's last literal bytes to the output final buffer
                memcpy(thread_group_handle.dst, prev_thread_info.additional_state_info, prev_thread_info.last_bytes_len);
                dst_offset += prev_thread_info.last_bytes_len;
                thread_group_handle.dst += prev_thread_info.last_bytes_len;

                //Copy this thread's chunk to the output final buffer
                memcpy(thread_group_handle.dst, cur_thread_info.dst_trap, cur_thread_info.dst_trap_size);
                dst_offset += cur_thread_info.dst_trap_size;
                thread_group_handle.dst += cur_thread_info.dst_trap_size;

                *(AOCL_UINT32*)dst_ptr = (prev_offset + prev_len); //For storing this thread's RAP offset
                *(AOCL_INT32*)(dst_ptr + RAP_OFFSET_BYTES) = dst_offset; //For storing this thread's RAP length
                dst_ptr += RAP_DATA_BYTES;
                //For storing this thread's decompressed (src) length
                decomp_len = cur_thread_info.partition_src_size - cur_thread_info.last_bytes_len;
                if ((thread_cnt != (thread_group_handle.num_threads - 1)) &&
                    ((AOCL_UCHAR*)cur_thread_info.additional_state_info - (AOCL_UCHAR*)cur_thread_info.partition_src) !=
                    decomp_len)
                {
#ifdef AOCL_THREADS_LOG
                    printf("Compress Thread [id: %d] : Error in last bytes position\n", thread_cnt);
#endif
                    result = 0;
                    aocl_destroy_parallel_compress_mt(&thread_group_handle);
                    return result;
                }
                *(AOCL_INT32*)dst_ptr = decomp_len + prev_thread_info.last_bytes_len;
                dst_ptr += DECOMP_LEN_BYTES;

                prev_thread_info = cur_thread_info;
                prev_offset = (prev_offset + prev_len);
                prev_len = dst_offset;
            }
        }
        // <-- RAP Metadata payload -->

        result = thread_group_handle.dst - dest;
        aocl_destroy_parallel_compress_mt(&thread_group_handle);
    }//thread_group_handle.num_threads > 1
    return result;
}
#endif /* AOCL_LZ4_AVX_OPT */

int AOCL_LZ4_compress_fast_st(const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration){
    int result = 0;
    #if (LZ4_HEAPMODE)
        LZ4_stream_t* ctxPtr = ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
        if (ctxPtr == NULL) return 0;
    #else
        LZ4_stream_t ctx;
        LZ4_stream_t* const ctxPtr = &ctx;
    #endif
    #ifdef AOCL_LZ4_OPT
        result = LZ4_compress_fast_extState_fp(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
    #else
        result = LZ4_compress_fast_extState(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
    #endif

    #if (LZ4_HEAPMODE)
        FREEMEM(ctxPtr);
    #endif
        return result;
}

static int (*LZ4_compress_fast_mt_fp)(const char* source, char* dest, int inputSize, 
    int maxOutputSize, int acceleration) = AOCL_LZ4_compress_fast_st;
#endif

int LZ4_compress_fast(const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    AOCL_SETUP_NATIVE();
    int result;

#ifdef AOCL_ENABLE_THREADS
    result = LZ4_compress_fast_mt_fp(source, dest, inputSize, maxOutputSize, acceleration);
    return result;
#else
#if (LZ4_HEAPMODE)
    LZ4_stream_t* ctxPtr = ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
    if (ctxPtr == NULL) return 0;
#else
    LZ4_stream_t ctx;
    LZ4_stream_t* const ctxPtr = &ctx;
#endif
#ifdef AOCL_LZ4_OPT
    result = LZ4_compress_fast_extState_fp(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
#else
    result = LZ4_compress_fast_extState(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
#endif

#if (LZ4_HEAPMODE)
    FREEMEM(ctxPtr);
#endif
    return result;
    
#endif /* AOCL_ENABLE_THREADS */
}

int LZ4_compress_default(const char* src, char* dst, int srcSize, int maxOutputSize)
{
    LOG_UNFORMATTED(TRACE, logCtx, "Enter");

    int ret = LZ4_compress_fast(src, dst, srcSize, maxOutputSize, 1);

    LOG_UNFORMATTED(INFO, logCtx, "Exit");
    return ret;
}


/* Note!: This function leaves the stream in an unclean/broken state!
 * It is not safe to subsequently use the same state with a _fastReset() or
 * _continue() call without resetting it. */
static int LZ4_compress_destSize_extState (LZ4_stream_t* state, const char* src, char* dst, int* srcSizePtr, int targetDstSize)
{
    AOCL_SETUP_NATIVE();
    if(state==NULL || src==NULL || dst==NULL || srcSizePtr==NULL)
        return 0;
    
    void* const s = LZ4_initStream(state, sizeof (*state));
    assert(s != NULL); (void)s;

    if (targetDstSize >= LZ4_compressBound(*srcSizePtr)) {  /* compression success is guaranteed */
        return LZ4_compress_fast_extState(state, src, dst, *srcSizePtr, targetDstSize, 1);
    } else {
        if (*srcSizePtr < LZ4_64Klimit) {
            return LZ4_compress_generic(&state->internal_donotuse, src, dst, *srcSizePtr, srcSizePtr, targetDstSize, fillOutput, byU16, noDict, noDictIssue, 1);
        } else {
            tableType_t const addrMode = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(&state->internal_donotuse, src, dst, *srcSizePtr, srcSizePtr, targetDstSize, fillOutput, addrMode, noDict, noDictIssue, 1);
    }   }
}


int LZ4_compress_destSize(const char* src, char* dst, int* srcSizePtr, int targetDstSize)
{
    AOCL_SETUP_NATIVE();
#if (LZ4_HEAPMODE)
    LZ4_stream_t* ctx = (LZ4_stream_t*)ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
    if (ctx == NULL) return 0;
#else
    LZ4_stream_t ctxBody;
    LZ4_stream_t* ctx = &ctxBody;
#endif

    int result = LZ4_compress_destSize_extState(ctx, src, dst, srcSizePtr, targetDstSize);

#if (LZ4_HEAPMODE)
    FREEMEM(ctx);
#endif
    return result;
}



/*-******************************
*  Streaming functions
********************************/

LZ4_stream_t* LZ4_createStream(void)
{
    LZ4_stream_t* const lz4s = (LZ4_stream_t*)ALLOC(sizeof(LZ4_stream_t));
    LZ4_STATIC_ASSERT(LZ4_STREAMSIZE >= sizeof(LZ4_stream_t_internal));    /* A compilation error here means LZ4_STREAMSIZE is not large enough */
    DEBUGLOG(4, "LZ4_createStream %p", lz4s);
    if (lz4s == NULL) return NULL;
    LZ4_initStream(lz4s, sizeof(*lz4s));
    return lz4s;
}

static size_t LZ4_stream_t_alignment(void)
{
#if LZ4_ALIGN_TEST
    typedef struct { char c; LZ4_stream_t t; } t_a;
    return sizeof(t_a) - sizeof(LZ4_stream_t);
#else
    return 1;  /* effectively disabled */
#endif
}

LZ4_stream_t* LZ4_initStream (void* buffer, size_t size)
{
    DEBUGLOG(5, "LZ4_initStream");
    if (buffer == NULL) { return NULL; }
    if (size < sizeof(LZ4_stream_t)) { return NULL; }
    if (!LZ4_isAligned(buffer, LZ4_stream_t_alignment())) return NULL;
    MEM_INIT(buffer, 0, sizeof(LZ4_stream_t_internal));
    return (LZ4_stream_t*)buffer;
}

/* resetStream is now deprecated,
 * prefer initStream() which is more general */
void LZ4_resetStream (LZ4_stream_t* LZ4_stream)
{
    DEBUGLOG(5, "LZ4_resetStream (ctx:%p)", LZ4_stream);
    MEM_INIT(LZ4_stream, 0, sizeof(LZ4_stream_t_internal));
}

void LZ4_resetStream_fast(LZ4_stream_t* ctx) 
{
    if(ctx==NULL)
        return;

    LZ4_prepareTable(&(ctx->internal_donotuse), 0, byU32);
}

int LZ4_freeStream (LZ4_stream_t* LZ4_stream)
{
    if (!LZ4_stream) return 0;   /* support free on NULL */
    DEBUGLOG(5, "LZ4_freeStream %p", LZ4_stream);
    FREEMEM(LZ4_stream);
    return (0);
}


#define HASH_UNIT sizeof(reg_t)
int LZ4_loadDict (LZ4_stream_t* LZ4_dict, const char* dictionary, int dictSize)
{
    if(LZ4_dict==NULL)
        return -1;

    LZ4_stream_t_internal* dict = &LZ4_dict->internal_donotuse;
    const tableType_t tableType = byU32;
    const BYTE* p = (const BYTE*)dictionary;
    const BYTE* const dictEnd = p + dictSize;
    const BYTE* base;

    DEBUGLOG(4, "LZ4_loadDict (%i bytes from %p into %p)", dictSize, dictionary, LZ4_dict);

    /* It's necessary to reset the context,
     * and not just continue it with prepareTable()
     * to avoid any risk of generating overflowing matchIndex
     * when compressing using this dictionary */
    LZ4_resetStream(LZ4_dict);

    /* We always increment the offset by 64 KB, since, if the dict is longer,
     * we truncate it to the last 64k, and if it's shorter, we still want to
     * advance by a whole window length so we can provide the guarantee that
     * there are only valid offsets in the window, which allows an optimization
     * in LZ4_compress_fast_continue() where it uses noDictIssue even when the
     * dictionary isn't a full 64k. */
    dict->currentOffset += 64 KB;

    if (dictSize < (int)HASH_UNIT) {
        return 0;
    }
    if (dictionary == NULL){
        return -1;
    }

    if ((dictEnd - p) > 64 KB) p = dictEnd - 64 KB;
    base = dictEnd - dict->currentOffset;
    dict->dictionary = p;
    dict->dictSize = (U32)(dictEnd - p);
    dict->tableType = (U32)tableType;

    while (p <= dictEnd-HASH_UNIT) {
        LZ4_putPosition(p, dict->hashTable, tableType, base);
        p+=3;
    }

    return (int)dict->dictSize;
}

void LZ4_attach_dictionary(LZ4_stream_t* workingStream, const LZ4_stream_t* dictionaryStream) {
    const LZ4_stream_t_internal* dictCtx = dictionaryStream == NULL ? NULL :
        &(dictionaryStream->internal_donotuse);

    DEBUGLOG(4, "LZ4_attach_dictionary (%p, %p, size %u)",
             workingStream, dictionaryStream,
             dictCtx != NULL ? dictCtx->dictSize : 0);

    if (dictCtx != NULL) {
        /* If the current offset is zero, we will never look in the
         * external dictionary context, since there is no value a table
         * entry can take that indicate a miss. In that case, we need
         * to bump the offset to something non-zero.
         */
        if (workingStream->internal_donotuse.currentOffset == 0) {
            workingStream->internal_donotuse.currentOffset = 64 KB;
        }

        /* Don't actually attach an empty dictionary.
         */
        if (dictCtx->dictSize == 0) {
            dictCtx = NULL;
        }
    }
    workingStream->internal_donotuse.dictCtx = dictCtx;
}


static void LZ4_renormDictT(LZ4_stream_t_internal* LZ4_dict, int nextSize)
{
    assert(nextSize >= 0);
    if (LZ4_dict->currentOffset + (unsigned)nextSize > 0x80000000) {   /* potential ptrdiff_t overflow (32-bits mode) */
        /* rescale hash table */
        U32 const delta = LZ4_dict->currentOffset - 64 KB;
        const BYTE* dictEnd = LZ4_dict->dictionary + LZ4_dict->dictSize;
        int i;
        DEBUGLOG(4, "LZ4_renormDictT");
        for (i=0; i<LZ4_HASH_SIZE_U32; i++) {
            if (LZ4_dict->hashTable[i] < delta) LZ4_dict->hashTable[i]=0;
            else LZ4_dict->hashTable[i] -= delta;
        }
        LZ4_dict->currentOffset = 64 KB;
        if (LZ4_dict->dictSize > 64 KB) LZ4_dict->dictSize = 64 KB;
        LZ4_dict->dictionary = dictEnd - LZ4_dict->dictSize;
    }
}


int LZ4_compress_fast_continue (LZ4_stream_t* LZ4_stream,
                                const char* source, char* dest,
                                int inputSize, int maxOutputSize,
                                int acceleration)
{
    AOCL_SETUP_NATIVE();
    if(LZ4_stream==NULL || source==NULL || dest==NULL)
        return 0;
        
    const tableType_t tableType = byU32;
    LZ4_stream_t_internal* streamPtr = &LZ4_stream->internal_donotuse;
    const BYTE* dictEnd = streamPtr->dictionary + streamPtr->dictSize;

    DEBUGLOG(5, "LZ4_compress_fast_continue (inputSize=%i)", inputSize);

    LZ4_renormDictT(streamPtr, inputSize);   /* avoid index overflow */
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;

    /* invalidate tiny dictionaries */
    if ( (streamPtr->dictSize-1 < 4-1)   /* intentional underflow */
      && (dictEnd != (const BYTE*)source) ) {
        DEBUGLOG(5, "LZ4_compress_fast_continue: dictSize(%u) at addr:%p is too small", streamPtr->dictSize, streamPtr->dictionary);
        streamPtr->dictSize = 0;
        streamPtr->dictionary = (const BYTE*)source;
        dictEnd = (const BYTE*)source;
    }

    /* Check overlapping input/dictionary space */
    {   const BYTE* sourceEnd = (const BYTE*) source + inputSize;
        if ((sourceEnd > streamPtr->dictionary) && (sourceEnd < dictEnd)) {
            streamPtr->dictSize = (U32)(dictEnd - sourceEnd);
            if (streamPtr->dictSize > 64 KB) streamPtr->dictSize = 64 KB;
            if (streamPtr->dictSize < 4) streamPtr->dictSize = 0;
            streamPtr->dictionary = dictEnd - streamPtr->dictSize;
        }
    }

    /* prefix mode : source data follows dictionary */
    if (dictEnd == (const BYTE*)source) {
        if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset))
            return LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, withPrefix64k, dictSmall, acceleration);
        else
            return LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, withPrefix64k, noDictIssue, acceleration);
    }

    /* external dictionary mode */
    {   int result;
        if (streamPtr->dictCtx) {
            /* We depend here on the fact that dictCtx'es (produced by
             * LZ4_loadDict) guarantee that their tables contain no references
             * to offsets between dictCtx->currentOffset - 64 KB and
             * dictCtx->currentOffset - dictCtx->dictSize. This makes it safe
             * to use noDictIssue even when the dict isn't a full 64 KB.
             */
            if (inputSize > 4 KB) {
                /* For compressing large blobs, it is faster to pay the setup
                 * cost to copy the dictionary's tables into the active context,
                 * so that the compression loop is only looking into one table.
                 */
                LZ4_memcpy(streamPtr, streamPtr->dictCtx, sizeof(*streamPtr));
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, noDictIssue, acceleration);
            } else {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingDictCtx, noDictIssue, acceleration);
            }
        } else {
            if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset)) {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, dictSmall, acceleration);
            } else {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, noDictIssue, acceleration);
            }
        }
        streamPtr->dictionary = (const BYTE*)source;
        streamPtr->dictSize = (U32)inputSize;
        return result;
    }
}


/* Hidden debug function, to force-test external dictionary mode */
int LZ4_compress_forceExtDict (LZ4_stream_t* LZ4_dict, const char* source, char* dest, int srcSize)
{
    AOCL_SETUP_NATIVE();
    LZ4_stream_t_internal* streamPtr = &LZ4_dict->internal_donotuse;
    int result;

    LZ4_renormDictT(streamPtr, srcSize);

    if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset)) {
        result = LZ4_compress_generic(streamPtr, source, dest, srcSize, NULL, 0, notLimited, byU32, usingExtDict, dictSmall, 1);
    } else {
        result = LZ4_compress_generic(streamPtr, source, dest, srcSize, NULL, 0, notLimited, byU32, usingExtDict, noDictIssue, 1);
    }

    streamPtr->dictionary = (const BYTE*)source;
    streamPtr->dictSize = (U32)srcSize;

    return result;
}


/*! LZ4_saveDict() :
 *  If previously compressed data block is not guaranteed to remain available at its memory location,
 *  save it into a safer place (char* safeBuffer).
 *  Note : you don't need to call LZ4_loadDict() afterwards,
 *         dictionary is immediately usable, you can therefore call LZ4_compress_fast_continue().
 *  Return : saved dictionary size in bytes (necessarily <= dictSize), or 0 if error.
 */
int LZ4_saveDict (LZ4_stream_t* LZ4_dict, char* safeBuffer, int dictSize)
{
    if(LZ4_dict == NULL || (safeBuffer == NULL && dictSize != 0))
        return 0;
        
    LZ4_stream_t_internal* const dict = &LZ4_dict->internal_donotuse;
    const BYTE* const previousDictEnd = dict->dictionary + dict->dictSize;

    if ((U32)dictSize > 64 KB) { dictSize = 64 KB; } /* useless to define a dictionary > 64 KB */
    if ((U32)dictSize > dict->dictSize) { dictSize = (int)dict->dictSize; }

    if (safeBuffer == NULL) assert(dictSize == 0);
    if (dictSize > 0)
        memmove(safeBuffer, previousDictEnd - dictSize, dictSize);

    dict->dictionary = (const BYTE*)safeBuffer;
    dict->dictSize = (U32)dictSize;

    return dictSize;
}



/*-*******************************
 *  Decompression functions
 ********************************/

typedef enum { endOnOutputSize = 0, endOnInputSize = 1 } endCondition_directive;
typedef enum { decode_full_block = 0, partial_decode = 1 } earlyEnd_directive;


#undef MIN
#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )

/* Read the variable-length literal or match length.
 *
 * ip - pointer to use as input.
 * lencheck - end ip.  Return an error if ip advances >= lencheck.
 * loop_check - check ip >= lencheck in body of loop.  Returns loop_error if so.
 * initial_check - check ip >= lencheck before start of loop.  Returns initial_error if so.
 * error (output) - error code.  Should be set to 0 before call.
 */
typedef enum { loop_error = -2, initial_error = -1, ok = 0 } variable_length_error;
LZ4_FORCE_INLINE unsigned
read_variable_length(const BYTE**ip, const BYTE* lencheck,
                     int loop_check, int initial_check,
                     variable_length_error* error)
{
    U32 length = 0;
    U32 s;
    if (initial_check && unlikely((*ip) >= lencheck)) {    /* overflow detection */
        *error = initial_error;
        return length;
    }
    do {
        s = **ip;
        (*ip)++;
        length += s;
        if (loop_check && unlikely((*ip) >= lencheck)) {    /* overflow detection */
            *error = loop_error;
            return length;
        }
    } while (s==255);

    return length;
}

#ifdef AOCL_LZ4_AVX_OPT
/*! AOCL_LZ4_decompress_generic() :
 *  This generic decompression function covers all use cases.
 *  It shall be instantiated several times, using different sets of directives.
 *  Note that it is important for performance that this function really get inlined,
 *  in order to remove useless branches during compilation optimization.
 *  
 *  Same as LZ4_decompress_generic(), but calls `AOCL_LZ4_wildCopy64_AVX()` instead 
 *  of `LZ4_wildCopy32()` to copy
 *  - literals (when endCondition_directive is endOnInput and literal length >= 15), 
 *  - matched characters (when offset>=32)
 */
LZ4_FORCE_INLINE int
AOCL_LZ4_decompress_generic(
                 const char* const src,
                 char* const dst,
                 int srcSize,
                 int outputSize,         /* If endOnInput==endOnInputSize, this value is `dstCapacity` */

                 endCondition_directive endOnInput,   /* endOnOutputSize, endOnInputSize */
                 earlyEnd_directive partialDecoding,  /* full, partial */
                 dict_directive dict,                 /* noDict, withPrefix64k, usingExtDict */
                 const BYTE* const lowPrefix,  /* always <= dst, == dst when no prefix */
                 const BYTE* const dictStart,  /* only if dict==usingExtDict */
                 const size_t dictSize         /* note : = 0 if noDict */
                 )
{
    if (src == NULL || dst == NULL)
        return -1;

    {   const BYTE* ip = (const BYTE*) src;
        const BYTE* const iend = ip + srcSize;

        BYTE* op = (BYTE*) dst;
        BYTE* const oend = op + outputSize;
        BYTE* cpy;

        const BYTE* const dictEnd = (dictStart == NULL) ? NULL : dictStart + dictSize;

        const int safeDecode = (endOnInput==endOnInputSize);
        const int checkOffset = ((safeDecode) && (dictSize < (int)(64 KB)));


        /* Set up the "end" pointers for the shortcut. */
        const BYTE* const shortiend = iend - (endOnInput ? 14 : 8) /*maxLL*/ - 2 /*offset*/;
        const BYTE* const shortoend = oend - (endOnInput ? 14 : 8) /*maxLL*/ - 18 /*maxML*/;

        const BYTE* match;
        size_t offset;
        unsigned token;
        size_t length;


        DEBUGLOG(5, "AOCL_LZ4_decompress_generic (srcSize:%i, dstSize:%i)", srcSize, outputSize);

        /* Special cases */
        assert(lowPrefix <= op);
        if ((endOnInput) && (unlikely(outputSize==0))) {
            /* Empty output buffer */
            if (partialDecoding) return 0;
            return ((srcSize==1) && (*ip==0)) ? 0 : -1;
        }
        if ((!endOnInput) && (unlikely(outputSize==0))) { return (*ip==0 ? 1 : -1); }
        if ((endOnInput) && unlikely(srcSize==0)) { return -1; }

        /* Currently the fast loop shows a regression on qualcomm arm chips. */
#if LZ4_FAST_DEC_LOOP
        if ((oend - op) < FASTLOOP_SAFE_DISTANCE) {
            DEBUGLOG(6, "skip fast decode loop");
            goto safe_decode;
        }

        /* Fast loop : decode sequences as long as output < iend-FASTLOOP_SAFE_DISTANCE */
        while (1) {
            /* Main fastloop assertion: We can always wildcopy FASTLOOP_SAFE_DISTANCE */
            assert(oend - op >= FASTLOOP_SAFE_DISTANCE);
            if (endOnInput) { assert(ip < iend); }
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

            /* decode literal length */
            if (length == RUN_MASK) {
                variable_length_error error = ok;
                length += read_variable_length(&ip, iend-RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
                if (error == initial_error) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if ((safeDecode) && unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */

                /* copy literals */
                cpy = op+length;
                LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
                if (endOnInput) {  /* LZ4_decompress_safe() */
                    if ((cpy>oend-64) || (ip+length>iend-64)) { goto safe_literal_copy; }
                    AOCL_LZ4_wildCopy64_AVX(op, ip, cpy);
                } else {   /* LZ4_decompress_fast() */
                    if (cpy>oend-8) { goto safe_literal_copy; }
                    LZ4_wildCopy8(op, ip, cpy); /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                                                 * it doesn't know input length, and only relies on end-of-block properties */
                }
                ip += length; op = cpy;
            } else {
                cpy = op+length;
                if (endOnInput) {  /* LZ4_decompress_safe() */
                    DEBUGLOG(7, "copy %u bytes in a 16-bytes stripe", (unsigned)length);
                    /* We don't need to check oend, since we check it once for each loop below */
                    if (ip > iend-(16 + 1/*max lit + offset + nextToken*/)) { goto safe_literal_copy; }
                    /* Literals can only be 14, but hope compilers optimize if we copy by a register size */
                    LZ4_memcpy(op, ip, 16);
                } else {  /* LZ4_decompress_fast() */
                    /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                     * it doesn't know input length, and relies on end-of-block properties */
                    LZ4_memcpy(op, ip, 8);
                    if (length > 8) { LZ4_memcpy(op+8, ip+8, 8); }
                }
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;
            assert(match <= op);

            /* get matchlength */
            length = token & ML_MASK;

            if (length == ML_MASK) {
                variable_length_error error = ok;
                if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
                length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
                if (error != ok) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)op)) { goto _output_error; } /* overflow detection */
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }
            } else {
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }

                /* Fastpath check: Avoids a branch in LZ4_wildCopy32 if true */
                if ((dict == withPrefix64k) || (match >= lowPrefix)) {
                    if (offset >= 8) {
                        assert(match >= lowPrefix);
                        assert(match <= op);
                        assert(op + 18 <= oend);

                        LZ4_memcpy(op, match, 8);
                        LZ4_memcpy(op+8, match+8, 8);
                        LZ4_memcpy(op+16, match+16, 2);
                        op += length;
                        continue;
            }   }   }

            if (checkOffset && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) {
                        DEBUGLOG(7, "partialDecoding: dictionary match, close to dstEnd");
                        length = MIN(length, (size_t)(oend-op));
                    } else {
                        goto _output_error;  /* end-of-block condition violated */
                }   }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) { *op++ = *copyFrom++; }
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }

            /* copy match within block */
            cpy = op + length;

            assert((op <= oend) && (oend-op >= 32));

            if(offset >= 32){
                AOCL_LZ4_wildCopy64_AVX(op, match, cpy);
                op = cpy;
                continue;
            }

            if (unlikely(offset<16)) {
                LZ4_memcpy_using_offset(op, match, cpy, offset);
            } else {
                LZ4_wildCopy32(op, match, cpy);
            }

            op = cpy;   /* wildcopy correction */
        }
    safe_decode:
#endif

        /* Main Loop : decode remaining sequences where output < FASTLOOP_SAFE_DISTANCE */
        while (1) {
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

            /* A two-stage shortcut for the most common case:
             * 1) If the literal length is 0..14, and there is enough space,
             * enter the shortcut and copy 16 bytes on behalf of the literals
             * (in the fast mode, only 8 bytes can be safely copied this way).
             * 2) Further if the match length is 4..18, copy 18 bytes in a similar
             * manner; but we ensure that there's enough space in the output for
             * those 18 bytes earlier, upon entering the shortcut (in other words,
             * there is a combined check for both stages).
             */
            if ( (endOnInput ? length != RUN_MASK : length <= 8)
                /* strictly "less than" on input, to re-enter the loop with at least one byte */
              && likely((endOnInput ? ip < shortiend : 1) & (op <= shortoend)) ) {
                /* Copy the literals */
                LZ4_memcpy(op, ip, endOnInput ? 16 : 8);
                op += length; ip += length;

                /* The second stage: prepare for match copying, decode full info.
                 * If it doesn't work out, the info won't be wasted. */
                length = token & ML_MASK; /* match length */
                offset = LZ4_readLE16(ip); ip += 2;
                match = op - offset;
                assert(match <= op); /* check overflow */

                /* Do not deal with overlapping matches. */
                if ( (length != ML_MASK)
                  && (offset >= 8)
                  && (dict==withPrefix64k || match >= lowPrefix) ) {
                    /* Copy the match. */
                    LZ4_memcpy(op + 0, match + 0, 8);
                    LZ4_memcpy(op + 8, match + 8, 8);
                    LZ4_memcpy(op +16, match +16, 2);
                    op += length + MINMATCH;
                    /* Both stages worked, load the next token. */
                    continue;
                }

                /* The second stage didn't work out, but the info is ready.
                 * Propel it right to the point of match copying. */
                goto _copy_match;
            }

            /* decode literal length */
            if (length == RUN_MASK) {
                variable_length_error error = ok;
                length += read_variable_length(&ip, iend-RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
                if (error == initial_error) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if ((safeDecode) && unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */
            }

            /* copy literals */
            cpy = op+length;
#if LZ4_FAST_DEC_LOOP
        safe_literal_copy:
#endif
            LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
            if ( ((endOnInput) && ((cpy>oend-MFLIMIT) || (ip+length>iend-(2+1+LASTLITERALS))) )
              || ((!endOnInput) && (cpy>oend-WILDCOPYLENGTH)) )
            {
                /* We've either hit the input parsing restriction or the output parsing restriction.
                 * In the normal scenario, decoding a full block, it must be the last sequence,
                 * otherwise it's an error (invalid input or dimensions).
                 * In partialDecoding scenario, it's necessary to ensure there is no buffer overflow.
                 */
                if (partialDecoding) {
                    /* Since we are partial decoding we may be in this block because of the output parsing
                     * restriction, which is not valid since the output buffer is allowed to be undersized.
                     */
                    assert(endOnInput);
                    DEBUGLOG(7, "partialDecoding: copying literals, close to input or output end")
                    DEBUGLOG(7, "partialDecoding: literal length = %u", (unsigned)length);
                    DEBUGLOG(7, "partialDecoding: remaining space in dstBuffer : %i", (int)(oend - op));
                    DEBUGLOG(7, "partialDecoding: remaining space in srcBuffer : %i", (int)(iend - ip));
                    /* Finishing in the middle of a literals segment,
                     * due to lack of input.
                     */
                    if (ip+length > iend) {
                        length = (size_t)(iend-ip);
                        cpy = op + length;
                    }
                    /* Finishing in the middle of a literals segment,
                     * due to lack of output space.
                     */
                    if (cpy > oend) {
                        cpy = oend;
                        assert(op<=oend);
                        length = (size_t)(oend-op);
                    }
                } else {
                    /* We must be on the last sequence because of the parsing limitations so check
                     * that we exactly regenerate the original size (must be exact when !endOnInput).
                     */
                    if ((!endOnInput) && (cpy != oend)) { goto _output_error; }
                     /* We must be on the last sequence (or invalid) because of the parsing limitations
                      * so check that we exactly consume the input and don't overrun the output buffer.
                      */
                    if ((endOnInput) && ((ip+length != iend) || (cpy > oend))) {
                        DEBUGLOG(6, "should have been last run of literals")
                        DEBUGLOG(6, "ip(%p) + length(%i) = %p != iend (%p)", ip, (int)length, ip+length, iend);
                        DEBUGLOG(6, "or cpy(%p) > oend(%p)", cpy, oend);
                        goto _output_error;
                    }
                }
                memmove(op, ip, length);  /* supports overlapping memory regions; only matters for in-place decompression scenarios */
                ip += length;
                op += length;
                /* Necessarily EOF when !partialDecoding.
                 * When partialDecoding, it is EOF if we've either
                 * filled the output buffer or
                 * can't proceed with reading an offset for following match.
                 */
                if (!partialDecoding || (cpy == oend) || (ip >= (iend-2))) {
                    break;
                }
            } else {
                LZ4_wildCopy8(op, ip, cpy);   /* may overwrite up to WILDCOPYLENGTH beyond cpy */
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;

            /* get matchlength */
            length = token & ML_MASK;

    _copy_match:
            if (length == ML_MASK) {
              variable_length_error error = ok;
              length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
              if (error != ok) goto _output_error;
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)op)) goto _output_error;   /* overflow detection */
            }
            length += MINMATCH;

#if LZ4_FAST_DEC_LOOP
        safe_match_copy:
#endif
            if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) goto _output_error;   /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) length = MIN(length, (size_t)(oend-op));
                    else goto _output_error;   /* doesn't respect parsing restriction */
                }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) *op++ = *copyFrom++;
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }
            assert(match >= lowPrefix);

            /* copy match within block */
            cpy = op + length;

            /* partialDecoding : may end anywhere within the block */
            assert(op<=oend);
            if (partialDecoding && (cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                size_t const mlen = MIN(length, (size_t)(oend-op));
                const BYTE* const matchEnd = match + mlen;
                BYTE* const copyEnd = op + mlen;
                if (matchEnd > op) {   /* overlap copy */
                    while (op < copyEnd) { *op++ = *match++; }
                } else {
                    LZ4_memcpy(op, match, mlen);
                }
                op = copyEnd;
                if (op == oend) { break; }
                continue;
            }

            if (unlikely(offset<8)) {
                LZ4_write32(op, 0);   /* silence msan warning when offset==0 */
                op[0] = match[0];
                op[1] = match[1];
                op[2] = match[2];
                op[3] = match[3];
                match += inc32table[offset];
                LZ4_memcpy(op+4, match, 4);
                match -= dec64table[offset];
            } else {
                LZ4_memcpy(op, match, 8);
                match += 8;
            }
            op += 8;

            if (unlikely(cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                BYTE* const oCopyLimit = oend - (WILDCOPYLENGTH-1);
                if (cpy > oend-LASTLITERALS) { goto _output_error; } /* Error : last LASTLITERALS bytes must be literals (uncompressed) */
                if (op < oCopyLimit) {
                    LZ4_wildCopy8(op, match, oCopyLimit);
                    match += oCopyLimit - op;
                    op = oCopyLimit;
                }
                while (op < cpy) { *op++ = *match++; }
            } else {
                LZ4_memcpy(op, match, 8);
                if (length > 16)  { LZ4_wildCopy8(op+8, match+8, cpy); }
            }
            op = cpy;   /* wildcopy correction */
        }

        /* end of decoding */
        if (endOnInput) {
            DEBUGLOG(5, "decoded %i bytes", (int) (((char*)op)-dst));
           return (int) (((char*)op)-dst);     /* Nb of output bytes decoded */
       } else {
           return (int) (((const char*)ip)-src);   /* Nb of input bytes read */
       }

        /* Overflow error detected */
    _output_error:
        return (int) (-(((const char*)ip)-src))-1;
    }
}

#ifdef AOCL_ENABLE_THREADS
//Same as AOCL_LZ4_decompress_generic, but with multi-threaded support
LZ4_FORCE_INLINE int
AOCL_LZ4_decompress_generic_mt(
    const char* const src,
    char* const dst,
    int srcSize,
    int outputSize,         /* If endOnInput==endOnInputSize, this value is `dstCapacity` */

    endCondition_directive endOnInput,   /* endOnOutputSize, endOnInputSize */
    earlyEnd_directive partialDecoding,  /* full, partial */
    dict_directive dict,                 /* noDict, withPrefix64k, usingExtDict */
    const BYTE* const lowPrefix,  /* always <= dst, == dst when no prefix */
    const BYTE* const dictStart,  /* only if dict==usingExtDict */
    const size_t dictSize,         /* note : = 0 if noDict */
    int is_last_thread)
{
    if (src == NULL || dst == NULL)
        return -1;

    {   const BYTE* ip = (const BYTE*)src;
    const BYTE* const iend = ip + srcSize;

    BYTE* op = (BYTE*)dst;
    BYTE* const oend = op + outputSize;
    BYTE* cpy;

    const BYTE* const dictEnd = (dictStart == NULL) ? NULL : dictStart + dictSize;

    const int safeDecode = (endOnInput == endOnInputSize);
    const int checkOffset = ((safeDecode) && (dictSize < (int)(64 KB)));


    /* Set up the "end" pointers for the shortcut. */
    const BYTE* const shortiend = iend - (endOnInput ? 14 : 8) /*maxLL*/ - 2 /*offset*/;
    const BYTE* const shortoend = oend - (endOnInput ? 14 : 8) /*maxLL*/ - 18 /*maxML*/;

    const BYTE* match;
    size_t offset;
    unsigned token;
    size_t length;


    DEBUGLOG(5, "AOCL_LZ4_decompress_generic_mt (srcSize:%i, dstSize:%i)", srcSize, outputSize);

    /* Special cases */
    assert(lowPrefix <= op);
    if ((endOnInput) && (unlikely(outputSize == 0))) {
        /* Empty output buffer */
        if (partialDecoding) return 0;
        return ((srcSize == 1) && (*ip == 0)) ? 0 : -1;
    }
    if ((!endOnInput) && (unlikely(outputSize == 0))) { return (*ip == 0 ? 1 : -1); }
    if ((endOnInput) && unlikely(srcSize == 0)) { return -1; }

    /* Currently the fast loop shows a regression on qualcomm arm chips. */
#if LZ4_FAST_DEC_LOOP
    if ((oend - op) < FASTLOOP_SAFE_DISTANCE) {
        DEBUGLOG(6, "skip fast decode loop");
        goto safe_decode;
    }

    /* Fast loop : decode sequences as long as output < iend-FASTLOOP_SAFE_DISTANCE */
    while (1) {
        /* Main fastloop assertion: We can always wildcopy FASTLOOP_SAFE_DISTANCE */
        assert(oend - op >= FASTLOOP_SAFE_DISTANCE);
        if (endOnInput) { assert(ip < iend); }
        token = *ip++;
        length = token >> ML_BITS;  /* literal length */

        assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

        /* decode literal length */
        if (length == RUN_MASK) {
            variable_length_error error = ok;
            length += read_variable_length(&ip, iend - RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
            if (error == initial_error) {
                goto _output_error;
            }
            if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)(op))) {
                goto _output_error;
            } /* overflow detection */
            if ((safeDecode) && unlikely((uptrval)(ip)+length < (uptrval)(ip))) {
                goto _output_error;
            } /* overflow detection */

            /* copy literals */
            cpy = op + length;
            LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
            if (endOnInput) {  /* LZ4_decompress_safe() */
                if ((cpy > oend - 64) || (ip + length > iend - 64)) { goto safe_literal_copy; }
                AOCL_LZ4_wildCopy64_AVX(op, ip, cpy);
            }
            else {   /* LZ4_decompress_fast() */
                if (cpy > oend - 8) { goto safe_literal_copy; }
                LZ4_wildCopy8(op, ip, cpy); /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                                             * it doesn't know input length, and only relies on end-of-block properties */
            }
            ip += length; op = cpy;
        }
        else {
            cpy = op + length;
            if (endOnInput) {  /* LZ4_decompress_safe() */
                DEBUGLOG(7, "copy %u bytes in a 16-bytes stripe", (unsigned)length);
                /* We don't need to check oend, since we check it once for each loop below */
                if (ip > iend - (16 + 1/*max lit + offset + nextToken*/)) { goto safe_literal_copy; }
                /* Literals can only be 14, but hope compilers optimize if we copy by a register size */
                LZ4_memcpy(op, ip, 16);
            }
            else {  /* LZ4_decompress_fast() */
             /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
              * it doesn't know input length, and relies on end-of-block properties */
                LZ4_memcpy(op, ip, 8);
                if (length > 8) { LZ4_memcpy(op + 8, ip + 8, 8); }
            }
            ip += length; op = cpy;
        }

        /* get offset */
        offset = LZ4_readLE16(ip); ip += 2;
        match = op - offset;
        assert(match <= op);

        /* get matchlength */
        length = token & ML_MASK;

        if (length == ML_MASK) {
            variable_length_error error = ok;
            if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) {
                goto _output_error;
            } /* Error : offset outside buffers */
            if (is_last_thread) {
                length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
            } else {
                length += read_variable_length(&ip, iend + 1, (int)endOnInput, 0, &error);
            }
            if (error != ok) {
                goto _output_error;
            }
            if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)op)) {
                goto _output_error;
            } /* overflow detection */
            length += MINMATCH;
            if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                goto safe_match_copy;
            }
        }
        else {
            length += MINMATCH;
            if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                goto safe_match_copy;
            }

            /* Fastpath check: Avoids a branch in LZ4_wildCopy32 if true */
            if ((dict == withPrefix64k) || (match >= lowPrefix)) {
                if (offset >= 8) {
                    assert(match >= lowPrefix);
                    assert(match <= op);
                    assert(op + 18 <= oend);

                    LZ4_memcpy(op, match, 8);
                    LZ4_memcpy(op + 8, match + 8, 8);
                    LZ4_memcpy(op + 16, match + 16, 2);
                    op += length;
                    continue;
                }
            }
        }

        if (checkOffset && (unlikely(match + dictSize < lowPrefix))) {
            goto _output_error;
        } /* Error : offset outside buffers */
        /* match starting within external dictionary */
        if ((dict == usingExtDict) && (match < lowPrefix)) {
            if (unlikely(op + length > oend - LASTLITERALS)) {
                if (partialDecoding) {
                    DEBUGLOG(7, "partialDecoding: dictionary match, close to dstEnd");
                    length = MIN(length, (size_t)(oend - op));
                }
                else {
                    goto _output_error;  /* end-of-block condition violated */
                }
            }

            if (length <= (size_t)(lowPrefix - match)) {
                /* match fits entirely within external dictionary : just copy */
                memmove(op, dictEnd - (lowPrefix - match), length);
                op += length;
            }
            else {
                /* match stretches into both external dictionary and current block */
                size_t const copySize = (size_t)(lowPrefix - match);
                size_t const restSize = length - copySize;
                LZ4_memcpy(op, dictEnd - copySize, copySize);
                op += copySize;
                if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                    BYTE* const endOfMatch = op + restSize;
                    const BYTE* copyFrom = lowPrefix;
                    while (op < endOfMatch) { *op++ = *copyFrom++; }
                }
                else {
                    LZ4_memcpy(op, lowPrefix, restSize);
                    op += restSize;
                }
            }
            continue;
        }

        /* copy match within block */
        cpy = op + length;

        assert((op <= oend) && (oend - op >= 32));

        if (offset >= 32) {
            AOCL_LZ4_wildCopy64_AVX(op, match, cpy);
            op = cpy;
            continue;
        }

        if (unlikely(offset < 16)) {
            LZ4_memcpy_using_offset(op, match, cpy, offset);
        }
        else {
            LZ4_wildCopy32(op, match, cpy);
        }

        op = cpy;   /* wildcopy correction */
    }
safe_decode:
#endif

    /* Main Loop : decode remaining sequences where output < FASTLOOP_SAFE_DISTANCE */
    while (1) {
        token = *ip++;
        length = token >> ML_BITS;  /* literal length */

        assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

        /* A two-stage shortcut for the most common case:
         * 1) If the literal length is 0..14, and there is enough space,
         * enter the shortcut and copy 16 bytes on behalf of the literals
         * (in the fast mode, only 8 bytes can be safely copied this way).
         * 2) Further if the match length is 4..18, copy 18 bytes in a similar
         * manner; but we ensure that there's enough space in the output for
         * those 18 bytes earlier, upon entering the shortcut (in other words,
         * there is a combined check for both stages).
         */
        if ((endOnInput ? length != RUN_MASK : length <= 8)
            /* strictly "less than" on input, to re-enter the loop with at least one byte */
            && likely((endOnInput ? ip < shortiend : 1) & (op <= shortoend))) {
            /* Copy the literals */
            LZ4_memcpy(op, ip, endOnInput ? 16 : 8);
            op += length; ip += length;

            /* The second stage: prepare for match copying, decode full info.
             * If it doesn't work out, the info won't be wasted. */
            length = token & ML_MASK; /* match length */
            offset = LZ4_readLE16(ip); ip += 2;
            match = op - offset;
            assert(match <= op); /* check overflow */

            /* Do not deal with overlapping matches. */
            if ((length != ML_MASK)
                && (offset >= 8)
                && (dict == withPrefix64k || match >= lowPrefix)) {
                /* Copy the match. */
                LZ4_memcpy(op + 0, match + 0, 8);
                LZ4_memcpy(op + 8, match + 8, 8);
                LZ4_memcpy(op + 16, match + 16, 2);
                op += length + MINMATCH;
                /* Both stages worked, load the next token. */
                continue;
            }

            /* The second stage didn't work out, but the info is ready.
             * Propel it right to the point of match copying. */
            goto _copy_match;
        }

        /* decode literal length */
        if (length == RUN_MASK) {
            variable_length_error error = ok;
            length += read_variable_length(&ip, iend - RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
            if (error == initial_error) {
                goto _output_error;
            }
            if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)(op))) {
                goto _output_error;
            } /* overflow detection */
            if ((safeDecode) && unlikely((uptrval)(ip)+length < (uptrval)(ip))) {
                goto _output_error;
            } /* overflow detection */
        }

        /* copy literals */
        cpy = op + length;
#if LZ4_FAST_DEC_LOOP
        safe_literal_copy :
#endif
        LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
        if (((endOnInput) && ((cpy > oend - MFLIMIT) || (ip + length > iend - (2 + 1 + LASTLITERALS))))
            || ((!endOnInput) && (cpy > oend - WILDCOPYLENGTH)))
        {
            /* We've either hit the input parsing restriction or the output parsing restriction.
             * In the normal scenario, decoding a full block, it must be the last sequence,
             * otherwise it's an error (invalid input or dimensions).
             * In partialDecoding scenario, it's necessary to ensure there is no buffer overflow.
             */
            if (partialDecoding) {
                /* Since we are partial decoding we may be in this block because of the output parsing
                 * restriction, which is not valid since the output buffer is allowed to be undersized.
                 */
                assert(endOnInput);
                DEBUGLOG(7, "partialDecoding: copying literals, close to input or output end")
                    DEBUGLOG(7, "partialDecoding: literal length = %u", (unsigned)length);
                DEBUGLOG(7, "partialDecoding: remaining space in dstBuffer : %i", (int)(oend - op));
                DEBUGLOG(7, "partialDecoding: remaining space in srcBuffer : %i", (int)(iend - ip));
                /* Finishing in the middle of a literals segment,
                 * due to lack of input.
                 */
                if (ip + length > iend) {
                    length = (size_t)(iend - ip);
                    cpy = op + length;
                }
                /* Finishing in the middle of a literals segment,
                 * due to lack of output space.
                 */
                if (cpy > oend) {
                    cpy = oend;
                    assert(op <= oend);
                    length = (size_t)(oend - op);
                }
            }
            else {
                /* We must be on the last sequence because of the parsing limitations so check
                 * that we exactly regenerate the original size (must be exact when !endOnInput).
                 */
                if ((!endOnInput) && (cpy != oend)) {
                    goto _output_error;
                }
                /* We must be on the last sequence (or invalid) because of the parsing limitations
                 * so check that we exactly consume the input and don't overrun the output buffer.
                 */
                if ((endOnInput) && ((is_last_thread && (ip + length != iend)) || (cpy > oend))) {
                    DEBUGLOG(6, "should have been last run of literals")
                        DEBUGLOG(6, "ip(%p) + length(%i) = %p != iend (%p)", ip, (int)length, ip + length, iend);
                    DEBUGLOG(6, "or cpy(%p) > oend(%p)", cpy, oend);
                    goto _output_error;
                }
            }
            memmove(op, ip, length);  /* supports overlapping memory regions; only matters for in-place decompression scenarios */
            ip += length;
            op += length;
            /* Necessarily EOF when !partialDecoding.
             * When partialDecoding, it is EOF if we've either
             * filled the output buffer or
             * can't proceed with reading an offset for following match.
             */
            if ((is_last_thread && !partialDecoding) || (cpy == oend) || (is_last_thread && (ip >= (iend - 2)))) {
                break;
            }
        }
        else {
            LZ4_wildCopy8(op, ip, cpy);   /* may overwrite up to WILDCOPYLENGTH beyond cpy */
            ip += length; op = cpy;
        }

        /* get offset */
        offset = LZ4_readLE16(ip); ip += 2;
        match = op - offset;

        /* get matchlength */
        length = token & ML_MASK;

    _copy_match:
        if (length == ML_MASK) {
            variable_length_error error = ok;
            if(is_last_thread) {
                length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
            } else {
                length += read_variable_length(&ip, iend + 1, (int)endOnInput, 0, &error);
            }
            if (error != ok)
                goto _output_error;
            if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)op))
                goto _output_error;   /* overflow detection */
        }
        length += MINMATCH;

#if LZ4_FAST_DEC_LOOP
        safe_match_copy :
#endif
        if ((checkOffset) && (unlikely(match + dictSize < lowPrefix)))
            goto _output_error;   /* Error : offset outside buffers */
        /* match starting within external dictionary */
        if ((dict == usingExtDict) && (match < lowPrefix)) {
            if (unlikely(op + length > oend - LASTLITERALS)) {
                if (partialDecoding) length = MIN(length, (size_t)(oend - op));
                else
                    goto _output_error;   /* doesn't respect parsing restriction */
            }

            if (length <= (size_t)(lowPrefix - match)) {
                /* match fits entirely within external dictionary : just copy */
                memmove(op, dictEnd - (lowPrefix - match), length);
                op += length;
            }
            else {
                /* match stretches into both external dictionary and current block */
                size_t const copySize = (size_t)(lowPrefix - match);
                size_t const restSize = length - copySize;
                LZ4_memcpy(op, dictEnd - copySize, copySize);
                op += copySize;
                if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                    BYTE* const endOfMatch = op + restSize;
                    const BYTE* copyFrom = lowPrefix;
                    while (op < endOfMatch) *op++ = *copyFrom++;
                }
                else {
                    LZ4_memcpy(op, lowPrefix, restSize);
                    op += restSize;
                }
            }
            continue;
        }
        assert(match >= lowPrefix);

        /* copy match within block */
        cpy = op + length;

        /* partialDecoding : may end anywhere within the block */
        assert(op <= oend);
        if (partialDecoding && (cpy > oend - MATCH_SAFEGUARD_DISTANCE)) {
            size_t const mlen = MIN(length, (size_t)(oend - op));
            const BYTE* const matchEnd = match + mlen;
            BYTE* const copyEnd = op + mlen;
            if (matchEnd > op) {   /* overlap copy */
                while (op < copyEnd) { *op++ = *match++; }
            }
            else {
                LZ4_memcpy(op, match, mlen);
            }
            op = copyEnd;
            if (op == oend) { break; }
            continue;
        }

        if (unlikely(offset < 8)) {
            LZ4_write32(op, 0);   /* silence msan warning when offset==0 */
            op[0] = match[0];
            op[1] = match[1];
            op[2] = match[2];
            op[3] = match[3];
            match += inc32table[offset];
            LZ4_memcpy(op + 4, match, 4);
            match -= dec64table[offset];
        }
        else {
            LZ4_memcpy(op, match, 8);
            match += 8;
        }
        op += 8;

        if (unlikely(cpy > oend - MATCH_SAFEGUARD_DISTANCE)) {
            BYTE* const oCopyLimit = oend - (WILDCOPYLENGTH - 1);
            if (cpy > oend - LASTLITERALS) {
                goto _output_error;
            } /* Error : last LASTLITERALS bytes must be literals (uncompressed) */
            if (op < oCopyLimit) {
                LZ4_wildCopy8(op, match, oCopyLimit);
                match += oCopyLimit - op;
                op = oCopyLimit;
            }
            while (op < cpy) { *op++ = *match++; }
        }
        else {
            LZ4_memcpy(op, match, 8);
            if (length > 16) { LZ4_wildCopy8(op + 8, match + 8, cpy); }
        }
        op = cpy;   /* wildcopy correction */

        if ((cpy == oend) || (ip >= iend))
        {
            break;
        }
    }

    /* end of decoding */
    if (endOnInput) {
        DEBUGLOG(5, "decoded %i bytes", (int)(((char*)op) - dst));
        return (int)(((char*)op) - dst);     /* Nb of output bytes decoded */
    }
    else {
        return (int)(((const char*)ip) - src);   /* Nb of input bytes read */
    }

    /* Overflow error detected */
    _output_error:
        return (int) (-(((const char*)ip)-src))-1;
    }
}
#endif /* AOCL_ENABLE_THREADS */
#endif /* AOCL_LZ4_OPT */

/*! LZ4_decompress_generic() :
 *  This generic decompression function covers all use cases.
 *  It shall be instantiated several times, using different sets of directives.
 *  Note that it is important for performance that this function really get inlined,
 *  in order to remove useless branches during compilation optimization.
 */
LZ4_FORCE_INLINE int
LZ4_decompress_generic(
                 const char* const src,
                 char* const dst,
                 int srcSize,
                 int outputSize,         /* If endOnInput==endOnInputSize, this value is `dstCapacity` */

                 endCondition_directive endOnInput,   /* endOnOutputSize, endOnInputSize */
                 earlyEnd_directive partialDecoding,  /* full, partial */
                 dict_directive dict,                 /* noDict, withPrefix64k, usingExtDict */
                 const BYTE* const lowPrefix,  /* always <= dst, == dst when no prefix */
                 const BYTE* const dictStart,  /* only if dict==usingExtDict */
                 const size_t dictSize         /* note : = 0 if noDict */
                 )
{
    if (src == NULL || dst == NULL)
        return -1;

    {   const BYTE* ip = (const BYTE*) src;
        const BYTE* const iend = ip + srcSize;

        BYTE* op = (BYTE*) dst;
        BYTE* const oend = op + outputSize;
        BYTE* cpy;

        const BYTE* const dictEnd = (dictStart == NULL) ? NULL : dictStart + dictSize;

        const int safeDecode = (endOnInput==endOnInputSize);
        const int checkOffset = ((safeDecode) && (dictSize < (int)(64 KB)));


        /* Set up the "end" pointers for the shortcut. */
        const BYTE* const shortiend = iend - (endOnInput ? 14 : 8) /*maxLL*/ - 2 /*offset*/;
        const BYTE* const shortoend = oend - (endOnInput ? 14 : 8) /*maxLL*/ - 18 /*maxML*/;

        const BYTE* match;
        size_t offset;
        unsigned token;
        size_t length;


        DEBUGLOG(5, "LZ4_decompress_generic (srcSize:%i, dstSize:%i)", srcSize, outputSize);

        /* Special cases */
        assert(lowPrefix <= op);
        if ((endOnInput) && (unlikely(outputSize==0))) {
            /* Empty output buffer */
            if (partialDecoding) return 0;
            return ((srcSize==1) && (*ip==0)) ? 0 : -1;
        }
        if ((!endOnInput) && (unlikely(outputSize==0))) { return (*ip==0 ? 1 : -1); }
        if ((endOnInput) && unlikely(srcSize==0)) { return -1; }

	/* Currently the fast loop shows a regression on qualcomm arm chips. */
#if LZ4_FAST_DEC_LOOP
        if ((oend - op) < FASTLOOP_SAFE_DISTANCE) {
            DEBUGLOG(6, "skip fast decode loop");
            goto safe_decode;
        }

        /* Fast loop : decode sequences as long as output < iend-FASTLOOP_SAFE_DISTANCE */
        while (1) {
            /* Main fastloop assertion: We can always wildcopy FASTLOOP_SAFE_DISTANCE */
            assert(oend - op >= FASTLOOP_SAFE_DISTANCE);
            if (endOnInput) { assert(ip < iend); }
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

            /* decode literal length */
            if (length == RUN_MASK) {
                variable_length_error error = ok;
                length += read_variable_length(&ip, iend-RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
                if (error == initial_error) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if ((safeDecode) && unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */

                /* copy literals */
                cpy = op+length;
                LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
                if (endOnInput) {  /* LZ4_decompress_safe() */
                    if ((cpy>oend-32) || (ip+length>iend-32)) { goto safe_literal_copy; }
                    LZ4_wildCopy32(op, ip, cpy);
                } else {   /* LZ4_decompress_fast() */
                    if (cpy>oend-8) { goto safe_literal_copy; }
                    LZ4_wildCopy8(op, ip, cpy); /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                                                 * it doesn't know input length, and only relies on end-of-block properties */
                }
                ip += length; op = cpy;
            } else {
                cpy = op+length;
                if (endOnInput) {  /* LZ4_decompress_safe() */
                    DEBUGLOG(7, "copy %u bytes in a 16-bytes stripe", (unsigned)length);
                    /* We don't need to check oend, since we check it once for each loop below */
                    if (ip > iend-(16 + 1/*max lit + offset + nextToken*/)) { goto safe_literal_copy; }
                    /* Literals can only be 14, but hope compilers optimize if we copy by a register size */
                    LZ4_memcpy(op, ip, 16);
                } else {  /* LZ4_decompress_fast() */
                    /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                     * it doesn't know input length, and relies on end-of-block properties */
                    LZ4_memcpy(op, ip, 8);
                    if (length > 8) { LZ4_memcpy(op+8, ip+8, 8); }
                }
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;
            assert(match <= op);

            /* get matchlength */
            length = token & ML_MASK;

            if (length == ML_MASK) {
                variable_length_error error = ok;
                if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
                length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
                if (error != ok) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)op)) { goto _output_error; } /* overflow detection */
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }
            } else {
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }

                /* Fastpath check: Avoids a branch in LZ4_wildCopy32 if true */
                if ((dict == withPrefix64k) || (match >= lowPrefix)) {
                    if (offset >= 8) {
                        assert(match >= lowPrefix);
                        assert(match <= op);
                        assert(op + 18 <= oend);

                        LZ4_memcpy(op, match, 8);
                        LZ4_memcpy(op+8, match+8, 8);
                        LZ4_memcpy(op+16, match+16, 2);
                        op += length;
                        continue;
            }   }   }

            if (checkOffset && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) {
                        DEBUGLOG(7, "partialDecoding: dictionary match, close to dstEnd");
                        length = MIN(length, (size_t)(oend-op));
                    } else {
                        goto _output_error;  /* end-of-block condition violated */
                }   }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) { *op++ = *copyFrom++; }
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }

            /* copy match within block */
            cpy = op + length;

            assert((op <= oend) && (oend-op >= 32));
            if (unlikely(offset<16)) {
                LZ4_memcpy_using_offset(op, match, cpy, offset);
            } else {
                LZ4_wildCopy32(op, match, cpy);
            }

            op = cpy;   /* wildcopy correction */
        }
    safe_decode:
#endif

        /* Main Loop : decode remaining sequences where output < FASTLOOP_SAFE_DISTANCE */
        while (1) {
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

            /* A two-stage shortcut for the most common case:
             * 1) If the literal length is 0..14, and there is enough space,
             * enter the shortcut and copy 16 bytes on behalf of the literals
             * (in the fast mode, only 8 bytes can be safely copied this way).
             * 2) Further if the match length is 4..18, copy 18 bytes in a similar
             * manner; but we ensure that there's enough space in the output for
             * those 18 bytes earlier, upon entering the shortcut (in other words,
             * there is a combined check for both stages).
             */
            if ( (endOnInput ? length != RUN_MASK : length <= 8)
                /* strictly "less than" on input, to re-enter the loop with at least one byte */
              && likely((endOnInput ? ip < shortiend : 1) & (op <= shortoend)) ) {
                /* Copy the literals */
                LZ4_memcpy(op, ip, endOnInput ? 16 : 8);
                op += length; ip += length;

                /* The second stage: prepare for match copying, decode full info.
                 * If it doesn't work out, the info won't be wasted. */
                length = token & ML_MASK; /* match length */
                offset = LZ4_readLE16(ip); ip += 2;
                match = op - offset;
                assert(match <= op); /* check overflow */

                /* Do not deal with overlapping matches. */
                if ( (length != ML_MASK)
                  && (offset >= 8)
                  && (dict==withPrefix64k || match >= lowPrefix) ) {
                    /* Copy the match. */
                    LZ4_memcpy(op + 0, match + 0, 8);
                    LZ4_memcpy(op + 8, match + 8, 8);
                    LZ4_memcpy(op +16, match +16, 2);
                    op += length + MINMATCH;
                    /* Both stages worked, load the next token. */
                    continue;
                }

                /* The second stage didn't work out, but the info is ready.
                 * Propel it right to the point of match copying. */
                goto _copy_match;
            }

            /* decode literal length */
            if (length == RUN_MASK) {
                variable_length_error error = ok;
                length += read_variable_length(&ip, iend-RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
                if (error == initial_error) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if ((safeDecode) && unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */
            }

            /* copy literals */
            cpy = op+length;
#if LZ4_FAST_DEC_LOOP
        safe_literal_copy:
#endif
            LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
            if ( ((endOnInput) && ((cpy>oend-MFLIMIT) || (ip+length>iend-(2+1+LASTLITERALS))) )
              || ((!endOnInput) && (cpy>oend-WILDCOPYLENGTH)) )
            {
                /* We've either hit the input parsing restriction or the output parsing restriction.
                 * In the normal scenario, decoding a full block, it must be the last sequence,
                 * otherwise it's an error (invalid input or dimensions).
                 * In partialDecoding scenario, it's necessary to ensure there is no buffer overflow.
                 */
                if (partialDecoding) {
                    /* Since we are partial decoding we may be in this block because of the output parsing
                     * restriction, which is not valid since the output buffer is allowed to be undersized.
                     */
                    assert(endOnInput);
                    DEBUGLOG(7, "partialDecoding: copying literals, close to input or output end")
                    DEBUGLOG(7, "partialDecoding: literal length = %u", (unsigned)length);
                    DEBUGLOG(7, "partialDecoding: remaining space in dstBuffer : %i", (int)(oend - op));
                    DEBUGLOG(7, "partialDecoding: remaining space in srcBuffer : %i", (int)(iend - ip));
                    /* Finishing in the middle of a literals segment,
                     * due to lack of input.
                     */
                    if (ip+length > iend) {
                        length = (size_t)(iend-ip);
                        cpy = op + length;
                    }
                    /* Finishing in the middle of a literals segment,
                     * due to lack of output space.
                     */
                    if (cpy > oend) {
                        cpy = oend;
                        assert(op<=oend);
                        length = (size_t)(oend-op);
                    }
                } else {
                    /* We must be on the last sequence because of the parsing limitations so check
                     * that we exactly regenerate the original size (must be exact when !endOnInput).
                     */
                    if ((!endOnInput) && (cpy != oend)) { goto _output_error; }
                     /* We must be on the last sequence (or invalid) because of the parsing limitations
                      * so check that we exactly consume the input and don't overrun the output buffer.
                      */
                    if ((endOnInput) && ((ip+length != iend) || (cpy > oend))) {
                        DEBUGLOG(6, "should have been last run of literals")
                        DEBUGLOG(6, "ip(%p) + length(%i) = %p != iend (%p)", ip, (int)length, ip+length, iend);
                        DEBUGLOG(6, "or cpy(%p) > oend(%p)", cpy, oend);
                        goto _output_error;
                    }
                }
                memmove(op, ip, length);  /* supports overlapping memory regions; only matters for in-place decompression scenarios */
                ip += length;
                op += length;
                /* Necessarily EOF when !partialDecoding.
                 * When partialDecoding, it is EOF if we've either
                 * filled the output buffer or
                 * can't proceed with reading an offset for following match.
                 */
                if (!partialDecoding || (cpy == oend) || (ip >= (iend-2))) {
                    break;
                }
            } else {
                LZ4_wildCopy8(op, ip, cpy);   /* may overwrite up to WILDCOPYLENGTH beyond cpy */
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;

            /* get matchlength */
            length = token & ML_MASK;

    _copy_match:
            if (length == ML_MASK) {
              variable_length_error error = ok;
              length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
              if (error != ok) goto _output_error;
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)op)) goto _output_error;   /* overflow detection */
            }
            length += MINMATCH;

#if LZ4_FAST_DEC_LOOP
        safe_match_copy:
#endif
            if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) goto _output_error;   /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) length = MIN(length, (size_t)(oend-op));
                    else goto _output_error;   /* doesn't respect parsing restriction */
                }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) *op++ = *copyFrom++;
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }
            assert(match >= lowPrefix);

            /* copy match within block */
            cpy = op + length;

            /* partialDecoding : may end anywhere within the block */
            assert(op<=oend);
            if (partialDecoding && (cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                size_t const mlen = MIN(length, (size_t)(oend-op));
                const BYTE* const matchEnd = match + mlen;
                BYTE* const copyEnd = op + mlen;
                if (matchEnd > op) {   /* overlap copy */
                    while (op < copyEnd) { *op++ = *match++; }
                } else {
                    LZ4_memcpy(op, match, mlen);
                }
                op = copyEnd;
                if (op == oend) { break; }
                continue;
            }

            if (unlikely(offset<8)) {
                LZ4_write32(op, 0);   /* silence msan warning when offset==0 */
                op[0] = match[0];
                op[1] = match[1];
                op[2] = match[2];
                op[3] = match[3];
                match += inc32table[offset];
                LZ4_memcpy(op+4, match, 4);
                match -= dec64table[offset];
            } else {
                LZ4_memcpy(op, match, 8);
                match += 8;
            }
            op += 8;

            if (unlikely(cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                BYTE* const oCopyLimit = oend - (WILDCOPYLENGTH-1);
                if (cpy > oend-LASTLITERALS) { goto _output_error; } /* Error : last LASTLITERALS bytes must be literals (uncompressed) */
                if (op < oCopyLimit) {
                    LZ4_wildCopy8(op, match, oCopyLimit);
                    match += oCopyLimit - op;
                    op = oCopyLimit;
                }
                while (op < cpy) { *op++ = *match++; }
            } else {
                LZ4_memcpy(op, match, 8);
                if (length > 16)  { LZ4_wildCopy8(op+8, match+8, cpy); }
            }
            op = cpy;   /* wildcopy correction */
        }

        /* end of decoding */
        if (endOnInput) {
            DEBUGLOG(5, "decoded %i bytes", (int) (((char*)op)-dst));
           return (int) (((char*)op)-dst);     /* Nb of output bytes decoded */
       } else {
           return (int) (((const char*)ip)-src);   /* Nb of input bytes read */
       }

        /* Overflow error detected */
    _output_error:
        return (int) (-(((const char*)ip)-src))-1;
    }
}


/*===== Instantiate the API decoding functions. =====*/

/* Wrapper function for static inlined LZ4_decompress_generic function for extending dynamic dispatcher support to decompression functions. */
LZ4_FORCE_O2
static int LZ4_decompress_wrapper(const char* source, char* dest, int compressedSize, int maxDecompressedSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
                                  endOnInputSize, decode_full_block, noDict,
                                  (BYTE*)dest, NULL, 0);
}

#ifdef AOCL_LZ4_AVX_OPT
/* Wrapper function for static inlined AOCL_LZ4_decompress_generic function for extending dynamic dispatcher support to decompression functions. */
LZ4_FORCE_O2
static int AOCL_LZ4_decompress_wrapper(const char* source, char* dest, int compressedSize, int maxDecompressedSize)
{
    return AOCL_LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
                                    endOnInputSize, decode_full_block, noDict,
                                    (BYTE*)dest, NULL, 0);
}
#endif

static int (*LZ4_decompress_wrapper_fp) (const char* source, char* dest, int compressedSize, int maxDecompressedSize) = LZ4_decompress_wrapper;

#ifdef AOCL_ENABLE_THREADS
LZ4_FORCE_O2
int LZ4_decompress_safe_ST(const char* source, char* dest, int compressedSize, int maxDecompressedSize)
{
#ifdef AOCL_LZ4_OPT
    return LZ4_decompress_wrapper_fp(source, dest, compressedSize, maxDecompressedSize);
#else
    return LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
                                  endOnInputSize, decode_full_block, noDict,
                                  (BYTE*)dest, NULL, 0);
#endif /* AOCL_LZ4_OPT */

}

#ifdef AOCL_LZ4_AVX_OPT
int AOCL_LZ4_decompress_safe_mt(const char* source, char* dest, int compressedSize, int maxDecompressedSize){
    LOG_UNFORMATTED(TRACE, logCtx, "Enter");
    if (source == NULL || dest == NULL)
    {
        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return -1;
    }
    
    int result;
    aocl_thread_group_t thread_group_handle;
    aocl_thread_info_t cur_thread_info;
    AOCL_INT32 ret_status = -1;
    AOCL_UINT32 thread_cnt = 0;
    AOCL_INT32 use_ST_decompressor = 0;

    ret_status = aocl_setup_parallel_decompress_mt(&thread_group_handle, (char *)source, dest,
                                                   compressedSize, maxDecompressedSize, use_ST_decompressor);
    if (ret_status < 0)
    {
        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return -1;
    }
    

    if (thread_group_handle.num_threads == 1 || use_ST_decompressor == 1)
    {
        result = LZ4_decompress_safe_ST(source + ret_status, dest, compressedSize - ret_status, maxDecompressedSize);
        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return result;
    }
    else
    {
#ifdef AOCL_THREADS_LOG
        printf("Decompress Thread [id: %d] : Before parallel region\n", omp_get_thread_num());
#endif
#pragma omp parallel private(cur_thread_info) shared(thread_group_handle) num_threads(thread_group_handle.num_threads)
        {
#ifdef AOCL_THREADS_LOG
            printf("Decompress Thread [id: %d] : Inside parallel region\n", omp_get_thread_num());
#endif
            AOCL_UINT32 cmpr_bound_pad = MATCH_SAFEGUARD_DISTANCE + MFLIMIT;
            AOCL_UINT32 is_error = 1;
            AOCL_UINT32 thread_id = omp_get_thread_num();
            AOCL_INT32 local_result = -1;
            AOCL_INT32 thread_parallel_res = 0;

            thread_parallel_res = aocl_do_partition_decompress_mt(&thread_group_handle, &cur_thread_info, cmpr_bound_pad, thread_id);
            if (thread_parallel_res == 0)
            {
                local_result = AOCL_LZ4_decompress_generic_mt(cur_thread_info.partition_src, cur_thread_info.dst_trap, 
                    cur_thread_info.partition_src_size, cur_thread_info.dst_trap_size,
                    endOnInputSize, decode_full_block, noDict,
                    (BYTE*)cur_thread_info.dst_trap, NULL, 0,
                    (thread_id == (thread_group_handle.num_threads - 1)) ? 1 : 0);

                is_error = 0;
            }//aocl_do_partition_decompress_mt
            else if (thread_parallel_res == 1)
            {
                local_result = 0;
                is_error = 0;
            }

            thread_group_handle.threads_info_list[thread_id].partition_src = cur_thread_info.partition_src;
            thread_group_handle.threads_info_list[thread_id].dst_trap = cur_thread_info.dst_trap;
            thread_group_handle.threads_info_list[thread_id].additional_state_info = NULL;
            thread_group_handle.threads_info_list[thread_id].dst_trap_size = local_result;
            thread_group_handle.threads_info_list[thread_id].partition_src_size = cur_thread_info.partition_src_size;
            thread_group_handle.threads_info_list[thread_id].last_bytes_len = 0;
            thread_group_handle.threads_info_list[thread_id].is_error = is_error;
            thread_group_handle.threads_info_list[thread_id].num_child_threads = 0;

        }//#pragma omp parallel
#ifdef AOCL_THREADS_LOG
        printf("Decompress Thread [id: %d] : After parallel region\n", omp_get_thread_num());
#endif

        //For all the threads: Write to a single output buffer in a single-threaded mode
        for (thread_cnt = 0; thread_cnt < thread_group_handle.num_threads; thread_cnt++)
        {
            cur_thread_info = thread_group_handle.threads_info_list[thread_cnt];
            //In case of any thread partitioning or alloc errors, exit the decompression process with error
            if (cur_thread_info.is_error)
            {
                result = -1;
                aocl_destroy_parallel_decompress_mt(&thread_group_handle);
#ifdef AOCL_THREADS_LOG
                printf("Decompress Thread [id: %d] : Encountered ERROR\n", thread_cnt);
#endif
                LOG_UNFORMATTED(INFO, logCtx, "Exit");
                return result;
            }

            //Copy this thread's chunk to the output final buffer
            memcpy(thread_group_handle.dst, cur_thread_info.dst_trap, cur_thread_info.dst_trap_size);
            thread_group_handle.dst += cur_thread_info.dst_trap_size;
        }

        result = thread_group_handle.dst - dest;

        aocl_destroy_parallel_decompress_mt(&thread_group_handle);

        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return result;
    }//thread_group_handle.num_threads > 1
}

static int (*LZ4_decompress_wrapper_mt_fp) (const char* source, char* dest, 
            int compressedSize, int maxDecompressedSize) = LZ4_decompress_wrapper;
#endif /* AOCL_LZ4_AVX_OPT */
#endif /* AOCL_ENABLE_THREADS */

LZ4_FORCE_O2
int LZ4_decompress_safe(const char* source, char* dest, int compressedSize, int maxDecompressedSize)
{
    int result = 0;
    LOG_UNFORMATTED(TRACE, logCtx, "Enter");

#ifdef AOCL_ENABLE_THREADS
#ifdef AOCL_LZ4_AVX_OPT
    result = LZ4_decompress_wrapper_mt_fp(source, dest, compressedSize, maxDecompressedSize);
#else
    result = LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
                                  endOnInputSize, decode_full_block, noDict,
                                  (BYTE*)dest, NULL, 0);
#endif
#else /* !AOCL_ENABLE_THREADS */
#ifdef AOCL_LZ4_OPT
    result = LZ4_decompress_wrapper_fp(source, dest, compressedSize, maxDecompressedSize);
#else
    result = LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
                                  endOnInputSize, decode_full_block, noDict,
                                  (BYTE*)dest, NULL, 0);
#endif /* AOCL_LZ4_OPT */
#endif /* AOCL_ENABLE_THREADS */

    LOG_UNFORMATTED(INFO, logCtx, "Exit");
    return result;
}

static void aocl_register_lz4_fmv(int optOff, int optLevel)
{
    if (optOff)
    {
        //C version
        LZ4_compress_fast_extState_fp = LZ4_compress_fast_extState;
        LZ4_decompress_wrapper_fp = LZ4_decompress_wrapper;
    }
    else
    {
        switch (optLevel)
        {
        case -1: // undecided. use defaults based on compiler flags
#ifdef AOCL_LZ4_AVX_OPT
            LZ4_compress_fast_extState_fp = AOCL_LZ4_compress_fast_extState;
            LZ4_decompress_wrapper_fp = AOCL_LZ4_decompress_wrapper;
#ifdef AOCL_ENABLE_THREADS
            LZ4_decompress_wrapper_mt_fp = AOCL_LZ4_decompress_safe_mt;
            LZ4_compress_fast_mt_fp = AOCL_LZ4_compress_fast_mt;
#endif
#elif defined(AOCL_LZ4_OPT)
            LZ4_compress_fast_extState_fp = AOCL_LZ4_compress_fast_extState;
            LZ4_decompress_wrapper_fp = LZ4_decompress_wrapper;
#ifdef AOCL_ENABLE_THREADS
            LZ4_compress_fast_mt_fp = AOCL_LZ4_compress_fast_st;
#endif
#else
            LZ4_compress_fast_extState_fp = LZ4_compress_fast_extState;
            LZ4_decompress_wrapper_fp = LZ4_decompress_wrapper;
#ifdef AOCL_ENABLE_THREADS
            LZ4_compress_fast_mt_fp = AOCL_LZ4_compress_fast_st;
#endif
#endif
            break;
#ifdef AOCL_LZ4_OPT
        case 0://C version
        case 1://SSE version
            LZ4_compress_fast_extState_fp = AOCL_LZ4_compress_fast_extState;
            LZ4_decompress_wrapper_fp = LZ4_decompress_wrapper;
#ifdef AOCL_ENABLE_THREADS
            LZ4_compress_fast_mt_fp = AOCL_LZ4_compress_fast_st;
#endif
            break;
        case 2://AVX version
        case 3://AVX2 version
        default://AVX512 and other versions
#ifdef AOCL_LZ4_AVX_OPT
            LZ4_compress_fast_extState_fp = AOCL_LZ4_compress_fast_extState;
            LZ4_decompress_wrapper_fp = AOCL_LZ4_decompress_wrapper;
#ifdef AOCL_ENABLE_THREADS
            LZ4_decompress_wrapper_mt_fp = AOCL_LZ4_decompress_safe_mt;
            LZ4_compress_fast_mt_fp = AOCL_LZ4_compress_fast_mt;
#endif
#else
            LZ4_compress_fast_extState_fp = AOCL_LZ4_compress_fast_extState;
            LZ4_decompress_wrapper_fp = LZ4_decompress_wrapper;
#ifdef AOCL_ENABLE_THREADS
            LZ4_compress_fast_mt_fp = AOCL_LZ4_compress_fast_st;
#endif
#endif
            break;
#else
        default:
            LZ4_compress_fast_extState_fp = LZ4_compress_fast_extState;
            LZ4_decompress_wrapper_fp = LZ4_decompress_wrapper;
#ifdef AOCL_ENABLE_THREADS
            LZ4_compress_fast_mt_fp = AOCL_LZ4_compress_fast_st;
#endif
            break;
#endif
        }
    }
}

char* aocl_setup_lz4(int optOff, int optLevel, size_t insize,
    size_t level, size_t windowLog)
{
    AOCL_ENTER_CRITICAL(setup_lz4)
    if (!setup_ok_lz4) {
        optOff = optOff ? 1 : get_disable_opt_flags(0);
        aocl_register_lz4_fmv(optOff, optLevel);
        setup_ok_lz4 = 1;
    }
    AOCL_EXIT_CRITICAL(setup_lz4)
    return NULL;
}

void aocl_destroy_lz4(void){
    AOCL_ENTER_CRITICAL(setup_lz4)
    setup_ok_lz4 = 0;
    AOCL_EXIT_CRITICAL(setup_lz4)
}

#ifdef AOCL_LZ4_OPT
static void aocl_setup_native(void) {
    AOCL_ENTER_CRITICAL(setup_lz4)
    if (!setup_ok_lz4) {
        int optLevel = get_cpu_opt_flags(0);
        int optOff = get_disable_opt_flags(0);
        aocl_register_lz4_fmv(optOff, optLevel);
        setup_ok_lz4 = 1;
    }
    AOCL_EXIT_CRITICAL(setup_lz4)
}
#endif /* AOCL_LZ4_OPT */

#ifdef AOCL_UNIT_TEST
#ifdef AOCL_LZ4_AVX_OPT
/* Wrapper function for static inlined AOCL_LZ4_wildCopy64_AVX function for unit testing. */
void Test_AOCL_LZ4_wildCopy64_AVX(void*dstPtr, const void* srcPtr, void*dstEnd)
{
    AOCL_LZ4_wildCopy64_AVX(dstPtr, srcPtr, dstEnd);
}
#endif /* AOCL_LZ4_AVX_OPT */
#endif /* AOCL_UNIT_TEST */

LZ4_FORCE_O2
int LZ4_decompress_safe_partial(const char* src, char* dst, int compressedSize, int targetOutputSize, int dstCapacity)
{
    AOCL_SETUP_NATIVE();
    dstCapacity = MIN(targetOutputSize, dstCapacity);
    return LZ4_decompress_generic(src, dst, compressedSize, dstCapacity,
                                  endOnInputSize, partial_decode,
                                  noDict, (BYTE*)dst, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_fast(const char* source, char* dest, int originalSize)
{
    AOCL_SETUP_NATIVE();
    return LZ4_decompress_generic(source, dest, 0, originalSize,
                                  endOnOutputSize, decode_full_block, withPrefix64k,
                                  (BYTE*)dest - 64 KB, NULL, 0);
}

/*===== Instantiate a few more decoding cases, used more than once. =====*/

LZ4_FORCE_O2 /* Exported, an obsolete API function. */
int LZ4_decompress_safe_withPrefix64k(const char* source, char* dest, int compressedSize, int maxOutputSize)
{
    AOCL_SETUP_NATIVE();
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, withPrefix64k,
                                  (BYTE*)dest - 64 KB, NULL, 0);
}

/* Another obsolete API function, paired with the previous one. */
int LZ4_decompress_fast_withPrefix64k(const char* source, char* dest, int originalSize)
{
    AOCL_SETUP_NATIVE();
    /* LZ4_decompress_fast doesn't validate match offsets,
     * and thus serves well with any prefixed dictionary. */
    return LZ4_decompress_fast(source, dest, originalSize);
}

LZ4_FORCE_O2
static int LZ4_decompress_safe_withSmallPrefix(const char* source, char* dest, int compressedSize, int maxOutputSize,
                                               size_t prefixSize)
{
    AOCL_SETUP_NATIVE();
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, noDict,
                                  (BYTE*)dest-prefixSize, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_safe_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int maxOutputSize,
                                     const void* dictStart, size_t dictSize)
{
    AOCL_SETUP_NATIVE();
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest, (const BYTE*)dictStart, dictSize);
}

LZ4_FORCE_O2
static int LZ4_decompress_fast_extDict(const char* source, char* dest, int originalSize,
                                       const void* dictStart, size_t dictSize)
{
    AOCL_SETUP_NATIVE();
    return LZ4_decompress_generic(source, dest, 0, originalSize,
                                  endOnOutputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest, (const BYTE*)dictStart, dictSize);
}

/* The "double dictionary" mode, for use with e.g. ring buffers: the first part
 * of the dictionary is passed as prefix, and the second via dictStart + dictSize.
 * These routines are used only once, in LZ4_decompress_*_continue().
 */
LZ4_FORCE_INLINE
int LZ4_decompress_safe_doubleDict(const char* source, char* dest, int compressedSize, int maxOutputSize,
                                   size_t prefixSize, const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest-prefixSize, (const BYTE*)dictStart, dictSize);
}

LZ4_FORCE_INLINE
int LZ4_decompress_fast_doubleDict(const char* source, char* dest, int originalSize,
                                   size_t prefixSize, const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_generic(source, dest, 0, originalSize,
                                  endOnOutputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest-prefixSize, (const BYTE*)dictStart, dictSize);
}

/*===== streaming decompression functions =====*/

LZ4_streamDecode_t* LZ4_createStreamDecode(void)
{
    LZ4_streamDecode_t* lz4s = (LZ4_streamDecode_t*) ALLOC_AND_ZERO(sizeof(LZ4_streamDecode_t));
    LZ4_STATIC_ASSERT(LZ4_STREAMDECODESIZE >= sizeof(LZ4_streamDecode_t_internal));    /* A compilation error here means LZ4_STREAMDECODESIZE is not large enough */
    return lz4s;
}

int LZ4_freeStreamDecode (LZ4_streamDecode_t* LZ4_stream)
{
    if (LZ4_stream == NULL) { return 0; }  /* support free on NULL */
    FREEMEM(LZ4_stream);
    return 0;
}

/*  LZ4_setStreamDecode() :
 *  Use this function to instruct where to find the dictionary.
 *  This function is not necessary if previous data is still available where it was decoded.
 *  Loading a size of 0 is allowed (same effect as no dictionary).
 *  @return : 1 if OK, 0 if error
 */
int LZ4_setStreamDecode (LZ4_streamDecode_t* LZ4_streamDecode, const char* dictionary, int dictSize)
{
    if(LZ4_streamDecode==NULL)
        return 0;
    
    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    lz4sd->prefixSize = (size_t) dictSize;
    lz4sd->prefixEnd = (const BYTE*) dictionary + dictSize;
    lz4sd->externalDict = NULL;
    lz4sd->extDictSize  = 0;
    return 1;
}

/*  LZ4_decoderRingBufferSize() :
 *  when setting a ring buffer for streaming decompression (optional scenario),
 *  provides the minimum size of this ring buffer
 *  to be compatible with any source respecting maxBlockSize condition.
 *  Note : in a ring buffer scenario,
 *  blocks are presumed decompressed next to each other.
 *  When not enough space remains for next block (remainingSize < maxBlockSize),
 *  decoding resumes from beginning of ring buffer.
 *  @return : minimum ring buffer size,
 *           or 0 if there is an error (invalid maxBlockSize).
 */
int LZ4_decoderRingBufferSize(int maxBlockSize)
{
    if (maxBlockSize < 0) return 0;
    if (maxBlockSize > LZ4_MAX_INPUT_SIZE) return 0;
    if (maxBlockSize < 16) maxBlockSize = 16;
    return LZ4_DECODER_RING_BUFFER_SIZE(maxBlockSize);
}

/*
*_continue() :
    These decoding functions allow decompression of multiple blocks in "streaming" mode.
    Previously decoded blocks must still be available at the memory position where they were decoded.
    If it's not possible, save the relevant part of decoded data into a safe buffer,
    and indicate where it stands using LZ4_setStreamDecode()
*/
LZ4_FORCE_O2
int LZ4_decompress_safe_continue (LZ4_streamDecode_t* LZ4_streamDecode, const char* source, char* dest, int compressedSize, int maxOutputSize)
{
    AOCL_SETUP_NATIVE();
    if(LZ4_streamDecode==NULL)
        return -1;

    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    int result;

    if (lz4sd->prefixSize == 0) {
        /* The first call, no dictionary yet. */
        assert(lz4sd->extDictSize == 0);
        result = LZ4_decompress_safe(source, dest, compressedSize, maxOutputSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)result;
        lz4sd->prefixEnd = (BYTE*)dest + result;
    } else if (lz4sd->prefixEnd == (BYTE*)dest) {
        /* They're rolling the current segment. */
        if (lz4sd->prefixSize >= 64 KB - 1)
            result = LZ4_decompress_safe_withPrefix64k(source, dest, compressedSize, maxOutputSize);
        else if (lz4sd->extDictSize == 0)
            result = LZ4_decompress_safe_withSmallPrefix(source, dest, compressedSize, maxOutputSize,
                                                         lz4sd->prefixSize);
        else
            result = LZ4_decompress_safe_doubleDict(source, dest, compressedSize, maxOutputSize,
                                                    lz4sd->prefixSize, lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize += (size_t)result;
        lz4sd->prefixEnd  += result;
    } else {
        /* The buffer wraps around, or they're switching to another buffer. */
        lz4sd->extDictSize = lz4sd->prefixSize;
        lz4sd->externalDict = lz4sd->prefixEnd - lz4sd->extDictSize;
        result = LZ4_decompress_safe_forceExtDict(source, dest, compressedSize, maxOutputSize,
                                                  lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)result;
        lz4sd->prefixEnd  = (BYTE*)dest + result;
    }

    return result;
}

LZ4_FORCE_O2
int LZ4_decompress_fast_continue (LZ4_streamDecode_t* LZ4_streamDecode, const char* source, char* dest, int originalSize)
{
    AOCL_SETUP_NATIVE();
    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    int result;
    assert(originalSize >= 0);

    if (lz4sd->prefixSize == 0) {
        assert(lz4sd->extDictSize == 0);
        result = LZ4_decompress_fast(source, dest, originalSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)originalSize;
        lz4sd->prefixEnd = (BYTE*)dest + originalSize;
    } else if (lz4sd->prefixEnd == (BYTE*)dest) {
        if (lz4sd->prefixSize >= 64 KB - 1 || lz4sd->extDictSize == 0)
            result = LZ4_decompress_fast(source, dest, originalSize);
        else
            result = LZ4_decompress_fast_doubleDict(source, dest, originalSize,
                                                    lz4sd->prefixSize, lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize += (size_t)originalSize;
        lz4sd->prefixEnd  += originalSize;
    } else {
        lz4sd->extDictSize = lz4sd->prefixSize;
        lz4sd->externalDict = lz4sd->prefixEnd - lz4sd->extDictSize;
        result = LZ4_decompress_fast_extDict(source, dest, originalSize,
                                             lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)originalSize;
        lz4sd->prefixEnd  = (BYTE*)dest + originalSize;
    }

    return result;
}


/*
Advanced decoding functions :
*_usingDict() :
    These decoding functions work the same as "_continue" ones,
    the dictionary must be explicitly provided within parameters
*/

int LZ4_decompress_safe_usingDict(const char* source, char* dest, int compressedSize, int maxOutputSize, const char* dictStart, int dictSize)
{
    AOCL_SETUP_NATIVE();
    if (dictSize==0)
        return LZ4_decompress_safe(source, dest, compressedSize, maxOutputSize);
    if (dictStart+dictSize == dest) {
        if (dictSize >= 64 KB - 1) {
            return LZ4_decompress_safe_withPrefix64k(source, dest, compressedSize, maxOutputSize);
        }
        assert(dictSize >= 0);
        return LZ4_decompress_safe_withSmallPrefix(source, dest, compressedSize, maxOutputSize, (size_t)dictSize);
    }
    assert(dictSize >= 0);
    return LZ4_decompress_safe_forceExtDict(source, dest, compressedSize, maxOutputSize, dictStart, (size_t)dictSize);
}

int LZ4_decompress_fast_usingDict(const char* source, char* dest, int originalSize, const char* dictStart, int dictSize)
{
    AOCL_SETUP_NATIVE();
    if (dictSize==0 || dictStart+dictSize == dest)
        return LZ4_decompress_fast(source, dest, originalSize);
    assert(dictSize >= 0);
    return LZ4_decompress_fast_extDict(source, dest, originalSize, dictStart, (size_t)dictSize);
}


/*=*************************************************
*  Obsolete Functions
***************************************************/
/* obsolete compression functions */
int LZ4_compress_limitedOutput(const char* source, char* dest, int inputSize, int maxOutputSize)
{
    return LZ4_compress_default(source, dest, inputSize, maxOutputSize);
}
int LZ4_compress(const char* src, char* dest, int srcSize)
{
    return LZ4_compress_default(src, dest, srcSize, LZ4_compressBound(srcSize));
}
int LZ4_compress_limitedOutput_withState (void* state, const char* src, char* dst, int srcSize, int dstSize)
{
    return LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 1);
}
int LZ4_compress_withState (void* state, const char* src, char* dst, int srcSize)
{
    return LZ4_compress_fast_extState(state, src, dst, srcSize, LZ4_compressBound(srcSize), 1);
}
int LZ4_compress_limitedOutput_continue (LZ4_stream_t* LZ4_stream, const char* src, char* dst, int srcSize, int dstCapacity)
{
    return LZ4_compress_fast_continue(LZ4_stream, src, dst, srcSize, dstCapacity, 1);
}
int LZ4_compress_continue (LZ4_stream_t* LZ4_stream, const char* source, char* dest, int inputSize)
{
    return LZ4_compress_fast_continue(LZ4_stream, source, dest, inputSize, LZ4_compressBound(inputSize), 1);
}

/*
These decompression functions are deprecated and should no longer be used.
They are only provided here for compatibility with older user programs.
- LZ4_uncompress is totally equivalent to LZ4_decompress_fast
- LZ4_uncompress_unknownOutputSize is totally equivalent to LZ4_decompress_safe
*/
int LZ4_uncompress (const char* source, char* dest, int outputSize)
{
    return LZ4_decompress_fast(source, dest, outputSize);
}
int LZ4_uncompress_unknownOutputSize (const char* source, char* dest, int isize, int maxOutputSize)
{
    return LZ4_decompress_safe(source, dest, isize, maxOutputSize);
}

/* Obsolete Streaming functions */

int LZ4_sizeofStreamState(void) { return LZ4_STREAMSIZE; }

int LZ4_resetStreamState(void* state, char* inputBuffer)
{
    (void)inputBuffer;
    LZ4_resetStream((LZ4_stream_t*)state);
    return 0;
}

void* LZ4_create (char* inputBuffer)
{
    (void)inputBuffer;
    return LZ4_createStream();
}

char* LZ4_slideInputBuffer (void* state)
{
    /* avoid const char * -> char * conversion warning */
    return (char *)(uptrval)((LZ4_stream_t*)state)->internal_donotuse.dictionary;
}

#endif   /* LZ4_COMMONDEFS_ONLY */
