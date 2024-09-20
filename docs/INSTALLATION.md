
@page Build Build and Installation



Installation
------------

1. Download the latest stable release from the Github repository:<br>
https://github.com/amd/aocl-compression
2. Install CMake on the machine where the sources are to be compiled.
3. Make any one of the compilers GCC or Clang available on the machine.
4. Then, use the cmake based build system to compile and generate AOCL-Compression <br>
library and testsuite binary as explained below for Linux® and Windows® platforms.

Building on Linux
-----------------

1. To create a build directory and configure the build system in it, run the following:
   ```
    cmake -B <build directory> <CMakeList.txt filepath>
   ```
   Additional options that can be specified for build configuration are:
   ```
   cmake -B <build directory> <CMakeList.txt filepath> 
      -DCMAKE_INSTALL_PREFIX=<install path> 
      -DCMAKE_BUILD_TYPE=<Debug or Release> 
      -DBUILD_STATIC_LIBS=ON
      <Additional Library Build Options>
   ```

   To use clang compiler for the build, specify `-DCMAKE_C_COMPILER=clang` as the option.
2. Compile using the following command:
   ```
   cmake --build <build directory> --target install -j
   ```
   The library is generated in "lib" directory. <br>
   The test bench executable is generated in "build". <br>
   The additional option `--target install` will install the library, and <br>
   interface header files in the installation path as specified with <br>
   `-DCMAKE_INSTALL_PREFIX` option or in the local system path. <br>
   The option `-j` will run the compilation process using multiple cores.
3. To uninstall the installed files, run the following custom command:
   ```
   cmake --build <build directory> --target uninstall
   ```
   To uninstall and then install the build package, run the following command:
   ```
   cmake --build <build directory> --target uninstall --target install -j -v
   ```
   The option `-v` will print verbose build logs on the console.
4. To clear or delete the build folder or files, manually remove the build directory or its files.


Building on Windows
-------------------
As a prerequisite, make Microsoft Visual Studio® available along with <br>
__Desktop development with C++__ toolset that includes the Clang compiler.

Building with Visual Studio IDE (GUI)
-------------------------------------
1. Launch CMake GUI and set the locations for source package and build output.
2.  Click __Configure__ option and select:
      - __Generator__ as the Installed Microsoft Visual Studio Version
      - __Platform__ as __x64__
      - __Optional toolset__ as __ClangCl__
3. Select additional library config and build options.
4. Configure CMAKE_INSTALL_PREFIX appropriately.
5. Click __Generate__.
   Microsoft Visual Studio project is generated.
6. Click __Open Project__.
   Microsoft Visual Studio project for the source package __is launched__.
7. Build the entire solution or the required projects.

Building with Visual Studio IDE (command line)
----------------------------------------------
1. Go to AOCL-Compression source package and create a folder named build.
2. Go to the build folder.
3. Use the following command to configure and build the library and test bench executable.
```
cmake .. -T ClangCl -G <installed Visual Studio version> && cmake --build . --config Release --target INSTALL
```
You can pass additional library configuration and build options in the command.

Additional Library Build Options
--------------------------------
Use the following additional options to configure your build:

