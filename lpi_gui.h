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
lpi_gui: an OpenGL GUI
*/

#ifndef LPI_GUI_H_INCLUDED
#define LPI_GUI_H_INCLUDED

#include "lpi_gui_draw.h"

#include "lpi_color.h"
#include "lpi_event.h"
#include "lpi_texture.h"
#include "lpi_text.h"


namespace lpi
{
namespace gui
{

enum Sticky //how to resize child widget with master widget
{
  TOPLEFT, //follow top/left side of master
  BOTTOMRIGHT, //follow bottom/right side of master
  RELATIVE00, //position relative to master, distance of this subelement's side to upper left ("00") corner of subelement will stay constant
  RELATIVE11, //position relative to master, distance of this subelement's side to lower right ("11") corner of subelement will stay constant
  NOTHING //don't do anything in resize(). But in move() it'll still translate.
};

template<typename T>
struct Pos
{
  T x0;
  T y0;
  T x1;
  T y1;
};

//some named combinations for Pos<Sticky> values, more are possible but these are probably the most common
extern Pos<Sticky> STICKYTOPLEFT; //always keeps same size, stays at same position compared to parent's top left
extern Pos<Sticky> STICKYTOPRIGHT; //always keeps same size, stays at same position compared to parent's top right
extern Pos<Sticky> STICKYBOTTOMRIGHT; //always keeps same size, stays at same position compared to parent's bottom right
extern Pos<Sticky> STICKYBOTTOMLEFT; //always keeps same size, stays at same position compared to parent's bottom left
extern Pos<Sticky> STICKYFULL; //follows each corresponding side, so it resizes as much as its master
extern Pos<Sticky> STICKYNOTHING; //don't do anything in resize() of parent, but still move if the parent move()s
//for e.g. scrollbars:
extern Pos<Sticky> STICKYVERTICALLEFT; //follows both sides in vertical direction, follows left side
extern Pos<Sticky> STICKYVERTICALRIGHT; //follows both sides in vertical direction, follows right side
extern Pos<Sticky> STICKYHORIZONTALTOP; //follows both sides in horizontal direction, follows top side
extern Pos<Sticky> STICKYHORIZONTALBOTTOM; //follows both sides in horizontal direction, follows bottom side
//some involving relative:
extern Pos<Sticky> STICKYRELATIVE; //relative in all directions - use for subelements that may be resized too in all directions
extern Pos<Sticky> STICKYRELATIVE00; //position relative, but size stays constant (position determined by top left corner)
extern Pos<Sticky> STICKYRELATIVE01; //position relative, but size stays constant (position determined by bottom left corner)
extern Pos<Sticky> STICKYRELATIVE10; //position relative, but size stays constant (position determined by top right corner)
extern Pos<Sticky> STICKYRELATIVE11; //position relative, but size stays constant (position determined by bottom right corner)
extern Pos<Sticky> STICKYRELATIVEHORIZONTAL0; //size and position relative in horizontal direction, position relative in vertical direction
extern Pos<Sticky> STICKYRELATIVEVERTICAL0; //size and position relative in vertical direction, position relative in horizontal direction
extern Pos<Sticky> STICKYRELATIVEHORIZONTAL1; //size and position relative in horizontal direction, position relative in vertical direction
extern Pos<Sticky> STICKYRELATIVEVERTICAL1; //size and position relative in vertical direction, position relative in horizontal direction
extern Pos<Sticky> STICKYRELATIVEHORIZONTALFULL; //relative in horizontal direction, follows both sides in vertical direction
extern Pos<Sticky> STICKYRELATIVEVERTICALFULL; //relative in vertical direction, follows both sides in horizontal direction
extern Pos<Sticky> STICKYRELATIVELEFT; //relative in horizontal direction, follows left side in vertical direction
extern Pos<Sticky> STICKYRELATIVETOP; //relative in vertical direction, follows top side in horizontal direction
extern Pos<Sticky> STICKYRELATIVERIGHT; //relative in horizontal direction, follows right side in vertical direction
extern Pos<Sticky> STICKYRELATIVEBOTTOM; //relative in vertical direction, follows bottom side in horizontal direction

class Element;

class InternalContainer
{
  std::vector<Element*> elements;
  
  public:
  
  const std::vector<Element*>& getElements() const { return elements; }
  std::vector<Element*>& getElements() { return elements; }
  
  void resize(const Pos<int>& oldPos, const Pos<int>& newPos); //this resizes the 2D size of elements, not the amount of elements
  void move(int x, int  y);
  void setElementOver(bool state); //this says to all elements whether or not another element is in front of it in Z order, causing mouse to not work

  void addSubElement(Element* element, const Pos<Sticky>& sticky, Element* parent);
  void insertSubElement(size_t index, Element* element, const Pos<Sticky>& sticky, Element* parent);
  
  Element* getElement(size_t i) const { return elements[i]; }
  
