/**
 * Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
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

 /** @file third_party_bench.c
 *
 *  @brief Interface to invoke various third party test benches
 *         to test aocl compression library.
 *
 *  This file contains the main function that will invoke main APIs
 *  of various test benches and fuzzers.
 *
 *  @author Ashish Sriram
 */

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "aocl_compression.h"
#include "utils.h"
#include "third_party_bench.h"

#ifdef _WINDOWS
#include <windows.h>
#define strcasecmp _stricmp
#endif

#define LOG_UNSUPPORTED_INTERNAL_TEST(logCtx) \
LOG_UNFORMATTED(ERR, logCtx, "Test not supported. Library needs to be built with BUILD_STATIC_LIBS to support this."); \
printf("Test not supported. Library needs to be built with BUILD_STATIC_LIBS to support this.\n");

#define LOG_UNSUPPORTED_METHOD_TEST(logCtx) \
LOG_UNFORMATTED(ERR, logCtx, "Test not supported. Library is not built with support for this method."); \
printf("Test not supported. Library is not built with support for this method.\n");

AOCL_VOID print_user_options(AOCL_VOID)
{
    printf("\nAOCL Compression Library version: %s\n", aocl_llc_version());
    printf("Internal Library version: %s\n", INTERNAL_LIBRARY_VERSION);
    printf("C Compiler: %s\n", CCompiler);
    printf("C++ Compiler: %s\n", CXXCompiler);
    printf("Compile Options: %s%s\n\n", CFLAGS_SET1, CFLAGS_SET2);
    printf("Usage: aocl_third_party_bench <options> <test_bench_options>\n\n");
    printf("where <options> can be:\n");
    printf("-h | --help Print help info\n");
    printf("-l    List all the available third party test benches\n");
    printf("-e<>  Third party test bench identifier\n");
    printf("-v <file1> <file2> Compare if files are equal\n");
    printf("followed by <test_bench_options> options for the specific test bench\n");
}

/* List all available third party test benches and their status */
AOCL_VOID print_available_test_benches(AOCL_VOID)
{
    printf("\nFollowing third party test benches are available:\n");
    for (AOCL_INTP i = 0; i < TP_TEST_BENCH_COUNT; i++) {
        const char* status = (tp_test_bench[i].fp == unsupported_internal_test
            || tp_test_bench[i].fp == unsupported_method_test) ?
            "unsupported" : ""; // indicate unsupported if test is not supported for the build configuration used
        printf("%-30s\t%s\n", tp_test_bench[i].name, status);
    }
}

/* Get corresponding test bench function from tp_test_bench[] */
test_bench_main get_test_bench_fp(const AOCL_CHAR* str)
{
    for (AOCL_INTP i = 0; i < TP_TEST_BENCH_COUNT; i++) {
        if (strcasecmp(str, tp_test_bench[i].name) == 0)
        {
            return tp_test_bench[i].fp;
        }
    }
    return NULL;
}

/* Library is built without support for said method. Hence test target is not supported. */
int unsupported_method_test(int argc, char** argv) {
    LOG_UNSUPPORTED_METHOD_TEST(logCtx);
    return ERR_TP_BENCH_METHOD;
}

/* Library is not built with support to test non-API functions. Hence test target is not supported. */
int unsupported_internal_test(int argc, char** argv) {
    LOG_UNSUPPORTED_INTERNAL_TEST(logCtx);
    return ERR_TP_BENCH_TEST;
}

/* File validation function
 * Compare two files for equality
 * Can be extended to validate specific patterns like compression format compliance, etc. */
