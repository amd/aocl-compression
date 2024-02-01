# Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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

set (TESTBENCH_SRC_FILES 
                codec_bench.c 
                codec_native_api_bench.c 
                codec_bench.h)

if (NOT AOCL_EXCLUDE_BZIP2)
    set (METHODS ${METHODS} bzip2)
else ()
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_BZIP2")
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
    set (METHODS ${METHODS} lz4)
else ()
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4")
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC AND NOT AOCL_EXCLUDE_LZ4)
    set (METHODS ${METHODS} lz4hc)
else ()
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZ4HC")
endif ()
if (NOT AOCL_EXCLUDE_LZMA)
    set (METHODS ${METHODS} lzma)
else ()
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_LZMA")
endif ()
if (NOT AOCL_EXCLUDE_SNAPPY)
    set (METHODS ${METHODS} snappy)
else ()
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_SNAPPY")
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
    set (METHODS ${METHODS} zlib)
else ()
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
endif ()
if (NOT AOCL_EXCLUDE_ZSTD)
    set (METHODS ${METHODS} zstd)
else ()
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZSTD")
endif ()

if (AOCL_DYNAMIC_DISPATCHER)
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DYNAMIC_DISPATCHER")
    option(ENABLE_STRICT_WARNINGS "Enable compiler flags to treat all warnings as errors (Enabled by default)" ON)
endif()

if (AOCL_ENABLE_THREADS)
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_THREADS")
endif()

if (NATIVE_ENABLE_THREADS)
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DNATIVE_ENABLE_THREADS")
endif()

#Enabling strict warnings
if(ENABLE_STRICT_WARNINGS)
    if (WIN32)
        #suppress warnings suggesting to use fopen_s, ... in place of fopen, ...
        set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-D_CRT_SECURE_NO_WARNINGS")
    endif()
endif()


set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${DEST_LIB_PATH}")

set(INTERNAL_INC_FILES  ${INC_FILES}
                        api/types.h
                        utils/utils.h)
list(TRANSFORM INTERNAL_INC_FILES PREPEND ${CMAKE_SOURCE_DIR}/)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/include/types.h
    COMMENT "Copy interface headers to build path"
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/include
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include
    COMMAND ${CMAKE_COMMAND} -E copy ${INTERNAL_INC_FILES} ${CMAKE_BINARY_DIR}/include 
    DEPENDS ${TESTBENCH_SRC_FILES} ${INTERNAL_INC_FILES})

#add executable
set (exe_name aocl_compression_bench)

if (WIN32)
message(STATUS "C Compiler flags used for this windows build are: ${CMAKE_C_FLAGS}")
message(STATUS "C++ Compiler flags used for this windows build are: ${CMAKE_CXX_FLAGS}")
add_executable(${exe_name} ${TESTBENCH_SRC_FILES} ${CMAKE_BINARY_DIR}/include/types.h)
else ()
message(STATUS "C Compiler flags used for this Unix build are: ${CMAKE_C_FLAGS}")
message(STATUS "C++ Compiler flags used for this Unix build are: ${CMAKE_CXX_FLAGS}")
list(APPEND TESTBENCH_SRC_FILES ipp_codec_bench.c)
add_executable(${exe_name} ${TESTBENCH_SRC_FILES} ${CMAKE_BINARY_DIR}/include/types.h)
endif()

set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
target_include_directories(${exe_name} PRIVATE ${CMAKE_BINARY_DIR}/include)

