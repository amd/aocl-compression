# Copyright (C) 2022-2026, Advanced Micro Devices. All rights reserved.
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
# ZSTD Template Generation
# ==============================================================================
# Generates optimized ZSTD function variants from template files
# Uses pure sed/shell scripting (no Python/CMake dependencies)
#
# Complexity: High - 12+ variants with conditional logic
# Variants depend on AOCL_DECOMPRESS_FAST level (OFF, 1, 2, 3)
# ==============================================================================

.PHONY: zstd-templates

# Set default directories if not already set
ROOT_DIR ?= $(CURDIR)

# Output directory for generated headers (in build tree)
GEN_DIR ?= $(ROOT_DIR)/build/generated
ZSTD_GEN_DIR := $(GEN_DIR)/algos/zstd/lib/compress

# Template source files (located in algos/zstd/lib/)
ZSTD_LAZY_TEMPLATE := algos/zstd/lib/compress/zstd_lazy_aocl.h.in
ZSTD_DFAST_TEMPLATE := algos/zstd/lib/compress/zstd_double_fast_noDict_generic_fds_aocl.h.in
ZSTD_FAST_TEMPLATE := algos/zstd/lib/compress/zstd_fast_noDict_generic_fds_aocl.h.in
ZSTD_DECOMP_DECODE_TEMPLATE := algos/zstd/lib/decompress/zstd_decompress_block_decode_sequence_aocl.h.in
ZSTD_DECOMP_DECODE_FDS_TEMPLATE := algos/zstd/lib/decompress/zstd_decompress_block_decode_sequence_fds_aocl.h.in
ZSTD_DECOMP_EXEC_FDS_TEMPLATE := algos/zstd/lib/decompress/zstd_decompress_block_exec_sequence_fds_aocl.h.in
ZSTD_DECOMP_SEQS_FDS_TEMPLATE := algos/zstd/lib/decompress/zstd_decompress_block_decompress_sequences_fds_aocl.h.in

# Generated header files - Lazy strategy
ZSTD_LAZY_FDS2_BASE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_lazy_fds2_base.h
ZSTD_LAZY_FDS3_OFFSET8 := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_lazy_fds3_offset8.h
ZSTD_LAZY_FDS3_ANALYZE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_lazy_fds3_analyze.h
ZSTD_LAZY_FDS3_BASE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_lazy_fds3_base.h

# Generated header files - Double Fast strategy
ZSTD_DFAST_FDS2_BASE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_doubleFast_noDict_generic_fds2_base.h
ZSTD_DFAST_FDS3_ANALYZE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_doubleFast_noDict_generic_fds3_analyze.h
ZSTD_DFAST_FDS3_BASE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_doubleFast_noDict_generic_fds3_base.h

# Generated header files - Fast strategy
ZSTD_FAST_FDS2_BASE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_fast_noDict_generic_fds2_base.h
ZSTD_FAST_FDS3_ANALYZE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_fast_noDict_generic_fds3_analyze.h
ZSTD_FAST_FDS3_BASE := $(ZSTD_GEN_DIR)/aocl_zstd_compressBlock_fast_noDict_generic_fds3_base.h

# Decompression directory (generated headers in build tree)
ZSTD_DECOMP_DIR := $(GEN_DIR)/algos/zstd/lib/decompress

# Generated decompression headers
ZSTD_DECOMP_DECODE := $(ZSTD_DECOMP_DIR)/aocl_zstd_decodeSequence.h
ZSTD_DECOMP_DECODE_GCC := $(ZSTD_DECOMP_DIR)/aocl_zstd_decodeSequence_gcc.h
ZSTD_DECOMP_DECODE_F2 := $(ZSTD_DECOMP_DIR)/aocl_zstd_decodeSequence_mem64_fast2.h
ZSTD_DECOMP_DECODE_GCC_F2 := $(ZSTD_DECOMP_DIR)/aocl_zstd_decodeSequence_mem64_gcc_fast2.h
ZSTD_DECOMP_DECODE_F3 := $(ZSTD_DECOMP_DIR)/aocl_zstd_decodeSequence_mem64_fast3.h
ZSTD_DECOMP_DECODE_GCC_F3 := $(ZSTD_DECOMP_DIR)/aocl_zstd_decodeSequence_mem64_gcc_fast3.h
ZSTD_DECOMP_EXEC_F2 := $(ZSTD_DECOMP_DIR)/aocl_zstd_execSequence_mem64_fast2.h
ZSTD_DECOMP_EXEC_NODICT := $(ZSTD_DECOMP_DIR)/aocl_zstd_execSequence_mem64_nodict.h
ZSTD_DECOMP_SEQS_BODY := $(ZSTD_DECOMP_DIR)/aocl_zstd_decompressSequences_body.h
ZSTD_DECOMP_SEQS_F2 := $(ZSTD_DECOMP_DIR)/aocl_zstd_decompressSequences_body_mem64_fast2.h
ZSTD_DECOMP_SEQS_F3 := $(ZSTD_DECOMP_DIR)/aocl_zstd_decompressSequences_body_mem64_fast3.h
ZSTD_DECOMP_SEQS_F2_NOTB := $(ZSTD_DECOMP_DIR)/aocl_zstd_decompressSequences_body_mem64_fast2_NOTB_NOEXT_REP2.h

# All generated files
ZSTD_GENERATED_HEADERS := $(ZSTD_LAZY_FDS2_BASE) \
                          $(ZSTD_LAZY_FDS3_OFFSET8) \
                          $(ZSTD_LAZY_FDS3_ANALYZE) \
                          $(ZSTD_LAZY_FDS3_BASE) \
                          $(ZSTD_DFAST_FDS2_BASE) \
                          $(ZSTD_DFAST_FDS3_ANALYZE) \
                          $(ZSTD_DFAST_FDS3_BASE) \
                          $(ZSTD_FAST_FDS2_BASE) \
                          $(ZSTD_FAST_FDS3_ANALYZE) \
                          $(ZSTD_FAST_FDS3_BASE) \
                          $(ZSTD_DECOMP_DECODE) \
                          $(ZSTD_DECOMP_DECODE_GCC) \
                          $(ZSTD_DECOMP_DECODE_F2) \
                          $(ZSTD_DECOMP_DECODE_GCC_F2) \
                          $(ZSTD_DECOMP_DECODE_F3) \
                          $(ZSTD_DECOMP_DECODE_GCC_F3) \
                          $(ZSTD_DECOMP_EXEC_F2) \
                          $(ZSTD_DECOMP_EXEC_NODICT) \
                          $(ZSTD_DECOMP_SEQS_BODY) \
                          $(ZSTD_DECOMP_SEQS_F2) \
                          $(ZSTD_DECOMP_SEQS_F3) \
                          $(ZSTD_DECOMP_SEQS_F2_NOTB)

# Main target
zstd-templates: $(ZSTD_GENERATED_HEADERS)
	@echo "✓ ZSTD templates generated (22 variants)"

# ==============================================================================
# LAZY STRATEGY VARIANTS
# ==============================================================================

