..  Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.

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

.. _ref-zstd:

ZSTD API
--------

.. doxygengroup:: ZSTD_API
   :project: compression
   :members:


.. _ZSTD_compress_advanced:

Deprecated function
===================

.. function:: size_t ZSTD_compress_advanced(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize, const void* dict, size_t dictSize, ZSTD_parameters params)

   :brief: This function compresses data using custom compression parameters specified in a ``ZSTD_parameters`` structure. It supports both single-threaded and multi-threaded compression.

   .. list-table::
      :widths: 10 10 70
      :header-rows: 1

      * - Parameter
        - Direction
        - Description
      * - ``cctx``
        - in, out
        - Explicit ZSTD compression context. When compressing many times, it is recommended to allocate the context just once and re-use it for each successive compression operation.
      * - ``dst``
        - out
        - Destination buffer, compressed data is kept here, memory should be allocated already.
      * - ``dstCapacity``
        - in
        - Size of buffer ``dst`` (which must be already allocated).
      * - ``src``
        - in
        - Source buffer, the data which you want to compress is copied/or pointed here.
      * - ``srcSize``
        - in
        - Size of buffer ``src``.
      * - ``dict``
        - in
        - Dictionary buffer.
      * - ``dictSize``
        - in
        - Size of the dictionary buffer.
      * - ``params``
        - in
        - Compression parameters controlling compression level, window size, and strategy. These parameters must be initialized before calling the function.

   :return:

   .. list-table::
      :widths: 20 80
      :header-rows: 1

      * - Result
        - Description
      * - Success
        - The number of bytes written into ``dst`` (necessarily <= dstCapacity) on success.
      * - Failure
        - An error code on failure.
