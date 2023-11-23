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

cmake_minimum_required(VERSION 3.18.0)

if (NOT AOCL_EXCLUDE_BZIP2)
    set (METHODS ${METHODS} bzip2)
endif ()
if (NOT AOCL_EXCLUDE_LZ4)
    set (METHODS ${METHODS} lz4)
endif ()
if (NOT AOCL_EXCLUDE_LZ4HC AND NOT AOCL_EXCLUDE_LZ4)
    set (METHODS ${METHODS} lz4hc)
endif ()
if (NOT AOCL_EXCLUDE_LZMA)
    set (METHODS ${METHODS} lzma)
endif ()
if (NOT AOCL_EXCLUDE_SNAPPY)
    set (METHODS ${METHODS} snappy)
endif ()
if (NOT AOCL_EXCLUDE_ZLIB)
    set (METHODS ${METHODS} zlib)
endif ()
if (NOT AOCL_EXCLUDE_ZSTD)
    set (METHODS ${METHODS} zstd)
endif ()

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
