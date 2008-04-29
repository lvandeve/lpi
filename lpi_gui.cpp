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


#include "lpi_gui.h"
#include "lpi_gl.h"
#include "lpi_draw2dgl.h"
#include "lpi_time.h"
#include "lpi_math.h"

#include <iostream>

namespace lpi
{
namespace gui
{


void InternalContainer::resize(const Pos<int>& oldPos, const Pos<int>& newPos)
{
  int w = newPos.x1 - newPos.x0;
  int h = newPos.y1 - newPos.y0;
  int dx0 = newPos.x0 - oldPos.x0;
  int dy0 = newPos.y0 - oldPos.y0;
  int dx1 = newPos.x1 - oldPos.x1;
  int dy1 = newPos.y1 - oldPos.y1;
  
  for(size_t i = 0; i < elements.size(); i++)
  {
    Element& e = *elements[i];
    Pos<int> newPos2;
    
    if(e.sticky.x0 == TOPLEFT) newPos2.x0 = e.getX0() + dx0;
    else if(e.sticky.x0 == RELATIVE00) newPos2.x0 = newPos.x0 + (int)(e.relativePos.x0 * w);
    else if(e.sticky.x0 == RELATIVE11) newPos2.x0 = newPos.x0 + (int)(e.relativePos.x1 * w) - e.getSizex();
    else if(e.sticky.x0 == BOTTOMRIGHT) newPos2.x0 = e.getX0() + dx1;
    else /*if(e.sticky.x0 == NOTHING)*/ newPos2.x0 = e.getX0();
    
    if(e.sticky.y0 == TOPLEFT) newPos2.y0 = e.getY0() + dy0;
    else if(e.sticky.y0 == RELATIVE00) newPos2.y0 = newPos.y0 + (int)(e.relativePos.y0 * h);
    else if(e.sticky.y0 == RELATIVE11) newPos2.y0 = newPos.y0 + (int)(e.relativePos.y1 * h) - e.getSizey();
    else if(e.sticky.y0 == BOTTOMRIGHT) newPos2.y0 = e.getY0() + dy1;
    else /*if(e.sticky.x0 == NOTHING)*/ newPos2.y0 = e.getY0();
    
    if(e.sticky.x1 == TOPLEFT) newPos2.x1 = e.getX1() + dx0;
    else if(e.sticky.x1 == RELATIVE11) newPos2.x1 = newPos.x0 + (int)(e.relativePos.x1 * w);
    else if(e.sticky.x0 == RELATIVE00) newPos2.x1 = newPos.x0 + (int)(e.relativePos.x0 * w) + e.getSizex();
    else if(e.sticky.x1 == BOTTOMRIGHT) newPos2.x1 = e.getX1() + dx1;
    else /*if(e.sticky.x0 == NOTHING)*/ newPos2.x1 = e.getX1();
    
    if(e.sticky.y1 == TOPLEFT) newPos2.y1 = e.getY1() + dy0;
    else if(e.sticky.y1 == RELATIVE11) newPos2.y1 = newPos.y0 + (int)(e.relativePos.y1 * h);
    else if(e.sticky.y0 == RELATIVE00) newPos2.y1 = newPos.y0 + (int)(e.relativePos.y0 * h) + e.getSizey();
    else if(e.sticky.y1 == BOTTOMRIGHT) newPos2.y1 = e.getY1() + dy1;
    else /*if(e.sticky.x0 == NOTHING)*/ newPos2.y1 = e.getY1();
  
    e.resize(newPos2.x0, newPos2.y0, newPos2.x1, newPos2.y1);
  }
}

void InternalContainer::move(int x, int y)
{
  for(size_t i = 0; i < elements.size(); i++)
  {
    elements[i]->move(x, y);
  }
}

void InternalContainer::setElementOver(bool state)
{
  for(size_t i = 0; i < elements.size(); i++)
  {
    elements[i]->setElementOver(state);
  }
}

void InternalContainer::initSubElement(Element* element, const Pos<Sticky>& sticky, Element* parent)
{
  element->sticky = sticky;
  element->relativePos.x0 = (element->getX0() - parent->getX0()) / (double)(parent->getSizex());
  element->relativePos.y0 = (element->getY0() - parent->getY0()) / (double)(parent->getSizey());
  element->relativePos.x1 = (element->getX1() - parent->getX0()) / (double)(parent->getSizex());
  element->relativePos.y1 = (element->getY1() - parent->getY0()) / (double)(parent->getSizey());
}

void InternalContainer::addSubElement(Element* element, const Pos<Sticky>& sticky, Element* parent)
{
  elements.push_back(element);
  initSubElement(element, sticky, parent);
}

void InternalContainer::insertSubElement(size_t index, Element* element, const Pos<Sticky>& sticky, Element* parent)
{
  initSubElement(element, sticky, parent);
  elements.insert(elements.begin() + index, element);
}


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

int BasicElement::mouseGetRelPosX() const
{
  return globalMouseX - x0;
}

int BasicElement::mouseGetRelPosY() const
{
  return globalMouseY - y0;
}

bool BasicElement::mouseOverShape() const
{
  if(globalMouseX >= x0 && globalMouseX < x1 && globalMouseY >= y0 && globalMouseY < y1) return true;
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

bool BasicElement::mouseOver() const
{
  return mouseOverShape();
}

bool BasicElement::mouseDown(MouseButton button) const
{
  return mouseOver() && getGlobalMouseButton(button);
}

bool BasicElement::mouseDownHere(MouseState& state, MouseButton button) const
{
  bool down = getGlobalMouseButton(button);//mouseDown(button);
  bool over = mouseOver();
  
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

bool BasicElement::mouseDownHere(MouseButton button)
{
  return mouseDownHere(_mouseState[button], button);
}

bool BasicElement::mouseGrabbed(MouseState& state, MouseButton button) const
{
  if(!mouseGrabbable()) return false;
  
  //grab
  if(mouseJustDownHere(state.grabbed_prev, button))
  {
    state.grabbed_grabbed = true;
    state.grabx = globalMouseX;
    state.graby = globalMouseY;
    state.grabrelx = mouseGetRelPosX();
    state.grabrely = mouseGetRelPosY();
  }
  
  //ungrab
  if(!getGlobalMouseButton(button))
  {
    state.grabbed_grabbed = false;
    state.grabbed_prev = false;
  }

  return state.grabbed_grabbed;
}

bool BasicElement::mouseGrabbed(MouseButton button)
{
  return mouseGrabbed(_mouseState[button], button);
}

void BasicElement::mouseGrab(MouseState& state) const
{
  state.grabbed_grabbed = true;
  state.grabx = globalMouseX;
  state.graby = globalMouseY;
  state.grabrelx = mouseGetRelPosX();
  state.grabrely = mouseGetRelPosY();
}

void BasicElement::mouseGrab(MouseButton button)
{
  mouseGrab(_mouseState[button]);
}


void BasicElement::mouseUngrab(MouseState& state) const
{
  state.grabbed_grabbed = false;
  state.grabbed_prev = false;
}

void BasicElement::mouseUngrab(MouseButton button)
{
  mouseUngrab(_mouseState[button]);
}

bool BasicElement::mouseJustDown(bool& prevstate, MouseButton button) const
{
  bool nowDown = mouseOver() && getGlobalMouseButton(button);
  
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

bool BasicElement::mouseJustDown(MouseState& state, MouseButton button) const
{
  return mouseJustDown(state.justdown_prev, button);
}

bool BasicElement::mouseJustDown(MouseButton button)
{
  return mouseJustDown(_mouseState[button], button);
}

bool BasicElement::mouseJustDownHere(bool& prevstate, MouseButton button) const
{
  bool nowDown = mouseOver() && getGlobalMouseButton(button);
  
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
  
  if(!mouseOver()) prevstate = true; //so that it can't return true anymore after mouse was not on this

  return false;
}

bool BasicElement::mouseJustDownHere(MouseState& state, MouseButton button) const
{
  return mouseJustDownHere(state.justdownhere_prev, button);
}

bool BasicElement::mouseJustDownHere(MouseButton button)
{
  return mouseJustDownHere(_mouseState[button], button);
}

bool BasicElement::mouseJustUpHere(MouseState& state, MouseButton button) const
{
  if(mouseJustDownHere(state.justuphere_bool1, button))
  {
    state.justuphere_bool2 = true;
  }

  if(state.justuphere_bool2 && !getGlobalMouseButton(button))
  {
    state.justuphere_bool2 = false;
    if(mouseOver()) return true;
    else return false;
  }
  
  return false;
}

bool BasicElement::mouseJustUpHere(MouseButton button)
{
  return mouseJustUpHere(_mouseState[button], button);
}

bool BasicElement::pressed(MouseButton button)
{
  return mouseActive() && mouseJustDownHere(button);
}

bool BasicElement::clicked(MouseButton button)
{
  return mouseActive() &&  mouseJustUpHere(button);
}

bool BasicElement::mouseScrollUp() const
{
  return mouseActive() && mouseOver() && globalMouseWheelUp;
}

bool BasicElement::mouseScrollDown() const
{
  return mouseActive() && mouseOver() && globalMouseWheelDown;
}

bool BasicElement::mouseDoubleClicked(MouseState& state, MouseButton button) const
{
  bool dclick = 0;
  
  bool down = mouseOver() && getGlobalMouseButton(button); //is the button down

  switch(state.doubleClickState)
  {
    case 0:
      if(down)
      {
        state.doubleClickTime = getTicks() / 1000.0;
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
        if(getTicks() / 1000.0 - state.doubleClickTime < doubleClickTime) dclick = 1;
      }
      break;
  }
  
  if
  (
     (state.doubleClickState > 0 && (getTicks() / 1000.0) - state.doubleClickTime  > doubleClickTime)
  || (!mouseOver())
  )
  {
    state.doubleClickState = 0;
    state.doubleClickTime = 0;
  }
  
  return dclick;
}

bool BasicElement::mouseDoubleClicked(MouseButton button)
{
  return mouseDoubleClicked(_mouseState[button], button);
}

BasicElement::BasicElement() : x0(0),
                               y0(0),
                               x1(0),
                               y1(0),
                               doubleClickTime(500)
{}

////////////////////////////////////////////////////////////////////////////////
//TOOLTIPMANAGER                                                              //
////////////////////////////////////////////////////////////////////////////////

void ToolTipManager::registerMe(const Element* element)
{
  this->element = element;
}

void ToolTipManager::draw() const
{
  if(enabled && element)
    element->drawToolTip();
}

ToolTipManager defaultTooltipManager;

////////////////////////////////////////////////////////////////////////////////
//GUIELEMENT////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
GuiElement is the mother class of all the other GUI classes below.
*/

void Element::addSubElement(Element* element, const Pos<Sticky>& sticky)
{
  ic.addSubElement(element, sticky, this);
}

Element::Element() : selfActivate(false),
                     minSizex(0),
                     minSizey(0),
                     labelX(0),
                     labelY(0),
                     tooltipenabled(false),
                     hasBackgroundRectangle(false),
                     elementOver(false),
                     notDrawnByContainer(false)
{
  totallyEnable();
}

void Element::drawBorder(const ColorRGB& color) const
{
  drawLine(x0    , y0    , x0    , y1 - 1, color);
  drawLine(x1 - 1, y0    , x1 - 1, y1 - 1, color);
  drawLine(x0    , y0    , x1 - 1, y0     , color);
  drawLine(x0    , y1 - 1, x1 - 1, y1 - 1, color);
}

void Element::drawLabel() const
{
  if(label.length() > 0)
  print(label, x0 + labelX, y0 + labelY, labelMarkup);
}

void Element::drawToolTip() const
{
  if(tooltipenabled && mouseOver())
  {
    int numlines = 1;
    int linelength = 0;
    int longestline = 0;
    for(size_t i = 0; i < tooltip.size(); i++)
    {
      if(tooltip[i] == 10 || (tooltip[i] == 13 && (i == 0 || tooltip[i - 1] != 10)))
      {
        numlines++;
        if(linelength > longestline) longestline = linelength;
        linelength = 0;
      }
      else linelength++;
    }
    if(linelength > longestline) longestline = linelength;
    int sizex = longestline * TS_Black6.getWidth() + 4;
    int sizey = 2 +  TS_Black6.getHeight() * numlines;
    int x = globalMouseX;
    int y = globalMouseY - sizey;
    drawRectangle(x, y, x + sizex, y + sizey, RGBA_Lightyellow(224));
    printText(tooltip, x + 2, y + 2, TS_Black6);
  }
}

void Element::makeLabel(const std::string& label, int labelX, int labelY, const Markup& labelMarkup)
{
  this->label = label;
  this->labelX = labelX;
  this->labelY = labelY;
  this->labelMarkup = labelMarkup;
}

bool Element::mouseOver() const
{
  if(!present) return false;
  if(elementOver) return false; //there is another element over this element, so says the container containing this element
  
  return BasicElement::mouseOver();
}

bool Element::mouseGrabbable() const
{
  return present;
}


void Element::draw() const
{
  if(!visible) return;
  
  if(hasBackgroundRectangle) drawRectangle(x0, y0, x1, y1, backgroundRectangleColor);
  
  drawWidget();
  drawLabel();
  
  if(tooltipenabled && mouseOver()) tooltipmanager->registerMe(this);
}

/*void Element::drawWidget() const
{
  //this function is pure virtual
}*/

void Element::move(int x, int y)
{
  this->x0 += x;
  this->y0 += y;
  this->x1 += x;
  this->y1 += y;
  
  ic.move(x, y);
  
  moveWidget(x, y);
}

void Element::moveWidget(int /*x*/, int /*y*/)
{
  //this function is virtual
}

void Element::moveTo(int x, int y)
{
  this->move(x - this->x0, y - this->y0);
}

void Element::moveCenterTo(int x, int y)
{
  this->moveTo(x - this->getSizex() / 2, y - this->getSizey() / 2);
}

void Element::autoActivate()
{
  if(selfActivate)
  {
    if(mouseDownHere(auto_activate_mouse_state)) active = 1;
    else
    {
      if(!mouseOver() && globalLMB && !mouseGrabbed(auto_activate_mouse_state)) active = 0; //"forceActive" enabled so that this disactivating also works if mouseActive is false!
      //without the mouseGrabbed() test, it'll become inactive when you grab the scrollbar scroller and go outside with the mouse = annoying
    }
  }
}

void Element::handle()
{
  autoActivate();
  
  if(!active) return;
  
  handleWidget();
}

void Element::handleWidget()
{
  //no stuff needed for most elements
}

void Element::setElementOver(bool state)
{
  elementOver = state;
  
  ic.setElementOver(state);
}

bool Element::hasElementOver() const
{
  return elementOver;
}

bool Element::isContainer() const
{
  return 0;
}

//I have serious doubts if this function shouldn't be made obsolete: use a container the size of the screen instead and use it's function to keep elements inside...
//no wait the function is still handy here and there if you want to compare something to the screen, not it's container, or if there is no container at all
void Element::putInScreen()
{
    int newx = x0, newy = y0;
    
    if(x0 < 0) newx = 0;
    if(y0 < 0) newy = 0;
    if(x1 >= screenWidth()) newx = screenWidth() - getSizex();
    if(y1 >= screenHeight()) newy = screenWidth() - getSizey();
    
    moveTo(newx, newy);
}

void Element::resize(int x0, int y0, int x1, int y1)
{
  
  if(x1 - x0 < minSizex) x1 = x0 + minSizex;
  if(y1 - y0 < minSizey) y1 = y0 + minSizey;

  Pos<int> oldPos = { this->x0, this->y0, this->x1, this->y1 };
  Pos<int> newPos = { x0, y0, x1, y1 };
  
  resizeWidget(newPos);
  
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
  
  ic.resize(oldPos, newPos);
}

void Element::resizeWidget(const Pos<int>& /*newPos*/)
{
  //nothing to do. Overload this for guielements that need to do something to sub elements if they resize.
}

////////////////////////////////////////////////////////////////////////////////
//GUICONTAINER//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Contains many elements in it and draws them and handles them, and even makes
sure you can't press the mouse "through" elements on top of other elements.
*/

Container::Container()
{
  clear(); //clear the element list
  totallyEnable();
  
  //the default container is as big as the screen (note: don't forget to resize it if you resize the resolution of the screen!)
  x0 = 0;
  y0 = 0;
  x1 = screenWidth();
  y1 = screenHeight();
}

void Container::make(int x, int y, int sizex, int sizey)
{
  ic.getElements().clear();
  
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
}

void Container::setElementOver(bool state)
{
  Element::setElementOver(state);
  elements.setElementOver(state);
}

void Container::handleWidget()
{
  //if(mouseOver())
  if(!elementOver && mouseOver())
  {
    int topElement = -1;
    
    //priority to mouseGrabbed over mouseOver
    for(int i = size() - 1; i >= 0; i--)
    {
      elements.getElement(i)->setElementOver(0);
      if(elements.getElement(i)->mouseGrabbed(elements.getElement(i)->getMouseStateForContainer()))
      {
        if(topElement < 0) topElement = i;
        //break;
      }
      elements.getElement(i)->setElementOver(1);
    }
    
    //only now test mouseOver
    if(topElement == -1)
    for(int i = size() - 1; i >= 0; i--)
    {
      elements.getElement(i)->setElementOver(0);
      if(elements.getElement(i)->mouseOver())
      {
        topElement = i;
        break;
      }
      elements.getElement(i)->setElementOver(1);
    }

    //make all elements unresponsive to the mouse by setting "elementover", except the topElement
    for(size_t i = 0; i < size(); i++) elements.getElement(i)->setElementOver(1);
    if(topElement >= 0 && topElement < (int)size())
    {
      elements.getElement(topElement)->setElementOver(0);
      if(elements.getElement(topElement)->isContainer() && elements.getElement(topElement)->mouseDownHere(elements.getElement(topElement)->getMouseStateForContainer())) bringToTop(elements.getElement(topElement));
    }
  }
  else if(!elementOver) for(size_t i = 0; i < size(); i++) elements.getElement(i)->setElementOver(1); //mouse is over the bars!
  
  for(unsigned long i = 0; i < size(); i++)
  {
    elements.getElement(i)->handle();
  }
}

void Container::drawElements() const
{
  for(unsigned long i = 0; i < size(); i++)
  {
    if(!elements.getElement(i)->isNotDrawnByContainer())
    {
      elements.getElement(i)->draw();
    }
  }
}

void Container::drawWidget() const
{
  setSmallestScissor(x0, y0, x1, y1);
  drawElements();
  resetScissor();
}

unsigned long Container::size() const
{
  return elements.getElements().size();
}

void Container::remove(Element* element)
{
  size_t index = elements.findElementIndex(element);
  if(index >= size()) return;
  elements.getElements().erase(elements.getElements().begin() + index);
}

void Container::bringToTop(Element* element) //precondition: element must already be in the list
{
  size_t index = elements.findElementIndex(element);
  if(index >= size()) return;
  elements.getElements().erase(elements.getElements().begin() + index);
  elements.getElements().push_back(element);
}

void Container::pushTop(Element* element, const Pos<Sticky>& sticky)
{
  pushTopAt(element, element->getX0() - x0, element->getY0() - y0, sticky);
}

void Container::pushBottom(Element* element, const Pos<Sticky>& sticky)
{
  pushBottomAt(element, element->getX0() - x0, element->getY0() - y0, sticky);
}

void Container::insert(size_t pos, Element* element, const Pos<Sticky>& sticky)
{
  insertAt(pos, element, element->getX0() - x0, element->getY0() - y0, sticky);
}

void Container::pushTopRelative(Element* element, const Pos<Sticky>& sticky)
{
  pushTopAt(element, element->getX0(), element->getY0(), sticky);
}

void Container::pushBottomRelative(Element* element, const Pos<Sticky>& sticky)
{
  pushBottomAt(element, element->getX0(), element->getY0(), sticky);
}

void Container::insertRelative(size_t pos, Element* element, const Pos<Sticky>& sticky)
{
  insertAt(pos, element, element->getX0(), element->getY0(), sticky);
}

void Container::pushTopAt(Element* element, int x, int y, const Pos<Sticky>& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements.addSubElement(element, sticky, this);
}

void Container::pushBottomAt(Element* element, int x, int y, const Pos<Sticky>& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements.insertSubElement(0, element, sticky, this);
}

void Container::insertAt(size_t pos, Element* element, int x, int y, const Pos<Sticky>& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements.insertSubElement(pos, element, sticky, this);
}

void Container::centerElement(Element* element)
{
  element->moveCenterTo(getCenterx(), getCentery());
}

bool Container::isContainer() const
{
  return true;
}

void Container::clear()
{
  elements.getElements().clear();
}

void Container::putInside(unsigned long i)
{
  if(i < size())
  {
    int ex0 = elements.getElement(i)->getX0();
    int ey0 = elements.getElement(i)->getY0();
    int ex1 = elements.getElement(i)->getX1();
    int ey1 = elements.getElement(i)->getY1();
    int esx = ex1 - ex0;
    int esy = ey1 - ey0;
    
    int newx = ex0, newy = ey0;
    
    if(ex0 < getX0()) newx = getX0();
    if(ey0 < getY0()) newy = getY0();
    if(ex1 > getX1()) newx = getX1() - esx;
    if(ey1 > getY1()) newy = getY1() - esy;
    
    //if the size of the element is too large to fit in the window, there's no reason to move it (or it'll warp all the time)
    if(elements.getElement(i)->getSizex() > getSizex()) newx = elements.getElement(i)->getX0();
    if(elements.getElement(i)->getSizey() > getSizey()) newy = elements.getElement(i)->getY0();
    
    elements.getElement(i)->moveTo(newx, newy);
  }
}

void Container::moveWidget(int x, int y)
{
  elements.move(x, y);
}

void Container::getRelativeElementPos(Element& element, int& ex, int& ey) const
{
  ex = element.getX0() - x0;
  ey = element.getY0() - y0;
}

void Container::resizeWidget(const Pos<int>& newPos)
{
  Pos<int> oldPos = { this->x0, this->y0, this->x1, this->y1 };
  elements.resize(oldPos, newPos);
}

void Container::setSizeToElements()
{
  int newx0 = x0, newy0 = y0, newx1 = x1, newy1 = y1;
  if(size() > 0)
  {
    newx0 = elements.getElement(0)->getX0();
    newy0 = elements.getElement(0)->getY0();
    newx1 = elements.getElement(0)->getX1();
    newy1 = elements.getElement(0)->getY1();
  }
  for(unsigned i = 1; i < size(); i++)
  {
    if(elements.getElement(i)->getX0() < newx0) newx0 = elements.getElement(i)->getX0();
    if(elements.getElement(i)->getY0() < newy0) newy0 = elements.getElement(i)->getY0();
    if(elements.getElement(i)->getX1() > newx1) newx1 = elements.getElement(i)->getX1();
    if(elements.getElement(i)->getY1() > newy1) newy1 = elements.getElement(i)->getY1();
  }
  
  x0 = newx0;
  y0 = newy0;
  x1 = newx1;
  y1 = newy1;
}

////////////////////////////////////////////////////////////////////////////////
//SCROLLELEMENT///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
ScrollElement will contain 1 element. Depending on the size of that element compared to this ScrollElement,
1 or 2 scrollbars will appear, allowing to scroll to see all of that element.
The size of that element isn't affected by this ScrollElement.
The position of that element is completely controlled by this ScrollElement or its scrollbars
*/

void ScrollElement::drawWidget() const
{
  setSmallestScissor(getVisibleX0(), getVisibleY0(), getVisibleX1(), getVisibleY1()); //currently does same as setScissor (because otherwise there's weird bug, to reproduce: resize the red window and look at smiley in the small grey window), so elements from container in container are still drawn outside container. DEBUG THIS ASAP!!!
  element->draw();
  resetScissor();
  
  bars.draw();
}

void ScrollElement::moveWidget(int x, int y)
{
  if(element) element->move(x, y);
}

void ScrollElement::resizeWidget(const Pos<int>& /*newPos*/)
{
  updateBars(); //if this is done at the wrong moment, the bars may appear after resizing the container at times where it isn't desired
}

void ScrollElement::handleWidget()
{
  if(element) element->handle();
  
  bars.handle();
  int scrollx = x0 - int(bars.hbar.scrollPos); //the scrollpos is 0 if a bar is not enabled
  int scrolly = y0 - int(bars.vbar.scrollPos);
  moveAreaTo(scrollx, scrolly);
  
  updateBars();
}

void ScrollElement::setElementOver(bool state)
{
  Element::setElementOver(state);
  element->setElementOver(state);
}

ScrollElement::ScrollElement()
{
  element = 0;
}

void ScrollElement::make(int x, int y, int sizex, int sizey, Element* element)
{
  ic.getElements().clear();
  
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  
  bars.make(x, y, sizex, sizey);
  bars.disableV();
  bars.disableH();
  addSubElement(&bars, STICKYFULL);
  

  this->element = element;

  initBars();
}

void ScrollElement::moveAreaTo(int x, int y)
{
  if(element)element->moveTo(x, y);
}

void ScrollElement::initBars()
{
  if(!element) return;
  
  toggleBars();

  bars.vbar.scrollSize = element->getSizey() - getVisibleSizey();
  bars.hbar.scrollSize = element->getSizex() - getVisibleSizex();
  bars.vbar.scrollPos = y0 - element->getY0();
  bars.hbar.scrollPos = x0 - element->getX0();
}

void ScrollElement::updateBars()
{
  if(!element) return;
  
  toggleBars();

  bars.hbar.scrollSize = element->getSizex() - getVisibleSizex();
  bars.vbar.scrollSize = element->getSizey() - getVisibleSizey();
  if(bars.hbar.scrollSize < 0) bars.hbar.scrollSize = 0;
  if(bars.vbar.scrollSize < 0) bars.vbar.scrollSize = 0;
  if(bars.hbar.scrollPos > bars.hbar.scrollSize) bars.hbar.scrollPos = bars.hbar.scrollSize;
  if(bars.vbar.scrollPos > bars.vbar.scrollSize) bars.vbar.scrollPos = bars.vbar.scrollSize;
}

int ScrollElement::getVisibleX0() const
{
  return x0;
}

int ScrollElement::getVisibleY0() const
{
  return y0;
}

int ScrollElement::getVisibleX1() const
{
  if(bars.venabled) return x1 - bars.vbar.getSizex();
  else return x1;
}

int ScrollElement::getVisibleY1() const
{
  if(bars.henabled) return y1 - bars.hbar.getSizey();
  else return y1;
}

int ScrollElement::getVisibleSizex() const
{
  return getVisibleX1() - getVisibleX0();
}

int ScrollElement::getVisibleSizey() const
{
  return getVisibleY1() - getVisibleY0();
}

bool ScrollElement::mouseInVisibleZone() const
{
  if(!mouseOver()) return false;
  return globalMouseX >= getVisibleX0()
      && globalMouseX < getVisibleX1()
      && globalMouseY >= getVisibleY0()
      && globalMouseY < getVisibleY1();
}


void ScrollElement::toggleBars() //turns bars on or of if they're needed or not (depending on visible size and scroll area size) (different than MS Windows where scrollbars become inactive grey when not needed)
{
  if(!element) return;
  
  if(element->getSizex() <= getVisibleSizex() && element->getSizey() <= getVisibleSizey())
  {
    bars.disableH();
    bars.disableV();
  }
  else if(element->getSizex() > getVisibleSizex() && element->getSizey() <= getVisibleSizey())
  {
    bars.enableH();
    bars.disableV();
  }
  else if(element->getSizex() <= getVisibleSizex() && element->getSizey() > getVisibleSizey())
  {
    bars.disableH();
    bars.enableV();
  }
  else if(element->getSizex() > getVisibleSizex() && element->getSizey() > getVisibleSizey())
  {
    bars.enableH();
    bars.enableV();
  }
}

////////////////////////////////////////////////////////////////////////////////
//GUIGROUP//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Group::mouseOverShape() const
{
  for(int i = size() - 1; i >= 0; i--)
  {
    elements.getElement(i)->setElementOver(0);
    if(elements.getElement(i)->mouseOver()) return true;
  }
  
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//GUIPANEL//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Panel functions.
Panel is a class containing a backpanel, but it also shares the Element functions
*/

Panel::Panel()
{
  this->panel.colorMod = RGB_White;
  this->panel.fillColor = RGB_White;
  this->panel.enableSides = 0;
  this->panel.enableCenter = 0;
  this->panel.t00 = &emptyTexture;
  this->panel.t01 = &emptyTexture;
  this->panel.t02 = &emptyTexture;
  this->panel.t10 = &emptyTexture;
  this->panel.t11 = &emptyTexture;
  this->panel.t12 = &emptyTexture;
  this->panel.t20 = &emptyTexture;
  this->panel.t21 = &emptyTexture;
  this->panel.t22 = &emptyTexture;
}

void Panel::makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  
  this->panel.makeUntextured(fillColor);
}

void Panel::makeTextured(int x, int y, int sizex, int sizey,
       const Texture* t00, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  
  this->panel.makeTextured(t00, colorMod);
}

void Panel::make(int x, int y, int sizex, int sizey,
       const GuiSet* set)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  
  this->panel = *set->windowPanel;
}

//this function should become obsolete after there is a general "moveTo, resize" combo function which is planned to be made
void Panel::setSize(int x, int y, int sizex, int sizey)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
}

void Panel::drawWidget() const
{
  panel.draw(x0, y0, getSizex(), getSizey());
}

////////////////////////////////////////////////////////////////////////////////
//GUILINE//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Rule is a class containing a BackLine, but it also shares the Element functions
*/

Rule::Rule()
{
  this->line.colorMod = RGB_White;
  this->line.enableSides = 0;
  this->line.t0 = &emptyTexture;
  this->line.t1 = &emptyTexture;
  this->line.t2 = &emptyTexture;
}

void Rule::makeHorizontal(int x, int y, int length, const GuiSet* set)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(length);
  this->setSizey(set->hline[0]->getV());
  this->totallyEnable();
  
  this->line.makeHorizontal1(set->hline[0], set->mainColor);
}

void Rule::makeHorizontal1(int x, int y, int length, Texture* t0, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(length);
  this->setSizey(t0->getV());
  this->totallyEnable();
  
  this->line.makeHorizontal1(t0, colorMod);
}

void Rule::makeVertical(int x, int y, int length, const GuiSet* set)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizey(length);
  this->setSizex(set->vline[0]->getU());
  this->totallyEnable();
  
