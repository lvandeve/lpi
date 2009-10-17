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


#include "lpi_gui_ex.h"

#include "lodepng.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_draw2d_buffer.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"

#include <iostream>

namespace lpi
{
namespace gui
{

InternalList::InternalList()
: allowMultiSelection(false)
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

void InternalList::addItem(const std::string& value)
{
  if(selectedItem >= getNumItems()) selectedItem++;
  items.push_back(value);
  selection.push_back(false);
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
  for(size_t i = 0; i < items.size(); i++)
  {
    if(isSelected(i)) drawer.drawText(items[i], x0, y0 + getItemHeight() * i, FONT_Red);
    else drawer.drawText(items[i], x0, y0 + getItemHeight() * i, FONT_Black);
  }
}

void InternalList::clear()
{
  items.clear();
  selection.clear();
  selectedItem = lastClickedItem = 0;
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

////////////////////////////////////////////////////////////////////////////////

List::List(const IGUIPartGeom& geom)
{
  make(0, 0, 64, 64, &list, geom);
}

void List::resizeImpl(const Pos<int>& newPos)
{
  ScrollElement::resizeImpl(newPos);
  
  list.resize(newPos.x0, newPos.y0, newPos.x1, newPos.y0 + list.getSizeY());
}

size_t List::getNumItems() const { return list.getNumItems(); }
size_t List::getSelectedItem() const { return list.getSelectedItem(); }
bool List::isSelected(size_t i) const { return list.isSelected(i); }
void List::setSelected(size_t i, bool selected) { list.setSelected(i, selected); }
const std::string& List::getValue(size_t i) const { return list.getValue(i); }
void List::setValue(size_t i, const std::string& value) { list.setValue(i, value); }
void List::addItem(const std::string& value) { list.addItem(value); }
void List::insertItem(size_t i, const std::string& value) { list.insertItem(i, value); }
void List::removeItem(size_t i) { list.removeItem(i); }
void List::setAllowMultiSelection(bool set) { list.setAllowMultiSelection(set); }
void List::clear() { list.clear(); }
size_t List::getMouseItem(const IInput& input) const { return list.getMouseItem(input); }

//////////////////////////////////////////////////////////////////////////////////
////DropMenu/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


///*
//The DropMenu class.
//DropMenu is a list of text buttons, each having their own text.
//The DropMenu can have a panel or rectangle behind the text buttons as background.

//How to use the DropMenu class:
//- x and y define the position of the top left corner of the menu
//- text contains the text of all buttons, separated by '|' chars.
//- put two '|' chars in a row to get a separator in the menu
//- draw the menu on screen with it's draw() function
//- It can be used perfectly for both right click menus of in game objects, 
  //as for menus at the top of the program window
//*/


////Constructor to make an empty menu
//DropMenu::DropMenu()
//{
  //this->markup1 = TS_White;
  //this->markup2 = TS_White;
  
  //setOptions("");
  
  //autoDisable = true;
//}

////Constructor to make the menu
//void DropMenu::makeColored(int x, int y, const std::string& text, 
           //const Font* font1, const Font& font2,
           //const ColorRGB& /*menuColor*/, BackRule hrule)
//{
  //this->x0 = x;
  //this->y0 = y;
  //this->setEnabled(true);
  //this->markup1 = markup1;
  //this->markup2 = markup2;
  ////this->panel.makeUntextured(menuColor);
  //this->hrule = hrule;
  
  //setOptions(text);
  ////putInScreen(); //reposition then does setOptions a second time to fix changed positions
//}

////Constructor to make the menu
//void DropMenu::make(int x, int y, const std::string& text, 
           //const Font* font1, const Font& font2,
           //BackPanel panel, BackRule hrule)
//{
  //this->x0 = x;
  //this->y0 = y;
  //this->setEnabled(true);
  //this->markup1 = markup1;
  //this->markup2 = markup2;
  //this->panel = panel;
  //this->hrule = hrule;
  
  //setOptions(text);
  ////putInScreen(); //reposition then does setOptions a second time to fix changed positions
//}

//void DropMenu::clearOptions()
//{
  //menuButton.clear();
  //separator.clear();
  //identity.clear();
//}

///*Initialize the menu, calculate all positions, make the buttons, ...
//This function is called automaticly by the constructors*/
//void DropMenu::setOptions(const std::string& text)
//{
  //clearOptions();
  
  //unsigned long maxTextSize = 0; //the size of the largest text
  
  ////calculate n, the number of elements
  //unsigned long pos = 0;
  //unsigned long n = 0;
  //while(pos < text.length())
  //{
    //if(text[pos] == '|') n++;
    //pos++;
  //}
  //n++;
  
  //unsigned long i = 0;
  //unsigned long t = 0;

  //while(i < n)
  //{
    //std::string buttonText = "";

    //while(text[t] != '|' && t < text.length())
    //{
      //buttonText += text[t];
      //t++;
      //if(text[t] == 0) break;
    //}

    //separator.push_back(false);
    //if(buttonText.length() == 0) separator[i] = true;
    //t++;
    
    //Button b;
    //b.makeText(x0 + 2, y0 + 2 + (markup1.getHeight() + 2) * i, buttonText);
    //menuButton.push_back(b);
    
    //identity.push_back(0); //no useful identity added, but it has to be increased in size to match menuButton size
    
    //if(buttonText.length() > maxTextSize) maxTextSize = buttonText.length();
    
    //i++;
  //}
  
   
  //setSizeX(maxTextSize * markup1.getWidth() + markup1.getWidth() / 2);
  //setSizeY(n * (markup1.getHeight() + 2) + markup1.getHeight() / 2);
//}

//void DropMenu::addOption(const std::string& text, int id)
//{
    //Button b;
    //b.makeText(x0 + 2, y0 + 2 + (markup1.getHeight() + 2) * menuButton.size(), text);
    //menuButton.push_back(b);
    //separator.push_back(false);
    
    //addSubElement(&b);//TODO: fix this big memory corruption problem and Button copying
    
    //int separatorsize = 0;
    //for(unsigned long i = 0; i < separator.size(); i++) if(separator[i]) separatorsize++;
    
    //setSizeY((menuButton.size() + separatorsize) * (markup1.getHeight() + 2) + markup1.getHeight() / 2);

    ////int newSizex = text.length() * markup1.getWidth() + markup1.getWidth() / 2;
    //int newSizex;// = text.length();
    //int textheight;
    //getFormattedTextSize(text, newSizex, textheight, markup1);
    //newSizex +=  markup1.getWidth() / 2;
    
    //if(newSizex > getSizeX()) setSizeX(newSizex);
    
    //identity.push_back(id);
//}

//void DropMenu::drawImpl(IGUIDrawer& drawer) const
//{
  //panel.draw(drawer, x0, y0, getSizeX(), getSizeY());

  //for(unsigned long i = 0; i < menuButton.size(); i++)
  //{
    //if(!separator[i]) menuButton[i].draw(drawer);
    //else hrule.draw(drawer, x0 + markup1.getWidth() / 2, y0 + 2 + (markup1.getHeight() + 2) * i + markup1.getHeight() / 2, getSizeX() - markup1.getWidth());
  //}
//}

//void DropMenu::handleImpl(const IInput& input)
//{
  //if(autoDisable && input.mouseButtonDown(LMB) && !mouseDown(input)) totallyDisable();
//}

///*
//Returns which of the options was pressed 
//Note: separaters take up a number spot too

//It returns:

//-1 if nothing clicked
//i (= 0 or a positive integer) if option i was clicked

//*/

//int DropMenu::check(const IInput& input)
//{
  //for(unsigned long i = 0; i < menuButton.size(); i++)
  //{
    //if(!separator[i])
    //{
      //if(menuButton[i].clicked(input)) return i;
    //}
  //}
  //return -1;
//}

//Button* DropMenu::getButton(int i)
//{
  //return &menuButton[i];
//}

//Button* DropMenu::getButton(const std::string& name)
//{
  //int i = 0;
  
  //for(int j = 0; j < getNumButtons(); j++) if(menuButton[j].text == name) i = j;
  
  //return &menuButton[i];
//}


//std::string DropMenu::checkText(const IInput& input)
//{
  //for(unsigned long i = 0; i < menuButton.size(); i++)
  //{
    //if(!separator[i])
    //{
      //if(menuButton[i].clicked(input)) return menuButton[i].text;
    //}
  //}
  //return "";
//}

//int DropMenu::checkIdentity(const IInput& input)
//{
  //for(unsigned long i = 0; i < menuButton.size(); i++)
  //{
    //if(!separator[i])
    //{
      //if(menuButton[i].clicked(input)) return identity[i];
    //}
  //}
  //return 0;
//}

//////////////////////////////////////////////////////////////////////////////////
////GUIDROPLIST///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///*
//The DropList class

//this class contains a list of options that can be opened or closed with a button,
//the list will have a scrollbar and allow you to choose an option with the mouse
//*/

////Constructor to make an empty droplist
//Droplist::Droplist()
//{
  //this->x0 = 0;
  //this->y0 = 0;
  //this->markup1 = TS_White;
  //this->markup2 = TS_White;
  
  //addSubElement(&listButton);
  //addSubElement(&bar);
  
  //init("", -1);
//}

//void Droplist::make(int x, int y, const std::string& text, int numVisibleOptions,
          //const Font* font1, const Font& font2, const Font& font3,
          ////BackPanel topPanel, BackPanel listPanel,
          //Texture* buttonTexture)
//{
  ////this->topPanel = topPanel;
  ////this->listPanel = listPanel;
  //this->markup1 = markup1;
  //this->markup2 = markup2;
  //this->markup3 = markup3;
  //this->opened = 0;
  //this->selected = 0;
  //this->x0 = x;
  //this->y0 = y;
  //this->setEnabled(true);
  
  //int buttonWidth = buttonTexture->getU();
  //int buttonHeight = buttonTexture->getV();
  
  //sizeyc = buttonHeight;
  //init(text, numVisibleOptions);
  //sizexo += buttonWidth;
  //sizexc = sizexo;
  
  //this->setSizeX(sizexc);
  //this->setSizeY(sizeyc);
  
  //listButton.makeImage(x1 - buttonWidth, y0, buttonTexture, buttonTexture, buttonTexture, RGB_White, RGB_Grey, RGB_Grey);


  //bar.makeVertical(x1 - buttonWidth, y0 + sizeyc, sizeyo - sizeyc,
                   //100, 0, 0, 1,
                   //&builtInGuiSet, 1);

//}

//void Droplist::makeScrollbar(const GuiSet* set)
//{
  //bar.makeVertical(x1 - set->arrowN->getU(), y0 + sizeyc, sizeyo - sizeyc,
                   //100, 0, 0, 1,
                   //set, 1);
//}  

//void Droplist::init(const std::string& text, int numVisibleOptions)
//{
  //textButton.clear();
  
  //unsigned long maxTextSize = 0; //the size of the largest text
  
  ////calculate n, the number of elements
  //unsigned long pos = 0;
  //unsigned long n = 0;
  //while(pos < text.length())
  //{
    //if(text[pos] == '|') n++;
    //pos++;
  //}
  //n++;
  
  //unsigned long i = 0;
  //unsigned long t = 0;

  //while(i < n)
  //{
    //std::string buttonText = "";

    //while(text[t] != '|' && t < text.length())
    //{
      //buttonText += text[t];
      //t++;
    //}
    //t++;
    
    //Button b;
    //b.makeText(x0 + 2, y0 + sizeyc + 2 + (markup1.getHeight() + 2) * i, buttonText);
    //textButton.push_back(b);
    
    //if(buttonText.length() > maxTextSize) maxTextSize = buttonText.length();
    
    //i++;
  //}
  
  //if(numVisibleOptions == -1) this->numVisibleOptions = numVisibleOptions = n;
  //else this->numVisibleOptions = numVisibleOptions;
   
  //sizexo = maxTextSize * markup1.getWidth() + markup1.getWidth() / 2 + bar.getSizeX();
  //sizeyo = sizeyc + numVisibleOptions * (markup1.getHeight() + 2) + markup1.getHeight() / 2;
  
  //int buttonWidth = maxTextSize * markup1.getWidth();
  //for(unsigned long i = 0; i < textButton.size(); i++)
  //{
    //textButton[i].setSizeX(buttonWidth);
  //}
//}

//void Droplist::addOption(const std::string& text)
//{
  //Button b;
  //b.makeText(x0 + 2, y0 + sizeyc + 2 + (markup1.getHeight() + 2) * textButton.size(), text);
  //textButton.push_back(b);
  //addSubElement(&textButton.back()); //TODO: fix this big memory corruption problem and Button copying
  
  //unsigned long maxTextSize = b.getSizeX() / markup1.getWidth();
  //if(text.length() > maxTextSize) maxTextSize = text.length();
  
  //int buttonWidth = maxTextSize * markup1.getWidth();
  //for(unsigned long i = 0; i < textButton.size(); i++)
  //{
    //textButton[i].setSizeX(buttonWidth);
  //}
  
  //setSizeX(maxTextSize * markup1.getWidth() + markup1.getWidth() / 2 + bar.getSizeX());
  //sizexo = sizexc = getSizeX();
  
  //bar.moveTo(x0 + sizexo - bar.getSizeX(), bar.getY0());
  //listButton.moveTo(x0 + sizexc - listButton.getSizeX(), listButton.getY0());
//}

//void Droplist::drawImpl(IGUIDrawer& drawer) const
//{
  //topPanel.draw(drawer, x0, y0, sizexc, sizeyc);
  //listButton.draw(drawer);
  //print(textButton[selected].text, x0, y0 + sizeyc / 2 - markup3.getHeight() / 2, markup3);

  //if(opened)
  //{
    //listPanel.draw(drawer, x0, y0 + sizeyc, sizexo, sizeyo - sizeyc);
    //for(unsigned long i = 0; i < textButton.size(); i++) textButton[i].draw(drawer);
    //bar.draw(drawer);
  //}
//}

//int Droplist::check()
//{
  //return selected;
//}

//std::string Droplist::checkText()
//{
  //return textButton[selected].text;
//}

//void Droplist::handleImpl(const IInput& input)
//{
  //if(listButton.clicked(input) || (clicked(input) && mouseGetRelPosY(input) < sizeyc && !listButton.mouseOver(input))) //behind the or is to open it also if you press the thing itself but not the button, the getMouseY() < sizeyc is necessary, without it, the code after this will not work anymore (starting from if(openen)
  //{
    //if(!opened)
    //{
      //open();
    //}
    //else
    //{
      //close();
    //}
  //}
  
  //if(opened)
  //{
    //for(unsigned long i = 0; i <  textButton.size(); i++)
    //{
      //if(textButton[i].clicked(input))
      //{
        //close();
        //selected = i;
      //}
    //}
    
    //if(mouseScrollUp(input) && !bar.mouseOver(input)) bar.scroll(input, -2);
    //if(mouseScrollDown(input) && !bar.mouseOver(input)) bar.scroll(input, 2);
    //bar.handle(input);
    //scroll();
  //}
//}

//void Droplist::scroll()
//{
  //int v = numVisibleOptions;
  //int n = textButton.size();
  
  //if(n <= v); //dan moet er ni gescrolld worden
  //else
  //{
    //int s = int(((n - v) * bar.scrollPos) / bar.scrollSize);
    
    //for(int i = 0; i < n; i++)
    //{
      //textButton[i].moveTo(textButton[i].getX0(), y0 + sizeyc + 2 + (markup1.getHeight() + 2) * (i - s));
      
      //if(i >= s && i < s + v)
      //{
        //textButton[i].setVisible(1);
        //textButton[i].setActive(1);
      //}
      //else
      //{
        //textButton[i].setVisible(0);
        //textButton[i].setActive(0);
      //}
    //}
  //}
//}

//void Droplist::open()
//{
  //opened = 1;
  //setSizeX(sizexo);
  //setSizeY(sizeyo);
//}

//void Droplist::close()
//{
  //opened = 0;
  //setSizeX(sizexc);
  //setSizeY(sizeyc);
//}

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
//YESNOWINDOW///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

YesNoWindow::YesNoWindow()
{
  this->enabled = 0;
}

void YesNoWindow::make(int x, int y, int sizex, int sizey, const std::string& text)
{
  Window::resize(x, y , sizex, sizey);
  //addTop();
  
  yes.makeTextPanel(0, 0, "Yes");
  no.makeTextPanel(0, 0, "No");
  //yes.autoTextSize();
  //no.autoTextSize();
  
  message.make(0, 0, text);
  
  pushTopAt(&message, 16, 16);
  int centerx = getSizeX() / 2;
  pushTopAt(&yes, centerx - yes.getSizeX() - 16, getSizeY() - yes.getSizeY() - 8 - 16);
  pushTopAt(&no, centerx + 16, getSizeY() - no.getSizeY() - 8 - 16);
  
  this->enabled = true;
}

////////////////////////////////////////////////////////////////////////////////
//OKWINDOW//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

OkWindow::OkWindow()
{
  this->enabled = false;
}

void OkWindow::make(int x, int y, int sizex, int sizey, const std::string& text)
{
  resize(x, y , sizex, sizey);
  //addTop();
  
  ok.makeTextPanel(0, 0, "Ok");
  //ok.autoTextSize();
  
  message.make(0, 0, text);
  
  pushTopAt(&message, 16, 16);
  int centerx = getSizeX() / 2;
  pushTopAt(&ok, centerx - ok.getSizeX() / 2, getSizeY() - ok.getSizeY() - 8 - 16);
  
  this->enabled = 1;
}
  
////////////////////////////////////////////////////////////////////////////////
//GUICANVAS/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Canvas: allows you to draw on it with the mouse
*/

Canvas::Canvas(IGUIDrawer& drawer) : canvas(new ITexture*)
{
  (*canvas) = drawer.createTexture();
  this->enabled = 0;
}

Canvas::~Canvas()
{
  delete *canvas;
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
      Drawer2DTexture drawer(*canvas);
      drawer.drawLine(oldMouseX, oldMouseY, drawx, drawy, drawColor);
      (*canvas)->update();
    }
    oldMouseX = drawx;
    oldMouseY = drawy;
    validOldMousePos = true;
  }
  else validOldMousePos = false;
}

void Canvas::drawImpl(IGUIDrawer& drawer) const
{
  drawer.convertTextureIfNeeded((*canvas));
  drawer.drawTexture((*canvas), x0, y0);
}

void Canvas::clear()
{
  makeTextureSolid((*canvas), backColor, getSizeX(), getSizeY());
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
  this->texture = emptyTexture;
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

} //namespace gui
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////

