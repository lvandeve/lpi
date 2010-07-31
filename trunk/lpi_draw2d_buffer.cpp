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
#include "lpi_math2d.h"
#include "lpi_texture.h"

#include <vector>
#include <cmath>

namespace lpi
{


namespace
{

void pset(unsigned char* buffer, size_t bufferpos, const ColorRGB& color)
{
  ///Opaque
  /*buffer[bufferPos + 0] = color.r;
  buffer[bufferPos + 1] = color.g;
  buffer[bufferPos + 2] = color.b;
  buffer[bufferPos + 3] = color.a;*/
  
  ///Alpha Channeled
  const unsigned short ccr = color.r;
  const unsigned short ccg = color.g;
  const unsigned short ccb = color.b;
  const unsigned short cca = color.a;
  const unsigned short cca1 = 256 - cca;
    
  const unsigned short orr = buffer[bufferpos + 0];
  const unsigned short org = buffer[bufferpos + 1];
  const unsigned short orb = buffer[bufferpos + 2];
  const unsigned short ora = buffer[bufferpos + 3];
  
  buffer[bufferpos + 0] = ((orr * cca1) + (ccr * cca)) / 256;
  buffer[bufferpos + 1] = ((org * cca1) + (ccg * cca)) / 256;
  buffer[bufferpos + 2] = ((orb * cca1) + (ccb * cca)) / 256;
  buffer[bufferpos + 3] = ora > cca ? ora : cca; //not really correct but good enough for now.
}

void pset(unsigned char* buffer, int buffer_w, int x, int y, const ColorRGB& color)
{
  pset(buffer, 4 * buffer_w * y + 4 * x, color);
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
  
  int bufferpos = 4 * buffer_w * y;
  for(int x = x1; x <= x2; x++)
  {
    /*buffer[bufferpos + 0] = color.r;
    buffer[bufferpos + 1] = color.g;
    buffer[bufferpos + 2] = color.b;
    buffer[bufferpos + 3] = color.a;*/
    pset(buffer, bufferpos, color);
    bufferpos++;
  }
}

//Fast vertical line from (x,y1) to (x,y2), with rgb color
void verLine(unsigned char* buffer, int buffer_w, int buffer_h, int x, int y1, int y2, const ColorRGB& color)
{
  if(y2 < y1) std::swap(y1, y2); //swap y1 and y2 because y1 must be the topmost endpoint
  if(x < 0 || x > buffer_w - 1 || y2 < 0 || y1 > buffer_h - 1) return;
  
  for(int y = y1; y <= y2; y++)
  {
    int bufferpos = 4 * buffer_w * y + 4 * x;
    /*buffer[bufferpos + 0] = color.r;
    buffer[bufferpos + 1] = color.g;
    buffer[bufferpos + 2] = color.b;
    buffer[bufferpos + 3] = color.a;*/
    pset(buffer, bufferpos, color);
    bufferpos++;
  }
}

//this one does NOT check if the line is inside the buffer! (hence only width, not height, as parameter)
void bresenhamLine(unsigned char* buffer, int w, int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  //draw the line with bresenham
  int deltax = (int)std::abs((double)(x1 - x0));    // The difference between the x's
  int deltay = (int)std::abs((double)(y1 - y0));    // The difference between the y's
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
    if(!clipLine(x2, y2, x3, y3, x0, y0, x1, y1, clip.x0, clip.y0, clip.x1, clip.y1)) return;
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
, texture_alpha_as_opacity(true)
, color_alpha_as_opacity(true)
, extra_opacity(1.0)
{
  TextureFactory<TextureBuffer> factory;
}

ADrawer2DBuffer::~ADrawer2DBuffer()
{
}

void ADrawer2DBuffer::setTextureAlphaAsOpacity(bool set)
{
  texture_alpha_as_opacity = set;
}

void ADrawer2DBuffer::setColorAlphaAsOpacity(bool set)
{
  color_alpha_as_opacity = set;
}

void ADrawer2DBuffer::setExtraOpacity(double opacity)
{
  extra_opacity = opacity;
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

void ADrawer2DBuffer::drawGradientRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  drawGradientQuad(x0, y0, x0, y1, x1, y1, x1, y0, color0, color1, color2, color3);
}

void ADrawer2DBuffer::drawGradientTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2)
{
  Vector2 a(x0, y0);
  Vector2 b(x1, y1);
  Vector2 c(x2, y2);

  ColorRGB colora = color0;
  ColorRGB colorb = color1;
  ColorRGB colorc = color2;
  if(b.y < a.y) { std::swap(a, b); std::swap(colora, colorb); }
  if(c.y < a.y) { std::swap(a, c); std::swap(colora, colorc); }
  if(c.y < b.y) { std::swap(b, c); std::swap(colorb, colorc); }

  double stepxab = (b.x - a.x) / (b.y - a.y);
  double stepxac = (c.x - a.x) / (c.y - a.y);
  double stepxbc = (c.x - b.x) / (c.y - b.y);
  
  double ystart = a.y < 0.0 ? 0.0 : a.y;
  double yend = c.y > h ? h : c.y;
  
  for(double y = (int)ystart; y < yend; y++) //it's important that the x and y of every triangle starts at same fractional part (without, there are ugly pixels on the side and seams between touching triangles are visible), hence the (int) conversion.
  {
    double x0, x1;
    if(y < b.y)
    {
      x0 = a.x + (y - a.y) * stepxab;
      x1 = a.x + (y - a.y) * stepxac;
    }
    else
    {
      x0 = b.x + (y - b.y) * stepxbc;
      x1 = a.x + (y - a.y) * stepxac;
    }
    
    if(x1 < x0) std::swap(x0, x1);
    if(x0 < 0.0) x0 = 0.0;
    if(x1 > w) x1 = w;
    x0 = (int)x0; //it's important that the x and y of every triangle starts at same fractional part (without, there are ugly pixels on the side and seams between touching triangles are visible), hence the (int) conversion.
    
    //instead of doing the barycentric calculation for every x, do it only for begin and end and then step linearly
    double alpha, beta, gamma;
    double alpha2, beta2, gamma2;
    barycentric(alpha, beta, gamma, a, b, c,  Vector2(x0, y));
    barycentric(alpha2, beta2, gamma2, a, b, c,  Vector2(x1, y));
    double stepalphax = (alpha2 - alpha) / (x1 - x0);
    double stepbetax = (beta2 - beta) / (x1 - x0);
    double stepgammax = (gamma2 - gamma) / (x1 - x0);
    
    for(double x = (int)x0; x < x1; x++)
    {
      if(alpha >= 0.0 && beta >= 0.0 && gamma >= 0.0) //pixel inside triangle
      {
        ColorRGB color = alpha * colora + beta * colorb + gamma * colorc;
        pset(buffer, w, (int)x, (int)y, color);
      }
      
      alpha += stepalphax;
      beta += stepbetax;
      gamma += stepgammax;
    }
  }
}

void ADrawer2DBuffer::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  if(filled)
  {
    //not so super great implementation for this plain-colored triangle, but will fit nicely in a drawGradientTriangle function later thanks to the barycentric coordinates
    Vector2 a(x0, y0);
    Vector2 b(x1, y1);
    Vector2 c(x2, y2);

    if(b.y < a.y) std::swap(a, b);
    if(c.y < a.y) std::swap(a, c);
    if(c.y < b.y) std::swap(b, c);

    double stepxab = (b.x - a.x) / (b.y - a.y);
    double stepxac = (c.x - a.x) / (c.y - a.y);
    double stepxbc = (c.x - b.x) / (c.y - b.y);
    
    double ystart = a.y < 0.0 ? 0.0 : a.y;
    double yend = c.y > h ? h : c.y;
    
    for(double y = (int)ystart; y < yend; y++) //it's important that the x and y of every triangle starts at same fractional part (without, there are ugly pixels on the side and seams between touching triangles are visible), hence the (int) conversion.
    {
      double x0, x1;
      if(y < b.y)
      {
        x0 = a.x + (y - a.y) * stepxab;
        x1 = a.x + (y - a.y) * stepxac;
      }
      else
      {
        x0 = b.x + (y - b.y) * stepxbc;
        x1 = a.x + (y - a.y) * stepxac;
      }
      
      if(x1 < x0) std::swap(x0, x1);
      if(x0 < 0.0) x0 = 0.0;
      if(x1 > w) x1 = w;
      x0 = (int)x0; //it's important that the x and y of every triangle starts at same fractional part (without, there are ugly pixels on the side and seams between touching triangles are visible), hence the (int) conversion.
      
      //instead of doing the barycentric calculation for every x, do it only for begin and end and then step linearly
      double alpha, beta, gamma;
      double alpha2, beta2, gamma2;
      barycentric(alpha, beta, gamma, a, b, c,  Vector2(x0, y));
      barycentric(alpha2, beta2, gamma2, a, b, c,  Vector2(x1, y));
      double stepalphax = (alpha2 - alpha) / (x1 - x0);
      double stepbetax = (beta2 - beta) / (x1 - x0);
      double stepgammax = (gamma2 - gamma) / (x1 - x0);
      
      for(double x = (int)x0; x < x1; x++)
      {
        if(alpha >= 0.0 && beta >= 0.0 && gamma >= 0.0) //pixel inside triangle
        {
          pset(buffer, w, (int)x, (int)y, color);
        }
        
        alpha += stepalphax;
        beta += stepbetax;
        gamma += stepgammax;
      }
    }
  }
  else
  {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
  }
}

void ADrawer2DBuffer::drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled)
{
  (void)x0; (void)y0; (void)x1; (void)y1; (void)x2; (void)y2; (void)x3; (void)y3; (void)color; (void)filled;
  //TODO!
  if(filled)
  {
    //very lame temporary solution: draw it as some triangles (probably not always correct for non convex shapes!!)
    int cx = (x0 + x1 + x2 + x3) / 4;
    int cy = (y0 + y1 + y2 + y3) / 4;
    drawTriangle(x0, y0, x1, y1, cx, cy, color, true);
    drawTriangle(x1, y1, x2, y2, cx, cy, color, true);
    drawTriangle(x2, y2, x3, y3, cx, cy, color, true);
    drawTriangle(x3, y3, x0, y0, cx, cy, color, true);
  }
  else
  {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x0, y0, color);
  }
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
  //(void)x0; (void)y0; (void)x1; (void)y1; (void)x2; (void)y2; (void)x3; (void)y3; (void)color0; (void)color1; (void)color2; (void)color3;
  ////TODO!
  //drawQuad(x0, y0, x1, y1, x2, y2, x3, y3, color0, true); //very lame temporary implementation - no gradient at all...
  
  drawGradientTriangle(x0, y0, x1, y1, x3, y3, color0, color1, color3);
  drawGradientTriangle(x1, y1, x2, y2, x3, y3, color1, color2, color3);
}

