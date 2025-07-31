# Copyright (C) 2022-2025, Advanced Micro Devices. All rights reserved.
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
 
# @file CMakeLists.txt
# 
# @brief CMake based build system
#
# Compiles and installs the library and test bench binary.
# Supports uninstall custom command to remove the installed files.
#
# @author S. Biplab Raut


cmake_minimum_required(VERSION 3.26.0)

#set the project name
project(AOCL-COMPRESSION-LIBRARY CXX C ASM)
set(CMAKE_C_STANDARD 11) # support atomic operations
message(STATUS "Building for AOCL COMPRESSION LIBRARY")

string(TIMESTAMP TODAY "%Y%m%d")
add_compile_definitions(AOCL_BUILD_VERSION="Build ${TODAY}")

message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER_ID}")
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    if (${CMAKE_C_COMPILER_VERSION} VERSION_LESS 8.5)
        message(FATAL_ERROR "Minimum GCC version supported is 8.5!")
    endif ()
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    if (${CMAKE_C_COMPILER_VERSION} VERSION_LESS 11.0)
        message(FATAL_ERROR "Minimum Clang version supported is 11.0! In case of AOCC, use version 2.3 and above!")
endif ()
else ()
    message(FATAL_ERROR "Compiler not supported! Library has only been tested with Clang and GCC.")
endif ()

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
option (BUILD_STATIC_LIBS "Build static library (Default build type is shared library)" OFF)
option (AOCL_LZ4_OPT_PREFETCH_BACKWARDS "Enable LZ4 optimizations related to backward prefetching of data" OFF)
option (SNAPPY_MATCH_SKIP_OPT "Enable Snappy match skipping optimization (Enabled by default)" ON)
option (SNAPPY_HIGH_COMPRESSION "Enable Snappy high compression to get better ratio by compromising on speed (Disabled by default)" OFF)
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    option (SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS "Enable Snappy branchless decompression optimization (Disabled by default for GCC)" OFF)
else()
    option (SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS "Enable Snappy branchless decompression optimization (Enabled by default for non-GCC compilers)" ON)
endif()
option (LZ4_FRAME_FORMAT_SUPPORT "Enable building LZ4 with Frame format and API support (Enabled by default)" ON)
option (AOCL_ZSTD_SEARCH_SKIP_OPT "Enable ZSTD match skipping optimization that steps more aggresively when matches are not found (Enabled by default)" ON)
option (AOCL_ZSTD_DYN_BLOCK_SIZE "Enable ZSTD dynamic block size determination  (Disabled by default)" OFF)
option (AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS "Disable Pattern Analysis in LZ4HC for level 9 (Enabled by default)" ON)
option (AOCL_TEST_COVERAGE "Enable GTest, AOCL test bench and third party test bench based CTest suite (Disabled by default)" OFF)
if(UNIX)
    option (CODE_COVERAGE "Enable source code coverage (only on Linux with GCC compiler)" OFF)
    option (ASAN "Enable Address Sanitizer checks (only on Linux/Debug build)" OFF)
    option (VALGRIND "Enable Valgrind checks (only on Linux/Debug. Incompatible with ASAN=ON)" OFF)
endif()
option (BUILD_DOC "Build documentation for aocl-compression (Disabled by default)" OFF)  
option (BUILD_EXAMPLE "Build examples for aocl-compression (Disabled by default)" OFF)
option (AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1 "Enable LZ4 match skipping optimization strategy-1 based on a larger base step size (Disabled by default)" OFF)
option (AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2 "Enable LZ4 match skipping optimization strategy-2 by aggressively setting search distance on top of strategy-1 (Disabled by default)" OFF)
option (AOCL_LZ4_NEW_PRIME_NUMBER "Enable the usage of a new prime number for LZ4 hashing function (Disabled by default)" OFF)
option (AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES "Enable storing of additional potential matches to improve compression ratio. Recommended for higher compressibility use cases (Disabled by default)" OFF)
option (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL "Build with xz utils lzma APIs. Experimental feature with limited API support (Disabled by default)" OFF)
option (AOCL_ENABLE_LOG_FEATURE "Enable logging support in library, for which log level is determined by environement variable AOCL_ENABLE_LOG (go through README.md) (Disabled by default)" OFF)
option (AOCL_ENABLE_THREADS "Enable multi-threaded compression and decompression using SMP openMP threads (Disabled by default)" OFF)
option (TEST_COVERAGE_THIRD_PARTY "Enable third party test bench based CTest suite (Disabled by default)" OFF)
option (BUILD_UTILITY "Enable third party utility build: zstd (Disabled by default)" OFF)
option (NATIVE_ENABLE_THREADS "Enable native multi-threaded compression for supported methods (Disabled by default)" OFF)
option (AOCL_TEST_FUZZER "Enable AOCL Compression fuzzer (only on Linux with Clang compiler)" OFF)
option (AOCL_TEST_FUZZER_WITH_CORPUS "Run fuzzer tests with corpus (only on Linux with Clang compiler)" OFF)
option (ENABLE_FAST_MATH "Enable fast-math optimizations (Disabled by default)" OFF)