  size_t findElementIndex(Element* element)
  {
    for(size_t i = 0; i < elements.size(); i++) if(elements[i] == element) return i;
    return elements.size();
  }
  
  void removeElement(Element* element)
  {
    size_t index = findElementIndex(element);
    if(index < elements.size()) elements.erase(elements.begin() + index);
  }
  
  private:
  void initSubElement(Element* element, const Pos<Sticky>& sticky, Element* parent);
};

const int NUM_MOUSE_BUTTONS = 2;

struct MouseState
{
  bool downhere_bool1;
  bool downhere_bool2;
  bool justdown_prev;
  bool justdownhere_prev;
  bool justuphere_bool1;
  bool justuphere_bool2;
  bool grabbed_grabbed;
  bool grabbed_prev;
  int grabx;
  int graby;
  int grabrelx;
  int grabrely;
  int doubleClickState;
  double doubleClickTime; //in seconds
  
  MouseState();
};

class BasicElement //more basic than "Element" - only describes the shape and mouse handling in this shape
{
  protected:
    ////position
    int x0; //position of upper left corner of this element on screen
    int y0;
    int x1; //position of the bottom right corner of this element on screen
    int y1;
    
    virtual bool mouseOverShape() const; //can be overridden for elements with different shape, e.g. circle, or the gui::Group which's shape is that of all elements in it
  
  public:
    BasicElement();
    
    int getX0() const { return x0; }
    int getY0() const { return y0; }
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    void setX0(int x0) { this->x0 = x0; }
    void setY0(int y0) { this->y0 = y0; }
    void setX1(int x1) { this->x1 = x1; }
    void setY1(int y1) { this->y1 = y1; }
    
    int getSizex() const { return x1 - x0; } //get the size of this element
    int getSizey() const { return y1 - y0; }
    //obviously the functoins below are best used after setting x0 and y0
    void setSizex(const int sizex) { x1 = x0 + sizex; } //change x1, y1 to get the given size
    void setSizey(const int sizey) { y1 = y0 + sizey; }
    int getCenterx() const { return (x0 + x1) / 2; } //the center in screen coordinates
    int getCentery() const { return (y0 + y1) / 2; }
    int getRelCenterx() const { return (x1 - x0) / 2; } //the half of the size
    int getRelCentery() const { return (y1 - y0) / 2; }

    ////MOUSE RELATED STUFF
    int mouseGetRelPosX() const; //returns relative mouse positions (relative to x and y of the elemnt)
    int mouseGetRelPosY() const; //returns relative mouse positions (relative to x and y of the elemnt)
    
    //these are virtual because other states can have an influence
    virtual bool mouseOver() const; //mouse cursor over the element (and no other element lpiged by gui container above it)
    virtual bool mouseGrabbable() const { return true; }
    virtual bool mouseActive() const { return true; } //false if the element should not respond to "clicked", "pressed", etc..
    
    bool mouseDown(MouseButton button = LMB) const; //mouse is over and down
    bool mouseDownHere(MouseState& state, MouseButton button = LMB) const; //mouse is over, down, and was pressed while it was on here
    bool mouseDownHere(MouseButton button = LMB);
    bool mouseGrabbed(MouseState& state, MouseButton button = LMB) const; //like mouseDownHere, but keeps returning true if you move the mouse away from the element while keeping button pressed
    bool mouseGrabbed(MouseButton button = LMB);
    void mouseGrab(MouseState& state) const; //sets states as if element were grabbed
    void mouseGrab(MouseButton button = LMB);
    void mouseUngrab(MouseState& state) const; //sets states as if element were not grabbed
    void mouseUngrab(MouseButton button = LMB);
    int mouseGetGrabX(const MouseState& state) const { return state.grabx; } //absolute location where you last started grabbing (x)
    int mouseGetGrabX(MouseButton button = LMB) const { return mouseGetGrabX(_mouseState[button]); }
    int mouseGetGrabY(const MouseState& state) const { return state.graby; }
    int mouseGetGrabY(MouseButton button = LMB) const { return mouseGetGrabY(_mouseState[button]); }
    int mouseGetRelGrabX(const MouseState& state) const { return state.grabrelx; } //relative location where you last started grabbing (x)
    int mouseGetRelGrabX(MouseButton button = LMB) const { return mouseGetRelGrabX(_mouseState[button]); }
    int mouseGetRelGrabY(const MouseState& state) const { return state.grabrely; }
    int mouseGetRelGrabY(MouseButton button = LMB) const { return mouseGetRelGrabY(_mouseState[button]); }
    
    bool mouseJustDown(bool& prevstate, MouseButton button = LMB) const; //generalized version with only boolean given
    bool mouseJustDown(MouseState& state, MouseButton button = LMB) const; //mouse down for the first time after being up or not over the element
    bool mouseJustDown(MouseButton button = LMB);
    
    bool mouseJustDownHere(bool& prevstate, MouseButton button = LMB) const;
    bool mouseJustDownHere(MouseState& state, MouseButton button = LMB) const; //mouse down for the first time after being up, only returns true if the mouse was above it before you clicked already
    bool mouseJustDownHere(MouseButton button = LMB);
    
