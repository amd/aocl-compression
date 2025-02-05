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
 
# @file zstd_compress_config.cmake
# 
# @brief Function templates for aocl optimized zstd compress functions

# Summary of functions used in AOCL compress block based on AOCL_DECOMPRESS_FAST settings and ZSTD_strategy:
# ---------------------- | ---------------------- | -------------------------| ------------------------------------------------------------| ---------------------------------------------------------------|
# ZSTD_strategy          | AOCL_DECOMPRESS_FAST=0 | AOCL_DECOMPRESS_FAST=1   | AOCL_DECOMPRESS_FAST=2                                      | AOCL_DECOMPRESS_FAST=3                                         |
# ---------------------- | ---------------------- | -------------------------| ------------------------------------------------------------| ---------------------------------------------------------------|
# ZSTD_fast              | AOCL_ZSTD_compressBlock_fast_noDict_generic       | AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_base       | AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_analyze,      |
#                        |                                                   |                                                             | AOCL_ZSTD_compressBlock_fast_noDict_generic                    |
# ---------------------- | ---------------------- | -------------------------| ------------------------------------------------------------| ---------------------------------------------------------------|
# ZSTD_dfast             | AOCL_ZSTD_compressBlock_doubleFast_noDict_generic | AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_base | AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_analyze,| 
#                        |                                                   |                                                             | AOCL_ZSTD_compressBlock_doubleFast_noDict_generic              |
# ---------------------- | ---------------------- | -------------------------| ------------------------------------------------------------| ---------------------------------------------------------------|
# ZSTD_greedy/lazy/lazy2 | AOCL_ZSTD_compressBlock_lazy_generic              | AOCL_ZSTD_compressBlock_lazy_fds2_base                      | AOCL_ZSTD_compressBlock_lazy_fds2_analyze,                     |
#                        |                                                   |                                                             | AOCL_ZSTD_compressBlock_lazy_fds3_base,                        |
#                        |                                                   |                                                             | AOCL_ZSTD_compressBlock_lazy_fds2_offset8,                     |
#                        |                                                   |                                                             | AOCL_ZSTD_compressBlock_lazy_generic                           |
# ---------------------- | ---------------------- | -------------------------| ------------------------------------------------------------| ---------------------------------------------------------------|

