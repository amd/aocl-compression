/**
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

 /** @file third_party_bench.h
 *
 *  @brief APIs to invoke various third party test benches.
 *
 *  This file contains wrappers to main functions of various test benches and fuzzers.
 *
 *  @author Ashish Sriram
 */

typedef int (*test_bench_main)(int argc, char** argv);

typedef struct {
    const char* name; /* name of test bench target used as identifier for -e option */
    test_bench_main fp; /* function pointer to entry point function of test bench target */
} test_bench_t;

//Error codes
#define ERR_TP_BENCH_ARGS -1
#define ERR_TP_BENCH_METHOD -2
#define ERR_TP_BENCH_TEST -3

int unsupported_method_test(int argc, char** argv);
int unsupported_internal_test(int argc, char** argv);

/* List of entry point functions for various third party test benches */
#ifndef AOCL_EXCLUDE_ZSTD
int zstd_fuzzer_main(int argc, char** argv);
int zstd_zstreamtest_main(int argc, char** argv);
int zstd_paramgrill_main(int argc, char** argv);
int zstd_bigdict_main(int argc, char** argv);
int zstd_longmatch_main(int argc, char** argv);
int zstd_invaliddictionaries_main(int argc, char** argv);
int zstd_roundTripCrash_main(int argc, char** argv);
#ifdef THIRD_PARTY_INTERNAL_TEST /* These functions test non-API functions */
int zstd_fullbench_main(int argc, char** argv);
int zstd_decodecorpus_main(int argc, char** argv);
#endif
#endif

/* List of all test bench targets. 
 * To run a specific test bench, target can be passed via -e option to 
 * aocl_third_party_bench */
static const test_bench_t tp_test_bench[] = {
#ifndef AOCL_EXCLUDE_ZSTD
    { "ZSTD_FUZZER",              zstd_fuzzer_main },
    { "ZSTD_ZSTREAMTEST",         zstd_zstreamtest_main },
    { "ZSTD_PARAMGRILL",          zstd_paramgrill_main },
    { "ZSTD_BIGDICT",             zstd_bigdict_main },
    { "ZSTD_LONGMATCH",           zstd_longmatch_main },
    { "ZSTD_INVALIDDICTIONARIES", zstd_invaliddictionaries_main },
    { "ZSTD_ROUNDTRIPCRASH",      zstd_roundTripCrash_main },
#ifdef THIRD_PARTY_INTERNAL_TEST
    { "ZSTD_FULLBENCH",           zstd_fullbench_main },
    { "ZSTD_DECODECORPUS",        zstd_decodecorpus_main },
#else
    { "ZSTD_FULLBENCH",           unsupported_internal_test },
    { "ZSTD_DECODECORPUS",        unsupported_internal_test },
#endif
#else
    { "ZSTD_FUZZER",              unsupported_method_test },
    { "ZSTD_ZSTREAMTEST",         unsupported_method_test },
    { "ZSTD_PARAMGRILL",          unsupported_method_test },
    { "ZSTD_BIGDICT",             unsupported_method_test },
    { "ZSTD_LONGMATCH",           unsupported_method_test },
    { "ZSTD_INVALIDDICTIONARIES", unsupported_method_test },
    { "ZSTD_ROUNDTRIPCRASH",      unsupported_method_test },
    { "ZSTD_FULLBENCH",           unsupported_method_test },
    { "ZSTD_DECODECORPUS",        unsupported_method_test },
#endif
};

#define TP_TEST_BENCH_COUNT (sizeof(tp_test_bench)/sizeof(tp_test_bench[0]))