  this->line.makeVertical1(set->vline[0], set->mainColor);
}

void Rule::makeVertical1(int x, int y, int length, Texture* t0, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizey(length);
  this->setSizex(t0->getU());
  this->totallyEnable();
  
  this->line.makeVertical1(t0, colorMod);
}

void Rule::setSize(int x, int y, int length)
{
  this->x0 = x;
  this->y0 = y;
  if(line.direction == H) this->setSizex(length);
  else this->setSizey(length);
}

void Rule::drawWidget() const
{
  if(line.direction == H) line.draw(x0, y0, getSizex());
  else line.draw(x0, y0, getSizey());
}

////////////////////////////////////////////////////////////////////////////////
//GUIWINDOW/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Window is a window that contains pointers to any kind of other Elements.

When the window gets drawn, all Elements get drawn as well.

When the window moves, all Elements are moved as well.

You can still operate the Elements like you normally do.

The window also contains a Panel for it's own background, and will have a bar
at the top that allows you to drag the window if you click it with the mouse.

Things like the drawing order of the window, or detecting if you click on the
window and not something behind it, you'll have to do yourself...
*/

Window::Window()
{
  this->active = 0;
  this->visible = 0;
  this->enableResizer = 0;
  this->closed = 0;
  this->resizerOverContainer = 0;
  
  addSubElement(&top, STICKYHORIZONTALTOP);
  addSubElement(&resizer, STICKYBOTTOMRIGHT);
  addSubElement(&container, STICKYFULL);
  addSubElement(&closeButton, STICKYTOPRIGHT);
  
  minSizex = 64;
  minSizey = 64;
}

int Window::getRelContentStart() const
{
  int result = 0;
  if(enableTop) result = top.getY1() - y0;
  
  return result;
}

void Window::addCloseButton(int offsetX, int offsetY, const GuiSet* set)
{
  int closeX = x1 + offsetX - set->closeButton->getU();
  int closeY = y0 + offsetY;
  closeButton.makeImage(closeX, closeY, set->closeButton, set->closeButton, set->closeButton, set->mainColor, set->mouseOverColor, set->mouseDownColor);
  closed = 0;
  closeEnabled = 1;
}

void Window::addResizer(const GuiSet* set, bool overContainer, int offsetX, int offsetY)
{
  int resizerX = x1 - offsetX - set->resizer->getU();
  int resizerY = y1 - offsetY - set->resizer->getV();
  resizer.makeImage(resizerX, resizerY, set->resizer, set->resizer, set->resizer, set->mainColor, set->mouseOverColor, set->mouseDownColor);
  enableResizer = true;
  
  if(!overContainer)
  {
    container.resize(container.getX0(), container.getY0(), container.getX1(), container.getY1() - (y1 - resizer.getY0()));
  }
  
  resizerOverContainer = overContainer;
}

//returns the lowest position the container bottom side is allowed to have (for example not over resizer if resizerOverContainer is false
int Window::getContainerLowest() const
{
  int pos = y1;
  if(enableResizer && !resizerOverContainer) pos = resizer.getY0();
  
  return pos;
}
//returns the highest position the container top side is allowed to have 
int Window::getContainerHighest() const
{
  int pos = y0;
  if(enableTop) pos = top.getY1();
  
  return pos;
}
//returns the leftmost position the container left side is allowed to have 
int Window::getContainerLeftmost() const
{
  return x0;
}
//returns the rightmost position the container right side is allowed to have 
int Window::getContainerRightmost() const
{
  return x1;
}

void Window::setContainerBorders(int left, int up, int right, int down)
{
  if(up < 0) up = left;
  if(right < 0) right = left;
  if(down < 0) down = left;
  
  container.resize(getContainerLeftmost() + left, getContainerHighest() + up, getContainerRightmost() - right, getContainerLowest() - down);
}

bool Window::isContainer() const
{
  return true;
}

void Window::makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  this->enableTop = 0;
  
