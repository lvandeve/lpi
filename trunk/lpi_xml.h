/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef lpi_xml_h_included
#define lpi_xml_h_included

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <iostream>

/*
Lode's XML system
-----------------

This supports parsing and generating of XML files without mixed content. This is to be used to read/save settings
from/to a file, such as descriptions of levels of a game, mouse/keyboard controls, profiles, ...

Mixed content (values and nested elements inside the same element) is not supported because it adds
extra complexity without adding any more value to the purpose. Each element with a value in it, may contain
only one value so that each element with a value has only 1 meaning (e.g. the strength of a monster, the value
of a keyboard key, ...).

It may not have the safety and efficiency of some other systems, but it's convenient and small, at least if you
follow the example in the .cpp file with the World, MonsterType and Monster tags.

A disadvantage is that when parsing it may go over the same text multiple times, each time again for subtags,
however this doesn't grow really much, if the depth of the tree is D it'll only go at max D times over a
certain text, and the max depth of the tree is only roughly O(log(N)) where N is a very small fraction of the length
of the text.

Supported:
----------

-Parsing of ascii or UTF-8 xml documents (this depends on where this is compiled though)
-Nested elements if they have no mixed content, e.g. <a><b>text</b><c>more text</c></a>
-Singleton tags of the form <a/>
-Attributes in the opening tag or in singleton tags, e.g. <a b="c">d</a> or <a b="c"/>
-Several value types, such as integers, floating point numbers, booleans, strings and base64-encoded octet streams.
-Reading an XML string and writing it out as a nicely formatted and indented version of the same string. However
  it will not write the comments and xml declaration (for now).
-Storing of pointers in xml and recreating a datastructure with pointers (with updated addresses) when parsing.
-Storing binary data in base64 format in XML
-Case sensitive tag names, as specified by the XML standard. So the following is invalid: <a>text</A>
-Comments, these are tags of the form <!--comment-->. Avoid using other forms. But, the confusing rule
  about the "--" is followed, so this is a legal comment: "<!------>comment-->".
  The empty comment is "<!>".
  Comments are allowed only between tags or nested tags. They are NOT allowed to be mixed inside value tags.
  That means, this is ok: <a><!><b>value</b></a>, but the following isn't: <a><!>value</a>
  Comments are ignored by the composition method, but are optionally stored as comments in XMLTree
  to allow pretty printing to include the comments and xml declaration.

Not supported:
--------------

-Other kinds of strings than ascii or UTF-8 (this depends on where this is compiled though)
-It does not support mixed content, that is: combining data and nested tags together in 1 tag.
 The following can be parsed correctly:
 "<a><b>c</b><d><e/></d><f/></a>"
 The following cannot be parsed correctly because it has a value and tags inside the same <a> tag:
 "<a><b>c</b>d</a>"
-Similar to the above, comment tags are also NOT allowed to be mixed inside a value tag, comments are only allowed between nested tags
-Validating with or using an xsd or other type of schema or doc type definition isn't supported - your C++ class structure is the "schema" :)
-The xml declaration of the form <?...?> is optional and ignored. But at least it won't cause an error if you include one: it's treated as a comment.
-If you parse with comments, the xml declaration is seen as comment, but the ? is kept so you can recognise it if needed

Different methods
-----------------

There are actually different ways to parse and generate available:

*) Tools: There's a basic toolset to parse and generate tags, and convert values
      from tags to C++ datatypes. This also defines the primitive datatypes that
      can be parsed and generated to tags.
*) With composition: this is intended to be used by composition. You can translate your class
      structure to XML this way. It can be tricky to implement this in a nice way though.
      This way allows a sort of serialization and allows storing and loading a complete data
      structure that includes pointers to other data members.
*) XMLTree: this allows storing the XML in a tree and generating XML out of it again. This is an easy way
            that works without enforcing certain functions to be made in classes or even making classes at all.
*) Combinations: You can also combine the composition and XMLTree method, e.g. using XMLTree to parse subtags that
                 are encountered.

Errors and exceptions
---------------------

Some parse functions indicate their level of success with an integer:

0 or XML::SUCCESS: result fine, end not reached
1 or XML::END: result fine, end reached of this tag or subtag (see the example for how in a while loop you keep parsing while the return value is SUCCESS and stop if it's END)
>= 10: some error happened, which error it is: see the return values in the code

Other parse functions will throw an lpi::XML::Error object, with in it an integer value
that has the same rules as the returned integer described above.

The reason for the exceptions is because I saw no other sane way to release the implementer
of parse... functions to do a lot of error checking. However, the functions you normally
call from the external world will not throw anything and return an integer instead. The
C++ exceptions are only used internally. You can throw in the parse... functions that
you implement though, if you need it. It's indicated in this header file which parse functions
throw and which ones not.

Whitespace
----------

Whitespace follows the official rules of XML (not those of HTML):

Whitespace inside the content of value tags is preserved. This includes spaces, newlines and tabs.
Whitespace between tags, where there is no content, is ignored.

Glossary:
---------

Here are the meanings of some of the names used in the comments or the identifiers:

-parse: convert from xml to your format, in other words, read the xml
-generate: convert from your format to xml, in other words, write the xml
-name: name of a tag, for example <name>x</name>
-opening tag: <a>
-closing tag: </a>
-singleton tag: tag that only has a name (empty tag). It can have attributes though. Example: <a/>
-element: the combination of an opening tag + content + closing tag, or a singleton tag
-content: that what is between the tags, for example: <a x="y">content</a>
-attributes: that what is optionally inside an opening or singleton tag, for example: <a attribute_name="attribute_content">b</a>
-value: content that is not tags but a string that can be converted to a C++ type, also called "simple content"
-nested content: content that consists out of more elements inside of it, also called "element content"
-mixed content: content that contains both elements and a value that is not inside an element. Not allowed in lpi_xml.
-entity: the symbols &, < and > must be converted to an entity before you save them as string in XML
-pos: the index of the character we're currently parsing in the full xml string
-end: the index of the end of the full xml string; unlike pos, the end is constan

Example:
--------

There is an example at the bottom of the .cpp file, that shows how to use the different methods.
In the compose example, it shows how to implement the parsing and generating functions. In the parseXML
functions, you need to use the while loop similar to in the example.
*/


