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

/*
parse:
utilities to parse things
it uses the C-locale and ascii or UTF-8 strings
*/

#ifndef LPI_PARSE_H_INCLUDED
#define LPI_PARSE_H_INCLUDED

#include <string>
#include <sstream>
#include <iomanip>

namespace lpi
{

//usage: std::string str = valtostr(25454.91654654f);
template<typename T>
std::string valtostr(const T& val)
{
  std::ostringstream sstream;
  sstream << val;
  return sstream.str();
}

//usage: double val = strtoval<double>("465498.654");
template<typename T>
T strtoval(const std::string& s)
{
  std::istringstream sstream(s);
  T val;
  sstream >> val;
  return val;
}

//length is decimal precision of the floating point number
template<typename T>
std::string valtostr(const T& val, int length, bool fixed = true)
{
  std::ostringstream sstream;
  if(fixed) sstream << std::fixed;
  sstream << std::setprecision(length) << val;
  return sstream.str();
}

bool isWhiteSpace(char c);
bool isDigit(char c);

int parseInteger(const std::string& s, size_t& pos, size_t end);
double parseDouble(const std::string& s, size_t& pos, size_t end); //parse double precision floating point number

//parse the string until the predicate returns true on the current character or until the end
void parseStringUntil(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char));

//parse the string until the predicate returns false on the current character or until the end
void parseStringWhile(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char));

//skip the string until the predicate returns true on the current character or until the end
void skipStringUntil(size_t& pos, size_t end, const std::string& in, bool predicate(char));

//skip the string until the predicate returns false on the current character or until the end
void skipStringWhile(size_t& pos, size_t end, const std::string& in, bool predicate(char));

//skip, then parse the string until the predicate returns true on the current character or until the end
void skipThenParseStringUntil(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char));

//skip, then parse the string until the predicate returns false on the current character or until the end
void skipThenParseStringWhile(std::string& out, size_t& pos, size_t end, const std::string& in, bool predicate(char));

#endif

} //namespace lpi