  this->panel.makeUntextured(fillColor);
  
  initContainer();
}

void Window::makeTextured(int x, int y, int sizex, int sizey,
       const Texture* t00, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  
  this->enableTop = 0;
  
  this->panel.makeTextured(t00, colorMod);
  
  initContainer();
}

void Window::make(int x, int y, int sizex, int sizey,
       const GuiSet* set)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  
  this->enableTop = 0;
  
  this->panel = *set->windowPanel;
  
  initContainer();
}

void Window::addTop(Texture* t0, int offsetLeft, int offsetRight, int offsetTop, const ColorRGB& colorMod)
{
  top.makeHorizontal1(x0 + offsetLeft, y0 + offsetTop, getSizex() - offsetLeft - offsetRight, t0, colorMod);
  this->enableTop = 1;
  
  container.resize(container.getX0(), container.getY0() + (top.getY1() - y0), container.getX1(), container.getY1());
}


void Window::initContainer()
{
  container.make(x0, y0, getSizex(), getSizey());
}

//to let the scrollbars work properly, call this AFTER using setContainerBorders, addTop, addResizer and such of the window
void Window::addScrollbars()
{
  if(scroll.element) return;
  
  scroll.make(container.getX0(), container.getY0(), container.getSizex(), container.getSizey(), &container);
  container.setSizeToElements();
  ic.removeElement(&container);
  addSubElement(&scroll, STICKYFULL);
}

