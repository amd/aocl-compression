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
# Installation Module
# ==============================================================================
# Handles installation of library, headers, examples, and symbolic links
# ==============================================================================

# Installation directories
INSTALL_LIB_DIR := $(DESTDIR)$(PREFIX)/$(DEST_LIB_PATH)
INSTALL_INC_DIR := $(DESTDIR)$(PREFIX)/$(DEST_INC_PATH)
INSTALL_EXAMPLES_DIR := $(DESTDIR)$(PREFIX)/$(DEST_EXAMPLES_PATH)

# ==============================================================================
# INSTALL TARGET
# ==============================================================================

.PHONY: install install-lib install-headers install-examples install-symlinks

install: all install-lib install-headers install-examples install-symlinks
	@echo ""
	@echo "=========================================="
	@echo "Installation complete!"
	@echo "Library: $(INSTALL_LIB_DIR)"
	@echo "Headers: $(INSTALL_INC_DIR)"
	@echo "Examples: $(INSTALL_EXAMPLES_DIR)"
	@echo "=========================================="

# ==============================================================================
# INSTALL LIBRARY
# ==============================================================================

install-lib: $(LIB_TARGET)
	@echo "Installing library to $(INSTALL_LIB_DIR)..."
	@mkdir -p $(INSTALL_LIB_DIR)
	@cp -f $(LIB_TARGET) $(INSTALL_LIB_DIR)/
	@chmod 755 $(INSTALL_LIB_DIR)/$(notdir $(LIB_TARGET))
	@echo "✓ Library installed"

# ==============================================================================
# INSTALL HEADERS
# ==============================================================================

install-headers: $(GEN_AOCL_ALGO_OPT)
	@echo "Installing headers to $(INSTALL_INC_DIR)..."
	@mkdir -p $(INSTALL_INC_DIR)
	@# Install main API header
	@install -m 644 api/aocl_compression.h $(INSTALL_INC_DIR)/
	@# Install common headers (always)
	@install -m 644 $(GEN_AOCL_ALGO_OPT) $(INSTALL_INC_DIR)/
	@install -m 644 algos/common/aoclFds.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/common/aoclPrefix.h $(INSTALL_INC_DIR)/
	@# Install algorithm-specific headers (conditional)
ifneq ($(AOCL_EXCLUDE_BZIP2),1)
	@install -m 644 algos/bzip2/bzlib.h $(INSTALL_INC_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_LZ4),1)
	@install -m 644 algos/lz4/lz4.h $(INSTALL_INC_DIR)/
ifeq ($(LZ4_FRAME_FORMAT_SUPPORT),1)
	@install -m 644 algos/lz4/lz4frame.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/lz4/lz4frame_static.h $(INSTALL_INC_DIR)/
endif
endif
ifneq ($(AOCL_EXCLUDE_LZ4HC),1)
	@install -m 644 algos/lz4/lz4hc.h $(INSTALL_INC_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_LZMA),1)
	@install -m 644 algos/lzma/7zTypes.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/lzma/Alloc.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/lzma/LzmaDec.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/lzma/LzmaEnc.h $(INSTALL_INC_DIR)/
