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


#pragma once

#include "lpi_gui_drawer.h"
#include "lpi_draw2dgl.h"
#include "lpi_gui_base.h"
#include "lpi_color.h"
#include "lpi_texture.h"
#include "lpi_text_drawer_gl.h"
#include "lpi_input_sdl.h"
#include "lpi_guipartdraw_int.h"

namespace lpi
{
namespace gui
{

class GUIDrawerGL : public AGUIDrawer
{
  private:
    InputSDL input;
    Drawer2DGL drawer;
    TextDrawerGL textdrawer;
    GUIPartDrawerInternal guidrawer;
  
  protected:
  
    virtual IDrawer2D& getDrawer() { return drawer; }
    virtual const IDrawer2D& getDrawer() const { return drawer; }
    virtual ITextDrawer& getTextDrawer() { return textdrawer; }
    virtual const ITextDrawer& getTextDrawer() const { return textdrawer; }
    virtual IGUIPartDrawer& getGUIPartDrawer() { return guidrawer; }
    virtual const IGUIPartDrawer& getGUIPartDrawer() const { return guidrawer; }
    
  public:
    GUIDrawerGL(ScreenGL* screen);
    
    GuiSet& getInternal() { return guidrawer.getGUISet(); } //can be used to set font and such
    
    void loadGUITextures(const std::vector<unsigned char>& png); //optional. Built in textures are already loaded. This is for custom ones from external PNG. Requires PNG file exactly matching the built-in GUI.

    //input
    virtual IInput& getInput();
    
    ScreenGL* getScreen() { return drawer.getScreen(); }
};


} //namespace gui
} //namespace lpi

