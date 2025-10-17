/**
 * Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.
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

 /** @file aoclPrefix.h
 *
 *  @brief Prefix library symbols to avoid conflicts.
 *
 *  @author Ravi Jangra
 */

 #ifndef __AOCL_PREFIX_H
 #define __AOCL_PREFIX_H

 #ifdef AOCL_LLC_PREFIX
 #define AOCL_PREFIX_SET

 /**** ZLIB ****/
 /* all linked symbols and init macros */
#  define _dist_code            AOCL_LLC__dist_code
#  define _length_code          AOCL_LLC__length_code
#  define _tr_align             AOCL_LLC__tr_align
#  define _tr_flush_bits        AOCL_LLC__tr_flush_bits
#  define _tr_flush_block       AOCL_LLC__tr_flush_block
#  define _tr_init              AOCL_LLC__tr_init
#  define _tr_stored_block      AOCL_LLC__tr_stored_block
#  define _tr_tally             AOCL_LLC__tr_tally
#  define adler32               AOCL_LLC_adler32
#  define adler32_combine       AOCL_LLC_adler32_combine
#  define adler32_combine64     AOCL_LLC_adler32_combine64
#  define adler32_z             AOCL_LLC_adler32_z
#  ifndef Z_SOLO
#    define compress              AOCL_LLC_compress
#    define compress2             AOCL_LLC_compress2
#    define compressBound         AOCL_LLC_compressBound
#  endif
#  define crc32                 AOCL_LLC_crc32
#  define crc32_combine         AOCL_LLC_crc32_combine
#  define crc32_combine64       AOCL_LLC_crc32_combine64
#  define crc32_combine_gen     AOCL_LLC_crc32_combine_gen
#  define crc32_combine_gen64   AOCL_LLC_crc32_combine_gen64
#  define crc32_combine_op      AOCL_LLC_crc32_combine_op
#  define crc32_z               AOCL_LLC_crc32_z
#  define deflate               AOCL_LLC_deflate
#  define deflate_medium        AOCL_LLC_deflate_medium
#  define deflate_quick         AOCL_LLC_deflate_quick
#  define deflateBound          AOCL_LLC_deflateBound
#  define deflateCopy           AOCL_LLC_deflateCopy
#  define deflateEnd            AOCL_LLC_deflateEnd
#  define deflateGetDictionary  AOCL_LLC_deflateGetDictionary
#  define deflateInit           AOCL_LLC_deflateInit
#  define deflateInit2          AOCL_LLC_deflateInit2
#  define deflateInit2_         AOCL_LLC_deflateInit2_
#  define deflateInit_          AOCL_LLC_deflateInit_
#  define deflateParams         AOCL_LLC_deflateParams
#  define deflatePending        AOCL_LLC_deflatePending
#  define deflatePrime          AOCL_LLC_deflatePrime
#  define deflateReset          AOCL_LLC_deflateReset
#  define deflateResetKeep      AOCL_LLC_deflateResetKeep
#  define deflateSetDictionary  AOCL_LLC_deflateSetDictionary
#  define deflateSetHeader      AOCL_LLC_deflateSetHeader
#  define deflateTune           AOCL_LLC_deflateTune
#  define deflate_copyright     AOCL_LLC_deflate_copyright
#  define get_crc_table         AOCL_LLC_get_crc_table
#  ifndef Z_SOLO
#    define gz_error              AOCL_LLC_gz_error
#    define gz_intmax             AOCL_LLC_gz_intmax
#    define gz_strwinerror        AOCL_LLC_gz_strwinerror
#    define gzbuffer              AOCL_LLC_gzbuffer
#    define gzclearerr            AOCL_LLC_gzclearerr
#    define gzclose               AOCL_LLC_gzclose
#    define gzclose_r             AOCL_LLC_gzclose_r
#    define gzclose_w             AOCL_LLC_gzclose_w
#    define gzdirect              AOCL_LLC_gzdirect
#    define gzdopen               AOCL_LLC_gzdopen
#    define gzeof                 AOCL_LLC_gzeof
#    define gzerror               AOCL_LLC_gzerror
#    define gzflush               AOCL_LLC_gzflush
#    define gzfread               AOCL_LLC_gzfread
#    define gzfwrite              AOCL_LLC_gzfwrite
//#    define gzgetc                AOCL_LLC_gzgetc
#    define gzgetc_               AOCL_LLC_gzgetc_
#    define gzgets                AOCL_LLC_gzgets
#    define gzoffset              AOCL_LLC_gzoffset
#    define gzoffset64            AOCL_LLC_gzoffset64
#    define gzopen                AOCL_LLC_gzopen
#    define gzopen64              AOCL_LLC_gzopen64
#    ifdef _WIN32
#      define gzopen_w              AOCL_LLC_gzopen_w
#    endif
#    define gzprintf              AOCL_LLC_gzprintf
#    define gzputc                AOCL_LLC_gzputc
#    define gzputs                AOCL_LLC_gzputs
#    define gzread                AOCL_LLC_gzread
#    define gzrewind              AOCL_LLC_gzrewind
#    define gzseek                AOCL_LLC_gzseek
#    define gzseek64              AOCL_LLC_gzseek64
#    define gzsetparams           AOCL_LLC_gzsetparams
#    define gztell                AOCL_LLC_gztell
#    define gztell64              AOCL_LLC_gztell64
#    define gzungetc              AOCL_LLC_gzungetc
#    define gzvprintf             AOCL_LLC_gzvprintf
#    define gzwrite               AOCL_LLC_gzwrite
#  endif
#  define inflate               AOCL_LLC_inflate
#  define inflateBack           AOCL_LLC_inflateBack
#  define inflateBackEnd        AOCL_LLC_inflateBackEnd
#  define inflateBackInit       AOCL_LLC_inflateBackInit
#  define inflateBackInit_      AOCL_LLC_inflateBackInit_
#  define inflateCodesUsed      AOCL_LLC_inflateCodesUsed
#  define inflateCopy           AOCL_LLC_inflateCopy
#  define inflateEnd            AOCL_LLC_inflateEnd
#  define inflateGetDictionary  AOCL_LLC_inflateGetDictionary
#  define inflateGetHeader      AOCL_LLC_inflateGetHeader
#  define inflateInit           AOCL_LLC_inflateInit
#  define inflateInit2          AOCL_LLC_inflateInit2
#  define inflateInit2_         AOCL_LLC_inflateInit2_
#  define inflateInit_          AOCL_LLC_inflateInit_
#  define inflateMark           AOCL_LLC_inflateMark
#  define inflatePrime          AOCL_LLC_inflatePrime
#  define inflateReset          AOCL_LLC_inflateReset
#  define inflateReset2         AOCL_LLC_inflateReset2
#  define inflateResetKeep      AOCL_LLC_inflateResetKeep
#  define inflateSetDictionary  AOCL_LLC_inflateSetDictionary
#  define inflateSync           AOCL_LLC_inflateSync
#  define inflateSyncPoint      AOCL_LLC_inflateSyncPoint
#  define inflateUndermine      AOCL_LLC_inflateUndermine
#  define inflateValidate       AOCL_LLC_inflateValidate
#  define inflate_copyright     AOCL_LLC_inflate_copyright
#  define inflate_fast          AOCL_LLC_inflate_fast
#  define inflate_table         AOCL_LLC_inflate_table
#  ifndef Z_SOLO
#    define uncompress            AOCL_LLC_uncompress
#    define uncompress2           AOCL_LLC_uncompress2
#  endif
#  define zError                AOCL_LLC_zError
#  define z_errmsg              AOCL_LLC_z_errmsg
#  ifndef Z_SOLO
#    define zcalloc               AOCL_LLC_zcalloc
#    define zcfree                AOCL_LLC_zcfree
#  endif
#  define zlibCompileFlags      AOCL_LLC_zlibCompileFlags
#  define zlibVersion           AOCL_LLC_zlibVersion
#  define static_ltree          AOCL_LLC_static_ltree

