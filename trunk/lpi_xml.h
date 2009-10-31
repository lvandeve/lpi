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

#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <iostream>


/*
lpi_xml: Lode's XML system
--------------------------

This supports parsing and generating of XML files for games and simple programs.
This is to be used to read/save settings from/to a file, such as descriptions of
levels of a game, mouse/keyboard controls, profiles, ...

It may not have the safety and efficiency of some other systems, but it's convenient and small, at least if you
follow the example in the .cpp file with the World, MonsterType and Monster tags.

Supported:
----------

-Parsing of ascii or UTF-8 XML documents (this depends on where this is compiled though)
-4 content types: empty, pure text/value, child elements, mixed
-7 tag types: start, end, singleton, comment, section, declaration, processing instruction
-Attributes in start and singleton tags, e.g. <a b="c">d</a> or <a b="c"/>
-Several value types, such as integers, floating point numbers, booleans, void*, strings and base64-encoded octet streams.
-Pretty Printing
-Storing and recreating a datastructure with pointers (with updated addresses).
-Storing binary data in base64 format in XML
-Case sensitive tag names, as specified by the XML standard. So the following is invalid: <a>text</A>
-Comments, these are tags of the form <!--comment-->. Avoid using other forms. But, the confusing rule
  about the "--" is followed, so this is a legal comment: "<!------>comment-->".
  NOTE: when using code like the example, do not place comments in simple elements or they become mixed elements
  and will be parsed incorrectly.
-Other tag types that don't participate in the data (such as declarations) are recognised but ignored.
-The option to completely ignore comments is supported
-The concept of well-formedness of XML is supported.

Not supported:
--------------

-Other kinds of string formats than ascii or UTF-8 (this depends on where this is compiled though)
-Schema or doctype definition and validation - your C++ code is the "schema" :)
-Mixed content (that is, both text/values and nested elements) isn't always supported (but sometimes is).
-The XML version declaration of the form <?...?> is optional and ignored. But at least it won't cause an error if you include one: it's treated as a comment.
-The concept of validity of XML isn't supported since schemas/doctype definitions aren't supported

Different methods
-----------------

There are different ways to parse and generate available:

*) Tools: There's a basic toolset to parse and generate tags, and convert values
      from tags to C++ datatypes. This also defines the primitive datatypes that
      can be parsed and generated to tags.
*) With composition: this is intended to be used by composition. You can translate your class
      structure to XML this way. It can be tricky to implement this in a nice way though.
      This way allows a sort of serialization and allows storing and loading a complete data
      structure that includes pointers to other data members.
      This method always ignores comments & co, because the focus is on the data.
*) XMLTree: this allows storing the XML in a tree and generating XML out of it again. This is an easy way
            that works without enforcing certain functions to be made in classes or even making classes at all.
            This method allows you to include comments, for full pretty printing of everything.
*) Combinations: You can also combine the composition and XMLTree method, e.g. using XMLTree to parse child
                  elements that are encountered.

Errors and exceptions
---------------------

Some parse functions indicate their level of success with an integer and won't throw anything:

0 or XML::SUCCESS: result fine, end not reached
1 or XML::END: result fine, end reached of this tag or subtag (see the example for how in a while loop you keep parsing while the return value is SUCCESS and stop if it's END)
>= 10: some error happened. To see which error it is: look in the lpi_xml.cpp code for the value.

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

Whitespace are spaces, tabs, newlines, ...

Whitespace follows these rules:

*) Whitespace inside the content of simple value tags is preserved. This
    includes spaces, newlines and tabs. They're all parsed as part of the
    string. This is the XML rule "whitespace is preserved".
*) Whitespace between tags, where there is no content, is ignored. When pretty
    printing, this whitespace may be changed to make a pretty tree.
*) For mixed elements, any white space before a mixed text is ignored, but any
    whitespace behind the mixed text is included in the mixed text. Here too,
    when pretting printing, the white space before it can be changed to make it
    a pretty tree.

Glossary:
---------

Here are the meanings of some of the names used in the comments or the identifiers of lpi_xml.
This is based on a mix of the main XML specification and the XML Schema specification.

*) general
-parse: convert from XML to your format, in other words, read the XML
-generate: convert from your format to XML, in other words, write the XML
-tag: something that starts with "<" and ends with ">", usually with no "<" or ">" symbols in between
-name: name of a tag, for example <name>x</name>
-element: the combination of an opening tag + content + closing tag, or a
  singleton tag if the element is empty
-content: that what is between the tags in an element, for example: <a x="y">content</a>
-attributes: that what is optionally inside an opening or singleton tag, for
  example: <a attribute_name="attribute_content">b</a>
-entity: symbol. E.g. the symbols &, < and > must be converted to an entity
  before you save them as string in XML
-pos: the index of the character we're currently parsing in the full XML string
-end: the index of the end of the full XML string; unlike pos, the end is constan
-child element: element that resides inside the content of another element

*) tag types
-start tag: opening tag: <a>
-end tag: closing tag: </a>
-empty-element tag: singleton tag: shortcut for empty element. It can have
  attributes though. Example: <a/>, the same as <a></a>
-processing instruction: a lookalike-tag of the form <?text?>
-comment: a lookalike-tag of the form <!--text-->
-declaration: a lookalike-tag of the form <!text> (that is not a section)
-section: a lookalike-tag of the form <![text[text]]>
-mixed text: not a tag, but in mixed elements, the text parts are parsed as
  tags that are of type "mixed text" and aren't really a tag (no < > symbols)

*) content types (also determines the type of the element of which this is the content)
-value: content that is not tags but a string that can be converted to a C++
  type, also called "simple content"
-element content: content that consists out of only child elements
-empty content: no content at all. Note: lpi_xml allows pure whitespace to be
  parsed in its form as a string, so whitespace isn't necessarily empty content.
-mixed content: content that contains both elements and a value that is not
  inside an element. Not allowed in lpi_xml. NOT the "mixed content" described
  in the XML spec: they see a value with no child tags next to it also as mixed

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

//symbols used when generating
extern const std::string xml_newline_symbol; //should always be LF
extern std::string xml_indentation_symbol; //some spaces or tabs, default is two spaces
extern std::string xml_attr_quote_symbol; //may be " or ', default is "

////////////////////////////////////////////////////////////////////////////////
// 1.) General XML Tools
////////////////////////////////////////////////////////////////////////////////

///Tag and Content types

enum TagType
{
  TT_START, // <a>, can have attributes
  TT_END, // </a>
  TT_SINGLETON, // <a/>, can have attributes
  TT_MIXEDTEXT, // a, this isn't a tag, but is what you encounter while parsing mixed content. Note: text is stored in *name* of "tag"
  TT_COMMENT, // <!--a-->
  TT_PI, // <?a?>, Processing Instruction
  TT_DECLARATION, // <!a>
  TT_SECTION //<![a[a]]>
};

/*
comment, PI, declaration and section tags are all treated as comment by lpi_xml
(usually skipped) and the string between the <> symbols is seen as the tag name
*/
inline bool isTreatedAsComment(TagType type) { return type >= TT_COMMENT; }

