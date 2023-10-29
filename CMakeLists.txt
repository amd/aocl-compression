# Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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


cmake_minimum_required(VERSION 3.10.0)

#set the project name
project(AOCL-COMPRESSION-LIBRARY CXX C ASM)
message(STATUS "Building for AOCL COMPRESSION LIBRARY")

string(TIMESTAMP TODAY "%Y%m%d")
add_compile_definitions(AOCL_BUILD_VERSION="Build ${TODAY}")

message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER_ID}")
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    if (${CMAKE_C_COMPILER_VERSION} VERSION_LESS 6.1)
        message(FATAL_ERROR "Minimum GCC version supported is 6.1!")
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
option (SNAPPY_MATCH_SKIP_OPT "Enable Snappy match skipping optimization (Disabled by default)" OFF)
option (LZ4_FRAME_FORMAT_SUPPORT "Enable building LZ4 with Frame format and API support (Enabled by default)" ON)
option (AOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST "Enable ZSTD match skipping optimization, and reduce search strength/tolerance for levels 1-4 (Disabled by default)" OFF)
option (AOCL_ZSTD_WILDCOPY_LONG "Faster wildcopy when match lengths are long in ZSTD decompression (Disabled by default) " OFF)
option (AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS "Disable Pattern Analysis in LZ4HC for level 9 (Enabled by default)" ON)
option (AOCL_TEST_COVERAGE "Enable GTest and AOCL test bench based CTest suite (Disabled by default)" OFF)
option (CODE_COVERAGE "Enable source code coverage (only on Linux with GCC compiler)" OFF)
option (ASAN "Enable Address Sanitizer checks (only on Linux/Debug build)" OFF)
option (VALGRIND "Enable Valgrind checks (only on Linux/Debug. Incompatible with ASAN=ON)" OFF)
option (BUILD_DOC "Build documentation for aocl-compression (Disabled by default)" OFF) 
option (AOCL_ENABLE_LOG_FEATURE "Enable logging support in library, for which log level is determined by environement variable AOCL_ENABLE_LOG (go through README.md) (Disabled by default)" OFF) 
option (ZLIB_DEFLATE_FAST_MODE "Enable ZLIB deflate quick strategy (Disabled by default)" OFF)
option (AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1 "Enable LZ4 match skipping optimization strategy-1 based on a larger base step size (Disabled by default)" OFF)
option (AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2 "Enable LZ4 match skipping optimization strategy-2 by aggressively setting search distance on top of strategy-1 (Disabled by default)" OFF)
option (AOCL_LZ4_NEW_PRIME_NUMBER "Enable the usage of a new prime number for LZ4 hashing function (Disabled by default)" OFF)
option (AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES "Enable storing of additional potential matches to improve compression ratio. Recommended for higher compressibility use cases (Disabled by default)" OFF)
option (AOCL_LZ4_HASH_BITS_USED "Control the number of bits used for LZ4 hashing, allowed values are LOW (low perf gain and less CR regression) and HIGH (high perf gain and high CR regression) (Disabled by default)" OFF)
option (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL "Build with xz utils lzma APIs. Experimental feature with limited API support (Disabled by default)" OFF)


