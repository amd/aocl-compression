// Copyright 2005 and onwards Google Inc.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// A light-weight compression algorithm.  It is designed for speed of
// compression and decompression, rather than for the utmost in space
// savings.
//
// For getting better compression ratios when you are compressing data
// with long repeated sequences or compressing data that is similar to
// other data, while still compressing fast, you might look at first
// using BMDiff and then compressing the output of BMDiff with
// Snappy.

#ifndef THIRD_PARTY_SNAPPY_SNAPPY_H__
#define THIRD_PARTY_SNAPPY_SNAPPY_H__

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "snappy-stubs-public.h"

#ifndef SNAPPYLIB_VISIBILITY
#  if defined(__GNUC__) && (__GNUC__ >= 4)
#    define SNAPPYLIB_VISIBILITY __attribute__ ((visibility ("default")))
#  else
#    define SNAPPYLIB_VISIBILITY
#  endif
#endif
#if defined(SNAPPY_DLL_EXPORT) && (SNAPPY_DLL_EXPORT==1)
#  define SNAPPYLIB_API __declspec(dllexport) SNAPPYLIB_VISIBILITY
#elif defined(SNAPPY_DLL_IMPORT) && (SNAPPY_DLL_IMPORT==1)
#  define SNAPPYLIB_API __declspec(dllimport) SNAPPYLIB_VISIBILITY /* It isn't required but allows to generate better code, saving a function pointer load from the IAT and an indirect jump.*/
#else
#  define SNAPPYLIB_API SNAPPYLIB_VISIBILITY
#endif

/**
 * 
 * \addtogroup SNAPPY_API
 * @brief
 * A light-weight compression algorithm.  It is designed for speed of
 * compression and decompression, rather than for the utmost in space
 * savings.
 *
 * For getting better compression ratios when you are compressing data
 * with long repeated sequences or compressing data that is similar to
 * other data, while still compressing fast, you might look at first
 * using BMDiff and then compressing the output of BMDiff with
 * Snappy.
 * @{
 */

namespace snappy {
  class Source;
  class Sink;

  // ------------------------------------------------------------------------
  // Generic compression/decompression routines.
  // ------------------------------------------------------------------------

/**
 * @brief 
 * Compress the bytes read from "*source" and append to "*sink". Return the
 * number of bytes written.
 * 
 * @param source A Source is an interface that yields a sequence of bytes, you can initialize it by calling
 * snappy::ByteArraySource(inBuf,inBufLen);
 * where inBuf is the pointer to original data and inBufLen is the size of inBuf
 * @param sink A Sink is an interface that consumes a sequence of bytes, you can initialize it by calling 
 * snappy::UncheckedByteArraySink(dest); where dest is the pointer to the destination buffer.
 * 
 * @return Return the number of bytes written.
 */

 SNAPPYLIB_API size_t Compress(Source* source, Sink* sink);

/**
 * @brief
 * Find the uncompressed length of the given stream, as given by the header.
 * Note that the true length could deviate from this; the stream could e.g.
 * be truncated.
 *
 * @param source A Source is an interface that yields a sequence of bytes, you can initialize it by calling
 * snappy::ByteArraySource(inBuf,inBufLen).
 * where inBuf is the pointer to original data and inBufLen is the size of inBuf.
 * @param result Uncompressed length of the given stream is stored here.
 * 
 * @note Also note that this leaves "*source" in a state that is unsuitable for
 * further operations, such as RawUncompress(). You will need to rewind
 * or recreate the source yourself before attempting any further calls. 
 * 
 * @return It will return \b false if the data inside the source is corrupted.
 * @return If the data inside the source is uncorrupted it will return \b true.
 */

 SNAPPYLIB_API bool GetUncompressedLength(Source* source, uint32_t* result);

  // ------------------------------------------------------------------------
  // Higher-level string based routines (should be sufficient for most users)
  // ------------------------------------------------------------------------

/**
 * @brief 
 * Sets "*compressed" to the compressed version of "input[0,input_length-1]".
 * Original contents of *compressed are lost.
 *
 * @param input This is the buffer where the data we want to compress is accessible.
 * @param input_length Length of the input buffer.
 * @param compressed This is a buffer in which compressed data is stored.
 * 
 * @attention REQUIRES: "input[]" is not an alias of "*compressed".
 * 
 * @return Return the number of bytes written.
 */