int validate(const char* file1, const char* file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        printf("Error: Could not open one or both files for validation.\n");
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return ERR_TP_BENCH_TEST;
    }
    fseek(f1, 0, SEEK_END);
    fseek(f2, 0, SEEK_END);
    size_t size1 = ftell(f1);
    size_t size2 = ftell(f2);
    rewind(f1);
    rewind(f2);
    if (size1 != size2) {
        printf("Validation failed: File sizes differ (%zu vs %zu bytes)\n", size1, size2);
        fclose(f1);
        fclose(f2);
        return ERR_TP_BENCH_TEST;
    }
    unsigned char buf1[4096], buf2[4096];
    size_t total = 0;
    int mismatch = 0;
    while (!feof(f1) && !feof(f2)) {
        size_t r1 = fread(buf1, 1, sizeof(buf1), f1);
        size_t r2 = fread(buf2, 1, sizeof(buf2), f2);
        if (r1 != r2) {
            printf("Validation failed: Read size mismatch at offset %zu\n", total);
            mismatch = 1;
            break;
        }
        int cmp = memcmp(buf1, buf2, r1);
        if (cmp != 0) {
            for (size_t i = 0; i < r1; ++i) {
                if (buf1[i] != buf2[i]) {
                    printf("Validation failed: Mismatch at byte %zu of %zu (0x%02x != 0x%02x)\n", total + i, size1, buf1[i], buf2[i]);
                    mismatch = 1;
                    break;
                }
            }
            break;
        }
        total += r1;
    }
    fclose(f1);
    fclose(f2);
    if (mismatch) {
        return ERR_TP_BENCH_TEST;
    }
    printf("Validation succeeded: Files are identical (%zu bytes)\n", size1);
    return 0;
}

AOCL_INT32 main(AOCL_INT32 argc, AOCL_CHAR** argv)
{
    AOCL_INTP cnt = 1;
    AOCL_INTP ret = 1;
    AOCL_CHAR option;

    LOG_UNFORMATTED(TRACE, logCtx, "Enter");

    if (argc <= 1)
    {
        print_user_options();
        LOG_UNFORMATTED(TRACE, logCtx, "Exit");
        return 2;
    }

    while (cnt < argc)
    {
        option = argv[cnt][0];
        if (option == '-')
        {
            switch (argv[cnt][1])
            {
            case 'h': // help
            {
                print_user_options();
                ret = 2;
                break;
            }
            case '-': // help
            {
                if (!strcmp(&argv[cnt][2], "help"))
                {
                    print_user_options();
                    ret = 2;
                }
                else {
                    LOG_FORMATTED(ERR, logCtx, "Invalid user option %s", &argv[cnt][2]);
                    ret = ERR_TP_BENCH_ARGS;
                }
                break;
            }
            case 'l':  // list supported test benches
            {
                print_available_test_benches();
                ret = 2;
                break;
            }
            case 'e': // invoke respective test bench
            {
                const char* test_name = &argv[cnt][2];
                test_bench_main fp = get_test_bench_fp(test_name);
                if (fp == NULL) {
                    LOG_UNSUPPORTED_METHOD_TEST(logCtx);
                    ret = ERR_TP_BENCH_ARGS;
                }
                else
                {
                    /* Arguments succeeding 'cnt' need to be passed to test bench *_main()
                    * As first argument needs to be program name, argv[cnt] i.e. -e<TEST_BENCH_NAME>
                    * itself is passed for this. */
                    AOCL_INT32 tp_argc = argc - cnt;
                    AOCL_CHAR** tp_argv = &argv[cnt];
                    ret = fp(tp_argc, tp_argv);
                    if (ret != 0) {
                        LOG_FORMATTED(ERR, logCtx, "%s test failed", test_name);
                    }
                    return ret;
                }
                break;
            }
            case 'v': // validate two files
            {
                if (cnt + 2 >= argc) {
                    printf("Error: -v requires two file arguments\n");
                    ret = ERR_TP_BENCH_ARGS;
                    break;
                }
                const char* file1 = argv[cnt + 1];
                const char* file2 = argv[cnt + 2];
                ret = validate(file1, file2);
                cnt += 2; // skip file arguments
                break;
            }
            }
        }
        else {
            LOG_UNFORMATTED(ERR, logCtx, "Invalid user option");
            ret = ERR_TP_BENCH_ARGS;
        }
        cnt++;
        if (ret < 0) {
            printf("Invalid arguments passed. Exiting application.\n");
            break;
        }
    }

    LOG_UNFORMATTED(TRACE, logCtx, "Exit");
    return ret;
}
