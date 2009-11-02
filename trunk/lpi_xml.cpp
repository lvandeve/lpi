/*
Copyright (c) 2005-2009 Lode Vandevenne
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

#include "lpi_xml.h"

#include <string>
#include <vector>
#include <sstream>

//parse <-> generate

namespace lpi
{
namespace xml
{

const std::string xml_newline_symbol = "\n";
std::string xml_indentation_symbol = "  ";
std::string xml_attr_quote_symbol = "\"";

////////////////////////////////////////////////////////////////////////////////
// 1.) General XML Tools
////////////////////////////////////////////////////////////////////////////////

void skipWhiteSpace(const std::string& in, size_t& pos, size_t end)
{
  while(pos < end && isWhiteSpace(in[pos])) pos++;
}

void skipToChar(const std::string& in, size_t& pos, size_t end, char c)
{
  while(pos < end && in[pos] != c) pos++;
}

bool isWhiteSpace(char c)
{
  return (c <= 32);
}

bool isCharacter(const std::string& in, size_t pos, size_t end, char c) //made to have the < size and == test in one function
{
  return(pos < end && in[pos] == c);
}

bool isNotCharacter(const std::string& in, size_t pos, size_t end, char c) //made to have the < size and != test in one function
{
  return(pos < end && in[pos] != c);
}

void entitify(std::string& out, const std::string& in, size_t pos, size_t end)
{
  out.reserve(out.size() + end - pos);
  for(size_t i = pos; i < end; i++)
  {
    if(in[i] == '&') out += "&amp;";
    else if(in[i] == '<') out += "&lt;";
    else if(in[i] == '>') out += "&gt;";
    else if(in[i] == '\'') out += "&apos;";
    else if(in[i] == '"') out += "&quot;";
    else out += in[i];
  }
}

void unentitify(std::string& out, const std::string& in, size_t pos, size_t end)
{
  out.reserve(out.size() + end - pos);
  for(size_t i = pos; i < end; i++)
  {
    if(in[i] == '&')
    {
      if(i > end - 3) break;
      if(in[i + 1] == 'a' && in[i + 2] == 'm') { out += "&"; i += 4; }
      else if(in[i + 1] == 'l') { out += "<"; i += 3; }
      else if(in[i + 1] == 'g') { out += ">"; i += 3; }
      else if(in[i + 1] == 'a' && in[i + 2] == 'p') { out += "'"; i += 5; }
      else if(in[i + 1] == 'l') { out += "\""; i += 5; }
    }
    else out += in[i];
  }
}

//////////////////////////////////////////////////////////////////////////////
void encodeBase64(const std::vector<unsigned char>& in, std::string& out)
{
  const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned long bit24 = 0;
  unsigned long sextet[4] = {0, 0, 0, 0};
  unsigned long octet[3] = {0, 0, 0};
  
  out.clear();
  out.reserve((4 * in.size()) / 3);
  
  for(size_t pos = 0; pos < in.size(); pos += 3)
  {
    octet[0] = in[pos + 0];
    octet[1] = (pos + 1 < in.size()) ? in[pos + 1] : 0;
    octet[2] = (pos + 2 < in.size()) ? in[pos + 2] : 0;
    
    bit24  = 256 * 256 * octet[0];
    bit24 +=       256 * octet[1];
    bit24 +=             octet[2];
    
    sextet[0] = (bit24 / (64 * 64 * 64)) % 64;
    sextet[1] = (bit24 / (     64 * 64)) % 64;
    sextet[2] = (bit24 / (          64)) % 64;
    sextet[3] = (bit24 / (           1)) % 64;
    
    for(size_t i = 0; i < 4; i++)
    {
      if(pos + i <= in.size()) out.push_back(characters[sextet[i]]);
      else out.push_back('=');
    }
    
    if(pos % 57 == 0 && pos != 0) out.push_back(10); //newline char every 76 chars (57 = 3/4th of 76)
  }
}

void decodeBase64(std::vector<unsigned char>& out, const std::string& in, size_t begin, size_t end)
{
  if(end == 0) end = in.size();
  size_t size = end - begin;
  
  unsigned long bit24 = 0;
  unsigned long sextet[4] = {0, 0, 0, 0};
  unsigned long octet[3] = {0, 0, 0};
  
  out.clear();
  out.reserve((3 * size) / 4);
  
  for(size_t pos = begin; pos < end - 3; pos += 4)
  {
    for(size_t i = 0; i < 4; i++)
    {
      unsigned long c = in[pos + i];
      if(c >= 65 && c <= 90) sextet[i] = c - 65;
      else if(c >= 97 && c <= 122) sextet[i] = c - 71;
      else if(c >= 48 && c <= 57) sextet[i] = c + 4;
      else if(c == '+') sextet[i] = 62;
      else if(c == '/') sextet[i] = 63;
      else if(c == '=') sextet[i] = 0; //value doesn't matter
      else //unknown char, can be whitespace, newline, ...
      {
        pos++;
        if(pos > end - 3) return;
        i--;
      }
    }
    
    bit24  = 64 * 64 * 64 * sextet[0];
    bit24 +=      64 * 64 * sextet[1];
    bit24 +=           64 * sextet[2];
    bit24 +=                sextet[3];
    
    octet[0] = (bit24 / (256 * 256)) % 256;
    octet[1] = (bit24 / (      256)) % 256;
    octet[2] = (bit24 / (        1)) % 256;
    
    for(size_t i = 0; i < 3; i++)
    {
      if(in[pos + 1 + i] != '=') out.push_back(octet[i]);
    }
  }
}

//conversions between C++ and strings for xml

void convert(std::string& out, bool in) //bool -> boolean
{
  out += (in ? "true" : "false");
}

void convert(std::string& out, char in) //char -> byte
{
  std::stringstream ss;
  ss << int(in);
  out += ss.str();
}

void convert(std::string& out, int in) //int -> int
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, short in) //short -> short
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, long in) //long -> long
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, unsigned char in) //unsigned char -> unsignedByte
{
  std::stringstream ss;
  ss << int(in);
  out += ss.str();
}

void convert(std::string& out, unsigned short in) //unsigned short -> unsignedShort
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, unsigned int in) //unsigned int -> unsignedInt
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, unsigned long in) //unsigned long -> unsignedLong
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, float in) //float -> float
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, double in) //double -> double
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void convert(std::string& out, const void* in) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  //it becomes saved as 64-bit hex string, including "0x" and always 16+2 symbols, e.g. "0x0123456789ABCDEF"
  size_t address = (size_t)in;
  out += "0x";
  size_t pos = out.size();
  out.resize(out.size() + 16);
  for(int i = 0; i < 16; i++)
  {
    unsigned char symbol = address % 16;
    if(symbol < 10) symbol += '0';
    else symbol += ('A' - 10);
    out[pos + 15 - i] = symbol;
    address >>= 4;
  }
}

void convert(std::string& out, void* in) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  convert(out, (const void*)in);
}

void convert(std::string& out, const std::string& in) //std::string -> string
{
  entitify(out, in, 0, in.size()); //we generate a string in XML, so turn &, <, >, ' and " into entities
}

void convert(std::string& out, const std::vector<unsigned char>& in) //std::vector<unsigned char> -> base64Binary
{
  encodeBase64(in, out);
}

////////////////

void unconvert(bool& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  
  out = (s == "true");
}

void unconvert(char& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  int i;
  ss >> i;
  out = i;
}

void unconvert(short& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(int& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(long& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(unsigned char& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  unsigned int i;
  ss >> i;
  out = i;
}

void unconvert(unsigned short& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(unsigned int& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(unsigned long& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(float& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(double& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void unconvert(const void*& out, const std::string& in, size_t pos, size_t end) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  //it was saved as 64-bit hex string, including "0x" and always 16+2 symbols, e.g. "0x0123456789ABCDEF"
  std::string s(in, pos, end - pos);
  size_t address = 0;
  if(s.size() > 2 && s[0] == '0' && s[1] == 'x') //hex
  {
    size_t size = s.size() - 2;
    for(size_t i = 0; i < size; i++)
    {
      address <<= 4;
      unsigned char symbol = s[2 + i];
      if(symbol >= '0' && symbol <= '9') symbol -= '0';
      else symbol -= ('A' - 10);
      address |= symbol;
    }
  }
  else //decimal
  {
    std::stringstream ss(s);
    ss >> address;
  }
  out = (void*)address;
}

void unconvert(void*& out, const std::string& in, size_t pos, size_t end) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  //it was saved as 64-bit hex string, including "0x" and always 16+2 symbols, e.g. "0x0123456789ABCDEF"
  unconvert((const void*&)out, in, pos, end);
}

void unconvert(std::string& out, const std::string& in, size_t pos, size_t end)
{
  out.clear();
  unentitify(out, in, pos, end); //we parse a string from XML, so turn entities into &, <, >, ' or "
}

void unconvert(std::vector<unsigned char>& out, const std::string& in, size_t pos, size_t end)
{
  out.clear();
  decodeBase64(out, in, pos, end); //we parse a string from XML, so turn entities into &, <, >, ' or "
}

void parseTag(const std::string& in, size_t& pos, size_t end, std::string& name, TagPos& tagpos)
{
  /*
  The following are NOT well-formed XML (no spaces in the names of start, end and singleton tags allowed):
  <a/ >
  < /a>
  </ a>
  < a/>
  < a>
  But the following are well formad (location where attributes may come can have space)
  <a >
  <a />
  */

  name.clear();
  
  tagpos.ab = tagpos.ae = 0;
  
  skipWhiteSpace(in, pos, end);
  tagpos.tb = pos;
  if(pos >= end) throw Error(64, pos);
  if(in[pos] != '<') //not a tag, but text mixed in a mixed element
  {
    tagpos.type = TT_MIXEDTEXT;
    
    //everything until next '<' or end is considered the content
    skipToChar(in, pos, end, '<');
    tagpos.te = pos;
    
    name = in.substr(tagpos.tb, tagpos.te - tagpos.tb);
  }
  else
  {
    pos++;
    if(pos >= end) throw Error(61, pos);
    if(in[pos] == '/') //end tag
    {
      tagpos.type = TT_END;
      pos++;
      if(pos >= end) throw Error(61, pos);
      //parse the name of the tag
      while(pos < end && !isWhiteSpace(in[pos]) && in[pos] != '>')
      {
        name += in[pos];
        pos++;
      }
      
      skipWhiteSpace(in, pos, end);
    
      tagpos.ab = pos;
      while(pos < end && in[pos] != '>') pos++; //while there are attributes
      tagpos.ae = pos;
      tagpos.te = ++pos; //go after last >
    }
    
    else if(in[pos] != '?' && in[pos] != '!') //start tag or singleton tag
    {
      //parse the name of the tag
      while(pos < end && !isWhiteSpace(in[pos]) && in[pos] != '>' && in[pos] != '/')
      {
        name += in[pos];
        pos++;
      }
      
      skipWhiteSpace(in, pos, end);
    
      tagpos.ab = pos;
      char quote = 0; //to skip attribute values containing / or > in them
      while(pos < end && (quote != 0 || (in[pos] != '>' && in[pos] != '/')))
      {
        if(quote != 0 && in[pos] == quote) quote = 0;
        else if(in[pos] == '\'' || in[pos] == '"') quote = in[pos];
        pos++; //while there are attributes
      }
      tagpos.ae = pos;
      
      if(in[pos] == '/') //singleton tag
      {
        tagpos.type = TT_SINGLETON;
        pos++;
        if(!isCharacter(in, pos, end, '>')) throw Error(62, pos);
      }
      else tagpos.type = TT_START;
      
      tagpos.te = ++pos; //go after last >
    }
    
    else if(in[pos] == '?') //processing instruction
    {
      tagpos.type = TT_PI;
      skipToChar(in, pos, end, '>');
      if(in[pos - 1] != '?') throw Error(63, pos);
      tagpos.te = ++pos; //go after last >
      name = in.substr(tagpos.tb + 1, tagpos.te - tagpos.tb - 2);
    }
    
    else if(in[pos] == '!' && pos + 1 < end && in[pos + 1] == '-') //comment
    {
      tagpos.type = TT_COMMENT;
      int num_minusses = 0; //how many "--" symbols (-- counts as 1, not 2)
      for(;;)
      {
        pos++;
        if(pos >= end) throw Error(64, pos); //document ends in the middle of a comment...
        if(in[pos] == '>' && num_minusses % 2 == 0) break; //only if a multiple of 4 minusses occured, a '>' is the end tag symbol
        if(in[pos] == '-' && pos < end && in[pos + 1] == '-') num_minusses++;
      }
      tagpos.te = ++pos; //go after last >
      name = in.substr(tagpos.tb + 1, tagpos.te - tagpos.tb - 2);
    }
    
    else if(in[pos] == '!' && in[pos + 1] == '[') //section
    {
      tagpos.type = TT_SECTION;
      //go to first "]]>" encountered
      for(;;)
      {
        pos++;
        if(pos + 2 >= end) throw Error(65, pos);
        if(in[pos] == ']' && in[pos + 1] == ']' && in[pos + 2] == '>') break;
      }
      tagpos.te = ++pos; //go after last >
      name = in.substr(tagpos.tb + 1, tagpos.te - tagpos.tb - 2);
    }
    else if(in[pos] == '!') //declaration
    {
      tagpos.type = TT_DECLARATION;
      skipToChar(in, pos, end, '>');
      tagpos.te = ++pos; //go after last >
      name = in.substr(tagpos.tb + 1, tagpos.te - tagpos.tb - 2);
    }
    else throw Error(66, pos); //not a valid tag
  }
}

