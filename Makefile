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
# AOCL-Compression GNU Make Build System
# ==============================================================================
# Main Makefile - Entry point for building AOCL-Compression library
#
# Usage:
#   make                    # Build library with default configuration
#   make BUILD_TYPE=Debug   # Build debug version
#   make test               # Build and run tests
#   make install            # Install library and headers
#   make clean              # Clean build artifacts
#
# Configuration:
#   Edit config.mk to customize build options
# ==============================================================================

# Ensure we're using a POSIX-compliant shell
SHELL := /bin/bash

# Project root directory
ROOT_DIR := $(CURDIR)
export ROOT_DIR

# Build output directory
BUILD_DIR ?= $(ROOT_DIR)/build
export BUILD_DIR

# ==============================================================================
# INCLUDE CONFIGURATION AND PLATFORM DETECTION
# ==============================================================================

# User configuration (all build options)
include config.mk

# Platform and compiler detection
include mk/platform.mk

# Verify the supported platform (Unix or Windows with MinGW/MSYS2)
ifeq ($(filter $(OS_TYPE),unix windows),)
    $(error GNU Make build supports Unix and Windows (MinGW/MSYS2). Detected OS_TYPE: $(OS_TYPE))
endif

# ==============================================================================
# ISA LEVEL NUMERIC MAPPING
# ==============================================================================

# Normalize to uppercase for case-insensitive matching
AOCL_MAX_ISA_LEVEL := $(shell echo '$(AOCL_MAX_ISA_LEVEL)' | tr '[:lower:]' '[:upper:]')

# Map AOCL_MAX_ISA_LEVEL string to numeric value
ifeq ($(AOCL_MAX_ISA_LEVEL),SSE2)
    AOCL_MAX_ISA_LEVEL_NUM := 1
else ifeq ($(AOCL_MAX_ISA_LEVEL),AVX)
    AOCL_MAX_ISA_LEVEL_NUM := 2
else ifeq ($(AOCL_MAX_ISA_LEVEL),AVX2)
    AOCL_MAX_ISA_LEVEL_NUM := 3
else
    AOCL_MAX_ISA_LEVEL_NUM := 4
endif

# Build rules and patterns
include mk/rules.mk

# ==============================================================================
# BUILD VERSION
# ==============================================================================

BUILD_DATE := $(shell date +%Y%m%d)
BUILD_VERSION := Build $(BUILD_DATE)
export BUILD_VERSION

# ==============================================================================
# LIBRARY CONFIGURATION
# ==============================================================================

LIB_NAME := aocl_compression
LIB_DIR := $(BUILD_DIR)/lib
OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/deps
GEN_DIR := $(BUILD_DIR)/generated

# Generated common header path
GEN_AOCL_ALGO_OPT := $(GEN_DIR)/algos/common/aoclAlgoOpt.h

# Library output
ifeq ($(BUILD_STATIC_LIBS),1)
    LIB_TARGET := $(LIB_DIR)/$(LIB_PREFIX)$(LIB_NAME)$(LIB_STATIC_EXT)
    LIB_TYPE := static
else
    LIB_TARGET := $(LIB_DIR)/$(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT)
    LIB_TYPE := shared
endif

# Link-time library artifact (import library on Windows/MinGW shared builds).
LIB_LINK_TARGET := $(LIB_TARGET)
ifneq ($(BUILD_STATIC_LIBS),1)
ifeq ($(OS_TYPE),windows)
    LIB_LINK_TARGET := $(LIB_DIR)/$(LIB_PREFIX)$(LIB_NAME).dll.a
endif
endif

export LIB_TARGET LIB_LINK_TARGET LIB_NAME LIB_DIR OBJ_DIR DEP_DIR GEN_DIR

# ==============================================================================
# INCLUDE ALGORITHM MODULES
# ==============================================================================

# Initialize source lists
ALL_SOURCES :=
ALL_HEADERS :=
ALL_CFLAGS :=
ALL_CXXFLAGS :=

# API, algorithm modules, threading, and utilities (inlined)
# ---- BEGIN INLINE: mk/algos/api.mk ----
# ==============================================================================
# API Module
# ==============================================================================
# AOCL-Compression API sources (always included)
# ==============================================================================

# API source files
API_SOURCES := api/code_alignment.cpp \
               api/api.cpp \
               api/codec.cpp

# API header files
API_HEADERS := api/aocl_compression.h \
               api/codec.h \
               api/types.h

# Add to global lists
ALL_SOURCES += $(API_SOURCES)
ALL_HEADERS += $(API_HEADERS)

# API-specific flags
API_CFLAGS :=
API_CXXFLAGS :=

# Add compiler and flags definitions
API_CFLAGS += -DCCompiler="\"$(CC)\""
API_CXXFLAGS += -DCXXCompiler="\"$(CXX)\""
API_CFLAGS += -DCFLAGS_SET2="\"$(CFLAGS)\""
API_CXXFLAGS += -DCFLAGS_SET2="\"$(CXXFLAGS)\""

# Add build type specific flags
ifeq ($(BUILD_TYPE),Release)
    API_CFLAGS += -DCFLAGS_SET1="\"-O3 -DNDEBUG\""
    API_CXXFLAGS += -DCFLAGS_SET1="\"-O3 -DNDEBUG\""
else
    API_CFLAGS += -DCFLAGS_SET1="\"-g -O0 -DDEBUG\""
    API_CXXFLAGS += -DCFLAGS_SET1="\"-g -O0 -DDEBUG\""
endif

# Add algorithm exclusion flags to API sources
ifeq ($(AOCL_EXCLUDE_BZIP2),1)
    API_CFLAGS += -DAOCL_EXCLUDE_BZIP2
    API_CXXFLAGS += -DAOCL_EXCLUDE_BZIP2
endif

ifeq ($(AOCL_EXCLUDE_LZ4),1)
    API_CFLAGS += -DAOCL_EXCLUDE_LZ4
    API_CXXFLAGS += -DAOCL_EXCLUDE_LZ4
endif

ifeq ($(AOCL_EXCLUDE_LZ4HC),1)
    API_CFLAGS += -DAOCL_EXCLUDE_LZ4HC
    API_CXXFLAGS += -DAOCL_EXCLUDE_LZ4HC
endif

ifeq ($(AOCL_EXCLUDE_LZMA),1)
    API_CFLAGS += -DAOCL_EXCLUDE_LZMA
    API_CXXFLAGS += -DAOCL_EXCLUDE_LZMA
endif

ifeq ($(AOCL_EXCLUDE_SNAPPY),1)
    API_CFLAGS += -DAOCL_EXCLUDE_SNAPPY
    API_CXXFLAGS += -DAOCL_EXCLUDE_SNAPPY
endif

ifeq ($(AOCL_EXCLUDE_ZLIB),1)
    API_CFLAGS += -DAOCL_EXCLUDE_ZLIB
    API_CXXFLAGS += -DAOCL_EXCLUDE_ZLIB
endif

ifeq ($(AOCL_EXCLUDE_ZSTD),1)
    API_CFLAGS += -DAOCL_EXCLUDE_ZSTD
    API_CXXFLAGS += -DAOCL_EXCLUDE_ZSTD
endif

# Experimental LZMA API support
ifeq ($(AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL),1)
    API_CFLAGS += -DAOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL
    API_CXXFLAGS += -DAOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL
endif


# Symbol prefixing support
ifeq ($(AOCL_LLC_PREFIX),1)
    API_CFLAGS += -DAOCL_LLC_PREFIX
    API_CXXFLAGS += -DAOCL_LLC_PREFIX
endif

# Add to global flags
ALL_CFLAGS += $(API_CFLAGS)
ALL_CXXFLAGS += $(API_CXXFLAGS)

# Per-file flags for API sources (need algos/common for includes)
$(OBJ_DIR)/api/%.o: CFLAGS += $(API_CFLAGS)
$(OBJ_DIR)/api/%.o: CXXFLAGS += $(API_CXXFLAGS)
# ---- END INLINE: mk/algos/api.mk ----

# ---- BEGIN INLINE: mk/algos/bzip2.mk ----
# ==============================================================================
# BZIP2 Module
# ==============================================================================
# BZIP2 compression algorithm integration
# ==============================================================================

ifneq ($(AOCL_EXCLUDE_BZIP2),1)

# BZIP2 source files
BZIP2_SOURCES := algos/bzip2/code_alignment.c \
                 algos/bzip2/blocksort.c \
                 algos/bzip2/huffman.c \
                 algos/bzip2/crctable.c \
                 algos/bzip2/randtable.c \
                 algos/bzip2/compress.c \
                 algos/bzip2/decompress.c \
                 algos/bzip2/bzlib.c \
                 algos/bzip2/libsais.c

# BZIP2 header files
BZIP2_HEADERS := algos/bzip2/bzlib.h \
                 algos/bzip2/libsais.h

# Add multi-threaded helper if threads enabled
ifeq ($(AOCL_ENABLE_THREADS),1)
    BZIP2_SOURCES += algos/bzip2/aocl_bzip2_mt_helper.c
    BZIP2_HEADERS += algos/bzip2/aocl_bzip2_mt_helper.h
endif

# Add to global lists
ALL_SOURCES += $(BZIP2_SOURCES)
ALL_HEADERS += $(BZIP2_HEADERS)

# BZIP2-specific flags
BZIP2_CFLAGS :=

# Huffman iterations optimization
BZIP2_CFLAGS += -DAOCL_BZIP2_HUFFMAN_ITERATIONS=$(AOCL_BZIP2_HUFFMAN_ITERATIONS)

# Debug flags for Debug build (if not testing)
ifeq ($(BUILD_TYPE),Debug)
    ifneq ($(AOCL_TEST_COVERAGE),1)
        BZIP2_CFLAGS += -DBZ_DEBUG
    endif
endif

# Windows DLL export (shared build) -- matches CMake set_property pattern
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
    BZIP2_CFLAGS += -DBZIP2_DLL_EXPORT=1
endif
endif

# Per-file flags for BZIP2 sources
$(OBJ_DIR)/algos/bzip2/%.o: CFLAGS += $(BZIP2_CFLAGS)

# Installation headers
INSTALL_HEADERS += algos/bzip2/bzlib.h

endif # AOCL_EXCLUDE_BZIP2
# ---- END INLINE: mk/algos/bzip2.mk ----

# ---- BEGIN INLINE: mk/algos/lz4.mk ----
# ==============================================================================
# LZ4 Module
# ==============================================================================
# LZ4 compression algorithm integration
# Includes template generation for optimized variants
# ==============================================================================

ifneq ($(AOCL_EXCLUDE_LZ4),1)

# LZ4 source files
LZ4_SOURCES := algos/lz4/code_alignment_lz4.c \
               algos/lz4/lz4.c \
               algos/lz4/xxhash.c

# LZ4 header files
LZ4_HEADERS := algos/lz4/lz4.h \
               algos/lz4/xxhash.h

# Add LZ4 frame format support if enabled
ifeq ($(LZ4_FRAME_FORMAT_SUPPORT),1)
    LZ4_SOURCES += algos/lz4/lz4frame.c
    LZ4_HEADERS += algos/lz4/lz4frame.h \
                   algos/lz4/lz4frame_static.h
    
    # If LZ4HC is excluded but frame format is enabled, include lz4hc sources
    ifeq ($(AOCL_EXCLUDE_LZ4HC),1)
        LZ4_SOURCES += algos/lz4/lz4hc.c
        LZ4_HEADERS += algos/lz4/lz4hc.h
    endif
endif

# Add to global lists
ALL_SOURCES += $(LZ4_SOURCES)
ALL_HEADERS += $(LZ4_HEADERS)

# LZ4-specific flags
LZ4_CFLAGS :=

# Optimization flags
ifeq ($(AOCL_LZ4_OPT_PREFETCH_BACKWARDS),1)
    LZ4_CFLAGS += -DAOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
endif

ifeq ($(AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1),1)
    LZ4_CFLAGS += -DAOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
endif

ifeq ($(AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2),1)
    LZ4_CFLAGS += -DAOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
    LZ4_CFLAGS += -DAOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2
endif

ifeq ($(AOCL_LZ4_NEW_PRIME_NUMBER),1)
    LZ4_CFLAGS += -DAOCL_LZ4_NEW_PRIME_NUMBER
endif

ifeq ($(AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES),1)
    LZ4_CFLAGS += -DAOCL_LZ4_EXTRA_HASH_TABLE_UPDATES
endif

# Hash bits configuration
ifneq ($(AOCL_LZ4_HASH_BITS_USED),OFF)
    ifeq ($(AOCL_LZ4_HASH_BITS_USED),LOW)
        LZ4_CFLAGS += -DAOCL_LZ4_HASH_BITS_USED=41
    else ifeq ($(AOCL_LZ4_HASH_BITS_USED),HIGH)
        LZ4_CFLAGS += -DAOCL_LZ4_HASH_BITS_USED=44
    endif
endif

# Fast decompression mode
ifneq ($(AOCL_DECOMPRESS_FAST),OFF)
    LZ4_CFLAGS += -DAOCL_LZ4_DECOMPRESS_FAST
endif

# Exclude deprecated APIs (for code coverage)
ifeq ($(AOCL_EXCLUDE_DEPRECATED_APIS),1)
    LZ4_CFLAGS += -DAOCL_EXCLUDE_DEPRECATED_APIS
endif


# Disable asserts for testing
ifeq ($(BUILD_TYPE),Debug)
    ifeq ($(AOCL_TEST_COVERAGE),1)
        LZ4_CFLAGS += -DNDEBUG
    endif
endif

# Clang-specific: Force O2 for LZ4 sources
ifeq ($(CC_ID),clang)
    LZ4_CFLAGS += -O2
endif

# Symbol prefixing support
ifeq ($(AOCL_LLC_PREFIX),1)
    LZ4_CFLAGS += -DAOCL_LLC_PREFIX
endif

# Windows DLL export (shared build) -- matches CMake set_property pattern
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
    LZ4_CFLAGS += -DLZ4_DLL_EXPORT=1
endif
endif

# Add include path for aoclPrefix.h (in algos/common)

# Per-file flags for LZ4 sources
$(OBJ_DIR)/algos/lz4/%.o: CFLAGS += $(LZ4_CFLAGS)

# Installation headers
INSTALL_HEADERS += algos/lz4/lz4.h

