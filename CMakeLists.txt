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
project(AOCL-COMPRESSION-LIBRARY)
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

option (BUILD_STATIC_LIBS "Build static library (Default build type is shared library)" OFF)
option (AOCL_LZ4_OPT_PREFETCH_BACKWARDS "Enable LZ4 optimizations related to backward prefetching of data" OFF)
option (SNAPPY_MATCH_SKIP_OPT "Enable Snappy match skipping optimization (Disabled by default)" OFF)
option (LZ4_FRAME_FORMAT_SUPPORT "Enable building LZ4 with Frame format and API support (Disabled by default)" OFF)
option (AOCL_ZSTD_4BYTE_LAZY2_MATCH_FINDER "Enable 4-byte comparison when finding match candidates with Lazy2 compressor (Disabled by default)" OFF)
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
option (AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS "Disable Pattern Analysis in LZ4HC for level 9 (Enabled by default)" ON)
option(GTEST_ENABLE "Enable gtest based test suite (Disabled by default)" OFF)
option(BUILD_DOC "Build documentation for aocl-compression (Disabled by default)" OFF)
option(ZLIB_DEFLATE_FAST_MODE_2 "Enable optimization for deflate fast using Z_FIXED strategy (Disabled by default)" OFF)
option(ZLIB_DEFLATE_FAST_MODE_3 "Enable ZLIB deflate quick strategy (Disabled by default)" OFF)

if(ZLIB_DEFLATE_FAST_MODE_2 AND ZLIB_DEFLATE_FAST_MODE_3)
	message(FATAL_ERROR "You can not enable ZLIB_DEFLATE_FAST_MODE_2 and ZLIB_DEFLATE_FAST_MODE_3 together.")
endif()