    bool mouseJustUpHere(MouseState& state, MouseButton button = LMB) const; //mouse up for first time after being down, and over the element (so if you have mouse down on element and move mouse away, this will NOT return true, only if you release mouse button while cursor is over it, and mousedownhere)
    bool mouseJustUpHere(MouseButton button = LMB);
    
    bool pressed(MouseButton button = LMB); //mouseJustDown and active
    bool clicked(MouseButton button = LMB); //mouseJustUp and active

    bool mouseScrollUp() const; //scrolled up while on this element
    bool mouseScrollDown() const; //scrolled down while on this element
    
    double doubleClickTime; //maximum time for a doubleclick
    void setDoubleClickTime(double i_doubleClickTime) { doubleClickTime = i_doubleClickTime; }
    double getDoubleClickTime() { return doubleClickTime; }
    
    bool mouseDoubleClicked(MouseState& state, MouseButton button = LMB) const; //double clicked on this element
    bool mouseDoubleClicked(MouseButton button = LMB);
    
    MouseState& getMouseStateForContainer() { return mouse_state_for_containers; }
    
    protected:
    
    MouseState _mouseState[NUM_MOUSE_BUTTONS];
    MouseState mouse_state_for_containers; //for bookkeeping by containers that contain this element
};

class Element;

class ToolTipManager //this is made to draw the tooltip at the very end to avoid other gui items to be drawn over it
{
  public:
    void registerMe(const Element* element); //can register only one per frame (last one to register will get drawn); guiElements should do this in the draw function.
    void draw() const; //this will call the drawTooltip function of the one registered element, call this after drawing all gui elements
    void enableToolTips(bool set) { enabled = set; }
    ToolTipManager() : element(0), enabled(true) {}
  private:
    const Element* element;
    bool enabled;
};

extern ToolTipManager defaultTooltipManager;

/*
Possible things to check if your gui::Elements aren't behaving as they should:

-Check sizex and sizey
-The gui element constructors and make functions must be called AFTER the textures are loaded, otherwise the textures have size 0 and thus the gui element will have size 0
-mouse functions like pressed() work only once per time, pressed will return "true" only once per guielement, until the mouse button is up again and then down again
*/
class Element : public BasicElement
{
  public:
    Element(); //constructor
    virtual ~Element() { };
    bool selfActivate;
    
    ////minimum size
    int minSizex; //you can't resize this element to something smaller than this
    int minSizey;

    bool isVisible() { return visible; } //if false, the draw() function doesn't draw anything
    bool isActive() { return active; } //if false, handle() does nothing, and mouse tests return always false
    bool isPresent() { return present; } //if true, it reacts to the mouse. if false, it ignores the mouse, even if forceActive is true, if a gui element isn't present, it really isn't present
    void setVisible(bool i_visible) { visible = i_visible; }
    void setActive(bool i_active) { active = i_active; }
    void setPresent(bool i_present) { present = i_present; }
    void totallyDisable() {visible = active = present = false;} //this sets visible, active and present all at once
    void totallyEnable() {visible = active = present = true;}
    
    ////mouse overrides
    virtual bool mouseOver() const;
    virtual bool mouseGrabbable() const;
    virtual bool mouseActive() const { return active; }
    
    ////core functions of gui::Elements
    void draw() const; //will draw the actual widget, and if it's enabled, the label, do NOT overload this function
    virtual void drawWidget() const = 0; //called by draw(), this one can be overloaded for each widget defined below
    void handle();
    virtual void handleWidget();
    void move(int x, int y);
    virtual void moveWidget(int /*x*/, int /*y*/); //Override this if you have subelements, unless you use getAutoSubElement.
    
    void autoActivate();
    
    void moveTo(int x, int y);
    void moveCenterTo(int x, int y);
    void resize(int x0, int y0, int x1, int y1); //especially useful for windows and their container; parameters are the new values for x0, y0, x1 and y1 so this function can both move the object to a target and resize
    void growX0(int d) { resize(x0 + d, y0    , x1    , y1    ); } //growing can also be shrinking
    void growY0(int d) { resize(x0    , y0 + d, x1    , y1    ); }
    void growX1(int d) { resize(x0    , y0    , x1 + d, y1    ); }
    void growY1(int d) { resize(x0    , y0    , x1    , y1 + d); }
    void growSizeX0(int sizex) { resize(x1 - sizex, y0        , x1        , y1        ); } //growing can also be shrinking
    void growSizeY0(int sizey) { resize(x0        , y1 - sizey, x1        , y1        ); }
    void growSizeX1(int sizex) { resize(x0        , y0        , x0 + sizex, y1        ); }
    void growSizeY1(int sizey) { resize(x0        , y0        , x1        , y0 + sizey); }
    virtual void resizeWidget(const Pos<int>& newPos); //always called after resize, will resize the other elements to the correct size. Override this if you have subelements, unless you use getAutoSubElement.
    virtual bool isContainer() const; //returns 0 if the type of element isn't a container, 1 if it is (Window, Container, ...); this value is used by for example Container: it brings containers to the top of the screen if you click on them. Actually so far it's only been used for that mouse test. It's something for containers, by containers :p
    void putInScreen(); //puts element in screen if it's outside

