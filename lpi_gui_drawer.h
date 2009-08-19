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

#include "lpi_color.h"
#include "lpi_text.h" //for markup
#include "lpi_texture.h" //because drawing images requires a texture class

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
  GP_BUTTON_OVER_PANEL,
  GP_BUTTON_DOWN_PANEL,
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
  GP_SLIDER_BUTTON, //omnidirectional slider button with fixed size
  GP_SLIDER_HBACK,
  GP_SLIDER_VBACK,
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
  GP_CROSSHAIR
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

class IGUIDrawer
{
  public:
    virtual ~IGUIDrawer(){};
    virtual void drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color) = 0;
    virtual void drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled) = 0;
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled) = 0;
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled) = 0;
    
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3) = 0;

    virtual void drawText(const std::string& text, int x = 0, int y = 0, const Markup& markup = TS_W) = 0;
    virtual void drawTextCentered(const std::string& text, int x = 0, int y = 0, const Markup& markup = TS_W) = 0;
    
    virtual void drawTexture(int x, int y, const Texture* texture, const ColorRGB& colorMod = RGB_White) = 0;
    
    //things that need to be done before and after the drawing, e.g. setting the scissor
    virtual void setSmallestScissor(int x0, int y0, int x1, int y1) = 0;
    virtual void resetScissor() = 0;
    
    //not all GUI parts use all input parameters! only x0 and y0 are always used.
    virtual void drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, bool inactive = false, const ColorRGB& color = RGB_White) = 0;
    
    //input
    virtual IGUIInput& getInput() = 0;
};

} //namespace gui
} //namespace lpi
