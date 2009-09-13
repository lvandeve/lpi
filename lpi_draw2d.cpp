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

#include "lpi_draw2d.h"

#include <vector>
#include <cmath>

namespace lpi
{

//helper-function for drawing bezier curves (a stop condition)
bool bezier_nearly_flat(double x0, double y0,
                        double x1, double y1,
                        double x2, double y2,
                        double x3, double y3)
{
    double dist = std::fabs(x0 - x3) + fabs(y0 - y3);
    
    double precision = 20; //20 = quite high, 5 = visible line segments, 2 = way too low
    
    if(std::fabs(x0 + x2 - x1 - x1) +
       std::fabs(y0 + y2 - y1 - y1) +
       std::fabs(x1 + x3 - x2 - x2) +
       std::fabs(y1 + y3 - y2 - y2) <= dist / precision)
    {
        return true;
    }
    else return false;
}

void recursive_bezier(unsigned char* buffer, int w, int h,
                      double x0, double y0, //endpoint
                      double x1, double y1, //handle
                      double x2, double y2, //handle
                      double x3, double y3, //endpoint
                      const ColorRGB& color,
                      int n = 0) //extra recursion test for safety
{
  if(bezier_nearly_flat(x0, y0, x1, y1, x2, y2, x3, y3) || n > 20)
  {
    drawLine(buffer, w, h, (int)x0, (int)y0, (int)x3, (int)y3, color);
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
    
    recursive_bezier(buffer, w, h, x0, y0, x01, y01, x012, y012, x0123, y0123, color, n + 1); 
    recursive_bezier(buffer, w, h, x0123, y0123, x123, y123, x23, y23, x3, y3, color, n + 1);
  }
}

void drawBezier(unsigned char* buffer, int w, int h, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color)
{
  recursive_bezier(buffer, w, h, x0, y0, x1, y1, x2, y2, x3, y3, color);
}

//this one does NOT check if the line is inside the buffer! (hence only width, not height, as parameter)
void bresenhamLine(unsigned char* buffer, int w, int x1, int y1, int x2, int y2, const ColorRGB& color)
{
  //draw the line with bresenham
  int deltax = std::abs((double)(x2 - x1));    // The difference between the x's
  int deltay = std::abs((double)(y2 - y1));    // The difference between the y's
  int x = x1;           // Start x off at the first pixel
  int y = y1;           // Start y off at the first pixel
  int xinc1, xinc2, yinc1, yinc2, den, num, numadd, numpixels, curpixel;

  if (x2 >= x1)         // The x-values are increasing
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else              // The x-values are decreasing
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  if (y2 >= y1)         // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else              // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  if (deltax >= deltay)     // There is at least one x-value for every y-value
  {
    xinc1 = 0;          // Don't change the x when numerator >= denominator
    yinc2 = 0;          // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;     // There are more x-values than y-values
  }
  else              // There is at least one y-value for every x-value
  {
    xinc2 = 0;          // Don't change the x for every iteration
    yinc1 = 0;          // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;     // There are more y-values than x-values
  }
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    pset(buffer, w, x, y, color);  // Draw the current pixel
    num += numadd;        // Increase the numerator by the top of the fraction
    if (num >= den)       // Check if numerator >= denominator
    {
      num -= den;       // Calculate the new numerator value
      x += xinc1;       // Change the x as appropriate
      y += yinc1;       // Change the y as appropriate
    }
    x += xinc2;         // Change the x as appropriate
    y += yinc2;         // Change the y as appropriate
  }
}

//Draw a line from (x1, y1) to (x2, y2) on a buffer where the color is 4 unsigned chars (with bresenham)
void drawLine(unsigned char* buffer, int w, int h, int x1, int y1, int x2, int y2, const ColorRGB& color)
{
  drawLine(buffer, w, h, x1, y1, x2, y2, color, 0, 0, w, h);
}

