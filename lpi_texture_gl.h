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
#include "lpi_gl_context.h"

#include <SDL/SDL.h>


namespace lpi
{

//TODO: graphics cards can't handle textures larger than ... * ... So divide a large texture into multiple 512*512 parts.
class TextureGL : public ITexture
{
  /*
  Important:
  
  There is only one texture uploaded to the videocard per texture object.
  That means that assigning or copying texture objects should not be done, unless the original one will be destroyed and only the new one will be used.
  */
  
  public:
  
    struct Part
    {
      int generated_id; //-1 if not generated, id of GL context otherwise
      GLuint texture; //the unique OpenGL texture "name" to identify ourselves
      size_t u;
      size_t v;
      //width and height as powers of two (this will be the actual size of the buffer, because OpenGL only supports such textures)
      size_t u2;
      size_t v2;
      
      GLContext* context;
      
      int shiftx; //shift compared to topleft of main texture
      int shifty; //shift compared to topleft of main texture
      
      Part(GLContext* context);
      ~Part();
    };
  
  private:
    std::vector<unsigned char> buffer; //the buffer containing the image data in pc memory, can also be altered: just upload() the texture again to make the new result visible
    
    //width and height of the texture
    size_t u;
    size_t v;
    //width and height as powers of two (this will be the actual size of the buffer, because OpenGL only supports such textures)
    size_t u2;
    size_t v2;
    
    mutable std::vector<Part> parts;
    
    GLContext* context;
    
    
  public:

    //divide texture in multiple ones if size is larger than this so that all video hardware can support it
    static const size_t MAXX = 1024;
    static const size_t MAXY = 1024;
  
    TextureGL(GLContext* context);
    ~TextureGL();
    
    virtual void setSize(size_t u, size_t v) { this->u = u; this->v = v; makeBuffer(); }
    //width and height of the texture
    virtual size_t getU() const {return u;}
    virtual size_t getV() const {return v;}
    //width and height as powers of two (this will be the actual size of the buffer, because OpenGL only supports such textures)
    virtual size_t getU2() const {return u2;}
    virtual size_t getV2() const {return v2;}
    
    virtual void update() { upload(); }
    virtual void updatePartial(int x0, int y0, int x1, int y1) { uploadPartial(x0, y0, x1, y1); }
    
    virtual unsigned char* getBuffer()
    {
      return &buffer[0];
    }
    
    virtual const unsigned char* getBuffer() const
    {
      return &buffer[0];
    }
    
    void bind(bool smoothing, size_t index) const; //set this texture for OpenGL
    
    size_t getNumParts() const { return parts.size(); }
    const Part& getPart(size_t i) const { return parts[i]; }
    
    /*
    updateForNewOpenGLContextIfNeeded: If the GL context changed (previous destroyed, new one created),
    the texture needs to be reuploaded. If this function is called, the texture will detect that it
    has been destroyed by checking the context member.
    */
    bool updateForNewOpenGLContextIfNeeded() const;
    
  private:
    
    void makeBuffer(); //creates memory for the buffer

    void uploadPartial(int x0, int y0, int x1, int y1); //todo: make use of this for efficiency
    void upload() const; //sets the texture to openGL with correct datatype and such. Everytime something changes in the data in the buffer, upload it again to let the videocard/API know the changes. Also, use upload AFTER a screen is already set! And when the screen changes resolution, everything has to be uploaded again.
    void reupload() const; //call this after you changed the screen (causing the textures to be erased from the video card)
    
    //get/set buffer that has the (possible non power of two) size of the wanted image (u * v RGBA pixels)
    void getTextAlignedBuffer(std::vector<unsigned char>& out);
    void setTextAlignedBuffer(const std::vector<unsigned char>& in);
};

/*
Default implementation of texturefactory: templatized for convenience
*/
class TextureFactoryGL : public ITextureFactory
{
  private:
    GLContext* context;
  public:
    TextureFactoryGL(GLContext* context) : context(context) {};
    virtual TextureGL* createNewTexture() const { return new TextureGL(context); }
};

} //namespace lpi

