/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011-2020, Yann Collet.
   Copyright (C) 2024, Advanced Micro Devices. All rights reserved.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
    - LZ4 homepage : http://www.lz4.org
    - LZ4 source repository : https://github.com/lz4/lz4
*/
#if AOCL_LZ4_COMPRESS_GENERIC_VARIANT == AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_BASE

    #define AOCL_LZ4_COMPRESS_GENERIC_FUNC AOCL_LZ4_compress_generic_validated

    #define AOCL_LZ4_CGV_ADDITIONAL_PARAMS /* none */

    #define AOCL_LZ4_CGV_INIT_START_BYTE \
            /* First Byte */ \
            AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base); \
            ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);

    #define AOCL_LZ4_CGV_INIT_IPDATA U32 ipData; 

    #define AOCL_LZ4_CGV_INIT_MATCHDATA auto U32 matchData;

    #define AOCL_LZ4_CGV_SET_IPDATA ipData=*(U32*)ip; 

    #define AOCL_LZ4_CGV_SET_MATCHDATA matchData=*(U32*)match; 

    #define AOCL_LZ4_CGV_CMP_IP_MATCH_DATA (matchData == ipData)

    #define AOCL_LZ4_CGV_CLEAR_HASH LZ4_clearHash

    #define AOCL_LZ4_CGV_CMP_IP_MATCH if (LZ4_read32(match) == LZ4_read32(ip))

    #define AOCL_LZ4_CGV_LAST_LITERALS /* do nothing */

    #define AOCL_LZ4_CGV_SET_RETURN_VALUES result = (int)(((char*)op) - dest); \
            assert(result > 0);

#elif AOCL_LZ4_COMPRESS_GENERIC_VARIANT == AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_FDS_NODICT

    #define AOCL_LZ4_COMPRESS_GENERIC_FUNC AOCL_LZ4_compress_generic_validated_FDS_noDict

    #define AOCL_LZ4_CGV_ADDITIONAL_PARAMS /* none */

    #define AOCL_LZ4_CGV_INIT_START_BYTE \
        assert(dictDirective == noDict); \
        if (tableType == byU32) AOCL_INITIALIZE_HASHTABLE_FOR_NODICT(cctx, U32, (1U << (LZ4_MEMORY_USAGE - 2))) \
        if (tableType == byU16) AOCL_INITIALIZE_HASHTABLE_FOR_NODICT(cctx, U16, (1U << (LZ4_MEMORY_USAGE - 1))) \
        /* Skip first two bytes */ \
        ip += 2; \
        /* Third Byte */ \
        AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base); \
        ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);

    #define AOCL_LZ4_CGV_INIT_IPDATA U64 ipDataFwd, ipDataBck; 

    #define AOCL_LZ4_CGV_INIT_MATCHDATA auto U64 matchDataFwd, matchDataBck;

    #define AOCL_LZ4_CGV_SET_IPDATA AOCL_READ_FWD_AND_BACK_BYTES(ip, ipDataFwd, ipDataBck)

    #define AOCL_LZ4_CGV_SET_MATCHDATA AOCL_READ_FWD_AND_BACK_BYTES(match, matchDataFwd, matchDataBck) 

    #define AOCL_LZ4_CGV_CMP_IP_MATCH_DATA (matchDataFwd == ipDataFwd || matchDataBck == ipDataBck)

    #define AOCL_LZ4_CGV_CLEAR_HASH AOCL_LZ4_clearHash

    #define AOCL_LZ4_CGV_LAST_LITERALS /* do nothing */

    #define AOCL_LZ4_CGV_SET_RETURN_VALUES result = (int)(((char*)op) - dest); \
            assert(result > 0);

    #define AOCL_LZ4_CGV_CMP_IP_MATCH \
            U64 ipDataFwd, ipDataBck; \
            auto U64 matchDataFwd, matchDataBck; \
            AOCL_READ_FWD_AND_BACK_BYTES(ip, ipDataFwd, ipDataBck) \
            AOCL_READ_FWD_AND_BACK_BYTES(match, matchDataFwd, matchDataBck) \
            if (matchDataFwd == ipDataFwd || matchDataBck == ipDataBck)

#elif AOCL_LZ4_COMPRESS_GENERIC_VARIANT == AOCL_LZ4_COMPRESS_GENERIC_VALIDATED_MT

    #define AOCL_LZ4_COMPRESS_GENERIC_FUNC AOCL_LZ4_compress_generic_validated_mt

    #define AOCL_LZ4_CGV_ADDITIONAL_PARAMS ,unsigned char** last_anchor_ptr \
            ,unsigned int* last_bytes_len

    #define AOCL_LZ4_CGV_INIT_START_BYTE \
            BYTE* dst_without_lastLiterals; \
            /* First Byte */ \
            AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base); \
            ip++; forwardH = AOCL_LZ4_hashPosition(ip, tableType);

    #define AOCL_LZ4_CGV_INIT_IPDATA U32 ipData; 

    #define AOCL_LZ4_CGV_INIT_MATCHDATA auto U32 matchData;

    #define AOCL_LZ4_CGV_SET_IPDATA ipData=*(U32*)ip; 

    #define AOCL_LZ4_CGV_SET_MATCHDATA matchData=*(U32*)match; 

    #define AOCL_LZ4_CGV_CMP_IP_MATCH_DATA (matchData == ipData)

    #define AOCL_LZ4_CGV_CLEAR_HASH LZ4_clearHash

    #define AOCL_LZ4_CGV_CMP_IP_MATCH if (LZ4_read32(match) == LZ4_read32(ip))

    #define AOCL_LZ4_CGV_LAST_LITERALS dst_without_lastLiterals = op;

    #define AOCL_LZ4_CGV_SET_RETURN_VALUES \
            if (last_bytes_len != NULL) \
            { \
                result = (int)(((char*)dst_without_lastLiterals) - dest); \
                *last_anchor_ptr = (BYTE*)anchor; /* src pointer until which compressed output is written : To support ST decompression on parallel compressed stream */ \
                *last_bytes_len = (size_t)(iend - anchor);/* length of src bytes pending for compression : To support ST decompression on parallel compressed stream */ \
                LOG_FORMATTED(INFO, logCtx, "Thread [id: %d] : result=%i, last_bytes_len=%i", omp_get_thread_num(), result, (int)(*last_bytes_len)); \
            } \
            else \
            { \
                result = (int)(((char*)op) - dest); \
                *last_anchor_ptr = (BYTE*)op; /* Write the complete commpressed chunk */ \
                /* *last_bytes_len = 0;//Last thread needs no joining with the next chunk */ \
                LOG_FORMATTED(INFO, logCtx, "Thread [id: %d] : result=%i", omp_get_thread_num(), result); \
            } \
            assert(result >= 0); /* result=0 when no match found, (all literals). */