    ////parameters for "sticky" system
    Pos<Sticky> sticky; //the sticky value for each of the 4 sides of this child widget
    Pos<double> relativePos; //position in coordinates in range [0.0, 1.0], relative to parent element's size, for the RELATIVE sticky types
    
    ////optional label //TODO: remove this, instead only give elements that really need a label (such as checkbox) a label
    std::string label;
    int labelX; //label position is relative to the position of the element
    int labelY;
    Markup labelMarkup;
    void drawLabel() const;
    void makeLabel(const std::string& label, int labelX, int labelY, const Markup& labelMarkup);
    
    ////optional tooltip. Drawing it must be controlled by a higher layer, e.g. see the Container's implementation. //TODO: make tooltip class and only have pointer to it here, being 0 if no tooltip, to decrease memory footprint of element
    std::string tooltip;
    bool tooltipenabled;
    void addToolTip(const std::string& text, ToolTipManager* tooltipmanager = &defaultTooltipManager) { tooltipenabled = true; tooltip = text; this->tooltipmanager = tooltipmanager;}
    void removeToolTip() { tooltipenabled = false; }
    void drawToolTip() const;
    ToolTipManager* tooltipmanager;
    
    
    virtual void setElementOver(bool state); //ALL gui types that have gui elements inside of them, must set elementOver of all gui elements inside of them too! ==> override this virtual function for those. Override this if you have subelements, unless you use getAutoSubElement.
    bool hasElementOver() const;

    ////special visible parts, for example for debugging
    void drawBorder(const ColorRGB& color = RGB_White) const;
    
    //add background rectangle
    void addBackgroundRectangle(const ColorRGB& color) { this->hasBackgroundRectangle = true; this->backgroundRectangleColor = color; }
    
    bool isNotDrawnByContainer() { return notDrawnByContainer; }
    void setNotDrawnByContainer(bool set) { notDrawnByContainer = set; }
    
    private:
    
    //rectangle behind it
    bool hasBackgroundRectangle; //TODO: remove this feature, only give elements that need a background rectangle something like this
    ColorRGB backgroundRectangleColor;
    
    MouseState auto_activate_mouse_state; //for the autoActivate() function //TODO: make this pointer, to save memory. If it's 0, no selfactivate
    
    protected:
    void addSubElement(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT); //only used for INTERNAL parts of the gui element, such as the buttons in a scrollbar, hence this function is protected
    
    bool elementOver; //true if there is an element over this element, causing the mouse NOT to be over this one (Z-order related)

    bool visible; //if false, the draw() function doesn't draw anything
    bool active; //if false, handle() does nothing, and mouse tests return always false
    bool present; //if true, it reacts to the mouse. if false, it ignores the mouse, even if forceActive is true, if a gui element isn't present, it really isn't present

    //TODO: remove notDrawnByContainer feature: things are handled by internalcontainer now for sub elements, which never draws elements anyway
    bool notDrawnByContainer; //default false. If true, this element won't be drawn by the container. You have to draw it yourself. Advantage: you can choose when it's drawn, allowing determining drawing-order of non-gui things intermixed with gui things.
    
    InternalContainer ic; //TODO: make this a pointer and use new and delete, to decrease memory size of elements that don't use the internal container
};

//Dummy = exactly the same as Element but not abstract, nothing implemented except pure virtuals of Element
class Dummy : public Element
{
  void drawWidget() const {}
};

class Button : public Element
{
  /*
  the button has 3 separate graphical elements:
  *) text
  *) an image
  *) a panel (resisable rectangle with sides)
  */
  
  public:
    Button();
    ////part "back image"
    bool enableImage;
    const Texture* image[3]; //0=normal, 1=mouse over, 2=mouse down
    int imageOffsetx;
    int imageOffsety;
    ColorRGB imageColor[3]; //0=normal, 1=mouse over, 2=mouse down
    
    ////part "front image"
    bool enableImage2;
    const Texture* image2[3]; //0=normal, 1=mouse over, 2=mouse down
    int imageOffsetx2;
    int imageOffsety2;
    ColorRGB imageColor2[3]; //0=normal, 1=mouse over, 2=mouse down

    ////part "text"
    bool enableText;
    std::string text;
    int textOffsetx;
    int textOffsety;
    Markup markup[3];
    void autoTextSize(int extrasize = 0); //will automaticly adjust it's size to fit text size
    void centerText(); //center the text in the texture if the button has a texture (sizex and sizey used for size)
    
