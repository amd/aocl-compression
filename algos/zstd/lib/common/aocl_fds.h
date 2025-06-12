/**
 * Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
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

 /** @file aocl_fds.h
 *
 *  @brief AOCL Fast Decompress Settings
 * 
 * This file contains the definitions for AOCL fast decompress settings.
 *
 */

#ifndef AOCL_FDS_H
#define AOCL_FDS_H

#if AOCL_DECOMPRESS_FAST > 1

typedef struct {
    U64 state;       // AOCL fast decompress settings
    U32 ratio;       // Compression ratio of block compressor
    U16 count;       // Analysis counter
    U16 written;     // Is FDS frame written to stream?
    U64 single_pass; // Is input provided through a single-pass API?
    // APIs like : ZSTD_compress(), ZSTD_compress2(), ZSTD_compressCCtx(), ZSTD_compress_usingDict(), etc
} aocl_fds_t;

#define AOCL_HUF_TABLELOG_MIN 8
#define T_MIN 0.05          // Minimum thrshold for ratio loss
#define T_MAX 0.1375        // Maximum threshold for ratio loss
#define SLIDING_WINDOW 5
typedef struct {
    double avgTableLogReduction; // Store the average number of blocks in which HUF tableLog is reduced over a specified sliding window.
    void* ptrWorkspace;          // Pointer to AOCL_FDS_WORKSPACE_SIZE bytes of workspace
} aocl_entropy_fds_t;

typedef struct nodeElt_s {
    U32 count;
    U16 parent;
    BYTE byte;
    BYTE nbBits;
} nodeElt;

#endif /* AOCL_DECOMPRESS_FAST > 1 */

#endif // AOCL_FDS_H