int parseElement(const std::string& in, size_t& pos, size_t end, std::string& name, ElementPos& elementpos, bool skipcomments) //the string is empty if it was a singleton tag or an empty tag (you can't see the difference)
{
  skipWhiteSpace(in, pos, end);
  
  TagPos tagpos;
  
  for(;;) //until a tag we want to use is found
  {
    if(pos >= end) return END;
    parseTag(in, pos, end, name, tagpos);
    skipWhiteSpace(in, pos, end);
    if(!skipcomments || !isTreatedAsComment(tagpos.type)) break;
  }
  
  elementpos.eb = tagpos.tb;
  //element.ee is still to be determined later, depending on the tag type
  elementpos.ab = tagpos.ab;
  elementpos.ae = tagpos.ae;
  elementpos.cb = elementpos.ce = pos;
  
  switch(tagpos.type)
  {
    case TT_START: //only in this case there is content
      {
        //go to matching end tag and while at it determine the content type (simple, mixed, nested, empty)
        TagPos endtagpos;
        int tagcount = 1; //this is for finding _matching_ endtag. It's 1 already because our current tag itself is also counted
        std::string end_tag_name;
        size_t numchildtags = 0; //this makes it nested or mixed
        size_t numchildtexts = 0; //this makes it simple or mixed

        for(;;)
        {
          if(pos >= end) throw Error(68, pos); //shouldn't reach end in the middle of tags
          parseTag(in, pos, end, end_tag_name, endtagpos);
          
          if(endtagpos.type == TT_END)
          {
            tagcount--;
            if(tagcount < 1) //matching end tag reached
            {
              if(end_tag_name != name) throw Error(14, pos); //name must match that of start tag
              elementpos.ee = pos; //element end
              elementpos.ce = endtagpos.tb; //content end
              break; //end the for(;;) loop
            }
            else numchildtags++;
          }
          else if(endtagpos.type == TT_START)
          {
            tagcount++;
            numchildtags++;
          }
          else if(endtagpos.type == TT_MIXEDTEXT) numchildtexts++;
          else numchildtags++; //comments and singleton tags count as child tags
        }
        
        //4 content types
        if(numchildtexts == 0 && numchildtags == 0) elementpos.type = ET_EMPTY;
        else if(numchildtexts == 1 && numchildtags == 0) elementpos.type = ET_SIMPLE;
        else if(numchildtexts == 0 && numchildtags > 0) elementpos.type = ET_NESTED;
        else elementpos.type = ET_MIXED;
      }
      break;
    case TT_SINGLETON:
      elementpos.type = ET_EMPTY;
      elementpos.ee = tagpos.te;
      break;
    case TT_END:
      throw Error(67, pos); //error, it starts with an end tag, that can't be
      break;
    case TT_MIXEDTEXT:
      elementpos.type = ET_MIXEDTEXT;
      elementpos.ee = tagpos.te;
      break;
    case TT_COMMENT:
      elementpos.type = ET_COMMENT;
      elementpos.ee = tagpos.te;
      break;
    case TT_PI:
      elementpos.type = ET_PI;
      elementpos.ee = tagpos.te;
      break;
    case TT_DECLARATION:
      elementpos.type = ET_DECLARATION;
      elementpos.ee = tagpos.te;
      break;
    case TT_SECTION:
      elementpos.type = ET_SECTION;
      elementpos.ee = tagpos.te;
      break;
  }
  
  return SUCCESS;
}

//TODO: support single quotes (') too instead of only double quotes (")
int parseAttribute(const std::string& in, size_t& pos, size_t end, std::string& name, ElementPos& elementpos)
{
  skipWhiteSpace(in, pos, end);
  
  if(pos >= end || in[pos] == '>' || in[pos] == '/') return END;
  
  name.clear();
  elementpos.cb = elementpos.ce = pos;
  
  //parse name
  while(!isWhiteSpace(in[pos]) && isNotCharacter(in, pos, end, '='))
  {
    name.push_back(in[pos]);
    pos++;
  }
  
  skipWhiteSpace(in, pos, end);
  
  if(in[pos] != '=') throw Error(17, pos);
  
  pos++;
  skipWhiteSpace(in, pos, end);
  
  if(isNotCharacter(in, pos, end, '\'') && isNotCharacter(in, pos, end, '"')) throw Error(18, pos); //the value must be in quotes
  
  pos++;
  
  //parse the value
  elementpos.cb = pos; //value begin
  while(isNotCharacter(in, pos, end, '\'') && isNotCharacter(in, pos, end, '"')) pos++;
  elementpos.ce = pos; //value end
  
  pos++; //go behind the "
  
  return SUCCESS;
}

int getLineNumber(const std::string& fulltext, size_t pos)
{
  /*
  getLineNumber can be used for showing error messages: if something returns "ERROR", use this with current pos
  works with ascii 10 as newline symbol
  */
  int line = 1;
  for(size_t i = 0; i < pos; i++) if(fulltext[i] == 10) line++;
  return line;
}

