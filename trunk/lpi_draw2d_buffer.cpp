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

#include "lpi_draw2d_buffer.h"

#include <vector>
#include <cmath>

namespace lpi
{


namespace
{

void pset(unsigned char* buffer, int buffer_w, int x, int y, const ColorRGB& color)
{
  size_t bufferPos = 4 * buffer_w * y + 4 * x;
  buffer[bufferPos + 0] = color.r;
  buffer[bufferPos + 1] = color.g;
  buffer[bufferPos + 2] = color.b;
  buffer[bufferPos + 3] = color.a;
}

void psetClipped(unsigned char* buffer, int buffer_w, const ADrawer2DBuffer::Clip& c, int x, int y, const ColorRGB& color)
{
  if(x >= c.x0 && x < c.x1 && y >= c.y0 && y < c.y1) pset(buffer, buffer_w, x, y, color);
}

//Fast horizontal line from (x1,y) to (x2,y), with rgb color
void horLine(unsigned char* buffer, int buffer_w, int buffer_h, int y, int x1, int x2, const ColorRGB& color)
{
  if(x2 < x1) std::swap(x1, x2); //swap x1 and x2 because x1 must be the leftmost endpoint
  if(x2 < 0 || x1 > buffer_w - 1 || y < 0 || y > buffer_h - 1) return; //no single point of the line is on screen
  
  for(int x = x1; x <= x2; x++)
  {
    int bufferPos = 4 * buffer_w * y + 4 * x;
    buffer[bufferPos + 0] = color.r;
    buffer[bufferPos + 1] = color.g;
    buffer[bufferPos + 2] = color.b;
    buffer[bufferPos + 3] = color.a;
  }
}

//Fast vertical line from (x,y1) to (x,y2), with rgb color
void verLine(unsigned char* buffer, int buffer_w, int buffer_h, int x, int y1, int y2, const ColorRGB& color)
{
  if(y2 < y1) std::swap(y1, y2); //swap y1 and y2 because y1 must be the topmost endpoint
  if(x < 0 || x > buffer_w - 1 || y2 < 0 || y1 > buffer_h - 1) return;
  
  for(int y = y1; y <= y2; y++)
  {
    int bufferPos = 4 * buffer_w * y + 4 * x;
    buffer[bufferPos + 0] = color.r;
    buffer[bufferPos + 1] = color.g;
    buffer[bufferPos + 2] = color.b;
    buffer[bufferPos + 3] = color.a;
  }
}

//this one does NOT check if the line is inside the buffer! (hence only width, not height, as parameter)
void bresenhamLine(unsigned char* buffer, int w, int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  //draw the line with bresenham
  int deltax = std::abs((double)(x1 - x0));    // The difference between the x's
  int deltay = std::abs((double)(y1 - y0));    // The difference between the y's
  int x = x0;           // Start x off at the first pixel
  int y = y0;           // Start y off at the first pixel
  int xinc0, xinc1, yinc0, yinc1, den, num, numadd, numpixels, curpixel;

  if(x1 >= x0)         // The x-values are increasing
  {
    xinc0 = 1;
    xinc1 = 1;
  }
  else              // The x-values are decreasing
  {
    xinc0 = -1;
    xinc1 = -1;
  }
  if (y1 >= y0)         // The y-values are increasing
  {
    yinc0 = 1;
    yinc1 = 1;
  }
  else              // The y-values are decreasing
  {
    yinc0 = -1;
    yinc1 = -1;
  }
  if(deltax >= deltay)     // There is at least one x-value for every y-value
  {
    xinc0 = 0;          // Don't change the x when numerator >= denominator
    yinc1 = 0;          // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;     // There are more x-values than y-values
  }
  else              // There is at least one y-value for every x-value
  {
    xinc1 = 0;          // Don't change the x for every iteration
    yinc0 = 0;          // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;     // There are more y-values than x-values
  }
  for(curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    pset(buffer, w, x, y, color);  // Draw the current pixel
    num += numadd;        // Increase the numerator by the top of the fraction
    if (num >= den)       // Check if numerator >= denominator
    {
      num -= den;       // Calculate the new numerator value
      x += xinc0;       // Change the x as appropriate
      y += yinc0;       // Change the y as appropriate
    }
    x += xinc1;         // Change the x as appropriate
    y += yinc1;         // Change the y as appropriate
  }
}

void drawLine(unsigned char* buffer, int w, const ADrawer2DBuffer::Clip& clip, int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  //clip if some point is outside the clipping area
  if(x0 < clip.x0 || x0 >= clip.x1 || x1 < clip.x0 || x1 >= clip.x1 || y0 < clip.y0 || y0 >= clip.y1 || y1 < clip.y0 || y1 >= clip.y1)
  {
    int x2, y2, x3, y3;
    if(!clipLine(x0, y0, x1, y1, x2, y2, x3, y3, clip.x0, clip.y0, clip.x1, clip.y1)) return;
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
  
  bresenhamLine(buffer, w, x0, y0, x1, y1, color);
}

void recursive_bezier(unsigned char* buffer, int w, const ADrawer2DBuffer::Clip& clip,
                                       double x0, double y0, //endpoint
                                       double x1, double y1, //handle
                                       double x2, double y2, //handle
                                       double x3, double y3, //endpoint
                                       const ColorRGB& color,
                                       int n) //extra recursion test for safety
{
  if(bezier_nearly_flat(x0, y0, x1, y1, x2, y2, x3, y3) || n > 20)
  {
    drawLine(buffer, w, clip, (int)x0, (int)y0, (int)x3, (int)y3, color);
  }
  else
  {
    double x01   = (x0 + x1) / 2;
    double y01   = (y0 + y1) / 2;
    double x12   = (x1 + x2) / 2;
    double y12   = (y1 + y2) / 2;
    double x23   = (x2 + x3) / 2;
    double y23   = (y2 + y3) / 2;
    double x012  = (x01 + x12) / 2;
    double y012  = (y01 + y12) / 2;
    double x123  = (x12 + x23) / 2;
    double y123  = (y12 + y23) / 2;
    double x0123 = (x012 + x123) / 2;
    double y0123 = (y012 + y123) / 2;
    
    recursive_bezier(buffer, w, clip, x0, y0, x01, y01, x012, y012, x0123, y0123, color, n + 1); 
    recursive_bezier(buffer, w, clip, x0123, y0123, x123, y123, x23, y23, x3, y3, color, n + 1);
  }
}

void drawEllipseBorder(unsigned char* buffer, int w, const ADrawer2DBuffer::Clip& clip, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color)
{
  int twoASquare = 2 * radiusx * radiusx;
  int twoBSquare = 2 * radiusy * radiusy;
  int x = radiusx;
  int y = 0;
  int xchange = radiusy * radiusy * (1 - 2 * radiusx);
  int ychange = radiusx * radiusx;
  int ellipseError = 0;
  int stoppingx = twoBSquare * radiusx;
  int stoppingy = 0;

  while(stoppingx >= stoppingy)
  {
    psetClipped(buffer, w, clip, cx + x, cy + y, color);
    psetClipped(buffer, w, clip, cx - x, cy + y, color);
    psetClipped(buffer, w, clip, cx - x, cy - y, color);
    psetClipped(buffer, w, clip, cx + x, cy - y, color);

    y++;
    stoppingy += twoASquare;
    ellipseError += ychange;
    ychange += twoASquare;
    if((2 * ellipseError + xchange) > 0)
    {
      x--;
      stoppingx -= twoBSquare;
      ellipseError += xchange;
      xchange += twoBSquare;
    }
  }
        
  x = 0;
  y = radiusy;
  xchange = radiusy * radiusy;
  ychange = radiusx * radiusx * (1 - 2 * radiusy);
  ellipseError = 0;
  stoppingx = 0;
  stoppingy = twoASquare * radiusy;
  
  while(stoppingx <= stoppingy)
  {
    psetClipped(buffer, w, clip, cx + x, cy + y, color);
    psetClipped(buffer, w, clip, cx - x, cy + y, color);
    psetClipped(buffer, w, clip, cx - x, cy - y, color);
    psetClipped(buffer, w, clip, cx + x, cy - y, color);
    
    x++;
    stoppingx += twoBSquare;
    ellipseError += xchange;
    xchange += twoBSquare;
    if ((2*ellipseError + ychange) > 0 )
    {
      y--;
      stoppingy -= twoASquare;
      ellipseError += ychange;
      ychange += twoASquare;
    }
  }
}

void drawFilledEllipse(unsigned char* buffer, int w, int h, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color)
{
  int twoASquare = 2 * radiusx * radiusx;
  int twoBSquare = 2 * radiusy * radiusy;
  int x = radiusx;
  int y = 0;
  int xchange = radiusy * radiusy * (1 - 2 * radiusx);
  int ychange = radiusx * radiusx;
  int ellipseError = 0;
  int stoppingx = twoBSquare * radiusx;
  int stoppingy = 0;

  while(stoppingx >= stoppingy)
  {
    //TODO: use clip!
    horLine(buffer, w, h, cy - y, cx - x, cx + x, color);
    horLine(buffer, w, h, cy + y, cx - x, cx + x, color);

    y++;
    stoppingy += twoASquare;
    ellipseError += ychange;
    ychange += twoASquare;
    if((2 * ellipseError + xchange) > 0)
    {
      x--;
      stoppingx -= twoBSquare;
      ellipseError += xchange;
      xchange += twoBSquare;
    }
  }
        
  x = 0;
  y = radiusy;
  xchange = radiusy * radiusy;
  ychange = radiusx * radiusx*(1 - 2 * radiusy);
  ellipseError = 0;
  stoppingx = 0;
  stoppingy = twoASquare * radiusy;
  while ( stoppingx <= stoppingy )
  {
    //TODO: use clip!
    horLine(buffer, w, h, cy - y, cx - x, cx + x, color);
    horLine(buffer, w, h, cy + y, cx - x, cx + x, color);

    x++;
    stoppingx += twoBSquare;
    ellipseError += xchange;
    xchange += twoBSquare;
    if ((2*ellipseError + ychange) > 0 )
    {
      y--;
      stoppingy -= twoASquare;
      ellipseError += ychange;
      ychange += twoASquare;
    }
  }
}

void drawCircleBorder(unsigned char* buffer, int w, const ADrawer2DBuffer::Clip& clip, int cx, int cy, int radius, const ColorRGB& color)
{
  int x = radius;
  int y = 0;
  int xchange = 1 - 2 * radius;
  int ychange = 1;
  int radiuserror = 0;
  while(x >= y)
  {
    psetClipped(buffer, w, clip, cx + x, cy + y, color);
    psetClipped(buffer, w, clip, cx - x, cy + y, color);
    psetClipped(buffer, w, clip, cx - x, cy - y, color);
    psetClipped(buffer, w, clip, cx + x, cy - y, color);
    psetClipped(buffer, w, clip, cx + y, cy + x, color);
    psetClipped(buffer, w, clip, cx - y, cy + x, color);
    psetClipped(buffer, w, clip, cx - y, cy - x, color);
    psetClipped(buffer, w, clip, cx + y, cy - x, color);
    y++;
    radiuserror += ychange;
    ychange += 2;
    if(2 * radiuserror + xchange > 0 )
    {
      x--;
      radiuserror += xchange;
      xchange += 2;
    }
  }
}

//Filled bresenham circle with center at (xc,yc) with radius and RGB color
void drawDisk(unsigned char* buffer, int buffer_w, int buffer_h, int xc, int yc, int radius, const ColorRGB& color)
{
  if(xc + radius < 0 || xc - radius >= buffer_w || yc + radius < 0 || yc - radius >= buffer_h) return; //every single pixel outside screen, so don't waste time on it
  int x = 0;
  int y = radius;
  int p = 3 - (radius << 1);
  int a, b, c, d, e, f, g, h;
  int pb = xc - radius, pd = xc - radius; //previous values: to avoid drawing horizontal lines multiple times    
  while (x <= y)
  {
    // write data
    a = xc + x;
    b = yc + y;
    c = xc - x;
    d = yc - y;
    e = xc + y;
    f = yc + x;
    g = xc - y;
    h = yc - x;
    //TODO: use the clipping!
    if(b != pb) horLine(buffer, buffer_w, buffer_h, b, a, c, color);
    if(d != pd) horLine(buffer, buffer_w, buffer_h, d, a, c, color);
    if(f != b)  horLine(buffer, buffer_w, buffer_h, f, e, g, color);
    if(h != d && h != f) horLine(buffer, buffer_w, buffer_h, h, e, g, color);
    pb = b;
    pd = d;
    if(p < 0) p += (x++ << 2) + 6;
    else p += ((x++ - y--) << 2) + 10;
  }
}

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


ADrawer2DBuffer::ADrawer2DBuffer()
: buffer(0)
, w(0)
, h(0)
{
}

void ADrawer2DBuffer::pushScissor(int x0, int y0, int x1, int y1)
{
  clipstack.push_back(clip);
  clip.x0 = x0;
  clip.y0 = y0;
  clip.x1 = x1;
  clip.y1 = y1;
  clip.fit(w, h);
}

void ADrawer2DBuffer::pushSmallestScissor(int x0, int y0, int x1, int y1)
{
  int sx0 = x0;
  int sy0 = y0;
  int sx1 = x1;
  int sy1 = y1;
  
  if(clip.x0 > sx0) sx0 = clip.x0;
  if(clip.y0 >  sy0)  sy0 = clip.y0; 
  if(clip.x1 < sx1) sx1 = clip.x1;
  if(clip.y1 < sy1) sy1 = clip.y1;
  
  pushScissor(sx0,  sy0, sx1, sy1);
}

void ADrawer2DBuffer::popScissor()
{
  clip = clipstack.back();
  clip.fit(w, h);
  clipstack.pop_back();
}

void ADrawer2DBuffer::drawPoint(int x, int y, const ColorRGB& color)
{
  psetClipped(buffer, w, clip, x, y, color);
}

void ADrawer2DBuffer::drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  lpi::drawLine(buffer, w, clip, x0, y0, x1, y1, color);
}

void ADrawer2DBuffer::drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color)
{
  recursive_bezier(buffer, w, clip, x0, y0, x1, y1, x2, y2, x3, y3, color, 0);
}

    
void ADrawer2DBuffer::drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled)
{
  if(filled)
  {
    int sx0 = x0;
    int sy0 = y0;
    int sx1 = x1;
    int sy1 = y1;
    
    if(clip.x0 > sx0) sx0 = clip.x0;
    if(clip.y0 >  sy0)  sy0 = clip.y0; 
    if(clip.x1 < sx1) sx1 = clip.x1;
    if(clip.y1 < sy1) sy1 = clip.y1;
    
    for(int x = sx0; x < sx1; x++)
    for(int y = sy0; y < sy1; y++)
    {
      pset(buffer, w, x, y, color);
    }
  }
  else
  {
    drawLine(x0, y0, x1, y0, color);
    drawLine(x0, y1 - 1, x1, y1 - 1, color);
    drawLine(x0, y0, x0, y1, color);
    drawLine(x1 - 1, y0, x1 - 1, y1, color);
  }
}

