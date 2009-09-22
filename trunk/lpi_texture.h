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

//create the texture contents from some source (solid color, buffer, file)
void makeTextureSolid(ITexture* texture, const ColorRGB& color, size_t w, size_t h); //create texture with plain color of given size
void makeTextureFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, const AlphaEffect& effect = AE_Opaque, int x1=-1, int y1=-1, int x2=-1, int y2=-1); //w and h are the size of the buffer
void makeTextureFromPNGFile(ITexture* texture, const std::string& filename, const AlphaEffect& effect = AE_Opaque, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
//these affect only the alpha channel of the texture, e.g. use a greyscale PNG as alpha channel
void makeTextureAlphaFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
void makeTextureAlphaFromPNGFile(ITexture* texture, const std::string& filename, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
//this applies an alpha effect to the texture after it already has some contents
void applyAlphaEffect(ITexture* texture, const AlphaEffect& effect); //uses the setAlpha function in api.cpp on the buffer  


void loadTextures(std::vector<unsigned char>& buffer, std::vector<ITexture*>& textures, const ITextureFactory* factory, int widths, int heights, int w, int h, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTextures(const std::string& filename, std::vector<ITexture*>& textures, const ITextureFactory* factory, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTexturesAlpha(std::vector<unsigned char>& buffer, std::vector<ITexture>& textures, int widths, int heights, int w, int h);
void loadTexturesAlpha(const std::string& filename, std::vector<ITexture>& textures, int widths, int heights);
void loadTexturesFromBase64PNG(std::vector<ITexture*>& textures, const ITextureFactory* factory, const std::string& base64, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));

extern ITexture* emptyTexture; //default texture for initializing pointers

} //namespace lpi

