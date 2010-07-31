/*
Copyright (c) 2005-2010 Lode Vandevenne
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

class IFileBrowse;

namespace gui
{

struct FileDialogPersist;

class InternalList : public Element
{
  /*
  TODO: ability to set a custom font for every separate item
  */
  
  private:
    bool allowMultiSelection;
    bool hasIcons; //is false initially and after using clear, but becomes true once any item is given a valid icon
    
    std::vector<std::string> items;
    std::vector<HTexture*> icons;
    std::vector<bool> selection; //used if allowMultiSelection is true
    size_t selectedItem; //used if allowMultiSelection is false
    size_t lastClickedItem; //used for multiselection with shift key
    
    MouseState mouse_state_for_selection;
    
  protected:
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    
  public:
  
    InternalList();
    size_t getItemHeight() const;
    size_t getNumItems() const;
    size_t getSelectedItem() const; //works only correctly if allowMultiSelection is false. Returns value larger than getNumItems if no item at all is selected.
    bool isSelected(size_t i) const; //works both if allowMultiSelection is true or false
    void setSelected(size_t i, bool selected = true); //selects this item. Others get deselected if allowMultiSelection is false.
    void deselectAll();
    const std::string& getValue(size_t i) const; //the value is tje display name of the item
    void setValue(size_t i, const std::string& value);
    void setIcon(size_t i, HTexture* icon);
    void addItem(const std::string& value);
    void addItem(const std::string& value, HTexture* icon);
    void insertItem(size_t i, const std::string& value);
    void removeItem(size_t i);
    void setAllowMultiSelection(bool set);
    void clear();
    size_t getMouseItem(const IInput& input) const; //this returns the item over which the mouse is, which you can use together with checks like "doubleclicked" to determine if a certain item is being doubleclicked or whatever else you check. Returns invalid index if mouse is not over this list.
    void swap(size_t item1, size_t item2); //swapping location of two items, e.g. for sorting
    void drawPartial(IGUIDrawer& drawer, int vy0, int vy1) const; //draw only the texts thare are visible between the absolute coordinates vy0 and vy1 (for efficiency)
};

class List : public ScrollElement
{
  private:
    InternalList list;
    
  protected:
  
    virtual void resizeImpl(const Pos<int>& newPos);
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    
  public:
    
    List(const IGUIDrawer& geom);
    
    //wrapped from InternalList
    size_t getNumItems() const;
    size_t getSelectedItem() const; //works only correctly if allowMultiSelection is false. Returns value larger than getNumItems if no item at all is selected.
    bool isSelected(size_t i) const; //works both if allowMultiSelection is true or false
    void setSelected(size_t i, bool selected = true); //selects this item. Others get deselected if allowMultiSelection is false.
    void deselectAll();
    const std::string& getValue(size_t i) const; //the value is tje display name of the item
    void setValue(size_t i, const std::string& value);
    void setIcon(size_t i, HTexture* icon);
    void addItem(const std::string& value);
    void addItem(const std::string& value, HTexture* icon);
    void insertItem(size_t i, const std::string& value);
    void removeItem(size_t i);
    void setAllowMultiSelection(bool set);
    void clear();
    size_t getMouseItem(const IInput& input) const; //this returns the item over which the mouse is, which you can use together with checks like "doubleclicked" to determine if a certain item is being doubleclicked or whatever else you check. Returns invalid index if mouse is not over this list.
    void swap(size_t item1, size_t item2);
    
    bool clickedOnItem(const IInput& input); //when clicked on an item in the list (but not clicking on the scrollbar or so)
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
  
  unsigned long getTileX(const IInput& input) const;
  unsigned long getTileY(const IInput& input) const;
  
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
  
  virtual void drawImpl(IGUIDrawer& drawer) const;
  
  int tileSizeX;
  int tileSizeY;
  
  unsigned long getNumx() const;
  unsigned long getNumy() const;
  
  unsigned long getNumElements() const;
  
  int getTileX(const IInput& input) const;
  int getTileY(const IInput& input) const;
  int getTile(const IInput& input) const; //returns index of the tile
  
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
  ITexture* texture;
  std::string text;
  Font textFont;
  int type; //0: point, 1: line, 2: texture (coord = top left), 3: centered texture, 4: text
};

class Painter : public Element
{
  public:
    ColorRGB color;
    virtual void drawImpl(IGUIDrawer& drawer) const;
    Painter();
    void make(int x, int y, int sizex = 200, int sizey = 150, const ColorRGB& color = RGB_Invisible);
    
