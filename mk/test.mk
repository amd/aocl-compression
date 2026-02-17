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
# Testing System
# ==============================================================================
# Independent test orchestration (no CTest dependency)
# Supports GTest, benchmarks, and third-party native API tests
# ==============================================================================

# Test directories
TEST_BUILD_DIR := $(BUILD_DIR)/test
TEST_RESULTS_DIR := $(BUILD_DIR)/test-results
TEST_DATA_DIR := test/data

# Test flags
TEST_CFLAGS := $(PUBLIC_INCLUDE_DIRS) \
               -DAOCL_UNIT_TEST \
               -Wno-error=unused-variable -Wno-error=unused-function \
               -Wno-error=sign-compare -Wno-error=unused-but-set-variable \
               -Wno-error=dangling-else -Wno-error=vla -Wno-error=overflow
TEST_CXXFLAGS := $(PUBLIC_INCLUDE_DIRS) \
                 -DAOCL_UNIT_TEST \
                 -Wno-error=unused-variable -Wno-error=unused-function \
                 -Wno-error=sign-compare -Wno-error=unused-but-set-variable \
                 -Wno-error=dangling-else -Wno-error=vla -Wno-error=overflow \
                 -Wno-error=deprecated-declarations -Wno-error=maybe-uninitialized \
                 -Wno-error=conversion-null

# Conditionally add sys/uio.h define if header is available
ifeq ($(HAS_SYS_UIO_H),1)
    TEST_CXXFLAGS += -DHAVE_SYS_UIO_H
endif

# Match CMake test compile definitions for supported options
ifeq ($(BUILD_TYPE),Debug)
    TEST_CFLAGS += -DDEBUG_ASSERT_ENABLED
    TEST_CXXFLAGS += -DDEBUG_ASSERT_ENABLED
endif
ifeq ($(AOCL_ENABLE_THREADS),1)
    TEST_CFLAGS += -DAOCL_ENABLE_THREADS
    TEST_CXXFLAGS += -DAOCL_ENABLE_THREADS
endif
ifeq ($(NATIVE_ENABLE_THREADS),1)
    TEST_CFLAGS += -DNATIVE_ENABLE_THREADS
    TEST_CXXFLAGS += -DNATIVE_ENABLE_THREADS
endif
ifneq ($(AOCL_DECOMPRESS_FAST),OFF)
    TEST_CFLAGS += -DAOCL_DECOMPRESS_FAST=$(AOCL_DECOMPRESS_FAST)
    TEST_CXXFLAGS += -DAOCL_DECOMPRESS_FAST=$(AOCL_DECOMPRESS_FAST)
endif
ifneq ($(AOCL_COMPRESS_FAST),OFF)
    TEST_CFLAGS += -DAOCL_COMPRESS_FAST=$(AOCL_COMPRESS_FAST)
    TEST_CXXFLAGS += -DAOCL_COMPRESS_FAST=$(AOCL_COMPRESS_FAST)
endif
ifeq ($(AOCL_ENABLE_LOG_FEATURE),1)
    TEST_CFLAGS += -DAOCL_ENABLE_LOG_FEATURE
    TEST_CXXFLAGS += -DAOCL_ENABLE_LOG_FEATURE
endif
ifeq ($(AOCL_EXCLUDE_BZIP2),1)
    TEST_CFLAGS += -DAOCL_EXCLUDE_BZIP2
    TEST_CXXFLAGS += -DAOCL_EXCLUDE_BZIP2
endif
ifeq ($(AOCL_EXCLUDE_LZ4),1)
    TEST_CFLAGS += -DAOCL_EXCLUDE_LZ4
    TEST_CXXFLAGS += -DAOCL_EXCLUDE_LZ4
endif
ifeq ($(AOCL_EXCLUDE_LZ4HC),1)
    TEST_CFLAGS += -DAOCL_EXCLUDE_LZ4HC
    TEST_CXXFLAGS += -DAOCL_EXCLUDE_LZ4HC
endif
# LZ4 extra hash table updates
ifeq ($(AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES),1)
    TEST_CFLAGS += -DAOCL_LZ4_EXTRA_HASH_TABLE_UPDATES
    TEST_CXXFLAGS += -DAOCL_LZ4_EXTRA_HASH_TABLE_UPDATES
