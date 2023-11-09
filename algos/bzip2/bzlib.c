
/*-------------------------------------------------------------*/
/*--- Library top-level functions.                          ---*/
/*---                                               bzlib.c ---*/
/*-------------------------------------------------------------*/

/* ------------------------------------------------------------------
   This file is part of bzip2/libbzip2, a program and library for
   lossless, block-sorting data compression.

   bzip2/libbzip2 version 1.0.8 of 13 July 2019
   Copyright (C) 1996-2019 Julian Seward <jseward@acm.org>
   Copyright (C) 2023, Advanced Micro Devices. All rights reserved.

   Please read the WARNING, DISCLAIMER and PATENTS sections in the 
   README file.

   This program is released under the terms of the license contained
   in the file LICENSE.
   ------------------------------------------------------------------ */

/* CHANGES
   0.9.0    -- original version.
   0.9.0a/b -- no changes in this file.
   0.9.0c   -- made zero-length BZ_FLUSH work correctly in bzCompress().
     fixed bzWrite/bzRead to ignore zero-length requests.
     fixed bzread to correctly handle read requests after EOF.
     wrong parameter order in call to bzDecompressInit in
     bzBuffToBuffDecompress.  Fixed.
*/

#include "utils/utils.h"
#include "bzlib_private.h"

#ifdef AOCL_BZIP2_OPT
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

static int setup_ok_bzip2 = 0; // flag to indicate status of dynamic dispatcher setup

/*---------------------------------------------------*/
/*--- Compression stuff                           ---*/
/*---------------------------------------------------*/


/*---------------------------------------------------*/
#ifndef BZ_NO_STDIO
void BZ2_bz__AssertH__fail ( int errcode )
{
   fprintf(stderr, 
      "\n\nbzip2/libbzip2: internal error number %d.\n"
      "This is a bug in bzip2/libbzip2, %s.\n"
      "Please report it to: bzip2-devel@sourceware.org.  If this happened\n"
      "when you were using some program which uses libbzip2 as a\n"
      "component, you should also report this bug to the author(s)\n"
      "of that program.  Please make an effort to report this bug;\n"
      "timely and accurate bug reports eventually lead to higher\n"
      "quality software.  Thanks.\n\n",
      errcode,
      BZ2_bzlibVersion()
   );

   if (errcode == 1007) {
   fprintf(stderr,
      "\n*** A special note about internal error number 1007 ***\n"
      "\n"
      "Experience suggests that a common cause of i.e. 1007\n"
      "is unreliable memory or other hardware.  The 1007 assertion\n"
      "just happens to cross-check the results of huge numbers of\n"
      "memory reads/writes, and so acts (unintendedly) as a stress\n"
      "test of your memory system.\n"
      "\n"
      "I suggest the following: try compressing the file again,\n"
      "possibly monitoring progress in detail with the -vv flag.\n"
      "\n"
      "* If the error cannot be reproduced, and/or happens at different\n"
      "  points in compression, you may have a flaky memory system.\n"
      "  Try a memory-test program.  I have used Memtest86\n"
      "  (www.memtest86.com).  At the time of writing it is free (GPLd).\n"
      "  Memtest86 tests memory much more thorougly than your BIOSs\n"
      "  power-on test, and may find failures that the BIOS doesn't.\n"
      "\n"
      "* If the error can be repeatably reproduced, this is a bug in\n"
      "  bzip2, and I would very much like to hear about it.  Please\n"
      "  let me know, and, ideally, save a copy of the file causing the\n"
      "  problem -- without which I will be unable to investigate it.\n"
      "\n"
   );
   }

   exit(3);
}
#endif

static Bool copy_input_until_stop ( EState* s );
static Bool copy_output_until_stop ( EState* s );
static Bool AOCL_copy_input_until_stop ( EState* s );
static Bool AOCL_copy_output_until_stop ( EState* s );
static Bool AOCL_copy_output_until_stop_avx2 ( EState* s );

 Int32 (*AOCL_BZ2_decompress_fp) ( DState* ) = BZ2_decompress;
 Bool  (*AOCL_copy_input_until_stop_fp) ( EState* s) = copy_input_until_stop;
 Bool  (*AOCL_copy_output_until_stop_fp) ( EState* s) = copy_output_until_stop;

void aocl_register_decompress_fmv(int optOff, int optLevel)
{
   if (optOff)
   {
      AOCL_BZ2_decompress_fp = BZ2_decompress;
   }
   else
   {
      switch (optLevel)
      {
         case -1: // undecided. use defaults based on compiler flags
#ifdef AOCL_BZIP2_OPT
            AOCL_BZ2_decompress_fp = AOCL_BZ2_decompress;
#else
            AOCL_BZ2_decompress_fp = BZ2_decompress;
#endif
            break;
         case 0://C version
         case 1://SSE version
         case 2://AVX version
         case 3://AVX2 version
         default://AVX512 and other versions
            AOCL_BZ2_decompress_fp = AOCL_BZ2_decompress;
            break;
      }
   }
}

void aocl_register_copy_fmv(int optOff, int optLevel)
{
   if (optOff)
   {
      AOCL_copy_input_until_stop_fp = copy_input_until_stop;
      AOCL_copy_output_until_stop_fp = copy_output_until_stop;
   }
   else
   {
      switch (optLevel)
      {
         case -1: // undecided. use defaults based on compiler flags
#ifdef AOCL_BZIP2_AVX2_OPT
            AOCL_copy_input_until_stop_fp = AOCL_copy_input_until_stop;
            AOCL_copy_output_until_stop_fp = AOCL_copy_output_until_stop_avx2;
#elif defined(AOCL_BZIP2_OPT)
            AOCL_copy_input_until_stop_fp = AOCL_copy_input_until_stop;
            AOCL_copy_output_until_stop_fp = AOCL_copy_output_until_stop;
#else
            AOCL_copy_input_until_stop_fp = copy_input_until_stop;
            AOCL_copy_output_until_stop_fp = copy_output_until_stop;
#endif
            break;
         case 0://C version
         case 1://SSE version
         case 2://AVX version
            AOCL_copy_input_until_stop_fp = AOCL_copy_input_until_stop;
            AOCL_copy_output_until_stop_fp = AOCL_copy_output_until_stop;
            break;
         case 3://AVX2 version
         default://AVX512 and other versions
            AOCL_copy_input_until_stop_fp = AOCL_copy_input_until_stop;
            AOCL_copy_output_until_stop_fp = AOCL_copy_output_until_stop_avx2;
            break;
      }
   }
}

BZ_EXTERN char * BZ_API(aocl_setup_bzip2) 
                     ( int optOff,
                       int optLevel,
                       size_t insize,
                       size_t level,
                       size_t windowLog )
{
    AOCL_ENTER_CRITICAL(setup_bzip2)
    if (!setup_ok_bzip2) {
        optOff = optOff ? 1 : get_disable_opt_flags(0);
        aocl_register_decompress_fmv(optOff, optLevel);
        aocl_register_copy_fmv(optOff, optLevel);
        aocl_register_mainSimpleSort_fmv(optOff, optLevel);
        setup_ok_bzip2 = 1;
    }
    AOCL_EXIT_CRITICAL(setup_bzip2)
   return NULL;
}

#ifdef AOCL_BZIP2_OPT
static void aocl_setup_native(void) {
    AOCL_ENTER_CRITICAL(setup_bzip2)
    if (!setup_ok_bzip2) {
        int optLevel = get_cpu_opt_flags(0);
        int optOff = get_disable_opt_flags(0);
        aocl_register_decompress_fmv(optOff, optLevel);
        aocl_register_copy_fmv(optOff, optLevel);
        aocl_register_mainSimpleSort_fmv(optOff, optLevel);
        setup_ok_bzip2 = 1;
    }
    AOCL_EXIT_CRITICAL(setup_bzip2)
}
#endif

