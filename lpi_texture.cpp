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
#include "lpi_gl.h"
#include "lpi_base64.h"

#include <algorithm>

namespace lpi
{


Texture emptyTexture;


//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////
//Texture Class/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//
Texture::Texture()
{
  deleteBuffer();
  generated = false;
}

void Texture::operator*=(double a)
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
void Texture::makeBuffer(int u, int v)
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
void Texture::deleteBuffer()
{
  buffer.clear();
}

void Texture::create(const ColorRGB& color, int w, int h)
{
  //if anything is invalid, use full rectangle instead
  if(w < 0) w = 0;
  if(h < 0) h = 0;

  deleteBuffer();
  makeBuffer(w, h);
  
  for(size_t y = 0; y < v2; y++)
  for(size_t x = 0; x < u2; x++)
  {
    this->buffer[4 * u2 * y + 4 * x + 0] = color.r;
    this->buffer[4 * u2 * y + 4 * x + 1] = color.g;
    this->buffer[4 * u2 * y + 4 * x + 2] = color.b;
    this->buffer[4 * u2 * y + 4 * x + 3] = color.a;
  }

  upload();
}

/*
This loads the texture from a buffer or from a part of the buffer in the rectangle (x1,y1)-(x2,y2)
To get full buffer, make x1 = -1
w and h are width and height of the buffer, and the function assumes the memory of the buffer is big enough for that
The width of the texture buffer becomes x2 - x1, y2 - y1
Note that the buffer has an third dimension for the color channels and alpha
*/
void Texture::create(unsigned char * buffer, int w, int h, const AlphaEffect& effect, int x1, int y1, int x2, int y2)
{
  //if anything is invalid, use full rectangle instead
  if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x1 > w || y1 > h || x2 > w || y2 > h || x1 > x2 || y1 > y2)
  {
    x1 = 0;
    y1 = 0;
    x2 = w;
    y2 = h;
  }

  deleteBuffer();
  makeBuffer(x2 - x1, y2 - y1);
  for(int y = 0; y < y2 - y1; y++)
  for(int x = 0; x < x2 - x1; x++)
  for(int c = 0; c < 4; c++)
  {
    this->buffer[4 * u2 * y + 4 * x + c] = buffer[4 * w * (y1 + y) + 4 * (x1 + x) + c];
  }
  
  calculateAlpha(effect);
  upload();
}

namespace
{
  bool isPowerOfTwo(int n) //does not work properly if n is <= 0, then an extra test n > 0 should be added
  {
    return !(n & (n - 1)); //this checks if the integer n is a power of two or not
  }
}

/*
Load the texture from a file
currently works with the png loader function
*/
void Texture::create(const std::string& filename, const AlphaEffect& effect, int x1, int y1, int x2, int y2)
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
  
  if(x1 == -1 && y1 == -1 && x2 == -1 && y2 == -1 && isPowerOfTwo(pngdec.getWidth()) && isPowerOfTwo(pngdec.getHeight())) //this should be faster
  {
    deleteBuffer();
    makeBuffer(pngdec.getWidth(), pngdec.getHeight());
    buffer.swap(image);
    calculateAlpha(effect);
    upload();
  }
  else create(&image[0], pngdec.getWidth(), pngdec.getHeight(), effect, x1, y1, x2, y2);
}

/*
Same as create, but only the alpha channel is affected, and no buffer is made so make sure the texture already has the correct size (and rgb colors) before you create its alpha channel.
The RED channel of the input image is used to make this alpha channel!
*/
void Texture::createAlpha(unsigned char * buffer, int w, int h, int x1, int y1, int x2, int y2)
{
  //if anything is invalid, use full rectangle instead
  if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x1 > w || y1 > h || x2 > w || y2 > h || x1 > x2 || y1 > y2)
  {
    x1 = 0;
    y1 = 0;
    x2 = w;
    y2 = h;
  }

  for(int y = 0; y < y2 - y1; y++)
  for(int x = 0; x < x2 - x1; x++)
  {
    this->buffer[4 * u2 * y + 4 * x + 3] = buffer[4 * w * (y1 + y) + 4 * (x1 + x) + 0];
  }
  
  upload();
}