option (AOCL_EXCLUDE_BZIP2 "Exclude BZIP2 compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_LZ4 "Exclude LZ4 compression method from the library build. LZ4HC also gets excluded (Disabled by default)" OFF)
option (AOCL_EXCLUDE_LZ4HC "Exclude LZ4HC compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_LZMA "Exclude LZMA compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_SNAPPY "Exclude SNAPPY compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_ZLIB "Exclude ZLIB compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_ZSTD "Exclude ZSTD compression method from the library build (Disabled by default)" OFF)

set(AOCL_LZ4_HASH_BITS_USED "LOW" CACHE STRING "Control the number of bits used for LZ4 hashing, allowed values are OFF, LOW (low perf gain and less CR regression) and HIGH (high perf gain and high CR regression) (LOW by default)")
set_property(
    CACHE AOCL_LZ4_HASH_BITS_USED
    PROPERTY STRINGS
    "OFF" "LOW" "HIGH"
)

set(AOCL_BZIP2_HUFFMAN_ITERATIONS "3" CACHE STRING "In BZIP2, huffman tables are fine-tuned by iterating through each block of data multiple times, AOCL_BZIP2_HUFFMAN_ITERATIONS controls the number of iterations (allowed values are 1, 2, 3 and 4), lower iterations enhance speed but worsen the compression ratio (3 by default).")
set_property(
    CACHE AOCL_BZIP2_HUFFMAN_ITERATIONS
    PROPERTY STRINGS
    "1" "2" "3" "4"
)

# Faster decompress settings for supported methods
set(AOCL_DECOMPRESS_FAST "OFF" CACHE STRING "Enable fast decompression modes that might compromise on compression speed / ratio to produce streams that decompress faster. Supported values: {1,2,3} ZSTD, {1,2} Snappy, {1} LZ4." )
set_property(
    CACHE AOCL_DECOMPRESS_FAST
    PROPERTY STRINGS
    "OFF" "1" "2" "3" # Higher values generally leads to faster decompression with larger compromise on compression speed/ratio
)
if (NOT AOCL_DECOMPRESS_FAST STREQUAL "OFF" AND (AOCL_ENABLE_THREADS OR NATIVE_ENABLE_THREADS))
    message(WARNING "Fast decompression mode excluded as it is supported in single threaded mode only")
    set(AOCL_DECOMPRESS_FAST "OFF") # Fast decompression modes supported in single threaded mode only
endif()
if(NOT AOCL_DECOMPRESS_FAST STREQUAL "OFF")
    # ZSTD
    set(AOCL_ZSTD_WILDCOPY_LONG "ON") # >= 1 
    # LZ4
    set(AOCL_LZ4_DECOMPRESS_FAST "ON")
endif()

# Faster compression settings for supported methods
set(AOCL_COMPRESS_FAST "OFF" CACHE STRING "Enable fast compression modes that might compromise on compression ratio but compress faster. Supported values: {1,2} ZSTD." )
set_property(
    CACHE AOCL_COMPRESS_FAST
    PROPERTY STRINGS
    "OFF" "1" "2" # Higher values generally lead to faster compression with larger compromise on compression ratio
)

# Avoid enabling AOCL and native multi-threaded implementations simultaneously as it might lead to sub-optimal performance
if (AOCL_ENABLE_THREADS AND NATIVE_ENABLE_THREADS)
    message(WARNING "Both AOCL_ENABLE_THREADS and NATIVE_ENABLE_THREADS are enabled. NATIVE_ENABLE_THREADS will be disabled.")
    set(NATIVE_ENABLE_THREADS OFF)
endif()

# Force static build when BUILD_UTILITY is enabled as some utilities cannot link to shared library
# Note: minigzip utility can be built by linking to shared library as well. If that is desired, disable this section.
if (BUILD_UTILITY AND NOT BUILD_STATIC_LIBS)
    message(WARNING "Building static library to allow all utilities to be built.")
    set(BUILD_STATIC_LIBS ON)
endif()

if (AOCL_EXCLUDE_LZ4 AND NOT AOCL_EXCLUDE_LZ4HC)
    # as lz4hc depends on lz4, it should be excluded along with lz4
    message(WARNING "lz4hc is excluded as it depends on lz4")
    set(AOCL_EXCLUDE_LZ4HC ON)
endif()

if (AOCL_EXCLUDE_BZIP2 AND AOCL_EXCLUDE_LZ4 AND AOCL_EXCLUDE_LZ4HC AND AOCL_EXCLUDE_LZMA AND AOCL_EXCLUDE_SNAPPY AND AOCL_EXCLUDE_ZLIB AND AOCL_EXCLUDE_ZSTD)
    message(FATAL_ERROR "All compression methods are excluded. Unable to build library.")
endif()

# setup to get source code coverage(GCOV)
if (CODE_COVERAGE)
    if (NOT (${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${CMAKE_C_COMPILER_ID} STREQUAL "GNU"))
        # Switch off GCOV: target not supported
        set(CODE_COVERAGE OFF)
        message(WARNING "Target not supported. Code coverage is only supported on Linux OS with GCC compiler")
    endif()
endif()

# setup to run memory check using AddressSanitizer 
if (ASAN)
    if (NOT (${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND CMAKE_BUILD_TYPE MATCHES "Debug"))
        # Switch off ASAN: target not supported
        set(ASAN OFF)
        message(WARNING "Target not supported. ASAN is only supported on Linux/Debug build")
    endif()
endif()

# setup to run memory check using Valgrind
if (VALGRIND)
    if ((NOT (${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND CMAKE_BUILD_TYPE MATCHES "Debug")) OR ASAN)
        set(VALGRIND OFF)
        message(WARNING "Target not supported. VALGRIND is only supported with Linux/Debug build and incompatible with ASAN=ON")
    endif()
endif()

# Enable CTest for any of these cases
if(VALGRIND OR ASAN)
    set(AOCL_TEST_COVERAGE ON)
endif()

# validating google fuzz test environment
if(AOCL_TEST_FUZZER_WITH_CORPUS)
    set(AOCL_TEST_FUZZER ON)
endif()
if(AOCL_TEST_FUZZER)
    if (NOT (${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${CMAKE_C_COMPILER_ID} STREQUAL "Clang"))
        set(AOCL_TEST_FUZZER OFF)
        message(WARNING  "Target not supported. Fuzz tests are only supported on Linux OS with Clang compiler")
    endif()
endif()
if(AOCL_TEST_FUZZER)
    set(AOCL_TEST_COVERAGE ON)
endif()

#CODE_COVERAGE=ON, AOCL_TEST_COVERAGE=ON                                 : run coverage for aocl + third_party ctests
#CODE_COVERAGE=ON, AOCL_TEST_COVERAGE=OFF, TEST_COVERAGE_THIRD_PARTY=OFF : run coverage for aocl + third_party ctests
#CODE_COVERAGE=ON, AOCL_TEST_COVERAGE=OFF, TEST_COVERAGE_THIRD_PARTY=ON  : run coverage for third_party ctests only
if(CODE_COVERAGE)
    set(AOCL_EXCLUDE_DEPRECATED_APIS ON) # exclude deprecated APIs while getting code coverage statistics
    if(NOT (AOCL_TEST_COVERAGE OR TEST_COVERAGE_THIRD_PARTY))
        set(AOCL_TEST_COVERAGE ON) # enable maximum coverage
    endif()
endif()

if(AOCL_TEST_COVERAGE)
    set(TEST_COVERAGE_THIRD_PARTY ON) # enable to maximize test coverage
endif()

#list source files to be compiled for the library
set (API_SRC_FILES  api/code_alignment.cpp
                    api/api.cpp
                    api/codec.cpp
                    api/aocl_compression.h
                    api/codec.h
                    api/types.h)

set (ALGOS_PATH algos)
set (CMAKE_CONFIG_PATH cmake)
if (NOT AOCL_EXCLUDE_BZIP2)
set (BZIP2_SRC_FILES    ${ALGOS_PATH}/bzip2/code_alignment.c
                        ${ALGOS_PATH}/bzip2/blocksort.c
                        ${ALGOS_PATH}/bzip2/huffman.c
                        ${ALGOS_PATH}/bzip2/crctable.c
                        ${ALGOS_PATH}/bzip2/randtable.c
                        ${ALGOS_PATH}/bzip2/compress.c
                        ${ALGOS_PATH}/bzip2/decompress.c
                        ${ALGOS_PATH}/bzip2/bzlib.c
                        ${ALGOS_PATH}/bzip2/bzlib.h
                        ${ALGOS_PATH}/bzip2/libsais.c
                        ${ALGOS_PATH}/bzip2/libsais.h)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_BZIP2")
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
include(${CMAKE_CONFIG_PATH}/algos/lz4/lz4_compress_config.cmake)
include(${CMAKE_CONFIG_PATH}/algos/lz4/lz4_decompress_config.cmake)
set (LZ4_SRC_FILES  ${ALGOS_PATH}/lz4/code_alignment_lz4.c
                    ${ALGOS_PATH}/lz4/lz4.c
                    ${ALGOS_PATH}/lz4/lz4.h
                    ${ALGOS_PATH}/lz4/xxhash.c)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4")
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC)
set (LZ4HC_SRC_FILES    ${ALGOS_PATH}/lz4/code_alignment_lz4hc.c
                        ${ALGOS_PATH}/lz4/lz4hc.c
                        ${ALGOS_PATH}/lz4/lz4hc.h)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4HC")
endif ()
if (NOT AOCL_EXCLUDE_LZMA)
set (LZMA_SRC_FILES ${ALGOS_PATH}/lzma/code_alignment.c
                    ${ALGOS_PATH}/lzma/LzFind.c
                    ${ALGOS_PATH}/lzma/LzmaDec.c
                    ${ALGOS_PATH}/lzma/LzmaEnc.c
                    ${ALGOS_PATH}/lzma/Alloc.c
                    ${ALGOS_PATH}/lzma/Alloc.h
                    ${ALGOS_PATH}/lzma/LzmaDec.h
                    ${ALGOS_PATH}/lzma/LzmaEnc.h
                    ${ALGOS_PATH}/lzma/LzFind.h)

if (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL)
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL")
set (LZMA_SRC_FILES ${LZMA_SRC_FILES}
                    ${ALGOS_PATH}/lzma/xzlzma/XzLzma.c
                    ${ALGOS_PATH}/lzma/xzlzma/XzLzma.h
                    ${ALGOS_PATH}/lzma/xzlzma/XzVersion.h)
endif()
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZMA")
endif ()
if (NOT AOCL_EXCLUDE_SNAPPY)
set (SNAPPY_SRC_FILES   ${ALGOS_PATH}/snappy/code_alignment.cc
                        ${ALGOS_PATH}/snappy/snappy-sinksource.cc
                        ${ALGOS_PATH}/snappy/snappy-stubs-internal.cc
                        ${ALGOS_PATH}/snappy/snappy.cc
                        ${ALGOS_PATH}/snappy/snappy-c.cc
                        ${ALGOS_PATH}/snappy/snappy.h)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_SNAPPY")
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
set (ZLIB_SRC_FILES ${ALGOS_PATH}/zlib/code_alignment.c
                    ${ALGOS_PATH}/zlib/adler32.c
                    ${ALGOS_PATH}/zlib/adler32_x86.c
                    ${ALGOS_PATH}/zlib/compress.c
                    ${ALGOS_PATH}/zlib/crc32.c
                    ${ALGOS_PATH}/zlib/crc32_x86_pclmul.c
                    ${ALGOS_PATH}/zlib/crc32_x86_vpclmulqdq.c
                    ${ALGOS_PATH}/zlib/deflate.c
                    ${ALGOS_PATH}/zlib/deflate_medium.c
                    ${ALGOS_PATH}/zlib/deflate_quick.c
                    ${ALGOS_PATH}/zlib/gzclose.c
                    ${ALGOS_PATH}/zlib/gzlib.c
                    ${ALGOS_PATH}/zlib/gzread.c
                    ${ALGOS_PATH}/zlib/gzwrite.c
                    ${ALGOS_PATH}/zlib/infback.c
                    ${ALGOS_PATH}/zlib/inffast.c
                    ${ALGOS_PATH}/zlib/inflate.c
                    ${ALGOS_PATH}/zlib/inftrees.c
                    ${ALGOS_PATH}/zlib/longest_match_x86.c
                    ${ALGOS_PATH}/zlib/slide_hash_x86.c
                    ${ALGOS_PATH}/zlib/trees.c
                    ${ALGOS_PATH}/zlib/uncompr.c
                    ${ALGOS_PATH}/zlib/zutil.c
                    ${ALGOS_PATH}/zlib/zlib.h
                    ${ALGOS_PATH}/zlib/aocl_zlib_utils.c)
set (ZLIBNG_SRC_FILES        ${ALGOS_PATH}/zlib/crc32_x86_vpclmulqdq.c
                             ${ALGOS_PATH}/zlib/zlib-ng/arch/x86/chunkset_sse2.c
                             ${ALGOS_PATH}/zlib/zlib-ng/arch/x86/chunkset_avx512.c)
set_property(SOURCE ${ZLIBNG_SRC_FILES} APPEND PROPERTY INCLUDE_DIRECTORIES 
                    "${CMAKE_SOURCE_DIR}/${ALGOS_PATH}/zlib/zlib-ng/;${CMAKE_SOURCE_DIR}/${ALGOS_PATH}/zlib/")
set_property(SOURCE ${ZLIBNG_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DHAVE_ATTRIBUTE_ALIGNED=1;-DHAVE_HIDDEN=1")
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
endif ()
if (NOT AOCL_EXCLUDE_ZSTD)
include(${CMAKE_CONFIG_PATH}/algos/zstd/zstd_compress_config.cmake)
include(${CMAKE_CONFIG_PATH}/algos/zstd/zstd_decompress_config.cmake)
set (ZSTD_SRC_FILES ${ALGOS_PATH}/zstd/lib/common/code_alignment.c
                    ${ALGOS_PATH}/zstd/lib/common/zstd_common.c
                    ${ALGOS_PATH}/zstd/lib/common/fse_decompress.c
                    ${ALGOS_PATH}/zstd/lib/common/xxhash.c
                    ${ALGOS_PATH}/zstd/lib/common/error_private.c
                    ${ALGOS_PATH}/zstd/lib/common/entropy_common.c
                    ${ALGOS_PATH}/zstd/lib/common/pool.c
                    ${ALGOS_PATH}/zstd/lib/common/debug.c
                    ${ALGOS_PATH}/zstd/lib/common/threading.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_compress.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_compress_literals.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_compress_sequences.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_compress_superblock.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstdmt_compress.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_double_fast.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_fast.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_lazy.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_ldm.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_opt.c
                    ${ALGOS_PATH}/zstd/lib/compress/fse_compress.c
                    ${ALGOS_PATH}/zstd/lib/compress/huf_compress.c
                    ${ALGOS_PATH}/zstd/lib/compress/hist.c
                    ${ALGOS_PATH}/zstd/lib/compress/zstd_preSplit.c
                    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress.c
                    ${ALGOS_PATH}/zstd/lib/decompress/huf_decompress.c
                    ${ALGOS_PATH}/zstd/lib/decompress/huf_decompress_amd64.S
                    ${ALGOS_PATH}/zstd/lib/decompress/zstd_ddict.c
                    ${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block.c
                    ${ALGOS_PATH}/zstd/lib/dictBuilder/cover.c
                    ${ALGOS_PATH}/zstd/lib/dictBuilder/divsufsort.c
                    ${ALGOS_PATH}/zstd/lib/dictBuilder/fastcover.c
                    ${ALGOS_PATH}/zstd/lib/dictBuilder/zdict.c
                    ${ALGOS_PATH}/zstd/lib/zstd.h)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZSTD")
endif ()
set (COMMON_SRC_FILES ${ALGOS_PATH}/common/aoclHashChain.h
                      ${ALGOS_PATH}/common/aoclAlgoOpt.h
                      ${ALGOS_PATH}/common/aoclFds.h)
set (UTILS_SRC_FILES	utils/code_alignment.cpp
						utils/utils.cpp
						utils/utils.h)
# Source files utilizing AVX-512 and higher ISA extensions
set (AVX512_SRC_FILES ${ALGOS_PATH}/zlib/crc32_x86_vpclmulqdq.c
                      ${ALGOS_PATH}/zlib/zlib-ng/arch/x86/chunkset_avx512.c)

if (AOCL_ENABLE_THREADS)
    set (THREADS_SRC_FILES threads/code_alignment.c
                           threads/threads.c
                           threads/threads.h)
    if (NOT AOCL_EXCLUDE_BZIP2)
        set(BZIP2_SRC_FILES ${BZIP2_SRC_FILES}  ${ALGOS_PATH}/bzip2/aocl_bzip2_mt_helper.c 
                                                ${ALGOS_PATH}/bzip2/aocl_bzip2_mt_helper.h)
    endif ()
endif ()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DEST_LIB_PATH lib)
set(DEST_INC_PATH include)
set(DEST_EXAMPLES_PATH examples)
#set(DEST_BIN_PATH bin)

set(INC_FILES api/aocl_compression.h
              ${ALGOS_PATH}/common/aoclAlgoOpt.h
              ${ALGOS_PATH}/common/aoclFds.h)
if (NOT AOCL_EXCLUDE_BZIP2)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/bzip2/bzlib.h)
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/lz4/lz4.h)
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/lz4/lz4hc.h)
endif ()
if (NOT AOCL_EXCLUDE_LZMA)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/lzma/7zTypes.h
                               ${ALGOS_PATH}/lzma/Alloc.h
                               ${ALGOS_PATH}/lzma/LzmaDec.h
                               ${ALGOS_PATH}/lzma/LzmaEnc.h)
    if (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL)
        set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/lzma/lzma.h)
        set(INC_XZ_UTILS_LZMA ${ALGOS_PATH}/lzma/xzlzma)
    endif()       
endif ()
if (NOT AOCL_EXCLUDE_SNAPPY)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/snappy/snappy-stubs-public.h
                               ${ALGOS_PATH}/snappy/snappy-sinksource.h
                               ${ALGOS_PATH}/snappy/snappy.h
                               ${ALGOS_PATH}/snappy/snappy-c.h)
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/zlib/zconf.h
                               ${ALGOS_PATH}/zlib/zlib.h)
