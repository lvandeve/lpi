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


#include "lpi_gui_ex.h"

#include "lodepng.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_draw2d_buffer.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"
#include "lpi_gui_file.h"
#include "lpi_gui_color.h"
#include "lpi_gui_text.h"

#include <iostream>

namespace lpi
{
namespace gui
{

InternalList::InternalList()
: allowMultiSelection(false)
, hasIcons(false)
, selectedItem(0)
, lastClickedItem(0)
{
  setEnabled(true);
}

void InternalList::deselectAll()
{
  for(size_t i = 0; i < selection.size(); i++) selection[i] = false;
  selectedItem = lastClickedItem = getNumItems();
}

size_t InternalList::getNumItems() const
{
  return items.size();
}

size_t InternalList::getSelectedItem() const
{
  return selectedItem;
}

bool InternalList::isSelected(size_t i) const
{
  if(allowMultiSelection) return selection[i];
  else return i == selectedItem;
}

void InternalList::setSelected(size_t i, bool selected)
{
  if(allowMultiSelection) selection[i] = selected;
  else selectedItem = selected ? i : getNumItems();
}

const std::string& InternalList::getValue(size_t i) const
{
  return items[i];
}

void InternalList::setValue(size_t i, const std::string& value)
{
  items[i] = value;
}

void InternalList::setIcon(size_t i, HTexture* icon)
{
  hasIcons = true;
  icons[i] = icon;
}

void InternalList::addItem(const std::string& value)
{
  if(selectedItem >= getNumItems()) selectedItem++;
  items.push_back(value);
  selection.push_back(false);
  icons.push_back(0);
}

void InternalList::addItem(const std::string& value, HTexture* icon)
{
  addItem(value);
  setIcon(icons.size() - 1, icon);
}

void InternalList::insertItem(size_t i, const std::string& value)
{
  if(i <= selectedItem) selectedItem++;
  items.insert(items.begin() + i, value);
  selection.insert(selection.begin() + i, false);
}

void InternalList::removeItem(size_t i)
{
  if(i < selectedItem) selectedItem--;
  else if(i == selectedItem) selectedItem = getNumItems();
  items.erase(items.begin() + i);
  selection.erase(selection.begin() + i);
}

void InternalList::setAllowMultiSelection(bool set)
{
  allowMultiSelection = set;
}

void InternalList::handleImpl(const IInput& input)
{
  int sizey = getNumItems() * getItemHeight();
  if(sizey != getSizeY()) resize(x0, y0, x1, y0 + sizey);
  
  //if(pressed(input))
  if(mouse_state_for_selection.mouseJustDownHere(mouseOver(input), input.mouseButtonDown(LMB)))
  {
    size_t index = (input.mouseY() - y0) / getItemHeight();
    if(index >= getNumItems()) return;
    
    if(allowMultiSelection && input.keyDown(SDLK_LSHIFT) && lastClickedItem < getNumItems())
    {
      if(lastClickedItem < index)
      {
        for(size_t i = lastClickedItem; i <= index; i++) setSelected(i);
      }
      else
      {
        for(size_t i = index; i <= lastClickedItem; i++) setSelected(i);
      }
    }
    else if(input.keyDown(SDLK_LCTRL))
    {
      if(isSelected(index)) setSelected(index, false);
      else setSelected(index, true);
      lastClickedItem = index;
    }
    else
    {
      deselectAll();
      setSelected(index);
      lastClickedItem = index;
    }
  }
}

void InternalList::drawImpl(IGUIDrawer& drawer) const
{
  drawPartial(drawer, y0, y1);
}

void InternalList::drawPartial(IGUIDrawer& drawer, int vy0, int vy1) const
{
  for(size_t i = 0; i < items.size(); i++)
  {
    int y = y0 + getItemHeight() * i + getItemHeight() / 2;
    if(y > vy1 || y < (int)(vy0 - getItemHeight())) continue; //it's not visible
    
    const Font& font = isSelected(i) ? FONT_Red : FONT_Black;
    drawer.drawText(items[i], x0 + (hasIcons ? 16 : 0), y, font, TextAlign(HA_LEFT, VA_CENTER));

    HTexture* icon = icons[i];
    if(icon && icon->texture)
    {
      drawer.convertTextureIfNeeded(icon->texture);
      drawer.drawTexture(icon->texture, x0, y0 + getItemHeight() * i);
    }
  }
}

void InternalList::clear()
{
  items.clear();
  selection.clear();
  icons.clear();
  selectedItem = lastClickedItem = 0;
  hasIcons = false;
}

size_t InternalList::getMouseItem(const IInput& input) const
{
  if(mouseOver(input)) return (input.mouseY() - y0) / getItemHeight();
  else return getNumItems();
}

size_t InternalList::getItemHeight() const
{
  return 16;
}

void InternalList::swap(size_t item1, size_t item2)
{
  std::swap(items[item1], items[item2]);
  std::swap(icons[item1], icons[item2]);
  bool temp = selection[item1]; //std::vector<bool> is special in C++, so std::swap won't work
  selection[item1] = selection[item2];
  selection[item2] = temp;
  if(selectedItem == item1) selectedItem = item2;
  else if(selectedItem == item2) selectedItem = item1;
  if(lastClickedItem == item1) lastClickedItem = item2;
  else if(lastClickedItem == item2) lastClickedItem = item1;
}

////////////////////////////////////////////////////////////////////////////////

List::List(const IGUIDrawer& geom)
{
  make(0, 0, 64, 64, &list, geom);
  setKeepElementsInside(true);
}

void List::resizeImpl(const Pos<int>& newPos)
{
  ScrollElement::resizeImpl(newPos);
  
  list.resize(newPos.x0, list.getY0(), newPos.x1 - (bars.venabled ? bars.vbar.getSizeX() : 0), list.getY1());
}

void List::handleImpl(const IInput& input)
{
  ScrollElement::handleImpl(input);
  bars.vbar.setSpeedMode(0);
  bars.vbar.absoluteSpeed = list.getItemHeight() * 8; //the factor is the number of items per second scrolled
  
  if(!bars.vbar.mouseOver(input))
  {
    if(mouseScrollUp(input)) forwardScrollToVerticalScrollbar(-1);
    if(mouseScrollDown(input)) forwardScrollToVerticalScrollbar(1);
  }
}

void List::drawImpl(IGUIDrawer& drawer) const
{
  drawer.pushSmallestScissor(getVisibleX0(), getVisibleY0(), getVisibleX1(), getVisibleY1()); //TODO: currently does same as pushScissor (because otherwise there's weird bug, to reproduce: resize the red window and look at smiley in the small grey window), so elements from container in container are still drawn outside container. DEBUG THIS ASAP!!!
  list.drawPartial(drawer, getVisibleY0(), getVisibleY1());
  drawer.popScissor();

  bars.draw(drawer);
}

bool List::clickedOnItem(const IInput& input)
{
  return !(bars.venabled && bars.vbar.mouseDown(input)) && clicked(input);
}

size_t List::getNumItems() const { return list.getNumItems(); }
size_t List::getSelectedItem() const { return list.getSelectedItem(); }
bool List::isSelected(size_t i) const { return list.isSelected(i); }
void List::setSelected(size_t i, bool selected) { list.setSelected(i, selected); }
void List::deselectAll() { list.deselectAll(); }
const std::string& List::getValue(size_t i) const { return list.getValue(i); }
void List::setValue(size_t i, const std::string& value) { list.setValue(i, value); }
void List::setIcon(size_t i, HTexture* icon) { list.setIcon(i, icon); }
void List::addItem(const std::string& value) { list.addItem(value); }
void List::addItem(const std::string& value, HTexture* icon) { list.addItem(value, icon); }
void List::insertItem(size_t i, const std::string& value) { list.insertItem(i, value); }
void List::removeItem(size_t i) { list.removeItem(i); }
void List::setAllowMultiSelection(bool set) { list.setAllowMultiSelection(set); }
void List::clear() { list.clear(); }
size_t List::getMouseItem(const IInput& input) const { return list.getMouseItem(input); }
void List::swap(size_t item1, size_t item2) { list.swap(item1, item2); }

////////////////////////////////////////////////////////////////////////////////
//GUIMATRIX/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Matrix::make(int x0, int y0, int x1, int y1, int numx, int numy)
{
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
  this->enabled = true;
  this->numx = numx;
  this->numy = numy;
}

unsigned long Matrix::getTileX(const IInput& input) const
{
  return mouseGetRelPosX(input) / (getSizeX() / numx);
}

unsigned long Matrix::getTileY(const IInput& input) const
{
  return mouseGetRelPosY(input) / (getSizeY() / numy);
}

int Matrix::getScreenX(int tilex) const
{
  return (tilex * getSizeX()) / numx + x0;
}

int Matrix::getScreenY(int tiley) const
{
  return (tiley * getSizeY()) / numy + y0;
}

////////////////////////////////////////////////////////////////////////////////
//GUIGRID///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::make(int x0, int y0, int x1, int y1, int tileSizeX, int tileSizeY)
{
  this->x0 = x0;
  this->y0 = y0; 
  this->x1 = x1;
  this->y1 = y1;
  this->enabled = true;
  this->tileSizeX = tileSizeX;
  this->tileSizeY = tileSizeY;
}

void Grid::setNumTiles(int amount) //real amount can be larger, as the width of rows will stay the same, it'll add rows at the bottom or remove rows
{
  int newsize = 0;
  if(amount > 0) newsize = tileSizeY * (((amount - 1) / getNumx()) + 1);
  growSizeY1(newsize);
}
  
Grid::Grid()
{
  showGrid = false;
}

//returns x position on screen of center of given tile
int Grid::getTileCenterx(int index) const //index = index of the tile
{
  if(getNumx() == 0) return x0;
  int x = (index % getNumx());
  
  x = x * tileSizeX;
  x += tileSizeX / 2;
  
  return x0 + x;
}

//returns y position on screen of center of given tile
int Grid::getTileCentery(int index) const //index = index of the tile
{
  if(getNumy() == 0) return y0;
  int y = (index / getNumx());
  y = y * tileSizeY;
  y += tileSizeY / 2;
  
  return y0 + y;
}

unsigned long Grid::getNumx() const
{
  if(tileSizeX == 0) return 0;
  else return getSizeX() / tileSizeX;
}

unsigned long Grid::getNumy() const
{
  if(tileSizeY == 0) return 0;
  else return getSizeY() / tileSizeY;
}

unsigned long Grid::getNumElements() const
{
  return getNumx() * getNumy();
}

int Grid::getTileX(const IInput& input) const
{
  return mouseGetRelPosX(input) / tileSizeX;
}

int Grid::getTileY(const IInput& input) const
{
  return mouseGetRelPosY(input) / tileSizeY;
}

int Grid::getTile(const IInput& input) const //returns index of the tile
{
  return getTileX(input) + getNumx() * getTileY(input);
}

int Grid::getScreenX(int tilex) const
{
  return tilex * tileSizeX + x0;
}

int Grid::getScreenY(int tiley) const
{
  return tiley * tileSizeY + y0;
}


void Grid::drawImpl(IGUIDrawer& drawer) const
{
  if(showGrid)
  {
    for(int y = y0; y < y1; y += tileSizeY)
    for(int x = x0; x < x1; x += tileSizeX)
    {
      drawer.drawLine(x, y0, x, y1, gridColor);
      drawer.drawLine(x0, y, x1, y, gridColor);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//GUIPAINTER////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
You give Painter drawing commands (lines, pictures, points, ...) that are queued, and then
all drawn when you call draw(). The queues are then cleared, to be filled again the next frame.

There's also by default a rectangle behind it, if you don't want it, set alpha channel of rectangle color to 0
*/

Painter::Painter()
{
  this->enabled = 0;
  this->stack.clear();
}

void Painter::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->color = color;
  this->stack.clear();
  this->setEnabled(true);
}

void Painter::drawImpl(IGUIDrawer& drawer) const
{
  //first draw the rectangle if the alpha channel of color is > 0
  if(color.a > 0) drawer.drawRectangle(x0, y0, x1, y1, color, true);
  
  //then draw the queued elements, in order
  for(unsigned long i = 0; i < stack.size(); i++)
  {
    switch(stack[i].type)
    {
      case 0: 
        drawer.drawPoint(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 1: 
        drawer.drawLine(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].x1 + x0, stack[i].y1 + y0, stack[i].color);
        break;
      case 2: 
        drawer.drawTexture(stack[i].texture, stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 3: 
        drawer.drawTextureCentered(stack[i].texture, stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 4: 
        drawer.drawRectangle(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].x1 + x0, stack[i].y1 + y0, stack[i].color, true);
        break;
      case 5:
        drawer.drawText(stack[i].text, stack[i].x0 + x0, stack[i].y0 + y0, stack[i].textFont);
        break;
      default: break;
    }
  }
  
  stack.clear();
}

void Painter::queuePoint(int x, int y, const ColorRGB& color)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].color = color;
  stack[i].type = 0;
}
void Painter::queueLine(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x0;
  stack[i].y0 = y0;
  stack[i].x1 = x1;
  stack[i].y1 = y1;
  stack[i].color = color;
  stack[i].type = 1;
}

void Painter::queueTexture(int x, int y, ITexture* texture, const ColorRGB& colorMod)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].color = colorMod;
  stack[i].texture = texture;
  stack[i].type = 2;
}

void Painter::queueTextureCentered(int x, int y, ITexture* texture, const ColorRGB& colorMod)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].color = colorMod;
  stack[i].texture = texture;
  stack[i].type = 3;
}

