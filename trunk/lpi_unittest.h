/*
Copyright (c) 2005-2008 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
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
