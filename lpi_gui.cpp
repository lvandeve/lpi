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


#include "lpi_gui.h"
#include "lpi_math.h"

#include <iostream>

namespace lpi
{
namespace gui
{

AInternalContainer::AInternalContainer()
: elementOver(false)
{
}

void AInternalContainer::move(int x, int y)
{
  if(x == 0 && y == 0) return;
  for(size_t i = 0; i < elements.size(); i++)
  {
    elements[i]->move(x, y);
  }
}

void AInternalContainer::setElementOver(bool state)
{
  if(state == elementOver) return; //this optimization is done because setElementOver gets called so much, it pops up on top of a profiler even when not even moving the mouse in the program
  elementOver = state;
  for(size_t i = 0; i < elements.size(); i++)
  {
    elements[i]->setElementOver(state);
  }
}

size_t AInternalContainer::findElementIndex(Element* element)
{
  for(size_t i = 0; i < elements.size(); i++) if(elements[i] == element) return i;
  return elements.size();
}

const Element* AInternalContainer::hitTest(const IInput& input) const
{
  for(size_t j = 0; j < elements.size(); j++)
  {
    size_t i = elements.size() - j - 1; //invert order, the last elements are on top
    if(elements[i]->mouseOver(input)) return elements[i]->hitTest(input);
  }
  return 0;
}

void AInternalContainer::manageHover(IHoverManager& hover)
{
  for(size_t j = 0; j < elements.size(); j++)
  {
    elements[j]->manageHover(hover);
  }
}

int AInternalContainer::getKeyboardFocus() const
{
  int result = 0;
  
  for(size_t j = 0; j < elements.size(); j++)
  {
    result |= elements[j]->getKeyboardFocus();
  }
  
  return result;
}

////////////////////////////////////////////////////////////////////////////////

void InternalContainer::setStickyElementSize(Element* element, const Pos<int>& newPos)
{
  Sticky& s = sticky[element];
  
  if(s.sticky)
  {
    int w = newPos.x1 - newPos.x0;
    int h = newPos.y1 - newPos.y0;
    
    Pos<int> newPos2;
    
    if(!s.follow.x0) newPos2.x0 = newPos.x0 + (int)(s.d.x0 * w + s.i.x0);
    else newPos2.x0 = element->getX0();
    if(!s.follow.y0) newPos2.y0 = newPos.y0 + (int)(s.d.y0 * h + s.i.y0);
    else newPos2.y0 = element->getY0();
    if(!s.follow.x1) newPos2.x1 = newPos.x0 + (int)(s.d.x1 * w + s.i.x1);
    else newPos2.x1 = element->getX1();
    if(!s.follow.y1) newPos2.y1 = newPos.y0 + (int)(s.d.y1 * h + s.i.y1);
    else newPos2.y1 = element->getY1();
    
    element->resize(newPos2.x0, newPos2.y0, newPos2.x1, newPos2.y1);
  }
}

void InternalContainer::setStickyElementSize(Element* element, Element* parent)
{
  Pos<int> newPos = { parent->getX0(), parent->getY0(), parent->getX1(), parent->getY1() };
  setStickyElementSize(element, newPos);
}

void InternalContainer::resize(const Pos<int>& oldPos, const Pos<int>& newPos)  //this function is where the "sticky"-related calculations happen
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
    Sticky& s = sticky[&e];
    
    if(s.sticky)
    {
      Pos<int> newPos2;
      
      if(!s.follow.x0) newPos2.x0 = newPos.x0 + (int)(s.d.x0 * w + s.i.x0);
      else newPos2.x0 = e.getX0() + (int)(dx0 * (1.0 - s.d.x0) + dx1 * (s.d.x0));
      if(!s.follow.y0) newPos2.y0 = newPos.y0 + (int)(s.d.y0 * h + s.i.y0);
      else newPos2.y0 = e.getY0() + (int)(dy0 * (1.0 - s.d.y0) + dy1 * (s.d.y0));
      if(!s.follow.x1) newPos2.x1 = newPos.x0 + (int)(s.d.x1 * w + s.i.x1);
      else newPos2.x1 = e.getX1() + (int)(dx0 * (1.0 - s.d.x1) + dx1 * (s.d.x1));
      if(!s.follow.y1) newPos2.y1 = newPos.y0 + (int)(s.d.y1 * h + s.i.y1);
      else newPos2.y1 = e.getY1() + (int)(dy0 * (1.0 - s.d.y1) + dy1 * (s.d.y1));
      
      e.resize(newPos2.x0, newPos2.y0, newPos2.x1, newPos2.y1);
    }
  }
}


Sticky InternalContainer::getSticky(Element* element) const
{
  std::map<Element*, Sticky>::const_iterator it = sticky.find(element);
  if(it != sticky.end()) return it->second;
  else return Sticky();
}

void InternalContainer::setSticky(Element* element, const Sticky& sticky, Element* parent)
{
  this->sticky[element] = sticky;
  setStickyElementSize(element, parent);
}

void InternalContainer::setStickyRelative(Element* element, Element* parent)
{
  Sticky sticky((double)(element->getX0() - parent->getX0()) / (double)(parent->getSizeX()), 0
              , (double)(element->getY0() - parent->getY0()) / (double)(parent->getSizeY()), 0
              , (double)(element->getX1() - parent->getX0()) / (double)(parent->getSizeX()), 0
              , (double)(element->getY1() - parent->getY0()) / (double)(parent->getSizeY()), 0);
  setSticky(element, sticky, parent);
}

void InternalContainer::setStickyFull(Element* element, Element* parent)
{
  Sticky sticky(0.0, element->getX0() - parent->getX0()
              , 0.0, element->getY0() - parent->getY0()
              , 1.0, element->getX1() - parent->getX1()
              , 1.0, element->getY1() - parent->getY1());
  setSticky(element, sticky, parent);
}

void InternalContainer::initSubElement(Element* element, const Sticky& sticky, Element* parent)
{
  this->sticky[element] = sticky;
  setStickyElementSize(element, parent);
}

void InternalContainer::addSubElement(Element* element, const Sticky& sticky, Element* parent)
{
  elements.push_back(element);
  initSubElement(element, sticky, parent);
}

void InternalContainer::insertSubElement(size_t index, Element* element, const Sticky& sticky, Element* parent)
{
  initSubElement(element, sticky, parent);
  elements.insert(elements.begin() + index, element);
}

void InternalContainer::removeElement(Element* element)
{
  elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
  if(sticky.find(element) != sticky.end()) sticky.erase(sticky.find(element));
}

void InternalContainer::clearSubElements()
{
  elements.clear();
  sticky.clear();
}

////////////////////////////////////////////////////////////////////////////////

void InternalContainerWrapping::resize(const Pos<int>& oldPos, const Pos<int>& newPos)  //this function is where the "sticky"-related calculations happen
{
  (void)oldPos;
  int x0 = newPos.x0;
  int y0 = newPos.y0;
  int x1 = newPos.x1;
  //int y1 = newPos.y1;
  int dx = x1 - x0;
  //int dy = y1 - y0;

  int y = y0;
  int ny = y0; //next y

  int x = x0;

  int num = 0; //num of this row so far
  
  for(size_t i = 0; i < elements.size(); i++)
  {
    Element& e = *elements[i];
    
    if(e.getSizeX() > dx)
    {
      if(num == 0)
      {
        e.moveTo(x, y);
        y += e.getSizeY();
        ny = y;
      }
      else
      {
        num = 0;
        x = x0;
        y = ny;
        e.moveTo(x, y);
        y += e.getSizeY();
      }
    }
    else
    {
      if(x + e.getSizeX() > x1)
      {
        num = 1;
        x = x0;
        y = ny;
        e.moveTo(x, y);
        ny += e.getSizeY();
        x += e.getSizeX();
      }
      else
      {
        e.moveTo(x, y);
        x += e.getSizeX();
        num++;
        if(y + e.getSizeY() > ny) ny = y + e.getSizeY();
      }
    }
  }
}

void InternalContainerWrapping::addSubElement(Element* element)
{
  elements.push_back(element);
}


void InternalContainerWrapping::removeElement(Element* element)
{
  elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
}

void InternalContainerWrapping::clearSubElements()
{
  elements.clear();
}

////////////////////////////////////////////////////////////////////////////////

MainContainer::MainContainer()
{
  ctor();
}

MainContainer::MainContainer(IGUIDrawer& drawer)
: c(drawer)
, e(drawer)
{
  //the default container is as big as the screen (note: don't forget to resize it if you resize the resolution of the screen!)
  x0 = 0;
  y0 = 0;
  x1 = drawer.getWidth();
  y1 = drawer.getHeight();
  
  ctor();
}

void MainContainer::ctor()
{
  setEnabled();
  addSubElement(&c, STICKYFULL);
  c.pushTop(&e, STICKYFULL);
  c.pushTop(&h, STICKYFULL);
}

void MainContainer::addHoverElement(Element* element)
{
  h.pushTop(element);
  element->manageHover(*this);
}

void MainContainer::handleImpl(const IInput& input)
{
  h.clear();
  e.manageHover(*this);
  c.handle(input);
}

void MainContainer::drawImpl(IGUIDrawer& drawer) const
{
  c.draw(drawer);
  tooltips.draw(this, drawer);
}

const Element* MainContainer::hitTest(const IInput& input) const
{
  if(mouseOver(input))
  {
    const Element* result = c.hitTest(input);
    if(result) return result;
    return this;
  }
  else return 0;
}

ToolTipManager& MainContainer::getToolTipManager()
{
  return tooltips;
}

const ToolTipManager& MainContainer::getToolTipManager() const
{
  return tooltips;
}