namespace lpi
{
namespace xml
{

extern const std::string xml_indentation_symbol;
extern const std::string xml_newline_symbol;

////////////////////////////////////////////////////////////////////////////////
// 1.) General XML Tools
////////////////////////////////////////////////////////////////////////////////

///Tools

template<typename T>
void deletify(T& t) //delete all pointers, then clear the container. This function has nothing to do with XML, except that you often need to deletify std::containers of pointers before parsing
{
  for(typename T::iterator it = t.begin(); it != t.end();) delete (*it); t.clear();
}

void skipWhiteSpace(const std::string& in, size_t& pos, size_t end);
void skipToChar(const std::string& in, size_t& pos, size_t end, char c);
bool isWhiteSpace(char c);
bool isCharacter(const std::string& in, size_t pos, size_t end, char c); //made to have the < size and == test in one function
bool isNotCharacter(const std::string& in, size_t pos, size_t end, char c); //made to have the < size and != test in one function

void entitify(std::string& out, const std::string& in, size_t pos, size_t end);
void unentitify(std::string& out, const std::string& in, size_t pos, size_t end);

//conversions between C++ and strings for xml

//C++ to XML (convert). The converted value is appended to the string.
void convert(std::string& out, bool in); //bool -> boolean
void convert(std::string& out, char in); //char -> byte
void convert(std::string& out, int in); //int -> int
void convert(std::string& out, short in); //short -> short
void convert(std::string& out, long in); //long -> long
void convert(std::string& out, unsigned char in); //unsigned char -> unsignedByte
void convert(std::string& out, unsigned short in); //unsigned short -> unsignedShort
void convert(std::string& out, unsigned int in); //unsigned int -> unsignedInt
void convert(std::string& out, unsigned long in); //unsigned long -> unsignedLong
void convert(std::string& out, float in); //float -> float
void convert(std::string& out, double in); //double -> double
void convert(std::string& out, const void* in);
void convert(std::string& out, void* in);
void convert(std::string& out, const std::string& in); //std::string -> string
void convert(std::string& out, const std::vector<unsigned char>& in); //std::vector<unsigned char> -> base64Binary

//XML to C++ (unconvert). The pos and end values are the range in the string to parse.
void unconvert(bool& out, const std::string& in, size_t pos, size_t end);
void unconvert(char& out, const std::string& in, size_t pos, size_t end);
void unconvert(short& out, const std::string& in, size_t pos, size_t end);
void unconvert(int& out, const std::string& in, size_t pos, size_t end);
void unconvert(long& out, const std::string& in, size_t pos, size_t end);
void unconvert(unsigned char& out, const std::string& in, size_t pos, size_t end);
void unconvert(unsigned short& out, const std::string& in, size_t pos, size_t end);
void unconvert(unsigned int& out, const std::string& in, size_t pos, size_t end);
void unconvert(unsigned long& out, const std::string& in, size_t pos, size_t end);
void unconvert(float& out, const std::string& in, size_t pos, size_t end);
void unconvert(double& out, const std::string& in, size_t pos, size_t end);
void unconvert(const void*& out, const std::string& in, size_t pos, size_t end);
void unconvert(void*& out, const std::string& in, size_t pos, size_t end);
void unconvert(std::string& out, const std::string& in, size_t pos, size_t end);
void unconvert(std::vector<unsigned char>& out, const std::string& in, size_t pos, size_t end);

template<typename T>
void unconvert(T& out, const std::string& in)
{
  unconvert(out, in, 0, in.size());
}

//base64 is used to put binary data in XML tags
void encodeBase64(const std::vector<unsigned char>& in, std::string& out);
void decodeBase64(std::vector<unsigned char>& out, const std::string& in, size_t pos = 0, size_t end = 0);

//getting line and column number is useful for showing error messages given the pos in the string where it happened
int getLineNumber(const std::string& fulltext, size_t pos);
int getColumnNumber(const std::string& fulltext, size_t pos);

//returns if this tag is nested (true) or a value tag (false). Combinations are not supported!
bool isNestedTag(const std::string& in, size_t pos, size_t end);
//checks if the given name is a comment <!----> or xml declaration <??>
bool isCommentTag(const std::string& in, size_t pos, size_t end);

///Generating

//generates a space, then the attribute name, then "=", then a quote, then the value, then a quote again to close it
template<typename T>
static void generateAttribute(std::string& out, const std::string& name, const T& value)
{
  out += " ";
  out += name;
  out += "=";
  out += '"';
  convert(out, value);
  out += '"';
}

/*
generateValueTag: generate a tag with a simple value or string in it. This can only be used
to generate tags with primitives. Nested tags and attributes can't be generated with this.
If the value converts into an empty string, a singleton tag is generated.
*/
template<typename T>
static void generateValueTag(std::string& out, const std::string& name, const T& value, size_t indent) //if the input is of type string, then it'll be entitified. Therefore, you can't use this function to generate nested tags.
{
  for(size_t i = 0; i < indent; i++) out += xml_indentation_symbol;
  out += "<" + name + ">";
  size_t size = out.size();
  convert(out, value);
  if(out.size() == size) //nothing was added, so make this a singleton tag. Change the ">" into "/>".
  {
    out[out.size() - 1] = '/'; //the > must become a '/'
    out += ">";
  }
  else //value was added, now add the end tag
  {
    out += "</" + name + ">";
  }
  out += xml_newline_symbol;
}

///Parsing

struct Error //this is thrown by some parsing functions
{
  int error;
  int pos; //the position in the text file, convert to line number with XML::getLineNumber(pos)
  