BZ_EXTERN void BZ_API(aocl_destroy_bzip2) (void){
    AOCL_ENTER_CRITICAL(setup_bzip2)
    setup_ok_bzip2 = 0;
    AOCL_EXIT_CRITICAL(setup_bzip2)
}

/*---------------------------------------------------*/
static
int bz_config_ok ( void )
{
   if (sizeof(int)   != 4) return 0;
   if (sizeof(short) != 2) return 0;
   if (sizeof(char)  != 1) return 0;
   return 1;
}


/*---------------------------------------------------*/
static
void* default_bzalloc ( void* opaque, Int32 items, Int32 size )
{
   void* v = malloc ( items * size );
   return v;
}

static
void default_bzfree ( void* opaque, void* addr )
{
   if (addr != NULL) free ( addr );
}


/*---------------------------------------------------*/
static
void prepare_new_block ( EState* s )
{
   Int32 i;
   s->nblock = 0;
   s->numZ = 0;
   s->state_out_pos = 0;
   BZ_INITIALISE_CRC ( s->blockCRC );
   for (i = 0; i < 256; i++) s->inUse[i] = False;
   s->blockNo++;
}


/*---------------------------------------------------*/
static
void init_RL ( EState* s )
{
   s->state_in_ch  = 256;
   s->state_in_len = 0;
}


static
Bool isempty_RL ( EState* s )
{
   if (s->state_in_ch < 256 && s->state_in_len > 0)
      return False; else
      return True;
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzCompressInit) 
                    ( bz_stream* strm, 
                     int        blockSize100k,
                     int        verbosity,
                     int        workFactor )
{
   Int32   n;
   EState* s;

   if (!bz_config_ok()) return BZ_CONFIG_ERROR;

   if (strm == NULL || 
       blockSize100k < 1 || blockSize100k > 9 ||
       workFactor < 0 || workFactor > 250 ||
       verbosity < 0 || verbosity > 4)
     return BZ_PARAM_ERROR;

   if (workFactor == 0) workFactor = 30;
   if (strm->bzalloc == NULL) strm->bzalloc = default_bzalloc;
   if (strm->bzfree == NULL) strm->bzfree = default_bzfree;

   s = BZALLOC( sizeof(EState) );
   if (s == NULL) return BZ_MEM_ERROR;
   s->strm = strm;

   s->arr1 = NULL;
   s->arr2 = NULL;
   s->ftab = NULL;

   n       = 100000 * blockSize100k;
   s->arr1 = BZALLOC( n                  * sizeof(UInt32) );
   s->arr2 = BZALLOC( (n+BZ_N_OVERSHOOT) * sizeof(UInt32) );
   s->ftab = BZALLOC( 65537              * sizeof(UInt32) );

   if (s->arr1 == NULL || s->arr2 == NULL || s->ftab == NULL) {
      if (s->arr1 != NULL) BZFREE(s->arr1);
      if (s->arr2 != NULL) BZFREE(s->arr2);
      if (s->ftab != NULL) BZFREE(s->ftab);
      if (s       != NULL) BZFREE(s);
      return BZ_MEM_ERROR;
   }

   s->blockNo           = 0;
   s->state             = BZ_S_INPUT;
   s->mode              = BZ_M_RUNNING;
   s->combinedCRC       = 0;
   s->blockSize100k     = blockSize100k;
   s->nblockMAX         = 100000 * blockSize100k - 19;
   s->verbosity         = verbosity;
   s->workFactor        = workFactor;

   s->block             = (UChar*)s->arr2;
   s->mtfv              = (UInt16*)s->arr1;
   s->zbits             = NULL;
   s->ptr               = (UInt32*)s->arr1;

   strm->state          = s;
   strm->total_in_lo32  = 0;
   strm->total_in_hi32  = 0;
   strm->total_out_lo32 = 0;
   strm->total_out_hi32 = 0;
   init_RL ( s );
   prepare_new_block ( s );
   return BZ_OK;
}


/*---------------------------------------------------*/
static
void add_pair_to_block ( EState* s )
{
   Int32 i;
   UChar ch = (UChar)(s->state_in_ch);
   for (i = 0; i < s->state_in_len; i++) {
      BZ_UPDATE_CRC( s->blockCRC, ch );
   }
   s->inUse[s->state_in_ch] = True;
   switch (s->state_in_len) {
      case 1:
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         break;
      case 2:
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         break;
      case 3:
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         break;
      default:
         s->inUse[s->state_in_len-4] = True;
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         s->block[s->nblock] = (UChar)ch; s->nblock++;
         s->block[s->nblock] = ((UChar)(s->state_in_len-4));
         s->nblock++;
         break;
   }
}


/*---------------------------------------------------*/
static
void flush_RL ( EState* s )
{
   if (s->state_in_ch < 256) add_pair_to_block ( s );
   init_RL ( s );
}


/*---------------------------------------------------*/
#define ADD_CHAR_TO_BLOCK(zs,zchh0)               \
{                                                 \
   UInt32 zchh = (UInt32)(zchh0);                 \
   /*-- fast track the common case --*/           \
   if (zchh != zs->state_in_ch &&                 \
       zs->state_in_len == 1) {                   \
      UChar ch = (UChar)(zs->state_in_ch);        \
      BZ_UPDATE_CRC( zs->blockCRC, ch );          \
      zs->inUse[zs->state_in_ch] = True;          \
      zs->block[zs->nblock] = (UChar)ch;          \
      zs->nblock++;                               \
      zs->state_in_ch = zchh;                     \
   }                                              \
   else                                           \
   /*-- general, uncommon cases --*/              \
   if (zchh != zs->state_in_ch ||                 \
      zs->state_in_len == 255) {                  \
      if (zs->state_in_ch < 256)                  \
         add_pair_to_block ( zs );                \
      zs->state_in_ch = zchh;                     \
      zs->state_in_len = 1;                       \
   } else {                                       \
      zs->state_in_len++;                         \
   }                                              \
}


/*---------------------------------------------------*/
static
Bool copy_input_until_stop ( EState* s )
{
   Bool progress_in = False;

   if (s->mode == BZ_M_RUNNING) {

      /*-- fast track the common case --*/
      while (True) {
         /*-- block full? --*/
         if (s->nblock >= s->nblockMAX) break;
         /*-- no input? --*/
         if (s->strm->avail_in == 0) break;
         progress_in = True;
         ADD_CHAR_TO_BLOCK ( s, (UInt32)(*((UChar*)(s->strm->next_in))) ); 
         s->strm->next_in++;
         s->strm->avail_in--;
         s->strm->total_in_lo32++;
         if (s->strm->total_in_lo32 == 0) s->strm->total_in_hi32++;
      }

   } else {

      /*-- general, uncommon case --*/
      while (True) {
         /*-- block full? --*/
         if (s->nblock >= s->nblockMAX) break;
         /*-- no input? --*/
         if (s->strm->avail_in == 0) break;
         /*-- flush/finish end? --*/
         if (s->avail_in_expect == 0) break;
         progress_in = True;
         ADD_CHAR_TO_BLOCK ( s, (UInt32)(*((UChar*)(s->strm->next_in))) ); 
         s->strm->next_in++;
         s->strm->avail_in--;
         s->strm->total_in_lo32++;
         if (s->strm->total_in_lo32 == 0) s->strm->total_in_hi32++;
         s->avail_in_expect--;
      }
   }
   return progress_in;
}

