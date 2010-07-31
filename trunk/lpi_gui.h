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


/*
lpi_gui: an OpenGL GUI. Well actually now it is updated to support any lpi::IDrawer2D, so it can use OpenGL or something else.
*/

#pragma once

#include <map>

#include "lpi_gui_base.h"

#include "lpi_color.h"
#include "lpi_texture.h"

#include "lpi_gui_drawer.h"

/*
TODO:
[X] modal vs modeless dialogs or elements
[ ] progress bar (and a good multilevel one)
[X] menu (both menu bar and right click popup menus)
[ ] minimizable windows (and bar to dock them in?)
[ ] maximizable windows
[X] spinner (value with up and down button)
[X] dropdown list
[ ] tree (and flat list and table)
[X] file dialog
[X] toolbar (just a bunch of image buttons, not hard to make)
[ ] textfield with multiple lines
[X] status bar in windows

Hint:
When executing the GUI in a gameloop, when you have put all elements in a MainContainer, you have to
call each frame both "handle" and "draw" on the container. While it doesn't really matter in what order you call them,
it works best if you do "handle" first and then immediatly "draw", because then the graphics are shown to the
user one frame sooner than if you'd do the inverse. The more things you do between "handle" and "draw", the more
there will appear mouse lag, but if you call them immediatly after each other there won't be any mouse lag in the GUI.
So the best order is:
-*beginning of frame*
-pause a few milliseconds to give the CPU something else to do
-clear screen (so that all new things can be drawn on it)
-calculations of the game or program (including drawing game or program related things except the gui)
-handle lpi gui
-draw lpi gui
-redraw screen (that is, make all changes of this frame visible)
-*end of frame*
*/

namespace lpi
{
namespace gui
{

class Element;

class ToolTipManager //this is made to draw the tooltip at the very end to avoid other gui items to be drawn over it
{
  private:
    std::map<const Element*, std::string> elements;
    
    
  public:
    void registerElement(Element* element, const std::string& tip); //doing this overrides the tooltip the element itself generates, but usually the element itself won't generate any tooltip at all on its own and using this function of the tooltipmanager is the only way to get a tooltip for that element
    void draw(const Element* root, IGUIDrawer& drawer) const;
    
    static void drawToolTip(const std::string& tip, IGUIDrawer& drawer);
};

/*
Hovering elements can be caused by any GUI Element. Examples are:
-menu's (regular menus, which give a popup when opened)
-dropdown lists: the dropped down part
-opened colorchooser in dynamic page
-...

This allows GUI elements to generate something that appears in front of everything
else, no matter how much levels higher or lower it is in the tree. But, beware,
to avoid resulting in a messy screen, not too many elements should have a hovering
element open at the same time. Typically, if you click the mouse outside of one hovering
panel, it should disappear. If you make a GUI element that doesn't do that, there
could be created many hovering panels and then all usefullnes is lost since only
one can be at the top and there's only so much screenspace.

The effect is reached by drawing these elements after all the others, and by
handling them without any other elements going over them to prevent blocking mouseOver.
*/
class IHoverManager
{
  public:
    virtual void addHoverElement(Element* element) = 0;
};

class AInternalContainer //container inside elements, for elements that contain sub elements to work (e.g. scrollbar exists out of background and 3 buttons)
{
  protected:
    std::vector<Element*> elements;
    bool elementOver;
  
  public:
  
    AInternalContainer();
  
    const std::vector<Element*>& getElements() const { return elements; }
    std::vector<Element*>& getElements() { return elements; }
    
    virtual void resize(const Pos<int>& oldPos, const Pos<int>& newPos) = 0; //this resizes the 2D size of elements, not the amount of elements
    void move(int x, int  y);
    void setElementOver(bool state); //this says to all elements whether or not another element is in front of it in Z order, causing mouse to not work
    void manageHover(IHoverManager& hover);
    void setBasicMouseInfo(const IInput& input);
    const Element* hitTest(const IInput& input) const;
    
    Element* getElement(size_t i) const { return elements[i]; }
    
    size_t findElementIndex(Element* element);
    
    int getKeyboardFocus() const;
};

class InternalContainer : public AInternalContainer //container inside elements, for elements that contain sub elements to work (e.g. scrollbar exists out of background and 3 buttons)
{
  private:
    std::map<Element*, Sticky> sticky;
  
  public:
  
    
    virtual void resize(const Pos<int>& oldPos, const Pos<int>& newPos); //this resizes the 2D size of elements, not the amount of elements

    void addSubElement(Element* element, const Sticky& sticky, Element* parent);
    void insertSubElement(size_t index, Element* element, const Sticky& sticky, Element* parent);
    void clearSubElements();
    
    Sticky getSticky(Element* element) const;
    void setSticky(Element* element, const Sticky& sticky, Element* parent);
    void setStickyRelative(Element* element, Element* parent); //automatically calculates the sticky for completely relative position, based on the current sizes
    void setStickyFull(Element* element, Element* parent); //automatically calculates the sticky for "full" behaviour, sides will follow parent's corresponding size, compared to current position

    void removeElement(Element* element);
  