/* all zlib typedefs in zlib.h and zconf.h */
#  define Byte                  AOCL_LLC_Byte
#  define Bytef                 AOCL_LLC_Bytef
#  define alloc_func            AOCL_LLC_alloc_func
#  define charf                 AOCL_LLC_charf
#  define free_func             AOCL_LLC_free_func
#  ifndef Z_SOLO
#    define gzFile                AOCL_LLC_gzFile
#  endif
#  define gz_header             AOCL_LLC_gz_header
#  define gz_headerp            AOCL_LLC_gz_headerp
#  define in_func               AOCL_LLC_in_func
#  define intf                  AOCL_LLC_intf
#  define out_func              AOCL_LLC_out_func
#  define uInt                  AOCL_LLC_uInt
#  define uIntf                 AOCL_LLC_uIntf
#  define uLong                 AOCL_LLC_uLong
#  define uLongf                AOCL_LLC_uLongf
#  define voidp                 AOCL_LLC_voidp
#  define voidpc                AOCL_LLC_voidpc
#  define voidpf                AOCL_LLC_voidpf
#  define z_stream              AOCL_LLC_z_stream
#  define z_streamp             AOCL_LLC_z_streamp

/* all zlib structs in zlib.h and zconf.h */
#  define gz_header_s           AOCL_LLC_gz_header_s
#  define internal_state        AOCL_LLC_internal_state
#  define z_stream_s            AOCL_LLC_z_stream_s

/**** ZLIB ****/

#endif /* AOCL_LLC_PREFIX */
#endif
