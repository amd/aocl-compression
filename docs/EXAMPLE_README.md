Example Test Programs
=====================

| Example              | Description |
| -------------------- | ----------- |
| @subpage ETP         | Sample usage and calling sequence of aocl - compression APIs to compress and decompress a test input. |
| @subpage MT_ETP      | Sample usage and calling sequence of aocl - compression APIs to extract format compliant compressed stream from a stream produced by AOCL multi-threaded compressor. |
| @subpage MT_GZIP_ETP | Sample usage and calling sequence of aocl - compression APIs for multi-threaded gzip compression. |


@page ETP Example 1

The following test program shows the sample usage and calling sequence of aocl - compression APIs to compress and decompress a test input :

```C
#include <stdio.h> 
#include <stdlib.h>
#include "aocl_compression.h"

int main (int argc, char **argv)
{
    aocl_compression_desc aocl_compression_ds;
    aocl_compression_desc* aocl_compression_handle = &aocl_compression_ds;
    FILE* inFp = NULL;
    int file_size = 0;
    char* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    int64_t resultCompBound = 0, resultComp = 0, resultDecomp = 0;

    printf("Running example_unified_api\n");
    if (argc < 2)
    {
        printf("Provide input test file path\n");
        return -1;
    }
    inFp = fopen(argv[1], "rb");
    fseek(inFp, 0L, SEEK_END);
    file_size = ftell(inFp);
    rewind(inFp);

    // One of the compression methods as per aocl_compression_type
    aocl_compression_type method = LZ4;
    aocl_compression_handle->level = 0;
    aocl_compression_handle->optVar = 0;
    aocl_compression_handle->optOff = 0;
    aocl_compression_handle->measureStats = 0;

    // 1. setup and create a handle
    if (aocl_llc_setup(aocl_compression_handle, method) != 0)
    {
        printf("Setup: failed\n");
        goto error_exit;
    }

    // 2. allocate buffers
    aocl_compression_handle->inSize = file_size;
    resultCompBound = aocl_llc_compressBound(method, aocl_compression_handle->inSize);
    if (resultCompBound < 0)
    {
        printf("CompressBound: failed\n");
        goto error_exit;
    }
    aocl_compression_handle->outSize = resultCompBound;
    inPtr = (char*)calloc(1, aocl_compression_handle->inSize);
    compPtr = (char*)calloc(1, aocl_compression_handle->outSize);
    decompPtr = (char*)calloc(1, aocl_compression_handle->inSize);
    aocl_compression_handle->inBuf = inPtr;
    aocl_compression_handle->outBuf = compPtr;
    file_size = fread(inPtr, 1, file_size, inFp);

    // 3. compress
    resultComp = aocl_llc_compress(aocl_compression_handle, method);

    if (resultComp <= 0)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    // 4. decompress
    aocl_compression_handle->inSize = resultComp;
    aocl_compression_handle->outSize = file_size;
    aocl_compression_handle->inBuf = compPtr;
    aocl_compression_handle->outBuf = decompPtr;

    resultDecomp = aocl_llc_decompress(aocl_compression_handle, method);

    if (resultDecomp <= 0)
    {
        printf("Decompression Failure\n");
        goto error_exit;
    }
    printf("Decompression: done\n");

    // 5. destroy handle
    aocl_llc_destroy(aocl_compression_handle, method);

error_exit:
    if (inPtr)
        free(inPtr);
    if (compPtr)
        free(compPtr);
    if (decompPtr)
        free(decompPtr);
    if (inFp)
        fclose(inFp);
    return 0;
}

```

To build this example test program on a Linux system using GCC or AOCC, you must specify
path to aocl_compression.h header file and link with libaocl_compression.so file as follows:

`gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so directory path> -laocl_compression -Wl,-rpath=<libaocl_compression.so directory path>`

Before running the example program, ensure it points to the right library dependencies for openMP, etc.

@page MT_ETP Example 2

The following test program shows the sample usage and calling sequence of aocl - compression APIs to extract format compliant compressed stream from a stream produced by AOCL multi-threaded compressor :

Build AOCL-Compression library with `AOCL_ENABLE_THREADS`.

