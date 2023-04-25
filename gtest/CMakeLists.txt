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
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.13.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  # Avoid installing gtest libs/headers by default
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

if (NOT AOCL_EXCLUDE_LZ4)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/lz4/lz4_gtest.cc)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4")
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC AND NOT AOCL_EXCLUDE_LZ4)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/lz4hc/lz4hc_gtest.cc)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4HC")
endif ()
if (NOT AOCL_EXCLUDE_LZMA)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/lzma/lzma_gtest.cc)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZMA")
endif ()
if (NOT AOCL_EXCLUDE_SNAPPY)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/snappy/snappy_gtest.cc)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_SNAPPY")
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
    set (GTEST_SRC_FILES ${GTEST_SRC_FILES} ${CMAKE_SOURCE_DIR}/gtest/zlib/zlib_gtest.cc)
else ()
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
endif ()

set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZMA_UNIT_TEST")
set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4HC_UNIT_TEST")
set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ZLIB_UNIT_TEST")
if (AOCL_DYNAMIC_DISPATCHER)
    set_property(SOURCE ${GTEST_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
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
