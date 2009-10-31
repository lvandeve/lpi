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
}

//free the memory again
void TextureGL::deleteBuffer()
{
  buffer.clear();
}

void TextureGL::uploadPartial(int x0, int y0, int x1, int y1)
{
  if(!generated)
  {
    upload();
    return;
  }
  
  bind(false);
  //indicate how we'll read from buffer
  glPixelStorei( GL_UNPACK_ROW_LENGTH, u2);
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, x0);
  glPixelStorei( GL_UNPACK_SKIP_ROWS, y0);
    
  glTexSubImage2D(GL_TEXTURE_2D, 0, x0, y0, x1 - x0, y1 - y0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0] );
  
  //set back to default so that the rest behaves normally
  glPixelStorei( GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei( GL_UNPACK_SKIP_ROWS, 0);

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

void TextureGL::getTextAlignedBuffer(std::vector<unsigned char>& out)
{
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
