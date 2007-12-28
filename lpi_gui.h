/*
Copyright (c) 2005-2007 Lode Vandevenne
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

#include "lpi_color.h"
#include "lpi_event.h"
#include "lpi_texture.h"
#include "lpi_text.h"


namespace lpi
{
namespace gui
{

void initBuiltInGui(const std::string& xmlgui);
void initBuiltInGuiTexturesSmall64(const std::string& png_base64);
void initBuiltInGuiIconsSmall64(const std::string& png_base64);
void initBuiltInGuiTexturesSmall(const std::vector<unsigned char>& png);
void initBuiltInGuiIconsSmall(const std::vector<unsigned char>& png);
void initBuiltInGuiTexturesSmall(const std::string& png_file_name);
void initBuiltInGuiIconsSmall(const std::string& png_file_name);

////////////////////////////////////////////////////////////////////////////////
//ENUMS/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//direction for gui elements like scrollbars and stuff
enum Direction
{
  H, //horizontal
  V  //vertical
};

struct TextureSet //contains the actual texture data, not just pointers
{
  private:
  Texture texture[128];
  
  public:
  Texture& operator[](int index);
};


extern Texture builtInTexture[128];


/*
MultiLineText
text divided over multiple lines, and functions 
to let it create the lines in different ways, such as (including, but not limited to)

-use ascii 10 chars of the string for new lines
-break off between letters in limited rectangle
-break off between words in limited rectangle

the multiple line text is represented by an array of integers (line) containing the positions of the first letter of each new line

all functions that need to draw text this way HAVE to use this class from now on
*/
class MultiLineText
{
  private:
    std::vector<unsigned long> line; //if the value is -1, it's no new line, it means the end is reached
    std::vector<Markup> lineMarkup; //only used of useFormatting is true: then it has to remember the start-markup of every line, which will be generated at the start with the function generateLineMarkup
  public:
    std::string text;
    
    Markup markup; //the markup currently used
    
    bool char10; //if true, ascii code 10 will act as newline and ascii code 13 will be ignored
    bool useFormatting; //if true, the # codes of formatted text are used. See the printFormatted function for more about this!
    
    void resetLines();
    
    void splitWords(int width); //markup must be given to be able to calculate width ==> max amount of letters in one line
    void splitLetters(int width);
    void splitChar10(); //only make a new line at ascii code 10 characters
    
    void generateLineMarkup(); //only used it useFormatting is true
    
    void setText(const std::string& text);
    void addText(const std::string& text);
    
    unsigned long getNumLines() const;
    
    std::string getLine(unsigned long i) const; //returns string filled with only that line
    
    
    void draw(int x, int y, unsigned long startLine, unsigned long endLine) const;
    void draw(int x, int y) const;
    
    void cursorAtCharPos(unsigned long pos, unsigned long& cline, unsigned long& column) const;
    int charAtCursorPos(unsigned long cline, unsigned long column) const;
    
    MultiLineText();
};


/*
a BackPanel is a collection of 9 textures that should be tileable (except the corners)
so that you can make a variable size rectangle with it
*/
class BackPanel
{
  public:
  ColorRGB colorMod; //used for modifying the textures
  ColorRGB fillColor;
  
  /*
  enable side and/or center texture
  if center is disabled, fillColor is used instead for the center
  if sides are disabled, the center texture is used for the whole thing
  */
  bool enableSides;
  bool enableCenter;
  
  //pointers to the 9 different textures making up a panel that can have any size
  const Texture* t00; //top left corner
  const Texture* t01; //top side
  const Texture* t02; //top right corner
  const Texture* t10; //left side
  const Texture* t11; //center
  const Texture* t12; //right side
  const Texture* t20; //bottom left corner
  const Texture* t21; //bottom side
  const Texture* t22; //bottom right corner

  //draw the panel at x, y position, with given width and height
  void draw(int x, int y, int width, int height) const;
  