void Window::removeScrollbars()
{
  if(!scroll.element) return;
  
  scroll.element = 0;
  ic.removeElement(&scroll);
  addSubElement(&container, STICKYFULL);
  container.resize(scroll.getX0(), scroll.getY0(), scroll.getX1(), scroll.getY1());
}

//this function could be obsolete once there's the resize function
void Window::setSize(int x, int y, int sizex, int sizey)
{
  x0 = x;
  y0 = y;
  setSizex(sizex);
  setSizey(sizey);
}

void Window::putInside(int i)
{
  container.putInside(i);
}


void Window::handleWidget()
{
  //the close button
  if(closeEnabled && closeButton.clicked()) closed = 1;
  
  if(scroll.element) scroll.handle();
  else container.handle();
  
  //resize window
  if(enableResizer && resizer.mouseGrabbed()) resize(x0, y0, globalMouseX - resizer.mouseGetRelGrabX() + (x1 - resizer.getX0()), globalMouseY - resizer.mouseGetRelGrabY() + (y1 - resizer.getY0()));

  //drag window
  if(!enableTop && mouseGrabbed()) moveTo(globalMouseX - mouseGetRelGrabX(), globalMouseY - mouseGetRelGrabY()); //um this means a window without top can always be dragged?? maybe I should turn this off? in case you want a non moveable window...
  if(enableTop && top.mouseGrabbed())
  {
    moveTo(globalMouseX - top.mouseGetRelGrabX() - (top.getX0() - getX0()), globalMouseY - top.mouseGetRelGrabY() - (top.getY0() - getY0()));
  }
  
  //the scrollbar's conserveCorner should be the same as this window's resizerOverContainer
  scroll.bars.conserveCorner = resizerOverContainer;
  
}

void Window::drawWidget() const
{
  panel.draw(x0, y0, getSizex(), getSizey());
  if(enableTop)
  {
    top.draw(); //draw top bar before the elements, or it'll appear above windows relative to the current window
    print(title, top.getX0() + titleX, top.getY0() + titleY, titleMarkup);
  }
  
  if(closeEnabled) closeButton.draw();

  if(scroll.element) scroll.draw();
  else container.draw();

  if(enableResizer) resizer.draw(); //draw this after the container so the resizer is drawn over scrollbars if that is needed
}

void Window::addTitle(const std::string& title, int titleX, int titleY, const Markup& titleMarkup)
{
  this->title = title;
  this->titleX = titleX;
  this->titleY = titleY;
  this->titleMarkup = titleMarkup;
}

void Window::setTitle(const std::string& title)
{
  this->title = title;
}

unsigned long Window::size() const
{
  return container.size();
}

void Window::remove(Element* element)
{
  container.remove(element);
}

void Window::bringToTop(Element* element) //precondition: element must already be in the list
{
  container.bringToTop(element);
}

void Window::pushTopAt(Element* element, int x, int y, const Pos<Sticky>& sticky)
{
  container.pushTopAt(element, x, y, sticky);
}

void Window::pushBottomAt(Element* element, int x, int y, const Pos<Sticky>& sticky)
{
  container.pushBottomAt(element, x, y, sticky);
}

void Window::insertAt(size_t pos, Element* element, int x, int y, const Pos<Sticky>& sticky)
{
  container.insertAt(pos, element, x, y, sticky);
}

void Window::pushTop(Element* element, const Pos<Sticky>& sticky)
{
  container.pushTop(element, sticky);
}

void Window::pushBottom(Element* element, const Pos<Sticky>& sticky)
{
  container.pushBottom(element, sticky);
}

void Window::insert(size_t pos, Element* element, const Pos<Sticky>& sticky)
{
  container.insert(pos, element, sticky);
}

void Window::pushTopRelative(Element* element, const Pos<Sticky>& sticky)
{
  container.pushTopRelative(element, sticky);
}

void Window::pushBottomRelative(Element* element, const Pos<Sticky>& sticky)
{
  container.pushBottomRelative(element, sticky);
}

void Window::insertRelative(size_t pos, Element* element, const Pos<Sticky>& sticky)
{
  container.insertRelative(pos, element, sticky);
}

////////////////////////////////////////////////////////////////////////////////
//GUIBUTTON/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Button functions.
Button is a class for a button supporting different graphics on mouseover,
and that can return if the mouse is over the button, the mouse is down, or the
mouse was down and is released again.
*/

//Empty Constructor
Button::Button()
{
  this->enableImage = 0;
  this->enableImage2 = 0;
  this->enableText = 0;
  this->enablePanel = 0;

  this->image[0] = 0;
  this->image[1] = 0;
  this->image[2] = 0;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = RGB_Black;
  this->imageColor[1] = RGB_Black;
  this->imageColor[2] = RGB_Black;
  
  this->text = "";
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = TS_Black;
  this->markup[1] = TS_Black;
  this->markup[2] = TS_Black;
  
  /*this->panel1.t00 = &emptyTexture;
  this->panel1.t01 = &emptyTexture;
  this->panel1.t02 = &emptyTexture;
  this->panel1.t10 = &emptyTexture;
  this->panel1.t11 = &emptyTexture;
  this->panel1.t12 = &emptyTexture;
  this->panel1.t20 = &emptyTexture;
  this->panel1.t21 = &emptyTexture;
  this->panel1.t22 = &emptyTexture;
  this->panel2.t00 = &emptyTexture;
  this->panel2.t01 = &emptyTexture;
  this->panel2.t02 = &emptyTexture;
  this->panel2.t10 = &emptyTexture;
  this->panel2.t11 = &emptyTexture;
  this->panel2.t12 = &emptyTexture;
  this->panel2.t20 = &emptyTexture;
  this->panel2.t21 = &emptyTexture;
  this->panel2.t22 = &emptyTexture;  
  this->panelOffsetx = 0;
  this->panelOffsety = 0;
  this->panel1.colorMod = RGB_Black;
  this->panel2.colorMod = RGB_Black;
  this->panel1.enableSides = 0;
  this->panel2.enableSides = 0;
  this->panel1.enableCenter = 0;
  this->panel2.enableCenter = 0;*/
  
  this->active = 0;
  this->visible = 0;
  this->doubleClickTime = 500;
  
  this->mouseDownVisualStyle = 0;
}

//Full Constructor
void Button::make
(
  int x, int y, int sizex, int sizey, //basic properties
  bool enableImage, Texture* texture1, Texture* texture2, Texture* texture3, int imageOffsetx, int imageOffsety, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3, //image
  bool enableText, const std::string& text, int textOffsetx, int textOffsety, const Markup& markup1, const Markup& markup2, const Markup& markup3, //text
  bool enablePanel, const BackPanel* panel1, const BackPanel* panel2, const BackPanel* panel3, int panelOffsetx, int panelOffsety //panel
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  
  this->enableImage = enableImage;
  this->enableText = enableText;
  this->enablePanel = enablePanel;

  this->image[0] = texture1;
  this->image[1] = texture2;
  this->image[2] = texture3;
  this->imageOffsetx = imageOffsetx;
  this->imageOffsety = imageOffsety;
  this->imageColor[0] = imageColor1;
  this->imageColor[1] = imageColor2;
  this->imageColor[2] = imageColor3;
  
  this->text = text;
  this->textOffsetx = textOffsetx;
  this->textOffsety = textOffsety;
  this->markup[0] = markup1;
  this->markup[1] = markup2;
  this->markup[2] = markup3;
  
  this->panel[0] = panel1;
  this->panel[1] = panel2;
  this->panel[2] = panel3;
  this->panelOffsetx = panelOffsetx;
  this->panelOffsety = panelOffsety;
  
  /*if(panelTexture1 != &emptyTexture)
  {
    this->panel1.t00 = &panelTexture1[0]; //+ 0*sizeof(Texture);
    this->panel1.t01 = &panelTexture1[1]; //+ 1*sizeof(Texture);
    this->panel1.t02 = &panelTexture1[2]; //+ 2*sizeof(Texture);
    this->panel1.t10 = &panelTexture1[3]; //+ 3*sizeof(Texture);
    this->panel1.t11 = &panelTexture1[4]; //+ 4*sizeof(Texture);
    this->panel1.t12 = &panelTexture1[5]; //+ 5*sizeof(Texture);
    this->panel1.t20 = &panelTexture1[6]; //+ 6*sizeof(Texture);
    this->panel1.t21 = &panelTexture1[7]; //+ 7*sizeof(Texture);
    this->panel1.t22 = &panelTexture1[8]; //+ 8*sizeof(Texture);
  }
  if(panelTexture2 != &emptyTexture)
  {
    this->panel2.t00 = &panelTexture2[0]; //+ 0*sizeof(Texture);
    this->panel2.t01 = &panelTexture2[1]; //+ 1*sizeof(Texture);
    this->panel2.t02 = &panelTexture2[2]; //+ 2*sizeof(Texture);
    this->panel2.t10 = &panelTexture2[3]; //+ 3*sizeof(Texture);
    this->panel2.t11 = &panelTexture2[4]; //+ 4*sizeof(Texture);
    this->panel2.t12 = &panelTexture2[5]; //+ 5*sizeof(Texture);
    this->panel2.t20 = &panelTexture2[6]; //+ 6*sizeof(Texture);
    this->panel2.t21 = &panelTexture2[7]; //+ 7*sizeof(Texture);
    this->panel2.t22 = &panelTexture2[8]; //+ 8*sizeof(Texture);
  }

  this->panelOffsetx = panelOffsetx;
  this->panelOffsety = panelOffsety;
  this->panel1.colorMod = panelColor1;
  this->panel2.colorMod = panelColor2;
  this->panel1.enableSides = 1;
  this->panel2.enableSides = 1;
  this->panel1.enableCenter = 1;
  this->panel2.enableCenter = 1;*/
  
  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->mouseDownVisualStyle = 0;
}

void Button::addFrontImage
(
  const Texture* texture1, const Texture* texture2, const Texture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3
)
{
  this->enableImage2 = 1;
  
  this->image2[0] = texture1;
  this->image2[1] = texture2;
  this->image2[2] = texture3;
  this->imageOffsetx2 = 0;
  this->imageOffsety2 = 0;
  this->imageColor2[0] = imageColor1;
  this->imageColor2[1] = imageColor2;
  this->imageColor2[2] = imageColor3;
}

void Button::addFrontImage(const Texture* texture)
{
  addFrontImage(texture, texture, texture, imageColor[0], imageColor[1], imageColor[2]);
}