Option                              |  Description
------------------------------------|----------------------------------------------------------------------------------------
AOCL_LZ4_OPT_PREFETCH_BACKWARDS     |  Enable LZ4 optimizations related to backward prefetching of data (Disabled by default)
SNAPPY_MATCH_SKIP_OPT               |  Enable Snappy match skipping optimization (Enabled by default)
LZ4_FRAME_FORMAT_SUPPORT            |  Enable building LZ4 with Frame format and API support (Enabled by default)
AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS |  Disable Pattern Analysis in LZ4HC for level 9 (Enabled by default)
AOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST|  Enable ZSTD match skipping optimization, and reduce search strength/tolerance for levels 1-4 (Enabled by default)
AOCL_DECOMPRESS_FAST                |  Enable fast decompression modes that might compromise on compression speed / ratio to produce streams that decompress faster. Supported values 1, 2 for ZSTD. (Disabled by default)
AOCL_TEST_COVERAGE                  |  Enable GTest, AOCL test bench and third party test bench based CTest suite (Disabled by default)
AOCL_ENABLE_LOG_FEATURE             |  Enables logging through environment variable `AOCL_ENABLE_LOG` (Disabled by default)
CODE_COVERAGE                       |  Enable source code coverage. Only supported on Linux with the GCC compiler (Disabled by default)
ASAN                                |  Enable Address Sanitizer checks. Only supported on Linux/Debug build (Disabled by default)
VALGRIND                            |  Enable Valgrind checks. Only supported on Linux/Debug and incompatible with ASAN=ON (Disabled by default)
BUILD_DOC                           |  Build documentation for this library (Disabled by default)
AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1  |  Enable LZ4 match skipping optimization strategy-1 based on a larger base step size applied for long distance search (Disabled by default)
AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2  |  Enable LZ4 match skipping optimization strategy-2 by aggressively setting search distance on top of strategy-1. Preferred to be used with Silesia corpus (Disabled by default)
AOCL_LZ4_NEW_PRIME_NUMBER           |  Enable the usage of a new prime number for LZ4 hashing function. Preferred to be used with Silesia corpus (Disabled by default)
AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES   |  Enable storing of additional potential matches to improve compression ratio. Recommended for higher compressibility use cases (Disabled by default)
AOCL_LZ4_HASH_BITS_USED             |  Control the number of bits used for LZ4 hashing, allowed values are OFF, LOW (low perf gain and less CR regression) and HIGH (high perf gain and high CR regression) (LOW by default)
AOCL_EXCLUDE_BZIP2                  |  Exclude BZIP2 compression method from the library build (Disabled by default)
AOCL_EXCLUDE_LZ4                    |  Exclude LZ4 compression method from the library build. LZ4HC also gets excluded (Disabled by default)
AOCL_EXCLUDE_LZ4HC                  |  Exclude LZ4HC compression method from the library build (Disabled by default)
AOCL_EXCLUDE_LZMA                   |  Exclude LZMA compression method from the library build (Disabled by default)
AOCL_EXCLUDE_SNAPPY                 |  Exclude SNAPPY compression method from the library build (Disabled by default)
AOCL_EXCLUDE_ZLIB                   |  Exclude ZLIB compression method from the library build (Disabled by default)
AOCL_EXCLUDE_ZSTD                   |  Exclude ZSTD compression method from the library build (Disabled by default)
AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL |  Build with xz utils lzma APIs. Experimental feature with limited API support (Disabled by default)
AOCL_ENABLE_THREADS                 |  Enable multi-threaded compression and decompression using SMP based openMP threads (Disabled by default)
TEST_COVERAGE_THIRD_PARTY           |  Enable third party test bench based CTest suite (Disabled by default)
NATIVE_ENABLE_THREADS               |  Enable native multi-threaded compression for supported methods (Disabled by default)
AOCL_TEST_FUZZER                    |  Enable fuzz test along with GTest. Only supported on Linux with the Clang compiler (Disabled by default)
AOCL_TEST_FUZZER_WITH_CORPUS        |  Run fuzz tests with corpus. Only supported on Linux with the Clang compiler (Disabled by default)
ENABLE_FAST_MATH                    |  Enable fast-math optimizations (Disabled by default)

* NOTE: <br>
   1. ZLIB supports quicker compression strategy for Level 1 by trading off compression ratio. Enable it by <br>
   setting environment variable AOCL_ZLIB_QUICK_MODE. It also improves performance for levels 2, 3 and 5 <br>
   while trading off compression ratio. <br>

Running tests with CTest
------------------------

CTest is configured in CMake build system to run the test cases implemented with GTest and AOCL Test Bench for Silesia, Calgary, and Canterbury datasets.
To enable testing with CTest, use AOCL_TEST_COVERAGE option while configuring the CMake build.

Following are a few sample commands that can be executed in the build directory to run the test cases with CTest.

 To run all the tests (GTest and Test bench)<br>
 `ctest` 
 
 To only run Test bench<br>
 `ctest -R BENCH`
 
 To run GTest test cases for a specific method<br>
 `ctest -R <METHOD_NAME_IN_CAPITALS>`

Running fuzzer tests
--------------------

To list all the fuzz tests available for a method, use the following command:
   `<METHOD_GTEST_EXECUTABLE> --list_fuzz_tests`
   example: `zlib_gtest --list_fuzz_tests`

Fuzzer test can be run in two modes:

1. Unit test mode: Default operation mode of AOCL_TEST_FUZZER. Can be run as part of ctest. No sanitizer and coverage instrumentation.
   `ctest -R <TestSuiteName>.<FuzzTestName>`