bool MainContainer::doModalDialog(Dialog& dialog, IModalFrameHandler& frame)
{
  int x0, y0, x1, y1;
  dialog.setElementOver(false);
  while(frame.doFrame())
  {
    frame.getScreenSize(x0, y0, x1, y1); //gotten again every frame in case size changed
    dialog.handle(frame.getDrawer().getInput());
    h.clear();
    dialog.manageHover(*this);
    dialog.setElementOver(h.mouseOver(frame.getDrawer().getInput()));
    h.setElementOver(false);
    h.handle(frame.getDrawer().getInput());
    draw(frame.getDrawer());
    frame.getDrawer().drawRectangle(x0, y0, x1, y1, ColorRGB(0,0,0,128), true); //"modal darkness"
    dialog.draw(frame.getDrawer());
    tooltips.draw(&dialog, frame.getDrawer());
    h.draw(frame.getDrawer());
    if(dialog.done()) return true;
  }
  
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//TOOLTIPMANAGER                                                              //
////////////////////////////////////////////////////////////////////////////////

void ToolTipManager::registerElement(Element* element, const std::string& tip)
{
  elements[element] = tip;
}

void ToolTipManager::draw(const Element* root, IGUIDrawer& drawer) const
{
  const Element* element = root->hitTest(drawer.getInput());
  if(!element) return;
  std::map<const Element*, std::string>::const_iterator it = elements.find(element);
  if(it != elements.end())
  {
    drawToolTip(it->second, drawer);
  }
  else element->drawToolTip(drawer);
  
  /*
  TODO! think whether hitTest should only return container elements, or also
  internal-container elements from ElementComposite, and whether or not these
  may be publically available like that.
  */
}

void ToolTipManager::drawToolTip(const std::string& tip, IGUIDrawer& drawer)
{
  drawer.drawGUIPartText(GPT_TOOLTIP, tip, drawer.getInput().mouseX(), drawer.getInput().mouseY(), drawer.getInput().mouseX(), drawer.getInput().mouseY());
}

////////////////////////////////////////////////////////////////////////////////
//GUIELEMENT////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
GuiElement is the mother class of all the other GUI classes below.
*/

void Element::setEnabled(bool i_enable)
{
  if(i_enable)
  {
    enabled = true;
    //setElementOver(false); //don't do this, other things already handle this, the reason for not doing this is, if you constantly totallyEnable an element, then all the elementOver mechanism of containers won't work properly anymore
  }
  else
  {
    enabled = false;
    setElementOver(true); //so that sub-elements are also recursively hidden for "mouseOver" events
  }
}

Element::Element() : elementOver(false)
{
  setEnabled(true);
}

void Element::manageHoverImpl(IHoverManager& hover)
{
  (void)hover;
}

void Element::manageHover(IHoverManager& hover)
{
  if(enabled) manageHoverImpl(hover);
}

void Element::drawDebugBorder(IGUIDrawer& drawer, const ColorRGB& color) const
{
  drawer.drawLine(x0    , y0    , x0    , y1 - 1, color);
  drawer.drawLine(x1 - 1, y0    , x1 - 1, y1 - 1, color);
  drawer.drawLine(x0    , y0    , x1 - 1, y0    , color);
  drawer.drawLine(x0    , y1 - 1, x1 - 1, y1 - 1, color);
}

void Element::drawGUIPart(IGUIDrawer& drawer, const Element* element, GUIPart part)
{
  drawer.drawGUIPart(part, element->getX0(), element->getY0(), element->getX1(), element->getY1(), GUIPartMod(element->mouseOver(drawer.getInput()), element->mouseDown(drawer.getInput())));
}

void Element::drawGUIPartColor(IGUIDrawer& drawer, const Element* element, GUIPart part, const ColorRGB& color)
{
  drawer.drawGUIPartColor(part, color, element->getX0(), element->getY0(), element->getX1(), element->getY1(), GUIPartMod(element->mouseOver(drawer.getInput()), element->mouseDown(drawer.getInput())));
}

void Element::drawGUIPartText(IGUIDrawer& drawer, const Element* element, GUIPart part, const std::string& text)
{
  drawer.drawGUIPartText(part, text, element->getX0(), element->getY0(), element->getX1(), element->getY1(), GUIPartMod(element->mouseOver(drawer.getInput()), element->mouseDown(drawer.getInput())));
}


void Element::drawDebugCross(IGUIDrawer& drawer, const ColorRGB& color) const
{
  drawer.drawLine(x0, y0    , x1    , y1 - 1, color);
  drawer.drawLine(x0, y1 - 1, x1 - 1, y0    , color);
}

void Element::drawToolTip(IGUIDrawer& drawer) const
{
  (void)drawer; //by default, do nothing.
}

bool Element::mouseOver(const IInput& input) const
{
  if(!enabled) return false;
  if(elementOver) return false; //there is another element over this element, so says the container containing this element
  
  return ElementShape::mouseOver(input);
}

bool Element::mouseGrabbable() const
{
  return enabled;
}


void Element::draw(IGUIDrawer& drawer) const
{
  if(!enabled) return;
  
  drawImpl(drawer);
}

void Element::move(int x, int y)
{
  if(x == 0 && y == 0) return; //optimization to avoid many recursive calls to child elements for nothing
  
  this->x0 += x;
  this->y0 += y;
  this->x1 += x;
  this->y1 += y;
  
  moveImpl(x, y);
}

void Element::moveImpl(int /*x*/, int /*y*/)
{
  //this function is virtual
}

void Element::moveTo(int x, int y)
{
  this->move(x - this->x0, y - this->y0);
}

void Element::moveCenterTo(int x, int y)
{
  this->moveTo(x - this->getSizeX() / 2, y - this->getSizeY() / 2);
}

void Element::autoActivate(const IInput& input, MouseState& auto_activate_mouse_state, bool& control_active)
{
  bool over = mouseOver(input), down = input.mouseButtonDown(LMB);
  if(control_active && auto_activate_mouse_state.mouseDownElsewhere(over, down, input.mouseX(), input.mouseY(), mouseGetRelPosX(input), mouseGetRelPosY(input))) control_active = false;
  else if(!control_active && auto_activate_mouse_state.mouseDownHere(over, down)) control_active = true;
}

void Element::handle(const IInput& input)
{
  if(!enabled) return;
  
  handleImpl(input);
}

void Element::handleImpl(const IInput& input)
{
  (void)input;
  //no stuff needed for most elements
}

void Element::setElementOver(bool state)
{
  elementOver = state;
}

bool Element::hasElementOver() const
{
  return elementOver;
}

const Element* Element::hitTest(const IInput& input) const
{
  if(mouseOver(input)) return this;
  else return 0;
}

Element* Element::hitTest(const IInput& input)
{
  return const_cast<Element*>((const_cast<const Element*>(this))->hitTest(input));
}

bool Element::isFloating() const
{
  return false;
}

void Element::resize(int x0, int y0, int x1, int y1)
{
  if(x1 - x0 < getMinSizeX()) x1 = x0 + getMinSizeX();
  if(y1 - y0 < getMinSizeY()) y1 = y0 + getMinSizeY();

  Pos<int> newPos = { x0, y0, x1, y1 };
  
  resizeImpl(newPos); //done BEFORE the this->x0 etc... are set. Use the info from newPos instead.
  
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
}

void Element::resizeImpl(const Pos<int>& /*newPos*/)
{
  //nothing to do. Overload this for guielements that need to do something to sub elements if they resize.
}

void Element::drag(const IInput& input, MouseButton button)
{
  if(mouseGrabbed(input, button))
  {
    moveTo(input.mouseX() - mouseGetRelGrabX(), input.mouseY() - mouseGetRelGrabY());
  }
}

////////////////////////////////////////////////////////////////////////////////

void AElementComposite::move(int x, int y)
{
  if(x == 0 && y == 0) return; //optimization to avoid many recursive calls to child elements for nothing
  
  ic.move(x, y);
  
  Element::move(x, y);
}

void AElementComposite::setElementOver(bool state)
{
  Element::setElementOver(state);
  
  ic.setElementOver(state);
}

void AElementComposite::resize(int x0, int y0, int x1, int y1)
{
  Pos<int> oldPos = { this->x0, this->y0, this->x1, this->y1 };
  
  Element::resize(x0, y0, x1, y1);
  
  Pos<int> newPos = { this->x0, this->y0, this->x1, this->y1 };
  
  ic.resize(oldPos, newPos);
}

void AElementComposite::manageHoverImpl(IHoverManager& hover)
{
  ic.manageHover(hover);
}

int AElementComposite::getKeyboardFocus() const
{
  return ic.getKeyboardFocus();
}

////////////////////////////////////////////////////////////////////////////////

void ElementComposite::addSubElement(Element* element, const Sticky& sticky)
{
  my_ic.addSubElement(element, sticky, this);
}

void ElementComposite::clearSubElements()
{
  my_ic.clearSubElements();
}

////////////////////////////////////////////////////////////////////////////////
//LABEL/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Label::Label() : labelX(0), labelY(0)
{
}

void Label::makeLabel(const std::string& label, int labelX, int labelY, const Font& labelFont)
{
  this->label = label;
  this->labelX = labelX;
  this->labelY = labelY;
  this->labelFont = labelFont;
}

void Label::drawLabel(IGUIDrawer& drawer, const Element* element) const
{
  if(!label.empty())
    drawer.drawText(label, element->getX0() + labelX, element->getY0() + labelY, labelFont);
}

////////////////////////////////////////////////////////////////////////////////
//GUICONTAINER//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AContainer::AContainer(AInternalContainer* ic)
: elements(ic)
{
  setEnabled(true);
  x0 = 0;
  y0 = 0;
  x1 = 1;
  y1 = 1;
}

AContainer::AContainer(AInternalContainer* ic, IGUIDrawer& drawer)
: elements(ic)
{
  setEnabled(true);
  
  //the default container is as big as the screen (note: don't forget to resize it if you resize the resolution of the screen!)
  x0 = 0;
  y0 = 0;
  x1 = drawer.getWidth();
  y1 = drawer.getHeight();
}

void AContainer::make(int x, int y, int sizex, int sizey)
{
  //ic.getElements().clear();
  
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
}

void AContainer::setElementOver(bool state)
{
  if(state == elementOver) return; //this optimization is done because setElementOver gets called so much, it pops up on top of a profiler even when not even moving the mouse in the program
  Element::setElementOver(state);
  elements->setElementOver(state);
}

const Element* AContainer::hitTest(const IInput& input) const
{
  if(mouseOver(input))
  {
    const Element* result = elements->hitTest(input);
    if(result) return result;
    return this;
  }
  else return 0;
}

int AContainer::getKeyboardFocus() const
{
  return elements->getKeyboardFocus();
}

void AContainer::handleImpl(const IInput& input)
{
  if(mouseOver(input))
  {
    int topElement = -1;
    
    //priority to mouseGrabbed over mouseOver
    for(int i = size() - 1; i >= 0; i--)
    {
      elements->getElement(i)->setElementOver(false);
      bool grabbed = elements->getElement(i)->getMouseStateForContainer().mouseGrabbed(elements->getElement(i)->mouseOver(input)
                                                                                    , input.mouseButtonDown(LMB)
                                                                                    , input.mouseX()
                                                                                    , input.mouseY()
                                                                                    , elements->getElement(i)->mouseGetRelPosX(input)
                                                                                    , elements->getElement(i)->mouseGetRelPosY(input));
      if(grabbed)
      {
        if(topElement < 0) topElement = i;
        //break;
      }
      elements->getElement(i)->setElementOver(true);
    }
    
    //only now test mouseOver
    if(topElement == -1)
    for(int i = size() - 1; i >= 0; i--)
    {
      elements->getElement(i)->setElementOver(false);
      if(elements->getElement(i)->mouseOver(input))
      {
        topElement = i;
        break;
      }
      elements->getElement(i)->setElementOver(true);
    }

    //make all elements unresponsive to the mouse by setting "elementover", except the topElement
    for(size_t i = 0; i < size(); i++) elements->getElement(i)->setElementOver(true);
    if(topElement >= 0 && topElement < (int)size())
    {
      elements->getElement(topElement)->setElementOver(false);
      if(elements->getElement(topElement)->isFloating()
      && elements->getElement(topElement)->getMouseStateForContainer().mouseDownHere(elements->getElement(topElement)->mouseOver(input), input.mouseButtonDown(LMB)))
        bringToTop(elements->getElement(topElement));
    }
  }
  else for(size_t i = 0; i < size(); i++) elements->getElement(i)->setElementOver(true);

  for(unsigned long i = 0; i < size(); i++)
  {
    elements->getElement(i)->handle(input);
  }
}

void AContainer::drawElements(IGUIDrawer& drawer) const
{
  for(unsigned long i = 0; i < size(); i++)
  {
    elements->getElement(i)->draw(drawer);
  }
}

void AContainer::drawImpl(IGUIDrawer& drawer) const
{
  drawer.pushSmallestScissor(x0, y0, x1, y1);
  drawElements(drawer);
  drawer.popScissor();
}

void AContainer::manageHoverImpl(IHoverManager& hover)
{
  elements->manageHover(hover);
}

unsigned long AContainer::size() const
{
  return elements->getElements().size();
}

void AContainer::centerElement(Element* element)
{
  element->moveCenterTo(getCenterX(), getCenterY());
}

void AContainer::putInside(unsigned long i)
{
  if(i < size())
  {
    int ex0 = elements->getElement(i)->getX0();
    int ey0 = elements->getElement(i)->getY0();
    int ex1 = elements->getElement(i)->getX1();
    int ey1 = elements->getElement(i)->getY1();
    int esx = ex1 - ex0;
    int esy = ey1 - ey0;
    
    int newx = ex0, newy = ey0;
    
    if(ex0 < getX0()) newx = getX0();
    if(ey0 < getY0()) newy = getY0();
    if(ex1 > getX1()) newx = getX1() - esx;
    if(ey1 > getY1()) newy = getY1() - esy;
    
    //if the size of the element is too large to fit in the window, there's no reason to move it (or it'll warp all the time)
    if(elements->getElement(i)->getSizeX() > getSizeX()) newx = elements->getElement(i)->getX0();
    if(elements->getElement(i)->getSizeY() > getSizeY()) newy = elements->getElement(i)->getY0();
    
    elements->getElement(i)->moveTo(newx, newy);
  }
}

void AContainer::moveImpl(int x, int y)
{
  elements->move(x, y);
}

void AContainer::getRelativeElementPos(Element& element, int& ex, int& ey) const
{
  ex = element.getX0() - x0;
  ey = element.getY0() - y0;
}

void AContainer::resizeImpl(const Pos<int>& newPos)
{
  Pos<int> oldPos = { this->x0, this->y0, this->x1, this->y1 };
  elements->resize(oldPos, newPos);
}

void AContainer::setSizeToElements()
{
  int newx0 = x0, newy0 = y0, newx1 = x1, newy1 = y1;
  if(size() > 0)
  {
    newx0 = elements->getElement(0)->getX0();
    newy0 = elements->getElement(0)->getY0();
    newx1 = elements->getElement(0)->getX1();
    newy1 = elements->getElement(0)->getY1();
  }
  for(unsigned i = 1; i < size(); i++)
  {
    if(elements->getElement(i)->getX0() < newx0) newx0 = elements->getElement(i)->getX0();
    if(elements->getElement(i)->getY0() < newy0) newy0 = elements->getElement(i)->getY0();
    if(elements->getElement(i)->getX1() > newx1) newx1 = elements->getElement(i)->getX1();
    if(elements->getElement(i)->getY1() > newy1) newy1 = elements->getElement(i)->getY1();
  }
  
  x0 = newx0;
  y0 = newy0;
  x1 = newx1;
  y1 = newy1;
}

void AContainer::bringToTop(Element* element) //precondition: element must already be in the list
{
  size_t index = elements->findElementIndex(element);
  if(index >= size()) return;
  elements->getElements().erase(elements->getElements().begin() + index);
  elements->getElements().push_back(element);
}

////////////////////////////////////////////////////////////////////////////////

/*
Contains many elements in it and draws them and handles them, and even makes
sure you can't press the mouse "through" elements on top of other elements.
*/

Container::Container()
: AContainer(&elements_ic)
{
}

Container::Container(IGUIDrawer& drawer)
: AContainer(&elements_ic, drawer)
{
}

void Container::remove(Element* element)
{
  /*size_t index = elements.findElementIndex(element);
  if(index >= size()) return;
  elements.getElements().erase(elements.getElements().begin() + index);*/
  elements_ic.removeElement(element);
}

void Container::pushTop(Element* element, const Sticky& sticky)
{
  pushTopAt(element, element->getX0() - x0, element->getY0() - y0, sticky);
}

void Container::pushBottom(Element* element, const Sticky& sticky)
{
  pushBottomAt(element, element->getX0() - x0, element->getY0() - y0, sticky);
}

void Container::insert(size_t pos, Element* element, const Sticky& sticky)
{
  insertAt(pos, element, element->getX0() - x0, element->getY0() - y0, sticky);
}

void Container::pushTopRelative(Element* element, const Sticky& sticky)
{
  pushTopAt(element, element->getX0(), element->getY0(), sticky);
}

void Container::pushBottomRelative(Element* element, const Sticky& sticky)
{
  pushBottomAt(element, element->getX0(), element->getY0(), sticky);
}

void Container::insertRelative(size_t pos, Element* element, const Sticky& sticky)
{
  insertAt(pos, element, element->getX0(), element->getY0(), sticky);
}

void Container::pushTopAt(Element* element, int x, int y, const Sticky& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements_ic.addSubElement(element, sticky, this);
}

void Container::pushBottomAt(Element* element, int x, int y, const Sticky& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements_ic.insertSubElement(0, element, sticky, this);
}

void Container::insertAt(size_t pos, Element* element, int x, int y, const Sticky& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements_ic.insertSubElement(pos, element, sticky, this);
}

void Container::clear()
{
  elements_ic.clearSubElements();
}

////////////////////////////////////////////////////////////////////////////////

ContainerWrapping::ContainerWrapping()
: AContainer(&elements_ic)
{
}

ContainerWrapping::ContainerWrapping(IGUIDrawer& drawer)
: AContainer(&elements_ic, drawer)
{
}

void ContainerWrapping::remove(Element* element)
{
  elements_ic.removeElement(element);
}

void ContainerWrapping::pushTop(Element* element)
{
  remove(element); //prevent duplicates
  elements_ic.addSubElement(element);
}

void ContainerWrapping::clear()
{
  elements_ic.clearSubElements();
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



ScrollElement::ScrollElement()
: keepelementsinside(false)
, prevx0(-16000)
, prevy0(-16000)
, prevdx(-16000)
, prevdy(-16000)
, element(0)
{
}

void ScrollElement::make(int x, int y, int sizex, int sizey, Element* element, const IGUIDrawer& geom)
{
  ic.getElements().clear();
  
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  
  bars.make(x, y, sizex, sizey, 100.0, 100.0, geom);
  bars.disableV();
  bars.disableH();
  addSubElement(&bars, Sticky(0.0, 0, 0.0, 0, 1.0, 0, 1.0, 0));
  
  this->element = element;

  initBars();
}

void ScrollElement::initBars()
{
  if(!element) return;
  
  toggleBars();

  bars.vbar.scrollSize = element->getSizeY() - getVisibleSizeY();
  bars.hbar.scrollSize = element->getSizeX() - getVisibleSizeX();
  bars.vbar.scrollPos = y0 - element->getY0();
  bars.hbar.scrollPos = x0 - element->getX0();
}

void ScrollElement::updateBars()
{
  if(!element) return;
  
  if(bars.hbar.scrollSize < 0) bars.hbar.scrollSize = 0;
  if(bars.vbar.scrollSize < 0) bars.vbar.scrollSize = 0;
  if(keepelementsinside)
  {
    if(bars.hbar.scrollPos < 0) bars.hbar.scrollPos = 0;
    if(bars.vbar.scrollPos < 0) bars.vbar.scrollPos = 0;
    if(bars.hbar.scrollPos > bars.hbar.scrollSize) bars.hbar.scrollPos = bars.hbar.scrollSize;
    if(bars.vbar.scrollPos > bars.vbar.scrollSize) bars.vbar.scrollPos = bars.vbar.scrollSize;
  }
  
  resize(getX0(), getY0(), getX1(), getY1()); //VERY STUPID CODE. There is problem with the file list: if the number of files is such that the height of the list is exactly the list of the scroll element, it constantly flickers between V and H scrollbar. Simply doing this resize call fixes that problem. TODO: investigate the cause of the flickering and solve it in a better way
}

void ScrollElement::drawImpl(IGUIDrawer& drawer) const
{
  drawElements(drawer);
  drawScroll(drawer);
}

void ScrollElement::drawElements(IGUIDrawer& drawer) const
{
  drawer.pushSmallestScissor(getVisibleX0(), getVisibleY0(), getVisibleX1(), getVisibleY1()); //TODO: currently does same as pushScissor (because otherwise there's weird bug, to reproduce: resize the red window and look at smiley in the small grey window), so elements from container in container are still drawn outside container. DEBUG THIS ASAP!!!
  element->draw(drawer);
  drawer.popScissor();
}

void ScrollElement::drawScroll(IGUIDrawer& drawer) const
{
  bars.draw(drawer);
}

void ScrollElement::manageHoverImpl(IHoverManager& hover)
{
  ElementComposite::manageHoverImpl(hover);
  element->manageHover(hover);
}

const Element* ScrollElement::hitTest(const IInput& input) const
{
  if(mouseOver(input))
  {
    const Element* result = element->hitTest(input);
    if(result) return result;
    return this;
  }
  else return 0;
}

int ScrollElement::getKeyboardFocus() const
{
  return element->getKeyboardFocus();
}


void ScrollElement::moveImpl(int x, int y)
{
  ElementComposite::moveImpl(x, y);
  if(element) element->move(x, y);
}

void ScrollElement::handleImpl(const IInput& input)
{
  if(element)
  {
    if(mouseOver(input) && !(bars.henabled && bars.hbar.mouseOver(input)) && !(bars.venabled && bars.vbar.mouseOver(input)))
    {
      element->setElementOver(false);
      element->handle(input);
    }
    else
    {
      element->setElementOver(true);
      element->handle(input);
    }
  }
  
  bars.handle(input);
  
  if(element)
  {
    if(prevx0 != x0 - element->getX0() || prevy0 != y0 - element->getY0() || prevdx != element->getSizeX() - getVisibleSizeX() || prevdy != element->getSizeY() - getVisibleSizeY())
    {
      initBars();
    }
    
    int scrollx = x0 - int(bars.hbar.scrollPos);
    int scrolly = y0 - int(bars.vbar.scrollPos);
    
    //if bars are not enabled (if their size is 0), the position of the element should not be affected except that it should stay inside the scroll area
    if(bars.hbar.scrollSize == 0)
    {
      scrollx = element->getX0();
      if(keepelementsinside)
      {
        if(scrollx + element->getSizeX() > x1) scrollx = x1 - element->getSizeX();
        if(scrollx < x0) scrollx = x0;
      }
    }
    if(bars.vbar.scrollSize == 0)
    {
      scrolly = element->getY0();
      if(keepelementsinside)
      {
        if(scrolly + element->getSizeY() > y1) scrolly = y1 - element->getSizeY();
        if(scrolly < y0) scrolly = y0;
      }
    }
    
    moveAreaTo(scrollx, scrolly);
  }
  
  updateBars();

  if(element)
  {
    prevx0 = x0 - element->getX0();
    prevy0 = y0 - element->getY0();
    prevdx = element->getSizeX() - getVisibleSizeX();
    prevdy = element->getSizeY() - getVisibleSizeY();
  }
}

void ScrollElement::setElementOver(bool state)
{
  ElementComposite::setElementOver(state);
  element->setElementOver(state);
}

void ScrollElement::moveAreaTo(int x, int y)
{
  if(element) element->moveTo(x, y);
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
  if(bars.venabled) return x1 - bars.vbar.getSizeX();
  else return x1;
}

int ScrollElement::getVisibleY1() const
{
  if(bars.henabled) return y1 - bars.hbar.getSizeY();
  else return y1;
}

int ScrollElement::getVisibleSizeX() const
{
  return getVisibleX1() - getVisibleX0();
}

int ScrollElement::getVisibleSizeY() const
{
  return getVisibleY1() - getVisibleY0();
}

bool ScrollElement::mouseInVisibleZone(const IInput& input) const
{
  if(!mouseOver(input)) return false;
  return input.mouseX() >= getVisibleX0()
      && input.mouseX() < getVisibleX1()
      && input.mouseY() >= getVisibleY0()
      && input.mouseY() < getVisibleY1();
}


void ScrollElement::toggleBars() //turns bars on or of if they're needed or not (depending on visible size and scroll area size) (different than MS Windows where scrollbars become inactive grey when not needed)
{
  if(!element) return;
  
  if(element->getSizeX() <= getVisibleSizeX() && element->getSizeY() <= getVisibleSizeY())
  {
    bars.disableH();
    bars.disableV();
  }
  else if(element->getSizeX() > getVisibleSizeX() && element->getSizeY() <= getVisibleSizeY())
  {
    bars.enableH();
    bars.disableV();
  }
  else if(element->getSizeX() <= getVisibleSizeX() && element->getSizeY() > getVisibleSizeY())
  {
    bars.disableH();
    bars.enableV();
  }
  else if(element->getSizeX() > getVisibleSizeX() && element->getSizeY() > getVisibleSizeY())
  {
    bars.enableH();
    bars.enableV();
  }
}

////////////////////////////////////////////////////////////////////////////////
//GUIGROUP//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Group::isInside(int x, int y) const
{
  for(int i = size() - 1; i >= 0; i--)
  {
    if(elements->getElement(i)->isInside(x, y)) return true;
  }
  
  return false;
}

void Group::drawImpl(IGUIDrawer& drawer) const
{
  drawElements(drawer);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

StatusBar::StatusBar()
{
}

void StatusBar::drawImpl(IGUIDrawer& drawer) const
{
  drawer.pushSmallestScissor(x0, y0, x1, y1);
  drawer.drawGUIPart(GP_STATUSBAR, x0, y0, x1, y1);
  
  double x = 0;
  
  int available = x1 - x0;
  for(size_t i = 0; i < zonesizesabs.size(); i++) available -= zonesizesabs[i];

  for(size_t i = 0; i < zonesizesrel.size(); i++)
  {
    int xa = x0 + (int)x;
    if(i < zonesizesrel.size() - 1)
    {
      x += available * zonesizesrel[i];
      x += zonesizesabs[i];
    }
    else x = x1 - x0;
    int xb = x0 + (int)x;
    //int xb = (i == zonesize.size() - 1) ? x1 : (int)x + (int)((x1 - x0) * zonesizesrel[i] + 1);
    //if(i < zonesize.size() - 1) drawer.drawGUIPart(GP_STATUSBAR_SEPARATOR, xb, y0, xb, y1);
    if(i > 0) drawer.drawGUIPart(GP_STATUSBAR_SEPARATOR, xa, y0, xa, y1);
    
    drawer.pushSmallestScissor(xa, y0, xb, y1);
    drawer.drawGUIPartText(GPT_STATUSBAR_TEXT, texts[i], xa, y0 , xb, y1);
    drawer.popScissor();
  }
  drawer.popScissor();
}


void StatusBar::handleImpl(const IInput& input)
{
  (void)input;
}

void StatusBar::clear()
{
  texts.clear();
  zonesizesrel.clear();
  zonesizesabs.clear();
}

size_t StatusBar::getNumZones() const
{
  return zonesizesrel.size();
}

void StatusBar::setNumZones(size_t num)
{
  zonesizesrel.resize(num);
  zonesizesabs.resize(num);
  texts.resize(num);
}

void StatusBar::setZoneSize(size_t i, double relsize, int abssize)
{
  zonesizesrel[i] = relsize;
  zonesizesabs[i] = abssize;
}

void StatusBar::setZoneText(size_t i, const std::string& text)
{
  texts[i] = text;
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
: colorMod(RGB_Grey)
, enableTop(false)
, closeEnabled(false)
, enableResizer(false)
, resizerOverContainer(false)
, statusBar(0)
{
  setEnabled(true);

  addSubElement(&top, Sticky(0.0, 0, 0.0, 0, 1.0, 0, 0.0, top.getSizeY()));
  addSubElement(&resizer, Sticky(1.0, -resizer.getSizeX(), 1.0, -resizer.getSizeY(), 1.0, 0, 1.0, 0));
  addSubElement(&container, Sticky(0.0, 0, 0.0, 0, 1.0, 0, 1.0, 0));
  addSubElement(&closeButton, Sticky(1.0, -closeButton.getSizeX(), 0.0, 0, 1.0, 0, 0.0, closeButton.getSizeY()));
}

Window::~Window()
{
  if(statusBar) delete statusBar;
}

const Element* Window::hitTest(const IInput& input) const
{
  if(mouseOver(input))
  {
    const Element* result = container.hitTest(input);
    if(result && result != &container) return result;
    return this;
  }
  else return 0;
}

int Window::getRelContentStart() const
{
  int result = 0;
  if(enableTop) result = top.getY1() - y0;
  
  return result;
}

void Window::addCloseButton(const IGUIDrawer& geom)
{
  int closeX = x1 - geom.getGUIPartSizeX(GP_WINDOW_CLOSE); //TODO: give IGUIPartDrawer functions to get hte width and height of some GUI components like this one and use that here.
  int closeY = y0;
  closeButton.resize(closeX, closeY, closeX + geom.getGUIPartSizeX(GP_WINDOW_CLOSE), closeY + geom.getGUIPartSizeY(GP_WINDOW_CLOSE));
  
  closeEnabled = true;
  
  my_ic.setSticky(&closeButton, Sticky(1.0, -closeButton.getSizeX(), 0.0, 0, 1.0, 0, 0.0, closeButton.getSizeY()), this);
}

void Window::addResizer(const IGUIDrawer& geom, bool overContainer)
{
  int resizerX = x1 - geom.getGUIPartSizeX(GP_WINDOW_RESIZER);
  int resizerY = y1 - geom.getGUIPartSizeY(GP_WINDOW_RESIZER);
  resizer.resize(resizerX, resizerY, resizerX + geom.getGUIPartSizeX(GP_WINDOW_RESIZER), resizerY + geom.getGUIPartSizeY(GP_WINDOW_RESIZER));
  enableResizer = true;
  my_ic.setSticky(&resizer, Sticky(1.0, -resizer.getSizeX() - 1, 1.0, -resizer.getSizeY() - 1, 1.0, -1, 1.0, -1), this);
  
  if(!overContainer)
  {
    Sticky sticky = my_ic.getSticky(&container);
    sticky.i.y1 = resizer.getY0() - y1;
    my_ic.setSticky(&container, sticky, this);
  }
  
  resizerOverContainer = overContainer;
}

void Window::addStatusBar()
{
  statusBar = new StatusBar();
  statusBar->resize(x0 + 1, 0, x1 - 1, 16);
  addSubElement(statusBar, Sticky(0.0,+1 ,1.0,-17, 1.0,-1, 1.0,-1));
}


void Window::addTop(const IGUIDrawer& geom)
{
  top.resize(x0, y0, x1, y0 + geom.getGUIPartSizeY(GP_WINDOW_TOP));
  this->enableTop = 1;
  my_ic.setSticky(&top, Sticky(0.0, 0, 0.0, 0, 1.0, 0, 0.0, top.getSizeY()), this);

  Sticky sticky = my_ic.getSticky(&container);
  sticky.i.y0 = top.getY1() - y0;
  my_ic.setSticky(&container, sticky, this);
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

bool Window::isFloating() const
{
  return true;
}

//to let the scrollbars work properly, call this AFTER using setContainerBorders, addTop, addResizer and such of the window
void Window::addScrollbars(const IGUIDrawer& geom)
{
  if(scroll.element) return;
  
  scroll.make(container.getX0(), container.getY0(), container.getSizeX(), container.getSizeY(), &container, geom);
  container.setSizeToElements();
  my_ic.removeElement(&container); //the scrollbars must control the container now
  addSubElement(&scroll);
  my_ic.setStickyFull(&scroll, this);
}

void Window::removeScrollbars()
{
  if(!scroll.element) return;
  
  scroll.element = 0;
  my_ic.removeElement(&scroll);
  addSubElement(&container);
  container.resize(scroll.getX0(), scroll.getY0(), scroll.getX1(), scroll.getY1());
  my_ic.setStickyFull(&container, this);
}

void Window::updateScroll()
{
  container.setSizeToElements();
  scroll.updateBars();
}

void Window::putInside(int i)
{
  container.putInside(i);
}

void Window::handleImpl(const IInput& input)
{
  if(scroll.element) scroll.handle(input);
  else container.handle(input);
  
  //resize window
  if(enableResizer && resizer.mouseGrabbed(input)) resize(x0, y0, input.mouseX() - resizer.mouseGetRelGrabX() + (x1 - resizer.getX0()), input.mouseY() - resizer.mouseGetRelGrabY() + (y1 - resizer.getY0()));

  //drag window
  if(!enableTop && mouseGrabbed(input)) moveTo(input.mouseX() - mouseGetRelGrabX(), input.mouseY() - mouseGetRelGrabY()); //dragging of window without a top
  if(enableTop && top.mouseGrabbed(input))
  {
    moveTo(input.mouseX() - top.mouseGetRelGrabX() - (top.getX0() - getX0()), input.mouseY() - top.mouseGetRelGrabY() - (top.getY0() - getY0()));
  }
  
  //the scrollbar's conserveCorner should be the same as this window's resizerOverContainer
  scroll.bars.conserveCorner = resizerOverContainer;
}

void Window::drawWindow(IGUIDrawer& drawer) const
{
  drawer.drawGUIPartColor(GPC_WINDOW_PANEL, colorMod, x0, y0, x1, y1);
  if(enableTop)
  {
    drawer.drawGUIPart(GP_WINDOW_TOP, top.getX0(), top.getY0(), top.getX1(), top.getY1()); //draw top bar before the elements, or it'll appear above windows relative to the current window
    drawer.drawGUIPartText(GPT_WINDOW_TITLE, title, top.getX0(), top.getY0(), top.getX1(), top.getY1());
  }

  if(closeEnabled) drawGUIPart(drawer, &closeButton, GP_WINDOW_CLOSE);

  if(statusBar != 0) statusBar->draw(drawer);
  if(enableResizer) drawer.drawGUIPart(GP_WINDOW_RESIZER, resizer.getX0(), resizer.getY0(), resizer.getX1(), resizer.getY1()); //draw this after the container so the resizer is drawn over scrollbars if that is needed
  
  if(scroll.element) scroll.drawScroll(drawer);
  
}

void Window::drawElements(IGUIDrawer& drawer) const
{
  if(scroll.element) scroll.drawElements(drawer);
  else container.draw(drawer);
}

void Window::drawImpl(IGUIDrawer& drawer) const
{
  drawWindow(drawer);
  drawElements(drawer);
}

void Window::manageHoverImpl(IHoverManager& hover)
{
  ElementComposite::manageHoverImpl(hover);
  if(scroll.element) scroll.manageHover(hover);
  else container.manageHover(hover);
}

int Window::getKeyboardFocus() const
{
  if(scroll.element) return scroll.getKeyboardFocus();
  else return container.getKeyboardFocus();
}

void Window::addTitle(const std::string& title, const Font& titleFont)
{
  this->title = title;
  this->titleFont = titleFont;
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

void Window::pushTopAt(Element* element, int x, int y, const Sticky& sticky)
{
  container.pushTopAt(element, x, y, sticky);
}

void Window::pushBottomAt(Element* element, int x, int y, const Sticky& sticky)
{
  container.pushBottomAt(element, x, y, sticky);
}

void Window::insertAt(size_t pos, Element* element, int x, int y, const Sticky& sticky)
{
  container.insertAt(pos, element, x, y, sticky);
}

void Window::pushTop(Element* element, const Sticky& sticky)
{
  container.pushTop(element, sticky);
}

void Window::pushBottom(Element* element, const Sticky& sticky)
{
  container.pushBottom(element, sticky);
}

void Window::insert(size_t pos, Element* element, const Sticky& sticky)
{
  container.insert(pos, element, sticky);
}

void Window::pushTopRelative(Element* element, const Sticky& sticky)
{
  container.pushTopRelative(element, sticky);
}

void Window::pushBottomRelative(Element* element, const Sticky& sticky)
{
  container.pushBottomRelative(element, sticky);
}

void Window::insertRelative(size_t pos, Element* element, const Sticky& sticky)
{
  container.insertRelative(pos, element, sticky);
}

void Window::clear()
{
  container.clear();
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
: draw_mouse_button_down_style(false)
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
  this->font[0].color = RGB_Black;
  this->font[1].color = RGB_Black;
  this->font[2].color = RGB_Black;
  
  this->enabled = 0;
  
  this->mouseDownVisualStyle = 0;
}

void Button::addFrontImage
(
  HTexture* texture1, HTexture* texture2, HTexture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3
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

void Button::addFrontImage(HTexture* texture)
{
  addFrontImage(texture, texture, texture, imageColor[0], imageColor[1], imageColor[2]);
}

//Constructor to make a button with only images, no text
void Button::makeImage
(
  int x, int y, //basic properties
  HTexture* texture1, HTexture* texture2, HTexture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3 //image
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(texture1->texture->getU());
  this->setSizeY(texture1->texture->getV());
  
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
  this->font[0].color = RGB_Black;
  this->font[1].color = RGB_Black;
  this->font[2].color = RGB_Black;
  
  this->setEnabled(true);
  
  this->mouseDownVisualStyle = 0;
}

//Constructor to make a button with only text, no images
void Button::makeText
(
  int x, int y, //basic properties
  const std::string& text, //text
  const ITextDrawer& drawer
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(0);
  this->setSizeY(0);
  
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
  
  this->setEnabled(true);
  
  this->mouseDownVisualStyle = 0;
  
  autoTextSize(&drawer);
}

//constructor for button with panel and text, auto generated text offset
void Button::makeTextPanel
(
  int x, int y, const std::string& text, int sizex, int sizey //basic properties + text
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  
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
  
  this->panelOffsetx = 0;
  this->panelOffsety = 0;

  this->setEnabled(true);
  
  this->mouseDownVisualStyle = 1;
}

void Button::handleImpl(const IInput& input)
{
  if((mouseDownVisualStyle == 0 && mouseDown(input, LMB))
  || (mouseDownVisualStyle == 1 && button_drawing_mouse_test.mouseDownHere(mouseOver(input), input.mouseButtonDown(LMB)))
  || (mouseDownVisualStyle == 2 && button_drawing_mouse_test.mouseGrabbed(mouseOver(input), input.mouseButtonDown(LMB), input.mouseX(), input.mouseY(), mouseGetRelPosX(input), mouseGetRelPosY(input))))
  {
    draw_mouse_button_down_style = true;
  }
  else draw_mouse_button_down_style = false;
}

//Draws the button, also checks for mouseover and mousedown to draw mouseover graphic
void Button::drawImpl(IGUIDrawer& drawer) const
{
  int style = 0; //0 = normal, 1 = mouseOver, 2 = mouseDown
  
  if(mouseOver(drawer.getInput())) style = 1;
  if(draw_mouse_button_down_style) style = 2;
  
  if(enablePanel) drawGUIPart(drawer, this, GP_BUTTON_PANEL);
  if(enableImage)
  {
    drawer.convertTextureIfNeeded(image[style]->texture);
    drawer.drawTexture(image[style]->texture, x0 + imageOffsetx, y0 + imageOffsety, imageColor[style]);
  }
  if(enableImage2)
  {
    drawer.convertTextureIfNeeded(image2[style]->texture);
    drawer.drawTexture(image2[style]->texture, x0 + imageOffsetx2, y0 + imageOffsety2, imageColor2[style]);
  }
  if(enableText && !enablePanel) drawGUIPartText(drawer, this, GPT_TEXT_BUTTON_TEXT, text);
  //TODO: write all button texts using the guipartdrawer
  else drawGUIPartText(drawer, this, GPT_PANEL_BUTTON_TEXT, text);
  
}

/*This function automaticly changes sizex and sizey of the button so that the
size of the button matches the size of the text, so that it detects the mouse
as being in the button when the mouse is over the text*/
void Button::autoTextSize(const ITextDrawer* drawer, int extrasize)
{
  int w, h;
  drawer->calcTextRectSize(w, h, text, font[0]);
  setSizeX(w);
  setSizeY(h);
  x1 += 2 * extrasize;
  y1 += 2 * extrasize;
  x0 -= extrasize;
  y0 -= extrasize;
}

void Button::makeImage(int x, int y,  HTexture* texture123, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3)
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
    return getSizeY() - buttonUp.getSizeY() - buttonDown.getSizeY() - scroller.getSizeY(); //scroller length also subtracted from it: the scroller can only move over this distance since it's full width must always be inside the slider length
  }
  else
  {
    return getSizeX() - buttonUp.getSizeX() - buttonDown.getSizeX() - scroller.getSizeX();
  }
}

int Scrollbar::getSliderStart() const
{
  if(direction == V)
  {
    return buttonUp.getSizeY();
  }
  else
  {
    return buttonUp.getSizeX();
  }
}

int Scrollbar::getSliderEnd() const
{
  if(direction == V)
  {
    return getSizeY() - buttonDown.getSizeY() ;
  }
  else
  {
    return getSizeX() - buttonDown.getSizeX() ;
  }
}

void Scrollbar::init(const IGUIDrawer& geom)
{
  if(scrollSize == 0) scrollSize = 1;

  if(direction == V)
  {
    buttonUp.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_N), geom.getGUIPartSizeY(GP_SCROLLBAR_N));
    buttonUp.moveTo(x0, y0);
    buttonDown.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_S), geom.getGUIPartSizeY(GP_SCROLLBAR_S));
    buttonDown.moveTo(x0, y1 - geom.getGUIPartSizeY(GP_SCROLLBAR_S));
    scroller.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_SCROLLER), geom.getGUIPartSizeY(GP_SCROLLBAR_SCROLLER));
    scroller.moveTo(x0, int(y0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize));
    my_ic.setSticky(&buttonDown, Sticky(0.0, 0, 1.0, -buttonDown.getSizeY(), 1.0, 0, 1.0, 0), this);
  }
  else
  {
    buttonUp.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_W), geom.getGUIPartSizeY(GP_SCROLLBAR_W));
    buttonUp.moveTo(x0, y0);
    buttonDown.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_E), geom.getGUIPartSizeY(GP_SCROLLBAR_E));
    buttonDown.moveTo(x1 - geom.getGUIPartSizeX(GP_SCROLLBAR_E), y0);
    scroller.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_SCROLLER), geom.getGUIPartSizeY(GP_SCROLLBAR_SCROLLER));
    scroller.moveTo(int(x0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), y0);
    my_ic.setSticky(&buttonDown, Sticky(1.0, -buttonDown.getSizeX(), 0.0, 0, 1.0, 0, 1.0, 0), this);
  }
}

