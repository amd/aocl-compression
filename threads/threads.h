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

 /** @file threads.h
 *
 *  @brief Data structures and function declarations for supporting SMP threads
 *
 *  This file contains the data structures and function declarations that 
 *  support SMP multi-threading of the compression and decompression methods.
 *
 *  @author S. Biplab Raut
 */

#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>
#include <omp.h>
#include "api/types.h"

/****************************************************************************************************************************************************************
 * RAP (Random Access Point) Frame and Metadata Format specification:                                                                                           *
 * -------------------------------------------                                                                                                                  *
 *                                                                                                                                                              *
 * | <--------------------------------------------------------------------- RAP Frame -------------------------------------------------------------------> |    *
 * | <----------------------------- RAP Header -----------------------------> | <----------------------------- RAP Metadata -----------------------------> |    *
 * | <-- RAP Magic word (8 bytes) --> | <-- RAP Metadata length (4 bytes) --> |                                                                                 *
 *                                                                              | <--- Num Main Threads (2 bytes) --> | <-- Num Child Threads (2 bytes) ---> |  *
 *                                                                                                                                                              *
 * | <----------------------------------------------------------- RAP Metadata cont'd -------------------------------------------------------------------> |    *
 * | <-- Main Thread-1 (RAP Offset(4 bytes), RAP Length(4 bytes), Opt Decompressed Length(4 bytes)) --> | ................................................      *
 *                                                                                                                                                              *
 * | <----------------------------------------------------------- RAP Metadata cont'd -------------------------------------------------------------------> |    *
 *   ............................................... | <-- Main Thread-N (RAP Offset (4 bytes), RAP Length(4 bytes), Opt Decompressed Length(4 bytes)) --> |    *
 *                                                                                                                                                              *
 * | <------------------------------------------------------------------ RAP Metadata cont'd ------------------------------------------------------------> |    *
 * | <-- Opt Child Thread-1 (RAP Offset (4 bytes), RAP Length(4 bytes)) --> | ... | <-- Opt Child Thread-N (RAP Offset (4 bytes), RAP Length(4 bytes)) --> |    *
 *                                                                                                                                                              *
 * Note 1: AOCL multi-threaded compressor and decompressor will be compliant to this RAP specification.                                                         *
 *         The RAP frame can be placed anywhere in the stream and can be found by looking for the magic word in the stream.                                     *
 *         However, it is recommended to place this RAP frame at the start of the stream to avoid parsing of the stream.                                        *
 * Note 2: AOCL multi-threaded compressed output can be decompressed by a legacy single-threaded decompressor by incorporating                                  *
 *         an additional step of : Skipping the RAP frame bytes and passing the updated source stream buffer pointer to the decompressor.                       *
 * Note 3: To seamlessly decode a AOCL RAP compliant by a legacy single-threaded decompressor without any minor change :                                        *
 *         The decompressors are recommended to skip processing the invalid sequences/blocks/frames and continue with the                                       *
 *         processing of the next sequences/blocks/frames in the stream.                                                                                        *
 ****************************************************************************************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#define RAP_MAGIC_WORD 0x434C4C5F4C434F41 //ASCII encoding of AOCL_LLC
#define RAP_MAGIC_WORD_BYTES 8
#define RAP_METADATA_LEN_BYTES 4
#define RAP_MAIN_THREAD_COUNT_BYTES 2
#define RAP_CHILD_THREAD_COUNT_BYTES 2
#define RAP_START_OF_PARTITIONS (RAP_MAGIC_WORD_BYTES + \
                                 RAP_METADATA_LEN_BYTES + \
                                 RAP_MAIN_THREAD_COUNT_BYTES + \
                                 RAP_CHILD_THREAD_COUNT_BYTES)
#define RAP_OFFSET_BYTES 4
#define RAP_LEN_BYTES 4
#define DECOMP_LEN_BYTES 4
#define RAP_DATA_BYTES (RAP_OFFSET_BYTES + RAP_LEN_BYTES)
#define RAP_DATA_BYTES_WITH_DECOMP_LEN (RAP_OFFSET_BYTES + RAP_LEN_BYTES + DECOMP_LEN_BYTES)
#define RAP_FRAME_LEN(mainThreads, childThreads)    ( RAP_MAGIC_WORD_BYTES + \
            RAP_METADATA_LEN_BYTES + \
            RAP_MAIN_THREAD_COUNT_BYTES + RAP_CHILD_THREAD_COUNT_BYTES + \
            (mainThreads * (RAP_OFFSET_BYTES + RAP_LEN_BYTES)) + \
            (childThreads * mainThreads * (RAP_OFFSET_BYTES + RAP_LEN_BYTES)) )
#define RAP_FRAME_LEN_WITH_DECOMP_LENGTH(mainThreads, childThreads)    ( \
    RAP_MAGIC_WORD_BYTES + \
    RAP_METADATA_LEN_BYTES + \
    RAP_MAIN_THREAD_COUNT_BYTES + RAP_CHILD_THREAD_COUNT_BYTES + \
    (mainThreads * (RAP_OFFSET_BYTES + RAP_LEN_BYTES + DECOMP_LEN_BYTES)) + \
    (childThreads * mainThreads * (RAP_OFFSET_BYTES + RAP_LEN_BYTES)) )
//#define APPROX_PADDED_DST_CHUNK //Keep this disabled as it is more accurate to send the actual dest decompressed len bytes

#define WINDOW_FACTOR 4

//#define AOCL_THREADS_LOG
#ifdef AOCL_THREADS_LOG
#include <stdio.h>
#endif

//Thread Info data structure per thread holding its necessary state information and buffers
typedef struct thread_info
{
    AOCL_CHAR *partition_src;        //Pointer to current src partition
    AOCL_CHAR *dst_trap;             //Pointer to current thread's random access point for dst
    AOCL_VOID *additional_state_info;//This may save info like the pointer to the last src position upto which compression is valid
    AOCL_INTP partition_src_size;    //Length of current src partition
    AOCL_INTP dst_trap_size;         //Length of current thread's random access point for dst
    AOCL_UINT32 last_bytes_len;      //Left over uncompressed bytes that exist beyond last anchor point
    AOCL_UINT32 num_child_threads;   //Child threads : May be used in future for further overlapped parallel processing
    AOCL_UINT32 is_error;            //Has the compression or decompression executed for this chunk: Does not gurantee correctness of results
    AOCL_UINT32 thread_id;           //Thread id of the current thread
    struct thread_info *next;   //In case, more no. of compressed chunks/partitions needs to be decompressed using a smaller no. of threads
} aocl_thread_info_t;

//Thread group data structure held by the master thread that spawns all the threads
// Holds a list of thread info related to all the spawned threads and actual input and output stream buffer pointer
typedef struct thread_group
{
    aocl_thread_info_t *threads_info_list;  //List of thread info related to all the spawned threads 
    AOCL_CHAR *src;                              //Actual input stream buffer pointer
    AOCL_CHAR *dst;                              //Actual output stream buffer pointer
    AOCL_INTP src_size;                          //Actual input stream buffer length
    AOCL_INTP dst_size;                          //Actual output stream buffer length
    AOCL_INTP common_part_src_size;              //Partitioned src length based on num_threads (may not be equal)
    AOCL_INTP leftover_part_src_bytes;           //Leftover src length after partitioning
    AOCL_UINT32 num_threads;                     //Dynamically determined threads to be used for processing
    AOCL_UINT32 search_window_length;            //Search window (dictionary) size used by the partitioning scheme
} aocl_thread_group_t;

#ifndef EXPORT_SYM_THREADS
#ifdef AOCL_UNIT_TEST
#ifdef _WINDOWS
/**
 * You can export data, functions, classes, or class member functions from a DLL
 * using the __declspec(dllexport) keyword. __declspec(dllexport) adds the export
 * directive to the object file so you do not need to use a .def file.
 *
 */
