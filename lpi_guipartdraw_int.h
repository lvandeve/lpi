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


#pragma once

#include "lpi_gui_drawer.h"
#include "lpi_draw2dgl.h"
#include "lpi_gui_base.h"
#include "lpi_color.h"
#include "lpi_text_drawer.h"
#include "lpi_gui_input_sdl.h"

namespace lpi
{
namespace gui
{

extern const std::string builtInGuiData;

/*
a BackPanel is a collection of 9 textures that should be tileable (except the corners)
so that you can make a variable size rectangle with it
*/
class BackPanel
{
  public:
  /*
  enable side and/or center texture
  if center is disabled, only sides are drawn (so it can be used to draw a border around something) 
  if sides are disabled, the center texture is used for the whole thing
  */
  bool enableSides;
  bool enableCenter;
  
  //pointers to the 9 different textures making up a panel that can have any size
  const ITexture* t00; //top left corner
  const ITexture* t01; //top side
  const ITexture* t02; //top right corner
  const ITexture* t10; //left side
  const ITexture* t11; //center
  const ITexture* t12; //right side
  const ITexture* t20; //bottom left corner
  const ITexture* t21; //bottom side
  const ITexture* t22; //bottom right corner

  //draw the panel at x, y position, with given width and height
  void draw(IDrawer2D& drawer, int x, int y, int width, int height, const ColorRGB& colorMod=RGB_White) const;
  
  //constructors
  BackPanel();
  
  //make panel with flat color (no textures but fill color)
  void makeUntextured();
  //give 9 separate textures
  void makeTextured9(const ITexture* t00, const ITexture* t01, const ITexture* t02, 
            const ITexture* t10, const ITexture* t11, const ITexture* t12, 
            const ITexture* t20, const ITexture* t21, const ITexture* t22);
  //give 1 texture, the other 8 are assumed to have successive memory locations
  void makeTextured(const ITexture** t00);
};

#define DEFAULTPANEL BackPanel(1)

/*
a BackRule is a collection of 3 textures (the center one tileable) that
can form a horizontal or vertical line
*/
class BackRule
{
  public:
  /*
  if sides are disabled, the center texture is used for the whole thing
  */
  bool enableSides;
  
  Direction direction;
   
  //the 3 different textures making up the line
  const ITexture* t0; //left or top corner
  const ITexture* t1; //tilable center
  const ITexture* t2; //right or bottom corner

  //draw the line at x, y position, with given length
  void draw(IDrawer2D& drawer, int x, int y, int length, const ColorRGB& colorMod=RGB_White) const;
  
  //constructors
  BackRule();
  
  //give 3 separate textures
  void makeHorizontal(const ITexture* t0, ITexture* t1, ITexture* t2);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeHorizontal1(const ITexture* t0);
       
  //give 3 separate textures
  void makeVertical(const ITexture* t0, ITexture* t1, ITexture* t2);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeVertical1(const ITexture* t0);
};

#define DEFAULTHRULE BackRule(1)
#define DEFAULTVRULE BackRule(2)

extern BackPanel builtInPanel[8];
extern BackRule builtInRule[2];

struct GuiSet //GuiSet is a bit of a LEGACY lpi concept. Currently it's just used as the container of the internal built in GUI textures.
{
  const ITexture* windowTextures[9];
  const ITexture* buttonTextures[9];
  const ITexture* buttonOverTextures[9];
  const ITexture* buttonDownTextures[9];
  const ITexture* arrowN; //for scrollbars, arrow of droplist, ...
  const ITexture* arrowE;
  const ITexture* arrowS;
  const ITexture* arrowW;
  const ITexture* scroller; //for scrollbars
  const ITexture* scrollbarBackground;
  const ITexture* scrollBarPairCorner; //the cornerpiece of a scrollbarpair
  const ITexture* emptyButton;
  const ITexture* roundButton;
  const ITexture* slider; //the button of a slider, the simplified scrollbar (the slider is what the scroller is to scrollbars)
  const ITexture* smallSliderH; //button for small horizontal slider
  const ITexture* checkBox[2];
  const ITexture* smallCheckBox[2];
  const ITexture* bullet[2];
  const ITexture* hline[3];
  const ITexture* vline[3];
  const ITexture* smiley;
  const ITexture* windowTop[3];
  const ITexture* closeButton; //from a window
  const ITexture* resizer; //the resizer at bottom right of a window
  const ITexture* whiteButton; //can easily be given any color with color mods
  const ITexture* whiteRoundButton;
  const ITexture* crossHair; //for example to indicate a 2D location on a map, color picker, ...
  const ITexture* tabUnSelected[9];
  const ITexture* tabSelected[9];
  const ITexture* borderPanelTextures[9];
  const ITexture* whitePanelTextures[9];
  
  
  const BackPanel* windowPanel;
  const BackPanel* buttonPanel;
  const BackPanel* buttonOverPanel;
  const BackPanel* buttonDownPanel;
  const BackPanel* tabUnSelectedPanel;
  const BackPanel* tabSelectedPanel;
  const BackPanel* borderPanel;
  const BackPanel* whitePanel;
  
  const BackRule* sliderHRule;
  const BackRule* sliderVRule;
  const BackRule* smallSliderHRule;
  const BackRule* windowTopRule;
  
  ColorRGB mainColor; //if the mouse is not over or down a button
  ColorRGB mouseOverColor; //this isn't for panel buttons, but for image buttons like the arrows of a scrollbar, ...
  ColorRGB mouseDownColor; //this isn't for panel buttons, but for image buttons like the arrows of a scrollbar, ...
  
  Font panelButtonFont[3];
  Font textButtonFont[3];
  Font windowTopFont;
};

class GUIPartDrawerInternal : public IGUIPartDrawer
{
    IDrawer2D* drawer;
    ITextDrawer* textdrawer;
    GuiSet* guiset;
    
    std::vector<ITexture*> builtInTexture;
    BackPanel builtInPanel[8];
    BackRule builtInRule[3];
    GuiSet builtInGuiSet;
    
    void initBuiltInGui(const ITextureFactory& factory);
    void initBuiltInGui(const ITextureFactory& factory, const std::string& xmlgui);
    void initBuiltInGuiTexturesSmall64(const ITextureFactory& factory, const std::string& png_base64);
    void initBuiltInGuiIconsSmall64(const ITextureFactory& factory, const std::string& png_base64);
    void initBuiltInGuiTexturesSmall(const ITextureFactory& factory, const std::vector<unsigned char>& png);
    void initBuiltInGuiIconsSmall(const ITextureFactory& factory, const std::vector<unsigned char>& png);
    void initBuiltInGuiTexturesSmall(const ITextureFactory& factory, const std::string& png_file_name);
    void initBuiltInGuiIconsSmall(const ITextureFactory& factory, const std::string& png_file_name);

    
  public:
  
    GUIPartDrawerInternal(const ITextureFactory& factory, IDrawer2D* drawer, ITextDrawer* textdrawer);
    ~GUIPartDrawerInternal();
    
    virtual void drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual void drawGUIPartColor(GUIPart part, const ColorRGB& color, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual void drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod = GPM_Default);
    virtual size_t getGUIPartSizeX(GUIPart part) const;
    virtual size_t getGUIPartSizeY(GUIPart part) const;

};


} // namespace gui
} // namespace lpi