//Constructor to make a button with only images, no text
void Button::makeImage
(
  int x, int y, //basic properties
  const Texture* texture1, const Texture* texture2, const Texture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3 //image
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(texture1->getU());
  this->setSizey(texture1->getV());
  
  this->enableImage = 1;
  this->enableText = 0;
  this->enablePanel = 0;
  
  this->image[0] = texture1;
  this->image[1] = texture2;
  this->image[2] = texture3;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = imageColor1;
  this->imageColor[1] = imageColor2;
  this->imageColor[2] = imageColor3;
  
  this->text = "";
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = TS_Black;
  this->markup[1] = TS_Black;
  this->markup[2] = TS_Black;
  
  /*this->panel1.t00 = &emptyTexture;
  this->panel1.t01 = &emptyTexture;
  this->panel1.t02 = &emptyTexture;
  this->panel1.t10 = &emptyTexture;
  this->panel1.t11 = &emptyTexture;
  this->panel1.t12 = &emptyTexture;
  this->panel1.t20 = &emptyTexture;
  this->panel1.t21 = &emptyTexture;
  this->panel1.t22 = &emptyTexture;
  this->panel2.t00 = &emptyTexture;
  this->panel2.t01 = &emptyTexture;
  this->panel2.t02 = &emptyTexture;
  this->panel2.t10 = &emptyTexture;
  this->panel2.t11 = &emptyTexture;
  this->panel2.t12 = &emptyTexture;
  this->panel2.t20 = &emptyTexture;
  this->panel2.t21 = &emptyTexture;
  this->panel2.t22 = &emptyTexture;  
  this->panelOffsetx = 0;
  this->panelOffsety = 0;
  this->panel1.colorMod = RGB_Black;
  this->panel2.colorMod = RGB_Black;
  this->panel1.enableSides = 0;
  this->panel2.enableSides = 0;
  this->panel1.enableCenter = 0;
  this->panel2.enableCenter = 0;*/
  
  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->mouseDownVisualStyle = 0;
}

//Constructor to make a button with only text, no images
void Button::makeText
(
  int x, int y, //basic properties
  const std::string& text, //text
  const GuiSet* set
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(0);
  this->setSizey(0);
  
  this->enableImage = 0;
  this->enableText = 1;
  this->enablePanel = 0;

  
  this->image[0] = 0;
  this->image[1] = 0;
  this->image[2] = 0;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = RGB_Black;
  this->imageColor[1] = RGB_Black;
  this->imageColor[2] = RGB_Black;
  
  this->text = text;
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = set->textButtonMarkup[0];
  this->markup[1] = set->textButtonMarkup[1];
  this->markup[2] = set->textButtonMarkup[2];
  
  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->mouseDownVisualStyle = 0;
  
  autoTextSize();
}

//constructor for button with panel and text, auto generated text offset
void Button::makeTextPanel
(
  int x, int y, const std::string& text, int sizex, int sizey, //basic properties + text
  const GuiSet* set //panel
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  
  this->enableImage = 0;
  this->enableText = 1;
  this->enablePanel = 1;
  
  this->image[0] = 0;
  this->image[1] = 0;
  this->image[2] = 0;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = RGB_Black;
  this->imageColor[1] = RGB_Black;
  this->imageColor[2] = RGB_Black;
  
  this->text = text;
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = set->panelButtonMarkup[0];
  this->markup[1] = set->panelButtonMarkup[0];
  this->markup[2] = set->panelButtonMarkup[0];
  
  this->panel[0] = set->buttonPanel;
  this->panel[1] = set->buttonOverPanel;
  this->panel[2] = set->buttonDownPanel;
  
  this->panelOffsetx = 0;
  this->panelOffsety = 0;

  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->mouseDownVisualStyle = 1;
  
   
  //put text in the center
  
  centerText();
}

//Draws the button, also checks for mouseover and mousedown to draw mouseover graphic
void Button::drawWidget() const
{
  int i = 0; //0 = normal, 1 = mouseOver, 2 = mouseDown
  
  if((mouseDownVisualStyle == 0 && mouseDown(LMB/*, 15*/))
  || (mouseDownVisualStyle == 1 && mouseDownHere(mutable_button_drawing_mouse_test))
  || (mouseDownVisualStyle == 2 && mouseGrabbed(mutable_button_drawing_mouse_test)))
    i = 2;
  else if(mouseOver()) i = 1;
  
  int textDownOffset = 0;
  if(i == 2) textDownOffset = 1; //on mouseDown, the text goes down a bit too
  
  if(enablePanel) panel[i]->draw(x0 + panelOffsetx, y0 + panelOffsety, getSizex(), getSizey());
  if(enableImage) image[i]->draw(x0 + imageOffsetx, y0 + imageOffsety, imageColor[i]);
  if(enableImage2) image2[i]->draw(x0 + imageOffsetx2, y0 + imageOffsety2, imageColor2[i]);
  if(enableText) printFormatted(text, x0 + textOffsetx + textDownOffset, y0 + textOffsety + textDownOffset, markup[i]);
  
  //NOTE: ik heb de print functies nu veranderd naar printFormatted! Zo kan je gekleurde texten enzo printen met # codes
}

/*This function automaticly changes sizex and sizey of the button so that the
size of the button matches the size of the text, so that it detects the mouse
as being in the button when the mouse is over the text*/
void Button::autoTextSize(int extrasize)
{
  setSizex(text.length() * markup[0].getWidth());
  setSizey(markup[0].getHeight());
  x1 += 2 * extrasize;
  y1 += 2 * extrasize;
  x0 -= extrasize;
  y0 -= extrasize;
  textOffsetx += extrasize;
  textOffsety += extrasize;
}

/*Place the text in the center of the button without having to calculate
these positions by hand*/
void Button::centerText()
{
  textOffsetx = (getSizex() / 2) - text.length() * (markup[0].getWidth() / 2); //4 = fontwidth / 2
  textOffsety = (getSizey() / 2) - (markup[0].getHeight() / 2);
}



void  Button::makeImage(int x, int y,  const Texture* texture123, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3)
{
  makeImage(x, y, texture123, texture123, texture123, imageColor1, imageColor2, imageColor3);
}

////////////////////////////////////////////////////////////////////////////////
//GUISCROLLBAR//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Scrollbar Class Functions
Scrollbar is a scrollbar that can be used for lists of things that are too
large to fit on screen so you have to scroll through them, for example useful
for Console.

It has a length on screen (sizey) and a length of the scroller (scrollSize), in
pixels, and you can scroll up or down with the mouse.

See gui.h for meanings of all the members of ScrollBar
*/

int Scrollbar::getSliderSize() const
{
  if(direction == V)
  {
    return getSizey() - txUp->getV() - txDown->getV() - txScroller->getV(); //scroller length also subtracted from it: the scroller can only move over this distance since it's full width must always be inside the slider length
  }
  else
  {
    return getSizex() - txUp->getU() - txDown->getU() - txScroller->getU();
  }
}

int Scrollbar::getSliderStart() const
{
  if(direction == V)
  {
    return txUp->getV();
  }
  else
  {
    return txUp->getU();
  }
}

int Scrollbar::getSliderEnd() const
{
  if(direction == V)
  {
    return getSizey() - txDown->getV();
  }
  else
  {
    return getSizex() - txDown->getU();
  }
}

void Scrollbar::init()
{
  if(scrollSize == 0) scrollSize = 1;

  if(direction == V)
  {
    buttonUp.makeImage(x0, y0, txUp, txUpOver, txUpOver, colorMod, colorModOver, colorModDown);
    buttonDown.makeImage(x0, y1 - txDown->getV(), txDown, txDownOver, txDownOver, colorMod, colorModOver, colorModDown);
    scroller.makeImage(x0, int(y0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), txScroller, txScrollerOver, txScrollerOver, colorMod, colorModOver, colorModDown);
  }
  else
  {
    buttonUp.makeImage(x0, y0, txUp, txUpOver, txUpOver, colorMod, colorModOver, colorModDown);
    buttonDown.makeImage(x1 - txDown->getU(), y0, txDown, txDownOver, txDownOver, colorMod, colorModOver, colorModDown);
    scroller.makeImage(int(x0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), y0, txScroller, txScrollerOver, txScrollerOver, colorMod, colorModOver, colorModDown);
  }
  
  buttonUp.mouseDownVisualStyle = 1;
  buttonDown.mouseDownVisualStyle = 1;
  scroller.mouseDownVisualStyle = 2;
}

Scrollbar::Scrollbar()
{
  this->scrollSize = 0;
  this->scrollPos = 0;
  this->scrollSpeed = 0;
  this->txUp = &emptyTexture;
  this->txDown = &emptyTexture;
  this->txScroller = &emptyTexture;
  this->txBack = &emptyTexture;
  this->txUpOver = &emptyTexture;
  this->txDownOver = &emptyTexture;
  this->txScrollerOver = &emptyTexture;
  this->colorMod = RGB_White;
  this->colorModOver = RGB_White;
  this->colorModDown = RGB_White;
  this->absoluteSpeed = 0;
  this->speedMode = 0;
  this->enableValue = 0;
  this->forwardedScroll = 0;
  
//TODO: sticky parameters juist zetten
  addSubElement(&scroller);
  addSubElement(&buttonUp);
  addSubElement(&buttonDown);
}

bool Scrollbar::forwardedMouseScrollUp() const
{
  if(forwardedScroll < 0)
  {
    forwardedScroll = 0;
    return true;
  }
  return false;
}

bool Scrollbar::forwardedMouseScrollDown() const
{
  if(forwardedScroll > 0)
  {
    forwardedScroll = 0;
    return true;
  }
  return false;
}

void Scrollbar::forwardScroll(int scroll)
{
  forwardedScroll += scroll;
}

void Scrollbar::showValue(int x, int y, const Markup& valueMarkup, int type)
{
  this->valueMarkup = valueMarkup;
  this->valueX = x;
  this->valueY = y;
  this->enableValue = type;
}

void Scrollbar::makeVertical(int x, int y, int length,
                double scrollSize, double scrollPos, double offset, double scrollSpeed,
                const GuiSet* set, int speedMode)
{
  this->x0 = x;
  this->y0 = y;
  this->direction = V;
  this->setSizex(set->scroller->getU());
  this->setSizey(length);
  this->scrollSize = scrollSize;
  this->scrollPos = scrollPos;
  this->scrollSpeed = scrollSpeed;
  this->offset = offset;
  
  this->txUp = set->arrowN;
  this->txDown = set->arrowS;
  this->txScroller = set->scroller;
  this->txBack = set->scrollbarBackground;
  this->txUpOver = set->arrowN;
  this->txDownOver = set->arrowS;
  this->txScrollerOver = set->scroller;
  this->colorMod = set->mainColor;
  this->colorModOver = set->mouseOverColor;
  this->colorModDown = set->mouseDownColor;
  
  this->absoluteSpeed = scrollSpeed;
  this->speedMode = speedMode;
  
  this->totallyEnable();
  
  init();
  
  if(speedMode == 1) setRelativeScrollSpeed();
}

void Scrollbar::makeHorizontal(int x, int y, int length,
                  double scrollSize, double scrollPos, double offset, double scrollSpeed,
                  const GuiSet* set, int speedMode)
{
  this->x0 = x;
  this->y0 = y;
  this->direction = H;
  this->setSizex(length);
  this->setSizey(set->scroller->getV());
  this->scrollSize = scrollSize;
  this->scrollPos = scrollPos;
  this->scrollSpeed = scrollSpeed;
  this->offset = offset;
  
  this->txUp = set->arrowW;
  this->txDown = set->arrowE;
  this->txScroller = set->scroller;;
  this->txBack = set->scrollbarBackground;
  this->txUpOver = set->arrowW;
  this->txDownOver = set->arrowE;
  this->txScrollerOver = set->scroller;;
  this->colorMod = set->mainColor;
  this->colorModOver = set->mouseOverColor;
  this->colorModDown = set->mouseDownColor;
  
  this->absoluteSpeed = scrollSpeed;
  this->speedMode = speedMode;
  
  this->totallyEnable();
  
  init();
  
  if(speedMode == 1) setRelativeScrollSpeed();
}

double Scrollbar::getValue() const
{
  return offset + scrollPos;
}