 SNAPPYLIB_API size_t Compress(const char* input, size_t input_length,
                  std::string* compressed);
/**
 * @brief
 *  Decompresses "compressed[0,compressed_length-1]" to "*uncompressed".
 *  Original contents of "*uncompressed" are lost.
 * 
 * @param compressed This is a buffer which contains compressed data.
 * @param compressed_length This is the length of the compressed buffer.
 * @param uncompressed Uncompressed data is stored in this buffer.
 * 
 * @attention REQUIRES: "compressed[]" is not an alias of "*uncompressed".
 * 
 * @return Returns \b false if the message is corrupted and could not be decompressed.
 * 
 */

 SNAPPYLIB_API bool Uncompress(const char* compressed, size_t compressed_length,
                  std::string* uncompressed);
  /**
   * @brief Decompresses "compressed" to "*uncompressed".
   * 
   * @param compressed A Source is an interface that yields a sequence of bytes, you can initialize it by calling
   * snappy::ByteArraySource(inBuf,inBufLen);
   * where inBuf is the pointer to original data and inBufLen is the size of inBuf .
   * @param uncompressed A Sink is an interface that consumes a sequence of bytes, you can initialize it by calling 
   * snappy::UncheckedByteArraySink(dest); where dest is the pointer to the destination buffer.
   * 
   * @return \b true if successfull.
   * @return \b false if the message is corrupted and could not be decompressed.
   */

 SNAPPYLIB_API bool Uncompress(Source* compressed, Sink* uncompressed);

  /**
   * @brief 
   * This routine uncompresses as much of the "compressed" as possible
   * into sink.  It returns the number of valid bytes added to sink
   * (extra invalid bytes may have been added due to errors; the caller
   * should ignore those). The emitted data typically has length
   * GetUncompressedLength(), but may be shorter if an error is
   * encountered.
   * 
   * @param compressed A Source is an interface that yields a sequence of bytes, you can initialize it by calling
   * snappy::ByteArraySource(inBuf,inBufLen);
   * where inBuf is the pointer to original data and inBufLen is the size of inBuf .
   * @param uncompressed A Sink is an interface that consumes a sequence of bytes, you can initialize it by calling 
   * snappy::UncheckedByteArraySink(dest); where dest is the pointer to the destination buffer.
   * 
   * @return It returns the number of valid bytes added to sink
   * (extra invalid bytes may have been added due to errors; the caller
   * should ignore those).
   */

 SNAPPYLIB_API size_t UncompressAsMuchAsPossible(Source* compressed, Sink* uncompressed);

  // ------------------------------------------------------------------------
  // Lower-level character array based routines.  May be useful for
  // efficiency reasons in certain circumstances.
  // ------------------------------------------------------------------------

  /**
   * @brief 
   *
   * Takes the data stored in "input[0..input_length]" and stores
   * it in the array pointed to by "compressed".
   *
   * "*compressed_length" is set to the length of the compressed output.
   * 
   * @param input This is the buffer where the data we want to compress is accessible.
   * @param input_length Length of the input buffer.
   * @param compressed This is a buffer in which compressed data is stored.
   * @param compressed_length The length of the data after compression is stored in this.
   * 
   * @attention REQUIRES: "compressed" must point to an area of memory that is at
   * least "MaxCompressedLength(input_length)" bytes in length.
   * 
   * @note - Example:\n\n
   *  \code{.c}
   *            char  output = new char[snappy::MaxCompressedLength(input_length)];\n
   *            size_t output_length;\n
   *            RawCompress(input, input_length, output, &output_length);\n
   *            ... Process(output, output_length) ...\n
   *            delete [] output;\n
   * \endcode
   * @return \b  void
   */

 SNAPPYLIB_API void RawCompress(const char* input,
                   size_t input_length,
                   char* compressed,
                   size_t* compressed_length);

