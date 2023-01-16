/* LzFind.h -- Match finder for LZ algorithms
2021-07-13 : Igor Pavlov : Public domain */

/**
 * Copyright (C) 2022, Advanced Micro Devices. All rights reserved.
 */

#ifndef __LZ_FIND_H
#define __LZ_FIND_H

#include "7zTypes.h"

EXTERN_C_BEGIN

typedef UInt32 CLzRef;

/* descriptions for hash table parameters below are based on
 * 4 - byte associated hash currently being used in source code*/
typedef struct _CMatchFinder
{
  Byte *buffer; // pointer to input data stream
  UInt32 pos;
  UInt32 posLimit;
  UInt32 streamPos;  /* wrap over Zero is allowed (streamPos < pos). Use (UInt32)(streamPos - pos) */
  UInt32 lenLimit;

  UInt32 cyclicBufferPos;  // these 'pos' values are saved in hash. range [0, cyclicBufferSize)
  UInt32 cyclicBufferSize; /* it must be = (historySize + 1) */

  Byte streamEndWasReached;
  Byte btMode;
  Byte bigHash;
  Byte directInput;

  UInt32 matchMaxLen;
  /* hash and son point to contiguous memory locations
* hash : hash tables
* son  : binary tree or hash chain
* hash                                          son
* ---------------------------------------------------------------------------
* | 2-byte table | 3-byte table | 4-byte table |
* ---------------------------------------------------------------------------
* <------fixedHashSize--------->
* <---------------hashSizeSum------------------><-------numSons-------------->
* numSons = cyclicBufferSize [for HC] and 2*cyclicBufferSize [for BT] algos
* numSons can be of fixed size, as max number of nodes that can be
* present in the dictionary at any given point = historySize */
  CLzRef *hash; // Initialized to all 0s. As and when matches are found, it is set to 'pos' of match
  CLzRef *son;
  UInt32 hashMask; // determines the max number of bits in the hash
  UInt32 cutValue; // hard limit on number of nodes to look at in BT/HC when searching for matches

  Byte *bufferBase;
  ISeqInStream *stream;
  
  UInt32 blockSize;
  UInt32 keepSizeBefore;
  UInt32 keepSizeAfter;

  UInt32 numHashBytes; // default 4 
  size_t directInputRem;
  UInt32 historySize; // size of search buffer aka dictionary
  UInt32 fixedHashSize; // size for all 2-byte, 3-byte associated hash tables
  UInt32 hashSizeSum; // size for all hash tables combined (2-byte, 3-byte, 4-byte)
  SRes result;
  UInt32 crc[256]; // crc used in hash calculation
  size_t numRefs;

  UInt64 expectedDataSize;
} CMatchFinder;

#define Inline_MatchFinder_GetPointerToCurrentPos(p) ((const Byte *)(p)->buffer)

#define Inline_MatchFinder_GetNumAvailableBytes(p) ((UInt32)((p)->streamPos - (p)->pos))

/*
#define Inline_MatchFinder_IsFinishedOK(p) \
    ((p)->streamEndWasReached \
        && (p)->streamPos == (p)->pos \
        && (!(p)->directInput || (p)->directInputRem == 0))
*/
      
int MatchFinder_NeedMove(CMatchFinder *p);
/* Byte *MatchFinder_GetPointerToCurrentPos(CMatchFinder *p); */
void MatchFinder_MoveBlock(CMatchFinder *p);
void MatchFinder_ReadIfRequired(CMatchFinder *p);

void MatchFinder_Construct(CMatchFinder *p);

/* Conditions:
     historySize <= 3 GB
     keepAddBufferBefore + matchMaxLen + keepAddBufferAfter < 511MB
*/
int MatchFinder_Create(CMatchFinder *p, UInt32 historySize,
    UInt32 keepAddBufferBefore, UInt32 matchMaxLen, UInt32 keepAddBufferAfter,
    ISzAllocPtr alloc);
void MatchFinder_Free(CMatchFinder *p, ISzAllocPtr alloc);
void MatchFinder_Normalize3(UInt32 subValue, CLzRef *items, size_t numItems);
// void MatchFinder_ReduceOffsets(CMatchFinder *p, UInt32 subValue);

/*
#define Inline_MatchFinder_InitPos(p, val) \
    (p)->pos = (val); \
    (p)->streamPos = (val);
*/

#define Inline_MatchFinder_ReduceOffsets(p, subValue) \
    (p)->pos -= (subValue); \
    (p)->streamPos -= (subValue);


UInt32 * GetMatchesSpec1(UInt32 lenLimit, UInt32 curMatch, UInt32 pos, const Byte *buffer, CLzRef *son,
    size_t _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 _cutValue,
    UInt32 *distances, UInt32 maxLen);

/*
Conditions:
  Mf_GetNumAvailableBytes_Func must be called before each Mf_GetMatchLen_Func.
  Mf_GetPointerToCurrentPos_Func's result must be used only before any other function
*/

typedef void (*Mf_Init_Func)(void *object);
typedef UInt32 (*Mf_GetNumAvailableBytes_Func)(void *object);
typedef const Byte * (*Mf_GetPointerToCurrentPos_Func)(void *object);
typedef UInt32 * (*Mf_GetMatches_Func)(void *object, UInt32 *distances);
typedef void (*Mf_Skip_Func)(void *object, UInt32);

typedef struct _IMatchFinder
{
  Mf_Init_Func Init;
  Mf_GetNumAvailableBytes_Func GetNumAvailableBytes;
  Mf_GetPointerToCurrentPos_Func GetPointerToCurrentPos;
  Mf_GetMatches_Func GetMatches;
  Mf_Skip_Func Skip;
} IMatchFinder2;

void MatchFinder_CreateVTable(CMatchFinder *p, IMatchFinder2 *vTable);

void MatchFinder_Init_LowHash(CMatchFinder *p);
void MatchFinder_Init_HighHash(CMatchFinder *p);
void MatchFinder_Init_4(CMatchFinder *p);
void MatchFinder_Init(CMatchFinder *p);

UInt32* Bt3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances);
UInt32* Hc3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances);

void Bt3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num);
void Hc3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num);

#ifdef AOCL_DYNAMIC_DISPATCHER
/* Conditions:
     historySize <= 3 GB
     keepAddBufferBefore + matchMaxLen + keepAddBufferAfter < 511MB
*/
int AOCL_MatchFinder_Create(CMatchFinder* p, UInt32 historySize,
  UInt32 keepAddBufferBefore, UInt32 matchMaxLen, UInt32 keepAddBufferAfter,
  ISzAllocPtr alloc);
void AOCL_MatchFinder_Free(CMatchFinder* p, ISzAllocPtr alloc);
void AOCL_MatchFinder_CreateVTable(CMatchFinder* p, IMatchFinder2* vTable);
#endif
EXTERN_C_END

#define GetUi16(p) (*(const UInt16 *)(const void *)(p))
#define GetUi32(p) (*(const UInt32 *)(const void *)(p))
#define SetUi16(p, v) { *(UInt16 *)(void *)(p) = (v); }
#define SetUi32(p, v) { *(UInt32 *)(void *)(p) = (v); }

#endif
