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

#include "lpi_text_drawer_int.h"
#include "lpi_draw2dgl.h"
#include "lpi_texture_gl.h"

namespace lpi
{

/*
A InternalTextDrawer with a Draw2DGL is capeable on its own to draw text, but,
TextDrawerGL is a bit more optimized
*/
class TextDrawerGL : public ITextDrawer
{
  Drawer2DGL* drawer;
  InternalGlyphs glyphs;
  
  const InternalGlyphs::Glyphs* getGlyphsForFont(const Font& font) const;
  
  public: //todo: make more things private and remove many old things
  
    TextDrawerGL(const TextureFactoryGL& factory, Drawer2DGL* drawer);
    
    virtual void drawText(const std::string& text, int x, int y, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP));
    virtual void calcTextRectSize(int& w, int& h, const std::string& text, const Font& font) const;
    virtual size_t calcTextPosToChar(int x, int y, const std::string& text, const Font& font, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const;
    virtual void calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const;
    
  private:

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
};

} // namespace lpi
