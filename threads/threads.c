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

 /** @file threads.c
 *
 *  @brief Multi-threaded compression and decompression support via SMP threads
 *
 *  This file contains the functions to setup, partition and destroy the compression
 *  decompression methods.
 *
 *  @author S. Biplab Raut
 */

#include <stdlib.h>
#include <assert.h>
#include "api/types.h"
#include "api/aocl_compression.h"
#include "api/aocl_threads.h"
#include "threads.h"

AOCL_INT32 aocl_setup_parallel_compress_mt(aocl_thread_group_t *thread_grp, 
                                      AOCL_CHAR *src, AOCL_CHAR *dst, AOCL_INT32 in_size,
                                      AOCL_INT32 out_size, AOCL_INT32 window_len,
                                      AOCL_INT32 window_factor)
{
    assert(thread_grp != NULL);
    if (dst == NULL || window_len <= 0 || window_factor <= 0)
        return ERR_INVALID_INPUT;

    AOCL_UINT32 max_threads = omp_get_max_threads();
    AOCL_INT32 rap_frame_len = 0;
    AOCL_INT32 chunk_size = window_len * window_factor;

    thread_grp->src = src;
    thread_grp->dst = dst;
    thread_grp->src_size = in_size;
    thread_grp->dst_size = out_size;
    thread_grp->search_window_length = window_len;
    thread_grp->threads_info_list = NULL;

    if (thread_grp->src_size < chunk_size)
    {
        //Single threaded execution for very small stream (< window_len * window_factor)
        thread_grp->num_threads = 1;
    }
    else
    {
        //Find number of partitions in the src stream
        AOCL_INT32 num_parallel_partitions = thread_grp->src_size / chunk_size;
        AOCL_INT32 leftover_size = thread_grp->src_size % chunk_size;
        
        //Sufficiently large leftover bytes adds another thread for processing
        if (leftover_size >= 
            ((window_factor > 1) ? (chunk_size >> 1) : (window_len >> 1)))
            num_parallel_partitions++;

        //Find number of threads to process the number of parallel partitions
        if (max_threads >= num_parallel_partitions)
            //Use only upto num_parallel_partitions threads
            thread_grp->num_threads = num_parallel_partitions;
        else
            //Use all the available threads
            thread_grp->num_threads = max_threads;

        if (thread_grp->num_threads == 1)
            return rap_frame_len;

        //Tentative partitioning. Actual partitioning is done at thread level
        thread_grp->common_part_src_size = thread_grp->src_size /
                                                thread_grp->num_threads;
        thread_grp->leftover_part_src_bytes = thread_grp->src_size %
                                                thread_grp->num_threads;

        //Allocate threads list to hold references to threads_info
        thread_grp->threads_info_list = (aocl_thread_info_t*)malloc(
                        sizeof(aocl_thread_info_t) * thread_grp->num_threads);
        if (thread_grp->threads_info_list == NULL)
            return -1;

        rap_frame_len = RAP_FRAME_LEN_WITH_DECOMP_LENGTH(thread_grp->num_threads, 0);
        *(AOCL_INT64*)dst = RAP_MAGIC_WORD; //For storing the magic word
        dst += RAP_MAGIC_WORD_BYTES;
        *(AOCL_UINT32*)dst = rap_frame_len; //For storing the total RAP frame length
        dst += RAP_METADATA_LEN_BYTES;
        *(AOCL_UINT32*)dst = thread_grp->num_threads; //For storing the no. of threads
    }
#ifdef AOCL_THREADS_LOG
    printf("Input stream size: [%td], Minimum per thread chunk size: [%d]\n",
        thread_grp->src_size, chunk_size);
    printf("Number of max threads: [%d], Number of threads set for execution: [%d]\n",
        max_threads, thread_grp->num_threads);
#endif
    return rap_frame_len;
}

