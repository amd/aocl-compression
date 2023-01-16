
@page Build Build and Installation



INSTALLATION
------------

1. Download the latest stable release from the Github repository:
https://github.amd.com/AOCL/aocl-compression
2. Install cmake on the machine where the sources are to be compiled.
3. Make available any one of the compilers GCC or Clang on the machine.
4. Then, use the cmake based build system to compile and generate AOCL Compression
library and testsuite binary as explained below for Linux and Windows platforms.

BUILDING ON LINUX
-----------------

1. To create a build directory and configure the build system in it, run the below command:
   ```
    cmake -B <build directory> <CMakeList.txt filepath>
   ```
   Additional options that can be specified for build configuration are:
   ```
   cmake -B <build directory> <CMakeList.txt filepath> 
      -DCMAKE_INSTALL_PREFIX=<install path> 
      -DCMAKE_BUILD_TYPE=<Debug or Release> 
      -DBUILD_STATIC_LIBS=ON
      -DBUILD_DOC=ON
   ```

   To use clang compiler for the build, specify -DCMAKE_C_COMPILER=clang as the option.
2. Compile using the below command:
   ```
   cmake --build <build directory> --target install -j
   ```
   The library will be generated in the "lib" directory.
   The test bench executable will be generated inside "build" directory itself.
   The additional option 

   `--target install`
   will install the library, binary and interface header files in the installation path as specified with -DCMAKE_INSTALL_PREFIX option or in the local system path otherwise.
   The option "-j" will run the compilation process using multiple cores.
3. To uninstall the installed files, run the below custom command provided:
   ```
   cmake --build <build directory> --target uninstall
   ```
   To uninstall and then install the build package, run the below command:
   ```
   cmake --build <build directory> --target uninstall --target install -j -v
   ```
   The option `-v` will print verbose build logs on the console.
4. To clear or delete the build folder or files, manually remove the build directory or its files.

Note: When using cmake with version lesser than 3.15, `-B` option is not supported,
so the build folder needs to be created manually.
The option `-v` is also not supported in cmake with version lesser than 3.15.

BUILD OPTIONS
-------------
You can use the following additional options for configuring your build.

Option                           |  Description
---------------------------------|----------------------------------------------------------------------------------------
AOCL_LZ4_OPT_PREFETCH_BACKWARDS  |	Enable LZ4 optimizations related to backward prefetching of data (Disabled by default)
SNAPPY_MATCH_SKIP_OPT	         |	Enable Snappy match skipping optimization (Disabled by default)
LZ4_FRAME_FORMAT_SUPPORT         |	Enable building LZ4 with Frame format and API support (Disabled by default)
BUILD_DOC                        |	Build documentation for this library (Defult value is off)

BUILDING ON WINDOWS
-------------------
As the prerequisites, make available Microsoft Visual Studio along with it's
   "Desktop development with C++" toolset that includes the Clang compiler.

BUILDING WITH VISUAL STUDIO IDE (GUI)
-------------------------------------
1. Launch CMake GUI and set the locations for source package and build output.
   Click the configure option and choose:
   Generator as the installed Visual Studio version,
   Platform as x64,
   Optional toolset as ClangCl.
   Click the Generate option.
   After Microsoft Visual Studio project is generated, click "Open Project".
   This will launch the Microsoft Visual Studio project for the source package.
2. To build a Static library, go to project aocl_compression -> Properties and then:
   General -> "Configuration Type" and set it to Static library,
   C/C++ -> "Code Generation" -> "Runtime Library" and set it to /MT or /MTd
   depending upon whether the build type is Release or Debug.
3. To build a Dynamic library, go to project aocl_compression -> Properties and then:
   General -> "Configuration Type" and set it to Dynamic library,
   C/C++ -> "Code Generation" -> "Runtime Library" and set it to /MD or /MDd
   depending upon whether the build type is Release or Debug.
4. To build the test bench binary, go to project aocl_compression_bench -> Properties and then:
   Linker -> General -> "Additional Library Directories" and add the path to find aocl_compression library,
   Linker -> General -> "Link Library Dependencies" and set it as "Yes",
   Linker -> Input -> "Additional Dependencies" and add the aocl_compression library name.