int getColumnNumber(const std::string& fulltext, size_t pos)
{
  /*
  getColumnNumber can be used for showing error messages: if something returns "ERROR", use this with current pos
  works with ascii 10 as newline symbol
  */
  int column = 0;
  
  for(size_t i = pos - 1; i < pos; i++) //if i is 0, it'll overflow to a large number, so it'll be >= pos, ending the loop
  {
    column++;
    if(fulltext[i] == 10) break;
  }
  
  return column;
}

////////////////////////////////////////////////////////////////////////////////
// 2.) Reference Resolver Tool
////////////////////////////////////////////////////////////////////////////////

void RefRes::resolve() //store the new values in all the clients
{
  for(size_t i = 0; i < clients.size(); i++)
  {
    const void* newval = pointers_reading[clients[i].id];
    (*(clients[i].ref)) = newval;
  }
}

////////////////////////////////////////////////////////////////////////////////
// 3.) XML parsing and generating with composition
////////////////////////////////////////////////////////////////////////////////

//all in template functions in the header

////////////////////////////////////////////////////////////////////////////////
// 4.) XML parsing and generating with tree
////////////////////////////////////////////////////////////////////////////////

XMLTree::XMLTree() : parent(0), outer(false), type(ET_EMPTY)
{
}

XMLTree::~XMLTree()
{
  for(size_t i = 0; i < children.size(); i++)
  {
    delete children[i];
  }
}

