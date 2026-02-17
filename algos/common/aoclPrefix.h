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

/**** LZ4 ****/
/* LZ4 core functions */
#  define LZ4_compress                    AOCL_LLC_LZ4_compress
#  define LZ4_decompress_safe             AOCL_LLC_LZ4_decompress_safe
#  define LZ4_decompress_fast             AOCL_LLC_LZ4_decompress_fast
#  define LZ4_compress_default            AOCL_LLC_LZ4_compress_default
#  define LZ4_compress_fast               AOCL_LLC_LZ4_compress_fast
#  define LZ4_compress_fast_extState      AOCL_LLC_LZ4_compress_fast_extState
#  define LZ4_compress_fast_extState_fastReset  AOCL_LLC_LZ4_compress_fast_extState_fastReset
#  define LZ4_compress_destSize           AOCL_LLC_LZ4_compress_destSize
#  define LZ4_compressBound               AOCL_LLC_LZ4_compressBound
#  define LZ4_sizeofState                 AOCL_LLC_LZ4_sizeofState
#  define LZ4_decompress_safe_partial     AOCL_LLC_LZ4_decompress_safe_partial

/* LZ4 streaming functions */
#  define LZ4_createStream                AOCL_LLC_LZ4_createStream
#  define LZ4_freeStream                  AOCL_LLC_LZ4_freeStream
#  define LZ4_resetStream                 AOCL_LLC_LZ4_resetStream
#  define LZ4_resetStream_fast            AOCL_LLC_LZ4_resetStream_fast
#  define LZ4_loadDict                    AOCL_LLC_LZ4_loadDict
#  define LZ4_compress_fast_continue      AOCL_LLC_LZ4_compress_fast_continue
#  define LZ4_saveDict                    AOCL_LLC_LZ4_saveDict
#  define LZ4_createStreamDecode          AOCL_LLC_LZ4_createStreamDecode
#  define LZ4_freeStreamDecode            AOCL_LLC_LZ4_freeStreamDecode
#  define LZ4_setStreamDecode             AOCL_LLC_LZ4_setStreamDecode
#  define LZ4_decompress_safe_continue    AOCL_LLC_LZ4_decompress_safe_continue
#  define LZ4_decompress_fast_continue    AOCL_LLC_LZ4_decompress_fast_continue
#  define LZ4_decompress_safe_usingDict   AOCL_LLC_LZ4_decompress_safe_usingDict
#  define LZ4_decompress_fast_usingDict   AOCL_LLC_LZ4_decompress_fast_usingDict

/* LZ4 version and utility functions */
#  define LZ4_versionNumber               AOCL_LLC_LZ4_versionNumber
#  define LZ4_versionString               AOCL_LLC_LZ4_versionString

/* LZ4 HC (High Compression) functions */
#  define LZ4_compress_HC                 AOCL_LLC_LZ4_compress_HC
#  define LZ4_compress_HC_extStateHC      AOCL_LLC_LZ4_compress_HC_extStateHC
#  define LZ4_compress_HC_extStateHC_fastReset  AOCL_LLC_LZ4_compress_HC_extStateHC_fastReset
#  define LZ4_sizeofStateHC               AOCL_LLC_LZ4_sizeofStateHC
#  define LZ4_compress_HC_destSize        AOCL_LLC_LZ4_compress_HC_destSize
#  define LZ4_createStreamHC              AOCL_LLC_LZ4_createStreamHC
#  define LZ4_freeStreamHC                AOCL_LLC_LZ4_freeStreamHC
#  define LZ4_resetStreamHC               AOCL_LLC_LZ4_resetStreamHC
#  define LZ4_resetStreamHC_fast          AOCL_LLC_LZ4_resetStreamHC_fast
#  define LZ4_loadDictHC                  AOCL_LLC_LZ4_loadDictHC
#  define LZ4_compress_HC_continue        AOCL_LLC_LZ4_compress_HC_continue
#  define LZ4_compress_HC_continue_destSize  AOCL_LLC_LZ4_compress_HC_continue_destSize
#  define LZ4_saveDictHC                  AOCL_LLC_LZ4_saveDictHC
#  define LZ4_initStreamHC                AOCL_LLC_LZ4_initStreamHC
#  define LZ4_setCompressionLevel         AOCL_LLC_LZ4_setCompressionLevel
#  define LZ4_favorDecompressionSpeed     AOCL_LLC_LZ4_favorDecompressionSpeed