  Error(int i_error, int i_pos) : error(i_error), pos(i_pos) {}
};

//return values of parse functions
const int SUCCESS = 0;
const int END = 1;
const int MIN_ERROR = 10; //error values are values >= 10

inline bool isError(int result) { return result >= MIN_ERROR; }

struct ParsePos //information about the tag, in an efficient format by giving positions in the main string instead of copying each string part (this works for parsing but not for generating)
{
  //std::string* s; //pointer to the full string, to which the positions refer
  size_t tb; //tag begin: the begin of this tag (the position of the first < symbol)
  size_t te; //tag end: the end of this tag (the position after the last > symbol)
  size_t ab; //attributes begin: the beginning of all the attributes
  size_t ae; //attributes end: the end of all the attributes (after the last ' or " symbol)
  size_t cb; //content begin: the beginning of all the content (after the position of the > symbol of the start tag)
  size_t ce; //content end: the end of all the content (the position of the < symbol of the end tag)
};

//these parse functions can throw lpi::XML::Error objects if an error happens; their return value is to see the difference between SUCCESS and END
int parseTag(const std::string& in, size_t& pos, size_t end, std::string& name, ParsePos& parsepos, bool skipcomments = true); //the content is empty if it was a singleton tag or an empty tag (you can't see the difference)
int parseAttribute(const std::string& in, size_t& pos, size_t end, std::string& name, ParsePos& parsepos);

//this function can also throw an lpi::XML::Error object
void skipToNonCommentTag(const std::string& in, size_t& pos, size_t end); //call when you want to go to a '<' that is not of a comment tag. After calling this function, pos is on top of the first non-comment '<'. This function can throw.

////////////////////////////////////////////////////////////////////////////////
// 2.) Reference Resolver Tool
////////////////////////////////////////////////////////////////////////////////

struct RefRes //reference resolver
{
  /*
  RefRes is the reference (or pointer/address) resolver.
  When generating, let things that are pointed to save their address, and let things that point to it
  save the pointer value. When parsing, RefRes can then resolve everything back.
  Please see the World/Monster/MonsterType example in the .cpp file for usage.
  */
  std::map<const void*, void*> pointers;
  std::vector<void**> clients; //these will be resolved at the end
  const void* lastold; //used to communicate address to the caller
  