bool ADrawer2DBuffer::supportsTexture(ITexture* texture)
{
  return dynamic_cast<TextureBuffer*>(texture);
}

ITexture* ADrawer2DBuffer::createTexture() const
{
  return new TextureBuffer();
}

ITexture* ADrawer2DBuffer::createTexture(ITexture* texture) const
{
  TextureBuffer* t = new TextureBuffer();
  makeTextureFromBuffer(t
                      , texture->getBuffer(), texture->getU2(), texture->getV2()
                      , AE_Nothing
                      , 0, 0, texture->getU(), texture->getV());
  return t;
}

/*
ob: output buffer (RGBA)
oi: index in output buffer
ib: input buffer (RGBA), this buffer is being blended over ob
ii: index in input buffer
colorMod: multiplied with output buffer
texture_alpha_as_opacity: is the alpha channel of the input texture treated as opacity?
extra_opacity: extra weighed average
*/
static void blend(unsigned char* ob, size_t oi, const unsigned char* ib, size_t ii
               , const ColorRGB& colorMod
               , bool texture_alpha_as_opacity, bool color_alpha_as_opacity, double extra_opacity)
{
  int r=0, g=0, b=0, a=0;

  if(color_alpha_as_opacity)
  {
    if(texture_alpha_as_opacity)
    {
      int ri = (ib[ii + 0] * colorMod.r) / 255;
      int gi = (ib[ii + 1] * colorMod.g) / 255;
      int bi = (ib[ii + 2] * colorMod.b) / 255;
      int ai = (ib[ii + 3] * colorMod.a) / 255;

      int ro = ob[oi + 0];
      int go = ob[oi + 1];
      int bo = ob[oi + 2];
      int ao = ob[oi + 3];

      int o = 255 - ((255 - ai) * ao) / 255;
      if(ai < ao) o = std::min(ai, o); //avoid color of fully transparent foreground leaking through
      r = (ri * o + ro * (255 - o)) / 255;
      g = (gi * o + go * (255 - o)) / 255;
      b = (bi * o + bo * (255 - o)) / 255;
      a = ai + ((255 - ai) * ao) / 255;
    }
    else
    {
      //TODO
    }
  }
  else
  {
    if(texture_alpha_as_opacity)
    {
      int ri = (ib[ii + 0] * colorMod.r) / 255;
      int gi = (ib[ii + 1] * colorMod.g) / 255;
      int bi = (ib[ii + 2] * colorMod.b) / 255;
      int ai = ib[ii + 3];
      
      int ro = ob[oi + 0];
      int go = ob[oi + 1];
      int bo = ob[oi + 2];
      int ao = ob[oi + 3];

      int o = 255 - ((255 - ai) * ao) / 255;
      if(ai < ao) o = std::min(ai, o); //avoid color of fully transparent foreground leaking through
      o = (o * ai) / 255;
      r = (ri * o + ro * (255 - o)) / 255;
      g = (gi * o + go * (255 - o)) / 255;
      b = (bi * o + bo * (255 - o)) / 255;
      //alpha channel: the intention is: if ia is 0, the alpha channel must become oa. If ia is 255, the alpha channel must become colorMod.a. For values in between: not sure yet, TODO!
      a = (colorMod.a * ai + ao * (255 - ai)) / 255;
    }
    else
    {
      r = ib[ii + 0];
      g = ib[ii + 1];
      b = ib[ii + 2];
      a = ib[ii + 3];
    }
  }
  
  if(extra_opacity != 1.0)
  {
    int o = (int)(255 * extra_opacity);
    ob[oi + 0] = (r * o + ob[oi + 0] * (255 - o)) / 255;
    ob[oi + 1] = (g * o + ob[oi + 1] * (255 - o)) / 255;
    ob[oi + 2] = (b * o + ob[oi + 2] * (255 - o)) / 255;
    ob[oi + 3] = (a * o + ob[oi + 3] * (255 - o)) / 255;
  }
  else
  {
    ob[oi + 0] = r;
    ob[oi + 1] = g;
    ob[oi + 2] = b;
    ob[oi + 3] = a;
  }
}
    
