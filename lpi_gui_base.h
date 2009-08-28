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
namespace gui
{

//direction for gui elements like scrollbars and stuff
enum Direction
{
  H, //horizontal
  V  //vertical
};

enum GUIMouseButton
{
  GUI_LMB,
  GUI_MMB,
  GUI_RMB,
  NUM_MOUSE_BUTTONS //this must be the last value!!!
};

class IGUIInput
{
  public:
    virtual ~IGUIInput(){}
    
    ///Time
    
    virtual double getSeconds() const = 0; //ticks in seconds
    
    ///Mouse
    
    //check position of mouse
    virtual int mouseX() const = 0;
    virtual int mouseY() const = 0;
    //check the state of the 3 buttons of mouse
    virtual bool mouseButtonDown(GUIMouseButton button) const = 0;
    
    virtual void setMousePos(int x, int y) const = 0;
    virtual void changeMousePos(int x, int y) const = 0;
    
    virtual bool doubleClicked(GUIMouseButton button) const = 0;
    virtual bool tripleClicked(GUIMouseButton button) const = 0;
    virtual bool quadrupleClicked(GUIMouseButton button) const = 0;
    
    //check if scroll wheel of mouse was moved up or down
    virtual bool mouseWheelUp() const = 0;
    virtual bool mouseWheelDown() const = 0;
    
    //in pixels/second, this calculates the speed of the mouse (it should use a history of the position of multiple frames to be more accurate)
    virtual double mouseSpeedX() const = 0;
    virtual double mouseSpeedY() const = 0;
    
    ///Keyboard
    
    virtual bool keyDown(int key) const = 0;
    virtual bool keyPressed(int key) const = 0; //only returns true the first time the key is down and you check (can use mutable variable internally for this)
    virtual bool keyPressedTime(int key, double warmupTime = 0.5, double repTime = 0.025) const = 0;
    virtual int unicodeKey(int allowedChars, double warmupTime = 0.5, double repTime = 0.025) const = 0;
};

class IGUIInputClick : public IGUIInput //this one already implements the double clicking in a way, and also the mouse speed
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
    
  private:
    double mouseSpeedImp(int pos, std::vector<int>& mousePosHistory, std::vector<double>& mousePosTimeHistory) const;

  public:

    virtual bool doubleClicked(GUIMouseButton button) const;
    virtual bool tripleClicked(GUIMouseButton button) const;
    virtual bool quadrupleClicked(GUIMouseButton button) const;
    
    virtual double mouseSpeedX() const;
    virtual double mouseSpeedY() const;
};

template<typename T>
struct Pos
{
  T x0;
  T y0;
  T x1;
  T y1;
};

struct Sticky
{
  bool sticky; //if true, is handled by resize using d and i, else not
  Pos<double> d;
  Pos<int> i;
  Pos<bool> follow; //if true, this side isn't fixed, it just follows the movement of the point indicated by d
  
  Sticky()
  : sticky(false)
  , d() //initializes it at 0,0,0,0
  , i() //initializes it at 0,0,0,0
  , follow() //initializes it to false,false,false,false
  {
  }
  
  Sticky(double dx0, int ix0, //left side
         double dy0, int iy0, //top side
         double dx1, int ix1, //right side
         double dy1, int iy1) //bottom side
  : sticky(true)
  , follow() //initializes it to false,false,false,false
  {
    d.x0 = dx0;
    d.y0 = dy0;
    d.x1 = dx1;
    d.y1 = dy1;
    i.x0 = ix0;
    i.y0 = iy0;
    i.x1 = ix1;
    i.y1 = iy1;
  }

  Sticky(double dx0, int ix0, bool followx0, //left side
         double dy0, int iy0, bool followy0, //top side
         double dx1, int ix1, bool followx1, //right side
         double dy1, int iy1, bool followy1) //bottom side
  : sticky(true)
  {
    d.x0 = dx0;
    d.y0 = dy0;
    d.x1 = dx1;
    d.y1 = dy1;
    i.x0 = ix0;
    i.y0 = iy0;
    i.x1 = ix1;
    i.y1 = iy1;
    follow.x0 = followx0;
    follow.y0 = followy0;
    follow.x1 = followx1;
    follow.y1 = followy1;
  }
};

static const Sticky STICKYOFF;
static const Sticky STICKYTOPLEFT(0.0, 0, true, 0.0, 0, true, 0.0, 0, true, 0.0, 0, true); //follow top left side
//static const Sticky STICKYFULL(0.0, 0, 1.0, 0, 0.0, 0, 1.0, 0);
static const Sticky& STICKYDEFAULT = STICKYTOPLEFT;


struct MouseState
{
  unsigned short downhere_bool1 : 1;
  unsigned short downhere_bool2 : 1;
  unsigned short justdown_prev : 1;
  unsigned short justdownhere_prev : 1;
  unsigned short justuphere_bool1 : 1;
  unsigned short justuphere_bool2 : 1;
  unsigned short grabbed_grabbed : 1;
  unsigned short grabbed_prev : 1;
  int grabx;
  int graby;
  int grabrelx;
  int grabrely;
  
  MouseState();
  
  bool mouseDownHere(bool over, bool down);
  bool mouseJustDown(bool over, bool down);
  bool mouseJustDownHere(bool over, bool down);
  bool mouseJustUpHere(bool over, bool down);
  
  bool mouseGrabbed(bool over, bool down, int x, int y, int relx, int rely);
  void mouseGrab(int x, int y, int relx, int rely);
  void mouseUngrab();
  
  int mouseGetGrabX() const { return grabx; }
  int mouseGetGrabY() const { return graby; }
  int mouseGetRelGrabX() const { return grabrelx; }
  int mouseGetRelGrabY() const { return grabrely; }
};