#list source files to be compiled for the library
file(GLOB API_SRC_FILES api/*.c api/*.cpp api/*.h)
set (ALGOS_PATH algos)
set (BZIP2_SRC_FILES 	${ALGOS_PATH}/bzip2/blocksort.c
						${ALGOS_PATH}/bzip2/huffman.c
						${ALGOS_PATH}/bzip2/crctable.c
						${ALGOS_PATH}/bzip2/randtable.c
						${ALGOS_PATH}/bzip2/compress.c
						${ALGOS_PATH}/bzip2/decompress.c
						${ALGOS_PATH}/bzip2/bzlib.c
						${ALGOS_PATH}/bzip2/bzlib.h)
set (LZ4_SRC_FILES	${ALGOS_PATH}/lz4/lz4.c
					${ALGOS_PATH}/lz4/lz4.h)
set (LZ4HC_SRC_FILES	${ALGOS_PATH}/lz4/lz4hc.c
						${ALGOS_PATH}/lz4/lz4hc.h)
set (LZMA_SRC_FILES	${ALGOS_PATH}/lzma/LzFind.c
					${ALGOS_PATH}/lzma/LzmaDec.c
					${ALGOS_PATH}/lzma/LzmaEnc.c
					${ALGOS_PATH}/lzma/Alloc.c
					${ALGOS_PATH}/lzma/Alloc.h
					${ALGOS_PATH}/lzma/LzmaDec.h
					${ALGOS_PATH}/lzma/LzmaEnc.h
					${ALGOS_PATH}/lzma/LzFind.h)
set (SNAPPY_SRC_FILES 	${ALGOS_PATH}/snappy/snappy-sinksource.cc
						${ALGOS_PATH}/snappy/snappy-stubs-internal.cc
						${ALGOS_PATH}/snappy/snappy.cc
						${ALGOS_PATH}/snappy/snappy.h)
set (ZLIB_SRC_FILES	${ALGOS_PATH}/zlib/adler32.c
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
set (ZSTD_SRC_FILES	${ALGOS_PATH}/zstd/lib/common/zstd_common.c
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
					${ALGOS_PATH}/zstd/lib/decompress/zstd_ddict.c
					${ALGOS_PATH}/zstd/lib/decompress/zstd_decompress_block.c
					${ALGOS_PATH}/zstd/lib/dictBuilder/cover.c
					${ALGOS_PATH}/zstd/lib/dictBuilder/divsufsort.c
					${ALGOS_PATH}/zstd/lib/dictBuilder/fastcover.c
					${ALGOS_PATH}/zstd/lib/dictBuilder/zdict.c
					${ALGOS_PATH}/zstd/lib/zstd.h)
file(GLOB UTILS_SRC_FILES utils/*.c utils/*.cpp utils/*.h)

#set the expected path variables
include_directories(.)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${ROOT_DIR}/build/)
set(DEST_LIB_PATH aocl_compression/lib)
set(DEST_INC_PATH aocl_compression/include)
set(DEST_BIN_PATH aocl_compression/bin)

set(INC_FILES api/api.h 
	${ALGOS_PATH}/bzip2/bzlib.h
	${ALGOS_PATH}/lz4/lz4.h
	${ALGOS_PATH}/lz4/lz4hc.h
	${ALGOS_PATH}/lzma/Alloc.h
	${ALGOS_PATH}/lzma/LzmaDec.h
	${ALGOS_PATH}/lzma/LzmaEnc.h 
	${ALGOS_PATH}/snappy/snappy.h
	${ALGOS_PATH}/zlib/zlib.h
	${ALGOS_PATH}/zstd/lib/zstd.h
    )

if (LZ4_FRAME_FORMAT_SUPPORT)
    set(INC_FILES ${INC_FILES}
                  ${ALGOS_PATH}/lz4/lz4frame.h)
    set (LZ4_SRC_FILES ${LZ4_SRC_FILES}
                       ${ALGOS_PATH}/lz4/lz4frame.c
                       ${ALGOS_PATH}/lz4/lz4frame.h)
    if (AOCL_EXCLUDE_LZ4HC)
        set (LZ4_SRC_FILES ${LZ4_SRC_FILES}
                           ${ALGOS_PATH}/lz4/lz4hc.c
                           ${ALGOS_PATH}/lz4/lz4hc.h)
    endif()
    if (AOCL_EXCLUDE_ZSTD)
        set (LZ4_SRC_FILES ${LZ4_SRC_FILES}
                           ${ALGOS_PATH}/lz4/xxhash.c
                           ${ALGOS_PATH}/lz4/xxhash.h)
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
endif()

#set(USER_DEBUG_ON "-g")
message(STATUS "C Compiler used: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler used: ${CMAKE_CXX_COMPILER}")
if(ZLIB_DEFLATE_FAST_MODE_3)
set_source_files_properties(${ZLIB_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1;-DAOCL_ZLIB_DEFLATE_FAST_MODE_3=1")
elseif(ZLIB_DEFLATE_FAST_MODE_2)
set_source_files_properties(${ZLIB_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1;-DAOCL_ZLIB_DEFLATE_FAST_MODE_2=1")
else()
set_source_files_properties(${ZLIB_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1")
endif()
if (WIN32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} /O2 /DNDEBUG /fp:fast /Oy /arch:SSE2 /arch:AVX /arch:AVX2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} /O2 /DNDEBUG /fp:fast /Oy /arch:SSE2 /arch:AVX /arch:AVX2")
set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_EXPECT=1")
elseif (UNIX)
set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_FLAGS "-msse4.1 -mavx -mbmi2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USER_DEBUG_ON} -fomit-frame-pointer -fstrict-aliasing -ffast-math")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_DEBUG_ON} -fomit-frame-pointer -fstrict-aliasing -ffast-math")
set_source_files_properties(${SNAPPY_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DHAVE_BUILTIN_CTZ=1;-DHAVE_BUILTIN_EXPECT=1")
endif()

if (AOCL_LZ4_OPT_PREFETCH_BACKWARDS)
    set_source_files_properties(${LZ4_SRC_FILES} PROPERTIES COMPILE_OPTIONS "-DAOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS")
endif()
if (SNAPPY_MATCH_SKIP_OPT)
    set_property(SOURCE ${SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_SNAPPY_MATCH_SKIP_OPT")
endif()
if (AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS)
    set_property(SOURCE ${LZ4HC_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS")
endif()
if (AOCL_ZSTD_4BYTE_LAZY2_MATCH_FINDER)
    set_property(SOURCE ${ZSTD_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ZSTD_4BYTE_LAZY2_MATCH_FINDER")
endif()

add_compile_definitions(CCompiler="${CMAKE_C_COMPILER}")
add_compile_definitions(CXXCompiler="${CMAKE_CXX_COMPILER}")
add_compile_definitions(CFLAGS_SET2="${CMAKE_C_FLAGS}")

#set library and sources
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ROOT_DIR}/lib)
set (lib_name aocl_compression)
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
	)
set (BENCH_PATH test)

#common flag for aocl_compression_bench and gtest_combined_test
set(AOCL_DYNAMIC_DISPATCHER ON)
if (AOCL_DYNAMIC_DISPATCHER)
	set_property(SOURCE ${SRC_FILES} ${BENCH_PATH}/codec_bench.c APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
	option(ENABLE_STRICT_WARNINGS "Enable compiler flags to treat all warnings as errors (Enabled by default)" ON)
else()
	set(ENABLE_STRICT_WARNINGS OFF)
endif()

#setup gtest and build test executable
if (GTEST_ENABLE)
	include(FetchContent)
	FetchContent_Declare(
	    googletest
	    # Specify the commit you depend on and update it regularly.
	    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_GetProperties(googletest)
	if(NOT googletest_POPULATED)
  		FetchContent_Populate(googletest)
  		add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
	endif()
        if(NOT BUILD_STATIC_LIBS AND WIN32)
		set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZ4_DLL_EXPORT=1")
		set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DSNAPPY_DLL_EXPORT=1")
		set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DZLIB_DLL=1")
		set_property(SOURCE ${SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DLZMA_DLL_EXPORT=1")
        endif()
	set_property(SOURCE ${LZMA_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZMA_UNIT_TEST")

	# gtest_combined_test is an executable generated inside build folder
	set (GTEST_PATH gtest)
	set(GTEST_SRC_FILES 
        ${GTEST_PATH}/lz4/lz4_gtest.cc
		${GTEST_PATH}/lzma/lzma_gtest.cc
		${GTEST_PATH}/snappy/snappy_gtest.cc
		${GTEST_PATH}/zlib/zlib_gtest.cc
		${GTEST_PATH}/gtest_combined_test.cc
	)
	set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZMA_UNIT_TEST")
	if (AOCL_DYNAMIC_DISPATCHER)
		set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
	endif()

	add_executable(gtest_combined_test ${GTEST_SRC_FILES})
	target_link_libraries(gtest_combined_test gtest_main ${lib_name})
        string(REPLACE " " ";" CXX_FLAGS ${CMAKE_CXX_FLAGS})
        target_compile_options(gtest_combined_test PRIVATE ${CXX_FLAGS})
endif()

#Enabling strict warnings
if(ENABLE_STRICT_WARNINGS)
	if (WIN32)
		#suppress warnings suggesting to use fopen_s, ... in place of fopen, ...
		set_property(SOURCE ${SRC_FILES} ${BENCH_PATH}/codec_bench.c APPEND PROPERTY COMPILE_OPTIONS "-D_CRT_SECURE_NO_WARNINGS")
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

set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${DEST_LIB_PATH}")

#add executable
set (exe_name aocl_compression_bench)
if (WIN32)
message(STATUS "C Compiler flags used for this windows build are: ${CMAKE_C_FLAGS}")
message(STATUS "C++ Compiler flags used for this windows build are: ${CMAKE_CXX_FLAGS}")
add_executable(${exe_name} ${BENCH_PATH}/codec_bench.c ${BENCH_PATH}/codec_bench.h)
else ()
message(STATUS "C Compiler flags used for this Unix build are: ${CMAKE_C_FLAGS}")
message(STATUS "C++ Compiler flags used for this Unix build are: ${CMAKE_CXX_FLAGS}")
add_executable(${exe_name} ${BENCH_PATH}/codec_bench.c ${BENCH_PATH}/ipp_codec_bench.c ${BENCH_PATH}/codec_bench.h)
endif()
target_link_libraries(${exe_name} ${lib_name})
target_link_libraries(${exe_name} ${CMAKE_DL_LIBS})

#Install into destination path by using -DCMAKE_INSTALL_PREFIX=<path> while configuring cmake
message(STATUS "AOCL COMPRESSION Library, Interface header files and Test binary will be installed at ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${exe_name} DESTINATION ${DEST_BIN_PATH})
install(TARGETS ${lib_name} DESTINATION ${DEST_LIB_PATH})
install(FILES ${INC_FILES} DESTINATION ${DEST_INC_PATH})

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
		if (WIN32)
			set(DOXYGEN "${CMAKE_CURRENT_SOURCE_DIR}/build/doxygen-1.9.6/doxygen.exe")
			if(NOT (EXISTS ${DOXYGEN}))
				message(STATUS "so downloading doxygen executables")
				file(DOWNLOAD https://github.com/doxygen/doxygen/releases/download/Release_1_9_6/doxygen-1.9.6.windows.x64.bin.zip ${CMAKE_CURRENT_SOURCE_DIR}/build/doxygen.zip
						SHOW_PROGRESS )
				execute_process(COMMAND powershell Expand-Archive ./doxygen.zip ./doxygen-1.9.6 -Force
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/)
				file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/build/doxygen.zip)
			endif()
		elseif (UNIX)
			set(DOXYGEN "${CMAKE_CURRENT_SOURCE_DIR}/build/doxygen-1.9.6/bin/doxygen")
			if(NOT (EXISTS ${DOXYGEN}))
				message(STATUS "so downloading doxygen executables")
				file(DOWNLOAD https://github.com/doxygen/doxygen/releases/download/Release_1_9_6/doxygen-1.9.6.linux.bin.tar.gz ${CMAKE_CURRENT_SOURCE_DIR}/build/doxygen.gz
						SHOW_PROGRESS )
				execute_process(COMMAND tar -xf doxygen.gz
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
				file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/build/doxygen.gz)
			endif()
		endif()
		execute_process(COMMAND "${DOXYGEN}" Doxyfile
				WORKING_DIRECTORY ${W_DIR})
	else()
		execute_process(COMMAND doxygen Doxyfile
				WORKING_DIRECTORY ${W_DIR}
				COMMAND_ECHO STDOUT)
	endif()
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/docs/html/index.html)
		message(STATUS "Documentation generated successfully, to view documentation open docs/html/index.html .")
	else()
		message(STATUS "Document generation failed.")
	endif()
endif()