if (AOCL_ENABLE_LOG_FEATURE)
    set_property(SOURCE ${TESTBENCH_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_LOG_FEATURE")
endif()

target_link_libraries(${exe_name} ${lib_name})
target_link_libraries(${exe_name} ${CMAKE_DL_LIBS})

message(STATUS "Test binary will be installed at ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${exe_name} DESTINATION ${DEST_BIN_PATH})

#download datasets
if (AOCL_TEST_COVERAGE OR TEST_COVERAGE_THIRD_PARTY)
    set(CALGARY "${CMAKE_BINARY_DIR}/datasets/calgary.zip")
    if(NOT (EXISTS ${CALGARY}))
        message(STATUS "Downloading Calgary dataset ...")
        file(DOWNLOAD http://corpus.canterbury.ac.nz/resources/calgary.zip ${CMAKE_BINARY_DIR}/datasets/calgary.zip SHOW_PROGRESS)
        message(STATUS "Extracting Calgary dataset ...")
        file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/datasets/calgary.zip DESTINATION ${CMAKE_BINARY_DIR}/calgary)
    endif()

    set(CANTERBURY "${CMAKE_BINARY_DIR}/datasets/cantrbry.zip")
    if(NOT (EXISTS ${CANTERBURY}))
        message(STATUS "Downloading Canterbury dataset ...")
        file(DOWNLOAD http://corpus.canterbury.ac.nz/resources/cantrbry.zip ${CMAKE_BINARY_DIR}/datasets/cantrbry.zip SHOW_PROGRESS)
        message(STATUS "Extracting Canterbury dataset ...")
        file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/datasets/cantrbry.zip DESTINATION ${CMAKE_BINARY_DIR}/canterbury)
    endif()

    set(SILESIA "${CMAKE_BINARY_DIR}/datasets/silesia.zip")
    if(NOT (EXISTS ${SILESIA}))
        message(STATUS "Downloading Silesia dataset ...")
        file(DOWNLOAD http://sun.aei.polsl.pl/~sdeor/corpus/silesia.zip ${CMAKE_BINARY_DIR}/datasets/silesia.zip SHOW_PROGRESS)
        message(STATUS "Extracting Silesia dataset ...")
        file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/datasets/silesia.zip DESTINATION ${CMAKE_BINARY_DIR}/silesia)
    endif()

    # only run if code coverage is enabled
    if (CODE_COVERAGE)
        find_program(GCOV gcov)
        find_program(LCOV lcov)
        find_program(GENHTML genhtml)
    
        if (NOT (GCOV AND LCOV AND GENHTML))
            message(WARNING "Required tools not found: gcov,lcov,genhtml. Code coverage can not be performed")
        endif()  
    
        # set lcov variable to exclude paths during code coverage
        set(LCOV_FILTERS "'/usr/*';'/*/_deps/*'")
    
        # add custom target for running ctest and generating code coverage report
        add_custom_target(code-coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory coverage/
            COMMAND ctest -j
            COMMAND ${LCOV} --rc lcov_branch_coverage=1 --capture --directory . --output-file coverage/coverage.info
            COMMAND ${LCOV} --rc lcov_branch_coverage=1 --remove  coverage/coverage.info ${LCOV_FILTERS} --output-file coverage/coverage_filtered.info 
            COMMAND ${LCOV} --list coverage/coverage_filtered.info
            COMMAND ${GENHTML} --rc genhtml_branch_coverage=1 coverage/coverage_filtered.info --output-directory coverage/html_report
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating Code Coverage Report (LCOV) after tests.."
        )
    endif()

    if (VALGRIND)
        set(MEMORYCHECK_COMMAND_OPTIONS "-q --trace-children=yes --child-silent-after-fork=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --default-suppressions=no --suppressions=${PROJECT_SOURCE_DIR}/test/.valgrind-suppressions")
        # To run all registered tests with valgrind use "ctest -T memcheck"
        include(CTest) # Default to using valgrind
    endif()
endif ()

# add test cases
if (AOCL_TEST_COVERAGE)
    cmake_minimum_required(VERSION 3.18.0)
    enable_testing()

    file(GLOB calgary_dataset ${CMAKE_BINARY_DIR}/calgary/*)
    foreach(filename ${calgary_dataset})
        file(RELATIVE_PATH TFILE ${CMAKE_BINARY_DIR}/calgary ${filename})
        foreach(method ${METHODS})
            add_test(BENCH-${method}-calgary-${TFILE} ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t ${filename})
            add_test(BENCH-${method}-calgary-${TFILE}-optoff ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t -o ${filename})
            add_test(BENCH-${method}-calgary-${TFILE}-native ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t -n ${filename})
        endforeach()
    endforeach()

    file(GLOB canterbury_dataset ${CMAKE_BINARY_DIR}/canterbury/*)
    foreach(filename ${canterbury_dataset})
        file(RELATIVE_PATH TFILE ${CMAKE_BINARY_DIR}/canterbury ${filename})
        foreach(method ${METHODS})
            add_test(BENCH-${method}-canterbury-${TFILE} ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t ${filename})
            add_test(BENCH-${method}-canterbury-${TFILE}-optoff ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t -o ${filename})
            add_test(BENCH-${method}-canterbury-${TFILE}-native ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t -n ${filename})
        endforeach()
    endforeach()

    file(GLOB silesia_dataset ${CMAKE_BINARY_DIR}/silesia/*)
    foreach(filename ${silesia_dataset})
        file(RELATIVE_PATH TFILE ${CMAKE_BINARY_DIR}/silesia ${filename})
        foreach(method ${METHODS})
            add_test(BENCH-${method}-silesia-${TFILE} ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t ${filename})
            add_test(BENCH-${method}-silesia-${TFILE}-optoff ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t -o ${filename})
            add_test(BENCH-${method}-silesia-${TFILE}-native ${CMAKE_BINARY_DIR}/aocl_compression_bench -e${method} -t -n ${filename})
        endforeach()
    endforeach()
endif()
