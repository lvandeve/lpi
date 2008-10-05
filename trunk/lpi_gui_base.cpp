#include "lpi_gui_base.h"

namespace lpi
{
namespace gui
{

Pos<Sticky> STICKYTOPLEFT = { TOPLEFT, TOPLEFT, TOPLEFT, TOPLEFT };
Pos<Sticky> STICKYTOPRIGHT = { BOTTOMRIGHT, TOPLEFT, BOTTOMRIGHT, TOPLEFT };
Pos<Sticky> STICKYBOTTOMRIGHT = { BOTTOMRIGHT, BOTTOMRIGHT, BOTTOMRIGHT, BOTTOMRIGHT };
Pos<Sticky> STICKYBOTTOMLEFT = { TOPLEFT, BOTTOMRIGHT, TOPLEFT, BOTTOMRIGHT };
Pos<Sticky> STICKYFULL = { TOPLEFT, TOPLEFT, BOTTOMRIGHT, BOTTOMRIGHT };
Pos<Sticky> STICKYNOTHING = { NOTHING, NOTHING, NOTHING, NOTHING };
Pos<Sticky> STICKYVERTICALLEFT = { TOPLEFT, TOPLEFT, TOPLEFT, BOTTOMRIGHT };
Pos<Sticky> STICKYVERTICALRIGHT = { BOTTOMRIGHT, TOPLEFT, BOTTOMRIGHT, BOTTOMRIGHT };
Pos<Sticky> STICKYHORIZONTALTOP = { TOPLEFT, TOPLEFT, BOTTOMRIGHT, TOPLEFT };
Pos<Sticky> STICKYHORIZONTALBOTTOM = { TOPLEFT, BOTTOMRIGHT, BOTTOMRIGHT, BOTTOMRIGHT };
Pos<Sticky> STICKYRELATIVE = { RELATIVE00, RELATIVE00, RELATIVE11, RELATIVE11 };
Pos<Sticky> STICKYRELATIVE00 = { RELATIVE00, RELATIVE00, RELATIVE00, RELATIVE00 };
Pos<Sticky> STICKYRELATIVE01 = { RELATIVE00, RELATIVE11, RELATIVE00, RELATIVE11 };
Pos<Sticky> STICKYRELATIVE10 = { RELATIVE11, RELATIVE00, RELATIVE11, RELATIVE00 };
Pos<Sticky> STICKYRELATIVE11 = { RELATIVE11, RELATIVE11, RELATIVE11, RELATIVE11 };
Pos<Sticky> STICKYRELATIVEHORIZONTAL0 = { RELATIVE00, RELATIVE00, RELATIVE11, RELATIVE00 };
Pos<Sticky> STICKYRELATIVEVERTICAL0 = { RELATIVE00, RELATIVE00, RELATIVE00, RELATIVE11 };
Pos<Sticky> STICKYRELATIVEHORIZONTAL1 = { RELATIVE00, RELATIVE11, RELATIVE11, RELATIVE11 };
Pos<Sticky> STICKYRELATIVEVERTICAL1 = { RELATIVE11, RELATIVE00, RELATIVE11, RELATIVE11 };
Pos<Sticky> STICKYRELATIVEHORIZONTALFULL = { RELATIVE00, TOPLEFT, RELATIVE11, BOTTOMRIGHT };
Pos<Sticky> STICKYRELATIVEVERTICALFULL = { TOPLEFT, RELATIVE00, BOTTOMRIGHT, RELATIVE11 };
Pos<Sticky> STICKYRELATIVELEFT = { TOPLEFT, RELATIVE00, TOPLEFT, RELATIVE11 };
Pos<Sticky> STICKYRELATIVETOP = { RELATIVE00, TOPLEFT, RELATIVE11, TOPLEFT };
Pos<Sticky> STICKYRELATIVERIGHT = { BOTTOMRIGHT, RELATIVE00, BOTTOMRIGHT, RELATIVE11 };
Pos<Sticky> STICKYRELATIVEBOTTOM = { RELATIVE00, BOTTOMRIGHT, RELATIVE11, BOTTOMRIGHT };


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
}

////////////////////////////////////////////////////////////////////////////////
//BASICELEMENT
////////////////////////////////////////////////////////////////////////////////

int BasicElement::mouseGetRelPosX(const IGUIInput* input) const
{
  return input->mouseX() - x0;
}

int BasicElement::mouseGetRelPosY(const IGUIInput* input) const
{
  return input->mouseY() - y0;
}

bool BasicElement::mouseOverShape(const IGUIInput* input) const
{
  if(input->mouseX() >= x0 && input->mouseX() < x1 && input->mouseY() >= y0 && input->mouseY() < y1) return true;
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

bool BasicElement::mouseOver(const IGUIInput* input) const
{
  return mouseOverShape(input);
}

bool BasicElement::mouseDown(const IGUIInput* input, GUIMouseButton button) const
{
  return mouseOver(input) && input->mouseButtonDown(button);
}

bool BasicElement::mouseDownHere(MouseState& state, const IGUIInput* input, GUIMouseButton button) const
{
  bool down = input->mouseButtonDown(button);//mouseDown(button);
  bool over = mouseOver(input);
  
  if(!down)
  {
    state.downhere_bool1 = false;
    state.downhere_bool2 = false;
  }
  
  if(down && state.downhere_bool2 == false) //state.downhere_bool2 means justOver (it's the prevstate)
  {
    state.downhere_bool2 = true;
    state.downhere_bool1 = over; //true means it was inside when just clicked, false that it was not
  }
  
  return state.downhere_bool1 && over;
}

bool BasicElement::mouseDownHere(const IGUIInput* input, GUIMouseButton button)
{
  return mouseDownHere(_mouseState[button], input, button);
}

bool BasicElement::mouseGrabbed(MouseState& state, const IGUIInput* input, GUIMouseButton button) const
{
  if(!mouseGrabbable()) return false;
  
  //grab
  if(mouseJustDownHere(state.grabbed_prev, input, button))
  {
    state.grabbed_grabbed = true;
    state.grabx = input->mouseX();
    state.graby = input->mouseY();
    state.grabrelx = mouseGetRelPosX(input);
    state.grabrely = mouseGetRelPosY(input);
  }
  
  //ungrab
  if(!input->mouseButtonDown(button))
  {
    state.grabbed_grabbed = false;
    state.grabbed_prev = false;
  }

  return state.grabbed_grabbed;
}

bool BasicElement::mouseGrabbed(const IGUIInput* input, GUIMouseButton button)
{
  return mouseGrabbed(_mouseState[button], input, button);
}

void BasicElement::mouseGrab(MouseState& state, const IGUIInput* input) const
{
  state.grabbed_grabbed = true;
  state.grabx = input->mouseX();
  state.graby = input->mouseY();
  state.grabrelx = mouseGetRelPosX(input);
  state.grabrely = mouseGetRelPosY(input);
}

void BasicElement::mouseGrab(const IGUIInput* input, GUIMouseButton button)
{
  mouseGrab(_mouseState[button], input);
}


void BasicElement::mouseUngrab(MouseState& state) const
{
  state.grabbed_grabbed = false;
  state.grabbed_prev = false;
}

void BasicElement::mouseUngrab(GUIMouseButton button)
{
  mouseUngrab(_mouseState[button]);
}

bool BasicElement::mouseJustDown(bool& prevstate, const IGUIInput* input, GUIMouseButton button) const
{
  bool nowDown = mouseOver(input) && input->mouseButtonDown(button);
  
  if(nowDown)
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

bool BasicElement::mouseJustDown(MouseState& state, const IGUIInput* input, GUIMouseButton button) const
{
  return mouseJustDown(state.justdown_prev, input, button);
}

bool BasicElement::mouseJustDown(const IGUIInput* input, GUIMouseButton button)
{
  return mouseJustDown(_mouseState[button], input, button);
}

bool BasicElement::mouseJustDownHere(bool& prevstate, const IGUIInput* input, GUIMouseButton button) const
{
  bool over = mouseOver(input);
  bool nowDown = over && input->mouseButtonDown(button);
  
  if(nowDown)
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
  }
  
  if(!over) prevstate = true; //so that it can't return true anymore after mouse was not on this

  return false;
}

bool BasicElement::mouseJustDownHere(MouseState& state, const IGUIInput* input, GUIMouseButton button) const
{
  return mouseJustDownHere(state.justdownhere_prev, input, button);
}

bool BasicElement::mouseJustDownHere(const IGUIInput* input, GUIMouseButton button)
{
  return mouseJustDownHere(_mouseState[button], input, button);
}

bool BasicElement::mouseJustUpHere(MouseState& state, const IGUIInput* input, GUIMouseButton button) const
{
  if(mouseJustDownHere(state.justuphere_bool1, input, button))
  {
    state.justuphere_bool2 = true;
  }

  if(state.justuphere_bool2 && !input->mouseButtonDown(button))
  {
    state.justuphere_bool2 = false;
    if(mouseOver(input)) return true;
    else return false;
  }
  
  return false;
}

bool BasicElement::mouseJustUpHere(const IGUIInput* input, GUIMouseButton button)
{
  return mouseJustUpHere(_mouseState[button], input, button);
}

bool BasicElement::pressed(const IGUIInput* input, GUIMouseButton button)
{
  return mouseActive() && mouseJustDownHere(input, button);
}

bool BasicElement::clicked(const IGUIInput* input, GUIMouseButton button)
{
  return mouseActive() &&  mouseJustUpHere(input, button);
}

bool BasicElement::mouseScrollUp(const IGUIInput* input) const
{
  return mouseActive() && mouseOver(input) && input->mouseWheelUp();
}

bool BasicElement::mouseScrollDown(const IGUIInput* input) const
{
  return mouseActive() && mouseOver(input) && input->mouseWheelDown();
}

bool BasicElement::mouseDoubleClicked(MouseState& state, const IGUIInput* input, GUIMouseButton button) const
{
  bool dclick = 0;
  
  bool down = mouseOver(input) && input->mouseButtonDown(button); //is the button down

  switch(state.doubleClickState)
  {
    case 0:
      if(down)
      {
        state.doubleClickTime = input->getSeconds();
        state.doubleClickState = 1;
      }
      break;
    case 1:
      if(!down)
      {
        state.doubleClickState = 2;
      }
      break;
    case 2:
      if(down)
      {
        state.doubleClickState = 3;
      }
      break;
    case 3:
      if(!down)
      {
        state.doubleClickState = 0;
        if(input->getSeconds() - state.doubleClickTime < doubleClickTime) dclick = 1;
      }
      break;
  }
  
  if
  (
     (state.doubleClickState > 0 && (input->getSeconds() - state.doubleClickTime)  > doubleClickTime)
  || (!mouseOver(input))
  )
  {
    state.doubleClickState = 0;
    state.doubleClickTime = 0;
  }
  
  return dclick;
}

bool BasicElement::mouseDoubleClicked(const IGUIInput* input, GUIMouseButton button)
{
  return mouseDoubleClicked(_mouseState[button], input, button);
}

BasicElement::BasicElement() : x0(0),
                               y0(0),
                               x1(0),
                               y1(0),
                               doubleClickTime(500)
{}



////////////////////////////////////////////////////////////////////////////////




} //namespace gui
} //namespace lpi