  /**
   * @brief 
   * Given data in "compressed[0..compressed_length-1]" generated by
   * calling the Snappy::Compress routine, this routine
   * stores the uncompressed data to
   *    uncompressed[0..GetUncompressedLength(compressed)-1] .
   * 
   * @param compressed This is a buffer which contains compressed data.
   * @param compressed_length This is the length of the compressed buffer.
   * @param uncompressed Uncompressed data is stored in this buffer.
   * 
   * @return \b true if successfull.
   * @return \b false if the message is corrupted and could not be decrypted.
   */

 SNAPPYLIB_API bool RawUncompress(const char* compressed, size_t compressed_length,
                     char* uncompressed);

  /**
   * @brief 
   * Given data from the byte source 'compressed' generated by calling
   * the Snappy::Compress routine, this routine stores the uncompressed
   * data to
   *    uncompressed[0..GetUncompressedLength(compressed,compressed_length)-1] .
   * 
   * @param compressed A Source is an interface that yields a sequence of bytes, you can initialize it by calling
   * snappy::ByteArraySource(inBuf,inBufLen);
   * where inBuf is the pointer to original data and inBufLen is the size of inBuf .
   * @param uncompressed Uncompressed data is stored in this buffer.
   * 
   * @return \b true if successfull.
   * @return \b false if the message is corrupted and could not be decrypted.
   */

 SNAPPYLIB_API bool RawUncompress(Source* compressed, char* uncompressed);

  /**
   * @brief 
   * Given data in "compressed[0..compressed_length-1]" generated by
   * calling the Snappy::Compress routine, this routine
   * stores the uncompressed data to the iovec "iov". The number of physical
   * buffers in "iov" is given by iov_cnt and their cumulative size
   * must be at least GetUncompressedLength(compressed). The individual buffers
   * in "iov" must not overlap with each other.
   * 
   * @param compressed This is a buffer which contains compressed data.
   * @param compressed_length This is the length of the compressed buffer.
   * @param iov The struct iovec defines one vector element. 
   * Normally, this structure is used as an array of multiple elements. 
   * @param iov_cnt This is the number of \a iovec structures in the array of \a iov .
   * 
   * @return \b true if successfull.
   * @return \b false if the message is corrupted and could not be decrypted.
   */

 SNAPPYLIB_API bool RawUncompressToIOVec(const char* compressed, size_t compressed_length,
                            const struct iovec* iov, size_t iov_cnt);

  /**
   * @brief 
   * Given data from the byte source 'compressed' generated by calling
   * the Snappy::Compress routine, this routine stores the uncompressed
   * data to the iovec "iov". The number of physical
   * buffers in "iov" is given by iov_cnt and their cumulative size
   * must be at least GetUncompressedLength(compressed). The individual buffers
   * in "iov" must not overlap with each other.
   * 
   * @param compressed A Source is an interface that yields a sequence of bytes, you can initialize it by calling snappy::ByteArraySource(inBuf,inBufLen);
   * where inBuf is the pointer to original data and inBufLen is the size of inBuf .
   * @param iov The struct iovec defines one vector element. 
   * Normally, this structure is used as an array of multiple elements. 
   * @param iov_cnt This is the number of \a iovec structures in the array of \a iov .
   * 
   * @return \b true if successfull.
   * @return \b false if the message is corrupted and could not be decrypted.
   */

 SNAPPYLIB_API bool RawUncompressToIOVec(Source* compressed, const struct iovec* iov,
                            size_t iov_cnt);

  /**
   * @brief This function determines the maximal size of the compressed representation of
   * input data that is "source_bytes" bytes in length.
   * 
   * @param source_bytes The size of source in bytes.
   * 
   * @return Returns the maximal size of the compressed representation of
   * input data that is "source_bytes" bytes in length.
   */

 SNAPPYLIB_API size_t MaxCompressedLength(size_t source_bytes);

  /**
   * @brief Get the Uncompressed Length object.
   * 
   * This operation takes O(1) time.
   * 
   * @attention REQUIRES: "compressed[]" was produced by RawCompress() or Compress().
   * 
   * @param compressed This is a buffer which contains compressed data.
   * @param compressed_length This is the length of the compressed buffer.
   * @param result This is the pointer to type size_t where the uncompressed length is stored.
   * 
   * @return \b true on successfull parsing.
   * @return \b false on parsing error.
   */