#ifdef AOCL_BZIP2_OPT
/*
   In this optimized function, if condititions and variable incr/decr are
   removed from the while loop, and loop limit is pre-calculated before
   executing the loop and variables are modified accordingly.
*/
static
Bool AOCL_copy_input_until_stop ( EState* s )
{
   Bool progress_in = False;

   // The variable `chars_to_copy` stores the number of times the loop is going to get iterated
   // and suitably it modifies the values of other variables at one go.
   UInt32 chars_to_copy = s->strm->avail_in;

   if(s->mode != BZ_M_RUNNING && chars_to_copy > s->avail_in_expect)
      chars_to_copy = s->avail_in_expect;

   // This condition is to check if the while loop is executed atleast once.
   if(chars_to_copy && s->nblock < s->nblockMAX)
      progress_in = True;

   if(s->strm->total_in_lo32 + chars_to_copy < s->strm->total_in_lo32)
      s->strm->total_in_hi32++;

   s->strm->total_in_lo32 += chars_to_copy;
   s->strm->avail_in -= chars_to_copy;

   if(s->mode != BZ_M_RUNNING)
      s->avail_in_expect-=chars_to_copy;

   while (s->nblock < s->nblockMAX && chars_to_copy) {
      chars_to_copy--;
      ADD_CHAR_TO_BLOCK ( s, (UInt32)(*((UChar*)(s->strm->next_in)))); 
      s->strm->next_in++;
   }
   
   // If the loop quits before `chars_to_copy` becomes zero, i.e, s->nblock >= s->nblockMAX
   // then those values will be corrected accordingly.
   if(s->strm->total_in_lo32 - chars_to_copy > s->strm->total_in_lo32)
      s->strm->total_in_hi32--;

   s->strm->total_in_lo32 -= chars_to_copy;
   s->strm->avail_in += chars_to_copy;

   if(s->mode != BZ_M_RUNNING)
      s->avail_in_expect+=chars_to_copy;
   
   return progress_in;
}
#endif

/*---------------------------------------------------*/
static
Bool copy_output_until_stop ( EState* s )
{
   Bool progress_out = False;

   while (True) {

      /*-- no output space? --*/
      if (s->strm->avail_out == 0) break;

      /*-- block done? --*/
      if (s->state_out_pos >= s->numZ) break;

      progress_out = True;
      *(s->strm->next_out) = s->zbits[s->state_out_pos];
      s->state_out_pos++;
      s->strm->avail_out--;
      s->strm->next_out++;
      s->strm->total_out_lo32++;
      if (s->strm->total_out_lo32 == 0) s->strm->total_out_hi32++;
   }

   return progress_out;
}

#ifdef AOCL_BZIP2_OPT
#ifdef AOCL_BZIP2_AVX2_OPT
#include <immintrin.h>
__attribute__((__target__("avx2")))
static inline void FastMemcopy64Bytes(UChar* dst, UChar* src) {
   LOG_UNFORMATTED(DEBUG, logCtx, "Enter");
   __m256i* dst1 = (__m256i*)dst;
   __m256i* src1 = (__m256i*)src;
   __m256i s1 = _mm256_lddqu_si256(src1);
   __m256i s2 = _mm256_lddqu_si256(src1 + 1);
   _mm256_storeu_si256(dst1, s1);
   _mm256_storeu_si256(dst1 + 1, s2);
}

static inline void memcpy_ (UChar* dst, UChar* src, UInt32 length) {
   UInt32 fastLen = length - (length % 64);
   UInt32 i=0;

   for (i=0; i < fastLen; i += 64)
      FastMemcopy64Bytes(dst + i, src + i);

   memcpy(dst + fastLen, src + fastLen, length-fastLen);
}

static Bool AOCL_copy_output_until_stop_avx2 ( EState* s ) {
   Bool progress_out = False;
   UInt32 chars_to_copy = 0;

   if (s->strm->avail_out == 0 || s->state_out_pos >= s->numZ) return False;

   progress_out = True;

   chars_to_copy = s->strm->avail_out;
   if((s->numZ - s->state_out_pos) < chars_to_copy)
   {
      chars_to_copy = (s->numZ - s->state_out_pos);
   }

   memcpy_((UChar*)s->strm->next_out, &s->zbits[s->state_out_pos], chars_to_copy);

   s->strm->total_out_hi32 += (unsigned int)(s->strm->total_out_lo32 + chars_to_copy) < s->strm->total_out_lo32 ? 1 : 0;
   s->strm->total_out_lo32 += chars_to_copy;
   s->strm->next_out += chars_to_copy;
   s->strm->avail_out -= chars_to_copy;
   s->state_out_pos += chars_to_copy;
   
   return progress_out;
}
#endif /* AOCL_BZIP2_AVX2_OPT */

static Bool AOCL_copy_output_until_stop ( EState* s ) {
   Bool progress_out = False;
   
   // This variable stores the number of characters (loop iterations) that are to be copied
   // from `s->zbits` to `s->strm->next_out`.
   UInt32 chars_to_copy = 0;

   if (s->strm->avail_out == 0 || s->state_out_pos >= s->numZ) return False;

   progress_out = True;

   chars_to_copy = s->strm->avail_out;
   if((s->numZ - s->state_out_pos) < chars_to_copy)
   {
      chars_to_copy = (s->numZ - s->state_out_pos);
   }

   memcpy((UChar*)s->strm->next_out, &s->zbits[s->state_out_pos], chars_to_copy);

   // Instead of incrementing/decrementing all the variable values at each iteration,
   // `chars_to_copy` is used to change the values of other variables at one go.
   s->strm->total_out_hi32 += (s->strm->total_out_lo32 + chars_to_copy) < s->strm->total_out_lo32 ? 1 : 0;
   s->strm->total_out_lo32 += chars_to_copy;
   s->strm->next_out += chars_to_copy;
   s->strm->avail_out -= chars_to_copy;
   s->state_out_pos += chars_to_copy;

   return progress_out;
}
#endif /* AOCL_BZIP2_OPT */

