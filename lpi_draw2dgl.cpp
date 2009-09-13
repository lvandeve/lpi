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

#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_gl.h"
#include <vector>

namespace lpi
{

static int numSegmentsHelper(double radius)
{
  static const int DEFAULT = 64;
  if(radius > DEFAULT / 3)
    return DEFAULT;
  else if(radius > 3) return (int)(radius * 3);
  else return 3;
}

//Draw a line from (x1, y1) to (x2, y2) on the OpenGL screen. NOTE: end coordinates should not be included in the line
void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1, int clipy1, int clipx2, int clipy2)
{
  //don't clip if the user gives no clipping area by making clipx2 smaller than clipx1
  bool clip = (clipx2 >= clipx1);
  
  //clip if some point is outside the clipping area
  if(clip)
  if(x1 < clipx1 || x1 >= clipx2 || x2 < clipx1 || x2 >= clipx2 || y1 < clipy1 || y1 >= clipy2 || y2 < clipy1 || y2 >= clipy2)
  {
    int x3, y3, x4, y4;
    if(!clipLine(x1, y1, x2, y2, x3, y3, x4, y4, clipx1, clipy1, clipx2, clipy2)) return;
    x1 = x3;
    y1 = y3;
    x2 = x4;
    y2 = y4;
  }
  
  //draw the line with OpenGL
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glColor4ub(color.r, color.g, color.b, color.a);
  
  setOpenGLScissor(); //everything that draws something must always do this

    
  glBegin(GL_LINES);
    glVertex2d(x1 + 0.5, y1 + 0.5);
    glVertex2d(x2 + 0.5, y2 + 0.5);
  glEnd();
}

void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color)
{
  drawLine(x1, y1, x2, y2, color, 0, 0, screenWidth(), screenHeight());
}

void recursive_bezier(double x0, double y0, //endpoint
                      double x1, double y1, //handle
                      double x2, double y2, //handle
                      double x3, double y3, //endpoint
                      const ColorRGB& color,
                      int n = 0) //extra recursion test for safety
{
  if(bezier_nearly_flat(x0, y0, x1, y1, x2, y2, x3, y3) || n > 20)
  {
    drawLine((int)x0, (int)y0, (int)x3, (int)y3, color);
  }
  else
  {
    double x01   = (x0 + x1) / 2;
    double y01   = (y0 + y1) / 2;
    double x12   = (x1 + x2) / 2;
    double y12   = (y1 + y2) / 2;
    double x23   = (x2 + x3) / 2;
    double y23   = (y2 + y3) / 2;
    double x012  = (x01 + x12) / 2;
    double y012  = (y01 + y12) / 2;
    double x123  = (x12 + x23) / 2;
    double y123  = (y12 + y23) / 2;
    double x0123 = (x012 + x123) / 2;
    double y0123 = (y012 + y123) / 2;
    
    recursive_bezier(x0, y0, x01, y01, x012, y012, x0123, y0123, color, n + 1); 
    recursive_bezier(x0123, y0123, x123, y123, x23, y23, x3, y3, color, n + 1);
  }
}

void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color)
{
  recursive_bezier(x0, y0, x1, y1, x2, y2, x3, y3, color);
}

void pset(int x, int y, const ColorRGB& color)
{
  //draw the point with OpenGL
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  
  setOpenGLScissor(); //everything that draws something must always do this

  glBegin(GL_POINTS);
    glVertex2d(x + 0.375, y + 0.375);
  glEnd();
}

void drawPoint(int x, int y, const ColorRGB& color)
{
  pset(x, y, color);
}
  
//Draw a gradient line from (x1, y1) to (x2, y2)
void gradientLine(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2)
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  
  setOpenGLScissor(); //everything that draws something must always do this

  glBegin(GL_LINES);
    glColor4f(color1.r / 255.0, color1.g / 255.0, color1.b / 255.0, color1.a / 255.0);
    glVertex3d(x1, screenHeight() - y1, 1);
    glColor4f(color2.r / 255.0, color2.g / 255.0, color2.b / 255.0, color2.a / 255.0);
    glVertex3d(x2, screenHeight() - y2, 1);
  glEnd();
}

