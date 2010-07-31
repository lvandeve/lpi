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
#include "lpi_texture_gl.h"

#include <vector>
#include <GL/gl.h>

namespace lpi
{

Drawer2DGL::Drawer2DGL(ScreenGL* screen)
: screen(screen)
{
  //TextureFactoryGL factory(screen->getGLContext());
}

Drawer2DGL::~Drawer2DGL()
{
}

namespace
{

int numSegmentsHelper(double radius)
{
  static const int DEFAULT = 64;
  if(radius > DEFAULT / 3)
    return DEFAULT;
  else if(radius > 3) return (int)(radius * 3);
  else return 3;
}

}

void Drawer2DGL::prepareDrawUntextured(bool filledGeometry)
{
  screen->set2DScreen(filledGeometry);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  //screen->setOpenGLScissor(); //everything that draws something must always do this //TODO: investigate this statement
}

void Drawer2DGL::prepareDrawTextured()
{
  screen->set2DScreen(true);
  glEnable(GL_TEXTURE_2D);
  //screen->setOpenGLScissor(); //everything that draws something must always do this //TODO: investigate that statement
}

void Drawer2DGL::drawLineInternal(int x0, int y0, int x1, int y1) //doesn't call "prepareDraw", to be used by other things that draw multiple lines
{
  glBegin(GL_LINES);
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);
  glEnd();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Drawer2DGL::frameStart()
{
  screen->setOpenGLScissor();
}

void Drawer2DGL::frameEnd()
{
}


//Draw a rectangle with 4 different corner colors on screen from (x1, y1) to (x2, y2). The end coordinates should NOT be included
void Drawer2DGL::drawGradientRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  prepareDrawUntextured(true);
  
  glBegin(GL_QUADS);
    glColor4ub(color1.r, color1.g, color1.b, color1.a);
    glVertex2f(x1, y0);
    glColor4ub(color0.r, color0.g, color0.b, color0.a);
    glVertex2f(x0, y0);
    glColor4ub(color2.r, color2.g, color2.b, color2.a);
    glVertex2f(x0, y1);
    glColor4ub(color3.r, color3.g, color3.b, color3.a);
    glVertex2f(x1, y1);
  glEnd();
}

void Drawer2DGL::drawGradientDisk(int x, int y, double radius, const ColorRGB& color1, const ColorRGB& color2)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = numSegmentsHelper(radius);
  
  prepareDrawUntextured(true);
  
  glBegin(GL_TRIANGLE_FAN);
    glColor4ub(color1.r, color1.g, color1.b, color1.a);
    glVertex2f(x, y);
    glColor4ub(color2.r, color2.g, color2.b, color2.a);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      
      glVertex2f(x + std::cos(angle) * radius, y + std::sin(angle) * radius);
      angle += 2 * pi / numsegments;
    }
    glVertex2f(x + radius, y);
  glEnd();
}

void Drawer2DGL::drawGradientEllipse(int x, int y, double radiusx, double radiusy, const ColorRGB& color1, const ColorRGB& color2)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = 32;
  
  prepareDrawUntextured(true);
  
  glBegin(GL_TRIANGLE_FAN);
    glColor4ub(color1.r, color1.g, color1.b, color1.a);
    glVertex2f(x, y);
    glColor4ub(color2.r, color2.g, color2.b, color2.a);
    double angle = 0;
    for(size_t i = 0; i < numsegments; i++)
    {
      
      glVertex2f(x + std::cos(angle) * radiusx, y + std::sin(angle) * radiusy);
      angle += 2 * pi / numsegments;
    }
    glVertex2f(x + radiusx, y);
  glEnd();
}

