# Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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
 
# @file zstd_decompress_config.cmake
# 
# @brief Function templates for aocl optimized zstd decompress functions

# Call stack for AOCL decompress block based on FDS metadata received:
# + FDS_FAST2_NOTB_SO4_NOEXT_REP3:
#     AOCL_ZSTD_decompressSequences_body_mem64_fast3
#         AOCL_ZSTD_decodeSequence_mem64_gcc_fast3 / AOCL_ZSTD_decodeSequence_mem64_fast3
#         AOCL_ZSTD_execSequence_mem64_fast2
# 
# + FDS_FAST2_NOTB_SO4_NOEXT_REP2:
#     AOCL_ZSTD_decompressSequences_body_mem64_fast2
#         AOCL_ZSTD_decodeSequence_mem64_gcc_fast2 / AOCL_ZSTD_decodeSequence_mem64_fast2
#         AOCL_ZSTD_execSequence_mem64_fast2
# 
# + FDS_FAST2_NOTB_SO3_NOEXT_REP2:
#     AOCL_ZSTD_decompressSequences_body_mem64_fast2_NOTB_NOEXT_REP2
#         AOCL_ZSTD_decodeSequence_mem64_gcc_fast2 / AOCL_ZSTD_decodeSequence_mem64_fast2
#         AOCL_ZSTD_execSequence_mem64_noDict
# 
# + FDS_NONE
#     AOCL_ZSTD_decompressSequences_body
#         AOCL_ZSTD_decodeSequence_gcc / AOCL_ZSTD_decodeSequence
#         AOCL_ZSTD_execSequence
 
# Function template instantiation for zstd_decompress_block_decode_sequence_aocl.h.in - start
set(AOCL_ZSTD_DS_FUNC "AOCL_ZSTD_decodeSequence")
set(AOCL_ZSTD_DS_INIT [[
    U32 const ofBase = ofDInfo->baseValue;
    U16 const llNext = llDInfo->nextState;
    U16 const mlNext = mlDInfo->nextState;
    U16 const ofNext = ofDInfo->nextState;
    U32 const llnbBits = llDInfo->nbBits; 
    U32 const mlnbBits = mlDInfo->nbBits;
    U32 const ofnbBits = ofDInfo->nbBits;
]])
set(AOCL_ZSTD_DS_OFBASE "ofBase")
set(AOCL_ZSTD_DS_SHIFT_PREVOFFSETS [[
    #if defined(_MSC_VER) && defined(__clang__)
        __m128i regv = _mm_loadu_si128((__m128i const*)(&seqState->prevOffset[0]));
        _mm_storeu_si128((__m128i*)(&seqState->prevOffset[1]), regv);
    #else
        seqState->prevOffset[2] = seqState->prevOffset[1];
        seqState->prevOffset[1] = seqState->prevOffset[0];
    #endif
]])
set(AOCL_ZSTD_DS_UPDATE_FSE_STATE [[
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llNext, llnbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlNext, mlnbBits);    /* <=  9 bits */
    if (MEM_32bits()) BIT_reloadDStream(&seqState->DStream);    /* <= 18 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofNext, ofnbBits);  /* <=  8 bits */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence.h
)

set(AOCL_ZSTD_DS_FUNC "AOCL_ZSTD_decodeSequence_gcc")
set(AOCL_ZSTD_DS_INIT "")
set(AOCL_ZSTD_DS_OFBASE "ofDInfo->baseValue")
set(AOCL_ZSTD_DS_SHIFT_PREVOFFSETS [[
    seqState->prevOffset[2] = seqState->prevOffset[1];
    seqState->prevOffset[1] = seqState->prevOffset[0];
]])
set(AOCL_ZSTD_DS_UPDATE_FSE_STATE [[
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llDInfo->nextState, llDInfo->nbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlDInfo->nextState, mlDInfo->nbBits);    /* <=  9 bits */
    if (MEM_32bits()) BIT_reloadDStream(&seqState->DStream);    /* <= 18 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofDInfo->nextState, ofDInfo->nbBits);  /* <=  8 bits */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence_gcc.h
)
# Function template instantiation for zstd_decompress_block_decode_sequence_aocl.h.in - end

# Function template instantiation for zstd_decompress_block_decode_sequence_fds_aocl.h.in - start
set(AOCL_ZSTD_DSF2_FUNC "AOCL_ZSTD_decodeSequence_mem64_fast2")
set(AOCL_ZSTD_DSF2_INIT [[
    U16 const llNext = llDInfo->nextState;
    U16 const mlNext = mlDInfo->nextState;
    U16 const ofNext = ofDInfo->nextState;
    U32 const llnbBits = llDInfo->nbBits;
    U32 const mlnbBits = mlDInfo->nbBits;
    U32 const ofnbBits = ofDInfo->nbBits;
]])
set(AOCL_ZSTD_DSF2_OFFSET [[
    if (ofBits > 1) {
        ZSTD_STATIC_ASSERT(ZSTD_lo_isLongOffset == 1);
        ZSTD_STATIC_ASSERT(LONG_OFFSETS_MAX_EXTRA_BITS_32 == 5);
        ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 > LONG_OFFSETS_MAX_EXTRA_BITS_32);
        ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 - LONG_OFFSETS_MAX_EXTRA_BITS_32 >= MaxMLBits);

        offset = ofDInfo->baseValue + BIT_readBitsFast(&seqState->DStream, ofBits/*>0*/);   /* <=  (ZSTD_WINDOWLOG_MAX-1) bits */
        seqState->prevOffset = offset;
    }
    else {
        assert(ofBits == 0); /* Repeated_Offset1 */
        offset = seqState->prevOffset;
    }
]])
set(AOCL_ZSTD_DSF2_UPDATE_FSE_STATE [[
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llNext, llnbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlNext, mlnbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofNext, ofnbBits);  /* <=  8 bits */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence_mem64_fast2.h
)

set(AOCL_ZSTD_DSF2_FUNC "AOCL_ZSTD_decodeSequence_mem64_gcc_fast2")
set(AOCL_ZSTD_DSF2_INIT "")
set(AOCL_ZSTD_DSF2_UPDATE_FSE_STATE [[
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llDInfo->nextState, llDInfo->nbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlDInfo->nextState, mlDInfo->nbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofDInfo->nextState, ofDInfo->nbBits);  /* <=  8 bits */
]])
#set(AOCL_ZSTD_DSF2_OFFSET) same as above
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence_mem64_gcc_fast2.h
)

