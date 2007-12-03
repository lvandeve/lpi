 
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//2D drawing with opengl

#ifndef LPI_DRAW2DGL_H_INCLUDED
#define LPI_DRAW2DGL_H_INCLUDED


#include <vector>
#include <cmath>

#include "lpi_screen.h"
#include "lpi_color.h"

namespace lpi
{

void pset(int x, int y, const ColorRGB& color);
void drawPoint(int x, int y, const ColorRGB& color); //same as pset but with other name
void drawLine(int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1 = 0, int clipy1 = 0, int clipx2 = screenWidth(), int clipy2 = screenHeight());
void drawRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color = RGB_White);
void drawDisk(int x, int y, double radius, const ColorRGB& color);
void gradientLine(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2);
void gradientRectangle(int x1, int y1, int x2, int y2, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4);
void drawGradientDisk(int x, int y, double radius, const ColorRGB& color1, const ColorRGB& color2);
} //end of namespace lpi

#endif
