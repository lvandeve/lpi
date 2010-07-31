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

//2D drawing with opengl

#pragma once


#include <vector>
#include <cmath>

#include "lpi_color.h"
#include "lpi_texture.h"
#include "lpi_draw2d.h"
#include "lpi_screen_gl.h"

namespace lpi
{

class InternalTextDrawer;

class Drawer2DGL : public ADrawer2D
{
  private:
    ScreenGL* screen;
    
  private:
    void recursive_bezier(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, int n);
    void drawLineInternal(int x0, int y0, int x1, int y1); //doesn't call "prepareDraw", to be used by other things that draw multiple lines
    
  public:
  
    void prepareDrawUntextured(bool filledGeometry);
    void prepareDrawTextured();

  public:
    Drawer2DGL(ScreenGL* screen);
    ~Drawer2DGL();
    
    virtual void frameStart();
    virtual void frameEnd();
    
    virtual size_t getWidth();
    virtual size_t getHeight();
    
    virtual void pushScissor(int x0, int y0, int x1, int y1);
    virtual void pushSmallestScissor(int x0, int y0, int x1, int y1); //the result will be smaller than the given coordinates and the last active scissor
    virtual void popScissor(); //pops the last set scissor, bringing the previous one back (it works like a stack, "set" pushes, "reset" pops)
    
    virtual void drawPoint(int x, int y, const ColorRGB& color);
    virtual void drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color);
    virtual void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color);
    
    virtual void drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled);
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled);
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled);
    virtual void drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled);
    virtual void drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled);
    
    virtual void drawGradientRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    virtual void drawGradientTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2);
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    
    virtual bool supportsTexture(ITexture* texture);
    virtual ITexture* createTexture() const;
    virtual ITexture* createTexture(ITexture* texture) const;
    
    /*
    Note: while this drawer uses OpenGL, the intention for the 2D drawer is that when
    drawing a texture sized, that NO filtering is done on the pixels (not blurred). For
    that, create a different drawer that doesn't use the IDrawer2D interface.
    */
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSizedRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureGradient(const ITexture* texture, int x, int y
                                   , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11);
    virtual void drawTextureRepeatedGradient(const ITexture* texture, int x0, int y0, int x1, int y1
                                           , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11);

    
  public:
  
    //todo: these functions are not in a higher interface yet
    //todo: move these into IDrawer2D or somewhere else or remove some?
    void gradientLine(int x0, int y0, int x1, int y1, const ColorRGB& color1, const ColorRGB& color2);
    void drawGradientDisk(int x, int y, double radius, const ColorRGB& color1, const ColorRGB& color2);
    void drawGradientEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color1, const ColorRGB& color2);
    
    
  public:
    ScreenGL* getScreen() { return screen; }
};




} //end of namespace lpi