ifeq ($(LZ4_FRAME_FORMAT_SUPPORT),1)
    INSTALL_HEADERS += algos/lz4/lz4frame.h \
                       algos/lz4/lz4frame_static.h
endif

endif # AOCL_EXCLUDE_LZ4
# ---- END INLINE: mk/algos/lz4.mk ----

# ---- BEGIN INLINE: mk/algos/lz4hc.mk ----
# ==============================================================================
# LZ4HC Module
# ==============================================================================
# LZ4HC (High Compression) algorithm integration
# Note: LZ4HC depends on LZ4, so LZ4 must not be excluded
# ==============================================================================

ifneq ($(AOCL_EXCLUDE_LZ4HC),1)

# LZ4HC source files
LZ4HC_SOURCES := algos/lz4/code_alignment_lz4hc.c \
                 algos/lz4/lz4hc.c

# LZ4HC header files
LZ4HC_HEADERS := algos/lz4/lz4hc.h

# Add to global lists
ALL_SOURCES += $(LZ4HC_SOURCES)
ALL_HEADERS += $(LZ4HC_HEADERS)

# LZ4HC-specific flags
LZ4HC_CFLAGS :=

# Disable pattern analysis optimization
ifeq ($(AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS),1)
    LZ4HC_CFLAGS += -DAOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS
endif

# Exclude deprecated APIs (for code coverage)
ifeq ($(AOCL_EXCLUDE_DEPRECATED_APIS),1)
    LZ4HC_CFLAGS += -DAOCL_EXCLUDE_DEPRECATED_APIS
endif


# Disable asserts for testing
ifeq ($(BUILD_TYPE),Debug)
    ifeq ($(AOCL_TEST_COVERAGE),1)
        LZ4HC_CFLAGS += -DNDEBUG
    endif
endif

# Symbol prefixing support
ifeq ($(AOCL_LLC_PREFIX),1)
    LZ4HC_CFLAGS += -DAOCL_LLC_PREFIX
endif

# Windows DLL export (shared build) -- matches CMake set_property pattern.
# LZ4HC sources reuse the LZ4_DLL_EXPORT gate (see algos/lz4/lz4hc.h).
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
    LZ4HC_CFLAGS += -DLZ4_DLL_EXPORT=1
endif
endif

# Add include path for aoclPrefix.h (in algos/common)

# Per-file flags for LZ4HC sources
$(OBJ_DIR)/algos/lz4/%.o: CFLAGS += $(LZ4HC_CFLAGS)

# Installation headers
INSTALL_HEADERS += algos/lz4/lz4hc.h

endif # AOCL_EXCLUDE_LZ4HC
# ---- END INLINE: mk/algos/lz4hc.mk ----

# ---- BEGIN INLINE: mk/algos/lzma.mk ----
# ==============================================================================
# LZMA Module
# ==============================================================================
# LZMA compression algorithm integration
# ==============================================================================

ifneq ($(AOCL_EXCLUDE_LZMA),1)

# LZMA source files
LZMA_SOURCES := algos/lzma/code_alignment.c \
                algos/lzma/LzFind.c \
                algos/lzma/LzmaDec.c \
                algos/lzma/LzmaEnc.c \
                algos/lzma/Alloc.c

# LZMA header files
LZMA_HEADERS := algos/lzma/Alloc.h \
                algos/lzma/LzmaDec.h \
                algos/lzma/LzmaEnc.h \
                algos/lzma/LzFind.h \
                algos/lzma/7zTypes.h

# Add experimental xz-utils LZMA API support if enabled
ifeq ($(AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL),1)
    LZMA_SOURCES += algos/lzma/xzlzma/XzLzma.c
    LZMA_HEADERS += algos/lzma/xzlzma/XzLzma.h \
                    algos/lzma/xzlzma/XzVersion.h \
                    algos/lzma/lzma.h
endif

# Add to global lists
ALL_SOURCES += $(LZMA_SOURCES)
ALL_HEADERS += $(LZMA_HEADERS)

# LZMA-specific flags
LZMA_CFLAGS :=


# Disable asserts for testing
ifeq ($(BUILD_TYPE),Debug)
    ifeq ($(AOCL_TEST_COVERAGE),1)
        LZMA_CFLAGS += -DNDEBUG
    endif
endif

# Windows DLL export (shared build) -- matches CMake set_property pattern
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
    LZMA_CFLAGS += -DLZMA_DLL_EXPORT=1
endif
endif

# Per-file flags for LZMA sources
$(OBJ_DIR)/algos/lzma/%.o: CFLAGS += $(LZMA_CFLAGS)

# Installation headers
INSTALL_HEADERS += algos/lzma/7zTypes.h \
                   algos/lzma/Alloc.h \
                   algos/lzma/LzmaDec.h \
                   algos/lzma/LzmaEnc.h

# Install xz-utils headers if experimental API enabled
ifeq ($(AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL),1)
    INSTALL_HEADERS += algos/lzma/lzma.h
    # xzlzma directory will be installed separately
    INSTALL_DIRS += algos/lzma/xzlzma
endif

endif # AOCL_EXCLUDE_LZMA
# ---- END INLINE: mk/algos/lzma.mk ----

# ---- BEGIN INLINE: mk/algos/snappy.mk ----
# ==============================================================================
# SNAPPY Module
# ==============================================================================
# SNAPPY compression algorithm integration
# ==============================================================================

ifneq ($(AOCL_EXCLUDE_SNAPPY),1)

# SNAPPY source files
SNAPPY_SOURCES := algos/snappy/code_alignment.cc \
                  algos/snappy/snappy-sinksource.cc \
                  algos/snappy/snappy-stubs-internal.cc \
                  algos/snappy/snappy.cc \
                  algos/snappy/snappy-c.cc

# SNAPPY header files
SNAPPY_HEADERS := algos/snappy/snappy.h \
                  algos/snappy/snappy-c.h \
                  algos/snappy/snappy-stubs-public.h \
                  algos/snappy/snappy-sinksource.h

# Add to global lists
ALL_SOURCES += $(SNAPPY_SOURCES)
ALL_HEADERS += $(SNAPPY_HEADERS)

# SNAPPY-specific flags
SNAPPY_CXXFLAGS :=

# SSE4.1 / hardware CRC32 (supported by GCC and Clang on both Unix and MinGW)
SNAPPY_CXXFLAGS += -msse4.1

# Compiler capability defines (GCC/Clang built-ins; available on MinGW too)
SNAPPY_CXXFLAGS += -DHAVE_ATTRIBUTE_ALWAYS_INLINE=1
SNAPPY_CXXFLAGS += -DHAVE_BUILTIN_CTZ=1
SNAPPY_CXXFLAGS += -DHAVE_BUILTIN_EXPECT=1
SNAPPY_CXXFLAGS += -DHAVE_BUILTIN_PREFETCH=1
SNAPPY_CXXFLAGS += -DSNAPPY_HAVE_X86_CRC32=1

# sys/uio.h is Unix-only
ifeq ($(OS_TYPE),unix)
    SNAPPY_HAS_SYS_UIO := $(shell printf '%s\n' '\#include <sys/uio.h>' | $(CC) -E - >/dev/null 2>&1; if [ $$? -eq 0 ]; then echo 1; else echo 0; fi)
    ifeq ($(SNAPPY_HAS_SYS_UIO),1)
        SNAPPY_CXXFLAGS += -DHAVE_SYS_UIO_H
    endif
