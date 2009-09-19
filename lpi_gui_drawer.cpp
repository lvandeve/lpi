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

#include <iostream>

#include "lpi_gui_drawer.h"

namespace lpi
{
namespace gui
{

size_t AGUIDrawer::getWidth()
{
  return getDrawer().getWidth();
}

size_t AGUIDrawer::getHeight()
{
  return getDrawer().getHeight();
}

void AGUIDrawer::drawPoint(int x, int y, const ColorRGB& color)
{
  getDrawer().drawPoint(x, y, color);
}

void AGUIDrawer::drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  getDrawer().drawLine(x0, y0, x1, y1, color);
}

void AGUIDrawer::drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color)
{
  getDrawer().drawBezier(x0, y0, x1, y1, x2, y2, x3, y3, color);
}

void AGUIDrawer::drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled)
{
  getDrawer().drawRectangle(x0, y0, x1, y1, color, filled);
}

void AGUIDrawer::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  getDrawer().drawTriangle(x0, y0, x1, y1, x2, y2, color, filled);
}

void AGUIDrawer::drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled)
{
  getDrawer().drawQuad(x0, y0, x1, y1, x2, y2, x3, y3, color, filled);
}

void AGUIDrawer::drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled)
{
  getDrawer().drawEllipseCentered(x, y, radiusx, radiusy, color, filled);
}

void AGUIDrawer::drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  getDrawer().drawGradientQuad(x0, y0, x1, y1, x2, y2, x3, y3, color0, color1, color2, color3);
}

void AGUIDrawer::pushScissor(int x0, int y0, int x1, int y1)
{
  getDrawer().pushScissor(x0, y0, x1, y1);
}

void AGUIDrawer::pushSmallestScissor(int x0, int y0, int x1, int y1)
{
  getDrawer().pushSmallestScissor(x0, y0, x1, y1);
}

void AGUIDrawer::popScissor()
{
  getDrawer().popScissor();
}

bool AGUIDrawer::supportsTexture(ITexture* texture)
{
  return getDrawer().supportsTexture(texture);
}

ITexture* AGUIDrawer::createTexture()
{
  return getDrawer().createTexture();
}

ITexture* AGUIDrawer::createTexture(ITexture* texture)
{
  return getDrawer().createTexture(texture);
}
    
void AGUIDrawer::drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod)
{
  getDrawer().drawTexture(texture, x, y, colorMod);
}

void AGUIDrawer::drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  getDrawer().drawTextureSized(texture, x, y, sizex, sizey, colorMod);
}

void AGUIDrawer::drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod)
{
  getDrawer().drawTextureRepeated(texture, x0, y0, x1, y1, colorMod);
}

void AGUIDrawer::calcTextRectSize(int& w, int& h, const std::string& text, const Font& font)
{
  return getTextDrawer().calcTextRectSize(w, h, text, font);
}

size_t AGUIDrawer::calcTextPosToChar(int x, int y, const std::string& text, const Font& font, const TextAlign& align)
{
  return getTextDrawer().calcTextPosToChar(x, y, text, font, align);
}

void AGUIDrawer::calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font, const TextAlign& align)
{
  getTextDrawer().calcTextCharToPos(x, y, index, text, font, align);
}


void AGUIDrawer::drawText(const std::string& text, int x, int y, const Font& font, const TextAlign& align)
{
  getTextDrawer().drawText(text, x, y, font, align);
}

} //namespace gui
} //namespace lpi
