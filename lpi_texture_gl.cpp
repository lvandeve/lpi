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

#include "lpi_texture_gl.h"

#include "lodepng.h"
//#include "lpi_screen_gl.h"
#include "lpi_base64.h"

#include <algorithm>
#include <iostream>

namespace lpi
{

TextureGL::TextureGL()
{
  deleteBuffer();
  generated = false;
}

void TextureGL::operator*=(double a)
{
  for(size_t y = 0; y < v2; y++)
  for(size_t x = 0; x < u2; x++)
  for(size_t c = 0; c < 3; c++)
  {
    buffer[4 * u2 * y + 4 * x + c] = (unsigned char)(buffer[4 * u2 * y + 4 * x + c] * a);
  }
  
  upload();
}

//make memory for the buffer of the texture
void TextureGL::makeBuffer(int u, int v)
{
  //find first larger power of two of width and store it in u2
  int u2 = 1;
  while(u2 < u) u2 *= 2;
  
  //find first larger power of two of height and store it in v2
  int v2 = 1;
  while(v2 < v) v2 *= 2;
  
  buffer.resize(4 * u2 * v2);
  
  this->u = u;
  this->v = v;
  this->u2 = u2;
  this->v2 = v2;
  this->u3 = u / double(u2);
  this->v3 = v / double(v2);
}

//free the memory again
void TextureGL::deleteBuffer()
{
  buffer.clear();
}

//This generates the OpenGL texture so that OpenGL can use it, also use after changing the texture buffer
void TextureGL::upload()
{
  if(!generated)
  {
    glGenTextures(1, texture);
    generated = true;
  }
  bind(false);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0]);
}

void TextureGL::reupload()
{
  generated = false;
  upload();
}

//make this the selected one for drawing
void TextureGL::bind(bool smoothing) const
{
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  if(smoothing)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
}

////Draw the texture on screen
//void TextureGL::draw(int x, int y, const ColorRGB& colorMod, int sizex, int sizey, int skewx, int skewy) const
//{
//  if(sizex < 0) sizex = u;
//  if(sizey < 0) sizey = v;
//  if(sizex == 0 || sizey == 0) return;
//  glEnable(GL_TEXTURE_2D);
//
//  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
//  bind();
//
//  setOpenGLScissor(); //everything that draws something must always do this
//
//  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
//  glBegin(GL_QUADS);
//    glTexCoord2d(0.0, 0.0); glVertex2d(x + 0     + skewx, y + 0     + 0    );
//    glTexCoord2d(+u3, 0.0); glVertex2d(x + sizex + skewx, y + 0     + skewy);
//    glTexCoord2d(+u3, +v3); glVertex2d(x + sizex + 0    , y + sizey + skewy);
//    glTexCoord2d(0.0, +v3); glVertex2d(x + 0     + 0    , y + sizey + 0    );
//  glEnd();
//}
//
////Draw the texture on screen
//void TextureGL::draw(int x1, int y1, int x2, int y2, const ColorRGB& colorMod) const
//{
//  if(x2 - x1 == 0 || y2 - y1 == 0) return;
//  glEnable(GL_TEXTURE_2D);
//
//  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
//  bind();
//
//  setOpenGLScissor(); //everything that draws something must always do this
//
//  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
//  glBegin(GL_QUADS);
//    glTexCoord2d(0.0, 0.0); glVertex3d(x1, y1, 1);
//    glTexCoord2d(+u3, 0.0); glVertex3d(x2, y1, 1);
//    glTexCoord2d(+u3, +v3); glVertex3d(x2, y2, 1);
//    glTexCoord2d(0.0, +v3); glVertex3d(x1, y2 , 1);
//  glEnd();
//}
//
//void TextureGL::draw(int x, int y, double scalex, double scaley, const ColorRGB& colorMod) const
//{
//  double sizex = u * scalex;
//  double sizey = v * scaley;
//  if(sizex == 0.0 || sizey == 0.0) return;
//  glEnable(GL_TEXTURE_2D);
//
//  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
//  bind();
//
//  setOpenGLScissor(); //everything that draws something must always do this
//
//  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
//  glBegin(GL_QUADS);
//    glTexCoord2d(0.0, 0.0); glVertex3d(x + 0  , y + 0  , 1);
//    glTexCoord2d(+u3, 0.0); glVertex3d(x + sizex, y + 0  , 1);
//    glTexCoord2d(+u3, +v3); glVertex3d(x + sizex, y + sizey, 1);
//    glTexCoord2d(0.0, +v3); glVertex3d(x + 0  , y + sizey, 1);
//  glEnd();
//}
//
//void TextureGL::draw(int x, int y, double scale, const ColorRGB& colorMod) const
//{
//  draw(x, y, scale, scale, colorMod);
//}
//
////Draw the texture on screen where x and y are the center instead of the top left corner
//void TextureGL::drawCentered(int x, int y, const ColorRGB& colorMod, int sizex, int sizey, int skewx, int skewy) const
//{
//  if(sizex < 0) sizex = u;
//  if(sizey < 0) sizey = v;
//  if(sizex == 0 || sizey == 0) return;
//  draw(x - sizex / 2, y - sizey / 2, colorMod, sizex, sizey, skewx, skewy);
//}
//
//void TextureGL::drawCentered(int x, int y, double scalex, double scaley, const ColorRGB& colorMod) const
//{
//  int sizex = int(u * scalex);
//  int sizey = int(v * scaley);
//  if(sizex == 0 || sizey == 0) return;
//  draw(x - sizex / 2, y - sizey / 2, scalex, scaley, colorMod);
//}
//
//void TextureGL::drawCentered(int x, int y, double scale, const ColorRGB& colorMod) const
//{
//  drawCentered(x, y, scale, scale, colorMod);
//}
//
///*
//draws the texture repeated, useful to fill something with a tilable texture
//The texture size must be a power of 2, or you get ugly black things in between
//scale is a scaling factor of the texture
//*/
//void TextureGL::drawRepeated(int x1, int y1, int x2, int y2, double scalex, double scaley, const ColorRGB& colorMod) const
//{
//  glEnable(GL_TEXTURE_2D);
//
//  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
//  bind();
//
//  setOpenGLScissor(); //everything that draws something must always do this
//  
//  double coorx = (double(x2 - x1) / u) / scalex;//(u3 * (x2 - x1)) / scalex;
//  double coory = (double(y2 - y1) / v) / scaley; //(v3 * (y2 - y1)) / scaley;
//
//  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
//  glBegin(GL_QUADS);
//    glTexCoord2d(0.0,       0.0); glVertex3d(x1, y1, 1);
//    glTexCoord2d(0.0,    +coory); glVertex3d(x1, y2, 1);
//    glTexCoord2d(+coorx, +coory); glVertex3d(x2, y2, 1);
//    glTexCoord2d(+coorx,    0.0); glVertex3d(x2, y1, 1);
//  glEnd();
//}

void TextureGL::getTextAlignedBuffer(std::vector<unsigned char>& out)
{
  //out = buffer;
  out.clear();
  for(size_t y = 0; y < v; y++)
  {
    out.insert(out.end(), buffer.begin() + 4 * y * u2, buffer.begin() + 4 * y * u2 + 4 * u);
  }
}

void TextureGL::setTextAlignedBuffer(const std::vector<unsigned char>& in)
{
  for(size_t y = 0; y < v; y++)
  {
    std::copy(in.begin() + 4 * y * u, in.begin() + 4 * y * u + 4 * u, buffer.begin() + 4 * y * u2);
  }
}



} //namespace lpi