void Painter::queueRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x0;
  stack[i].y0 = y0;
  stack[i].x1 = x1;
  stack[i].y1 = y1;
  stack[i].color = color;
  stack[i].type = 4;
}

void Painter::queueText(int x, int y, const std::string& text, const Font& font)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].x1 = x;
  stack[i].y1 = y;
  stack[i].text = text;
  stack[i].textFont = font;
  stack[i].type = 5;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

MessageBox::MessageBox(const IGUIDrawer& geom, const std::string& text, const std::string& title)
: text(text)
{
  resize(0, 0 , 300, 150);
  addTop(geom);
  if(!title.empty()) addTitle(title);
  addCloseButton(geom);

  ok.makeTextPanel(0, 0, "Ok");

  pushTop(&ok, Sticky(0.5,-40, 1.0,-32, 0.5,+40, 1.0,-4));

  setEnabled(true);
}

bool MessageBox::done() const
{
  return !isEnabled();
}

Dialog::Result MessageBox::getResult() const
{
  return OK;
}

void MessageBox::handleImpl(const IInput& input)
{
  Dialog::handleImpl(input);
  if(ok.clicked(input) || closeButtonClicked(input)) setEnabled(false);
}

void MessageBox::drawImpl(IGUIDrawer& drawer) const
{
  Dialog::drawImpl(drawer);
  drawer.drawGUIPartText(GPT_MESSAGE_TEXT, text, x0 + 4, y0 + 4, x1 - 4, y1 - 4);
}
  
