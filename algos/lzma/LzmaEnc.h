/*  LzmaEnc.h -- LZMA Encoder
2019-10-30 : Igor Pavlov : Public domain */

/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
 */

#ifndef __LZMA_ENC_H
#define __LZMA_ENC_H

#include "7zTypes.h"

EXTERN_C_BEGIN

/*!
 * \addtogroup LZMA_API
 * @{
*/

/// @cond DOXYGEN_SHOULD_SKIP_THIS
#define LZMA_PROPS_SIZE 5
/// @endcond /* DOXYGEN_SHOULD_SKIP_THIS */

typedef struct _CLzmaEncProps
{
  int level;        /**< Control degree of compression. Lower level gives less compression at higher speed. \n 0 <= level <= 9 */
  UInt32 dictSize;  /**< size of dictionary / search buffer \n (1 << 12) <= dictSize <= (1 << 27) for 32-bit version \n
                      (1 << 12) <= dictSize <= (3 << 29) for 64-bit version \n 
                      \b default = (1 << 24) */
  int lc;           /**< Number of high bits of the previous byte to use as a context for literal encoding. \n 
                        0 <= lc <= 8, \n \b default = 3 */
  int lp;           /**< Number of low bits of the dictionary position to include in literal posState. \n 
                         0 <= lp <= 4, \n \b default = 0 */
  int pb;           /**< Number of low bits of processedPos to include in posState. \n 
                         0 <= pb <= 4, \n \b default = 2 */
  int algo;         /**< Dictionary search algo to use: 0 - fast (hash chain), 1 - normal (binary search tree). \n 
                         0 - fast, 1 - normal, \n \b default = 1 */
  int fb;           /**< Number of fast bytes. \n 
                         5 <= fb <= 273, \n \b default = 32 */
  int btMode;       /**< 0 - hashChain Mode, 1 - binTree mode - normal, \n \b default = 1 */
  int numHashBytes; /**< Number of bytes used to compute hash. \n 
                         2, 3 or 4, \n \b default = 4 */
  UInt32 mc;        /**< Cut value, limit on number of nodes to search in dictionary. \n 
                         1 <= mc <= (1 << 30), \n \b default = 32 */
  unsigned writeEndMark;  /**< 0 - do not write EOPM, 1 - write EOPM, \n \b default = 0 */
  int numThreads;   /**< Threads used for processing. \n 
                         1 or 2, \n \b default = 1 */

  UInt64 reduceSize; /**< estimated size of data that will be compressed. default = (UInt64)(Int64)-1. \n 
                        Encoder uses this value to reduce dictionary size */

  UInt64 affinity;

#ifdef AOCL_LZMA_OPT
  size_t srcLen;
#endif
} CLzmaEncProps;

/*!
 * @name Encode Functions
 * @{
 */

/*! @brief Init properties. Properties are set to auto select mode.
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b p       | out         | Lzma encode properties object |
*
* @return void
* 
*/
LZMALIB_API void LzmaEncProps_Init(CLzmaEncProps *p);

/*! @brief Set default values for properties in auto select mode.
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b p       | out         | Lzma encode properties object |
*
* @return void
* 
*/
LZMALIB_API void LzmaEncProps_Normalize(CLzmaEncProps *p);

/*! @brief Normalize props2 and return dictionary size.
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b props2  | out         | Lzma encode properties object |
*
* @return dictionary size
*/
LZMALIB_API UInt32 LzmaEncProps_GetDictSize(const CLzmaEncProps *props2);

/**
 * @}
*/

/* ---------- CLzmaEncHandle Interface ---------- */

/* LzmaEnc* functions can return the following exit codes:
SRes:
  SZ_OK           - OK
  SZ_ERROR_MEM    - Memory allocation error
  SZ_ERROR_PARAM  - Incorrect parameter in props
  SZ_ERROR_WRITE  - ISeqOutStream write callback error
  SZ_ERROR_OUTPUT_EOF - output buffer overflow - version with (Byte *) output
  SZ_ERROR_PROGRESS - some break from progress callback
  SZ_ERROR_THREAD - error in multithreading functions (only for Mt version)
*/

typedef void * CLzmaEncHandle;

/*!
 * @name Encode Functions
 * @{
 */

/*! @brief Construct Lzma encoder
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b alloc   | in          | Allocator object |
*
* @return Lzma encoder handle
* 
*/
LZMALIB_API CLzmaEncHandle LzmaEnc_Create(ISzAllocPtr alloc);

