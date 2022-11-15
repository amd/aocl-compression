/* LzFind.c -- Match finder for LZ algorithms
2018-07-08 : Igor Pavlov : Public domain */

/**
 * Copyright (C) 2022, Advanced Micro Devices. All rights reserved.
 */

#include "Precomp.h"

#include <string.h>

#include "LzFind.h"
#include "LzHash.h"

#define kEmptyHashValue 0
#define kMaxValForNormalize ((UInt32)0xFFFFFFFF)
#define kNormalizeStepMin (1 << 10) /* it must be power of 2 */
#define kNormalizeMask (~(UInt32)(kNormalizeStepMin - 1))
#define kMaxHistorySize ((UInt32)7 << 29) // 3 GB

#define kStartMaxLen 3

static void LzInWindow_Free(CMatchFinder *p, ISzAllocPtr alloc)
{
  if (!p->directInput)
  {
    ISzAlloc_Free(alloc, p->bufferBase);
    p->bufferBase = NULL;
  }
}

/* keepSizeBefore + keepSizeAfter + keepSizeReserv must be < 4G) */

static int LzInWindow_Create(CMatchFinder *p, UInt32 keepSizeReserv, ISzAllocPtr alloc)
{
  UInt32 blockSize = p->keepSizeBefore + p->keepSizeAfter + keepSizeReserv;
  if (p->directInput)
  {
    p->blockSize = blockSize;
    return 1;
  }
  if (!p->bufferBase || p->blockSize != blockSize)
  {
    LzInWindow_Free(p, alloc);
    p->blockSize = blockSize;
    p->bufferBase = (Byte *)ISzAlloc_Alloc(alloc, (size_t)blockSize);
  }
  return (p->bufferBase != NULL);
}

Byte *MatchFinder_GetPointerToCurrentPos(CMatchFinder *p) { return p->buffer; }

UInt32 MatchFinder_GetNumAvailableBytes(CMatchFinder *p) { return p->streamPos - p->pos; }

void MatchFinder_ReduceOffsets(CMatchFinder *p, UInt32 subValue)
{
  p->posLimit -= subValue;
  p->pos -= subValue;
  p->streamPos -= subValue;
}

static void MatchFinder_ReadBlock(CMatchFinder *p)
{
  if (p->streamEndWasReached || p->result != SZ_OK)
    return;

  /* We use (p->streamPos - p->pos) value. (p->streamPos < p->pos) is allowed. */

  if (p->directInput)
  {
    UInt32 curSize = 0xFFFFFFFF - (p->streamPos - p->pos);
    if (curSize > p->directInputRem)
      curSize = (UInt32)p->directInputRem;
    p->directInputRem -= curSize;
    p->streamPos += curSize;
    if (p->directInputRem == 0)
      p->streamEndWasReached = 1;
    return;
  }
  
  for (;;)
  {
    Byte *dest = p->buffer + (p->streamPos - p->pos);
    size_t size = (p->bufferBase + p->blockSize - dest);
    if (size == 0)
      return;

    p->result = ISeqInStream_Read(p->stream, dest, &size);
    if (p->result != SZ_OK)
      return;
    if (size == 0)
    {
      p->streamEndWasReached = 1;
      return;
    }
    p->streamPos += (UInt32)size;
    if (p->streamPos - p->pos > p->keepSizeAfter)
      return;
  }
}

void MatchFinder_MoveBlock(CMatchFinder *p)
{
  memmove(p->bufferBase,
      p->buffer - p->keepSizeBefore,
      (size_t)(p->streamPos - p->pos) + p->keepSizeBefore);
  p->buffer = p->bufferBase + p->keepSizeBefore;
}

int MatchFinder_NeedMove(CMatchFinder *p)
{
  if (p->directInput)
    return 0;
  /* if (p->streamEndWasReached) return 0; */
  return ((size_t)(p->bufferBase + p->blockSize - p->buffer) <= p->keepSizeAfter);
}

void MatchFinder_ReadIfRequired(CMatchFinder *p)
{
  if (p->streamEndWasReached)
    return;
  if (p->keepSizeAfter >= p->streamPos - p->pos)
    MatchFinder_ReadBlock(p);
}

static void MatchFinder_CheckAndMoveAndRead(CMatchFinder *p)
{
  if (MatchFinder_NeedMove(p))
    MatchFinder_MoveBlock(p);
  MatchFinder_ReadBlock(p);
}

static void MatchFinder_SetDefaultSettings(CMatchFinder *p)
{
  p->cutValue = 32;
  p->btMode = 1;
  p->numHashBytes = 4;
  p->bigHash = 0;
}

#define kCrcPoly 0xEDB88320

void MatchFinder_Construct(CMatchFinder *p)
{
  unsigned i;
  p->bufferBase = NULL;
  p->directInput = 0;
  p->hash = NULL;
  p->expectedDataSize = (UInt64)(Int64)-1;
  MatchFinder_SetDefaultSettings(p);

  for (i = 0; i < 256; i++)
  {
    UInt32 r = (UInt32)i;
    unsigned j;
    for (j = 0; j < 8; j++)
      r = (r >> 1) ^ (kCrcPoly & ((UInt32)0 - (r & 1)));
    p->crc[i] = r;
  }
}

static void MatchFinder_FreeThisClassMemory(CMatchFinder *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->hash);
  p->hash = NULL;
}

void MatchFinder_Free(CMatchFinder *p, ISzAllocPtr alloc)
{
  MatchFinder_FreeThisClassMemory(p, alloc);
  LzInWindow_Free(p, alloc);
}

static CLzRef* AllocRefs(size_t num, ISzAllocPtr alloc)
{
  size_t sizeInBytes = (size_t)num * sizeof(CLzRef);
  if (sizeInBytes / sizeof(CLzRef) != num)
    return NULL;
  return (CLzRef *)ISzAlloc_Alloc(alloc, sizeInBytes);
}

int MatchFinder_Create(CMatchFinder *p, UInt32 historySize,
    UInt32 keepAddBufferBefore, UInt32 matchMaxLen, UInt32 keepAddBufferAfter,
    ISzAllocPtr alloc)
{
  UInt32 sizeReserv; // how much of historySize i.e. dictSize to reserve initially
  
  if (historySize > kMaxHistorySize)
  {
    MatchFinder_Free(p, alloc);
    return 0;
  }
  
  sizeReserv = historySize >> 1;
       if (historySize >= ((UInt32)3 << 30)) sizeReserv = historySize >> 3;
  else if (historySize >= ((UInt32)2 << 30)) sizeReserv = historySize >> 2;
  
  sizeReserv += (keepAddBufferBefore + matchMaxLen + keepAddBufferAfter) / 2 + (1 << 19);

  p->keepSizeBefore = historySize + keepAddBufferBefore + 1;
  p->keepSizeAfter = matchMaxLen + keepAddBufferAfter;
  
  /* we need one additional byte, since we use MoveBlock after pos++ and before dictionary using */
  
  if (LzInWindow_Create(p, sizeReserv, alloc))
  {
    UInt32 newCyclicBufferSize = historySize + 1;
    UInt32 hs;
    p->matchMaxLen = matchMaxLen;
    {
      p->fixedHashSize = 0;
      if (p->numHashBytes == 2)
        hs = (1 << 16) - 1;
      else
      {
        hs = historySize;
        if (hs > p->expectedDataSize)
          hs = (UInt32)p->expectedDataSize;
        if (hs != 0)
          hs--;
        hs |= (hs >> 1);
        hs |= (hs >> 2);
        hs |= (hs >> 4);
        hs |= (hs >> 8);
        hs >>= 1;
        hs |= 0xFFFF; /* don't change it! It's required for Deflate */
        if (hs > (1 << 24))
        {
          if (p->numHashBytes == 3)
            hs = (1 << 24) - 1;
          else
            hs >>= 1;
          /* if (bigHash) mode, GetHeads4b() in LzFindMt.c needs (hs >= ((1 << 24) - 1))) */
        }
      }
      /* p->hashMask is set based on dictionary size 
      * which is inturn set based on level and input data size
      * This determines the max number of bits up to which a value can be stored in the hash
      * E.g: historySize (4MB)  : hashMask (0x1F FFFF)
      * E.g: historySize (16MB) : hashMask (0x7F FFFF)
      * E.g: historySize (64MB) : hashMask (0xFF FFFF)
      */
      p->hashMask = hs;
      hs++; // size of hash table required
      if (p->numHashBytes > 2) p->fixedHashSize += kHash2Size; // space for additional hash tables : 2-byte hash
      if (p->numHashBytes > 3) p->fixedHashSize += kHash3Size; // space for additional hash tables : 3-byte hash
      if (p->numHashBytes > 4) p->fixedHashSize += kHash4Size; // not used
      hs += p->fixedHashSize; // space for all hash tables combined
    }

    {
      size_t newSize;
      size_t numSons;
      p->historySize = historySize;
      p->hashSizeSum = hs;
      p->cyclicBufferSize = newCyclicBufferSize;
      
      numSons = newCyclicBufferSize;
      if (p->btMode)
        numSons <<= 1; //for bt, son is : <leftNode0> <rightNode0> <leftNode1> <rightNode1>... Hence 2x size.
      newSize = hs + numSons;

      if (p->hash && p->numRefs == newSize)
        return 1;
      
      MatchFinder_FreeThisClassMemory(p, alloc);
      p->numRefs = newSize;
      p->hash = AllocRefs(newSize, alloc);
      
      if (p->hash)
      {
        p->son = p->hash + p->hashSizeSum;
        return 1;
      }
    }
  }

  MatchFinder_Free(p, alloc);
  return 0;
}

static void MatchFinder_SetLimits(CMatchFinder *p)
{
  UInt32 limit = kMaxValForNormalize - p->pos;
  UInt32 limit2 = p->cyclicBufferSize - p->cyclicBufferPos;
  
  if (limit2 < limit)
    limit = limit2;
  limit2 = p->streamPos - p->pos;
  
  if (limit2 <= p->keepSizeAfter)
  {
    if (limit2 > 0)
      limit2 = 1;
  }
  else
    limit2 -= p->keepSizeAfter;
  
  if (limit2 < limit)
    limit = limit2;
  
  {
    UInt32 lenLimit = p->streamPos - p->pos;
    if (lenLimit > p->matchMaxLen)
      lenLimit = p->matchMaxLen;
    p->lenLimit = lenLimit;
  }
  p->posLimit = p->pos + limit;
}


void MatchFinder_Init_LowHash(CMatchFinder *p)
{
  size_t i;
  CLzRef *items = p->hash;
  size_t numItems = p->fixedHashSize;
  for (i = 0; i < numItems; i++)
    items[i] = kEmptyHashValue;
}


void MatchFinder_Init_HighHash(CMatchFinder *p)
{
  size_t i;
  CLzRef *items = p->hash + p->fixedHashSize;
  size_t numItems = (size_t)p->hashMask + 1;
  for (i = 0; i < numItems; i++)
    items[i] = kEmptyHashValue;
}


void MatchFinder_Init_3(CMatchFinder *p, int readData)
{
  p->cyclicBufferPos = 0;
  p->buffer = p->bufferBase;
  p->pos =
  p->streamPos = p->cyclicBufferSize;
  p->result = SZ_OK;
  p->streamEndWasReached = 0;
  
  if (readData)
    MatchFinder_ReadBlock(p);
  
  MatchFinder_SetLimits(p);
}


void MatchFinder_Init(CMatchFinder *p)
{
  MatchFinder_Init_HighHash(p);
  MatchFinder_Init_LowHash(p);
  MatchFinder_Init_3(p, True);
}

  
static UInt32 MatchFinder_GetSubValue(CMatchFinder *p)
{
  return (p->pos - p->historySize - 1) & kNormalizeMask;
}

void MatchFinder_Normalize3(UInt32 subValue, CLzRef *items, size_t numItems)
{
  size_t i;
  for (i = 0; i < numItems; i++)
  {
    UInt32 value = items[i];
    if (value <= subValue)
      value = kEmptyHashValue;
    else
      value -= subValue;
    items[i] = value;
  }
}

static void MatchFinder_Normalize(CMatchFinder *p)
{
  UInt32 subValue = MatchFinder_GetSubValue(p);
  MatchFinder_Normalize3(subValue, p->hash, p->numRefs);
  MatchFinder_ReduceOffsets(p, subValue);
}


MY_NO_INLINE
static void MatchFinder_CheckLimits(CMatchFinder *p)
{
  if (p->pos == kMaxValForNormalize)
    MatchFinder_Normalize(p);
  if (!p->streamEndWasReached && p->keepSizeAfter == p->streamPos - p->pos)
    MatchFinder_CheckAndMoveAndRead(p);
  if (p->cyclicBufferPos == p->cyclicBufferSize) // reached end of look-ahead buffer, re-start counter
    p->cyclicBufferPos = 0;
  MatchFinder_SetLimits(p);
}

/*
  (lenLimit > maxLen)
*/
/*
* This function is used for finding matches when hashChain mode
* algorithm is selected. It navigates through the linked list based 
* hash chain dictionary and looks for matches at these positions.
* 
* Finds <len,dist> pairs for matches found in dictionary
* for input stream in cur[0...lenLimit] and saves them in 'distances'
* @output: distances
*
* @inputs:
* lenLimit: max length match we are looking for
* curMatch: position value from hash table for Hash(cur[0-4])
*           we expect to find matching bytes at this offset from cur
* pos : position value of cur
* *cur: pointer to current byte in stream
* *son: pointer to dictionary
* _cyclicBufferPos: current position in cyclic buffer
* _cyclicBufferSize: size of cyclic buffer
* cutValue: hard limit on number of nodes to look at when searching for matches
* *distances: output array to store <len,dist> pairs that are found
* maxLen: minimum length of match we should be able to find in
* dictionary (maxLen is previously set by searching for match in UPDATE_maxLen)
*
* 'son' holds the dictionary
* The dictionary is made up of multiple linked lists (hash chains)
* son[x] either has 0, if no link exists, or a link to next ()
* item in the linked list (pointers themselves are not stored in the 
* dictionary. Instead UInt32 values that map to position are stored)
* E.g:
* abcdef...1234....abcdexy...123...abcd....
* <----------------<---------------
*          <-----------------
* root node of each of hash chains is an entry from hash table (curMatch)
* Every call to Hc_GetMatchesSpec, adds curMatch to the dictionary
*/
MY_FORCE_INLINE
static UInt32 * Hc_GetMatchesSpec(unsigned lenLimit, UInt32 curMatch, UInt32 pos, const Byte *cur, CLzRef *son,
    UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 cutValue,
    UInt32 *distances, unsigned maxLen)
{
  /*
  son[_cyclicBufferPos] = curMatch;
  for (;;)
  {
    UInt32 delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
      return distances;
    {
      const Byte *pb = cur - delta;
      curMatch = son[_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)];
      if (pb[maxLen] == cur[maxLen] && *pb == *cur)
      {
        UInt32 len = 0;
        while (++len != lenLimit)
          if (pb[len] != cur[len])
            break;
        if (maxLen < len)
        {
          maxLen = len;
          *distances++ = len;
          *distances++ = delta - 1;
          if (len == lenLimit)
            return distances;
        }
      }
    }
  }
  */

  const Byte *lim = cur + lenLimit; // do not check for length beyond this
  son[_cyclicBufferPos] = curMatch; // add curMatch to dictionary
  do
  {
    UInt32 delta = pos - curMatch;
    /* range of pos [_cyclicBufferSize, _cyclicBufferSize+N)
    * when no link exists at a particular position, it will have value 0
    * by default. When we reach such a node, curMatch become 0
    * delta becomes >= _cyclicBufferSize
    * return at this point. search completed.*/
    if (delta >= _cyclicBufferSize)
      break;
    {
      ptrdiff_t diff;
      curMatch = son[_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)];  // offset where you expect to find a match
      diff = (ptrdiff_t)0 - delta;
      /* cur[0:maxlen - 1] are already matched by this iteration
      *  Just need to check from cur[maxLen] onwards */
      if (cur[maxLen] == cur[maxLen + diff])
      {
        const Byte *c = cur;
        while (*c == c[diff]) // compare bytes cur wrt previous match
        {
          if (++c == lim) // length of match reached lenlimit, no need to match further
          {
            distances[0] = (UInt32)(lim - cur);
            distances[1] = delta - 1;
            return distances + 2; // return <len,dist> pairs found
          }
        }
        {
          unsigned len = (unsigned)(c - cur);
          if (maxLen < len) // found a match longer than maxLen. save <len,dist> pair in distances
          {
            maxLen = len;
            distances[0] = (UInt32)len;
            distances[1] = delta - 1;
            distances += 2;
          }
        }
      }
    }
  }
  while (--cutValue); // limit number of nodes in hash chain to look at for matches
  
  return distances;
}

/*
* This function is used for finding matches when binTree mode
* algorithm is selected. It navigates through the binary tree based 
* dictionary and looks for matches at these positions.
* 
* Finds <len,dist> pairs for matches found in dictionary
* for input stream in cur[0...lenLimit] and saves them in 'distances'
* @output: distances
*
* @inputs:
* lenLimit: max length match we are looking for
* curMatch: position value from hash table for Hash(cur[0-4])
*           we expect to find matching bytes at this offset from cur
* pos : position value of cur
* *cur: pointer to current byte in stream
* *son: pointer to dictionary
* _cyclicBufferPos: current position in cyclic buffer
* _cyclicBufferSize: size of cyclic buffer
* cutValue: hard limit on number of nodes to look at when searching for matches
* *distances: output array to store <len,dist> pairs that are found
* maxLen: minimum length of match we should be able to find in
* dictionary (maxLen is previously set by searching for match in UPDATE_maxLen)
*
* 'son' holds the dictionary
* The dictionary is made up of multiple binary trees
* ptr0 points to right-child node
* ptr1 points to left-child node
* root node of each of the binary trees is an entry from hash table (curMatch)
*
* Pointers themselves are not stored in the dictionary. Instead UInt32 values
* that indicate position are used. 'pos', 'curMatch' contain such position values.
* E.g:          [abcd]nodeP=450
*                  |
*         ------------------
*         |                |
* [abcc]nodeL=102   [abce]nodeR=205
* At offset of (450-102) from current position [abcd], you have a match [abcc]
*
* Every call to GetMatchesSpec1, adds curMatch to the dictionary (unless a
* match longer than lenLimit is found)
*/
MY_FORCE_INLINE
UInt32 * GetMatchesSpec1(UInt32 lenLimit, UInt32 curMatch, UInt32 pos, const Byte *cur, CLzRef *son,
    UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 cutValue,
    UInt32 *distances, UInt32 maxLen)
{
  CLzRef *ptr0 = son + ((size_t)_cyclicBufferPos << 1) + 1; // right child node
  CLzRef *ptr1 = son + ((size_t)_cyclicBufferPos << 1); // left child node
  unsigned len0 = 0, len1 = 0; // len of match found in right and left branch respectively
  
  /* Look through binary tree associated with curMatch if one exists
  * Find matches and save <len, dist> pairs in distances */
  for (;;)
  {
    UInt32 delta = pos - curMatch; // offset to match wrt cur
    
    if (cutValue-- == 0 || delta >= _cyclicBufferSize) // search only within dictionary
    {
      /* range of pos [_cyclicBufferSize, _cyclicBufferSize+N)
      * when no entry exits for a particular leaf node in tree, it will have value 0
      * by default. When we reach such a node, curMatch become 0
      * delta becomes >= _cyclicBufferSize
      * return at this point. search completed.*/
      *ptr0 = *ptr1 = kEmptyHashValue;
      return distances; // search completed return
    }
    {
      // pair of <left,right> node at offset delta within circular buffer
      CLzRef *pair = son + ((size_t)(_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)) << 1);
      const Byte *pb = cur - delta; // previous bytes in stream to compare cur with

      /* We can start comparing from len onwards as pb[0:len-1] == cur[0:len-1]
      until this iteration. Nodes further down the tree will satisfy this condition
      and hopefully match some more bytes. Why select min(len0, len1) though? */
      unsigned len = (len0 < len1 ? len0 : len1);
      UInt32 pair0 = pair[0];
      if (pb[len] == cur[len]) // bytes matched
      {
        if (++len != lenLimit && pb[len] == cur[len])
          while (++len != lenLimit) // keep matching until you get a mismatch byte or reach lenLimit
            if (pb[len] != cur[len])
              break;
        if (maxLen < len) // found a match longer than maxLen. save <len,dist> pair in distances
        {
          maxLen = (UInt32)len;
          *distances++ = (UInt32)len; // save len of match
          *distances++ = delta - 1; // save dist of match
          if (len == lenLimit) // match of lenLimit found. No need to check further. return.
          {
            *ptr1 = pair0;
            *ptr0 = pair[1];
            return distances;
          }
        }
      }

      /* pb[0 to len-1] == cur[0 to len-1]
      * pb[len] != cur[len]. Mismatch on this byte. */
      if (pb[len] < cur[len]) // previous byte is smaller. set curMatch to left child node 
      {
        *ptr1 = curMatch;
        ptr1 = pair + 1; // move to matched left child node
        curMatch = *ptr1; // propagate matched position
        len1 = len;
      }
      else // previous byte is larger. set curMatch to right child node 
      {
        *ptr0 = curMatch;
        ptr0 = pair;  // move to matched right child node
        curMatch = *ptr0; // propagate matched position
        len0 = len;
      }
    }
  }
}

// Exact same code as GetMatchesSpec1, except this does not update distances
static void SkipMatchesSpec(UInt32 lenLimit, UInt32 curMatch, UInt32 pos, const Byte *cur, CLzRef *son,
    UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 cutValue)
{
  CLzRef *ptr0 = son + ((size_t)_cyclicBufferPos << 1) + 1;
  CLzRef *ptr1 = son + ((size_t)_cyclicBufferPos << 1);
  unsigned len0 = 0, len1 = 0;
  for (;;)
  {
    UInt32 delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
    {
      *ptr0 = *ptr1 = kEmptyHashValue;
      return;
    }
    {
      CLzRef *pair = son + ((size_t)(_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)) << 1);
      const Byte *pb = cur - delta;
      unsigned len = (len0 < len1 ? len0 : len1);
      if (pb[len] == cur[len])
      {
        while (++len != lenLimit)
          if (pb[len] != cur[len])
            break;
        {
          if (len == lenLimit)
          {
            *ptr1 = pair[0];
            *ptr0 = pair[1];
            return;
          }
        }
      }
      if (pb[len] < cur[len])
      {
        *ptr1 = curMatch;
        ptr1 = pair + 1;
        curMatch = *ptr1;
        len1 = len;
      }
      else
      {
        *ptr0 = curMatch;
        ptr0 = pair;
        curMatch = *ptr0;
        len0 = len;
      }
    }
  }
}

#define MOVE_POS \
  ++p->cyclicBufferPos; \
  p->buffer++; \
  if (++p->pos == p->posLimit) MatchFinder_CheckLimits(p);

#define MOVE_POS_RET MOVE_POS return (UInt32)offset;

static void MatchFinder_MovePos(CMatchFinder *p) { MOVE_POS; }

#define GET_MATCHES_HEADER2(minLen, ret_op) \
  unsigned lenLimit; UInt32 hv; const Byte *cur; UInt32 curMatch; \
  lenLimit = (unsigned)p->lenLimit; { if (lenLimit < minLen) { MatchFinder_MovePos(p); ret_op; }} \
  cur = p->buffer;

#define GET_MATCHES_HEADER(minLen) GET_MATCHES_HEADER2(minLen, return 0)
#define SKIP_HEADER(minLen)        GET_MATCHES_HEADER2(minLen, continue)

#define MF_PARAMS(p) p->pos, p->buffer, p->son, p->cyclicBufferPos, p->cyclicBufferSize, p->cutValue

#define GET_MATCHES_FOOTER(offset, maxLen) \
  offset = (unsigned)(GetMatchesSpec1((UInt32)lenLimit, curMatch, MF_PARAMS(p), \
  distances + offset, (UInt32)maxLen) - distances); MOVE_POS_RET;

#define SKIP_FOOTER \
  SkipMatchesSpec((UInt32)lenLimit, curMatch, MF_PARAMS(p)); MOVE_POS;

#define UPDATE_maxLen { \
    ptrdiff_t diff = (ptrdiff_t)0 - d2; \
    const Byte *c = cur + maxLen; \
    const Byte *lim = cur + lenLimit; \
    for (; c != lim; c++) if (*(c + diff) != *c) break; \
    maxLen = (unsigned)(c - cur); }

static UInt32 Bt2_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  unsigned offset;
  GET_MATCHES_HEADER(2)
  HASH2_CALC;
  curMatch = p->hash[hv];
  p->hash[hv] = p->pos;
  offset = 0;
  GET_MATCHES_FOOTER(offset, 1)
}

UInt32 Bt3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  unsigned offset;
  GET_MATCHES_HEADER(3)
  HASH_ZIP_CALC;
  curMatch = p->hash[hv];
  p->hash[hv] = p->pos;
  offset = 0;
  GET_MATCHES_FOOTER(offset, 2)
}

static UInt32 Bt3_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, d2, pos;
  unsigned maxLen, offset;
  UInt32 *hash;
  GET_MATCHES_HEADER(3)

  HASH3_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash[h2];

  curMatch = (hash + kFix3HashSize)[hv];
  
  hash[h2] = pos;
  (hash + kFix3HashSize)[hv] = pos;

  maxLen = 2;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    UPDATE_maxLen
    distances[0] = (UInt32)maxLen;
    distances[1] = d2 - 1;
    offset = 2;
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec((UInt32)lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET; // process and return offset
    }
  }
  
  GET_MATCHES_FOOTER(offset, maxLen) // process and return offset
}

/* Compute hash, locate node in binTree and find matches at these positions.
* Hash is computed on 4 bytes: Hash(p->buffer[0-4]).
* GetMatchesSpec1() is called to find matches in binTree.
* 
* @outputs: 
* return: number of matched pairs
* distances: matches saved as <len,dist> pairs
* 
* @inputs
* p: match finder struct
* p->buffer: pointer to current byte in input stream */
static UInt32 Bt4_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, d2, d3, pos;
  unsigned maxLen, offset;
  UInt32 *hash;
  GET_MATCHES_HEADER(4)

  /* read 4 bytes from input stream p->buffer and compute hash for it
  * cur = p->buffer
  * cur[0-4] are used to compute hash
  * 
  * compute hashes: h2, h3, hv
  * 3 hash tables are created and stored in 'hash'
  * H2 (10-bit hash) created using c[0-1] stored from [0, kFix3HashSize)
  * H3 (16-bit hash) created using c[0-2] stored from [kFix3HashSize, kFix4HashSize)
  * HV (hashMask   ) created using c[0-3] stored from [kFix4HashSize, max hash size) */
  HASH4_CALC;

  hash = p->hash;
  pos = p->pos;

  /* Eg:
  *                 curMatch                                                   pos
  *              ------------------------------------------------------------------
  * Data stream: ...abcd..............abcw.................abxy................abcd
  *                                                        <---------d2------->
  *                                   <-------------------d3------------------>
  *                 <---------------------------delta------------------------->
  * 
  * How does hash guarentee 2/3/4 bytes are same? 
  * Can't we have 2 sequences e.g. abcd, uvwx map to the same hash? */
  d2 = pos - hash                  [h2]; // dist to last 2 byte match
  d3 = pos - (hash + kFix3HashSize)[h3]; // dist to last 3 byte match

  curMatch = (hash + kFix4HashSize)[hv]; // dist to last 4 byte match

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;
  
  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur) // match of atleast len 2
  {
    maxLen = 2;
    distances[0] = 2; // set len
    distances[1] = d2 - 1; // set dist to match
    offset = 2;
  }
  
  if (d2 != d3 && d3 < p->cyclicBufferSize && *(cur - d3) == *cur) // match of atleast len 3
  {
    maxLen = 3;
    distances[(size_t)offset + 1] = d3 - 1; // set dist to match. len set below
    offset += 2;
    d2 = d3;
  }
  
  if (offset != 0)
  {
    /* 2 or 3 bytes are already matched at this point (maxLen)
    * starting from maxLen+1, find how many bytes match at offset d2 
    * update maxLen to this length
    * Data stream: ....ab****..............ab$$$$
    *                  <--------d2-------->
    *                  match *s and $s */
    UPDATE_maxLen
    distances[(size_t)offset - 2] = (UInt32)maxLen; // set len
    
    /* if already reached lenLimit, there is no need to check for better 
    * matches at d3 and curMatch offsets, as d2 will have the smallest offset
    * why is lenLimit != 273? */ 
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec((UInt32)lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET;
    }
  }
  
  if (maxLen < 3)
    maxLen = 3;
  
  GET_MATCHES_FOOTER(offset, maxLen)
}

/*
static UInt32 Bt5_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, h4, d2, d3, d4, maxLen, offset, pos;
  UInt32 *hash;
  GET_MATCHES_HEADER(5)

  HASH5_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash                  [h2];
  d3 = pos - (hash + kFix3HashSize)[h3];
  d4 = pos - (hash + kFix4HashSize)[h4];

  curMatch = (hash + kFix5HashSize)[hv];

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[h4] = pos;
  (hash + kFix5HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    distances[0] = maxLen = 2;
    distances[1] = d2 - 1;
    offset = 2;
    if (*(cur - d2 + 2) == cur[2])
      distances[0] = maxLen = 3;
    else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
    {
      distances[2] = maxLen = 3;
      distances[3] = d3 - 1;
      offset = 4;
      d2 = d3;
    }
  }
  else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
  {
    distances[0] = maxLen = 3;
    distances[1] = d3 - 1;
    offset = 2;
    d2 = d3;
  }
  
  if (d2 != d4 && d4 < p->cyclicBufferSize
      && *(cur - d4) == *cur
      && *(cur - d4 + 3) == *(cur + 3))
  {
    maxLen = 4;
    distances[(size_t)offset + 1] = d4 - 1;
    offset += 2;
    d2 = d4;
  }
  
  if (offset != 0)
  {
    UPDATE_maxLen
    distances[(size_t)offset - 2] = maxLen;
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec(lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET;
    }
  }

  if (maxLen < 4)
    maxLen = 4;
  
  GET_MATCHES_FOOTER(offset, maxLen)
}
*/

/* Compute hash, locate node in hashChian and find matches at these positions.
* Hash is computed on 4 bytes: Hash(p->buffer[0-4]).
* Hc_GetMatchesSpec() is called to find matches in hashChain.
*
* @outputs:
* return: number of matched pairs
* distances: matches saved as <len,dist> pairs
*
* @inputs
* p: match finder struct
* p->buffer: pointer to current byte in input stream */
static UInt32 Hc4_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, d2, d3, pos;
  unsigned maxLen, offset;
  UInt32 *hash;
  GET_MATCHES_HEADER(4)

  HASH4_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash                  [h2];
  d3 = pos - (hash + kFix3HashSize)[h3];
  curMatch = (hash + kFix4HashSize)[hv];

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    maxLen = 2;
    distances[0] = 2;
    distances[1] = d2 - 1;
    offset = 2;
  }
  
  if (d2 != d3 && d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
  {
    maxLen = 3;
    distances[(size_t)offset + 1] = d3 - 1;
    offset += 2;
    d2 = d3;
  }
  
  if (offset != 0)
  {
    UPDATE_maxLen
    distances[(size_t)offset - 2] = (UInt32)maxLen;
    if (maxLen == lenLimit)
    {
      p->son[p->cyclicBufferPos] = curMatch;
      MOVE_POS_RET;
    }
  }
  
  if (maxLen < 3)
    maxLen = 3;

  offset = (unsigned)(Hc_GetMatchesSpec(lenLimit, curMatch, MF_PARAMS(p),
      distances + offset, maxLen) - (distances));
  MOVE_POS_RET
}

/*
static UInt32 Hc5_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, h4, d2, d3, d4, maxLen, offset, pos
  UInt32 *hash;
  GET_MATCHES_HEADER(5)

  HASH5_CALC;

  hash = p->hash;
  pos = p->pos;
  
  d2 = pos - hash                  [h2];
  d3 = pos - (hash + kFix3HashSize)[h3];
  d4 = pos - (hash + kFix4HashSize)[h4];

  curMatch = (hash + kFix5HashSize)[hv];

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[h4] = pos;
  (hash + kFix5HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    distances[0] = maxLen = 2;
    distances[1] = d2 - 1;
    offset = 2;
    if (*(cur - d2 + 2) == cur[2])
      distances[0] = maxLen = 3;
    else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
    {
      distances[2] = maxLen = 3;
      distances[3] = d3 - 1;
      offset = 4;
      d2 = d3;
    }
  }
  else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
  {
    distances[0] = maxLen = 3;
    distances[1] = d3 - 1;
    offset = 2;
    d2 = d3;
  }
  
  if (d2 != d4 && d4 < p->cyclicBufferSize
      && *(cur - d4) == *cur
      && *(cur - d4 + 3) == *(cur + 3))
  {
    maxLen = 4;
    distances[(size_t)offset + 1] = d4 - 1;
    offset += 2;
    d2 = d4;
  }
  
  if (offset != 0)
  {
    UPDATE_maxLen
    distances[(size_t)offset - 2] = maxLen;
    if (maxLen == lenLimit)
    {
      p->son[p->cyclicBufferPos] = curMatch;
      MOVE_POS_RET;
    }
  }
  
  if (maxLen < 4)
    maxLen = 4;

  offset = (UInt32)(Hc_GetMatchesSpec(lenLimit, curMatch, MF_PARAMS(p),
      distances + offset, maxLen) - (distances));
  MOVE_POS_RET
}
*/

UInt32 Hc3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  unsigned offset;
  GET_MATCHES_HEADER(3)
  HASH_ZIP_CALC;
  curMatch = p->hash[hv];
  p->hash[hv] = p->pos;
  offset = (unsigned)(Hc_GetMatchesSpec(lenLimit, curMatch, MF_PARAMS(p),
      distances, 2) - (distances));
  MOVE_POS_RET
}

static void Bt2_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    SKIP_HEADER(2)
    HASH2_CALC;
    curMatch = p->hash[hv];
    p->hash[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

void Bt3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    SKIP_HEADER(3)
    HASH_ZIP_CALC;
    curMatch = p->hash[hv];
    p->hash[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

static void Bt3_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2;
    UInt32 *hash;
    SKIP_HEADER(3)
    HASH3_CALC;
    hash = p->hash;
    curMatch = (hash + kFix3HashSize)[hv];
    hash[h2] =
    (hash + kFix3HashSize)[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

static void Bt4_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3;
    UInt32 *hash;
    SKIP_HEADER(4) // Move to next char in buffer. 'cur' points to it. Does other limit checks
    HASH4_CALC; // Calculate hash for 'cur'
    hash = p->hash;
    curMatch = (hash + kFix4HashSize)[hv];
    // set 'pos' at respective slot in hash table
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

/*
static void Bt5_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3, h4;
    UInt32 *hash;
    SKIP_HEADER(5)
    HASH5_CALC;
    hash = p->hash;
    curMatch = (hash + kFix5HashSize)[hv];
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[h4] =
    (hash + kFix5HashSize)[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}
*/

static void Hc4_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3;
    UInt32 *hash;
    SKIP_HEADER(4)
    HASH4_CALC;
    hash = p->hash;
    curMatch = (hash + kFix4HashSize)[hv];
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[hv] = p->pos;
    p->son[p->cyclicBufferPos] = curMatch;
    MOVE_POS
  }
  while (--num != 0);
}

/*
static void Hc5_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3, h4;
    UInt32 *hash;
    SKIP_HEADER(5)
    HASH5_CALC;
    hash = p->hash;
    curMatch = hash + kFix5HashSize)[hv];
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[h4] =
    (hash + kFix5HashSize)[hv] = p->pos;
    p->son[p->cyclicBufferPos] = curMatch;
    MOVE_POS
  }
  while (--num != 0);
}
*/

void Hc3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    SKIP_HEADER(3)
    HASH_ZIP_CALC;
    curMatch = p->hash[hv];
    p->hash[hv] = p->pos;
    p->son[p->cyclicBufferPos] = curMatch;
    MOVE_POS
  }
  while (--num != 0);
}

void MatchFinder_CreateVTable(CMatchFinder *p, IMatchFinder *vTable)
{
  vTable->Init = (Mf_Init_Func)MatchFinder_Init;
  vTable->GetNumAvailableBytes = (Mf_GetNumAvailableBytes_Func)MatchFinder_GetNumAvailableBytes;
  vTable->GetPointerToCurrentPos = (Mf_GetPointerToCurrentPos_Func)MatchFinder_GetPointerToCurrentPos;
  if (!p->btMode)
  {
    /* if (p->numHashBytes <= 4) */
    {
      vTable->GetMatches = (Mf_GetMatches_Func)Hc4_MatchFinder_GetMatches;
      vTable->Skip = (Mf_Skip_Func)Hc4_MatchFinder_Skip;
    }
    /*
    else
    {
      vTable->GetMatches = (Mf_GetMatches_Func)Hc5_MatchFinder_GetMatches;
      vTable->Skip = (Mf_Skip_Func)Hc5_MatchFinder_Skip;
    }
    */
  }
  else if (p->numHashBytes == 2)
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt2_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt2_MatchFinder_Skip;
  }
  else if (p->numHashBytes == 3)
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt3_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt3_MatchFinder_Skip;
  }
  else /* if (p->numHashBytes == 4). Default setting. */
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt4_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt4_MatchFinder_Skip;
  }
  /*
  else
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt5_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt5_MatchFinder_Skip;
  }
  */
}