/*---------------------------------------------------*/
static
Bool handle_compress ( bz_stream* strm )
{
   Bool progress_in  = False;
   Bool progress_out = False;
   EState* s = strm->state;
   
   while (True) {

      if (s->state == BZ_S_OUTPUT) {
#ifdef AOCL_BZIP2_OPT
         progress_out |= AOCL_copy_output_until_stop_fp ( s );
#else
         progress_out |= copy_output_until_stop ( s );
#endif
         if (s->state_out_pos < s->numZ) break;
         if (s->mode == BZ_M_FINISHING && 
             s->avail_in_expect == 0 &&
             isempty_RL(s)) break;
         prepare_new_block ( s );
         s->state = BZ_S_INPUT;
         if (s->mode == BZ_M_FLUSHING && 
             s->avail_in_expect == 0 &&
             isempty_RL(s)) break;
      }

      if (s->state == BZ_S_INPUT) {
#ifdef AOCL_BZIP2_OPT
         progress_in |= AOCL_copy_input_until_stop_fp ( s );
#else
         progress_in |= copy_input_until_stop ( s );
#endif
         if (s->mode != BZ_M_RUNNING && s->avail_in_expect == 0) {
            flush_RL ( s );
            BZ2_compressBlock ( s, (Bool)(s->mode == BZ_M_FINISHING) );
            s->state = BZ_S_OUTPUT;
         }
         else
         if (s->nblock >= s->nblockMAX) {
            BZ2_compressBlock ( s, False );
            s->state = BZ_S_OUTPUT;
         }
         else
         if (s->strm->avail_in == 0) {
            break;
         }
      }

   }

   return progress_in || progress_out;
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzCompress) ( bz_stream *strm, int action )
{
   AOCL_SETUP_NATIVE();
   Bool progress;
   EState* s;
   if (strm == NULL) return BZ_PARAM_ERROR;
   s = strm->state;
   if (s == NULL) return BZ_PARAM_ERROR;
   if (s->strm != strm) return BZ_PARAM_ERROR;

   preswitch:
   switch (s->mode) {

      case BZ_M_IDLE:
         return BZ_SEQUENCE_ERROR;

      case BZ_M_RUNNING:
         if (action == BZ_RUN) {
            progress = handle_compress ( strm );
            return progress ? BZ_RUN_OK : BZ_PARAM_ERROR;
         } 
         else
	      if (action == BZ_FLUSH) {
            s->avail_in_expect = strm->avail_in;
            s->mode = BZ_M_FLUSHING;
            goto preswitch;
         }
         else
         if (action == BZ_FINISH) {
            s->avail_in_expect = strm->avail_in;
            s->mode = BZ_M_FINISHING;
            goto preswitch;
         }
         else 
            return BZ_PARAM_ERROR;

      case BZ_M_FLUSHING:
         if (action != BZ_FLUSH) return BZ_SEQUENCE_ERROR;
         if (s->avail_in_expect != s->strm->avail_in) 
            return BZ_SEQUENCE_ERROR;
         progress = handle_compress ( strm );
         if (s->avail_in_expect > 0 || !isempty_RL(s) ||
             s->state_out_pos < s->numZ) return BZ_FLUSH_OK;
         s->mode = BZ_M_RUNNING;
         return BZ_RUN_OK;

      case BZ_M_FINISHING:
         if (action != BZ_FINISH) return BZ_SEQUENCE_ERROR;
         if (s->avail_in_expect != s->strm->avail_in) 
            return BZ_SEQUENCE_ERROR;
         progress = handle_compress ( strm );
         if (!progress) return BZ_SEQUENCE_ERROR;
         if (s->avail_in_expect > 0 || !isempty_RL(s) ||
             s->state_out_pos < s->numZ) return BZ_FINISH_OK;
         s->mode = BZ_M_IDLE;
         return BZ_STREAM_END;
   }
   return BZ_PARAM_ERROR; /*--not reached--*/
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzCompressEnd)  ( bz_stream *strm )
{
   EState* s;
   if (strm == NULL) return BZ_PARAM_ERROR;
   s = strm->state;
   if (s == NULL) return BZ_PARAM_ERROR;
   if (s->strm != strm) return BZ_PARAM_ERROR;

   if (s->arr1 != NULL) BZFREE(s->arr1);
   if (s->arr2 != NULL) BZFREE(s->arr2);
   if (s->ftab != NULL) BZFREE(s->ftab);
   BZFREE(strm->state);

   strm->state = NULL;   

   return BZ_OK;
}


/*---------------------------------------------------*/
/*--- Decompression stuff                         ---*/
/*---------------------------------------------------*/

/*---------------------------------------------------*/
int BZ_API(BZ2_bzDecompressInit) 
                     ( bz_stream* strm, 
                       int        verbosity,
                       int        small )
{
   DState* s;

   if (!bz_config_ok()) return BZ_CONFIG_ERROR;

   if (strm == NULL) return BZ_PARAM_ERROR;
   if (small != 0 && small != 1) return BZ_PARAM_ERROR;
   if (verbosity < 0 || verbosity > 4) return BZ_PARAM_ERROR;

   if (strm->bzalloc == NULL) strm->bzalloc = default_bzalloc;
   if (strm->bzfree == NULL) strm->bzfree = default_bzfree;

   s = BZALLOC( sizeof(DState) );
   if (s == NULL) return BZ_MEM_ERROR;
   s->strm                  = strm;
   strm->state              = s;
   s->state                 = BZ_X_MAGIC_1;
   s->bsLive                = 0;
   s->bsBuff                = 0;
   s->calculatedCombinedCRC = 0;
   strm->total_in_lo32      = 0;
   strm->total_in_hi32      = 0;
   strm->total_out_lo32     = 0;
   strm->total_out_hi32     = 0;
   s->smallDecompress       = (Bool)small;
   s->ll4                   = NULL;
   s->ll16                  = NULL;
   s->tt                    = NULL;
   s->currBlockNo           = 0;
   s->verbosity             = verbosity;

   return BZ_OK;
}