    void queuePoint(int x, int y, const ColorRGB& color = RGB_White);
    void queueLine(int x0, int y0, int x1, int y1, const ColorRGB& color = RGB_White);
    void queueRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color = RGB_White);
    void queueTexture(int x, int y, ITexture* texture, const ColorRGB& colorMod = RGB_White);
    void queueTextureCentered(int x, int y, ITexture* texture, const ColorRGB& colorMod = RGB_White);
    void queueText(int x, int y, const std::string& text, const Font& font);
    
  private:
    mutable std::vector<PainterStack> stack; //mutable because it's emptied everytime after drawing
};

//message boxes
class MessageBox : public Dialog
{
  protected:
    Button ok;
    std::string text;

  protected:
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;

  public:

    MessageBox(const IGUIDrawer& geom, const std::string& text, const std::string& title = "");
    virtual bool done() const;
    virtual Result getResult() const;

};

class YesNoWindow : public Dialog
{
  protected:
    Button yes;
    Button no;
    Text message;
    bool value;

  protected:
    virtual void handleImpl(const IInput& input);

  public:
    YesNoWindow(const IGUIDrawer& geom, const std::string& text, const std::string& title = "");
    virtual bool done() const;
    virtual Result getResult() const; //returns OK or CANCEL (since there's no cancel button, it's always OK)
    bool getValue() const; //returns true for yes, false for no
    void setButtonTexts(const std::string& textYes, const std::string& textNo);
};

//a painting canvas that allows you to paint with the mouse
//TODO: make this more general by removing the mouse-painting and brushes and allowing the user to implement that
//TODO: with the painting brush, this is more a "demo" or "sample" widget, maybe create a "lpi_gui_demo.h" and put it in there?
class Canvas : public Element
{
  private:
    void init();
    int oldMouseX;
    int oldMouseY;
    bool validOldMousePos;
    HTexture* canvas; //the canvas texture (gets updated with canvasData all the time)
    
  public:
    ColorRGB leftColor; //color of the brush for left mouse button
    ColorRGB rightColor; //color of the brush for right mouse button
    ColorRGB backColor; //the initial background texture of the canvas
    double size; //size of the brush in pixels (it's the radius (not diameter) when the brush is round)
    double hardness; //hardness of the brush from 0.0 to 1.0
    double opacity; //opacity of the brush from 0.0 to 1.0
    
    int border; //TODO: old, remove this
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    void clear(); //clear the whole texture to backColor, deleting it's previous contents
    
    Canvas(IGUIDrawer& drawer);
    ~Canvas();
    void make(int x, int y, int sizex, int sizey, const ColorRGB& backColor = RGB_White, int border = 0, const ColorRGB& leftColor = RGB_Black, const ColorRGB& rightColor = RGB_Black, double size = 1.0, double hardness = 1.0, double opacity = 1.0);
    void setBrush(const ColorRGB& leftColor = RGB_Black, const ColorRGB& rightColor = RGB_Black, double size = 1.0, double hardness = 1.0, double opacity = 1.0);
};


template<typename T>
class Variable : public Element //can be anything the typename is: integer, float, ..., as long as it can be given to a stringstream
{
  public:
    T v;
    std::string label;
    Font font;
    
    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      drawer.drawText(label, x0, y0, font);
      int labelw, labelh;
      drawer.calcTextRectSize(labelw, labelh, label, font);
      drawer.drawText(v, x0 + labelw, y0, font);
    }
    
    Variable() { setEnabled(false); }
    
    void make(int x, int y, T v, const std::string& label="", const Font& font = FONT_Default)
    {
      this->x0 = x;
      this->y0 = y;
      //for now, only the length of the label + 1 is used
      this->setSizex((label.length() + 1) * /*markup.getWidth()*/8); //TODO: use a drawer to determine text size
      this->setSizey(/*markup.getHeight()*/8);
      this->v = v;
      this->label = label;
      this->font = font;
      this->visible = 1;
      this->active = 1;
    }
    
    void setValue(const T& v) { this->v = v; }
    
    T& getValue() { return this->v; }
    
    const T& getValue() const { return this->v; }
};

template<typename T>
class PVariable : public Element //A bit similar purpose to Variable, but, uses a pointer and automatically displays the value (no "set" and "get" needed)
{
  public:
    Font font;
    
    PVariable()
    : value(0)
    {
      this->enabled = 0;
    }

    void make(int x, int y, T* value, const Font& font = FONT_Default)
    {
      this->x0 = x;
      this->y0 = y;
      this->setSizeX(16 * /*markup.getWidth()*/8); //TODO: use a drawer to determine text size
      this->setSizeY(/*markup.getHeight()*/8);
      this->value = value;
      this->font = font;
      this->setEnabled(true);
    }

    void drawImpl(IGUIDrawer& drawer) const
    {
      if(value) drawer.drawText(valtostr(*value), x0, y0, font);
    }
    
