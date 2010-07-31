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
#include <SDL/SDL.h> //this is for the key codes like SDLK_TAB

namespace lpi
{
namespace gui
{

static ColorRGB rectangleColor(255, 255, 255, 128);

DynamicColor::DynamicColor(ColorRGB* value, const IGUIDrawer& geom)
: box(value)
, edit(geom)
{
  this->bind = value;
  ctor();
}

void DynamicColor::ctor()
{
  this->resize(0, 0, 20, CONTROLHEIGHT);
  box.resize(0, 0, 12, 12);
  box.move(1, 1);
  this->addSubElement(&box, Sticky(0.0,0, 0.5,-box.getSizeY() / 2, 0.0,box.getSizeX(), 0.5,box.getSizeY() / 2));
  edit.setEnabled(false);
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

DynamicColord::DynamicColord(ColorRGBd* value, const IGUIDrawer& geom)
: box(value)
, edit(geom)
{
  this->bind = value;
  ctor();
}

DynamicColord::~DynamicColord()
{
}

void DynamicColord::ctor()
{
  this->resize(0, 0, 20, CONTROLHEIGHT);
  box.resize(0, 0, 12, 12);
  box.move(1, 1);
  this->addSubElement(&box, Sticky(0.0,0, 0.5,-box.getSizeY() / 2, 0.0,box.getSizeX(), 0.5,box.getSizeY() / 2));
  edit.setEnabled(false);
}

void DynamicColord::getValue(ColorRGBd* value)
{
  (void)value;
}

void DynamicColord::setValue(ColorRGBd* value)
{
  (void)value;
}

void DynamicColord::handleImpl(const IInput& input)
{
  box.handle(input);

  if(this->clicked(input))
  {
    edit.setEnabled(true);
    edit.setColor(*bind);
    edit.moveTo(input.mouseX(), input.mouseY());
  }
  else if(edit.isEnabled() && edit.mouseJustDownElsewhere(input))
  {
    edit.setEnabled(false);
  }
  else if(edit.pressedOk(input)) edit.setEnabled(false);

  if(edit.isEnabled())
  {
    edit.getColor(*bind);
  }
}

void DynamicColord::drawImpl(IGUIDrawer& drawer) const
{
  box.draw(drawer);
}

void DynamicColord::manageHoverImpl(IHoverManager& hover)
{
  if(edit.isEnabled())
    hover.addHoverElement(&edit);
}
////////////////////////////////////////////////////////////////////////////////

void DynamicFile::ctor()
{
  this->resize(0, 0, 20, CONTROLHEIGHT);
  pick.resize(0, 0, 12, 12);
  this->addSubElement(&pick, Sticky(1.0,-pick.getSizeX(), 0.5,-pick.getSizeY() / 2, 1.0,0, 0.5,pick.getSizeY() / 2));
  edit.setEnabled(false);
  line.make(this->x0, this->y0, 256);
  line.move(0, (this->getSizeY() - line.getSizeY()) / 2);
  this->addSubElement(&line, Sticky(0.0,0, 0.5,-line.getSizeY() / 2, 1.0,0, 0.5,line.getSizeY() / 2));
}

DynamicFile::DynamicFile(std::string* value, const IGUIDrawer& geom, IFileBrowse* browser, FileDialogPersist* persist)
: edit(geom, browser, false, false, persist)
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

const static double TITLE_WIDTH = 0.45;

void DynamicPage::RowControl::draw(int x0, int y0, int x1, int y1, IGUIDrawer& drawer) const
{
  int xb = (int)((x1-x0) * TITLE_WIDTH); //"x-between": the divider between title and value
  drawer.drawRectangle(x0, y0, x0 + xb, y1 + 1, rectangleColor, false);
  drawer.drawRectangle(x0 + xb, y0, x1, y1 + 1, rectangleColor, false);
  drawer.drawGUIPartText(lpi::gui::GPT_DEFAULT_TEXT00, name, x0 + 4, y0 + 3, xb, y1 - 4); //drawer.drawText(name, x0 + 4, y0 + 4);
  control->draw(drawer);
}

DynamicPage::RowControl::~RowControl()
{
  delete control;
}

void DynamicPage::RowText::draw(int x0, int y0, int x1, int y1, IGUIDrawer& drawer) const
{
  drawer.drawRectangle(x0, y0, x1, y1 + 1, rectangleColor, false);
  drawer.drawText(text, x0 + 4, y0 + 4);
}

DynamicPage::DynamicPage()
: enableTitle(false)
, activeNumber(-1)
, totalSizeY(0)
{
  resize(0, 0, 200, 200); //if not resized to something at begin, then added rows go wrong... BUG! :(
}

DynamicPage::~DynamicPage()
{
  clear();
}

void DynamicPage::clear()
{
  clearSubElements();
  for(size_t i = 0; i < rows.size(); i++)
    delete rows[i];
  rows.clear();
  activeNumber = -1;
  totalSizeY = 0;
}

void DynamicPage::controlToValue()
{
  for(size_t i = 0; i < rows.size(); i++)
    rows[i]->controlToValue();
}

void DynamicPage::valueToControl()
{
  for(size_t i = 0; i < rows.size(); i++)
    rows[i]->valueToControl();
}

size_t DynamicPage::addControl(const std::string& name, IDynamicControl* control)
{
  rows.push_back(new RowControl(control, name));
  
  size_t controlheight = control->getSizeY();

  size_t i = rows.size() - 1;
  int xb = (int)(getSizeX() * TITLE_WIDTH);
  int titleheight = (enableTitle ? TITLEHEIGHT : 0);
  control->resize(x0 + xb + 1, y0 + titleheight + i * controlheight + 1, x1, y0 + titleheight + (i + 1) * controlheight);
  addSubElement(control, Sticky(TITLE_WIDTH,1, 0.0,totalSizeY, 1.0,0, 0.0,totalSizeY+controlheight));
  
  totalSizeY += controlheight;
  
  return rows.size() - 1;
}

int DynamicPage::mouseToRow(int mouseY) const
{
  int my = mouseY - getY0();
  if(enableTitle) my -= TITLEHEIGHT;
  int h = 0;
  for(int i = 0; i < (int)rows.size(); i++)
  {
    if(my < h) return i - 1;
    h += rows[i]->getHeight();
  }
  return -1;
}

size_t DynamicPage::addTextRow(const std::string& text)
{
  rows.push_back(new RowText(text));
  totalSizeY += CONTROLHEIGHT;
  return rows.size() - 1;
}

void DynamicPage::addToolTipToLastRow(const std::string& text)
{
  if(!rows.empty())
  {
    rows.back()->tooltip = text;
  }
}

void DynamicPage::drawToolTip(IGUIDrawer& drawer) const
{
  int index = mouseToRow(drawer.getInput().mouseY());
  if(index >= 0 && index < (int)rows.size())
  {
    if(!rows[index]->tooltip.empty())
      ToolTipManager::drawToolTip(rows[index]->tooltip, drawer);
  }
}

void DynamicPage::drawImpl(IGUIDrawer& drawer) const
{
  if(enableTitle)
  {
    drawer.drawRectangle(x0, y0, x1, y0 + TITLEHEIGHT, rectangleColor, false);
    drawer.drawText(title, x0 + 4, y0 + 4);
  }
  
  int y = 0;
  int titleheight = (enableTitle ? TITLEHEIGHT : 0);
  
  for(size_t i = 0; i < rows.size(); i++)
  {
    rows[i]->draw(x0, y0 + y + titleheight, x1, y0 + y + rows[i]->getHeight() + titleheight, drawer);
    
    y += rows[i]->getHeight();
  }
}

void DynamicPage::manageHoverImpl(IHoverManager& hover)
{
  for(size_t i = 0; i < rows.size(); i++)
  {
    rows[i]->manageHover(hover);
  }
}


void DynamicPage::handleImpl(const IInput& input)
{
  for(size_t i = 0; i < rows.size(); i++)
    rows[i]->handle(input);
  
  setSizeY(totalSizeY);
  
  if(input.keyPressed(SDLK_TAB))
  {
    int active = getActiveNumber();
    if(active >= 0)
    {
      IDynamicControl* control = getControl(active);
      control->tabDeActivate();
      int j = active;
      int origj = j;
      if(input.keyDown(SDLK_LSHIFT))
      {
        for(;;)
        {
          j--;
          if(j < 0) j = rows.size() - 1;
          if(j == origj) break;
          IDynamicControl* control = getControl(j);
          if(control && control->canTab()) break;
        }
      }
      else
      {
        for(;;)
        {
          j++;
          if(j >= (int)rows.size()) j = 0;
          if(j == origj) break;
          IDynamicControl* control = getControl(j);
          if(control && control->canTab()) break;
        }
      }
      
      IDynamicControl* newControl = getControl(j);
      if(newControl)
      {
        newControl->tabActivate();
        activeNumber = j;
      }
    }
  }
}

void DynamicPage::setTitle(const std::string& title)
{
  enableTitle = true;
  this->title = title;
  totalSizeY += TITLEHEIGHT;
}

int DynamicPage::getKeyboardFocus() const
{
  int parentresult = ElementComposite::getKeyboardFocus();
  
  int active = getActiveNumber();
  if(active >= 0)
  {
    activeNumber = active;
    return parentresult | lpi::gui::KM_TAB;
  }
  else return parentresult;
}

IDynamicControl* DynamicPage::getControl(size_t index) const
{
  if(RowControl* c = dynamic_cast<RowControl*>(rows[index]))
  {
    return c->control;
  }
  return 0;
}

int DynamicPage::getActiveNumber() const
{
  const IDynamicControl* control = 0;
  
  if(activeNumber >= 0)
  {
    if(IDynamicControl* c = getControl(activeNumber))
    {
      control = c;
    }
  }
  
  if(activeNumber >= 0 && control && control->tabIsActive())
  {
    return activeNumber;
  }
  else
  {
    for(size_t i = 0; i < rows.size(); i++)
    {
      if(IDynamicControl* c = getControl(i))
      {
        if(c->tabIsActive()) return i;
      }
    }
  }
  return -1;
}

} //namespace gui
} //namespace lpi