void Scrollbar::setValue(double value)
{
  scrollPos = value - offset;
}

void Scrollbar::randomize()
{
  double r = getRandom();
  scrollPos = r * scrollSize;
}

void Scrollbar::handleWidget()
{
  int scrollDir = 0;

  if(buttonUp.mouseDownHere()) scrollDir = -1;
  if(buttonDown.mouseDownHere()) scrollDir = 1;

  if(direction == V)
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed();
    bool buttonDown_grabbed = buttonDown.mouseGrabbed();
    if(scroller.mouseGrabbed())
      scrollPos = (scrollSize * (globalMouseY - y0 - getSliderStart() - txScroller->getV() / 2)) / getSliderSize();
    else if(mouseDownHere() && !scroller.mouseGrabbed() && !buttonUp_grabbed && !buttonDown_grabbed)
    {
      scrollPos = (scrollSize * (globalMouseY - y0 - getSliderStart() - txScroller->getV() / 2)) / getSliderSize();
      scroller.mouseGrab();
    }
    if(mouseScrollUp()) scrollDir = -3;
    if(mouseScrollDown()) scrollDir = 3;
    if(forwardedMouseScrollUp()) scrollDir = -3;
    if(forwardedMouseScrollDown()) scrollDir = 3;
  }
  else
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed();
    bool buttonDown_grabbed = buttonDown.mouseGrabbed();
    if(scroller.mouseGrabbed())
      scrollPos = (scrollSize * (globalMouseX - x0 - getSliderStart() - txScroller->getU() / 2)) / getSliderSize();
    else if(mouseDownHere() && !scroller.mouseGrabbed() && !buttonUp_grabbed && !buttonDown_grabbed)
    {
      scrollPos = (scrollSize * (globalMouseX - x0 - getSliderStart() - txScroller->getU() / 2)) / getSliderSize();
      scroller.mouseGrab();
    }
  }
  
  if(scrollDir != 0)
  {
    scrollPos += scrollDir * absoluteSpeed * ((getSeconds() - oldTime));
  }
  oldTime = getSeconds();

  if(scrollPos < 0) scrollPos = 0;
  if(scrollPos > scrollSize) scrollPos = scrollSize;
  
  if(speedMode == 1) setRelativeScrollSpeed();
  
  if(direction == V) scroller.moveTo(scroller.getX0(), int(y0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize));
  else scroller.moveTo(int(x0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), scroller.getY0());
}

//from an external source, use this function only BEFORE using the handle() function or getTicks() - oldTime will be zero
void Scrollbar::scroll(int dir)
{
  scrollPos += dir * absoluteSpeed * (getTicks() - oldTime) / 1000.0;
}

void Scrollbar::drawWidget() const
{
  //in the drawRepeated functions, sizeButton is divided through two, so if the arrow buttons have a few transparent pixels, in one half of the button you see the background through, in the other half not
  if(direction == V)
  {
    txBack->drawRepeated(x0, y0 + getSliderStart(), x1, y0 + getSliderEnd(), 1.0, 1.0, colorMod);
  }
  else
  {
    txBack->drawRepeated(x0 + getSliderStart(), y0, x0 + getSliderEnd(), y1, 1.0, 1.0, colorMod);
  }
  buttonUp.draw();
  buttonDown.draw();
  if(scrollSize > 0) scroller.draw();
  
  if(enableValue == 1)
  {
    print(getValue(), x0 + valueX, y0 + valueY, valueMarkup);
  }
  else if(enableValue == 2)
  {
    print(int(getValue()), x0 + valueX, y0 + valueY, valueMarkup);
  }
}

void Scrollbar::setRelativePosition(float position)
{
  scrollPos = int(position * getSliderSize());
}

void Scrollbar::setRelativeScrollSpeed()
{
  absoluteSpeed = scrollSize / (scrollSpeed);
}

////////////////////////////////////////////////////////////////////////////////
//GUISCROLLBARPAIR//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
  a horizontal and vertical scrollbar in one, for use for areas that can be scrolled in two directions
*/

ScrollbarPair::ScrollbarPair() : venabled(false), henabled(false)
{
  this->conserveCorner = false;
  scrollbarGuiSet = &builtInGuiSet;
  addSubElement(&vbar, STICKYVERTICALRIGHT);
  addSubElement(&hbar, STICKYHORIZONTALBOTTOM);
}

int ScrollbarPair::getVisiblex() const
{
  if(venabled) return x1 - x0 - vbar.getSizex();
  else return x1 - x0;
}

int ScrollbarPair::getVisibley() const
{
  if(venabled) return y1 - y0 - hbar.getSizey();
  else return y1 - y0;
}

void ScrollbarPair::make(int x, int y, int sizex, int sizey, double scrollSizeH, double scrollSizeV,
                         const GuiSet* set)
{
  scrollbarGuiSet = set;
  this->totallyEnable();
  this->x0 = x;
  this->y0 = y;
  this->x1 = x + sizex;
  this->y1 = y + sizey;
  this->txCorner = set->scrollBarPairCorner;
  
  vbar.makeVertical(x1 - set->arrowN->getU(), y0, sizey - set->arrowE->getV(),
          scrollSizeV, 0, 0, 1,
          set, 1);
  hbar.makeHorizontal(x0, y1 - set->arrowW->getU(), sizex - set->arrowS->getU(),
          scrollSizeH, 0, 0, 1,
          set, 1);
  
  this->venabled = true;
  this->henabled = true;
}

void ScrollbarPair::disableV()
{
  venabled = false;
  
  if(henabled)
  hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex() * conserveCorner,
          hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
          scrollbarGuiSet, hbar.speedMode);
}

void ScrollbarPair::disableH()
{
  henabled = false;
  
  if(venabled)
  vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey() * conserveCorner,
          vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
          scrollbarGuiSet, vbar.speedMode);
}

void ScrollbarPair::enableV()
{
  venabled = true;
  
  if(henabled)
  {
    vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey(),
              vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
              scrollbarGuiSet, vbar.speedMode);
    hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex(),
              hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
              scrollbarGuiSet, hbar.speedMode);
  }
  else
  {
    vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey() * conserveCorner,
              vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
              scrollbarGuiSet, vbar.speedMode);
  }
}

void ScrollbarPair::enableH()
{
  henabled = true;
  
  if(venabled)
  {
    hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex(),
              hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
              scrollbarGuiSet, hbar.speedMode);
    vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey(),
              vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
              scrollbarGuiSet, vbar.speedMode);
  }
  else
  {
    hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex() * conserveCorner,
              hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
              scrollbarGuiSet, hbar.speedMode);
  }
}

void ScrollbarPair::handleWidget()
{
  if(venabled) vbar.handle();
  if(henabled) hbar.handle();
}

void ScrollbarPair::drawWidget() const
{
  if(venabled) vbar.draw();
  if(henabled) hbar.draw();
  if((venabled && henabled && !conserveCorner) || ((venabled || henabled) && conserveCorner)) txCorner->draw(x1 - txCorner->getU(), y1 - txCorner->getV());
}

////////////////////////////////////////////////////////////////////////////////
//GUISLIDER/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
A simplified version of the scrollbar
*/

Slider::Slider()
{
  this->visible = 0;
  this->active = 0;
  addSubElement(&slider);
}

void Slider::makeHorizontal(int x, int y, int length, double scrollSize, const GuiSet* set)
{
  this->totallyEnable();

  int sliderCenter = set->slider->getV() / 2;
  int rulerCenter = set->sliderHRule->t0->getV() / 2;
  int centerPos = sliderCenter;
  if(rulerCenter > sliderCenter) centerPos = rulerCenter;
  
  this->direction = H;
  this->scrollPos = 0;
  this->scrollSize = scrollSize;
  this->x0 = x;
  this->y0 = y;
  this->setSizex(length);
  this->setSizey(set->slider->getV());
  this->ruler = set->sliderHRule;
  this->slider.makeImage(x0, y0 + centerPos - sliderCenter, set->slider, set->slider, set->slider, set->mainColor, set->mouseOverColor, set->mouseDownColor);
//TODO: sticky parameters goed zetten
  slider.mouseDownVisualStyle = 2;
}

void Slider::makeVertical(int x, int y, int length, double scrollSize, const GuiSet* set)
{
  this->totallyEnable();
  
  int sliderCenter = set->slider->getU() / 2;
  int rulerCenter = set->sliderVRule->t0->getU() / 2;
  int centerPos = sliderCenter;
  if(rulerCenter > sliderCenter) centerPos = rulerCenter;
  
  this->direction = V;
  this->scrollPos = 0;
  this->scrollSize = scrollSize;
  this->x0 = x;
  this->y0 = y;
  this->setSizey(length);
  this->setSizex(set->slider->getU());
  this->ruler = set->sliderVRule;
  this->slider.makeImage(x + centerPos - sliderCenter, y, set->slider, set->slider, set->slider, set->mainColor, set->mouseOverColor, set->mouseDownColor);
  
//TODO: sticky parameters goed zetten

  slider.mouseDownVisualStyle = 2;
}

//screenPos must be relative to the x or y position of the gui element!!!
double Slider::screenPosToScrollPos(int screenPos)
{
  if(direction == H)
  {
    int sliderSize = slider.image[0]->getU();
    return (scrollSize * (screenPos - sliderSize / 2)) / (getSizex() - sliderSize);
  }
  else //if(direction == V)
  {
    int sliderSize = slider.image[0]->getV();
    return (scrollSize * (screenPos - sliderSize / 2)) / (getSizey() - sliderSize);
  }
}

//screenPos is relative to the x or y position of the gui element!!!
int Slider::scrollPosToScreenPos(double scrollPos)
{
  if(direction == H)
  {
    int sliderSize = slider.image[0]->getU();
    return int(((getSizex() - sliderSize) * scrollPos) / scrollSize);
  }
  else //if(direction == V)
  {
    int sliderSize = slider.image[0]->getV();
    return int(((getSizey() - sliderSize) * scrollPos) / scrollSize);
  }
}

void Slider::drawWidget() const
{
  if(direction == H)
  {
    int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCentery();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    
    ruler->draw(x0, y0 + centerPos - rulerCenter, getSizex());
    slider.draw();
  }
  else //if(direction == V)
  {
    int rulerCenter = ruler->t0->getU() / 2;
    int centerPos = slider.getRelCenterx();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    
    ruler->draw(x0 + centerPos - rulerCenter, y0, getSizey());
    slider.draw();
  }
}

void Slider::handleWidget()
{
  if(direction == H)
  {
    int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCentery();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    //int sliderSize = slider.texture1->getU();
    
    if(slider.mouseGrabbed()) scrollPos = screenPosToScrollPos(mouseGetRelPosX());
    else if(mouseDownHere() && !slider.mouseOver())
    {
      scrollPos = screenPosToScrollPos(mouseGetRelPosX());
      slider.mouseGrab();
    }
    
    if(scrollPos < 0) scrollPos = 0;
    if(scrollPos > scrollSize) scrollPos = scrollSize;
    
    slider.moveTo(x0 + scrollPosToScreenPos(scrollPos), slider.getY0());
  }
  else //if(direction == V)
  {
    int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCenterx();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    //int sliderSize = slider.texture1->getV();
    
    if(slider.mouseGrabbed()) scrollPos = screenPosToScrollPos(mouseGetRelPosY());
    else if(mouseDownHere() && !slider.mouseOver())
    {
      scrollPos = screenPosToScrollPos(mouseGetRelPosY());
      slider.mouseGrab();
    }
    
    if(scrollPos < 0) scrollPos = 0;
    if(scrollPos > scrollSize) scrollPos = scrollSize;
    
    slider.moveTo(slider.getX0(), y0 + scrollPosToScreenPos(scrollPos));
  }
}