////////////////////////////////////////////////////////////////////////////////
//YESNOWINDOW///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

YesNoWindow::YesNoWindow(const IGUIDrawer& geom, const std::string& text, const std::string& title)
: value(false)
{
  resize(0, 0 , 300, 150);
  addTop(geom);
  if(!title.empty()) addTitle(title);
  //addCloseButton(geom);

  yes.makeTextPanel(0, 0, "Yes");
  no.makeTextPanel(0, 0, "No");
  message.make(0, 0, text);

  pushTop(&message, Sticky(0.0,4, 0.0,4, 1.0,-4, 1.0,-36));
  pushTop(&yes, Sticky(0.5,-82, 1.0,-32, 0.5,-2, 1.0,-4));
  pushTop(&no, Sticky(0.5,+2, 1.0,-32, 0.5,+82, 1.0,-4));

  setEnabled(true);
}

bool YesNoWindow::done() const
{
  return !isEnabled();
}

Dialog::Result YesNoWindow::getResult() const
{
  return OK;
}

void YesNoWindow::handleImpl(const IInput& input)
{
  Dialog::handleImpl(input);
  if(yes.clicked(input)) { value = true; setEnabled(false); }
  if(no.clicked(input)) { value = false; setEnabled(false); }
}

bool YesNoWindow::getValue() const
{
  return value;
}

void YesNoWindow::setButtonTexts(const std::string& textYes, const std::string& textNo)
{
  yes.makeTextPanel(0, 0, textYes);
  no.makeTextPanel(0, 0, textNo);
}

////////////////////////////////////////////////////////////////////////////////
//GUICANVAS/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Canvas: allows you to draw on it with the mouse
*/

Canvas::Canvas(IGUIDrawer& drawer) : canvas(new HTexture)
{
  canvas->texture = drawer.createTexture();
  this->enabled = 0;
}

Canvas::~Canvas()
{
  delete canvas;
}

void Canvas::make(int x, int y, int sizex, int sizey, const ColorRGB& backColor, int border, const ColorRGB& leftColor, const ColorRGB& rightColor, double size, double hardness, double opacity)
{
  this->enabled = true;
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->leftColor = leftColor;
  this->rightColor = rightColor;
  this->backColor = backColor;
  this->border = border;
  
  this->size = size;
  this->opacity = opacity;
  this->hardness = hardness;
  
  this->oldMouseX = -1;
  this->oldMouseY = -1;
  
  init();
}

void Canvas::init()
{
  clear();
}

