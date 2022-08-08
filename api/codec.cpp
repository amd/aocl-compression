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
 
 /** @file codec.cpp
 *  
 *  @brief Wrapper functions of the supported native codec methods.
 *
 *  This file contains the wrapper functions of the supported compression and
 *  decompression methods calling their associated native APIs.
 *
 *  @author S. Biplab Raut
 */
 
#include "types.h"
#include "api.h"
#include "codec.h"

//bzip2
#ifndef AOCL_EXCLUDE_BZIP2
#include "algos/bzip2/bzlib.h"
#endif
//lz4
#ifndef AOCL_EXCLUDE_LZ4
#include "algos/lz4/lz4.h"
#endif
//lz4hc
#ifndef AOCL_EXCLUDE_LZ4HC
#include "algos/lz4/lz4hc.h"
#endif
//lzma
#ifndef AOCL_EXCLUDE_LZMA
#include <string.h>
#include "algos/lzma/Alloc.h"
#include "algos/lzma/LzmaDec.h"
#include "algos/lzma/LzmaEnc.h"
#endif
//snappy
#ifndef AOCL_EXCLUDE_SNAPPY
#include "algos/snappy/snappy.h"
#endif
//zlib
#ifndef AOCL_EXCLUDE_ZLIB
#include "algos/zlib/zlib.h"
#endif
//zstd
#ifndef AOCL_EXCLUDE_ZSTD
#define ZSTD_STATIC_LINKING_ONLY
#include "algos/zstd/lib/zstd.h"
#endif

//bzip2
#ifndef AOCL_EXCLUDE_BZIP2
CHAR *aocl_bzip2_setup(INTP optOff, INTP optLevel,
                       UINTP insize, UINTP level, UINTP windowLog)
{
    //ToDo: Implement a new bzip2 API for setup
    return NULL;
}

INT64 aocl_bzip2_compress(CHAR *inbuf, UINTP insize, CHAR *outbuf, 
						  UINTP outsize, UINTP level, UINTP windowLog, CHAR *)
{
   UINT32 outSizeL = outsize;
   if (BZ2_bzBuffToBuffCompress((CHAR *)outbuf, &outSizeL, (CHAR *)inbuf, 
	   (UINTP)insize, level, 0, 0)==BZ_OK)
	   return outSizeL;
   else
	   return -1;
}

INT64 aocl_bzip2_decompress(CHAR *inbuf, UINTP insize, CHAR *outbuf, 
							UINTP outsize, UINTP level, UINTP, CHAR *)
{
   UINT32 outSizeL = outsize;
   if (BZ2_bzBuffToBuffDecompress((CHAR *)outbuf, &outSizeL, (CHAR *)inbuf, 
	   (UINTP)insize, 0, 0)==BZ_OK)
	   return outSizeL;
   else
	   return -1;
}
#endif


//lz4
#ifndef AOCL_EXCLUDE_LZ4
CHAR *aocl_lz4_setup(INTP optOff, INTP optLevel,
                     UINTP insize, UINTP level, UINTP windowLog)
{
    //ToDo: Implement a new lz4 API for setup
    return NULL;
}

INT64 aocl_lz4_compress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
                        UINTP outsize, UINTP level, UINTP, CHAR *)
{
	return LZ4_compress_default(inbuf, outbuf, insize, outsize);
}

INT64 aocl_lz4_decompress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
                          UINTP outsize, UINTP level, UINTP, CHAR *)
{
	return LZ4_decompress_safe(inbuf, outbuf, insize, outsize);
}
#endif


//lz4hc
#ifndef AOCL_EXCLUDE_LZ4HC
CHAR *aocl_lz4hc_setup(INTP optOff, INTP optLevel,
                       UINTP insize, UINTP level, UINTP windowLog)
{
    //ToDo: Implement a new lz4hc API for setup
    return NULL;
}

INT64 aocl_lz4hc_compress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
                          UINTP outsize, UINTP level, UINTP, CHAR *)
{
	return LZ4_compress_HC(inbuf, outbuf, insize, outsize, level);
}

INT64 aocl_lz4hc_decompress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
							UINTP outsize, UINTP, UINTP, CHAR *)
{
	return LZ4_decompress_safe(inbuf, outbuf, insize, outsize);
}
#endif


//lzma
#ifndef AOCL_EXCLUDE_LZMA
CHAR *aocl_lzma_setup(INTP optOff, INTP optLevel,
                      UINTP insize, UINTP level, UINTP windowLog)
{
    //ToDo: Implement a new lzma API for setup
    return NULL;
}

INT64 aocl_lzma_compress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
                         UINTP outsize, UINTP level, UINTP, CHAR *)
{
	CLzmaEncProps encProps;
	INTP res;
    UINTP headerSize = LZMA_PROPS_SIZE;
	SizeT outLen = outsize - LZMA_PROPS_SIZE;
	
	LzmaEncProps_Init(&encProps);
	encProps.level = level;
	LzmaEncProps_Normalize(&encProps);

  	res = LzmaEncode((UINT8 *)outbuf+LZMA_PROPS_SIZE, &outLen, (UINT8 *)inbuf, 
				     insize, &encProps, (UINT8 *)outbuf, &headerSize, 0, NULL, 
					 &g_Alloc, &g_Alloc);
	if (res != SZ_OK)
		return 0;
	
	return LZMA_PROPS_SIZE + outLen;
}

