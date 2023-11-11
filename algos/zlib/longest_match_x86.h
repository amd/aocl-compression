/**
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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

#ifdef AOCL_ZLIB_AVX_OPT
// This header file is an template for avx and above function multiversion, application should not use it directly
__attribute__((__target__("avx"))) // uses SSE4.2 intrinsics
ZLIB_INTERNAL uint32_t LONGEST_MATCH_AVX_FAMILY(deflate_state* s, IPos cur_match)
{
    AOCL_SIMD_UNIT_TEST(DEBUG, logCtx, "Enter");
    unsigned chain_length = s->max_chain_length;/* max hash chain length */
    register Bytef *scan = s->window + s->strstart; /* current string */
    register int len;                           /* length of current match */
    int best_len = s->prev_length;              /* ignore strings, shorter or of the same length */
    int nice_match = s->nice_match;             /* stop if match long enough */
    int offset = 0;                             /* offset of current hash chain */
    IPos limit_base = s->strstart > (IPos)MAX_DIST(s) ?
        s->strstart - (IPos)MAX_DIST(s) : NIL;
    /*?? are MAX_DIST matches allowed ?! */
    IPos limit = limit_base;                    /* limit will be limit_base+offset */
    /* Stop when cur_match becomes <= limit. To simplify the code,
     * we prevent matches with the string of window index 0.
     */
    Bytef *match_base = s->window;              /* s->window - offset */
    Bytef *match_base2;                         /* s->window + best_len-1 - offset */
    /* "offset search" mode will speedup only with large chain_length; plus it is
     * impossible for deflate_fast(), because this function does not perform
     * INSERT_STRING() for matched strings (hash table have "holes"). deflate_fast()'s
     * max_chain is <= 32, deflate_slow() max_chain > 64 starting from compression
     * level 6; so - offs0_mode==true only for deflate_slow() with level >= 6)
     */
    int offs0_mode = chain_length < 64;         /* bool, mode with offset==0 */
    Posf *prev = s->prev;                       /* lists of the hash chains */
    uInt wmask = s->w_mask;

        /* points to last byte for maximal-length scan */
    register ush scan_start = *(ushf*)scan;     /* 1st 2 bytes of scan */
    uInt scan_start32 = *(uIntf*)scan;          /* 1st 4 bytes of scan */
    register ush scan_end;                      /* last byte of scan + next one */

#if (MIN_MATCH != 3) || (MAX_MATCH != 258)
#error The code is designed for MIN_MATCH==3 && MAX_MATCH==258
#endif

#define UPDATE_MATCH_BASE2  match_base2 = match_base+best_len-1
#define UPDATE_SCAN_END     scan_end = *(ushf*)(scan+best_len-1)

    UPDATE_MATCH_BASE2;
    UPDATE_SCAN_END;

    /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
     * It is easy to get rid of this optimization if necessary.
     */
    Assert(s->hash_bits >= 8, "Code too clever");

    /* Do not waste too much time if we already have a good match: */
    if (s->prev_length >= s->good_match) {
        chain_length >>= 2;
    }
    /* Do not look for matches beyond the end of the input. This is necessary
     * to make deflate deterministic.
     */
    if ((uInt)nice_match > s->lookahead) nice_match = s->lookahead;
    Assert((ulg)s->strstart <= s->window_size-MIN_LOOKAHEAD, "need lookahead");

    if (best_len >= MIN_MATCH) {
        /* We're continuing search (lazy evaluation).
         * Note: for deflate_fast best_len is always MIN_MATCH-1 here
         */
        register int i;
        IPos pos;
        register uInt hash = 0;
        /* Find a most distant chain starting from scan with index=1 (index=0 corresponds
         * to cur_match). Note: we cannot use s->prev[strstart+1,...] immediately, because
         * these strings are not yet inserted into hash table yet.
         */
        for (i = 3; i <= best_len; i++) {
            UPDATE_HASH_CRC(s, hash, scan[i]);
            /* If we're starting with best_len >= 3, we can use offset search. */
            pos = s->head[hash];
            if (pos < cur_match) {
                offset = i - 2;
                cur_match = pos;
            }
        }
        /* update variables to correspond offset */
        limit = limit_base + offset;
        if (cur_match <= limit) goto break_matching;
        match_base -= offset;
        match_base2 -= offset;
    }

