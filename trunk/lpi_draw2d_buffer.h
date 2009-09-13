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
    
    //current clip area
    Clip clip;
    //clip stack
    std::vector<Clip> clipstack;
    
  protected:
  
    static void pset(unsigned char* buffer, int buffer_w, int x, int y, const ColorRGB& color); //pset does NOT check if the pixel is actually in the buffer (risk for out of bounds but much faster)
    static void psetClipped(unsigned char* buffer, int buffer_w, const Clip& c, int x, int y, const ColorRGB& color);
    static void horLine(unsigned char* buffer, int buffer_w, int buffer_h, int y, int x1, int x2, const ColorRGB& color); //Fast horizontal line from (x1,y) to (x2,y), with rgb color
    static void verLine(unsigned char* buffer, int buffer_w, int buffer_h, int x, int y1, int y2, const ColorRGB& color); //Fast vertical line from (x,y1) to (x,y2), with rgb color
    static void bresenhamLine(unsigned char* buffer, int w, int x1, int y1, int x2, int y2, const ColorRGB& color); //this one does NOT check if the line is inside the buffer! (hence only width, not height, as parameter)
    static void drawLine(unsigned char* buffer, int w, Clip& clip, int x0, int y0, int x1, int y1, const ColorRGB& color);
    static void recursive_bezier(unsigned char* buffer, int w, Clip& clip, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, const ColorRGB& color, int n);
    static void drawEllipseBorder(unsigned char* buffer, int w, Clip& clip, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color);
    static void drawFilledEllipse(unsigned char* buffer, int w, int h, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color);
    static void drawCircleBorder(unsigned char* buffer, int w, Clip& clip, int cx, int cy, int radius, const ColorRGB& color);
    static void drawDisk(unsigned char* buffer, int buffer_w, int buffer_h, int xc, int yc, int radius, const ColorRGB& color);
    
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
    
  public:

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
    
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    
    virtual bool supportsTexture(ITexture* texture);
    virtual ITexture* createTexture();
    virtual ITexture* createTexture(ITexture* texture);
    
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White);

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

    
    virtual size_t getWidth() { return w; }
    virtual size_t getHeight() { return h; }
    
    void setTexture(ITexture* texture)
    {
      setBufferInternal(texture->getBuffer(), texture->getU(), texture->getV());
      u = texture->getU2();
      v = texture->getV2();
    }
};

} //end of namespace lpi