//Draw a gradient line from (x1, y1) to (x2, y2)
void Drawer2DGL::gradientLine(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2)
{
  prepareDrawUntextured(false);

  glBegin(GL_LINES);
    glColor4ub(color1.r, color1.g, color1.b, color1.a);
    glVertex2f(x1, screen->screenHeight() - y1);
    glColor4ub(color2.r, color2.g, color2.b, color2.a);
    glVertex2f(x2, screen->screenHeight() - y2);
  glEnd();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

size_t Drawer2DGL::getWidth()
{
  return screen->screenWidth();
}

size_t Drawer2DGL::getHeight()
{
  return screen->screenHeight();
}

void Drawer2DGL::pushScissor(int x0, int y0, int x1, int y1)
{
  screen->setScissor(x0, y0, x1, y1);
}

void Drawer2DGL::pushSmallestScissor(int x0, int y0, int x1, int y1)
{
  screen->setSmallestScissor(x0, y0, x1, y1);
}

void Drawer2DGL::popScissor()
{
  screen->resetScissor();
}

void Drawer2DGL::drawPoint(int x, int y, const ColorRGB& color)
{
  prepareDrawUntextured(false);

  glColor4ub(color.r, color.g, color.b, color.a);

  glBegin(GL_POINTS);
    glVertex2d(x + 0.375, y + 0.375);
  glEnd();
}

void Drawer2DGL::drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  prepareDrawUntextured(false);
  glColor4ub(color.r, color.g, color.b, color.a);
  
  drawLineInternal(x0, y0, x1, y1);
}

void Drawer2DGL::recursive_bezier(double x0, double y0, //endpoint
                                  double x1, double y1, //handle
                                  double x2, double y2, //handle
                                  double x3, double y3, //endpoint
                                  int n) //extra recursion test for safety
{
  if(bezier_nearly_flat(x0, y0, x1, y1, x2, y2, x3, y3) || n > 20)
  {
    drawLineInternal((int)x0, (int)y0, (int)x3, (int)y3);
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
    
    recursive_bezier(x0, y0, x01, y01, x012, y012, x0123, y0123, n + 1); 
    recursive_bezier(x0123, y0123, x123, y123, x23, y23, x3, y3, n + 1);
  }
}

void Drawer2DGL::drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color)
{
  prepareDrawUntextured(false);
  glColor4ub(color.r, color.g, color.b, color.a);
  
  recursive_bezier(x0, y0, x1, y1, x2, y2, x3, y3, 0);
}

    
void Drawer2DGL::drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled)
{
  prepareDrawUntextured(filled);
  glColor4ub(color.r, color.g, color.b, color.a);

  if(filled)
  {
    glBegin(GL_QUADS);
      glVertex2f(x1, y0);
      glVertex2f(x0, y0);
      glVertex2f(x0, y1);
      glVertex2f(x1, y1);
    glEnd();
  }
  else
  {
    glBegin(GL_LINE_LOOP);
      glVertex2f(x0, y0);
      glVertex2f(x1 - 1, y0);
      glVertex2f(x1 - 1, y1 - 1);
      glVertex2f(x0, y1 - 1);
    glEnd();
  }
}

void Drawer2DGL::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  prepareDrawUntextured(filled);
  glColor4ub(color.r, color.g, color.b, color.a);

  if(filled)
  {
    glBegin(GL_TRIANGLES);
      glVertex2f(x0, y0);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
    glEnd();
  }
  else
  {
    glBegin(GL_LINE_LOOP);
      glVertex2f(x0, y0);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
    glEnd();
  }
}

void Drawer2DGL::drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled)
{
  prepareDrawUntextured(filled);
  glColor4ub(color.r, color.g, color.b, color.a);

  if(filled)
  {
    glBegin(GL_QUADS);
      glVertex2f(x0, y0);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
      glVertex2f(x3, y3);
    glEnd();
  }
  else
  {
    glBegin(GL_LINE_LOOP);
      glVertex2f(x0, y0);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
      glVertex2f(x3, y3);
    glEnd();
  }
}

void Drawer2DGL::drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled)
{
  drawEllipseCentered(x, y, radius, radius, color, filled);
}

void Drawer2DGL::drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled)
{
  static const double pi = 3.141592653589793238;
  static const size_t numsegments = 64;
  
  prepareDrawUntextured(filled);
  glColor4ub(color.r, color.g, color.b, color.a);

  if(filled)
  {
    glBegin(GL_TRIANGLE_FAN);
      glVertex2f(x, y);
      double angle = 0;
      for(size_t i = 0; i < numsegments; i++)
      {
        glVertex2f(x + std::cos(angle) * radiusx, y + std::sin(angle) * radiusy);
        angle += 2 * pi / numsegments;
      }
      glVertex2f(x + radiusx, y);
    glEnd();
  }
  else
  {
    glBegin(GL_LINE_STRIP);
      double angle = 0;
      for(size_t i = 0; i < numsegments; i++)
      {
        glVertex2f(x + std::cos(angle) * radiusx, y + std::sin(angle) * radiusy);
        angle += 2 * pi / numsegments;
      }
      glVertex2f(x + radiusx, y);
    glEnd();
  }
}