void ADrawer2DBuffer::drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod)
{
  const unsigned char* tb = texture->getBuffer();
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
    int bufferpos = (y + ty) * w * 4 + (x + tx) * 4;
    int tbufferpos = ty * tu2 * 4 + tx * 4;
    
    blend(buffer, bufferpos, tb, tbufferpos, colorMod, texture_alpha_as_opacity, color_alpha_as_opacity, extra_opacity);
  }
}

void ADrawer2DBuffer::drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  (void)texture; (void)x; (void)y; (void)sizex; (void)sizey; (void)colorMod;
  //TODO
}

void ADrawer2DBuffer::drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod)
{
  lpi::clipRect(x0, y0, x1, y1, x0, y0, x1, y1, clip.x0, clip.y0, clip.x1, clip.y1);

  const unsigned char* tb = texture->getBuffer();
  
    
  for(int y = y0, ty = 0; y < y1; y++)
  {
    for(int x = x0, tx = 0; x < x1; x++)
    {
      int bufferpos = 4 * (y * w + x);
      int tbufferpos = 4 * (ty * texture->getU2() + tx);
      

      blend(buffer, bufferpos, tb, tbufferpos, colorMod, texture_alpha_as_opacity, color_alpha_as_opacity, extra_opacity);

      tx++;
      if(tx >= (int)texture->getU()) tx = 0;
    }
    ty++;
    if(ty >= (int)texture->getV()) ty = 0;
  }
}

