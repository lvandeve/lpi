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

size_t InternalContainer::findElementIndex(Element* element)
{
  for(size_t i = 0; i < elements.size(); i++) if(elements[i] == element) return i;
  return elements.size();
}

void InternalContainer::removeElement(Element* element)
{
  elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
  sticky.erase(sticky.find(element));
}

Element* InternalContainer::hitTest(const IInput& input)
{
  for(size_t j = 0; j < elements.size(); j++)
  {
    size_t i = elements.size() - j - 1; //invert order, the last elements are on top
    if(elements[i]->mouseOver(input)) return elements[i]->hitTest(input);
  }
  return 0;
}

void InternalContainer::manageHover(IHoverManager& hover)
{
  for(size_t j = 0; j < elements.size(); j++)
  {
    elements[j]->manageHover(hover);
  }
}

////////////////////////////////////////////////////////////////////////////////

Element* MainContainer::hitTest(const IInput& input)
{
  if(mouseOver(input))
  {
    Element* result = c.hitTest(input);
    if(result) return result;
    return this;
  }
  else return 0;
}

////////////////////////////////////////////////////////////////////////////////
//TOOLTIPMANAGER                                                              //
////////////////////////////////////////////////////////////////////////////////

void ToolTipManager::registerElement(Element* element, const std::string& tip)
{
  elements[element] = tip;
}