  //constructors
  BackPanel();
  BackPanel(int style); //used to quickly generate working BackPanel prototypes in parameters
  BackPanel(const ColorRGB& color); //used for color prototypes
  
  //make panel with flat color (no textures but fill color)
  void makeUntextured(const ColorRGB& fillColor);
  //give 9 separate textures
  void makeTextured9(const Texture* t00=&builtInTexture[0], const Texture* t01=&builtInTexture[1], const Texture* t02=&builtInTexture[2], 
            const Texture* t10=&builtInTexture[3], const Texture* t11=&builtInTexture[4], const Texture* t12=&builtInTexture[5], 
            const Texture* t20=&builtInTexture[6], const Texture* t21=&builtInTexture[7], const Texture* t22=&builtInTexture[8],
            const ColorRGB& colorMod=RGB_White);
  //give 1 texture, the other 8 are assumed to have successive memory locations
  void makeTextured(const Texture* t00=&builtInTexture[0],
       const ColorRGB& colorMod=RGB_White);
};

#define DEFAULTPANEL BackPanel(1)
#define COLORPANEL(color) BackPanel(color)

/*
a BackRule is a collection of 3 textures (the center one tileable) that
can form a horizontal or vertical line
*/
class BackRule
{
  public:
  ColorRGB colorMod; //used for modifying the tetures
  
  /*
  if sides are disabled, the center texture is used for the whole thing
  */
  bool enableSides;
  
  Direction direction;
   
  //the 3 different textures making up the line
  const Texture* t0; //left or top corner
  const Texture* t1; //tilable center
  const Texture* t2; //right or bottom corner

  //draw the line at x, y position, with given length
  void draw(int x, int y, int length) const;
  
  //constructors
  BackRule();
  BackRule(int style); //for prototypes
  
  //give 3 separate textures
  void makeHorizontal(const Texture* t0=&builtInTexture[41], Texture* t1=&builtInTexture[42], Texture* t2=&builtInTexture[43], const ColorRGB& colorMod=RGB_White);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeHorizontal1(const Texture* t0=&builtInTexture[41], const ColorRGB& colorMod=RGB_White);
       
  //give 3 separate textures
  void makeVertical(const Texture* t0=&builtInTexture[44], Texture* t1=&builtInTexture[45], Texture* t2=&builtInTexture[46], const ColorRGB& colorMod=RGB_White);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeVertical1(const Texture* t0=&builtInTexture[44], const ColorRGB& colorMod=RGB_White);
};

#define DEFAULTHRULE BackRule(1)
#define DEFAULTVRULE BackRule(2)

struct GuiSet
{
  const Texture* windowTextures[9];
  const Texture* buttonTextures[9];
  const Texture* buttonOverTextures[9];
  const Texture* buttonDownTextures[9];
  const Texture* arrowN; //for scrollbars, arrow of droplist, ...
  const Texture* arrowE;
  const Texture* arrowS;
  const Texture* arrowW;
  const Texture* scroller; //for scrollbars
  const Texture* emptyButton;
  const Texture* roundButton;
  const Texture* slider; //the button of a slider, the simplified scrollbar (the slider is what the scroller is to scrollbars)
  const Texture* scrollbarBackground;
  const Texture* checkBox[2];
  const Texture* bullet[2];
  const Texture* hline[3];
  const Texture* vline[3];
  const Texture* smiley;
  const Texture* windowTop[3];
  const Texture* closeButton;
  const Texture* resizer; //the resizer at bottom right of a window
  const Texture* whiteButton; //can easily be given any color with color mods
  const Texture* whiteRoundButton;
  const Texture* scrollBarPairCorner; //the cornerpiece of a scrollbarpair
  const Texture* crossHair; //for example to indicate a 2D location on a map, color picker, ...
  
  const BackPanel* windowPanel;
  const BackPanel* buttonPanel;
  const BackPanel* buttonOverPanel;
  const BackPanel* buttonDownPanel;
  