  private:
    void initSubElement(Element* element, const Sticky& sticky, Element* parent);
    void setStickyElementSize(Element* element, Element* parent);
    void setStickyElementSize(Element* element, const Pos<int>& newPos);
};

class InternalContainerWrapping : public AInternalContainer //container inside elements, for elements that contain sub elements to work (e.g. scrollbar exists out of background and 3 buttons)
{
  
  public:
  
    virtual void resize(const Pos<int>& oldPos, const Pos<int>& newPos); //this resizes the 2D size of elements, not the amount of elements

    void addSubElement(Element* element);
    void clearSubElements();
    
    void removeElement(Element* element);
};

enum KeyMod
{
  /*
  An intention is for example that if a GUI component says it has focus for KM_TEXT,
  but not for KM_CTRL, that then a higher up component that wants to use a CTRL+letter shortcut,
  will perform that shortcut action, because the lower element which has the focus doesn't use
  that combination.
  */
  
  //nothing (0) means no focus
  KM_TEXT = 1, //regular typing (letters, arrows to move cursor, ...), also shift, alt gr and enter, insert, del, ... (because those are for typing text too)
  KM_CTRL = 2, //this is supposed to take over the letters (ctrl+letter is not same as typing that letter)
  KM_ALT = 4, //same behaviour as ctrl, but for the alt key
  //not in the list: shift and alt+gr, because those are not for shortcuts but for typing alternate letters
  KM_F = 8, //function keys (F1-F12)
  KM_ESC = 16,
  KM_TAB = 32 //special because it might be used to change focus
};

class Element : public ElementRectangular
{
  private:
  
    MouseState mouse_state_for_containers; //for bookkeeping by containers that contain this element TODO: make container itself remember one per element
    
  protected:
    
    bool elementOver; //true if there is an element over this element, causing the mouse NOT to be over this one (Z-order related)
    bool enabled; //if false, the draw() and handle() functions don't do anything, and mouse checks return false. So then it's invisible, inactive, totally not present.

    ////minimum size
    virtual int getMinSizeX() { return 0; } //you can't resize this element to something smaller than this
    virtual int getMinSizeY() { return 0; }
    
  protected:
    void autoActivate(const IInput& input, MouseState& auto_activate_mouse_state, bool& control_active); //utility function used by text input controls, they need a member variable like "MouseState auto_activate_mouse_state" in them for this and a bool "control_active", and in their handleWidget, put, "autoActivate(input, auto_activate_mouse_state, control_active); if(!control_active) return;"
    
    virtual void drawImpl(IGUIDrawer& drawer) const = 0; //called by draw(), this one can be overloaded for each widget defined below
    virtual void handleImpl(const IInput& input);
    virtual void moveImpl(int x, int y); //Override this if you have subelements, unless you use addSubElement in ElementComposite.
    virtual void resizeImpl(const Pos<int>& newPos); //always called after resize, will resize the other elements to the correct size. Override this if you have subelements, unless you use addSubElement in ElementComposite. When resizeImpl is called, you can get the new size from newPos, while the old size is still in x0, y0, x1, y1 from this and will be set after resizeImpl is called.
    virtual void manageHoverImpl(IHoverManager& hover);

    /*
    drawGUIPart: convenient helper function to draw a sub-element with its size and mouseoverstate etc...,
    so that you don't always have to write the same code to create GUIPartMod, and give the 4 coordinates,
    over and over.
    */
    static void drawGUIPart(IGUIDrawer& drawer, const Element* element, GUIPart part);
    static void drawGUIPartColor(IGUIDrawer& drawer, const Element* element, GUIPart part, const ColorRGB& color);
    static void drawGUIPartText(IGUIDrawer& drawer, const Element* element, GUIPart part, const std::string& text);

  public:
    Element();
    virtual ~Element(){};
    
    bool isEnabled() const { return enabled; }
    void setEnabled(bool i_enabled = true);
    
    ////mouse overrides
    virtual bool mouseOver(const IInput& input) const;
    virtual bool mouseGrabbable() const;
    virtual bool mouseActive() const { return enabled; }
    
    ////core functions of gui::Elements
    void draw(IGUIDrawer& drawer) const; //will draw the actual widget and do some other always-to-do stuff, do NOT override this function
    void handle(const IInput& input);
    virtual void move(int x, int y);
    virtual void resize(int x0, int y0, int x1, int y1); //especially useful for windows and their container; parameters are the new values for x0, y0, x1 and y1 so this function can both move the object to a target and resize
    void manageHover(IHoverManager& hover);
    /*
    hitTest: if you override this, only return publically accessible elements, not internal parts of the element. For example, it's ok for a window to return elements you placed inside of it or itself, but not to return its top bar or close button.
    It's for example not ok for a scrollbar to return its arrow buttons or the scroller button
    TODO: revise this.
    */
    virtual const Element* hitTest(const IInput& input) const;
    virtual Element* hitTest(const IInput& input);
    virtual bool isFloating() const; //returns true if it's an element that should go to the top if you click on it (like a window). This is used by Container to bring elements to the top.