enum ElementType //Element Type is determined by the content, or for special tags, by the TagType
{
  ET_EMPTY, //nothing, not even whitespace, not even comments & co: singleton tag or start/end pair with nothing in between (but attributes can still occur)
  ET_SIMPLE, //pure value/text only, no child elements, no comments & co
  ET_NESTED, //only child elements (whitespace does not matter here), called Element Content in the XML spec.
  ET_MIXED, //both values/text and child elements (each value/text starts after whitespace and ends at next tag, all whitespace in between is part of the value/text)
  //these below match TagTypes that aren't TT_START, TT_END and TT_SINGLETON
  ET_MIXEDTEXT, //Note: in this case, text is stored in name, not in value.
  ET_COMMENT, // <!--a-->
  ET_PI, //Processing Instruction
  ET_DECLARATION, // <!a>
  ET_SECTION //<![a[a]]>
};

/*
comment, PI, declaration and section tags are all treated as comment by lpi_xml
(usually skipped) and the string between the <> symbols is seen as the tag name
*/
inline bool isTreatedAsComment(ElementType type) { return type >= ET_COMMENT; }

///Tools

//general parsing tools
void skipWhiteSpace(const std::string& in, size_t& pos, size_t end);
void skipToChar(const std::string& in, size_t& pos, size_t end, char c);
bool isWhiteSpace(char c);
bool isCharacter(const std::string& in, size_t pos, size_t end, char c); //made to have the < size and == test in one function
bool isNotCharacter(const std::string& in, size_t pos, size_t end, char c); //made to have the < size and != test in one function