# Lazy FDS2 Base
$(ZSTD_LAZY_FDS2_BASE): $(ZSTD_LAZY_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD lazy FDS2 base..."
	@sed -e 's|@AOCL_ZSTD_CLNF_FUNC@|AOCL_ZSTD_compressBlock_lazy_fds2_base|g' \
	     -e 's|@AOCL_ZSTD_CLNF_MIN_OFFSET@|WILDCOPY_VECLEN|g' \
	     -e '/@AOCL_ZSTD_CLNF_REP_MATCH_D0@/c\    /* check repCode \n    * (offset_1 > 0) changed to (offset_1 >= WILDCOPY_VECLEN) as initial rep codes \n    * can be < WILDCOPY_VECLEN and offsets < WILDCOPY_VECLEN should not be included in the sequences\n    */\n    if ((offset_1 >= WILDCOPY_VECLEN) \& (MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1))) {\n        matchLength = ZSTD_count(ip + 1 + 4, ip + 1 + 4 - offset_1, iend) + 4;\n        if ((depth == 0) \&\& is_totalbits_limited_seq_possible(ip, anchor, matchLength, 1 /* Repeated_Offset1 */, MINMATCH))\n            goto _storeSequence;\n    }' \
	     -e '/@AOCL_ZSTD_CLNF_REP_MATCH_D1@/c\    if ((offBase) \&\& ((offset_1 >= WILDCOPY_VECLEN) \& (MEM_read32(ip) == MEM_read32(ip - offset_1)))) {\n        size_t const mlRep = ZSTD_count(ip + 4, ip + 4 - offset_1, iend) + 4;\n        int const gain2 = (int)(mlRep * 3);\n        int const gain1 = (int)(matchLength * 3 - ZSTD_highbit32((U32)offBase) + 1);\n        if ((mlRep >= 4) \&\& (gain2 > gain1))\n            matchLength = mlRep, offBase = REPCODE1_TO_OFFBASE, start = ip;\n    }' \
	     -e '/@AOCL_ZSTD_CLNF_REP_MATCH_D2@/c\    if ((offBase) \&\& ((offset_1 >= WILDCOPY_VECLEN) \& (MEM_read32(ip) == MEM_read32(ip - offset_1)))) {\n        size_t const mlRep = ZSTD_count(ip + 4, ip + 4 - offset_1, iend) + 4;\n        int const gain2 = (int)(mlRep * 4);\n        int const gain1 = (int)(matchLength * 4 - ZSTD_highbit32((U32)offBase) + 1);\n        if ((mlRep >= 4) \&\& (gain2 > gain1))\n            matchLength = mlRep, offBase = REPCODE1_TO_OFFBASE, start = ip;\n    }' \
	     -e 's|@AOCL_ZSTD_CLNF_LONG_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CLNF_STORE_SEQ@/c\_storeSequence:\n    assert((OFFBASE_IS_OFFSET(offBase) \&\& (OFFBASE_TO_OFFSET(offBase) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offBase)));\n    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength, MINMATCH);' \
	     $< > $@

# Lazy FDS3 Offset8
$(ZSTD_LAZY_FDS3_OFFSET8): $(ZSTD_LAZY_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD lazy FDS3 offset8..."
	@sed -e 's|@AOCL_ZSTD_CLNF_FUNC@|AOCL_ZSTD_compressBlock_lazy_fds3_offset8|g' \
	     -e 's|@AOCL_ZSTD_CLNF_MIN_OFFSET@|(WILDCOPY_VECLEN/2)|g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D0@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D1@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D2@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_LONG_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CLNF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(offBase) \&\& (OFFBASE_TO_OFFSET(offBase) >= (WILDCOPY_VECLEN/2))) || (!OFFBASE_IS_OFFSET(offBase)));\n    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength, MINMATCH);' \
	     $< > $@

# Lazy FDS3 Analyze (with state transition)
$(ZSTD_LAZY_FDS3_ANALYZE): $(ZSTD_LAZY_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD lazy FDS3 analyze..."
	@sed -e 's|@AOCL_ZSTD_CLNF_FUNC@|AOCL_ZSTD_compressBlock_lazy_fds3_base_with_trans|g' \
	     -e 's|@AOCL_ZSTD_CLNF_MIN_OFFSET@|WILDCOPY_VECLEN|g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D0@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D1@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D2@||g' \
	     -e '/@AOCL_ZSTD_CLNF_LONG_MATCH@/c\    /* If its a long overlapping match; do not skip it,\n     * instead mark state as FDS_NONE and continue to save the match */\n    U32 offset_offBase = OFFBASE_TO_OFFSET(offBase);\n    if (UNLIKELY(matchLength > AOCL_LONG_MATCH_LIMIT_LAZY \&\& offset_offBase > 0 \&\& offset_offBase < WILDCOPY_VECLEN))\n    {\n        seqStore->fds_config.state = FDS_NONE; // stop imposing constraints from next block\n        offset_1 = offset_offBase;\n        goto _storeSequence;\n    }\n    else' \
	     -e '/@AOCL_ZSTD_CLNF_STORE_SEQ@/c\_storeSequence:\n    if (seqStore->fds_config.state == FDS_NONE)\n    {\n        ZSTD_storeSeq(seqStore, (size_t)(start-anchor), anchor, iend, (U32)offBase, matchLength);\n    }\n    else\n    {\n        AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, (U32)offBase, matchLength, MINMATCH);\n    }' \
	     $< > $@

# Lazy FDS3 Base
$(ZSTD_LAZY_FDS3_BASE): $(ZSTD_LAZY_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD lazy FDS3 base..."
	@sed -e 's|@AOCL_ZSTD_CLNF_FUNC@|AOCL_ZSTD_compressBlock_lazy_fds3_base|g' \
	     -e 's|@AOCL_ZSTD_CLNF_MIN_OFFSET@|WILDCOPY_VECLEN|g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D0@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D1@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_REP_MATCH_D2@||g' \
	     -e 's|@AOCL_ZSTD_CLNF_LONG_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CLNF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(offBase) \&\& (OFFBASE_TO_OFFSET(offBase) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offBase)));\n    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength, MINMATCH);' \
	     $< > $@

# ==============================================================================
# DOUBLE FAST STRATEGY VARIANTS
# ==============================================================================

# Double Fast FDS2 Base
$(ZSTD_DFAST_FDS2_BASE): $(ZSTD_DFAST_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD double-fast FDS2 base..."
	@sed -e '/@AOCL_ZSTD_CDNGF_COMMENT@/c\    /*\n    * Derived from AOCL_ZSTD_compressBlock_doubleFast_noDict_generic, but enforces :\n    *   totalBits < (STREAM_ACCUMULATOR_MIN_64 - (LLFSELog + MLFSELog + OffFSELog)\n    *   no Repeated_Offset2 and Repeated_Offset3\n    *   no offsets go into extDict (no offset beyond prefix)\n    *   offset >= WILDCOPY_VECLEN\n    * Calls AOCL_ZSTD_storeSequences to store sequences\n    */' \
	     -e 's|@AOCL_ZSTD_CDNGF_FUNC@|AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_base|g' \
	     -e 's|@AOCL_ZSTD_CDNGF_IS_LONG_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CDNGF_REP_MATCH@/c\    /* check noDict repcode\n    * (offset_1 > 0) changed to (offset_1 >= WILDCOPY_VECLEN) as initial rep codes\n    * can be < WILDCOPY_VECLEN and offsets < WILDCOPY_VECLEN should not be included in the sequences\n    */\n    if ((offset_1 >= WILDCOPY_VECLEN) \& (MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1)))\n    {\n        PREFETCH_MATCH_L(ip + 1 + 4);\n        mLength = AOCL_ZSTD_count(ip + 1 + 4, ip + 1 + 4 - offset_1, iend) + 4;\n        ip++;\n\n        if (is_totalbits_limited_seq_possible(ip, anchor, mLength, 1 /* Repeated_Offset1 */, MINMATCH))\n        {\n            ZSTD_STORE_SEQ(seqStore, (size_t)(ip - anchor), anchor, iend, REPCODE1_TO_OFFBASE, mLength);\n            goto _match_stored;\n        }\n    }' \
	     -e '/@AOCL_ZSTD_CDNGF_LONG_MATCH@/c\    /* check prefix long match */\n    if (((ip - matchl0) >= WILDCOPY_VECLEN) \&\& (MEM_read64(matchl0) == MEM_read64(ip))) \n    {\n        PREFETCH_MATCH_L(ip1 + 8);\n        PREFETCH_MATCH_L(ip1 + 9);\n        PREFETCH_MATCH_L(ip1 + 10);\n        PREFETCH_MATCH_L(ip1 + 11);\n        mLength = AOCL_ZSTD_count(ip + 8, matchl0 + 8, iend) + 8;\n        offset = (U32)(ip - matchl0);\n        ipfwd = ip;\n        while (((ip > anchor) \& (matchl0 > prefixLowest)) \&\& (ip[-1] == matchl0[-1])) { ip--; matchl0--; mLength++; } /* catch up */\n        if (is_totalbits_limited_seq_possible(ip, anchor, mLength, offset, MINMATCH)) \n        {\n            goto _match_found;\n        }\n        else \n        {\n            ip = ipfwd; //revert\n        }\n    }' \
	     -e '/@AOCL_ZSTD_CDNGF_SHORT_MATCH@/c\    /* check prefix short match */\n    if (((ip - matchs0) >= WILDCOPY_VECLEN) \&\& (MEM_read32(matchs0) == MEM_read32(ip))) \n    {\n        PREFETCH_MATCH_S(ip + 4);\n        goto _search_next_long;\n    }' \
	     -e '/@AOCL_ZSTD_CDNGF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset)) \&\& (offset >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset))));\n    AOCL_ZSTD_storeSequences(seqStore, ip, anchor, iend, OFFSET_TO_OFFBASE(offset), mLength, MINMATCH);' \
	     -e 's|@AOCL_ZSTD_CDNGF_MATCH_STORED@|_match_stored:|g' \
	     $< > $@

# Double Fast FDS3 Analyze
$(ZSTD_DFAST_FDS3_ANALYZE): $(ZSTD_DFAST_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD double-fast FDS3 analyze..."
	@sed -e '/@AOCL_ZSTD_CDNGF_COMMENT@/c\    /*\n    * Derived from AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_base.\n    * Additionally no Repeated_Offset1.\n    * Also, checks for long matches with offset < WILDCOPY_VECLEN. Such \n    * matches can be handled significantly faster during decompresion. Hence,\n    * if encountered stop imposing constraints and mark state as FDS_NONE.\n    */' \
	     -e 's|@AOCL_ZSTD_CDNGF_FUNC@|AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds3_base_with_trans|g' \
	     -e '/@AOCL_ZSTD_CDNGF_IS_LONG_MATCH@/c\    #if AOCL_DECOMPRESS_FAST > 2\n    /* Skipping long matches when enforcing constraints can significantly impact \n     * compression ratio and decompression speed. If such matches are encountered\n     * while imposing constraints, stop enforcing constraints. */\n    #define AOCL_LONG_MATCH_LIMIT_DFAST (16 * 1024)\n    FORCE_INLINE_TEMPLATE\n    size_t AOCL_ZSTD_isLongMatch(SeqStore_t* seqStore, \n    const BYTE* ip, const BYTE* match, const BYTE* const iend)\n    {\n        if (MEM_read64(match) == MEM_read64(ip)) {\n            size_t mLength = AOCL_ZSTD_count(ip + 8, match + 8, iend) + 8;\n            if (mLength > AOCL_LONG_MATCH_LIMIT_DFAST) {\n                seqStore->fds_config.state = FDS_NONE; // stop imposing constraints from next block\n                return mLength;\n            }\n        }\n        return 0;\n    }\n    #endif /* AOCL_DECOMPRESS_FAST > 2 */' \
	     -e 's|@AOCL_ZSTD_CDNGF_REP_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CDNGF_LONG_MATCH@/c\    /* check prefix long match */\n    if (LIKELY((ip - matchl0) >= WILDCOPY_VECLEN))\n    {\n        if ((MEM_read64(matchl0) == MEM_read64(ip))) \n        {\n            PREFETCH_MATCH_L(ip1 + 8);\n            PREFETCH_MATCH_L(ip1 + 9);\n            PREFETCH_MATCH_L(ip1 + 10);\n            PREFETCH_MATCH_L(ip1 + 11);\n            mLength = AOCL_ZSTD_count(ip + 8, matchl0 + 8, iend) + 8;\n            offset = (U32)(ip - matchl0);\n            ipfwd = ip;\n            while (((ip > anchor) \& (matchl0 > prefixLowest)) \&\& (ip[-1] == matchl0[-1])) { ip--; matchl0--; mLength++; } /* catch up */\n            if (is_totalbits_limited_seq_possible(ip, anchor, mLength, offset, MINMATCH))\n            {\n                goto _match_found;\n            }\n            else \n            {\n                ip = ipfwd; //revert\n            }\n        }\n    }\n    else if((ip - matchl0) > 0) \n    {\n        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip, matchl0, iend);\n        if (mLength) \n        {\n            offset = (U32)(ip - matchl0);\n            ipfwd = ip;\n            while (((ip > anchor) \& (matchl0 > prefixLowest)) \&\& (ip[-1] == matchl0[-1])) { ip--; matchl0--; mLength++; } /* catch up */\n            if (step < 4)\n                hashLong[hl1] = (U32)(ip1 - base);\n            ZSTD_storeSeq(seqStore, (size_t)(ip - anchor), anchor, iend, OFFSET_TO_OFFBASE(offset), mLength);\n            offset_1 = offset;\n            goto _match_stored;\n        }\n    }' \
	     -e '/@AOCL_ZSTD_CDNGF_SHORT_MATCH@/c\    /* check prefix short match */\n    if (LIKELY(((ip - matchs0) >= WILDCOPY_VECLEN))) \n    {\n        if ((MEM_read32(matchs0) == MEM_read32(ip))) \n        {\n            PREFETCH_MATCH_S(ip + 4);\n            goto _search_next_long;\n        }\n    }\n    else if((ip - matchs0) > 0)\n    {\n        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip, matchs0, iend);\n        if (mLength) \n        {\n            offset = (U32)(ip - matchs0);\n            ipfwd = ip;\n            while (((ip > anchor) \& (matchs0 > prefixLowest)) \&\& (ip[-1] == matchs0[-1])) { ip--; matchs0--; mLength++; } /* catch up */\n            if (step < 4)\n                hashLong[hl1] = (U32)(ip1 - base);\n            ZSTD_storeSeq(seqStore, (size_t)(ip - anchor), anchor, iend, OFFSET_TO_OFFBASE(offset), mLength);\n            offset_1 = offset;\n            goto _match_stored;\n        }\n    }' \
	     -e '/@AOCL_ZSTD_CDNGF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset)) \&\& (offset >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset))));\n    AOCL_ZSTD_storeSequences(seqStore, ip, anchor, iend, OFFSET_TO_OFFBASE(offset), mLength, MINMATCH);' \
	     -e 's|@AOCL_ZSTD_CDNGF_MATCH_STORED@|_match_stored:|g' \
	     $< > $@

# Double Fast FDS3 Base
$(ZSTD_DFAST_FDS3_BASE): $(ZSTD_DFAST_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD double-fast FDS3 base..."
	@sed -e '/@AOCL_ZSTD_CDNGF_COMMENT@/c\    /*\n    * Derived from AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_base.\n    * Additionally no Repeated_Offset1.\n    */' \
	     -e 's|@AOCL_ZSTD_CDNGF_FUNC@|AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds3_base|g' \
	     -e 's|@AOCL_ZSTD_CDNGF_IS_LONG_MATCH@||g' \
	     -e 's|@AOCL_ZSTD_CDNGF_REP_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CDNGF_LONG_MATCH@/c\    /* check prefix long match */\n    if (LIKELY((ip - matchl0) >= WILDCOPY_VECLEN))\n    {\n        if ((MEM_read64(matchl0) == MEM_read64(ip))) \n        {\n            PREFETCH_MATCH_L(ip1 + 8);\n            PREFETCH_MATCH_L(ip1 + 9);\n            PREFETCH_MATCH_L(ip1 + 10);\n            PREFETCH_MATCH_L(ip1 + 11);\n            mLength = AOCL_ZSTD_count(ip + 8, matchl0 + 8, iend) + 8;\n            offset = (U32)(ip - matchl0);\n            ipfwd = ip;\n            while (((ip > anchor) \& (matchl0 > prefixLowest)) \&\& (ip[-1] == matchl0[-1])) { ip--; matchl0--; mLength++; } /* catch up */\n            if (is_totalbits_limited_seq_possible(ip, anchor, mLength, offset, MINMATCH))\n            {\n                goto _match_found;\n            }\n            else \n            {\n                ip = ipfwd; //revert\n            }\n        }\n    }' \
	     -e '/@AOCL_ZSTD_CDNGF_SHORT_MATCH@/c\    /* check prefix short match */\n    if (LIKELY(((ip - matchs0) >= WILDCOPY_VECLEN))) \n    {\n        if ((MEM_read32(matchs0) == MEM_read32(ip))) \n        {\n            PREFETCH_MATCH_S(ip + 4);\n            goto _search_next_long;\n        }\n    }' \
	     -e '/@AOCL_ZSTD_CDNGF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset)) \&\& (offset >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset))));\n    AOCL_ZSTD_storeSequences(seqStore, ip, anchor, iend, OFFSET_TO_OFFBASE(offset), mLength, MINMATCH);' \
	     -e 's|@AOCL_ZSTD_CDNGF_MATCH_STORED@||g' \
	     $< > $@

# ==============================================================================
# FAST STRATEGY VARIANTS
# ==============================================================================

# Fast FDS2 Base
$(ZSTD_FAST_FDS2_BASE): $(ZSTD_FAST_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD fast FDS2 base..."
	@sed -e '/@AOCL_ZSTD_CFNGF_COMMENT@/c\    /*\n    * Derived from AOCL_ZSTD_compressBlock_fast_noDict_generic, but enforces :\n    *   totalBits < (STREAM_ACCUMULATOR_MIN_64 - (LLFSELog + MLFSELog + OffFSELog)\n    *   no Repeated_Offset2 and Repeated_Offset3\n    *   no offsets go into extDict (no offset beyond prefix)\n    *   offset >= WILDCOPY_VECLEN\n    * Calls AOCL_ZSTD_storeSequences to store sequences\n    */' \
	     -e 's|@AOCL_ZSTD_CFNGF_FUNC@|AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_base|g' \
	     -e 's|@AOCL_ZSTD_CFNGF_IS_LONG_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CFNGF_REP_MATCH@/c\    /* check repcode at ip[2] \n    * (rep_offset1 > 0) changed to (rep_offset1 >= WILDCOPY_VECLEN) as initial rep codes \n    * can be < WILDCOPY_VECLEN and offsets < WILDCOPY_VECLEN should not be included in the sequences\n    */\n    /* load repcode match for ip[2]*/\n    const U32 rval = MEM_read32(ip2 - rep_offset1);\n    if ((MEM_read32(ip2) == rval) \& (rep_offset1 >= WILDCOPY_VECLEN)) \n    {\n       ip0 = ip2;\n       match0 = ip0 - rep_offset1;\n       mLength = ip0[-1] == match0[-1];\n       ip0 -= mLength;\n       match0 -= mLength;\n       offcode = REPCODE1_TO_OFFBASE;\n       mLength += 4;\n\n       /* Count the forward length. */\n       mLength += AOCL_ZSTD_count(ip0 + mLength, match0 + mLength, iend);\n       if (is_totalbits_limited_seq_possible(ip0, anchor, mLength, rep_offset1, MINMATCH))\n       {\n            /* Write next hash table entry: it'\''s already calculated.\n            * This write is known to be safe because ip1 is before the\n            * repcode (ip2). */\n            hashTable[hash1] = (U32)(ip1 - base);\n           goto _store_sequences;\n       }\n    }' \
	     -e '/@AOCL_ZSTD_CFNGF_MATCH@/c\    if (rep_offset1 >= WILDCOPY_VECLEN) \\\n    { \\\n        /* Count the forward length. */ \\\n        mLength += AOCL_ZSTD_count(ip0 + mLength, match0 + mLength, iend); \\\n        if (is_totalbits_limited_seq_possible(ip0, anchor, mLength, rep_offset1, MINMATCH)) { \\\n            /* write already calculated next hash table entry only if match is valid */ \\\n            if (safe) \\\n            { \\\n                hashTable[hash1] = (U32)(ip1 - base); \\\n            } \\\n            else \\\n            { \\\n                if (step <= 4) \\\n                { \\\n                    /* We need to avoid writing an index into the hash table >= ... */ \\\n                    hashTable[hash1] = (U32)(ip1 - base); \\\n                } \\\n            } \\\n            goto _store_sequences; \\\n        } \\\n        else \\\n            rep_offset1 = prev_rep_offset1; /* revert */ \\\n    } \\\n    else \\\n    { \\\n        rep_offset1 = prev_rep_offset1; /* revert */ \\\n    } \\' \
	     -e '/@AOCL_ZSTD_CFNGF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(offcode) \&\& (OFFBASE_TO_OFFSET(offcode) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offcode)));\n    AOCL_ZSTD_storeSequences(seqStore, ip0, anchor, iend, offcode, mLength, MINMATCH);' \
	     -e 's|@AOCL_ZSTD_CFNGF_MATCH_STORED@||g' \
	     $< > $@

# Fast FDS3 Analyze
$(ZSTD_FAST_FDS3_ANALYZE): $(ZSTD_FAST_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD fast FDS3 analyze..."
	@sed -e '/@AOCL_ZSTD_CFNGF_COMMENT@/c\    /*\n    * Derived from AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_base.\n    * Additionally no Repeated_Offset1. \n    * Also, checks for long matches with offset < WILDCOPY_VECLEN. Such \n    * matches can be handled significantly faster during decompresion. Hence,\n    * if encountered stop imposing constraints and mark state as FDS_NONE.\n    */' \
	     -e 's|@AOCL_ZSTD_CFNGF_FUNC@|AOCL_ZSTD_compressBlock_fast_noDict_generic_fds3_base_with_trans|g' \
	     -e '/@AOCL_ZSTD_CFNGF_IS_LONG_MATCH@/c\    #if AOCL_DECOMPRESS_FAST > 2\n    /* Skipping long matches when enforcing constraints can significantly impact \n     * compression ratio and decompression speed. If such matches are encountered\n     * while imposing constraints, stop enforcing constraints. */\n    #define AOCL_LONG_MATCH_LIMIT_FAST (16 * 1024)\n    FORCE_INLINE_TEMPLATE\n    size_t AOCL_ZSTD_isLongMatch(SeqStore_t* seqStore, \n        const BYTE* ip, const BYTE* match, const BYTE* const iend)\n    {\n        if (MEM_read64(match) == MEM_read64(ip)) \n        {\n            size_t mLength = AOCL_ZSTD_count(ip + 8, match + 8, iend) + 8;\n            if (mLength > AOCL_LONG_MATCH_LIMIT_FAST) \n            {\n                seqStore->fds_config.state = FDS_NONE; // stop imposing constraints from next block\n                return mLength;\n            }\n        }\n        return 0;\n    }\n    #endif' \
	     -e 's|@AOCL_ZSTD_CFNGF_REP_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CFNGF_MATCH@/c\    if (rep_offset1 >= WILDCOPY_VECLEN) \\\n    { \\\n        /* Count the forward length. */ \\\n        mLength += AOCL_ZSTD_count(ip0 + mLength, match0 + mLength, iend); \\\n        if (is_totalbits_limited_seq_possible(ip0, anchor, mLength, rep_offset1, MINMATCH)) { \\\n            /* write already calculated next hash table entry only if match is valid */ \\\n            if (safe) \\\n            { \\\n                hashTable[hash1] = (U32)(ip1 - base); \\\n            } \\\n            else \\\n            { \\\n                if (step <= 4) \\\n                { \\\n                    /* We need to avoid writing an index into the hash table >= ... */ \\\n                    hashTable[hash1] = (U32)(ip1 - base); \\\n                } \\\n            } \\\n            goto _store_sequences; \\\n        } \\\n        else \\\n            rep_offset1 = prev_rep_offset1; /* revert */ \\\n    } \\\n    else if (rep_offset1 > 0) /* rep_offset1 < WILDCOPY_VECLEN */ \\\n    { \\\n        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip0, match0, iend); \\\n        if (mLength) \\\n        { \\\n            /* write already calculated next hash table entry only if match is valid */ \\\n            if (safe) \\\n            { \\\n                hashTable[hash1] = (U32)(ip1 - base); \\\n            } \\\n            else \\\n            { \\\n                if (step <= 4) \\\n                { \\\n                    /* We need to avoid writing an index into the hash table >= ... */ \\\n                    hashTable[hash1] = (U32)(ip1 - base); \\\n                } \\\n            } \\\n            ZSTD_storeSeq(seqStore, (size_t)(ip0 - anchor), anchor, iend, offcode, mLength); \\\n            goto _match_stored; \\\n        } \\\n        else \\\n            rep_offset1 = prev_rep_offset1; /* revert */ \\\n    } \\\n    else \\\n        rep_offset1 = prev_rep_offset1; /* revert */ \\' \
	     -e '/@AOCL_ZSTD_CFNGF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(offcode) \&\& (OFFBASE_TO_OFFSET(offcode) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offcode)));\n    AOCL_ZSTD_storeSequences(seqStore, ip0, anchor, iend, offcode, mLength, MINMATCH);' \
	     -e 's|@AOCL_ZSTD_CFNGF_MATCH_STORED@|_match_stored:|g' \
	     $< > $@

# Fast FDS3 Base
$(ZSTD_FAST_FDS3_BASE): $(ZSTD_FAST_TEMPLATE) | $(ZSTD_GEN_DIR)
	@echo "Generating ZSTD fast FDS3 base..."
	@sed -e '/@AOCL_ZSTD_CFNGF_COMMENT@/c\    /*\n    * Derived from AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_base.\n    * Additionally no Repeated_Offset1. \n    */' \
	     -e 's|@AOCL_ZSTD_CFNGF_FUNC@|AOCL_ZSTD_compressBlock_fast_noDict_generic_fds3_base|g' \
	     -e 's|@AOCL_ZSTD_CFNGF_IS_LONG_MATCH@||g' \
	     -e 's|@AOCL_ZSTD_CFNGF_REP_MATCH@||g' \
	     -e '/@AOCL_ZSTD_CFNGF_MATCH@/c\    if (rep_offset1 >= WILDCOPY_VECLEN) \\\n    { \\\n        /* Count the forward length. */ \\\n        mLength += AOCL_ZSTD_count(ip0 + mLength, match0 + mLength, iend); \\\n        if (is_totalbits_limited_seq_possible(ip0, anchor, mLength, rep_offset1, MINMATCH)) { \\\n            /* write already calculated next hash table entry only if match is valid */ \\\n            if (safe) \\\n            { \\\n                hashTable[hash1] = (U32)(ip1 - base); \\\n            } \\\n            else \\\n            { \\\n                if (step <= 4) \\\n                { \\\n                    /* We need to avoid writing an index into the hash table >= ... */ \\\n                    hashTable[hash1] = (U32)(ip1 - base); \\\n                } \\\n            } \\\n            goto _store_sequences; \\\n        } \\\n        else \\\n            rep_offset1 = prev_rep_offset1; /* revert */ \\\n    } \\\n    else \\\n        rep_offset1 = prev_rep_offset1; /* revert */ \\' \
	     -e '/@AOCL_ZSTD_CFNGF_STORE_SEQ@/c\    assert((OFFBASE_IS_OFFSET(offcode) \&\& (OFFBASE_TO_OFFSET(offcode) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offcode)));\n    AOCL_ZSTD_storeSequences(seqStore, ip0, anchor, iend, offcode, mLength, MINMATCH);' \
	     -e 's|@AOCL_ZSTD_CFNGF_MATCH_STORED@||g' \
	     $< > $@
# ==============================================================================
# DECOMPRESSION DECODE SEQUENCE VARIANTS
# ==============================================================================

# Decode Sequence (standard)
$(ZSTD_DECOMP_DECODE): $(ZSTD_DECOMP_DECODE_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decode sequence..."
	@sed -e 's|@AOCL_ZSTD_DS_FUNC@|AOCL_ZSTD_decodeSequence|g' \
     -e '/@AOCL_ZSTD_DS_INIT@/c\            U32 const ofBase = ofDInfo->baseValue;\n    U16 const llNext = llDInfo->nextState;\n    U16 const mlNext = mlDInfo->nextState;\n    U16 const ofNext = ofDInfo->nextState;\n    U32 const llnbBits = llDInfo->nbBits; \n    U32 const mlnbBits = mlDInfo->nbBits;\n    U32 const ofnbBits = ofDInfo->nbBits;\n' \
	     -e 's|@AOCL_ZSTD_DS_OFBASE@|ofBase|g' \
     -e '/@AOCL_ZSTD_DS_SHIFT_PREVOFFSETS@/c\                    #if defined(_MSC_VER) && defined(__clang__)\n        __m128i regv = _mm_loadu_si128((__m128i const*)(&seqState->prevOffset[0]));\n        _mm_storeu_si128((__m128i*)(&seqState->prevOffset[1]), regv);\n    #else\n        seqState->prevOffset[2] = seqState->prevOffset[1];\n        seqState->prevOffset[1] = seqState->prevOffset[0];\n    #endif\n' \
     -e '/@AOCL_ZSTD_DS_UPDATE_FSE_STATE@/c\            AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateLL, \&seqState->DStream, llNext, llnbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateML, \&seqState->DStream, mlNext, mlnbBits);    /* <=  9 bits */\n    if (MEM_32bits()) BIT_reloadDStream(\&seqState->DStream);    /* <= 18 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateOffb, \&seqState->DStream, ofNext, ofnbBits);  /* <=  8 bits */\n' \
	     $< > $@

# Decode Sequence GCC
$(ZSTD_DECOMP_DECODE_GCC): $(ZSTD_DECOMP_DECODE_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decode sequence GCC..."
	@sed -e 's|@AOCL_ZSTD_DS_FUNC@|AOCL_ZSTD_decodeSequence_gcc|g' \
	     -e 's|@AOCL_ZSTD_DS_INIT@||g' \
	     -e 's|@AOCL_ZSTD_DS_OFBASE@|ofDInfo->baseValue|g' \
     -e '/@AOCL_ZSTD_DS_SHIFT_PREVOFFSETS@/c\                    seqState->prevOffset[2] = seqState->prevOffset[1];\n    seqState->prevOffset[1] = seqState->prevOffset[0];\n' \
     -e '/@AOCL_ZSTD_DS_UPDATE_FSE_STATE@/c\            AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateLL, \&seqState->DStream, llDInfo->nextState, llDInfo->nbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateML, \&seqState->DStream, mlDInfo->nextState, mlDInfo->nbBits);    /* <=  9 bits */\n    if (MEM_32bits()) BIT_reloadDStream(\&seqState->DStream);    /* <= 18 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateOffb, \&seqState->DStream, ofDInfo->nextState, ofDInfo->nbBits);  /* <=  8 bits */\n' \
	     $< > $@

# Decode Sequence Fast2
$(ZSTD_DECOMP_DECODE_F2): $(ZSTD_DECOMP_DECODE_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decode sequence fast2..."
	@sed -e 's|@AOCL_ZSTD_DSF2_FUNC@|AOCL_ZSTD_decodeSequence_mem64_fast2|g' \
     -e '/@AOCL_ZSTD_DSF2_INIT@/c\            U16 const llNext = llDInfo->nextState;\n    U16 const mlNext = mlDInfo->nextState;\n    U16 const ofNext = ofDInfo->nextState;\n    U32 const llnbBits = llDInfo->nbBits;\n    U32 const mlnbBits = mlDInfo->nbBits;\n    U32 const ofnbBits = ofDInfo->nbBits;\n' \
     -e '/@AOCL_ZSTD_DSF2_OFFSET@/c\                if (ofBits > 1) {\n        ZSTD_STATIC_ASSERT(ZSTD_lo_isLongOffset == 1);\n        ZSTD_STATIC_ASSERT(LONG_OFFSETS_MAX_EXTRA_BITS_32 == 5);\n        ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 > LONG_OFFSETS_MAX_EXTRA_BITS_32);\n        ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 - LONG_OFFSETS_MAX_EXTRA_BITS_32 >= MaxMLBits);\n\n        offset = ofDInfo->baseValue + BIT_readBitsFast(\&seqState->DStream, ofBits/*>0*/);   /* <=  (ZSTD_WINDOWLOG_MAX-1) bits */\n        seqState->prevOffset = offset;\n    }\n    else {\n        assert(ofBits == 0); /* Repeated_Offset1 */\n        offset = seqState->prevOffset;\n    }\n' \
     -e '/@AOCL_ZSTD_DSF2_UPDATE_FSE_STATE@/c\        AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateLL, \&seqState->DStream, llNext, llnbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateML, \&seqState->DStream, mlNext, mlnbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateOffb, \&seqState->DStream, ofNext, ofnbBits);  /* <=  8 bits */\n' \
	     $< > $@

# Decode Sequence GCC Fast2
$(ZSTD_DECOMP_DECODE_GCC_F2): $(ZSTD_DECOMP_DECODE_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decode sequence GCC fast2..."
	@sed -e 's|@AOCL_ZSTD_DSF2_FUNC@|AOCL_ZSTD_decodeSequence_mem64_gcc_fast2|g' \
	     -e 's|@AOCL_ZSTD_DSF2_INIT@||g' \
     -e '/@AOCL_ZSTD_DSF2_OFFSET@/c\                if (ofBits > 1) {\n        ZSTD_STATIC_ASSERT(ZSTD_lo_isLongOffset == 1);\n        ZSTD_STATIC_ASSERT(LONG_OFFSETS_MAX_EXTRA_BITS_32 == 5);\n        ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 > LONG_OFFSETS_MAX_EXTRA_BITS_32);\n        ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 - LONG_OFFSETS_MAX_EXTRA_BITS_32 >= MaxMLBits);\n\n        offset = ofDInfo->baseValue + BIT_readBitsFast(\&seqState->DStream, ofBits/*>0*/);   /* <=  (ZSTD_WINDOWLOG_MAX-1) bits */\n        seqState->prevOffset = offset;\n    }\n    else {\n        assert(ofBits == 0); /* Repeated_Offset1 */\n        offset = seqState->prevOffset;\n    }\n' \
     -e '/@AOCL_ZSTD_DSF2_UPDATE_FSE_STATE@/c\        AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateLL, \&seqState->DStream, llDInfo->nextState, llDInfo->nbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateML, \&seqState->DStream, mlDInfo->nextState, mlDInfo->nbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateOffb, \&seqState->DStream, ofDInfo->nextState, ofDInfo->nbBits);  /* <=  8 bits */\n' \
	     $< > $@

# Decode Sequence Fast3
$(ZSTD_DECOMP_DECODE_F3): $(ZSTD_DECOMP_DECODE_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decode sequence fast3..."
	@sed -e 's|@AOCL_ZSTD_DSF2_FUNC@|AOCL_ZSTD_decodeSequence_mem64_fast3|g' \
     -e '/@AOCL_ZSTD_DSF2_INIT@/c\            U16 const llNext = llDInfo->nextState;\n    U16 const mlNext = mlDInfo->nextState;\n    U16 const ofNext = ofDInfo->nextState;\n    U32 const llnbBits = llDInfo->nbBits;\n    U32 const mlnbBits = mlDInfo->nbBits;\n    U32 const ofnbBits = ofDInfo->nbBits;\n' \
     -e '/@AOCL_ZSTD_DSF2_OFFSET@/c\                ZSTD_STATIC_ASSERT(ZSTD_lo_isLongOffset == 1);\n    ZSTD_STATIC_ASSERT(LONG_OFFSETS_MAX_EXTRA_BITS_32 == 5);\n    ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 > LONG_OFFSETS_MAX_EXTRA_BITS_32);\n    ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 - LONG_OFFSETS_MAX_EXTRA_BITS_32 >= MaxMLBits);\n\n    offset = ofDInfo->baseValue + BIT_readBitsFast(\&seqState->DStream, ofBits/*>0*/);   /* <=  (ZSTD_WINDOWLOG_MAX-1) bits */\n    seqState->prevOffset = offset;\n' \
     -e '/@AOCL_ZSTD_DSF2_UPDATE_FSE_STATE@/c\        AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateLL, \&seqState->DStream, llNext, llnbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateML, \&seqState->DStream, mlNext, mlnbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateOffb, \&seqState->DStream, ofNext, ofnbBits);  /* <=  8 bits */\n' \
	     $< > $@

# Decode Sequence GCC Fast3
$(ZSTD_DECOMP_DECODE_GCC_F3): $(ZSTD_DECOMP_DECODE_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decode sequence GCC fast3..."
	@sed -e 's|@AOCL_ZSTD_DSF2_FUNC@|AOCL_ZSTD_decodeSequence_mem64_gcc_fast3|g' \
	     -e 's|@AOCL_ZSTD_DSF2_INIT@||g' \
     -e '/@AOCL_ZSTD_DSF2_OFFSET@/c\                ZSTD_STATIC_ASSERT(ZSTD_lo_isLongOffset == 1);\n    ZSTD_STATIC_ASSERT(LONG_OFFSETS_MAX_EXTRA_BITS_32 == 5);\n    ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 > LONG_OFFSETS_MAX_EXTRA_BITS_32);\n    ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 - LONG_OFFSETS_MAX_EXTRA_BITS_32 >= MaxMLBits);\n\n    offset = ofDInfo->baseValue + BIT_readBitsFast(\&seqState->DStream, ofBits/*>0*/);   /* <=  (ZSTD_WINDOWLOG_MAX-1) bits */\n    seqState->prevOffset = offset;\n' \
     -e '/@AOCL_ZSTD_DSF2_UPDATE_FSE_STATE@/c\        AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateLL, \&seqState->DStream, llDInfo->nextState, llDInfo->nbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateML, \&seqState->DStream, mlDInfo->nextState, mlDInfo->nbBits);    /* <=  9 bits */\n    AOCL_ZSTD_updateFseStateWithDInfo(\&seqState->stateOffb, \&seqState->DStream, ofDInfo->nextState, ofDInfo->nbBits);  /* <=  8 bits */\n' \
	     $< > $@

# ==============================================================================
# DECOMPRESSION EXEC SEQUENCE VARIANTS
# ==============================================================================

# Exec Sequence Fast2
$(ZSTD_DECOMP_EXEC_F2): $(ZSTD_DECOMP_EXEC_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD exec sequence fast2..."
	@sed -e 's|@AOCL_ZSTD_ESM_FUNC@|AOCL_ZSTD_execSequence_mem64_fast2|g' \
     -e '/@AOCL_ZSTD_ESM_COPY_MATCH@/c\    #ifdef AOCL_UNIT_TEST\n    RETURN_ERROR_IF(UNLIKELY(sequence.offset < WILDCOPY_VECLEN), corruption_detected, ""); /* not FDS compliant */\n#else\n    assert(sequence.offset >= WILDCOPY_VECLEN);\n#endif\n    /* We bet on a full wildcopy for matches, since we expect matches to be\n    * longer than literals (in general). */\n    AOCL_ZSTD_WILDCOPY_LONG_IMPL(op, match, (ptrdiff_t)sequence.matchLength, ZSTD_no_overlap);\n' \
	     $< > $@

# Exec Sequence NoDict
$(ZSTD_DECOMP_EXEC_NODICT): $(ZSTD_DECOMP_EXEC_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD exec sequence nodict..."
	@sed -e 's|@AOCL_ZSTD_ESM_FUNC@|AOCL_ZSTD_execSequence_mem64_noDict|g' \
     -e '/@AOCL_ZSTD_ESM_COPY_MATCH@/c\        /* Nearly all offsets are >= WILDCOPY_VECLEN bytes, which means we can use wildcopy\n    * without overlap checking. */\n    if (LIKELY(sequence.offset >= WILDCOPY_VECLEN)) {\n        /* We bet on a full wildcopy for matches, since we expect matches to be\n         * longer than literals (in general). */\n        AOCL_ZSTD_WILDCOPY_LONG_IMPL(op, match, (ptrdiff_t)sequence.matchLength, ZSTD_no_overlap);\n        return sequenceLength;\n    }\n    assert(sequence.offset < WILDCOPY_VECLEN);\n    /* Copy 8 bytes and spread the offset to be >= 8. */\n    ZSTD_overlapCopy8(\&op, \&match, sequence.offset);\n    /* If the match length is > 8 bytes, then continue with the wildcopy. */\n    if (sequence.matchLength > 8) {\n        assert(op < oMatchEnd);\n        ZSTD_wildcopy(op, match, (ptrdiff_t)sequence.matchLength - 8, ZSTD_overlap_src_before_dst);\n    }\n' \
	     $< > $@

# ==============================================================================
# DECOMPRESSION SEQUENCES BODY VARIANTS
# ==============================================================================

# Decompress Sequences Body (standard)
$(ZSTD_DECOMP_SEQS_BODY): $(ZSTD_DECOMP_SEQS_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decompress sequences body..."
	@sed -e 's|@AOCL_ZSTD_DSB_FUNC@|AOCL_ZSTD_decompressSequences_body|g' \
     -e '/@AOCL_ZSTD_DSB_INIT_SEQS_STATE@/c\            seqState_t seqState;\n    dctx->fseEntropy = 1;\n    { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) seqState.prevOffset[i] = dctx->entropy.rep[i]; }\n    RETURN_ERROR_IF(ERR_isError(BIT_initDStream(\&seqState.DStream, ip, iend - ip)), corruption_detected, "");\n    ZSTD_initFseState(\&seqState.stateLL, \&seqState.DStream, dctx->LLTptr);\n    ZSTD_initFseState(\&seqState.stateOffb, \&seqState.DStream, dctx->OFTptr);\n    ZSTD_initFseState(\&seqState.stateML, \&seqState.DStream, dctx->MLTptr);\n    assert(dst != NULL);\n    (\&seqState)->stateLL.state_ptr = (\&seqState)->stateLL.table + (\&seqState)->stateLL.state;\n    (\&seqState)->stateML.state_ptr = (\&seqState)->stateML.table + (\&seqState)->stateML.state;\n    (\&seqState)->stateOffb.state_ptr = (\&seqState)->stateOffb.table + (\&seqState)->stateOffb.state;\n' \
	     -e 's|@AOCL_ZSTD_DSB_DECODE_SEQUENCE@|AOCL_ZSTD_DECODESEQUENCE_MEM64|g' \
	     -e 's|@AOCL_ZSTD_DSB_EXEC_SEQUENCE@|AOCL_ZSTD_execSequence|g' \
     -e '/@AOCL_ZSTD_DSB_UPDATE_SEQS_STATE@/c\            { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) dctx->entropy.rep[i] = (U32)(seqState.prevOffset[i]); }\n' \
	     $< > $@

# Decompress Sequences Body Fast2
$(ZSTD_DECOMP_SEQS_F2): $(ZSTD_DECOMP_SEQS_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decompress sequences body fast2..."
	@sed -e 's|@AOCL_ZSTD_DSB_FUNC@|AOCL_ZSTD_decompressSequences_body_mem64_fast2|g' \
     -e '/@AOCL_ZSTD_DSB_INIT_SEQS_STATE@/c\            aocl_fast2_seqState_t seqState;\n    dctx->fseEntropy = 1;\n    seqState.prevOffset = dctx->entropy.rep[0];\n    RETURN_ERROR_IF(ERR_isError(BIT_initDStream(\&seqState.DStream, ip, iend - ip)), corruption_detected, "");\n    ZSTD_initFseState(\&seqState.stateLL, \&seqState.DStream, dctx->LLTptr);\n    ZSTD_initFseState(\&seqState.stateOffb, \&seqState.DStream, dctx->OFTptr);\n    ZSTD_initFseState(\&seqState.stateML, \&seqState.DStream, dctx->MLTptr);\n    assert(dst != NULL);\n    (\&seqState)->stateLL.state_ptr = (\&seqState)->stateLL.table + (\&seqState)->stateLL.state;\n    (\&seqState)->stateML.state_ptr = (\&seqState)->stateML.table + (\&seqState)->stateML.state;\n    (\&seqState)->stateOffb.state_ptr = (\&seqState)->stateOffb.table + (\&seqState)->stateOffb.state;\n' \
	     -e 's|@AOCL_ZSTD_DSB_DECODE_SEQUENCE@|AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS2|g' \
	     -e 's|@AOCL_ZSTD_DSB_EXEC_SEQUENCE@|AOCL_ZSTD_execSequence_mem64_fast2|g' \
     -e '/@AOCL_ZSTD_DSB_UPDATE_SEQS_STATE@/c\            /* During compression offsets that are not maintained are set to rep[0] to account for scenarios\n       where switch from FDS to non-FDS modes occur. Similar operation must be replicated in \n       decompression as well to account for this scenario. */\n    { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) dctx->entropy.rep[i] = (U32)(seqState.prevOffset); }\n' \
	     $< > $@

# Decompress Sequences Body Fast3
$(ZSTD_DECOMP_SEQS_F3): $(ZSTD_DECOMP_SEQS_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decompress sequences body fast3..."
	@sed -e 's|@AOCL_ZSTD_DSB_FUNC@|AOCL_ZSTD_decompressSequences_body_mem64_fast3|g' \
     -e '/@AOCL_ZSTD_DSB_INIT_SEQS_STATE@/c\            aocl_fast2_seqState_t seqState;\n    dctx->fseEntropy = 1;\n    seqState.prevOffset = dctx->entropy.rep[0];\n    RETURN_ERROR_IF(ERR_isError(BIT_initDStream(\&seqState.DStream, ip, iend - ip)), corruption_detected, "");\n    ZSTD_initFseState(\&seqState.stateLL, \&seqState.DStream, dctx->LLTptr);\n    ZSTD_initFseState(\&seqState.stateOffb, \&seqState.DStream, dctx->OFTptr);\n    ZSTD_initFseState(\&seqState.stateML, \&seqState.DStream, dctx->MLTptr);\n    assert(dst != NULL);\n    (\&seqState)->stateLL.state_ptr = (\&seqState)->stateLL.table + (\&seqState)->stateLL.state;\n    (\&seqState)->stateML.state_ptr = (\&seqState)->stateML.table + (\&seqState)->stateML.state;\n    (\&seqState)->stateOffb.state_ptr = (\&seqState)->stateOffb.table + (\&seqState)->stateOffb.state;\n' \
	     -e 's|@AOCL_ZSTD_DSB_DECODE_SEQUENCE@|AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS3|g' \
	     -e 's|@AOCL_ZSTD_DSB_EXEC_SEQUENCE@|AOCL_ZSTD_execSequence_mem64_fast2|g' \
     -e '/@AOCL_ZSTD_DSB_UPDATE_SEQS_STATE@/c\            /* During compression offsets that are not maintained are set to rep[0] to account for scenarios\n       where switch from FDS to non-FDS modes occur. Similar operation must be replicated in \n       decompression as well to account for this scenario. */\n    { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) dctx->entropy.rep[i] = (U32)(seqState.prevOffset); }\n' \
	     $< > $@

# Decompress Sequences Body Fast2 NOTB
$(ZSTD_DECOMP_SEQS_F2_NOTB): $(ZSTD_DECOMP_SEQS_FDS_TEMPLATE) | $(ZSTD_DECOMP_DIR)
	@echo "Generating ZSTD decompress sequences body fast2 NOTB..."
	@sed -e 's|@AOCL_ZSTD_DSB_FUNC@|AOCL_ZSTD_decompressSequences_body_mem64_fast2_NOTB_NOEXT_REP2|g' \
     -e '/@AOCL_ZSTD_DSB_INIT_SEQS_STATE@/c\            aocl_fast2_seqState_t seqState;\n    dctx->fseEntropy = 1;\n    seqState.prevOffset = dctx->entropy.rep[0];\n    RETURN_ERROR_IF(ERR_isError(BIT_initDStream(\&seqState.DStream, ip, iend - ip)), corruption_detected, "");\n    ZSTD_initFseState(\&seqState.stateLL, \&seqState.DStream, dctx->LLTptr);\n    ZSTD_initFseState(\&seqState.stateOffb, \&seqState.DStream, dctx->OFTptr);\n    ZSTD_initFseState(\&seqState.stateML, \&seqState.DStream, dctx->MLTptr);\n    assert(dst != NULL);\n    (\&seqState)->stateLL.state_ptr = (\&seqState)->stateLL.table + (\&seqState)->stateLL.state;\n    (\&seqState)->stateML.state_ptr = (\&seqState)->stateML.table + (\&seqState)->stateML.state;\n    (\&seqState)->stateOffb.state_ptr = (\&seqState)->stateOffb.table + (\&seqState)->stateOffb.state;\n' \
	     -e 's|@AOCL_ZSTD_DSB_DECODE_SEQUENCE@|AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS2|g' \
	     -e 's|@AOCL_ZSTD_DSB_EXEC_SEQUENCE@|AOCL_ZSTD_execSequence_mem64_noDict|g' \
     -e '/@AOCL_ZSTD_DSB_UPDATE_SEQS_STATE@/c\            /* During compression offsets that are not maintained are set to rep[0] to account for scenarios\n       where switch from FDS to non-FDS modes occur. Similar operation must be replicated in \n       decompression as well to account for this scenario. */\n    { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) dctx->entropy.rep[i] = (U32)(seqState.prevOffset); }\n' \
	     $< > $@

# ==============================================================================
# DIRECTORY CREATION
# ==============================================================================

$(ZSTD_GEN_DIR) $(ZSTD_DECOMP_DIR):
	@mkdir -p $@

# ==============================================================================
# CLEAN
# ==============================================================================

.PHONY: clean-zstd-templates

clean-zstd-templates:
	@rm -f $(ZSTD_GENERATED_HEADERS)
	@echo "✓ ZSTD generated templates cleaned"
