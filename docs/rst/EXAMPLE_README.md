Example Test Programs
=====================

| Example                   | Description |
| --------------------------|------------ |
| [Example 1](#example-1)   | Sample usage and calling sequence of aocl - compression APIs to compress and decompress a test input. |
| [Example 2](#example-2)   | Sample usage and calling sequence of aocl - compression APIs to get ST compatible compressed stream from a stream produced by AOCL MT compressor. |



### Example 1

The following test program shows the sample usage and calling sequence of aocl - compression APIs to compress and decompress a test input :

```C
#include <stdio.h> 
#include <stdlib.h>
#include "aocl_compression.h"

int main (int argc, char **argv)
{
    aocl_compression_desc aocl_compression_ds;
    aocl_compression_desc *aocl_compression_handle = &aocl_compression_ds;
    FILE *inFp = NULL;
    int file_size = 0;
    char *inPtr = NULL, *compPtr = NULL, *decompPtr = NULL;
    int64_t resultComp = 0, resultDecomp = 0;

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
    aocl_compression_handle->inSize = file_size;
    aocl_compression_handle->outSize = (file_size + (file_size / 6) + (16 * 1024));
    inPtr = (char *)calloc(1, aocl_compression_handle->inSize);
    compPtr = (char *)calloc(1, aocl_compression_handle->outSize);
    decompPtr = (char *)calloc(1, aocl_compression_handle->inSize);
    aocl_compression_handle->inBuf = inPtr;
    aocl_compression_handle->outBuf = compPtr;
    file_size = fread(inPtr, 1, file_size, inFp);

    // 1. setup and create a handle
    aocl_llc_setup(aocl_compression_handle, method);

    // 2. compress
    resultComp = aocl_llc_compress(aocl_compression_handle, method);
    
    if (resultComp <= 0)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    // decompress
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

    // destroy handle
    aocl_llc_destroy(aocl_compression_handle, method);
    error_exit:
    if (inPtr)
        free(inPtr);
    if (compPtr)
        free(compPtr);
    if (decompPtr)
        free(decompPtr);
    return 0;
}

```

To build this example test program on a Linux system using GCC or AOCC, you must specify
path to aocl_compression.h header file and link with libaocl_compression.so file as follows:

`gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so file path> -laocl_compression`

### Example 2

The following test program shows the sample usage and calling sequence of aocl - compression APIs to get ST compatible compressed stream from a stream produced by AOCL MT compressor :

Build AOCL-Compression library with `AOCL_ENABLE_THREADS`.

```C
#include <stdio.h> 
#include <stdlib.h>
#include "aocl_compression.h"
#include "aocl_threads.h"

int main (int argc, char **argv)
{
    aocl_compression_desc aocl_compression_ds;
    aocl_compression_desc *aocl_compression_handle = &aocl_compression_ds;
    FILE *inFp = NULL;
    int file_size = 0;
    char *inPtr = NULL, *compPtr = NULL, *decompPtr = NULL;
    int64_t resultComp = 0, resultDecomp = 0;

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
    aocl_compression_handle->inSize = file_size;
    aocl_compression_handle->outSize = (file_size + (file_size / 6) + (16 * 1024)) /* LZ4 ST compress bound */ 
                                        + aocl_get_rap_frame_bound_mt() /* upper bound of RAP frame bytes */;
    inPtr = (char *)calloc(1, aocl_compression_handle->inSize);
    compPtr = (char *)calloc(1, aocl_compression_handle->outSize);
    decompPtr = (char *)calloc(1, aocl_compression_handle->inSize);
    aocl_compression_handle->inBuf = inPtr;
    aocl_compression_handle->outBuf = compPtr;
    file_size = fread(inPtr, 1, file_size, inFp);

    // 1. setup and create a handle
    aocl_llc_setup(aocl_compression_handle, method);

    // 2. MT compress
    resultComp = aocl_llc_compress(aocl_compression_handle, method);
    
    if (resultComp <= 0)
    {
        printf("Compression: failed\n");
        goto error_exit;
    }
    printf("Compression: done\n");

    //3. ST decompress
    // Get number of bytes for the RAP frame
    int rap_frame_len = aocl_skip_rap_frame_mt((char *)compPtr, resultComp);

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

    // destroy handle
    aocl_llc_destroy(aocl_compression_handle, method);
    error_exit:
    if (inPtr)
        free(inPtr);
    if (compPtr)
        free(compPtr);
    if (decompPtr)
        free(decompPtr);
    return 0;
}

```

To build this example test program on a Linux system using GCC or AOCC, 
you must specify path to aocl_compression.h and aocl_threads.h header files
and link with libaocl_compression.so file as follows:

`gcc test.c -I <aocl_compression.h file path> -L <libaocl_compression.so file path> -laocl_compression`