//Draw an untextured, filled, rectangle on screen from (x1, y1) to (x2, y2). The end coordinates should NOT be included
void drawRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  if(filled)
  {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
  
    glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
    
    setOpenGLScissor(); //everything that draws something must always do this
    
    static const double OGLDIFF = 0.0;
  
    glBegin(GL_QUADS);
      glVertex3d(x2 - OGLDIFF, y1          , 1);
      glVertex3d(x1          , y1          , 1);
      glVertex3d(x1          , y2 - OGLDIFF, 1);
      glVertex3d(x2 - OGLDIFF, y2 - OGLDIFF, 1);
    glEnd();
  }
  else
  {
    drawLine(x1, y1, x2, y1, color);
    drawLine(x1, y2 - 1, x2, y2 - 1, color);
    drawLine(x1, y1, x1, y2, color);
    drawLine(x2 - 1, y1, x2 - 1, y2, color);
  }
}

void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  if(filled)
  {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
  
    glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
    
    setOpenGLScissor(); //everything that draws something must always do this
    
    //static const double OGLDIFFX = 0.0, OGLDIFFY = 0.0;
  
    glBegin(GL_TRIANGLES);
      glVertex3d(x0, y0, 1);
      glVertex3d(x1, y1, 1);
      glVertex3d(x2, y2, 1);
    glEnd();
  }
  else
  {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
  }
}

void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled)
{
  if(filled)
  {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
  
    glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
    
    setOpenGLScissor(); //everything that draws something must always do this
    
    //static const double OGLDIFFX = 0.0, OGLDIFFY = 0.0;
  
    glBegin(GL_QUADS);
      glVertex3d(x0, y0, 1);
      glVertex3d(x1, y1, 1);
      glVertex3d(x2, y2, 1);
      glVertex3d(x3, y3, 1);
    glEnd();
  }
  else
  {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x0, y0, color);
  }
}

void gradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_QUADS);
    glColor4f(color0.r / 255.0, color0.g / 255.0, color0.b / 255.0, color0.a / 255.0);
    glVertex3d(x0, y0, 1);
    glColor4f(color1.r / 255.0, color1.g / 255.0, color1.b / 255.0, color1.a / 255.0);
    glVertex3d(x1, y1, 1);
    glColor4f(color2.r / 255.0, color2.g / 255.0, color2.b / 255.0, color2.a / 255.0);
    glVertex3d(x2, y2, 1);
    glColor4f(color3.r / 255.0, color3.g / 255.0, color3.b / 255.0, color3.a / 255.0);
    glVertex3d(x3, y3, 1);
  glEnd();
}

//Draw a rectangle with 4 different corner colors on screen from (x1, y1) to (x2, y2). The end coordinates should NOT be included
void gradientRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4)
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_QUADS);
    glColor4f(color2.r / 255.0, color2.g / 255.0, color2.b / 255.0, color2.a / 255.0);
    glVertex3d(x2, y1, 1);
    glColor4f(color1.r / 255.0, color1.g / 255.0, color1.b / 255.0, color1.a / 255.0);
    glVertex3d(x1, y1, 1);
    glColor4f(color3.r / 255.0, color3.g / 255.0, color3.b / 255.0, color3.a / 255.0);
    glVertex3d(x1, y2, 1);
    glColor4f(color4.r / 255.0, color4.g / 255.0, color4.b / 255.0, color4.a / 255.0);
    glVertex3d(x2, y2, 1);
  glEnd();
}

void drawEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = 32;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_LINE_STRIP);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      glVertex3d(x + std::cos(angle) * radiusx, y + std::sin(angle) * radiusy, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radiusx, y, 1);
  glEnd();
}

void drawFilledEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = 32;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_TRIANGLE_FAN);
    glVertex3d(x, y, 1);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      glVertex3d(x + std::cos(angle) * radiusx, y + std::sin(angle) * radiusy, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radiusx, y, 1);
  glEnd();
}