#define EXPORT_SYM_THREADS __declspec(dllexport)
#else
/**
 * For Linux EXPORT_SYM_THREADS is NULL, by default the symbols are publicly exposed.
 */
#define EXPORT_SYM_THREADS
#endif
#else
#define EXPORT_SYM_THREADS
#endif
#endif

/**
 * Function to setup the multi-threaded compressor.
 *
 * Sets thread_grp data structure.
 * Writes the RAP frame header to the dst.
 *
 * Call from a single master thread.
 * The master thread shall allocate and hold thread_grp before calling this function
 *
 * This function allocates thread context and determines how many threads are suitable to compress the input.
 * Each thread processes a part of the src (partition). partition size = win_len * window_factor.
 *
 * | Parameters             | Direction   | Description |
 * |:-----------------------|:-----------:|:------------|
 * | \b thread_grp          | out         | Holds list of thread info, pointers to input and output streams and other information needed for multi-threaded compression. |
 * | \b src                 | in          | Input stream buffer pointer. |
 * | \b dst                 | in          | Output stream buffer pointer. |
 * | \b in_size             | in          | Input stream buffer size. |
 * | \b out_size            | in          | Output stream buffer pointer. |
 * | \b win_len             | in          | Search window length. |
 * | \b window_factor       | in          | Multiplication factor used to determine partition size. |
 *
 * return
 * | Result     | Description |
 * |:-----------|:------------|
 * | Success    | RAP frame length (RAP header + RAP metadata) |
 * | Fail       | `ERR_INVALID_INPUT`                          |
 * | ^          | -1                                           |
 *
 */
