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
#include "lpi_texture.h" //because drawing images requires a texture class

/*
The IGUIDrawer works as follows:
It inherits from 3 different drawer interfaces:
-IDrawer2D: to draw geometric primitives
-ITextDrawer: to draw text
-IGUIPartDrawer: to draw GUI specific things, e.g. a window panel
Furthermore it also has a function to get a IGUIInput.
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
          |                 |      IGUIPartGeom _________   |  |
          |                 |             ^     |        |  |  |
          v                 v             |     v        |  |  |
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
-color: only for some things, e.g. the white button, not for most other things
*/
enum GUIPart
{
  //button panel
  GP_BUTTON_PANEL,
  //scrollbar
  GP_SCROLLBAR_N,
  GP_SCROLLBAR_E,
  GP_SCROLLBAR_S,
  GP_SCROLLBAR_W,
  GP_SCROLLBAR_SCROLLER, //scroller button with fixed size
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
  //custom colorable
  GP_WHITE_PANEL, //panel which can be given any color
  GP_WHITE_BUTTON, //button with same size as scrollbar buttons
  GP_WHITE_BUTTON_ROUND, //button with same size as scrollbar buttons
  //other panels
  GP_PANEL_BORDER, //only border, the center is invisible
  //rulers / lines (to divide areas)
  GP_RULER_H,
  GP_RULER_V,
  //built-in extra's
  GP_SMILEY,
  GP_CROSSHAIR,
  
  ///Colored Parts
  GPC_WINDOW_PANEL, //colored windows
  GPC_WHITE_PANEL, //panel which can be given any color
  GPC_WHITE_BUTTON, //button with same size as scrollbar buttons
  GPC_WHITE_BUTTON_ROUND, //button with same size as scrollbar buttons
  
  
  
  ///Text Parts
  
  //text button (NOT the text on a button with a panel, but a plain text-only button)
  GPT_TEXT_BUTTON_TEXT,
  GPT_PANEL_BUTTON_TEXT,
  GPT_WINDOW_TITLE,
  
  GP_END_DONT_USE //don't use this, it's placed here to have an element without comma at the end of the enumarator list.
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

extern const GUIPartMod GPM_Default;

class IGUIPartGeom //returns geometrical info about how the drawer will draw these GUI parts, sometimes needed to build up elements.
{
  public:
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
};


class IGUIPartDrawer : public IGUIPartGeom
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
};

class IGUIInput;

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
    virtual IGUIInput& getInput() = 0;
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
  
    virtual size_t getWidth();
    virtual size_t getHeight();
    
    virtual void drawPoint(int x, int y, const ColorRGB& color);
    virtual void drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color);
    virtual void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color);
    
    virtual void drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled);
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled);
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled);
    virtual void drawEllipseCentered(int x, int y, int radiusx, int radiusy, const ColorRGB& color, bool filled);
    
    virtual void drawGradientTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2);
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    
    virtual bool supportsTexture(ITexture* texture); //if true, you can use it. If false, first use createTexture(texture) and then it'll be drawable by this drawer!
    virtual ITexture* createTexture(); //creates a new texture of the type this drawer implementation supports best
    virtual ITexture* createTexture(ITexture* texture); //creates a new texture from the given one, the type this drawer implementation supports best. May ruin/destroy/clear the input texture!!
    
    virtual void drawTexture(const ITexture* texture, int x, int y, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureSized(const ITexture* texture, int x, int y, size_t sizex, size_t sizey, const ColorRGB& colorMod = RGB_White);
    virtual void drawTextureRepeated(const ITexture* texture, int x0, int y0, int x1, int y1, const ColorRGB& colorMod = RGB_White);
    
    virtual void pushScissor(int x0, int y0, int x1, int y1);
    virtual void pushSmallestScissor(int x0, int y0, int x1, int y1);
    virtual void popScissor();
    
    virtual void calcTextRectSize(int& w, int& h, const std::string& text, const Font& font = FONT_Default);
    virtual size_t calcTextPosToChar(int x, int y, const std::string& text, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP));
    virtual void calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP));
    virtual void drawText(const std::string& text, int x, int y, const Font& font = FONT_Default, const TextAlign& align = TextAlign(HA_LEFT, VA_TOP));

    //not all GUI parts use all input parameters! only x0 and y0 are always used.
    virtual void drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual void drawGUIPartColor(GUIPart part, const ColorRGB& color, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual void drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual size_t getGUIPartSizeX(GUIPart part) const;
    virtual size_t getGUIPartSizeY(GUIPart part) const;
};

} //namespace gui
} //namespace lpi
