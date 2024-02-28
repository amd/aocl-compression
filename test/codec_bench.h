/**
 * Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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
 
 /** @file codec_bench.h
 *  
 *  @brief Function prototypes and data structures of Test bench application.
 *
 *  This file contains the function prototypes and data structures for
 *  testing the compression and decompression methods.
 *
 *  @author S. Biplab Raut
 */

#ifndef CODEC_BENCH_H
#define CODEC_BENCH_H

#ifdef _WINDOWS
#include <windows.h>
#define strtok_r strtok_s
#define strcasecmp _stricmp
#endif

#include "utils/utils.h"

#define MAX_FILENAME_LEN 256

//Number of iterations of compression/decompression operations
#define BENCH_NUM_ITERS 10

//Size of input data to be processed in memory at a time
//Few compression methods are limited by datatype upper bound (2^31 -1)
//Output buffer needs extra space to accomodate the additional header bytes
#define MAX_MEM_SIZE_FOR_FILE_READ 1073741824 //(1024 MBs)

//Uninitialized level value
#define UNINIT_LEVEL 999

//Uninitialized optional param value
#define UNINIT_OPT_VAR 999

//Minimum compressed buffer size
#define MIN_PAD_SIZE (16*1024)

#define RUN_OPERATION_DEFAULT 0     // run compress and decompress
#define RUN_OPERATION_COMPRESS 1    // run compress only
#define RUN_OPERATION_DECOMPRESS 2  // run decompress only

//Error codes
#define ERR_CODEC_BENCH_ARGS -1
#define ERR_CODEC_BENCH_METHOD -2
#define ERR_CODEC_BENCH_FILE_IO -3
#define ERR_CODEC_BENCH_MEM -4

//Data structure for storing levels and other parametric details of compressors
typedef struct
{
    const AOCL_CHAR* codec_name;
    AOCL_INTP lower_level;
    AOCL_INTP upper_level;
    AOCL_INTP extra_param;
    AOCL_INTP max_block_size_param;
    const AOCL_CHAR* extension;
} codec_list_t;

//List of supported compression methods along with their parameters
//The list is ordered as per the enum aocl_compression_type
static const codec_list_t codec_list[AOCL_COMPRESSOR_ALGOS_NUM] =
{
    {"LZ4",    0, 0,  0, 0, ".lz4"},
    {"LZ4HC",  1, 12, 0, 0, ".lz4"},
    {"LZMA",   0, 9,  0, 0, ".lzma"},
    {"BZIP2",  1, 9,  0, 0, ".bz2"},
    {"SNAPPY", 0, 0,  0, 0, ".snappy"},
    {"ZLIB",   1, 9,  0, 0, ".zlib"},
    {"ZSTD",   1, 22, 0, 0, ".zst"}
};

//Main data structure for Test bench functionality
typedef struct
{
    AOCL_CHAR *inPtr;            //buffer containing input data
    AOCL_CHAR *outPtr;           //buffer containing output data
    AOCL_CHAR *decompPtr;        //buffer containing decompressed data for validation
    AOCL_UINTP inSize;           //input data length
    AOCL_UINTP outSize;          //output data length
    AOCL_UINTP optVar;           //optional param used by compression method
    AOCL_INTP use_all_codecs;		
    AOCL_UINTP mem_limit;
    AOCL_INTP codec_method;
    AOCL_INTP codec_level;
    AOCL_INTP iterations;
    AOCL_INTP verify;
    AOCL_INTP print_stats;
    AOCL_INTP optOff;
    AOCL_UINTP file_size;
    AOCL_INTP useIPP;
    AOCL_CHAR *ippDir;
    AOCL_INTP useNAPI;
    FILE *fp;
    AOCL_CHAR *fName;
    AOCL_UINT64 cSize;           //size of compressed output
    AOCL_UINT64 dSize;           //size of decompressed output
    AOCL_UINT64 cTime;           //time to compress input
    AOCL_UINT64 dTime;           //time to decompress input
    AOCL_UINT64 cBestTime;       //best time to compress input
    AOCL_UINT64 dBestTime;       //best time to decompress input
    AOCL_FLOAT32 cSpeed;         //speed of compression
    AOCL_FLOAT32 dSpeed;         //speed of decompression
    AOCL_FLOAT32 cBestSpeed;     //best speed to compress
    AOCL_FLOAT32 dBestSpeed;     //best speed to decompress
    AOCL_CHAR *dumpFile;
    AOCL_INTP cfile_count;
    FILE* dumpFp;           //optional file for saving output data
    FILE* valFp;            //optional file for loading validation data in decompress only mode
    AOCL_INTP runOperation;      //operation to run: compress, decompress, both (default)
} aocl_codec_bench_info;

#ifdef AOCL_ENABLE_LOG_FEATURE
static aocl_log_ctx log_ctx = {-1};
#endif /* AOCL_ENABLE_LOG_FEATURE */

//Function declarations
AOCL_INTP init(aocl_codec_bench_info *codec_bench_handle,
          aocl_compression_desc *aocl_codec_handle);
AOCL_INTP read_user_options (AOCL_INTP argc,
                       AOCL_CHAR **argv,
                       aocl_codec_bench_info *codec_bench_handle);
AOCL_INTP aocl_bench_run(aocl_compression_desc *aocl_codec_handle,
                    aocl_codec_bench_info *codec_bench_handle);
AOCL_INTP ipp_bench_run(aocl_compression_desc *aocl_codec_handle,
                    aocl_codec_bench_info *codec_bench_handle);
AOCL_INTP native_api_bench_run(aocl_compression_desc *aocl_codec_handle, 
                    aocl_codec_bench_info *codec_bench_handle);
AOCL_VOID destroy(aocl_codec_bench_info *codec_bench_handle);

#endif
