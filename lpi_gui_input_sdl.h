#pragma once

#include "lpi_gui.h"
#include "lpi_event.h"

namespace lpi
{
namespace gui
{

//this uses the global variables for mouse states that lpi gets from SDL
class GUIInputSDL : public IGUIInput
{
  private:
    mutable KeyState keystate;
  public:
    virtual double getSeconds() const; //ticks in seconds
  
    virtual ~GUIInputSDL(){}
    //check position of mouse
    virtual int mouseX() const;
    virtual int mouseY() const;
    //check the state of the 3 buttons of mouse
    virtual bool mouseButtonDown(GUIMouseButton button) const;
    //check if scroll wheel of mouse was moved up or down
    virtual bool mouseWheelUp() const;
    virtual bool mouseWheelDown() const;
    
    //keyboard keys
    virtual bool keyDown(int key) const;
    virtual bool keyPressed(int key) const; //only returns true the first time the key is down and you check
    virtual int unicodeKey(int allowedChars, double time, double warmupTime = 0.5, double repTime = 0.025) const;
};

} //namespace gui
} //namespace lpi

namespace lpi
{

extern gui::GUIInputSDL gGUIInput; //global gui input usig SDL. Outside of namespace gui on purpose (too long name otherwise)

} //namespace lpi