endif ()
if (NOT AOCL_EXCLUDE_ZSTD)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/zstd/lib/zstd.h
                               ${ALGOS_PATH}/zstd/lib/zdict.h
                               ${ALGOS_PATH}/zstd/lib/zstd_errors.h)
endif ()

if (NOT AOCL_EXCLUDE_LZ4)
    if (LZ4_FRAME_FORMAT_SUPPORT)
        set(INC_FILES ${INC_FILES}
                      ${ALGOS_PATH}/lz4/lz4frame.h
                      ${ALGOS_PATH}/lz4/lz4frame_static.h)
        set (LZ4_SRC_FILES ${LZ4_SRC_FILES}
                           ${ALGOS_PATH}/lz4/lz4frame.c
                           ${ALGOS_PATH}/lz4/lz4frame.h)
        if (AOCL_EXCLUDE_LZ4HC)
            set (LZ4_SRC_FILES ${LZ4_SRC_FILES}
                               ${ALGOS_PATH}/lz4/lz4hc.c
                               ${ALGOS_PATH}/lz4/lz4hc.h)
        endif()
    endif()
endif()

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type selected is Release")
    add_compile_definitions(CFLAGS_SET1="${CMAKE_C_FLAGS_RELEASE}")
    add_compile_definitions(_FORTIFY_SOURCE=2)
