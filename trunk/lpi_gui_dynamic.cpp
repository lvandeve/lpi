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

void DynamicPage::addControl(const std::string& name, IDynamicPageControl* control)
{
  controls.push_back(control);
  control_names.push_back(name);
  
  size_t i = controls.size() - 1;
  int xb = (int)(getSizeX() * title_width);
  int h = control->getSizeY();
  control->resize(x0 + xb + 1, y0 + i * CONTROLHEIGHT + 1, x1, y0 + (i + 1) * CONTROLHEIGHT);
  addSubElement(control, STICKYRELATIVETOP);
}

void DynamicPage::drawWidget(IGUIDrawer& drawer) const
{
  for(size_t i = 0; i < controls.size(); i++)
  {
    int y = CONTROLHEIGHT * i;
    int xb = (int)(getSizeX() * title_width); //"x-between": the divider between title and value
    
    drawer.drawRectangle(x0, y0 + y, x0 + xb, y0 + y + CONTROLHEIGHT, RGB_White, false);
    drawer.drawRectangle(x0 + xb, y0 + y, x1, y0 + y + CONTROLHEIGHT, RGB_White, false);
    drawer.drawText(control_names[i], x0 + 4, y0 + y + 4);
  
    controls[i]->draw(drawer);
  }
}

void DynamicPage::handleWidget(const IGUIInput& input)
{
  for(size_t i = 0; i < controls.size(); i++)
    controls[i]->handle(input);
  
  setSizeY(controls.size() * CONTROLHEIGHT);
}

} //namespace gui
} //namespace lpi