    void moveTo(int x, int y);
    void moveCenterTo(int x, int y);
    void growX0(int d) { resize(x0 + d, y0    , x1    , y1    ); } //growing can also be shrinking
    void growY0(int d) { resize(x0    , y0 + d, x1    , y1    ); }
    void growX1(int d) { resize(x0    , y0    , x1 + d, y1    ); }
    void growY1(int d) { resize(x0    , y0    , x1    , y1 + d); }
    void growSizeX0(int sizex) { resize(x1 - sizex, y0        , x1        , y1        ); } //growing can also be shrinking
    void growSizeY0(int sizey) { resize(x0        , y1 - sizey, x1        , y1        ); }
    void growSizeX1(int sizex) { resize(x0        , y0        , x0 + sizex, y1        ); }
    void growSizeY1(int sizey) { resize(x0        , y0        , x1        , y0 + sizey); }
    
    ////custom tooltip
    virtual void drawToolTip(IGUIDrawer& drawer) const; //override if you can invent a fallback tooltip to draw for the element, but it's not required, the TooltipManager only uses this if no other tooltip was specified by the user (use the static ToolTipManager::drawToolTip function if you want the default style)
    
    virtual void setElementOver(bool state); //ALL gui types that have gui elements inside of them, must set elementOver of all gui elements inside of them too! ==> override this virtual function for those. Override this if you have subelements, unless you use addSubElement in ElementComposite.
    bool hasElementOver() const;
    
    MouseState& getMouseStateForContainer() { return mouse_state_for_containers; }

    ////for debugging: if you've got no idea what's going on with a GUI element, this function at least is guaranteed to show where it is (if in screen)
    void drawDebugBorder(IGUIDrawer& drawer, const ColorRGB& color = RGB_Red) const;
    void drawDebugCross(IGUIDrawer& drawer, const ColorRGB& color = RGB_Red) const;
    
    void drag(const IInput& input, MouseButton button = LMB); //utility function, for moving this element when it's dragged by the mouse
    
    /*
    hasKeyboardFocus: experimental new system. Elements take focus themselves,
    you don't set focus on them (e.g. inputlines take focus if you click on
    them, remove focus if you click outside of them with the mouse). But this
    can be used to detect whether one has focus, and if so you can ignore
    keyboard commands yourself because the element is already taking them
    (otherwise multiple things at the same time would use the command, e.g.
    input box and shortcut)
    */
    virtual int getKeyboardFocus() const { return 0; }
};

class AElementComposite : public Element //element with "internal container" to automatically handle child elements for you
{
  protected:
    AInternalContainer& ic;

  public:
    AElementComposite(AInternalContainer& ic) : ic(ic) {}
  
    virtual void move(int x, int y);
    virtual void setElementOver(bool state);
    virtual void resize(int x0, int y0, int x1, int y1);
    virtual void manageHoverImpl(IHoverManager& hover);
    //virtual const Element* hitTest(const IInput& input);
    virtual int getKeyboardFocus() const;
};

class ElementComposite : public AElementComposite //element with "internal container" to automatically handle child elements for you
{
  protected:
    InternalContainer my_ic;

  protected:
    ElementComposite() : AElementComposite(my_ic) {};
    void addSubElement(Element* element, const Sticky& sticky = STICKYDEFAULT); //only used for INTERNAL parts of the gui element, such as the buttons in a scrollbar, hence this function is protected
    void clearSubElements();
};

/*
ElementWrapper can be used (by inheritance) if your element contains one element in it that should have the same
size and so on, as if it's your element, but you want to wrap it to add some functionality
without inheriting from it directly.
*/
class ElementWrapper : public Element
{
  protected:
    Element* wrapped;

  public:
    ElementWrapper(Element* wrapped) : wrapped(wrapped) {};
    virtual void move(int x, int y) { Element::move(x, y); wrapped->move(x, y); }
    virtual void setElementOver(bool state) { Element::setElementOver(state); wrapped->setElementOver(state); }
    virtual void resize(int x0, int y0, int x1, int y1) { Element::resize(x0, y0, x1, y1); wrapped->resize(x0, y0, x1, y1); }
    virtual void manageHoverImpl(IHoverManager& hover) { Element::manageHoverImpl(hover); wrapped->manageHover(hover); }
    virtual Element* hitTest(const IInput& input) { return wrapped->hitTest(input); }
    virtual void drawImpl(IGUIDrawer& drawer) const { if(enabled) wrapped->draw(drawer); }
    virtual void handleImpl(const IInput& input) { if(enabled) wrapped->handle(input); }
};

class Label //convenience class: elements that want an optional label (e.g. checkboxes) can inherit from this and use drawLabel(this) in their drawWidget function
{
  private:
    std::string label;
    int labelX; //label position is relative to the position of the element
    int labelY;
    Font labelFont;
  
  protected:
    void drawLabel(IGUIDrawer& drawer, const Element* element) const;
    
  public:
    Label();
    virtual ~Label(){}
    
    //position is relative to the element
    void makeLabel(const std::string& label, int labelX, int labelY, const Font& labelFont = FONT_Default);
};


//Dummy = exactly the same as Element but not abstract, nothing implemented except pure virtuals of Element
class Dummy : public Element
{
  void drawImpl(IGUIDrawer& /*drawer*/) const {}
};

class Button : public Element
{
  /*
  the button has 3 separate graphical elements:
  *) text
  *) an image (or two)
  *) a panel (resisable rectangle with sides)
  */
  
