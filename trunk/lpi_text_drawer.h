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

#pragma once

#include "lpi_font.h"

#include <sstream>

namespace lpi
{

  enum HTextAlign
  {
    HA_LEFT,
    HA_CENTER,
    HA_RIGHT
  };
  
  enum VTextAlign
  {
    VA_TOP,
    VA_CENTER,
    VA_BOTTOM
  };
  
  struct TextAlign
  {
    TextAlign(HTextAlign halign, VTextAlign valign) : halign(halign), valign(valign) {}
    HTextAlign halign;
    VTextAlign valign;
  };
  
  static const TextAlign ALIGN_TopLeft(HA_LEFT, VA_TOP);
  static const TextAlign ALIGN_00(HA_LEFT, VA_TOP);
  static const TextAlign ALIGN_05(HA_LEFT, VA_CENTER);
  static const TextAlign ALIGN_01(HA_LEFT, VA_BOTTOM);
  static const TextAlign ALIGN_50(HA_CENTER, VA_TOP);
  static const TextAlign ALIGN_55(HA_CENTER, VA_CENTER);
  static const TextAlign ALIGN_51(HA_CENTER, VA_BOTTOM);
  static const TextAlign ALIGN_10(HA_RIGHT, VA_TOP);
  static const TextAlign ALIGN_15(HA_RIGHT, VA_CENTER);
  static const TextAlign ALIGN_11(HA_RIGHT, VA_BOTTOM);


  class ITextDrawer
  {
  
    public:
    
    //TODO: make functions that calculate a size, const
    
    
    /*
    drawText: the most basic text printing function.
    x and y are the coordinates of the rectangle in which the text will be. For
    HA_LEFT, VA_TOP alignment, this is the top left position of the rectangle,
    for HA_CENTER, VA_CENTER alignment, this is the center of the rectangle, 
    and so on...
    */
    virtual void drawText(const std::string& text, int x, int y, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) = 0;
    
    /*
    calcTextRectSize: get the size of the rectangle that the text drawn with drawText will take.
    There are no alignment parameters because it has no effect on the rectangle size, but note that
    alignment will affect the position of the rectangle.
    */
    virtual void calcTextRectSize(int& w, int& h, const std::string& text, const Font& font = FONT_Default) const = 0;
    
    /*
    calcTextPosToChar: gives the index of the character in the string text, that will be drawn at
    location x, y, the position relative the the coordinates you will give to the drawText function.
    This can be used to go from mouse position to the index of the cursor, or for line-breaking
    strategies of text in a limited area.
    The index returned is a char in the string which will always be the first char of
    a glyph (if a glyph exists out of multiple char's, like can happen with UTF-8).
    */
    virtual size_t calcTextPosToChar(int x, int y, const std::string& text, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const = 0;
    
    /*
    calcTextCharToPos: this does the opposite of calcTextPosToChar. Given the index of the character in the text,
    it returns the relative position to the (x, y) of the drawTextFunction of that character's top left corner.
    x and y are output parameters.
    */
    virtual void calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const = 0;
    
    
    //templatized version for convenience. Different name than drawText because otherwise overriding drawText hides this function.
    template<typename T>
    void print(const T& val, int x, int y, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP))
    {
      std::ostringstream sstream;
      sstream << val;
      drawText(sstream.str(), x, y, font, align);
    }
    
    /*
    TODO: drawing text in a limited rectangular area with word wrapping. To do that, don't add functions
    to IDrawer2D, but instead make a class which has a line breaking strategy, and which uses the
    functions like calcTextRectSize and so on to determine where it has to break the text. It can have
    strategies like breaking between words, between letters, ... It should already itself break
    the text into multiple strings (one per line) and then do the line breaking per substring (faster
    than asking these parameters for the complete string all the time)
    */
    
    //TODO: interface for caching rendered text in objects (for efficiency, depends on the implementation of the drawer)
  };
    
} // namespace lpi