/*---------------------------------------------------*/
/* Return  True iff data corruption is discovered.
   Returns False if there is no problem.
*/
static
Bool unRLE_obuf_to_output_FAST ( DState* s )
{
   UChar k1;

   if (s->blockRandomised) {

      while (True) {
         /* try to finish existing run */
         while (True) {
            if (s->strm->avail_out == 0) return False;
            if (s->state_out_len == 0) break;
            *( (UChar*)(s->strm->next_out) ) = s->state_out_ch;
            BZ_UPDATE_CRC ( s->calculatedBlockCRC, s->state_out_ch );
            s->state_out_len--;
            s->strm->next_out++;
            s->strm->avail_out--;
            s->strm->total_out_lo32++;
            if (s->strm->total_out_lo32 == 0) s->strm->total_out_hi32++;
         }

         /* can a new run be started? */
         if (s->nblock_used == s->save_nblock+1) return False;
               
         /* Only caused by corrupt data stream? */
         if (s->nblock_used > s->save_nblock+1)
            return True;
   
         s->state_out_len = 1;
         s->state_out_ch = s->k0;
         BZ_GET_FAST(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 2;
         BZ_GET_FAST(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 3;
         BZ_GET_FAST(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         BZ_GET_FAST(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         s->state_out_len = ((Int32)k1) + 4;
         BZ_GET_FAST(s->k0); BZ_RAND_UPD_MASK; 
         s->k0 ^= BZ_RAND_MASK; s->nblock_used++;
      }

   } else {

      /* restore */
      UInt32        c_calculatedBlockCRC = s->calculatedBlockCRC;
      UChar         c_state_out_ch       = s->state_out_ch;
      Int32         c_state_out_len      = s->state_out_len;
      Int32         c_nblock_used        = s->nblock_used;
      Int32         c_k0                 = s->k0;
      UInt32*       c_tt                 = s->tt;
      UInt32        c_tPos               = s->tPos;
      char*         cs_next_out          = s->strm->next_out;
      unsigned int  cs_avail_out         = s->strm->avail_out;
      Int32         ro_blockSize100k     = s->blockSize100k;
      /* end restore */

      UInt32       avail_out_INIT = cs_avail_out;
      Int32        s_save_nblockPP = s->save_nblock+1;
      unsigned int total_out_lo32_old;

      while (True) {

         /* try to finish existing run */
         if (c_state_out_len > 0) {
            while (True) {
               if (cs_avail_out == 0) goto return_notr;
               if (c_state_out_len == 1) break;
               *( (UChar*)(cs_next_out) ) = c_state_out_ch;
               BZ_UPDATE_CRC ( c_calculatedBlockCRC, c_state_out_ch );
               c_state_out_len--;
               cs_next_out++;
               cs_avail_out--;
            }
            s_state_out_len_eq_one:
            {
               if (cs_avail_out == 0) { 
                  c_state_out_len = 1; goto return_notr;
               };
               *( (UChar*)(cs_next_out) ) = c_state_out_ch;
               BZ_UPDATE_CRC ( c_calculatedBlockCRC, c_state_out_ch );
               cs_next_out++;
               cs_avail_out--;
            }
         }   
         /* Only caused by corrupt data stream? */
         if (c_nblock_used > s_save_nblockPP)
            return True;

         /* can a new run be started? */
         if (c_nblock_used == s_save_nblockPP) {
            c_state_out_len = 0; goto return_notr;
         };   
         c_state_out_ch = c_k0;
         BZ_GET_FAST_C(k1); c_nblock_used++;
         if (k1 != c_k0) { 
            c_k0 = k1; goto s_state_out_len_eq_one; 
         };
         if (c_nblock_used == s_save_nblockPP) 
            goto s_state_out_len_eq_one;
   
         c_state_out_len = 2;
         BZ_GET_FAST_C(k1); c_nblock_used++;
         if (c_nblock_used == s_save_nblockPP) continue;
         if (k1 != c_k0) { c_k0 = k1; continue; };
   
         c_state_out_len = 3;
         BZ_GET_FAST_C(k1); c_nblock_used++;
         if (c_nblock_used == s_save_nblockPP) continue;
         if (k1 != c_k0) { c_k0 = k1; continue; };
   
         BZ_GET_FAST_C(k1); c_nblock_used++;
         c_state_out_len = ((Int32)k1) + 4;
         BZ_GET_FAST_C(c_k0); c_nblock_used++;
      }

      return_notr:
      total_out_lo32_old = s->strm->total_out_lo32;
      s->strm->total_out_lo32 += (avail_out_INIT - cs_avail_out);
      if (s->strm->total_out_lo32 < total_out_lo32_old)
         s->strm->total_out_hi32++;

      /* save */
      s->calculatedBlockCRC = c_calculatedBlockCRC;
      s->state_out_ch       = c_state_out_ch;
      s->state_out_len      = c_state_out_len;
      s->nblock_used        = c_nblock_used;
      s->k0                 = c_k0;
      s->tt                 = c_tt;
      s->tPos               = c_tPos;
      s->strm->next_out     = cs_next_out;
      s->strm->avail_out    = cs_avail_out;
      /* end save */
   }
   return False;
}



/*---------------------------------------------------*/
__inline__ Int32 BZ2_indexIntoF ( Int32 indx, Int32 *cftab )
{
   Int32 nb, na, mid;
   nb = 0;
   na = 256;
   do {
      mid = (nb + na) >> 1;
      if (indx >= cftab[mid]) nb = mid; else na = mid;
   }
   while (na - nb != 1);
   return nb;
}


/*---------------------------------------------------*/
/* Return  True iff data corruption is discovered.
   Returns False if there is no problem.
*/
static
Bool unRLE_obuf_to_output_SMALL ( DState* s )
{
   UChar k1;

   if (s->blockRandomised) {

      while (True) {
         /* try to finish existing run */
         while (True) {
            if (s->strm->avail_out == 0) return False;
            if (s->state_out_len == 0) break;
            *( (UChar*)(s->strm->next_out) ) = s->state_out_ch;
            BZ_UPDATE_CRC ( s->calculatedBlockCRC, s->state_out_ch );
            s->state_out_len--;
            s->strm->next_out++;
            s->strm->avail_out--;
            s->strm->total_out_lo32++;
            if (s->strm->total_out_lo32 == 0) s->strm->total_out_hi32++;
         }
   
         /* can a new run be started? */
         if (s->nblock_used == s->save_nblock+1) return False;

         /* Only caused by corrupt data stream? */
         if (s->nblock_used > s->save_nblock+1)
            return True;
   
         s->state_out_len = 1;
         s->state_out_ch = s->k0;
         BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 2;
         BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 3;
         BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK; 
         k1 ^= BZ_RAND_MASK; s->nblock_used++;
         s->state_out_len = ((Int32)k1) + 4;
         BZ_GET_SMALL(s->k0); BZ_RAND_UPD_MASK; 
         s->k0 ^= BZ_RAND_MASK; s->nblock_used++;
      }

   } else {

      while (True) {
         /* try to finish existing run */
         while (True) {
            if (s->strm->avail_out == 0) return False;
            if (s->state_out_len == 0) break;
            *( (UChar*)(s->strm->next_out) ) = s->state_out_ch;
            BZ_UPDATE_CRC ( s->calculatedBlockCRC, s->state_out_ch );
            s->state_out_len--;
            s->strm->next_out++;
            s->strm->avail_out--;
            s->strm->total_out_lo32++;
            if (s->strm->total_out_lo32 == 0) s->strm->total_out_hi32++;
         }
   
         /* can a new run be started? */
         if (s->nblock_used == s->save_nblock+1) return False;

         /* Only caused by corrupt data stream? */
         if (s->nblock_used > s->save_nblock+1)
            return True;
   
         s->state_out_len = 1;
         s->state_out_ch = s->k0;
         BZ_GET_SMALL(k1); s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 2;
         BZ_GET_SMALL(k1); s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 3;
         BZ_GET_SMALL(k1); s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         BZ_GET_SMALL(k1); s->nblock_used++;
         s->state_out_len = ((Int32)k1) + 4;
         BZ_GET_SMALL(s->k0); s->nblock_used++;
      }

   }
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzDecompress) ( bz_stream *strm )
{
   AOCL_SETUP_NATIVE();
   Bool    corrupt;
   DState* s;
   if (strm == NULL) return BZ_PARAM_ERROR;
   s = strm->state;
   if (s == NULL) return BZ_PARAM_ERROR;
   if (s->strm != strm) return BZ_PARAM_ERROR;

   while (True) {
      if (s->state == BZ_X_IDLE) return BZ_SEQUENCE_ERROR;
      if (s->state == BZ_X_OUTPUT) {
         if (s->smallDecompress)
            corrupt = unRLE_obuf_to_output_SMALL ( s ); else
            corrupt = unRLE_obuf_to_output_FAST  ( s );
         if (corrupt) return BZ_DATA_ERROR;
         if (s->nblock_used == s->save_nblock+1 && s->state_out_len == 0) {
            BZ_FINALISE_CRC ( s->calculatedBlockCRC );
            if (s->verbosity >= 3) 
               VPrintf2 ( " {0x%08x, 0x%08x}", s->storedBlockCRC, 
                          s->calculatedBlockCRC );
            if (s->verbosity >= 2) VPrintf0 ( "]" );
            if (s->calculatedBlockCRC != s->storedBlockCRC)
               return BZ_DATA_ERROR;
            s->calculatedCombinedCRC 
               = (s->calculatedCombinedCRC << 1) | 
                    (s->calculatedCombinedCRC >> 31);
            s->calculatedCombinedCRC ^= s->calculatedBlockCRC;
            s->state = BZ_X_BLKHDR_1;
         } else {
            return BZ_OK;
         }
      }
      if (s->state >= BZ_X_MAGIC_1) {
#ifdef AOCL_BZIP2_OPT
         Int32 r = AOCL_BZ2_decompress_fp(s);
#else
         Int32 r = BZ2_decompress(s);
#endif

         if (r == BZ_STREAM_END) {
            if (s->verbosity >= 3)
               VPrintf2 ( "\n    combined CRCs: stored = 0x%08x, computed = 0x%08x", 
                          s->storedCombinedCRC, s->calculatedCombinedCRC );
            if (s->calculatedCombinedCRC != s->storedCombinedCRC)
               return BZ_DATA_ERROR;
            return r;
         }
         if (s->state != BZ_X_OUTPUT) return r;
      }
   }

   AssertH ( 0, 6001 );

   return 0;  /*NOTREACHED*/
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzDecompressEnd)  ( bz_stream *strm )
{
   DState* s;
   if (strm == NULL) return BZ_PARAM_ERROR;
   s = strm->state;
   if (s == NULL) return BZ_PARAM_ERROR;
   if (s->strm != strm) return BZ_PARAM_ERROR;

   if (s->tt   != NULL) BZFREE(s->tt);
   if (s->ll16 != NULL) BZFREE(s->ll16);
   if (s->ll4  != NULL) BZFREE(s->ll4);

   BZFREE(strm->state);
   strm->state = NULL;

   return BZ_OK;
}


#ifndef BZ_NO_STDIO
/*---------------------------------------------------*/
/*--- File I/O stuff                              ---*/
/*---------------------------------------------------*/

#define BZ_SETERR(eee)                    \
{                                         \
   if (bzerror != NULL) *bzerror = eee;   \
   if (bzf != NULL) bzf->lastErr = eee;   \
}

typedef 
   struct {
      FILE*     handle;
      Char      buf[BZ_MAX_UNUSED];
      Int32     bufN;
      Bool      writing;
      bz_stream strm;
      Int32     lastErr;
      Bool      initialisedOk;
   }
   bzFile;


/*---------------------------------------------*/
static Bool myfeof ( FILE* f )
{
   Int32 c = fgetc ( f );
   if (c == EOF) return True;
   ungetc ( c, f );
   return False;
}


/*---------------------------------------------------*/
BZFILE* BZ_API(BZ2_bzWriteOpen) 
                    ( int*  bzerror,      
                      FILE* f, 
                      int   blockSize100k, 
                      int   verbosity,
                      int   workFactor )
{
   Int32   ret;
   bzFile* bzf = NULL;

   BZ_SETERR(BZ_OK);

   if (f == NULL ||
       (blockSize100k < 1 || blockSize100k > 9) ||
       (workFactor < 0 || workFactor > 250) ||
       (verbosity < 0 || verbosity > 4))
      { BZ_SETERR(BZ_PARAM_ERROR); return NULL; };

   if (ferror(f))
      { BZ_SETERR(BZ_IO_ERROR); return NULL; };

   bzf = malloc ( sizeof(bzFile) );
   if (bzf == NULL)
      { BZ_SETERR(BZ_MEM_ERROR); return NULL; };

   BZ_SETERR(BZ_OK);
   bzf->initialisedOk = False;
   bzf->bufN          = 0;
   bzf->handle        = f;
   bzf->writing       = True;
   bzf->strm.bzalloc  = NULL;
   bzf->strm.bzfree   = NULL;
   bzf->strm.opaque   = NULL;

   if (workFactor == 0) workFactor = 30;
   ret = BZ2_bzCompressInit ( &(bzf->strm), blockSize100k, 
                              verbosity, workFactor );
   if (ret != BZ_OK)
      { BZ_SETERR(ret); free(bzf); return NULL; };

   bzf->strm.avail_in = 0;
   bzf->initialisedOk = True;
   return bzf;   
}



/*---------------------------------------------------*/
void BZ_API(BZ2_bzWrite)
             ( int*    bzerror, 
               BZFILE* b, 
               void*   buf, 
               int     len )
{
   AOCL_SETUP_NATIVE();
   Int32 n, n2, ret;
   bzFile* bzf = (bzFile*)b;

   BZ_SETERR(BZ_OK);
   if (bzf == NULL || buf == NULL || len < 0)
      { BZ_SETERR(BZ_PARAM_ERROR); return; };
   if (!(bzf->writing))
      { BZ_SETERR(BZ_SEQUENCE_ERROR); return; };
   if (ferror(bzf->handle))
      { BZ_SETERR(BZ_IO_ERROR); return; };

   if (len == 0)
      { BZ_SETERR(BZ_OK); return; };

   bzf->strm.avail_in = len;
   bzf->strm.next_in  = buf;

   while (True) {
      bzf->strm.avail_out = BZ_MAX_UNUSED;
      bzf->strm.next_out = bzf->buf;
      ret = BZ2_bzCompress ( &(bzf->strm), BZ_RUN );
      if (ret != BZ_RUN_OK)
         { BZ_SETERR(ret); return; };

      if (bzf->strm.avail_out < BZ_MAX_UNUSED) {
         n = BZ_MAX_UNUSED - bzf->strm.avail_out;
         n2 = fwrite ( (void*)(bzf->buf), sizeof(UChar), 
                       n, bzf->handle );
         if (n != n2 || ferror(bzf->handle))
            { BZ_SETERR(BZ_IO_ERROR); return; };
      }

      if (bzf->strm.avail_in == 0)
         { BZ_SETERR(BZ_OK); return; };
   }
}


/*---------------------------------------------------*/
void BZ_API(BZ2_bzWriteClose)
                  ( int*          bzerror, 
                    BZFILE*       b, 
                    int           abandon,
                    unsigned int* nbytes_in,
                    unsigned int* nbytes_out )
{
   BZ2_bzWriteClose64 ( bzerror, b, abandon, 
                        nbytes_in, NULL, nbytes_out, NULL );
}


void BZ_API(BZ2_bzWriteClose64)
                  ( int*          bzerror, 
                    BZFILE*       b, 
                    int           abandon,
                    unsigned int* nbytes_in_lo32,
                    unsigned int* nbytes_in_hi32,
                    unsigned int* nbytes_out_lo32,
                    unsigned int* nbytes_out_hi32 )
{
   Int32   n, n2, ret;
   bzFile* bzf = (bzFile*)b;

   if (bzf == NULL)
      { BZ_SETERR(BZ_OK); return; };
   if (!(bzf->writing))
      { BZ_SETERR(BZ_SEQUENCE_ERROR); return; };
   if (ferror(bzf->handle))
      { BZ_SETERR(BZ_IO_ERROR); return; };

   if (nbytes_in_lo32 != NULL) *nbytes_in_lo32 = 0;
   if (nbytes_in_hi32 != NULL) *nbytes_in_hi32 = 0;
   if (nbytes_out_lo32 != NULL) *nbytes_out_lo32 = 0;
   if (nbytes_out_hi32 != NULL) *nbytes_out_hi32 = 0;

   if ((!abandon) && bzf->lastErr == BZ_OK) {
      while (True) {
         bzf->strm.avail_out = BZ_MAX_UNUSED;
         bzf->strm.next_out = bzf->buf;
         ret = BZ2_bzCompress ( &(bzf->strm), BZ_FINISH );
         if (ret != BZ_FINISH_OK && ret != BZ_STREAM_END)
            { BZ_SETERR(ret); return; };

         if (bzf->strm.avail_out < BZ_MAX_UNUSED) {
            n = BZ_MAX_UNUSED - bzf->strm.avail_out;
            n2 = fwrite ( (void*)(bzf->buf), sizeof(UChar), 
                          n, bzf->handle );
            if (n != n2 || ferror(bzf->handle))
               { BZ_SETERR(BZ_IO_ERROR); return; };
         }

         if (ret == BZ_STREAM_END) break;
      }
   }

   if ( !abandon && !ferror ( bzf->handle ) ) {
      fflush ( bzf->handle );
      if (ferror(bzf->handle))
         { BZ_SETERR(BZ_IO_ERROR); return; };
   }

   if (nbytes_in_lo32 != NULL)
      *nbytes_in_lo32 = bzf->strm.total_in_lo32;
   if (nbytes_in_hi32 != NULL)
      *nbytes_in_hi32 = bzf->strm.total_in_hi32;
   if (nbytes_out_lo32 != NULL)
      *nbytes_out_lo32 = bzf->strm.total_out_lo32;
   if (nbytes_out_hi32 != NULL)
      *nbytes_out_hi32 = bzf->strm.total_out_hi32;

   BZ_SETERR(BZ_OK);
   BZ2_bzCompressEnd ( &(bzf->strm) );
   free ( bzf );
}


/*---------------------------------------------------*/
BZFILE* BZ_API(BZ2_bzReadOpen) 
                   ( int*  bzerror, 
                     FILE* f, 
                     int   verbosity,
                     int   small,
                     void* unused,
                     int   nUnused )
{
   bzFile* bzf = NULL;
   int     ret;

   BZ_SETERR(BZ_OK);

   if (f == NULL || 
       (small != 0 && small != 1) ||
       (verbosity < 0 || verbosity > 4) ||
       (unused == NULL && nUnused != 0) ||
       (unused != NULL && (nUnused < 0 || nUnused > BZ_MAX_UNUSED)))
      { BZ_SETERR(BZ_PARAM_ERROR); return NULL; };

   if (ferror(f))
      { BZ_SETERR(BZ_IO_ERROR); return NULL; };

   bzf = malloc ( sizeof(bzFile) );
   if (bzf == NULL) 
      { BZ_SETERR(BZ_MEM_ERROR); return NULL; };

   BZ_SETERR(BZ_OK);

   bzf->initialisedOk = False;
   bzf->handle        = f;
   bzf->bufN          = 0;
   bzf->writing       = False;
   bzf->strm.bzalloc  = NULL;
   bzf->strm.bzfree   = NULL;
   bzf->strm.opaque   = NULL;
   
   while (nUnused > 0) {
      bzf->buf[bzf->bufN] = *((UChar*)(unused)); bzf->bufN++;
      unused = ((void*)( 1 + ((UChar*)(unused))  ));
      nUnused--;
   }

   ret = BZ2_bzDecompressInit ( &(bzf->strm), verbosity, small );
   if (ret != BZ_OK)
      { BZ_SETERR(ret); free(bzf); return NULL; };

   bzf->strm.avail_in = bzf->bufN;
   bzf->strm.next_in  = bzf->buf;

   bzf->initialisedOk = True;
   return bzf;   
}


/*---------------------------------------------------*/
void BZ_API(BZ2_bzReadClose) ( int *bzerror, BZFILE *b )
{
   bzFile* bzf = (bzFile*)b;

   BZ_SETERR(BZ_OK);
   if (bzf == NULL)
      { BZ_SETERR(BZ_OK); return; };

   if (bzf->writing)
      { BZ_SETERR(BZ_SEQUENCE_ERROR); return; };

   if (bzf->initialisedOk)
      (void)BZ2_bzDecompressEnd ( &(bzf->strm) );
   free ( bzf );
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzRead) 
           ( int*    bzerror, 
             BZFILE* b, 
             void*   buf, 
             int     len )
{
   AOCL_SETUP_NATIVE();
   Int32   n, ret;
   bzFile* bzf = (bzFile*)b;

   BZ_SETERR(BZ_OK);

   if (bzf == NULL || buf == NULL || len < 0)
      { BZ_SETERR(BZ_PARAM_ERROR); return 0; };

   if (bzf->writing)
      { BZ_SETERR(BZ_SEQUENCE_ERROR); return 0; };

   if (len == 0)
      { BZ_SETERR(BZ_OK); return 0; };

   bzf->strm.avail_out = len;
   bzf->strm.next_out = buf;

   while (True) {

      if (ferror(bzf->handle)) 
         { BZ_SETERR(BZ_IO_ERROR); return 0; };

      if (bzf->strm.avail_in == 0 && !myfeof(bzf->handle)) {
         n = fread ( bzf->buf, sizeof(UChar), 
                     BZ_MAX_UNUSED, bzf->handle );
         if (ferror(bzf->handle))
            { BZ_SETERR(BZ_IO_ERROR); return 0; };
         bzf->bufN = n;
         bzf->strm.avail_in = bzf->bufN;
         bzf->strm.next_in = bzf->buf;
      }

      ret = BZ2_bzDecompress ( &(bzf->strm) );

      if (ret != BZ_OK && ret != BZ_STREAM_END)
         { BZ_SETERR(ret); return 0; };

      if (ret == BZ_OK && myfeof(bzf->handle) && 
          bzf->strm.avail_in == 0 && bzf->strm.avail_out > 0)
         { BZ_SETERR(BZ_UNEXPECTED_EOF); return 0; };

      if (ret == BZ_STREAM_END)
         { BZ_SETERR(BZ_STREAM_END);
           return len - bzf->strm.avail_out; };
      if (bzf->strm.avail_out == 0)
         { BZ_SETERR(BZ_OK); return len; };
      
   }

   return 0; /*not reached*/
}


/*---------------------------------------------------*/
void BZ_API(BZ2_bzReadGetUnused) 
                     ( int*    bzerror, 
                       BZFILE* b, 
                       void**  unused, 
                       int*    nUnused )
{
   bzFile* bzf = (bzFile*)b;
   if (bzf == NULL)
      { BZ_SETERR(BZ_PARAM_ERROR); return; };
   if (bzf->lastErr != BZ_STREAM_END)
      { BZ_SETERR(BZ_SEQUENCE_ERROR); return; };
   if (unused == NULL || nUnused == NULL)
      { BZ_SETERR(BZ_PARAM_ERROR); return; };

   BZ_SETERR(BZ_OK);
   *nUnused = bzf->strm.avail_in;
   *unused = bzf->strm.next_in;
}
#endif


/*---------------------------------------------------*/
/*--- Misc convenience stuff                      ---*/
/*---------------------------------------------------*/

/*---------------------------------------------------*/
int BZ_API(BZ2_bzBuffToBuffCompress) 
                         ( char*         dest, 
                           unsigned int* destLen,
                           char*         source, 
                           unsigned int  sourceLen,
                           int           blockSize100k, 
                           int           verbosity, 
                           int           workFactor )
{
   AOCL_SETUP_NATIVE();
   bz_stream strm;
   LOG_UNFORMATTED(TRACE, logCtx, "Enter");
   int ret;

   if (dest == NULL || destLen == NULL || 
       source == NULL ||
       blockSize100k < 1 || blockSize100k > 9 ||
       verbosity < 0 || verbosity > 4 ||
       workFactor < 0 || workFactor > 250)
   {
      LOG_UNFORMATTED(INFO, logCtx, "Exit");
      return BZ_PARAM_ERROR;
   }

   if (workFactor == 0) workFactor = 30;
   strm.bzalloc = NULL;
   strm.bzfree = NULL;
   strm.opaque = NULL;
   ret = BZ2_bzCompressInit ( &strm, blockSize100k, 
                              verbosity, workFactor );
   if (ret != BZ_OK)
   {
      LOG_UNFORMATTED(INFO, logCtx, "Exit");
      return ret;
   }

   strm.next_in = source;
   strm.next_out = dest;
   strm.avail_in = sourceLen;
   strm.avail_out = *destLen;

   ret = BZ2_bzCompress ( &strm, BZ_FINISH );
   if (ret == BZ_FINISH_OK) goto output_overflow;
   if (ret != BZ_STREAM_END) goto errhandler;

   /* normal termination */
   *destLen -= strm.avail_out;   
   BZ2_bzCompressEnd ( &strm );
   LOG_UNFORMATTED(INFO, logCtx, "Exit");
   return BZ_OK;

   output_overflow:
   BZ2_bzCompressEnd ( &strm );
   LOG_UNFORMATTED(INFO, logCtx, "Exit");
   return BZ_OUTBUFF_FULL;

   errhandler:
   BZ2_bzCompressEnd ( &strm );
   LOG_UNFORMATTED(INFO, logCtx, "Exit");
   return ret;
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzBuffToBuffDecompress) 
                           ( char*         dest, 
                             unsigned int* destLen,
                             char*         source, 
                             unsigned int  sourceLen,
                             int           small,
                             int           verbosity )
{
   AOCL_SETUP_NATIVE();
   bz_stream strm;
   LOG_UNFORMATTED(TRACE, logCtx, "Enter");
   int ret;

   if (dest == NULL || destLen == NULL || 
       source == NULL ||
       (small != 0 && small != 1) ||
       verbosity < 0 || verbosity > 4) 
       {
          LOG_UNFORMATTED(INFO, logCtx, "Exit");
          return BZ_PARAM_ERROR;
       }

   strm.bzalloc = NULL;
   strm.bzfree = NULL;
   strm.opaque = NULL;
   ret = BZ2_bzDecompressInit ( &strm, verbosity, small );
   if (ret != BZ_OK)
   {
      LOG_UNFORMATTED(INFO, logCtx, "Exit");
      return ret;
   }

   strm.next_in = source;
   strm.next_out = dest;
   strm.avail_in = sourceLen;
   strm.avail_out = *destLen;

   ret = BZ2_bzDecompress ( &strm );
   if (ret == BZ_OK) goto output_overflow_or_eof;
   if (ret != BZ_STREAM_END) goto errhandler;

   /* normal termination */
   *destLen -= strm.avail_out;
   BZ2_bzDecompressEnd ( &strm );
   LOG_UNFORMATTED(INFO, logCtx, "Exit");
   return BZ_OK;

   output_overflow_or_eof:
   if (strm.avail_out > 0) {
      BZ2_bzDecompressEnd ( &strm );
      LOG_UNFORMATTED(INFO, logCtx, "Exit");
      return BZ_UNEXPECTED_EOF;
   } else {
      BZ2_bzDecompressEnd ( &strm );
      LOG_UNFORMATTED(INFO, logCtx, "Exit");
      return BZ_OUTBUFF_FULL;
   };      

   errhandler:
   BZ2_bzDecompressEnd ( &strm );
   
   LOG_UNFORMATTED(INFO, logCtx, "Exit");
   return ret; 
}


/*---------------------------------------------------*/
/*--
   Code contributed by Yoshioka Tsuneo (tsuneo@rr.iij4u.or.jp)
   to support better zlib compatibility.
   This code is not _officially_ part of libbzip2 (yet);
   I haven't tested it, documented it, or considered the
   threading-safeness of it.
   If this code breaks, please contact both Yoshioka and me.
--*/
/*---------------------------------------------------*/

/*---------------------------------------------------*/
/*--
   return version like "0.9.5d, 4-Sept-1999".
--*/
const char * BZ_API(BZ2_bzlibVersion)(void)
{
   return BZ_VERSION;
}


#ifndef BZ_NO_STDIO
/*---------------------------------------------------*/

#if defined(_WIN32) || defined(OS2) || defined(MSDOS)
#   include <fcntl.h>
#   include <io.h>
#ifdef ENABLE_STRICT_WARNINGS
#   define SET_BINARY_MODE(file) _setmode(_fileno(file),O_BINARY)
#else
#   define SET_BINARY_MODE(file) setmode(fileno(file),O_BINARY)
#endif
#else
#   define SET_BINARY_MODE(file)
#endif
static
BZFILE * bzopen_or_bzdopen
               ( const char *path,   /* no use when bzdopen */
                 int fd,             /* no use when bzdopen */
                 const char *mode,
                 int open_mode)      /* bzopen: 0, bzdopen:1 */
{
   int    bzerr;
   char   unused[BZ_MAX_UNUSED];
   int    blockSize100k = 9;
   int    writing       = 0;
   char   mode2[10]     = "";
   FILE   *fp           = NULL;
   BZFILE *bzfp         = NULL;
   int    verbosity     = 0;
   int    workFactor    = 30;
   int    smallMode     = 0;
   int    nUnused       = 0; 

   if (mode == NULL) return NULL;
   while (*mode) {
      switch (*mode) {
      case 'r':
         writing = 0; break;
      case 'w':
         writing = 1; break;
      case 's':
         smallMode = 1; break;
      default:
         if (isdigit((int)(*mode))) {
            blockSize100k = *mode-BZ_HDR_0;
         }
      }
      mode++;
   }
   strcat(mode2, writing ? "w" : "r" );
   strcat(mode2,"b");   /* binary mode */

   if (open_mode==0) {
      if (path==NULL || strcmp(path,"")==0) {
        fp = (writing ? stdout : stdin);
        SET_BINARY_MODE(fp);
      } else {
        fp = fopen(path,mode2);
      }
   } else {
#ifdef BZ_STRICT_ANSI
      fp = NULL;
#else
#ifdef _WIN32
#ifdef ENABLE_STRICT_WARNINGS
      fp = _fdopen(fd,mode2);
#else
      fp = fdopen(fd, mode2);
#endif
#else
      fp = fdopen(fd, mode2);
#endif

#endif
   }
   if (fp == NULL) return NULL;

   if (writing) {
      /* Guard against total chaos and anarchy -- JRS */
      if (blockSize100k < 1) blockSize100k = 1;
      if (blockSize100k > 9) blockSize100k = 9; 
      bzfp = BZ2_bzWriteOpen(&bzerr,fp,blockSize100k,
                             verbosity,workFactor);
   } else {
      bzfp = BZ2_bzReadOpen(&bzerr,fp,verbosity,smallMode,
                            unused,nUnused);
   }
   if (bzfp == NULL) {
      if (fp != stdin && fp != stdout) fclose(fp);
      return NULL;
   }
   return bzfp;
}


/*---------------------------------------------------*/
/*--
   open file for read or write.
      ex) bzopen("file","w9")
      case path="" or NULL => use stdin or stdout.
--*/
BZFILE * BZ_API(BZ2_bzopen)
               ( const char *path,
                 const char *mode )
{
   return bzopen_or_bzdopen(path,-1,mode,/*bzopen*/0);
}


/*---------------------------------------------------*/
BZFILE * BZ_API(BZ2_bzdopen)
               ( int fd,
                 const char *mode )
{
   return bzopen_or_bzdopen(NULL,fd,mode,/*bzdopen*/1);
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzread) (BZFILE* b, void* buf, int len )
{
   int bzerr, nread;
   if (((bzFile*)b)->lastErr == BZ_STREAM_END) return 0;
   nread = BZ2_bzRead(&bzerr,b,buf,len);
   if (bzerr == BZ_OK || bzerr == BZ_STREAM_END) {
      return nread;
   } else {
      return -1;
   }
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzwrite) (BZFILE* b, void* buf, int len )
{
   int bzerr;

   BZ2_bzWrite(&bzerr,b,buf,len);
   if(bzerr == BZ_OK){
      return len;
   }else{
      return -1;
   }
}


/*---------------------------------------------------*/
int BZ_API(BZ2_bzflush) (BZFILE *b)
{
   /* do nothing now... */
   return 0;
}


/*---------------------------------------------------*/
void BZ_API(BZ2_bzclose) (BZFILE* b)
{
   int bzerr;
   FILE *fp;
   
   if (b==NULL) {return;}
   fp = ((bzFile *)b)->handle;
   if(((bzFile*)b)->writing){
      BZ2_bzWriteClose(&bzerr,b,0,NULL,NULL);
      if(bzerr != BZ_OK){
         BZ2_bzWriteClose(NULL,b,1,NULL,NULL);
      }
   }else{
      BZ2_bzReadClose(&bzerr,b);
   }
   if(fp!=stdin && fp!=stdout){
      fclose(fp);
   }
}


/*---------------------------------------------------*/
/*--
   return last error code 
--*/
static const char *bzerrorstrings[] = {
       "OK"
      ,"SEQUENCE_ERROR"
      ,"PARAM_ERROR"
      ,"MEM_ERROR"
      ,"DATA_ERROR"
      ,"DATA_ERROR_MAGIC"
      ,"IO_ERROR"
      ,"UNEXPECTED_EOF"
      ,"OUTBUFF_FULL"
      ,"CONFIG_ERROR"
      ,"???"   /* for future */
      ,"???"   /* for future */
      ,"???"   /* for future */
      ,"???"   /* for future */
      ,"???"   /* for future */
      ,"???"   /* for future */
};


const char * BZ_API(BZ2_bzerror) (BZFILE *b, int *errnum)
{
   int err = ((bzFile *)b)->lastErr;

   if(err>0) err = 0;
   *errnum = err;
   return bzerrorstrings[err*-1];
}
#endif


/*-------------------------------------------------------------*/
/*--- end                                           bzlib.c ---*/
/*-------------------------------------------------------------*/
