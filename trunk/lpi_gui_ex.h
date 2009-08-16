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


/*
extra GUI elements such as menus, canvas, ...

NOTE: some of these elements are currently broken due to refactoring and will be fixed later

TODO: fix menu and other such elements
*/

#ifndef LPI_GUI_EX_H_INCLUDED
#define LPI_GUI_EX_H_INCLUDED

#include "lpi_gui.h"


namespace lpi
{
namespace gui
{

class DropMenu : public Element
{
  private:
    std::vector<Button> menuButton;
    std::vector<bool> separator;
    
    /*
    identity:
    unique identifier (number) for each option, which can optionally be used to check options,
    for example in an rpg game with a drop menu for items in inventory, option "drop item" can
    always get number 5, option "use" can get number 7 for example, and then no matter at
    what location in the menu the "use" option is, or no matter what alternative name (e.g.
    "drink") the use option has, you can still check if it was pressed by using checkIdentity()
    
    identities can only be set to useful values if you use the addOption function to add options
    one by one, the setOptions function doesn't fill in useful identity values but only 0
    
    negative identities are most certainly allowed
    */
    std::vector<int> identity;
    
  public:
    //std::string text; //the menu texts, sorted, with | chars between them, and closed with a final NULL char
    
    void setOptions(const std::string& text);
    void clearOptions();
    
    //text styles
    Markup markup1;
    Markup markup2;
    
    BackPanel panel;
    BackRule hrule; //for the separators
    
    DropMenu();
    void makeColored(int x, int y, const std::string& text,
                     const Markup& markup1 = TS_White, const Markup& markup2 = TS_White,
                     const ColorRGB& menuColor = RGB_Grey, BackRule hrule = DEFAULTHRULE); //make with simple color panel
    void make(int x, int y, const std::string& text,
              const Markup& markup1 = TS_White, const Markup& markup2 = TS_White,
              BackPanel panel = DEFAULTPANEL, BackRule hrule = DEFAULTHRULE); //make with given panel    
    virtual void drawWidget(IGUIDrawer& drawer) const;
    virtual void handleWidget(const IGUIInput& input);
    int check(const IGUIInput& input);
    Button* getButton(int i);
    Button* getButton(const std::string& name);
    int getNumButtons() const { return menuButton.size(); }
    std::string checkText(const IGUIInput& input);
    /*
    returns 0 if none, or a button where no identity was set, is pressed, or
    the identity of the button if one with an identity was pressed
    */
    int checkIdentity(const IGUIInput& input);
    void addOption(const std::string& text, int id = 0); //id is an optional identity
    
    bool autoDisable; //if true, the menu will totallyDisable itself if you click anywhere not on the menu
};



class Droplist : public Element
{
  private:
    std::vector<Button> textButton;
    void init(const std::string& text, int numVisibleOptions);
    
    int sizexc; //width when closed = width to be able to contain widest text in the list
    int sizexo; //width when open = width of widest text in the list
    int sizeyc; //height when closed
    int sizeyo; //height when open
    
    void open();
    void close();
  public:
    //std::string text; //the menu texts, sorted, with | chars between them, and closed with a final NULL char
    
    //text styles
    Markup markup1; //Markup for a textbutton of the selection list
    Markup markup2; //textbutton mouseOver
    Markup markup3; //the selected one on top (instead of in the list)
    
    BackPanel topPanel; //color of the top where the selected text is
    BackPanel listPanel; //color of the selection list
    
    int numVisibleOptions; //in the selectionlist
    bool opened; //true if the selectionlist is visible
    
    Droplist();
    Scrollbar bar;
    Button listButton; //the button to open and close the list
    
    void scroll();
    
    int selected;//the selected option of the list (it's index)
    
    void make(int x, int y, const std::string& text, int numVisibleOptions = -1,
              const Markup& markup1 = TS_Black, const Markup& markup2 = TS_Red, const Markup& markup3 = TS_Shadow,
              //BackPanel topPanel = COLORPANEL(RGB_Grey), BackPanel listPanel = COLORPANEL(RGB_White),
              Texture* buttonTexture = &builtInTexture[28]);
    void makeScrollbar(const GuiSet* set = &builtInGuiSet);
    virtual void drawWidget(IGUIDrawer& drawer) const;
    int check();
    std::string checkText();
    virtual void handleWidget(const IGUIInput& input);

    void addOption(const std::string& text);
};

/*
An invisible grid of numx * numy rectangles, it can return over which square the mouse is (top left one has coordinates 0, 0)
*/
class Matrix : public Element
{
  //nothing required in here! it's a Element!
  public:
  void make(int x0, int y0, int x1, int y1, int numx, int numy);
  
  unsigned long numx;
  unsigned long numy;
  
  unsigned long getTileX(const IGUIInput& input) const;
  unsigned long getTileY(const IGUIInput& input) const;
  
  int getScreenX(int tilex) const;
  int getScreenY(int tiley) const;
};

/*
similar to Matrix, except here the size of the rectangles is constant, and not the number of them (if the gui element resizes)
*/
class Grid : public Element
{
  //nothing required in here! it's a Element!
  public:
  void make(int x0, int y0, int x1, int y1, int tileSizeX, int tileSizeY);
  
  void setNumTiles(int amount); //real amount can be larger, as the width of rows will stay the same, it'll add rows at the bottom or remove rows
  
  Grid();
  
  //returns x position on screen of center of given tile
  int getTileCenterx(int index) const; //index = index of the tile
  
