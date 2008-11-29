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

//const static int NUM_MOUSE_BUTTONS = 3; //lmb, mmb, rmb
const static int NUM_MOUSE_BUTTONS = 2; //lmb, rmb

enum GUIMouseButton
{
  GUI_LMB,
  //GUI_MMB,
  GUI_RMB
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
    
    virtual bool doubleClicked(GUIMouseButton button) const = 0;
    virtual bool tripleClicked(GUIMouseButton button) const = 0;
    virtual bool quadrupleClicked(GUIMouseButton button) const = 0;
    
    //check if scroll wheel of mouse was moved up or down
    virtual bool mouseWheelUp() const = 0;
    virtual bool mouseWheelDown() const = 0;
    
    ///Keyboard
    
    virtual bool keyDown(int key) const = 0;
    virtual bool keyPressed(int key) const = 0; //only returns true the first time the key is down and you check (can use mutable variable internally for this)
    virtual int unicodeKey(int allowedChars, double time, double warmupTime = 0.5, double repTime = 0.025) const = 0;
};

class IGUIInputClick : public IGUIInput //this one already implements the double clicking in a way
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

  public:

    virtual bool doubleClicked(GUIMouseButton button) const;
    virtual bool tripleClicked(GUIMouseButton button) const;
    virtual bool quadrupleClicked(GUIMouseButton button) const;
};

enum Sticky //how to resize child widget with master widget
{
  TOPLEFT, //follow top/left side of master
  BOTTOMRIGHT, //follow bottom/right side of master
  RELATIVE00, //position relative to master, distance of this subelement's side to upper left ("00") corner of subelement will stay constant
  RELATIVE11, //position relative to master, distance of this subelement's side to lower right ("11") corner of subelement will stay constant
  NOTHING //don't do anything in resize(). But in move() it'll still translate.
};

template<typename T>
struct Pos
{
  T x0;
  T y0;
  T x1;
  T y1;
};

//some named combinations for Pos<Sticky> values, more are possible but these are probably the most common
extern Pos<Sticky> STICKYTOPLEFT; //always keeps same size, stays at same position compared to parent's top left
extern Pos<Sticky> STICKYTOPRIGHT; //always keeps same size, stays at same position compared to parent's top right
extern Pos<Sticky> STICKYBOTTOMRIGHT; //always keeps same size, stays at same position compared to parent's bottom right
extern Pos<Sticky> STICKYBOTTOMLEFT; //always keeps same size, stays at same position compared to parent's bottom left
extern Pos<Sticky> STICKYFULL; //follows each corresponding side, so it resizes as much as its master
extern Pos<Sticky> STICKYNOTHING; //don't do anything in resize() of parent, but still move if the parent move()s
//for e.g. scrollbars:
extern Pos<Sticky> STICKYVERTICALLEFT; //follows both sides in vertical direction, follows left side
extern Pos<Sticky> STICKYVERTICALRIGHT; //follows both sides in vertical direction, follows right side
extern Pos<Sticky> STICKYHORIZONTALTOP; //follows both sides in horizontal direction, follows top side
extern Pos<Sticky> STICKYHORIZONTALBOTTOM; //follows both sides in horizontal direction, follows bottom side
//some involving relative:
extern Pos<Sticky> STICKYRELATIVE; //relative in all directions - use for subelements that may be resized too in all directions
extern Pos<Sticky> STICKYRELATIVE00; //position relative, but size stays constant (position determined by top left corner)
extern Pos<Sticky> STICKYRELATIVE01; //position relative, but size stays constant (position determined by bottom left corner)
extern Pos<Sticky> STICKYRELATIVE10; //position relative, but size stays constant (position determined by top right corner)
extern Pos<Sticky> STICKYRELATIVE11; //position relative, but size stays constant (position determined by bottom right corner)
extern Pos<Sticky> STICKYRELATIVEHORIZONTAL0; //size and position relative in horizontal direction, position relative in vertical direction
extern Pos<Sticky> STICKYRELATIVEVERTICAL0; //size and position relative in vertical direction, position relative in horizontal direction
extern Pos<Sticky> STICKYRELATIVEHORIZONTAL1; //size and position relative in horizontal direction, position relative in vertical direction
extern Pos<Sticky> STICKYRELATIVEVERTICAL1; //size and position relative in vertical direction, position relative in horizontal direction
extern Pos<Sticky> STICKYRELATIVEHORIZONTALFULL; //relative in horizontal direction, follows both sides in vertical direction
extern Pos<Sticky> STICKYRELATIVEVERTICALFULL; //relative in vertical direction, follows both sides in horizontal direction
extern Pos<Sticky> STICKYRELATIVELEFT; //relative in horizontal direction, follows left side in vertical direction
extern Pos<Sticky> STICKYRELATIVETOP; //relative in vertical direction, follows top side in horizontal direction
extern Pos<Sticky> STICKYRELATIVERIGHT; //relative in horizontal direction, follows right side in vertical direction
extern Pos<Sticky> STICKYRELATIVEBOTTOM; //relative in vertical direction, follows bottom side in horizontal direction

