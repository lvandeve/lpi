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

This supports parsing and generating of xml files without mixed content. This is to be used to read/save settings
from/to a file, such as descriptions of levels of a game, series of input keys from a keyboard, profiles, ...

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
-Case sensitive tag names, as specified by the XML standard. So the following is invalid: <a>text</A>
-Comments, these are tags of the form <!--comment-->. Avoid using other forms. But, the confusing rule
  about the "--" is followed, so this is a legal comment: "<!------>comment-->".
  The empty comment is "<!>".
  Comments are allowed only between tags or nested tags. They are NOT allowed to be mixed inside value tags.
  That means, this is ok: <a><!><b>value</b></a>, but this is NOT ok: <a><!>value</a>
  Comments are ignored by XMLCompose, but are optionally stored as comments in XMLTree to allow pretty printing
  to include the comments and xml declaration.

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

Three Classes:
--------------

There are 3 classes and thus 3 different ways (+ combinations) to parse and generate your xml:

*) XML: this class provides only a basic toolset to parse and generate tags, and convert values
        from tags to C++ datatypes. This is made a class, even though it has only static functions,
        because inheriting from it them brings all these functions in the scope of the class
        that inherits from it, which turned out to be handier than namespaces.
*) XMLCompose: this class is intended to be used by composition. You can translate your class structure
               to XML this way. It can be tricky to implement this and the class diagram and xml
               structure have to match each other. This way allows a sort of serialization and allows
               storing and loading a complete data structure that includes pointers to other data members.
*) XMLTree: this allows storing the xml in a tree and generating xml out of it again. This is an easy way
            that works without enforcing the xml on your class structure, instead you get a tree that can
            be easier to get values from.
*) Combinations: You can also combine XMLCompose and XMLTree, e.g. using XMLTree to parse subtags that
                 XMLCompose encounters. This allows a class to use a more nested structure, without
                 needing more classes.

Errors and exceptions
---------------------

Some parse functions indicate their level of success with an integer:

