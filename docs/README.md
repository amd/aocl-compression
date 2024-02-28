AOCL COMPRESSION
================
@mainpage

#### Introduction
AOCL Compression is a software framework of various lossless compression and
decompression methods tuned and optimized for AMD Zen™ based CPUs.
This framework offers a single set of unified APIs for all the supported
compression and decompression methods which facilitate the applications to
easily integrate and use them. For AOCL Compression framework APIs, refer to @ref API.

#### Methods Supported
AOCL Compression supports compression and decompression methods of the following libraries along with their native APIs.
1. @ref LZ4_API "lz4"
2. @ref ZLIB_API "zlib/deflate"
3. @ref LZMA_API "lzma"
4. @ref ZSTD_API "zstd"
5. @ref BZIP2_API "bzip2" 
6. @ref SNAPPY_API "snappy"
7. @ref LZ4HC_API "lz4hc"

It supports the dynamic dispatcher feature that executes the most optimal
function variant implemented using Function Multi-versioning thereby offering
a single optimized library portable across different x86 CPU architectures.
AOCL Compression framework is developed in C for UNIX® and Windows® based systems.
A test suite is provided for the validation and performance benchmarking
of the supported compression and decompression methods. This suite also
supports the benchmarking of IPP compression methods, such as, lz4, lz4hc, zlib and bzip2.
The library build framework offers CTest-based testing of the test cases
implemented using GTest and the library test suite. Starting from AOCL-Compression 4.2,
the library offers openMP based multi-threaded compression for a few methods.

#### Contacts

AOCL Compression is developed and maintained by AMD.<br>
For support, send an email to toolchainsupport@amd.com.
