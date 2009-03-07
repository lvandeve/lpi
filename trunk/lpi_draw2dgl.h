/*
Copyright (c) 2005-2009 Lode Vandevenne
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

//2D drawing with opengl

#ifndef LPI_DRAW2DGL_H_INCLUDED
#define LPI_DRAW2DGL_H_INCLUDED


#include <vector>
#include <cmath>

#include "lpi_color.h"

namespace lpi
{
//NOTE: as much as possible, the "to" pixel itself is not included, e.g. a rectangle from 0-100 is 100 pixels, not 101 pixels in size (0, ..., 99 are filled, 100 not)
void pset(int x, int y, const ColorRGB& color);
void drawPoint(int x, int y, const ColorRGB& color); //same as pset but with other name
void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1, int clipy1, int clipx2, int clipy2);
void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color);
void drawRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color = RGB_White, bool filled = true);
void drawDisk(int x, int y, double radius, const ColorRGB& color);
void gradientLine(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2);
void gradientRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4);
void drawGradientDisk(int x, int y, double radius, const ColorRGB& color1, const ColorRGB& color2);
} //end of namespace lpi

#endif