void drawCircle(int x, int y, double radius, const ColorRGB& color)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = numSegmentsHelper(radius);
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_LINE_STRIP);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      glVertex3d(x + std::cos(angle) * radius, y + std::sin(angle) * radius, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radius, y, 1);
  glEnd();
}

void drawDisk(int x, int y, double radius, const ColorRGB& color)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = numSegmentsHelper(radius);
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_TRIANGLE_FAN);
    glVertex3d(x, y, 1);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      glVertex3d(x + std::cos(angle) * radius, y + std::sin(angle) * radius, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radius, y, 1);
  glEnd();
}


void drawGradientDisk(int x, int y, double radius, const ColorRGB& color1, const ColorRGB& color2)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = numSegmentsHelper(radius);
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  //glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_TRIANGLE_FAN);
    glColor4f(color1.r / 255.0, color1.g / 255.0, color1.b / 255.0, color1.a / 255.0);
    glVertex3d(x, y, 1);
    glColor4f(color2.r / 255.0, color2.g / 255.0, color2.b / 255.0, color2.a / 255.0);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      
      glVertex3d(x + std::cos(angle) * radius, y + std::sin(angle) * radius, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radius, y, 1);
  glEnd();
}

void drawGradientEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color1, const ColorRGB& color2)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = 32;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  //glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  setOpenGLScissor(); //everything that draws something must always do this
  
  glBegin(GL_TRIANGLE_FAN);
    glColor4f(color1.r / 255.0, color1.g / 255.0, color1.b / 255.0, color1.a / 255.0);
    glVertex3d(x, y, 1);
    glColor4f(color2.r / 255.0, color2.g / 255.0, color2.b / 255.0, color2.a / 255.0);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      
      glVertex3d(x + std::cos(angle) * radiusx, y + std::sin(angle) * radiusy, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radiusx, y, 1);
  glEnd();
}

////////////////////////////////////////////////////////////////////////////////

size_t Drawer2DGL::getWidth()
{
  return lpi::screenWidth();
}

size_t Drawer2DGL::getHeight()
{
  return lpi::screenHeight();
}

void Drawer2DGL::pushScissor(int x0, int y0, int x1, int y1)
{
  lpi::setScissor(x0, y0, x1, y1);
}

void Drawer2DGL::pushSmallestScissor(int x0, int y0, int x1, int y1)
{
  lpi::setSmallestScissor(x0, y0, x1, y1);
}

void Drawer2DGL::popScissor()
{
  lpi::resetScissor();
}

void Drawer2DGL::drawPoint(int x, int y, const ColorRGB& color)
{
  lpi::drawPoint(x, y, color);
}

void Drawer2DGL::drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  lpi::drawLine(x0, y0, x1, y1, color);
}

void Drawer2DGL::drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color)
{
  lpi::drawBezier(x0, y0, x1, y1, x2, y2, x3, y3, color);
}

    
void Drawer2DGL::drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled)
{
  lpi::drawRectangle(x0, y0, x1, y1, color, filled);
}

void Drawer2DGL::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  lpi::drawTriangle(x0, y0, x1, y1, x2, y2, color, filled);
}

void Drawer2DGL::drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled)
{
  lpi::drawQuad(x0, y0, x1, y1, x2, y2, x3, y3, color, filled);
}

void Drawer2DGL::drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled)
{
  filled ? lpi::drawDisk(x, y, (double)radius, color) : lpi::drawCircle(x, y, (double)radius, color);
}

void Drawer2DGL::drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled)
{
  filled ? lpi::drawFilledEllipse(x, y, radiusx, radiusy, color) : lpi::drawEllipse(x, y, radiusx, radiusy, color);
}

void Drawer2DGL::drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  lpi::gradientQuad(x0, y0, x1, y1, x2, y2, x3, y3, color0, color1, color2, color3);
}

bool Drawer2DGL::supportsTexture(ITexture* texture)
{
  return dynamic_cast<TextureGL*>(texture);
}

