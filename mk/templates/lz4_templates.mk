# Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# ==============================================================================
# LZ4 Template Generation
# ==============================================================================
# Generates optimized LZ4 function variants from template files
# Uses pure sed/shell scripting (no Python/CMake dependencies)
#
# Compression variants: 3
# Decompression variants: 2
# Total: 5 generated headers
# ==============================================================================

.PHONY: lz4-templates

# Set default directories if not already set
ROOT_DIR ?= $(CURDIR)

# Output directory for generated headers (in build tree)
GEN_DIR ?= $(ROOT_DIR)/build/generated
LZ4_GEN_DIR := $(GEN_DIR)/algos/lz4

# Template source files
LZ4_COMPRESS_TEMPLATE := algos/lz4/lz4_compress_aocl.h.in
LZ4_DECOMPRESS_TEMPLATE := algos/lz4/lz4_decompress_aocl.h.in

# Generated header files
LZ4_COMPRESS_VARIANT1 := $(LZ4_GEN_DIR)/AOCL_LZ4_compress_generic_validated.h
LZ4_COMPRESS_VARIANT2 := $(LZ4_GEN_DIR)/AOCL_LZ4_compress_generic_validated_FDS_noDict.h
LZ4_COMPRESS_VARIANT3 := $(LZ4_GEN_DIR)/AOCL_LZ4_compress_generic_validated_mt.h
LZ4_DECOMPRESS_VARIANT1 := $(LZ4_GEN_DIR)/AOCL_LZ4_decompress_generic.h
LZ4_DECOMPRESS_VARIANT2 := $(LZ4_GEN_DIR)/AOCL_LZ4_decompress_generic_mt.h

# All generated files
LZ4_GENERATED_HEADERS := $(LZ4_COMPRESS_VARIANT1) \
                         $(LZ4_COMPRESS_VARIANT2) \
                         $(LZ4_COMPRESS_VARIANT3) \
                         $(LZ4_DECOMPRESS_VARIANT1) \
                         $(LZ4_DECOMPRESS_VARIANT2)

# Main target
lz4-templates: $(LZ4_GENERATED_HEADERS)
	@echo "✓ LZ4 templates generated (5 variants)"

# ==============================================================================
# COMPRESSION VARIANT 1: AOCL_LZ4_compress_generic_validated
# ==============================================================================

$(LZ4_COMPRESS_VARIANT1): $(LZ4_COMPRESS_TEMPLATE) | $(LZ4_GEN_DIR)
	@echo "Generating LZ4 compress variant 1..."
	@sed -e 's|@AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_COMMENT@||g' \
	     -e 's|@AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_FUNC@|AOCL_LZ4_compress_generic_validated|g' \
	     -e 's|@AOCL_LZ4_CGV_ADDITIONAL_PARAMS@|/* none */|g' \
	     -e '/@AOCL_LZ4_CGV_INIT_START_BYTE@/c\    /* First Byte */\n    {   U32 const h = AOCL_LZ4_hashPosition(ip, tableType);\n        if (tableType == byPtr) {\n            LZ4_putPositionOnHash(ip, h, cctx->hashTable, byPtr);\n        } else {\n            LZ4_putIndexOnHash(startIndex, h, cctx->hashTable, tableType);\n    }   }\n    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);' \
	     -e 's|@AOCL_LZ4_CGV_INIT_IPDATA@|U32 ipData;|g' \
	     -e 's|@AOCL_LZ4_CGV_INIT_MATCHDATA@|auto U32 matchData;|g' \
	     -e 's|@AOCL_LZ4_CGV_SET_IPDATA@|ipData=*(U32*)ip;|g' \
	     -e 's|@AOCL_LZ4_CGV_SET_MATCHDATA@|matchData=*(U32*)match;|g' \
	     -e 's|@AOCL_LZ4_CGV_CMP_IP_MATCH_DATA@|matchData == ipData|g' \
	     -e 's|@AOCL_LZ4_CGV_CLEAR_HASH@|LZ4_clearHash|g' \
	     -e 's|@AOCL_LZ4_CGV_CMP_IP_MATCH@|if (LZ4_read32(match) == LZ4_read32(ip))|g' \
	     -e 's|@AOCL_LZ4_CGV_LAST_LITERALS@|/* do nothing */|g' \
	     -e '/@AOCL_LZ4_CGV_SET_RETURN_VALUES@/c\    result = (int)(((char*)op) - dest);\n    assert(result > 0);' \
	     $< > $@

# ==============================================================================
# COMPRESSION VARIANT 2: AOCL_LZ4_compress_generic_validated_FDS_noDict
# ==============================================================================

