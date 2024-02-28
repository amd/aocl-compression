/* compress.c -- compress a memory buffer
 * Copyright (C) 1995-2005, 2014, 2016 Jean-loup Gailly, Mark Adler
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#define ZLIB_INTERNAL
#include "zlib.h"
#include "utils/utils.h"

#ifdef AOCL_ZLIB_OPT
#include "aocl_zlib_setup.h"

int zlibOptOff = 0; // default, run reference code
static int setup_ok_zlib = 0; // flag to indicate status of dynamic dispatcher setup

/* Dynamic dispatcher setup function for native APIs.
 * All native APIs that call aocl optimized functions within their call stack,
 * must call AOCL_SETUP_NATIVE() at the start of the function. This sets up 
 * appropriate code paths to take based on user defined environment variables,
 * as well as cpu instruction set supported by the runtime machine. */
static void aocl_setup_native(void);
#define AOCL_SETUP_NATIVE() aocl_setup_native()
#else
#define AOCL_SETUP_NATIVE()
#endif /* AOCL_ZLIB_OPT */

/* AOCL-Compression defined setup function that sets up ZLIB with the right
*  AMD optimized zlib routines depending upon the CPU features. */
ZEXTERN char * ZEXPORT aocl_setup_zlib(int optOff, int optLevel, int insize,
    int level, int windowLog)
{
#ifdef AOCL_ZLIB_OPT
    AOCL_ENTER_CRITICAL(setup_zlib)
    if (!setup_ok_zlib) {
        optOff = optOff ? 1 : get_disable_opt_flags(0);
        zlibOptOff = optOff;
        aocl_setup_deflate(optOff, optLevel);
        aocl_setup_inflate(optOff, optLevel);
        aocl_setup_adler32(optOff, optLevel);
        setup_ok_zlib = 1;
    }
    AOCL_EXIT_CRITICAL(setup_zlib)
#endif /* AOCL_ZLIB_OPT */
    return NULL;
}

#ifdef AOCL_ZLIB_OPT
static void aocl_setup_native(void) {
    AOCL_ENTER_CRITICAL(setup_zlib)
    if (!setup_ok_zlib) {
        int optLevel = get_cpu_opt_flags(0);
        int optOff = get_disable_opt_flags(0);
        zlibOptOff = optOff;
        aocl_setup_deflate(optOff, optLevel);
        aocl_setup_inflate(optOff, optLevel);
        aocl_setup_adler32(optOff, optLevel);
        setup_ok_zlib = 1;
    }
    AOCL_EXIT_CRITICAL(setup_zlib)
}
#endif

ZEXTERN void ZEXPORT aocl_destroy_zlib (void) {
#ifdef AOCL_ZLIB_OPT
    AOCL_ENTER_CRITICAL(setup_zlib)
    setup_ok_zlib = 0;
    AOCL_EXIT_CRITICAL(setup_zlib)
    aocl_destroy_adler32();
    aocl_destroy_deflate();
    aocl_destroy_inflate();
#endif /* AOCL_ZLIB_OPT */
}

#ifdef AOCL_ENABLE_THREADS
#define ZLIB_MT_WINDOW_LEN 32768
#include <string.h>
#include "threads/threads.h"
#endif
/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/

#ifdef AOCL_ENABLE_THREADS
static inline int compress2_ST(aocl_thread_info_t *cThread, int level, int finalFlush) {
    Bytef *dest = (Bytef*)cThread->dst_trap;
    uLongf *destLen = (uLongf*)&(cThread->dst_trap_size);
    Bytef *source = (Bytef*)cThread->partition_src;
    uLong sourceLen = cThread->partition_src_size;

    if(destLen == NULL)
    {
        return Z_BUF_ERROR;
    }
    
    z_stream stream;
    int err;
    const uInt max = (uInt)-1;
    uLong left;

    left = *destLen;
    *destLen = 0;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit(&stream, level);
    if (err != Z_OK) return err;

    stream.next_out = dest;
    stream.avail_out = 0;
    stream.next_in = (z_const Bytef *)source;
    stream.avail_in = 0;

    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong)max ? max : (uInt)left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
            sourceLen -= stream.avail_in;
        }
        err = deflate(&stream, finalFlush);
        if(finalFlush != Z_FINISH && sourceLen == 0)
            break;
    } while (err == Z_OK);

    cThread->last_bytes_len = stream.adler;

    *destLen = stream.total_out;
    deflateEnd(&stream);
    return err == Z_STREAM_END ? Z_OK : err;
}

