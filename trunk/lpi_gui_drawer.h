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


#pragma once

#include "lpi_draw2d.h"
#include "lpi_color.h"
#include "lpi_text_drawer.h"
#include "lpi_input.h"

/*
The IGUIDrawer works as follows:
It inherits from 3 different drawer interfaces:
-IDrawer2D: to draw geometric primitives
-ITextDrawer: to draw text
-IGUIPartDrawer: to draw GUI specific things, e.g. a window panel
Furthermore it also has a function to get a IInput.
So the IGUIDrawer integrates 4 things (3 drawers and an input checker) together.
When implementing IGUIDrawer, please implement the 3 drawers by composition. The
convenience class AGUIDrawer already does that while still allowing to choose
custom drawers.

Class diagram of a typical scenario, where
-Full arrows (_____) represent inheritance
-Dotted arrows (.....) represent dependency or usage or composition of some implementation of/with the used interface
-Everything that starts with "I" is a pure interface
-Everything that starts with "Some" is some implementation

           ____________________________________________________ 
          |                  _______________________________   |
          |                 |                    ________   |  |
          |                 |                   |        |  |  |
          v                 v                   v        |  |  |
     IDrawer2D       ITextDrawer     IGUIPartDrawer    IGUIDrawer
      ^ ^ ^ ^          ^ ^ ^              ^ ^                 ^
      | : : :          | : :              | :................ |
      | : : :          | : :..............|................ : |
      | : : :          | :................|.............. : : |
      | : : :..........|..............    |             : : : |
      | : :............|............ :    |             : : : |
      | :..........    |           : :    |             : : : |
      |           :    |           : :    |             : : : |
   SomeDrawer2D  SomeTextDrawer   SomeGUIPartDrawer    SomeGUIDrawer


So in the above scenario:
ITextDrawer-implementations use a IDrawer2D
IGUIPartDrawer-implementations use both a IDrawer2D and a ITextDrawer
IGUIDrawer-implementations use all 3 IDrawer2D, ITextDrawer and IGUIPartDrawer
IGUIDrawer inherits from all 3 drawer interfaces (IDrawer2D, ITextDrawer and IGUIPartDrawer), but its implementations implement them through composition

NOTE: in reality, IGUIDrawer and most GUI drawer implementations inherit from
ADrawer2D instead of IDrawer2D, for convenience, but that is not shown in the above
diagram for clarity reasons.
*/  

namespace lpi
{
namespace gui
{

/*
GUIPart: what part of the GUI is being drawn (e.g. a button of a scrollbar, ...)

Parameters that affect how the part is drawn:
-inactive? ==> greyed out
-size (in x and y direction) --> some things have size determined by the Drawer, other things are determined by the element itself
-color: only for some things
*/
enum GUIPart
{
  //button panel
  GP_BUTTON_PANEL,
  GP_BUTTON_PANEL_UP,
  GP_BUTTON_PANEL_DOWN, //not used for graphic of button down if mouse is down, but if you want one down without having mouse down over it
  GP_INVISIBLE_BUTTON_PANEL,
  GP_INVISIBLE_BUTTON_PANEL_UP,
  GP_INVISIBLE_BUTTON_PANEL_DOWN, //not used for graphic of button down if mouse is down, but if you want one down without having mouse down over it
  //scrollbar
  GP_SCROLLBAR_N,
  GP_SCROLLBAR_E,
  GP_SCROLLBAR_S,
  GP_SCROLLBAR_W,
  GP_SCROLLBAR_SCROLLER, //scroller button with fixed size
  GP_SCROLLBAR_SCROLLER_OUT_OF_RANGE, //indication that value of the scrollbar is out of range for the "user" zone due to external setValue (is allowed on purpose!!! E.G. for the ScrollElement to allow elements to be out of range
  GP_SCROLLBAR_HBACK,
  GP_SCROLLBAR_VBACK,
  GP_SCROLLBARPAIR_CORNER,
  //slider
  GP_SLIDER_HBUTTON,
  GP_SLIDER_VBUTTON,
  GP_SLIDER_HBACK,
  GP_SLIDER_VBACK,
  GP_SMALL_SLIDER_HBUTTON,
  GP_SMALL_SLIDER_VBUTTON,
  GP_SMALL_SLIDER_HBACK,
  GP_SMALL_SLIDER_VBACK,
  //checkboxes & bullets
  GP_CHECKBOX_OFF,
  GP_CHECKBOX_ON,
  GP_CHECKBOX_HALF,
  GP_BULLET_OFF,
  GP_BULLET_ON,
  GP_BULLET_HALF,
  //window
  GP_WINDOW_PANEL,
  GP_WINDOW_TOP,
  GP_WINDOW_CLOSE,
  GP_WINDOW_RESIZER,
  //tabs
  GP_TAB_UNSELECTED,
  GP_TAB_SELECTED,
  //menu
  GP_MENU_PANEL,
  GP_MENU_SEPARATOR,
  //image buttons with same size as scrollbar buttons
  GP_EMPTY_BUTTON,
  GP_EMPTY_BUTTON_ROUND,
  //other panels
  GP_PANEL_BORDER, //only border, the center is invisible
  //rulers / lines (to divide areas)
  GP_RULER_H,
  GP_RULER_V,
  //fgbgcolor
  GP_FG_BG_COLOR_ARROWS, //those arrows to swap colors at a FG-BG color selector
  //tooltip background
  GP_TOOLTIP_BACKGROUND, //background of tooltip, as big as you want. No need to use this if you use GPT_TOOLTIP
  //menu's
  GP_HMENU_PANEL, //the panel behind the complete menu
  GP_HMENU_SEPARATOR, //separator of a horizontal menu (the separator itself is a vertical line)
  GP_VMENU_PANEL, //the panel behind the complete menu
  GP_VMENU_SEPARATOR, //separator of a vertical menu (the separator itself is a horizontal line)
  //spinner
  GP_SPINNER_UP, //a spinner is a numerical input field in which value can be entered with keyboard and with a convenient up and down button to change the value. GP_SPINNER_UP is the up button.
  GP_SPINNER_DOWN, //idem for down button
  //dropdown list
  GP_DROPDOWN_BUTTON,
  //toolbar
  GP_TOOLBAR_SEPARATOR, //vertical separator for in toolbar
  GP_TOOLBAR_BUTTON,
  //status bar
  GP_STATUSBAR,
  GP_STATUSBAR_SEPARATOR, //vertical separator for in status bar

