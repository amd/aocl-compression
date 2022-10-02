/**
 * Copyright (C) 2022, Advanced Micro Devices. All rights reserved.
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

#define MAX_FILENAME_LEN 128

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

//Data structure for storing levels and other parametric details of compressors
typedef struct
{
    const CHAR* codec_name;
    INTP lower_level;
    INTP upper_level;
    INTP extra_param;
    INTP max_block_size_param;
} codec_list_t;

//List of supported compression methods along with their parameters
//The list is ordered as per the enum aocl_compression_type
static const codec_list_t codec_list[AOCL_COMPRESSOR_ALGOS_NUM] =
{
    {"LZ4",    0, 0,  0, 0},
    {"LZ4HC",  1, 12, 0, 0},
    {"LZMA",   0, 9,  0, 0},
    {"BZIP2",  1, 9,  0, 0},
    {"SNAPPY", 0, 0,  0, 0},
    {"ZLIB",   1, 9,  0, 0},
    {"ZSTD",   1, 22, 0, 0}
};

//Main data structure for Test bench functionality
typedef struct
{
    CHAR *inPtr;            //buffer containing input data
    CHAR *compPtr;          //buffer containing compressed data
    CHAR *decompPtr;        //buffer containing decompressed data
    UINTP inSize;           //input data length
    UINTP outSize;          //output data length
    UINTP optVar;           //optional param used by compression method
    INTP use_all_codecs;		
    UINTP mem_limit;
    INTP codec_method;
    INTP codec_level;
    INTP iterations;
    INTP verify;
    INTP print_stats;
    INTP enable_verbosity;
    INTP optOff;
    UINTP file_size;
    INTP useIPP;
    FILE *fp;
    CHAR *fName;
    UINT64 cSize;           //size of compressed output
    UINT64 dSize;           //size of decompressed output
    UINT64 cTime;           //time to compress input
    UINT64 dTime;           //time to decompress input
    UINT64 cBestTime;       //best time to compress input
    UINT64 dBestTime;       //best time to decompress input
    FLOAT32 cSpeed;         //speed of compression
    FLOAT32 dSpeed;         //speed of decompression
    FLOAT32 cBestSpeed;     //best speed to compress
    FLOAT32 dBestSpeed;     //best speed to decompress
} aocl_codec_bench_info;

//Function declarations
INTP init(aocl_codec_bench_info *codec_bench_handle,
          aocl_compression_desc *aocl_codec_handle);
INTP read_user_options (INTP argc,
                       CHAR **argv,
                       aocl_codec_bench_info *codec_bench_handle);
INTP aocl_bench_run(aocl_compression_desc *aocl_codec_handle,
                    aocl_codec_bench_info *codec_bench_handle);
INTP ipp_bench_run(aocl_compression_desc *aocl_codec_handle,
                   aocl_codec_bench_info *codec_bench_handle);
VOID destroy(aocl_codec_bench_info *codec_bench_handle);

#endif