Scrollbar::Scrollbar()
{
  this->scrollSize = 0;
  this->scrollPos = 0;
  this->scrollSpeed = 0;
  this->absoluteSpeed = 0;
  this->speedMode = 0;
  this->forwardedScroll = 0;
  
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

void Scrollbar::makeVertical(int x, int y, int length,
                double scrollSize, const IGUIDrawer& geom,
                double scrollPos, double offset, double scrollSpeed,
                int speedMode)
{
  this->x0 = x;
  this->y0 = y;
  this->direction = V;
  this->setSizeX(geom.getGUIPartSizeX(GP_SCROLLBAR_VBACK));
  this->setSizeY(length);
  this->scrollSize = scrollSize;
  this->scrollPos = scrollPos;
  this->scrollSpeed = scrollSpeed;
  this->offset = offset;
  
  this->absoluteSpeed = scrollSpeed;
  this->speedMode = speedMode;
  
  this->setEnabled(true);
  
  init(geom);
  
  if(speedMode == 1) setRelativeScrollSpeed();
}

void Scrollbar::makeHorizontal(int x, int y, int length,
                  double scrollSize, const IGUIDrawer& geom,
                  double scrollPos, double offset, double scrollSpeed,
                  int speedMode)
{
  this->x0 = x;
  this->y0 = y;
  this->direction = H;
  this->setSizeX(length);
  this->setSizeY(geom.getGUIPartSizeY(GP_SCROLLBAR_HBACK));
  this->scrollSize = scrollSize;
  this->scrollPos = scrollPos;
  this->scrollSpeed = scrollSpeed;
  this->offset = offset;
  
  this->absoluteSpeed = scrollSpeed;
  this->speedMode = speedMode;
  
  this->setEnabled(true);
  
  init(geom);
  
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

void Scrollbar::handleImpl(const IInput& input)
{
  double scrollDir = 0;

  if(buttonUp.mouseDownHere(input)) scrollDir = -1;
  if(buttonDown.mouseDownHere(input)) scrollDir = 1;

  if(direction == V)
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed(input);
    bool buttonDown_grabbed = buttonDown.mouseGrabbed(input);
    if(scroller.mouseGrabbed(input))
    {
      scrollPos = (scrollSize * (input.mouseY() - y0 - getSliderStart() - scroller.getSizeY() / 2)) / getSliderSize();
      if(scrollPos < 0) scrollPos = 0;
      if(scrollPos > scrollSize) scrollPos = scrollSize;
    }
    else if(mouseDownHere(input) && !scroller.mouseGrabbed(input) && !buttonUp_grabbed && !buttonDown_grabbed)
    {
      scrollPos = (scrollSize * (input.mouseY() - y0 - getSliderStart() - scroller.getSizeY() / 2)) / getSliderSize();
      scroller.mouseGrab(input);
    }
    if(mouseScrollUp(input)) scrollDir = -4;
    if(mouseScrollDown(input)) scrollDir = 4;
    if(forwardedMouseScrollUp()) scrollDir = -4;
    if(forwardedMouseScrollDown()) scrollDir = 4;
  }
  else
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed(input);
    bool buttonDown_grabbed = buttonDown.mouseGrabbed(input);
    if(scroller.mouseGrabbed(input))
    {
      scrollPos = (scrollSize * (input.mouseX() - x0 - getSliderStart() - scroller.getSizeX() / 2)) / getSliderSize();
      if(scrollPos < 0) scrollPos = 0;
      if(scrollPos > scrollSize) scrollPos = scrollSize;
    }
    else if(mouseDownHere(input) && !scroller.mouseGrabbed(input) && !buttonUp_grabbed && !buttonDown_grabbed)
    {
      scrollPos = (scrollSize * (input.mouseX() - x0 - getSliderStart() - scroller.getSizeX() / 2)) / getSliderSize();
      scroller.mouseGrab(input);
    }
  }
  
  if(scrollDir != 0)
  {
    scrollPos += scrollDir * absoluteSpeed * ((input.getSeconds() - oldTime));
  }
  oldTime = input.getSeconds();

  //DON'T do this HERE! Allow storing values in the scrollbar that are out of range! Only when the user interacts with the scrollbar, will in-range values be resulted.
  //if(scrollPos < 0) scrollPos = 0;
  //if(scrollPos > scrollSize) scrollPos = scrollSize;
  
  if(speedMode == 1) setRelativeScrollSpeed();
  
  int scrollPosForVisuals = (int)scrollPos;
  if(scrollPosForVisuals < 0) scrollPosForVisuals = 0;
  if(scrollPosForVisuals > scrollSize) scrollPosForVisuals = (int)scrollSize;
  if(direction == V) scroller.moveTo(scroller.getX0(), int(y0 + getSliderStart() + (getSliderSize() * scrollPosForVisuals) / scrollSize));
  else scroller.moveTo(int(x0 + getSliderStart() + (getSliderSize() * scrollPosForVisuals) / scrollSize), scroller.getY0());
}