AOCL_INT32 aocl_do_partition_compress_mt(aocl_thread_group_t *thread_grp,
                                   aocl_thread_info_t *cur_thread_info,
                                   AOCL_UINT32 cmpr_bound_pad, AOCL_UINT32 thread_id)
{
    assert(thread_grp != NULL);
    assert(cur_thread_info != NULL);
    cur_thread_info->partition_src = thread_grp->src + 
                                (thread_grp->common_part_src_size * thread_id);
    cur_thread_info->thread_id = thread_id;

    if (thread_id != (thread_grp->num_threads - 1))
        cur_thread_info->partition_src_size = thread_grp->common_part_src_size;
    else
        cur_thread_info->partition_src_size = thread_grp->common_part_src_size +
                                        thread_grp->leftover_part_src_bytes;
    
    cur_thread_info->dst_trap_size = cur_thread_info->partition_src_size +
                                        cmpr_bound_pad;

    cur_thread_info->dst_trap = (AOCL_CHAR *)malloc(cur_thread_info->dst_trap_size);

#ifdef AOCL_THREADS_LOG
    printf("aocl_do_partition_compress_mt(): thread id: [%d] dest_trap_size [%td]\n",
        cur_thread_info->thread_id, cur_thread_info->dst_trap_size);
#endif

    if (cur_thread_info->dst_trap == NULL)
        return -1;

    cur_thread_info->next = NULL;//Unused as of now

    return 0;
}

void aocl_destroy_parallel_compress_mt(aocl_thread_group_t *thread_grp)
{
    assert(thread_grp != NULL);
    if (thread_grp->threads_info_list)
    {
        AOCL_UINT32 thread_cnt;
        for (thread_cnt = 0; thread_cnt < thread_grp->num_threads; thread_cnt++)
        {
            if (thread_grp->threads_info_list[thread_cnt].dst_trap)
            {
                free(thread_grp->threads_info_list[thread_cnt].dst_trap);
                thread_grp->threads_info_list[thread_cnt].dst_trap = NULL;
            }
        }
        free(thread_grp->threads_info_list);
        thread_grp->threads_info_list = NULL;
    }
}

AOCL_INT32 aocl_setup_parallel_decompress_mt(aocl_thread_group_t *thread_grp,
                                        AOCL_CHAR* src, AOCL_CHAR* dst, AOCL_INT32 in_size,
                                        AOCL_INT32 out_size, AOCL_INT32 use_ST_decompressor)
{
    assert(thread_grp != NULL);
    if (src == NULL)
        return ERR_INVALID_INPUT;

    AOCL_CHAR *src_base;
    AOCL_UINT32 rap_metadata_len;
    AOCL_UINT32 max_threads = omp_get_max_threads();

    thread_grp->src = src;
    thread_grp->dst = dst;
    thread_grp->src_size = in_size;
    thread_grp->dst_size = out_size;
    thread_grp->threads_info_list = NULL;

    src_base = thread_grp->src;

    if ((thread_grp->src_size < RAP_MAGIC_WORD_BYTES) ||
        (RAP_MAGIC_WORD != *(AOCL_INT64*)src_base))
    {
        //Stream is very small or not in multi-threaded RAP format
        //Decompress in single threaded mode
        thread_grp->num_threads = 1;
        rap_metadata_len = 0;
    }
    else
    {
        AOCL_CHAR *src_ptr;
        AOCL_UINT32 num_main_threads;
        src_ptr = src_base + RAP_MAGIC_WORD_BYTES;
        rap_metadata_len = *(AOCL_UINT32 *)(src_ptr);
        src_ptr += RAP_METADATA_LEN_BYTES;
        num_main_threads = *(AOCL_UINT32*)(src_ptr);

        if (num_main_threads == 0)
            return -1; // invalid main thread count in stream. Must be >= 1.

        if (use_ST_decompressor == 1)
            return rap_metadata_len;
        
        //Find number of threads to process the number of parallel partitions
        if (max_threads >= num_main_threads)
            //Use exactly num_main_threads threads
            thread_grp->num_threads = num_main_threads;
        else
            //When max_threads is lesser than num_main_threads, we can handle
            //in two ways:
            //1) Multi partitions are assigned to each thread as a linked list
            //using next pointer - This is the efficient and smart approach.
            //2) Decompress using single-threaded decompressor - This is the
            //simplest (less complex) and less efficient approach.
            //Currently, approach 2) is implemented.
            //thread_grp->num_threads = max_threads;
            thread_grp->num_threads = 1;

        if (thread_grp->num_threads == 1)
            return rap_metadata_len;

        //Tentative partitioning. Actual partitioning is done at thread level
        thread_grp->common_part_src_size = thread_grp->dst_size /
                                            thread_grp->num_threads;
        thread_grp->leftover_part_src_bytes = thread_grp->dst_size %
                                            thread_grp->num_threads;

        //Allocate threads list to hold references to threads_info
        thread_grp->threads_info_list = (aocl_thread_info_t*)malloc(
                        sizeof(aocl_thread_info_t) * thread_grp->num_threads);

        if (thread_grp->threads_info_list == NULL)
            return -1;
    }
    return rap_metadata_len;
}