```C
#include <stdio.h> 
#include <stdlib.h>
#include "aocl_compression.h"

int main (int argc, char **argv)
{
    aocl_compression_desc aocl_compression_ds;
    aocl_compression_desc* aocl_compression_handle = &aocl_compression_ds;
    FILE* inFp = NULL;
    int file_size = 0;
    char* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    int64_t resultCompBound = 0, resultComp = 0, resultDecomp = 0;

    printf("Running example_aocl_llc_skip_rap_frame\n");
    if (argc < 2)
    {
        printf("Provide input test file path\n");
        return -1;
    }
    inFp = fopen(argv[1], "rb");
    fseek(inFp, 0L, SEEK_END);
    file_size = ftell(inFp);
    rewind(inFp);

    aocl_compression_type method = LZ4; // One of the compression methods as per aocl_compression_type
    aocl_compression_handle->level = 0;
    aocl_compression_handle->optVar = 0;
    aocl_compression_handle->optOff = 0;
    aocl_compression_handle->measureStats = 0;

    // 1. setup and create a handle
    if (aocl_llc_setup(aocl_compression_handle, method) != 0)
    {
        printf("Setup: failed\n");
        goto error_exit;
    }

    // 2. allocate buffers
    aocl_compression_handle->inSize = file_size;
    resultCompBound = aocl_llc_compressBound(method, aocl_compression_handle->inSize);
    if (resultCompBound < 0)
    {
        printf("CompressBound: failed\n");
        goto error_exit;
    }
    aocl_compression_handle->outSize = resultCompBound;
    inPtr = (char*)calloc(1, aocl_compression_handle->inSize);
    compPtr = (char*)calloc(1, aocl_compression_handle->outSize);
    decompPtr = (char*)calloc(1, aocl_compression_handle->inSize);
    aocl_compression_handle->inBuf = inPtr;
    aocl_compression_handle->outBuf = compPtr;
    file_size = fread(inPtr, 1, file_size, inFp);


    // 3. MT compress
    resultComp = aocl_llc_compress(aocl_compression_handle, method);

    if (resultComp <= 0)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    //4. ST decompress
    // Get number of bytes for the RAP frame
    int rap_frame_len = aocl_llc_skip_rap_frame((char*)compPtr, resultComp);

    // Skip RAP frame in input stream and pass this to ST decompressor
    aocl_compression_handle->inSize = resultComp - rap_frame_len;
    aocl_compression_handle->outSize = file_size;
    aocl_compression_handle->inBuf = compPtr + rap_frame_len;
    aocl_compression_handle->outBuf = decompPtr;

    // Pass format compliant stream to aocl decompressor (or any legacy ST decompressor)
    resultDecomp = aocl_llc_decompress(aocl_compression_handle, method);

    if (resultDecomp <= 0)
    {
        printf("Decompression Failure\n");
        goto error_exit;
    }
    printf("Decompression: done\n");

    // 5. destroy handle
    aocl_llc_destroy(aocl_compression_handle, method);
error_exit:
    if (inPtr)
        free(inPtr);
    if (compPtr)
        free(compPtr);
    if (decompPtr)
        free(decompPtr);
    if (inFp)
        fclose(inFp);
    return 0;
}

```

To build this example test program on a Linux system using GCC or AOCC, you must specify
path to aocl_compression.h header file and link with libaocl_compression.so file as follows:

`gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so directory path> -laocl_compression -Wl,-rpath=<libaocl_compression.so directory path>`

Before running the example program, ensure it points to the right library dependencies for openMP, etc.

@page MT_GZIP_ETP Example 3

The following test program shows the sample usage and calling sequence of aocl - compression APIs for multi-threaded gzip compression :

Build AOCL-Compression library with `AOCL_ENABLE_THREADS`.

```C
#include <stdio.h>
#include <stdlib.h>
#include "zlib.h"

int main(int argc, char** argv)
{
    FILE* inFp = NULL;
    uLong inpSize = 0;
    Bytef* inPtr = NULL, * compPtr = NULL, * decompPtr = NULL;
    uLong outSize = 0;
    int resultComp, resultDecomp;
    int level = 6;

    if (argc < 2)
    {
        printf("Provide input test file path\n");
        return -1;
    }
    inFp = fopen(argv[1], "rb");
    fseek(inFp, 0L, SEEK_END);
    inpSize = ftell(inFp);
    rewind(inFp);

    // 1. allocate buffers
    outSize = compressBound_gzip(inpSize);
    if (outSize == 0)
    {
        printf("CompressBound_gzip: failed\n");
        goto error_exit;
    }
    inPtr     = (Bytef*)calloc(1, inpSize);
    compPtr   = (Bytef*)calloc(1, outSize);
    decompPtr = (Bytef*)calloc(1, inpSize);
    inpSize = fread(inPtr, 1, inpSize, inFp);

    // 2. compress
    resultComp = compress2_gzip(compPtr, &outSize, inPtr, inpSize, level);
    if (resultComp != Z_OK)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    // 3. decompress
    resultDecomp = uncompress2_gzip(decompPtr, &inpSize, compPtr, &outSize);
    if (resultDecomp != Z_OK)
    {
        printf("Decompression Failure\n");
        goto error_exit;
    }
    printf("Decompression: done\n");

    // 4. cleanup
error_exit:
    if (inPtr)
        free(inPtr);
    if (compPtr)
        free(compPtr);
    if (decompPtr)
        free(decompPtr);
    if (inFp)
        fclose(inFp);
    return 0;
}

```

To build this example test program on a Linux system using GCC or AOCC, you must specify
path to aocl_compression.h header file and link with libaocl_compression.so file as follows:

`gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so directory path> -laocl_compression -Wl,-rpath=<libaocl_compression.so directory path>`

Before running the example program, ensure it points to the right library dependencies for openMP, etc.