  public:
    Button();
    
    ////part "back image"
    bool enableImage;
    HTexture* image[3]; //0=normal, 1=mouse over, 2=mouse down
    int imageOffsetx;
    int imageOffsety;
    ColorRGB imageColor[3]; //0=normal, 1=mouse over, 2=mouse down
    
    ////part "front image"
    bool enableImage2;
    HTexture* image2[3]; //0=normal, 1=mouse over, 2=mouse down
    int imageOffsetx2;
    int imageOffsety2;
    ColorRGB imageColor2[3]; //0=normal, 1=mouse over, 2=mouse down

    ////part "text"
    bool enableText;
    std::string text;
    int textOffsetx;
    int textOffsety;
    Font font[3];
    void autoTextSize(const ITextDrawer* drawer, int extrasize = 0); //will automaticly adjust it's size to fit text size
    
    ////part "panel"
    bool enablePanel;
    int panelOffsetx;
    int panelOffsety;
    
    //special options
    /*
    mouseDownVisualStyle: when to change the image of the button to the mouseDown image
    0: only when mouseDown()
    1: only when mouseDownHere()
    2: only when grabbed()
    */
    int mouseDownVisualStyle;

    ////make functions

    //image only constructor (without offset)
    void makeImage(int x, int y,
                   HTexture* texture1, HTexture* texture2, HTexture* texture3, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey); //image
    
    void makeImage(int x, int y,  HTexture* texture123, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey);
    
    void addFrontImage(HTexture* texture1, HTexture* texture2, HTexture* texture3,
                       const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey);
    void addFrontImage(HTexture* texture);
    
    //text only constructor (without offset)
    void makeText(int x, int y, //basic properties
                  const std::string& text, //text
                  const ITextDrawer& drawer); //drawer is for auto-text-size
    
    //panel + text constructor (text always in center of panel, no offsets and thinking needed)
    //this is the constructor with default parameters
    void makeTextPanel(int x, int y, const std::string& text, int sizex = 64, int sizey = 24); //basic properties + actual text; give drawer if you want the text centered

    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    //mouse functions without having to give the IInput
    /*bool clicked(MouseButton button = LMB);
    bool pressed(MouseButton button = LMB);*/
    
  private:
    MouseState button_drawing_mouse_test;
    bool draw_mouse_button_down_style;
};

//The Scrollbar
class Scrollbar : public ElementComposite
{
  private:
    double oldTime; //in seconds
    void init(const IGUIDrawer& geom);
    
    //buttons of the scrollbar
    Dummy buttonUp;
    Dummy buttonDown;
    Dummy scroller;
    
    int speedMode;
    
  public:
    //get length and begin and end coordinates of the slider part (the part between the up and down buttons) (relative to x, y of the scrollbar)
    int getSliderSize() const;
    int getSliderStart() const;
    int getSliderEnd() const;
    
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;

    Direction direction; //0 = vertical, 1 = horizontal
    double scrollSize; //length of the total scrollbar (in steps)
    double scrollPos; //position of the scroller on the bar (in steps)
    double scrollSpeed; //if speedMode == 0: steps / second, if speedMode == 1: seconds / whole bar
    double absoluteSpeed; //steps / second
    
    void setRelativeScrollSpeed(); //time = time to scroll from top to bottom (in seconds)
    void setRelativePosition(float position); //position = 0.0-1.0
    void setSpeedMode(int speedMode) { this->speedMode = speedMode; } //0: absolute, 1: relative

    Scrollbar();
    void makeVertical(int x, int y, int length,
                      double scrollSize,
                      const IGUIDrawer& geom,
                      double scrollPos = 0, double offset = 0, double scrollSpeed = 1,
                      int speedMode = 1);
    void makeHorizontal(int x, int y, int length,
                        double scrollSize,
                        const IGUIDrawer& geom,
                        double scrollPos = 0, double offset = 0, double scrollSpeed = 1,
                        int speedMode = 1);
    
    void scroll(const IInput& input, int dir); //make it scroll from an external command
    
    double offset; //used as an offset of ScrollPos to get/set the scroll value with offset added with the functions below
    double getValue() const;
    void setValue(double value);
    
    bool forwardedMouseScrollUp() const; //see int forwardedScroll;
    bool forwardedMouseScrollDown() const; //see int forwardedScroll;
    void forwardScroll(int scroll); //see int forwardedScroll;
    
  protected:
    
    mutable int forwardedScroll; //forwarded mouse scroll event from other element; +1 is down, -1 is up
};

class ScrollbarPair : public ElementComposite
{
  public:
    ScrollbarPair();
    void make(int x, int y, int sizex, int sizey, double scrollSizeH, double scrollSizeV, const IGUIDrawer& geom);
        
    Scrollbar vbar;
    Scrollbar hbar;
    Dummy corner; //the corner piece between the two scrollbars
    
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void resizeImpl(const Pos<int>& newPos);
    
    bool venabled;
    bool henabled;
    
    void disableV();
    void disableH();
    void enableV();
    void enableH();
    
