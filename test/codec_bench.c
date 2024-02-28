/**
 * Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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
#include "api/aocl_compression.h"
#include "api/aocl_threads.h"
#include "codec_bench.h"
#include <sys/stat.h>

#ifndef _WINDOWS
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif

#ifdef _WINDOWS
#define CODEC_STRCAT(name) strcat(name, "\\")
#else
#define CODEC_STRCAT(name) strcat(name, "/")
#endif

#ifdef _WINDOWS
#define LOG_SYSTEM_ERROR()\
{\
    LOG_FORMATTED(ERR, log_ctx,"System Error Code: %lu", GetLastError());\
    printf("System Error Code: %lu\n", GetLastError());\
} 
#else
#define LOG_SYSTEM_ERROR()\
{\
    LOG_FORMATTED(ERR, log_ctx,"Error Message: %s.", strerror(errno));\
    printf("Error Message: %s.\n", strerror(errno));\
}
#endif

#define STRING_OVERFLOW(src_sz, append_sz, limit) ((src_sz + append_sz)  > (limit - 1))
#define LOG_STRING_OVERFLOW(text_overflowed)\
{\
    LOG_FORMATTED(ERR, log_ctx,"String Overflow Error: [%s] is longer than [%d] characters.", text_overflowed, (int)(MAX_FILENAME_LEN-1));\
    printf("String Overflow Error: [%s] is longer than [%d] characters.\n", text_overflowed, (int)(MAX_FILENAME_LEN-1));\
}

//Input test file name
AOCL_CHAR inFile[MAX_FILENAME_LEN];
AOCL_CHAR inFolder[MAX_FILENAME_LEN];
AOCL_CHAR dumpFile[MAX_FILENAME_LEN];
AOCL_INTP dumpEnabled = 0;
AOCL_CHAR valFile[MAX_FILENAME_LEN];
AOCL_INTP valEnabled = 0;
AOCL_INTP isFolder = 0;

AOCL_VOID print_user_options (AOCL_VOID)
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
    printf("-o          Turn off all optimizations\n");
    printf("-r          Run particular operation only. Supported options are 'compress' and 'decompress'.\n\n");
    printf("-d          File to dump output data. Based on -r, saves compressed/decompressed data.\n\n");
    printf("-f          Input uncompressed file to be used for validation in -rdecompress mode.\n\n");
    printf("-c          Run IPP library methods. Provide the path for the IPP library path after the -c option.\n\n");
    printf("-n          Use Native APIs for compression/decompression.\n\n");
}

AOCL_VOID print_supported_compressors (AOCL_VOID)
{
   printf("\nSupported compression/decompression methods along with their supported levels are:\n\n");
   printf("===========================================\n");
   printf("Method Name\tLower Level\tUpper Level\n");
   printf("===========================================\n");
   printf("LZ4\t\t %s\t\t%s\n", "NA", "NA");
   printf("LZ4HC\t\t %td\t\t%td\n", codec_list[LZ4HC].lower_level, codec_list[LZ4HC].upper_level);
   printf("LZMA\t\t %td\t\t%td\n", codec_list[LZMA].lower_level, codec_list[LZMA].upper_level);
   printf("SNAPPY\t\t %s\t\t%s\n", "NA", "NA");
   printf("ZLIB\t\t %td\t\t%td\n", codec_list[ZLIB].lower_level, codec_list[ZLIB].upper_level);
   printf("ZSTD\t\t %td\t\t%td\n", codec_list[ZSTD].lower_level, codec_list[ZSTD].upper_level);
   printf("BZIP2\t\t %td\t\t%td\n\n", codec_list[BZIP2].lower_level, codec_list[BZIP2].upper_level);
}

AOCL_VOID *allocMem(AOCL_UINTP size, AOCL_INTP zeroInit)
{
    AOCL_VOID *bufPtr = (zeroInit == 0) ? calloc(1, size) : malloc(size);
    return bufPtr;
}

AOCL_INTP get_codec_method_level(AOCL_CHAR *str, 
                          aocl_codec_bench_info *codec_bench_handle)
{
    AOCL_CHAR *tok;
    AOCL_CHAR *save = str;
    AOCL_INTP i = 0;
    AOCL_INTP match = 99;
 
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
        if ((codec_bench_handle->codec_level < 
             codec_list[codec_bench_handle->codec_method].lower_level) ||
            (codec_bench_handle->codec_level > 
             codec_list[codec_bench_handle->codec_method].upper_level))
            return -2;
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


AOCL_INTP read_file_name(AOCL_CHAR* dst, const AOCL_CHAR* src) {
    AOCL_UINTP src_sz = strlen(src);
    if (src_sz > (MAX_FILENAME_LEN - 1)) { 
        return ERR_CODEC_BENCH_ARGS;
    }
    else {
        strcpy(dst, src);
    }
    return 0;
}

AOCL_VOID append_file_name_ext(AOCL_CHAR* dst, const AOCL_CHAR* ext) {
    AOCL_UINTP cur_sz = strlen(dst);
    AOCL_UINTP ext_sz = strlen(ext);
    if ((cur_sz + ext_sz) > (MAX_FILENAME_LEN - 1)) {
        // truncate. Else it will overflow
        strcpy(dst + (MAX_FILENAME_LEN - 1 - ext_sz), ext);
    }
    else {
        strcpy(dst + cur_sz, ext);
    }
}

AOCL_INTP get_file_name(aocl_codec_bench_info* codec_bench_handle, AOCL_CHAR* dmpFile, AOCL_INT32 file_cnt){

    strcpy(dmpFile, codec_bench_handle->fName);
    
    AOCL_CHAR file_cnt_str[MAX_FILENAME_LEN];
    sprintf(file_cnt_str, "_%d", file_cnt);
    
    // check for string overflow
    AOCL_UINTP append_len = strlen(file_cnt_str) + strlen(codec_list[codec_bench_handle->codec_method].extension);
    if(STRING_OVERFLOW(strlen(dmpFile), append_len, MAX_FILENAME_LEN)) {
        return ERR_CODEC_BENCH_ARGS;
    }

    //append count number
    append_file_name_ext(dmpFile, file_cnt_str);

    // append extension
    append_file_name_ext(dmpFile, codec_list[codec_bench_handle->codec_method].extension);
    return 0;
}

AOCL_INTP is_dir(const AOCL_CHAR* filename)
{

#ifdef _WINDOWS
    unsigned long file_attributes = GetFileAttributes(filename);
	return (AOCL_INTP)((file_attributes != INVALID_FILE_ATTRIBUTES) && (file_attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat path;
    AOCL_INTP ret = stat(filename, &path);
    // The macro S_ISDIR returns a non-zero integer if given file is a directory, 
    // else returns 0.
    return (AOCL_INTP)((ret == 0) && (S_ISDIR(path.st_mode)!=0));
#endif

}

AOCL_INTP file_object_exists(const AOCL_CHAR* dirName)
{

#ifdef _WINDOWS
    unsigned long file_attributes = GetFileAttributes(dirName);
	return (AOCL_INTP)(file_attributes != INVALID_FILE_ATTRIBUTES);

#else
    struct stat path;
    if (stat(dirName, &path) == 0){
        // file object named `dirName` exists
        return 1;
    }
    return 0;
#endif

}

AOCL_INTP get_file_count(const AOCL_CHAR* dirName)
{
    AOCL_INTP cnt = 0;

#ifdef _WINDOWS
	AOCL_CHAR dir [MAX_FILENAME_LEN];
    memset(dir, '\0', sizeof(dir));
    sprintf(dir, "%s\\*", dirName);

	WIN32_FIND_DATA find_data;
	HANDLE searchHandle = FindFirstFile(dir, &find_data);

	if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
        cnt += 1;
	}

	while(FindNextFile(searchHandle, &find_data)!=0){
		if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
            cnt += 1;
		}
	}

	FindClose(searchHandle);
#else

    DIR *dir_p;
    struct dirent *dir;
    dir_p = opendir(dirName);
    if (dir_p)
    {
        while ((dir = readdir(dir_p)) != NULL)
        {
            if(dir->d_type == DT_REG){  
                // [excludes . and ..]
                cnt += 1;
            }
        }
        closedir(dir_p);
    }
#endif

    return cnt;
}
AOCL_INTP read_user_options (AOCL_INTP argc, 
                       AOCL_CHAR **argv, 
                       aocl_codec_bench_info *codec_bench_handle)
{
    AOCL_INTP cnt = 1;
    AOCL_INTP fileIn = 0;
    AOCL_INTP ret = 1;
    AOCL_CHAR option;

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    if (argc <= cnt)
    {
        print_user_options();
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
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
    codec_bench_handle->outPtr = NULL;
    codec_bench_handle->decompPtr = NULL;
    codec_bench_handle->optOff = 0;
    codec_bench_handle->useIPP = 0;
    codec_bench_handle->useNAPI = 0;
    codec_bench_handle->dumpFp = NULL;
    codec_bench_handle->dumpFile = NULL;
    codec_bench_handle->valFp = NULL;
    codec_bench_handle->runOperation = RUN_OPERATION_DEFAULT;
    
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
                        ret = ERR_CODEC_BENCH_ARGS;
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
                        ret = ERR_CODEC_BENCH_METHOD;
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

                case 'n':
                    codec_bench_handle->useNAPI = 1;
                break;
                case 'o':
                    codec_bench_handle->optOff = 1;
                break;
                case 'c':
                    codec_bench_handle->useIPP = 1;
                    codec_bench_handle->ippDir = &argv[cnt][2];
                break;

                case 'r': /* Select operation to run. If not specified, run both. */
                    if (codec_bench_handle->runOperation != RUN_OPERATION_DEFAULT) 
                    { //r option was already processed once
                        printf("Multiple -r options are not allowed.\n\n");
                        ret = ERR_CODEC_BENCH_ARGS;
                        break;
                    }
                    if (strcasecmp(&argv[cnt][2], "compress") == 0) 
                    {
                        codec_bench_handle->runOperation = RUN_OPERATION_COMPRESS;
                    }
                    else if (strcasecmp(&argv[cnt][2], "decompress") == 0) 
                    {
                        codec_bench_handle->runOperation = RUN_OPERATION_DECOMPRESS;
                    }
                    else 
                    {
                        printf("Invalid -r option. Valid options are 'compress' and 'decompress'\n\n");
                        ret = ERR_CODEC_BENCH_ARGS;
                    }
                    break;

                case 'd': /* file to dump output data */
                    if (dumpEnabled) 
                    {
                        printf("Multiple -d options are not allowed.\n\n");
                        ret = ERR_CODEC_BENCH_ARGS;
                        break;
                    }
                    if (read_file_name(dumpFile, &argv[cnt][2]) != 0){
                        LOG_STRING_OVERFLOW(&argv[cnt][2]);
                        ret = ERR_CODEC_BENCH_ARGS;
                    }
                    dumpEnabled = 1;
                    break;

                case 'f': /* file for validation in decompress only mode */
                    if (valEnabled) 
                    {
                        printf("Multiple -f options are not allowed.\n\n");
                        ret = ERR_CODEC_BENCH_ARGS;
                        break;
                    }
                    if (read_file_name(valFile, &argv[cnt][2]) != 0) {
                        LOG_STRING_OVERFLOW(&argv[cnt][2]);
                        ret = ERR_CODEC_BENCH_ARGS;
                    }
                    valEnabled = 1;
                    break;
 
                default:
                    ret = ERR_CODEC_BENCH_ARGS;
                break;
            }
        }
        else
        {
            if (!fileIn)
            {
                AOCL_CHAR *tmpStr;
                if (read_file_name(inFile, argv[cnt]) != 0) {
                    LOG_STRING_OVERFLOW(argv[cnt]);
                    ret = ERR_CODEC_BENCH_ARGS;
                }
#ifdef _WINDOWS
                tmpStr = strrchr(inFile, '\\');
#else
                tmpStr = strrchr(inFile, '/');
#endif
                codec_bench_handle->fName = tmpStr ? tmpStr+1 : inFile;

                if(is_dir(inFile)) {
                    strcpy(inFolder, inFile);
                    isFolder = 1;
                }
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

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
    return ret;
}

AOCL_UINTP compression_bound(AOCL_UINTP inSize)
{
    AOCL_UINTP outSize = (inSize + (inSize / 6) + MIN_PAD_SIZE);
#ifdef AOCL_ENABLE_THREADS
    outSize += aocl_get_rap_frame_bound_mt();
#endif
    return outSize;
}

AOCL_INTP init(aocl_codec_bench_info *codec_bench_handle,
          aocl_compression_desc *aocl_codec_handle)
{
    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");


    if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
        codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS) 
    {
        codec_bench_handle->inSize = 
            (codec_bench_handle->file_size > codec_bench_handle->mem_limit) ?
            codec_bench_handle->mem_limit : codec_bench_handle->file_size;
        codec_bench_handle->inPtr =
            (AOCL_CHAR*)allocMem(codec_bench_handle->inSize, 0);
        codec_bench_handle->outSize = compression_bound(codec_bench_handle->inSize);
        codec_bench_handle->outPtr =
            (AOCL_CHAR*)allocMem(codec_bench_handle->outSize, 0); // ptr to hold compressed data
        codec_bench_handle->decompPtr =
            (AOCL_CHAR*)allocMem(codec_bench_handle->inSize, 0); // get size of decompressed data from input
    }
    else 
    { // codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS

        if (codec_bench_handle->file_size > compression_bound(codec_bench_handle->mem_limit))
        {
            LOG_UNFORMATTED(ERR, log_ctx, 
                "Cannot decompress this large file.");
            printf("Cannot decompress this large file.\n");
            return ERR_CODEC_BENCH_MEM;

        }
        codec_bench_handle->inSize = codec_bench_handle->file_size;
        codec_bench_handle->inPtr =
            (AOCL_CHAR*)allocMem(codec_bench_handle->inSize, 0);
        codec_bench_handle->outSize = codec_bench_handle->mem_limit;
        codec_bench_handle->outPtr =
            (AOCL_CHAR*)allocMem(codec_bench_handle->outSize, 0); // ptr to hold decompressed data
        codec_bench_handle->decompPtr =
            (AOCL_CHAR*)allocMem(codec_bench_handle->outSize, 0); // get size of decompressed data from output
    }

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


    if (!codec_bench_handle->inPtr || !codec_bench_handle->outPtr ||
        !codec_bench_handle->decompPtr)
    {
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return -1;
    }
    else
    {
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return 0;
    }
}