//from an external source, use this function only BEFORE using the handle() function or getTicks() - oldTime will be zero
void Scrollbar::scroll(const IInput& input, int dir)
{
  scrollPos += dir * absoluteSpeed * (input.getSeconds() - oldTime);
}

void Scrollbar::drawImpl(IGUIDrawer& drawer) const
{
  //in the drawRepeated functions, sizeButton is divided through two, so if the arrow buttons have a few transparent pixels, in one half of the button you see the background through, in the other half not
  if(direction == V)
  {
    drawer.drawGUIPart(GP_SCROLLBAR_VBACK, x0, y0 + getSliderStart(), x1, y0 + getSliderEnd());
    drawer.drawGUIPart(GP_SCROLLBAR_N, buttonUp.getX0(), buttonUp.getY0(), buttonUp.getX1(), buttonUp.getY1());
    drawer.drawGUIPart(GP_SCROLLBAR_S, buttonDown.getX0(), buttonDown.getY0(), buttonDown.getX1(), buttonDown.getY1());

  }
  else
  {
    drawer.drawGUIPart(GP_SCROLLBAR_HBACK, x0 + getSliderStart(), y0, x0 + getSliderEnd(), y1);
    drawer.drawGUIPart(GP_SCROLLBAR_W, buttonUp.getX0(), buttonUp.getY0(), buttonUp.getX1(), buttonUp.getY1());
    drawer.drawGUIPart(GP_SCROLLBAR_E, buttonDown.getX0(), buttonDown.getY0(), buttonDown.getX1(), buttonDown.getY1());
  }

  if(scrollSize > 0)
  {
    if(scrollPos < 0 || scrollPos > scrollSize) drawer.drawGUIPart(GP_SCROLLBAR_SCROLLER_OUT_OF_RANGE, scroller.getX0(), scroller.getY0(), scroller.getX1(), scroller.getY1());
    else drawer.drawGUIPart(GP_SCROLLBAR_SCROLLER, scroller.getX0(), scroller.getY0(), scroller.getX1(), scroller.getY1());
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
  addSubElement(&vbar);
  addSubElement(&hbar);
  addSubElement(&corner);
}

int ScrollbarPair::getVisiblex() const
{
  if(venabled) return x1 - x0 - vbar.getSizeX();
  else return x1 - x0;
}

int ScrollbarPair::getVisibley() const
{
  if(venabled) return y1 - y0 - hbar.getSizeY();
  else return y1 - y0;
}

void ScrollbarPair::make(int x, int y, int sizex, int sizey, double scrollSizeH, double scrollSizeV, const IGUIDrawer& geom)
{
  this->setEnabled(true);
  this->x0 = x;
  this->y0 = y;
  this->x1 = x + sizex;
  this->y1 = y + sizey;
  //TODO: use gui part drawer to draw this
  //this->txCorner = set->scrollBarPairCorner;
  
  vbar.makeVertical(x1 - geom.getGUIPartSizeX(GP_SCROLLBAR_VBACK), y0, sizey - geom.getGUIPartSizeY(GP_SCROLLBARPAIR_CORNER),
          scrollSizeV, geom, 0, 0, 1, 1);
  hbar.makeHorizontal(x0, y1 - geom.getGUIPartSizeX(GP_SCROLLBAR_HBACK), sizex - geom.getGUIPartSizeX(GP_SCROLLBARPAIR_CORNER),
          scrollSizeH, geom, 0, 0, 1, 1);

  my_ic.setSticky(&vbar, STICKYOFF/*Sticky(1.0, -vbar.getSizeX(), 0.0, 0, 1.0, 0, 1.0, 0)*/, this);
  my_ic.setSticky(&hbar, STICKYOFF/*Sticky(0.0,  0, 1.0, -hbar.getSizeY(), 1.0, 0, 1.0, 0)*/, this);
  this->venabled = true;
  this->henabled = true;
}

void ScrollbarPair::resizeImpl(const Pos<int>& newPos)
{
  if(venabled) hbar.resize(newPos.x0, newPos.y1 - hbar.getSizeY(), newPos.x1 - vbar.getSizeX(), newPos.y1);
  else hbar.resize(newPos.x0, newPos.y1 - hbar.getSizeY(), newPos.x1 - vbar.getSizeX() * conserveCorner, newPos.y1);
  
  if(henabled) vbar.resize(newPos.x1 - vbar.getSizeX(), newPos.y0, newPos.x1, newPos.y1 - hbar.getSizeY());
  else vbar.resize(newPos.x1 - vbar.getSizeX(), newPos.y0, newPos.x1, newPos.y1 - hbar.getSizeY() * conserveCorner);
}

void ScrollbarPair::disableV()
{
  venabled = false;
  
  if(henabled) hbar.resize(hbar.getX0(), hbar.getY0(), hbar.getX0() + getSizeX() - vbar.getSizeX() * conserveCorner, hbar.getY1());
}

void ScrollbarPair::disableH()
{
  henabled = false;
  
  if(venabled) vbar.resize(vbar.getX0(), vbar.getY0(), vbar.getX1(), vbar.getY0() + getSizeY() - hbar.getSizeY() * conserveCorner);
}

void ScrollbarPair::enableV()
{
  venabled = true;
  
  if(henabled)
  {
    vbar.resize(vbar.getX0(), vbar.getY0(), vbar.getX1(), vbar.getY0() + getSizeY() - hbar.getSizeY());
    hbar.resize(hbar.getX0(), hbar.getY0(), hbar.getX0() + getSizeX() - vbar.getSizeX(), hbar.getY1());
  }
  else
  {
    vbar.resize(vbar.getX0(), vbar.getY0(), vbar.getX1(), vbar.getY0() + getSizeY() - hbar.getSizeY() * conserveCorner);
  }
}

void ScrollbarPair::enableH()
{
  henabled = true;
  
  if(venabled)
  {
    hbar.resize(hbar.getX0(), hbar.getY0(), hbar.getX0() + getSizeX() - vbar.getSizeX(), hbar.getY1());
    vbar.resize(vbar.getX0(), vbar.getY0(), vbar.getX1(), vbar.getY0() + getSizeY() - hbar.getSizeY());
  }
  else
  {
    hbar.resize(hbar.getX0(), hbar.getY0(), hbar.getX0() + getSizeX() - vbar.getSizeX() * conserveCorner, hbar.getY1());
  }
}

void ScrollbarPair::handleImpl(const IInput& input)
{
  if(venabled) vbar.handle(input);
  if(henabled) hbar.handle(input);
}

void ScrollbarPair::drawImpl(IGUIDrawer& drawer) const
{
  if(venabled) vbar.draw(drawer);
  if(henabled) hbar.draw(drawer);
  if((venabled && henabled && !conserveCorner) || ((venabled || henabled) && conserveCorner)) drawer.drawGUIPart(GP_SCROLLBARPAIR_CORNER, x1, y1, x1, y1);
}

////////////////////////////////////////////////////////////////////////////////
//GUISLIDER/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
A simplified version of the scrollbar
*/

Slider::Slider()
: slidertype(GP_SLIDER_HBUTTON)
{
  this->enabled = 0;
  addSubElement(&slider);
}

void Slider::makeSmallHorizontal(int x, int y, int length, double scrollSize, const IGUIDrawer& geom)
{
  this->setEnabled(true);
  
  slidertype = GP_SMALL_SLIDER_HBUTTON;
  rulertype = GP_SMALL_SLIDER_HBACK;
  
  size_t sliderw = geom.getGUIPartSizeX(slidertype);
  size_t sliderh = geom.getGUIPartSizeY(slidertype);
  slider.resize(0, 0, sliderw, sliderh);

  int sliderCenter = sliderh / 2;
  int rulerCenter = geom.getGUIPartSizeY(rulertype) / 2;
  int centerPos = sliderCenter;
  if(rulerCenter > sliderCenter) centerPos = rulerCenter;
  
  this->direction = H;
  this->scrollPos = 0;
  this->scrollSize = scrollSize;
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(length);
  this->setSizeY(sliderh);
}

void Slider::makeHorizontal(int x, int y, int length, double scrollSize, const IGUIDrawer& geom)
{
  this->setEnabled(true);
  
  slidertype = GP_SLIDER_HBUTTON;
  rulertype = GP_SLIDER_HBACK;
  
  size_t sliderw = geom.getGUIPartSizeX(slidertype);
  size_t sliderh = geom.getGUIPartSizeY(slidertype);
  slider.resize(0, 0, sliderw, sliderh);

  int sliderCenter = sliderh / 2;
  int rulerCenter = geom.getGUIPartSizeY(rulertype) / 2;
  int centerPos = sliderCenter;
  if(rulerCenter > sliderCenter) centerPos = rulerCenter;
  
  this->direction = H;
  this->scrollPos = 0;
  this->scrollSize = scrollSize;
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(length);
  this->setSizeY(sliderh);
}

void Slider::makeVertical(int x, int y, int length, double scrollSize, const IGUIDrawer& geom)
{
  this->setEnabled(true);
  
  slidertype = GP_SLIDER_VBUTTON;
  rulertype = GP_SLIDER_VBACK;
  
  size_t sliderw = geom.getGUIPartSizeX(slidertype);
  size_t sliderh = geom.getGUIPartSizeY(slidertype);
  slider.resize(0, 0, sliderw, sliderh);
  
  int sliderCenter =  sliderw / 2;
  int rulerCenter = geom.getGUIPartSizeX(rulertype) / 2;
  int centerPos = sliderCenter;
  if(rulerCenter > sliderCenter) centerPos = rulerCenter;
  
  this->direction = V;
  this->scrollPos = 0;
  this->scrollSize = scrollSize;
  this->x0 = x;
  this->y0 = y;
  this->setSizeY(length);
  this->setSizeX(sliderw);
}

//screenPos must be relative to the x or y position of the gui element!!!
double Slider::screenPosToScrollPos(int screenPos)
{
  if(direction == H)
  {
    int sliderSize = slider.getSizeX();
    return (scrollSize * (screenPos - sliderSize / 2)) / (getSizeX() - sliderSize);
  }
  else //if(direction == V)
  {
    int sliderSize = slider.getSizeY();
    return (scrollSize * (screenPos - sliderSize / 2)) / (getSizeY() - sliderSize);
  }
}

//screenPos is relative to the x or y position of the gui element!!!
int Slider::scrollPosToScreenPos(double scrollPos)
{
  if(direction == H)
  {
    int sliderSize = slider.getSizeX();
    return int(((getSizeX() - sliderSize) * scrollPos) / scrollSize);
  }
  else //if(direction == V)
  {
    int sliderSize = slider.getSizeY();
    return int(((getSizeY() - sliderSize) * scrollPos) / scrollSize);
  }
}

void Slider::drawImpl(IGUIDrawer& drawer) const
{
  if(direction == H)
  {
    drawer.drawGUIPart(rulertype, x0, y0, x1, y1);
    drawer.drawGUIPart(slidertype, slider.getX0(), slider.getY0(), slider.getX1(), slider.getY1());
  }
  else //if(direction == V)
  {
    drawer.drawGUIPart(rulertype, x0, y0, x1, y1);
    drawer.drawGUIPart(slidertype, slider.getX0(), slider.getY0(), slider.getX1(), slider.getY1());
  }
}

void Slider::handleImpl(const IInput& input)
{
  if(direction == H)
  {
    /*int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCenterY();
    if(rulerCenter > centerPos) centerPos = rulerCenter;*/
    //int sliderSize = slider.texture1->getU();
    
    if(slider.mouseGrabbed(input)) scrollPos = screenPosToScrollPos(mouseGetRelPosX(input));
    else if(mouseDownHere(input) && !slider.mouseOver(input))
    {
      scrollPos = screenPosToScrollPos(mouseGetRelPosX(input));
      slider.mouseGrab(input);
    }
    
    if(scrollPos < 0) scrollPos = 0;
    if(scrollPos > scrollSize) scrollPos = scrollSize;
    
    slider.moveTo(x0 + scrollPosToScreenPos(scrollPos), getCenterY() - slider.getSizeY() / 2);
  }
  else //if(direction == V)
  {
    /*int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCenterX();
    if(rulerCenter > centerPos) centerPos = rulerCenter;*/
    //int sliderSize = slider.texture1->getV();
    
    if(slider.mouseGrabbed(input)) scrollPos = screenPosToScrollPos(mouseGetRelPosY(input));
    else if(mouseDownHere(input) && !slider.mouseOver(input))
    {
      scrollPos = screenPosToScrollPos(mouseGetRelPosY(input));
      slider.mouseGrab(input);
    }
    
    if(scrollPos < 0) scrollPos = 0;
    if(scrollPos > scrollSize) scrollPos = scrollSize;
    
    slider.moveTo(getCenterX() - slider.getSizeX() / 2, y0 + scrollPosToScreenPos(scrollPos));
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
: useCustomImages(false)
, useCustomImages2(false)
, partOn(GP_CHECKBOX_ON)
, partOff(GP_CHECKBOX_OFF)
{
  //bad old code, must be fixed!
  this->enabled = 0;
  this->checked = 0;
  this->colorMod[0] = RGB_White;
  this->colorMod[1] = RGB_White;
  this->colorMod[2] = RGB_White;
  this->colorMod[3] = RGB_White;
  this->toggleOnMouseUp = 0;
  this->enableText = 0;
}

void Checkbox::make(int x, int y, bool checked, int toggleOnMouseUp)
{
  useCustomImages = false;
  useCustomImages2 = false;
  this->x0 = x;
  this->y0 = y;
  this->checked = checked;
  /*this->texture[0] = set->checkBox[0];
  this->texture[1] = set->checkBox[0];
  this->texture[2] = set->checkBox[1];
  this->texture[3] = set->checkBox[1];
  this->colorMod[0] = set->mainColor;
  this->colorMod[1] = set->mouseOverColor;
  this->colorMod[2] = set->mainColor;
  this->colorMod[3] = set->mouseOverColor;*/
  this->setSizeX(/*set->checkBox[0]->getU()*/16); //TODO: get size from IGUIPartDrawer
  this->setSizeY(/*set->checkBox[0]->getV()*/16); //TODO: get size from IGUIPartDrawer
  this->toggleOnMouseUp = toggleOnMouseUp;
  this->setEnabled(true);
  
  positionText();
}

void Checkbox::makeSmall(int x, int y, bool checked, int toggleOnMouseUp)
{
  useCustomImages = false;
  useCustomImages2 = false;
  this->x0 = x;
  this->y0 = y;
  this->checked = checked;
  /*this->texture[0] = set->smallCheckBox[0];
  this->texture[1] = set->smallCheckBox[0];
  this->texture[2] = set->smallCheckBox[1];
  this->texture[3] = set->smallCheckBox[1];*/
  /*this->colorMod[0] = set->mainColor;
  this->colorMod[1] = set->mouseOverColor;
  this->colorMod[2] = set->mainColor;
  this->colorMod[3] = set->mouseOverColor;*/
  this->setSizeX(/*set->smallCheckBox[0]->getU()*/12); //TODO: get size from IGUIPartDrawer
  this->setSizeY(/*set->smallCheckBox[0]->getV()*/12); //TODO: get size from IGUIPartDrawer
  this->toggleOnMouseUp = toggleOnMouseUp;
  this->setEnabled(true);
  
  positionText();
}

void Checkbox::setTexturesAndColors(HTexture* texture1, HTexture* texture2, HTexture* texture3, HTexture* texture4, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4)
{
  useCustomImages = true;
  this->texture[0] = texture1;
  this->texture[1] = texture2;
  this->texture[2] = texture3;
  this->texture[3] = texture4;
  this->colorMod[0] = color1;
  this->colorMod[1] = color2;
  this->colorMod[2] = color3;
  this->colorMod[3] = color4;
  this->setSizeX(texture[0]->texture->getU());
  this->setSizeY(texture[0]->texture->getV());
  
  positionText();
}

void Checkbox::setText(const std::string& newText)
{
  text = newText;
}

void Checkbox::setTexturesAndColors(HTexture* texture1, HTexture* texture2, const ColorRGB& color1, const ColorRGB& color2)
{
  setTexturesAndColors(texture1, texture1, texture2, texture2, color1, color1, color2, color2);
}

void Checkbox::addFrontImage
(
  HTexture* texture1, HTexture* texture2, HTexture* texture3, HTexture* texture4,
  const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3, const ColorRGB& imageColor4
)
{
  useCustomImages2 = true;

  this->texture2[0] = texture1;
  this->texture2[1] = texture2;
  this->texture2[2] = texture3;
  this->texture2[3] = texture4;
  this->colorMod2[0] = imageColor1;
  this->colorMod2[1] = imageColor2;
  this->colorMod2[2] = imageColor3;
  this->colorMod2[3] = imageColor4;
}

void Checkbox::setCustomColor(const ColorRGB& color)
{
  colorMod[0] = color;
  colorMod[1] = color;
  colorMod[2] = color;
  colorMod[3] = color;
}

void Checkbox::setCustomColor2(const ColorRGB& color)
{
  colorMod2[0] = color;
  colorMod2[1] = color;
  colorMod2[2] = color;
  colorMod2[3] = color;
}


void Checkbox::addFrontImage(HTexture* texture)
{
  addFrontImage(texture, texture, texture, texture, colorMod[0], colorMod[1], colorMod[2], colorMod[3]);
}

void Checkbox::addText(const std::string& text, const Font& font)
{
  this->text = text;
  this->font = font;
  this->enableText = 1;
  positionText();
}

//place the text next to the checkbox
void Checkbox::positionText()
{
  textOffsetX = getSizeX() + 2; //need some number of pixels that text is away from the texture, eg 2
  textOffsetY = getSizeY() / 2 - 8 / 2;
}

//toggle "checked" or "unchecked" state
void Checkbox::toggle()
{
  if(checked) uncheck();
  else check();
}

//see how you click with the mouse and toggle on click
void Checkbox::handleImpl(const IInput& input)
{
  //make sure never both pressed() and clicked() are checked, because one test screws up the other, hence the order of the tests in the if conditions
  if(toggleOnMouseUp == 0 && pressed(input)) toggle();
  if(toggleOnMouseUp == 1 && clicked(input)) toggle();
}

//draw the checkbox with a texture depending on it's state
void Checkbox::drawImpl(IGUIDrawer& drawer) const
{
  bool over = mouseOver(drawer.getInput());
  if(useCustomImages)
  {
    size_t index = 0;
    if(!checked && !over) index = 0;
    if(!checked && over) index = 1;
    if(checked && !over) index = 2;
    if(checked && over) index = 3; 
    
    if(texture[index] && texture[index]->texture)
    {
      drawer.convertTextureIfNeeded(texture[index]->texture);
      drawer.drawTexture(texture[index]->texture, x0, y0, colorMod[index]);
    }
  }
  else
  {
    if(checked) drawer.drawGUIPart(partOn, x0, y0, x1, y1);
    else drawer.drawGUIPart(partOff, x0, y0, x1, y1);
  }

  if(useCustomImages2)
  {
    size_t index = 0;
    if(!checked && !over) index = 0;
    if(!checked && over) index = 1;
    if(checked && !over) index = 2;
    if(checked && over) index = 3; 
    
    if(texture2[index] && texture2[index]->texture)
    {
      drawer.convertTextureIfNeeded(texture2[index]->texture);
      drawer.drawTexture(texture2[index]->texture, x0, y0, colorMod2[index]);
    }
  }


  if(enableText)
    drawer.drawText(text, x0 + textOffsetX, y0 + textOffsetY, font);
  
  drawLabel(drawer, this);
}

void Checkbox::setCustomGUIParts(GUIPart part_on, GUIPart part_off)
{
  partOn = part_on;
  partOff = part_off;
}

////////////////////////////////////////////////////////////////////////////////
//GUIBULLETLIST/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



ABulletList::ABulletList()
{
  this->lastChecked = -1;
}

ABulletList::~ABulletList()
{
}

void ABulletList::handleBullets(const IInput& input)
{
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i]->handle(input);
  }

  int newChecked = -1;
  
  for(size_t i = 0; i < bullet.size(); i++)
  {
    if(bullet[i]->isChecked() && int(i) != lastChecked) newChecked = i;
    bullet[i]->setChecked(false);
  }

  if(newChecked >= 0)
  {
    bullet[newChecked]->setChecked(true);
    lastChecked = newChecked;
  }
  else if(lastChecked >= 0 && lastChecked < int(bullet.size()))
    bullet[lastChecked]->setChecked(true);
}

void ABulletList::drawBullets(IGUIDrawer& drawer) const
{
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i]->draw(drawer);
  }
}

