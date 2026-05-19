# Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

# ==============================================================================
# AOCL-Compression Build Configuration
# ==============================================================================
# This file contains all user-configurable build options.
# It mirrors the CMake options from CMakeLists.txt
#
# Usage:
#   Edit values below to customize the build, or override on command line:
#   make BUILD_STATIC_LIBS=1 AOCL_EXCLUDE_ZSTD=1
# ==============================================================================

# ==============================================================================
# LIBRARY BUILD TYPE
# ==============================================================================
# Build static library instead of shared library
# CMake equivalent: BUILD_STATIC_LIBS
BUILD_STATIC_LIBS ?= 0

# ==============================================================================
# BUILD CONFIGURATION
# ==============================================================================
# Build type: Release or Debug
# CMake equivalent: CMAKE_BUILD_TYPE
BUILD_TYPE ?= Release

# Enable verbose build output (shows full compiler commands)
VERBOSE ?= 0

# ==============================================================================
# ALGORITHM EXCLUSIONS
# ==============================================================================
# Exclude specific compression algorithms from the build
# CMake equivalent: AOCL_EXCLUDE_*

AOCL_EXCLUDE_BZIP2 ?= 0
AOCL_EXCLUDE_LZ4 ?= 0
AOCL_EXCLUDE_LZ4HC ?= 0
AOCL_EXCLUDE_LZMA ?= 0
AOCL_EXCLUDE_SNAPPY ?= 0
AOCL_EXCLUDE_ZLIB ?= 0
AOCL_EXCLUDE_ZSTD ?= 0

# ==============================================================================
# LZ4 OPTIMIZATIONS
# ==============================================================================
# CMake equivalent: AOCL_LZ4_*

# Enable LZ4 backward prefetching optimizations
AOCL_LZ4_OPT_PREFETCH_BACKWARDS ?= 0

# Enable LZ4 match skipping optimization strategy 1
AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1 ?= 0

# Enable LZ4 match skipping optimization strategy 2
AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2 ?= 0

# Use alternative prime number for LZ4 hashing
AOCL_LZ4_NEW_PRIME_NUMBER ?= 0

# Enable additional hash table updates for better compression ratio
AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES ?= 0

# Control number of bits used for LZ4 hashing
# Values: OFF, LOW, HIGH
AOCL_LZ4_HASH_BITS_USED ?= LOW

# Disable pattern analysis in LZ4HC level 9
AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS ?= 1

# Enable LZ4 frame format and API support
LZ4_FRAME_FORMAT_SUPPORT ?= 0

# ==============================================================================
# SNAPPY OPTIMIZATIONS
# ==============================================================================
# CMake equivalent: SNAPPY_*

# Enable Snappy match skipping optimization
SNAPPY_MATCH_SKIP_OPT ?= 1

# Enable Snappy high compression mode (slower, better ratio)
SNAPPY_HIGH_COMPRESSION ?= 0

# Enable Snappy branchless decompression (auto-detected based on compiler)
# Default: ON for Clang, OFF for GCC
SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS ?= AUTO

# ==============================================================================
# ZSTD OPTIMIZATIONS
# ==============================================================================
# CMake equivalent: AOCL_ZSTD_*

# Enable ZSTD aggressive match skipping optimization
AOCL_ZSTD_SEARCH_SKIP_OPT ?= 1

# Enable ZSTD dynamic block size determination
AOCL_ZSTD_DYN_BLOCK_SIZE ?= 0

# ==============================================================================
# BZIP2 OPTIMIZATIONS
# ==============================================================================
# CMake equivalent: AOCL_BZIP2_*

# Number of Huffman table iterations (1, 2, 3, or 4)
# Lower values = faster but worse compression ratio
AOCL_BZIP2_HUFFMAN_ITERATIONS ?= 3

# ==============================================================================
# FAST DECOMPRESSION MODES
# ==============================================================================
# Enable fast decompression modes that may compromise compression speed/ratio
# to produce streams that decompress faster
# CMake equivalent: AOCL_DECOMPRESS_FAST
#
# Values:
#   OFF - Disabled
#   1   - Level 1 (ZSTD, Snappy, LZ4)
#   2   - Level 2 (ZSTD, Snappy)
#   3   - Level 3 (ZSTD only)
AOCL_DECOMPRESS_FAST ?= OFF

# ==============================================================================
# FAST COMPRESSION MODES
# ==============================================================================
# Enable fast compression modes that may compromise compression ratio
# CMake equivalent: AOCL_COMPRESS_FAST
#
# Values:
#   OFF - Disabled
#   1   - Level 1 (ZSTD)
#   2   - Level 2 (ZSTD)
AOCL_COMPRESS_FAST ?= OFF

