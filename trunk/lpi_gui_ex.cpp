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
#include "lpi_gl.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"

#include <iostream>

namespace lpi
{
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
//DropMenu/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*
The DropMenu class.
DropMenu is a list of text buttons, each having their own text.
The DropMenu can have a panel or rectangle behind the text buttons as background.

How to use the DropMenu class:
- x and y define the position of the top left corner of the menu
- text contains the text of all buttons, separated by '|' chars.
- put two '|' chars in a row to get a separator in the menu
- draw the menu on screen with it's draw() function
- It can be used perfectly for both right click menus of in game objects, 
  as for menus at the top of the program window
*/


//Constructor to make an empty menu
DropMenu::DropMenu()
{
  this->markup1 = TS_White;
  this->markup2 = TS_White;
  
  setOptions("");
  
  autoDisable = true;
}

//Constructor to make the menu
void DropMenu::makeColored(int x, int y, const std::string& text, 
           const Markup& markup1, const Markup& markup2,
           const ColorRGB& menuColor, BackRule hrule)
{
  this->x0 = x;
  this->y0 = y;
  this->totallyEnable();
  this->markup1 = markup1;
  this->markup2 = markup2;
  this->panel.makeUntextured(menuColor);
  this->hrule = hrule;
  
  setOptions(text);
  putInScreen(); //reposition then does setOptions a second time to fix changed positions
}

//Constructor to make the menu
void DropMenu::make(int x, int y, const std::string& text, 
           const Markup& markup1, const Markup& markup2,
           BackPanel panel, BackRule hrule)
{
  this->x0 = x;
  this->y0 = y;
  this->totallyEnable();
  this->markup1 = markup1;
  this->markup2 = markup2;
  this->panel = panel;
  this->hrule = hrule;
  
  setOptions(text);
  putInScreen(); //reposition then does setOptions a second time to fix changed positions
}

void DropMenu::clearOptions()
{
  menuButton.clear();
  separator.clear();
  identity.clear();
}

/*Initialize the menu, calculate all positions, make the buttons, ...
This function is called automaticly by the constructors*/
void DropMenu::setOptions(const std::string& text)
{
  clearOptions();
  
  unsigned long maxTextSize = 0; //the size of the largest text
  
  //calculate n, the number of elements
  unsigned long pos = 0;
  unsigned long n = 0;
  while(pos < text.length())
  {
    if(text[pos] == '|') n++;
    pos++;
  }
  n++;
  
  unsigned long i = 0;
  unsigned long t = 0;

  while(i < n)
  {
    std::string buttonText = "";

    while(text[t] != '|' && t < text.length())
    {
      buttonText += text[t];
      t++;
      if(text[t] == 0) break;
    }

    separator.push_back(false);
    if(buttonText.length() == 0) separator[i] = true;
    t++;
    
    Button b;
    b.makeText(x0 + 2, y0 + 2 + (markup1.getHeight() + 2) * i, buttonText);
    menuButton.push_back(b);
    
    identity.push_back(0); //no useful identity added, but it has to be increased in size to match menuButton size
    
    if(buttonText.length() > maxTextSize) maxTextSize = buttonText.length();
    
    i++;
  }
  
   
  setSizex(maxTextSize * markup1.getWidth() + markup1.getWidth() / 2);
  setSizey(n * (markup1.getHeight() + 2) + markup1.getHeight() / 2);
}

void DropMenu::addOption(const std::string& text, int id)
{
    Button b;
    b.makeText(x0 + 2, y0 + 2 + (markup1.getHeight() + 2) * menuButton.size(), text);
    menuButton.push_back(b);
    separator.push_back(false);
    
    addSubElement(&b);//TODO: fix this big memory corruption problem and Button copying
    
    int separatorsize = 0;
    for(unsigned long i = 0; i < separator.size(); i++) if(separator[i]) separatorsize++;
    
    setSizey((menuButton.size() + separatorsize) * (markup1.getHeight() + 2) + markup1.getHeight() / 2);

    //int newSizex = text.length() * markup1.getWidth() + markup1.getWidth() / 2;
    int newSizex;// = text.length();
    int textheight;
    getFormattedTextSize(text, newSizex, textheight, markup1);
    newSizex +=  markup1.getWidth() / 2;
    
    if(newSizex > getSizex()) setSizex(newSizex);
    
    identity.push_back(id);
}

void DropMenu::drawWidget() const
{
  panel.draw(x0, y0, getSizex(), getSizey());

  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i]) menuButton[i].draw();
    else hrule.draw(x0 + markup1.getWidth() / 2, y0 + 2 + (markup1.getHeight() + 2) * i + markup1.getHeight() / 2, getSizex() - markup1.getWidth());
  }
}

