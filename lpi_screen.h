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

#ifndef LPI_SCREEN_H_INCLUDED
#define LPI_SCREEN_H_INCLUDED

#include <SDL/SDL.h>
#include <vector>

#include "lpi_color.h"
#include "lpi_gl.h"

namespace lpi
{

////////////////////////////////////////////////////////////////////////////////
//BASIC SCREEN FUNCTIONS////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern int screenMode; //0 = 2D screen, 1 = 3D screen, this is used by the set2DScreen and set3DScreen functions to only change when not changed to this mode already

void screen(int width, int height, bool fullscreen, bool enable_fsaa, const char* text);
void lock();
void unlock();
void cls(const ColorRGB& color = RGB_Black);
void redraw();

} //namespace lpi

#endif