/*
Load the texture's alpha channel from a file
Make sure the alpha channel file is NOT larger than the current size the texture has!
*/
void Texture::createAlpha(const std::string& filename, int x1, int y1, int x2, int y2)
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
  
  createAlpha(&image[0], pngdec.getWidth(), pngdec.getHeight(), x1, y1, x2, y2);
}

//Create an alpha channel for the texture with the wanted effect
void Texture::calculateAlpha(const AlphaEffect& effect)
{
  createImageAlpha(&buffer[0], u2, v2, effect);
  upload();
}

//This generates the OpenGL texture so that OpenGL can use it, also use after changing the texture buffer
void Texture::upload()
{
  if(!generated)
  {
    glGenTextures(1, texture);
    generated = true;
  }
  bind();
  glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0]);
}

void Texture::reupload()
{
  generated = false;
  upload();
}

//make this the selected one for drawing
void Texture::bind() const
{
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  if(enabledSmoothing())
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

//Draw the texture on screen
void Texture::draw(int x, int y, const ColorRGB& colorMod, int sizex, int sizey, int skewx, int skewy) const
{
  if(sizex < 0) sizex = u;
  if(sizey < 0) sizey = v;
  if(sizex == 0 || sizey == 0) return;
  glEnable(GL_TEXTURE_2D);

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  bind();

  setOpenGLScissor(); //everything that draws something must always do this

  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(x + 0     + skewx, y + 0     + 0    );
    glTexCoord2d(+u3, 0.0); glVertex2d(x + sizex + skewx, y + 0     + skewy);
    glTexCoord2d(+u3, +v3); glVertex2d(x + sizex + 0    , y + sizey + skewy);
    glTexCoord2d(0.0, +v3); glVertex2d(x + 0     + 0    , y + sizey + 0    );
  glEnd();
}

//Draw the texture on screen
void Texture::draw(int x1, int y1, int x2, int y2, const ColorRGB& colorMod) const
{
  if(x2 - x1 == 0 || y2 - y1 == 0) return;
  glEnable(GL_TEXTURE_2D);

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  bind();

  setOpenGLScissor(); //everything that draws something must always do this

  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(x1, y1, 1);
    glTexCoord2d(+u3, 0.0); glVertex3d(x2, y1, 1);
    glTexCoord2d(+u3, +v3); glVertex3d(x2, y2, 1);
    glTexCoord2d(0.0, +v3); glVertex3d(x1, y2 , 1);
  glEnd();
}

void Texture::draw(int x, int y, double scalex, double scaley, const ColorRGB& colorMod) const
{
  double sizex = u * scalex;
  double sizey = v * scaley;
  if(sizex == 0.0 || sizey == 0.0) return;
  glEnable(GL_TEXTURE_2D);

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  bind();

  setOpenGLScissor(); //everything that draws something must always do this

  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(x + 0  , y + 0  , 1);
    glTexCoord2d(+u3, 0.0); glVertex3d(x + sizex, y + 0  , 1);
    glTexCoord2d(+u3, +v3); glVertex3d(x + sizex, y + sizey, 1);
    glTexCoord2d(0.0, +v3); glVertex3d(x + 0  , y + sizey, 1);
  glEnd();
}

void Texture::draw(int x, int y, double scale, const ColorRGB& colorMod) const
{
  draw(x, y, scale, scale, colorMod);
}

//Draw the texture on screen where x and y are the center instead of the top left corner
void Texture::drawCentered(int x, int y, const ColorRGB& colorMod, int sizex, int sizey, int skewx, int skewy) const
{
  if(sizex < 0) sizex = u;
  if(sizey < 0) sizey = v;
  if(sizex == 0 || sizey == 0) return;
  draw(x - sizex / 2, y - sizey / 2, colorMod, sizex, sizey, skewx, skewy);
}

void Texture::drawCentered(int x, int y, double scalex, double scaley, const ColorRGB& colorMod) const
{
  int sizex = int(u * scalex);
  int sizey = int(v * scaley);
  if(sizex == 0 || sizey == 0) return;
  draw(x - sizex / 2, y - sizey / 2, scalex, scaley, colorMod);
}

void Texture::drawCentered(int x, int y, double scale, const ColorRGB& colorMod) const
{
  drawCentered(x, y, scale, scale, colorMod);
}

/*
draws the texture repeated, useful to fill something with a tilable texture
The texture size must be a power of 2, or you get ugly black things in between
scale is a scaling factor of the texture
*/
void Texture::drawRepeated(int x1, int y1, int x2, int y2, double scalex, double scaley, const ColorRGB& colorMod) const
{
  glEnable(GL_TEXTURE_2D);

  glColor4ub(colorMod.r, colorMod.g, colorMod.b, colorMod.a);
  bind();

  setOpenGLScissor(); //everything that draws something must always do this
  
  double coorx = (double(x2 - x1) / u) / scalex;//(u3 * (x2 - x1)) / scalex;
  double coory = (double(y2 - y1) / v) / scaley; //(v3 * (y2 - y1)) / scaley;

  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
  glBegin(GL_QUADS);
    glTexCoord2d(0.0,       0.0); glVertex3d(x1, y1, 1);
    glTexCoord2d(0.0,    +coory); glVertex3d(x1, y2, 1);
    glTexCoord2d(+coorx, +coory); glVertex3d(x2, y2, 1);
    glTexCoord2d(+coorx,    0.0); glVertex3d(x2, y1, 1);
  glEnd();
}

void Texture::getAlignedBuffer(std::vector<unsigned char>& out)
{
  //out = buffer;
  out.clear();
  for(size_t y = 0; y < v; y++)
  {
    out.insert(out.end(), buffer.begin() + 4 * y * u2, buffer.begin() + 4 * y * u2 + 4 * u);
  }
}

void Texture::setAlignedBuffer(const std::vector<unsigned char>& in)
{
  for(size_t y = 0; y < v; y++)
  {
    std::copy(in.begin() + 4 * y * u, in.begin() + 4 * y * u + 4 * u, buffer.begin() + 4 * y * u2);
  }
}

void loadTextures(std::vector<unsigned char>& buffer, std::vector<Texture>& textures, int widths, int heights, int w, int h, const AlphaEffect& effect)
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
    textures.back().create(&buffer[0], w, h, effect, x * widths, y * heights, (x + 1) * widths, (y + 1) * heights);
  }
} 