void drawLine(unsigned char* buffer, int buffer_w, int buffer_h, int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1, int clipy1, int clipx2, int clipy2)
{
  //make sure clipping area isn't bigger than the buffer
  if(clipx1 < 0) clipx1 = 0;
  if(clipy1 < 0) clipy1 = 0;
  if(clipx2 >= buffer_w) clipx2 = buffer_w - 1;
  if(clipy2 >= buffer_h) clipy2 = buffer_h - 1;
  
  //clip if some point is outside the clipping area
  if(x1 < clipx1 || x1 >= clipx2 || x2 < clipx1 || x2 >= clipx2 || y1 < clipy1 || y1 >= clipy2 || y2 < clipy1 || y2 >= clipy2)
  {
    int x3, y3, x4, y4;
    if(!clipLine(x1, y1, x2, y2, x3, y3, x4, y4, clipx1, clipy1, clipx2, clipy2)) return;
    x1 = x3;
    y1 = y3;
    x2 = x4;
    y2 = y4;
  }
  
  bresenhamLine(buffer, buffer_w, x1, y1, x2, y2, color);
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

void drawEllipse(unsigned char* buffer, int w, int h, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color)
{
  int twoASquare = 2*radiusx*radiusx;
  int twoBSquare = 2*radiusy*radiusy;
  int x = radiusx;
  int y = 0;
  int xchange = radiusy*radiusy*(1 - 2*radiusx);
  int ychange = radiusx*radiusx;
  int ellipseError = 0;
  int stoppingx = twoBSquare*radiusx;
  int stoppingy = 0;

  while(stoppingx >= stoppingy)
  {
    drawPixel(buffer, w, h, cx+x, cy+y, color);
    drawPixel(buffer, w, h, cx-x, cy+y, color);
    drawPixel(buffer, w, h, cx-x, cy-y, color);
    drawPixel(buffer, w, h, cx+x, cy-y, color);

    y++;
    stoppingy += twoASquare;
    ellipseError += ychange;
    ychange += twoASquare;
    if((2*ellipseError + xchange) > 0)
    {
      x--;
      stoppingx -= twoBSquare;
      ellipseError += xchange;
      xchange += twoBSquare;
    }
  }
        
  x = 0;
  y = radiusy;
  xchange = radiusy*radiusy;
  ychange = radiusx*radiusx*(1 - 2*radiusy);
  ellipseError = 0;
  stoppingx = 0;
  stoppingy = twoASquare*radiusy;
  while ( stoppingx <= stoppingy )
  {
    drawPixel(buffer, w, h, cx+x, cy+y, color);
    drawPixel(buffer, w, h, cx-x, cy+y, color);
    drawPixel(buffer, w, h, cx-x, cy-y, color);
    drawPixel(buffer, w, h, cx+x, cy-y, color);
    
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
  int twoASquare = 2*radiusx*radiusx;
  int twoBSquare = 2*radiusy*radiusy;
  int x = radiusx;
  int y = 0;
  int xchange = radiusy*radiusy*(1 - 2*radiusx);
  int ychange = radiusx*radiusx;
  int ellipseError = 0;
  int stoppingx = twoBSquare*radiusx;
  int stoppingy = 0;

  while(stoppingx >= stoppingy)
  {
    horLine(buffer, w, h, cy-y, cx-x, cx + x, color);
    horLine(buffer, w, h, cy+y, cx-x, cx + x, color);

    y++;
    stoppingy += twoASquare;
    ellipseError += ychange;
    ychange += twoASquare;
    if((2*ellipseError + xchange) > 0)
    {
      x--;
      stoppingx -= twoBSquare;
      ellipseError += xchange;
      xchange += twoBSquare;
    }
  }
        
  x = 0;
  y = radiusy;
  xchange = radiusy*radiusy;
  ychange = radiusx*radiusx*(1 - 2*radiusy);
  ellipseError = 0;
  stoppingx = 0;
  stoppingy = twoASquare*radiusy;
  while ( stoppingx <= stoppingy )
  {
    horLine(buffer, w, h, cy-y, cx-x, cx + x, color);
    horLine(buffer, w, h, cy+y, cx-x, cx + x, color);

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

void drawCircle(unsigned char* buffer, int w, int h, int cx, int cy, int radius, const ColorRGB& color)
{
  int x = radius;
  int y = 0;
  int xchange = 1 - 2*radius;
  int ychange = 1;
  int radiuserror = 0;
  while(x >= y)
  {
    drawPixel(buffer, w, h, cx + x, cy + y, color);
    drawPixel(buffer, w, h, cx - x, cy + y, color);
    drawPixel(buffer, w, h, cx - x, cy - y, color);
    drawPixel(buffer, w, h, cx + x, cy - y, color);
    drawPixel(buffer, w, h, cx + y, cy + x, color);
    drawPixel(buffer, w, h, cx - y, cy + x, color);
    drawPixel(buffer, w, h, cx - y, cy - x, color);
    drawPixel(buffer, w, h, cx + y, cy - x, color);
    y++;
    radiuserror += ychange;
    ychange += 2;
    if(2*radiuserror + xchange > 0 )
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


void drawPixel(unsigned char* buffer, int buffer_w, int buffer_h, int x, int y, const ColorRGB& color)
{
  if(x < 0 || y < 0 || x >= buffer_w || y >= buffer_h) return;
  
  pset(buffer, buffer_w, x, y, color);
}

//Set a pixel on a buffer
//pset does NOT check if the pixel is actually in the buffer (risk for out of bounds)
void pset(unsigned char* buffer, int buffer_w, int x, int y, const ColorRGB& color)
{
  int bufferPos = 4 * buffer_w * y + 4 * x;
  buffer[bufferPos + 0] = color.r;
  buffer[bufferPos + 1] = color.g;
  buffer[bufferPos + 2] = color.b;
  buffer[bufferPos + 3] = color.a;
}

  


//Functions for clipping a 2D line to the screen, which is the rectangle (0,0)-(w,h)
//This is the Cohen-Sutherland Clipping Algorithm
//Each of 9 regions gets an outcode, based on if it's at the top, bottom, left or right of the screen
// 1001 1000 1010  9 8 10
// 0001 0000 0010  1 0 2
// 0101 0100 0110  5 4 6
//int findregion returns which of the 9 regions a point is in, void clipline does the actual clipping
int findRegion(int x, int y, int left, int top, int right, int bottom)
{
  int code=0;
  if(y >= bottom)
  code |= 1; //top
  else if( y < top)
  code |= 2; //bottom
  if(x >= right)
  code |= 4; //right
  else if ( x < left)
  code |= 8; //left
  return(code);
}
bool clipLine(int x1, int y1, int x2, int y2, int & x3, int & y3, int & x4, int & y4, int left, int top, int right, int bottom)
{
  int code1, code2, codeout;
  bool accept = 0, done = 0;
  code1 = findRegion(x1, y1, left, top, right, bottom); //the region outcodes for the endpoints
  code2 = findRegion(x2, y2, left, top, right, bottom);
  do  //In theory, this can never end up in an infinite loop, it'll always come in one of the trivial cases eventually
  {
    if(!(code1 | code2)) accept = done = 1;  //accept because both endpoints are in screen or on the border, trivial accept
    else if(code1 & code2) done = 1; //the line isn't visible on screen, trivial reject
    else  //if no trivial reject or accept, continue the loop
    {
      int x, y;
      codeout = code1 ? code1 : code2;
      if(codeout & 1) //top
      {
        x = x1 + (x2 - x1) * (bottom - y1) / (y2 - y1);
        y = bottom - 1;
      }
      else if(codeout & 2) //bottom
      {
        x = x1 + (x2 - x1) * (top - y1) / (y2 - y1);
        y = top;
      }
      else if(codeout & 4) //right
      {
        y = y1 + (y2 - y1) * (right - x1) / (x2 - x1);
        x = right - 1;
      }
      else //left
      {
        y = y1 + (y2 - y1) * (left - x1) / (x2 - x1);
        x = left;
      }
      if(codeout == code1) //first endpoint was clipped
      {
        x1 = x; y1 = y;
        code1 = findRegion(x1, y1, left, top, right, bottom);
      }
      else //second endpoint was clipped
      {
        x2 = x; y2 = y;
        code2 = findRegion(x2, y2, left, top, right, bottom);
      }
    }
  }
  while(done == 0);

  if(accept)
  {
    x3 = x1;
    x4 = x2;
    y3 = y1;
    y4 = y2;
    return 1;
  }
  else
  {
    x3 = x4 = y3 = y4 = 0;
    return 0;
  }
}

void ADrawer2D::drawEllipse(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled)
{
  drawEllipseCentered((x0 + x1) / 2, (y0 + y1) / 2, x1 - x0, y1 - y0, color, filled);
}

void ADrawer2D::drawCircle(int x, int y, int radius, const ColorRGB& color, bool filled)
{
  drawEllipseCentered(x, y, radius, radius, color, filled);
}

void ADrawer2D::convertTextureIfNeeded(ITexture*& texture)
{
  if(!supportsTexture(texture))
  {
    ITexture* newtexture = createTexture(texture);
    delete texture;
    texture = newtexture;
  }
}

void ADrawer2D::drawTextureCentered(const ITexture* texture, int x, int y, const ColorRGB& colorMod)
{
  drawTexture(texture, x - texture->getU() / 2, y - texture->getV() / 2, colorMod);
}

void ADrawer2D::drawTextureSizedCentered(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod)
{
  drawTextureSized(texture, x, y, x - sizex / 2, y - sizey / 2, colorMod);
}

//void ADrawer2D::drawTextureTransformedCentered(const ITexture* texture, int x, int y, const double* matrix, const ColorRGB& colorMod)
//{
//  double cx = texture.getU() / 2.0;
//  double cy = texture.getV() / 2.0;
//  
//  double tx = matrix[0] * cx + matrix[1] * cy;
//  double ty = matrix[2] * cx + matrix[3] * cy;
//  
//  drawTextureTransformed(texture, x + (int)(cx - tx), y + (int)(cy - ty), matrix, colorMod);
//}

} //end of namespace lpi