int ABulletList::check()
{
  return lastChecked;
}

void ABulletList::addText(const std::string& text, unsigned long i)
{
  if(i < bullet.size()) bullet[i]->addText(text);
}

std::string ABulletList::getText(unsigned long i) const
{
  std::string result;
  if(i < bullet.size()) result = bullet[i]->getText();
  return result;
}

const std::string& ABulletList::getCurrentText() const
{
  return bullet[lastChecked]->getText();
}

void ABulletList::set(unsigned long i)
{
  /*if(i >= 0 && i < bullet.size()) bullet[i].checked = true;
  if(!enabled) lastChecked = i;*/

  for(unsigned long j = 0; j < bullet.size(); j++)
  {
    if(i != j) bullet[j]->setChecked(false);
    else bullet[j]->setChecked(true);
  }
  
  lastChecked = i;
  
  //handle(input);
}

////////////////////////////////////////////////////////////////////////////////

/*
The BulletList

it's a list of checkboxes, where only one can be selected at the time

you first make() the prototype, and make() the bulletlist. Then all checkboxes have the
style you  want!
*/

BulletList::BulletList()
{
  this->enabled = 0;
}

BulletList::~BulletList()
{
  clear();
}

void BulletList::clear()
{
  for(size_t i = 0; i < bullet.size(); i++) delete bullet[i];
  bullet.clear();
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff)
{
  make(x, y, amount, xDiff, yDiff, 1);
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff, unsigned long amountx)
{
  if(amountx < 1) amountx = 1;
  
  this->x0 = x;
  this->y0 = y;
  this->xDiff = xDiff;
  this->yDiff = yDiff;
  this->setEnabled(true);

  clear();
  int xPos = x, yPos = y;
  for(unsigned long i = 0; i < amount; i++)
  {
    xPos = x + xDiff * (i % amountx);
    yPos = y + yDiff * (i / amountx);
    bullet.push_back(new Checkbox);
    bullet.back()->make(0,0,0,0);
    bullet.back()->moveTo(xPos, yPos);
    addSubElement(bullet.back());
  }
  
  setCorrectSize();
}