    bool conserveCorner; //if false, the cornerpiece will disappear if one scrollbar is gone, if true, it'll only disappear if both scrollbars are disabled
    
    //size of the area without the scrollbars
    int getVisiblex() const; //returns x1 - x0 - vbar.getWidth()  if there's a vbar, or x1 - x0 if there's no vbar
    int getVisibley() const; //returns y1 - y0 - hbar.getHeight() if there's a hbar, or y1 - y0 if there's no hbar
};

//the Slider is a simplified version of the scrollbar (no up and down buttons) that also looks different
class Slider : public ElementComposite
{
  private:
    
    Dummy slider; //(by default round) button
    GUIPart slidertype;
    GUIPart rulertype;
    
  public:
    Slider();
    double getValue() const; //a number between 0.0 and scrollSize
    void setValue(double value);
    double getRelValue() const; //a number between 0.0 and 1.0
    void setRelValue(double value);
        
    Direction direction;
    
    
    
    double scrollSize;
    double scrollPos;

    void makeHorizontal(int x, int y, int length, double scrollSize, const IGUIDrawer& geom);
    void makeVertical(int x, int y, int length, double scrollSize, const IGUIDrawer& geom);
    void makeSmallHorizontal(int x, int y, int length, double scrollSize, const IGUIDrawer& geom);
    
    double screenPosToScrollPos(int screenPos);
    int scrollPosToScreenPos(double scrollPos);

    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
};

class AContainer : public Element
{
  protected:
    AInternalContainer* elements;

  protected:
    
    void drawElements(IGUIDrawer& drawer) const;
    void drawElementsPopup(IGUIDrawer& drawer) const;
    
  public:
    
    AContainer(AInternalContainer* ic);
    AContainer(AInternalContainer* ic, IGUIDrawer& drawer); //drawer is used to initialize the size of the container to the full screen size of the drawer
    virtual void handleImpl(const IInput& input); //you're supposed to handle() before you draw()
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
    
    Element* getElement(size_t i) const { return elements->getElement(i); }
    unsigned long size() const;
    
    void bringToTop(Element* element); //precondition: element must already be in the list
    
    void centerElement(Element* element); //TODO: remove this function from this class, if necessary put it somewhere else as utility function

    void putInside(unsigned long i);
    virtual void moveImpl(int x, int y);
    void make(int x, int y, int sizex, int sizey);
    
    void getRelativeElementPos(Element& element, int& ex, int& ey) const;
    virtual void resizeImpl(const Pos<int>& newPos);
    
    virtual void setElementOver(bool state);
    virtual const Element* hitTest(const IInput& input) const;
    
    void setSizeToElements(); //makes the size of the container as big as the elements. This resizes the container in a non-sticky way: no element is affected
    
    virtual int getKeyboardFocus() const;
};

class Container : public AContainer
{
  protected:
    InternalContainer elements_ic;
    
  public:
    
    Container();
    Container(IGUIDrawer& drawer); //drawer is used to initialize the size of the container to the full screen size of the drawer
    
