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
lpi_gui: an OpenGL GUI
*/

#ifndef LPI_GUI_H_INCLUDED
#define LPI_GUI_H_INCLUDED

#include <map>

#include "lpi_gui_base.h"

#include "lpi_color.h"
#include "lpi_texture.h"
#include "lpi_text.h"

#include "lpi_gui_drawer.h"
#include "lpi_gui_drawer_gl.h" //TODO: make lpi_gui independent of this header inclusion (requires removing all GuiSet references)

/*
TODO: all "BackPanel", "BackRule", "GuiSet", "Texture(GL)", ... will have to go away here!
"ITexture" is allowed, but no implementations like "Texture(GL)"
*/

namespace lpi
{
namespace gui
{

class Element;

class InternalContainer //container inside elements, for elements that contain sub elements to work (e.g. scrollbar exists out of background and 3 buttons)
{
  private:
    std::vector<Element*> elements;
    
    std::map<Element*, Sticky> sticky;
  
  public:
  
    const std::vector<Element*>& getElements() const { return elements; }
    std::vector<Element*>& getElements() { return elements; }
    
    void resize(const Pos<int>& oldPos, const Pos<int>& newPos); //this resizes the 2D size of elements, not the amount of elements
    void move(int x, int  y);
    void setElementOver(bool state); //this says to all elements whether or not another element is in front of it in Z order, causing mouse to not work
    void setBasicMouseInfo(const IGUIInput& input);

    void addSubElement(Element* element, const Sticky& sticky, Element* parent);
    void insertSubElement(size_t index, Element* element, const Sticky& sticky, Element* parent);
    
    Element* getElement(size_t i) const { return elements[i]; }
    
    size_t findElementIndex(Element* element);
    void removeElement(Element* element);
    
    void setSticky(Element* element, const Sticky& sticky, Element* parent);
    void setStickyRelative(Element* element, Element* parent); //automatically calculates the sticky for completely relative position, based on the current sizes
    void setStickyFull(Element* element, Element* parent); //automatically calculates the sticky for "full" behaviour, sides will follow parent's corresponding size, compared to current position
  
  private:
    void initSubElement(Element* element, const Sticky& sticky, Element* parent);
    void setStickyElementSize(Element* element, Element* parent);
    void setStickyElementSize(Element* element, const Pos<int>& newPos);
};

class ToolTipManager //this is made to draw the tooltip at the very end to avoid other gui items to be drawn over it
{
  private:
    const Element* element;
    bool enabled;
    
  public:
    void registerMe(const Element* element); //can register only one per frame (last one to register will get drawn); guiElements should do this in the draw function.
    void draw(IGUIDrawer& drawer) const; //this will call the drawTooltip function of the one registered element, call this after drawing all gui elements
    void enableToolTips(bool set) { enabled = set; }
    ToolTipManager() : element(0), enabled(true) {}
};

extern ToolTipManager defaultTooltipManager;

/*
Possible things to check if your gui::Elements aren't behaving as they should:

-Check sizex and sizey
-The gui element constructors and make functions must be called AFTER the textures are loaded, otherwise the textures have size 0 and thus the gui element will have size 0
-mouse functions like pressed() work only once per time, pressed will return "true" only once per guielement, until the mouse button is up again and then down again
*/
class Element : public ElementRectangular
{
  private:
  
    MouseState mouse_state_for_containers; //for bookkeeping by containers that contain this element TODO: make container itself remember one per element
    
  protected:
    
    
    //TODO: try to eliminate some of these booleans. Visible and Active can go together. ElementOver is needed of course for the correct handling of elements in containers. Present can maybe be comined with active and visible too???
    bool elementOver; //true if there is an element over this element, causing the mouse NOT to be over this one (Z-order related)
    bool visible; //if false, the draw() function doesn't draw anything
    bool active; //if false, handle() does nothing, and mouse tests return always false
    bool present; //if true, it reacts to the mouse. if false, it ignores the mouse, even if forceActive is true, if a gui element isn't present, it really isn't present