  void addPair(const void* old, void* newa); //register a old/new value pair
  void addClient(void** client, void* address); //register a client, that will be resolved (correct address stored in it) at the end
  void resolve(); //store the new values in all the clients

  /*use RefRes::getAddress(this) instead of "this" or pointers to a member to write the address to the XML,
  to make sure an unambigous address is written to it (using the convert or generateValueTag function with void*)*/
  template<typename T> static void* getAddress(T* t) { return static_cast<void*>(t); }
  template<typename T> static const void* getAddress(const T* t) { return static_cast<const void*>(t); }
};

////////////////////////////////////////////////////////////////////////////////
// 3.) XML parsing and generating with composition
////////////////////////////////////////////////////////////////////////////////

/*
For this system, make your classes have a parseXML function for parsing, or a
generateContent and generateAttribute function for the generating, and then use
these template functions to make it all work.
For parsing, in a parseXML function, loop through parseTag calls and then:
-to parse primitive data members, use the unconvert function
-to parse other classes, call their parseXML function
-to initiate the parsing, use one of the template functions below
For generating in a generateContent or generateAttributes function:
-to generate primitive data members, use generateValueTag/generateAttribute
-to generate other classes or start the process, use the template function below on it
See the example in the .cpp file for this usage.

Note: there are two versions of the parser: one with and one without RefRes. For
the one with RefRes, the parseXML function in classes should have such a parameter too.
*/

/*
The parse functions work on classes that must have the following public function:
  void parseXML(const std::string& in, xml::ParsePos& pos)
  or
  void parseXML(const std::string& in, xml::ParsePos& pos, RefRes& ref)
These parse functions will not throw but return the error code, and these are the ones
to be called from the outside world (while parseXML is supposed to be called from within
other parseXML functions)
*/

//version without RefRes

template<typename T>
int parse(T& t, const std::string& in, size_t& pos, size_t end)
{
  try
  {
    ParsePos parsepos;
    std::string name_dummy; //value will be ignored...
    parseTag(in, pos, end, name_dummy, parsepos);
    t.parseXML(in, parsepos);
  }
  catch(Error error)
  {
    pos = error.pos;
    return error.error;
  }
  
  return SUCCESS;
}

template<typename T>
int parse(T& t, const std::string& in, size_t& pos)
{
  return parse(t, in, pos, in.size());
}

template<typename T>
int parse(T& t, const std::string& in)
{
  size_t pos = 0;
  return parse(t, in, pos, in.size());
}

//RefRes version

template<typename T>
int parseRefRes(T& t, const std::string& in, size_t& pos, size_t end)
{
  try
  {
    ParsePos parsepos;
    std::string name_dummy; //value will be ignored...
    parseTag(in, pos, end, name_dummy, parsepos);
    RefRes ref;
    t.parseXML(in, parsepos, ref);
    ref.resolve();
  }
  catch(Error error)
  {
    pos = error.pos;
    return error.error;
  }
  
  return SUCCESS;
}

template<typename T>
int parseRefRes(T& t, const std::string& in, size_t& pos)
{
  return parse(t, in, pos, in.size());
}

template<typename T>
int parseRefRes(T& t, const std::string& in)
{
  size_t pos = 0;
  return parse(t, in, pos, in.size());
}

/*
The generate functions require two functions in the type:
  void generateContent(std::string& out, size_t indent) const
  and
  void generateAttributes(std::string& out) const
I found no way to allow putting them in one function without too much temporary std::string copies.
*/

template<typename T>
void generate(std::string& out, const std::string& name, const T& t, size_t indent = 0)
{
  for(size_t i = 0; i < indent; i++) out += xml_indentation_symbol;

  indent++;
  
  out += "<" + name;
  t.generateAttributes(out);
  size_t size_before_adding_tag_close_symbol = out.size(); //you can't know for sure how much characters the newline symbol will be
  out += ">" + xml_newline_symbol;
  size_t size_before_adding_content = out.size();
  t.generateContent(out, indent);
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

    out += "</" + name + ">";
  }
  out += xml_newline_symbol;
}

////////////////////////////////////////////////////////////////////////////////
// 4.) XML parsing and generating with tree
////////////////////////////////////////////////////////////////////////////////

class XMLTree //this is 1 node, all nodes together make up the tree
{
  /*
  Unlike the above, where the parse functions only parse the attributes and content,
  for XMLTree each node will parse it's own complete tag, including tag name.
  */
  public:
  