void Canvas::handleImpl(const IInput& input)
{
  if(mouseGrabbed(input, LMB) || mouseGrabbed(input, RMB))
  {
    ColorRGB drawColor;
    
    if(input.mouseButtonDown(LMB)) drawColor = leftColor;
    else drawColor = rightColor;
    
    int drawx = mouseGetRelPosX(input);
    int drawy = mouseGetRelPosY(input);

    if(validOldMousePos == true)
    {
      Drawer2DTexture drawer(canvas->texture);
      drawer.drawLine(oldMouseX, oldMouseY, drawx, drawy, drawColor);
      canvas->texture->update();
    }
    oldMouseX = drawx;
    oldMouseY = drawy;
    validOldMousePos = true;
  }
  else validOldMousePos = false;
}

void Canvas::drawImpl(IGUIDrawer& drawer) const
{
  drawer.convertTextureIfNeeded(canvas->texture);
  drawer.drawTexture(canvas->texture, x0, y0);
}

void Canvas::clear()
{
  if(canvas->texture) makeTextureSolid(canvas->texture, backColor, getSizeX(), getSizeY());
}

////////////////////////////////////////////////////////////////////////////////
//GUIRECTANGLE//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Rectangle is a simple rectangle but as child class of Element
*/

Rectangle::Rectangle()
{
  this->enabled = 0;
  this->color = RGB_Black;
}

void Rectangle::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->color = color;
  this->setEnabled(true);
}

void Rectangle::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawRectangle(x0, y0, x1, y1, color, false);
}

////////////////////////////////////////////////////////////////////////////////
//GUILINE///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Line is a simple line but as child class of Element
*/

Line::Line()
{
  this->enabled = 0;
  this->color = RGB_Black;

  this->lx0 = 0;
  this->ly0 = 0;
  this->lx1 = 0;
  this->ly1 = 0;
}

void Line::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizeX(sizex);
  this->setSizeY(sizey);
  this->color = color;
  this->setEnabled(true);
  this->lx0 = x;
  this->ly0 = y;
  this->lx1 = x + sizex;
  this->ly1 = y + sizey;
}

void Line::setEndpoints(int x0, int y0, int x1, int y1)
{
  this->lx0 = x0;
  this->ly0 = y0;
  this->lx1 = x1;
  this->ly1 = y1;
}

void Line::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawLine(lx0, ly0, lx1, ly1, color);
}

////////////////////////////////////////////////////////////////////////////////
//GUINSTATESTATE////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
This class is used by NState
*/

NStateState::NStateState()
{
  this->texture = getDefaultEmptyTexture();
  this->colorMod = RGB_White;
  this->enableText = 0;
  this->text = "";
}

//constructor for checkbox with text title
void NStateState::make(ITexture* texture, const ColorRGB& colorMod, const std::string& text, const Font& font)
{
  this->texture = texture;
  this->colorMod = colorMod;
  this->text = text;
  this->font = font;
  if(text != "") enableText = 1;
  
  positionText();
}



//place the text next to the checkbox
void NStateState::positionText()
{
  textOffsetX = texture->getU() + 2; //need some number of pixels that text is away from the texture, eg 2
  textOffsetY = texture->getV() / 2 - /*markup.getHeight()*/8 / 2; //TODo use drawer to determine text size
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
  this->enabled = 0;
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
  this->setEnabled(true);
  this->setSizeX(0); //no states yet, size 0
  this->setSizeY(0);
}

void NState::addState(ITexture* texture, const ColorRGB& colorMod, const std::string& text, const Font& font)
{
  this->setSizeX(texture->getU()); //set the size of the NState to that of the last added texture
  this->setSizeY(texture->getV());
  NStateState s;
  s.make(texture, colorMod, text, font);
  states.push_back(s);
}


//see how you click with the mouse and toggle on click
void NState::handleImpl(const IInput& input)
{
  //make sure never both pressed() and clicked() are checked, because one test screws up the other, hence the order of the tests in the if conditions
  if(states.size() == 0) return;
  
  if((toggleOnMouseUp == 0 && pressed(input, LMB)) || (toggleOnMouseUp == 1 && clicked(input, LMB)))
  {
    if(state >= states.size() - 1) state = 0;
    else state++;
  }
  if((toggleOnMouseUp == 0 && pressed(input, RMB)) || (toggleOnMouseUp == 1 && clicked(input, RMB)))
  {
    if(state == 0) state = states.size() - 1;
    else state--;
  }
  
}

//draw the checkbox with a texture depending on it's state
void NState::drawImpl(IGUIDrawer& drawer) const
{
  if(states.size() == 0) return;
  
  NStateState s = states[state];
  drawer.drawTexture(s.texture, x0, y0, s.colorMod);
  
  if(s.enableText) drawer.drawText(s.text, x0 + s.textOffsetX, y0 + s.textOffsetY, s.font);
  
  drawLabel(drawer, this);
}

////////////////////////////////////////////////////////////////////////////////

AMenu::AMenu()
: lastItem((size_t)(-1))
, stay(false)
, openedsubmenu((size_t)(-1))
, parent(0)
{
}

size_t AMenu::addCommand(const std::string& name, const IGUIDrawer& geom)
{
  items.resize(items.size() + 1);
  items.back().type = COMMAND;
  items.back().name = name;
  items.back().submenu = 0;
  onAddItem(geom);
  
  return getNumItems() - 1;
}

size_t AMenu::addSubMenu(AMenu* submenu, const std::string& name, const IGUIDrawer& geom)
{
  items.resize(items.size() + 1);
  items.back().type = SUBMENU;
  items.back().name = name;
  items.back().submenu = submenu;
  submenu->setParent(this);
  onAddItem(geom);
  
  return getNumItems() - 1;
}

size_t AMenu::addSeparator(const IGUIDrawer& geom)
{
  items.resize(items.size() + 1);
  items.back().type = SEPARATOR;
  items.back().submenu = 0;
  onAddItem(geom);
  
  return getNumItems() - 1;
}

int AMenu::findSubMenu(const std::string& name)
{
  for(size_t i = 0; i < items.size(); i++)
  {
    if(items[i].type == SUBMENU && items[i].name == name) return i;
  }
  return -1;
}