#define NEXT_CHAIN \
    cur_match = prev[cur_match & wmask]; \
    if (cur_match <= limit) goto break_matching; \
    if (--chain_length == 0) goto break_matching; \
    Assert(cur_match - offset < s->strstart, "no future");

    do {
        /* Find a candidate for matching using hash table. Jump over hash
         * table chain until we'll have a partial march. Doing "break" when
         * matched, and NEXT_CHAIN to try different place.
         */
        if (best_len < MIN_MATCH) {
            /* Here we have best_len < MIN_MATCH, and this means, that
             * offset == 0. So, we need to check only first 2 bytes of
             * match (remaining 1 byte will be the same, because of nature of
             * hash function)
             */
            for (;;) {
                if (*(ushf*)(match_base + cur_match) == scan_start) break;
                NEXT_CHAIN;
            }
        } else if (best_len > MIN_MATCH) {
            /* current len > MIN_MATCH (>= 4 bytes); compare 1st 4 bytes and last 2 bytes */
            for (;;) {
                if (*(ushf*)(match_base2 + cur_match) == scan_end &&
                    *(uIntf*)(match_base + cur_match) == scan_start32) break;
                NEXT_CHAIN;
            }
        } else {
            /* current len is exactly MIN_MATCH (3 bytes); compare 4 bytes */
            for (;;) {
                if (*(uIntf*)(match_base + cur_match) == scan_start32) break;
                NEXT_CHAIN;
            }
        }
        /* scan is not updated in COMPARE256 , so no need to reset it at every iteration 
           no need to match first two bytes as they are already matched above */
        len = COMPARE256(scan + 2, match_base + cur_match + 2) + 2;

        Assert(scan + len <= s->window+(unsigned)(s->window_size-1), "wild scan");

        if (len > best_len) {
            /* new string is longer than previous - remember it */
            s->match_start = cur_match - offset;
            best_len = len;
            if (len >= nice_match) break;
            UPDATE_SCAN_END;
            /* look for better string offset */
			/*!! TODO: check if "cur_match - offset + len < s->strstart" condition is really needed - it restricts RLE-like compression */
            if (len > MIN_MATCH && cur_match - offset + len < s->strstart && !offs0_mode) {
                /* NOTE: if deflate algorithm will perform INSERT_STRING for
                 *   a whole scan (not for scan[0] only), can remove
                 *   "cur_match + len < s->strstart" limitation and replace it
                 *   with "cur_match + len < strend".
                 */
                IPos    pos, next_pos;
                register int i;
                register uInt hash;
                Bytef* scan_end;

                /* go back to offset 0 */
                cur_match -= offset;
                offset = 0;
                next_pos = cur_match;
                for (i = 0; i <= len - MIN_MATCH; i++) {
                    pos = prev[(cur_match + i) & wmask];
                    if (pos < next_pos) {
                        /* this hash chain is more distant, use it */
                        if (pos <= limit_base + i) goto break_matching;
                        next_pos = pos;
                        offset = i;
                    }
                }
                /* Switch cur_match to next_pos chain */
                cur_match = next_pos;

                /* Try hash head at len-(MIN_MATCH-1) position to see if we could get
                 * a better cur_match at the end of string. Using (MIN_MATCH-1) lets
                 * us to include one more byte into hash - the byte which will be checked
                 * in main loop now, and which allows to grow match by 1.
                 */
                hash = 0;
                scan_end = scan + len - MIN_MATCH + 1;
                UPDATE_HASH_CRC(s, hash, scan_end[2]);
                pos = s->head[hash];
                if (pos < cur_match) {
                    offset = len - MIN_MATCH + 1;
                    if (pos <= limit_base + offset) goto break_matching;
                    cur_match = pos;
                }

                /* update offset-dependent vars */
                limit = limit_base + offset;
                match_base = s->window - offset;
                UPDATE_MATCH_BASE2;
                continue;
            } else {
                /* There's no way to change offset - simply update match_base2 for
                 * new best_len (this is similar to what original algorithm does)
                 */
                UPDATE_MATCH_BASE2;
            }
        }
        /* follow hash chain */
        cur_match = prev[cur_match & wmask];
    } while (cur_match > limit && --chain_length != 0);

break_matching: /* sorry for goto's, but such code is smaller and easier to view ... */
    if ((uInt)best_len <= s->lookahead) return (uInt)best_len;
    return s->lookahead;
}
#endif /* AOCL_ZLIB_AVX_OPT */