uLong ZEXPORT compressBound_ST(uLong sourceLen) {
    return sourceLen + (sourceLen >> 12) + (sourceLen >> 14) +
           (sourceLen >> 25) + 13;
}
#endif /* AOCL_ENABLE_THREADS */

int ZEXPORT compress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen, int level) {
    LOG_UNFORMATTED(TRACE, logCtx, "Enter");
    AOCL_SETUP_NATIVE();
    if(destLen == NULL)
    {
        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return Z_BUF_ERROR;
    }
#ifndef AOCL_ENABLE_THREADS //Non threaded
    z_stream stream;
    int err;
    const uInt max = (uInt)-1;
    uLong left;

    left = *destLen;
    *destLen = 0;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit(&stream, level);
    if (err != Z_OK)
    {
        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return err;
    }

    stream.next_out = dest;
    stream.avail_out = 0;
    stream.next_in = (z_const Bytef *)source;
    stream.avail_in = 0;

    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong)max ? max : (uInt)left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
            sourceLen -= stream.avail_in;
        }
        err = deflate(&stream, sourceLen ? Z_NO_FLUSH : Z_FINISH);
    } while (err == Z_OK);

    *destLen = stream.total_out;
    deflateEnd(&stream);
    LOG_UNFORMATTED(INFO, logCtx, "Exit");
    
    return err == Z_STREAM_END ? Z_OK : err;