  ///Colored Parts //todo: just give everything a color parameter instead if needed
  GPC_WINDOW_PANEL, //colored windows

  ///Text Parts
  
  
  GPT_DEFAULT_TEXT00,
  GPT_DEFAULT_TEXT01,
  GPT_DEFAULT_TEXT02,
  GPT_DEFAULT_TEXT10,
  GPT_DEFAULT_TEXT11,
  GPT_DEFAULT_TEXT12,
  GPT_DEFAULT_TEXT20,
  GPT_DEFAULT_TEXT21,
  GPT_DEFAULT_TEXT22,
  GPT_TEXT_BUTTON_TEXT, //text button (NOT the text on a button with a panel, but a plain text-only button)
  GPT_PANEL_BUTTON_TEXT,
  GPT_WINDOW_TITLE,
  GPT_TOOLTIP, //both the background and the text of the tooltip, fit to the text size
  GPT_HMENU_TEXT, //the text as well as panel behind the text if any
  GPT_HMENU_SUBMENUTEXT,
  GPT_VMENU_TEXT,
  GPT_VMENU_SUBMENUTEXT,
  GPT_MESSAGE_TEXT,
  GPT_STATUSBAR_TEXT,
  GPT_TEXTINPUTLINE,
  GPT_TEXTINPUTLINE_TITLE,
  
  GP_END_DONT_USE //don't use this, it's placed here to have an element without comma at the end of the enumarator list.
};

//The GUI has some standard icons that can be used in toolbars and other things (like file dialog) anywhere. Their names are in this enum.
enum GUIIcon
{
  GI_FILE,
  GI_FOLDER,
  GI_NEW,
  GI_OPEN,
  GI_SAVE,
  GI_SAVE_AS,
  GI_SAVE_ALL,
  GI_CLOSE,
  GI_CUT,
  GI_COPY,
  GI_PASTE,
  GI_UNDO,
  GI_REDO,

  GI_END_DONT_USE//don't use this, it's placed here to have an element without comma at the end of the enumarator list.
};

struct GUIPartMod
{
  GUIPartMod();
  GUIPartMod(bool inactive);
  GUIPartMod(bool mouseover, bool mousedown);
  GUIPartMod(bool mouseover, bool mousedown, bool inactive);
  bool inactive;
  bool mouseover;
  bool mousedown;
};

static const GUIPartMod GPM_Default;


class IGUIPartDrawer
{
  public:
    /*
    Not all GUI parts are drawn by every function, some require a color parameter, others a text parameter,
    so the color and text function need to be used for those.
    NOTE: sometimes x0, y0 are not used by x1, y1 instead (or some other combination), for example this is
    so for the window resizer and the scrollbarpair-corner, which are at the bottom right instead of the top left
    and have a fixed size there. but that may depend on the drawer, and best is to always give
    all 4 x0, y0, x1, y1 to represent the exact rectangle in which you want the part to be drawn.
    */
    virtual void drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default) = 0;
    virtual void drawGUIPartColor(GUIPart part, const ColorRGB& color, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default) = 0;
    virtual void drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default) = 0;
    