AOCL_INTP open_file(aocl_codec_bench_info* codec_bench_handle, 
                    const AOCL_CHAR* filePath, const AOCL_CHAR* filename)
{
    codec_bench_handle->fp = fopen(filePath, "rb");
    if (!codec_bench_handle->fp)
    {
        LOG_FORMATTED(ERR, log_ctx,
            "Error in opening input file [%s].", filename);
        printf("Error in opening input file [%s].\n", filename);

        return 0;
    }

#ifdef _WINDOWS
    _fseeki64(codec_bench_handle->fp, 0L, SEEK_END);
    codec_bench_handle->file_size = _ftelli64(codec_bench_handle->fp);
#else
    fseek(codec_bench_handle->fp, 0L, SEEK_END);
    codec_bench_handle->file_size = ftell(codec_bench_handle->fp);
#endif
    rewind(codec_bench_handle->fp);

    return 1;
}

AOCL_INTP open_file_in_folder(aocl_codec_bench_info* codec_bench_handle,
    AOCL_INT32 file_number)
{
    AOCL_CHAR filename[MAX_FILENAME_LEN];
    AOCL_CHAR filePath[MAX_FILENAME_LEN];

    // Copy folder name to filePath
    strcpy(filePath, inFolder);
    // Store file name in 'filename'
    if (get_file_name(codec_bench_handle, filename, file_number) != 0){
        LOG_STRING_OVERFLOW("Compressed input file path");
        return ERR_CODEC_BENCH_ARGS;
    }

    // check for string overflow
    AOCL_UINTP append_len = 1 + strlen(filename);
    if (STRING_OVERFLOW(strlen(filePath), append_len, MAX_FILENAME_LEN)){
        LOG_STRING_OVERFLOW("Compressed input file path");
        return ERR_CODEC_BENCH_ARGS;
    }

    CODEC_STRCAT(filePath);
    strcat(filePath, filename);

    if (!open_file(codec_bench_handle, filePath, filename))
    {
        return ERR_CODEC_BENCH_FILE_IO;
    }

    codec_bench_handle->inSize = codec_bench_handle->file_size;
    codec_bench_handle->outSize = codec_bench_handle->mem_limit;
    codec_bench_handle->inPtr = (AOCL_CHAR*)allocMem(codec_bench_handle->file_size, 0);
    codec_bench_handle->outPtr = (AOCL_CHAR*)allocMem(codec_bench_handle->outSize, 0);

    return 0;
}