    ////part "panel"
    bool enablePanel;
    const BackPanel* panel[3];
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
    //make full
    void make(int x, int y, int sizex, int sizey, //basic properties
              bool enableImage, Texture* texture1, Texture* texture2, Texture* texture3, int imageOffsetx, int imageOffsety, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3, //image
              bool enableText, const std::string& text, int textOffsetx, int textOffsety, const Markup& markup1, const Markup& markup2, const Markup& markup3, //text
              bool enablePanel, const BackPanel* panel1 = &builtInPanel[1], const BackPanel* panel2 = &builtInPanel[2], const BackPanel* panel2 = &builtInPanel[3], int panelOffsetx = 0, int panelOffsety = 0); //panel
    
    //image only constructor (without offset)
    void makeImage(int x, int y,
                   const Texture* texture1, const Texture* texture2, const Texture* texture3, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey); //image
    
    void makeImage(int x, int y,  const Texture* texture123, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey);
    
    void addFrontImage(const Texture* texture1, const Texture* texture2, const Texture* texture3,
                       const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey);
    
    void addFrontImage(const Texture* texture);
    
    //text only constructor (without offset)
    void makeText(int x, int y, //basic properties
                  const std::string& text, //text
                  const GuiSet* set = &builtInGuiSet);
    
    //panel + text constructor (text always in center of panel, no offsets and thinking needed)
    //this is the constructor with default parameters
    void makeTextPanel(int x, int y, const std::string& text = "", int sizex = 64, int sizey = 24, //basic properties + actual text
                       const GuiSet* set = &builtInGuiSet); //panel

    virtual void drawWidget() const;
    
    private:
    mutable MouseState mutable_button_drawing_mouse_test;
};

//The Scrollbar
class Scrollbar : public Element
{
  private:
    double oldTime;
    void init();
  public:
    //get length and begin and end coordinates of the slider part (the part between the up and down buttons) (relative to x, y of the scrollbar)
    int getSliderSize() const;
    int getSliderStart() const;
    int getSliderEnd() const;
    
    virtual void handleWidget();
    virtual void drawWidget() const;

    Direction direction; //0 = vertical, 1 = horizontal
    double scrollSize; //length of the total scrollbar (in steps)
    double scrollPos; //position of the scroller on the bar (in steps)
    double scrollSpeed; //if speedMode == 0: steps / second, if speedMode == 1: seconds / whole bar
    double absoluteSpeed; //steps / second
    
    int speedMode;
    
    const Texture* txUp; //texture of the "up" button (left button if it's horizontal)
    const Texture* txUpOver; //on mouseover
    const Texture* txDown; //texture of the "down" button (right button if horizontal)
    const Texture* txDownOver; //mouseover
    const Texture* txScroller; //texture of the center button (the scroller)
    const Texture* txScrollerOver; //mouseover
    const Texture* txBack; //texture of the background (the slider)
        
    //only one shared color modifier and a color modifier on mouseover/down
    ColorRGB colorMod;
    ColorRGB colorModOver;
    ColorRGB colorModDown;
    
    //buttons of the scrollbar
    Button buttonUp;
    Button buttonDown;
    Button scroller;
    
    void setRelativeScrollSpeed(); //time = time to scroll from top to bottom (in seconds)
    void setRelativePosition(float position); //position = 0.0-1.0

    Scrollbar();
    void makeVertical(int x, int y, int length = 80,
                      double scrollSize = 100, double scrollPos = 0, double offset = 0, double scrollSpeed = 1,
                      const GuiSet* set = &builtInGuiSet, int speedMode = 1);
    void makeHorizontal(int x, int y, int length = 80,
                        double scrollSize = 100, double scrollPos = 0, double offset = 0, double scrollSpeed = 1,
                        const GuiSet* set = &builtInGuiSet, int speedMode = 1);
    void showValue(int x, int y, const Markup& valueMarkup, int type); //type: 0=don't, 1=float, 2=int
    
    void scroll(int dir); //make it scroll from an external command
    
    double offset; //used as an offset of ScrollPos to get/set the scroll value with offset added with the functions below
    double getValue() const;
    void setValue(double value);
    void randomize(); //it will get a random value
    
    int enableValue; //if 1, value is shown everytime you draw as floating point, if 2, as integer (value = offset + scrollPos)
    Markup valueMarkup; //text style of the value
    int valueX; //x position of the value (relative)
    int valueY; //y position of the value (relative)
    
    bool forWardedMouseScrollUp() const; //see int forwardedScroll;
    bool forWardedMouseScrollDown() const; //see int forwardedScroll;
    void forwardScroll(int scroll); //see int forwardedScroll;
    
    protected:
    
    mutable int forwardedScroll; //forwarded mouse scroll event from other element; +1 is down, -1 is up
};

class ScrollbarPair : public Element
{
  private:
    const GuiSet* scrollbarGuiSet; //the guiSet used for the scrollbars has to be remembered for when remaking them
  public:
    ScrollbarPair();
    void make(int x, int y, int sizex, int sizey, double scrollSizeH = 100, double scrollSizeV = 100,
              const GuiSet* set = &builtInGuiSet);
        