/* LZ4 deprecated/legacy functions */
#  define LZ4_compress_limitedOutput      AOCL_LLC_LZ4_compress_limitedOutput
#  define LZ4_compress_limitedOutput_withState  AOCL_LLC_LZ4_compress_limitedOutput_withState
#  define LZ4_compress_withState          AOCL_LLC_LZ4_compress_withState
#  define LZ4_compress_limitedOutput_continue  AOCL_LLC_LZ4_compress_limitedOutput_continue
#  define LZ4_compress_continue           AOCL_LLC_LZ4_compress_continue
#  define LZ4_uncompress                  AOCL_LLC_LZ4_uncompress
#  define LZ4_uncompress_unknownOutputSize  AOCL_LLC_LZ4_uncompress_unknownOutputSize
#  define LZ4_create                      AOCL_LLC_LZ4_create
#  define LZ4_sizeofStreamState           AOCL_LLC_LZ4_sizeofStreamState
#  define LZ4_resetStreamState            AOCL_LLC_LZ4_resetStreamState
#  define LZ4_slideInputBuffer            AOCL_LLC_LZ4_slideInputBuffer
#  define LZ4_compressHC                  AOCL_LLC_LZ4_compressHC
#  define LZ4_compressHC_limitedOutput    AOCL_LLC_LZ4_compressHC_limitedOutput
#  define LZ4_compressHC2                 AOCL_LLC_LZ4_compressHC2
#  define LZ4_compressHC2_limitedOutput   AOCL_LLC_LZ4_compressHC2_limitedOutput
#  define LZ4_compressHC_withStateHC      AOCL_LLC_LZ4_compressHC_withStateHC
#  define LZ4_compressHC_limitedOutput_withStateHC  AOCL_LLC_LZ4_compressHC_limitedOutput_withStateHC
#  define LZ4_compressHC2_withStateHC     AOCL_LLC_LZ4_compressHC2_withStateHC
#  define LZ4_compressHC2_limitedOutput_withStateHC  AOCL_LLC_LZ4_compressHC2_limitedOutput_withStateHC
#  define LZ4_compressHC_continue         AOCL_LLC_LZ4_compressHC_continue
#  define LZ4_compressHC_limitedOutput_continue  AOCL_LLC_LZ4_compressHC_limitedOutput_continue
#  define LZ4_compressHC2_continue        AOCL_LLC_LZ4_compressHC2_continue
#  define LZ4_compressHC2_limitedOutput_continue  AOCL_LLC_LZ4_compressHC2_limitedOutput_continue
#  define LZ4_createHC                    AOCL_LLC_LZ4_createHC
#  define LZ4_freeHC                      AOCL_LLC_LZ4_freeHC
#  define LZ4_slideInputBufferHC          AOCL_LLC_LZ4_slideInputBufferHC
#  define LZ4_sizeofStreamStateHC         AOCL_LLC_LZ4_sizeofStreamStateHC
#  define LZ4_resetStreamStateHC          AOCL_LLC_LZ4_resetStreamStateHC

/* LZ4 internal/static functions */
#  define LZ4_initStream                  AOCL_LLC_LZ4_initStream
#  define LZ4_attach_dictionary           AOCL_LLC_LZ4_attach_dictionary
#  define LZ4_compress_destSize_extState  AOCL_LLC_LZ4_compress_destSize_extState
#  define LZ4_compress_forceExtDict       AOCL_LLC_LZ4_compress_forceExtDict
#  define LZ4_decompress_safe_withPrefix64k  AOCL_LLC_LZ4_decompress_safe_withPrefix64k
#  define LZ4_decompress_fast_withPrefix64k  AOCL_LLC_LZ4_decompress_fast_withPrefix64k
#  define LZ4_decompress_safe_forceExtDict  AOCL_LLC_LZ4_decompress_safe_forceExtDict
#  define LZ4_decompress_safe_partial_forceExtDict  AOCL_LLC_LZ4_decompress_safe_partial_forceExtDict
#  define LZ4_decompress_safe_partial_usingDict  AOCL_LLC_LZ4_decompress_safe_partial_usingDict
#  define LZ4_decoderRingBufferSize       AOCL_LLC_LZ4_decoderRingBufferSize
#  define LZ4_compressBound_st            AOCL_LLC_LZ4_compressBound_st
#  define LZ4_loadDict_internal           AOCL_LLC_LZ4_loadDict_internal
#  define LZ4_loadDictSlow                AOCL_LLC_LZ4_loadDictSlow
#  define LZ4_attach_HC_dictionary        AOCL_LLC_LZ4_attach_HC_dictionary
#  define LZ4HC_searchExtDict             AOCL_LLC_LZ4HC_searchExtDict