AOCL_VOID close_file(aocl_codec_bench_info* codec_bench_handle) {
    if (codec_bench_handle->fp) {
        fclose(codec_bench_handle->fp);
        codec_bench_handle->fp = NULL;
    }
    codec_bench_handle->file_size = 0;
}

AOCL_VOID close_file_in_folder(aocl_codec_bench_info* codec_bench_handle)
{
    close_file(codec_bench_handle);
    
    // free the mem allocated before
    if (codec_bench_handle->inPtr) {
        free(codec_bench_handle->inPtr);
        codec_bench_handle->inPtr = NULL;
    }

    if (codec_bench_handle->outPtr) {
        free(codec_bench_handle->outPtr);
        codec_bench_handle->outPtr = NULL;
    }

}

AOCL_INTP dump_to_file(aocl_codec_bench_info* codec_bench_handle,
                aocl_compression_type codec, AOCL_INTP level,
                AOCL_INT32 file_cnt, const AOCL_CHAR* buffer, AOCL_UINTP size) {

    FILE* dumpFp = NULL;
    AOCL_CHAR dmpFile[MAX_FILENAME_LEN];
    AOCL_CHAR dmpFolder[MAX_FILENAME_LEN];

    strcpy(dmpFile, codec_bench_handle->dumpFile);
    strcpy(dmpFolder, codec_bench_handle->dumpFile);

    AOCL_CHAR file_cnt_str[MAX_FILENAME_LEN];
    sprintf(file_cnt_str, "_%d", file_cnt);

    // check for string overflow
    AOCL_UINTP append_len = strlen(file_cnt_str) + strlen(codec_list[codec_bench_handle->codec_method].extension) + 1 + strlen(dmpFile);
    if (STRING_OVERFLOW(strlen(dmpFolder), append_len, MAX_FILENAME_LEN)){
        LOG_STRING_OVERFLOW("Dump file path");
        return ERR_CODEC_BENCH_ARGS;
    }

    //append count number
    append_file_name_ext(dmpFile, file_cnt_str);
    // append extension
    append_file_name_ext(dmpFile, codec_list[codec_bench_handle->codec_method].extension);

    CODEC_STRCAT(dmpFolder);
    strcat(dmpFolder, dmpFile);
    // open file to dump compressed data
    if (!(dumpFp = fopen(dmpFolder, "wb")))
    {
        LOG_FORMATTED(ERR, log_ctx,
            "Error in opening output file [%s].", dmpFolder);
        printf("Error in opening output file [%s].\n", dmpFolder);
        return ERR_CODEC_BENCH_FILE_IO;
    }

    // dump compressed data to file
    AOCL_UINTP written = fwrite(buffer, sizeof(AOCL_CHAR), size, dumpFp);
    if (written < size)
    {
        printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] Dump: failed\n",
            codec_list[codec].codec_name,
            level, dmpFolder);
        if (dumpFp) {
            fclose(dumpFp);
            dumpFp = NULL;
        }
        return ERR_CODEC_BENCH_FILE_IO;
    }

    if (dumpFp) {
        fclose(dumpFp);
        dumpFp = NULL;
    }

    return 0;
}