$(LZ4_COMPRESS_VARIANT2): $(LZ4_COMPRESS_TEMPLATE) | $(LZ4_GEN_DIR)
	@echo "Generating LZ4 compress variant 2 (FDS_noDict)..."
	@sed -e '/@AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_COMMENT@/c\/**\n* For cases when dict_directive == noDict, to avoid out-of-bound memory access\n* on reading 2 bytes in backward direction (when `match` pointer points to start\n* of the source buffer), initialize hashTable entries with 2. This ensures that\n* `match` pointer never points to 0th index of the source buffer.\n*/' \
	     -e 's|@AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_FUNC@|AOCL_LZ4_compress_generic_validated_FDS_noDict|g' \
	     -e 's|@AOCL_LZ4_CGV_ADDITIONAL_PARAMS@|/* none */|g' \
	     -e '/@AOCL_LZ4_CGV_INIT_START_BYTE@/c\    assert(dictDirective == noDict);\n    if (tableType == byU32) AOCL_INITIALIZE_HASHTABLE_FOR_NODICT(cctx, U32, (1U << (LZ4_MEMORY_USAGE - 2)))\n    if (tableType == byU16) AOCL_INITIALIZE_HASHTABLE_FOR_NODICT(cctx, U16, (1U << (LZ4_MEMORY_USAGE - 1)))\n    /* Skip first two bytes */\n    ip += 2;\n    /* Third Byte */\n    {   U32 const h = AOCL_LZ4_hashPosition(ip, tableType);\n        if (tableType == byPtr) {\n            LZ4_putPositionOnHash(ip, h, cctx->hashTable, byPtr);\n        } else {\n            LZ4_putIndexOnHash(ip-base, h, cctx->hashTable, tableType);\n    }   }\n    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);' \
	     -e 's|@AOCL_LZ4_CGV_INIT_IPDATA@|U64 ipDataFwd, ipDataBck;|g' \
	     -e 's|@AOCL_LZ4_CGV_INIT_MATCHDATA@|auto U64 matchDataFwd, matchDataBck;|g' \
	     -e 's|@AOCL_LZ4_CGV_SET_IPDATA@|AOCL_READ_FWD_AND_BACK_BYTES(ip, ipDataFwd, ipDataBck)|g' \
	     -e 's|@AOCL_LZ4_CGV_SET_MATCHDATA@|AOCL_READ_FWD_AND_BACK_BYTES(match, matchDataFwd, matchDataBck)|g' \
	     -e 's|@AOCL_LZ4_CGV_CMP_IP_MATCH_DATA@|matchDataFwd == ipDataFwd \|\| matchDataBck == ipDataBck|g' \
	     -e 's|@AOCL_LZ4_CGV_CLEAR_HASH@|AOCL_LZ4_clearHash|g' \
	     -e '/@AOCL_LZ4_CGV_CMP_IP_MATCH@/c\                U64 ipDataFwd, ipDataBck;\n                auto U64 matchDataFwd, matchDataBck;\n                AOCL_READ_FWD_AND_BACK_BYTES(ip, ipDataFwd, ipDataBck)\n                AOCL_READ_FWD_AND_BACK_BYTES(match, matchDataFwd, matchDataBck)\n                if (matchDataFwd == ipDataFwd || matchDataBck == ipDataBck)' \
	     -e 's|@AOCL_LZ4_CGV_LAST_LITERALS@|/* do nothing */|g' \
	     -e '/@AOCL_LZ4_CGV_SET_RETURN_VALUES@/c\    result = (int)(((char*)op) - dest);\n    assert(result > 0);' \
	     $< > $@

# ==============================================================================
# COMPRESSION VARIANT 3: AOCL_LZ4_compress_generic_validated_mt
# ==============================================================================

