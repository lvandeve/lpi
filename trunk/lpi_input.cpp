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

#include "lpi_input.h"

#include <cstdlib> //abs on int

namespace lpi
{
IInputClick::IInputClick()
{
  for(size_t i = 0; i < NUM_MOUSE_BUTTONS; i++)
  {
    doubleClickState[i] = 0;
    doubleClickTime[i] = 0;
    doubleClickX[i] = 0;
    doubleClickY[i] = 0;
    tripleClickState[i] = 0;
    tripleClickTime[i] = 0;
    tripleClickX[i] = 0;
    tripleClickY[i] = 0;
    quadrupleClickState[i] = 0;
    quadrupleClickTime[i] = 0;
    quadrupleClickX[i] = 0;
    quadrupleClickY[i] = 0;
    keyReleased[i] = false;
    keyWarmedUp[i] = false;
    lastTime[i] = 0;
  }
}

bool IInputClick::doubleClicked(MouseButton button) const
{
  double timeBetween = 0.5;
  bool dclick = 0;
  
  bool down = mouseButtonDown(button); //is the button down

  switch(doubleClickState[button])
  {
    case 0:
      if(down)
      {
        doubleClickTime[button] = getSeconds();
        doubleClickState[button] = 1;
        doubleClickX[button] = mouseX();
        doubleClickY[button] = mouseY();
      }
      break;
    case 1:
      if(!down)
        doubleClickState[button] = 2;
      break;
    case 2:
      if(down)
        doubleClickState[button] = 3;
      break;
    case 3:
      if(!down)
      {
        doubleClickState[button] = 0;
        if(getSeconds() - doubleClickTime[button] < timeBetween) dclick = 1;
      }
      break;
  }
  
  if
  (
     (doubleClickState[button] > 0 && (getSeconds() - doubleClickTime[button])  > timeBetween)
  || std::abs(mouseX() - doubleClickX[button]) > 1 || std::abs(mouseY() - doubleClickY[button]) > 1 //movement of 1 pixel allowed
  )
  {
    doubleClickState[button] = 0;
    doubleClickTime[button] = 0;
  }
  
  return dclick;
}

bool IInputClick::tripleClicked(MouseButton button) const
{
  double timeBetween = 0.75;
  bool tclick = 0;
  
  bool down = mouseButtonDown(button); //is the button down

  switch(tripleClickState[button])
  {
    case 0:
      if(down)
      {
        tripleClickTime[button] = getSeconds();
        tripleClickState[button] = 1;
        tripleClickX[button] = mouseX();
        tripleClickY[button] = mouseY();
      }
      break;
    case 1:
    case 3:
      if(!down)
        tripleClickState[button]++;
      break;
    case 2:
    case 4:
      if(down)
        tripleClickState[button]++;
      break;
    case 5:
      if(!down)
      {
        tripleClickState[button] = 0;
        if(getSeconds() - tripleClickTime[button] < timeBetween) tclick = 1;
      }
      break;
  }
  
  if
  (
     (tripleClickState[button] > 0 && (getSeconds() - tripleClickTime[button])  > timeBetween)
  || std::abs(mouseX() - tripleClickX[button]) > 1 || std::abs(mouseY() - tripleClickY[button]) > 1 //movement of 1 pixel allowed
  )
  {
    tripleClickState[button] = 0;
    tripleClickTime[button] = 0;
  }
  
  return tclick;
}

bool IInputClick::quadrupleClicked(MouseButton button) const
{
  double timeBetween = 1.0;
  bool tclick = 0;
  
  bool down = mouseButtonDown(button); //is the button down

  switch(quadrupleClickState[button])
  {
    case 0:
      if(down)
      {
        quadrupleClickTime[button] = getSeconds();
        quadrupleClickState[button] = 1;
        quadrupleClickX[button] = mouseX();
        quadrupleClickY[button] = mouseY();
      }
      break;
    case 1:
    case 3:
    case 5:
      if(!down)
        quadrupleClickState[button]++;
      break;
    case 2:
    case 4:
    case 6:
      if(down)
        quadrupleClickState[button]++;
      break;
    case 7:
      if(!down)
      {
        quadrupleClickState[button] = 0;
        if(getSeconds() - quadrupleClickTime[button] < timeBetween) tclick = 1;
      }
      break;
  }
  
  if
  (
     (quadrupleClickState[button] > 0 && (getSeconds() - quadrupleClickTime[button])  > timeBetween)
  || std::abs(mouseX() - quadrupleClickX[button]) > 1 || std::abs(mouseY() - quadrupleClickY[button]) > 1 //movement of 1 pixel allowed
  )
  {
    quadrupleClickState[button] = 0;
    quadrupleClickTime[button] = 0;
  }
  
  return tclick;
}

double IInputClick::mouseSpeedImp(int pos, std::vector<int>& mousePosHistory, std::vector<double>& mousePosTimeHistory) const
{
  static const size_t MIN_VALUES = 3;
  static const size_t MAX_VALUES = 4;
  //static const double TRACK_TIME = 0.1; //how a long mouse history we want to track
  
  mousePosHistory.push_back(pos);
  mousePosTimeHistory.push_back(getSeconds());
  
  double dt = mousePosTimeHistory.back() - mousePosTimeHistory[0];
  size_t size = mousePosHistory.size();
  
  
  double result = dt == 0 ? 0 : (double)(mousePosHistory.back() - mousePosHistory[0]) / dt;

  bool remove = false;
  if(size > MAX_VALUES) remove = true;
  //if(dt > TRACK_TIME) remove = true;
  if(size < MIN_VALUES) remove = false;

  if(remove)
  {
    mousePosHistory.erase(mousePosHistory.begin());
    mousePosTimeHistory.erase(mousePosTimeHistory.begin());
  }
  
  return result;
}

double IInputClick::mouseSpeedX() const
{
  return mouseSpeedImp(mouseX(), mousePosXHistory, mousePosXTimeHistory);
}

double IInputClick::mouseSpeedY() const
{
  return mouseSpeedImp(mouseY(), mousePosYHistory, mousePosYTimeHistory);
}

bool IInputClick::mouseButtonDownTimed(MouseButton button, double warmupTime, double repTime) const
{
  double time = getSeconds();
  if(mouseButtonDown(button))
  {
    if(keyReleased[button])
    {
      keyReleased[button] = false;
      keyWarmedUp[button] = false;
      lastTime[button] = time;
      return true;
    }
    else if(!keyWarmedUp[button])
    {
      if(time - lastTime[button] > warmupTime)
      {
        lastTime[button] = time;
        keyWarmedUp[button] = true;
        return true;
      }
      else return false;
    }
    else
    {
      if(time - lastTime[button] > repTime)
      {
        lastTime[button] = time;
        return true;
      }
      else return false;
    }
  }
  else keyReleased[button] = true;
  return false;
}

} //namespace lpi
