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

#include "lpi_texture.h"

#include "lodepng.h"
#include "lpi_base64.h"

#include <algorithm>
#include <iostream>

namespace lpi
{

ITexture::ITexture()
{
}

ITexture::~ITexture()
{
}

void loadTextures(std::vector<unsigned char>& buffer, std::vector<ITexture*>& textures, const ITextureFactory* factory, int widths, int heights, int w, int h, const AlphaEffect& effect)
{
  int numx, numy;
   
  if(widths > 0 && heights > 0)
  {
    numx = w / widths;
    numy = h / heights;
  }
  else
  {
    numx = 1;
    numy = 1;
    widths = w;
    heights = h;
  }
  
  //the order is row per row, column per column
  for(int y = 0; y < numy; y++)
  for(int x = 0; x < numx; x++)
  {
    textures.resize(textures.size() + 1);
    textures.back() = factory->createNewTexture();
    makeTextureFromBuffer(textures.back(), &buffer[0], w, h, effect, x * widths, y * heights, (x + 1) * widths, (y + 1) * heights);
  }
} 

/*
*/
void loadTextures(const std::string& filename, std::vector<ITexture*>& textures, const ITextureFactory* factory, int widths, int heights, const AlphaEffect& effect)
{
  LodePNG::Decoder pngdec;
  
  std::vector<unsigned char> file;
  LodePNG::loadFile(file, filename);

  std::vector<unsigned char> image;
  
  //load the png and if it gives an error return it
  pngdec.decode(image, file);
  if(pngdec.hasError())
  {
    std::cout << "\npng loading error " << pngdec.getError() << " in file " << filename;
    return;
  }
  
  loadTextures(image, textures, factory, widths, heights, pngdec.getWidth(), pngdec.getHeight(), effect);
}

void loadTextures(std::vector<unsigned char>& buffer, std::vector<HTexture>& textures, const ITextureFactory* factory, int widths, int heights, int w, int h, const AlphaEffect& effect)
{
  int numx, numy;
   
  if(widths > 0 && heights > 0)
  {
    numx = w / widths;
    numy = h / heights;
  }
  else
  {
    numx = 1;
    numy = 1;
    widths = w;
    heights = h;
  }
  
  textures.clear(); //call the destructors of possible earlier HTextures in it, to avoid memory leaks
  textures.resize(numx * numy);
  
  //the order is row per row, column per column
  for(int y = 0; y < numy; y++)
  for(int x = 0; x < numx; x++)
  {
    HTexture& tex = textures[y * numx + x];
    tex.texture = factory->createNewTexture();
    makeTextureFromBuffer(tex.texture, &buffer[0], w, h, effect, x * widths, y * heights, (x + 1) * widths, (y + 1) * heights);
  }
} 

void loadTextures(const std::string& filename, std::vector<HTexture>& textures, const ITextureFactory* factory, int widths, int heights, const AlphaEffect& effect)
{
  LodePNG::Decoder pngdec;
  
  std::vector<unsigned char> file;
  LodePNG::loadFile(file, filename);

  std::vector<unsigned char> image;
  
  //load the png and if it gives an error return it
  pngdec.decode(image, file);
  if(pngdec.hasError())
  {
    std::cout << "\npng loading error " << pngdec.getError() << " in file " << filename;
    return;
  }
  
  loadTextures(image, textures, factory, widths, heights, pngdec.getWidth(), pngdec.getHeight(), effect);
}

//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////
//ALPHA/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//

/*
The function createImageAlpha takes an image buffer and gives it an alpha channel.
For example, it can give an image an alpha channel to make darker pixels more
translucent, brighter pixels more translucent, give the image an invisible color
(a color key), etc...

The AlphaEffect effect determines how the alpha channel of the image is calculated:
AlphaEffect contains a color, an alpha value, and the integer style:
Concerning the integer style:
  The 6 least significant bits are used to ask for a certain effect.
  The other bits can be turned on or off to modify the effect in a certain way.
The RGB color color decides what pixels to perform the effect on.
The alpha value decides what to set pixels to for effects that need this.

Styles of the integer "style":

In the explanation of the styles below:
A means "the alpha value of the current pixel of the image"
a means "the alpha value of the effect parameter",
K means "the alpha value of pixels of the image that have the same RGB color as
     the color of the effect parameter", this is the Key color. 
r, g and b mean the red, green and blue component of the current pixel.
min means the component r, g or b with the lowest value
max means the component r, g or b with the highest value
sat means the saturation of the current pixel's RGB color

For example:
A = a means: "set the alpha channel of the current image pixel to effect.alpha"
K = 0 means "make pixels of the image that have the same RGB color as alpha.color invisible".


Values of the first 6 bits (the least significant bits)
+-------+---------------------+------------------------------------------------+
| value | effect              | description                                    |
+-------+---------------------+------------------------------------------------+
|   0   | none                | do nothing                                     |
|   1   | A = a               | set the alpha channel of the whole image to a  |
|   2   | A = a, K = 255      | same as 1, but make the key color opaque       |
|   3   | A = 255, K = a      | make image opaque, but set key color to a      |
+-------+---------------------+------------------------------------------------+
|   4   | A = (r + g + b) / 3 | set alpha channel to the Brightness            |
|   5   | A = (min + max) / 2 | set alpha channel to the Lightness             |
|   6   | A = min             | set alpha channel to the Darkness              |
|   7   | A = max             | set alpha channel to the Value                 |
|   8   | A = sat             | set alpha channel to the Saturation            |
+-------+---------------------+------------------------------------------------+
|   9   | A = r               | set alpha channel to Red                       |
|   10  | A = g               | set alpha channel to Green                     |
|   11  | A = b               | set alpha channel to Blue                      |
|   12  | A = (g + b) / 2     | set alpha channel to Cyan                      |
|   13  | A = (r + b) / 2     | set alpha channel to Magenta                   |
|   14  | A = (r + g) / 2     | set alpha channel to Yellow                    |
+-------+---------------------+------------------------------------------------+
|   15  | A = A / a           | divide alpha channel through a (truncated)     |
|   16  | A = A * a           | multiply alpha channel with a (truncated)      |
|   15  | A = A - a           | subtract a from alpha cnannel (truncated)      |
|   16  | A = A + a           | add a to alpha channel (truncated)             |
+-------+---------------------+------------------------------------------------+
|   19  | A = mod(RGB)        | modulated                                      |
|   20  | A = imod(RGB)       | inverse modulated                              |
+-------+---------------------+------------------------------------------------+


Optional values of the other bits:
+-------+---------------------+------------------------------------------------+
| bit   | effect        | description                  |
+-------+---------------------+------------------------------------------------+
|  &64  | A = 255 - A     | after calculating A, invert it         |
|  &128 | K = 0         | make key color invisible, e.g. for sprites   |
|  &256 | color = alphaColor  | after gen. alpha, fix color; eg for particles  |
|  &512 | color = Hp,Sp,Va  | color is set to hue and sat of pixel, brightness of alphaColor |
|  &1024|           | convert RGB 128,0,128 to black with alpha channel = 128 |
|  &2048|           | convert RGB 192,0,192 to black with alpha channel = 64 |
+-------+---------------------+------------------------------------------------+

EXAMPLE: style 68 (4 | 64) makes alphachannel depend on brightness, and makes key color zero
*/
void createImageAlpha(unsigned char* image, int w, int h, const AlphaEffect& effect)
{
  if(effect.style == 0) return; //no effect to be done, just return instead of going through the loop
  
  if((effect.style & 63) > 0)
  switch(effect.style & 63) //&63 to filter out the extra modifiers: those are handled later on
  {
    case 1:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      image[index + 3] = effect.alpha;
    }
    break;
    case 2:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      bool alphaPixel = (r == effect.alphaColor.r && g == effect.alphaColor.g && b == effect.alphaColor.b);
      if(!alphaPixel) image[index + 3] = effect.alpha;
      else image[index + 3] = 255;
    }
    break;
    case 3:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      bool alphaPixel = (r == effect.alphaColor.r && g == effect.alphaColor.g && b == effect.alphaColor.b);
      if(!alphaPixel) image[index + 3] = 255;
      else image[index + 3] = effect.alpha;
    }
    break;
    case 4:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      int alpha = (r + g + b) / 3;
      if(alpha < 0) alpha = 0; if(alpha > 255) alpha = 255;
      image[index + 3] = alpha;
    }
    break;
    case 5:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      int minc = r; if(g < minc) minc = g; if(b < minc) minc = b;
      int maxc = r; if(g > maxc) maxc = g; if(b > maxc) maxc = b;

      image[index + 3] = (minc + maxc) / 2;
    }
    break;
    case 6:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      int minc = r; if(g < minc) minc = g; if(b < minc) minc = b;

      image[index + 3] = minc;
    }
    break;
    case 7:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      int maxc = r; if(g > maxc) maxc = g; if(b > maxc) maxc = b;

      image[index + 3] = maxc;
    }
    break;
    case 8: //saturation of the HSV model
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      int minc = r; if(g < minc) minc = g; if(b < minc) minc = b;
      int maxc = r; if(g > maxc) maxc = g; if(b > maxc) maxc = b;

      if(maxc == 0) image[index + 3] = 0;
      else image[index + 3] = (255 * (maxc - minc)) / maxc;
    }
    break;
    case 9:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      image[index + 3] = r;
    }
    break;
    case 10:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int g = image[index + 1];
      image[index + 3] = g;
    }
    break;
    case 11:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int b = image[index + 2];
      image[index + 3] = b;
    }
    break;
    case 12:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int g = image[index + 1];
      int b = image[index + 2];
      image[index + 3] = (g + b) / 2;
    }
    break;
    case 13:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int b = image[index + 2];
      image[index + 3] = (r + b) / 2;
    }
    break;
    case 14:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      image[index + 3] = (r + g) / 2;
    }
    break;
    case 15:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      image[index + 3] /= effect.alpha;
    }
    break;
    case 16:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      
      int temp = image[index + 3] * effect.alpha;
      if(temp > 255) temp = 255;
      image[index + 3] = temp;
    }
    break;
    case 17:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      
      int temp = image[index + 3] - effect.alpha;
      if(temp < 0) temp = 0;
      image[index + 3] = temp;
    }
    break;
    case 18:
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      
      int temp = image[index + 3] + effect.alpha;
      if(temp > 255) temp = 255;
      image[index + 3] = temp;
    }
    break;
    case 19: //modulated
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      int alpha = (2 * abs(r - 128)
            + 2 * abs(g - 128)
            + 2 * abs(b - 128)) / 3;
      if(alpha < 0) alpha = 0; if(alpha > 255) alpha = 255;
      image[index + 3] = alpha;
    }
    break;
    case 20: //inverse modulated
    for(int x = 0; x < w; x++)
    for(int y = 0; y < h; y++)
    {
      int index = h * 4 * x + 4 * y;
      int r = image[index + 0];
      int g = image[index + 1];
      int b = image[index + 2];
      int alpha = (2 * abs(r - 128)
            + 2 * abs(g - 128)
            + 2 * abs(b - 128)) / 3;
      if(alpha < 0) alpha = 0; if(alpha > 255) alpha = 255;
      image[index + 3] = 255 - alpha;
    }
    break;
    default:
    break;
  }
  
  if(effect.style & 64)
  for(int x = 0; x < w; x++)
  for(int y = 0; y < h; y++)
  {
    int index = h * 4 * x + 4 * y; 
    image[index + 3] = 255 - image[index + 3];
  }
  
  if(effect.style & 128)
  for(int x = 0; x < w; x++)
  for(int y = 0; y < h; y++)
  {
    int index = h * 4 * x + 4 * y; 
    int r = image[index + 0];
    int g = image[index + 1];
    int b = image[index + 2];
    bool alphaPixel = (r == effect.alphaColor.r && g == effect.alphaColor.g && b == effect.alphaColor.b);
    if(alphaPixel) image[index + 3] = 0;
  }
  
  if(effect.style & 256)
  for(int x = 0; x < w; x++)
  for(int y = 0; y < h; y++)
  {
    int index = h * 4 * x + 4 * y; 
    image[index + 0] = effect.alphaColor.r;
    image[index + 1] = effect.alphaColor.g;
    image[index + 2] = effect.alphaColor.b;
  }

  if(effect.style & 512)
  for(int x = 0; x < w; x++)
  for(int y = 0; y < h; y++)
  {
    int index = h * 4 * x + 4 * y; 
    int r = image[index + 0];
    int g = image[index + 1];
    int b = image[index + 2];
    ColorRGB pixelRGB = ColorRGB(r, g, b);
    ColorHSV pixelHSV = RGBtoHSV(pixelRGB);
    ColorHSV alphaHSV = RGBtoHSV(effect.alphaColor);
    ColorRGB HpSpVa = HSVtoRGB(ColorHSV(pixelHSV.h, pixelHSV.s, alphaHSV.v));
    
    image[index + 0] = HpSpVa.r;
    image[index + 1] = HpSpVa.g;
    image[index + 2] = HpSpVa.b;
  }

  if(effect.style & 1024 || effect.style & 2048)
  for(int x = 0; x < w; x++)
  for(int y = 0; y < h; y++)
  {
    int index = h * 4 * x + 4 * y; 
    int r = image[index + 0];
    int g = image[index + 1];
    int b = image[index + 2];
    if(effect.style & 1024 && r == 128 && g == 0 && b == 128)
    {
      image[index + 0] = 0;
      image[index + 1] = 0;
      image[index + 2] = 0;
      image[index + 3] = 128;
    }
    if(effect.style & 2048 && r == 192 && g == 0 && b == 192)
    {
      image[index + 0] = 0;
      image[index + 1] = 0;
      image[index + 2] = 0;
      image[index + 3] = 64;
    }
  }
}