AOCL_INTP create_dump_file(aocl_codec_bench_info* codec_bench_handle, FILE** dumpFp)
{
    // append extension
    if (codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS) {
        append_file_name_ext(codec_bench_handle->dumpFile, ".decomp");
    }
    else {
        append_file_name_ext(codec_bench_handle->dumpFile, codec_list[codec_bench_handle->codec_method].extension);
    }

    // create file
    if (!(*dumpFp = fopen(codec_bench_handle->dumpFile, "wb"))){
                
        LOG_FORMATTED(ERR, log_ctx,
                "Error in opening output file [%s].", codec_bench_handle->dumpFile);
        printf("Error in opening output file [%s].\n", codec_bench_handle->dumpFile);
        return ERR_CODEC_BENCH_FILE_IO;        
    }
    codec_bench_handle->dumpFp = *dumpFp;
    return 0;
}

AOCL_INTP create_dump_folder(aocl_codec_bench_info* codec_bench_handle)
{
    if(file_object_exists(codec_bench_handle->dumpFile))
    {
        LOG_FORMATTED(ERR, log_ctx,
                "Error, file object named [%s] already exists. Please give a different name for dump folder.", codec_bench_handle->dumpFile);
        printf("Error, file object named [%s] already exists. Please give a different name for dump folder.\n", codec_bench_handle->dumpFile);
        return ERR_CODEC_BENCH_FILE_IO;
    }
    // create dump folder with name codec_bench_handle->dumpFile
#ifdef _WINDOWS
    if (CreateDirectory(codec_bench_handle->dumpFile, NULL) == 0)
    {
#else
    if (mkdir(codec_bench_handle->dumpFile, 0777) == -1)
    {
#endif
        LOG_FORMATTED(ERR, log_ctx,
                "Error in creating dump folder [%s].", codec_bench_handle->dumpFile);
        printf("Error in creating dump folder [%s].\n", codec_bench_handle->dumpFile);
        LOG_SYSTEM_ERROR();
        return ERR_CODEC_BENCH_FILE_IO;
    } 
    return 0;
}

/* Run for a particular codec and level */
AOCL_INTP aocl_bench_codec_run(aocl_compression_desc* aocl_codec_handle,
    aocl_codec_bench_info* codec_bench_handle,
    aocl_compression_type codec, AOCL_INTP level) 
{
    AOCL_INTP status = 0;
    AOCL_UINTP inSize, file_size;
    AOCL_INT64 resultComp = 0;
    AOCL_INT64 resultDecomp = 0;
    AOCL_INTP folder_created = 0;
    AOCL_INTP error = 0;
    FILE* dumpFp = NULL;

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    codec_bench_handle->cTime = 0;
    codec_bench_handle->cSize = 0;
    codec_bench_handle->dTime = 0;
    codec_bench_handle->dSize = 0;
    codec_bench_handle->cBestTime = UINT64_MAX;
    codec_bench_handle->dBestTime = UINT64_MAX;
    aocl_codec_handle->level = level;

    //setup the codec method
    if (aocl_llc_setup(aocl_codec_handle, codec) != 0)
    {
        LOG_FORMATTED(ERR, log_ctx,
            "Setup failed for codec [%s].", codec_list[codec].codec_name);
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return -2;
    }
    if(dumpEnabled) {
        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT /* Compress and decompress */ ||
            codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS /* Compress only */) {
                if (codec_bench_handle->file_size > codec_bench_handle->inSize){
                    error = create_dump_folder(codec_bench_handle);
                    if (error != 0) return error;

                    folder_created = 1;
                } else {
                    error = create_dump_file(codec_bench_handle, &dumpFp);
                    if (error != 0) return error;
                }
        } else {
            /* Decompress only */
            error = create_dump_file(codec_bench_handle, &dumpFp);
            if (error != 0) return error;
        }
    }
    for (AOCL_INTP k = 0; k < codec_bench_handle->iterations; k++)
    {
        AOCL_UINT64 temp_cTime = 0;
        AOCL_UINT64 temp_dTime = 0;
        inSize = codec_bench_handle->inSize;
        file_size = codec_bench_handle->file_size;
        AOCL_INTP file_count = codec_bench_handle->cfile_count;

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT /* Compress and decompress */ ||
            codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS /* Compress only */) 
        {
            AOCL_INT32 chunk_cnt = 1;
            while (inSize)
            {
                inSize = fread(codec_bench_handle->inPtr, 1, inSize, codec_bench_handle->fp); // read data in blocks of inSize

                // compress
                aocl_codec_handle->inSize = inSize;
                aocl_codec_handle->outSize = codec_bench_handle->outSize;
                aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
                aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
                resultComp = aocl_llc_compress(aocl_codec_handle, codec);
                if (resultComp <= 0)
                {
                    printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] Compression: failed\n",
                        codec_list[codec].codec_name,
                        level, codec_bench_handle->fName);
                    status = -1;
                    break;
                }

                if (dumpEnabled && k == 0 /* dump only during 1st iteration */) 
                {
                    if(folder_created){
                        error = dump_to_file(codec_bench_handle, codec, level, chunk_cnt, aocl_codec_handle->outBuf, resultComp);
                        if (error!=0) return error;
                    } else {
                        // dump compressed data to file
                        AOCL_UINTP written = fwrite(aocl_codec_handle->outBuf, sizeof(AOCL_CHAR), resultComp,
                            codec_bench_handle->dumpFp);
                        if (written < resultComp) 
                        {
                            printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] Dump: failed\n",
                                codec_list[codec].codec_name,
                                level, codec_bench_handle->fName);
                        }
                    }
                }

                if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT) 
                {
                    // decompress
                    // use file compressed above as input
                    aocl_codec_handle->inSize = resultComp;
                    aocl_codec_handle->outSize = inSize;
                    aocl_codec_handle->inBuf = codec_bench_handle->outPtr;
                    aocl_codec_handle->outBuf = codec_bench_handle->decompPtr;

                    resultDecomp = aocl_llc_decompress(aocl_codec_handle, codec);
                    if (resultDecomp <= 0)
                    {
                        printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] Decompression: failed\n",
                            codec_list[codec].codec_name,
                            level, codec_bench_handle->fName);
                        status = -1;
                        break;
                    }

                    if (codec_bench_handle->verify) // verification supported only if decompress is enabled
                    {
                        if (memcmp(codec_bench_handle->inPtr,
                            codec_bench_handle->decompPtr, inSize) != 0)
                        {
                            printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] verification: failed\n",
                                codec_list[codec].codec_name,
                                level, codec_bench_handle->fName);
                            status = -1;
                            break;
                        }
                    }
                }

                if (codec_bench_handle->print_stats) 
                {
                    // compression stats
                    codec_bench_handle->cTime += aocl_codec_handle->cTime;
                    codec_bench_handle->cSize += aocl_codec_handle->cSize;
                    temp_cTime += aocl_codec_handle->cTime;

                    if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT) 
                    {
                        // decompression stats if both are enabled
                        codec_bench_handle->dTime += aocl_codec_handle->dTime;
                        codec_bench_handle->dSize += aocl_codec_handle->dSize;
                        temp_dTime += aocl_codec_handle->dTime;
                    }
                }
                file_size -= inSize;
                inSize = (file_size > inSize) ? inSize : file_size;
                chunk_cnt++;
            }
            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->cBestTime = temp_cTime < codec_bench_handle->cBestTime ?
                    temp_cTime : codec_bench_handle->cBestTime;
                if(codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT)
                {
                    codec_bench_handle->dBestTime = temp_dTime < codec_bench_handle->dBestTime ?
                        temp_dTime : codec_bench_handle->dBestTime;
                }
            }
        }
        else 
        {   /* Decompress only */
            /* In this mode, 
            * codec_bench_handle->fp is the input compressed data.
            * valFp is the decompressed data for validation.*/

            FILE* valFp = codec_bench_handle->valFp;
            AOCL_INT64 totalResultDecomp = 0;

            // load input compressed file
            inSize = fread(codec_bench_handle->inPtr, 1, codec_bench_handle->inSize, codec_bench_handle->fp);

            do {
                // decompress
                aocl_codec_handle->inSize = inSize;
                aocl_codec_handle->outSize = codec_bench_handle->outSize;
                aocl_codec_handle->inBuf = codec_bench_handle->inPtr;
                aocl_codec_handle->outBuf = codec_bench_handle->outPtr;
                resultDecomp = aocl_llc_decompress(aocl_codec_handle, codec);
                totalResultDecomp += resultDecomp;
                if (resultDecomp <= 0)
                {
                    printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] Decompression: failed\n",
                        codec_list[codec].codec_name,
                        level, codec_bench_handle->fName);
                    status = -1;
                    break;
                }

                if (dumpEnabled && k == 0 /* dump only during 1st iteration */)
                {
                    // dump decompressed data to file
                    AOCL_UINTP written = fwrite(aocl_codec_handle->outBuf, sizeof(AOCL_CHAR), resultDecomp,
                        codec_bench_handle->dumpFp);
                    if (written < resultDecomp)
                    {
                        printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] Dump: failed\n",
                            codec_list[codec].codec_name,
                            level, codec_bench_handle->fName);
                    }
                }

                if (codec_bench_handle->verify)
                {
                    if (valFp == NULL) 
                    {
                        printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] verification file not provided\n",
                            codec_list[codec].codec_name,
                            level, codec_bench_handle->fName);
                        status = -1;
                        break;
                    }
                    // load decompressed file for validation
                    codec_bench_handle->outSize = fread(codec_bench_handle->decompPtr,
                        1, resultDecomp, valFp);

                    if (memcmp(codec_bench_handle->outPtr,
                        codec_bench_handle->decompPtr, codec_bench_handle->outSize) != 0)
                    {
                        printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] verification: failed\n",
                            codec_list[codec].codec_name,
                            level, codec_bench_handle->fName);
                        status = -1;
                        break;
                    }
                    // Rewind in case when the compressed input is a single file
                    if (!isFolder) rewind(valFp);
                }

                if (codec_bench_handle->print_stats)
                { // decompression stats
                    codec_bench_handle->dTime += aocl_codec_handle->dTime;
                    codec_bench_handle->dSize += aocl_codec_handle->dSize;
                    temp_dTime += aocl_codec_handle->dTime;
                }

                file_count -= 1;
                if (file_count == 0) break;

                close_file_in_folder(codec_bench_handle); //close prev file if open
                error = open_file_in_folder(codec_bench_handle, (AOCL_INT32)(codec_bench_handle->cfile_count - file_count + 1));
                if(error != 0) return error;

                // codec_bench_handle->fp points to the next file in the folder
                inSize = fread(codec_bench_handle->inPtr, 1, codec_bench_handle->inSize, codec_bench_handle->fp);

            } while (1);

            if (isFolder && (k < codec_bench_handle->iterations - 1))
            {
                close_file_in_folder(codec_bench_handle); //close prev file if open
                // codec_bench_handle->fp is made to point to first file in the folder for next iteration
                error = open_file_in_folder(codec_bench_handle, 1);
                if (error != 0) return error;
            }
            resultDecomp = totalResultDecomp;
            if (codec_bench_handle->print_stats)
            {
                codec_bench_handle->dBestTime = temp_dTime < codec_bench_handle->dBestTime ?
                                                    temp_dTime : codec_bench_handle->dBestTime;
            }
        }

        rewind(codec_bench_handle->fp);
        if (status != 0)
            break;
    }

    if (dumpFp) {
        fclose(dumpFp);
        dumpFp = NULL;
    }

    //destroy the codec method
    aocl_llc_destroy(aocl_codec_handle, codec);

    if (status != 0)
    {
        LOG_FORMATTED(ERR, log_ctx,
            "Compression/Decompression/Verification operation failed for codec [%s].", codec_list[codec].codec_name);
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return status;
    }

    if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
        codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS) 
    {
        if (codec_bench_handle->verify)
        {
            printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] verification: passed\n",
                codec_list[codec].codec_name,
                level, codec_bench_handle->fName);

        }
    }

    if (codec_bench_handle->print_stats)
    {
        printf("AOCL-COMPRESSION [%s-%td] [Filename:%s] -------------------------------------\n",
            codec_list[codec].codec_name,
            level, codec_bench_handle->fName);

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
            codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS) 
        {
            codec_bench_handle->cSpeed = (codec_bench_handle->file_size *
                codec_bench_handle->iterations * 1000.0) /
                codec_bench_handle->cTime;
            codec_bench_handle->cBestSpeed =
                (codec_bench_handle->file_size * 1000.0) /
                codec_bench_handle->cBestTime;
            printf("Compression:         speed(avg) %.2f MB/s, time(avg) %.2f ms, size %zu, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                codec_bench_handle->cSpeed,
                codec_bench_handle->cTime /
                (codec_bench_handle->iterations * 1000000.0),
                codec_bench_handle->cSize / codec_bench_handle->iterations,
                codec_bench_handle->cBestSpeed,
                codec_bench_handle->cBestTime / 1000000.0);
        }

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
            codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS) 
        {
            if(codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT)
            {
                codec_bench_handle->dSpeed = (codec_bench_handle->file_size *
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->dTime;
                codec_bench_handle->dBestSpeed = (codec_bench_handle->file_size * 1000.0) /
                    codec_bench_handle->dBestTime;
            }
            else
            {
                codec_bench_handle->dSpeed = (resultDecomp *
                    codec_bench_handle->iterations * 1000.0) /
                    codec_bench_handle->dTime;
                codec_bench_handle->dBestSpeed = (resultDecomp * 1000.0) /
                    codec_bench_handle->dBestTime;
            }
            printf("Decompression:       speed(avg) %.2f MB/s, time(avg) %.2f ms, size %zu, speed(best) %.2f MB/s, time(best) %.2f ms\n",
                codec_bench_handle->dSpeed,
                codec_bench_handle->dTime /
                (codec_bench_handle->iterations * 1000000.0),
                codec_bench_handle->dSize / codec_bench_handle->iterations,
                codec_bench_handle->dBestSpeed,
                codec_bench_handle->dBestTime / 1000000.0);
        }

        if (codec_bench_handle->runOperation == RUN_OPERATION_DEFAULT ||
            codec_bench_handle->runOperation == RUN_OPERATION_COMPRESS) 
        {
            printf("Ratio:               %.2f\n",
                (((codec_bench_handle->cSize * 100.0) /
                    codec_bench_handle->iterations) /
                    codec_bench_handle->file_size));
        }
    }

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
    return status;
}