AMenu* AMenu::getSubMenu(size_t index)
{
  if(items[index].type == SUBMENU) return items[index].submenu;
  return 0;
}

void AMenu::clear()
{
  onClear();
  items.clear();
}

size_t AMenu::getNumItems() const
{
  return items.size();
}

bool AMenu::itemClicked(size_t i, const IInput& input) const
{
  (void)input;
  
  if(lastItem == i)
  {
    lastItem = (size_t)(-1);
    return true;
  }
  return false;
}

size_t AMenu::itemClicked(const IInput& input) const
{
  (void)input;
  size_t result = lastItem;
  lastItem = (size_t)(-1);
  return result;
}

void AMenu::handleImpl(const IInput& input)
{
  if(clicked(input))
  {
    lastItem = getMouseIndex(input);
    if(lastItem < items.size())
    {
      Item& item = items[lastItem];
      if(item.type == SUBMENU)
      {
        if(openedsubmenu == lastItem)
        {
          item.submenu->disableMenu();
          openedsubmenu = (size_t)(-1);
        }
        else
        {
          item.submenu->setEnabled(true);
          if(openedsubmenu < getNumItems()) items[openedsubmenu].submenu->disableMenu();
          openedsubmenu = lastItem;
          onOpenSubMenu(input, lastItem);

        }
      }
      if(item.type == COMMAND && !stay)
      {
        disableMenu();
        disableParents();
      }
    }
  }
  
  if(openedsubmenu < getNumItems() || !stay) //opening menu by just going with mouse over it
  {
    size_t itemover = getMouseIndex(input);
    if(itemover < getNumItems())
    {
      Item& item = items[itemover];
      if(itemover != openedsubmenu && item.type == SUBMENU)
      {
        item.submenu->setEnabled(true);
        if(openedsubmenu < getNumItems()) items[openedsubmenu].submenu->disableMenu();
        openedsubmenu = itemover;
        onOpenSubMenu(input, itemover);
      }
    }
  }
  
  bool temp_left = !stay && mouseJustDownElsewhere(input) && !(parent != 0 && parent->mouseDown(input));
  bool temp_right = !stay && mouseJustDownElsewhere(input, lpi::RMB) && !(parent != 0 && parent->mouseDown(input, lpi::RMB));
  
  if(temp_left || temp_right)
  {
    if(openedsubmenu >= getNumItems())
    {
      //setEnabled(false);
      disableMenu();
      disableParents();
    }
  }
  
  if(openedsubmenu < getNumItems() && !items[openedsubmenu].submenu->isEnabled())
  {
    openedsubmenu = (size_t)(-1);
  }
}

void AMenu::disableMenu()
{
  if(openedsubmenu < getNumItems())
  {
    items[openedsubmenu].submenu->disableMenu();
    openedsubmenu = (size_t)(-1);
  }
  setEnabled(false);
}

void AMenu::manageHoverImpl(IHoverManager& hover)
{
  for(size_t i = 0; i < items.size(); i++)
  {
    if(items[i].type == SUBMENU && items[i].submenu->isEnabled())
    {
      hover.addHoverElement(items[i].submenu);
    }
  }
}

void AMenu::disableParents()
{
  if(parent && !parent->stay)
  {
    parent->setEnabled(false);
    parent->disableParents();
  }
}

////////////////////////////////////////////////////////////////////////////////

MenuHorizontal::MenuHorizontal()
{
  stay = true;
  clear();
  setEnabled(true);
}

size_t MenuHorizontal::getMouseIndex(const IInput& input) const
{
  if(!mouseOver(input)) return (size_t)(-1);
  
  int mousePos = mouseGetRelPosX(input);
  
  int totalSize = 0;
  
  for(size_t i = 0; i < sizes.size(); i++)
  {
    totalSize += sizes[i];
    if(mousePos < totalSize) return i;
  }
  
  return (size_t)(-1);
}

void MenuHorizontal::onAddItem(const IGUIDrawer& geom)
{
  Item& item = items.back();
  int size = 0;
  
  if(item.type == COMMAND || item.type == SUBMENU)
  {
    int w, h;
    geom.calcTextRectSize(w, h, item.name);
    size = w + 12;
  }
  else size = 8;
  
  sizes.push_back(size);
  positions.push_back(positions.empty() ? 0 : (positions.back() + sizes[sizes.size() - 2]));
  
  resize(x0, y0, x1 + size, y1);
}

void MenuHorizontal::onOpenSubMenu(const IInput& input, size_t index)
{
  (void)input;
  Item& item = items[index];
  item.submenu->moveTo(x0 + positions[index], y1);
}

void MenuHorizontal::onClear()
{
  sizes.clear();
  positions.clear();
  resize(0, 0, 1, 20);
}

void MenuHorizontal::drawImpl(IGUIDrawer& drawer) const
{
  int totalSize = 0;
  
  drawer.drawGUIPart(GP_HMENU_PANEL, x0, y0, x1, y1);
  
  size_t mouseElement = getMouseIndex(drawer.getInput());
  
  for(size_t i = 0; i < items.size(); i++)
  {
    GUIPartMod mod(mouseElement == i, mouseElement == i && mouseDown(drawer.getInput()));
    
    if(items[i].type == COMMAND)
    {
      drawer.drawGUIPartText(GPT_HMENU_TEXT, items[i].name, x0 + totalSize, y0, x0 + totalSize + sizes[i], y1, mod);
    }
    else if(items[i].type == SUBMENU)
    {
      drawer.drawGUIPartText(GPT_HMENU_SUBMENUTEXT, items[i].name, x0 + totalSize, y0, x0 + totalSize + sizes[i], y1, mod);
    }
    else if(items[i].type == SEPARATOR)
    {
      drawer.drawGUIPart(GP_HMENU_SEPARATOR, x0 + totalSize, y0, x0 + totalSize + sizes[i], y1, mod);
    }
    totalSize += sizes[i];
  }
}