2. Fuzzing mode: Enabled with cmake option FUZZTEST_FUZZING_MODE. Runs each fuzz test with sanitizer and coverage instrumentation

   To run all fuzz tests for a specified duration, use the following command:
   `<METHOD_GTEST_EXECUTABLE> --fuzz_for=<DURATION>`
   example: `zlib_gtest --fuzz_for=60s`

   To run a single fuzz test until a bug is found or until manually stopped:
   `<METHOD_GTEST_EXECUTABLE> --fuzz=<TestSuiteName>.<FuzzTestName>`

   To run a single fuzz test by feeding in an external corpus of seeds: Enabled with cmake option AOCL_TEST_FUZZER_WITH_CORPUS.
   Place folders containing seed files in the directory pointed by environment variable AOCL_FUZZ_CORPUS_DIR.
   Sub-folders under this must be as follows:
   *   /compress_fuzz : Must contain uncompressed raw files for compress API fuzz tests.
   *   /*_fuzz        : Folders with individual fuzz test names must contain compressed files 
                        for respective methods used for decompress API fuzz tests.
                        Example: /LZ4_decompress_safe_fuzz, /RawUncompress_fuzz, etc
   Run the single fuzz test:
   `<METHOD_GTEST_EXECUTABLE> --fuzz=<TestSuiteName>.<FuzzTestName>`
   example: `zlib_gtest --fuzz=AOCL_Compression_zlib.compress2_fuzz`
   Additional seed properties can be specified by environment variables:
   *  AOCL_FUZZ_SIZE_MAX : Max size in bytes to use for i/o buffers used in fuzz testing.
   *  AOCL_FUZZ_CPR_RATIO : Compression ratio estimate of compressed files used for decompress API fuzz tests.

Running Performance Benchmarking
--------------------------------

Use test_speed.py script to benchmark performance and compare AOCL-Compression library with other 
compression libraries such as open-source reference or IPP. It generates summary reports describing compression/decompression speeds and compression ratio.

Following are a few sample commands to use the script available in the 'scripts' directory.

 To print usage options<br>
 `python3 test_speed.py --help` 
 
 To run AOCL optimized vs Reference methods for lz4, snappy and zlib levels 1 and 2:<br>
 `python3 test_speed.py --dataset $PATH_DATASETS_DIR -m lz4 snappy zlib:1 zlib:2 -cw vanilla`
 
 To run AOCL optimized vs IPP for lz4 method:<br>
 `python3 test_speed.py --dataset $PATH_DATASETS_DIR -m lz4 -cw ipp --ipp $IPP_PATCHED_LZ4_LIBS_PATH`
 

Generating Documentation
------------------------
- To generate documentation, specify the `-DBUILD_DOC=ON` option while building.
- Documents will be generated in HTML format in the folder __docs/html__ as doxygen output &  __docs/sphinx/html__ as sphinx output. Open the index.html file from respective folders in any browser to view the documentation.
- The following packages are expected before running CMake with `-DBUILD_DOC=ON` option:
   1. Doxygen.
   2. Python packages:
      - Sphinx
      - rocm_docs
      - breathe
      - myst_parser
- CMake halts if required packages are missing by providing directives for installing the absent packages.

Enabling/disabling optimizations
--------------------------------
- AOCL optimizations can be disabled by setting the environment variable AOCL_DISABLE_OPT to ON.
- Reference code paths are taken in such a scenario.
- This needs to be set before launching the application for it to take effect.
- If optimization is turned off via aocl_compression_desc::optOff (= 1) passed to aocl_llc_setup(), then reference code paths are taken.
- If optimization is turned on  via aocl_compression_desc::optOff (= 0) passed to aocl_llc_setup(), then AOCL_DISABLE_OPT is checked 
  additionally to override aocl_compression_desc::optOff value.

Enabling specific instructions (ISA)
------------------------------------
- AOCL optimizations can be restricted to certain ISAs by setting the environment variable 
  AOCL_ENABLE_INSTRUCTIONS. Supported values are SSE2, AVX, AVX2 and AVX512.
- This ensures optimized code paths with ISAs above the set value are not taken. E.g. If 
  it is set to AVX, no AVX2 and AVX512 optimized code paths are taken.
- This needs to be set before launching the application for it to take effect.
- It takes precedence over aocl_compression_desc::optLevel setting passed to aocl_llc_setup().
- Note: When calling aocl_llc_setup() API from multiple threads, changing aocl_compression_desc::optOff
  and aocl_compression_desc::optLevel values between threads can lead to undefined behaviour.