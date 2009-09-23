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

#pragma once

#include "lpi_event.h"
#include "lpi_input.h"

namespace lpi
{

//this uses the global variables for mouse states that lpi gets from SDL
class InputSDL : public IInputClick
{
  private:
    mutable KeyState keystate;
  
  public:
    virtual double getSeconds() const; //ticks in seconds
  
    virtual ~InputSDL(){}
    //check position of mouse
    virtual int mouseX() const;
    virtual int mouseY() const;
    //check the state of the 3 buttons of mouse
    virtual bool mouseButtonDown(MouseButton button) const;
    //check if scroll wheel of mouse was moved up or down
    virtual bool mouseWheelUp() const;
    virtual bool mouseWheelDown() const;
    
    virtual void setMousePos(int x, int y) const;
    virtual void changeMousePos(int x, int y) const;
    
    //keyboard keys
    virtual bool keyDown(int key) const;
    virtual bool keyPressed(int key) const; //only returns true the first time the key is down and you check
    virtual bool keyPressedTime(int key, double warmupTime = 0.5, double repTime = 0.025) const;
    virtual int unicodeKey(double warmupTime = 0.5, double repTime = 0.025) const;
};

extern InputSDL gSDLInput; //TODO: remove this

} //namespace lpi
