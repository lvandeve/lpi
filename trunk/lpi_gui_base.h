#pragma once

namespace lpi
{
namespace gui
{

enum GUIMouseButton
{
  GUI_LMB,
  GUI_RMB,
  GUI_MMB
};

class IGUIInput
{
  public:
    virtual ~IGUIInput(){}
    
    //TIME
    
    virtual double getSeconds() const = 0; //ticks in seconds
    
    //MOUSE
    
    //check position of mouse
    virtual int mouseX() const = 0;
    virtual int mouseY() const = 0;
    //check the state of the 3 buttons of mouse
    virtual bool mouseButtonDown(GUIMouseButton button) const = 0;
    //check if scroll wheel of mouse was moved up or down
    virtual bool mouseWheelUp() const = 0;
    virtual bool mouseWheelDown() const = 0;
    
    //KEYBOARD
    
    //keyboard keys
    virtual bool keyDown(int key) const = 0;
    virtual bool keyPressed(int key) const = 0; //only returns true the first time the key is down and you check (can use mutable variable internally for this)
    virtual int unicodeKey(int allowedChars, double time, double warmupTime = 0.5, double repTime = 0.025) const = 0;
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

const int NUM_MOUSE_BUTTONS = 2;

struct MouseState
{
  bool downhere_bool1;
  bool downhere_bool2;
  bool justdown_prev;
  bool justdownhere_prev;
  bool justuphere_bool1;
  bool justuphere_bool2;
  bool grabbed_grabbed;
  bool grabbed_prev;
  int grabx;
  int graby;
  int grabrelx;
  int grabrely;
  int doubleClickState;
  double doubleClickTime; //in seconds
  
  MouseState();
};

class BasicElement //more basic than "Element" - only describes the shape and mouse handling in this shape
{
  protected:
    ////position
    int x0; //position of upper left corner of this element on screen
    int y0;
    int x1; //position of the bottom right corner of this element on screen
    int y1;
    
    virtual bool mouseOverShape(const IGUIInput* input) const; //can be overridden for elements with different shape, e.g. circle, or the gui::Group which's shape is that of all elements in it
  
  public:
    BasicElement();
    
    int getX0() const { return x0; }
    int getY0() const { return y0; }
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    void setX0(int x0) { this->x0 = x0; }
    void setY0(int y0) { this->y0 = y0; }
    void setX1(int x1) { this->x1 = x1; }
    void setY1(int y1) { this->y1 = y1; }
    
    int getSizex() const { return x1 - x0; } //get the size of this element
    int getSizey() const { return y1 - y0; }
    //obviously the functoins below are best used after setting x0 and y0
    void setSizex(const int sizex) { x1 = x0 + sizex; } //change x1, y1 to get the given size
    void setSizey(const int sizey) { y1 = y0 + sizey; }
    int getCenterx() const { return (x0 + x1) / 2; } //the center in screen coordinates
    int getCentery() const { return (y0 + y1) / 2; }
    int getRelCenterx() const { return (x1 - x0) / 2; } //the half of the size
    int getRelCentery() const { return (y1 - y0) / 2; }

    ////MOUSE RELATED STUFF
    int mouseGetRelPosX(const IGUIInput* input) const; //returns relative mouse positions (relative to x and y of the elemnt)
    int mouseGetRelPosY(const IGUIInput* input) const; //returns relative mouse positions (relative to x and y of the elemnt)
    
    //these are virtual because other states can have an influence
    virtual bool mouseOver(const IGUIInput* input) const; //mouse cursor over the element (and no other element lpiged by gui container above it)
    virtual bool mouseGrabbable() const { return true; }
    virtual bool mouseActive() const { return true; } //false if the element should not respond to "clicked", "pressed", etc..
    
    bool mouseDown(const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //mouse is over and down
    bool mouseDownHere(MouseState& state, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //mouse is over, down, and was pressed while it was on here
    bool mouseDownHere(const IGUIInput* input, GUIMouseButton button = GUI_LMB);
    bool mouseGrabbed(MouseState& state, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //like mouseDownHere, but keeps returning true if you move the mouse away from the element while keeping button pressed
    bool mouseGrabbed(const IGUIInput* input, GUIMouseButton button = GUI_LMB);
    void mouseGrab(MouseState& state, const IGUIInput* input) const; //sets states as if element were grabbed
    void mouseGrab(const IGUIInput* input, GUIMouseButton button = GUI_LMB);
    void mouseUngrab(MouseState& state) const; //sets states as if element were not grabbed
    void mouseUngrab(GUIMouseButton button = GUI_LMB);
    
    int mouseGetGrabX(const MouseState& state) const { return state.grabx; } //absolute location where you last started grabbing (x)
    int mouseGetGrabX(GUIMouseButton button = GUI_LMB) const { return mouseGetGrabX(_mouseState[button]); }
    int mouseGetGrabY(const MouseState& state) const { return state.graby; }
    int mouseGetGrabY(GUIMouseButton button = GUI_LMB) const { return mouseGetGrabY(_mouseState[button]); }
    int mouseGetRelGrabX(const MouseState& state) const { return state.grabrelx; } //relative location where you last started grabbing (x)
    int mouseGetRelGrabX(GUIMouseButton button = GUI_LMB) const { return mouseGetRelGrabX(_mouseState[button]); }
    int mouseGetRelGrabY(const MouseState& state) const { return state.grabrely; }
    int mouseGetRelGrabY(GUIMouseButton button = GUI_LMB) const { return mouseGetRelGrabY(_mouseState[button]); }
    
    bool mouseJustDown(bool& prevstate, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //generalized version with only boolean given
    bool mouseJustDown(MouseState& state, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //mouse down for the first time after being up or not over the element
    bool mouseJustDown(const IGUIInput* input, GUIMouseButton button = GUI_LMB);
    
    bool mouseJustDownHere(bool& prevstate, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const;
    bool mouseJustDownHere(MouseState& state, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //mouse down for the first time after being up, only returns true if the mouse was above it before you clicked already
    bool mouseJustDownHere(const IGUIInput* input, GUIMouseButton button = GUI_LMB);
    
    bool mouseJustUpHere(MouseState& state, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //mouse up for first time after being down, and over the element (so if you have mouse down on element and move mouse away, this will NOT return true, only if you release mouse button while cursor is over it, and mousedownhere)
    bool mouseJustUpHere(const IGUIInput* input, GUIMouseButton button = GUI_LMB);
    
    bool pressed(const IGUIInput* input, GUIMouseButton button = GUI_LMB); //mouseJustDown and active
    bool clicked(const IGUIInput* input, GUIMouseButton button = GUI_LMB); //mouseJustUp and active

    bool mouseScrollUp(const IGUIInput* input) const; //scrolled up while on this element
    bool mouseScrollDown(const IGUIInput* input) const; //scrolled down while on this element
    
    void setDoubleClickTime(double i_doubleClickTime) { doubleClickTime = i_doubleClickTime; }
    double getDoubleClickTime() { return doubleClickTime; }
    
    //times are given in seconds
    bool mouseDoubleClicked(MouseState& state, const IGUIInput* input, GUIMouseButton button = GUI_LMB) const; //double clicked on this element
    bool mouseDoubleClicked(const IGUIInput* input, GUIMouseButton button = GUI_LMB);
    
    MouseState& getMouseStateForContainer() { return mouse_state_for_containers; }
    
    protected:
    
    double doubleClickTime; //maximum time for a doubleclick
    MouseState _mouseState[NUM_MOUSE_BUTTONS];
    MouseState mouse_state_for_containers; //for bookkeeping by containers that contain this element
};

} //namespace gui
} //namespace lpi