void ToolTipManager::draw(Element* root, IGUIDrawer& drawer) const
{
  Element* element = root->hitTest(drawer.getInput());
  if(!element) return;
  std::map<Element*, std::string>::const_iterator it = elements.find(element);
  if(it != elements.end())
  {
    drawer.drawGUIPartText(GPT_TOOLTIP, it->second, drawer.getInput().mouseX(), drawer.getInput().mouseY(), drawer.getInput().mouseX(), drawer.getInput().mouseY());
  }
  else element->drawToolTip(drawer);
  
  /*
  TODO! think whether hitTest should only return container elements, or also
  internal-container elements from ElementComposite, and whether or not these
  may be publically available like that.
  */
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
                   , minSizeX(0)
                   , minSizeY(0)
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

Element* Element::hitTest(const IInput& input)
{
  if(mouseOver(input)) return this;
  else return 0;
}

bool Element::isFloating() const
{
  return false;
}

void Element::resize(int x0, int y0, int x1, int y1)
{
  
  if(x1 - x0 < minSizeX) x1 = x0 + minSizeX;
  if(y1 - y0 < minSizeY) y1 = y0 + minSizeY;

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

void ElementComposite::addSubElement(Element* element, const Sticky& sticky)
{
  ic.addSubElement(element, sticky, this);
}

void ElementComposite::move(int x, int y)
{
  ic.move(x, y);
  
  Element::move(x, y);
}

void ElementComposite::setElementOver(bool state)
{
  Element::setElementOver(state);
  
  ic.setElementOver(state);
}

void ElementComposite::resize(int x0, int y0, int x1, int y1)
{
  Pos<int> oldPos = { this->x0, this->y0, this->x1, this->y1 };
  
  Element::resize(x0, y0, x1, y1);
  
  Pos<int> newPos = { this->x0, this->y0, this->x1, this->y1 };
  
  ic.resize(oldPos, newPos);
}

void ElementComposite::manageHoverImpl(IHoverManager& hover)
{
  ic.manageHover(hover);
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

/*
Contains many elements in it and draws them and handles them, and even makes
sure you can't press the mouse "through" elements on top of other elements.
*/

Container::Container()
{
  setEnabled(true);
  x0 = 0;
  y0 = 0;
  x1 = 1;
  y1 = 1;
}

Container::Container(IGUIDrawer& drawer)
{
  setEnabled(true);
  
  //the default container is as big as the screen (note: don't forget to resize it if you resize the resolution of the screen!)
  x0 = 0;
  y0 = 0;
  x1 = drawer.getWidth();
  y1 = drawer.getHeight();
}

void Container::make(int x, int y, int sizex, int sizey)
{
  //ic.getElements().clear();
  
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
}

void Container::setElementOver(bool state)
{
  Element::setElementOver(state);
  elements.setElementOver(state);
}

Element* Container::hitTest(const IInput& input)
{
  if(mouseOver(input))
  {
    Element* result = elements.hitTest(input);
    if(result) return result;
    return this;
  }
  else return 0;
}

void Container::handleImpl(const IInput& input)
{
  if(mouseOver(input))
  {
    int topElement = -1;
    
    //priority to mouseGrabbed over mouseOver
    for(int i = size() - 1; i >= 0; i--)
    {
      elements.getElement(i)->setElementOver(false);
      bool grabbed = elements.getElement(i)->getMouseStateForContainer().mouseGrabbed(elements.getElement(i)->mouseOver(input)
                                                                                    , input.mouseButtonDown(LMB)
                                                                                    , input.mouseX()
                                                                                    , input.mouseY()
                                                                                    , elements.getElement(i)->mouseGetRelPosX(input)
                                                                                    , elements.getElement(i)->mouseGetRelPosY(input));
      if(grabbed)
      {
        if(topElement < 0) topElement = i;
        //break;
      }
      elements.getElement(i)->setElementOver(true);
    }
    
    //only now test mouseOver
    if(topElement == -1)
    for(int i = size() - 1; i >= 0; i--)
    {
      elements.getElement(i)->setElementOver(false);
      if(elements.getElement(i)->mouseOver(input))
      {
        topElement = i;
        break;
      }
      elements.getElement(i)->setElementOver(true);
    }

    //make all elements unresponsive to the mouse by setting "elementover", except the topElement
    for(size_t i = 0; i < size(); i++) elements.getElement(i)->setElementOver(true);
    if(topElement >= 0 && topElement < (int)size())
    {
      elements.getElement(topElement)->setElementOver(false);
      if(elements.getElement(topElement)->isFloating()
      && elements.getElement(topElement)->getMouseStateForContainer().mouseDownHere(elements.getElement(topElement)->mouseOver(input), input.mouseButtonDown(LMB)))
        bringToTop(elements.getElement(topElement));
    }
  }
  else for(size_t i = 0; i < size(); i++) elements.getElement(i)->setElementOver(true);

  for(unsigned long i = 0; i < size(); i++)
  {
    elements.getElement(i)->handle(input);
  }
}

void Container::drawElements(IGUIDrawer& drawer) const
{
  for(unsigned long i = 0; i < size(); i++)
  {
    elements.getElement(i)->draw(drawer);
  }
}

void Container::drawImpl(IGUIDrawer& drawer) const
{
  drawer.pushSmallestScissor(x0, y0, x1, y1);
  drawElements(drawer);
  drawer.popScissor();
}

void Container::manageHoverImpl(IHoverManager& hover)
{
  elements.manageHover(hover);
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
  elements.addSubElement(element, sticky, this);
}

void Container::pushBottomAt(Element* element, int x, int y, const Sticky& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements.insertSubElement(0, element, sticky, this);
}

void Container::insertAt(size_t pos, Element* element, int x, int y, const Sticky& sticky)
{
  remove(element); //prevent duplicates
  element->moveTo(x0 + x, y0 + y);
  elements.insertSubElement(pos, element, sticky, this);
}

void Container::centerElement(Element* element)
{
  element->moveCenterTo(getCenterX(), getCenterY());
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
    if(elements.getElement(i)->getSizeX() > getSizeX()) newx = elements.getElement(i)->getX0();
    if(elements.getElement(i)->getSizeY() > getSizeY()) newy = elements.getElement(i)->getY0();
    
    elements.getElement(i)->moveTo(newx, newy);
  }
}

void Container::moveImpl(int x, int y)
{
  elements.move(x, y);
}

void Container::getRelativeElementPos(Element& element, int& ex, int& ey) const
{
  ex = element.getX0() - x0;
  ey = element.getY0() - y0;
}

void Container::resizeImpl(const Pos<int>& newPos)
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

void ScrollElement::drawImpl(IGUIDrawer& drawer) const
{
  drawer.pushSmallestScissor(getVisibleX0(), getVisibleY0(), getVisibleX1(), getVisibleY1()); //TODO: currently does same as pushScissor (because otherwise there's weird bug, to reproduce: resize the red window and look at smiley in the small grey window), so elements from container in container are still drawn outside container. DEBUG THIS ASAP!!!
  element->draw(drawer);
  drawer.popScissor();
  
  bars.draw(drawer);
}

void ScrollElement::manageHoverImpl(IHoverManager& hover)
{
  ElementComposite::manageHoverImpl(hover);
  element->manageHover(hover);
}

Element* ScrollElement::hitTest(const IInput& input)
{
  if(mouseOver(input))
  {
    Element* result = element->hitTest(input);
    if(result) return result;
    return this;
  }
  else return 0;
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
    if(mouseOver(input))
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
    int scrollx = x0 - int(bars.hbar.scrollPos); //the scrollpos is 0 if a bar is not enabled
    int scrolly = y0 - int(bars.vbar.scrollPos);
    
    //if bars are not enabled (if their size is 0), the position of the element should not be affected except that it should stay inside the scroll area
    if(bars.hbar.scrollSize == 0)
    {
      scrollx = element->getX0();
      if(scrollx + element->getSizeX() > x1) scrollx = x1 - element->getSizeX();
      if(scrollx < x0) scrollx = x0;
    }
    if(bars.vbar.scrollSize == 0)
    {
      scrolly = element->getY0();
      if(scrolly + element->getSizeY() > y1) scrolly = y1 - element->getSizeY();
      if(scrolly < y0) scrolly = y0;
    }
    
    moveAreaTo(scrollx, scrolly);
  }
  
  updateBars();
}

void ScrollElement::setElementOver(bool state)
{
  ElementComposite::setElementOver(state);
  element->setElementOver(state);
}

ScrollElement::ScrollElement()
{
  element = 0;
}

void ScrollElement::make(int x, int y, int sizex, int sizey, Element* element, const IGUIPartGeom& geom)
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

void ScrollElement::moveAreaTo(int x, int y)
{
  if(element) element->moveTo(x, y);
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
  
  toggleBars();

  bars.hbar.scrollSize = element->getSizeX() - getVisibleSizeX();
  bars.vbar.scrollSize = element->getSizeY() - getVisibleSizeY();
  bars.vbar.scrollPos = y0 - element->getY0();
  bars.hbar.scrollPos = x0 - element->getX0();
  if(bars.hbar.scrollSize < 0) bars.hbar.scrollSize = 0;
  if(bars.vbar.scrollSize < 0) bars.vbar.scrollSize = 0;
  if(bars.hbar.scrollPos < 0) bars.hbar.scrollPos = 0;
  if(bars.vbar.scrollPos < 0) bars.vbar.scrollPos = 0;
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
    if(elements.getElement(i)->isInside(x, y)) return true;
  }
  
  return false;
}