////////////////////////////////////////////////////////////////////////////////

MenuVertical::MenuVertical()
{
  stay = false;
  clear();
  setEnabled(false);
}

size_t MenuVertical::getMouseIndex(const IInput& input) const
{
  if(!mouseOver(input)) return (size_t)(-1);

  int mousePos = mouseGetRelPosY(input);

  int totalSize = 0;

  for(size_t i = 0; i < sizes.size(); i++)
  {
    totalSize += sizes[i];
    if(mousePos < totalSize) return i;
  }

  return (size_t)(-1);
}

void MenuVertical::onAddItem(const IGUIDrawer& geom)
{
  Item& item = items.back();
  int size = 0;

  if(item.type == COMMAND || item.type == SUBMENU)
  {
    int w, h;
    geom.calcTextRectSize(w, h, item.name);
    size = 24;
    w += 8;
    if(item.type == SUBMENU) w += 16; //space for the arrow icon
    if(getSizeX() < w) resize(x0, y0, x0 + w, y1);
  }
  else size = 8;

  sizes.push_back(size);
  positions.push_back(positions.empty() ? 0 : (positions.back() + sizes[sizes.size() - 2]));

  resize(x0, y0, x1, y1 + size);
}

void MenuVertical::onOpenSubMenu(const IInput& input, size_t index)
{
  (void)input;
  Item& item = items[index];
  item.submenu->moveTo(x1, y0 + positions[index] + 1 - 2);
}

void MenuVertical::onClear()
{
  sizes.clear();
  positions.clear();
  resize(0, 0, 64, 1);
}

void MenuVertical::drawImpl(IGUIDrawer& drawer) const
{
  int totalSize = 0;

  if(items.empty()) drawer.drawGUIPart(GP_VMENU_PANEL, x0, y0, x1, y0 + 16);
  else drawer.drawGUIPart(GP_VMENU_PANEL, x0, y0, x1, y1);
  
  size_t mouseElement = getMouseIndex(drawer.getInput());

  for(size_t i = 0; i < items.size(); i++)
  {
    GUIPartMod mod(mouseElement == i, mouseElement == i && mouseDown(drawer.getInput()));
    
    if(items[i].type == COMMAND)
    {
      drawer.drawGUIPartText(GPT_VMENU_TEXT, items[i].name, x0, y0 + totalSize, x1, y0 + totalSize + sizes[i], mod);
    }
    else if(items[i].type == SUBMENU)
    {
      drawer.drawGUIPartText(GPT_VMENU_SUBMENUTEXT, items[i].name, x0, y0 + totalSize, x1, y0 + totalSize + sizes[i], mod);
    }
    else if(items[i].type == SEPARATOR)
    {
      drawer.drawGUIPart(GP_VMENU_SEPARATOR, x0, y0 + totalSize, x1, y0 + totalSize + sizes[i], mod);
    }
    totalSize += sizes[i];
  }
}

////////////////////////////////////////////////////////////////////////////////

#define TOOLBARICONW 24
#define TOOLBARICONH 24
#define TOOLBARSEPARATORW 8

ToolBar::ToolBar()
//: lastItem((size_t)(-1))
{
  for(size_t i = 0; i < NUM_MOUSE_BUTTONS; i++) lastItem[i] = (size_t)(-1);
}

void ToolBar::drawImpl(IGUIDrawer& drawer) const
{
  size_t mouseElement = getMouseIndex(drawer.getInput());
  
  drawer.drawGUIPart(GP_HMENU_PANEL, x0, y0, x1, y1);
  
  int x = 0;

  for(size_t i = 0; i < items.size(); i++)
  {
    GUIPartMod mod(mouseElement == i, mouseElement == i && mouseDown(drawer.getInput()));
    int itemWidth = (items[i].type == SEPARATOR ? TOOLBARSEPARATORW : TOOLBARICONW);
    
    if(items[i].type == COMMAND)
    {
      drawer.drawGUIPart(GP_TOOLBAR_BUTTON, x0 + x, y0, x0 + x + itemWidth, y0 + TOOLBARICONH, mod);
      drawer.convertTextureIfNeeded(items[i].icon->texture);
      drawer.drawTextureCentered(items[i].icon->texture, x0 + x + itemWidth / 2, y0 + TOOLBARICONH / 2);
    }
    else if(items[i].type == TOGGLE)
    {
      drawer.drawGUIPart(GP_TOOLBAR_BUTTON, x0 + x, y0, x0 + x + itemWidth, y0 + TOOLBARICONH, mod);
      drawer.convertTextureIfNeeded(items[i].icon->texture);
      drawer.drawTextureCentered(items[i].icon->texture, x0 + x + itemWidth / 2, y0 + TOOLBARICONH / 2);
      if(items[i].toggle) drawer.drawRectangle(x0 + x + 1, y0 + 1, x0 + x + itemWidth - 1, y0 + TOOLBARICONH - 1, RGB_Red, false);
    }
    else if(items[i].type == SEPARATOR)
    {
      drawer.drawGUIPart(GP_TOOLBAR_SEPARATOR, x0 + x, y0, x0 + x + itemWidth, y0 + TOOLBARICONH, mod);
    }
    
    x += itemWidth;
  }
}

void ToolBar::drawToolTip(IGUIDrawer& drawer) const
{
  size_t mouseElement = getMouseIndex(drawer.getInput());
  if(mouseElement < getNumItems())
  {
    ToolTipManager::drawToolTip(items[mouseElement].tooltip, drawer);
  }
}

void ToolBar::handleImpl(const IInput& input)
{
  for(size_t i = 0; i < NUM_MOUSE_BUTTONS; i++)
  {
    if(clicked(input, (MouseButton)i))
    {
      size_t j = getMouseIndex(input);
      lastItem[i] = j;
      if(i == 0 && items[j].type == TOGGLE) items[j].toggle = !items[j].toggle;
    }
  }
}

