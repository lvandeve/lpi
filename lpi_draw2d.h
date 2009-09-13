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

#ifndef LPI_DRAW2D_H_INCLUDED
#define LPI_DRAW2D_H_INCLUDED

#include "lpi_color.h"
#include "lpi_texture.h"

#include <vector>
#include <string>

#include <iostream>

namespace lpi
{


//helper functions for 2D graphics
bool bezier_nearly_flat(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);
bool clipLine(int x1, int y1, int x2, int y2, int& x3, int& y3, int& x4, int& y4, int left, int top, int right, int bottom);


class IDrawer2D
{
  public:
    virtual ~IDrawer2D(){};
    
    ///size of drawable coordinates, goes from (0,0) to (width, height)
    
    virtual size_t getWidth() = 0;
    virtual size_t getHeight() = 0;
    
    ///scissor, limited drawing area
    
    virtual void setScissor(int x0, int y0, int x1, int y1) = 0;
    virtual void setSmallestScissor(int x0, int y0, int x1, int y1) = 0; //the result will be smaller than the given coordinates and the last active scissor
    virtual void resetScissor() = 0; //pops the last set scissor, bringing the previous one back (it works like a stack, "set" pushes, "reset" pops)
  
    ///thin shapes
    
    virtual void drawPoint(int x, int y, const ColorRGB& color) = 0; //SLOW! If you need to draw lots of pixels, use bit blit or texture
    virtual void drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color) = 0;
    virtual void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color) = 0;
    //todo: polyline
    //todo: polybezier
    //todo: arc
    
    ///filled shapes (or border if not filled)
    
    virtual void drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled) = 0;
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled) = 0;
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled) = 0;
    virtual void drawEllipse(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled) = 0;
    virtual void drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled) = 0;
    virtual void drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled) = 0;
    //todo: polygon
    //todo: rounded rectangle
    //todo: pie
    //todo: chord (shape is e.g. "D", "|)", ...)
    
    
    ///gradient shapes
    
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3) = 0;
    //todo: gradientLine, gradientTriangle, gradientCircle and gradientEllipse
    
    ///textures
    
    //you can only draw textures that have the correct subtype of ITexture. therefore, you need to use the functions below to get one first before starting to draw
    virtual bool supportsTexture(ITexture* texture) = 0; //if true, you can use it. If false, first use createTexture(texture) and then it'll be drawable by this drawer!
    virtual ITexture* createTexture() = 0; //creates a new texture of the type this drawer implementation supports best
    virtual ITexture* createTexture(ITexture* texture) = 0; //creates a new texture from the given one, the type this drawer implementation supports best. May ruin/destroy/clear the input texture!!
    virtual void convertTextureIfNeeded(ITexture*& texture) = 0; //Utility function to make sure your texture is of the supported type. IMPORTANT!!!! Your ITexture* pointer must be created with new and may be deleted! Instead (if needed) your pointer is made to point to another texture created with new (Note how the pointer is given by reference to allow this).
    
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureCentered(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureSizedCentered(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White) = 0;
    
    ////"matrix" is 2x2 matrix given as an array of 4 doubles: topleft element, topright element, bottomleft element, bottomright element. The matrix has column vectors for doing the transformation (the OpenGL convention, not the Direct3D convention).
    //virtual void drawTextureTransformed(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod = RGB_White) = 0; //transformed around the top left corner of the texture
    //virtual void drawTextureTransformedCentered(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod = RGB_White) = 0; //transformed around the center point of the texture
    
    //todo: add "bit blit"?
    
    ///text
    
    //TODO: draw text, draw text centered/aligned, cache rendered text in objects, ...
    
};

class ADrawer2D : public IDrawer2D //abstract IDrawer2D already implements some functions in terms of some others
{
  public:
  
    virtual void drawEllipse(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled);
    virtual void drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled);
    
    virtual void convertTextureIfNeeded(ITexture*& texture);

    virtual void drawTextureCentered(const ITexture* texture, int x, int y, const ColorRGB& colorMod);
    virtual void drawTextureSizedCentered(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod);
    //virtual void drawTextureTransformedCentered(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod);
};


//OLD interface, todo: remove
void drawLine(unsigned char* buffer, int w, int h, int x1, int y1, int x2, int y2, const ColorRGB& color);
void drawLine(unsigned char* buffer, int w, int h, int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1, int clipy1, int clipx2, int clipy2);
void horLine(unsigned char* buffer, int w, int h, int y, int x1, int x2, const ColorRGB& color);
void verLine(unsigned char* buffer, int w, int h, int x, int y1, int y2, const ColorRGB& color);
void drawCircle(unsigned char* buffer, int w, int h, int x, int y, int radius, const ColorRGB& color);
void drawDisk(unsigned char* buffer, int buffer_w, int buffer_h, int xc, int yc, int radius, const ColorRGB& color);
void drawEllipse(unsigned char* buffer, int w, int h, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color);
void drawFilledEllipse(unsigned char* buffer, int w, int h, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color);
void drawBezier(unsigned char* buffer, int w, int h, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color);
//drawPixel checks if the pixel is inside the buffer (it's simply not drawn if outside)
void drawPixel(unsigned char* buffer, int w, int h, int x, int y, const ColorRGB& color);
//pset does NOT check if the pixel is actually in the buffer (risk for out of bounds but much faster)
void pset(unsigned char* buffer, int w, int x, int y, const ColorRGB& color);


} //end of namespace lpi

#endif