# Function template instantiation for zstd_lazy_aocl.h.in - start
set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds2_base")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "WILDCOPY_VECLEN")
set(AOCL_ZSTD_CLNF_REP_MATCH_D0_TEMPLATE [[
    /* check repCode 
    * (offset_1 > 0) changed to (offset_1 >= @AOCL_ZSTD_CLNF_MIN_OFFSET@) as initial rep codes 
    * can be < @AOCL_ZSTD_CLNF_MIN_OFFSET@ and offsets < @AOCL_ZSTD_CLNF_MIN_OFFSET@ should not be included in the sequences
    */
    if ((offset_1 >= @AOCL_ZSTD_CLNF_MIN_OFFSET@) & (MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1))) {
        matchLength = ZSTD_count(ip + 1 + 4, ip + 1 + 4 - offset_1, iend) + 4;
        if ((depth == 0) && is_totalbits_limited_seq_possible(ip, anchor, matchLength, 1 /* Repeated_Offset1 */))
            goto _storeSequence;
    }
]])
set(AOCL_ZSTD_CLNF_REP_MATCH_D1_TEMPLATE [[
    if ((offBase) && ((offset_1 >= @AOCL_ZSTD_CLNF_MIN_OFFSET@) & (MEM_read32(ip) == MEM_read32(ip - offset_1)))) {
        size_t const mlRep = ZSTD_count(ip + 4, ip + 4 - offset_1, iend) + 4;
        int const gain2 = (int)(mlRep * 3);
        int const gain1 = (int)(matchLength * 3 - ZSTD_highbit32((U32)offBase) + 1);
        if ((mlRep >= 4) && (gain2 > gain1))
            matchLength = mlRep, offBase = REPCODE1_TO_OFFBASE, start = ip;
    }
]])
set(AOCL_ZSTD_CLNF_REP_MATCH_D2_TEMPLATE [[
    if ((offBase) && ((offset_1 >= @AOCL_ZSTD_CLNF_MIN_OFFSET@) & (MEM_read32(ip) == MEM_read32(ip - offset_1)))) {
        size_t const mlRep = ZSTD_count(ip + 4, ip + 4 - offset_1, iend) + 4;
        int const gain2 = (int)(mlRep * 4);
        int const gain1 = (int)(matchLength * 4 - ZSTD_highbit32((U32)offBase) + 1);
        if ((mlRep >= 4) && (gain2 > gain1))
            matchLength = mlRep, offBase = REPCODE1_TO_OFFBASE, start = ip;
    }
]])
string(REPLACE "@AOCL_ZSTD_CLNF_MIN_OFFSET@" "${AOCL_ZSTD_CLNF_MIN_OFFSET}" AOCL_ZSTD_CLNF_REP_MATCH_D0 "${AOCL_ZSTD_CLNF_REP_MATCH_D0_TEMPLATE}")
string(REPLACE "@AOCL_ZSTD_CLNF_MIN_OFFSET@" "${AOCL_ZSTD_CLNF_MIN_OFFSET}" AOCL_ZSTD_CLNF_REP_MATCH_D1 "${AOCL_ZSTD_CLNF_REP_MATCH_D1_TEMPLATE}")
string(REPLACE "@AOCL_ZSTD_CLNF_MIN_OFFSET@" "${AOCL_ZSTD_CLNF_MIN_OFFSET}" AOCL_ZSTD_CLNF_REP_MATCH_D2 "${AOCL_ZSTD_CLNF_REP_MATCH_D2_TEMPLATE}")
set(AOCL_ZSTD_CLNF_LONG_MATCH "")
set(AOCL_ZSTD_CLNF_STORE_SEQ [[
_storeSequence:
    assert((OFFBASE_IS_OFFSET(offBase) && (OFFBASE_TO_OFFSET(offBase) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offBase)));
    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength);
]])
set(AOCL_ZSTD_CLNF_SAVE_REP [[
    rep[0] = offset_1 ? offset_1 : offsetSaved1;
    for (int i=1; i<ZSTD_REP_NUM; ++i) rep[i] = 0; /* reset as these have not been maintained */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds2_base.h
)

set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds2_offset8")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "(WILDCOPY_VECLEN/2)")
string(REPLACE "@AOCL_ZSTD_CLNF_MIN_OFFSET@" "${AOCL_ZSTD_CLNF_MIN_OFFSET}" AOCL_ZSTD_CLNF_REP_MATCH_D0 "${AOCL_ZSTD_CLNF_REP_MATCH_D0_TEMPLATE}")
string(REPLACE "@AOCL_ZSTD_CLNF_MIN_OFFSET@" "${AOCL_ZSTD_CLNF_MIN_OFFSET}" AOCL_ZSTD_CLNF_REP_MATCH_D1 "${AOCL_ZSTD_CLNF_REP_MATCH_D1_TEMPLATE}")
string(REPLACE "@AOCL_ZSTD_CLNF_MIN_OFFSET@" "${AOCL_ZSTD_CLNF_MIN_OFFSET}" AOCL_ZSTD_CLNF_REP_MATCH_D2 "${AOCL_ZSTD_CLNF_REP_MATCH_D2_TEMPLATE}")
#set(AOCL_ZSTD_CLNF_LONG_MATCH same as above)
set(AOCL_ZSTD_CLNF_STORE_SEQ [[
_storeSequence:
    assert((OFFBASE_IS_OFFSET(offBase) && (OFFBASE_TO_OFFSET(offBase) >= (WILDCOPY_VECLEN/2))) || (!OFFBASE_IS_OFFSET(offBase)));
    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength);
]])
#set(AOCL_ZSTD_CLNF_SAVE_REP same as above)
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds2_offset8.h
)

