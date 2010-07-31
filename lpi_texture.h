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

#include <iostream>
#include <cmath>
#include <vector>

#include "lpi_color.h"


namespace lpi
{

class ITexture
{
  public:
  
  ITexture();
  virtual ~ITexture();
  
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
  So the size (in chars) of the buffer is: 4 * getU2() * getV2().
  You CAN change the contents of the buffer. This can be used to change colors,
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
  
  /*
  indicate only the part in the given rectangular zone changed, some implementations
  can perform this more efficiently than updating *everything* if  only a small part changed.
  x1 and y1 are the end coordinates of the rectangular area and are *not* inclusive.
  */
  virtual void updatePartial(int x0, int y0, int x1, int y1) = 0;
};

/*
HTexture is a utility for the following situation:
GUI elements cannot contain a texture implementation directly, because depending
on the drawer given to its draw function, a different type of texture is supported
and the ITexture* pointer may be assigned to a new texture created by the drawer.
So for icons and such in GUI, in GUI elements that use the icon, use a HTexture*
inside the element (not a HTexture but a HTexture*).

Example of drawing one with a IDrawer2D:

HTexture* icon = ........;
if(icon && icon->texture)
{
  drawer.convertTextureIfNeeded(icon->texture);
  drawer.drawTexture(icon->texture, x, y);
}

Never copy a HTexture directly if the texture member isn't NULL. It has a
refcount but is far from as powerful as a shared pointer. Treat it as
an auto_ptr. It works in std::vectors however, which is the point of the refcount.
*/
class HTexture
{
  private:
    int* refcount;
    
  public:

    ITexture* texture;
  
    HTexture();
    HTexture(const HTexture& other);
    ~HTexture();
};

/*
Texture implemented with a simple buffer.
Maybe the name is confusing, but this is NOT a buffer of textures!

TextureBuffer is also the class you should use to initialize a texture with
data while you don't yet know what drawer will draw it, since this texture class
can contain the complete data but doesn't have any dependencies.
*/
class TextureBuffer : public ITexture
{
  private:
    std::vector<unsigned char> buffer;
    size_t u;
    size_t v;

  public:

    TextureBuffer();

    virtual void setSize(size_t u, size_t v);
    virtual size_t getU() const;
    virtual size_t getV() const;

    virtual size_t getU2() const;
    virtual size_t getV2() const;

    virtual unsigned char* getBuffer();
    virtual const unsigned char* getBuffer() const;

    virtual void update();
    virtual void updatePartial(int x0, int y0, int x1, int y1);
};

/*
ITextureFactory: used to create textures of some type, for generic things
*/
class ITextureFactory
{
  public:
    virtual ~ITextureFactory(){};
    virtual ITexture* createNewTexture() const = 0;
};

/*
Default implementation of texturefactory: templatized for convenience
*/
template<typename T> //T must be a subclass of ITexture
class TextureFactory : public ITextureFactory
{
  public:
    virtual T* createNewTexture() const { return new T; }
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

static const AlphaEffect AE_Nothing(0, 255, RGB_Black); //no alpha effect
static const AlphaEffect AE_Opaque(1, 255, RGB_Black); //make alpha channel opaque
static const AlphaEffect AE_GreenKey(128, 255, RGB_Green); //make the green color invisible
static const AlphaEffect AE_PinkKey(128, 255, RGB_Magenta); //make the magenta color invisible
static const AlphaEffect AE_PinkKeyS(3200, 255, RGB_Magenta); //make the magenta color invisible, dark magenta (128) will become shadow, slightly brighter magenta (192) becomes more translucent shadow
static const AlphaEffect AE_PinkKeyT25(3201, 64, RGB_Magenta); //make the magenta color invisible and the rest 25% visible + shadow effect
static const AlphaEffect AE_PinkKeyT50(3201, 128, RGB_Magenta); //make the magenta color invisible and the rest 50% visible + shadow effect
static const AlphaEffect AE_PinkKeyT75(3201, 192, RGB_Magenta); //make the magenta color invisible and the rest 75% visible + shadow effect
static const AlphaEffect AE_BlackKey(128, 255, RGB_Black); //make the black color invisible
static const AlphaEffect AE_Translucent(4, 255, RGB_Black); //Translucent: the darker, the more transparent
static const AlphaEffect AE_ITranslucent(68, 255, RGB_Black); //Inverse Translucent: the brighter, the more transparent
static const AlphaEffect AE_Particle(260, 255, RGB_White); //Use this for turning greyscale particles into 100% color where blackness is now translucency
static const AlphaEffect AE_ColorParticle(516, 255, RGB_White); //Same as AE_Particle, but hue and saturation information is preserved. 
static const AlphaEffect AE_Modulated(19, 255, RGB_White); //Modulated (special translucency, as in Unreal 1)
static const AlphaEffect AE_IModulated(20, 255, RGB_White); //Inverse Modulated
static const AlphaEffect AE_Saturation(8, 255, RGB_White); //Saturation to alpha

void createImageAlpha(unsigned char* image, int w, int h, const AlphaEffect& effect);

///utility functions

//create the texture contents from some source (solid color, buffer, file)
void makeTextureSolid(ITexture* texture, const ColorRGB& color, size_t w, size_t h); //create texture with plain color of given size
void makeTextureFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, const AlphaEffect& effect = AE_Nothing, int x1=-1, int y1=-1, int x2=-1, int y2=-1); //w and h are the size of the buffer
void makeTextureFromPNGFile(ITexture* texture, const std::string& filename, const AlphaEffect& effect = AE_Nothing, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
//these affect only the alpha channel of the texture, e.g. use a greyscale PNG as alpha channel
void makeTextureAlphaFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
void makeTextureAlphaFromPNGFile(ITexture* texture, const std::string& filename, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
//this applies an alpha effect to the texture after it already has some contents
void applyAlphaEffect(ITexture* texture, const AlphaEffect& effect); //uses the setAlpha function in api.cpp on the buffer  


void loadTextures(std::vector<unsigned char>& buffer, std::vector<ITexture*>& textures, const ITextureFactory* factory, int widths, int heights, int w, int h, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTextures(const std::string& filename, std::vector<ITexture*>& textures, const ITextureFactory* factory, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTexturesFromBase64PNG(std::vector<ITexture*>& textures, const ITextureFactory* factory, const std::string& base64, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTextureFromBase64PNG(ITexture* texture, const std::string& base64, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));

//DANGER! these functions create an std::vector<HTexture>, where HTexture's are like auto_ptr's. Never resize the std::vectors after it was created in this function.
void loadTextures(std::vector<unsigned char>& buffer, std::vector<HTexture>& textures, const ITextureFactory* factory, int widths, int heights, int w, int h, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTextures(const std::string& filename, std::vector<HTexture>& textures, const ITextureFactory* factory, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));


ITexture* getDefaultEmptyTexture();

//aligned buffer = having the actual u*v size, instead of the u2*v2 size internally in the texture
void getAlignedBuffer(std::vector<unsigned char>& buffer, const ITexture* texture);
void setAlignedBuffer(ITexture* texture, const unsigned char* buffer); //assumes texture has already correct size and buffer contains that many pixels

void createTexture(ITexture* texture, size_t w, size_t h, const ColorRGB& color);

void setPixel(ITexture* texture, int x, int y, const ColorRGB& color);
ColorRGB getPixel(const ITexture* texture, int x, int y);

void copyTexture(ITexture* dest, const ITexture* source);

} //namespace lpi