//convert string to entificated string and vica versa (do this before saving string as content, and after parsing it)
void entitify(std::string& out, const std::string& in, size_t pos, size_t end);
void unentitify(std::string& out, const std::string& in, size_t pos, size_t end);

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

//this one is for e.g. enums to avoid warning "dereferencing type-punned pointer will break strict-aliasing rules" when casting to (long&)
template<typename T>
T unconvert(const std::string& in, size_t pos, size_t end)
{
  T t;
  unconvert(t, in, pos, end);
  return t;
}

template<typename T>
T unconvert(const std::string& in)
{
  T t;
  unconvert(t, in);
  return t;
}


//base64 is used to put binary data in XML tags
void encodeBase64(const std::vector<unsigned char>& in, std::string& out);
void decodeBase64(std::vector<unsigned char>& out, const std::string& in, size_t pos = 0, size_t end = 0);

//getting line and column number is useful for showing error messages given the pos in the string where it happened
int getLineNumber(const std::string& fulltext, size_t pos);
int getColumnNumber(const std::string& fulltext, size_t pos);

//used in some examples
template<typename T>
void deletify(T& t) //delete all pointers, then clear the container. This function has nothing to do with XML, except that you may need to deletify std::containers of pointers before parsing
{
  for(typename T::iterator it = t.begin(); it != t.end();) delete (*it); t.clear();
}

///Generating

//generates a space, then the attribute name, then "=", then a quote, then the value, then a quote again to close it
template<typename T>
static void generateAttribute(std::string& out, const std::string& name, const T& value)
{
  out += " ";
  out += name;
  out += "=";
  out += xml_attr_quote_symbol;
  convert(out, value);
  out += xml_attr_quote_symbol;
}

/*
generateSimpleElement: generate a tag with a simple value or string in it. This can only be used
to generate tags with primitives. Nested tags and attributes can't be generated with this.
If the value converts into an empty string, a singleton tag is generated.
*/
template<typename T>
static void generateSimpleElement(std::string& out, const std::string& name, const T& value, size_t indent) //if the input is of type string, then it'll be entitified. Therefore, you can't use this function to generate nested tags.
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
const int SUCCESS = 0; //tag or element successfully parsed
const int END = 1; //parsing after last element or tag of this element
const int MIN_ERROR = 10; //error values are values >= 10

inline bool isError(int result) { return result >= MIN_ERROR; }

struct TagPos //information about the tag, in an efficient format by giving positions in the main string instead of copying each string part (this works for parsing but not for generating)
{
  TagType type; //the type of this tag
  size_t tb; //tag begin: the position of the first < symbol (or first non-whitespace symbol if type is "CONTENT")
  size_t te; //tag end: the position of the last > symbol (or last symbol before next tag if type is "CONTENT")
  size_t ab; //attributes begin: the beginning of all the attributes (if any)
  size_t ae; //attributes end: the end of all the attributes (after the last ' or " symbol) (if any)
};

//pos must be on or before the start of the next tag
void parseTag(const std::string& in, size_t& pos, size_t end, std::string& name, TagPos& tagpos);

struct ElementPos //information about the element, in an efficient format by giving positions in the main string instead of copying each string part (this works for parsing but not for generating)
{
  ElementType type; //the type of content this element has
  size_t eb; //element begin: the begin of this element (the position of the first < symbol)
  size_t ee; //element end: the end of this element (the position after the last > symbol)
  size_t ab; //attributes begin: the beginning of all the attributes
  size_t ae; //attributes end: the end of all the attributes (after the last ' or " symbol)
  size_t cb; //content begin: the beginning of all the content (after the position of the > symbol of the start tag)
  size_t ce; //content end: the end of all the content (the position of the < symbol of the end tag)
};

//these parse functions can throw lpi::XML::Error objects if an error happens; their return value is to see the difference between SUCCESS and END
int parseElement(const std::string& in, size_t& pos, size_t end, std::string& name, ElementPos& elementpos, bool skipcomments = true); //the content is empty if it was a singleton tag or an empty tag (you can't see the difference)
int parseAttribute(const std::string& in, size_t& pos, size_t end, std::string& name, ElementPos& elementpos);

