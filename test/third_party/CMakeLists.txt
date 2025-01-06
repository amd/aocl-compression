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
 
# @author Ashish Sriram

cmake_minimum_required(VERSION 3.26.0)

set (ALGOS_PATH ../../algos)

if (NOT BUILD_STATIC_LIBS)
    # For test benches that test non-API functions in aocl_compression library, 
    # visibility of these functions to the test bench is available only when the
    # test bench is built with static linking to the library
    # These test benches are added only when BUILD_STATIC_LIBS=ON

    message(WARNING "BUILD_STATIC_LIBS must be ON to build zstd utility and third party tests")
endif ()

# Build third party tests
if (TEST_COVERAGE_THIRD_PARTY AND BUILD_STATIC_LIBS)
    set(THIRD_PARTY_INTERNAL_TEST ON)
    # Add third party tests for each algo
    if (NOT AOCL_EXCLUDE_ZSTD)
        # Add test benches for zstd
        set (ZSTD_TP_PATH zstd)
        set (ZSTD_TP_SRC_FILES 
                            ${ZSTD_TP_PATH}/programs/benchfn.c
                            ${ZSTD_TP_PATH}/programs/benchzstd.c
                            ${ZSTD_TP_PATH}/programs/datagen.c
                            ${ZSTD_TP_PATH}/programs/timefn.c
                            ${ZSTD_TP_PATH}/programs/util.c
                            ${ZSTD_TP_PATH}/tests/bigdict.c
                            ${ZSTD_TP_PATH}/tests/external_matchfinder.c
                            ${ZSTD_TP_PATH}/tests/invalidDictionaries.c
                            ${ZSTD_TP_PATH}/tests/longmatch.c
                            ${ZSTD_TP_PATH}/tests/fuzzer.c
                            ${ZSTD_TP_PATH}/tests/paramgrill.c
                            ${ZSTD_TP_PATH}/tests/roundTripCrash.c
                            ${ZSTD_TP_PATH}/tests/seqgen.c
                            ${ZSTD_TP_PATH}/tests/zstreamtest.c)

        if (THIRD_PARTY_INTERNAL_TEST)
            set (ZSTD_TP_SRC_FILES
                                ${ZSTD_TP_SRC_FILES} 
                                ${ZSTD_TP_PATH}/tests/fullbench.c
                                ${ZSTD_TP_PATH}/tests/decodecorpus.c)
        endif ()
    endif ()
    if (NOT AOCL_EXCLUDE_ZLIB)
        set (ZLIB_TP_PATH zlib)
        file (GLOB ZLIB_TP_SRC_FILES 
                            ${ZLIB_TP_PATH}/test/example.c
                            ${ZLIB_TP_PATH}/test/infcover.c)
    endif ()

    set (TP_SRC_FILES 
            ${ZSTD_TP_SRC_FILES}
            ${ZLIB_TP_SRC_FILES}
            third_party_bench.c
    )
    if (THIRD_PARTY_INTERNAL_TEST)
        set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DTHIRD_PARTY_INTERNAL_TEST")
    endif ()

    if (AOCL_EXCLUDE_ZSTD)
        set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZSTD")
    endif ()
    if (AOCL_DECOMPRESS_FAST)
        set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_DECOMPRESS_FAST=${AOCL_DECOMPRESS_FAST}")
    endif ()
    if (AOCL_EXCLUDE_ZLIB)
        set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZLIB")
    endif ()

    if (AOCL_ENABLE_THREADS)
        set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_ENABLE_THREADS")
    endif ()

    set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_NO_REL_INC_PATH")
    set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_THIRD_PARTY_TEST")

    # Create third party test bench
    set (tp_exe_name aocl_third_party_bench)
    add_executable(${tp_exe_name} ${TP_SRC_FILES})
    set_target_properties(${tp_exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

    # Add additional include directories needed by zstd test benches
    target_include_directories (${tp_exe_name} PRIVATE  ${CMAKE_SOURCE_DIR}/api
                                                        ${CMAKE_SOURCE_DIR}/utils
                                                        ${ALGOS_PATH}/common)
    if (NOT AOCL_EXCLUDE_ZSTD)
        # Add additional include directories needed by zstd test benches
        target_include_directories (${tp_exe_name} PRIVATE ${ALGOS_PATH}/zstd/lib 
                                                        ${ALGOS_PATH}/zstd/lib/common
                                                        ${ALGOS_PATH}/zstd/lib/compress
                                                        ${ALGOS_PATH}/zstd/lib/decompress
                                                        ${ZSTD_TP_PATH}/programs)
        include(CheckLibraryExists)
        check_library_exists("m" "log" "" LIBM_EXISTS) # Some of the zstd tests need this.
        if (${LIBM_EXISTS})
            target_link_libraries(${tp_exe_name} m)
        endif ()
    endif ()
    if (NOT AOCL_EXCLUDE_ZLIB)
        target_include_directories (${tp_exe_name} PRIVATE ${ALGOS_PATH}/zlib)
    endif ()

    target_link_libraries(${tp_exe_name} ${lib_name})
    target_link_libraries(${tp_exe_name} ${CMAKE_DL_LIBS})

    # Add tests
    # Each test runs aocl_third_party_bench by passing a specific test bench target passed via -e option.
    # Subsequent options are specific to the selected test bench target and are forwarded to them.
    if (NOT AOCL_EXCLUDE_ZSTD)
        add_test(TP_ZSTD_BIGDICT              ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_BIGDICT)
        add_test(TP_ZSTD_FUZZER               ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_FUZZER -i1 -v)
        add_test(TP_ZSTD_INVALIDDICTIONARIES  ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_INVALIDDICTIONARIES)
        add_test(TP_ZSTD_LONGMATCH            ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_LONGMATCH)
        add_test(TP_ZSTD_PARAMGRILL           ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_PARAMGRILL -v -S)
        add_test(TP_ZSTD_ROUNDTRIPCRASH       ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_ROUNDTRIPCRASH ${CMAKE_BINARY_DIR}/silesia/dickens)
    if (AOCL_DECOMPRESS_FAST STREQUAL "OFF")
        add_test(TP_ZSTD_ZSTREAMTEST_1        ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_ZSTREAMTEST -i1 -v)
        add_test(TP_ZSTD_ZSTREAMTEST_2        ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_ZSTREAMTEST --newapi -t1 -i1)
    endif ()
    if (THIRD_PARTY_INTERNAL_TEST)
        add_test(TP_ZSTD_FULLBENCH            ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_FULLBENCH -i1)
        add_test(TP_ZSTD_DECODECORPUS         ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_DECODECORPUS -t -T1mn)
    endif ()
    endif ()

    if (NOT AOCL_EXCLUDE_ZLIB)
        add_test(TP_ZLIB_EXAMPLE              ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZLIB_EXAMPLE)
        add_test(TP_ZLIB_INFCOVER             ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZLIB_INFCOVER)        
    endif ()
endif()

# Build zstd cli utility
if (BUILD_UTILITY)
    if(BUILD_STATIC_LIBS)
      set(BU_ZSTD_LIBRARY_DIR ${ALGOS_PATH})
      # Set the path to the zstd directory
      set (BU_ZSTD_TP_PATH zstd)
      set (BU_ZSTD_PROGRAMS_DIR ${BU_ZSTD_TP_PATH}/programs)    
  
      # Check If ZSTD exclude flag is set
      if (NOT AOCL_EXCLUDE_ZSTD)
  
          # Set the path to the zstd program directory
          file(GLOB ZSTD_PROGRAM_SRCS "${BU_ZSTD_PROGRAMS_DIR}/*.c")
  
          # Create testbench zstd_utility
          set (zstd_utility zstd_utility) 
          add_executable(${zstd_utility} ${ZSTD_PROGRAM_SRCS}) 
          set_target_properties(${zstd_utility} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
          target_include_directories(${zstd_utility} PRIVATE ${CMAKE_SOURCE_DIR} 
  							${CMAKE_SOURCE_DIR}/api 
                                              		${CMAKE_SOURCE_DIR}/utils 
  							${BU_ZSTD_LIBRARY_DIR}/common)
          target_include_directories(${zstd_utility} PRIVATE ${BU_ZSTD_LIBRARY_DIR}/zstd/lib 
  							${BU_ZSTD_LIBRARY_DIR}/zstd/lib/common 
                                              		${BU_ZSTD_LIBRARY_DIR}/zstd/lib/compress 
  							${BU_ZSTD_LIBRARY_DIR}/zstd/lib/decompress 
                                              		${LIBRARY_DIR}/zstd/lib/dictBuilder)
         
          target_link_libraries(${zstd_utility} ${lib_name})
          target_link_libraries(${zstd_utility} ${CMAKE_DL_LIBS})
      endif()
    endif()
    # Build minigzip utility
    # Set the path to the zlib directory
    set (BU_ZLIB_TP_PATH zlib)
    set (BU_ZLIB_GZIP_DIR ${BU_ZLIB_TP_PATH}/test)
    if (NOT AOCL_EXCLUDE_ZLIB)
        set(ZLIB_MINIGZIP_SRCS "${BU_ZLIB_GZIP_DIR}/minigzip.c")
        set (minigzip minigzip) 
        add_executable(${minigzip} ${ZLIB_MINIGZIP_SRCS})
        set_target_properties(${minigzip} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
        target_include_directories (${minigzip} PRIVATE ${ALGOS_PATH}/zlib
                                                        ${ALGOS_PATH}/common)
        target_link_libraries(${minigzip} ${lib_name})
        target_link_libraries(${minigzip} ${CMAKE_DL_LIBS})
        add_test(TP_ZLIB_MINIGZIP_C_L1        ${CMAKE_BINARY_DIR}/minigzip -1 ${CMAKE_BINARY_DIR}/silesia/dickens)
        add_test(TP_ZLIB_MINIGZIP_D_L1        ${CMAKE_BINARY_DIR}/minigzip -d ${CMAKE_BINARY_DIR}/silesia/dickens.gz)
        set_tests_properties(TP_ZLIB_MINIGZIP_C_L1 PROPERTIES FIXTURES_SETUP FIXTURE_CL1)
        set_tests_properties(TP_ZLIB_MINIGZIP_D_L1 PROPERTIES FIXTURES_REQUIRED FIXTURE_CL1)

        add_test(TP_ZLIB_MINIGZIP_C_L6        ${CMAKE_BINARY_DIR}/minigzip -6 ${CMAKE_BINARY_DIR}/silesia/xml)
        add_test(TP_ZLIB_MINIGZIP_D_L6        ${CMAKE_BINARY_DIR}/minigzip -d ${CMAKE_BINARY_DIR}/silesia/xml.gz)
        set_tests_properties(TP_ZLIB_MINIGZIP_C_L6 PROPERTIES FIXTURES_SETUP FIXTURE_CL6)
        set_tests_properties(TP_ZLIB_MINIGZIP_D_L6 PROPERTIES FIXTURES_REQUIRED FIXTURE_CL6)

        add_test(TP_ZLIB_MINIGZIP_C_L9        ${CMAKE_BINARY_DIR}/minigzip -9 ${CMAKE_BINARY_DIR}/silesia/webster)
        add_test(TP_ZLIB_MINIGZIP_D_L9        ${CMAKE_BINARY_DIR}/minigzip -d ${CMAKE_BINARY_DIR}/silesia/webster.gz)
        set_tests_properties(TP_ZLIB_MINIGZIP_C_L9 PROPERTIES FIXTURES_SETUP FIXTURE_CL9)
        set_tests_properties(TP_ZLIB_MINIGZIP_D_L9 PROPERTIES FIXTURES_REQUIRED FIXTURE_CL9)  
    endif()
endif ()


