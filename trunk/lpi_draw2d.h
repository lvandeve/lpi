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
    
    enum HAlign
    {
      HA_LEFT,
      HA_CENTER,
      HA_RIGHT
    };
    
    enum VAlign
    {
      VA_TOP,
      VA_CENTER,
      VA_BOTTOM
    };
    
    
    /*
    drawText: the most basic text printing function.
    x and y are the coordinates of the rectangle in which the text will be. For
    HA_LEFT, VA_TOP alignment, this is the top left position of the rectangle,
    for HA_CENTER, VA_CENTER alignment, this is the center of the rectangle, 
    and so on...
    */
    virtual void drawText(const std::string& text, int x, int y, const Font& font = FONT_Default, HAlign halign = HA_LEFT, VAlign valign = VA_TOP) = 0;
    
    /*
    getFontHeight
    Returns the height of the font. This is the height of a single line of text.
    Unlike the width of characters, it is guaranteed that the height of a line
    is always the same. So given the result of this function, you can know exactly
    how high any amount of lines of text will be (in pixels).
    */
    virtual size_t getFontHeight(const Font& font = FONT_Default) = 0;
    
    /*
    calcTextRectSize: get the size of the rectangle that the text drawn with drawText will take.
    There are no alignment parameters because it has no effect on the rectangle size, but note that
    alignment will affect the position of the rectangle.
    */
    virtual void calcTextRectSize(int& w, int& h, const std::string& text, const Font& font = FONT_Default) = 0;
    
    /*
    calcTextPosToChar: gives the index of the character in the string text, that will be drawn at
    location x, y, the position relative the the coordinates you will give to the drawText function.
    This can be used to go from mouse position to the index of the cursor, or for line-breaking
    strategies of text in a limited area.
    The index returned is a char in the string which will always be the first char of
    a glyph (if a glyph exists out of multiple char's, like can happen with UTF-8).
    */
    virtual size_t calcTextPosToChar(int x, int y, const std::string& text, const Font& font = FONT_Default, HAlign halign = HA_LEFT, VAlign valign = VA_TOP) = 0;
    
    /*
    calcTextCharToPos: this does the opposite of calcTextPosToChar. Given the index of the character in the text,
    it returns the relative position to the (x, y) of the drawTextFunction of that character's top left corner.
    x and y are output parameters.
    */
    virtual void calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font = FONT_Default, HAlign halign = HA_LEFT, VAlign valign = VA_TOP) = 0;
    
    
    //templatized version for convenience. Different name than drawText because otherwise overriding drawText hides this function.
    template<typename T>
    void print(const T& val, int x, int y, const Font& font = FONT_Default, HAlign halign = HA_LEFT, VAlign valign = VA_TOP)
    {
      std::ostringstream sstream;
      sstream << val;
      drawText(sstream.str(), x, y, font, halign, valign);
    }
    
    /*
    TODO: drawing text in a limited rectangular area with word wrapping. To do that, don't add functions
    to IDrawer2D, but instead make a class which has a line breaking strategy, and which uses the
    functions like calcTextRectSize and so on to determine where it has to break the text. It can have
    strategies like breaking between words, between letters, ... It should already itself break
    the text into multiple strings (one per line) and then do the line breaking per substring (faster
    than asking these parameters for the complete string all the time)
    */
    
    //TODO: cache rendered text in objects, ...
    
    
    
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

    virtual void drawTextureCentered(const ITexture* texture, int x, int y, const ColorRGB& colorMod);
    virtual void drawTextureSizedCentered(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod);
    //virtual void drawTextureTransformedCentered(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod);
};


} //end of namespace lpi