INT64 aocl_lzma_decompress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
						   UINTP outsize, UINTP, UINTP, CHAR *)
{
	INTP res;
	SizeT outLen = outsize;
	SizeT srcLen = insize - LZMA_PROPS_SIZE;
	ELzmaStatus status;
	
	res = LzmaDecode((UINT8 *)outbuf, &outLen, (UINT8 *)inbuf+LZMA_PROPS_SIZE, 
					 &srcLen, (UINT8 *)inbuf, LZMA_PROPS_SIZE, LZMA_FINISH_END,
					 &status, &g_Alloc);
	if (res != SZ_OK)
		return 0;

    return outLen;
}
#endif


#ifndef AOCL_EXCLUDE_SNAPPY
CHAR *aocl_snappy_setup(INTP optOff, INTP optLevel,
                        UINTP insize, UINTP level, UINTP windowLog)
{
    //ToDo: Implement a new snappy API for setup
    return NULL;
}

INT64 aocl_snappy_compress(CHAR *inbuf, UINTP insize, CHAR *outbuf, 
						   UINTP outsize, UINTP, UINTP, CHAR *)
{
	snappy::RawCompress(inbuf, insize, outbuf, &outsize);
	return outsize;
}

INT64 aocl_snappy_decompress(CHAR *inbuf, UINTP insize, CHAR *outbuf, 
							 UINTP outsize, UINTP, UINTP, CHAR *)
{
	snappy::RawUncompress(inbuf, insize, outbuf);
	return outsize;
}
#endif


#ifndef AOCL_EXCLUDE_ZLIB
CHAR *aocl_zlib_setup(INTP optOff, INTP optLevel,
                      UINTP insize, UINTP level, UINTP windowLog)
{
    //ToDo: Implement a new zlib API for setup
    return NULL;
}

INT64 aocl_zlib_compress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
                         UINTP outsize, UINTP level, UINTP, CHAR *)
{
	uLongf zencLen = insize;
	INTP res = compress2((UINT8 *)outbuf, &zencLen, 
						(UINT8 *)inbuf, insize, level);
	if (res != Z_OK)
		return 0;
	return zencLen;
}

INT64 aocl_zlib_decompress(CHAR *inbuf, UINTP insize, CHAR *outbuf, 
						   UINTP outsize, UINTP, UINTP, CHAR *)
{
	uLongf zdecLen = outsize;
	INTP res = uncompress((UINT8*)outbuf, &zdecLen, (UINT8 *)inbuf, insize);
	if (res != Z_OK)
		return 0;
	return zdecLen;
}
#endif


#ifndef AOCL_EXCLUDE_ZSTD
#define ZSTD_STATIC_LINKING_ONLY
typedef struct {
    ZSTD_CCtx *cctx;
    ZSTD_DCtx *dctx;
    ZSTD_CDict *cdict;
    ZSTD_parameters zparams;
    ZSTD_customMem cmem;
} zstd_params_t;
CHAR *aocl_zstd_setup(INTP optOff, INTP optLevel,
                      UINTP insize, UINTP level, UINTP windowLog)
{
    zstd_params_t *zstd_params = (zstd_params_t *) 
    malloc(sizeof(zstd_params_t));
    if (!zstd_params)
		return NULL;
    zstd_params->cctx = ZSTD_createCCtx();
    zstd_params->dctx = ZSTD_createDCtx();
    zstd_params->cdict = NULL;

    return (CHAR*) zstd_params;
}

VOID aocl_zstd_destroy(CHAR *workmem)
{
    zstd_params_t *zstd_params = (zstd_params_t *) workmem;
    if (!zstd_params)
		return;
    if (zstd_params->cctx)
		ZSTD_freeCCtx(zstd_params->cctx);
    if (zstd_params->dctx)
		ZSTD_freeDCtx(zstd_params->dctx);
    if (zstd_params->cdict)
		ZSTD_freeCDict(zstd_params->cdict);
    free(workmem);
}

INT64 aocl_zstd_compress(CHAR *inbuf, UINTP insize, CHAR *outbuf,
                         UINTP outsize, UINTP level, UINTP windowLog,
                         CHAR *workmem)
{
    UINTP res;
    zstd_params_t *zstd_params = (zstd_params_t *) workmem;
    
	if (!zstd_params || !zstd_params->cctx)
		return 0;

    zstd_params->zparams = ZSTD_getParams(level, insize, 0);
    ZSTD_CCtx_setParameter(zstd_params->cctx, ZSTD_c_compressionLevel, level);
    zstd_params->zparams.fParams.contentSizeFlag = 1;

    if (windowLog && zstd_params->zparams.cParams.windowLog > windowLog) {
        zstd_params->zparams.cParams.windowLog = windowLog;
        zstd_params->zparams.cParams.chainLog = windowLog + 
			((zstd_params->zparams.cParams.strategy == ZSTD_btlazy2) || 
			(zstd_params->zparams.cParams.strategy == ZSTD_btopt) || 
			(zstd_params->zparams.cParams.strategy == ZSTD_btultra));
    }
    res = ZSTD_compress_advanced(zstd_params->cctx, outbuf, outsize, inbuf, 
								 insize, NULL, 0, zstd_params->zparams);

    if (ZSTD_isError(res))
		return res;

    return res;
}

INT64 aocl_zstd_decompress(CHAR *inbuf, UINTP insize, CHAR *outbuf, 
						   UINTP outsize, UINTP, UINTP, CHAR *workmem)
{
    zstd_params_t *zstd_params = (zstd_params_t *) workmem;
    if (!zstd_params || !zstd_params->dctx)
		return 0;

    return ZSTD_decompressDCtx(zstd_params->dctx, outbuf, outsize, 
							   inbuf, insize);
}
#endif
