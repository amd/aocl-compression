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
 
 /** @file codec_bench.c
 *  
 *  @brief Test bench application to test the AOCL Compression library
 *
 *  This file contains the functions to test, verify and benchmark all the
 *  supported compression and decompression methods.
 *
 *  @author S. Biplab Raut
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api/types.h"
#include "api/api.h"
#include "utils/utils.h"
#include "codec_bench.h"

//Input test file name
CHAR inFile[MAX_FILENAME_LEN];

void print_user_options (void)
{
    printf("\nAOCL Compression Library version: %s\n", aocl_llc_version());
    printf("Internal Library version: %s\n", INTERNAL_LIBRARY_VERSION);
    printf("C Compiler: %s\n", CCompiler);
    printf("C++ Compiler: %s\n", CXXCompiler);
    printf("Compile Options: %s%s\n\n", CFLAGS_SET1, CFLAGS_SET2);
    printf("Usage: aocl_compression_bench <options> input\n\n");
    printf("where input is the test file name and <options> can be:\n");
    printf("-h | --help Print help info\n");
    printf("-l          List all the available compression/decompression methods\n");
    printf("-a          Use all the available compression/decompression methods\n");
    printf("-m<>        Maximum size in MBs of the input for compression and decompression. Default=MIN(filesize, 1024 MB)\n");
    printf("-e<>:<>:<>  Compression/decompression method. Optional level and additional param may be specified using : separator\n");
    printf("-i<>        Number of iterations of compression/decompression\n");
    printf("-t          Verification and functional tests of the compression/decompression methods\n");
    printf("-p          Print stats like compression/decompression time, speed, ratio\n");
#ifdef AOCL_DYNAMIC_DISPATCHER
    printf("-o          Turn off all optimizations\n");
#endif
    printf("-v<>        Enable verbosity. Allowed values: 1 for Error (default), 2 for Info, 3 for Debug, 4 for Trace.\n");
    printf("-c          Run IPP library methods. Make sure to set the library path with LD_LIBRARY_PATH.\n\n");
}

void print_supported_compressors (void)
{
   printf("\nSupported compression/decompression methods along with their supported levels are:\n\n");
   printf("===========================================\n");
   printf("Method Name\tLower Level\tUpper Level\n");
   printf("===========================================\n");
   printf("LZ4\t\t %s\t\t%s\n", "NA", "NA");
   printf("LZ4HC\t\t %ld\t\t%ld\n", codec_list[LZ4HC].lower_level, codec_list[LZ4HC].upper_level);
   printf("LZMA\t\t %ld\t\t%ld\n", codec_list[LZMA].lower_level, codec_list[LZMA].upper_level);
   printf("SNAPPY\t\t %s\t\t%s\n", "NA", "NA");
   printf("ZLIB\t\t %ld\t\t%ld\n", codec_list[ZLIB].lower_level, codec_list[ZLIB].upper_level);
   printf("ZSTD\t\t %ld\t\t%ld\n", codec_list[ZSTD].lower_level, codec_list[ZSTD].upper_level);
   printf("BZIP2\t\t %ld\t\t%ld\n\n", codec_list[BZIP2].lower_level, codec_list[BZIP2].upper_level);
}

VOID *allocMem(UINTP size, INTP zeroInit)
{
    VOID *bufPtr = (zeroInit == 0) ? calloc(1, size) : malloc(size);
    return bufPtr;
}

INTP get_codec_method_level(CHAR *str, 
                          aocl_codec_bench_info *codec_bench_handle)
{
    CHAR *tok;
    CHAR *save = str;
    INTP i = 0;
    INTP match = 99;
 
    tok = strtok_r(save, ":", &save);
    tok = (tok != NULL) ? tok : str;
    
    while (i < AOCL_COMPRESSOR_ALGOS_NUM)
    {
        match = strcasecmp(tok, (codec_list[i].codec_name));
        if (match == 0)
        {
            codec_bench_handle->codec_method = i;
            break;
        }
        i++;
    }
    if (match)
    {
        return -1;
    }
    tok = strtok_r(save, ":", &save);
    if (tok)
    {
        codec_bench_handle->codec_level = atoi(tok);
        tok = strtok_r(save, ":", &save);
        if (tok)
        {
            codec_bench_handle->optVar = atoi(tok);
            tok = strtok_r(save, ":", &save);
            if (tok)
            {
                return -2;
            }
        }
    }
    
    return 0;
}

INTP read_user_options (INTP argc, 
                       CHAR **argv, 
                       aocl_codec_bench_info *codec_bench_handle)
{
    INTP cnt = 1;
    INTP fileIn = 0;
    INTP ret = 1;
    CHAR option;

    codec_bench_handle->enable_verbosity = 0;

    if (argc <= cnt)
    {
        print_user_options();
        return 2;
    }

    codec_bench_handle->use_all_codecs = 0;
    codec_bench_handle->mem_limit = MAX_MEM_SIZE_FOR_FILE_READ;
    codec_bench_handle->codec_method = -1;
    codec_bench_handle->codec_level = UNINIT_LEVEL;
    codec_bench_handle->iterations = BENCH_NUM_ITERS;
    codec_bench_handle->verify = 0;
    codec_bench_handle->print_stats = 0;
    codec_bench_handle->optVar = UNINIT_OPT_VAR;
    codec_bench_handle->inPtr = NULL;
    codec_bench_handle->compPtr = NULL;
    codec_bench_handle->decompPtr = NULL;
    codec_bench_handle->optOff = 0;
    codec_bench_handle->useIPP = 0;
    
    while (cnt < argc)
    {
        option = argv[cnt][0];
        if (option == '-')
        {
            switch (argv[cnt][1])
            {
                case 'h':
                    print_user_options();
                    ret = 2;
                break;

                case '-': 
                    if (!strcmp(&argv[cnt][2], "help"))
                    {
                        print_user_options();
                        ret = 2;
                    }
                    else
                        ret = -1;
                break;
                
                case 'l':
                    print_supported_compressors();
                    ret = 2;
                break;
                
                case 'a':
                    codec_bench_handle->use_all_codecs = 1;
                break;
                
                case 'm':
                    codec_bench_handle->mem_limit =
                        (atoi(&argv[cnt][2]))*1024*1024;//MBs to bytes
                break;
                
                case 'e':
                    if (get_codec_method_level(&argv[cnt][2],
                        codec_bench_handle) < 0)
                        ret = -2;
                break;
                
                case 'i':
                    codec_bench_handle->iterations = atoi(&argv[cnt][2]);
                break;
                
                case 't':
                    codec_bench_handle->verify = 1;
                break;
                
                case 'p':
                    codec_bench_handle->print_stats = 1;
                break;
                
                case 'v':
                    if (argv[cnt][2] != '\0')
                    {
                        codec_bench_handle->enable_verbosity =
                            atoi(&argv[cnt][2]);
                        if (codec_bench_handle->enable_verbosity > TRACE ||
                            codec_bench_handle->enable_verbosity < ERR)
                            codec_bench_handle->enable_verbosity = ERR;
                    }
                    else
                        codec_bench_handle->enable_verbosity = ERR;
                break;
#ifdef AOCL_DYNAMIC_DISPATCHER
                case 'o':
                    codec_bench_handle->optOff = 1;
                break;
#endif
                case 'c':
                    codec_bench_handle->useIPP = 1;
                break;
 
                default:
                    ret = -1;
                break;
            }
        }
        else
        {
            if (!fileIn)
            {
                CHAR *tmpStr;
                memcpy(inFile, argv[cnt], MAX_FILENAME_LEN);
#ifdef _WINDOWS
                tmpStr = strrchr(inFile, '\\');
#else
                tmpStr = strrchr(inFile, '/');
#endif
                codec_bench_handle->fName = tmpStr ? tmpStr+1 : inFile;
                
                fileIn = 1;
            }
            else
            {
                ret = 0;
            }
        }
        cnt++;
        if (ret < 0)
            break;
    }

    return ret;
}

UINTP compression_bound(UINTP inSize)
{
    UINTP outSize = (inSize + (inSize / 6) + MIN_PAD_SIZE);
    return outSize;
}

INTP init(aocl_codec_bench_info *codec_bench_handle,
          aocl_compression_desc *aocl_codec_handle)
{
    LOG(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    codec_bench_handle->inSize = 
         (codec_bench_handle->file_size > codec_bench_handle->mem_limit) ?
         codec_bench_handle->mem_limit : codec_bench_handle->file_size;
    codec_bench_handle->outSize = compression_bound(codec_bench_handle->inSize);
    codec_bench_handle->inPtr = 
        (char *)allocMem(codec_bench_handle->inSize, 0);
    codec_bench_handle->compPtr = 
        (char *)allocMem(codec_bench_handle->outSize, 0);
    codec_bench_handle->decompPtr = 
        (char *)allocMem(codec_bench_handle->inSize, 0);
    
    codec_bench_handle->cTime = 0;
    codec_bench_handle->cSize = 0;
    codec_bench_handle->cSpeed = 0.0;
    codec_bench_handle->dTime = 0;
    codec_bench_handle->dSize = 0;
    codec_bench_handle->dSpeed = 0.0;

    aocl_codec_handle->level = codec_bench_handle->codec_level;
    aocl_codec_handle->optVar = codec_bench_handle->optVar;
    aocl_codec_handle->numThreads = 1;
    aocl_codec_handle->numMPIranks = 0;
    aocl_codec_handle->measureStats = codec_bench_handle->print_stats;
    aocl_codec_handle->workBuf = NULL;
    aocl_codec_handle->optOff = codec_bench_handle->optOff;

    LOG(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    if (!codec_bench_handle->inPtr || !codec_bench_handle->compPtr ||
        !codec_bench_handle->decompPtr)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

INTP aocl_bench_run(aocl_compression_desc *aocl_codec_handle,
                   aocl_codec_bench_info *codec_bench_handle)
{
    INT64 resultComp = 0;
    INT64 resultDecomp = 0;
    UINTP inSize, file_size;
    aocl_compression_type i; 
    INTP j, k, l;
    INTP status = 0, retStatus = 0;
    FILE *inFp = codec_bench_handle->fp;

    LOG(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    aocl_codec_handle->memLimit = codec_bench_handle->mem_limit;

    if (codec_bench_handle->print_stats == 0 &&
        codec_bench_handle->verify == 1)
        codec_bench_handle->iterations = 1;

    if (codec_bench_handle->codec_method < 0)
    {
        for (i = LZ4; i < AOCL_COMPRESSOR_ALGOS_NUM; i++)
        {
            aocl_codec_handle->optVar = codec_list[i].extra_param;
            for (l = codec_list[i].lower_level;
                 l <= codec_list[i].upper_level; l++)
            {
                codec_bench_handle->cTime = 0;
                codec_bench_handle->cSize = 0;
                codec_bench_handle->dTime = 0;
                codec_bench_handle->dSize = 0;
                codec_bench_handle->cBestTime = 0;
                codec_bench_handle->dBestTime = 0;
                aocl_codec_handle->level = l;
                status = 0;
                
                //setup the codec method
                aocl_llc_setup(aocl_codec_handle, i);

                for (k = 0; k < codec_bench_handle->iterations; k++)
                {
                    inSize = codec_bench_handle->inSize;
                    file_size = codec_bench_handle->file_size;
                    while (inSize)
                    {
                        inSize = fread(codec_bench_handle->inPtr, 1,
                                       inSize, inFp);

                        //compress
                        aocl_codec_handle->inSize = inSize;
                        aocl_codec_handle->outSize = codec_bench_handle->outSize;
                        aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
                        aocl_codec_handle->outBuf = codec_bench_handle->compPtr;
                        resultComp = aocl_llc_compress(aocl_codec_handle, i);
                        if (resultComp <= 0)
                        {
                            printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] Compression: failed\n",
                                codec_list[i].codec_name,
                                l, codec_bench_handle->fName);
                            status = -1;
                            break;
                        }

                        //decompress
                        aocl_codec_handle->inSize = resultComp;
                        aocl_codec_handle->outSize = inSize;
                        aocl_codec_handle->inBuf = codec_bench_handle->compPtr;
                        aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
                        resultDecomp = aocl_llc_decompress(aocl_codec_handle,
                                                             i);
                        if (resultDecomp <= 0)
                        {
                            printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] Decompression: failed\n",
                                codec_list[i].codec_name,
                                l, codec_bench_handle->fName);
                            status = -1;
                            break;
                        }

                        if (codec_bench_handle->verify)
                        {
                            j = memcmp(codec_bench_handle->inPtr,
                                       codec_bench_handle->decompPtr, inSize);
                            if (j != 0)
                            {
                                printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] verification: failed\n",
                                    codec_list[i].codec_name,
                                    l, codec_bench_handle->fName);
                                status = -1;
                                break;
                            }
                        }
                        if (codec_bench_handle->print_stats)
                        {
                            codec_bench_handle->cTime +=
                                aocl_codec_handle->cTime;
                            codec_bench_handle->cSize +=
                                aocl_codec_handle->cSize;
                            codec_bench_handle->dTime +=
                                aocl_codec_handle->dTime;
                            codec_bench_handle->dSize +=
                                aocl_codec_handle->dSize;
                            codec_bench_handle->cBestTime = 
                                (codec_bench_handle->cBestTime == 0) ? 
                                aocl_codec_handle->cTime :
                                (codec_bench_handle->cBestTime > 
                                aocl_codec_handle->cTime) ?
                                aocl_codec_handle->cTime : 
                                codec_bench_handle->cBestTime;
                            codec_bench_handle->dBestTime = 
                                (codec_bench_handle->dBestTime == 0) ? 
                            aocl_codec_handle->dTime :
                                (codec_bench_handle->dBestTime > 
                                aocl_codec_handle->dTime) ?
                                aocl_codec_handle->dTime : 
                                codec_bench_handle->dBestTime;
                        }
                        file_size -= inSize;
                        inSize = (file_size > inSize) ? inSize : file_size;
                    }
                    rewind(inFp);
                    if (status != 0)
                        break;
                }

                //destroy the codec method
                aocl_llc_destroy(aocl_codec_handle, i);

                if (status != 0)
                {
                    retStatus = status;
                    continue;
                }

                if (codec_bench_handle->verify)
                {
                    printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] verification: passed\n",
                        codec_list[i].codec_name,
                        l, codec_bench_handle->fName);

                }
                if (codec_bench_handle->print_stats)
                {
                    codec_bench_handle->cSpeed = (codec_bench_handle->inSize * 
                        codec_bench_handle->iterations * 1000.0) / 
                        codec_bench_handle->cTime;
                    codec_bench_handle->dSpeed = (codec_bench_handle->inSize * 
                        codec_bench_handle->iterations * 1000.0) / 
                        codec_bench_handle->dTime;
                    codec_bench_handle->cBestSpeed = 
                        (codec_bench_handle->inSize * 1000.0) / 
                        codec_bench_handle->cBestTime;
                    codec_bench_handle->dBestSpeed = 
                        (codec_bench_handle->inSize * 1000.0) / 
                        codec_bench_handle->dBestTime;
                    printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] -------------------------------------\n",
                        codec_list[i].codec_name,
                        l, codec_bench_handle->fName);
                    printf("Compression:         speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                        codec_bench_handle->cSpeed,
                        codec_bench_handle->cTime/
                        (codec_bench_handle->iterations * 1000000.0),
                        codec_bench_handle->cSize/codec_bench_handle->iterations,
                        codec_bench_handle->cBestSpeed,
                        codec_bench_handle->cBestTime/1000000.0);
                    printf("Decompression:       speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n", 
                        codec_bench_handle->dSpeed,
                        codec_bench_handle->dTime/
                        (codec_bench_handle->iterations * 1000000.0),
                        codec_bench_handle->dSize/codec_bench_handle->iterations,
                        codec_bench_handle->dBestSpeed,
                        codec_bench_handle->dBestTime/1000000.0);
                    printf("Ratio:               %.2f\n",
                        (((codec_bench_handle->cSize*100.0)/
                        codec_bench_handle->iterations)/
                        codec_bench_handle->inSize));
                }
            }
            
        }
    }
    else
    {
        INTP def_level = aocl_codec_handle->level;
        INTP lower_level, upper_level;

        if (def_level == UNINIT_LEVEL)
        {
            lower_level = 
                codec_list[codec_bench_handle->codec_method].lower_level;
            upper_level = 
                codec_list[codec_bench_handle->codec_method].upper_level;
        }
        else
        {
            lower_level = upper_level = def_level;
        }
        if (aocl_codec_handle->optVar == UNINIT_OPT_VAR)
            aocl_codec_handle->optVar = 
                codec_list[codec_bench_handle->codec_method].extra_param;
                
        for (l = lower_level; l <= upper_level; l++)
        {
            codec_bench_handle->cTime = 0;
            codec_bench_handle->cSize = 0;
            codec_bench_handle->dTime = 0;
            codec_bench_handle->dSize = 0;
            codec_bench_handle->cBestTime = 0;
            codec_bench_handle->dBestTime = 0;
            aocl_codec_handle->level = l;
            status = 0;

            //setup the codec method
            aocl_llc_setup(aocl_codec_handle, codec_bench_handle->codec_method);
        
            for (k = 0; k < codec_bench_handle->iterations; k++)
            {
                inSize = codec_bench_handle->inSize;
                file_size = codec_bench_handle->file_size;
                while (inSize)
                {
                    inSize = fread(codec_bench_handle->inPtr, 1, inSize, inFp);

                    //compress
                    aocl_codec_handle->inSize = inSize;
                    aocl_codec_handle->outSize = codec_bench_handle->outSize;
                    aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
                    aocl_codec_handle->outBuf = codec_bench_handle->compPtr;
                    resultComp = aocl_llc_compress(aocl_codec_handle,
                                            codec_bench_handle->codec_method);
                    if (resultComp <= 0)
                    {
                        printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] Compression: failed\n",
                            codec_list[codec_bench_handle->codec_method].codec_name,
                            aocl_codec_handle->level, codec_bench_handle->fName);
                        status = -1;
                        break;
                    }

                    //decompress
                    aocl_codec_handle->inSize = resultComp;
                    aocl_codec_handle->outSize = inSize;
                    aocl_codec_handle->inBuf = codec_bench_handle->compPtr;
                    aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;
                    resultDecomp = aocl_llc_decompress(aocl_codec_handle,
                                        codec_bench_handle->codec_method);
                    if (resultDecomp <= 0)
                    {
                        printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] Decompression: failed\n",
                            codec_list[codec_bench_handle->codec_method].codec_name,
                            aocl_codec_handle->level, codec_bench_handle->fName);
                        status = -1;
                        break;
                    }

                    if (codec_bench_handle->verify)
                    {
                        j = memcmp(codec_bench_handle->inPtr,
                                codec_bench_handle->decompPtr, inSize);
                        if (j != 0)
                        {
                            printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] verification: failed\n",
                                codec_list[codec_bench_handle->codec_method].
                                codec_name,
                                aocl_codec_handle->level,
                                codec_bench_handle->fName);
                            status = -1;
                            break;
                        }
                    }
                    if (codec_bench_handle->print_stats)
                    {
                        codec_bench_handle->cTime += aocl_codec_handle->cTime;
                        codec_bench_handle->cSize += aocl_codec_handle->cSize;
                        codec_bench_handle->dTime += aocl_codec_handle->dTime;
                        codec_bench_handle->dSize += aocl_codec_handle->dSize;
                        codec_bench_handle->cBestTime = 
                            (codec_bench_handle->cBestTime == 0) ? 
                            aocl_codec_handle->cTime : 
                            (codec_bench_handle->cBestTime > 
                            aocl_codec_handle->cTime) ?
                            aocl_codec_handle->cTime : 
                            codec_bench_handle->cBestTime;
                        codec_bench_handle->dBestTime = 
                            (codec_bench_handle->dBestTime == 0) ? 
                            aocl_codec_handle->dTime :
                            (codec_bench_handle->dBestTime > 
                            aocl_codec_handle->dTime) ?
                            aocl_codec_handle->dTime : 
                            codec_bench_handle->dBestTime;
                    }
                    file_size -= inSize;
                    inSize = (file_size > inSize) ? inSize : file_size;
                }
                rewind(inFp);
                if (status != 0)
                    break;
            }

            //destroy the codec method
            aocl_llc_destroy(aocl_codec_handle,
                               codec_bench_handle->codec_method);

            if (status != 0)
            {
                retStatus = status;
                continue;
            }

            if (codec_bench_handle->verify)
            {
                printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] verification: passed\n", 
                    codec_list[codec_bench_handle->codec_method].codec_name,
                    aocl_codec_handle->level,
                    codec_bench_handle->fName);
    
            }
            if (codec_bench_handle->print_stats)
            {        
                codec_bench_handle->cSpeed = (codec_bench_handle->inSize * 
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->cTime;
                codec_bench_handle->dSpeed = (codec_bench_handle->inSize * 
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->dTime;
                codec_bench_handle->cBestSpeed = (codec_bench_handle->inSize * 
                    1000.0) / codec_bench_handle->cBestTime;
                codec_bench_handle->dBestSpeed = (codec_bench_handle->inSize * 
                    1000.0) / codec_bench_handle->dBestTime;
                printf("AOCL-COMPRESSION [%s-%ld] [Filename:%s] -------------------------------------\n",
                        codec_list[codec_bench_handle->codec_method].codec_name,
                        aocl_codec_handle->level, codec_bench_handle->fName);
                printf("Compression:         speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                        codec_bench_handle->cSpeed,
                        codec_bench_handle->cTime/
                        (codec_bench_handle->iterations * 1000000.0),
                        codec_bench_handle->cSize/codec_bench_handle->iterations,
                        codec_bench_handle->cBestSpeed,
                        codec_bench_handle->cBestTime/1000000.0);
                printf("Decompression:       speed(avg) %.2f MB/s, time(avg) %.2f ms, size %ld, speed(best) %.2f MB/s, time(best) %.2f ms\n", 
                        codec_bench_handle->dSpeed,
                        codec_bench_handle->dTime/
                        (codec_bench_handle->iterations * 1000000.0),
                        codec_bench_handle->dSize/codec_bench_handle->iterations,
                        codec_bench_handle->dBestSpeed,
                        codec_bench_handle->dBestTime/1000000.0);
                printf("Ratio:               %.2f\n",
                        (((codec_bench_handle->cSize*100.0)/
                        codec_bench_handle->iterations)/
                        codec_bench_handle->inSize));
            }
        }
    }
    LOG(TRACE, aocl_codec_handle->printDebugLogs, "Exit");

    return retStatus;
}

void destroy(aocl_codec_bench_info *codec_bench_handle)
{
    LOG(TRACE, codec_bench_handle->enable_verbosity, "Enter");

    if (codec_bench_handle->inPtr)
        free(codec_bench_handle->inPtr);
    if (codec_bench_handle->compPtr)
        free(codec_bench_handle->compPtr);
    if (codec_bench_handle->decompPtr)
        free(codec_bench_handle->decompPtr);

    LOG(TRACE, codec_bench_handle->enable_verbosity, "Exit");
}
    
INT32 main (INT32 argc, CHAR **argv)
{
    aocl_codec_bench_info codec_bench_handle;
    aocl_compression_desc aocl_codec_ds;
    aocl_compression_desc *aocl_codec_handle = &aocl_codec_ds;
    FILE *inFp = NULL;
    INTP result;
    INTP ret;

    ret = read_user_options (argc, argv, &codec_bench_handle);

#ifdef AOCL_CL_DTL
    aocl_codec_handle->printDebugLogs = codec_bench_handle.enable_verbosity;
#else
    aocl_codec_handle->printDebugLogs = 0;
#endif

    LOG(TRACE, aocl_codec_handle->printDebugLogs, "Enter");

    if (ret == 0)
    {
        LOG(ERR, aocl_codec_handle->printDebugLogs,
        "Invalid option passed, ignoring more than one input file. Use -h to know supported user options.");
    }
    else if (ret == -1)
    {
        LOG(ERR, aocl_codec_handle->printDebugLogs,
        "Invalid option passed. Use -h to know supported user options.");
	printf("Invalid option passed. Use -h to know supported user options.\n");
        goto exit;
    }
    else if (ret == -2)
    {
        LOG(ERR, aocl_codec_handle->printDebugLogs,
        "Unsupported compression method or level specified. Use -l to know supported methods and -h for exact user options.");
        printf("Unsupported option passed. Use -l to know supported methods and -h for exact user options.\n");
		goto exit;
    }
    else if (ret == 2)
        goto exit;

    if (!(inFp = fopen(inFile, "rb")))
    {
        LOG(ERR, aocl_codec_handle->printDebugLogs,
        "Error in opening input file [%s].", inFile);
        goto exit;
    }
    fseek(inFp, 0L, SEEK_END);
    codec_bench_handle.file_size = ftell(inFp);
    rewind(inFp);

    if (init(&codec_bench_handle, aocl_codec_handle) < 0)
    {
        LOG(ERR, aocl_codec_handle->printDebugLogs,
        "Error in allocating memory.");
		goto exit;
    }
    codec_bench_handle.fp = inFp;

    if (codec_bench_handle.useIPP)
#ifdef _WINDOWS
        LOG(ERR, aocl_codec_handle->printDebugLogs,
            "IPP test execution not supported on Windows for now.");
#else
        result = ipp_bench_run(aocl_codec_handle, &codec_bench_handle);
#endif
    else
        result = aocl_bench_run(aocl_codec_handle, &codec_bench_handle);
    if (result != 0)
    {
        LOG(ERR, aocl_codec_handle->printDebugLogs,
        "Error in compression and decompression operations.");
    }

    destroy(&codec_bench_handle);

exit:
    if (inFp)
        fclose(inFp);
    LOG(TRACE, aocl_codec_handle->printDebugLogs, "Exit");
    return result;
}