endif

# Optimization flags
ifeq ($(SNAPPY_MATCH_SKIP_OPT),1)
    SNAPPY_CXXFLAGS += -DAOCL_SNAPPY_MATCH_SKIP_OPT
endif

ifeq ($(SNAPPY_HIGH_COMPRESSION),1)
    SNAPPY_CXXFLAGS += -DAOCL_SNAPPY_HIGH_COMPRESSION
endif

ifeq ($(SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS),1)
    SNAPPY_CXXFLAGS += -DAOCL_SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS
endif

# Fast decompression mode
ifneq ($(AOCL_DECOMPRESS_FAST),OFF)
    SNAPPY_CXXFLAGS += -DAOCL_DECOMPRESS_FAST=$(AOCL_DECOMPRESS_FAST)
endif


# Disable asserts for testing
ifeq ($(BUILD_TYPE),Debug)
    ifeq ($(AOCL_TEST_COVERAGE),1)
        SNAPPY_CXXFLAGS += -DNDEBUG
    endif
endif

# Windows DLL export (shared build) -- matches CMake set_property pattern
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
    SNAPPY_CXXFLAGS += -DSNAPPY_DLL_EXPORT=1
endif
endif

# Per-file flags for SNAPPY sources
$(OBJ_DIR)/algos/snappy/%.o: CXXFLAGS += $(SNAPPY_CXXFLAGS)

# Installation headers
INSTALL_HEADERS += algos/snappy/snappy-stubs-public.h \
                   algos/snappy/snappy-sinksource.h \
                   algos/snappy/snappy.h \
                   algos/snappy/snappy-c.h

endif # AOCL_EXCLUDE_SNAPPY
# ---- END INLINE: mk/algos/snappy.mk ----

# ---- BEGIN INLINE: mk/algos/zlib.mk ----
# ==============================================================================
# ZLIB Module
# ==============================================================================
# ZLIB compression algorithm integration
# Includes zlib-ng optimized sources
# ==============================================================================

ifneq ($(AOCL_EXCLUDE_ZLIB),1)

# ZLIB source files
ZLIB_SOURCES := algos/zlib/code_alignment.c \
                algos/zlib/adler32.c \
                algos/zlib/adler32_x86.c \
                algos/zlib/compress.c \
                algos/zlib/crc32.c \
                algos/zlib/crc32_x86_pclmul.c \
                algos/zlib/crc32_x86_vpclmulqdq.c \
                algos/zlib/deflate.c \
                algos/zlib/deflate_medium.c \
                algos/zlib/deflate_quick.c \
                algos/zlib/gzclose.c \
                algos/zlib/gzlib.c \
                algos/zlib/gzread.c \
                algos/zlib/gzwrite.c \
                algos/zlib/infback.c \
                algos/zlib/inffast.c \
                algos/zlib/inflate.c \
                algos/zlib/inftrees.c \
                algos/zlib/longest_match_x86.c \
                algos/zlib/slide_hash_x86.c \
                algos/zlib/trees.c \
                algos/zlib/uncompr.c \
                algos/zlib/zutil.c \
                algos/zlib/aocl_zlib_utils.c

# ZLIB-NG optimized sources
ZLIBNG_SOURCES := algos/zlib/zlib-ng/arch/x86/chunkset_sse2.c
ifeq ($(AOCL_MAX_ISA_LEVEL_NUM),4)
    ZLIBNG_SOURCES += algos/zlib/zlib-ng/arch/x86/chunkset_avx512.c
endif

# ZLIB header files
ZLIB_HEADERS := algos/zlib/zlib.h \
                algos/zlib/zconf.h

# Add to global lists
ALL_SOURCES += $(ZLIB_SOURCES) $(ZLIBNG_SOURCES)
ALL_HEADERS += $(ZLIB_HEADERS)

# ZLIB-specific flags
ZLIB_CFLAGS := -DHAVE_BUILTIN_CTZ=1 \
               -DHAVE_BUILTIN_EXPECT=1

# ZLIB-NG specific flags
ZLIBNG_CFLAGS := -DHAVE_ATTRIBUTE_ALIGNED=1
# Hidden visibility attribute (ELF only; PE/COFF rejects it with -Werror=attributes)
ifneq ($(OS_TYPE),windows)
    ZLIB_HAS_HIDDEN := $(shell printf '%s\n' 'int __attribute__((visibility("hidden"))) foo;' | $(CC) -x c -fsyntax-only - >/dev/null 2>&1; if [ $$? -eq 0 ]; then echo 1; else echo 0; fi)
    ifeq ($(ZLIB_HAS_HIDDEN),1)
        ZLIB_CFLAGS   += -DHAVE_HIDDEN
        ZLIBNG_CFLAGS += -DHAVE_HIDDEN
    endif
endif

# Debug flags for Debug build (if not testing)
ifeq ($(BUILD_TYPE),Debug)
    ifneq ($(AOCL_TEST_COVERAGE),1)
        ZLIB_CFLAGS += -DZLIB_DEBUG
    endif
endif


# Symbol prefixing support
ifeq ($(AOCL_LLC_PREFIX),1)
    ZLIB_CFLAGS += -DAOCL_LLC_PREFIX
endif

# Windows DLL export (shared build) -- matches CMake set_property pattern.
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
    ZLIB_CFLAGS   += -DZLIB_DLL_EXPORT=1
endif
endif

# Per-file flags for ZLIB sources (need to find aoclPrefix.h in algos/common)
$(OBJ_DIR)/algos/zlib/%.o: CFLAGS += $(ZLIB_CFLAGS) -I$(ROOT_DIR)/algos/zlib

# Per-file flags for ZLIB-NG sources (need additional include paths)
$(OBJ_DIR)/algos/zlib/zlib-ng/%.o: CFLAGS += $(ZLIBNG_CFLAGS) -I$(ROOT_DIR)/algos/zlib/zlib-ng -I$(ROOT_DIR)/algos/zlib

# AVX-512 sources need special flags (only when ISA level includes AVX512)
AVX512_SOURCES := algos/zlib/crc32_x86_vpclmulqdq.c \
                  algos/zlib/zlib-ng/arch/x86/chunkset_avx512.c

ifeq ($(AOCL_MAX_ISA_LEVEL_NUM),4)
$(OBJ_DIR)/algos/zlib/crc32_x86_vpclmulqdq.o: CFLAGS += -mpclmul -mvpclmulqdq -mavx512f
$(OBJ_DIR)/algos/zlib/zlib-ng/arch/x86/chunkset_avx512.o: CFLAGS += -mavx512f -mavx512bw
endif

# Installation headers
INSTALL_HEADERS += algos/zlib/zconf.h \
                   algos/zlib/zlib.h

endif # AOCL_EXCLUDE_ZLIB
# ---- END INLINE: mk/algos/zlib.mk ----

# ---- BEGIN INLINE: mk/algos/zstd.mk ----
# ==============================================================================
# ZSTD Module
# ==============================================================================
# ZSTD compression algorithm integration
# Includes template generation for optimized variants
# ==============================================================================

ifneq ($(AOCL_EXCLUDE_ZSTD),1)