double Slider::getValue() const
{
  return scrollPos;
}

void Slider::setValue(double value)
{
  scrollPos = value;
}

double Slider::getRelValue() const
{
  return scrollPos / scrollSize;
}

void Slider::setRelValue(double value)
{
  scrollPos = scrollSize * value;
}

////////////////////////////////////////////////////////////////////////////////
//          ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Invisible::make(int x0, int y0, int x1, int y1)
{
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
  this->active = true;
  this->visible = true;
}

void Invisible::drawWidget() const
{
  //intentionally do nothing, because this is invisible
}

////////////////////////////////////////////////////////////////////////////////
//GUICHECKBOX///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Checkbox Class Functions
CheckBox is a simple box that can have a different texture if "on" or "off",
and toggles if you press or release the mouse button on it.

How to use:
  Give it two textures, texture1 one that looks like an unchecked checkbox,
  texture two like a checked checkbox.
  Optionally, give it a title that'll be drawn next to the textbox
  Choose if you want it to toggle on mousedown, or on mouseup with the
  toggleOnMouseUp boolean.
*/

Checkbox::Checkbox()
{
  this->enableImage2 = 0;
  //bad old code, must be fixed!
  this->active = 0;
  this->checked = 0;
  this->texture[0] = &emptyTexture;
  this->texture[1] = &emptyTexture;
  this->texture[2] = &emptyTexture;
  this->texture[3] = &emptyTexture;
  this->colorMod[0] = RGB_White;
  this->colorMod[1] = RGB_White;
  this->colorMod[2] = RGB_White;
  this->colorMod[3] = RGB_White;
  this->toggleOnMouseUp = 0;
  this->enableText = 0;
}

void Checkbox::addFrontImage
(
  const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, 
  const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3, const ColorRGB& imageColor4
)
{
  this->enableImage2 = 1;
  
  this->texture2[0] = texture1;
  this->texture2[1] = texture2;
  this->texture2[2] = texture3;
  this->texture2[3] = texture4;
  this->colorMod2[0] = imageColor1;
  this->colorMod2[1] = imageColor2;
  this->colorMod2[2] = imageColor3;
  this->colorMod2[3] = imageColor4;
}

void Checkbox::addFrontImage(const Texture* texture)
{
  addFrontImage(texture, texture, texture, texture, colorMod[0], colorMod[1], colorMod[2], colorMod[3]);
}
//constructor for checkbox with text title
void Checkbox::make(int x, int y, bool checked, const GuiSet* set, int toggleOnMouseUp)
{
  this->x0 = x;
  this->y0 = y;
  this->checked = checked;
  this->texture[0] = set->checkBox[0];
  this->texture[1] = set->checkBox[0];
  this->texture[2] = set->checkBox[1];
  this->texture[3] = set->checkBox[1];
  this->colorMod[0] = set->mainColor;
  this->colorMod[1] = set->mouseOverColor;
  this->colorMod[2] = set->mainColor;
  this->colorMod[3] = set->mouseOverColor;
  this->setSizex(set->checkBox[0]->getU());
  this->setSizey(set->checkBox[0]->getV());
  this->toggleOnMouseUp = toggleOnMouseUp;
  this->totallyEnable();
  
  positionText();
}

void Checkbox::setTexturesAndColors(const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4)
{
  this->texture[0] = texture1;
  this->texture[1] = texture2;
  this->texture[2] = texture3;
  this->texture[3] = texture4;
  this->colorMod[0] = color1;
  this->colorMod[1] = color2;
  this->colorMod[2] = color3;
  this->colorMod[3] = color4;
  this->setSizex(texture[0]->getU());
  this->setSizey(texture[0]->getV());
  
  positionText();
}

void Checkbox::setTexturesAndColors(const Texture* texture1, const Texture* texture2, const ColorRGB& color1, const ColorRGB& color2)
{
  setTexturesAndColors(texture1, texture1, texture2, texture2, color1, color1, color2, color2);
}

void Checkbox::addText(const std::string& text, const Markup& markup)
{
  this->text = text;
  this->markup = markup;
  this->enableText = 1;
  positionText();
}

//place the text next to the checkbox
void Checkbox::positionText()
{
  textOffsetX = getSizex() + 2; //need some number of pixels that text is away from the texture, eg 2
  textOffsetY = getSizey() / 2 - markup.getHeight() / 2;
}

//toggle "checked" or "unchecked" state
void Checkbox::toggle()
{
  if(checked) uncheck();
  else check();
}

//see how you click with the mouse and toggle on click
void Checkbox::handleWidget()
{
  //make sure never both pressed() and clicked() are checked, because one test screws up the other, hence the order of the tests in the if conditions
  if(toggleOnMouseUp == 0 && pressed()) toggle();
  if(toggleOnMouseUp == 1 && clicked()) toggle();
}

//draw the checkbox with a texture depending on it's state
void Checkbox::drawWidget() const
{
  /*if(checked) texture2->draw(x0, y0, colorMod2);
  else texture1->draw(x0, y0, colorMod1);*/
  int i = 0;
  if(mouseOver()) i++;
  if(checked) i += 2;
  
  texture[i]->draw(x0, y0, colorMod[i]);
  
  if(enableImage2)
    texture2[i]->draw(x0, y0, colorMod2[i]);
  
  if(enableText)
  print(text, x0 + textOffsetX, y0 + textOffsetY, markup);
}

////////////////////////////////////////////////////////////////////////////////
//GUINSTATESTATE////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
This class is used by NState
*/

NStateState::NStateState()
{
  this->texture = &emptyTexture;
  this->colorMod = RGB_White;
  this->enableText = 0;
  this->text = "";
}

//constructor for checkbox with text title
void NStateState::make(Texture* texture, const ColorRGB& colorMod, const std::string& text, const Markup& markup)
{
  this->texture = texture;
  this->colorMod = colorMod;
  this->text = text;
  this->markup = markup;
  if(text != "") enableText = 1;
  
  positionText();
}



//place the text next to the checkbox
void NStateState::positionText()
{
  textOffsetX = texture->getU() + 2; //need some number of pixels that text is away from the texture, eg 2
  textOffsetY = texture->getV() / 2 - markup.getHeight() / 2;
}

////////////////////////////////////////////////////////////////////////////////
//GUINSTATE/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
This is a sort of advanced checkbox, it can go through multiple states and you can add more states.
*/

NState::NState()
{
  //bad old code, must be fixed!
  this->active = 0;
  this->states.clear();
  this->toggleOnMouseUp = 0;
}

//constructor for checkbox with text title
void NState::make(int x, int y, int toggleOnMouseUp)
{
  states.clear();
  this->x0 = x;
  this->y0 = y;
  this->toggleOnMouseUp = toggleOnMouseUp;
  this->totallyEnable();
  this->setSizex(0); //no states yet, size 0
  this->setSizey(0);
}

void NState::addState(Texture* texture, const ColorRGB& colorMod, const std::string& text, const Markup& markup)
{
  this->setSizex(texture->getU()); //set the size of the NState to that of the last added texture
  this->setSizey(texture->getV());
  NStateState s;
  s.make(texture, colorMod, text, markup);
  states.push_back(s);
}


//see how you click with the mouse and toggle on click
void NState::handleWidget()
{
  //make sure never both pressed() and clicked() are checked, because one test screws up the other, hence the order of the tests in the if conditions
  if(states.size() == 0) return;
  
  if((toggleOnMouseUp == 0 && pressed()) || (toggleOnMouseUp == 1 && clicked()))
  {
    if(state >= states.size() - 1) state = 0;
    else state++;
  }
  if((toggleOnMouseUp == 0 && pressed(RMB)) || (toggleOnMouseUp == 1 && clicked(RMB)))
  {
    if(state == 0) state = states.size() - 1;
    else state--;
  }
  
}

//draw the checkbox with a texture depending on it's state
void NState::drawWidget() const
{
  if(states.size() == 0) return;
  
  NStateState s = states[state];
  s.texture->draw(x0, y0, s.colorMod);
  
  if(s.enableText) print(s.text, x0 + s.textOffsetX, y0 + s.textOffsetY, s.markup);
}

////////////////////////////////////////////////////////////////////////////////
//GUIBULLETLIST/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The BulletList

it's a list of checkboxes, where only one can be selected at the time

you first make() the prototype, and make() the bulletlist. Then all checkboxes have the
style you  want!
*/

BulletList::BulletList()
{
  this->active = 0;
  this->visible = 0;
  this->lastChecked = -1;
  
  this->prototype.make(0, 0, 0, &builtInGuiSet, 1);
  this->prototype.setTexturesAndColors(builtInGuiSet.bullet[0], builtInGuiSet.bullet[1], builtInGuiSet.mainColor, builtInGuiSet.mainColor);
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff, const GuiSet* set)
{
  this->prototype.make(0, 0, 0, set, 1);
  this->prototype.setTexturesAndColors(set->bullet[0], set->bullet[1], set->mainColor, set->mainColor);
  
  this->x0 = x;
  this->y0 = y;
  this->xDiff = xDiff;
  this->yDiff = yDiff;
  this->totallyEnable();

  bullet.clear();
  for(unsigned long i = 0; i < amount; i++)
  {
    bullet.push_back(prototype);
    bullet[i].moveTo(x + xDiff * i, y + yDiff * i);
    addSubElement(&bullet[i]); //TODO: fix this big memory corruption problem and Element copying
  }
  
  //NOTE: DIT IS NIET CORRECT, DEZE FORMULES
  this->setSizex(amount * xDiff + prototype.getSizex());
  this->setSizey(amount * yDiff + prototype.getSizey());
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff, unsigned long amountx, const GuiSet* set)
{
  if(amountx < 1) amountx = 1;
  this->prototype.make(0, 0, 0, set, 1);
  this->prototype.setTexturesAndColors(set->bullet[0], set->bullet[1], set->mainColor, set->mainColor);
  
  this->x0 = x;
  this->y0 = y;
  this->xDiff = xDiff;
  this->yDiff = yDiff;
  this->totallyEnable();

  bullet.clear();
  int xPos = x, yPos = y;
  for(unsigned long i = 0; i < amount; i++)
  {
    xPos = x + xDiff * (i % amountx);
    yPos = y + yDiff * (i / amountx);
    bullet.push_back(prototype);
    bullet[i].moveTo(xPos, yPos);
  }
  
  setCorrectSize();
}

void BulletList::setCorrectSize()
{
  if(bullet.size() == 0) return;
  
  int minx = bullet[0].getX0();
  int miny = bullet[0].getY0();
  int maxx = bullet[0].getX1();
  int maxy = bullet[0].getY1();
  
  for(size_t i = 1; i < bullet.size(); i++)
  {
    minx = std::min(minx, bullet[i].getX0());
    miny = std::min(miny, bullet[i].getY0());
    maxx = std::max(maxx, bullet[i].getX1());
    maxy = std::max(maxy, bullet[i].getY1());
  }
  
  this->x0 = minx;
  this->y0 = miny;
  this->setSizex(maxx - minx);
  this->setSizey(maxy - miny);
}

void BulletList::handleWidget()
{
  if(!mouseOver()) return; //some speed gain, don't do all those loops if the mouse isn't over this widget
  
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i].handle();
  }

  int newChecked = -1;
  
  for(size_t i = 0; i < bullet.size(); i++)
  {
    if(bullet[i].checked && int(i) != lastChecked) newChecked = i;
    bullet[i].checked = false;
  }

  if(newChecked >= 0)
  {
    bullet[newChecked].checked = true;
    lastChecked = newChecked;
  }
  else if(lastChecked >= 0 && lastChecked < int(bullet.size()))
    bullet[lastChecked].checked = true;
}

void BulletList::drawWidget() const
{
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i].draw();
  }
}