  //returns y position on screen of center of given tile
  int getTileCentery(int index) const; //index = index of the tile
  
  bool showGrid;
  ColorRGB gridColor;
  
  virtual void drawWidget(IGUIDrawer& drawer) const;
  
  int tileSizeX;
  int tileSizeY;
  
  unsigned long getNumx() const;
  unsigned long getNumy() const;
  
  unsigned long getNumElements() const;
  
  int getTileX(const IGUIInput& input) const;
  int getTileY(const IGUIInput& input) const;
  int getTile(const IGUIInput& input) const; //returns index of the tile
  
  int getScreenX(int tilex) const;
  int getScreenY(int tiley) const;
};

struct PainterStack
{
  int x0;
  int y0;
  int x1;
  int y1;
  ColorRGB color;
  Texture* texture;
  std::string text;
  Markup textMarkup;
  int type; //0: point, 1: line, 2: texture (coord = top left), 3: centered texture, 4: text
};

class Painter : public Element
{
  public:
    ColorRGB color;
    virtual void drawWidget(IGUIDrawer& drawer) const;
    Painter();
    void make(int x, int y, int sizex = 200, int sizey = 150, const ColorRGB& color = RGB_Invisible);
    
    void queuePoint(int x, int y, const ColorRGB& color = RGB_White);
    void queueLine(int x0, int y0, int x1, int y1, const ColorRGB& color = RGB_White);
    void queueRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color = RGB_White);
    void queueTexture(int x, int y, Texture* texture, const ColorRGB& colorMod = RGB_White);
    void queueTextureCentered(int x, int y, Texture* texture, const ColorRGB& colorMod = RGB_White);
    void queueText(int x, int y, const std::string& text, const Markup& markup);
    
  private:
    mutable std::vector<PainterStack> stack; //mutable because it's emptied everytime after drawing
};

//message boxes
class YesNoWindow : public Window
{
  public:
  Button yes;
  Button no;
  Text message;
  
  YesNoWindow();
  void make(int x, int y, int sizex, int sizey, const std::string& text);
};

class OkWindow : public Window
{
  public:
  Button ok;
  Text message;
  
  OkWindow();
  void make(int x, int y, int sizex, int sizey, const std::string& text);
};

//a painting canvas that allows you to paint with the mouse --> TODO: make this more general usable as per-pixel plotting tool
class Canvas : public Element
{
  private:
    void init();
    int oldMouseX;
    int oldMouseY;
    bool validOldMousePos;
  
  public:
    ColorRGB leftColor; //color of the brush for left mouse button
    ColorRGB rightColor; //color of the brush for right mouse button
    ColorRGB backColor; //the initial background texture of the canvas
    double size; //size of the brush in pixels (it's the radius (not diameter) when the brush is round)
    double hardness; //hardness of the brush from 0.0 to 1.0
    double opacity; //opacity of the brush from 0.0 to 1.0
    
    Texture canvas; //the OpenGL canvas texture (gets updated with canvasData all the time)
    
    int border;
    
    virtual void drawWidget(IGUIDrawer& drawer) const;
    virtual void handleWidget(const IGUIInput& input);
    
    void clear(); //clear the whole texture to backColor, deleting it's previous contents
    
    Canvas();
    void make(int x, int y, int sizex, int sizey, const ColorRGB& backColor = RGB_White, int border = 0, const ColorRGB& leftColor = RGB_Black, const ColorRGB& rightColor = RGB_Red, double size = 1.0, double hardness = 1.0, double opacity = 1.0);
    void setBrush(const ColorRGB& leftColor = RGB_Black, const ColorRGB& rightColor = RGB_Red, double size = 1.0, double hardness = 1.0, double opacity = 1.0);
};


template<typename T>
class Variable : public Element //can be anything the typename is: integer, float, ..., as long as it can be given to a stringstream
{
  public:
    T v;
    std::string label;
    Markup markup;
    
    virtual void drawWidget(IGUIDrawer& drawer) const
    {
      print(label, x0, y0, markup);
      print(v, x0 + label.length() * markup.getWidth(), y0, markup);
    }
    
    Variable() { totallyDisable(); }
    
    void make(int x, int y, T v, const std::string& label="", const Markup& markup = TS_W)
    {
      this->x0 = x;
      this->y0 = y;
      //for now, only the length of the label + 1 is used
      this->setSizex((label.length() + 1) * markup.getWidth());
      this->setSizey(markup.getHeight());
      this->v = v;
      this->label = label;
      this->markup = markup;
      this->visible = 1;
      this->active = 1;
    }
    
    void setValue(T v) { this->v = v; }
    
    T& getValue() { return this->v; }
    
    const T& getValue() const { return this->v; }
};

class Rectangle : public Element
{
  public:
    ColorRGB color;
    virtual void drawWidget(IGUIDrawer& drawer) const;
    Rectangle();
    void make(int x, int y, int sizex=64, int sizey=64, const ColorRGB& color = RGB_Grey);
};

class Line : public Element
{
  public:
    ColorRGB color;
    virtual void drawWidget(IGUIDrawer& drawer) const;
    Line();
    int lx0;
    int ly0;
    int lx1;
    int ly1;
    void make(int x, int y, int sizex=64, int sizey=64, const ColorRGB& color = RGB_Grey);
    void setEndpoints(int x0, int y0, int x1, int y1);
};

} //namespace gui
} //namespace lpi

#endif