bool XMLTree::isOuter() const
{
  return outer;
}

size_t XMLTree::getLevel() const //depth in the tree
{
  if(parent)
  {
    if(parent->isOuter()) return parent->getLevel();
    else return parent->getLevel() + 1;
  }
  else return 0;
}

int XMLTree::parse(const std::string& in, bool skipcomments)
{
  size_t pos = 0;
  return parse(in, pos, in.size(), skipcomments);
}

int XMLTree::parse(const std::string& in, size_t& pos, bool skipcomments)
{
  return parse(in, pos, in.size(), skipcomments);
}

void XMLTree::parseContent(const std::string& in, ElementPos& elementpos, bool skipcomments)
{
  bool nest = elementpos.type != ET_SIMPLE && elementpos.type != ET_EMPTY;
  std::string elementname;
  
  if(!isTreatedAsComment(type))
  {
    if(nest)
    {
      size_t subpos = elementpos.cb;
      ElementPos subparsepos;
      while(parseElement(in, subpos, elementpos.ce, elementname, subparsepos, skipcomments) == SUCCESS)
      {
        children.push_back(new XMLTree);
        children.back()->parent = this;
        children.back()->parse(in, subparsepos.eb, subparsepos.ee, skipcomments);
      }
    }
    else //it's a value
    {
      content.value = in.substr(elementpos.cb, elementpos.ce - elementpos.cb);
    }
  }
}

int XMLTree::parse(const std::string& in, size_t& pos, size_t end, bool skipcomments)
{
  try
  {
    ElementPos elementpos;
    std::string elementname;
    parseElement(in, pos, end, elementname, elementpos, skipcomments);
    type = elementpos.type;
    content.name = elementname;
    
    if(elementpos.ae > elementpos.ab) //if there are attributes
    {
      //parse attributes
      ElementPos attrpos;
      std::string attrname;
      size_t posa = elementpos.ab;
      while(parseAttribute(in, posa, elementpos.ae, attrname,  attrpos) == SUCCESS)
      {
        attributes.resize(attributes.size() + 1);
        attributes.back().name = attrname;
        attributes.back().value = in.substr(attrpos.cb, attrpos.ce - attrpos.cb);
      }
    }
    
    parseContent(in, elementpos, skipcomments);
  }
  catch(Error error)
  {
    pos = error.pos;
    return error.error;
  }
  
  return SUCCESS;
}

int XMLTree::parseOuter(const std::string& in, bool skipcomments)
{
  size_t pos = 0;
  return parseOuter(in, pos, in.size(), skipcomments);
}

int XMLTree::parseOuter(const std::string& in, size_t& pos, bool skipcomments)
{
  return parseOuter(in, pos, in.size(), skipcomments);
}

int XMLTree::parseOuter(const std::string& in, size_t& pos, size_t end, bool skipcomments)
{
  outer = true;
  
  ElementPos elementpos;
  elementpos.cb = pos;
  elementpos.ce = end;
  
  try
  {
    parseContent(in, elementpos, skipcomments);
  }
  catch(Error error)
  {
    pos = error.pos;
    return error.error;
  }
  
  return SUCCESS;
}