5. Build the entire solution or both the projects one by one separately.

BUILDING WITH VISUAL STUDIO IDE (command line)
----------------------------------------------
1. Go to aocl-compression source package and create a folder named build.
2. Go to the build folder.
3. Use the below command to configure and build the library and test bench executable.
```
cmake .. -T ClangCl -G <installed Visual Studio version> && cmake --build . --config Release --target INSTALL
```
      Additional config and build options can be passed to the above command.

RUNNING AOCL COMPRESSION TEST BENCH ON LINUX
--------------------------------------

Test bench supports several options to validate, benchmark or debug the supported
compression methods.
It uses the unified API set to invoke the compression methods supported by AOCL Compression.
Test bench can invoke and benchmark some of the IPP's compression methods as well.

* To check various options supported by the test bench, use the command:
  `aocl_compression_bench -h`
   Or,
  `aocl_compression_bench --help`
* To check all the supported compression methods, use the command:
  `aocl_compression_bench -l`

* To run the test bench with requested number of iterations, use the command:
  `aocl_compression_bench -i`

* To run the test bench and check the performance of all the supported
compression and decompression methods for a given input file, use the command:
   `aocl_compression_bench -a -p <input filename>`

* To run the test bench and validate the outputs from all the supported
compression and decompression methods for a given input file, use the command:
   `aocl_compression_bench -a -t <input filename>`

* To run the test bench and check the performance of a particular
compression and decompression method for a given input file, use the command:
   `aocl_compression_bench -ezstd:5:0 -p <input filename>`
Here, 5 is the level and 0 is the additional parameter passed to ZSTD method.


* To run the test bench and validate the output of a particular
compression and decompression method for a given input file, use the command:
   `aocl_compression_bench -ezstd:5:0 -t <input filename>`
Here, 5 is the level and 0 is the additional parameter passed to ZSTD method.
  


* To run the test bench with error/debug/trace/info logs, use the command:
   `aocl_compression_bench -a -t -v <input filename>`
Here, `-v` can be passed with a number like v<n> that can take values: 
	1 for Error (default), 2 for Info, 3 for Debug, 4 for Trace.

---
  
To test and benchmark the performance of IPP's compression methods, use the
test bench option `-c` along with other relevant options (as explained above).
Currently, IPP's lz4, lz4hc and zlib methods are supported by the test bench.
Check the following details for the exact steps:
1. Set the library path environment variable (export LD_LIBRARY_PATH on
   Linux) to point to the installed IPP library path.
   Alternatively, one can also run vars.sh that comes along with the
   IPP installation to setup the environment variable.
2. Download lz4-1.9.3 and zlib-1.2.11 source packages.
3. Apply IPP's lz4 and zlib patch files as per below command:
   `patch -p1 < path to corresponding patch file>`

4. Build the patched IPP lz4 and zlib libraries as per the steps given
   in IPP's readme files present in the corresponding patch file
   locations for these compression methods.
5. Set the library path environment variable (export LD_LIBRARY_PATH on
   Linux) to point to patched IPP lz4 and zlib libraries.
6. Run the test bench as given below to benchmark IPP library methods:
```
    aocl_compression_bench -a -p -c <input filename>
    aocl_compression_bench -elz4 -p -c <input filename>
    aocl_compression_bench -elz4hc -p -c <input filename>
    aocl_compression_bench -ezlib -p -c <input filename>
```

RUNNING AOCL COMPRESSION TEST BENCH ON WINDOWS
----------------------------------------

Test bench on Windows supports all the user options as supported on Linux
except for the "-c" option to link and test IPP's compression methods.
Refer the previous section on Linux to learn about the various user options.
To set and launch the test bench with a specific user option,
go to project aocl_compression_bench -> Properties -> Debugging and
specify the user options and the input test file.

GENERATING DOCUMENTATION
------------------------
- To generate documentation, specify the `-DBUILD_DOC=ON` option while building.
- Documents will be generated in HTML format inside the folder __docs/html__ , you can open the index.html file with any browser to view the documentation.
- CMake will use the existing Doxygen if available or else it will download the respective Doxygen binaries according to the OS you are using to build the documentation.