//separate struct from MouseState because it's independent from mouse button
struct MouseOverState
{
  bool over_prev;
  bool left_prev;
  
  MouseOverState();
  
  bool mouseJustOver(bool over_now);
  bool mouseJustLeft(bool over_now);
};

class ElementShape //describes the shape and mouse handling in this shape
{
  private:
    ////mouse related data
    MouseState _mouseState[NUM_MOUSE_BUTTONS];
    MouseOverState mouse_over_state;

  protected:

  public:
    ElementShape();
    virtual ~ElementShape(){};
    
    virtual bool isInside(int x, int y) const = 0; //Is point x, y inside this element? Can be overridden for elements with different shape, e.g. circle, or the gui::Group which's shape is that of all elements in it

    ////mouse related functions
    
    //these are virtual because other states can have an influence (e.g. due to other elements in Z order on top of it...)
    virtual bool mouseGrabbable() const { return true; }
    virtual bool mouseActive() const { return true; } //false if the element should not respond to "clicked", "pressed", etc..
    virtual bool mouseOver(const IGUIInput& input) const; //mouse cursor over the element (and no other element managed by gui container above it)
    
    bool mouseJustOver(const IGUIInput& input);
    bool mouseJustLeft(const IGUIInput& input);

    bool mouseDown(const IGUIInput& input, GUIMouseButton button = GUI_LMB) const; //mouse is over and down (does NOT return true if the mouse button is down but the mouse isn't inside this element)
    bool mouseDownHere(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //mouse is over, down, and was pressed while it was on here
    bool mouseJustDown(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //mouse down for the first time after being up or not over the element
    bool mouseJustDownHere(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //mouse down for the first time after being up, only returns true if the mouse was above it before you clicked already
    bool mouseJustUpHere(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //mouse up for first time after being down, and over the element (so if you have mouse down on element and move mouse away, this will NOT return true, only if you release mouse button while cursor is over it, and mousedownhere)
    bool pressed(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //mouseJustDown and active
    bool clicked(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //mouseJustUp and active

    //these use info from IGUIInput that isn't remembered by each element, so you need to pass the IGUIInput when calling these
    bool mouseScrollUp(const IGUIInput& input) const; //scrolled up while on this element
    bool mouseScrollDown(const IGUIInput& input) const; //scrolled down while on this element
    bool mouseDoubleClicked(const IGUIInput& input, GUIMouseButton button = GUI_LMB) const;
    bool mouseTripleClicked(const IGUIInput& input, GUIMouseButton button = GUI_LMB) const;
    bool mouseQuadrupleClicked(const IGUIInput& input, GUIMouseButton button = GUI_LMB) const;

    //everything related to grabbing - position is needed for this, hence some functions require IGUIInput parameter
    bool mouseGrabbed(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //like mouseDownHere, but keeps returning true if you move the mouse away from the element while keeping button pressed
    void mouseGrab(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //sets states as if element were grabbed
    void mouseUngrab(GUIMouseButton button = GUI_LMB); //sets states as if element were not grabbed
    int mouseGetGrabX(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetGrabX(); } //absolute location where you last started grabbing (x)
    int mouseGetGrabY(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetGrabY(); }
    int mouseGetRelGrabX(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetRelGrabX(); } //relative location where you last started grabbing (x)
    int mouseGetRelGrabY(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetRelGrabY(); }
    int mouseGetGrabDiffX(const IGUIInput& input, GUIMouseButton button = GUI_LMB) const; //return difference between grab position and current mouse position
    int mouseGetGrabDiffY(const IGUIInput& input, GUIMouseButton button = GUI_LMB) const;

    virtual int mouseGetRelPosX(const IGUIInput& input) const = 0; //returns relative mouse positions (relative to x and y of the elemnt)
    virtual int mouseGetRelPosY(const IGUIInput& input) const = 0; //returns relative mouse positions (relative to x and y of the elemnt)
};

class ElementRectangular : public ElementShape //all GUI elements inherit from this, but you can make other shapes than rectangle by overriding isInside, there will always be a rectangular bounding box to your shape though, that's the way the GUI is handled...
{
  protected:
    ////position
    int x0; //position of upper left corner of this element on screen
    int y0;
    int x1; //position of the bottom right corner of this element on screen
    int y1;

  public:
    ElementRectangular();
    virtual ~ElementRectangular(){};
    
    virtual bool isInside(int x, int y) const;
    
    int getX0() const { return x0; }
    int getY0() const { return y0; }
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    void setX0(int x0) { this->x0 = x0; }
    void setY0(int y0) { this->y0 = y0; }
    void setX1(int x1) { this->x1 = x1; }
    void setY1(int y1) { this->y1 = y1; }
    void initPosition(int x0, int y0, int x1, int y1);
    
    int getSizeX() const { return x1 - x0; } //get the size of this element
    int getSizeY() const { return y1 - y0; }
    //the functions below are best used after setting x0 and y0
    void setSizeX(int sizex) { x1 = x0 + sizex; } //change x1, y1 to get the given size
    void setSizeY(int sizey) { y1 = y0 + sizey; }
    void setSizeXY(int sizex, int sizey) { setSizeX(sizex); setSizeY(sizey); }
    int getCenterX() const { return (x0 + x1) / 2; } //the center in screen coordinates
    int getCenterY() const { return (y0 + y1) / 2; }
    int getRelCenterX() const { return (x1 - x0) / 2; } //the half of the size
    int getRelCenterY() const { return (y1 - y0) / 2; }
    
    virtual int mouseGetRelPosX(const IGUIInput& input) const; //returns relative mouse positions (relative to x and y of the elemnt)
    virtual int mouseGetRelPosY(const IGUIInput& input) const; //returns relative mouse positions (relative to x and y of the elemnt)
};


} //namespace gui
} //namespace lpi