    /*
    Getting the size of GUI parts only returns a usable result for non-variable parts.
    For example, the size of a window panel is variable, so don't use that here.
    But, the size of the button of a slider is fixed, and which it is for this drawer,
    can be returned here.
    Sometimes only X or only Y makes sense, e.g. the scrollbar backgrounf of a horizontal
    scroller has a fixed Y size but a variable X size.
    */
    virtual size_t getGUIPartSizeX(GUIPart part) const = 0;
    virtual size_t getGUIPartSizeY(GUIPart part) const = 0;
    virtual void getGUIPartTextSize(int& w, int& h, GUIPart part, const std::string& text) const = 0;
    
    virtual void createIcon(ITexture& texture, GUIIcon icon, int size = 16) const = 0; //size is just an indication of roughly the size you want it to be, it can be ignored by the drawer if it only supports a certain size
};

/*
TODO:
lpi_gui shouldn't keep any textures internally (except things like Image).
But it should be possible to draw elements with custom styles. These styles should be defined in the Drawer instead.
But, each GUI element should then be able to keep a sort of "token", "handle" or "pointer" to such a style.
This style and the handle are created and interpreted by the Drawer.
The handle should become a parameter of the drawGUIPart function.
If a handle/token points to something a Drawer doesn't understand (it's created by a different type of drawer), it should ignore it and use default style.
There are also things the GUI element should be able to get in an abstract way from the style, such as, which standard width or height of the element is associated with this style.
*/
class IGUIDrawer : public ADrawer2D, public ITextDrawer, public IGUIPartDrawer //if you use a ADrawer2D implementation for you IGUIDrawer, best is to use composition (not make your subclass inherit from a ADrawer2D subclass)
{
  public:
    virtual ~IGUIDrawer(){};
    
    //input
    virtual IInput& getInput() = 0;
};

class AGUIDrawer : public IGUIDrawer //abstract GUI drawer which already wraps an ADrawer2D for you (but you need to implement the protected function getDrawer)
{
  protected:
    virtual IDrawer2D& getDrawer() = 0;
    virtual const IDrawer2D& getDrawer() const = 0;
    virtual ITextDrawer& getTextDrawer() = 0;
    virtual const ITextDrawer& getTextDrawer() const = 0;
    virtual IGUIPartDrawer& getGUIPartDrawer() = 0;
    virtual const IGUIPartDrawer& getGUIPartDrawer() const = 0;
    
  public:
  
    virtual void frameStart();
    virtual void frameEnd();
  
    virtual size_t getWidth();
    virtual size_t getHeight();
    
    virtual void drawPoint(int x, int y, const ColorRGB& color);
    virtual void drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color);
    virtual void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color);
    
    virtual void drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled);
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled);
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled);
    virtual void drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled);
    
    virtual void drawGradientRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    virtual void drawGradientTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2);
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    
    virtual bool supportsTexture(ITexture* texture); //if true, you can use it. If false, first use createTexture(texture) and then it'll be drawable by this drawer!
    virtual ITexture* createTexture() const; //creates a new texture of the type this drawer implementation supports best
    virtual ITexture* createTexture(ITexture* texture) const; //creates a new texture from the given one, the type this drawer implementation supports best. May ruin/destroy/clear the input texture!!
    
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSizedRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureGradient(const ITexture* texture, int x, int y
                                   , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11);
    virtual void drawTextureRepeatedGradient(const ITexture* texture, int x0, int y0, int x1, int y1
                                           , const ColorRGB& color00, const ColorRGB& color01, const ColorRGB& color10, const ColorRGB& color11);

    virtual void pushScissor(int x0, int y0, int x1, int y1);
    virtual void pushSmallestScissor(int x0, int y0, int x1, int y1);
    virtual void popScissor();
    
    virtual void calcTextRectSize(int& w, int& h, const std::string& text, const Font& font = FONT_Default) const;
    virtual size_t calcTextPosToChar(int x, int y, const std::string& text, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const;
    virtual void calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP)) const;
    virtual void drawText(const std::string& text, int x, int y, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP));

    //not all GUI parts use all input parameters! only x0 and y0 are always used.
    virtual void drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual void drawGUIPartColor(GUIPart part, const ColorRGB& color, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual void drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual size_t getGUIPartSizeX(GUIPart part) const;
    virtual size_t getGUIPartSizeY(GUIPart part) const;
    virtual void getGUIPartTextSize(int& w, int& h, GUIPart part, const std::string& text) const;
    virtual void createIcon(ITexture& texture, GUIIcon icon, int size = 16) const;
    
};

} //namespace gui
} //namespace lpi