$(LZ4_COMPRESS_VARIANT3): $(LZ4_COMPRESS_TEMPLATE) | $(LZ4_GEN_DIR)
	@echo "Generating LZ4 compress variant 3 (multi-threaded)..."
	@sed -e '/@AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_COMMENT@/c\/**\n* Same as AOCL_LZ4_compress_generic_validated, but with state information\n* for Multi - threaded support\n*/' \
	     -e 's|@AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_FUNC@|AOCL_LZ4_compress_generic_validated_mt|g' \
	     -e 's|@AOCL_LZ4_CGV_ADDITIONAL_PARAMS@|,unsigned char** last_anchor_ptr\n                 ,unsigned int* last_bytes_len|g' \
	     -e '/@AOCL_LZ4_CGV_INIT_START_BYTE@/c\    BYTE* dst_without_lastLiterals;\n    /* First Byte */\n    {   U32 const h = AOCL_LZ4_hashPosition(ip, tableType);\n        if (tableType == byPtr) {\n            LZ4_putPositionOnHash(ip, h, cctx->hashTable, byPtr);\n        } else {\n            LZ4_putIndexOnHash(startIndex, h, cctx->hashTable, tableType);\n    }   }\n    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);' \
	     -e 's|@AOCL_LZ4_CGV_INIT_IPDATA@|U32 ipData;|g' \
	     -e 's|@AOCL_LZ4_CGV_INIT_MATCHDATA@|auto U32 matchData;|g' \
	     -e 's|@AOCL_LZ4_CGV_SET_IPDATA@|ipData=*(U32*)ip;|g' \
	     -e 's|@AOCL_LZ4_CGV_SET_MATCHDATA@|matchData=*(U32*)match;|g' \
	     -e 's|@AOCL_LZ4_CGV_CMP_IP_MATCH_DATA@|matchData == ipData|g' \
	     -e 's|@AOCL_LZ4_CGV_CLEAR_HASH@|LZ4_clearHash|g' \
	     -e 's|@AOCL_LZ4_CGV_CMP_IP_MATCH@|if (LZ4_read32(match) == LZ4_read32(ip))|g' \
	     -e 's|@AOCL_LZ4_CGV_LAST_LITERALS@|dst_without_lastLiterals = op;|g' \
	     -e '/@AOCL_LZ4_CGV_SET_RETURN_VALUES@/c\    if (last_bytes_len != NULL)\n    {\n        result = (int)(((char*)dst_without_lastLiterals) - dest);\n        *last_anchor_ptr = (BYTE*)anchor;\n        *last_bytes_len = (size_t)(iend - anchor);\n        LOG_FORMATTED(INFO, logCtx, "Thread [id: %d] : result=%i, last_bytes_len=%i", omp_get_thread_num(), result, (int)(*last_bytes_len));\n    }\n    else\n    {\n        result = (int)(((char*)op) - dest);\n        *last_anchor_ptr = (BYTE*)op;\n        LOG_FORMATTED(INFO, logCtx, "Thread [id: %d] : result=%i", omp_get_thread_num(), result);\n    }\n    assert(result >= 0);' \
	     $< > $@

# ==============================================================================
# DECOMPRESSION VARIANT 1: AOCL_LZ4_decompress_generic
# ==============================================================================

$(LZ4_DECOMPRESS_VARIANT1): $(LZ4_DECOMPRESS_TEMPLATE) | $(LZ4_GEN_DIR)
	@echo "Generating LZ4 decompress variant 1..."
	@sed -e 's|@AOCL_LZ4_DECOMPRESS_GENERIC_COMMENT@||g' \
	     -e 's|@AOCL_LZ4_DECOMPRESS_GENERIC_FUNC@|AOCL_LZ4_decompress_generic|g' \
	     -e 's|@AOCL_LZ4_DGV_ADDITIONAL_PARAMS@|/* none */|g' \
	     -e '/@AOCL_LZ4_DGV_LONG_MATCH@/c\                size_t const addl = read_variable_length(\&ip, iend - LASTLITERALS + 1, 0);\n                if (addl == rvl_error) {\n                    LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional match length.");\n                    DEBUGLOG(5, "error reading long match length");\n                    goto _output_error;\n                }\n                length += addl;\n' \
	     -e '/@AOCL_LZ4_DGV_LAST_SEQ_CHECK@/c\                    if ((ip+length != iend) || (cpy > oend)) {\n                        LOG_FORMATTED(ERR, logCtx, "Must be the last (or invalid) sequence because of the parsing limitations. Error, %s.",\n                            (ip+length != iend) ? "exact input not consumed" : "output buffer overflow" );\n' \
	     -e 's|@AOCL_LZ4_DGV_EOF_CHECK@|!partialDecoding \|\| (cpy == oend) \|\| (ip >= (iend-2))|g' \
	     -e '/@AOCL_LZ4_DGV_COPY_MATCH@/c\                size_t const addl = read_variable_length(\&ip, iend - LASTLITERALS + 1, 0);\n                if (addl == rvl_error) {\n                    LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional match length.");\n                    goto _output_error;\n                }\n                length += addl;\n' \
	     -e '/@AOCL_LZ4_DGV_MATCH_COPY@/c\            if (unlikely(offset<8)) {\n                LZ4_write32(op, 0);   /* silence msan warning when offset==0 */\n                op[0] = match[0];\n                op[1] = match[1];\n                op[2] = match[2];\n                op[3] = match[3];\n                match += inc32table[offset];\n                LZ4_memcpy(op+4, match, 4);\n                match -= dec64table[offset];\n            } else {\n                LZ4_memcpy(op, match, 8);\n                match += 8;\n            }\n            op += 8;' \
	     -e 's|@AOCL_LZ4_DGV_CHECK_LAST_THREAD@|/* none */|g' \
	     -e 's|@AOCL_LZ4_DGV_SAFE_DECODE_EXIT@|/* nothing */|g' \
	     $< > $@

