AOCL COMPRESSION
================
@mainpage
AOCL Compression is a software framework of various lossless compression and
decompression methods tuned and optimized for AMD Zen™ based CPUs.
The framework offers a single set of unified APIs for all the supported
compression and decompression methods which facilitate the applications to
easily integrate and use them. For AOCL Compression framework API's refer @ref API.

AOCL Compression supports the following libraries's compression and decompression methods along with their native APIs.
1. @ref LZ4_API "lz4"
2. @ref ZLIB_API "zlib/deflate"
3. @ref LZMA_API "lzma"
5. zstd
6. @ref BZIP2_API "bzip2" 
7. @ref SNAPPY_API "snappy"
8. @ref LZ4HC_API "lz4hc"

It supports a dynamic dispatcher feature that executes the most optimal
function variant implemented using Function Multi-versioning thereby offering
a single optimized library portable across different x86 CPU architectures.
AOCL Compression framework is developed in C for Unix and Windows based systems.
A test suite is provided for validation and performance benchmarking
of the supported compression and decompression methods. The test suite also
supports the benchmarking of IPP compression methods like lz4, lz4hc, zlib and bzip2.

CONTACTS
--------
AOCL Compression is developed and maintained by AMD.
You can contact us on the email-id toolchainsupport@amd.com.