    Scrollbar vbar;
    Scrollbar hbar;
    
    const Texture* txCorner; //the corner piece between the two scrollbars
        
    virtual void handleWidget();
    virtual void drawWidget() const;
    
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
class Slider : public Element
{
  public:
    Slider();
    double getValue() const; //a number between 0.0 and scrollSize
    void setValue(double value);
    double getRelValue() const; //a number between 0.0 and 1.0
    void setRelValue(double value);
        
    Direction direction;
    
    Button slider; //(by default round) button
    const BackRule* ruler; //the line behind the button
    
    double scrollSize;
    double scrollPos;

    void makeHorizontal(int x, int y, int length = 100, double scrollSize = 100, const GuiSet* set = &builtInGuiSet);
    void makeVertical(int x, int y, int length = 100, double scrollSize = 100, const GuiSet* set = &builtInGuiSet);
    
    double screenPosToScrollPos(int screenPos);
    int scrollPosToScreenPos(double scrollPos);

    virtual void drawWidget() const;
    virtual void handleWidget();
};

class Invisible : public Element
{
  public:
  void make(int x0, int y0, int x1, int y1);

  virtual void drawWidget() const;
};

class Container : public Element
{
  protected:
    
    void drawElements() const;
    
    InternalContainer elements;
    
    virtual bool mouseInVisibleZone() const; //is the mouse in the zone where elements are drawn
    
  public:
    
    Container();
    virtual void handleWidget(); //you're supposed to handle() before you draw()
    virtual void drawWidget() const;
    
