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

#include "lpi_input_sdl.h"

#include "lpi_time.h"

#include <cstdlib> //abs on int
#include <iostream>

namespace lpi
{
double InputSDL::getSeconds() const
{
  return lpi::getSeconds();
}

int InputSDL::mouseX() const
{
  return globalMouseX;
}

int InputSDL::mouseY() const
{
  return globalMouseY;
}

bool InputSDL::mouseButtonDown(MouseButton button) const
{
  if(button == LMB)
    return globalLMB;
  else if(button == MMB)
    return globalMMB;
  else if(button == RMB)
    return globalRMB;
  else return false;
}

bool InputSDL::mouseWheelUp() const
{
  return globalMouseWheelUp;
}

bool InputSDL::mouseWheelDown() const
{
  return globalMouseWheelDown;
}

void InputSDL::setMousePos(int x, int y) const
{
  lpi::setMousePos(x, y);
}

void InputSDL::changeMousePos(int x, int y) const
{
  lpi::changeMousePos(x, y);
}

bool InputSDL::keyDown(int key) const
{
  return lpi::keyDown(key);
}

bool InputSDL::keyPressed(int key) const
{
  return lpi::keyPressed(key, &keystate);
}

bool InputSDL::keyPressedTime(int key, double warmupTime, double repTime) const
{
  return lpi::keyPressedTime(key, getSeconds(), warmupTime, repTime, &keystate);
}

int InputSDL::unicodeKey(double warmupTime, double repTime) const
{
  return lpi::unicodeKey(getSeconds(), warmupTime, repTime, &keystate);
}


InputSDL gSDLInput;

} //namespace lpi
