/* LzmaDec.c -- LZMA Decoder
2018-07-04 : Igor Pavlov : Public domain */

/**
 * Copyright (C) 2022, Advanced Micro Devices. All rights reserved.
 */

#include "Precomp.h"

#include <string.h>

/* #include "CpuArch.h" */
#include "LzmaDec.h"

#define kNumTopBits 24
#define kTopValue ((UInt32)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5

#define RC_INIT_SIZE 5 //skip 1st byte. use next 4 for code initialization

/* Key terms used in range decoder:
* 
* range: Dynamically scaled integer value that provides the bound for current
*        range being used by the decoder. In an equivalent arithmetic coder,
*        this range would be fixed at [0,1]. Here, it is set to a large enough
*        value so that 'bound' that is computed within this range gives an integer
*        value. If range is too small for this condition to be met, it is scaled
*        up accordingly (range <<= 8) until we can get an integer 'bound'.
* code:  Contains compressed data that is currently being decompressed.
*        Current code also locates a point within range that determines
*        the next symbol to be emmited. Code and range must be normalized in sync
*        as code should locate a point within range in every iteration
* bound: As range encoder here is implemented in binary, there are only 2 
*        symbols(0 and 1). Bound is a scaled integral representation of 
*        the probabilty boundary between the 2 symbols within current range.
*        If current code < bound, emit 0, else emit 1.
* 
* Normalization: If range is too small, splitting it further will result
* in fractional ranges for symbols. So, range needs to be scaled up to 
* allow for integral splits in the future. */

/* if range < 2^24 normalize range and code.
* Set LSB 8 bits of code to next compressed byte value from stream */
#define NORMALIZE if (range < kTopValue) { range <<= 8; code = (code << 8) | (*buf++); }

/* Compute probability boundary between the 2 symbols bound = (range / 2^11) * prob 
* Check if code < bound */
#define IF_BIT_0(p) ttt = *(p); NORMALIZE; bound = (range >> kNumBitModelTotalBits) * (UInt32)ttt; if (code < bound)

/* If (code < bound), emit 0, range for next iteration becomes [0, bound). i.e. size = bound
* Update probability model: prob + delta [Prob of 0 increased] */
#define UPDATE_0(p) range = bound; *(p) = (CLzmaProb)(ttt + ((kBitModelTotal - ttt) >> kNumMoveBits));

/* If (code >= bound), emit 1, range for next iteration becomes [bound, range). i.e. size = range-bound
* Update probability model: prob - delta [Prob of 1 increased]  */
#define UPDATE_1(p) range -= bound; code -= bound; *(p) = (CLzmaProb)(ttt - (ttt >> kNumMoveBits));

/* Updates for i can be used to generate byte from bits when GET_BIT2
* is called multiple times. (i<<1)+(0 or 1 based on current bit) on every call */
#define GET_BIT2(p, i, A0, A1) IF_BIT_0(p) \
  { UPDATE_0(p); i = (i + i); A0; } else \
  { UPDATE_1(p); i = (i + i) + 1; A1; }

/* If TREE_GET_BIT is called multiple times in succession,
* each call changes i to (i<<1)+(0 or 1). So, subsequent call
* will use a probs context model located at the new position */
#define TREE_GET_BIT(probs, i) { GET_BIT2(probs + i, i, ;, ;); }

#define REV_BIT(p, i, A0, A1) IF_BIT_0(p + i) \
  { UPDATE_0(p + i); A0; } else \
  { UPDATE_1(p + i); A1; }
#define REV_BIT_VAR(  p, i, m) REV_BIT(p, i, i += m; m += m, m += m; i += m; )
#define REV_BIT_CONST(p, i, m) REV_BIT(p, i, i += m;       , i += m * 2; )
#define REV_BIT_LAST( p, i, m) REV_BIT(p, i, i -= m        , ; )

#define TREE_DECODE(probs, limit, i) \
  { i = 1; do { TREE_GET_BIT(probs, i); } while (i < limit); i -= limit; }

/* #define _LZMA_SIZE_OPT */

#ifdef _LZMA_SIZE_OPT
#define TREE_6_DECODE(probs, i) TREE_DECODE(probs, (1 << 6), i)
#else
#define TREE_6_DECODE(probs, i) \
  { i = 1; \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  i -= 0x40; }
#endif

#define NORMAL_LITER_DEC TREE_GET_BIT(prob, symbol)
#define MATCHED_LITER_DEC \
  matchByte += matchByte; \
  bit = offs; \
  offs &= matchByte; \
  probLit = prob + (offs + bit + symbol); \
  GET_BIT2(probLit, symbol, offs ^= bit; , ;)



#define NORMALIZE_CHECK if (range < kTopValue) { if (buf >= bufLimit) return DUMMY_ERROR; range <<= 8; code = (code << 8) | (*buf++); }

#define IF_BIT_0_CHECK(p) ttt = *(p); NORMALIZE_CHECK; bound = (range >> kNumBitModelTotalBits) * (UInt32)ttt; if (code < bound)
#define UPDATE_0_CHECK range = bound;
#define UPDATE_1_CHECK range -= bound; code -= bound;
#define GET_BIT2_CHECK(p, i, A0, A1) IF_BIT_0_CHECK(p) \
  { UPDATE_0_CHECK; i = (i + i); A0; } else \
  { UPDATE_1_CHECK; i = (i + i) + 1; A1; }
