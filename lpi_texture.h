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

#ifndef LPI_TEXTURE_H_INCLUDED
#define LPI_TEXTURE_H_INCLUDED

#include <GL/gl.h>
#include <iostream>
#include <cmath>
#include <vector>

#include "lpi_color.h"
#include "lpi_screen.h"


namespace lpi
{

class ITexture
{
  public:
  
  /*
  Get and set the size of the texture. The x-coordinate or width is called "u",
  the y-coordinate or height is called "v".
  NOTE: after using setSize, the contents of the buffer are in an undefined
  state. You should then go set the pixel values to something.
  */
  virtual void setSize(size_t u, size_t v) = 0;
  virtual size_t getU() const = 0;
  virtual size_t getV() const = 0;
  
  /*
  getU32 and getV2 return the size of the buffer that you get with getBuffer,
  see the comments there for why this has to be used instead of getU and getV
  */
  virtual size_t getU2() const = 0;
  virtual size_t getV2() const = 0;
  
  /*
  getBuffers returns an editable buffer of the texture, consecutive in memory, with 4
  unsigned chars per pixel (R, G, B, A).
  The buffer does NOT have size (getU(), getV()), but (getU2(), getV2()). This is
  because how the texture is put in memory is implementation dependent. For example
  in the OpenGL texture, the buffer sizes are powers of two.
  You CAN change the contents of the line! This can be used to change colors,
  shapes, the image, ... on the texture. BUT: you need to call "update" after
  done changing, or the effect MAY or MAY NOT be visible depending on the
  implementation, platform, etc...
  */
  virtual unsigned char* getBuffer() = 0;
  virtual const unsigned char* getBuffer() const = 0;
  
  /*
  lets the implemenation know you changed some pixels of the texture,
  so that it can make the changes visible if that is needed (e.g. upload
  the texture again to video card memory). This may be inefficient so don't
  call it after every pixel you change.
  */
  virtual void update() = 0;
};

////////////////////////////////////////////////////////////////////////////////
//ALPHA/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//used for the createAlpha function
class AlphaEffect
{
  public:
  int style;
  unsigned char alpha;
  ColorRGB alphaColor;
  
  AlphaEffect(int style, unsigned char alpha = 0, const ColorRGB& alphaColor = RGB_Black);
};

#define AE_Opaque AlphaEffect(0, 255, lpi::RGB_Black) //no alpha effect
#define AE_GreenKey AlphaEffect(128, 255, lpi::RGB_Green) //make the green color invisible
#define AE_PinkKey AlphaEffect(128, 255, lpi::RGB_Magenta) //make the magenta color invisible
#define AE_PinkKeyS AlphaEffect(3200, 255, lpi::RGB_Magenta) //make the magenta color invisible, dark magenta (128) will become shadow, slightly brighter magenta (192) becomes more translucent shadow
#define AE_PinkKeyT25 AlphaEffect(3201, 64, lpi::RGB_Magenta) //make the magenta color invisible and the rest 25% visible + shadow effect
#define AE_PinkKeyT50 AlphaEffect(3201, 128, lpi::RGB_Magenta) //make the magenta color invisible and the rest 50% visible + shadow effect
#define AE_PinkKeyT75 AlphaEffect(3201, 192, lpi::RGB_Magenta) //make the magenta color invisible and the rest 75% visible + shadow effect
#define AE_BlackKey AlphaEffect(128, 255, lpi::RGB_Black) //make the black color invisible
#define AE_Translucent AlphaEffect(4, 255, lpi::RGB_Black) //Translucent: the darker, the more transparent
#define AE_ITranslucent AlphaEffect(68, 255, lpi::RGB_Black) //Inverse Translucent: the brighter, the more transparent
#define AE_Particle AlphaEffect(260, 255, lpi::RGB_White) //Use this for turning greyscale particles into 100% color where blackness is now translucency
#define AE_ColorParticle AlphaEffect(516, 255, lpi::RGB_White) //Same as AE_Particle, but hue and saturation information is preserved. 
#define AE_Modulated AlphaEffect(19, 255, lpi::RGB_White) //Modulated (special translucency, as in Unreal 1)
#define AE_IModulated AlphaEffect(20, 255, lpi::RGB_White) //Inverse Modulated

void createImageAlpha(unsigned char* image, int w, int h, const AlphaEffect& effect);

/*
Texture implemented with a simple buffer.
Maybe the name is confusing, but this is NOT a buffer of textures!
*/
class TextureBuffer : public ITexture
{
  private:
    std::vector<unsigned char> buffer;
    size_t u;
    size_t v;
    