AlphaEffect::AlphaEffect(int style, unsigned char alpha, const ColorRGB& alphaColor)
{
  this->style = style;
  this->alpha = alpha;
  this->alphaColor = alphaColor;
}

void loadTexturesFromBase64PNG(std::vector<ITexture*>& textures, const ITextureFactory* factory, const std::string& base64, int widths, int heights, const AlphaEffect& effect)
{
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> decoded64, pixels;
  
  decodeBase64(decoded64, base64);
  pngdec.decode(pixels, decoded64);
  loadTextures(pixels, textures, factory, widths, heights, pngdec.getWidth(), pngdec.getHeight(), effect);
}

void loadTextureFromBase64PNG(ITexture* texture, const std::string& base64, const AlphaEffect& effect)
{
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> decoded64, pixels;

  decodeBase64(decoded64, base64);
  pngdec.decode(pixels, decoded64);
  createImageAlpha(pixels.empty() ? 0 : &pixels[0], pngdec.getWidth(), pngdec.getHeight(), effect);
  texture->setSize(pngdec.getWidth(), pngdec.getHeight());
  setAlignedBuffer(texture, pixels.empty() ? 0 : &pixels[0]);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void makeTextureSolid(ITexture* texture, const ColorRGB& color, size_t w, size_t h)
{
  texture->setSize(w, h);
  
  size_t u2 = texture->getU2();
  unsigned char* tbuffer = texture->getBuffer();
  
  for(size_t y = 0; y < w; y++)
  for(size_t x = 0; x < h; x++)
  {
    tbuffer[4 * u2 * y + 4 * x + 0] = color.r;
    tbuffer[4 * u2 * y + 4 * x + 1] = color.g;
    tbuffer[4 * u2 * y + 4 * x + 2] = color.b;
    tbuffer[4 * u2 * y + 4 * x + 3] = color.a;
  }

  texture->update();
}

/*
This loads the texture from a buffer or from a part of the buffer in the rectangle (x1,y1)-(x2,y2)
To get full buffer, make x1 = -1
w and h are width and height of the buffer, and the function assumes the memory of the buffer is big enough for that
The width of the texture buffer becomes x2 - x1, y2 - y1
Note that the buffer has an third dimension for the color channels and alpha
*/
void makeTextureFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, const AlphaEffect& effect, int x1, int y1, int x2, int y2)
{
  //out of range values means the user doesn't want to use sub-rectangle
  if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x1 > (int)w || y1 > (int)h || x2 > (int)w || y2 > (int)h || x1 > x2 || y1 > y2)
  {
    x1 = 0;
    y1 = 0;
    x2 = w;
    y2 = h;
  }
  texture->setSize(x2 - x1, y2 - y1);
  size_t u2 = texture->getU2();
  unsigned char* tbuffer = texture->getBuffer();

  for(int y = 0; y < y2 - y1; y++)
  for(int x = 0; x < x2 - x1; x++)
  for(int c = 0; c < 4; c++)
  {
    tbuffer[4 * u2 * y + 4 * x + c] = buffer[4 * w * (y1 + y) + 4 * (x1 + x) + c];
  }
  
  applyAlphaEffect(texture, effect);
  texture->update();
}