    void setValue(T* value) { this->value = value; }

  private:
    T* value;
};

class Rectangle : public Element
{
  public:
    ColorRGB color;
    virtual void drawImpl(IGUIDrawer& drawer) const;
    Rectangle();
    void make(int x, int y, int sizex=64, int sizey=64, const ColorRGB& color = RGB_Grey);
};

class Line : public Element
{
  public:
    ColorRGB color;
    virtual void drawImpl(IGUIDrawer& drawer) const;
    Line();
    int lx0;
    int ly0;
    int lx1;
    int ly1;
    void make(int x, int y, int sizex=64, int sizey=64, const ColorRGB& color = RGB_Grey);
    void setEndpoints(int x0, int y0, int x1, int y1);
};

//this is one state of the NState
class NStateState
{
    public:
    NStateState();
    int textOffsetX;
    int textOffsetY;
    //bool downAndTested; //if mouse is down and that is already handled, leave this on so that it'll ignore mouse till it's back up
    void positionText(); //automaticly place the text a few pixels next to the checkbox, in the center
    
    ITexture* texture; //the texture of this state
    ColorRGB colorMod;
    
    bool enableText; //the text is a title drawn next to the checkbox, with automaticly calculated position
    std::string text;
    Font font;
    
    void make(ITexture* texture, const ColorRGB& colorMod, const std::string& text = "", const Font& font = FONT_Default);
};

//circle between N states (you can add states, the make function makes no states). Left mouse click goes to next state, right mouse click goes to previous state.
class NState : public Element, public Label
{
  private:
  public:
    unsigned long state;
    std::vector<NStateState> states;
    int toggleOnMouseUp;
        
    NState();
    void make(int x, int y, int toggleOnMouseUp = 0);
    void addState(ITexture* texture, const ColorRGB& colorMod = RGB_White, const std::string& text = "", const Font& font = FONT_Default);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
};

/*
Menu which can contain commands, submenu's and/or separators.
It does not keep any data or anything with commands, the user of this menu
is responsible for bookkeeping what items he hadded at which index.
Note: submenu's and separators get an index too. Everything you add has a
successive index. If you add command A, separator, command B, submenu, command C,
then A has index 0, B index 2 and C index 4.
To recreate the menu to change it, use clear and build everything again.
*/
class AMenu : public Element
{
  /*
  TODO: Icons in menus
  TODO: checkboxes in menu (maybe with the icons)
  */
  protected:
    
    enum Type
    {
      COMMAND,
      SUBMENU,
      SEPARATOR
    };
    
    struct Item
    {
      Type type;
      std::string name;
      AMenu* submenu;
    };
  
  protected:
    std::vector<Item> items;
    mutable size_t lastItem; //last item clicked on, if any
    bool stay; //if true, menu stays no matter where you click, if false the menu disappears again if you choose command or click outside of it (typically true for horizontal, false for vertical)
    size_t openedsubmenu;
    AMenu* parent;
    
  protected:
  
    virtual size_t getMouseIndex(const IInput& input) const = 0; //index of item over which the mouse is (depends on implementation, horizontal or vertical menu...)
    virtual void onAddItem(const IGUIDrawer& geom) = 0;
    virtual void onClear() = 0;
    virtual void manageHoverImpl(IHoverManager& hover);
    virtual void onOpenSubMenu(const IInput& input, size_t index) = 0;
    
    virtual void handleImpl(const IInput& input);
    
    void setParent(AMenu* parent) { this->parent = parent; }
    
    void disableParents(); //disables all parents that have "stay" to false, it stops disabling once a parent with stay = true is reached
    
  public:
    AMenu();
    
    size_t addCommand(const std::string& name, const IGUIDrawer& geom);
    size_t addSubMenu(AMenu* submenu, const std::string& name, const IGUIDrawer& geom);
    size_t addSeparator(const IGUIDrawer& geom);
    void clear();
    
    size_t getNumItems() const;
    bool itemClicked(size_t i, const IInput& input) const; //returns whether item with given index is clicked
    size_t itemClicked(const IInput& input) const; //returns index of item that is clicked if some item is clicked, an invalid index (>= getNumItems()) otherwise.
    
    int findSubMenu(const std::string& name); //returns -1 if no such submenu is found
    AMenu* getSubMenu(size_t index); //returns null pointer if there's no submenu at that index
    
    void disableMenu();
};

class MenuHorizontal : public AMenu
{
  protected:
  
    std::vector<int> sizes;
    std::vector<int> positions;
  
  protected:
  
    virtual size_t getMouseIndex(const IInput& input) const;
    virtual void onAddItem(const IGUIDrawer& geom);
    virtual void onClear();
    virtual void onOpenSubMenu(const IInput& input, size_t index);

