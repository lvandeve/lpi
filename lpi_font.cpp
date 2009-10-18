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

#include "lpi_font.h"

namespace lpi
{

Font::Font()
: typeface("lpi8")
, bold(false)
/*, italic(false)*/
/*, serif(false)*/
/*, monospaced(false)*/
/*, width(8)*/
/*, height(8)*/
, spacing(0)
, linespacing(0)
, color(RGB_White)
, shadow(false)
, shadowColor(RGB_Black)
{
}

Font::Font(const ColorRGB& color)
: typeface("lpi8")
, bold(false)
/*, italic(false)*/
/*, serif(false)*/
/*, monospaced(false)*/
/*, width(8)*/
/*, height(8)*/
, spacing(0)
, linespacing(0)
, color(color)
, shadow(false)
, shadowColor(RGB_Black)
{
}

Font::Font(const std::string& typeface, const ColorRGB& color)
: typeface(typeface)
, bold(false)
/*, italic(false)*/
/*, serif(false)*/
/*, monospaced(false)*/
/*, width(8)*/
/*, height(8)*/
, spacing(0)
, linespacing(0)
, color(color)
, shadow(false)
, shadowColor(RGB_Black)
{
}

namespace
{
  Font createFont(const std::string& typeface, const ColorRGB& color)
  {
    Font font;
    font.typeface = typeface;
    font.color = color;
    return font;
  }
  
  Font createShadowFont(const std::string& typeface, const ColorRGB& color, const ColorRGB& shadowColor)
  {
    Font font;
    font.typeface = typeface;
    font.color = color;
    font.shadowColor = shadowColor;
    font.shadow = true;
    return font;
  }
}

const Font FONT_Default(RGB_Black);
const Font FONT_Black(RGB_Black);
const Font FONT_White(RGB_White);
const Font FONT_Shadow = createShadowFont("lpi8", RGB_White, RGB_Black);
const Font FONT_ShadowInverted = createShadowFont("lpi8", RGB_Black, RGB_White);
const Font FONT_Grey(RGB_Grey);
const Font FONT_Gray(RGB_Gray);
const Font FONT_Red(RGB_Red);
const Font FONT_Green(RGB_Green);
const Font FONT_Blue(RGB_Blue);

const Font FONT_Default6(createFont("lpi6", RGB_Black));
const Font FONT_Black6(createFont("lpi6", RGB_Black));
const Font FONT_White6(createFont("lpi6", RGB_White));
const Font FONT_Shadow6 = createShadowFont("lpi6", RGB_White, RGB_Black);
const Font FONT_ShadowInverted6 = createShadowFont("lpi6", RGB_Black, RGB_White);
const Font FONT_Grey6(createFont("lpi6", RGB_Grey));
const Font FONT_Gray6(createFont("lpi6", RGB_Gray));
const Font FONT_Red6(createFont("lpi6", RGB_Red));
const Font FONT_Green6(createFont("lpi6", RGB_Green));
const Font FONT_Blue6(createFont("lpi6", RGB_Blue));

const Font FONT_Default4(createFont("lpi4", RGB_Black));
const Font FONT_Black4(createFont("lpi4", RGB_Black));
const Font FONT_White4(createFont("lpi4", RGB_White));
const Font FONT_Shadow4 = createShadowFont("lpi4", RGB_White, RGB_Black);
const Font FONT_ShadowInverted4 = createShadowFont("lpi4", RGB_Black, RGB_White);
const Font FONT_Grey4(createFont("lpi4", RGB_Grey));
const Font FONT_Gray4(createFont("lpi4", RGB_Gray));
const Font FONT_Red4(createFont("lpi4", RGB_Red));
const Font FONT_Green4(createFont("lpi4", RGB_Green));
const Font FONT_Blue4(createFont("lpi4", RGB_Blue));





} // namespace lpi