endif
# LZ4 hash bits configuration (must match library compilation)
ifneq ($(AOCL_LZ4_HASH_BITS_USED),OFF)
    ifeq ($(AOCL_LZ4_HASH_BITS_USED),LOW)
        TEST_CFLAGS += -DAOCL_LZ4_HASH_BITS_USED=41
        TEST_CXXFLAGS += -DAOCL_LZ4_HASH_BITS_USED=41
    else ifeq ($(AOCL_LZ4_HASH_BITS_USED),HIGH)
        TEST_CFLAGS += -DAOCL_LZ4_HASH_BITS_USED=44
        TEST_CXXFLAGS += -DAOCL_LZ4_HASH_BITS_USED=44
    endif
endif
ifeq ($(AOCL_EXCLUDE_LZMA),1)
    TEST_CFLAGS += -DAOCL_EXCLUDE_LZMA
    TEST_CXXFLAGS += -DAOCL_EXCLUDE_LZMA
endif
ifeq ($(AOCL_EXCLUDE_SNAPPY),1)
    TEST_CFLAGS += -DAOCL_EXCLUDE_SNAPPY
    TEST_CXXFLAGS += -DAOCL_EXCLUDE_SNAPPY
endif
ifeq ($(AOCL_EXCLUDE_ZLIB),1)
    TEST_CFLAGS += -DAOCL_EXCLUDE_ZLIB
    TEST_CXXFLAGS += -DAOCL_EXCLUDE_ZLIB
endif
ifeq ($(AOCL_EXCLUDE_ZSTD),1)
    TEST_CFLAGS += -DAOCL_EXCLUDE_ZSTD
    TEST_CXXFLAGS += -DAOCL_EXCLUDE_ZSTD
endif
ifeq ($(BUILD_STATIC_LIBS),1)
    TEST_CXXFLAGS += -DAOCL_INTERNAL_TEST
endif

# Symbol prefixing support
ifeq ($(AOCL_LLC_PREFIX),1)
    TEST_CFLAGS += -DAOCL_LLC_PREFIX
    TEST_CXXFLAGS += -DAOCL_LLC_PREFIX
endif
TEST_LDFLAGS := -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR)
TEST_RUN_ENV = LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH
TEST_LIBS := $(LIB_LINK_TARGET) -lstdc++ $(LIBS)
TEST_LIBS += -ldl

# ==============================================================================
# GTEST CONFIGURATION
# ==============================================================================

# GTest directories
GTEST_SOURCE_DIR := gtest
GTEST_CFLAGS := $(shell pkg-config --cflags gtest 2>/dev/null)
GTEST_LIB := $(shell pkg-config --libs gtest_main 2>/dev/null)
ifeq ($(strip $(GTEST_LIB)),)
    GTEST_CFLAGS :=
    GTEST_LIB := -lgtest -lgtest_main -lpthread
endif

