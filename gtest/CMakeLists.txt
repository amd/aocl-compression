# Copyright (C) 2023-2026, Advanced Micro Devices. All rights reserved.
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
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)
if (NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  # Avoid installing gtest libs/headers by default
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


if(AOCL_TEST_FUZZER)
    # Detect filesystem library requirements for older Clang when fuzzing is enabled.
    # Only check when using Clang < 14 with libstdc++ or libc++.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0)
        include(CheckCXXSourceCompiles)
        
        # Save current state
        set(_CMAKE_REQUIRED_LIBRARIES_SAVE ${CMAKE_REQUIRED_LIBRARIES})
        set(_CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
        set(_CMAKE_REQUIRED_QUIET_SAVE ${CMAKE_REQUIRED_QUIET})
        set(CMAKE_REQUIRED_QUIET TRUE)
        
        # Append C++17 flag to existing flags (portable approach)
        if(MSVC)
            set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} /std:c++17")
        else()
            set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++17")
        endif()
        
        # Test for stdc++fs (libstdc++)
        set(CMAKE_REQUIRED_LIBRARIES stdc++fs)
        check_cxx_source_compiles("
            #include <filesystem>
            int main() {
                std::filesystem::path p(\"/tmp\");
                std::filesystem::exists(p);
                return 0;
            }
        " HAVE_STDCXXFS)
        
        # Test for c++fs (libc++)
        set(CMAKE_REQUIRED_LIBRARIES c++fs)
        check_cxx_source_compiles("
            #include <filesystem>
            int main() {
                std::filesystem::path p(\"/tmp\");
                std::filesystem::exists(p);
                return 0;
            }
        " HAVE_CXXFS)
        
        # Restore state
        set(CMAKE_REQUIRED_LIBRARIES ${_CMAKE_REQUIRED_LIBRARIES_SAVE})
        set(CMAKE_REQUIRED_FLAGS ${_CMAKE_REQUIRED_FLAGS_SAVE})
        set(CMAKE_REQUIRED_QUIET ${_CMAKE_REQUIRED_QUIET_SAVE})
        unset(_CMAKE_REQUIRED_LIBRARIES_SAVE)
        unset(_CMAKE_REQUIRED_FLAGS_SAVE)
        unset(_CMAKE_REQUIRED_QUIET_SAVE)
    endif()
    
    FetchContent_Declare(
        googlefuzztest
        QUIET
        URL https://github.com/google/fuzztest/archive/6f4caf7abdd19a7f3b611789fa92a6be05b6c8f9.zip #commit 6f4caf7
    )

    FetchContent_GetProperties(googlefuzztest)
    if (NOT googlefuzztest_POPULATED)
    FetchContent_Populate(googlefuzztest)
    # Avoid installing gtest libs/headers by default
    add_subdirectory(${googlefuzztest_SOURCE_DIR} ${googlefuzztest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    fuzztest_setup_fuzzing_flags()
endif()

set (GTEST_SRC_PATH ${AOCL_COMPRESSION_ROOT}/gtest)
set (GTEST_UTILS_SRC_FILES ${GTEST_SRC_PATH}/utils/utils_gtest.cpp)
set (GTEST_API_SRC_FILES ${GTEST_SRC_PATH}/api/api_gtest.cpp)
set (GTEST_COMMON_SRC_FILES ${GTEST_SRC_PATH}/common/common_gtest.cpp)
set (GTEST_UTILITY_SRC_FILES ${GTEST_SRC_PATH}/gtest_utils.cpp)
if (AOCL_ENABLE_THREADS)
    set (GTEST_THREADS_SRC_FILES ${GTEST_SRC_PATH}/threads/threads_gtest.cpp)
endif()
if (NOT AOCL_EXCLUDE_BZIP2)
    set (GTEST_BZIP2_SRC_FILES ${GTEST_SRC_PATH}/bzip2/bzip2_gtest.cpp)
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
    set (GTEST_LZ4_SRC_FILES ${GTEST_SRC_PATH}/lz4/lz4_gtest.cpp)
    if (AOCL_LZ4_HASH_BITS_USED)
        if (AOCL_LZ4_HASH_BITS_USED  STREQUAL "LOW")
            set_property(SOURCE ${GTEST_LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_HASH_BITS_USED=41")
        elseif (AOCL_LZ4_HASH_BITS_USED  STREQUAL "HIGH")
            set_property(SOURCE ${GTEST_LZ4_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_LZ4_HASH_BITS_USED=44")
        endif()
    endif()
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC)
    set (GTEST_LZ4HC_SRC_FILES ${GTEST_SRC_PATH}/lz4hc/lz4hc_gtest.cpp)
endif ()
if (NOT AOCL_EXCLUDE_LZMA)
    set (GTEST_LZMA_SRC_FILES ${GTEST_SRC_PATH}/lzma/lzma_gtest.cpp)
    if (AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL)
        set_property(SOURCE ${GTEST_LZMA_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL")
    endif()
endif ()
if (NOT AOCL_EXCLUDE_SNAPPY)
    set (GTEST_SNAPPY_SRC_FILES ${GTEST_SRC_PATH}/snappy/snappy_gtest.cpp)
    if (${SNAPPY_HAVE_SYS_UIO_H})
        set_property(SOURCE ${GTEST_SNAPPY_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DHAVE_SYS_UIO_H")
    endif()
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
    file (GLOB GTEST_ZLIB_SRC_FILES "${GTEST_SRC_PATH}/zlib/*.cpp")
endif ()
if (NOT AOCL_EXCLUDE_ZSTD)
    file (GLOB GTEST_ZSTD_SRC_FILES "${GTEST_SRC_PATH}/zstd/*.cpp")
endif ()

include(GoogleTest)
#create executable for each of the directories (module) in gtest/
#expect source files to be in ${GTEST_<MODULE>_SRC_FILES}
file(GLOB GTEST_MODULE_DIRS LIST_DIRECTORIES true ${GTEST_SRC_PATH}/*)
foreach(FOLDER ${GTEST_MODULE_DIRS})
    if (IS_DIRECTORY ${FOLDER}) #add 1 executable per directory
        get_filename_component(GTEST_NAME ${FOLDER} NAME)
        set(GTEST_CUR_EXE_NAME ${GTEST_NAME}_gtest)
        string(TOUPPER ${GTEST_NAME} GTEST_NAME)
        set(GTEST_CUR_SRC_FILES ${GTEST_${GTEST_NAME}_SRC_FILES})
        if (NOT ("${GTEST_CUR_SRC_FILES}" STREQUAL "")) # ignore skipped tests. src files are selectively set based on cmake options.
            
            # set properties for current source files
            set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_UNIT_TEST")
            if (AOCL_DYNAMIC_DISPATCHER)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
            endif()
            if (AOCL_ENABLE_THREADS)
	            set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_THREADS")
            endif()
            if (AOCL_DECOMPRESS_FAST)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DECOMPRESS_FAST=${AOCL_DECOMPRESS_FAST}")
            endif()
            if (AOCL_COMPRESS_FAST)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_COMPRESS_FAST=${AOCL_COMPRESS_FAST}")
            endif()
            if (AOCL_ENABLE_LOG_FEATURE)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_LOG_FEATURE")
            endif()
            if (AOCL_EXCLUDE_BZIP2)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_BZIP2")
            endif()
            if (AOCL_EXCLUDE_LZ4)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4")
            endif()
            if (AOCL_EXCLUDE_LZ4HC)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4HC")
            endif()
            if (AOCL_EXCLUDE_LZMA)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZMA")
            endif()
            if (AOCL_EXCLUDE_SNAPPY)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_SNAPPY")
            endif()
            if (AOCL_EXCLUDE_ZLIB)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
            endif()
            if (AOCL_EXCLUDE_ZSTD)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZSTD")
            # else()
            #     set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DDYNAMIC_BMI2=1")
            endif()
            if(AOCL_TEST_FUZZER_WITH_CORPUS)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_TEST_FUZZER_WITH_CORPUS")
            endif()
            if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DDEBUG_ASSERT_ENABLED")
            endif()

            if(SYSTEM_AVAILABLE_MEMORY_MB GREATER_EQUAL AOCL_MIN_MEMORY_INTENSIVE_TEST_MB)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_MEM_INT_TEST")
            endif()

            # Non API function tests
            if (BUILD_STATIC_LIBS)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_INTERNAL_TEST")
            endif()
            # add target
            add_executable(${GTEST_CUR_EXE_NAME} ${GTEST_CUR_SRC_FILES} ${GTEST_UTILITY_SRC_FILES})
            set_target_properties(${GTEST_CUR_EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
            target_include_directories(${GTEST_CUR_EXE_NAME} PRIVATE ${AOCL_COMPRESSION_ROOT}
                                                PRIVATE ${AOCL_COMPRESSION_ROOT}/${ALGOS_PATH}/common
                                                PRIVATE ${AOCL_COMPRESSION_ROOT}/api
                                                PRIVATE ${AOCL_GENERATED_COMMON_INC_DIR}
                                                PRIVATE ${GTEST_SRC_PATH})
            if(AOCL_TEST_FUZZER)
                set_property(SOURCE ${GTEST_CUR_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_TEST_FUZZER")
                target_link_libraries(${GTEST_CUR_EXE_NAME} PRIVATE ${lib_name})
                link_fuzztest(${GTEST_CUR_EXE_NAME})
                # Older Clang with some libstdc++/libc++ setups requires explicit
                # linking of a separate filesystem library (stdc++fs or c++fs).
                # Only link it when the compiler version suggests it *may* be needed
                # and when the corresponding library is actually available.
                if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0)
                    if(HAVE_STDCXXFS)
                        target_link_libraries(${GTEST_CUR_EXE_NAME} PRIVATE stdc++fs)
                    elseif(HAVE_CXXFS)
                        target_link_libraries(${GTEST_CUR_EXE_NAME} PRIVATE c++fs)
                    endif()
                endif()
            else()
                target_link_libraries(${GTEST_CUR_EXE_NAME} ${lib_name} GTest::gtest_main)
            endif()
            gtest_discover_tests(${GTEST_CUR_EXE_NAME})
        endif()
    endif()
endforeach()
