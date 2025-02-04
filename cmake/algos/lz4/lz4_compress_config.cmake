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

# @file lz4_compress_config.cmake
# 
# @brief Function templates for aocl optimized lz4 compress functions

# Function template instantiation for lz4_compress_aocl.h.in - start
set(AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_COMMENT "")
set(AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_FUNC "AOCL_LZ4_compress_generic_validated")
set(AOCL_LZ4_CGV_ADDITIONAL_PARAMS "/* none */")
set(AOCL_LZ4_CGV_INIT_START_BYTE [[/* First Byte */
    AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);
]])
set(AOCL_LZ4_CGV_INIT_IPDATA "U32 ipData;")
set(AOCL_LZ4_CGV_INIT_MATCHDATA "auto U32 matchData;")
set(AOCL_LZ4_CGV_SET_IPDATA "ipData=*(U32*)ip;")
set(AOCL_LZ4_CGV_SET_MATCHDATA "matchData=*(U32*)match;")
set(AOCL_LZ4_CGV_CMP_IP_MATCH_DATA "matchData == ipData")
set(AOCL_LZ4_CGV_CLEAR_HASH "LZ4_clearHash")
set(AOCL_LZ4_CGV_CMP_IP_MATCH "if (LZ4_read32(match) == LZ4_read32(ip))")
set(AOCL_LZ4_CGV_LAST_LITERALS "/* do nothing */")
set(AOCL_LZ4_CGV_SET_RETURN_VALUES [[result = (int)(((char*)op) - dest);
    assert(result > 0);
]])

configure_file(
    ${ALGOS_PATH}/lz4/lz4_compress_aocl.h.in
    ${ALGOS_PATH}/lz4/AOCL_LZ4_compress_generic_validated.h
)


set(AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_COMMENT [[/**
* For cases when dict_directive == noDict, to avoid out-of-bound memory access
* on reading 2 bytes in backward direction (when `match` pointer points to start
* of the source buffer), initialize hashTable entries with 2. This ensures that
* `match` pointer never points to 0th index of the source buffer.
*/
]])
set(AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_FUNC "AOCL_LZ4_compress_generic_validated_FDS_noDict")
set(AOCL_LZ4_CGV_ADDITIONAL_PARAMS "/* none */")
set(AOCL_LZ4_CGV_INIT_START_BYTE [[assert(dictDirective == noDict);
    if (tableType == byU32) AOCL_INITIALIZE_HASHTABLE_FOR_NODICT(cctx, U32, (1U << (LZ4_MEMORY_USAGE - 2)))
    if (tableType == byU16) AOCL_INITIALIZE_HASHTABLE_FOR_NODICT(cctx, U16, (1U << (LZ4_MEMORY_USAGE - 1)))
    /* Skip first two bytes */
    ip += 2;
    /* Third Byte */
    AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);
]])
set(AOCL_LZ4_CGV_INIT_IPDATA "U64 ipDataFwd, ipDataBck;") 
set(AOCL_LZ4_CGV_INIT_MATCHDATA "auto U64 matchDataFwd, matchDataBck;")
set(AOCL_LZ4_CGV_SET_IPDATA "AOCL_READ_FWD_AND_BACK_BYTES(ip, ipDataFwd, ipDataBck)")
set(AOCL_LZ4_CGV_SET_MATCHDATA "AOCL_READ_FWD_AND_BACK_BYTES(match, matchDataFwd, matchDataBck)")
set(AOCL_LZ4_CGV_CMP_IP_MATCH_DATA "matchDataFwd == ipDataFwd || matchDataBck == ipDataBck")
set(AOCL_LZ4_CGV_CLEAR_HASH "AOCL_LZ4_clearHash")
set(AOCL_LZ4_CGV_CMP_IP_MATCH [[U64 ipDataFwd, ipDataBck;
                auto U64 matchDataFwd, matchDataBck;
                AOCL_READ_FWD_AND_BACK_BYTES(ip, ipDataFwd, ipDataBck)
                AOCL_READ_FWD_AND_BACK_BYTES(match, matchDataFwd, matchDataBck)
                if (matchDataFwd == ipDataFwd || matchDataBck == ipDataBck)
]])
set(AOCL_LZ4_CGV_LAST_LITERALS "/* do nothing */")
set(AOCL_LZ4_CGV_SET_RETURN_VALUES [[result = (int)(((char*)op) - dest);
    assert(result > 0);
]])

configure_file(
    ${ALGOS_PATH}/lz4/lz4_compress_aocl.h.in
    ${ALGOS_PATH}/lz4/AOCL_LZ4_compress_generic_validated_FDS_noDict.h
)


set(AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_COMMENT [[/**
* Same as AOCL_LZ4_compress_generic_validated, but with state information
* for Multi - threaded support 
*/
]])
set(AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_FUNC "AOCL_LZ4_compress_generic_validated_mt")
set(AOCL_LZ4_CGV_ADDITIONAL_PARAMS [[,unsigned char** last_anchor_ptr
                 ,unsigned int* last_bytes_len
]])
set(AOCL_LZ4_CGV_INIT_START_BYTE [[BYTE* dst_without_lastLiterals;
    /* First Byte */
    AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
    ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);
]])
set(AOCL_LZ4_CGV_INIT_IPDATA "U32 ipData;") 
set(AOCL_LZ4_CGV_INIT_MATCHDATA "auto U32 matchData;")
set(AOCL_LZ4_CGV_SET_IPDATA "ipData=*(U32*)ip;")
set(AOCL_LZ4_CGV_SET_MATCHDATA "matchData=*(U32*)match;")
set(AOCL_LZ4_CGV_CMP_IP_MATCH_DATA "matchData == ipData")
set(AOCL_LZ4_CGV_CLEAR_HASH "LZ4_clearHash")
set(AOCL_LZ4_CGV_CMP_IP_MATCH "if (LZ4_read32(match) == LZ4_read32(ip))")
set(AOCL_LZ4_CGV_LAST_LITERALS "dst_without_lastLiterals = op;")
set(AOCL_LZ4_CGV_SET_RETURN_VALUES [[if (last_bytes_len != NULL)
    {
        result = (int)(((char*)dst_without_lastLiterals) - dest);
        *last_anchor_ptr = (BYTE*)anchor; /* src pointer until which compressed output is written : To support ST decompression on parallel compressed stream */
        *last_bytes_len = (size_t)(iend - anchor);/* length of src bytes pending for compression : To support ST decompression on parallel compressed stream */
        LOG_FORMATTED(INFO, logCtx, "Thread [id: %d] : result=%i, last_bytes_len=%i", omp_get_thread_num(), result, (int)(*last_bytes_len));
    }
    else
    {
        result = (int)(((char*)op) - dest);
        *last_anchor_ptr = (BYTE*)op; /* Write the complete commpressed chunk */
        /* *last_bytes_len = 0;//Last thread needs no joining with the next chunk */
        LOG_FORMATTED(INFO, logCtx, "Thread [id: %d] : result=%i", omp_get_thread_num(), result);
    }
    assert(result >= 0); /* result=0 when no match found, (all literals). */
]])

configure_file(
    ${ALGOS_PATH}/lz4/lz4_compress_aocl.h.in
    ${ALGOS_PATH}/lz4/AOCL_LZ4_compress_generic_validated_mt.h
)
# Function template instantiation for lz4_compress_aocl.h.in - end
