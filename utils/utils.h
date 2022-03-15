/**
 * Copyright (C) 2022, Advanced Micro Devices. All rights reserved.
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

/** @file utils.h
 *  
 *  @brief Utility functions related to logger, timer and others.
 *
 *  This file contains the utility functions related to logging mechanism,
 *  timer and others along with the associated data structures and macros.
 *
 *  @author S. Biplab Raut
 */

#ifndef UTILS_H
#define UTILS_H

#define AOCL_CL_DTL

#define AOCL_CL_STATS

#define AOCL_CL_CPUID_SIMD_DETECTION

#ifdef AOCL_CL_DTL
#define ERR      1
#define INFO     2
#define DEBUG    3
#define TRACE    4
#else
#define ERR      0
#define INFO     0
#define DEBUG    0
#define TRACE    0

#endif

#ifdef AOCL_CL_STATS
#ifdef WINDOWS
#include <windows.h>
typedef LARGE_INTEGER timer;
typedef LARGE_INTEGER timeVal;
#else
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
typedef struct timespec timer;
typedef struct timespec timeVal;
#endif
#endif

//Logger - DTL
#ifdef AOCL_CL_DTL
#include <stdio.h>
#ifdef __cplusplus
#define LOG(logType, enableLog, str, ...)     do {\
                            if (enableLog)\
                            {\
                                const char *type=NULL;\
                                if (logType == ERR)\
                                    type = "ERR";\
                                else if (logType == INFO)\
                                    type = "INFO";\
                                else if (logType == DEBUG)\
                                    type = "DEBUG";\
                                else if (logType == TRACE)\
                                    type = "TRACE";\
                                if (logType <= enableLog)\
                                {\
                                    fprintf (stdout, "[%s] : %s : %s : %d : ",\
                                    type, __FILE__, __FUNCTION__, __LINE__);\
                                    fprintf(stdout, str, ##__VA_ARGS__);\
                                    fprintf(stdout, "\n");\
                                }\
                            }\
                        } while (0)
#else
#define LOG(logType, enableLog, str, ...)     do {\
                            if (enableLog)\
                            {\
                                const char *type=NULL;\
                                if (logType == ERR)\
                                    type = "ERR";\
                                else if (logType == INFO)\
                                    type = "INFO";\
                                else if (logType == DEBUG)\
                                    type = "DEBUG";\
                                else if (logType == TRACE)\
                                    type = "TRACE";\
                                if (logType <= enableLog)\
                                    printf ("[%s] : %s : %s : %d : "str"\n",\
                                    type, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
                            }\
                        } while (0)
#endif
#else
#define LOG(logType, enableLog, str, ...) 
#endif

//Timer and stats keeping
#ifdef AOCL_CL_STATS
#ifdef WINDOWS
#include <windows.h>
#define initTimer(timer) if(!QueryPerformanceFrequency(&frequency))\
                         {\
                            LOG(ERR, \
                            "QueryPerformanceFrequency based Timer failed.");\
                         }
#define getTime(timeVal) QueryPerformanceCounter(&timeVal)
#define diffTime(timer, startTime, endTime) ((1000000000ULL * \
                (endTime.QuadPart - startTime.QuadPart))/timer.QuadPart)
#else
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define initTimer(timer)
#define getTime(timeVal) clock_gettime(CLOCK_REALTIME, &timeVal)
#define diffTime(timer, startTime, endTime) (1000000000ULL * \
                (endTime.tv_sec - startTime.tv_sec) + \
                endTime.tv_nsec - startTime.tv_nsec)
#endif
#endif

//CPU Features detection using CPUID
#ifdef AOCL_CL_CPUID_SIMD_DETECTION
#ifndef _WIN32
inline VOID cpu_features_detection(INT fn, INT optVal,
                                   INT *eax, INT *ebx,
                                   INT *ecx, INT *edx)
{
    *eax = fn;
    *ecx = optVal;
    *ebx = 0;
    *edx = 0;
    __asm__ ("cpuid            \n\t"
             : "+a" (*eax), "+b" (*ebx), "+c" (*ecx), "+d" (*edx));
}
#endif
#endif

VOID set_cpu_opt_flags(VOID *handle);

#endif