    //push the element without affecting absolute position
    void pushTop(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void pushBottom(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void insert(size_t pos, Element* element, const Sticky& sticky = STICKYDEFAULT);
    
    //push the element so that its top left is relative to the top left of this container, thus moving it if the container isn't at 0,0
    void pushTopRelative(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void pushBottomRelative(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void insertRelative(size_t pos, Element* element, const Sticky& sticky = STICKYDEFAULT);

    //push the element at the given x, y (relative to this container's top left)
    void pushTopAt(Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT);
    void pushBottomAt(Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT);
    void insertAt(size_t pos, Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT);

    void remove(Element* element);
    void clear(); //clears all the elements
};

class ContainerWrapping : public AContainer
{
  protected:
    InternalContainerWrapping elements_ic;
    
  public:
    
    ContainerWrapping();
    ContainerWrapping(IGUIDrawer& drawer); //drawer is used to initialize the size of the container to the full screen size of the drawer
    
    //push the element without affecting absolute position
    void pushTop(Element* element);
    void remove(Element* element);
    void clear(); //clears all the elements
};


class Group : public Container
{
  public:
    virtual bool isInside(int x, int y) const; //difference with the isInside from other guielements, is that it checks all sub elements, not itself, for mouseovers
    virtual void drawImpl(IGUIDrawer& drawer) const;
};

class IModalFrameHandler //functionality the MainContainer needs to do modal gameloops
{
  public:
    virtual bool doFrame() = 0; //should do everything needed every frame, including clearing the screen and redrawing it if necessary (or even draw lots of extra stuff from a computer game in the background). Returns true normally, false if you need to force-quit.
    virtual IGUIDrawer& getDrawer() = 0; //since GUIDrawer also has a getInput in it, this is also used for getting the IInput
    virtual void getScreenSize(int& x0, int& y0, int& x1, int& y1) = 0;
};

class Dialog; //forward declaration of class Dialog for the doModalDialog function of MainContainer

/*
MainContainer has a system for handling hovering elements. Use this as the container spanning
the whole screen in which every other element is contained, to make hovering (popups) work. Without
this or an alternative system where you use IHoverManager somehow yourself, menu's, dropdown lists,
and other elements that use hovering elements won't work.
TODO: give MainContainer also a ToolTipManager and let it handle it
*/
class MainContainer : public IHoverManager, public ElementComposite
{
  private:
    Container c;
    Container e;
    Group h;
    
    ToolTipManager tooltips;
    
  public:
  
    MainContainer();
    MainContainer(IGUIDrawer& drawer);
    
    void ctor();
    
    ToolTipManager& getToolTipManager();
    const ToolTipManager& getToolTipManager() const;
  
    virtual void addHoverElement(Element* element);
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    
    //push the element without affecting absolute position
    void pushTop(Element* element, const Sticky& sticky = STICKYDEFAULT){e.pushTop(element, sticky);}
    void pushBottom(Element* element, const Sticky& sticky = STICKYDEFAULT){e.pushBottom(element, sticky);}
    void insert(size_t pos, Element* element, const Sticky& sticky = STICKYDEFAULT){e.insert(pos, element, sticky);}
    
    //push the element so that its top left is relative to the top left of this container, thus moving it if the container isn't at 0,0
    void pushTopRelative(Element* element, const Sticky& sticky = STICKYDEFAULT){e.pushTop(element, sticky);}
    void pushBottomRelative(Element* element, const Sticky& sticky = STICKYDEFAULT){e.pushBottomRelative(element, sticky);}
    void insertRelative(size_t pos, Element* element, const Sticky& sticky = STICKYDEFAULT){e.insertRelative(pos, element, sticky);}

    //push the element at the given x, y (relative to this container's top left)
    void pushTopAt(Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT){e.pushTopAt(element, x, y, sticky);}
    void pushBottomAt(Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT){e.pushBottomAt(element, x, y, sticky);}
    void insertAt(size_t pos, Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT){e.insertAt(pos, element, x, y, sticky);}
    
    void remove(Element* element) { e.remove(element); }
    
    virtual const Element* hitTest(const IInput& input) const;
    
    Element* getElement(size_t i) const { return e.getElement(i); }
    unsigned long size() const { return e.size(); }
    
    bool doModalDialog(Dialog& dialog, IModalFrameHandler& frame); //Blocking function call. Runs a gameloop where only the given dialog is handled. Returns true normally, false if the loop during modal dialog showing was force-quit. After running doModalDialog(), you should check for ok/cancel with dialog's getResult, and if it was OK, use the value if the dialog (if any).
};

class ScrollElement : public ElementComposite //a.k.a "ScrollZone"
{
  /*
  ScrollElement will contain 1 element (which can be a container with more elements).
  Depending on the size of that element compared to this ScrollElement,
  1 or 2 scrollbars will appear, allowing to scroll to see all of that element.
  The size of that element isn't affected by this ScrollElement.
  The position of that element is completely controlled by this ScrollElement or its scrollbars
  */
  protected:
  
    bool keepelementsinside; //false by default
    int prevx0, prevy0, prevdx, prevdy; //used to do initBars again if element changed size or position due to external change
  
  public:
    Element* element;
  
    ScrollElement();
    virtual void handleImpl(const IInput& input); //you're supposed to handle() before you draw()
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
    virtual void moveImpl(int x, int y);
    virtual void setElementOver(bool state);
    virtual const Element* hitTest(const IInput& input) const;
    
    void drawElements(IGUIDrawer& drawer) const;
    void drawScroll(IGUIDrawer& drawer) const;

    void setKeepElementsInside(bool set) { keepelementsinside = set; }

    void make(int x, int y, int sizex, int sizey, Element* element, const IGUIDrawer& geom);
    
    ////everything concerning the scrollability
    ScrollbarPair bars;

    //the zone where elements are drawn: the size of this container excluding the scrollbarpair's bars
    int getVisibleSizeX() const;
    int getVisibleSizeY() const;
    int getVisibleX0() const;
    int getVisibleY0() const;
    int getVisibleX1() const;
    int getVisibleY1() const;
    
    int oldScrollx; //used to move elements every frame when you scroll the bars
    int oldScrolly;
    
    void moveAreaTo(int x, int y); //moves the area to given position, and all the elements, but not the bars and x0, y0, x1, y1, Used when you scroll.
    
    void forwardScrollToVerticalScrollbar(int scroll) //if you scroll the mouse and want this container's vertical scrollbar to handle it, use this! -1 for up, +1 for down
    {
      bars.vbar.forwardScroll(scroll);
    }
    
    void updateBars(); //this is called automatically now and then, but can also be called by you at any time when the element changed position to ensure the scrollers position won't override your new position
    
    virtual int getKeyboardFocus() const;
  
  protected:
  
    virtual bool mouseInVisibleZone(const IInput& input) const; //is the mouse in the zone where elements are drawn
    
    void initBars();
    void toggleBars(); //turns the bars on or of depending on if they're needed or not
};

class StatusBar : public ElementComposite
{
  protected:
  
  
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
   
    std::vector<std::string> texts;
    std::vector<double> zonesizesrel; //last size is ignored, instead it's the remaining space. Sum is maximum 1.0.
    std::vector<int> zonesizesabs;
    
  public:
  
    StatusBar();
    void setNumZones(size_t num);
    size_t getNumZones() const;
    void clear();
    
    void setZoneSize(size_t i, double relsize, int abssize);
    void setZoneText(size_t i, const std::string& text);
};

//Window is a container for other gui elements that'll move and get drawn at the command of the window
class Window : public ElementComposite
{
  protected:
    Container container;
    ScrollElement scroll;
    ColorRGB colorMod;
    Dummy closeButton;
    Dummy resizer;
    bool enableTop; //enable the top bar of the window (then you can drag it with this instead of everywhere on the window)
    bool closeEnabled; //close button is enabled
    bool enableResizer;
    bool resizerOverContainer;
    StatusBar* statusBar;

    virtual int getMinSizeX() { return 128; }
    virtual int getMinSizeY() { return 64; }
    
  protected:
  
    ////container
    int getContainerLowest() const;
    int getContainerHighest() const;
    int getContainerLeftmost() const;
    int getContainerRightmost() const;
    Container* getContainer() { return &container; }
    
    //get the parameters for the container surface on which the elements in the window will be
    int getContainerX0() const { return container.getX0(); }
    int getContainerY0() const { return container.getY0(); }
    int getContainerX1() const { return container.getX1(); }
    int getContainerY1() const { return container.getY1(); }
    int getContainerSizeX() const { return container.getSizeX(); }
    int getContainerSizeY() const { return container.getSizeY(); }
    
    int getRelContainerStart() const { return container.getY0() - y0; }
    int getRelContentStart() const;
    
    //the drawImpl call will first do drawWindow, then drawElements. You can override drawImpl and call these function too and do something else between them if needed.
    void drawWindow(IGUIDrawer& drawer) const;
    void drawElements(IGUIDrawer& drawer) const;

    
  public:

    Window();
    ~Window();
    //push the element without affecting absolute position
    void pushTop(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void pushBottom(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void insert(size_t pos, Element* element, const Sticky& sticky = STICKYDEFAULT);
    
    //push the element so that its top left is relative to the top left of this container, thus moving it if the container isn't at 0,0
    void pushTopRelative(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void pushBottomRelative(Element* element, const Sticky& sticky = STICKYDEFAULT);
    void insertRelative(size_t pos, Element* element, const Sticky& sticky = STICKYDEFAULT);

    //push the element at the given x, y (relative to this container's top left)
    void pushTopAt(Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT);
    void pushBottomAt(Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT);
    void insertAt(size_t pos, Element* element, int x, int y, const Sticky& sticky = STICKYDEFAULT);
    
    void clear();

    void bringToTop(Element* element); //precondition: element must already be in the list
    
    void remove(Element* element);
    unsigned long size() const;
    void putInside(int i);
    
    //these scrollbars will be part of the container
    void addScrollbars(const IGUIDrawer& geom);
    void removeScrollbars();
    void updateScroll(); //call this after elements inside window changed size or position. It updates size of container to elements inside it, then updates scroll (updateBars()). I've had this work very well together with a zoomable and movable image inside a window with scrollbars!!
    
    void addTop(const IGUIDrawer& geom);
    Dummy top; //todo make this private or protected (currently used by the unittest)
    
    ////optional part "title"
    std::string title;
    Font titleFont;
    void addTitle(const std::string& title, const Font& titleFont = FONT_Default);
    void setTitle(const std::string& title); //only to be used after "addTitle" (or the x, y position will be messed up)
    
    ////optional part "close button"
    bool closeButtonClicked(const IInput& input) { return closeButton.clicked(input); }
    void addCloseButton(const IGUIDrawer& geom); //ofsset from top *right* corner, choose style of close button by making it, it's the built in texture by default
    
    ////optional part "resizer" = draggable bottom right corner with diagonally striped image
    void addResizer(const IGUIDrawer& geom, bool overContainer = false);
    
    ////optional part "status bar"
    void addStatusBar();

    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
    
    virtual void handleImpl(const IInput& input);
    virtual bool isFloating() const;
    virtual const Element* hitTest(const IInput& input) const;
    
    void setColorMod(const ColorRGB& color) { colorMod = color; }
    
    virtual int getKeyboardFocus() const;
    
    StatusBar* getStatusBar() { return statusBar; }
};

//The Checkbox
class Checkbox : public Element, public Label
{
  private:
    int textOffsetX;
    int textOffsetY;
    //bool downAndTested; //if mouse is down and that is already handled, leave this on so that it'll ignore mouse till it's back up
    void positionText(); //automaticly place the text a few pixels next to the checkbox, in the center
  
    bool checked; //if true: checked, if false: unchecked
    int toggleOnMouseUp;

    //text
    bool enableText; //the text is a title drawn next to the checkbox, with automaticly calculated position
    std::string text;
    Font font;


    //for when using checkbox with custom image
    bool useCustomImages;
    HTexture* texture[4];
    ColorRGB colorMod[4];
    bool useCustomImages2;
    HTexture* texture2[4];
    ColorRGB colorMod2[4];
    
    bool useCustomGUIPart;
    GUIPart partOn;
    GUIPart partOff;
    
  public:

    Checkbox();
    void make(int x, int y, bool checked = 0, int toggleOnMouseUp = 0);
    void makeSmall(int x, int y, bool checked = 0, int toggleOnMouseUp = 0);
    void addText(const std::string& text, const Font& font = FONT_Default);
    virtual void drawImpl(IGUIDrawer& drawer) const; //also handles it by calling handle(): toggles when mouse down or not
    virtual void handleImpl(const IInput& input);

    void setText(const std::string& newText);
    const std::string& getText() const { return text; }
    void toggle();
    void check() { checked = true; }
    void uncheck() { checked = false; }
    bool isChecked() { return checked; }
    void setChecked(bool check) { checked = check; }
    
    //TODO: make the functions below more logical and clean
    //for giving it alternative textures. texture1 = not checked, mouse not over. Texture2 = not checked, mouse over. Texture3 = checked, mouse not over. Texture4 = checked, mouse over.
    void setTexturesAndColors(HTexture* texture1, HTexture* texture2, HTexture* texture3, HTexture* texture4, 
                              const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4);
    //without mouseOver effect
    void setTexturesAndColors(HTexture* texture1, HTexture* texture2, 
                              const ColorRGB& color1, const ColorRGB& color2);
    void addFrontImage(HTexture* texture1, HTexture* texture2, HTexture* texture3, HTexture* texture4,
                       const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Grey, const ColorRGB& imageColor3 = RGB_White, const ColorRGB& imageColor4 = RGB_Grey);
    void addFrontImage(HTexture* texture);
    void setCustomColor(const ColorRGB& color);
    void setCustomColor2(const ColorRGB& color); //for the front image
    void setCustomGUIParts(GUIPart part_on, GUIPart part_off);

};

//The bulletlist, a list of checkboxes where only one can be selected (radio buttons)
//TODO: improve this interface
class ABulletList
{
  public:
    std::vector <Checkbox*> bullet; //todo: make private
    
    ABulletList();
    ~ABulletList();
    
    void drawBullets(IGUIDrawer& drawer) const;
    void handleBullets(const IInput& input);

    int check(); //returns which one is checked

    std::string getText(unsigned long i) const;
    const std::string& getCurrentText() const;
    void addText(const std::string& text, unsigned long i);
    void set(unsigned long i);
    
  private:
    int lastChecked; //remember which one was checked, so it can know when a new one is checked, which one that is
};

//The bulletlist, a list of checkboxes where only one can be selected (radio buttons)
//TODO: improve this interface
class BulletList : public ABulletList, public ElementComposite
{
  public:
    
    BulletList();
    ~BulletList();
    void clear();

    void make(int x, int y, unsigned long amount, int xDiff, int yDiff); //diff = the location difference between successive checkboxes
    void make(int x, int y, unsigned long amount, int xDiff, int yDiff, unsigned long amountx); //make in 2D pattern
    void setCorrectSize();

    int xDiff; //just added for "book keeping"
    int yDiff; //just added for "book keeping"
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    virtual const Element* hitTest(const IInput& input) const;
};

class Text : public Element
{
  public:
    bool useNewLine;
    Font font;
    virtual void drawImpl(IGUIDrawer& drawer) const;
    Text();
    void make(int x = 0, int y = 0, const std::string& text = "", const Font& font = FONT_Default);
    void setText(const std::string& text);
    const std::string& getText() const { return text; }
  private:
    std::string text;
};

class Image : public Element
{
  private:
    ITexture* image;
    
  public:
    ColorRGB colorMod;
    virtual void drawImpl(IGUIDrawer& drawer) const;
    Image();
    void make(int x, int y, ITexture* image, const ColorRGB& colorMod = RGB_White);
    void make(int x, int y, int sizex, int sizey, ITexture* image, const ColorRGB& colorMod = RGB_White);
};

class Tabs : public ElementComposite
{
  private:
    struct Tab : public Element
    {
      std::string name; //name of this tab
      Container container; //contains the GUI elements inside this tab
     
      Tab(); 
      virtual void drawImpl(IGUIDrawer& drawer) const;
    };
    
    std::vector<Tab*> tabs;
    size_t selected_tab;
    
    void generateTabSizes();
    void updateActiveContainer();
    
  public:
    Tabs();
    ~Tabs();
    
    void addTab(const std::string& name);
    size_t getNumTabs() const;
    Container& getTabContent(size_t index) const;
    void clear();
    
    size_t getSelectedTab() const { return selected_tab; }
    void selectTab(size_t i_index);
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
    virtual void handleImpl(const IInput& input);
};

//a Dialog can be done "Modal" by MainContainer
class Dialog : public Window
{
  public:
    enum Result
    {
      OK, //there's no "yes" or "no" results. A yes/no dialog has a boolean result for that, similar to how a color dialog has a ColorRGB result and so on.
      CANCEL
    };

  protected:

    Result result;

  public:

    Dialog()
    : result(CANCEL)
    {
    }

    virtual bool done() const; //returns true if the dialog is closed by the user (by pressing ok, cancel, ...)
    virtual Result getResult() const; //get this after done was true. This is NOT the value of the dialog (like the color of a color dialog or the yes/no boolean of a yesno dialog). Instead, this only indicates if the value is valid (if it's OK) or not (if it's CANCEL)
};

} //namespace gui
} //namespace lpi

