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

#pragma once

#include <vector>

namespace lpi
{

//left or right mouse button
enum MouseButton
{
  LMB,
  RMB,
  MMB,
  NUM_MOUSE_BUTTONS //this must be the last value!!!

};

class IInput //TODO: move this interface in its own header file, and, make it outside of the "gui" namespace, general, for everything (that also fixes the problem with the two enums for mouse buttons, RMB versus RMB)
{
  public:
    virtual ~IInput(){}
    
    ///Time
    
    virtual double getSeconds() const = 0; //ticks in seconds, this is the time since the program started, NOT usable to get current date.
    
    ///Mouse
    
    //check position of mouse
    virtual int mouseX() const = 0;
    virtual int mouseY() const = 0;
    //check the state of the 3 buttons of mouse
    virtual bool mouseButtonDown(MouseButton button) const = 0;
    virtual bool mouseButtonDownTimed(MouseButton button, double warmupTime = 0.5, double repTime = 0.025) const = 0; //NOTE: only one thing can check this at the same time. It returns true only once per time interval, so if two things ask this during the same frame, only one will get "true" as result.
    
    virtual void setMousePos(int x, int y) const = 0;
    virtual void changeMousePos(int x, int y) const = 0;
    
    virtual bool doubleClicked(MouseButton button) const = 0;
    virtual bool tripleClicked(MouseButton button) const = 0;
    virtual bool quadrupleClicked(MouseButton button) const = 0;
    
    //check if scroll wheel of mouse was moved up or down
    virtual bool mouseWheelUp() const = 0;
    virtual bool mouseWheelDown() const = 0;
    
    //in pixels/second, this calculates the speed of the mouse (it should use a history of the position of multiple frames to be more accurate)
    virtual double mouseSpeedX() const = 0;
    virtual double mouseSpeedY() const = 0;
    
    ///Keyboard
    
    virtual bool keyDown(int key) const = 0;
    virtual bool keyPressed(int key) const = 0; //only returns true the first time the key is down and you check (can use mutable variable internally for this)
    virtual bool keyPressedTime(int key, double warmupTime = 0.5, double repTime = 0.025) const = 0; //NOTE: only one thing can check this at the same time. It returns true only once per time interval, so if two things ask this during the same frame, only one will get "true" as result.
    virtual int unicodeKey(double warmupTime = 0.5, double repTime = 0.025) const = 0;
};

class IInputClick : public IInput //this one already implements the double clicking in a way, and also the mouse speed
{
  protected:

    mutable char doubleClickState[NUM_MOUSE_BUTTONS];
    mutable double doubleClickTime[NUM_MOUSE_BUTTONS]; //in seconds
    mutable int doubleClickX[NUM_MOUSE_BUTTONS];
    mutable int doubleClickY[NUM_MOUSE_BUTTONS];
    
    mutable char tripleClickState[NUM_MOUSE_BUTTONS];
    mutable double tripleClickTime[NUM_MOUSE_BUTTONS]; //in seconds
    mutable int tripleClickX[NUM_MOUSE_BUTTONS];
    mutable int tripleClickY[NUM_MOUSE_BUTTONS];
    
    mutable char quadrupleClickState[NUM_MOUSE_BUTTONS];
    mutable double quadrupleClickTime[NUM_MOUSE_BUTTONS]; //in seconds
    mutable int quadrupleClickX[NUM_MOUSE_BUTTONS];
    mutable int quadrupleClickY[NUM_MOUSE_BUTTONS];
    
    mutable std::vector<int> mousePosXHistory;
    mutable std::vector<int> mousePosYHistory;
    mutable std::vector<double> mousePosXTimeHistory;
    mutable std::vector<double> mousePosYTimeHistory;
    
    mutable bool keyReleased[NUM_MOUSE_BUTTONS]; //for mouseButtonDownTimed
    mutable bool keyWarmedUp[NUM_MOUSE_BUTTONS]; //for mouseButtonDownTimed
    mutable double lastTime[NUM_MOUSE_BUTTONS]; //for mouseButtonDownTimed; last time it returned true
    
  private:
    double mouseSpeedImp(int pos, std::vector<int>& mousePosHistory, std::vector<double>& mousePosTimeHistory) const;

  public:
  
    IInputClick();

    virtual bool doubleClicked(MouseButton button) const;
    virtual bool tripleClicked(MouseButton button) const;
    virtual bool quadrupleClicked(MouseButton button) const;
    
    virtual double mouseSpeedX() const;
    virtual double mouseSpeedY() const;
    
    virtual bool mouseButtonDownTimed(MouseButton button, double warmupTime = 0.5, double repTime = 0.025) const;
};

} //namespace lpi
