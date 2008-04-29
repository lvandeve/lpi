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

#include "lpi_time.h"


namespace lpi
{

//using namespace std;



//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////
//NON GRAPHICS//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//


//function removed here as it couldn't work when multiple things test for doubleclicking
/*bool doubleClick(float time, bool button)*/

//Returns the time in integer milliseconds since the program started
Uint32 getTicks()
{
  return SDL_GetTicks();
}

} //namespace lpi
