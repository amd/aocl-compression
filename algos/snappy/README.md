Snappy - Introduction
================================

Introduction
============

Snappy is a compression/decompression library. It does not aim for maximum
compression or compatibility with any other compression library; instead,
it aims for very high speeds and reasonable compression.

Snappy has the following properties:

 * Fast: Compared to the fastest mode of zlib, Snappy is faster for most inputs.
 * Stable: Over the last few years, Snappy has compressed and decompressed
   Petabytes of data in Google's production environment. The Snappy bitstream
   format is stable and does not change between versions.
 * Robust: The Snappy decompressor is designed not to crash in the face of
   corrupted or malicious input.
 * Free and open-source software: Snappy is licensed under a BSD-type license.
   For more information, refer to the included COPYING file.

Snappy has previously been called "Zippy" in some Google presentations
and the like.


Performance
===========


Although Snappy should be fairly portable, it is primarily optimized
for 64-bit x86-compatible processors, and may run slower in other environments.
In particular:

 - It uses 64-bit operations in several places to process more data at
   once.
 - It assumes unaligned 32 and 64-bit loads and stores are cheap.
   On some platforms, these must be emulated with single-byte loads
   and stores, which is much slower.
 - It assumes little-endian throughout, and needs to byte-swap data in
   several places if running on a big-endian platform.

Even heavily tuned code can be improved.