AOCL_INTP aocl_bench_run(aocl_compression_desc *aocl_codec_handle,
                   aocl_codec_bench_info *codec_bench_handle)
{
    AOCL_INTP retStatus = 0;

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    aocl_codec_handle->memLimit = codec_bench_handle->mem_limit;

    if (codec_bench_handle->print_stats == 0 &&
        codec_bench_handle->verify == 1)
        codec_bench_handle->iterations = 1;

    if (codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS
        && codec_bench_handle->codec_method < 0) 
    {
        /* data decompressed using a certain codec can only be decompressed
         * by that particular codec. Run-all-codecs mode is not supported in this case */
        LOG_UNFORMATTED(ERR, log_ctx,
            "Codec not specified. Specify codec using -e when running -rdecompress mode.");
        printf("Codec not specified. Specify codec using -e when running -rdecompress mode.\n\n");
        LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
        return -2;
    }

    if (codec_bench_handle->codec_method < 0) // run for multiple codecs
    {
        for (aocl_compression_type codec = LZ4; codec < AOCL_COMPRESSOR_ALGOS_NUM; codec++)
        {
            aocl_codec_handle->optVar = codec_list[codec].extra_param;
            for (AOCL_INTP level = codec_list[codec].lower_level; level <= codec_list[codec].upper_level; level++)
            {
                AOCL_INTP status = aocl_bench_codec_run(aocl_codec_handle, codec_bench_handle, 
                    codec, level);
                if (status < 0) 
                {
                    retStatus = status;
                    if (status == -2)
                        break;
                }
            }
            
        }
    }
    else
    {
        AOCL_INTP def_level = aocl_codec_handle->level;
        AOCL_INTP lower_level, upper_level;

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

        if (codec_bench_handle->runOperation == RUN_OPERATION_DECOMPRESS) 
        {
            /* In decompress only mode, given input file is decompressed.
             * Level information is not used. Run only once. */
            AOCL_INTP status = aocl_bench_codec_run(aocl_codec_handle, codec_bench_handle,
                codec_bench_handle->codec_method, def_level);
            if (status < 0) 
            {
                retStatus = status;
                LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
                return retStatus;
            }
        }
        else 
        {
            for (AOCL_INTP level = lower_level; level <= upper_level; level++) // run for requested levels
            {
                AOCL_INTP status = aocl_bench_codec_run(aocl_codec_handle, codec_bench_handle,
                    codec_bench_handle->codec_method, level);
                if (status < 0) 
                {
                    retStatus = status;
                    if (status == -2)
                        break;
                }
            }
        }
    }
    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");

    return retStatus;
}