/*
Load the texture from a file
currently works with the png loader function
*/
void makeTextureFromPNGFile(ITexture* texture, const std::string& filename, const AlphaEffect& effect, int x1, int y1, int x2, int y2)
{
  LodePNG::Decoder pngdec;

  std::vector<unsigned char> file;
  LodePNG::loadFile(file, filename);
  
  //create the image and info std::vectors
  std::vector<unsigned char> image;
  
  //load the png and if it gives an error return it
  pngdec.decode(image, file);
  if(pngdec.hasError())
  {
    std::cout << "\npng loading error " << pngdec.getError() << " in file " << filename;
    return;
  }
  
  makeTextureFromBuffer(texture, &image[0], pngdec.getWidth(), pngdec.getHeight(), effect, x1, y1, x2, y2);
}

/*
Same as create, but only the alpha channel is affected, and no buffer is made so make sure the texture already has the correct size (and rgb colors) before you create its alpha channel.
The RED channel of the input image is used to make this alpha channel!
*/
void makeTextureAlphaFromBuffer(ITexture* texture, unsigned char* buffer, size_t w, size_t h, int x1, int y1, int x2, int y2)
{
  //if anything is invalid, use full rectangle instead
  if(w > texture->getU()) w = texture->getU();
  if(h > texture->getV()) h = texture->getV();
  if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x1 > (int)w || y1 > (int)h || x2 > (int)w || y2 > (int)h || x1 > x2 || y1 > y2)
  {
    x1 = 0;
    y1 = 0;
    x2 = w;
    y2 = h;
  }
  
  size_t u2 = texture->getU2();
  unsigned char* tbuffer = texture->getBuffer();

  for(int y = 0; y < y2 - y1; y++)
  for(int x = 0; x < x2 - x1; x++)
  {
    tbuffer[4 * u2 * y + 4 * x + 3] = buffer[4 * w * (y1 + y) + 4 * (x1 + x) + 0];
  }
  
  texture->update();
}