    ////data for tooltip, TODO: move this to somewhere else
    std::string tooltip;
    bool tooltipenabled;
    ToolTipManager* tooltipmanager;

  public: //TODO: make these not-public
    
    ////minimum size
    int minSizeX; //you can't resize this element to something smaller than this
    int minSizeY; //TODO: make this virtual private functions instead of member variables, or even better, remove this completely and make the resize function of elements determine this
    
  protected:
    void autoActivate(const IGUIInput& input, MouseState& auto_activate_mouse_state, bool& control_active); //utility function used by text input controls, they need a member variable like "MouseState auto_activate_mouse_state" in them for this and a bool "control_active", and in their handleWidget, put, "autoActivate(input, auto_activate_mouse_state, control_active); if(!control_active) return;"
    
  public:
    Element();
    virtual ~Element(){};
    
    bool isVisible() { return visible; } //if false, the draw() function doesn't draw anything
    bool isActive() { return active; } //if false, handle() does nothing, and mouse tests return always false
    bool isPresent() { return present; } //if true, it reacts to the mouse. if false, it ignores the mouse, even if forceActive is true, if a gui element isn't present, it really isn't present
    void setVisible(bool i_visible) { visible = i_visible; }
    void setActive(bool i_active) { active = i_active; }
    void setPresent(bool i_present) { present = i_present; }
    void totallyDisable(); //this sets visible, active and present all at once
    void totallyEnable();
    
    ////mouse overrides
    virtual bool mouseOver(const IGUIInput& input) const;
    virtual bool mouseGrabbable() const;
    virtual bool mouseActive() const { return active; }
    
    ////core functions of gui::Elements
    void draw(IGUIDrawer& drawer) const; //will draw the actual widget and do some other always-to-do stuff, do NOT override this function
    void handle(const IGUIInput& input);
    virtual void move(int x, int y);
    virtual void resize(int x0, int y0, int x1, int y1); //especially useful for windows and their container; parameters are the new values for x0, y0, x1 and y1 so this function can both move the object to a target and resize
    
    virtual void drawWidget(IGUIDrawer& drawer) const = 0; //called by draw(), this one can be overloaded for each widget defined below
    virtual void handleWidget(const IGUIInput& input);
    virtual void moveWidget(int x, int y); //Override this if you have subelements, unless you use addSubElement in ElementComposite.
    
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
    virtual void resizeWidget(const Pos<int>& newPos); //always called after resize, will resize the other elements to the correct size. Override this if you have subelements, unless you use addSubElement in ElementComposite.
    virtual bool isContainer() const; //returns 0 if the type of element isn't a container, 1 if it is (Window, Container, ...); this value is used by for example Container: it brings containers to the top of the screen if you click on them. Actually so far it's only been used for that mouse test. It's something for containers, by containers :p
    void putInScreen(); //puts element in screen if it's outside
    
    ////optional tooltip. Drawing it must be controlled by a higher layer, e.g. see the Container's implementation. //TODO: make tooltip class and only have pointer to it here, being 0 if no tooltip, to decrease memory footprint of element, OR, store every info of the tooltip in ToolTipManager
    void addToolTip(const std::string& text, ToolTipManager* tooltipmanager = &defaultTooltipManager) { tooltipenabled = true; tooltip = text; this->tooltipmanager = tooltipmanager;}
    void removeToolTip() { tooltipenabled = false; }
    void drawToolTip(IGUIDrawer& drawer) const; //TODO: move this function to ToolTipManager
    
    virtual void setElementOver(bool state); //ALL gui types that have gui elements inside of them, must set elementOver of all gui elements inside of them too! ==> override this virtual function for those. Override this if you have subelements, unless you use addSubElement in ElementComposite.
    bool hasElementOver() const;
    
