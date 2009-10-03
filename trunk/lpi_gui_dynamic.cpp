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

#include "lpi_gui_dynamic.h"

#include <iostream>

namespace lpi
{
namespace gui
{

void DynamicColor::ctor()
{
  this->resize(0, 0, 20, 20);
  box.resize(0, 0, 12, 12);
  box.move(1, 1);
  this->addSubElement(&box, Sticky(0.0, 0, 0.5, -box.getSizeY() / 2, 0.0, box.getSizeX(), 0.5, box.getSizeY() / 2));
  edit.resize(0, 20, 256, 128);
  edit.setEnabled(false);
}

DynamicColor::DynamicColor(ColorRGB* value)
: box(value)
{
  this->bind = value;
  ctor();
}

void DynamicColor::getValue(ColorRGB* value)
{
  (void)value;
}

void DynamicColor::setValue(ColorRGB* value)
{
  (void)value;
}

void DynamicColor::handleImpl(const IInput& input)
{
  box.handle(input);
  
  if(this->clicked(input))
  {
    edit.setEnabled(true);
    edit.moveTo(input.mouseX(), input.mouseY());
  }
  else if(edit.isEnabled() && edit.mouseJustDownElsewhere(input))
  {
    edit.setEnabled(false);
  }
}

void DynamicColor::drawImpl(IGUIDrawer& drawer) const
{
  box.draw(drawer);
}

void DynamicColor::manageHoverImpl(IHoverManager& hover)
{
  if(edit.isEnabled())
    hover.addHoverElement(&edit);
}
////////////////////////////////////////////////////////////////////////////////

DynamicPage::DynamicPage()
: title_width(0.5)
{
  resize(0, 0, 200, 200); //if not resized to something at begin, then added rows go wrong... BUG! :(
}

DynamicPage::~DynamicPage()
{
  for(size_t i = 0; i < controls.size(); i++)
    delete controls[i];
}


void DynamicPage::controlToValue()
{
  for(size_t i = 0; i < controls.size(); i++)
    controls[i]->controlToValue();
}

void DynamicPage::valueToControl()
{
  for(size_t i = 0; i < controls.size(); i++)
    controls[i]->valueToControl();
}

void DynamicPage::addControl(const std::string& name, IDynamicControl* control)
{
  controls.push_back(control);
  control_names.push_back(name);
  
  size_t i = controls.size() - 1;
  int xb = (int)(getSizeX() * title_width);
  control->resize(x0 + xb + 1, y0 + TITLEHEIGHT + i * CONTROLHEIGHT + 1, x1, y0 + TITLEHEIGHT + (i + 1) * CONTROLHEIGHT);
  addSubElement(control, Sticky(0.0, x0 + xb + 1, 0.0, TITLEHEIGHT + i * CONTROLHEIGHT + 1, 1.0, 0, 0.0, TITLEHEIGHT + (i + 1) * CONTROLHEIGHT));
}

void DynamicPage::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawRectangle(x0, y0, x1, y0 + TITLEHEIGHT, RGB_White, false);
  drawer.drawText(title, x0 + 4, y0 + 4);
  
  for(size_t i = 0; i < controls.size(); i++)
  {
    int y = TITLEHEIGHT + CONTROLHEIGHT * i;
    int xb = (int)(getSizeX() * title_width); //"x-between": the divider between title and value
    
    drawer.drawRectangle(x0, y0 + y, x0 + xb, y0 + y + CONTROLHEIGHT, RGB_White, false);
    drawer.drawRectangle(x0 + xb, y0 + y, x1, y0 + y + CONTROLHEIGHT, RGB_White, false);
    drawer.drawText(control_names[i], x0 + 4, y0 + y + 4);
  
    controls[i]->draw(drawer);
  }
}

void DynamicPage::manageHoverImpl(IHoverManager& hover)
{
  for(size_t i = 0; i < controls.size(); i++)
  {
    controls[i]->manageHover(hover);
  }
}


void DynamicPage::handleImpl(const IInput& input)
{
  for(size_t i = 0; i < controls.size(); i++)
    controls[i]->handle(input);
  
  setSizeY(TITLEHEIGHT + controls.size() * CONTROLHEIGHT);
}

void DynamicPage::setTitle(const std::string& title)
{
  this->title = title;
}

} //namespace gui
} //namespace lpi

