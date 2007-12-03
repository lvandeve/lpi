
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LPI_UNITTEST_H_INCLUDED
#define LPI_UNITTEST_H_INCLUDED

/*
simple unit testing functionality

you have to define LUT_MY_RESET yourself for cleanup of parameters between each case

for example for a test on a GUI where I wanted the mouse to be controlled, the reset after every case was like this:

#define LUT_MY_RESET \
{\
  debugSetLMB(0);\
  debugSetRMB(0);\
  debugSetMousePos(0, 0);\
}
*/


#define LUT_START_UNIT_TEST \
  int _TEST_INDEX = -1;\
  int _TOTAL_ERRORS_TRIGGERED = 0;\
  int _LOCAL_ERRORS_TRIGGERED;\
  int _TESTS_FAILED = 0;\
  std::string _TEST_NAME;\
  std::string _ERROR_SIGNATURE;\
  std::ostringstream LUT_APPEND_MSG;\
  int silent = 0;\
  bool subError = 0;\
  bool subCheck = 0;\
  std::cout << "\nstarting unit test";

#define LUT_CASE(x) \
{\
  _TEST_INDEX++;\
  _LOCAL_ERRORS_TRIGGERED = 0;\
  _TEST_NAME = x;\
  _ERROR_SIGNATURE = "";\
  LUT_APPEND_MSG.str("");\
  LUT_MY_RESET;
  
#define LUT_SUB \
  subError = false;\
  if(subCheck) { std::cout << "\nbad sub!\n"; return; } subCheck = true;\


#define LUT_SUB_ERROR \
{\
  _TOTAL_ERRORS_TRIGGERED++;\
  _LOCAL_ERRORS_TRIGGERED++;\
  subError = true;\
  if(!subCheck) { std::cout << "\nbad sub!\n"; return; }\
}

#define LUT_SUB_END \
  if(subError) _ERROR_SIGNATURE += "-"; else _ERROR_SIGNATURE += "+";\
  if(!subCheck) { std::cout << "\nbad sub!\n"; return; } subCheck = false;\
  
#define LUT_SUB_ASSERT_FALSE(x) \
  LUT_SUB\
  if(x) LUT_SUB_ERROR\
  LUT_SUB_END
  
#define LUT_SUB_ASSERT_TRUE(x) \
  LUT_SUB\
  if(!(x)) LUT_SUB_ERROR\
  LUT_SUB_END
  
#define LUT_CASE_END \
  if(_LOCAL_ERRORS_TRIGGERED > 0) _TESTS_FAILED++;\
  if(!silent)\
  {\
    std::cout << "\n";\
    if(_LOCAL_ERRORS_TRIGGERED > 0) std::cout << " :E "; else std::cout << " :) ";\
    std::cout << "case " << _TEST_INDEX << ": " /*<< _LOCAL_ERRORS_TRIGGERED <<" errors "*/ << _ERROR_SIGNATURE << " \"" << _TEST_NAME << "\"";\
    if(LUT_APPEND_MSG.str().size() > 0) std::cout << " >>> " << LUT_APPEND_MSG.str();\
  }\
}

#define LUT_CASE_APPEND_MSG(x)\
  LUT_APPEND_MSG << x;


#define LUT_END_UNIT_TEST \
  std::cout << "\ncases ran: " << _TEST_INDEX + 1 << "\ncases failed: " << _TESTS_FAILED << "\ntotal errors: " << _TOTAL_ERRORS_TRIGGERED << "\n";
 
#endif
