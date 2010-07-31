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

GUIPartMod::GUIPartMod()
: inactive(false)
, mouseover(false)
, mousedown(false)
{
}

GUIPartMod::GUIPartMod(bool inactive)
: inactive(inactive)
, mouseover(false)
, mousedown(false)
{
}

GUIPartMod::GUIPartMod(bool mouseover, bool mousedown)
: inactive(false)
, mouseover(mouseover)
, mousedown(mousedown)
{
}

GUIPartMod::GUIPartMod(bool mouseover, bool mousedown, bool inactive)
: inactive(inactive)
, mouseover(mouseover)
, mousedown(mousedown)
{
}

void AGUIDrawer::frameStart()
{
  getDrawer().frameStart();
}

void AGUIDrawer::frameEnd()
{
  getDrawer().frameEnd();
}

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

void AGUIDrawer::drawGradientRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  getDrawer().drawGradientRectangle(x0, y0, x1, y1, color0, color1, color2, color3);
}

void AGUIDrawer::drawGradientTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2)
{
  getDrawer().drawGradientTriangle(x0, y0, x1, y1, x2, y2, color0, color1, color2);
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

ITexture* AGUIDrawer::createTexture() const
{
  return getDrawer().createTexture();
}

ITexture* AGUIDrawer::createTexture(ITexture* texture) const
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

void AGUIDrawer::drawTextureSizedRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  getDrawer().drawTextureSizedRepeated(texture, x0, y0, x1, y1, sizex, sizey, colorMod);
}

void AGUIDrawer::drawTextureGradient(const ITexture* texture, int x, int y
                                        , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11)
{
  getDrawer().drawTextureGradient(texture, x, y, color00, color01, color10, color11);
}

void AGUIDrawer::drawTextureRepeatedGradient(const ITexture* texture, int x0, int y0, int x1, int y1
                                                , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11)
{
  getDrawer().drawTextureRepeatedGradient(texture, x0, y0, x1, y1, color00, color01, color10, color11);
}


void AGUIDrawer::calcTextRectSize(int& w, int& h, const std::string& text, const Font& font) const
{
  return getTextDrawer().calcTextRectSize(w, h, text, font);
}

size_t AGUIDrawer::calcTextPosToChar(int x, int y, const std::string& text, const Font& font, const TextAlign& align) const
{
  return getTextDrawer().calcTextPosToChar(x, y, text, font, align);
}

void AGUIDrawer::calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font, const TextAlign& align) const
{
  getTextDrawer().calcTextCharToPos(x, y, index, text, font, align);
}


void AGUIDrawer::drawText(const std::string& text, int x, int y, const Font& font, const TextAlign& align)
{
  getTextDrawer().drawText(text, x, y, font, align);
}


void AGUIDrawer::drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  getGUIPartDrawer().drawGUIPart(part, x0, y0, x1, y1, mod);
}

void AGUIDrawer::drawGUIPartColor(GUIPart part, const ColorRGB& color, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  getGUIPartDrawer().drawGUIPartColor(part, color, x0, y0, x1, y1, mod);
}

void AGUIDrawer::drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  getGUIPartDrawer().drawGUIPartText(part, text, x0, y0, x1, y1, mod);
}

size_t AGUIDrawer::getGUIPartSizeX(GUIPart part) const
{
  return getGUIPartDrawer().getGUIPartSizeX(part);
}

size_t AGUIDrawer::getGUIPartSizeY(GUIPart part) const
{
  return getGUIPartDrawer().getGUIPartSizeY(part);
}

void AGUIDrawer::getGUIPartTextSize(int& w, int& h, GUIPart part, const std::string& text) const
{
  return getGUIPartDrawer().getGUIPartTextSize(w, h, part, text);
}


void AGUIDrawer::createIcon(ITexture& texture, GUIIcon icon, int size) const
{
  getGUIPartDrawer().createIcon(texture, icon, size);
}

} //namespace gui
} //namespace lpi
