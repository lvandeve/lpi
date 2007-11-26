 
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DRAW2D_H
#define DRAW2D_H

#include "lpi_color.h"

#include <vector>
#include <string>

#include <iostream>

namespace lpi
{

void drawLine(unsigned char* buffer, int w, int h, int x1, int y1, int x2, int y2, const ColorRGB& color, int clipx1, int clipy1, int clipx2, int clipy2);
bool clipLine(int x1, int y1, int x2, int y2, int& x3, int& y3, int& x4, int& y4, int left, int top, int right, int bottom);
void horLine(unsigned char* buffer, int w, int h, int y, int x1, int x2, const ColorRGB& color);
void verLine(unsigned char* buffer, int w, int h, int x, int y1, int y2, const ColorRGB& color);
void drawDisk(unsigned char* buffer, int buffer_w, int buffer_h, int xc, int yc, int radius, const ColorRGB& color);
void pset(unsigned char* buffer, int w, int h, int x, int y, const ColorRGB& color);


} //end of namespace lpi

#endif
