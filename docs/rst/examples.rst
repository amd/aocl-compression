Example Test Programs
=====================

.. list-table::
   :header-rows: 1

   * - Example
     - Description
   * - `Example 1`_
     - Sample usage and calling sequence of AOCL-Compression APIs to compress and decompress a test input.
   * - `Example 2`_
     - Sample usage and calling sequence of AOCL-Compression APIs to extract format compliant compressed stream from a stream produced by AOCL multi-threaded compressor.
   * - `Example 3`_
     - Sample usage and calling sequence of AOCL-Compression APIs for multi-threaded gzip compression.

Example 1
---------

The following test program shows the sample usage and calling sequence of aocl - compression APIs to compress and decompress a test input :

.. literalinclude:: ../../examples/api/example_unified_api.c
   :language: c
   :linenos:
   :lines: 42-135

To build this example test program on a Linux system using GCC or AOCC, you must specify
path to aocl_compression.h header file and link with libaocl_compression.so file as follows:

``gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so directory path> -laocl_compression -Wl,-rpath=<libaocl_compression.so directory path>``

Before running the example program, ensure it points to the right library dependencies for openMP, etc.


Example 2
---------

The following test program shows the sample usage and calling sequence of aocl - compression APIs to extract format compliant compressed stream from a stream produced by AOCL multi-threaded compressor :

.. note:: Build AOCL-Compression library with ``AOCL_ENABLE_THREADS``.

.. literalinclude:: ../../examples/api/example_aocl_llc_skip_rap_frame.c
   :language: c
   :linenos:
   :lines: 45-142

To build this example test program on a Linux system using GCC or AOCC, you must specify
path to aocl_compression.h header file and link with libaocl_compression.so file as follows:

``gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so directory path> -laocl_compression -Wl,-rpath=<libaocl_compression.so directory path>``

Before running the example program, ensure it points to the right library dependencies for openMP, etc.

Example 3
---------

The following test program shows the sample usage and calling sequence of aocl - compression APIs for multi-threaded gzip compression :

.. note:: Build AOCL-Compression library with ``AOCL_ENABLE_THREADS``.

.. literalinclude:: ../../examples/zlib/example_compress2_gzip.c
   :language: c
   :linenos:
   :lines: 43-109


To build this example test program on a Linux system using GCC or AOCC, you must specify
path to aocl_compression.h header file and link with libaocl_compression.so file as follows:

``gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so directory path> -laocl_compression -Wl,-rpath=<libaocl_compression.so directory path>``

Before running the example program, ensure it points to the right library dependencies for openMP, etc.
