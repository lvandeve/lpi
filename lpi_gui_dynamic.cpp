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
  this->addSubElement(&box, Sticky(0.0,0, 0.5,-box.getSizeY() / 2, 0.0,box.getSizeX(), 0.5,box.getSizeY() / 2));
  edit.setEnabled(false);
}

DynamicColor::DynamicColor(ColorRGB* value, const IGUIDrawer& geom)
: box(value)
, edit(geom)
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
    edit.setColor(RGBtoRGBd(*bind));
    edit.moveTo(input.mouseX(), input.mouseY());
  }
  else if(edit.isEnabled() && edit.mouseJustDownElsewhere(input))
  {
    edit.setEnabled(false);
  }
  else if(edit.pressedOk(input)) edit.setEnabled(false);
  
  if(edit.isEnabled())
  {
    edit.getColor255(*bind);
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

void DynamicFile::ctor()
{
  this->resize(0, 0, 20, 20);
  pick.resize(0, 0, 12, 12);
  this->addSubElement(&pick, Sticky(1.0,-pick.getSizeX(), 0.5,-pick.getSizeY() / 2, 1.0,0, 0.5,pick.getSizeY() / 2));
  edit.setEnabled(false);
  line.make(this->x0, this->y0, 256);
  line.move(0, (this->getSizeY() - line.getSizeY()) / 2);
  this->addSubElement(&line, Sticky(0.0,0, 0.5,-line.getSizeY() / 2, 1.0,0, 0.5,line.getSizeY() / 2));
}

DynamicFile::DynamicFile(std::string* value, const IGUIDrawer& geom, IFileBrowse* browser)
: edit(geom, browser, false, false)
{
  this->bind = value;
  ctor();
}

void DynamicFile::getValue(std::string* value)
{
  *value = line.getText();
}

void DynamicFile::setValue(std::string* value)
{
  line.setText(*value);
}

void DynamicFile::handleImpl(const IInput& input)
{
  line.handle(input);
  
  if(pick.clicked(input)) edit.setEnabled(true);
  else if(edit.done() && edit.getResult() == Dialog::OK)
  {
    edit.setEnabled(false);
    if(edit.getNumFiles() > 0)
    {
      std::string value = edit.getFileName(0);
      setValue(&value);
    }
  }
  else if(edit.done() && edit.getResult() == Dialog::CANCEL) edit.setEnabled(false);
}

void DynamicFile::drawImpl(IGUIDrawer& drawer) const
{
  line.draw(drawer);
  //pick.drawDebugBorder(drawer);
  drawer.drawRectangle(pick.getX0(), pick.getY0(), pick.getX1(), pick.getY1(), RGB_Grey, true);
  drawer.drawRectangle(pick.getX0(), pick.getY0(), pick.getX1(), pick.getY1(), RGB_Black, false);
  //drawer.drawText("...", pick.getCenterX(), pick.getCenterY(), FONT_Black4, TextAlign(HA_CENTER, VA_CENTER));
  drawer.drawPoint(pick.getCenterX(), pick.getY1() - 4, RGB_Black);
  drawer.drawPoint(pick.getCenterX() - 2, pick.getY1() - 4, RGB_Black);
  drawer.drawPoint(pick.getCenterX() + 2, pick.getY1() - 4, RGB_Black);
}

void DynamicFile::manageHoverImpl(IHoverManager& hover)
{
  if(edit.isEnabled())
    hover.addHoverElement(&edit);
}
////////////////////////////////////////////////////////////////////////////////

DynamicPage::DynamicPage()
: enableTitle(false)
, title_width(0.5)
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
  int titleheight = (enableTitle ? TITLEHEIGHT : 0);
  control->resize(x0 + xb + 1, y0 + titleheight + i * CONTROLHEIGHT + 1, x1, y0 + titleheight + (i + 1) * CONTROLHEIGHT);
  addSubElement(control, Sticky(title_width,1, 0.0,titleheight + i * CONTROLHEIGHT + 1, 1.0,0, 0.0,titleheight + (i + 1) * CONTROLHEIGHT));
}

void DynamicPage::drawImpl(IGUIDrawer& drawer) const
{
  if(enableTitle)
  {
    drawer.drawRectangle(x0, y0, x1, y0 + TITLEHEIGHT, RGB_White, false);
    drawer.drawText(title, x0 + 4, y0 + 4);
  }
  
  for(size_t i = 0; i < controls.size(); i++)
  {
    int y = (enableTitle ? TITLEHEIGHT : 0) + CONTROLHEIGHT * i;
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
  
  setSizeY((enableTitle ? TITLEHEIGHT : 0) + controls.size() * CONTROLHEIGHT);
}

void DynamicPage::setTitle(const std::string& title)
{
  enableTitle = true;
  this->title = title;
}

} //namespace gui
} //namespace lpi

