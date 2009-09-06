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

#include "lpi_gui_input_sdl.h"

#include "lpi_time.h"

#include <cstdlib> //abs on int
#include <iostream>

namespace lpi
{
namespace gui
{

double GUIInputSDL::getSeconds() const
{
  return lpi::getSeconds();
}

int GUIInputSDL::mouseX() const
{
  return globalMouseX;
}

int GUIInputSDL::mouseY() const
{
  return globalMouseY;
}

bool GUIInputSDL::mouseButtonDown(GUIMouseButton button) const
{
  if(button == GUI_LMB)
    return globalLMB;
  else if(button == GUI_MMB)
    return globalMMB;
  else if(button == GUI_RMB)
    return globalRMB;
  else return false;
}

bool GUIInputSDL::mouseWheelUp() const
{
  return globalMouseWheelUp;
}

bool GUIInputSDL::mouseWheelDown() const
{
  return globalMouseWheelDown;
}

void GUIInputSDL::setMousePos(int x, int y) const
{
  lpi::setMousePos(x, y);
}

void GUIInputSDL::changeMousePos(int x, int y) const
{
  lpi::changeMousePos(x, y);
}

bool GUIInputSDL::keyDown(int key) const
{
  return lpi::keyDown(key);
}

bool GUIInputSDL::keyPressed(int key) const
{
  return lpi::keyPressed(key, &keystate);
}

bool GUIInputSDL::keyPressedTime(int key, double warmupTime, double repTime) const
{
  return lpi::keyPressedTime(key, getSeconds(), warmupTime, repTime, &keystate);
}

int GUIInputSDL::unicodeKey(double warmupTime, double repTime) const
{
  return lpi::unicodeKey(getSeconds(), warmupTime, repTime, &keystate);
}

} //namespace gui
} //namespace lpi

namespace lpi
{

gui::GUIInputSDL gGUIInput;

} //namespace lpi
