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

#include "lpi_gui_base.h"

#include <cstdlib> //abs on int

namespace lpi
{
namespace gui
{

////////////////////////////////////////////////////////////////////////////////

MouseOverState::MouseOverState()
: over_prev(false)
, left_prev(false)
{
}

bool MouseOverState::mouseJustOver(bool over_now)
{
  bool result = over_now && !over_prev;
  over_prev = over_now;
  return result;
}

bool MouseOverState::mouseJustLeft(bool over_now)
{
  bool result = !over_now && left_prev;
  left_prev = over_now;
  return result;
}

////////////////////////////////////////////////////////////////////////////////

bool mouseJustDown(bool& prevstate, bool over, bool down)
{
  if(over && down)
  {
    if(!prevstate)
    {
      prevstate = true;
      return true;
    }
    else return false;
  }
  else
  {
    prevstate = false;
    return false;
  }
}

bool mouseJustDownHere(bool& prevstate, bool over, bool down)
{
  if(over && down)
  {
    if(!prevstate)
    {
      prevstate = true;
      return true;
    }
    else return false;
  }
  else
  {
    if(over)
      prevstate = false;
    else
      prevstate = true; //so that it can't return true anymore after mouse was not on this
    return false;
  }
}

MouseState::MouseState()
{
  justdown_prev = 0;
  justdownhere_prev = 0;
  grabbed_grabbed = 0;
  grabbed_prev = 0;
  downhere_bool1 = 0;
  downhere_bool2 = 0;
  justuphere_bool1 = 0;
  justuphere_bool2 = 0;
  justdownelsewhere_prev = 0;
}

bool MouseState::mouseDownHere(bool over, bool down)
{
  if(!down)
  {
    downhere_bool1 = false;
    downhere_bool2 = false;
  }
  
  if(down && downhere_bool2 == false) //downhere_bool2 means justOver (it's the prevstate)
  {
    downhere_bool2 = true;
    downhere_bool1 = over; //true means it was inside when just clicked, false that it was not
  }
  
  return downhere_bool1 && over;
}

bool MouseState::mouseGrabbed(bool over, bool down, int x, int y, int relx, int rely)
{
  //grab
  bool temp_bool = grabbed_prev;
  if(lpi::gui::mouseJustDownHere(temp_bool, over, down))
  {
    grabbed_grabbed = true;
    grabx = x;
    graby = y;
    grabrelx = relx;
    grabrely = rely;
  }
  grabbed_prev = temp_bool;
  
  //ungrab
  if(!down)
  {
    grabbed_grabbed = false;
    grabbed_prev = false;
  }

  return grabbed_grabbed;
}

void MouseState::mouseUngrab()
{
  grabbed_grabbed = false;
  grabbed_prev = false;
}

bool MouseState::mouseJustDown(bool over, bool down)
{
  bool temp_bool = justdown_prev;
  bool result = lpi::gui::mouseJustDown(temp_bool, over, down);
  justdown_prev = temp_bool;
  return result;
}

bool MouseState::mouseJustDownHere(bool over, bool down)
{
  bool temp_bool = justdownhere_prev;
  bool result = lpi::gui::mouseJustDownHere(temp_bool, over, down);
  justdownhere_prev = temp_bool;
  return result;
}

bool MouseState::mouseJustUpHere(bool over, bool down)
{
  bool temp_bool = justuphere_bool1;
  if(lpi::gui::mouseJustDownHere(temp_bool, over, down))
  {
    justuphere_bool2 = true;
  }
  justuphere_bool1 = temp_bool;

  if(justuphere_bool2 && !down)
  {
    justuphere_bool2 = false;
    if(over) return true;
    else return false;
  }
  
  return false;
}

void MouseState::mouseGrab(int x, int y, int relx, int rely)
{
  grabbed_grabbed = true;
  grabx = x;
  graby = y;
  grabrelx = relx;
  grabrely = rely;
}

bool MouseState::mouseDownElsewhere(bool over, bool down, int x, int y, int relx, int rely)
{
  return !mouseGrabbed(over, down, x, y, relx, rely) && down && !over;
}

bool MouseState::mouseJustDownElsewhere(bool over, bool down)
{
  bool temp_bool = justdownelsewhere_prev;
  bool result = lpi::gui::mouseJustDownHere(temp_bool, !over, down);
  justdownelsewhere_prev = temp_bool;
  return result;
}


////////////////////////////////////////////////////////////////////////////////
//ELEMENTSHAPE
////////////////////////////////////////////////////////////////////////////////

bool ElementShape::mouseJustOver(const IInput& input)
{
  return mouse_over_state.mouseJustOver(mouseOver(input));
}

bool ElementShape::mouseJustLeft(const IInput& input)
{
  return mouse_over_state.mouseJustLeft(mouseOver(input));
}

bool ElementShape::mouseDown(const IInput& input, MouseButton button) const
{
  return mouseOver(input) && input.mouseButtonDown(button);
}

bool ElementShape::mouseDownHere(const IInput& input, MouseButton button)
{
  return _mouseState[button].mouseDownHere(mouseOver(input), input.mouseButtonDown(button));
}

bool ElementShape::mouseGrabbed(const IInput& input, MouseButton button)
{
  if(!mouseGrabbable()) return false;
  
  return _mouseState[button].mouseGrabbed(mouseOver(input)
                                        , input.mouseButtonDown(button)
                                        , input.mouseX()
                                        , input.mouseY()
                                        , mouseGetRelPosX(input)
                                        , mouseGetRelPosY(input));
}

void ElementShape::mouseGrab(const IInput& input, MouseButton button)
{
  return _mouseState[button].mouseGrab(input.mouseX()
                                     , input.mouseY()
                                     , mouseGetRelPosX(input)
                                     , mouseGetRelPosY(input));
}


void ElementShape::mouseUngrab(MouseButton button)
{
  _mouseState[button].mouseUngrab();
}

int ElementShape::mouseGetGrabDiffX(const IInput& input, MouseButton button) const
{
  return mouseGetRelGrabX(button) - mouseGetRelPosX(input);
}

int ElementShape::mouseGetGrabDiffY(const IInput& input, MouseButton button) const
{
  return mouseGetRelGrabY(button) - mouseGetRelPosY(input);
}

bool ElementShape::mouseJustDown(const IInput& input, MouseButton button)
{
  return _mouseState[button].mouseJustDown(mouseOver(input), input.mouseButtonDown(button));
}

bool ElementShape::mouseJustDownHere(const IInput& input, MouseButton button)
{
  return _mouseState[button].mouseJustDownHere(mouseOver(input), input.mouseButtonDown(button));
}

bool ElementShape::mouseJustUpHere(const IInput& input, MouseButton button)
{
  return _mouseState[button].mouseJustUpHere(mouseOver(input), input.mouseButtonDown(button));
}

bool ElementShape::mouseDownElsewhere(const IInput& input, MouseButton button)
{
  return _mouseState[button].mouseDownElsewhere(mouseOver(input)
                                              , input.mouseButtonDown(button)
                                              , input.mouseX()
                                              , input.mouseY()
                                              , mouseGetRelPosX(input)
                                              , mouseGetRelPosY(input));
}

bool ElementShape::mouseJustDownElsewhere(const IInput& input, MouseButton button)
{
  return _mouseState[button].mouseJustDownElsewhere(mouseOver(input), input.mouseButtonDown(button));
}


bool ElementShape::pressed(const IInput& input, MouseButton button)
{
  return mouseActive() && mouseJustDownHere(input, button);
}

bool ElementShape::clicked(const IInput& input, MouseButton button)
{
  return mouseActive() &&  mouseJustUpHere(input, button);
}

bool ElementShape::mouseScrollUp(const IInput& input) const
{
  return mouseActive() && mouseOver(input) && input.mouseWheelUp();
}

bool ElementShape::mouseScrollDown(const IInput& input) const
{
  return mouseActive() && mouseOver(input) && input.mouseWheelDown();
}

bool ElementShape::mouseDoubleClicked(const IInput& input, MouseButton button) const
{
  return mouseOver(input) && input.doubleClicked(button);
}

bool ElementShape::mouseTripleClicked(const IInput& input, MouseButton button) const
{
  return mouseOver(input) && input.tripleClicked(button);
}

bool ElementShape::mouseQuadrupleClicked(const IInput& input, MouseButton button) const
{
  return mouseOver(input) && input.quadrupleClicked(button);
}

ElementShape::ElementShape()
{
}

bool ElementShape::mouseOver(const IInput& input) const
{
  return isInside(input.mouseX(), input.mouseY());
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//ELEMENTRECTANGULAR
////////////////////////////////////////////////////////////////////////////////

ElementRectangular::ElementRectangular() : x0(0),
                                           y0(0),
                                           x1(0),
                                           y1(0)
{
}

void ElementRectangular::initPosition(int x0, int y0, int x1, int y1)
{
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
}

int ElementRectangular::mouseGetRelPosX(const IInput& input) const
{
  return input.mouseX() - x0;
}

int ElementRectangular::mouseGetRelPosY(const IInput& input) const
{
  return input.mouseY() - y0;
}

bool ElementRectangular::isInside(int x, int y) const
{
  if(x >= x0 && x < x1 && y >= y0 && y < y1) return true;
  else return false;
  
  
  //old "shape" code. Now, to get a shape like triangle, make child class of the gui element and override mouseOverShape with the new shape code
  /*switch(shape)
  {
    case 0: //rectangle
      over = 1;
      break;
    case 1: //triangle pointing up
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symX = std::abs(getSizex() / 2 - relX); //it's symmetrical
      if(relY >= (2 * symX * getSizey()) / getSizex()) over = 1;
      break;
    case 2: //triangle pointing right
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symY = std::abs(getSizey() / 2 - relY); //it's symmetrical
      if(getSizex() - relX >= (2 * symY * getSizex()) / getSizey()) over = 1;
      break;
    case 3: //triangle pointing down
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symX = std::abs(getSizex() / 2 - relX); //it's symmetrical
      if(getSizey() - relY >= (2 * symX * getSizey()) / getSizex()) over = 1;
    case 4: //triangle pointing left
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symY = std::abs(getSizey() / 2 - relY); //it's symmetrical
      if(relX >= (2 * symY * getSizey()) / getSizey()) over = 1;
      break;
    default: //rectangle
      over = 1;
      break;
  }*/
}




} //namespace gui
} //namespace lpi
