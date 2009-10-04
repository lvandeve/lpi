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

#include <GL/gl.h>
#include <iostream>

#include "lpi_texture.h"
//#include "lpi_screen_gl.h"


namespace lpi
{

class TextureGL : public ITexture
{
  /*
  Important:
  
  There is only one texture uploaded to the videocard per texture object.
  That means that assigning or copying texture objects should not be done, unless the original one will be destroyed and only the new one will be used.
  */
  public:
  
    TextureGL();
    
    virtual void setSize(size_t u, size_t v) { makeBuffer(u, v); }
    //width and height of the texture
    virtual size_t getU() const {return u;}
    virtual size_t getV() const {return v;}
    //width and height as powers of two (this will be the actual size of the buffer, because OpenGL only supports such textures)
    virtual size_t getU2() const {return u2;}
    virtual size_t getV2() const {return v2;}
    
    virtual void update() { upload(); }
    
    virtual unsigned char* getBuffer()
    {
      return &buffer[0];
    }
    
    virtual const unsigned char* getBuffer() const
    {
      return &buffer[0];
    }
    
    void bind(bool smoothing) const; //set this texture for OpenGL
  private:
    
    void makeBuffer(int u, int v); //creates memory for the buffer
    void deleteBuffer(); //deletes memory of the buffer

    void upload(); //sets the texture to openGL with correct datatype and such. Everytime something changes in the data in the buffer, upload it again to let the videocard/API know the changes. Also, use upload AFTER a screen is already set! And when the screen changes resolution, everything has to be uploaded again.
    void reupload(); //call this after you changed the screen (causing the textures to be erased from the video card)
    
    //get/set buffer that has the (possible non power of two) size of the wanted image (u * v RGBA pixels)
    void getTextAlignedBuffer(std::vector<unsigned char>& out);
    void setTextAlignedBuffer(const std::vector<unsigned char>& in);
    
    
    void operator*=(double a);
    
  private:
    std::vector<unsigned char> buffer; //the buffer containing the image data in pc memory, can also be altered: just upload() the texture again to make the new result visible

    GLuint texture[1]; //the OpenGL texture datatype
    bool generated;
    //width and height of the texture
    size_t u;
    size_t v;
    //width and height as powers of two (this will be the actual size of the buffer, because OpenGL only supports such textures)
    size_t u2;
    size_t v2;
};

} //namespace lpi