# ZSTD common sources
ZSTD_COMMON_SOURCES := algos/zstd/lib/common/code_alignment.c \
                       algos/zstd/lib/common/zstd_common.c \
                       algos/zstd/lib/common/fse_decompress.c \
                       algos/zstd/lib/common/xxhash.c \
                       algos/zstd/lib/common/error_private.c \
                       algos/zstd/lib/common/entropy_common.c \
                       algos/zstd/lib/common/pool.c \
                       algos/zstd/lib/common/debug.c \
                       algos/zstd/lib/common/threading.c

# ZSTD compression sources
ZSTD_COMPRESS_SOURCES := algos/zstd/lib/compress/zstd_compress.c \
                         algos/zstd/lib/compress/zstd_compress_literals.c \
                         algos/zstd/lib/compress/zstd_compress_sequences.c \
                         algos/zstd/lib/compress/zstd_compress_superblock.c \
                         algos/zstd/lib/compress/zstdmt_compress.c \
                         algos/zstd/lib/compress/zstd_double_fast.c \
                         algos/zstd/lib/compress/zstd_fast.c \
                         algos/zstd/lib/compress/zstd_lazy.c \
                         algos/zstd/lib/compress/zstd_ldm.c \
                         algos/zstd/lib/compress/zstd_opt.c \
                         algos/zstd/lib/compress/fse_compress.c \
                         algos/zstd/lib/compress/huf_compress.c \
                         algos/zstd/lib/compress/hist.c \
                         algos/zstd/lib/compress/zstd_preSplit.c

# ZSTD decompression sources
ZSTD_DECOMPRESS_SOURCES := algos/zstd/lib/decompress/zstd_decompress.c \
                           algos/zstd/lib/decompress/huf_decompress.c \
                           algos/zstd/lib/decompress/huf_decompress_amd64.S \
                           algos/zstd/lib/decompress/zstd_ddict.c \
                           algos/zstd/lib/decompress/zstd_decompress_block.c

# ZSTD dictionary builder sources
ZSTD_DICTBUILDER_SOURCES := algos/zstd/lib/dictBuilder/cover.c \
                            algos/zstd/lib/dictBuilder/divsufsort.c \
                            algos/zstd/lib/dictBuilder/fastcover.c \
                            algos/zstd/lib/dictBuilder/zdict.c

# Combine all ZSTD sources
ZSTD_SOURCES := $(ZSTD_COMMON_SOURCES) \
                $(ZSTD_COMPRESS_SOURCES) \
                $(ZSTD_DECOMPRESS_SOURCES) \
                $(ZSTD_DICTBUILDER_SOURCES)

# ZSTD header files
ZSTD_HEADERS := algos/zstd/lib/zstd.h \
                algos/zstd/lib/zdict.h \
                algos/zstd/lib/zstd_errors.h

# Add to global lists
ALL_SOURCES += $(ZSTD_SOURCES)
ALL_HEADERS += $(ZSTD_HEADERS)

# ZSTD-specific flags
ZSTD_CFLAGS :=

# Optimization flags
ifeq ($(AOCL_ZSTD_SEARCH_SKIP_OPT),1)
    ZSTD_CFLAGS += -DAOCL_ZSTD_SEARCH_SKIP_OPT
endif

# Fast decompression mode
ifneq ($(AOCL_DECOMPRESS_FAST),OFF)
    ZSTD_CFLAGS += -DAOCL_DECOMPRESS_FAST=$(AOCL_DECOMPRESS_FAST)
    # Enable wildcopy long for FDS >= 1
    ZSTD_CFLAGS += -DAOCL_ZSTD_WILDCOPY_LONG
endif

# Fast compression mode
ifneq ($(AOCL_COMPRESS_FAST),OFF)
    ZSTD_CFLAGS += -DAOCL_COMPRESS_FAST=$(AOCL_COMPRESS_FAST)
endif

# Native multi-threading support
ifeq ($(NATIVE_ENABLE_THREADS),1)
    ZSTD_CFLAGS += -DZSTD_MULTITHREAD
endif

# Debug level for Debug build
ifeq ($(BUILD_TYPE),Debug)
    ZSTD_CFLAGS += -DDEBUGLEVEL=1
endif

# Symbol prefixing support
ifeq ($(AOCL_LLC_PREFIX),1)
    ZSTD_CFLAGS += -DAOCL_LLC_PREFIX
endif

# Windows DLL export (shared build) -- matches CMake set_property pattern.
# FSE_DLL_EXPORT / XXH_EXPORT are added by CMake only for TEST_COVERAGE_THIRD_PARTY
# or BUILD_UTILITY paths, which are not wired in this Make build yet.
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
    ZSTD_CFLAGS += -DZSTD_DLL_EXPORT=1
endif
endif

# Per-file flags for ZSTD sources
$(OBJ_DIR)/algos/zstd/%.o: CFLAGS += $(ZSTD_CFLAGS)

# Installation headers
INSTALL_HEADERS += algos/zstd/lib/zstd.h \
                   algos/zstd/lib/zdict.h \
                   algos/zstd/lib/zstd_errors.h

endif # AOCL_EXCLUDE_ZSTD
# ---- END INLINE: mk/algos/zstd.mk ----

# ---- BEGIN INLINE: mk/algos/common.mk ----
# ==============================================================================
# Common Module
# ==============================================================================
# Common headers used across all algorithms
# ==============================================================================

# Common header files (always included)
COMMON_SOURCES := algos/common/aoclHashChain.h

COMMON_HEADERS := $(GEN_AOCL_ALGO_OPT) \
                  algos/common/aoclFds.h \
                  algos/common/aoclPrefix.h \
                  algos/common/aoclAlgoLog.h

# Add to global lists
ALL_SOURCES += $(COMMON_SOURCES)
ALL_HEADERS += $(COMMON_HEADERS)

# These headers are always installed
INSTALL_HEADERS += $(GEN_AOCL_ALGO_OPT) \
                   algos/common/aoclFds.h \
                   algos/common/aoclPrefix.h
# ---- END INLINE: mk/algos/common.mk ----

# ---- BEGIN INLINE: mk/features/threads.mk ----
# ==============================================================================
# Threading Support Module
# ==============================================================================
# Handles OpenMP and native pthread multi-threading
# ==============================================================================

# ==============================================================================
# AOCL MULTI-THREADING (OpenMP)
# ==============================================================================

ifeq ($(AOCL_ENABLE_THREADS),1)

# Add threading sources
THREADS_SOURCES := threads/code_alignment.c \
                   threads/threads.c

THREADS_HEADERS := threads/threads.h

ALL_SOURCES += $(THREADS_SOURCES)
ALL_HEADERS += $(THREADS_HEADERS)

# OpenMP flags
THREADS_CFLAGS := -fopenmp
THREADS_CXXFLAGS := -fopenmp
THREADS_LDFLAGS := -fopenmp

# Check if we need to link OpenMP library explicitly
ifeq ($(OS_TYPE),unix)
    # On Linux, check if libomp exists
    LIBOMP_PATH := $(shell $(CC) -print-file-name=libomp.so 2>/dev/null)
    ifneq ($(LIBOMP_PATH),libomp.so)
        # libomp found, link it
        LIBS += -lomp
    endif
else ifeq ($(OS_TYPE),windows)
    # GCC/MinGW auto-links libgomp via -fopenmp; Clang/MinGW needs explicit -lomp
    ifeq ($(CC_ID),clang)
        LIBS += -lomp
    endif
