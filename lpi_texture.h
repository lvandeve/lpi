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


namespace lpi
{


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


class Texture
{
  /*
  Important:
  
  There is only one texture uploaded to the videocard per texture object.
  That means that assigning or copying texture objects should not be done, unless the original one will be destroyed and only the new one will be used.
  */
  public:
    
    int getU() const {return u;}
    int getV() const {return v;}
    int getU2() const {return u2;}
    int getV2() const {return v2;}
    double getU3() const {return u3;}
    double getV3() const {return v3;}
    
    void makeBuffer(int u, int v); //creates memory for the buffer
    void deleteBuffer(); //deletes memory of the buffer
    
    //load functions will make the buffer if it's still a null pointer
    void create(const ColorRGB& color, int w, int h); //create texture with plain color of given size
    void create(unsigned char* buffer, int w, int h, const AlphaEffect& effect = AE_Opaque, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
    void create(const std::string& filename, const AlphaEffect& effect = AE_Opaque, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
    void createAlpha(unsigned char* buffer, int w, int h, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
    void createAlpha(const std::string& filename, int x1=-1, int y1=-1, int x2=-1, int y2=-1);
    void calculateAlpha(const AlphaEffect& effect); //uses the setAlpha function in api.cpp on the buffer  

    
    //2D drawing of textures
    //do not draw the texture before binding it
    //factors "scale" are allowed to be negative to obtain mirrored texture
    void draw(int x, int y, const ColorRGB& colorMod = RGB_White, int sizex = -1, int sizey = -1, int skewx = 0, int skewy = 0) const;
    void draw(int x, int y, double scale, const ColorRGB& colorMod = RGB_White) const;
    void draw(int x, int y, double scalex, double scaley, const ColorRGB& colorMod = RGB_White) const;
    void draw(int x1, int y1, int x2, int y2, const ColorRGB& colorMod = RGB_White) const;
    void drawCentered(int x, int y, const ColorRGB& colorMod = RGB_White, int sizex = -1, int sizey = -1, int skewx = 0, int skewy = 0) const;
    void drawCentered(int x, int y, double scale, const ColorRGB& colorMod = RGB_White) const;
    void drawCentered(int x, int y, double scalex, double scaley, const ColorRGB& colorMod = RGB_White) const;
    void drawRepeated(int x1, int y1, int x2, int y2, double scalex = 1.0, double scaley = 1.0, const ColorRGB& colorMod = RGB_White) const;
    
    unsigned char getPixel(int x, int y, int c) const
    {
      return buffer[4 * u2 * y + 4 * x + c];
    }
    
    ColorRGB getPixel(int x, int y) const
    {
      return ColorRGB(getPixel(x, y, 0), getPixel(x, y, 1), getPixel(x, y, 2), getPixel(x, y, 3));
    }
    
    void setPixel(int x, int y, const ColorRGB& color)
    {
      buffer[4 * u2 * y + 4 * x + 0] = color.r;
      buffer[4 * u2 * y + 4 * x + 1] = color.g;
      buffer[4 * u2 * y + 4 * x + 2] = color.b;
      buffer[4 * u2 * y + 4 * x + 3] = color.a;
    }
    
    void bind() const; //set this texture for OpenGL
    
    Texture();
    
    void upload(); //sets the texture to openGL with correct datatype and such. Everytime something changes in the data in the buffer, upload it again to let the videocard/API know the changes. Also, use upload AFTER a screen is already set! And when the screen changes resolution, everything has to be uploaded again.
    void reupload(); //call this after you changed the screen (causing the textures to be erased from the video card)
    
    void getAlignedBuffer(std::vector<unsigned char>& out)
    {
      out = buffer;
    }
    
    void setAlignedBuffer(const std::vector<unsigned char>& in)
    {
      buffer = in;
    }
    
    std::vector<unsigned char>& getOpenGLBuffer()
    {
      return buffer;
    }
    
    std::vector<unsigned char>& getBuffer() { return buffer; }
    const std::vector<unsigned char>& getBuffer() const { return buffer; }
    
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
    //multiply openGL texture coordinates between 0.0 and 1.0 with u3 and v3 to let OpenGL draw them correct even when not power of two
    double u3; //should always be u / double(u2)
    double v3; //should always be v / double(v2)
};

void loadTextures(std::vector<unsigned char>& buffer, std::vector<Texture>& textures, int widths, int heights, int w, int h, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTextures(const std::string& filename, std::vector<Texture>& textures, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));
void loadTexturesAlpha(std::vector<unsigned char>& buffer, std::vector<Texture>& textures, int widths, int heights, int w, int h);
void loadTexturesAlpha(const std::string& filename, std::vector<Texture>& textures, int widths, int heights);
void loadTexturesFromBase64PNG(std::vector<Texture>& textures, const std::string& base64, int widths, int heights, const AlphaEffect& effect = AlphaEffect(0, 0, RGB_Black));

extern Texture emptyTexture; //default texture for initializing pointers


} //namespace lpi


#endif