int BulletList::check()
{
  return lastChecked;
}

void BulletList::addText(const std::string& text, unsigned long i)
{
  if(i < bullet.size()) bullet[i].addText(text);
}

std::string BulletList::getText(unsigned long i) const
{
  std::string result;
  if(i < bullet.size()) result = bullet[i].getText();
  return result;
}

const std::string& BulletList::getCurrentText() const
{
  return bullet[lastChecked].getText();
}

void BulletList::set(unsigned long i)
{
  /*if(i >= 0 && i < bullet.size()) bullet[i].checked = true;
  if(!active) lastChecked = i;*/

  for(unsigned long j = 0; j < bullet.size(); j++)
  {
    if(i != j) bullet[j].checked = false;
    else bullet[j].checked = true;
  }
  
  lastChecked = i;
  
  handle();
}

////////////////////////////////////////////////////////////////////////////////
//GUITEXT///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Text is just some text as child of Element
*/

Text::Text()
{
  this->visible = 0;
  this->active = 0;
  this->useNewLine = true;
}

void Text::make(int x, int y, const std::string& text, const Markup& markup)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(text.length() * markup.getWidth());
  this->setSizey(markup.getHeight());
  this->text = text;
  this->markup = markup;
  this->totallyEnable();
}

void Text::drawWidget() const
{
  print(text, x0, y0, markup, useNewLine);
}

void Text::setText(const std::string& text)
{
  this->text = text;
}

////////////////////////////////////////////////////////////////////////////////
//GUIIMAGE//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Image is a simple texture but as child class of Element
*/

Image::Image()
{
  this->visible = 0;
  this->active = 0;
  this->image = &emptyTexture;
}

void Image::make(int x, int y, Texture* image, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(image->getU());
  this->setSizey(image->getV());
  this->image = image;
  this->colorMod = colorMod;
  this->totallyEnable();
}

void Image::make(int x, int y, int sizex, int sizey, Texture* image, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->image = image;
  this->colorMod = colorMod;
  this->totallyEnable();
}

void Image::drawWidget() const
{
  image->draw(x0, y0, colorMod, getSizex(), getSizey());
}

////////////////////////////////////////////////////////////////////////////////
//OTHER FUNCTIONS///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//#define ENABLE_UNIT_TEST
#ifdef ENABLE_UNIT_TEST

#include "lpi_unittest.h"

#define LUT_MY_RESET \
{\
  debugSetLMB(0);\
  debugSetRMB(0);\
  debugSetMousePos(0, 0);\
}
  
void unitTest()
{
  screen(1024, 768, 0, "Unit Testing...");
  
  LUT_START_UNIT_TEST
  
  //I had a bug where when making a new BulletList and using "set" to give it a value, "check" wasn't returning the correct value until mouseOver
  LUT_CASE("set bulletlist")
    BulletList test1;
    test1.make(0, 0, 5, 0, 0);
    test1.set(3);
    LUT_SUB_ASSERT_FALSE(test1.check() != 3)
    test1.set(1);
    LUT_SUB_ASSERT_FALSE(test1.check() != 1)
  LUT_CASE_END
  
  LUT_CASE("mouseOver and mouseDown on Dummy")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseOver())
    debugSetMousePos(1, 1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseDown())
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown())
  LUT_CASE_END
  
  LUT_CASE("mouseJustDown")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    debugSetMousePos(20, 20);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown()) //the mouse moved over the element, the mouse button was already down since being outside the element, so it's now down over the element for the first time
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown()) //now it may not return true anymore!
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown()) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetMousePos(20, 20);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDownHere())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere()) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere on top of window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    w.addTop();
    c.handle();c.handle();
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetMousePos(mx, my);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetLMB(0);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseJustDownHere())
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere()) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseGrabbed")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse not down yet
    debugSetMousePos(10, 10);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse down on element so it's grabbed
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 10 && dummy.mouseGetGrabY() == 10)
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse down somewhere else not on element ==> not grabbed
    debugSetMousePos(15, 15);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed
    /*//now repeat the test to see if states can properly be reused
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse down on element so it's grabbed
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 15 && dummy.mouseGetGrabY() == 15)
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse down somewhere else not on element ==> not grabbed
    debugSetMousePos(11, 11);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed*/
  LUT_CASE_END
  
  //the fact that the container and the window handles the elements, influences the result
  LUT_CASE("mouseGrabbed on top of window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    w.addTop();
    c.handle();c.handle();
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
 
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed()) //mouse not down yet
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed()) //mouse down on element so it's grabbed
    debugSetMousePos(200, 200);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed()) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(w.top.mouseGetGrabX() == mx && w.top.mouseGetGrabY() == my)
    debugSetLMB(0);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed()) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed()) //mouse down somewhere else not on element ==> normally not grabbed, BUT, the window moves!! ==> under mouse and grabbed again
  LUT_CASE_END
  
  LUT_CASE("dragging a window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    w.addTop();
    c.handle();c.handle();
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    
    int wx1 = w.getX0();
    int wy1 = w.getY0();
    
    debugSetMousePos(mx, my); //mouse above top bar of the window
    debugSetLMB(1);
    c.handle();c.handle();
    
    int wx2 = w.getX0();
    int wy2 = w.getY0();
    
    LUT_SUB_ASSERT_TRUE(wx1 == wx2 && wy1 == wy2)
    
    debugSetMousePos(mx + 100, my + 50);
    c.handle();c.handle();
    int wx3 = w.getX0();
    int wy3 = w.getY0();
    
    LUT_SUB_ASSERT_FALSE(wx3 == wx1 && wy3 == wy1)
    LUT_SUB_ASSERT_TRUE(wx3 == wx1 + 100 && wy3 == wy1 + 50)
    LUT_APPEND_MSG << "wx1: " << wx1 << "wy1: " << wy1 << "wx2: " << wx2 << "wy2: " << wy2 << "wx3: " << wx3 << "wy3: " << wy3;
  LUT_CASE_END
  
  //functions like mouseJustDown and mouseJustDownHere may not influence each other's result, so both can return true once
  LUT_CASE("mouse independence")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    debugSetMousePos(10, 10); //mouse above
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetLMB(1); //press button
    //now both must return true
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDownHere())
    //but of course now both must return false
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
  LUT_CASE_END
  
  //when boolean mouseActive is true, mouseOver should work, when it's false, mouseOver shout NOT work
  LUT_CASE("mouseOver when active and not active")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    debugSetMousePos(1, 1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver())
    dummy.setElementOver(true);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseOver())
    //note: containers like a gui::Window have to take control of the mouseActive boolean of the elements in it
  LUT_CASE_END
    
  
  //the selfActivate system (used by text input boxes) didn't work anymore, so I added this test to make sure it won't break anymore
  LUT_CASE("selfActivate of InputLine")
    InputLine line;
    line.make(0, 0, 10);
    
    //initially it's not active
    line.handle(); //handle line all the time to let the selfActivate system work
    LUT_SUB_ASSERT_TRUE(!line.isActive())
    
    //move mouse above it and click
    debugSetMousePos(1, 1);
    debugSetLMB(1);
    line.handle();
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //now release the mouse button
    debugSetLMB(0);
    line.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away
    debugSetMousePos(100, 100);
    line.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away from it and click
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    line.handle();
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isActive())
  LUT_CASE_END
  LUT_CASE("selfActivate of InputLine in Window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    InputLine line;
    line.make(0, 0, 10);
    w.pushTop(&line, 0, 0);
    
    //initially it's not active
    c.handle(); //handle line all the time to let the selfActivate system work
    LUT_SUB_ASSERT_TRUE(!line.isActive())
    
    //now a little test to see if it's at the correct position for these tests below
    debugSetMousePos(line.getCenterx(), line.getCentery());
    line.setActive(true);
    line.setElementOver(false);
    LUT_SUB_ASSERT_TRUE(line.mouseOver())
    line.setElementOver(true);
    line.setActive(false);
    
    //move mouse above it and click
    debugSetMousePos(line.getCenterx(), line.getCentery());
    debugSetLMB(1);
    c.handle();
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //now release the mouse button
    debugSetLMB(0);
    c.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away
    debugSetMousePos(100, 100);
    c.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away from it and click
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    c.handle();
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isActive())
  LUT_CASE_END
  
  //two things on a container, both at the same location, first the one is totallyEnabled and the other totallyDisabled, then vica versa, then each time only the enabled one may return true to mouseOver
  LUT_CASE("enabled and disabled on each other")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    Dummy dummy1;
    dummy1.resize(0, 0, 50, 50);
    w.pushTop(&dummy1, 0, 0);
    
    Dummy dummy2;
    dummy2.resize(0, 0, 50, 50);
    w.pushTop(&dummy2, 0, 0);
    
    debugSetMousePos(dummy1.getCenterx(), dummy1.getCentery()); //doesn't matter if you take center of dummy1 or dummy2, both are exactly at same position
    
    dummy1.totallyEnable();
    dummy2.totallyDisable();
    
    LUT_SUB_ASSERT_TRUE(dummy1.mouseOver())
    LUT_SUB_ASSERT_TRUE(!dummy2.mouseOver())
    
    dummy1.totallyDisable();
    dummy2.totallyEnable();
    
    LUT_SUB_ASSERT_TRUE(!dummy1.mouseOver())
    LUT_SUB_ASSERT_TRUE(dummy2.mouseOver())
  LUT_CASE_END
  
  //dragging a window that has a top
  LUT_CASE("dragging of a window")
  
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop();
    c.pushTop(&w, 0, 0);
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    
    //grab the window
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    
    c.handle();
    
    //move
    debugSetMousePos(200, 200);
    
    c.handle();
    
    //now the window must still be below the mouse
    LUT_SUB_ASSERT_TRUE(w.mouseOver())  
  LUT_CASE_END
  
  //when there are two windows, A and B, and you're dragging one over the other, but you drag the mouse fast and the mousepointer goes to a location not over the dragged window (the dragged window will be under the mouse again soon), then the other window may not swap to top
  LUT_CASE("fast dragging of one window over another")
  
    Container c;
    Window A;
    A.make(0, 0, 100, 100);
    A.addTop();
    c.pushTopAt(&A, 0, 0);
    
    Window B;
    B.make(0, 0, 100, 100);
    B.addTop();
    c.pushTopAt(&B, 200, 0);
    
    int ax = A.top.getCenterx();
    int ay = A.top.getCentery();
    int bx = B.top.getCenterx();
    int by = B.top.getCentery();
    
    //grab the window A
    debugSetMousePos(ax, ay);
    debugSetLMB(1);
    
    c.handle();
    
    //move it over window B
    debugSetMousePos(bx, by);
    
    c.handle();
    
    //now window A must be over window B, so window A must have mouseOver, window B not
    
    LUT_SUB_ASSERT_TRUE(A.mouseOver())
    LUT_SUB_ASSERT_TRUE(!B.mouseOver())
  
  LUT_CASE_END
  
  //dragging a window that has a top that is shifted: when starting to drag, the window may not "jump" (but I once had)
  LUT_CASE("dragging of a window with shifted top")
  
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop(&lpi::gui::builtInTexture[47], 2, 2, 2); //could cause jump of two pixels when the bug was there
    c.pushTop(&w, 0, 0);
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    
    //grab the window
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    
    c.handle();
    
    LUT_SUB_ASSERT_TRUE(w.getX0() == 0 && w.getY0() == 0); //NOT shifted over two pixels
  LUT_CASE_END

  LUT_END_UNIT_TEST
}
#else
void unitTest()
{
  std::cout<<"gui unit test disabled at compile time. #define ENABLE_UNIT_TEST in " << __FILE__ << std::endl;
}
#endif //ENABLE_UNIT_TEST

} //namespace gui
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////