endif

# Add threading flags
CFLAGS += $(THREADS_CFLAGS)
CXXFLAGS += $(THREADS_CXXFLAGS)
LDFLAGS += $(THREADS_LDFLAGS)

# Add AOCL_ENABLE_THREADS definition to all sources
ALL_CFLAGS += -DAOCL_ENABLE_THREADS
ALL_CXXFLAGS += -DAOCL_ENABLE_THREADS

endif # AOCL_ENABLE_THREADS

# ==============================================================================
# NATIVE MULTI-THREADING (pthread)
# ==============================================================================

ifeq ($(NATIVE_ENABLE_THREADS),1)

# Link pthread library (MSYS2/MinGW provides POSIX threads via winpthreads)
LIBS += -lpthread

endif # NATIVE_ENABLE_THREADS
# ---- END INLINE: mk/features/threads.mk ----

# ---- BEGIN INLINE: mk/algos/utils.mk ----
# ==============================================================================
# Utils Module
# ==============================================================================
# Utility sources for testing
# Only included when AOCL_TEST_COVERAGE is enabled
# ==============================================================================

# Utils source files
UTILS_SOURCES := utils/code_alignment.cpp \
                 utils/utils.cpp \
                 utils/dispatcher.cpp \
                 utils/cpu_features.cpp

# Utils header files
UTILS_HEADERS := utils/utils.h \
                 utils/dispatcher.h

# Add to global lists
ALL_SOURCES += $(UTILS_SOURCES)
ALL_HEADERS += $(UTILS_HEADERS)

# Utils-specific flags
UTILS_CXXFLAGS :=



# Per-file flags for utils sources
$(OBJ_DIR)/utils/%.o: CXXFLAGS += $(UTILS_CXXFLAGS)
# ---- END INLINE: mk/algos/utils.mk ----

# ==============================================================================
# PUBLIC HEADER INCLUDE PATHS (for benchmark/tests)
# ==============================================================================

PUBLIC_INCLUDE_DIRS := -I$(ROOT_DIR) \
                       -I$(ROOT_DIR)/api \
                       -I$(ROOT_DIR)/algos/common \
                       -I$(ROOT_DIR)/utils \
                       -I$(GEN_DIR)/algos/common

ifneq ($(AOCL_EXCLUDE_BZIP2),1)
    PUBLIC_INCLUDE_DIRS += -I$(ROOT_DIR)/algos/bzip2
endif
ifneq ($(AOCL_EXCLUDE_LZ4),1)
    PUBLIC_INCLUDE_DIRS += -I$(ROOT_DIR)/algos/lz4
endif
ifneq ($(AOCL_EXCLUDE_LZMA),1)
    PUBLIC_INCLUDE_DIRS += -I$(ROOT_DIR)/algos/lzma
endif
ifneq ($(AOCL_EXCLUDE_SNAPPY),1)
    PUBLIC_INCLUDE_DIRS += -I$(ROOT_DIR)/algos/snappy
endif
ifneq ($(AOCL_EXCLUDE_ZLIB),1)
    PUBLIC_INCLUDE_DIRS += -I$(ROOT_DIR)/algos/zlib
endif
ifneq ($(AOCL_EXCLUDE_ZSTD),1)
    PUBLIC_INCLUDE_DIRS += -I$(ROOT_DIR)/algos/zstd/lib
endif

# ==============================================================================
# COMPILER FLAGS
# ==============================================================================

# Include compiler-specific flags (inlined)
ifeq ($(CC_ID),gcc)
# ---- BEGIN INLINE: mk/compiler/gcc.mk ----
# ==============================================================================
# GCC Compiler-Specific Flags
# ==============================================================================

# GCC-specific optimizations
GCC_CFLAGS :=
GCC_CXXFLAGS :=

# Per-file compiler flags to recover the ZLIB and ZSTD performance lost for GCC >=15
ifeq ($(shell major=$(CC_MAJOR); [ -n "$$major" ] && [ "$$major" -ge 15 ] && echo 1),1)
$(OBJ_DIR)/algos/zlib/zlib-ng/arch/x86/chunkset_avx512.o: CFLAGS += -fno-schedule-insns2 -fno-code-hoisting
$(OBJ_DIR)/algos/zstd/lib/decompress/zstd_decompress_block.o: CFLAGS += -fno-tree-sink
endif

# TODO: Future implementation
# # Code coverage support
# ifeq ($(CODE_COVERAGE),1)
#     GCC_CFLAGS += --coverage -fprofile-abs-path
#     GCC_CXXFLAGS += --coverage -fprofile-abs-path
#     LDFLAGS += --coverage
# endif

# TODO: Future implementation
# # Address Sanitizer support (Linux only)
# ifeq ($(ASAN),1)
#     ifeq ($(PLATFORM),linux)
#         ifeq ($(BUILD_TYPE),Debug)
#             GCC_CFLAGS += -fsanitize=address -O0 -gdwarf-5 -g3
#             GCC_CXXFLAGS += -fsanitize=address -O0 -gdwarf-5 -g3
#             LDFLAGS += -fsanitize=address
#         endif
#     endif
# endif

# Add GCC-specific flags to global flags
CFLAGS += $(GCC_CFLAGS)
CXXFLAGS += $(GCC_CXXFLAGS)
# ---- END INLINE: mk/compiler/gcc.mk ----
else ifeq ($(CC_ID),clang)
# ---- BEGIN INLINE: mk/compiler/clang.mk ----
# ==============================================================================
# Clang Compiler-Specific Flags
# ==============================================================================

# Clang-specific optimizations
CLANG_CFLAGS :=
CLANG_CXXFLAGS :=

# Clang doesn't support per-function optimization level controls
# So we set -O2 at file level for LZ4 (already handled in lz4.mk)

# TODO: Future implementation
# # Code coverage support (llvm-cov)
# ifeq ($(CODE_COVERAGE),1)
#     CLANG_CFLAGS += -fprofile-instr-generate -fcoverage-mapping
#     CLANG_CXXFLAGS += -fprofile-instr-generate -fcoverage-mapping
#     LDFLAGS += -fprofile-instr-generate -fcoverage-mapping
# endif

# TODO: Future implementation
# # Address Sanitizer support (Linux only)
# ifeq ($(ASAN),1)
#     ifeq ($(PLATFORM),linux)
#         ifeq ($(BUILD_TYPE),Debug)
#             CLANG_CFLAGS += -fsanitize=address -O0 -gdwarf-5 -g3
#             CLANG_CXXFLAGS += -fsanitize=address -O0 -gdwarf-5 -g3
#             LDFLAGS += -fsanitize=address
#         endif
#     endif
# endif

# TODO: Future implementation
# # Fuzzer support (Clang only, Linux only)
# ifeq ($(AOCL_TEST_FUZZER),1)
#     ifeq ($(PLATFORM),linux)
#         CLANG_CFLAGS += -fsanitize=fuzzer-no-link
#         CLANG_CXXFLAGS += -fsanitize=fuzzer-no-link
#     endif
# endif

# Add Clang-specific flags to global flags
CFLAGS += $(CLANG_CFLAGS)
CXXFLAGS += $(CLANG_CXXFLAGS)
# ---- END INLINE: mk/compiler/clang.mk ----
endif

