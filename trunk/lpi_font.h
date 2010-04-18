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

#include "lpi_color.h"
#include <string>

namespace lpi
{


/*
Font:

A Font describes how to render characters (using a typeface and parameters like size, color, ...).
Currently, LPI only supports 3 bitmap fonts, and can not apply things like italic, serifs, different sizes, ... to it,
so many parameters in Font are not actually used (and commented out for that reason for now).
But the rules of the Font struct are like this:

It checks if it can find the mentioned typeface. If the typeface is an empty string, it'll never find one.
A1) If it found the typeface:
      It will render using the glyph data from that typeface.
      It tries to apply the styles (italic, ...) and sizes as much as possible to the typeface, but
      it is not certain that it'll actually be able to make a difference. It's very possible that
      a setting like "italic", or even the size, is ignored. Depending on the data inside the typeface,
      and the renderer, it may or may not be able to use that data.
A2) If it did not find the typeface:
      It will try to find a typeface that matches the style and size as much as possible (e.g. an italic
      typeface designed for letters of height 10 pixels...). There are no guarantees at all that it will
      find something that works.
      B1) If it finds such a close-mathing typeface:
            It'll render it like described in step A1).
      B2) If doesn't find such a close-matching typeface:
            It'll just render some characters on screen that may or may not fit the style and size. But the
            characters will be readable for sure if it are ASCII characters :)

So in conclusion, how your Font struct is interpreted depends on the renderer, the available
typefaces, and the data in the available typeface if a matching one was found. Note that the
typefaces are part of the renderer, so you could say that how it's interpreted depends purely
on the renderer (the IDrawer2D implementation).

NOTE: since LPI currently has only 3 bitmap fonts, the above rules are quite irrelevant.

The 3 bitmap fonts have following typeface names:

"lpi8": 8-pixel bitmap font, monospace, not resizable, not italicable, not serifable
"lpi7": 7-pixel bitmap font, monospace, not resizable, not italicable, not serifable
"lpi6": 6-pixel bitmap font, monospace, not resizable, not italicable, not serifable
"lpi4": 4x5-pixel bitmap font, monospace, not resizable, not italicable, not serifable
*/
struct Font
{
  Font();
  Font(const ColorRGB& color);
  Font(const std::string& typeface, const ColorRGB& color);
  
  //typeface (font family name)
  std::string typeface;
  
  //typeface style
  bool bold;
  /*bool italic;*/
  /*bool serif;*/ //if false, it's "swiss"
  /*bool monospaced;*/ //if false, it's "proportional"
  
  //typeface size
  /*int width;*/ //rough width of characters in pixels
  /*int height;*/ //rough height of characters in pixels
  int spacing; //adjust horizontal space between characters (positive or negative)
  int linespacing; //adjust vertical spacing (positive or negative)
  
  //independent from shape/face
  ColorRGB color;
  
  //special effects
  bool shadow;
  ColorRGB shadowColor;
  //bool background;
  //ColorRGB backgroundColor;
};


Font createFont(const std::string& typeface, const ColorRGB& color);
Font createShadowFont(const std::string& typeface, const ColorRGB& color, const ColorRGB& shadowColor);

static const Font FONT_Default(RGB_Black);
static const Font FONT_Black(RGB_Black);
static const Font FONT_White(RGB_White);
static const Font FONT_Shadow = createShadowFont("lpi8", RGB_White, RGB_Black);
static const Font FONT_ShadowInverted = createShadowFont("lpi8", RGB_Black, RGB_White);
static const Font FONT_Grey(RGB_Grey);
static const Font FONT_Gray(RGB_Gray);
static const Font FONT_Red(RGB_Red);
static const Font FONT_Green(RGB_Green);
static const Font FONT_Blue(RGB_Blue);

static const Font FONT_Default7(createFont("lpi7", RGB_Black));
static const Font FONT_Black7(createFont("lpi7", RGB_Black));
static const Font FONT_White7(createFont("lpi7", RGB_White));
static const Font FONT_Shadow7 = createShadowFont("lpi7", RGB_White, RGB_Black);
static const Font FONT_ShadowInverted7 = createShadowFont("lpi7", RGB_Black, RGB_White);
static const Font FONT_Grey7(createFont("lpi7", RGB_Grey));
static const Font FONT_Gray7(createFont("lpi7", RGB_Gray));
static const Font FONT_Red7(createFont("lpi7", RGB_Red));
static const Font FONT_Green7(createFont("lpi7", RGB_Green));
static const Font FONT_Blue7(createFont("lpi7", RGB_Blue));

static const Font FONT_Default6(createFont("lpi6", RGB_Black));
static const Font FONT_Black6(createFont("lpi6", RGB_Black));
static const Font FONT_White6(createFont("lpi6", RGB_White));
static const Font FONT_Shadow6 = createShadowFont("lpi6", RGB_White, RGB_Black);
static const Font FONT_ShadowInverted6 = createShadowFont("lpi6", RGB_Black, RGB_White);
static const Font FONT_Grey6(createFont("lpi6", RGB_Grey));
static const Font FONT_Gray6(createFont("lpi6", RGB_Gray));
static const Font FONT_Red6(createFont("lpi6", RGB_Red));
static const Font FONT_Green6(createFont("lpi6", RGB_Green));
static const Font FONT_Blue6(createFont("lpi6", RGB_Blue));

static const Font FONT_Default4(createFont("lpi4", RGB_Black));
static const Font FONT_Black4(createFont("lpi4", RGB_Black));
static const Font FONT_White4(createFont("lpi4", RGB_White));
static const Font FONT_Shadow4 = createShadowFont("lpi4", RGB_White, RGB_Black);
static const Font FONT_ShadowInverted4 = createShadowFont("lpi4", RGB_Black, RGB_White);
static const Font FONT_Grey4(createFont("lpi4", RGB_Grey));
static const Font FONT_Gray4(createFont("lpi4", RGB_Gray));
static const Font FONT_Red4(createFont("lpi4", RGB_Red));
static const Font FONT_Green4(createFont("lpi4", RGB_Green));
static const Font FONT_Blue4(createFont("lpi4", RGB_Blue));

} // namespace lpi