set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds2_analyze")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "WILDCOPY_VECLEN")
set(AOCL_ZSTD_CLNF_REP_MATCH_D0 "")
set(AOCL_ZSTD_CLNF_REP_MATCH_D1 "")
set(AOCL_ZSTD_CLNF_REP_MATCH_D2 "")
set(AOCL_ZSTD_CLNF_LONG_MATCH [[
    /* If its a long overlapping match; do not skip it,
     * instead mark state as FDS_NONE and continue to save the match */
    U32 offset_offBase = OFFBASE_TO_OFFSET(offBase);
    if (UNLIKELY(matchLength > AOCL_LONG_MATCH_LIMIT_LAZY && offset_offBase > 0 && offset_offBase < WILDCOPY_VECLEN))
    {
        seqStore->fds_config.state = FDS_NONE; // stop imposing constraints from next block
        goto _storeSequence;
    }
    else
]])
set(AOCL_ZSTD_CLNF_STORE_SEQ [[
_storeSequence:
    if (seqStore->fds_config.state == FDS_NONE) 
    {
        ZSTD_storeSeq(seqStore, (size_t)(start-anchor), anchor, iend, (U32)offBase, matchLength);
    }
    else
    {
        AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, (U32)offBase, matchLength);
    }
]])
set(AOCL_ZSTD_CLNF_SAVE_REP [[
    (void)offsetSaved1;
    for (int i=0; i<ZSTD_REP_NUM; ++i) rep[i] = 0; /* reset as these have not been maintained */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds2_analyze.h
)

set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds3_base")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "WILDCOPY_VECLEN")
set(AOCL_ZSTD_CLNF_REP_MATCH_D0 "")
set(AOCL_ZSTD_CLNF_REP_MATCH_D1 "")
set(AOCL_ZSTD_CLNF_REP_MATCH_D2 "")
set(AOCL_ZSTD_CLNF_LONG_MATCH "")
set(AOCL_ZSTD_CLNF_STORE_SEQ [[
    assert((OFFBASE_IS_OFFSET(offBase) && (OFFBASE_TO_OFFSET(offBase) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offBase)));
    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength);
]])
#set(AOCL_ZSTD_CLNF_SAVE_REP same as above)
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds3_base.h
)

set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds3_offset8")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "(WILDCOPY_VECLEN/2)")
#set(AOCL_ZSTD_CLNF_REP_MATCH_D0 same as above)
#set(AOCL_ZSTD_CLNF_REP_MATCH_D1 same as above)
#set(AOCL_ZSTD_CLNF_REP_MATCH_D2 same as above)
#set(AOCL_ZSTD_CLNF_LONG_MATCH same as above)
set(AOCL_ZSTD_CLNF_STORE_SEQ [[
    assert((OFFBASE_IS_OFFSET(offBase) && (OFFBASE_TO_OFFSET(offBase) >= (WILDCOPY_VECLEN/2))) || (!OFFBASE_IS_OFFSET(offBase)));
    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength);
]])
#set(AOCL_ZSTD_CLNF_SAVE_REP same as above)
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds3_offset8.h
)
# Function template instantiation for zstd_lazy_aocl.h.in - end