void BulletList::setCorrectSize()
{
  if(bullet.empty()) return;
  
  int minx = bullet[0]->getX0();
  int miny = bullet[0]->getY0();
  int maxx = bullet[0]->getX1();
  int maxy = bullet[0]->getY1();
  
  for(size_t i = 1; i < bullet.size(); i++)
  {
    minx = std::min(minx, bullet[i]->getX0());
    miny = std::min(miny, bullet[i]->getY0());
    maxx = std::max(maxx, bullet[i]->getX1());
    maxy = std::max(maxy, bullet[i]->getY1());
  }
  
  this->x0 = minx;
  this->y0 = miny;
  this->setSizeX(maxx - minx);
  this->setSizeY(maxy - miny);
}


    
void BulletList::drawImpl(IGUIDrawer& drawer) const
{
  drawBullets(drawer);
}

void BulletList::handleImpl(const IInput& input)
{
  if(!mouseOver(input)) return; //some speed gain, don't do all those loops if the mouse isn't over this widget

  handleBullets(input);
}

const Element* BulletList::hitTest(const IInput& input) const
{
  const Element* result = ic.hitTest(input);
  if(result) return result;
  return Element::hitTest(input);
}

////////////////////////////////////////////////////////////////////////////////
//GUITEXT///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Text is just some text as child of Element
*/