set(AOCL_ZSTD_DSF2_FUNC "AOCL_ZSTD_decodeSequence_mem64_fast3")
set(AOCL_ZSTD_DSF2_INIT [[
    U16 const llNext = llDInfo->nextState;
    U16 const mlNext = mlDInfo->nextState;
    U16 const ofNext = ofDInfo->nextState;
    U32 const llnbBits = llDInfo->nbBits;
    U32 const mlnbBits = mlDInfo->nbBits;
    U32 const ofnbBits = ofDInfo->nbBits;
]])
set(AOCL_ZSTD_DSF2_OFFSET [[
    ZSTD_STATIC_ASSERT(ZSTD_lo_isLongOffset == 1);
    ZSTD_STATIC_ASSERT(LONG_OFFSETS_MAX_EXTRA_BITS_32 == 5);
    ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 > LONG_OFFSETS_MAX_EXTRA_BITS_32);
    ZSTD_STATIC_ASSERT(STREAM_ACCUMULATOR_MIN_32 - LONG_OFFSETS_MAX_EXTRA_BITS_32 >= MaxMLBits);

    offset = ofDInfo->baseValue + BIT_readBitsFast(&seqState->DStream, ofBits/*>0*/);   /* <=  (ZSTD_WINDOWLOG_MAX-1) bits */
    seqState->prevOffset = offset;
]])
set(AOCL_ZSTD_DSF2_UPDATE_FSE_STATE [[
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llNext, llnbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlNext, mlnbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofNext, ofnbBits);  /* <=  8 bits */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence_mem64_fast3.h
)

set(AOCL_ZSTD_DSF2_FUNC "AOCL_ZSTD_decodeSequence_mem64_gcc_fast3")
set(AOCL_ZSTD_DSF2_INIT "")
#set(AOCL_ZSTD_DSF2_OFFSET) same as above
set(AOCL_ZSTD_DSF2_UPDATE_FSE_STATE [[
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llDInfo->nextState, llDInfo->nbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlDInfo->nextState, mlDInfo->nbBits);    /* <=  9 bits */
    AOCL_ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofDInfo->nextState, ofDInfo->nbBits);  /* <=  8 bits */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence_mem64_gcc_fast3.h
)
# Function template instantiation for zstd_decompress_block_decode_sequence_fds_aocl.h.in - end

# Function template instantiation for zstd_decompress_block_exec_sequence_fds_aocl.h.in - start
set(AOCL_ZSTD_ESM_FUNC "AOCL_ZSTD_execSequence_mem64_fast2")
set(AOCL_ZSTD_ESM_COPY_MATCH [[
#ifdef AOCL_UNIT_TEST
    RETURN_ERROR_IF(UNLIKELY(sequence.offset < WILDCOPY_VECLEN), corruption_detected, ""); /* not FDS compliant */
#else
    assert(sequence.offset >= WILDCOPY_VECLEN);
#endif
    /* We bet on a full wildcopy for matches, since we expect matches to be
    * longer than literals (in general). */
    AOCL_ZSTD_WILDCOPY_LONG_IMPL(op, match, (ptrdiff_t)sequence.matchLength, ZSTD_no_overlap);
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_exec_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_execSequence_mem64_fast2.h
)

set(AOCL_ZSTD_ESM_FUNC "AOCL_ZSTD_execSequence_mem64_noDict")
set(AOCL_ZSTD_ESM_COPY_MATCH [[
    /* Nearly all offsets are >= WILDCOPY_VECLEN bytes, which means we can use wildcopy
    * without overlap checking. */
    if (LIKELY(sequence.offset >= WILDCOPY_VECLEN)) {
        /* We bet on a full wildcopy for matches, since we expect matches to be
         * longer than literals (in general). */
        AOCL_ZSTD_WILDCOPY_LONG_IMPL(op, match, (ptrdiff_t)sequence.matchLength, ZSTD_no_overlap);
        return sequenceLength;
    }
    assert(sequence.offset < WILDCOPY_VECLEN);
    /* Copy 8 bytes and spread the offset to be >= 8. */
    ZSTD_overlapCopy8(&op, &match, sequence.offset);
    /* If the match length is > 8 bytes, then continue with the wildcopy. */
    if (sequence.matchLength > 8) {
        assert(op < oMatchEnd);
        ZSTD_wildcopy(op, match, (ptrdiff_t)sequence.matchLength - 8, ZSTD_overlap_src_before_dst);
    }
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_exec_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_execSequence_mem64_nodict.h
)
# Function template instantiation for zstd_decompress_block_exec_sequence_fds_aocl.h.in - end

# Function template instantiation for zstd_decompress_block_decompress_sequences_fds_aocl.h.in - start
set(AOCL_ZSTD_DSB_FUNC "AOCL_ZSTD_decompressSequences_body")
set(AOCL_ZSTD_DSB_INIT_SEQS_STATE [[
    seqState_t seqState;
    dctx->fseEntropy = 1;
    { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) seqState.prevOffset[i] = dctx->entropy.rep[i]; }
    RETURN_ERROR_IF(ERR_isError(BIT_initDStream(&seqState.DStream, ip, iend - ip)), corruption_detected, "");
    ZSTD_initFseState(&seqState.stateLL, &seqState.DStream, dctx->LLTptr);
    ZSTD_initFseState(&seqState.stateOffb, &seqState.DStream, dctx->OFTptr);
    ZSTD_initFseState(&seqState.stateML, &seqState.DStream, dctx->MLTptr);
    assert(dst != NULL);
    (&seqState)->stateLL.state_ptr = (&seqState)->stateLL.table + (&seqState)->stateLL.state;
    (&seqState)->stateML.state_ptr = (&seqState)->stateML.table + (&seqState)->stateML.state;
    (&seqState)->stateOffb.state_ptr = (&seqState)->stateOffb.table + (&seqState)->stateOffb.state;
]])
set(AOCL_ZSTD_DSB_DECODE_SEQUENCE "AOCL_ZSTD_DECODESEQUENCE_MEM64")
set(AOCL_ZSTD_DSB_EXEC_SEQUENCE "AOCL_ZSTD_execSequence")
set(AOCL_ZSTD_DSB_UPDATE_SEQS_STATE [[
    { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) dctx->entropy.rep[i] = (U32)(seqState.prevOffset[i]); }
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decompress_sequences_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decompressSequences_body.h
)

set(AOCL_ZSTD_DSB_FUNC "AOCL_ZSTD_decompressSequences_body_mem64_fast2")
set(AOCL_ZSTD_DSB_INIT_SEQS_STATE [[
    aocl_fast2_seqState_t seqState;
    dctx->fseEntropy = 1;
    seqState.prevOffset = dctx->entropy.rep[0];
    RETURN_ERROR_IF(ERR_isError(BIT_initDStream(&seqState.DStream, ip, iend - ip)), corruption_detected, "");
    ZSTD_initFseState(&seqState.stateLL, &seqState.DStream, dctx->LLTptr);
    ZSTD_initFseState(&seqState.stateOffb, &seqState.DStream, dctx->OFTptr);
    ZSTD_initFseState(&seqState.stateML, &seqState.DStream, dctx->MLTptr);
    assert(dst != NULL);
    (&seqState)->stateLL.state_ptr = (&seqState)->stateLL.table + (&seqState)->stateLL.state;
    (&seqState)->stateML.state_ptr = (&seqState)->stateML.table + (&seqState)->stateML.state;
    (&seqState)->stateOffb.state_ptr = (&seqState)->stateOffb.table + (&seqState)->stateOffb.state;
]])
set(AOCL_ZSTD_DSB_DECODE_SEQUENCE "AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS2")
set(AOCL_ZSTD_DSB_EXEC_SEQUENCE "AOCL_ZSTD_execSequence_mem64_fast2")
set(AOCL_ZSTD_DSB_UPDATE_SEQS_STATE [[
    /* During compression offsets that are not maintained are set to rep[0] to account for scenarios
       where switch from FDS to non-FDS modes occur. Similar operation must be replicated in 
       decompression as well to account for this scenario. */
    { U32 i; for (i = 0; i < ZSTD_REP_NUM; i++) dctx->entropy.rep[i] = (U32)(seqState.prevOffset); }
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decompress_sequences_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decompressSequences_body_mem64_fast2.h
)

set(AOCL_ZSTD_DSB_FUNC "AOCL_ZSTD_decompressSequences_body_mem64_fast3")
#set(AOCL_ZSTD_DSB_INIT_SEQS_STATE) same as above
set(AOCL_ZSTD_DSB_DECODE_SEQUENCE "AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS3")
set(AOCL_ZSTD_DSB_EXEC_SEQUENCE "AOCL_ZSTD_execSequence_mem64_fast2")
#set(AOCL_ZSTD_DSB_UPDATE_SEQS_STATE) same as above
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decompress_sequences_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decompressSequences_body_mem64_fast3.h
)

set(AOCL_ZSTD_DSB_FUNC "AOCL_ZSTD_decompressSequences_body_mem64_fast2_NOTB_NOEXT_REP2")
#set(AOCL_ZSTD_DSB_INIT_SEQS_STATE) same as above
set(AOCL_ZSTD_DSB_DECODE_SEQUENCE "AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS2")
set(AOCL_ZSTD_DSB_EXEC_SEQUENCE "AOCL_ZSTD_execSequence_mem64_noDict")
#set(AOCL_ZSTD_DSB_UPDATE_SEQS_STATE) same as above
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decompress_sequences_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decompressSequences_body_mem64_fast2_NOTB_NOEXT_REP2.h
)
# Function template instantiation for zstd_decompress_block_decompress_sequences_fds_aocl.h.in - end