AOCL_INT32 aocl_do_partition_decompress_mt(aocl_thread_group_t* thread_grp,
                                      aocl_thread_info_t* cur_thread_info,
                                      AOCL_UINT32 cmpr_bound_pad, AOCL_UINT32 thread_id)
{
    assert(thread_grp != NULL);
    assert(cur_thread_info != NULL);

    AOCL_UINT32 cur_rap_pos = RAP_START_OF_PARTITIONS + 
                            (thread_id * (RAP_DATA_BYTES_WITH_DECOMP_LEN));
    cur_thread_info->partition_src = thread_grp->src +
                            *(AOCL_UINT32*)(thread_grp->src + cur_rap_pos);
    cur_thread_info->partition_src_size = *(AOCL_UINT32*)(thread_grp->src +
                                            cur_rap_pos + RAP_OFFSET_BYTES);
    cur_thread_info->thread_id = thread_id;

    if (cur_thread_info->partition_src_size == 0)
    {
        cur_thread_info->dst_trap = NULL;
        return 1;
    }

#ifdef APPROX_PADDED_DST_CHUNK //DO NOT ENABLE/USE THIS UNLESS YOU KNOW THE PAD SIZE
    cur_thread_info->dst_trap_size = 
        (thread_grp->dst_size / thread_grp->num_threads) + 
        (thread_grp->dst_size % thread_grp->num_threads) + cmpr_bound_pad;
    cur_thread_info->dst_trap = (AOCL_CHAR*)malloc(cur_thread_info->dst_trap_size);
#else
    cur_thread_info->dst_trap_size = (*(AOCL_UINT32*)(thread_grp->src +
                                        cur_rap_pos + RAP_DATA_BYTES)) +
                                        cmpr_bound_pad;
    cur_thread_info->dst_trap = (AOCL_CHAR*)malloc(cur_thread_info->dst_trap_size);
#endif

#ifdef AOCL_THREADS_LOG
    printf("aocl_do_partition_decompress_mt(): thread id: [%d]\n",
        cur_thread_info->thread_id);
#endif

    if (cur_thread_info->dst_trap == NULL)
        return -1;

    return 0;
}

void aocl_destroy_parallel_decompress_mt(aocl_thread_group_t* thread_grp)
{
    assert(thread_grp != NULL);

    if (thread_grp->threads_info_list)
    {
        AOCL_UINT32 thread_cnt;
        for (thread_cnt = 0; thread_cnt < thread_grp->num_threads; thread_cnt++)
        {
            if (thread_grp->threads_info_list[thread_cnt].dst_trap)
            {
                free(thread_grp->threads_info_list[thread_cnt].dst_trap);
                thread_grp->threads_info_list[thread_cnt].dst_trap = NULL;
            }
        }
        free(thread_grp->threads_info_list);
        thread_grp->threads_info_list = NULL;
    }
}

AOCL_INT32 aocl_get_rap_frame_bound_mt(void) {
    AOCL_UINT32 max_threads = omp_get_max_threads();
    return RAP_FRAME_LEN_WITH_DECOMP_LENGTH(max_threads, 0); // upper bound of rap frame length in bytes based on max threads possible
}

AOCL_INT32 aocl_skip_rap_frame_mt(AOCL_CHAR* src, AOCL_INT32 src_size)
{
    if (src == NULL)
        return ERR_INVALID_INPUT;

    if ((src_size < RAP_MAGIC_WORD_BYTES) ||
        (RAP_MAGIC_WORD != *(AOCL_INT64*)src))
    {
        return 0; //Stream is very small or not in multi-threaded RAP format
    }
    else
    {
        AOCL_CHAR* src_ptr = src + RAP_MAGIC_WORD_BYTES;
        AOCL_UINT32 rap_metadata_len = *(AOCL_UINT32*)(src_ptr);
        return rap_metadata_len;
    }
}
