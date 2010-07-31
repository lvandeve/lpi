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

////////////////////////////////////////////////////////////////////////////////

//will make the booleans at the positions of matching strings, true
void find(std::vector<bool>& matches, const std::string& text, const std::string& to_find)
{
  for(size_t i = 0; i < text.size(); i++)
  {
    bool match = true;
    
    for(size_t j = 0; j < to_find.size(); j++)
      if(text[i + j] != to_find[j]) { match = false; break; }

    if(match)
      for(size_t j = 0; j < to_find.size(); j++) matches[i + j] = true;
  }
}

void searchAndReplace(std::string& o_text, const std::string& i_text, const std::string& from, const std::string& to)
{
  for(size_t i = 0; i < i_text.size(); i++)
  {
    bool match = true;
    
    for(size_t j = 0; j < from.size(); j++)
      if(i_text[i + j] != from[j]) { match = false; break; }

    if(match)
    {
      o_text += to;
      i += from.size() - 1;
    }
    else o_text += i_text[i];
  }
}

void searchAndReplace(std::string& io_text, const std::string& from, const std::string& to)
{
  std::string result;
  searchAndReplace(result, io_text, from, to);
  io_text = result;
}

void searchAndReplace(std::string& o_text, const std::string& i_text, const std::string& from, const std::string& to, const std::vector<std::string>& exclude)
{
  std::vector<bool> exclude_chars; //exclude those characters
  exclude_chars.resize(i_text.size(), false);
  
  for(size_t i = 0; i < exclude.size(); i++) find(exclude_chars, i_text, exclude[i]);
  
  for(size_t i = 0; i < i_text.size(); i++)
  {
    bool match = true;
    
    for(size_t j = 0; j < from.size(); j++)
      if(i_text[i + j] != from[j] || exclude_chars[i]) { match = false; break; }

    if(match)
    {
      o_text += to;
      i += from.size() - 1;
    }
    else o_text += i_text[i];
  }
}

void searchAndReplace(std::string& io_text, const std::string& from, const std::string& to, const std::vector<std::string>& exclude)
{
  std::string result;
  searchAndReplace(result, io_text, from, to, exclude);
  io_text = result;
}

bool equalsIgnoreCase(const std::string& a, const std::string& b)
{
  if(a.size() != b.size()) return false;
  for(size_t i = 0; i < a.size(); i++)
  {
    if(std::tolower(a[i]) != std::tolower(b[i])) return false;
  }
  return true;
}


void splitString(std::vector<std::string>& out, const std::string& in, char delimiter)
{
  for(size_t i = 0; i < in.size(); i++)
  {
    if(in[i] == delimiter)
    {
      if(!out.empty() && !out.back().empty())
      {
        out.resize(out.size() + 1);
      }
    }
    else
    {
      if(out.empty()) out.resize(out.size() + 1);
      out.back() += in[i];
    }
  }
}

} //namespace lpi
