/*
Copyright (c) 2005-2010 Lode Vandevenne
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

#include "lpi_texture.h"
#include "lpi_color.h"
#include "lpi_font.h"
#include "lpi_text_drawer.h"
#include "lpi_parse.h"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

/*
Terminology for fonts in LPI:

-Glyph: graphical symbol that can be rendered, many glyphs together can form a text in some language. So a glyph is an image, and it's part of a typeface so size, color, etc... don't matter
-Character: ASCII or UTF-8 character
-Typeface: Data out of which some fonts can be generated, these fonts will all have the same style but may have different size, color, ...
-Font Family: for LPI, this is the same as a Typeface. It's also just a bunch of fonts.
-Glyphs (plural): data for a typeface, used by typefaces, or the typeface itself, because a typeface in LPI is basically a set of glyphs
-Font: exact appearance of glyphs on screen, a font is described by giving typeface name and then all the parameters like size, color, whether it's bold, ...
-Markup: not used anymore, was used in the old LPI to do what "Font" does now

*) In old book printing, multiple fonts defined the typeface. The fonts are the data (metal sheets with
graphical symbols on them) and the typeface was an abstract collection of some of these data's together.
*) But in LPI (and generally in most computer systems I guess), it's somewhat the opposite. The data is in
the typeface (bitmap data, vector data, ...). The font is generated out of the data from a typeface.

In LPI, a font is a simple struct with some parameters like size, color and typeface name. But the typeface
data isn't externally in the LPI interface, it depends on the drawer how it draws a particular combination
of typeface name and other parameters of a Font struct. Internally this drawer will probably use some class
representing a typeface and containing all the glyph data.

*/

namespace lpi
{

class IDrawer2D;

/*
InternalGlyphs: internal lpi bitmap typefaces
Uses a texture to describe each glyph of vareous internally defined LPI typefaces (lpi8, lpi6, lpi4)
Currently not extendable to support other typefaces.
Needs an ITextureFactory to create the textures of the correct type for that what suits your need.
*/
class InternalGlyphs
{
  public:
    struct Glyphs
    {
      std::vector<ITexture*> texture; //256 images
      int height;
      int width;
      
      ~Glyphs();
    };
  
    static const int NUMFONT = 256;
    
    Glyphs glyphs8x8;
    Glyphs glyphs7x9;
    Glyphs glyphs6x6;
    Glyphs glyphs4x5;
    
    void initBuiltInFontTextures(const ITextureFactory* factory, bool allInOneBigTexture);
  
    InternalGlyphs(const ITextureFactory* factory, bool allInOneBigTexture);
    ~InternalGlyphs();
};

const std::string& getBuiltIn8x8FontTexture();
const std::string& getBuiltIn7x9FontTexture();
const std::string& getBuiltIn6x6FontTexture();
const std::string& getBuiltIn4x5FontTexture();

class InternalTextDrawer : public ITextDrawer //uses InternalGlyphs
{
  IDrawer2D* drawer;
  InternalGlyphs glyphs;
  
  void drawLetter(unsigned char n, int x, int y, const InternalGlyphs::Glyphs* glyphs, const Font& font);
  
  const InternalGlyphs::Glyphs* getGlyphsForFont(const Font& font) const;
  
  public: //todo: make more things private and remove many old things
  
  InternalTextDrawer(const ITextureFactory& factory, IDrawer2D* drawer);
  
  virtual void drawText(const std::string& text, int x, int y, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP));
  virtual void calcTextRectSize(int& w, int& h, const std::string& text, const Font& font) const;
  virtual size_t calcTextPosToChar(int x, int y, const std::string& text, const Font& font, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const;
  virtual void calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const;
  
  private:
  
  //TODO: clean this up a bit, these are functions from the old interface.

  //used to print a text with newlines
  void printText(const std::string& text, int x, int y, const Font& font, unsigned long forceLength=0);

  //print something (string, int, float, ...)
  template<typename T> void print(const T& val, int x, int y, const Font& font, unsigned long forceLength=0)
  {
    std::string text = valtostr(val);
    printText(text, x, y, font, forceLength);
  }

  template<typename T> void print(const T& val, int x, int y, const ColorRGB& color, unsigned long forceLength=0)
  {
    Font font;
    font.color = color;
    print(val, x, y, font, forceLength);
  }



  /*
  THIS IS LEGACY-LPI!!!
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

  //int printFormatted(const std::string& text, int x, int y, const Font& font);
  //int printFormattedM(const std::string&, int x, int y, Markup& markup, const Markup& originalMarkup = TS_W); //this once changes the markup

  //int getFormattedTextAttrSize(char c); //give the char after the #, and it returns how much symbols follow after that
  //int getFormattedTextSymbolPrintSize(char c); //give the char after the #, and it returns how much symbols will be printed

  //void getFormattedTextSize(const std::string& text, int& xsize, int& ysize, const Markup& markup = TS_W);

  //void getFormattedMarkup(const std::string& text, Markup& markup, const Markup& originalMarkup = TS_W); //this one calculates which markup you'll get after this text

};

} //namespace lpi

