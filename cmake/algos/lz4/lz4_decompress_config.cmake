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

# @file lz4_decompress_config.cmake
# 
# @brief Function templates for aocl optimized lz4 decompress functions

# Function template instantiation for lz4_decompress_aocl.h.in - start
set(AOCL_LZ4_DECOMPRESS_GENERIC_COMMENT "")
set(AOCL_LZ4_DECOMPRESS_GENERIC_FUNC "AOCL_LZ4_decompress_generic")
set(AOCL_LZ4_DGV_ADDITIONAL_PARAMS "/* none */")
set(AOCL_LZ4_DGV_LONG_MATCH [[size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0);
                if (addl == rvl_error) {
                    LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional match length.");
                    DEBUGLOG(5, "error reading long match length");
                    goto _output_error;
                }
                length += addl;
]])
set(AOCL_LZ4_DGV_LAST_SEQ_CHECK [[if ((ip+length != iend) || (cpy > oend)) {
                        LOG_FORMATTED(ERR, logCtx, "Must be the last (or invalid) sequence because of the parsing limitations. Error, %s.",
                            (ip+length != iend) ? "exact input not consumed" : "output buffer overflow" );
]])

set(AOCL_LZ4_DGV_EOF_CHECK "!partialDecoding || (cpy == oend) || (ip >= (iend-2))")
set(AOCL_LZ4_DGV_COPY_MATCH [[size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0);
                if (addl == rvl_error) {
                    LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional match length.");
                    goto _output_error;
                }
                length += addl;
]])
set(AOCL_LZ4_DGV_SAFE_DECODE_EXIT "/* nothing */")

configure_file(
    ${ALGOS_PATH}/lz4/lz4_decompress_aocl.h.in
    ${ALGOS_PATH}/lz4/AOCL_LZ4_decompress_generic.h
)


set(AOCL_LZ4_DECOMPRESS_GENERIC_COMMENT "/* Same as AOCL_LZ4_decompress_generic, but with multi-threaded support */")
set(AOCL_LZ4_DECOMPRESS_GENERIC_FUNC "AOCL_LZ4_decompress_generic_mt")
set(AOCL_LZ4_DGV_ADDITIONAL_PARAMS ",int is_last_thread")
set(AOCL_LZ4_DGV_LONG_MATCH [[if (is_last_thread) {
                    size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0);
                    if (addl == rvl_error) { 
                        LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX"Encountered variable_length_error while decoding additional match length.", omp_get_thread_num());
                        DEBUGLOG(5, "error reading long match length");
                        goto _output_error; 
                    }
                    length += addl;
                } else {
                    size_t const addl = read_variable_length(&ip, iend + 1, 0);
                    if (addl == rvl_error) {
                        LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX"Encountered variable_length_error while decoding additional match length.", omp_get_thread_num());
                        DEBUGLOG(5, "error reading long match length");
                        goto _output_error; 
                    }
                    length += addl;
                }
]])
set(AOCL_LZ4_DGV_LAST_SEQ_CHECK [[if ((is_last_thread && (ip + length != iend)) || (cpy > oend)) {
                        LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX" Must be the last (or invalid) sequence because of the parsing limitations. Error, %s.",
                        omp_get_thread_num(), (is_last_thread && (ip + length != iend)) ? "exact input not consumed" : "output buffer overflow");
]])
set(AOCL_LZ4_DGV_EOF_CHECK "(is_last_thread && !partialDecoding) || (cpy == oend) || (is_last_thread && (ip >= (iend - 2)))")
set(AOCL_LZ4_DGV_COPY_MATCH [[if (is_last_thread) {
                    size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0);
                    if (addl == rvl_error) { goto _output_error; }
                    length += addl;
                }
                else {
                    size_t const addl = read_variable_length(&ip, iend + 1, 0);
                    if (addl == rvl_error) { goto _output_error; }
                    length += addl;
                }
]])
set(AOCL_LZ4_DGV_SAFE_DECODE_EXIT [[if ((cpy == oend) || (ip >= iend)) {
                break;
            }
]])

configure_file(
    ${ALGOS_PATH}/lz4/lz4_decompress_aocl.h.in
    ${ALGOS_PATH}/lz4/AOCL_LZ4_decompress_generic_mt.h
)
# Function template instantiation for lz4_decompress_aocl.h.in - end