void ADrawer2DBuffer::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  (void)x0; (void)y0; (void)x1; (void)y1; (void)x2; (void)y2; (void)color; (void)filled;
  //TODO!
}

void ADrawer2DBuffer::drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled)
{
  (void)x0; (void)y0; (void)x1; (void)y1; (void)x2; (void)y2; (void)x3; (void)y3; (void)color; (void)filled;
  //TODO!
}

void ADrawer2DBuffer::drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled)
{
  filled ? drawDisk(buffer, w, h, x, y, radius, color) : drawCircleBorder(buffer, w, clip, x, y, radius, color);
}

void ADrawer2DBuffer::drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled)
{
  filled ? drawFilledEllipse(buffer, w, h, x, y, radiusx, radiusy, color) : drawEllipseBorder(buffer, w, clip, x, y, radiusx, radiusy, color);
}

void ADrawer2DBuffer::drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  (void)x0; (void)y0; (void)x1; (void)y1; (void)x2; (void)y2; (void)x3; (void)y3; (void)color0; (void)color1; (void)color2; (void)color3;
  //TODO!
}

bool ADrawer2DBuffer::supportsTexture(ITexture* texture)
{
  return dynamic_cast<TextureBuffer*>(texture);
}

ITexture* ADrawer2DBuffer::createTexture()
{
  return new TextureBuffer();
}