bool XMLTree::isValueTag() const //returns true if this has no nested tags in it (no children)
{
  return children.empty();
}

XMLTree* XMLTree::getFirstNonCommentNode() const
{
  for(size_t i = 0; i < children.size(); i++)
  {
    if(!isTreatedAsComment(children[i]->getType())) return children[i];
  }
  return 0;
}

void XMLTree::generate(std::string& out) const //appends to the string
{
  if(isOuter())
  {
    generateChildren(out);
  }
  else
  {
    size_t level = getLevel();
    
    for(size_t i = 0; i < level; i++) out += xml_indentation_symbol;
    
    if(type == ET_MIXEDTEXT)
    {
      out += content.name;
    }
    else
    {
      out += "<" + content.name;
      for(size_t i = 0; i < attributes.size(); i++)
      {
        generateAttribute(out, attributes[i].name, attributes[i].value);
      }
      
      if(isTreatedAsComment(type))
      {
        out += ">";
      }
      else if(type == ET_EMPTY)
      {
        out += "/>";
      }
      else if(isValueTag())
      {
        out += ">";
        out += content.value;
        out += "</" + content.name + ">";
      }
      else //nested tag
      {
        out += ">";
        out += xml_newline_symbol;
        generateChildren(out);
        for(size_t i = 0; i < level; i++) out += xml_indentation_symbol;
        out += "</" + content.name + ">";
      }
    }
    
    
    out += xml_newline_symbol;
  }
}

void XMLTree::generateChildren(std::string& out) const
{
  for(size_t i = 0; i < children.size(); i++)
  {
    children[i]->generate(out);
  }
}

XMLTree* XMLTree::createNewChild()
{
  XMLTree* result = new XMLTree;
  children.push_back(result);
  result->parent = this;
  return result;
}

} //namespace xml
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///Tests and examples of lpi_xml
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if 0 //LPI_XML_TEST

int main(){}

#include <iostream>
using namespace lpi;
namespace xmltest
{
  ////////////////////////////////////////////////////////////////////////////////
  ///Test and example of parsing/generating with the composition method
  ////////////////////////////////////////////////////////////////////////////////
  
  class MonsterType
  {
    public:
    double constitution;
    double strength;
    std::string name; //for the example, name will be saved as attributes instead of value tags
    
    void parseXML(const std::string& in, xml::ElementPos& pos, xml::RefRes& ref)
    {
      xml::ElementPos elementpos;
      std::string elementname;
      
      //content
      while(parseElement(in, pos.cb, pos.ce, elementname, elementpos) == xml::SUCCESS)
      {
        if(elementname == "constitution") xml::unconvert(constitution, in, elementpos.cb, elementpos.ce);
        else if(elementname == "strength") xml::unconvert(strength, in, elementpos.cb, elementpos.ce);
        else if(elementname == "id")
        {
          int key = 0;
          xml::unconvert(key, in, elementpos.cb, elementpos.ce);
          ref.addPair(key, this);
        }
      }
      
      //attributes
      while(parseAttribute(in, pos.ab, pos.ae, elementname, elementpos) == xml::SUCCESS)
      {
        if(elementname == "name") xml::unconvert(name, in, elementpos.cb, elementpos.ce);
      }
      
      
    }
    
    void generateAttributes(std::string& out, xml::RefRes&) const
    {
      xml::generateAttribute(out, "name", name);
    }
    
    void generateContent(std::string& out, size_t indent, xml::RefRes& ref) const
    {
      xml::generateSimpleElement(out, "id", ref.getKey(this), indent);
      xml::generateSimpleElement(out, "constitution", constitution, indent);
      xml::generateSimpleElement(out, "strength", strength, indent);
    }
  };
  
  class Vector2
  {
    public:
    double x;
    double y;
    
    void parseXML(const std::string& in, xml::ElementPos& pos, xml::RefRes&)
    {
      xml::ElementPos elementpos;
      std::string elementname;
      
      while(parseElement(in, pos.cb, pos.ce, elementname, elementpos) == xml::SUCCESS)
      {
        if(elementname == "x") xml::unconvert(x, in, elementpos.cb, elementpos.ce);
        else if(elementname == "y") xml::unconvert(y, in, elementpos.cb, elementpos.ce);
      }
    }
    
    void generateContent(std::string& out, size_t indent, xml::RefRes&) const
    {
      xml::generateSimpleElement(out, "x", x, indent);
      xml::generateSimpleElement(out, "y", y, indent);
    }
    