# Build type specific flags
ifeq ($(BUILD_TYPE),Debug)
    CFLAGS += -g -O0 -DDEBUG
    CXXFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O3 -DNDEBUG
    CXXFLAGS += -O3 -DNDEBUG
endif

# Add build version with proper quote escaping
CFLAGS += -DAOCL_BUILD_VERSION="\"$(BUILD_VERSION)\""
CXXFLAGS += -DAOCL_BUILD_VERSION="\"$(BUILD_VERSION)\""

# Add dynamic dispatcher flag (always enabled)
CFLAGS += -DAOCL_DYNAMIC_DISPATCHER
CXXFLAGS += -DAOCL_DYNAMIC_DISPATCHER

# Pass AOCL_MAX_ISA_LEVEL to all source files
CFLAGS += -DAOCL_MAX_ISA_LEVEL=$(AOCL_MAX_ISA_LEVEL_NUM)
CXXFLAGS += -DAOCL_MAX_ISA_LEVEL=$(AOCL_MAX_ISA_LEVEL_NUM)

# Match CMake compile definitions for shared library builds and strict warnings
ifneq ($(BUILD_STATIC_LIBS),1)
    CFLAGS += -Daocl_compression_EXPORTS
    CXXFLAGS += -Daocl_compression_EXPORTS
endif
ifeq ($(ENABLE_STRICT_WARNINGS),1)
    CFLAGS += -DENABLE_STRICT_WARNINGS
    CXXFLAGS += -DENABLE_STRICT_WARNINGS
endif

# Add generated-headers include path (template outputs under build tree)
CFLAGS += -I$(GEN_DIR)
CXXFLAGS += -I$(GEN_DIR)
CFLAGS += -I$(GEN_DIR)/algos/common
CXXFLAGS += -I$(GEN_DIR)/algos/common

# Add unit test flag when test coverage is enabled
ifeq ($(AOCL_TEST_COVERAGE),1)
    CFLAGS += -DAOCL_UNIT_TEST
    CXXFLAGS += -DAOCL_UNIT_TEST
endif

# Combine all flags
CFLAGS += $(ALL_CFLAGS)
CXXFLAGS += $(ALL_CXXFLAGS)

# ==============================================================================
# OBJECT FILES
# ==============================================================================

# Convert source files to object files
C_SOURCES := $(filter %.c,$(ALL_SOURCES))
CXX_SOURCES := $(filter %.cpp %.cc,$(ALL_SOURCES))
ASM_SOURCES := $(filter %.S,$(ALL_SOURCES))

C_OBJECTS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
CXX_OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(patsubst %.cc,$(OBJ_DIR)/%.o,$(CXX_SOURCES)))
ASM_OBJECTS := $(patsubst %.S,$(OBJ_DIR)/%.o,$(ASM_SOURCES))

ALL_OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS) $(ASM_OBJECTS)

# Dependency files
DEPS := $(patsubst $(OBJ_DIR)/%.o,$(DEP_DIR)/%.d,$(ALL_OBJECTS))

# ==============================================================================
# GENERATE COMPILATION RULES
# ==============================================================================

# Generate explicit rules for each C source file
$(foreach src,$(C_SOURCES),$(eval $(call COMPILE_C_RULE,$(basename $(src)))))

# Generate explicit rules for each C++ source file (.cpp)
$(foreach src,$(filter %.cpp,$(CXX_SOURCES)),$(eval $(call COMPILE_CPP_RULE,$(basename $(src)))))

# Generate explicit rules for each C++ source file (.cc)
$(foreach src,$(filter %.cc,$(CXX_SOURCES)),$(eval $(call COMPILE_CC_RULE,$(basename $(src)))))

# Generate explicit rules for each assembly file
$(foreach src,$(ASM_SOURCES),$(eval $(call COMPILE_ASM_RULE,$(basename $(src)))))

# ==============================================================================
# MAIN TARGETS
# ==============================================================================

.PHONY: all clean install uninstall help

# Default target
all: $(LIB_TARGET)
	@echo ""
	@echo "=========================================="
	@echo "Build completed successfully!"
	@echo "Library: $(LIB_TARGET)"
	@echo "Type: $(LIB_TYPE)"
	@echo "=========================================="

# Build library.
# Windows shared builds emit two artifacts from a single link: the DLL and the
# MinGW import library (.dll.a, via -Wl,--out-implib). Declare both with GNU
# Make grouped-target syntax (&:, Make 4.3+) so deleting either output forces
# a re-link instead of leaving a stale pair.
ifeq ($(LIB_LINK_TARGET),$(LIB_TARGET))
$(LIB_TARGET): $(ALL_OBJECTS) | $(LIB_DIR)
	@echo "Linking $(LIB_TYPE) library: $@"
ifeq ($(BUILD_STATIC_LIBS),1)
	$(AR) rcs $@ $^
else
	$(CXX) $(LDFLAGS) $(SHARED_LDFLAGS) -o $@ $^ $(LIBS)
endif
	@mkdir -p $(ROOT_DIR)/lib
	@cp $@ $(ROOT_DIR)/lib/
else
$(LIB_TARGET) $(LIB_LINK_TARGET) &: $(ALL_OBJECTS) | $(LIB_DIR)
	@echo "Linking $(LIB_TYPE) library: $(LIB_TARGET)"
	$(CXX) $(LDFLAGS) $(SHARED_LDFLAGS) -o $(LIB_TARGET) $(ALL_OBJECTS) $(LIBS)
	@mkdir -p $(ROOT_DIR)/lib
	@cp $(LIB_TARGET) $(ROOT_DIR)/lib/
	@cp $(LIB_LINK_TARGET) $(ROOT_DIR)/lib/
endif

# ==============================================================================
# TEMPLATE GENERATION
# ==============================================================================