    MouseState& getMouseStateForContainer() { return mouse_state_for_containers; }

    ////for debugging: if you've got no idea what's going on with a GUI element, this function at least is guaranteed to show where it is (if in screen)
    void drawDebugBorder(IGUIDrawer& drawer, const ColorRGB& color = RGB_Red) const;
    void drawDebugCross(IGUIDrawer& drawer, const ColorRGB& color = RGB_Red) const;
    
    void drag(const IGUIInput& input, GUIMouseButton button = GUI_LMB);
};

class ElementComposite : public Element //element with "internal container" to automatically handle child elements for you
{
  protected:
    InternalContainer ic;
    
  protected:
    void addSubElement(Element* element, const Sticky& sticky = STICKYDEFAULT); //only used for INTERNAL parts of the gui element, such as the buttons in a scrollbar, hence this function is protected
  
  public:
    virtual void move(int x, int y);
    virtual void setElementOver(bool state);
    virtual void resize(int x0, int y0, int x1, int y1);
};

class Label //convenience class: elements that want an optional label (e.g. checkboxes) can inherit from this and use drawLabel(this) in their drawWidget function
{
  private:
    std::string label;
    int labelX; //label position is relative to the position of the element
    int labelY;
    Markup labelMarkup;
  
  protected:
    void drawLabel(const Element* element) const;
    
  public:
    Label();
    virtual ~Label(){}
    
    void makeLabel(const std::string& label, int labelX, int labelY, const Markup& labelMarkup);
};


//Dummy = exactly the same as Element but not abstract, nothing implemented except pure virtuals of Element
class Dummy : public Element
{
  void drawWidget(IGUIDrawer& /*drawer*/) const {}
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
                   const Texture* texture1, const Texture* texture2, const Texture* texture3, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey); //image
    
    void makeImage(int x, int y,  const Texture* texture123, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey);
    
    void addFrontImage(const Texture* texture1, const Texture* texture2, const Texture* texture3,
                       const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey);
    
    void addFrontImage(const Texture* texture);
    
    //text only constructor (without offset)
    void makeText(int x, int y, //basic properties
                  const std::string& text); //text
    
    //panel + text constructor (text always in center of panel, no offsets and thinking needed)
    //this is the constructor with default parameters
    void makeTextPanel(int x, int y, const std::string& text = "", int sizex = 64, int sizey = 24); //basic properties + actual text

    virtual void drawWidget(IGUIDrawer& drawer) const;
    virtual void handleWidget(const IGUIInput& input);
    
    //mouse functions without having to give the IGUIInput
    /*bool clicked(GUIMouseButton button = GUI_LMB);
    bool pressed(GUIMouseButton button = GUI_LMB);*/
    
  private:
    MouseState button_drawing_mouse_test;
    bool draw_mouse_button_down_style;
};

//The Scrollbar
class Scrollbar : public ElementComposite
{
  private:
    double oldTime; //in seconds
    void init();
  public:
    //get length and begin and end coordinates of the slider part (the part between the up and down buttons) (relative to x, y of the scrollbar)
    int getSliderSize() const;
    int getSliderStart() const;
    int getSliderEnd() const;
    
    virtual void handleWidget(const IGUIInput& input);
    virtual void drawWidget(IGUIDrawer& drawer) const;

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
    
    void scroll(const IGUIInput& input, int dir); //make it scroll from an external command
    
    double offset; //used as an offset of ScrollPos to get/set the scroll value with offset added with the functions below
    double getValue() const;
    void setValue(double value);
    void randomize(); //it will get a random value
    
    int enableValue; //if 1, value is shown everytime you draw as floating point, if 2, as integer (value = offset + scrollPos)
    Markup valueMarkup; //text style of the value
    int valueX; //x position of the value (relative)
    int valueY; //y position of the value (relative)
    
    bool forwardedMouseScrollUp() const; //see int forwardedScroll;
    bool forwardedMouseScrollDown() const; //see int forwardedScroll;
    void forwardScroll(int scroll); //see int forwardedScroll;
    