/* LZ4 internal implementation functions (usually hidden but may be exported) */
#  define LZ4_compress_fast_continue_internal  AOCL_LLC_LZ4_compress_fast_continue_internal
#  define LZ4_compress_fast_extState_internal  AOCL_LLC_LZ4_compress_fast_extState_internal
#  define LZ4_compress_HC_destSize_internal  AOCL_LLC_LZ4_compress_HC_destSize_internal
#  define LZ4_compress_HC_extStateHC_fastReset_internal  AOCL_LLC_LZ4_compress_HC_extStateHC_fastReset_internal
#  define LZ4_compress_HC_extStateHC_internal  AOCL_LLC_LZ4_compress_HC_extStateHC_internal
#  define LZ4_compress_HC_internal        AOCL_LLC_LZ4_compress_HC_internal
#  define LZ4_decompress_safe_doubleDict_internal  AOCL_LLC_LZ4_decompress_safe_doubleDict_internal
#  define LZ4_decompress_safe_forceExtDict_internal  AOCL_LLC_LZ4_decompress_safe_forceExtDict_internal
#  define LZ4_decompress_safe_partial_forceExtDict_internal  AOCL_LLC_LZ4_decompress_safe_partial_forceExtDict_internal
#  define LZ4_decompress_safe_partial_internal  AOCL_LLC_LZ4_decompress_safe_partial_internal
#  define LZ4_decompress_safe_withPrefix64k_internal  AOCL_LLC_LZ4_decompress_safe_withPrefix64k_internal

/* LZ4 Frame internal functions */
#  define LZ4F_getErrorCode               AOCL_LLC_LZ4F_getErrorCode
#  define LZ4F_getBlockSize               AOCL_LLC_LZ4F_getBlockSize
#  define LZ4F_createCDict_advanced       AOCL_LLC_LZ4F_createCDict_advanced
#  define LZ4F_createCompressionContext_advanced  AOCL_LLC_LZ4F_createCompressionContext_advanced
#  define LZ4F_compressBegin_internal     AOCL_LLC_LZ4F_compressBegin_internal
#  define LZ4F_compressBegin_usingDict    AOCL_LLC_LZ4F_compressBegin_usingDict
#  define LZ4F_compressBegin_usingDictOnce  AOCL_LLC_LZ4F_compressBegin_usingDictOnce
#  define LZ4F_compressFrame_usingCDict   AOCL_LLC_LZ4F_compressFrame_usingCDict
#  define LZ4F_createDecompressionContext_advanced  AOCL_LLC_LZ4F_createDecompressionContext_advanced
#  define LZ4F_headerSize                 AOCL_LLC_LZ4F_headerSize

/* LZ4 Frame functions */
#  define LZ4F_isError                    AOCL_LLC_LZ4F_isError
#  define LZ4F_getErrorName               AOCL_LLC_LZ4F_getErrorName
#  define LZ4F_compressionLevel_max       AOCL_LLC_LZ4F_compressionLevel_max
#  define LZ4F_compressFrameBound         AOCL_LLC_LZ4F_compressFrameBound
#  define LZ4F_compressFrame              AOCL_LLC_LZ4F_compressFrame
#  define LZ4F_getVersion                 AOCL_LLC_LZ4F_getVersion
#  define LZ4F_createCompressionContext   AOCL_LLC_LZ4F_createCompressionContext
#  define LZ4F_freeCompressionContext     AOCL_LLC_LZ4F_freeCompressionContext
#  define LZ4F_compressBegin              AOCL_LLC_LZ4F_compressBegin
#  define LZ4F_compressBound              AOCL_LLC_LZ4F_compressBound
#  define LZ4F_compressUpdate             AOCL_LLC_LZ4F_compressUpdate
#  define LZ4F_flush                      AOCL_LLC_LZ4F_flush
#  define LZ4F_compressEnd                AOCL_LLC_LZ4F_compressEnd
#  define LZ4F_createDecompressionContext AOCL_LLC_LZ4F_createDecompressionContext
#  define LZ4F_freeDecompressionContext   AOCL_LLC_LZ4F_freeDecompressionContext
#  define LZ4F_getFrameInfo               AOCL_LLC_LZ4F_getFrameInfo
#  define LZ4F_decompress                 AOCL_LLC_LZ4F_decompress
#  define LZ4F_resetDecompressionContext  AOCL_LLC_LZ4F_resetDecompressionContext
#  define LZ4F_createCDict                AOCL_LLC_LZ4F_createCDict
#  define LZ4F_freeCDict                  AOCL_LLC_LZ4F_freeCDict
#  define LZ4F_compressBegin_usingCDict   AOCL_LLC_LZ4F_compressBegin_usingCDict
#  define LZ4F_decompress_usingDict       AOCL_LLC_LZ4F_decompress_usingDict
#  define LZ4F_uncompressedUpdate         AOCL_LLC_LZ4F_uncompressedUpdate

