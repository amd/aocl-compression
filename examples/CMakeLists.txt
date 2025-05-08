# Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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
# @brief CMakeLists.txt to build example programs for AOCL-Compression
#
# This file can be used as the top-level CMakeLists.txt to build examples 
# shared as part of AOCL-Compression package
#
# @author Ashish Sriram

cmake_minimum_required(VERSION 3.26.0)

# Use out-of-src-tree build if building directly from AOCL-Compression package
if(NOT DEFINED ENABLE_EXAMPLE_OUTOFSRCTREE)
    SET(ENABLE_EXAMPLE_OUTOFSRCTREE ON)
endif()

if(ENABLE_EXAMPLE_OUTOFSRCTREE)
    # If building from AOCL-Compression package
    project(AOCL-COMPRESSION-EXAMPLES CXX C)
    message(STATUS "Building for AOCL COMPRESSION EXAMPLES")
    # Default values for include and lib directories are relative to examples folder in AOCL-Compression package
    set(AOCL_COMPRESSION_INC_DIR "${CMAKE_SOURCE_DIR}/../include" CACHE STRING "include directory path of AOCL Compression library")
    set(AOCL_COMPRESSION_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib" CACHE STRING "lib directory path of AOCL Compression library")
    set(lib_name aocl_compression)

    # Create example_name executable
    FUNCTION(add_example_outofsource example_name example_source)
        add_executable(${example_name} ${example_source})
        target_include_directories(${example_name} PRIVATE ${AOCL_COMPRESSION_INC_DIR})
        target_link_directories(${example_name} PRIVATE ${AOCL_COMPRESSION_LIB_DIR})
        target_link_libraries(${example_name} ${lib_name})
        target_link_libraries(${example_name} ${CMAKE_DL_LIBS})
        message(STATUS "Example added out-of-src-tree : ${example_name}")
    ENDFUNCTION()

    # Build all example programs starting with example_*
    file(GLOB_RECURSE EXAMPLE_FILES "example_*.c*")
    foreach(example_path ${EXAMPLE_FILES})
        get_filename_component(example_name ${example_path} NAME_WE)
        add_example_outofsource(${example_name} ${example_path})
    endforeach()
else()
    # If building alongside AOCL-Compression library source code
    set(EXAMPLE_INC_FILES ${INC_FILES})
    set(AOCL_COMPRESSION_INC_DIR ${CMAKE_BINARY_DIR}/include_examples)
    list(TRANSFORM EXAMPLE_INC_FILES PREPEND ${CMAKE_SOURCE_DIR}/)
    add_custom_command(
        OUTPUT ${AOCL_COMPRESSION_INC_DIR}
        COMMENT "Copy interface headers to build path"
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${AOCL_COMPRESSION_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${AOCL_COMPRESSION_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${EXAMPLE_INC_FILES} ${AOCL_COMPRESSION_INC_DIR} 
        DEPENDS ${EXAMPLE_INC_FILES})
    # Ensure include folder is copied before building executables
    add_custom_target(copy_include_examples DEPENDS ${AOCL_COMPRESSION_INC_DIR})

    # Create example_name executable
    FUNCTION(add_example example_name example_source)
        if(ENABLE_STRICT_WARNINGS)
            if (WIN32)
                #suppress warnings suggesting to use fopen_s, ... in place of fopen, ...
                set_property(SOURCE ${example_source} APPEND PROPERTY COMPILE_OPTIONS "-D_CRT_SECURE_NO_WARNINGS")
            endif()
        endif()
        add_executable(${example_name} ${example_source})
        add_dependencies(${example_name} copy_include_examples)
        set_target_properties(${example_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
        target_include_directories(${example_name} PRIVATE ${AOCL_COMPRESSION_INC_DIR})
        target_link_libraries(${example_name} ${lib_name})
        target_link_libraries(${example_name} ${CMAKE_DL_LIBS})
        message(STATUS "Example added : ${example_name}")
    ENDFUNCTION()

    # Build all example programs starting with example_*
    FUNCTION(add_examples_dir example_dir)
        file(GLOB EXAMPLE_FILES "${example_dir}/example_*.c*")
        foreach(example_path ${EXAMPLE_FILES})
            get_filename_component(example_name ${example_path} NAME_WE)
            add_example(${example_name} ${example_path})
        endforeach()
    ENDFUNCTION()

    if (NOT AOCL_EXCLUDE_BZIP2)
        add_examples_dir(bzip2)
    endif ()
    if (NOT AOCL_EXCLUDE_LZ4)
        add_examples_dir(lz4)
    endif ()
    if (NOT AOCL_EXCLUDE_LZ4HC)
        add_examples_dir(lz4hc)
    endif ()
    if (NOT AOCL_EXCLUDE_LZMA)
        add_examples_dir(lzma)
    endif ()
    if (NOT AOCL_EXCLUDE_SNAPPY)
        add_examples_dir(snappy)
    endif ()
    if (NOT AOCL_EXCLUDE_ZLIB)
        add_example(example_compress2 zlib/example_compress2.c)
        if (AOCL_ENABLE_THREADS)
            add_example(example_compress2_gzip zlib/example_compress2_gzip.c)
            add_example(example_compress2_raw zlib/example_compress2_raw.c)
        endif()
    endif ()
    if (NOT AOCL_EXCLUDE_ZSTD)
        add_examples_dir(zstd)
    endif ()

    add_example(example_unified_api api/example_unified_api.c)
    if (AOCL_ENABLE_THREADS)
        add_example(example_aocl_llc_skip_rap_frame api/example_aocl_llc_skip_rap_frame.c)
    endif()
endif()
