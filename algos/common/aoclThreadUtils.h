/**
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

 /** @file aoclThreadUtils.h
 *
 *  @brief Common routines used in multithreaded implementations.
 *
 *  This file contains common routines and definitions used in multithreaded 
 *  implementations across methods.
 *
 *  @author Ashish Sriram
 */

#ifndef __COMMON_THREAD_UTILS_H
#define __COMMON_THREAD_UTILS_H

#define AOCL_MT_NO_PARTITIONS(thread_group_handle) (thread_group_handle.threads_info_list == NULL) /* no partitions found after setup */

#define AOCL_MT_CUR_THREAD_SERIAL_ID(ti_cur) ti_cur->thread_id /* serialized id of partition associated with a thread */

#define AOCL_MT_IS_FIRST_PARTITION(ti_cur) \
        (AOCL_MT_CUR_THREAD_SERIAL_ID(ti_cur) == 0) /* is first partition of first thread? */

#define AOCL_MT_IS_LAST_PARTITION(thread_group_handle, ti_cur, thread_id) ( /* is last partition of last thread? */ \
        (thread_id == (thread_group_handle.num_threads - 1) /* last thread */) \
        && ti_cur->next == NULL /* last partition for this thread */)

#define AOCL_MT_PROCESS_PARTITION_START(thread_group_handle, ti_cur, thread_id) /* processing partitions serially. loop start */ \
        aocl_thread_info_t* ti_cur = &thread_group_handle.threads_info_list[thread_id]; \
        while (ti_cur) {


#define AOCL_MT_PROCESS_PARTITION_END(ti_cur) /* processing partitions serially. loop end */ \
        ti_cur = ti_cur->next; /* next linked partition */ \
        }

#endif /* __COMMON_THREAD_UTILS_H */