Text::Text()
{
  this->enabled = 0;
  this->useNewLine = true;
}

void Text::make(int x, int y, const std::string& text, const Font& font)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(text.length() * /*markup.getWidth()*/8);
  this->setSizeY(/*markup.getHeight()*/8);
  this->text = text;
  this->font = font;
  this->setEnabled(true);
}

void Text::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawText(text, x0, y0, font);
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

Image::Image() : image(0)
{
  this->enabled = 0;
}

void Image::make(int x, int y, ITexture* image, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(image->getU());
  this->setSizeY(image->getV());
  this->image = image;
  this->colorMod = colorMod;
  this->setEnabled(true);
}

void Image::make(int x, int y, int sizex, int sizey, ITexture* image, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->image = image;
  this->colorMod = colorMod;
  this->setEnabled(true);
}

void Image::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawTextureSized(image, x0, y0, getSizeX(), getSizeY(), colorMod);
}


////////////////////////////////////////////////////////////////////////////////

Tabs::Tab::Tab()
{
}

void Tabs::Tab::drawImpl(IGUIDrawer& drawer) const
{
  (void)drawer; //the tab is drawn by Tabs because only Tabs knows which tab is selected and which not
}

Tabs::Tabs()
: selected_tab(0)
{
}

Tabs::~Tabs()
{
  clear();
}