/*
Load the texture's alpha channel from a file
Make sure the alpha channel file is NOT larger than the current size the texture has!
*/
void makeTextureAlphaFromPNGFile(ITexture* texture, const std::string& filename, int x1, int y1, int x2, int y2)
{
  LodePNG::Decoder pngdec;
  
  std::vector<unsigned char> file;
  LodePNG::loadFile(file, filename);
  
  //create the image and info std::vectors
  std::vector<unsigned char> image;
  
  //load the png and if it gives an error return it
  pngdec.decode(image, file);
  if(pngdec.hasError())
  {
    std::cout << "\npng loading error " << pngdec.getError() << " in file " << filename;
    return;
  }
  
  makeTextureAlphaFromBuffer(texture, &image[0], pngdec.getWidth(), pngdec.getHeight(), x1, y1, x2, y2);
}

//Create an alpha channel for the texture with the wanted effect
void applyAlphaEffect(ITexture* texture, const AlphaEffect& effect)
{
  createImageAlpha(texture->getBuffer(), texture->getU2(), texture->getV2(), effect);
  texture->update();
}

ITexture* getDefaultEmptyTexture()
{
  class EmptyTexture : public ITexture
  {
    virtual void setSize(size_t, size_t) {}
    virtual size_t getU() const { return 0; }
    virtual size_t getV() const { return 0; }
    virtual size_t getU2() const { return 0; }
    virtual size_t getV2() const { return 0; }
    virtual unsigned char* getBuffer() { return 0; }
    virtual const unsigned char* getBuffer() const { return 0; }
    virtual void update() {};
    virtual void updatePartial(int,int,int,int) {};
  };
  
  static EmptyTexture empty;

  return &empty;
}

