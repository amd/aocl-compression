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

cmake_minimum_required(VERSION 3.18.0)

set (ALGOS_PATH ../../algos)

#Add third party tests for each algo
if (NOT AOCL_EXCLUDE_ZSTD)
    #zstd fuzzer 
    set (ZSTD_TP_PATH zstd)
    set (ZSTD_TP_SRC_FILES 
                        ${ZSTD_TP_PATH}/programs/datagen.c
                        ${ZSTD_TP_PATH}/programs/timefn.c
                        ${ZSTD_TP_PATH}/tests/fuzzer.c)
endif ()

set (TP_SRC_FILES 
        ${ZSTD_TP_SRC_FILES}
        third_party_bench.c
)

if (AOCL_EXCLUDE_ZSTD)
    set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_EXCLUDE_ZSTD")
endif()

set_property(SOURCE ${TP_SRC_FILES} APPEND PROPERTY COMPILE_OPTIONS "-DAOCL_NO_REL_INC_PATH")

#create third party test bench
set (tp_exe_name aocl_third_party_bench)
add_executable(${tp_exe_name} ${TP_SRC_FILES})
set_target_properties(${tp_exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (NOT AOCL_EXCLUDE_ZSTD)
    target_include_directories (${tp_exe_name} PRIVATE ${ALGOS_PATH}/zstd/lib 
                                                       ${ALGOS_PATH}/zstd/lib/common
                                                       ${ZSTD_TP_PATH}/programs
                                                       ${CMAKE_SOURCE_DIR}/api
                                                       ${CMAKE_SOURCE_DIR}/utils
                                                       ${ALGOS_PATH}/common)
endif()

target_link_libraries(${tp_exe_name} ${lib_name})
target_link_libraries(${tp_exe_name} ${CMAKE_DL_LIBS})

#Add tests
if (NOT AOCL_EXCLUDE_ZSTD)
    add_test(ZSTD_FUZZER ${CMAKE_BINARY_DIR}/aocl_third_party_bench -eZSTD_FUZZER -i1 -v) #Add zstd fuzzer test
endif ()