////////////////////////////////////////////////////////////////////////////////
// 2.) Reference Resolver Tool
////////////////////////////////////////////////////////////////////////////////

struct RefRes //reference resolver
{
  typedef int Key;
  
  Key index;
  
  RefRes() : index(0) {}
  
  /*
  RefRes is the reference (or pointer/address) resolver.
  When generating, let things that are pointed to save their address, and let things that point to it
  save the pointer value. When parsing, RefRes can then resolve everything back.
  Please see the World/Monster/MonsterType example in the .cpp file for usage.
  */
  std::map<const void*, Key> pointers_writing;
  std::map<Key, const void*> pointers_reading;
  
  struct Client
  {
    const void** ref; //pointer to the pointer that must be made to point to the correct value when resolving
    Key id; //the id assiciated with this reference
  };
  
  std::vector<Client> clients; //these will be resolved at the end
  
  
  ///For Reading
  
  void addPair(Key id, void* address) //register a old/new value pair
  {
    pointers_reading[id] = address;
  }
  
  template<typename T>
  void addClient(Key id, T*& ref) //register a client, that will be resolved (correct address stored in it) at the end
  {
    clients.resize(clients.size() + 1);
    clients.back().id = id;
    clients.back().ref = (const void**)(&ref);
    (*(clients.back().ref)) = 0; //set it to null, so it's still null in case the reference remains unresolved
  }
  
  void resolve(); //store the new values in all the clients

  ///For Writing
  
  template<typename T>
  Key getKey(const T* t)
  {
    const void* old = static_cast<const void*>(t);
    if(pointers_writing.find(old) == pointers_writing.end())
    {
      pointers_writing[old] = index;
      index++;
    }
    
    return pointers_writing[old];
  }
};

////////////////////////////////////////////////////////////////////////////////
// 3.) XML parsing and generating with composition
////////////////////////////////////////////////////////////////////////////////

/*
For this system, make your classes have a parseXML function for parsing, or a
generateContent and generateAttribute function for the generating, and then use
these template functions to make it all work.
For parsing, in a parseXML function, loop through parseElement calls and then:
-to parse primitive data members, use the unconvert function
-to parse other classes, call their parseXML function
-to initiate the parsing, use one of the template functions below
For generating in a generateContent or generateAttributes function:
-to generate primitive data members, use generateSimpleElement/generateAttribute
-to generate other classes or start the process, use the template function below on it
See the example in the .cpp file for this usage.

Note: there are two versions of the parser: one with and one without RefRes. For
the one with RefRes, the parseXML function in classes should have such a parameter too.
*/

/*
The parse functions work on classes that must have the following public function:
  void parseXML(const std::string& in, XML::ElementPos& pos)
  or
  void parseXML(const std::string& in, XML::ElementPos& pos, RefRes& ref)
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
    ElementPos elementpos;
    std::string name_dummy; //value will be ignored...
    parseElement(in, pos, end, name_dummy, elementpos);
    t.parseXML(in, elementpos);
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
    ElementPos elementpos;
    std::string name_dummy; //value will be ignored...
    parseElement(in, pos, end, name_dummy, elementpos);
    RefRes ref;
    t.parseXML(in, elementpos, ref);
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
  return parseRefRes(t, in, pos, in.size());
}

template<typename T>
int parseRefRes(T& t, const std::string& in)
{
  size_t pos = 0;
  return parseRefRes(t, in, pos, in.size());
}

/*
Class which can be used for generating tags, in 3 steps:
step1 = before adding attributes
(then you must add attributes)
step2 = after adding attributes, before adding content
(then you must add content, one indentation level further)
step3 = after adding content
See the generate() template function to see an example of this class
in use and for an explanation when you want to use this class instead
of the template generate function.

The steps are done in constructor and destructor, except step2 if you
want to add attributes in between. In that case, you have to indicate
when you're done adding attributes with attributesDone()
*/
class Generate
{
  private:
  std::string& out;
  std::string name;
  size_t indent;
  size_t size_before_adding_tag_close_symbol;
  size_t size_before_adding_content;
  
  public:
  
  Generate(std::string& out, const std::string& name, size_t indent = 0, bool attributes = false)
  : out(out)
  , name(name)
  , indent(indent)
  , size_before_adding_tag_close_symbol(0)
  , size_before_adding_content(0)
  {
    //"step1"
    for(size_t i = 0; i < indent; i++) out += xml_indentation_symbol;
    out += "<" + name;
    
    if(!attributes) attributesDone();
  }
  
