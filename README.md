AOCL CODEC
==========

AOCL Codec is a software framework of various lossless compression and
decompression methods tuned and optimized for AMD Zen based CPUs.
The framework offers a single set of unified APIs for all the supported
compression and decompression methods which facilitate the applications to
easily integrate and use them.
AOCL codec supports lz4, zlib/deflate, lzma, zstd, bzip2, snappy, and lz4hc
based compression and decompression methods along with their native APIs.
AOCL Codec framework is developed in C for Unix based systems.
A test suite is also provided for validation and performance benchmarking
of the supported compression and decompression methods.


INSTALLATION
------------

Download the latest stable release from the Github.
Install cmake on the machine where the sources are to be compiled.
Make available any one of the compilers GCC or Clang on the machine.
Then, use the cmake based build system to compile and generate AOCL Codec
library and testsuite binary as explained below.
1. To create a build directory and configure the build system in it,
   run the below command:
   "cmake -B <build dir> <CMakeList.txt filepath>"
   Additional options can be specified for build type and installation path as:
   "cmake -B <build dir> <CMakeList.txt filepath> 
   -DCMAKE_INSTALL_PREFIX=<install path>
   -DCMAKE_BUILD_TYPE=<Debug or Release>
   -DBUILD_STATIC_LIBS=ON"
2. Compile using the command "cmake --build <build dir> --target install -j".
   The library will be generated in the directory "lib".
   The test bench executable will be generated inside "build" directory itself.
   The additional option "--target install" will install the library, binary
   and interface header files in the installation path as specified with
   -DCMAKE_INSTALL_PREFIX option or in the local system path otherwise.
   The option "-j" will run the compilation process using multiple cores.
3. To uninstall the installed files, run the below custom command provided:
   "cmake --build . --target uninstall"
4. To clear or delete the build folder or files, manually remove the
   build directory or its files.


RUNNING AOCL CODEC TEST BENCH
-----------------------------

Test bench supports several options to validate, benchmark or debug a supported
compression method. It uses the unified API set as supported by AOCL Codec.

To check various options supported by the test bench, use the command:
	aocl_codec_bench -h
	
To check all the supported compression methods, use the command:
	aocl_codec_bench -l
	
To run the test bench with requested number of iterations, use the command:
	aocl_codec_bench -i

To run the test bench and check the performance of all the supported
compression and decompression methods for a given input file, use the command:
	aocl_codec_bench -a -p <input filename>

To run the test bench and validate the outputs from all the supported
compression and decompression methods for a given input file, use the command:
	aocl_codec_bench -a -t <input filename>

To run the test bench and check the performance of a particular
compression and decompression method for a given input file, use the command:
	aocl_codec_bench -ezstd:5:0 -p <input filename>
Here, 5 is the level and 0 is the additional parameter passed to ZSTD method.

To run the test bench and validate the output of a particular
compression and decompression method for a given input file, use the command:
	aocl_codec_bench -ezstd:5:0 -t <input filename>
Here, 5 is the level and 0 is the additional parameter passed to ZSTD method.

To run the test bench with error/debug/trace/info logs, use the command:
	aocl_codec_bench -a -t -v <input filename>
Here, -v can be passed with a number like v<n> that can take values: 
	1 for Error (default), 2 for Info, 3 for Debug, 4 for Trace.

 
CONTACTS
--------
AOCL Codec is developed and maintained by AMD.
You can contact us on the email-id aoclsupport@amd.com.