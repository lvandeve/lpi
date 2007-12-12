/*
Copyright (c) 2005-2007 Lode Vandevenne
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

#ifndef LPI_SCREEN_H_INCLUDED
#define LPI_SCREEN_H_INCLUDED

#include <SDL/SDL.h>
#include <vector>


#include <GL/gl.h>

#include "lpi_color.h"

namespace lpi
{

////////////////////////////////////////////////////////////////////////////////
//EXTERNAL VARIABLES////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int screenWidth();
int screenHeight();

void setScissor(int left, int top, int right, int bottom);
void setSmallestScissor(int left, int top, int right, int bottom); //same as setScissor, but will new scissor area will be inside the old scissor area, all parts outside are removed
void setOpenGLScissor();
void resetScissor();
bool onScreen(int x, int y);


////////////////////////////////////////////////////////////////////////////////
//BASIC SCREEN FUNCTIONS////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern int screenMode; //0 = 2D screen, 1 = 3D screen, this is used by the set2DScreen and set3DScreen functions to only change when not changed to this mode already

void screen(int width, int height, bool fullscreen, const char* text);
void initGL();
void set2DScreen();
void set3DScreen(double near, double far);
void lock();
void unlock();
void redraw();
void cls(const ColorRGB& color = RGB_Black);

void enableOneSided();
void enableTwoSided();
void enableZBuffer();
void disableZBuffer();
void enableSmoothing();
void disableSmoothing();
bool enabledSmoothing();




} //namespace lpi

#endif

