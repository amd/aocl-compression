# Copyright (C) 2022-2026, Advanced Micro Devices. All rights reserved.
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
# @brief CMake based sphinx documentation generator.
#
# Checks if necessary packages are installed for generating sphinx documentation
# if they are not installed then neccessary steps are shown to install the required packages.
#
# @author Niranjan, Reddy

cmake_minimum_required(VERSION 3.13.0)

project(aoclcompression-doc)

find_package(Doxygen 1.9.1)
set(W_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Build Doxygen documentation
if(NOT (DOXYGEN_FOUND))
    message(STATUS "Doxygen not found please install doxygen and try again.")
else()
    execute_process(COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
            WORKING_DIRECTORY ${W_DIR})
endif()


# Check if xml generated successfully from Doxygen, if yes continue building sphinx documentation
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/xml/index.xml)
    message(STATUS "xml generated from Doxygen successfully, at ${W_DIR}/xml/ .")

# Check for python and the documentation packages:
# - Sphinx
# - rocm-docs-core
# - breathe
# - myst_parser

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    execute_process(
      COMMAND ${Python3_EXECUTABLE} -c "import sphinx"
      RESULT_VARIABLE EXIT_CODE
      OUTPUT_QUIET)
    if(NOT ${EXIT_CODE} EQUAL 0)
      message(
        FATAL_ERROR
          "Python3 package sphinx is required to build the documentation\n   Use: ${Python3_EXECUTABLE} -m pip3 install sphinx"
      )
    endif()
    execute_process(
      COMMAND ${Python3_EXECUTABLE} -c "from rocm_docs import ROCmDocs"
      RESULT_VARIABLE EXIT_CODE
      OUTPUT_QUIET)
    if(NOT ${EXIT_CODE} EQUAL 0)
      message(
        FATAL_ERROR
          "Python3 package rocm_docs is required to build the documentation\n   Use: ${Python3_EXECUTABLE} -m pip3 install rocm-docs-core"
      )
    endif()


    execute_process(
      COMMAND ${Python3_EXECUTABLE} -c "import breathe"
      RESULT_VARIABLE EXIT_CODE
      OUTPUT_QUIET)
    if(NOT ${EXIT_CODE} EQUAL 0)
      message(
        FATAL_ERROR
          "Python3 package breathe is required to build the documentation\n   Use: ${Python3_EXECUTABLE} -m pip3 install breathe"
      )
    endif()
    execute_process(
      COMMAND ${Python3_EXECUTABLE} -c "import myst_parser"
      RESULT_VARIABLE EXIT_CODE
      OUTPUT_QUIET)
    if(NOT ${EXIT_CODE} EQUAL 0)
      message(
        FATAL_ERROR
          "Python3 package myst_parser is required to build the documentation\n   Use: ${Python3_EXECUTABLE} -m pip3 install myst_parser"
      )
    endif()

    include (FindPackageHandleStandardArgs)
 
    ## Look for an executable called sphinx-build
    find_program (SPHINX_EXECUTABLE
                  NAMES sphinx-build sphinx-build.exe
                  DOC "Sphinx Documentation Generator"
    )

    if(NOT SPHINX_EXECUTABLE)
      message(FATAL_ERROR "Failed to find sphinx-build executable.")
    endif()
    
    message(STATUS "Building Sphinx documentation.")

    execute_process(
      COMMAND ${SPHINX_EXECUTABLE} -M html ./rst/ ./sphinx
      RESULT_VARIABLE EXIT_CODE
      WORKING_DIRECTORY ${W_DIR})

    if(NOT ${EXIT_CODE} EQUAL 0)
      message(
        FATAL_ERROR
          "Sphinx document generation failed: ${EXIT_CODE}"
      )
    else()
      message(STATUS "Sphinx generated successfully at ${W_DIR}/sphinx/html")
      file(REMOVE_RECURSE ${W_DIR}/sphinx/doctrees)
    endif()
else() # !(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/docs/xml/index.xml)
    message(STATUS "Doxygen document generation failed.")
endif() # (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/docs/xml/index.xml)