  protected:
    
    mutable int forwardedScroll; //forwarded mouse scroll event from other element; +1 is down, -1 is up
};

class ScrollbarPair : public ElementComposite
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
        
    virtual void handleWidget(const IGUIInput& input);
    virtual void drawWidget(IGUIDrawer& drawer) const;
    
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
    void makeSmallHorizontal(int x, int y, int length = 100, double scrollSize = 100, const GuiSet* set = &builtInGuiSet);
    
    double screenPosToScrollPos(int screenPos);
    int scrollPosToScreenPos(double scrollPos);

    virtual void drawWidget(IGUIDrawer& drawer) const;
    virtual void handleWidget(const IGUIInput& input);
};

class Invisible : public Element
{
  public:
  void make(int x0, int y0, int x1, int y1);

  virtual void drawWidget(IGUIDrawer& drawer) const;
};

class Container : public ElementComposite
{
  protected:
    InternalContainer elements;

  protected:
    
    void drawElements(IGUIDrawer& drawer) const;
    
  public:
    
    Container();
    virtual void handleWidget(const IGUIInput& input); //you're supposed to handle() before you draw()
    virtual void drawWidget(IGUIDrawer& drawer) const;
    
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

class ScrollElement : public ElementComposite //a.k.a "ScrollZone"
{
  /*
  ScrollElement will contain 1 element (which can be a container with more elements).
  Depending on the size of that element compared to this ScrollElement,
  1 or 2 scrollbars will appear, allowing to scroll to see all of that element.
  The size of that element isn't affected by this ScrollElement.
  The position of that element is completely controlled by this ScrollElement or its scrollbars
  */
  public:
    Element* element;
  
    ScrollElement();
    virtual void handleWidget(const IGUIInput& input); //you're supposed to handle() before you draw()
    virtual void drawWidget(IGUIDrawer& drawer) const;
    virtual void resizeWidget(const Pos<int>& newPos);
    virtual void moveWidget(int x, int y);
    virtual void setElementOver(bool state);

    void make(int x, int y, int sizex, int sizey, Element* element);
    
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
  
  protected:
  
    virtual bool mouseInVisibleZone(const IGUIInput& input) const; //is the mouse in the zone where elements are drawn
    
    void initBars();
    void toggleBars(); //turns the bars on or of depending on if they're needed or not
};

class Group : public Container
{
  public:
    virtual bool isInside(int x, int y) const; //difference with the isInside from other guielements, is that it checks all sub elements, not itself, for mouseovers
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

    virtual void drawWidget(IGUIDrawer& drawer) const;
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

  virtual void drawWidget(IGUIDrawer& drawer) const;
};

//Window is a container for other gui elements that'll move and get drawn at the command of the window
class Window : public ElementComposite
{
  protected:
    Container container;
    ScrollElement scroll;
    ColorRGB colorMod;
    
  public:
  
    Window();
    
    ////container
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
    int getContainerSizeX() const { return container.getSizeX(); }
    int getContainerSizeY() const { return container.getSizeY(); }
    
    //if a parameter is -1, it's set to left
    void setContainerBorders(int left = 0, int up = -1, int right = -1, int down = -1);
    
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

    void bringToTop(Element* element); //precondition: element must already be in the list
    
    void remove(Element* element);
    unsigned long size() const;
    void putInside(int i);
    
    //these scrollbars will be part of the container
    void addScrollbars();
    void removeScrollbars();
    void updateScroll(); //call this after elements inside window changed size or position. It updates size of container to elements inside it, then updates scroll (updateBars()). I've had this work very well together with a zoomable and movable image inside a window with scrollbars!!
    
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
    virtual void drawWidget(IGUIDrawer& drawer) const;
    
    virtual void handleWidget(const IGUIInput& input);
    virtual bool isContainer() const;
    