#else //Threaded
    int result = Z_OK;
    aocl_thread_group_t thread_group_handle;
    aocl_thread_info_t cur_thread_info;
    AOCL_INT32 rap_metadata_len = -1;
    AOCL_UINT32 thread_cnt = 0;

    rap_metadata_len = aocl_setup_parallel_compress_mt(&thread_group_handle, (char *)source,
                                                 (char *)dest, sourceLen, *destLen,
                                                 ZLIB_MT_WINDOW_LEN, WINDOW_FACTOR);
    if (rap_metadata_len < 0)
        return Z_MEM_ERROR;

    if (thread_group_handle.num_threads == 1)
    {
        cur_thread_info.partition_src = (char *)source;
        cur_thread_info.dst_trap = (char *)dest;
        cur_thread_info.dst_trap_size = *destLen;
        cur_thread_info.partition_src_size = sourceLen;
        result = compress2_ST(&cur_thread_info, level, Z_FINISH);
        *destLen = cur_thread_info.dst_trap_size;
        return result;
    }
    else
    {
        *destLen = rap_metadata_len;
#ifdef AOCL_THREADS_LOG
        printf("Compress Thread [id: %d] : Before parallel region\n", omp_get_thread_num());
#endif

#pragma omp parallel private(cur_thread_info) shared(thread_group_handle) num_threads(thread_group_handle.num_threads)
        {
#ifdef AOCL_THREADS_LOG
            printf("Compress Thread [id: %d] : Inside parallel region\n", omp_get_thread_num());
#endif
            AOCL_UINT32 thread_id = omp_get_thread_num();
            AOCL_UINT32 cmpr_bound_pad;
            AOCL_UINT32 is_error = Z_OK;
            if (thread_id != (thread_group_handle.num_threads - 1))
                cmpr_bound_pad = compressBound_ST(thread_group_handle.common_part_src_size);
            else
                cmpr_bound_pad = compressBound_ST(thread_group_handle.common_part_src_size + 
                                        thread_group_handle.leftover_part_src_bytes);
            if (aocl_do_partition_compress_mt(&thread_group_handle, &cur_thread_info, cmpr_bound_pad, thread_id) == 0)
            {
                if (thread_id != (thread_group_handle.num_threads - 1))
                    is_error = compress2_ST(&cur_thread_info, level, Z_SYNC_FLUSH);
                else
                    is_error = compress2_ST(&cur_thread_info, level, Z_FINISH);
            } //aocl_do_partition_compress_mt
#ifdef AOCL_THREADS_LOG
            printf("Compress Thread [id: %d] : Return value %d\n", omp_get_thread_num(), is_error);
#endif
            thread_group_handle.threads_info_list[thread_id].partition_src = cur_thread_info.partition_src;
            thread_group_handle.threads_info_list[thread_id].dst_trap = cur_thread_info.dst_trap;
            thread_group_handle.threads_info_list[thread_id].additional_state_info = NULL;
            thread_group_handle.threads_info_list[thread_id].dst_trap_size = cur_thread_info.dst_trap_size;
            thread_group_handle.threads_info_list[thread_id].partition_src_size = cur_thread_info.partition_src_size;
            thread_group_handle.threads_info_list[thread_id].last_bytes_len = cur_thread_info.last_bytes_len;
            thread_group_handle.threads_info_list[thread_id].is_error = is_error;
            thread_group_handle.threads_info_list[thread_id].num_child_threads = 0;
        } //#pragma omp parallel
#ifdef AOCL_THREADS_LOG
        printf("Compress Thread [id: %d] : After parallel region\n", omp_get_thread_num());
#endif
        //Post processing in single-threaded mode: Prepares RAP frame and joins the last sequences of the neighboring threads

        // <-- RAP Header -->
        //Add at the start of the stream : Although it can be at the end or at any other point in the stream, but it is more easier for parsing at the start
        AOCL_CHAR* dst_org = thread_group_handle.dst;
        AOCL_CHAR* dst_ptr = dst_org;
        AOCL_UINT32 decomp_len;
        thread_group_handle.dst += rap_metadata_len;
        dst_ptr += RAP_START_OF_PARTITIONS;
        // <-- RAP Header -->

        // <-- RAP Metadata payload -->
        AOCL_UINT32 offset = 0, adler = 1;
        for (; thread_cnt < thread_group_handle.num_threads; thread_cnt++)
        {
            cur_thread_info = thread_group_handle.threads_info_list[thread_cnt];
            //In case of any thread partitioning or alloc errors, exit the compression process with error
            if (cur_thread_info.is_error || cur_thread_info.dst_trap_size < 0)
            {
                result = cur_thread_info.is_error;
                aocl_destroy_parallel_compress_mt(&thread_group_handle);
    #ifdef AOCL_THREADS_LOG
                printf("Compress Thread [id: %d] : Encountered ERROR\n", thread_cnt);
    #endif
                return result;
            }

            //Copy this thread's chunk to the output final buffer
            memcpy(thread_group_handle.dst, cur_thread_info.dst_trap + offset, cur_thread_info.dst_trap_size - offset);

            *(AOCL_UINT32*)dst_ptr = *destLen; //For storing this thread's RAP offset
            dst_ptr += RAP_OFFSET_BYTES;
            *(AOCL_INT32*)dst_ptr = cur_thread_info.dst_trap_size - offset; //For storing this thread's RAP length
            dst_ptr += RAP_LEN_BYTES;
            //For storing this thread's decompressed (src) length
            decomp_len = cur_thread_info.partition_src_size;

            *(AOCL_INT32*)dst_ptr = decomp_len;
            dst_ptr += DECOMP_LEN_BYTES;
            thread_group_handle.dst += (cur_thread_info.dst_trap_size - offset);
            *destLen += (cur_thread_info.dst_trap_size - offset);
            offset = 2; // skip 2 bytes zlib header
            // combining partition checksum value stored in last_bytes_len of thread info
            adler = adler32_combine(adler, cur_thread_info.last_bytes_len, cur_thread_info.partition_src_size);
        }
        //Update checksum
        adler = ((((adler) >> 24) & 0xff) + (((adler) >> 8) & 0xff00) + (((adler) & 0xff00) << 8) + (((adler) & 0xff) << 24));
        memcpy((thread_group_handle.dst - 4), &adler, 4);

        aocl_destroy_parallel_compress_mt(&thread_group_handle);

        LOG_UNFORMATTED(INFO, logCtx, "Exit");
        return result;
    }
#endif /* !AOCL_ENABLE_THREADS */
}

/* ===========================================================================
 */
int ZEXPORT compress(Bytef *dest, uLongf *destLen, const Bytef *source,
                     uLong sourceLen) {
    return compress2(dest, destLen, source, sourceLen, Z_DEFAULT_COMPRESSION);
}

/* ===========================================================================
     If the default memLevel or windowBits for deflateInit() is changed, then
   this function needs to be updated.
 */
uLong ZEXPORT compressBound(uLong sourceLen) {
#ifdef AOCL_ENABLE_THREADS
    return sourceLen + (sourceLen >> 12) + (sourceLen >> 14) +
           (sourceLen >> 25) + 13 + RAP_FRAME_LEN_WITH_DECOMP_LENGTH(omp_get_max_threads(), 0);
#else
    return sourceLen + (sourceLen >> 12) + (sourceLen >> 14) +
           (sourceLen >> 25) + 13;
#endif
}