#define GET_BIT_CHECK(p, i) GET_BIT2_CHECK(p, i, ; , ;)
#define TREE_DECODE_CHECK(probs, limit, i) \
  { i = 1; do { GET_BIT_CHECK(probs + i, i) } while (i < limit); i -= limit; }


#define REV_BIT_CHECK(p, i, m) IF_BIT_0_CHECK(p + i) \
  { UPDATE_0_CHECK; i += m; m += m; } else \
  { UPDATE_1_CHECK; m += m; i += m; }


#define kNumPosBitsMax 4
#define kNumPosStatesMax (1 << kNumPosBitsMax)

#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define LenLow 0
#define LenHigh (LenLow + 2 * (kNumPosStatesMax << kLenNumLowBits))
#define kNumLenProbs (LenHigh + kLenNumHighSymbols)

#define LenChoice LenLow
#define LenChoice2 (LenLow + (1 << kLenNumLowBits))

#define kNumStates 12 //states in state machine for context modelling
#define kNumStates2 16 //states extended [0000(0) - 1100(12) valid] [1101-1111 unused]
#define kNumLitStates 7 //state 0-6 => prev=LIT

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

#define kNumPosSlotBits 6 //number of slot bits
#define kNumLenToPosStates 4 //lengths 0,1,2,>2 are used to determine context for distance slot

#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)

#define kMatchMinLen 2
#define kMatchSpecLenStart (kMatchMinLen + kLenNumLowSymbols * 2 + kLenNumHighSymbols)

/* External ASM code needs same CLzmaProb array layout. So don't change it. */

/* (probs_1664) is faster and better for code size at some platforms */
/*
#ifdef MY_CPU_X86_OR_AMD64
*/
#define kStartOffset 1664
#define GET_PROBS p->probs_1664
/*
#define GET_PROBS p->probs + kStartOffset
#else
#define kStartOffset 0
#define GET_PROBS p->probs
#endif
*/


/* Depending on type of packet and bit position within the packet, different
* contexts are used by the range encoder.
* Following probability contexts exist:
* LenCoder[512], RepLenCoder[512] : 256 for high len bits 
* IsMatch[256], IsRep0Long[256] : 16 states * 2^4. 4 bits from dictionary pos can be used as context. 
* Align[16] : 4 align bits in long distances
* IsRep[12], IsRepG0[12], IsRepG1[12], IsRepG2[12] : states = 12
* PosSlot[256] : 2^6 slots * 4 len values used in distance context 
* Literal[?] */

// Offsets in probability table for different context models
#define SpecPos (-kStartOffset)
#define IsRep0Long (SpecPos + kNumFullDistances)
#define RepLenCoder (IsRep0Long + (kNumStates2 << kNumPosBitsMax))
#define LenCoder (RepLenCoder + kNumLenProbs)
#define IsMatch (LenCoder + kNumLenProbs)
#define Align (IsMatch + (kNumStates2 << kNumPosBitsMax))
#define IsRep (Align + kAlignTableSize)
#define IsRepG0 (IsRep + kNumStates)
#define IsRepG1 (IsRepG0 + kNumStates)
#define IsRepG2 (IsRepG1 + kNumStates)
#define PosSlot (IsRepG2 + kNumStates)
#define Literal (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
#define NUM_BASE_PROBS (Literal + kStartOffset)

#if Align != 0 && kStartOffset != 0
  #error Stop_Compiling_Bad_LZMA_kAlign
#endif

#if NUM_BASE_PROBS != 1984
  #error Stop_Compiling_Bad_LZMA_PROBS
#endif


#define LZMA_LIT_SIZE 0x300

#define LzmaProps_GetNumProbs(p) (NUM_BASE_PROBS + ((UInt32)LZMA_LIT_SIZE << ((p)->lc + (p)->lp)))


/* Get last N bits of processedPos
* Total 12 states are possible. When encoded 0000(0) to 1100(12)
* << 4 to allow for these to be merged in COMBINED_PS_STATE
* COMBINED_PS_STATE = [posState|state]
* For default pb=2, this gives 4*12 = 48 possible combined states */
#define CALC_POS_STATE(processedPos, pbMask) (((processedPos) & (pbMask)) << 4)
#define COMBINED_PS_STATE (posState + state)
#define GET_LEN_STATE (posState)

#define LZMA_DIC_MIN (1 << 12)

/*
p->remainLen : shows status of LZMA decoder:
    < kMatchSpecLenStart : normal remain
    = kMatchSpecLenStart : finished
    = kMatchSpecLenStart + 1 : need init range coder
    = kMatchSpecLenStart + 2 : need init range coder and state
*/

/* ---------- LZMA_DECODE_REAL ---------- */
/*
LzmaDec_DecodeReal_3() can be implemented in external ASM file.
3 - is the code compatibility version of that function for check at link time.
*/

#define LZMA_DECODE_REAL LzmaDec_DecodeReal_3

