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

#include "threads/threads.h"

//Call from a single master thread : Allocates thread context and determines
//how many threads are suitable to compress the input.
//The master thread shall allocate and hold thread_grp before calling this function
//Writes the RAP Frame header to the dest buffer.
//window_len and window_factor passed to this function shall be positive values.
//Returns number of bytes for the RAP frame (RAP header + RAP metadata) on success
INT32 aocl_setup_parallel_compress_mt(aocl_thread_group_t *thread_grp, 
                                      CHAR *src, CHAR *dst, INT32 in_size,
                                      INT32 out_size, INT32 window_len,
                                      INT32 window_factor)
{
    UINT32 max_threads = omp_get_max_threads();
    INT32 rap_frame_len = 0;
    INT32 chunk_size = window_len * window_factor;

    thread_grp->src = src;
    thread_grp->dst = dst;
    thread_grp->src_size = in_size;
    thread_grp->dst_size = out_size;
    thread_grp->search_window_length = window_len;

    if (thread_grp->src_size < chunk_size)
    {
        //Single threaded execution for very small stream (< window_len * window_factor)
        thread_grp->num_threads = 1;
    }
    else
    {
        //Find number of partitions in the src stream
        INT32 num_parallel_partitions = thread_grp->src_size / chunk_size;
        INT32 leftover_size = thread_grp->src_size % chunk_size;
        
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
        *(INT64*)dst = RAP_MAGIC_WORD; //For storing the magic word
        dst += RAP_MAGIC_WORD_BYTES;
        *(UINT32*)dst = rap_frame_len; //For storing the total RAP frame length
        dst += RAP_METADATA_LEN_BYTES;
        *(UINT32*)dst = thread_grp->num_threads; //For storing the no. of threads
    }
#ifdef AOCL_THREADS_LOG
    printf("Input stream size: [%td], Minimum per thread chunk size: [%d]\n",
        thread_grp->src_size, chunk_size);
    printf("Number of max threads: [%d], Number of threads set for execution: [%d]\n",
        max_threads, thread_grp->num_threads);
#endif
    return rap_frame_len;
}

//Call for each thread from a multi-threaded parallel regions.
//Partitions the problem and allocates thread working buffer.
//Each thread holds its own local cur_thread_info that is allocated here.
//At end of the compression, references from cur_thread_info should be copied 
//into thread_grp->threads_info_list[thread_id].
INT32 aocl_do_partition_compress_mt(aocl_thread_group_t *thread_grp,
                                   aocl_thread_info_t *cur_thread_info,
                                   UINT32 cmpr_bound_pad, UINT32 thread_id)
{
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

    cur_thread_info->dst_trap = (CHAR *)malloc(cur_thread_info->dst_trap_size);

#ifdef AOCL_THREADS_LOG
    printf("aocl_do_partition_compress_mt(): thread id: [%d] dest_trap_size [%td]\n",
        cur_thread_info->thread_id, cur_thread_info->dst_trap_size);
#endif

    if (cur_thread_info->dst_trap == NULL)
        return -1;

    cur_thread_info->next = NULL;//Unused as of now

    return 0;
}

//Call from the master thread : Frees the thread related buffers and context
void aocl_destroy_parallel_compress_mt(aocl_thread_group_t *thread_grp)
{
    UINT32 thread_cnt;
    for (thread_cnt = 0; thread_cnt < thread_grp->num_threads; thread_cnt++)
    {
        if (thread_grp->threads_info_list[thread_cnt].dst_trap)
        {
            free(thread_grp->threads_info_list[thread_cnt].dst_trap);
            thread_grp->threads_info_list[thread_cnt].dst_trap = NULL;
        }
    }

    if (thread_grp->threads_info_list)
    {
        free(thread_grp->threads_info_list);
        thread_grp->threads_info_list = NULL;
    }
}

//Call from a single master thread.
//Reads the RAP Frame header from the src buffer to setup the thread group
//Allocates thread context and determines no. of threads suitable to decompress
//the input.
//When use_ST_decompressor is set to 1, this function just returns the RAP frame
//length without setting up the thread group for multi-threaded execution.
//The master thread shall allocate and hold thread_grp before calling this function.
//Returns number of bytes for the RAP frame (RAP header + RAP metadata) on success
INT32 aocl_setup_parallel_decompress_mt(aocl_thread_group_t *thread_grp,
                                        CHAR* src, CHAR* dst, INT32 in_size,
                                        INT32 out_size, INT32 use_ST_decompressor)
{
    CHAR *src_base;
    UINT32 rap_metadata_len;
    UINT32 max_threads = omp_get_max_threads();

    thread_grp->src = src;
    thread_grp->dst = dst;
    thread_grp->src_size = in_size;
    thread_grp->dst_size = out_size;

    src_base = thread_grp->src;

    if ((thread_grp->src_size < RAP_MAGIC_WORD_BYTES) ||
        (RAP_MAGIC_WORD != *(INT64*)src_base))
    {
        //Stream is very small or not in multi-threaded RAP format
        //Decompress in single threaded mode
        thread_grp->num_threads = 1;
        rap_metadata_len = 0;
    }
    else
    {
        CHAR *src_ptr;
        UINT32 num_main_threads;
        src_ptr = src_base + RAP_MAGIC_WORD_BYTES;
        rap_metadata_len = *(UINT32 *)(src_ptr);
        src_ptr += RAP_METADATA_LEN_BYTES;
        num_main_threads = *(UINT32*)(src_ptr);

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

//Call for each thread from a multi-threaded parallel regions.
//Partitions the problem and allocates thread working buffer.
//Each thread holds its own local cur_thread_info that is allocated here.
//At end of the decompression, references from cur_thread_info should be copied
//into thread_grp->threads_info_list[thread_id].
INT32 aocl_do_partition_decompress_mt(aocl_thread_group_t* thread_grp,
                                      aocl_thread_info_t* cur_thread_info,
                                      UINT32 cmpr_bound_pad, UINT32 thread_id)
{
    UINT32 cur_rap_pos = RAP_START_OF_PARTITIONS + 
                            (thread_id * (RAP_DATA_BYTES_WITH_DECOMP_LEN));
    cur_thread_info->partition_src = thread_grp->src +
                            *(UINT32*)(thread_grp->src + cur_rap_pos);
    cur_thread_info->partition_src_size = *(UINT32*)(thread_grp->src +
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
    cur_thread_info->dst_trap = (CHAR*)malloc(cur_thread_info->dst_trap_size);
#else
    cur_thread_info->dst_trap_size = (*(UINT32*)(thread_grp->src +
                                        cur_rap_pos + RAP_DATA_BYTES)) +
                                        cmpr_bound_pad;
    cur_thread_info->dst_trap = (CHAR*)malloc(cur_thread_info->dst_trap_size);
#endif

#ifdef AOCL_THREADS_LOG
    printf("aocl_do_partition_decompress_mt(): thread id: [%d]\n",
        cur_thread_info->thread_id);
#endif

    if (cur_thread_info->dst_trap == NULL)
        return -1;

    return 0;
}

//Call from a single master thread : Frees the thread related buffers and context
void aocl_destroy_parallel_decompress_mt(aocl_thread_group_t* thread_grp)
{
    UINT32 thread_cnt;
    for (thread_cnt = 0; thread_cnt < thread_grp->num_threads; thread_cnt++)
    {
        if (thread_grp->threads_info_list[thread_cnt].dst_trap)
        {
            free(thread_grp->threads_info_list[thread_cnt].dst_trap);
            thread_grp->threads_info_list[thread_cnt].dst_trap = NULL;
        }
    }

    if (thread_grp->threads_info_list)
    {
        free(thread_grp->threads_info_list);
        thread_grp->threads_info_list = NULL;
    }
}
