/**
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
 
 /** @file aocl_threads.h
 *  
 *  @brief Interface API related to Multi-threading support
 *  in AOCL-Compression library
 *
 *  This file contains APIs that can be used to interpret streams
 *  produced by AOCL-Compression library with multi-threaded support.
 *
 *  @author S. Biplab Raut
 */
 
#ifndef AOCL_MT_H
#define AOCL_MT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {

#endif
/**
 * \defgroup MT_API Multi-threaded API
 */

/**
 * \addtogroup MT_API
 *  @brief Interface API for multi-threaded support in AOCL Compression library are described in this section.
 *
 *  This file contains APIs that can be used to interpret streams
 *  produced by AOCL-Compression library with multi-threaded support.
 * 
 * RAP (Random Access Point) Frame :
 * AOCL multi-threaded compressor might add an additional RAP frame at the start of the compressed stream
 * for methods with no support for skippable blocks/frames. This contains metadata which the AOCL multi-threaded 
 * decompressor will use to decompress the stream on multiple threads.
 * 
 * Note : AOCL multi-threaded compressed output can be decompressed by a legacy single-threaded decompressor
 *        by incorporating an additional step of skipping the RAP frame bytes and passing the source stream
 *        buffer pointer that succeeds the RAP frame to the decompressor.
 * @{
 */

/// @cond DOXYGEN_SHOULD_SKIP_THIS
#ifndef EXPORT_SYM_DYN
#ifdef _WINDOWS
/**
 * You can export data, functions, classes, or class member functions from a DLL
 * using the __declspec(dllexport) keyword. __declspec(dllexport) adds the export
 * directive to the object file so you do not need to use a .def file.
 * 
 */
#define EXPORT_SYM_DYN __declspec(dllexport)
#else
/**
 * For Linux EXPORT_SYM_DYN is NULL, by default the symbols are publicly exposed.
 */
#define EXPORT_SYM_DYN
#endif
#endif

/// @endcond /* DOXYGEN_SHOULD_SKIP_THIS */

/****************************************************************************************************************************************************************/
/* AOCL compression multi-threaded interface */
/****************************************************************************************************************************************************************/

/**
 * @brief Interface API to get the upper bound of RAP frame bytes that will be added 
 * during multithreaded compression.
 *
 * Applications may call this API to know how many additional bytes are needed in the destination
 * buffer, and allocate them in addition to what is needed for single threaded compression.
 *
 * @return
 * | Result     | Description |
 * |:-----------|:------------|
 * | RAP_frame_bound | Upper bound of RAP frame bytes |
 *
 */
EXPORT_SYM_DYN int32_t aocl_get_rap_frame_bound_mt(void);

/**
 * @brief Interface API to get the length of the RAP frame in the compressed stream.
 *
 * Legacy single threaded decompressors can call this API to know how many bytes of the compressed
 * stream to skip to get the format compliant compressed stream that they can decompress.
 * 
 * Note : Presence of RAP frame is determined by checking for the magic word: 0x434C4C5F4C434F41 
 *        (ASCII encoding of AOCL_LLC) at the start of the stream.
 *
 * | Parameters      | Direction   | Description |
 * |:----------------|:-----------:|:------------|
 * | \b src          | in          | Input stream buffer pointer. |
 * | \b src_size     | in          | Input stream buffer size. |
 * 
 * @return
 * | Result     | Description |
 * |:-----------|:------------|
 * | RAP_frame_length | Length of RAP frame bytes in src |
 * | 0                | If RAP frame does not exist      |
 * | Fail             | `ERR_INVALID_INPUT`              |
 *
 */
EXPORT_SYM_DYN int32_t aocl_skip_rap_frame_mt(char* src, int32_t src_size);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif
