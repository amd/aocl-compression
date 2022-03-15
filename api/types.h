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
 
 /** @file types.h
 *  
 *  @brief Typedef declarations of data types 
 *
 *  This file contains the typedef declarations of different data types.
 *
 *  @author S. Biplab Raut
 */
 
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h> 

typedef int64_t INT64;          //fixed signed 64 bits int
typedef int32_t INT32;          //fixed signed 32 bits int : Use it for signed int
typedef ptrdiff_t INT;          //portable signed int type : 4 bytes (ILP32), 8 bytes (LP64)
typedef uint64_t UINT64;        //fixed unsigned 64 bits int
typedef uint32_t UINT32;        //fixed unsigned 32 bits int : Use it for unsigned int
typedef size_t UINT;            //portable unsigned int type : 4 bytes (ILP32), 8 bytes (LP64)
typedef char CHAR;              //signed character data type : 1 byte
typedef unsigned char UCHAR;    //unsigned character data type : 1 byte
typedef short SHORT;            //signed short integer : 2 bytes
typedef unsigned short USHORT;  //unsigned short integer : 2 bytes
typedef void VOID;              //void
typedef float FLOAT32;          //single-precision
typedef double FLOAT64;         //double-precision
typedef double DOUBLE;          //double-precision
typedef uint8_t UINT8;          //unsigned 1 byte integer
typedef int8_t INT8;            //signed 1 byte integer

#endif