void Drawer2DGL::drawGradientTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2)
{
  prepareDrawUntextured(true);
  
  glBegin(GL_TRIANGLES);
    glColor4ub(color0.r, color0.g, color0.b, color0.a);
    glVertex2f(x0, y0);
    glColor4ub(color1.r, color1.g, color1.b, color1.a);
    glVertex2f(x1, y1);
    glColor4ub(color2.r, color2.g, color2.b, color2.a);
    glVertex2f(x2, y2);
  glEnd();
}


void Drawer2DGL::drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  prepareDrawUntextured(true);
  
  glBegin(GL_QUADS);
    glColor4ub(color0.r, color0.g, color0.b, color0.a);
    glVertex2f(x0, y0);
    glColor4ub(color1.r, color1.g, color1.b, color1.a);
    glVertex2f(x1, y1);
    glColor4ub(color2.r, color2.g, color2.b, color2.a);
    glVertex2f(x2, y2);
    glColor4ub(color3.r, color3.g, color3.b, color3.a);
    glVertex2f(x3, y3);
  glEnd();
}

bool Drawer2DGL::supportsTexture(ITexture* texture)
{
  return dynamic_cast<TextureGL*>(texture);
}

ITexture* Drawer2DGL::createTexture() const
{
  return new TextureGL(screen->getGLContext());
}

ITexture* Drawer2DGL::createTexture(ITexture* texture) const
{
  TextureGL* gl = new TextureGL(screen->getGLContext());
  makeTextureFromBuffer(gl
                      , texture->getBuffer(), texture->getU2(), texture->getV2()
                      , AE_Nothing
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

  prepareDrawTextured();
  texturegl->updateForNewOpenGLContextIfNeeded();
  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  
  if(texturegl->getNumParts() == 1)
  {
    texturegl->bind(screen->isSmoothingEnabled(), 0);

    double u3 = (double)texturegl->getU() / (double)texturegl->getU2();
    double v3 = (double)texturegl->getV() / (double)texturegl->getV2();

    //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
    glBegin(GL_QUADS);
      glTexCoord2d(0.0, 0.0); glVertex2f(x +          0, y +          0);
      glTexCoord2d(+u3, 0.0); glVertex2f(x + (int)sizex, y +          0);
      glTexCoord2d(+u3, +v3); glVertex2f(x + (int)sizex, y + (int)sizey);
      glTexCoord2d(0.0, +v3); glVertex2f(x +          0, y + (int)sizey);
    glEnd();
  }
  else
  {
    for(size_t i = 0; i < texturegl->getNumParts(); i++)
    {
      texturegl->bind(screen->isSmoothingEnabled(), i);
      const TextureGL::Part& part = texturegl->getPart(i);
      double u3 = (double)part.u / part.u2;
      double v3 = (double)part.v / part.v2;
      
      double zoomx = (double)sizex / (double)texture->getU();
      double zoomy = (double)sizey / (double)texture->getV();
      
      double px = x + zoomx * part.shiftx;
      double py = y + zoomy * part.shifty;
      double psizex = sizex * ((double)part.u / (double)texture->getU());
      double psizey = sizey * ((double)part.v / (double)texture->getV());

      //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
      glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(px +      0, py +      0);
        glTexCoord2d(+u3, 0.0); glVertex2d(px + psizex, py +      0);
        glTexCoord2d(+u3, +v3); glVertex2d(px + psizex, py + psizey);
        glTexCoord2d(0.0, +v3); glVertex2d(px +      0, py + psizey);
      glEnd();

    }
  }
}

void Drawer2DGL::drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod)
{
  if(x0 == x1 || y0 == y1) return;
  
  const TextureGL* texturegl = dynamic_cast<const TextureGL*>(texture);
  if(!texturegl) return;
  
  prepareDrawTextured();
  texturegl->updateForNewOpenGLContextIfNeeded();

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  texturegl->bind(screen->isSmoothingEnabled(), 0);
  
  bool simple = true;
  if(x1 - x0 > (int)texture->getU() && texture->getU() != texture->getU2()) simple = false;
  if(y1 - y0 > (int)texture->getV() && texture->getV() != texture->getV2()) simple = false;
  if(texturegl->getNumParts() != 1) simple = false;
  
  if(simple)
  {
    double coorx = (double(x1 - x0) / texturegl->getU());
    double coory = (double(y1 - y0) / texturegl->getV());

    //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
    glBegin(GL_QUADS);
      glTexCoord2d(0.0,       0.0); glVertex2f(x0, y0);
      glTexCoord2d(0.0,    +coory); glVertex2f(x0, y1);
      glTexCoord2d(+coorx, +coory); glVertex2f(x1, y1);
      glTexCoord2d(+coorx,    0.0); glVertex2f(x1, y0);
    glEnd();
  }
  else if(texturegl->getNumParts() == 1) //need to tile manually, slow!!
  {
    int numx = (x1 - x0) / texture->getU();
    int numy = (y1 - y0) / texture->getV();
    
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
          glTexCoord2d(0.0, 0.0); glVertex2f(xb0, yb0);
          glTexCoord2d(0.0, 1.0); glVertex2f(xb0, yb1);
          glTexCoord2d(1.0, 1.0); glVertex2f(xb1, yb1);
          glTexCoord2d(1.0, 0.0); glVertex2f(xb1, yb0);
        glEnd();
      }
    }
  }
  else
  {
    //TODO: larger than max size textures
  }
}

