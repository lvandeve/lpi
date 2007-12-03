 
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"

#include <vector>

namespace lpi
{

//Draw a line from (x1, y1) to (x2, y2) on the OpenGL screen
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
    glVertex2d(x1, y1);
    glVertex2d(x2, y2);
  glEnd();
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

//Draw an untextured, filled, rectangle on screen from (x1, y1) to (x2, y2)
void drawRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color)
{
  x2++;
  y2++;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
  
  setOpenGLScissor(); //everything that draws something must always do this

  glBegin(GL_QUADS);
    glVertex3d(x2, y1, 1);
    glVertex3d(x1, y1, 1);
    glVertex3d(x1, y2, 1);
    glVertex3d(x2, y2, 1);
  glEnd();
}

//Draw a rectangle with 4 different corner colors on screen from (x1, y1) to (x2, y2)
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

void drawDisk(int x, int y, double radius, const ColorRGB& color)
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
      glVertex3d(x + std::cos(angle) * radius, y + std::sin(angle) * radius, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radius, y, 1);
  glEnd();
}


void drawGradientDisk(int x, int y, double radius, const ColorRGB& color1, const ColorRGB& color2)
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
      
      glVertex3d(x + std::cos(angle) * radius, y + std::sin(angle) * radius, 1);
      angle += 2 * pi / numsegments;
    }
    glVertex3d(x + radius, y, 1);
  glEnd();
}
} //end of namespace lpi

