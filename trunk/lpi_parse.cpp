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

#include "lpi_parse.h"

#include <cmath>
#include <sstream>

namespace lpi
{

bool isWhiteSpace(char c)
{
  return c < 33; //I consider everything with the ascii code of a space and lower to be whitespace
}

bool isDigit(char c)
{
  return c >= '0' && c <= '9';
}

int parseInteger(const std::string& s, size_t& pos, size_t end)
{
  if(pos >= end) return 0; //...
  
  std::string str;
  std::stringstream ss;
  int result;
  
  while(pos < end && (isDigit(s[pos]) || s[pos] == '-')) str += s[pos++];
  ss.str(str);
  ss >> result;
  
  return result;
}

double parseDouble(const std::string& s, size_t& pos, size_t end)
{
  if(pos >= end) return 0; //...
  
  std::string str;
  std::stringstream ss;
  double result;
  
  while(pos < end && (isDigit(s[pos]) || s[pos] == '-' || s[pos] == '.')) str += s[pos++];
  ss.str(str);
  ss >> result;
  
  return result;
}

void parseStringUntil(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char))
{
  while(pos < end && !predicate(in[pos]))
  {
    out.push_back(in[pos]);
    pos++;
  }
}

void parseStringWhile(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char))
{
  while(pos < end && predicate(in[pos]))
  {
    out.push_back(in[pos]);
    pos++;
  }
}

void skipStringUntil(size_t& pos, size_t end, const std::string& in, bool predicate(char))
{
  while(pos < end && !predicate(in[pos])) pos++;
}

void skipStringWhile(size_t& pos, size_t end, const std::string& in, bool predicate(char))
{
  while(pos < end && predicate(in[pos])) pos++;
}

void skipThenParseStringUntil(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char))
{
  skipStringWhile(pos, end, in, predicate);
  parseStringUntil(out, pos, end, in, predicate);
}

void skipThenParseStringWhile(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char))
{
  skipStringUntil(pos, end, in, predicate);
  parseStringWhile(out, pos, end, in, predicate);
}

} //namespace lpi