void Group::drawImpl(IGUIDrawer& drawer) const
{
  drawElements(drawer);
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
: enableResizer(false)
, closeEnabled(false)
, enableTop(false)
, closed(false)
, resizerOverContainer(false)
{
  enabled = false;

  addSubElement(&top, Sticky(0.0, 0, 0.0, 0, 1.0, 0, 0.0, top.getSizeY()));
  addSubElement(&resizer, Sticky(1.0, -resizer.getSizeX(), 1.0, -resizer.getSizeY(), 1.0, 0, 1.0, 0));
  addSubElement(&container, Sticky(0.0, 0, 0.0, 0, 1.0, 0, 1.0, 0));
  addSubElement(&closeButton, Sticky(1.0, -closeButton.getSizeX(), 0.0, 0, 1.0, 0, 0.0, closeButton.getSizeY()));
  
  minSizeX = 64;
  minSizeY = 64;
}


Element* Window::hitTest(const IInput& input)
{
  if(mouseOver(input))
  {
    Element* result = container.hitTest(input);
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

void Window::addCloseButton(const IGUIPartGeom& geom)
{
  int closeX = x1 - geom.getGUIPartSizeX(GP_WINDOW_CLOSE); //TODO: give IGUIPartDrawer functions to get hte width and height of some GUI components like this one and use that here.
  int closeY = y0;
  closeButton.resize(closeX, closeY, closeX + geom.getGUIPartSizeX(GP_WINDOW_CLOSE), closeY + geom.getGUIPartSizeY(GP_WINDOW_CLOSE));
  
  closed = 0;
  closeEnabled = 1;
  
  ic.setSticky(&closeButton, Sticky(1.0, -closeButton.getSizeX(), 0.0, 0, 1.0, 0, 0.0, closeButton.getSizeY()), this);
}

void Window::addResizer(const IGUIPartGeom& geom, bool overContainer)
{
  int resizerX = x1 - geom.getGUIPartSizeX(GP_WINDOW_RESIZER);
  int resizerY = y1 - geom.getGUIPartSizeY(GP_WINDOW_RESIZER);
  resizer.resize(resizerX, resizerY, resizerX + geom.getGUIPartSizeX(GP_WINDOW_RESIZER), resizerY + geom.getGUIPartSizeY(GP_WINDOW_RESIZER));
  enableResizer = true;
  ic.setSticky(&resizer, Sticky(1.0, -resizer.getSizeX(), 1.0, -resizer.getSizeY(), 1.0, 0, 1.0, 0), this);
  
  if(!overContainer)
  {
    container.resize(container.getX0(), container.getY0(), container.getX1(), container.getY1() - (y1 - resizer.getY0()));
    ic.setStickyFull(&container, this);
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
  
  ic.setStickyFull(&container, this);
}

bool Window::isFloating() const
{
  return true;
}

void Window::makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->setEnabled(true);
  this->enableTop = 0;
  
  colorMod = fillColor;
  
  initContainer();
}

void Window::makeTextured(int x, int y, int sizex, int sizey,
       const ITexture* /*t00*/, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->setEnabled(true);
  
  this->enableTop = 0;
  
  this->colorMod = colorMod;
  
  initContainer();
}

void Window::make(int x, int y, int sizex, int sizey)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->setEnabled(true);
  
  this->enableTop = 0;
  
  initContainer();
}

void Window::addTop(const IGUIPartGeom& geom)
{
  top.resize(x0, y0, x1, y0 + geom.getGUIPartSizeY(GP_WINDOW_TOP));
  this->enableTop = 1;
  ic.setSticky(&top, Sticky(0.0, 0, 0.0, 0, 1.0, 0, 0.0, top.getSizeY()), this);
  
  container.resize(container.getX0(), container.getY0() + (top.getY1() - y0), container.getX1(), container.getY1());
  ic.setStickyFull(&container, this);
}


void Window::initContainer()
{
  container.make(x0, y0, getSizeX(), getSizeY());
  ic.setStickyFull(&container, this);
}

//to let the scrollbars work properly, call this AFTER using setContainerBorders, addTop, addResizer and such of the window
void Window::addScrollbars(const IGUIPartGeom& geom)
{
  if(scroll.element) return;
  
  scroll.make(container.getX0(), container.getY0(), container.getSizeX(), container.getSizeY(), &container, geom);
  container.setSizeToElements();
  ic.removeElement(&container); //the scrollbars must control the container now
  addSubElement(&scroll);
  ic.setStickyFull(&scroll, this);
}

void Window::removeScrollbars()
{
  if(!scroll.element) return;
  
  scroll.element = 0;
  ic.removeElement(&scroll);
  addSubElement(&container);
  container.resize(scroll.getX0(), scroll.getY0(), scroll.getX1(), scroll.getY1());
  ic.setStickyFull(&container, this);
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
  //the close button
  if(closeEnabled && closeButton.clicked(input)) closed = 1;
  
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

void Window::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawGUIPartColor(GPC_WINDOW_PANEL, colorMod, x0, y0, x1, y1);
  if(enableTop)
  {
    drawer.drawGUIPart(GP_WINDOW_TOP, top.getX0(), top.getY0(), top.getX1(), top.getY1()); //draw top bar before the elements, or it'll appear above windows relative to the current window
    drawer.drawGUIPartText(GPT_WINDOW_TITLE, title, top.getX0(), top.getY0(), top.getX1(), top.getY1());
  }
  
  if(closeEnabled) drawGUIPart(drawer, &closeButton, GP_WINDOW_CLOSE);

  if(scroll.element) scroll.draw(drawer);
  else container.draw(drawer);

  if(enableResizer) drawer.drawGUIPart(GP_WINDOW_RESIZER, resizer.getX0(), resizer.getY0(), resizer.getX1(), resizer.getY1()); //draw this after the container so the resizer is drawn over scrollbars if that is needed
}

void Window::manageHoverImpl(IHoverManager& hover)
{
  ElementComposite::manageHoverImpl(hover);
  if(scroll.element) scroll.manageHover(hover);
  else container.manageHover(hover);
}

void Window::addTitle(const std::string& title, int titleX, int titleY, const Font& titleFont)
{
  this->title = title;
  this->titleX = titleX;
  this->titleY = titleY;
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
  const ITexture* texture1, const ITexture* texture2, const ITexture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3
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

void Button::addFrontImage(const ITexture* texture)
{
  addFrontImage(texture, texture, texture, imageColor[0], imageColor[1], imageColor[2]);
}

//Constructor to make a button with only images, no text
void Button::makeImage
(
  int x, int y, //basic properties
  const ITexture* texture1, const ITexture* texture2, const ITexture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3 //image
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(texture1->getU());
  this->setSizeY(texture1->getV());
  
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
  
  this->setEnabled(true);
  
  this->mouseDownVisualStyle = 0;
}

//Constructor to make a button with only text, no images
void Button::makeText
(
  int x, int y, //basic properties
  const std::string& text, //text
  ITextDrawer& drawer
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
  if(enableImage) drawer.drawTexture(image[style], x0 + imageOffsetx, y0 + imageOffsety, imageColor[style]);
  if(enableImage2) drawer.drawTexture(image2[style], x0 + imageOffsetx2, y0 + imageOffsety2, imageColor2[style]);
  if(enableText && !enablePanel) drawGUIPartText(drawer, this, GPT_TEXT_BUTTON_TEXT, text);
  //TODO: write all button texts using the guipartdrawer
  else drawGUIPartText(drawer, this, GPT_PANEL_BUTTON_TEXT, text);
  
}

/*This function automaticly changes sizex and sizey of the button so that the
size of the button matches the size of the text, so that it detects the mouse
as being in the button when the mouse is over the text*/
void Button::autoTextSize(ITextDrawer* drawer, int extrasize)
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

void Button::makeImage(int x, int y,  const ITexture* texture123, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3)
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

void Scrollbar::init(const IGUIPartGeom& geom)
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
    ic.setSticky(&buttonDown, Sticky(0.0, 0, 1.0, -buttonDown.getSizeY(), 1.0, 0, 1.0, 0), this);
  }
  else
  {
    buttonUp.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_W), geom.getGUIPartSizeY(GP_SCROLLBAR_W));
    buttonUp.moveTo(x0, y0);
    buttonDown.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_E), geom.getGUIPartSizeY(GP_SCROLLBAR_E));
    buttonDown.moveTo(x1 - geom.getGUIPartSizeX(GP_SCROLLBAR_E), y0);
    scroller.resize(0, 0, geom.getGUIPartSizeX(GP_SCROLLBAR_SCROLLER), geom.getGUIPartSizeY(GP_SCROLLBAR_SCROLLER));
    scroller.moveTo(int(x0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), y0);
    ic.setSticky(&buttonDown, Sticky(1.0, -buttonDown.getSizeX(), 0.0, 0, 1.0, 0, 1.0, 0), this);
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
                double scrollSize, const IGUIPartGeom& geom,
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
                  double scrollSize, const IGUIPartGeom& geom,
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
  int scrollDir = 0;

  if(buttonUp.mouseDownHere(input)) scrollDir = -1;
  if(buttonDown.mouseDownHere(input)) scrollDir = 1;

  if(direction == V)
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed(input);
    bool buttonDown_grabbed = buttonDown.mouseGrabbed(input);
    if(scroller.mouseGrabbed(input))
      scrollPos = (scrollSize * (input.mouseY() - y0 - getSliderStart() - scroller.getSizeY() / 2)) / getSliderSize();
    else if(mouseDownHere(input) && !scroller.mouseGrabbed(input) && !buttonUp_grabbed && !buttonDown_grabbed)
    {
      scrollPos = (scrollSize * (input.mouseY() - y0 - getSliderStart() - scroller.getSizeY() / 2)) / getSliderSize();
      scroller.mouseGrab(input);
    }
    if(mouseScrollUp(input)) scrollDir = -3;
    if(mouseScrollDown(input)) scrollDir = 3;
    if(forwardedMouseScrollUp()) scrollDir = -3;
    if(forwardedMouseScrollDown()) scrollDir = 3;
  }
  else
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed(input);
    bool buttonDown_grabbed = buttonDown.mouseGrabbed(input);
    if(scroller.mouseGrabbed(input))
      scrollPos = (scrollSize * (input.mouseX() - x0 - getSliderStart() - scroller.getSizeX() / 2)) / getSliderSize();
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

  if(scrollPos < 0) scrollPos = 0;
  if(scrollPos > scrollSize) scrollPos = scrollSize;
  
  if(speedMode == 1) setRelativeScrollSpeed();
  
  if(direction == V) scroller.moveTo(scroller.getX0(), int(y0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize));
  else scroller.moveTo(int(x0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), scroller.getY0());
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

  if(scrollSize > 0) drawer.drawGUIPart(GP_SCROLLBAR_SCROLLER, scroller.getX0(), scroller.getY0(), scroller.getX1(), scroller.getY1());
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