option (AOCL_EXCLUDE_BZIP2 "Exclude BZIP2 compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_LZ4 "Exclude LZ4 compression method from the library build. LZ4HC also gets excluded (Disabled by default)" OFF)
option (AOCL_EXCLUDE_LZ4HC "Exclude LZ4HC compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_LZMA "Exclude LZMA compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_SNAPPY "Exclude SNAPPY compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_ZLIB "Exclude ZLIB compression method from the library build (Disabled by default)" OFF)
option (AOCL_EXCLUDE_ZSTD "Exclude ZSTD compression method from the library build (Disabled by default)" OFF)
option (AOCL_ENABLE_THREADS "Enable multi-threaded compression and decompression using SMP openMP threads (Disabled by default)" OFF)

#list source files to be compiled for the library
set (API_SRC_FILES  api/code_alignment.cpp
                    api/api.cpp
                    api/codec.cpp
                    api/aocl_compression.h
                    api/codec.h
                    api/types.h)

set (BENCH_PATH test)
set (TESTBENCH_SRC_FILES 
                    ${BENCH_PATH}/codec_bench.c 
                    ${BENCH_PATH}/codec_native_api_bench.c 
                    ${BENCH_PATH}/codec_bench.h)

set (ALGOS_PATH algos)
if (NOT AOCL_EXCLUDE_BZIP2)
set (BZIP2_SRC_FILES    ${ALGOS_PATH}/bzip2/code_alignment.c
                        ${ALGOS_PATH}/bzip2/blocksort.c
                        ${ALGOS_PATH}/bzip2/huffman.c
                        ${ALGOS_PATH}/bzip2/crctable.c
                        ${ALGOS_PATH}/bzip2/randtable.c
                        ${ALGOS_PATH}/bzip2/compress.c
                        ${ALGOS_PATH}/bzip2/decompress.c
                        ${ALGOS_PATH}/bzip2/bzlib.c
                        ${ALGOS_PATH}/bzip2/bzlib.h)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_BZIP2")
set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_BZIP2")
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
set (LZ4_SRC_FILES  ${ALGOS_PATH}/lz4/code_alignment_lz4.c
                    ${ALGOS_PATH}/lz4/lz4.c
                    ${ALGOS_PATH}/lz4/lz4.h
                    ${ALGOS_PATH}/lz4/xxhash.c)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4")
set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4")
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC AND NOT AOCL_EXCLUDE_LZ4)
set (LZ4HC_SRC_FILES    ${ALGOS_PATH}/lz4/code_alignment_lz4hc.c
                        ${ALGOS_PATH}/lz4/lz4hc.c
                        ${ALGOS_PATH}/lz4/lz4hc.h)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4HC")
set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4HC")
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
set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZMA")
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
set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_SNAPPY")
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
set (ZLIB_SRC_FILES ${ALGOS_PATH}/zlib/code_alignment.c
                    ${ALGOS_PATH}/zlib/adler32.c
                    ${ALGOS_PATH}/zlib/adler32_x86.c
                    ${ALGOS_PATH}/zlib/compress.c
                    ${ALGOS_PATH}/zlib/crc32.c
                    ${ALGOS_PATH}/zlib/deflate.c
                    ${ALGOS_PATH}/zlib/deflate_medium.c
                    ${ALGOS_PATH}/zlib/deflate_quick.c
                    ${ALGOS_PATH}/zlib/gzclose.c
                    ${ALGOS_PATH}/zlib/gzlib.c
                    ${ALGOS_PATH}/zlib/gzread.c
                    ${ALGOS_PATH}/zlib/gzwrite.c
                    ${ALGOS_PATH}/zlib/infback.c
                    ${ALGOS_PATH}/zlib/inffast.c
                    ${ALGOS_PATH}/zlib/inffast_chunk.c
                    ${ALGOS_PATH}/zlib/inflate.c
                    ${ALGOS_PATH}/zlib/inftrees.c
                    ${ALGOS_PATH}/zlib/longest_match_x86.c
                    ${ALGOS_PATH}/zlib/slide_hash_x86.c
                    ${ALGOS_PATH}/zlib/trees.c
                    ${ALGOS_PATH}/zlib/uncompr.c
                    ${ALGOS_PATH}/zlib/zutil.c
                    ${ALGOS_PATH}/zlib/zlib.h)
else ()
set_property(SOURCE ${API_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
endif ()
if (NOT AOCL_EXCLUDE_ZSTD)
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
set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZSTD")
endif ()
set (COMMON_SRC_FILES ${ALGOS_PATH}/common/aoclHashChain.h)
set (UTILS_SRC_FILES	utils/code_alignment.cpp
						utils/utils.cpp
						utils/utils.h)

if (AOCL_ENABLE_THREADS)
    set (THREADS_SRC_FILES threads/threads.c threads/threads.h)
endif ()

#set the expected path variables
include_directories(.)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${ROOT_DIR}/build/)
set(DEST_LIB_PATH aocl_compression/lib)
set(DEST_INC_PATH aocl_compression/include)
set(DEST_BIN_PATH aocl_compression/bin)

set(INC_FILES api/aocl_compression.h)
if (NOT AOCL_EXCLUDE_BZIP2)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/bzip2/bzlib.h)
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
    set(INC_FILES ${INC_FILES} ${ALGOS_PATH}/lz4/lz4.h)
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC AND NOT AOCL_EXCLUDE_LZ4)
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
else ()
    message(STATUS "Build type selected is Debug")
    add_compile_definitions(CFLAGS_SET1="${CMAKE_C_FLAGS_DEBUG}")
	if (NOT AOCL_TEST_COVERAGE)
		set_property(SOURCE ${BZIP2_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DBZ_DEBUG")
		set_property(SOURCE ${ZLIB_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DZLIB_DEBUG")
		set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DDEBUGLEVEL=1")
	else()
		set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
		set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
		set_property(SOURCE ${LZMA_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
		set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNDEBUG")
	endif()
endif()

#set(USER_DEBUG_ON "-g")
message(STATUS "C Compiler used: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler used: ${CMAKE_CXX_COMPILER}")
if(ZLIB_DEFLATE_FAST_MODE)
set_source_files_properties(${ZLIB_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1;-DAOCL_ZLIB_DEFLATE_FAST_MODE=1")
else()
set_source_files_properties(${ZLIB_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1")
endif()
if (WIN32)
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} /O2 /DNDEBUG /fp:fast /Oy /arch:SSE2 /arch:AVX /arch:AVX2 /openmp")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} /O2 /DNDEBUG /fp:fast /Oy /arch:SSE2 /arch:AVX /arch:AVX2 /openmp")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} /O2 /fp:fast /Oy /arch:SSE2 /arch:AVX /arch:AVX2 /openmp")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} /O2 /fp:fast /Oy /arch:SSE2 /arch:AVX /arch:AVX2 /openmp")
	endif()
	set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_EXPECT=1")
if(NOT BUILD_STATIC_LIBS)
    set_property(SOURCE ${ZLIB_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DZLIB_DLL_EXPORT=1")
    set_property(SOURCE ${LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZ4_DLL_EXPORT=1")
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DSNAPPY_DLL_EXPORT=1")
    set_property(SOURCE ${LZMA_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZMA_DLL_EXPORT=1")
    set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZ4_DLL_EXPORT=1")
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DZSTD_DLL_EXPORT=1")
    set_property(SOURCE ${BZIP2_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DBZIP2_DLL_EXPORT=1")
endif()
elseif (UNIX)
set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_FLAGS "-msse4.1 -mavx -mbmi2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} -fomit-frame-pointer -fstrict-aliasing -ffast-math -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} -fomit-frame-pointer -fstrict-aliasing -ffast-math -fopenmp")
set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1")
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
    endif()
endif()
if (SNAPPY_MATCH_SKIP_OPT)
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_SNAPPY_MATCH_SKIP_OPT")
endif()
if (AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS)
    set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS")
endif()
if (AOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST)
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST")
endif()
if (AOCL_ZSTD_WILDCOPY_LONG)
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ZSTD_WILDCOPY_LONG")
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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ROOT_DIR}/lib)
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
        ${ZSTD_SRC_FILES}
        ${COMMON_SRC_FILES}
        ${THREADS_SRC_FILES}
)


#common flag for aocl_compression_bench and gtest_combined_test
set(AOCL_DYNAMIC_DISPATCHER ON)
if (AOCL_DYNAMIC_DISPATCHER)
    set_property(SOURCE ${SRC_FILES} ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
    option(ENABLE_STRICT_WARNINGS "Enable compiler flags to treat all warnings as errors (Enabled by default)" ON)
else()
    set(ENABLE_STRICT_WARNINGS OFF)
endif()

if (AOCL_ENABLE_THREADS)
    set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_THREADS")
endif()

if (AOCL_ENABLE_LOG_FEATURE)
	set_property(SOURCE ${SRC_FILES} ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_LOG_FEATURE")
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

# Enable CTest for any of the cases
if(VALGRIND OR ASAN OR CODE_COVERAGE)
    set(AOCL_TEST_COVERAGE ON)
endif()

#setup CTest for GoogleTest and aocl_compression_bench test
if (AOCL_TEST_COVERAGE)
    # enabling ctest
    enable_testing()

	set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_UNIT_TEST")

    add_subdirectory(gtest)
    add_subdirectory(test)
endif()

#Enabling strict warnings
if(ENABLE_STRICT_WARNINGS)
    if (WIN32)
        #suppress warnings suggesting to use fopen_s, ... in place of fopen, ...
        set_property(SOURCE ${SRC_FILES} ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-D_CRT_SECURE_NO_WARNINGS")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
        set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DENABLE_STRICT_WARNINGS")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic")
        set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DENABLE_STRICT_WARNINGS")
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

set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${DEST_LIB_PATH}")

if (AOCL_ENABLE_THREADS AND WIN32)
    #openmp support
    find_package(OpenMP)
    #include_directories(SYSTEM ${OpenMP_INCLUDE_PATH})
    if (OPENMP_FOUND)
        target_include_directories (${lib_name} INTERFACE $<INSTALL_INTERFACE:include>)
        target_link_libraries (${lib_name} ${CMAKE_THREAD_LIBS_INIT} PUBLIC OpenMP::OpenMP_C OpenMP::OpenMP_CXX)
        target_compile_options (${lib_name} PRIVATE "-openmp")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else ()
        unset(AOCL_ENABLE_THREADS)
        message(WARNING "OpenMP not found. Library is not built with multi-threaded support.")
    endif ()
endif ()

#add executable
set (exe_name aocl_compression_bench)

if (WIN32)
message(STATUS "C Compiler flags used for this windows build are: ${CMAKE_C_FLAGS}")
message(STATUS "C++ Compiler flags used for this windows build are: ${CMAKE_CXX_FLAGS}")
add_executable(${exe_name} ${TESTBENCH_SRC_FILES})
else ()
message(STATUS "C Compiler flags used for this Unix build are: ${CMAKE_C_FLAGS}")
message(STATUS "C++ Compiler flags used for this Unix build are: ${CMAKE_CXX_FLAGS}")
list(APPEND TESTBENCH_SRC_FILES ${BENCH_PATH}/ipp_codec_bench.c)
add_executable(${exe_name} ${TESTBENCH_SRC_FILES})
endif()

target_link_libraries(${exe_name} ${lib_name})
target_link_libraries(${exe_name} ${CMAKE_DL_LIBS})

#Install into destination path by using -DCMAKE_INSTALL_PREFIX=<path> while configuring cmake
message(STATUS "AOCL COMPRESSION Library, Interface header files and Test binary will be installed at ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${exe_name} DESTINATION ${DEST_BIN_PATH})
install(TARGETS ${lib_name} DESTINATION ${DEST_LIB_PATH})
install(FILES ${INC_FILES} DESTINATION ${DEST_INC_PATH})
if (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL)
    install(DIRECTORY ${INC_XZ_UTILS_LZMA} DESTINATION ${DEST_INC_PATH} FILES_MATCHING PATTERN "*.h")
endif()

#Create symbolic links from expected library names to aocl library
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
create_sym_link_to_lib(libbz2)
create_sym_link_to_lib(libz)
create_sym_link_to_lib(liblz4)
create_sym_link_to_lib(libsnappy)
create_sym_link_to_lib(liblzma)
create_sym_link_to_lib(libzstd)

if((NOT BUILD_STATIC_LIBS) AND WIN32 AND AOCL_TEST_COVERAGE)
    add_custom_command(
        TARGET ${lib_name}
        POST_BUILD
        COMMENT "Copy DLL to GTest executables path"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_name}> ${CMAKE_BINARY_DIR}/gtest
        )
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
    find_package(Doxygen 1.9.6)
    set(W_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")
    if(NOT (DOXYGEN_FOUND))
        message(STATUS "Doxygen not found please install doxygen and try again.")
    else()
        execute_process(COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
                WORKING_DIRECTORY ${W_DIR}
                COMMAND_ECHO STDOUT)
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/docs/html/index.html)
        message(STATUS "Documentation generated successfully, to view documentation open docs/html/index.html .")
    else()
        message(STATUS "Document generation failed.")
    endif()
endif()