/*! @brief Free Lzma encoder
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b p          | out         | Lzma encoder handle |
* | \b alloc      | in          | Allocator object |
* | \b allocBig   | in          | Allocator object for large blocks |
*
* @return void
* 
*/
LZMALIB_API void LzmaEnc_Destroy(CLzmaEncHandle p, ISzAllocPtr alloc, ISzAllocPtr allocBig);

/*! @brief Update properties in p with values in props
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b p       | out         | Lzma encoder handle |
* | \b props   | in          | Lzma encoder properties |
*
* @return 
* | Result     | Description |
* |:-----------|:------------|
* | Success    |SZ_OK                      |
* | Fail       |SZ_ERROR_PARAM      - Incorrect parameter in props  |
* 
*/
LZMALIB_API SRes LzmaEnc_SetProps(CLzmaEncHandle p, const CLzmaEncProps *props);

/*! @brief Set expected data size in p to expectedDataSiize
*
* | Parameters           | Direction   | Description |
* |:---------------------|:-----------:|:------------|
* | \b p                 | out         | Lzma encoder handle |
* | \b expectedDataSiize | in          | Expected size of data |
*
* @return void
* 
*/
LZMALIB_API void LzmaEnc_SetDataSize(CLzmaEncHandle p, UInt64 expectedDataSiize);

/*! @brief Build header bytes and save in properties
*
* | Parameters    | Direction   | Description |
* |:--------------|:-----------:|:------------|
* | \b p          | in          | Lzma encoder handle |
* | \b properties | out         | Buffer to write header bytes into |
* | \b size       | in,out      | Number of bytes saved in properties |
*
* @return 
* | Result     | Description |
* |:-----------|:------------|
* | Success    |SZ_OK        |
* | Fail       |SZ_ERROR_PARAM      - Incorrect parameter in props |
* 
*/
LZMALIB_API SRes LzmaEnc_WriteProperties(CLzmaEncHandle p, Byte *properties, SizeT *size);

/*! @brief Get write end mark saved within p
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b p       | in          | Lzma encoder handle |
*
* @return writeEndMark
* 
*/
LZMALIB_API unsigned LzmaEnc_IsWriteEndMark(CLzmaEncHandle p);

/// @cond DOXYGEN_SHOULD_SKIP_THIS
LZMALIB_API SRes LzmaEnc_Encode(CLzmaEncHandle p, ISeqOutStream *outStream, ISeqInStream *inStream,
    ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig);
/// @endcond /* DOXYGEN_SHOULD_SKIP_THIS */


/*! @brief Encode src in-memory and save compressed data to dest
*
* | Parameters      | Direction   | Description |
* |:----------------|:-----------:|:------------|
* | \b p            | in,out      | Lzma encoder handle |
* | \b dest         | out         | Destination buffer to hold compressed data |
* | \b destLen      | out         | Size of compressed data written to dest |
* | \b src          | in          | Source buffer with uncompressed data |
* | \b srcLen       | in          | Size of uncompressed data in src |
* | \b writeEndMark | in          | If non-0, finish stream with end mark |
* | \b progress     | in          | Compression progress indicator |
* | \b alloc        | in          | Allocator object |
* | \b allocBig     | in          | Allocator object for large blocks |
*
* @return 
* | Result     | Description |
* |:-----------|:------------|
* | Success    |SZ_OK                      |
* | Fail       |SZ_ERROR_MEM        - Memory allocation error |
* | ^          |SZ_ERROR_PARAM      - Incorrect parameter in props  |
* | ^          |SZ_ERROR_WRITE      - ISeqOutStream write callback error  |
* | ^          |SZ_ERROR_OUTPUT_EOF - output buffer overflow - version with (Byte *) output  |
* | ^          |SZ_ERROR_PROGRESS   - some break from progress callback  |
* 
*/
LZMALIB_API SRes LzmaEnc_MemEncode(CLzmaEncHandle p, Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
    int writeEndMark, ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig);

/**
 * @}
 */

/* ---------- One Call Interface ---------- */
/*!
 * @name Encode One Call Interface
 * @{
 */

