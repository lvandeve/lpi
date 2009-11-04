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
#include "lpi_base64.h"

#include <algorithm>
#include <iostream>

namespace lpi
{

TextureGL::TextureGL()
: partsx(1)
, partsy(1)
{
  deleteBuffer();
}

//make memory for the buffer of the texture
void TextureGL::makeBuffer()
{
  if(u <= MAXX && v <= MAXY)
  {
    partsx = partsy = 1;
    
    //find first larger power of two of width and store it in u2
    u2 = 1;
    while(u2 < u) u2 *= 2;
    
    //find first larger power of two of height and store it in v2
    v2 = 1;
    while(v2 < v) v2 *= 2;
    
    
    parts.resize(1);
    Part& part = parts[0];
    part.shiftx = 0;
    part.shifty = 0;
    part.u = u;
    part.v = v;
    part.u2 = u2;
    part.v2 = v2;
  }
  else
  {
    partsx = (u + MAXX - 1) / MAXX;
    partsy = (v + MAXY - 1) / MAXY;
    
    size_t newsize = partsx * partsy;
    for(size_t i = newsize; i < parts.size(); i++)
    {
      glDeleteTextures(1, &parts[i].texture);
    }

    parts.resize(partsx * partsy); //TODO: clean up old textures from the video card if some disappear
    for(size_t y = 0; y < partsy; y++)
    for(size_t x = 0; x < partsx; x++)
    {
      size_t index = y * partsx + x;
      Part& part = parts[index];
      part.shiftx = x * MAXX;
      part.shifty = y * MAXY;
      part.u = MAXX;
      if(x == partsx - 1) part.u = (u % MAXX == 0) ? MAXX : u % MAXX;
      part.v = MAXY;
      if(y == partsy - 1) part.v = (v % MAXY == 0) ? MAXY : v % MAXY;
      part.u2 = MAXX;
      part.v2 = MAXY;
    }
    
    u2 = partsx * MAXX;
    v2 = partsy * MAXY;
  }
  
  buffer.resize(4 * u2 * v2);
}

//free the memory again
void TextureGL::deleteBuffer()
{
  buffer.clear();
}

void TextureGL::uploadPartial(int x0, int y0, int x1, int y1)
{
  for(size_t i = 0; i < parts.size(); i++)
  {
    if(!parts[i].generated)
    {
      upload();
      return;
    }
  }
  
 
  if(u <= MAXX && v <= MAXY)
  {
    bind(false, 0);
      
    //indicate how we'll read from buffer
    glPixelStorei( GL_UNPACK_ROW_LENGTH, u2);
    glPixelStorei( GL_UNPACK_SKIP_PIXELS, x0);
    glPixelStorei( GL_UNPACK_SKIP_ROWS, y0);

    glTexSubImage2D(GL_TEXTURE_2D, 0, x0, y0, x1 - x0, y1 - y0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0] );

  }
  else
  {
    for(size_t i = 0; i < parts.size(); i++)
    {
      Part& part = parts[i];
      
      bool xoverlap = false;
      if(x0 >= part.shiftx && x0 <= (int)(part.shiftx + part.u)) xoverlap = true;
      if(x1 >= part.shiftx && x1 <= (int)(part.shiftx + part.u)) xoverlap = true;
      bool yoverlap = false;
      if(y0 >= part.shifty && y0 <= (int)(part.shifty + part.v)) yoverlap = true;
      if(y1 >= part.shifty && y1 <= (int)(part.shifty + part.v)) yoverlap = true;

      if(xoverlap && yoverlap)
      {
        bind(false, i);
        
        int px0 = x0 - part.shiftx; if(px0 < 0) px0 = 0; /*if(px0 > (int)part.u) px0 = part.u;*/
        int py0 = y0 - part.shifty; if(py0 < 0) py0 = 0; /*if(py0 > (int)part.v) py0 = part.v;*/
        int px1 = x1 - part.shiftx; /*if(px1 < 0) px1 = 0;*/ if(px1 > (int)part.u) px1 = part.u;
        int py1 = y1 - part.shifty; /*if(py1 < 0) py1 = 0;*/ if(py1 > (int)part.v) py1 = part.v;
        
        //indicate how we'll read from buffer
        glPixelStorei( GL_UNPACK_ROW_LENGTH, u2);
        glPixelStorei( GL_UNPACK_SKIP_PIXELS, part.shiftx + px0);
        glPixelStorei( GL_UNPACK_SKIP_ROWS, part.shifty + py0);
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, px0, py0, px1 - px0, py1 - py0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0] );
        
      }
    }
  }
  
  //set back to default so that the rest behaves normally
  glPixelStorei( GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei( GL_UNPACK_SKIP_ROWS, 0);

}

//This generates the OpenGL texture so that OpenGL can use it, also use after changing the texture buffer
void TextureGL::upload()
{
  for(size_t i = 0; i < parts.size(); i++)
  {
    Part& part = parts[i];
    if(!part.generated)
    {
      glGenTextures(1, &part.texture);
      part.generated = true;
    }
  }
  
  if(u <= MAXX && v <= MAXY)
  {
    bind(false, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0]);

  }
  else
  {
    for(size_t i = 0; i < parts.size(); i++)
    {
      Part& part = parts[i];
      bind(false, i);
      glPixelStorei( GL_UNPACK_ROW_LENGTH, u2);
      glPixelStorei( GL_UNPACK_SKIP_PIXELS, part.shiftx);
      glPixelStorei( GL_UNPACK_SKIP_ROWS, part.shifty);
      glTexImage2D(GL_TEXTURE_2D, 0, 4, part.u2, part.v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0]);
    }
    //set back to default so that the rest behaves normally
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei( GL_UNPACK_SKIP_ROWS, 0);
  }
}

void TextureGL::reupload()
{
  for(size_t i = 0; i < parts.size(); i++) parts[i].generated = false;
  upload();
}

//make this the selected one for drawing
void TextureGL::bind(bool smoothing, size_t index) const
{
  glBindTexture(GL_TEXTURE_2D, parts[index].texture);
  
  if(smoothing) //todo: remove this from the "bind" function, do it in generate or separatly instead
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