# ==============================================================================
# DECOMPRESSION VARIANT 2: AOCL_LZ4_decompress_generic_mt
# ==============================================================================

$(LZ4_DECOMPRESS_VARIANT2): $(LZ4_DECOMPRESS_TEMPLATE) | $(LZ4_GEN_DIR)
	@echo "Generating LZ4 decompress variant 2 (multi-threaded)..."
	@sed -e 's|@AOCL_LZ4_DECOMPRESS_GENERIC_COMMENT@|/* Same as AOCL_LZ4_decompress_generic, but with multi-threaded support */|g' \
	     -e 's|@AOCL_LZ4_DECOMPRESS_GENERIC_FUNC@|AOCL_LZ4_decompress_generic_mt|g' \
	     -e 's|@AOCL_LZ4_DGV_ADDITIONAL_PARAMS@|,int is_last_thread|g' \
	     -e '/@AOCL_LZ4_DGV_LONG_MATCH@/c\                if (is_last_thread) {\n                    size_t const addl = read_variable_length(\&ip, iend - LASTLITERALS + 1, 0);\n                    if (addl == rvl_error) { \n                        LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX"Encountered variable_length_error while decoding additional match length.", omp_get_thread_num());\n                        DEBUGLOG(5, "error reading long match length");\n                        goto _output_error; \n                    }\n                    length += addl;\n                } else {\n                    size_t const addl = read_variable_length(\&ip, iend + 1, 0);\n                    if (addl == rvl_error) {\n                        LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX"Encountered variable_length_error while decoding additional match length.", omp_get_thread_num());\n                        DEBUGLOG(5, "error reading long match length");\n                        goto _output_error; \n                    }\n                    length += addl;\n                }\n' \
	     -e '/@AOCL_LZ4_DGV_LAST_SEQ_CHECK@/c\                    if ((is_last_thread && (ip + length != iend)) || (cpy > oend)) {\n                        LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX" Must be the last (or invalid) sequence because of the parsing limitations. Error, %s.",\n                        omp_get_thread_num(), (is_last_thread && (ip + length != iend)) ? "exact input not consumed" : "output buffer overflow");\n' \
	     -e 's|@AOCL_LZ4_DGV_EOF_CHECK@|(is_last_thread \&\& !partialDecoding) \|\| (cpy == oend) \|\| (is_last_thread \&\& (ip >= (iend - 2)))|g' \
	     -e '/@AOCL_LZ4_DGV_COPY_MATCH@/c\                if (is_last_thread) {\n                    size_t const addl = read_variable_length(\&ip, iend - LASTLITERALS + 1, 0);\n                    if (addl == rvl_error) { goto _output_error; }\n                    length += addl;\n                }\n                else {\n                    size_t const addl = read_variable_length(\&ip, iend + 1, 0);\n                    if (addl == rvl_error) { goto _output_error; }\n                    length += addl;\n                }\n' \
	     -e "/@AOCL_LZ4_DGV_MATCH_COPY@/c\            if(cpy <= oend /* ensure that the output buffer is not overflowed */)\n            {\n                /* \n                    In multi-threaded mode, chunks differ from traditional LZ4 format - they may end with\n                    an LZ4 compressed sequence instead of LASTLITERALS bytes. The traditional LZ4 code copies\n                    8 bytes at a time, which could cause buffer overflows by writing into the next thread's\n                    starting point of output buffer. Therefore, we need to handle copying differently in multi-threaded mode.\n                */\n                while(op < cpy)\n                {\n                    *op = *match;\n                    op++;\n                    match++;\n                }\n            }" \
	     -e 's|@AOCL_LZ4_DGV_CHECK_LAST_THREAD@|\&\& is_last_thread /* LASTLITERALS validation applies solely to the last thread; other threads might process chunks with last-literals smaller than the minimum. */|g' \
	     -e '/@AOCL_LZ4_DGV_SAFE_DECODE_EXIT@/c\            if ((cpy == oend) || (ip >= iend)) {\n                break;\n            }\n' \
	     $< > $@

# ==============================================================================
# DIRECTORY CREATION
# ==============================================================================

$(LZ4_GEN_DIR):
	@mkdir -p $@

# ==============================================================================
# CLEAN
# ==============================================================================

.PHONY: clean-lz4-templates

clean-lz4-templates:
	@rm -f $(LZ4_GENERATED_HEADERS)
	@echo "✓ LZ4 generated templates cleaned"