#endif /* AOCL_LZ4_COMPRESS_GENERIC_VARIANT == AOCL_LZ4_compress_generic_validated_* */

#ifdef AOCL_LZ4_COMPRESS_GENERIC_VARIANT
/** AOCL_LZ4_COMPRESS_GENERIC_VARIANT() :
 *  inlined, to ensure branches are decided at compilation time.
 *  Presumed already validated at this stage:
 *  - source != NULL
 *  - inputSize > 0
 *  Implements AOCL optimized LZ4 compression
 *  With `noDict` mode, search range upper bound check is made to execute optimally after the match is found.
 *  The following optimizations will be included if the respective flags are enabled:
 * - AOCL_LZ4_OPT_PREFETCH_BACKWARDS    : Data access optimizations related to backward prefetching of data.
 * - AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1 : Uses larger base step size. [For next sequence `step` starts from `half of current step` instead of `1`.]
 * - AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2 : Aggressively sets search distance on top of strategy-1.
 * - AOCL_LZ4_NEW_PRIME_NUMBER          : New prime number for hashing.
 * - AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES  : Additional hash table updates to improve ratio.
 */
LZ4_FORCE_INLINE int AOCL_LZ4_COMPRESS_GENERIC_FUNC(
                 LZ4_stream_t_internal* const cctx,
                 const char* const source,
                 char* const dest,
                 const int inputSize,
                 int *inputConsumed, /* only written when outputDirective == fillOutput */
                 const int maxOutputSize,
                 const limitedOutput_directive outputDirective,
                 const tableType_t tableType,
                 const dict_directive dictDirective,
                 const dictIssue_directive dictIssue,
                 const int acceleration
                 AOCL_LZ4_CGV_ADDITIONAL_PARAMS)
{
    int result;
    const BYTE* ip = (const BYTE*) source;

    U32 const startIndex = cctx->currentOffset;
    const BYTE* base = (const BYTE*) source - startIndex;
    const BYTE* lowLimit;

    const LZ4_stream_t_internal* dictCtx = (const LZ4_stream_t_internal*) cctx->dictCtx;
    const BYTE* const dictionary =
        dictDirective == usingDictCtx ? dictCtx->dictionary : cctx->dictionary;
    const U32 dictSize =
        dictDirective == usingDictCtx ? dictCtx->dictSize : cctx->dictSize;
    const U32 dictDelta = (dictDirective == usingDictCtx) ? startIndex - dictCtx->currentOffset : 0;   /* make indexes in dictCtx comparable with index in current context */

    int const maybe_extMem = (dictDirective == usingExtDict) || (dictDirective == usingDictCtx);
    U32 const prefixIdxLimit = startIndex - dictSize;   /* used when dictDirective == dictSmall */
    const BYTE* const dictEnd = dictionary ? dictionary + dictSize : dictionary;
    const BYTE* anchor = (const BYTE*) source;
    const BYTE* const iend = ip + inputSize;
    const BYTE* const mflimitPlusOne = iend - MFLIMIT + 1;
    const BYTE* const matchlimit = iend - LASTLITERALS;

    /* the dictCtx currentOffset is indexed on the start of the dictionary,
     * while a dictionary in the current context precedes the currentOffset */
    const BYTE* dictBase = (dictionary == NULL) ? NULL :
                           (dictDirective == usingDictCtx) ?
                            dictionary + dictSize - dictCtx->currentOffset :
                            dictionary + dictSize - startIndex;

    BYTE* op = (BYTE*) dest;
    BYTE* const olimit = op + maxOutputSize;

    U32 offset = 0;
    U32 forwardH;

    LOG_FORMATTED(INFO, logCtx, "srcSize=%i, maxOutputSize=%i tableType=%u", inputSize, maxOutputSize, tableType);
    DEBUGLOG(5, __FUNCTION__": srcSize=%i, tableType=%u", inputSize, tableType);
    assert(ip != NULL);
    /* If init conditions are not met, we don't have to mark stream
     * as having dirty context, since no action was taken yet */
    if (outputDirective == fillOutput && maxOutputSize < 1) { return 0; } /* Impossible to store anything */
    if ((tableType == byU16) && (inputSize>=LZ4_64Klimit)) { return 0; }  /* Size too large (not within 64K limit) */
    if (tableType==byPtr) assert(dictDirective==noDict);      /* only supported use case with byPtr */
    assert(acceleration >= 1);

    lowLimit = (const BYTE*)source - (dictDirective == withPrefix64k ? dictSize : 0);

    /* Update context state */
    if (dictDirective == usingDictCtx) {
        /* Subsequent linked blocks can't use the dictionary. */
        /* Instead, they use the block we just compressed. */
        cctx->dictCtx = NULL;
        cctx->dictSize = (U32)inputSize;
    } else {
        cctx->dictSize += (U32)inputSize;
    }
    cctx->currentOffset += (U32)inputSize;
    cctx->tableType = (U32)tableType;

    if (inputSize < LZ4_minLength) 
        goto _last_literals;        /* Input too small, no compression (all literals) */

    AOCL_LZ4_CGV_INIT_START_BYTE

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
    int prevStep = 0;
    int presetMatchNb = 0;
#endif
#if defined(__clang__) && (__clang_major__ > 16) && defined(AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2)
    /* Alignment for clang version 17 and above when `AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2` is enabled. */
    __asm__(".p2align 6");
#endif
    /* Main Loop */
    for ( ; ; ) {
        const BYTE* match;
        BYTE* token;
        const BYTE* filledIp;
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
        typedef union { reg_t u; BYTE c[8]; } vecInt;
        vecInt ipPrevData;
        int prevOffset = 0;
#endif

        /* Find a match */
        if (tableType == byPtr) {
            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                match = LZ4_getPositionOnHash(h, cctx->hashTable, tableType, base);
                forwardH = AOCL_LZ4_hashPosition(forwardIp, tableType);
                LZ4_putPositionOnHash(ip, h, cctx->hashTable, tableType, base);

            } while ( (match+LZ4_DISTANCE_MAX < ip)
                   || (LZ4_read32(match) != LZ4_read32(ip)) );

        } else {   /* byU32, byU16 */

            const BYTE* forwardIp = ip;
            int step = 1;
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
            int searchMatchNb = acceleration << (LZ4_skipTrigger-presetMatchNb);
#else
            int searchMatchNb = acceleration << LZ4_skipTrigger;
#endif

            AOCL_LZ4_CGV_INIT_IPDATA
#if defined(__clang__) && (__clang_major__ > 16) && defined(AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2)
            /* Alignment for clang version 17 and above when `AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2` is enabled. */
            __asm__(".p2align 3");
#endif
            do {
                U32 const h = forwardH;
                U32 const current = (U32)(forwardIp - base);
                U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
                AOCL_LZ4_CGV_INIT_MATCHDATA

                assert(matchIndex <= current);
                assert(forwardIp - base < (ptrdiff_t)(2 GB - 1));
                ip = forwardIp;
                forwardIp += step;

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
                step = (searchMatchNb++ >> (LZ4_skipTrigger-presetMatchNb)) + prevStep;
#else
                step = (searchMatchNb++ >> LZ4_skipTrigger);
#endif

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                if (dictDirective == usingDictCtx) {
                    if (matchIndex < startIndex) {
                        /* there was no match, try the dictionary */
                        assert(tableType == byU32);
                        matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                        match = dictBase + matchIndex;
                        matchIndex += dictDelta;   /* make dictCtx index comparable with current context */
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else if (dictDirective==usingExtDict) {
                    if (matchIndex < startIndex) {
                        DEBUGLOG(7, "extDict candidate: matchIndex=%5u  <  startIndex=%5u", matchIndex, startIndex);
                        assert(startIndex - matchIndex >= MINMATCH);
                        assert(dictBase);
                        match = dictBase + matchIndex;
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else {   /* single continuous memory segment */
                    match = base + matchIndex;
                }
                AOCL_LZ4_CGV_SET_IPDATA

#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
                prevOffset = ((ip - anchor) > 8) ? 8 : (ip - anchor);
#endif
                forwardH = AOCL_LZ4_hashPosition(forwardIp, tableType);
                LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);

                DEBUGLOG(7, "candidate at pos=%u  (offset=%u \n", matchIndex, current - matchIndex);
                if ((dictIssue == dictSmall) && (matchIndex < prefixIdxLimit)) { continue; }    /* match outside of valid area */
                assert(matchIndex < current);

                /* For cases when `dictDirective == noDict`, search range upper bound check is made to execute optimally after the match is found. */
                if ((dictDirective > noDict) && ((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
                    && (matchIndex+LZ4_DISTANCE_MAX < current)) {
                    continue;
                } /* too far */

                AOCL_LZ4_CGV_SET_MATCHDATA

                if (AOCL_LZ4_CGV_CMP_IP_MATCH_DATA) {
                    /* The below conditional is moved inside `if(matchData == ipData)` for performance improvement */
                    if ((dictDirective == noDict) && ((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
                        && (matchIndex+LZ4_DISTANCE_MAX < current)) {
                        continue;
                    } /* too far */
                    assert((current - matchIndex) <= LZ4_DISTANCE_MAX);  /* match now expected within distance */
                    LOG_FORMATTED(DEBUG, logCtx, "candidate at pos=%u  (offset=%u", matchIndex, current - matchIndex);

#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
                    ipPrevData.u = *(reg_t*)(ip - prevOffset);
#endif
                    if (maybe_extMem) offset = current - matchIndex;

#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT1
                    if (step > AOCL_LZ4_MATCH_SKIPPING_THRESHOLD) {
                        prevStep = (step / 2) - 1 ;   /* for the next sequence `step` starts from `half of current step` instead of 1. */
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2
                        presetMatchNb = 1;
#endif
                    } else {
                        prevStep = 0;                 /* for the next sequence `step` starts from 1. */
#ifdef AOCL_LZ4_MATCH_SKIP_OPT_LDS_STRAT2
                        presetMatchNb = 0;
#endif
                    }
#endif

#ifdef AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES

                    /* Hash and update hash table with indexes of ip+1, ip+2 and ip+3.
                       This results in storing additional potential matches which improves
                       compression ratio. Recommended for higher compressibility use cases.
                    */
                    U32 next_h = AOCL_LZ4_hashPosition(ip+1, tableType);
                    LZ4_putIndexOnHash(current+1, next_h, cctx->hashTable, tableType);

                    next_h = AOCL_LZ4_hashPosition(ip+2, tableType);
                    LZ4_putIndexOnHash(current+2, next_h, cctx->hashTable, tableType);

                    next_h = AOCL_LZ4_hashPosition(ip+3, tableType);
                    LZ4_putIndexOnHash(current+3, next_h, cctx->hashTable, tableType);

#endif /* AOCL_LZ4_EXTRA_HASH_TABLE_UPDATES */

                    break;   /* match found */
                }

            } while(1);
        }

        /* Catch up */
        filledIp = ip;
#ifdef AOCL_LZ4_DATA_ACCESS_OPT_PREFETCH_BACKWARDS
        prevOffset--;
        while ((prevOffset > -1) && ((ip>anchor) & (match > lowLimit)) && (unlikely(ipPrevData.c[prevOffset]==match[-1])))
        {
            ip--; match--; prevOffset--;
        }
        while (((ip>anchor) & (match > lowLimit)) && (unlikely(ip[-1]==match[-1])))
        {
            ip--; match--; 
        }
#else
        while (((ip > anchor) & (match > lowLimit)) && (unlikely(ip[-1] == match[-1]))) { ip--; match--; }
#endif

        /* Encode Literals */
        {   unsigned const litLength = (unsigned)(ip - anchor);
            token = op++;
            if ((outputDirective == limitedOutput) &&  /* Check output buffer overflow */
                (unlikely(op + litLength + (2 + 1 + LASTLITERALS) + (litLength/255) > olimit)) ) {
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
            if ((outputDirective == fillOutput) &&
                (unlikely(op + (litLength+240)/255 /* litlen */ + litLength /* literals */ + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit))) {
                op--;
                goto _last_literals;
            }
            if (litLength >= RUN_MASK) {
                int len = (int)(litLength - RUN_MASK);
                *token = (RUN_MASK<<ML_BITS);
                for(; len >= 255 ; len-=255) *op++ = 255;
                *op++ = (BYTE)len;
            }
            else *token = (BYTE)(litLength<<ML_BITS);

            /* Copy Literals */
            LZ4_wildCopy8(op, anchor, op+litLength);
            op+=litLength;
            LOG_FORMATTED(DEBUG, logCtx, "seq.start:%i, literals=%u, match.start:%i",
                        (int)(anchor-(const BYTE*)source), litLength, (int)(ip-(const BYTE*)source));
            DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                        (int)(anchor-(const BYTE*)source), litLength, (int)(ip-(const BYTE*)source));
        }

_next_match:
        /* at this stage, the following variables must be correctly set :
         * - ip : at start of LZ operation
         * - match : at start of previous pattern occurrence; can be within current prefix, or within extDict
         * - offset : if maybe_ext_memSegment==1 (constant)
         * - lowLimit : must be == dictionary to mean "match is within extDict"; must be == source otherwise
         * - token and *token : position to write 4-bits for match length; higher 4-bits for literal length supposed already written
         */

        if ((outputDirective == fillOutput) &&
            (op + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit)) {
            /* the match was too close to the end, rewind and go to last literals */
            op = token;
            goto _last_literals;
        }

        /* Encode Offset */
        if (maybe_extMem) {   /* static test */
            DEBUGLOG(6, "             with offset=%u  (ext if > %i)", offset, (int)(ip - (const BYTE*)source));
            assert(offset <= LZ4_DISTANCE_MAX && offset > 0);
            LZ4_writeLE16(op, (U16)offset); op+=2;
        } else  {
            LOG_FORMATTED(DEBUG, logCtx, "             with offset=%u  (same segment)", (U32)(ip - match));
            DEBUGLOG(6, "             with offset=%u  (same segment)", (U32)(ip - match));
            assert(ip-match <= LZ4_DISTANCE_MAX);
            LZ4_writeLE16(op, (U16)(ip - match)); op+=2;
        }

        /* Encode MatchLength */
        {   unsigned matchCode;

            if ( (dictDirective==usingExtDict || dictDirective==usingDictCtx)
              && (lowLimit==dictionary) /* match within extDict */ ) {
                const BYTE* limit = ip + (dictEnd-match);
                assert(dictEnd > match);
                if (limit > matchlimit) limit = matchlimit;
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, limit);
                ip += (size_t)matchCode + MINMATCH;
                if (ip==limit) {
                    unsigned const more = LZ4_count(limit, (const BYTE*)source, matchlimit);
                    matchCode += more;
                    ip += more;
                }
                DEBUGLOG(6, "             with matchLength=%u starting in extDict", matchCode+MINMATCH);
            } else {
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, matchlimit);
                ip += (size_t)matchCode + MINMATCH;
                LOG_FORMATTED(DEBUG, logCtx, "             with matchLength=%u", matchCode+MINMATCH);
                DEBUGLOG(6, "             with matchLength=%u", matchCode+MINMATCH);
            }

            if ((outputDirective) &&    /* Check output buffer overflow */
                (unlikely(op + (1 + LASTLITERALS) + (matchCode+240)/255 > olimit)) ) {
                if (outputDirective == fillOutput) {
                    /* Match description too long : reduce it */
                    U32 newMatchCode = 15 /* in token */ - 1 /* to avoid needing a zero byte */ + ((U32)(olimit - op) - 1 - LASTLITERALS) * 255;
                    ip -= matchCode - newMatchCode;
                    assert(newMatchCode < matchCode);
                    matchCode = newMatchCode;
                    if (unlikely(ip <= filledIp)) {
                        /* We have already filled up to filledIp so if ip ends up less than filledIp
                         * we have positions in the hash table beyond the current position. This is
                         * a problem if we reuse the hash table. So we have to remove these positions
                         * from the hash table.
                         */
                        const BYTE* ptr;
                        DEBUGLOG(5, "Clearing %u positions", (U32)(filledIp - ip));
                        for (ptr = ip; ptr <= filledIp; ++ptr) {
                            U32 const h = AOCL_LZ4_hashPosition(ptr, tableType);
                            AOCL_LZ4_CGV_CLEAR_HASH(h, cctx->hashTable, tableType);
                        }
                    }
                } else {
                    assert(outputDirective == limitedOutput);
                    return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
                }
            }
            if (matchCode >= ML_MASK) {
                *token += ML_MASK;
                matchCode -= ML_MASK;
                LZ4_write32(op, 0xFFFFFFFF);
                while (matchCode >= 4*255) {
                    op+=4;
                    LZ4_write32(op, 0xFFFFFFFF);
                    matchCode -= 4*255;
                }
                op += matchCode / 255;
                *op++ = (BYTE)(matchCode % 255);
            } else
                *token += (BYTE)(matchCode);
        }
        /* Ensure we have enough space for the last literals. */
        assert(!(outputDirective == fillOutput && op + 1 + LASTLITERALS > olimit));

        anchor = ip;

        /* Test end of chunk */
        if (ip >= mflimitPlusOne) break;

        /* Fill table */
        AOCL_LZ4_putPosition(ip-2, cctx->hashTable, tableType, base);

        /* Test next position */
        if (tableType == byPtr) {

            match = AOCL_LZ4_getPosition(ip, cctx->hashTable, tableType, base);
            AOCL_LZ4_putPosition(ip, cctx->hashTable, tableType, base);
            if ( (match+LZ4_DISTANCE_MAX >= ip)
              && (LZ4_read32(match) == LZ4_read32(ip)) )
            { token=op++; *token=0; goto _next_match; }

        } else {   /* byU32, byU16 */

            U32 const h = AOCL_LZ4_hashPosition(ip, tableType);
            U32 const current = (U32)(ip-base);
            U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (dictDirective == usingDictCtx) {
                if (matchIndex < startIndex) {
                    /* there was no match, try the dictionary */
                    matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                    matchIndex += dictDelta;
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;  /* required for match length counter */
                }
            } else if (dictDirective==usingExtDict) {
                if (matchIndex < startIndex) {
                    assert(dictBase);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;   /* required for match length counter */
                }
            } else {   /* single memory segment */
                match = base + matchIndex;
            }
            LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);
            assert(matchIndex < current);

            if ( ((dictIssue==dictSmall) ? (matchIndex >= prefixIdxLimit) : 1)
              && (((tableType==byU16) && (LZ4_DISTANCE_MAX == LZ4_DISTANCE_ABSOLUTE_MAX)) ? 1 : (matchIndex+LZ4_DISTANCE_MAX >= current))) {
                AOCL_LZ4_CGV_CMP_IP_MATCH 
                {
                    token=op++;
                    *token=0;
                    if (maybe_extMem) offset = current - matchIndex;
                    LOG_FORMATTED(DEBUG, logCtx, "seq.start:%i, literals=%u, match.start:%i",
                                (int)(anchor-(const BYTE*)source), 0, (int)(ip-(const BYTE*)source));
                    DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                                (int)(anchor-(const BYTE*)source), 0, (int)(ip-(const BYTE*)source));
                    goto _next_match;
                }
            }
        }

        /* Prepare next loop */
        forwardH = AOCL_LZ4_hashPosition(++ip, tableType);

    }

_last_literals:
    AOCL_LZ4_CGV_LAST_LITERALS
    /* Encode Last Literals */
    {   size_t lastRun = (size_t)(iend - anchor);
        if ( (outputDirective) &&  /* Check output buffer overflow */
            (op + lastRun + 1 + ((lastRun+255-RUN_MASK)/255) > olimit)) {
            if (outputDirective == fillOutput) {
                /* adapt lastRun to fill 'dst' */
                assert(olimit >= op);
                lastRun  = (size_t)(olimit-op) - 1/*token*/;
                lastRun -= (lastRun + 256 - RUN_MASK) / 256;  /*additional length tokens*/
            } else {
                assert(outputDirective == limitedOutput);
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
        }
        LOG_FORMATTED(DEBUG, logCtx, "Final literal run : %i literals", (int)lastRun);
        DEBUGLOG(6, "Final literal run : %i literals", (int)lastRun);
        if (lastRun >= RUN_MASK) {
            size_t accumulator = lastRun - RUN_MASK;
            *op++ = RUN_MASK << ML_BITS;
            for(; accumulator >= 255 ; accumulator-=255) *op++ = 255;
            *op++ = (BYTE) accumulator;
        } else {
            *op++ = (BYTE)(lastRun<<ML_BITS);
        }
        LZ4_memcpy(op, anchor, lastRun);
        ip = anchor + lastRun;
        op += lastRun;
    }

    if (outputDirective == fillOutput) {
        *inputConsumed = (int) (((const char*)ip)-source);
    }

    AOCL_LZ4_CGV_SET_RETURN_VALUES
    
    LOG_FORMATTED(INFO, logCtx, "Compressed %i bytes into %i bytes", inputSize, result);
    DEBUGLOG(5, __FUNCTION__": compressed %i bytes into %i bytes", inputSize, result);
    return result;
}
#endif

#undef AOCL_LZ4_COMPRESS_GENERIC_VARIANT
#undef AOCL_LZ4_COMPRESS_GENERIC_FUNC
#undef AOCL_LZ4_CGV_ADDITIONAL_PARAMS
#undef AOCL_LZ4_CGV_INIT_START_BYTE
#undef AOCL_LZ4_CGV_INIT_IPDATA
#undef AOCL_LZ4_CGV_INIT_MATCHDATA
#undef AOCL_LZ4_CGV_SET_IPDATA
#undef AOCL_LZ4_CGV_SET_MATCHDATA
#undef AOCL_LZ4_CGV_CMP_IP_MATCH_DATA
#undef AOCL_LZ4_CGV_CLEAR_HASH
#undef AOCL_LZ4_CGV_CMP_IP_MATCH
#undef AOCL_LZ4_CGV_LAST_LITERALS
#undef AOCL_LZ4_CGV_SET_RETURN_VALUES


#if AOCL_LZ4_DECOMPRESS_GENERIC_VARIANT == AOCL_LZ4_DECOMPRESS_GENERIC_BASE

    #define AOCL_LZ4_DECOMPRESS_GENERIC_FUNC AOCL_LZ4_decompress_generic

    #define AOCL_LZ4_DGV_ADDITIONAL_PARAMS /* none */

    #define AOCL_LZ4_DGV_LOG_UNFORMATTED(logType, logCtx, str) \
            LOG_UNFORMATTED(logType, logCtx, str)

    #define AOCL_LZ4_DGV_LOG_FORMATTED(logType, logCtx, str, ...)  \
            LOG_FORMATTED(logType, logCtx, str, ...)

    #define AOCL_LZ4_DGV_LONG_MATCH \
            size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0); \
            if (addl == rvl_error) { \
                LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional match length."); \
                goto _output_error; \
            } \
            length += addl;

    #define AOCL_LZ4_DGV_LAST_SEQ_CHECK \
            if ((ip+length != iend) || (cpy > oend)) { \
                LOG_FORMATTED(ERR, logCtx, "Must be the last (or invalid) sequence because of the parsing limitations. Error, %s.", \
                (ip+length != iend) ? "exact input not consumed" : "output buffer overflow" );

    #define AOCL_LZ4_DGV_EOF_CHECK (!partialDecoding || (cpy == oend) || (ip >= (iend-2)))

    #define AOCL_LZ4_DGV_COPY_MATCH \
            size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0); \
            if (addl == rvl_error) { \
                LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional match length."); \
                goto _output_error; \
            } \
            length += addl;

    #define AOCL_LZ4_DGV_SAFE_DECODE_EXIT /* nothing */

#elif AOCL_LZ4_DECOMPRESS_GENERIC_VARIANT == AOCL_LZ4_DECOMPRESS_GENERIC_MT

    #define AOCL_LZ4_DECOMPRESS_GENERIC_FUNC AOCL_LZ4_decompress_generic_mt

    #define AOCL_LZ4_DGV_ADDITIONAL_PARAMS ,int is_last_thread

    #define AOCL_LZ4_DGV_LOG_PREFIX "Thread [id: %d] : "

    #define AOCL_LZ4_DGV_LOG_UNFORMATTED(logType, logCtx, str) \
            LOG_FORMATTED(logType, logCtx, AOCL_LZ4_DGV_LOG_PREFIX str, omp_get_thread_num())

    #define AOCL_LZ4_DGV_LOG_FORMATTED(logType, logCtx, str, ...)  \
            LOG_FORMATTED(logType, logCtx, AOCL_LZ4_DGV_LOG_PREFIX str, omp_get_thread_num(), ...)

    #define AOCL_LZ4_DGV_LONG_MATCH \
            if (is_last_thread) { \
                size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0); \
                if (addl == rvl_error) {  \
                    LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX"Encountered variable_length_error while decoding additional match length.", omp_get_thread_num()); \
                    goto _output_error;  \
                } \
                length += addl; \
            } else { \
                size_t const addl = read_variable_length(&ip, iend + 1, 0); \
                if (addl == rvl_error) { \
                    LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX"Encountered variable_length_error while decoding additional match length.", omp_get_thread_num()); \
                    goto _output_error;  \
                } \
                length += addl; \
            }

    #define AOCL_LZ4_DGV_LAST_SEQ_CHECK \
            if ((is_last_thread && (ip + length != iend)) || (cpy > oend)) { \
                LOG_FORMATTED(ERR, logCtx, AOCL_LZ4_DGV_LOG_PREFIX"Must be the last (or invalid) sequence because of the parsing limitations. Error, %s.", \
                omp_get_thread_num(), (is_last_thread && (ip + length != iend)) ? "exact input not consumed" : "output buffer overflow");

    #define AOCL_LZ4_DGV_EOF_CHECK ((is_last_thread && !partialDecoding) || (cpy == oend) || (is_last_thread && (ip >= (iend - 2))))

    #define AOCL_LZ4_DGV_COPY_MATCH \
            if (is_last_thread) { \
                size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0); \
                if (addl == rvl_error) { goto _output_error; } \
                length += addl; \
            } \
            else { \
                size_t const addl = read_variable_length(&ip, iend + 1, 0); \
                if (addl == rvl_error) { goto _output_error; } \
                length += addl; \
            }

    #define AOCL_LZ4_DGV_SAFE_DECODE_EXIT \
            if ((cpy == oend) || (ip >= iend)) { \
                break; \
            }

#endif

#ifdef AOCL_LZ4_DECOMPRESS_GENERIC_VARIANT
/*! AOCL_LZ4_DECOMPRESS_GENERIC_VARIANT() :
 *  This generic decompression function covers all use cases.
 *  It shall be instantiated several times, using different sets of directives.
 *  Note that it is important for performance that this function really get inlined,
 *  in order to remove useless branches during compilation optimization.
 *  
 *  Same as LZ4_decompress_generic(), but calls `AOCL_LZ4_wildCopy64_AVX()` instead 
 *  of `LZ4_wildCopy32()` to copy
 *  - literals (when endCondition_directive is endOnInput and literal length >= 15), 
 *  - matched characters (when offset>=32)
 */
LZ4_FORCE_INLINE int
AOCL_LZ4_DECOMPRESS_GENERIC_FUNC(
                 const char* const src,
                 char* const dst,
                 int srcSize,
                 int outputSize,         /* If endOnInput==endOnInputSize, this value is `dstCapacity` */

                 earlyEnd_directive partialDecoding,  /* full, partial */
                 dict_directive dict,                 /* noDict, withPrefix64k, usingExtDict */
                 const BYTE* const lowPrefix,  /* always <= dst, == dst when no prefix */
                 const BYTE* const dictStart,  /* only if dict==usingExtDict */
                 const size_t dictSize         /* note : = 0 if noDict */
                 AOCL_LZ4_DGV_ADDITIONAL_PARAMS)
{
    if ((src == NULL) || (outputSize < 0))
    {
        AOCL_LZ4_DGV_LOG_UNFORMATTED(ERR, logCtx, "Invalid input");
        return -1;
    }

    {   const BYTE* ip = (const BYTE*) src;
        const BYTE* const iend = ip + srcSize;

        BYTE* op = (BYTE*) dst;
        BYTE* const oend = op + outputSize;
        BYTE* cpy;

        const BYTE* const dictEnd = (dictStart == NULL) ? NULL : dictStart + dictSize;

        const int checkOffset = (dictSize < (int)(64 KB));


        /* Set up the "end" pointers for the shortcut. */
        const BYTE* const shortiend = iend - 14 /*maxLL*/ - 2 /*offset*/;
        const BYTE* const shortoend = oend - 14 /*maxLL*/ - 18 /*maxML*/;

        const BYTE* match;
        size_t offset;
        unsigned token;
        size_t length;

        AOCL_LZ4_DGV_LOG_FORMATTED(INFO, logCtx, "srcSize:%i, dstSize:%i", srcSize, outputSize);
        DEBUGLOG(5, __FUNCTION__" (srcSize:%i, dstSize:%i)", srcSize, outputSize);

        /* Special cases */
        assert(lowPrefix <= op);
        if (unlikely(outputSize==0)) {
            /* Empty output buffer */
            if (partialDecoding) return 0;
            return ((srcSize==1) && (*ip==0)) ? 0 : -1;
        }
        if (unlikely(srcSize==0)) { return -1; }

	/* LZ4_FAST_DEC_LOOP:
     * designed for modern OoO performance cpus,
     * where copying reliably 32-bytes is preferable to an unpredictable branch.
     * note : fast loop may show a regression for some client arm chips. */
#if LZ4_FAST_DEC_LOOP
        if ((oend - op) < FASTLOOP_SAFE_DISTANCE) {
            AOCL_LZ4_DGV_LOG_UNFORMATTED(INFO, logCtx, "skip fast decode loop");
            DEBUGLOG(6, "skip fast decode loop");
            goto safe_decode;
        }

        /* Fast loop : decode sequences as long as output < oend-FASTLOOP_SAFE_DISTANCE */
        while (1) {
            /* Main fastloop assertion: We can always wildcopy FASTLOOP_SAFE_DISTANCE */
            assert(oend - op >= FASTLOOP_SAFE_DISTANCE);
            assert(ip < iend);
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            /* decode literal length */
            if (length == RUN_MASK) {
                size_t const addl = read_variable_length(&ip, iend-RUN_MASK, 1);
                if (addl == rvl_error){ 
                    AOCL_LZ4_DGV_LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional literal length.");
                    goto _output_error; 
                }
                length += addl;
                if (unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if (unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */

                /* copy literals */
                cpy = op+length;
                LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
                if ((cpy>oend-64) || (ip+length>iend-64)) { goto safe_literal_copy; }
                AOCL_LZ4_wildCopy64_AVX(op, ip, cpy);
                ip += length; op = cpy;
            } else {
                cpy = op+length;
                DEBUGLOG(7, "copy %u bytes in a 16-bytes stripe", (unsigned)length);
                /* We don't need to check oend, since we check it once for each loop below */
                if (ip > iend-(16 + 1/*max lit + offset + nextToken*/)) { goto safe_literal_copy; }
                /* Literals can only be <= 14, but hope compilers optimize better when copy by a register size */
                LZ4_memcpy(op, ip, 16);
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;
            assert(match <= op);  /* overflow check */

            /* get matchlength */
            length = token & ML_MASK;

            if (length == ML_MASK) {
                AOCL_LZ4_DGV_LONG_MATCH
                length += MINMATCH;
                if (unlikely((uptrval)(op)+length<(uptrval)op)) { goto _output_error; } /* overflow detection */
                if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }
            } else {
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }

                /* Fastpath check: skip LZ4_wildCopy32 when true */
                if ((dict == withPrefix64k) || (match >= lowPrefix)) {
                    if (offset >= 8) {
                        assert(match >= lowPrefix);
                        assert(match <= op);
                        assert(op + 18 <= oend);

                        LZ4_memcpy(op, match, 8);
                        LZ4_memcpy(op+8, match+8, 8);
                        LZ4_memcpy(op+16, match+16, 2);
                        op += length;
                        continue;
            }   }   }

            if (checkOffset && (unlikely(match + dictSize < lowPrefix))) { 
                AOCL_LZ4_DGV_LOG_UNFORMATTED(ERR, logCtx, "Offset outside buffers.");
                goto _output_error; 
            } /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                assert(dictEnd != NULL);
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) {
                        DEBUGLOG(7, "partialDecoding: dictionary match, close to dstEnd");
                        length = MIN(length, (size_t)(oend-op));
                    } else {
                        goto _output_error;  /* end-of-block condition violated */
                }   }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    LZ4_memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) { *op++ = *copyFrom++; }
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }

            /* copy match within block */
            cpy = op + length;

            assert((op <= oend) && (oend-op >= 32));

            if (offset >= 32){
                AOCL_LZ4_wildCopy64_AVX(op, match, cpy);
                op = cpy;
                continue;
            }

            if (unlikely(offset<16)) {
                LZ4_memcpy_using_offset(op, match, cpy, offset);
            } else {
                LZ4_wildCopy32(op, match, cpy);
            }

            op = cpy;   /* wildcopy correction */
        }
    safe_decode:
