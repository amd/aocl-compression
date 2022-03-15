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
 
 /** @file api.h
 *  
 *  @brief Interface APIs and data structures of AOCL Codec library
 *
 *  This file contains the unified interface API set and associated
 *  data structure.
 *
 *  @author S. Biplab Raut
 */
 
#ifndef API_H
#define API_H

#ifdef __cplusplus
extern "C" {
#endif

#define LIBRARY_VERSION "AOCL-CODEC 3.2"
#define INTERNAL_LIBRARY_VERSION "AOCL LOSSLESS DATA CODEC 1.0"

//Types of compression methods supported
typedef enum
{
    LZ4 = 0,
    LZ4HC,
    LZMA,
    BZIP2,
    SNAPPY,
    ZLIB,
    ZSTD,
    AOCL_COMPRESSOR_ALGOS_NUM
} aocl_codec_type;

//Interface data structure
typedef struct
{
    char *inBuf;         //pointer to input buffer data
    char *outBuf;        //pointer to output buffer data
    char *workBuf;       //pointer to temporary work buffer
    size_t inSize;       //input data length
    size_t outSize;      //output data length
    size_t level;        //requested compression level
    size_t optVar;       //additional variables or parameters
    int numThreads;      //number of threads available for multi-threading
    int numMPIranks;     //number of available multi-core MPI ranks
    int memLimit;        //maximum memory limit for compression/decompression
    int measureStats;    //Measure speed and size of compression/decompression
    uint64_t cSize;      //size of compressed output
    uint64_t dSize;      //size of decompressed output
    uint64_t cTime;      //time to compress input
    uint64_t dTime;      //time to decompress input
    float cSpeed;        //speed of compression
    float dSpeed;        //speed of decompression
    int optOff;          //Turn off all optimizations
    int optLevel;        //Optimization level:0-NA,1-SSE2,2-AVX,3-AVX2,4-AVX512
    int printDebugLogs;  //print debug logs
    //size_t chunk_size; //Unused variable
} aocl_codec_desc;

//Interface API to compress data
int64_t aocl_codec_compress(aocl_codec_desc *handle,
                            aocl_codec_type codec_type);

//Interface API to decompress data
int64_t aocl_codec_decompress(aocl_codec_desc *handle,
                              aocl_codec_type codec_type);

//Interface API to setup the codec method
void aocl_codec_setup(aocl_codec_desc *handle,
                      aocl_codec_type codec_type);

//Interface API to destroy the codec method
void aocl_codec_destroy(aocl_codec_desc *handle,
                        aocl_codec_type codec_type);

//Interface API to get compression library version string
const char *aocl_codec_version(void);

#ifdef __cplusplus
}
#endif

#endif