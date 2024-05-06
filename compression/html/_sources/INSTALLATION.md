Installation
------------

1. Download the latest stable release from the Github repository:<br>
[https://github.amd.com/AOCL/aocl-compression](https://github.amd.com/AOCL/aocl-compression)
2. Install CMake on the machine where the sources are to be compiled.
3. Make any one of the compilers GCC or Clang available on the machine.
4. Then, use the cmake based build system to compile and generate AOCL-Compression <br>
library and testsuite binary as explained below for Linux® and Windows® platforms.

#### Building on Linux

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
   The additional option `--target install` will install the library, binary, and <br>
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

__Note:__ When using cmake version older than 3.15, `-B` option is not supported. <br>
So the build folder must be created manually. <br>
The option `-v` is also not supported in cmake version older than 3.15.


#### Building on Windows

As a prerequisite, make Microsoft Visual Studio® available along with <br>
__Desktop development with C++__ toolset that includes the Clang compiler.

#### Building with Visual Studio IDE (GUI)

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
7. For building multi-threaded library based on AOCL_ENABLE_THREADS, set the 
   LLVM openMP library path in the Linker->General option and openMP library name
   in the Linker->Input under the project properties. Set /openmp as the additional
   compilation option.
8. Build the entire solution or the required projects.

#### Building with Visual Studio IDE (command line)

1. Go to AOCL-Compression source package and create a folder named build.
2. Go to the build folder.
3. Use the following command to configure and build the library to test bench executable.
```
cmake .. -T ClangCl -G <installed Visual Studio version> && cmake --build . --config Release --target INSTALL
```
You can pass additional library configuration and build options in the command.

#### Additional Library Build Options

Use the following additional options to configure your build:

Option                              |  Description
------------------------------------|----------------------------------------------------------------------------------------
AOCL_LZ4_OPT_PREFETCH_BACKWARDS     |  Enable LZ4 optimizations related to backward prefetching of data (Disabled by default)
SNAPPY_MATCH_SKIP_OPT               |  Enable Snappy match skipping optimization (Disabled by default)
LZ4_FRAME_FORMAT_SUPPORT            |  Enable building LZ4 with Frame format and API support (Enabled by default)
AOCL_LZ4HC_DISABLE_PATTERN_ANALYSIS |  Disable Pattern Analysis in LZ4HC for level 9 (Enabled by default)
AOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST|  Enable ZSTD match skipping optimization, and reduce search strength/tolerance for levels 1-4 (Disabled by default)
AOCL_ZSTD_WILDCOPY_LONG             |  Faster wildcopy when match lengths are long in ZSTD decompression (Disabled by default)
AOCL_TEST_COVERAGE                  |  Enable GTest and AOCL test bench based CTest suite (Disabled by default)
AOCL_ENABLE_LOG_FEATURE             |  Enables logging through environment variable `AOCL_ENABLE_LOG` (Disabled by default)
CODE_COVERAGE                       |  Enable source code coverage. Only supported on Linux with the GCC compiler (Disabled by default)
ASAN                                |  Enable Address Sanitizer checks. Only supported on Linux/Debug build (Disabled by default)
VALGRIND                            |  Enable Valgrind checks. Only supported on Linux/Debug and incompatible with ASAN=ON (Disabled by default)
BUILD_DOC                           |  Build documentation for this library (Disabled by default)
ZLIB_DEFLATE_FAST_MODE              |  Enable ZLIB deflate quick strategy (Disabled by default)
AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1  |  Enable LZ4 match skipping optimization strategy-1 based on a larger base step size applied for long distance search (Disabled by default)
AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2  |  Enable LZ4 match skipping optimization strategy-2 by aggressively setting search distance on top of strategy-1. Preferred to be used with Silesia corpus (Disabled by default)
AOCL_LZ4_NEW_PRIME_NUMBER           |  Enable the usage of a new prime number for LZ4 hashing function. Preferred to be used with Silesia corpus (Disabled by default)
AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES   |  Enable storing of additional potential matches to improve compression ratio. Recommended for higher compressibility use cases (Disabled by default)
AOCL_LZ4_HASH_BITS_USED             |  Control the number of bits used for LZ4 hashing, allowed values are LOW (low perf gain and less CR regression) and HIGH (high perf gain and high CR regression) (Disabled by default)
AOCL_EXCLUDE_BZIP2                  |  Exclude BZIP2 compression method from the library build (Disabled by default)
AOCL_EXCLUDE_LZ4                    |  Exclude LZ4 compression method from the library build. LZ4HC also gets excluded (Disabled by default)
AOCL_EXCLUDE_LZ4HC                  |  Exclude LZ4HC compression method from the library build (Disabled by default)
AOCL_EXCLUDE_LZMA                   |  Exclude LZMA compression method from the library build (Disabled by default)
AOCL_EXCLUDE_SNAPPY                 |  Exclude SNAPPY compression method from the library build (Disabled by default)
AOCL_EXCLUDE_ZLIB                   |  Exclude ZLIB compression method from the library build (Disabled by default)
AOCL_EXCLUDE_ZSTD                   |  Exclude ZSTD compression method from the library build (Disabled by default)
AOCL_XZ_UTILS_LZMA_API_EXPERIMENTAL |  Build with xz utils lzma APIs. Experimental feature with limited API support (Disabled by default)
AOCL_ENABLE_THREADS                 |  Enable multi-threaded compression and decompression using SMP based openMP threads (Disabled by default)

#### Running AOCL-Compression Test Bench On Linux

Test bench supports several options to validate, benchmark or debug the supported
compression methods.
It uses the unified API set to invoke the compression methods supported by AOCL-Compression.
Test bench can invoke and benchmark some of the IPP's compression methods as well.

* To check various options supported by the test bench, use one of the following commands:<br>
  `aocl_compression_bench -h`  
  `aocl_compression_bench --help`

* To check all the supported compression methods, use the command:<br>
  `aocl_compression_bench -l`

* To run the test bench with requested number of iterations, use the command:<br>
  `aocl_compression_bench -i`

* To run the test bench to check the performance of all the supported compression <br>
   and decompression methods for a given input file, use the command:<br>
   `aocl_compression_bench -a -p <input filename>`

* To run the test bench to validate the outputs from all the supported compression <br>
   and decompression methods for a given input file, use the command:<br>
   `aocl_compression_bench -a -t <input filename>`

* To run the test bench to check the performance of a compression and decompression <br>
   method for a given input file, use the command:<br>
   `aocl_compression_bench -ezstd:5:0 -p <input filename>`<br>
Here, 5 is the level and 0 is the additional parameter passed to ZSTD method.


* To run the test bench to validate the output of a compression and decompression <br>
   method for a given input file, use the command:<br>
   `aocl_compression_bench -ezstd:5:0 -t <input filename>`<br>
   Here, 5 is the level and 0 is the additional parameter passed to ZSTD method.
  

* To run the test bench with error/debug/trace/info logs, build the library by using `-DAOCL_ENABLE_LOG_FEATURE=ON` & set the environment variable `AOCL_ENABLE_LOG` to any of the following:<br>
   * `AOCL_ENABLE_LOG=ERR`   for Error logs.
   * `AOCL_ENABLE_LOG=INFO`  for Error, Info logs.
   * `AOCL_ENABLE_LOG=DEBUG` for Error, Info, Debug logs.
   * `AOCL_ENABLE_LOG=TRACE` for Error, Info, Debug, Trace logs.


* To run the test bench but only compression or decompression <br>
   for a given input file, use the command:<br>
   `aocl_compression_bench -rcompress <input filename>` or <br>
   `aocl_compression_bench -rdecompress -ezstd <compressed input filename>` or <br>
   `aocl_compression_bench -rdecompress -ezstd -t -f<uncompressed file for validation> <compressed input filename>` <br>
   Note: In -rdecompress mode, compression method must be specified using -e option. <br>
   If validation of decompressed data is needed, specify -t and -f options additionally.

* To run the test bench and dump output data generated <br>
   for a given input file, use the command:<br>
   `aocl_compression_bench -d<dump filename> -ezstd:1 <input filename>` or <br>
   `aocl_compression_bench -d<dump filename> -rcompress -ezstd:1 <input filename>` or <br>
   `aocl_compression_bench -d<dump filename> -rdecompress -ezstd <compressed input filename>` <br>
   Here, when -rcompress operation is selected, compressed file gets dumped <br>
   and when -rdecompress operation is selected, decompressed file gets dumped. <br>
   Method name and level must be specified using -e for default and -rcompress modes. <br>
   Method name must be specified using -e for -rdecompress mode.

---
  
To test and benchmark the performance of IPP's compression methods, use the
test bench option `-c<path to IPP library method>` along with other relevant options (as explained above).
IPP's lz4, lz4hc, zlib and bzip2 methods are supported by the test bench.
Check the following details for the exact steps:
1. Set the library path environment variable (export LD_LIBRARY_PATH on <br>
   Linux) to point to the installed IPP library path. <br>
   Alternatively, you can also run vars.sh that comes along with the <br>
   IPP installation to setup the environment variable.
2. Download lz4-1.9.3, zlib-1.2.11 and bzip2-1.0.8 source packages.
3. Apply IPP patch files using the command:<br>
   `patch -p1 < path to corresponding patch file>`

4. Build the patched IPP lz4, zlib and bzip2 libraries per the steps <br>
   in the IPP readme files in the corresponding patch file <br>
   locations for these compression methods.
5. Append the library path to `-c` option and pass it to executable as command line argument <br>
   (Linux is only supported) for running patched IPP lz4, zlib and bzip2 libraries.
6. Run the test bench to benchmark the IPP library methods as follows:
```
    aocl_compression_bench -a -p -c/path/to/ipp_patch <input filename>
    aocl_compression_bench -elz4 -p -c/path/to/ipp_patch <input filename>
    aocl_compression_bench -elz4hc -p -c/path/to/ipp_patch <input filename>
    aocl_compression_bench -ezlib -p -c/path/to/ipp_patch <input filename>
    aocl_compression_bench -ebzip2 -p -c/path/to/ipp_patch <input filename>
```

#### Running AOCL-Compression Test Bench On Windows

Test bench on Windows supports all the user options as Linux,
except for the `-c` option to link and test IPP compression methods.
For more information on various user options, refer to the previous section on Linux.
To set and launch the test bench with a specific user option,
go to project aocl_compression_bench -> Properties -> Debugging;
specify the user options and the input test file.

#### Running tests with CTest

CTest is configured in CMake build system to run the test cases implemented with GTest and AOCL Test Bench for Silesia, Calgary, and Canterbury datasets.
To enable testing with CTest, use AOCL_TEST_COVERAGE option while configuring the CMake build.

Following are a few sample commands that can be executed in the build directory to run the test cases with CTest.

 To run all the tests (GTest and Test bench)<br>
 `ctest` 
 
 To only run Test bench<br>
 `ctest -R BENCH`
 
 To run GTest test cases for a specific method<br>
 `ctest -R <METHOD_NAME_IN_CAPITALS>`

#### Running Performance Benchmarking

Use test_speed.py script to benchmark performance and compare AOCL-Compression library with other 
compression libraries such as open-source reference or IPP. It generates summary reports describing compression/decompression speeds and compression ratio.

Following are a few sample commands to use the script available in the 'scripts' directory.

 To print usage options<br>
 `python3 test_speed.py --help` 
 
 To run AOCL optimized vs Reference methods for lz4, snappy and zlib levels 1 and 2:<br>
 `python3 test_speed.py --dataset $PATH_DATASETS_DIR -m lz4 snappy zlib:1 zlib:2 -cw vanilla`
 
 To run AOCL optimized vs IPP for lz4 method:<br>
 `python3 test_speed.py --dataset $PATH_DATASETS_DIR -m lz4 -cw ipp --ipp $IPP_PATCHED_LZ4_LIBS_PATH`
 

#### Generating Documentation

- To generate documentation, specify the `-DBUILD_DOC=ON` option while building.
- Documents will be generated in HTML format in the folder __docs/html__ . Open the index.html file in any browser to view the documentation.
- CMake will use the existing Doxygen if available. Else, it will prompt the user to install doxygen and try again.

#### Enabling/disabling optimizations

- AOCL optimizations can be disabled by setting the environment variable AOCL_DISABLE_OPT to ON.
- Reference code paths are taken in such a scenario.
- This needs to be set before launching the application for it to take effect.
- If optimization is turned off via aocl_compression_desc::optOff (= 1) passed to aocl_llc_setup(), then reference code paths are taken.
- If optimization is turned on  via aocl_compression_desc::optOff (= 0) passed to aocl_llc_setup(), then AOCL_DISABLE_OPT is checked 
  additionally to override aocl_compression_desc::optOff value.

#### Enabling specific instructions (ISA)

- AOCL optimizations can be restricted to certain ISAs by setting the environment variable 
  AOCL_ENABLE_INSTRUCTIONS. Supported values are SSE2, AVX, AVX2 and AVX512.
- This ensures optimized code paths with ISAs above the set value are not taken. E.g. If 
  it is set to AVX, no AVX2 and AVX512 optimized code paths are taken.
- This needs to be set before launching the application for it to take effect.
- It takes precedence over aocl_compression_desc::optLevel setting passed to aocl_llc_setup().
- Note: When calling aocl_llc_setup() API from multiple threads, changing aocl_compression_desc::optOff
  and aocl_compression_desc::optLevel values between threads can lead to undefined behaviour.
