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

#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_gl.h"
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
    glVertex2d(x1 + 0.5, y1 + 0.5);
    glVertex2d(x2 + 0.5, y2 + 0.5);
  glEnd();
}
void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color)
{
  drawLine(x1, y1, x2, y2, color, 0, 0, screenWidth(), screenHeight());
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

