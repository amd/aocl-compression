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
 *  @brief Interface APIs and data structures of AOCL Compression library
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
/**
 * \defgroup API Standardized/Unified API
 * \defgroup LZ4_API
 * \defgroup SNAPPY_API
 * \defgroup ZLIB_API
 * \defgroup BZIP2_API
 * \defgroup LZMA_API
 * \defgroup LZ4HC_API
 */

/**
 * \addtogroup API
 *  @brief Interface APIs and data structures of AOCL Compression library
 *
 *  This file contains the unified interface API set and associated data structure.
 * @{
 */

#ifdef _WINDOWS
/**
 * You can export data, functions, classes, or class member functions from a DLL
 * using the __declspec(dllexport) keyword. __declspec(dllexport) adds the export
 * directive to the object file so you do not need to use a .def file.
 * 
 */
#define EXPORT_SYM_DYN __declspec(dllexport)
#else
/**
 * For Linux EXPORT_SYM_DYN is NULL, by default the symbols are publicly exposed.
 */
#define EXPORT_SYM_DYN
#endif

#define LIBRARY_VERSION "AOCL-COMPRESSION 4.0.1"
#define INTERNAL_LIBRARY_VERSION "AOCL LOSSLESS DATA COMPRESSION 1.0"


 /**
  * @brief Error codes supported in aocl-compression.
  *
  * 
  */
typedef enum
{
    ERR_UNSUPPORTED_METHOD = -3,     ///<compression method not supported by the library
    ERR_EXCLUDED_METHOD,             ///<compression method excluded from this library build
    ERR_COMPRESSION_FAILED,          ///<failure in compression/decompression
    ERR_COMPRESSION_INVALID_OUTPUT   ///<invalid compression/decompression output
} aocl_error_type;

/**
 * @brief Types of compression methods supported.
 * 
 * Optimizations are included for all the supported methods
 */
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
} aocl_compression_type;

/**
 * @brief This acts as a handle for compression and decompression of aocl library.
 * 
 */
typedef struct
{
    char *inBuf;         ///<pointer to input buffer data
    char *outBuf;        ///<pointer to output buffer data
    char *workBuf;       ///<pointer to temporary work buffer
    size_t inSize;       ///<input data length
    size_t outSize;      ///<output data length
    size_t level;        ///<requested compression level
    size_t optVar;       ///<additional variables or parameters
    int numThreads;      ///<number of threads available for multi-threading
    int numMPIranks;     ///<number of available multi-core MPI ranks
    size_t memLimit;     ///<maximum memory limit for compression/decompression
    int measureStats;    ///<Measure speed and size of compression/decompression
    uint64_t cSize;      ///<size of compressed output
    uint64_t dSize;      ///<size of decompressed output
    uint64_t cTime;      ///<time to compress input
    uint64_t dTime;      ///<time to decompress input
    float cSpeed;        ///<speed of compression
    float dSpeed;        ///<speed of decompression
    int optOff;          ///<Turn off all optimizations
    int optLevel;        ///<Optimization level:0-NA,1-SSE2,2-AVX,3-AVX2,4-AVX512
    int printDebugLogs;  ///<print debug logs
    //size_t chunk_size; //Unused variable
} aocl_compression_desc;

/**
 * @brief Interface API to compress data.
 * 
 * @param handle This acts as a handle for compression and decompression. Refer to aocl_compression_desc for more info.
 * @param codec_type Select the algorithm you want to use for compression, choose from aocl_compression_type.
 * 
 * @return Numbers of bytes compressed on success. Error codes ERR_COMPRESSION_FAILED or ERR_COMPRESSION_INVALID_OUTPUT on failure.
 */
EXPORT_SYM_DYN int64_t aocl_llc_compress(aocl_compression_desc *handle,
                            aocl_compression_type codec_type);

/**
 * @brief Interface API to decompress data.
 * 
 * @param handle This acts as a handle for compression and decompression. Refer to aocl_compression_desc for more info.
 * @param codec_type Select the algorithm you want to use for compression, choose from aocl_compression_type.
 * 
 * @return Numbers of bytes decompressed on success. Error codes ERR_COMPRESSION_FAILED or ERR_COMPRESSION_INVALID_OUTPUT on failure.
 */

EXPORT_SYM_DYN int64_t aocl_llc_decompress(aocl_compression_desc *handle,
                              aocl_compression_type codec_type);

/**
 * @brief Interface API to setup the codec method.
 * 
 * @param handle This acts as a handle for compression and decompression. Refer to aocl_compression_desc for more info.
 * @param codec_type Select the algorithm you want to use for compression, choose from aocl_compression_type.
 * 
 * @return 0 on success. Error codes ERR_UNSUPPORTED_METHOD or ERR_EXCLUDED_METHOD on failure.
 */

EXPORT_SYM_DYN int32_t aocl_llc_setup(aocl_compression_desc *handle,
                      aocl_compression_type codec_type);

/**
 * @brief Interface API to destroy the codec method.
 * 
 * @param handle This acts as a handle for compression and decompression. Refer to aocl_compression_desc for more info.
 * @param codec_type Select the algorithm you want to use for compression, choose from aocl_compression_type.
 * 
 * @return void 
 */
EXPORT_SYM_DYN void aocl_llc_destroy(aocl_compression_desc *handle,
                        aocl_compression_type codec_type);
/**
 * @brief Interface API to get compression library version string.
 * 
 * @return aocl library version 
 */

EXPORT_SYM_DYN const char *aocl_llc_version(void);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif
