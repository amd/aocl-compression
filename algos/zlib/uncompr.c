/* uncompr.c -- decompress a memory buffer
 * Copyright (C) 1995-2003, 2010, 2014, 2016 Jean-loup Gailly, Mark Adler
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#define ZLIB_INTERNAL
#include "zlib.h"

#include "utils/utils.h"

#ifdef AOCL_ENABLE_THREADS
#include <string.h>
#include "threads/threads.h"
#define MAX_WBITS 15
#endif
/* ===========================================================================
     Decompresses the source buffer into the destination buffer.  *sourceLen is
   the byte length of the source buffer. Upon entry, *destLen is the total size
   of the destination buffer, which must be large enough to hold the entire
   uncompressed data. (The size of the uncompressed data must have been saved
   previously by the compressor and transmitted to the decompressor by some
   mechanism outside the scope of this compression library.) Upon exit,
   *destLen is the size of the decompressed data and *sourceLen is the number
   of source bytes consumed. Upon return, source + *sourceLen points to the
   first unused input byte.

     uncompress returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer, or
   Z_DATA_ERROR if the input data was corrupted, including if the input data is
   an incomplete zlib stream.
*/
#ifdef AOCL_ENABLE_THREADS
#ifdef AOCL_ZLIB_OPT
extern uint32_t adler32_x86_internal(uint32_t adler, const Bytef *buf, z_size_t len);
#endif /* AOCL_ZLIB_OPT */
static inline AOCL_UINT32 partition_checksum(Bytef *source, AOCL_INTP length)
{
#ifdef AOCL_ZLIB_OPT
    return adler32_x86_internal(1, source, length);
#else
    return adler32(1, source, length);
#endif /* AOCL_ZLIB_OPT */
}
static inline int uncompress2_ST(Bytef *dest, uLongf *destLen, const Bytef *source,
                        uLong *sourceLen, short winBits) {
    if(destLen == NULL || sourceLen == NULL)
    {
        return Z_BUF_ERROR;
    }
    
    z_stream stream;
    int err;
    const uInt max = (uInt)-1;
    uLong len, left;
    Byte buf[1];    /* for detection of incomplete stream when *destLen == 0 */

    len = *sourceLen;
    if (*destLen) {
        left = *destLen;
        *destLen = 0;
    }
    else {
        left = 1;
        dest = buf;
    }

    stream.next_in = (z_const Bytef *)source;
    stream.avail_in = 0;
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = inflateInit2(&stream, winBits);
    if (err != Z_OK) return err;

    stream.next_out = dest;
    stream.avail_out = 0;

    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong)max ? max : (uInt)left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = len > (uLong)max ? max : (uInt)len;
            len -= stream.avail_in;
        }

        err = inflate(&stream, Z_NO_FLUSH);
    } while (err == Z_OK);

    *sourceLen -= len + stream.avail_in;
    if (dest != buf)
        *destLen = stream.total_out;
    else if (stream.total_out && err == Z_BUF_ERROR)
        left = 1;

    inflateEnd(&stream);
    return err == Z_STREAM_END ? Z_OK :
           err == Z_NEED_DICT ? Z_DATA_ERROR  :
           err == Z_BUF_ERROR && left + stream.avail_out ? Z_DATA_ERROR :
           err;
}
#endif

int ZEXPORT uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                        uLong *sourceLen) {
#ifndef AOCL_ENABLE_THREADS //Non threaded
    if(destLen == NULL || sourceLen == NULL)
    {
        return Z_BUF_ERROR;
    }
    
    z_stream stream;
    int err;
    const uInt max = (uInt)-1;
    uLong len, left;
    Byte buf[1];    /* for detection of incomplete stream when *destLen == 0 */

    len = *sourceLen;
    if (*destLen) {
        left = *destLen;
        *destLen = 0;
    }
    else {
        left = 1;
        dest = buf;
    }

    stream.next_in = (z_const Bytef *)source;
    stream.avail_in = 0;
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = inflateInit(&stream);
    if (err != Z_OK) return err;

    stream.next_out = dest;
    stream.avail_out = 0;

    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong)max ? max : (uInt)left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = len > (uLong)max ? max : (uInt)len;
            len -= stream.avail_in;
        }
        err = inflate(&stream, Z_NO_FLUSH);
    } while (err == Z_OK);

    *sourceLen -= len + stream.avail_in;
    if (dest != buf)
        *destLen = stream.total_out;
    else if (stream.total_out && err == Z_BUF_ERROR)
        left = 1;

    inflateEnd(&stream);
    return err == Z_STREAM_END ? Z_OK :
           err == Z_NEED_DICT ? Z_DATA_ERROR  :
           err == Z_BUF_ERROR && left + stream.avail_out ? Z_DATA_ERROR :
           err;
#else //Threaded
    if(destLen == NULL || sourceLen == NULL || source == NULL)
    {
        return Z_BUF_ERROR;
    }
    int result = Z_OK;
    aocl_thread_group_t thread_group_handle;
    aocl_thread_info_t cur_thread_info;
    AOCL_INT32 use_ST_decompressor = 0;
    AOCL_UINT32 thread_cnt = 0;
    AOCL_INT32 rap_metadata_len = 0;

    rap_metadata_len = aocl_setup_parallel_decompress_mt(&thread_group_handle, (char *)source, (char *)dest,
                                                   *sourceLen, *destLen, use_ST_decompressor);

    if(rap_metadata_len < 0)
        return Z_MEM_ERROR;

    if (thread_group_handle.num_threads == 1 || use_ST_decompressor == 1)
    {
        source += rap_metadata_len;
        *sourceLen -= rap_metadata_len;

        return uncompress2_ST(dest, destLen, source, sourceLen, MAX_WBITS);
    }
    else
    {
#ifdef AOCL_THREADS_LOG
        printf("Decompress Thread [id: %d] : Before parallel region\n", omp_get_thread_num());
#endif
#pragma omp parallel private(cur_thread_info) shared(thread_group_handle) num_threads(thread_group_handle.num_threads)
        {
#ifdef AOCL_THREADS_LOG
            printf("Decompress Thread [id: %d] : Inside parallel region\n", omp_get_thread_num());
#endif
            AOCL_UINT32 cmpr_bound_pad = 0;
            AOCL_UINT32 is_error = 0;
            AOCL_UINT32 thread_id = omp_get_thread_num();
            AOCL_INT32 thread_parallel_res = 0;

            thread_parallel_res = aocl_do_partition_decompress_mt(&thread_group_handle, &cur_thread_info, cmpr_bound_pad, thread_id);
            thread_group_handle.threads_info_list[thread_id].additional_state_info = NULL;
            if(thread_id == 0) // skip header from first partition
            {
                cur_thread_info.partition_src += 2;
                cur_thread_info.partition_src_size -= 2;
            }
            else if(thread_id == (thread_group_handle.num_threads - 1)) // skip trailer from first partition
            {
                cur_thread_info.partition_src_size -= 4;
                thread_group_handle.threads_info_list[thread_id].additional_state_info = cur_thread_info.partition_src + cur_thread_info.partition_src_size;
            }
            if (thread_parallel_res == 0)
            {
                is_error = uncompress2_ST((Bytef *)cur_thread_info.dst_trap, (uLong *)&(cur_thread_info.dst_trap_size),
                                            (Bytef *)cur_thread_info.partition_src, (uLong *)&(cur_thread_info.partition_src_size), -1 * MAX_WBITS);
                cur_thread_info.last_bytes_len = partition_checksum((Bytef *)cur_thread_info.dst_trap, cur_thread_info.dst_trap_size);
            }//aocl_do_partition_decompress_mt
            else if (thread_parallel_res == 1)
            {
                is_error = 0;
            }
#ifdef AOCL_THREADS_LOG
            printf("Decompress Thread [id: %d] : Return value %d\n", omp_get_thread_num(), is_error);
#endif
            thread_group_handle.threads_info_list[thread_id].partition_src = cur_thread_info.partition_src;
            thread_group_handle.threads_info_list[thread_id].dst_trap = cur_thread_info.dst_trap;
            thread_group_handle.threads_info_list[thread_id].dst_trap_size = cur_thread_info.dst_trap_size;
            thread_group_handle.threads_info_list[thread_id].partition_src_size = cur_thread_info.partition_src_size;
            thread_group_handle.threads_info_list[thread_id].last_bytes_len = cur_thread_info.last_bytes_len; // storing checksum value
            thread_group_handle.threads_info_list[thread_id].is_error = is_error;
            thread_group_handle.threads_info_list[thread_id].num_child_threads = 0;

        }//#pragma omp parallel
#ifdef AOCL_THREADS_LOG
        printf("Decompress Thread [id: %d] : After parallel region\n", omp_get_thread_num());
#endif
        //For all the threads: Write to a single output buffer in a single-threaded mode
        AOCL_UINT32 adler = 1;
        for (thread_cnt = 0; thread_cnt < thread_group_handle.num_threads; thread_cnt++)
        {
            cur_thread_info = thread_group_handle.threads_info_list[thread_cnt];
            //In case of any thread partitioning or alloc errors, exit the compression process with error
            if (cur_thread_info.is_error && cur_thread_info.is_error != Z_BUF_ERROR)
            {
                result = cur_thread_info.is_error;
                aocl_destroy_parallel_decompress_mt(&thread_group_handle);
#ifdef AOCL_THREADS_LOG
                printf("Decompress Thread [id: %d] : Encountered ERROR\n", thread_cnt);
#endif
                return result;
            }
            result = cur_thread_info.is_error;
            //Copy this thread's chunk to the output final buffer
            memcpy(thread_group_handle.dst, cur_thread_info.dst_trap, cur_thread_info.dst_trap_size);
            thread_group_handle.dst += cur_thread_info.dst_trap_size;
            adler = adler32_combine(adler, cur_thread_info.last_bytes_len, cur_thread_info.dst_trap_size);
        }
        // verify uncompressed data integrity
        AOCL_UINT32 stream_adler = *(AOCL_UINT32*)(thread_group_handle.threads_info_list[thread_group_handle.num_threads - 1].additional_state_info);
        adler = ((((adler) >> 24) & 0xff) + (((adler) >> 8) & 0xff00) + (((adler) & 0xff00) << 8) + (((adler) & 0xff) << 24));
        if(adler != stream_adler)
            result = Z_DATA_ERROR;

        aocl_destroy_parallel_decompress_mt(&thread_group_handle);

        return result;
    }
#endif /* !AOCL_ENABLE_THREADS */
}

int ZEXPORT uncompress(Bytef *dest, uLongf *destLen, const Bytef *source,
                       uLong sourceLen) {
    LOG_UNFORMATTED(TRACE, logCtx, "Enter");
    int ret = uncompress2(dest, destLen, source, &sourceLen);
    LOG_UNFORMATTED(INFO, logCtx, "Exit");
    return ret;
}