void getAlignedBuffer(std::vector<unsigned char>& buffer, const ITexture* texture)
{
  size_t u = texture->getU();
  size_t v = texture->getV();
  size_t u2 = texture->getU2();
  buffer.resize(u * v * 4);
  for(size_t y = 0; y < v; y++)
  for(size_t x = 0; x < u; x++)
  for(size_t c = 0; c < 4; c++)
  {
    buffer[y * u * 4 + x * 4 + c] = texture->getBuffer()[y * u2 * 4 + x * 4 + c];
  }
}

void setAlignedBuffer(ITexture* texture, const unsigned char* buffer)
{
  size_t u = texture->getU();
  size_t v = texture->getV();
  size_t u2 = texture->getU2();
  for(size_t y = 0; y < v; y++)
  for(size_t x = 0; x < u; x++)
  for(size_t c = 0; c < 4; c++)
  {
    texture->getBuffer()[y * u2 * 4 + x * 4 + c] = buffer[y * u * 4 + x * 4 + c];
  }
  texture->update();
}

void createTexture(ITexture* texture, size_t w, size_t h, const ColorRGB& color)
{
  texture->setSize(w, h);
  unsigned char* buffer = texture->getBuffer();
  size_t u2 = texture->getU2();
  for(size_t y = 0; y < h; y++)
  for(size_t x = 0; x < w; x++)
  {
    buffer[y * u2 * 4 + x * 4 + 0] = color.r;
    buffer[y * u2 * 4 + x * 4 + 1] = color.g;
    buffer[y * u2 * 4 + x * 4 + 2] = color.b;
    buffer[y * u2 * 4 + x * 4 + 3] = color.a;
  }
}

