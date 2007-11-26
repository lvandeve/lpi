/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_xml.h"

#include <string>
#include <vector>
#include <sstream>

//parse <-> generate

namespace lpi
{

static const std::string xml_indentation_symbol = "  ";
static const std::string xml_newline_symbol = "\n";

////////////////////////////////////////////////////////////////////////////////
///XML Tools
////////////////////////////////////////////////////////////////////////////////

void XML::skipWhiteSpace(const std::string& in, size_t& pos, size_t end)
{
  while(pos < end && isWhiteSpace(in[pos])) pos++;
}

void XML::skipToChar(const std::string& in, size_t& pos, size_t end, char c)
{
  while(pos < end && in[pos] != c) pos++;
}

bool XML::isWhiteSpace(char c)
{
  return (c <= 32);
}

bool XML::isCharacter(const std::string& in, size_t pos, size_t end, char c) //made to have the < size and == test in one function
{
  return(pos < end && in[pos] == c);
}

bool XML::isNotCharacter(const std::string& in, size_t pos, size_t end, char c) //made to have the < size and != test in one function
{
  return(pos < end && in[pos] != c);
}

void XML::entitify(std::string& out, const std::string& in, size_t pos, size_t end)
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

void XML::unentitify(std::string& out, const std::string& in, size_t pos, size_t end)
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
void XML::encodeBase64(const std::vector<unsigned char>& in, std::string& out)
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
      if(pos + i - 1 < in.size()) out.push_back(characters[sextet[i]]);
      else out.push_back('=');
    }
    
    if(pos % 57 == 0 && pos != 0) out.push_back(10); //newline char every 76 chars (57 = 3/4th of 76)
  }
}

void XML::decodeBase64(std::vector<unsigned char>& out, const std::string& in, size_t begin, size_t end)
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

//C++ to XML (convert)
void XML::convert(std::string& out, bool in) //bool -> boolean
{
  out += (in ? "true" : "false");
}

void XML::convert(std::string& out, char in) //char -> byte
{
  std::stringstream ss;
  ss << int(in);
  out += ss.str();
}

void XML::convert(std::string& out, int in) //int -> int
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, short in) //short -> short
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, long in) //long -> long
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, unsigned char in) //unsigned char -> unsignedByte
{
  std::stringstream ss;
  ss << int(in);
  out += ss.str();
}

void XML::convert(std::string& out, unsigned short in) //unsigned short -> unsignedShort
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, unsigned int in) //unsigned int -> unsignedInt
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, unsigned long in) //unsigned long -> unsignedLong
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, float in) //float -> float
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, double in) //double -> double
{
  std::stringstream ss;
  ss << in;
  out += ss.str();
}

void XML::convert(std::string& out, const void* in) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  size_t address = (size_t)in;
  std::stringstream ss;
  ss << address;
  out += ss.str();
}

void XML::convert(std::string& out, void* in) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  size_t address = (size_t)in;
  std::stringstream ss;
  ss << address;
  out += ss.str();
}

void XML::convert(std::string& out, const std::string& in) //std::string -> string
{
  entitify(out, in, 0, in.size()); //we generate a string in XML, so turn &, <, >, ' and " into entities
}

void XML::convert(std::string& out, const std::vector<unsigned char>& in) //std::vector<unsigned char> -> base64Binary
{
  encodeBase64(in, out);
}

//XML to C++ (unconvert)
void XML::unconvert(bool& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  
  out = (s == "true");
}

void XML::unconvert(char& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  int i;
  ss >> i;
  out = i;
}