/*
*/
void loadTextures(const std::string& filename, std::vector<Texture>& textures, int widths, int heights, const AlphaEffect& effect)
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
  
  loadTextures(image, textures, widths, heights, pngdec.getWidth(), pngdec.getHeight(), effect);
}

void loadTexturesAlpha(std::vector<unsigned char>& buffer, std::vector<Texture>& textures, int widths, int heights, int w, int h)
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
  int index = 0;
  for(int y = 0; y < numy; y++)
  for(int x = 0; x < numx; x++)
  {
    textures[index].createAlpha(&buffer[0], w, h, x * widths, y * heights, (x + 1) * widths, (y + 1) * heights);
    index++;
  }
} 

/*
*/
void loadTexturesAlpha(const std::string& filename, std::vector<Texture>& textures, int widths, int heights)
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
  
  loadTexturesAlpha(image, textures, widths, heights, pngdec.getWidth(), pngdec.getHeight());
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
      else image[index + 3] = (maxc - minc) / maxc;
      break;
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

void loadTexturesFromBase64PNG(std::vector<Texture>& textures, const std::string& base64, int widths, int heights, const AlphaEffect& effect)
{
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> decoded64, pixels;
  
  decodeBase64(decoded64, base64);
  pngdec.decode(pixels, decoded64);
  loadTextures(pixels, textures, widths, heights, pngdec.getWidth(), pngdec.getHeight(), effect);
}


} //namespace lpi