/*
LZMA_DECODE_REAL()
In:
  RangeCoder is normalized
  if (p->dicPos == limit)
  {
    LzmaDec_TryDummy() was called before to exclude LITERAL and MATCH-REP cases.
    So first symbol can be only MATCH-NON-REP. And if that MATCH-NON-REP symbol
    is not END_OF_PAYALOAD_MARKER, then function returns error code.
  }

Processing:
  first LZMA symbol will be decoded in any case
  All checks for limits are at the end of main loop,
  It will decode new LZMA-symbols while (p->buf < bufLimit && dicPos < limit),
  RangeCoder is still without last normalization when (p->buf < bufLimit) is being checked.

Out:
  RangeCoder is normalized
  Result:
    SZ_OK - OK
    SZ_ERROR_DATA - Error
  p->remainLen:
    < kMatchSpecLenStart : normal remain
    = kMatchSpecLenStart : finished
*/


#ifdef _LZMA_DEC_OPT

int MY_FAST_CALL LZMA_DECODE_REAL(CLzmaDec *p, SizeT limit, const Byte *bufLimit);

#else

static
int MY_FAST_CALL LZMA_DECODE_REAL(CLzmaDec *p, SizeT limit, const Byte *bufLimit)
{
  CLzmaProb *probs = GET_PROBS; //probabilities for all the different contexts
  unsigned state = (unsigned)p->state;
  UInt32 rep0 = p->reps[0], rep1 = p->reps[1], rep2 = p->reps[2], rep3 = p->reps[3];
  unsigned pbMask = ((unsigned)1 << (p->prop.pb)) - 1;
  unsigned lc = p->prop.lc;
  unsigned lpMask = ((unsigned)0x100 << p->prop.lp) - ((unsigned)0x100 >> lc);

  Byte *dic = p->dic;
  SizeT dicBufSize = p->dicBufSize;
  SizeT dicPos = p->dicPos;
  
  UInt32 processedPos = p->processedPos;
  UInt32 checkDicSize = p->checkDicSize;
  unsigned len = 0;

  const Byte *buf = p->buf;
  UInt32 range = p->range;
  UInt32 code = p->code;

  /* State transitions are as follows:
  *           next state
  * cur   packet LIT MATCH REP SREP
  * state
  * 0            0   7     8   9
  * 1            0   7     8   9
  * 2            0   7     8   9
  * 3            0   7     8   9
  * 4            1   7     8   9
  * 5            2   7     8   9
  * 6            3   7     8   9
  * 7            4   10    11  11
  * 8            5   10    11  11
  * 9            6   10    11  11
  * 10           4   10    11  11
  * 11           5   10    11  11 */
  do
  {
    CLzmaProb *prob;
    UInt32 bound;
    unsigned ttt;
    unsigned posState = CALC_POS_STATE(processedPos, pbMask);

    prob = probs + IsMatch + COMBINED_PS_STATE;
    IF_BIT_0(prob)
    {
      unsigned symbol; //extract literal byte here
      UPDATE_0(prob); //code 0 + literal
      prob = probs + Literal;
      if (processedPos != 0 || checkDicSize != 0)
        prob += (UInt32)3 * ((((processedPos << 8) + dic[(dicPos == 0 ? dicBufSize : dicPos) - 1]) & lpMask) << lc);
      processedPos++;

      if (state < kNumLitStates) //cur= LIT, prev=LIT
      {
        state -= (state < 4) ? state : 3;
        symbol = 1; //start with 1 and build byte bitwise in each of the _LITER_DEC calls
        /* NORMAL_LITER_DEC is called 8 times in succession to decode a byte fully.
        * Each call adds an extra bit to symbol. As symbol can range from [0-255],
        * we can have 256 probs context models. */
        #ifdef _LZMA_SIZE_OPT
        do { NORMAL_LITER_DEC } while (symbol < 0x100);
        #else
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        #endif
      }
      else //cur= LIT, prev=Non-LIT
      {
        unsigned matchByte = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
        unsigned offs = 0x100;
        state -= (state < 10) ? 3 : 6;
        symbol = 1;
        #ifdef _LZMA_SIZE_OPT
        do
        {
          unsigned bit;
          CLzmaProb *probLit;
          MATCHED_LITER_DEC
        }
        while (symbol < 0x100);
        #else
        {
          unsigned bit;
          CLzmaProb *probLit;
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
        }
        #endif
      }

      dic[dicPos++] = (Byte)symbol; //add symbol to decompressed dictionary
      continue; //done. go to next packet
    }
    
    {
      UPDATE_1(prob); //code 1
      prob = probs + IsRep + state;
      IF_BIT_0(prob)
      {
        UPDATE_0(prob); //code 10 + len + dis (match)
        state += kNumStates; //just an indicator for 'match' state check later on
        prob = probs + LenCoder;
      }
      else
      {
        UPDATE_1(prob); //code 11 (srep or rep)
        /*
        // that case was checked before with kBadRepCode
        if (checkDicSize == 0 && processedPos == 0)
          return SZ_ERROR_DATA;
        */
        prob = probs + IsRepG0 + state;
        IF_BIT_0(prob)
        {
          UPDATE_0(prob); //code 110 (srep or rep0)
          prob = probs + IsRep0Long + COMBINED_PS_STATE;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob); //code 1100 (srep)
            //copy single byte from dictionary at srep position
            dic[dicPos] = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
            dicPos++;
            processedPos++;
            state = state < kNumLitStates ? 9 : 11;
            continue; //done. go to next packet
          }
          UPDATE_1(prob); //code 1101 (rep0)
        }
        else
        {
          UInt32 distance; //get distance from rep values
          UPDATE_1(prob);  //code 111 (rep1-3)
          prob = probs + IsRepG1 + state;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob); //code 1110 (rep1)
            distance = rep1;
          }
          else
          {
            UPDATE_1(prob); //code 1111 (rep2-3)
            prob = probs + IsRepG2 + state;
            IF_BIT_0(prob)
            {
              UPDATE_0(prob); //code 11110 (rep2)
              distance = rep2;
            }
            else
            {
              UPDATE_1(prob); //code 11111 (rep3)
              distance = rep3;
              rep3 = rep2; //shift rep queue
            }
            rep2 = rep1;
          }
          rep1 = rep0;
          rep0 = distance;
        }
        state = state < kNumLitStates ? 8 : 11;
        prob = probs + RepLenCoder;
      }
      
      //Process len bits for Match or Rep
      #ifdef _LZMA_SIZE_OPT
      {
        unsigned lim, offset;
        CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0(probLen)
        {
          UPDATE_0(probLen);
          probLen = prob + LenLow + GET_LEN_STATE;
          offset = 0;
          lim = (1 << kLenNumLowBits);
        }
        else
        {
          UPDATE_1(probLen);
          probLen = prob + LenChoice2;
          IF_BIT_0(probLen)
          {
            UPDATE_0(probLen);
            probLen = prob + LenLow + GET_LEN_STATE + (1 << kLenNumLowBits);
            offset = kLenNumLowSymbols;
            lim = (1 << kLenNumLowBits);
          }
          else
          {
            UPDATE_1(probLen);
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols * 2;
            lim = (1 << kLenNumHighBits);
          }
        }
        TREE_DECODE(probLen, lim, len);
        len += offset;
      }
      #else
      {
        CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0(probLen)
        {
          UPDATE_0(probLen); //code 0 len
          probLen = prob + LenLow + GET_LEN_STATE;
          //Decompress 3 len bits representing length in range[0-7]
          len = 1;
          TREE_GET_BIT(probLen, len);
          TREE_GET_BIT(probLen, len);
          TREE_GET_BIT(probLen, len);
          len -= 8; // len obtained above is in range [8-15]. -8 to bring it down to [0-7]
        }
        else
        {
          UPDATE_1(probLen); //code 1x len
          probLen = prob + LenChoice2;
          IF_BIT_0(probLen)
          {
            UPDATE_0(probLen); //code 10 len
            probLen = prob + LenLow + GET_LEN_STATE + (1 << kLenNumLowBits);
            /* Decompress 3 len bits representing length in range[8-15]
            * Same context prob used for all 3 bits */
            len = 1;
            TREE_GET_BIT(probLen, len);
            TREE_GET_BIT(probLen, len);
            TREE_GET_BIT(probLen, len);
          }
          else
          {
            UPDATE_1(probLen); //code 11 len
            probLen = prob + LenHigh;
            //Decompress 8 len bits representing length in range[16-271]
            TREE_DECODE(probLen, (1 << kLenNumHighBits), len); //Do 8 TREE_GET_BIT() calls for 8-bit len
            len += kLenNumLowSymbols * 2; // len obtained above is in range [0-255]. Shift to [16-271]
          }
        }
      }
      #endif

      //Process distance bits for Match
      if (state >= kNumStates) //if match, we need to decompress distance as well
      {
        UInt32 distance;
        //prob context for distance is based on len as well: 0,1,2,>2
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) << kNumPosSlotBits);
        TREE_6_DECODE(prob, distance); //decompress 6 bit slot part
        if (distance >= kStartPosModelIndex) //if slot range [4, max_dist] decompress remaining bits
        {
          unsigned posSlot = (unsigned)distance;
          unsigned numDirectBits = (unsigned)(((distance >> 1) - 1));
          distance = (2 | (distance & 1));
          if (posSlot < kEndPosModelIndex) //distance range [4-127] direct bits only
          {
            distance <<= numDirectBits;
            prob = probs + SpecPos;
            {
              UInt32 m = 1;
              distance++;
              do
              {
                REV_BIT_VAR(prob, distance, m);
              }
              while (--numDirectBits);
              distance -= m;
            }
          }
          else //distance range [128, max_dist] direct bits + 4 align bits
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE
              range >>= 1;
              
              {
                UInt32 t;
                code -= range;
                t = (0 - ((UInt32)code >> 31)); /* (UInt32)((Int32)code >> 31) */
                distance = (distance << 1) + (t + 1);
                code += range & t;
              }
              /*
              distance <<= 1;
              if (code >= range)
              {
                code -= range;
                distance |= 1;
              }
              */
            }
            while (--numDirectBits);
            prob = probs + Align;
            distance <<= kNumAlignBits;
            { //decompress remaining 4 align bits
              unsigned i = 1;
              REV_BIT_CONST(prob, i, 1);
              REV_BIT_CONST(prob, i, 2);
              REV_BIT_CONST(prob, i, 4);
              REV_BIT_LAST (prob, i, 8);
              distance |= i;
            }
            if (distance == (UInt32)0xFFFFFFFF)
            {
              len = kMatchSpecLenStart;
              state -= kNumStates;
              break;
            }
          }
        }
        
        rep3 = rep2; // shift rep queue by 1 position. latest rep must be in rep0
        rep2 = rep1;
        rep1 = rep0;
        rep0 = distance + 1;
        state = (state < kNumStates + kNumLitStates) ? kNumLitStates : kNumLitStates + 3;
        if (distance >= (checkDicSize == 0 ? processedPos: checkDicSize))
        {
          p->dicPos = dicPos;
          return SZ_ERROR_DATA;
        }
      }

      len += kMatchMinLen; // add offset 2 to len to bring to range [2-273]

      {
        SizeT rem;
        unsigned curLen;
        SizeT pos;
        
        if ((rem = limit - dicPos) == 0)
        {
          p->dicPos = dicPos;
          return SZ_ERROR_DATA;
        }
        
        curLen = ((rem < len) ? (unsigned)rem : len);
        pos = dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0);

        processedPos += (UInt32)curLen;

        len -= curLen;
        if (curLen <= dicBufSize - pos)
        {
          Byte *dest = dic + dicPos;
          ptrdiff_t src = (ptrdiff_t)pos - (ptrdiff_t)dicPos;
          const Byte *lim = dest + curLen;
          dicPos += (SizeT)curLen;
          do
            *(dest) = (Byte)*(dest + src);
          while (++dest != lim);
        }
        else
        {
          do
          {
            dic[dicPos++] = dic[pos];
            if (++pos == dicBufSize)
              pos = 0;
          }
          while (--curLen != 0);
        }
      }
    }
  }
  while (dicPos < limit && buf < bufLimit);

  NORMALIZE;
  
  p->buf = buf;
  p->range = range;
  p->code = code;
  p->remainLen = (UInt32)len;
  p->dicPos = dicPos;
  p->processedPos = processedPos;
  p->reps[0] = rep0;
  p->reps[1] = rep1;
  p->reps[2] = rep2;
  p->reps[3] = rep3;
  p->state = (UInt32)state;

  return SZ_OK;
}
#endif

static void MY_FAST_CALL LzmaDec_WriteRem(CLzmaDec *p, SizeT limit)
{
  if (p->remainLen != 0 && p->remainLen < kMatchSpecLenStart)
  {
    Byte *dic = p->dic;
    SizeT dicPos = p->dicPos;
    SizeT dicBufSize = p->dicBufSize;
    unsigned len = (unsigned)p->remainLen;
    SizeT rep0 = p->reps[0]; /* we use SizeT to avoid the BUG of VC14 for AMD64 */
    SizeT rem = limit - dicPos;
    if (rem < len)
      len = (unsigned)(rem);

    if (p->checkDicSize == 0 && p->prop.dicSize - p->processedPos <= len)
      p->checkDicSize = p->prop.dicSize;

    p->processedPos += (UInt32)len;
    p->remainLen -= (UInt32)len;
    while (len != 0)
    {
      len--;
      dic[dicPos] = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
      dicPos++;
    }
    p->dicPos = dicPos;
  }
}


#define kRange0 0xFFFFFFFF
#define kBound0 ((kRange0 >> kNumBitModelTotalBits) << (kNumBitModelTotalBits - 1))
#define kBadRepCode (kBound0 + (((kRange0 - kBound0) >> kNumBitModelTotalBits) << (kNumBitModelTotalBits - 1)))
#if kBadRepCode != (0xC0000000 - 0x400)
  #error Stop_Compiling_Bad_LZMA_Check
#endif

static int MY_FAST_CALL LzmaDec_DecodeReal2(CLzmaDec *p, SizeT limit, const Byte *bufLimit)
{
  do
  {
    SizeT limit2 = limit;
    if (p->checkDicSize == 0)
    {
      UInt32 rem = p->prop.dicSize - p->processedPos;
      if (limit - p->dicPos > rem)
        limit2 = p->dicPos + rem;

      if (p->processedPos == 0)
        if (p->code >= kBadRepCode)
          return SZ_ERROR_DATA;
    }

    RINOK(LZMA_DECODE_REAL(p, limit2, bufLimit));
    
    if (p->checkDicSize == 0 && p->processedPos >= p->prop.dicSize)
      p->checkDicSize = p->prop.dicSize;
    
    LzmaDec_WriteRem(p, limit);
  }
  while (p->dicPos < limit && p->buf < bufLimit && p->remainLen < kMatchSpecLenStart);

  return 0;
}

typedef enum
{
  DUMMY_ERROR, /* unexpected end of input stream */
  DUMMY_LIT,
  DUMMY_MATCH,
  DUMMY_REP
} ELzmaDummy;

static ELzmaDummy LzmaDec_TryDummy(const CLzmaDec *p, const Byte *buf, SizeT inSize)
{
  UInt32 range = p->range;
  UInt32 code = p->code;
  const Byte *bufLimit = buf + inSize;
  const CLzmaProb *probs = GET_PROBS;
  unsigned state = (unsigned)p->state;
  ELzmaDummy res;

  {
    const CLzmaProb *prob;
    UInt32 bound;
    unsigned ttt;
    unsigned posState = CALC_POS_STATE(p->processedPos, (1 << p->prop.pb) - 1);

    prob = probs + IsMatch + COMBINED_PS_STATE;
    IF_BIT_0_CHECK(prob)
    {
      UPDATE_0_CHECK

      /* if (bufLimit - buf >= 7) return DUMMY_LIT; */

      prob = probs + Literal;
      if (p->checkDicSize != 0 || p->processedPos != 0)
        prob += ((UInt32)LZMA_LIT_SIZE *
            ((((p->processedPos) & ((1 << (p->prop.lp)) - 1)) << p->prop.lc) +
            (p->dic[(p->dicPos == 0 ? p->dicBufSize : p->dicPos) - 1] >> (8 - p->prop.lc))));

      if (state < kNumLitStates)
      {
        unsigned symbol = 1;
        do { GET_BIT_CHECK(prob + symbol, symbol) } while (symbol < 0x100);
      }
      else
      {
        unsigned matchByte = p->dic[p->dicPos - p->reps[0] +
            (p->dicPos < p->reps[0] ? p->dicBufSize : 0)];
        unsigned offs = 0x100;
        unsigned symbol = 1;
        do
        {
          unsigned bit;
          const CLzmaProb *probLit;
          matchByte += matchByte;
          bit = offs;
          offs &= matchByte;
          probLit = prob + (offs + bit + symbol);
          GET_BIT2_CHECK(probLit, symbol, offs ^= bit; , ; )
        }
        while (symbol < 0x100);
      }
      res = DUMMY_LIT;
    }
    else
    {
      unsigned len;
      UPDATE_1_CHECK;

      prob = probs + IsRep + state;
      IF_BIT_0_CHECK(prob)
      {
        UPDATE_0_CHECK;
        state = 0;
        prob = probs + LenCoder;
        res = DUMMY_MATCH;
      }
      else
      {
        UPDATE_1_CHECK;
        res = DUMMY_REP;
        prob = probs + IsRepG0 + state;
        IF_BIT_0_CHECK(prob)
        {
          UPDATE_0_CHECK;
          prob = probs + IsRep0Long + COMBINED_PS_STATE;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
            NORMALIZE_CHECK;
            return DUMMY_REP;
          }
          else
          {
            UPDATE_1_CHECK;
          }
        }
        else
        {
          UPDATE_1_CHECK;
          prob = probs + IsRepG1 + state;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
          }
          else
          {
            UPDATE_1_CHECK;
            prob = probs + IsRepG2 + state;
            IF_BIT_0_CHECK(prob)
            {
              UPDATE_0_CHECK;
            }
            else
            {
              UPDATE_1_CHECK;
            }
          }
        }
        state = kNumStates;
        prob = probs + RepLenCoder;
      }
      {
        unsigned limit, offset;
        const CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0_CHECK(probLen)
        {
          UPDATE_0_CHECK;
          probLen = prob + LenLow + GET_LEN_STATE;
          offset = 0;
          limit = 1 << kLenNumLowBits;
        }
        else
        {
          UPDATE_1_CHECK;
          probLen = prob + LenChoice2;
          IF_BIT_0_CHECK(probLen)
          {
            UPDATE_0_CHECK;
            probLen = prob + LenLow + GET_LEN_STATE + (1 << kLenNumLowBits);
            offset = kLenNumLowSymbols;
            limit = 1 << kLenNumLowBits;
          }
          else
          {
            UPDATE_1_CHECK;
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols * 2;
            limit = 1 << kLenNumHighBits;
          }
        }
        TREE_DECODE_CHECK(probLen, limit, len);
        len += offset;
      }

      if (state < 4)
      {
        unsigned posSlot;
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates - 1 ? len : kNumLenToPosStates - 1) <<
            kNumPosSlotBits);
        TREE_DECODE_CHECK(prob, 1 << kNumPosSlotBits, posSlot);
        if (posSlot >= kStartPosModelIndex)
        {
          unsigned numDirectBits = ((posSlot >> 1) - 1);

          /* if (bufLimit - buf >= 8) return DUMMY_MATCH; */

          if (posSlot < kEndPosModelIndex)
          {
            prob = probs + SpecPos + ((2 | (posSlot & 1)) << numDirectBits);
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE_CHECK
              range >>= 1;
              code -= range & (((code - range) >> 31) - 1);
              /* if (code >= range) code -= range; */
            }
            while (--numDirectBits);
            prob = probs + Align;
            numDirectBits = kNumAlignBits;
          }
          {
            unsigned i = 1;
            unsigned m = 1;
            do
            {
              REV_BIT_CHECK(prob, i, m);
            }
            while (--numDirectBits);
          }
        }
      }
    }
  }
  NORMALIZE_CHECK;
  return res;
}


void LzmaDec_InitDicAndState(CLzmaDec *p, BoolInt initDic, BoolInt initState)
{
  p->remainLen = kMatchSpecLenStart + 1;
  p->tempBufSize = 0;

  if (initDic)
  {
    p->processedPos = 0;
    p->checkDicSize = 0;
    p->remainLen = kMatchSpecLenStart + 2;
  }
  if (initState)
    p->remainLen = kMatchSpecLenStart + 2;
}

void LzmaDec_Init(CLzmaDec *p)
{
  p->dicPos = 0;
  LzmaDec_InitDicAndState(p, True, True);
}


SRes LzmaDec_DecodeToDic(CLzmaDec *p, SizeT dicLimit, const Byte *src, SizeT *srcLen,
    ELzmaFinishMode finishMode, ELzmaStatus *status)
{
  SizeT inSize = *srcLen;
  (*srcLen) = 0;
  
  *status = LZMA_STATUS_NOT_SPECIFIED;

  if (p->remainLen > kMatchSpecLenStart)
  {
    for (; inSize > 0 && p->tempBufSize < RC_INIT_SIZE; (*srcLen)++, inSize--)
      p->tempBuf[p->tempBufSize++] = *src++;
    if (p->tempBufSize != 0 && p->tempBuf[0] != 0)
      return SZ_ERROR_DATA;
    if (p->tempBufSize < RC_INIT_SIZE)
    {
      *status = LZMA_STATUS_NEEDS_MORE_INPUT;
      return SZ_OK;
    }
    p->code =
        ((UInt32)p->tempBuf[1] << 24)
      | ((UInt32)p->tempBuf[2] << 16)
      | ((UInt32)p->tempBuf[3] << 8)
      | ((UInt32)p->tempBuf[4]); // set to 32-bit value starting at 2nd byte. big endian format.
    p->range = 0xFFFFFFFF; // 2^32 - 1
    p->tempBufSize = 0;

    if (p->remainLen > kMatchSpecLenStart + 1)
    {
      SizeT numProbs = LzmaProps_GetNumProbs(&p->prop);
      SizeT i;
      CLzmaProb *probs = p->probs;
      for (i = 0; i < numProbs; i++)
        probs[i] = kBitModelTotal >> 1;
      p->reps[0] = p->reps[1] = p->reps[2] = p->reps[3] = 1;
      p->state = 0;
    }

    p->remainLen = 0;
  }

  LzmaDec_WriteRem(p, dicLimit);

  while (p->remainLen != kMatchSpecLenStart)
  {
      int checkEndMarkNow = 0;

      if (p->dicPos >= dicLimit)
      {
        if (p->remainLen == 0 && p->code == 0)
        {
          *status = LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK;
          return SZ_OK;
        }
        if (finishMode == LZMA_FINISH_ANY)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return SZ_OK;
        }
        if (p->remainLen != 0)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return SZ_ERROR_DATA;
        }
        checkEndMarkNow = 1;
      }

      if (p->tempBufSize == 0)
      {
        SizeT processed;
        const Byte *bufLimit;
        if (inSize < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, src, inSize);
          if (dummyRes == DUMMY_ERROR)
          {
            memcpy(p->tempBuf, src, inSize);
            p->tempBufSize = (unsigned)inSize;
            (*srcLen) += inSize;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return SZ_OK;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return SZ_ERROR_DATA;
          }
          bufLimit = src;
        }
        else
          bufLimit = src + inSize - LZMA_REQUIRED_INPUT_MAX;
        p->buf = src;
        if (LzmaDec_DecodeReal2(p, dicLimit, bufLimit) != 0)
          return SZ_ERROR_DATA;
        processed = (SizeT)(p->buf - src);
        (*srcLen) += processed;
        src += processed;
        inSize -= processed;
      }
      else
      {
        unsigned rem = p->tempBufSize, lookAhead = 0;
        while (rem < LZMA_REQUIRED_INPUT_MAX && lookAhead < inSize)
          p->tempBuf[rem++] = src[lookAhead++];
        p->tempBufSize = rem;
        if (rem < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, p->tempBuf, (SizeT)rem);
          if (dummyRes == DUMMY_ERROR)
          {
            (*srcLen) += (SizeT)lookAhead;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return SZ_OK;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return SZ_ERROR_DATA;
          }
        }
        p->buf = p->tempBuf;
        if (LzmaDec_DecodeReal2(p, dicLimit, p->buf) != 0)
          return SZ_ERROR_DATA;
        
        {
          unsigned kkk = (unsigned)(p->buf - p->tempBuf);
          if (rem < kkk)
            return SZ_ERROR_FAIL; /* some internal error */
          rem -= kkk;
          if (lookAhead < rem)
            return SZ_ERROR_FAIL; /* some internal error */
          lookAhead -= rem;
        }
        (*srcLen) += (SizeT)lookAhead;
        src += lookAhead;
        inSize -= (SizeT)lookAhead;
        p->tempBufSize = 0;
      }
  }
  
  if (p->code != 0)
    return SZ_ERROR_DATA;
  *status = LZMA_STATUS_FINISHED_WITH_MARK;
  return SZ_OK;
}


SRes LzmaDec_DecodeToBuf(CLzmaDec *p, Byte *dest, SizeT *destLen, const Byte *src, SizeT *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status)
{
  SizeT outSize = *destLen;
  SizeT inSize = *srcLen;
  *srcLen = *destLen = 0;
  for (;;)
  {
    SizeT inSizeCur = inSize, outSizeCur, dicPos;
    ELzmaFinishMode curFinishMode;
    SRes res;
    if (p->dicPos == p->dicBufSize)
      p->dicPos = 0;
    dicPos = p->dicPos;
    if (outSize > p->dicBufSize - dicPos)
    {
      outSizeCur = p->dicBufSize;
      curFinishMode = LZMA_FINISH_ANY;
    }
    else
    {
      outSizeCur = dicPos + outSize;
      curFinishMode = finishMode;
    }

    res = LzmaDec_DecodeToDic(p, outSizeCur, src, &inSizeCur, curFinishMode, status);
    src += inSizeCur;
    inSize -= inSizeCur;
    *srcLen += inSizeCur;
    outSizeCur = p->dicPos - dicPos;
    memcpy(dest, p->dic + dicPos, outSizeCur);
    dest += outSizeCur;
    outSize -= outSizeCur;
    *destLen += outSizeCur;
    if (res != 0)
      return res;
    if (outSizeCur == 0 || outSize == 0)
      return SZ_OK;
  }
}