# ==============================================================================
# FEATURE SUPPORT
# ==============================================================================
# CMake equivalent: AOCL_ENABLE_*, NATIVE_ENABLE_*

# Enable logging support (controlled by AOCL_ENABLE_LOG environment variable)
AOCL_ENABLE_LOG_FEATURE ?= 0

# Enable multi-threaded compression using OpenMP
AOCL_ENABLE_THREADS ?= 0

# Enable native multi-threading for supported methods (pthread)
NATIVE_ENABLE_THREADS ?= 0

# Enable symbol prefixing
AOCL_LLC_PREFIX ?= 0

# Enable experimental xz-utils LZMA APIs
AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL ?= 0

# ==============================================================================
# TESTING & COVERAGE
# ==============================================================================
# CMake equivalent: AOCL_TEST_*, CODE_COVERAGE, etc.

# Enable GTest, AOCL test bench, and third-party test suites
AOCL_TEST_COVERAGE ?= 0

# TODO: Future implementation
# # Enable code coverage (gcov for GCC, llvm-cov for Clang)
# CODE_COVERAGE ?= 0

# TODO: Future implementation
# # Enable third-party test bench CTest suite
# TEST_COVERAGE_THIRD_PARTY ?= 0

# TODO: Future implementation
# # Enable Address Sanitizer (Linux/Debug only)
# ASAN ?= 0

# TODO: Future implementation
# # Enable Valgrind checks (Linux/Debug only, incompatible with ASAN)
# VALGRIND ?= 0

# TODO: Future implementation
# # Enable AOCL fuzzer (Linux + Clang only)
# AOCL_TEST_FUZZER ?= 0

# TODO: Future implementation
# # Run fuzzer with corpus
# AOCL_TEST_FUZZER_WITH_CORPUS ?= 0

# ==============================================================================
# BUILD UTILITIES
# ==============================================================================
# CMake equivalent: BUILD_*

# Build documentation
# BUILD_DOC ?= 0  # NOT YET IMPLEMENTED

# Build examples
# BUILD_EXAMPLE ?= 0  # NOT YET IMPLEMENTED

# Build third-party utilities (minigzip, zstd_utility)
# BUILD_UTILITY ?= 0  # NOT YET IMPLEMENTED

# ==============================================================================
# COMPILER OPTIMIZATIONS
# ==============================================================================

# Enable fast-math optimizations
ENABLE_FAST_MATH ?= 0

# Treat warnings as errors
ENABLE_STRICT_WARNINGS ?= 1

# ==============================================================================
# INSTALLATION PATHS
# ==============================================================================
# CMake equivalent: CMAKE_INSTALL_PREFIX, DEST_*

# Installation prefix
PREFIX ?= /usr/local

# Destination directory (for staged installs)
DESTDIR ?=

# Relative paths under PREFIX
DEST_LIB_PATH ?= lib
DEST_INC_PATH ?= include
DEST_EXAMPLES_PATH ?= examples

# ==============================================================================
# ADVANCED OPTIONS
# ==============================================================================

# Exclude deprecated APIs (used for code coverage)
AOCL_EXCLUDE_DEPRECATED_APIS ?= 0

# Number of parallel jobs (auto-detected)
JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# ==============================================================================
# VALIDATION
# ==============================================================================

# Validate configuration
ifeq ($(AOCL_EXCLUDE_BZIP2)$(AOCL_EXCLUDE_LZ4)$(AOCL_EXCLUDE_LZ4HC)$(AOCL_EXCLUDE_LZMA)$(AOCL_EXCLUDE_SNAPPY)$(AOCL_EXCLUDE_ZLIB)$(AOCL_EXCLUDE_ZSTD),1111111)
    $(error All compression methods are excluded. Unable to build library.)
endif

# LZ4HC depends on LZ4
ifeq ($(AOCL_EXCLUDE_LZ4),1)
    ifneq ($(AOCL_EXCLUDE_LZ4HC),1)
        $(warning LZ4HC depends on LZ4. Excluding LZ4HC as well.)
        override AOCL_EXCLUDE_LZ4HC := 1
    endif
endif

# Fast decompression not supported with multi-threading
ifneq ($(AOCL_DECOMPRESS_FAST),OFF)
    ifneq ($(AOCL_ENABLE_THREADS)$(NATIVE_ENABLE_THREADS),00)
        $(warning Fast decompression mode excluded as it is supported in single threaded mode only)
        override AOCL_DECOMPRESS_FAST := OFF
    endif
endif