  public:
  
  virtual void setSize(size_t u, size_t v);
  virtual size_t getU() const;
  virtual size_t getV() const;
  
  virtual size_t getU2() const;
  virtual size_t getV2() const;
  
  virtual unsigned char* getBuffer();
  virtual const unsigned char* getBuffer() const;
  
  virtual void update();
};

class TextureGL : public ITexture
{
  /*
  Important:
  
  There is only one texture uploaded to the videocard per texture object.
  That means that assigning or copying texture objects should not be done, unless the original one will be destroyed and only the new one will be used.
  */
  public:
  
    TextureGL(bool smoothing = false);
    
    virtual void setSize(size_t u, size_t v) { makeBuffer(u, v); }
    //width and height of the texture
    virtual size_t getU() const {return u;}
    virtual size_t getV() const {return v;}
    //width and height as powers of two (this will be the actual size of the buffer, because OpenGL only supports such textures)
    virtual size_t getU2() const {return u2;}
    virtual size_t getV2() const {return v2;}
    //multiply openGL texture coordinates between 0.0 and 1.0 with u3 and v3 to let OpenGL draw them correct even when not power of two
    double getU3() const {return u3;}
    double getV3() const {return v3;}
    
    virtual void update() { upload(); }
    
    virtual unsigned char* getBuffer()
    {
      return &buffer[0];
    }
    
    virtual const unsigned char* getBuffer() const
    {
      return &buffer[0];
    }
    
    void bind() const; //set this texture for OpenGL
  private:
    
    void makeBuffer(int u, int v); //creates memory for the buffer
    void deleteBuffer(); //deletes memory of the buffer

    void upload(); //sets the texture to openGL with correct datatype and such. Everytime something changes in the data in the buffer, upload it again to let the videocard/API know the changes. Also, use upload AFTER a screen is already set! And when the screen changes resolution, everything has to be uploaded again.
    void reupload(); //call this after you changed the screen (causing the textures to be erased from the video card)
    
    //get/set buffer that has the (possible non power of two) size of the wanted image (u * v RGBA pixels)
    void getAlignedBuffer(std::vector<unsigned char>& out);
    void setAlignedBuffer(const std::vector<unsigned char>& in);
    
    
    void operator*=(double a);
    
  private:
    bool smoothing;
    std::vector<unsigned char> buffer; //the buffer containing the image data in pc memory, can also be altered: just upload() the texture again to make the new result visible

    GLuint texture[1]; //the OpenGL texture datatype
    bool generated;
    //width and height of the texture
    size_t u;
    size_t v;
    //width and height as powers of two (this will be the actual size of the buffer, because OpenGL only supports such textures)
    size_t u2;
    size_t v2;
    //multiply openGL texture coordinates between 0.0 and 1.0 with u3 and v3 to let OpenGL draw them correct even when not power of two
    double u3; //should always be u / double(u2)
    double v3; //should always be v / double(v2)
};

//create the texture contents from some source (solid color, buffer, file)
void makeTextureSolid(ITexture* texture, const ColorRGB& color, size_t w, size_t h); //create texture with plain color of given size
void makeTextureFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, const AlphaEffect& effect = AE_Opaque, int x1=-1, int y1=-1, int x2=-1, int y2=-1); //w and h are the size of the buffer
void makeTextureFromPNGFile(ITexture* texture, const std::string& filename, const AlphaEffect& effect = AE_Opaque, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
//these affect only the alpha channel of the texture, e.g. use a greyscale PNG as alpha channel
void makeTextureAlphaFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
void makeTextureAlphaFromPNGFile(ITexture* texture, const std::string& filename, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
//this applies an alpha effect to the texture after it already has some contents
void applyAlphaEffect(ITexture* texture, const AlphaEffect& effect); //uses the setAlpha function in api.cpp on the buffer  


void loadTextures(std::vector<unsigned char>& buffer, std::vector<TextureGL>& textures, int widths, int heights, int w, int h, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTextures(const std::string& filename, std::vector<TextureGL>& textures, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTexturesAlpha(std::vector<unsigned char>& buffer, std::vector<TextureGL>& textures, int widths, int heights, int w, int h);
void loadTexturesAlpha(const std::string& filename, std::vector<TextureGL>& textures, int widths, int heights);
void loadTexturesFromBase64PNG(std::vector<TextureGL>& textures, const std::string& base64, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));

extern TextureGL emptyTexture; //default texture for initializing pointers

typedef TextureGL Texture; //TODO: remove this again once everything uses ITexture


} //namespace lpi


#endif