  void attributesDone()
  {
    //"step2"
    size_before_adding_tag_close_symbol = out.size(); //you can't know for sure how much characters the newline symbol will be
    out += ">" + xml_newline_symbol;
    size_before_adding_content = out.size();
  }
  
  ~Generate()
  {
    //"step3"
    size_t size_after_adding_content = out.size();
    
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
};

/*
The generate function requires two functions in the type T:
  void generateContent(std::string& out, size_t indent) const
  and
  void generateAttributes(std::string& out) const
I found no way to allow putting them in one function without too much temporary std::string copies.
Also this means for each nested tag you need another class with these two functions.
If you don't want that, you can use the Generate class a bit higher up instead, it allows
you to use custom functions to generate attributes and content between step1, step2 and step3.
*/

template<typename T>
void generate(std::string& out, const std::string& name, const T& t, size_t indent = 0)
{
  Generate generator(out, name, indent, true);
  t.generateAttributes(out);
  generator.attributesDone();
  t.generateContent(out, indent + 1);
}

//RefRes version

template<typename T>
void generateRefRes(std::string& out, const std::string& name, const T& t, size_t indent, RefRes& refres)
{
  Generate generator(out, name, indent, true);
  t.generateAttributes(out, refres);
  generator.attributesDone();
  t.generateContent(out, indent + 1, refres);
}

template<typename T>
void generateRefRes(std::string& out, const std::string& name, const T& t, size_t indent = 0)
{
  RefRes refres;
  generateRefRes(out, name, t, indent, refres);
}

////////////////////////////////////////////////////////////////////////////////
// 4.) XML parsing and generating with tree
////////////////////////////////////////////////////////////////////////////////

class XMLTree //this is 1 node, all nodes together make up the tree
{
  /*
  Unlike the above, where the parse functions only parse the attributes and content,
  for XMLTree each node will parse its own complete tag, including tag name.
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
  int parse(const std::string& in, bool skipcomments = true); //feed the complete XML string to this, including the tags of this node itself
  int parse(const std::string& in, size_t& pos, bool skipcomments = true);
  int parse(const std::string& in, size_t& pos, size_t end, bool skipcomments = true);
  
  /*
  Using the parseOuter functions makes the name and attributes of this node empty and is
  meant for the "outer" scope. Input must be the content instead of a full tag with name.
  It'll parse all tags (including comments if enabled), and (unlike parse()) supports
  multiple root elements. If you parse without skipcomments, you have to use parseOuter,
  otherwise the XML declaration will be wrongly seen as the sole root element. The whole
  point of the XML-rule that there must be a root tag is made useless due to the fact that
  there can be comments and an XML declaration at the highest scope... hence these
  parseOuter functions. If you parse without comments, you can use the parse functions
  directly, the advantage then is that this node will be the real root node.
  */
  int parseOuter(const std::string& in, bool skipcomments = true);
  int parseOuter(const std::string& in, size_t& pos, bool skipcomments = true);
  int parseOuter(const std::string& in, size_t& pos, size_t end, bool skipcomments = true);
  
  void generate(std::string& out) const; //appends to the string
  
  ///Helper functions
  
  //For Parsing
  ElementType getType() const { return type; }
  bool isValueTag() const; //returns true if this node has no children
  bool isOuter() const; //returns true if this element has no name, only content (it's higher than the root element)
  XMLTree* getFirstNonCommentNode() const; //use this if you parsed with "parseOuter", to get the true XML root.
  XMLTree* getParent() const { return parent; }
  size_t getLevel() const; //depth in the tree
  
  //For Generating
  void setType(ElementType type) { this->type = type; }
  XMLTree* createNewChild();
  /*
  if this tag is a comment or XML declaration, then the name will include the !, -- and/or ? symbols,
  and the content will be empty
  */
  
  protected:
  XMLTree* parent;
  bool outer; //if true: when generating, generate no start and end tags for this, only the content
  ElementType type;
  void parseContent(const std::string& in, ElementPos& elementpos, bool skipcomments = true); //can throw if error happens
  void generateChildren(std::string& out) const;
};


} //namespace xml
} //namespace lpi

