AOCL-Compression Examples
=========================

This directory contains sample source files showing
usage of AOCL-Compression library functions.
Use the provided cmake script file "CMakeLists.txt" 
to compile and run the programs. Same cmake script may
be used for both Linux and Windows platforms.

Building on Linux
-----------------

1. To create a build directory and configure the build system in it, run the following:
   ```
    cmake -B <build directory> <CMakeList.txt filepath>
   ```
   Additional options that can be specified for build configuration are:
   ```
   cmake -B <build directory> <CMakeList.txt filepath> 
      -DAOCL_COMPRESSION_INC_DIR=<include directory path of AOCL Compression library> 
      -DAOCL_COMPRESSION_LIB_DIR=<lib directory path of AOCL Compression library> 
   ```

2. Compile using the following command:
   ```
   cmake --build <build directory>
   ```
   The example executables are generated in "build". <br>

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
4. Click __Generate__.
   Microsoft Visual Studio project is generated.
5. Click __Open Project__.
   Microsoft Visual Studio project for the source package __is launched__.
6. Build the entire solution or the required projects.

Building with Visual Studio IDE (command line)
----------------------------------------------
1. Go to examples/ folder in AOCL-Compression install package and create a folder named build.
2. Go to the build folder.
3. Use the following command to configure and build example executables.
```
cmake .. -T ClangCl -G <installed Visual Studio version> && cmake --build . --config Release
```

Running AOCL-Compression Examples
---------------------------------

* CAUTION: <br>
   Before running the example programs, check whether it points to the right library dependency. <br>

Example programs are provided for both unified API and native APIs of each compression method.

* To run example program for unified API, use the command:<br>
  `example_unified_api <input filename>`

* To run example program for LZ4 native API, use the command:<br>
  `example_LZ4_compress_default <input filename>`

CONTACTS
--------
AOCL-Compression is developed and maintained by AMD.<br>
For support, send an email to toolchainsupport@amd.com.