  const BackRule* sliderHRule;
  const BackRule* sliderVRule;
  
  ColorRGB mainColor; //if the mouse is not over or down a button
  ColorRGB mouseOverColor; //this isn't for panel buttons, but for image buttons like the arrows of a scrollbar, ...
  ColorRGB mouseDownColor; //this isn't for panel buttons, but for image buttons like the arrows of a scrollbar, ...
  
  Markup panelButtonMarkup[3];
  Markup textButtonMarkup[3];
};


extern BackPanel builtInPanel[4];
extern BackRule builtInRule[2];
extern GuiSet builtInGuiSet;

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
    int shape; //mouse sensitive shape of this element, shapes: 0: rectangle 1: triangle pointing up 2: triangle pointing right 3: triangle pointing down 4: triangle pointing left (5: triangle pointing topleft) (6: triangle pointing topright) (7: triangle pointing bottomright) (8: triangle pointing bottomleft) (9: circle) (10: 45�)

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
    
    //some mouse variables
    MouseState auto_activate_mouse_state;
    MouseState _mouseState[NUM_MOUSE_BUTTONS];
    MouseState mouse_state_for_containers; //for bookkeeping of containers that contain this element
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
    virtual void resizeWidget(); //always called after resize, will resize the other elements to the correct size. Override this if you have subelements, unless you use getAutoSubElement.
    virtual bool isContainer() const; //returns 0 if the type of element isn't a container, 1 if it is (Window, Container, ...); this value is used by for example Container: it brings containers to the top of the screen if you click on them. Actually so far it's only been used for that mouse test. It's something for containers, by containers :p
    void putInScreen(); //puts element in screen if it's outside

    ////initial position of this element relative to it's container or master (if it has one), these variables are only used by its master if this master needs them and are never updated or changed by this element itself
    int ix0;
    int iy0;
    int ix1;
    int iy1;
    int iMasterSizex;
    int iMasterSizey;
    //the parameters below are the coordinates of the master
    void saveInitialPosition(int x0, int y0, int x1, int y1)
    {
      ix0 = this->x0 - x0;
      iy0 = this->y0 - y0;
      ix1 = this->x1 - x0;
      iy1 = this->y1 - y0;
      this->iMasterSizex = x1 - x0;
      this->iMasterSizey = y1 - y0;
    }

    ////"sticky" variables, this tells which side of this gui element will stick to which side of a resizing container this element is part of and the container has to use and handle these variables, not this gui element itself
    //useful for elements of containers, and also for buttons of scrollbars and stuff
    double leftSticky; //0: follows left side of container, 1: follows right side of container, between 0-1: weighed average
    double topSticky; //0: follows top side of container, 1: follows bottom side of container, between 0-1: weighed average
    double rightSticky; //0: follows left side of container, 1: follows right side of container, between 0-1: weighed average
    double bottomSticky; //0: follows top side of container, 1: follows bottom side of container, between 0-1: weighed average
    //if leftsticky and rightsticky have the same value, this element won't resize in the x direction when it's master resizes, it'll only translate. Same for top and bottom.
    //if a sticky value is -1.0, it'll always keep the same relative size
    //if a sticky value is -2.0, it means: "do NOT move or resize this element if the master is resized". HOWEVER moving when the master is moved with move or moveTo should still work!!!
    //if a sticky value is -3.0, it means the same as -2.0, except it WILL move when it goes outside it's master, in other words it'll always stay inside
    void resizeSticky(int x0, int y0, int x1, int y1); //see implementation and locations where it's used what this function does; the parameters are the new sides of the master
    void setSticky(double left, double top, double right, double bottom) {leftSticky = left; topSticky = top; rightSticky = right; bottomSticky = bottom;}
    
    ////optional label
    std::string label;
    int labelX; //label position is relative to the position of the element
    int labelY;
    Markup labelMarkup;
    void drawLabel() const;
    void makeLabel(const std::string& label, int labelX, int labelY, const Markup& labelMarkup);
    
    ////optional tooltip. Drawing it must be controlled by a higher layer, e.g. see the Container's implementation.
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
    
    protected:
    
    /*
    override this to return sub elements (0 if i >= amount of sub elements) to automatically do
    resizeWidget, moveWidget and setElementOver for these elements. That can save you from having
    to override those 3 other functions instead.
    Draw and handle are not called by this, because those usually require more manual tweaking.
    Make sure to return 0 if i >= amount of sub elements, or it causes an infinite loop!
    E.g. in a scrollbar, getAutoSubElement could return its buttons and so on.
    */
    virtual Element* getAutoSubElement(int /*i*/) { return 0; }
    
    private:
    
    //for the timeDown function
    mutable float downTime;
    
    //rectangle behind it
    bool hasBackgroundRectangle;
    ColorRGB backgroundRectangleColor;
    
    protected:
    bool elementOver; //true if there is an element over this element, causing the mouse NOT to be over this one

    bool visible; //if false, the draw() function doesn't draw anything
    bool active; //if false, handle() does nothing, and mouse tests return always false
    bool present; //if true, it reacts to the mouse. if false, it ignores the mouse, even if forceActive is true, if a gui element isn't present, it really isn't present

    bool notDrawnByContainer; //default false. If true, this element won't be drawn by the container. You have to draw it yourself. Advantage: you can choose when it's drawn, allowing determining drawing-order of non-gui things intermixed with gui things.
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
              bool enablePanel, const BackPanel* panel1 = &builtInPanel[1], const BackPanel* panel2 = &builtInPanel[2], const BackPanel* panel2 = &builtInPanel[3], int panelOffsetx = 0, int panelOffsety = 0, //panel
              int shape = 0);
    
    //image only constructor (without offset)
    void makeImage(int x, int y,
                   const Texture* texture1, const Texture* texture2, const Texture* texture3, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey, //image
                   int shape = 0);
    
    void makeImage(int x, int y,  const Texture* texture123, const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey,  int shape = 0);
    
    void addFrontImage(const Texture* texture1, const Texture* texture2, const Texture* texture3,
                       const ColorRGB& imageColor1 = RGB_White, const ColorRGB& imageColor2 = RGB_Brightred, const ColorRGB& imageColor3 = RGB_Grey);
    
    void addFrontImage(const Texture* texture);
    
    //text only constructor (without offset)
    void makeText(int x, int y, //basic properties
                  const std::string& text, //text
                  const GuiSet* set = &builtInGuiSet,
                  int shape = 0 /*special options*/);
    
    //panel + text constructor (text always in center of panel, no offsets and thinking needed)
    //this is the constructor with default parameters
    void makeTextPanel(int x, int y, const std::string& text = "", int sizex = 64, int sizey = 24, //basic properties + actual text
                       const GuiSet* set = &builtInGuiSet, //panel
                       int shape = 0);


    virtual void drawWidget() const;
    
    private:
    mutable MouseState mutable_button_drawing_mouse_test;
};

