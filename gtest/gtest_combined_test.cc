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
 
 /** @file combined_test.cc
 *  
 *  @brief This combines all methods test cases into a single executable
 *
 *  Using the executable generated from this file you can choose
 *  the method you want to run, if none is choosed all methods, whose
 *  test cases are written will run.
 *
 *  @author J Niranjan Reddy
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <regex>
#include <iostream>
// gtest.h and windows.h have common #defines and min, max function definitions
// that conflict, so added the below `NOMINMAX`
#define NOMINMAX
#include "api/types.h"
#include "api/api.h"
#include "utils/utils.h"
#include <gtest/gtest.h>

#include "algos/lz4/lz4.h"
#include "algos/lz4/lz4hc.h"
#include "algos/snappy/snappy.h"
#include "algos/zlib/zlib.h"
#include "algos/lzma/LzmaDec.h"
#include "algos/lzma/LzmaEnc.h"

using namespace std;

// sets the optLevel of a method, if the optLevel crosses
// supported level by a machine, prints error and exits.
int setup(aocl_compression_type method, int optLevel)
{
   aocl_compression_desc handle;
   handle.optOff = 0;
   handle.optLevel = 0;
   handle.inSize = 0;
   handle.level = 0;
   handle.optVar = 0;
   // sets the highest supported optLevel at handle.optLevel
   // and also does setup once.
   aocl_llc_setup(&handle, method);
   // if the previous optLevel setup is different from the one
   // we requested, does the setup once again with requested optLevel.
   if (optLevel != handle.optLevel)
   {
#ifdef AOCL_DYNAMIC_DISPATCHER
      switch(method)
      {
         case LZ4:
            aocl_setup_lz4(0, optLevel, 0, 0, 0);
            break;
         case SNAPPY:
            snappy::aocl_setup_snappy(0, optLevel, 0, 0, 0);
            break;
         case ZLIB:
            aocl_setup_zlib(0, optLevel, 0, 0, 0);
            break;
         case LZMA:
            aocl_setup_lzma_encode(0, optLevel, 0, 0, 0);
            aocl_setup_lzma_decode(0, optLevel, 0, 0, 0);
            break;
         case LZ4HC:
            aocl_setup_lz4hc(0, optLevel, 0, 0, 0);
            break;
         default:
             cout << "Error: Unsupported method\n";
             break;
      }
#endif
   }
   else if(optLevel > handle.optLevel)
   {
      cout << "Error: Unsupported optLevel\n";
      exit(0);
   }
   return 0;
}

void print_filter_help() {
    cout << "There are two ways to run this program:\n\n";
    cout << "1.<program name> -<method name>:<optLevel>\n";
    cout << "2.<program name> -<method name>\n\n";
    cout << "You can also pass multiple methods as arguments,\n";
    cout << "by default it will run all supported method's tests if no arguemnts are passed.\n\n";
    cout << "Supported methods are\n";
    cout << "1. lz4\n";
    cout << "2. snappy\n";
    cout << "3. zlib\n";
    cout << "4. lzma\n\n";
    cout << "Examples:\n";
    cout << "gtest_combined_test -lz4\n";
    cout << "gtest_combined_test -snappy:2 -zlib:0\n";
}

// A filter is created and aocl_setup_<method> is called for respective methods 
// that is passed as input arguments to this executable.
string create_filter(vector<string> &prog)
{
   string filt;
   int num = 0;
   smatch match;
   aocl_compression_type method = AOCL_COMPRESSOR_ALGOS_NUM;

   for (auto s : prog)
   {
      // matches if argument is either -<method name> or -<method name>:<optLevel>
      // if any other arguments are passed, error message shows up and exits from program.
      if (regex_match(s, match, regex("-lz4(:\\d)?")))
      {
         filt += "LZ4_*:LLZ4_*";
         s = (string)match[1];
         num = s.size()==2 ? s[1]-'0' : 0;
         method = LZ4;
      }
      else if (regex_match(s, match, regex("-snappy(:\\d)?")))
      {
         filt += "SNAPPY*";
         s = (string)match[1];
         num = s.size()==2 ? s[1]-'0' : 0;
         method = SNAPPY;
      }
      else if (regex_match(s, match, regex("-zlib(:\\d)?")))
      {
         filt += "ZLIB*";
         s = (string)match[1];
         num = s.size()==2 ? s[1]-'0' : 0;
         method = ZLIB;
      }
      else if (regex_match(s, match, regex("-lzma(:\\d)?")))
      {
          filt += "LZMA*";
          s = (string)match[1];
          num = s.size() == 2 ? s[1] - '0' : 0;
          method = LZMA;
      }
      else if (regex_match(s, match, regex("-lz4hc(:\\d)?")))
      {
         filt += "LZ4HC*";
         s = (string)match[1];
         num = s.size() == 2 ? s[1]-'0' : 0;
         method = LZ4HC;
      }
      else if (regex_match(s, match, regex("-(zstd|bzip2)(:\\d)?")))
      {
         cout << "Error: Current testing framework for the method ";
         cout << match[1] << " is currently unsupported\n";
         print_filter_help();
         exit(0);
      }
      else
      {
         if(!(s == "-h" || s == "--help"))
         {
            cout << "Error: Unsupported way of passing arguments.\n\n";
         }
         print_filter_help();
         exit(0);
      }
      setup(method, num);
      filt += ":";
   }

   return filt;
}

int main(int argc, char *argv[])
{
   // This is a vector where each element points to seperate argument.
   vector<string> prog;
   for (int i = 1; i < argc; i++)
   {
      prog.push_back(argv[i]);
   }

   // If no arguments are passed all methods would be executed.
   if (prog.size() == 0)
   {
      prog.push_back("-lz4");
      prog.push_back("-snappy");
      prog.push_back("-zlib");
      prog.push_back("-lzma");
      prog.push_back("-lz4hc");
   }

   string filt = create_filter(prog);

   // Initial setup is done for all the methods, but will not be executed yet.
   ::testing::InitGoogleTest();
   // User selected methods if passed any or else all methods would be filtered.
   ::testing::GTEST_FLAG(filter) = filt;
   return RUN_ALL_TESTS();
}