EXPORT_SYM_THREADS AOCL_INT32 aocl_setup_parallel_compress_mt(aocl_thread_group_t* thread_grp,
                                      AOCL_CHAR* src, AOCL_CHAR* dst, AOCL_INT32 in_size,
                                      AOCL_INT32 out_size, AOCL_INT32 win_len,
                                      AOCL_INT32 window_factor);

/**
 * Function to perform partitioning for the multi-threaded compressor.
 *
 * Sets cur_thread_info with info about the partition said thread is expected to process.
 * If an algorithm has a compress bound of D bytes for a source of S bytes, cmpr_bound_pad must be set to (D-S) bytes.
 *
 * Call for each thread from a multi-threaded parallel region.
 *
 * This function partitions the problem and allocates thread working buffer.
 * Each thread holds its own local cur_thread_info that is allocated here.
 * Upon completion of compression, references from cur_thread_info should be copied into thread_grp->threads_info_list[thread_id].
 *
 * | Parameters             | Direction   | Description |
 * |:-----------------------|:-----------:|:------------|
 * | \b thread_grp          | in          | Holds list of thread info, pointers to input and output streams and other information needed for multi-threaded compression. |
 * | \b cur_thread_info     | out         | Current thread info. |
 * | \b cmpr_bound_pad      | in          | Numbers of bytes in addition to source partition size that compressed stream can produce. |
 * | \b thread_id           | in          | Current thread id. |
 *
 * return
 * | Result     | Description |
 * |:-----------|:------------|
 * | Success    | 0           |
 * | Fail       | -1          |
 *
 */
EXPORT_SYM_THREADS AOCL_INT32 aocl_do_partition_compress_mt(aocl_thread_group_t* thread_grp,
                                   aocl_thread_info_t* cur_thread_info,
                                   AOCL_UINT32 cmpr_bound_pad, AOCL_UINT32 thread_id);

/**
 * Function to free memory associated with the multi-threaded compressor.
 *
 * Call from the master thread.
 * Frees the thread related buffers and context.
 *
 * | Parameters             | Direction   | Description |
 * |:-----------------------|:-----------:|:------------|
 * | \b thread_grp          | in/out      | Holds list of thread info, pointers to input and output streams and other information needed for multi-threaded compression. |
 *
 * return void
 *
 */
