LZ4 - Introduction
==================

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



Documentation
-------------------------

The raw LZ4 block compression format is detailed within [lz4_Block_format].

Arbitrarily, for streaming requirements, long files or data streams are compressed using multiple blocks. These blocks are organized into a frame,
defined into [lz4_Frame_format].
Interoperable versions of LZ4 must also respect the frame format.

[lz4_Block_format]: https://github.com/lz4/lz4/blob/dev/doc/lz4_Block_format.md
[lz4_Frame_format]: https://github.com/lz4/lz4/blob/dev/doc/lz4_Frame_format.md


Other source versions
-------------------------

Beyond the C reference source,
many contributors have created versions of LZ4 in multiple languages
(Java, C#, Python, Perl, Ruby, and so on).
A list of known source ports is maintained on the [LZ4 Homepage].

[LZ4 Homepage]: http://www.lz4.org

# LZ4 - Library Files

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


### Merging

LZ4 source code can be merged into a single file.
One can combine all source code into `lz4_all.c` by using following command:
```
cat lz4.c lz4hc.c lz4frame.c > lz4_all.c
```
(__Note:__ The `cat` file order is important). Compile `lz4_all.c`.
All the `*.h` files present in `/lib` are mandatory to compile `lz4_all.c`.


### License

All the source material in __lib__ directory is BSD 2-Clause licensed.
For more information, refer to [LICENSE](LICENSE) for details.
This license is also reminded at the top of each source file.