AOCL_VOID destroy(aocl_codec_bench_info *codec_bench_handle)
{
    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    if (codec_bench_handle->inPtr)
        free(codec_bench_handle->inPtr);
    if (codec_bench_handle->outPtr)
        free(codec_bench_handle->outPtr);
    if (codec_bench_handle->decompPtr)
        free(codec_bench_handle->decompPtr);

    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
}

AOCL_INT32 main (AOCL_INT32 argc, AOCL_CHAR **argv)
{
    aocl_codec_bench_info codec_bench_handle;
    codec_bench_handle.fp = NULL;
    aocl_compression_desc aocl_codec_ds;
    aocl_compression_desc *aocl_codec_handle = &aocl_codec_ds;
    FILE* valFp = NULL;
    AOCL_INTP result = 0;
    AOCL_INTP ret;

    ret = read_user_options (argc, argv, &codec_bench_handle);

    LOG_UNFORMATTED(TRACE, log_ctx, "Enter");

    if (ret == 0)
    {
        LOG_UNFORMATTED(ERR, log_ctx,
        "Invalid option passed, ignoring more than one input file. Use -h to know supported user options.");
        printf("Invalid option passed, ignoring more than one input file. Use -h to know supported user options.\n");
    }
    else if (ret == ERR_CODEC_BENCH_ARGS)
    {
        LOG_UNFORMATTED(ERR, log_ctx,
        "Invalid option passed. Use -h to know supported user options.");
        printf("Invalid option passed. Use -h to know supported user options.\n");
        result = ret;
        goto exit;
    }
    else if (ret == ERR_CODEC_BENCH_METHOD)
    {
        LOG_UNFORMATTED(ERR, log_ctx,
        "Unsupported compression method or level specified. Use -l to know supported methods and -h for exact user options.");
        printf("Unsupported option passed. Use -l to know supported methods and -h for exact user options.\n");
        result = ret;
		goto exit;
    }
    else if (ret == 2) // user option requested help information
        goto exit;

    if(codec_bench_handle.runOperation == RUN_OPERATION_DECOMPRESS)
    {
        if (isFolder)
        {
            //List all the files
            AOCL_INTP file_count = get_file_count(inFolder);
            if(file_count <= 0){
                LOG_UNFORMATTED(ERR, log_ctx,"No input file present in the folder path provided.");
                printf("No input file present in the folder path provided.\n");
                return ERR_CODEC_BENCH_FILE_IO;
            }

            codec_bench_handle.cfile_count = file_count;

            // open the first file in the folder
            AOCL_CHAR filename[MAX_FILENAME_LEN];
            AOCL_CHAR filePath[MAX_FILENAME_LEN];

            strcpy(filePath, inFolder);
            if(get_file_name(&codec_bench_handle, filename, 1) != 0) {
                LOG_STRING_OVERFLOW("Compressed input file path");
                ret = ERR_CODEC_BENCH_ARGS;
                goto exit;
            }

            AOCL_UINTP append_len = 1 + strlen(filename);
            if (STRING_OVERFLOW(strlen(filePath), append_len, MAX_FILENAME_LEN)){
                LOG_STRING_OVERFLOW("Compressed input file path");
                ret = ERR_CODEC_BENCH_ARGS;
                goto exit;
            }
            CODEC_STRCAT(filePath);
            strcat(filePath, filename);

            if (!open_file(&codec_bench_handle, filePath, filename))
            {
                result = ERR_CODEC_BENCH_FILE_IO;
                goto exit;
            }      
        }
        else 
        {
            codec_bench_handle.cfile_count = 1;
        }
    }   

    if (!(codec_bench_handle.runOperation == RUN_OPERATION_DECOMPRESS && isFolder)) {
        if (!open_file(&codec_bench_handle, inFile, inFile))
        {
            result = ERR_CODEC_BENCH_FILE_IO;
            goto exit;
        }
    }

    if (dumpEnabled) 
    {
        // handle invalid cases
        if (codec_bench_handle.codec_method < 0) 
        {
            LOG_UNFORMATTED(ERR, log_ctx,
                "Arg -d ignored. Specify codec and level using -e to avail this feature.");
            printf("Arg -d ignored. Specify codec and level using -e to avail this feature.\n\n");
            dumpEnabled = 0;
        }
        else if (codec_bench_handle.codec_level == UNINIT_LEVEL &&
            !(codec_bench_handle.runOperation == RUN_OPERATION_DECOMPRESS ||
                (codec_list[codec_bench_handle.codec_method].lower_level
                    == codec_list[codec_bench_handle.codec_method].upper_level /* method supports only 1 level */)))
        {
            // Level must be specified in non-decompress only modes, as dump is for a single run only.
            LOG_UNFORMATTED(ERR, log_ctx,
                "Arg -d ignored. Specify codec level with -e arg to avail this feature.");
            printf("Arg -d ignored. Specify codec level with -e arg to avail this feature.\n\n");
            dumpEnabled = 0;
        }
        else 
        {
            codec_bench_handle.dumpFile = dumpFile;
        }
    }

    if (valEnabled) 
    {
        if (codec_bench_handle.runOperation != RUN_OPERATION_DECOMPRESS) 
        {
            LOG_UNFORMATTED(INFO, log_ctx,
                "-f option ignored. It is valid only when -rdecompress is passed.");
        }
        // open file with reference decompressed data for validation
        if (!(valFp = fopen(valFile, "rb")))
        {
            LOG_FORMATTED(ERR, log_ctx,
                "Error in opening validation file [%s].", valFile);
            printf("Error in opening validation file [%s].\n", valFile);
            result = ERR_CODEC_BENCH_FILE_IO;
            goto exit;
        }
        codec_bench_handle.valFp = valFp;
    }

    if (init(&codec_bench_handle, aocl_codec_handle) < 0)
    {
        LOG_UNFORMATTED(ERR, log_ctx,
        "Error in allocating memory.");
        result = ERR_CODEC_BENCH_MEM;
		goto exit;
    }

    if (codec_bench_handle.useIPP)
#ifdef _WINDOWS
        LOG_UNFORMATTED(ERR, log_ctx,
            "IPP test execution not supported on Windows for now.");
#else
        result = ipp_bench_run(aocl_codec_handle, &codec_bench_handle);
#endif
    else if (codec_bench_handle.useNAPI)
        {
            result = native_api_bench_run(aocl_codec_handle, &codec_bench_handle);
        }   
    else
        result = aocl_bench_run(aocl_codec_handle, &codec_bench_handle);
    if (result != 0)
    {
        LOG_UNFORMATTED(ERR, log_ctx,
        "Error in compression and decompression operations.");
    }

    destroy(&codec_bench_handle);

exit:
    close_file(&codec_bench_handle);
    if (valFp)
        fclose(valFp);
    LOG_UNFORMATTED(TRACE, log_ctx, "Exit");
    return result;
}