  XMLTree();
  ~XMLTree();
  
  ///Public data members representing the XML tag
  
  struct NameValue
  {
    std::string name;
    std::string value; //use the XML::convert functions to convert to your datatype
  };
  
  NameValue content;
  std::vector<XMLTree*> children;
  std::vector<NameValue> attributes;
  
  ///Parse and generate functions
  
  //these parse functions will not throw anything but return the error as an integer. Input must be tag, including name and content
  int parse(const std::string& in, bool skipcomments = true); //feed the complete xml string to this, including the tags of this node itself
  int parse(const std::string& in, size_t& pos, bool skipcomments = true);
  int parse(const std::string& in, size_t& pos, size_t end, bool skipcomments = true);
  
  /*
  Using the parseOuter functions makes the name and attributes of this node empty and is
  meant for the "outer" scope. Input must be the content instead of a full tag with name.
  It'll parse all tags (including comments if enabled), and (unlike parse()) supports
  multiple root elements. If you parse without skipcomments, you have to use parseOuter,
  otherwise the xml declaration will be wrongly seen as the sole root element. The whole
  point of the xml-rule that there must be a root tag is made useless due to the fact that
  there can be comments and an xml declaration at the highest scope... hence these
  parseOuter functions. If you parse without comments, you can use the parse functions
  directly, the advantage then is that this node will be the real root node.
  */
  int parseOuter(const std::string& in, bool skipcomments = true);
  int parseOuter(const std::string& in, size_t& pos, bool skipcomments = true);
  int parseOuter(const std::string& in, size_t& pos, size_t end, bool skipcomments = true);
  
  void generate(std::string& out) const; //appends to the string
  
  ///Helper functions
  
  bool isEmptyValueTag() const; //returns true if it has no children and the value is empty (==> create singleton tag)
  bool isValueTag() const; //returns true if this has no nested tags in it (no children)
  bool isComment() const; //returns true if this is a comment or xml declaration
  bool isOuter() const; //returns true if this tag has no name, only content
  XMLTree* getFirstNonCommentNode() const; //use this if you parsed with "parseOuter", to get the true xml root.
  XMLTree* getParent() const { return parent; }
  size_t getLevel() const; //depth in the tree
  
  /*
  if this tag is a comment or xml declaration, then the name will include the !, -- and/or ? symbols,
  and the content will be empty
  */
  
  protected:
  XMLTree* parent;
  bool outer; //if true: generate no name tags for this, only the content
  bool comment;
  void parseContent(const std::string& in, ParsePos& parsepos, bool skipcomments = true); //can throw if error happens
  void generateChildren(std::string& out) const;
};


} //namespace xml
} //namespace lpi

#endif //lpi_xml_h_included
