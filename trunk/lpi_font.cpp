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



} // namespace lpi