ITexture* Drawer2DGL::createTexture()
{
  return new TextureGL();
}

ITexture* Drawer2DGL::createTexture(ITexture* texture)
{
  TextureGL* gl = new TextureGL();
  makeTextureFromBuffer(gl
                      , texture->getBuffer(), texture->getU2(), texture->getV2()
                      , AE_Opaque
                      , 0, 0, texture->getU(), texture->getV());
  return gl;
}

    
void Drawer2DGL::drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod)
{
  drawTextureSized(texture, x, y, texture->getU(), texture->getV(), colorMod);
}

void Drawer2DGL::drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  if(sizex == 0 || sizey == 0) return;
  
  const TextureGL* texturegl = dynamic_cast<const TextureGL*>(texture);
  if(!texturegl) return;

  glEnable(GL_TEXTURE_2D);

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  texturegl->bind();

  lpi::setOpenGLScissor(); //everything that draws something must always do this //TODO: investigate that statement
  
  double u3 = (double)texturegl->getU() / (double)texturegl->getU2();
  double v3 = (double)texturegl->getV() / (double)texturegl->getV2();

  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(x +          0, y +          0);
    glTexCoord2d(+u3, 0.0); glVertex2d(x + (int)sizex, y +          0);
    glTexCoord2d(+u3, +v3); glVertex2d(x + (int)sizex, y + (int)sizey);
    glTexCoord2d(0.0, +v3); glVertex2d(x +          0, y + (int)sizey);
  glEnd();
}

void Drawer2DGL::drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod)
{
  if(x0 == x1 || y0 == y1) return;
  
  const TextureGL* texturegl = dynamic_cast<const TextureGL*>(texture);
  if(!texturegl) return;
  
  glEnable(GL_TEXTURE_2D);

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  texturegl->bind();

  lpi::setOpenGLScissor(); //everything that draws something must always do this //TODO: investigate that statement
  
  bool simple = true;
  if(x1 - x0 > (int)texture->getU() && texture->getU() != texture->getU2()) simple = false;
  if(y1 - y0 > (int)texture->getV() && texture->getV() != texture->getV2()) simple = false;
  
  if(simple)
  {
    double coorx = (double(x1 - x0) / texturegl->getU());
    double coory = (double(y1 - y0) / texturegl->getV());

    //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
    glBegin(GL_QUADS);
      glTexCoord2d(0.0,       0.0); glVertex3d(x0, y0, 1);
      glTexCoord2d(0.0,    +coory); glVertex3d(x0, y1, 1);
      glTexCoord2d(+coorx, +coory); glVertex3d(x1, y1, 1);
      glTexCoord2d(+coorx,    0.0); glVertex3d(x1, y0, 1);
    glEnd();
  }
  else //need to tile manually, slow!!
  {
    int numx = (x1 - x0) / texture->getU();
    int numy = (y1 - y0) / texture->getV();
    
    //TODO: this is just a very lazy implementation because I was doing something else. Fix the TODO's below!!!
    //TODO: make this more efficient, e.g. quadstrip...
    //TODO: also add the extra textures at the edge (I mean, now I only have the full tiles, at the sides are possibly also partial tiles)!!!!
    for(int x = 0; x < numx; x++)
    {
      for(int y = 0; y < numy; y++)
      {
        int xb0 = x0 + x * texture->getU();
        int yb0 = y0 + y * texture->getV();
        int xb1 = xb0 + texture->getU();
        int yb1 = yb0 + texture->getV();
        
        glBegin(GL_QUADS);
          glTexCoord2d(0.0, 0.0); glVertex3d(xb0, yb0, 1);
          glTexCoord2d(0.0, 1.0); glVertex3d(xb0, yb1, 1);
          glTexCoord2d(1.0, 1.0); glVertex3d(xb1, yb1, 1);
          glTexCoord2d(1.0, 0.0); glVertex3d(xb1, yb0, 1);
        glEnd();
      }
    }
  }
}

} //end of namespace lpi