ifeq ($(AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL),1)
	@install -m 644 algos/lzma/lzma.h $(INSTALL_INC_DIR)/
	@mkdir -p $(INSTALL_INC_DIR)/xzlzma
	@for f in algos/lzma/xzlzma/*.h; do \
		[ -f "$$f" ] && install -m 644 "$$f" $(INSTALL_INC_DIR)/xzlzma/ || true; \
	done
endif
endif
ifneq ($(AOCL_EXCLUDE_SNAPPY),1)
	@install -m 644 algos/snappy/snappy-stubs-public.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/snappy/snappy-sinksource.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/snappy/snappy.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/snappy/snappy-c.h $(INSTALL_INC_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_ZLIB),1)
	@install -m 644 algos/zlib/zconf.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/zlib/zlib.h $(INSTALL_INC_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_ZSTD),1)
	@install -m 644 algos/zstd/lib/zstd.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/zstd/lib/zdict.h $(INSTALL_INC_DIR)/
	@install -m 644 algos/zstd/lib/zstd_errors.h $(INSTALL_INC_DIR)/
endif
	@echo "✓ Headers installed"

# ==============================================================================
# INSTALL EXAMPLES
# ==============================================================================

install-examples:
	@echo "Installing examples to $(INSTALL_EXAMPLES_DIR)..."
	@mkdir -p $(INSTALL_EXAMPLES_DIR)
	@# Install CMakeLists.txt and README
	@install -m 644 examples/CMakeLists.txt $(INSTALL_EXAMPLES_DIR)/
	@install -m 644 examples/README.md $(INSTALL_EXAMPLES_DIR)/
	@# Install algorithm-specific examples (conditional)
ifneq ($(AOCL_EXCLUDE_BZIP2),1)
	@cp -r examples/bzip2 $(INSTALL_EXAMPLES_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_LZ4),1)
	@cp -r examples/lz4 $(INSTALL_EXAMPLES_DIR)/
endif
# AOCL_EXCLUDE_LZ4 and AOCL_EXCLUDE_LZ4HC are synchronized in config.mk.
ifneq ($(AOCL_EXCLUDE_LZ4HC),1)
	@cp -r examples/lz4hc $(INSTALL_EXAMPLES_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_LZMA),1)
	@cp -r examples/lzma $(INSTALL_EXAMPLES_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_SNAPPY),1)
	@cp -r examples/snappy $(INSTALL_EXAMPLES_DIR)/
endif
ifneq ($(AOCL_EXCLUDE_ZLIB),1)
ifeq ($(AOCL_ENABLE_THREADS),1)
	@cp -r examples/zlib $(INSTALL_EXAMPLES_DIR)/
else
	@# Exclude MT-specific examples if threading not enabled
	@mkdir -p $(INSTALL_EXAMPLES_DIR)/zlib
	@cp examples/zlib/*.c $(INSTALL_EXAMPLES_DIR)/zlib/ 2>/dev/null || true
	@rm -f $(INSTALL_EXAMPLES_DIR)/zlib/example_compress2_gzip.c
	@rm -f $(INSTALL_EXAMPLES_DIR)/zlib/example_compress2_raw.c
endif
endif
ifneq ($(AOCL_EXCLUDE_ZSTD),1)
	@cp -r examples/zstd $(INSTALL_EXAMPLES_DIR)/
endif
	@# Install API examples
ifeq ($(AOCL_ENABLE_THREADS),1)
	@cp -r examples/api $(INSTALL_EXAMPLES_DIR)/
else
	@# Exclude MT-specific API examples if threading not enabled
	@mkdir -p $(INSTALL_EXAMPLES_DIR)/api
	@cp examples/api/*.c $(INSTALL_EXAMPLES_DIR)/api/ 2>/dev/null || true
	@rm -f $(INSTALL_EXAMPLES_DIR)/api/example_aocl_llc_skip_rap_frame.c
endif
	@echo "✓ Examples installed"

# ==============================================================================
# INSTALL SYMBOLIC LINKS
# ==============================================================================

install-symlinks:
ifeq ($(filter 1 ON on TRUE true YES yes,$(BUILD_STATIC_LIBS)),)
	@echo "Creating library symbolic links..."
	@mkdir -p $(INSTALL_LIB_DIR)
	@cd $(INSTALL_LIB_DIR) && \
	 rm -f $(LIB_PREFIX)bz2$(LIB_EXT) $(LIB_PREFIX)lz4$(LIB_EXT) \
	       $(LIB_PREFIX)snappy$(LIB_EXT) $(LIB_PREFIX)lzma$(LIB_EXT) \
	       $(LIB_PREFIX)zstd$(LIB_EXT) $(LIB_PREFIX)z$(LIB_EXT)
ifneq ($(AOCL_EXCLUDE_BZIP2),1)
	@cd $(INSTALL_LIB_DIR) && ln -sf $(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT) $(LIB_PREFIX)bz2$(LIB_EXT)
	@echo "  Created: libbz2 -> libaocl_compression"
endif
ifneq ($(AOCL_EXCLUDE_LZ4),1)
	@cd $(INSTALL_LIB_DIR) && ln -sf $(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT) $(LIB_PREFIX)lz4$(LIB_EXT)
	@echo "  Created: liblz4 -> libaocl_compression"
endif
ifneq ($(AOCL_EXCLUDE_SNAPPY),1)
	@cd $(INSTALL_LIB_DIR) && ln -sf $(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT) $(LIB_PREFIX)snappy$(LIB_EXT)
	@echo "  Created: libsnappy -> libaocl_compression"
endif
ifneq ($(AOCL_EXCLUDE_LZMA),1)
	@cd $(INSTALL_LIB_DIR) && ln -sf $(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT) $(LIB_PREFIX)lzma$(LIB_EXT)
	@echo "  Created: liblzma -> libaocl_compression"
endif
ifneq ($(AOCL_EXCLUDE_ZSTD),1)
	@cd $(INSTALL_LIB_DIR) && ln -sf $(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT) $(LIB_PREFIX)zstd$(LIB_EXT)
	@echo "  Created: libzstd -> libaocl_compression"
endif
ifneq ($(AOCL_EXCLUDE_ZLIB),1)
	@cd $(INSTALL_LIB_DIR) && ln -sf $(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT) $(LIB_PREFIX)z$(LIB_EXT)
	@echo "  Created: libz -> libaocl_compression"
endif
	@echo "✓ Symbolic links created"
endif

# ==============================================================================
# UNINSTALL TARGET
# ==============================================================================

.PHONY: uninstall

uninstall: clean
	@echo "Uninstalling AOCL-Compression..."
	@# Remove library
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT)
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)$(LIB_NAME)$(LIB_STATIC_EXT)
	@# Remove symbolic links
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)bz2$(LIB_EXT)
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)lz4$(LIB_EXT)
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)snappy$(LIB_EXT)
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)lzma$(LIB_EXT)
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)zstd$(LIB_EXT)
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_PREFIX)z$(LIB_EXT)
	@# Remove headers
	@rm -f $(INSTALL_INC_DIR)/aocl_compression.h
	@rm -f $(INSTALL_INC_DIR)/aoclAlgoOpt.h
	@rm -f $(INSTALL_INC_DIR)/aoclFds.h
	@rm -f $(INSTALL_INC_DIR)/aoclPrefix.h
	@rm -f $(INSTALL_INC_DIR)/bzlib.h
	@rm -f $(INSTALL_INC_DIR)/lz4.h
	@rm -f $(INSTALL_INC_DIR)/lz4hc.h
	@rm -f $(INSTALL_INC_DIR)/lz4frame.h
	@rm -f $(INSTALL_INC_DIR)/lz4frame_static.h
	@rm -f $(INSTALL_INC_DIR)/7zTypes.h
	@rm -f $(INSTALL_INC_DIR)/Alloc.h
	@rm -f $(INSTALL_INC_DIR)/LzmaDec.h
	@rm -f $(INSTALL_INC_DIR)/LzmaEnc.h
	@rm -f $(INSTALL_INC_DIR)/lzma.h
	@rm -rf $(INSTALL_INC_DIR)/xzlzma
	@rm -f $(INSTALL_INC_DIR)/snappy*.h
	@rm -f $(INSTALL_INC_DIR)/zconf.h
	@rm -f $(INSTALL_INC_DIR)/zlib.h
	@rm -f $(INSTALL_INC_DIR)/zstd.h
	@rm -f $(INSTALL_INC_DIR)/zdict.h
	@rm -f $(INSTALL_INC_DIR)/zstd_errors.h
	@# Remove examples
	@rm -rf $(INSTALL_EXAMPLES_DIR)
	@# Remove empty directories
	@rmdir $(INSTALL_LIB_DIR) 2>/dev/null || true
	@rmdir $(INSTALL_INC_DIR) 2>/dev/null || true
	@# Remove local lib directory
	@rm -rf $(ROOT_DIR)/lib
	@echo "✓ Uninstall complete"