void ADrawer2DBuffer::drawTextureSizedRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  (void)sizex; (void)sizey; //TODO: use the size!!!
  
  lpi::clipRect(x0, y0, x1, y1, x0, y0, x1, y1, clip.x0, clip.y0, clip.x1, clip.y1);

  const unsigned char* tb = texture->getBuffer();


  for(int y = y0, ty = 0; y < y1; y++)
  {
    for(int x = x0, tx = 0; x < x1; x++)
    {
      int bufferpos = 4 * (y * w + x);
      int tbufferpos = 4 * (ty * texture->getU2() + tx);
      
      blend(buffer, bufferpos, tb, tbufferpos, colorMod, texture_alpha_as_opacity, color_alpha_as_opacity, extra_opacity);
  

      tx++;
      if(tx >= (int)texture->getU()) tx = 0;
    }
    ty++;
    if(ty >= (int)texture->getV()) ty = 0;
  }
}

void ADrawer2DBuffer::drawTextureGradient(const ITexture* texture, int x, int y
                                        , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11)
{
  drawTexture(texture, x, y, (color00+color01+color10+color11)/4);
}

void ADrawer2DBuffer::drawTextureRepeatedGradient(const ITexture* texture, int x0, int y0, int x1, int y1
                                                , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11)
{
  drawTextureRepeated(texture, x0, y0, x1, y1, (color00+color01+color10+color11)/4);
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