# Function template instantiation for zstd_double_fast_noDict_generic_fds_aocl.h.in - start
set(AOCL_ZSTD_CDNGF_COMMENT [[
    /*
    * Derived from AOCL_ZSTD_compressBlock_doubleFast_noDict_generic, but enforces :
    *   totalBits < (STREAM_ACCUMULATOR_MIN_64 - (LLFSELog + MLFSELog + OffFSELog)
    *   no Repeated_Offset2 and Repeated_Offset3
    *   no offsets go into extDict (no offset beyond prefix)
    *   offset >= WILDCOPY_VECLEN
    * Calls AOCL_ZSTD_storeSequences to store sequences
    */
]])
set(AOCL_ZSTD_CDNGF_FUNC "AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_base")
set(AOCL_ZSTD_CDNGF_REP_MATCH [[
    /* check noDict repcode
    * (offset_1 > 0) changed to (offset_1 >= WILDCOPY_VECLEN) as initial rep codes
    * can be < WILDCOPY_VECLEN and offsets < WILDCOPY_VECLEN should not be included in the sequences
    */
    if ((offset_1 >= WILDCOPY_VECLEN) & (MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1))) 
    {
        PREFETCH_MATCH_L(ip + 1 + 4);
        mLength = AOCL_ZSTD_count(ip + 1 + 4, ip + 1 + 4 - offset_1, iend) + 4;
        ip++;

        if (is_totalbits_limited_seq_possible(ip, anchor, mLength, 1 /* Repeated_Offset1 */)) 
        {
            ZSTD_STORE_SEQ(seqStore, (size_t)(ip - anchor), anchor, iend, REPCODE1_TO_OFFBASE, mLength);
            goto _match_stored;
        }
    }
]])
set(AOCL_ZSTD_CDNGF_LONG_MATCH [[
    /* check prefix long match */
    if (((ip - matchl0) >= WILDCOPY_VECLEN) && (MEM_read64(matchl0) == MEM_read64(ip))) 
    {
        PREFETCH_MATCH_L(ip1 + 8);
        PREFETCH_MATCH_L(ip1 + 9);
        PREFETCH_MATCH_L(ip1 + 10);
        PREFETCH_MATCH_L(ip1 + 11);
        mLength = AOCL_ZSTD_count(ip + 8, matchl0 + 8, iend) + 8;
        offset = (U32)(ip - matchl0);
        ipfwd = ip;
        while (((ip > anchor) & (matchl0 > prefixLowest)) && (ip[-1] == matchl0[-1])) { ip--; matchl0--; mLength++; } /* catch up */

        if (is_totalbits_limited_seq_possible(ip, anchor, mLength, offset)) 
        {
            goto _match_found;
        }
        else 
        {
            ip = ipfwd; //revert
        }
    }
]])
set(AOCL_ZSTD_CDNGF_SHORT_MATCH [[
    /* check prefix short match */
    if (((ip - matchs0) >= WILDCOPY_VECLEN) && (MEM_read32(matchs0) == MEM_read32(ip))) 
    {
        PREFETCH_MATCH_S(ip + 4);
        goto _search_next_long;
    }
]])
set(AOCL_ZSTD_CDNGF_STORE_SEQ [[
    assert((OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset)) && (offset >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(OFFSET_TO_OFFBASE(offset))));
    AOCL_ZSTD_storeSequences(seqStore, ip, anchor, iend, OFFSET_TO_OFFBASE(offset), mLength);
]])
set(AOCL_ZSTD_CDNGF_SAVE_REP [[
    rep[0] = offset_1 ? offset_1 : offsetSaved1;
    for (int i=1; i<ZSTD_REP_NUM; ++i) rep[i] = 0; /* reset as these have not been maintained */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_double_fast_noDict_generic_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_doubleFast_noDict_generic_fds2_base.h
)

set(AOCL_ZSTD_CDNGF_COMMENT [[
    /*
    * Derived from AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_base.
    * Additionally checks for long matches with offset < WILDCOPY_VECLEN. Such 
    * matches can be handled significantly faster during decompresion. Hence,
    * if encountered stop imposing constraints and mark state as FDS_NONE.
    */
]])
set(AOCL_ZSTD_CDNGF_FUNC "AOCL_ZSTD_compressBlock_doubleFast_noDict_generic_fds2_analyze")
set(AOCL_ZSTD_CDNGF_REP_MATCH "")
set(AOCL_ZSTD_CDNGF_LONG_MATCH [[
    /* check prefix long match */
    if (LIKELY((ip - matchl0) >= WILDCOPY_VECLEN))
    {
        if ((MEM_read64(matchl0) == MEM_read64(ip))) 
        {
            PREFETCH_MATCH_L(ip1 + 8);
            PREFETCH_MATCH_L(ip1 + 9);
            PREFETCH_MATCH_L(ip1 + 10);
            PREFETCH_MATCH_L(ip1 + 11);
            mLength = AOCL_ZSTD_count(ip + 8, matchl0 + 8, iend) + 8;
            offset = (U32)(ip - matchl0);
            ipfwd = ip;
            while (((ip > anchor) & (matchl0 > prefixLowest)) && (ip[-1] == matchl0[-1])) { ip--; matchl0--; mLength++; } /* catch up */

            if (is_totalbits_limited_seq_possible(ip, anchor, mLength, offset)) 
            {
                goto _match_found;
            }
            else 
            {
                ip = ipfwd; //revert
            }
        }
    }
    else if((ip - matchl0) > 0) 
    {
        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip, matchl0, iend);
        if (mLength) 
        {
            offset = (U32)(ip - matchl0);
            ipfwd = ip;
            while (((ip > anchor) & (matchl0 > prefixLowest)) && (ip[-1] == matchl0[-1])) { ip--; matchl0--; mLength++; } /* catch up */
            
            if (step < 4)
                hashLong[hl1] = (U32)(ip1 - base);
            ZSTD_storeSeq(seqStore, (size_t)(ip - anchor), anchor, iend, OFFSET_TO_OFFBASE(offset), mLength);
            goto _match_stored;
        }
    }
]])
set(AOCL_ZSTD_CDNGF_SHORT_MATCH [[
    /* check prefix short match */
    if (LIKELY(((ip - matchs0) >= WILDCOPY_VECLEN))) 
    {
        if ((MEM_read32(matchs0) == MEM_read32(ip))) 
        {
            PREFETCH_MATCH_S(ip + 4);
            goto _search_next_long;
        }
    }
    else if((ip - matchs0) > 0)
    {
        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip, matchs0, iend);
        if (mLength) 
        {
            offset = (U32)(ip - matchs0);
            ipfwd = ip;
            while (((ip > anchor) & (matchs0 > prefixLowest)) && (ip[-1] == matchs0[-1])) { ip--; matchs0--; mLength++; } /* catch up */
            
            if (step < 4)
                hashLong[hl1] = (U32)(ip1 - base);
            ZSTD_storeSeq(seqStore, (size_t)(ip - anchor), anchor, iend, OFFSET_TO_OFFBASE(offset), mLength);
            goto _match_stored;
        }
    }
]])
#set(AOCL_ZSTD_CDNGF_STORE_SEQ same as above)
set(AOCL_ZSTD_CDNGF_SAVE_REP [[
    (void)offsetSaved1;
    for (int i=0; i<ZSTD_REP_NUM; ++i) rep[i] = 0; /* reset as these have not been maintained */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_double_fast_noDict_generic_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_doubleFast_noDict_generic_fds2_analyze.h
)
# Function template instantiation for zstd_double_fast_noDict_generic_fds_aocl.h.in - end

# Function template instantiation for zstd_fast_noDict_generic_fds_aocl.h.in - start
set(AOCL_ZSTD_CFNGF_COMMENT [[
    /*
    * Derived from AOCL_ZSTD_compressBlock_fast_noDict_generic, but enforces :
    *   totalBits < (STREAM_ACCUMULATOR_MIN_64 - (LLFSELog + MLFSELog + OffFSELog)
    *   no Repeated_Offset2 and Repeated_Offset3
    *   no offsets go into extDict (no offset beyond prefix)
    *   offset >= WILDCOPY_VECLEN
    * Calls AOCL_ZSTD_storeSequences to store sequences
    */
]])
set(AOCL_ZSTD_CFNGF_FUNC "AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_base")
set(AOCL_ZSTD_CFNGF_REP_MATCH [[
    /* check repcode at ip[2] 
    * (rep_offset1 > 0) changed to (rep_offset1 >= WILDCOPY_VECLEN) as initial rep codes 
    * can be < WILDCOPY_VECLEN and offsets < WILDCOPY_VECLEN should not be included in the sequences
    */
    /* load repcode match for ip[2]*/
    const U32 rval = MEM_read32(ip2 - rep_offset1);
    if ((MEM_read32(ip2) == rval) & (rep_offset1 >= WILDCOPY_VECLEN)) 
    {
       ip0 = ip2;
       match0 = ip0 - rep_offset1;
       mLength = ip0[-1] == match0[-1];
       ip0 -= mLength;
       match0 -= mLength;
       offcode = REPCODE1_TO_OFFBASE;
       mLength += 4;

       /* First write next hash table entry; we've already calculated it.
       * This write is known to be safe because the ip1 is before the
       * repcode (ip2). */
       hashTable[hash1] = (U32)(ip1 - base);

       /* Count the forward length. */
       mLength += AOCL_ZSTD_count(ip0 + mLength, match0 + mLength, iend);
       if (is_totalbits_limited_seq_possible(ip0, anchor, mLength, rep_offset1))
           goto _store_sequences;
    }
]])
set(AOCL_ZSTD_CFNGF_MATCH [[
    if (rep_offset1 >= WILDCOPY_VECLEN) \
    { \
        /* first write next hash table entry; we've already calculated it */ \
        if (safe) \
        { \
            hashTable[hash1] = (U32)(ip1 - base); \
        } \
        else \
        { \
            if (step <= 4) \
            { \
                /* We need to avoid writing an index into the hash table >= ... */ \
                hashTable[hash1] = (U32)(ip1 - base); \
            } \
        } \
        /* Count the forward length. */ \
        mLength += AOCL_ZSTD_count(ip0 + mLength, match0 + mLength, iend); \
        if (is_totalbits_limited_seq_possible(ip0, anchor, mLength, rep_offset1)) \
            goto _store_sequences; \
        else \
            rep_offset1 = prev_rep_offset1; /* revert */ \
    } \
    else \
    { \
        rep_offset1 = prev_rep_offset1; /* revert */ \
    } \]])
set(AOCL_ZSTD_CFNGF_STORE_SEQ [[
    assert((OFFBASE_IS_OFFSET(offcode) && (OFFBASE_TO_OFFSET(offcode) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offcode)));
    AOCL_ZSTD_storeSequences(seqStore, ip0, anchor, iend, offcode, mLength);
]])
set(AOCL_ZSTD_CFNGF_SAVE_REP [[
    rep[0] = rep_offset1 ? rep_offset1 : offsetSaved1;
    for (int i=1; i<ZSTD_REP_NUM; ++i) rep[i] = 0; /* reset as these have not been maintained */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_fast_noDict_generic_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_fast_noDict_generic_fds2_base.h
)

set(AOCL_ZSTD_CFNGF_COMMENT [[
    /*
    * Derived from AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_base.
    * Additionally checks for long matches with offset < WILDCOPY_VECLEN. Such 
    * matches can be handled significantly faster during decompresion. Hence,
    * if encountered stop imposing constraints and mark state as FDS_NONE.
    */
]])
set(AOCL_ZSTD_CFNGF_FUNC "AOCL_ZSTD_compressBlock_fast_noDict_generic_fds2_analyze")
set(AOCL_ZSTD_CFNGF_REP_MATCH "")
set(AOCL_ZSTD_CFNGF_MATCH [[
    if (rep_offset1 >= WILDCOPY_VECLEN) \
    { \
        /* first write next hash table entry; we've already calculated it */ \
        if (safe) \
        { \
            hashTable[hash1] = (U32)(ip1 - base); \
        } \
        else \
        { \
            if (step <= 4) \
            { \
                /* We need to avoid writing an index into the hash table >= ... */ \
                hashTable[hash1] = (U32)(ip1 - base); \
            } \
        } \
        /* Count the forward length. */ \
        mLength += AOCL_ZSTD_count(ip0 + mLength, match0 + mLength, iend); \
        if (is_totalbits_limited_seq_possible(ip0, anchor, mLength, rep_offset1)) \
            goto _store_sequences; \
    } \
    else if (rep_offset1 > 0) /* rep_offset1 < WILDCOPY_VECLEN */ \
    { \
        /* first write next hash table entry; we've already calculated it */ \
        if (safe) \
        { \
            hashTable[hash1] = (U32)(ip1 - base); \
        } \
        else \
        { \
            if (step <= 4) \
            { \
                /* We need to avoid writing an index into the hash table >= ... */ \
                hashTable[hash1] = (U32)(ip1 - base); \
            } \
        } \
        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip0, match0, iend); \
        if (mLength) \
        { \
            ZSTD_storeSeq(seqStore, (size_t)(ip0 - anchor), anchor, iend, offcode, mLength); \
            goto _match_stored; \
        } \
    } \]])
#set(AOCL_ZSTD_CFNGF_STORE_SEQ same as above)
set(AOCL_ZSTD_CFNGF_SAVE_REP [[
    (void)offsetSaved1; (void)prev_rep_offset1;
    for (int i=0; i<ZSTD_REP_NUM; ++i) rep[i] = 0; /* reset as these have not been maintained */
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_fast_noDict_generic_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_fast_noDict_generic_fds2_analyze.h
)