    void generateAttributes(std::string&, xml::RefRes&) const {} //needed because no default function can be given to template
  };
  
  class Monster
  {
    public:
    
    double health;
    MonsterType* type;
    Vector2 position;
    Vector2 velocity;
    
    void parseXML(const std::string& in, xml::ElementPos& pos, xml::RefRes& ref)
    {
      xml::ElementPos elementpos;
      std::string elementname;
      
      while(parseElement(in, pos.cb, pos.ce, elementname, elementpos) == xml::SUCCESS)
      {
        if(elementname == "health") xml::unconvert(health, in, elementpos.cb, elementpos.ce);
        else if(elementname == "position") position.parseXML(in, elementpos, ref);
        else if(elementname == "type")
        {
          int key;
          xml::unconvert(key, in, elementpos.cb, elementpos.ce);
          ref.addClient(key, type);
        }
      }
    }
    
    void generateContent(std::string& out, size_t indent, xml::RefRes& ref) const
    {
      xml::generateSimpleElement(out, "type", ref.getKey(type), indent);
      xml::generateSimpleElement(out, "health", health, indent);
      xml::generateRefRes(out, "position", position, indent, ref);
    }
    
    void generateAttributes(std::string&, xml::RefRes&) const {} //needed because no default function can be given to template
  };
  
  class World
  {
    public:
    
    std::vector<MonsterType*> monstertypes;
    std::vector<Monster*> monsters;
    int sizex;
    int sizey;
    
    void parseXML(const std::string& in, xml::ElementPos& pos, xml::RefRes& ref)
    {
      xml::deletify(monstertypes);
      xml::deletify(monsters);
      
      xml::ElementPos elementpos;
      std::string elementname;
      
      while(parseElement(in, pos.cb, pos.ce, elementname, elementpos) == xml::SUCCESS)
      {
        if(elementname == "monster")
        {
          monsters.push_back(new Monster);
          monsters.back()->parseXML(in, elementpos, ref);
        }
        else if(elementname == "monstertype")
        {
          monstertypes.push_back(new MonsterType);
          monstertypes.back()->parseXML(in, elementpos, ref);
        }
        else if(elementname == "sizex") xml::unconvert(sizex, in, elementpos.cb, elementpos.ce);
        else if(elementname == "sizey") xml::unconvert(sizey, in, elementpos.cb, elementpos.ce);
      }
    }
    
    void generateContent(std::string& out, size_t indent, xml::RefRes& ref) const
    {
      xml::generateSimpleElement(out, "sizex", sizex, indent);
      xml::generateSimpleElement(out, "sizey", sizey, indent);
      
      for(size_t i = 0; i < monstertypes.size(); i++)
      {
        xml::generateRefRes(out, "monstertype", *monstertypes[i], indent, ref); //or alternatively you can use "monstertypes[i]->generate(out, "monstertype", indent);"
      }
      
      for(size_t i = 0; i < monsters.size(); i++)
      {
        xml::generateRefRes(out, "monster", *monsters[i], indent, ref); //or alternatively you can use "monsters[i]->generate(out, "monster", indent);"
      }
    }
    
    void generateAttributes(std::string&, xml::RefRes&) const {} //needed because no default function can be given to template
  };
  
