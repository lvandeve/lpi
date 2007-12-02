/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef text_h
#define text_h

#include "lpi_texture.h"
#include "lpi_color.h"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace lpi
{


static const int NUMFONT = 256;

extern const std::string font8x8string;
extern const std::string font6x6string;
extern const std::string font4x5string;

struct Font
{
  std::vector<Texture> texture; //pointer to array of 256 textures, one for each ascii character
  int getWidth(); //width of the font (of one character)
  int getHeight(); //height of the font (of one character)
};

extern Font builtInFont8x8;
extern Font builtInFont6x6;
extern Font builtInFont4x5;
extern std::vector<Font> externFont; //some other fonts that can be loaded

/*
The Markup struct allows you to set the color and shape of the letters.
Use the parameters as follows:

Markup.color1: the foreground color of the text
Markup.color2: the background or style color of the text
Markup.style: the style of the text, see below
//Markup.alpha: transparency --> included in color now
Markup.font: set it to 0 for now

Pictures of the font are declared at the bottom of this file in the "font" array

Styles of Markup.style: use bits of the style integer to turn on or off style options.
To have different effects enabled, set the style to the sum of the bit numbers
of all options you want enabled, for example 1024 + 512 = 1536, which is bold and italic

+-------+----------------------------------------------------------------------+
| bit   | effect                                                               |
+-------+----------------------------------------------------------------------+
| 0     | normal text                                                          |
| &1    | enable opaque background (this disables shadows)                     |
| &2    | shadow right one pixel                                               |
| &4    | shadow left one pixel                                                |
| &8    | shadow up one pixel                                                  |
| &16   | shadow down one pixel                                                |
| &32   | shadow right 2 pixels                                                |
| &64   | shadow left 2 pixels                                                 |
| &128  | shadow up 2 pixels                                                   |
| &256  | shadow down 2 pixels                                                 |
| &512  | bold                                                                 |
| &1024 | italic                                                               |
+-------+----------------------------------------------------------------------+

Some examples of useful combos:

0: nothing
1: background enabled
18: shadow bottom right
288: shadow two pixels bottom right
512: bold
513: bold with background
560: bold with shadow bottom right
1024: italic
1025: italic with background
1042: italic with shadow bottom right
*/
struct Markup
{
  ColorRGB color1; //text color
  ColorRGB color2; //background or shadow color
  int style; //see text.cpp for an explanation of the styles (background, bold, italic, shadow, ...)
  Font* font; //0 is the default built in font, other fonts can be loaded from color images
  int charSpacing; //how many pixels between two characters (increases width), can be negative (letters will be drawn on top of each other)
  int lineSpacing; //how many pixels between two lines (increased height), can be negative
  
  Markup(const ColorRGB& color1, const ColorRGB& color2 = RGB_Black, int style = 0, Font* font = &builtInFont8x8, int charSpacing = 0, int lineSpacing = 1);
  
  int getWidth() const;
  int getHeight() const;
  
  Markup()
  {
    font = &builtInFont8x8;
    color1 = RGB_White;
    color2 = RGB_Black;
    style = 0;
    charSpacing = 0;
    lineSpacing = 1;
  }
};

Markup operator/(Markup ts, int a);

#define TS_White Markup(RGB_White, RGB_Invisible, 0, &builtInFont8x8) //default white text
#define TS_Black Markup(RGB_Black, RGB_Invisible, 0, &builtInFont8x8) //default black text
#define TS_W Markup(RGB_White, RGB_Invisible, 0, &builtInFont8x8) //shortcut for default white text
#define TS_B Markup(RGB_Black, RGB_Invisible, 0, &builtInFont8x8) //shortcut for default black text
#define TS_BG Markup(RGB_White, RGB_Black, 1, &builtInFont8x8) //default white text on black background
#define TS_Shadow Markup(RGB_White, RGB_Black, 18, &builtInFont8x8) //white text with black shadow bottom right
#define TS_Red Markup(RGB_Red, RGB_Black, 0, &builtInFont8x8) //default red text
#define TS_Green Markup(RGB_Green, RGB_Black, 0, &builtInFont8x8) //default green text
#define TS_Grey Markup(RGB_Grey, RGB_Black, 0, &builtInFont8x8) //default grey text
#define TS_Blue Markup(RGB_Blue, RGB_Black, 0, &builtInFont8x8) //default blue text
#define TS_RShadow Markup(RGB_Red, RGB_Black, 18, &builtInFont8x8) //default red text with shadow
#define TS_Bold Markup(RGB_White, RGB_Black, 512, &builtInFont8x8) //default white text, bold
#define TS_BBold Markup(RGB_Black, RGB_Black, 512, &builtInFont8x8) //default black text, bold
#define TS_Italic Markup(RGB_White, RGB_Black, 1024, &builtInFont8x8) //default white text, italic
#define TS_BItalic Markup(RGB_Black, RGB_Black, 1024, &builtInFont8x8) //default black text, italic
#define TS_Alpha Markup(RGBA_White(128), RGBA_Black(128), 0, &builtInFont8x8) //translucent white text
#define TS_Rainbow Markup(RGB_Magenta, RGB_Yellow, 18, &builtInFont8x8) //colorful text
#define TS_Yellow Markup(RGB_Yellow, RGB_Black, 0, &builtInFont8x8)
#define TS_Olive Markup(RGB_Olive, RGB_Black, 0, &builtInFont8x8)
#define TS_Maroon Markup(RGB_Maroon, RGB_Black, 0, &builtInFont8x8)
#define TS_Font1 Markup(RGB_White, RGB_Black, 0, &font[1]) //the first bitmap font
#define TS_Grey Markup(RGB_Grey, RGB_Black, 0, &builtInFont8x8)

#define TS_White6  Markup(RGB_White, RGB_Black, 0, &builtInFont6x6) //default white text
#define TS_Black6  Markup(RGB_Black, RGB_White, 0, &builtInFont6x6) //default black text
#define TS_W6  Markup(RGB_White, RGB_Black, 0, &builtInFont6x6) //shortcut for default white text
#define TS_B6  Markup(RGB_Black, RGB_White, 0, &builtInFont6x6) //shortcut for default black text
#define TS_BG6  Markup(RGB_White, RGB_Black, 1, &builtInFont6x6) //default white text on black background
#define TS_Shadow6  Markup(RGB_White, RGB_Black, 18, &builtInFont6x6) //white text with black shadow bottom right
#define TS_Red6  Markup(RGB_Red, RGB_Black, 0, &builtInFont6x6) //default red text
#define TS_Green6  Markup(RGB_Green, RGB_Black, 0, &builtInFont6x6) //default green text
#define TS_Grey6  Markup(RGB_Grey, RGB_Black, 0, &builtInFont6x6) //default grey text
#define TS_Blue6  Markup(RGB_Blue, RGB_Black, 0, &builtInFont6x6) //default blue text
#define TS_RShadow6  Markup(RGB_Red, RGB_Black, 18, &builtInFont6x6) //default red text with shadow
#define TS_Bold6  Markup(RGB_White, RGB_Black, 512, &builtInFont6x6) //default white text, bold
#define TS_BBold6  Markup(RGB_Black, RGB_Black, 512, &builtInFont6x6) //default black text, bold
#define TS_Italic6  Markup(RGB_White, RGB_Black, 1024, &builtInFont6x6) //default white text, italic
#define TS_BItalic6  Markup(RGB_Black, RGB_Black, 1024, &builtInFont6x6) //default black text, italic
#define TS_Alpha6  Markup(RGBA_White(128), RGBA_Black(128), 0, &builtInFont6x6) //translucent white text
#define TS_Rainbow6  Markup(RGB_Magenta, RGB_Yellow, 18, &builtInFont6x6) //colorful text
#define TS_Yellow6  Markup(RGB_Yellow, RGB_Black, 0, &builtInFont6x6)
#define TS_Olive6  Markup(RGB_Olive, RGB_Black, 0, &builtInFont6x6)
#define TS_Maroon6  Markup(RGB_Maroon, RGB_Black, 0, &builtInFont6x6)
#define TS_Font16  Markup(RGB_White, RGB_Black, 0, &font[1]) //the first bitmap font
#define TS_Grey6  Markup(RGB_Grey, RGB_Black, 0, &builtInFont6x6)

#define TS_White4  Markup(RGB_White, RGB_Black, 0, &builtInFont4x4) //default white text
#define TS_Black4  Markup(RGB_Black, RGB_White, 0, &builtInFont4x4) //default black text
#define TS_W4  Markup(RGB_White, RGB_Black, 0, &builtInFont4x4) //shortcut for default white text
#define TS_B4  Markup(RGB_Black, RGB_White, 0, &builtInFont4x4) //shortcut for default black text
#define TS_BG4  Markup(RGB_White, RGB_Black, 1, &builtInFont4x4) //default white text on black background
#define TS_Shadow4  Markup(RGB_White, RGB_Black, 18, &builtInFont4x4) //white text with black shadow bottom right
#define TS_Red4  Markup(RGB_Red, RGB_Black, 0, &builtInFont4x4) //default red text
#define TS_Green4  Markup(RGB_Green, RGB_Black, 0, &builtInFont4x4) //default green text
#define TS_Grey4  Markup(RGB_Grey, RGB_Black, 0, &builtInFont4x4) //default grey text
#define TS_Blue4  Markup(RGB_Blue, RGB_Black, 0, &builtInFont4x4) //default blue text
#define TS_RShadow4  Markup(RGB_Red, RGB_Black, 18, &builtInFont4x4) //default red text with shadow
#define TS_Bold4  Markup(RGB_White, RGB_Black, 512, &builtInFont4x4) //default white text, bold
#define TS_BBold4  Markup(RGB_Black, RGB_Black, 512, &builtInFont4x4) //default black text, bold
#define TS_Italic4  Markup(RGB_White, RGB_Black, 1024, &builtInFont4x4) //default white text, italic
#define TS_BItalic4  Markup(RGB_Black, RGB_Black, 1024, &builtInFont4x4) //default black text, italic
#define TS_Alpha4  Markup(RGBA_White(128), RGBA_Black(128), 0, &builtInFont4x4) //translucent white text
#define TS_Rainbow4  Markup(RGB_Magenta, RGB_Yellow, 18, &builtInFont4x4) //colorful text
#define TS_Yellow4  Markup(RGB_Yellow, RGB_Black, 0, &builtInFont4x4)
#define TS_Olive4  Markup(RGB_Olive, RGB_Black, 0, &builtInFont4x4)
#define TS_Maroon4  Markup(RGB_Maroon, RGB_Black, 0, &builtInFont4x4)
#define TS_Font14  Markup(RGB_White, RGB_Black, 0, &font[1]) //the first bitmap font
#define TS_Grey4  Markup(RGB_Grey, RGB_Black, 0, &builtInFont4x4)

////////////////////////////////////////////////////////////////////////////////
//TEXT FUNCTIONS////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void drawLetter(unsigned char n, int x, int y, const Markup& markup);

//internally used to print a string
int printString(std::string text, int x = 0, int y = 0, const Markup& markup = TS_W, unsigned long forceLength=0);
//internally used to print a text
int printText(std::string text, int x = 0, int y = 0, const Markup& markup = TS_W, unsigned long forceLength=0);

//print something (string, int, float, ...)
template<typename T> int print(T val, int x = 0, int y = 0, const Markup& markup = TS_W, bool useNewline = false, unsigned long forceLength=0)
{
  std::string text = valtostr(val);
  if(useNewline) return printText(text, x, y, markup, forceLength);
  else return printString(text, x, y, markup, forceLength);
}

void addTexts(std::string  text1, int size1, char * text2, int size2, char * text3, int size3);
void printCentered(std::string text, int x = 0, int y = 0, const Markup& markup = TS_W, unsigned long forceLength=0);

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

///////////////////////////////////////////////////////////////////////////////
//FORMATTED TEXT///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
This is just plain simple text, but it has its own print function and will
print itself in a formatted way: there are special symbols with special effects
such as color and newline symbols
The character for special symbols is #
All other 255 extended ascii characters will be printed as-is
The following codes are available:

#n: newline
##: print a single #
#': shortcut to print a "
#c: color (follow the #c symbols by 8 hex digits for RGBA)
#b: background color (follow the #b symbols by 8 hex digits for RGBA)
#!: reset markup to original one (all #c, #b, ... codes reset)
#?: print any of the 256 glyphs (follow the #? symbols by 2 hex digits for 8-bit glyph code)

etc.., see the implementation for possible more codes

*/
int printFormatted(std::string text, int x = 0, int y = 0, const Markup& markup = TS_W);
int printFormattedM(std::string text, int x, int y, Markup& markup, const Markup& originalMarkup = TS_W); //this once changes the markup

int getFormattedTextAttrSize(char c); //give the char after the #, and it returns how much symbols follow after that
int getFormattedTextSymbolPrintSize(char c); //give the char after the #, and it returns how much symbols will be printed

void getFormattedTextSize(std::string text, int& xsize, int& ysize, const Markup& markup = TS_W);

void getFormattedMarkup(std::string text, Markup& markup, const Markup& originalMarkup = TS_W); //this one calculates which markup you'll get after this text

void initBuiltInFontTextures();

} //namespace lpi

#endif //text_h