void DropMenu::handleWidget(const IGUIInput* input)
{
  if(autoDisable && input->mouseButtonDown(GUI_LMB) && !mouseDown(input)) totallyDisable();
}

/*
Returns which of the options was pressed 
Note: separaters take up a number spot too

It returns:

-1 if nothing clicked
i (= 0 or a positive integer) if option i was clicked

*/

int DropMenu::check(const IGUIInput* input)
{
  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i])
    {
      if(menuButton[i].clicked(input)) return i;
    }
  }
  return -1;
}

Button* DropMenu::getButton(int i)
{
  return &menuButton[i];
}

Button* DropMenu::getButton(const std::string& name)
{
  int i = 0;
  
  for(int j = 0; j < getNumButtons(); j++) if(menuButton[j].text == name) i = j;
  
  return &menuButton[i];
}


std::string DropMenu::checkText(const IGUIInput* input)
{
  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i])
    {
      if(menuButton[i].clicked(input)) return menuButton[i].text;
    }
  }
  return "";
}

int DropMenu::checkIdentity(const IGUIInput* input)
{
  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i])
    {
      if(menuButton[i].clicked(input)) return identity[i];
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//GUIDROPLIST///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The DropList class

this class contains a list of options that can be opened or closed with a button,
the list will have a scrollbar and allow you to choose an option with the mouse
*/

//Constructor to make an empty droplist
Droplist::Droplist()
{
  this->x0 = 0;
  this->y0 = 0;
  this->markup1 = TS_White;
  this->markup2 = TS_White;
  
  addSubElement(&listButton);
  addSubElement(&bar);
  
  init("", -1);
}

void Droplist::make(int x, int y, const std::string& text, int numVisibleOptions,
          const Markup& markup1, const Markup& markup2, const Markup& markup3,
          BackPanel topPanel, BackPanel listPanel,
          Texture* buttonTexture)
{
  this->topPanel = topPanel;
  this->listPanel = listPanel;
  this->markup1 = markup1;
  this->markup2 = markup2;
  this->markup3 = markup3;
  this->opened = 0;
  this->selected = 0;
  this->x0 = x;
  this->y0 = y;
  this->totallyEnable();
  
  int buttonWidth = buttonTexture->getU();
  int buttonHeight = buttonTexture->getV();
  
  sizeyc = buttonHeight;
  init(text, numVisibleOptions);
  sizexo += buttonWidth;
  sizexc = sizexo;
  
  this->setSizex(sizexc);
  this->setSizey(sizeyc);
  
  listButton.makeImage(x1 - buttonWidth, y0, buttonTexture, buttonTexture, buttonTexture, RGB_White, RGB_Grey, RGB_Grey);


  bar.makeVertical(x1 - buttonWidth, y0 + sizeyc, sizeyo - sizeyc,
                   100, 0, 0, 1,
                   &builtInGuiSet, 1);

}

void Droplist::makeScrollbar(const GuiSet* set)
{
  bar.makeVertical(x1 - set->arrowN->getU(), y0 + sizeyc, sizeyo - sizeyc,
                   100, 0, 0, 1,
                   set, 1);
}  

void Droplist::init(const std::string& text, int numVisibleOptions)
{
  textButton.clear();
  
  unsigned long maxTextSize = 0; //the size of the largest text
  
  //calculate n, the number of elements
  unsigned long pos = 0;
  unsigned long n = 0;
  while(pos < text.length())
  {
    if(text[pos] == '|') n++;
    pos++;
  }
  n++;
  
  unsigned long i = 0;
  unsigned long t = 0;

  while(i < n)
  {
    std::string buttonText = "";

    while(text[t] != '|' && t < text.length())
    {
      buttonText += text[t];
      t++;
    }
    t++;
    
    Button b;
    b.makeText(x0 + 2, y0 + sizeyc + 2 + (markup1.getHeight() + 2) * i, buttonText);
    textButton.push_back(b);
    
    if(buttonText.length() > maxTextSize) maxTextSize = buttonText.length();
    
    i++;
  }
  
  if(numVisibleOptions == -1) this->numVisibleOptions = numVisibleOptions = n;
  else this->numVisibleOptions = numVisibleOptions;
   
  sizexo = maxTextSize * markup1.getWidth() + markup1.getWidth() / 2 + bar.getSizex();
  sizeyo = sizeyc + numVisibleOptions * (markup1.getHeight() + 2) + markup1.getHeight() / 2;
  
  int buttonWidth = maxTextSize * markup1.getWidth();
  for(unsigned long i = 0; i < textButton.size(); i++)
  {
    textButton[i].setSizex(buttonWidth);
  }
}

void Droplist::addOption(const std::string& text)
{
  Button b;
  b.makeText(x0 + 2, y0 + sizeyc + 2 + (markup1.getHeight() + 2) * textButton.size(), text);
  textButton.push_back(b);
  addSubElement(&textButton.back()); //TODO: fix this big memory corruption problem and Button copying
  
  unsigned long maxTextSize = b.getSizex() / markup1.getWidth();
  if(text.length() > maxTextSize) maxTextSize = text.length();
  
  int buttonWidth = maxTextSize * markup1.getWidth();
  for(unsigned long i = 0; i < textButton.size(); i++)
  {
    textButton[i].setSizex(buttonWidth);
  }
  
  setSizex(maxTextSize * markup1.getWidth() + markup1.getWidth() / 2 + bar.getSizex());
  sizexo = sizexc = getSizex();
  
  bar.moveTo(x0 + sizexo - bar.getSizex(), bar.getY0());
  listButton.moveTo(x0 + sizexc - listButton.getSizex(), listButton.getY0());
}

void Droplist::drawWidget() const
{
  topPanel.draw(x0, y0, sizexc, sizeyc);
  listButton.draw();
  print(textButton[selected].text, x0, y0 + sizeyc / 2 - markup3.getHeight() / 2, markup3);

  if(opened)
  {
    listPanel.draw(x0, y0 + sizeyc, sizexo, sizeyo - sizeyc);
    for(unsigned long i = 0; i < textButton.size(); i++) textButton[i].draw();
    bar.draw();
  }
}

int Droplist::check()
{
  return selected;
}

std::string Droplist::checkText()
{
  return textButton[selected].text;
}

void Droplist::handleWidget(const IGUIInput* input)
{
  if(listButton.clicked(input) || (clicked(input) && mouseGetRelPosY(input) < sizeyc && !listButton.mouseOver(input))) //behind the or is to open it also if you press the thing itself but not the button, the getMouseY() < sizeyc is necessary, without it, the code after this will not work anymore (starting from if(openen)
  {
    if(!opened)
    {
      open();
    }
    else
    {
      close();
    }
  }
  
  if(opened)
  {
    for(unsigned long i = 0; i <  textButton.size(); i++)
    {
      if(textButton[i].clicked(input))
      {
        close();
        selected = i;
      }
    }
    
    if(mouseScrollUp(input) && !bar.mouseOver(input)) bar.scroll(input, -2);
    if(mouseScrollDown(input) && !bar.mouseOver(input)) bar.scroll(input, 2);
    bar.handle(input);
    scroll();
  }
}

void Droplist::scroll()
{
  int v = numVisibleOptions;
  int n = textButton.size();
  
  if(n <= v); //dan moet er ni gescrolld worden
  else
  {
    int s = int(((n - v) * bar.scrollPos) / bar.scrollSize);
    
    for(int i = 0; i < n; i++)
    {
      textButton[i].moveTo(textButton[i].getX0(), y0 + sizeyc + 2 + (markup1.getHeight() + 2) * (i - s));
      
      if(i >= s && i < s + v)
      {
        textButton[i].setVisible(1);
        textButton[i].setActive(1);
      }
      else
      {
        textButton[i].setVisible(0);
        textButton[i].setActive(0);
      }
    }
  }
}

void Droplist::open()
{
  opened = 1;
  setSizex(sizexo);
  setSizey(sizeyo);
}

void Droplist::close()
{
  opened = 0;
  setSizex(sizexc);
  setSizey(sizeyc);
}

////////////////////////////////////////////////////////////////////////////////
//GUIMATRIX/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Matrix::make(int x0, int y0, int x1, int y1, int numx, int numy)
{
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
  this->active = true;
  this->visible = true;
  this->numx = numx;
  this->numy = numy;
}

unsigned long Matrix::getTileX(const IGUIInput* input) const
{
  return mouseGetRelPosX(input) / (getSizex() / numx);
}

unsigned long Matrix::getTileY(const IGUIInput* input) const
{
  return mouseGetRelPosY(input) / (getSizey() / numy);
}

int Matrix::getScreenX(int tilex) const
{
  return (tilex * getSizex()) / numx + x0;
}

int Matrix::getScreenY(int tiley) const
{
  return (tiley * getSizey()) / numy + y0;
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
  this->active = true;
  this->visible = true;
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
  else return getSizex() / tileSizeX;
}

unsigned long Grid::getNumy() const
{
  if(tileSizeY == 0) return 0;
  else return getSizey() / tileSizeY;
}

unsigned long Grid::getNumElements() const
{
  return getNumx() * getNumy();
}

int Grid::getTileX(const IGUIInput* input) const
{
  return mouseGetRelPosX(input) / tileSizeX;
}

int Grid::getTileY(const IGUIInput* input) const
{
  return mouseGetRelPosY(input) / tileSizeY;
}

int Grid::getTile(const IGUIInput* input) const //returns index of the tile
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


void Grid::drawWidget() const
{
  if(showGrid)
  {
    for(int y = y0; y < y1; y += tileSizeY)
    for(int x = x0; x < x1; x += tileSizeX)
    {
      drawLine(x, y0, x, y1, gridColor);
      drawLine(x0, y, x1, y, gridColor);
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
  this->visible = 0;
  this->active = 0;
  this->stack.clear();
}

void Painter::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->color = color;
  this->stack.clear();
  this->totallyEnable();
}

void Painter::drawWidget() const
{
  //first draw the rectangle if the alpha channel of color is > 0
  if(color.a > 0) drawRectangle(x0, y0, x1, y1, color);
  
  //then draw the queued elements, in order
  for(unsigned long i = 0; i < stack.size(); i++)
  {
    switch(stack[i].type)
    {
      case 0: 
        drawPoint(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 1: 
        drawLine(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].x1 + x0, stack[i].y1 + y0, stack[i].color);
        break;
      case 2: 
        stack[i].texture->draw(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 3: 
        stack[i].texture->drawCentered(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 4: 
        drawRectangle(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].x1 + x0, stack[i].y1 + y0, stack[i].color);
        break;
      case 5:
        print(stack[i].text, stack[i].x0 + x0, stack[i].y0 + y0, stack[i].textMarkup);
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

void Painter::queueTexture(int x, int y, Texture* texture, const ColorRGB& colorMod)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].color = colorMod;
  stack[i].texture = texture;
  stack[i].type = 2;
}

void Painter::queueTextureCentered(int x, int y, Texture* texture, const ColorRGB& colorMod)
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

void Painter::queueText(int x, int y, const std::string& text, const Markup& markup)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].x1 = x;
  stack[i].y1 = y;
  stack[i].text = text;
  stack[i].textMarkup = markup;
  stack[i].type = 5;
}

////////////////////////////////////////////////////////////////////////////////
//YESNOWINDOW///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

YesNoWindow::YesNoWindow()
{
  this->active = 0;
  this->visible = 0;
}

void YesNoWindow::make(int x, int y, int sizex, int sizey, const std::string& text)
{
  Window::make(x, y , sizex, sizey);
  addTop();
  
  yes.makeTextPanel(0, 0, "Yes");
  no.makeTextPanel(0, 0, "No");
  //yes.autoTextSize();
  //no.autoTextSize();
  
  message.make(0, 0, text);
  
  pushTopAt(&message, 16, 16);
  int centerx = getSizex() / 2;
  pushTopAt(&yes, centerx - yes.getSizex() - 16, getSizey() - yes.getSizey() - 8 - 16);
  pushTopAt(&no, centerx + 16, getSizey() - no.getSizey() - 8 - 16);
  
  this->active = 1;
  this->visible = 1;
}

////////////////////////////////////////////////////////////////////////////////
//OKWINDOW//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

OkWindow::OkWindow()
{
  this->active = 0;
  this->visible = 0;
}

void OkWindow::make(int x, int y, int sizex, int sizey, const std::string& text)
{
  Window::make(x, y , sizex, sizey);
  addTop();
  
  ok.makeTextPanel(0, 0, "Ok");
  //ok.autoTextSize();
  
  message.make(0, 0, text);
  
  pushTopAt(&message, 16, 16);
  int centerx = getSizex() / 2;
  pushTopAt(&ok, centerx - ok.getSizex() / 2, getSizey() - ok.getSizey() - 8 - 16);
  
  this->active = 1;
  this->visible = 1;
}
  
////////////////////////////////////////////////////////////////////////////////
//GUICANVAS/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Canvas: allows you to draw on it with the mouse
*/

Canvas::Canvas()
{
  this->visible = 0;
  this->active = 0;
}

void Canvas::make(int x, int y, int sizex, int sizey, const ColorRGB& backColor, int border, const ColorRGB& leftColor, const ColorRGB& rightColor, double size, double hardness, double opacity)
{
  this->visible = true;
  this->active = true;
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
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
  canvas.create(backColor, getSizex(), getSizey());
}

void Canvas::handleWidget(const IGUIInput* input)
{
  if(mouseGrabbed(input, GUI_LMB) || mouseGrabbed(input, GUI_RMB))
  {
    ColorRGB drawColor;
    
    if(input->mouseButtonDown(GUI_LMB)) drawColor = leftColor;
    else drawColor = rightColor;
    
    int drawx = mouseGetRelPosX(input);
    int drawy = mouseGetRelPosY(input);

    if(validOldMousePos == true)
    {
      drawLine(&canvas.getOpenGLBuffer()[0], canvas.getU2(), canvas.getV2(), oldMouseX, oldMouseY, drawx, drawy, drawColor, border, border, getSizex() - border, getSizey() - border);
      canvas.upload();
    }
    oldMouseX = drawx;
    oldMouseY = drawy;
    validOldMousePos = true;
  }
  else validOldMousePos = false;
}

void Canvas::drawWidget() const
{
  canvas.draw(x0, y0);
}

void Canvas::clear()
{
  canvas.create(backColor, getSizex(), getSizey());
}

////////////////////////////////////////////////////////////////////////////////
//GUIRECTANGLE//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Rectangle is a simple rectangle but as child class of Element
*/

Rectangle::Rectangle()
{
  this->visible = 0;
  this->active = 0;
  this->color = RGB_Black;
}

void Rectangle::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->color = color;
  this->totallyEnable();
}

void Rectangle::drawWidget() const
{
  drawRectangle(x0, y0, x1, y1, color);
}

////////////////////////////////////////////////////////////////////////////////
//GUILINE///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Line is a simple line but as child class of Element
*/

Line::Line()
{
  this->visible = 0;
  this->active = 0;
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
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->color = color;
  this->totallyEnable();
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

void Line::drawWidget() const
{
  drawLine(lx0, ly0, lx1, ly1, color);
}

} //namespace gui
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////