size_t ToolBar::getMouseIndex(const IInput& input) const
{
  if(!mouseOver(input)) return (size_t)(-1);

  int mousePos = mouseGetRelPosX(input);

  int totalSize = 0;

  for(size_t i = 0; i < items.size(); i++)
  {
    totalSize += (items[i].type == SEPARATOR ? TOOLBARSEPARATORW : TOOLBARICONW);
    if(mousePos < totalSize) return i;
  }

  return (size_t)(-1);
}

size_t ToolBar::addCommand(HTexture* icon, const std::string& tooltip)
{
  items.resize(items.size() + 1);
  items.back().icon = icon;
  items.back().tooltip = tooltip;
  items.back().type = COMMAND;
  resize(x0, y0, x1 + TOOLBARICONW, y0 + TOOLBARICONH);
  return getNumItems() - 1;
}

size_t ToolBar::addToggle(HTexture* icon, const std::string& tooltip, bool enabled)
{
  items.resize(items.size() + 1);
  items.back().icon = icon;
  items.back().tooltip = tooltip;
  items.back().type = TOGGLE;
  items.back().toggle = enabled;
  resize(x0, y0, x1 + TOOLBARICONW, y0 + TOOLBARICONH);
  return getNumItems() - 1;
}

size_t ToolBar::addSeparator()
{
  items.resize(items.size() + 1);
  items.back().icon = 0;
  items.back().type = SEPARATOR;
  resize(x0, y0, x1 + TOOLBARSEPARATORW, y0 + TOOLBARICONH);
  return getNumItems() - 1;
}

bool ToolBar::toggleEnabled(size_t i) const
{
  return items[i].toggle;
}

void ToolBar::setToggle(size_t i, bool enabled)
{
  items[i].toggle = enabled;
}

void ToolBar::clear()
{
  items.clear();
}

size_t ToolBar::getNumItems() const
{
  return items.size();
}

bool ToolBar::itemClicked(size_t i, const IInput& input, MouseButton button) const
{
  (void)input;
  
  if(lastItem[button] == i)
  {
    lastItem[button] = (size_t)(-1);
    return true;
  }
  return false;
}

size_t ToolBar::itemClicked(const IInput& input, MouseButton button) const
{
  (void)input;
  size_t result = lastItem[button];
  lastItem[button] = (size_t)(-1);
  return result;
}


////////////////////////////////////////////////////////////////////////////////

  
ProgressBarDialog::ProgressBarDialog(const IGUIDrawer& geom)
: progress(0.0)
, isdone(false)
{
  resize(0, 0, 300, 100);
  addTop(geom);
  addTitle("Progress");
  addCloseButton(geom);
  setColorMod(lpi::ColorRGB(255,255,255,224));

  cancel.makeTextPanel(0, 0, "cancel");
  
  pushTop(&cancel, Sticky(0.5,-40, 1.0,-24, 0.5,40, 1.0,-4));
  pushTop(&bar, Sticky(0.0,20, 0.4,-10, 1.0,-20, 0.4,10));
}

void ProgressBarDialog::handleImpl(const lpi::IInput& input)
{
  cancel.setEnabled(progress > 0);
  Dialog::handleImpl(input);
  if(cancel.clicked(input) || closeButtonClicked(input)) { result = CANCEL; isdone = true; }
}

void ProgressBarDialog::drawImpl(IGUIDrawer& drawer) const
{
  Dialog::drawImpl(drawer);
  //drawer.drawText(lpi::valtostr(progress.getProgress()), x0 + 8, y0 + 20);
  
  
  if(progress > 0)
  {
    int px = (int)(bar.getX0() + progress * (bar.getX1() - bar.getX0()));
    drawer.drawRectangle(bar.getX0(), bar.getY0(), px, bar.getY1(), lpi::RGB_Blue, true);
    drawer.drawRectangle(bar.getX0(), bar.getY0(), bar.getX1(), bar.getY1(), lpi::RGB_Black, false);
  }
  else
  {
    int value = (int)(drawer.getInput().getSeconds() * 43) % 256;
    ColorRGB c = HSVtoRGB(ColorHSV(value, 128, 128));
    drawer.drawRectangle(bar.getX0(), bar.getY0(), bar.getX1(), bar.getY1(), c, true);
    drawer.drawRectangle(bar.getX0(), bar.getY0(), bar.getX1(), bar.getY1(), lpi::RGB_Black, false);
  }
}

bool ProgressBarDialog::done() const
{
  return isdone;
}

void ProgressBarDialog::setProgress(double val)
{
  progress = val;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void DropDownList::handleImpl(const IInput& input)
{
  if(pressed(input))
  {
    list.setEnabled(!list.isEnabled());
    if(list.isEnabled())
    {
      list.resize(x0, list.getY0(), x1, list.getY0() + 120);
    }
  }
  if(list.clickedOnItem(input))
  {
    list.setEnabled(false);
    changed = true;
    listvalue = list.getSelectedItem();
  }
  else if(!list.mouseDown(input)) list.setSelected(listvalue); //lame, but, this is for the dynamic enum page with auto update (sets value before handle, gets value after handle). It goes wrong when setting while mouse is down (to select item...)
}

void DropDownList::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawRectangle(x0, y0, x1, y1, RGB_White, true);
  drawer.drawRectangle(x0, y0, x1, y1, RGB_Black, false);
  if(list.getSelectedItem() < list.getNumItems())
    drawer.drawText(list.getValue(list.getSelectedItem()), x0 + 2, (y0 + y1) / 2, FONT_Default, ALIGN_05);
  drawer.drawGUIPart(GP_DROPDOWN_BUTTON, x1 - 15, y0 + 1, x1 - 1, y1 - 1);
}

void DropDownList::manageHoverImpl(IHoverManager& hover)
{
  if(list.isEnabled())
  {
    list.moveTo(x0, y1);
    hover.addHoverElement(&list);
  }
}

DropDownList::DropDownList(const IGUIDrawer& geom)
: list(geom)
, changed(false)
{
  setEnabled(true);
  list.setEnabled(false);
  listvalue = list.getSelectedItem();
}