void Drawer2DGL::drawTextureSizedRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  if(x0 == x1 || y0 == y1) return;

  const TextureGL* texturegl = dynamic_cast<const TextureGL*>(texture);
  if(!texturegl) return;

  prepareDrawTextured();
  texturegl->updateForNewOpenGLContextIfNeeded();

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  texturegl->bind(screen->isSmoothingEnabled(), 0);

  bool simple = true;
  if(x1 - x0 > (int)texture->getU() && texture->getU() != texture->getU2()) simple = false;
  if(y1 - y0 > (int)texture->getV() && texture->getV() != texture->getV2()) simple = false;
  if(texturegl->getNumParts() != 1) simple = false;

  if(simple)
  {
    double scalex = (double)texture->getU() / (double)sizex;
    double scaley = (double)texture->getV() / (double)sizey;
    double coorx = ((double)(x1 - x0) / texturegl->getU()) * scalex;
    double coory = ((double)(y1 - y0) / texturegl->getV()) * scaley;

    //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
    glBegin(GL_QUADS);
      glTexCoord2d(0.0,       0.0); glVertex2f(x0, y0);
      glTexCoord2d(0.0,    +coory); glVertex2f(x0, y1);
      glTexCoord2d(+coorx, +coory); glVertex2f(x1, y1);
      glTexCoord2d(+coorx,    0.0); glVertex2f(x1, y0);
    glEnd();
  }
  else if(texturegl->getNumParts() == 1) //need to tile manually, slow!!
  {
    int numx = (x1 - x0) / texture->getU();
    int numy = (y1 - y0) / texture->getV();

    //TODO: make this more efficient, e.g. quadstrip...
    //TODO: also add the extra textures at the edge (I mean, now I only have the full tiles, at the sides are possibly also partial tiles)!!!!
    //TODO: the scaling of sizex and sizey are ignored here, implement that too!!!!
    for(int x = 0; x < numx; x++)
    {
      for(int y = 0; y < numy; y++)
      {
        int xb0 = x0 + x * texture->getU();
        int yb0 = y0 + y * texture->getV();
        int xb1 = xb0 + texture->getU();
        int yb1 = yb0 + texture->getV();

        glBegin(GL_QUADS);
          glTexCoord2d(0.0, 0.0); glVertex2f(xb0, yb0);
          glTexCoord2d(0.0, 1.0); glVertex2f(xb0, yb1);
          glTexCoord2d(1.0, 1.0); glVertex2f(xb1, yb1);
          glTexCoord2d(1.0, 0.0); glVertex2f(xb1, yb0);
        glEnd();
      }
    }
  }
  else
  {
    //TODO: larger than max size textures
  }

}




