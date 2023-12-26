# Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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
 
# @author Ravi Jangra

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
  # Avoid installing gtest libs/headers by default
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/utils/utils_gtest.cpp)
set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/api/api_gtest.cpp)
set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/common/common_gtest.cpp)
if (AOCL_ENABLE_THREADS)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/threads/threads_gtest.cpp)
endif()
if (NOT AOCL_EXCLUDE_BZIP2)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/bzip2/bzip2_gtest.cpp)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_BZIP2")
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/lz4/lz4_gtest.cpp)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4")
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC AND NOT AOCL_EXCLUDE_LZ4)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/lz4hc/lz4hc_gtest.cpp)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4HC")
endif ()
if (NOT AOCL_EXCLUDE_LZMA)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/lzma/lzma_gtest.cpp)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZMA")
endif ()
if (NOT AOCL_EXCLUDE_SNAPPY)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/snappy/snappy_gtest.cpp)
    if (${SNAPPY_HAVE_SYS_UIO_H})
        set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DHAVE_SYS_UIO_H")
    endif()
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_SNAPPY")
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/zlib/zlib_gtest.cpp)
    if(ZLIB_DEFLATE_FAST_MODE)
        set_property(SOURCE ${CMAKE_SOURCE_DIR}/gtest/zlib/zlib_gtest.cpp APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ZLIB_DEFLATE_FAST_MODE=1")
    endif()
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
endif ()
if (NOT AOCL_EXCLUDE_ZSTD)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/zstd/zstd_gtest.cpp)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZSTD")
endif ()

set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_UNIT_TEST")
if (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL)
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL")
endif()

if (AOCL_DYNAMIC_DISPATCHER)
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
endif()

if (AOCL_ENABLE_THREADS)
	set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_THREADS")
endif()

if (AOCL_LZ4_HASH_BITS_USED)
	if (AOCL_LZ4_HASH_BITS_USED  STREQUAL "LOW")
		set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_HASH_BITS_USED=41")
	elseif(AOCL_LZ4_HASH_BITS_USED  STREQUAL "HIGH")
		set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_HASH_BITS_USED=44")
	endif()
endif()

if (AOCL_ENABLE_LOG_FEATURE)
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_LOG_FEATURE")
endif()

foreach(FILE ${GTEST_SRC_FILES})
    string(REGEX MATCH "[A-Za-z0-9]*_gtest" TNAME ${FILE})
    add_executable(${TNAME} ${FILE})
    target_link_libraries(${TNAME} ${lib_name} GTest::gtest_main)
endforeach()


include(GoogleTest)
foreach(FILE ${GTEST_SRC_FILES})
    string(REGEX MATCH "[A-Za-z0-9]*_gtest" TNAME ${FILE})
    gtest_discover_tests(${TNAME})
endforeach()