struct MouseOverState
{
  bool over_prev;
  bool left_prev;
  
  MouseOverState();
  
  bool mouseJustOver(bool over_now);
  bool mouseJustLeft(bool over_now);
};

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
  
  //same mouse functions but with custom MouseState
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

class ElementShape //describes the shape and mouse handling in this shape
{
  protected:
    ////position
    int x0; //position of upper left corner of this element on screen
    int y0;
    int x1; //position of the bottom right corner of this element on screen
    int y1;

  private:
    ////mouse related data
    MouseState _mouseState[NUM_MOUSE_BUTTONS];
    MouseOverState mouse_over_state;
    MouseState mouse_state_for_containers; //for bookkeeping by containers that contain this element TODO: make container itself remember one per element

  protected:

  public:
    ElementShape();
    virtual ~ElementShape(){};
    
    int getX0() const { return x0; }
    int getY0() const { return y0; }
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    void setX0(int x0) { this->x0 = x0; }
    void setY0(int y0) { this->y0 = y0; }
    void setX1(int x1) { this->x1 = x1; }
    void setY1(int y1) { this->y1 = y1; }
    
    int getSizeX() const { return x1 - x0; } //get the size of this element
    int getSizeY() const { return y1 - y0; }
    //the functions below are best used after setting x0 and y0
    void setSizeX(const int sizex) { x1 = x0 + sizex; } //change x1, y1 to get the given size
    void setSizeY(const int sizey) { y1 = y0 + sizey; }
    int getCenterX() const { return (x0 + x1) / 2; } //the center in screen coordinates
    int getCenterY() const { return (y0 + y1) / 2; }
    int getRelCenterX() const { return (x1 - x0) / 2; } //the half of the size
    int getRelCenterY() const { return (y1 - y0) / 2; }
    
    virtual bool isInside(int x, int y) const; //Is point x, y inside this element? Can be overridden for elements with different shape, e.g. circle, or the gui::Group which's shape is that of all elements in it

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
    int mouseGetRelPosX(const IGUIInput& input) const; //returns relative mouse positions (relative to x and y of the elemnt)
    int mouseGetRelPosY(const IGUIInput& input) const; //returns relative mouse positions (relative to x and y of the elemnt)

    //everything related to grabbing - position is needed for this, hence some functions require IGUIInput parameter
    bool mouseGrabbed(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //like mouseDownHere, but keeps returning true if you move the mouse away from the element while keeping button pressed
    void mouseGrab(const IGUIInput& input, GUIMouseButton button = GUI_LMB); //sets states as if element were grabbed
    void mouseUngrab(GUIMouseButton button = GUI_LMB); //sets states as if element were not grabbed
    int mouseGetGrabX(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetGrabX(); } //absolute location where you last started grabbing (x)
    int mouseGetGrabY(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetGrabY(); }
    int mouseGetRelGrabX(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetRelGrabX(); } //relative location where you last started grabbing (x)
    int mouseGetRelGrabY(GUIMouseButton button = GUI_LMB) const { return _mouseState[button].mouseGetRelGrabY(); }
    

    
    MouseState& getMouseStateForContainer() { return mouse_state_for_containers; }
};

} //namespace gui
} //namespace lpi