void setPixel(ITexture* texture, int x, int y, const ColorRGB& color)
{
  unsigned char* buffer = texture->getBuffer();
  size_t u2 = texture->getU2();
  buffer[y * u2 * 4 + x * 4 + 0] = color.r;
  buffer[y * u2 * 4 + x * 4 + 1] = color.g;
  buffer[y * u2 * 4 + x * 4 + 2] = color.b;
  buffer[y * u2 * 4 + x * 4 + 3] = color.a;
}

ColorRGB getPixel(const ITexture* texture, int x, int y)
{
  ColorRGB result;
  const unsigned char* buffer = texture->getBuffer();
  size_t u2 = texture->getU2();
  result.r = buffer[y * u2 * 4 + x * 4 + 0];
  result.g = buffer[y * u2 * 4 + x * 4 + 1];
  result.b = buffer[y * u2 * 4 + x * 4 + 2];
  result.a = buffer[y * u2 * 4 + x * 4 + 3];
  return result;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


HTexture::HTexture()
: refcount(new int(1))
, texture(0)
{
}

HTexture::HTexture(const HTexture& other)
: refcount(0)
, texture(0)
{
  if(other.texture != 0)
  {
    refcount = other.refcount;
    texture = other.texture;
    (*refcount)++;
    if((*refcount) > 2)
    {
      static bool warningGiven = false;
      if(!warningGiven) std::cout << "Warning: unintended usage of HTexture (this warning is displayed only once)" << std::endl;
      warningGiven = true;
    }
  }
  else refcount = new int(1);
}

HTexture::~HTexture()
{
  (*refcount)--;
  if(*refcount == 0)
  {
    if(texture) delete texture;
    delete refcount;
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TextureBuffer::TextureBuffer()
: u(0)
, v(0)
{
}

void TextureBuffer::setSize(size_t u, size_t v)
{
  buffer.resize(u * v * 4);
  this->u = u;
  this->v = v;
}

size_t TextureBuffer::getU() const
{
  return u;
}

size_t TextureBuffer::getV() const
{
  return v;
}

size_t TextureBuffer::getU2() const
{
  return u;
}

size_t TextureBuffer::getV2() const
{
  return v;
}

unsigned char* TextureBuffer::getBuffer()
{
  return buffer.empty() ? 0 : &buffer[0];
}

const unsigned char* TextureBuffer::getBuffer() const
{
  return buffer.empty() ? 0 : &buffer[0];
}

void TextureBuffer::update()
{
  //nothing to do :)
}

void TextureBuffer::updatePartial(int x0, int y0, int x1, int y1)
{
  //standard way to suppress the warnings
  (void)x0;
  (void)y0;
  (void)x1;
  (void)y1;
  
  //nothing to do :)
}

void copyTexture(ITexture* dest, const ITexture* source)
{
  size_t u = source->getU();
  size_t v = source->getV();
  size_t u2 = source->getU2();
  dest->setSize(u, v);
  size_t u2b = dest->getU2();
  for(size_t y = 0; y < v; y++)
  for(size_t x = 0; x < u; x++)
  for(size_t c = 0; c < 4; c++)
  {
    size_t index = y * u2 * 4 + x * 4 + c;
    size_t indexb = y * u2b * 4 + x * 4 + c;
    dest->getBuffer()[indexb] = source->getBuffer()[index];
  }
  dest->update();
}

} //namespace lpi
