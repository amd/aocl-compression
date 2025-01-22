# Function template instantiation for zstd_lazy_aocl.h.in - start
set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds2_base")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "WILDCOPY_VECLEN")
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds2_base.h
)

set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds2_offset8")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "(WILDCOPY_VECLEN/2)")
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds2_offset8.h
)
# Function template instantiation for zstd_lazy_aocl.h.in - end

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
set(AOCL_ZSTD_DSF2_UPDATE_FSE_STATE [[
    ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llNext, llnbBits);    /* <=  9 bits */
    ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlNext, mlnbBits);    /* <=  9 bits */
    ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofNext, ofnbBits);  /* <=  8 bits */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence_mem64_fast2.h
)

set(AOCL_ZSTD_DSF2_FUNC "AOCL_ZSTD_decodeSequence_mem64_gcc_fast2")
set(AOCL_ZSTD_DSF2_INIT "")
set(AOCL_ZSTD_DSF2_UPDATE_FSE_STATE [[
    ZSTD_updateFseStateWithDInfo(&seqState->stateLL, &seqState->DStream, llDInfo->nextState, llDInfo->nbBits);    /* <=  9 bits */
    ZSTD_updateFseStateWithDInfo(&seqState->stateML, &seqState->DStream, mlDInfo->nextState, mlDInfo->nbBits);    /* <=  9 bits */
    ZSTD_updateFseStateWithDInfo(&seqState->stateOffb, &seqState->DStream, ofDInfo->nextState, ofDInfo->nbBits);  /* <=  8 bits */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decode_sequence_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decodeSequence_mem64_gcc_fast2.h
)
# Function template instantiation for zstd_decompress_block_decode_sequence_fds_aocl.h.in - end

# Function template instantiation for zstd_decompress_block_exec_sequence_fds_aocl.h.in - start
set(AOCL_ZSTD_ESM_FUNC "AOCL_ZSTD_execSequence_mem64_fast2")
set(AOCL_ZSTD_ESM_COPY_MATCH [[
    assert(sequence.offset >= WILDCOPY_VECLEN);
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
]])
set(AOCL_ZSTD_DSB_DECODE_SEQUENCE "AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS")
set(AOCL_ZSTD_DSB_EXEC_SEQUENCE "AOCL_ZSTD_execSequence_mem64_fast2")
set(AOCL_ZSTD_DSB_UPDATE_SEQS_STATE [[
    dctx->entropy.rep[0] = (U32)(seqState.prevOffset);
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decompress_sequences_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decompressSequences_body_mem64_fast2.h
)

set(AOCL_ZSTD_DSB_FUNC "AOCL_ZSTD_decompressSequences_body_mem64_fast2_NOTB_NOEXT_REP2")
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
set(AOCL_ZSTD_DSB_DECODE_SEQUENCE "AOCL_ZSTD_DECODESEQUENCE_MEM64_FDS")
set(AOCL_ZSTD_DSB_EXEC_SEQUENCE "AOCL_ZSTD_execSequence_mem64_noDict")
set(AOCL_ZSTD_DSB_UPDATE_SEQS_STATE [[
    dctx->entropy.rep[0] = (U32)(seqState.prevOffset);
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block_decompress_sequences_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/decompress/aocl_zstd_decompressSequences_body_mem64_fast2_NOTB_NOEXT_REP2.h
)
# Function template instantiation for zstd_decompress_block_decompress_sequences_fds_aocl.h.in - end