0 or XML::SUCCESS: result fine, end not reached
1 or XML::END: result fine, end reached of this tag or subtag (see the example for how in a while loop you keep parsing while the return value is SUCCESS and stop if it's END)
>= 10: some error happened, which error it is: see the return values in the code

Other parse functions will throw an lpi::XML::Error object, with in it an integer value
that has the same rules as the returned integer described above.

The reason for the exceptions is because I saw no other sane way to release the implementer
of doParse... functions to do a lot of error checking. However, the functions you normally
call from the external world will not throw anything and return an integer instead. The
C++ exceptions are only used internally. You can throw in the doParse... functions that
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

There is an example at the bottom of the .cpp file, that shows how to use both XMLCompose and XMLTree.
In the XMLCompose example, it shows how to implement the parse and generate functions. In the parse
functions, you need to use the while loop similar to in the example.
*/


namespace lpi
{

////////////////////////////////////////////////////////////////////////////////
//XML Tools
////////////////////////////////////////////////////////////////////////////////

class XML //tools for XML
{
  public:
  
  virtual ~XML(){}
  
  ///Tools
  
  template<typename T>
  static void deletify(T& t) //delete all pointers, then clear the container. This function has nothing to do with XML, except that you often need to deletify std::containers of pointers before parsing
  {
    for(typename T::iterator it = t.begin(); it != t.end();) delete (*it); t.clear();
  }
  
  static void skipWhiteSpace(const std::string& in, size_t& pos, size_t end);
  static void skipToChar(const std::string& in, size_t& pos, size_t end, char c);
  static bool isWhiteSpace(char c);
  static bool isCharacter(const std::string& in, size_t pos, size_t end, char c); //made to have the < size and == test in one function
  static bool isNotCharacter(const std::string& in, size_t pos, size_t end, char c); //made to have the < size and != test in one function

  static void entitify(std::string& out, const std::string& in, size_t pos, size_t end);
  static void unentitify(std::string& out, const std::string& in, size_t pos, size_t end);

  //conversions between C++ and strings for xml
  
  //C++ to XML (convert)
  static void convert(std::string& out, bool in); //bool -> boolean
  static void convert(std::string& out, char in); //char -> byte
  static void convert(std::string& out, int in); //int -> int
  static void convert(std::string& out, short in); //short -> short
  static void convert(std::string& out, long in); //long -> long
  static void convert(std::string& out, unsigned char in); //unsigned char -> unsignedByte
  static void convert(std::string& out, unsigned short in); //unsigned short -> unsignedShort
  static void convert(std::string& out, unsigned int in); //unsigned int -> unsignedInt
  static void convert(std::string& out, unsigned long in); //unsigned long -> unsignedLong
  static void convert(std::string& out, float in); //float -> float
  static void convert(std::string& out, double in); //double -> double
  static void convert(std::string& out, const void* in);
  static void convert(std::string& out, void* in);
  static void convert(std::string& out, const std::string& in); //std::string -> string
  static void convert(std::string& out, const std::vector<unsigned char>& in); //std::vector<unsigned char> -> base64Binary
  //static void convert(std::string& out, const XMLCompose* in); //nested; this can't be implemented: it must know more parameters, to do indentation of nested tags
  
  //XML to C++ (unconvert)
  static void unconvert(bool& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(char& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(short& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(int& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(long& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(unsigned char& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(unsigned short& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(unsigned int& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(unsigned long& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(float& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(double& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(const void*& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(void*& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(std::string& out, const std::string& in, size_t pos, size_t end);
  static void unconvert(std::vector<unsigned char>& out, const std::string& in, size_t pos, size_t end);
  //static void unconvert(XMLCompose* out, const std::string& in, size_t pos, size_t end); //this can't be implemented: it must know more parameters, to do indentation of nested tags
  
  template<typename T>
  static void unconvert(T& out, const std::string& in)
  {
    unconvert(out, in, 0, in.size());
  }
  
  static void encodeBase64(const std::vector<unsigned char>& in, std::string& out);
  static void decodeBase64(std::vector<unsigned char>& out, const std::string& in, size_t pos = 0, size_t end = 0);
  
  static int getLineNumber(const std::string& fulltext, size_t pos); //used for showing error message that indicate line instead of character number
  static int getColumnNumber(const std::string& fulltext, size_t pos); //used for showing error message that indicate column instead of character number
  
  static bool isNestedTag(const std::string& in, size_t pos, size_t end); //checks if the given content is either a value, or nested tag(s) (combinations are not supported!)
  static bool isCommentTag(const std::string& in, size_t pos, size_t end); //checks if the given name is a comment <!----> or xml declaration <??>
  ///Generating
  
  /*
  -Done separatly in XMLCompose and XMLTree due to the big differences of their system.
  -Also generating is much easier than parsing so no shared tools were needed here.
  -Finally, there is e.g. no function that generates an open or close tag here, because
  generating an open tag with "<" + name + ">" would be quite useless since in some cases
  you want to add attributes between the name and the ">". So it ended up not being code
  duplication at all to have statements of the form "<" + name + ... separatly
  in XMLCompose and XMLTree.
  */
  
  ///Parsing
  
  struct Error //this is thrown by some parsing functions
  {
    int error;
    int pos; //the position in the text file, convert to line number with XML::getLineNumber(pos)
    
    Error(int i_error, int i_pos) : error(i_error), pos(i_pos) {}
  };
  
  //return values of parse functions
  static const int SUCCESS = 0;
  static const int END = 1;
  static const int MIN_ERROR = 10; //error values are values >= 10
  
  static bool isError(int result) { return result >= MIN_ERROR; }
  
  struct ParseTagPos //information about the tag, in an efficient format by giving positions in the main string instead of copying each string part (this works for parsing but not for generating)
  {
    //std::string* s; //pointer to the full string, to which the positions refer
    size_t tb; //tag begin: the begin of this tag (the position of the first < symbol)
    size_t te; //tag end: the end of this tag (the position after the last > symbol)
    size_t ab; //attributes begin: the beginning of all the attributes
    size_t ae; //attributes end: the end of all the attributes (after the last ' or " symbol)
    size_t cb; //content begin: the beginning of all the content (after the position of the > symbol of the start tag)
    size_t ce; //content end: the end of all the content (the position of the < symbol of the end tag)
  };
  
  struct ParseAttributePos //information about the attribute, in an efficient format by giving positions in the main string instead of copying each string part (this works for parsing but not for generating)
  {
    //std::string* s; //pointer to the full string, to which the positions refer
    size_t ab; //attribute begin: the beginning of this attribute (name + value)
    size_t ae; //attribute end: the end of this attribute (name + value)
    size_t cb; //content begin: the beginning of all the value (after the opening ' or ")
    size_t ce; //content end: the end of all the value (the closing ' or ")
  };
  
  //these parse functions can throw lpi::XML::Error objects if an error happens; their return value is to see the difference between SUCCESS and END
  static int parseTag(const std::string& in, size_t& pos, size_t end, std::string& name, ParseTagPos& parsepos, bool skipcomments = true); //the content is empty if it was a singleton tag or an empty tag (you can't see the difference)
  static int parseAttribute(const std::string& in, size_t& pos, size_t end, std::string& name, ParseAttributePos& parsepos);

  //this function can also throw an lpi::XML::Error object
  static void skipToNonCommentTag(const std::string& in, size_t& pos, size_t end); //call when you want to go to a '<' that is not of a comment tag. After calling this function, pos is on top of the first non-comment '<'. This function can throw.
};

////////////////////////////////////////////////////////////////////////////////
//XMLCompose (without tree, use by inheriting from it)
////////////////////////////////////////////////////////////////////////////////

class XMLCompose : public XML
{
  /*
  The XMLCompose will parse/generate only the attributes and the content. It will
  not parse/generate the name of its own tag. The reason is that the higher up class chooses
  the name of the tag of this class: for example if a Monster class would contain
  multiple Vector values, it'll call several generate functions of the Vector class,
  but will give each a different name (e.g. position, velocity, ...).
  All you have to implement is the doParseContent and doGenerateContent
  (optionally also doParseAttributes and doGenerateAttributes)
  functions, in which you read/write the values (the members of you class).
  */
  public:
  
  struct RefRes //reference resolver
  {
    /*
    RefRes is the reference resolver. It resolves references, or more precicely, pointers and addresses.
    When generating, let things that are pointed to save their address, and let things that point to it
    save the pointer value.
    When parsing, if you use RefRes correctly, you can make it resolve all addresses back even if after
    parsing totally new values were generated.
    Please see the World - Monster - MonsterType example in the cpp file for usage.
    */
    std::map<const void*, void*> pointers;
    std::vector<void**> clients; //these will be resolved at the end
    const void* lastold; //used to communicate address to the caller
    
    void addPair(const void* old, void* newa); //register a old/new value pair
    
    /*
    addClient: register a client, that will be resolved at the end
    e.g. if you have a Monster with in it a MonsterType* type, use "add((void**)(&type), address)" while
    parsing, where address is the void* that you read from the XML file. MonsterType should also
    inherit from XMLCompose.
    */
    void addClient(void** client, void* address);
    
    void resolve(); //store the new values in all the clients
  };
  
  void* getAddress(); //use getAddress() instead of "this" or pointers to a member to write the address of this to the XML, to make sure an unambigous address is written to the XML
  const void* getAddress() const; //use getAddress() instead of "this" or pointers to a member to write the address of this to the XML, to make sure an unambigous address is written to the XML
  
  virtual ~XMLCompose(){}
  
  //these parse functions will NOT throw but return the error code, and these are the ones to be called from the outside world
  int parse(const std::string& in);
  int parse(const std::string& in, size_t& pos); //this one allows you to retrieve the pos, to see where a possible error happened to display the line number to the user
  int parse(const std::string& in, size_t& pos, size_t end);
  //but THIS one throws an lpi::XML::error object because it's meant to be used from the inside of doParse... functions
  void parse(const std::string& in, size_t ab, size_t ae, size_t cb, size_t ce, RefRes& ref);
  
  void generate(std::string& out, const std::string& name) const;
  void generate(std::string& out, const std::string& name, size_t indent) const;
  
  protected:
  
  //these parse functions can throw lpi::XML::Error objects
  virtual void doParseAttributes(const std::string& /*in*/, size_t& /*pos*/, size_t /*end*/, RefRes& /*ref*/) {}
  virtual void doParseContent(const std::string& /*in*/, size_t& /*pos*/, size_t /*end*/, RefRes& /*ref*/) {}
  virtual void doGenerateAttributes(std::string& /*out*/) const {}
  virtual void doGenerateContent(std::string& /*out*/, size_t /*indent*/) const {}
  
  //if the content string is empty, it'll generate a singleton tag. The generate function will NOT convert anything to entities, you must manually call functions on a string to convert that part to entities. This because the content can contain other tags, which have all these symbols and may not be broken.
  template<typename T>
  static void generateValueTag(std::string& out, const std::string& name, const T& value, size_t indent) //if the input is of type string, then it'll be entitified. Therefore, you can't use this function to generate nested tags.
  {
    std::string value_string;
    convert(value_string, value);
    
    generateTagString(out, name, value_string, indent);
  }
  
  static void generateTag(std::string& out, const std::string& name, const XMLCompose& value, size_t indent);
  static void generateTag(std::string& out, const std::string& name, const XMLCompose* value, size_t indent);
  
  //generates a space, then the attribute name, then "=", then a quote, then the value, then a quote again to close it
  template<typename T>
  static void generateAttribute(std::string& out, const std::string& name, const T& value)
  {
    std::string value_string;
    convert(value_string, value);
    generateAttributeString(out, name, value_string);
  }
  
  static void generateTagString(std::string& out, const std::string& name, const std::string& content, size_t indent);
  static void generateAttributeString(std::string& out, const std::string& name, const std::string& value);
};

////////////////////////////////////////////////////////////////////////////////
//XMLTreeParser (stores result in tree)
////////////////////////////////////////////////////////////////////////////////

class XMLTree : public XML //this is 1 node, all nodes together make up the tree
{
  /*
  Unlike XMLCompose, for XMLTree each node will parse it's own tag, including
  tag name, completely by itself. However, XMLTree is not designed to be
  inherited from.
  */
  public:
  
  struct NameValue
  {
    std::string name;
    std::string value; //use the XML::convert functions to convert to your datatype
  };
  
  NameValue content;
  std::vector<XMLTree*> children;
  std::vector<NameValue> attributes;
  XMLTree* parent;
  
  XMLTree();
  ~XMLTree();
  size_t getLevel() const; //depth in the tree
  
  //these parse functions will not throw anything but return the error as an integer. Input must be tag, including name and content
  int parse(const std::string& in, bool skipcomments = true); //feed the complete xml string to this, including the tags of this node itself
  int parse(const std::string& in, size_t& pos, bool skipcomments = true);
  int parse(const std::string& in, size_t& pos, size_t end, bool skipcomments = true);
  
  /*
  using the parseOuter functions makes the name and attributes of this node empty and is
  meant for the "outer" scope. Input is "content" instead of a full tag with name.
  It'll parse all tags (including comments if enabled), and supports multiple root elements.
  If you parse without skipcomments, you have to use this, otherwise the xml declaration will be
  wrongly seen as the root element. The whole point of the xml-rule that there must be a root
  tag is made useless due to the fact that there can be comments and an xml declaration at
  the highest scope... hence these parseOuter functions. If you parse without comments, you
  can use the parse functions directly, the advantage then is that this node will be the real
  root node.
  */
  int parseOuter(const std::string& in, bool skipcomments = true);
  int parseOuter(const std::string& in, size_t& pos, bool skipcomments = true);
  int parseOuter(const std::string& in, size_t& pos, size_t end, bool skipcomments = true);
  
  bool isEmptyValueTag() const; //returns true if it has no children and the value is empty (==> create singleton tag)
  bool isValueTag() const; //returns true if this has no nested tags in it (no children)
  void generate(std::string& out) const; //appends to the string
  bool isComment() const; //returns true if this is a comment or xml declaration
  bool isOuter() const; //returns true if this tag has no name, only content
  XMLTree* getFirstNonCommentNode() const; //use this if you parsed with "parseUpper", to get the true xml root.
  
  /*
  if this tag is a comment or xml declaration, then the name will include the !, -- and/or ? symbols,
  and the content will be empty
  */
  
  protected:
  bool outer; //if true: generate no name tags for this, only the content
  bool comment;
  void parseContent(const std::string& in, ParseTagPos& parsepos, bool skipcomments = true); //can throw if error happens
  void generateChildren(std::string& out) const;
};

}

#endif //lpi_xml_h_included
