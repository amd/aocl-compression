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

# Function template instantiation for zstd_lazy_aocl.h.in - start
set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds2_base")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "WILDCOPY_VECLEN")
set(AOCL_ZSTD_CLNF_STORE_SEQ [[
    assert((OFFBASE_IS_OFFSET(offBase) && (OFFBASE_TO_OFFSET(offBase) >= WILDCOPY_VECLEN)) || (!OFFBASE_IS_OFFSET(offBase)));
    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength);
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds2_base.h
)

set(AOCL_ZSTD_CLNF_FUNC "AOCL_ZSTD_compressBlock_lazy_fds2_offset8")
set(AOCL_ZSTD_CLNF_MIN_OFFSET "(WILDCOPY_VECLEN/2)")
set(AOCL_ZSTD_CLNF_STORE_SEQ [[
    assert((OFFBASE_IS_OFFSET(offBase) && (OFFBASE_TO_OFFSET(offBase) >= (WILDCOPY_VECLEN/2))) || (!OFFBASE_IS_OFFSET(offBase)));
    AOCL_ZSTD_storeSequences(seqStore, start, anchor, iend, offBase, matchLength);
]])
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_lazy_fds2_offset8.h
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
set(AOCL_ZSTD_CDNGF_REP_MATCH [[
    if (LIKELY(offset_1 >= WILDCOPY_VECLEN)) 
    {
        if ((MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1))) 
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
    }
    else if (offset_1 > 0)
    { // offset_1 < WILDCOPY_VECLEN
        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip + 1, ip + 1 - offset_1, iend);
        if (mLength) 
        {
            ip++;
            ZSTD_storeSeq(seqStore, (size_t)(ip - anchor), anchor, iend, REPCODE1_TO_OFFBASE, mLength);
            goto _match_stored;
        }
    }
]])
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
            
            offset_1 = offset;
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
            
            offset_1 = offset;
            if (step < 4)
                hashLong[hl1] = (U32)(ip1 - base);
            ZSTD_storeSeq(seqStore, (size_t)(ip - anchor), anchor, iend, OFFSET_TO_OFFBASE(offset), mLength);
            goto _match_stored;
        }
    }
]])
#set(AOCL_ZSTD_CDNGF_STORE_SEQ same as above)
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
set(AOCL_ZSTD_CFNGF_REP_MATCH [[
    if (LIKELY(rep_offset1 >= WILDCOPY_VECLEN)) 
    {
        if (MEM_read32(ip2) == rval)
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
    }
    else if (rep_offset1 > 0) // rep_offset1 < WILDCOPY_VECLEN
    {
        mLength = AOCL_ZSTD_isLongMatch(seqStore, ip2, ip2 - rep_offset1, iend);
        if (mLength)
        {
            ip0 = ip2;
            match0 = ip0 - rep_offset1;
            offcode = REPCODE1_TO_OFFBASE;

            /* First write next hash table entry; we've already calculated it.
            * This write is known to be safe because the ip1 is before the
            * repcode (ip2). */
            hashTable[hash1] = (U32)(ip1 - base);

            ZSTD_storeSeq(seqStore, (size_t)(ip0 - anchor), anchor, iend, offcode, mLength);
            goto _match_stored;
        }
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
        else \
            rep_offset1 = prev_rep_offset1; /* revert */ \
    } \
    else \
    { \
        rep_offset1 = prev_rep_offset1; /* revert */ \
    } \]])
#set(AOCL_ZSTD_CFNGF_STORE_SEQ same as above)
configure_file(
    ${ALGOS_PATH}/zstd/lib/compress/zstd_fast_noDict_generic_fds_aocl.h.in
    ${ALGOS_PATH}/zstd/lib/compress/aocl_zstd_compressBlock_fast_noDict_generic_fds2_analyze.h
)