/*! @brief Encode data in src and save compressed data to dest
*
* | Parameters      | Direction   | Description |
* |:----------------|:-----------:|:------------|
* | \b dest         | out         | Destination buffer to hold compressed data |
* | \b destLen      | out         | Size of compressed data written to dest |
* | \b src          | in          | Source buffer with uncompressed data |
* | \b srcLen       | in          | Size of uncompressed data in src |
* | \b props        | in          | Properties to control compression method |
* | \b propsEncoded | out         | Buffer to save header bytes |
* | \b propsSize    | out         | Size of header bytes |
* | \b writeEndMark | in          | If non-0, finish stream with end mark |
* | \b progress     | in          | Compression progress indicator |
* | \b alloc        | in          | Allocator object |
* | \b allocBig     | in          | Allocator object for large blocks |
*
* @return
* | Result     | Description |
* |:-----------|:------------|
* | Success    |SZ_OK                      |
* | Fail       |SZ_ERROR_MEM        - Memory allocation error |
* | ^          |SZ_ERROR_PARAM      - Incorrect parameter in props  |
* | ^          |SZ_ERROR_WRITE      - ISeqOutStream write callback error  |
* | ^          |SZ_ERROR_OUTPUT_EOF - output buffer overflow - version with (Byte *) output  |
* | ^          |SZ_ERROR_PROGRESS   - some break from progress callback  |
*
*/
LZMALIB_API SRes LzmaEncode(Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
    const CLzmaEncProps *props, Byte *propsEncoded, SizeT *propsSize, int writeEndMark,
    ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig);

/**
 * @}
*/

#ifdef AOCL_DYNAMIC_DISPATCHER
/*! @brief AOCL-Compression defined setup function that configures with the right
 * AMD optimized lzma routines depending upon the detected CPU features.
 *
* | Parameters    | Description |
 * |:-------------|:------------|
 * | \b optOff    | Turn off all optimizations .                                                                  |
 * | \b optLevel  | Optimization level: 0 - C optimization, 1 - SSE2, 2 - AVX, 3 - AVX2, 4 - AVX512 .             |
 * | \b insize    | Input data length.                                                                            |
 * | \b level     | Requested compression level.                                                                  |
 * | \b windowLog | Largest match distance : larger == more compression, more memory needed during decompression. |
 *
 * @return \b NULL 
 */
LZMALIB_API void aocl_setup_lzma_encode(int optOff, int optLevel, size_t insize,
  size_t level, size_t windowLog);
#endif

/*!
 * @name Encode Functions
 * @{
 */
#ifdef AOCL_LZMA_OPT
/*! @brief AOCL optimized LzmaEncProps_Normalize function. \n 
*          Set default values for properties in auto select mode.
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b p       | out         | Lzma encode properties object |
*
* @return void
* 
*/
LZMALIB_API void AOCL_LzmaEncProps_Normalize(CLzmaEncProps* p);

/*! @brief AOCL optimized LzmaEnc_SetProps function. \n
*          Update properties in pp with values in props2
*
* | Parameters | Direction   | Description |
* |:-----------|:-----------:|:------------|
* | \b pp       | out         | Lzma encoder handle |
* | \b props2   | in          | Lzma encoder properties |
*
* @return 
* | Result     | Description |
* |:-----------|:------------|
* | Success    |SZ_OK                      |
* | Fail       |SZ_ERROR_PARAM      - Incorrect parameter in props2  |
* 
*/
LZMALIB_API SRes AOCL_LzmaEnc_SetProps(CLzmaEncHandle pp, const CLzmaEncProps* props2);
#endif

/**
 * @}
*/

#ifdef AOCL_LZMA_UNIT_TEST
LZMALIB_API void Test_LzmaEncProps_Normalize_Dyn(CLzmaEncProps* p);
LZMALIB_API SRes Test_SetProps_Dyn(CLzmaEncHandle pp, const CLzmaEncProps* props);
LZMALIB_API SRes Test_Validate_NumFastBytes(CLzmaEncHandle pp, unsigned numFastBytes);
LZMALIB_API SRes Test_Validate_NumHashBytes(CLzmaEncHandle pp, unsigned numHashBytes);
LZMALIB_API UInt64 Test_SetDataSize(CLzmaEncHandle pp, UInt64 expectedDataSize);
LZMALIB_API SRes Test_WriteProperties(CLzmaEncHandle pp, Byte* props, SizeT* size, UInt32 dictSize);
LZMALIB_API unsigned Test_IsWriteEndMark(CLzmaEncHandle pp, unsigned wem);
#endif

/**
 * @}
 */

EXTERN_C_END

#endif