ITexture* ADrawer2DBuffer::createTexture(ITexture* texture)
{
  TextureBuffer* t = new TextureBuffer();
  makeTextureFromBuffer(t
                      , texture->getBuffer(), texture->getU2(), texture->getV2()
                      , AE_Opaque
                      , 0, 0, texture->getU(), texture->getV());
  return t;
}

    
void ADrawer2DBuffer::drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod)
{
  (void)colorMod; //TODO: use the colorMod if it's not white
  
  const unsigned char* tbuffer = texture->getBuffer();
  size_t tu = texture->getU();
  size_t tv = texture->getV();
  size_t tu2 = texture->getU2();
  //size_t tv2 = texture->getV2();
  int x0 = 0;
  int y0 = 0;
  int x1 = tu;
  int y1 = tv;
  if(x < clip.x0) x0 = clip.x0 - x;
  if(y < clip.y0) y0 = clip.y0 - y;
  if(x + x1 > clip.x1) x1 = clip.x1 - x;
  if(y + y1 > clip.y1) y1 = clip.y1 - y;
  if(x0 >= x1 || y0 >= y1) return;
  for(int tx = x0; tx < x1; tx++)
  for(int ty = y0; ty < y1; ty++)
  {
    int index = (y + ty) * w * 4 + (x + tx) * 4;
    int tindex = ty * tu2 * 4 + tx * 4;
    //TODO: use the alpha channel of the texture for alpha blending the RGB, instead of copying it
    buffer[index + 0] = tbuffer[tindex + 0];
    buffer[index + 1] = tbuffer[tindex + 1];
    buffer[index + 2] = tbuffer[tindex + 2];
    buffer[index + 3] = tbuffer[tindex + 3];
  }
}

void ADrawer2DBuffer::drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  (void)texture; (void)x; (void)y; (void)sizex; (void)sizey; (void)colorMod;
  //TODO
}

void ADrawer2DBuffer::drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod)
{
  (void)texture; (void)x0; (void)y0; (void)x1; (void)y1; (void)colorMod;
  //TODO
}

/*void ADrawer2DBuffer::cls(const ColorRGB& color)
{
  size_t w = getWidth();
  size_t h = getHeight();
  unsigned char r = color.r;
  unsigned char g = color.g;
  unsigned char b = color.b;
  unsigned char a = color.a;
  
  for(size_t x = 0; x < w; x++)
  for(size_t y = 0; y < h; y++)
  {
    size_t index = y * w * 4 + x * 4;
    buffer[index + 0] = r;
    buffer[index + 1] = g;
    buffer[index + 2] = b;
    buffer[index + 3] = a;
  }
}*/

} //namespace lpi