# Template inputs and stamp file
TEMPLATE_SOURCES := $(wildcard mk/templates/*.in) $(wildcard algos/lz4/*.in) $(wildcard algos/zstd/lib/compress/*.in) $(wildcard algos/zstd/lib/decompress/*.in) algos/common/aoclAlgoOpt.h.in
TEMPLATE_STAMP   := $(BUILD_DIR)/.templates.stamp
.PHONY: templates

$(GEN_AOCL_ALGO_OPT): algos/common/aoclAlgoOpt.h.in config.mk | $(GEN_DIR)/algos/common
	@echo "Generating aoclAlgoOpt.h (LZ4_FRAME_FORMAT_SUPPORT=$(LZ4_FRAME_FORMAT_SUPPORT))..."
ifeq ($(LZ4_FRAME_FORMAT_SUPPORT),1)
	@sed 's|@AOCL_LZ4HC_OPT@|// #define AOCL_LZ4HC_OPT|g' $< > $@
else
	@sed 's|@AOCL_LZ4HC_OPT@|#define AOCL_LZ4HC_OPT|g' $< > $@
endif

$(GEN_DIR)/algos/common: | $(GEN_DIR)
	@mkdir -p $@

# Internal rule: generate all templates and update stamp
$(TEMPLATE_STAMP): $(TEMPLATE_SOURCES) config.mk $(GEN_AOCL_ALGO_OPT) mk/templates/lz4_templates.mk mk/templates/zstd_templates.mk | $(BUILD_DIR)
	@echo "Generating templates..."
	@$(MAKE) -f mk/templates/lz4_templates.mk
	@$(MAKE) -f mk/templates/zstd_templates.mk
	@touch $@
	@echo "✓ Template generation complete"
# User-facing target to force template regeneration
templates: $(TEMPLATE_STAMP)
# Ensure templates are generated before building objects
$(ALL_OBJECTS): $(TEMPLATE_STAMP) | $(OBJ_DIR) $(DEP_DIR)

# Ensure object directories exist
$(C_OBJECTS) $(CXX_OBJECTS) $(ASM_OBJECTS): | $(OBJ_DIR) $(DEP_DIR)

# ==============================================================================
# BENCHMARK (always built, like CMake)
# ==============================================================================

# Always build benchmark executable (matches CMake behavior)
BENCH_SOURCES := test/codec_bench.c test/codec_bench.h test/codec_native_api_bench.c
BENCH_BIN := $(BUILD_DIR)/aocl_compression_bench$(EXE_EXT)

# Add IPP codec bench for Linux
ifeq ($(PLATFORM),linux)
    BENCH_SOURCES += test/ipp_codec_bench.c
endif

.PHONY: bench

bench: $(BENCH_BIN)

# Benchmark-specific flags (include algorithm exclusions)
BENCH_CFLAGS := -O2 -DCCompiler='"$(CC)"' -DCXXCompiler='"$(CXX)"' \
                -DCFLAGS_SET1='"-O2"' -DCFLAGS_SET2='"-O2"' \
                -D_DEFAULT_SOURCE -D_BSD_SOURCE -DAOCL_DYNAMIC_DISPATCHER \
                $(EXE_CFLAGS)

# Benchmark include paths (align with CMake benchmark header visibility)
BENCH_INCLUDES := $(PUBLIC_INCLUDE_DIRS)

# Add algorithm exclusion flags
ifeq ($(AOCL_EXCLUDE_BZIP2),1)
    BENCH_CFLAGS += -DAOCL_EXCLUDE_BZIP2
endif
ifeq ($(AOCL_EXCLUDE_LZ4),1)
    BENCH_CFLAGS += -DAOCL_EXCLUDE_LZ4
endif
ifeq ($(AOCL_EXCLUDE_LZ4HC),1)
    BENCH_CFLAGS += -DAOCL_EXCLUDE_LZ4HC
endif
ifeq ($(AOCL_EXCLUDE_LZMA),1)
    BENCH_CFLAGS += -DAOCL_EXCLUDE_LZMA
endif
ifeq ($(AOCL_EXCLUDE_SNAPPY),1)
    BENCH_CFLAGS += -DAOCL_EXCLUDE_SNAPPY
endif
ifeq ($(AOCL_EXCLUDE_ZLIB),1)
    BENCH_CFLAGS += -DAOCL_EXCLUDE_ZLIB
endif
ifeq ($(AOCL_EXCLUDE_ZSTD),1)
    BENCH_CFLAGS += -DAOCL_EXCLUDE_ZSTD
endif

# Symbol prefixing support
ifeq ($(AOCL_LLC_PREFIX),1)
    BENCH_CFLAGS += -DAOCL_LLC_PREFIX
endif

# Match CMake testbench compile definitions
ifeq ($(AOCL_ENABLE_THREADS),1)
    BENCH_CFLAGS += -DAOCL_ENABLE_THREADS
endif
ifeq ($(NATIVE_ENABLE_THREADS),1)
    BENCH_CFLAGS += -DNATIVE_ENABLE_THREADS
endif
ifeq ($(AOCL_ENABLE_LOG_FEATURE),1)
    BENCH_CFLAGS += -DAOCL_ENABLE_LOG_FEATURE
endif
ifeq ($(BUILD_TYPE),Debug)
    BENCH_CFLAGS += -DDEBUG_ASSERT_ENABLED
endif
# Apply the same Release-mode hardening as the library (see mk/platform.mk).
ifeq ($(BUILD_TYPE),Release)
    BENCH_CFLAGS += $(SECURITY_FLAGS) -D_FORTIFY_SOURCE=2
endif

# Bench link uses executable and platform linker hardening flags.
ifeq ($(OS_TYPE),windows)
BENCH_LINK_FLAGS := $(LIB_LINK_TARGET) $(LDFLAGS) -lstdc++ $(EXE_LDFLAGS)
else
BENCH_LINK_FLAGS := $(LIB_LINK_TARGET) $(LDFLAGS) -Wl,-rpath,$(LIB_DIR) -ldl -lstdc++ $(EXE_LDFLAGS)
endif

$(BENCH_BIN): $(BENCH_SOURCES) $(LIB_LINK_TARGET)
	@mkdir -p $(dir $@)
	@echo "Building benchmark: $(notdir $@)"
	@$(CC) $(BENCH_CFLAGS) $(BENCH_INCLUDES) \
		test/codec_bench.c test/codec_native_api_bench.c $(if $(filter linux,$(PLATFORM)),test/ipp_codec_bench.c) \
		-o $@ \
		$(BENCH_LINK_FLAGS) $(LIBS)
ifeq ($(OS_TYPE),windows)
ifneq ($(BUILD_STATIC_LIBS),1)
	@cp -f $(LIB_TARGET) $(dir $@)
endif
endif

# Build benchmark by default
all: $(LIB_TARGET) $(BENCH_BIN)

# ==============================================================================
# TESTING
# ==============================================================================

ifeq ($(AOCL_TEST_COVERAGE),1)
    include mk/test.mk

    .PHONY: test-build

    # Build test executables as part of default build when coverage is enabled
    test-build: $(GTEST_BINS) $(BENCH_BIN)
    # TODO: Future implementation
    # ifeq ($(TEST_COVERAGE_THIRD_PARTY),1)
    # test-build: $(THIRD_PARTY_TESTS)
    # endif

    all: test-build
endif

# ==============================================================================
# INSTALLATION
# ==============================================================================

include mk/install.mk

# ==============================================================================
# CLEAN
# ==============================================================================

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ROOT_DIR)/lib
	@echo "✓ Clean complete"

# ==============================================================================
# HELP
# ==============================================================================

help:
	@echo "AOCL-Compression GNU Make Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all (default)  - Build the library"
	@echo "  templates      - Generate template files"
	@echo "  test           - Build and run tests"
	@echo "  install        - Install library and headers"
	@echo "  uninstall      - Remove installed files"
	@echo "  clean          - Remove build artifacts"
	@echo "  help           - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  Edit config.mk to customize build options"
	@echo ""
	@echo "Examples:"
	@echo "  make                          # Build with default settings"
	@echo "  make BUILD_TYPE=Debug         # Build debug version"
	@echo "  make BUILD_STATIC_LIBS=1      # Build static library"
	@echo "  make AOCL_EXCLUDE_ZSTD=1      # Build without ZSTD"
	@echo "  make test                     # Run tests"
	@echo "  make install PREFIX=/usr/local # Install to /usr/local"

# ==============================================================================
# INCLUDE DEPENDENCIES
# ==============================================================================

-include $(DEPS)

# ==============================================================================
# DIRECTORY CREATION
# ==============================================================================

$(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(DEP_DIR) $(GEN_DIR):
	@mkdir -p $@

.PRECIOUS: $(DEP_DIR)/%.d
