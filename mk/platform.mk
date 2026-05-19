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
# Platform and Compiler Detection
# ==============================================================================

# ==============================================================================
# OPERATING SYSTEM DETECTION
# ==============================================================================

UNAME_S := $(shell uname -s 2>/dev/null || echo unknown)
UNAME_M := $(shell uname -m 2>/dev/null || echo x86_64)

ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
    OS_TYPE := unix
else
    $(error GNU Make build is supported only on Linux. Detected platform: $(UNAME_S))
endif

# ==============================================================================
# COMPILER DETECTION
# ==============================================================================

# Default compilers
CC ?= gcc
CXX ?= g++
AR ?= ar
AS ?= as

# Detect compiler version and ID
CC_VERSION_OUTPUT := $(shell $(CC) --version 2>/dev/null | head -n1)
CXX_VERSION_OUTPUT := $(shell $(CXX) --version 2>/dev/null | head -n1)

# Identify compiler
CC_ID := unknown
ifneq (,$(findstring gcc,$(CC_VERSION_OUTPUT)))
    CC_ID := gcc
else ifneq (,$(findstring GCC,$(CC_VERSION_OUTPUT)))
    CC_ID := gcc
else ifneq (,$(findstring clang,$(CC_VERSION_OUTPUT)))
    CC_ID := clang
else ifneq (,$(findstring Clang,$(CC_VERSION_OUTPUT)))
    CC_ID := clang
else
    $(error Unsupported compiler: $(CC). Only GCC and Clang are supported.)
endif

# Get compiler version
ifeq ($(CC_ID),gcc)
    CC_MAJOR := $(shell $(CC) -dumpversion | cut -d. -f1)
    CC_MINOR := $(shell $(CC) -dumpversion | cut -d. -f2)
else ifeq ($(CC_ID),clang)
    CC_MAJOR := $(shell $(CC) -dumpversion | cut -d. -f1)
    CC_MINOR := $(shell $(CC) -dumpversion | cut -d. -f2)
endif

# Validate compiler version
ifeq ($(CC_ID),gcc)
    ifeq ($(shell major=$(CC_MAJOR); minor=$(CC_MINOR); [ -n "$$minor" ] || minor=0; ( [ "$$major" -lt 8 ] || { [ "$$major" -eq 8 ] && [ "$$minor" -lt 5 ]; } ) && echo 1),1)
        $(error GCC version must be >= 8.5, found $(CC_MAJOR).$(CC_MINOR))
    endif
else ifeq ($(CC_ID),clang)
    ifeq ($(shell major=$(CC_MAJOR); minor=$(CC_MINOR); [ -n "$$minor" ] || minor=0; ( [ "$$major" -lt 11 ] || { [ "$$major" -eq 11 ] && [ "$$minor" -lt 0 ]; } ) && echo 1),1)
        $(error Clang version must be >= 11.0 (AOCC 2.3+), found $(CC_MAJOR).$(CC_MINOR))
    endif
endif

# ==============================================================================
# LIBRARY NAMING CONVENTIONS
# ==============================================================================

LIB_PREFIX := lib
LIB_EXT := .so
SHARED_LDFLAGS := -shared
LIB_STATIC_EXT := .a
EXE_EXT :=
OBJ_EXT := .o

# ==============================================================================
# PLATFORM-SPECIFIC FLAGS
# ==============================================================================

# Initialize flags
CFLAGS :=
CXXFLAGS :=
LDFLAGS :=
LIBS :=

# C standard (GNU extensions enabled for GCC, matches CMake behavior)
CFLAGS += -std=gnu11

# Common flags for all platforms
CFLAGS += -fomit-frame-pointer -fstrict-aliasing
CXXFLAGS += -fomit-frame-pointer -fstrict-aliasing

# Platform-specific flags
ifeq ($(OS_TYPE),unix)
    # POSIX feature test macros (needed for fdopen, etc. on Unix)
    CFLAGS += -D_POSIX_C_SOURCE=200809L

    # Security flags
    ifeq ($(BUILD_TYPE),Release)
        SECURITY_FLAGS := -fstack-protector-strong -Wformat -Wformat-security
        CFLAGS += $(SECURITY_FLAGS)
        CXXFLAGS += $(SECURITY_FLAGS)
        CFLAGS += -D_FORTIFY_SOURCE=2
        CXXFLAGS += -D_FORTIFY_SOURCE=2
        LDFLAGS += -Wl,-z,relro -Wl,-z,now

        # Position Independent Executable flags (for executables only)
        EXE_CFLAGS += -fpie
        EXE_LDFLAGS += -pie
    endif
    
    # Position Independent Code for all libraries (shared and static)
    # Critical: Static libs need -fPIC to link into PIE executables
    CFLAGS += -fPIC
    CXXFLAGS += -fPIC
endif

# Fast-math optimization
ifeq ($(ENABLE_FAST_MATH),1)
    CFLAGS += -ffast-math
    CXXFLAGS += -ffast-math
endif

# Strict warnings
ifeq ($(ENABLE_STRICT_WARNINGS),1)
    ifeq ($(BUILD_TYPE),Release)
        CFLAGS += -Wall -Werror -Wpedantic
        CXXFLAGS += -Wall -Werror -Wpedantic
    endif
endif

# ==============================================================================
# HEADER AVAILABILITY DETECTION
# ==============================================================================

# Detect sys/uio.h availability (used by Snappy and tests)
HAS_SYS_UIO_H := $(shell printf '%s\n' '\#include <sys/uio.h>' | $(CC) -E - >/dev/null 2>&1; if [ $$? -eq 0 ]; then echo 1; else echo 0; fi)

# ==============================================================================
# SNAPPY BRANCHLESS DECOMPRESSION AUTO-DETECTION
# ==============================================================================

ifeq ($(SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS),AUTO)
    ifeq ($(CC_ID),gcc)
        override SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS := 0
    else
        override SNAPPY_ENABLE_DECOMPRESS_BRANCHLESS := 1
    endif
endif

# ==============================================================================
# EXPORT VARIABLES
# ==============================================================================

export PLATFORM OS_TYPE
export CC CXX AR AS
export CC_ID CC_MAJOR CC_MINOR
export LIB_PREFIX LIB_EXT LIB_STATIC_EXT EXE_EXT OBJ_EXT SHARED_LDFLAGS
export CFLAGS CXXFLAGS LDFLAGS LIBS EXE_CFLAGS EXE_LDFLAGS
export HAS_SYS_UIO_H

# ==============================================================================
# DISPLAY CONFIGURATION
# ==============================================================================

$(info )
$(info ==========================================)
$(info AOCL-Compression Build Configuration)
$(info ==========================================)
$(info Platform: $(PLATFORM) ($(OS_TYPE)))
$(info Architecture: $(UNAME_M))
$(info C Compiler: $(CC) ($(CC_ID) $(CC_MAJOR).$(CC_MINOR)))
$(info C++ Compiler: $(CXX))
$(info Build Type: $(BUILD_TYPE))
$(info Library Type: $(if $(filter 1,$(BUILD_STATIC_LIBS)),static,shared))
$(info ==========================================)
$(info )