void Drawer2DGL::drawTextureGradient(const ITexture* texture, int x, int y
                                   , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11)
{
  const TextureGL* texturegl = dynamic_cast<const TextureGL*>(texture);
  if(!texturegl) return;
  
  int sizex = texture->getU();
  int sizey = texture->getV();

  prepareDrawTextured();
  texturegl->updateForNewOpenGLContextIfNeeded();
  
  if(texturegl->getNumParts() == 1)
  {
    texturegl->bind(screen->isSmoothingEnabled(), 0);

    double u3 = (double)texturegl->getU() / (double)texturegl->getU2();
    double v3 = (double)texturegl->getV() / (double)texturegl->getV2();

    //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
    glBegin(GL_QUADS);
      glColor4ub(color00.r, color00.g, color00.b, color00.a); glTexCoord2d(0.0, 0.0); glVertex2d(x +          0, y +          0);
      glColor4ub(color10.r, color10.g, color10.b, color10.a); glTexCoord2d(+u3, 0.0); glVertex2d(x + (int)sizex, y +          0);
      glColor4ub(color11.r, color11.g, color11.b, color11.a); glTexCoord2d(+u3, +v3); glVertex2d(x + (int)sizex, y + (int)sizey);
      glColor4ub(color01.r, color01.g, color01.b, color01.a); glTexCoord2d(0.0, +v3); glVertex2d(x +          0, y + (int)sizey);
    glEnd();
  }
  else
  {
    //TODO
  }
}

void Drawer2DGL::drawTextureRepeatedGradient(const ITexture* texture, int x0, int y0, int x1, int y1
                                           , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11)
{
  if(x0 == x1 || y0 == y1) return;
  
  const TextureGL* texturegl = dynamic_cast<const TextureGL*>(texture);
  if(!texturegl) return;
  
  prepareDrawTextured();
  texturegl->updateForNewOpenGLContextIfNeeded();

  texturegl->bind(screen->isSmoothingEnabled(), 0);
  
  bool simple = true;
  if(x1 - x0 > (int)texture->getU() && texture->getU() != texture->getU2()) simple = false;
  if(y1 - y0 > (int)texture->getV() && texture->getV() != texture->getV2()) simple = false;
  if(texturegl->getNumParts() != 1) simple = false;
  
  if(simple)
  {
    double coorx = (double(x1 - x0) / texturegl->getU());
    double coory = (double(y1 - y0) / texturegl->getV());

    //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
    glBegin(GL_QUADS);
      glColor4ub(color00.r, color00.g, color00.b, color00.a); glTexCoord2d(0.0,       0.0); glVertex2f(x0, y0);
      glColor4ub(color01.r, color01.g, color01.b, color01.a); glTexCoord2d(0.0,    +coory); glVertex2f(x0, y1);
      glColor4ub(color11.r, color11.g, color11.b, color11.a); glTexCoord2d(+coorx, +coory); glVertex2f(x1, y1);
      glColor4ub(color10.r, color10.g, color10.b, color10.a); glTexCoord2d(+coorx,    0.0); glVertex2f(x1, y0);
    glEnd();
  }
  else if(texturegl->getNumParts() == 1) //need to tile manually, slow!!
  {
    int numx = (x1 - x0) / texture->getU();
    int numy = (y1 - y0) / texture->getV();
    
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
        
        double xf0 = (xb0 - x0) / (double)(x1 - x0);
        double yf0 = (yb0 - y0) / (double)(y1 - y0);
        double xf1 = (xb1 - x0) / (double)(x1 - x0);
        double yf1 = (yb1 - y0) / (double)(y1 - y0);
        
        ColorRGB c00 = (color00 * (1.0-xf0) + color10 * xf0) * (1.0-yf0) + (color01 * (1.0-xf0) + color11 * xf0) * yf0;
        ColorRGB c01 = (color00 * (1.0-xf0) + color10 * xf0) * (1.0-yf1) + (color01 * (1.0-xf0) + color11 * xf0) * yf1;
        ColorRGB c10 = (color00 * (1.0-xf1) + color10 * xf1) * (1.0-yf0) + (color01 * (1.0-xf1) + color11 * xf1) * yf0;
        ColorRGB c11 = (color00 * (1.0-xf1) + color10 * xf1) * (1.0-yf1) + (color01 * (1.0-xf1) + color11 * xf1) * yf1;
        
        glBegin(GL_QUADS);
          glTexCoord2d(0.0, 0.0); glVertex2f(xb0, yb0);
          glTexCoord2d(0.0, 1.0); glVertex2f(xb0, yb1);
          glTexCoord2d(1.0, 1.0); glVertex2f(xb1, yb1);
          glTexCoord2d(1.0, 0.0); glVertex2f(xb1, yb0);
        glEnd();
      }
    }
  }
  else
  {
    //TODO: larger than max size textures
  }
}




} //end of namespace lpi

