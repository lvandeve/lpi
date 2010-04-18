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
#include <iostream>

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
  else if(y < top)
  code |= 2; //bottom
  if(x >= right)
  code |= 4; //right
  else if (x < left)
  code |= 8; //left
  return(code);
}
bool clipLine(int& ox0, int& oy0, int& ox1, int& oy1, int ix0, int iy0, int ix1, int iy1, int left, int top, int right, int bottom)
{
  int code0, code1, codeout;
  bool accept = 0, done = 0;
  code0 = findRegion(ix0, iy0, left, top, right, bottom); //the region outcodes for the endpoints
  code1 = findRegion(ix1, iy1, left, top, right, bottom);
  do  //In theory, this can never end up in an infinite loop, it'll always come in one of the trivial cases eventually
  {
    if(!(code0 | code1)) accept = done = true;  //accept because both endpoints are in screen or on the border, trivial accept
    else if(code0 & code1) done = true; //the line isn't visible on screen, trivial reject
    else  //if no trivial reject or accept, continue the loop
    {
      int x, y;
      codeout = code0 ? code0 : code1;
      if(codeout & 1) //top
      {
        x = ix0 + (ix1 - ix0) * (bottom - iy0) / (iy1 - iy0);
        y = bottom - 1;
      }
      else if(codeout & 2) //bottom
      {
        x = ix0 + (ix1 - ix0) * (top - iy0) / (iy1 - iy0);
        y = top;
      }
      else if(codeout & 4) //right
      {
        y = iy0 + (iy1 - iy0) * (right - ix0) / (ix1 - ix0);
        x = right - 1;
      }
      else //left
      {
        y = iy0 + (iy1 - iy0) * (left - ix0) / (ix1 - ix0);
        x = left;
      }
      if(codeout == code0) //first endpoint was clipped
      {
        ix0 = x; iy0 = y;
        code0 = findRegion(ix0, iy0, left, top, right, bottom);
      }
      else //second endpoint was clipped
      {
        ix1 = x; iy1 = y;
        code1 = findRegion(ix1, iy1, left, top, right, bottom);
      }
    }
  }
  while(done == 0);

  if(accept)
  {
    ox0 = ix0;
    ox1 = ix1;
    oy0 = iy0;
    oy1 = iy1;
    return 1;
  }
  else
  {
    ox0 = ox1 = oy0 = oy1 = 0;
    return 0;
  }
}


void clipRect(int& ox0, int& oy0, int& ox1, int& oy1, int ix0, int iy0, int ix1, int iy1, int left, int top, int right, int bottom)
{
  if(ix0 < left) ix0 = left;
  if(ix0 > right) ix0 = right;
  if(ix1 < left) ix1 = left;
  if(ix1 > right) ix1 = right;
  if(iy0 < top) iy0 = top;
  if(iy0 > bottom) iy0 = bottom;
  if(iy1 < top) iy1 = top;
  if(iy1 > bottom) iy1 = bottom;
  ox0 = ix0;
  oy0 = iy0;
  ox1 = ix1;
  oy1 = iy1;
}


void ADrawer2D::drawEllipse(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled)
{
  drawEllipseCentered((x0 + x1) / 2, (y0 + y1) / 2, (x1 - x0) / 2, (y1 - y0) / 2, color, filled);
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
  drawTextureSized(texture, x - sizex / 2, y - sizey / 2, sizex, sizey, colorMod);
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