/* LZ4 public typedefs and structs (must be prefixed to avoid conflicts) */
#  define LZ4_stream_u                    AOCL_LLC_LZ4_stream_u
#  define LZ4_stream_t                    AOCL_LLC_LZ4_stream_t
#  define LZ4_stream_t_internal           AOCL_LLC_LZ4_stream_t_internal
#  define LZ4_streamDecode_u              AOCL_LLC_LZ4_streamDecode_u
#  define LZ4_streamDecode_t              AOCL_LLC_LZ4_streamDecode_t
#  define LZ4_streamHC_u                  AOCL_LLC_LZ4_streamHC_u
#  define LZ4_streamHC_t                  AOCL_LLC_LZ4_streamHC_t
#  define LZ4HC_CCtx_internal             AOCL_LLC_LZ4HC_CCtx_internal

/* LZ4 Frame typedefs and structs */
#  define LZ4F_errorCode_t                AOCL_LLC_LZ4F_errorCode_t
#  define LZ4F_blockSizeID_t              AOCL_LLC_LZ4F_blockSizeID_t
#  define LZ4F_blockMode_t                AOCL_LLC_LZ4F_blockMode_t
#  define LZ4F_contentChecksum_t          AOCL_LLC_LZ4F_contentChecksum_t
#  define LZ4F_blockChecksum_t            AOCL_LLC_LZ4F_blockChecksum_t
#  define LZ4F_frameType_t                AOCL_LLC_LZ4F_frameType_t
#  define LZ4F_frameInfo_t                AOCL_LLC_LZ4F_frameInfo_t
#  define LZ4F_preferences_t              AOCL_LLC_LZ4F_preferences_t
#  define LZ4F_compressOptions_t          AOCL_LLC_LZ4F_compressOptions_t
#  define LZ4F_decompressOptions_t        AOCL_LLC_LZ4F_decompressOptions_t
#  define LZ4F_cctx_s                     AOCL_LLC_LZ4F_cctx_s
#  define LZ4F_cctx                       AOCL_LLC_LZ4F_cctx
#  define LZ4F_compressionContext_t       AOCL_LLC_LZ4F_compressionContext_t
#  define LZ4F_dctx_s                     AOCL_LLC_LZ4F_dctx_s
#  define LZ4F_dctx                       AOCL_LLC_LZ4F_dctx
#  define LZ4F_decompressionContext_t     AOCL_LLC_LZ4F_decompressionContext_t
#  define LZ4F_CDict_s                    AOCL_LLC_LZ4F_CDict_s
#  define LZ4F_CDict                      AOCL_LLC_LZ4F_CDict
#  define LZ4F_errorCodes                 AOCL_LLC_LZ4F_errorCodes
#  define LZ4F_AllocFunction              AOCL_LLC_LZ4F_AllocFunction
#  define LZ4F_CallocFunction             AOCL_LLC_LZ4F_CallocFunction
#  define LZ4F_FreeFunction               AOCL_LLC_LZ4F_FreeFunction
#  define LZ4F_CustomMem                  AOCL_LLC_LZ4F_CustomMem

/* Deprecated type aliases (for backward compatibility) */
#  define blockSizeID_t                   AOCL_LLC_blockSizeID_t
#  define blockMode_t                     AOCL_LLC_blockMode_t
#  define contentChecksum_t               AOCL_LLC_contentChecksum_t
#  define frameType_t                     AOCL_LLC_frameType_t

/* XXHash types (used by LZ4) */
#  define XXH_errorcode                   AOCL_LLC_LZ4_XXH_errorcode
#  define XXH_OK                          AOCL_LLC_LZ4_XXH_OK
#  define XXH_ERROR                       AOCL_LLC_LZ4_XXH_ERROR
#  define XXH32_hash_t                    AOCL_LLC_LZ4_XXH32_hash_t
#  define XXH64_hash_t                    AOCL_LLC_LZ4_XXH64_hash_t
#  define XXH32_state_t                   AOCL_LLC_LZ4_XXH32_state_t
#  define XXH64_state_t                   AOCL_LLC_LZ4_XXH64_state_t
#  define XXH32_canonical_t               AOCL_LLC_LZ4_XXH32_canonical_t
#  define XXH64_canonical_t               AOCL_LLC_LZ4_XXH64_canonical_t
#  define XXH32_state_s                   AOCL_LLC_LZ4_XXH32_state_s
#  define XXH64_state_s                   AOCL_LLC_LZ4_XXH64_state_s

/**** LZ4 ****/

#endif /* AOCL_LLC_PREFIX */
#endif /* __AOCL_PREFIX_H */