//The Scrollbar
class Scrollbar : public Element
{
  private:
    float oldTime;
    //bool scrollTimeTrigger;
    void init();
  public:
    //get length and begin and end coordinates of the slider part (the part between the up and down buttons) (relative to x, y of the scrollbar)
    int getSliderSize() const;
    int getSliderStart() const;
    int getSliderEnd() const;
    
    virtual void handleWidget();
    virtual void drawWidget() const;

    Direction direction; //0 = vertical, 1 = horizontal
    //int sizeButton; //size of the up and down button
    //int sizeScroller; //size of the center button
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
    
    protected:
    virtual Element* getAutoSubElement(int i);
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
    virtual void resizeWidget();
    virtual void moveWidget(int x, int y);
    virtual void setElementOver(bool state);
    
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
    
    protected:
    virtual Element* getAutoSubElement(int i);
};

class Invisible : public Element
{
  public:
  void make(int x0, int y0, int x1, int y1);

  virtual void drawWidget() const;
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
  
  unsigned long getTileX() const;
  unsigned long getTileY() const;
  
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
  
  virtual void drawWidget() const;
  
  int tileSizeX;
  int tileSizeY;
  
  unsigned long getNumx() const;
  unsigned long getNumy() const;
  
  unsigned long getNumElements() const;
  