 SNAPPYLIB_API bool GetUncompressedLength(const char* compressed, size_t compressed_length,
                             size_t* result);

  /**
   * @brief 
   * Returns \b true iff the contents of "compressed[]" can be uncompressed
   * successfully.  Does not return the uncompressed data.  Takes
   * time proportional to compressed_length, but is usually at least
   * a factor of four faster than actual decompression.
   * 
   * @param compressed This is a buffer which contains compressed data.
   * @param compressed_length This is the length of the compressed buffer.
   * 
   * @return \b true iff the contents of "compressed[]" can be uncompressed successfully.
   * @return \b false if error.
   */

 SNAPPYLIB_API bool IsValidCompressedBuffer(const char* compressed,
                               size_t compressed_length);

  /**
   * @brief 
   * Returns \b true iff the contents of "compressed" can be uncompressed
   * successfully.  Does not return the uncompressed data.  Takes
   * time proportional to *compressed length, but is usually at least
   * a factor of four faster than actual decompression.
   * On success, consumes all of *compressed.  On failure, consumes an
   * unspecified prefix of *compressed.
   * 
   * @param compressed A Source is an interface that yields a sequence of bytes, you can initialize it by calling
   * snappy::ByteArraySource(inBuf,inBufLen);
   * where inBuf is the pointer to original data and inBufLen is the size of inBuf .
   * 
   * @return \b true iff the contents of "compressed" can be uncompressed successfully.
   * @return \b false if error.
   */
 SNAPPYLIB_API bool IsValidCompressed(Source* compressed);

  /* AOCL-Compression defined setup function that configures with the right
*  AMD optimized snappy routines depending upon the detected CPU features. */
#ifdef AOCL_DYNAMIC_DISPATCHER

/**
 * @brief AOCL-Compression defined setup function that configures with the right
 * AMD optimized snappy routines depending upon the detected CPU features.
 * 
 * @param optOff Turn off all optimizations
 * @param optLevel Optimization level: 0 - C optimization, 1 - SSE2, 2 - AVX, 3 - AVX2, 4 - AVX512
 * @param insize Input data length.
 * @param level Requested compression level.
 * @param windowLog Largest match distance : larger == more compression, more memory needed during decompression.
 * 
 * @return NULL .
 */

 SNAPPYLIB_API char * aocl_setup_snappy(int optOff, int optLevel, size_t insize,
                           size_t level, size_t windowLog);
#endif

/**
 * @brief This class is created to expose internal functions which are not available external to this method.
 * 
 * The test cases written for API level testing needed these internal functions, but can't access them directly
 * so a seperate class was needed for calling those internal functions.
 */
 class SNAPPYLIB_API SNAPPY_Gtest_Util
 {
  public:
    static Source * ByteArraySource_ext(const char *p, size_t n);
    static Sink * UncheckedByteArraySink_ext(char *dest);
    static void Append32(std::string* s, uint32_t value);
 };

  // The size of a compression block. Note that many parts of the compression
  // code assumes that kBlockSize <= 65536; in particular, the hash table
  // can only store 16-bit offsets, and EmitCopy() also assumes the offset
  // is 65535 bytes or less. Note also that if you change this, it will
  // affect the framing format (see framing_format.txt).
  //
  // Note that there might be older data around that is compressed with larger
  // block sizes, so the decompression code should not rely on the
  // non-existence of long backreferences.
  static constexpr int kBlockLog = 16;
  static constexpr size_t kBlockSize = 1 << kBlockLog;

  static constexpr int kMinHashTableBits = 8;
  static constexpr size_t kMinHashTableSize = 1 << kMinHashTableBits;

  static constexpr int kMaxHashTableBits = 14;
  static constexpr size_t kMaxHashTableSize = 1 << kMaxHashTableBits;
}  // end namespace snappy

#endif  // THIRD_PARTY_SNAPPY_SNAPPY_H__
/*
/**
 * @}
 *
 * /
 */