void ScrollbarPair::make(int x, int y, int sizex, int sizey, double scrollSizeH, double scrollSizeV, const IGUIPartGeom& geom)
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

  ic.setSticky(&vbar, STICKYOFF/*Sticky(1.0, -vbar.getSizeX(), 0.0, 0, 1.0, 0, 1.0, 0)*/, this);
  ic.setSticky(&hbar, STICKYOFF/*Sticky(0.0,  0, 1.0, -hbar.getSizeY(), 1.0, 0, 1.0, 0)*/, this);
  this->venabled = true;
  this->henabled = true;
}

void ScrollbarPair::resizeImpl(const Pos<int>& newPos)
{
  if(henabled && venabled)
  {
    vbar.resize(newPos.x1 - vbar.getSizeX(), newPos.y0, newPos.x1, newPos.y1 - hbar.getSizeY());
    hbar.resize(newPos.x0, newPos.y1 - hbar.getSizeY(), newPos.x1 - vbar.getSizeX(), newPos.y1);
  }
  else if(venabled)
  {
    vbar.resize(newPos.x1 - vbar.getSizeX(), newPos.y0, newPos.x1, newPos.y1 - hbar.getSizeY() * conserveCorner);
  }
  else if(henabled)
  {
    hbar.resize(newPos.x0, newPos.y1 - hbar.getSizeY(), newPos.x1 - vbar.getSizeX() * conserveCorner, newPos.y1);
  }
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

void Slider::makeSmallHorizontal(int x, int y, int length, double scrollSize, const IGUIPartGeom& geom)
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

void Slider::makeHorizontal(int x, int y, int length, double scrollSize, const IGUIPartGeom& geom)
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

void Slider::makeVertical(int x, int y, int length, double scrollSize, const IGUIPartGeom& geom)
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
{
  this->enableImage2 = 0;
  //bad old code, must be fixed!
  this->enabled = 0;
  this->checked = 0;
  this->texture[0] = emptyTexture;
  this->texture[1] = emptyTexture;
  this->texture[2] = emptyTexture;
  this->texture[3] = emptyTexture;
  this->colorMod[0] = RGB_White;
  this->colorMod[1] = RGB_White;
  this->colorMod[2] = RGB_White;
  this->colorMod[3] = RGB_White;
  this->toggleOnMouseUp = 0;
  this->enableText = 0;
}

void Checkbox::addFrontImage
(
  const ITexture* texture1, const ITexture* texture2, const ITexture* texture3, const ITexture* texture4, 
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

void Checkbox::addFrontImage(const ITexture* texture)
{
  addFrontImage(texture, texture, texture, texture, colorMod[0], colorMod[1], colorMod[2], colorMod[3]);
}

void Checkbox::make(int x, int y, bool checked, int toggleOnMouseUp)
{
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

void Checkbox::setTexturesAndColors(const ITexture* texture1, const ITexture* texture2, const ITexture* texture3, const ITexture* texture4, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4)
{
  this->texture[0] = texture1;
  this->texture[1] = texture2;
  this->texture[2] = texture3;
  this->texture[3] = texture4;
  this->colorMod[0] = color1;
  this->colorMod[1] = color2;
  this->colorMod[2] = color3;
  this->colorMod[3] = color4;
  this->setSizeX(texture[0]->getU());
  this->setSizeY(texture[0]->getV());
  
  positionText();
}

void Checkbox::setTexturesAndColors(const ITexture* texture1, const ITexture* texture2, const ColorRGB& color1, const ColorRGB& color2)
{
  setTexturesAndColors(texture1, texture1, texture2, texture2, color1, color1, color2, color2);
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
  if(checked) drawer.drawGUIPart(GP_CHECKBOX_ON, x0, y0, x1, y1);
  else drawer.drawGUIPart(GP_CHECKBOX_OFF, x0, y0, x1, y1);
  
  if(enableText)
    drawer.drawText(text, x0 + textOffsetX, y0 + textOffsetY, font);
  
  drawLabel(drawer, this);
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
  this->enabled = 0;
  this->lastChecked = -1;
  
  this->prototype.make(0, 0, 0, 1);
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff)
{
  this->prototype.make(0, 0, 0, 1);
  
  this->x0 = x;
  this->y0 = y;
  this->xDiff = xDiff;
  this->yDiff = yDiff;
  this->setEnabled(true);

  bullet.clear();
  for(unsigned long i = 0; i < amount; i++)
  {
    bullet.push_back(prototype);
    bullet[i].moveTo(x + xDiff * i, y + yDiff * i);
  }
  
  for(unsigned long i = 0; i < amount; i++)
    addSubElement(&bullet[i]); //TODO: make bullet a vector of pointers to Bullet, now it copies elements, and gives wrong pointers if you add an element.
  
  //NOTE: DIT IS NIET CORRECT, DEZE FORMULES
  this->setSizeX(amount * xDiff + prototype.getSizeX());
  this->setSizeY(amount * yDiff + prototype.getSizeY());
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff, unsigned long amountx)
{
  if(amountx < 1) amountx = 1;
  this->prototype.make(0, 0, 0, 1);
  //todo: use guidarwer to draw this
  //this->prototype.setTexturesAndColors(set->bullet[0], set->bullet[1], set->mainColor, set->mainColor);
  
  this->x0 = x;
  this->y0 = y;
  this->xDiff = xDiff;
  this->yDiff = yDiff;
  this->setEnabled(true);

  bullet.clear();
  int xPos = x, yPos = y;
  for(unsigned long i = 0; i < amount; i++)
  {
    xPos = x + xDiff * (i % amountx);
    yPos = y + yDiff * (i / amountx);
    bullet.push_back(prototype);
    bullet[i].moveTo(xPos, yPos);
  }
  
  for(unsigned long i = 0; i < amount; i++)
    addSubElement(&bullet[i]); //TODO: make bullet a vector of pointers to Bullet, now it copies elements, and gives wrong pointers if you add an element.
  
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
  this->setSizeX(maxx - minx);
  this->setSizeY(maxy - miny);
}

void BulletList::handleImpl(const IInput& input)
{
  if(!mouseOver(input)) return; //some speed gain, don't do all those loops if the mouse isn't over this widget
  
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i].handle(input);
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

void BulletList::drawImpl(IGUIDrawer& drawer) const
{
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i].draw(drawer);
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
  if(!enabled) lastChecked = i;*/

  for(unsigned long j = 0; j < bullet.size(); j++)
  {
    if(i != j) bullet[j].checked = false;
    else bullet[j].checked = true;
  }
  
  lastChecked = i;
  
  //handle(input);
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
    ic.setSticky(tabs[i], Sticky(i * (1.0 / tabs.size()), 0, 0.0, 0, (i + 1) * (1.0 / tabs.size()), 0, 0.0, TABHEIGHT), this);
    tabs[i]->container.resize(x0, TABHEIGHT, x1, y1);
    //ic.updateRelativeSize(&tabs[i]->container, this);
    ic.setSticky(&tabs[i]->container, Sticky(0.0, 0, 0.0, TABHEIGHT, 1.0, 0, 1.0, 0), this);
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


} //namespace gui
} //namespace lpi

