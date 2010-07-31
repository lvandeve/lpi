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

#include "lpi_draw2d.h"

#include <vector>

namespace lpi
{

class InternalTextDrawer;

/*
ADrawer2DBuffer: generic, works on any unsigned char* buffer
can't be instantiated, because it has no functions to set the buffer (that's
what the sub classes Drawer2DBuffer, Drawer2DTexture, etc... are for)
*/
class ADrawer2DBuffer : public ADrawer2D //draws on a buffer
{
  protected:
    unsigned char* buffer; //not memory managed by this, instead you can set any external buffer with setBuffer
    //w and h are width and height of the buffer, not necessarily the same as getWidth and getHeight return
    size_t w;
    size_t h;
    
    bool texture_alpha_as_opacity;
    bool color_alpha_as_opacity;
    double extra_opacity;

  public:
    
    struct Clip
    {
      int x0;
      int y0;
      int x1;
      int y1;
      
      void fit(int ex0, int ey0, int ex1, int ey1)
      {
        if(x0 < ex0) x0 = ex0;
        if(y0 < ey0) y0 = ey0;
        if(x1 < ex0) x1 = ex0;
        if(y1 < ey0) y1 = ey0;
        if(x0 > ex1) x0 = ex1;
        if(y0 > ey1) y0 = ey1;
        if(x1 > ex1) x1 = ex1;
        if(y1 > ey1) y1 = ey1;
      }
      
      void fit(int w, int h)
      {
        if(x0 > w) x0 = w;
        if(y0 > h) y0 = h;
        if(x1 > w) x1 = w;
        if(y1 > h) y1 = h;
      }
    };
    
  protected:
    
    //current clip area
    Clip clip;
    //clip stack
    std::vector<Clip> clipstack;
    
  protected:
  
    void setBufferInternal(unsigned char* buffer, size_t w, size_t h)
    {
      this->buffer = buffer;
      this->w = w;
      this->h = h;
      clip.x0 = 0;
      clip.y0 = 0;
      clip.x1 = w;
      clip.y1 = h;
    }
    
    ADrawer2DBuffer();
    virtual ~ADrawer2DBuffer();
    
  public:
  
    virtual void frameStart(){};
    virtual void frameEnd(){};

    virtual size_t getWidth() = 0;
    virtual size_t getHeight() = 0;
    
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
    
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSizedRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureGradient(const ITexture* texture, int x, int y
                                   , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11);
    virtual void drawTextureRepeatedGradient(const ITexture* texture, int x0, int y0, int x1, int y1
                                           , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11);

    /*
    This sets whether you want the alpha channel of textures to be treated as opacity when drawing, or as literal.
    When treated as opacity, the shape is drawn over the existing buffer.
    When treated as literal, the existing pixels are completely replaced by the new values, with the literal alpha value.
    */
    void setTextureAlphaAsOpacity(bool set);
    void setColorAlphaAsOpacity(bool set);

    /*
    Works only for a few things, currently only for the texture drawing, rest is to do
    */
    void setExtraOpacity(double opacity);

};

class Drawer2DBuffer : public ADrawer2DBuffer
{
  public:
    Drawer2DBuffer(){}
    
    virtual size_t getWidth() { return w; }
    virtual size_t getHeight() { return h; }
    
    void setBuffer(unsigned char* buffer, size_t w, size_t h)
    {
      setBufferInternal(buffer, w, h);
    }
};


/*
Drawer2DTexture: is an Drawer2DBuffer that uses the buffer of a texture as buffer
*/
class Drawer2DTexture : public ADrawer2DBuffer
{
  protected:
  
    size_t u;
    size_t v;
  
  public:
    Drawer2DTexture(){}
    Drawer2DTexture(ITexture* texture){setTexture(texture);}

    
    virtual size_t getWidth() { return u; }
    virtual size_t getHeight() { return v; }
    
    void setTexture(ITexture* texture)
    {
      setBufferInternal(texture->getBuffer(), texture->getU2(), texture->getV2());
      w = texture->getU2();
      h = texture->getV2();
    }
};

} //end of namespace lpi