void DropDownList::setItems(const std::vector<std::string>& items)
{
  list.clear();
  for(size_t i = 0; i < items.size(); i++) list.addItem(items[i]);
  if(!items.empty()) { list.setSelected(0); changed = true; }
}

void DropDownList::addItem(const std::string& item)
{
  list.addItem(item);
  if(list.getNumItems() == 1) { list.setSelected(0); changed = true; }
}

size_t DropDownList::getNumItems() const
{
  return list.getNumItems();
}

const std::string& DropDownList::getValue(size_t i) const
{
  return list.getValue(i);
}

size_t DropDownList::getSelectedItem() const
{
  return listvalue;//list.getSelectedItem();
}

void DropDownList::setSelectedItem(size_t i)
{
  //list.setSelected(i);
  listvalue = i;
}

bool DropDownList::hasChanged()
{
  bool result = changed;
  changed = false;
  return result;
}

////////////////////////////////////////////////////////////////////////////////

void showMessageBox(MainContainer& c, IModalFrameHandler& frame, const std::string& text, const std::string& title)
{
  int x0, y0, x1, y1;
  frame.getScreenSize(x0, y0, x1, y1);
  
  lpi::gui::MessageBox dialog(frame.getDrawer(), text, title);
  
  /*int numlines = 0;
  for(size_t i = 0; i < text.size(); i++) if(text[i] == 10) numlines++;
  int height = std::max(200, numlines * 9 + 64);
  int width = 16 + 7 * getTextWidth(text);
  if(width > 800) width = 800;*/
  
  int width, height;
  frame.getDrawer().getGUIPartTextSize(width, height, lpi::gui::GPT_MESSAGE_TEXT, text);
  height += 64;
  width += 8;
  
  //TODO: make the size of the dialog depend on the text size
  dialog.resize(0, 0, width, height);
  dialog.moveCenterTo((x0+x1)/2, (y0+y1)/2);
  c.doModalDialog(dialog, frame);
}

lpi::gui::Dialog::Result getFileNameModal(MainContainer& c, IModalFrameHandler& frame, IFileBrowse* browser, std::string& filename, const std::string& current_path, bool save, FileDialogPersist* persist)
{
  (void)current_path; //TODO
  
  int x0, y0, x1, y1;
  frame.getScreenSize(x0, y0, x1, y1);

  lpi::gui::FileDialog dialog(frame.getDrawer(), browser, save, false, persist);
  dialog.moveCenterTo((x0+x1)/2, (y0+y1)/2);
  c.doModalDialog(dialog, frame);
  filename = dialog.getFileName();
  return dialog.getResult();
}

lpi::gui::Dialog::Result getFileNamesModal(MainContainer& c, IModalFrameHandler& frame, IFileBrowse* browser, std::vector<std::string>& filenames, const std::string& current_path, FileDialogPersist* persist)
{
  (void)current_path; //TODO
  
  int x0, y0, x1, y1;
  frame.getScreenSize(x0, y0, x1, y1);

  lpi::gui::FileDialog dialog(frame.getDrawer(), browser, false, true, persist);
  dialog.moveCenterTo((x0+x1)/2, (y0+y1)/2);
  c.doModalDialog(dialog, frame);
  for(size_t i = 0; i < dialog.getNumFiles(); i++)
  {
    filenames.push_back(dialog.getFileName(i));
  }
  return dialog.getResult();
}

lpi::gui::Dialog::Result getFileNameModal(MainContainer& c, IModalFrameHandler& frame, FileDialog& dialog, std::string& filename)
{
  int x0, y0, x1, y1;
  frame.getScreenSize(x0, y0, x1, y1);

  dialog.moveCenterTo((x0+x1)/2, (y0+y1)/2);
  c.doModalDialog(dialog, frame);
  filename = dialog.getFileName();
  return dialog.getResult();
}

lpi::gui::Dialog::Result getFileNamesModal(MainContainer& c, IModalFrameHandler& frame, FileDialog& dialog, std::vector<std::string>& filenames)
{
  int x0, y0, x1, y1;
  frame.getScreenSize(x0, y0, x1, y1);

  dialog.moveCenterTo((x0+x1)/2, (y0+y1)/2);
  c.doModalDialog(dialog, frame);
  for(size_t i = 0; i < dialog.getNumFiles(); i++)
  {
    filenames.push_back(dialog.getFileName(i));
  }
  return dialog.getResult();
}



bool getYesNoModal(MainContainer& c, IModalFrameHandler& frame, const std::string& question)
{
  int x0, y0, x1, y1;
  frame.getScreenSize(x0, y0, x1, y1);
  
  YesNoWindow dialog(frame.getDrawer(), question);
  
  int numlines = 0;
  for(size_t i = 0; i < question.size(); i++) if(question[i] == 10) numlines++;
  int height = std::max(200, numlines * 9 + 64);
  //TODO: make the size of the dialog depend on the text size
  dialog.resize(0, 0, 500, height);

  dialog.moveCenterTo((x0+x1)/2, (y0+y1)/2);
  c.doModalDialog(dialog, frame);

  return dialog.getValue();
}

bool getYesNoModal(MainContainer& c, IModalFrameHandler& frame, const std::string& question, const std::string& textYes, const std::string& textNo)
{
  int x0, y0, x1, y1;
  frame.getScreenSize(x0, y0, x1, y1);

  YesNoWindow dialog(frame.getDrawer(), question);
  dialog.setButtonTexts(textYes, textNo);

  int numlines = 0;
  for(size_t i = 0; i < question.size(); i++) if(question[i] == 10) numlines++;
  int height = std::max(200, numlines * 9 + 64);
  //TODO: make the size of the dialog depend on the text size
  dialog.resize(0, 0, 500, height);

  dialog.moveCenterTo((x0+x1)/2, (y0+y1)/2);
  c.doModalDialog(dialog, frame);

  return dialog.getValue();
}

} //namespace gui
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////