# Avoid enabling both AOCL and native multi-threading
ifeq ($(AOCL_ENABLE_THREADS),1)
    ifeq ($(NATIVE_ENABLE_THREADS),1)
        $(warning Both AOCL_ENABLE_THREADS and NATIVE_ENABLE_THREADS are enabled. Disabling NATIVE_ENABLE_THREADS.)
        override NATIVE_ENABLE_THREADS := 0
    endif
endif

# Force static build when BUILD_UTILITY is enabled
# ifeq ($(BUILD_UTILITY),1)
#     ifneq ($(BUILD_STATIC_LIBS),1)
#         $(warning Building static library to allow all utilities to be built.)
#         override BUILD_STATIC_LIBS := 1
#     endif
# endif

# Validate AOCL_LZ4_HASH_BITS_USED
ifneq ($(filter-out OFF LOW HIGH,$(AOCL_LZ4_HASH_BITS_USED)),)
    $(error AOCL_LZ4_HASH_BITS_USED must be OFF, LOW, or HIGH. Got: $(AOCL_LZ4_HASH_BITS_USED))
endif

# Validate AOCL_BZIP2_HUFFMAN_ITERATIONS
ifneq ($(filter-out 1 2 3 4,$(AOCL_BZIP2_HUFFMAN_ITERATIONS)),)
    $(error AOCL_BZIP2_HUFFMAN_ITERATIONS must be 1, 2, 3, or 4. Got: $(AOCL_BZIP2_HUFFMAN_ITERATIONS))
endif

# Validate AOCL_DECOMPRESS_FAST
ifneq ($(filter-out OFF 1 2 3,$(AOCL_DECOMPRESS_FAST)),)
    $(error AOCL_DECOMPRESS_FAST must be OFF, 1, 2, or 3. Got: $(AOCL_DECOMPRESS_FAST))
endif

# Validate AOCL_COMPRESS_FAST
ifneq ($(filter-out OFF 1 2,$(AOCL_COMPRESS_FAST)),)
    $(error AOCL_COMPRESS_FAST must be OFF, 1, or 2. Got: $(AOCL_COMPRESS_FAST))
endif

# Validate SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS
ifneq ($(filter-out AUTO 0 1,$(SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS)),)
    $(error SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS must be AUTO, 0, or 1. Got: $(SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS))
endif

# ==============================================================================
# AUTO-ENABLING LOGIC (mirrors CMake behavior)
# ==============================================================================

# TODO: Future implementation
# # When AOCL_TEST_COVERAGE is enabled, automatically enable third-party tests
# # to maximize test coverage (matches CMake behavior)
# ifeq ($(AOCL_TEST_COVERAGE),1)
#     ifeq ($(TEST_COVERAGE_THIRD_PARTY),0)
#         # User explicitly disabled third-party tests, respect that
#     else
#         override TEST_COVERAGE_THIRD_PARTY := 1
#     endif
# endif

# Export all configuration variables
export BUILD_STATIC_LIBS BUILD_TYPE VERBOSE
export AOCL_EXCLUDE_BZIP2 AOCL_EXCLUDE_LZ4 AOCL_EXCLUDE_LZ4HC AOCL_EXCLUDE_LZMA
export AOCL_EXCLUDE_SNAPPY AOCL_EXCLUDE_ZLIB AOCL_EXCLUDE_ZSTD
export AOCL_LZ4_OPT_PREFETCH_BACKWARDS AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
export AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2 AOCL_LZ4_NEW_PRIME_NUMBER
export AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES AOCL_LZ4_HASH_BITS_USED
export AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS LZ4_FRAME_FORMAT_SUPPORT
export SNAPPY_MATCH_SKIP_OPT SNAPPY_HIGH_COMPRESSION SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS
export AOCL_ZSTD_SEARCH_SKIP_OPT AOCL_ZSTD_DYN_BLOCK_SIZE
export AOCL_BZIP2_HUFFMAN_ITERATIONS
export AOCL_DECOMPRESS_FAST AOCL_COMPRESS_FAST
export AOCL_ENABLE_LOG_FEATURE AOCL_ENABLE_THREADS NATIVE_ENABLE_THREADS
export AOCL_LLC_PREFIX AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL
export AOCL_TEST_COVERAGE
# TODO: Future implementation
# export CODE_COVERAGE TEST_COVERAGE_THIRD_PARTY
# export ASAN VALGRIND AOCL_TEST_FUZZER AOCL_TEST_FUZZER_WITH_CORPUS
# export BUILD_DOC BUILD_EXAMPLE BUILD_UTILITY
export ENABLE_FAST_MATH ENABLE_STRICT_WARNINGS
export PREFIX DESTDIR DEST_LIB_PATH DEST_INC_PATH DEST_EXAMPLES_PATH
export AOCL_EXCLUDE_DEPRECATED_APIS JOBS
