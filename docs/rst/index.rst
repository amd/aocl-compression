..  Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.

..  Redistribution and use in source and binary forms, with or without
..  modification, are permitted provided that the following conditions are met:

..  1. Redistributions of source code must retain the above copyright notice,
..  this list of conditions and the following disclaimer.
..  2. Redistributions in binary form must reproduce the above copyright notice,
..  this list of conditions and the following disclaimer in the documentation
..  and/or other materials provided with the distribution.
..  3. Neither the name of the copyright holder nor the names of its
..  contributors may be used to endorse or promote products derived from this
..  software without specific prior written permission.

..  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
..  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
..  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
..  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
..  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
..  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
..  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
..  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
..  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
..  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
..  POSSIBILITY OF SUCH DAMAGE.

================
AOCL-Compression
================


Introduction
`````````````
AOCL-Compression is a software framework of various lossless compression and
decompression methods tuned and optimized for AMD Zen™ based CPUs.
This framework offers a single set of unified APIs for all the supported
compression and decompression methods which facilitate the applications to
easily integrate and use them. For AOCL-Compression framework APIs, refer to :ref:`ref-unified-api`.


Methods Supported
..................

AOCL-Compression supports compression and decompression methods of the following libraries.

.. toctree::
   :maxdepth: 2

   INTRODUCTION

It supports the dynamic dispatcher feature that executes the most optimal
function variant implemented using Function Multi-versioning thereby offering
a single optimized library portable across different x86 CPU architectures.
AOCL-Compression framework is developed in C for UNIX® and Windows® based systems.
A test suite is provided for the validation and performance benchmarking
of the supported compression and decompression methods. This suite also
supports the benchmarking of IPP compression methods, such as, lz4, lz4hc, zlib and bzip2.
The library build framework offers CTest-based testing of the test cases
implemented using GTest and the library test suite. Starting from AOCL-Compression 4.2,
the library offers openMP based multi-threaded compression for a few methods.


Unified APIs
````````````
Unified API abstracts individual compression and decompression routines into a single, standardized interface that uses a common handle and shared data structures, simplifying integration and maintenance compared to managing separate APIs for each method.

.. toctree::
   :maxdepth: 2

   unified_api

Native APIs
````````````
In-depth overview of native APIs provided by compression methods included in AOCL-Compression are described here.

.. toctree::
   :maxdepth: 1

   lz4
   zlib
   lzma
   zstd
   bzip2
   snappy
   lz4hc

Multi-threaded APIs
````````````````````
These APIs are designed to take advantage of multi-core CPUs to improve the performance of compression and decompression. The library needs to be built with `AOCL_ENABLE_THREADS=ON` for multi-core processing to be enabled.


LZ4
...

- :ref:`int LZ4_compress_fast(const char *src, char *dst, int srcSize, int dstCapacity, int acceleration) <LZ4_compress_fast>`
- :ref:`int LZ4_compress_default(const char *src, char *dst, int srcSize, int dstCapacity) <LZ4_compress_default>`
- :ref:`int LZ4_decompress_safe(const char *src, char *dst, int compressedSize, int dstCapacity) <LZ4_decompress_safe>`


ZLIB
....

- :ref:`int compress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen) <compress>`
- :ref:`int compress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level) <compress2>`
- :ref:`int compress2_raw(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level) <compress2_raw>`
- :ref:`int compress2_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level) <compress2_gzip>`
- :ref:`int uncompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen) <uncompress>`
- :ref:`int uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen) <uncompress2>`
- :ref:`int uncompress2_raw(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen) <uncompress2_raw>`
- :ref:`int uncompress2_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen) <uncompress2_gzip>`


ZSTD
....

AOCL Optimized multi-threaded APIs
**********************************

- :ref:`size_t ZSTD_compress_advanced(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize, const void* dict,size_t dictSize, ZSTD_parameters params) <ZSTD_compress_advanced>`
- :ref:`size_t ZSTD_decompress(void* dst, size_t dstCapacity, const void* src, size_t compressedSize) <ZSTD_decompress>`
- :ref:`size_t ZSTD_decompressDCtx(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize) <ZSTD_decompressDCtx>`
 
Native multi-threaded APIs
**************************
The library needs to be built with `NATIVE_ENABLE_THREADS=ON` & `AOCL_ENABLE_THREADS=OFF` for multi-core processing to be enabled for these APIs.

- :ref:`size_t ZSTD_compress2( ZSTD_CCtx* cctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize) <ZSTD_compress2>`
- :ref:`size_t ZSTD_compressStream2(ZSTD_CCtx* cctx, ZSTD_outBuffer* output, ZSTD_inBuffer* input, ZSTD_EndDirective endOp) <ZSTD_compressStream2>`
- :ref:`size_t ZSTD_compressStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output, ZSTD_inBuffer* input) <ZSTD_compressStream>`
- :ref:`size_t ZSTD_flushStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output) <ZSTD_flushStream>`
- :ref:`size_t ZSTD_endStream(ZSTD_CStream* zcs, ZSTD_outBuffer* output) <ZSTD_endStream>`


SNAPPY
......

- :ref:`void RawCompress(const char* input, size_t input_length, char* compressed, size_t* compressed_length, CompressionOptions options) <RawCompress_with_options>`
- :ref:`void RawCompress(const char* input, size_t input_length, char* compressed, size_t* compressed_length) <RawCompress>`
- :ref:`bool RawUncompress(const char* compressed, size_t compressed_length, char* uncompressed) <RawUncompress>`


LZ4HC
.....

- :ref:`int LZ4_compress_HC(const char *src, char *dst, int srcSize, int dstCapacity, int compressionLevel) <LZ4_compress_HC>`

Examples
``````````
Example programs illustrating how to use AOCL-Compression APIs are presented here.

.. toctree::
   :maxdepth: 2

   EXAMPLE_README

**Contacts**

AOCL-Compression is developed and maintained by AMD.
For support, send an email to toolchainsupport@amd.com.