#endif

        /* Main Loop : decode remaining sequences where output < FASTLOOP_SAFE_DISTANCE */
        while (1) {
            assert(ip < iend);
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            /* A two-stage shortcut for the most common case:
             * 1) If the literal length is 0..14, and there is enough space,
             * enter the shortcut and copy 16 bytes on behalf of the literals
             * (in the fast mode, only 8 bytes can be safely copied this way).
             * 2) Further if the match length is 4..18, copy 18 bytes in a similar
             * manner; but we ensure that there's enough space in the output for
             * those 18 bytes earlier, upon entering the shortcut (in other words,
             * there is a combined check for both stages).
             */
            if ( (length != RUN_MASK)
                /* strictly "less than" on input, to re-enter the loop with at least one byte */
              && likely((ip < shortiend) & (op <= shortoend)) ) {
                /* Copy the literals */
                LZ4_memcpy(op, ip, 16);
                op += length; ip += length;

                /* The second stage: prepare for match copying, decode full info.
                 * If it doesn't work out, the info won't be wasted. */
                length = token & ML_MASK; /* match length */
                offset = LZ4_readLE16(ip); ip += 2;
                match = op - offset;
                assert(match <= op); /* check overflow */

                /* Do not deal with overlapping matches. */
                if ( (length != ML_MASK)
                  && (offset >= 8)
                  && (dict==withPrefix64k || match >= lowPrefix) ) {
                    /* Copy the match. */
                    LZ4_memcpy(op + 0, match + 0, 8);
                    LZ4_memcpy(op + 8, match + 8, 8);
                    LZ4_memcpy(op +16, match +16, 2);
                    op += length + MINMATCH;
                    /* Both stages worked, load the next token. */
                    continue;
                }

                /* The second stage didn't work out, but the info is ready.
                 * Propel it right to the point of match copying. */
                goto _copy_match;
            }

            /* decode literal length */
            if (length == RUN_MASK) {
                size_t const addl = read_variable_length(&ip, iend-RUN_MASK, 1);
                if (addl == rvl_error) { 
                    AOCL_LZ4_DGV_LOG_UNFORMATTED(ERR, logCtx, "Encountered variable_length_error while decoding additional literal length.");
                    goto _output_error; }
                length += addl;
                if (unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if (unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */
            }

            /* copy literals */
            cpy = op+length;
#if LZ4_FAST_DEC_LOOP
        safe_literal_copy:
#endif
            LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
            if ((cpy>oend-MFLIMIT) || (ip+length>iend-(2+1+LASTLITERALS))) {
                /* We've either hit the input parsing restriction or the output parsing restriction.
                 * In the normal scenario, decoding a full block, it must be the last sequence,
                 * otherwise it's an error (invalid input or dimensions).
                 * In partialDecoding scenario, it's necessary to ensure there is no buffer overflow.
                 */
                if (partialDecoding) {
                    /* Since we are partial decoding we may be in this block because of the output parsing
                     * restriction, which is not valid since the output buffer is allowed to be undersized.
                     */
                    DEBUGLOG(7, "partialDecoding: copying literals, close to input or output end")
                    DEBUGLOG(7, "partialDecoding: literal length = %u", (unsigned)length);
                    DEBUGLOG(7, "partialDecoding: remaining space in dstBuffer : %i", (int)(oend - op));
                    DEBUGLOG(7, "partialDecoding: remaining space in srcBuffer : %i", (int)(iend - ip));
                    /* Finishing in the middle of a literals segment,
                     * due to lack of input.
                     */
                    if (ip+length > iend) {
                        length = (size_t)(iend-ip);
                        cpy = op + length;
                    }
                    /* Finishing in the middle of a literals segment,
                     * due to lack of output space.
                     */
                    if (cpy > oend) {
                        cpy = oend;
                        assert(op<=oend);
                        length = (size_t)(oend-op);
                    }
                } else {
                     /* We must be on the last sequence (or invalid) because of the parsing limitations
                      * so check that we exactly consume the input and don't overrun the output buffer.
                      */
                    AOCL_LZ4_DGV_LAST_SEQ_CHECK
                        DEBUGLOG(6, "should have been last run of literals")
                        DEBUGLOG(6, "ip(%p) + length(%i) = %p != iend (%p)", (void *)ip, (int)length, (void *)(ip+length), (void *)iend);
                        DEBUGLOG(6, "or cpy(%p) > oend(%p)", (void *)cpy, (void *)oend);
                        goto _output_error;
                    }
                }
                LZ4_memmove(op, ip, length);  /* supports overlapping memory regions; only matters for in-place decompression scenarios */
                ip += length;
                op += length;
                /* Necessarily EOF when !partialDecoding.
                 * When partialDecoding, it is EOF if we've either
                 * filled the output buffer or
                 * can't proceed with reading an offset for following match.
                 */
                if (AOCL_LZ4_DGV_EOF_CHECK) {
                    break;
                }
            } else {
                LZ4_wildCopy8(op, ip, cpy);   /* can overwrite up to 8 beyond cpy */
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;

            /* get matchlength */
            length = token & ML_MASK;

    _copy_match:
            if (length == ML_MASK) {
                AOCL_LZ4_DGV_COPY_MATCH
                if (unlikely((uptrval)(op)+length<(uptrval)op)) goto _output_error;   /* overflow detection */
            }
            length += MINMATCH;

#if LZ4_FAST_DEC_LOOP
        safe_match_copy:
#endif
            if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) {
                AOCL_LZ4_DGV_LOG_UNFORMATTED(ERR, logCtx, "Offset outside buffers.");
                goto _output_error;
            }   /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                assert(dictEnd != NULL);
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) length = MIN(length, (size_t)(oend-op));
                    else goto _output_error;   /* doesn't respect parsing restriction */
                }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    LZ4_memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) *op++ = *copyFrom++;
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }
            assert(match >= lowPrefix);

            /* copy match within block */
            cpy = op + length;

            /* partialDecoding : may end anywhere within the block */
            assert(op<=oend);
            if (partialDecoding && (cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                size_t const mlen = MIN(length, (size_t)(oend-op));
                const BYTE* const matchEnd = match + mlen;
                BYTE* const copyEnd = op + mlen;
                if (matchEnd > op) {   /* overlap copy */
                    while (op < copyEnd) { *op++ = *match++; }
                } else {
                    LZ4_memcpy(op, match, mlen);
                }
                op = copyEnd;
                if (op == oend) { break; }
                continue;
            }

            if (unlikely(offset<8)) {
                LZ4_write32(op, 0);   /* silence msan warning when offset==0 */
                op[0] = match[0];
                op[1] = match[1];
                op[2] = match[2];
                op[3] = match[3];
                match += inc32table[offset];
                LZ4_memcpy(op+4, match, 4);
                match -= dec64table[offset];
            } else {
                LZ4_memcpy(op, match, 8);
                match += 8;
            }
            op += 8;

            if (unlikely(cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                BYTE* const oCopyLimit = oend - (WILDCOPYLENGTH-1);
                if (cpy > oend-LASTLITERALS) { 
                    AOCL_LZ4_DGV_LOG_UNFORMATTED(ERR, logCtx, "Last LASTLITERALS bytes must be uncompressed literals.");
                    goto _output_error; 
                } /* Error : last LASTLITERALS bytes must be literals (uncompressed) */
                if (op < oCopyLimit) {
                    LZ4_wildCopy8(op, match, oCopyLimit);
                    match += oCopyLimit - op;
                    op = oCopyLimit;
                }
                while (op < cpy) { *op++ = *match++; }
            } else {
                LZ4_memcpy(op, match, 8);
                if (length > 16)  { LZ4_wildCopy8(op+8, match+8, cpy); }
            }
            op = cpy;   /* wildcopy correction */

            AOCL_LZ4_DGV_SAFE_DECODE_EXIT
        }

        /* end of decoding */
        AOCL_LZ4_DGV_LOG_FORMATTED(INFO, logCtx, "Decoded %i bytes", (int)(((char*)op) - dst));
        DEBUGLOG(5, "decoded %i bytes", (int) (((char*)op)-dst));
        return (int) (((char*)op)-dst);     /* Nb of output bytes decoded */

        /* Overflow error detected */
    _output_error:
        return (int) (-(((const char*)ip)-src))-1;
    }
}
#endif

#undef AOCL_LZ4_DECOMPRESS_GENERIC_VARIANT
#undef AOCL_LZ4_DECOMPRESS_GENERIC_FUNC
#undef AOCL_LZ4_DGV_ADDITIONAL_PARAMS
#undef AOCL_LZ4_DGV_LOG_FORMATTED
#undef AOCL_LZ4_DGV_LOG_UNFORMATTED
#undef AOCL_LZ4_DGV_LONG_MATCH
#undef AOCL_LZ4_DGV_LAST_SEQ_CHECK
#undef AOCL_LZ4_DGV_EOF_CHECK
#undef AOCL_LZ4_DGV_COPY_MATCH
#undef AOCL_LZ4_DGV_SAFE_DECODE_EXIT