else ()
    message(STATUS "Build type selected is Debug")
    add_compile_definitions(CFLAGS_SET1="${CMAKE_C_FLAGS_DEBUG}")
	if (AOCL_TEST_COVERAGE OR TEST_COVERAGE_THIRD_PARTY) # disable asserts to allow execution of unit tests without intermediate exit
		set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
		set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
		set_property(SOURCE ${LZMA_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
		set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
    else () # enable all debug flags
		set_property(SOURCE ${BZIP2_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DBZ_DEBUG")
		set_property(SOURCE ${ZLIB_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DZLIB_DEBUG")
	endif()
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DDEBUGLEVEL=1")
endif()

#set(USER_DEBUG_ON "-g")
message(STATUS "C Compiler used: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler used: ${CMAKE_CXX_COMPILER}")
set_source_files_properties(${ZLIB_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1")

if(AOCL_ENABLE_THREADS) # OpenMP dependency for MT feature support
    if(WIN32 OR NOT OpenMP_libomp_LIBRARY)
        find_package(OpenMP REQUIRED)
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    endif()
endif()

if (WIN32)
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} /O2 /DNDEBUG /Oy /arch:SSE2 /arch:AVX /arch:AVX2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} /O2 /DNDEBUG /Oy /arch:SSE2 /arch:AVX /arch:AVX2")
        # Security flags
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /GS /DYNAMICBASE /w14774 /w14775")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS /DYNAMICBASE /w14774 /w14775")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DYNAMICBASE /NXCOMPAT /GUARD:CF /HIGHENTROPYVA")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DYNAMICBASE /NXCOMPAT /GUARD:CF /HIGHENTROPYVA")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} /O2 /Oy /arch:SSE2 /arch:AVX /arch:AVX2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} /O2 /Oy /arch:SSE2 /arch:AVX /arch:AVX2")
	endif()
    if(ENABLE_FAST_MATH)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:fast")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
    endif()
	set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_EXPECT=1")
    set_source_files_properties(${AVX512_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-mpclmul;-mvpclmulqdq;/arch:AVX512")
    # set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DDYNAMIC_BMI2=1")
if(NOT BUILD_STATIC_LIBS)
    set_property(SOURCE ${ZLIB_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DZLIB_DLL_EXPORT=1")
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZ4_DLL_EXPORT=1")
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DSNAPPY_DLL_EXPORT=1")
    set_property(SOURCE ${LZMA_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZMA_DLL_EXPORT=1")
    set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZ4_DLL_EXPORT=1")
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DZSTD_DLL_EXPORT=1")
    set_property(SOURCE ${BZIP2_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DBZIP2_DLL_EXPORT=1")
    if (TEST_COVERAGE_THIRD_PARTY OR BUILD_UTILITY)
        set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DFSE_DLL_EXPORT=1")
        set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DXXH_EXPORT=1")
    endif()
endif()
elseif (UNIX)
    set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_FLAGS "-msse4.1")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(SECURITY_FLAGS "-fstack-protector-strong -fpie -Wformat -Wformat-security")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SECURITY_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SECURITY_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -Wl,-z,relro -Wl,-z,now")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now")
endif()
if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    # Optimization level controls at per function level are not available in Clang.
    # Hence setting it at file level here for lz4.
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-O2")
endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} -fomit-frame-pointer -fstrict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} -fomit-frame-pointer -fstrict-aliasing")
    if(ENABLE_FAST_MATH)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
    endif()
    set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_ATTRIBUTE_ALWAYS_INLINE=1;-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1;-DHAVE_BUILTIN_PREFETCH=1;-DSNAPPY_HAVE_X86_CRC32=1;")
endif()

include(CheckIncludeFile)
check_include_file("sys/uio.h" SNAPPY_HAVE_SYS_UIO_H) # Some of the snappy APIs change based on this.
if (${SNAPPY_HAVE_SYS_UIO_H})
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DHAVE_SYS_UIO_H")
endif()

if (AOCL_LZ4_OPT_PREFETCH_BACKWARDS)
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS")
endif()
if (AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1)
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1")
endif()
if (AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2)
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1")
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2")
endif()
if (AOCL_LZ4_NEW_PRIME_NUMBER)
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_NEW_PRIME_NUMBER")
endif()
if (AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES)
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_EXTRA_HASH_TABLE_UPDATES")
endif()
if (AOCL_LZ4_HASH_BITS_USED)
    string(TOUPPER ${AOCL_LZ4_HASH_BITS_USED} AOCL_LZ4_HASH_BITS_USED)
    if (AOCL_LZ4_HASH_BITS_USED  STREQUAL "LOW")
        set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_HASH_BITS_USED=41")
    elseif(AOCL_LZ4_HASH_BITS_USED  STREQUAL "HIGH")
        set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_HASH_BITS_USED=44")
    elseif(NOT AOCL_LZ4_HASH_BITS_USED  STREQUAL "OFF")
        message(FATAL_ERROR "AOCL_LZ4_HASH_BITS_USED can only be LOW or HIGH, the provided value is:${AOCL_LZ4_HASH_BITS_USED}")
    endif()
endif()
if (AOCL_LZ4_DECOMPRESS_FAST)
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_DECOMPRESS_FAST")
endif()
if (SNAPPY_MATCH_SKIP_OPT)
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_SNAPPY_MATCH_SKIP_OPT")
endif()
if (SNAPPY_HIGH_COMPRESSION)
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_SNAPPY_HIGH_COMPRESSION")
endif()
if (SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS)
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS")
endif()
if (AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS)
    set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS")
endif()
if (AOCL_ZSTD_SEARCH_SKIP_OPT)
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ZSTD_SEARCH_SKIP_OPT")
endif()
if (AOCL_DECOMPRESS_FAST)
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DECOMPRESS_FAST=${AOCL_DECOMPRESS_FAST}")
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DECOMPRESS_FAST=${AOCL_DECOMPRESS_FAST}")
endif()
if (AOCL_COMPRESS_FAST)
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_COMPRESS_FAST=${AOCL_COMPRESS_FAST}")
endif()
if (AOCL_ZSTD_WILDCOPY_LONG)
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ZSTD_WILDCOPY_LONG")
endif()
if (NATIVE_ENABLE_THREADS)
    set_property(SOURCE ${ZSTD_SRC_FILES}  APPEND PROPERTY COMPILE_OPTIONS "-DZSTD_MULTITHREAD")
    # Prefer -pthread flag
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    message(STATUS "Found Threads library for native multi-threading support")
endif()
if(AOCL_EXCLUDE_DEPRECATED_APIS)
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_DEPRECATED_APIS")
    set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_DEPRECATED_APIS")
endif()
if (AOCL_BZIP2_HUFFMAN_ITERATIONS)
    if (AOCL_BZIP2_HUFFMAN_ITERATIONS STREQUAL "1" OR
        AOCL_BZIP2_HUFFMAN_ITERATIONS STREQUAL "2" OR
        AOCL_BZIP2_HUFFMAN_ITERATIONS STREQUAL "3" OR
        AOCL_BZIP2_HUFFMAN_ITERATIONS STREQUAL "4")
        set_property(SOURCE ${BZIP2_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_BZIP2_HUFFMAN_ITERATIONS=${AOCL_BZIP2_HUFFMAN_ITERATIONS}")
    else()
        message(FATAL_ERROR "AOCL_BZIP2_HUFFMAN_ITERATIONS can only be '1', '2', '3' or '4', the provided value is:${AOCL_BZIP2_HUFFMAN_ITERATIONS}")
    endif()
endif()
include(CheckCSourceCompiles)
#
# Check if we can hide zlib internal symbols that are linked between separate source files using hidden
#
check_c_source_compiles(
    "#define ZLIB_INTERNAL __attribute__((visibility (\"hidden\")))
    int ZLIB_INTERNAL foo;
    int main() {
        return 0;
    }"
    HAVE_ATTRIBUTE_VISIBILITY_HIDDEN FAIL_REGEX "visibility")
if(HAVE_ATTRIBUTE_VISIBILITY_HIDDEN)
    set_property(SOURCE ${ZLIB_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DHAVE_HIDDEN")
endif()

add_compile_definitions(CCompiler="${CMAKE_C_COMPILER}")
add_compile_definitions(CXXCompiler="${CMAKE_CXX_COMPILER}")
add_compile_definitions(CFLAGS_SET2="${CMAKE_C_FLAGS}")

#set library and sources
set(lib_name aocl_compression)
set(SRC_FILES 
        ${UTILS_SRC_FILES}
        ${API_SRC_FILES}
        ${BZIP2_SRC_FILES}
        ${LZ4_SRC_FILES}
        ${LZ4HC_SRC_FILES} 
        ${LZMA_SRC_FILES}
        ${SNAPPY_SRC_FILES}
        ${ZLIB_SRC_FILES}
        ${ZLIBNG_SRC_FILES}
        ${ZSTD_SRC_FILES}
        ${COMMON_SRC_FILES}
        ${THREADS_SRC_FILES}
)


#common flag for aocl_compression_bench and gtest_combined_test
set(AOCL_DYNAMIC_DISPATCHER ON)
if (AOCL_DYNAMIC_DISPATCHER)
    set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
    option(ENABLE_STRICT_WARNINGS "Enable compiler flags to treat all warnings as errors (Enabled by default)" ON)
else()
    set(ENABLE_STRICT_WARNINGS OFF)
endif()

if (AOCL_ENABLE_THREADS)
    set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_THREADS")
endif()

#setup CTest for GoogleTest and aocl_compression_bench test
if (AOCL_TEST_COVERAGE OR TEST_COVERAGE_THIRD_PARTY)
    # enabling ctest
    enable_testing()

	set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_UNIT_TEST")
    
    if (AOCL_TEST_COVERAGE)
        add_subdirectory(gtest)
    endif()

    if (TEST_COVERAGE_THIRD_PARTY)
        add_subdirectory(test/third_party)
    endif()
endif()

#Disable strict warnings for Debug build
if (NOT(CMAKE_BUILD_TYPE STREQUAL "Release"))
    message(WARNING  "Do not treat warnings as errors for non Release build")
    set(ENABLE_STRICT_WARNINGS OFF)
endif()
#Enabling strict warnings
if(ENABLE_STRICT_WARNINGS)
    if (WIN32)
        #suppress warnings suggesting to use fopen_s, ... in place of fopen, ...
        set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-D_CRT_SECURE_NO_WARNINGS")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
        set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DENABLE_STRICT_WARNINGS")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic")
        set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DENABLE_STRICT_WARNINGS")
    endif()
endif()

# Add utility tests
if (BUILD_UTILITY)
    if (NOT TEST_COVERAGE_THIRD_PARTY)
        add_subdirectory(test/third_party)
    endif()
endif()

#add library
if (BUILD_STATIC_LIBS)
    add_library(${lib_name} STATIC ${SRC_FILES})
    message(STATUS "Library type being built is: Static")
else ()
    add_library(${lib_name} SHARED ${SRC_FILES})
    message(STATUS "Library type being built is: Shared")
endif()
set_target_properties(${lib_name}   PROPERTIES 
                                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
                                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
                                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_command(
        TARGET ${lib_name}
        POST_BUILD
        COMMENT "Copy library to ${ROOT_DIR}/lib"
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${ROOT_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ROOT_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_name}> ${ROOT_DIR}/lib
        )
# add the compiler and linker flags for code coverage
if (CODE_COVERAGE)
    target_compile_options(${lib_name} PUBLIC --coverage -fprofile-abs-path)
    target_link_options(${lib_name} PUBLIC --coverage)        
endif()

# add the compiler and linker flags for AddressSanitizer 
if (ASAN)
    target_compile_options(${lib_name} PUBLIC -fsanitize=address -O0 -gdwarf-5 -g3 )
    target_link_options(${lib_name} PUBLIC -fsanitize=address)
endif()

set_target_properties(${lib_name} PROPERTIES PUBLIC_HEADER "${INC_FILES}")

target_include_directories(${lib_name}
                            PRIVATE ${CMAKE_SOURCE_DIR}
                            PRIVATE ${PROJECT_BINARY_DIR} # required for files generated from .in templates
                            PRIVATE ${ALGOS_PATH}/common
                            PRIVATE api)

if (OPENMP_FOUND AND (WIN32 OR NOT OpenMP_libomp_LIBRARY))
    target_link_libraries (${lib_name} PUBLIC OpenMP::OpenMP_C OpenMP::OpenMP_CXX)
else()
    target_link_libraries(${lib_name} PUBLIC ${OpenMP_libomp_LIBRARY})
endif ()

# Link pthread library for native multi-threading 
if (NATIVE_ENABLE_THREADS)
    target_link_libraries(${lib_name} PUBLIC Threads::Threads)
    message(STATUS "Linked Threads::Threads for native multi-threading support")
endif()

#add benchmarking application and tests
add_subdirectory(test)

if (AOCL_ENABLE_LOG_FEATURE)
	set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_LOG_FEATURE")
endif()

#Install into destination path by using -DCMAKE_INSTALL_PREFIX=<path> while configuring cmake
message(STATUS "AOCL COMPRESSION Library, Interface header files will be installed at ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${lib_name} 
        DESTINATION ${DEST_LIB_PATH}
        PUBLIC_HEADER DESTINATION ${DEST_INC_PATH})
if (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL)
    install(DIRECTORY ${INC_XZ_UTILS_LZMA} DESTINATION ${DEST_INC_PATH} FILES_MATCHING PATTERN "*.h")
endif()

#Install examples
set (EXAMPLES_PATH examples)
if (NOT AOCL_EXCLUDE_BZIP2)
    install(DIRECTORY ${EXAMPLES_PATH}/bzip2 DESTINATION ${DEST_EXAMPLES_PATH})
endif()
if (NOT AOCL_EXCLUDE_ZLIB)
    if(AOCL_ENABLE_THREADS)
        install(DIRECTORY ${EXAMPLES_PATH}/zlib DESTINATION ${DEST_EXAMPLES_PATH})
    else()
        install(DIRECTORY ${EXAMPLES_PATH}/zlib DESTINATION ${DEST_EXAMPLES_PATH}
            PATTERN "example_compress2_gzip.c" EXCLUDE
            PATTERN "example_compress2_raw.c" EXCLUDE)
    endif()
endif()
if (NOT AOCL_EXCLUDE_LZ4)
    install(DIRECTORY ${EXAMPLES_PATH}/lz4 DESTINATION ${DEST_EXAMPLES_PATH})
endif()
if (NOT AOCL_EXCLUDE_LZ4HC)
    install(DIRECTORY ${EXAMPLES_PATH}/lz4hc DESTINATION ${DEST_EXAMPLES_PATH})
endif()
if (NOT AOCL_EXCLUDE_SNAPPY)
    install(DIRECTORY ${EXAMPLES_PATH}/snappy DESTINATION ${DEST_EXAMPLES_PATH})
endif()
if (NOT AOCL_EXCLUDE_LZMA)
    install(DIRECTORY ${EXAMPLES_PATH}/lzma DESTINATION ${DEST_EXAMPLES_PATH})
endif()
if (NOT AOCL_EXCLUDE_ZSTD)
    install(DIRECTORY ${EXAMPLES_PATH}/zstd DESTINATION ${DEST_EXAMPLES_PATH})
endif()
if(AOCL_ENABLE_THREADS)
    install(DIRECTORY ${EXAMPLES_PATH}/api DESTINATION ${DEST_EXAMPLES_PATH})
else()
    install(DIRECTORY ${EXAMPLES_PATH}/api DESTINATION ${DEST_EXAMPLES_PATH}
        PATTERN "example_aocl_llc_skip_rap_frame.c" EXCLUDE)
endif()
install(FILES ${EXAMPLES_PATH}/CMakeLists.txt DESTINATION ${DEST_EXAMPLES_PATH})
install(FILES ${EXAMPLES_PATH}/README.md DESTINATION ${DEST_EXAMPLES_PATH})

#Create symbolic links from expected library names to aocl library during non-static build
function(create_sym_link_to_lib sym_name)
    file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/${DEST_LIB_PATH}" DIR_SYM_INSTALL)
    if(WIN32)
        install(CODE "execute_process(COMMAND cmd /c echo Creating symbolic link ${sym_name}.dll to ${lib_name}.dll)")
        install(CODE "execute_process( \
        COMMAND cmd /c if exist ${sym_name}.dll del ${sym_name}.dll \
        WORKING_DIRECTORY ${DIR_SYM_INSTALL})")
        install(CODE "execute_process( \
        COMMAND cmd /c mklink /H ${sym_name}.dll ${lib_name}.dll \
        WORKING_DIRECTORY ${DIR_SYM_INSTALL})")
    else()
        install(CODE "execute_process(COMMAND echo Creating symbolic link ${sym_name}.so to ${lib_name}.so)")
        install(CODE "execute_process( \
        COMMAND rm -f ${sym_name}.so \
        WORKING_DIRECTORY ${DIR_SYM_INSTALL})")
        install(CODE "execute_process( \
        COMMAND ln -s lib${lib_name}.so ${sym_name}.so \
        WORKING_DIRECTORY ${DIR_SYM_INSTALL})")
    endif()
endfunction()
if(NOT BUILD_STATIC_LIBS)
    if (NOT AOCL_EXCLUDE_BZIP2)
        create_sym_link_to_lib(libbz2)
    endif()
    if (NOT AOCL_EXCLUDE_ZLIB)
        create_sym_link_to_lib(libz)
    endif()
    if (NOT AOCL_EXCLUDE_LZ4)
        create_sym_link_to_lib(liblz4)
    endif()
    if (NOT AOCL_EXCLUDE_SNAPPY)
        create_sym_link_to_lib(libsnappy)
    endif()
    if (NOT AOCL_EXCLUDE_LZMA)
        create_sym_link_to_lib(liblzma)
    endif()
    if (NOT AOCL_EXCLUDE_ZSTD)
        create_sym_link_to_lib(libzstd)
    endif()
endif()

#Uninstall the binary. library and header files using custom command
add_custom_target("uninstall" COMMENT "Uninstalling the installed files from ${install_dir}")
add_custom_command(
    TARGET "uninstall"
    POST_BUILD
    COMMENT "Uninstall the installed files as per install_manifest.txt and from ${ROOT_DIR}/lib/"
    COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in
            install_manifest.txt to be uninstalled!
    COMMAND rm -vf ${ROOT_DIR}/lib/*
    )

if (BUILD_DOC)
    add_subdirectory(docs)
endif()

if (BUILD_EXAMPLE)
    set(ENABLE_EXAMPLE_OUTOFSRCTREE OFF)
    add_subdirectory(examples)
endif()