void XML::unconvert(short& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(int& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(long& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(unsigned char& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  unsigned int i;
  ss >> i;
  out = i;
}

void XML::unconvert(unsigned short& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(unsigned int& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(unsigned long& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(float& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(double& out, const std::string& in, size_t pos, size_t end)
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  ss >> out;
}

void XML::unconvert(const void*& out, const std::string& in, size_t pos, size_t end) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  size_t address;
  ss >> address;
  out = (void*)address;
}

void XML::unconvert(void*& out, const std::string& in, size_t pos, size_t end) //NOTE: currently not safe if written by 64-bit PC and read by 32-bit PC
{
  std::string s(in, pos, end - pos);
  std::stringstream ss(s);
  size_t address;
  ss >> address;
  out = (void*)address;
}

void XML::unconvert(std::string& out, const std::string& in, size_t pos, size_t end)
{
  out.clear();
  unentitify(out, in, pos, end); //we parse a string from XML, so turn entities into &, <, >, ' or "
}

void XML::unconvert(std::vector<unsigned char>& out, const std::string& in, size_t pos, size_t end)
{
  out.clear();
  decodeBase64(out, in, pos, end); //we parse a string from XML, so turn entities into &, <, >, ' or "
}

void XML::skipToNonCommentTag(const std::string& in, size_t& pos, size_t end) //call when you want to go to a '<' that is not of a comment tag. After calling this function, pos is on top of the first non-comment '<'. This function can throw.
{
  bool tag_found = false; //comments and <?...?> tags are not counted as tags
  size_t ltpos = pos; //pos of the last '<' character
  while(!tag_found)
  {
    skipToChar(in, pos, end, '<');
    ltpos = pos;
    pos++;
    skipWhiteSpace(in, pos, end);
    
    //if there is an exclamation mark (!), it's a comment tag. Also attempt to follow the rules about "--" here.
    if(isCharacter(in, pos, end, '!'))
    {
      int num_minusses = 0; //how many "--" symbols (-- counts as 1, not 2)
      for(;;)
      {
        pos++;
        if(pos >= end) throw Error(20, pos); //document ends in the middle of a comment...
        if(in[pos] == '>') if(num_minusses % 2 == 0) { pos++; break; } //only if a multiple of 4 minusses occured, a '>' is the end tag symbol
        if(in[pos] == '-' && pos < end && in[pos + 1] == '-') num_minusses++;
      }
    }
    //if there is a question mark (?), it's an xml start tag at the top. Ignore it, skip to the matching ?>, and try again.
    else if(isCharacter(in, pos, end, '?'))
    {
      pos++;
      skipToChar(in, pos, end, '>');
      pos++;
    }
    else tag_found = true;
  }
  
  pos = ltpos; //this is so that whoever calls skipToNonCommentTag has pos on the expected position: on top of the '<'
}

int XML::parseTag(const std::string& in, size_t& pos, size_t end, std::string& name, ParseTagPos& parsepos, bool skipcomments) //the string is empty if it was a singleton tag or an empty tag (you can't see the difference)
{
  skipWhiteSpace(in, pos, end);
  if(skipcomments) skipToNonCommentTag(in, pos, end); //go to on top of next '<' symbol that is not of a comment tag
  
  if(pos >= end) return END;
  
  name.clear();
  
  parsepos.tb = pos;
  pos++;
  skipWhiteSpace(in, pos, end);
  
  //parse the name of the tag
  while(pos < end && !isWhiteSpace(in[pos]) && in[pos] != '>' && in[pos] != '/')
  {
    name += in[pos];
    pos++;
  }
  
  if(!skipcomments && isCommentTag(name, 0, name.size()))
  {
    pos++; //go after the >
    //these have no meaning
    parsepos.ab = 0;
    parsepos.ae = 0;
    parsepos.cb = 0;
    parsepos.ce = 0;
    parsepos.te = pos;
    return SUCCESS; //comments have no content so don't continue
  }
  
  skipWhiteSpace(in, pos, end);

  parsepos.ab = pos;
  while(pos < end && in[pos] != '>' && in[pos] != '/') pos++; //while there are attributes
  parsepos.ae = pos;
  
  if(pos < end && in[pos] == '/') //it's a singleton tag
  {
    pos++;
    skipWhiteSpace(in, pos, end);
    parsepos.cb = parsepos.ce = pos;
    if(isCharacter(in, pos, end, '>'))
    {
      pos++; //bring pos to after what we just parsed
      parsepos.te = pos;
      return SUCCESS;
    }
    else throw Error(11, pos);
  }
  else if(!(pos < end && in[pos] == '>')) throw Error(12, pos);
  
  //it's not a singleton tag, now parse the content
  //there must be kept parseing until tagcount is 1 and you reach an end tag (making tagcount 0). Then check if the end tag has the same text as the start tag, that must be to be so correct XML.
  int tagcount = 1; //1 already because our current tag itself is also counted
  bool done = false;
  pos++;
  parsepos.cb = pos; //content begin
  
  while(pos < end && !done)
  {
    /*if(skipcomments)*/ skipToNonCommentTag(in, pos, end); //go to after next '<' symbol that is not of a comment tag

    size_t temp_pos = pos;
    pos++;
    skipWhiteSpace(in, pos, end);
    if(isCharacter(in, pos, end, '/')) //it's an end tag
    {
      pos++;
      tagcount--;
      if(tagcount < 1) //matching end tag reached
      {
        std::string end_tag_name;
        skipWhiteSpace(in, pos, end);
        while(pos < end && !isWhiteSpace(in[pos]) && in[pos] != '>' && in[pos] != '/')
        {
          end_tag_name += in[pos];
          pos++;
        }
        
        skipWhiteSpace(in, pos, end);
        
        if(isNotCharacter(in, pos, end, '>')) throw Error(13, pos);
        if(end_tag_name != name) throw Error(14, pos);
        
        pos++; //place the position after the end tag
        parsepos.te = pos; //tag end
        
        //now we know where the content start and ends, so parse the content
        parsepos.ce = temp_pos; //content end
        
        return SUCCESS;
      }
    }
    else //it's a singleton tag or start tag
    {
      while(pos < end && in[pos] != '>' && in[pos] != '/') pos++;
      
      if(isCharacter(in, pos, end, '/')) //it's a singleton tag
      {
        pos++;
        skipWhiteSpace(in, pos, end);
        if(isNotCharacter(in, pos, end, '>')) throw Error(15, pos);
        pos++;
      }
      else if(isCharacter(in, pos, end, '>'))//it's a start tag
      {
        tagcount++;
        pos++;
      }
    }
  }
  
  throw Error(16, pos); //if you got here, something strange must have happened
}

int XML::parseAttribute(const std::string& in, size_t& pos, size_t end, std::string& name, ParseAttributePos& parsepos)
{
  skipWhiteSpace(in, pos, end);
  
  if(pos >= end || in[pos] == '>' || in[pos] == '/') return END;
  
  name.clear();
  parsepos.cb = parsepos.ce = pos;
  
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
  parsepos.cb = pos; //value begin
  while(isNotCharacter(in, pos, end, '\'') && isNotCharacter(in, pos, end, '"')) pos++;
  parsepos.ce = pos; //value end
  
  pos++; //go behind the "
  
  return SUCCESS;
}

int XML::getLineNumber(const std::string& fulltext, size_t pos)
{
  /*
  getLineNumber can be used for showing error messages: if something returns "ERROR", use this with current pos
  works with ascii 10 as newline symbol
  */
  int line = 1;
  for(size_t i = 0; i < pos; i++) if(fulltext[i] == 10) line++;
  return line;
}

int XML::getColumnNumber(const std::string& fulltext, size_t pos)
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

bool XML::isNestedTag(const std::string& in, size_t pos, size_t end)
{
  while(pos < end)
  {
    skipWhiteSpace(in, pos, end);
    if(pos >= end) return false;
    if(in[pos] == '<') return true;
    else return false; //if it doesn't begin with a <, it's no tag
  }
  
  return false;
}

bool XML::isCommentTag(const std::string& in, size_t pos, size_t end)
{
  if(in[pos] == '?' && in[end - 1] == '?') return true; //it's an xml declaration
  if(in.size() >= 5
     && in[pos] == '!' && in[pos + 1] == '-' && in[pos + 2] == '-'
     && in[end - 2] == '-' && in[end - 1] == '-') return true; //it's a comment

  //TODO: check in a more precise way if it's a comment, by counting the number of -- pairs
  
  return false;
}

////////////////////////////////////////////////////////////////////////////////
///XMLCompose (without tree)
////////////////////////////////////////////////////////////////////////////////

void XMLCompose::RefRes::addPair(const void* old, void* newa)
{
  pointers[old] = newa;
}

void XMLCompose::RefRes::addClient(void** client, void* address)
{
  *client = address;
  clients.push_back(client);
}

void XMLCompose::RefRes::resolve() //store the new values in all the clients
{
  for(size_t i = 0; i < clients.size(); i++)
  {
    const void* old = *(clients[i]);
    *(clients[i]) = pointers[old];
  }
}

void* XMLCompose::getAddress()
{
  return static_cast<void*>(this);
}

const void* XMLCompose::getAddress() const
{
  return dynamic_cast<const void*>(this);
}

int XMLCompose::parse(const std::string& in) //To this function you have to give the complete xml string, including the name of this tag, even though this name will be ignored. It'll parse the attributes and content.
{
  size_t pos = 0;
  return parse(in, pos, in.size());
}

int XMLCompose::parse(const std::string& in, size_t& pos)
{
  return parse(in, pos, in.size());
}

int XMLCompose::parse(const std::string& in, size_t& pos, size_t end)
{
  try
  {
    ParseTagPos parsepos;
    std::string name_dummy; //value will be ignored...
    parseTag(in, pos, end, name_dummy, parsepos);
    RefRes ref;
    parse(in, parsepos.ab, parsepos.ae, parsepos.cb, parsepos.ce, ref);
    ref.resolve();
  }
  catch(Error error)
  {
    pos = error.pos;
    return error.error;
  }
  
  return SUCCESS;
}

void XMLCompose::parse(const std::string& in, size_t ab, size_t ae, size_t cb, size_t ce, RefRes& ref)
{
  doParseAttributes(in, ab, ae, ref);
  doParseContent(in, cb, ce, ref);
}

void XMLCompose::generate(std::string& out, const std::string& name) const
{
  generate(out, name, 0);
}

void XMLCompose::generate(std::string& out, const std::string& name, size_t indent) const
{
  for(size_t i = 0; i < indent; i++) out += xml_indentation_symbol;

  indent++;
  
  out += "<";
  out += name;
  doGenerateAttributes(out);
  size_t size_before_adding_tag_close_symbol = out.size(); //you can't know for sure how much characters the newline symbol will be
  out += ">";
  out += xml_newline_symbol;
  size_t size_before_adding_content = out.size();
  doGenerateContent(out, indent);
  size_t size_after_adding_content = out.size();
  
  indent--;
  
  if(size_before_adding_content == size_after_adding_content) //make this a singleton tag
  {
    out.resize(size_before_adding_tag_close_symbol); //remove the newline and >, to put a singleton tag end instead
    out += "/>";
  }
  else
  {
    for(size_t i = 0; i < indent; i++) out += xml_indentation_symbol;

    out += "</";
    out += name;
    out += ">";
  }
  out += xml_newline_symbol;
}

//if the content string is empty, it'll generate a singleton tag. If the attributes string is not empty it'll generate the attributes in the tag WITHOUT a space between name and the attributes. The attributes themselves must be correct with quotes already (create them with generateAttribute to get them correct) and with a space in front of each attribute name. The generate function will NOT convert anything to entities, you must manually call functions on a string to convert that part to entities. This because the content can contain other tags, which have all these symbols and may not be broken.
void XMLCompose::generateTagString(std::string& out, const std::string& name, const std::string& content, size_t indent)
{
  for(size_t i = 0; i < indent; i++) out += xml_indentation_symbol;
  out += "<";
  out += name;
  //out += attributes;
  if(content.size() == 0) //empty content, make singleton tag
  {
    out += "/>";
  }
  else
  {
    out += ">";
    out += content;
    out += "</";
    out += name;
    out += ">";
  }
  out += xml_newline_symbol;
}

void XMLCompose::generateAttributeString(std::string& out, const std::string& name, const std::string& value)
{
  std::string value_string;
  convert(value_string, value);
    
  out += " ";
  out += name;
  out += "=";
  out += '"';
  out += value_string;
  out += '"';
}

void XMLCompose::generateTag(std::string& out, const std::string& name, const XMLCompose& value, size_t indent)
{
  value.generate(out, name, indent);
}

void XMLCompose::generateTag(std::string& out, const std::string& name, const XMLCompose* value, size_t indent)
{
  value->generate(out, name, indent);
}

////////////////////////////////////////////////////////////////////////////////
///XMLTree (stores result in tree)
////////////////////////////////////////////////////////////////////////////////

XMLTree::XMLTree() : parent(0), outer(false), comment(false)
{
}

XMLTree::~XMLTree()
{
  for(size_t i = 0; i < children.size(); i++)
  {
    delete children[i];
  }
}

bool XMLTree::isComment() const
{
  return comment;
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

void XMLTree::parseContent(const std::string& in, ParseTagPos& parsepos, bool skipcomments)
{
  bool nest = isNestedTag(in, parsepos.cb, parsepos.ce);
  std::string tagname;
  
  if(!isComment())
  {
    if(nest)
    {
      size_t subpos = parsepos.cb;
      ParseTagPos subparsepos;
      while(parseTag(in, subpos, parsepos.ce, tagname, subparsepos, skipcomments) == SUCCESS)
      {
        children.push_back(new XMLTree);
        children.back()->parent = this;
        children.back()->parse(in, subparsepos.tb, subparsepos.te, skipcomments);
      }
    }
    else //it's a value
    {
      content.value = in.substr(parsepos.cb, parsepos.ce - parsepos.cb);
    }
  }
}

int XMLTree::parse(const std::string& in, size_t& pos, size_t end, bool skipcomments)
{
  try
  {
    ParseTagPos parsepos;
    std::string tagname;
    parseTag(in, pos, end, tagname, parsepos, skipcomments);
    
    content.name = tagname;
    
    if(!skipcomments && isCommentTag(tagname, 0, tagname.size()))
    {
      comment = true;
    }
    
    if(parsepos.ae > parsepos.ab) //if there are attributes
    {
      //parse attributes
      ParseAttributePos attrpos;
      std::string attrname;
      size_t posa = parsepos.ab;
      while(parseAttribute(in, posa, parsepos.ae, attrname,  attrpos) == SUCCESS)
      {
        attributes.resize(attributes.size() + 1);
        attributes.back().name = attrname;
        attributes.back().value = in.substr(attrpos.cb, attrpos.ce - attrpos.cb);
      }
    }
    
    parseContent(in, parsepos, skipcomments);
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
  
  ParseTagPos parsepos;
  parsepos.cb = pos;
  parsepos.ce = end;
  
  try
  {
    parseContent(in, parsepos, skipcomments);
  }
  catch(Error error)
  {
    pos = error.pos;
    return error.error;
  }
  
  return SUCCESS;
}

bool XMLTree::isEmptyValueTag() const //returns true if it has no children and the value is empty
{
  return children.empty() && content.value.empty();
}

bool XMLTree::isValueTag() const //returns true if this has no nested tags in it (no children)
{
  return children.empty();
}

XMLTree* XMLTree::getFirstNonCommentNode() const
{
  for(size_t i = 0; i < children.size(); i++)
  {
    if(!children[i]->isComment()) return children[i];
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
    out += "<" + content.name;
    for(size_t i = 0; i < attributes.size(); i++)
    {
      out += " " + attributes[i].name + "=\"" + attributes[i].value + "\"";
    }
    
    if(isComment())
    {
      out += ">";
    }
    else if(isEmptyValueTag())
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

} //end of namespace lpi

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

//int main(){}

#include <iostream>
using namespace lpi;
namespace xmltest
{
  ////////////////////////////////////////////////////////////////////////////////
  ///Test and example of XMLCompose
  ////////////////////////////////////////////////////////////////////////////////
  
  class MonsterType : public XMLCompose
  {
    public:
    double constitution;
    double strength;
    std::string name; //for the example, name will be saved as attributes instead of value tags
    
    virtual void doParseContent(const std::string& in, size_t& pos, size_t end, RefRes& ref)
    {
      ParseTagPos parsepos;
      std::string tagname;
      const void* old_address = 0;
      
      while(parseTag(in, pos, end, tagname, parsepos) == SUCCESS)
      {
        if(tagname == "constitution") unconvert(constitution, in, parsepos.cb, parsepos.ce);
        else if(tagname == "strength") unconvert(strength, in, parsepos.cb, parsepos.ce);
        else if(tagname == "id") unconvert(old_address, in, parsepos.cb, parsepos.ce);
      }
      ref.lastold = old_address;
    }
    
    virtual void doParseAttributes(const std::string& in, size_t& pos, size_t end, RefRes& /*ref*/)
    {
      ParseAttributePos parsepos;
      std::string tagname;
      
      while(parseAttribute(in, pos, end, tagname, parsepos) == SUCCESS)
      {
        if(tagname == "name") unconvert(name, in, parsepos.cb, parsepos.ce);
      }
    }
    
    virtual void doGenerateAttributes(std::string& out) const
    {
      generateAttribute(out, "name", name);
    }
    
    virtual void doGenerateContent(std::string& out, size_t indent) const
    {
      generateValueTag(out, "id", getAddress(), indent);
      generateValueTag(out, "constitution", constitution, indent);
      generateValueTag(out, "strength", strength, indent);
    }
  };
  
  class Vector2 : public XMLCompose
  {
    public:
    double x;
    double y;
    
    virtual void doParseContent(const std::string& in, size_t& pos, size_t end, RefRes& /*ref*/)
    {
      ParseTagPos parsepos;
      std::string tagname;
      
      while(parseTag(in, pos, end, tagname, parsepos) == SUCCESS)
      {
        if(tagname == "x") unconvert(x, in, parsepos.cb, parsepos.ce);
        else if(tagname == "y") unconvert(y, in, parsepos.cb, parsepos.ce);
      }
    }
    
    virtual void doGenerateContent(std::string& out, size_t indent) const
    {
      generateValueTag(out, "x", x, indent);
      generateValueTag(out, "y", y, indent);
    }
  };
  
  class Monster : public XMLCompose
  {
    public:
    
    double health;
    MonsterType* type;
    Vector2 position;
    Vector2 velocity;
    
    virtual void doParseContent(const std::string& in, size_t& pos, size_t end, RefRes& ref)
    {
      ParseTagPos parsepos;
      std::string tagname;
      
      while(parseTag(in, pos, end, tagname, parsepos) == SUCCESS)
      {
        if(tagname == "health") unconvert(health, in, parsepos.cb, parsepos.ce);
        else if(tagname == "position") position.parse(in, parsepos.ab, parsepos.ae, parsepos.cb, parsepos.ce, ref);
        else if(tagname == "type")
        {
          void* address;
          unconvert(address, in, parsepos.cb, parsepos.ce);
          ref.addClient((void**)(&type), address);
        }
      }
    }
    
    virtual void doGenerateContent(std::string& out, size_t indent) const
    {
      generateValueTag(out, "type", type->getAddress(), indent);
      generateValueTag(out, "health", health, indent);
      generateTag(out, "position", position, indent);
    }
  };
  
  class World : public XMLCompose
  {
    public:
    
    std::vector<MonsterType*> monstertypes;
    std::vector<Monster*> monsters;
    int sizex;
    int sizey;
    
    virtual void doParseContent(const std::string& in, size_t& pos, size_t end, RefRes& ref)
    {
      deletify(monstertypes);
      deletify(monsters);
      
      ParseTagPos parsepos;
      std::string tagname;
      
      while(parseTag(in, pos, end, tagname, parsepos) == SUCCESS)
      {
        if(tagname == "monster")
        {
          monsters.push_back(new Monster);
          monsters.back()->parse(in, parsepos.ab, parsepos.ae, parsepos.cb, parsepos.ce, ref);
        }
        else if(tagname == "monstertype")
        {
          monstertypes.push_back(new MonsterType);
          monstertypes.back()->parse(in, parsepos.ab, parsepos.ae, parsepos.cb, parsepos.ce, ref);
          ref.addPair(ref.lastold, monstertypes.back()->getAddress());
        }
        else if(tagname == "sizex") unconvert(sizex, in, parsepos.cb, parsepos.ce);
        else if(tagname == "sizey") unconvert(sizey, in, parsepos.cb, parsepos.ce);
      }
    }
    
    virtual void doGenerateContent(std::string& out, size_t indent) const
    {
      generateValueTag(out, "sizex", sizex, indent);
      generateValueTag(out, "sizey", sizey, indent);
      
      for(size_t i = 0; i < monstertypes.size(); i++)
      {
        generateTag(out, "monstertype", monstertypes[i], indent); //or alternatively you can use "monstertypes[i]->generate(out, "monstertype", indent);"
      }
      
      for(size_t i = 0; i < monsters.size(); i++)
      {
        generateTag(out, "monster", monsters[i], indent); //or alternatively you can use "monsters[i]->generate(out, "monster", indent);"
      }
    }
  };
  
  void xmltest()
  {
    std::string xml_in = "<world>\n\
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
    int result = world.parse(xml_in, pos, xml_in.size());
    if(XML::isError(result))
      std::cout << "parsing error " << result << " on line " << XML::getLineNumber(xml_in, pos) << std::endl;
    
    std::string xml_out;
    world.generate(xml_out, "world");
    std::cout << xml_out; //after generating the xml again, output it again, it should be the same as the input (except the whitespace, which is indented in a fixed way)
    
    std::cout << std::endl;
    std::cout << "Segfault test: " << world.monsters[0]->type->name << std::endl; //if this segfaults, the RefRes system doesn't work correctly, or there aren't enough monsters / no correct types&id's in the xml code
    std::cout << std::endl;
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  ///Test and example of XMLTree
  ////////////////////////////////////////////////////////////////////////////////
  
  void xmltreetest()
  {
    std::string xml = "<a><b x='y'>c</b><d><e/></d><f/><g><h>i</h><j><k>l</k></j></g></a>";
    
    XMLTree tree;
    int error = tree.parse(xml); //comments are ignored
    if(error) std::cout << "An error happened: " << error << std::endl;
    
    std::string result;
    
    tree.generate(result);
    
    std::cout << result << std::endl;
  }
  
  void xmltreetest_comments()
  {
    std::string xml = "<?declaration?><a><!--comment1--><!--comment2--><b x='y'>c</b><d><!--comment3--><e/></d><f/><g><h>i</h><j><k>l</k></j></g><!--comment4--></a><!--comment5-->";
    
    XMLTree tree;
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
      std::cout << "[XMLCompose test]"<<std::endl;
      xmltest();
      std::cout << "[XMLTree pretty printing test]"<<std::endl;
      xmltreetest();
      std::cout << "[XMLTree pretty printing test with comments]"<<std::endl;
      xmltreetest_comments();
      std::cout << "[Custom Convert test]"<<std::endl;
      custom_convert_test();
    }
  } test;

} //end of namespace xmltest

#endif
