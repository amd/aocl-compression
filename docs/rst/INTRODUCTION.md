Methods supported
-----------------

### LZ4 - Introduction


LZ4 is a lossless compression algorithm
providing a compression speed > 500 MB/s per core,
scalable with multi-cores CPU.
It features an extremely fast decoder
with speed in multiple GB/s per core,
reaching RAM speed limits on multi-core systems.


### ZLIB - Introduction


zlib 1.3.1 is a general-purpose data compression library.  All the code is
thread safe.  The data format used by the zlib library is described by
Request for Comments (RFCs) 1950 to 1952 in the files
[http://tools.ietf.org/html/rfc1950](http://tools.ietf.org/html/rfc1950) (zlib format), rfc1951 (deflate format), and
rfc1952 (gzip format).



### LZMA - Introduction

LZMA / LZMA2 are default and general compression methods of 7z format in the 7-Zip program.
LZMA provides a high compression ratio and fast decompression, so it is very suitable for 
embedded applications. For example, it can be used for ROM (firmware) compressing.


### ZSTD - Introduction

__Zstandard__, or `zstd` as short version, is a fast lossless compression algorithm,
targeting real-time compression scenarios at zlib-level and better compression ratios.
It's backed by a very fast entropy stage, provided by [Huff0 and FSE library](https://github.com/Cyan4973/FiniteStateEntropy).


### BZIP2 - Introduction

Bzip2 compresses files using the Burrows-Wheeler block-sorting text compression algorithm, and Huffman coding. Compression is generally considerably better than that achieved by more conventional LZ77/LZ78-based compressors, and approaches the performance of the PPM family of statistical compressors.


### Snappy - Introduction

Snappy is a compression/decompression library. It does not aim for maximum
compression or compatibility with any other compression library; instead,
it aims for very high speeds and reasonable compression.




### LZ4HC - Introduction

LZ4HC is the High Compression variant of LZ4.
It offers more compression ratio at the cost of compression speed.
The files lz4hc.c and lz4hc.h add the support for LZ4HC.