    virtual void drawImpl(IGUIDrawer& drawer) const;
    
  public:

    MenuHorizontal();
};

class MenuVertical : public AMenu
{
  protected:
  
    std::vector<int> sizes;
    std::vector<int> positions;
  
  protected:
  
    virtual size_t getMouseIndex(const IInput& input) const;
    virtual void onAddItem(const IGUIDrawer& geom);
    virtual void onClear();
    virtual void onOpenSubMenu(const IInput& input, size_t index);
  
    virtual void drawImpl(IGUIDrawer& drawer) const;
    
  public:

    MenuVertical();
};

class ToolBar : public ElementComposite
{
  protected:
  
    mutable size_t lastItem[NUM_MOUSE_BUTTONS];
  
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    size_t getMouseIndex(const IInput& input) const; //index of item over which the mouse is
    
    enum Type
    {
      COMMAND,
      TOGGLE,
      SEPARATOR
    };

    struct Item
    {
      Type type;
      std::string tooltip;
      HTexture* icon;
      bool toggle; //for TOGGLE mode
    };

    std::vector<Item> items;
    
  public:
  
    ToolBar();
    size_t addCommand(HTexture* icon, const std::string& tooltip);
    size_t addToggle(HTexture* icon, const std::string& tooltip, bool enabled);
    size_t addSeparator();
    void clear();
    
    size_t getNumItems() const;
    bool itemClicked(size_t i, const IInput& input, MouseButton button = LMB) const; //returns whether item with given index is clicked
    size_t itemClicked(const IInput& input, MouseButton button = LMB) const; //returns index of item that is clicked if some item is clicked, an invalid index (>= getNumItems()) otherwise.
    
    bool toggleEnabled(size_t i) const;
    void setToggle(size_t i, bool enabled);
    
    virtual void drawToolTip(IGUIDrawer& drawer) const;
};

class DropDownList : public Element
{
  protected:
    class MyList : public List
    {
      protected:
        virtual void drawImpl(IGUIDrawer& drawer) const
        {
          drawer.drawRectangle(x0, y0, x1, y1, RGB_White, true);
          drawer.drawRectangle(x0, y0, x1, y1, RGB_Black, false);
          List::drawImpl(drawer);
        }
        
      public:
        MyList(const IGUIDrawer& geom)
        : List(geom)
        {
        }
    };
    MyList list; //the hovering list when opened
    
    bool changed;
    size_t listvalue; //used while changed = true. Reason for using this and not getting it from list directly: auto-updating (in both direction) dynamic GUI page with enum overwrites value before each handle

  protected:
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);

  public:
    DropDownList(const IGUIDrawer& geom);
    
    void setItems(const std::vector<std::string>& items);
    void addItem(const std::string& item);
    size_t getSelectedItem() const;
    void setSelectedItem(size_t i);
    
    size_t getNumItems() const;
    const std::string& getValue(size_t i) const;
    
    bool hasChanged(); //the first one who calls this after the change gets true, then it resets back to false

};

class ProgressBarDialog : public lpi::gui::Dialog
{
  private:
    Button cancel;
    double progress;
    bool isdone;
    Dummy bar;
  
  public:
  
    ProgressBarDialog( const IGUIDrawer& geom);
    virtual void handleImpl(const lpi::IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual bool done() const;
    virtual void setProgress(double val);
};

//Utility functions to quickly show and use modal dialogs

void showMessageBox(MainContainer& c, IModalFrameHandler& frame, const std::string& text, const std::string& title = "");
//current_path is the folder the file dialog should show initially. If you give empty string, then something is chosen for you instead.
lpi::gui::Dialog::Result getFileNameModal(MainContainer& c, IModalFrameHandler& frame, IFileBrowse* browser, std::string& filename, const std::string& current_path, bool save, FileDialogPersist* persist);
lpi::gui::Dialog::Result getFileNamesModal(MainContainer& c, IModalFrameHandler& frame, IFileBrowse* browser, std::vector<std::string>& filenames, const std::string& current_path, FileDialogPersist* persist);
class FileDialog;
lpi::gui::Dialog::Result getFileNameModal(MainContainer& c, IModalFrameHandler& frame, FileDialog& dialog, std::string& filename);
lpi::gui::Dialog::Result getFileNamesModal(MainContainer& c, IModalFrameHandler& frame, FileDialog& dialog, std::vector<std::string>& filenames);
bool getYesNoModal(MainContainer& c, IModalFrameHandler& frame, const std::string& question);
bool getYesNoModal(MainContainer& c, IModalFrameHandler& frame, const std::string& question, const std::string& textYes, const std::string& textNo);


} //namespace gui
} //namespace lpi

#endif
