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
#include "lpi_texture.h"
#include "lpi_font.h"

#include <vector>
#include <string>

#include <iostream>
#include <sstream>

namespace lpi
{

//helper functions for 2D graphics
bool bezier_nearly_flat(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);
bool clipLine(int& ox0, int& oy0, int& ox1, int& oy1, int ix0, int iy0, int ix1, int iy1, int left, int top, int right, int bottom);
void clipRect(int& ox0, int& oy0, int& ox1, int& oy1, int ix0, int iy0, int ix1, int iy1, int left, int top, int right, int bottom);

class IDrawer2D
{
  public:
    virtual ~IDrawer2D(){};
    
    ///call these before and after drawing with this drawer during a frame (some implementations need this, others ignore it)
    
    /*
    Between a frameStart and frameEnd call, this drawer has set the state of the backend it
    uses to the state it needs. Don't change the state of the backend (e.g. OpenGL) manually
    while using the drawer during these two calls. Before frameStart and after frameEnd
    some changes are allowed (e.g. the scissor area size of OpenGL or the view matrix).
    */
    virtual void frameStart() = 0;
    virtual void frameEnd() = 0;
    
    ///size of drawable coordinates, goes from (0,0) to (width, height)
    
    virtual size_t getWidth() = 0;
    virtual size_t getHeight() = 0;
    
    ///scissor, limited drawing area
    
    virtual void pushScissor(int x0, int y0, int x1, int y1) = 0;
    virtual void pushSmallestScissor(int x0, int y0, int x1, int y1) = 0; //the result will be smaller than the given coordinates and the last active scissor
    virtual void popScissor() = 0; //pops the last set scissor, bringing the previous one back (it works like a stack)
  
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
    //drawQuad: the points must be given in counter-clockwise direction
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled) = 0;
    virtual void drawEllipse(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled) = 0;
    virtual void drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled) = 0;
    virtual void drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled) = 0;
    //todo: polygon
    //todo: rounded rectangle
    //todo: pie
    //todo: chord (shape is e.g. "D", "|)", ...)
    
    
    ///gradient shapes
    
    //drawGradientQuad: the points must be given in counter-clockwise direction
    virtual void drawGradientRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3) = 0;
    virtual void drawGradientTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2) = 0;
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3) = 0;
    //todo: gradientLine, gradientCircle and gradientEllipse
    
    ///textures
    
    //you can only draw textures that have the correct subtype of ITexture. therefore, you need to use the functions below to get one first before starting to draw
    virtual bool supportsTexture(ITexture* texture) = 0; //if true, you can use it. If false, first use createTexture(texture) and then it'll be drawable by this drawer!
    //TODO: instead of the createTexture functions here, make it return a texture factory, and put the one that takes a texture as parameter in the interface of the texture factory...
    virtual ITexture* createTexture() const = 0; //creates a new texture of the type this drawer implementation supports best
    virtual ITexture* createTexture(ITexture* texture) const = 0; //creates a new texture from the given one, the type this drawer implementation supports best. May ruin/destroy/clear the input texture!!
    virtual void convertTextureIfNeeded(ITexture*& texture) = 0; //Utility function to make sure your texture is of the supported type. IMPORTANT!!!! Your ITexture* pointer must be created with new and may be deleted! Instead (if needed) your pointer is made to point to another texture created with new (Note how the pointer is given by reference to allow this).
    
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureCentered(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureSizedCentered(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureSizedRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White) = 0;
    virtual void drawTextureGradient(const ITexture* texture, int x, int y
                                   , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11) = 0;
    virtual void drawTextureRepeatedGradient(const ITexture* texture, int x0, int y0, int x1, int y1
                                           , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11) = 0;

    ////"matrix" is 2x2 matrix given as an array of 4 doubles: topleft element, topright element, bottomleft element, bottomright element. The matrix has column vectors for doing the transformation (the OpenGL convention, not the Direct3D convention).
    //virtual void drawTextureTransformed(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod = RGB_White) = 0; //transformed around the top left corner of the texture
    //virtual void drawTextureTransformedCentered(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod = RGB_White) = 0; //transformed around the center point of the texture
    
    //todo: add "bit blit"?
    
    
    //Clear screen COMMENTED OUT because this is actually not a task of the drawer, but of that what controls the screen (and does the updating, redrawing, ...)
    //Using clear screen is something you do in OpenGL not only when drawing 2D things, but also when drawing 3D graphics to render the next screen. So it doesn't belong in IDrawer2D
    //virtual void cls(const ColorRGB& color) = 0; //give everything this color
    
};

class ADrawer2D : public IDrawer2D //abstract IDrawer2D already implements some functions in terms of some others
{
  public:
  
    virtual void drawEllipse(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled);
    virtual void drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled);
    
    virtual void convertTextureIfNeeded(ITexture*& texture);

    virtual void drawTextureCentered(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSizedCentered(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    //virtual void drawTextureTransformedCentered(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod);
};


} //end of namespace lpi