EXPORT_SYM_THREADS void aocl_destroy_parallel_compress_mt(aocl_thread_group_t* thread_grp);

/**
 * Function to setup the multi-threaded decompressor.
 *
 * Sets thread_grp data structure.
 *
 * Call from a single master thread.
 * Reads the RAP Frame header from the src buffer to setup the thread group.
 * Allocates thread context and determines no. of threads suitable to decompress the input.
 * The master thread shall allocate and hold thread_grp before calling this function.
 *
 *
 * | Parameters             | Direction   | Description |
 * |:-----------------------|:-----------:|:------------|
 * | \b thread_grp          | out         | Holds list of thread info, pointers to input and output streams and other information needed for multi-threaded decompression. |
 * | \b src                 | in          | Input stream buffer pointer. |
 * | \b dst                 | in          | Output stream buffer pointer. |
 * | \b in_size             | in          | Input stream buffer size. |
 * | \b out_size            | in          | Output stream buffer pointer. |
 * | \b use_ST_decompressor| in          | If set to 1, just returns the RAP frame length without setting up the thread group for multi-threaded execution. |
 *
 * return
 * | Result     | Description |
 * |:-----------|:------------|
 * | Success    | RAP frame length (RAP header + RAP metadata) |
 * | Fail       | `ERR_INVALID_INPUT`                          |
 * | ^          | -1                                           |
 *
 */
EXPORT_SYM_THREADS AOCL_INT32 aocl_setup_parallel_decompress_mt(aocl_thread_group_t* thread_grp,
                                        AOCL_CHAR* src, AOCL_CHAR* dst, AOCL_INT32 in_size,
                                        AOCL_INT32 out_size, 
                                        AOCL_INT32 use_ST_decoompressor);
/**
 * Function to perform partitioning for the multi-threaded decompressor.
 *
 * Sets cur_thread_info with info about the partition said thread is expected to process.
 *
 * Call for each thread from a multi-threaded parallel region.
 *
 * This function partitions the problem and allocates thread working buffer.
 * Each thread holds its own local cur_thread_info that is allocated here.
 * Upon completion of decompression, references from cur_thread_info should be copied into thread_grp->threads_info_list[thread_id].
 *
 * | Parameters             | Direction   | Description |
 * |:-----------------------|:-----------:|:------------|
 * | \b thread_grp          | in          | Holds list of thread info, pointers to input and output streams and other information needed for multi-threaded decompression. |
 * | \b cur_thread_info     | out         | Current thread info. |
 * | \b cmpr_bound_pad      | in          | Number of additional padding bytes if needed for the allocated destination buffer. |
 * | \b thread_id           | in          | Current thread id. |
 *
 * return
 * | Result     | Description |
 * |:-----------|:------------|
 * | Success    | 0           |
 * | Success    | 1 when partition source size for this thread_id is 0 |
 * | Fail       | -1                                                   |
 *
 */
EXPORT_SYM_THREADS AOCL_INT32 aocl_do_partition_decompress_mt(aocl_thread_group_t* thread_grp,
                                     aocl_thread_info_t* cur_thread_info,
                                     AOCL_UINT32 cmpr_bound_pad, AOCL_UINT32 thread_id);

/**
 * Function to free memory associated with the multi-threaded decompressor.
 *
 * Call from the master thread.
 * Frees the thread related buffers and context.
 *
 * | Parameters             | Direction   | Description |
 * |:-----------------------|:-----------:|:------------|
 * | \b thread_grp          | in/out      | Holds list of thread info, pointers to input and output streams and other information needed for multi-threaded decompression. |
 *
 * return void
 *
 */
EXPORT_SYM_THREADS void aocl_destroy_parallel_decompress_mt(aocl_thread_group_t* thread_grp);

#ifdef __cplusplus
}
#endif

#endif //THREADS_H
