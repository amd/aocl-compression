Brief Overview of algos
=======================

LZ4 - Introduction
------------------

LZ4 is a lossless compression algorithm
providing a compression speed > 500 MB/s per core,
scalable with multi-cores CPU.
It features an extremely fast decoder
with speed in multiple GB/s per core,
reaching RAM speed limits on multi-core systems.

Speed can be tuned dynamically, selecting an "acceleration" factor
which trades compression ratio for faster speed.
On the other end, a high compression derivative, LZ4_HC, is also provided,
trading CPU time for improved compression ratio.
All the versions feature the same decompression speed.

LZ4 is also compatible with [dictionary compression](https://github.com/facebook/zstd#the-case-for-small-data-compression),
both at [API](https://github.com/lz4/lz4/blob/v1.8.3/lib/lz4frame.h#L481) and [CLI](https://github.com/lz4/lz4/blob/v1.8.3/programs/lz4.1.md#operation-modifiers) levels.
It can ingest any input file as dictionary, though only the final 64 KB is used.
This capability can be combined with the [Zstandard Dictionary Builder](https://github.com/facebook/zstd/blob/v1.3.5/programs/zstd.1.md#dictionary-builder)
to improve the compression performance on small files.


LZ4 library is provided as an open-source software using BSD 2-Clause license.

### Documentation

The raw LZ4 block compression format is detailed within [lz4_Block_format].

Arbitrarily, for streaming requirements, long files or data streams are compressed using multiple blocks. These blocks are organized into a frame,
defined into [lz4_Frame_format].
Interoperable versions of LZ4 must also respect the frame format.

[lz4_Block_format]: https://github.com/lz4/lz4/blob/dev/doc/lz4_Block_format.md
[lz4_Frame_format]: https://github.com/lz4/lz4/blob/dev/doc/lz4_Frame_format.md


### Other source versions

Beyond the C reference source,
many contributors have created versions of LZ4 in multiple languages
(Java, C#, Python, Perl, Ruby, and so on).
A list of known source ports is maintained on the [LZ4 Homepage].

[LZ4 Homepage]: http://www.lz4.org

### LZ4 - Library Files

The `/lib` directory contains many files, but depending on the project's objectives,
not all of them are necessary.

### Minimal LZ4 build

The minimum required is **lz4.c** and **lz4.h**,
which provides the fast compression and decompression algorithms.
They generate and decode data using the [LZ4 block format].


### High Compression variant

For more compression ratio at the cost of compression speed,
the High Compression variant called **lz4hc** is available.
Add the files **lz4hc.c** and **lz4hc.h**.
This variant also compresses data using the [LZ4 block format]
and depends on the regular `lib/lz4.*` source files.


### Frame Support for Interoperability

To produce compressed data compatible with `lz4` command line utility,
it's necessary to use the [official interoperable frame format].
This format is generated and decoded automatically by the **lz4frame** library.
Its public API is described in `lib/lz4frame.h`.
To work properly, lz4frame needs all other modules present in `/lib`,
including, lz4, lz4hc, and **xxhash**.
So it's necessary to include all the `*.c` and `*.h` files present in `/lib`.


### Advanced / Experimental API

Definitions which are not guaranteed to remain stable in future versions,
are protected behind macros, such as `LZ4_STATIC_LINKING_ONLY`.
As the name strongly implies, these definitions should only be invoked
in the context of static linking ***only***.
Otherwise, the dependent application may fail on API or ABI break in the future.
The associated symbols are also not exposed by the dynamic library by default.
If required, it's possible to force their publication
by using build macros `LZ4_PUBLISH_STATIC_FUNCTIONS`
and `LZ4F_PUBLISH_STATIC_FUNCTIONS`.


### Build macros

The following build macro can be selected to adjust the source code behavior during the compilation:

- `LZ4_FAST_DEC_LOOP` : Triggers a speed optimized decompression loop, more powerful on modern CPU.
  This loop works optimally on `x86`, `x64` and `aarch64` CPU, and is automatically enabled for them.
  It's also possible to enable or disable it manually by passing `LZ4_FAST_DEC_LOOP=1` or `0` to the preprocessor.
  For example, with `GCC` : `-DLZ4_FAST_DEC_LOOP=1`
  and with `make` : `CPPFLAGS+=-DLZ4_FAST_DEC_LOOP=1 make lz4`.

- `LZ4_DISTANCE_MAX` : Controls the maximum offset that the compressor will allow.
  Set to 65535 by default, which is the maximum value supported by LZ4 format.
  Reducing the maximum distance will reduce opportunities for LZ4 to find matches and
  hence, will produce a worse compression ratio.
  However, a smaller maximum distance can allow compatibility with specific decoders using a limited memory.
  This build macro only influences the compressed output of the compressor.

- `LZ4_DISABLE_DEPRECATE_WARNINGS` : Invoking a deprecated function will make the compiler generate a warning.
  This is meant to invite the users to update their source code.
  Should this be a problem, it's possible to make the compiler ignore these warnings,
  for example, with `-Wno-deprecated-declarations` on `GCC`
  or `_CRT_SECURE_NO_WARNINGS` for Microsoft Visual Studio.
  This build macro offers another project-specific method
  by defining `LZ4_DISABLE_DEPRECATE_WARNINGS` before including the LZ4 header files.

- `LZ4_USER_MEMORY_FUNCTIONS` : Replace calls to <stdlib>'s `malloc`, `calloc` and `free`
  by user defined functions, which must be called `LZ4_malloc()`, `LZ4_calloc()`, and `LZ4_free()`.
  User functions must be available at link time.

- `LZ4_FORCE_SW_BITCOUNT` : By default, the compression algorithm tries to determine lengths
  using bit count instructions, generally implemented as fast single instructions in many CPUs.
  In case, the target CPUs don't support it, compiler intrinsic doesn't work, or feature bad performance,
  it's possible to use an optimized software path instead.
  This is achieved by setting the build macros .
  In most cases, it can be considered for rare platforms.

- `LZ4_ALIGN_TEST` : Alignment test ensures that the memory area
  passed as an argument to become a compression state is suitably aligned.
  This test can be disabled if it proves flaky, by setting this value to 0.


### License

All the source material in __lib__ directory is BSD 2-Clause licensed.
For more information, refer to [LICENSE](../../algos/lz4/LICENSE) for details.
This license is also reminded at the top of each source file.

LZ4HC - Introduction
--------------------
LZ4HC is the High Compression variant of LZ4.
It offers more compression ratio at the cost of compression speed.
The files lz4hc.c and lz4hc.h add the support for LZ4HC.

This variant also compresses data using the LZ4 block format, 
and depends on regular lib/lz4.* source files.

It is covered by the same LICENSE as that of LZ4.

LZMA - Introduction
-------------------
LZMA / LZMA2 are default and general compression methods of 7z format in the 7-Zip program.
LZMA provides a high compression ratio and fast decompression, so it is very suitable for 
embedded applications. For example, it can be used for ROM (firmware) compressing.

LZMA features:

Compression speed: 3 MB/s on 3 GHz dual-core CPU.
Decompression speed:
20-50 MB/s on modern 3 GHz CPU (Intel, AMD, ARM).
Small memory requirements for decompression: 8-32 KB + DictionarySize
Small code size for decompression: 2-8 KB (depending on speed optimizations)

The LZMA decoder uses only CPU integer instructions and can be implemented for any modern 32-bit CPU.

Snappy - Introduction
----------------------

### Introduction


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


### Performance


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

ZLIB - Introduction
-------------------

zlib 1.3 is a general-purpose data compression library.  All the code is
thread safe.  The data format used by the zlib library is described by
Request for Comments (RFCs) 1950 to 1952 in the files
[http://tools.ietf.org/html/rfc1950](http://tools.ietf.org/html/rfc1950) (zlib format), rfc1951 (deflate format), and
rfc1952 (gzip format).

All functions of the compression library are documented in the file zlib.h (volunteer to write man pages welcome, contact zlib@gzip.org).


### Notes:

- For 64-bit Irix, deflate.c must be compiled without any optimization. With
  -O, one libpng test fails. The test works in 32-bit mode (with the -n32
  compiler flag). The compiler bug has been reported to SGI.

- zlib doesn't work with GCC 2.6.3 on a DEC 3000/300LX under OSF/1 2.1. It works
  when compiled with cc.

- On Digital Unix 4.0D (formerly OSF/1) on AlphaServer, the CC option -std1 is
  necessary to get gzprintf working correctly. This is done by configure.

- zlib doesn't work on HP-UX 9.05 with some versions of /bin/cc. It works with
  the other compilers. Use "make test" to check your compiler.

- gzdopen is not supported on RISCOS or BEOS.

- For PalmOs, refer to [http://palmzlib.sourceforge.net/](http://palmzlib.sourceforge.net/)

ZSTD - Introduction
-------------------
__Zstandard__, or `zstd` as short version, is a fast lossless compression algorithm,
targeting real-time compression scenarios at zlib-level and better compression ratios.
It's backed by a very fast entropy stage, provided by [Huff0 and FSE library](https://github.com/Cyan4973/FiniteStateEntropy).

The project is provided as an open-source dual [BSD](../../algos/zstd/LICENSE) and [GPLv2](https://github.com/facebook/zstd/blob/dev/COPYING) licensed **C** library,
and a command line utility producing and decoding `.zst`, `.gz`, `.xz` and `.lz4` files.
Should your project require another programming language,
a list of known ports and bindings is provided on [Zstandard homepage](http://www.zstd.net/#other-languages).

The negative compression levels, specified with `--fast=#`,
offer faster compression and decompression speed in exchange for some loss in
compression ratio compared to level 1, as seen in the table above.

Zstd can also offer stronger compression ratios at the cost of compression speed.
Speed vs Compression trade-off is configurable by small increments.
Decompression speed is preserved and remains roughly the same at all settings,
a property shared by most LZ compression algorithms, such as [zlib] or lzma.


### The case for Small Data compression

The smaller the amount of data to compress, the more difficult it is to compress. This problem is common to all compression algorithms, and reason is, compression algorithms learn from past data how to compress future data. But at the beginning of a new data set, there is no "past" to build upon.

To solve this situation, Zstd offers a __training mode__, which can be used to tune the algorithm for a selected type of data.
Training Zstandard is achieved by providing it with a few samples (one file per sample). The result of this training is stored in a file called "dictionary", which must be loaded before compression and decompression.
Using this dictionary, the compression ratio achievable on small data improves dramatically.

Training works if there is some correlation in a family of small data samples. The more data-specific a dictionary is, the more efficient it is (there is no _universal dictionary_).
Hence, deploying one dictionary per type of data will provide the greatest benefits.
Dictionary gains are mostly effective in the first few KB. Then, the compression algorithm will gradually use previously decoded content to better compress the rest of the file.


### License

Zstandard is dual-licensed under [BSD](../../algos/zstd/LICENSE) and [GPLv2](https://github.com/facebook/zstd/blob/dev/COPYING).

BZIP2 - Introduction
--------------------
Bzip2 compresses files using the Burrows-Wheeler block-sorting text compression algorithm, and Huffman coding. Compression is generally considerably better than that achieved by more conventional LZ77/LZ78-based compressors, and approaches the performance of the PPM family of statistical compressors.

This file is part of bzip2/libbzip2, a program and library for
lossless, block-sorting data compression.

bzip2/libbzip2 version 1.0.8 of 13 July 2019
Copyright (C) 1996-2019 Julian Seward <jseward@acm.org>

Please read the WARNING, DISCLAIMER and PATENTS sections in the file
available at [https://github.com/joachimmetz/bzip2/blob/main/README](https://github.com/joachimmetz/bzip2/blob/main/README) .

This program is released under the terms of the license contained in the file LICENSE.