void Tabs::clear()
{
  for(size_t i = 0; i < tabs.size(); i++)
    delete tabs[i];
  tabs.clear();
  selected_tab = 0;
}

void Tabs::generateTabSizes()
{
  size_t num = tabs.size();
  size_t dx = getSizeX();
  static const int TABHEIGHT = 16;
  
  for(size_t i = 0; i < tabs.size(); i++)
  {
    tabs[i]->resize(x0 + (dx * i) / num, y0, x0 + (dx * (i + 1)) / num, TABHEIGHT);
    //ic.updateRelativeSize(tabs[i], this);
    my_ic.setSticky(tabs[i], Sticky(i * (1.0 / tabs.size()), 0, 0.0, 0, (i + 1) * (1.0 / tabs.size()), 0, 0.0, TABHEIGHT), this);
    tabs[i]->container.resize(x0, TABHEIGHT, x1, y1);
    //ic.updateRelativeSize(&tabs[i]->container, this);
    my_ic.setSticky(&tabs[i]->container, Sticky(0.0, 0, 0.0, TABHEIGHT, 1.0, 0, 1.0, 0), this);
  }
}

void Tabs::addTab(const std::string& name)
{
  tabs.push_back(new Tab);
  tabs.back()->name = name;
  addSubElement(tabs.back());
  addSubElement(&tabs.back()->container);
  generateTabSizes();
  updateActiveContainer();
}

size_t Tabs::getNumTabs() const
{
  return tabs.size();
}

Container& Tabs::getTabContent(size_t index) const
{
  return tabs[index]->container;
}

void Tabs::drawImpl(IGUIDrawer& drawer) const
{
  for(size_t i = 0; i < tabs.size(); i++)
  {
    if(i == selected_tab)
      drawer.drawGUIPart(GP_TAB_SELECTED, tabs[i]->getX0(), tabs[i]->getY0(), tabs[i]->getX1(), tabs[i]->getY1());
    else
      drawer.drawGUIPart(GP_TAB_UNSELECTED, tabs[i]->getX0(), tabs[i]->getY0(), tabs[i]->getX1(), tabs[i]->getY1());
    drawer.drawText(tabs[i]->name, tabs[i]->getCenterX(), tabs[i]->getCenterY(), FONT_Black, TextAlign(HA_CENTER, VA_CENTER));
    
    tabs[i]->container.draw(drawer);
  }
}

void Tabs::manageHoverImpl(IHoverManager& hover)
{
  ElementComposite::manageHoverImpl(hover);
  for(size_t i = 0; i < tabs.size(); i++)
  {
    tabs[i]->container.manageHover(hover);
  }
}

void Tabs::updateActiveContainer()
{
  for(size_t i = 0; i < tabs.size(); i++)
  {
    if(i == selected_tab)
    {
      tabs[i]->container.setEnabled(true);
    }
    else
    {
      tabs[i]->container.setEnabled(false);
    }
  }
}

void Tabs::selectTab(size_t i_index)
{
  selected_tab = i_index;
  updateActiveContainer();
}

void Tabs::handleImpl(const IInput& input)
{
  for(size_t i = 0; i < tabs.size(); i++)
  {
    if(i != selected_tab)
      tabs[i]->container.setElementOver(true);
    if(tabs[i]->pressed(input))
    {
      selectTab(i);
    }
  }
  
  for(size_t i = 0; i < tabs.size(); i++)
  {
    tabs[i]->container.handle(input);
  }
}

////////////////////////////////////////////////////////////////////////////////

bool Dialog::done() const
{
  return !isEnabled();
}

lpi::gui::Dialog::Result Dialog::getResult() const
{
  return result;
}

} //namespace gui
} //namespace lpi

