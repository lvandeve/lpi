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

//2D drawing with opengl

#ifndef LPI_DRAW2DGL_H_INCLUDED
#define LPI_DRAW2DGL_H_INCLUDED


#include <vector>
#include <cmath>

#include "lpi_color.h"
#include "lpi_texture.h"
#include "lpi_draw2d.h"

namespace lpi
{

class Drawer2DGL : public ADrawer2D
{
  public:
    virtual size_t getWidth();
    virtual size_t getHeight();
    
    virtual void setScissor(int x0, int y0, int x1, int y1);
    virtual void setSmallestScissor(int x0, int y0, int x1, int y1); //the result will be smaller than the given coordinates and the last active scissor
    virtual void resetScissor(); //pops the last set scissor, bringing the previous one back (it works like a stack, "set" pushes, "reset" pops)
    
    virtual void drawPoint(int x, int y, const ColorRGB& color);
    virtual void drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color);
    virtual void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color);
    
    virtual void drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled);
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled);
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled);
    virtual void drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled);
    virtual void drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled);
    
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    
    virtual bool supportsTexture(ITexture* texture);
    virtual ITexture* createTexture();
    virtual ITexture* createTexture(ITexture* texture);
    
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White);
};



//NOTE: as much as possible, the "to" pixel itself is not included, e.g. a rectangle from 0-100 is 100 pixels, not 101 pixels in size (0, ..., 99 are filled, 100 not)
void pset(int x, int y, const ColorRGB& color);
void drawPoint(int x, int y, const ColorRGB& color); //same as pset but with other name
void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1, int clipy1, int clipx2, int clipy2);
void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color);
void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color);
void drawRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color = RGB_White, bool filled = true);
void drawCircle(int x, int y, double radius, const ColorRGB& color);
void drawDisk(int x, int y, double radius, const ColorRGB& color);
void drawEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color);
void drawFilledEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color);
void gradientLine(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2);
void gradientRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4);
void drawGradientDisk(int x, int y, double radius, const ColorRGB& color1, const ColorRGB& color2);
void drawGradientEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color1, const ColorRGB& color2);
void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled);
void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled);
void gradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
} //end of namespace lpi

#endif
