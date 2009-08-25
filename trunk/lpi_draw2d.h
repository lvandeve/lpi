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

#ifndef LPI_DRAW2D_H_INCLUDED
#define LPI_DRAW2D_H_INCLUDED

#include "lpi_color.h"

#include <vector>
#include <string>

#include <iostream>

namespace lpi
{

void drawLine(unsigned char* buffer, int w, int h, int x1, int y1, int x2, int y2, const ColorRGB& color);
void drawLine(unsigned char* buffer, int w, int h, int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1, int clipy1, int clipx2, int clipy2);
void horLine(unsigned char* buffer, int w, int h, int y, int x1, int x2, const ColorRGB& color);
void verLine(unsigned char* buffer, int w, int h, int x, int y1, int y2, const ColorRGB& color);
void drawCircle(unsigned char* buffer, int w, int h, int x, int y, int radius, const ColorRGB& color);
void drawDisk(unsigned char* buffer, int buffer_w, int buffer_h, int xc, int yc, int radius, const ColorRGB& color);
void drawEllipse(unsigned char* buffer, int w, int h, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color);
void drawFilledEllipse(unsigned char* buffer, int w, int h, int cx, int cy, int radiusx, int radiusy, const ColorRGB& color);
void drawBezier(unsigned char* buffer, int w, int h, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color);
//drawPixel checks if the pixel is inside the buffer (it's simply not drawn if outside)
void drawPixel(unsigned char* buffer, int w, int h, int x, int y, const ColorRGB& color);
//pset does NOT check if the pixel is actually in the buffer (risk for out of bounds)
void pset(unsigned char* buffer, int w, int x, int y, const ColorRGB& color);


//helper functions for 2D graphics
bool bezier_nearly_flat(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);
bool clipLine(int x1, int y1, int x2, int y2, int& x3, int& y3, int& x4, int& y4, int left, int top, int right, int bottom);

} //end of namespace lpi

#endif