    ////useful for the close button
    void close() { closed = 1; totallyDisable(); } //use this if closed == 1
    void unClose() { closed = 0; totallyEnable(); }
    void toggleClose() { if(closed) unClose(); else close(); }
    
    void setColor(const ColorRGB& color) { colorMod = color; }
};

//The Checkbox
class Checkbox : public Element, public Label
{
  private:
    int textOffsetX;
    int textOffsetY;
    //bool downAndTested; //if mouse is down and that is already handled, leave this on so that it'll ignore mouse till it's back up
    void positionText(); //automaticly place the text a few pixels next to the checkbox, in the center
  
  public:
    bool checked; //if true: checked, if false: unchecked
    const Texture* texture[4]; //todo: remove this member, not used anymore
    ColorRGB colorMod[4];
    int toggleOnMouseUp;
    
    //front image
    bool enableImage2; //todo: not used anymore, old, remove this!!!
    const Texture* texture2[4]; //texture when off, mouseover&off, and checked, mouseover&checked
    ColorRGB colorMod2[4];
    //todo: totally irellevant, old, remove these! No more textures are supposed to be in the Checkbox
    void addFrontImage(const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, 
                       const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Grey, const ColorRGB& imageColor3 = RGB_White, const ColorRGB& imageColor4 = RGB_Grey);
    void addFrontImage(const Texture* texture);
    
    //text
    bool enableText; //the text is a title drawn next to the checkbox, with automaticly calculated position
    std::string text;
    Markup markup;
    
    Checkbox();
    void make(int x, int y, bool checked = 0, const GuiSet* set = &builtInGuiSet, int toggleOnMouseUp = 0);
    void makeSmall(int x, int y, bool checked = 0, const GuiSet* set = &builtInGuiSet, int toggleOnMouseUp = 0);
    void addText(const std::string& text, const Markup& markup = TS_W);
    virtual void drawWidget(IGUIDrawer& drawer) const; //also handles it by calling handle(): toggles when mouse down or not
    virtual void handleWidget(const IGUIInput& input);

    void setText(const std::string& newText);
    const std::string& getText() const { return text; }
    void toggle();
    void check() { checked = true; }
    void uncheck() { checked = false; }
    bool isChecked() { return checked; }
    void setChecked(bool check) { checked = check; }
    
    //give it alternative textures than those in the GuiSet
    void setTexturesAndColors(const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, 
                              const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4);
    //without mouseOver effect
    void setTexturesAndColors(const Texture* texture1, const Texture* texture2, 
                              const ColorRGB& color1, const ColorRGB& color2);
};

//The bulletlist, a list of checkboxes where only one can be selected
class BulletList : public ElementComposite
{
  public:
    std::vector <Checkbox> bullet;
    Checkbox prototype;
    
    BulletList();
    virtual void drawWidget(IGUIDrawer& drawer) const;
    virtual void handleWidget(const IGUIInput& input);
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
    virtual void drawWidget(IGUIDrawer& drawer) const;
    Text();
    void make(int x = 0, int y = 0, const std::string& text = "", const Markup& markup = TS_W);
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
    virtual void drawWidget(IGUIDrawer& drawer) const;
    Image();
    void make(int x, int y, ITexture* image=&builtInTexture[37], const ColorRGB& colorMod = RGB_White);
    void make(int x, int y, int sizex, int sizey, ITexture* image=&builtInTexture[37], const ColorRGB& colorMod = RGB_White);
};

class Tabs : public ElementComposite
{
  private:
    struct Tab : public Element
    {
      std::string name; //name of this tab
      Container container; //contains the GUI elements inside this tab
     
      Tab(); 
      virtual void drawWidget(IGUIDrawer& drawer) const;
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
    
    virtual void drawWidget(IGUIDrawer& drawer) const;
    virtual void handleWidget(const IGUIInput& input);
};

} //namespace gui
} //namespace lpi

#endif
