..  Copyright (C) 2024, Advanced Micro Devices. All rights reserved.

..  Redistribution and use in source and binary forms, with or without
..  modification, are permitted provided that the following conditions are met:

..  1. Redistributions of source code must retain the above copyright notice,
..  this list of conditions and the following disclaimer.
..  2. Redistributions in binary form must reproduce the above copyright notice,
..  this list of conditions and the following disclaimer in the documentation
..  and/or other materials provided with the distribution.
..  3. Neither the name of the copyright holder nor the names of its
..  contributors may be used to endorse or promote products derived from this
..  software without specific prior written permission.

..  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
..  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
..  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
..  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
..  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
..  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
..  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
..  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
..  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
..  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
..  POSSIBILITY OF SUCH DAMAGE.

================
AOCL COMPRESSION
================


Introduction
`````````````
AOCL Compression is a software framework of various lossless compression and
decompression methods tuned and optimized for AMD Zen™ based CPUs.
This framework offers a single set of unified APIs for all the supported
compression and decompression methods which facilitate the applications to
easily integrate and use them. For AOCL Compression framework APIs, refer to :ref:`ref-unified-api`.


Methods Supported
..................

AOCL Compression supports compression and decompression methods of the following libraries along with their native APIs.

#. :ref:`ref-lz4`
#. :ref:`ref-zlib`
#. :ref:`ref-lzma`
#. :ref:`ref-zstd`
#. :ref:`ref-bzip2`
#. :ref:`ref-snappy`
#. :ref:`ref-lz4hc`

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

.. toctree::
   :maxdepth: 1
   :caption: Table of Contents

   INSTALLATION
   unified_api
   algos
   multi-thread
   INTRODUCTION
   EXAMPLE_README

Contacts
........

AOCL Compression is developed and maintained by AMD.
For support, send an email to toolchainsupport@amd.com.