# Find all GTest source files
GTEST_API_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/api/*.cpp)
ifneq ($(AOCL_EXCLUDE_BZIP2),1)
GTEST_BZIP2_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/bzip2/*.cpp)
endif
ifneq ($(AOCL_EXCLUDE_LZ4),1)
GTEST_LZ4_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/lz4/*.cpp)
endif
# AOCL_EXCLUDE_LZ4 and AOCL_EXCLUDE_LZ4HC are synchronized in config.mk.
ifneq ($(AOCL_EXCLUDE_LZ4HC),1)
GTEST_LZ4HC_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/lz4hc/*.cpp)
endif
ifneq ($(AOCL_EXCLUDE_LZMA),1)
GTEST_LZMA_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/lzma/*.cpp)
endif
ifneq ($(AOCL_EXCLUDE_SNAPPY),1)
GTEST_SNAPPY_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/snappy/*.cpp)
endif
ifneq ($(AOCL_EXCLUDE_ZLIB),1)
GTEST_ZLIB_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/zlib/*.cpp)
endif
ifneq ($(AOCL_EXCLUDE_ZSTD),1)
GTEST_ZSTD_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/zstd/*.cpp)
endif
GTEST_COMMON_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/common/*.cpp)
ifeq ($(AOCL_ENABLE_THREADS),1)
GTEST_THREADS_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/threads/*.cpp)
endif
GTEST_UTILS_SOURCES := $(wildcard $(GTEST_SOURCE_DIR)/utils/*.cpp)

# Combine all GTest sources (excluding zstd/zlib which need special handling)
GTEST_ALL_SOURCES := $(GTEST_API_SOURCES) \
                     $(GTEST_BZIP2_SOURCES) \
                     $(GTEST_LZ4_SOURCES) \
                     $(GTEST_LZ4HC_SOURCES) \
                     $(GTEST_LZMA_SOURCES) \
                     $(GTEST_SNAPPY_SOURCES) \
                     $(GTEST_COMMON_SOURCES) \
                     $(GTEST_THREADS_SOURCES) \
                     $(GTEST_UTILS_SOURCES)

# Generate test executable names
GTEST_BINS := $(patsubst $(GTEST_SOURCE_DIR)/%.cpp,$(TEST_BUILD_DIR)/%$(EXE_EXT),$(GTEST_ALL_SOURCES))

# ZSTD tests need all source files compiled together (they share common functions)
ifneq ($(GTEST_ZSTD_SOURCES),)
    GTEST_BINS += $(TEST_BUILD_DIR)/zstd/zstd_gtest$(EXE_EXT)
endif

# ZLIB tests need all source files compiled together (matches CMake behavior)
ifneq ($(GTEST_ZLIB_SOURCES),)
    GTEST_BINS += $(TEST_BUILD_DIR)/zlib/zlib_gtest$(EXE_EXT)
endif

# ==============================================================================
# BENCHMARK CONFIGURATION
# ==============================================================================

# ==============================================================================
# THIRD-PARTY TEST CONFIGURATION
# ==============================================================================

# TODO: Future implementation
# ifeq ($(TEST_COVERAGE_THIRD_PARTY),1)

# THIRD_PARTY_TEST_DIR := test/third_party

# Third-party test sources (conditional based on excluded algorithms)
# THIRD_PARTY_TESTS :=

# ifneq ($(AOCL_EXCLUDE_BZIP2),1)
#     THIRD_PARTY_TESTS += $(TEST_BUILD_DIR)/third_party/bzip2_test$(EXE_EXT)
# endif

# ifneq ($(AOCL_EXCLUDE_LZ4),1)
#     THIRD_PARTY_TESTS += $(TEST_BUILD_DIR)/third_party/lz4_test$(EXE_EXT)
# endif

# ifneq ($(AOCL_EXCLUDE_LZMA),1)
#     THIRD_PARTY_TESTS += $(TEST_BUILD_DIR)/third_party/lzma_test$(EXE_EXT)
# endif

# ifneq ($(AOCL_EXCLUDE_SNAPPY),1)
#     THIRD_PARTY_TESTS += $(TEST_BUILD_DIR)/third_party/snappy_test$(EXE_EXT)
# endif

# ifneq ($(AOCL_EXCLUDE_ZLIB),1)
#     THIRD_PARTY_TESTS += $(TEST_BUILD_DIR)/third_party/zlib_test$(EXE_EXT)
# endif

# ifneq ($(AOCL_EXCLUDE_ZSTD),1)
#     THIRD_PARTY_TESTS += $(TEST_BUILD_DIR)/third_party/zstd_test$(EXE_EXT)
# endif

# endif # TEST_COVERAGE_THIRD_PARTY

# ==============================================================================
# TEST TARGETS
# ==============================================================================

.PHONY: test test-unit test-bench test-third-party test-all test-clean

# Main test target
test: test-unit test-bench
	@echo ""
	@echo "=========================================="
	@echo "All tests completed successfully!"
	@echo "=========================================="

# Run all tests including third-party
# TODO: Future implementation
test-all: test-unit test-bench
# ifeq ($(TEST_COVERAGE_THIRD_PARTY),1)
# 	@$(MAKE) test-third-party
# endif
	@echo ""
	@echo "=========================================="
	@echo "Complete test suite passed!"
	@echo "=========================================="

# ==============================================================================
# UNIT TESTS (GTest)
# ==============================================================================

test-unit: $(GTEST_BINS)
	@echo ""
	@echo "=========================================="
	@echo "Running Unit Tests"
	@echo "=========================================="
	@mkdir -p $(TEST_RESULTS_DIR)
	@set -o pipefail; passed=0; failed=0; total=0; \
	for test in $(GTEST_BINS); do \
		if [ -f "$$test" ]; then \
			test_name=$$(basename $$test); \
			total=$$((total + 1)); \
			echo ""; \
			echo "Running $$test_name..."; \
			if $(TEST_RUN_ENV) \
			   $$test --gtest_output=xml:$(TEST_RESULTS_DIR)/$$test_name.xml 2>&1 | \
			   tee $(TEST_RESULTS_DIR)/$$test_name.log; then \
				echo "✓ $$test_name PASSED"; \
				passed=$$((passed + 1)); \
			else \
				echo "✗ $$test_name FAILED"; \
				failed=$$((failed + 1)); \
			fi; \
		fi; \
	done; \
	echo ""; \
	echo "=========================================="; \
	echo "Unit Test Summary:"; \
	echo "  Total:  $$total"; \
	echo "  Passed: $$passed"; \
	echo "  Failed: $$failed"; \
	echo "=========================================="; \
	if [ $$failed -gt 0 ]; then exit 1; fi

# ==============================================================================
# BENCHMARK TESTS
# ==============================================================================

test-bench: $(BENCH_BIN)
	@echo ""
	@echo "=========================================="
	@echo "Running Benchmark Tests"
	@echo "=========================================="
	@mkdir -p $(TEST_RESULTS_DIR)
	@set -o pipefail; $(TEST_RUN_ENV) \
	 $(BENCH_BIN) 2>&1 | tee $(TEST_RESULTS_DIR)/benchmark.log; \
	BENCH_EXIT=$$?; \
	if [ $$BENCH_EXIT -eq 0 ]; then \
	    echo "✓ Benchmark tests completed"; \
	else \
	    echo "✗ Benchmark tests FAILED (exit code: $$BENCH_EXIT)"; \
	    exit $$BENCH_EXIT; \
	fi

# ==============================================================================
# THIRD-PARTY NATIVE API TESTS
# TODO: Future implementation
# ==============================================================================

# ifeq ($(TEST_COVERAGE_THIRD_PARTY),1)

# test-third-party: $(THIRD_PARTY_TESTS)
# 	@echo ""
# 	@echo "=========================================="
# 	@echo "Running Third-Party Native API Tests"
# 	@echo "=========================================="
# 	@mkdir -p $(TEST_RESULTS_DIR)
# 	@set -o pipefail; passed=0; failed=0; total=0; \
# 	for test in $(THIRD_PARTY_TESTS); do \
# 		if [ -f "$$test" ]; then \
# 			test_name=$$(basename $$test); \
# 			total=$$((total + 1)); \
# 			echo ""; \
# 			echo "Running $$test_name..."; \
# 			if $(TEST_RUN_ENV) \
# 			   $$test 2>&1 | tee $(TEST_RESULTS_DIR)/$$test_name.log; then \
# 				echo "✓ $$test_name PASSED"; \
# 				passed=$$((passed + 1)); \
# 			else \
# 				echo "✗ $$test_name FAILED"; \
# 				failed=$$((failed + 1)); \
# 			fi; \
# 		fi; \
# 	done; \
# 	echo ""; \
# 	echo "=========================================="; \
# 	echo "Third-Party Test Summary:"; \
# 	echo "  Total:  $$total"; \
# 	echo "  Passed: $$passed"; \
# 	echo "  Failed: $$failed"; \
# 	echo "=========================================="; \
# 	if [ $$failed -gt 0 ]; then exit 1; fi

# endif # TEST_COVERAGE_THIRD_PARTY

# ==============================================================================
# BUILD GTEST BINARIES
# ==============================================================================

# Special rule for ZSTD tests (all files compiled together)
$(TEST_BUILD_DIR)/zstd/zstd_gtest$(EXE_EXT): $(GTEST_ZSTD_SOURCES) $(LIB_TARGET) $(GTEST_SOURCE_DIR)/gtest_utils.cpp
	@mkdir -p $(dir $@)
	@echo "Building test: $(notdir $@) (all zstd test files)"
	@$(CXX) $(CXXFLAGS) $(TEST_CXXFLAGS) \
		$(GTEST_CFLAGS) \
		-I$(GTEST_SOURCE_DIR) \
		$(GTEST_ZSTD_SOURCES) $(GTEST_SOURCE_DIR)/gtest_utils.cpp \
		-o $@ \
		$(TEST_LDFLAGS) $(LDFLAGS) $(LIB_LINK_TARGET) $(GTEST_LIB) $(LIBS)

# Special rule for ZLIB tests (all files compiled together)
$(TEST_BUILD_DIR)/zlib/zlib_gtest$(EXE_EXT): $(GTEST_ZLIB_SOURCES) $(LIB_TARGET) $(GTEST_SOURCE_DIR)/gtest_utils.cpp
	@mkdir -p $(dir $@)
	@echo "Building test: $(notdir $@) (all zlib test files)"
	@$(CXX) $(CXXFLAGS) $(TEST_CXXFLAGS) \
		$(GTEST_CFLAGS) \
		-I$(GTEST_SOURCE_DIR) \
		$(GTEST_ZLIB_SOURCES) $(GTEST_SOURCE_DIR)/gtest_utils.cpp \
		-o $@ \
		$(TEST_LDFLAGS) $(LDFLAGS) $(LIB_LINK_TARGET) $(GTEST_LIB) $(LIBS)

# Build GTest binaries (one per source file)
$(TEST_BUILD_DIR)/%$(EXE_EXT): $(GTEST_SOURCE_DIR)/%.cpp $(LIB_TARGET) $(GTEST_SOURCE_DIR)/gtest_utils.cpp
	@mkdir -p $(dir $@)
	@echo "Building test: $(notdir $@)"
	@$(CXX) $(CXXFLAGS) $(TEST_CXXFLAGS) \
		$(GTEST_CFLAGS) \
		-I$(GTEST_SOURCE_DIR) \
		$< $(GTEST_SOURCE_DIR)/gtest_utils.cpp \
		-o $@ \
		$(TEST_LDFLAGS) $(LDFLAGS) $(LIB_LINK_TARGET) $(GTEST_LIB) $(LIBS)

# ==============================================================================
# BUILD BENCHMARK BINARY
# ==============================================================================
# Benchmark build rule is provided by top-level Makefile and reused here.

# ==============================================================================
# TODO: Future implementation
# BUILD THIRD-PARTY TEST BINARIES
# ==============================================================================

# ifeq ($(TEST_COVERAGE_THIRD_PARTY),1)

# Pattern rule for third-party tests
# $(TEST_BUILD_DIR)/third_party/%_test$(EXE_EXT): $(THIRD_PARTY_TEST_DIR)/%_test.c $(LIB_TARGET)
# 	@mkdir -p $(dir $@)
# 	@echo "Building third-party test: $(notdir $@)"
# 	@$(CC) $(CFLAGS) $(TEST_CFLAGS) \
# 		$< -o $@ \
# 		$(TEST_LDFLAGS) $(LDFLAGS) $(TEST_LIBS)

# endif # TEST_COVERAGE_THIRD_PARTY

# ==============================================================================
# TEST DATA DOWNLOAD
# ==============================================================================

.PHONY: test-data

test-data:
	@echo "Downloading test datasets..."
	@mkdir -p $(TEST_DATA_DIR)
	@# Calgary Corpus
	@if [ ! -f $(TEST_DATA_DIR)/calgary.zip ]; then \
		echo "Downloading Calgary corpus..."; \
		wget -q -O $(TEST_DATA_DIR)/calgary.zip \
			http://www.data-compression.info/files/corpora/calgarycorpus.zip 2>/dev/null || \
		curl -sL -o $(TEST_DATA_DIR)/calgary.zip \
			http://www.data-compression.info/files/corpora/calgarycorpus.zip 2>/dev/null || \
		echo "Warning: Could not download Calgary corpus"; \
	fi
	@# Silesia Corpus
	@if [ ! -f $(TEST_DATA_DIR)/silesia.zip ]; then \
		echo "Downloading Silesia corpus..."; \
		wget -q -O $(TEST_DATA_DIR)/silesia.zip \
			http://sun.aei.polsl.pl/~sdeor/corpus/silesia.zip 2>/dev/null || \
		curl -sL -o $(TEST_DATA_DIR)/silesia.zip \
			http://sun.aei.polsl.pl/~sdeor/corpus/silesia.zip 2>/dev/null || \
		echo "Warning: Could not download Silesia corpus"; \
	fi
	@# Canterbury Corpus
	@if [ ! -f $(TEST_DATA_DIR)/canterbury.tar.gz ]; then \
		echo "Downloading Canterbury corpus..."; \
		wget -q -O $(TEST_DATA_DIR)/canterbury.tar.gz \
			http://corpus.canterbury.ac.nz/resources/cantrbry.tar.gz 2>/dev/null || \
		curl -sL -o $(TEST_DATA_DIR)/canterbury.tar.gz \
			http://corpus.canterbury.ac.nz/resources/cantrbry.tar.gz 2>/dev/null || \
		echo "Warning: Could not download Canterbury corpus"; \
	fi
	@echo "✓ Test data ready"

# TODO: Future implementation
# ==============================================================================
# FUZZER TESTS (Linux + Clang only)
# ==============================================================================

# ifeq ($(AOCL_TEST_FUZZER),1)

# FUZZER_SOURCE_DIR := gtest/fuzzer
# FUZZER_SOURCES := $(wildcard $(FUZZER_SOURCE_DIR)/*.cpp)
# FUZZER_BINS := $(patsubst $(FUZZER_SOURCE_DIR)/%.cpp,$(TEST_BUILD_DIR)/fuzzer/%$(EXE_EXT),$(FUZZER_SOURCES))

# .PHONY: test-fuzzer

# test-fuzzer: $(FUZZER_BINS)
# 	@echo ""
# 	@echo "=========================================="
# 	@echo "Running Fuzzer Tests"
# 	@echo "=========================================="
# 	@for fuzzer in $(FUZZER_BINS); do \
# 		if [ -f "$$fuzzer" ]; then \
# 			fuzzer_name=$$(basename $$fuzzer); \
# 			echo "Running $$fuzzer_name..."; \
# 			if [ "$(AOCL_TEST_FUZZER_WITH_CORPUS)" = "1" ]; then \
# 				LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH \
# 				$$fuzzer -max_total_time=60 corpus/ 2>&1 | tee $(TEST_RESULTS_DIR)/$$fuzzer_name.log; \
# 			else \
# 				LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH \
# 				$$fuzzer -max_total_time=10 2>&1 | tee $(TEST_RESULTS_DIR)/$$fuzzer_name.log; \
# 			fi; \
# 		fi; \
# 	done
# 	@echo "✓ Fuzzer tests completed"

# Build fuzzer binaries
# $(TEST_BUILD_DIR)/fuzzer/%$(EXE_EXT): $(FUZZER_SOURCE_DIR)/%.cpp $(LIB_TARGET)
# 	@mkdir -p $(dir $@)
# 	@echo "Building fuzzer: $(notdir $@)"
# 	@$(CXX) $(CXXFLAGS) $(TEST_CXXFLAGS) \
# 		-fsanitize=fuzzer \
# 		$< -o $@ \
# 		$(TEST_LDFLAGS) $(TEST_LIBS)

# endif # AOCL_TEST_FUZZER
# TODO: Future implementation

# ==============================================================================
# VALGRIND SUPPORT
# ==============================================================================

# ifeq ($(VALGRIND),1)

# .PHONY: test-valgrind

# test-valgrind: $(GTEST_BINS)
# 	@echo ""
# 	@echo "=========================================="
# 	@echo "Running Tests with Valgrind"
# 	@echo "=========================================="
# 	@mkdir -p $(TEST_RESULTS_DIR)
# 	@passed=0; failed=0; \
# 	for test in $(GTEST_BINS); do \
# 		if [ -f "$$test" ]; then \
# 			test_name=$$(basename $$test); \
# 			echo ""; \
# 			echo "Running $$test_name with Valgrind..."; \
# 			if $(TEST_RUN_ENV) \
# 			   valgrind --leak-check=full --error-exitcode=1 \
# 			   --suppressions=test/.valgrind-suppressions \
# 			   $$test 2>&1 | tee $(TEST_RESULTS_DIR)/valgrind_$$test_name.log; then \
# 				echo "✓ $$test_name PASSED (Valgrind)"; \
# 				passed=$$((passed + 1)); \
# 			else \
# 				echo "✗ $$test_name FAILED (Valgrind)"; \
# 				failed=$$((failed + 1)); \
# 			fi; \
# 		fi; \
# 	done; \
# 	echo ""; \
# 	echo "=========================================="; \
# 	echo "Valgrind Test Summary:"; \
# 	echo "  Passed: $$passed"; \
# 	echo "  Failed: $$failed"; \
# 	echo "=========================================="; \
# 	if [ $$failed -gt 0 ]; then exit 1; fi

# endif # VALGRIND

# ==============================================================================
# TEST REPORT GENERATION
# ==============================================================================

.PHONY: test-report

test-report:
	@echo "Generating test report..."
	@mkdir -p $(TEST_RESULTS_DIR)
	@echo "# AOCL-Compression Test Report" > $(TEST_RESULTS_DIR)/report.md
	@echo "" >> $(TEST_RESULTS_DIR)/report.md
	@echo "Generated: $$(date)" >> $(TEST_RESULTS_DIR)/report.md
	@echo "" >> $(TEST_RESULTS_DIR)/report.md
	@echo "## Configuration" >> $(TEST_RESULTS_DIR)/report.md
	@echo "" >> $(TEST_RESULTS_DIR)/report.md
	@echo "- Platform: $(PLATFORM)" >> $(TEST_RESULTS_DIR)/report.md
	@echo "- Compiler: $(CC_ID) $(CC_MAJOR).$(CC_MINOR)" >> $(TEST_RESULTS_DIR)/report.md
	@echo "- Build Type: $(BUILD_TYPE)" >> $(TEST_RESULTS_DIR)/report.md
	@echo "- Library Type: $(if $(filter 1,$(BUILD_STATIC_LIBS)),static,shared)" >> $(TEST_RESULTS_DIR)/report.md
	@echo "" >> $(TEST_RESULTS_DIR)/report.md
	@echo "## Test Results" >> $(TEST_RESULTS_DIR)/report.md
	@echo "" >> $(TEST_RESULTS_DIR)/report.md
	@for log in $(TEST_RESULTS_DIR)/*.log; do \
		if [ -f "$$log" ]; then \
			echo "### $$(basename $$log .log)" >> $(TEST_RESULTS_DIR)/report.md; \
			echo '```' >> $(TEST_RESULTS_DIR)/report.md; \
			tail -30 $$log >> $(TEST_RESULTS_DIR)/report.md; \
			echo '```' >> $(TEST_RESULTS_DIR)/report.md; \
			echo "" >> $(TEST_RESULTS_DIR)/report.md; \
		fi; \
	done
	@echo "✓ Test report generated: $(TEST_RESULTS_DIR)/report.md"

# ==============================================================================
# CLEAN TEST ARTIFACTS
# ==============================================================================

test-clean:
	@echo "Cleaning test artifacts..."
	@rm -rf $(TEST_BUILD_DIR) $(TEST_RESULTS_DIR)
	@echo "✓ Test artifacts cleaned"

# ==============================================================================
# HELP
# ==============================================================================

.PHONY: test-help

test-help:
	@echo "AOCL-Compression Testing System"
	@echo ""
	@echo "Test Targets:"
	@echo "  test           - Run unit tests and benchmarks"
	@echo "  test-all       - Run complete test suite (including third-party)"
	@echo "  test-unit      - Run GTest unit tests only"
	@echo "  test-bench     - Run benchmarks only"
	@# echo "  test-third-party - Run third-party native API tests"
	@echo "  test-data      - Download test datasets"
	@echo "  test-report    - Generate test report"
	@echo "  test-clean     - Clean test artifacts"
	@# echo ""
	@# echo "Fuzzer Targets (Clang + Linux only):"
	@# echo "  test-fuzzer    - Run fuzzer tests"
	@# echo ""
	@# echo "Valgrind Targets (Linux + Debug only):"
	@# echo "  test-valgrind  - Run tests with Valgrind"
	@echo ""
	@echo "Configuration:"
	@echo "  Set AOCL_TEST_COVERAGE=1 to enable testing"
	@# echo "  TODO: TEST_COVERAGE_THIRD_PARTY (future implementation)"
	@# echo "  TODO: VALGRIND (future implementation)"
	@# echo "  TODO: AOCL_TEST_FUZZER (future implementation)"