    //push the element without affecting absolute position
    void pushTop(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void pushBottom(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void insert(size_t pos, Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    
    //push the element so that its top left is relative to the top left of this container, thus moving it if the container isn't at 0,0
    void pushTopRelative(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void pushBottomRelative(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void insertRelative(size_t pos, Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);

    //push the element at the given x, y (relative to this container's top left)
    void pushTopAt(Element* element, int x, int y, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void pushBottomAt(Element* element, int x, int y, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void insertAt(size_t pos, Element* element, int x, int y, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    
    Element* getElement(size_t i) const { return elements.getElement(i); }

    void bringToTop(Element* element); //precondition: element must already be in the list
    
    void centerElement(Element* element);

    void remove(Element* element);
    unsigned long size() const;
    virtual bool isContainer() const;
    void clear(); //clears all the elements
    void putInside(unsigned long i);
    virtual void moveWidget(int x, int y);
    void make(int x, int y, int sizex, int sizey);
    
    void getRelativeElementPos(Element& element, int& ex, int& ey) const;
    virtual void resizeWidget(const Pos<int>& newPos);
    
    virtual void setElementOver(bool state);
    
    void setSizeToElements(); //makes the size of the container as big as the elements. This resizes the container in a non-sticky way: no element is affected
};

class ScrollElement : public Element
{
  /*
  ScrollElement will contain 1 element. Depending on the size of that element compared to this ScrollElement,
  1 or 2 scrollbars will appear, allowing to scroll to see all of that element.
  The size of that element isn't affected by this ScrollElement.
  The position of that element is completely controlled by this ScrollElement or its scrollbars
  */
  public:
    Element* element;
  
  
    ScrollElement();
    virtual void handleWidget(); //you're supposed to handle() before you draw()
    virtual void drawWidget() const;
    virtual void resizeWidget(const Pos<int>& newPos);
    virtual void moveWidget(int x, int y);
    virtual void setElementOver(bool state);

    void make(int x, int y, int sizex, int sizey, Element* element);
    
    ////everything concerning the scrollability
    ScrollbarPair bars;

    //the zone where elements are drawn: the size of this container excluding the scrollbarpair's bars
    int getVisibleSizex() const;
    int getVisibleSizey() const;
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
    
  protected:
  
    virtual bool mouseInVisibleZone() const; //is the mouse in the zone where elements are drawn
    
    void initBars();
    void updateBars();
    void toggleBars(); //turns the bars on or of depending on if they're needed or not
};

class Group : public Container
{
  public:
    virtual bool mouseOverShape() const; //difference with the mouseOverShape from other guielements, is that it checks all sub elements, not itself, for mouseovers
};

class Panel : public Element
{
  private:
    BackPanel panel;

  public:
    Panel();
    void make(int x, int y, int sizex, int sizey,
              const GuiSet* set = &builtInGuiSet);
    void makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor);
    //give 1 texture, the other 8 are assumed to have successive memory locations
    void makeTextured(int x, int y, int sizex, int sizey,
                      const Texture* t00, const ColorRGB& colorMod = RGB_White);
    void setSize(int x, int y, int sizex, int sizey);
         
    virtual void drawWidget() const;
};

class Rule : public Element
{
  private:
    BackRule line;
  
  public:
    Rule();
         
  //give 3 separate textures
  void makeHorizontal(int x, int y, int length, const GuiSet* set = &builtInGuiSet);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeHorizontal1(int x, int y, int length, Texture* t0=&builtInTexture[41], const ColorRGB& colorMod=RGB_White);
       
  //give 3 separate textures
  void makeVertical(int x, int y, int length, const GuiSet* set = &builtInGuiSet);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeVertical1(int x, int y, int length, Texture* t0=&builtInTexture[44], const ColorRGB& colorMod=RGB_White);
  
  void setSize(int x, int y, int length);

  virtual void drawWidget() const;
};

//Window is a container for other gui elements that'll move and get drawn at the command of the window
class Window : public Element
{
  protected:
    Container container;
    ScrollElement scroll;
    
  public:
    void disableCenterTexture() { panel.enableCenter = false; }
    void setFillColor(const ColorRGB& color) { panel.fillColor = color; }
  
    Window();
    
    ////obligatory part "panel"
    BackPanel panel;
    
    ////obligatory part "container"
    int getContainerLowest() const;
    int getContainerHighest() const;
    int getContainerLeftmost() const;
    int getContainerRightmost() const;
    void initContainer();
    Container* getContainer() { return &container; }
    
    //get the parameters for the container surface on which the elements in the window will be
    int getContainerX0() const { return container.getX0(); }
    int getContainerY0() const { return container.getY0(); }
    int getContainerX1() const { return container.getX1(); }
    int getContainerY1() const { return container.getY1(); }
    int getContainerSizex() const { return container.getSizex(); }
    int getContainerSizey() const { return container.getSizey(); }
    
    
    //if a parameter is -1, it's set to left
    void setContainerBorders(int left = 0, int up = -1, int right = -1, int down = -1);
    
    //push the element without affecting absolute position
    void pushTop(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void pushBottom(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void insert(size_t pos, Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    
    //push the element so that its top left is relative to the top left of this container, thus moving it if the container isn't at 0,0
    void pushTopRelative(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void pushBottomRelative(Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void insertRelative(size_t pos, Element* element, const Pos<Sticky>& sticky = STICKYTOPLEFT);

    //push the element at the given x, y (relative to this container's top left)
    void pushTopAt(Element* element, int x, int y, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void pushBottomAt(Element* element, int x, int y, const Pos<Sticky>& sticky = STICKYTOPLEFT);
    void insertAt(size_t pos, Element* element, int x, int y, const Pos<Sticky>& sticky = STICKYTOPLEFT);

    void bringToTop(Element* element); //precondition: element must already be in the list
    
    void remove(Element* element);
    unsigned long size() const;
    void putInside(int i);
    
    //these scrollbars will be part of the container
    void addScrollbars();
    void removeScrollbars();
    
    ////optional part "top"
    Rule top; //not a "back" one, so that you can easily detect mouse on it, for dragging
    void addTop(Texture * t0 = &builtInTexture[47], int offsetLeft = 0, int offsetRight = 0, int offsetTop = 0, const ColorRGB& colorMod = ColorRGB(96, 96, 255));
    bool enableTop; //enable the top bar of the window (then you can drag it with this instead of everywhere on the window)

    ////optional part "title"
    std::string title;
    int titleX; //position of title, relative to the top bar (NOT relative to the window but to the TOP BAR!)
    int titleY;
    Markup titleMarkup;
    void addTitle(const std::string& title, int titleX = 2, int titleY = 4, const Markup& titleMarkup = TS_W);
    void setTitle(const std::string& title); //only to be used after "addTitle" (or the x, y position will be messed up)
    
    ////optional part "close button"
    bool closed;
    Button closeButton;
    void addCloseButton(int offsetX = 0, int offsetY = 0, const GuiSet* set = &builtInGuiSet); //ofsset from top *right* corner, choose style of close button by making it, it's the built in texture by default
    bool closeEnabled; //close button is enabled
    
    ////optional part "resizer" = draggable bottom right corner with diagonally striped image
    bool enableResizer;
    Button resizer;
    void addResizer(const GuiSet* set = &builtInGuiSet, bool overContainer = false, int offsetX = 0, int offsetY = 0); //ofsset = from bottom right corner
    bool resizerOverContainer;
    ////the make functions
    void make(int x, int y, int sizex, int sizey,
              const GuiSet* set = &builtInGuiSet);
    void makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor);
    void makeTextured(int x, int y, int sizex, int sizey,
                      const Texture* t00, const ColorRGB& colorMod = RGB_White);
         
    int getRelContainerStart() const { return container.getY0() - y0; }
    
    int getRelContentStart() const;
    
    void setSize(int x, int y, int sizex, int sizey);
    
    ////overloaded functions
    virtual void drawWidget() const;
    
    virtual void handleWidget();
    virtual bool isContainer() const;
    
    ////useful for the close button
    void close() { closed = 1; totallyDisable(); } //use this if closed == 1
    void unClose() { closed = 0; totallyEnable(); }
    void toggleClose() { if(closed) unClose(); else close(); }
    
    void setColor(const ColorRGB& color) { panel.colorMod = color; }
};

//The Checkbox
class Checkbox : public Element
{
  private:
    int textOffsetX;
    int textOffsetY;
    //bool downAndTested; //if mouse is down and that is already handled, leave this on so that it'll ignore mouse till it's back up
    void positionText(); //automaticly place the text a few pixels next to the checkbox, in the center
  
  public:
    bool checked; //if true: checked, if false: unchecked
    const Texture* texture[4]; //texture when off, mouseover&off, and checked, mouseover&checked
    ColorRGB colorMod[4];
    int toggleOnMouseUp;
    
    //front image
    bool enableImage2;
    const Texture* texture2[4]; //texture when off, mouseover&off, and checked, mouseover&checked
    ColorRGB colorMod2[4];
    void addFrontImage(const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, 
                       const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Grey, const ColorRGB& imageColor3 = RGB_White, const ColorRGB& imageColor4 = RGB_Grey);
    void addFrontImage(const Texture* texture);
    
    //text
    bool enableText; //the text is a title drawn next to the checkbox, with automaticly calculated position
    std::string text;
    Markup markup;
    
    Checkbox();
    void make(int x, int y, bool checked = 0, const GuiSet* set = &builtInGuiSet, int toggleOnMouseUp = 0);
    void addText(const std::string& text, const Markup& markup = TS_W);
    virtual void drawWidget() const; //also handles it by calling handle(): toggles when mouse down or not
    virtual void handleWidget();

    void setText(const std::string& newText);
    const std::string& getText() const { return text; }
    void toggle();
    void check() { checked = true; }
    void uncheck() { checked = false; }
    bool isChecked() { return checked; }
    
    //give it alternative textures than those in the GuiSet
    void setTexturesAndColors(const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, 
                              const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4);
    //without mouseOver effect
    void setTexturesAndColors(const Texture* texture1, const Texture* texture2, 
                              const ColorRGB& color1, const ColorRGB& color2);
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
    
    Texture* texture; //the texture of this state
    ColorRGB colorMod;
    
    bool enableText; //the text is a title drawn next to the checkbox, with automaticly calculated position
    std::string text;
    Markup markup;
    
    void make(Texture* texture, const ColorRGB& colorMod, const std::string& text = "", const Markup& markup = TS_W);
};

//circle between N states (you can add states, the make function makes no states). Left mouse click goes to next state, right mouse click goes to previous state.
class NState : public Element
{
  private:
  public:
    unsigned long state;
    std::vector<NStateState> states;
    int toggleOnMouseUp;
        
    NState();
    void make(int x, int y, int toggleOnMouseUp = 0);
    void addState(Texture* texture, const ColorRGB& colorMod = RGB_White, const std::string& text = "", const Markup& markup = TS_W);
    virtual void drawWidget() const; //also handles it by calling handle(): toggles when mouse down or not
    virtual void handleWidget();
};

//The bulletlist, a list of checkboxes where only one can be selected
class BulletList : public Element
{
  public:
    std::vector <Checkbox> bullet;
    Checkbox prototype;
    
    BulletList();
    virtual void drawWidget() const;
    virtual void handleWidget();
    void make(int x, int y, unsigned long amount, int xDiff, int yDiff, const GuiSet* set = &builtInGuiSet); //diff = the location difference between successive checkboxes
    void make(int x, int y, unsigned long amount, int xDiff, int yDiff, unsigned long amountx, const GuiSet* set = &builtInGuiSet); //make in 2D pattern
    void setCorrectSize();
    //set style of the bullets by using prototype.make([checkbox parameters where x and y will be ignored])
    int check(); //returns which one is checked
    int xDiff; //just added for "book keeping"
    int yDiff; //just added for "book keeping"

    std::string getText(unsigned long i) const;
    const std::string& getCurrentText() const;
    void addText(const std::string& text, unsigned long i);
    void set(unsigned long i);
    
  private:
    int lastChecked; //remember which one was checked, so it can know when a new one is checked, which one that is
};

class Text : public Element
{
  public:
    bool useNewLine;
    Markup markup;
    virtual void drawWidget() const;
    Text();
    void make(int x = 0, int y = 0, const std::string& text = "", const Markup& markup = TS_W);
    void setText(const std::string& text);
    const std::string& getText() const { return text; }
  private:
    std::string text;
};


class Image : public Element
{
  public:
    Texture* image;
    ColorRGB colorMod;
    virtual void drawWidget() const;
    Image();
    void make(int x, int y, Texture* image=&builtInTexture[37], const ColorRGB& colorMod = RGB_White);
    void make(int x, int y, int sizex, int sizey, Texture* image=&builtInTexture[37], const ColorRGB& colorMod = RGB_White);
};

/*
a way of unit testing all the GUI classes and mouse behaviour
*/
void unitTest();

} //namespace gui
} //namespace lpi

#endif