  int getTileX() const;
  int getTileY() const;
  int getTile() const; //returns index of the tile
  
  int getScreenX(int tilex) const;
  int getScreenY(int tiley) const;
};

class Container : public Element
{
  private:
    bool keepElementsInside; //will not allow you to drag elements outside (if the element is dragged(), it'll be kept inside the container)
    void initElement(Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky);
  public:
    std::vector<Element*> element;
    
    Container();
    virtual void handleWidget(); //you're supposed to handle() before you draw()
    virtual void drawWidget() const;
    
    //push the element without affecting absolute position
    void pushTop(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void pushBottom(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void insert(size_t pos, Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    
    //push the element so that its top left is relative to the top left of this container, thus moving it if the container isn't at 0,0
    void pushTopRelative(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void pushBottomRelative(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void insertRelative(size_t pos, Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);

    //push the element at the given x, y (relative to this container's top left)
    void pushTopAt(Element* element, int x, int y, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void pushBottomAt(Element* element, int x, int y, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void insertAt(size_t pos, Element* element, int x, int y, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);

    void bringToTop(Element* element); //precondition: element must already be in the list
    
    void centerElement(Element* element);

    void remove(Element* element);
    unsigned long size() const;
    virtual bool isContainer() const;
    void clear(); //clears all the elements
    void putInside(unsigned long i);
    virtual void moveWidget(int x, int y);
    void make(int x, int y, int sizex, int sizey,
              int areax = 0, int areay = 0, int areasizex = -1, int areasizey = -1, //areax and areay are relative to the container!
              double areaLeftSticky = 0, double areaTopSticky = 0, double areaRightSticky = 1, double areaBottomSticky = 1);
    
    void setKeepElementsInside(bool set) { keepElementsInside = set; }
    
    void getRelativeElementPos(Element& element, int& ex, int& ey) const;
    virtual void resizeWidget();
    
    virtual void setElementOver(bool state);
    
    ////the initial position of the elements of this container and container itself at that point
    void saveInitialPositions(); //sets the "ix0, iy0, ix1, iy1, masterSizex and masterSizey" parameters or all elements
    
    ////everything concerning the scrollability
    ScrollbarPair bars;

    int getVisibleSizex() const;
    int getVisibleSizey() const;
    int getVisibleX0() const;
    int getVisibleY0() const;
    int getVisibleX1() const;
    int getVisibleY1() const;
    
    //the scrollable area "behind" or "inside" the container
    Invisible area; //the area that can be scrolled will be represented by this, so it's move function and such can easily be called while you scroll
    
    int oldScrollx; //used to move elements every frame when you scroll the bars
    int oldScrolly;
    
    void moveAreaTo(int x, int y); //moves the area to given position, and all the elements, but not the bars and x0, y0, x1, y1, Used when you scroll.
    
    void setScrollSizeToElements(); //makes the size of the scroll area as big as the elements
    
    void initBars();
    void updateBars();
    void toggleBars(); //turns the bars on or of depending on if they're needed or not
};

class Group : public Container
{
  public:
    virtual bool mouseOver() const; //difference with the mouseOver from other guielements, is that it checks all sub elements, not itself, for mouseovers
};

class Panel : public Element
{
  private:
    BackPanel panel;

  public:
    Panel();
    void make(int x, int y, int sizex, int sizey,
              const GuiSet* set = &builtInGuiSet, int shape=0);
    void makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor);
    //give 1 texture, the other 8 are assumed to have successive memory locations
    void makeTextured(int x, int y, int sizex, int sizey,
                      const Texture* t00, const ColorRGB& colorMod = RGB_White, int shape=0);
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
//now WITH container in it!
class Window : public Element
{
  private:
  public:
    void disableCenterTexture() { panel.enableCenter = false; }
    void setFillColor(const ColorRGB& color) { panel.fillColor = color; }
  
    Window();
    
    ////obligatory part "panel"
    BackPanel panel;
    
    ////obligatory part "container"
    Container container;
    int getContainerLowest() const;
    int getContainerHighest() const;
    int getContainerLeftmost() const;
    int getContainerRightmost() const;
    
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
    void pushTop(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void pushBottom(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void insert(size_t pos, Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    
    //push the element so that its top left is relative to the top left of this container, thus moving it if the container isn't at 0,0
    void pushTopRelative(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void pushBottomRelative(Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void insertRelative(size_t pos, Element* element, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);

    //push the element at the given x, y (relative to this container's top left)
    void pushTopAt(Element* element, int x, int y, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void pushBottomAt(Element* element, int x, int y, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);
    void insertAt(size_t pos, Element* element, int x, int y, double leftSticky=0.0, double topSticky=0.0, double rightSticky=0.0, double bottomSticky=0.0);

    void bringToTop(Element* element); //precondition: element must already be in the list
    
    void remove(Element* element);
    int size();
    void putInside(int i);
    
    //these scrollbars will be part of the container
    void addScrollbars(int areax = 0, int areay = 0, int areasizex = -1, int areasizey = -1, //areax and areay are relative to the container!
                       double areaLeftSticky = 0, double areaTopSticky = 0, double areaRightSticky = 1, double areaBottomSticky = 1);
    
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
              const GuiSet* set = &builtInGuiSet, int shape=0);
    void makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor);
    void makeTextured(int x, int y, int sizex, int sizey,
                      const Texture* t00, const ColorRGB& colorMod = RGB_White, int shape=0);
         
    int getRelContainerStart() const { return container.getY0() - y0; }
    
    int getRelContentStart() const;
    
    void setSize(int x, int y, int sizex, int sizey);
    
    ////overloaded functions
    virtual void drawWidget() const;
    virtual void moveWidget(int x, int y);
    
    virtual void handleWidget();
    virtual bool isContainer() const;
    virtual void setElementOver(bool state); //window has top bar in it that needs to be set inactive too so has it's own function for this defined
    virtual void resizeWidget();
    ////other functions
    void initContainer();
    
    ////useful for the close button
    void close() { closed = 1; totallyDisable(); } //use this if closed == 1
    void unClose() { closed = 0; totallyEnable(); }
    void toggleClose() { if(closed) unClose(); else close(); }
    
    void setColor(const ColorRGB& color) { panel.colorMod = color; }
};



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
    virtual void drawWidget() const;
    virtual void handleWidget();
    int check();
    Button* getButton(int i);
    Button* getButton(const std::string& name);
    int getNumButtons() const { return menuButton.size(); }
    std::string checkText();
    /*
    returns 0 if none, or a button where no identity was set, is pressed, or
    the identity of the button if one with an identity was pressed
    */
    int checkIdentity();
    virtual void moveWidget(int x, int y);
    virtual void setElementOver(bool state);
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
              BackPanel topPanel = COLORPANEL(RGB_Grey), BackPanel listPanel = COLORPANEL(RGB_White),
              Texture* buttonTexture = &builtInTexture[28]);
    void makeScrollbar(const GuiSet* set = &builtInGuiSet);
    virtual void drawWidget() const;
    int check();
    std::string checkText();
    virtual void handleWidget();

    virtual void moveWidget(int x, int y);
    virtual void setElementOver(bool state);
    void addOption(const std::string& text);
};


class InputLine : public Element//input text line
{
  private:
    //unsigned long titleLength;
    unsigned long cursor; //position of the cursor (0 = before first char)
    bool entered; //after you pressed enter!
    
  public:
    unsigned long l; //max length
    Markup markup;
    Markup titleMarkup;
    ColorRGB cursorColor;
    /*
    types:
    0: normal text line
    1: password (displays * instead of text)
    2: integer number
    */
    int type;
    int allowedChars; //0 enables all, enabling bits disables certain symbols, see the unicode input function
    std::string title;
    std::string text;
        
    InputLine();
    void make(int x, int y, unsigned long l,
              const Markup& markup = TS_W,
              int type = 0, int allowedChars = 0, const std::string& title = "", const Markup& titleMarkup = TS_W, const ColorRGB& cursorColor = RGB_White);
    
    virtual void drawWidget() const;
    virtual void handleWidget();

    void setText(const std::string& i_text);
    const std::string& getText() const;
    
    void setTitle(const std::string& i_title);
    const std::string& getTitle() const;
   
    void clear();
    //return values of check():
    /*
    0: the text string is empty, and the rest of the things below aren't true either
    &1: the text string is not empty
    &2: you pressed ENTER (to enter the text string!)
    &4: mouse pointer is inside
    &8: left mouse button is down
    */    
    int check();
    int enter();
    int getInteger() const; //returns any integer number that may be in the string you typed
    int keyCheckIndex; //if you have multiple input lines and don't want interference for the key input, give them a differnt keyCheckIndex
};

class InputBox : public Element
{
  private:
    unsigned long cursor; //position of cursor in the text, column and line position will be calculated by the MultiLineText
    int textWidth; //sizex - width of the scrollbar
    int firstVisibleLine; //depends on the scrollbar position
    
    MultiLineText multiText;
    std::string text;
    
    void init();
    
  public:
    int maxLines;
    
    InputBox();
    void make(int x, int y, int sizex, int sizey, int maxLines, int border = 2, const Markup& markup = TS_B, BackPanel panel = COLORPANEL(RGB_White), const ColorRGB& cursorColor = RGB_Black);
    void makeScrollbar(const GuiSet* set = &builtInGuiSet);
    const std::string& getText() const { return text; }
    
    virtual void moveWidget(int x, int y);
    virtual void setElementOver(bool state);
    
    virtual void drawWidget() const;
    virtual void handleWidget();
    
    int border;
    
    //get the positions (relative to the area's x and y) of the text area
    int getLeftText() const;
    int getRightText() const;
    int getTopText() const;
    int getBottomText() const;
    int getTextAreaHeight() const;
    int getTextAreaWidth() const;
    int getLinesVisible() const; //how much lines of text are visible
    
    Scrollbar bar;
    
    BackPanel panel;
    
    Markup markup;
    ColorRGB cursorColor;
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
    virtual void moveWidget(int x, int y);
    int xDiff; //just added for "book keeping"
    int yDiff; //just added for "book keeping"
    
    virtual void setElementOver(bool state); //all subelements of this must also be made to the same mouse activiness

    std::string getText(unsigned long i) const;
    const std::string& getCurrentText() const;
    void addText(const std::string& text, unsigned long i);
    void set(unsigned long i);
    
  private:
    int lastChecked; //remember which one was checked, so it can know when a new one is checked, which one that is
};

//The String Stack
class StringStack
{
  public:
    int num; //max number of strings on your stringstack
    
    int next; //physical address of next string that'll be written
    int oldest; //physical address of oldest string that was written
    
    std::vector<std::string> text;
    
    StringStack();
    StringStack(int num);
    
    //void makeBuffer(int num);
    //void deleteBuffer();
    void make(int num);
    void push(const std::string& text);
    int increase(int pointer, int amount = 1);
    int decrease(int pointer, int amount = 1);
    int getNumText() const;
    const std::string& getOldest(int offset = 0);
    const std::string& getNewest(int offset = 0);
  private:
    int numFilled; //how much of the memory places are filled currently
};

//The console (only shows output text, use for example InputLine to input text into the console)
class Console : public Element
{
  private:
    int numLines; //how much actual lines does it take on screen
    mutable StringStack stack; //mutable because it changes while drawing this widget
    void calcNumLines(); //calculates number of lines from the current messages; to be recalculated when sizex changes or messages are pushed/popped

  public:
    int scroll; //how much lines away from the first line (= oldest) are you scrolled, note: line != message, message can be multiple lines big!
    int num; //max number of messages
    
    //for now, no special text markup yet. All text has the same color and format.
    Markup markup;
    
    int getNumLines() const;
    int getNumMessages() const;
    int getVisibleLines(const Markup& markup);
    virtual void drawWidget() const;
    
    void push(const std::string& text);
    Console();
    void make(int x, int y, int sizex, int sizey, int num, const Markup& markup = TS_W);
};

//The textarea: can show you a single multilinetext and allows you to scroll (if you use optional or separate scrollbar or something else)
class TextArea : public Element
{
  public:
    int scroll; //how much lines away from the first line
    
    MultiLineText text;
    
    Markup markup;
    
    int getNumLines() const;
    int getVisibleLines(const Markup& markup) const;
    virtual void drawWidget() const;
    virtual void handleWidget();
    virtual void resizeWidget();
    virtual void moveWidget(int x, int y);
    virtual void setElementOver(bool state);

    TextArea();
    void make(int x, int y, int sizex, int sizey, const std::string& text, const Markup& markup = TS_W);
    void setText(const std::string& text, const Markup& markup = TS_W);
    
    ///optional scrollbar
    Scrollbar scrollbar;
    bool scrollEnabled;
    void addScrollbar();
    void setScrollbarSize();
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

class Rectangle : public Element
{
  public:
    ColorRGB color;
    virtual void drawWidget() const;
    Rectangle();
    void make(int x, int y, int sizex=64, int sizey=64, const ColorRGB& color = RGB_Grey);
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
    virtual void drawWidget() const;
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

class Line : public Element
{
  public:
    ColorRGB color;
    virtual void drawWidget() const;
    Line();
    int lx0;
    int ly0;
    int lx1;
    int ly1;
    void make(int x, int y, int sizex=64, int sizey=64, const ColorRGB& color = RGB_Grey);
    void setEndpoints(int x0, int y0, int x1, int y1);
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

class FormattedText : public Element
{
  public:
    Markup markup;
    virtual void drawWidget() const;
    FormattedText();
    void make(int x = 0, int y = 0, const std::string& text = "", const Markup& markup = TS_W);
    void setText(const std::string& text);
    const std::string& getText() const { return text; }
  private:
    std::string text;
};

template<typename T>
class Variable : public Element //can be anything the typename is: integer, float, ..., as long as it can be given to a stringstream
{
  public:
    T v;
    std::string label;
    Markup markup;
    
    virtual void drawWidget() const
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
    
    virtual void drawWidget() const;
    virtual void handleWidget();
    
    void clear(); //clear the whole texture to backColor, deleting it's previous contents
    
    Canvas();
    void make(int x, int y, int sizex, int sizey, const ColorRGB& backColor = RGB_White, int border = 0, const ColorRGB& leftColor = RGB_Black, const ColorRGB& rightColor = RGB_Red, double size = 1.0, double hardness = 1.0, double opacity = 1.0);
    void setBrush(const ColorRGB& leftColor = RGB_Black, const ColorRGB& rightColor = RGB_Red, double size = 1.0, double hardness = 1.0, double opacity = 1.0);
};

/*
a way of unit testing all the GUI classes and mouse behaviour
*/
void unitTest();

} //namespace gui
} //namespace lpi

#endif