  void xmltest()
  {
    std::string xml_in = "<!DOCTYPE none>\n\
<?xml version=\"1.0\"?>\n\
<world>\n\
  <sizex>16</sizex>\n\
  <sizey>16</sizey>\n\
  <!--comment-->\n\
  <monstertype  name='rat'>\n\
    <!--comment with extra '>' symbol in it-->\n\
    <id>0</id>\n\
    <constitution>10</constitution>\n\
    <strength>2</strength>\n\
  </monstertype>\n\
  <monstertype  name='dragon'>\n\
    <id>1</id>\n\
    <constitution>2000</constitution>\n\
    <strength>85</strength>\n\
    <!>\n\
  </monstertype>\n\
  <![WEIRD SECTION[ignored by lpi_xml]]>\n\
  <monster>\n\
    <type>1</type>\n\
    <health>8</health>\n\
    <position>\n\
      <x>15.1</x>\n\
      <y>8.3</y>\n\
    </position>\n\
  </monster>\n\
  <monster>\n\
    <type>0</type>\n\
    <health>8</health>\n\
    <position>\n\
      <x>2.0</x>\n\
      <y>0.0</y>\n\
    </position>\n\
  </monster>\n\
  <monster>\n\
    <type>0</type>\n\
    <health>8</health>\n\
    <position>\n\
      <x>10.1</x>\n\
      <y>9.6</y>\n\
    </position>\n\
  </monster>\n\
</world>";
    
    World world;
    
    size_t pos = 0; //allows displaying line number of error message, if any
    int result = xml::parseRefRes(world, xml_in, pos, xml_in.size());
    if(xml::isError(result))
      std::cout << "parsing error " << result << " on line " << xml::getLineNumber(xml_in, pos) << std::endl;
    
    std::string xml_out;
    xml::generateRefRes(xml_out, "world", world);
    std::cout << xml_out; //after generating the xml again, output it again, it should be the same as the input (except the whitespace, which is indented in a fixed way)
    
    std::cout << std::endl;
    std::cout << "RefRes anti-segfault test (should say \"dragon, rat\"):" << std::endl;
    std::cout << world.monsters[0]->type->name << ", " << world.monsters[1]->type->name << " " << std::endl; //if this segfaults, the RefRes system doesn't work correctly, or there aren't enough monsters / no correct types&id's in the xml code
    std::cout << std::endl;
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  ///Test and example of XMLTree
  ////////////////////////////////////////////////////////////////////////////////
  
  void xmltreetest()
  {
    std::string xml = "<!DOCTYPE none><?xml version=\"1.0\"?><a><!--comment1--><!--comment2--><b x='y'>c</b><d><!--comment3--><e/></d><f/><g><h>i</h><j><k>l<m/>n</k></j></g><!--comment4--></a><!--comment5-->";
    
    xml::XMLTree tree;
    int error = tree.parse(xml); //comments are ignored
    if(error) std::cout << "An error happened: " << error << std::endl;
    
    std::string result;
    
    tree.generate(result);
    
    std::cout << result << std::endl;
  }
  
  void xmltreetest_comments()
  {
    std::string xml = "<!DOCTYPE none><?xml version=\"1.0\"?><a><!--comment1--><!--comment2--><b x='y'>c</b><d><!--comment3--><e/></d><f/><g><h>i</h><j><k>l<m/>n</k></j></g><!--comment4--></a><!--comment5-->";
    
    xml::XMLTree tree;
    int error = tree.parseOuter(xml, false); //comments and xml declaration not ignored, but included in pretty printing
    if(error) std::cout << "An error happened: " << error << std::endl;
    
    std::string result;
    
    tree.generate(result);
    
    std::cout << result << std::endl;
  }
  

  
  ////////////////////////////////////////////////////////////////////////////////
  ///Test and example of a custom convert and unconvert function
  ////////////////////////////////////////////////////////////////////////////////
  
  /*
  Here follows an example to use the convert/unconvert functions for custom structs.
  convert and unconvert functions don't have to be places inside the class.
  This allows using the convert/unconvert notation for simple members such as
  colors and vectors, where you can use a notation such as "255 255 128 0" for
  an RGBA color instead of a long subtag. The example shows this for a Vector3.
  All examples use stringstreams so different primitive values are separated
  by spaces to make it work.
  */
  
  struct Vector3
  {
    double x;
    double y;
    double z;
  };
  
  void unconvert(Vector3& out, const std::string& in, size_t pos, size_t end)
  {
    std::string s(in, pos, end - pos);
    std::stringstream ss(s);
    ss >> out.x;
    ss >> out.y;
    ss >> out.z;
  }
  
  static void convert(std::string& out, const Vector3& in)
  {
    std::stringstream ss;
    ss << in.x;
    ss << " ";
    ss << in.y;
    ss << " ";
    ss << in.z;
    out += ss.str();
  }
  
  void custom_convert_test()
  {
    Vector3 v;
    std::string s = "100 500 300"; //x, y and z component of the vector separated by spaces
    /*
    this could be in tags, e.g.
    <position>100 500 300</position>
    <velocity>0.5 0.1 0.23</velocity></spaceship>
    */
    
    unconvert(v, s, 0, s.size());
    
    std::cout << "Vector3 unconvert test: " << v.x << " " << v.y << " " << v.z << std::endl;
    
    std::string s2;
    
    convert(s2, v);
    
    std::cout << "Vector3 convert test: " << s2 << std::endl;
  }
  
  
  ////////////////////////////////////////////////////////////////////////////////
  ///Run the test...
  ////////////////////////////////////////////////////////////////////////////////
  
  struct TEST
  {
    TEST()
    {
      std::cout << "[composition test]"<<std::endl;
      xmltest();
      std::cout << "[XMLTree pretty printing test without comments]"<<std::endl;
      xmltreetest();
      std::cout << "[XMLTree pretty printing test with comments]"<<std::endl;
      xmltreetest_comments();
      std::cout << "[Custom Convert test]"<<std::endl;
      custom_convert_test();
    }
  } test;

} //end of namespace xmltest

#endif