void LzmaDec_FreeProbs(CLzmaDec *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->probs);
  p->probs = NULL;
}

static void LzmaDec_FreeDict(CLzmaDec *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->dic);
  p->dic = NULL;
}

void LzmaDec_Free(CLzmaDec *p, ISzAllocPtr alloc)
{
  LzmaDec_FreeProbs(p, alloc);
  LzmaDec_FreeDict(p, alloc);
}

SRes LzmaProps_Decode(CLzmaProps *p, const Byte *data, unsigned size)
{
  UInt32 dicSize;
  Byte d;
  
  if (size < LZMA_PROPS_SIZE)
    return SZ_ERROR_UNSUPPORTED;
  else
    dicSize = data[1] | ((UInt32)data[2] << 8) | ((UInt32)data[3] << 16) | ((UInt32)data[4] << 24);
 
  if (dicSize < LZMA_DIC_MIN)
    dicSize = LZMA_DIC_MIN;
  p->dicSize = dicSize;

  d = data[0];
  if (d >= (9 * 5 * 5))
    return SZ_ERROR_UNSUPPORTED;

  p->lc = (Byte)(d % 9);
  d /= 9;
  p->pb = (Byte)(d / 5);
  p->lp = (Byte)(d % 5);

  return SZ_OK;
}

static SRes LzmaDec_AllocateProbs2(CLzmaDec *p, const CLzmaProps *propNew, ISzAllocPtr alloc)
{
  UInt32 numProbs = LzmaProps_GetNumProbs(propNew);
  if (!p->probs || numProbs != p->numProbs)
  {
    LzmaDec_FreeProbs(p, alloc);
    p->probs = (CLzmaProb *)ISzAlloc_Alloc(alloc, numProbs * sizeof(CLzmaProb));
    if (!p->probs)
      return SZ_ERROR_MEM;
    p->probs_1664 = p->probs + 1664;
    p->numProbs = numProbs;
  }
  return SZ_OK;
}

SRes LzmaDec_AllocateProbs(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAllocPtr alloc)
{
  CLzmaProps propNew;
  RINOK(LzmaProps_Decode(&propNew, props, propsSize));
  RINOK(LzmaDec_AllocateProbs2(p, &propNew, alloc));
  p->prop = propNew;
  return SZ_OK;
}

SRes LzmaDec_Allocate(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAllocPtr alloc)
{
  CLzmaProps propNew;
  SizeT dicBufSize;
  RINOK(LzmaProps_Decode(&propNew, props, propsSize));
  RINOK(LzmaDec_AllocateProbs2(p, &propNew, alloc));

  {
    UInt32 dictSize = propNew.dicSize;
    SizeT mask = ((UInt32)1 << 12) - 1;
         if (dictSize >= ((UInt32)1 << 30)) mask = ((UInt32)1 << 22) - 1;
    else if (dictSize >= ((UInt32)1 << 22)) mask = ((UInt32)1 << 20) - 1;;
    dicBufSize = ((SizeT)dictSize + mask) & ~mask;
    if (dicBufSize < dictSize)
      dicBufSize = dictSize;
  }

  if (!p->dic || dicBufSize != p->dicBufSize)
  {
    LzmaDec_FreeDict(p, alloc);
    p->dic = (Byte *)ISzAlloc_Alloc(alloc, dicBufSize);
    if (!p->dic)
    {
      LzmaDec_FreeProbs(p, alloc);
      return SZ_ERROR_MEM;
    }
  }
  p->dicBufSize = dicBufSize;
  p->prop = propNew;
  return SZ_OK;
}

SRes LzmaDecode(Byte *dest, SizeT *destLen, const Byte *src, SizeT *srcLen,
    const Byte *propData, unsigned propSize, ELzmaFinishMode finishMode,
    ELzmaStatus *status, ISzAllocPtr alloc)
{
  CLzmaDec p;
  SRes res;
  SizeT outSize = *destLen, inSize = *srcLen;
  *destLen = *srcLen = 0;
  *status = LZMA_STATUS_NOT_SPECIFIED;
  if (inSize < RC_INIT_SIZE)
    return SZ_ERROR_INPUT_EOF;
  LzmaDec_Construct(&p);
  RINOK(LzmaDec_AllocateProbs(&p, propData, propSize, alloc));
  p.dic = dest;
  p.dicBufSize = outSize;
  LzmaDec_Init(&p);
  *srcLen = inSize;
  res = LzmaDec_DecodeToDic(&p, outSize, src, srcLen, finishMode, status);
  *destLen = p.dicPos;
  if (res == SZ_OK && *status == LZMA_STATUS_NEEDS_MORE_INPUT)
    res = SZ_ERROR_INPUT_EOF;
  LzmaDec_FreeProbs(&p, alloc);
  return res;
}
